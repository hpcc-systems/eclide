#pragma once

#include "logger.h"
#include "StringSet.h"

CLIB_API const TCHAR * GetSoapError(int code);

static CCRTHeap crtHeap;
//  ===========================================================================
template<typename T>
class CStructOut
{
protected:
	T m_struct;

public:
	CStructOut()
	{
		memset(&m_struct, 0, sizeof(T));
	}
	virtual ~CStructOut()
	{
		AtlCleanupValue(&m_struct);
	}
	T* operator ->()
	{
		return &m_struct;
	}
	operator T* ()
	{
		return &m_struct;
	}
	T& operator * ()
	{
		return m_struct;
	}
};
//  ===========================================================================
template<typename T>
class CStructArrayIn
{
protected:
	std::vector<T> m_arr;
	int m_count;
	std::vector<BSTR> m_buff;

public:
	CStructArrayIn()
	{
		m_arr.resize(0);
		m_count = 0;
	}

	CStructArrayIn(int count)
	{
		m_arr.resize(count);
		m_count = count;
	}

	~CStructArrayIn()
	{
		for(std::vector<BSTR>::iterator itr = m_buff.begin(); itr != m_buff.end(); ++itr)
		{
			::SysFreeString(*itr);
		}
	}

	void Resize(int count)
	{
		m_arr.resize(count);
		m_count = count;
	}

	int GetCount()
	{
		return m_count;
	}

	void SetBSTR(BSTR & bstr, const std::_tstring & val)
	{
		bstr = ::SysAllocString(CT2W(val.c_str()));
		m_buff.push_back(bstr);
	}

	T * GetItem(int i)
	{
		ATLASSERT(i >= 0 && i < m_count);
		return &m_arr[i];
	}

	T * GetArray()
	{
		return &m_arr[0];
	}
};
//  ===========================================================================
template<typename T>
class CStructArrayOut
{
protected:
	T * m_arr;
	int m_count;

public:
	CStructArrayOut() : m_arr(NULL), m_count(0)
	{
	}
	virtual ~CStructArrayOut()
	{
        AtlCleanupArrayEx(m_arr, m_count, &crtHeap);
        crtHeap.Free(m_arr);
	}

	T ** GetArrayAddress()
	{
		return &m_arr;
	}

	int * GetCountAddress()
	{
		return &m_count;
	}

	int GetCount()
	{
		return m_count;
	}

	T * GetItem(int i)
	{
		if (i >= m_count)
			return NULL;
		return (T *)(m_arr + i);
	}
};
//  ===========================================================================
template<typename T>
class CReportingStructArrayOut : public CStructArrayOut<T>
{
protected:
	std::_tstring m_file;
	int m_line;

public:
	CReportingStructArrayOut(const std::string & file, int line)
	{
		m_file = CA2T(file.c_str());
		m_line = line;
	}

	virtual ~CReportingStructArrayOut()
	{
		for(int i = 0; i < GetCount(); ++i)
		{
			T * item = GetItem(i);
			_DBGLOG(LEVEL_WARNING, (boost::_tformat(_T("ESP Exception - %1%:  %2% (%3%, %4%)")) % 
				std::_tstring(CW2T(item->Source)) % 
				std::_tstring(CW2T(item->Message)) %
				m_file %
				m_line).str().c_str());
		}
	}
};
//  ===========================================================================
class CReportingStatusOut 
{
protected:
	std::_tstring m_file;
	int m_line;

public:
	int m_statusCode;
	CComBSTR m_statusMessage;

	CReportingStatusOut(const std::string & file, int line)
	{
		m_file = CA2T(file.c_str());
		m_line = line;
		m_statusCode = -1;
		m_statusMessage = _T("");
	}

	virtual ~CReportingStatusOut()
	{
		_DBGLOG(LEVEL_WARNING, (boost::_tformat(_T("ESP Status - %1%:  %2% (%3%, %4%)")) % 
		m_statusCode % 
		std::_tstring(CW2T(m_statusMessage)) %
		m_file %
		m_line).str().c_str());
	}
};
//  ===========================================================================
template<typename T, typename U>
class CReportingEspException : public CStructOut<T>
{
protected:
	std::_tstring m_file;
	int m_line;

	std::_tstring m_source;

public:
	CReportingEspException(const std::string & file, int line) 
	{
		m_file = CA2T(file.c_str());
		m_line = line;
	}

