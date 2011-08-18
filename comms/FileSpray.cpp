#include "StdAfx.h"
#include "FileSpray.h"
#include "dfuworkunit.h"

#include "cache.h"
#include "SoapUtil.h"
#include "clib.h"
#include "logger.h"
#include "global.h"
#include "thread.h"

#if _COMMS_VER < 51100
#else
using namespace FileSpray;
#endif

namespace WsFileSpray
{
IWorkunit * CreateIWorkunit(const std::_tstring & url, DFUWorkunit *data);
IFileSprayPath * CreateIFileSprayPath(IFileSpray * server, const std::_tstring & ip, const std::_tstring & path, const FileSpray::PhysicalFileStruct * data);

static void DoMonitorState(const std::_tstring wuid)
{
	//TODO Check we are not monitoring already and degrade sleep a bit more...
	CComPtr<IFileSpray> server = AttachFileSpray();
	for (CComPtr<IWorkunit> wu = server->GetWorkunit(wuid); !wu->IsComplete(); wu = server->GetWorkunit(wuid))
	{
		Sleep(3000);
	}
}

class FileVisitor : public IVisitor
{
protected:
	IFileSprayPathVector * m_results;

public:
	FileVisitor(IFileSprayPathVector * results) : m_results(results)
	{
	}

	void StartFolder(IFileSprayPath * path)
	{
	}

	void EndFolder(IFileSprayPath * path)
	{
	}

	void FileVistited(IFileSprayPath * path)
	{
		m_results->push_back(path);
	}
};

class CFileSpray : public IFileSpray, public clib::CLockableUnknown
{
protected:
	CString m_label;
	CString m_url;
	std::_tstring m_build;
	std::_tstring m_unknown;

public:
	IMPLEMENT_CLOCKABLEUNKNOWN

	CFileSpray(const CString & url, const CString & label) : m_label(label), m_url(url)
	{
		m_unknown = _T("???");
	}

	~CFileSpray()
	{
	}

	const TCHAR * GetID() const
	{
		return m_url;
	}

	const TCHAR * GetCacheID() const
	{
		return m_url;
	}

	const TCHAR * GetLabel() const
	{
		return m_label;
	}

	const TCHAR * GetUrl() const
	{
		return m_url;
	}

	class CFileStructCompare
	{
	public:
		bool operator ()(const StlLinked<IFileSprayPath> & l, const StlLinked<IFileSprayPath> & r)
		{
			if (l->IsDir() != r->IsDir())
			{
				return l->IsDir() > r->IsDir();
			}
			CString lhs = l->GetName();
			return lhs.CompareNoCase(r->GetName()) > 0 ? false : true;
		}
	};

	unsigned GetFiles(const std::_tstring & _NetAddr, const std::_tstring & _Path, const std::_tstring & _Mask, IFileSprayPathVector * results)
	{
		FileVisitor visitor(results);
		WalkFiles(_NetAddr, _Path, _Mask, &visitor);
		CFileStructCompare cmp;
		std::sort(results->begin(), results->end(), cmp);
		return results->size();
	}

	void WalkFiles(const std::_tstring & _NetAddr, const std::_tstring & _Path, const std::_tstring & _Mask, IVisitor * visitor)
	{
#if _COMMS_VER < 51100 
		return 0;
#else
		CComInitialize com;
		CFileSprayT<CCustomActionSecureSoapSocketClient> server;
		server.SetUrl(m_url);

		CComBSTR NetAddr = CT2W(_NetAddr.c_str(), CP_UTF8);
		CComBSTR Path = CT2W(_Path.c_str(), CP_UTF8);
		ESP_EXCEPTION_LOG(EspException);
		unsigned count = 0;

#if _COMMS_VER < 52501 
		CStructArrayOut<BSTR> response;
		if (server.FileList(&NetAddr, &Path, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), response.GetArrayAddress(), response.GetCountAddress()) == S_OK)
#elif _COMMS_VER < 54700 
		CStructArrayOut<PhysicalFileStruct> response;
		if (server.FileList(&NetAddr, &Path, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), response.GetArrayAddress(), response.GetCountAddress()) == S_OK)
#elif _COMMS_VER < 57600 
		StlLinked<IFileSprayPath> folder = CreateIFileSprayPath(_NetAddr, _Path);
		visitor->StartFolder(folder);
		CComBSTR Mask = CT2W(_Mask.c_str(), CP_UTF8);
		CStructArrayOut<PhysicalFileStruct> response;
		if (server.FileList(&NetAddr, &Path, &Mask, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), response.GetArrayAddress(), response.GetCountAddress()) == S_OK)
