//
// sproxy.exe generated file
// do not modify this file
//
// Created: 10/28/2009@09:14:18
//

#pragma once


#if !defined(_WIN32_WINDOWS) && !defined(_WIN32_WINNT) && !defined(_WIN32_WCE)
#pragma message("warning: defining _WIN32_WINDOWS = 0x0410")
#define _WIN32_WINDOWS 0x0410
#endif

#include <atlsoap.h>

namespace WsWorkunits
{

struct ECLTimingData
{
	BSTR Name;
	int GraphNum;
	int SubGraphNum;
	int GID;
	int Min;
	int MS;
};

struct LogicalFileUpload
{
	int Type;
	BSTR Source;
	BSTR Destination;
	BSTR EventTag;
};

struct ECLSchemaItem
{
	BSTR ColumnName;
	BSTR ColumnType;
	int ColumnTypeCode;
	bool isConditional;
};

struct ECLWorkflow
{
	BSTR WFID;
	BSTR EventName;
	BSTR EventText;
	int Count;
	int CountRemaining;
};

struct ArrayOfECLSchemaItem
{
	ECLSchemaItem *_ECLSchemaItem;
	int ___ECLSchemaItem_nSizeIs;
};

struct ECLResult
{
	BSTR Name;
	int Sequence;
	BSTR Value;
	BSTR Link;
	BSTR FileName;
	bool IsSupplied;
	bool ShowFileContent;
	__int64 Total;
	ArrayOfECLSchemaItem ECLSchemas;
};

struct ECLSourceFile
{
	BSTR FileCluster;
	BSTR Name;
	bool IsSuperFile;
	int Count;
};

struct ServerInfo
{
	BSTR Name;
	BSTR NetAddress;
};

struct ECLGraph
{
	BSTR Name;
	BSTR Label;
	BSTR Type;
	bool Running;
	bool Complete;
	bool Failed;
	__int64 RunningId;
};

struct EspException
{
	BSTR Code;
	BSTR Audience;
	BSTR Source;
	BSTR Message;
};

struct ArrayOfEspException
{
	BSTR Source;
	EspException *Exception;
	int __Exception_nSizeIs;
};

struct WUECLAttribute
{
	BSTR ModuleName;
	BSTR AttributeName;
	bool IsLocked;
	bool IsCheckedOut;
	bool IsSandbox;
	bool IsOrphaned;
};

struct ECLException
{
	BSTR Source;
	BSTR Severity;
	int Code;
	BSTR Message;
	BSTR FileName;
	int LineNo;
	int Column;
};

struct DebugValue
{
	BSTR Name;
	BSTR Value;
};

struct WUActionResult
{
	BSTR Wuid;
	BSTR Action;
	BSTR Result;
};

struct ECLQuery
{
	BSTR Text;
	BSTR Cpp;
	BSTR ResTxt;
	BSTR Dll;
	BSTR ThorLog;
};

struct ECLHelpFile
{
	BSTR Name;
	BSTR Type;
};

struct ECLTimer
{
	BSTR Name;
	BSTR Value;
	int count;
	BSTR GraphName;
	int SubGraphId;
};

struct ApplicationValue
{
	BSTR Application;
	BSTR Name;
	BSTR Value;
};

struct ECLWorkunit
{
	BSTR Wuid;
	BSTR Owner;
	BSTR Cluster;
	BSTR RoxieCluster;
	BSTR Jobname;
	BSTR Queue;
	int StateID;
	BSTR State;
	BSTR StateEx;
	BSTR Description;
	bool AddDrilldownFields;
	bool Protected;
	bool Active;
	int Action;
	BSTR DateTimeScheduled;
	int PriorityClass;
	int PriorityLevel;
	BSTR Scope;
	BSTR Snapshot;
	int ResultLimit;
	bool Archived;
	int EventSchedule;
	bool HaveSubGraphTimings;
	ECLQuery Query;
	ECLHelpFile *Helpers;
	int __Helpers_nSizeIs;
	ECLException *Exceptions;
	int __Exceptions_nSizeIs;
	ECLGraph *Graphs;
	int __Graphs_nSizeIs;
	ECLSourceFile *SourceFiles;
	int __SourceFiles_nSizeIs;
	ECLResult *Results;
	int __Results_nSizeIs;
	ECLResult *Variables;
	int __Variables_nSizeIs;
	ECLTimer *Timers;
	int __Timers_nSizeIs;
	DebugValue *DebugValues;
	int __DebugValues_nSizeIs;
	ApplicationValue *ApplicationValues;
	int __ApplicationValues_nSizeIs;
	ECLWorkflow *Workflows;
	int __Workflows_nSizeIs;
	ECLTimingData *TimingData;
	int __TimingData_nSizeIs;
	BSTR *AllowedClusters;
	int __AllowedClusters_nSizeIs;
	int ErrorCount;
	int WarningCount;
	int InfoCount;
	int GraphCount;
	int SourceFileCount;
	int ResultCount;
	int VariableCount;
	int TimerCount;
	bool HasDebugValue;
	int ApplicationValueCount;
	BSTR XmlParams;
	int AccessFlag;
	int ClusterFlag;
};

struct ECLGraphEx
{
	BSTR Name;
	BSTR Label;
	BSTR Type;
	BSTR Graph;
	bool Running;
	__int64 RunningId;
	bool Complete;
	bool Failed;
};

struct ECLJob
{
	BSTR Wuid;
	BSTR Graph;
	BSTR State;
	BSTR StartedDate;
	BSTR FinishedDate;
	BSTR Cluster;
	BSTR GraphNum;
	BSTR SubGraphNum;
	BSTR NumOfRuns;
	int Duration;
};

struct ScheduledWU
{
	BSTR Wuid;
	BSTR EclServer;
	BSTR EventName;
	BSTR EventText;
	BSTR JobName;
};

template <typename TClient = CSoapSocketClientT<> >
class CWsWorkunitsT : 
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

	CWsWorkunitsT(ISAXXMLReader *pReader = NULL)
		:TClient(_T("http://10.173.88.201:8010/WsWorkunits\?ver_=1.25"))
	{
		SetClient(true);
		SetReader(pReader);
	}
	
	~CWsWorkunitsT()
	{
		Uninitialize();
	}
	
	void Uninitialize()
	{
		UninitializeSOAP();
	}	

	HRESULT WUCreateAndUpdate(
		BSTR Wuid, 
		int State, 
		int StateOrig, 
		BSTR Jobname, 
		BSTR JobnameOrig, 
		BSTR QueryText, 
		int Action, 
		BSTR Description, 
		BSTR DescriptionOrig, 
		bool AddDrilldownFields, 
		int ResultLimit, 
		bool Protected, 
		bool ProtectedOrig, 
		int PriorityClass, 
		int PriorityLevel, 
		BSTR Scope, 
		BSTR ScopeOrig, 
		BSTR ClusterSelection, 
		BSTR ClusterOrig, 
		BSTR XmlParams, 
		BSTR ThorSlaveIP, 
		DebugValue* DebugValues, int DebugValues_nSizeIs, 
		ApplicationValue* ApplicationValues, int ApplicationValues_nSizeIs, 
		ArrayOfEspException* Exceptions, 
		ECLWorkunit* Workunit
	);

	HRESULT WUSubmit(
		BSTR Wuid, 
		BSTR Cluster, 
		BSTR Queue, 
		BSTR Snapshot, 
		int MaxRunTime, 
		int BlockTillFinishTimer, 
		bool SyntaxCheck, 
		ArrayOfEspException* Exceptions
	);

	HRESULT WUSchedule(
		BSTR Wuid, 
		BSTR Cluster, 
		BSTR Queue, 
		BSTR Snapshot, 
		BSTR When, 
		int MaxRunTime, 
		ArrayOfEspException* Exceptions
	);

	HRESULT WUPushEvent(
		BSTR EventName, 
		BSTR EventText, 
		ArrayOfEspException* Exceptions
	);

	HRESULT WUUpdate(
		BSTR Wuid, 
		int State, 
		int StateOrig, 
		BSTR Jobname, 
		BSTR JobnameOrig, 
		BSTR QueryText, 
		int Action, 
		BSTR Description, 
		BSTR DescriptionOrig, 
		bool AddDrilldownFields, 
		int ResultLimit, 
		bool Protected, 
		bool ProtectedOrig, 
		int PriorityClass, 
		int PriorityLevel, 
		BSTR Scope, 
		BSTR ScopeOrig, 
		BSTR ClusterSelection, 
		BSTR ClusterOrig, 
		BSTR XmlParams, 
		BSTR ThorSlaveIP, 
		DebugValue* DebugValues, int DebugValues_nSizeIs, 
		ApplicationValue* ApplicationValues, int ApplicationValues_nSizeIs, 
		ArrayOfEspException* Exceptions, 
		ECLWorkunit* Workunit
	);

	HRESULT WUAddLocalFileToWorkunit(
		BSTR* Wuid, 
		BSTR* Name, 
		BSTR Val, 
		BSTR DefVal, 
		int Type, 
		int Length, 
		ArrayOfEspException* Exceptions, 
		BSTR* Result
	);

	HRESULT WUExport(
		BSTR Cluster, 
		BSTR Owner, 
		BSTR State, 
		BSTR StartDate, 
		BSTR EndDate, 
		BSTR ECL, 
		BSTR Jobname, 
		ArrayOfEspException* Exceptions, 
		ATLSOAP_BLOB* ExportData
	);

	HRESULT WUQuery(
		BSTR Wuid, 
		BSTR* Type, 
		BSTR* Cluster, 
		BSTR* RoxieCluster, 
		BSTR* Owner, 
		BSTR* State, 
		BSTR* StartDate, 
		BSTR* EndDate, 
		BSTR* ECL, 
		BSTR* Jobname, 
		BSTR* LogicalFile, 
		BSTR* LogicalFileSearchType, 
		BSTR ApplicationName, 
		BSTR ApplicationKey, 
		BSTR ApplicationData, 
		BSTR After, 
		BSTR Before, 
		int* Count, 
		__int64* PageSize, 
		__int64* PageStartFrom, 
		__int64* PageEndAt, 
		BSTR* Sortby, 
		bool* Descending, 
		ArrayOfEspException* Exceptions, 
		BSTR* Current, 
		BSTR* Next, 
		__int64* PrevPage, 
		__int64* NextPage, 
		__int64* LastPage, 
		int* NumWUs, 
		bool* First, 
		BSTR* BasicQuery, 
		BSTR* Filters, 
		ECLWorkunit** Workunits, int* Workunits_nSizeIs
	);

	HRESULT WUFile(
		BSTR Name, 
		BSTR Wuid, 
		BSTR Type, 
		BSTR SlaveIP, 
		ArrayOfEspException* Exceptions, 
		ATLSOAP_BLOB* thefile
	);

	HRESULT WUDelete(
		BSTR* Wuids, int Wuids_nSizeIs, 
		int BlockTillFinishTimer, 
		ArrayOfEspException* Exceptions, 
		WUActionResult** ActionResults, int* ActionResults_nSizeIs
	);

	HRESULT WUAction(
		BSTR* Wuids, int Wuids_nSizeIs, 
		BSTR ActionType, 
		BSTR Cluster, 
		BSTR Owner, 
		BSTR State, 
		BSTR StartDate, 
		BSTR EndDate, 
		BSTR ECL, 
		BSTR Jobname, 
		BSTR Test, 
		BSTR CurrentPage, 
		BSTR PageSize, 
		BSTR Sortby, 
		bool Descending, 
		BSTR EclServer, 
		BSTR EventName, 
		BSTR PageFrom, 
		int BlockTillFinishTimer, 
		ArrayOfEspException* Exceptions, 
		WUActionResult** ActionResults, int* ActionResults_nSizeIs
	);

	HRESULT WUCreate(
		ArrayOfEspException* Exceptions, 
		ECLWorkunit* Workunit
	);

	HRESULT WUWaitComplete(
		BSTR Wuid, 
		int Wait, 
		bool ReturnOnWait, 
		ArrayOfEspException* Exceptions, 
		int* StateID
	);

	HRESULT WUProtect(
		BSTR* Wuids, int Wuids_nSizeIs, 
		bool Protect, 
		ArrayOfEspException* Exceptions, 
		WUActionResult** ActionResults, int* ActionResults_nSizeIs
	);

	HRESULT WUCompileECL(
		BSTR ECL, 
		BSTR ModuleName, 
		BSTR AttributeName, 
		BSTR Queue, 
		BSTR Cluster, 
		BSTR Snapshot, 
		bool IncludeDependencies, 
		bool IncludeComplexity, 
		int TimeToWait, 
		ArrayOfEspException* Exceptions, 
		BSTR* Complexity, 
		ECLException** Errors, int* Errors_nSizeIs, 
		WUECLAttribute** Dependencies, int* Dependencies_nSizeIs
	);

	HRESULT WUSyntaxCheckECL(
		BSTR ECL, 
		BSTR ModuleName, 
		BSTR AttributeName, 
		BSTR Queue, 
		BSTR Cluster, 
		BSTR Snapshot, 
		int TimeToWait, 
		DebugValue* DebugValues, int DebugValues_nSizeIs, 
		ArrayOfEspException* Exceptions, 
		ECLException** Errors, int* Errors_nSizeIs
	);

	HRESULT WUListLocalFileRequired(
		BSTR Wuid, 
		ArrayOfEspException* Exceptions, 
		LogicalFileUpload** LocalFileUploads, int* LocalFileUploads_nSizeIs
	);

	HRESULT WUShowScheduled(
		BSTR EclServer, 
		BSTR* EventName, 
		BSTR* PushEventName, 
		BSTR* PushEventText, 
		ArrayOfEspException* Exceptions, 
		int* EclServerSelected, 
		BSTR* Query, 
		ServerInfo** EclServers, int* EclServers_nSizeIs, 
		ScheduledWU** Workunits, int* Workunits_nSizeIs
	);

	HRESULT WUClusterJobSummaryXLS(
		BSTR Cluster, 
		BSTR StartDate, 
		BSTR EndDate, 
		bool ShowAll, 
		BSTR BusinessStartTime, 
		BSTR BusinessEndTime, 
		ArrayOfEspException* Exceptions, 
		ATLSOAP_BLOB* Result
	);

	HRESULT WUInfo(
		BSTR Wuid, 
		BSTR Type, 
		bool IncludeExceptions, 
		bool IncludeGraphs, 
		bool IncludeSourceFiles, 
		bool IncludeResults, 
		bool IncludeVariables, 
		bool IncludeTimers, 
		bool IncludeDebugValues, 
		bool IncludeApplicationValues, 
		bool IncludeWorkflows, 
		bool SuppressResultSchemas, 
		BSTR* ThorSlaveIP, 
		ArrayOfEspException* Exceptions, 
		ECLWorkunit* Workunit, 
		int* AutoRefresh, 
		bool* CanCompile
	);

	HRESULT WUInfoDetails(
		BSTR Wuid, 
		BSTR Type, 
		bool IncludeExceptions, 
		bool IncludeGraphs, 
		bool IncludeSourceFiles, 
		bool IncludeResults, 
		bool IncludeVariables, 
		bool IncludeTimers, 
		bool IncludeDebugValues, 
		bool IncludeApplicationValues, 
		bool IncludeWorkflows, 
		bool SuppressResultSchemas, 
		BSTR* ThorSlaveIP, 
		ArrayOfEspException* Exceptions, 
		ECLWorkunit* Workunit, 
		int* AutoRefresh, 
		bool* CanCompile
	);

	HRESULT WUGetDependancyTrees(
		BSTR Cluster, 
		BSTR Queue, 
		BSTR Snapshot, 
		BSTR Items, 
		BSTR TimeoutMilliSec, 
		ArrayOfEspException* Exceptions, 
		ECLException** Errors, int* Errors_nSizeIs, 
		ATLSOAP_BLOB* DependancyTrees
	);

	HRESULT WUAbort(
		BSTR* Wuids, int Wuids_nSizeIs, 
		int BlockTillFinishTimer, 
		ArrayOfEspException* Exceptions, 
		WUActionResult** ActionResults, int* ActionResults_nSizeIs
	);

	HRESULT WUCDebug(
		BSTR Wuid, 
		BSTR Command, 
		ArrayOfEspException* Exceptions, 
		BSTR* Result
	);

	HRESULT GVCAjaxGraph(
		BSTR* Name, 
		BSTR* GraphName, 
		int* SubGraphId, 
		bool* SubGraphOnly, 
		BSTR* GraphType
	);

	HRESULT WUJobList(
		BSTR Cluster, 
		BSTR* StartDate, 
		BSTR* EndDate, 
		bool ShowAll, 
		int BusinessStartHour, 
		int BusinessEndHour, 
		ArrayOfEspException* Exceptions, 
		ECLJob** Jobs, int* Jobs_nSizeIs
	);

	HRESULT WUGetGraph(
		BSTR Wuid, 
		BSTR GraphName, 
		BSTR SubGraphId, 
		ArrayOfEspException* Exceptions, 
		ECLGraphEx** Graphs, int* Graphs_nSizeIs
	);

	HRESULT WUResultSummary(
		BSTR* Wuid, 
		int* Sequence, 
		ArrayOfEspException* Exceptions, 
		int* Format, 
		ECLResult* Result
	);

	HRESULT WUGraphTiming(
		BSTR Wuid, 
		ArrayOfEspException* Exceptions, 
		ECLWorkunit* Workunit
	);

	HRESULT WUClusterJobQueueXLS(
		BSTR Cluster, 
		BSTR StartDate, 
		BSTR EndDate, 
		BSTR ShowType, 
		ArrayOfEspException* Exceptions, 
		ATLSOAP_BLOB* Result
	);

	HRESULT WUResult(
		BSTR* Wuid, 
		int* Sequence, 
		BSTR ResultName, 
		BSTR* LogicalName, 
		BSTR* Cluster, 
		__int64* Start, 
		int* Count, 
		ArrayOfEspException* Exceptions, 
		BSTR* Name, 
		int* Requested, 
		__int64* Total, 
		BSTR* Result
	);

	HRESULT WUWaitCompiled(
		BSTR Wuid, 
		int Wait, 
		bool ReturnOnWait, 
		ArrayOfEspException* Exceptions, 
		int* StateID
	);

	HRESULT WUProcessGraph(
		BSTR FileName, 
		BSTR PopupId, 
		bool XmlGraph, 
		bool Stats, 
		ArrayOfEspException* Exceptions, 
		ATLSOAP_BLOB* theGraph
	);

	HRESULT WUResubmit(
		BSTR* Wuids, int Wuids_nSizeIs, 
		bool Recompile, 
		int BlockTillFinishTimer, 
		ArrayOfEspException* Exceptions
	);

	HRESULT WUClusterJobQueueLOG(
		BSTR Cluster, 
		BSTR StartDate, 
		BSTR EndDate, 
		ArrayOfEspException* Exceptions, 
		ATLSOAP_BLOB* thefile
	);

	HRESULT WUGraphInfo(
		BSTR* Wuid, 
		BSTR* Name, 
		BSTR* GID, 
		int* BatchWU, 
		ArrayOfEspException* Exceptions, 
		bool* Running
	);

	HRESULT WUGVCGraphInfo(
		BSTR* Wuid, 
		BSTR* Name, 
		BSTR* GID, 
		int* BatchWU, 
		int SubgraphId, 
		ArrayOfEspException* Exceptions, 
		bool* Running, 
		BSTR* TheGraph
	);

	HRESULT WUResultBin(
		BSTR LogicalName, 
		BSTR* Wuid, 
		BSTR ResultName, 
		int* Sequence, 
		BSTR* Format, 
		__int64* Start, 
		int* Count, 
		ArrayOfEspException* Exceptions, 
		BSTR* Name, 
		int* Requested, 
		__int64* Total, 
		ATLSOAP_BLOB* Result
	);

	HRESULT WUClusterJobXLS(
		BSTR Cluster, 
		BSTR StartDate, 
		BSTR EndDate, 
		bool ShowAll, 
		BSTR BusinessStartTime, 
		BSTR BusinessEndTime, 
		ArrayOfEspException* Exceptions, 
		ATLSOAP_BLOB* Result
	);
};

typedef CWsWorkunitsT<> CWsWorkunits;

__if_not_exists(__ECLTimingData_entries)
{
extern __declspec(selectany) const _soapmapentry __ECLTimingData_entries[] =
{
	{ 
		0x002C70A1, 
		"Name", 
		L"Name", 
		sizeof("Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLTimingData, Name),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xB81A9882, 
		"GraphNum", 
		L"GraphNum", 
		sizeof("GraphNum")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLTimingData, GraphNum),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x65A0F30C, 
		"SubGraphNum", 
		L"SubGraphNum", 
		sizeof("SubGraphNum")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLTimingData, SubGraphNum),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x000137B4, 
		"GID", 
		L"GID", 
		sizeof("GID")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLTimingData, GID),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x00015584, 
		"Min", 
		L"Min", 
		sizeof("Min")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLTimingData, Min),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x00000A40, 
		"MS", 
		L"MS", 
		sizeof("MS")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLTimingData, MS),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __ECLTimingData_map =
{
	0x19079D76,
	"ECLTimingData",
	L"ECLTimingData",
	sizeof("ECLTimingData")-1,
	sizeof("ECLTimingData")-1,
	SOAPMAP_STRUCT,
	__ECLTimingData_entries,
	sizeof(ECLTimingData),
	6,
	-1,
	SOAPFLAG_NONE,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};
}

