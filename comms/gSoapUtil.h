#pragma once

#include "SoapUtil.h"
#include "Global.h"
#include "..\config.h"

//  ===========================================================================
void ResetNamespace(IConfig * config);
void CalcNamespace(IConfig * config, const CString & user, const CString & pw);
SOAP_NMAC struct Namespace * GetNamespace(const std::_tstring & url);
//  ===========================================================================
#define RECV_TIMEOUT 3 * 60 
template<typename T>
class CSoapInitialize : public T
{
protected:
	std::string m_url;
	std::string m_user;
	std::string m_password;
	std::_tstring m_clientError;

public:
	CSoapInitialize(const std::_tstring & url, const std::_tstring & user, const std::_tstring & password) : T()
	{
		Init(url, user, password);
	}
	CSoapInitialize(const std::_tstring & url) : T()
	{
		CString user = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_USER);
		CString password = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_PASSWORD);
		Init(url, static_cast<const TCHAR *>(user), static_cast<const TCHAR *>(password));
	}
	CSoapInitialize(const CString & url, const CString & user, const CString & password, int _connect_timeout, int _send_timeout = 0, int _recv_timeout = RECV_TIMEOUT) : T()
	{
		Init(url, user, password, _connect_timeout, _send_timeout, _recv_timeout);
	}
	CSoapInitialize(const CString & url, const CString & user, const CString & password) : T()
	{
		Init(url, user, password);
	}
	CSoapInitialize(const CString & url) : T()
	{
		CString user = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_USER);
		CString password = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_PASSWORD);
		Init(url, user, password);
	}

	//CSoapInitialize(const std::_tstring & url, int send_timeout = 0, int recv_timeout = RECV_TIMEOUT) : T()
	//{
	//	CString user = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_USER);
	//	CString password = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_PASSWORD);
	//	Init(url, static_cast<const TCHAR *>(user), static_cast<const TCHAR *>(password), send_timeout, recv_timeout);
	//}
	//CSoapInitialize(const CString & url, int send_timeout = 0, int recv_timeout = RECV_TIMEOUT) : T()
	//{
	//	CString user = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_USER);
	//	CString password = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_PASSWORD);
	//	Init(static_cast<const TCHAR *>(url), static_cast<const TCHAR *>(user), static_cast<const TCHAR *>(password), send_timeout, recv_timeout);
	//}
	//CSoapInitialize(const TCHAR * url, int send_timeout = 0, int recv_timeout = RECV_TIMEOUT) : T()
	//{
	//	CString user = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_USER);
	//	CString password = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_PASSWORD);
	//	Init(url, static_cast<const TCHAR *>(user), static_cast<const TCHAR *>(password), send_timeout, recv_timeout);
	//}

	int GetClientErrorCode()
	{
		return error;
	}

	const TCHAR *GetClientErrorMsg()
	{
		std::stringstream str;
		T::soap_stream_fault(str);
		m_clientError = CA2T(str.str().c_str());
		return m_clientError.c_str();
	}
private:
	void Init(const std::_tstring & url, const std::_tstring & user, const std::_tstring & password, int _connect_timeout = 0, int _send_timeout = 0, int _recv_timeout = RECV_TIMEOUT)
	{
		m_url = CT2A(url.c_str());
		m_url += "?ClientID=ECLIDE-";
		m_url += BUILD_TAG;
		m_user = CT2A(user.c_str());
		m_password = CT2A(password.c_str());

		soap_init1(this, SOAP_IO_KEEPALIVE);
		max_keep_alive = 100; // at most 100 calls per keep-alive session 
		accept_timeout = 600; // optional: let server time out after ten minutes of inactivity

		connect_timeout = _connect_timeout;
		send_timeout = _send_timeout;
		recv_timeout = _recv_timeout;

		soap_register_plugin(this, wininet_plugin);

		soap_endpoint = m_url.c_str();
		userid = m_user.c_str();
		passwd = m_password.c_str();

		namespaces = GetNamespace(url);
	}

	void Init(const CString & url, const CString & user, const CString & password, int _connect_timeout = 0, int _send_timeout = 0, int _recv_timeout = RECV_TIMEOUT)
	{
		Init(std::_tstring(static_cast<const TCHAR *>(url)), static_cast<const TCHAR *>(user), static_cast<const TCHAR *>(password), _connect_timeout, _send_timeout, _recv_timeout);
	}
};

