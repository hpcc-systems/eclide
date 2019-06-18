//
// sproxy.exe generated file
// do not modify this file
//
// Created: 10/28/2009@09:13:12
//

#pragma once


#if !defined(_WIN32_WINDOWS) && !defined(_WIN32_WINNT) && !defined(_WIN32_WCE)
#pragma message("warning: defining _WIN32_WINDOWS = 0x0410")
#define _WIN32_WINDOWS 0x0410
#endif

#include <atlsoap.h>

namespace WsSMC
{

struct HoleCluster
{
	BSTR ClusterName;
	BSTR DataModel;
};

struct ThorCluster
{
	BSTR ClusterName;
	BSTR QueueName;
	BSTR QueueStatus;
	int DoCommand;
	int QueueAvailable;
	int JobsRunning;
	int JobsInQueue;
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

struct ActiveWorkunit
{
	BSTR Wuid;
	BSTR State;
	int StateID;
	BSTR Owner;
	BSTR Jobname;
	BSTR Server;
	BSTR Instance;
	BSTR Priority;
	BSTR Extra;
	BSTR GraphName;
	BSTR Duration;
	BSTR GID;
	BSTR QueueName;
	int MemoryBlocked;
};

struct DFUJob
{
	BSTR TimeStarted;
	int Done;
	int Total;
	BSTR Command;
};

template <typename TClient = CSoapSocketClientT<> >
class CWsSMCT : 
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

	CWsSMCT(ISAXXMLReader *pReader = NULL)
		:TClient(_T("http://10.173.88.201:8010/WsSMC\?ver_=1.05"))
	{
		SetClient(true);
		SetReader(pReader);
	}
	
	~CWsSMCT()
	{
		Uninitialize();
	}
	
	void Uninitialize()
	{
		UninitializeSOAP();
	}	

	HRESULT MoveJobUp(
		BSTR Cluster, 
		BSTR Wuid, 
		ArrayOfEspException* Exceptions
	);

	HRESULT MoveJobBack(
		BSTR Cluster, 
		BSTR Wuid, 
		ArrayOfEspException* Exceptions
	);

	HRESULT Activity(
		ArrayOfEspException* Exceptions, 
		BSTR* Build, 
		ThorCluster** ThorClusters, int* ThorClusters_nSizeIs, 
		HoleCluster** HoleClusters, int* HoleClusters_nSizeIs, 
		DFUJob** DFUJobs, int* DFUJobs_nSizeIs, 
		ActiveWorkunit** Running, int* Running_nSizeIs
	);

	HRESULT PauseQueue(
		BSTR Cluster, 
		ArrayOfEspException* Exceptions
	);

	HRESULT SetJobPriority(
		BSTR Cluster, 
		BSTR Wuid, 
		BSTR Priority, 
		ArrayOfEspException* Exceptions
	);

	HRESULT MoveJobFront(
		BSTR Cluster, 
		BSTR Wuid, 
		ArrayOfEspException* Exceptions
	);

	HRESULT StopQueue(
		BSTR Cluster, 
		ArrayOfEspException* Exceptions
	);

	HRESULT GetThorQueueAvailability(
		ArrayOfEspException* Exceptions, 
		ThorCluster** ThorClusters, int* ThorClusters_nSizeIs
	);

	HRESULT ResumeQueue(
		BSTR Cluster, 
		ArrayOfEspException* Exceptions
	);

	HRESULT Index(
		ArrayOfEspException* Exceptions
	);

	HRESULT MoveJobDown(
		BSTR Cluster, 
		BSTR Wuid, 
		ArrayOfEspException* Exceptions
	);

	HRESULT RemoveJob(
		BSTR Cluster, 
		BSTR Wuid, 
		ArrayOfEspException* Exceptions
	);

