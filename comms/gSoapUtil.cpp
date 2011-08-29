#include "StdAfx.h"

#include "gSoapUtil.h"
#include "thread.h"

//  ===========================================================================
SOAP_NMAC struct Namespace * g_WorkingNamespace = NULL;
clib::recursive_mutex g_namespaceMutex;
//  ===========================================================================
SOAP_NMAC struct Namespace namespacesOSS[] =
{
	{"SOAP-ENV", "http://schemas.xmlsoap.org/soap/envelope/", "http://www.w3.org/*/soap-envelope", NULL},
	{"SOAP-ENC", "http://schemas.xmlsoap.org/soap/encoding/", "http://www.w3.org/*/soap-encoding", NULL},
	{"xsi", "http://www.w3.org/2001/XMLSchema-instance", "http://www.w3.org/*/XMLSchema-instance", NULL},
	{"xsd", "http://www.w3.org/2001/XMLSchema", "http://www.w3.org/*/XMLSchema", NULL},
	{"ns1", "urn:hpccsystems:ws:ws_account", NULL, NULL},
	{"ns2", "urn:hpccsystems:ws:wsattributes", NULL, NULL},
	{"ns3", "urn:hpccsystems:ws:wsdfu", NULL, NULL},
	{"ns4", "urn:hpccsystems:ws:wssmc", NULL, NULL},
	{"ns5", "urn:hpccsystems:ws:wstopology", NULL, NULL},
	{"ns6", "urn:hpccsystems:ws:wsworkunits", NULL, NULL},
	{NULL, NULL, NULL, NULL}
};

SOAP_NMAC struct Namespace namespaces[] =
{
	{"SOAP-ENV", "http://schemas.xmlsoap.org/soap/envelope/", "http://www.w3.org/*/soap-envelope", NULL},
	{"SOAP-ENC", "http://schemas.xmlsoap.org/soap/encoding/", "http://www.w3.org/*/soap-encoding", NULL},
	{"xsi", "http://www.w3.org/2001/XMLSchema-instance", "http://www.w3.org/*/XMLSchema-instance", NULL},
	{"xsd", "http://www.w3.org/2001/XMLSchema", "http://www.w3.org/*/XMLSchema", NULL},
	{"ns1", "http://webservices.seisint.com/ws_account", NULL, NULL},
	{"ns2", "http://webservices.seisint.com/WsAttributes", NULL, NULL},
	{"ns3", "http://webservices.seisint.com/WsDfu", NULL, NULL},
	{"ns4", "http://webservices.seisint.com/WsSMC", NULL, NULL},
	{"ns5", "http://webservices.seisint.com/WsTopology", NULL, NULL},
	{"ns6", "http://webservices.seisint.com/WsWorkunits", NULL, NULL},
	{NULL, NULL, NULL, NULL}
};
//  ===========================================================================
void ResetNamespace()
{
	clib::recursive_mutex::scoped_lock proc(g_namespaceMutex);
	g_WorkingNamespace = NULL;
}

SOAP_NMAC struct Namespace * GetNamespace(const std::_tstring & url)
{
	clib::recursive_mutex::scoped_lock proc(g_namespaceMutex);
	if (!g_WorkingNamespace)
	{
		g_WorkingNamespace = namespacesOSS;
		CSoapInitialize<ws_USCOREaccountServiceSoapProxy> server(url, 0, 0);
		_ns1__VerifyUserRequest request;
		_ns1__VerifyUserResponse response;
		if (server.VerifyUser(&request, &response) != SOAP_OK)
			g_WorkingNamespace = namespaces;
	}
	return g_WorkingNamespace;
}
//  ===========================================================================
#if _COMMS_VER == 68200
#  include "68200/WebService.cpp"
#elif _COMMS_VER == 68201
#  include "68201/WebService.cpp"
#endif