	virtual ~CReportingEspException()
	{
		if (m_struct.Source)
			m_source = CW2T(m_struct.Source);

		for(int i = 0; i < m_struct.__Exception_nSizeIs; ++i)
		{
			if (m_struct.Exception[i].Source && m_struct.Exception[i].Message)
			{
				_DBGLOG(LEVEL_WARNING, (boost::_tformat(_T("ESP Exception - %1%:  %2% (%3%, %4%)")) % 
					std::_tstring(CW2T(m_struct.Exception[i].Source)) % 
					std::_tstring(CW2T(m_struct.Exception[i].Message)) %
					m_file %
					m_line).str().c_str());
			}
			else
			{
				//ATLASSERT(false);
			}
			//	_DBGLOG(LEVEL_WARNING, (boost::_tformat(_T("ESP Exception - %1%:  %2% (%3%, %4%)")) % 
			//		_T("GAB - Unknown Source (New Exception Interface?)") % 
			//		_T("Unknown Message (New Exception Interface?)") %
			//		m_file %
			//		m_line).str().c_str());
		}

		AtlCleanupArrayEx(m_struct.Exception, m_struct.__Exception_nSizeIs, &crtHeap);
	}
	unsigned int GetCount()
	{
		return m_struct.__Exception_nSizeIs;
	}
	U * GetItem(unsigned int i)
	{
		return &m_struct.Exception[i];
	}
	T* operator ->()
	{
		return &m_struct;
	}
	operator T* () 
	{
		return &m_struct;
	}
	T& operator * () const
	{
		return m_struct;
	}
};
//  ===========================================================================
#define ESP_EXCEPTION_LOG(T) CReportingStructArrayOut<T> exceptions(__FILE__, __LINE__)
#define ESP_EXCEPTION_LOG2(T) CReportingEspException<ArrayOf##T, T> exceptions(__FILE__, __LINE__)
#define ESP_STATUS_LOG CReportingStatusOut status(__FILE__, __LINE__)

CLIB_API BSTR * CreateBstrArray(const StdStringVector &strArr);
CLIB_API void DestroyBstrArray(BSTR * arr, unsigned arrSize);
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
	CSoapInitialize(const std::_tstring & url, const std::_tstring & user, const std::_tstring & password, int send_timeout = 0, int recv_timeout = RECV_TIMEOUT) : T()
	{
		Init(url, user, password, send_timeout, recv_timeout);
	}
	CSoapInitialize(const CString & url, const CString & user, const CString & password, int send_timeout = 0, int recv_timeout = RECV_TIMEOUT) : T()
	{
		Init(static_cast<const TCHAR *>(url), static_cast<const TCHAR *>(user), static_cast<const TCHAR *>(password), send_timeout, recv_timeout);
	}
	CSoapInitialize(const std::_tstring & url, int send_timeout = 0, int recv_timeout = RECV_TIMEOUT) : T()
	{
		CString user = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_USER);
		CString password = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_PASSWORD);
		Init(url, static_cast<const TCHAR *>(user), static_cast<const TCHAR *>(password), send_timeout, recv_timeout);
	}
	CSoapInitialize(const CString & url, int send_timeout = 0, int recv_timeout = RECV_TIMEOUT) : T()
	{
		CString user = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_USER);
		CString password = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_PASSWORD);
		Init(static_cast<const TCHAR *>(url), static_cast<const TCHAR *>(user), static_cast<const TCHAR *>(password), send_timeout, recv_timeout);
	}
	CSoapInitialize(const TCHAR * url, int send_timeout = 0, int recv_timeout = RECV_TIMEOUT) : T()
	{
		CString user = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_USER);
		CString password = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_PASSWORD);
		Init(url, static_cast<const TCHAR *>(user), static_cast<const TCHAR *>(password), send_timeout, recv_timeout);
	}

	void Init(const std::_tstring & url, const std::_tstring & user, const std::_tstring & password, int _send_timeout, int _recv_timeout)
	{
		m_url = CT2A(url.c_str());
		m_user = CT2A(user.c_str());
		m_password = CT2A(password.c_str());

		soap_init1(this, SOAP_IO_KEEPALIVE);
		max_keep_alive = 100; // at most 100 calls per keep-alive session 
		accept_timeout = 600; // optional: let server time out after ten minutes of inactivity

		send_timeout = _send_timeout;
		recv_timeout = _recv_timeout;

		soap_register_plugin(this, wininet_plugin);

		soap_endpoint = m_url.c_str();
		userid = m_user.c_str();
		passwd = m_password.c_str();
	}

	int GetClientErrorXXX()
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
};

