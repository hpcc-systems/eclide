//
// sproxy.exe generated file
// do not modify this file
//
// Created: 10/28/2009@09:14:31
//

#pragma once


#if !defined(_WIN32_WINDOWS) && !defined(_WIN32_WINNT) && !defined(_WIN32_WCE)
#pragma message("warning: defining _WIN32_WINDOWS = 0x0410")
#define _WIN32_WINDOWS 0x0410
#endif

#include <atlsoap.h>

namespace ws_account
{

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

template <typename TClient = CSoapSocketClientT<> >
class Cws_accountT : 
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

	Cws_accountT(ISAXXMLReader *pReader = NULL)
		:TClient(_T("http://10.173.88.201:8010/ws_account\?ver_=0"))
	{
		SetClient(true);
		SetReader(pReader);
	}
	
	~Cws_accountT()
	{
		Uninitialize();
	}
	
	void Uninitialize()
	{
		UninitializeSOAP();
	}	

	HRESULT WhoAmI(
		ArrayOfEspException* Exceptions, 
		BSTR* username
	);

	HRESULT VerifyUser(
		BSTR application, 
		BSTR version, 
		ArrayOfEspException* Exceptions, 
		int* retcode, 
		BSTR* message
	);

	HRESULT UpdateUserInput(
		ArrayOfEspException* Exceptions, 
		BSTR* username
	);

	HRESULT UpdateUser(
		BSTR username, 
		BSTR oldpass, 
		BSTR newpass1, 
		BSTR newpass2, 
		ArrayOfEspException* Exceptions, 
		int* retcode, 
		BSTR* message
	);
};

typedef Cws_accountT<> Cws_account;

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
	0x6531C539,
	"http://webservices.seisint.com/ws_account",
	L"http://webservices.seisint.com/ws_account",
	sizeof("http://webservices.seisint.com/ws_account")-1
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
	0x6531C539,
	"http://webservices.seisint.com/ws_account",
	L"http://webservices.seisint.com/ws_account",
	sizeof("http://webservices.seisint.com/ws_account")-1
};
}

struct __Cws_account_WhoAmI_struct
{
	ArrayOfEspException Exceptions;
	BSTR username;
};

