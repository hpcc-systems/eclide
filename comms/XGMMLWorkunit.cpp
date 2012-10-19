#include "StdAfx.h"

#include "workunit.h"
#include "dali.h"
#include "SoapUtil.h"
#include "clib.h"
#include "cache.h"
#include "logger.h"
#include "result.h"
#include "graph.h"
#include "unicodefile.h"

#if _COMMS_VER < 68200
using namespace WsWorkunits;
using namespace WsSMC;
#elif _COMMS_VER < 700000
#else
using namespace WsECLWorkunits;
using namespace WsEnvironment;
typedef ECLWUResult ECLResult;
typedef ECLWUDetail ECLWorkunit;
typedef ECLWUException ECLException;
typedef ECLWUApplicationValue ApplicationValue;
typedef ECLWUDebugValue DebugValue;
typedef ActiveOrQueuedWorkunit ActiveWorkunit;
#endif

namespace Dali
{
IGraph * CreateGraph(const CString& url, const CString & wuid, const std::_tstring & xgmml);

class CXGMMLWorkunit : public IWorkunit, public clib::CLockableUnknown
{
protected:
	CString m_id;
	CString m_url;
	CString m_daliLabel;
	CString m_label;
	CString m_jobName;
	CString m_dateTime;
	CString m_owner;
	CString m_queue;
	CString m_cluster;
	WUState m_stateID;
	CString m_state;
	CString m_wuid;
	CString m_ecl;
	CString m_snapShot;
	CString m_schedule;
	bool m_protected;
	bool m_followup;
	bool m_archived;
	int m_resultLimit;
	int m_maxRuntime;
	CString m_debugString;
	bool m_archiveCpp;
	bool m_noCommonPrivateAttributes;
	bool m_resultsLoaded;

	IResultMap m_resultsMap;
	IResultVector m_resultsVector;
	IGraphMap m_graphsMap;
	IGraphVector m_graphsVector;
	ILocalFileVector m_localFiles;
	CEclExceptionVector m_exceptions;
	workunit_signal_type on_refresh;
	int m_filesToUploadCount;

public:
	IMPLEMENT_CLOCKABLEUNKNOWN;

	CXGMMLWorkunit(const CString & url, const CString daliLabel, const CString & wuid) : m_url(url), m_daliLabel(daliLabel), m_wuid(wuid)
	{
		m_id = m_url + _T("/") + m_daliLabel + _T("/") + m_wuid;
		m_resultLimit = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_WORKUNIT_RESULTLIMIT);
		m_maxRuntime = 0;
		m_stateID = WUStateUnknown;
		m_protected = false;
		m_followup = false;
		m_resultsLoaded = false;
		m_archived = false;
		m_filesToUploadCount = 0;
	}
	virtual ~CXGMMLWorkunit()
	{
	}
	const TCHAR *GetID() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_id;
	}
	const TCHAR * GetCacheID() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_id;
	}
	const TCHAR *GetWuid() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_wuid;
	}
	const TCHAR *GetDateTime() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_dateTime;
	}
	const TCHAR *GetScheduledDateTime() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_schedule;
	}
	const TCHAR *GetLabel() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_label;
	}
	const TCHAR *GetOwner() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_owner;
	}
	const TCHAR *GetQueue() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_queue;
	}
	const TCHAR *GetCluster() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_cluster;
	}
	WUState GetState() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_stateID;
	}
	void SetState(WUState state)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		m_stateID = state;
	}
	const TCHAR *GetStateLabel() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_state;
	}
	const TCHAR *GetEcl() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_ecl;
	}
	const TCHAR *GetSnapshot() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_snapShot;
	}
	bool GetProtected() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_protected;
	}
	bool GetArchived() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_archived;
	}
	bool GetFollowup() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_followup;
	}
	bool IsComplete() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		switch (m_stateID)
		{
		case WUStateCompleted:
		case WUStateFailed:
		case WUStateAborted:
		case WUStateArchived:
			return true;
		}
		return false;
	}
	bool IsDebugging() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		switch (m_stateID)
		{
		case WUStateDebugRunning:
		case WUStateDebugPaused:
			return true;
		}
		return false;
	}
	int GetResultLimit() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_resultLimit;
	}
	int GetMaxRuntime() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_maxRuntime;
	}
	const TCHAR *GetDebugString() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_debugString;
	}
	bool GetArchiveCpp() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_archiveCpp;
	}
	bool GetNoCommonPrivateAttributes() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_noCommonPrivateAttributes;
	}
