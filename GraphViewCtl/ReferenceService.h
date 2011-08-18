//
// sproxy.exe generated file
// do not modify this file
//
// Created: 02/27/2007@15:41:45
//

#pragma once


#ifndef _WIN32_WINDOWS
#pragma message("warning: defining _WIN32_WINDOWS = 0x0410")
#define _WIN32_WINDOWS 0x0410
#endif

#include <atlsoap.h>

namespace Service
{

struct VertexColor
{
	unsigned int Border;
	unsigned int Fill;
};

struct UniqueID
{
	int Category;
	BSTR ID;
};

struct VertexMetaInformation
{
	int Category;
	BSTR Label;
	VertexColor Normal;
	VertexColor Selected;
	VertexColor Hot;
};

struct Property
{
	int Key;
	BSTR Value;
};

struct Edge
{
	UniqueID ID;
	UniqueID From;
	UniqueID To;
	Property *Properties;
	int __Properties_nSizeIs;
};

struct EdgeMetaInformation
{
	int Category;
	BSTR Label;
	unsigned int Normal;
	unsigned int Shortest;
	unsigned int Width;
};

struct Vertex
{
	UniqueID ID;
	Property *Properties;
	int __Properties_nSizeIs;
};

template <typename TClient = CSoapSocketClientT<> >
class CServiceT : 
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

	CServiceT(ISAXXMLReader *pReader = NULL)
		:TClient(_T("http://localhost/SeisintGraphView/SampleService.asmx"))
	{
		SetClient(true);
		SetReader(pReader);
	}
	
	~CServiceT()
	{
		Uninitialize();
	}
	
	void Uninitialize()
	{
		UninitializeSOAP();
	}	

	HRESULT GetMeta(
		VertexMetaInformation** vertexInformation, int* vertexInformation_nSizeIs, 
		EdgeMetaInformation** edgeInformation, int* edgeInformation_nSizeIs
	);

	HRESULT GetData(
		UniqueID id, 
		Vertex** vertices, int* vertices_nSizeIs, 
		Edge** edges, int* edges_nSizeIs
	);
};

typedef CServiceT<> CService;