extern __declspec(selectany) const _soapmapentry __Cws_account_WhoAmI_entries[] =
{

	{
		0x7B0DBD02, 
		"Exceptions", 
		L"Exceptions", 
		sizeof("Exceptions")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__Cws_account_WhoAmI_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0x82658340, 
		"username", 
		L"username", 
		sizeof("username")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__Cws_account_WhoAmI_struct, username),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __Cws_account_WhoAmI_map =
{
	0x87B5DB34,
	"WhoAmIRequest",
	L"WhoAmIResponse",
	sizeof("WhoAmIRequest")-1,
	sizeof("WhoAmIResponse")-1,
	SOAPMAP_FUNC,
	__Cws_account_WhoAmI_entries,
	sizeof(__Cws_account_WhoAmI_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x6531C539,
	"http://webservices.seisint.com/ws_account",
	L"http://webservices.seisint.com/ws_account",
	sizeof("http://webservices.seisint.com/ws_account")-1
};


struct __Cws_account_VerifyUser_struct
{
	BSTR application;
	BSTR version;
	ArrayOfEspException Exceptions;
	int retcode;
	BSTR message;
};

extern __declspec(selectany) const _soapmapentry __Cws_account_VerifyUser_entries[] =
{

	{
		0x6FA5BF14, 
		"application", 
		L"application", 
		sizeof("application")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__Cws_account_VerifyUser_struct, application),
		NULL,
		NULL,
		-1,
	},
	{
		0x6F40CEE6, 
		"version", 
		L"version", 
		sizeof("version")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__Cws_account_VerifyUser_struct, version),
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
		offsetof(__Cws_account_VerifyUser_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0x3B735EE6, 
		"retcode", 
		L"retcode", 
		sizeof("retcode")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__Cws_account_VerifyUser_struct, retcode),
		NULL,
		NULL,
		-1,
	},
	{
		0xBA86A185, 
		"message", 
		L"message", 
		sizeof("message")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__Cws_account_VerifyUser_struct, message),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __Cws_account_VerifyUser_map =
{
	0x3422DC03,
	"VerifyUserRequest",
	L"VerifyUserResponse",
	sizeof("VerifyUserRequest")-1,
	sizeof("VerifyUserResponse")-1,
	SOAPMAP_FUNC,
	__Cws_account_VerifyUser_entries,
	sizeof(__Cws_account_VerifyUser_struct),
	3,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x6531C539,
	"http://webservices.seisint.com/ws_account",
	L"http://webservices.seisint.com/ws_account",
	sizeof("http://webservices.seisint.com/ws_account")-1
};


struct __Cws_account_UpdateUserInput_struct
{
	ArrayOfEspException Exceptions;
	BSTR username;
};

extern __declspec(selectany) const _soapmapentry __Cws_account_UpdateUserInput_entries[] =
{

	{
		0x7B0DBD02, 
		"Exceptions", 
		L"Exceptions", 
		sizeof("Exceptions")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__Cws_account_UpdateUserInput_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0x82658340, 
		"username", 
		L"username", 
		sizeof("username")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__Cws_account_UpdateUserInput_struct, username),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __Cws_account_UpdateUserInput_map =
{
	0x8B782801,
	"UpdateUserInputRequest",
	L"UpdateUserInputResponse",
	sizeof("UpdateUserInputRequest")-1,
	sizeof("UpdateUserInputResponse")-1,
	SOAPMAP_FUNC,
	__Cws_account_UpdateUserInput_entries,
	sizeof(__Cws_account_UpdateUserInput_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x6531C539,
	"http://webservices.seisint.com/ws_account",
	L"http://webservices.seisint.com/ws_account",
	sizeof("http://webservices.seisint.com/ws_account")-1
};


struct __Cws_account_UpdateUser_struct
{
	BSTR username;
	BSTR oldpass;
	BSTR newpass1;
	BSTR newpass2;
	ArrayOfEspException Exceptions;
	int retcode;
	BSTR message;
};

extern __declspec(selectany) const _soapmapentry __Cws_account_UpdateUser_entries[] =
{

	{
		0x82658340, 
		"username", 
		L"username", 
		sizeof("username")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__Cws_account_UpdateUser_struct, username),
		NULL,
		NULL,
		-1,
	},
	{
		0x63BE2456, 
		"oldpass", 
		L"oldpass", 
		sizeof("oldpass")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__Cws_account_UpdateUser_struct, oldpass),
		NULL,
		NULL,
		-1,
	},
	{
		0x00BB3512, 
		"newpass1", 
		L"newpass1", 
		sizeof("newpass1")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__Cws_account_UpdateUser_struct, newpass1),
		NULL,
		NULL,
		-1,
	},
	{
		0x00BB3513, 
		"newpass2", 
		L"newpass2", 
		sizeof("newpass2")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__Cws_account_UpdateUser_struct, newpass2),
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
		offsetof(__Cws_account_UpdateUser_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0x3B735EE6, 
		"retcode", 
		L"retcode", 
		sizeof("retcode")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__Cws_account_UpdateUser_struct, retcode),
		NULL,
		NULL,
		-1,
	},
	{
		0xBA86A185, 
		"message", 
		L"message", 
		sizeof("message")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__Cws_account_UpdateUser_struct, message),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __Cws_account_UpdateUser_map =
{
	0xDA5C5851,
	"UpdateUserRequest",
	L"UpdateUserResponse",
	sizeof("UpdateUserRequest")-1,
	sizeof("UpdateUserResponse")-1,
	SOAPMAP_FUNC,
	__Cws_account_UpdateUser_entries,
	sizeof(__Cws_account_UpdateUser_struct),
	3,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x6531C539,
	"http://webservices.seisint.com/ws_account",
	L"http://webservices.seisint.com/ws_account",
	sizeof("http://webservices.seisint.com/ws_account")-1
};

extern __declspec(selectany) const _soapmap * __Cws_account_funcs[] =
{
	&__Cws_account_WhoAmI_map,
	&__Cws_account_VerifyUser_map,
	&__Cws_account_UpdateUserInput_map,
	&__Cws_account_UpdateUser_map,
	NULL
};

template <typename TClient>
inline HRESULT Cws_accountT<TClient>::WhoAmI(
		ArrayOfEspException* Exceptions, 
		BSTR* username
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( username == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__Cws_account_WhoAmI_struct __params;
	memset(&__params, 0x00, sizeof(__params));

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"ws_account/WhoAmI\?ver_=0\"\r\n"));
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
	*username = __params.username;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT Cws_accountT<TClient>::VerifyUser(
		BSTR application, 
		BSTR version, 
		ArrayOfEspException* Exceptions, 
		int* retcode, 
		BSTR* message
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( retcode == NULL )
		return E_POINTER;
    if ( message == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__Cws_account_VerifyUser_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.application = application;
	__params.version = version;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"ws_account/VerifyUser\?ver_=0\"\r\n"));
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
	*retcode = __params.retcode;
	*message = __params.message;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT Cws_accountT<TClient>::UpdateUserInput(
		ArrayOfEspException* Exceptions, 
		BSTR* username
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( username == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__Cws_account_UpdateUserInput_struct __params;
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"ws_account/UpdateUserInput\?ver_=0\"\r\n"));
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
	*username = __params.username;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT Cws_accountT<TClient>::UpdateUser(
		BSTR username, 
		BSTR oldpass, 
		BSTR newpass1, 
		BSTR newpass2, 
		ArrayOfEspException* Exceptions, 
		int* retcode, 
		BSTR* message
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( retcode == NULL )
		return E_POINTER;
    if ( message == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__Cws_account_UpdateUser_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.username = username;
	__params.oldpass = oldpass;
	__params.newpass1 = newpass1;
	__params.newpass2 = newpass2;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"ws_account/UpdateUser\?ver_=0\"\r\n"));
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
	*retcode = __params.retcode;
	*message = __params.message;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