	HRESULT ClearQueue(
		BSTR Cluster, 
		ArrayOfEspException* Exceptions
	);
};

typedef CWsSMCT<> CWsSMC;

__if_not_exists(__HoleCluster_entries)
{
extern __declspec(selectany) const _soapmapentry __HoleCluster_entries[] =
{
	{ 
		0xCF62D983, 
		"ClusterName", 
		L"ClusterName", 
		sizeof("ClusterName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(HoleCluster, ClusterName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x3F578DAB, 
		"DataModel", 
		L"DataModel", 
		sizeof("DataModel")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(HoleCluster, DataModel),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __HoleCluster_map =
{
	0xC711D8AA,
	"HoleCluster",
	L"HoleCluster",
	sizeof("HoleCluster")-1,
	sizeof("HoleCluster")-1,
	SOAPMAP_STRUCT,
	__HoleCluster_entries,
	sizeof(HoleCluster),
	2,
	-1,
	SOAPFLAG_NONE,
	0x18AC5110,
	"http://webservices.seisint.com/WsSMC",
	L"http://webservices.seisint.com/WsSMC",
	sizeof("http://webservices.seisint.com/WsSMC")-1
};
}

__if_not_exists(__ThorCluster_entries)
{
extern __declspec(selectany) const _soapmapentry __ThorCluster_entries[] =
{
	{ 
		0xCF62D983, 
		"ClusterName", 
		L"ClusterName", 
		sizeof("ClusterName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ThorCluster, ClusterName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x4C6BB166, 
		"QueueName", 
		L"QueueName", 
		sizeof("QueueName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ThorCluster, QueueName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x2318FA69, 
		"QueueStatus", 
		L"QueueStatus", 
		sizeof("QueueStatus")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ThorCluster, QueueStatus),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x91F5B592, 
		"DoCommand", 
		L"DoCommand", 
		sizeof("DoCommand")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ThorCluster, DoCommand),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x82D83A66, 
		"QueueAvailable", 
		L"QueueAvailable", 
		sizeof("QueueAvailable")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ThorCluster, QueueAvailable),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xC7AEE34F, 
		"JobsRunning", 
		L"JobsRunning", 
		sizeof("JobsRunning")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ThorCluster, JobsRunning),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0085A40A, 
		"JobsInQueue", 
		L"JobsInQueue", 
		sizeof("JobsInQueue")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ThorCluster, JobsInQueue),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __ThorCluster_map =
{
	0x47ED823F,
	"ThorCluster",
	L"ThorCluster",
	sizeof("ThorCluster")-1,
	sizeof("ThorCluster")-1,
	SOAPMAP_STRUCT,
	__ThorCluster_entries,
	sizeof(ThorCluster),
	7,
	-1,
	SOAPFLAG_NONE,
	0x18AC5110,
	"http://webservices.seisint.com/WsSMC",
	L"http://webservices.seisint.com/WsSMC",
	sizeof("http://webservices.seisint.com/WsSMC")-1
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
	0x18AC5110,
	"http://webservices.seisint.com/WsSMC",
	L"http://webservices.seisint.com/WsSMC",
	sizeof("http://webservices.seisint.com/WsSMC")-1
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
	0x18AC5110,
	"http://webservices.seisint.com/WsSMC",
	L"http://webservices.seisint.com/WsSMC",
	sizeof("http://webservices.seisint.com/WsSMC")-1
};
}

__if_not_exists(__ActiveWorkunit_entries)
{
extern __declspec(selectany) const _soapmapentry __ActiveWorkunit_entries[] =
{
	{ 
		0x0031B499, 
		"Wuid", 
		L"Wuid", 
		sizeof("Wuid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ActiveWorkunit, Wuid),
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
		offsetof(ActiveWorkunit, State),
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
		offsetof(ActiveWorkunit, StateID),
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
		offsetof(ActiveWorkunit, Owner),
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
		offsetof(ActiveWorkunit, Jobname),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xC9005DB7, 
		"Server", 
		L"Server", 
		sizeof("Server")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ActiveWorkunit, Server),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x86F73BB5, 
		"Instance", 
		L"Instance", 
		sizeof("Instance")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ActiveWorkunit, Instance),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x19082F82, 
		"Priority", 
		L"Priority", 
		sizeof("Priority")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ActiveWorkunit, Priority),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x052464C4, 
		"Extra", 
		L"Extra", 
		sizeof("Extra")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ActiveWorkunit, Extra),
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
		offsetof(ActiveWorkunit, GraphName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x00E319E6, 
		"Duration", 
		L"Duration", 
		sizeof("Duration")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ActiveWorkunit, Duration),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x000137B4, 
		"GID", 
		L"GID", 
		sizeof("GID")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ActiveWorkunit, GID),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x4C6BB166, 
		"QueueName", 
		L"QueueName", 
		sizeof("QueueName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ActiveWorkunit, QueueName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x1FB1498D, 
		"MemoryBlocked", 
		L"MemoryBlocked", 
		sizeof("MemoryBlocked")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ActiveWorkunit, MemoryBlocked),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __ActiveWorkunit_map =
{
	0x2CF180FF,
	"ActiveWorkunit",
	L"ActiveWorkunit",
	sizeof("ActiveWorkunit")-1,
	sizeof("ActiveWorkunit")-1,
	SOAPMAP_STRUCT,
	__ActiveWorkunit_entries,
	sizeof(ActiveWorkunit),
	14,
	-1,
	SOAPFLAG_NONE,
	0x18AC5110,
	"http://webservices.seisint.com/WsSMC",
	L"http://webservices.seisint.com/WsSMC",
	sizeof("http://webservices.seisint.com/WsSMC")-1
};
}

__if_not_exists(__DFUJob_entries)
{
extern __declspec(selectany) const _soapmapentry __DFUJob_entries[] =
{
	{ 
		0x4489FC86, 
		"TimeStarted", 
		L"TimeStarted", 
		sizeof("TimeStarted")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUJob, TimeStarted),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x00273086, 
		"Done", 
		L"Done", 
		sizeof("Done")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUJob, Done),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x062EE2C4, 
		"Total", 
		L"Total", 
		sizeof("Total")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUJob, Total),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x305C63BF, 
		"Command", 
		L"Command", 
		sizeof("Command")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUJob, Command),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __DFUJob_map =
{
	0xA3C163BA,
	"DFUJob",
	L"DFUJob",
	sizeof("DFUJob")-1,
	sizeof("DFUJob")-1,
	SOAPMAP_STRUCT,
	__DFUJob_entries,
	sizeof(DFUJob),
	4,
	-1,
	SOAPFLAG_NONE,
	0x18AC5110,
	"http://webservices.seisint.com/WsSMC",
	L"http://webservices.seisint.com/WsSMC",
	sizeof("http://webservices.seisint.com/WsSMC")-1
};
}

struct __CWsSMC_MoveJobUp_struct
{
	BSTR Cluster;
	BSTR Wuid;
	ArrayOfEspException Exceptions;
};

