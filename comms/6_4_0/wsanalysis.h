//
// sproxy.exe generated file
// do not modify this file
//
// Created: 09/03/2009@17:33:50
//

#pragma once


#if !defined(_WIN32_WINDOWS) && !defined(_WIN32_WINNT) && !defined(_WIN32_WCE)
#pragma message("warning: defining _WIN32_WINDOWS = 0x0410")
#define _WIN32_WINDOWS 0x0410
#endif

#include <atlsoap.h>

namespace SEEP3
{

struct RowType2
{
	unsigned __int64 id_type;
	unsigned __int64 id;
	unsigned __int64 score;
};

struct RowType
{
	BSTR sparam;
	BSTR orig;
	__int64 docid;
	__int64 pos;
	unsigned __int64 classify_score;
	BSTR cleanedinput;
	unsigned __int64 entity_type;
	unsigned __int64 local_entity_number;
	bool verified;
	bool resolved;
	RowType2 *identifications;
	int __identifications_nSizeIs;
};

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
		:TClient(_T("http://10.173.190.101:8022/SEEP3\?ver_=0"))
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

	HRESULT SEEP3_EntityResolve(
		BSTR cluster_, 
		BSTR queue_, 
		bool include_schemas_, 
		bool limitResults_, 
		RowType* EntityList, int EntityList_nSizeIs, 
		ArrayOfEspException* Exceptions, 
		BSTR* Results, 
		BSTR* Workunit, 
		bool* Async
	);

	HRESULT SEEP3_TestClassify(
		BSTR cluster_, 
		BSTR queue_, 
		bool include_schemas_, 
		bool limitResults_, 
		BSTR SParam, 
		BSTR* Contents, int Contents_nSizeIs, 
		BSTR* Categories, int Categories_nSizeIs, 
		BSTR Format, 
		ArrayOfEspException* Exceptions, 
		BSTR* Results, 
		BSTR* Workunit, 
		bool* Async
	);

	HRESULT SEEP3_EntityClassifyResolve(
		BSTR cluster_, 
		BSTR queue_, 
		bool include_schemas_, 
		bool limitResults_, 
		BSTR SParam, 
		BSTR* Contents, int Contents_nSizeIs, 
		BSTR* Categories, int Categories_nSizeIs, 
		BSTR Format, 
		ArrayOfEspException* Exceptions, 
		BSTR* Results, 
		BSTR* Workunit, 
		bool* Async
	);

	HRESULT SEEP3_EntityEnhance(
		BSTR cluster_, 
		BSTR queue_, 
		bool include_schemas_, 
		bool limitResults_, 
		RowType* EntityList, int EntityList_nSizeIs, 
		__int64 MaxDids, 
		__int64 MaxFull, 
		ArrayOfEspException* Exceptions, 
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
		ArrayOfEspException* Exceptions, 
		BSTR* Results, 
		BSTR* Workunit, 
		bool* Async
	);
};

typedef CSEEP3T<> CSEEP3;