__if_not_exists(__LogicalFileUpload_entries)
{
extern __declspec(selectany) const _soapmapentry __LogicalFileUpload_entries[] =
{
	{ 
		0x00302162, 
		"Type", 
		L"Type", 
		sizeof("Type")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(LogicalFileUpload, Type),
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
		offsetof(LogicalFileUpload, Source),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x00DDB342, 
		"Destination", 
		L"Destination", 
		sizeof("Destination")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(LogicalFileUpload, Destination),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x1CB6FEBE, 
		"EventTag", 
		L"EventTag", 
		sizeof("EventTag")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(LogicalFileUpload, EventTag),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __LogicalFileUpload_map =
{
	0x1EBEFA00,
	"LogicalFileUpload",
	L"LogicalFileUpload",
	sizeof("LogicalFileUpload")-1,
	sizeof("LogicalFileUpload")-1,
	SOAPMAP_STRUCT,
	__LogicalFileUpload_entries,
	sizeof(LogicalFileUpload),
	4,
	-1,
	SOAPFLAG_NONE,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};
}

__if_not_exists(__ECLSchemaItem_entries)
{
extern __declspec(selectany) const _soapmapentry __ECLSchemaItem_entries[] =
{
	{ 
		0x7111D2CF, 
		"ColumnName", 
		L"ColumnName", 
		sizeof("ColumnName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLSchemaItem, ColumnName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x71158390, 
		"ColumnType", 
		L"ColumnType", 
		sizeof("ColumnType")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLSchemaItem, ColumnType),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x40736E6B, 
		"ColumnTypeCode", 
		L"ColumnTypeCode", 
		sizeof("ColumnTypeCode")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLSchemaItem, ColumnTypeCode),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x333A0C10, 
		"isConditional", 
		L"isConditional", 
		sizeof("isConditional")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLSchemaItem, isConditional),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __ECLSchemaItem_map =
{
	0x86219C14,
	"ECLSchemaItem",
	L"ECLSchemaItem",
	sizeof("ECLSchemaItem")-1,
	sizeof("ECLSchemaItem")-1,
	SOAPMAP_STRUCT,
	__ECLSchemaItem_entries,
	sizeof(ECLSchemaItem),
	4,
	-1,
	SOAPFLAG_NONE,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};
}

__if_not_exists(__ECLWorkflow_entries)
{
extern __declspec(selectany) const _soapmapentry __ECLWorkflow_entries[] =
{
	{ 
		0x0030E86A, 
		"WFID", 
		L"WFID", 
		sizeof("WFID")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkflow, WFID),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xB3938D63, 
		"EventName", 
		L"EventName", 
		sizeof("EventName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkflow, EventName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xB396EA27, 
		"EventText", 
		L"EventText", 
		sizeof("EventText")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkflow, EventText),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x04FB4829, 
		"Count", 
		L"Count", 
		sizeof("Count")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkflow, Count),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xA42D2463, 
		"CountRemaining", 
		L"CountRemaining", 
		sizeof("CountRemaining")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkflow, CountRemaining),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __ECLWorkflow_map =
{
	0x5248310F,
	"ECLWorkflow",
	L"ECLWorkflow",
	sizeof("ECLWorkflow")-1,
	sizeof("ECLWorkflow")-1,
	SOAPMAP_STRUCT,
	__ECLWorkflow_entries,
	sizeof(ECLWorkflow),
	5,
	-1,
	SOAPFLAG_NONE,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};
}

__if_not_exists(__ArrayOfECLSchemaItem_entries)
{
extern __declspec(selectany) const _soapmapentry __ArrayOfECLSchemaItem_entries[] =
{
	{ 
		0x86219C14, 
		"ECLSchemaItem", 
		L"ECLSchemaItem", 
		sizeof("ECLSchemaItem")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(ArrayOfECLSchemaItem, _ECLSchemaItem),
		NULL, 
		&__ECLSchemaItem_map, 
		0+1 
	},
	{ 
		0x86219C14, 
		"ECLSchemaItem", 
		L"ECLSchemaItem", 
		sizeof("ECLSchemaItem")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(ArrayOfECLSchemaItem, ___ECLSchemaItem_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __ArrayOfECLSchemaItem_map =
{
	0x8B807CC8,
	"ArrayOfECLSchemaItem",
	L"ArrayOfECLSchemaItem",
	sizeof("ArrayOfECLSchemaItem")-1,
	sizeof("ArrayOfECLSchemaItem")-1,
	SOAPMAP_STRUCT,
	__ArrayOfECLSchemaItem_entries,
	sizeof(ArrayOfECLSchemaItem),
	1,
	-1,
	SOAPFLAG_NONE,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};
}

__if_not_exists(__ECLResult_entries)
{
extern __declspec(selectany) const _soapmapentry __ECLResult_entries[] =
{
	{ 
		0x002C70A1, 
		"Name", 
		L"Name", 
		sizeof("Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLResult, Name),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x08276279, 
		"Sequence", 
		L"Sequence", 
		sizeof("Sequence")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLResult, Sequence),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x064B46FD, 
		"Value", 
		L"Value", 
		sizeof("Value")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLResult, Value),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x002B7A0E, 
		"Link", 
		L"Link", 
		sizeof("Link")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLResult, Link),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x2FE06021, 
		"FileName", 
		L"FileName", 
		sizeof("FileName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLResult, FileName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x500972E2, 
		"IsSupplied", 
		L"IsSupplied", 
		sizeof("IsSupplied")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLResult, IsSupplied),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xD82FA97C, 
		"ShowFileContent", 
		L"ShowFileContent", 
		sizeof("ShowFileContent")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLResult, ShowFileContent),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x062EE2C4, 
		"Total", 
		L"Total", 
		sizeof("Total")-1, 
		SOAPTYPE_LONG, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLResult, Total),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x9A998018, 
		"ECLSchemas", 
		L"ECLSchemas", 
		sizeof("ECLSchemas")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARRWRAPPER | SOAPFLAG_NULLABLE, 
		offsetof(ECLResult, ECLSchemas),
		NULL, 
		&__ArrayOfECLSchemaItem_map, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __ECLResult_map =
{
	0xDBBAF393,
	"ECLResult",
	L"ECLResult",
	sizeof("ECLResult")-1,
	sizeof("ECLResult")-1,
	SOAPMAP_STRUCT,
	__ECLResult_entries,
	sizeof(ECLResult),
	9,
	-1,
	SOAPFLAG_NONE,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};
}

__if_not_exists(__ECLSourceFile_entries)
{
extern __declspec(selectany) const _soapmapentry __ECLSourceFile_entries[] =
{
	{ 
		0xA4190762, 
		"FileCluster", 
		L"FileCluster", 
		sizeof("FileCluster")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLSourceFile, FileCluster),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x002C70A1, 
		"Name", 
		L"Name", 
		sizeof("Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLSourceFile, Name),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x37E8846B, 
		"IsSuperFile", 
		L"IsSuperFile", 
		sizeof("IsSuperFile")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLSourceFile, IsSuperFile),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x04FB4829, 
		"Count", 
		L"Count", 
		sizeof("Count")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLSourceFile, Count),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __ECLSourceFile_map =
{
	0xCCF6C605,
	"ECLSourceFile",
	L"ECLSourceFile",
	sizeof("ECLSourceFile")-1,
	sizeof("ECLSourceFile")-1,
	SOAPMAP_STRUCT,
	__ECLSourceFile_entries,
	sizeof(ECLSourceFile),
	4,
	-1,
	SOAPFLAG_NONE,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};
}

__if_not_exists(__ServerInfo_entries)
{
extern __declspec(selectany) const _soapmapentry __ServerInfo_entries[] =
{
	{ 
		0x002C70A1, 
		"Name", 
		L"Name", 
		sizeof("Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ServerInfo, Name),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xBA4461CD, 
		"NetAddress", 
		L"NetAddress", 
		sizeof("NetAddress")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ServerInfo, NetAddress),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __ServerInfo_map =
{
	0xE9004A63,
	"ServerInfo",
	L"ServerInfo",
	sizeof("ServerInfo")-1,
	sizeof("ServerInfo")-1,
	SOAPMAP_STRUCT,
	__ServerInfo_entries,
	sizeof(ServerInfo),
	2,
	-1,
	SOAPFLAG_NONE,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};
}

__if_not_exists(__ECLGraph_entries)
{
extern __declspec(selectany) const _soapmapentry __ECLGraph_entries[] =
{
	{ 
		0x002C70A1, 
		"Name", 
		L"Name", 
		sizeof("Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLGraph, Name),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x05962560, 
		"Label", 
		L"Label", 
		sizeof("Label")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLGraph, Label),
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
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLGraph, Type),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xC117ADC1, 
		"Running", 
		L"Running", 
		sizeof("Running")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLGraph, Running),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x3C250979, 
		"Complete", 
		L"Complete", 
		sizeof("Complete")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLGraph, Complete),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xAA5FD185, 
		"Failed", 
		L"Failed", 
		sizeof("Failed")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLGraph, Failed),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x65BA2BCE, 
		"RunningId", 
		L"RunningId", 
		sizeof("RunningId")-1, 
		SOAPTYPE_LONG, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLGraph, RunningId),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __ECLGraph_map =
{
	0x89C95166,
	"ECLGraph",
	L"ECLGraph",
	sizeof("ECLGraph")-1,
	sizeof("ECLGraph")-1,
	SOAPMAP_STRUCT,
	__ECLGraph_entries,
	sizeof(ECLGraph),
	7,
	-1,
	SOAPFLAG_NONE,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
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
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};
}

__if_not_exists(__ArrayOfEspException_entries)
{
extern __declspec(selectany) const _soapmapentry __ArrayOfEspException_entries[] =
{
	{ 
		0xC9B6E691, 
		"Source", 
		L"Source", 
		sizeof("Source")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ArrayOfEspException, Source),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xDCF0E6AF, 
		"Exception", 
		L"Exception", 
		sizeof("Exception")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(ArrayOfEspException, Exception),
		NULL, 
		&__EspException_map, 
		1+1 
	},
	{ 
		0xDCF0E6AF, 
		"Exception", 
		L"Exception", 
		sizeof("Exception")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(ArrayOfEspException, __Exception_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __ArrayOfEspException_map =
{
	0xC3B996AB,
	"ArrayOfEspException",
	L"ArrayOfEspException",
	sizeof("ArrayOfEspException")-1,
	sizeof("ArrayOfEspException")-1,
	SOAPMAP_STRUCT,
	__ArrayOfEspException_entries,
	sizeof(ArrayOfEspException),
	2,
	-1,
	SOAPFLAG_NONE,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};
}

__if_not_exists(__WUECLAttribute_entries)
{
extern __declspec(selectany) const _soapmapentry __WUECLAttribute_entries[] =
{
	{ 
		0x3474D1E7, 
		"ModuleName", 
		L"ModuleName", 
		sizeof("ModuleName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(WUECLAttribute, ModuleName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0CA09C15, 
		"AttributeName", 
		L"AttributeName", 
		sizeof("AttributeName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(WUECLAttribute, AttributeName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xAC87CCAE, 
		"IsLocked", 
		L"IsLocked", 
		sizeof("IsLocked")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(WUECLAttribute, IsLocked),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x3B0F47FB, 
		"IsCheckedOut", 
		L"IsCheckedOut", 
		sizeof("IsCheckedOut")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(WUECLAttribute, IsCheckedOut),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x3873FEEB, 
		"IsSandbox", 
		L"IsSandbox", 
		sizeof("IsSandbox")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(WUECLAttribute, IsSandbox),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xB77CB22D, 
		"IsOrphaned", 
		L"IsOrphaned", 
		sizeof("IsOrphaned")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(WUECLAttribute, IsOrphaned),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __WUECLAttribute_map =
{
	0x5DC0FFF4,
	"WUECLAttribute",
	L"WUECLAttribute",
	sizeof("WUECLAttribute")-1,
	sizeof("WUECLAttribute")-1,
	SOAPMAP_STRUCT,
	__WUECLAttribute_entries,
	sizeof(WUECLAttribute),
	6,
	-1,
	SOAPFLAG_NONE,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};
}

__if_not_exists(__ECLException_entries)
{
extern __declspec(selectany) const _soapmapentry __ECLException_entries[] =
{
	{ 
		0xC9B6E691, 
		"Source", 
		L"Source", 
		sizeof("Source")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLException, Source),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x12B6B37B, 
		"Severity", 
		L"Severity", 
		sizeof("Severity")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLException, Severity),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0026A2DB, 
		"Code", 
		L"Code", 
		sizeof("Code")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLException, Code),
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
		offsetof(ECLException, Message),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x2FE06021, 
		"FileName", 
		L"FileName", 
		sizeof("FileName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLException, FileName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xB8F22685, 
		"LineNo", 
		L"LineNo", 
		sizeof("LineNo")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLException, LineNo),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xA45F7B2E, 
		"Column", 
		L"Column", 
		sizeof("Column")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLException, Column),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __ECLException_map =
{
	0x28EF97A3,
	"ECLException",
	L"ECLException",
	sizeof("ECLException")-1,
	sizeof("ECLException")-1,
	SOAPMAP_STRUCT,
	__ECLException_entries,
	sizeof(ECLException),
	7,
	-1,
	SOAPFLAG_NONE,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};
}

__if_not_exists(__DebugValue_entries)
{
extern __declspec(selectany) const _soapmapentry __DebugValue_entries[] =
{
	{ 
		0x002C70A1, 
		"Name", 
		L"Name", 
		sizeof("Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DebugValue, Name),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x064B46FD, 
		"Value", 
		L"Value", 
		sizeof("Value")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DebugValue, Value),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __DebugValue_map =
{
	0x9E070444,
	"DebugValue",
	L"DebugValue",
	sizeof("DebugValue")-1,
	sizeof("DebugValue")-1,
	SOAPMAP_STRUCT,
	__DebugValue_entries,
	sizeof(DebugValue),
	2,
	-1,
	SOAPFLAG_NONE,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};
}

__if_not_exists(__WUActionResult_entries)
{
extern __declspec(selectany) const _soapmapentry __WUActionResult_entries[] =
{
	{ 
		0x0031B499, 
		"Wuid", 
		L"Wuid", 
		sizeof("Wuid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(WUActionResult, Wuid),
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
		offsetof(WUActionResult, Action),
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
		offsetof(WUActionResult, Result),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __WUActionResult_map =
{
	0x6497CE49,
	"WUActionResult",
	L"WUActionResult",
	sizeof("WUActionResult")-1,
	sizeof("WUActionResult")-1,
	SOAPMAP_STRUCT,
	__WUActionResult_entries,
	sizeof(WUActionResult),
	3,
	-1,
	SOAPFLAG_NONE,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};
}

__if_not_exists(__ECLQuery_entries)
{
extern __declspec(selectany) const _soapmapentry __ECLQuery_entries[] =
{
	{ 
		0x002FCD65, 
		"Text", 
		L"Text", 
		sizeof("Text")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLQuery, Text),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x00012BE3, 
		"Cpp", 
		L"Cpp", 
		sizeof("Cpp")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLQuery, Cpp),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xC6AB334A, 
		"ResTxt", 
		L"ResTxt", 
		sizeof("ResTxt")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLQuery, ResTxt),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x00012F9C, 
		"Dll", 
		L"Dll", 
		sizeof("Dll")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLQuery, Dll),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x3CCCE5DF, 
		"ThorLog", 
		L"ThorLog", 
		sizeof("ThorLog")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLQuery, ThorLog),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __ECLQuery_map =
{
	0x8A7FFCEA,
	"ECLQuery",
	L"ECLQuery",
	sizeof("ECLQuery")-1,
	sizeof("ECLQuery")-1,
	SOAPMAP_STRUCT,
	__ECLQuery_entries,
	sizeof(ECLQuery),
	5,
	-1,
	SOAPFLAG_NONE,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};
}

__if_not_exists(__ECLHelpFile_entries)
{
extern __declspec(selectany) const _soapmapentry __ECLHelpFile_entries[] =
{
	{ 
		0x002C70A1, 
		"Name", 
		L"Name", 
		sizeof("Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLHelpFile, Name),
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
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLHelpFile, Type),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __ECLHelpFile_map =
{
	0x6AF045BD,
	"ECLHelpFile",
	L"ECLHelpFile",
	sizeof("ECLHelpFile")-1,
	sizeof("ECLHelpFile")-1,
	SOAPMAP_STRUCT,
	__ECLHelpFile_entries,
	sizeof(ECLHelpFile),
	2,
	-1,
	SOAPFLAG_NONE,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};
}

__if_not_exists(__ECLTimer_entries)
{
extern __declspec(selectany) const _soapmapentry __ECLTimer_entries[] =
{
	{ 
		0x002C70A1, 
		"Name", 
		L"Name", 
		sizeof("Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLTimer, Name),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x064B46FD, 
		"Value", 
		L"Value", 
		sizeof("Value")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLTimer, Value),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x073E5849, 
		"count", 
		L"count", 
		sizeof("count")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLTimer, count),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xBB6D5413, 
		"GraphName", 
		L"GraphName", 
		sizeof("GraphName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLTimer, GraphName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x67ED9A09, 
		"SubGraphId", 
		L"SubGraphId", 
		sizeof("SubGraphId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLTimer, SubGraphId),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __ECLTimer_map =
{
	0x8AAFD235,
	"ECLTimer",
	L"ECLTimer",
	sizeof("ECLTimer")-1,
	sizeof("ECLTimer")-1,
	SOAPMAP_STRUCT,
	__ECLTimer_entries,
	sizeof(ECLTimer),
	5,
	-1,
	SOAPFLAG_NONE,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};
}

__if_not_exists(__ApplicationValue_entries)
{
extern __declspec(selectany) const _soapmapentry __ApplicationValue_entries[] =
{
	{ 
		0xC40F16F4, 
		"Application", 
		L"Application", 
		sizeof("Application")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ApplicationValue, Application),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x002C70A1, 
		"Name", 
		L"Name", 
		sizeof("Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ApplicationValue, Name),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x064B46FD, 
		"Value", 
		L"Value", 
		sizeof("Value")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ApplicationValue, Value),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __ApplicationValue_map =
{
	0x3662D671,
	"ApplicationValue",
	L"ApplicationValue",
	sizeof("ApplicationValue")-1,
	sizeof("ApplicationValue")-1,
	SOAPMAP_STRUCT,
	__ApplicationValue_entries,
	sizeof(ApplicationValue),
	3,
	-1,
	SOAPFLAG_NONE,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};
}

__if_not_exists(__ECLWorkunit_entries)
{
extern __declspec(selectany) const _soapmapentry __ECLWorkunit_entries[] =
{
	{ 
		0x0031B499, 
		"Wuid", 
		L"Wuid", 
		sizeof("Wuid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkunit, Wuid),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x05D8B24B, 
		"Owner", 
		L"Owner", 
		sizeof("Owner")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkunit, Owner),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x29F147E2, 
		"Cluster", 
		L"Cluster", 
		sizeof("Cluster")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkunit, Cluster),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x8D0F25C9, 
		"RoxieCluster", 
		L"RoxieCluster", 
		sizeof("RoxieCluster")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkunit, RoxieCluster),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x4A6D8BBC, 
		"Jobname", 
		L"Jobname", 
		sizeof("Jobname")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkunit, Jobname),
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
		offsetof(ECLWorkunit, Queue),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0AD4B7AE, 
		"StateID", 
		L"StateID", 
		sizeof("StateID")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkunit, StateID),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x061F39C1, 
		"State", 
		L"State", 
		sizeof("State")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkunit, State),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0AD4B75E, 
		"StateEx", 
		L"StateEx", 
		sizeof("StateEx")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkunit, StateEx),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xFAB05784, 
		"Description", 
		L"Description", 
		sizeof("Description")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkunit, Description),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x75F775AF, 
		"AddDrilldownFields", 
		L"AddDrilldownFields", 
		sizeof("AddDrilldownFields")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkunit, AddDrilldownFields),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x12A6CC6A, 
		"Protected", 
		L"Protected", 
		sizeof("Protected")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkunit, Protected),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x9EE034FC, 
		"Active", 
		L"Active", 
		sizeof("Active")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkunit, Active),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x9EE0341E, 
		"Action", 
		L"Action", 
		sizeof("Action")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkunit, Action),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xD8BFF41E, 
		"DateTimeScheduled", 
		L"DateTimeScheduled", 
		sizeof("DateTimeScheduled")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkunit, DateTimeScheduled),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xD4B57F58, 
		"PriorityClass", 
		L"PriorityClass", 
		sizeof("PriorityClass")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkunit, PriorityClass),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xD554DCBA, 
		"PriorityLevel", 
		L"PriorityLevel", 
		sizeof("PriorityLevel")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkunit, PriorityLevel),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0616225A, 
		"Scope", 
		L"Scope", 
		sizeof("Scope")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkunit, Scope),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x974E1610, 
		"Snapshot", 
		L"Snapshot", 
		sizeof("Snapshot")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkunit, Snapshot),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x2B9D213E, 
		"ResultLimit", 
		L"ResultLimit", 
		sizeof("ResultLimit")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkunit, ResultLimit),
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
		offsetof(ECLWorkunit, Archived),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x6F5E338F, 
		"EventSchedule", 
		L"EventSchedule", 
		sizeof("EventSchedule")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkunit, EventSchedule),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x8B0C627B, 
		"HaveSubGraphTimings", 
		L"HaveSubGraphTimings", 
		sizeof("HaveSubGraphTimings")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkunit, HaveSubGraphTimings),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x05FBA5F6, 
		"Query", 
		L"Query", 
		sizeof("Query")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkunit, Query),
		NULL, 
		&__ECLQuery_map, 
		-1 
	},
	{ 
		0x99DB9B73, 
		"Helpers", 
		L"Helpers", 
		sizeof("Helpers")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkunit, Helpers),
		NULL, 
		&__ECLHelpFile_map, 
		24+1 
	},
	{ 
		0x99DB9B73, 
		"Helpers", 
		L"Helpers", 
		sizeof("Helpers")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(ECLWorkunit, __Helpers_nSizeIs),
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
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkunit, Exceptions),
		NULL, 
		&__ECLException_map, 
		26+1 
	},
	{ 
		0x7B0DBD02, 
		"Exceptions", 
		L"Exceptions", 
		sizeof("Exceptions")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(ECLWorkunit, __Exceptions_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 
		0xADE44925, 
		"Graphs", 
		L"Graphs", 
		sizeof("Graphs")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkunit, Graphs),
		NULL, 
		&__ECLGraph_map, 
		28+1 
	},
	{ 
		0xADE44925, 
		"Graphs", 
		L"Graphs", 
		sizeof("Graphs")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(ECLWorkunit, __Graphs_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 
		0x2560C924, 
		"SourceFiles", 
		L"SourceFiles", 
		sizeof("SourceFiles")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkunit, SourceFiles),
		NULL, 
		&__ECLSourceFile_map, 
		30+1 
	},
	{ 
		0x2560C924, 
		"SourceFiles", 
		L"SourceFiles", 
		sizeof("SourceFiles")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(ECLWorkunit, __SourceFiles_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 
		0x9C238272, 
		"Results", 
		L"Results", 
		sizeof("Results")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkunit, Results),
		NULL, 
		&__ECLResult_map, 
		32+1 
	},
	{ 
		0x9C238272, 
		"Results", 
		L"Results", 
		sizeof("Results")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(ECLWorkunit, __Results_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 
		0xEA2F3679, 
		"Variables", 
		L"Variables", 
		sizeof("Variables")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkunit, Variables),
		NULL, 
		&__ECLResult_map, 
		34+1 
	},
	{ 
		0xEA2F3679, 
		"Variables", 
		L"Variables", 
		sizeof("Variables")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(ECLWorkunit, __Variables_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 
		0xCB9AE3D4, 
		"Timers", 
		L"Timers", 
		sizeof("Timers")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkunit, Timers),
		NULL, 
		&__ECLTimer_map, 
		36+1 
	},
	{ 
		0xCB9AE3D4, 
		"Timers", 
		L"Timers", 
		sizeof("Timers")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(ECLWorkunit, __Timers_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 
		0x5EE78D37, 
		"DebugValues", 
		L"DebugValues", 
		sizeof("DebugValues")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkunit, DebugValues),
		NULL, 
		&__DebugValue_map, 
		38+1 
	},
	{ 
		0x5EE78D37, 
		"DebugValues", 
		L"DebugValues", 
		sizeof("DebugValues")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(ECLWorkunit, __DebugValues_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 
		0x02BDA504, 
		"ApplicationValues", 
		L"ApplicationValues", 
		sizeof("ApplicationValues")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkunit, ApplicationValues),
		NULL, 
		&__ApplicationValue_map, 
		40+1 
	},
	{ 
		0x02BDA504, 
		"ApplicationValues", 
		L"ApplicationValues", 
		sizeof("ApplicationValues")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(ECLWorkunit, __ApplicationValues_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 
		0x4F4FA26E, 
		"Workflows", 
		L"Workflows", 
		sizeof("Workflows")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkunit, Workflows),
		NULL, 
		&__ECLWorkflow_map, 
		42+1 
	},
	{ 
		0x4F4FA26E, 
		"Workflows", 
		L"Workflows", 
		sizeof("Workflows")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(ECLWorkunit, __Workflows_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 
		0x4D32CE02, 
		"TimingData", 
		L"TimingData", 
		sizeof("TimingData")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkunit, TimingData),
		NULL, 
		&__ECLTimingData_map, 
		44+1 
	},
	{ 
		0x4D32CE02, 
		"TimingData", 
		L"TimingData", 
		sizeof("TimingData")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(ECLWorkunit, __TimingData_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 
		0xA8D2999D, 
		"AllowedClusters", 
		L"AllowedClusters", 
		sizeof("AllowedClusters")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkunit, AllowedClusters),
		NULL, 
		NULL, 
		46+1 
	},
	{ 
		0xA8D2999D, 
		"AllowedClusters", 
		L"AllowedClusters", 
		sizeof("AllowedClusters")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(ECLWorkunit, __AllowedClusters_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 
		0x9AFBF413, 
		"ErrorCount", 
		L"ErrorCount", 
		sizeof("ErrorCount")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkunit, ErrorCount),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xF4C7A4DF, 
		"WarningCount", 
		L"WarningCount", 
		sizeof("WarningCount")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkunit, WarningCount),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x6F61CCD5, 
		"InfoCount", 
		L"InfoCount", 
		sizeof("InfoCount")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkunit, InfoCount),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x285899DB, 
		"GraphCount", 
		L"GraphCount", 
		sizeof("GraphCount")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkunit, GraphCount),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0A03015A, 
		"SourceFileCount", 
		L"SourceFileCount", 
		sizeof("SourceFileCount")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkunit, SourceFileCount),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x2AFDB1A8, 
		"ResultCount", 
		L"ResultCount", 
		sizeof("ResultCount")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkunit, ResultCount),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x4042112F, 
		"VariableCount", 
		L"VariableCount", 
		sizeof("VariableCount")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkunit, VariableCount),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x202AF40A, 
		"TimerCount", 
		L"TimerCount", 
		sizeof("TimerCount")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkunit, TimerCount),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x659D7E80, 
		"HasDebugValue", 
		L"HasDebugValue", 
		sizeof("HasDebugValue")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkunit, HasDebugValue),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xC02ACD3A, 
		"ApplicationValueCount", 
		L"ApplicationValueCount", 
		sizeof("ApplicationValueCount")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkunit, ApplicationValueCount),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xDE184715, 
		"XmlParams", 
		L"XmlParams", 
		sizeof("XmlParams")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkunit, XmlParams),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xE388550C, 
		"AccessFlag", 
		L"AccessFlag", 
		sizeof("AccessFlag")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkunit, AccessFlag),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xCF5EA3BC, 
		"ClusterFlag", 
		L"ClusterFlag", 
		sizeof("ClusterFlag")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLWorkunit, ClusterFlag),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __ECLWorkunit_map =
{
	0x52507277,
	"ECLWorkunit",
	L"ECLWorkunit",
	sizeof("ECLWorkunit")-1,
	sizeof("ECLWorkunit")-1,
	SOAPMAP_STRUCT,
	__ECLWorkunit_entries,
	sizeof(ECLWorkunit),
	49,
	-1,
	SOAPFLAG_NONE,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};
}

__if_not_exists(__ECLGraphEx_entries)
{
extern __declspec(selectany) const _soapmapentry __ECLGraphEx_entries[] =
{
	{ 
		0x002C70A1, 
		"Name", 
		L"Name", 
		sizeof("Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLGraphEx, Name),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x05962560, 
		"Label", 
		L"Label", 
		sizeof("Label")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLGraphEx, Label),
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
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLGraphEx, Type),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0544FA72, 
		"Graph", 
		L"Graph", 
		sizeof("Graph")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLGraphEx, Graph),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xC117ADC1, 
		"Running", 
		L"Running", 
		sizeof("Running")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLGraphEx, Running),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x65BA2BCE, 
		"RunningId", 
		L"RunningId", 
		sizeof("RunningId")-1, 
		SOAPTYPE_LONG, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLGraphEx, RunningId),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x3C250979, 
		"Complete", 
		L"Complete", 
		sizeof("Complete")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLGraphEx, Complete),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xAA5FD185, 
		"Failed", 
		L"Failed", 
		sizeof("Failed")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLGraphEx, Failed),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __ECLGraphEx_map =
{
	0x21634C43,
	"ECLGraphEx",
	L"ECLGraphEx",
	sizeof("ECLGraphEx")-1,
	sizeof("ECLGraphEx")-1,
	SOAPMAP_STRUCT,
	__ECLGraphEx_entries,
	sizeof(ECLGraphEx),
	8,
	-1,
	SOAPFLAG_NONE,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};
}

__if_not_exists(__ECLJob_entries)
{
extern __declspec(selectany) const _soapmapentry __ECLJob_entries[] =
{
	{ 
		0x0031B499, 
		"Wuid", 
		L"Wuid", 
		sizeof("Wuid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLJob, Wuid),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0544FA72, 
		"Graph", 
		L"Graph", 
		sizeof("Graph")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLJob, Graph),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x061F39C1, 
		"State", 
		L"State", 
		sizeof("State")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLJob, State),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x3E853BB5, 
		"StartedDate", 
		L"StartedDate", 
		sizeof("StartedDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLJob, StartedDate),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xF33374E8, 
		"FinishedDate", 
		L"FinishedDate", 
		sizeof("FinishedDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLJob, FinishedDate),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x29F147E2, 
		"Cluster", 
		L"Cluster", 
		sizeof("Cluster")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLJob, Cluster),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xB81A9882, 
		"GraphNum", 
		L"GraphNum", 
		sizeof("GraphNum")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLJob, GraphNum),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x65A0F30C, 
		"SubGraphNum", 
		L"SubGraphNum", 
		sizeof("SubGraphNum")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLJob, SubGraphNum),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xFE38DBCD, 
		"NumOfRuns", 
		L"NumOfRuns", 
		sizeof("NumOfRuns")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLJob, NumOfRuns),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x00E319E6, 
		"Duration", 
		L"Duration", 
		sizeof("Duration")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLJob, Duration),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __ECLJob_map =
{
	0xA5DB536F,
	"ECLJob",
	L"ECLJob",
	sizeof("ECLJob")-1,
	sizeof("ECLJob")-1,
	SOAPMAP_STRUCT,
	__ECLJob_entries,
	sizeof(ECLJob),
	10,
	-1,
	SOAPFLAG_NONE,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};
}

