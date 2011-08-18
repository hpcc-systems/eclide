//
// sproxy.exe generated file
// do not modify this file
//
// Created: 03/23/2007@07:29:23
//

#pragma once


#ifndef _WIN32_WINDOWS
#pragma message("warning: defining _WIN32_WINDOWS = 0x0410")
#define _WIN32_WINDOWS 0x0410
#endif

#include <atlsoap.h>

namespace SEEP3
{

struct EspException
{
    BSTR Code;
    BSTR Audience;
    BSTR Source;
    BSTR Message;
};

template <typename TClient = CSoapSocketClientT<> >
class CSEEP3T :
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

    CSEEP3T(ISAXXMLReader *pReader = NULL)
        :TClient(_T("http://10.173.190.83:8022/SEEP3\?ver_=0"))
    {
        SetClient(true);
        SetReader(pReader);
    }

    ~CSEEP3T()
    {
        Uninitialize();
    }

    void Uninitialize()
    {
        UninitializeSOAP();
    }

    HRESULT SEEP3_EntityEnhance(
        BSTR cluster_,
        BSTR queue_,
        bool include_schemas_,
        bool limitResults_,
        BSTR EntityList,
        __int64 MaxDids,
        __int64 MaxFull,
        EspException** Exceptions, int* Exceptions_nSizeIs,
        BSTR* Results,
        BSTR* Workunit,
        bool* Async
    );

    HRESULT SEEP3_EntityClassify(
        BSTR cluster_,
        BSTR queue_,
        bool include_schemas_,
        bool limitResults_,
        BSTR SParam,
        BSTR* Contents, int Contents_nSizeIs,
        BSTR* Categories, int Categories_nSizeIs,
        BSTR Format,
        EspException** Exceptions, int* Exceptions_nSizeIs,
        BSTR* Results,
        BSTR* Workunit,
        bool* Async
    );

    HRESULT SEEP3_EntityResolve(
        BSTR cluster_,
        BSTR queue_,
        bool include_schemas_,
        bool limitResults_,
        BSTR EntityList,
        EspException** Exceptions, int* Exceptions_nSizeIs,
        BSTR* Results,
        BSTR* Workunit,
        bool* Async
    );
};

typedef CSEEP3T<> CSEEP3;

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
    0x184A7A62,
    "http://webservices.seisint.com/SEEP3",
    L"http://webservices.seisint.com/SEEP3",
    sizeof("http://webservices.seisint.com/SEEP3")-1
};
}

struct __CSEEP3_SEEP3_EntityEnhance_struct
{
    BSTR cluster_;
    BSTR queue_;
    bool include_schemas_;
    bool limitResults_;
    BSTR EntityList;
    __int64 MaxDids;
    __int64 MaxFull;
    EspException *Exceptions;
    int __Exceptions_nSizeIs;
    BSTR Results;
    BSTR Workunit;
    bool Async;
};

