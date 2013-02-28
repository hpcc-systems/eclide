#include "StdAfx.h"
#include "account.h"

#include "gSoapUtil.h"
#include "SMCVersion.h"
#include "EclCC.h"

#define ESP_EXCEPTION_LOG3(T) CReportingGSoapEspException<ns1__ArrayOfEspException> exceptions(T, __FILE__, __LINE__)

#if _COMMS_VER >= 68200
COMMS_API bool ChangePassword(const CString &serverIP, const CString &user, const CString &oldPassword, const CString &newPassword, const CString &confirmPassword, int &retCode, CString &retMsg)
{
	CSoapInitialize<ws_USCOREaccountServiceSoapProxy> server(serverIP, user, oldPassword);

	_ns1__UpdateUserRequest request;
	CStringAssign username(request.username, user);
	CStringAssign oldpass(request.oldpass, oldPassword);
	CStringAssign newpass1(request.newpass1, newPassword);
	CStringAssign newpass2(request.newpass2, confirmPassword);

	_ns1__UpdateUserResponse response;
	ESP_EXCEPTION_LOG3(response.Exceptions);
	if (server.UpdateUser(&request, &response) == SOAP_OK)
	{
		SAFE_ASSIGN(retCode, response.retcode);
		if (response.message)
			retMsg = response.message->c_str();
		return true;
	}
	else
	{
		_DBGLOG(serverIP, LEVEL_WARNING, server.GetClientErrorMsg());
		retCode = server.GetClientErrorCode();
		retMsg = _T("Change Password SOAP Failure");
	}
	return false;
}

COMMS_API bool VerifyUser(IConfig * config, const CString &user, const CString &password, int &retCode, CString &retMsg)
{
	CommsInitialize();

	ResetNamespace(config);
	CalcNamespace(config, user, password);
	CSoapInitialize<ws_USCOREaccountServiceSoapProxy> server(config->Get(GLOBAL_SERVER_ACCOUNT), user, password);
	if (server.namespaces == NULL)	//  No Server  ---
	{
		retCode = 1003;
		retMsg = _T("Unable to communicate with server.");
		return false;
	}

	_ns1__VerifyUserRequest request;
	CStringAssign application(request.application, global::GetApplicationName());
	CComPtr<SMC::IVersion> version = GetCommsVersion();
	CStringAssign versionStr(request.version, version->GetString());

	_ns1__VerifyUserResponse response;
	ESP_EXCEPTION_LOG3(response.Exceptions);
	if (server.VerifyUser(&request, &response) == SOAP_OK)
	{
		SAFE_ASSIGN(retCode, response.retcode);
		CString responseMessage;
#if _COMMS_VER < 300800
		if (response.message)
			responseMessage = response.message->c_str();
#endif
		if (responseMessage.IsEmpty() && response.Exceptions)
		{
			for (unsigned int i = 0; i < response.Exceptions->Exception.size(); ++i)
			{
				if (response.Exceptions->Exception[i]->Message)
				{
					if (!retMsg.IsEmpty())
						retMsg += _T("\r\n");

					retMsg += response.Exceptions->Exception[i]->Message->c_str();
				}
			}
		}
		return true;
	}
	else
	{
		_DBGLOG(config->Get(GLOBAL_SERVER_ACCOUNT), LEVEL_WARNING, server.GetClientErrorMsg());
		retCode = server.GetClientErrorCode();
		if (retCode == 401)
			retMsg = _T("Invalid User ID/Password");	//Parse error _could_ mean bad password for Verify User
		else
			retMsg = server.GetClientErrorMsg();
	}
	ResetNamespace(config);
	return false;
}
#else
#if _COMMS_VER < 491
#elif _COMMS_VER < 700000
using namespace ws_account;
typedef Cws_accountT<CCustomActionSecureSoapSocketClient> ServerOldT;
typedef Cws_accountT<CCustomActionSecureSoapSocketClient> ServerT;
#else
typedef ws_account::Cws_accountT<CCustomActionSecureSoapSocketClient> ServerOldT;
using namespace WsUsers;
typedef CWsUsersT<CCustomActionSecureSoapSocketClient> ServerT;
#endif