ATL_NOINLINE inline const _soapmap ** Cws_accountT<TClient>::GetFunctionMap()
{
	return __Cws_account_funcs;
}

template <typename TClient>
ATL_NOINLINE inline const _soapmap ** Cws_accountT<TClient>::GetHeaderMap()
{
	static const _soapmapentry __Cws_account_WhoAmI_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __Cws_account_WhoAmI_atlsoapheader_map = 
	{
		0x87B5DB34,
		"WhoAmIRequest",
		L"WhoAmIResponse",
		sizeof("WhoAmIRequest")-1,
		sizeof("WhoAmIResponse")-1,
		SOAPMAP_HEADER,
		__Cws_account_WhoAmI_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x6531C539,
		"http://webservices.seisint.com/ws_account",
		L"http://webservices.seisint.com/ws_account",
		sizeof("http://webservices.seisint.com/ws_account")-1
	};

	static const _soapmapentry __Cws_account_VerifyUser_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __Cws_account_VerifyUser_atlsoapheader_map = 
	{
		0x3422DC03,
		"VerifyUserRequest",
		L"VerifyUserResponse",
		sizeof("VerifyUserRequest")-1,
		sizeof("VerifyUserResponse")-1,
		SOAPMAP_HEADER,
		__Cws_account_VerifyUser_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x6531C539,
		"http://webservices.seisint.com/ws_account",
		L"http://webservices.seisint.com/ws_account",
		sizeof("http://webservices.seisint.com/ws_account")-1
	};

	static const _soapmapentry __Cws_account_UpdateUserInput_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __Cws_account_UpdateUserInput_atlsoapheader_map = 
	{
		0x8B782801,
		"UpdateUserInputRequest",
		L"UpdateUserInputResponse",
		sizeof("UpdateUserInputRequest")-1,
		sizeof("UpdateUserInputResponse")-1,
		SOAPMAP_HEADER,
		__Cws_account_UpdateUserInput_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x6531C539,
		"http://webservices.seisint.com/ws_account",
		L"http://webservices.seisint.com/ws_account",
		sizeof("http://webservices.seisint.com/ws_account")-1
	};

	static const _soapmapentry __Cws_account_UpdateUser_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __Cws_account_UpdateUser_atlsoapheader_map = 
	{
		0xDA5C5851,
		"UpdateUserRequest",
		L"UpdateUserResponse",
		sizeof("UpdateUserRequest")-1,
		sizeof("UpdateUserResponse")-1,
		SOAPMAP_HEADER,
		__Cws_account_UpdateUser_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x6531C539,
		"http://webservices.seisint.com/ws_account",
		L"http://webservices.seisint.com/ws_account",
		sizeof("http://webservices.seisint.com/ws_account")-1
	};


	static const _soapmap * __Cws_account_headers[] =
	{
		&__Cws_account_WhoAmI_atlsoapheader_map,
		&__Cws_account_VerifyUser_atlsoapheader_map,
		&__Cws_account_UpdateUserInput_atlsoapheader_map,
		&__Cws_account_UpdateUser_atlsoapheader_map,
		NULL
	};
	
	return __Cws_account_headers;
}

template <typename TClient>
ATL_NOINLINE inline void * Cws_accountT<TClient>::GetHeaderValue()
{
	return this;
}

template <typename TClient>
ATL_NOINLINE inline const wchar_t * Cws_accountT<TClient>::GetNamespaceUri()
{
	return L"http://webservices.seisint.com/ws_account";
}

template <typename TClient>
ATL_NOINLINE inline const char * Cws_accountT<TClient>::GetServiceName()
{
	return NULL;
}

template <typename TClient>
ATL_NOINLINE inline const char * Cws_accountT<TClient>::GetNamespaceUriA()
{
	return "http://webservices.seisint.com/ws_account";
}

template <typename TClient>
ATL_NOINLINE inline HRESULT Cws_accountT<TClient>::CallFunction(
	void *, 
	const wchar_t *, int,
	size_t)
{
	return E_NOTIMPL;
}

template <typename TClient>
ATL_NOINLINE inline HRESULT Cws_accountT<TClient>::GetClientReader(ISAXXMLReader **ppReader)
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

} // namespace ws_account

template<>
inline HRESULT AtlCleanupValue<ws_account::EspException>(ws_account::EspException *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Code);
	AtlCleanupValue(&pVal->Audience);
	AtlCleanupValue(&pVal->Source);
	AtlCleanupValue(&pVal->Message);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<ws_account::EspException>(ws_account::EspException *pVal, IAtlMemMgr *pMemMgr)
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
inline HRESULT AtlCleanupValue<ws_account::ArrayOfEspException>(ws_account::ArrayOfEspException *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Source);
	AtlCleanupArray(pVal->Exception, pVal->__Exception_nSizeIs);	
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<ws_account::ArrayOfEspException>(ws_account::ArrayOfEspException *pVal, IAtlMemMgr *pMemMgr)
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
