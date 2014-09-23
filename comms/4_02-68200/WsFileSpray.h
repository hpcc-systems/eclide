//
// sproxy.exe generated file
// do not modify this file
//
// Created: 10/03/2008@12:50:51
//

#pragma once


#if !defined(_WIN32_WINDOWS) && !defined(_WIN32_WINNT) && !defined(_WIN32_WCE)
#pragma message("warning: defining _WIN32_WINDOWS = 0x0410")
#define _WIN32_WINDOWS 0x0410
#endif

#include <atlsoap.h>

namespace FileSpray
{

struct DFUActionResult
{
	BSTR ID;
	BSTR Action;
	BSTR Result;
};

struct PhysicalFileStruct
{
	BSTR name;
	bool isDir;
	__int64 filesize;
	BSTR modifiedtime;
};

struct EspException
{
	BSTR Code;
	BSTR Audience;
	BSTR Source;
	BSTR Message;
};

struct DFUWorkunit
{
	BSTR ID;
	BSTR ClusterName;
	BSTR JobName;
	BSTR Queue;
	BSTR User;
	bool isProtected;
	int Command;
	BSTR CommandMessage;
	int PercentDone;
	int SecsLeft;
	BSTR ProgressMessage;
	BSTR SummaryMessage;
	int State;
	BSTR SourceLogicalName;
	BSTR SourceIP;
	BSTR SourceFilePath;
	BSTR SourceDali;
	int SourceRecordSize;
	int SourceFormat;
	BSTR RowTag;
	int SourceNumParts;
	BSTR SourceDirectory;
	BSTR DestLogicalName;
	BSTR DestGroupName;
	BSTR DestDirectory;
	BSTR DestIP;
	BSTR DestFilePath;
	int DestFormat;
	int DestNumParts;
	int DestRecordSize;
	bool Replicate;
	bool Overwrite;
	bool Compress;
	BSTR TimeStarted;
	BSTR TimeStopped;
	BSTR StateMessage;
	BSTR MonitorEventName;
	bool MonitorSub;
	int MonitorShotLimit;
	BSTR SourceDiffKeyName;
	BSTR DestDiffKeyName;
	bool Archived;
	BSTR encrypt;
	BSTR decrypt;
};

struct DFUException
{
	int Code;
	BSTR Message;
};

template <typename TClient = CSoapSocketClientT<> >
class CFileSprayT : 
	public TClient, 
	public CSoapRootHandler
{
protected:

	const _soapmap ** GetFunctionMap();
	const _soapmap ** GetHeaderMap();
	void * GetHeaderValue();
	const wchar_t * GetNamespaceUri();
	const char * GetServiceName();
	const char * GetNamespaceUriA();
	HRESULT CallFunction(
		void *pvParam, 
		const wchar_t *wszLocalName, int cchLocalName,
		size_t nItem);
	HRESULT GetClientReader(ISAXXMLReader **ppReader);

public:

	HRESULT __stdcall QueryInterface(REFIID riid, void **ppv)
	{
		if (ppv == NULL)
		{
			return E_POINTER;
		}

		*ppv = NULL;

		if (InlineIsEqualGUID(riid, IID_IUnknown) ||
			InlineIsEqualGUID(riid, IID_ISAXContentHandler))
		{
			*ppv = static_cast<ISAXContentHandler *>(this);
			return S_OK;
		}

		return E_NOINTERFACE;
	}

	ULONG __stdcall AddRef()
	{
		return 1;
	}

	ULONG __stdcall Release()
	{
		return 1;
	}

	CFileSprayT(ISAXXMLReader *pReader = NULL)
		:TClient(_T("http://10.173.88.214:8010/FileSpray\?ver_=1.03"))
	{
		SetClient(true);
		SetReader(pReader);
	}
	
	~CFileSprayT()
	{
		Uninitialize();
	}
	
	void Uninitialize()
	{
		UninitializeSOAP();
	}	

	HRESULT GetDFUProgress(
		BSTR* wuid, 
		EspException** Exceptions, int* Exceptions_nSizeIs, 
		int* PercentDone, 
		int* SecsLeft, 
		int* KbPerSecAve, 
		int* KbPerSec, 
		int* SlavesDone, 
		BSTR* TimeTaken, 
		BSTR* ProgressMessage, 
		BSTR* SummaryMessage, 
		BSTR* State
	);

	HRESULT SprayVariable(
		BSTR sourceIP, 
		BSTR sourcePath, 
		ATLSOAP_BLOB srcxml, 
		int sourceMaxRecordSize, 
		int sourceFormat, 
		BSTR sourceCsvSeparate, 
		BSTR sourceCsvTerminate, 
		BSTR sourceCsvQuote, 
		BSTR sourceRowTag, 
		BSTR destGroup, 
		BSTR destLogicalName, 
		bool overwrite, 
		bool replicate, 
		int ReplicateOffset, 
		int maxConnections, 
		int throttle, 
		int transferBufferSize, 
		BSTR prefix, 
		bool nosplit, 
		bool norecover, 
		bool compress, 
		bool push, 
		bool pull, 
		BSTR encrypt, 
		BSTR decrypt, 
		EspException** Exceptions, int* Exceptions_nSizeIs, 
		BSTR* wuid
	);

	HRESULT SprayFixed(
		BSTR sourceIP, 
		BSTR sourcePath, 
		ATLSOAP_BLOB srcxml, 
		int sourceRecordSize, 
		BSTR destGroup, 
		BSTR destLogicalName, 
		bool overwrite, 
		bool replicate, 
		int ReplicateOffset, 
		int maxConnections, 
		int throttle, 
		int transferBufferSize, 
		BSTR prefix, 
		bool nosplit, 
		bool norecover, 
		bool compress, 
		bool push, 
		bool pull, 
		BSTR encrypt, 
		BSTR decrypt, 
		EspException** Exceptions, int* Exceptions_nSizeIs, 
		BSTR* wuid
	);

	HRESULT Despray(
		BSTR sourceLogicalName, 
		BSTR destIP, 
		BSTR destPath, 
		ATLSOAP_BLOB dstxml, 
		bool overwrite, 
		int maxConnections, 
		int throttle, 
		int transferBufferSize, 
		BSTR splitprefix, 
		bool norecover, 
		bool wrap, 
		bool multiCopy, 
		bool SingleConnection, 
		bool compress, 
		BSTR encrypt, 
		BSTR decrypt, 
		EspException** Exceptions, int* Exceptions_nSizeIs, 
		BSTR* wuid
	);

	HRESULT DFUWUSearch(
		EspException** Exceptions, int* Exceptions_nSizeIs, 
		BSTR** ClusterNames, int* ClusterNames_nSizeIs
	);

	HRESULT Copy(
		BSTR sourceLogicalName, 
		BSTR destGroup, 
		BSTR destGroupRoxie, 
		BSTR destLogicalName, 
		BSTR sourceDali, 
		BSTR srcusername, 
		BSTR srcpassword, 
		bool overwrite, 
		bool replicate, 
		int ReplicateOffset, 
		int maxConnections, 
		int throttle, 
		int transferBufferSize, 
		bool nosplit, 
		bool norecover, 
		bool compress, 
		bool Wrap, 
		bool Multicopy, 
		BSTR SourceDiffKeyName, 
		BSTR DestDiffKeyName, 
		bool superCopy, 
		bool push, 
		bool pull, 
		bool ifnewer, 
		BSTR encrypt, 
		BSTR decrypt, 
		EspException** Exceptions, int* Exceptions_nSizeIs, 
		BSTR* result
	);

	HRESULT DFUWUFile(
		BSTR Wuid, 
		BSTR Type, 
		EspException** Exceptions, int* Exceptions_nSizeIs, 
		BSTR* file
	);

	HRESULT DeleteDFUWorkunits(
		BSTR* wuids, int wuids_nSizeIs, 
		EspException** Exceptions, int* Exceptions_nSizeIs
	);

	HRESULT ShowResult(
		BSTR* Result, 
		EspException** Exceptions, int* Exceptions_nSizeIs
	);

	HRESULT GetDFUWorkunit(
		BSTR wuid, 
		EspException** Exceptions, int* Exceptions_nSizeIs, 
		DFUWorkunit* result, 
		int* AutoRefresh
	);

	HRESULT EchoDateTime(
		BSTR dt, 
		BSTR* result
	);

	HRESULT Dkc(
		BSTR srcname, 
		BSTR dstip, 
		BSTR dstfile, 
		bool overwrite, 
		int maxConnections, 
		bool norecover, 
		EspException** Exceptions, int* Exceptions_nSizeIs, 
		BSTR* wuid
	);

	HRESULT GetDFUWorkunits(
		BSTR* Owner, 
		BSTR* Cluster, 
		BSTR* StateReq, 
		BSTR* Type, 
		BSTR Jobname, 
		__int64* PageSize, 
		int CurrentPage, 
		__int64* PageStartFrom, 
		BSTR* Sortby, 
		bool* Descending, 
		EspException** Exceptions, int* Exceptions_nSizeIs, 
		DFUWorkunit** results, int* results_nSizeIs, 
		__int64* PrevPage, 
		__int64* NextPage, 
		__int64* LastPage, 
		__int64* NumWUs, 
		__int64* PageEndAt, 
		bool* First, 
		BSTR* BasicQuery, 
		BSTR* Filters
	);

	HRESULT GetDFUExceptions(
		BSTR wuid, 
		EspException** Exceptions, int* Exceptions_nSizeIs, 
		DFUException** result, int* result_nSizeIs
	);

	HRESULT FileList(
		BSTR* Netaddr, 
		BSTR* Path, 
		BSTR* Mask, 
		BSTR* OS, 
		bool* DirectoryOnly, 
		EspException** Exceptions, int* Exceptions_nSizeIs, 
		PhysicalFileStruct** files, int* files_nSizeIs
	);

	HRESULT CreateDFUWorkunit(
		EspException** Exceptions, int* Exceptions_nSizeIs, 
		DFUWorkunit* result
	);

	HRESULT SubmitDFUWorkunit(
		BSTR wuid, 
		EspException** Exceptions, int* Exceptions_nSizeIs
	);

	HRESULT DFUWorkunitsAction(
		BSTR* wuids, int wuids_nSizeIs, 
		BSTR Type, 
		EspException** Exceptions, int* Exceptions_nSizeIs, 
		DFUActionResult** DFUActionResults, int* DFUActionResults_nSizeIs
	);

	HRESULT UpdateDFUWorkunit(
		DFUWorkunit wu, 
		BSTR ClusterOrig, 
		BSTR JobNameOrig, 
		bool isProtectedOrig, 
		int StateOrig, 
		EspException** Exceptions, int* Exceptions_nSizeIs
	);

	HRESULT Replicate(
		BSTR sourceLogicalName, 
		int replicateOffset, 
		BSTR cluster, 
		bool repeatLast, 
		bool onlyRepeated, 
		EspException** Exceptions, int* Exceptions_nSizeIs, 
		BSTR* wuid
	);

	HRESULT Rename(
		BSTR srcname, 
		BSTR dstname, 
		bool overwrite, 
		EspException** Exceptions, int* Exceptions_nSizeIs, 
		BSTR* wuid
	);

	HRESULT DeleteDFUWorkunit(
		BSTR wuid, 
		EspException** Exceptions, int* Exceptions_nSizeIs, 
		bool* result
	);

	HRESULT DfuMonitor(
		BSTR EventName, 
		BSTR LogicalName, 
		BSTR Ip, 
		BSTR Filename, 
		bool Sub, 
		int ShotLimit, 
		EspException** Exceptions, int* Exceptions_nSizeIs, 
		BSTR* wuid
	);

	HRESULT AbortDFUWorkunit(
		BSTR wuid, 
		EspException** Exceptions, int* Exceptions_nSizeIs
	);
};

typedef CFileSprayT<> CFileSpray;

__if_not_exists(__DFUActionResult_entries)
{
extern __declspec(selectany) const _soapmapentry __DFUActionResult_entries[] =
{
	{ 
		0x000009AD, 
		"ID", 
		L"ID", 
		sizeof("ID")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUActionResult, ID),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x9EE0341E, 
		"Action", 
		L"Action", 
		sizeof("Action")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUActionResult, Action),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xC6ABBE1F, 
		"Result", 
		L"Result", 
		sizeof("Result")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUActionResult, Result),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __DFUActionResult_map =
{
	0xD44D21DC,
	"DFUActionResult",
	L"DFUActionResult",
	sizeof("DFUActionResult")-1,
	sizeof("DFUActionResult")-1,
	SOAPMAP_STRUCT,
	__DFUActionResult_entries,
	sizeof(DFUActionResult),
	3,
	-1,
	SOAPFLAG_NONE,
	0x10852992,
	"http://webservices.seisint.com/FileSpray",
	L"http://webservices.seisint.com/FileSpray",
	sizeof("http://webservices.seisint.com/FileSpray")-1
};
}

__if_not_exists(__PhysicalFileStruct_entries)
{
extern __declspec(selectany) const _soapmapentry __PhysicalFileStruct_entries[] =
{
	{ 
		0x003DFCC1, 
		"name", 
		L"name", 
		sizeof("name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(PhysicalFileStruct, name),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x07AC4BBB, 
		"isDir", 
		L"isDir", 
		sizeof("isDir")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(PhysicalFileStruct, isDir),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xB82F69FB, 
		"filesize", 
		L"filesize", 
		sizeof("filesize")-1, 
		SOAPTYPE_LONG, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(PhysicalFileStruct, filesize),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xADC62390, 
		"modifiedtime", 
		L"modifiedtime", 
		sizeof("modifiedtime")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(PhysicalFileStruct, modifiedtime),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __PhysicalFileStruct_map =
{
	0xB28C7122,
	"PhysicalFileStruct",
	L"PhysicalFileStruct",
	sizeof("PhysicalFileStruct")-1,
	sizeof("PhysicalFileStruct")-1,
	SOAPMAP_STRUCT,
	__PhysicalFileStruct_entries,
	sizeof(PhysicalFileStruct),
	4,
	-1,
	SOAPFLAG_NONE,
	0x10852992,
	"http://webservices.seisint.com/FileSpray",
	L"http://webservices.seisint.com/FileSpray",
	sizeof("http://webservices.seisint.com/FileSpray")-1
};
}

__if_not_exists(__EspException_entries)
{
extern __declspec(selectany) const _soapmapentry __EspException_entries[] =
{
	{ 
		0x0026A2DB, 
		"Code", 
		L"Code", 
		sizeof("Code")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(EspException, Code),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x1BFE007E, 
		"Audience", 
		L"Audience", 
		sizeof("Audience")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(EspException, Audience),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xC9B6E691, 
		"Source", 
		L"Source", 
		sizeof("Source")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(EspException, Source),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x1B3F0965, 
		"Message", 
		L"Message", 
		sizeof("Message")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(EspException, Message),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __EspException_map =
{
	0xBBCDFC77,
	"EspException",
	L"EspException",
	sizeof("EspException")-1,
	sizeof("EspException")-1,
	SOAPMAP_STRUCT,
	__EspException_entries,
	sizeof(EspException),
	4,
	-1,
	SOAPFLAG_NONE,
	0x10852992,
	"http://webservices.seisint.com/FileSpray",
	L"http://webservices.seisint.com/FileSpray",
	sizeof("http://webservices.seisint.com/FileSpray")-1
};
}

__if_not_exists(__DFUWorkunit_entries)
{
extern __declspec(selectany) const _soapmapentry __DFUWorkunit_entries[] =
{
	{ 
		0x000009AD, 
		"ID", 
		L"ID", 
		sizeof("ID")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUWorkunit, ID),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xCF62D983, 
		"ClusterName", 
		L"ClusterName", 
		sizeof("ClusterName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUWorkunit, ClusterName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x4A5BFF9C, 
		"JobName", 
		L"JobName", 
		sizeof("JobName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUWorkunit, JobName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x05FBA645, 
		"Queue", 
		L"Queue", 
		sizeof("Queue")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUWorkunit, Queue),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x003092DF, 
		"User", 
		L"User", 
		sizeof("User")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUWorkunit, User),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x966C55E6, 
		"isProtected", 
		L"isProtected", 
		sizeof("isProtected")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUWorkunit, isProtected),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x305C63BF, 
		"Command", 
		L"Command", 
		sizeof("Command")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUWorkunit, Command),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x868BE044, 
		"CommandMessage", 
		L"CommandMessage", 
		sizeof("CommandMessage")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUWorkunit, CommandMessage),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xE9AF4417, 
		"PercentDone", 
		L"PercentDone", 
		sizeof("PercentDone")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUWorkunit, PercentDone),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xE74D1D59, 
		"SecsLeft", 
		L"SecsLeft", 
		sizeof("SecsLeft")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUWorkunit, SecsLeft),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x11380DDA, 
		"ProgressMessage", 
		L"ProgressMessage", 
		sizeof("ProgressMessage")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUWorkunit, ProgressMessage),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x4A21EDD3, 
		"SummaryMessage", 
		L"SummaryMessage", 
		sizeof("SummaryMessage")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUWorkunit, SummaryMessage),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x061F39C1, 
		"State", 
		L"State", 
		sizeof("State")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUWorkunit, State),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xB7B838ED, 
		"SourceLogicalName", 
		L"SourceLogicalName", 
		sizeof("SourceLogicalName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUWorkunit, SourceLogicalName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x130AD88A, 
		"SourceIP", 
		L"SourceIP", 
		sizeof("SourceIP")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUWorkunit, SourceIP),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x179A615E, 
		"SourceFilePath", 
		L"SourceFilePath", 
		sizeof("SourceFilePath")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUWorkunit, SourceFilePath),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0120BBCB, 
		"SourceDali", 
		L"SourceDali", 
		sizeof("SourceDali")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUWorkunit, SourceDali),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xC92EC8CB, 
		"SourceRecordSize", 
		L"SourceRecordSize", 
		sizeof("SourceRecordSize")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUWorkunit, SourceRecordSize),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xD1E9EB1A, 
		"SourceFormat", 
		L"SourceFormat", 
		sizeof("SourceFormat")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUWorkunit, SourceFormat),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xC76256D4, 
		"RowTag", 
		L"RowTag", 
		sizeof("RowTag")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUWorkunit, RowTag),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x1646A24B, 
		"SourceNumParts", 
		L"SourceNumParts", 
		sizeof("SourceNumParts")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUWorkunit, SourceNumParts),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xF12D40E6, 
		"SourceDirectory", 
		L"SourceDirectory", 
		sizeof("SourceDirectory")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUWorkunit, SourceDirectory),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x614CA7AC, 
		"DestLogicalName", 
		L"DestLogicalName", 
		sizeof("DestLogicalName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUWorkunit, DestLogicalName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x2008C27E, 
		"DestGroupName", 
		L"DestGroupName", 
		sizeof("DestGroupName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUWorkunit, DestGroupName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x3508F3E5, 
		"DestDirectory", 
		L"DestDirectory", 
		sizeof("DestDirectory")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUWorkunit, DestDirectory),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xA6037C69, 
		"DestIP", 
		L"DestIP", 
		sizeof("DestIP")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUWorkunit, DestIP),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x673C307D, 
		"DestFilePath", 
		L"DestFilePath", 
		sizeof("DestFilePath")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUWorkunit, DestFilePath),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xB63F6679, 
		"DestFormat", 
		L"DestFormat", 
		sizeof("DestFormat")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUWorkunit, DestFormat),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x65E8716A, 
		"DestNumParts", 
		L"DestNumParts", 
		sizeof("DestNumParts")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUWorkunit, DestNumParts),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x8880DBAA, 
		"DestRecordSize", 
		L"DestRecordSize", 
		sizeof("DestRecordSize")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUWorkunit, DestRecordSize),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x36E0E9D9, 
		"Replicate", 
		L"Replicate", 
		sizeof("Replicate")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUWorkunit, Replicate),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x4A0EC407, 
		"Overwrite", 
		L"Overwrite", 
		sizeof("Overwrite")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUWorkunit, Overwrite),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x3C2853AC, 
		"Compress", 
		L"Compress", 
		sizeof("Compress")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUWorkunit, Compress),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x4489FC86, 
		"TimeStarted", 
		L"TimeStarted", 
		sizeof("TimeStarted")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUWorkunit, TimeStarted),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x458629CE, 
		"TimeStopped", 
		L"TimeStopped", 
		sizeof("TimeStopped")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUWorkunit, TimeStopped),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xEE85A006, 
		"StateMessage", 
		L"StateMessage", 
		sizeof("StateMessage")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUWorkunit, StateMessage),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xFD49B9AB, 
		"MonitorEventName", 
		L"MonitorEventName", 
		sizeof("MonitorEventName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUWorkunit, MonitorEventName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xE6BE06D2, 
		"MonitorSub", 
		L"MonitorSub", 
		sizeof("MonitorSub")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUWorkunit, MonitorSub),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x7D7EC365, 
		"MonitorShotLimit", 
		L"MonitorShotLimit", 
		sizeof("MonitorShotLimit")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUWorkunit, MonitorShotLimit),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0E1B6774, 
		"SourceDiffKeyName", 
		L"SourceDiffKeyName", 
		sizeof("SourceDiffKeyName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUWorkunit, SourceDiffKeyName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xB7AFD633, 
		"DestDiffKeyName", 
		L"DestDiffKeyName", 
		sizeof("DestDiffKeyName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUWorkunit, DestDiffKeyName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x32AA5CE6, 
		"Archived", 
		L"Archived", 
		sizeof("Archived")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUWorkunit, Archived),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x66917C05, 
		"encrypt", 
		L"encrypt", 
		sizeof("encrypt")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUWorkunit, encrypt),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0498D19B, 
		"decrypt", 
		L"decrypt", 
		sizeof("decrypt")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUWorkunit, decrypt),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __DFUWorkunit_map =
{
	0xF17369A2,
	"DFUWorkunit",
	L"DFUWorkunit",
	sizeof("DFUWorkunit")-1,
	sizeof("DFUWorkunit")-1,
	SOAPMAP_STRUCT,
	__DFUWorkunit_entries,
	sizeof(DFUWorkunit),
	44,
	-1,
	SOAPFLAG_NONE,
	0x10852992,
	"http://webservices.seisint.com/FileSpray",
	L"http://webservices.seisint.com/FileSpray",
	sizeof("http://webservices.seisint.com/FileSpray")-1
};
}

