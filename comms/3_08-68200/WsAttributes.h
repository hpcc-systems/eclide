//
// sproxy.exe generated file
// do not modify this file
//
// Created: 10/28/2009@09:13:36
//

#pragma once


#if !defined(_WIN32_WINDOWS) && !defined(_WIN32_WINNT) && !defined(_WIN32_WCE)
#pragma message("warning: defining _WIN32_WINDOWS = 0x0410")
#define _WIN32_WINDOWS 0x0410
#endif

#include <atlsoap.h>

namespace WsAttributes
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

struct SaveAttributeRequest
{
	BSTR ModuleName;
	BSTR AttributeName;
	int Flags;
	int ResultType;
	BSTR Description;
	BSTR Text;
};

struct CheckinAttributeRequest
{
	BSTR ModuleName;
	BSTR AttributeName;
	BSTR Description;
};

struct CheckoutAttributeRequest
{
	BSTR ModuleName;
	BSTR AttributeName;
};

struct RenameAttributeRequest
{
	BSTR ModuleName;
	BSTR AttributeName;
	BSTR NewModuleName;
	BSTR NewAttributeName;
};

struct RollbackAttributeRequest
{
	BSTR ModuleName;
	BSTR AttributeName;
};

struct ECLLabel
{
	BSTR Name;
	BSTR ModifiedDate;
};

struct ImportAttributeRequest
{
	BSTR ModuleName;
	BSTR AttributeName;
	BSTR Type;
	int Flags;
	int ResultType;
	BSTR Description;
	BSTR Text;
};

struct DeleteAttributeRequest
{
	BSTR ModuleName;
	BSTR AttributeName;
};

struct ECLModule
{
	BSTR Name;
	int Flags;
	int Access;
	__int64 Timestamp;
	bool IsPlugin;
	int Rank;
	BSTR FileName;
	BSTR FileCRC;
	BSTR Version;
	BSTR Checksum;
};

struct ECLAttributeLabel
{
	BSTR ModuleName;
	BSTR AttributeName;
	int AttributeVersion;
	BSTR LabelName;
	BSTR ModifiedDate;
};

struct AttributeType
{
	int ID;
	BSTR Name;
	BSTR Description;
};

struct ECLTextLine
{
	int LineNumber;
	BSTR LineText;
};

struct ECLAttribute
{
	BSTR ModuleName;
	BSTR Name;
	BSTR Type;
	int Version;
	int LatestVersion;
	int SandboxVersion;
	int Flags;
	int Access;
	bool IsLocked;
	bool IsCheckedOut;
	bool IsSandbox;
	bool IsOrphaned;
	int ResultType;
	BSTR LockedBy;
	BSTR ModifiedBy;
	BSTR ModifiedDate;
	BSTR Description;
	BSTR Checksum;
	BSTR Text;
	ECLTextLine *MatchedLines;
	int __MatchedLines_nSizeIs;
};

template <typename TClient = CSoapSocketClientT<> >
class CWsAttributesT : 
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

	CWsAttributesT(ISAXXMLReader *pReader = NULL)
		:TClient(_T("http://10.173.88.201:8145/WsAttributes\?ver_=1.2"))
	{
		SetClient(true);
		SetReader(pReader);
	}
	
	~CWsAttributesT()
	{
		Uninitialize();
	}
	
	void Uninitialize()
	{
		UninitializeSOAP();
	}	

	HRESULT GetAttribute(
		BSTR ModuleName, 
		BSTR AttributeName, 
		BSTR Type, 
		int Version, 
		bool GetSandbox, 
		bool GetText, 
		BSTR Label, 
		ArrayOfEspException* Exceptions, 
		ECLAttribute* outAttribute
	);

	HRESULT CreateModule(
		BSTR ModuleName, 
		ArrayOfEspException* Exceptions, 
		ECLModule* ModuleInfo
	);

	HRESULT CreateAttribute(
		BSTR ModuleName, 
		BSTR AttributeName, 
		BSTR Type, 
		ArrayOfEspException* Exceptions, 
		ECLAttribute* AttributeInfo
	);

	HRESULT CheckinAttributes(
		CheckinAttributeRequest* Attributes, int Attributes_nSizeIs, 
		ArrayOfEspException* Exceptions, 
		ECLAttribute** outAttributes, int* outAttributes_nSizeIs
	);

	HRESULT RenameAttributes(
		RenameAttributeRequest* Attributes, int Attributes_nSizeIs, 
		ArrayOfEspException* Exceptions, 
		ECLAttribute** outAttributes, int* outAttributes_nSizeIs
	);

	HRESULT DeleteAttributes(
		DeleteAttributeRequest* Attributes, int Attributes_nSizeIs, 
		ArrayOfEspException* Exceptions, 
		ECLAttribute** outAttributes, int* outAttributes_nSizeIs
	);

	HRESULT GetLabels(
		BSTR Name, 
		ArrayOfEspException* Exceptions, 
		ECLLabel** outLabels, int* outLabels_nSizeIs
	);

	HRESULT GetAttributeLabels(
		BSTR ModuleName, 
		BSTR AttributeName, 
		ArrayOfEspException* Exceptions, 
		ECLAttributeLabel** Labels, int* Labels_nSizeIs
	);

	HRESULT GetAttributeHistory(
		BSTR ModuleName, 
		BSTR AttributeName, 
		BSTR FromDate, 
		BSTR ToDate, 
		ArrayOfEspException* Exceptions, 
		ECLAttribute** outAttributes, int* outAttributes_nSizeIs
	);

	HRESULT UpdateLabel(
		BSTR Name, 
		BSTR ModuleName, 
		BSTR AtributeName, 
		int AttrbuteVersion, 
		ArrayOfEspException* Exceptions
	);

	HRESULT ListAttributeTypes(
		AttributeType** Types, int* Types_nSizeIs
	);

	HRESULT ImportAttributes(
		ImportAttributeRequest* Attributes, int Attributes_nSizeIs, 
		ArrayOfEspException* Exceptions, 
		ECLAttribute** outAttributes, int* outAttributes_nSizeIs
	);

	HRESULT DeleteLabel(
		BSTR Name, 
		ArrayOfEspException* Exceptions
	);

	HRESULT GetModules(
		__int64 ModifiedSince, 
		bool IncludeDeleted, 
		bool GetChecksum, 
		BSTR Label, 
		BSTR EarMark, 
		ArrayOfEspException* Exceptions, 
		ECLModule** outModules, int* outModules_nSizeIs
	);

	HRESULT CreateLabel(
		BSTR Name, 
		ArrayOfEspException* Exceptions
	);

	HRESULT CheckoutAttributes(
		CheckoutAttributeRequest* Attributes, int Attributes_nSizeIs, 
		ArrayOfEspException* Exceptions, 
		ECLAttribute** outAttributes, int* outAttributes_nSizeIs
	);

	HRESULT RenameModule(
		BSTR ModuleName, 
		BSTR NewModuleName, 
		ArrayOfEspException* Exceptions, 
		ECLModule* ModuleInfo
	);

	HRESULT GetAttributes(
		BSTR ModuleName, 
		BSTR* TypeList, int TypeList_nSizeIs, 
		BSTR Label, 
		ArrayOfEspException* Exceptions, 
		ECLAttribute** outAttributes, int* outAttributes_nSizeIs
	);

	HRESULT SupportsEarMark(
		bool* Support
	);

	HRESULT SaveAttributes(
		SaveAttributeRequest* Attributes, int Attributes_nSizeIs, 
		ArrayOfEspException* Exceptions, 
		ECLAttribute** outAttributes, int* outAttributes_nSizeIs
	);

	HRESULT RollbackAttributes(
		RollbackAttributeRequest* Attributes, int Attributes_nSizeIs, 
		ArrayOfEspException* Exceptions, 
		ECLAttribute** outAttributes, int* outAttributes_nSizeIs
	);

	HRESULT DeleteModule(
		BSTR ModuleName, 
		ArrayOfEspException* Exceptions
	);

	HRESULT FindAttributes(
		BSTR ModuleName, 
		BSTR AttributeName, 
		BSTR UserName, 
		BSTR Pattern, 
		BSTR Regexp, 
		BSTR ChangedSince, 
		BSTR* TypeList, int TypeList_nSizeIs, 
		bool Sandboxed, 
		bool Locked, 
		bool CheckedOut, 
		bool Orphaned, 
		bool GetText, 
		bool GetMatchedLines, 
		ArrayOfEspException* Exceptions, 
		ECLAttribute** outAttributes, int* outAttributes_nSizeIs
	);
};

typedef CWsAttributesT<> CWsAttributes;

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
	0xAA017D94,
	"http://webservices.seisint.com/WsAttributes",
	L"http://webservices.seisint.com/WsAttributes",
	sizeof("http://webservices.seisint.com/WsAttributes")-1
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
	0xAA017D94,
	"http://webservices.seisint.com/WsAttributes",
	L"http://webservices.seisint.com/WsAttributes",
	sizeof("http://webservices.seisint.com/WsAttributes")-1
};
}