#elif _COMMS_VER < 60000 
		StlLinked<IFileSprayPath> folder = CreateIFileSprayPath(_NetAddr, _Path);
		visitor->StartFolder(folder);
		CComBSTR Mask = CT2W(_Mask.c_str(), CP_UTF8);
		CComBSTR OS = _T("");
		CStructArrayOut<PhysicalFileStruct> response;
		if (server.FileList(&NetAddr, &Path, &Mask, &OS, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), response.GetArrayAddress(), response.GetCountAddress()) == S_OK)
#else
		StlLinked<IFileSprayPath> folder = CreateIFileSprayPath(_NetAddr, _Path);
		visitor->StartFolder(folder);
		CComBSTR Mask = CT2W(_Mask.c_str(), CP_UTF8);
		CComBSTR OS = _T("");
        bool DirectoryOnly = false;
		CStructArrayOut<PhysicalFileStruct> response;
		if (server.FileList(&NetAddr, &Path, &Mask, &OS, &DirectoryOnly, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), response.GetArrayAddress(), response.GetCountAddress()) == S_OK)
#endif
		{
			count = response.GetCount();
			for(unsigned i = 0; i < count; ++i)
			{
				StlLinked<IFileSprayPath> path = CreateIFileSprayPath(this, _NetAddr, _Path, response.GetItem(i));
				visitor->FileVistited(path);
				path->Delete();
			}
		}
		else
			_DBGLOG(m_url, LEVEL_WARNING, server.GetClientError());
		visitor->EndFolder(folder);