__if_not_exists(__DFUException_entries)
{
extern __declspec(selectany) const _soapmapentry __DFUException_entries[] =
{
	{ 
		0x0026A2DB, 
		"Code", 
		L"Code", 
		sizeof("Code")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUException, Code),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x1B3F0965, 
		"Message", 
		L"Message", 
		sizeof("Message")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUException, Message),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __DFUException_map =
{
	0xAC71742E,
	"DFUException",
	L"DFUException",
	sizeof("DFUException")-1,
	sizeof("DFUException")-1,
	SOAPMAP_STRUCT,
	__DFUException_entries,
	sizeof(DFUException),
	2,
	-1,
	SOAPFLAG_NONE,
	0x10852992,
	"http://webservices.seisint.com/FileSpray",
	L"http://webservices.seisint.com/FileSpray",
	sizeof("http://webservices.seisint.com/FileSpray")-1
};
}

struct __CFileSpray_GetDFUProgress_struct
{
	BSTR wuid;
	EspException *Exceptions;
	int __Exceptions_nSizeIs;
	int PercentDone;
	int SecsLeft;
	int KbPerSecAve;
	int KbPerSec;
	int SlavesDone;
	BSTR TimeTaken;
	BSTR ProgressMessage;
	BSTR SummaryMessage;
	BSTR State;
};

extern __declspec(selectany) const _soapmapentry __CFileSpray_GetDFUProgress_entries[] =
{

	{
		0x004340B9, 
		"wuid", 
		L"wuid", 
		sizeof("wuid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_GetDFUProgress_struct, wuid),
		NULL,
		NULL,
		-1,
	},
	{
		0x7B0DBD02, 
		"Exceptions", 
		L"Exceptions", 
		sizeof("Exceptions")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_GetDFUProgress_struct, Exceptions),
		NULL,
		&__EspException_map,
		1+1,
	},
	{
		0x7B0DBD02,
		"__Exceptions_nSizeIs",
		L"__Exceptions_nSizeIs",
		sizeof("__Exceptions_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CFileSpray_GetDFUProgress_struct, __Exceptions_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xE9AF4417, 
		"PercentDone", 
		L"PercentDone", 
		sizeof("PercentDone")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_GetDFUProgress_struct, PercentDone),
		NULL,
		NULL,
		-1,
	},
	{
		0xE74D1D59, 
		"SecsLeft", 
		L"SecsLeft", 
		sizeof("SecsLeft")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_GetDFUProgress_struct, SecsLeft),
		NULL,
		NULL,
		-1,
	},
	{
		0x4B77486B, 
		"KbPerSecAve", 
		L"KbPerSecAve", 
		sizeof("KbPerSecAve")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_GetDFUProgress_struct, KbPerSecAve),
		NULL,
		NULL,
		-1,
	},
	{
		0x7114EEAF, 
		"KbPerSec", 
		L"KbPerSec", 
		sizeof("KbPerSec")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_GetDFUProgress_struct, KbPerSec),
		NULL,
		NULL,
		-1,
	},
	{
		0x68179E54, 
		"SlavesDone", 
		L"SlavesDone", 
		sizeof("SlavesDone")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_GetDFUProgress_struct, SlavesDone),
		NULL,
		NULL,
		-1,
	},
	{
		0xD8215A02, 
		"TimeTaken", 
		L"TimeTaken", 
		sizeof("TimeTaken")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_GetDFUProgress_struct, TimeTaken),
		NULL,
		NULL,
		-1,
	},
	{
		0x11380DDA, 
		"ProgressMessage", 
		L"ProgressMessage", 
		sizeof("ProgressMessage")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_GetDFUProgress_struct, ProgressMessage),
		NULL,
		NULL,
		-1,
	},
	{
		0x4A21EDD3, 
		"SummaryMessage", 
		L"SummaryMessage", 
		sizeof("SummaryMessage")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_GetDFUProgress_struct, SummaryMessage),
		NULL,
		NULL,
		-1,
	},
	{
		0x061F39C1, 
		"State", 
		L"State", 
		sizeof("State")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_GetDFUProgress_struct, State),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CFileSpray_GetDFUProgress_map =
{
	0xD5547544,
	"ProgressRequest",
	L"ProgressResponse",
	sizeof("ProgressRequest")-1,
	sizeof("ProgressResponse")-1,
	SOAPMAP_FUNC,
	__CFileSpray_GetDFUProgress_entries,
	sizeof(__CFileSpray_GetDFUProgress_struct),
	11,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x10852992,
	"http://webservices.seisint.com/FileSpray",
	L"http://webservices.seisint.com/FileSpray",
	sizeof("http://webservices.seisint.com/FileSpray")-1
};


struct __CFileSpray_SprayVariable_struct
{
	BSTR sourceIP;
	BSTR sourcePath;
	ATLSOAP_BLOB srcxml;
	int sourceMaxRecordSize;
	int sourceFormat;
	BSTR sourceCsvSeparate;
	BSTR sourceCsvTerminate;
	BSTR sourceCsvQuote;
	BSTR sourceRowTag;
	BSTR destGroup;
	BSTR destLogicalName;
	bool overwrite;
	bool replicate;
	int ReplicateOffset;
	int maxConnections;
	int throttle;
	int transferBufferSize;
	BSTR prefix;
	bool nosplit;
	bool norecover;
	bool compress;
	bool push;
	bool pull;
	BSTR encrypt;
	BSTR decrypt;
	EspException *Exceptions;
	int __Exceptions_nSizeIs;
	BSTR wuid;
};

