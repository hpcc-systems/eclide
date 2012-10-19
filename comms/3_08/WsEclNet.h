//
// sproxy.exe generated file
// do not modify this file
//
// Created: 01/27/2005@10:01:38
//

#pragma once


#ifndef _WIN32_WINDOWS
#pragma message("warning: defining _WIN32_WINDOWS = 0x0410")
#define _WIN32_WINDOWS 0x0410
#endif

#include <atlsoap.h>

namespace Gordon
{

struct EspException
{
    BSTR Code;	
    BSTR Audience;
    BSTR Source;
    BSTR Message;
};

template <typename TClient = CSoapSocketClientT<> >
class CGordonT :
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

    CGordonT(ISAXXMLReader *pReader = NULL)
        :TClient(_T("http://10.150.51.40:8002/Gordon\?ver_=0.000000"))
    {
        SetClient(true);
        SetReader(pReader);
    }

    ~CGordonT()
    {
        Uninitialize();
    }

    void Uninitialize()
    {
        UninitializeSOAP();
    }

    HRESULT EclNet_ExecuteVertex(
        BSTR cluster_,
        BSTR queue_,
        bool include_schemas_,
        bool limitResults_,
        __int64 Category,
        BSTR ID,
        EspException** Exceptions, int* Exceptions_nSizeIs,
        BSTR* Results
    );

    HRESULT EclNet_GetMeta(
        BSTR cluster_,
        BSTR queue_,
        bool include_schemas_,
        bool limitResults_,
        EspException** Exceptions, int* Exceptions_nSizeIs,
        BSTR* Results
    );
};

typedef CGordonT<> CGordon;

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
		SOAPFLAG_NULLABLE | SOAPFLAG_NONE,
    0x08ACEEAC,
    "http://webservices.seisint.com/Gordon",
    L"http://webservices.seisint.com/Gordon",
    sizeof("http://webservices.seisint.com/Gordon")-1
};
}

struct __CGordon_EclNet_ExecuteVertex_struct
{
    BSTR cluster_;
    BSTR queue_;
    bool include_schemas_;
    bool limitResults_;
    __int64 Category;
    BSTR ID;
    EspException *Exceptions;
    int __Exceptions_nSizeIs;
    BSTR Results;
};