__if_not_exists(__ScheduledWU_entries)
{
extern __declspec(selectany) const _soapmapentry __ScheduledWU_entries[] =
{
	{ 
		0x0031B499, 
		"Wuid", 
		L"Wuid", 
		sizeof("Wuid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ScheduledWU, Wuid),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0591C76B, 
		"EclServer", 
		L"EclServer", 
		sizeof("EclServer")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ScheduledWU, EclServer),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xB3938D63, 
		"EventName", 
		L"EventName", 
		sizeof("EventName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ScheduledWU, EventName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xB396EA27, 
		"EventText", 
		L"EventText", 
		sizeof("EventText")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ScheduledWU, EventText),
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
		offsetof(ScheduledWU, JobName),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __ScheduledWU_map =
{
	0x8032839D,
	"ScheduledWU",
	L"ScheduledWU",
	sizeof("ScheduledWU")-1,
	sizeof("ScheduledWU")-1,
	SOAPMAP_STRUCT,
	__ScheduledWU_entries,
	sizeof(ScheduledWU),
	5,
	-1,
	SOAPFLAG_NONE,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};
}

struct __CWsWorkunits_WUCreateAndUpdate_struct
{
	BSTR Wuid;
	int State;
	int StateOrig;
	BSTR Jobname;
	BSTR JobnameOrig;
	BSTR QueryText;
	int Action;
	BSTR Description;
	BSTR DescriptionOrig;
	bool AddDrilldownFields;
	int ResultLimit;
	bool Protected;
	bool ProtectedOrig;
	int PriorityClass;
	int PriorityLevel;
	BSTR Scope;
	BSTR ScopeOrig;
	BSTR ClusterSelection;
	BSTR ClusterOrig;
	BSTR XmlParams;
	BSTR ThorSlaveIP;
	DebugValue *DebugValues;
	int __DebugValues_nSizeIs;
	ApplicationValue *ApplicationValues;
	int __ApplicationValues_nSizeIs;
	ArrayOfEspException Exceptions;
	ECLWorkunit Workunit;
};

extern __declspec(selectany) const _soapmapentry __CWsWorkunits_WUCreateAndUpdate_entries[] =
{

	{
		0x0031B499, 
		"Wuid", 
		L"Wuid", 
		sizeof("Wuid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUCreateAndUpdate_struct, Wuid),
		NULL,
		NULL,
		-1,
	},
	{
		0x061F39C1, 
		"State", 
		L"State", 
		sizeof("State")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUCreateAndUpdate_struct, State),
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
		offsetof(__CWsWorkunits_WUCreateAndUpdate_struct, StateOrig),
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
		offsetof(__CWsWorkunits_WUCreateAndUpdate_struct, Jobname),
		NULL,
		NULL,
		-1,
	},
	{
		0x99B24E8D, 
		"JobnameOrig", 
		L"JobnameOrig", 
		sizeof("JobnameOrig")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUCreateAndUpdate_struct, JobnameOrig),
		NULL,
		NULL,
		-1,
	},
	{
		0x46D97E5B, 
		"QueryText", 
		L"QueryText", 
		sizeof("QueryText")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUCreateAndUpdate_struct, QueryText),
		NULL,
		NULL,
		-1,
	},
	{
		0x9EE0341E, 
		"Action", 
		L"Action", 
		sizeof("Action")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUCreateAndUpdate_struct, Action),
		NULL,
		NULL,
		-1,
	},
	{
		0xFAB05784, 
		"Description", 
		L"Description", 
		sizeof("Description")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUCreateAndUpdate_struct, Description),
		NULL,
		NULL,
		-1,
	},
	{
		0x0285BE55, 
		"DescriptionOrig", 
		L"DescriptionOrig", 
		sizeof("DescriptionOrig")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUCreateAndUpdate_struct, DescriptionOrig),
		NULL,
		NULL,
		-1,
	},
	{
		0x75F775AF, 
		"AddDrilldownFields", 
		L"AddDrilldownFields", 
		sizeof("AddDrilldownFields")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUCreateAndUpdate_struct, AddDrilldownFields),
		NULL,
		NULL,
		-1,
	},
	{
		0x2B9D213E, 
		"ResultLimit", 
		L"ResultLimit", 
		sizeof("ResultLimit")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUCreateAndUpdate_struct, ResultLimit),
		NULL,
		NULL,
		-1,
	},
	{
		0x12A6CC6A, 
		"Protected", 
		L"Protected", 
		sizeof("Protected")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUCreateAndUpdate_struct, Protected),
		NULL,
		NULL,
		-1,
	},
	{
		0x68D8363B, 
		"ProtectedOrig", 
		L"ProtectedOrig", 
		sizeof("ProtectedOrig")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUCreateAndUpdate_struct, ProtectedOrig),
		NULL,
		NULL,
		-1,
	},
	{
		0xD4B57F58, 
		"PriorityClass", 
		L"PriorityClass", 
		sizeof("PriorityClass")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUCreateAndUpdate_struct, PriorityClass),
		NULL,
		NULL,
		-1,
	},
	{
		0xD554DCBA, 
		"PriorityLevel", 
		L"PriorityLevel", 
		sizeof("PriorityLevel")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUCreateAndUpdate_struct, PriorityLevel),
		NULL,
		NULL,
		-1,
	},
	{
		0x0616225A, 
		"Scope", 
		L"Scope", 
		sizeof("Scope")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUCreateAndUpdate_struct, Scope),
		NULL,
		NULL,
		-1,
	},
	{
		0x8EE1042B, 
		"ScopeOrig", 
		L"ScopeOrig", 
		sizeof("ScopeOrig")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUCreateAndUpdate_struct, ScopeOrig),
		NULL,
		NULL,
		-1,
	},
	{
		0xCF627268, 
		"ClusterSelection", 
		L"ClusterSelection", 
		sizeof("ClusterSelection")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUCreateAndUpdate_struct, ClusterSelection),
		NULL,
		NULL,
		-1,
	},
	{
		0xCF63ADB3, 
		"ClusterOrig", 
		L"ClusterOrig", 
		sizeof("ClusterOrig")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUCreateAndUpdate_struct, ClusterOrig),
		NULL,
		NULL,
		-1,
	},
	{
		0xDE184715, 
		"XmlParams", 
		L"XmlParams", 
		sizeof("XmlParams")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUCreateAndUpdate_struct, XmlParams),
		NULL,
		NULL,
		-1,
	},
	{
		0x1628C5D1, 
		"ThorSlaveIP", 
		L"ThorSlaveIP", 
		sizeof("ThorSlaveIP")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUCreateAndUpdate_struct, ThorSlaveIP),
		NULL,
		NULL,
		-1,
	},
	{
		0x5EE78D37, 
		"DebugValues", 
		L"DebugValues", 
		sizeof("DebugValues")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUCreateAndUpdate_struct, DebugValues),
		NULL,
		&__DebugValue_map,
		21+1,
	},
	{
		0x5EE78D37,
		"__DebugValues_nSizeIs",
		L"__DebugValues_nSizeIs",
		sizeof("__DebugValues_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsWorkunits_WUCreateAndUpdate_struct, __DebugValues_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x02BDA504, 
		"ApplicationValues", 
		L"ApplicationValues", 
		sizeof("ApplicationValues")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUCreateAndUpdate_struct, ApplicationValues),
		NULL,
		&__ApplicationValue_map,
		23+1,
	},
	{
		0x02BDA504,
		"__ApplicationValues_nSizeIs",
		L"__ApplicationValues_nSizeIs",
		sizeof("__ApplicationValues_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsWorkunits_WUCreateAndUpdate_struct, __ApplicationValues_nSizeIs),
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
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUCreateAndUpdate_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0xBC9E1003, 
		"Workunit", 
		L"Workunit", 
		sizeof("Workunit")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUCreateAndUpdate_struct, Workunit),
		NULL,
		&__ECLWorkunit_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsWorkunits_WUCreateAndUpdate_map =
{
	0x53FCFD7E,
	"WUCreateAndUpdate",
	L"WUUpdateResponse",
	sizeof("WUCreateAndUpdate")-1,
	sizeof("WUUpdateResponse")-1,
	SOAPMAP_FUNC,
	__CWsWorkunits_WUCreateAndUpdate_entries,
	sizeof(__CWsWorkunits_WUCreateAndUpdate_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};


struct __CWsWorkunits_WUSubmit_struct
{
	BSTR Wuid;
	BSTR Cluster;
	BSTR Queue;
	BSTR Snapshot;
	int MaxRunTime;
	int BlockTillFinishTimer;
	bool SyntaxCheck;
	ArrayOfEspException Exceptions;
};

extern __declspec(selectany) const _soapmapentry __CWsWorkunits_WUSubmit_entries[] =
{

	{
		0x0031B499, 
		"Wuid", 
		L"Wuid", 
		sizeof("Wuid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUSubmit_struct, Wuid),
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
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUSubmit_struct, Cluster),
		NULL,
		NULL,
		-1,
	},
	{
		0x05FBA645, 
		"Queue", 
		L"Queue", 
		sizeof("Queue")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUSubmit_struct, Queue),
		NULL,
		NULL,
		-1,
	},
	{
		0x974E1610, 
		"Snapshot", 
		L"Snapshot", 
		sizeof("Snapshot")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUSubmit_struct, Snapshot),
		NULL,
		NULL,
		-1,
	},
	{
		0xDA39AD8A, 
		"MaxRunTime", 
		L"MaxRunTime", 
		sizeof("MaxRunTime")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUSubmit_struct, MaxRunTime),
		NULL,
		NULL,
		-1,
	},
	{
		0x032F6A42, 
		"BlockTillFinishTimer", 
		L"BlockTillFinishTimer", 
		sizeof("BlockTillFinishTimer")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUSubmit_struct, BlockTillFinishTimer),
		NULL,
		NULL,
		-1,
	},
	{
		0x890D9DA5, 
		"SyntaxCheck", 
		L"SyntaxCheck", 
		sizeof("SyntaxCheck")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUSubmit_struct, SyntaxCheck),
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
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUSubmit_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsWorkunits_WUSubmit_map =
{
	0xEF9FE0AF,
	"WUSubmit",
	L"WUSubmitResponse",
	sizeof("WUSubmit")-1,
	sizeof("WUSubmitResponse")-1,
	SOAPMAP_FUNC,
	__CWsWorkunits_WUSubmit_entries,
	sizeof(__CWsWorkunits_WUSubmit_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};


struct __CWsWorkunits_WUSchedule_struct
{
	BSTR Wuid;
	BSTR Cluster;
	BSTR Queue;
	BSTR Snapshot;
	BSTR When;
	int MaxRunTime;
	ArrayOfEspException Exceptions;
};

extern __declspec(selectany) const _soapmapentry __CWsWorkunits_WUSchedule_entries[] =
{

	{
		0x0031B499, 
		"Wuid", 
		L"Wuid", 
		sizeof("Wuid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUSchedule_struct, Wuid),
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
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUSchedule_struct, Cluster),
		NULL,
		NULL,
		-1,
	},
	{
		0x05FBA645, 
		"Queue", 
		L"Queue", 
		sizeof("Queue")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUSchedule_struct, Queue),
		NULL,
		NULL,
		-1,
	},
	{
		0x974E1610, 
		"Snapshot", 
		L"Snapshot", 
		sizeof("Snapshot")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUSchedule_struct, Snapshot),
		NULL,
		NULL,
		-1,
	},
	{
		0x00317CD2, 
		"When", 
		L"When", 
		sizeof("When")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUSchedule_struct, When),
		NULL,
		NULL,
		-1,
	},
	{
		0xDA39AD8A, 
		"MaxRunTime", 
		L"MaxRunTime", 
		sizeof("MaxRunTime")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUSchedule_struct, MaxRunTime),
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
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUSchedule_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsWorkunits_WUSchedule_map =
{
	0x04868888,
	"WUSchedule",
	L"WUScheduleResponse",
	sizeof("WUSchedule")-1,
	sizeof("WUScheduleResponse")-1,
	SOAPMAP_FUNC,
	__CWsWorkunits_WUSchedule_entries,
	sizeof(__CWsWorkunits_WUSchedule_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};


struct __CWsWorkunits_WUPushEvent_struct
{
	BSTR EventName;
	BSTR EventText;
	ArrayOfEspException Exceptions;
};

extern __declspec(selectany) const _soapmapentry __CWsWorkunits_WUPushEvent_entries[] =
{

	{
		0xB3938D63, 
		"EventName", 
		L"EventName", 
		sizeof("EventName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUPushEvent_struct, EventName),
		NULL,
		NULL,
		-1,
	},
	{
		0xB396EA27, 
		"EventText", 
		L"EventText", 
		sizeof("EventText")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUPushEvent_struct, EventText),
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
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUPushEvent_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsWorkunits_WUPushEvent_map =
{
	0x42A74C3D,
	"WUPushEvent",
	L"WUPushEventResponse",
	sizeof("WUPushEvent")-1,
	sizeof("WUPushEventResponse")-1,
	SOAPMAP_FUNC,
	__CWsWorkunits_WUPushEvent_entries,
	sizeof(__CWsWorkunits_WUPushEvent_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};


struct __CWsWorkunits_WUUpdate_struct
{
	BSTR Wuid;
	int State;
	int StateOrig;
	BSTR Jobname;
	BSTR JobnameOrig;
	BSTR QueryText;
	int Action;
	BSTR Description;
	BSTR DescriptionOrig;
	bool AddDrilldownFields;
	int ResultLimit;
	bool Protected;
	bool ProtectedOrig;
	int PriorityClass;
	int PriorityLevel;
	BSTR Scope;
	BSTR ScopeOrig;
	BSTR ClusterSelection;
	BSTR ClusterOrig;
	BSTR XmlParams;
	BSTR ThorSlaveIP;
	DebugValue *DebugValues;
	int __DebugValues_nSizeIs;
	ApplicationValue *ApplicationValues;
	int __ApplicationValues_nSizeIs;
	ArrayOfEspException Exceptions;
	ECLWorkunit Workunit;
};

extern __declspec(selectany) const _soapmapentry __CWsWorkunits_WUUpdate_entries[] =
{

	{
		0x0031B499, 
		"Wuid", 
		L"Wuid", 
		sizeof("Wuid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUUpdate_struct, Wuid),
		NULL,
		NULL,
		-1,
	},
	{
		0x061F39C1, 
		"State", 
		L"State", 
		sizeof("State")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUUpdate_struct, State),
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
		offsetof(__CWsWorkunits_WUUpdate_struct, StateOrig),
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
		offsetof(__CWsWorkunits_WUUpdate_struct, Jobname),
		NULL,
		NULL,
		-1,
	},
	{
		0x99B24E8D, 
		"JobnameOrig", 
		L"JobnameOrig", 
		sizeof("JobnameOrig")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUUpdate_struct, JobnameOrig),
		NULL,
		NULL,
		-1,
	},
	{
		0x46D97E5B, 
		"QueryText", 
		L"QueryText", 
		sizeof("QueryText")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUUpdate_struct, QueryText),
		NULL,
		NULL,
		-1,
	},
	{
		0x9EE0341E, 
		"Action", 
		L"Action", 
		sizeof("Action")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUUpdate_struct, Action),
		NULL,
		NULL,
		-1,
	},
	{
		0xFAB05784, 
		"Description", 
		L"Description", 
		sizeof("Description")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUUpdate_struct, Description),
		NULL,
		NULL,
		-1,
	},
	{
		0x0285BE55, 
		"DescriptionOrig", 
		L"DescriptionOrig", 
		sizeof("DescriptionOrig")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUUpdate_struct, DescriptionOrig),
		NULL,
		NULL,
		-1,
	},
	{
		0x75F775AF, 
		"AddDrilldownFields", 
		L"AddDrilldownFields", 
		sizeof("AddDrilldownFields")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUUpdate_struct, AddDrilldownFields),
		NULL,
		NULL,
		-1,
	},
	{
		0x2B9D213E, 
		"ResultLimit", 
		L"ResultLimit", 
		sizeof("ResultLimit")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUUpdate_struct, ResultLimit),
		NULL,
		NULL,
		-1,
	},
	{
		0x12A6CC6A, 
		"Protected", 
		L"Protected", 
		sizeof("Protected")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUUpdate_struct, Protected),
		NULL,
		NULL,
		-1,
	},
	{
		0x68D8363B, 
		"ProtectedOrig", 
		L"ProtectedOrig", 
		sizeof("ProtectedOrig")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUUpdate_struct, ProtectedOrig),
		NULL,
		NULL,
		-1,
	},
	{
		0xD4B57F58, 
		"PriorityClass", 
		L"PriorityClass", 
		sizeof("PriorityClass")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUUpdate_struct, PriorityClass),
		NULL,
		NULL,
		-1,
	},
	{
		0xD554DCBA, 
		"PriorityLevel", 
		L"PriorityLevel", 
		sizeof("PriorityLevel")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUUpdate_struct, PriorityLevel),
		NULL,
		NULL,
		-1,
	},
	{
		0x0616225A, 
		"Scope", 
		L"Scope", 
		sizeof("Scope")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUUpdate_struct, Scope),
		NULL,
		NULL,
		-1,
	},
	{
		0x8EE1042B, 
		"ScopeOrig", 
		L"ScopeOrig", 
		sizeof("ScopeOrig")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUUpdate_struct, ScopeOrig),
		NULL,
		NULL,
		-1,
	},
	{
		0xCF627268, 
		"ClusterSelection", 
		L"ClusterSelection", 
		sizeof("ClusterSelection")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUUpdate_struct, ClusterSelection),
		NULL,
		NULL,
		-1,
	},
	{
		0xCF63ADB3, 
		"ClusterOrig", 
		L"ClusterOrig", 
		sizeof("ClusterOrig")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUUpdate_struct, ClusterOrig),
		NULL,
		NULL,
		-1,
	},
	{
		0xDE184715, 
		"XmlParams", 
		L"XmlParams", 
		sizeof("XmlParams")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUUpdate_struct, XmlParams),
		NULL,
		NULL,
		-1,
	},
	{
		0x1628C5D1, 
		"ThorSlaveIP", 
		L"ThorSlaveIP", 
		sizeof("ThorSlaveIP")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUUpdate_struct, ThorSlaveIP),
		NULL,
		NULL,
		-1,
	},
	{
		0x5EE78D37, 
		"DebugValues", 
		L"DebugValues", 
		sizeof("DebugValues")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUUpdate_struct, DebugValues),
		NULL,
		&__DebugValue_map,
		21+1,
	},
	{
		0x5EE78D37,
		"__DebugValues_nSizeIs",
		L"__DebugValues_nSizeIs",
		sizeof("__DebugValues_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsWorkunits_WUUpdate_struct, __DebugValues_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x02BDA504, 
		"ApplicationValues", 
		L"ApplicationValues", 
		sizeof("ApplicationValues")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUUpdate_struct, ApplicationValues),
		NULL,
		&__ApplicationValue_map,
		23+1,
	},
	{
		0x02BDA504,
		"__ApplicationValues_nSizeIs",
		L"__ApplicationValues_nSizeIs",
		sizeof("__ApplicationValues_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsWorkunits_WUUpdate_struct, __ApplicationValues_nSizeIs),
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
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUUpdate_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0xBC9E1003, 
		"Workunit", 
		L"Workunit", 
		sizeof("Workunit")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUUpdate_struct, Workunit),
		NULL,
		&__ECLWorkunit_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsWorkunits_WUUpdate_map =
{
	0x53FCFD7E,
	"WUUpdate",
	L"WUUpdateResponse",
	sizeof("WUUpdate")-1,
	sizeof("WUUpdateResponse")-1,
	SOAPMAP_FUNC,
	__CWsWorkunits_WUUpdate_entries,
	sizeof(__CWsWorkunits_WUUpdate_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};


struct __CWsWorkunits_WUAddLocalFileToWorkunit_struct
{
	BSTR Wuid;
	BSTR Name;
	BSTR Val;
	BSTR DefVal;
	int Type;
	int Length;
	ArrayOfEspException Exceptions;
	BSTR Result;
};

extern __declspec(selectany) const _soapmapentry __CWsWorkunits_WUAddLocalFileToWorkunit_entries[] =
{

	{
		0x0031B499, 
		"Wuid", 
		L"Wuid", 
		sizeof("Wuid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUAddLocalFileToWorkunit_struct, Wuid),
		NULL,
		NULL,
		-1,
	},
	{
		0x002C70A1, 
		"Name", 
		L"Name", 
		sizeof("Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUAddLocalFileToWorkunit_struct, Name),
		NULL,
		NULL,
		-1,
	},
	{
		0x00017AC3, 
		"Val", 
		L"Val", 
		sizeof("Val")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUAddLocalFileToWorkunit_struct, Val),
		NULL,
		NULL,
		-1,
	},
	{
		0xA5FBDF12, 
		"DefVal", 
		L"DefVal", 
		sizeof("DefVal")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUAddLocalFileToWorkunit_struct, DefVal),
		NULL,
		NULL,
		-1,
	},
	{
		0x00302162, 
		"Type", 
		L"Type", 
		sizeof("Type")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUAddLocalFileToWorkunit_struct, Type),
		NULL,
		NULL,
		-1,
	},
	{
		0xB8A9D1E2, 
		"Length", 
		L"Length", 
		sizeof("Length")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUAddLocalFileToWorkunit_struct, Length),
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
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUAddLocalFileToWorkunit_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0xC6ABBE1F, 
		"Result", 
		L"Result", 
		sizeof("Result")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUAddLocalFileToWorkunit_struct, Result),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsWorkunits_WUAddLocalFileToWorkunit_map =
{
	0xC60E2355,
	"WUAddLocalFileToWorkunit",
	L"WUAddLocalFileToWorkunitResponse",
	sizeof("WUAddLocalFileToWorkunit")-1,
	sizeof("WUAddLocalFileToWorkunitResponse")-1,
	SOAPMAP_FUNC,
	__CWsWorkunits_WUAddLocalFileToWorkunit_entries,
	sizeof(__CWsWorkunits_WUAddLocalFileToWorkunit_struct),
	4,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};


struct __CWsWorkunits_WUExport_struct
{
	BSTR Cluster;
	BSTR Owner;
	BSTR State;
	BSTR StartDate;
	BSTR EndDate;
	BSTR ECL;
	BSTR Jobname;
	ArrayOfEspException Exceptions;
	ATLSOAP_BLOB ExportData;
};

extern __declspec(selectany) const _soapmapentry __CWsWorkunits_WUExport_entries[] =
{

	{
		0x29F147E2, 
		"Cluster", 
		L"Cluster", 
		sizeof("Cluster")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUExport_struct, Cluster),
		NULL,
		NULL,
		-1,
	},
	{
		0x05D8B24B, 
		"Owner", 
		L"Owner", 
		sizeof("Owner")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUExport_struct, Owner),
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
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUExport_struct, State),
		NULL,
		NULL,
		-1,
	},
	{
		0x0F44464C, 
		"StartDate", 
		L"StartDate", 
		sizeof("StartDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUExport_struct, StartDate),
		NULL,
		NULL,
		-1,
	},
	{
		0xC7425D55, 
		"EndDate", 
		L"EndDate", 
		sizeof("EndDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUExport_struct, EndDate),
		NULL,
		NULL,
		-1,
	},
	{
		0x00012E74, 
		"ECL", 
		L"ECL", 
		sizeof("ECL")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUExport_struct, ECL),
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
		offsetof(__CWsWorkunits_WUExport_struct, Jobname),
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
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUExport_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0x80C1B85C, 
		"ExportData", 
		L"ExportData", 
		sizeof("ExportData")-1, 
		SOAPTYPE_BASE64BINARY, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUExport_struct, ExportData),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsWorkunits_WUExport_map =
{
	0x7CA1765D,
	"WUExport",
	L"WUExportResponse",
	sizeof("WUExport")-1,
	sizeof("WUExportResponse")-1,
	SOAPMAP_FUNC,
	__CWsWorkunits_WUExport_entries,
	sizeof(__CWsWorkunits_WUExport_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};


struct __CWsWorkunits_WUQuery_struct
{
	BSTR Wuid;
	BSTR Type;
	BSTR Cluster;
	BSTR RoxieCluster;
	BSTR Owner;
	BSTR State;
	BSTR StartDate;
	BSTR EndDate;
	BSTR ECL;
	BSTR Jobname;
	BSTR LogicalFile;
	BSTR LogicalFileSearchType;
	BSTR ApplicationName;
	BSTR ApplicationKey;
	BSTR ApplicationData;
	BSTR After;
	BSTR Before;
	int Count;
	__int64 PageSize;
	__int64 PageStartFrom;
	__int64 PageEndAt;
	BSTR Sortby;
	bool Descending;
	ArrayOfEspException Exceptions;
	BSTR Current;
	BSTR Next;
	__int64 PrevPage;
	__int64 NextPage;
	__int64 LastPage;
	int NumWUs;
	bool First;
	BSTR BasicQuery;
	BSTR Filters;
	ECLWorkunit *Workunits;
	int __Workunits_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CWsWorkunits_WUQuery_entries[] =
{

	{
		0x0031B499, 
		"Wuid", 
		L"Wuid", 
		sizeof("Wuid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUQuery_struct, Wuid),
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
		offsetof(__CWsWorkunits_WUQuery_struct, Type),
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
		offsetof(__CWsWorkunits_WUQuery_struct, Cluster),
		NULL,
		NULL,
		-1,
	},
	{
		0x8D0F25C9, 
		"RoxieCluster", 
		L"RoxieCluster", 
		sizeof("RoxieCluster")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUQuery_struct, RoxieCluster),
		NULL,
		NULL,
		-1,
	},
	{
		0x05D8B24B, 
		"Owner", 
		L"Owner", 
		sizeof("Owner")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUQuery_struct, Owner),
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
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUQuery_struct, State),
		NULL,
		NULL,
		-1,
	},
	{
		0x0F44464C, 
		"StartDate", 
		L"StartDate", 
		sizeof("StartDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUQuery_struct, StartDate),
		NULL,
		NULL,
		-1,
	},
	{
		0xC7425D55, 
		"EndDate", 
		L"EndDate", 
		sizeof("EndDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUQuery_struct, EndDate),
		NULL,
		NULL,
		-1,
	},
	{
		0x00012E74, 
		"ECL", 
		L"ECL", 
		sizeof("ECL")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUQuery_struct, ECL),
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
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUQuery_struct, Jobname),
		NULL,
		NULL,
		-1,
	},
	{
		0xCC00EDDB, 
		"LogicalFile", 
		L"LogicalFile", 
		sizeof("LogicalFile")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUQuery_struct, LogicalFile),
		NULL,
		NULL,
		-1,
	},
	{
		0xB57B61F3, 
		"LogicalFileSearchType", 
		L"LogicalFileSearchType", 
		sizeof("LogicalFileSearchType")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUQuery_struct, LogicalFileSearchType),
		NULL,
		NULL,
		-1,
	},
	{
		0xD315E195, 
		"ApplicationName", 
		L"ApplicationName", 
		sizeof("ApplicationName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUQuery_struct, ApplicationName),
		NULL,
		NULL,
		-1,
	},
	{
		0x8A466EFD, 
		"ApplicationKey", 
		L"ApplicationKey", 
		sizeof("ApplicationKey")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUQuery_struct, ApplicationKey),
		NULL,
		NULL,
		-1,
	},
	{
		0xD31066AE, 
		"ApplicationData", 
		L"ApplicationData", 
		sizeof("ApplicationData")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUQuery_struct, ApplicationData),
		NULL,
		NULL,
		-1,
	},
	{
		0x04D22252, 
		"After", 
		L"After", 
		sizeof("After")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUQuery_struct, After),
		NULL,
		NULL,
		-1,
	},
	{
		0xA151FA53, 
		"Before", 
		L"Before", 
		sizeof("Before")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUQuery_struct, Before),
		NULL,
		NULL,
		-1,
	},
	{
		0x04FB4829, 
		"Count", 
		L"Count", 
		sizeof("Count")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUQuery_struct, Count),
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
		offsetof(__CWsWorkunits_WUQuery_struct, PageSize),
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
		offsetof(__CWsWorkunits_WUQuery_struct, PageStartFrom),
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
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUQuery_struct, PageEndAt),
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
		offsetof(__CWsWorkunits_WUQuery_struct, Sortby),
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
		offsetof(__CWsWorkunits_WUQuery_struct, Descending),
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
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUQuery_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0x3EB8A103, 
		"Current", 
		L"Current", 
		sizeof("Current")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUQuery_struct, Current),
		NULL,
		NULL,
		-1,
	},
	{
		0x002C831F, 
		"Next", 
		L"Next", 
		sizeof("Next")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUQuery_struct, Next),
		NULL,
		NULL,
		-1,
	},
	{
		0x101F6FDA, 
		"PrevPage", 
		L"PrevPage", 
		sizeof("PrevPage")-1, 
		SOAPTYPE_LONG, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUQuery_struct, PrevPage),
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
		offsetof(__CWsWorkunits_WUQuery_struct, NextPage),
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
		offsetof(__CWsWorkunits_WUQuery_struct, LastPage),
		NULL,
		NULL,
		-1,
	},
	{
		0xBE74D6CF, 
		"NumWUs", 
		L"NumWUs", 
		sizeof("NumWUs")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUQuery_struct, NumWUs),
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
		offsetof(__CWsWorkunits_WUQuery_struct, First),
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
		offsetof(__CWsWorkunits_WUQuery_struct, BasicQuery),
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
		offsetof(__CWsWorkunits_WUQuery_struct, Filters),
		NULL,
		NULL,
		-1,
	},
	{
		0x506010D6, 
		"Workunits", 
		L"Workunits", 
		sizeof("Workunits")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUQuery_struct, Workunits),
		NULL,
		&__ECLWorkunit_map,
		33+1,
	},
	{
		0x506010D6,
		"__Workunits_nSizeIs",
		L"__Workunits_nSizeIs",
		sizeof("__Workunits_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsWorkunits_WUQuery_struct, __Workunits_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsWorkunits_WUQuery_map =
{
	0x3F398D31,
	"WUQuery",
	L"WUQueryResponse",
	sizeof("WUQuery")-1,
	sizeof("WUQueryResponse")-1,
	SOAPMAP_FUNC,
	__CWsWorkunits_WUQuery_entries,
	sizeof(__CWsWorkunits_WUQuery_struct),
	28,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};


struct __CWsWorkunits_WUFile_struct
{
	BSTR Name;
	BSTR Wuid;
	BSTR Type;
	BSTR SlaveIP;
	ArrayOfEspException Exceptions;
	ATLSOAP_BLOB thefile;
};

extern __declspec(selectany) const _soapmapentry __CWsWorkunits_WUFile_entries[] =
{

	{
		0x002C70A1, 
		"Name", 
		L"Name", 
		sizeof("Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUFile_struct, Name),
		NULL,
		NULL,
		-1,
	},
	{
		0x0031B499, 
		"Wuid", 
		L"Wuid", 
		sizeof("Wuid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUFile_struct, Wuid),
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
		offsetof(__CWsWorkunits_WUFile_struct, Type),
		NULL,
		NULL,
		-1,
	},
	{
		0xF82C8B74, 
		"SlaveIP", 
		L"SlaveIP", 
		sizeof("SlaveIP")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUFile_struct, SlaveIP),
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
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUFile_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0xDB596F61, 
		"thefile", 
		L"thefile", 
		sizeof("thefile")-1, 
		SOAPTYPE_BASE64BINARY, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUFile_struct, thefile),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsWorkunits_WUFile_map =
{
	0xEE5658BD,
	"WUFile",
	L"WULogFileResponse",
	sizeof("WUFile")-1,
	sizeof("WULogFileResponse")-1,
	SOAPMAP_FUNC,
	__CWsWorkunits_WUFile_entries,
	sizeof(__CWsWorkunits_WUFile_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};


struct __CWsWorkunits_WUDelete_struct
{
	BSTR *Wuids;
	int __Wuids_nSizeIs;
	int BlockTillFinishTimer;
	ArrayOfEspException Exceptions;
	WUActionResult *ActionResults;
	int __ActionResults_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CWsWorkunits_WUDelete_entries[] =
{

	{
		0x0668482C, 
		"Wuids", 
		L"Wuids", 
		sizeof("Wuids")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUDelete_struct, Wuids),
		NULL,
		NULL,
		0+1,
	},
	{
		0x0668482C,
		"__Wuids_nSizeIs",
		L"__Wuids_nSizeIs",
		sizeof("__Wuids_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsWorkunits_WUDelete_struct, __Wuids_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x032F6A42, 
		"BlockTillFinishTimer", 
		L"BlockTillFinishTimer", 
		sizeof("BlockTillFinishTimer")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUDelete_struct, BlockTillFinishTimer),
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
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUDelete_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0xCDF828D0, 
		"ActionResults", 
		L"ActionResults", 
		sizeof("ActionResults")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUDelete_struct, ActionResults),
		NULL,
		&__WUActionResult_map,
		4+1,
	},
	{
		0xCDF828D0,
		"__ActionResults_nSizeIs",
		L"__ActionResults_nSizeIs",
		sizeof("__ActionResults_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsWorkunits_WUDelete_struct, __ActionResults_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsWorkunits_WUDelete_map =
{
	0x98B1814E,
	"WUDelete",
	L"WUDeleteResponse",
	sizeof("WUDelete")-1,
	sizeof("WUDeleteResponse")-1,
	SOAPMAP_FUNC,
	__CWsWorkunits_WUDelete_entries,
	sizeof(__CWsWorkunits_WUDelete_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};


struct __CWsWorkunits_WUAction_struct
{
	BSTR *Wuids;
	int __Wuids_nSizeIs;
	BSTR ActionType;
	BSTR Cluster;
	BSTR Owner;
	BSTR State;
	BSTR StartDate;
	BSTR EndDate;
	BSTR ECL;
	BSTR Jobname;
	BSTR Test;
	BSTR CurrentPage;
	BSTR PageSize;
	BSTR Sortby;
	bool Descending;
	BSTR EclServer;
	BSTR EventName;
	BSTR PageFrom;
	int BlockTillFinishTimer;
	ArrayOfEspException Exceptions;
	WUActionResult *ActionResults;
	int __ActionResults_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CWsWorkunits_WUAction_entries[] =
{

	{
		0x0668482C, 
		"Wuids", 
		L"Wuids", 
		sizeof("Wuids")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUAction_struct, Wuids),
		NULL,
		NULL,
		0+1,
	},
	{
		0x0668482C,
		"__Wuids_nSizeIs",
		L"__Wuids_nSizeIs",
		sizeof("__Wuids_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsWorkunits_WUAction_struct, __Wuids_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xBE293480, 
		"ActionType", 
		L"ActionType", 
		sizeof("ActionType")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUAction_struct, ActionType),
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
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUAction_struct, Cluster),
		NULL,
		NULL,
		-1,
	},
	{
		0x05D8B24B, 
		"Owner", 
		L"Owner", 
		sizeof("Owner")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUAction_struct, Owner),
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
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUAction_struct, State),
		NULL,
		NULL,
		-1,
	},
	{
		0x0F44464C, 
		"StartDate", 
		L"StartDate", 
		sizeof("StartDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUAction_struct, StartDate),
		NULL,
		NULL,
		-1,
	},
	{
		0xC7425D55, 
		"EndDate", 
		L"EndDate", 
		sizeof("EndDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUAction_struct, EndDate),
		NULL,
		NULL,
		-1,
	},
	{
		0x00012E74, 
		"ECL", 
		L"ECL", 
		sizeof("ECL")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUAction_struct, ECL),
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
		offsetof(__CWsWorkunits_WUAction_struct, Jobname),
		NULL,
		NULL,
		-1,
	},
	{
		0x002FCCC0, 
		"Test", 
		L"Test", 
		sizeof("Test")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUAction_struct, Test),
		NULL,
		NULL,
		-1,
	},
	{
		0x3C84F320, 
		"CurrentPage", 
		L"CurrentPage", 
		sizeof("CurrentPage")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUAction_struct, CurrentPage),
		NULL,
		NULL,
		-1,
	},
	{
		0xF6F9E158, 
		"PageSize", 
		L"PageSize", 
		sizeof("PageSize")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUAction_struct, PageSize),
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
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUAction_struct, Sortby),
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
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUAction_struct, Descending),
		NULL,
		NULL,
		-1,
	},
	{
		0x0591C76B, 
		"EclServer", 
		L"EclServer", 
		sizeof("EclServer")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUAction_struct, EclServer),
		NULL,
		NULL,
		-1,
	},
	{
		0xB3938D63, 
		"EventName", 
		L"EventName", 
		sizeof("EventName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUAction_struct, EventName),
		NULL,
		NULL,
		-1,
	},
	{
		0xF6F2E551, 
		"PageFrom", 
		L"PageFrom", 
		sizeof("PageFrom")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUAction_struct, PageFrom),
		NULL,
		NULL,
		-1,
	},
	{
		0x032F6A42, 
		"BlockTillFinishTimer", 
		L"BlockTillFinishTimer", 
		sizeof("BlockTillFinishTimer")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUAction_struct, BlockTillFinishTimer),
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
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUAction_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0xCDF828D0, 
		"ActionResults", 
		L"ActionResults", 
		sizeof("ActionResults")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUAction_struct, ActionResults),
		NULL,
		&__WUActionResult_map,
		20+1,
	},
	{
		0xCDF828D0,
		"__ActionResults_nSizeIs",
		L"__ActionResults_nSizeIs",
		sizeof("__ActionResults_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsWorkunits_WUAction_struct, __ActionResults_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsWorkunits_WUAction_map =
{
	0xE96BA4D9,
	"WUAction",
	L"WUActionResponse",
	sizeof("WUAction")-1,
	sizeof("WUActionResponse")-1,
	SOAPMAP_FUNC,
	__CWsWorkunits_WUAction_entries,
	sizeof(__CWsWorkunits_WUAction_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};


struct __CWsWorkunits_WUCreate_struct
{
	ArrayOfEspException Exceptions;
	ECLWorkunit Workunit;
};

extern __declspec(selectany) const _soapmapentry __CWsWorkunits_WUCreate_entries[] =
{

	{
		0x7B0DBD02, 
		"Exceptions", 
		L"Exceptions", 
		sizeof("Exceptions")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUCreate_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0xBC9E1003, 
		"Workunit", 
		L"Workunit", 
		sizeof("Workunit")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUCreate_struct, Workunit),
		NULL,
		&__ECLWorkunit_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsWorkunits_WUCreate_map =
{
	0xF92A608F,
	"WUCreate",
	L"WUCreateResponse",
	sizeof("WUCreate")-1,
	sizeof("WUCreateResponse")-1,
	SOAPMAP_FUNC,
	__CWsWorkunits_WUCreate_entries,
	sizeof(__CWsWorkunits_WUCreate_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};


struct __CWsWorkunits_WUWaitComplete_struct
{
	BSTR Wuid;
	int Wait;
	bool ReturnOnWait;
	ArrayOfEspException Exceptions;
	int StateID;
};

extern __declspec(selectany) const _soapmapentry __CWsWorkunits_WUWaitComplete_entries[] =
{

	{
		0x0031B499, 
		"Wuid", 
		L"Wuid", 
		sizeof("Wuid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUWaitComplete_struct, Wuid),
		NULL,
		NULL,
		-1,
	},
	{
		0x00315F95, 
		"Wait", 
		L"Wait", 
		sizeof("Wait")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUWaitComplete_struct, Wait),
		NULL,
		NULL,
		-1,
	},
	{
		0x66472BF2, 
		"ReturnOnWait", 
		L"ReturnOnWait", 
		sizeof("ReturnOnWait")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUWaitComplete_struct, ReturnOnWait),
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
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUWaitComplete_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0x0AD4B7AE, 
		"StateID", 
		L"StateID", 
		sizeof("StateID")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUWaitComplete_struct, StateID),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsWorkunits_WUWaitComplete_map =
{
	0x392AC450,
	"WUWaitComplete",
	L"WUWaitResponse",
	sizeof("WUWaitComplete")-1,
	sizeof("WUWaitResponse")-1,
	SOAPMAP_FUNC,
	__CWsWorkunits_WUWaitComplete_entries,
	sizeof(__CWsWorkunits_WUWaitComplete_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};


struct __CWsWorkunits_WUProtect_struct
{
	BSTR *Wuids;
	int __Wuids_nSizeIs;
	bool Protect;
	ArrayOfEspException Exceptions;
	WUActionResult *ActionResults;
	int __ActionResults_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CWsWorkunits_WUProtect_entries[] =
{

	{
		0x0668482C, 
		"Wuids", 
		L"Wuids", 
		sizeof("Wuids")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUProtect_struct, Wuids),
		NULL,
		NULL,
		0+1,
	},
	{
		0x0668482C,
		"__Wuids_nSizeIs",
		L"__Wuids_nSizeIs",
		sizeof("__Wuids_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsWorkunits_WUProtect_struct, __Wuids_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x20390AC1, 
		"Protect", 
		L"Protect", 
		sizeof("Protect")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUProtect_struct, Protect),
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
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUProtect_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0xCDF828D0, 
		"ActionResults", 
		L"ActionResults", 
		sizeof("ActionResults")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUProtect_struct, ActionResults),
		NULL,
		&__WUActionResult_map,
		4+1,
	},
	{
		0xCDF828D0,
		"__ActionResults_nSizeIs",
		L"__ActionResults_nSizeIs",
		sizeof("__ActionResults_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsWorkunits_WUProtect_struct, __ActionResults_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsWorkunits_WUProtect_map =
{
	0x0D607FFC,
	"WUProtect",
	L"WUProtectResponse",
	sizeof("WUProtect")-1,
	sizeof("WUProtectResponse")-1,
	SOAPMAP_FUNC,
	__CWsWorkunits_WUProtect_entries,
	sizeof(__CWsWorkunits_WUProtect_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};


struct __CWsWorkunits_WUCompileECL_struct
{
	BSTR ECL;
	BSTR ModuleName;
	BSTR AttributeName;
	BSTR Queue;
	BSTR Cluster;
	BSTR Snapshot;
	bool IncludeDependencies;
	bool IncludeComplexity;
	int TimeToWait;
	ArrayOfEspException Exceptions;
	BSTR Complexity;
	ECLException *Errors;
	int __Errors_nSizeIs;
	WUECLAttribute *Dependencies;
	int __Dependencies_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CWsWorkunits_WUCompileECL_entries[] =
{

	{
		0x00012E74, 
		"ECL", 
		L"ECL", 
		sizeof("ECL")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUCompileECL_struct, ECL),
		NULL,
		NULL,
		-1,
	},
	{
		0x3474D1E7, 
		"ModuleName", 
		L"ModuleName", 
		sizeof("ModuleName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUCompileECL_struct, ModuleName),
		NULL,
		NULL,
		-1,
	},
	{
		0x0CA09C15, 
		"AttributeName", 
		L"AttributeName", 
		sizeof("AttributeName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUCompileECL_struct, AttributeName),
		NULL,
		NULL,
		-1,
	},
	{
		0x05FBA645, 
		"Queue", 
		L"Queue", 
		sizeof("Queue")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUCompileECL_struct, Queue),
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
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUCompileECL_struct, Cluster),
		NULL,
		NULL,
		-1,
	},
	{
		0x974E1610, 
		"Snapshot", 
		L"Snapshot", 
		sizeof("Snapshot")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUCompileECL_struct, Snapshot),
		NULL,
		NULL,
		-1,
	},
	{
		0xA343DCEB, 
		"IncludeDependencies", 
		L"IncludeDependencies", 
		sizeof("IncludeDependencies")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUCompileECL_struct, IncludeDependencies),
		NULL,
		NULL,
		-1,
	},
	{
		0x1F526CB2, 
		"IncludeComplexity", 
		L"IncludeComplexity", 
		sizeof("IncludeComplexity")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUCompileECL_struct, IncludeComplexity),
		NULL,
		NULL,
		-1,
	},
	{
		0xDD3EE887, 
		"TimeToWait", 
		L"TimeToWait", 
		sizeof("TimeToWait")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUCompileECL_struct, TimeToWait),
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
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUCompileECL_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0xD98F9DAE, 
		"Complexity", 
		L"Complexity", 
		sizeof("Complexity")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUCompileECL_struct, Complexity),
		NULL,
		NULL,
		-1,
	},
	{
		0xA943475D, 
		"Errors", 
		L"Errors", 
		sizeof("Errors")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUCompileECL_struct, Errors),
		NULL,
		&__ECLException_map,
		11+1,
	},
	{
		0xA943475D,
		"__Errors_nSizeIs",
		L"__Errors_nSizeIs",
		sizeof("__Errors_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsWorkunits_WUCompileECL_struct, __Errors_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xE1913CE7, 
		"Dependencies", 
		L"Dependencies", 
		sizeof("Dependencies")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUCompileECL_struct, Dependencies),
		NULL,
		&__WUECLAttribute_map,
		13+1,
	},
	{
		0xE1913CE7,
		"__Dependencies_nSizeIs",
		L"__Dependencies_nSizeIs",
		sizeof("__Dependencies_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsWorkunits_WUCompileECL_struct, __Dependencies_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsWorkunits_WUCompileECL_map =
{
	0x875A8A38,
	"WUCompileECL",
	L"WUCompileECLResponse",
	sizeof("WUCompileECL")-1,
	sizeof("WUCompileECLResponse")-1,
	SOAPMAP_FUNC,
	__CWsWorkunits_WUCompileECL_entries,
	sizeof(__CWsWorkunits_WUCompileECL_struct),
	4,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};


struct __CWsWorkunits_WUSyntaxCheckECL_struct
{
	BSTR ECL;
	BSTR ModuleName;
	BSTR AttributeName;
	BSTR Queue;
	BSTR Cluster;
	BSTR Snapshot;
	int TimeToWait;
	DebugValue *DebugValues;
	int __DebugValues_nSizeIs;
	ArrayOfEspException Exceptions;
	ECLException *Errors;
	int __Errors_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CWsWorkunits_WUSyntaxCheckECL_entries[] =
{

	{
		0x00012E74, 
		"ECL", 
		L"ECL", 
		sizeof("ECL")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUSyntaxCheckECL_struct, ECL),
		NULL,
		NULL,
		-1,
	},
	{
		0x3474D1E7, 
		"ModuleName", 
		L"ModuleName", 
		sizeof("ModuleName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUSyntaxCheckECL_struct, ModuleName),
		NULL,
		NULL,
		-1,
	},
	{
		0x0CA09C15, 
		"AttributeName", 
		L"AttributeName", 
		sizeof("AttributeName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUSyntaxCheckECL_struct, AttributeName),
		NULL,
		NULL,
		-1,
	},
	{
		0x05FBA645, 
		"Queue", 
		L"Queue", 
		sizeof("Queue")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUSyntaxCheckECL_struct, Queue),
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
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUSyntaxCheckECL_struct, Cluster),
		NULL,
		NULL,
		-1,
	},
	{
		0x974E1610, 
		"Snapshot", 
		L"Snapshot", 
		sizeof("Snapshot")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUSyntaxCheckECL_struct, Snapshot),
		NULL,
		NULL,
		-1,
	},
	{
		0xDD3EE887, 
		"TimeToWait", 
		L"TimeToWait", 
		sizeof("TimeToWait")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUSyntaxCheckECL_struct, TimeToWait),
		NULL,
		NULL,
		-1,
	},
	{
		0x5EE78D37, 
		"DebugValues", 
		L"DebugValues", 
		sizeof("DebugValues")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUSyntaxCheckECL_struct, DebugValues),
		NULL,
		&__DebugValue_map,
		7+1,
	},
	{
		0x5EE78D37,
		"__DebugValues_nSizeIs",
		L"__DebugValues_nSizeIs",
		sizeof("__DebugValues_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsWorkunits_WUSyntaxCheckECL_struct, __DebugValues_nSizeIs),
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
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUSyntaxCheckECL_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0xA943475D, 
		"Errors", 
		L"Errors", 
		sizeof("Errors")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUSyntaxCheckECL_struct, Errors),
		NULL,
		&__ECLException_map,
		10+1,
	},
	{
		0xA943475D,
		"__Errors_nSizeIs",
		L"__Errors_nSizeIs",
		sizeof("__Errors_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsWorkunits_WUSyntaxCheckECL_struct, __Errors_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsWorkunits_WUSyntaxCheckECL_map =
{
	0x2035DCE0,
	"WUSyntaxCheckECL",
	L"WUSyntaxCheckResponse",
	sizeof("WUSyntaxCheckECL")-1,
	sizeof("WUSyntaxCheckResponse")-1,
	SOAPMAP_FUNC,
	__CWsWorkunits_WUSyntaxCheckECL_entries,
	sizeof(__CWsWorkunits_WUSyntaxCheckECL_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};


struct __CWsWorkunits_WUListLocalFileRequired_struct
{
	BSTR Wuid;
	ArrayOfEspException Exceptions;
	LogicalFileUpload *LocalFileUploads;
	int __LocalFileUploads_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CWsWorkunits_WUListLocalFileRequired_entries[] =
{

	{
		0x0031B499, 
		"Wuid", 
		L"Wuid", 
		sizeof("Wuid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUListLocalFileRequired_struct, Wuid),
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
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUListLocalFileRequired_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0x25A4AE03, 
		"LocalFileUploads", 
		L"LocalFileUploads", 
		sizeof("LocalFileUploads")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUListLocalFileRequired_struct, LocalFileUploads),
		NULL,
		&__LogicalFileUpload_map,
		2+1,
	},
	{
		0x25A4AE03,
		"__LocalFileUploads_nSizeIs",
		L"__LocalFileUploads_nSizeIs",
		sizeof("__LocalFileUploads_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsWorkunits_WUListLocalFileRequired_struct, __LocalFileUploads_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsWorkunits_WUListLocalFileRequired_map =
{
	0xE3D48A43,
	"WUListLocalFileRequired",
	L"WUListLocalFileRequiredResponse",
	sizeof("WUListLocalFileRequired")-1,
	sizeof("WUListLocalFileRequiredResponse")-1,
	SOAPMAP_FUNC,
	__CWsWorkunits_WUListLocalFileRequired_entries,
	sizeof(__CWsWorkunits_WUListLocalFileRequired_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};


struct __CWsWorkunits_WUShowScheduled_struct
{
	BSTR EclServer;
	BSTR EventName;
	BSTR PushEventName;
	BSTR PushEventText;
	ArrayOfEspException Exceptions;
	int EclServerSelected;
	BSTR Query;
	ServerInfo *EclServers;
	int __EclServers_nSizeIs;
	ScheduledWU *Workunits;
	int __Workunits_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CWsWorkunits_WUShowScheduled_entries[] =
{

	{
		0x0591C76B, 
		"EclServer", 
		L"EclServer", 
		sizeof("EclServer")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUShowScheduled_struct, EclServer),
		NULL,
		NULL,
		-1,
	},
	{
		0xB3938D63, 
		"EventName", 
		L"EventName", 
		sizeof("EventName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUShowScheduled_struct, EventName),
		NULL,
		NULL,
		-1,
	},
	{
		0x6D7094A3, 
		"PushEventName", 
		L"PushEventName", 
		sizeof("PushEventName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUShowScheduled_struct, PushEventName),
		NULL,
		NULL,
		-1,
	},
	{
		0x6D73F167, 
		"PushEventText", 
		L"PushEventText", 
		sizeof("PushEventText")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUShowScheduled_struct, PushEventText),
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
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUShowScheduled_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0x6DC712B4, 
		"EclServerSelected", 
		L"EclServerSelected", 
		sizeof("EclServerSelected")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUShowScheduled_struct, EclServerSelected),
		NULL,
		NULL,
		-1,
	},
	{
		0x05FBA5F6, 
		"Query", 
		L"Query", 
		sizeof("Query")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUShowScheduled_struct, Query),
		NULL,
		NULL,
		-1,
	},
	{
		0xB7CAB53E, 
		"EclServers", 
		L"EclServers", 
		sizeof("EclServers")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUShowScheduled_struct, EclServers),
		NULL,
		&__ServerInfo_map,
		7+1,
	},
	{
		0xB7CAB53E,
		"__EclServers_nSizeIs",
		L"__EclServers_nSizeIs",
		sizeof("__EclServers_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsWorkunits_WUShowScheduled_struct, __EclServers_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x506010D6, 
		"Workunits", 
		L"Workunits", 
		sizeof("Workunits")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUShowScheduled_struct, Workunits),
		NULL,
		&__ScheduledWU_map,
		9+1,
	},
	{
		0x506010D6,
		"__Workunits_nSizeIs",
		L"__Workunits_nSizeIs",
		sizeof("__Workunits_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsWorkunits_WUShowScheduled_struct, __Workunits_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsWorkunits_WUShowScheduled_map =
{
	0x968F60CD,
	"WUShowScheduled",
	L"WUShowScheduledResponse",
	sizeof("WUShowScheduled")-1,
	sizeof("WUShowScheduledResponse")-1,
	SOAPMAP_FUNC,
	__CWsWorkunits_WUShowScheduled_entries,
	sizeof(__CWsWorkunits_WUShowScheduled_struct),
	8,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};


struct __CWsWorkunits_WUClusterJobSummaryXLS_struct
{
	BSTR Cluster;
	BSTR StartDate;
	BSTR EndDate;
	bool ShowAll;
	BSTR BusinessStartTime;
	BSTR BusinessEndTime;
	ArrayOfEspException Exceptions;
	ATLSOAP_BLOB Result;
};

extern __declspec(selectany) const _soapmapentry __CWsWorkunits_WUClusterJobSummaryXLS_entries[] =
{

	{
		0x29F147E2, 
		"Cluster", 
		L"Cluster", 
		sizeof("Cluster")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUClusterJobSummaryXLS_struct, Cluster),
		NULL,
		NULL,
		-1,
	},
	{
		0x0F44464C, 
		"StartDate", 
		L"StartDate", 
		sizeof("StartDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUClusterJobSummaryXLS_struct, StartDate),
		NULL,
		NULL,
		-1,
	},
	{
		0xC7425D55, 
		"EndDate", 
		L"EndDate", 
		sizeof("EndDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUClusterJobSummaryXLS_struct, EndDate),
		NULL,
		NULL,
		-1,
	},
	{
		0xEFD537DA, 
		"ShowAll", 
		L"ShowAll", 
		sizeof("ShowAll")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUClusterJobSummaryXLS_struct, ShowAll),
		NULL,
		NULL,
		-1,
	},
	{
		0x4A703D09, 
		"BusinessStartTime", 
		L"BusinessStartTime", 
		sizeof("BusinessStartTime")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUClusterJobSummaryXLS_struct, BusinessStartTime),
		NULL,
		NULL,
		-1,
	},
	{
		0x0B0D0112, 
		"BusinessEndTime", 
		L"BusinessEndTime", 
		sizeof("BusinessEndTime")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUClusterJobSummaryXLS_struct, BusinessEndTime),
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
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUClusterJobSummaryXLS_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0xC6ABBE1F, 
		"Result", 
		L"Result", 
		sizeof("Result")-1, 
		SOAPTYPE_BASE64BINARY, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUClusterJobSummaryXLS_struct, Result),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsWorkunits_WUClusterJobSummaryXLS_map =
{
	0xE529DDFD,
	"WUClusterJobSummaryXLS",
	L"WUClusterJobSummaryXLSResponse",
	sizeof("WUClusterJobSummaryXLS")-1,
	sizeof("WUClusterJobSummaryXLSResponse")-1,
	SOAPMAP_FUNC,
	__CWsWorkunits_WUClusterJobSummaryXLS_entries,
	sizeof(__CWsWorkunits_WUClusterJobSummaryXLS_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};


struct __CWsWorkunits_WUInfo_struct
{
	BSTR Wuid;
	BSTR Type;
	bool IncludeExceptions;
	bool IncludeGraphs;
	bool IncludeSourceFiles;
	bool IncludeResults;
	bool IncludeVariables;
	bool IncludeTimers;
	bool IncludeDebugValues;
	bool IncludeApplicationValues;
	bool IncludeWorkflows;
	bool SuppressResultSchemas;
	BSTR ThorSlaveIP;
	ArrayOfEspException Exceptions;
	ECLWorkunit Workunit;
	int AutoRefresh;
	bool CanCompile;
};

extern __declspec(selectany) const _soapmapentry __CWsWorkunits_WUInfo_entries[] =
{

	{
		0x0031B499, 
		"Wuid", 
		L"Wuid", 
		sizeof("Wuid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUInfo_struct, Wuid),
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
		offsetof(__CWsWorkunits_WUInfo_struct, Type),
		NULL,
		NULL,
		-1,
	},
	{
		0xC0D08C06, 
		"IncludeExceptions", 
		L"IncludeExceptions", 
		sizeof("IncludeExceptions")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUInfo_struct, IncludeExceptions),
		NULL,
		NULL,
		-1,
	},
	{
		0x85303629, 
		"IncludeGraphs", 
		L"IncludeGraphs", 
		sizeof("IncludeGraphs")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUInfo_struct, IncludeGraphs),
		NULL,
		NULL,
		-1,
	},
	{
		0x237D78A8, 
		"IncludeSourceFiles", 
		L"IncludeSourceFiles", 
		sizeof("IncludeSourceFiles")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUInfo_struct, IncludeSourceFiles),
		NULL,
		NULL,
		-1,
	},
	{
		0x5CED0FF6, 
		"IncludeResults", 
		L"IncludeResults", 
		sizeof("IncludeResults")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUInfo_struct, IncludeResults),
		NULL,
		NULL,
		-1,
	},
	{
		0x039234FD, 
		"IncludeVariables", 
		L"IncludeVariables", 
		sizeof("IncludeVariables")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUInfo_struct, IncludeVariables),
		NULL,
		NULL,
		-1,
	},
	{
		0xA2E6D0D8, 
		"IncludeTimers", 
		L"IncludeTimers", 
		sizeof("IncludeTimers")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUInfo_struct, IncludeTimers),
		NULL,
		NULL,
		-1,
	},
	{
		0x5D043CBB, 
		"IncludeDebugValues", 
		L"IncludeDebugValues", 
		sizeof("IncludeDebugValues")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUInfo_struct, IncludeDebugValues),
		NULL,
		NULL,
		-1,
	},
	{
		0xE668E788, 
		"IncludeApplicationValues", 
		L"IncludeApplicationValues", 
		sizeof("IncludeApplicationValues")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUInfo_struct, IncludeApplicationValues),
		NULL,
		NULL,
		-1,
	},
	{
		0x68B2A0F2, 
		"IncludeWorkflows", 
		L"IncludeWorkflows", 
		sizeof("IncludeWorkflows")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUInfo_struct, IncludeWorkflows),
		NULL,
		NULL,
		-1,
	},
	{
		0xDF2E3DA8, 
		"SuppressResultSchemas", 
		L"SuppressResultSchemas", 
		sizeof("SuppressResultSchemas")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUInfo_struct, SuppressResultSchemas),
		NULL,
		NULL,
		-1,
	},
	{
		0x1628C5D1, 
		"ThorSlaveIP", 
		L"ThorSlaveIP", 
		sizeof("ThorSlaveIP")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUInfo_struct, ThorSlaveIP),
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
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUInfo_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0xBC9E1003, 
		"Workunit", 
		L"Workunit", 
		sizeof("Workunit")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUInfo_struct, Workunit),
		NULL,
		&__ECLWorkunit_map,
		-1,
	},
	{
		0x89835C68, 
		"AutoRefresh", 
		L"AutoRefresh", 
		sizeof("AutoRefresh")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUInfo_struct, AutoRefresh),
		NULL,
		NULL,
		-1,
	},
	{
		0xE692705B, 
		"CanCompile", 
		L"CanCompile", 
		sizeof("CanCompile")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUInfo_struct, CanCompile),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsWorkunits_WUInfo_map =
{
	0xFF02F4E7,
	"WUInfo",
	L"WUInfoResponse",
	sizeof("WUInfo")-1,
	sizeof("WUInfoResponse")-1,
	SOAPMAP_FUNC,
	__CWsWorkunits_WUInfo_entries,
	sizeof(__CWsWorkunits_WUInfo_struct),
	5,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};


struct __CWsWorkunits_WUInfoDetails_struct
{
	BSTR Wuid;
	BSTR Type;
	bool IncludeExceptions;
	bool IncludeGraphs;
	bool IncludeSourceFiles;
	bool IncludeResults;
	bool IncludeVariables;
	bool IncludeTimers;
	bool IncludeDebugValues;
	bool IncludeApplicationValues;
	bool IncludeWorkflows;
	bool SuppressResultSchemas;
	BSTR ThorSlaveIP;
	ArrayOfEspException Exceptions;
	ECLWorkunit Workunit;
	int AutoRefresh;
	bool CanCompile;
};

extern __declspec(selectany) const _soapmapentry __CWsWorkunits_WUInfoDetails_entries[] =
{

	{
		0x0031B499, 
		"Wuid", 
		L"Wuid", 
		sizeof("Wuid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUInfoDetails_struct, Wuid),
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
		offsetof(__CWsWorkunits_WUInfoDetails_struct, Type),
		NULL,
		NULL,
		-1,
	},
	{
		0xC0D08C06, 
		"IncludeExceptions", 
		L"IncludeExceptions", 
		sizeof("IncludeExceptions")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUInfoDetails_struct, IncludeExceptions),
		NULL,
		NULL,
		-1,
	},
	{
		0x85303629, 
		"IncludeGraphs", 
		L"IncludeGraphs", 
		sizeof("IncludeGraphs")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUInfoDetails_struct, IncludeGraphs),
		NULL,
		NULL,
		-1,
	},
	{
		0x237D78A8, 
		"IncludeSourceFiles", 
		L"IncludeSourceFiles", 
		sizeof("IncludeSourceFiles")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUInfoDetails_struct, IncludeSourceFiles),
		NULL,
		NULL,
		-1,
	},
	{
		0x5CED0FF6, 
		"IncludeResults", 
		L"IncludeResults", 
		sizeof("IncludeResults")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUInfoDetails_struct, IncludeResults),
		NULL,
		NULL,
		-1,
	},
	{
		0x039234FD, 
		"IncludeVariables", 
		L"IncludeVariables", 
		sizeof("IncludeVariables")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUInfoDetails_struct, IncludeVariables),
		NULL,
		NULL,
		-1,
	},
	{
		0xA2E6D0D8, 
		"IncludeTimers", 
		L"IncludeTimers", 
		sizeof("IncludeTimers")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUInfoDetails_struct, IncludeTimers),
		NULL,
		NULL,
		-1,
	},
	{
		0x5D043CBB, 
		"IncludeDebugValues", 
		L"IncludeDebugValues", 
		sizeof("IncludeDebugValues")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUInfoDetails_struct, IncludeDebugValues),
		NULL,
		NULL,
		-1,
	},
	{
		0xE668E788, 
		"IncludeApplicationValues", 
		L"IncludeApplicationValues", 
		sizeof("IncludeApplicationValues")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUInfoDetails_struct, IncludeApplicationValues),
		NULL,
		NULL,
		-1,
	},
	{
		0x68B2A0F2, 
		"IncludeWorkflows", 
		L"IncludeWorkflows", 
		sizeof("IncludeWorkflows")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUInfoDetails_struct, IncludeWorkflows),
		NULL,
		NULL,
		-1,
	},
	{
		0xDF2E3DA8, 
		"SuppressResultSchemas", 
		L"SuppressResultSchemas", 
		sizeof("SuppressResultSchemas")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUInfoDetails_struct, SuppressResultSchemas),
		NULL,
		NULL,
		-1,
	},
	{
		0x1628C5D1, 
		"ThorSlaveIP", 
		L"ThorSlaveIP", 
		sizeof("ThorSlaveIP")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUInfoDetails_struct, ThorSlaveIP),
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
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUInfoDetails_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0xBC9E1003, 
		"Workunit", 
		L"Workunit", 
		sizeof("Workunit")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUInfoDetails_struct, Workunit),
		NULL,
		&__ECLWorkunit_map,
		-1,
	},
	{
		0x89835C68, 
		"AutoRefresh", 
		L"AutoRefresh", 
		sizeof("AutoRefresh")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUInfoDetails_struct, AutoRefresh),
		NULL,
		NULL,
		-1,
	},
	{
		0xE692705B, 
		"CanCompile", 
		L"CanCompile", 
		sizeof("CanCompile")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUInfoDetails_struct, CanCompile),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsWorkunits_WUInfoDetails_map =
{
	0xFF02F4E7,
	"WUInfoDetails",
	L"WUInfoResponse",
	sizeof("WUInfoDetails")-1,
	sizeof("WUInfoResponse")-1,
	SOAPMAP_FUNC,
	__CWsWorkunits_WUInfoDetails_entries,
	sizeof(__CWsWorkunits_WUInfoDetails_struct),
	5,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};


