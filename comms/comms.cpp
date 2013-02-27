// comms.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

#include "comms.h"
#include "repository.h"
#include "dali.h"
#include "dfu.h"
#include "smc.h"
#include "topology.h"
#include "unicodefile.h"
#include "utildatetime.h"

#define MAX_CONNECTIONS 50

LONG g_CommsInitializeCount = 0;

void CommsInitialize()
{
	if (::InterlockedIncrement(&g_CommsInitializeCount) > 1)
		return;

	DWORD dwData = 0;
	DWORD dwSize = sizeof(dwData);

	InternetQueryOption(NULL, INTERNET_OPTION_MAX_CONNS_PER_SERVER, &dwData, &dwSize);

	dwData = MAX_CONNECTIONS;
	BOOL didit = InternetSetOption(NULL, INTERNET_OPTION_MAX_CONNS_PER_SERVER,&dwData, sizeof(dwData));

	didit = InternetQueryOption(NULL, INTERNET_OPTION_MAX_CONNS_PER_1_0_SERVER, &dwData, &dwSize);

	dwData = MAX_CONNECTIONS;
	didit = InternetSetOption(NULL, INTERNET_OPTION_MAX_CONNS_PER_1_0_SERVER, &dwData, sizeof(dwData));
}

#ifndef SEISINT_LIBEXPORTS
class CATLDllModule : public CAtlDllModuleT< CATLDllModule >
{
public:
	CATLDllModule()
	{
		int d = 0;
	}
	~CATLDllModule()
	{
		int d = 0;
	}
} _AtlModule;

BOOL APIENTRY DllMain( HANDLE /*hModule*/, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_PROCESS_DETACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
    return _AtlModule.DllMain(ul_reason_for_call, lpReserved); 
}
#endif

const SectionLabelDefault GLOBAL_SERVER_IP(SectionLabel(_T("Server"), _T("ServerIP")), _T(""));
const SectionLabelDefault GLOBAL_SERVER_SSL(SectionLabel(_T("Server"), _T("SSL")), false);
const SectionLabelDefault GLOBAL_SERVER_ADVANCED(SectionLabel(_T("Server"), _T("Advanced")), false);

const SectionLabelDefault GLOBAL_SERVER_ACCOUNT(SectionLabel(_T("Server"), _T("Account")), _T(""));
const SectionLabelDefault GLOBAL_SERVER_ATTRIBUTE(SectionLabel(_T("Server"), _T("Attribute")), _T(""));
const SectionLabelDefault GLOBAL_SERVER_SMC(SectionLabel(_T("Server"), _T("SMC")), _T(""));
const SectionLabelDefault GLOBAL_SERVER_FILESPRAY(SectionLabel(_T("Server"), _T("FileSpray")), _T(""));
const SectionLabelDefault GLOBAL_SERVER_DFU(SectionLabel(_T("Server"), _T("DFU")), _T(""));
const SectionLabelDefault GLOBAL_SERVER_ANALYSIS(SectionLabel(_T("Server"), _T("Analysis")), _T(""));
const SectionLabelDefault GLOBAL_ACTION_CLASSIFY(SectionLabel(_T("Action"), _T("Classify")), _T("SEEP2/EntityClassify"));
const SectionLabelDefault GLOBAL_ACTION_RESOLVE(SectionLabel(_T("Action"), _T("Resolve")), _T("SEEP2/EntityResolve"));
const SectionLabelDefault GLOBAL_ACTION_ENHANCE(SectionLabel(_T("Action"), _T("Enhance")), _T("Seep2/EntityEnhance"));

static const TCHAR topologyRoot[]	= _T("8010/WsTopology");
static const TCHAR workunitRoot[]	= _T("8010/WsWorkunits");
static const TCHAR attributeRoot[]	= _T("8145/WsAttributes");
static const TCHAR accountRoot[]	= _T("8010/Ws_Account");
static const TCHAR smcRoot[]		= _T("8010/WsSMC");
static const TCHAR filesprayRoot[]	= _T("8010/FileSpray");
static const TCHAR dfuRoot[]		= _T("8010/WsDfu");

struct ServerAndRoot
{
	const SectionLabelDefault& label; 
	const TCHAR* root;
};

#define nSERVERS 7
static ServerAndRoot Servers[nSERVERS] = 
{ 
	{GLOBAL_SERVER_TOPOLOGY,topologyRoot},
	{GLOBAL_SERVER_WORKUNIT,workunitRoot},
	{GLOBAL_SERVER_ATTRIBUTE,attributeRoot},
	{GLOBAL_SERVER_ACCOUNT,accountRoot},
	{GLOBAL_SERVER_SMC,smcRoot},
	{GLOBAL_SERVER_FILESPRAY,filesprayRoot},
	{GLOBAL_SERVER_DFU,dfuRoot}
};

COMMS_API bool VerifyCommsServerConfig()
{
	IConfig* config = GetIConfig(QUERYBUILDER_CFG);

	bool bAdvanced = config->Get(GLOBAL_SERVER_ADVANCED);

	if ( bAdvanced )
	{
		//see if any are the default, return false if that is the case
		for (unsigned i=0; i<nSERVERS; i++ )
		{
			CString server = config->Get(Servers[i].label);
			if ( server.Compare(CString(Servers[i].label.second)) == 0 )
				return false;
		}
	}
	else
	{
		bool bSSL = config->Get(GLOBAL_SERVER_SSL);
		CString serverIP = config->Get(GLOBAL_SERVER_IP);
		std::_tstring url = bSSL ? _T("https://") : _T("http://");
		url += serverIP;
		url += (bSSL ? _T(":1") : _T(":"));

		//just set them all to the proper value
		for (unsigned i=0; i<nSERVERS; i++ )
		{
			config->Set(Servers[i].label, url + Servers[i].root);
		}
	}
	return true;
}

COMMS_API void ReleaseAllSingletons()
{
	ClearRepositorySingletons(false);
	Dfu::ClearSingletons();
	Dali::ClearSingletons();
	SMC::ClearSingletons();
	Topology::ClearSingletons();
}