extern __declspec(selectany) const _soapmapentry __CGordon_EclNet_ExecuteVertex_entries[] =
{

    {
        0xF054E0A1,
        "cluster_",
        L"cluster_",
        sizeof("cluster_")-1,
        SOAPTYPE_STRING,
        SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
        offsetof(__CGordon_EclNet_ExecuteVertex_struct, cluster_),
        NULL,
        NULL,
        -1,
    },
    {
        0x10158364,
        "queue_",
        L"queue_",
        sizeof("queue_")-1,
        SOAPTYPE_STRING,
        SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
        offsetof(__CGordon_EclNet_ExecuteVertex_struct, queue_),
        NULL,
        NULL,
        -1,
    },
    {
        0xBA876726,
        "include_schemas_",
        L"include_schemas_",
        sizeof("include_schemas_")-1,
        SOAPTYPE_BOOLEAN,
		SOAPFLAG_NULLABLE | SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
        offsetof(__CGordon_EclNet_ExecuteVertex_struct, include_schemas_),
        NULL,
        NULL,
        -1,
    },
    {
        0xE9AF07F0,
        "limitResults_",
        L"limitResults_",
        sizeof("limitResults_")-1,
        SOAPTYPE_BOOLEAN,
		SOAPFLAG_NULLABLE | SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
        offsetof(__CGordon_EclNet_ExecuteVertex_struct, limitResults_),
        NULL,
        NULL,
        -1,
    },
    {
        0x1600323E,
        "Category",
        L"Category",
        sizeof("Category")-1,
        SOAPTYPE_INTEGER,
		SOAPFLAG_NULLABLE | SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
        offsetof(__CGordon_EclNet_ExecuteVertex_struct, Category),
        NULL,
        NULL,
        -1,
    },
    {
        0x000009AD,
        "ID",
        L"ID",
        sizeof("ID")-1,
        SOAPTYPE_STRING,
        SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
        offsetof(__CGordon_EclNet_ExecuteVertex_struct, ID),
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
        SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
        offsetof(__CGordon_EclNet_ExecuteVertex_struct, Exceptions),
        NULL,
        &__EspException_map,
        6+1,
    },
    {
        0x7B0DBD02,
        "__Exceptions_nSizeIs",
        L"__Exceptions_nSizeIs",
        sizeof("__Exceptions_nSizeIs")-1,
        SOAPTYPE_INT,
		SOAPFLAG_NULLABLE | SOAPFLAG_NOMARSHAL,
        offsetof(__CGordon_EclNet_ExecuteVertex_struct, __Exceptions_nSizeIs),
        NULL,
        NULL,
        -1
    },
    {
        0x9C238272,
        "Results",
        L"Results",
        sizeof("Results")-1,
        SOAPTYPE_STRING,
        SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
        offsetof(__CGordon_EclNet_ExecuteVertex_struct, Results),
        NULL,
        NULL,
        -1,
    },
    { 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CGordon_EclNet_ExecuteVertex_map =
{
    0x2FE1B4FA,
    "EclNet_ExecuteVertexRequest",
    L"EclNet_ExecuteVertexResponse",
    sizeof("EclNet_ExecuteVertexRequest")-1,
    sizeof("EclNet_ExecuteVertexResponse")-1,
    SOAPMAP_FUNC,
    __CGordon_EclNet_ExecuteVertex_entries,
    sizeof(__CGordon_EclNet_ExecuteVertex_struct),
    2,
    -1,
		SOAPFLAG_NULLABLE | SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
    0x08ACEEAC,
    "http://webservices.seisint.com/Gordon",
    L"http://webservices.seisint.com/Gordon",
    sizeof("http://webservices.seisint.com/Gordon")-1
};


struct __CGordon_EclNet_GetMeta_struct
{
    BSTR cluster_;
    BSTR queue_;
    bool include_schemas_;
    bool limitResults_;
    EspException *Exceptions;
    int __Exceptions_nSizeIs;
    BSTR Results;
};

extern __declspec(selectany) const _soapmapentry __CGordon_EclNet_GetMeta_entries[] =
{

    {
        0xF054E0A1,
        "cluster_",
        L"cluster_",
        sizeof("cluster_")-1,
        SOAPTYPE_STRING,
        SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
        offsetof(__CGordon_EclNet_GetMeta_struct, cluster_),
        NULL,
        NULL,
        -1,
    },
    {
        0x10158364,
        "queue_",
        L"queue_",
        sizeof("queue_")-1,
        SOAPTYPE_STRING,
        SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
        offsetof(__CGordon_EclNet_GetMeta_struct, queue_),
        NULL,
        NULL,
        -1,
    },
    {
        0xBA876726,
        "include_schemas_",
        L"include_schemas_",
        sizeof("include_schemas_")-1,
        SOAPTYPE_BOOLEAN,
		SOAPFLAG_NULLABLE | SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
        offsetof(__CGordon_EclNet_GetMeta_struct, include_schemas_),
        NULL,
        NULL,
        -1,
    },
    {
        0xE9AF07F0,
        "limitResults_",
        L"limitResults_",
        sizeof("limitResults_")-1,
        SOAPTYPE_BOOLEAN,
		SOAPFLAG_NULLABLE | SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
        offsetof(__CGordon_EclNet_GetMeta_struct, limitResults_),
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
        SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
        offsetof(__CGordon_EclNet_GetMeta_struct, Exceptions),
        NULL,
        &__EspException_map,
        4+1,
    },
    {
        0x7B0DBD02,
        "__Exceptions_nSizeIs",
        L"__Exceptions_nSizeIs",
        sizeof("__Exceptions_nSizeIs")-1,
        SOAPTYPE_INT,
		SOAPFLAG_NULLABLE | SOAPFLAG_NOMARSHAL,
        offsetof(__CGordon_EclNet_GetMeta_struct, __Exceptions_nSizeIs),
        NULL,
        NULL,
        -1
    },
    {
        0x9C238272,
        "Results",
        L"Results",
        sizeof("Results")-1,
        SOAPTYPE_STRING,
        SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
        offsetof(__CGordon_EclNet_GetMeta_struct, Results),
        NULL,
        NULL,
        -1,
    },
    { 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CGordon_EclNet_GetMeta_map =
{
    0x53DF2ED0,
    "EclNet_GetMetaRequest",
    L"EclNet_GetMetaResponse",
    sizeof("EclNet_GetMetaRequest")-1,
    sizeof("EclNet_GetMetaResponse")-1,
    SOAPMAP_FUNC,
    __CGordon_EclNet_GetMeta_entries,
    sizeof(__CGordon_EclNet_GetMeta_struct),
    2,
    -1,
		SOAPFLAG_NULLABLE | SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
    0x08ACEEAC,
    "http://webservices.seisint.com/Gordon",
    L"http://webservices.seisint.com/Gordon",
    sizeof("http://webservices.seisint.com/Gordon")-1
};

extern __declspec(selectany) const _soapmap * __CGordon_funcs[] =
{
    &__CGordon_EclNet_ExecuteVertex_map,
    &__CGordon_EclNet_GetMeta_map,
    NULL
};

template <typename TClient>
inline HRESULT CGordonT<TClient>::EclNet_ExecuteVertex(
        BSTR cluster_,
        BSTR queue_,
        bool include_schemas_,
        bool limitResults_,
        __int64 Category,
        BSTR ID,
        EspException** Exceptions, int* __Exceptions_nSizeIs,
        BSTR* Results
    )
{
    if ( Exceptions == NULL )
        return E_POINTER;
    if( __Exceptions_nSizeIs == NULL )
        return E_POINTER;
    if ( Results == NULL )
        return E_POINTER;

    HRESULT __atlsoap_hr = InitializeSOAP(NULL);
    if (FAILED(__atlsoap_hr))
    {
        SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
        return __atlsoap_hr;
    }

    CleanupClient();

    CComPtr<IStream> __atlsoap_spReadStream;
    __CGordon_EclNet_ExecuteVertex_struct __params;
    memset(&__params, 0x00, sizeof(__params));
    __params.cluster_ = cluster_;
    __params.queue_ = queue_;
    __params.include_schemas_ = include_schemas_;
    __params.limitResults_ = limitResults_;
    __params.Category = Category;
    __params.ID = ID;

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

    __atlsoap_hr = SendRequest(_T("SOAPAction: \"/Gordon/EclNet_ExecuteVertex\?ver_=1.0\"\r\n"));
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
    *__Exceptions_nSizeIs = __params.__Exceptions_nSizeIs;
    *Results = __params.Results;
    goto __skip_cleanup;

__cleanup:
    Cleanup();
__skip_cleanup:
    ResetClientState(true);
    memset(&__params, 0x00, sizeof(__params));
    return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CGordonT<TClient>::EclNet_GetMeta(
        BSTR cluster_,
        BSTR queue_,
        bool include_schemas_,
        bool limitResults_,
        EspException** Exceptions, int* __Exceptions_nSizeIs,
        BSTR* Results
    )
{
    if ( Exceptions == NULL )
        return E_POINTER;
    if( __Exceptions_nSizeIs == NULL )
        return E_POINTER;
    if ( Results == NULL )
        return E_POINTER;

    HRESULT __atlsoap_hr = InitializeSOAP(NULL);
    if (FAILED(__atlsoap_hr))
    {
        SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
        return __atlsoap_hr;
    }

    CleanupClient();

    CComPtr<IStream> __atlsoap_spReadStream;
    __CGordon_EclNet_GetMeta_struct __params;
    memset(&__params, 0x00, sizeof(__params));
    __params.cluster_ = cluster_;
    __params.queue_ = queue_;
    __params.include_schemas_ = include_schemas_;
    __params.limitResults_ = limitResults_;

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

    __atlsoap_hr = SendRequest(_T("SOAPAction: \"/Gordon/EclNet_GetMeta\?ver_=1.0\"\r\n"));
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
    *__Exceptions_nSizeIs = __params.__Exceptions_nSizeIs;
    *Results = __params.Results;
    goto __skip_cleanup;

__cleanup:
    Cleanup();
__skip_cleanup:
    ResetClientState(true);
    memset(&__params, 0x00, sizeof(__params));
    return __atlsoap_hr;
}

template <typename TClient>
ATL_NOINLINE inline const _soapmap ** CGordonT<TClient>::GetFunctionMap()
{
    return __CGordon_funcs;
}

template <typename TClient>
ATL_NOINLINE inline const _soapmap ** CGordonT<TClient>::GetHeaderMap()
{
    static const _soapmapentry __CGordon_EclNet_ExecuteVertex_atlsoapheader_entries[] =
    {
        { 0x00000000 }
    };

    static const _soapmap __CGordon_EclNet_ExecuteVertex_atlsoapheader_map =
    {
        0x2FE1B4FA,
        "EclNet_ExecuteVertexRequest",
        L"EclNet_ExecuteVertexResponse",
        sizeof("EclNet_ExecuteVertexRequest")-1,
        sizeof("EclNet_ExecuteVertexResponse")-1,
        SOAPMAP_HEADER,
        __CGordon_EclNet_ExecuteVertex_atlsoapheader_entries,
        0,
        0,
        -1,
		SOAPFLAG_NULLABLE | SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
        0x08ACEEAC,
        "http://webservices.seisint.com/Gordon",
        L"http://webservices.seisint.com/Gordon",
        sizeof("http://webservices.seisint.com/Gordon")-1
    };

    static const _soapmapentry __CGordon_EclNet_GetMeta_atlsoapheader_entries[] =
    {
        { 0x00000000 }
    };

    static const _soapmap __CGordon_EclNet_GetMeta_atlsoapheader_map =
    {
        0x53DF2ED0,
        "EclNet_GetMetaRequest",
        L"EclNet_GetMetaResponse",
        sizeof("EclNet_GetMetaRequest")-1,
        sizeof("EclNet_GetMetaResponse")-1,
        SOAPMAP_HEADER,
        __CGordon_EclNet_GetMeta_atlsoapheader_entries,
        0,
        0,
        -1,
		SOAPFLAG_NULLABLE | SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
        0x08ACEEAC,
        "http://webservices.seisint.com/Gordon",
        L"http://webservices.seisint.com/Gordon",
        sizeof("http://webservices.seisint.com/Gordon")-1
    };


    static const _soapmap * __CGordon_headers[] =
    {
        &__CGordon_EclNet_ExecuteVertex_atlsoapheader_map,
        &__CGordon_EclNet_GetMeta_atlsoapheader_map,
        NULL
    };

    return __CGordon_headers;
}

template <typename TClient>
ATL_NOINLINE inline void * CGordonT<TClient>::GetHeaderValue()
{
    return this;
}

template <typename TClient>
ATL_NOINLINE inline const wchar_t * CGordonT<TClient>::GetNamespaceUri()
{
    return L"http://webservices.seisint.com/Gordon";
}

template <typename TClient>
ATL_NOINLINE inline const char * CGordonT<TClient>::GetServiceName()
{
    return NULL;
}

template <typename TClient>
ATL_NOINLINE inline const char * CGordonT<TClient>::GetNamespaceUriA()
{
    return "http://webservices.seisint.com/Gordon";
}

template <typename TClient>
ATL_NOINLINE inline HRESULT CGordonT<TClient>::CallFunction(
    void *,
    const wchar_t *, int,
    size_t)
{
    return E_NOTIMPL;
}

template <typename TClient>
ATL_NOINLINE inline HRESULT CGordonT<TClient>::GetClientReader(ISAXXMLReader **ppReader)
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

} // namespace Gordon

template<>
inline HRESULT AtlCleanupValue<Gordon::EspException>(Gordon::EspException *pVal)
{
    pVal;
    AtlCleanupValue(&pVal->Code);
    AtlCleanupValue(&pVal->Audience);
    AtlCleanupValue(&pVal->Source);
    AtlCleanupValue(&pVal->Message);
    return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<Gordon::EspException>(Gordon::EspException *pVal, IAtlMemMgr *pMemMgr)
{
    pVal;
    pMemMgr;

    AtlCleanupValueEx(&pVal->Code, pMemMgr);
    AtlCleanupValueEx(&pVal->Audience, pMemMgr);
    AtlCleanupValueEx(&pVal->Source, pMemMgr);
    AtlCleanupValueEx(&pVal->Message, pMemMgr);
    return S_OK;
}