class CStringAssign
{
	std::wstring *& m_soapString;
	std::wstring m_value;

public:
	CStringAssign(std::wstring *& soapString, const std::_tstring & val) : m_soapString(soapString)
	{
		m_value = CT2W(val.c_str());
		m_soapString = &m_value;
	}
	CStringAssign(std::wstring *& soapString, const CString & val) : m_soapString(soapString)
	{
		m_value = CT2W(val);
		m_soapString = &m_value;
	}
	CStringAssign(std::wstring *& soapString, const TCHAR * val) : m_soapString(soapString)
	{
		m_value = CT2W(val);
		m_soapString = &m_value;
	}
};
class CStringPool
{
	std::vector<std::wstring *> m_strings;

public:
	CStringPool()
	{
	}
	~CStringPool()
	{
		for(std::size_t i = 0; i < m_strings.size(); ++i)
		{
			delete m_strings[i];
			m_strings[i] = NULL;
		}
	}
	std::wstring * Create(const std::_tstring & val)
	{
		std::wstring * retVal = new std::wstring(CT2W(val.c_str()));
		m_strings.push_back(retVal);
		return retVal;
	}
	std::wstring * Create(const CString & val)
	{
		std::wstring * retVal = new std::wstring(CT2W(val));
		m_strings.push_back(retVal);
		return retVal;
	}
	std::wstring * Create(const TCHAR * val)
	{
		std::wstring * retVal = new std::wstring(CT2W(val));
		m_strings.push_back(retVal);
		return retVal;
	}
};
template<typename T>
class CStructPool
{
	std::vector<T *> m_structs;

public:
	CStructPool()
	{
	}
	~CStructPool()
	{
		for(std::size_t i = 0; i < m_structs.size(); ++i)
		{
			delete m_structs[i];
			m_structs[i] = NULL;
		}
	}
	T * Create()
	{
		T * retVal = new T();
		m_structs.push_back(retVal);
		return retVal;
	}
	T * Create(const T & val)
	{
		T * retVal = new T(val);
		m_structs.push_back(retVal);
		return retVal;
	}
};
template<typename T>
class CStringArrayAssign
{
	T m_soapStringArray;

public:
	CStringArrayAssign(T *& soapStringArray)
	{
		soapStringArray = &m_soapStringArray;
	}
	~CStringArrayAssign()
	{
		for(std::size_t i = 0; i < m_soapStringArray.Item.size(); ++i)
		{
			delete m_soapStringArray.Item[i];
			m_soapStringArray.Item[i] = NULL;
		}
	}
	void Append(const std::_tstring & val)
	{
		m_soapStringArray.Item.push_back(new std::_tstring(val));
	}
	void Append(const CString & val)
	{
		m_soapStringArray.Item.push_back(new std::_tstring(static_cast<const TCHAR *>(val)));
	}
	void Append(const TCHAR * val)
	{
		m_soapStringArray.Item.push_back(new std::_tstring(val));
	}
};

#define SAFE_ASSIGN(TARGET, SOURCE) if (SOURCE) TARGET = *SOURCE
#define SAFE_ASSIGN2CSTRING(TARGET, SOURCE) if (SOURCE) TARGET = SOURCE->c_str()
#define SAFE_ASSIGN2CSTRINGIFNOTBLANK(TARGET, SOURCE) if (SOURCE && SOURCE->length()) TARGET = SOURCE->c_str()
#define SAFE_ASSIGN_MARK_CHANGED(TARGET, SOURCE) if (SOURCE) TARGET = *SOURCE

template<typename T>
class CReportingGSoapEspException
{
protected:
	T *& m_exceptions;
	std::_tstring m_file;
	int m_line;

	std::_tstring m_source;

public:
	CReportingGSoapEspException(T *& exceptions, const std::string & file, int line) : m_exceptions(exceptions)
	{
		m_file = CA2T(file.c_str());
		m_line = line;
	}

	virtual ~CReportingGSoapEspException()
	{
		if (!m_exceptions)
			return;

		if (m_exceptions->Source)
			m_source = *m_exceptions->Source;

		for(std::size_t i = 0; i < m_exceptions->Exception.size(); ++i)
		{
			if (m_exceptions->Exception[i]->Source && m_exceptions->Exception[i]->Message)
			{
				_DBGLOG(LEVEL_WARNING, (boost::_tformat(_T("ESP Exception - %1%:  %2% (%3%, %4%)")) % 
					std::_tstring(CW2T(m_exceptions->Exception[i]->Source->c_str())) % 
					std::_tstring(CW2T(m_exceptions->Exception[i]->Message->c_str())) %
					m_file %
					m_line).str().c_str());
			}
			else
			{
				ATLASSERT(false);
			}
			//	_DBGLOG(LEVEL_WARNING, (boost::_tformat(_T("ESP Exception - %1%:  %2% (%3%, %4%)")) % 
			//		_T("GAB - Unknown Source (New Exception Interface?)") % 
			//		_T("Unknown Message (New Exception Interface?)") %
			//		m_file %
			//		m_line).str().c_str());
		}
	}
};