__if_not_exists(__VertexColor_entries)
{
extern __declspec(selectany) const _soapmapentry __VertexColor_entries[] =
{
	{ 
		0xA20D537E, 
		"Border", 
		L"Border", 
		sizeof("Border")-1, 
		SOAPTYPE_UNSIGNEDINT, 
		SOAPFLAG_FIELD, 
		offsetof(VertexColor, Border),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x00282F87, 
		"Fill", 
		L"Fill", 
		sizeof("Fill")-1, 
		SOAPTYPE_UNSIGNEDINT, 
		SOAPFLAG_FIELD, 
		offsetof(VertexColor, Fill),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __VertexColor_map =
{
	0xCE2F13DD,
	"VertexColor",
	L"VertexColor",
	sizeof("VertexColor")-1,
	sizeof("VertexColor")-1,
	SOAPMAP_STRUCT,
	__VertexColor_entries,
	sizeof(VertexColor),
	2,
	-1,
	SOAPFLAG_NONE,
	0x24819104,
	"http://seisint.com/webservices/",
	L"http://seisint.com/webservices/",
	sizeof("http://seisint.com/webservices/")-1
};
}

__if_not_exists(__UniqueID_entries)
{
extern __declspec(selectany) const _soapmapentry __UniqueID_entries[] =
{
	{ 
		0x1600323E, 
		"Category", 
		L"Category", 
		sizeof("Category")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(UniqueID, Category),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x000009AD, 
		"ID", 
		L"ID", 
		sizeof("ID")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(UniqueID, ID),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __UniqueID_map =
{
	0x828E2444,
	"UniqueID",
	L"UniqueID",
	sizeof("UniqueID")-1,
	sizeof("UniqueID")-1,
	SOAPMAP_STRUCT,
	__UniqueID_entries,
	sizeof(UniqueID),
	2,
	-1,
	SOAPFLAG_NONE,
	0x24819104,
	"http://seisint.com/webservices/",
	L"http://seisint.com/webservices/",
	sizeof("http://seisint.com/webservices/")-1
};
}

__if_not_exists(__VertexMetaInformation_entries)
{
extern __declspec(selectany) const _soapmapentry __VertexMetaInformation_entries[] =
{
	{ 
		0x1600323E, 
		"Category", 
		L"Category", 
		sizeof("Category")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(VertexMetaInformation, Category),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x05962560, 
		"Label", 
		L"Label", 
		sizeof("Label")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(VertexMetaInformation, Label),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xBE0B60C9, 
		"Normal", 
		L"Normal", 
		sizeof("Normal")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD, 
		offsetof(VertexMetaInformation, Normal),
		NULL, 
		&__VertexColor_map, 
		-1 
	},
	{ 
		0xFB5B1049, 
		"Selected", 
		L"Selected", 
		sizeof("Selected")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD, 
		offsetof(VertexMetaInformation, Selected),
		NULL, 
		&__VertexColor_map, 
		-1 
	},
	{ 
		0x0001410B, 
		"Hot", 
		L"Hot", 
		sizeof("Hot")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD, 
		offsetof(VertexMetaInformation, Hot),
		NULL, 
		&__VertexColor_map, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __VertexMetaInformation_map =
{
	0xFCFC3B8B,
	"VertexMetaInformation",
	L"VertexMetaInformation",
	sizeof("VertexMetaInformation")-1,
	sizeof("VertexMetaInformation")-1,
	SOAPMAP_STRUCT,
	__VertexMetaInformation_entries,
	sizeof(VertexMetaInformation),
	5,
	-1,
	SOAPFLAG_NONE,
	0x24819104,
	"http://seisint.com/webservices/",
	L"http://seisint.com/webservices/",
	sizeof("http://seisint.com/webservices/")-1
};
}

__if_not_exists(__Property_entries)
{
extern __declspec(selectany) const _soapmapentry __Property_entries[] =
{
	{ 
		0x00014C89, 
		"Key", 
		L"Key", 
		sizeof("Key")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(Property, Key),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x064B46FD, 
		"Value", 
		L"Value", 
		sizeof("Value")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(Property, Value),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __Property_map =
{
	0x27124125,
	"Property",
	L"Property",
	sizeof("Property")-1,
	sizeof("Property")-1,
	SOAPMAP_STRUCT,
	__Property_entries,
	sizeof(Property),
	2,
	-1,
	SOAPFLAG_NONE,
	0x24819104,
	"http://seisint.com/webservices/",
	L"http://seisint.com/webservices/",
	sizeof("http://seisint.com/webservices/")-1
};
}

__if_not_exists(__Edge_entries)
{
extern __declspec(selectany) const _soapmapentry __Edge_entries[] =
{
	{ 
		0x000009AD, 
		"ID", 
		L"ID", 
		sizeof("ID")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD, 
		offsetof(Edge, ID),
		NULL, 
		&__UniqueID_map, 
		-1 
	},
	{ 
		0x00285634, 
		"From", 
		L"From", 
		sizeof("From")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD, 
		offsetof(Edge, From),
		NULL, 
		&__UniqueID_map, 
		-1 
	},
	{ 
		0x00000B43, 
		"To", 
		L"To", 
		sizeof("To")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD, 
		offsetof(Edge, To),
		NULL, 
		&__UniqueID_map, 
		-1 
	},
	{ 
		0x34A6E7CD, 
		"Properties", 
		L"Properties", 
		sizeof("Properties")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(Edge, Properties),
		NULL, 
		&__Property_map, 
		3+1 
	},
	{ 
		0x34A6E7CD, 
		"Properties", 
		L"Properties", 
		sizeof("Properties")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(Edge, __Properties_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __Edge_map =
{
	0x00278D35,
	"Edge",
	L"Edge",
	sizeof("Edge")-1,
	sizeof("Edge")-1,
	SOAPMAP_STRUCT,
	__Edge_entries,
	sizeof(Edge),
	4,
	-1,
	SOAPFLAG_NONE,
	0x24819104,
	"http://seisint.com/webservices/",
	L"http://seisint.com/webservices/",
	sizeof("http://seisint.com/webservices/")-1
};
}

__if_not_exists(__EdgeMetaInformation_entries)
{
extern __declspec(selectany) const _soapmapentry __EdgeMetaInformation_entries[] =
{
	{ 
		0x1600323E, 
		"Category", 
		L"Category", 
		sizeof("Category")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD, 
		offsetof(EdgeMetaInformation, Category),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x05962560, 
		"Label", 
		L"Label", 
		sizeof("Label")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(EdgeMetaInformation, Label),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xBE0B60C9, 
		"Normal", 
		L"Normal", 
		sizeof("Normal")-1, 
		SOAPTYPE_UNSIGNEDINT, 
		SOAPFLAG_FIELD, 
		offsetof(EdgeMetaInformation, Normal),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xEA3D937C, 
		"Shortest", 
		L"Shortest", 
		sizeof("Shortest")-1, 
		SOAPTYPE_UNSIGNEDINT, 
		SOAPFLAG_FIELD, 
		offsetof(EdgeMetaInformation, Shortest),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0661A060, 
		"Width", 
		L"Width", 
		sizeof("Width")-1, 
		SOAPTYPE_UNSIGNEDINT, 
		SOAPFLAG_FIELD, 
		offsetof(EdgeMetaInformation, Width),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __EdgeMetaInformation_map =
{
	0x7D26A0C2,
	"EdgeMetaInformation",
	L"EdgeMetaInformation",
	sizeof("EdgeMetaInformation")-1,
	sizeof("EdgeMetaInformation")-1,
	SOAPMAP_STRUCT,
	__EdgeMetaInformation_entries,
	sizeof(EdgeMetaInformation),
	5,
	-1,
	SOAPFLAG_NONE,
	0x24819104,
	"http://seisint.com/webservices/",
	L"http://seisint.com/webservices/",
	sizeof("http://seisint.com/webservices/")-1
};
}

__if_not_exists(__Vertex_entries)
{
extern __declspec(selectany) const _soapmapentry __Vertex_entries[] =
{
	{ 
		0x000009AD, 
		"ID", 
		L"ID", 
		sizeof("ID")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD, 
		offsetof(Vertex, ID),
		NULL, 
		&__UniqueID_map, 
		-1 
	},
	{ 
		0x34A6E7CD, 
		"Properties", 
		L"Properties", 
		sizeof("Properties")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(Vertex, Properties),
		NULL, 
		&__Property_map, 
		1+1 
	},
	{ 
		0x34A6E7CD, 
		"Properties", 
		L"Properties", 
		sizeof("Properties")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(Vertex, __Properties_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __Vertex_map =
{
	0xCFFFCF1E,
	"Vertex",
	L"Vertex",
	sizeof("Vertex")-1,
	sizeof("Vertex")-1,
	SOAPMAP_STRUCT,
	__Vertex_entries,
	sizeof(Vertex),
	2,
	-1,
	SOAPFLAG_NONE,
	0x24819104,
	"http://seisint.com/webservices/",
	L"http://seisint.com/webservices/",
	sizeof("http://seisint.com/webservices/")-1
};
}

struct __CService_GetMeta_struct
{
	VertexMetaInformation *vertexInformation;
	int __vertexInformation_nSizeIs;
	EdgeMetaInformation *edgeInformation;
	int __edgeInformation_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CService_GetMeta_entries[] =
{

	{
		0x3DCA0CE4, 
		"vertexInformation", 
		L"vertexInformation", 
		sizeof("vertexInformation")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CService_GetMeta_struct, vertexInformation),
		NULL,
		&__VertexMetaInformation_map,
		0+1,
	},
	{
		0x3DCA0CE4,
		"__vertexInformation_nSizeIs",
		L"__vertexInformation_nSizeIs",
		sizeof("__vertexInformation_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CService_GetMeta_struct, __vertexInformation_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xBA6DE69B, 
		"edgeInformation", 
		L"edgeInformation", 
		sizeof("edgeInformation")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CService_GetMeta_struct, edgeInformation),
		NULL,
		&__EdgeMetaInformation_map,
		2+1,
	},
	{
		0xBA6DE69B,
		"__edgeInformation_nSizeIs",
		L"__edgeInformation_nSizeIs",
		sizeof("__edgeInformation_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CService_GetMeta_struct, __edgeInformation_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CService_GetMeta_map =
{
	0x0F288AD6,
	"GetMeta",
	L"GetMetaResponse",
	sizeof("GetMeta")-1,
	sizeof("GetMetaResponse")-1,
	SOAPMAP_FUNC,
	__CService_GetMeta_entries,
	sizeof(__CService_GetMeta_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x24819104,
	"http://seisint.com/webservices/",
	L"http://seisint.com/webservices/",
	sizeof("http://seisint.com/webservices/")-1
};


struct __CService_GetData_struct
{
	UniqueID id;
	Vertex *vertices;
	int __vertices_nSizeIs;
	Edge *edges;
	int __edges_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CService_GetData_entries[] =
{

	{
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		offsetof(__CService_GetData_struct, id),
		NULL,
		&__UniqueID_map,
		-1,
	},
	{
		0x576C9065, 
		"vertices", 
		L"vertices", 
		sizeof("vertices")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CService_GetData_struct, vertices),
		NULL,
		&__Vertex_map,
		1+1,
	},
	{
		0x576C9065,
		"__vertices_nSizeIs",
		L"__vertices_nSizeIs",
		sizeof("__vertices_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CService_GetData_struct, __vertices_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x075C4468, 
		"edges", 
		L"edges", 
		sizeof("edges")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CService_GetData_struct, edges),
		NULL,
		&__Edge_map,
		3+1,
	},
	{
		0x075C4468,
		"__edges_nSizeIs",
		L"__edges_nSizeIs",
		sizeof("__edges_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CService_GetData_struct, __edges_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CService_GetData_map =
{
	0x41276D69,
	"GetData",
	L"GetDataResponse",
	sizeof("GetData")-1,
	sizeof("GetDataResponse")-1,
	SOAPMAP_FUNC,
	__CService_GetData_entries,
	sizeof(__CService_GetData_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x24819104,
	"http://seisint.com/webservices/",
	L"http://seisint.com/webservices/",
	sizeof("http://seisint.com/webservices/")-1
};

extern __declspec(selectany) const _soapmap * __CService_funcs[] =
{
	&__CService_GetMeta_map,
	&__CService_GetData_map,
	NULL
};

template <typename TClient>
inline HRESULT CServiceT<TClient>::GetMeta(
		VertexMetaInformation** vertexInformation, int* __vertexInformation_nSizeIs, 
		EdgeMetaInformation** edgeInformation, int* __edgeInformation_nSizeIs
	)
{
    if ( vertexInformation == NULL )
		return E_POINTER;
    if( __vertexInformation_nSizeIs == NULL )
		return E_POINTER;
    if ( edgeInformation == NULL )
		return E_POINTER;
    if( __edgeInformation_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CService_GetMeta_struct __params;
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://seisint.com/webservices/GetMeta\"\r\n"));
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

	*vertexInformation = __params.vertexInformation;
	*__vertexInformation_nSizeIs = __params.__vertexInformation_nSizeIs;
	*edgeInformation = __params.edgeInformation;
	*__edgeInformation_nSizeIs = __params.__edgeInformation_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CServiceT<TClient>::GetData(
		UniqueID id, 
		Vertex** vertices, int* __vertices_nSizeIs, 
		Edge** edges, int* __edges_nSizeIs
	)
{
    if ( vertices == NULL )
		return E_POINTER;
    if( __vertices_nSizeIs == NULL )
		return E_POINTER;
    if ( edges == NULL )
		return E_POINTER;
    if( __edges_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CService_GetData_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.id = id;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://seisint.com/webservices/GetData\"\r\n"));
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

	*vertices = __params.vertices;
	*__vertices_nSizeIs = __params.__vertices_nSizeIs;
	*edges = __params.edges;
	*__edges_nSizeIs = __params.__edges_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
ATL_NOINLINE inline const _soapmap ** CServiceT<TClient>::GetFunctionMap()
{
	return __CService_funcs;
}

template <typename TClient>
ATL_NOINLINE inline const _soapmap ** CServiceT<TClient>::GetHeaderMap()
{
	static const _soapmapentry __CService_GetMeta_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CService_GetMeta_atlsoapheader_map = 
	{
		0x0F288AD6,
		"GetMeta",
		L"GetMetaResponse",
		sizeof("GetMeta")-1,
		sizeof("GetMetaResponse")-1,
		SOAPMAP_HEADER,
		__CService_GetMeta_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x24819104,
		"http://seisint.com/webservices/",
		L"http://seisint.com/webservices/",
		sizeof("http://seisint.com/webservices/")-1
	};

	static const _soapmapentry __CService_GetData_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CService_GetData_atlsoapheader_map = 
	{
		0x41276D69,
		"GetData",
		L"GetDataResponse",
		sizeof("GetData")-1,
		sizeof("GetDataResponse")-1,
		SOAPMAP_HEADER,
		__CService_GetData_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x24819104,
		"http://seisint.com/webservices/",
		L"http://seisint.com/webservices/",
		sizeof("http://seisint.com/webservices/")-1
	};


	static const _soapmap * __CService_headers[] =
	{
		&__CService_GetMeta_atlsoapheader_map,
		&__CService_GetData_atlsoapheader_map,
		NULL
	};
	
	return __CService_headers;
}

template <typename TClient>
ATL_NOINLINE inline void * CServiceT<TClient>::GetHeaderValue()
{
	return this;
}

template <typename TClient>
ATL_NOINLINE inline const wchar_t * CServiceT<TClient>::GetNamespaceUri()
{
	return L"http://seisint.com/webservices/";
}

template <typename TClient>
ATL_NOINLINE inline const char * CServiceT<TClient>::GetServiceName()
{
	return NULL;
}

template <typename TClient>
ATL_NOINLINE inline const char * CServiceT<TClient>::GetNamespaceUriA()
{
	return "http://seisint.com/webservices/";
}

template <typename TClient>
ATL_NOINLINE inline HRESULT CServiceT<TClient>::CallFunction(
	void *, 
	const wchar_t *, int,
	size_t)
{
	return E_NOTIMPL;
}

template <typename TClient>
ATL_NOINLINE inline HRESULT CServiceT<TClient>::GetClientReader(ISAXXMLReader **ppReader)
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

} // namespace Service

template<>
inline HRESULT AtlCleanupValue<Service::VertexColor>(Service::VertexColor *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Border);
	AtlCleanupValue(&pVal->Fill);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<Service::VertexColor>(Service::VertexColor *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Border, pMemMgr);
	AtlCleanupValueEx(&pVal->Fill, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<Service::UniqueID>(Service::UniqueID *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Category);
	AtlCleanupValue(&pVal->ID);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<Service::UniqueID>(Service::UniqueID *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Category, pMemMgr);
	AtlCleanupValueEx(&pVal->ID, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<Service::VertexMetaInformation>(Service::VertexMetaInformation *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Category);
	AtlCleanupValue(&pVal->Label);
	AtlCleanupValue(&pVal->Normal);
	AtlCleanupValue(&pVal->Selected);
	AtlCleanupValue(&pVal->Hot);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<Service::VertexMetaInformation>(Service::VertexMetaInformation *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Category, pMemMgr);
	AtlCleanupValueEx(&pVal->Label, pMemMgr);
	AtlCleanupValueEx(&pVal->Normal, pMemMgr);
	AtlCleanupValueEx(&pVal->Selected, pMemMgr);
	AtlCleanupValueEx(&pVal->Hot, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<Service::Property>(Service::Property *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Key);
	AtlCleanupValue(&pVal->Value);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<Service::Property>(Service::Property *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Key, pMemMgr);
	AtlCleanupValueEx(&pVal->Value, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<Service::Edge>(Service::Edge *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->ID);
	AtlCleanupValue(&pVal->From);
	AtlCleanupValue(&pVal->To);
	AtlCleanupArray(pVal->Properties, pVal->__Properties_nSizeIs);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<Service::Edge>(Service::Edge *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->ID, pMemMgr);
	AtlCleanupValueEx(&pVal->From, pMemMgr);
	AtlCleanupValueEx(&pVal->To, pMemMgr);
	if (pVal->Properties != NULL)
	{
		AtlCleanupArrayEx(pVal->Properties, pVal->__Properties_nSizeIs, pMemMgr);
		pMemMgr->Free(pVal->Properties);
	}
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<Service::EdgeMetaInformation>(Service::EdgeMetaInformation *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Category);
	AtlCleanupValue(&pVal->Label);
	AtlCleanupValue(&pVal->Normal);
	AtlCleanupValue(&pVal->Shortest);
	AtlCleanupValue(&pVal->Width);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<Service::EdgeMetaInformation>(Service::EdgeMetaInformation *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Category, pMemMgr);
	AtlCleanupValueEx(&pVal->Label, pMemMgr);
	AtlCleanupValueEx(&pVal->Normal, pMemMgr);
	AtlCleanupValueEx(&pVal->Shortest, pMemMgr);
	AtlCleanupValueEx(&pVal->Width, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<Service::Vertex>(Service::Vertex *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->ID);
	AtlCleanupArray(pVal->Properties, pVal->__Properties_nSizeIs);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<Service::Vertex>(Service::Vertex *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->ID, pMemMgr);
	if (pVal->Properties != NULL)
	{
		AtlCleanupArrayEx(pVal->Properties, pVal->__Properties_nSizeIs, pMemMgr);
		pMemMgr->Free(pVal->Properties);
	}
	return S_OK;
}