extern __declspec(selectany) const _soapmapentry __CWsSMC_MoveJobUp_entries[] =
{

	{
		0x29F147E2, 
		"Cluster", 
		L"Cluster", 
		sizeof("Cluster")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsSMC_MoveJobUp_struct, Cluster),
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
		offsetof(__CWsSMC_MoveJobUp_struct, Wuid),
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
		offsetof(__CWsSMC_MoveJobUp_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsSMC_MoveJobUp_map =
{
	0xEA59720D,
	"MoveJobUp",
	L"SMCJobResponse",
	sizeof("MoveJobUp")-1,
	sizeof("SMCJobResponse")-1,
	SOAPMAP_FUNC,
	__CWsSMC_MoveJobUp_entries,
	sizeof(__CWsSMC_MoveJobUp_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x18AC5110,
	"http://webservices.seisint.com/WsSMC",
	L"http://webservices.seisint.com/WsSMC",
	sizeof("http://webservices.seisint.com/WsSMC")-1
};


struct __CWsSMC_MoveJobBack_struct
{
	BSTR Cluster;
	BSTR Wuid;
	ArrayOfEspException Exceptions;
};

extern __declspec(selectany) const _soapmapentry __CWsSMC_MoveJobBack_entries[] =
{

	{
		0x29F147E2, 
		"Cluster", 
		L"Cluster", 
		sizeof("Cluster")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsSMC_MoveJobBack_struct, Cluster),
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
		offsetof(__CWsSMC_MoveJobBack_struct, Wuid),
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
		offsetof(__CWsSMC_MoveJobBack_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsSMC_MoveJobBack_map =
{
	0xEA59720D,
	"MoveJobBack",
	L"SMCJobResponse",
	sizeof("MoveJobBack")-1,
	sizeof("SMCJobResponse")-1,
	SOAPMAP_FUNC,
	__CWsSMC_MoveJobBack_entries,
	sizeof(__CWsSMC_MoveJobBack_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x18AC5110,
	"http://webservices.seisint.com/WsSMC",
	L"http://webservices.seisint.com/WsSMC",
	sizeof("http://webservices.seisint.com/WsSMC")-1
};


struct __CWsSMC_Activity_struct
{
	ArrayOfEspException Exceptions;
	BSTR Build;
	ThorCluster *ThorClusters;
	int __ThorClusters_nSizeIs;
	HoleCluster *HoleClusters;
	int __HoleClusters_nSizeIs;
	DFUJob *DFUJobs;
	int __DFUJobs_nSizeIs;
	ActiveWorkunit *Running;
	int __Running_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CWsSMC_Activity_entries[] =
{

	{
		0x7B0DBD02, 
		"Exceptions", 
		L"Exceptions", 
		sizeof("Exceptions")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsSMC_Activity_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0x04EC4690, 
		"Build", 
		L"Build", 
		sizeof("Build")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsSMC_Activity_struct, Build),
		NULL,
		NULL,
		-1,
	},
	{
		0x459DCA92, 
		"ThorClusters", 
		L"ThorClusters", 
		sizeof("ThorClusters")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsSMC_Activity_struct, ThorClusters),
		NULL,
		&__ThorCluster_map,
		2+1,
	},
	{
		0x459DCA92,
		"__ThorClusters_nSizeIs",
		L"__ThorClusters_nSizeIs",
		sizeof("__ThorClusters_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsSMC_Activity_struct, __ThorClusters_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xA94CEE5D, 
		"HoleClusters", 
		L"HoleClusters", 
		sizeof("HoleClusters")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsSMC_Activity_struct, HoleClusters),
		NULL,
		&__HoleCluster_map,
		4+1,
	},
	{
		0xA94CEE5D,
		"__HoleClusters_nSizeIs",
		L"__HoleClusters_nSizeIs",
		sizeof("__HoleClusters_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsSMC_Activity_struct, __HoleClusters_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x1BEDDB6D, 
		"DFUJobs", 
		L"DFUJobs", 
		sizeof("DFUJobs")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsSMC_Activity_struct, DFUJobs),
		NULL,
		&__DFUJob_map,
		6+1,
	},
	{
		0x1BEDDB6D,
		"__DFUJobs_nSizeIs",
		L"__DFUJobs_nSizeIs",
		sizeof("__DFUJobs_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsSMC_Activity_struct, __DFUJobs_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xC117ADC1, 
		"Running", 
		L"Running", 
		sizeof("Running")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsSMC_Activity_struct, Running),
		NULL,
		&__ActiveWorkunit_map,
		8+1,
	},
	{
		0xC117ADC1,
		"__Running_nSizeIs",
		L"__Running_nSizeIs",
		sizeof("__Running_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsSMC_Activity_struct, __Running_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsSMC_Activity_map =
{
	0xEC3C839C,
	"Activity",
	L"ActivityResponse",
	sizeof("Activity")-1,
	sizeof("ActivityResponse")-1,
	SOAPMAP_FUNC,
	__CWsSMC_Activity_entries,
	sizeof(__CWsSMC_Activity_struct),
	6,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x18AC5110,
	"http://webservices.seisint.com/WsSMC",
	L"http://webservices.seisint.com/WsSMC",
	sizeof("http://webservices.seisint.com/WsSMC")-1
};


struct __CWsSMC_PauseQueue_struct
{
	BSTR Cluster;
	ArrayOfEspException Exceptions;
};

extern __declspec(selectany) const _soapmapentry __CWsSMC_PauseQueue_entries[] =
{

	{
		0x29F147E2, 
		"Cluster", 
		L"Cluster", 
		sizeof("Cluster")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsSMC_PauseQueue_struct, Cluster),
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
		offsetof(__CWsSMC_PauseQueue_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsSMC_PauseQueue_map =
{
	0x9B8D5D97,
	"PauseQueue",
	L"SMCQueueResponse",
	sizeof("PauseQueue")-1,
	sizeof("SMCQueueResponse")-1,
	SOAPMAP_FUNC,
	__CWsSMC_PauseQueue_entries,
	sizeof(__CWsSMC_PauseQueue_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x18AC5110,
	"http://webservices.seisint.com/WsSMC",
	L"http://webservices.seisint.com/WsSMC",
	sizeof("http://webservices.seisint.com/WsSMC")-1
};


struct __CWsSMC_SetJobPriority_struct
{
	BSTR Cluster;
	BSTR Wuid;
	BSTR Priority;
	ArrayOfEspException Exceptions;
};

extern __declspec(selectany) const _soapmapentry __CWsSMC_SetJobPriority_entries[] =
{

	{
		0x29F147E2, 
		"Cluster", 
		L"Cluster", 
		sizeof("Cluster")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsSMC_SetJobPriority_struct, Cluster),
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
		offsetof(__CWsSMC_SetJobPriority_struct, Wuid),
		NULL,
		NULL,
		-1,
	},
	{
		0x19082F82, 
		"Priority", 
		L"Priority", 
		sizeof("Priority")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsSMC_SetJobPriority_struct, Priority),
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
		offsetof(__CWsSMC_SetJobPriority_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsSMC_SetJobPriority_map =
{
	0x23B104F4,
	"SetJobPriority",
	L"SMCPriorityResponse",
	sizeof("SetJobPriority")-1,
	sizeof("SMCPriorityResponse")-1,
	SOAPMAP_FUNC,
	__CWsSMC_SetJobPriority_entries,
	sizeof(__CWsSMC_SetJobPriority_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x18AC5110,
	"http://webservices.seisint.com/WsSMC",
	L"http://webservices.seisint.com/WsSMC",
	sizeof("http://webservices.seisint.com/WsSMC")-1
};


struct __CWsSMC_MoveJobFront_struct
{
	BSTR Cluster;
	BSTR Wuid;
	ArrayOfEspException Exceptions;
};

extern __declspec(selectany) const _soapmapentry __CWsSMC_MoveJobFront_entries[] =
{

	{
		0x29F147E2, 
		"Cluster", 
		L"Cluster", 
		sizeof("Cluster")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsSMC_MoveJobFront_struct, Cluster),
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
		offsetof(__CWsSMC_MoveJobFront_struct, Wuid),
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
		offsetof(__CWsSMC_MoveJobFront_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsSMC_MoveJobFront_map =
{
	0xEA59720D,
	"MoveJobFront",
	L"SMCJobResponse",
	sizeof("MoveJobFront")-1,
	sizeof("SMCJobResponse")-1,
	SOAPMAP_FUNC,
	__CWsSMC_MoveJobFront_entries,
	sizeof(__CWsSMC_MoveJobFront_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x18AC5110,
	"http://webservices.seisint.com/WsSMC",
	L"http://webservices.seisint.com/WsSMC",
	sizeof("http://webservices.seisint.com/WsSMC")-1
};


struct __CWsSMC_StopQueue_struct
{
	BSTR Cluster;
	ArrayOfEspException Exceptions;
};

extern __declspec(selectany) const _soapmapentry __CWsSMC_StopQueue_entries[] =
{

	{
		0x29F147E2, 
		"Cluster", 
		L"Cluster", 
		sizeof("Cluster")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsSMC_StopQueue_struct, Cluster),
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
		offsetof(__CWsSMC_StopQueue_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsSMC_StopQueue_map =
{
	0x9B8D5D97,
	"StopQueue",
	L"SMCQueueResponse",
	sizeof("StopQueue")-1,
	sizeof("SMCQueueResponse")-1,
	SOAPMAP_FUNC,
	__CWsSMC_StopQueue_entries,
	sizeof(__CWsSMC_StopQueue_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x18AC5110,
	"http://webservices.seisint.com/WsSMC",
	L"http://webservices.seisint.com/WsSMC",
	sizeof("http://webservices.seisint.com/WsSMC")-1
};


struct __CWsSMC_GetThorQueueAvailability_struct
{
	ArrayOfEspException Exceptions;
	ThorCluster *ThorClusters;
	int __ThorClusters_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CWsSMC_GetThorQueueAvailability_entries[] =
{

	{
		0x7B0DBD02, 
		"Exceptions", 
		L"Exceptions", 
		sizeof("Exceptions")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsSMC_GetThorQueueAvailability_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0x459DCA92, 
		"ThorClusters", 
		L"ThorClusters", 
		sizeof("ThorClusters")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsSMC_GetThorQueueAvailability_struct, ThorClusters),
		NULL,
		&__ThorCluster_map,
		1+1,
	},
	{
		0x459DCA92,
		"__ThorClusters_nSizeIs",
		L"__ThorClusters_nSizeIs",
		sizeof("__ThorClusters_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsSMC_GetThorQueueAvailability_struct, __ThorClusters_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsSMC_GetThorQueueAvailability_map =
{
	0x0F48D12C,
	"GetThorQueueAvailability",
	L"GetThorQueueAvailabilityResponse",
	sizeof("GetThorQueueAvailability")-1,
	sizeof("GetThorQueueAvailabilityResponse")-1,
	SOAPMAP_FUNC,
	__CWsSMC_GetThorQueueAvailability_entries,
	sizeof(__CWsSMC_GetThorQueueAvailability_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x18AC5110,
	"http://webservices.seisint.com/WsSMC",
	L"http://webservices.seisint.com/WsSMC",
	sizeof("http://webservices.seisint.com/WsSMC")-1
};


struct __CWsSMC_ResumeQueue_struct
{
	BSTR Cluster;
	ArrayOfEspException Exceptions;
};

extern __declspec(selectany) const _soapmapentry __CWsSMC_ResumeQueue_entries[] =
{

	{
		0x29F147E2, 
		"Cluster", 
		L"Cluster", 
		sizeof("Cluster")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsSMC_ResumeQueue_struct, Cluster),
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
		offsetof(__CWsSMC_ResumeQueue_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsSMC_ResumeQueue_map =
{
	0x9B8D5D97,
	"ResumeQueue",
	L"SMCQueueResponse",
	sizeof("ResumeQueue")-1,
	sizeof("SMCQueueResponse")-1,
	SOAPMAP_FUNC,
	__CWsSMC_ResumeQueue_entries,
	sizeof(__CWsSMC_ResumeQueue_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x18AC5110,
	"http://webservices.seisint.com/WsSMC",
	L"http://webservices.seisint.com/WsSMC",
	sizeof("http://webservices.seisint.com/WsSMC")-1
};


struct __CWsSMC_Index_struct
{
	ArrayOfEspException Exceptions;
};

extern __declspec(selectany) const _soapmapentry __CWsSMC_Index_entries[] =
{

	{
		0x7B0DBD02, 
		"Exceptions", 
		L"Exceptions", 
		sizeof("Exceptions")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsSMC_Index_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsSMC_Index_map =
{
	0xA9241FAA,
	"Index",
	L"SMCIndexResponse",
	sizeof("Index")-1,
	sizeof("SMCIndexResponse")-1,
	SOAPMAP_FUNC,
	__CWsSMC_Index_entries,
	sizeof(__CWsSMC_Index_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x18AC5110,
	"http://webservices.seisint.com/WsSMC",
	L"http://webservices.seisint.com/WsSMC",
	sizeof("http://webservices.seisint.com/WsSMC")-1
};


struct __CWsSMC_MoveJobDown_struct
{
	BSTR Cluster;
	BSTR Wuid;
	ArrayOfEspException Exceptions;
};

extern __declspec(selectany) const _soapmapentry __CWsSMC_MoveJobDown_entries[] =
{

	{
		0x29F147E2, 
		"Cluster", 
		L"Cluster", 
		sizeof("Cluster")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsSMC_MoveJobDown_struct, Cluster),
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
		offsetof(__CWsSMC_MoveJobDown_struct, Wuid),
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
		offsetof(__CWsSMC_MoveJobDown_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsSMC_MoveJobDown_map =
{
	0xEA59720D,
	"MoveJobDown",
	L"SMCJobResponse",
	sizeof("MoveJobDown")-1,
	sizeof("SMCJobResponse")-1,
	SOAPMAP_FUNC,
	__CWsSMC_MoveJobDown_entries,
	sizeof(__CWsSMC_MoveJobDown_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x18AC5110,
	"http://webservices.seisint.com/WsSMC",
	L"http://webservices.seisint.com/WsSMC",
	sizeof("http://webservices.seisint.com/WsSMC")-1
};


struct __CWsSMC_RemoveJob_struct
{
	BSTR Cluster;
	BSTR Wuid;
	ArrayOfEspException Exceptions;
};

extern __declspec(selectany) const _soapmapentry __CWsSMC_RemoveJob_entries[] =
{

	{
		0x29F147E2, 
		"Cluster", 
		L"Cluster", 
		sizeof("Cluster")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsSMC_RemoveJob_struct, Cluster),
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
		offsetof(__CWsSMC_RemoveJob_struct, Wuid),
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
		offsetof(__CWsSMC_RemoveJob_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsSMC_RemoveJob_map =
{
	0xEA59720D,
	"RemoveJob",
	L"SMCJobResponse",
	sizeof("RemoveJob")-1,
	sizeof("SMCJobResponse")-1,
	SOAPMAP_FUNC,
	__CWsSMC_RemoveJob_entries,
	sizeof(__CWsSMC_RemoveJob_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x18AC5110,
	"http://webservices.seisint.com/WsSMC",
	L"http://webservices.seisint.com/WsSMC",
	sizeof("http://webservices.seisint.com/WsSMC")-1
};


struct __CWsSMC_ClearQueue_struct
{
	BSTR Cluster;
	ArrayOfEspException Exceptions;
};

extern __declspec(selectany) const _soapmapentry __CWsSMC_ClearQueue_entries[] =
{

	{
		0x29F147E2, 
		"Cluster", 
		L"Cluster", 
		sizeof("Cluster")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsSMC_ClearQueue_struct, Cluster),
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
		offsetof(__CWsSMC_ClearQueue_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsSMC_ClearQueue_map =
{
	0x9B8D5D97,
	"ClearQueue",
	L"SMCQueueResponse",
	sizeof("ClearQueue")-1,
	sizeof("SMCQueueResponse")-1,
	SOAPMAP_FUNC,
	__CWsSMC_ClearQueue_entries,
	sizeof(__CWsSMC_ClearQueue_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x18AC5110,
	"http://webservices.seisint.com/WsSMC",
	L"http://webservices.seisint.com/WsSMC",
	sizeof("http://webservices.seisint.com/WsSMC")-1
};

extern __declspec(selectany) const _soapmap * __CWsSMC_funcs[] =
{
	&__CWsSMC_MoveJobUp_map,
	&__CWsSMC_MoveJobBack_map,
	&__CWsSMC_Activity_map,
	&__CWsSMC_PauseQueue_map,
	&__CWsSMC_SetJobPriority_map,
	&__CWsSMC_MoveJobFront_map,
	&__CWsSMC_StopQueue_map,
	&__CWsSMC_GetThorQueueAvailability_map,
	&__CWsSMC_ResumeQueue_map,
	&__CWsSMC_Index_map,
	&__CWsSMC_MoveJobDown_map,
	&__CWsSMC_RemoveJob_map,
	&__CWsSMC_ClearQueue_map,
	NULL
};

template <typename TClient>
inline HRESULT CWsSMCT<TClient>::MoveJobUp(
		BSTR Cluster, 
		BSTR Wuid, 
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
	__CWsSMC_MoveJobUp_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Cluster = Cluster;
	__params.Wuid = Wuid;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsSMC/MoveJobUp\?ver_=1.05\"\r\n"));
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
inline HRESULT CWsSMCT<TClient>::MoveJobBack(
		BSTR Cluster, 
		BSTR Wuid, 
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
	__CWsSMC_MoveJobBack_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Cluster = Cluster;
	__params.Wuid = Wuid;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsSMC/MoveJobBack\?ver_=1.05\"\r\n"));
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
inline HRESULT CWsSMCT<TClient>::Activity(
		ArrayOfEspException* Exceptions, 
		BSTR* Build, 
		ThorCluster** ThorClusters, int* __ThorClusters_nSizeIs, 
		HoleCluster** HoleClusters, int* __HoleClusters_nSizeIs, 
		DFUJob** DFUJobs, int* __DFUJobs_nSizeIs, 
		ActiveWorkunit** Running, int* __Running_nSizeIs
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( Build == NULL )
		return E_POINTER;
    if ( ThorClusters == NULL )
		return E_POINTER;
	if( __ThorClusters_nSizeIs == NULL )
		return E_POINTER;
    if ( HoleClusters == NULL )
		return E_POINTER;
	if( __HoleClusters_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUJobs == NULL )
		return E_POINTER;
	if( __DFUJobs_nSizeIs == NULL )
		return E_POINTER;
    if ( Running == NULL )
		return E_POINTER;
	if( __Running_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsSMC_Activity_struct __params;
	memset(&__params, 0x00, sizeof(__params));

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsSMC/Activity\?ver_=1.05\"\r\n"));
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
	*Build = __params.Build;
	*ThorClusters = __params.ThorClusters;
	*__ThorClusters_nSizeIs = __params.__ThorClusters_nSizeIs;
	*HoleClusters = __params.HoleClusters;
	*__HoleClusters_nSizeIs = __params.__HoleClusters_nSizeIs;
	*DFUJobs = __params.DFUJobs;
	*__DFUJobs_nSizeIs = __params.__DFUJobs_nSizeIs;
	*Running = __params.Running;
	*__Running_nSizeIs = __params.__Running_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsSMCT<TClient>::PauseQueue(
		BSTR Cluster, 
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
	__CWsSMC_PauseQueue_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Cluster = Cluster;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsSMC/PauseQueue\?ver_=1.05\"\r\n"));
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
inline HRESULT CWsSMCT<TClient>::SetJobPriority(
		BSTR Cluster, 
		BSTR Wuid, 
		BSTR Priority, 
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
	__CWsSMC_SetJobPriority_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Cluster = Cluster;
	__params.Wuid = Wuid;
	__params.Priority = Priority;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsSMC/SetJobPriority\?ver_=1.05\"\r\n"));
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
inline HRESULT CWsSMCT<TClient>::MoveJobFront(
		BSTR Cluster, 
		BSTR Wuid, 
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
	__CWsSMC_MoveJobFront_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Cluster = Cluster;
	__params.Wuid = Wuid;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsSMC/MoveJobFront\?ver_=1.05\"\r\n"));
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
inline HRESULT CWsSMCT<TClient>::StopQueue(
		BSTR Cluster, 
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
	__CWsSMC_StopQueue_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Cluster = Cluster;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsSMC/StopQueue\?ver_=1.05\"\r\n"));
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
inline HRESULT CWsSMCT<TClient>::GetThorQueueAvailability(
		ArrayOfEspException* Exceptions, 
		ThorCluster** ThorClusters, int* __ThorClusters_nSizeIs
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( ThorClusters == NULL )
		return E_POINTER;
	if( __ThorClusters_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsSMC_GetThorQueueAvailability_struct __params;
	memset(&__params, 0x00, sizeof(__params));

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsSMC/GetThorQueueAvailability\?ver_=1.05\"\r\n"));
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
	*ThorClusters = __params.ThorClusters;
	*__ThorClusters_nSizeIs = __params.__ThorClusters_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsSMCT<TClient>::ResumeQueue(
		BSTR Cluster, 
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
	__CWsSMC_ResumeQueue_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Cluster = Cluster;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsSMC/ResumeQueue\?ver_=1.05\"\r\n"));
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
inline HRESULT CWsSMCT<TClient>::Index(
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
	__CWsSMC_Index_struct __params;
	memset(&__params, 0x00, sizeof(__params));

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsSMC/Index\?ver_=1.05\"\r\n"));
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
inline HRESULT CWsSMCT<TClient>::MoveJobDown(
		BSTR Cluster, 
		BSTR Wuid, 
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
	__CWsSMC_MoveJobDown_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Cluster = Cluster;
	__params.Wuid = Wuid;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsSMC/MoveJobDown\?ver_=1.05\"\r\n"));
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
inline HRESULT CWsSMCT<TClient>::RemoveJob(
		BSTR Cluster, 
		BSTR Wuid, 
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
	__CWsSMC_RemoveJob_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Cluster = Cluster;
	__params.Wuid = Wuid;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsSMC/RemoveJob\?ver_=1.05\"\r\n"));
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
inline HRESULT CWsSMCT<TClient>::ClearQueue(
		BSTR Cluster, 
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
	__CWsSMC_ClearQueue_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Cluster = Cluster;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsSMC/ClearQueue\?ver_=1.05\"\r\n"));
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
ATL_NOINLINE inline const _soapmap ** CWsSMCT<TClient>::GetFunctionMap()
{
	return __CWsSMC_funcs;
}

template <typename TClient>
ATL_NOINLINE inline const _soapmap ** CWsSMCT<TClient>::GetHeaderMap()
{
	static const _soapmapentry __CWsSMC_MoveJobUp_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsSMC_MoveJobUp_atlsoapheader_map = 
	{
		0xEA59720D,
		"MoveJobUp",
		L"SMCJobResponse",
		sizeof("MoveJobUp")-1,
		sizeof("SMCJobResponse")-1,
		SOAPMAP_HEADER,
		__CWsSMC_MoveJobUp_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x18AC5110,
		"http://webservices.seisint.com/WsSMC",
		L"http://webservices.seisint.com/WsSMC",
		sizeof("http://webservices.seisint.com/WsSMC")-1
	};

	static const _soapmapentry __CWsSMC_MoveJobBack_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsSMC_MoveJobBack_atlsoapheader_map = 
	{
		0xEA59720D,
		"MoveJobBack",
		L"SMCJobResponse",
		sizeof("MoveJobBack")-1,
		sizeof("SMCJobResponse")-1,
		SOAPMAP_HEADER,
		__CWsSMC_MoveJobBack_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x18AC5110,
		"http://webservices.seisint.com/WsSMC",
		L"http://webservices.seisint.com/WsSMC",
		sizeof("http://webservices.seisint.com/WsSMC")-1
	};

	static const _soapmapentry __CWsSMC_Activity_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsSMC_Activity_atlsoapheader_map = 
	{
		0xEC3C839C,
		"Activity",
		L"ActivityResponse",
		sizeof("Activity")-1,
		sizeof("ActivityResponse")-1,
		SOAPMAP_HEADER,
		__CWsSMC_Activity_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x18AC5110,
		"http://webservices.seisint.com/WsSMC",
		L"http://webservices.seisint.com/WsSMC",
		sizeof("http://webservices.seisint.com/WsSMC")-1
	};

	static const _soapmapentry __CWsSMC_PauseQueue_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsSMC_PauseQueue_atlsoapheader_map = 
	{
		0x9B8D5D97,
		"PauseQueue",
		L"SMCQueueResponse",
		sizeof("PauseQueue")-1,
		sizeof("SMCQueueResponse")-1,
		SOAPMAP_HEADER,
		__CWsSMC_PauseQueue_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x18AC5110,
		"http://webservices.seisint.com/WsSMC",
		L"http://webservices.seisint.com/WsSMC",
		sizeof("http://webservices.seisint.com/WsSMC")-1
	};

	static const _soapmapentry __CWsSMC_SetJobPriority_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsSMC_SetJobPriority_atlsoapheader_map = 
	{
		0x23B104F4,
		"SetJobPriority",
		L"SMCPriorityResponse",
		sizeof("SetJobPriority")-1,
		sizeof("SMCPriorityResponse")-1,
		SOAPMAP_HEADER,
		__CWsSMC_SetJobPriority_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x18AC5110,
		"http://webservices.seisint.com/WsSMC",
		L"http://webservices.seisint.com/WsSMC",
		sizeof("http://webservices.seisint.com/WsSMC")-1
	};

	static const _soapmapentry __CWsSMC_MoveJobFront_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsSMC_MoveJobFront_atlsoapheader_map = 
	{
		0xEA59720D,
		"MoveJobFront",
		L"SMCJobResponse",
		sizeof("MoveJobFront")-1,
		sizeof("SMCJobResponse")-1,
		SOAPMAP_HEADER,
		__CWsSMC_MoveJobFront_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x18AC5110,
		"http://webservices.seisint.com/WsSMC",
		L"http://webservices.seisint.com/WsSMC",
		sizeof("http://webservices.seisint.com/WsSMC")-1
	};

	static const _soapmapentry __CWsSMC_StopQueue_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsSMC_StopQueue_atlsoapheader_map = 
	{
		0x9B8D5D97,
		"StopQueue",
		L"SMCQueueResponse",
		sizeof("StopQueue")-1,
		sizeof("SMCQueueResponse")-1,
		SOAPMAP_HEADER,
		__CWsSMC_StopQueue_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x18AC5110,
		"http://webservices.seisint.com/WsSMC",
		L"http://webservices.seisint.com/WsSMC",
		sizeof("http://webservices.seisint.com/WsSMC")-1
	};

	static const _soapmapentry __CWsSMC_GetThorQueueAvailability_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsSMC_GetThorQueueAvailability_atlsoapheader_map = 
	{
		0x0F48D12C,
		"GetThorQueueAvailability",
		L"GetThorQueueAvailabilityResponse",
		sizeof("GetThorQueueAvailability")-1,
		sizeof("GetThorQueueAvailabilityResponse")-1,
		SOAPMAP_HEADER,
		__CWsSMC_GetThorQueueAvailability_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x18AC5110,
		"http://webservices.seisint.com/WsSMC",
		L"http://webservices.seisint.com/WsSMC",
		sizeof("http://webservices.seisint.com/WsSMC")-1
	};

	static const _soapmapentry __CWsSMC_ResumeQueue_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsSMC_ResumeQueue_atlsoapheader_map = 
	{
		0x9B8D5D97,
		"ResumeQueue",
		L"SMCQueueResponse",
		sizeof("ResumeQueue")-1,
		sizeof("SMCQueueResponse")-1,
		SOAPMAP_HEADER,
		__CWsSMC_ResumeQueue_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x18AC5110,
		"http://webservices.seisint.com/WsSMC",
		L"http://webservices.seisint.com/WsSMC",
		sizeof("http://webservices.seisint.com/WsSMC")-1
	};

	static const _soapmapentry __CWsSMC_Index_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsSMC_Index_atlsoapheader_map = 
	{
		0xA9241FAA,
		"Index",
		L"SMCIndexResponse",
		sizeof("Index")-1,
		sizeof("SMCIndexResponse")-1,
		SOAPMAP_HEADER,
		__CWsSMC_Index_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x18AC5110,
		"http://webservices.seisint.com/WsSMC",
		L"http://webservices.seisint.com/WsSMC",
		sizeof("http://webservices.seisint.com/WsSMC")-1
	};

	static const _soapmapentry __CWsSMC_MoveJobDown_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsSMC_MoveJobDown_atlsoapheader_map = 
	{
		0xEA59720D,
		"MoveJobDown",
		L"SMCJobResponse",
		sizeof("MoveJobDown")-1,
		sizeof("SMCJobResponse")-1,
		SOAPMAP_HEADER,
		__CWsSMC_MoveJobDown_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x18AC5110,
		"http://webservices.seisint.com/WsSMC",
		L"http://webservices.seisint.com/WsSMC",
		sizeof("http://webservices.seisint.com/WsSMC")-1
	};

	static const _soapmapentry __CWsSMC_RemoveJob_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsSMC_RemoveJob_atlsoapheader_map = 
	{
		0xEA59720D,
		"RemoveJob",
		L"SMCJobResponse",
		sizeof("RemoveJob")-1,
		sizeof("SMCJobResponse")-1,
		SOAPMAP_HEADER,
		__CWsSMC_RemoveJob_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x18AC5110,
		"http://webservices.seisint.com/WsSMC",
		L"http://webservices.seisint.com/WsSMC",
		sizeof("http://webservices.seisint.com/WsSMC")-1
	};

	static const _soapmapentry __CWsSMC_ClearQueue_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsSMC_ClearQueue_atlsoapheader_map = 
	{
		0x9B8D5D97,
		"ClearQueue",
		L"SMCQueueResponse",
		sizeof("ClearQueue")-1,
		sizeof("SMCQueueResponse")-1,
		SOAPMAP_HEADER,
		__CWsSMC_ClearQueue_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x18AC5110,
		"http://webservices.seisint.com/WsSMC",
		L"http://webservices.seisint.com/WsSMC",
		sizeof("http://webservices.seisint.com/WsSMC")-1
	};


	static const _soapmap * __CWsSMC_headers[] =
	{
		&__CWsSMC_MoveJobUp_atlsoapheader_map,
		&__CWsSMC_MoveJobBack_atlsoapheader_map,
		&__CWsSMC_Activity_atlsoapheader_map,
		&__CWsSMC_PauseQueue_atlsoapheader_map,
		&__CWsSMC_SetJobPriority_atlsoapheader_map,
		&__CWsSMC_MoveJobFront_atlsoapheader_map,
		&__CWsSMC_StopQueue_atlsoapheader_map,
		&__CWsSMC_GetThorQueueAvailability_atlsoapheader_map,
		&__CWsSMC_ResumeQueue_atlsoapheader_map,
		&__CWsSMC_Index_atlsoapheader_map,
		&__CWsSMC_MoveJobDown_atlsoapheader_map,
		&__CWsSMC_RemoveJob_atlsoapheader_map,
		&__CWsSMC_ClearQueue_atlsoapheader_map,
		NULL
	};
	
	return __CWsSMC_headers;
}

template <typename TClient>
ATL_NOINLINE inline void * CWsSMCT<TClient>::GetHeaderValue()
{
	return this;
}

template <typename TClient>
ATL_NOINLINE inline const wchar_t * CWsSMCT<TClient>::GetNamespaceUri()
{
	return L"http://webservices.seisint.com/WsSMC";
}

template <typename TClient>
ATL_NOINLINE inline const char * CWsSMCT<TClient>::GetServiceName()
{
	return NULL;
}

template <typename TClient>
ATL_NOINLINE inline const char * CWsSMCT<TClient>::GetNamespaceUriA()
{
	return "http://webservices.seisint.com/WsSMC";
}

template <typename TClient>
ATL_NOINLINE inline HRESULT CWsSMCT<TClient>::CallFunction(
	void *, 
	const wchar_t *, int,
	size_t)
{
	return E_NOTIMPL;
}

template <typename TClient>
ATL_NOINLINE inline HRESULT CWsSMCT<TClient>::GetClientReader(ISAXXMLReader **ppReader)
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

} // namespace WsSMC

template<>
inline HRESULT AtlCleanupValue<WsSMC::HoleCluster>(WsSMC::HoleCluster *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->ClusterName);
	AtlCleanupValue(&pVal->DataModel);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsSMC::HoleCluster>(WsSMC::HoleCluster *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->ClusterName, pMemMgr);
	AtlCleanupValueEx(&pVal->DataModel, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsSMC::ThorCluster>(WsSMC::ThorCluster *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->ClusterName);
	AtlCleanupValue(&pVal->QueueName);
	AtlCleanupValue(&pVal->QueueStatus);
	AtlCleanupValue(&pVal->DoCommand);
	AtlCleanupValue(&pVal->QueueAvailable);
	AtlCleanupValue(&pVal->JobsRunning);
	AtlCleanupValue(&pVal->JobsInQueue);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsSMC::ThorCluster>(WsSMC::ThorCluster *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->ClusterName, pMemMgr);
	AtlCleanupValueEx(&pVal->QueueName, pMemMgr);
	AtlCleanupValueEx(&pVal->QueueStatus, pMemMgr);
	AtlCleanupValueEx(&pVal->DoCommand, pMemMgr);
	AtlCleanupValueEx(&pVal->QueueAvailable, pMemMgr);
	AtlCleanupValueEx(&pVal->JobsRunning, pMemMgr);
	AtlCleanupValueEx(&pVal->JobsInQueue, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsSMC::EspException>(WsSMC::EspException *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Code);
	AtlCleanupValue(&pVal->Audience);
	AtlCleanupValue(&pVal->Source);
	AtlCleanupValue(&pVal->Message);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsSMC::EspException>(WsSMC::EspException *pVal, IAtlMemMgr *pMemMgr)
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
inline HRESULT AtlCleanupValue<WsSMC::ArrayOfEspException>(WsSMC::ArrayOfEspException *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Source);
	AtlCleanupArray(pVal->Exception, pVal->__Exception_nSizeIs);	
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsSMC::ArrayOfEspException>(WsSMC::ArrayOfEspException *pVal, IAtlMemMgr *pMemMgr)
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
inline HRESULT AtlCleanupValue<WsSMC::ActiveWorkunit>(WsSMC::ActiveWorkunit *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Wuid);
	AtlCleanupValue(&pVal->State);
	AtlCleanupValue(&pVal->StateID);
	AtlCleanupValue(&pVal->Owner);
	AtlCleanupValue(&pVal->Jobname);
	AtlCleanupValue(&pVal->Server);
	AtlCleanupValue(&pVal->Instance);
	AtlCleanupValue(&pVal->Priority);
	AtlCleanupValue(&pVal->Extra);
	AtlCleanupValue(&pVal->GraphName);
	AtlCleanupValue(&pVal->Duration);
	AtlCleanupValue(&pVal->GID);
	AtlCleanupValue(&pVal->QueueName);
	AtlCleanupValue(&pVal->MemoryBlocked);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsSMC::ActiveWorkunit>(WsSMC::ActiveWorkunit *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Wuid, pMemMgr);
	AtlCleanupValueEx(&pVal->State, pMemMgr);
	AtlCleanupValueEx(&pVal->StateID, pMemMgr);
	AtlCleanupValueEx(&pVal->Owner, pMemMgr);
	AtlCleanupValueEx(&pVal->Jobname, pMemMgr);
	AtlCleanupValueEx(&pVal->Server, pMemMgr);
	AtlCleanupValueEx(&pVal->Instance, pMemMgr);
	AtlCleanupValueEx(&pVal->Priority, pMemMgr);
	AtlCleanupValueEx(&pVal->Extra, pMemMgr);
	AtlCleanupValueEx(&pVal->GraphName, pMemMgr);
	AtlCleanupValueEx(&pVal->Duration, pMemMgr);
	AtlCleanupValueEx(&pVal->GID, pMemMgr);
	AtlCleanupValueEx(&pVal->QueueName, pMemMgr);
	AtlCleanupValueEx(&pVal->MemoryBlocked, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsSMC::DFUJob>(WsSMC::DFUJob *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->TimeStarted);
	AtlCleanupValue(&pVal->Done);
	AtlCleanupValue(&pVal->Total);
	AtlCleanupValue(&pVal->Command);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsSMC::DFUJob>(WsSMC::DFUJob *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->TimeStarted, pMemMgr);
	AtlCleanupValueEx(&pVal->Done, pMemMgr);
	AtlCleanupValueEx(&pVal->Total, pMemMgr);
	AtlCleanupValueEx(&pVal->Command, pMemMgr);
	return S_OK;
}
