#include "StdAfx.h"

#include "gSoapUtil.h"
#include "thread.h"

//  ===========================================================================
typedef std::vector<SOAP_NMAC struct Namespace * > NamespaceVector;

clib::recursive_mutex g_namespaceMutex;
NamespaceVector g_namespaces;
SOAP_NMAC struct Namespace * g_workingNamespace = NULL;
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
	g_workingNamespace = NULL;
}

bool testCall(const std::string & url, const std::string & user, const std::string & password, SOAP_NMAC struct Namespace * _namespace)
{
	ws_USCOREaccountServiceSoapProxy server;
	soap_init1(&server, SOAP_IO_KEEPALIVE);
	server.max_keep_alive = 100; // at most 100 calls per keep-alive session 
	server.accept_timeout = 600; // optional: let server time out after ten minutes of inactivity

	server.namespaces = _namespace;
	server.connect_timeout = 5;

	soap_register_plugin(&server, wininet_plugin);

	server.soap_endpoint = url.c_str();
	server.userid = user.c_str();
	server.passwd = password.c_str();

	_ns1__VerifyUserRequest request;
	_ns1__VerifyUserResponse response;
	switch (server.VerifyUser(&request, &response))
	{
	case 29:
		return false;

	case 0:
	case 401:	//  401 Is invallid crentials and is returned before the namespace is tested...
		g_workingNamespace = _namespace;
		break;
	}
	return true;
}

SOAP_NMAC struct Namespace * GetNamespace(const std::string & url, const std::string & user, const std::string & pw)
{
	clib::recursive_mutex::scoped_lock proc(g_namespaceMutex);
	if (g_namespaces.empty())
	{
		g_namespaces.push_back(namespacesOSS);
		g_namespaces.push_back(namespaces);
	}

	for(NamespaceVector::const_iterator itr = g_namespaces.begin(); itr != g_namespaces.end(); ++itr)
	{
		if (!testCall(url, user, pw, *itr))
			break;

		if (g_workingNamespace)
			break;
	}

	return g_workingNamespace;
}
//  ===========================================================================
#if _COMMS_VER == 68200
#  include "68200/WebService.cpp"
#elif _COMMS_VER == 68201
#  include "68201/WebService.cpp"
#endif