#if _COMMS_VER < 51300
	void SetResultLimit(int resultLimit) 
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		m_resultLimit = resultLimit;
	}
#endif
	void LoadResults() const
	{
		if (!m_resultsLoaded)
		{
			StlLinked<IDali> server;
			server = AttachDali(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT), _T("Dali"));
			CComPtr<IWorkunit> forceRefresh = server->GetWorkunit(GetWuid(), true);  //no Broadcast otherwise we will deadlock.
		}
	}
	unsigned int GetResultCount() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		LoadResults();
		return m_resultsMap.size();
	}
	std::pair<IResultVector::const_iterator, IResultVector::const_iterator> GetResultIterator() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		LoadResults();
		return std::make_pair(m_resultsVector.begin(), m_resultsVector.end());
	}
	IResult * GetResult(const std::_tstring & id) const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		LoadResults();
		IResultMap::const_iterator itr = m_resultsMap.find(id);
		if (itr != m_resultsMap.end())
		{
			return itr->second;
		}
		return NULL;
	}
	void Refresh(bool noBroadcast)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
	}
	void RefreshGraph() 
	{
	}
	unsigned int GetGraphCount() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return 1;
	}
	unsigned int GetGraphNames(StdStringVector & results) const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		results.push_back(_T("Graph1"));
		return results.size();
	}
	IGraph * GetGraph(const std::_tstring & graphName) const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		std::_tstring xgmml;
		GetAllGraphXGMML(xgmml);
		return CreateGraph(m_url, m_wuid, xgmml);
	}
	const TCHAR * GetGraphXGMML(const std::_tstring & graphName, std::_tstring & xgmml) const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return GetAllGraphXGMML(xgmml);
	}
	const TCHAR * GetAllGraphXGMML(std::_tstring & xgmml) const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		std::_tstring xml;
		CUnicodeFile f;
		if (f.Open(m_url))
		{
			f.Read(xml);
			f.Close();
		}
		xgmml = _T("<?xml version=\"1.0\" encoding=\"utf-8\"?><GJS SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:control=\"http://www.w3.org/1999/XMLSchema\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">") + xml + _T("</GJS>");
		return xgmml.c_str();
	}
	WUVisualState GetVisualState() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return WUVisualState_completed;
	}
	void RefreshGraphTiming() 
	{
	}
	int GetGraphTimingCount() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return 0;
	}
	const CGraphTiming * GetGraphTiming(unsigned item) const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return NULL;
	}
	int GetGraphTimings(CGraphTimingVector & results) const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return results.size();
	}
	int GetExceptionCount() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_exceptions.size();
	}
	const CEclException * GetException(unsigned item) const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		ATLASSERT(item >= 0 && item < m_exceptions.size());
		return m_exceptions[item].get();
	}
	const CEclException * GetCurrException(unsigned lineNo) const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		for(CEclExceptionVector::size_type i = 0; i < m_exceptions.size(); ++i)
		{
			unsigned int errorLine = (unsigned int)m_exceptions[i]->m_lineNo;
			if (errorLine == lineNo)
				return m_exceptions[i].get();
		}
		return NULL;
	}
	const CEclException * GetNextException(unsigned lineNo) const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		int found = -1;
		unsigned exceptionCount = m_exceptions.size();
		for(unsigned i = 0; i < exceptionCount; ++i)
		{
			unsigned errorLine = m_exceptions[exceptionCount - i - 1]->m_lineNo;
			if (errorLine <= lineNo)
				break;
			found = exceptionCount - i - 1;
		}
		return found == -1 ? NULL : m_exceptions[found].get();
	}
	const CEclException * GetPrevException(unsigned lineNo) const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		int found = -1;
		for(unsigned i = 0; i < m_exceptions.size(); ++i)
		{
			unsigned errorLine = m_exceptions[i]->m_lineNo;
			if (errorLine >= lineNo)
				break;
			found = i;
		}
		return found == -1 ? NULL : m_exceptions[found].get();
	}
	int GetExceptions(CEclExceptionVector & results) const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		for(unsigned i = 0; i < m_exceptions.size(); ++i)
		{
			results.push_back(m_exceptions[i]);
		}
		return results.size();
	}
	void GetLocalFilesToUpload()
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		if (m_filesToUploadCount == 0)
		{
			StlLinked<IDali> server;
			server = AttachDali(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT), _T("Dali"));
			m_localFiles.clear();
			if (server->GetLocalFiles(GetWuid(), m_localFiles) > 0)
				m_filesToUploadCount = m_localFiles.size();
			else
				m_filesToUploadCount = -1;

			UploadFiles();//TODO Should be moved to a GUI thread.
		}
	}
	void UploadFiles()
	{
		for(ILocalFileVector::iterator itr = m_localFiles.begin(); itr != m_localFiles.end(); ++itr)
		{
#define TEXTFILE_ONLY
#ifdef TEXTFILE_ONLY
			std::_tstring rawData;
			CUnicodeFile file;
			if (file.Open(itr->get()->GetSource()))
			{
				file.Read(rawData);
			}
		}
#else
#define BLOCKSIZE 32768
			std::vector<char> rawData;
			CAtlFile file;
			if (file.Create(itr->get()->GetSource(), GENERIC_READ, 0, OPEN_EXISTING) == S_OK)
			{
				ULONGLONG size, pos;
				if (file.GetSize(size) == S_OK && file.GetPosition(pos) == S_OK)
				{
					size -= pos;
					DWORD bytesRead;
					rawData.resize(static_cast<std::_tstring::size_type>(size));
					for(int i = 0; i < size; i += BLOCKSIZE)
					{
						unsigned int readSize = rawData.size() - i;
						if (readSize > BLOCKSIZE)
							readSize = BLOCKSIZE;
						file.Read(&rawData[0] + i, readSize, bytesRead);
						ATLASSERT(bytesRead == readSize);
					}
				}
			}
		}