__if_not_exists(__SaveAttributeRequest_entries)
{
extern __declspec(selectany) const _soapmapentry __SaveAttributeRequest_entries[] =
{
	{ 
		0x3474D1E7, 
		"ModuleName", 
		L"ModuleName", 
		sizeof("ModuleName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(SaveAttributeRequest, ModuleName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0CA09C15, 
		"AttributeName", 
		L"AttributeName", 
		sizeof("AttributeName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(SaveAttributeRequest, AttributeName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x052F968D, 
		"Flags", 
		L"Flags", 
		sizeof("Flags")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(SaveAttributeRequest, Flags),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x94BBD701, 
		"ResultType", 
		L"ResultType", 
		sizeof("ResultType")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(SaveAttributeRequest, ResultType),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xFAB05784, 
		"Description", 
		L"Description", 
		sizeof("Description")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(SaveAttributeRequest, Description),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x002FCD65, 
		"Text", 
		L"Text", 
		sizeof("Text")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(SaveAttributeRequest, Text),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __SaveAttributeRequest_map =
{
	0xBD65C82C,
	"SaveAttributeRequest",
	L"SaveAttributeRequest",
	sizeof("SaveAttributeRequest")-1,
	sizeof("SaveAttributeRequest")-1,
	SOAPMAP_STRUCT,
	__SaveAttributeRequest_entries,
	sizeof(SaveAttributeRequest),
	6,
	-1,
	SOAPFLAG_NONE,
	0xAA017D94,
	"http://webservices.seisint.com/WsAttributes",
	L"http://webservices.seisint.com/WsAttributes",
	sizeof("http://webservices.seisint.com/WsAttributes")-1
};
}

__if_not_exists(__CheckinAttributeRequest_entries)
{
extern __declspec(selectany) const _soapmapentry __CheckinAttributeRequest_entries[] =
{
	{ 
		0x3474D1E7, 
		"ModuleName", 
		L"ModuleName", 
		sizeof("ModuleName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(CheckinAttributeRequest, ModuleName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0CA09C15, 
		"AttributeName", 
		L"AttributeName", 
		sizeof("AttributeName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(CheckinAttributeRequest, AttributeName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xFAB05784, 
		"Description", 
		L"Description", 
		sizeof("Description")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(CheckinAttributeRequest, Description),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CheckinAttributeRequest_map =
{
	0x710F95F2,
	"CheckinAttributeRequest",
	L"CheckinAttributeRequest",
	sizeof("CheckinAttributeRequest")-1,
	sizeof("CheckinAttributeRequest")-1,
	SOAPMAP_STRUCT,
	__CheckinAttributeRequest_entries,
	sizeof(CheckinAttributeRequest),
	3,
	-1,
	SOAPFLAG_NONE,
	0xAA017D94,
	"http://webservices.seisint.com/WsAttributes",
	L"http://webservices.seisint.com/WsAttributes",
	sizeof("http://webservices.seisint.com/WsAttributes")-1
};
}

__if_not_exists(__CheckoutAttributeRequest_entries)
{
extern __declspec(selectany) const _soapmapentry __CheckoutAttributeRequest_entries[] =
{
	{ 
		0x3474D1E7, 
		"ModuleName", 
		L"ModuleName", 
		sizeof("ModuleName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(CheckoutAttributeRequest, ModuleName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0CA09C15, 
		"AttributeName", 
		L"AttributeName", 
		sizeof("AttributeName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(CheckoutAttributeRequest, AttributeName),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CheckoutAttributeRequest_map =
{
	0x4C92C173,
	"CheckoutAttributeRequest",
	L"CheckoutAttributeRequest",
	sizeof("CheckoutAttributeRequest")-1,
	sizeof("CheckoutAttributeRequest")-1,
	SOAPMAP_STRUCT,
	__CheckoutAttributeRequest_entries,
	sizeof(CheckoutAttributeRequest),
	2,
	-1,
	SOAPFLAG_NONE,
	0xAA017D94,
	"http://webservices.seisint.com/WsAttributes",
	L"http://webservices.seisint.com/WsAttributes",
	sizeof("http://webservices.seisint.com/WsAttributes")-1
};
}

__if_not_exists(__RenameAttributeRequest_entries)
{
extern __declspec(selectany) const _soapmapentry __RenameAttributeRequest_entries[] =
{
	{ 
		0x3474D1E7, 
		"ModuleName", 
		L"ModuleName", 
		sizeof("ModuleName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RenameAttributeRequest, ModuleName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0CA09C15, 
		"AttributeName", 
		L"AttributeName", 
		sizeof("AttributeName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RenameAttributeRequest, AttributeName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xB185CFB1, 
		"NewModuleName", 
		L"NewModuleName", 
		sizeof("NewModuleName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RenameAttributeRequest, NewModuleName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xBADB3D9F, 
		"NewAttributeName", 
		L"NewAttributeName", 
		sizeof("NewAttributeName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RenameAttributeRequest, NewAttributeName),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __RenameAttributeRequest_map =
{
	0x9C0F35B5,
	"RenameAttributeRequest",
	L"RenameAttributeRequest",
	sizeof("RenameAttributeRequest")-1,
	sizeof("RenameAttributeRequest")-1,
	SOAPMAP_STRUCT,
	__RenameAttributeRequest_entries,
	sizeof(RenameAttributeRequest),
	4,
	-1,
	SOAPFLAG_NONE,
	0xAA017D94,
	"http://webservices.seisint.com/WsAttributes",
	L"http://webservices.seisint.com/WsAttributes",
	sizeof("http://webservices.seisint.com/WsAttributes")-1
};
}

__if_not_exists(__RollbackAttributeRequest_entries)
{
extern __declspec(selectany) const _soapmapentry __RollbackAttributeRequest_entries[] =
{
	{ 
		0x3474D1E7, 
		"ModuleName", 
		L"ModuleName", 
		sizeof("ModuleName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RollbackAttributeRequest, ModuleName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0CA09C15, 
		"AttributeName", 
		L"AttributeName", 
		sizeof("AttributeName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(RollbackAttributeRequest, AttributeName),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __RollbackAttributeRequest_map =
{
	0x21595787,
	"RollbackAttributeRequest",
	L"RollbackAttributeRequest",
	sizeof("RollbackAttributeRequest")-1,
	sizeof("RollbackAttributeRequest")-1,
	SOAPMAP_STRUCT,
	__RollbackAttributeRequest_entries,
	sizeof(RollbackAttributeRequest),
	2,
	-1,
	SOAPFLAG_NONE,
	0xAA017D94,
	"http://webservices.seisint.com/WsAttributes",
	L"http://webservices.seisint.com/WsAttributes",
	sizeof("http://webservices.seisint.com/WsAttributes")-1
};
}

__if_not_exists(__ECLLabel_entries)
{
extern __declspec(selectany) const _soapmapentry __ECLLabel_entries[] =
{
	{ 
		0x002C70A1, 
		"Name", 
		L"Name", 
		sizeof("Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLLabel, Name),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x8F40041F, 
		"ModifiedDate", 
		L"ModifiedDate", 
		sizeof("ModifiedDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLLabel, ModifiedDate),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __ECLLabel_map =
{
	0x8A1A7C54,
	"ECLLabel",
	L"ECLLabel",
	sizeof("ECLLabel")-1,
	sizeof("ECLLabel")-1,
	SOAPMAP_STRUCT,
	__ECLLabel_entries,
	sizeof(ECLLabel),
	2,
	-1,
	SOAPFLAG_NONE,
	0xAA017D94,
	"http://webservices.seisint.com/WsAttributes",
	L"http://webservices.seisint.com/WsAttributes",
	sizeof("http://webservices.seisint.com/WsAttributes")-1
};
}

__if_not_exists(__ImportAttributeRequest_entries)
{
extern __declspec(selectany) const _soapmapentry __ImportAttributeRequest_entries[] =
{
	{ 
		0x3474D1E7, 
		"ModuleName", 
		L"ModuleName", 
		sizeof("ModuleName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ImportAttributeRequest, ModuleName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0CA09C15, 
		"AttributeName", 
		L"AttributeName", 
		sizeof("AttributeName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ImportAttributeRequest, AttributeName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x00302162, 
		"Type", 
		L"Type", 
		sizeof("Type")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ImportAttributeRequest, Type),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x052F968D, 
		"Flags", 
		L"Flags", 
		sizeof("Flags")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ImportAttributeRequest, Flags),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x94BBD701, 
		"ResultType", 
		L"ResultType", 
		sizeof("ResultType")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ImportAttributeRequest, ResultType),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xFAB05784, 
		"Description", 
		L"Description", 
		sizeof("Description")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ImportAttributeRequest, Description),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x002FCD65, 
		"Text", 
		L"Text", 
		sizeof("Text")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ImportAttributeRequest, Text),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __ImportAttributeRequest_map =
{
	0xA1DB4718,
	"ImportAttributeRequest",
	L"ImportAttributeRequest",
	sizeof("ImportAttributeRequest")-1,
	sizeof("ImportAttributeRequest")-1,
	SOAPMAP_STRUCT,
	__ImportAttributeRequest_entries,
	sizeof(ImportAttributeRequest),
	7,
	-1,
	SOAPFLAG_NONE,
	0xAA017D94,
	"http://webservices.seisint.com/WsAttributes",
	L"http://webservices.seisint.com/WsAttributes",
	sizeof("http://webservices.seisint.com/WsAttributes")-1
};
}

__if_not_exists(__DeleteAttributeRequest_entries)
{
extern __declspec(selectany) const _soapmapentry __DeleteAttributeRequest_entries[] =
{
	{ 
		0x3474D1E7, 
		"ModuleName", 
		L"ModuleName", 
		sizeof("ModuleName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DeleteAttributeRequest, ModuleName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0CA09C15, 
		"AttributeName", 
		L"AttributeName", 
		sizeof("AttributeName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DeleteAttributeRequest, AttributeName),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __DeleteAttributeRequest_map =
{
	0x04594C10,
	"DeleteAttributeRequest",
	L"DeleteAttributeRequest",
	sizeof("DeleteAttributeRequest")-1,
	sizeof("DeleteAttributeRequest")-1,
	SOAPMAP_STRUCT,
	__DeleteAttributeRequest_entries,
	sizeof(DeleteAttributeRequest),
	2,
	-1,
	SOAPFLAG_NONE,
	0xAA017D94,
	"http://webservices.seisint.com/WsAttributes",
	L"http://webservices.seisint.com/WsAttributes",
	sizeof("http://webservices.seisint.com/WsAttributes")-1
};
}

__if_not_exists(__ECLModule_entries)
{
extern __declspec(selectany) const _soapmapentry __ECLModule_entries[] =
{
	{ 
		0x002C70A1, 
		"Name", 
		L"Name", 
		sizeof("Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLModule, Name),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x052F968D, 
		"Flags", 
		L"Flags", 
		sizeof("Flags")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLModule, Flags),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x9ED6D132, 
		"Access", 
		L"Access", 
		sizeof("Access")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLModule, Access),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xDA5C9354, 
		"Timestamp", 
		L"Timestamp", 
		sizeof("Timestamp")-1, 
		SOAPTYPE_LONG, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLModule, Timestamp),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xB5AFF40B, 
		"IsPlugin", 
		L"IsPlugin", 
		sizeof("IsPlugin")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLModule, IsPlugin),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x002EA24C, 
		"Rank", 
		L"Rank", 
		sizeof("Rank")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLModule, Rank),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x2FE06021, 
		"FileName", 
		L"FileName", 
		sizeof("FileName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLModule, FileName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x09352758, 
		"FileCRC", 
		L"FileCRC", 
		sizeof("FileCRC")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLModule, FileCRC),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xCFF936C6, 
		"Version", 
		L"Version", 
		sizeof("Version")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLModule, Version),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0DB88BF3, 
		"Checksum", 
		L"Checksum", 
		sizeof("Checksum")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLModule, Checksum),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __ECLModule_map =
{
	0xD0BDE3BA,
	"ECLModule",
	L"ECLModule",
	sizeof("ECLModule")-1,
	sizeof("ECLModule")-1,
	SOAPMAP_STRUCT,
	__ECLModule_entries,
	sizeof(ECLModule),
	10,
	-1,
	SOAPFLAG_NONE,
	0xAA017D94,
	"http://webservices.seisint.com/WsAttributes",
	L"http://webservices.seisint.com/WsAttributes",
	sizeof("http://webservices.seisint.com/WsAttributes")-1
};
}

__if_not_exists(__ECLAttributeLabel_entries)
{
extern __declspec(selectany) const _soapmapentry __ECLAttributeLabel_entries[] =
{
	{ 
		0x3474D1E7, 
		"ModuleName", 
		L"ModuleName", 
		sizeof("ModuleName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLAttributeLabel, ModuleName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0CA09C15, 
		"AttributeName", 
		L"AttributeName", 
		sizeof("AttributeName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLAttributeLabel, AttributeName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0FC11DBA, 
		"AttributeVersion", 
		L"AttributeVersion", 
		sizeof("AttributeVersion")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLAttributeLabel, AttributeVersion),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x85164601, 
		"LabelName", 
		L"LabelName", 
		sizeof("LabelName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLAttributeLabel, LabelName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x8F40041F, 
		"ModifiedDate", 
		L"ModifiedDate", 
		sizeof("ModifiedDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLAttributeLabel, ModifiedDate),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __ECLAttributeLabel_map =
{
	0xE06728C8,
	"ECLAttributeLabel",
	L"ECLAttributeLabel",
	sizeof("ECLAttributeLabel")-1,
	sizeof("ECLAttributeLabel")-1,
	SOAPMAP_STRUCT,
	__ECLAttributeLabel_entries,
	sizeof(ECLAttributeLabel),
	5,
	-1,
	SOAPFLAG_NONE,
	0xAA017D94,
	"http://webservices.seisint.com/WsAttributes",
	L"http://webservices.seisint.com/WsAttributes",
	sizeof("http://webservices.seisint.com/WsAttributes")-1
};
}

__if_not_exists(__AttributeType_entries)
{
extern __declspec(selectany) const _soapmapentry __AttributeType_entries[] =
{
	{ 
		0x000009AD, 
		"ID", 
		L"ID", 
		sizeof("ID")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(AttributeType, ID),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x002C70A1, 
		"Name", 
		L"Name", 
		sizeof("Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(AttributeType, Name),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xFAB05784, 
		"Description", 
		L"Description", 
		sizeof("Description")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(AttributeType, Description),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __AttributeType_map =
{
	0x0CA44CD6,
	"AttributeType",
	L"AttributeType",
	sizeof("AttributeType")-1,
	sizeof("AttributeType")-1,
	SOAPMAP_STRUCT,
	__AttributeType_entries,
	sizeof(AttributeType),
	3,
	-1,
	SOAPFLAG_NONE,
	0xAA017D94,
	"http://webservices.seisint.com/WsAttributes",
	L"http://webservices.seisint.com/WsAttributes",
	sizeof("http://webservices.seisint.com/WsAttributes")-1
};
}

__if_not_exists(__ECLTextLine_entries)
{
extern __declspec(selectany) const _soapmapentry __ECLTextLine_entries[] =
{
	{ 
		0x9BA5E7B1, 
		"LineNumber", 
		L"LineNumber", 
		sizeof("LineNumber")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLTextLine, LineNumber),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xBE190B6D, 
		"LineText", 
		L"LineText", 
		sizeof("LineText")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLTextLine, LineText),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __ECLTextLine_map =
{
	0x9A4FD461,
	"ECLTextLine",
	L"ECLTextLine",
	sizeof("ECLTextLine")-1,
	sizeof("ECLTextLine")-1,
	SOAPMAP_STRUCT,
	__ECLTextLine_entries,
	sizeof(ECLTextLine),
	2,
	-1,
	SOAPFLAG_NONE,
	0xAA017D94,
	"http://webservices.seisint.com/WsAttributes",
	L"http://webservices.seisint.com/WsAttributes",
	sizeof("http://webservices.seisint.com/WsAttributes")-1
};
}

__if_not_exists(__ECLAttribute_entries)
{
extern __declspec(selectany) const _soapmapentry __ECLAttribute_entries[] =
{
	{ 
		0x3474D1E7, 
		"ModuleName", 
		L"ModuleName", 
		sizeof("ModuleName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLAttribute, ModuleName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x002C70A1, 
		"Name", 
		L"Name", 
		sizeof("Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLAttribute, Name),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x00302162, 
		"Type", 
		L"Type", 
		sizeof("Type")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLAttribute, Type),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xCFF936C6, 
		"Version", 
		L"Version", 
		sizeof("Version")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLAttribute, Version),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x544F07B3, 
		"LatestVersion", 
		L"LatestVersion", 
		sizeof("LatestVersion")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLAttribute, LatestVersion),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0673C475, 
		"SandboxVersion", 
		L"SandboxVersion", 
		sizeof("SandboxVersion")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLAttribute, SandboxVersion),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x052F968D, 
		"Flags", 
		L"Flags", 
		sizeof("Flags")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLAttribute, Flags),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x9ED6D132, 
		"Access", 
		L"Access", 
		sizeof("Access")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLAttribute, Access),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xAC87CCAE, 
		"IsLocked", 
		L"IsLocked", 
		sizeof("IsLocked")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLAttribute, IsLocked),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x3B0F47FB, 
		"IsCheckedOut", 
		L"IsCheckedOut", 
		sizeof("IsCheckedOut")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLAttribute, IsCheckedOut),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x3873FEEB, 
		"IsSandbox", 
		L"IsSandbox", 
		sizeof("IsSandbox")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLAttribute, IsSandbox),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xB77CB22D, 
		"IsOrphaned", 
		L"IsOrphaned", 
		sizeof("IsOrphaned")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLAttribute, IsOrphaned),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x94BBD701, 
		"ResultType", 
		L"ResultType", 
		sizeof("ResultType")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLAttribute, ResultType),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x72C3934D, 
		"LockedBy", 
		L"LockedBy", 
		sizeof("LockedBy")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLAttribute, LockedBy),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x8B100B1C, 
		"ModifiedBy", 
		L"ModifiedBy", 
		sizeof("ModifiedBy")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLAttribute, ModifiedBy),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x8F40041F, 
		"ModifiedDate", 
		L"ModifiedDate", 
		sizeof("ModifiedDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLAttribute, ModifiedDate),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xFAB05784, 
		"Description", 
		L"Description", 
		sizeof("Description")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLAttribute, Description),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0DB88BF3, 
		"Checksum", 
		L"Checksum", 
		sizeof("Checksum")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLAttribute, Checksum),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x002FCD65, 
		"Text", 
		L"Text", 
		sizeof("Text")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(ECLAttribute, Text),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x6A944811, 
		"MatchedLines", 
		L"MatchedLines", 
		sizeof("MatchedLines")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(ECLAttribute, MatchedLines),
		NULL, 
		&__ECLTextLine_map, 
		19+1 
	},
	{ 
		0x6A944811, 
		"MatchedLines", 
		L"MatchedLines", 
		sizeof("MatchedLines")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(ECLAttribute, __MatchedLines_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __ECLAttribute_map =
{
	0xE05F4268,
	"ECLAttribute",
	L"ECLAttribute",
	sizeof("ECLAttribute")-1,
	sizeof("ECLAttribute")-1,
	SOAPMAP_STRUCT,
	__ECLAttribute_entries,
	sizeof(ECLAttribute),
	20,
	-1,
	SOAPFLAG_NONE,
	0xAA017D94,
	"http://webservices.seisint.com/WsAttributes",
	L"http://webservices.seisint.com/WsAttributes",
	sizeof("http://webservices.seisint.com/WsAttributes")-1
};
}

struct __CWsAttributes_GetAttribute_struct
{
	BSTR ModuleName;
	BSTR AttributeName;
	BSTR Type;
	int Version;
	bool GetSandbox;
	bool GetText;
	BSTR Label;
	ArrayOfEspException Exceptions;
	ECLAttribute outAttribute;
};

extern __declspec(selectany) const _soapmapentry __CWsAttributes_GetAttribute_entries[] =
{

	{
		0x3474D1E7, 
		"ModuleName", 
		L"ModuleName", 
		sizeof("ModuleName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_GetAttribute_struct, ModuleName),
		NULL,
		NULL,
		-1,
	},
	{
		0x0CA09C15, 
		"AttributeName", 
		L"AttributeName", 
		sizeof("AttributeName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_GetAttribute_struct, AttributeName),
		NULL,
		NULL,
		-1,
	},
	{
		0x00302162, 
		"Type", 
		L"Type", 
		sizeof("Type")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_GetAttribute_struct, Type),
		NULL,
		NULL,
		-1,
	},
	{
		0xCFF936C6, 
		"Version", 
		L"Version", 
		sizeof("Version")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_GetAttribute_struct, Version),
		NULL,
		NULL,
		-1,
	},
	{
		0x0140700F, 
		"GetSandbox", 
		L"GetSandbox", 
		sizeof("GetSandbox")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_GetAttribute_struct, GetSandbox),
		NULL,
		NULL,
		-1,
	},
	{
		0x4D62C8E5, 
		"GetText", 
		L"GetText", 
		sizeof("GetText")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_GetAttribute_struct, GetText),
		NULL,
		NULL,
		-1,
	},
	{
		0x05962560, 
		"Label", 
		L"Label", 
		sizeof("Label")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_GetAttribute_struct, Label),
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
		offsetof(__CWsAttributes_GetAttribute_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0x960EA82C, 
		"outAttribute", 
		L"outAttribute", 
		sizeof("outAttribute")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_GetAttribute_struct, outAttribute),
		NULL,
		&__ECLAttribute_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsAttributes_GetAttribute_map =
{
	0xA00F5323,
	"GetAttribute",
	L"GetAttributeResponse",
	sizeof("GetAttribute")-1,
	sizeof("GetAttributeResponse")-1,
	SOAPMAP_FUNC,
	__CWsAttributes_GetAttribute_entries,
	sizeof(__CWsAttributes_GetAttribute_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xAA017D94,
	"http://webservices.seisint.com/WsAttributes",
	L"http://webservices.seisint.com/WsAttributes",
	sizeof("http://webservices.seisint.com/WsAttributes")-1
};


struct __CWsAttributes_CreateModule_struct
{
	BSTR ModuleName;
	ArrayOfEspException Exceptions;
	ECLModule ModuleInfo;
};

extern __declspec(selectany) const _soapmapentry __CWsAttributes_CreateModule_entries[] =
{

	{
		0x3474D1E7, 
		"ModuleName", 
		L"ModuleName", 
		sizeof("ModuleName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_CreateModule_struct, ModuleName),
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
		offsetof(__CWsAttributes_CreateModule_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0x34724A72, 
		"ModuleInfo", 
		L"ModuleInfo", 
		sizeof("ModuleInfo")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_CreateModule_struct, ModuleInfo),
		NULL,
		&__ECLModule_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsAttributes_CreateModule_map =
{
	0xCECEB349,
	"CreateModule",
	L"CreateModuleResponse",
	sizeof("CreateModule")-1,
	sizeof("CreateModuleResponse")-1,
	SOAPMAP_FUNC,
	__CWsAttributes_CreateModule_entries,
	sizeof(__CWsAttributes_CreateModule_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xAA017D94,
	"http://webservices.seisint.com/WsAttributes",
	L"http://webservices.seisint.com/WsAttributes",
	sizeof("http://webservices.seisint.com/WsAttributes")-1
};


struct __CWsAttributes_CreateAttribute_struct
{
	BSTR ModuleName;
	BSTR AttributeName;
	BSTR Type;
	ArrayOfEspException Exceptions;
	ECLAttribute AttributeInfo;
};

extern __declspec(selectany) const _soapmapentry __CWsAttributes_CreateAttribute_entries[] =
{

	{
		0x3474D1E7, 
		"ModuleName", 
		L"ModuleName", 
		sizeof("ModuleName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_CreateAttribute_struct, ModuleName),
		NULL,
		NULL,
		-1,
	},
	{
		0x0CA09C15, 
		"AttributeName", 
		L"AttributeName", 
		sizeof("AttributeName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_CreateAttribute_struct, AttributeName),
		NULL,
		NULL,
		-1,
	},
	{
		0x00302162, 
		"Type", 
		L"Type", 
		sizeof("Type")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_CreateAttribute_struct, Type),
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
		offsetof(__CWsAttributes_CreateAttribute_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0x0C9E14A0, 
		"AttributeInfo", 
		L"AttributeInfo", 
		sizeof("AttributeInfo")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_CreateAttribute_struct, AttributeInfo),
		NULL,
		&__ECLAttribute_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsAttributes_CreateAttribute_map =
{
	0x366AA3F7,
	"CreateAttribute",
	L"CreateAttributeResponse",
	sizeof("CreateAttribute")-1,
	sizeof("CreateAttributeResponse")-1,
	SOAPMAP_FUNC,
	__CWsAttributes_CreateAttribute_entries,
	sizeof(__CWsAttributes_CreateAttribute_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xAA017D94,
	"http://webservices.seisint.com/WsAttributes",
	L"http://webservices.seisint.com/WsAttributes",
	sizeof("http://webservices.seisint.com/WsAttributes")-1
};


struct __CWsAttributes_CheckinAttributes_struct
{
	CheckinAttributeRequest *Attributes;
	int __Attributes_nSizeIs;
	ArrayOfEspException Exceptions;
	ECLAttribute *outAttributes;
	int __outAttributes_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CWsAttributes_CheckinAttributes_entries[] =
{

	{
		0x2072C067, 
		"Attributes", 
		L"Attributes", 
		sizeof("Attributes")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_CheckinAttributes_struct, Attributes),
		NULL,
		&__CheckinAttributeRequest_map,
		0+1,
	},
	{
		0x2072C067,
		"__Attributes_nSizeIs",
		L"__Attributes_nSizeIs",
		sizeof("__Attributes_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsAttributes_CheckinAttributes_struct, __Attributes_nSizeIs),
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
		offsetof(__CWsAttributes_CheckinAttributes_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0x57E3AE1F, 
		"outAttributes", 
		L"outAttributes", 
		sizeof("outAttributes")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_CheckinAttributes_struct, outAttributes),
		NULL,
		&__ECLAttribute_map,
		3+1,
	},
	{
		0x57E3AE1F,
		"__outAttributes_nSizeIs",
		L"__outAttributes_nSizeIs",
		sizeof("__outAttributes_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsAttributes_CheckinAttributes_struct, __outAttributes_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsAttributes_CheckinAttributes_map =
{
	0xD99C7419,
	"CheckinAttributes",
	L"UpdateAttributesResponse",
	sizeof("CheckinAttributes")-1,
	sizeof("UpdateAttributesResponse")-1,
	SOAPMAP_FUNC,
	__CWsAttributes_CheckinAttributes_entries,
	sizeof(__CWsAttributes_CheckinAttributes_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xAA017D94,
	"http://webservices.seisint.com/WsAttributes",
	L"http://webservices.seisint.com/WsAttributes",
	sizeof("http://webservices.seisint.com/WsAttributes")-1
};


struct __CWsAttributes_RenameAttributes_struct
{
	RenameAttributeRequest *Attributes;
	int __Attributes_nSizeIs;
	ArrayOfEspException Exceptions;
	ECLAttribute *outAttributes;
	int __outAttributes_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CWsAttributes_RenameAttributes_entries[] =
{

	{
		0x2072C067, 
		"Attributes", 
		L"Attributes", 
		sizeof("Attributes")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_RenameAttributes_struct, Attributes),
		NULL,
		&__RenameAttributeRequest_map,
		0+1,
	},
	{
		0x2072C067,
		"__Attributes_nSizeIs",
		L"__Attributes_nSizeIs",
		sizeof("__Attributes_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsAttributes_RenameAttributes_struct, __Attributes_nSizeIs),
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
		offsetof(__CWsAttributes_RenameAttributes_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0x57E3AE1F, 
		"outAttributes", 
		L"outAttributes", 
		sizeof("outAttributes")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_RenameAttributes_struct, outAttributes),
		NULL,
		&__ECLAttribute_map,
		3+1,
	},
	{
		0x57E3AE1F,
		"__outAttributes_nSizeIs",
		L"__outAttributes_nSizeIs",
		sizeof("__outAttributes_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsAttributes_RenameAttributes_struct, __outAttributes_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsAttributes_RenameAttributes_map =
{
	0xD99C7419,
	"RenameAttributes",
	L"UpdateAttributesResponse",
	sizeof("RenameAttributes")-1,
	sizeof("UpdateAttributesResponse")-1,
	SOAPMAP_FUNC,
	__CWsAttributes_RenameAttributes_entries,
	sizeof(__CWsAttributes_RenameAttributes_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xAA017D94,
	"http://webservices.seisint.com/WsAttributes",
	L"http://webservices.seisint.com/WsAttributes",
	sizeof("http://webservices.seisint.com/WsAttributes")-1
};


struct __CWsAttributes_DeleteAttributes_struct
{
	DeleteAttributeRequest *Attributes;
	int __Attributes_nSizeIs;
	ArrayOfEspException Exceptions;
	ECLAttribute *outAttributes;
	int __outAttributes_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CWsAttributes_DeleteAttributes_entries[] =
{

	{
		0x2072C067, 
		"Attributes", 
		L"Attributes", 
		sizeof("Attributes")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_DeleteAttributes_struct, Attributes),
		NULL,
		&__DeleteAttributeRequest_map,
		0+1,
	},
	{
		0x2072C067,
		"__Attributes_nSizeIs",
		L"__Attributes_nSizeIs",
		sizeof("__Attributes_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsAttributes_DeleteAttributes_struct, __Attributes_nSizeIs),
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
		offsetof(__CWsAttributes_DeleteAttributes_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0x57E3AE1F, 
		"outAttributes", 
		L"outAttributes", 
		sizeof("outAttributes")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_DeleteAttributes_struct, outAttributes),
		NULL,
		&__ECLAttribute_map,
		3+1,
	},
	{
		0x57E3AE1F,
		"__outAttributes_nSizeIs",
		L"__outAttributes_nSizeIs",
		sizeof("__outAttributes_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsAttributes_DeleteAttributes_struct, __outAttributes_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsAttributes_DeleteAttributes_map =
{
	0xD99C7419,
	"DeleteAttributes",
	L"UpdateAttributesResponse",
	sizeof("DeleteAttributes")-1,
	sizeof("UpdateAttributesResponse")-1,
	SOAPMAP_FUNC,
	__CWsAttributes_DeleteAttributes_entries,
	sizeof(__CWsAttributes_DeleteAttributes_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xAA017D94,
	"http://webservices.seisint.com/WsAttributes",
	L"http://webservices.seisint.com/WsAttributes",
	sizeof("http://webservices.seisint.com/WsAttributes")-1
};


struct __CWsAttributes_GetLabels_struct
{
	BSTR Name;
	ArrayOfEspException Exceptions;
	ECLLabel *outLabels;
	int __outLabels_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CWsAttributes_GetLabels_entries[] =
{

	{
		0x002C70A1, 
		"Name", 
		L"Name", 
		sizeof("Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_GetLabels_struct, Name),
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
		offsetof(__CWsAttributes_GetLabels_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0x15C9A38B, 
		"outLabels", 
		L"outLabels", 
		sizeof("outLabels")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_GetLabels_struct, outLabels),
		NULL,
		&__ECLLabel_map,
		2+1,
	},
	{
		0x15C9A38B,
		"__outLabels_nSizeIs",
		L"__outLabels_nSizeIs",
		sizeof("__outLabels_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsAttributes_GetLabels_struct, __outLabels_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsAttributes_GetLabels_map =
{
	0xB52B3282,
	"GetLabels",
	L"GetLabelsResponse",
	sizeof("GetLabels")-1,
	sizeof("GetLabelsResponse")-1,
	SOAPMAP_FUNC,
	__CWsAttributes_GetLabels_entries,
	sizeof(__CWsAttributes_GetLabels_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xAA017D94,
	"http://webservices.seisint.com/WsAttributes",
	L"http://webservices.seisint.com/WsAttributes",
	sizeof("http://webservices.seisint.com/WsAttributes")-1
};


struct __CWsAttributes_GetAttributeLabels_struct
{
	BSTR ModuleName;
	BSTR AttributeName;
	ArrayOfEspException Exceptions;
	ECLAttributeLabel *Labels;
	int __Labels_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CWsAttributes_GetAttributeLabels_entries[] =
{

	{
		0x3474D1E7, 
		"ModuleName", 
		L"ModuleName", 
		sizeof("ModuleName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_GetAttributeLabels_struct, ModuleName),
		NULL,
		NULL,
		-1,
	},
	{
		0x0CA09C15, 
		"AttributeName", 
		L"AttributeName", 
		sizeof("AttributeName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_GetAttributeLabels_struct, AttributeName),
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
		offsetof(__CWsAttributes_GetAttributeLabels_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0xB85AD1D3, 
		"Labels", 
		L"Labels", 
		sizeof("Labels")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_GetAttributeLabels_struct, Labels),
		NULL,
		&__ECLAttributeLabel_map,
		3+1,
	},
	{
		0xB85AD1D3,
		"__Labels_nSizeIs",
		L"__Labels_nSizeIs",
		sizeof("__Labels_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsAttributes_GetAttributeLabels_struct, __Labels_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsAttributes_GetAttributeLabels_map =
{
	0x15522EF6,
	"GetAttributeLabels",
	L"GetAttributeLabelsResponse",
	sizeof("GetAttributeLabels")-1,
	sizeof("GetAttributeLabelsResponse")-1,
	SOAPMAP_FUNC,
	__CWsAttributes_GetAttributeLabels_entries,
	sizeof(__CWsAttributes_GetAttributeLabels_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xAA017D94,
	"http://webservices.seisint.com/WsAttributes",
	L"http://webservices.seisint.com/WsAttributes",
	sizeof("http://webservices.seisint.com/WsAttributes")-1
};


struct __CWsAttributes_GetAttributeHistory_struct
{
	BSTR ModuleName;
	BSTR AttributeName;
	BSTR FromDate;
	BSTR ToDate;
	ArrayOfEspException Exceptions;
	ECLAttribute *outAttributes;
	int __outAttributes_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CWsAttributes_GetAttributeHistory_entries[] =
{

	{
		0x3474D1E7, 
		"ModuleName", 
		L"ModuleName", 
		sizeof("ModuleName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_GetAttributeHistory_struct, ModuleName),
		NULL,
		NULL,
		-1,
	},
	{
		0x0CA09C15, 
		"AttributeName", 
		L"AttributeName", 
		sizeof("AttributeName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_GetAttributeHistory_struct, AttributeName),
		NULL,
		NULL,
		-1,
	},
	{
		0xEC3745F2, 
		"FromDate", 
		L"FromDate", 
		sizeof("FromDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_GetAttributeHistory_struct, FromDate),
		NULL,
		NULL,
		-1,
	},
	{
		0xCBF0EA81, 
		"ToDate", 
		L"ToDate", 
		sizeof("ToDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_GetAttributeHistory_struct, ToDate),
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
		offsetof(__CWsAttributes_GetAttributeHistory_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0x57E3AE1F, 
		"outAttributes", 
		L"outAttributes", 
		sizeof("outAttributes")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_GetAttributeHistory_struct, outAttributes),
		NULL,
		&__ECLAttribute_map,
		5+1,
	},
	{
		0x57E3AE1F,
		"__outAttributes_nSizeIs",
		L"__outAttributes_nSizeIs",
		sizeof("__outAttributes_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsAttributes_GetAttributeHistory_struct, __outAttributes_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsAttributes_GetAttributeHistory_map =
{
	0xC38EF035,
	"GetAttributeHistory",
	L"GetAttributeHistoryResponse",
	sizeof("GetAttributeHistory")-1,
	sizeof("GetAttributeHistoryResponse")-1,
	SOAPMAP_FUNC,
	__CWsAttributes_GetAttributeHistory_entries,
	sizeof(__CWsAttributes_GetAttributeHistory_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xAA017D94,
	"http://webservices.seisint.com/WsAttributes",
	L"http://webservices.seisint.com/WsAttributes",
	sizeof("http://webservices.seisint.com/WsAttributes")-1
};


struct __CWsAttributes_UpdateLabel_struct
{
	BSTR Name;
	BSTR ModuleName;
	BSTR AtributeName;
	int AttrbuteVersion;
	ArrayOfEspException Exceptions;
};

extern __declspec(selectany) const _soapmapentry __CWsAttributes_UpdateLabel_entries[] =
{

	{
		0x002C70A1, 
		"Name", 
		L"Name", 
		sizeof("Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_UpdateLabel_struct, Name),
		NULL,
		NULL,
		-1,
	},
	{
		0x3474D1E7, 
		"ModuleName", 
		L"ModuleName", 
		sizeof("ModuleName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_UpdateLabel_struct, ModuleName),
		NULL,
		NULL,
		-1,
	},
	{
		0x25039801, 
		"AtributeName", 
		L"AtributeName", 
		sizeof("AtributeName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_UpdateLabel_struct, AtributeName),
		NULL,
		NULL,
		-1,
	},
	{
		0xC653FD91, 
		"AttrbuteVersion", 
		L"AttrbuteVersion", 
		sizeof("AttrbuteVersion")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_UpdateLabel_struct, AttrbuteVersion),
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
		offsetof(__CWsAttributes_UpdateLabel_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsAttributes_UpdateLabel_map =
{
	0x42F76432,
	"UpdateLabel",
	L"UpdateLabelResponse",
	sizeof("UpdateLabel")-1,
	sizeof("UpdateLabelResponse")-1,
	SOAPMAP_FUNC,
	__CWsAttributes_UpdateLabel_entries,
	sizeof(__CWsAttributes_UpdateLabel_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xAA017D94,
	"http://webservices.seisint.com/WsAttributes",
	L"http://webservices.seisint.com/WsAttributes",
	sizeof("http://webservices.seisint.com/WsAttributes")-1
};


struct __CWsAttributes_ListAttributeTypes_struct
{
	AttributeType *Types;
	int __Types_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CWsAttributes_ListAttributeTypes_entries[] =
{

	{
		0x06344E15, 
		"Types", 
		L"Types", 
		sizeof("Types")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_ListAttributeTypes_struct, Types),
		NULL,
		&__AttributeType_map,
		0+1,
	},
	{
		0x06344E15,
		"__Types_nSizeIs",
		L"__Types_nSizeIs",
		sizeof("__Types_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsAttributes_ListAttributeTypes_struct, __Types_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsAttributes_ListAttributeTypes_map =
{
	0xBCA452F4,
	"ListAttributeTypes",
	L"ListAttributeTypesResponse",
	sizeof("ListAttributeTypes")-1,
	sizeof("ListAttributeTypesResponse")-1,
	SOAPMAP_FUNC,
	__CWsAttributes_ListAttributeTypes_entries,
	sizeof(__CWsAttributes_ListAttributeTypes_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xAA017D94,
	"http://webservices.seisint.com/WsAttributes",
	L"http://webservices.seisint.com/WsAttributes",
	sizeof("http://webservices.seisint.com/WsAttributes")-1
};


struct __CWsAttributes_ImportAttributes_struct
{
	ImportAttributeRequest *Attributes;
	int __Attributes_nSizeIs;
	ArrayOfEspException Exceptions;
	ECLAttribute *outAttributes;
	int __outAttributes_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CWsAttributes_ImportAttributes_entries[] =
{

	{
		0x2072C067, 
		"Attributes", 
		L"Attributes", 
		sizeof("Attributes")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_ImportAttributes_struct, Attributes),
		NULL,
		&__ImportAttributeRequest_map,
		0+1,
	},
	{
		0x2072C067,
		"__Attributes_nSizeIs",
		L"__Attributes_nSizeIs",
		sizeof("__Attributes_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsAttributes_ImportAttributes_struct, __Attributes_nSizeIs),
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
		offsetof(__CWsAttributes_ImportAttributes_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0x57E3AE1F, 
		"outAttributes", 
		L"outAttributes", 
		sizeof("outAttributes")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_ImportAttributes_struct, outAttributes),
		NULL,
		&__ECLAttribute_map,
		3+1,
	},
	{
		0x57E3AE1F,
		"__outAttributes_nSizeIs",
		L"__outAttributes_nSizeIs",
		sizeof("__outAttributes_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsAttributes_ImportAttributes_struct, __outAttributes_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsAttributes_ImportAttributes_map =
{
	0xD99C7419,
	"ImportAttributes",
	L"UpdateAttributesResponse",
	sizeof("ImportAttributes")-1,
	sizeof("UpdateAttributesResponse")-1,
	SOAPMAP_FUNC,
	__CWsAttributes_ImportAttributes_entries,
	sizeof(__CWsAttributes_ImportAttributes_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xAA017D94,
	"http://webservices.seisint.com/WsAttributes",
	L"http://webservices.seisint.com/WsAttributes",
	sizeof("http://webservices.seisint.com/WsAttributes")-1
};


struct __CWsAttributes_DeleteLabel_struct
{
	BSTR Name;
	ArrayOfEspException Exceptions;
};

extern __declspec(selectany) const _soapmapentry __CWsAttributes_DeleteLabel_entries[] =
{

	{
		0x002C70A1, 
		"Name", 
		L"Name", 
		sizeof("Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_DeleteLabel_struct, Name),
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
		offsetof(__CWsAttributes_DeleteLabel_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsAttributes_DeleteLabel_map =
{
	0x1126CA02,
	"DeleteLabel",
	L"DeleteLabelResponse",
	sizeof("DeleteLabel")-1,
	sizeof("DeleteLabelResponse")-1,
	SOAPMAP_FUNC,
	__CWsAttributes_DeleteLabel_entries,
	sizeof(__CWsAttributes_DeleteLabel_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xAA017D94,
	"http://webservices.seisint.com/WsAttributes",
	L"http://webservices.seisint.com/WsAttributes",
	sizeof("http://webservices.seisint.com/WsAttributes")-1
};


struct __CWsAttributes_GetModules_struct
{
	__int64 ModifiedSince;
	bool IncludeDeleted;
	bool GetChecksum;
	BSTR Label;
	BSTR EarMark;
	ArrayOfEspException Exceptions;
	ECLModule *outModules;
	int __outModules_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CWsAttributes_GetModules_entries[] =
{

	{
		0x78544133, 
		"ModifiedSince", 
		L"ModifiedSince", 
		sizeof("ModifiedSince")-1, 
		SOAPTYPE_LONG, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_GetModules_struct, ModifiedSince),
		NULL,
		NULL,
		-1,
	},
	{
		0x26B66BDB, 
		"IncludeDeleted", 
		L"IncludeDeleted", 
		sizeof("IncludeDeleted")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_GetModules_struct, IncludeDeleted),
		NULL,
		NULL,
		-1,
	},
	{
		0x6AFD4773, 
		"GetChecksum", 
		L"GetChecksum", 
		sizeof("GetChecksum")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_GetModules_struct, GetChecksum),
		NULL,
		NULL,
		-1,
	},
	{
		0x05962560, 
		"Label", 
		L"Label", 
		sizeof("Label")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_GetModules_struct, Label),
		NULL,
		NULL,
		-1,
	},
	{
		0xA9F19363, 
		"EarMark", 
		L"EarMark", 
		sizeof("EarMark")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_GetModules_struct, EarMark),
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
		offsetof(__CWsAttributes_GetModules_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0x3CCD8031, 
		"outModules", 
		L"outModules", 
		sizeof("outModules")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_GetModules_struct, outModules),
		NULL,
		&__ECLModule_map,
		6+1,
	},
	{
		0x3CCD8031,
		"__outModules_nSizeIs",
		L"__outModules_nSizeIs",
		sizeof("__outModules_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsAttributes_GetModules_struct, __outModules_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsAttributes_GetModules_map =
{
	0xFD468E28,
	"GetModules",
	L"GetModulesResponse",
	sizeof("GetModules")-1,
	sizeof("GetModulesResponse")-1,
	SOAPMAP_FUNC,
	__CWsAttributes_GetModules_entries,
	sizeof(__CWsAttributes_GetModules_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xAA017D94,
	"http://webservices.seisint.com/WsAttributes",
	L"http://webservices.seisint.com/WsAttributes",
	sizeof("http://webservices.seisint.com/WsAttributes")-1
};


struct __CWsAttributes_CreateLabel_struct
{
	BSTR Name;
	ArrayOfEspException Exceptions;
};

extern __declspec(selectany) const _soapmapentry __CWsAttributes_CreateLabel_entries[] =
{

	{
		0x002C70A1, 
		"Name", 
		L"Name", 
		sizeof("Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_CreateLabel_struct, Name),
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
		offsetof(__CWsAttributes_CreateLabel_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsAttributes_CreateLabel_map =
{
	0x42A259E3,
	"CreateLabel",
	L"CreateLabelResponse",
	sizeof("CreateLabel")-1,
	sizeof("CreateLabelResponse")-1,
	SOAPMAP_FUNC,
	__CWsAttributes_CreateLabel_entries,
	sizeof(__CWsAttributes_CreateLabel_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xAA017D94,
	"http://webservices.seisint.com/WsAttributes",
	L"http://webservices.seisint.com/WsAttributes",
	sizeof("http://webservices.seisint.com/WsAttributes")-1
};


struct __CWsAttributes_CheckoutAttributes_struct
{
	CheckoutAttributeRequest *Attributes;
	int __Attributes_nSizeIs;
	ArrayOfEspException Exceptions;
	ECLAttribute *outAttributes;
	int __outAttributes_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CWsAttributes_CheckoutAttributes_entries[] =
{

	{
		0x2072C067, 
		"Attributes", 
		L"Attributes", 
		sizeof("Attributes")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_CheckoutAttributes_struct, Attributes),
		NULL,
		&__CheckoutAttributeRequest_map,
		0+1,
	},
	{
		0x2072C067,
		"__Attributes_nSizeIs",
		L"__Attributes_nSizeIs",
		sizeof("__Attributes_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsAttributes_CheckoutAttributes_struct, __Attributes_nSizeIs),
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
		offsetof(__CWsAttributes_CheckoutAttributes_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0x57E3AE1F, 
		"outAttributes", 
		L"outAttributes", 
		sizeof("outAttributes")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_CheckoutAttributes_struct, outAttributes),
		NULL,
		&__ECLAttribute_map,
		3+1,
	},
	{
		0x57E3AE1F,
		"__outAttributes_nSizeIs",
		L"__outAttributes_nSizeIs",
		sizeof("__outAttributes_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsAttributes_CheckoutAttributes_struct, __outAttributes_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsAttributes_CheckoutAttributes_map =
{
	0xD99C7419,
	"CheckoutAttributes",
	L"UpdateAttributesResponse",
	sizeof("CheckoutAttributes")-1,
	sizeof("UpdateAttributesResponse")-1,
	SOAPMAP_FUNC,
	__CWsAttributes_CheckoutAttributes_entries,
	sizeof(__CWsAttributes_CheckoutAttributes_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xAA017D94,
	"http://webservices.seisint.com/WsAttributes",
	L"http://webservices.seisint.com/WsAttributes",
	sizeof("http://webservices.seisint.com/WsAttributes")-1
};


struct __CWsAttributes_RenameModule_struct
{
	BSTR ModuleName;
	BSTR NewModuleName;
	ArrayOfEspException Exceptions;
	ECLModule ModuleInfo;
};

extern __declspec(selectany) const _soapmapentry __CWsAttributes_RenameModule_entries[] =
{

	{
		0x3474D1E7, 
		"ModuleName", 
		L"ModuleName", 
		sizeof("ModuleName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_RenameModule_struct, ModuleName),
		NULL,
		NULL,
		-1,
	},
	{
		0xB185CFB1, 
		"NewModuleName", 
		L"NewModuleName", 
		sizeof("NewModuleName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_RenameModule_struct, NewModuleName),
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
		offsetof(__CWsAttributes_RenameModule_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0x34724A72, 
		"ModuleInfo", 
		L"ModuleInfo", 
		sizeof("ModuleInfo")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_RenameModule_struct, ModuleInfo),
		NULL,
		&__ECLModule_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsAttributes_RenameModule_map =
{
	0xD35663AD,
	"RenameModule",
	L"RenameModuleResponse",
	sizeof("RenameModule")-1,
	sizeof("RenameModuleResponse")-1,
	SOAPMAP_FUNC,
	__CWsAttributes_RenameModule_entries,
	sizeof(__CWsAttributes_RenameModule_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xAA017D94,
	"http://webservices.seisint.com/WsAttributes",
	L"http://webservices.seisint.com/WsAttributes",
	sizeof("http://webservices.seisint.com/WsAttributes")-1
};


struct __CWsAttributes_GetAttributes_struct
{
	BSTR ModuleName;
	BSTR *TypeList;
	int __TypeList_nSizeIs;
	BSTR Label;
	ArrayOfEspException Exceptions;
	ECLAttribute *outAttributes;
	int __outAttributes_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CWsAttributes_GetAttributes_entries[] =
{

	{
		0x3474D1E7, 
		"ModuleName", 
		L"ModuleName", 
		sizeof("ModuleName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_GetAttributes_struct, ModuleName),
		NULL,
		NULL,
		-1,
	},
	{
		0xF4717D1E, 
		"TypeList", 
		L"TypeList", 
		sizeof("TypeList")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_GetAttributes_struct, TypeList),
		NULL,
		NULL,
		1+1,
	},
	{
		0xF4717D1E,
		"__TypeList_nSizeIs",
		L"__TypeList_nSizeIs",
		sizeof("__TypeList_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsAttributes_GetAttributes_struct, __TypeList_nSizeIs),
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
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_GetAttributes_struct, Label),
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
		offsetof(__CWsAttributes_GetAttributes_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0x57E3AE1F, 
		"outAttributes", 
		L"outAttributes", 
		sizeof("outAttributes")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_GetAttributes_struct, outAttributes),
		NULL,
		&__ECLAttribute_map,
		5+1,
	},
	{
		0x57E3AE1F,
		"__outAttributes_nSizeIs",
		L"__outAttributes_nSizeIs",
		sizeof("__outAttributes_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsAttributes_GetAttributes_struct, __outAttributes_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsAttributes_GetAttributes_map =
{
	0xEE843516,
	"GetAttributes",
	L"GetAttributesResponse",
	sizeof("GetAttributes")-1,
	sizeof("GetAttributesResponse")-1,
	SOAPMAP_FUNC,
	__CWsAttributes_GetAttributes_entries,
	sizeof(__CWsAttributes_GetAttributes_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xAA017D94,
	"http://webservices.seisint.com/WsAttributes",
	L"http://webservices.seisint.com/WsAttributes",
	sizeof("http://webservices.seisint.com/WsAttributes")-1
};


struct __CWsAttributes_SupportsEarMark_struct
{
	bool Support;
};

extern __declspec(selectany) const _soapmapentry __CWsAttributes_SupportsEarMark_entries[] =
{

	{
		0x0E374E5D, 
		"Support", 
		L"Support", 
		sizeof("Support")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_SupportsEarMark_struct, Support),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsAttributes_SupportsEarMark_map =
{
	0x80499502,
	"SupportsEarMark",
	L"SupportsEarMarkResponse",
	sizeof("SupportsEarMark")-1,
	sizeof("SupportsEarMarkResponse")-1,
	SOAPMAP_FUNC,
	__CWsAttributes_SupportsEarMark_entries,
	sizeof(__CWsAttributes_SupportsEarMark_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xAA017D94,
	"http://webservices.seisint.com/WsAttributes",
	L"http://webservices.seisint.com/WsAttributes",
	sizeof("http://webservices.seisint.com/WsAttributes")-1
};


struct __CWsAttributes_SaveAttributes_struct
{
	SaveAttributeRequest *Attributes;
	int __Attributes_nSizeIs;
	ArrayOfEspException Exceptions;
	ECLAttribute *outAttributes;
	int __outAttributes_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CWsAttributes_SaveAttributes_entries[] =
{

	{
		0x2072C067, 
		"Attributes", 
		L"Attributes", 
		sizeof("Attributes")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_SaveAttributes_struct, Attributes),
		NULL,
		&__SaveAttributeRequest_map,
		0+1,
	},
	{
		0x2072C067,
		"__Attributes_nSizeIs",
		L"__Attributes_nSizeIs",
		sizeof("__Attributes_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsAttributes_SaveAttributes_struct, __Attributes_nSizeIs),
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
		offsetof(__CWsAttributes_SaveAttributes_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0x57E3AE1F, 
		"outAttributes", 
		L"outAttributes", 
		sizeof("outAttributes")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_SaveAttributes_struct, outAttributes),
		NULL,
		&__ECLAttribute_map,
		3+1,
	},
	{
		0x57E3AE1F,
		"__outAttributes_nSizeIs",
		L"__outAttributes_nSizeIs",
		sizeof("__outAttributes_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsAttributes_SaveAttributes_struct, __outAttributes_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsAttributes_SaveAttributes_map =
{
	0xD99C7419,
	"SaveAttributes",
	L"UpdateAttributesResponse",
	sizeof("SaveAttributes")-1,
	sizeof("UpdateAttributesResponse")-1,
	SOAPMAP_FUNC,
	__CWsAttributes_SaveAttributes_entries,
	sizeof(__CWsAttributes_SaveAttributes_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xAA017D94,
	"http://webservices.seisint.com/WsAttributes",
	L"http://webservices.seisint.com/WsAttributes",
	sizeof("http://webservices.seisint.com/WsAttributes")-1
};


struct __CWsAttributes_RollbackAttributes_struct
{
	RollbackAttributeRequest *Attributes;
	int __Attributes_nSizeIs;
	ArrayOfEspException Exceptions;
	ECLAttribute *outAttributes;
	int __outAttributes_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CWsAttributes_RollbackAttributes_entries[] =
{

	{
		0x2072C067, 
		"Attributes", 
		L"Attributes", 
		sizeof("Attributes")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_RollbackAttributes_struct, Attributes),
		NULL,
		&__RollbackAttributeRequest_map,
		0+1,
	},
	{
		0x2072C067,
		"__Attributes_nSizeIs",
		L"__Attributes_nSizeIs",
		sizeof("__Attributes_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsAttributes_RollbackAttributes_struct, __Attributes_nSizeIs),
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
		offsetof(__CWsAttributes_RollbackAttributes_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0x57E3AE1F, 
		"outAttributes", 
		L"outAttributes", 
		sizeof("outAttributes")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_RollbackAttributes_struct, outAttributes),
		NULL,
		&__ECLAttribute_map,
		3+1,
	},
	{
		0x57E3AE1F,
		"__outAttributes_nSizeIs",
		L"__outAttributes_nSizeIs",
		sizeof("__outAttributes_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsAttributes_RollbackAttributes_struct, __outAttributes_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsAttributes_RollbackAttributes_map =
{
	0xD99C7419,
	"RollbackAttributes",
	L"UpdateAttributesResponse",
	sizeof("RollbackAttributes")-1,
	sizeof("UpdateAttributesResponse")-1,
	SOAPMAP_FUNC,
	__CWsAttributes_RollbackAttributes_entries,
	sizeof(__CWsAttributes_RollbackAttributes_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xAA017D94,
	"http://webservices.seisint.com/WsAttributes",
	L"http://webservices.seisint.com/WsAttributes",
	sizeof("http://webservices.seisint.com/WsAttributes")-1
};


struct __CWsAttributes_DeleteModule_struct
{
	BSTR ModuleName;
	ArrayOfEspException Exceptions;
};

extern __declspec(selectany) const _soapmapentry __CWsAttributes_DeleteModule_entries[] =
{

	{
		0x3474D1E7, 
		"ModuleName", 
		L"ModuleName", 
		sizeof("ModuleName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_DeleteModule_struct, ModuleName),
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
		offsetof(__CWsAttributes_DeleteModule_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsAttributes_DeleteModule_map =
{
	0x6DE12748,
	"DeleteModule",
	L"DeleteModuleResponse",
	sizeof("DeleteModule")-1,
	sizeof("DeleteModuleResponse")-1,
	SOAPMAP_FUNC,
	__CWsAttributes_DeleteModule_entries,
	sizeof(__CWsAttributes_DeleteModule_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xAA017D94,
	"http://webservices.seisint.com/WsAttributes",
	L"http://webservices.seisint.com/WsAttributes",
	sizeof("http://webservices.seisint.com/WsAttributes")-1
};


struct __CWsAttributes_FindAttributes_struct
{
	BSTR ModuleName;
	BSTR AttributeName;
	BSTR UserName;
	BSTR Pattern;
	BSTR Regexp;
	BSTR ChangedSince;
	BSTR *TypeList;
	int __TypeList_nSizeIs;
	bool Sandboxed;
	bool Locked;
	bool CheckedOut;
	bool Orphaned;
	bool GetText;
	bool GetMatchedLines;
	ArrayOfEspException Exceptions;
	ECLAttribute *outAttributes;
	int __outAttributes_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CWsAttributes_FindAttributes_entries[] =
{

	{
		0x3474D1E7, 
		"ModuleName", 
		L"ModuleName", 
		sizeof("ModuleName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_FindAttributes_struct, ModuleName),
		NULL,
		NULL,
		-1,
	},
	{
		0x0CA09C15, 
		"AttributeName", 
		L"AttributeName", 
		sizeof("AttributeName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_FindAttributes_struct, AttributeName),
		NULL,
		NULL,
		-1,
	},
	{
		0xFA195B00, 
		"UserName", 
		L"UserName", 
		sizeof("UserName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_FindAttributes_struct, UserName),
		NULL,
		NULL,
		-1,
	},
	{
		0xF8EBD47E, 
		"Pattern", 
		L"Pattern", 
		sizeof("Pattern")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_FindAttributes_struct, Pattern),
		NULL,
		NULL,
		-1,
	},
	{
		0xC6A4E70B, 
		"Regexp", 
		L"Regexp", 
		sizeof("Regexp")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_FindAttributes_struct, Regexp),
		NULL,
		NULL,
		-1,
	},
	{
		0xEF6882FC, 
		"ChangedSince", 
		L"ChangedSince", 
		sizeof("ChangedSince")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_FindAttributes_struct, ChangedSince),
		NULL,
		NULL,
		-1,
	},
	{
		0xF4717D1E, 
		"TypeList", 
		L"TypeList", 
		sizeof("TypeList")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_FindAttributes_struct, TypeList),
		NULL,
		NULL,
		6+1,
	},
	{
		0xF4717D1E,
		"__TypeList_nSizeIs",
		L"__TypeList_nSizeIs",
		sizeof("__TypeList_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsAttributes_FindAttributes_struct, __TypeList_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x4D4091B8, 
		"Sandboxed", 
		L"Sandboxed", 
		sizeof("Sandboxed")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_FindAttributes_struct, Sandboxed),
		NULL,
		NULL,
		-1,
	},
	{
		0xB958CDD2, 
		"Locked", 
		L"Locked", 
		sizeof("Locked")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_FindAttributes_struct, Locked),
		NULL,
		NULL,
		-1,
	},
	{
		0x5D043B1F, 
		"CheckedOut", 
		L"CheckedOut", 
		sizeof("CheckedOut")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_FindAttributes_struct, CheckedOut),
		NULL,
		NULL,
		-1,
	},
	{
		0x3C928C51, 
		"Orphaned", 
		L"Orphaned", 
		sizeof("Orphaned")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_FindAttributes_struct, Orphaned),
		NULL,
		NULL,
		-1,
	},
	{
		0x4D62C8E5, 
		"GetText", 
		L"GetText", 
		sizeof("GetText")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_FindAttributes_struct, GetText),
		NULL,
		NULL,
		-1,
	},
	{
		0x0ACAC391, 
		"GetMatchedLines", 
		L"GetMatchedLines", 
		sizeof("GetMatchedLines")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_FindAttributes_struct, GetMatchedLines),
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
		offsetof(__CWsAttributes_FindAttributes_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0x57E3AE1F, 
		"outAttributes", 
		L"outAttributes", 
		sizeof("outAttributes")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsAttributes_FindAttributes_struct, outAttributes),
		NULL,
		&__ECLAttribute_map,
		15+1,
	},
	{
		0x57E3AE1F,
		"__outAttributes_nSizeIs",
		L"__outAttributes_nSizeIs",
		sizeof("__outAttributes_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsAttributes_FindAttributes_struct, __outAttributes_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsAttributes_FindAttributes_map =
{
	0x2D141F97,
	"FindAttributes",
	L"FindAttributesResponse",
	sizeof("FindAttributes")-1,
	sizeof("FindAttributesResponse")-1,
	SOAPMAP_FUNC,
	__CWsAttributes_FindAttributes_entries,
	sizeof(__CWsAttributes_FindAttributes_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xAA017D94,
	"http://webservices.seisint.com/WsAttributes",
	L"http://webservices.seisint.com/WsAttributes",
	sizeof("http://webservices.seisint.com/WsAttributes")-1
};

extern __declspec(selectany) const _soapmap * __CWsAttributes_funcs[] =
{
	&__CWsAttributes_GetAttribute_map,
	&__CWsAttributes_CreateModule_map,
	&__CWsAttributes_CreateAttribute_map,
	&__CWsAttributes_CheckinAttributes_map,
	&__CWsAttributes_RenameAttributes_map,
	&__CWsAttributes_DeleteAttributes_map,
	&__CWsAttributes_GetLabels_map,
	&__CWsAttributes_GetAttributeLabels_map,
	&__CWsAttributes_GetAttributeHistory_map,
	&__CWsAttributes_UpdateLabel_map,
	&__CWsAttributes_ListAttributeTypes_map,
	&__CWsAttributes_ImportAttributes_map,
	&__CWsAttributes_DeleteLabel_map,
	&__CWsAttributes_GetModules_map,
	&__CWsAttributes_CreateLabel_map,
	&__CWsAttributes_CheckoutAttributes_map,
	&__CWsAttributes_RenameModule_map,
	&__CWsAttributes_GetAttributes_map,
	&__CWsAttributes_SupportsEarMark_map,
	&__CWsAttributes_SaveAttributes_map,
	&__CWsAttributes_RollbackAttributes_map,
	&__CWsAttributes_DeleteModule_map,
	&__CWsAttributes_FindAttributes_map,
	NULL
};

template <typename TClient>
inline HRESULT CWsAttributesT<TClient>::GetAttribute(
		BSTR ModuleName, 
		BSTR AttributeName, 
		BSTR Type, 
		int Version, 
		bool GetSandbox, 
		bool GetText, 
		BSTR Label, 
		ArrayOfEspException* Exceptions, 
		ECLAttribute* outAttribute
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( outAttribute == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsAttributes_GetAttribute_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.ModuleName = ModuleName;
	__params.AttributeName = AttributeName;
	__params.Type = Type;
	__params.Version = Version;
	__params.GetSandbox = GetSandbox;
	__params.GetText = GetText;
	__params.Label = Label;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsAttributes/GetAttribute\?ver_=1.2\"\r\n"));
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
	*outAttribute = __params.outAttribute;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsAttributesT<TClient>::CreateModule(
		BSTR ModuleName, 
		ArrayOfEspException* Exceptions, 
		ECLModule* ModuleInfo
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( ModuleInfo == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsAttributes_CreateModule_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.ModuleName = ModuleName;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsAttributes/CreateModule\?ver_=1.2\"\r\n"));
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
	*ModuleInfo = __params.ModuleInfo;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsAttributesT<TClient>::CreateAttribute(
		BSTR ModuleName, 
		BSTR AttributeName, 
		BSTR Type, 
		ArrayOfEspException* Exceptions, 
		ECLAttribute* AttributeInfo
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( AttributeInfo == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsAttributes_CreateAttribute_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.ModuleName = ModuleName;
	__params.AttributeName = AttributeName;
	__params.Type = Type;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsAttributes/CreateAttribute\?ver_=1.2\"\r\n"));
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
	*AttributeInfo = __params.AttributeInfo;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsAttributesT<TClient>::CheckinAttributes(
		CheckinAttributeRequest* Attributes, int __Attributes_nSizeIs, 
		ArrayOfEspException* Exceptions, 
		ECLAttribute** outAttributes, int* __outAttributes_nSizeIs
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( outAttributes == NULL )
		return E_POINTER;
	if( __outAttributes_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsAttributes_CheckinAttributes_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Attributes = Attributes;
	__params.__Attributes_nSizeIs = __Attributes_nSizeIs;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsAttributes/CheckinAttributes\?ver_=1.2\"\r\n"));
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
	*outAttributes = __params.outAttributes;
	*__outAttributes_nSizeIs = __params.__outAttributes_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsAttributesT<TClient>::RenameAttributes(
		RenameAttributeRequest* Attributes, int __Attributes_nSizeIs, 
		ArrayOfEspException* Exceptions, 
		ECLAttribute** outAttributes, int* __outAttributes_nSizeIs
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( outAttributes == NULL )
		return E_POINTER;
	if( __outAttributes_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsAttributes_RenameAttributes_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Attributes = Attributes;
	__params.__Attributes_nSizeIs = __Attributes_nSizeIs;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsAttributes/RenameAttributes\?ver_=1.2\"\r\n"));
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
	*outAttributes = __params.outAttributes;
	*__outAttributes_nSizeIs = __params.__outAttributes_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsAttributesT<TClient>::DeleteAttributes(
		DeleteAttributeRequest* Attributes, int __Attributes_nSizeIs, 
		ArrayOfEspException* Exceptions, 
		ECLAttribute** outAttributes, int* __outAttributes_nSizeIs
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( outAttributes == NULL )
		return E_POINTER;
	if( __outAttributes_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsAttributes_DeleteAttributes_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Attributes = Attributes;
	__params.__Attributes_nSizeIs = __Attributes_nSizeIs;

	__atlsoap_hr = SetClientStruct(&__params, 5);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsAttributes/DeleteAttributes\?ver_=1.2\"\r\n"));
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
	*outAttributes = __params.outAttributes;
	*__outAttributes_nSizeIs = __params.__outAttributes_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsAttributesT<TClient>::GetLabels(
		BSTR Name, 
		ArrayOfEspException* Exceptions, 
		ECLLabel** outLabels, int* __outLabels_nSizeIs
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( outLabels == NULL )
		return E_POINTER;
	if( __outLabels_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsAttributes_GetLabels_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Name = Name;

	__atlsoap_hr = SetClientStruct(&__params, 6);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsAttributes/GetLabels\?ver_=1.2\"\r\n"));
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
	*outLabels = __params.outLabels;
	*__outLabels_nSizeIs = __params.__outLabels_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsAttributesT<TClient>::GetAttributeLabels(
		BSTR ModuleName, 
		BSTR AttributeName, 
		ArrayOfEspException* Exceptions, 
		ECLAttributeLabel** Labels, int* __Labels_nSizeIs
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( Labels == NULL )
		return E_POINTER;
	if( __Labels_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsAttributes_GetAttributeLabels_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.ModuleName = ModuleName;
	__params.AttributeName = AttributeName;

	__atlsoap_hr = SetClientStruct(&__params, 7);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsAttributes/GetAttributeLabels\?ver_=1.2\"\r\n"));
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
	*Labels = __params.Labels;
	*__Labels_nSizeIs = __params.__Labels_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsAttributesT<TClient>::GetAttributeHistory(
		BSTR ModuleName, 
		BSTR AttributeName, 
		BSTR FromDate, 
		BSTR ToDate, 
		ArrayOfEspException* Exceptions, 
		ECLAttribute** outAttributes, int* __outAttributes_nSizeIs
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( outAttributes == NULL )
		return E_POINTER;
	if( __outAttributes_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsAttributes_GetAttributeHistory_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.ModuleName = ModuleName;
	__params.AttributeName = AttributeName;
	__params.FromDate = FromDate;
	__params.ToDate = ToDate;

	__atlsoap_hr = SetClientStruct(&__params, 8);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsAttributes/GetAttributeHistory\?ver_=1.2\"\r\n"));
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
	*outAttributes = __params.outAttributes;
	*__outAttributes_nSizeIs = __params.__outAttributes_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsAttributesT<TClient>::UpdateLabel(
		BSTR Name, 
		BSTR ModuleName, 
		BSTR AtributeName, 
		int AttrbuteVersion, 
		ArrayOfEspException* Exceptions
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsAttributes_UpdateLabel_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Name = Name;
	__params.ModuleName = ModuleName;
	__params.AtributeName = AtributeName;
	__params.AttrbuteVersion = AttrbuteVersion;

	__atlsoap_hr = SetClientStruct(&__params, 9);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsAttributes/UpdateLabel\?ver_=1.2\"\r\n"));
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
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsAttributesT<TClient>::ListAttributeTypes(
		AttributeType** Types, int* __Types_nSizeIs
	)
{
    if ( Types == NULL )
		return E_POINTER;
	if( __Types_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsAttributes_ListAttributeTypes_struct __params;
	memset(&__params, 0x00, sizeof(__params));

	__atlsoap_hr = SetClientStruct(&__params, 10);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsAttributes/ListAttributeTypes\?ver_=1.2\"\r\n"));
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

	*Types = __params.Types;
	*__Types_nSizeIs = __params.__Types_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsAttributesT<TClient>::ImportAttributes(
		ImportAttributeRequest* Attributes, int __Attributes_nSizeIs, 
		ArrayOfEspException* Exceptions, 
		ECLAttribute** outAttributes, int* __outAttributes_nSizeIs
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( outAttributes == NULL )
		return E_POINTER;
	if( __outAttributes_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsAttributes_ImportAttributes_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Attributes = Attributes;
	__params.__Attributes_nSizeIs = __Attributes_nSizeIs;

	__atlsoap_hr = SetClientStruct(&__params, 11);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsAttributes/ImportAttributes\?ver_=1.2\"\r\n"));
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
	*outAttributes = __params.outAttributes;
	*__outAttributes_nSizeIs = __params.__outAttributes_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsAttributesT<TClient>::DeleteLabel(
		BSTR Name, 
		ArrayOfEspException* Exceptions
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsAttributes_DeleteLabel_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Name = Name;

	__atlsoap_hr = SetClientStruct(&__params, 12);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsAttributes/DeleteLabel\?ver_=1.2\"\r\n"));
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
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsAttributesT<TClient>::GetModules(
		__int64 ModifiedSince, 
		bool IncludeDeleted, 
		bool GetChecksum, 
		BSTR Label, 
		BSTR EarMark, 
		ArrayOfEspException* Exceptions, 
		ECLModule** outModules, int* __outModules_nSizeIs
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( outModules == NULL )
		return E_POINTER;
	if( __outModules_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsAttributes_GetModules_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.ModifiedSince = ModifiedSince;
	__params.IncludeDeleted = IncludeDeleted;
	__params.GetChecksum = GetChecksum;
	__params.Label = Label;
	__params.EarMark = EarMark;

	__atlsoap_hr = SetClientStruct(&__params, 13);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsAttributes/GetModules\?ver_=1.2\"\r\n"));
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
	*outModules = __params.outModules;
	*__outModules_nSizeIs = __params.__outModules_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsAttributesT<TClient>::CreateLabel(
		BSTR Name, 
		ArrayOfEspException* Exceptions
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsAttributes_CreateLabel_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Name = Name;

	__atlsoap_hr = SetClientStruct(&__params, 14);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsAttributes/CreateLabel\?ver_=1.2\"\r\n"));
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
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsAttributesT<TClient>::CheckoutAttributes(
		CheckoutAttributeRequest* Attributes, int __Attributes_nSizeIs, 
		ArrayOfEspException* Exceptions, 
		ECLAttribute** outAttributes, int* __outAttributes_nSizeIs
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( outAttributes == NULL )
		return E_POINTER;
	if( __outAttributes_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsAttributes_CheckoutAttributes_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Attributes = Attributes;
	__params.__Attributes_nSizeIs = __Attributes_nSizeIs;

	__atlsoap_hr = SetClientStruct(&__params, 15);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsAttributes/CheckoutAttributes\?ver_=1.2\"\r\n"));
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
	*outAttributes = __params.outAttributes;
	*__outAttributes_nSizeIs = __params.__outAttributes_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsAttributesT<TClient>::RenameModule(
		BSTR ModuleName, 
		BSTR NewModuleName, 
		ArrayOfEspException* Exceptions, 
		ECLModule* ModuleInfo
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( ModuleInfo == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsAttributes_RenameModule_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.ModuleName = ModuleName;
	__params.NewModuleName = NewModuleName;

	__atlsoap_hr = SetClientStruct(&__params, 16);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsAttributes/RenameModule\?ver_=1.2\"\r\n"));
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
	*ModuleInfo = __params.ModuleInfo;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsAttributesT<TClient>::GetAttributes(
		BSTR ModuleName, 
		BSTR* TypeList, int __TypeList_nSizeIs, 
		BSTR Label, 
		ArrayOfEspException* Exceptions, 
		ECLAttribute** outAttributes, int* __outAttributes_nSizeIs
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( outAttributes == NULL )
		return E_POINTER;
	if( __outAttributes_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsAttributes_GetAttributes_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.ModuleName = ModuleName;
	__params.TypeList = TypeList;
	__params.__TypeList_nSizeIs = __TypeList_nSizeIs;
	__params.Label = Label;

	__atlsoap_hr = SetClientStruct(&__params, 17);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsAttributes/GetAttributes\?ver_=1.2\"\r\n"));
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
	*outAttributes = __params.outAttributes;
	*__outAttributes_nSizeIs = __params.__outAttributes_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsAttributesT<TClient>::SupportsEarMark(
		bool* Support
	)
{
    if ( Support == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsAttributes_SupportsEarMark_struct __params;
	memset(&__params, 0x00, sizeof(__params));

	__atlsoap_hr = SetClientStruct(&__params, 18);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsAttributes/SupportsEarMark\?ver_=1.2\"\r\n"));
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

	*Support = __params.Support;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsAttributesT<TClient>::SaveAttributes(
		SaveAttributeRequest* Attributes, int __Attributes_nSizeIs, 
		ArrayOfEspException* Exceptions, 
		ECLAttribute** outAttributes, int* __outAttributes_nSizeIs
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( outAttributes == NULL )
		return E_POINTER;
	if( __outAttributes_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsAttributes_SaveAttributes_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Attributes = Attributes;
	__params.__Attributes_nSizeIs = __Attributes_nSizeIs;

	__atlsoap_hr = SetClientStruct(&__params, 19);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsAttributes/SaveAttributes\?ver_=1.2\"\r\n"));
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
	*outAttributes = __params.outAttributes;
	*__outAttributes_nSizeIs = __params.__outAttributes_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsAttributesT<TClient>::RollbackAttributes(
		RollbackAttributeRequest* Attributes, int __Attributes_nSizeIs, 
		ArrayOfEspException* Exceptions, 
		ECLAttribute** outAttributes, int* __outAttributes_nSizeIs
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( outAttributes == NULL )
		return E_POINTER;
	if( __outAttributes_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsAttributes_RollbackAttributes_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Attributes = Attributes;
	__params.__Attributes_nSizeIs = __Attributes_nSizeIs;

	__atlsoap_hr = SetClientStruct(&__params, 20);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsAttributes/RollbackAttributes\?ver_=1.2\"\r\n"));
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
	*outAttributes = __params.outAttributes;
	*__outAttributes_nSizeIs = __params.__outAttributes_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsAttributesT<TClient>::DeleteModule(
		BSTR ModuleName, 
		ArrayOfEspException* Exceptions
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsAttributes_DeleteModule_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.ModuleName = ModuleName;

	__atlsoap_hr = SetClientStruct(&__params, 21);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsAttributes/DeleteModule\?ver_=1.2\"\r\n"));
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
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsAttributesT<TClient>::FindAttributes(
		BSTR ModuleName, 
		BSTR AttributeName, 
		BSTR UserName, 
		BSTR Pattern, 
		BSTR Regexp, 
		BSTR ChangedSince, 
		BSTR* TypeList, int __TypeList_nSizeIs, 
		bool Sandboxed, 
		bool Locked, 
		bool CheckedOut, 
		bool Orphaned, 
		bool GetText, 
		bool GetMatchedLines, 
		ArrayOfEspException* Exceptions, 
		ECLAttribute** outAttributes, int* __outAttributes_nSizeIs
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( outAttributes == NULL )
		return E_POINTER;
	if( __outAttributes_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsAttributes_FindAttributes_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.ModuleName = ModuleName;
	__params.AttributeName = AttributeName;
	__params.UserName = UserName;
	__params.Pattern = Pattern;
	__params.Regexp = Regexp;
	__params.ChangedSince = ChangedSince;
	__params.TypeList = TypeList;
	__params.__TypeList_nSizeIs = __TypeList_nSizeIs;
	__params.Sandboxed = Sandboxed;
	__params.Locked = Locked;
	__params.CheckedOut = CheckedOut;
	__params.Orphaned = Orphaned;
	__params.GetText = GetText;
	__params.GetMatchedLines = GetMatchedLines;

	__atlsoap_hr = SetClientStruct(&__params, 22);
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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsAttributes/FindAttributes\?ver_=1.2\"\r\n"));
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
	*outAttributes = __params.outAttributes;
	*__outAttributes_nSizeIs = __params.__outAttributes_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
ATL_NOINLINE inline const _soapmap ** CWsAttributesT<TClient>::GetFunctionMap()
{
	return __CWsAttributes_funcs;
}

template <typename TClient>
ATL_NOINLINE inline const _soapmap ** CWsAttributesT<TClient>::GetHeaderMap()
{
	static const _soapmapentry __CWsAttributes_GetAttribute_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsAttributes_GetAttribute_atlsoapheader_map = 
	{
		0xA00F5323,
		"GetAttribute",
		L"GetAttributeResponse",
		sizeof("GetAttribute")-1,
		sizeof("GetAttributeResponse")-1,
		SOAPMAP_HEADER,
		__CWsAttributes_GetAttribute_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xAA017D94,
		"http://webservices.seisint.com/WsAttributes",
		L"http://webservices.seisint.com/WsAttributes",
		sizeof("http://webservices.seisint.com/WsAttributes")-1
	};

	static const _soapmapentry __CWsAttributes_CreateModule_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsAttributes_CreateModule_atlsoapheader_map = 
	{
		0xCECEB349,
		"CreateModule",
		L"CreateModuleResponse",
		sizeof("CreateModule")-1,
		sizeof("CreateModuleResponse")-1,
		SOAPMAP_HEADER,
		__CWsAttributes_CreateModule_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xAA017D94,
		"http://webservices.seisint.com/WsAttributes",
		L"http://webservices.seisint.com/WsAttributes",
		sizeof("http://webservices.seisint.com/WsAttributes")-1
	};

	static const _soapmapentry __CWsAttributes_CreateAttribute_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsAttributes_CreateAttribute_atlsoapheader_map = 
	{
		0x366AA3F7,
		"CreateAttribute",
		L"CreateAttributeResponse",
		sizeof("CreateAttribute")-1,
		sizeof("CreateAttributeResponse")-1,
		SOAPMAP_HEADER,
		__CWsAttributes_CreateAttribute_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xAA017D94,
		"http://webservices.seisint.com/WsAttributes",
		L"http://webservices.seisint.com/WsAttributes",
		sizeof("http://webservices.seisint.com/WsAttributes")-1
	};

	static const _soapmapentry __CWsAttributes_CheckinAttributes_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsAttributes_CheckinAttributes_atlsoapheader_map = 
	{
		0xD99C7419,
		"CheckinAttributes",
		L"UpdateAttributesResponse",
		sizeof("CheckinAttributes")-1,
		sizeof("UpdateAttributesResponse")-1,
		SOAPMAP_HEADER,
		__CWsAttributes_CheckinAttributes_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xAA017D94,
		"http://webservices.seisint.com/WsAttributes",
		L"http://webservices.seisint.com/WsAttributes",
		sizeof("http://webservices.seisint.com/WsAttributes")-1
	};

	static const _soapmapentry __CWsAttributes_RenameAttributes_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsAttributes_RenameAttributes_atlsoapheader_map = 
	{
		0xD99C7419,
		"RenameAttributes",
		L"UpdateAttributesResponse",
		sizeof("RenameAttributes")-1,
		sizeof("UpdateAttributesResponse")-1,
		SOAPMAP_HEADER,
		__CWsAttributes_RenameAttributes_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xAA017D94,
		"http://webservices.seisint.com/WsAttributes",
		L"http://webservices.seisint.com/WsAttributes",
		sizeof("http://webservices.seisint.com/WsAttributes")-1
	};

	static const _soapmapentry __CWsAttributes_DeleteAttributes_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsAttributes_DeleteAttributes_atlsoapheader_map = 
	{
		0xD99C7419,
		"DeleteAttributes",
		L"UpdateAttributesResponse",
		sizeof("DeleteAttributes")-1,
		sizeof("UpdateAttributesResponse")-1,
		SOAPMAP_HEADER,
		__CWsAttributes_DeleteAttributes_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xAA017D94,
		"http://webservices.seisint.com/WsAttributes",
		L"http://webservices.seisint.com/WsAttributes",
		sizeof("http://webservices.seisint.com/WsAttributes")-1
	};

	static const _soapmapentry __CWsAttributes_GetLabels_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsAttributes_GetLabels_atlsoapheader_map = 
	{
		0xB52B3282,
		"GetLabels",
		L"GetLabelsResponse",
		sizeof("GetLabels")-1,
		sizeof("GetLabelsResponse")-1,
		SOAPMAP_HEADER,
		__CWsAttributes_GetLabels_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xAA017D94,
		"http://webservices.seisint.com/WsAttributes",
		L"http://webservices.seisint.com/WsAttributes",
		sizeof("http://webservices.seisint.com/WsAttributes")-1
	};

	static const _soapmapentry __CWsAttributes_GetAttributeLabels_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsAttributes_GetAttributeLabels_atlsoapheader_map = 
	{
		0x15522EF6,
		"GetAttributeLabels",
		L"GetAttributeLabelsResponse",
		sizeof("GetAttributeLabels")-1,
		sizeof("GetAttributeLabelsResponse")-1,
		SOAPMAP_HEADER,
		__CWsAttributes_GetAttributeLabels_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xAA017D94,
		"http://webservices.seisint.com/WsAttributes",
		L"http://webservices.seisint.com/WsAttributes",
		sizeof("http://webservices.seisint.com/WsAttributes")-1
	};

	static const _soapmapentry __CWsAttributes_GetAttributeHistory_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsAttributes_GetAttributeHistory_atlsoapheader_map = 
	{
		0xC38EF035,
		"GetAttributeHistory",
		L"GetAttributeHistoryResponse",
		sizeof("GetAttributeHistory")-1,
		sizeof("GetAttributeHistoryResponse")-1,
		SOAPMAP_HEADER,
		__CWsAttributes_GetAttributeHistory_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xAA017D94,
		"http://webservices.seisint.com/WsAttributes",
		L"http://webservices.seisint.com/WsAttributes",
		sizeof("http://webservices.seisint.com/WsAttributes")-1
	};

	static const _soapmapentry __CWsAttributes_UpdateLabel_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsAttributes_UpdateLabel_atlsoapheader_map = 
	{
		0x42F76432,
		"UpdateLabel",
		L"UpdateLabelResponse",
		sizeof("UpdateLabel")-1,
		sizeof("UpdateLabelResponse")-1,
		SOAPMAP_HEADER,
		__CWsAttributes_UpdateLabel_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xAA017D94,
		"http://webservices.seisint.com/WsAttributes",
		L"http://webservices.seisint.com/WsAttributes",
		sizeof("http://webservices.seisint.com/WsAttributes")-1
	};

	static const _soapmapentry __CWsAttributes_ListAttributeTypes_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsAttributes_ListAttributeTypes_atlsoapheader_map = 
	{
		0xBCA452F4,
		"ListAttributeTypes",
		L"ListAttributeTypesResponse",
		sizeof("ListAttributeTypes")-1,
		sizeof("ListAttributeTypesResponse")-1,
		SOAPMAP_HEADER,
		__CWsAttributes_ListAttributeTypes_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xAA017D94,
		"http://webservices.seisint.com/WsAttributes",
		L"http://webservices.seisint.com/WsAttributes",
		sizeof("http://webservices.seisint.com/WsAttributes")-1
	};

	static const _soapmapentry __CWsAttributes_ImportAttributes_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsAttributes_ImportAttributes_atlsoapheader_map = 
	{
		0xD99C7419,
		"ImportAttributes",
		L"UpdateAttributesResponse",
		sizeof("ImportAttributes")-1,
		sizeof("UpdateAttributesResponse")-1,
		SOAPMAP_HEADER,
		__CWsAttributes_ImportAttributes_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xAA017D94,
		"http://webservices.seisint.com/WsAttributes",
		L"http://webservices.seisint.com/WsAttributes",
		sizeof("http://webservices.seisint.com/WsAttributes")-1
	};

	static const _soapmapentry __CWsAttributes_DeleteLabel_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsAttributes_DeleteLabel_atlsoapheader_map = 
	{
		0x1126CA02,
		"DeleteLabel",
		L"DeleteLabelResponse",
		sizeof("DeleteLabel")-1,
		sizeof("DeleteLabelResponse")-1,
		SOAPMAP_HEADER,
		__CWsAttributes_DeleteLabel_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xAA017D94,
		"http://webservices.seisint.com/WsAttributes",
		L"http://webservices.seisint.com/WsAttributes",
		sizeof("http://webservices.seisint.com/WsAttributes")-1
	};

	static const _soapmapentry __CWsAttributes_GetModules_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsAttributes_GetModules_atlsoapheader_map = 
	{
		0xFD468E28,
		"GetModules",
		L"GetModulesResponse",
		sizeof("GetModules")-1,
		sizeof("GetModulesResponse")-1,
		SOAPMAP_HEADER,
		__CWsAttributes_GetModules_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xAA017D94,
		"http://webservices.seisint.com/WsAttributes",
		L"http://webservices.seisint.com/WsAttributes",
		sizeof("http://webservices.seisint.com/WsAttributes")-1
	};

	static const _soapmapentry __CWsAttributes_CreateLabel_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsAttributes_CreateLabel_atlsoapheader_map = 
	{
		0x42A259E3,
		"CreateLabel",
		L"CreateLabelResponse",
		sizeof("CreateLabel")-1,
		sizeof("CreateLabelResponse")-1,
		SOAPMAP_HEADER,
		__CWsAttributes_CreateLabel_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xAA017D94,
		"http://webservices.seisint.com/WsAttributes",
		L"http://webservices.seisint.com/WsAttributes",
		sizeof("http://webservices.seisint.com/WsAttributes")-1
	};

	static const _soapmapentry __CWsAttributes_CheckoutAttributes_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsAttributes_CheckoutAttributes_atlsoapheader_map = 
	{
		0xD99C7419,
		"CheckoutAttributes",
		L"UpdateAttributesResponse",
		sizeof("CheckoutAttributes")-1,
		sizeof("UpdateAttributesResponse")-1,
		SOAPMAP_HEADER,
		__CWsAttributes_CheckoutAttributes_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xAA017D94,
		"http://webservices.seisint.com/WsAttributes",
		L"http://webservices.seisint.com/WsAttributes",
		sizeof("http://webservices.seisint.com/WsAttributes")-1
	};

	static const _soapmapentry __CWsAttributes_RenameModule_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsAttributes_RenameModule_atlsoapheader_map = 
	{
		0xD35663AD,
		"RenameModule",
		L"RenameModuleResponse",
		sizeof("RenameModule")-1,
		sizeof("RenameModuleResponse")-1,
		SOAPMAP_HEADER,
		__CWsAttributes_RenameModule_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xAA017D94,
		"http://webservices.seisint.com/WsAttributes",
		L"http://webservices.seisint.com/WsAttributes",
		sizeof("http://webservices.seisint.com/WsAttributes")-1
	};

	static const _soapmapentry __CWsAttributes_GetAttributes_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsAttributes_GetAttributes_atlsoapheader_map = 
	{
		0xEE843516,
		"GetAttributes",
		L"GetAttributesResponse",
		sizeof("GetAttributes")-1,
		sizeof("GetAttributesResponse")-1,
		SOAPMAP_HEADER,
		__CWsAttributes_GetAttributes_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xAA017D94,
		"http://webservices.seisint.com/WsAttributes",
		L"http://webservices.seisint.com/WsAttributes",
		sizeof("http://webservices.seisint.com/WsAttributes")-1
	};

	static const _soapmapentry __CWsAttributes_SupportsEarMark_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsAttributes_SupportsEarMark_atlsoapheader_map = 
	{
		0x80499502,
		"SupportsEarMark",
		L"SupportsEarMarkResponse",
		sizeof("SupportsEarMark")-1,
		sizeof("SupportsEarMarkResponse")-1,
		SOAPMAP_HEADER,
		__CWsAttributes_SupportsEarMark_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xAA017D94,
		"http://webservices.seisint.com/WsAttributes",
		L"http://webservices.seisint.com/WsAttributes",
		sizeof("http://webservices.seisint.com/WsAttributes")-1
	};

	static const _soapmapentry __CWsAttributes_SaveAttributes_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsAttributes_SaveAttributes_atlsoapheader_map = 
	{
		0xD99C7419,
		"SaveAttributes",
		L"UpdateAttributesResponse",
		sizeof("SaveAttributes")-1,
		sizeof("UpdateAttributesResponse")-1,
		SOAPMAP_HEADER,
		__CWsAttributes_SaveAttributes_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xAA017D94,
		"http://webservices.seisint.com/WsAttributes",
		L"http://webservices.seisint.com/WsAttributes",
		sizeof("http://webservices.seisint.com/WsAttributes")-1
	};

	static const _soapmapentry __CWsAttributes_RollbackAttributes_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsAttributes_RollbackAttributes_atlsoapheader_map = 
	{
		0xD99C7419,
		"RollbackAttributes",
		L"UpdateAttributesResponse",
		sizeof("RollbackAttributes")-1,
		sizeof("UpdateAttributesResponse")-1,
		SOAPMAP_HEADER,
		__CWsAttributes_RollbackAttributes_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xAA017D94,
		"http://webservices.seisint.com/WsAttributes",
		L"http://webservices.seisint.com/WsAttributes",
		sizeof("http://webservices.seisint.com/WsAttributes")-1
	};

	static const _soapmapentry __CWsAttributes_DeleteModule_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsAttributes_DeleteModule_atlsoapheader_map = 
	{
		0x6DE12748,
		"DeleteModule",
		L"DeleteModuleResponse",
		sizeof("DeleteModule")-1,
		sizeof("DeleteModuleResponse")-1,
		SOAPMAP_HEADER,
		__CWsAttributes_DeleteModule_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xAA017D94,
		"http://webservices.seisint.com/WsAttributes",
		L"http://webservices.seisint.com/WsAttributes",
		sizeof("http://webservices.seisint.com/WsAttributes")-1
	};

	static const _soapmapentry __CWsAttributes_FindAttributes_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsAttributes_FindAttributes_atlsoapheader_map = 
	{
		0x2D141F97,
		"FindAttributes",
		L"FindAttributesResponse",
		sizeof("FindAttributes")-1,
		sizeof("FindAttributesResponse")-1,
		SOAPMAP_HEADER,
		__CWsAttributes_FindAttributes_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xAA017D94,
		"http://webservices.seisint.com/WsAttributes",
		L"http://webservices.seisint.com/WsAttributes",
		sizeof("http://webservices.seisint.com/WsAttributes")-1
	};


	static const _soapmap * __CWsAttributes_headers[] =
	{
		&__CWsAttributes_GetAttribute_atlsoapheader_map,
		&__CWsAttributes_CreateModule_atlsoapheader_map,
		&__CWsAttributes_CreateAttribute_atlsoapheader_map,
		&__CWsAttributes_CheckinAttributes_atlsoapheader_map,
		&__CWsAttributes_RenameAttributes_atlsoapheader_map,
		&__CWsAttributes_DeleteAttributes_atlsoapheader_map,
		&__CWsAttributes_GetLabels_atlsoapheader_map,
		&__CWsAttributes_GetAttributeLabels_atlsoapheader_map,
		&__CWsAttributes_GetAttributeHistory_atlsoapheader_map,
		&__CWsAttributes_UpdateLabel_atlsoapheader_map,
		&__CWsAttributes_ListAttributeTypes_atlsoapheader_map,
		&__CWsAttributes_ImportAttributes_atlsoapheader_map,
		&__CWsAttributes_DeleteLabel_atlsoapheader_map,
		&__CWsAttributes_GetModules_atlsoapheader_map,
		&__CWsAttributes_CreateLabel_atlsoapheader_map,
		&__CWsAttributes_CheckoutAttributes_atlsoapheader_map,
		&__CWsAttributes_RenameModule_atlsoapheader_map,
		&__CWsAttributes_GetAttributes_atlsoapheader_map,
		&__CWsAttributes_SupportsEarMark_atlsoapheader_map,
		&__CWsAttributes_SaveAttributes_atlsoapheader_map,
		&__CWsAttributes_RollbackAttributes_atlsoapheader_map,
		&__CWsAttributes_DeleteModule_atlsoapheader_map,
		&__CWsAttributes_FindAttributes_atlsoapheader_map,
		NULL
	};
	
	return __CWsAttributes_headers;
}

template <typename TClient>
ATL_NOINLINE inline void * CWsAttributesT<TClient>::GetHeaderValue()
{
	return this;
}

template <typename TClient>
ATL_NOINLINE inline const wchar_t * CWsAttributesT<TClient>::GetNamespaceUri()
{
	return L"http://webservices.seisint.com/WsAttributes";
}

template <typename TClient>
ATL_NOINLINE inline const char * CWsAttributesT<TClient>::GetServiceName()
{
	return NULL;
}

template <typename TClient>
ATL_NOINLINE inline const char * CWsAttributesT<TClient>::GetNamespaceUriA()
{
	return "http://webservices.seisint.com/WsAttributes";
}

template <typename TClient>
ATL_NOINLINE inline HRESULT CWsAttributesT<TClient>::CallFunction(
	void *, 
	const wchar_t *, int,
	size_t)
{
	return E_NOTIMPL;
}

template <typename TClient>
ATL_NOINLINE inline HRESULT CWsAttributesT<TClient>::GetClientReader(ISAXXMLReader **ppReader)
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

} // namespace WsAttributes

template<>
inline HRESULT AtlCleanupValue<WsAttributes::EspException>(WsAttributes::EspException *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Code);
	AtlCleanupValue(&pVal->Audience);
	AtlCleanupValue(&pVal->Source);
	AtlCleanupValue(&pVal->Message);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsAttributes::EspException>(WsAttributes::EspException *pVal, IAtlMemMgr *pMemMgr)
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
inline HRESULT AtlCleanupValue<WsAttributes::ArrayOfEspException>(WsAttributes::ArrayOfEspException *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Source);
	AtlCleanupArray(pVal->Exception, pVal->__Exception_nSizeIs);	
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsAttributes::ArrayOfEspException>(WsAttributes::ArrayOfEspException *pVal, IAtlMemMgr *pMemMgr)
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

template<>
inline HRESULT AtlCleanupValue<WsAttributes::SaveAttributeRequest>(WsAttributes::SaveAttributeRequest *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->ModuleName);
	AtlCleanupValue(&pVal->AttributeName);
	AtlCleanupValue(&pVal->Flags);
	AtlCleanupValue(&pVal->ResultType);
	AtlCleanupValue(&pVal->Description);
	AtlCleanupValue(&pVal->Text);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsAttributes::SaveAttributeRequest>(WsAttributes::SaveAttributeRequest *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->ModuleName, pMemMgr);
	AtlCleanupValueEx(&pVal->AttributeName, pMemMgr);
	AtlCleanupValueEx(&pVal->Flags, pMemMgr);
	AtlCleanupValueEx(&pVal->ResultType, pMemMgr);
	AtlCleanupValueEx(&pVal->Description, pMemMgr);
	AtlCleanupValueEx(&pVal->Text, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsAttributes::CheckinAttributeRequest>(WsAttributes::CheckinAttributeRequest *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->ModuleName);
	AtlCleanupValue(&pVal->AttributeName);
	AtlCleanupValue(&pVal->Description);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsAttributes::CheckinAttributeRequest>(WsAttributes::CheckinAttributeRequest *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->ModuleName, pMemMgr);
	AtlCleanupValueEx(&pVal->AttributeName, pMemMgr);
	AtlCleanupValueEx(&pVal->Description, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsAttributes::CheckoutAttributeRequest>(WsAttributes::CheckoutAttributeRequest *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->ModuleName);
	AtlCleanupValue(&pVal->AttributeName);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsAttributes::CheckoutAttributeRequest>(WsAttributes::CheckoutAttributeRequest *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->ModuleName, pMemMgr);
	AtlCleanupValueEx(&pVal->AttributeName, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsAttributes::RenameAttributeRequest>(WsAttributes::RenameAttributeRequest *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->ModuleName);
	AtlCleanupValue(&pVal->AttributeName);
	AtlCleanupValue(&pVal->NewModuleName);
	AtlCleanupValue(&pVal->NewAttributeName);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsAttributes::RenameAttributeRequest>(WsAttributes::RenameAttributeRequest *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->ModuleName, pMemMgr);
	AtlCleanupValueEx(&pVal->AttributeName, pMemMgr);
	AtlCleanupValueEx(&pVal->NewModuleName, pMemMgr);
	AtlCleanupValueEx(&pVal->NewAttributeName, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsAttributes::RollbackAttributeRequest>(WsAttributes::RollbackAttributeRequest *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->ModuleName);
	AtlCleanupValue(&pVal->AttributeName);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsAttributes::RollbackAttributeRequest>(WsAttributes::RollbackAttributeRequest *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->ModuleName, pMemMgr);
	AtlCleanupValueEx(&pVal->AttributeName, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsAttributes::ECLLabel>(WsAttributes::ECLLabel *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Name);
	AtlCleanupValue(&pVal->ModifiedDate);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsAttributes::ECLLabel>(WsAttributes::ECLLabel *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Name, pMemMgr);
	AtlCleanupValueEx(&pVal->ModifiedDate, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsAttributes::ImportAttributeRequest>(WsAttributes::ImportAttributeRequest *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->ModuleName);
	AtlCleanupValue(&pVal->AttributeName);
	AtlCleanupValue(&pVal->Type);
	AtlCleanupValue(&pVal->Flags);
	AtlCleanupValue(&pVal->ResultType);
	AtlCleanupValue(&pVal->Description);
	AtlCleanupValue(&pVal->Text);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsAttributes::ImportAttributeRequest>(WsAttributes::ImportAttributeRequest *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->ModuleName, pMemMgr);
	AtlCleanupValueEx(&pVal->AttributeName, pMemMgr);
	AtlCleanupValueEx(&pVal->Type, pMemMgr);
	AtlCleanupValueEx(&pVal->Flags, pMemMgr);
	AtlCleanupValueEx(&pVal->ResultType, pMemMgr);
	AtlCleanupValueEx(&pVal->Description, pMemMgr);
	AtlCleanupValueEx(&pVal->Text, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsAttributes::DeleteAttributeRequest>(WsAttributes::DeleteAttributeRequest *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->ModuleName);
	AtlCleanupValue(&pVal->AttributeName);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsAttributes::DeleteAttributeRequest>(WsAttributes::DeleteAttributeRequest *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->ModuleName, pMemMgr);
	AtlCleanupValueEx(&pVal->AttributeName, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsAttributes::ECLModule>(WsAttributes::ECLModule *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Name);
	AtlCleanupValue(&pVal->Flags);
	AtlCleanupValue(&pVal->Access);
	AtlCleanupValue(&pVal->Timestamp);
	AtlCleanupValue(&pVal->IsPlugin);
	AtlCleanupValue(&pVal->Rank);
	AtlCleanupValue(&pVal->FileName);
	AtlCleanupValue(&pVal->FileCRC);
	AtlCleanupValue(&pVal->Version);
	AtlCleanupValue(&pVal->Checksum);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsAttributes::ECLModule>(WsAttributes::ECLModule *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Name, pMemMgr);
	AtlCleanupValueEx(&pVal->Flags, pMemMgr);
	AtlCleanupValueEx(&pVal->Access, pMemMgr);
	AtlCleanupValueEx(&pVal->Timestamp, pMemMgr);
	AtlCleanupValueEx(&pVal->IsPlugin, pMemMgr);
	AtlCleanupValueEx(&pVal->Rank, pMemMgr);
	AtlCleanupValueEx(&pVal->FileName, pMemMgr);
	AtlCleanupValueEx(&pVal->FileCRC, pMemMgr);
	AtlCleanupValueEx(&pVal->Version, pMemMgr);
	AtlCleanupValueEx(&pVal->Checksum, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsAttributes::ECLAttributeLabel>(WsAttributes::ECLAttributeLabel *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->ModuleName);
	AtlCleanupValue(&pVal->AttributeName);
	AtlCleanupValue(&pVal->AttributeVersion);
	AtlCleanupValue(&pVal->LabelName);
	AtlCleanupValue(&pVal->ModifiedDate);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsAttributes::ECLAttributeLabel>(WsAttributes::ECLAttributeLabel *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->ModuleName, pMemMgr);
	AtlCleanupValueEx(&pVal->AttributeName, pMemMgr);
	AtlCleanupValueEx(&pVal->AttributeVersion, pMemMgr);
	AtlCleanupValueEx(&pVal->LabelName, pMemMgr);
	AtlCleanupValueEx(&pVal->ModifiedDate, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsAttributes::AttributeType>(WsAttributes::AttributeType *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->ID);
	AtlCleanupValue(&pVal->Name);
	AtlCleanupValue(&pVal->Description);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsAttributes::AttributeType>(WsAttributes::AttributeType *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->ID, pMemMgr);
	AtlCleanupValueEx(&pVal->Name, pMemMgr);
	AtlCleanupValueEx(&pVal->Description, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsAttributes::ECLTextLine>(WsAttributes::ECLTextLine *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->LineNumber);
	AtlCleanupValue(&pVal->LineText);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsAttributes::ECLTextLine>(WsAttributes::ECLTextLine *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->LineNumber, pMemMgr);
	AtlCleanupValueEx(&pVal->LineText, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsAttributes::ECLAttribute>(WsAttributes::ECLAttribute *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->ModuleName);
	AtlCleanupValue(&pVal->Name);
	AtlCleanupValue(&pVal->Type);
	AtlCleanupValue(&pVal->Version);
	AtlCleanupValue(&pVal->LatestVersion);
	AtlCleanupValue(&pVal->SandboxVersion);
	AtlCleanupValue(&pVal->Flags);
	AtlCleanupValue(&pVal->Access);
	AtlCleanupValue(&pVal->IsLocked);
	AtlCleanupValue(&pVal->IsCheckedOut);
	AtlCleanupValue(&pVal->IsSandbox);
	AtlCleanupValue(&pVal->IsOrphaned);
	AtlCleanupValue(&pVal->ResultType);
	AtlCleanupValue(&pVal->LockedBy);
	AtlCleanupValue(&pVal->ModifiedBy);
	AtlCleanupValue(&pVal->ModifiedDate);
	AtlCleanupValue(&pVal->Description);
	AtlCleanupValue(&pVal->Checksum);
	AtlCleanupValue(&pVal->Text);
	AtlCleanupArray(pVal->MatchedLines, pVal->__MatchedLines_nSizeIs);	
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsAttributes::ECLAttribute>(WsAttributes::ECLAttribute *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->ModuleName, pMemMgr);
	AtlCleanupValueEx(&pVal->Name, pMemMgr);
	AtlCleanupValueEx(&pVal->Type, pMemMgr);
	AtlCleanupValueEx(&pVal->Version, pMemMgr);
	AtlCleanupValueEx(&pVal->LatestVersion, pMemMgr);
	AtlCleanupValueEx(&pVal->SandboxVersion, pMemMgr);
	AtlCleanupValueEx(&pVal->Flags, pMemMgr);
	AtlCleanupValueEx(&pVal->Access, pMemMgr);
	AtlCleanupValueEx(&pVal->IsLocked, pMemMgr);
	AtlCleanupValueEx(&pVal->IsCheckedOut, pMemMgr);
	AtlCleanupValueEx(&pVal->IsSandbox, pMemMgr);
	AtlCleanupValueEx(&pVal->IsOrphaned, pMemMgr);
	AtlCleanupValueEx(&pVal->ResultType, pMemMgr);
	AtlCleanupValueEx(&pVal->LockedBy, pMemMgr);
	AtlCleanupValueEx(&pVal->ModifiedBy, pMemMgr);
	AtlCleanupValueEx(&pVal->ModifiedDate, pMemMgr);
	AtlCleanupValueEx(&pVal->Description, pMemMgr);
	AtlCleanupValueEx(&pVal->Checksum, pMemMgr);
	AtlCleanupValueEx(&pVal->Text, pMemMgr);
	if (pVal->MatchedLines != NULL)
	{
		AtlCleanupArrayEx(pVal->MatchedLines, pVal->__MatchedLines_nSizeIs, pMemMgr);		
		pMemMgr->Free(pVal->MatchedLines);
	}
	return S_OK;
}