extern __declspec(selectany) const _soapmapentry __CFileSpray_SprayVariable_entries[] =
{

	{
		0x9B4574AA, 
		"sourceIP", 
		L"sourceIP", 
		sizeof("sourceIP")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_SprayVariable_struct, sourceIP),
		NULL,
		NULL,
		-1,
	},
	{
		0x8279757E, 
		"sourcePath", 
		L"sourcePath", 
		sizeof("sourcePath")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_SprayVariable_struct, sourcePath),
		NULL,
		NULL,
		-1,
	},
	{
		0x14888039, 
		"srcxml", 
		L"srcxml", 
		sizeof("srcxml")-1, 
		SOAPTYPE_BASE64BINARY, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_SprayVariable_struct, srcxml),
		NULL,
		NULL,
		-1,
	},
	{
		0xBEC8FF51, 
		"sourceMaxRecordSize", 
		L"sourceMaxRecordSize", 
		sizeof("sourceMaxRecordSize")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_SprayVariable_struct, sourceMaxRecordSize),
		NULL,
		NULL,
		-1,
	},
	{
		0xF055973A, 
		"sourceFormat", 
		L"sourceFormat", 
		sizeof("sourceFormat")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_SprayVariable_struct, sourceFormat),
		NULL,
		NULL,
		-1,
	},
	{
		0xB4BD3DF2, 
		"sourceCsvSeparate", 
		L"sourceCsvSeparate", 
		sizeof("sourceCsvSeparate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_SprayVariable_struct, sourceCsvSeparate),
		NULL,
		NULL,
		-1,
	},
	{
		0x7637C426, 
		"sourceCsvTerminate", 
		L"sourceCsvTerminate", 
		sizeof("sourceCsvTerminate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_SprayVariable_struct, sourceCsvTerminate),
		NULL,
		NULL,
		-1,
	},
	{
		0xD2A0A82B, 
		"sourceCsvQuote", 
		L"sourceCsvQuote", 
		sizeof("sourceCsvQuote")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_SprayVariable_struct, sourceCsvQuote),
		NULL,
		NULL,
		-1,
	},
	{
		0x0C55D245, 
		"sourceRowTag", 
		L"sourceRowTag", 
		sizeof("sourceRowTag")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_SprayVariable_struct, sourceRowTag),
		NULL,
		NULL,
		-1,
	},
	{
		0x5EDA0B7D, 
		"destGroup", 
		L"destGroup", 
		sizeof("destGroup")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_SprayVariable_struct, destGroup),
		NULL,
		NULL,
		-1,
	},
	{
		0xCA3A5FCC, 
		"destLogicalName", 
		L"destLogicalName", 
		sizeof("destLogicalName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_SprayVariable_struct, destLogicalName),
		NULL,
		NULL,
		-1,
	},
	{
		0xD99CE427, 
		"overwrite", 
		L"overwrite", 
		sizeof("overwrite")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_SprayVariable_struct, overwrite),
		NULL,
		NULL,
		-1,
	},
	{
		0xC66F09F9, 
		"replicate", 
		L"replicate", 
		sizeof("replicate")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_SprayVariable_struct, replicate),
		NULL,
		NULL,
		-1,
	},
	{
		0xA0005B00, 
		"ReplicateOffset", 
		L"ReplicateOffset", 
		sizeof("ReplicateOffset")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_SprayVariable_struct, ReplicateOffset),
		NULL,
		NULL,
		-1,
	},
	{
		0xC72090C9, 
		"maxConnections", 
		L"maxConnections", 
		sizeof("maxConnections")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_SprayVariable_struct, maxConnections),
		NULL,
		NULL,
		-1,
	},
	{
		0x658373B6, 
		"throttle", 
		L"throttle", 
		sizeof("throttle")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_SprayVariable_struct, throttle),
		NULL,
		NULL,
		-1,
	},
	{
		0x0A46333A, 
		"transferBufferSize", 
		L"transferBufferSize", 
		sizeof("transferBufferSize")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_SprayVariable_struct, transferBufferSize),
		NULL,
		NULL,
		-1,
	},
	{
		0x0D89D20E, 
		"prefix", 
		L"prefix", 
		sizeof("prefix")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_SprayVariable_struct, prefix),
		NULL,
		NULL,
		-1,
	},
	{
		0x1ED2FE89, 
		"nosplit", 
		L"nosplit", 
		sizeof("nosplit")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_SprayVariable_struct, nosplit),
		NULL,
		NULL,
		-1,
	},
	{
		0xB84A4CD3, 
		"norecover", 
		L"norecover", 
		sizeof("norecover")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_SprayVariable_struct, norecover),
		NULL,
		NULL,
		-1,
	},
	{
		0xC462EFCC, 
		"compress", 
		L"compress", 
		sizeof("compress")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_SprayVariable_struct, compress),
		NULL,
		NULL,
		-1,
	},
	{
		0x003F6B60, 
		"push", 
		L"push", 
		sizeof("push")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_SprayVariable_struct, push),
		NULL,
		NULL,
		-1,
	},
	{
		0x003F6A7D, 
		"pull", 
		L"pull", 
		sizeof("pull")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_SprayVariable_struct, pull),
		NULL,
		NULL,
		-1,
	},
	{
		0x66917C05, 
		"encrypt", 
		L"encrypt", 
		sizeof("encrypt")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_SprayVariable_struct, encrypt),
		NULL,
		NULL,
		-1,
	},
	{
		0x0498D19B, 
		"decrypt", 
		L"decrypt", 
		sizeof("decrypt")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_SprayVariable_struct, decrypt),
		NULL,
		NULL,
		-1,
	},
	{
		0x7B0DBD02, 
		"Exceptions", 
		L"Exceptions", 
		sizeof("Exceptions")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_SprayVariable_struct, Exceptions),
		NULL,
		&__EspException_map,
		25+1,
	},
	{
		0x7B0DBD02,
		"__Exceptions_nSizeIs",
		L"__Exceptions_nSizeIs",
		sizeof("__Exceptions_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CFileSpray_SprayVariable_struct, __Exceptions_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x004340B9, 
		"wuid", 
		L"wuid", 
		sizeof("wuid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_SprayVariable_struct, wuid),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CFileSpray_SprayVariable_map =
{
	0x409EEF5E,
	"SprayVariable",
	L"SprayResponse",
	sizeof("SprayVariable")-1,
	sizeof("SprayResponse")-1,
	SOAPMAP_FUNC,
	__CFileSpray_SprayVariable_entries,
	sizeof(__CFileSpray_SprayVariable_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x10852992,
	"http://webservices.seisint.com/FileSpray",
	L"http://webservices.seisint.com/FileSpray",
	sizeof("http://webservices.seisint.com/FileSpray")-1
};


struct __CFileSpray_SprayFixed_struct
{
	BSTR sourceIP;
	BSTR sourcePath;
	ATLSOAP_BLOB srcxml;
	int sourceRecordSize;
	BSTR destGroup;
	BSTR destLogicalName;
	bool overwrite;
	bool replicate;
	int ReplicateOffset;
	int maxConnections;
	int throttle;
	int transferBufferSize;
	BSTR prefix;
	bool nosplit;
	bool norecover;
	bool compress;
	bool push;
	bool pull;
	BSTR encrypt;
	BSTR decrypt;
	EspException *Exceptions;
	int __Exceptions_nSizeIs;
	BSTR wuid;
};

extern __declspec(selectany) const _soapmapentry __CFileSpray_SprayFixed_entries[] =
{

	{
		0x9B4574AA, 
		"sourceIP", 
		L"sourceIP", 
		sizeof("sourceIP")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_SprayFixed_struct, sourceIP),
		NULL,
		NULL,
		-1,
	},
	{
		0x8279757E, 
		"sourcePath", 
		L"sourcePath", 
		sizeof("sourcePath")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_SprayFixed_struct, sourcePath),
		NULL,
		NULL,
		-1,
	},
	{
		0x14888039, 
		"srcxml", 
		L"srcxml", 
		sizeof("srcxml")-1, 
		SOAPTYPE_BASE64BINARY, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_SprayFixed_struct, srcxml),
		NULL,
		NULL,
		-1,
	},
	{
		0x4FD384EB, 
		"sourceRecordSize", 
		L"sourceRecordSize", 
		sizeof("sourceRecordSize")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_SprayFixed_struct, sourceRecordSize),
		NULL,
		NULL,
		-1,
	},
	{
		0x5EDA0B7D, 
		"destGroup", 
		L"destGroup", 
		sizeof("destGroup")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_SprayFixed_struct, destGroup),
		NULL,
		NULL,
		-1,
	},
	{
		0xCA3A5FCC, 
		"destLogicalName", 
		L"destLogicalName", 
		sizeof("destLogicalName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_SprayFixed_struct, destLogicalName),
		NULL,
		NULL,
		-1,
	},
	{
		0xD99CE427, 
		"overwrite", 
		L"overwrite", 
		sizeof("overwrite")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_SprayFixed_struct, overwrite),
		NULL,
		NULL,
		-1,
	},
	{
		0xC66F09F9, 
		"replicate", 
		L"replicate", 
		sizeof("replicate")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_SprayFixed_struct, replicate),
		NULL,
		NULL,
		-1,
	},
	{
		0xA0005B00, 
		"ReplicateOffset", 
		L"ReplicateOffset", 
		sizeof("ReplicateOffset")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_SprayFixed_struct, ReplicateOffset),
		NULL,
		NULL,
		-1,
	},
	{
		0xC72090C9, 
		"maxConnections", 
		L"maxConnections", 
		sizeof("maxConnections")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_SprayFixed_struct, maxConnections),
		NULL,
		NULL,
		-1,
	},
	{
		0x658373B6, 
		"throttle", 
		L"throttle", 
		sizeof("throttle")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_SprayFixed_struct, throttle),
		NULL,
		NULL,
		-1,
	},
	{
		0x0A46333A, 
		"transferBufferSize", 
		L"transferBufferSize", 
		sizeof("transferBufferSize")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_SprayFixed_struct, transferBufferSize),
		NULL,
		NULL,
		-1,
	},
	{
		0x0D89D20E, 
		"prefix", 
		L"prefix", 
		sizeof("prefix")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_SprayFixed_struct, prefix),
		NULL,
		NULL,
		-1,
	},
	{
		0x1ED2FE89, 
		"nosplit", 
		L"nosplit", 
		sizeof("nosplit")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_SprayFixed_struct, nosplit),
		NULL,
		NULL,
		-1,
	},
	{
		0xB84A4CD3, 
		"norecover", 
		L"norecover", 
		sizeof("norecover")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_SprayFixed_struct, norecover),
		NULL,
		NULL,
		-1,
	},
	{
		0xC462EFCC, 
		"compress", 
		L"compress", 
		sizeof("compress")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_SprayFixed_struct, compress),
		NULL,
		NULL,
		-1,
	},
	{
		0x003F6B60, 
		"push", 
		L"push", 
		sizeof("push")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_SprayFixed_struct, push),
		NULL,
		NULL,
		-1,
	},
	{
		0x003F6A7D, 
		"pull", 
		L"pull", 
		sizeof("pull")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_SprayFixed_struct, pull),
		NULL,
		NULL,
		-1,
	},
	{
		0x66917C05, 
		"encrypt", 
		L"encrypt", 
		sizeof("encrypt")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_SprayFixed_struct, encrypt),
		NULL,
		NULL,
		-1,
	},
	{
		0x0498D19B, 
		"decrypt", 
		L"decrypt", 
		sizeof("decrypt")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_SprayFixed_struct, decrypt),
		NULL,
		NULL,
		-1,
	},
	{
		0x7B0DBD02, 
		"Exceptions", 
		L"Exceptions", 
		sizeof("Exceptions")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_SprayFixed_struct, Exceptions),
		NULL,
		&__EspException_map,
		20+1,
	},
	{
		0x7B0DBD02,
		"__Exceptions_nSizeIs",
		L"__Exceptions_nSizeIs",
		sizeof("__Exceptions_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CFileSpray_SprayFixed_struct, __Exceptions_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x004340B9, 
		"wuid", 
		L"wuid", 
		sizeof("wuid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_SprayFixed_struct, wuid),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CFileSpray_SprayFixed_map =
{
	0x43DBC7AE,
	"SprayFixed",
	L"SprayFixedResponse",
	sizeof("SprayFixed")-1,
	sizeof("SprayFixedResponse")-1,
	SOAPMAP_FUNC,
	__CFileSpray_SprayFixed_entries,
	sizeof(__CFileSpray_SprayFixed_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x10852992,
	"http://webservices.seisint.com/FileSpray",
	L"http://webservices.seisint.com/FileSpray",
	sizeof("http://webservices.seisint.com/FileSpray")-1
};


struct __CFileSpray_Despray_struct
{
	BSTR sourceLogicalName;
	BSTR destIP;
	BSTR destPath;
	ATLSOAP_BLOB dstxml;
	bool overwrite;
	int maxConnections;
	int throttle;
	int transferBufferSize;
	BSTR splitprefix;
	bool norecover;
	bool wrap;
	bool multiCopy;
	bool SingleConnection;
	bool compress;
	BSTR encrypt;
	BSTR decrypt;
	EspException *Exceptions;
	int __Exceptions_nSizeIs;
	BSTR wuid;
};

extern __declspec(selectany) const _soapmapentry __CFileSpray_Despray_entries[] =
{

	{
		0x12F4790D, 
		"sourceLogicalName", 
		L"sourceLogicalName", 
		sizeof("sourceLogicalName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Despray_struct, sourceLogicalName),
		NULL,
		NULL,
		-1,
	},
	{
		0xF0A89089, 
		"destIP", 
		L"destIP", 
		sizeof("destIP")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Despray_struct, destIP),
		NULL,
		NULL,
		-1,
	},
	{
		0xBD13051D, 
		"destPath", 
		L"destPath", 
		sizeof("destPath")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Despray_struct, destPath),
		NULL,
		NULL,
		-1,
	},
	{
		0xF1A689BC, 
		"dstxml", 
		L"dstxml", 
		sizeof("dstxml")-1, 
		SOAPTYPE_BASE64BINARY, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Despray_struct, dstxml),
		NULL,
		NULL,
		-1,
	},
	{
		0xD99CE427, 
		"overwrite", 
		L"overwrite", 
		sizeof("overwrite")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Despray_struct, overwrite),
		NULL,
		NULL,
		-1,
	},
	{
		0xC72090C9, 
		"maxConnections", 
		L"maxConnections", 
		sizeof("maxConnections")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Despray_struct, maxConnections),
		NULL,
		NULL,
		-1,
	},
	{
		0x658373B6, 
		"throttle", 
		L"throttle", 
		sizeof("throttle")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Despray_struct, throttle),
		NULL,
		NULL,
		-1,
	},
	{
		0x0A46333A, 
		"transferBufferSize", 
		L"transferBufferSize", 
		sizeof("transferBufferSize")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Despray_struct, transferBufferSize),
		NULL,
		NULL,
		-1,
	},
	{
		0xA7B240DA, 
		"splitprefix", 
		L"splitprefix", 
		sizeof("splitprefix")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Despray_struct, splitprefix),
		NULL,
		NULL,
		-1,
	},
	{
		0xB84A4CD3, 
		"norecover", 
		L"norecover", 
		sizeof("norecover")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Despray_struct, norecover),
		NULL,
		NULL,
		-1,
	},
	{
		0x004332FA, 
		"wrap", 
		L"wrap", 
		sizeof("wrap")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Despray_struct, wrap),
		NULL,
		NULL,
		-1,
	},
	{
		0x22CD2406, 
		"multiCopy", 
		L"multiCopy", 
		sizeof("multiCopy")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Despray_struct, multiCopy),
		NULL,
		NULL,
		-1,
	},
	{
		0xB1FE4332, 
		"SingleConnection", 
		L"SingleConnection", 
		sizeof("SingleConnection")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Despray_struct, SingleConnection),
		NULL,
		NULL,
		-1,
	},
	{
		0xC462EFCC, 
		"compress", 
		L"compress", 
		sizeof("compress")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Despray_struct, compress),
		NULL,
		NULL,
		-1,
	},
	{
		0x66917C05, 
		"encrypt", 
		L"encrypt", 
		sizeof("encrypt")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Despray_struct, encrypt),
		NULL,
		NULL,
		-1,
	},
	{
		0x0498D19B, 
		"decrypt", 
		L"decrypt", 
		sizeof("decrypt")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Despray_struct, decrypt),
		NULL,
		NULL,
		-1,
	},
	{
		0x7B0DBD02, 
		"Exceptions", 
		L"Exceptions", 
		sizeof("Exceptions")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Despray_struct, Exceptions),
		NULL,
		&__EspException_map,
		16+1,
	},
	{
		0x7B0DBD02,
		"__Exceptions_nSizeIs",
		L"__Exceptions_nSizeIs",
		sizeof("__Exceptions_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CFileSpray_Despray_struct, __Exceptions_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x004340B9, 
		"wuid", 
		L"wuid", 
		sizeof("wuid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Despray_struct, wuid),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CFileSpray_Despray_map =
{
	0xEBD00347,
	"Despray",
	L"DesprayResponse",
	sizeof("Despray")-1,
	sizeof("DesprayResponse")-1,
	SOAPMAP_FUNC,
	__CFileSpray_Despray_entries,
	sizeof(__CFileSpray_Despray_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x10852992,
	"http://webservices.seisint.com/FileSpray",
	L"http://webservices.seisint.com/FileSpray",
	sizeof("http://webservices.seisint.com/FileSpray")-1
};


struct __CFileSpray_DFUWUSearch_struct
{
	EspException *Exceptions;
	int __Exceptions_nSizeIs;
	BSTR *ClusterNames;
	int __ClusterNames_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CFileSpray_DFUWUSearch_entries[] =
{

	{
		0x7B0DBD02, 
		"Exceptions", 
		L"Exceptions", 
		sizeof("Exceptions")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_DFUWUSearch_struct, Exceptions),
		NULL,
		&__EspException_map,
		0+1,
	},
	{
		0x7B0DBD02,
		"__Exceptions_nSizeIs",
		L"__Exceptions_nSizeIs",
		sizeof("__Exceptions_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CFileSpray_DFUWUSearch_struct, __Exceptions_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xBBBE0A56, 
		"ClusterNames", 
		L"ClusterNames", 
		sizeof("ClusterNames")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_DFUWUSearch_struct, ClusterNames),
		NULL,
		NULL,
		2+1,
	},
	{
		0xBBBE0A56,
		"__ClusterNames_nSizeIs",
		L"__ClusterNames_nSizeIs",
		sizeof("__ClusterNames_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CFileSpray_DFUWUSearch_struct, __ClusterNames_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CFileSpray_DFUWUSearch_map =
{
	0xABDF4B50,
	"DFUWUSearchRequest",
	L"DFUWUSearchResponse",
	sizeof("DFUWUSearchRequest")-1,
	sizeof("DFUWUSearchResponse")-1,
	SOAPMAP_FUNC,
	__CFileSpray_DFUWUSearch_entries,
	sizeof(__CFileSpray_DFUWUSearch_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x10852992,
	"http://webservices.seisint.com/FileSpray",
	L"http://webservices.seisint.com/FileSpray",
	sizeof("http://webservices.seisint.com/FileSpray")-1
};


struct __CFileSpray_Copy_struct
{
	BSTR sourceLogicalName;
	BSTR destGroup;
	BSTR destGroupRoxie;
	BSTR destLogicalName;
	BSTR sourceDali;
	BSTR srcusername;
	BSTR srcpassword;
	bool overwrite;
	bool replicate;
	int ReplicateOffset;
	int maxConnections;
	int throttle;
	int transferBufferSize;
	bool nosplit;
	bool norecover;
	bool compress;
	bool Wrap;
	bool Multicopy;
	BSTR SourceDiffKeyName;
	BSTR DestDiffKeyName;
	bool superCopy;
	bool push;
	bool pull;
	bool ifnewer;
	BSTR encrypt;
	BSTR decrypt;
	EspException *Exceptions;
	int __Exceptions_nSizeIs;
	BSTR result;
};

extern __declspec(selectany) const _soapmapentry __CFileSpray_Copy_entries[] =
{

	{
		0x12F4790D, 
		"sourceLogicalName", 
		L"sourceLogicalName", 
		sizeof("sourceLogicalName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Copy_struct, sourceLogicalName),
		NULL,
		NULL,
		-1,
	},
	{
		0x5EDA0B7D, 
		"destGroup", 
		L"destGroup", 
		sizeof("destGroup")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Copy_struct, destGroup),
		NULL,
		NULL,
		-1,
	},
	{
		0x89788564, 
		"destGroupRoxie", 
		L"destGroupRoxie", 
		sizeof("destGroupRoxie")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Copy_struct, destGroupRoxie),
		NULL,
		NULL,
		-1,
	},
	{
		0xCA3A5FCC, 
		"destLogicalName", 
		L"destLogicalName", 
		sizeof("destLogicalName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Copy_struct, destLogicalName),
		NULL,
		NULL,
		-1,
	},
	{
		0x8272DFEB, 
		"sourceDali", 
		L"sourceDali", 
		sizeof("sourceDali")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Copy_struct, sourceDali),
		NULL,
		NULL,
		-1,
	},
	{
		0x43DF4388, 
		"srcusername", 
		L"srcusername", 
		sizeof("srcusername")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Copy_struct, srcusername),
		NULL,
		NULL,
		-1,
	},
	{
		0x5DBD527B, 
		"srcpassword", 
		L"srcpassword", 
		sizeof("srcpassword")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Copy_struct, srcpassword),
		NULL,
		NULL,
		-1,
	},
	{
		0xD99CE427, 
		"overwrite", 
		L"overwrite", 
		sizeof("overwrite")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Copy_struct, overwrite),
		NULL,
		NULL,
		-1,
	},
	{
		0xC66F09F9, 
		"replicate", 
		L"replicate", 
		sizeof("replicate")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Copy_struct, replicate),
		NULL,
		NULL,
		-1,
	},
	{
		0xA0005B00, 
		"ReplicateOffset", 
		L"ReplicateOffset", 
		sizeof("ReplicateOffset")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Copy_struct, ReplicateOffset),
		NULL,
		NULL,
		-1,
	},
	{
		0xC72090C9, 
		"maxConnections", 
		L"maxConnections", 
		sizeof("maxConnections")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Copy_struct, maxConnections),
		NULL,
		NULL,
		-1,
	},
	{
		0x658373B6, 
		"throttle", 
		L"throttle", 
		sizeof("throttle")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Copy_struct, throttle),
		NULL,
		NULL,
		-1,
	},
	{
		0x0A46333A, 
		"transferBufferSize", 
		L"transferBufferSize", 
		sizeof("transferBufferSize")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Copy_struct, transferBufferSize),
		NULL,
		NULL,
		-1,
	},
	{
		0x1ED2FE89, 
		"nosplit", 
		L"nosplit", 
		sizeof("nosplit")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Copy_struct, nosplit),
		NULL,
		NULL,
		-1,
	},
	{
		0xB84A4CD3, 
		"norecover", 
		L"norecover", 
		sizeof("norecover")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Copy_struct, norecover),
		NULL,
		NULL,
		-1,
	},
	{
		0xC462EFCC, 
		"compress", 
		L"compress", 
		sizeof("compress")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Copy_struct, compress),
		NULL,
		NULL,
		-1,
	},
	{
		0x0031A6DA, 
		"Wrap", 
		L"Wrap", 
		sizeof("Wrap")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Copy_struct, Wrap),
		NULL,
		NULL,
		-1,
	},
	{
		0x93509006, 
		"Multicopy", 
		L"Multicopy", 
		sizeof("Multicopy")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Copy_struct, Multicopy),
		NULL,
		NULL,
		-1,
	},
	{
		0x0E1B6774, 
		"SourceDiffKeyName", 
		L"SourceDiffKeyName", 
		sizeof("SourceDiffKeyName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Copy_struct, SourceDiffKeyName),
		NULL,
		NULL,
		-1,
	},
	{
		0xB7AFD633, 
		"DestDiffKeyName", 
		L"DestDiffKeyName", 
		sizeof("DestDiffKeyName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Copy_struct, DestDiffKeyName),
		NULL,
		NULL,
		-1,
	},
	{
		0xEF46382A, 
		"superCopy", 
		L"superCopy", 
		sizeof("superCopy")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Copy_struct, superCopy),
		NULL,
		NULL,
		-1,
	},
	{
		0x003F6B60, 
		"push", 
		L"push", 
		sizeof("push")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Copy_struct, push),
		NULL,
		NULL,
		-1,
	},
	{
		0x003F6A7D, 
		"pull", 
		L"pull", 
		sizeof("pull")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Copy_struct, pull),
		NULL,
		NULL,
		-1,
	},
	{
		0x88910CB0, 
		"ifnewer", 
		L"ifnewer", 
		sizeof("ifnewer")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Copy_struct, ifnewer),
		NULL,
		NULL,
		-1,
	},
	{
		0x66917C05, 
		"encrypt", 
		L"encrypt", 
		sizeof("encrypt")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Copy_struct, encrypt),
		NULL,
		NULL,
		-1,
	},
	{
		0x0498D19B, 
		"decrypt", 
		L"decrypt", 
		sizeof("decrypt")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Copy_struct, decrypt),
		NULL,
		NULL,
		-1,
	},
	{
		0x7B0DBD02, 
		"Exceptions", 
		L"Exceptions", 
		sizeof("Exceptions")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Copy_struct, Exceptions),
		NULL,
		&__EspException_map,
		26+1,
	},
	{
		0x7B0DBD02,
		"__Exceptions_nSizeIs",
		L"__Exceptions_nSizeIs",
		sizeof("__Exceptions_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CFileSpray_Copy_struct, __Exceptions_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x1150D23F, 
		"result", 
		L"result", 
		sizeof("result")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Copy_struct, result),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CFileSpray_Copy_map =
{
	0x968FD1AA,
	"Copy",
	L"CopyResponse",
	sizeof("Copy")-1,
	sizeof("CopyResponse")-1,
	SOAPMAP_FUNC,
	__CFileSpray_Copy_entries,
	sizeof(__CFileSpray_Copy_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x10852992,
	"http://webservices.seisint.com/FileSpray",
	L"http://webservices.seisint.com/FileSpray",
	sizeof("http://webservices.seisint.com/FileSpray")-1
};


struct __CFileSpray_DFUWUFile_struct
{
	BSTR Wuid;
	BSTR Type;
	EspException *Exceptions;
	int __Exceptions_nSizeIs;
	BSTR file;
};

extern __declspec(selectany) const _soapmapentry __CFileSpray_DFUWUFile_entries[] =
{

	{
		0x0031B499, 
		"Wuid", 
		L"Wuid", 
		sizeof("Wuid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_DFUWUFile_struct, Wuid),
		NULL,
		NULL,
		-1,
	},
	{
		0x00302162, 
		"Type", 
		L"Type", 
		sizeof("Type")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_DFUWUFile_struct, Type),
		NULL,
		NULL,
		-1,
	},
	{
		0x7B0DBD02, 
		"Exceptions", 
		L"Exceptions", 
		sizeof("Exceptions")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_DFUWUFile_struct, Exceptions),
		NULL,
		&__EspException_map,
		2+1,
	},
	{
		0x7B0DBD02,
		"__Exceptions_nSizeIs",
		L"__Exceptions_nSizeIs",
		sizeof("__Exceptions_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CFileSpray_DFUWUFile_struct, __Exceptions_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x0039BBA0, 
		"file", 
		L"file", 
		sizeof("file")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_DFUWUFile_struct, file),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CFileSpray_DFUWUFile_map =
{
	0xEBDC241A,
	"DFUWUFileRequest",
	L"DFUWUFileResponse",
	sizeof("DFUWUFileRequest")-1,
	sizeof("DFUWUFileResponse")-1,
	SOAPMAP_FUNC,
	__CFileSpray_DFUWUFile_entries,
	sizeof(__CFileSpray_DFUWUFile_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x10852992,
	"http://webservices.seisint.com/FileSpray",
	L"http://webservices.seisint.com/FileSpray",
	sizeof("http://webservices.seisint.com/FileSpray")-1
};


struct __CFileSpray_DeleteDFUWorkunits_struct
{
	BSTR *wuids;
	int __wuids_nSizeIs;
	EspException *Exceptions;
	int __Exceptions_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CFileSpray_DeleteDFUWorkunits_entries[] =
{

	{
		0x08AB584C, 
		"wuids", 
		L"wuids", 
		sizeof("wuids")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_DeleteDFUWorkunits_struct, wuids),
		NULL,
		NULL,
		0+1,
	},
	{
		0x08AB584C,
		"__wuids_nSizeIs",
		L"__wuids_nSizeIs",
		sizeof("__wuids_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CFileSpray_DeleteDFUWorkunits_struct, __wuids_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x7B0DBD02, 
		"Exceptions", 
		L"Exceptions", 
		sizeof("Exceptions")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_DeleteDFUWorkunits_struct, Exceptions),
		NULL,
		&__EspException_map,
		2+1,
	},
	{
		0x7B0DBD02,
		"__Exceptions_nSizeIs",
		L"__Exceptions_nSizeIs",
		sizeof("__Exceptions_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CFileSpray_DeleteDFUWorkunits_struct, __Exceptions_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CFileSpray_DeleteDFUWorkunits_map =
{
	0x3DD1C897,
	"DeleteDFUWorkunits",
	L"DeleteDFUWorkunitsResponse",
	sizeof("DeleteDFUWorkunits")-1,
	sizeof("DeleteDFUWorkunitsResponse")-1,
	SOAPMAP_FUNC,
	__CFileSpray_DeleteDFUWorkunits_entries,
	sizeof(__CFileSpray_DeleteDFUWorkunits_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x10852992,
	"http://webservices.seisint.com/FileSpray",
	L"http://webservices.seisint.com/FileSpray",
	sizeof("http://webservices.seisint.com/FileSpray")-1
};


struct __CFileSpray_ShowResult_struct
{
	BSTR Result;
	EspException *Exceptions;
	int __Exceptions_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CFileSpray_ShowResult_entries[] =
{

	{
		0xC6ABBE1F, 
		"Result", 
		L"Result", 
		sizeof("Result")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_ShowResult_struct, Result),
		NULL,
		NULL,
		-1,
	},
	{
		0x7B0DBD02, 
		"Exceptions", 
		L"Exceptions", 
		sizeof("Exceptions")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_ShowResult_struct, Exceptions),
		NULL,
		&__EspException_map,
		1+1,
	},
	{
		0x7B0DBD02,
		"__Exceptions_nSizeIs",
		L"__Exceptions_nSizeIs",
		sizeof("__Exceptions_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CFileSpray_ShowResult_struct, __Exceptions_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CFileSpray_ShowResult_map =
{
	0xE3C6A1AF,
	"ShowResultRequest",
	L"ShowResultResponse",
	sizeof("ShowResultRequest")-1,
	sizeof("ShowResultResponse")-1,
	SOAPMAP_FUNC,
	__CFileSpray_ShowResult_entries,
	sizeof(__CFileSpray_ShowResult_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x10852992,
	"http://webservices.seisint.com/FileSpray",
	L"http://webservices.seisint.com/FileSpray",
	sizeof("http://webservices.seisint.com/FileSpray")-1
};


struct __CFileSpray_GetDFUWorkunit_struct
{
	BSTR wuid;
	EspException *Exceptions;
	int __Exceptions_nSizeIs;
	DFUWorkunit result;
	int AutoRefresh;
};

extern __declspec(selectany) const _soapmapentry __CFileSpray_GetDFUWorkunit_entries[] =
{

	{
		0x004340B9, 
		"wuid", 
		L"wuid", 
		sizeof("wuid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_GetDFUWorkunit_struct, wuid),
		NULL,
		NULL,
		-1,
	},
	{
		0x7B0DBD02, 
		"Exceptions", 
		L"Exceptions", 
		sizeof("Exceptions")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_GetDFUWorkunit_struct, Exceptions),
		NULL,
		&__EspException_map,
		1+1,
	},
	{
		0x7B0DBD02,
		"__Exceptions_nSizeIs",
		L"__Exceptions_nSizeIs",
		sizeof("__Exceptions_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CFileSpray_GetDFUWorkunit_struct, __Exceptions_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x1150D23F, 
		"result", 
		L"result", 
		sizeof("result")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_GetDFUWorkunit_struct, result),
		NULL,
		&__DFUWorkunit_map,
		-1,
	},
	{
		0x89835C68, 
		"AutoRefresh", 
		L"AutoRefresh", 
		sizeof("AutoRefresh")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_GetDFUWorkunit_struct, AutoRefresh),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CFileSpray_GetDFUWorkunit_map =
{
	0xDF6F5951,
	"GetDFUWorkunit",
	L"GetDFUWorkunitResponse",
	sizeof("GetDFUWorkunit")-1,
	sizeof("GetDFUWorkunitResponse")-1,
	SOAPMAP_FUNC,
	__CFileSpray_GetDFUWorkunit_entries,
	sizeof(__CFileSpray_GetDFUWorkunit_struct),
	3,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x10852992,
	"http://webservices.seisint.com/FileSpray",
	L"http://webservices.seisint.com/FileSpray",
	sizeof("http://webservices.seisint.com/FileSpray")-1
};


struct __CFileSpray_EchoDateTime_struct
{
	BSTR dt;
	BSTR result;
};

extern __declspec(selectany) const _soapmapentry __CFileSpray_EchoDateTime_entries[] =
{

	{
		0x00000D58, 
		"dt", 
		L"dt", 
		sizeof("dt")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_EchoDateTime_struct, dt),
		NULL,
		NULL,
		-1,
	},
	{
		0x1150D23F, 
		"result", 
		L"result", 
		sizeof("result")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_EchoDateTime_struct, result),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CFileSpray_EchoDateTime_map =
{
	0xA6F7A7FB,
	"EchoDateTime",
	L"EchoDateTimeResponse",
	sizeof("EchoDateTime")-1,
	sizeof("EchoDateTimeResponse")-1,
	SOAPMAP_FUNC,
	__CFileSpray_EchoDateTime_entries,
	sizeof(__CFileSpray_EchoDateTime_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x10852992,
	"http://webservices.seisint.com/FileSpray",
	L"http://webservices.seisint.com/FileSpray",
	sizeof("http://webservices.seisint.com/FileSpray")-1
};


struct __CFileSpray_Dkc_struct
{
	BSTR srcname;
	BSTR dstip;
	BSTR dstfile;
	bool overwrite;
	int maxConnections;
	bool norecover;
	EspException *Exceptions;
	int __Exceptions_nSizeIs;
	BSTR wuid;
};

extern __declspec(selectany) const _soapmapentry __CFileSpray_Dkc_entries[] =
{

	{
		0xA592D909, 
		"srcname", 
		L"srcname", 
		sizeof("srcname")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Dkc_struct, srcname),
		NULL,
		NULL,
		-1,
	},
	{
		0x07529D64, 
		"dstip", 
		L"dstip", 
		sizeof("dstip")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Dkc_struct, dstip),
		NULL,
		NULL,
		-1,
	},
	{
		0x266DD1CB, 
		"dstfile", 
		L"dstfile", 
		sizeof("dstfile")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Dkc_struct, dstfile),
		NULL,
		NULL,
		-1,
	},
	{
		0xD99CE427, 
		"overwrite", 
		L"overwrite", 
		sizeof("overwrite")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Dkc_struct, overwrite),
		NULL,
		NULL,
		-1,
	},
	{
		0xC72090C9, 
		"maxConnections", 
		L"maxConnections", 
		sizeof("maxConnections")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Dkc_struct, maxConnections),
		NULL,
		NULL,
		-1,
	},
	{
		0xB84A4CD3, 
		"norecover", 
		L"norecover", 
		sizeof("norecover")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Dkc_struct, norecover),
		NULL,
		NULL,
		-1,
	},
	{
		0x7B0DBD02, 
		"Exceptions", 
		L"Exceptions", 
		sizeof("Exceptions")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Dkc_struct, Exceptions),
		NULL,
		&__EspException_map,
		6+1,
	},
	{
		0x7B0DBD02,
		"__Exceptions_nSizeIs",
		L"__Exceptions_nSizeIs",
		sizeof("__Exceptions_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CFileSpray_Dkc_struct, __Exceptions_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x004340B9, 
		"wuid", 
		L"wuid", 
		sizeof("wuid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Dkc_struct, wuid),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CFileSpray_Dkc_map =
{
	0x496563A1,
	"Dkc",
	L"DkcResponse",
	sizeof("Dkc")-1,
	sizeof("DkcResponse")-1,
	SOAPMAP_FUNC,
	__CFileSpray_Dkc_entries,
	sizeof(__CFileSpray_Dkc_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x10852992,
	"http://webservices.seisint.com/FileSpray",
	L"http://webservices.seisint.com/FileSpray",
	sizeof("http://webservices.seisint.com/FileSpray")-1
};


struct __CFileSpray_GetDFUWorkunits_struct
{
	BSTR Owner;
	BSTR Cluster;
	BSTR StateReq;
	BSTR Type;
	BSTR Jobname;
	__int64 PageSize;
	int CurrentPage;
	__int64 PageStartFrom;
	BSTR Sortby;
	bool Descending;
	EspException *Exceptions;
	int __Exceptions_nSizeIs;
	DFUWorkunit *results;
	int __results_nSizeIs;
	__int64 PrevPage;
	__int64 NextPage;
	__int64 LastPage;
	__int64 NumWUs;
	__int64 PageEndAt;
	bool First;
	BSTR BasicQuery;
	BSTR Filters;
};

extern __declspec(selectany) const _soapmapentry __CFileSpray_GetDFUWorkunits_entries[] =
{

	{
		0x05D8B24B, 
		"Owner", 
		L"Owner", 
		sizeof("Owner")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_GetDFUWorkunits_struct, Owner),
		NULL,
		NULL,
		-1,
	},
	{
		0x29F147E2, 
		"Cluster", 
		L"Cluster", 
		sizeof("Cluster")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_GetDFUWorkunits_struct, Cluster),
		NULL,
		NULL,
		-1,
	},
	{
		0x656BD869, 
		"StateReq", 
		L"StateReq", 
		sizeof("StateReq")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_GetDFUWorkunits_struct, StateReq),
		NULL,
		NULL,
		-1,
	},
	{
		0x00302162, 
		"Type", 
		L"Type", 
		sizeof("Type")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_GetDFUWorkunits_struct, Type),
		NULL,
		NULL,
		-1,
	},
	{
		0x4A6D8BBC, 
		"Jobname", 
		L"Jobname", 
		sizeof("Jobname")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_GetDFUWorkunits_struct, Jobname),
		NULL,
		NULL,
		-1,
	},
	{
		0xF6F9E158, 
		"PageSize", 
		L"PageSize", 
		sizeof("PageSize")-1, 
		SOAPTYPE_LONG, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_GetDFUWorkunits_struct, PageSize),
		NULL,
		NULL,
		-1,
	},
	{
		0x3C84F320, 
		"CurrentPage", 
		L"CurrentPage", 
		sizeof("CurrentPage")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_GetDFUWorkunits_struct, CurrentPage),
		NULL,
		NULL,
		-1,
	},
	{
		0xA9A72FFF, 
		"PageStartFrom", 
		L"PageStartFrom", 
		sizeof("PageStartFrom")-1, 
		SOAPTYPE_LONG, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_GetDFUWorkunits_struct, PageStartFrom),
		NULL,
		NULL,
		-1,
	},
	{
		0xC9B549E3, 
		"Sortby", 
		L"Sortby", 
		sizeof("Sortby")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_GetDFUWorkunits_struct, Sortby),
		NULL,
		NULL,
		-1,
	},
	{
		0xE1D773F4, 
		"Descending", 
		L"Descending", 
		sizeof("Descending")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_GetDFUWorkunits_struct, Descending),
		NULL,
		NULL,
		-1,
	},
	{
		0x7B0DBD02, 
		"Exceptions", 
		L"Exceptions", 
		sizeof("Exceptions")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_GetDFUWorkunits_struct, Exceptions),
		NULL,
		&__EspException_map,
		10+1,
	},
	{
		0x7B0DBD02,
		"__Exceptions_nSizeIs",
		L"__Exceptions_nSizeIs",
		sizeof("__Exceptions_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CFileSpray_GetDFUWorkunits_struct, __Exceptions_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x3B6B1A92, 
		"results", 
		L"results", 
		sizeof("results")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_GetDFUWorkunits_struct, results),
		NULL,
		&__DFUWorkunit_map,
		12+1,
	},
	{
		0x3B6B1A92,
		"__results_nSizeIs",
		L"__results_nSizeIs",
		sizeof("__results_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CFileSpray_GetDFUWorkunits_struct, __results_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x101F6FDA, 
		"PrevPage", 
		L"PrevPage", 
		sizeof("PrevPage")-1, 
		SOAPTYPE_LONG, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_GetDFUWorkunits_struct, PrevPage),
		NULL,
		NULL,
		-1,
	},
	{
		0x7B14833C, 
		"NextPage", 
		L"NextPage", 
		sizeof("NextPage")-1, 
		SOAPTYPE_LONG, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_GetDFUWorkunits_struct, NextPage),
		NULL,
		NULL,
		-1,
	},
	{
		0x62FE1B51, 
		"LastPage", 
		L"LastPage", 
		sizeof("LastPage")-1, 
		SOAPTYPE_LONG, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_GetDFUWorkunits_struct, LastPage),
		NULL,
		NULL,
		-1,
	},
	{
		0xBE74D6CF, 
		"NumWUs", 
		L"NumWUs", 
		sizeof("NumWUs")-1, 
		SOAPTYPE_LONG, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_GetDFUWorkunits_struct, NumWUs),
		NULL,
		NULL,
		-1,
	},
	{
		0xD53B1169, 
		"PageEndAt", 
		L"PageEndAt", 
		sizeof("PageEndAt")-1, 
		SOAPTYPE_LONG, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_GetDFUWorkunits_struct, PageEndAt),
		NULL,
		NULL,
		-1,
	},
	{
		0x052E3B48, 
		"First", 
		L"First", 
		sizeof("First")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_GetDFUWorkunits_struct, First),
		NULL,
		NULL,
		-1,
	},
	{
		0xCF002458, 
		"BasicQuery", 
		L"BasicQuery", 
		sizeof("BasicQuery")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_GetDFUWorkunits_struct, BasicQuery),
		NULL,
		NULL,
		-1,
	},
	{
		0x093DF5F9, 
		"Filters", 
		L"Filters", 
		sizeof("Filters")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_GetDFUWorkunits_struct, Filters),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CFileSpray_GetDFUWorkunits_map =
{
	0x19E50104,
	"GetDFUWorkunits",
	L"GetDFUWorkunitsResponse",
	sizeof("GetDFUWorkunits")-1,
	sizeof("GetDFUWorkunitsResponse")-1,
	SOAPMAP_FUNC,
	__CFileSpray_GetDFUWorkunits_entries,
	sizeof(__CFileSpray_GetDFUWorkunits_struct),
	18,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x10852992,
	"http://webservices.seisint.com/FileSpray",
	L"http://webservices.seisint.com/FileSpray",
	sizeof("http://webservices.seisint.com/FileSpray")-1
};