#endif
	}
	void Update(const CString filePath)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		//CUnicodeFile f;
		//if (f.Open(filePath))
		//{
		//	CString xml;
		//	f.Read(xml);
		//	f.Close();
		//	CComBSTR bstrXML = _T("<?xml version=\"1.0\" encoding=\"utf-8\"?><GJS SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:control=\"http://www.w3.org/1999/XMLSchema\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">") + xml + _T("</GJS>");

		//	CComInitialize com;
		//	CComPtr<IXMLDOMDocument> pDoc;
		//	pDoc.CoCreateInstance(CLSID_DOMDocument30);
		//	VARIANT_BOOL r;
		//	pDoc->put_async(VARIANT_FALSE);
		//	pDoc->loadXML(bstrXML, &r);
		//	if (r != VARIANT_TRUE)
		//	{
		//		IXMLDOMParseError *pXMLErr=NULL;
		//		CComBSTR bstr = NULL;

		//		pDoc->get_parseError(&pXMLErr);
		//		pXMLErr->get_reason(&bstr);
		//		_DBGLOG(m_url, LEVEL_WARNING, (boost::_tformat(_T("Restoring Workspace:  %1% %2%")) % std::_tstring(filePath) % std::_tstring(CW2T(bstr))).str().c_str());
		//		return;
		//	}

		//	CComPtr<IXMLDOMNodeList> graphs;
		//	HRESULT hr = pDoc->selectNodes(CComBSTR(_T("//Graph")), &graphs);
		//	if (hr != S_OK)
		//		return;

		//	long length;
		//	graphs->get_length(&length);
		//	for(long i = 0; i < length;  ++i)
		//	{
		//		CComPtr<IXMLDOMNode> graph;
		//		hr = graphs->get_item(i, &graph);
		//		if (hr != S_OK)
		//			return;

		//		CComPtr<IXMLDOMNamedNodeMap> attrs;
		//		graph->get_attributes(&attrs);

		//		long attrLength;
		//		graphs->get_length(&attrLength);

		//		CComBSTR bstrName = _T("id");
		//		CComVariant idValue;
		//		CComPtr<IXMLDOMNode> idNode;
		//		attrs->getNamedItem(bstrName, &idNode);
		//		if (idNode)
		//		{
		//			idNode->get_nodeValue(&idValue);
		//		}

		//		CComBSTR bstrXML;
		//		graph->get_xml(&bstrXML);
		//		std::_tstring xml2 = CW2T(bstrXML);
		//		boost::algorithm::replace_all(xml2, "\n", "&#10;");
		//		bstrXML = CT2W(xml2.c_str());

		//		ECLGraphEx graphEx = { 0 };

		//		CComBSTR bstrLabel = _T("");
		//		CComBSTR bstrType = _T("");

		//		idValue.ChangeType(VT_BSTR);
		//		graphEx.Name = idValue.bstrVal;
		//		graphEx.Label = bstrLabel;
		//		graphEx.Type = bstrType;
		//		graphEx.Graph = bstrXML;
		//		m_graphsMap[idValue.bstrVal] = CreateGraph(m_url, m_wuid, &graphEx);
		//		m_graphsVector.push_back(m_graphsMap[idValue.bstrVal]);
		//		//CComBSTR name;
		//		//CComVariant value;
		//		//hr = graph->get_nodeName(&name);
		//		//if (hr != S_OK)
		//		//	return false;

		//		//CComPtr<IXMLDOMNode> val;
		//		//if (graph->get_firstChild(&val) == S_OK)
		//		//{
		//		//	hr = val->get_nodeValue(&value);
		//		//	value.ChangeType(VT_BSTR);
		//		//	std::_tstring str = CW2T(value.bstrVal, CP_UTF8);
		//		//}
		//	}
		//}
		proc.unlock();
		Broadcast();
	}

	void Broadcast(WUEvent evt = WUEventUpdated) 
	{
		on_refresh(evt, this);
	}

	void MonitorState() 
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		if (IsComplete())
		{
			proc.unlock();
			Broadcast();
			return;
		}
		StlLinked<IDali> server;
		server = AttachDali(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT), _T("Dali"));
		server->MonitorState(GetWuid());
	}

	void ForceBroadcast(WUEvent evt)
	{
		//  No lock here please...  
		Broadcast(evt);
	}

	boost::signals::connection on_refresh_connect(const workunit_slot_type& s)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return on_refresh.connect(s); 
	}
};

static CacheT<std::_tstring, CXGMMLWorkunit> XGMMLWorkunitCache;

void ClearXGMMLWorkunitCache()
{
	XGMMLWorkunitCache.Clear();
}

void ClearXGMMLWorkunitCache(const std::_tstring & id)
{
	ATLTRACE(_T("Workunit cache before clearing(size=%u)\r\n"), XGMMLWorkunitCache.Size());
	XGMMLWorkunitCache.Clear(id);
}

IWorkunit * CreateXGMMLWorkunit(const CString & filePath, const CString & daliLabel, const CString & wuid)
{
	CXGMMLWorkunit *retVal = XGMMLWorkunitCache.Get(new CXGMMLWorkunit(filePath, daliLabel, wuid));
	retVal->Update(filePath);
	return retVal;
}
}