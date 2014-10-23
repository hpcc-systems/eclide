//
// sproxy.exe generated file
// do not modify this file
//
// Created: 10/28/2009@09:12:50
//

#pragma once


#if !defined(_WIN32_WINDOWS) && !defined(_WIN32_WINNT) && !defined(_WIN32_WCE)
#pragma message("warning: defining _WIN32_WINDOWS = 0x0410")
#define _WIN32_WINDOWS 0x0410
#endif

#include <atlsoap.h>

namespace WsTopology
{

struct EspException
{
	BSTR Code;
	BSTR Audience;
	BSTR Source;
	BSTR Message;
};

struct TpCluster
{
	BSTR Type;
	BSTR Name;
	BSTR QueueName;
	BSTR Build;
	BSTR Directory;
	BSTR LogDirectory;
	BSTR Desc;
	BSTR Prefix;
	BSTR Path;
	BSTR DataModel;
	int OS;
};

struct TpMachine
{
	BSTR Name;
	BSTR Netaddress;
	BSTR Domain;
	BSTR Directory;
	BSTR Type;
	BSTR Available;
	int OS;
	BSTR Path;
	int Port;
};

struct ArrayOfTpMachine
{
	TpMachine *_TpMachine;
	int ___TpMachine_nSizeIs;
};

struct TpMySqlServer
{
	BSTR Name;
	BSTR Description;
	BSTR Build;
	BSTR Path;
	ArrayOfTpMachine TpMachines;
};

struct TpGroup
{
	BSTR Name;
	BSTR Prefix;
};

struct TpFTSlave
{
	BSTR Name;
	BSTR Description;
	BSTR Build;
	BSTR Path;
	ArrayOfTpMachine TpMachines;
};

struct TpEclAgent
{
	BSTR Name;
	BSTR Description;
	BSTR Build;
	BSTR Type;
	BSTR Path;
	ArrayOfTpMachine TpMachines;
};

struct TpDali
{
	BSTR Name;
	BSTR Description;
	BSTR Build;
	BSTR BackupComputer;
	BSTR BackupDirectory;
	BSTR Type;
	BSTR Path;
	BSTR LogDirectory;
	ArrayOfTpMachine TpMachines;
};

struct TpEclServer
{
	BSTR Name;
	BSTR Description;
	BSTR Build;
	BSTR Queue;
	BSTR LogDirectory;
	BSTR Type;
	BSTR Path;
	ArrayOfTpMachine TpMachines;
};

struct TpBinding
{
	BSTR Name;
	BSTR Service;
	BSTR ServiceType;
	BSTR Port;
	BSTR Protocol;
};

struct ArrayOfTpBinding
{
	TpBinding *_TpBinding;
	int ___TpBinding_nSizeIs;
};

struct TpEspServer
{
	BSTR Name;
	BSTR Description;
	BSTR Build;
	BSTR Type;
	BSTR Path;
	BSTR LogDirectory;
	ArrayOfTpMachine TpMachines;
	ArrayOfTpBinding TpBindings;
};

struct TpDfuServer
{
	BSTR Name;
	BSTR Description;
	BSTR Build;
	BSTR Queue;
	BSTR Type;
	BSTR Path;
	BSTR LogDirectory;
	ArrayOfTpMachine TpMachines;
};

struct TpSashaServer
{
	BSTR Name;
	BSTR Description;
	BSTR Build;
	BSTR Path;
	BSTR LogDirectory;
	ArrayOfTpMachine TpMachines;
};

struct TpLdapServer
{
	BSTR Name;
	BSTR Description;
	BSTR Build;
	BSTR Path;
	ArrayOfTpMachine TpMachines;
};

struct TpDropZone
{
	BSTR Name;
	BSTR Description;
	BSTR Build;
	BSTR Path;
	ArrayOfTpMachine TpMachines;
};

struct TpDkcSlave
{
	BSTR Name;
	BSTR Description;
	BSTR Build;
	BSTR Path;
	ArrayOfTpMachine TpMachines;
};

struct TpGenesisServer
{
	BSTR Name;
	BSTR Description;
	BSTR Build;
	BSTR Path;
	ArrayOfTpMachine TpMachines;
};

struct TpServices
{
	TpDali *TpDalis;
	int __TpDalis_nSizeIs;
	TpEclServer *TpEclServers;
	int __TpEclServers_nSizeIs;
	TpEclAgent *TpEclAgents;
	int __TpEclAgents_nSizeIs;
	TpEspServer *TpEspServers;
	int __TpEspServers_nSizeIs;
	TpDfuServer *TpDfuServers;
	int __TpDfuServers_nSizeIs;
	TpSashaServer *TpSashaServers;
	int __TpSashaServers_nSizeIs;
	TpMySqlServer *TpMySqlServers;
	int __TpMySqlServers_nSizeIs;
	TpLdapServer *TpLdapServers;
	int __TpLdapServers_nSizeIs;
	TpDropZone *TpDropZones;
	int __TpDropZones_nSizeIs;
	TpFTSlave *TpFTSlaves;
	int __TpFTSlaves_nSizeIs;
	TpDkcSlave *TpDkcSlaves;
	int __TpDkcSlaves_nSizeIs;
	TpGenesisServer *TpGenesisServers;
	int __TpGenesisServers_nSizeIs;
};

struct ArrayOfEspException
{
	BSTR Source;
	EspException *Exception;
	int __Exception_nSizeIs;
};

struct TpLogicalCluster
{
	BSTR Name;
	BSTR Queue;
};

template <typename TClient = CSoapSocketClientT<> >
class CWsTopologyT : 
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

	CWsTopologyT(ISAXXMLReader *pReader = NULL)
		:TClient(_T("http://10.173.88.201:8010/WsTopology\?ver_=1.06"))
	{
		SetClient(true);
		SetReader(pReader);
	}
	
	~CWsTopologyT()
	{
		Uninitialize();
	}
	
	void Uninitialize()
	{
		UninitializeSOAP();
	}	

	HRESULT TpGetComponentFile(
		BSTR CompType, 
		BSTR CompName, 
		BSTR NetAddress, 
		BSTR Directory, 
		BSTR FileType, 
		int OsType, 
		ArrayOfEspException* Exceptions, 
		ATLSOAP_BLOB* FileContents
	);

	HRESULT TpLogFileDisplay(
		BSTR* Name, 
		BSTR* Type, 
		BSTR* StartDate, 
		BSTR* EndDate, 
		int* FirstRows, 
		int* LastRows, 
		int* FilterType, 
		bool* Reversely, 
		bool* Zip, 
		int* PageNumber, 
		bool LoadData, 
		ArrayOfEspException* Exceptions, 
		int* LastHours, 
		BSTR* LogData, 
		bool* HasDate, 
		__int64* FileSize, 
		__int64* PageFrom, 
		__int64* PageTo, 
		int* PrevPage, 
		int* NextPage, 
		int* TotalPages, 
		ATLSOAP_BLOB* thefile
	);

	HRESULT TpGroupQuery(
		ArrayOfEspException* Exceptions, 
		TpGroup** TpGroups, int* TpGroups_nSizeIs
	);

	HRESULT TpXMLFile(
		BSTR Name, 
		ArrayOfEspException* Exceptions, 
		ATLSOAP_BLOB* thefile
	);

	HRESULT TpLogicalClusterQuery(
		BSTR EclServerQueue, 
		ArrayOfEspException* Exceptions, 
		TpLogicalCluster** TpLogicalClusters, int* TpLogicalClusters_nSizeIs
	);

	HRESULT TpLogFile(
		BSTR* Name, 
		BSTR* Type, 
		BSTR* StartDate, 
		BSTR* EndDate, 
		int* FirstRows, 
		int* LastRows, 
		int* FilterType, 
		bool* Reversely, 
		bool* Zip, 
		int* PageNumber, 
		bool LoadData, 
		ArrayOfEspException* Exceptions, 
		int* LastHours, 
		BSTR* LogData, 
		bool* HasDate, 
		__int64* FileSize, 
		__int64* PageFrom, 
		__int64* PageTo, 
		int* PrevPage, 
		int* NextPage, 
		int* TotalPages, 
		ATLSOAP_BLOB* thefile
	);

	HRESULT TpClusterQuery(
		BSTR Type, 
		ArrayOfEspException* Exceptions, 
		TpCluster** TpClusters, int* TpClusters_nSizeIs
	);

	HRESULT TpClusterInfo(
		BSTR* Name, 
		ArrayOfEspException* Exceptions, 
		BSTR* WorkUnit, 
		BSTR* Name2, 
		BSTR* WorkUnit2
	);

	HRESULT TpSwapNode(
		BSTR Cluster, 
		BSTR OldIP, 
		BSTR NewIP, 
		ArrayOfEspException* Exceptions, 
		bool* TpSwapNodeResult
	);

	HRESULT TpSetMachineStatus(
		BSTR MachinePath, 
		BSTR StatusValue, 
		ArrayOfEspException* Exceptions, 
		bool* TpSetMachineStatusResult
	);

	HRESULT TpServiceQuery(
		BSTR Type, 
		ArrayOfEspException* Exceptions, 
		int* MemThreshold, 
		int* DiskThreshold, 
		int* CpuThreshold, 
		bool* SystemUseRewrite, 
		BSTR* MemThresholdType, 
		BSTR* DiskThresholdType, 
		TpServices* ServiceList
	);

	HRESULT TpMachineQuery(
		BSTR* Type, 
		BSTR* Cluster, 
		BSTR* OldIP, 
		BSTR* Path, 
		BSTR Directory, 
		BSTR* LogDirectory, 
		ArrayOfEspException* Exceptions, 
		bool* EnablePreflightInfo, 
		int* MemThreshold, 
		int* DiskThreshold, 
		int* CpuThreshold, 
		BSTR* MemThresholdType, 
		BSTR* DiskThresholdType, 
		TpMachine** TpMachines, int* TpMachines_nSizeIs
	);
};

typedef CWsTopologyT<> CWsTopology;

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
	0x11E544AA,
	"http://webservices.seisint.com/WsTopology",
	L"http://webservices.seisint.com/WsTopology",
	sizeof("http://webservices.seisint.com/WsTopology")-1
};
}

__if_not_exists(__TpCluster_entries)
{
extern __declspec(selectany) const _soapmapentry __TpCluster_entries[] =
{
	{ 
		0x00302162, 
		"Type", 
		L"Type", 
		sizeof("Type")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpCluster, Type),
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
		offsetof(TpCluster, Name),
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
		offsetof(TpCluster, QueueName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x04EC4690, 
		"Build", 
		L"Build", 
		sizeof("Build")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpCluster, Build),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x12576735, 
		"Directory", 
		L"Directory", 
		sizeof("Directory")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpCluster, Directory),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0E0A1B77, 
		"LogDirectory", 
		L"LogDirectory", 
		sizeof("LogDirectory")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpCluster, LogDirectory),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0027069F, 
		"Desc", 
		L"Desc", 
		sizeof("Desc")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpCluster, Desc),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xC2E4BDEE, 
		"Prefix", 
		L"Prefix", 
		sizeof("Prefix")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpCluster, Prefix),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x002D8A4D, 
		"Path", 
		L"Path", 
		sizeof("Path")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpCluster, Path),
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
		offsetof(TpCluster, DataModel),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x00000A82, 
		"OS", 
		L"OS", 
		sizeof("OS")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpCluster, OS),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __TpCluster_map =
{
	0xBF937EA6,
	"TpCluster",
	L"TpCluster",
	sizeof("TpCluster")-1,
	sizeof("TpCluster")-1,
	SOAPMAP_STRUCT,
	__TpCluster_entries,
	sizeof(TpCluster),
	11,
	-1,
	SOAPFLAG_NONE,
	0x11E544AA,
	"http://webservices.seisint.com/WsTopology",
	L"http://webservices.seisint.com/WsTopology",
	sizeof("http://webservices.seisint.com/WsTopology")-1
};
}

__if_not_exists(__TpMachine_entries)
{
extern __declspec(selectany) const _soapmapentry __TpMachine_entries[] =
{
	{ 
		0x002C70A1, 
		"Name", 
		L"Name", 
		sizeof("Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpMachine, Name),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x598BF9ED, 
		"Netaddress", 
		L"Netaddress", 
		sizeof("Netaddress")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpMachine, Netaddress),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xA6B4DA98, 
		"Domain", 
		L"Domain", 
		sizeof("Domain")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpMachine, Domain),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x12576735, 
		"Directory", 
		L"Directory", 
		sizeof("Directory")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpMachine, Directory),
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
		offsetof(TpMachine, Type),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xA188B681, 
		"Available", 
		L"Available", 
		sizeof("Available")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpMachine, Available),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x00000A82, 
		"OS", 
		L"OS", 
		sizeof("OS")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpMachine, OS),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x002D8A4D, 
		"Path", 
		L"Path", 
		sizeof("Path")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpMachine, Path),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x002DC5A5, 
		"Port", 
		L"Port", 
		sizeof("Port")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpMachine, Port),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __TpMachine_map =
{
	0xA6653059,
	"TpMachine",
	L"TpMachine",
	sizeof("TpMachine")-1,
	sizeof("TpMachine")-1,
	SOAPMAP_STRUCT,
	__TpMachine_entries,
	sizeof(TpMachine),
	9,
	-1,
	SOAPFLAG_NONE,
	0x11E544AA,
	"http://webservices.seisint.com/WsTopology",
	L"http://webservices.seisint.com/WsTopology",
	sizeof("http://webservices.seisint.com/WsTopology")-1
};
}