COMMS_API bool ChangePassword(const CString &serverIP, const CString &user, const CString &oldPassword, const CString &newPassword, const CString &confirmPassword, int &pRetCode, CString &pRetMsg)
{
	CComInitialize com;
#if _COMMS_VER < 491
	pRetMsg = _T("Change Password unsupported in server build 471");
#else
	ServerT server;
	server.SetUrl(serverIP);
	server.SetUser(user);
	server.SetPassword(oldPassword);

	CComBSTR userID = user;
	CComBSTR oldPW = oldPassword;
	CComBSTR newPW = newPassword;
	CComBSTR confirmPW = confirmPassword;

	int retCode;
	CComBSTR retMsg;
#if _COMMS_VER < 52501
	if (server.UpdateUser(userID, oldPW, newPW, confirmPW, &retCode, &retMsg) == S_OK)
#elif _COMMS_VER < 67205
	ESP_EXCEPTION_LOG(EspException);
	if (server.UpdateUser(userID, oldPW, newPW, confirmPW, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &retCode, &retMsg) == S_OK)
	{
		pRetCode = retCode;
		pRetMsg = retMsg;
		return true;
	}
#elif _COMMS_VER < 700000
	ESP_EXCEPTION_LOG2(EspException);
	if (server.UpdateUser(userID, oldPW, newPW, confirmPW, exceptions, &retCode, &retMsg) == S_OK)
	{
		pRetCode = retCode;
		pRetMsg = retMsg;
		return true;
	}
#else
	ESP_STATUS_LOG;

	if (server.UpdatePassword(oldPW, newPW, confirmPW, &status.m_statusCode, &status.m_statusMessage) == S_OK)
	{
		pRetCode = status.m_statusCode;
		pRetMsg = status.m_statusMessage;
		return true;
	}
#endif
	else
	{
		_DBGLOG(m_url, LEVEL_WARNING, server.GetClientError());
		pRetCode = server.GetClientError();
		pRetMsg = _T("Change Password SOAP Failure");
	}
#endif
	return false;
}

COMMS_API bool VerifyUser(const CString &serverIP, const CString &user, const CString &password, int &pRetCode, CString &pRetMsg)
{
	CComInitialize com;
	if (user.IsEmpty())
	{
		pRetCode = 1001;
		pRetMsg = _T("User ID cannot be blank.");
		return false;
	}
	if (password.IsEmpty())
	{
		pRetCode = 1002;
		pRetMsg = _T("Password cannot be blank.");
		return false;
	}
#if _COMMS_VER < 505
	pRetMsg = _T("Verify User unsupported in this server version.");
	return true;
#else
	CComPtr<ISAXXMLReader> reader;
	reader.CoCreateInstance(CLSID_SAXXMLReader30, NULL, CLSCTX_INPROC_SERVER);
	ServerOldT server(reader);
	CString accountServer;
	accountServer = serverIP;
	server.SetUrl(accountServer);
	server.SetUser(user);
	server.SetPassword(password);
	int retCode;
	CComBSTR retMsg;
#if _COMMS_VER < 52501
	if (server.VerifyUser(&retCode, &retMsg) == S_OK)
#elif _COMMS_VER < 53313
	CReportingStructArrayOut<EspException> exceptions(_T("VerifyUser"));
	if (server.VerifyUser(exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &retCode, &retMsg) == S_OK)
#elif _COMMS_VER < 67205
	CComBSTR bstrApplication = CT2W(global::GetApplicationName(), CP_UTF8);
	CComPtr<SMC::IVersion> version = GetCommsVersion();
	CComBSTR bstrVersion = CT2W(version->GetString(), CP_UTF8);
	ESP_EXCEPTION_LOG(ws_account::EspException);
	if (server.VerifyUser(bstrApplication, bstrVersion, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &retCode, &retMsg) == S_OK)
#else
	CComBSTR bstrApplication = CT2W(global::GetApplicationName(), CP_UTF8);
	CComPtr<SMC::IVersion> version = GetCommsVersion();
	CComBSTR bstrVersion = CT2W(version->GetString(), CP_UTF8);
	ESP_EXCEPTION_LOG2(EspException);
	if (server.VerifyUser(bstrApplication, bstrVersion, exceptions, &retCode, &retMsg) == S_OK)
#endif
	{
		pRetCode = retCode;
		pRetMsg = retMsg;
		return true;
	}
	else
	{
		_DBGLOG(m_url, LEVEL_WARNING, server.GetClientError());
		pRetCode = server.GetClientError();
		if (pRetCode == 10)
			pRetMsg = _T("Invalid User ID/Password");	//Parse error _could_ mean bad password for Verify User
		else
			pRetMsg = GetSoapError(pRetCode);
	}
#endif
	return false;
}
#endif