struct __CWsWorkunits_WUGetDependancyTrees_struct
{
	BSTR Cluster;
	BSTR Queue;
	BSTR Snapshot;
	BSTR Items;
	BSTR TimeoutMilliSec;
	ArrayOfEspException Exceptions;
	ECLException *Errors;
	int __Errors_nSizeIs;
	ATLSOAP_BLOB DependancyTrees;
};

extern __declspec(selectany) const _soapmapentry __CWsWorkunits_WUGetDependancyTrees_entries[] =
{

	{
		0x29F147E2, 
		"Cluster", 
		L"Cluster", 
		sizeof("Cluster")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUGetDependancyTrees_struct, Cluster),
		NULL,
		NULL,
		-1,
	},
	{
		0x05FBA645, 
		"Queue", 
		L"Queue", 
		sizeof("Queue")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUGetDependancyTrees_struct, Queue),
		NULL,
		NULL,
		-1,
	},
	{
		0x974E1610, 
		"Snapshot", 
		L"Snapshot", 
		sizeof("Snapshot")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUGetDependancyTrees_struct, Snapshot),
		NULL,
		NULL,
		-1,
	},
	{
		0x056A54E2, 
		"Items", 
		L"Items", 
		sizeof("Items")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUGetDependancyTrees_struct, Items),
		NULL,
		NULL,
		-1,
	},
	{
		0x16E718B9, 
		"TimeoutMilliSec", 
		L"TimeoutMilliSec", 
		sizeof("TimeoutMilliSec")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUGetDependancyTrees_struct, TimeoutMilliSec),
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
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUGetDependancyTrees_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0xA943475D, 
		"Errors", 
		L"Errors", 
		sizeof("Errors")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUGetDependancyTrees_struct, Errors),
		NULL,
		&__ECLException_map,
		6+1,
	},
	{
		0xA943475D,
		"__Errors_nSizeIs",
		L"__Errors_nSizeIs",
		sizeof("__Errors_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsWorkunits_WUGetDependancyTrees_struct, __Errors_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x3784BBBE, 
		"DependancyTrees", 
		L"DependancyTrees", 
		sizeof("DependancyTrees")-1, 
		SOAPTYPE_BASE64BINARY, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUGetDependancyTrees_struct, DependancyTrees),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsWorkunits_WUGetDependancyTrees_map =
{
	0x8AE149F9,
	"WUGetDependancyTrees",
	L"WUGetDependancyTreesResponse",
	sizeof("WUGetDependancyTrees")-1,
	sizeof("WUGetDependancyTreesResponse")-1,
	SOAPMAP_FUNC,
	__CWsWorkunits_WUGetDependancyTrees_entries,
	sizeof(__CWsWorkunits_WUGetDependancyTrees_struct),
	3,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};


struct __CWsWorkunits_WUAbort_struct
{
	BSTR *Wuids;
	int __Wuids_nSizeIs;
	int BlockTillFinishTimer;
	ArrayOfEspException Exceptions;
	WUActionResult *ActionResults;
	int __ActionResults_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CWsWorkunits_WUAbort_entries[] =
{

	{
		0x0668482C, 
		"Wuids", 
		L"Wuids", 
		sizeof("Wuids")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUAbort_struct, Wuids),
		NULL,
		NULL,
		0+1,
	},
	{
		0x0668482C,
		"__Wuids_nSizeIs",
		L"__Wuids_nSizeIs",
		sizeof("__Wuids_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsWorkunits_WUAbort_struct, __Wuids_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x032F6A42, 
		"BlockTillFinishTimer", 
		L"BlockTillFinishTimer", 
		sizeof("BlockTillFinishTimer")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUAbort_struct, BlockTillFinishTimer),
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
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUAbort_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0xCDF828D0, 
		"ActionResults", 
		L"ActionResults", 
		sizeof("ActionResults")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUAbort_struct, ActionResults),
		NULL,
		&__WUActionResult_map,
		4+1,
	},
	{
		0xCDF828D0,
		"__ActionResults_nSizeIs",
		L"__ActionResults_nSizeIs",
		sizeof("__ActionResults_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsWorkunits_WUAbort_struct, __ActionResults_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsWorkunits_WUAbort_map =
{
	0x9669E673,
	"WUAbort",
	L"WUAbortResponse",
	sizeof("WUAbort")-1,
	sizeof("WUAbortResponse")-1,
	SOAPMAP_FUNC,
	__CWsWorkunits_WUAbort_entries,
	sizeof(__CWsWorkunits_WUAbort_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};


struct __CWsWorkunits_WUCDebug_struct
{
	BSTR Wuid;
	BSTR Command;
	ArrayOfEspException Exceptions;
	BSTR Result;
};

extern __declspec(selectany) const _soapmapentry __CWsWorkunits_WUCDebug_entries[] =
{

	{
		0x0031B499, 
		"Wuid", 
		L"Wuid", 
		sizeof("Wuid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUCDebug_struct, Wuid),
		NULL,
		NULL,
		-1,
	},
	{
		0x305C63BF, 
		"Command", 
		L"Command", 
		sizeof("Command")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUCDebug_struct, Command),
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
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUCDebug_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0xC6ABBE1F, 
		"Result", 
		L"Result", 
		sizeof("Result")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUCDebug_struct, Result),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsWorkunits_WUCDebug_map =
{
	0x7379DD22,
	"WUCDebug",
	L"WUDebugResponse",
	sizeof("WUCDebug")-1,
	sizeof("WUDebugResponse")-1,
	SOAPMAP_FUNC,
	__CWsWorkunits_WUCDebug_entries,
	sizeof(__CWsWorkunits_WUCDebug_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};


struct __CWsWorkunits_GVCAjaxGraph_struct
{
	BSTR Name;
	BSTR GraphName;
	int SubGraphId;
	bool SubGraphOnly;
	BSTR GraphType;
};

extern __declspec(selectany) const _soapmapentry __CWsWorkunits_GVCAjaxGraph_entries[] =
{

	{
		0x002C70A1, 
		"Name", 
		L"Name", 
		sizeof("Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_GVCAjaxGraph_struct, Name),
		NULL,
		NULL,
		-1,
	},
	{
		0xBB6D5413, 
		"GraphName", 
		L"GraphName", 
		sizeof("GraphName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_GVCAjaxGraph_struct, GraphName),
		NULL,
		NULL,
		-1,
	},
	{
		0x67ED9A09, 
		"SubGraphId", 
		L"SubGraphId", 
		sizeof("SubGraphId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_GVCAjaxGraph_struct, SubGraphId),
		NULL,
		NULL,
		-1,
	},
	{
		0x19BFC37E, 
		"SubGraphOnly", 
		L"SubGraphOnly", 
		sizeof("SubGraphOnly")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_GVCAjaxGraph_struct, SubGraphOnly),
		NULL,
		NULL,
		-1,
	},
	{
		0xBB7104D4, 
		"GraphType", 
		L"GraphType", 
		sizeof("GraphType")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_GVCAjaxGraph_struct, GraphType),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsWorkunits_GVCAjaxGraph_map =
{
	0xE2CAFF05,
	"GVCAjaxGraph",
	L"GVCAjaxGraphResponse",
	sizeof("GVCAjaxGraph")-1,
	sizeof("GVCAjaxGraphResponse")-1,
	SOAPMAP_FUNC,
	__CWsWorkunits_GVCAjaxGraph_entries,
	sizeof(__CWsWorkunits_GVCAjaxGraph_struct),
	5,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};


struct __CWsWorkunits_WUJobList_struct
{
	BSTR Cluster;
	BSTR StartDate;
	BSTR EndDate;
	bool ShowAll;
	int BusinessStartHour;
	int BusinessEndHour;
	ArrayOfEspException Exceptions;
	ECLJob *Jobs;
	int __Jobs_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CWsWorkunits_WUJobList_entries[] =
{

	{
		0x29F147E2, 
		"Cluster", 
		L"Cluster", 
		sizeof("Cluster")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUJobList_struct, Cluster),
		NULL,
		NULL,
		-1,
	},
	{
		0x0F44464C, 
		"StartDate", 
		L"StartDate", 
		sizeof("StartDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUJobList_struct, StartDate),
		NULL,
		NULL,
		-1,
	},
	{
		0xC7425D55, 
		"EndDate", 
		L"EndDate", 
		sizeof("EndDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUJobList_struct, EndDate),
		NULL,
		NULL,
		-1,
	},
	{
		0xEFD537DA, 
		"ShowAll", 
		L"ShowAll", 
		sizeof("ShowAll")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUJobList_struct, ShowAll),
		NULL,
		NULL,
		-1,
	},
	{
		0x4A69C318, 
		"BusinessStartHour", 
		L"BusinessStartHour", 
		sizeof("BusinessStartHour")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUJobList_struct, BusinessStartHour),
		NULL,
		NULL,
		-1,
	},
	{
		0x0B068721, 
		"BusinessEndHour", 
		L"BusinessEndHour", 
		sizeof("BusinessEndHour")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUJobList_struct, BusinessEndHour),
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
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUJobList_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0x002A794E, 
		"Jobs", 
		L"Jobs", 
		sizeof("Jobs")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUJobList_struct, Jobs),
		NULL,
		&__ECLJob_map,
		7+1,
	},
	{
		0x002A794E,
		"__Jobs_nSizeIs",
		L"__Jobs_nSizeIs",
		sizeof("__Jobs_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsWorkunits_WUJobList_struct, __Jobs_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsWorkunits_WUJobList_map =
{
	0x303514F2,
	"WUJobList",
	L"WUJobListResponse",
	sizeof("WUJobList")-1,
	sizeof("WUJobListResponse")-1,
	SOAPMAP_FUNC,
	__CWsWorkunits_WUJobList_entries,
	sizeof(__CWsWorkunits_WUJobList_struct),
	4,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};


struct __CWsWorkunits_WUGetGraph_struct
{
	BSTR Wuid;
	BSTR GraphName;
	BSTR SubGraphId;
	ArrayOfEspException Exceptions;
	ECLGraphEx *Graphs;
	int __Graphs_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CWsWorkunits_WUGetGraph_entries[] =
{

	{
		0x0031B499, 
		"Wuid", 
		L"Wuid", 
		sizeof("Wuid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUGetGraph_struct, Wuid),
		NULL,
		NULL,
		-1,
	},
	{
		0xBB6D5413, 
		"GraphName", 
		L"GraphName", 
		sizeof("GraphName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUGetGraph_struct, GraphName),
		NULL,
		NULL,
		-1,
	},
	{
		0x67ED9A09, 
		"SubGraphId", 
		L"SubGraphId", 
		sizeof("SubGraphId")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUGetGraph_struct, SubGraphId),
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
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUGetGraph_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0xADE44925, 
		"Graphs", 
		L"Graphs", 
		sizeof("Graphs")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUGetGraph_struct, Graphs),
		NULL,
		&__ECLGraphEx_map,
		4+1,
	},
	{
		0xADE44925,
		"__Graphs_nSizeIs",
		L"__Graphs_nSizeIs",
		sizeof("__Graphs_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsWorkunits_WUGetGraph_struct, __Graphs_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsWorkunits_WUGetGraph_map =
{
	0xE227BDAD,
	"WUGetGraph",
	L"WUGetGraphResponse",
	sizeof("WUGetGraph")-1,
	sizeof("WUGetGraphResponse")-1,
	SOAPMAP_FUNC,
	__CWsWorkunits_WUGetGraph_entries,
	sizeof(__CWsWorkunits_WUGetGraph_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};


struct __CWsWorkunits_WUResultSummary_struct
{
	BSTR Wuid;
	int Sequence;
	ArrayOfEspException Exceptions;
	int Format;
	ECLResult Result;
};

extern __declspec(selectany) const _soapmapentry __CWsWorkunits_WUResultSummary_entries[] =
{

	{
		0x0031B499, 
		"Wuid", 
		L"Wuid", 
		sizeof("Wuid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUResultSummary_struct, Wuid),
		NULL,
		NULL,
		-1,
	},
	{
		0x08276279, 
		"Sequence", 
		L"Sequence", 
		sizeof("Sequence")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUResultSummary_struct, Sequence),
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
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUResultSummary_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0xAB621BC9, 
		"Format", 
		L"Format", 
		sizeof("Format")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUResultSummary_struct, Format),
		NULL,
		NULL,
		-1,
	},
	{
		0xC6ABBE1F, 
		"Result", 
		L"Result", 
		sizeof("Result")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUResultSummary_struct, Result),
		NULL,
		&__ECLResult_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsWorkunits_WUResultSummary_map =
{
	0x88BFA3A8,
	"WUResultSummary",
	L"WUResultSummaryResponse",
	sizeof("WUResultSummary")-1,
	sizeof("WUResultSummaryResponse")-1,
	SOAPMAP_FUNC,
	__CWsWorkunits_WUResultSummary_entries,
	sizeof(__CWsWorkunits_WUResultSummary_struct),
	5,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};


struct __CWsWorkunits_WUGraphTiming_struct
{
	BSTR Wuid;
	ArrayOfEspException Exceptions;
	ECLWorkunit Workunit;
};

extern __declspec(selectany) const _soapmapentry __CWsWorkunits_WUGraphTiming_entries[] =
{

	{
		0x0031B499, 
		"Wuid", 
		L"Wuid", 
		sizeof("Wuid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUGraphTiming_struct, Wuid),
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
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUGraphTiming_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0xBC9E1003, 
		"Workunit", 
		L"Workunit", 
		sizeof("Workunit")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUGraphTiming_struct, Workunit),
		NULL,
		&__ECLWorkunit_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsWorkunits_WUGraphTiming_map =
{
	0xFC368075,
	"WUGraphTiming",
	L"WUGraphTimingResponse",
	sizeof("WUGraphTiming")-1,
	sizeof("WUGraphTimingResponse")-1,
	SOAPMAP_FUNC,
	__CWsWorkunits_WUGraphTiming_entries,
	sizeof(__CWsWorkunits_WUGraphTiming_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};


struct __CWsWorkunits_WUClusterJobQueueXLS_struct
{
	BSTR Cluster;
	BSTR StartDate;
	BSTR EndDate;
	BSTR ShowType;
	ArrayOfEspException Exceptions;
	ATLSOAP_BLOB Result;
};

extern __declspec(selectany) const _soapmapentry __CWsWorkunits_WUClusterJobQueueXLS_entries[] =
{

	{
		0x29F147E2, 
		"Cluster", 
		L"Cluster", 
		sizeof("Cluster")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUClusterJobQueueXLS_struct, Cluster),
		NULL,
		NULL,
		-1,
	},
	{
		0x0F44464C, 
		"StartDate", 
		L"StartDate", 
		sizeof("StartDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUClusterJobQueueXLS_struct, StartDate),
		NULL,
		NULL,
		-1,
	},
	{
		0xC7425D55, 
		"EndDate", 
		L"EndDate", 
		sizeof("EndDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUClusterJobQueueXLS_struct, EndDate),
		NULL,
		NULL,
		-1,
	},
	{
		0xEA86D683, 
		"ShowType", 
		L"ShowType", 
		sizeof("ShowType")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUClusterJobQueueXLS_struct, ShowType),
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
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUClusterJobQueueXLS_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0xC6ABBE1F, 
		"Result", 
		L"Result", 
		sizeof("Result")-1, 
		SOAPTYPE_BASE64BINARY, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUClusterJobQueueXLS_struct, Result),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsWorkunits_WUClusterJobQueueXLS_map =
{
	0x19F53574,
	"WUClusterJobQueueXLS",
	L"WUClusterJobQueueXLSResponse",
	sizeof("WUClusterJobQueueXLS")-1,
	sizeof("WUClusterJobQueueXLSResponse")-1,
	SOAPMAP_FUNC,
	__CWsWorkunits_WUClusterJobQueueXLS_entries,
	sizeof(__CWsWorkunits_WUClusterJobQueueXLS_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};


struct __CWsWorkunits_WUResult_struct
{
	BSTR Wuid;
	int Sequence;
	BSTR ResultName;
	BSTR LogicalName;
	BSTR Cluster;
	__int64 Start;
	int Count;
	ArrayOfEspException Exceptions;
	BSTR Name;
	int Requested;
	__int64 Total;
	BSTR Result;
};

extern __declspec(selectany) const _soapmapentry __CWsWorkunits_WUResult_entries[] =
{

	{
		0x0031B499, 
		"Wuid", 
		L"Wuid", 
		sizeof("Wuid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUResult_struct, Wuid),
		NULL,
		NULL,
		-1,
	},
	{
		0x08276279, 
		"Sequence", 
		L"Sequence", 
		sizeof("Sequence")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUResult_struct, Sequence),
		NULL,
		NULL,
		-1,
	},
	{
		0x94B82640, 
		"ResultName", 
		L"ResultName", 
		sizeof("ResultName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUResult_struct, ResultName),
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
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUResult_struct, LogicalName),
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
		offsetof(__CWsWorkunits_WUResult_struct, Cluster),
		NULL,
		NULL,
		-1,
	},
	{
		0x061F398E, 
		"Start", 
		L"Start", 
		sizeof("Start")-1, 
		SOAPTYPE_LONG, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUResult_struct, Start),
		NULL,
		NULL,
		-1,
	},
	{
		0x04FB4829, 
		"Count", 
		L"Count", 
		sizeof("Count")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUResult_struct, Count),
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
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUResult_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0x002C70A1, 
		"Name", 
		L"Name", 
		sizeof("Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUResult_struct, Name),
		NULL,
		NULL,
		-1,
	},
	{
		0x989A4732, 
		"Requested", 
		L"Requested", 
		sizeof("Requested")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUResult_struct, Requested),
		NULL,
		NULL,
		-1,
	},
	{
		0x062EE2C4, 
		"Total", 
		L"Total", 
		sizeof("Total")-1, 
		SOAPTYPE_LONG, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUResult_struct, Total),
		NULL,
		NULL,
		-1,
	},
	{
		0xC6ABBE1F, 
		"Result", 
		L"Result", 
		sizeof("Result")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUResult_struct, Result),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsWorkunits_WUResult_map =
{
	0x359D9FDA,
	"WUResult",
	L"WUResultResponse",
	sizeof("WUResult")-1,
	sizeof("WUResultResponse")-1,
	SOAPMAP_FUNC,
	__CWsWorkunits_WUResult_entries,
	sizeof(__CWsWorkunits_WUResult_struct),
	11,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};


struct __CWsWorkunits_WUWaitCompiled_struct
{
	BSTR Wuid;
	int Wait;
	bool ReturnOnWait;
	ArrayOfEspException Exceptions;
	int StateID;
};

extern __declspec(selectany) const _soapmapentry __CWsWorkunits_WUWaitCompiled_entries[] =
{

	{
		0x0031B499, 
		"Wuid", 
		L"Wuid", 
		sizeof("Wuid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUWaitCompiled_struct, Wuid),
		NULL,
		NULL,
		-1,
	},
	{
		0x00315F95, 
		"Wait", 
		L"Wait", 
		sizeof("Wait")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUWaitCompiled_struct, Wait),
		NULL,
		NULL,
		-1,
	},
	{
		0x66472BF2, 
		"ReturnOnWait", 
		L"ReturnOnWait", 
		sizeof("ReturnOnWait")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUWaitCompiled_struct, ReturnOnWait),
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
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUWaitCompiled_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0x0AD4B7AE, 
		"StateID", 
		L"StateID", 
		sizeof("StateID")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUWaitCompiled_struct, StateID),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsWorkunits_WUWaitCompiled_map =
{
	0x392AC450,
	"WUWaitCompiled",
	L"WUWaitResponse",
	sizeof("WUWaitCompiled")-1,
	sizeof("WUWaitResponse")-1,
	SOAPMAP_FUNC,
	__CWsWorkunits_WUWaitCompiled_entries,
	sizeof(__CWsWorkunits_WUWaitCompiled_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};


struct __CWsWorkunits_WUProcessGraph_struct
{
	BSTR FileName;
	BSTR PopupId;
	bool XmlGraph;
	bool Stats;
	ArrayOfEspException Exceptions;
	ATLSOAP_BLOB theGraph;
};

extern __declspec(selectany) const _soapmapentry __CWsWorkunits_WUProcessGraph_entries[] =
{

	{
		0x2FE06021, 
		"FileName", 
		L"FileName", 
		sizeof("FileName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUProcessGraph_struct, FileName),
		NULL,
		NULL,
		-1,
	},
	{
		0x194C6121, 
		"PopupId", 
		L"PopupId", 
		sizeof("PopupId")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUProcessGraph_struct, PopupId),
		NULL,
		NULL,
		-1,
	},
	{
		0xFE5F29E3, 
		"XmlGraph", 
		L"XmlGraph", 
		sizeof("XmlGraph")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUProcessGraph_struct, XmlGraph),
		NULL,
		NULL,
		-1,
	},
	{
		0x061F39CF, 
		"Stats", 
		L"Stats", 
		sizeof("Stats")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUProcessGraph_struct, Stats),
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
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUProcessGraph_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0x445B2653, 
		"theGraph", 
		L"theGraph", 
		sizeof("theGraph")-1, 
		SOAPTYPE_BASE64BINARY, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUProcessGraph_struct, theGraph),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsWorkunits_WUProcessGraph_map =
{
	0xCC6414EC,
	"WUProcessGraph",
	L"WUProcessGraphResponse",
	sizeof("WUProcessGraph")-1,
	sizeof("WUProcessGraphResponse")-1,
	SOAPMAP_FUNC,
	__CWsWorkunits_WUProcessGraph_entries,
	sizeof(__CWsWorkunits_WUProcessGraph_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};


struct __CWsWorkunits_WUResubmit_struct
{
	BSTR *Wuids;
	int __Wuids_nSizeIs;
	bool Recompile;
	int BlockTillFinishTimer;
	ArrayOfEspException Exceptions;
};

extern __declspec(selectany) const _soapmapentry __CWsWorkunits_WUResubmit_entries[] =
{

	{
		0x0668482C, 
		"Wuids", 
		L"Wuids", 
		sizeof("Wuids")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUResubmit_struct, Wuids),
		NULL,
		NULL,
		0+1,
	},
	{
		0x0668482C,
		"__Wuids_nSizeIs",
		L"__Wuids_nSizeIs",
		sizeof("__Wuids_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsWorkunits_WUResubmit_struct, __Wuids_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x557AF1E0, 
		"Recompile", 
		L"Recompile", 
		sizeof("Recompile")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUResubmit_struct, Recompile),
		NULL,
		NULL,
		-1,
	},
	{
		0x032F6A42, 
		"BlockTillFinishTimer", 
		L"BlockTillFinishTimer", 
		sizeof("BlockTillFinishTimer")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUResubmit_struct, BlockTillFinishTimer),
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
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUResubmit_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsWorkunits_WUResubmit_map =
{
	0xFAF1D706,
	"WUResubmit",
	L"WUResubmitResponse",
	sizeof("WUResubmit")-1,
	sizeof("WUResubmitResponse")-1,
	SOAPMAP_FUNC,
	__CWsWorkunits_WUResubmit_entries,
	sizeof(__CWsWorkunits_WUResubmit_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};


struct __CWsWorkunits_WUClusterJobQueueLOG_struct
{
	BSTR Cluster;
	BSTR StartDate;
	BSTR EndDate;
	ArrayOfEspException Exceptions;
	ATLSOAP_BLOB thefile;
};

extern __declspec(selectany) const _soapmapentry __CWsWorkunits_WUClusterJobQueueLOG_entries[] =
{

	{
		0x29F147E2, 
		"Cluster", 
		L"Cluster", 
		sizeof("Cluster")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUClusterJobQueueLOG_struct, Cluster),
		NULL,
		NULL,
		-1,
	},
	{
		0x0F44464C, 
		"StartDate", 
		L"StartDate", 
		sizeof("StartDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUClusterJobQueueLOG_struct, StartDate),
		NULL,
		NULL,
		-1,
	},
	{
		0xC7425D55, 
		"EndDate", 
		L"EndDate", 
		sizeof("EndDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUClusterJobQueueLOG_struct, EndDate),
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
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUClusterJobQueueLOG_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0xDB596F61, 
		"thefile", 
		L"thefile", 
		sizeof("thefile")-1, 
		SOAPTYPE_BASE64BINARY, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUClusterJobQueueLOG_struct, thefile),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsWorkunits_WUClusterJobQueueLOG_map =
{
	0x6FE71DBF,
	"WUClusterJobQueueLOG",
	L"WUClusterJobQueueLOGResponse",
	sizeof("WUClusterJobQueueLOG")-1,
	sizeof("WUClusterJobQueueLOGResponse")-1,
	SOAPMAP_FUNC,
	__CWsWorkunits_WUClusterJobQueueLOG_entries,
	sizeof(__CWsWorkunits_WUClusterJobQueueLOG_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};


struct __CWsWorkunits_WUGraphInfo_struct
{
	BSTR Wuid;
	BSTR Name;
	BSTR GID;
	int BatchWU;
	ArrayOfEspException Exceptions;
	bool Running;
};

extern __declspec(selectany) const _soapmapentry __CWsWorkunits_WUGraphInfo_entries[] =
{

	{
		0x0031B499, 
		"Wuid", 
		L"Wuid", 
		sizeof("Wuid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUGraphInfo_struct, Wuid),
		NULL,
		NULL,
		-1,
	},
	{
		0x002C70A1, 
		"Name", 
		L"Name", 
		sizeof("Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUGraphInfo_struct, Name),
		NULL,
		NULL,
		-1,
	},
	{
		0x000137B4, 
		"GID", 
		L"GID", 
		sizeof("GID")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUGraphInfo_struct, GID),
		NULL,
		NULL,
		-1,
	},
	{
		0xC33338AE, 
		"BatchWU", 
		L"BatchWU", 
		sizeof("BatchWU")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUGraphInfo_struct, BatchWU),
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
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUGraphInfo_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0xC117ADC1, 
		"Running", 
		L"Running", 
		sizeof("Running")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUGraphInfo_struct, Running),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsWorkunits_WUGraphInfo_map =
{
	0xDC1181D9,
	"WUGraphInfo",
	L"WUGraphInfoResponse",
	sizeof("WUGraphInfo")-1,
	sizeof("WUGraphInfoResponse")-1,
	SOAPMAP_FUNC,
	__CWsWorkunits_WUGraphInfo_entries,
	sizeof(__CWsWorkunits_WUGraphInfo_struct),
	6,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};


struct __CWsWorkunits_WUGVCGraphInfo_struct
{
	BSTR Wuid;
	BSTR Name;
	BSTR GID;
	int BatchWU;
	int SubgraphId;
	ArrayOfEspException Exceptions;
	bool Running;
	BSTR TheGraph;
};

extern __declspec(selectany) const _soapmapentry __CWsWorkunits_WUGVCGraphInfo_entries[] =
{

	{
		0x0031B499, 
		"Wuid", 
		L"Wuid", 
		sizeof("Wuid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUGVCGraphInfo_struct, Wuid),
		NULL,
		NULL,
		-1,
	},
	{
		0x002C70A1, 
		"Name", 
		L"Name", 
		sizeof("Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUGVCGraphInfo_struct, Name),
		NULL,
		NULL,
		-1,
	},
	{
		0x000137B4, 
		"GID", 
		L"GID", 
		sizeof("GID")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUGVCGraphInfo_struct, GID),
		NULL,
		NULL,
		-1,
	},
	{
		0xC33338AE, 
		"BatchWU", 
		L"BatchWU", 
		sizeof("BatchWU")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUGVCGraphInfo_struct, BatchWU),
		NULL,
		NULL,
		-1,
	},
	{
		0x07353229, 
		"SubgraphId", 
		L"SubgraphId", 
		sizeof("SubgraphId")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUGVCGraphInfo_struct, SubgraphId),
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
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUGVCGraphInfo_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0xC117ADC1, 
		"Running", 
		L"Running", 
		sizeof("Running")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUGVCGraphInfo_struct, Running),
		NULL,
		NULL,
		-1,
	},
	{
		0xBC208A33, 
		"TheGraph", 
		L"TheGraph", 
		sizeof("TheGraph")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUGVCGraphInfo_struct, TheGraph),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsWorkunits_WUGVCGraphInfo_map =
{
	0xEE631BB9,
	"WUGVCGraphInfo",
	L"WUGVCGraphInfoResponse",
	sizeof("WUGVCGraphInfo")-1,
	sizeof("WUGVCGraphInfoResponse")-1,
	SOAPMAP_FUNC,
	__CWsWorkunits_WUGVCGraphInfo_entries,
	sizeof(__CWsWorkunits_WUGVCGraphInfo_struct),
	7,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};


struct __CWsWorkunits_WUResultBin_struct
{
	BSTR LogicalName;
	BSTR Wuid;
	BSTR ResultName;
	int Sequence;
	BSTR Format;
	__int64 Start;
	int Count;
	ArrayOfEspException Exceptions;
	BSTR Name;
	int Requested;
	__int64 Total;
	ATLSOAP_BLOB Result;
};

extern __declspec(selectany) const _soapmapentry __CWsWorkunits_WUResultBin_entries[] =
{

	{
		0xCC052EFC, 
		"LogicalName", 
		L"LogicalName", 
		sizeof("LogicalName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUResultBin_struct, LogicalName),
		NULL,
		NULL,
		-1,
	},
	{
		0x0031B499, 
		"Wuid", 
		L"Wuid", 
		sizeof("Wuid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUResultBin_struct, Wuid),
		NULL,
		NULL,
		-1,
	},
	{
		0x94B82640, 
		"ResultName", 
		L"ResultName", 
		sizeof("ResultName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUResultBin_struct, ResultName),
		NULL,
		NULL,
		-1,
	},
	{
		0x08276279, 
		"Sequence", 
		L"Sequence", 
		sizeof("Sequence")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUResultBin_struct, Sequence),
		NULL,
		NULL,
		-1,
	},
	{
		0xAB621BC9, 
		"Format", 
		L"Format", 
		sizeof("Format")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUResultBin_struct, Format),
		NULL,
		NULL,
		-1,
	},
	{
		0x061F398E, 
		"Start", 
		L"Start", 
		sizeof("Start")-1, 
		SOAPTYPE_LONG, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUResultBin_struct, Start),
		NULL,
		NULL,
		-1,
	},
	{
		0x04FB4829, 
		"Count", 
		L"Count", 
		sizeof("Count")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUResultBin_struct, Count),
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
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUResultBin_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0x002C70A1, 
		"Name", 
		L"Name", 
		sizeof("Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUResultBin_struct, Name),
		NULL,
		NULL,
		-1,
	},
	{
		0x989A4732, 
		"Requested", 
		L"Requested", 
		sizeof("Requested")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUResultBin_struct, Requested),
		NULL,
		NULL,
		-1,
	},
	{
		0x062EE2C4, 
		"Total", 
		L"Total", 
		sizeof("Total")-1, 
		SOAPTYPE_LONG, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUResultBin_struct, Total),
		NULL,
		NULL,
		-1,
	},
	{
		0xC6ABBE1F, 
		"Result", 
		L"Result", 
		sizeof("Result")-1, 
		SOAPTYPE_BASE64BINARY, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUResultBin_struct, Result),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsWorkunits_WUResultBin_map =
{
	0x521313B3,
	"WUResultBin",
	L"WUResultBinResponse",
	sizeof("WUResultBin")-1,
	sizeof("WUResultBinResponse")-1,
	SOAPMAP_FUNC,
	__CWsWorkunits_WUResultBin_entries,
	sizeof(__CWsWorkunits_WUResultBin_struct),
	10,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};


struct __CWsWorkunits_WUClusterJobXLS_struct
{
	BSTR Cluster;
	BSTR StartDate;
	BSTR EndDate;
	bool ShowAll;
	BSTR BusinessStartTime;
	BSTR BusinessEndTime;
	ArrayOfEspException Exceptions;
	ATLSOAP_BLOB Result;
};

extern __declspec(selectany) const _soapmapentry __CWsWorkunits_WUClusterJobXLS_entries[] =
{

	{
		0x29F147E2, 
		"Cluster", 
		L"Cluster", 
		sizeof("Cluster")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUClusterJobXLS_struct, Cluster),
		NULL,
		NULL,
		-1,
	},
	{
		0x0F44464C, 
		"StartDate", 
		L"StartDate", 
		sizeof("StartDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUClusterJobXLS_struct, StartDate),
		NULL,
		NULL,
		-1,
	},
	{
		0xC7425D55, 
		"EndDate", 
		L"EndDate", 
		sizeof("EndDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUClusterJobXLS_struct, EndDate),
		NULL,
		NULL,
		-1,
	},
	{
		0xEFD537DA, 
		"ShowAll", 
		L"ShowAll", 
		sizeof("ShowAll")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUClusterJobXLS_struct, ShowAll),
		NULL,
		NULL,
		-1,
	},
	{
		0x4A703D09, 
		"BusinessStartTime", 
		L"BusinessStartTime", 
		sizeof("BusinessStartTime")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUClusterJobXLS_struct, BusinessStartTime),
		NULL,
		NULL,
		-1,
	},
	{
		0x0B0D0112, 
		"BusinessEndTime", 
		L"BusinessEndTime", 
		sizeof("BusinessEndTime")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUClusterJobXLS_struct, BusinessEndTime),
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
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUClusterJobXLS_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0xC6ABBE1F, 
		"Result", 
		L"Result", 
		sizeof("Result")-1, 
		SOAPTYPE_BASE64BINARY, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsWorkunits_WUClusterJobXLS_struct, Result),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsWorkunits_WUClusterJobXLS_map =
{
	0x852296AF,
	"WUClusterJobXLS",
	L"WUClusterJobXLSResponse",
	sizeof("WUClusterJobXLS")-1,
	sizeof("WUClusterJobXLSResponse")-1,
	SOAPMAP_FUNC,
	__CWsWorkunits_WUClusterJobXLS_entries,
	sizeof(__CWsWorkunits_WUClusterJobXLS_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x3D455C63,
	"http://webservices.seisint.com/WsWorkunits",
	L"http://webservices.seisint.com/WsWorkunits",
	sizeof("http://webservices.seisint.com/WsWorkunits")-1
};

extern __declspec(selectany) const _soapmap * __CWsWorkunits_funcs[] =
{
	&__CWsWorkunits_WUCreateAndUpdate_map,
	&__CWsWorkunits_WUSubmit_map,
	&__CWsWorkunits_WUSchedule_map,
	&__CWsWorkunits_WUPushEvent_map,
	&__CWsWorkunits_WUUpdate_map,
	&__CWsWorkunits_WUAddLocalFileToWorkunit_map,
	&__CWsWorkunits_WUExport_map,
	&__CWsWorkunits_WUQuery_map,
	&__CWsWorkunits_WUFile_map,
	&__CWsWorkunits_WUDelete_map,
	&__CWsWorkunits_WUAction_map,
	&__CWsWorkunits_WUCreate_map,
	&__CWsWorkunits_WUWaitComplete_map,
	&__CWsWorkunits_WUProtect_map,
	&__CWsWorkunits_WUCompileECL_map,
	&__CWsWorkunits_WUSyntaxCheckECL_map,
	&__CWsWorkunits_WUListLocalFileRequired_map,
	&__CWsWorkunits_WUShowScheduled_map,
	&__CWsWorkunits_WUClusterJobSummaryXLS_map,
	&__CWsWorkunits_WUInfo_map,
	&__CWsWorkunits_WUInfoDetails_map,
	&__CWsWorkunits_WUGetDependancyTrees_map,
	&__CWsWorkunits_WUAbort_map,
	&__CWsWorkunits_WUCDebug_map,
	&__CWsWorkunits_GVCAjaxGraph_map,
	&__CWsWorkunits_WUJobList_map,
	&__CWsWorkunits_WUGetGraph_map,
	&__CWsWorkunits_WUResultSummary_map,
	&__CWsWorkunits_WUGraphTiming_map,
	&__CWsWorkunits_WUClusterJobQueueXLS_map,
	&__CWsWorkunits_WUResult_map,
	&__CWsWorkunits_WUWaitCompiled_map,
	&__CWsWorkunits_WUProcessGraph_map,
	&__CWsWorkunits_WUResubmit_map,
	&__CWsWorkunits_WUClusterJobQueueLOG_map,
	&__CWsWorkunits_WUGraphInfo_map,
	&__CWsWorkunits_WUGVCGraphInfo_map,
	&__CWsWorkunits_WUResultBin_map,
	&__CWsWorkunits_WUClusterJobXLS_map,
	NULL
};

template <typename TClient>
inline HRESULT CWsWorkunitsT<TClient>::WUCreateAndUpdate(
		BSTR Wuid, 
		int State, 
		int StateOrig, 
		BSTR Jobname, 
		BSTR JobnameOrig, 
		BSTR QueryText, 
		int Action, 
		BSTR Description, 
		BSTR DescriptionOrig, 
		bool AddDrilldownFields, 
		int ResultLimit, 
		bool Protected, 
		bool ProtectedOrig, 
		int PriorityClass, 
		int PriorityLevel, 
		BSTR Scope, 
		BSTR ScopeOrig, 
		BSTR ClusterSelection, 
		BSTR ClusterOrig, 
		BSTR XmlParams, 
		BSTR ThorSlaveIP, 
		DebugValue* DebugValues, int __DebugValues_nSizeIs, 
		ApplicationValue* ApplicationValues, int __ApplicationValues_nSizeIs, 
		ArrayOfEspException* Exceptions, 
		ECLWorkunit* Workunit
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( Workunit == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsWorkunits_WUCreateAndUpdate_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Wuid = Wuid;
	__params.State = State;
	__params.StateOrig = StateOrig;
	__params.Jobname = Jobname;
	__params.JobnameOrig = JobnameOrig;
	__params.QueryText = QueryText;
	__params.Action = Action;
	__params.Description = Description;
	__params.DescriptionOrig = DescriptionOrig;
	__params.AddDrilldownFields = AddDrilldownFields;
	__params.ResultLimit = ResultLimit;
	__params.Protected = Protected;
	__params.ProtectedOrig = ProtectedOrig;
	__params.PriorityClass = PriorityClass;
	__params.PriorityLevel = PriorityLevel;
	__params.Scope = Scope;
	__params.ScopeOrig = ScopeOrig;
	__params.ClusterSelection = ClusterSelection;
	__params.ClusterOrig = ClusterOrig;
	__params.XmlParams = XmlParams;
	__params.ThorSlaveIP = ThorSlaveIP;
	__params.DebugValues = DebugValues;
	__params.__DebugValues_nSizeIs = __DebugValues_nSizeIs;
	__params.ApplicationValues = ApplicationValues;
	__params.__ApplicationValues_nSizeIs = __ApplicationValues_nSizeIs;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsWorkunits/WUCreateAndUpdate\?ver_=1.25\"\r\n"));
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
	*Workunit = __params.Workunit;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsWorkunitsT<TClient>::WUSubmit(
		BSTR Wuid, 
		BSTR Cluster, 
		BSTR Queue, 
		BSTR Snapshot, 
		int MaxRunTime, 
		int BlockTillFinishTimer, 
		bool SyntaxCheck, 
		ArrayOfEspException* Exceptions
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsWorkunits_WUSubmit_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Wuid = Wuid;
	__params.Cluster = Cluster;
	__params.Queue = Queue;
	__params.Snapshot = Snapshot;
	__params.MaxRunTime = MaxRunTime;
	__params.BlockTillFinishTimer = BlockTillFinishTimer;
	__params.SyntaxCheck = SyntaxCheck;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsWorkunits/WUSubmit\?ver_=1.25\"\r\n"));
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
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsWorkunitsT<TClient>::WUSchedule(
		BSTR Wuid, 
		BSTR Cluster, 
		BSTR Queue, 
		BSTR Snapshot, 
		BSTR When, 
		int MaxRunTime, 
		ArrayOfEspException* Exceptions
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsWorkunits_WUSchedule_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Wuid = Wuid;
	__params.Cluster = Cluster;
	__params.Queue = Queue;
	__params.Snapshot = Snapshot;
	__params.When = When;
	__params.MaxRunTime = MaxRunTime;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsWorkunits/WUSchedule\?ver_=1.25\"\r\n"));
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
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsWorkunitsT<TClient>::WUPushEvent(
		BSTR EventName, 
		BSTR EventText, 
		ArrayOfEspException* Exceptions
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsWorkunits_WUPushEvent_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.EventName = EventName;
	__params.EventText = EventText;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsWorkunits/WUPushEvent\?ver_=1.25\"\r\n"));
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
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsWorkunitsT<TClient>::WUUpdate(
		BSTR Wuid, 
		int State, 
		int StateOrig, 
		BSTR Jobname, 
		BSTR JobnameOrig, 
		BSTR QueryText, 
		int Action, 
		BSTR Description, 
		BSTR DescriptionOrig, 
		bool AddDrilldownFields, 
		int ResultLimit, 
		bool Protected, 
		bool ProtectedOrig, 
		int PriorityClass, 
		int PriorityLevel, 
		BSTR Scope, 
		BSTR ScopeOrig, 
		BSTR ClusterSelection, 
		BSTR ClusterOrig, 
		BSTR XmlParams, 
		BSTR ThorSlaveIP, 
		DebugValue* DebugValues, int __DebugValues_nSizeIs, 
		ApplicationValue* ApplicationValues, int __ApplicationValues_nSizeIs, 
		ArrayOfEspException* Exceptions, 
		ECLWorkunit* Workunit
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( Workunit == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsWorkunits_WUUpdate_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Wuid = Wuid;
	__params.State = State;
	__params.StateOrig = StateOrig;
	__params.Jobname = Jobname;
	__params.JobnameOrig = JobnameOrig;
	__params.QueryText = QueryText;
	__params.Action = Action;
	__params.Description = Description;
	__params.DescriptionOrig = DescriptionOrig;
	__params.AddDrilldownFields = AddDrilldownFields;
	__params.ResultLimit = ResultLimit;
	__params.Protected = Protected;
	__params.ProtectedOrig = ProtectedOrig;
	__params.PriorityClass = PriorityClass;
	__params.PriorityLevel = PriorityLevel;
	__params.Scope = Scope;
	__params.ScopeOrig = ScopeOrig;
	__params.ClusterSelection = ClusterSelection;
	__params.ClusterOrig = ClusterOrig;
	__params.XmlParams = XmlParams;
	__params.ThorSlaveIP = ThorSlaveIP;
	__params.DebugValues = DebugValues;
	__params.__DebugValues_nSizeIs = __DebugValues_nSizeIs;
	__params.ApplicationValues = ApplicationValues;
	__params.__ApplicationValues_nSizeIs = __ApplicationValues_nSizeIs;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsWorkunits/WUUpdate\?ver_=1.25\"\r\n"));
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
	*Workunit = __params.Workunit;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsWorkunitsT<TClient>::WUAddLocalFileToWorkunit(
		BSTR* Wuid, 
		BSTR* Name, 
		BSTR Val, 
		BSTR DefVal, 
		int Type, 
		int Length, 
		ArrayOfEspException* Exceptions, 
		BSTR* Result
	)
{
    if ( Wuid == NULL )
		return E_POINTER;
    if ( Name == NULL )
		return E_POINTER;
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( Result == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsWorkunits_WUAddLocalFileToWorkunit_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Wuid = *Wuid;
	__params.Name = *Name;
	__params.Val = Val;
	__params.DefVal = DefVal;
	__params.Type = Type;
	__params.Length = Length;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsWorkunits/WUAddLocalFileToWorkunit\?ver_=1.25\"\r\n"));
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

	*Wuid = __params.Wuid;
	*Name = __params.Name;
	*Exceptions = __params.Exceptions;
	*Result = __params.Result;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsWorkunitsT<TClient>::WUExport(
		BSTR Cluster, 
		BSTR Owner, 
		BSTR State, 
		BSTR StartDate, 
		BSTR EndDate, 
		BSTR ECL, 
		BSTR Jobname, 
		ArrayOfEspException* Exceptions, 
		ATLSOAP_BLOB* ExportData
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( ExportData == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsWorkunits_WUExport_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Cluster = Cluster;
	__params.Owner = Owner;
	__params.State = State;
	__params.StartDate = StartDate;
	__params.EndDate = EndDate;
	__params.ECL = ECL;
	__params.Jobname = Jobname;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsWorkunits/WUExport\?ver_=1.25\"\r\n"));
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
	*ExportData = __params.ExportData;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsWorkunitsT<TClient>::WUQuery(
		BSTR Wuid, 
		BSTR* Type, 
		BSTR* Cluster, 
		BSTR* RoxieCluster, 
		BSTR* Owner, 
		BSTR* State, 
		BSTR* StartDate, 
		BSTR* EndDate, 
		BSTR* ECL, 
		BSTR* Jobname, 
		BSTR* LogicalFile, 
		BSTR* LogicalFileSearchType, 
		BSTR ApplicationName, 
		BSTR ApplicationKey, 
		BSTR ApplicationData, 
		BSTR After, 
		BSTR Before, 
		int* Count, 
		__int64* PageSize, 
		__int64* PageStartFrom, 
		__int64* PageEndAt, 
		BSTR* Sortby, 
		bool* Descending, 
		ArrayOfEspException* Exceptions, 
		BSTR* Current, 
		BSTR* Next, 
		__int64* PrevPage, 
		__int64* NextPage, 
		__int64* LastPage, 
		int* NumWUs, 
		bool* First, 
		BSTR* BasicQuery, 
		BSTR* Filters, 
		ECLWorkunit** Workunits, int* __Workunits_nSizeIs
	)
{
    if ( Type == NULL )
		return E_POINTER;
    if ( Cluster == NULL )
		return E_POINTER;
    if ( RoxieCluster == NULL )
		return E_POINTER;
    if ( Owner == NULL )
		return E_POINTER;
    if ( State == NULL )
		return E_POINTER;
    if ( StartDate == NULL )
		return E_POINTER;
    if ( EndDate == NULL )
		return E_POINTER;
    if ( ECL == NULL )
		return E_POINTER;
    if ( Jobname == NULL )
		return E_POINTER;
    if ( LogicalFile == NULL )
		return E_POINTER;
    if ( LogicalFileSearchType == NULL )
		return E_POINTER;
    if ( Count == NULL )
		return E_POINTER;
    if ( PageSize == NULL )
		return E_POINTER;
    if ( PageStartFrom == NULL )
		return E_POINTER;
    if ( PageEndAt == NULL )
		return E_POINTER;
    if ( Sortby == NULL )
		return E_POINTER;
    if ( Descending == NULL )
		return E_POINTER;
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( Current == NULL )
		return E_POINTER;
    if ( Next == NULL )
		return E_POINTER;
    if ( PrevPage == NULL )
		return E_POINTER;
    if ( NextPage == NULL )
		return E_POINTER;
    if ( LastPage == NULL )
		return E_POINTER;
    if ( NumWUs == NULL )
		return E_POINTER;
    if ( First == NULL )
		return E_POINTER;
    if ( BasicQuery == NULL )
		return E_POINTER;
    if ( Filters == NULL )
		return E_POINTER;
    if ( Workunits == NULL )
		return E_POINTER;
	if( __Workunits_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsWorkunits_WUQuery_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Wuid = Wuid;
	__params.Type = *Type;
	__params.Cluster = *Cluster;
	__params.RoxieCluster = *RoxieCluster;
	__params.Owner = *Owner;
	__params.State = *State;
	__params.StartDate = *StartDate;
	__params.EndDate = *EndDate;
	__params.ECL = *ECL;
	__params.Jobname = *Jobname;
	__params.LogicalFile = *LogicalFile;
	__params.LogicalFileSearchType = *LogicalFileSearchType;
	__params.ApplicationName = ApplicationName;
	__params.ApplicationKey = ApplicationKey;
	__params.ApplicationData = ApplicationData;
	__params.After = After;
	__params.Before = Before;
	__params.Count = *Count;
	__params.PageSize = *PageSize;
	__params.PageStartFrom = *PageStartFrom;
	__params.PageEndAt = *PageEndAt;
	__params.Sortby = *Sortby;
	__params.Descending = *Descending;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsWorkunits/WUQuery\?ver_=1.25\"\r\n"));
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

	*Type = __params.Type;
	*Cluster = __params.Cluster;
	*RoxieCluster = __params.RoxieCluster;
	*Owner = __params.Owner;
	*State = __params.State;
	*StartDate = __params.StartDate;
	*EndDate = __params.EndDate;
	*ECL = __params.ECL;
	*Jobname = __params.Jobname;
	*LogicalFile = __params.LogicalFile;
	*LogicalFileSearchType = __params.LogicalFileSearchType;
	*Count = __params.Count;
	*PageSize = __params.PageSize;
	*PageStartFrom = __params.PageStartFrom;
	*PageEndAt = __params.PageEndAt;
	*Sortby = __params.Sortby;
	*Descending = __params.Descending;
	*Exceptions = __params.Exceptions;
	*Current = __params.Current;
	*Next = __params.Next;
	*PrevPage = __params.PrevPage;
	*NextPage = __params.NextPage;
	*LastPage = __params.LastPage;
	*NumWUs = __params.NumWUs;
	*First = __params.First;
	*BasicQuery = __params.BasicQuery;
	*Filters = __params.Filters;
	*Workunits = __params.Workunits;
	*__Workunits_nSizeIs = __params.__Workunits_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsWorkunitsT<TClient>::WUFile(
		BSTR Name, 
		BSTR Wuid, 
		BSTR Type, 
		BSTR SlaveIP, 
		ArrayOfEspException* Exceptions, 
		ATLSOAP_BLOB* thefile
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( thefile == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsWorkunits_WUFile_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Name = Name;
	__params.Wuid = Wuid;
	__params.Type = Type;
	__params.SlaveIP = SlaveIP;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsWorkunits/WUFile\?ver_=1.25\"\r\n"));
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
	*thefile = __params.thefile;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsWorkunitsT<TClient>::WUDelete(
		BSTR* Wuids, int __Wuids_nSizeIs, 
		int BlockTillFinishTimer, 
		ArrayOfEspException* Exceptions, 
		WUActionResult** ActionResults, int* __ActionResults_nSizeIs
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( ActionResults == NULL )
		return E_POINTER;
	if( __ActionResults_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsWorkunits_WUDelete_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Wuids = Wuids;
	__params.__Wuids_nSizeIs = __Wuids_nSizeIs;
	__params.BlockTillFinishTimer = BlockTillFinishTimer;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsWorkunits/WUDelete\?ver_=1.25\"\r\n"));
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
	*ActionResults = __params.ActionResults;
	*__ActionResults_nSizeIs = __params.__ActionResults_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsWorkunitsT<TClient>::WUAction(
		BSTR* Wuids, int __Wuids_nSizeIs, 
		BSTR ActionType, 
		BSTR Cluster, 
		BSTR Owner, 
		BSTR State, 
		BSTR StartDate, 
		BSTR EndDate, 
		BSTR ECL, 
		BSTR Jobname, 
		BSTR Test, 
		BSTR CurrentPage, 
		BSTR PageSize, 
		BSTR Sortby, 
		bool Descending, 
		BSTR EclServer, 
		BSTR EventName, 
		BSTR PageFrom, 
		int BlockTillFinishTimer, 
		ArrayOfEspException* Exceptions, 
		WUActionResult** ActionResults, int* __ActionResults_nSizeIs
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( ActionResults == NULL )
		return E_POINTER;
	if( __ActionResults_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsWorkunits_WUAction_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Wuids = Wuids;
	__params.__Wuids_nSizeIs = __Wuids_nSizeIs;
	__params.ActionType = ActionType;
	__params.Cluster = Cluster;
	__params.Owner = Owner;
	__params.State = State;
	__params.StartDate = StartDate;
	__params.EndDate = EndDate;
	__params.ECL = ECL;
	__params.Jobname = Jobname;
	__params.Test = Test;
	__params.CurrentPage = CurrentPage;
	__params.PageSize = PageSize;
	__params.Sortby = Sortby;
	__params.Descending = Descending;
	__params.EclServer = EclServer;
	__params.EventName = EventName;
	__params.PageFrom = PageFrom;
	__params.BlockTillFinishTimer = BlockTillFinishTimer;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsWorkunits/WUAction\?ver_=1.25\"\r\n"));
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
	*ActionResults = __params.ActionResults;
	*__ActionResults_nSizeIs = __params.__ActionResults_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsWorkunitsT<TClient>::WUCreate(
		ArrayOfEspException* Exceptions, 
		ECLWorkunit* Workunit
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( Workunit == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsWorkunits_WUCreate_struct __params;
	memset(&__params, 0x00, sizeof(__params));

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsWorkunits/WUCreate\?ver_=1.25\"\r\n"));
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
	*Workunit = __params.Workunit;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsWorkunitsT<TClient>::WUWaitComplete(
		BSTR Wuid, 
		int Wait, 
		bool ReturnOnWait, 
		ArrayOfEspException* Exceptions, 
		int* StateID
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( StateID == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsWorkunits_WUWaitComplete_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Wuid = Wuid;
	__params.Wait = Wait;
	__params.ReturnOnWait = ReturnOnWait;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsWorkunits/WUWaitComplete\?ver_=1.25\"\r\n"));
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
	*StateID = __params.StateID;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsWorkunitsT<TClient>::WUProtect(
		BSTR* Wuids, int __Wuids_nSizeIs, 
		bool Protect, 
		ArrayOfEspException* Exceptions, 
		WUActionResult** ActionResults, int* __ActionResults_nSizeIs
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( ActionResults == NULL )
		return E_POINTER;
	if( __ActionResults_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsWorkunits_WUProtect_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Wuids = Wuids;
	__params.__Wuids_nSizeIs = __Wuids_nSizeIs;
	__params.Protect = Protect;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsWorkunits/WUProtect\?ver_=1.25\"\r\n"));
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
	*ActionResults = __params.ActionResults;
	*__ActionResults_nSizeIs = __params.__ActionResults_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsWorkunitsT<TClient>::WUCompileECL(
		BSTR ECL, 
		BSTR ModuleName, 
		BSTR AttributeName, 
		BSTR Queue, 
		BSTR Cluster, 
		BSTR Snapshot, 
		bool IncludeDependencies, 
		bool IncludeComplexity, 
		int TimeToWait, 
		ArrayOfEspException* Exceptions, 
		BSTR* Complexity, 
		ECLException** Errors, int* __Errors_nSizeIs, 
		WUECLAttribute** Dependencies, int* __Dependencies_nSizeIs
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( Complexity == NULL )
		return E_POINTER;
    if ( Errors == NULL )
		return E_POINTER;
	if( __Errors_nSizeIs == NULL )
		return E_POINTER;
    if ( Dependencies == NULL )
		return E_POINTER;
	if( __Dependencies_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsWorkunits_WUCompileECL_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.ECL = ECL;
	__params.ModuleName = ModuleName;
	__params.AttributeName = AttributeName;
	__params.Queue = Queue;
	__params.Cluster = Cluster;
	__params.Snapshot = Snapshot;
	__params.IncludeDependencies = IncludeDependencies;
	__params.IncludeComplexity = IncludeComplexity;
	__params.TimeToWait = TimeToWait;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsWorkunits/WUCompileECL\?ver_=1.25\"\r\n"));
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
	*Complexity = __params.Complexity;
	*Errors = __params.Errors;
	*__Errors_nSizeIs = __params.__Errors_nSizeIs;
	*Dependencies = __params.Dependencies;
	*__Dependencies_nSizeIs = __params.__Dependencies_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsWorkunitsT<TClient>::WUSyntaxCheckECL(
		BSTR ECL, 
		BSTR ModuleName, 
		BSTR AttributeName, 
		BSTR Queue, 
		BSTR Cluster, 
		BSTR Snapshot, 
		int TimeToWait, 
		DebugValue* DebugValues, int __DebugValues_nSizeIs, 
		ArrayOfEspException* Exceptions, 
		ECLException** Errors, int* __Errors_nSizeIs
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( Errors == NULL )
		return E_POINTER;
	if( __Errors_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsWorkunits_WUSyntaxCheckECL_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.ECL = ECL;
	__params.ModuleName = ModuleName;
	__params.AttributeName = AttributeName;
	__params.Queue = Queue;
	__params.Cluster = Cluster;
	__params.Snapshot = Snapshot;
	__params.TimeToWait = TimeToWait;
	__params.DebugValues = DebugValues;
	__params.__DebugValues_nSizeIs = __DebugValues_nSizeIs;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsWorkunits/WUSyntaxCheckECL\?ver_=1.25\"\r\n"));
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
	*Errors = __params.Errors;
	*__Errors_nSizeIs = __params.__Errors_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsWorkunitsT<TClient>::WUListLocalFileRequired(
		BSTR Wuid, 
		ArrayOfEspException* Exceptions, 
		LogicalFileUpload** LocalFileUploads, int* __LocalFileUploads_nSizeIs
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( LocalFileUploads == NULL )
		return E_POINTER;
	if( __LocalFileUploads_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsWorkunits_WUListLocalFileRequired_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Wuid = Wuid;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsWorkunits/WUListLocalFileRequired\?ver_=1.25\"\r\n"));
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
	*LocalFileUploads = __params.LocalFileUploads;
	*__LocalFileUploads_nSizeIs = __params.__LocalFileUploads_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsWorkunitsT<TClient>::WUShowScheduled(
		BSTR EclServer, 
		BSTR* EventName, 
		BSTR* PushEventName, 
		BSTR* PushEventText, 
		ArrayOfEspException* Exceptions, 
		int* EclServerSelected, 
		BSTR* Query, 
		ServerInfo** EclServers, int* __EclServers_nSizeIs, 
		ScheduledWU** Workunits, int* __Workunits_nSizeIs
	)
{
    if ( EventName == NULL )
		return E_POINTER;
    if ( PushEventName == NULL )
		return E_POINTER;
    if ( PushEventText == NULL )
		return E_POINTER;
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( EclServerSelected == NULL )
		return E_POINTER;
    if ( Query == NULL )
		return E_POINTER;
    if ( EclServers == NULL )
		return E_POINTER;
	if( __EclServers_nSizeIs == NULL )
		return E_POINTER;
    if ( Workunits == NULL )
		return E_POINTER;
	if( __Workunits_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsWorkunits_WUShowScheduled_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.EclServer = EclServer;
	__params.EventName = *EventName;
	__params.PushEventName = *PushEventName;
	__params.PushEventText = *PushEventText;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsWorkunits/WUShowScheduled\?ver_=1.25\"\r\n"));
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

	*EventName = __params.EventName;
	*PushEventName = __params.PushEventName;
	*PushEventText = __params.PushEventText;
	*Exceptions = __params.Exceptions;
	*EclServerSelected = __params.EclServerSelected;
	*Query = __params.Query;
	*EclServers = __params.EclServers;
	*__EclServers_nSizeIs = __params.__EclServers_nSizeIs;
	*Workunits = __params.Workunits;
	*__Workunits_nSizeIs = __params.__Workunits_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsWorkunitsT<TClient>::WUClusterJobSummaryXLS(
		BSTR Cluster, 
		BSTR StartDate, 
		BSTR EndDate, 
		bool ShowAll, 
		BSTR BusinessStartTime, 
		BSTR BusinessEndTime, 
		ArrayOfEspException* Exceptions, 
		ATLSOAP_BLOB* Result
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( Result == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsWorkunits_WUClusterJobSummaryXLS_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Cluster = Cluster;
	__params.StartDate = StartDate;
	__params.EndDate = EndDate;
	__params.ShowAll = ShowAll;
	__params.BusinessStartTime = BusinessStartTime;
	__params.BusinessEndTime = BusinessEndTime;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsWorkunits/WUClusterJobSummaryXLS\?ver_=1.25\"\r\n"));
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
	*Result = __params.Result;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsWorkunitsT<TClient>::WUInfo(
		BSTR Wuid, 
		BSTR Type, 
		bool IncludeExceptions, 
		bool IncludeGraphs, 
		bool IncludeSourceFiles, 
		bool IncludeResults, 
		bool IncludeVariables, 
		bool IncludeTimers, 
		bool IncludeDebugValues, 
		bool IncludeApplicationValues, 
		bool IncludeWorkflows, 
		bool SuppressResultSchemas, 
		BSTR* ThorSlaveIP, 
		ArrayOfEspException* Exceptions, 
		ECLWorkunit* Workunit, 
		int* AutoRefresh, 
		bool* CanCompile
	)
{
    if ( ThorSlaveIP == NULL )
		return E_POINTER;
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( Workunit == NULL )
		return E_POINTER;
    if ( AutoRefresh == NULL )
		return E_POINTER;
    if ( CanCompile == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsWorkunits_WUInfo_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Wuid = Wuid;
	__params.Type = Type;
	__params.IncludeExceptions = IncludeExceptions;
	__params.IncludeGraphs = IncludeGraphs;
	__params.IncludeSourceFiles = IncludeSourceFiles;
	__params.IncludeResults = IncludeResults;
	__params.IncludeVariables = IncludeVariables;
	__params.IncludeTimers = IncludeTimers;
	__params.IncludeDebugValues = IncludeDebugValues;
	__params.IncludeApplicationValues = IncludeApplicationValues;
	__params.IncludeWorkflows = IncludeWorkflows;
	__params.SuppressResultSchemas = SuppressResultSchemas;
	__params.ThorSlaveIP = *ThorSlaveIP;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsWorkunits/WUInfo\?ver_=1.25\"\r\n"));
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

	*ThorSlaveIP = __params.ThorSlaveIP;
	*Exceptions = __params.Exceptions;
	*Workunit = __params.Workunit;
	*AutoRefresh = __params.AutoRefresh;
	*CanCompile = __params.CanCompile;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsWorkunitsT<TClient>::WUInfoDetails(
		BSTR Wuid, 
		BSTR Type, 
		bool IncludeExceptions, 
		bool IncludeGraphs, 
		bool IncludeSourceFiles, 
		bool IncludeResults, 
		bool IncludeVariables, 
		bool IncludeTimers, 
		bool IncludeDebugValues, 
		bool IncludeApplicationValues, 
		bool IncludeWorkflows, 
		bool SuppressResultSchemas, 
		BSTR* ThorSlaveIP, 
		ArrayOfEspException* Exceptions, 
		ECLWorkunit* Workunit, 
		int* AutoRefresh, 
		bool* CanCompile
	)
{
    if ( ThorSlaveIP == NULL )
		return E_POINTER;
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( Workunit == NULL )
		return E_POINTER;
    if ( AutoRefresh == NULL )
		return E_POINTER;
    if ( CanCompile == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsWorkunits_WUInfoDetails_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Wuid = Wuid;
	__params.Type = Type;
	__params.IncludeExceptions = IncludeExceptions;
	__params.IncludeGraphs = IncludeGraphs;
	__params.IncludeSourceFiles = IncludeSourceFiles;
	__params.IncludeResults = IncludeResults;
	__params.IncludeVariables = IncludeVariables;
	__params.IncludeTimers = IncludeTimers;
	__params.IncludeDebugValues = IncludeDebugValues;
	__params.IncludeApplicationValues = IncludeApplicationValues;
	__params.IncludeWorkflows = IncludeWorkflows;
	__params.SuppressResultSchemas = SuppressResultSchemas;
	__params.ThorSlaveIP = *ThorSlaveIP;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsWorkunits/WUInfoDetails\?ver_=1.25\"\r\n"));
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

	*ThorSlaveIP = __params.ThorSlaveIP;
	*Exceptions = __params.Exceptions;
	*Workunit = __params.Workunit;
	*AutoRefresh = __params.AutoRefresh;
	*CanCompile = __params.CanCompile;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsWorkunitsT<TClient>::WUGetDependancyTrees(
		BSTR Cluster, 
		BSTR Queue, 
		BSTR Snapshot, 
		BSTR Items, 
		BSTR TimeoutMilliSec, 
		ArrayOfEspException* Exceptions, 
		ECLException** Errors, int* __Errors_nSizeIs, 
		ATLSOAP_BLOB* DependancyTrees
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( Errors == NULL )
		return E_POINTER;
	if( __Errors_nSizeIs == NULL )
		return E_POINTER;
    if ( DependancyTrees == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsWorkunits_WUGetDependancyTrees_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Cluster = Cluster;
	__params.Queue = Queue;
	__params.Snapshot = Snapshot;
	__params.Items = Items;
	__params.TimeoutMilliSec = TimeoutMilliSec;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsWorkunits/WUGetDependancyTrees\?ver_=1.25\"\r\n"));
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
	*Errors = __params.Errors;
	*__Errors_nSizeIs = __params.__Errors_nSizeIs;
	*DependancyTrees = __params.DependancyTrees;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsWorkunitsT<TClient>::WUAbort(
		BSTR* Wuids, int __Wuids_nSizeIs, 
		int BlockTillFinishTimer, 
		ArrayOfEspException* Exceptions, 
		WUActionResult** ActionResults, int* __ActionResults_nSizeIs
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( ActionResults == NULL )
		return E_POINTER;
	if( __ActionResults_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsWorkunits_WUAbort_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Wuids = Wuids;
	__params.__Wuids_nSizeIs = __Wuids_nSizeIs;
	__params.BlockTillFinishTimer = BlockTillFinishTimer;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsWorkunits/WUAbort\?ver_=1.25\"\r\n"));
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
	*ActionResults = __params.ActionResults;
	*__ActionResults_nSizeIs = __params.__ActionResults_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsWorkunitsT<TClient>::WUCDebug(
		BSTR Wuid, 
		BSTR Command, 
		ArrayOfEspException* Exceptions, 
		BSTR* Result
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( Result == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsWorkunits_WUCDebug_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Wuid = Wuid;
	__params.Command = Command;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsWorkunits/WUCDebug\?ver_=1.25\"\r\n"));
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
	*Result = __params.Result;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsWorkunitsT<TClient>::GVCAjaxGraph(
		BSTR* Name, 
		BSTR* GraphName, 
		int* SubGraphId, 
		bool* SubGraphOnly, 
		BSTR* GraphType
	)
{
    if ( Name == NULL )
		return E_POINTER;
    if ( GraphName == NULL )
		return E_POINTER;
    if ( SubGraphId == NULL )
		return E_POINTER;
    if ( SubGraphOnly == NULL )
		return E_POINTER;
    if ( GraphType == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsWorkunits_GVCAjaxGraph_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Name = *Name;
	__params.GraphName = *GraphName;
	__params.SubGraphId = *SubGraphId;
	__params.SubGraphOnly = *SubGraphOnly;

	__atlsoap_hr = SetClientStruct(&__params, 24);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsWorkunits/GVCAjaxGraph\?ver_=1.25\"\r\n"));
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

	*Name = __params.Name;
	*GraphName = __params.GraphName;
	*SubGraphId = __params.SubGraphId;
	*SubGraphOnly = __params.SubGraphOnly;
	*GraphType = __params.GraphType;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsWorkunitsT<TClient>::WUJobList(
		BSTR Cluster, 
		BSTR* StartDate, 
		BSTR* EndDate, 
		bool ShowAll, 
		int BusinessStartHour, 
		int BusinessEndHour, 
		ArrayOfEspException* Exceptions, 
		ECLJob** Jobs, int* __Jobs_nSizeIs
	)
{
    if ( StartDate == NULL )
		return E_POINTER;
    if ( EndDate == NULL )
		return E_POINTER;
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( Jobs == NULL )
		return E_POINTER;
	if( __Jobs_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsWorkunits_WUJobList_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Cluster = Cluster;
	__params.StartDate = *StartDate;
	__params.EndDate = *EndDate;
	__params.ShowAll = ShowAll;
	__params.BusinessStartHour = BusinessStartHour;
	__params.BusinessEndHour = BusinessEndHour;

	__atlsoap_hr = SetClientStruct(&__params, 25);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsWorkunits/WUJobList\?ver_=1.25\"\r\n"));
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

	*StartDate = __params.StartDate;
	*EndDate = __params.EndDate;
	*Exceptions = __params.Exceptions;
	*Jobs = __params.Jobs;
	*__Jobs_nSizeIs = __params.__Jobs_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsWorkunitsT<TClient>::WUGetGraph(
		BSTR Wuid, 
		BSTR GraphName, 
		BSTR SubGraphId, 
		ArrayOfEspException* Exceptions, 
		ECLGraphEx** Graphs, int* __Graphs_nSizeIs
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( Graphs == NULL )
		return E_POINTER;
	if( __Graphs_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsWorkunits_WUGetGraph_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Wuid = Wuid;
	__params.GraphName = GraphName;
	__params.SubGraphId = SubGraphId;

	__atlsoap_hr = SetClientStruct(&__params, 26);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsWorkunits/WUGetGraph\?ver_=1.25\"\r\n"));
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
	*Graphs = __params.Graphs;
	*__Graphs_nSizeIs = __params.__Graphs_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsWorkunitsT<TClient>::WUResultSummary(
		BSTR* Wuid, 
		int* Sequence, 
		ArrayOfEspException* Exceptions, 
		int* Format, 
		ECLResult* Result
	)
{
    if ( Wuid == NULL )
		return E_POINTER;
    if ( Sequence == NULL )
		return E_POINTER;
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( Format == NULL )
		return E_POINTER;
    if ( Result == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsWorkunits_WUResultSummary_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Wuid = *Wuid;
	__params.Sequence = *Sequence;

	__atlsoap_hr = SetClientStruct(&__params, 27);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsWorkunits/WUResultSummary\?ver_=1.25\"\r\n"));
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

	*Wuid = __params.Wuid;
	*Sequence = __params.Sequence;
	*Exceptions = __params.Exceptions;
	*Format = __params.Format;
	*Result = __params.Result;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsWorkunitsT<TClient>::WUGraphTiming(
		BSTR Wuid, 
		ArrayOfEspException* Exceptions, 
		ECLWorkunit* Workunit
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( Workunit == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsWorkunits_WUGraphTiming_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Wuid = Wuid;

	__atlsoap_hr = SetClientStruct(&__params, 28);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsWorkunits/WUGraphTiming\?ver_=1.25\"\r\n"));
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
	*Workunit = __params.Workunit;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsWorkunitsT<TClient>::WUClusterJobQueueXLS(
		BSTR Cluster, 
		BSTR StartDate, 
		BSTR EndDate, 
		BSTR ShowType, 
		ArrayOfEspException* Exceptions, 
		ATLSOAP_BLOB* Result
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( Result == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsWorkunits_WUClusterJobQueueXLS_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Cluster = Cluster;
	__params.StartDate = StartDate;
	__params.EndDate = EndDate;
	__params.ShowType = ShowType;

	__atlsoap_hr = SetClientStruct(&__params, 29);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsWorkunits/WUClusterJobQueueXLS\?ver_=1.25\"\r\n"));
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
	*Result = __params.Result;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsWorkunitsT<TClient>::WUResult(
		BSTR* Wuid, 
		int* Sequence, 
		BSTR ResultName, 
		BSTR* LogicalName, 
		BSTR* Cluster, 
		__int64* Start, 
		int* Count, 
		ArrayOfEspException* Exceptions, 
		BSTR* Name, 
		int* Requested, 
		__int64* Total, 
		BSTR* Result
	)
{
    if ( Wuid == NULL )
		return E_POINTER;
    if ( Sequence == NULL )
		return E_POINTER;
    if ( LogicalName == NULL )
		return E_POINTER;
    if ( Cluster == NULL )
		return E_POINTER;
    if ( Start == NULL )
		return E_POINTER;
    if ( Count == NULL )
		return E_POINTER;
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( Name == NULL )
		return E_POINTER;
    if ( Requested == NULL )
		return E_POINTER;
    if ( Total == NULL )
		return E_POINTER;
    if ( Result == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsWorkunits_WUResult_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Wuid = *Wuid;
	__params.Sequence = *Sequence;
	__params.ResultName = ResultName;
	__params.LogicalName = *LogicalName;
	__params.Cluster = *Cluster;
	__params.Start = *Start;
	__params.Count = *Count;

	__atlsoap_hr = SetClientStruct(&__params, 30);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsWorkunits/WUResult\?ver_=1.25\"\r\n"));
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

	*Wuid = __params.Wuid;
	*Sequence = __params.Sequence;
	*LogicalName = __params.LogicalName;
	*Cluster = __params.Cluster;
	*Start = __params.Start;
	*Count = __params.Count;
	*Exceptions = __params.Exceptions;
	*Name = __params.Name;
	*Requested = __params.Requested;
	*Total = __params.Total;
	*Result = __params.Result;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsWorkunitsT<TClient>::WUWaitCompiled(
		BSTR Wuid, 
		int Wait, 
		bool ReturnOnWait, 
		ArrayOfEspException* Exceptions, 
		int* StateID
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( StateID == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsWorkunits_WUWaitCompiled_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Wuid = Wuid;
	__params.Wait = Wait;
	__params.ReturnOnWait = ReturnOnWait;

	__atlsoap_hr = SetClientStruct(&__params, 31);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsWorkunits/WUWaitCompiled\?ver_=1.25\"\r\n"));
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
	*StateID = __params.StateID;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsWorkunitsT<TClient>::WUProcessGraph(
		BSTR FileName, 
		BSTR PopupId, 
		bool XmlGraph, 
		bool Stats, 
		ArrayOfEspException* Exceptions, 
		ATLSOAP_BLOB* theGraph
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( theGraph == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsWorkunits_WUProcessGraph_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.FileName = FileName;
	__params.PopupId = PopupId;
	__params.XmlGraph = XmlGraph;
	__params.Stats = Stats;

	__atlsoap_hr = SetClientStruct(&__params, 32);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsWorkunits/WUProcessGraph\?ver_=1.25\"\r\n"));
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
	*theGraph = __params.theGraph;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsWorkunitsT<TClient>::WUResubmit(
		BSTR* Wuids, int __Wuids_nSizeIs, 
		bool Recompile, 
		int BlockTillFinishTimer, 
		ArrayOfEspException* Exceptions
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsWorkunits_WUResubmit_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Wuids = Wuids;
	__params.__Wuids_nSizeIs = __Wuids_nSizeIs;
	__params.Recompile = Recompile;
	__params.BlockTillFinishTimer = BlockTillFinishTimer;

	__atlsoap_hr = SetClientStruct(&__params, 33);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsWorkunits/WUResubmit\?ver_=1.25\"\r\n"));
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
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsWorkunitsT<TClient>::WUClusterJobQueueLOG(
		BSTR Cluster, 
		BSTR StartDate, 
		BSTR EndDate, 
		ArrayOfEspException* Exceptions, 
		ATLSOAP_BLOB* thefile
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( thefile == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsWorkunits_WUClusterJobQueueLOG_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Cluster = Cluster;
	__params.StartDate = StartDate;
	__params.EndDate = EndDate;

	__atlsoap_hr = SetClientStruct(&__params, 34);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsWorkunits/WUClusterJobQueueLOG\?ver_=1.25\"\r\n"));
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
	*thefile = __params.thefile;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsWorkunitsT<TClient>::WUGraphInfo(
		BSTR* Wuid, 
		BSTR* Name, 
		BSTR* GID, 
		int* BatchWU, 
		ArrayOfEspException* Exceptions, 
		bool* Running
	)
{
    if ( Wuid == NULL )
		return E_POINTER;
    if ( Name == NULL )
		return E_POINTER;
    if ( GID == NULL )
		return E_POINTER;
    if ( BatchWU == NULL )
		return E_POINTER;
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( Running == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsWorkunits_WUGraphInfo_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Wuid = *Wuid;
	__params.Name = *Name;
	__params.GID = *GID;
	__params.BatchWU = *BatchWU;

	__atlsoap_hr = SetClientStruct(&__params, 35);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsWorkunits/WUGraphInfo\?ver_=1.25\"\r\n"));
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

	*Wuid = __params.Wuid;
	*Name = __params.Name;
	*GID = __params.GID;
	*BatchWU = __params.BatchWU;
	*Exceptions = __params.Exceptions;
	*Running = __params.Running;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsWorkunitsT<TClient>::WUGVCGraphInfo(
		BSTR* Wuid, 
		BSTR* Name, 
		BSTR* GID, 
		int* BatchWU, 
		int SubgraphId, 
		ArrayOfEspException* Exceptions, 
		bool* Running, 
		BSTR* TheGraph
	)
{
    if ( Wuid == NULL )
		return E_POINTER;
    if ( Name == NULL )
		return E_POINTER;
    if ( GID == NULL )
		return E_POINTER;
    if ( BatchWU == NULL )
		return E_POINTER;
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( Running == NULL )
		return E_POINTER;
    if ( TheGraph == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsWorkunits_WUGVCGraphInfo_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Wuid = *Wuid;
	__params.Name = *Name;
	__params.GID = *GID;
	__params.BatchWU = *BatchWU;
	__params.SubgraphId = SubgraphId;

	__atlsoap_hr = SetClientStruct(&__params, 36);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsWorkunits/WUGVCGraphInfo\?ver_=1.25\"\r\n"));
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

	*Wuid = __params.Wuid;
	*Name = __params.Name;
	*GID = __params.GID;
	*BatchWU = __params.BatchWU;
	*Exceptions = __params.Exceptions;
	*Running = __params.Running;
	*TheGraph = __params.TheGraph;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsWorkunitsT<TClient>::WUResultBin(
		BSTR LogicalName, 
		BSTR* Wuid, 
		BSTR ResultName, 
		int* Sequence, 
		BSTR* Format, 
		__int64* Start, 
		int* Count, 
		ArrayOfEspException* Exceptions, 
		BSTR* Name, 
		int* Requested, 
		__int64* Total, 
		ATLSOAP_BLOB* Result
	)
{
    if ( Wuid == NULL )
		return E_POINTER;
    if ( Sequence == NULL )
		return E_POINTER;
    if ( Format == NULL )
		return E_POINTER;
    if ( Start == NULL )
		return E_POINTER;
    if ( Count == NULL )
		return E_POINTER;
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( Name == NULL )
		return E_POINTER;
    if ( Requested == NULL )
		return E_POINTER;
    if ( Total == NULL )
		return E_POINTER;
    if ( Result == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsWorkunits_WUResultBin_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.LogicalName = LogicalName;
	__params.Wuid = *Wuid;
	__params.ResultName = ResultName;
	__params.Sequence = *Sequence;
	__params.Format = *Format;
	__params.Start = *Start;
	__params.Count = *Count;

	__atlsoap_hr = SetClientStruct(&__params, 37);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsWorkunits/WUResultBin\?ver_=1.25\"\r\n"));
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

	*Wuid = __params.Wuid;
	*Sequence = __params.Sequence;
	*Format = __params.Format;
	*Start = __params.Start;
	*Count = __params.Count;
	*Exceptions = __params.Exceptions;
	*Name = __params.Name;
	*Requested = __params.Requested;
	*Total = __params.Total;
	*Result = __params.Result;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsWorkunitsT<TClient>::WUClusterJobXLS(
		BSTR Cluster, 
		BSTR StartDate, 
		BSTR EndDate, 
		bool ShowAll, 
		BSTR BusinessStartTime, 
		BSTR BusinessEndTime, 
		ArrayOfEspException* Exceptions, 
		ATLSOAP_BLOB* Result
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( Result == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsWorkunits_WUClusterJobXLS_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Cluster = Cluster;
	__params.StartDate = StartDate;
	__params.EndDate = EndDate;
	__params.ShowAll = ShowAll;
	__params.BusinessStartTime = BusinessStartTime;
	__params.BusinessEndTime = BusinessEndTime;

	__atlsoap_hr = SetClientStruct(&__params, 38);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsWorkunits/WUClusterJobXLS\?ver_=1.25\"\r\n"));
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
	*Result = __params.Result;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
ATL_NOINLINE inline const _soapmap ** CWsWorkunitsT<TClient>::GetFunctionMap()
{
	return __CWsWorkunits_funcs;
}

template <typename TClient>
ATL_NOINLINE inline const _soapmap ** CWsWorkunitsT<TClient>::GetHeaderMap()
{
	static const _soapmapentry __CWsWorkunits_WUCreateAndUpdate_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsWorkunits_WUCreateAndUpdate_atlsoapheader_map = 
	{
		0x53FCFD7E,
		"WUCreateAndUpdate",
		L"WUUpdateResponse",
		sizeof("WUCreateAndUpdate")-1,
		sizeof("WUUpdateResponse")-1,
		SOAPMAP_HEADER,
		__CWsWorkunits_WUCreateAndUpdate_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x3D455C63,
		"http://webservices.seisint.com/WsWorkunits",
		L"http://webservices.seisint.com/WsWorkunits",
		sizeof("http://webservices.seisint.com/WsWorkunits")-1
	};

	static const _soapmapentry __CWsWorkunits_WUSubmit_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsWorkunits_WUSubmit_atlsoapheader_map = 
	{
		0xEF9FE0AF,
		"WUSubmit",
		L"WUSubmitResponse",
		sizeof("WUSubmit")-1,
		sizeof("WUSubmitResponse")-1,
		SOAPMAP_HEADER,
		__CWsWorkunits_WUSubmit_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x3D455C63,
		"http://webservices.seisint.com/WsWorkunits",
		L"http://webservices.seisint.com/WsWorkunits",
		sizeof("http://webservices.seisint.com/WsWorkunits")-1
	};

	static const _soapmapentry __CWsWorkunits_WUSchedule_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsWorkunits_WUSchedule_atlsoapheader_map = 
	{
		0x04868888,
		"WUSchedule",
		L"WUScheduleResponse",
		sizeof("WUSchedule")-1,
		sizeof("WUScheduleResponse")-1,
		SOAPMAP_HEADER,
		__CWsWorkunits_WUSchedule_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x3D455C63,
		"http://webservices.seisint.com/WsWorkunits",
		L"http://webservices.seisint.com/WsWorkunits",
		sizeof("http://webservices.seisint.com/WsWorkunits")-1
	};

	static const _soapmapentry __CWsWorkunits_WUPushEvent_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsWorkunits_WUPushEvent_atlsoapheader_map = 
	{
		0x42A74C3D,
		"WUPushEvent",
		L"WUPushEventResponse",
		sizeof("WUPushEvent")-1,
		sizeof("WUPushEventResponse")-1,
		SOAPMAP_HEADER,
		__CWsWorkunits_WUPushEvent_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x3D455C63,
		"http://webservices.seisint.com/WsWorkunits",
		L"http://webservices.seisint.com/WsWorkunits",
		sizeof("http://webservices.seisint.com/WsWorkunits")-1
	};

	static const _soapmapentry __CWsWorkunits_WUUpdate_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsWorkunits_WUUpdate_atlsoapheader_map = 
	{
		0x53FCFD7E,
		"WUUpdate",
		L"WUUpdateResponse",
		sizeof("WUUpdate")-1,
		sizeof("WUUpdateResponse")-1,
		SOAPMAP_HEADER,
		__CWsWorkunits_WUUpdate_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x3D455C63,
		"http://webservices.seisint.com/WsWorkunits",
		L"http://webservices.seisint.com/WsWorkunits",
		sizeof("http://webservices.seisint.com/WsWorkunits")-1
	};

	static const _soapmapentry __CWsWorkunits_WUAddLocalFileToWorkunit_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsWorkunits_WUAddLocalFileToWorkunit_atlsoapheader_map = 
	{
		0xC60E2355,
		"WUAddLocalFileToWorkunit",
		L"WUAddLocalFileToWorkunitResponse",
		sizeof("WUAddLocalFileToWorkunit")-1,
		sizeof("WUAddLocalFileToWorkunitResponse")-1,
		SOAPMAP_HEADER,
		__CWsWorkunits_WUAddLocalFileToWorkunit_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x3D455C63,
		"http://webservices.seisint.com/WsWorkunits",
		L"http://webservices.seisint.com/WsWorkunits",
		sizeof("http://webservices.seisint.com/WsWorkunits")-1
	};

	static const _soapmapentry __CWsWorkunits_WUExport_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsWorkunits_WUExport_atlsoapheader_map = 
	{
		0x7CA1765D,
		"WUExport",
		L"WUExportResponse",
		sizeof("WUExport")-1,
		sizeof("WUExportResponse")-1,
		SOAPMAP_HEADER,
		__CWsWorkunits_WUExport_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x3D455C63,
		"http://webservices.seisint.com/WsWorkunits",
		L"http://webservices.seisint.com/WsWorkunits",
		sizeof("http://webservices.seisint.com/WsWorkunits")-1
	};

	static const _soapmapentry __CWsWorkunits_WUQuery_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsWorkunits_WUQuery_atlsoapheader_map = 
	{
		0x3F398D31,
		"WUQuery",
		L"WUQueryResponse",
		sizeof("WUQuery")-1,
		sizeof("WUQueryResponse")-1,
		SOAPMAP_HEADER,
		__CWsWorkunits_WUQuery_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x3D455C63,
		"http://webservices.seisint.com/WsWorkunits",
		L"http://webservices.seisint.com/WsWorkunits",
		sizeof("http://webservices.seisint.com/WsWorkunits")-1
	};

	static const _soapmapentry __CWsWorkunits_WUFile_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsWorkunits_WUFile_atlsoapheader_map = 
	{
		0xEE5658BD,
		"WUFile",
		L"WULogFileResponse",
		sizeof("WUFile")-1,
		sizeof("WULogFileResponse")-1,
		SOAPMAP_HEADER,
		__CWsWorkunits_WUFile_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x3D455C63,
		"http://webservices.seisint.com/WsWorkunits",
		L"http://webservices.seisint.com/WsWorkunits",
		sizeof("http://webservices.seisint.com/WsWorkunits")-1
	};

	static const _soapmapentry __CWsWorkunits_WUDelete_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsWorkunits_WUDelete_atlsoapheader_map = 
	{
		0x98B1814E,
		"WUDelete",
		L"WUDeleteResponse",
		sizeof("WUDelete")-1,
		sizeof("WUDeleteResponse")-1,
		SOAPMAP_HEADER,
		__CWsWorkunits_WUDelete_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x3D455C63,
		"http://webservices.seisint.com/WsWorkunits",
		L"http://webservices.seisint.com/WsWorkunits",
		sizeof("http://webservices.seisint.com/WsWorkunits")-1
	};

	static const _soapmapentry __CWsWorkunits_WUAction_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsWorkunits_WUAction_atlsoapheader_map = 
	{
		0xE96BA4D9,
		"WUAction",
		L"WUActionResponse",
		sizeof("WUAction")-1,
		sizeof("WUActionResponse")-1,
		SOAPMAP_HEADER,
		__CWsWorkunits_WUAction_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x3D455C63,
		"http://webservices.seisint.com/WsWorkunits",
		L"http://webservices.seisint.com/WsWorkunits",
		sizeof("http://webservices.seisint.com/WsWorkunits")-1
	};

	static const _soapmapentry __CWsWorkunits_WUCreate_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsWorkunits_WUCreate_atlsoapheader_map = 
	{
		0xF92A608F,
		"WUCreate",
		L"WUCreateResponse",
		sizeof("WUCreate")-1,
		sizeof("WUCreateResponse")-1,
		SOAPMAP_HEADER,
		__CWsWorkunits_WUCreate_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x3D455C63,
		"http://webservices.seisint.com/WsWorkunits",
		L"http://webservices.seisint.com/WsWorkunits",
		sizeof("http://webservices.seisint.com/WsWorkunits")-1
	};

	static const _soapmapentry __CWsWorkunits_WUWaitComplete_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsWorkunits_WUWaitComplete_atlsoapheader_map = 
	{
		0x392AC450,
		"WUWaitComplete",
		L"WUWaitResponse",
		sizeof("WUWaitComplete")-1,
		sizeof("WUWaitResponse")-1,
		SOAPMAP_HEADER,
		__CWsWorkunits_WUWaitComplete_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x3D455C63,
		"http://webservices.seisint.com/WsWorkunits",
		L"http://webservices.seisint.com/WsWorkunits",
		sizeof("http://webservices.seisint.com/WsWorkunits")-1
	};

	static const _soapmapentry __CWsWorkunits_WUProtect_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsWorkunits_WUProtect_atlsoapheader_map = 
	{
		0x0D607FFC,
		"WUProtect",
		L"WUProtectResponse",
		sizeof("WUProtect")-1,
		sizeof("WUProtectResponse")-1,
		SOAPMAP_HEADER,
		__CWsWorkunits_WUProtect_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x3D455C63,
		"http://webservices.seisint.com/WsWorkunits",
		L"http://webservices.seisint.com/WsWorkunits",
		sizeof("http://webservices.seisint.com/WsWorkunits")-1
	};

	static const _soapmapentry __CWsWorkunits_WUCompileECL_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsWorkunits_WUCompileECL_atlsoapheader_map = 
	{
		0x875A8A38,
		"WUCompileECL",
		L"WUCompileECLResponse",
		sizeof("WUCompileECL")-1,
		sizeof("WUCompileECLResponse")-1,
		SOAPMAP_HEADER,
		__CWsWorkunits_WUCompileECL_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x3D455C63,
		"http://webservices.seisint.com/WsWorkunits",
		L"http://webservices.seisint.com/WsWorkunits",
		sizeof("http://webservices.seisint.com/WsWorkunits")-1
	};

	static const _soapmapentry __CWsWorkunits_WUSyntaxCheckECL_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsWorkunits_WUSyntaxCheckECL_atlsoapheader_map = 
	{
		0x2035DCE0,
		"WUSyntaxCheckECL",
		L"WUSyntaxCheckResponse",
		sizeof("WUSyntaxCheckECL")-1,
		sizeof("WUSyntaxCheckResponse")-1,
		SOAPMAP_HEADER,
		__CWsWorkunits_WUSyntaxCheckECL_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x3D455C63,
		"http://webservices.seisint.com/WsWorkunits",
		L"http://webservices.seisint.com/WsWorkunits",
		sizeof("http://webservices.seisint.com/WsWorkunits")-1
	};

	static const _soapmapentry __CWsWorkunits_WUListLocalFileRequired_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsWorkunits_WUListLocalFileRequired_atlsoapheader_map = 
	{
		0xE3D48A43,
		"WUListLocalFileRequired",
		L"WUListLocalFileRequiredResponse",
		sizeof("WUListLocalFileRequired")-1,
		sizeof("WUListLocalFileRequiredResponse")-1,
		SOAPMAP_HEADER,
		__CWsWorkunits_WUListLocalFileRequired_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x3D455C63,
		"http://webservices.seisint.com/WsWorkunits",
		L"http://webservices.seisint.com/WsWorkunits",
		sizeof("http://webservices.seisint.com/WsWorkunits")-1
	};

	static const _soapmapentry __CWsWorkunits_WUShowScheduled_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsWorkunits_WUShowScheduled_atlsoapheader_map = 
	{
		0x968F60CD,
		"WUShowScheduled",
		L"WUShowScheduledResponse",
		sizeof("WUShowScheduled")-1,
		sizeof("WUShowScheduledResponse")-1,
		SOAPMAP_HEADER,
		__CWsWorkunits_WUShowScheduled_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x3D455C63,
		"http://webservices.seisint.com/WsWorkunits",
		L"http://webservices.seisint.com/WsWorkunits",
		sizeof("http://webservices.seisint.com/WsWorkunits")-1
	};

	static const _soapmapentry __CWsWorkunits_WUClusterJobSummaryXLS_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsWorkunits_WUClusterJobSummaryXLS_atlsoapheader_map = 
	{
		0xE529DDFD,
		"WUClusterJobSummaryXLS",
		L"WUClusterJobSummaryXLSResponse",
		sizeof("WUClusterJobSummaryXLS")-1,
		sizeof("WUClusterJobSummaryXLSResponse")-1,
		SOAPMAP_HEADER,
		__CWsWorkunits_WUClusterJobSummaryXLS_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x3D455C63,
		"http://webservices.seisint.com/WsWorkunits",
		L"http://webservices.seisint.com/WsWorkunits",
		sizeof("http://webservices.seisint.com/WsWorkunits")-1
	};

	static const _soapmapentry __CWsWorkunits_WUInfo_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsWorkunits_WUInfo_atlsoapheader_map = 
	{
		0xFF02F4E7,
		"WUInfo",
		L"WUInfoResponse",
		sizeof("WUInfo")-1,
		sizeof("WUInfoResponse")-1,
		SOAPMAP_HEADER,
		__CWsWorkunits_WUInfo_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x3D455C63,
		"http://webservices.seisint.com/WsWorkunits",
		L"http://webservices.seisint.com/WsWorkunits",
		sizeof("http://webservices.seisint.com/WsWorkunits")-1
	};

	static const _soapmapentry __CWsWorkunits_WUInfoDetails_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsWorkunits_WUInfoDetails_atlsoapheader_map = 
	{
		0xFF02F4E7,
		"WUInfoDetails",
		L"WUInfoResponse",
		sizeof("WUInfoDetails")-1,
		sizeof("WUInfoResponse")-1,
		SOAPMAP_HEADER,
		__CWsWorkunits_WUInfoDetails_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x3D455C63,
		"http://webservices.seisint.com/WsWorkunits",
		L"http://webservices.seisint.com/WsWorkunits",
		sizeof("http://webservices.seisint.com/WsWorkunits")-1
	};

	static const _soapmapentry __CWsWorkunits_WUGetDependancyTrees_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsWorkunits_WUGetDependancyTrees_atlsoapheader_map = 
	{
		0x8AE149F9,
		"WUGetDependancyTrees",
		L"WUGetDependancyTreesResponse",
		sizeof("WUGetDependancyTrees")-1,
		sizeof("WUGetDependancyTreesResponse")-1,
		SOAPMAP_HEADER,
		__CWsWorkunits_WUGetDependancyTrees_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x3D455C63,
		"http://webservices.seisint.com/WsWorkunits",
		L"http://webservices.seisint.com/WsWorkunits",
		sizeof("http://webservices.seisint.com/WsWorkunits")-1
	};

	static const _soapmapentry __CWsWorkunits_WUAbort_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsWorkunits_WUAbort_atlsoapheader_map = 
	{
		0x9669E673,
		"WUAbort",
		L"WUAbortResponse",
		sizeof("WUAbort")-1,
		sizeof("WUAbortResponse")-1,
		SOAPMAP_HEADER,
		__CWsWorkunits_WUAbort_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x3D455C63,
		"http://webservices.seisint.com/WsWorkunits",
		L"http://webservices.seisint.com/WsWorkunits",
		sizeof("http://webservices.seisint.com/WsWorkunits")-1
	};

	static const _soapmapentry __CWsWorkunits_WUCDebug_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsWorkunits_WUCDebug_atlsoapheader_map = 
	{
		0x7379DD22,
		"WUCDebug",
		L"WUDebugResponse",
		sizeof("WUCDebug")-1,
		sizeof("WUDebugResponse")-1,
		SOAPMAP_HEADER,
		__CWsWorkunits_WUCDebug_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x3D455C63,
		"http://webservices.seisint.com/WsWorkunits",
		L"http://webservices.seisint.com/WsWorkunits",
		sizeof("http://webservices.seisint.com/WsWorkunits")-1
	};

	static const _soapmapentry __CWsWorkunits_GVCAjaxGraph_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsWorkunits_GVCAjaxGraph_atlsoapheader_map = 
	{
		0xE2CAFF05,
		"GVCAjaxGraph",
		L"GVCAjaxGraphResponse",
		sizeof("GVCAjaxGraph")-1,
		sizeof("GVCAjaxGraphResponse")-1,
		SOAPMAP_HEADER,
		__CWsWorkunits_GVCAjaxGraph_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x3D455C63,
		"http://webservices.seisint.com/WsWorkunits",
		L"http://webservices.seisint.com/WsWorkunits",
		sizeof("http://webservices.seisint.com/WsWorkunits")-1
	};

	static const _soapmapentry __CWsWorkunits_WUJobList_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsWorkunits_WUJobList_atlsoapheader_map = 
	{
		0x303514F2,
		"WUJobList",
		L"WUJobListResponse",
		sizeof("WUJobList")-1,
		sizeof("WUJobListResponse")-1,
		SOAPMAP_HEADER,
		__CWsWorkunits_WUJobList_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x3D455C63,
		"http://webservices.seisint.com/WsWorkunits",
		L"http://webservices.seisint.com/WsWorkunits",
		sizeof("http://webservices.seisint.com/WsWorkunits")-1
	};

	static const _soapmapentry __CWsWorkunits_WUGetGraph_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsWorkunits_WUGetGraph_atlsoapheader_map = 
	{
		0xE227BDAD,
		"WUGetGraph",
		L"WUGetGraphResponse",
		sizeof("WUGetGraph")-1,
		sizeof("WUGetGraphResponse")-1,
		SOAPMAP_HEADER,
		__CWsWorkunits_WUGetGraph_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x3D455C63,
		"http://webservices.seisint.com/WsWorkunits",
		L"http://webservices.seisint.com/WsWorkunits",
		sizeof("http://webservices.seisint.com/WsWorkunits")-1
	};

	static const _soapmapentry __CWsWorkunits_WUResultSummary_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsWorkunits_WUResultSummary_atlsoapheader_map = 
	{
		0x88BFA3A8,
		"WUResultSummary",
		L"WUResultSummaryResponse",
		sizeof("WUResultSummary")-1,
		sizeof("WUResultSummaryResponse")-1,
		SOAPMAP_HEADER,
		__CWsWorkunits_WUResultSummary_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x3D455C63,
		"http://webservices.seisint.com/WsWorkunits",
		L"http://webservices.seisint.com/WsWorkunits",
		sizeof("http://webservices.seisint.com/WsWorkunits")-1
	};

	static const _soapmapentry __CWsWorkunits_WUGraphTiming_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsWorkunits_WUGraphTiming_atlsoapheader_map = 
	{
		0xFC368075,
		"WUGraphTiming",
		L"WUGraphTimingResponse",
		sizeof("WUGraphTiming")-1,
		sizeof("WUGraphTimingResponse")-1,
		SOAPMAP_HEADER,
		__CWsWorkunits_WUGraphTiming_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x3D455C63,
		"http://webservices.seisint.com/WsWorkunits",
		L"http://webservices.seisint.com/WsWorkunits",
		sizeof("http://webservices.seisint.com/WsWorkunits")-1
	};

	static const _soapmapentry __CWsWorkunits_WUClusterJobQueueXLS_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsWorkunits_WUClusterJobQueueXLS_atlsoapheader_map = 
	{
		0x19F53574,
		"WUClusterJobQueueXLS",
		L"WUClusterJobQueueXLSResponse",
		sizeof("WUClusterJobQueueXLS")-1,
		sizeof("WUClusterJobQueueXLSResponse")-1,
		SOAPMAP_HEADER,
		__CWsWorkunits_WUClusterJobQueueXLS_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x3D455C63,
		"http://webservices.seisint.com/WsWorkunits",
		L"http://webservices.seisint.com/WsWorkunits",
		sizeof("http://webservices.seisint.com/WsWorkunits")-1
	};

	static const _soapmapentry __CWsWorkunits_WUResult_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsWorkunits_WUResult_atlsoapheader_map = 
	{
		0x359D9FDA,
		"WUResult",
		L"WUResultResponse",
		sizeof("WUResult")-1,
		sizeof("WUResultResponse")-1,
		SOAPMAP_HEADER,
		__CWsWorkunits_WUResult_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x3D455C63,
		"http://webservices.seisint.com/WsWorkunits",
		L"http://webservices.seisint.com/WsWorkunits",
		sizeof("http://webservices.seisint.com/WsWorkunits")-1
	};

	static const _soapmapentry __CWsWorkunits_WUWaitCompiled_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsWorkunits_WUWaitCompiled_atlsoapheader_map = 
	{
		0x392AC450,
		"WUWaitCompiled",
		L"WUWaitResponse",
		sizeof("WUWaitCompiled")-1,
		sizeof("WUWaitResponse")-1,
		SOAPMAP_HEADER,
		__CWsWorkunits_WUWaitCompiled_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x3D455C63,
		"http://webservices.seisint.com/WsWorkunits",
		L"http://webservices.seisint.com/WsWorkunits",
		sizeof("http://webservices.seisint.com/WsWorkunits")-1
	};

	static const _soapmapentry __CWsWorkunits_WUProcessGraph_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsWorkunits_WUProcessGraph_atlsoapheader_map = 
	{
		0xCC6414EC,
		"WUProcessGraph",
		L"WUProcessGraphResponse",
		sizeof("WUProcessGraph")-1,
		sizeof("WUProcessGraphResponse")-1,
		SOAPMAP_HEADER,
		__CWsWorkunits_WUProcessGraph_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x3D455C63,
		"http://webservices.seisint.com/WsWorkunits",
		L"http://webservices.seisint.com/WsWorkunits",
		sizeof("http://webservices.seisint.com/WsWorkunits")-1
	};

	static const _soapmapentry __CWsWorkunits_WUResubmit_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsWorkunits_WUResubmit_atlsoapheader_map = 
	{
		0xFAF1D706,
		"WUResubmit",
		L"WUResubmitResponse",
		sizeof("WUResubmit")-1,
		sizeof("WUResubmitResponse")-1,
		SOAPMAP_HEADER,
		__CWsWorkunits_WUResubmit_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x3D455C63,
		"http://webservices.seisint.com/WsWorkunits",
		L"http://webservices.seisint.com/WsWorkunits",
		sizeof("http://webservices.seisint.com/WsWorkunits")-1
	};

	static const _soapmapentry __CWsWorkunits_WUClusterJobQueueLOG_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsWorkunits_WUClusterJobQueueLOG_atlsoapheader_map = 
	{
		0x6FE71DBF,
		"WUClusterJobQueueLOG",
		L"WUClusterJobQueueLOGResponse",
		sizeof("WUClusterJobQueueLOG")-1,
		sizeof("WUClusterJobQueueLOGResponse")-1,
		SOAPMAP_HEADER,
		__CWsWorkunits_WUClusterJobQueueLOG_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x3D455C63,
		"http://webservices.seisint.com/WsWorkunits",
		L"http://webservices.seisint.com/WsWorkunits",
		sizeof("http://webservices.seisint.com/WsWorkunits")-1
	};

	static const _soapmapentry __CWsWorkunits_WUGraphInfo_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsWorkunits_WUGraphInfo_atlsoapheader_map = 
	{
		0xDC1181D9,
		"WUGraphInfo",
		L"WUGraphInfoResponse",
		sizeof("WUGraphInfo")-1,
		sizeof("WUGraphInfoResponse")-1,
		SOAPMAP_HEADER,
		__CWsWorkunits_WUGraphInfo_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x3D455C63,
		"http://webservices.seisint.com/WsWorkunits",
		L"http://webservices.seisint.com/WsWorkunits",
		sizeof("http://webservices.seisint.com/WsWorkunits")-1
	};

	static const _soapmapentry __CWsWorkunits_WUGVCGraphInfo_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsWorkunits_WUGVCGraphInfo_atlsoapheader_map = 
	{
		0xEE631BB9,
		"WUGVCGraphInfo",
		L"WUGVCGraphInfoResponse",
		sizeof("WUGVCGraphInfo")-1,
		sizeof("WUGVCGraphInfoResponse")-1,
		SOAPMAP_HEADER,
		__CWsWorkunits_WUGVCGraphInfo_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x3D455C63,
		"http://webservices.seisint.com/WsWorkunits",
		L"http://webservices.seisint.com/WsWorkunits",
		sizeof("http://webservices.seisint.com/WsWorkunits")-1
	};

	static const _soapmapentry __CWsWorkunits_WUResultBin_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsWorkunits_WUResultBin_atlsoapheader_map = 
	{
		0x521313B3,
		"WUResultBin",
		L"WUResultBinResponse",
		sizeof("WUResultBin")-1,
		sizeof("WUResultBinResponse")-1,
		SOAPMAP_HEADER,
		__CWsWorkunits_WUResultBin_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x3D455C63,
		"http://webservices.seisint.com/WsWorkunits",
		L"http://webservices.seisint.com/WsWorkunits",
		sizeof("http://webservices.seisint.com/WsWorkunits")-1
	};

	static const _soapmapentry __CWsWorkunits_WUClusterJobXLS_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsWorkunits_WUClusterJobXLS_atlsoapheader_map = 
	{
		0x852296AF,
		"WUClusterJobXLS",
		L"WUClusterJobXLSResponse",
		sizeof("WUClusterJobXLS")-1,
		sizeof("WUClusterJobXLSResponse")-1,
		SOAPMAP_HEADER,
		__CWsWorkunits_WUClusterJobXLS_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x3D455C63,
		"http://webservices.seisint.com/WsWorkunits",
		L"http://webservices.seisint.com/WsWorkunits",
		sizeof("http://webservices.seisint.com/WsWorkunits")-1
	};


	static const _soapmap * __CWsWorkunits_headers[] =
	{
		&__CWsWorkunits_WUCreateAndUpdate_atlsoapheader_map,
		&__CWsWorkunits_WUSubmit_atlsoapheader_map,
		&__CWsWorkunits_WUSchedule_atlsoapheader_map,
		&__CWsWorkunits_WUPushEvent_atlsoapheader_map,
		&__CWsWorkunits_WUUpdate_atlsoapheader_map,
		&__CWsWorkunits_WUAddLocalFileToWorkunit_atlsoapheader_map,
		&__CWsWorkunits_WUExport_atlsoapheader_map,
		&__CWsWorkunits_WUQuery_atlsoapheader_map,
		&__CWsWorkunits_WUFile_atlsoapheader_map,
		&__CWsWorkunits_WUDelete_atlsoapheader_map,
		&__CWsWorkunits_WUAction_atlsoapheader_map,
		&__CWsWorkunits_WUCreate_atlsoapheader_map,
		&__CWsWorkunits_WUWaitComplete_atlsoapheader_map,
		&__CWsWorkunits_WUProtect_atlsoapheader_map,
		&__CWsWorkunits_WUCompileECL_atlsoapheader_map,
		&__CWsWorkunits_WUSyntaxCheckECL_atlsoapheader_map,
		&__CWsWorkunits_WUListLocalFileRequired_atlsoapheader_map,
		&__CWsWorkunits_WUShowScheduled_atlsoapheader_map,
		&__CWsWorkunits_WUClusterJobSummaryXLS_atlsoapheader_map,
		&__CWsWorkunits_WUInfo_atlsoapheader_map,
		&__CWsWorkunits_WUInfoDetails_atlsoapheader_map,
		&__CWsWorkunits_WUGetDependancyTrees_atlsoapheader_map,
		&__CWsWorkunits_WUAbort_atlsoapheader_map,
		&__CWsWorkunits_WUCDebug_atlsoapheader_map,
		&__CWsWorkunits_GVCAjaxGraph_atlsoapheader_map,
		&__CWsWorkunits_WUJobList_atlsoapheader_map,
		&__CWsWorkunits_WUGetGraph_atlsoapheader_map,
		&__CWsWorkunits_WUResultSummary_atlsoapheader_map,
		&__CWsWorkunits_WUGraphTiming_atlsoapheader_map,
		&__CWsWorkunits_WUClusterJobQueueXLS_atlsoapheader_map,
		&__CWsWorkunits_WUResult_atlsoapheader_map,
		&__CWsWorkunits_WUWaitCompiled_atlsoapheader_map,
		&__CWsWorkunits_WUProcessGraph_atlsoapheader_map,
		&__CWsWorkunits_WUResubmit_atlsoapheader_map,
		&__CWsWorkunits_WUClusterJobQueueLOG_atlsoapheader_map,
		&__CWsWorkunits_WUGraphInfo_atlsoapheader_map,
		&__CWsWorkunits_WUGVCGraphInfo_atlsoapheader_map,
		&__CWsWorkunits_WUResultBin_atlsoapheader_map,
		&__CWsWorkunits_WUClusterJobXLS_atlsoapheader_map,
		NULL
	};
	
	return __CWsWorkunits_headers;
}

template <typename TClient>
ATL_NOINLINE inline void * CWsWorkunitsT<TClient>::GetHeaderValue()
{
	return this;
}

template <typename TClient>
ATL_NOINLINE inline const wchar_t * CWsWorkunitsT<TClient>::GetNamespaceUri()
{
	return L"http://webservices.seisint.com/WsWorkunits";
}

template <typename TClient>
ATL_NOINLINE inline const char * CWsWorkunitsT<TClient>::GetServiceName()
{
	return NULL;
}

template <typename TClient>
ATL_NOINLINE inline const char * CWsWorkunitsT<TClient>::GetNamespaceUriA()
{
	return "http://webservices.seisint.com/WsWorkunits";
}

template <typename TClient>
ATL_NOINLINE inline HRESULT CWsWorkunitsT<TClient>::CallFunction(
	void *, 
	const wchar_t *, int,
	size_t)
{
	return E_NOTIMPL;
}

template <typename TClient>
ATL_NOINLINE inline HRESULT CWsWorkunitsT<TClient>::GetClientReader(ISAXXMLReader **ppReader)
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

} // namespace WsWorkunits

template<>
inline HRESULT AtlCleanupValue<WsWorkunits::ECLTimingData>(WsWorkunits::ECLTimingData *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Name);
	AtlCleanupValue(&pVal->GraphNum);
	AtlCleanupValue(&pVal->SubGraphNum);
	AtlCleanupValue(&pVal->GID);
	AtlCleanupValue(&pVal->Min);
	AtlCleanupValue(&pVal->MS);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsWorkunits::ECLTimingData>(WsWorkunits::ECLTimingData *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Name, pMemMgr);
	AtlCleanupValueEx(&pVal->GraphNum, pMemMgr);
	AtlCleanupValueEx(&pVal->SubGraphNum, pMemMgr);
	AtlCleanupValueEx(&pVal->GID, pMemMgr);
	AtlCleanupValueEx(&pVal->Min, pMemMgr);
	AtlCleanupValueEx(&pVal->MS, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsWorkunits::LogicalFileUpload>(WsWorkunits::LogicalFileUpload *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Type);
	AtlCleanupValue(&pVal->Source);
	AtlCleanupValue(&pVal->Destination);
	AtlCleanupValue(&pVal->EventTag);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsWorkunits::LogicalFileUpload>(WsWorkunits::LogicalFileUpload *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Type, pMemMgr);
	AtlCleanupValueEx(&pVal->Source, pMemMgr);
	AtlCleanupValueEx(&pVal->Destination, pMemMgr);
	AtlCleanupValueEx(&pVal->EventTag, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsWorkunits::ECLSchemaItem>(WsWorkunits::ECLSchemaItem *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->ColumnName);
	AtlCleanupValue(&pVal->ColumnType);
	AtlCleanupValue(&pVal->ColumnTypeCode);
	AtlCleanupValue(&pVal->isConditional);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsWorkunits::ECLSchemaItem>(WsWorkunits::ECLSchemaItem *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->ColumnName, pMemMgr);
	AtlCleanupValueEx(&pVal->ColumnType, pMemMgr);
	AtlCleanupValueEx(&pVal->ColumnTypeCode, pMemMgr);
	AtlCleanupValueEx(&pVal->isConditional, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsWorkunits::ECLWorkflow>(WsWorkunits::ECLWorkflow *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->WFID);
	AtlCleanupValue(&pVal->EventName);
	AtlCleanupValue(&pVal->EventText);
	AtlCleanupValue(&pVal->Count);
	AtlCleanupValue(&pVal->CountRemaining);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsWorkunits::ECLWorkflow>(WsWorkunits::ECLWorkflow *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->WFID, pMemMgr);
	AtlCleanupValueEx(&pVal->EventName, pMemMgr);
	AtlCleanupValueEx(&pVal->EventText, pMemMgr);
	AtlCleanupValueEx(&pVal->Count, pMemMgr);
	AtlCleanupValueEx(&pVal->CountRemaining, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsWorkunits::ArrayOfECLSchemaItem>(WsWorkunits::ArrayOfECLSchemaItem *pVal)
{
	pVal;
	AtlCleanupArray(pVal->_ECLSchemaItem, pVal->___ECLSchemaItem_nSizeIs);	
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsWorkunits::ArrayOfECLSchemaItem>(WsWorkunits::ArrayOfECLSchemaItem *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	if (pVal->_ECLSchemaItem != NULL)
	{
		AtlCleanupArrayEx(pVal->_ECLSchemaItem, pVal->___ECLSchemaItem_nSizeIs, pMemMgr);		
		pMemMgr->Free(pVal->_ECLSchemaItem);
	}
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsWorkunits::ECLResult>(WsWorkunits::ECLResult *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Name);
	AtlCleanupValue(&pVal->Sequence);
	AtlCleanupValue(&pVal->Value);
	AtlCleanupValue(&pVal->Link);
	AtlCleanupValue(&pVal->FileName);
	AtlCleanupValue(&pVal->IsSupplied);
	AtlCleanupValue(&pVal->ShowFileContent);
	AtlCleanupValue(&pVal->Total);
	AtlCleanupValue(&pVal->ECLSchemas);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsWorkunits::ECLResult>(WsWorkunits::ECLResult *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Name, pMemMgr);
	AtlCleanupValueEx(&pVal->Sequence, pMemMgr);
	AtlCleanupValueEx(&pVal->Value, pMemMgr);
	AtlCleanupValueEx(&pVal->Link, pMemMgr);
	AtlCleanupValueEx(&pVal->FileName, pMemMgr);
	AtlCleanupValueEx(&pVal->IsSupplied, pMemMgr);
	AtlCleanupValueEx(&pVal->ShowFileContent, pMemMgr);
	AtlCleanupValueEx(&pVal->Total, pMemMgr);
	AtlCleanupValueEx(&pVal->ECLSchemas, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsWorkunits::ECLSourceFile>(WsWorkunits::ECLSourceFile *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->FileCluster);
	AtlCleanupValue(&pVal->Name);
	AtlCleanupValue(&pVal->IsSuperFile);
	AtlCleanupValue(&pVal->Count);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsWorkunits::ECLSourceFile>(WsWorkunits::ECLSourceFile *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->FileCluster, pMemMgr);
	AtlCleanupValueEx(&pVal->Name, pMemMgr);
	AtlCleanupValueEx(&pVal->IsSuperFile, pMemMgr);
	AtlCleanupValueEx(&pVal->Count, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsWorkunits::ServerInfo>(WsWorkunits::ServerInfo *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Name);
	AtlCleanupValue(&pVal->NetAddress);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsWorkunits::ServerInfo>(WsWorkunits::ServerInfo *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Name, pMemMgr);
	AtlCleanupValueEx(&pVal->NetAddress, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsWorkunits::ECLGraph>(WsWorkunits::ECLGraph *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Name);
	AtlCleanupValue(&pVal->Label);
	AtlCleanupValue(&pVal->Type);
	AtlCleanupValue(&pVal->Running);
	AtlCleanupValue(&pVal->Complete);
	AtlCleanupValue(&pVal->Failed);
	AtlCleanupValue(&pVal->RunningId);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsWorkunits::ECLGraph>(WsWorkunits::ECLGraph *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Name, pMemMgr);
	AtlCleanupValueEx(&pVal->Label, pMemMgr);
	AtlCleanupValueEx(&pVal->Type, pMemMgr);
	AtlCleanupValueEx(&pVal->Running, pMemMgr);
	AtlCleanupValueEx(&pVal->Complete, pMemMgr);
	AtlCleanupValueEx(&pVal->Failed, pMemMgr);
	AtlCleanupValueEx(&pVal->RunningId, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsWorkunits::EspException>(WsWorkunits::EspException *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Code);
	AtlCleanupValue(&pVal->Audience);
	AtlCleanupValue(&pVal->Source);
	AtlCleanupValue(&pVal->Message);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsWorkunits::EspException>(WsWorkunits::EspException *pVal, IAtlMemMgr *pMemMgr)
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
inline HRESULT AtlCleanupValue<WsWorkunits::ArrayOfEspException>(WsWorkunits::ArrayOfEspException *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Source);
	AtlCleanupArray(pVal->Exception, pVal->__Exception_nSizeIs);	
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsWorkunits::ArrayOfEspException>(WsWorkunits::ArrayOfEspException *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Source, pMemMgr);
	if (pVal->Exception != NULL)
	{
		AtlCleanupArrayEx(pVal->Exception, pVal->__Exception_nSizeIs, pMemMgr);		
		pMemMgr->Free(pVal->Exception);
	}
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsWorkunits::WUECLAttribute>(WsWorkunits::WUECLAttribute *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->ModuleName);
	AtlCleanupValue(&pVal->AttributeName);
	AtlCleanupValue(&pVal->IsLocked);
	AtlCleanupValue(&pVal->IsCheckedOut);
	AtlCleanupValue(&pVal->IsSandbox);
	AtlCleanupValue(&pVal->IsOrphaned);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsWorkunits::WUECLAttribute>(WsWorkunits::WUECLAttribute *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->ModuleName, pMemMgr);
	AtlCleanupValueEx(&pVal->AttributeName, pMemMgr);
	AtlCleanupValueEx(&pVal->IsLocked, pMemMgr);
	AtlCleanupValueEx(&pVal->IsCheckedOut, pMemMgr);
	AtlCleanupValueEx(&pVal->IsSandbox, pMemMgr);
	AtlCleanupValueEx(&pVal->IsOrphaned, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsWorkunits::ECLException>(WsWorkunits::ECLException *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Source);
	AtlCleanupValue(&pVal->Severity);
	AtlCleanupValue(&pVal->Code);
	AtlCleanupValue(&pVal->Message);
	AtlCleanupValue(&pVal->FileName);
	AtlCleanupValue(&pVal->LineNo);
	AtlCleanupValue(&pVal->Column);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsWorkunits::ECLException>(WsWorkunits::ECLException *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Source, pMemMgr);
	AtlCleanupValueEx(&pVal->Severity, pMemMgr);
	AtlCleanupValueEx(&pVal->Code, pMemMgr);
	AtlCleanupValueEx(&pVal->Message, pMemMgr);
	AtlCleanupValueEx(&pVal->FileName, pMemMgr);
	AtlCleanupValueEx(&pVal->LineNo, pMemMgr);
	AtlCleanupValueEx(&pVal->Column, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsWorkunits::DebugValue>(WsWorkunits::DebugValue *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Name);
	AtlCleanupValue(&pVal->Value);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsWorkunits::DebugValue>(WsWorkunits::DebugValue *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Name, pMemMgr);
	AtlCleanupValueEx(&pVal->Value, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsWorkunits::WUActionResult>(WsWorkunits::WUActionResult *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Wuid);
	AtlCleanupValue(&pVal->Action);
	AtlCleanupValue(&pVal->Result);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsWorkunits::WUActionResult>(WsWorkunits::WUActionResult *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Wuid, pMemMgr);
	AtlCleanupValueEx(&pVal->Action, pMemMgr);
	AtlCleanupValueEx(&pVal->Result, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsWorkunits::ECLQuery>(WsWorkunits::ECLQuery *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Text);
	AtlCleanupValue(&pVal->Cpp);
	AtlCleanupValue(&pVal->ResTxt);
	AtlCleanupValue(&pVal->Dll);
	AtlCleanupValue(&pVal->ThorLog);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsWorkunits::ECLQuery>(WsWorkunits::ECLQuery *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Text, pMemMgr);
	AtlCleanupValueEx(&pVal->Cpp, pMemMgr);
	AtlCleanupValueEx(&pVal->ResTxt, pMemMgr);
	AtlCleanupValueEx(&pVal->Dll, pMemMgr);
	AtlCleanupValueEx(&pVal->ThorLog, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsWorkunits::ECLHelpFile>(WsWorkunits::ECLHelpFile *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Name);
	AtlCleanupValue(&pVal->Type);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsWorkunits::ECLHelpFile>(WsWorkunits::ECLHelpFile *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Name, pMemMgr);
	AtlCleanupValueEx(&pVal->Type, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsWorkunits::ECLTimer>(WsWorkunits::ECLTimer *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Name);
	AtlCleanupValue(&pVal->Value);
	AtlCleanupValue(&pVal->count);
	AtlCleanupValue(&pVal->GraphName);
	AtlCleanupValue(&pVal->SubGraphId);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsWorkunits::ECLTimer>(WsWorkunits::ECLTimer *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Name, pMemMgr);
	AtlCleanupValueEx(&pVal->Value, pMemMgr);
	AtlCleanupValueEx(&pVal->count, pMemMgr);
	AtlCleanupValueEx(&pVal->GraphName, pMemMgr);
	AtlCleanupValueEx(&pVal->SubGraphId, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsWorkunits::ApplicationValue>(WsWorkunits::ApplicationValue *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Application);
	AtlCleanupValue(&pVal->Name);
	AtlCleanupValue(&pVal->Value);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsWorkunits::ApplicationValue>(WsWorkunits::ApplicationValue *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Application, pMemMgr);
	AtlCleanupValueEx(&pVal->Name, pMemMgr);
	AtlCleanupValueEx(&pVal->Value, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsWorkunits::ECLWorkunit>(WsWorkunits::ECLWorkunit *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Wuid);
	AtlCleanupValue(&pVal->Owner);
	AtlCleanupValue(&pVal->Cluster);
	AtlCleanupValue(&pVal->RoxieCluster);
	AtlCleanupValue(&pVal->Jobname);
	AtlCleanupValue(&pVal->Queue);
	AtlCleanupValue(&pVal->StateID);
	AtlCleanupValue(&pVal->State);
	AtlCleanupValue(&pVal->StateEx);
	AtlCleanupValue(&pVal->Description);
	AtlCleanupValue(&pVal->AddDrilldownFields);
	AtlCleanupValue(&pVal->Protected);
	AtlCleanupValue(&pVal->Active);
	AtlCleanupValue(&pVal->Action);
	AtlCleanupValue(&pVal->DateTimeScheduled);
	AtlCleanupValue(&pVal->PriorityClass);
	AtlCleanupValue(&pVal->PriorityLevel);
	AtlCleanupValue(&pVal->Scope);
	AtlCleanupValue(&pVal->Snapshot);
	AtlCleanupValue(&pVal->ResultLimit);
	AtlCleanupValue(&pVal->Archived);
	AtlCleanupValue(&pVal->EventSchedule);
	AtlCleanupValue(&pVal->HaveSubGraphTimings);
	AtlCleanupValue(&pVal->Query);
	AtlCleanupArray(pVal->Helpers, pVal->__Helpers_nSizeIs);	
	AtlCleanupArray(pVal->Exceptions, pVal->__Exceptions_nSizeIs);	
	AtlCleanupArray(pVal->Graphs, pVal->__Graphs_nSizeIs);	
	AtlCleanupArray(pVal->SourceFiles, pVal->__SourceFiles_nSizeIs);	
	AtlCleanupArray(pVal->Results, pVal->__Results_nSizeIs);	
	AtlCleanupArray(pVal->Variables, pVal->__Variables_nSizeIs);	
	AtlCleanupArray(pVal->Timers, pVal->__Timers_nSizeIs);	
	AtlCleanupArray(pVal->DebugValues, pVal->__DebugValues_nSizeIs);	
	AtlCleanupArray(pVal->ApplicationValues, pVal->__ApplicationValues_nSizeIs);	
	AtlCleanupArray(pVal->Workflows, pVal->__Workflows_nSizeIs);	
	AtlCleanupArray(pVal->TimingData, pVal->__TimingData_nSizeIs);	
	AtlCleanupArray(pVal->AllowedClusters, pVal->__AllowedClusters_nSizeIs);	
	AtlCleanupValue(&pVal->ErrorCount);
	AtlCleanupValue(&pVal->WarningCount);
	AtlCleanupValue(&pVal->InfoCount);
	AtlCleanupValue(&pVal->GraphCount);
	AtlCleanupValue(&pVal->SourceFileCount);
	AtlCleanupValue(&pVal->ResultCount);
	AtlCleanupValue(&pVal->VariableCount);
	AtlCleanupValue(&pVal->TimerCount);
	AtlCleanupValue(&pVal->HasDebugValue);
	AtlCleanupValue(&pVal->ApplicationValueCount);
	AtlCleanupValue(&pVal->XmlParams);
	AtlCleanupValue(&pVal->AccessFlag);
	AtlCleanupValue(&pVal->ClusterFlag);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsWorkunits::ECLWorkunit>(WsWorkunits::ECLWorkunit *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Wuid, pMemMgr);
	AtlCleanupValueEx(&pVal->Owner, pMemMgr);
	AtlCleanupValueEx(&pVal->Cluster, pMemMgr);
	AtlCleanupValueEx(&pVal->RoxieCluster, pMemMgr);
	AtlCleanupValueEx(&pVal->Jobname, pMemMgr);
	AtlCleanupValueEx(&pVal->Queue, pMemMgr);
	AtlCleanupValueEx(&pVal->StateID, pMemMgr);
	AtlCleanupValueEx(&pVal->State, pMemMgr);
	AtlCleanupValueEx(&pVal->StateEx, pMemMgr);
	AtlCleanupValueEx(&pVal->Description, pMemMgr);
	AtlCleanupValueEx(&pVal->AddDrilldownFields, pMemMgr);
	AtlCleanupValueEx(&pVal->Protected, pMemMgr);
	AtlCleanupValueEx(&pVal->Active, pMemMgr);
	AtlCleanupValueEx(&pVal->Action, pMemMgr);
	AtlCleanupValueEx(&pVal->DateTimeScheduled, pMemMgr);
	AtlCleanupValueEx(&pVal->PriorityClass, pMemMgr);
	AtlCleanupValueEx(&pVal->PriorityLevel, pMemMgr);
	AtlCleanupValueEx(&pVal->Scope, pMemMgr);
	AtlCleanupValueEx(&pVal->Snapshot, pMemMgr);
	AtlCleanupValueEx(&pVal->ResultLimit, pMemMgr);
	AtlCleanupValueEx(&pVal->Archived, pMemMgr);
	AtlCleanupValueEx(&pVal->EventSchedule, pMemMgr);
	AtlCleanupValueEx(&pVal->HaveSubGraphTimings, pMemMgr);
	AtlCleanupValueEx(&pVal->Query, pMemMgr);
	if (pVal->Helpers != NULL)
	{
		AtlCleanupArrayEx(pVal->Helpers, pVal->__Helpers_nSizeIs, pMemMgr);		
		pMemMgr->Free(pVal->Helpers);
	}
	if (pVal->Exceptions != NULL)
	{
		AtlCleanupArrayEx(pVal->Exceptions, pVal->__Exceptions_nSizeIs, pMemMgr);		
		pMemMgr->Free(pVal->Exceptions);
	}
	if (pVal->Graphs != NULL)
	{
		AtlCleanupArrayEx(pVal->Graphs, pVal->__Graphs_nSizeIs, pMemMgr);		
		pMemMgr->Free(pVal->Graphs);
	}
	if (pVal->SourceFiles != NULL)
	{
		AtlCleanupArrayEx(pVal->SourceFiles, pVal->__SourceFiles_nSizeIs, pMemMgr);		
		pMemMgr->Free(pVal->SourceFiles);
	}
	if (pVal->Results != NULL)
	{
		AtlCleanupArrayEx(pVal->Results, pVal->__Results_nSizeIs, pMemMgr);		
		pMemMgr->Free(pVal->Results);
	}
	if (pVal->Variables != NULL)
	{
		AtlCleanupArrayEx(pVal->Variables, pVal->__Variables_nSizeIs, pMemMgr);		
		pMemMgr->Free(pVal->Variables);
	}
	if (pVal->Timers != NULL)
	{
		AtlCleanupArrayEx(pVal->Timers, pVal->__Timers_nSizeIs, pMemMgr);		
		pMemMgr->Free(pVal->Timers);
	}
	if (pVal->DebugValues != NULL)
	{
		AtlCleanupArrayEx(pVal->DebugValues, pVal->__DebugValues_nSizeIs, pMemMgr);		
		pMemMgr->Free(pVal->DebugValues);
	}
	if (pVal->ApplicationValues != NULL)
	{
		AtlCleanupArrayEx(pVal->ApplicationValues, pVal->__ApplicationValues_nSizeIs, pMemMgr);		
		pMemMgr->Free(pVal->ApplicationValues);
	}
	if (pVal->Workflows != NULL)
	{
		AtlCleanupArrayEx(pVal->Workflows, pVal->__Workflows_nSizeIs, pMemMgr);		
		pMemMgr->Free(pVal->Workflows);
	}
	if (pVal->TimingData != NULL)
	{
		AtlCleanupArrayEx(pVal->TimingData, pVal->__TimingData_nSizeIs, pMemMgr);		
		pMemMgr->Free(pVal->TimingData);
	}
	if (pVal->AllowedClusters != NULL)
	{
		AtlCleanupArrayEx(pVal->AllowedClusters, pVal->__AllowedClusters_nSizeIs, pMemMgr);		
		pMemMgr->Free(pVal->AllowedClusters);
	}
	AtlCleanupValueEx(&pVal->ErrorCount, pMemMgr);
	AtlCleanupValueEx(&pVal->WarningCount, pMemMgr);
	AtlCleanupValueEx(&pVal->InfoCount, pMemMgr);
	AtlCleanupValueEx(&pVal->GraphCount, pMemMgr);
	AtlCleanupValueEx(&pVal->SourceFileCount, pMemMgr);
	AtlCleanupValueEx(&pVal->ResultCount, pMemMgr);
	AtlCleanupValueEx(&pVal->VariableCount, pMemMgr);
	AtlCleanupValueEx(&pVal->TimerCount, pMemMgr);
	AtlCleanupValueEx(&pVal->HasDebugValue, pMemMgr);
	AtlCleanupValueEx(&pVal->ApplicationValueCount, pMemMgr);
	AtlCleanupValueEx(&pVal->XmlParams, pMemMgr);
	AtlCleanupValueEx(&pVal->AccessFlag, pMemMgr);
	AtlCleanupValueEx(&pVal->ClusterFlag, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsWorkunits::ECLGraphEx>(WsWorkunits::ECLGraphEx *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Name);
	AtlCleanupValue(&pVal->Label);
	AtlCleanupValue(&pVal->Type);
	AtlCleanupValue(&pVal->Graph);
	AtlCleanupValue(&pVal->Running);
	AtlCleanupValue(&pVal->RunningId);
	AtlCleanupValue(&pVal->Complete);
	AtlCleanupValue(&pVal->Failed);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsWorkunits::ECLGraphEx>(WsWorkunits::ECLGraphEx *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Name, pMemMgr);
	AtlCleanupValueEx(&pVal->Label, pMemMgr);
	AtlCleanupValueEx(&pVal->Type, pMemMgr);
	AtlCleanupValueEx(&pVal->Graph, pMemMgr);
	AtlCleanupValueEx(&pVal->Running, pMemMgr);
	AtlCleanupValueEx(&pVal->RunningId, pMemMgr);
	AtlCleanupValueEx(&pVal->Complete, pMemMgr);
	AtlCleanupValueEx(&pVal->Failed, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsWorkunits::ECLJob>(WsWorkunits::ECLJob *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Wuid);
	AtlCleanupValue(&pVal->Graph);
	AtlCleanupValue(&pVal->State);
	AtlCleanupValue(&pVal->StartedDate);
	AtlCleanupValue(&pVal->FinishedDate);
	AtlCleanupValue(&pVal->Cluster);
	AtlCleanupValue(&pVal->GraphNum);
	AtlCleanupValue(&pVal->SubGraphNum);
	AtlCleanupValue(&pVal->NumOfRuns);
	AtlCleanupValue(&pVal->Duration);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsWorkunits::ECLJob>(WsWorkunits::ECLJob *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Wuid, pMemMgr);
	AtlCleanupValueEx(&pVal->Graph, pMemMgr);
	AtlCleanupValueEx(&pVal->State, pMemMgr);
	AtlCleanupValueEx(&pVal->StartedDate, pMemMgr);
	AtlCleanupValueEx(&pVal->FinishedDate, pMemMgr);
	AtlCleanupValueEx(&pVal->Cluster, pMemMgr);
	AtlCleanupValueEx(&pVal->GraphNum, pMemMgr);
	AtlCleanupValueEx(&pVal->SubGraphNum, pMemMgr);
	AtlCleanupValueEx(&pVal->NumOfRuns, pMemMgr);
	AtlCleanupValueEx(&pVal->Duration, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsWorkunits::ScheduledWU>(WsWorkunits::ScheduledWU *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Wuid);
	AtlCleanupValue(&pVal->EclServer);
	AtlCleanupValue(&pVal->EventName);
	AtlCleanupValue(&pVal->EventText);
	AtlCleanupValue(&pVal->JobName);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsWorkunits::ScheduledWU>(WsWorkunits::ScheduledWU *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Wuid, pMemMgr);
	AtlCleanupValueEx(&pVal->EclServer, pMemMgr);
	AtlCleanupValueEx(&pVal->EventName, pMemMgr);
	AtlCleanupValueEx(&pVal->EventText, pMemMgr);
	AtlCleanupValueEx(&pVal->JobName, pMemMgr);
	return S_OK;
}