#endif
	}

	void CreateDFUWorkunit() const
	{
		CComInitialize com;
		CFileSprayT<CCustomActionSecureSoapSocketClient> server;
		server.SetUrl(m_url);

		ESP_EXCEPTION_LOG(EspException);
		DFUWorkunit wu;
		if (server.CreateDFUWorkunit(exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &wu) == S_OK)
		{
		}
		else
			_DBGLOG(m_url, LEVEL_WARNING, server.GetClientError());
	}

	void UpdateDFUWorkunit(DFUWorkunit &wu) const
	{
		CComInitialize com;
		CFileSprayT<CCustomActionSecureSoapSocketClient> server;
		server.SetUrl(m_url);

		ESP_EXCEPTION_LOG(EspException);
#if _COMMS_VER < 55800 
		if (server.UpdateDFUWorkunit(wu, exceptions.GetArrayAddress(), exceptions.GetCountAddress()) == S_OK)
#else
		CComBSTR clusterOrig;
		CComBSTR jobNameOrig;
		bool isProtected;
		int stateOrig;
		if (server.UpdateDFUWorkunit(wu, clusterOrig, jobNameOrig, isProtected, stateOrig, exceptions.GetArrayAddress(), exceptions.GetCountAddress()) == S_OK)
#endif
		{
		}
		else
			_DBGLOG(m_url, LEVEL_WARNING, server.GetClientError());
	}

	unsigned int GetDFUProgress(const std::_tstring & _wuid) const
	{
		CComInitialize com;
		CFileSprayT<CCustomActionSecureSoapSocketClient> server;
		server.SetUrl(m_url);

		CComBSTR wuid = CT2W(_wuid.c_str(), CP_UTF8);
		ESP_EXCEPTION_LOG(EspException);
		CComBSTR wuidOut;
		int PercentDone;
		int SecsLeft;
		int KbPerSecAve;
		int KbPerSec;
		int SlavesDone;
		CComBSTR TimeTaken;
		CComBSTR ProgressMessage;
		CComBSTR SummaryMessage;
		CComBSTR State;
		if (server.GetDFUProgress(&wuid, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &PercentDone, &SecsLeft, &KbPerSecAve, &KbPerSec, &SlavesDone, &TimeTaken, &ProgressMessage, &SummaryMessage, &State) == S_OK)
		{
		}
		else
			_DBGLOG(m_url, LEVEL_WARNING, server.GetClientError());
		return PercentDone;
	}

	IWorkunit * SprayVariable(const std::_tstring & _sourceIP, const std::_tstring & _sourcePath, const std::_tstring & _destGroup, const std::_tstring & _destLogicalName) const
	{
		CComInitialize com;
		CFileSprayT<CCustomActionSecureSoapSocketClient> server;
		server.SetUrl(m_url);

		CComBSTR sourceIP = CT2W(_sourceIP.c_str(), CP_UTF8);
		CComBSTR sourcePath = CT2W(_sourcePath.c_str(), CP_UTF8);
		ATLSOAP_BLOB srcxml = {0, NULL};
		int sourceMaxRecordSize = 8192;
		int sourceFormat = 1;
		CComBSTR sourceCsvSeparate = _T("\\,");
		CComBSTR sourceCsvTerminate = _T("\r\n,\n");
		CComBSTR sourceCsvQuote = _T("'");
		CComBSTR sourceRowTag = _T("");
		CComBSTR destGroup = CT2W(_destGroup.c_str(), CP_UTF8);
		CComBSTR destLogicalName = CT2W(_destLogicalName.c_str(), CP_UTF8);
		bool overwrite = true;
		bool replicate = false;
		int maxConnections = 2;
		int throttle = 0;
		CComBSTR prefix = _T("");
		bool nosplit = false;
		bool norecover = false;
		bool compress = false;

		ESP_EXCEPTION_LOG(EspException);
		CComBSTR wuid;

#if _COMMS_VER < 57600 
		if (server.SprayVariable(sourceIP, sourcePath, srcxml, sourceMaxRecordSize, sourceFormat, sourceCsvSeparate, sourceCsvTerminate, sourceCsvQuote, sourceRowTag, destGroup, destLogicalName, overwrite, replicate, maxConnections, throttle, prefix, nosplit, norecover, compress, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &wuid) == S_OK)
#elif _COMMS_VER < 57700
		CComBSTR destGroupRoxie = _T("");
        int ReplicateOffset = 0;
		bool Wrap = false;
		bool Multicopy = false;
		bool Farmers = false;
		CComBSTR SourceDiffKeyName = _T("");
		CComBSTR DestDiffKeyName = _T("");
		if (server.SprayVariable(sourceIP, sourcePath, srcxml, sourceMaxRecordSize, sourceFormat, sourceCsvSeparate, sourceCsvTerminate, sourceCsvQuote, sourceRowTag, destGroup, destGroupRoxie, destLogicalName, overwrite, replicate, ReplicateOffset, maxConnections, throttle, prefix, nosplit, norecover, compress, Wrap, Multicopy, Farmers, SourceDiffKeyName, DestDiffKeyName, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &wuid) == S_OK)
#elif _COMMS_VER < 60000
        int ReplicateOffset = 0;
		if (server.SprayVariable(sourceIP, sourcePath, srcxml, sourceMaxRecordSize, sourceFormat, sourceCsvSeparate, sourceCsvTerminate, sourceCsvQuote, sourceRowTag, destGroup, destLogicalName, overwrite, replicate, ReplicateOffset, maxConnections, throttle, prefix, nosplit, norecover, compress, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &wuid) == S_OK)
#elif _COMMS_VER < 6280
        int ReplicateOffset = 0;
		int transferBufferSize = 0;
		bool push = false;
		bool pull = false;
		if (server.SprayVariable(	sourceIP, sourcePath, srcxml, sourceMaxRecordSize, sourceFormat, sourceCsvSeparate, sourceCsvTerminate, sourceCsvQuote, 
									sourceRowTag, destGroup, destLogicalName, overwrite, replicate, ReplicateOffset, maxConnections, throttle, transferBufferSize, prefix, 
									nosplit, norecover, compress, push, pull, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &wuid) == S_OK)
#else
        int ReplicateOffset = 0;
		int transferBufferSize = 0;
		bool push = false;
		bool pull = false;
		CComBSTR encrypt;
		CComBSTR decrypt;
		if (server.SprayVariable(	sourceIP, sourcePath, srcxml, sourceMaxRecordSize, sourceFormat, sourceCsvSeparate, sourceCsvTerminate, sourceCsvQuote, 
									sourceRowTag, destGroup, destLogicalName, overwrite, replicate, ReplicateOffset, maxConnections, throttle, transferBufferSize, prefix, 
									nosplit, norecover, compress, push, pull, encrypt, decrypt, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &wuid) == S_OK)
#endif
		{
			if (wuid)
			{
				return GetWorkunit(std::_tstring(CW2T(wuid, CP_UTF8)));
			}
		}
		else
			_DBGLOG(m_url, LEVEL_WARNING, server.GetClientError());
		return NULL;
	}

	IWorkunit * SprayFixed(const std::_tstring & _sourceIP, const std::_tstring & _sourcePath, int _sourceRecordSize, const std::_tstring & _destGroup, const std::_tstring & _destLogicalName, const std::_tstring & _prefix, bool _nosplit) const
	{
		CComInitialize com;
		CFileSprayT<CCustomActionSecureSoapSocketClient> server;
		server.SetUrl(m_url);

		CComBSTR sourceIP = CT2W(_sourceIP.c_str(), CP_UTF8);
		CComBSTR sourcePath = CT2W(_sourcePath.c_str(), CP_UTF8);
		ATLSOAP_BLOB srcxml = {0, NULL};
        int sourceRecordSize = _sourceRecordSize;
		CComBSTR destGroup = CT2W(_destGroup.c_str(), CP_UTF8);
		CComBSTR destLogicalName = CT2W(_destLogicalName.c_str(), CP_UTF8);
		bool overwrite = true;
		bool replicate = false;
		int maxConnections = 2;
		int throttle = 0;
		CComBSTR prefix = CT2W(_prefix.c_str(), CP_UTF8);;
		bool nosplit = _nosplit;
		bool norecover = false;
		bool compress = false;

		ESP_EXCEPTION_LOG(EspException);
		CComBSTR wuid;

#if _COMMS_VER < 57600 
		if (server.SprayFixed(sourceIP, sourcePath, srcxml, sourceRecordSize, destGroup, destLogicalName, overwrite, replicate, maxConnections, throttle, prefix, nosplit, norecover, compress, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &wuid) == S_OK)
#elif _COMMS_VER < 57700
		CComBSTR destGroupRoxie = _T("");
        int ReplicateOffset = 0;
		bool Wrap = false;
		bool Multicopy = false;
		bool Farmers = false;
		CComBSTR SourceDiffKeyName = _T("");
		CComBSTR DestDiffKeyName = _T("");
		if (server.SprayFixed(sourceIP, sourcePath, srcxml, sourceRecordSize, destGroup, destGroupRoxie, destLogicalName, overwrite, replicate, ReplicateOffset, maxConnections, throttle, prefix, nosplit, norecover, compress, Wrap, Multicopy, Farmers, SourceDiffKeyName, DestDiffKeyName, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &wuid) == S_OK)
#elif _COMMS_VER < 60000
        int ReplicateOffset = 0;
		if (server.SprayFixed(	sourceIP, sourcePath, srcxml, sourceRecordSize, destGroup, destLogicalName, overwrite, replicate, ReplicateOffset, 
								maxConnections, throttle, prefix, nosplit, norecover, compress, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &wuid) == S_OK)
#elif _COMMS_VER < 62808
        int ReplicateOffset = 0;
		int transferBufferSize = 0;
		bool push = false;
		bool pull = false;
		if (server.SprayFixed(	sourceIP, sourcePath, srcxml, sourceRecordSize, destGroup, destLogicalName, overwrite, replicate, ReplicateOffset, 
								maxConnections, throttle, transferBufferSize, prefix, nosplit, norecover, compress, push, pull, 
								exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &wuid) == S_OK)
#else
        int ReplicateOffset = 0;
		int transferBufferSize = 0;
		bool push = false;
		bool pull = false;
		CComBSTR encrypt;
		CComBSTR decrypt;
		if (server.SprayFixed(	sourceIP, sourcePath, srcxml, sourceRecordSize, destGroup, destLogicalName, overwrite, replicate, ReplicateOffset, 
								maxConnections, throttle, transferBufferSize, prefix, nosplit, norecover, compress, push, pull, encrypt, decrypt,  
								exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &wuid) == S_OK)
#endif
		{
			return GetWorkunit(std::_tstring(CW2T(wuid, CP_UTF8)));
		}
		else
			_DBGLOG(m_url, LEVEL_WARNING, server.GetClientError());
		return NULL;
	}

	IWorkunit * GetWorkunit(const std::_tstring & _wuid) const
	{
		CComInitialize com;
		CFileSprayT<CCustomActionSecureSoapSocketClient> server;
		server.SetUrl(m_url);

		CComBSTR wuid = CT2W(_wuid.c_str(), CP_UTF8);
		ESP_EXCEPTION_LOG(EspException);
		DFUWorkunit result;
		int autoRefresh = 0;
		CComPtr<IWorkunit> retVal;

		if (server.GetDFUWorkunit(wuid, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &result, &autoRefresh) == S_OK)
		{
			if (result.ID)
			{
				retVal = CreateIWorkunit(static_cast<const TCHAR *>(m_url), &result);
			}
			AtlCleanupValueEx(&result, &crtHeap);
		}
		else
			_DBGLOG(m_url, LEVEL_WARNING, server.GetClientError());
		return retVal;
	}

	IFileSprayPath * GetFolder(const std::_tstring & _NetAddr, const std::_tstring & _Path)
	{
		FileSpray::PhysicalFileStruct  pfs;
		pfs.name=L""; // not really correct but gets us IFileSprayPath
		pfs.modifiedtime=L"";
		IFileSprayPath * retVal=CreateIFileSprayPath(this, _NetAddr, _Path, &pfs);
		return retVal;
	}

	void MonitorState(const std::_tstring & wuid) const
	{
		clib::thread run(__FUNCTION__, boost::bind(&DoMonitorState, wuid));
	}
};

IFileSpray * AttachFileSpray(const CString & url, const CString & label)
{
	static CacheT<std::_tstring, CFileSpray> FileSprayCache;
	return FileSprayCache.Get(new CFileSpray(url, label));
}

COMMS_API IFileSpray * AttachFileSpray()
{
	return AttachFileSpray(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_FILESPRAY) ,_T("FileSpray"));
}

}