__if_not_exists(__RowType2_entries)
{
extern __declspec(selectany) const _soapmapentry __RowType2_entries[] =
{
	{ 
		0x82DF8F6E, 
		"id_type", 
		L"id_type", 
		sizeof("id_type")-1, 
		SOAPTYPE_NONNEGATIVEINTEGER, 
		SOAPFLAG_FIELD, 
		offsetof(RowType2, id_type),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x00000DED, 
		"id", 
		L"id", 
		sizeof("id")-1, 
		SOAPTYPE_NONNEGATIVEINTEGER, 
		SOAPFLAG_FIELD, 
		offsetof(RowType2, id),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x085932BC, 
		"score", 
		L"score", 
		sizeof("score")-1, 
		SOAPTYPE_NONNEGATIVEINTEGER, 
		SOAPFLAG_FIELD, 
		offsetof(RowType2, score),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __RowType2_map =
{
	0x2960B86C,
	"Row",
	L"Row",
	sizeof("Row")-1,
	sizeof("Row")-1,
	SOAPMAP_STRUCT,
	__RowType2_entries,
	sizeof(RowType2),
	3,
	-1,
	SOAPFLAG_NONE,
	0x184A7A63,
	"http://webservices.seisint.com/SEEP3",
	L"http://webservices.seisint.com/SEEP3",
	sizeof("http://webservices.seisint.com/SEEP3")-1
};
}

__if_not_exists(__RowType_entries)
{
extern __declspec(selectany) const _soapmapentry __RowType_entries[] =
{
	{ 
		0x14631B64, 
		"sparam", 
		L"sparam", 
		sizeof("sparam")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RowType, sparam),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x003ED0F1, 
		"orig", 
		L"orig", 
		sizeof("orig")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RowType, orig),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x07502383, 
		"docid", 
		L"docid", 
		sizeof("docid")-1, 
		SOAPTYPE_INTEGER, 
		SOAPFLAG_FIELD, 
		offsetof(RowType, docid),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0001EB32, 
		"pos", 
		L"pos", 
		sizeof("pos")-1, 
		SOAPTYPE_INTEGER, 
		SOAPFLAG_FIELD, 
		offsetof(RowType, pos),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xDD087439, 
		"classify_score", 
		L"classify_score", 
		sizeof("classify_score")-1, 
		SOAPTYPE_NONNEGATIVEINTEGER, 
		SOAPFLAG_FIELD, 
		offsetof(RowType, classify_score),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xC07D419C, 
		"cleanedinput", 
		L"cleanedinput", 
		sizeof("cleanedinput")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RowType, cleanedinput),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0C1DE47E, 
		"entity_type", 
		L"entity_type", 
		sizeof("entity_type")-1, 
		SOAPTYPE_NONNEGATIVEINTEGER, 
		SOAPFLAG_FIELD, 
		offsetof(RowType, entity_type),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x9DF9624F, 
		"local_entity_number", 
		L"local_entity_number", 
		sizeof("local_entity_number")-1, 
		SOAPTYPE_NONNEGATIVEINTEGER, 
		SOAPFLAG_FIELD, 
		offsetof(RowType, local_entity_number),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x56A3F72E, 
		"verified", 
		L"verified", 
		sizeof("verified")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD, 
		offsetof(RowType, verified),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xA861E0E4, 
		"resolved", 
		L"resolved", 
		sizeof("resolved")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD, 
		offsetof(RowType, resolved),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x8CD8DDFD, 
		"identifications", 
		L"identifications", 
		sizeof("identifications")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(RowType, identifications),
		NULL, 
		&__RowType2_map, 
		10+1 
	},
	{ 
		0x8CD8DDFD, 
		"identifications", 
		L"identifications", 
		sizeof("identifications")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(RowType, __identifications_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __RowType_map =
{
	0xB3AD98FA,
	"Row",
	L"Row",
	sizeof("Row")-1,
	sizeof("Row")-1,
	SOAPMAP_STRUCT,
	__RowType_entries,
	sizeof(RowType),
	11,
	-1,
	SOAPFLAG_NONE,
	0x184A7A63,
	"http://webservices.seisint.com/SEEP3",
	L"http://webservices.seisint.com/SEEP3",
	sizeof("http://webservices.seisint.com/SEEP3")-1
};
}

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
	0x184A7A63,
	"http://webservices.seisint.com/SEEP3",
	L"http://webservices.seisint.com/SEEP3",
	sizeof("http://webservices.seisint.com/SEEP3")-1
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
	0x184A7A63,
	"http://webservices.seisint.com/SEEP3",
	L"http://webservices.seisint.com/SEEP3",
	sizeof("http://webservices.seisint.com/SEEP3")-1
};
}