struct __CFileSpray_GetDFUExceptions_struct
{
	BSTR wuid;
	EspException *Exceptions;
	int __Exceptions_nSizeIs;
	DFUException *result;
	int __result_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CFileSpray_GetDFUExceptions_entries[] =
{

	{
		0x004340B9, 
		"wuid", 
		L"wuid", 
		sizeof("wuid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_GetDFUExceptions_struct, wuid),
		NULL,
		NULL,
		-1,
	},
	{
		0x7B0DBD02, 
		"Exceptions", 
		L"Exceptions", 
		sizeof("Exceptions")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_GetDFUExceptions_struct, Exceptions),
		NULL,
		&__EspException_map,
		1+1,
	},
	{
		0x7B0DBD02,
		"__Exceptions_nSizeIs",
		L"__Exceptions_nSizeIs",
		sizeof("__Exceptions_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CFileSpray_GetDFUExceptions_struct, __Exceptions_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x1150D23F, 
		"result", 
		L"result", 
		sizeof("result")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_GetDFUExceptions_struct, result),
		NULL,
		&__DFUException_map,
		3+1,
	},
	{
		0x1150D23F,
		"__result_nSizeIs",
		L"__result_nSizeIs",
		sizeof("__result_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CFileSpray_GetDFUExceptions_struct, __result_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CFileSpray_GetDFUExceptions_map =
{
	0x89551910,
	"GetDFUExceptions",
	L"GetDFUExceptionsResponse",
	sizeof("GetDFUExceptions")-1,
	sizeof("GetDFUExceptionsResponse")-1,
	SOAPMAP_FUNC,
	__CFileSpray_GetDFUExceptions_entries,
	sizeof(__CFileSpray_GetDFUExceptions_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x10852992,
	"http://webservices.seisint.com/FileSpray",
	L"http://webservices.seisint.com/FileSpray",
	sizeof("http://webservices.seisint.com/FileSpray")-1
};


struct __CFileSpray_FileList_struct
{
	BSTR Netaddr;
	BSTR Path;
	BSTR Mask;
	BSTR OS;
	bool DirectoryOnly;
	EspException *Exceptions;
	int __Exceptions_nSizeIs;
	PhysicalFileStruct *files;
	int __files_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CFileSpray_FileList_entries[] =
{

	{
		0x68418C42, 
		"Netaddr", 
		L"Netaddr", 
		sizeof("Netaddr")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_FileList_struct, Netaddr),
		NULL,
		NULL,
		-1,
	},
	{
		0x002D8A4D, 
		"Path", 
		L"Path", 
		sizeof("Path")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_FileList_struct, Path),
		NULL,
		NULL,
		-1,
	},
	{
		0x002BE50C, 
		"Mask", 
		L"Mask", 
		sizeof("Mask")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_FileList_struct, Mask),
		NULL,
		NULL,
		-1,
	},
	{
		0x00000A82, 
		"OS", 
		L"OS", 
		sizeof("OS")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_FileList_struct, OS),
		NULL,
		NULL,
		-1,
	},
	{
		0xB19F2DF7, 
		"DirectoryOnly", 
		L"DirectoryOnly", 
		sizeof("DirectoryOnly")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_FileList_struct, DirectoryOnly),
		NULL,
		NULL,
		-1,
	},
	{
		0x7B0DBD02, 
		"Exceptions", 
		L"Exceptions", 
		sizeof("Exceptions")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_FileList_struct, Exceptions),
		NULL,
		&__EspException_map,
		5+1,
	},
	{
		0x7B0DBD02,
		"__Exceptions_nSizeIs",
		L"__Exceptions_nSizeIs",
		sizeof("__Exceptions_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CFileSpray_FileList_struct, __Exceptions_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x07713013, 
		"files", 
		L"files", 
		sizeof("files")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_FileList_struct, files),
		NULL,
		&__PhysicalFileStruct_map,
		7+1,
	},
	{
		0x07713013,
		"__files_nSizeIs",
		L"__files_nSizeIs",
		sizeof("__files_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CFileSpray_FileList_struct, __files_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CFileSpray_FileList_map =
{
	0x930EC86B,
	"FileListRequest",
	L"FileListResponse",
	sizeof("FileListRequest")-1,
	sizeof("FileListResponse")-1,
	SOAPMAP_FUNC,
	__CFileSpray_FileList_entries,
	sizeof(__CFileSpray_FileList_struct),
	7,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x10852992,
	"http://webservices.seisint.com/FileSpray",
	L"http://webservices.seisint.com/FileSpray",
	sizeof("http://webservices.seisint.com/FileSpray")-1
};


struct __CFileSpray_CreateDFUWorkunit_struct
{
	EspException *Exceptions;
	int __Exceptions_nSizeIs;
	DFUWorkunit result;
};

extern __declspec(selectany) const _soapmapentry __CFileSpray_CreateDFUWorkunit_entries[] =
{

	{
		0x7B0DBD02, 
		"Exceptions", 
		L"Exceptions", 
		sizeof("Exceptions")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_CreateDFUWorkunit_struct, Exceptions),
		NULL,
		&__EspException_map,
		0+1,
	},
	{
		0x7B0DBD02,
		"__Exceptions_nSizeIs",
		L"__Exceptions_nSizeIs",
		sizeof("__Exceptions_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CFileSpray_CreateDFUWorkunit_struct, __Exceptions_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x1150D23F, 
		"result", 
		L"result", 
		sizeof("result")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_CreateDFUWorkunit_struct, result),
		NULL,
		&__DFUWorkunit_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CFileSpray_CreateDFUWorkunit_map =
{
	0x79E22F25,
	"CreateDFUWorkunit",
	L"CreateDFUWorkunitResponse",
	sizeof("CreateDFUWorkunit")-1,
	sizeof("CreateDFUWorkunitResponse")-1,
	SOAPMAP_FUNC,
	__CFileSpray_CreateDFUWorkunit_entries,
	sizeof(__CFileSpray_CreateDFUWorkunit_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x10852992,
	"http://webservices.seisint.com/FileSpray",
	L"http://webservices.seisint.com/FileSpray",
	sizeof("http://webservices.seisint.com/FileSpray")-1
};


struct __CFileSpray_SubmitDFUWorkunit_struct
{
	BSTR wuid;
	EspException *Exceptions;
	int __Exceptions_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CFileSpray_SubmitDFUWorkunit_entries[] =
{

	{
		0x004340B9, 
		"wuid", 
		L"wuid", 
		sizeof("wuid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_SubmitDFUWorkunit_struct, wuid),
		NULL,
		NULL,
		-1,
	},
	{
		0x7B0DBD02, 
		"Exceptions", 
		L"Exceptions", 
		sizeof("Exceptions")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_SubmitDFUWorkunit_struct, Exceptions),
		NULL,
		&__EspException_map,
		1+1,
	},
	{
		0x7B0DBD02,
		"__Exceptions_nSizeIs",
		L"__Exceptions_nSizeIs",
		sizeof("__Exceptions_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CFileSpray_SubmitDFUWorkunit_struct, __Exceptions_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CFileSpray_SubmitDFUWorkunit_map =
{
	0x2A535B45,
	"SubmitDFUWorkunit",
	L"SubmitDFUWorkunitResponse",
	sizeof("SubmitDFUWorkunit")-1,
	sizeof("SubmitDFUWorkunitResponse")-1,
	SOAPMAP_FUNC,
	__CFileSpray_SubmitDFUWorkunit_entries,
	sizeof(__CFileSpray_SubmitDFUWorkunit_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x10852992,
	"http://webservices.seisint.com/FileSpray",
	L"http://webservices.seisint.com/FileSpray",
	sizeof("http://webservices.seisint.com/FileSpray")-1
};


struct __CFileSpray_DFUWorkunitsAction_struct
{
	BSTR *wuids;
	int __wuids_nSizeIs;
	BSTR Type;
	EspException *Exceptions;
	int __Exceptions_nSizeIs;
	DFUActionResult *DFUActionResults;
	int __DFUActionResults_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CFileSpray_DFUWorkunitsAction_entries[] =
{

	{
		0x08AB584C, 
		"wuids", 
		L"wuids", 
		sizeof("wuids")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_DFUWorkunitsAction_struct, wuids),
		NULL,
		NULL,
		0+1,
	},
	{
		0x08AB584C,
		"__wuids_nSizeIs",
		L"__wuids_nSizeIs",
		sizeof("__wuids_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CFileSpray_DFUWorkunitsAction_struct, __wuids_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x00302162, 
		"Type", 
		L"Type", 
		sizeof("Type")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_DFUWorkunitsAction_struct, Type),
		NULL,
		NULL,
		-1,
	},
	{
		0x7B0DBD02, 
		"Exceptions", 
		L"Exceptions", 
		sizeof("Exceptions")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_DFUWorkunitsAction_struct, Exceptions),
		NULL,
		&__EspException_map,
		3+1,
	},
	{
		0x7B0DBD02,
		"__Exceptions_nSizeIs",
		L"__Exceptions_nSizeIs",
		sizeof("__Exceptions_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CFileSpray_DFUWorkunitsAction_struct, __Exceptions_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x5DF15DCF, 
		"DFUActionResults", 
		L"DFUActionResults", 
		sizeof("DFUActionResults")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_DFUWorkunitsAction_struct, DFUActionResults),
		NULL,
		&__DFUActionResult_map,
		5+1,
	},
	{
		0x5DF15DCF,
		"__DFUActionResults_nSizeIs",
		L"__DFUActionResults_nSizeIs",
		sizeof("__DFUActionResults_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CFileSpray_DFUWorkunitsAction_struct, __DFUActionResults_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CFileSpray_DFUWorkunitsAction_map =
{
	0x40F8E362,
	"DFUWorkunitsActionRequest",
	L"DFUWorkunitsActionResponse",
	sizeof("DFUWorkunitsActionRequest")-1,
	sizeof("DFUWorkunitsActionResponse")-1,
	SOAPMAP_FUNC,
	__CFileSpray_DFUWorkunitsAction_entries,
	sizeof(__CFileSpray_DFUWorkunitsAction_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x10852992,
	"http://webservices.seisint.com/FileSpray",
	L"http://webservices.seisint.com/FileSpray",
	sizeof("http://webservices.seisint.com/FileSpray")-1
};


struct __CFileSpray_UpdateDFUWorkunit_struct
{
	DFUWorkunit wu;
	BSTR ClusterOrig;
	BSTR JobNameOrig;
	bool isProtectedOrig;
	int StateOrig;
	EspException *Exceptions;
	int __Exceptions_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CFileSpray_UpdateDFUWorkunit_entries[] =
{

	{
		0x00000FCC, 
		"wu", 
		L"wu", 
		sizeof("wu")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_UpdateDFUWorkunit_struct, wu),
		NULL,
		&__DFUWorkunit_map,
		-1,
	},
	{
		0xCF63ADB3, 
		"ClusterOrig", 
		L"ClusterOrig", 
		sizeof("ClusterOrig")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_UpdateDFUWorkunit_struct, ClusterOrig),
		NULL,
		NULL,
		-1,
	},
	{
		0x1177B26D, 
		"JobNameOrig", 
		L"JobNameOrig", 
		sizeof("JobNameOrig")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_UpdateDFUWorkunit_struct, JobNameOrig),
		NULL,
		NULL,
		-1,
	},
	{
		0xFEFE1DB7, 
		"isProtectedOrig", 
		L"isProtectedOrig", 
		sizeof("isProtectedOrig")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_UpdateDFUWorkunit_struct, isProtectedOrig),
		NULL,
		NULL,
		-1,
	},
	{
		0x12E57712, 
		"StateOrig", 
		L"StateOrig", 
		sizeof("StateOrig")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_UpdateDFUWorkunit_struct, StateOrig),
		NULL,
		NULL,
		-1,
	},
	{
		0x7B0DBD02, 
		"Exceptions", 
		L"Exceptions", 
		sizeof("Exceptions")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_UpdateDFUWorkunit_struct, Exceptions),
		NULL,
		&__EspException_map,
		5+1,
	},
	{
		0x7B0DBD02,
		"__Exceptions_nSizeIs",
		L"__Exceptions_nSizeIs",
		sizeof("__Exceptions_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CFileSpray_UpdateDFUWorkunit_struct, __Exceptions_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CFileSpray_UpdateDFUWorkunit_map =
{
	0x2D8F78B4,
	"UpdateDFUWorkunit",
	L"UpdateDFUWorkunitResponse",
	sizeof("UpdateDFUWorkunit")-1,
	sizeof("UpdateDFUWorkunitResponse")-1,
	SOAPMAP_FUNC,
	__CFileSpray_UpdateDFUWorkunit_entries,
	sizeof(__CFileSpray_UpdateDFUWorkunit_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x10852992,
	"http://webservices.seisint.com/FileSpray",
	L"http://webservices.seisint.com/FileSpray",
	sizeof("http://webservices.seisint.com/FileSpray")-1
};


struct __CFileSpray_Replicate_struct
{
	BSTR sourceLogicalName;
	int replicateOffset;
	BSTR cluster;
	bool repeatLast;
	bool onlyRepeated;
	EspException *Exceptions;
	int __Exceptions_nSizeIs;
	BSTR wuid;
};

extern __declspec(selectany) const _soapmapentry __CFileSpray_Replicate_entries[] =
{

	{
		0x12F4790D, 
		"sourceLogicalName", 
		L"sourceLogicalName", 
		sizeof("sourceLogicalName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Replicate_struct, sourceLogicalName),
		NULL,
		NULL,
		-1,
	},
	{
		0x08EE1320, 
		"replicateOffset", 
		L"replicateOffset", 
		sizeof("replicateOffset")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Replicate_struct, replicateOffset),
		NULL,
		NULL,
		-1,
	},
	{
		0xC938E002, 
		"cluster", 
		L"cluster", 
		sizeof("cluster")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Replicate_struct, cluster),
		NULL,
		NULL,
		-1,
	},
	{
		0x67F728D5, 
		"repeatLast", 
		L"repeatLast", 
		sizeof("repeatLast")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Replicate_struct, repeatLast),
		NULL,
		NULL,
		-1,
	},
	{
		0x630EC78C, 
		"onlyRepeated", 
		L"onlyRepeated", 
		sizeof("onlyRepeated")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Replicate_struct, onlyRepeated),
		NULL,
		NULL,
		-1,
	},
	{
		0x7B0DBD02, 
		"Exceptions", 
		L"Exceptions", 
		sizeof("Exceptions")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Replicate_struct, Exceptions),
		NULL,
		&__EspException_map,
		5+1,
	},
	{
		0x7B0DBD02,
		"__Exceptions_nSizeIs",
		L"__Exceptions_nSizeIs",
		sizeof("__Exceptions_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CFileSpray_Replicate_struct, __Exceptions_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x004340B9, 
		"wuid", 
		L"wuid", 
		sizeof("wuid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Replicate_struct, wuid),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CFileSpray_Replicate_map =
{
	0x37709508,
	"Replicate",
	L"ReplicateResponse",
	sizeof("Replicate")-1,
	sizeof("ReplicateResponse")-1,
	SOAPMAP_FUNC,
	__CFileSpray_Replicate_entries,
	sizeof(__CFileSpray_Replicate_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x10852992,
	"http://webservices.seisint.com/FileSpray",
	L"http://webservices.seisint.com/FileSpray",
	sizeof("http://webservices.seisint.com/FileSpray")-1
};


struct __CFileSpray_Rename_struct
{
	BSTR srcname;
	BSTR dstname;
	bool overwrite;
	EspException *Exceptions;
	int __Exceptions_nSizeIs;
	BSTR wuid;
};

extern __declspec(selectany) const _soapmapentry __CFileSpray_Rename_entries[] =
{

	{
		0xA592D909, 
		"srcname", 
		L"srcname", 
		sizeof("srcname")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Rename_struct, srcname),
		NULL,
		NULL,
		-1,
	},
	{
		0x267212EC, 
		"dstname", 
		L"dstname", 
		sizeof("dstname")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Rename_struct, dstname),
		NULL,
		NULL,
		-1,
	},
	{
		0xD99CE427, 
		"overwrite", 
		L"overwrite", 
		sizeof("overwrite")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Rename_struct, overwrite),
		NULL,
		NULL,
		-1,
	},
	{
		0x7B0DBD02, 
		"Exceptions", 
		L"Exceptions", 
		sizeof("Exceptions")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Rename_struct, Exceptions),
		NULL,
		&__EspException_map,
		3+1,
	},
	{
		0x7B0DBD02,
		"__Exceptions_nSizeIs",
		L"__Exceptions_nSizeIs",
		sizeof("__Exceptions_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CFileSpray_Rename_struct, __Exceptions_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x004340B9, 
		"wuid", 
		L"wuid", 
		sizeof("wuid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_Rename_struct, wuid),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CFileSpray_Rename_map =
{
	0xA9974567,
	"Rename",
	L"RenameResponse",
	sizeof("Rename")-1,
	sizeof("RenameResponse")-1,
	SOAPMAP_FUNC,
	__CFileSpray_Rename_entries,
	sizeof(__CFileSpray_Rename_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x10852992,
	"http://webservices.seisint.com/FileSpray",
	L"http://webservices.seisint.com/FileSpray",
	sizeof("http://webservices.seisint.com/FileSpray")-1
};


struct __CFileSpray_DeleteDFUWorkunit_struct
{
	BSTR wuid;
	EspException *Exceptions;
	int __Exceptions_nSizeIs;
	bool result;
};

extern __declspec(selectany) const _soapmapentry __CFileSpray_DeleteDFUWorkunit_entries[] =
{

	{
		0x004340B9, 
		"wuid", 
		L"wuid", 
		sizeof("wuid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_DeleteDFUWorkunit_struct, wuid),
		NULL,
		NULL,
		-1,
	},
	{
		0x7B0DBD02, 
		"Exceptions", 
		L"Exceptions", 
		sizeof("Exceptions")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_DeleteDFUWorkunit_struct, Exceptions),
		NULL,
		&__EspException_map,
		1+1,
	},
	{
		0x7B0DBD02,
		"__Exceptions_nSizeIs",
		L"__Exceptions_nSizeIs",
		sizeof("__Exceptions_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CFileSpray_DeleteDFUWorkunit_struct, __Exceptions_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x1150D23F, 
		"result", 
		L"result", 
		sizeof("result")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_DeleteDFUWorkunit_struct, result),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CFileSpray_DeleteDFUWorkunit_map =
{
	0xE847FA84,
	"DeleteDFUWorkunit",
	L"DeleteDFUWorkunitResponse",
	sizeof("DeleteDFUWorkunit")-1,
	sizeof("DeleteDFUWorkunitResponse")-1,
	SOAPMAP_FUNC,
	__CFileSpray_DeleteDFUWorkunit_entries,
	sizeof(__CFileSpray_DeleteDFUWorkunit_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x10852992,
	"http://webservices.seisint.com/FileSpray",
	L"http://webservices.seisint.com/FileSpray",
	sizeof("http://webservices.seisint.com/FileSpray")-1
};


struct __CFileSpray_DfuMonitor_struct
{
	BSTR EventName;
	BSTR LogicalName;
	BSTR Ip;
	BSTR Filename;
	bool Sub;
	int ShotLimit;
	EspException *Exceptions;
	int __Exceptions_nSizeIs;
	BSTR wuid;
};

extern __declspec(selectany) const _soapmapentry __CFileSpray_DfuMonitor_entries[] =
{

	{
		0xB3938D63, 
		"EventName", 
		L"EventName", 
		sizeof("EventName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_DfuMonitor_struct, EventName),
		NULL,
		NULL,
		-1,
	},
	{
		0xCC052EFC, 
		"LogicalName", 
		L"LogicalName", 
		sizeof("LogicalName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_DfuMonitor_struct, LogicalName),
		NULL,
		NULL,
		-1,
	},
	{
		0x000009D9, 
		"Ip", 
		L"Ip", 
		sizeof("Ip")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_DfuMonitor_struct, Ip),
		NULL,
		NULL,
		-1,
	},
	{
		0x2FF1EC41, 
		"Filename", 
		L"Filename", 
		sizeof("Filename")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_DfuMonitor_struct, Filename),
		NULL,
		NULL,
		-1,
	},
	{
		0x0001708A, 
		"Sub", 
		L"Sub", 
		sizeof("Sub")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_DfuMonitor_struct, Sub),
		NULL,
		NULL,
		-1,
	},
	{
		0x33C8971D, 
		"ShotLimit", 
		L"ShotLimit", 
		sizeof("ShotLimit")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_DfuMonitor_struct, ShotLimit),
		NULL,
		NULL,
		-1,
	},
	{
		0x7B0DBD02, 
		"Exceptions", 
		L"Exceptions", 
		sizeof("Exceptions")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_DfuMonitor_struct, Exceptions),
		NULL,
		&__EspException_map,
		6+1,
	},
	{
		0x7B0DBD02,
		"__Exceptions_nSizeIs",
		L"__Exceptions_nSizeIs",
		sizeof("__Exceptions_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CFileSpray_DfuMonitor_struct, __Exceptions_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x004340B9, 
		"wuid", 
		L"wuid", 
		sizeof("wuid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_DfuMonitor_struct, wuid),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CFileSpray_DfuMonitor_map =
{
	0x7F341276,
	"DfuMonitorRequest",
	L"DfuMonitorResponse",
	sizeof("DfuMonitorRequest")-1,
	sizeof("DfuMonitorResponse")-1,
	SOAPMAP_FUNC,
	__CFileSpray_DfuMonitor_entries,
	sizeof(__CFileSpray_DfuMonitor_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x10852992,
	"http://webservices.seisint.com/FileSpray",
	L"http://webservices.seisint.com/FileSpray",
	sizeof("http://webservices.seisint.com/FileSpray")-1
};


struct __CFileSpray_AbortDFUWorkunit_struct
{
	BSTR wuid;
	EspException *Exceptions;
	int __Exceptions_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CFileSpray_AbortDFUWorkunit_entries[] =
{

	{
		0x004340B9, 
		"wuid", 
		L"wuid", 
		sizeof("wuid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_AbortDFUWorkunit_struct, wuid),
		NULL,
		NULL,
		-1,
	},
	{
		0x7B0DBD02, 
		"Exceptions", 
		L"Exceptions", 
		sizeof("Exceptions")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CFileSpray_AbortDFUWorkunit_struct, Exceptions),
		NULL,
		&__EspException_map,
		1+1,
	},
	{
		0x7B0DBD02,
		"__Exceptions_nSizeIs",
		L"__Exceptions_nSizeIs",
		sizeof("__Exceptions_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CFileSpray_AbortDFUWorkunit_struct, __Exceptions_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CFileSpray_AbortDFUWorkunit_map =
{
	0x15D1B009,
	"AbortDFUWorkunit",
	L"AbortDFUWorkunitResponse",
	sizeof("AbortDFUWorkunit")-1,
	sizeof("AbortDFUWorkunitResponse")-1,
	SOAPMAP_FUNC,
	__CFileSpray_AbortDFUWorkunit_entries,
	sizeof(__CFileSpray_AbortDFUWorkunit_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x10852992,
	"http://webservices.seisint.com/FileSpray",
	L"http://webservices.seisint.com/FileSpray",
	sizeof("http://webservices.seisint.com/FileSpray")-1
};

extern __declspec(selectany) const _soapmap * __CFileSpray_funcs[] =
{
	&__CFileSpray_GetDFUProgress_map,
	&__CFileSpray_SprayVariable_map,
	&__CFileSpray_SprayFixed_map,
	&__CFileSpray_Despray_map,
	&__CFileSpray_DFUWUSearch_map,
	&__CFileSpray_Copy_map,
	&__CFileSpray_DFUWUFile_map,
	&__CFileSpray_DeleteDFUWorkunits_map,
	&__CFileSpray_ShowResult_map,
	&__CFileSpray_GetDFUWorkunit_map,
	&__CFileSpray_EchoDateTime_map,
	&__CFileSpray_Dkc_map,
	&__CFileSpray_GetDFUWorkunits_map,
	&__CFileSpray_GetDFUExceptions_map,
	&__CFileSpray_FileList_map,
	&__CFileSpray_CreateDFUWorkunit_map,
	&__CFileSpray_SubmitDFUWorkunit_map,
	&__CFileSpray_DFUWorkunitsAction_map,
	&__CFileSpray_UpdateDFUWorkunit_map,
	&__CFileSpray_Replicate_map,
	&__CFileSpray_Rename_map,
	&__CFileSpray_DeleteDFUWorkunit_map,
	&__CFileSpray_DfuMonitor_map,
	&__CFileSpray_AbortDFUWorkunit_map,
	NULL
};

template <typename TClient>
inline HRESULT CFileSprayT<TClient>::GetDFUProgress(
		BSTR* wuid, 
		EspException** Exceptions, int* __Exceptions_nSizeIs, 
		int* PercentDone, 
		int* SecsLeft, 
		int* KbPerSecAve, 
		int* KbPerSec, 
		int* SlavesDone, 
		BSTR* TimeTaken, 
		BSTR* ProgressMessage, 
		BSTR* SummaryMessage, 
		BSTR* State
	)
{
    if ( wuid == NULL )
		return E_POINTER;
    if ( Exceptions == NULL )
		return E_POINTER;
	if( __Exceptions_nSizeIs == NULL )
		return E_POINTER;
    if ( PercentDone == NULL )
		return E_POINTER;
    if ( SecsLeft == NULL )
		return E_POINTER;
    if ( KbPerSecAve == NULL )
		return E_POINTER;
    if ( KbPerSec == NULL )
		return E_POINTER;
    if ( SlavesDone == NULL )
		return E_POINTER;
    if ( TimeTaken == NULL )
		return E_POINTER;
    if ( ProgressMessage == NULL )
		return E_POINTER;
    if ( SummaryMessage == NULL )
		return E_POINTER;
    if ( State == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CFileSpray_GetDFUProgress_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.wuid = *wuid;

	__atlsoap_hr = SetClientStruct(&__params, 0);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"FileSpray/GetDFUProgress\?ver_=1.03\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*wuid = __params.wuid;
	*Exceptions = __params.Exceptions;
	*__Exceptions_nSizeIs = __params.__Exceptions_nSizeIs;
	*PercentDone = __params.PercentDone;
	*SecsLeft = __params.SecsLeft;
	*KbPerSecAve = __params.KbPerSecAve;
	*KbPerSec = __params.KbPerSec;
	*SlavesDone = __params.SlavesDone;
	*TimeTaken = __params.TimeTaken;
	*ProgressMessage = __params.ProgressMessage;
	*SummaryMessage = __params.SummaryMessage;
	*State = __params.State;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CFileSprayT<TClient>::SprayVariable(
		BSTR sourceIP, 
		BSTR sourcePath, 
		ATLSOAP_BLOB srcxml, 
		int sourceMaxRecordSize, 
		int sourceFormat, 
		BSTR sourceCsvSeparate, 
		BSTR sourceCsvTerminate, 
		BSTR sourceCsvQuote, 
		BSTR sourceRowTag, 
		BSTR destGroup, 
		BSTR destLogicalName, 
		bool overwrite, 
		bool replicate, 
		int ReplicateOffset, 
		int maxConnections, 
		int throttle, 
		int transferBufferSize, 
		BSTR prefix, 
		bool nosplit, 
		bool norecover, 
		bool compress, 
		bool push, 
		bool pull, 
		BSTR encrypt, 
		BSTR decrypt, 
		EspException** Exceptions, int* __Exceptions_nSizeIs, 
		BSTR* wuid
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
	if( __Exceptions_nSizeIs == NULL )
		return E_POINTER;
    if ( wuid == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CFileSpray_SprayVariable_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sourceIP = sourceIP;
	__params.sourcePath = sourcePath;
	__params.srcxml = srcxml;
	__params.sourceMaxRecordSize = sourceMaxRecordSize;
	__params.sourceFormat = sourceFormat;
	__params.sourceCsvSeparate = sourceCsvSeparate;
	__params.sourceCsvTerminate = sourceCsvTerminate;
	__params.sourceCsvQuote = sourceCsvQuote;
	__params.sourceRowTag = sourceRowTag;
	__params.destGroup = destGroup;
	__params.destLogicalName = destLogicalName;
	__params.overwrite = overwrite;
	__params.replicate = replicate;
	__params.ReplicateOffset = ReplicateOffset;
	__params.maxConnections = maxConnections;
	__params.throttle = throttle;
	__params.transferBufferSize = transferBufferSize;
	__params.prefix = prefix;
	__params.nosplit = nosplit;
	__params.norecover = norecover;
	__params.compress = compress;
	__params.push = push;
	__params.pull = pull;
	__params.encrypt = encrypt;
	__params.decrypt = decrypt;

	__atlsoap_hr = SetClientStruct(&__params, 1);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"FileSpray/SprayVariable\?ver_=1.03\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*Exceptions = __params.Exceptions;
	*__Exceptions_nSizeIs = __params.__Exceptions_nSizeIs;
	*wuid = __params.wuid;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CFileSprayT<TClient>::SprayFixed(
		BSTR sourceIP, 
		BSTR sourcePath, 
		ATLSOAP_BLOB srcxml, 
		int sourceRecordSize, 
		BSTR destGroup, 
		BSTR destLogicalName, 
		bool overwrite, 
		bool replicate, 
		int ReplicateOffset, 
		int maxConnections, 
		int throttle, 
		int transferBufferSize, 
		BSTR prefix, 
		bool nosplit, 
		bool norecover, 
		bool compress, 
		bool push, 
		bool pull, 
		BSTR encrypt, 
		BSTR decrypt, 
		EspException** Exceptions, int* __Exceptions_nSizeIs, 
		BSTR* wuid
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
	if( __Exceptions_nSizeIs == NULL )
		return E_POINTER;
    if ( wuid == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CFileSpray_SprayFixed_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sourceIP = sourceIP;
	__params.sourcePath = sourcePath;
	__params.srcxml = srcxml;
	__params.sourceRecordSize = sourceRecordSize;
	__params.destGroup = destGroup;
	__params.destLogicalName = destLogicalName;
	__params.overwrite = overwrite;
	__params.replicate = replicate;
	__params.ReplicateOffset = ReplicateOffset;
	__params.maxConnections = maxConnections;
	__params.throttle = throttle;
	__params.transferBufferSize = transferBufferSize;
	__params.prefix = prefix;
	__params.nosplit = nosplit;
	__params.norecover = norecover;
	__params.compress = compress;
	__params.push = push;
	__params.pull = pull;
	__params.encrypt = encrypt;
	__params.decrypt = decrypt;

	__atlsoap_hr = SetClientStruct(&__params, 2);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"FileSpray/SprayFixed\?ver_=1.03\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*Exceptions = __params.Exceptions;
	*__Exceptions_nSizeIs = __params.__Exceptions_nSizeIs;
	*wuid = __params.wuid;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CFileSprayT<TClient>::Despray(
		BSTR sourceLogicalName, 
		BSTR destIP, 
		BSTR destPath, 
		ATLSOAP_BLOB dstxml, 
		bool overwrite, 
		int maxConnections, 
		int throttle, 
		int transferBufferSize, 
		BSTR splitprefix, 
		bool norecover, 
		bool wrap, 
		bool multiCopy, 
		bool SingleConnection, 
		bool compress, 
		BSTR encrypt, 
		BSTR decrypt, 
		EspException** Exceptions, int* __Exceptions_nSizeIs, 
		BSTR* wuid
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
	if( __Exceptions_nSizeIs == NULL )
		return E_POINTER;
    if ( wuid == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CFileSpray_Despray_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sourceLogicalName = sourceLogicalName;
	__params.destIP = destIP;
	__params.destPath = destPath;
	__params.dstxml = dstxml;
	__params.overwrite = overwrite;
	__params.maxConnections = maxConnections;
	__params.throttle = throttle;
	__params.transferBufferSize = transferBufferSize;
	__params.splitprefix = splitprefix;
	__params.norecover = norecover;
	__params.wrap = wrap;
	__params.multiCopy = multiCopy;
	__params.SingleConnection = SingleConnection;
	__params.compress = compress;
	__params.encrypt = encrypt;
	__params.decrypt = decrypt;

	__atlsoap_hr = SetClientStruct(&__params, 3);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"FileSpray/Despray\?ver_=1.03\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*Exceptions = __params.Exceptions;
	*__Exceptions_nSizeIs = __params.__Exceptions_nSizeIs;
	*wuid = __params.wuid;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CFileSprayT<TClient>::DFUWUSearch(
		EspException** Exceptions, int* __Exceptions_nSizeIs, 
		BSTR** ClusterNames, int* __ClusterNames_nSizeIs
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
	if( __Exceptions_nSizeIs == NULL )
		return E_POINTER;
    if ( ClusterNames == NULL )
		return E_POINTER;
	if( __ClusterNames_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CFileSpray_DFUWUSearch_struct __params;
	memset(&__params, 0x00, sizeof(__params));

	__atlsoap_hr = SetClientStruct(&__params, 4);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"FileSpray/DFUWUSearch\?ver_=1.03\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*Exceptions = __params.Exceptions;
	*__Exceptions_nSizeIs = __params.__Exceptions_nSizeIs;
	*ClusterNames = __params.ClusterNames;
	*__ClusterNames_nSizeIs = __params.__ClusterNames_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CFileSprayT<TClient>::Copy(
		BSTR sourceLogicalName, 
		BSTR destGroup, 
		BSTR destGroupRoxie, 
		BSTR destLogicalName, 
		BSTR sourceDali, 
		BSTR srcusername, 
		BSTR srcpassword, 
		bool overwrite, 
		bool replicate, 
		int ReplicateOffset, 
		int maxConnections, 
		int throttle, 
		int transferBufferSize, 
		bool nosplit, 
		bool norecover, 
		bool compress, 
		bool Wrap, 
		bool Multicopy, 
		BSTR SourceDiffKeyName, 
		BSTR DestDiffKeyName, 
		bool superCopy, 
		bool push, 
		bool pull, 
		bool ifnewer, 
		BSTR encrypt, 
		BSTR decrypt, 
		EspException** Exceptions, int* __Exceptions_nSizeIs, 
		BSTR* result
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
	if( __Exceptions_nSizeIs == NULL )
		return E_POINTER;
    if ( result == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CFileSpray_Copy_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sourceLogicalName = sourceLogicalName;
	__params.destGroup = destGroup;
	__params.destGroupRoxie = destGroupRoxie;
	__params.destLogicalName = destLogicalName;
	__params.sourceDali = sourceDali;
	__params.srcusername = srcusername;
	__params.srcpassword = srcpassword;
	__params.overwrite = overwrite;
	__params.replicate = replicate;
	__params.ReplicateOffset = ReplicateOffset;
	__params.maxConnections = maxConnections;
	__params.throttle = throttle;
	__params.transferBufferSize = transferBufferSize;
	__params.nosplit = nosplit;
	__params.norecover = norecover;
	__params.compress = compress;
	__params.Wrap = Wrap;
	__params.Multicopy = Multicopy;
	__params.SourceDiffKeyName = SourceDiffKeyName;
	__params.DestDiffKeyName = DestDiffKeyName;
	__params.superCopy = superCopy;
	__params.push = push;
	__params.pull = pull;
	__params.ifnewer = ifnewer;
	__params.encrypt = encrypt;
	__params.decrypt = decrypt;

	__atlsoap_hr = SetClientStruct(&__params, 5);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"FileSpray/Copy\?ver_=1.03\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*Exceptions = __params.Exceptions;
	*__Exceptions_nSizeIs = __params.__Exceptions_nSizeIs;
	*result = __params.result;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CFileSprayT<TClient>::DFUWUFile(
		BSTR Wuid, 
		BSTR Type, 
		EspException** Exceptions, int* __Exceptions_nSizeIs, 
		BSTR* file
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
	if( __Exceptions_nSizeIs == NULL )
		return E_POINTER;
    if ( file == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CFileSpray_DFUWUFile_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Wuid = Wuid;
	__params.Type = Type;

	__atlsoap_hr = SetClientStruct(&__params, 6);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"FileSpray/DFUWUFile\?ver_=1.03\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*Exceptions = __params.Exceptions;
	*__Exceptions_nSizeIs = __params.__Exceptions_nSizeIs;
	*file = __params.file;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CFileSprayT<TClient>::DeleteDFUWorkunits(
		BSTR* wuids, int __wuids_nSizeIs, 
		EspException** Exceptions, int* __Exceptions_nSizeIs
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
	if( __Exceptions_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CFileSpray_DeleteDFUWorkunits_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.wuids = wuids;
	__params.__wuids_nSizeIs = __wuids_nSizeIs;

	__atlsoap_hr = SetClientStruct(&__params, 7);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"FileSpray/DeleteDFUWorkunits\?ver_=1.03\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*Exceptions = __params.Exceptions;
	*__Exceptions_nSizeIs = __params.__Exceptions_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CFileSprayT<TClient>::ShowResult(
		BSTR* Result, 
		EspException** Exceptions, int* __Exceptions_nSizeIs
	)
{
    if ( Result == NULL )
		return E_POINTER;
    if ( Exceptions == NULL )
		return E_POINTER;
	if( __Exceptions_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CFileSpray_ShowResult_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Result = *Result;

	__atlsoap_hr = SetClientStruct(&__params, 8);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"FileSpray/ShowResult\?ver_=1.03\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*Result = __params.Result;
	*Exceptions = __params.Exceptions;
	*__Exceptions_nSizeIs = __params.__Exceptions_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CFileSprayT<TClient>::GetDFUWorkunit(
		BSTR wuid, 
		EspException** Exceptions, int* __Exceptions_nSizeIs, 
		DFUWorkunit* result, 
		int* AutoRefresh
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
	if( __Exceptions_nSizeIs == NULL )
		return E_POINTER;
    if ( result == NULL )
		return E_POINTER;
    if ( AutoRefresh == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CFileSpray_GetDFUWorkunit_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.wuid = wuid;

	__atlsoap_hr = SetClientStruct(&__params, 9);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"FileSpray/GetDFUWorkunit\?ver_=1.03\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*Exceptions = __params.Exceptions;
	*__Exceptions_nSizeIs = __params.__Exceptions_nSizeIs;
	*result = __params.result;
	*AutoRefresh = __params.AutoRefresh;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CFileSprayT<TClient>::EchoDateTime(
		BSTR dt, 
		BSTR* result
	)
{
    if ( result == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CFileSpray_EchoDateTime_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.dt = dt;

	__atlsoap_hr = SetClientStruct(&__params, 10);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"FileSpray/EchoDateTime\?ver_=1.03\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*result = __params.result;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CFileSprayT<TClient>::Dkc(
		BSTR srcname, 
		BSTR dstip, 
		BSTR dstfile, 
		bool overwrite, 
		int maxConnections, 
		bool norecover, 
		EspException** Exceptions, int* __Exceptions_nSizeIs, 
		BSTR* wuid
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
	if( __Exceptions_nSizeIs == NULL )
		return E_POINTER;
    if ( wuid == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CFileSpray_Dkc_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.srcname = srcname;
	__params.dstip = dstip;
	__params.dstfile = dstfile;
	__params.overwrite = overwrite;
	__params.maxConnections = maxConnections;
	__params.norecover = norecover;

	__atlsoap_hr = SetClientStruct(&__params, 11);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"FileSpray/Dkc\?ver_=1.03\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*Exceptions = __params.Exceptions;
	*__Exceptions_nSizeIs = __params.__Exceptions_nSizeIs;
	*wuid = __params.wuid;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CFileSprayT<TClient>::GetDFUWorkunits(
		BSTR* Owner, 
		BSTR* Cluster, 
		BSTR* StateReq, 
		BSTR* Type, 
		BSTR Jobname, 
		__int64* PageSize, 
		int CurrentPage, 
		__int64* PageStartFrom, 
		BSTR* Sortby, 
		bool* Descending, 
		EspException** Exceptions, int* __Exceptions_nSizeIs, 
		DFUWorkunit** results, int* __results_nSizeIs, 
		__int64* PrevPage, 
		__int64* NextPage, 
		__int64* LastPage, 
		__int64* NumWUs, 
		__int64* PageEndAt, 
		bool* First, 
		BSTR* BasicQuery, 
		BSTR* Filters
	)
{
    if ( Owner == NULL )
		return E_POINTER;
    if ( Cluster == NULL )
		return E_POINTER;
    if ( StateReq == NULL )
		return E_POINTER;
    if ( Type == NULL )
		return E_POINTER;
    if ( PageSize == NULL )
		return E_POINTER;
    if ( PageStartFrom == NULL )
		return E_POINTER;
    if ( Sortby == NULL )
		return E_POINTER;
    if ( Descending == NULL )
		return E_POINTER;
    if ( Exceptions == NULL )
		return E_POINTER;
	if( __Exceptions_nSizeIs == NULL )
		return E_POINTER;
    if ( results == NULL )
		return E_POINTER;
	if( __results_nSizeIs == NULL )
		return E_POINTER;
    if ( PrevPage == NULL )
		return E_POINTER;
    if ( NextPage == NULL )
		return E_POINTER;
    if ( LastPage == NULL )
		return E_POINTER;
    if ( NumWUs == NULL )
		return E_POINTER;
    if ( PageEndAt == NULL )
		return E_POINTER;
    if ( First == NULL )
		return E_POINTER;
    if ( BasicQuery == NULL )
		return E_POINTER;
    if ( Filters == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CFileSpray_GetDFUWorkunits_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Owner = *Owner;
	__params.Cluster = *Cluster;
	__params.StateReq = *StateReq;
	__params.Type = *Type;
	__params.Jobname = Jobname;
	__params.PageSize = *PageSize;
	__params.CurrentPage = CurrentPage;
	__params.PageStartFrom = *PageStartFrom;
	__params.Sortby = *Sortby;
	__params.Descending = *Descending;

	__atlsoap_hr = SetClientStruct(&__params, 12);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"FileSpray/GetDFUWorkunits\?ver_=1.03\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*Owner = __params.Owner;
	*Cluster = __params.Cluster;
	*StateReq = __params.StateReq;
	*Type = __params.Type;
	*PageSize = __params.PageSize;
	*PageStartFrom = __params.PageStartFrom;
	*Sortby = __params.Sortby;
	*Descending = __params.Descending;
	*Exceptions = __params.Exceptions;
	*__Exceptions_nSizeIs = __params.__Exceptions_nSizeIs;
	*results = __params.results;
	*__results_nSizeIs = __params.__results_nSizeIs;
	*PrevPage = __params.PrevPage;
	*NextPage = __params.NextPage;
	*LastPage = __params.LastPage;
	*NumWUs = __params.NumWUs;
	*PageEndAt = __params.PageEndAt;
	*First = __params.First;
	*BasicQuery = __params.BasicQuery;
	*Filters = __params.Filters;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CFileSprayT<TClient>::GetDFUExceptions(
		BSTR wuid, 
		EspException** Exceptions, int* __Exceptions_nSizeIs, 
		DFUException** result, int* __result_nSizeIs
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
	if( __Exceptions_nSizeIs == NULL )
		return E_POINTER;
    if ( result == NULL )
		return E_POINTER;
	if( __result_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CFileSpray_GetDFUExceptions_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.wuid = wuid;

	__atlsoap_hr = SetClientStruct(&__params, 13);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"FileSpray/GetDFUExceptions\?ver_=1.03\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*Exceptions = __params.Exceptions;
	*__Exceptions_nSizeIs = __params.__Exceptions_nSizeIs;
	*result = __params.result;
	*__result_nSizeIs = __params.__result_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CFileSprayT<TClient>::FileList(
		BSTR* Netaddr, 
		BSTR* Path, 
		BSTR* Mask, 
		BSTR* OS, 
		bool* DirectoryOnly, 
		EspException** Exceptions, int* __Exceptions_nSizeIs, 
		PhysicalFileStruct** files, int* __files_nSizeIs
	)
{
    if ( Netaddr == NULL )
		return E_POINTER;
    if ( Path == NULL )
		return E_POINTER;
    if ( Mask == NULL )
		return E_POINTER;
    if ( OS == NULL )
		return E_POINTER;
    if ( DirectoryOnly == NULL )
		return E_POINTER;
    if ( Exceptions == NULL )
		return E_POINTER;
	if( __Exceptions_nSizeIs == NULL )
		return E_POINTER;
    if ( files == NULL )
		return E_POINTER;
	if( __files_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CFileSpray_FileList_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Netaddr = *Netaddr;
	__params.Path = *Path;
	__params.Mask = *Mask;
	__params.OS = *OS;
	__params.DirectoryOnly = *DirectoryOnly;

	__atlsoap_hr = SetClientStruct(&__params, 14);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"FileSpray/FileList\?ver_=1.03\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*Netaddr = __params.Netaddr;
	*Path = __params.Path;
	*Mask = __params.Mask;
	*OS = __params.OS;
	*DirectoryOnly = __params.DirectoryOnly;
	*Exceptions = __params.Exceptions;
	*__Exceptions_nSizeIs = __params.__Exceptions_nSizeIs;
	*files = __params.files;
	*__files_nSizeIs = __params.__files_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CFileSprayT<TClient>::CreateDFUWorkunit(
		EspException** Exceptions, int* __Exceptions_nSizeIs, 
		DFUWorkunit* result
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
	if( __Exceptions_nSizeIs == NULL )
		return E_POINTER;
    if ( result == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CFileSpray_CreateDFUWorkunit_struct __params;
	memset(&__params, 0x00, sizeof(__params));

	__atlsoap_hr = SetClientStruct(&__params, 15);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"FileSpray/CreateDFUWorkunit\?ver_=1.03\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*Exceptions = __params.Exceptions;
	*__Exceptions_nSizeIs = __params.__Exceptions_nSizeIs;
	*result = __params.result;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CFileSprayT<TClient>::SubmitDFUWorkunit(
		BSTR wuid, 
		EspException** Exceptions, int* __Exceptions_nSizeIs
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
	if( __Exceptions_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CFileSpray_SubmitDFUWorkunit_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.wuid = wuid;

	__atlsoap_hr = SetClientStruct(&__params, 16);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"FileSpray/SubmitDFUWorkunit\?ver_=1.03\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*Exceptions = __params.Exceptions;
	*__Exceptions_nSizeIs = __params.__Exceptions_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CFileSprayT<TClient>::DFUWorkunitsAction(
		BSTR* wuids, int __wuids_nSizeIs, 
		BSTR Type, 
		EspException** Exceptions, int* __Exceptions_nSizeIs, 
		DFUActionResult** DFUActionResults, int* __DFUActionResults_nSizeIs
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
	if( __Exceptions_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUActionResults == NULL )
		return E_POINTER;
	if( __DFUActionResults_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CFileSpray_DFUWorkunitsAction_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.wuids = wuids;
	__params.__wuids_nSizeIs = __wuids_nSizeIs;
	__params.Type = Type;

	__atlsoap_hr = SetClientStruct(&__params, 17);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"FileSpray/DFUWorkunitsAction\?ver_=1.03\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*Exceptions = __params.Exceptions;
	*__Exceptions_nSizeIs = __params.__Exceptions_nSizeIs;
	*DFUActionResults = __params.DFUActionResults;
	*__DFUActionResults_nSizeIs = __params.__DFUActionResults_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CFileSprayT<TClient>::UpdateDFUWorkunit(
		DFUWorkunit wu, 
		BSTR ClusterOrig, 
		BSTR JobNameOrig, 
		bool isProtectedOrig, 
		int StateOrig, 
		EspException** Exceptions, int* __Exceptions_nSizeIs
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
	if( __Exceptions_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CFileSpray_UpdateDFUWorkunit_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.wu = wu;
	__params.ClusterOrig = ClusterOrig;
	__params.JobNameOrig = JobNameOrig;
	__params.isProtectedOrig = isProtectedOrig;
	__params.StateOrig = StateOrig;

	__atlsoap_hr = SetClientStruct(&__params, 18);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"FileSpray/UpdateDFUWorkunit\?ver_=1.03\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*Exceptions = __params.Exceptions;
	*__Exceptions_nSizeIs = __params.__Exceptions_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CFileSprayT<TClient>::Replicate(
		BSTR sourceLogicalName, 
		int replicateOffset, 
		BSTR cluster, 
		bool repeatLast, 
		bool onlyRepeated, 
		EspException** Exceptions, int* __Exceptions_nSizeIs, 
		BSTR* wuid
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
	if( __Exceptions_nSizeIs == NULL )
		return E_POINTER;
    if ( wuid == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CFileSpray_Replicate_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.sourceLogicalName = sourceLogicalName;
	__params.replicateOffset = replicateOffset;
	__params.cluster = cluster;
	__params.repeatLast = repeatLast;
	__params.onlyRepeated = onlyRepeated;

	__atlsoap_hr = SetClientStruct(&__params, 19);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"FileSpray/Replicate\?ver_=1.03\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*Exceptions = __params.Exceptions;
	*__Exceptions_nSizeIs = __params.__Exceptions_nSizeIs;
	*wuid = __params.wuid;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CFileSprayT<TClient>::Rename(
		BSTR srcname, 
		BSTR dstname, 
		bool overwrite, 
		EspException** Exceptions, int* __Exceptions_nSizeIs, 
		BSTR* wuid
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
	if( __Exceptions_nSizeIs == NULL )
		return E_POINTER;
    if ( wuid == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CFileSpray_Rename_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.srcname = srcname;
	__params.dstname = dstname;
	__params.overwrite = overwrite;

	__atlsoap_hr = SetClientStruct(&__params, 20);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"FileSpray/Rename\?ver_=1.03\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*Exceptions = __params.Exceptions;
	*__Exceptions_nSizeIs = __params.__Exceptions_nSizeIs;
	*wuid = __params.wuid;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CFileSprayT<TClient>::DeleteDFUWorkunit(
		BSTR wuid, 
		EspException** Exceptions, int* __Exceptions_nSizeIs, 
		bool* result
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
	if( __Exceptions_nSizeIs == NULL )
		return E_POINTER;
    if ( result == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CFileSpray_DeleteDFUWorkunit_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.wuid = wuid;

	__atlsoap_hr = SetClientStruct(&__params, 21);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"FileSpray/DeleteDFUWorkunit\?ver_=1.03\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*Exceptions = __params.Exceptions;
	*__Exceptions_nSizeIs = __params.__Exceptions_nSizeIs;
	*result = __params.result;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CFileSprayT<TClient>::DfuMonitor(
		BSTR EventName, 
		BSTR LogicalName, 
		BSTR Ip, 
		BSTR Filename, 
		bool Sub, 
		int ShotLimit, 
		EspException** Exceptions, int* __Exceptions_nSizeIs, 
		BSTR* wuid
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
	if( __Exceptions_nSizeIs == NULL )
		return E_POINTER;
    if ( wuid == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CFileSpray_DfuMonitor_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.EventName = EventName;
	__params.LogicalName = LogicalName;
	__params.Ip = Ip;
	__params.Filename = Filename;
	__params.Sub = Sub;
	__params.ShotLimit = ShotLimit;

	__atlsoap_hr = SetClientStruct(&__params, 22);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"FileSpray/DfuMonitor\?ver_=1.03\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*Exceptions = __params.Exceptions;
	*__Exceptions_nSizeIs = __params.__Exceptions_nSizeIs;
	*wuid = __params.wuid;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CFileSprayT<TClient>::AbortDFUWorkunit(
		BSTR wuid, 
		EspException** Exceptions, int* __Exceptions_nSizeIs
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
	if( __Exceptions_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CFileSpray_AbortDFUWorkunit_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.wuid = wuid;

	__atlsoap_hr = SetClientStruct(&__params, 23);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"FileSpray/AbortDFUWorkunit\?ver_=1.03\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*Exceptions = __params.Exceptions;
	*__Exceptions_nSizeIs = __params.__Exceptions_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
ATL_NOINLINE inline const _soapmap ** CFileSprayT<TClient>::GetFunctionMap()
{
	return __CFileSpray_funcs;
}

template <typename TClient>
ATL_NOINLINE inline const _soapmap ** CFileSprayT<TClient>::GetHeaderMap()
{
	static const _soapmapentry __CFileSpray_GetDFUProgress_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CFileSpray_GetDFUProgress_atlsoapheader_map = 
	{
		0xD5547544,
		"ProgressRequest",
		L"ProgressResponse",
		sizeof("ProgressRequest")-1,
		sizeof("ProgressResponse")-1,
		SOAPMAP_HEADER,
		__CFileSpray_GetDFUProgress_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x10852992,
		"http://webservices.seisint.com/FileSpray",
		L"http://webservices.seisint.com/FileSpray",
		sizeof("http://webservices.seisint.com/FileSpray")-1
	};

	static const _soapmapentry __CFileSpray_SprayVariable_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CFileSpray_SprayVariable_atlsoapheader_map = 
	{
		0x409EEF5E,
		"SprayVariable",
		L"SprayResponse",
		sizeof("SprayVariable")-1,
		sizeof("SprayResponse")-1,
		SOAPMAP_HEADER,
		__CFileSpray_SprayVariable_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x10852992,
		"http://webservices.seisint.com/FileSpray",
		L"http://webservices.seisint.com/FileSpray",
		sizeof("http://webservices.seisint.com/FileSpray")-1
	};

	static const _soapmapentry __CFileSpray_SprayFixed_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CFileSpray_SprayFixed_atlsoapheader_map = 
	{
		0x43DBC7AE,
		"SprayFixed",
		L"SprayFixedResponse",
		sizeof("SprayFixed")-1,
		sizeof("SprayFixedResponse")-1,
		SOAPMAP_HEADER,
		__CFileSpray_SprayFixed_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x10852992,
		"http://webservices.seisint.com/FileSpray",
		L"http://webservices.seisint.com/FileSpray",
		sizeof("http://webservices.seisint.com/FileSpray")-1
	};

	static const _soapmapentry __CFileSpray_Despray_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CFileSpray_Despray_atlsoapheader_map = 
	{
		0xEBD00347,
		"Despray",
		L"DesprayResponse",
		sizeof("Despray")-1,
		sizeof("DesprayResponse")-1,
		SOAPMAP_HEADER,
		__CFileSpray_Despray_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x10852992,
		"http://webservices.seisint.com/FileSpray",
		L"http://webservices.seisint.com/FileSpray",
		sizeof("http://webservices.seisint.com/FileSpray")-1
	};

	static const _soapmapentry __CFileSpray_DFUWUSearch_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CFileSpray_DFUWUSearch_atlsoapheader_map = 
	{
		0xABDF4B50,
		"DFUWUSearchRequest",
		L"DFUWUSearchResponse",
		sizeof("DFUWUSearchRequest")-1,
		sizeof("DFUWUSearchResponse")-1,
		SOAPMAP_HEADER,
		__CFileSpray_DFUWUSearch_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x10852992,
		"http://webservices.seisint.com/FileSpray",
		L"http://webservices.seisint.com/FileSpray",
		sizeof("http://webservices.seisint.com/FileSpray")-1
	};

	static const _soapmapentry __CFileSpray_Copy_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CFileSpray_Copy_atlsoapheader_map = 
	{
		0x968FD1AA,
		"Copy",
		L"CopyResponse",
		sizeof("Copy")-1,
		sizeof("CopyResponse")-1,
		SOAPMAP_HEADER,
		__CFileSpray_Copy_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x10852992,
		"http://webservices.seisint.com/FileSpray",
		L"http://webservices.seisint.com/FileSpray",
		sizeof("http://webservices.seisint.com/FileSpray")-1
	};

	static const _soapmapentry __CFileSpray_DFUWUFile_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CFileSpray_DFUWUFile_atlsoapheader_map = 
	{
		0xEBDC241A,
		"DFUWUFileRequest",
		L"DFUWUFileResponse",
		sizeof("DFUWUFileRequest")-1,
		sizeof("DFUWUFileResponse")-1,
		SOAPMAP_HEADER,
		__CFileSpray_DFUWUFile_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x10852992,
		"http://webservices.seisint.com/FileSpray",
		L"http://webservices.seisint.com/FileSpray",
		sizeof("http://webservices.seisint.com/FileSpray")-1
	};

	static const _soapmapentry __CFileSpray_DeleteDFUWorkunits_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CFileSpray_DeleteDFUWorkunits_atlsoapheader_map = 
	{
		0x3DD1C897,
		"DeleteDFUWorkunits",
		L"DeleteDFUWorkunitsResponse",
		sizeof("DeleteDFUWorkunits")-1,
		sizeof("DeleteDFUWorkunitsResponse")-1,
		SOAPMAP_HEADER,
		__CFileSpray_DeleteDFUWorkunits_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x10852992,
		"http://webservices.seisint.com/FileSpray",
		L"http://webservices.seisint.com/FileSpray",
		sizeof("http://webservices.seisint.com/FileSpray")-1
	};

	static const _soapmapentry __CFileSpray_ShowResult_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CFileSpray_ShowResult_atlsoapheader_map = 
	{
		0xE3C6A1AF,
		"ShowResultRequest",
		L"ShowResultResponse",
		sizeof("ShowResultRequest")-1,
		sizeof("ShowResultResponse")-1,
		SOAPMAP_HEADER,
		__CFileSpray_ShowResult_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x10852992,
		"http://webservices.seisint.com/FileSpray",
		L"http://webservices.seisint.com/FileSpray",
		sizeof("http://webservices.seisint.com/FileSpray")-1
	};

	static const _soapmapentry __CFileSpray_GetDFUWorkunit_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CFileSpray_GetDFUWorkunit_atlsoapheader_map = 
	{
		0xDF6F5951,
		"GetDFUWorkunit",
		L"GetDFUWorkunitResponse",
		sizeof("GetDFUWorkunit")-1,
		sizeof("GetDFUWorkunitResponse")-1,
		SOAPMAP_HEADER,
		__CFileSpray_GetDFUWorkunit_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x10852992,
		"http://webservices.seisint.com/FileSpray",
		L"http://webservices.seisint.com/FileSpray",
		sizeof("http://webservices.seisint.com/FileSpray")-1
	};

	static const _soapmapentry __CFileSpray_EchoDateTime_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CFileSpray_EchoDateTime_atlsoapheader_map = 
	{
		0xA6F7A7FB,
		"EchoDateTime",
		L"EchoDateTimeResponse",
		sizeof("EchoDateTime")-1,
		sizeof("EchoDateTimeResponse")-1,
		SOAPMAP_HEADER,
		__CFileSpray_EchoDateTime_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x10852992,
		"http://webservices.seisint.com/FileSpray",
		L"http://webservices.seisint.com/FileSpray",
		sizeof("http://webservices.seisint.com/FileSpray")-1
	};

	static const _soapmapentry __CFileSpray_Dkc_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CFileSpray_Dkc_atlsoapheader_map = 
	{
		0x496563A1,
		"Dkc",
		L"DkcResponse",
		sizeof("Dkc")-1,
		sizeof("DkcResponse")-1,
		SOAPMAP_HEADER,
		__CFileSpray_Dkc_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x10852992,
		"http://webservices.seisint.com/FileSpray",
		L"http://webservices.seisint.com/FileSpray",
		sizeof("http://webservices.seisint.com/FileSpray")-1
	};

	static const _soapmapentry __CFileSpray_GetDFUWorkunits_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CFileSpray_GetDFUWorkunits_atlsoapheader_map = 
	{
		0x19E50104,
		"GetDFUWorkunits",
		L"GetDFUWorkunitsResponse",
		sizeof("GetDFUWorkunits")-1,
		sizeof("GetDFUWorkunitsResponse")-1,
		SOAPMAP_HEADER,
		__CFileSpray_GetDFUWorkunits_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x10852992,
		"http://webservices.seisint.com/FileSpray",
		L"http://webservices.seisint.com/FileSpray",
		sizeof("http://webservices.seisint.com/FileSpray")-1
	};

	static const _soapmapentry __CFileSpray_GetDFUExceptions_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CFileSpray_GetDFUExceptions_atlsoapheader_map = 
	{
		0x89551910,
		"GetDFUExceptions",
		L"GetDFUExceptionsResponse",
		sizeof("GetDFUExceptions")-1,
		sizeof("GetDFUExceptionsResponse")-1,
		SOAPMAP_HEADER,
		__CFileSpray_GetDFUExceptions_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x10852992,
		"http://webservices.seisint.com/FileSpray",
		L"http://webservices.seisint.com/FileSpray",
		sizeof("http://webservices.seisint.com/FileSpray")-1
	};

	static const _soapmapentry __CFileSpray_FileList_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CFileSpray_FileList_atlsoapheader_map = 
	{
		0x930EC86B,
		"FileListRequest",
		L"FileListResponse",
		sizeof("FileListRequest")-1,
		sizeof("FileListResponse")-1,
		SOAPMAP_HEADER,
		__CFileSpray_FileList_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x10852992,
		"http://webservices.seisint.com/FileSpray",
		L"http://webservices.seisint.com/FileSpray",
		sizeof("http://webservices.seisint.com/FileSpray")-1
	};

	static const _soapmapentry __CFileSpray_CreateDFUWorkunit_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CFileSpray_CreateDFUWorkunit_atlsoapheader_map = 
	{
		0x79E22F25,
		"CreateDFUWorkunit",
		L"CreateDFUWorkunitResponse",
		sizeof("CreateDFUWorkunit")-1,
		sizeof("CreateDFUWorkunitResponse")-1,
		SOAPMAP_HEADER,
		__CFileSpray_CreateDFUWorkunit_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x10852992,
		"http://webservices.seisint.com/FileSpray",
		L"http://webservices.seisint.com/FileSpray",
		sizeof("http://webservices.seisint.com/FileSpray")-1
	};

	static const _soapmapentry __CFileSpray_SubmitDFUWorkunit_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CFileSpray_SubmitDFUWorkunit_atlsoapheader_map = 
	{
		0x2A535B45,
		"SubmitDFUWorkunit",
		L"SubmitDFUWorkunitResponse",
		sizeof("SubmitDFUWorkunit")-1,
		sizeof("SubmitDFUWorkunitResponse")-1,
		SOAPMAP_HEADER,
		__CFileSpray_SubmitDFUWorkunit_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x10852992,
		"http://webservices.seisint.com/FileSpray",
		L"http://webservices.seisint.com/FileSpray",
		sizeof("http://webservices.seisint.com/FileSpray")-1
	};

	static const _soapmapentry __CFileSpray_DFUWorkunitsAction_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CFileSpray_DFUWorkunitsAction_atlsoapheader_map = 
	{
		0x40F8E362,
		"DFUWorkunitsActionRequest",
		L"DFUWorkunitsActionResponse",
		sizeof("DFUWorkunitsActionRequest")-1,
		sizeof("DFUWorkunitsActionResponse")-1,
		SOAPMAP_HEADER,
		__CFileSpray_DFUWorkunitsAction_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x10852992,
		"http://webservices.seisint.com/FileSpray",
		L"http://webservices.seisint.com/FileSpray",
		sizeof("http://webservices.seisint.com/FileSpray")-1
	};

	static const _soapmapentry __CFileSpray_UpdateDFUWorkunit_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CFileSpray_UpdateDFUWorkunit_atlsoapheader_map = 
	{
		0x2D8F78B4,
		"UpdateDFUWorkunit",
		L"UpdateDFUWorkunitResponse",
		sizeof("UpdateDFUWorkunit")-1,
		sizeof("UpdateDFUWorkunitResponse")-1,
		SOAPMAP_HEADER,
		__CFileSpray_UpdateDFUWorkunit_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x10852992,
		"http://webservices.seisint.com/FileSpray",
		L"http://webservices.seisint.com/FileSpray",
		sizeof("http://webservices.seisint.com/FileSpray")-1
	};

	static const _soapmapentry __CFileSpray_Replicate_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CFileSpray_Replicate_atlsoapheader_map = 
	{
		0x37709508,
		"Replicate",
		L"ReplicateResponse",
		sizeof("Replicate")-1,
		sizeof("ReplicateResponse")-1,
		SOAPMAP_HEADER,
		__CFileSpray_Replicate_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x10852992,
		"http://webservices.seisint.com/FileSpray",
		L"http://webservices.seisint.com/FileSpray",
		sizeof("http://webservices.seisint.com/FileSpray")-1
	};

	static const _soapmapentry __CFileSpray_Rename_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CFileSpray_Rename_atlsoapheader_map = 
	{
		0xA9974567,
		"Rename",
		L"RenameResponse",
		sizeof("Rename")-1,
		sizeof("RenameResponse")-1,
		SOAPMAP_HEADER,
		__CFileSpray_Rename_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x10852992,
		"http://webservices.seisint.com/FileSpray",
		L"http://webservices.seisint.com/FileSpray",
		sizeof("http://webservices.seisint.com/FileSpray")-1
	};

	static const _soapmapentry __CFileSpray_DeleteDFUWorkunit_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CFileSpray_DeleteDFUWorkunit_atlsoapheader_map = 
	{
		0xE847FA84,
		"DeleteDFUWorkunit",
		L"DeleteDFUWorkunitResponse",
		sizeof("DeleteDFUWorkunit")-1,
		sizeof("DeleteDFUWorkunitResponse")-1,
		SOAPMAP_HEADER,
		__CFileSpray_DeleteDFUWorkunit_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x10852992,
		"http://webservices.seisint.com/FileSpray",
		L"http://webservices.seisint.com/FileSpray",
		sizeof("http://webservices.seisint.com/FileSpray")-1
	};

	static const _soapmapentry __CFileSpray_DfuMonitor_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CFileSpray_DfuMonitor_atlsoapheader_map = 
	{
		0x7F341276,
		"DfuMonitorRequest",
		L"DfuMonitorResponse",
		sizeof("DfuMonitorRequest")-1,
		sizeof("DfuMonitorResponse")-1,
		SOAPMAP_HEADER,
		__CFileSpray_DfuMonitor_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x10852992,
		"http://webservices.seisint.com/FileSpray",
		L"http://webservices.seisint.com/FileSpray",
		sizeof("http://webservices.seisint.com/FileSpray")-1
	};

	static const _soapmapentry __CFileSpray_AbortDFUWorkunit_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CFileSpray_AbortDFUWorkunit_atlsoapheader_map = 
	{
		0x15D1B009,
		"AbortDFUWorkunit",
		L"AbortDFUWorkunitResponse",
		sizeof("AbortDFUWorkunit")-1,
		sizeof("AbortDFUWorkunitResponse")-1,
		SOAPMAP_HEADER,
		__CFileSpray_AbortDFUWorkunit_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x10852992,
		"http://webservices.seisint.com/FileSpray",
		L"http://webservices.seisint.com/FileSpray",
		sizeof("http://webservices.seisint.com/FileSpray")-1
	};


	static const _soapmap * __CFileSpray_headers[] =
	{
		&__CFileSpray_GetDFUProgress_atlsoapheader_map,
		&__CFileSpray_SprayVariable_atlsoapheader_map,
		&__CFileSpray_SprayFixed_atlsoapheader_map,
		&__CFileSpray_Despray_atlsoapheader_map,
		&__CFileSpray_DFUWUSearch_atlsoapheader_map,
		&__CFileSpray_Copy_atlsoapheader_map,
		&__CFileSpray_DFUWUFile_atlsoapheader_map,
		&__CFileSpray_DeleteDFUWorkunits_atlsoapheader_map,
		&__CFileSpray_ShowResult_atlsoapheader_map,
		&__CFileSpray_GetDFUWorkunit_atlsoapheader_map,
		&__CFileSpray_EchoDateTime_atlsoapheader_map,
		&__CFileSpray_Dkc_atlsoapheader_map,
		&__CFileSpray_GetDFUWorkunits_atlsoapheader_map,
		&__CFileSpray_GetDFUExceptions_atlsoapheader_map,
		&__CFileSpray_FileList_atlsoapheader_map,
		&__CFileSpray_CreateDFUWorkunit_atlsoapheader_map,
		&__CFileSpray_SubmitDFUWorkunit_atlsoapheader_map,
		&__CFileSpray_DFUWorkunitsAction_atlsoapheader_map,
		&__CFileSpray_UpdateDFUWorkunit_atlsoapheader_map,
		&__CFileSpray_Replicate_atlsoapheader_map,
		&__CFileSpray_Rename_atlsoapheader_map,
		&__CFileSpray_DeleteDFUWorkunit_atlsoapheader_map,
		&__CFileSpray_DfuMonitor_atlsoapheader_map,
		&__CFileSpray_AbortDFUWorkunit_atlsoapheader_map,
		NULL
	};
	
	return __CFileSpray_headers;
}

template <typename TClient>
ATL_NOINLINE inline void * CFileSprayT<TClient>::GetHeaderValue()
{
	return this;
}

template <typename TClient>
ATL_NOINLINE inline const wchar_t * CFileSprayT<TClient>::GetNamespaceUri()
{
	return L"http://webservices.seisint.com/FileSpray";
}

template <typename TClient>
ATL_NOINLINE inline const char * CFileSprayT<TClient>::GetServiceName()
{
	return NULL;
}

template <typename TClient>
ATL_NOINLINE inline const char * CFileSprayT<TClient>::GetNamespaceUriA()
{
	return "http://webservices.seisint.com/FileSpray";
}

template <typename TClient>
ATL_NOINLINE inline HRESULT CFileSprayT<TClient>::CallFunction(
	void *, 
	const wchar_t *, int,
	size_t)
{
	return E_NOTIMPL;
}

template <typename TClient>
ATL_NOINLINE inline HRESULT CFileSprayT<TClient>::GetClientReader(ISAXXMLReader **ppReader)
{
	if (ppReader == NULL)
	{
		return E_INVALIDARG;
	}
	
	CComPtr<ISAXXMLReader> spReader = GetReader();
	if (spReader.p != NULL)
	{
		*ppReader = spReader.Detach();
		return S_OK;
	}
	return TClient::GetClientReader(ppReader);
}

} // namespace FileSpray

template<>
inline HRESULT AtlCleanupValue<FileSpray::DFUActionResult>(FileSpray::DFUActionResult *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->ID);
	AtlCleanupValue(&pVal->Action);
	AtlCleanupValue(&pVal->Result);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<FileSpray::DFUActionResult>(FileSpray::DFUActionResult *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->ID, pMemMgr);
	AtlCleanupValueEx(&pVal->Action, pMemMgr);
	AtlCleanupValueEx(&pVal->Result, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<FileSpray::PhysicalFileStruct>(FileSpray::PhysicalFileStruct *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->name);
	AtlCleanupValue(&pVal->isDir);
	AtlCleanupValue(&pVal->filesize);
	AtlCleanupValue(&pVal->modifiedtime);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<FileSpray::PhysicalFileStruct>(FileSpray::PhysicalFileStruct *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->name, pMemMgr);
	AtlCleanupValueEx(&pVal->isDir, pMemMgr);
	AtlCleanupValueEx(&pVal->filesize, pMemMgr);
	AtlCleanupValueEx(&pVal->modifiedtime, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<FileSpray::EspException>(FileSpray::EspException *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Code);
	AtlCleanupValue(&pVal->Audience);
	AtlCleanupValue(&pVal->Source);
	AtlCleanupValue(&pVal->Message);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<FileSpray::EspException>(FileSpray::EspException *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Code, pMemMgr);
	AtlCleanupValueEx(&pVal->Audience, pMemMgr);
	AtlCleanupValueEx(&pVal->Source, pMemMgr);
	AtlCleanupValueEx(&pVal->Message, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<FileSpray::DFUWorkunit>(FileSpray::DFUWorkunit *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->ID);
	AtlCleanupValue(&pVal->ClusterName);
	AtlCleanupValue(&pVal->JobName);
	AtlCleanupValue(&pVal->Queue);
	AtlCleanupValue(&pVal->User);
	AtlCleanupValue(&pVal->isProtected);
	AtlCleanupValue(&pVal->Command);
	AtlCleanupValue(&pVal->CommandMessage);
	AtlCleanupValue(&pVal->PercentDone);
	AtlCleanupValue(&pVal->SecsLeft);
	AtlCleanupValue(&pVal->ProgressMessage);
	AtlCleanupValue(&pVal->SummaryMessage);
	AtlCleanupValue(&pVal->State);
	AtlCleanupValue(&pVal->SourceLogicalName);
	AtlCleanupValue(&pVal->SourceIP);
	AtlCleanupValue(&pVal->SourceFilePath);
	AtlCleanupValue(&pVal->SourceDali);
	AtlCleanupValue(&pVal->SourceRecordSize);
	AtlCleanupValue(&pVal->SourceFormat);
	AtlCleanupValue(&pVal->RowTag);
	AtlCleanupValue(&pVal->SourceNumParts);
	AtlCleanupValue(&pVal->SourceDirectory);
	AtlCleanupValue(&pVal->DestLogicalName);
	AtlCleanupValue(&pVal->DestGroupName);
	AtlCleanupValue(&pVal->DestDirectory);
	AtlCleanupValue(&pVal->DestIP);
	AtlCleanupValue(&pVal->DestFilePath);
	AtlCleanupValue(&pVal->DestFormat);
	AtlCleanupValue(&pVal->DestNumParts);
	AtlCleanupValue(&pVal->DestRecordSize);
	AtlCleanupValue(&pVal->Replicate);
	AtlCleanupValue(&pVal->Overwrite);
	AtlCleanupValue(&pVal->Compress);
	AtlCleanupValue(&pVal->TimeStarted);
	AtlCleanupValue(&pVal->TimeStopped);
	AtlCleanupValue(&pVal->StateMessage);
	AtlCleanupValue(&pVal->MonitorEventName);
	AtlCleanupValue(&pVal->MonitorSub);
	AtlCleanupValue(&pVal->MonitorShotLimit);
	AtlCleanupValue(&pVal->SourceDiffKeyName);
	AtlCleanupValue(&pVal->DestDiffKeyName);
	AtlCleanupValue(&pVal->Archived);
	AtlCleanupValue(&pVal->encrypt);
	AtlCleanupValue(&pVal->decrypt);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<FileSpray::DFUWorkunit>(FileSpray::DFUWorkunit *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->ID, pMemMgr);
	AtlCleanupValueEx(&pVal->ClusterName, pMemMgr);
	AtlCleanupValueEx(&pVal->JobName, pMemMgr);
	AtlCleanupValueEx(&pVal->Queue, pMemMgr);
	AtlCleanupValueEx(&pVal->User, pMemMgr);
	AtlCleanupValueEx(&pVal->isProtected, pMemMgr);
	AtlCleanupValueEx(&pVal->Command, pMemMgr);
	AtlCleanupValueEx(&pVal->CommandMessage, pMemMgr);
	AtlCleanupValueEx(&pVal->PercentDone, pMemMgr);
	AtlCleanupValueEx(&pVal->SecsLeft, pMemMgr);
	AtlCleanupValueEx(&pVal->ProgressMessage, pMemMgr);
	AtlCleanupValueEx(&pVal->SummaryMessage, pMemMgr);
	AtlCleanupValueEx(&pVal->State, pMemMgr);
	AtlCleanupValueEx(&pVal->SourceLogicalName, pMemMgr);
	AtlCleanupValueEx(&pVal->SourceIP, pMemMgr);
	AtlCleanupValueEx(&pVal->SourceFilePath, pMemMgr);
	AtlCleanupValueEx(&pVal->SourceDali, pMemMgr);
	AtlCleanupValueEx(&pVal->SourceRecordSize, pMemMgr);
	AtlCleanupValueEx(&pVal->SourceFormat, pMemMgr);
	AtlCleanupValueEx(&pVal->RowTag, pMemMgr);
	AtlCleanupValueEx(&pVal->SourceNumParts, pMemMgr);
	AtlCleanupValueEx(&pVal->SourceDirectory, pMemMgr);
	AtlCleanupValueEx(&pVal->DestLogicalName, pMemMgr);
	AtlCleanupValueEx(&pVal->DestGroupName, pMemMgr);
	AtlCleanupValueEx(&pVal->DestDirectory, pMemMgr);
	AtlCleanupValueEx(&pVal->DestIP, pMemMgr);
	AtlCleanupValueEx(&pVal->DestFilePath, pMemMgr);
	AtlCleanupValueEx(&pVal->DestFormat, pMemMgr);
	AtlCleanupValueEx(&pVal->DestNumParts, pMemMgr);
	AtlCleanupValueEx(&pVal->DestRecordSize, pMemMgr);
	AtlCleanupValueEx(&pVal->Replicate, pMemMgr);
	AtlCleanupValueEx(&pVal->Overwrite, pMemMgr);
	AtlCleanupValueEx(&pVal->Compress, pMemMgr);
	AtlCleanupValueEx(&pVal->TimeStarted, pMemMgr);
	AtlCleanupValueEx(&pVal->TimeStopped, pMemMgr);
	AtlCleanupValueEx(&pVal->StateMessage, pMemMgr);
	AtlCleanupValueEx(&pVal->MonitorEventName, pMemMgr);
	AtlCleanupValueEx(&pVal->MonitorSub, pMemMgr);
	AtlCleanupValueEx(&pVal->MonitorShotLimit, pMemMgr);
	AtlCleanupValueEx(&pVal->SourceDiffKeyName, pMemMgr);
	AtlCleanupValueEx(&pVal->DestDiffKeyName, pMemMgr);
	AtlCleanupValueEx(&pVal->Archived, pMemMgr);
	AtlCleanupValueEx(&pVal->encrypt, pMemMgr);
	AtlCleanupValueEx(&pVal->decrypt, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<FileSpray::DFUException>(FileSpray::DFUException *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Code);
	AtlCleanupValue(&pVal->Message);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<FileSpray::DFUException>(FileSpray::DFUException *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Code, pMemMgr);
	AtlCleanupValueEx(&pVal->Message, pMemMgr);
	return S_OK;
}