__if_not_exists(__ArrayOfTpMachine_entries)
{
extern __declspec(selectany) const _soapmapentry __ArrayOfTpMachine_entries[] =
{
	{ 
		0xA6653059, 
		"TpMachine", 
		L"TpMachine", 
		sizeof("TpMachine")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(ArrayOfTpMachine, _TpMachine),
		NULL, 
		&__TpMachine_map, 
		0+1 
	},
	{ 
		0xA6653059, 
		"TpMachine", 
		L"TpMachine", 
		sizeof("TpMachine")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(ArrayOfTpMachine, ___TpMachine_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __ArrayOfTpMachine_map =
{
	0xD227D70D,
	"ArrayOfTpMachine",
	L"ArrayOfTpMachine",
	sizeof("ArrayOfTpMachine")-1,
	sizeof("ArrayOfTpMachine")-1,
	SOAPMAP_STRUCT,
	__ArrayOfTpMachine_entries,
	sizeof(ArrayOfTpMachine),
	1,
	-1,
	SOAPFLAG_NONE,
	0x11E544AA,
	"http://webservices.seisint.com/WsTopology",
	L"http://webservices.seisint.com/WsTopology",
	sizeof("http://webservices.seisint.com/WsTopology")-1
};
}

__if_not_exists(__TpMySqlServer_entries)
{
extern __declspec(selectany) const _soapmapentry __TpMySqlServer_entries[] =
{
	{ 
		0x002C70A1, 
		"Name", 
		L"Name", 
		sizeof("Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpMySqlServer, Name),
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
		offsetof(TpMySqlServer, Description),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x04EC4690, 
		"Build", 
		L"Build", 
		sizeof("Build")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpMySqlServer, Build),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x002D8A4D, 
		"Path", 
		L"Path", 
		sizeof("Path")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpMySqlServer, Path),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x730B3BEC, 
		"TpMachines", 
		L"TpMachines", 
		sizeof("TpMachines")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARRWRAPPER | SOAPFLAG_NULLABLE, 
		offsetof(TpMySqlServer, TpMachines),
		NULL, 
		&__ArrayOfTpMachine_map, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __TpMySqlServer_map =
{
	0xCB82AFB1,
	"TpMySqlServer",
	L"TpMySqlServer",
	sizeof("TpMySqlServer")-1,
	sizeof("TpMySqlServer")-1,
	SOAPMAP_STRUCT,
	__TpMySqlServer_entries,
	sizeof(TpMySqlServer),
	5,
	-1,
	SOAPFLAG_NONE,
	0x11E544AA,
	"http://webservices.seisint.com/WsTopology",
	L"http://webservices.seisint.com/WsTopology",
	sizeof("http://webservices.seisint.com/WsTopology")-1
};
}

__if_not_exists(__TpGroup_entries)
{
extern __declspec(selectany) const _soapmapentry __TpGroup_entries[] =
{
	{ 
		0x002C70A1, 
		"Name", 
		L"Name", 
		sizeof("Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpGroup, Name),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xC2E4BDEE, 
		"Prefix", 
		L"Prefix", 
		sizeof("Prefix")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpGroup, Prefix),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __TpGroup_map =
{
	0x4CA2EC71,
	"TpGroup",
	L"TpGroup",
	sizeof("TpGroup")-1,
	sizeof("TpGroup")-1,
	SOAPMAP_STRUCT,
	__TpGroup_entries,
	sizeof(TpGroup),
	2,
	-1,
	SOAPFLAG_NONE,
	0x11E544AA,
	"http://webservices.seisint.com/WsTopology",
	L"http://webservices.seisint.com/WsTopology",
	sizeof("http://webservices.seisint.com/WsTopology")-1
};
}

__if_not_exists(__TpFTSlave_entries)
{
extern __declspec(selectany) const _soapmapentry __TpFTSlave_entries[] =
{
	{ 
		0x002C70A1, 
		"Name", 
		L"Name", 
		sizeof("Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpFTSlave, Name),
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
		offsetof(TpFTSlave, Description),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x04EC4690, 
		"Build", 
		L"Build", 
		sizeof("Build")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpFTSlave, Build),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x002D8A4D, 
		"Path", 
		L"Path", 
		sizeof("Path")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpFTSlave, Path),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x730B3BEC, 
		"TpMachines", 
		L"TpMachines", 
		sizeof("TpMachines")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARRWRAPPER | SOAPFLAG_NULLABLE, 
		offsetof(TpFTSlave, TpMachines),
		NULL, 
		&__ArrayOfTpMachine_map, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __TpFTSlave_map =
{
	0x6C1AFF59,
	"TpFTSlave",
	L"TpFTSlave",
	sizeof("TpFTSlave")-1,
	sizeof("TpFTSlave")-1,
	SOAPMAP_STRUCT,
	__TpFTSlave_entries,
	sizeof(TpFTSlave),
	5,
	-1,
	SOAPFLAG_NONE,
	0x11E544AA,
	"http://webservices.seisint.com/WsTopology",
	L"http://webservices.seisint.com/WsTopology",
	sizeof("http://webservices.seisint.com/WsTopology")-1
};
}

__if_not_exists(__TpEclAgent_entries)
{
extern __declspec(selectany) const _soapmapentry __TpEclAgent_entries[] =
{
	{ 
		0x002C70A1, 
		"Name", 
		L"Name", 
		sizeof("Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpEclAgent, Name),
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
		offsetof(TpEclAgent, Description),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x04EC4690, 
		"Build", 
		L"Build", 
		sizeof("Build")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpEclAgent, Build),
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
		offsetof(TpEclAgent, Type),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x002D8A4D, 
		"Path", 
		L"Path", 
		sizeof("Path")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpEclAgent, Path),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x730B3BEC, 
		"TpMachines", 
		L"TpMachines", 
		sizeof("TpMachines")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARRWRAPPER | SOAPFLAG_NULLABLE, 
		offsetof(TpEclAgent, TpMachines),
		NULL, 
		&__ArrayOfTpMachine_map, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __TpEclAgent_map =
{
	0xBD2C8287,
	"TpEclAgent",
	L"TpEclAgent",
	sizeof("TpEclAgent")-1,
	sizeof("TpEclAgent")-1,
	SOAPMAP_STRUCT,
	__TpEclAgent_entries,
	sizeof(TpEclAgent),
	6,
	-1,
	SOAPFLAG_NONE,
	0x11E544AA,
	"http://webservices.seisint.com/WsTopology",
	L"http://webservices.seisint.com/WsTopology",
	sizeof("http://webservices.seisint.com/WsTopology")-1
};
}

__if_not_exists(__TpDali_entries)
{
extern __declspec(selectany) const _soapmapentry __TpDali_entries[] =
{
	{ 
		0x002C70A1, 
		"Name", 
		L"Name", 
		sizeof("Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpDali, Name),
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
		offsetof(TpDali, Description),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x04EC4690, 
		"Build", 
		L"Build", 
		sizeof("Build")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpDali, Build),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x10DE4F65, 
		"BackupComputer", 
		L"BackupComputer", 
		sizeof("BackupComputer")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpDali, BackupComputer),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x7D8E928B, 
		"BackupDirectory", 
		L"BackupDirectory", 
		sizeof("BackupDirectory")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpDali, BackupDirectory),
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
		offsetof(TpDali, Type),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x002D8A4D, 
		"Path", 
		L"Path", 
		sizeof("Path")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpDali, Path),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0E0A1B77, 
		"LogDirectory", 
		L"LogDirectory", 
		sizeof("LogDirectory")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpDali, LogDirectory),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x730B3BEC, 
		"TpMachines", 
		L"TpMachines", 
		sizeof("TpMachines")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARRWRAPPER | SOAPFLAG_NULLABLE, 
		offsetof(TpDali, TpMachines),
		NULL, 
		&__ArrayOfTpMachine_map, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __TpDali_map =
{
	0xCC0301FE,
	"TpDali",
	L"TpDali",
	sizeof("TpDali")-1,
	sizeof("TpDali")-1,
	SOAPMAP_STRUCT,
	__TpDali_entries,
	sizeof(TpDali),
	9,
	-1,
	SOAPFLAG_NONE,
	0x11E544AA,
	"http://webservices.seisint.com/WsTopology",
	L"http://webservices.seisint.com/WsTopology",
	sizeof("http://webservices.seisint.com/WsTopology")-1
};
}

__if_not_exists(__TpEclServer_entries)
{
extern __declspec(selectany) const _soapmapentry __TpEclServer_entries[] =
{
	{ 
		0x002C70A1, 
		"Name", 
		L"Name", 
		sizeof("Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpEclServer, Name),
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
		offsetof(TpEclServer, Description),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x04EC4690, 
		"Build", 
		L"Build", 
		sizeof("Build")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpEclServer, Build),
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
		offsetof(TpEclServer, Queue),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0E0A1B77, 
		"LogDirectory", 
		L"LogDirectory", 
		sizeof("LogDirectory")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpEclServer, LogDirectory),
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
		offsetof(TpEclServer, Type),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x002D8A4D, 
		"Path", 
		L"Path", 
		sizeof("Path")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpEclServer, Path),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x730B3BEC, 
		"TpMachines", 
		L"TpMachines", 
		sizeof("TpMachines")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARRWRAPPER | SOAPFLAG_NULLABLE, 
		offsetof(TpEclServer, TpMachines),
		NULL, 
		&__ArrayOfTpMachine_map, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __TpEclServer_map =
{
	0x8C9CBF2F,
	"TpEclServer",
	L"TpEclServer",
	sizeof("TpEclServer")-1,
	sizeof("TpEclServer")-1,
	SOAPMAP_STRUCT,
	__TpEclServer_entries,
	sizeof(TpEclServer),
	8,
	-1,
	SOAPFLAG_NONE,
	0x11E544AA,
	"http://webservices.seisint.com/WsTopology",
	L"http://webservices.seisint.com/WsTopology",
	sizeof("http://webservices.seisint.com/WsTopology")-1
};
}

__if_not_exists(__TpBinding_entries)
{
extern __declspec(selectany) const _soapmapentry __TpBinding_entries[] =
{
	{ 
		0x002C70A1, 
		"Name", 
		L"Name", 
		sizeof("Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpBinding, Name),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xE90C2411, 
		"Service", 
		L"Service", 
		sizeof("Service")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpBinding, Service),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x1BE1E5F3, 
		"ServiceType", 
		L"ServiceType", 
		sizeof("ServiceType")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpBinding, ServiceType),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x002DC5A5, 
		"Port", 
		L"Port", 
		sizeof("Port")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpBinding, Port),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x275FDE72, 
		"Protocol", 
		L"Protocol", 
		sizeof("Protocol")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpBinding, Protocol),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __TpBinding_map =
{
	0x6B12B51F,
	"TpBinding",
	L"TpBinding",
	sizeof("TpBinding")-1,
	sizeof("TpBinding")-1,
	SOAPMAP_STRUCT,
	__TpBinding_entries,
	sizeof(TpBinding),
	5,
	-1,
	SOAPFLAG_NONE,
	0x11E544AA,
	"http://webservices.seisint.com/WsTopology",
	L"http://webservices.seisint.com/WsTopology",
	sizeof("http://webservices.seisint.com/WsTopology")-1
};
}

__if_not_exists(__ArrayOfTpBinding_entries)
{
extern __declspec(selectany) const _soapmapentry __ArrayOfTpBinding_entries[] =
{
	{ 
		0x6B12B51F, 
		"TpBinding", 
		L"TpBinding", 
		sizeof("TpBinding")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(ArrayOfTpBinding, _TpBinding),
		NULL, 
		&__TpBinding_map, 
		0+1 
	},
	{ 
		0x6B12B51F, 
		"TpBinding", 
		L"TpBinding", 
		sizeof("TpBinding")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(ArrayOfTpBinding, ___TpBinding_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __ArrayOfTpBinding_map =
{
	0x96D55BD3,
	"ArrayOfTpBinding",
	L"ArrayOfTpBinding",
	sizeof("ArrayOfTpBinding")-1,
	sizeof("ArrayOfTpBinding")-1,
	SOAPMAP_STRUCT,
	__ArrayOfTpBinding_entries,
	sizeof(ArrayOfTpBinding),
	1,
	-1,
	SOAPFLAG_NONE,
	0x11E544AA,
	"http://webservices.seisint.com/WsTopology",
	L"http://webservices.seisint.com/WsTopology",
	sizeof("http://webservices.seisint.com/WsTopology")-1
};
}

__if_not_exists(__TpEspServer_entries)
{
extern __declspec(selectany) const _soapmapentry __TpEspServer_entries[] =
{
	{ 
		0x002C70A1, 
		"Name", 
		L"Name", 
		sizeof("Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpEspServer, Name),
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
		offsetof(TpEspServer, Description),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x04EC4690, 
		"Build", 
		L"Build", 
		sizeof("Build")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpEspServer, Build),
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
		offsetof(TpEspServer, Type),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x002D8A4D, 
		"Path", 
		L"Path", 
		sizeof("Path")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpEspServer, Path),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0E0A1B77, 
		"LogDirectory", 
		L"LogDirectory", 
		sizeof("LogDirectory")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpEspServer, LogDirectory),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x730B3BEC, 
		"TpMachines", 
		L"TpMachines", 
		sizeof("TpMachines")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARRWRAPPER | SOAPFLAG_NULLABLE, 
		offsetof(TpEspServer, TpMachines),
		NULL, 
		&__ArrayOfTpMachine_map, 
		-1 
	},
	{ 
		0xCD695972, 
		"TpBindings", 
		L"TpBindings", 
		sizeof("TpBindings")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARRWRAPPER | SOAPFLAG_NULLABLE, 
		offsetof(TpEspServer, TpBindings),
		NULL, 
		&__ArrayOfTpBinding_map, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __TpEspServer_map =
{
	0x84A30043,
	"TpEspServer",
	L"TpEspServer",
	sizeof("TpEspServer")-1,
	sizeof("TpEspServer")-1,
	SOAPMAP_STRUCT,
	__TpEspServer_entries,
	sizeof(TpEspServer),
	8,
	-1,
	SOAPFLAG_NONE,
	0x11E544AA,
	"http://webservices.seisint.com/WsTopology",
	L"http://webservices.seisint.com/WsTopology",
	sizeof("http://webservices.seisint.com/WsTopology")-1
};
}

__if_not_exists(__TpDfuServer_entries)
{
extern __declspec(selectany) const _soapmapentry __TpDfuServer_entries[] =
{
	{ 
		0x002C70A1, 
		"Name", 
		L"Name", 
		sizeof("Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpDfuServer, Name),
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
		offsetof(TpDfuServer, Description),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x04EC4690, 
		"Build", 
		L"Build", 
		sizeof("Build")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpDfuServer, Build),
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
		offsetof(TpDfuServer, Queue),
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
		offsetof(TpDfuServer, Type),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x002D8A4D, 
		"Path", 
		L"Path", 
		sizeof("Path")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpDfuServer, Path),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0E0A1B77, 
		"LogDirectory", 
		L"LogDirectory", 
		sizeof("LogDirectory")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpDfuServer, LogDirectory),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x730B3BEC, 
		"TpMachines", 
		L"TpMachines", 
		sizeof("TpMachines")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARRWRAPPER | SOAPFLAG_NULLABLE, 
		offsetof(TpDfuServer, TpMachines),
		NULL, 
		&__ArrayOfTpMachine_map, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __TpDfuServer_map =
{
	0x91B1EF9A,
	"TpDfuServer",
	L"TpDfuServer",
	sizeof("TpDfuServer")-1,
	sizeof("TpDfuServer")-1,
	SOAPMAP_STRUCT,
	__TpDfuServer_entries,
	sizeof(TpDfuServer),
	8,
	-1,
	SOAPFLAG_NONE,
	0x11E544AA,
	"http://webservices.seisint.com/WsTopology",
	L"http://webservices.seisint.com/WsTopology",
	sizeof("http://webservices.seisint.com/WsTopology")-1
};
}

__if_not_exists(__TpSashaServer_entries)
{
extern __declspec(selectany) const _soapmapentry __TpSashaServer_entries[] =
{
	{ 
		0x002C70A1, 
		"Name", 
		L"Name", 
		sizeof("Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpSashaServer, Name),
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
		offsetof(TpSashaServer, Description),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x04EC4690, 
		"Build", 
		L"Build", 
		sizeof("Build")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpSashaServer, Build),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x002D8A4D, 
		"Path", 
		L"Path", 
		sizeof("Path")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpSashaServer, Path),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0E0A1B77, 
		"LogDirectory", 
		L"LogDirectory", 
		sizeof("LogDirectory")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpSashaServer, LogDirectory),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x730B3BEC, 
		"TpMachines", 
		L"TpMachines", 
		sizeof("TpMachines")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARRWRAPPER | SOAPFLAG_NULLABLE, 
		offsetof(TpSashaServer, TpMachines),
		NULL, 
		&__ArrayOfTpMachine_map, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __TpSashaServer_map =
{
	0x7D2E5C0B,
	"TpSashaServer",
	L"TpSashaServer",
	sizeof("TpSashaServer")-1,
	sizeof("TpSashaServer")-1,
	SOAPMAP_STRUCT,
	__TpSashaServer_entries,
	sizeof(TpSashaServer),
	6,
	-1,
	SOAPFLAG_NONE,
	0x11E544AA,
	"http://webservices.seisint.com/WsTopology",
	L"http://webservices.seisint.com/WsTopology",
	sizeof("http://webservices.seisint.com/WsTopology")-1
};
}

__if_not_exists(__TpLdapServer_entries)
{
extern __declspec(selectany) const _soapmapentry __TpLdapServer_entries[] =
{
	{ 
		0x002C70A1, 
		"Name", 
		L"Name", 
		sizeof("Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpLdapServer, Name),
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
		offsetof(TpLdapServer, Description),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x04EC4690, 
		"Build", 
		L"Build", 
		sizeof("Build")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpLdapServer, Build),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x002D8A4D, 
		"Path", 
		L"Path", 
		sizeof("Path")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpLdapServer, Path),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x730B3BEC, 
		"TpMachines", 
		L"TpMachines", 
		sizeof("TpMachines")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARRWRAPPER | SOAPFLAG_NULLABLE, 
		offsetof(TpLdapServer, TpMachines),
		NULL, 
		&__ArrayOfTpMachine_map, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __TpLdapServer_map =
{
	0x1795C5DC,
	"TpLdapServer",
	L"TpLdapServer",
	sizeof("TpLdapServer")-1,
	sizeof("TpLdapServer")-1,
	SOAPMAP_STRUCT,
	__TpLdapServer_entries,
	sizeof(TpLdapServer),
	5,
	-1,
	SOAPFLAG_NONE,
	0x11E544AA,
	"http://webservices.seisint.com/WsTopology",
	L"http://webservices.seisint.com/WsTopology",
	sizeof("http://webservices.seisint.com/WsTopology")-1
};
}

__if_not_exists(__TpDropZone_entries)
{
extern __declspec(selectany) const _soapmapentry __TpDropZone_entries[] =
{
	{ 
		0x002C70A1, 
		"Name", 
		L"Name", 
		sizeof("Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpDropZone, Name),
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
		offsetof(TpDropZone, Description),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x04EC4690, 
		"Build", 
		L"Build", 
		sizeof("Build")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpDropZone, Build),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x002D8A4D, 
		"Path", 
		L"Path", 
		sizeof("Path")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpDropZone, Path),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x730B3BEC, 
		"TpMachines", 
		L"TpMachines", 
		sizeof("TpMachines")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARRWRAPPER | SOAPFLAG_NULLABLE, 
		offsetof(TpDropZone, TpMachines),
		NULL, 
		&__ArrayOfTpMachine_map, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __TpDropZone_map =
{
	0x5DDF4015,
	"TpDropZone",
	L"TpDropZone",
	sizeof("TpDropZone")-1,
	sizeof("TpDropZone")-1,
	SOAPMAP_STRUCT,
	__TpDropZone_entries,
	sizeof(TpDropZone),
	5,
	-1,
	SOAPFLAG_NONE,
	0x11E544AA,
	"http://webservices.seisint.com/WsTopology",
	L"http://webservices.seisint.com/WsTopology",
	sizeof("http://webservices.seisint.com/WsTopology")-1
};
}

__if_not_exists(__TpDkcSlave_entries)
{
extern __declspec(selectany) const _soapmapentry __TpDkcSlave_entries[] =
{
	{ 
		0x002C70A1, 
		"Name", 
		L"Name", 
		sizeof("Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpDkcSlave, Name),
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
		offsetof(TpDkcSlave, Description),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x04EC4690, 
		"Build", 
		L"Build", 
		sizeof("Build")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpDkcSlave, Build),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x002D8A4D, 
		"Path", 
		L"Path", 
		sizeof("Path")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpDkcSlave, Path),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x730B3BEC, 
		"TpMachines", 
		L"TpMachines", 
		sizeof("TpMachines")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARRWRAPPER | SOAPFLAG_NULLABLE, 
		offsetof(TpDkcSlave, TpMachines),
		NULL, 
		&__ArrayOfTpMachine_map, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __TpDkcSlave_map =
{
	0x25068CF1,
	"TpDkcSlave",
	L"TpDkcSlave",
	sizeof("TpDkcSlave")-1,
	sizeof("TpDkcSlave")-1,
	SOAPMAP_STRUCT,
	__TpDkcSlave_entries,
	sizeof(TpDkcSlave),
	5,
	-1,
	SOAPFLAG_NONE,
	0x11E544AA,
	"http://webservices.seisint.com/WsTopology",
	L"http://webservices.seisint.com/WsTopology",
	sizeof("http://webservices.seisint.com/WsTopology")-1
};
}

__if_not_exists(__TpGenesisServer_entries)
{
extern __declspec(selectany) const _soapmapentry __TpGenesisServer_entries[] =
{
	{ 
		0x002C70A1, 
		"Name", 
		L"Name", 
		sizeof("Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpGenesisServer, Name),
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
		offsetof(TpGenesisServer, Description),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x04EC4690, 
		"Build", 
		L"Build", 
		sizeof("Build")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpGenesisServer, Build),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x002D8A4D, 
		"Path", 
		L"Path", 
		sizeof("Path")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpGenesisServer, Path),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x730B3BEC, 
		"TpMachines", 
		L"TpMachines", 
		sizeof("TpMachines")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARRWRAPPER | SOAPFLAG_NULLABLE, 
		offsetof(TpGenesisServer, TpMachines),
		NULL, 
		&__ArrayOfTpMachine_map, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __TpGenesisServer_map =
{
	0x513A93E9,
	"TpGenesisServer",
	L"TpGenesisServer",
	sizeof("TpGenesisServer")-1,
	sizeof("TpGenesisServer")-1,
	SOAPMAP_STRUCT,
	__TpGenesisServer_entries,
	sizeof(TpGenesisServer),
	5,
	-1,
	SOAPFLAG_NONE,
	0x11E544AA,
	"http://webservices.seisint.com/WsTopology",
	L"http://webservices.seisint.com/WsTopology",
	sizeof("http://webservices.seisint.com/WsTopology")-1
};
}

__if_not_exists(__TpServices_entries)
{
extern __declspec(selectany) const _soapmapentry __TpServices_entries[] =
{
	{ 
		0x4C634231, 
		"TpDalis", 
		L"TpDalis", 
		sizeof("TpDalis")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(TpServices, TpDalis),
		NULL, 
		&__TpDali_map, 
		0+1 
	},
	{ 
		0x4C634231, 
		"TpDalis", 
		L"TpDalis", 
		sizeof("TpDalis")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(TpServices, __TpDalis_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 
		0x2034A582, 
		"TpEclServers", 
		L"TpEclServers", 
		sizeof("TpEclServers")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(TpServices, TpEclServers),
		NULL, 
		&__TpEclServer_map, 
		2+1 
	},
	{ 
		0x2034A582, 
		"TpEclServers", 
		L"TpEclServers", 
		sizeof("TpEclServers")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(TpServices, __TpEclServers_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 
		0x62BCD3DA, 
		"TpEclAgents", 
		L"TpEclAgents", 
		sizeof("TpEclAgents")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(TpServices, TpEclAgents),
		NULL, 
		&__TpEclAgent_map, 
		4+1 
	},
	{ 
		0x62BCD3DA, 
		"TpEclAgents", 
		L"TpEclAgents", 
		sizeof("TpEclAgents")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(TpServices, __TpEclAgents_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 
		0x19030916, 
		"TpEspServers", 
		L"TpEspServers", 
		sizeof("TpEspServers")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(TpServices, TpEspServers),
		NULL, 
		&__TpEspServer_map, 
		6+1 
	},
	{ 
		0x19030916, 
		"TpEspServers", 
		L"TpEspServers", 
		sizeof("TpEspServers")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(TpServices, __TpEspServers_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 
		0xC7EFE34D, 
		"TpDfuServers", 
		L"TpDfuServers", 
		sizeof("TpDfuServers")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(TpServices, TpDfuServers),
		NULL, 
		&__TpDfuServer_map, 
		8+1 
	},
	{ 
		0xC7EFE34D, 
		"TpDfuServers", 
		L"TpDfuServers", 
		sizeof("TpDfuServers")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(TpServices, __TpDfuServers_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 
		0x22F9DDDE, 
		"TpSashaServers", 
		L"TpSashaServers", 
		sizeof("TpSashaServers")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(TpServices, TpSashaServers),
		NULL, 
		&__TpSashaServer_map, 
		10+1 
	},
	{ 
		0x22F9DDDE, 
		"TpSashaServers", 
		L"TpSashaServers", 
		sizeof("TpSashaServers")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(TpServices, __TpSashaServers_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 
		0x3BD8A644, 
		"TpMySqlServers", 
		L"TpMySqlServers", 
		sizeof("TpMySqlServers")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(TpServices, TpMySqlServers),
		NULL, 
		&__TpMySqlServer_map, 
		12+1 
	},
	{ 
		0x3BD8A644, 
		"TpMySqlServers", 
		L"TpMySqlServers", 
		sizeof("TpMySqlServers")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(TpServices, __TpMySqlServers_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 
		0x0A4E81CF, 
		"TpLdapServers", 
		L"TpLdapServers", 
		sizeof("TpLdapServers")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(TpServices, TpLdapServers),
		NULL, 
		&__TpLdapServer_map, 
		14+1 
	},
	{ 
		0x0A4E81CF, 
		"TpLdapServers", 
		L"TpLdapServers", 
		sizeof("TpLdapServers")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(TpServices, __TpLdapServers_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 
		0x19C74328, 
		"TpDropZones", 
		L"TpDropZones", 
		sizeof("TpDropZones")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(TpServices, TpDropZones),
		NULL, 
		&__TpDropZone_map, 
		16+1 
	},
	{ 
		0x19C74328, 
		"TpDropZones", 
		L"TpDropZones", 
		sizeof("TpDropZones")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(TpServices, __TpDropZones_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 
		0xEF7AEAEC, 
		"TpFTSlaves", 
		L"TpFTSlaves", 
		sizeof("TpFTSlaves")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(TpServices, TpFTSlaves),
		NULL, 
		&__TpFTSlave_map, 
		18+1 
	},
	{ 
		0xEF7AEAEC, 
		"TpFTSlaves", 
		L"TpFTSlaves", 
		sizeof("TpFTSlaves")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(TpServices, __TpFTSlaves_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 
		0xC5D82B84, 
		"TpDkcSlaves", 
		L"TpDkcSlaves", 
		sizeof("TpDkcSlaves")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(TpServices, TpDkcSlaves),
		NULL, 
		&__TpDkcSlave_map, 
		20+1 
	},
	{ 
		0xC5D82B84, 
		"TpDkcSlaves", 
		L"TpDkcSlaves", 
		sizeof("TpDkcSlaves")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(TpServices, __TpDkcSlaves_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 
		0x788D117C, 
		"TpGenesisServers", 
		L"TpGenesisServers", 
		sizeof("TpGenesisServers")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(TpServices, TpGenesisServers),
		NULL, 
		&__TpGenesisServer_map, 
		22+1 
	},
	{ 
		0x788D117C, 
		"TpGenesisServers", 
		L"TpGenesisServers", 
		sizeof("TpGenesisServers")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(TpServices, __TpGenesisServers_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __TpServices_map =
{
	0x5479B5E8,
	"TpServices",
	L"TpServices",
	sizeof("TpServices")-1,
	sizeof("TpServices")-1,
	SOAPMAP_STRUCT,
	__TpServices_entries,
	sizeof(TpServices),
	12,
	-1,
	SOAPFLAG_NONE,
	0x11E544AA,
	"http://webservices.seisint.com/WsTopology",
	L"http://webservices.seisint.com/WsTopology",
	sizeof("http://webservices.seisint.com/WsTopology")-1
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
	0x11E544AA,
	"http://webservices.seisint.com/WsTopology",
	L"http://webservices.seisint.com/WsTopology",
	sizeof("http://webservices.seisint.com/WsTopology")-1
};
}

__if_not_exists(__TpLogicalCluster_entries)
{
extern __declspec(selectany) const _soapmapentry __TpLogicalCluster_entries[] =
{
	{ 
		0x002C70A1, 
		"Name", 
		L"Name", 
		sizeof("Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(TpLogicalCluster, Name),
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
		offsetof(TpLogicalCluster, Queue),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __TpLogicalCluster_map =
{
	0x5FDB9EA1,
	"TpLogicalCluster",
	L"TpLogicalCluster",
	sizeof("TpLogicalCluster")-1,
	sizeof("TpLogicalCluster")-1,
	SOAPMAP_STRUCT,
	__TpLogicalCluster_entries,
	sizeof(TpLogicalCluster),
	2,
	-1,
	SOAPFLAG_NONE,
	0x11E544AA,
	"http://webservices.seisint.com/WsTopology",
	L"http://webservices.seisint.com/WsTopology",
	sizeof("http://webservices.seisint.com/WsTopology")-1
};
}

struct __CWsTopology_TpGetComponentFile_struct
{
	BSTR CompType;
	BSTR CompName;
	BSTR NetAddress;
	BSTR Directory;
	BSTR FileType;
	int OsType;
	ArrayOfEspException Exceptions;
	ATLSOAP_BLOB FileContents;
};

extern __declspec(selectany) const _soapmapentry __CWsTopology_TpGetComponentFile_entries[] =
{

	{
		0x3C1834F1, 
		"CompType", 
		L"CompType", 
		sizeof("CompType")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpGetComponentFile_struct, CompType),
		NULL,
		NULL,
		-1,
	},
	{
		0x3C148430, 
		"CompName", 
		L"CompName", 
		sizeof("CompName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpGetComponentFile_struct, CompName),
		NULL,
		NULL,
		-1,
	},
	{
		0xBA4461CD, 
		"NetAddress", 
		L"NetAddress", 
		sizeof("NetAddress")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpGetComponentFile_struct, NetAddress),
		NULL,
		NULL,
		-1,
	},
	{
		0x12576735, 
		"Directory", 
		L"Directory", 
		sizeof("Directory")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpGetComponentFile_struct, Directory),
		NULL,
		NULL,
		-1,
	},
	{
		0x2FE410E2, 
		"FileType", 
		L"FileType", 
		sizeof("FileType")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpGetComponentFile_struct, FileType),
		NULL,
		NULL,
		-1,
	},
	{
		0xC098AD04, 
		"OsType", 
		L"OsType", 
		sizeof("OsType")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpGetComponentFile_struct, OsType),
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
		offsetof(__CWsTopology_TpGetComponentFile_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0xFDDE934E, 
		"FileContents", 
		L"FileContents", 
		sizeof("FileContents")-1, 
		SOAPTYPE_BASE64BINARY, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpGetComponentFile_struct, FileContents),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsTopology_TpGetComponentFile_map =
{
	0xEC90DC46,
	"TpGetComponentFileRequest",
	L"TpGetComponentFileResponse",
	sizeof("TpGetComponentFileRequest")-1,
	sizeof("TpGetComponentFileResponse")-1,
	SOAPMAP_FUNC,
	__CWsTopology_TpGetComponentFile_entries,
	sizeof(__CWsTopology_TpGetComponentFile_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x11E544AA,
	"http://webservices.seisint.com/WsTopology",
	L"http://webservices.seisint.com/WsTopology",
	sizeof("http://webservices.seisint.com/WsTopology")-1
};


struct __CWsTopology_TpLogFileDisplay_struct
{
	BSTR Name;
	BSTR Type;
	BSTR StartDate;
	BSTR EndDate;
	int FirstRows;
	int LastRows;
	int FilterType;
	bool Reversely;
	bool Zip;
	int PageNumber;
	bool LoadData;
	ArrayOfEspException Exceptions;
	int LastHours;
	BSTR LogData;
	bool HasDate;
	__int64 FileSize;
	__int64 PageFrom;
	__int64 PageTo;
	int PrevPage;
	int NextPage;
	int TotalPages;
	ATLSOAP_BLOB thefile;
};

extern __declspec(selectany) const _soapmapentry __CWsTopology_TpLogFileDisplay_entries[] =
{

	{
		0x002C70A1, 
		"Name", 
		L"Name", 
		sizeof("Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpLogFileDisplay_struct, Name),
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
		offsetof(__CWsTopology_TpLogFileDisplay_struct, Type),
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
		offsetof(__CWsTopology_TpLogFileDisplay_struct, StartDate),
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
		offsetof(__CWsTopology_TpLogFileDisplay_struct, EndDate),
		NULL,
		NULL,
		-1,
	},
	{
		0x1D197E53, 
		"FirstRows", 
		L"FirstRows", 
		sizeof("FirstRows")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpLogFileDisplay_struct, FirstRows),
		NULL,
		NULL,
		-1,
	},
	{
		0x62FF71BF, 
		"LastRows", 
		L"LastRows", 
		sizeof("LastRows")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpLogFileDisplay_struct, LastRows),
		NULL,
		NULL,
		-1,
	},
	{
		0x62EF7128, 
		"FilterType", 
		L"FilterType", 
		sizeof("FilterType")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpLogFileDisplay_struct, FilterType),
		NULL,
		NULL,
		-1,
	},
	{
		0xF515ECA1, 
		"Reversely", 
		L"Reversely", 
		sizeof("Reversely")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpLogFileDisplay_struct, Reversely),
		NULL,
		NULL,
		-1,
	},
	{
		0x00018CD3, 
		"Zip", 
		L"Zip", 
		sizeof("Zip")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpLogFileDisplay_struct, Zip),
		NULL,
		NULL,
		-1,
	},
	{
		0x921FD206, 
		"PageNumber", 
		L"PageNumber", 
		sizeof("PageNumber")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpLogFileDisplay_struct, PageNumber),
		NULL,
		NULL,
		-1,
	},
	{
		0x6D88779A, 
		"LoadData", 
		L"LoadData", 
		sizeof("LoadData")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpLogFileDisplay_struct, LoadData),
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
		offsetof(__CWsTopology_TpLogFileDisplay_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0xC238AC65, 
		"LastHours", 
		L"LastHours", 
		sizeof("LastHours")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpLogFileDisplay_struct, LastHours),
		NULL,
		NULL,
		-1,
	},
	{
		0xE4A578BC, 
		"LogData", 
		L"LogData", 
		sizeof("LogData")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpLogFileDisplay_struct, LogData),
		NULL,
		NULL,
		-1,
	},
	{
		0x90ED72FA, 
		"HasDate", 
		L"HasDate", 
		sizeof("HasDate")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpLogFileDisplay_struct, HasDate),
		NULL,
		NULL,
		-1,
	},
	{
		0x2FE341BB, 
		"FileSize", 
		L"FileSize", 
		sizeof("FileSize")-1, 
		SOAPTYPE_LONG, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpLogFileDisplay_struct, FileSize),
		NULL,
		NULL,
		-1,
	},
	{
		0xF6F2E551, 
		"PageFrom", 
		L"PageFrom", 
		sizeof("PageFrom")-1, 
		SOAPTYPE_LONG, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpLogFileDisplay_struct, PageFrom),
		NULL,
		NULL,
		-1,
	},
	{
		0xC1B22F20, 
		"PageTo", 
		L"PageTo", 
		sizeof("PageTo")-1, 
		SOAPTYPE_LONG, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpLogFileDisplay_struct, PageTo),
		NULL,
		NULL,
		-1,
	},
	{
		0x101F6FDA, 
		"PrevPage", 
		L"PrevPage", 
		sizeof("PrevPage")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpLogFileDisplay_struct, PrevPage),
		NULL,
		NULL,
		-1,
	},
	{
		0x7B14833C, 
		"NextPage", 
		L"NextPage", 
		sizeof("NextPage")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpLogFileDisplay_struct, NextPage),
		NULL,
		NULL,
		-1,
	},
	{
		0x0FDDD9F4, 
		"TotalPages", 
		L"TotalPages", 
		sizeof("TotalPages")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpLogFileDisplay_struct, TotalPages),
		NULL,
		NULL,
		-1,
	},
	{
		0xDB596F61, 
		"thefile", 
		L"thefile", 
		sizeof("thefile")-1, 
		SOAPTYPE_BASE64BINARY, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpLogFileDisplay_struct, thefile),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsTopology_TpLogFileDisplay_map =
{
	0x7F63B175,
	"TpLogFileRequest",
	L"TpLogFileResponse",
	sizeof("TpLogFileRequest")-1,
	sizeof("TpLogFileResponse")-1,
	SOAPMAP_FUNC,
	__CWsTopology_TpLogFileDisplay_entries,
	sizeof(__CWsTopology_TpLogFileDisplay_struct),
	21,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x11E544AA,
	"http://webservices.seisint.com/WsTopology",
	L"http://webservices.seisint.com/WsTopology",
	sizeof("http://webservices.seisint.com/WsTopology")-1
};


struct __CWsTopology_TpGroupQuery_struct
{
	ArrayOfEspException Exceptions;
	TpGroup *TpGroups;
	int __TpGroups_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CWsTopology_TpGroupQuery_entries[] =
{

	{
		0x7B0DBD02, 
		"Exceptions", 
		L"Exceptions", 
		sizeof("Exceptions")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpGroupQuery_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0xE1007B04, 
		"TpGroups", 
		L"TpGroups", 
		sizeof("TpGroups")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpGroupQuery_struct, TpGroups),
		NULL,
		&__TpGroup_map,
		1+1,
	},
	{
		0xE1007B04,
		"__TpGroups_nSizeIs",
		L"__TpGroups_nSizeIs",
		sizeof("__TpGroups_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsTopology_TpGroupQuery_struct, __TpGroups_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsTopology_TpGroupQuery_map =
{
	0xB5FBFA36,
	"TpGroupQueryRequest",
	L"TpGroupQueryResponse",
	sizeof("TpGroupQueryRequest")-1,
	sizeof("TpGroupQueryResponse")-1,
	SOAPMAP_FUNC,
	__CWsTopology_TpGroupQuery_entries,
	sizeof(__CWsTopology_TpGroupQuery_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x11E544AA,
	"http://webservices.seisint.com/WsTopology",
	L"http://webservices.seisint.com/WsTopology",
	sizeof("http://webservices.seisint.com/WsTopology")-1
};


struct __CWsTopology_TpXMLFile_struct
{
	BSTR Name;
	ArrayOfEspException Exceptions;
	ATLSOAP_BLOB thefile;
};

extern __declspec(selectany) const _soapmapentry __CWsTopology_TpXMLFile_entries[] =
{

	{
		0x002C70A1, 
		"Name", 
		L"Name", 
		sizeof("Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpXMLFile_struct, Name),
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
		offsetof(__CWsTopology_TpXMLFile_struct, Exceptions),
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
		offsetof(__CWsTopology_TpXMLFile_struct, thefile),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsTopology_TpXMLFile_map =
{
	0xE92F2E84,
	"TpXMLFileRequest",
	L"TpXMLFileResponse",
	sizeof("TpXMLFileRequest")-1,
	sizeof("TpXMLFileResponse")-1,
	SOAPMAP_FUNC,
	__CWsTopology_TpXMLFile_entries,
	sizeof(__CWsTopology_TpXMLFile_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x11E544AA,
	"http://webservices.seisint.com/WsTopology",
	L"http://webservices.seisint.com/WsTopology",
	sizeof("http://webservices.seisint.com/WsTopology")-1
};


struct __CWsTopology_TpLogicalClusterQuery_struct
{
	BSTR EclServerQueue;
	ArrayOfEspException Exceptions;
	TpLogicalCluster *TpLogicalClusters;
	int __TpLogicalClusters_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CWsTopology_TpLogicalClusterQuery_entries[] =
{

	{
		0x5A59C890, 
		"EclServerQueue", 
		L"EclServerQueue", 
		sizeof("EclServerQueue")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpLogicalClusterQuery_struct, EclServerQueue),
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
		offsetof(__CWsTopology_TpLogicalClusterQuery_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0x5B4F7334, 
		"TpLogicalClusters", 
		L"TpLogicalClusters", 
		sizeof("TpLogicalClusters")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpLogicalClusterQuery_struct, TpLogicalClusters),
		NULL,
		&__TpLogicalCluster_map,
		2+1,
	},
	{
		0x5B4F7334,
		"__TpLogicalClusters_nSizeIs",
		L"__TpLogicalClusters_nSizeIs",
		sizeof("__TpLogicalClusters_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsTopology_TpLogicalClusterQuery_struct, __TpLogicalClusters_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsTopology_TpLogicalClusterQuery_map =
{
	0x0850BA66,
	"TpLogicalClusterQueryRequest",
	L"TpLogicalClusterQueryResponse",
	sizeof("TpLogicalClusterQueryRequest")-1,
	sizeof("TpLogicalClusterQueryResponse")-1,
	SOAPMAP_FUNC,
	__CWsTopology_TpLogicalClusterQuery_entries,
	sizeof(__CWsTopology_TpLogicalClusterQuery_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x11E544AA,
	"http://webservices.seisint.com/WsTopology",
	L"http://webservices.seisint.com/WsTopology",
	sizeof("http://webservices.seisint.com/WsTopology")-1
};


struct __CWsTopology_TpLogFile_struct
{
	BSTR Name;
	BSTR Type;
	BSTR StartDate;
	BSTR EndDate;
	int FirstRows;
	int LastRows;
	int FilterType;
	bool Reversely;
	bool Zip;
	int PageNumber;
	bool LoadData;
	ArrayOfEspException Exceptions;
	int LastHours;
	BSTR LogData;
	bool HasDate;
	__int64 FileSize;
	__int64 PageFrom;
	__int64 PageTo;
	int PrevPage;
	int NextPage;
	int TotalPages;
	ATLSOAP_BLOB thefile;
};

extern __declspec(selectany) const _soapmapentry __CWsTopology_TpLogFile_entries[] =
{

	{
		0x002C70A1, 
		"Name", 
		L"Name", 
		sizeof("Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpLogFile_struct, Name),
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
		offsetof(__CWsTopology_TpLogFile_struct, Type),
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
		offsetof(__CWsTopology_TpLogFile_struct, StartDate),
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
		offsetof(__CWsTopology_TpLogFile_struct, EndDate),
		NULL,
		NULL,
		-1,
	},
	{
		0x1D197E53, 
		"FirstRows", 
		L"FirstRows", 
		sizeof("FirstRows")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpLogFile_struct, FirstRows),
		NULL,
		NULL,
		-1,
	},
	{
		0x62FF71BF, 
		"LastRows", 
		L"LastRows", 
		sizeof("LastRows")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpLogFile_struct, LastRows),
		NULL,
		NULL,
		-1,
	},
	{
		0x62EF7128, 
		"FilterType", 
		L"FilterType", 
		sizeof("FilterType")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpLogFile_struct, FilterType),
		NULL,
		NULL,
		-1,
	},
	{
		0xF515ECA1, 
		"Reversely", 
		L"Reversely", 
		sizeof("Reversely")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpLogFile_struct, Reversely),
		NULL,
		NULL,
		-1,
	},
	{
		0x00018CD3, 
		"Zip", 
		L"Zip", 
		sizeof("Zip")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpLogFile_struct, Zip),
		NULL,
		NULL,
		-1,
	},
	{
		0x921FD206, 
		"PageNumber", 
		L"PageNumber", 
		sizeof("PageNumber")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpLogFile_struct, PageNumber),
		NULL,
		NULL,
		-1,
	},
	{
		0x6D88779A, 
		"LoadData", 
		L"LoadData", 
		sizeof("LoadData")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpLogFile_struct, LoadData),
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
		offsetof(__CWsTopology_TpLogFile_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0xC238AC65, 
		"LastHours", 
		L"LastHours", 
		sizeof("LastHours")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpLogFile_struct, LastHours),
		NULL,
		NULL,
		-1,
	},
	{
		0xE4A578BC, 
		"LogData", 
		L"LogData", 
		sizeof("LogData")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpLogFile_struct, LogData),
		NULL,
		NULL,
		-1,
	},
	{
		0x90ED72FA, 
		"HasDate", 
		L"HasDate", 
		sizeof("HasDate")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpLogFile_struct, HasDate),
		NULL,
		NULL,
		-1,
	},
	{
		0x2FE341BB, 
		"FileSize", 
		L"FileSize", 
		sizeof("FileSize")-1, 
		SOAPTYPE_LONG, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpLogFile_struct, FileSize),
		NULL,
		NULL,
		-1,
	},
	{
		0xF6F2E551, 
		"PageFrom", 
		L"PageFrom", 
		sizeof("PageFrom")-1, 
		SOAPTYPE_LONG, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpLogFile_struct, PageFrom),
		NULL,
		NULL,
		-1,
	},
	{
		0xC1B22F20, 
		"PageTo", 
		L"PageTo", 
		sizeof("PageTo")-1, 
		SOAPTYPE_LONG, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpLogFile_struct, PageTo),
		NULL,
		NULL,
		-1,
	},
	{
		0x101F6FDA, 
		"PrevPage", 
		L"PrevPage", 
		sizeof("PrevPage")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpLogFile_struct, PrevPage),
		NULL,
		NULL,
		-1,
	},
	{
		0x7B14833C, 
		"NextPage", 
		L"NextPage", 
		sizeof("NextPage")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpLogFile_struct, NextPage),
		NULL,
		NULL,
		-1,
	},
	{
		0x0FDDD9F4, 
		"TotalPages", 
		L"TotalPages", 
		sizeof("TotalPages")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpLogFile_struct, TotalPages),
		NULL,
		NULL,
		-1,
	},
	{
		0xDB596F61, 
		"thefile", 
		L"thefile", 
		sizeof("thefile")-1, 
		SOAPTYPE_BASE64BINARY, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpLogFile_struct, thefile),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsTopology_TpLogFile_map =
{
	0x7F63B175,
	"TpLogFileRequest",
	L"TpLogFileResponse",
	sizeof("TpLogFileRequest")-1,
	sizeof("TpLogFileResponse")-1,
	SOAPMAP_FUNC,
	__CWsTopology_TpLogFile_entries,
	sizeof(__CWsTopology_TpLogFile_struct),
	21,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x11E544AA,
	"http://webservices.seisint.com/WsTopology",
	L"http://webservices.seisint.com/WsTopology",
	sizeof("http://webservices.seisint.com/WsTopology")-1
};


struct __CWsTopology_TpClusterQuery_struct
{
	BSTR Type;
	ArrayOfEspException Exceptions;
	TpCluster *TpClusters;
	int __TpClusters_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CWsTopology_TpClusterQuery_entries[] =
{

	{
		0x00302162, 
		"Type", 
		L"Type", 
		sizeof("Type")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpClusterQuery_struct, Type),
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
		offsetof(__CWsTopology_TpClusterQuery_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0xB20353D9, 
		"TpClusters", 
		L"TpClusters", 
		sizeof("TpClusters")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpClusterQuery_struct, TpClusters),
		NULL,
		&__TpCluster_map,
		2+1,
	},
	{
		0xB20353D9,
		"__TpClusters_nSizeIs",
		L"__TpClusters_nSizeIs",
		sizeof("__TpClusters_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsTopology_TpClusterQuery_struct, __TpClusters_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsTopology_TpClusterQuery_map =
{
	0x5415BA8B,
	"TpClusterQueryRequest",
	L"TpClusterQueryResponse",
	sizeof("TpClusterQueryRequest")-1,
	sizeof("TpClusterQueryResponse")-1,
	SOAPMAP_FUNC,
	__CWsTopology_TpClusterQuery_entries,
	sizeof(__CWsTopology_TpClusterQuery_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x11E544AA,
	"http://webservices.seisint.com/WsTopology",
	L"http://webservices.seisint.com/WsTopology",
	sizeof("http://webservices.seisint.com/WsTopology")-1
};


struct __CWsTopology_TpClusterInfo_struct
{
	BSTR Name;
	ArrayOfEspException Exceptions;
	BSTR WorkUnit;
	BSTR Name2;
	BSTR WorkUnit2;
};

extern __declspec(selectany) const _soapmapentry __CWsTopology_TpClusterInfo_entries[] =
{

	{
		0x002C70A1, 
		"Name", 
		L"Name", 
		sizeof("Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpClusterInfo_struct, Name),
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
		offsetof(__CWsTopology_TpClusterInfo_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0xBC8C83E3, 
		"WorkUnit", 
		L"WorkUnit", 
		sizeof("WorkUnit")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpClusterInfo_struct, WorkUnit),
		NULL,
		NULL,
		-1,
	},
	{
		0x05BA84F3, 
		"Name2", 
		L"Name2", 
		sizeof("Name2")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpClusterInfo_struct, Name2),
		NULL,
		NULL,
		-1,
	},
	{
		0x4E1D0075, 
		"WorkUnit2", 
		L"WorkUnit2", 
		sizeof("WorkUnit2")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpClusterInfo_struct, WorkUnit2),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsTopology_TpClusterInfo_map =
{
	0x7401DF01,
	"TpClusterInfoRequest",
	L"TpClusterInfoResponse",
	sizeof("TpClusterInfoRequest")-1,
	sizeof("TpClusterInfoResponse")-1,
	SOAPMAP_FUNC,
	__CWsTopology_TpClusterInfo_entries,
	sizeof(__CWsTopology_TpClusterInfo_struct),
	5,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x11E544AA,
	"http://webservices.seisint.com/WsTopology",
	L"http://webservices.seisint.com/WsTopology",
	sizeof("http://webservices.seisint.com/WsTopology")-1
};


struct __CWsTopology_TpSwapNode_struct
{
	BSTR Cluster;
	BSTR OldIP;
	BSTR NewIP;
	ArrayOfEspException Exceptions;
	bool TpSwapNodeResult;
};

extern __declspec(selectany) const _soapmapentry __CWsTopology_TpSwapNode_entries[] =
{

	{
		0x29F147E2, 
		"Cluster", 
		L"Cluster", 
		sizeof("Cluster")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpSwapNode_struct, Cluster),
		NULL,
		NULL,
		-1,
	},
	{
		0x05D27BD8, 
		"OldIP", 
		L"OldIP", 
		sizeof("OldIP")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpSwapNode_struct, OldIP),
		NULL,
		NULL,
		-1,
	},
	{
		0x05BCDD83, 
		"NewIP", 
		L"NewIP", 
		sizeof("NewIP")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpSwapNode_struct, NewIP),
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
		offsetof(__CWsTopology_TpSwapNode_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0x9CC05044, 
		"TpSwapNodeResult", 
		L"TpSwapNodeResult", 
		sizeof("TpSwapNodeResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpSwapNode_struct, TpSwapNodeResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsTopology_TpSwapNode_map =
{
	0xCDBC9194,
	"TpSwapNodeRequest",
	L"TpSwapNodeResponse",
	sizeof("TpSwapNodeRequest")-1,
	sizeof("TpSwapNodeResponse")-1,
	SOAPMAP_FUNC,
	__CWsTopology_TpSwapNode_entries,
	sizeof(__CWsTopology_TpSwapNode_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x11E544AA,
	"http://webservices.seisint.com/WsTopology",
	L"http://webservices.seisint.com/WsTopology",
	sizeof("http://webservices.seisint.com/WsTopology")-1
};


struct __CWsTopology_TpSetMachineStatus_struct
{
	BSTR MachinePath;
	BSTR StatusValue;
	ArrayOfEspException Exceptions;
	bool TpSetMachineStatusResult;
};

extern __declspec(selectany) const _soapmapentry __CWsTopology_TpSetMachineStatus_entries[] =
{

	{
		0x464D4662, 
		"MachinePath", 
		L"MachinePath", 
		sizeof("MachinePath")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpSetMachineStatus_struct, MachinePath),
		NULL,
		NULL,
		-1,
	},
	{
		0x83BE19E1, 
		"StatusValue", 
		L"StatusValue", 
		sizeof("StatusValue")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpSetMachineStatus_struct, StatusValue),
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
		offsetof(__CWsTopology_TpSetMachineStatus_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0xFE966FE8, 
		"TpSetMachineStatusResult", 
		L"TpSetMachineStatusResult", 
		sizeof("TpSetMachineStatusResult")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpSetMachineStatus_struct, TpSetMachineStatusResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsTopology_TpSetMachineStatus_map =
{
	0xFD992A38,
	"TpSetMachineStatusRequest",
	L"TpSetMachineStatusResponse",
	sizeof("TpSetMachineStatusRequest")-1,
	sizeof("TpSetMachineStatusResponse")-1,
	SOAPMAP_FUNC,
	__CWsTopology_TpSetMachineStatus_entries,
	sizeof(__CWsTopology_TpSetMachineStatus_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x11E544AA,
	"http://webservices.seisint.com/WsTopology",
	L"http://webservices.seisint.com/WsTopology",
	sizeof("http://webservices.seisint.com/WsTopology")-1
};


struct __CWsTopology_TpServiceQuery_struct
{
	BSTR Type;
	ArrayOfEspException Exceptions;
	int MemThreshold;
	int DiskThreshold;
	int CpuThreshold;
	bool SystemUseRewrite;
	BSTR MemThresholdType;
	BSTR DiskThresholdType;
	TpServices ServiceList;
};

extern __declspec(selectany) const _soapmapentry __CWsTopology_TpServiceQuery_entries[] =
{

	{
		0x00302162, 
		"Type", 
		L"Type", 
		sizeof("Type")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpServiceQuery_struct, Type),
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
		offsetof(__CWsTopology_TpServiceQuery_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0x7E3DF8EC, 
		"MemThreshold", 
		L"MemThreshold", 
		sizeof("MemThreshold")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpServiceQuery_struct, MemThreshold),
		NULL,
		NULL,
		-1,
	},
	{
		0x31177D18, 
		"DiskThreshold", 
		L"DiskThreshold", 
		sizeof("DiskThreshold")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpServiceQuery_struct, DiskThreshold),
		NULL,
		NULL,
		-1,
	},
	{
		0x700CA5F5, 
		"CpuThreshold", 
		L"CpuThreshold", 
		sizeof("CpuThreshold")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpServiceQuery_struct, CpuThreshold),
		NULL,
		NULL,
		-1,
	},
	{
		0x4D318094, 
		"SystemUseRewrite", 
		L"SystemUseRewrite", 
		sizeof("SystemUseRewrite")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpServiceQuery_struct, SystemUseRewrite),
		NULL,
		NULL,
		-1,
	},
	{
		0xED58B04E, 
		"MemThresholdType", 
		L"MemThresholdType", 
		sizeof("MemThresholdType")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpServiceQuery_struct, MemThresholdType),
		NULL,
		NULL,
		-1,
	},
	{
		0xBC706A7A, 
		"DiskThresholdType", 
		L"DiskThresholdType", 
		sizeof("DiskThresholdType")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpServiceQuery_struct, DiskThresholdType),
		NULL,
		NULL,
		-1,
	},
	{
		0x1BDD3F4D, 
		"ServiceList", 
		L"ServiceList", 
		sizeof("ServiceList")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpServiceQuery_struct, ServiceList),
		NULL,
		&__TpServices_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsTopology_TpServiceQuery_map =
{
	0x5FC0AB1A,
	"TpServiceQueryRequest",
	L"TpServiceQueryResponse",
	sizeof("TpServiceQueryRequest")-1,
	sizeof("TpServiceQueryResponse")-1,
	SOAPMAP_FUNC,
	__CWsTopology_TpServiceQuery_entries,
	sizeof(__CWsTopology_TpServiceQuery_struct),
	8,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x11E544AA,
	"http://webservices.seisint.com/WsTopology",
	L"http://webservices.seisint.com/WsTopology",
	sizeof("http://webservices.seisint.com/WsTopology")-1
};


struct __CWsTopology_TpMachineQuery_struct
{
	BSTR Type;
	BSTR Cluster;
	BSTR OldIP;
	BSTR Path;
	BSTR Directory;
	BSTR LogDirectory;
	ArrayOfEspException Exceptions;
	bool EnablePreflightInfo;
	int MemThreshold;
	int DiskThreshold;
	int CpuThreshold;
	BSTR MemThresholdType;
	BSTR DiskThresholdType;
	TpMachine *TpMachines;
	int __TpMachines_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CWsTopology_TpMachineQuery_entries[] =
{

	{
		0x00302162, 
		"Type", 
		L"Type", 
		sizeof("Type")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpMachineQuery_struct, Type),
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
		offsetof(__CWsTopology_TpMachineQuery_struct, Cluster),
		NULL,
		NULL,
		-1,
	},
	{
		0x05D27BD8, 
		"OldIP", 
		L"OldIP", 
		sizeof("OldIP")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpMachineQuery_struct, OldIP),
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
		offsetof(__CWsTopology_TpMachineQuery_struct, Path),
		NULL,
		NULL,
		-1,
	},
	{
		0x12576735, 
		"Directory", 
		L"Directory", 
		sizeof("Directory")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpMachineQuery_struct, Directory),
		NULL,
		NULL,
		-1,
	},
	{
		0x0E0A1B77, 
		"LogDirectory", 
		L"LogDirectory", 
		sizeof("LogDirectory")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpMachineQuery_struct, LogDirectory),
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
		offsetof(__CWsTopology_TpMachineQuery_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0x5EC03CD8, 
		"EnablePreflightInfo", 
		L"EnablePreflightInfo", 
		sizeof("EnablePreflightInfo")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpMachineQuery_struct, EnablePreflightInfo),
		NULL,
		NULL,
		-1,
	},
	{
		0x7E3DF8EC, 
		"MemThreshold", 
		L"MemThreshold", 
		sizeof("MemThreshold")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpMachineQuery_struct, MemThreshold),
		NULL,
		NULL,
		-1,
	},
	{
		0x31177D18, 
		"DiskThreshold", 
		L"DiskThreshold", 
		sizeof("DiskThreshold")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpMachineQuery_struct, DiskThreshold),
		NULL,
		NULL,
		-1,
	},
	{
		0x700CA5F5, 
		"CpuThreshold", 
		L"CpuThreshold", 
		sizeof("CpuThreshold")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpMachineQuery_struct, CpuThreshold),
		NULL,
		NULL,
		-1,
	},
	{
		0xED58B04E, 
		"MemThresholdType", 
		L"MemThresholdType", 
		sizeof("MemThresholdType")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpMachineQuery_struct, MemThresholdType),
		NULL,
		NULL,
		-1,
	},
	{
		0xBC706A7A, 
		"DiskThresholdType", 
		L"DiskThresholdType", 
		sizeof("DiskThresholdType")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpMachineQuery_struct, DiskThresholdType),
		NULL,
		NULL,
		-1,
	},
	{
		0x730B3BEC, 
		"TpMachines", 
		L"TpMachines", 
		sizeof("TpMachines")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsTopology_TpMachineQuery_struct, TpMachines),
		NULL,
		&__TpMachine_map,
		13+1,
	},
	{
		0x730B3BEC,
		"__TpMachines_nSizeIs",
		L"__TpMachines_nSizeIs",
		sizeof("__TpMachines_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsTopology_TpMachineQuery_struct, __TpMachines_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsTopology_TpMachineQuery_map =
{
	0xDC47571E,
	"TpMachineQueryRequest",
	L"TpMachineQueryResponse",
	sizeof("TpMachineQueryRequest")-1,
	sizeof("TpMachineQueryResponse")-1,
	SOAPMAP_FUNC,
	__CWsTopology_TpMachineQuery_entries,
	sizeof(__CWsTopology_TpMachineQuery_struct),
	13,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x11E544AA,
	"http://webservices.seisint.com/WsTopology",
	L"http://webservices.seisint.com/WsTopology",
	sizeof("http://webservices.seisint.com/WsTopology")-1
};

extern __declspec(selectany) const _soapmap * __CWsTopology_funcs[] =
{
	&__CWsTopology_TpGetComponentFile_map,
	&__CWsTopology_TpLogFileDisplay_map,
	&__CWsTopology_TpGroupQuery_map,
	&__CWsTopology_TpXMLFile_map,
	&__CWsTopology_TpLogicalClusterQuery_map,
	&__CWsTopology_TpLogFile_map,
	&__CWsTopology_TpClusterQuery_map,
	&__CWsTopology_TpClusterInfo_map,
	&__CWsTopology_TpSwapNode_map,
	&__CWsTopology_TpSetMachineStatus_map,
	&__CWsTopology_TpServiceQuery_map,
	&__CWsTopology_TpMachineQuery_map,
	NULL
};

template <typename TClient>
inline HRESULT CWsTopologyT<TClient>::TpGetComponentFile(
		BSTR CompType, 
		BSTR CompName, 
		BSTR NetAddress, 
		BSTR Directory, 
		BSTR FileType, 
		int OsType, 
		ArrayOfEspException* Exceptions, 
		ATLSOAP_BLOB* FileContents
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( FileContents == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsTopology_TpGetComponentFile_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.CompType = CompType;
	__params.CompName = CompName;
	__params.NetAddress = NetAddress;
	__params.Directory = Directory;
	__params.FileType = FileType;
	__params.OsType = OsType;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsTopology/TpGetComponentFile\?ver_=1.06\"\r\n"));
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
	*FileContents = __params.FileContents;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsTopologyT<TClient>::TpLogFileDisplay(
		BSTR* Name, 
		BSTR* Type, 
		BSTR* StartDate, 
		BSTR* EndDate, 
		int* FirstRows, 
		int* LastRows, 
		int* FilterType, 
		bool* Reversely, 
		bool* Zip, 
		int* PageNumber, 
		bool LoadData, 
		ArrayOfEspException* Exceptions, 
		int* LastHours, 
		BSTR* LogData, 
		bool* HasDate, 
		__int64* FileSize, 
		__int64* PageFrom, 
		__int64* PageTo, 
		int* PrevPage, 
		int* NextPage, 
		int* TotalPages, 
		ATLSOAP_BLOB* thefile
	)
{
    if ( Name == NULL )
		return E_POINTER;
    if ( Type == NULL )
		return E_POINTER;
    if ( StartDate == NULL )
		return E_POINTER;
    if ( EndDate == NULL )
		return E_POINTER;
    if ( FirstRows == NULL )
		return E_POINTER;
    if ( LastRows == NULL )
		return E_POINTER;
    if ( FilterType == NULL )
		return E_POINTER;
    if ( Reversely == NULL )
		return E_POINTER;
    if ( Zip == NULL )
		return E_POINTER;
    if ( PageNumber == NULL )
		return E_POINTER;
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( LastHours == NULL )
		return E_POINTER;
    if ( LogData == NULL )
		return E_POINTER;
    if ( HasDate == NULL )
		return E_POINTER;
    if ( FileSize == NULL )
		return E_POINTER;
    if ( PageFrom == NULL )
		return E_POINTER;
    if ( PageTo == NULL )
		return E_POINTER;
    if ( PrevPage == NULL )
		return E_POINTER;
    if ( NextPage == NULL )
		return E_POINTER;
    if ( TotalPages == NULL )
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
	__CWsTopology_TpLogFileDisplay_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Name = *Name;
	__params.Type = *Type;
	__params.StartDate = *StartDate;
	__params.EndDate = *EndDate;
	__params.FirstRows = *FirstRows;
	__params.LastRows = *LastRows;
	__params.FilterType = *FilterType;
	__params.Reversely = *Reversely;
	__params.Zip = *Zip;
	__params.PageNumber = *PageNumber;
	__params.LoadData = LoadData;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsTopology/TpLogFileDisplay\?ver_=1.06\"\r\n"));
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
	*Type = __params.Type;
	*StartDate = __params.StartDate;
	*EndDate = __params.EndDate;
	*FirstRows = __params.FirstRows;
	*LastRows = __params.LastRows;
	*FilterType = __params.FilterType;
	*Reversely = __params.Reversely;
	*Zip = __params.Zip;
	*PageNumber = __params.PageNumber;
	*Exceptions = __params.Exceptions;
	*LastHours = __params.LastHours;
	*LogData = __params.LogData;
	*HasDate = __params.HasDate;
	*FileSize = __params.FileSize;
	*PageFrom = __params.PageFrom;
	*PageTo = __params.PageTo;
	*PrevPage = __params.PrevPage;
	*NextPage = __params.NextPage;
	*TotalPages = __params.TotalPages;
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
inline HRESULT CWsTopologyT<TClient>::TpGroupQuery(
		ArrayOfEspException* Exceptions, 
		TpGroup** TpGroups, int* __TpGroups_nSizeIs
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( TpGroups == NULL )
		return E_POINTER;
	if( __TpGroups_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsTopology_TpGroupQuery_struct __params;
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsTopology/TpGroupQuery\?ver_=1.06\"\r\n"));
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
	*TpGroups = __params.TpGroups;
	*__TpGroups_nSizeIs = __params.__TpGroups_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsTopologyT<TClient>::TpXMLFile(
		BSTR Name, 
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
	__CWsTopology_TpXMLFile_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Name = Name;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsTopology/TpXMLFile\?ver_=1.06\"\r\n"));
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
inline HRESULT CWsTopologyT<TClient>::TpLogicalClusterQuery(
		BSTR EclServerQueue, 
		ArrayOfEspException* Exceptions, 
		TpLogicalCluster** TpLogicalClusters, int* __TpLogicalClusters_nSizeIs
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( TpLogicalClusters == NULL )
		return E_POINTER;
	if( __TpLogicalClusters_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsTopology_TpLogicalClusterQuery_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.EclServerQueue = EclServerQueue;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsTopology/TpLogicalClusterQuery\?ver_=1.06\"\r\n"));
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
	*TpLogicalClusters = __params.TpLogicalClusters;
	*__TpLogicalClusters_nSizeIs = __params.__TpLogicalClusters_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsTopologyT<TClient>::TpLogFile(
		BSTR* Name, 
		BSTR* Type, 
		BSTR* StartDate, 
		BSTR* EndDate, 
		int* FirstRows, 
		int* LastRows, 
		int* FilterType, 
		bool* Reversely, 
		bool* Zip, 
		int* PageNumber, 
		bool LoadData, 
		ArrayOfEspException* Exceptions, 
		int* LastHours, 
		BSTR* LogData, 
		bool* HasDate, 
		__int64* FileSize, 
		__int64* PageFrom, 
		__int64* PageTo, 
		int* PrevPage, 
		int* NextPage, 
		int* TotalPages, 
		ATLSOAP_BLOB* thefile
	)
{
    if ( Name == NULL )
		return E_POINTER;
    if ( Type == NULL )
		return E_POINTER;
    if ( StartDate == NULL )
		return E_POINTER;
    if ( EndDate == NULL )
		return E_POINTER;
    if ( FirstRows == NULL )
		return E_POINTER;
    if ( LastRows == NULL )
		return E_POINTER;
    if ( FilterType == NULL )
		return E_POINTER;
    if ( Reversely == NULL )
		return E_POINTER;
    if ( Zip == NULL )
		return E_POINTER;
    if ( PageNumber == NULL )
		return E_POINTER;
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( LastHours == NULL )
		return E_POINTER;
    if ( LogData == NULL )
		return E_POINTER;
    if ( HasDate == NULL )
		return E_POINTER;
    if ( FileSize == NULL )
		return E_POINTER;
    if ( PageFrom == NULL )
		return E_POINTER;
    if ( PageTo == NULL )
		return E_POINTER;
    if ( PrevPage == NULL )
		return E_POINTER;
    if ( NextPage == NULL )
		return E_POINTER;
    if ( TotalPages == NULL )
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
	__CWsTopology_TpLogFile_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Name = *Name;
	__params.Type = *Type;
	__params.StartDate = *StartDate;
	__params.EndDate = *EndDate;
	__params.FirstRows = *FirstRows;
	__params.LastRows = *LastRows;
	__params.FilterType = *FilterType;
	__params.Reversely = *Reversely;
	__params.Zip = *Zip;
	__params.PageNumber = *PageNumber;
	__params.LoadData = LoadData;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsTopology/TpLogFile\?ver_=1.06\"\r\n"));
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
	*Type = __params.Type;
	*StartDate = __params.StartDate;
	*EndDate = __params.EndDate;
	*FirstRows = __params.FirstRows;
	*LastRows = __params.LastRows;
	*FilterType = __params.FilterType;
	*Reversely = __params.Reversely;
	*Zip = __params.Zip;
	*PageNumber = __params.PageNumber;
	*Exceptions = __params.Exceptions;
	*LastHours = __params.LastHours;
	*LogData = __params.LogData;
	*HasDate = __params.HasDate;
	*FileSize = __params.FileSize;
	*PageFrom = __params.PageFrom;
	*PageTo = __params.PageTo;
	*PrevPage = __params.PrevPage;
	*NextPage = __params.NextPage;
	*TotalPages = __params.TotalPages;
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
inline HRESULT CWsTopologyT<TClient>::TpClusterQuery(
		BSTR Type, 
		ArrayOfEspException* Exceptions, 
		TpCluster** TpClusters, int* __TpClusters_nSizeIs
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( TpClusters == NULL )
		return E_POINTER;
	if( __TpClusters_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsTopology_TpClusterQuery_struct __params;
	memset(&__params, 0x00, sizeof(__params));
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsTopology/TpClusterQuery\?ver_=1.06\"\r\n"));
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
	*TpClusters = __params.TpClusters;
	*__TpClusters_nSizeIs = __params.__TpClusters_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsTopologyT<TClient>::TpClusterInfo(
		BSTR* Name, 
		ArrayOfEspException* Exceptions, 
		BSTR* WorkUnit, 
		BSTR* Name2, 
		BSTR* WorkUnit2
	)
{
    if ( Name == NULL )
		return E_POINTER;
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( WorkUnit == NULL )
		return E_POINTER;
    if ( Name2 == NULL )
		return E_POINTER;
    if ( WorkUnit2 == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsTopology_TpClusterInfo_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Name = *Name;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsTopology/TpClusterInfo\?ver_=1.06\"\r\n"));
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
	*Exceptions = __params.Exceptions;
	*WorkUnit = __params.WorkUnit;
	*Name2 = __params.Name2;
	*WorkUnit2 = __params.WorkUnit2;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsTopologyT<TClient>::TpSwapNode(
		BSTR Cluster, 
		BSTR OldIP, 
		BSTR NewIP, 
		ArrayOfEspException* Exceptions, 
		bool* TpSwapNodeResult
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( TpSwapNodeResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsTopology_TpSwapNode_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Cluster = Cluster;
	__params.OldIP = OldIP;
	__params.NewIP = NewIP;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsTopology/TpSwapNode\?ver_=1.06\"\r\n"));
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
	*TpSwapNodeResult = __params.TpSwapNodeResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsTopologyT<TClient>::TpSetMachineStatus(
		BSTR MachinePath, 
		BSTR StatusValue, 
		ArrayOfEspException* Exceptions, 
		bool* TpSetMachineStatusResult
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( TpSetMachineStatusResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsTopology_TpSetMachineStatus_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.MachinePath = MachinePath;
	__params.StatusValue = StatusValue;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsTopology/TpSetMachineStatus\?ver_=1.06\"\r\n"));
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
	*TpSetMachineStatusResult = __params.TpSetMachineStatusResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsTopologyT<TClient>::TpServiceQuery(
		BSTR Type, 
		ArrayOfEspException* Exceptions, 
		int* MemThreshold, 
		int* DiskThreshold, 
		int* CpuThreshold, 
		bool* SystemUseRewrite, 
		BSTR* MemThresholdType, 
		BSTR* DiskThresholdType, 
		TpServices* ServiceList
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( MemThreshold == NULL )
		return E_POINTER;
    if ( DiskThreshold == NULL )
		return E_POINTER;
    if ( CpuThreshold == NULL )
		return E_POINTER;
    if ( SystemUseRewrite == NULL )
		return E_POINTER;
    if ( MemThresholdType == NULL )
		return E_POINTER;
    if ( DiskThresholdType == NULL )
		return E_POINTER;
    if ( ServiceList == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsTopology_TpServiceQuery_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Type = Type;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsTopology/TpServiceQuery\?ver_=1.06\"\r\n"));
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
	*MemThreshold = __params.MemThreshold;
	*DiskThreshold = __params.DiskThreshold;
	*CpuThreshold = __params.CpuThreshold;
	*SystemUseRewrite = __params.SystemUseRewrite;
	*MemThresholdType = __params.MemThresholdType;
	*DiskThresholdType = __params.DiskThresholdType;
	*ServiceList = __params.ServiceList;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsTopologyT<TClient>::TpMachineQuery(
		BSTR* Type, 
		BSTR* Cluster, 
		BSTR* OldIP, 
		BSTR* Path, 
		BSTR Directory, 
		BSTR* LogDirectory, 
		ArrayOfEspException* Exceptions, 
		bool* EnablePreflightInfo, 
		int* MemThreshold, 
		int* DiskThreshold, 
		int* CpuThreshold, 
		BSTR* MemThresholdType, 
		BSTR* DiskThresholdType, 
		TpMachine** TpMachines, int* __TpMachines_nSizeIs
	)
{
    if ( Type == NULL )
		return E_POINTER;
    if ( Cluster == NULL )
		return E_POINTER;
    if ( OldIP == NULL )
		return E_POINTER;
    if ( Path == NULL )
		return E_POINTER;
    if ( LogDirectory == NULL )
		return E_POINTER;
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( EnablePreflightInfo == NULL )
		return E_POINTER;
    if ( MemThreshold == NULL )
		return E_POINTER;
    if ( DiskThreshold == NULL )
		return E_POINTER;
    if ( CpuThreshold == NULL )
		return E_POINTER;
    if ( MemThresholdType == NULL )
		return E_POINTER;
    if ( DiskThresholdType == NULL )
		return E_POINTER;
    if ( TpMachines == NULL )
		return E_POINTER;
	if( __TpMachines_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsTopology_TpMachineQuery_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Type = *Type;
	__params.Cluster = *Cluster;
	__params.OldIP = *OldIP;
	__params.Path = *Path;
	__params.Directory = Directory;
	__params.LogDirectory = *LogDirectory;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsTopology/TpMachineQuery\?ver_=1.06\"\r\n"));
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
	*OldIP = __params.OldIP;
	*Path = __params.Path;
	*LogDirectory = __params.LogDirectory;
	*Exceptions = __params.Exceptions;
	*EnablePreflightInfo = __params.EnablePreflightInfo;
	*MemThreshold = __params.MemThreshold;
	*DiskThreshold = __params.DiskThreshold;
	*CpuThreshold = __params.CpuThreshold;
	*MemThresholdType = __params.MemThresholdType;
	*DiskThresholdType = __params.DiskThresholdType;
	*TpMachines = __params.TpMachines;
	*__TpMachines_nSizeIs = __params.__TpMachines_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
ATL_NOINLINE inline const _soapmap ** CWsTopologyT<TClient>::GetFunctionMap()
{
	return __CWsTopology_funcs;
}

template <typename TClient>
ATL_NOINLINE inline const _soapmap ** CWsTopologyT<TClient>::GetHeaderMap()
{
	static const _soapmapentry __CWsTopology_TpGetComponentFile_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsTopology_TpGetComponentFile_atlsoapheader_map = 
	{
		0xEC90DC46,
		"TpGetComponentFileRequest",
		L"TpGetComponentFileResponse",
		sizeof("TpGetComponentFileRequest")-1,
		sizeof("TpGetComponentFileResponse")-1,
		SOAPMAP_HEADER,
		__CWsTopology_TpGetComponentFile_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x11E544AA,
		"http://webservices.seisint.com/WsTopology",
		L"http://webservices.seisint.com/WsTopology",
		sizeof("http://webservices.seisint.com/WsTopology")-1
	};

	static const _soapmapentry __CWsTopology_TpLogFileDisplay_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsTopology_TpLogFileDisplay_atlsoapheader_map = 
	{
		0x7F63B175,
		"TpLogFileRequest",
		L"TpLogFileResponse",
		sizeof("TpLogFileRequest")-1,
		sizeof("TpLogFileResponse")-1,
		SOAPMAP_HEADER,
		__CWsTopology_TpLogFileDisplay_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x11E544AA,
		"http://webservices.seisint.com/WsTopology",
		L"http://webservices.seisint.com/WsTopology",
		sizeof("http://webservices.seisint.com/WsTopology")-1
	};

	static const _soapmapentry __CWsTopology_TpGroupQuery_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsTopology_TpGroupQuery_atlsoapheader_map = 
	{
		0xB5FBFA36,
		"TpGroupQueryRequest",
		L"TpGroupQueryResponse",
		sizeof("TpGroupQueryRequest")-1,
		sizeof("TpGroupQueryResponse")-1,
		SOAPMAP_HEADER,
		__CWsTopology_TpGroupQuery_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x11E544AA,
		"http://webservices.seisint.com/WsTopology",
		L"http://webservices.seisint.com/WsTopology",
		sizeof("http://webservices.seisint.com/WsTopology")-1
	};

	static const _soapmapentry __CWsTopology_TpXMLFile_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsTopology_TpXMLFile_atlsoapheader_map = 
	{
		0xE92F2E84,
		"TpXMLFileRequest",
		L"TpXMLFileResponse",
		sizeof("TpXMLFileRequest")-1,
		sizeof("TpXMLFileResponse")-1,
		SOAPMAP_HEADER,
		__CWsTopology_TpXMLFile_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x11E544AA,
		"http://webservices.seisint.com/WsTopology",
		L"http://webservices.seisint.com/WsTopology",
		sizeof("http://webservices.seisint.com/WsTopology")-1
	};

	static const _soapmapentry __CWsTopology_TpLogicalClusterQuery_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsTopology_TpLogicalClusterQuery_atlsoapheader_map = 
	{
		0x0850BA66,
		"TpLogicalClusterQueryRequest",
		L"TpLogicalClusterQueryResponse",
		sizeof("TpLogicalClusterQueryRequest")-1,
		sizeof("TpLogicalClusterQueryResponse")-1,
		SOAPMAP_HEADER,
		__CWsTopology_TpLogicalClusterQuery_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x11E544AA,
		"http://webservices.seisint.com/WsTopology",
		L"http://webservices.seisint.com/WsTopology",
		sizeof("http://webservices.seisint.com/WsTopology")-1
	};

	static const _soapmapentry __CWsTopology_TpLogFile_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsTopology_TpLogFile_atlsoapheader_map = 
	{
		0x7F63B175,
		"TpLogFileRequest",
		L"TpLogFileResponse",
		sizeof("TpLogFileRequest")-1,
		sizeof("TpLogFileResponse")-1,
		SOAPMAP_HEADER,
		__CWsTopology_TpLogFile_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x11E544AA,
		"http://webservices.seisint.com/WsTopology",
		L"http://webservices.seisint.com/WsTopology",
		sizeof("http://webservices.seisint.com/WsTopology")-1
	};

	static const _soapmapentry __CWsTopology_TpClusterQuery_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsTopology_TpClusterQuery_atlsoapheader_map = 
	{
		0x5415BA8B,
		"TpClusterQueryRequest",
		L"TpClusterQueryResponse",
		sizeof("TpClusterQueryRequest")-1,
		sizeof("TpClusterQueryResponse")-1,
		SOAPMAP_HEADER,
		__CWsTopology_TpClusterQuery_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x11E544AA,
		"http://webservices.seisint.com/WsTopology",
		L"http://webservices.seisint.com/WsTopology",
		sizeof("http://webservices.seisint.com/WsTopology")-1
	};

	static const _soapmapentry __CWsTopology_TpClusterInfo_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsTopology_TpClusterInfo_atlsoapheader_map = 
	{
		0x7401DF01,
		"TpClusterInfoRequest",
		L"TpClusterInfoResponse",
		sizeof("TpClusterInfoRequest")-1,
		sizeof("TpClusterInfoResponse")-1,
		SOAPMAP_HEADER,
		__CWsTopology_TpClusterInfo_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x11E544AA,
		"http://webservices.seisint.com/WsTopology",
		L"http://webservices.seisint.com/WsTopology",
		sizeof("http://webservices.seisint.com/WsTopology")-1
	};

	static const _soapmapentry __CWsTopology_TpSwapNode_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsTopology_TpSwapNode_atlsoapheader_map = 
	{
		0xCDBC9194,
		"TpSwapNodeRequest",
		L"TpSwapNodeResponse",
		sizeof("TpSwapNodeRequest")-1,
		sizeof("TpSwapNodeResponse")-1,
		SOAPMAP_HEADER,
		__CWsTopology_TpSwapNode_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x11E544AA,
		"http://webservices.seisint.com/WsTopology",
		L"http://webservices.seisint.com/WsTopology",
		sizeof("http://webservices.seisint.com/WsTopology")-1
	};

	static const _soapmapentry __CWsTopology_TpSetMachineStatus_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsTopology_TpSetMachineStatus_atlsoapheader_map = 
	{
		0xFD992A38,
		"TpSetMachineStatusRequest",
		L"TpSetMachineStatusResponse",
		sizeof("TpSetMachineStatusRequest")-1,
		sizeof("TpSetMachineStatusResponse")-1,
		SOAPMAP_HEADER,
		__CWsTopology_TpSetMachineStatus_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x11E544AA,
		"http://webservices.seisint.com/WsTopology",
		L"http://webservices.seisint.com/WsTopology",
		sizeof("http://webservices.seisint.com/WsTopology")-1
	};

	static const _soapmapentry __CWsTopology_TpServiceQuery_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsTopology_TpServiceQuery_atlsoapheader_map = 
	{
		0x5FC0AB1A,
		"TpServiceQueryRequest",
		L"TpServiceQueryResponse",
		sizeof("TpServiceQueryRequest")-1,
		sizeof("TpServiceQueryResponse")-1,
		SOAPMAP_HEADER,
		__CWsTopology_TpServiceQuery_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x11E544AA,
		"http://webservices.seisint.com/WsTopology",
		L"http://webservices.seisint.com/WsTopology",
		sizeof("http://webservices.seisint.com/WsTopology")-1
	};

	static const _soapmapentry __CWsTopology_TpMachineQuery_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsTopology_TpMachineQuery_atlsoapheader_map = 
	{
		0xDC47571E,
		"TpMachineQueryRequest",
		L"TpMachineQueryResponse",
		sizeof("TpMachineQueryRequest")-1,
		sizeof("TpMachineQueryResponse")-1,
		SOAPMAP_HEADER,
		__CWsTopology_TpMachineQuery_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x11E544AA,
		"http://webservices.seisint.com/WsTopology",
		L"http://webservices.seisint.com/WsTopology",
		sizeof("http://webservices.seisint.com/WsTopology")-1
	};


	static const _soapmap * __CWsTopology_headers[] =
	{
		&__CWsTopology_TpGetComponentFile_atlsoapheader_map,
		&__CWsTopology_TpLogFileDisplay_atlsoapheader_map,
		&__CWsTopology_TpGroupQuery_atlsoapheader_map,
		&__CWsTopology_TpXMLFile_atlsoapheader_map,
		&__CWsTopology_TpLogicalClusterQuery_atlsoapheader_map,
		&__CWsTopology_TpLogFile_atlsoapheader_map,
		&__CWsTopology_TpClusterQuery_atlsoapheader_map,
		&__CWsTopology_TpClusterInfo_atlsoapheader_map,
		&__CWsTopology_TpSwapNode_atlsoapheader_map,
		&__CWsTopology_TpSetMachineStatus_atlsoapheader_map,
		&__CWsTopology_TpServiceQuery_atlsoapheader_map,
		&__CWsTopology_TpMachineQuery_atlsoapheader_map,
		NULL
	};
	
	return __CWsTopology_headers;
}

template <typename TClient>
ATL_NOINLINE inline void * CWsTopologyT<TClient>::GetHeaderValue()
{
	return this;
}

template <typename TClient>
ATL_NOINLINE inline const wchar_t * CWsTopologyT<TClient>::GetNamespaceUri()
{
	return L"http://webservices.seisint.com/WsTopology";
}

template <typename TClient>
ATL_NOINLINE inline const char * CWsTopologyT<TClient>::GetServiceName()
{
	return NULL;
}

template <typename TClient>
ATL_NOINLINE inline const char * CWsTopologyT<TClient>::GetNamespaceUriA()
{
	return "http://webservices.seisint.com/WsTopology";
}

template <typename TClient>
ATL_NOINLINE inline HRESULT CWsTopologyT<TClient>::CallFunction(
	void *, 
	const wchar_t *, int,
	size_t)
{
	return E_NOTIMPL;
}

template <typename TClient>
ATL_NOINLINE inline HRESULT CWsTopologyT<TClient>::GetClientReader(ISAXXMLReader **ppReader)
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

} // namespace WsTopology

template<>
inline HRESULT AtlCleanupValue<WsTopology::EspException>(WsTopology::EspException *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Code);
	AtlCleanupValue(&pVal->Audience);
	AtlCleanupValue(&pVal->Source);
	AtlCleanupValue(&pVal->Message);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsTopology::EspException>(WsTopology::EspException *pVal, IAtlMemMgr *pMemMgr)
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
inline HRESULT AtlCleanupValue<WsTopology::TpCluster>(WsTopology::TpCluster *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Type);
	AtlCleanupValue(&pVal->Name);
	AtlCleanupValue(&pVal->QueueName);
	AtlCleanupValue(&pVal->Build);
	AtlCleanupValue(&pVal->Directory);
	AtlCleanupValue(&pVal->LogDirectory);
	AtlCleanupValue(&pVal->Desc);
	AtlCleanupValue(&pVal->Prefix);
	AtlCleanupValue(&pVal->Path);
	AtlCleanupValue(&pVal->DataModel);
	AtlCleanupValue(&pVal->OS);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsTopology::TpCluster>(WsTopology::TpCluster *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Type, pMemMgr);
	AtlCleanupValueEx(&pVal->Name, pMemMgr);
	AtlCleanupValueEx(&pVal->QueueName, pMemMgr);
	AtlCleanupValueEx(&pVal->Build, pMemMgr);
	AtlCleanupValueEx(&pVal->Directory, pMemMgr);
	AtlCleanupValueEx(&pVal->LogDirectory, pMemMgr);
	AtlCleanupValueEx(&pVal->Desc, pMemMgr);
	AtlCleanupValueEx(&pVal->Prefix, pMemMgr);
	AtlCleanupValueEx(&pVal->Path, pMemMgr);
	AtlCleanupValueEx(&pVal->DataModel, pMemMgr);
	AtlCleanupValueEx(&pVal->OS, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsTopology::TpMachine>(WsTopology::TpMachine *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Name);
	AtlCleanupValue(&pVal->Netaddress);
	AtlCleanupValue(&pVal->Domain);
	AtlCleanupValue(&pVal->Directory);
	AtlCleanupValue(&pVal->Type);
	AtlCleanupValue(&pVal->Available);
	AtlCleanupValue(&pVal->OS);
	AtlCleanupValue(&pVal->Path);
	AtlCleanupValue(&pVal->Port);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsTopology::TpMachine>(WsTopology::TpMachine *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Name, pMemMgr);
	AtlCleanupValueEx(&pVal->Netaddress, pMemMgr);
	AtlCleanupValueEx(&pVal->Domain, pMemMgr);
	AtlCleanupValueEx(&pVal->Directory, pMemMgr);
	AtlCleanupValueEx(&pVal->Type, pMemMgr);
	AtlCleanupValueEx(&pVal->Available, pMemMgr);
	AtlCleanupValueEx(&pVal->OS, pMemMgr);
	AtlCleanupValueEx(&pVal->Path, pMemMgr);
	AtlCleanupValueEx(&pVal->Port, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsTopology::ArrayOfTpMachine>(WsTopology::ArrayOfTpMachine *pVal)
{
	pVal;
	AtlCleanupArray(pVal->_TpMachine, pVal->___TpMachine_nSizeIs);	
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsTopology::ArrayOfTpMachine>(WsTopology::ArrayOfTpMachine *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	if (pVal->_TpMachine != NULL)
	{
		AtlCleanupArrayEx(pVal->_TpMachine, pVal->___TpMachine_nSizeIs, pMemMgr);		
		pMemMgr->Free(pVal->_TpMachine);
	}
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsTopology::TpMySqlServer>(WsTopology::TpMySqlServer *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Name);
	AtlCleanupValue(&pVal->Description);
	AtlCleanupValue(&pVal->Build);
	AtlCleanupValue(&pVal->Path);
	AtlCleanupValue(&pVal->TpMachines);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsTopology::TpMySqlServer>(WsTopology::TpMySqlServer *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Name, pMemMgr);
	AtlCleanupValueEx(&pVal->Description, pMemMgr);
	AtlCleanupValueEx(&pVal->Build, pMemMgr);
	AtlCleanupValueEx(&pVal->Path, pMemMgr);
	AtlCleanupValueEx(&pVal->TpMachines, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsTopology::TpGroup>(WsTopology::TpGroup *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Name);
	AtlCleanupValue(&pVal->Prefix);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsTopology::TpGroup>(WsTopology::TpGroup *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Name, pMemMgr);
	AtlCleanupValueEx(&pVal->Prefix, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsTopology::TpFTSlave>(WsTopology::TpFTSlave *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Name);
	AtlCleanupValue(&pVal->Description);
	AtlCleanupValue(&pVal->Build);
	AtlCleanupValue(&pVal->Path);
	AtlCleanupValue(&pVal->TpMachines);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsTopology::TpFTSlave>(WsTopology::TpFTSlave *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Name, pMemMgr);
	AtlCleanupValueEx(&pVal->Description, pMemMgr);
	AtlCleanupValueEx(&pVal->Build, pMemMgr);
	AtlCleanupValueEx(&pVal->Path, pMemMgr);
	AtlCleanupValueEx(&pVal->TpMachines, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsTopology::TpEclAgent>(WsTopology::TpEclAgent *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Name);
	AtlCleanupValue(&pVal->Description);
	AtlCleanupValue(&pVal->Build);
	AtlCleanupValue(&pVal->Type);
	AtlCleanupValue(&pVal->Path);
	AtlCleanupValue(&pVal->TpMachines);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsTopology::TpEclAgent>(WsTopology::TpEclAgent *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Name, pMemMgr);
	AtlCleanupValueEx(&pVal->Description, pMemMgr);
	AtlCleanupValueEx(&pVal->Build, pMemMgr);
	AtlCleanupValueEx(&pVal->Type, pMemMgr);
	AtlCleanupValueEx(&pVal->Path, pMemMgr);
	AtlCleanupValueEx(&pVal->TpMachines, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsTopology::TpDali>(WsTopology::TpDali *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Name);
	AtlCleanupValue(&pVal->Description);
	AtlCleanupValue(&pVal->Build);
	AtlCleanupValue(&pVal->BackupComputer);
	AtlCleanupValue(&pVal->BackupDirectory);
	AtlCleanupValue(&pVal->Type);
	AtlCleanupValue(&pVal->Path);
	AtlCleanupValue(&pVal->LogDirectory);
	AtlCleanupValue(&pVal->TpMachines);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsTopology::TpDali>(WsTopology::TpDali *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Name, pMemMgr);
	AtlCleanupValueEx(&pVal->Description, pMemMgr);
	AtlCleanupValueEx(&pVal->Build, pMemMgr);
	AtlCleanupValueEx(&pVal->BackupComputer, pMemMgr);
	AtlCleanupValueEx(&pVal->BackupDirectory, pMemMgr);
	AtlCleanupValueEx(&pVal->Type, pMemMgr);
	AtlCleanupValueEx(&pVal->Path, pMemMgr);
	AtlCleanupValueEx(&pVal->LogDirectory, pMemMgr);
	AtlCleanupValueEx(&pVal->TpMachines, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsTopology::TpEclServer>(WsTopology::TpEclServer *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Name);
	AtlCleanupValue(&pVal->Description);
	AtlCleanupValue(&pVal->Build);
	AtlCleanupValue(&pVal->Queue);
	AtlCleanupValue(&pVal->LogDirectory);
	AtlCleanupValue(&pVal->Type);
	AtlCleanupValue(&pVal->Path);
	AtlCleanupValue(&pVal->TpMachines);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsTopology::TpEclServer>(WsTopology::TpEclServer *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Name, pMemMgr);
	AtlCleanupValueEx(&pVal->Description, pMemMgr);
	AtlCleanupValueEx(&pVal->Build, pMemMgr);
	AtlCleanupValueEx(&pVal->Queue, pMemMgr);
	AtlCleanupValueEx(&pVal->LogDirectory, pMemMgr);
	AtlCleanupValueEx(&pVal->Type, pMemMgr);
	AtlCleanupValueEx(&pVal->Path, pMemMgr);
	AtlCleanupValueEx(&pVal->TpMachines, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsTopology::TpBinding>(WsTopology::TpBinding *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Name);
	AtlCleanupValue(&pVal->Service);
	AtlCleanupValue(&pVal->ServiceType);
	AtlCleanupValue(&pVal->Port);
	AtlCleanupValue(&pVal->Protocol);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsTopology::TpBinding>(WsTopology::TpBinding *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Name, pMemMgr);
	AtlCleanupValueEx(&pVal->Service, pMemMgr);
	AtlCleanupValueEx(&pVal->ServiceType, pMemMgr);
	AtlCleanupValueEx(&pVal->Port, pMemMgr);
	AtlCleanupValueEx(&pVal->Protocol, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsTopology::ArrayOfTpBinding>(WsTopology::ArrayOfTpBinding *pVal)
{
	pVal;
	AtlCleanupArray(pVal->_TpBinding, pVal->___TpBinding_nSizeIs);	
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsTopology::ArrayOfTpBinding>(WsTopology::ArrayOfTpBinding *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	if (pVal->_TpBinding != NULL)
	{
		AtlCleanupArrayEx(pVal->_TpBinding, pVal->___TpBinding_nSizeIs, pMemMgr);		
		pMemMgr->Free(pVal->_TpBinding);
	}
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsTopology::TpEspServer>(WsTopology::TpEspServer *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Name);
	AtlCleanupValue(&pVal->Description);
	AtlCleanupValue(&pVal->Build);
	AtlCleanupValue(&pVal->Type);
	AtlCleanupValue(&pVal->Path);
	AtlCleanupValue(&pVal->LogDirectory);
	AtlCleanupValue(&pVal->TpMachines);
	AtlCleanupValue(&pVal->TpBindings);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsTopology::TpEspServer>(WsTopology::TpEspServer *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Name, pMemMgr);
	AtlCleanupValueEx(&pVal->Description, pMemMgr);
	AtlCleanupValueEx(&pVal->Build, pMemMgr);
	AtlCleanupValueEx(&pVal->Type, pMemMgr);
	AtlCleanupValueEx(&pVal->Path, pMemMgr);
	AtlCleanupValueEx(&pVal->LogDirectory, pMemMgr);
	AtlCleanupValueEx(&pVal->TpMachines, pMemMgr);
	AtlCleanupValueEx(&pVal->TpBindings, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsTopology::TpDfuServer>(WsTopology::TpDfuServer *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Name);
	AtlCleanupValue(&pVal->Description);
	AtlCleanupValue(&pVal->Build);
	AtlCleanupValue(&pVal->Queue);
	AtlCleanupValue(&pVal->Type);
	AtlCleanupValue(&pVal->Path);
	AtlCleanupValue(&pVal->LogDirectory);
	AtlCleanupValue(&pVal->TpMachines);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsTopology::TpDfuServer>(WsTopology::TpDfuServer *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Name, pMemMgr);
	AtlCleanupValueEx(&pVal->Description, pMemMgr);
	AtlCleanupValueEx(&pVal->Build, pMemMgr);
	AtlCleanupValueEx(&pVal->Queue, pMemMgr);
	AtlCleanupValueEx(&pVal->Type, pMemMgr);
	AtlCleanupValueEx(&pVal->Path, pMemMgr);
	AtlCleanupValueEx(&pVal->LogDirectory, pMemMgr);
	AtlCleanupValueEx(&pVal->TpMachines, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsTopology::TpSashaServer>(WsTopology::TpSashaServer *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Name);
	AtlCleanupValue(&pVal->Description);
	AtlCleanupValue(&pVal->Build);
	AtlCleanupValue(&pVal->Path);
	AtlCleanupValue(&pVal->LogDirectory);
	AtlCleanupValue(&pVal->TpMachines);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsTopology::TpSashaServer>(WsTopology::TpSashaServer *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Name, pMemMgr);
	AtlCleanupValueEx(&pVal->Description, pMemMgr);
	AtlCleanupValueEx(&pVal->Build, pMemMgr);
	AtlCleanupValueEx(&pVal->Path, pMemMgr);
	AtlCleanupValueEx(&pVal->LogDirectory, pMemMgr);
	AtlCleanupValueEx(&pVal->TpMachines, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsTopology::TpLdapServer>(WsTopology::TpLdapServer *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Name);
	AtlCleanupValue(&pVal->Description);
	AtlCleanupValue(&pVal->Build);
	AtlCleanupValue(&pVal->Path);
	AtlCleanupValue(&pVal->TpMachines);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsTopology::TpLdapServer>(WsTopology::TpLdapServer *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Name, pMemMgr);
	AtlCleanupValueEx(&pVal->Description, pMemMgr);
	AtlCleanupValueEx(&pVal->Build, pMemMgr);
	AtlCleanupValueEx(&pVal->Path, pMemMgr);
	AtlCleanupValueEx(&pVal->TpMachines, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsTopology::TpDropZone>(WsTopology::TpDropZone *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Name);
	AtlCleanupValue(&pVal->Description);
	AtlCleanupValue(&pVal->Build);
	AtlCleanupValue(&pVal->Path);
	AtlCleanupValue(&pVal->TpMachines);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsTopology::TpDropZone>(WsTopology::TpDropZone *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Name, pMemMgr);
	AtlCleanupValueEx(&pVal->Description, pMemMgr);
	AtlCleanupValueEx(&pVal->Build, pMemMgr);
	AtlCleanupValueEx(&pVal->Path, pMemMgr);
	AtlCleanupValueEx(&pVal->TpMachines, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsTopology::TpDkcSlave>(WsTopology::TpDkcSlave *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Name);
	AtlCleanupValue(&pVal->Description);
	AtlCleanupValue(&pVal->Build);
	AtlCleanupValue(&pVal->Path);
	AtlCleanupValue(&pVal->TpMachines);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsTopology::TpDkcSlave>(WsTopology::TpDkcSlave *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Name, pMemMgr);
	AtlCleanupValueEx(&pVal->Description, pMemMgr);
	AtlCleanupValueEx(&pVal->Build, pMemMgr);
	AtlCleanupValueEx(&pVal->Path, pMemMgr);
	AtlCleanupValueEx(&pVal->TpMachines, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsTopology::TpGenesisServer>(WsTopology::TpGenesisServer *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Name);
	AtlCleanupValue(&pVal->Description);
	AtlCleanupValue(&pVal->Build);
	AtlCleanupValue(&pVal->Path);
	AtlCleanupValue(&pVal->TpMachines);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsTopology::TpGenesisServer>(WsTopology::TpGenesisServer *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Name, pMemMgr);
	AtlCleanupValueEx(&pVal->Description, pMemMgr);
	AtlCleanupValueEx(&pVal->Build, pMemMgr);
	AtlCleanupValueEx(&pVal->Path, pMemMgr);
	AtlCleanupValueEx(&pVal->TpMachines, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsTopology::TpServices>(WsTopology::TpServices *pVal)
{
	pVal;
	AtlCleanupArray(pVal->TpDalis, pVal->__TpDalis_nSizeIs);	
	AtlCleanupArray(pVal->TpEclServers, pVal->__TpEclServers_nSizeIs);	
	AtlCleanupArray(pVal->TpEclAgents, pVal->__TpEclAgents_nSizeIs);	
	AtlCleanupArray(pVal->TpEspServers, pVal->__TpEspServers_nSizeIs);	
	AtlCleanupArray(pVal->TpDfuServers, pVal->__TpDfuServers_nSizeIs);	
	AtlCleanupArray(pVal->TpSashaServers, pVal->__TpSashaServers_nSizeIs);	
	AtlCleanupArray(pVal->TpMySqlServers, pVal->__TpMySqlServers_nSizeIs);	
	AtlCleanupArray(pVal->TpLdapServers, pVal->__TpLdapServers_nSizeIs);	
	AtlCleanupArray(pVal->TpDropZones, pVal->__TpDropZones_nSizeIs);	
	AtlCleanupArray(pVal->TpFTSlaves, pVal->__TpFTSlaves_nSizeIs);	
	AtlCleanupArray(pVal->TpDkcSlaves, pVal->__TpDkcSlaves_nSizeIs);	
	AtlCleanupArray(pVal->TpGenesisServers, pVal->__TpGenesisServers_nSizeIs);	
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsTopology::TpServices>(WsTopology::TpServices *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	if (pVal->TpDalis != NULL)
	{
		AtlCleanupArrayEx(pVal->TpDalis, pVal->__TpDalis_nSizeIs, pMemMgr);		
		pMemMgr->Free(pVal->TpDalis);
	}
	if (pVal->TpEclServers != NULL)
	{
		AtlCleanupArrayEx(pVal->TpEclServers, pVal->__TpEclServers_nSizeIs, pMemMgr);		
		pMemMgr->Free(pVal->TpEclServers);
	}
	if (pVal->TpEclAgents != NULL)
	{
		AtlCleanupArrayEx(pVal->TpEclAgents, pVal->__TpEclAgents_nSizeIs, pMemMgr);		
		pMemMgr->Free(pVal->TpEclAgents);
	}
	if (pVal->TpEspServers != NULL)
	{
		AtlCleanupArrayEx(pVal->TpEspServers, pVal->__TpEspServers_nSizeIs, pMemMgr);		
		pMemMgr->Free(pVal->TpEspServers);
	}
	if (pVal->TpDfuServers != NULL)
	{
		AtlCleanupArrayEx(pVal->TpDfuServers, pVal->__TpDfuServers_nSizeIs, pMemMgr);		
		pMemMgr->Free(pVal->TpDfuServers);
	}
	if (pVal->TpSashaServers != NULL)
	{
		AtlCleanupArrayEx(pVal->TpSashaServers, pVal->__TpSashaServers_nSizeIs, pMemMgr);		
		pMemMgr->Free(pVal->TpSashaServers);
	}
	if (pVal->TpMySqlServers != NULL)
	{
		AtlCleanupArrayEx(pVal->TpMySqlServers, pVal->__TpMySqlServers_nSizeIs, pMemMgr);		
		pMemMgr->Free(pVal->TpMySqlServers);
	}
	if (pVal->TpLdapServers != NULL)
	{
		AtlCleanupArrayEx(pVal->TpLdapServers, pVal->__TpLdapServers_nSizeIs, pMemMgr);		
		pMemMgr->Free(pVal->TpLdapServers);
	}
	if (pVal->TpDropZones != NULL)
	{
		AtlCleanupArrayEx(pVal->TpDropZones, pVal->__TpDropZones_nSizeIs, pMemMgr);		
		pMemMgr->Free(pVal->TpDropZones);
	}
	if (pVal->TpFTSlaves != NULL)
	{
		AtlCleanupArrayEx(pVal->TpFTSlaves, pVal->__TpFTSlaves_nSizeIs, pMemMgr);		
		pMemMgr->Free(pVal->TpFTSlaves);
	}
	if (pVal->TpDkcSlaves != NULL)
	{
		AtlCleanupArrayEx(pVal->TpDkcSlaves, pVal->__TpDkcSlaves_nSizeIs, pMemMgr);		
		pMemMgr->Free(pVal->TpDkcSlaves);
	}
	if (pVal->TpGenesisServers != NULL)
	{
		AtlCleanupArrayEx(pVal->TpGenesisServers, pVal->__TpGenesisServers_nSizeIs, pMemMgr);		
		pMemMgr->Free(pVal->TpGenesisServers);
	}
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsTopology::ArrayOfEspException>(WsTopology::ArrayOfEspException *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Source);
	AtlCleanupArray(pVal->Exception, pVal->__Exception_nSizeIs);	
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsTopology::ArrayOfEspException>(WsTopology::ArrayOfEspException *pVal, IAtlMemMgr *pMemMgr)
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
inline HRESULT AtlCleanupValue<WsTopology::TpLogicalCluster>(WsTopology::TpLogicalCluster *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Name);
	AtlCleanupValue(&pVal->Queue);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsTopology::TpLogicalCluster>(WsTopology::TpLogicalCluster *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Name, pMemMgr);
	AtlCleanupValueEx(&pVal->Queue, pMemMgr);
	return S_OK;
}