extern __declspec(selectany) const _soapmapentry __CSEEP3_SEEP3_EntityEnhance_entries[] =
{

    {
        0xF054E0A1,
        "cluster_",
        L"cluster_",
        sizeof("cluster_")-1,
        SOAPTYPE_STRING,
        SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
        offsetof(__CSEEP3_SEEP3_EntityEnhance_struct, cluster_),
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
        offsetof(__CSEEP3_SEEP3_EntityEnhance_struct, queue_),
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
        offsetof(__CSEEP3_SEEP3_EntityEnhance_struct, include_schemas_),
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
        offsetof(__CSEEP3_SEEP3_EntityEnhance_struct, limitResults_),
        NULL,
        NULL,
        -1,
    },
    {
        0xDC188619,
        "EntityList",
        L"EntityList",
        sizeof("EntityList")-1,
        SOAPTYPE_STRING,
        SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
        offsetof(__CSEEP3_SEEP3_EntityEnhance_struct, EntityList),
        NULL,
        NULL,
        -1,
    },
    {
        0x122B3D4A,
        "MaxDids",
        L"MaxDids",
        sizeof("MaxDids")-1,
        SOAPTYPE_INTEGER,
		SOAPFLAG_NULLABLE | SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
        offsetof(__CSEEP3_SEEP3_EntityEnhance_struct, MaxDids),
        NULL,
        NULL,
        -1,
    },
    {
        0x122C8A19,
        "MaxFull",
        L"MaxFull",
        sizeof("MaxFull")-1,
        SOAPTYPE_INTEGER,
		SOAPFLAG_NULLABLE | SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
        offsetof(__CSEEP3_SEEP3_EntityEnhance_struct, MaxFull),
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
        offsetof(__CSEEP3_SEEP3_EntityEnhance_struct, Exceptions),
        NULL,
        &__EspException_map,
        7+1,
    },
    {
        0x7B0DBD02,
        "__Exceptions_nSizeIs",
        L"__Exceptions_nSizeIs",
        sizeof("__Exceptions_nSizeIs")-1,
        SOAPTYPE_INT,
		SOAPFLAG_NULLABLE | SOAPFLAG_NOMARSHAL,
        offsetof(__CSEEP3_SEEP3_EntityEnhance_struct, __Exceptions_nSizeIs),
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
        offsetof(__CSEEP3_SEEP3_EntityEnhance_struct, Results),
        NULL,
        NULL,
        -1,
    },
    {
        0xBC9E1003,
        "Workunit",
        L"Workunit",
        sizeof("Workunit")-1,
        SOAPTYPE_STRING,
        SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
        offsetof(__CSEEP3_SEEP3_EntityEnhance_struct, Workunit),
        NULL,
        NULL,
        -1,
    },
    {
        0x04D9599E,
        "Async",
        L"Async",
        sizeof("Async")-1,
        SOAPTYPE_BOOLEAN,
		SOAPFLAG_NULLABLE | SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
        offsetof(__CSEEP3_SEEP3_EntityEnhance_struct, Async),
        NULL,
        NULL,
        -1,
    },
    { 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CSEEP3_SEEP3_EntityEnhance_map =
{
    0xA6B2E90B,
    "SEEP3.EntityEnhanceRequest",
    L"SEEP3.EntityEnhanceResponse",
    sizeof("SEEP3.EntityEnhanceRequest")-1,
    sizeof("SEEP3.EntityEnhanceResponse")-1,
    SOAPMAP_FUNC,
    __CSEEP3_SEEP3_EntityEnhance_entries,
    sizeof(__CSEEP3_SEEP3_EntityEnhance_struct),
    4,
    -1,
		SOAPFLAG_NULLABLE | SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
    0x184A7A62,
    "http://webservices.seisint.com/SEEP3",
    L"http://webservices.seisint.com/SEEP3",
    sizeof("http://webservices.seisint.com/SEEP3")-1
};


struct __CSEEP3_SEEP3_EntityClassify_struct
{
    BSTR cluster_;
    BSTR queue_;
    bool include_schemas_;
    bool limitResults_;
    BSTR SParam;
    BSTR *Contents;
    int __Contents_nSizeIs;
    BSTR *Categories;
    int __Categories_nSizeIs;
    BSTR Format;
    EspException *Exceptions;
    int __Exceptions_nSizeIs;
    BSTR Results;
    BSTR Workunit;
    bool Async;
};

extern __declspec(selectany) const _soapmapentry __CSEEP3_SEEP3_EntityClassify_entries[] =
{

    {
        0xF054E0A1,
        "cluster_",
        L"cluster_",
        sizeof("cluster_")-1,
        SOAPTYPE_STRING,
        SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
        offsetof(__CSEEP3_SEEP3_EntityClassify_struct, cluster_),
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
        offsetof(__CSEEP3_SEEP3_EntityClassify_struct, queue_),
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
        offsetof(__CSEEP3_SEEP3_EntityClassify_struct, include_schemas_),
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
        offsetof(__CSEEP3_SEEP3_EntityClassify_struct, limitResults_),
        NULL,
        NULL,
        -1,
    },
    {
        0xC77AF724,
        "SParam",
        L"SParam",
        sizeof("SParam")-1,
        SOAPTYPE_STRING,
        SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
        offsetof(__CSEEP3_SEEP3_EntityClassify_struct, SParam),
        NULL,
        NULL,
        -1,
    },
    {
        0x3EBEE3CE,
        "Contents",
        L"Contents",
        sizeof("Contents")-1,
        SOAPTYPE_STRING,
        SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
        offsetof(__CSEEP3_SEEP3_EntityClassify_struct, Contents),
        NULL,
        NULL,
        5+1,
    },
    {
        0x3EBEE3CE,
        "__Contents_nSizeIs",
        L"__Contents_nSizeIs",
        sizeof("__Contents_nSizeIs")-1,
        SOAPTYPE_INT,
		SOAPFLAG_NULLABLE | SOAPFLAG_NOMARSHAL,
        offsetof(__CSEEP3_SEEP3_EntityClassify_struct, __Contents_nSizeIs),
        NULL,
        NULL,
        -1
    },
    {
        0x96D58326,
        "Categories",
        L"Categories",
        sizeof("Categories")-1,
        SOAPTYPE_STRING,
        SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
        offsetof(__CSEEP3_SEEP3_EntityClassify_struct, Categories),
        NULL,
        NULL,
        7+1,
    },
    {
        0x96D58326,
        "__Categories_nSizeIs",
        L"__Categories_nSizeIs",
        sizeof("__Categories_nSizeIs")-1,
        SOAPTYPE_INT,
		SOAPFLAG_NULLABLE | SOAPFLAG_NOMARSHAL,
        offsetof(__CSEEP3_SEEP3_EntityClassify_struct, __Categories_nSizeIs),
        NULL,
        NULL,
        -1
    },
    {
        0xAB621BC9,
        "Format",
        L"Format",
        sizeof("Format")-1,
        SOAPTYPE_STRING,
        SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
        offsetof(__CSEEP3_SEEP3_EntityClassify_struct, Format),
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
        offsetof(__CSEEP3_SEEP3_EntityClassify_struct, Exceptions),
        NULL,
        &__EspException_map,
        10+1,
    },
    {
        0x7B0DBD02,
        "__Exceptions_nSizeIs",
        L"__Exceptions_nSizeIs",
        sizeof("__Exceptions_nSizeIs")-1,
        SOAPTYPE_INT,
		SOAPFLAG_NULLABLE | SOAPFLAG_NOMARSHAL,
        offsetof(__CSEEP3_SEEP3_EntityClassify_struct, __Exceptions_nSizeIs),
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
        offsetof(__CSEEP3_SEEP3_EntityClassify_struct, Results),
        NULL,
        NULL,
        -1,
    },
    {
        0xBC9E1003,
        "Workunit",
        L"Workunit",
        sizeof("Workunit")-1,
        SOAPTYPE_STRING,
        SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
        offsetof(__CSEEP3_SEEP3_EntityClassify_struct, Workunit),
        NULL,
        NULL,
        -1,
    },
    {
        0x04D9599E,
        "Async",
        L"Async",
        sizeof("Async")-1,
        SOAPTYPE_BOOLEAN,
		SOAPFLAG_NULLABLE | SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
        offsetof(__CSEEP3_SEEP3_EntityClassify_struct, Async),
        NULL,
        NULL,
        -1,
    },
    { 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CSEEP3_SEEP3_EntityClassify_map =
{
    0x3470C1F7,
    "SEEP3.EntityClassifyRequest",
    L"SEEP3.EntityClassifyResponse",
    sizeof("SEEP3.EntityClassifyRequest")-1,
    sizeof("SEEP3.EntityClassifyResponse")-1,
    SOAPMAP_FUNC,
    __CSEEP3_SEEP3_EntityClassify_entries,
    sizeof(__CSEEP3_SEEP3_EntityClassify_struct),
    4,
    -1,
		SOAPFLAG_NULLABLE | SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
    0x184A7A62,
    "http://webservices.seisint.com/SEEP3",
    L"http://webservices.seisint.com/SEEP3",
    sizeof("http://webservices.seisint.com/SEEP3")-1
};


struct __CSEEP3_SEEP3_EntityResolve_struct
{
    BSTR cluster_;
    BSTR queue_;
    bool include_schemas_;
    bool limitResults_;
    BSTR EntityList;
    EspException *Exceptions;
    int __Exceptions_nSizeIs;
    BSTR Results;
    BSTR Workunit;
    bool Async;
};

extern __declspec(selectany) const _soapmapentry __CSEEP3_SEEP3_EntityResolve_entries[] =
{

    {
        0xF054E0A1,
        "cluster_",
        L"cluster_",
        sizeof("cluster_")-1,
        SOAPTYPE_STRING,
        SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
        offsetof(__CSEEP3_SEEP3_EntityResolve_struct, cluster_),
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
        offsetof(__CSEEP3_SEEP3_EntityResolve_struct, queue_),
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
        offsetof(__CSEEP3_SEEP3_EntityResolve_struct, include_schemas_),
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
        offsetof(__CSEEP3_SEEP3_EntityResolve_struct, limitResults_),
        NULL,
        NULL,
        -1,
    },
    {
        0xDC188619,
        "EntityList",
        L"EntityList",
        sizeof("EntityList")-1,
        SOAPTYPE_STRING,
        SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
        offsetof(__CSEEP3_SEEP3_EntityResolve_struct, EntityList),
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
        offsetof(__CSEEP3_SEEP3_EntityResolve_struct, Exceptions),
        NULL,
        &__EspException_map,
        5+1,
    },
    {
        0x7B0DBD02,
        "__Exceptions_nSizeIs",
        L"__Exceptions_nSizeIs",
        sizeof("__Exceptions_nSizeIs")-1,
        SOAPTYPE_INT,
		SOAPFLAG_NULLABLE | SOAPFLAG_NOMARSHAL,
        offsetof(__CSEEP3_SEEP3_EntityResolve_struct, __Exceptions_nSizeIs),
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
        offsetof(__CSEEP3_SEEP3_EntityResolve_struct, Results),
        NULL,
        NULL,
        -1,
    },
    {
        0xBC9E1003,
        "Workunit",
        L"Workunit",
        sizeof("Workunit")-1,
        SOAPTYPE_STRING,
        SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
        offsetof(__CSEEP3_SEEP3_EntityResolve_struct, Workunit),
        NULL,
        NULL,
        -1,
    },
    {
        0x04D9599E,
        "Async",
        L"Async",
        sizeof("Async")-1,
        SOAPTYPE_BOOLEAN,
		SOAPFLAG_NULLABLE | SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
        offsetof(__CSEEP3_SEEP3_EntityResolve_struct, Async),
        NULL,
        NULL,
        -1,
    },
    { 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CSEEP3_SEEP3_EntityResolve_map =
{
    0x348553F9,
    "SEEP3.EntityResolveRequest",
    L"SEEP3.EntityResolveResponse",
    sizeof("SEEP3.EntityResolveRequest")-1,
    sizeof("SEEP3.EntityResolveResponse")-1,
    SOAPMAP_FUNC,
    __CSEEP3_SEEP3_EntityResolve_entries,
    sizeof(__CSEEP3_SEEP3_EntityResolve_struct),
    4,
    -1,
		SOAPFLAG_NULLABLE | SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
    0x184A7A62,
    "http://webservices.seisint.com/SEEP3",
    L"http://webservices.seisint.com/SEEP3",
    sizeof("http://webservices.seisint.com/SEEP3")-1
};

extern __declspec(selectany) const _soapmap * __CSEEP3_funcs[] =
{
    &__CSEEP3_SEEP3_EntityEnhance_map,
    &__CSEEP3_SEEP3_EntityClassify_map,
    &__CSEEP3_SEEP3_EntityResolve_map,
    NULL
};

template <typename TClient>
inline HRESULT CSEEP3T<TClient>::SEEP3_EntityEnhance(
        BSTR cluster_,
        BSTR queue_,
        bool include_schemas_,
        bool limitResults_,
        BSTR EntityList,
        __int64 MaxDids,
        __int64 MaxFull,
        EspException** Exceptions, int* __Exceptions_nSizeIs,
        BSTR* Results,
        BSTR* Workunit,
        bool* Async
    )
{
    if ( Exceptions == NULL )
        return E_POINTER;
    if( __Exceptions_nSizeIs == NULL )
        return E_POINTER;
    if ( Results == NULL )
        return E_POINTER;
    if ( Workunit == NULL )
        return E_POINTER;
    if ( Async == NULL )
        return E_POINTER;

    HRESULT __atlsoap_hr = InitializeSOAP(NULL);
    if (FAILED(__atlsoap_hr))
    {
        SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
        return __atlsoap_hr;
    }

    CleanupClient();

    CComPtr<IStream> __atlsoap_spReadStream;
    __CSEEP3_SEEP3_EntityEnhance_struct __params;
    memset(&__params, 0x00, sizeof(__params));
    __params.cluster_ = cluster_;
    __params.queue_ = queue_;
    __params.include_schemas_ = include_schemas_;
    __params.limitResults_ = limitResults_;
    __params.EntityList = EntityList;
    __params.MaxDids = MaxDids;
    __params.MaxFull = MaxFull;

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

    __atlsoap_hr = SendRequest(_T("SOAPAction: \"/SEEP3/SEEP3.EntityEnhance\?ver_=1.0\"\r\n"));
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
    *Workunit = __params.Workunit;
    *Async = __params.Async;
    goto __skip_cleanup;

__cleanup:
    Cleanup();
__skip_cleanup:
    ResetClientState(true);
    memset(&__params, 0x00, sizeof(__params));
    return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CSEEP3T<TClient>::SEEP3_EntityClassify(
        BSTR cluster_,
        BSTR queue_,
        bool include_schemas_,
        bool limitResults_,
        BSTR SParam,
        BSTR* Contents, int __Contents_nSizeIs,
        BSTR* Categories, int __Categories_nSizeIs,
        BSTR Format,
        EspException** Exceptions, int* __Exceptions_nSizeIs,
        BSTR* Results,
        BSTR* Workunit,
        bool* Async
    )
{
    if ( Exceptions == NULL )
        return E_POINTER;
    if( __Exceptions_nSizeIs == NULL )
        return E_POINTER;
    if ( Results == NULL )
        return E_POINTER;
    if ( Workunit == NULL )
        return E_POINTER;
    if ( Async == NULL )
        return E_POINTER;

    HRESULT __atlsoap_hr = InitializeSOAP(NULL);
    if (FAILED(__atlsoap_hr))
    {
        SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
        return __atlsoap_hr;
    }

    CleanupClient();

    CComPtr<IStream> __atlsoap_spReadStream;
    __CSEEP3_SEEP3_EntityClassify_struct __params;
    memset(&__params, 0x00, sizeof(__params));
    __params.cluster_ = cluster_;
    __params.queue_ = queue_;
    __params.include_schemas_ = include_schemas_;
    __params.limitResults_ = limitResults_;
    __params.SParam = SParam;
    __params.Contents = Contents;
    __params.__Contents_nSizeIs = __Contents_nSizeIs;
    __params.Categories = Categories;
    __params.__Categories_nSizeIs = __Categories_nSizeIs;
    __params.Format = Format;

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

    __atlsoap_hr = SendRequest(_T("SOAPAction: \"/SEEP3/SEEP3.EntityClassify\?ver_=1.0\"\r\n"));
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
    *Workunit = __params.Workunit;
    *Async = __params.Async;
    goto __skip_cleanup;

__cleanup:
    Cleanup();
__skip_cleanup:
    ResetClientState(true);
    memset(&__params, 0x00, sizeof(__params));
    return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CSEEP3T<TClient>::SEEP3_EntityResolve(
        BSTR cluster_,
        BSTR queue_,
        bool include_schemas_,
        bool limitResults_,
        BSTR EntityList,
        EspException** Exceptions, int* __Exceptions_nSizeIs,
        BSTR* Results,
        BSTR* Workunit,
        bool* Async
    )
{
    if ( Exceptions == NULL )
        return E_POINTER;
    if( __Exceptions_nSizeIs == NULL )
        return E_POINTER;
    if ( Results == NULL )
        return E_POINTER;
    if ( Workunit == NULL )
        return E_POINTER;
    if ( Async == NULL )
        return E_POINTER;

    HRESULT __atlsoap_hr = InitializeSOAP(NULL);
    if (FAILED(__atlsoap_hr))
    {
        SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
        return __atlsoap_hr;
    }

    CleanupClient();

    CComPtr<IStream> __atlsoap_spReadStream;
    __CSEEP3_SEEP3_EntityResolve_struct __params;
    memset(&__params, 0x00, sizeof(__params));
    __params.cluster_ = cluster_;
    __params.queue_ = queue_;
    __params.include_schemas_ = include_schemas_;
    __params.limitResults_ = limitResults_;
    __params.EntityList = EntityList;

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

    __atlsoap_hr = SendRequest(_T("SOAPAction: \"/SEEP3/SEEP3.EntityResolve\?ver_=1.0\"\r\n"));
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
    *Workunit = __params.Workunit;
    *Async = __params.Async;
    goto __skip_cleanup;

__cleanup:
    Cleanup();
__skip_cleanup:
    ResetClientState(true);
    memset(&__params, 0x00, sizeof(__params));
    return __atlsoap_hr;
}

template <typename TClient>
ATL_NOINLINE inline const _soapmap ** CSEEP3T<TClient>::GetFunctionMap()
{
    return __CSEEP3_funcs;
}

template <typename TClient>
ATL_NOINLINE inline const _soapmap ** CSEEP3T<TClient>::GetHeaderMap()
{
    static const _soapmapentry __CSEEP3_SEEP3_EntityEnhance_atlsoapheader_entries[] =
    {
        { 0x00000000 }
    };

    static const _soapmap __CSEEP3_SEEP3_EntityEnhance_atlsoapheader_map =
    {
        0xA6B2E90B,
        "SEEP3.EntityEnhanceRequest",
        L"SEEP3.EntityEnhanceResponse",
        sizeof("SEEP3.EntityEnhanceRequest")-1,
        sizeof("SEEP3.EntityEnhanceResponse")-1,
        SOAPMAP_HEADER,
        __CSEEP3_SEEP3_EntityEnhance_atlsoapheader_entries,
        0,
        0,
        -1,
		SOAPFLAG_NULLABLE | SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
        0x184A7A62,
        "http://webservices.seisint.com/SEEP3",
        L"http://webservices.seisint.com/SEEP3",
        sizeof("http://webservices.seisint.com/SEEP3")-1
    };

    static const _soapmapentry __CSEEP3_SEEP3_EntityClassify_atlsoapheader_entries[] =
    {
        { 0x00000000 }
    };

    static const _soapmap __CSEEP3_SEEP3_EntityClassify_atlsoapheader_map =
    {
        0x3470C1F7,
        "SEEP3.EntityClassifyRequest",
        L"SEEP3.EntityClassifyResponse",
        sizeof("SEEP3.EntityClassifyRequest")-1,
        sizeof("SEEP3.EntityClassifyResponse")-1,
        SOAPMAP_HEADER,
        __CSEEP3_SEEP3_EntityClassify_atlsoapheader_entries,
        0,
        0,
        -1,
		SOAPFLAG_NULLABLE | SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
        0x184A7A62,
        "http://webservices.seisint.com/SEEP3",
        L"http://webservices.seisint.com/SEEP3",
        sizeof("http://webservices.seisint.com/SEEP3")-1
    };

    static const _soapmapentry __CSEEP3_SEEP3_EntityResolve_atlsoapheader_entries[] =
    {
        { 0x00000000 }
    };

    static const _soapmap __CSEEP3_SEEP3_EntityResolve_atlsoapheader_map =
    {
        0x348553F9,
        "SEEP3.EntityResolveRequest",
        L"SEEP3.EntityResolveResponse",
        sizeof("SEEP3.EntityResolveRequest")-1,
        sizeof("SEEP3.EntityResolveResponse")-1,
        SOAPMAP_HEADER,
        __CSEEP3_SEEP3_EntityResolve_atlsoapheader_entries,
        0,
        0,
        -1,
		SOAPFLAG_NULLABLE | SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
        0x184A7A62,
        "http://webservices.seisint.com/SEEP3",
        L"http://webservices.seisint.com/SEEP3",
        sizeof("http://webservices.seisint.com/SEEP3")-1
    };


    static const _soapmap * __CSEEP3_headers[] =
    {
        &__CSEEP3_SEEP3_EntityEnhance_atlsoapheader_map,
        &__CSEEP3_SEEP3_EntityClassify_atlsoapheader_map,
        &__CSEEP3_SEEP3_EntityResolve_atlsoapheader_map,
        NULL
    };

    return __CSEEP3_headers;
}

template <typename TClient>
ATL_NOINLINE inline void * CSEEP3T<TClient>::GetHeaderValue()
{
    return this;
}

template <typename TClient>
ATL_NOINLINE inline const wchar_t * CSEEP3T<TClient>::GetNamespaceUri()
{
    return L"http://webservices.seisint.com/SEEP3";
}

template <typename TClient>
ATL_NOINLINE inline const char * CSEEP3T<TClient>::GetServiceName()
{
    return NULL;
}

template <typename TClient>
ATL_NOINLINE inline const char * CSEEP3T<TClient>::GetNamespaceUriA()
{
    return "http://webservices.seisint.com/SEEP3";
}

template <typename TClient>
ATL_NOINLINE inline HRESULT CSEEP3T<TClient>::CallFunction(
    void *,
    const wchar_t *, int,
    size_t)
{
    return E_NOTIMPL;
}

template <typename TClient>
ATL_NOINLINE inline HRESULT CSEEP3T<TClient>::GetClientReader(ISAXXMLReader **ppReader)
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

} // namespace SEEP3

template<>
inline HRESULT AtlCleanupValue<SEEP3::EspException>(SEEP3::EspException *pVal)
{
    pVal;
    AtlCleanupValue(&pVal->Code);
    AtlCleanupValue(&pVal->Audience);
    AtlCleanupValue(&pVal->Source);
    AtlCleanupValue(&pVal->Message);
    return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<SEEP3::EspException>(SEEP3::EspException *pVal, IAtlMemMgr *pMemMgr)
{
    pVal;
    pMemMgr;

    AtlCleanupValueEx(&pVal->Code, pMemMgr);
    AtlCleanupValueEx(&pVal->Audience, pMemMgr);
    AtlCleanupValueEx(&pVal->Source, pMemMgr);
    AtlCleanupValueEx(&pVal->Message, pMemMgr);
    return S_OK;
}