struct __CSEEP3_SEEP3_EntityResolve_struct
{
	BSTR cluster_;
	BSTR queue_;
	bool include_schemas_;
	bool limitResults_;
	RowType *EntityList;
	int __EntityList_nSizeIs;
	ArrayOfEspException Exceptions;
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
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
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
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
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
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CSEEP3_SEEP3_EntityResolve_struct, EntityList),
		NULL,
		&__RowType_map,
		4+1,
	},
	{
		0xDC188619,
		"__EntityList_nSizeIs",
		L"__EntityList_nSizeIs",
		sizeof("__EntityList_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CSEEP3_SEEP3_EntityResolve_struct, __EntityList_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x7B0DBD02, 
		"Exceptions", 
		L"Exceptions", 
		sizeof("Exceptions")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CSEEP3_SEEP3_EntityResolve_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
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
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CSEEP3_SEEP3_EntityResolve_struct, Async),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CSEEP3_SEEP3_EntityResolve_map =
{
	0xFCBAF2BA,
	"SEEP3.EntityResolveRequest",
	L"SEEP3.EntityResolveResponse",
	sizeof("SEEP3.EntityResolveRequest")-1,
	sizeof("SEEP3.EntityResolveResponse")-1,
	SOAPMAP_FUNC,
	__CSEEP3_SEEP3_EntityResolve_entries,
	sizeof(__CSEEP3_SEEP3_EntityResolve_struct),
	4,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x184A7A63,
	"http://webservices.seisint.com/SEEP3",
	L"http://webservices.seisint.com/SEEP3",
	sizeof("http://webservices.seisint.com/SEEP3")-1
};


struct __CSEEP3_SEEP3_TestClassify_struct
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
	ArrayOfEspException Exceptions;
	BSTR Results;
	BSTR Workunit;
	bool Async;
};

extern __declspec(selectany) const _soapmapentry __CSEEP3_SEEP3_TestClassify_entries[] =
{

	{
		0xF054E0A1, 
		"cluster_", 
		L"cluster_", 
		sizeof("cluster_")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CSEEP3_SEEP3_TestClassify_struct, cluster_),
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
		offsetof(__CSEEP3_SEEP3_TestClassify_struct, queue_),
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
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CSEEP3_SEEP3_TestClassify_struct, include_schemas_),
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
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CSEEP3_SEEP3_TestClassify_struct, limitResults_),
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
		offsetof(__CSEEP3_SEEP3_TestClassify_struct, SParam),
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
		offsetof(__CSEEP3_SEEP3_TestClassify_struct, Contents),
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
		SOAPFLAG_NOMARSHAL,
		offsetof(__CSEEP3_SEEP3_TestClassify_struct, __Contents_nSizeIs),
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
		offsetof(__CSEEP3_SEEP3_TestClassify_struct, Categories),
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
		SOAPFLAG_NOMARSHAL,
		offsetof(__CSEEP3_SEEP3_TestClassify_struct, __Categories_nSizeIs),
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
		offsetof(__CSEEP3_SEEP3_TestClassify_struct, Format),
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
		offsetof(__CSEEP3_SEEP3_TestClassify_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0x9C238272, 
		"Results", 
		L"Results", 
		sizeof("Results")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CSEEP3_SEEP3_TestClassify_struct, Results),
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
		offsetof(__CSEEP3_SEEP3_TestClassify_struct, Workunit),
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
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CSEEP3_SEEP3_TestClassify_struct, Async),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CSEEP3_SEEP3_TestClassify_map =
{
	0x5584033B,
	"SEEP3.TestClassifyRequest",
	L"SEEP3.TestClassifyResponse",
	sizeof("SEEP3.TestClassifyRequest")-1,
	sizeof("SEEP3.TestClassifyResponse")-1,
	SOAPMAP_FUNC,
	__CSEEP3_SEEP3_TestClassify_entries,
	sizeof(__CSEEP3_SEEP3_TestClassify_struct),
	4,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x184A7A63,
	"http://webservices.seisint.com/SEEP3",
	L"http://webservices.seisint.com/SEEP3",
	sizeof("http://webservices.seisint.com/SEEP3")-1
};


struct __CSEEP3_SEEP3_EntityClassifyResolve_struct
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
	ArrayOfEspException Exceptions;
	BSTR Results;
	BSTR Workunit;
	bool Async;
};

extern __declspec(selectany) const _soapmapentry __CSEEP3_SEEP3_EntityClassifyResolve_entries[] =
{

	{
		0xF054E0A1, 
		"cluster_", 
		L"cluster_", 
		sizeof("cluster_")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CSEEP3_SEEP3_EntityClassifyResolve_struct, cluster_),
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
		offsetof(__CSEEP3_SEEP3_EntityClassifyResolve_struct, queue_),
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
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CSEEP3_SEEP3_EntityClassifyResolve_struct, include_schemas_),
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
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CSEEP3_SEEP3_EntityClassifyResolve_struct, limitResults_),
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
		offsetof(__CSEEP3_SEEP3_EntityClassifyResolve_struct, SParam),
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
		offsetof(__CSEEP3_SEEP3_EntityClassifyResolve_struct, Contents),
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
		SOAPFLAG_NOMARSHAL,
		offsetof(__CSEEP3_SEEP3_EntityClassifyResolve_struct, __Contents_nSizeIs),
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
		offsetof(__CSEEP3_SEEP3_EntityClassifyResolve_struct, Categories),
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
		SOAPFLAG_NOMARSHAL,
		offsetof(__CSEEP3_SEEP3_EntityClassifyResolve_struct, __Categories_nSizeIs),
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
		offsetof(__CSEEP3_SEEP3_EntityClassifyResolve_struct, Format),
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
		offsetof(__CSEEP3_SEEP3_EntityClassifyResolve_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0x9C238272, 
		"Results", 
		L"Results", 
		sizeof("Results")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CSEEP3_SEEP3_EntityClassifyResolve_struct, Results),
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
		offsetof(__CSEEP3_SEEP3_EntityClassifyResolve_struct, Workunit),
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
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CSEEP3_SEEP3_EntityClassifyResolve_struct, Async),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CSEEP3_SEEP3_EntityClassifyResolve_map =
{
	0x22F59918,
	"SEEP3.EntityClassifyResolveRequest",
	L"SEEP3.EntityClassifyResolveResponse",
	sizeof("SEEP3.EntityClassifyResolveRequest")-1,
	sizeof("SEEP3.EntityClassifyResolveResponse")-1,
	SOAPMAP_FUNC,
	__CSEEP3_SEEP3_EntityClassifyResolve_entries,
	sizeof(__CSEEP3_SEEP3_EntityClassifyResolve_struct),
	4,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x184A7A63,
	"http://webservices.seisint.com/SEEP3",
	L"http://webservices.seisint.com/SEEP3",
	sizeof("http://webservices.seisint.com/SEEP3")-1
};


struct __CSEEP3_SEEP3_EntityEnhance_struct
{
	BSTR cluster_;
	BSTR queue_;
	bool include_schemas_;
	bool limitResults_;
	RowType *EntityList;
	int __EntityList_nSizeIs;
	__int64 MaxDids;
	__int64 MaxFull;
	ArrayOfEspException Exceptions;
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
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
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
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
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
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CSEEP3_SEEP3_EntityEnhance_struct, EntityList),
		NULL,
		&__RowType_map,
		4+1,
	},
	{
		0xDC188619,
		"__EntityList_nSizeIs",
		L"__EntityList_nSizeIs",
		sizeof("__EntityList_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CSEEP3_SEEP3_EntityEnhance_struct, __EntityList_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x122B3D4A, 
		"MaxDids", 
		L"MaxDids", 
		sizeof("MaxDids")-1, 
		SOAPTYPE_INTEGER, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
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
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
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
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CSEEP3_SEEP3_EntityEnhance_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
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
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CSEEP3_SEEP3_EntityEnhance_struct, Async),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CSEEP3_SEEP3_EntityEnhance_map =
{
	0x6EE887CC,
	"SEEP3.EntityEnhanceRequest",
	L"SEEP3.EntityEnhanceResponse",
	sizeof("SEEP3.EntityEnhanceRequest")-1,
	sizeof("SEEP3.EntityEnhanceResponse")-1,
	SOAPMAP_FUNC,
	__CSEEP3_SEEP3_EntityEnhance_entries,
	sizeof(__CSEEP3_SEEP3_EntityEnhance_struct),
	4,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x184A7A63,
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
	ArrayOfEspException Exceptions;
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
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
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
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
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
		SOAPFLAG_NOMARSHAL,
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
		SOAPFLAG_NOMARSHAL,
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
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CSEEP3_SEEP3_EntityClassify_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
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
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CSEEP3_SEEP3_EntityClassify_struct, Async),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CSEEP3_SEEP3_EntityClassify_map =
{
	0x035A38D8,
	"SEEP3.EntityClassifyRequest",
	L"SEEP3.EntityClassifyResponse",
	sizeof("SEEP3.EntityClassifyRequest")-1,
	sizeof("SEEP3.EntityClassifyResponse")-1,
	SOAPMAP_FUNC,
	__CSEEP3_SEEP3_EntityClassify_entries,
	sizeof(__CSEEP3_SEEP3_EntityClassify_struct),
	4,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x184A7A63,
	"http://webservices.seisint.com/SEEP3",
	L"http://webservices.seisint.com/SEEP3",
	sizeof("http://webservices.seisint.com/SEEP3")-1
};

extern __declspec(selectany) const _soapmap * __CSEEP3_funcs[] =
{
	&__CSEEP3_SEEP3_EntityResolve_map,
	&__CSEEP3_SEEP3_TestClassify_map,
	&__CSEEP3_SEEP3_EntityClassifyResolve_map,
	&__CSEEP3_SEEP3_EntityEnhance_map,
	&__CSEEP3_SEEP3_EntityClassify_map,
	NULL
};

template <typename TClient>
inline HRESULT CSEEP3T<TClient>::SEEP3_EntityResolve(
		BSTR cluster_, 
		BSTR queue_, 
		bool include_schemas_, 
		bool limitResults_, 
		RowType* EntityList, int __EntityList_nSizeIs, 
		ArrayOfEspException* Exceptions, 
		BSTR* Results, 
		BSTR* Workunit, 
		bool* Async
	)
{
    if ( Exceptions == NULL )
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
	__params.__EntityList_nSizeIs = __EntityList_nSizeIs;

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
inline HRESULT CSEEP3T<TClient>::SEEP3_TestClassify(
		BSTR cluster_, 
		BSTR queue_, 
		bool include_schemas_, 
		bool limitResults_, 
		BSTR SParam, 
		BSTR* Contents, int __Contents_nSizeIs, 
		BSTR* Categories, int __Categories_nSizeIs, 
		BSTR Format, 
		ArrayOfEspException* Exceptions, 
		BSTR* Results, 
		BSTR* Workunit, 
		bool* Async
	)
{
    if ( Exceptions == NULL )
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
	__CSEEP3_SEEP3_TestClassify_struct __params;
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"/SEEP3/SEEP3.TestClassify\?ver_=1.0\"\r\n"));
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
inline HRESULT CSEEP3T<TClient>::SEEP3_EntityClassifyResolve(
		BSTR cluster_, 
		BSTR queue_, 
		bool include_schemas_, 
		bool limitResults_, 
		BSTR SParam, 
		BSTR* Contents, int __Contents_nSizeIs, 
		BSTR* Categories, int __Categories_nSizeIs, 
		BSTR Format, 
		ArrayOfEspException* Exceptions, 
		BSTR* Results, 
		BSTR* Workunit, 
		bool* Async
	)
{
    if ( Exceptions == NULL )
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
	__CSEEP3_SEEP3_EntityClassifyResolve_struct __params;
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"/SEEP3/SEEP3.EntityClassifyResolve\?ver_=1.0\"\r\n"));
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
inline HRESULT CSEEP3T<TClient>::SEEP3_EntityEnhance(
		BSTR cluster_, 
		BSTR queue_, 
		bool include_schemas_, 
		bool limitResults_, 
		RowType* EntityList, int __EntityList_nSizeIs, 
		__int64 MaxDids, 
		__int64 MaxFull, 
		ArrayOfEspException* Exceptions, 
		BSTR* Results, 
		BSTR* Workunit, 
		bool* Async
	)
{
    if ( Exceptions == NULL )
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
	__params.__EntityList_nSizeIs = __EntityList_nSizeIs;
	__params.MaxDids = MaxDids;
	__params.MaxFull = MaxFull;

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
		ArrayOfEspException* Exceptions, 
		BSTR* Results, 
		BSTR* Workunit, 
		bool* Async
	)
{
    if ( Exceptions == NULL )
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

	__atlsoap_hr = SetClientStruct(&__params, 4);
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
	static const _soapmapentry __CSEEP3_SEEP3_EntityResolve_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CSEEP3_SEEP3_EntityResolve_atlsoapheader_map = 
	{
		0xFCBAF2BA,
		"SEEP3.EntityResolveRequest",
		L"SEEP3.EntityResolveResponse",
		sizeof("SEEP3.EntityResolveRequest")-1,
		sizeof("SEEP3.EntityResolveResponse")-1,
		SOAPMAP_HEADER,
		__CSEEP3_SEEP3_EntityResolve_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x184A7A63,
		"http://webservices.seisint.com/SEEP3",
		L"http://webservices.seisint.com/SEEP3",
		sizeof("http://webservices.seisint.com/SEEP3")-1
	};

	static const _soapmapentry __CSEEP3_SEEP3_TestClassify_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CSEEP3_SEEP3_TestClassify_atlsoapheader_map = 
	{
		0x5584033B,
		"SEEP3.TestClassifyRequest",
		L"SEEP3.TestClassifyResponse",
		sizeof("SEEP3.TestClassifyRequest")-1,
		sizeof("SEEP3.TestClassifyResponse")-1,
		SOAPMAP_HEADER,
		__CSEEP3_SEEP3_TestClassify_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x184A7A63,
		"http://webservices.seisint.com/SEEP3",
		L"http://webservices.seisint.com/SEEP3",
		sizeof("http://webservices.seisint.com/SEEP3")-1
	};

	static const _soapmapentry __CSEEP3_SEEP3_EntityClassifyResolve_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CSEEP3_SEEP3_EntityClassifyResolve_atlsoapheader_map = 
	{
		0x22F59918,
		"SEEP3.EntityClassifyResolveRequest",
		L"SEEP3.EntityClassifyResolveResponse",
		sizeof("SEEP3.EntityClassifyResolveRequest")-1,
		sizeof("SEEP3.EntityClassifyResolveResponse")-1,
		SOAPMAP_HEADER,
		__CSEEP3_SEEP3_EntityClassifyResolve_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x184A7A63,
		"http://webservices.seisint.com/SEEP3",
		L"http://webservices.seisint.com/SEEP3",
		sizeof("http://webservices.seisint.com/SEEP3")-1
	};

	static const _soapmapentry __CSEEP3_SEEP3_EntityEnhance_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CSEEP3_SEEP3_EntityEnhance_atlsoapheader_map = 
	{
		0x6EE887CC,
		"SEEP3.EntityEnhanceRequest",
		L"SEEP3.EntityEnhanceResponse",
		sizeof("SEEP3.EntityEnhanceRequest")-1,
		sizeof("SEEP3.EntityEnhanceResponse")-1,
		SOAPMAP_HEADER,
		__CSEEP3_SEEP3_EntityEnhance_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x184A7A63,
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
		0x035A38D8,
		"SEEP3.EntityClassifyRequest",
		L"SEEP3.EntityClassifyResponse",
		sizeof("SEEP3.EntityClassifyRequest")-1,
		sizeof("SEEP3.EntityClassifyResponse")-1,
		SOAPMAP_HEADER,
		__CSEEP3_SEEP3_EntityClassify_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x184A7A63,
		"http://webservices.seisint.com/SEEP3",
		L"http://webservices.seisint.com/SEEP3",
		sizeof("http://webservices.seisint.com/SEEP3")-1
	};


	static const _soapmap * __CSEEP3_headers[] =
	{
		&__CSEEP3_SEEP3_EntityResolve_atlsoapheader_map,
		&__CSEEP3_SEEP3_TestClassify_atlsoapheader_map,
		&__CSEEP3_SEEP3_EntityClassifyResolve_atlsoapheader_map,
		&__CSEEP3_SEEP3_EntityEnhance_atlsoapheader_map,
		&__CSEEP3_SEEP3_EntityClassify_atlsoapheader_map,
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
inline HRESULT AtlCleanupValue<SEEP3::RowType2>(SEEP3::RowType2 *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->id_type);
	AtlCleanupValue(&pVal->id);
	AtlCleanupValue(&pVal->score);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<SEEP3::RowType2>(SEEP3::RowType2 *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->id_type, pMemMgr);
	AtlCleanupValueEx(&pVal->id, pMemMgr);
	AtlCleanupValueEx(&pVal->score, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<SEEP3::RowType>(SEEP3::RowType *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->sparam);
	AtlCleanupValue(&pVal->orig);
	AtlCleanupValue(&pVal->docid);
	AtlCleanupValue(&pVal->pos);
	AtlCleanupValue(&pVal->classify_score);
	AtlCleanupValue(&pVal->cleanedinput);
	AtlCleanupValue(&pVal->entity_type);
	AtlCleanupValue(&pVal->local_entity_number);
	AtlCleanupValue(&pVal->verified);
	AtlCleanupValue(&pVal->resolved);
	AtlCleanupArray(pVal->identifications, pVal->__identifications_nSizeIs);	
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<SEEP3::RowType>(SEEP3::RowType *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->sparam, pMemMgr);
	AtlCleanupValueEx(&pVal->orig, pMemMgr);
	AtlCleanupValueEx(&pVal->docid, pMemMgr);
	AtlCleanupValueEx(&pVal->pos, pMemMgr);
	AtlCleanupValueEx(&pVal->classify_score, pMemMgr);
	AtlCleanupValueEx(&pVal->cleanedinput, pMemMgr);
	AtlCleanupValueEx(&pVal->entity_type, pMemMgr);
	AtlCleanupValueEx(&pVal->local_entity_number, pMemMgr);
	AtlCleanupValueEx(&pVal->verified, pMemMgr);
	AtlCleanupValueEx(&pVal->resolved, pMemMgr);
	if (pVal->identifications != NULL)
	{
		AtlCleanupArrayEx(pVal->identifications, pVal->__identifications_nSizeIs, pMemMgr);		
		pMemMgr->Free(pVal->identifications);
	}
	return S_OK;
}

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

template<>
inline HRESULT AtlCleanupValue<SEEP3::ArrayOfEspException>(SEEP3::ArrayOfEspException *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Source);
	AtlCleanupArray(pVal->Exception, pVal->__Exception_nSizeIs);	
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<SEEP3::ArrayOfEspException>(SEEP3::ArrayOfEspException *pVal, IAtlMemMgr *pMemMgr)
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
