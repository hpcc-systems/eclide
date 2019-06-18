//
// sproxy.exe generated file
// do not modify this file
//
// Created: 10/28/2009@09:14:3
//

#pragma once


#if !defined(_WIN32_WINDOWS) && !defined(_WIN32_WINNT) && !defined(_WIN32_WCE)
#pragma message("warning: defining _WIN32_WINDOWS = 0x0410")
#define _WIN32_WINDOWS 0x0410
#endif

#include <atlsoap.h>

namespace WsDfu
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

struct DFUFileStat
{
	BSTR MinSkew;
	BSTR MaxSkew;
};

struct DFULogicalFile
{
	BSTR Prefix;
	BSTR ClusterName;
	BSTR Directory;
	BSTR Description;
	BSTR Parts;
	BSTR Name;
	BSTR Owner;
	BSTR Totalsize;
	BSTR RecordCount;
	BSTR Modified;
	BSTR LongSize;
	BSTR LongRecordCount;
	bool isSuperfile;
	bool isZipfile;
	bool isDirectory;
	bool Replicate;
	__int64 IntSize;
	__int64 IntRecordCount;
	bool FromRoxieCluster;
	bool BrowseData;
	bool IsKeyFile;
};

struct DFUSpaceItem
{
	BSTR Name;
	BSTR NumOfFiles;
	BSTR NumOfFilesUnknown;
	BSTR TotalSize;
	BSTR LargestFile;
	BSTR LargestSize;
	BSTR SmallestFile;
	BSTR SmallestSize;
};

struct DFUPart
{
	int Id;
	int Copy;
	BSTR ActualSize;
	BSTR Ip;
	BSTR Partsize;
};

struct DFUDataColumn
{
	BSTR ColumnLabel;
	BSTR ColumnType;
	BSTR ColumnValue;
	int ColumnSize;
	int MaxSize;
};

struct DFUFileDetail
{
	BSTR Name;
	BSTR Filename;
	BSTR Description;
	BSTR Dir;
	BSTR PathMask;
	BSTR Filesize;
	BSTR ActualSize;
	BSTR RecordSize;
	BSTR RecordCount;
	BSTR Wuid;
	BSTR Owner;
	BSTR Cluster;
	BSTR JobName;
	BSTR Persistent;
	BSTR Format;
	BSTR MaxRecordSize;
	BSTR CsvSeparate;
	BSTR CsvQuote;
	BSTR CsvTerminate;
	BSTR Modified;
	BSTR Ecl;
	bool ZipFile;
	DFUFileStat Stat;
	DFUPart *DFUFileParts;
	int __DFUFileParts_nSizeIs;
	bool isSuperfile;
	bool ShowFileContent;
	BSTR *subfiles;
	int __subfiles_nSizeIs;
	DFULogicalFile *Superfiles;
	int __Superfiles_nSizeIs;
	bool FromRoxieCluster;
	BSTR *Graphs;
	int __Graphs_nSizeIs;
	BSTR UserPermission;
};

template <typename TClient = CSoapSocketClientT<> >
class CWsDfuT : 
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

	CWsDfuT(ISAXXMLReader *pReader = NULL)
		:TClient(_T("http://10.173.88.201:8010/WsDfu\?ver_=1.16"))
	{
		SetClient(true);
		SetReader(pReader);
	}
	
	~CWsDfuT()
	{
		Uninitialize();
	}
	
	void Uninitialize()
	{
		UninitializeSOAP();
	}	

	HRESULT DFUSearchData(
		BSTR* Cluster, 
		BSTR* ClusterType, 
		BSTR* OpenLogicalName, 
		BSTR* FilterBy, 
		BSTR* ShowColumns, 
		int* ChooseFile, 
		__int64* StartIndex, 
		__int64* EndIndex, 
		BSTR* LogicalName, 
		BSTR* ParentName, 
		__int64* StartForGoback, 
		int* CountForGoback, 
		__int64* Start, 
		int* Count, 
		BSTR* File, 
		BSTR* Key, 
		bool* SchemaOnly, 
		ArrayOfEspException* Exceptions, 
		DFUDataColumn** DFUDataKeyedColumns1, int* DFUDataKeyedColumns1_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns2, int* DFUDataKeyedColumns2_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns3, int* DFUDataKeyedColumns3_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns4, int* DFUDataKeyedColumns4_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns5, int* DFUDataKeyedColumns5_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns6, int* DFUDataKeyedColumns6_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns7, int* DFUDataKeyedColumns7_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns8, int* DFUDataKeyedColumns8_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns9, int* DFUDataKeyedColumns9_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns10, int* DFUDataKeyedColumns10_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns11, int* DFUDataKeyedColumns11_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns12, int* DFUDataKeyedColumns12_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns13, int* DFUDataKeyedColumns13_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns14, int* DFUDataKeyedColumns14_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns15, int* DFUDataKeyedColumns15_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns16, int* DFUDataKeyedColumns16_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns17, int* DFUDataKeyedColumns17_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns18, int* DFUDataKeyedColumns18_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns19, int* DFUDataKeyedColumns19_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns20, int* DFUDataKeyedColumns20_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns1, int* DFUDataNonKeyedColumns1_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns2, int* DFUDataNonKeyedColumns2_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns3, int* DFUDataNonKeyedColumns3_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns4, int* DFUDataNonKeyedColumns4_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns5, int* DFUDataNonKeyedColumns5_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns6, int* DFUDataNonKeyedColumns6_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns7, int* DFUDataNonKeyedColumns7_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns8, int* DFUDataNonKeyedColumns8_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns9, int* DFUDataNonKeyedColumns9_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns10, int* DFUDataNonKeyedColumns10_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns11, int* DFUDataNonKeyedColumns11_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns12, int* DFUDataNonKeyedColumns12_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns13, int* DFUDataNonKeyedColumns13_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns14, int* DFUDataNonKeyedColumns14_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns15, int* DFUDataNonKeyedColumns15_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns16, int* DFUDataNonKeyedColumns16_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns17, int* DFUDataNonKeyedColumns17_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns18, int* DFUDataNonKeyedColumns18_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns19, int* DFUDataNonKeyedColumns19_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns20, int* DFUDataNonKeyedColumns20_nSizeIs, 
		__int64* RowCount, 
		BSTR* Name, 
		BSTR* FilterForGoBack, 
		DFUDataColumn** ColumnsHidden, int* ColumnsHidden_nSizeIs, 
		int* ColumnCount, 
		int* PageSize, 
		__int64* Total, 
		BSTR* Result, 
		BSTR* MsgToDisplay
	);

	HRESULT AddRemote(
		BSTR dstname, 
		BSTR srcname, 
		BSTR srcdali, 
		BSTR srcusername, 
		BSTR srcpassword, 
		ArrayOfEspException* Exceptions
	);

	HRESULT SuperfileList(
		BSTR* superfile, 
		ArrayOfEspException* Exceptions, 
		BSTR** subfiles, int* subfiles_nSizeIs
	);

	HRESULT DFUSearch(
		BSTR* ShowExample, 
		ArrayOfEspException* Exceptions, 
		BSTR** ClusterNames, int* ClusterNames_nSizeIs, 
		BSTR** FileTypes, int* FileTypes_nSizeIs
	);

	HRESULT DFUFileView(
		BSTR* Scope, 
		ArrayOfEspException* Exceptions, 
		int* NumFiles, 
		DFULogicalFile** DFULogicalFiles, int* DFULogicalFiles_nSizeIs
	);

	HRESULT DFUSpace(
		BSTR* CountBy, 
		BSTR* ScopeUnder, 
		BSTR* OwnerUnder, 
		BSTR* Interval, 
		BSTR* StartDate, 
		BSTR* EndDate, 
		ArrayOfEspException* Exceptions, 
		DFUSpaceItem** DFUSpaceItems, int* DFUSpaceItems_nSizeIs
	);

	HRESULT DFUInfo(
		BSTR Name, 
		BSTR Cluster, 
		bool UpdateDescription, 
		BSTR FileName, 
		BSTR FileDesc, 
		ArrayOfEspException* Exceptions, 
		DFUFileDetail* FileDetail
	);

	HRESULT DFUQuery(
		BSTR* Prefix, 
		BSTR* ClusterName, 
		BSTR* LogicalName, 
		BSTR* Description, 
		BSTR* Owner, 
		BSTR* StartDate, 
		BSTR* EndDate, 
		BSTR* FileType, 
		__int64* FileSizeFrom, 
		__int64* FileSizeTo, 
		int* FirstN, 
		BSTR* FirstNType, 
		int* PageSize, 
		int* PageStartFrom, 
		BSTR* Sortby, 
		bool* Descending, 
		bool OneLevelDirFileReturn, 
		ArrayOfEspException* Exceptions, 
		DFULogicalFile** DFULogicalFiles, int* DFULogicalFiles_nSizeIs, 
		int* LastPageFrom, 
		int* PageEndAt, 
		int* PrevPageFrom, 
		int* NextPageFrom, 
		int* NumFiles, 
		BSTR* BasicQuery, 
		BSTR* ParametersForPaging, 
		BSTR* Filters
	);

	HRESULT DFUGetDataColumns(
		BSTR OpenLogicalName, 
		BSTR* LogicalName, 
		BSTR FilterBy, 
		BSTR* ShowColumns, 
		int* ChooseFile, 
		BSTR* Cluster, 
		BSTR* ClusterType, 
		__int64* StartIndex, 
		__int64* EndIndex, 
		ArrayOfEspException* Exceptions, 
		DFUDataColumn** DFUDataKeyedColumns1, int* DFUDataKeyedColumns1_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns2, int* DFUDataKeyedColumns2_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns3, int* DFUDataKeyedColumns3_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns4, int* DFUDataKeyedColumns4_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns5, int* DFUDataKeyedColumns5_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns6, int* DFUDataKeyedColumns6_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns7, int* DFUDataKeyedColumns7_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns8, int* DFUDataKeyedColumns8_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns9, int* DFUDataKeyedColumns9_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns10, int* DFUDataKeyedColumns10_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns11, int* DFUDataKeyedColumns11_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns12, int* DFUDataKeyedColumns12_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns13, int* DFUDataKeyedColumns13_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns14, int* DFUDataKeyedColumns14_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns15, int* DFUDataKeyedColumns15_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns16, int* DFUDataKeyedColumns16_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns17, int* DFUDataKeyedColumns17_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns18, int* DFUDataKeyedColumns18_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns19, int* DFUDataKeyedColumns19_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns20, int* DFUDataKeyedColumns20_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns1, int* DFUDataNonKeyedColumns1_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns2, int* DFUDataNonKeyedColumns2_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns3, int* DFUDataNonKeyedColumns3_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns4, int* DFUDataNonKeyedColumns4_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns5, int* DFUDataNonKeyedColumns5_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns6, int* DFUDataNonKeyedColumns6_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns7, int* DFUDataNonKeyedColumns7_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns8, int* DFUDataNonKeyedColumns8_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns9, int* DFUDataNonKeyedColumns9_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns10, int* DFUDataNonKeyedColumns10_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns11, int* DFUDataNonKeyedColumns11_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns12, int* DFUDataNonKeyedColumns12_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns13, int* DFUDataNonKeyedColumns13_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns14, int* DFUDataNonKeyedColumns14_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns15, int* DFUDataNonKeyedColumns15_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns16, int* DFUDataNonKeyedColumns16_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns17, int* DFUDataNonKeyedColumns17_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns18, int* DFUDataNonKeyedColumns18_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns19, int* DFUDataNonKeyedColumns19_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns20, int* DFUDataNonKeyedColumns20_nSizeIs, 
		__int64* RowCount
	);

	HRESULT DFUBrowseData(
		BSTR* LogicalName, 
		BSTR* FilterBy, 
		BSTR ShowColumns, 
		bool* SchemaOnly, 
		__int64* StartForGoback, 
		int* CountForGoback, 
		int* ChooseFile, 
		BSTR* Cluster, 
		BSTR* ClusterType, 
		BSTR* ParentName, 
		__int64* Start, 
		int* Count, 
		ArrayOfEspException* Exceptions, 
		BSTR* Name, 
		BSTR* FilterForGoBack, 
		DFUDataColumn** ColumnsHidden, int* ColumnsHidden_nSizeIs, 
		int* ColumnCount, 
		int* PageSize, 
		__int64* Total, 
		BSTR* Result, 
		BSTR* MsgToDisplay
	);

	HRESULT DFUDefFile(
		BSTR Name, 
		BSTR Format, 
		ArrayOfEspException* Exceptions, 
		ATLSOAP_BLOB* defFile
	);

	HRESULT DFUArrayAction(
		BSTR Type, 
		bool NoDelete, 
		BSTR* BackToPage, 
		BSTR* LogicalFiles, int LogicalFiles_nSizeIs, 
		ArrayOfEspException* Exceptions, 
		BSTR* DFUArrayActionResult
	);

	HRESULT AddtoSuperfile(
		BSTR Superfile, 
		BSTR* Subfiles, 
		BSTR* names, int names_nSizeIs, 
		bool ExistingFile, 
		ArrayOfEspException* Exceptions, 
		BSTR** SubfileNames, int* SubfileNames_nSizeIs
	);

	HRESULT Savexml(
		BSTR name, 
		ArrayOfEspException* Exceptions, 
		ATLSOAP_BLOB* xmlmap
	);

	HRESULT SuperfileAddRaw(
		BSTR* superfile, 
		BSTR subfiles, 
		BSTR before, 
		ArrayOfEspException* Exceptions, 
		int* retcode
	);

	HRESULT SuperfileAction(
		BSTR action, 
		BSTR* superfile, 
		BSTR* subfiles, int subfiles_nSizeIs, 
		BSTR before, 
		bool ___delete, 
		ArrayOfEspException* Exceptions, 
		int* retcode
	);

	HRESULT Add(
		BSTR dstname, 
		ATLSOAP_BLOB xmlmap, 
		ArrayOfEspException* Exceptions
	);
};

typedef CWsDfuT<> CWsDfu;

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
	0x18AC14AC,
	"http://webservices.seisint.com/WsDfu",
	L"http://webservices.seisint.com/WsDfu",
	sizeof("http://webservices.seisint.com/WsDfu")-1
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
	0x18AC14AC,
	"http://webservices.seisint.com/WsDfu",
	L"http://webservices.seisint.com/WsDfu",
	sizeof("http://webservices.seisint.com/WsDfu")-1
};
}

__if_not_exists(__DFUFileStat_entries)
{
extern __declspec(selectany) const _soapmapentry __DFUFileStat_entries[] =
{
	{ 
		0x2427CF9E, 
		"MinSkew", 
		L"MinSkew", 
		sizeof("MinSkew")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUFileStat, MinSkew),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x12337FA0, 
		"MaxSkew", 
		L"MaxSkew", 
		sizeof("MaxSkew")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUFileStat, MaxSkew),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __DFUFileStat_map =
{
	0x64B8C6FB,
	"DFUFileStat",
	L"DFUFileStat",
	sizeof("DFUFileStat")-1,
	sizeof("DFUFileStat")-1,
	SOAPMAP_STRUCT,
	__DFUFileStat_entries,
	sizeof(DFUFileStat),
	2,
	-1,
	SOAPFLAG_NONE,
	0x18AC14AC,
	"http://webservices.seisint.com/WsDfu",
	L"http://webservices.seisint.com/WsDfu",
	sizeof("http://webservices.seisint.com/WsDfu")-1
};
}

__if_not_exists(__DFULogicalFile_entries)
{
extern __declspec(selectany) const _soapmapentry __DFULogicalFile_entries[] =
{
	{ 
		0xC2E4BDEE, 
		"Prefix", 
		L"Prefix", 
		sizeof("Prefix")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFULogicalFile, Prefix),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xCF62D983, 
		"ClusterName", 
		L"ClusterName", 
		sizeof("ClusterName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFULogicalFile, ClusterName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x12576735, 
		"Directory", 
		L"Directory", 
		sizeof("Directory")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFULogicalFile, Directory),
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
		offsetof(DFULogicalFile, Description),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x05DECD6A, 
		"Parts", 
		L"Parts", 
		sizeof("Parts")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFULogicalFile, Parts),
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
		offsetof(DFULogicalFile, Name),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x05D8B24B, 
		"Owner", 
		L"Owner", 
		sizeof("Owner")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFULogicalFile, Owner),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x74EB831F, 
		"Totalsize", 
		L"Totalsize", 
		sizeof("Totalsize")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFULogicalFile, Totalsize),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x023686C8, 
		"RecordCount", 
		L"RecordCount", 
		sizeof("RecordCount")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFULogicalFile, RecordCount),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x613705E1, 
		"Modified", 
		L"Modified", 
		sizeof("Modified")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFULogicalFile, Modified),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x8C1A2DCB, 
		"LongSize", 
		L"LongSize", 
		sizeof("LongSize")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFULogicalFile, LongSize),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xF904C058, 
		"LongRecordCount", 
		L"LongRecordCount", 
		sizeof("LongRecordCount")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFULogicalFile, LongRecordCount),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xE390B8AB, 
		"isSuperfile", 
		L"isSuperfile", 
		sizeof("isSuperfile")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFULogicalFile, isSuperfile),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xF5A8746F, 
		"isZipfile", 
		L"isZipfile", 
		sizeof("isZipfile")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFULogicalFile, isZipfile),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x961CF0B1, 
		"isDirectory", 
		L"isDirectory", 
		sizeof("isDirectory")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFULogicalFile, isDirectory),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x36E0E9D9, 
		"Replicate", 
		L"Replicate", 
		sizeof("Replicate")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFULogicalFile, Replicate),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xFC5534E6, 
		"IntSize", 
		L"IntSize", 
		sizeof("IntSize")-1, 
		SOAPTYPE_LONG, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFULogicalFile, IntSize),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x30A35B13, 
		"IntRecordCount", 
		L"IntRecordCount", 
		sizeof("IntRecordCount")-1, 
		SOAPTYPE_LONG, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFULogicalFile, IntRecordCount),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x15B669FD, 
		"FromRoxieCluster", 
		L"FromRoxieCluster", 
		sizeof("FromRoxieCluster")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFULogicalFile, FromRoxieCluster),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x368D292C, 
		"BrowseData", 
		L"BrowseData", 
		sizeof("BrowseData")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFULogicalFile, BrowseData),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xDAAD72E5, 
		"IsKeyFile", 
		L"IsKeyFile", 
		sizeof("IsKeyFile")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFULogicalFile, IsKeyFile),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __DFULogicalFile_map =
{
	0x7DDAD71A,
	"DFULogicalFile",
	L"DFULogicalFile",
	sizeof("DFULogicalFile")-1,
	sizeof("DFULogicalFile")-1,
	SOAPMAP_STRUCT,
	__DFULogicalFile_entries,
	sizeof(DFULogicalFile),
	21,
	-1,
	SOAPFLAG_NONE,
	0x18AC14AC,
	"http://webservices.seisint.com/WsDfu",
	L"http://webservices.seisint.com/WsDfu",
	sizeof("http://webservices.seisint.com/WsDfu")-1
};
}

__if_not_exists(__DFUSpaceItem_entries)
{
extern __declspec(selectany) const _soapmapentry __DFUSpaceItem_entries[] =
{
	{ 
		0x002C70A1, 
		"Name", 
		L"Name", 
		sizeof("Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUSpaceItem, Name),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xC47490F8, 
		"NumOfFiles", 
		L"NumOfFiles", 
		sizeof("NumOfFiles")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUSpaceItem, NumOfFiles),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x38F4E2C8, 
		"NumOfFilesUnknown", 
		L"NumOfFilesUnknown", 
		sizeof("NumOfFilesUnknown")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUSpaceItem, NumOfFilesUnknown),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x74D9F6FF, 
		"TotalSize", 
		L"TotalSize", 
		sizeof("TotalSize")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUSpaceItem, TotalSize),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x80C44832, 
		"LargestFile", 
		L"LargestFile", 
		sizeof("LargestFile")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUSpaceItem, LargestFile),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x80CB6AED, 
		"LargestSize", 
		L"LargestSize", 
		sizeof("LargestSize")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUSpaceItem, LargestSize),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x72FD6E65, 
		"SmallestFile", 
		L"SmallestFile", 
		sizeof("SmallestFile")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUSpaceItem, SmallestFile),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x73049120, 
		"SmallestSize", 
		L"SmallestSize", 
		sizeof("SmallestSize")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUSpaceItem, SmallestSize),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __DFUSpaceItem_map =
{
	0x09B3BC1A,
	"DFUSpaceItem",
	L"DFUSpaceItem",
	sizeof("DFUSpaceItem")-1,
	sizeof("DFUSpaceItem")-1,
	SOAPMAP_STRUCT,
	__DFUSpaceItem_entries,
	sizeof(DFUSpaceItem),
	8,
	-1,
	SOAPFLAG_NONE,
	0x18AC14AC,
	"http://webservices.seisint.com/WsDfu",
	L"http://webservices.seisint.com/WsDfu",
	sizeof("http://webservices.seisint.com/WsDfu")-1
};
}

__if_not_exists(__DFUPart_entries)
{
extern __declspec(selectany) const _soapmapentry __DFUPart_entries[] =
{
	{ 
		0x000009CD, 
		"Id", 
		L"Id", 
		sizeof("Id")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUPart, Id),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0026A47B, 
		"Copy", 
		L"Copy", 
		sizeof("Copy")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUPart, Copy),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x3916D495, 
		"ActualSize", 
		L"ActualSize", 
		sizeof("ActualSize")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUPart, ActualSize),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x000009D9, 
		"Ip", 
		L"Ip", 
		sizeof("Ip")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUPart, Ip),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x11C39BF2, 
		"Partsize", 
		L"Partsize", 
		sizeof("Partsize")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUPart, Partsize),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __DFUPart_map =
{
	0x1BF0EC36,
	"DFUPart",
	L"DFUPart",
	sizeof("DFUPart")-1,
	sizeof("DFUPart")-1,
	SOAPMAP_STRUCT,
	__DFUPart_entries,
	sizeof(DFUPart),
	5,
	-1,
	SOAPFLAG_NONE,
	0x18AC14AC,
	"http://webservices.seisint.com/WsDfu",
	L"http://webservices.seisint.com/WsDfu",
	sizeof("http://webservices.seisint.com/WsDfu")-1
};
}

__if_not_exists(__DFUDataColumn_entries)
{
extern __declspec(selectany) const _soapmapentry __DFUDataColumn_entries[] =
{
	{ 
		0x9327CD4E, 
		"ColumnLabel", 
		L"ColumnLabel", 
		sizeof("ColumnLabel")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUDataColumn, ColumnLabel),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x71158390, 
		"ColumnType", 
		L"ColumnType", 
		sizeof("ColumnType")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUDataColumn, ColumnType),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x93DCEEEB, 
		"ColumnValue", 
		L"ColumnValue", 
		sizeof("ColumnValue")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUDataColumn, ColumnValue),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x7114B469, 
		"ColumnSize", 
		L"ColumnSize", 
		sizeof("ColumnSize")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUDataColumn, ColumnSize),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x123379C1, 
		"MaxSize", 
		L"MaxSize", 
		sizeof("MaxSize")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUDataColumn, MaxSize),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __DFUDataColumn_map =
{
	0xD28C91C7,
	"DFUDataColumn",
	L"DFUDataColumn",
	sizeof("DFUDataColumn")-1,
	sizeof("DFUDataColumn")-1,
	SOAPMAP_STRUCT,
	__DFUDataColumn_entries,
	sizeof(DFUDataColumn),
	5,
	-1,
	SOAPFLAG_NONE,
	0x18AC14AC,
	"http://webservices.seisint.com/WsDfu",
	L"http://webservices.seisint.com/WsDfu",
	sizeof("http://webservices.seisint.com/WsDfu")-1
};
}

__if_not_exists(__DFUFileDetail_entries)
{
extern __declspec(selectany) const _soapmapentry __DFUFileDetail_entries[] =
{
	{ 
		0x002C70A1, 
		"Name", 
		L"Name", 
		sizeof("Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUFileDetail, Name),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x2FF1EC41, 
		"Filename", 
		L"Filename", 
		sizeof("Filename")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUFileDetail, Filename),
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
		offsetof(DFUFileDetail, Description),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x00012F3F, 
		"Dir", 
		L"Dir", 
		sizeof("Dir")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUFileDetail, Dir),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x157FCDD9, 
		"PathMask", 
		L"PathMask", 
		sizeof("PathMask")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUFileDetail, PathMask),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x2FF4CDDB, 
		"Filesize", 
		L"Filesize", 
		sizeof("Filesize")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUFileDetail, Filesize),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x3916D495, 
		"ActualSize", 
		L"ActualSize", 
		sizeof("ActualSize")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUFileDetail, ActualSize),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0F9DB8FA, 
		"RecordSize", 
		L"RecordSize", 
		sizeof("RecordSize")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUFileDetail, RecordSize),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x023686C8, 
		"RecordCount", 
		L"RecordCount", 
		sizeof("RecordCount")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUFileDetail, RecordCount),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0031B499, 
		"Wuid", 
		L"Wuid", 
		sizeof("Wuid")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUFileDetail, Wuid),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x05D8B24B, 
		"Owner", 
		L"Owner", 
		sizeof("Owner")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUFileDetail, Owner),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x29F147E2, 
		"Cluster", 
		L"Cluster", 
		sizeof("Cluster")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUFileDetail, Cluster),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x4A5BFF9C, 
		"JobName", 
		L"JobName", 
		sizeof("JobName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUFileDetail, JobName),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xFF700AD1, 
		"Persistent", 
		L"Persistent", 
		sizeof("Persistent")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUFileDetail, Persistent),
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
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUFileDetail, Format),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xFEDEED00, 
		"MaxRecordSize", 
		L"MaxRecordSize", 
		sizeof("MaxRecordSize")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUFileDetail, MaxRecordSize),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x6DCDF3E1, 
		"CsvSeparate", 
		L"CsvSeparate", 
		sizeof("CsvSeparate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUFileDetail, CsvSeparate),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x82E08C7A, 
		"CsvQuote", 
		L"CsvQuote", 
		sizeof("CsvQuote")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUFileDetail, CsvQuote),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x515F37F5, 
		"CsvTerminate", 
		L"CsvTerminate", 
		sizeof("CsvTerminate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUFileDetail, CsvTerminate),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x613705E1, 
		"Modified", 
		L"Modified", 
		sizeof("Modified")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUFileDetail, Modified),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x000132B4, 
		"Ecl", 
		L"Ecl", 
		sizeof("Ecl")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUFileDetail, Ecl),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x0CF9EDD3, 
		"ZipFile", 
		L"ZipFile", 
		sizeof("ZipFile")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUFileDetail, ZipFile),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x002F7DDC, 
		"Stat", 
		L"Stat", 
		sizeof("Stat")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUFileDetail, Stat),
		NULL, 
		&__DFUFileStat_map, 
		-1 
	},
	{ 
		0xFB913A69, 
		"DFUFileParts", 
		L"DFUFileParts", 
		sizeof("DFUFileParts")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(DFUFileDetail, DFUFileParts),
		NULL, 
		&__DFUPart_map, 
		23+1 
	},
	{ 
		0xFB913A69, 
		"DFUFileParts", 
		L"DFUFileParts", 
		sizeof("DFUFileParts")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(DFUFileDetail, __DFUFileParts_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 
		0xE390B8AB, 
		"isSuperfile", 
		L"isSuperfile", 
		sizeof("isSuperfile")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUFileDetail, isSuperfile),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xD82FA97C, 
		"ShowFileContent", 
		L"ShowFileContent", 
		sizeof("ShowFileContent")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUFileDetail, ShowFileContent),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0x3BFB22FD, 
		"subfiles", 
		L"subfiles", 
		sizeof("subfiles")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(DFUFileDetail, subfiles),
		NULL, 
		NULL, 
		27+1 
	},
	{ 
		0x3BFB22FD, 
		"subfiles", 
		L"subfiles", 
		sizeof("subfiles")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(DFUFileDetail, __subfiles_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 
		0x59311582, 
		"Superfiles", 
		L"Superfiles", 
		sizeof("Superfiles")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(DFUFileDetail, Superfiles),
		NULL, 
		&__DFULogicalFile_map, 
		29+1 
	},
	{ 
		0x59311582, 
		"Superfiles", 
		L"Superfiles", 
		sizeof("Superfiles")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(DFUFileDetail, __Superfiles_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 
		0x15B669FD, 
		"FromRoxieCluster", 
		L"FromRoxieCluster", 
		sizeof("FromRoxieCluster")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUFileDetail, FromRoxieCluster),
		NULL, 
		NULL, 
		-1 
	},
	{ 
		0xADE44925, 
		"Graphs", 
		L"Graphs", 
		sizeof("Graphs")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_DYNARR | SOAPFLAG_NULLABLE, 
		offsetof(DFUFileDetail, Graphs),
		NULL, 
		NULL, 
		32+1 
	},
	{ 
		0xADE44925, 
		"Graphs", 
		L"Graphs", 
		sizeof("Graphs")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_FIELD | SOAPFLAG_NOMARSHAL, 
		offsetof(DFUFileDetail, __Graphs_nSizeIs),
		NULL, 
		NULL, 
		-1
	},
	{ 
		0x38E318A8, 
		"UserPermission", 
		L"UserPermission", 
		sizeof("UserPermission")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_FIELD | SOAPFLAG_NULLABLE, 
		offsetof(DFUFileDetail, UserPermission),
		NULL, 
		NULL, 
		-1 
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __DFUFileDetail_map =
{
	0x5203C912,
	"DFUFileDetail",
	L"DFUFileDetail",
	sizeof("DFUFileDetail")-1,
	sizeof("DFUFileDetail")-1,
	SOAPMAP_STRUCT,
	__DFUFileDetail_entries,
	sizeof(DFUFileDetail),
	31,
	-1,
	SOAPFLAG_NONE,
	0x18AC14AC,
	"http://webservices.seisint.com/WsDfu",
	L"http://webservices.seisint.com/WsDfu",
	sizeof("http://webservices.seisint.com/WsDfu")-1
};
}

struct __CWsDfu_DFUSearchData_struct
{
	BSTR Cluster;
	BSTR ClusterType;
	BSTR OpenLogicalName;
	BSTR FilterBy;
	BSTR ShowColumns;
	int ChooseFile;
	__int64 StartIndex;
	__int64 EndIndex;
	BSTR LogicalName;
	BSTR ParentName;
	__int64 StartForGoback;
	int CountForGoback;
	__int64 Start;
	int Count;
	BSTR File;
	BSTR Key;
	bool SchemaOnly;
	ArrayOfEspException Exceptions;
	DFUDataColumn *DFUDataKeyedColumns1;
	int __DFUDataKeyedColumns1_nSizeIs;
	DFUDataColumn *DFUDataKeyedColumns2;
	int __DFUDataKeyedColumns2_nSizeIs;
	DFUDataColumn *DFUDataKeyedColumns3;
	int __DFUDataKeyedColumns3_nSizeIs;
	DFUDataColumn *DFUDataKeyedColumns4;
	int __DFUDataKeyedColumns4_nSizeIs;
	DFUDataColumn *DFUDataKeyedColumns5;
	int __DFUDataKeyedColumns5_nSizeIs;
	DFUDataColumn *DFUDataKeyedColumns6;
	int __DFUDataKeyedColumns6_nSizeIs;
	DFUDataColumn *DFUDataKeyedColumns7;
	int __DFUDataKeyedColumns7_nSizeIs;
	DFUDataColumn *DFUDataKeyedColumns8;
	int __DFUDataKeyedColumns8_nSizeIs;
	DFUDataColumn *DFUDataKeyedColumns9;
	int __DFUDataKeyedColumns9_nSizeIs;
	DFUDataColumn *DFUDataKeyedColumns10;
	int __DFUDataKeyedColumns10_nSizeIs;
	DFUDataColumn *DFUDataKeyedColumns11;
	int __DFUDataKeyedColumns11_nSizeIs;
	DFUDataColumn *DFUDataKeyedColumns12;
	int __DFUDataKeyedColumns12_nSizeIs;
	DFUDataColumn *DFUDataKeyedColumns13;
	int __DFUDataKeyedColumns13_nSizeIs;
	DFUDataColumn *DFUDataKeyedColumns14;
	int __DFUDataKeyedColumns14_nSizeIs;
	DFUDataColumn *DFUDataKeyedColumns15;
	int __DFUDataKeyedColumns15_nSizeIs;
	DFUDataColumn *DFUDataKeyedColumns16;
	int __DFUDataKeyedColumns16_nSizeIs;
	DFUDataColumn *DFUDataKeyedColumns17;
	int __DFUDataKeyedColumns17_nSizeIs;
	DFUDataColumn *DFUDataKeyedColumns18;
	int __DFUDataKeyedColumns18_nSizeIs;
	DFUDataColumn *DFUDataKeyedColumns19;
	int __DFUDataKeyedColumns19_nSizeIs;
	DFUDataColumn *DFUDataKeyedColumns20;
	int __DFUDataKeyedColumns20_nSizeIs;
	DFUDataColumn *DFUDataNonKeyedColumns1;
	int __DFUDataNonKeyedColumns1_nSizeIs;
	DFUDataColumn *DFUDataNonKeyedColumns2;
	int __DFUDataNonKeyedColumns2_nSizeIs;
	DFUDataColumn *DFUDataNonKeyedColumns3;
	int __DFUDataNonKeyedColumns3_nSizeIs;
	DFUDataColumn *DFUDataNonKeyedColumns4;
	int __DFUDataNonKeyedColumns4_nSizeIs;
	DFUDataColumn *DFUDataNonKeyedColumns5;
	int __DFUDataNonKeyedColumns5_nSizeIs;
	DFUDataColumn *DFUDataNonKeyedColumns6;
	int __DFUDataNonKeyedColumns6_nSizeIs;
	DFUDataColumn *DFUDataNonKeyedColumns7;
	int __DFUDataNonKeyedColumns7_nSizeIs;
	DFUDataColumn *DFUDataNonKeyedColumns8;
	int __DFUDataNonKeyedColumns8_nSizeIs;
	DFUDataColumn *DFUDataNonKeyedColumns9;
	int __DFUDataNonKeyedColumns9_nSizeIs;
	DFUDataColumn *DFUDataNonKeyedColumns10;
	int __DFUDataNonKeyedColumns10_nSizeIs;
	DFUDataColumn *DFUDataNonKeyedColumns11;
	int __DFUDataNonKeyedColumns11_nSizeIs;
	DFUDataColumn *DFUDataNonKeyedColumns12;
	int __DFUDataNonKeyedColumns12_nSizeIs;
	DFUDataColumn *DFUDataNonKeyedColumns13;
	int __DFUDataNonKeyedColumns13_nSizeIs;
	DFUDataColumn *DFUDataNonKeyedColumns14;
	int __DFUDataNonKeyedColumns14_nSizeIs;
	DFUDataColumn *DFUDataNonKeyedColumns15;
	int __DFUDataNonKeyedColumns15_nSizeIs;
	DFUDataColumn *DFUDataNonKeyedColumns16;
	int __DFUDataNonKeyedColumns16_nSizeIs;
	DFUDataColumn *DFUDataNonKeyedColumns17;
	int __DFUDataNonKeyedColumns17_nSizeIs;
	DFUDataColumn *DFUDataNonKeyedColumns18;
	int __DFUDataNonKeyedColumns18_nSizeIs;
	DFUDataColumn *DFUDataNonKeyedColumns19;
	int __DFUDataNonKeyedColumns19_nSizeIs;
	DFUDataColumn *DFUDataNonKeyedColumns20;
	int __DFUDataNonKeyedColumns20_nSizeIs;
	__int64 RowCount;
	BSTR Name;
	BSTR FilterForGoBack;
	DFUDataColumn *ColumnsHidden;
	int __ColumnsHidden_nSizeIs;
	int ColumnCount;
	int PageSize;
	__int64 Total;
	BSTR Result;
	BSTR MsgToDisplay;
};

extern __declspec(selectany) const _soapmapentry __CWsDfu_DFUSearchData_entries[] =
{

	{
		0x29F147E2, 
		"Cluster", 
		L"Cluster", 
		sizeof("Cluster")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, Cluster),
		NULL,
		NULL,
		-1,
	},
	{
		0xCF668A44, 
		"ClusterType", 
		L"ClusterType", 
		sizeof("ClusterType")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, ClusterType),
		NULL,
		NULL,
		-1,
	},
	{
		0x743A238E, 
		"OpenLogicalName", 
		L"OpenLogicalName", 
		sizeof("OpenLogicalName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, OpenLogicalName),
		NULL,
		NULL,
		-1,
	},
	{
		0x30FCAF41, 
		"FilterBy", 
		L"FilterBy", 
		sizeof("FilterBy")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, FilterBy),
		NULL,
		NULL,
		-1,
	},
	{
		0x66378EE2, 
		"ShowColumns", 
		L"ShowColumns", 
		sizeof("ShowColumns")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, ShowColumns),
		NULL,
		NULL,
		-1,
	},
	{
		0x45EAA401, 
		"ChooseFile", 
		L"ChooseFile", 
		sizeof("ChooseFile")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, ChooseFile),
		NULL,
		NULL,
		-1,
	},
	{
		0xF82E67A6, 
		"StartIndex", 
		L"StartIndex", 
		sizeof("StartIndex")-1, 
		SOAPTYPE_LONG, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, StartIndex),
		NULL,
		NULL,
		-1,
	},
	{
		0xAFEF5FCF, 
		"EndIndex", 
		L"EndIndex", 
		sizeof("EndIndex")-1, 
		SOAPTYPE_LONG, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, EndIndex),
		NULL,
		NULL,
		-1,
	},
	{
		0xCC052EFC, 
		"LogicalName", 
		L"LogicalName", 
		sizeof("LogicalName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, LogicalName),
		NULL,
		NULL,
		-1,
	},
	{
		0x0375F04B, 
		"ParentName", 
		L"ParentName", 
		sizeof("ParentName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, ParentName),
		NULL,
		NULL,
		-1,
	},
	{
		0x30DB669C, 
		"StartForGoback", 
		L"StartForGoback", 
		sizeof("StartForGoback")-1, 
		SOAPTYPE_LONG, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, StartForGoback),
		NULL,
		NULL,
		-1,
	},
	{
		0xAD8D1397, 
		"CountForGoback", 
		L"CountForGoback", 
		sizeof("CountForGoback")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, CountForGoback),
		NULL,
		NULL,
		-1,
	},
	{
		0x061F398E, 
		"Start", 
		L"Start", 
		sizeof("Start")-1, 
		SOAPTYPE_LONG, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, Start),
		NULL,
		NULL,
		-1,
	},
	{
		0x04FB4829, 
		"Count", 
		L"Count", 
		sizeof("Count")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, Count),
		NULL,
		NULL,
		-1,
	},
	{
		0x00282F80, 
		"File", 
		L"File", 
		sizeof("File")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, File),
		NULL,
		NULL,
		-1,
	},
	{
		0x00014C89, 
		"Key", 
		L"Key", 
		sizeof("Key")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, Key),
		NULL,
		NULL,
		-1,
	},
	{
		0xBA4FFE53, 
		"SchemaOnly", 
		L"SchemaOnly", 
		sizeof("SchemaOnly")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, SchemaOnly),
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
		offsetof(__CWsDfu_DFUSearchData_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0xA1D0505D, 
		"DFUDataKeyedColumns1", 
		L"DFUDataKeyedColumns1", 
		sizeof("DFUDataKeyedColumns1")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, DFUDataKeyedColumns1),
		NULL,
		&__DFUDataColumn_map,
		18+1,
	},
	{
		0xA1D0505D,
		"__DFUDataKeyedColumns1_nSizeIs",
		L"__DFUDataKeyedColumns1_nSizeIs",
		sizeof("__DFUDataKeyedColumns1_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUSearchData_struct, __DFUDataKeyedColumns1_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xA1D0505E, 
		"DFUDataKeyedColumns2", 
		L"DFUDataKeyedColumns2", 
		sizeof("DFUDataKeyedColumns2")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, DFUDataKeyedColumns2),
		NULL,
		&__DFUDataColumn_map,
		20+1,
	},
	{
		0xA1D0505E,
		"__DFUDataKeyedColumns2_nSizeIs",
		L"__DFUDataKeyedColumns2_nSizeIs",
		sizeof("__DFUDataKeyedColumns2_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUSearchData_struct, __DFUDataKeyedColumns2_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xA1D0505F, 
		"DFUDataKeyedColumns3", 
		L"DFUDataKeyedColumns3", 
		sizeof("DFUDataKeyedColumns3")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, DFUDataKeyedColumns3),
		NULL,
		&__DFUDataColumn_map,
		22+1,
	},
	{
		0xA1D0505F,
		"__DFUDataKeyedColumns3_nSizeIs",
		L"__DFUDataKeyedColumns3_nSizeIs",
		sizeof("__DFUDataKeyedColumns3_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUSearchData_struct, __DFUDataKeyedColumns3_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xA1D05060, 
		"DFUDataKeyedColumns4", 
		L"DFUDataKeyedColumns4", 
		sizeof("DFUDataKeyedColumns4")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, DFUDataKeyedColumns4),
		NULL,
		&__DFUDataColumn_map,
		24+1,
	},
	{
		0xA1D05060,
		"__DFUDataKeyedColumns4_nSizeIs",
		L"__DFUDataKeyedColumns4_nSizeIs",
		sizeof("__DFUDataKeyedColumns4_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUSearchData_struct, __DFUDataKeyedColumns4_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xA1D05061, 
		"DFUDataKeyedColumns5", 
		L"DFUDataKeyedColumns5", 
		sizeof("DFUDataKeyedColumns5")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, DFUDataKeyedColumns5),
		NULL,
		&__DFUDataColumn_map,
		26+1,
	},
	{
		0xA1D05061,
		"__DFUDataKeyedColumns5_nSizeIs",
		L"__DFUDataKeyedColumns5_nSizeIs",
		sizeof("__DFUDataKeyedColumns5_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUSearchData_struct, __DFUDataKeyedColumns5_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xA1D05062, 
		"DFUDataKeyedColumns6", 
		L"DFUDataKeyedColumns6", 
		sizeof("DFUDataKeyedColumns6")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, DFUDataKeyedColumns6),
		NULL,
		&__DFUDataColumn_map,
		28+1,
	},
	{
		0xA1D05062,
		"__DFUDataKeyedColumns6_nSizeIs",
		L"__DFUDataKeyedColumns6_nSizeIs",
		sizeof("__DFUDataKeyedColumns6_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUSearchData_struct, __DFUDataKeyedColumns6_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xA1D05063, 
		"DFUDataKeyedColumns7", 
		L"DFUDataKeyedColumns7", 
		sizeof("DFUDataKeyedColumns7")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, DFUDataKeyedColumns7),
		NULL,
		&__DFUDataColumn_map,
		30+1,
	},
	{
		0xA1D05063,
		"__DFUDataKeyedColumns7_nSizeIs",
		L"__DFUDataKeyedColumns7_nSizeIs",
		sizeof("__DFUDataKeyedColumns7_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUSearchData_struct, __DFUDataKeyedColumns7_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xA1D05064, 
		"DFUDataKeyedColumns8", 
		L"DFUDataKeyedColumns8", 
		sizeof("DFUDataKeyedColumns8")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, DFUDataKeyedColumns8),
		NULL,
		&__DFUDataColumn_map,
		32+1,
	},
	{
		0xA1D05064,
		"__DFUDataKeyedColumns8_nSizeIs",
		L"__DFUDataKeyedColumns8_nSizeIs",
		sizeof("__DFUDataKeyedColumns8_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUSearchData_struct, __DFUDataKeyedColumns8_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xA1D05065, 
		"DFUDataKeyedColumns9", 
		L"DFUDataKeyedColumns9", 
		sizeof("DFUDataKeyedColumns9")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, DFUDataKeyedColumns9),
		NULL,
		&__DFUDataColumn_map,
		34+1,
	},
	{
		0xA1D05065,
		"__DFUDataKeyedColumns9_nSizeIs",
		L"__DFUDataKeyedColumns9_nSizeIs",
		sizeof("__DFUDataKeyedColumns9_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUSearchData_struct, __DFUDataKeyedColumns9_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xDBDA5C2D, 
		"DFUDataKeyedColumns10", 
		L"DFUDataKeyedColumns10", 
		sizeof("DFUDataKeyedColumns10")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, DFUDataKeyedColumns10),
		NULL,
		&__DFUDataColumn_map,
		36+1,
	},
	{
		0xDBDA5C2D,
		"__DFUDataKeyedColumns10_nSizeIs",
		L"__DFUDataKeyedColumns10_nSizeIs",
		sizeof("__DFUDataKeyedColumns10_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUSearchData_struct, __DFUDataKeyedColumns10_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xDBDA5C2E, 
		"DFUDataKeyedColumns11", 
		L"DFUDataKeyedColumns11", 
		sizeof("DFUDataKeyedColumns11")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, DFUDataKeyedColumns11),
		NULL,
		&__DFUDataColumn_map,
		38+1,
	},
	{
		0xDBDA5C2E,
		"__DFUDataKeyedColumns11_nSizeIs",
		L"__DFUDataKeyedColumns11_nSizeIs",
		sizeof("__DFUDataKeyedColumns11_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUSearchData_struct, __DFUDataKeyedColumns11_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xDBDA5C2F, 
		"DFUDataKeyedColumns12", 
		L"DFUDataKeyedColumns12", 
		sizeof("DFUDataKeyedColumns12")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, DFUDataKeyedColumns12),
		NULL,
		&__DFUDataColumn_map,
		40+1,
	},
	{
		0xDBDA5C2F,
		"__DFUDataKeyedColumns12_nSizeIs",
		L"__DFUDataKeyedColumns12_nSizeIs",
		sizeof("__DFUDataKeyedColumns12_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUSearchData_struct, __DFUDataKeyedColumns12_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xDBDA5C30, 
		"DFUDataKeyedColumns13", 
		L"DFUDataKeyedColumns13", 
		sizeof("DFUDataKeyedColumns13")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, DFUDataKeyedColumns13),
		NULL,
		&__DFUDataColumn_map,
		42+1,
	},
	{
		0xDBDA5C30,
		"__DFUDataKeyedColumns13_nSizeIs",
		L"__DFUDataKeyedColumns13_nSizeIs",
		sizeof("__DFUDataKeyedColumns13_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUSearchData_struct, __DFUDataKeyedColumns13_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xDBDA5C31, 
		"DFUDataKeyedColumns14", 
		L"DFUDataKeyedColumns14", 
		sizeof("DFUDataKeyedColumns14")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, DFUDataKeyedColumns14),
		NULL,
		&__DFUDataColumn_map,
		44+1,
	},
	{
		0xDBDA5C31,
		"__DFUDataKeyedColumns14_nSizeIs",
		L"__DFUDataKeyedColumns14_nSizeIs",
		sizeof("__DFUDataKeyedColumns14_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUSearchData_struct, __DFUDataKeyedColumns14_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xDBDA5C32, 
		"DFUDataKeyedColumns15", 
		L"DFUDataKeyedColumns15", 
		sizeof("DFUDataKeyedColumns15")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, DFUDataKeyedColumns15),
		NULL,
		&__DFUDataColumn_map,
		46+1,
	},
	{
		0xDBDA5C32,
		"__DFUDataKeyedColumns15_nSizeIs",
		L"__DFUDataKeyedColumns15_nSizeIs",
		sizeof("__DFUDataKeyedColumns15_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUSearchData_struct, __DFUDataKeyedColumns15_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xDBDA5C33, 
		"DFUDataKeyedColumns16", 
		L"DFUDataKeyedColumns16", 
		sizeof("DFUDataKeyedColumns16")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, DFUDataKeyedColumns16),
		NULL,
		&__DFUDataColumn_map,
		48+1,
	},
	{
		0xDBDA5C33,
		"__DFUDataKeyedColumns16_nSizeIs",
		L"__DFUDataKeyedColumns16_nSizeIs",
		sizeof("__DFUDataKeyedColumns16_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUSearchData_struct, __DFUDataKeyedColumns16_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xDBDA5C34, 
		"DFUDataKeyedColumns17", 
		L"DFUDataKeyedColumns17", 
		sizeof("DFUDataKeyedColumns17")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, DFUDataKeyedColumns17),
		NULL,
		&__DFUDataColumn_map,
		50+1,
	},
	{
		0xDBDA5C34,
		"__DFUDataKeyedColumns17_nSizeIs",
		L"__DFUDataKeyedColumns17_nSizeIs",
		sizeof("__DFUDataKeyedColumns17_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUSearchData_struct, __DFUDataKeyedColumns17_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xDBDA5C35, 
		"DFUDataKeyedColumns18", 
		L"DFUDataKeyedColumns18", 
		sizeof("DFUDataKeyedColumns18")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, DFUDataKeyedColumns18),
		NULL,
		&__DFUDataColumn_map,
		52+1,
	},
	{
		0xDBDA5C35,
		"__DFUDataKeyedColumns18_nSizeIs",
		L"__DFUDataKeyedColumns18_nSizeIs",
		sizeof("__DFUDataKeyedColumns18_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUSearchData_struct, __DFUDataKeyedColumns18_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xDBDA5C36, 
		"DFUDataKeyedColumns19", 
		L"DFUDataKeyedColumns19", 
		sizeof("DFUDataKeyedColumns19")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, DFUDataKeyedColumns19),
		NULL,
		&__DFUDataColumn_map,
		54+1,
	},
	{
		0xDBDA5C36,
		"__DFUDataKeyedColumns19_nSizeIs",
		L"__DFUDataKeyedColumns19_nSizeIs",
		sizeof("__DFUDataKeyedColumns19_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUSearchData_struct, __DFUDataKeyedColumns19_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xDBDA5C4E, 
		"DFUDataKeyedColumns20", 
		L"DFUDataKeyedColumns20", 
		sizeof("DFUDataKeyedColumns20")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, DFUDataKeyedColumns20),
		NULL,
		&__DFUDataColumn_map,
		56+1,
	},
	{
		0xDBDA5C4E,
		"__DFUDataKeyedColumns20_nSizeIs",
		L"__DFUDataKeyedColumns20_nSizeIs",
		sizeof("__DFUDataKeyedColumns20_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUSearchData_struct, __DFUDataKeyedColumns20_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x1CCB8E28, 
		"DFUDataNonKeyedColumns1", 
		L"DFUDataNonKeyedColumns1", 
		sizeof("DFUDataNonKeyedColumns1")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, DFUDataNonKeyedColumns1),
		NULL,
		&__DFUDataColumn_map,
		58+1,
	},
	{
		0x1CCB8E28,
		"__DFUDataNonKeyedColumns1_nSizeIs",
		L"__DFUDataNonKeyedColumns1_nSizeIs",
		sizeof("__DFUDataNonKeyedColumns1_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUSearchData_struct, __DFUDataNonKeyedColumns1_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x1CCB8E29, 
		"DFUDataNonKeyedColumns2", 
		L"DFUDataNonKeyedColumns2", 
		sizeof("DFUDataNonKeyedColumns2")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, DFUDataNonKeyedColumns2),
		NULL,
		&__DFUDataColumn_map,
		60+1,
	},
	{
		0x1CCB8E29,
		"__DFUDataNonKeyedColumns2_nSizeIs",
		L"__DFUDataNonKeyedColumns2_nSizeIs",
		sizeof("__DFUDataNonKeyedColumns2_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUSearchData_struct, __DFUDataNonKeyedColumns2_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x1CCB8E2A, 
		"DFUDataNonKeyedColumns3", 
		L"DFUDataNonKeyedColumns3", 
		sizeof("DFUDataNonKeyedColumns3")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, DFUDataNonKeyedColumns3),
		NULL,
		&__DFUDataColumn_map,
		62+1,
	},
	{
		0x1CCB8E2A,
		"__DFUDataNonKeyedColumns3_nSizeIs",
		L"__DFUDataNonKeyedColumns3_nSizeIs",
		sizeof("__DFUDataNonKeyedColumns3_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUSearchData_struct, __DFUDataNonKeyedColumns3_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x1CCB8E2B, 
		"DFUDataNonKeyedColumns4", 
		L"DFUDataNonKeyedColumns4", 
		sizeof("DFUDataNonKeyedColumns4")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, DFUDataNonKeyedColumns4),
		NULL,
		&__DFUDataColumn_map,
		64+1,
	},
	{
		0x1CCB8E2B,
		"__DFUDataNonKeyedColumns4_nSizeIs",
		L"__DFUDataNonKeyedColumns4_nSizeIs",
		sizeof("__DFUDataNonKeyedColumns4_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUSearchData_struct, __DFUDataNonKeyedColumns4_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x1CCB8E2C, 
		"DFUDataNonKeyedColumns5", 
		L"DFUDataNonKeyedColumns5", 
		sizeof("DFUDataNonKeyedColumns5")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, DFUDataNonKeyedColumns5),
		NULL,
		&__DFUDataColumn_map,
		66+1,
	},
	{
		0x1CCB8E2C,
		"__DFUDataNonKeyedColumns5_nSizeIs",
		L"__DFUDataNonKeyedColumns5_nSizeIs",
		sizeof("__DFUDataNonKeyedColumns5_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUSearchData_struct, __DFUDataNonKeyedColumns5_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x1CCB8E2D, 
		"DFUDataNonKeyedColumns6", 
		L"DFUDataNonKeyedColumns6", 
		sizeof("DFUDataNonKeyedColumns6")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, DFUDataNonKeyedColumns6),
		NULL,
		&__DFUDataColumn_map,
		68+1,
	},
	{
		0x1CCB8E2D,
		"__DFUDataNonKeyedColumns6_nSizeIs",
		L"__DFUDataNonKeyedColumns6_nSizeIs",
		sizeof("__DFUDataNonKeyedColumns6_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUSearchData_struct, __DFUDataNonKeyedColumns6_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x1CCB8E2E, 
		"DFUDataNonKeyedColumns7", 
		L"DFUDataNonKeyedColumns7", 
		sizeof("DFUDataNonKeyedColumns7")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, DFUDataNonKeyedColumns7),
		NULL,
		&__DFUDataColumn_map,
		70+1,
	},
	{
		0x1CCB8E2E,
		"__DFUDataNonKeyedColumns7_nSizeIs",
		L"__DFUDataNonKeyedColumns7_nSizeIs",
		sizeof("__DFUDataNonKeyedColumns7_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUSearchData_struct, __DFUDataNonKeyedColumns7_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x1CCB8E2F, 
		"DFUDataNonKeyedColumns8", 
		L"DFUDataNonKeyedColumns8", 
		sizeof("DFUDataNonKeyedColumns8")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, DFUDataNonKeyedColumns8),
		NULL,
		&__DFUDataColumn_map,
		72+1,
	},
	{
		0x1CCB8E2F,
		"__DFUDataNonKeyedColumns8_nSizeIs",
		L"__DFUDataNonKeyedColumns8_nSizeIs",
		sizeof("__DFUDataNonKeyedColumns8_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUSearchData_struct, __DFUDataNonKeyedColumns8_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x1CCB8E30, 
		"DFUDataNonKeyedColumns9", 
		L"DFUDataNonKeyedColumns9", 
		sizeof("DFUDataNonKeyedColumns9")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, DFUDataNonKeyedColumns9),
		NULL,
		&__DFUDataColumn_map,
		74+1,
	},
	{
		0x1CCB8E30,
		"__DFUDataNonKeyedColumns9_nSizeIs",
		L"__DFUDataNonKeyedColumns9_nSizeIs",
		sizeof("__DFUDataNonKeyedColumns9_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUSearchData_struct, __DFUDataNonKeyedColumns9_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xB63D5358, 
		"DFUDataNonKeyedColumns10", 
		L"DFUDataNonKeyedColumns10", 
		sizeof("DFUDataNonKeyedColumns10")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, DFUDataNonKeyedColumns10),
		NULL,
		&__DFUDataColumn_map,
		76+1,
	},
	{
		0xB63D5358,
		"__DFUDataNonKeyedColumns10_nSizeIs",
		L"__DFUDataNonKeyedColumns10_nSizeIs",
		sizeof("__DFUDataNonKeyedColumns10_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUSearchData_struct, __DFUDataNonKeyedColumns10_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xB63D5359, 
		"DFUDataNonKeyedColumns11", 
		L"DFUDataNonKeyedColumns11", 
		sizeof("DFUDataNonKeyedColumns11")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, DFUDataNonKeyedColumns11),
		NULL,
		&__DFUDataColumn_map,
		78+1,
	},
	{
		0xB63D5359,
		"__DFUDataNonKeyedColumns11_nSizeIs",
		L"__DFUDataNonKeyedColumns11_nSizeIs",
		sizeof("__DFUDataNonKeyedColumns11_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUSearchData_struct, __DFUDataNonKeyedColumns11_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xB63D535A, 
		"DFUDataNonKeyedColumns12", 
		L"DFUDataNonKeyedColumns12", 
		sizeof("DFUDataNonKeyedColumns12")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, DFUDataNonKeyedColumns12),
		NULL,
		&__DFUDataColumn_map,
		80+1,
	},
	{
		0xB63D535A,
		"__DFUDataNonKeyedColumns12_nSizeIs",
		L"__DFUDataNonKeyedColumns12_nSizeIs",
		sizeof("__DFUDataNonKeyedColumns12_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUSearchData_struct, __DFUDataNonKeyedColumns12_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xB63D535B, 
		"DFUDataNonKeyedColumns13", 
		L"DFUDataNonKeyedColumns13", 
		sizeof("DFUDataNonKeyedColumns13")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, DFUDataNonKeyedColumns13),
		NULL,
		&__DFUDataColumn_map,
		82+1,
	},
	{
		0xB63D535B,
		"__DFUDataNonKeyedColumns13_nSizeIs",
		L"__DFUDataNonKeyedColumns13_nSizeIs",
		sizeof("__DFUDataNonKeyedColumns13_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUSearchData_struct, __DFUDataNonKeyedColumns13_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xB63D535C, 
		"DFUDataNonKeyedColumns14", 
		L"DFUDataNonKeyedColumns14", 
		sizeof("DFUDataNonKeyedColumns14")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, DFUDataNonKeyedColumns14),
		NULL,
		&__DFUDataColumn_map,
		84+1,
	},
	{
		0xB63D535C,
		"__DFUDataNonKeyedColumns14_nSizeIs",
		L"__DFUDataNonKeyedColumns14_nSizeIs",
		sizeof("__DFUDataNonKeyedColumns14_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUSearchData_struct, __DFUDataNonKeyedColumns14_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xB63D535D, 
		"DFUDataNonKeyedColumns15", 
		L"DFUDataNonKeyedColumns15", 
		sizeof("DFUDataNonKeyedColumns15")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, DFUDataNonKeyedColumns15),
		NULL,
		&__DFUDataColumn_map,
		86+1,
	},
	{
		0xB63D535D,
		"__DFUDataNonKeyedColumns15_nSizeIs",
		L"__DFUDataNonKeyedColumns15_nSizeIs",
		sizeof("__DFUDataNonKeyedColumns15_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUSearchData_struct, __DFUDataNonKeyedColumns15_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xB63D535E, 
		"DFUDataNonKeyedColumns16", 
		L"DFUDataNonKeyedColumns16", 
		sizeof("DFUDataNonKeyedColumns16")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, DFUDataNonKeyedColumns16),
		NULL,
		&__DFUDataColumn_map,
		88+1,
	},
	{
		0xB63D535E,
		"__DFUDataNonKeyedColumns16_nSizeIs",
		L"__DFUDataNonKeyedColumns16_nSizeIs",
		sizeof("__DFUDataNonKeyedColumns16_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUSearchData_struct, __DFUDataNonKeyedColumns16_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xB63D535F, 
		"DFUDataNonKeyedColumns17", 
		L"DFUDataNonKeyedColumns17", 
		sizeof("DFUDataNonKeyedColumns17")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, DFUDataNonKeyedColumns17),
		NULL,
		&__DFUDataColumn_map,
		90+1,
	},
	{
		0xB63D535F,
		"__DFUDataNonKeyedColumns17_nSizeIs",
		L"__DFUDataNonKeyedColumns17_nSizeIs",
		sizeof("__DFUDataNonKeyedColumns17_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUSearchData_struct, __DFUDataNonKeyedColumns17_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xB63D5360, 
		"DFUDataNonKeyedColumns18", 
		L"DFUDataNonKeyedColumns18", 
		sizeof("DFUDataNonKeyedColumns18")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, DFUDataNonKeyedColumns18),
		NULL,
		&__DFUDataColumn_map,
		92+1,
	},
	{
		0xB63D5360,
		"__DFUDataNonKeyedColumns18_nSizeIs",
		L"__DFUDataNonKeyedColumns18_nSizeIs",
		sizeof("__DFUDataNonKeyedColumns18_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUSearchData_struct, __DFUDataNonKeyedColumns18_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xB63D5361, 
		"DFUDataNonKeyedColumns19", 
		L"DFUDataNonKeyedColumns19", 
		sizeof("DFUDataNonKeyedColumns19")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, DFUDataNonKeyedColumns19),
		NULL,
		&__DFUDataColumn_map,
		94+1,
	},
	{
		0xB63D5361,
		"__DFUDataNonKeyedColumns19_nSizeIs",
		L"__DFUDataNonKeyedColumns19_nSizeIs",
		sizeof("__DFUDataNonKeyedColumns19_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUSearchData_struct, __DFUDataNonKeyedColumns19_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xB63D5379, 
		"DFUDataNonKeyedColumns20", 
		L"DFUDataNonKeyedColumns20", 
		sizeof("DFUDataNonKeyedColumns20")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, DFUDataNonKeyedColumns20),
		NULL,
		&__DFUDataColumn_map,
		96+1,
	},
	{
		0xB63D5379,
		"__DFUDataNonKeyedColumns20_nSizeIs",
		L"__DFUDataNonKeyedColumns20_nSizeIs",
		sizeof("__DFUDataNonKeyedColumns20_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUSearchData_struct, __DFUDataNonKeyedColumns20_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x2827B2C1, 
		"RowCount", 
		L"RowCount", 
		sizeof("RowCount")-1, 
		SOAPTYPE_LONG, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, RowCount),
		NULL,
		NULL,
		-1,
	},
	{
		0x002C70A1, 
		"Name", 
		L"Name", 
		sizeof("Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, Name),
		NULL,
		NULL,
		-1,
	},
	{
		0x47241CB4, 
		"FilterForGoBack", 
		L"FilterForGoBack", 
		sizeof("FilterForGoBack")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, FilterForGoBack),
		NULL,
		NULL,
		-1,
	},
	{
		0x7C9CAD0D, 
		"ColumnsHidden", 
		L"ColumnsHidden", 
		sizeof("ColumnsHidden")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, ColumnsHidden),
		NULL,
		&__DFUDataColumn_map,
		101+1,
	},
	{
		0x7C9CAD0D,
		"__ColumnsHidden_nSizeIs",
		L"__ColumnsHidden_nSizeIs",
		sizeof("__ColumnsHidden_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUSearchData_struct, __ColumnsHidden_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x928CF017, 
		"ColumnCount", 
		L"ColumnCount", 
		sizeof("ColumnCount")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, ColumnCount),
		NULL,
		NULL,
		-1,
	},
	{
		0xF6F9E158, 
		"PageSize", 
		L"PageSize", 
		sizeof("PageSize")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, PageSize),
		NULL,
		NULL,
		-1,
	},
	{
		0x062EE2C4, 
		"Total", 
		L"Total", 
		sizeof("Total")-1, 
		SOAPTYPE_LONG, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, Total),
		NULL,
		NULL,
		-1,
	},
	{
		0xC6ABBE1F, 
		"Result", 
		L"Result", 
		sizeof("Result")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, Result),
		NULL,
		NULL,
		-1,
	},
	{
		0x5B3F5AA0, 
		"MsgToDisplay", 
		L"MsgToDisplay", 
		sizeof("MsgToDisplay")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearchData_struct, MsgToDisplay),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsDfu_DFUSearchData_map =
{
	0x5325B93E,
	"DFUSearchDataRequest",
	L"DFUSearchDataResponse",
	sizeof("DFUSearchDataRequest")-1,
	sizeof("DFUSearchDataResponse")-1,
	SOAPMAP_FUNC,
	__CWsDfu_DFUSearchData_entries,
	sizeof(__CWsDfu_DFUSearchData_struct),
	67,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x18AC14AC,
	"http://webservices.seisint.com/WsDfu",
	L"http://webservices.seisint.com/WsDfu",
	sizeof("http://webservices.seisint.com/WsDfu")-1
};


struct __CWsDfu_AddRemote_struct
{
	BSTR dstname;
	BSTR srcname;
	BSTR srcdali;
	BSTR srcusername;
	BSTR srcpassword;
	ArrayOfEspException Exceptions;
};

extern __declspec(selectany) const _soapmapentry __CWsDfu_AddRemote_entries[] =
{

	{
		0x267212EC, 
		"dstname", 
		L"dstname", 
		sizeof("dstname")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_AddRemote_struct, dstname),
		NULL,
		NULL,
		-1,
	},
	{
		0xA592D909, 
		"srcname", 
		L"srcname", 
		sizeof("srcname")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_AddRemote_struct, srcname),
		NULL,
		NULL,
		-1,
	},
	{
		0xA58D5D22, 
		"srcdali", 
		L"srcdali", 
		sizeof("srcdali")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_AddRemote_struct, srcdali),
		NULL,
		NULL,
		-1,
	},
	{
		0x43DF4388, 
		"srcusername", 
		L"srcusername", 
		sizeof("srcusername")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_AddRemote_struct, srcusername),
		NULL,
		NULL,
		-1,
	},
	{
		0x5DBD527B, 
		"srcpassword", 
		L"srcpassword", 
		sizeof("srcpassword")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_AddRemote_struct, srcpassword),
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
		offsetof(__CWsDfu_AddRemote_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsDfu_AddRemote_map =
{
	0xB5FBD704,
	"AddRemoteRequest",
	L"AddRemoteResponse",
	sizeof("AddRemoteRequest")-1,
	sizeof("AddRemoteResponse")-1,
	SOAPMAP_FUNC,
	__CWsDfu_AddRemote_entries,
	sizeof(__CWsDfu_AddRemote_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x18AC14AC,
	"http://webservices.seisint.com/WsDfu",
	L"http://webservices.seisint.com/WsDfu",
	sizeof("http://webservices.seisint.com/WsDfu")-1
};


struct __CWsDfu_SuperfileList_struct
{
	BSTR superfile;
	ArrayOfEspException Exceptions;
	BSTR *subfiles;
	int __subfiles_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CWsDfu_SuperfileList_entries[] =
{

	{
		0xEF594F4F, 
		"superfile", 
		L"superfile", 
		sizeof("superfile")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_SuperfileList_struct, superfile),
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
		offsetof(__CWsDfu_SuperfileList_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0x3BFB22FD, 
		"subfiles", 
		L"subfiles", 
		sizeof("subfiles")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_SuperfileList_struct, subfiles),
		NULL,
		NULL,
		2+1,
	},
	{
		0x3BFB22FD,
		"__subfiles_nSizeIs",
		L"__subfiles_nSizeIs",
		sizeof("__subfiles_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_SuperfileList_struct, __subfiles_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsDfu_SuperfileList_map =
{
	0x621FC69A,
	"SuperfileListRequest",
	L"SuperfileListResponse",
	sizeof("SuperfileListRequest")-1,
	sizeof("SuperfileListResponse")-1,
	SOAPMAP_FUNC,
	__CWsDfu_SuperfileList_entries,
	sizeof(__CWsDfu_SuperfileList_struct),
	3,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x18AC14AC,
	"http://webservices.seisint.com/WsDfu",
	L"http://webservices.seisint.com/WsDfu",
	sizeof("http://webservices.seisint.com/WsDfu")-1
};


struct __CWsDfu_DFUSearch_struct
{
	BSTR ShowExample;
	ArrayOfEspException Exceptions;
	BSTR *ClusterNames;
	int __ClusterNames_nSizeIs;
	BSTR *FileTypes;
	int __FileTypes_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CWsDfu_DFUSearch_entries[] =
{

	{
		0x145F11ED, 
		"ShowExample", 
		L"ShowExample", 
		sizeof("ShowExample")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearch_struct, ShowExample),
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
		offsetof(__CWsDfu_DFUSearch_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0xBBBE0A56, 
		"ClusterNames", 
		L"ClusterNames", 
		sizeof("ClusterNames")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearch_struct, ClusterNames),
		NULL,
		NULL,
		2+1,
	},
	{
		0xBBBE0A56,
		"__ClusterNames_nSizeIs",
		L"__ClusterNames_nSizeIs",
		sizeof("__ClusterNames_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUSearch_struct, __ClusterNames_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x2C662D95, 
		"FileTypes", 
		L"FileTypes", 
		sizeof("FileTypes")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSearch_struct, FileTypes),
		NULL,
		NULL,
		4+1,
	},
	{
		0x2C662D95,
		"__FileTypes_nSizeIs",
		L"__FileTypes_nSizeIs",
		sizeof("__FileTypes_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUSearch_struct, __FileTypes_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsDfu_DFUSearch_map =
{
	0x07CF4704,
	"DFUSearchRequest",
	L"DFUSearchResponse",
	sizeof("DFUSearchRequest")-1,
	sizeof("DFUSearchResponse")-1,
	SOAPMAP_FUNC,
	__CWsDfu_DFUSearch_entries,
	sizeof(__CWsDfu_DFUSearch_struct),
	4,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x18AC14AC,
	"http://webservices.seisint.com/WsDfu",
	L"http://webservices.seisint.com/WsDfu",
	sizeof("http://webservices.seisint.com/WsDfu")-1
};


struct __CWsDfu_DFUFileView_struct
{
	BSTR Scope;
	ArrayOfEspException Exceptions;
	int NumFiles;
	DFULogicalFile *DFULogicalFiles;
	int __DFULogicalFiles_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CWsDfu_DFUFileView_entries[] =
{

	{
		0x0616225A, 
		"Scope", 
		L"Scope", 
		sizeof("Scope")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUFileView_struct, Scope),
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
		offsetof(__CWsDfu_DFUFileView_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0x2DDD0D43, 
		"NumFiles", 
		L"NumFiles", 
		sizeof("NumFiles")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUFileView_struct, NumFiles),
		NULL,
		NULL,
		-1,
	},
	{
		0x3935BACD, 
		"DFULogicalFiles", 
		L"DFULogicalFiles", 
		sizeof("DFULogicalFiles")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUFileView_struct, DFULogicalFiles),
		NULL,
		&__DFULogicalFile_map,
		3+1,
	},
	{
		0x3935BACD,
		"__DFULogicalFiles_nSizeIs",
		L"__DFULogicalFiles_nSizeIs",
		sizeof("__DFULogicalFiles_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUFileView_struct, __DFULogicalFiles_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsDfu_DFUFileView_map =
{
	0x77DA5A09,
	"DFUFileViewRequest",
	L"DFUFileViewResponse",
	sizeof("DFUFileViewRequest")-1,
	sizeof("DFUFileViewResponse")-1,
	SOAPMAP_FUNC,
	__CWsDfu_DFUFileView_entries,
	sizeof(__CWsDfu_DFUFileView_struct),
	4,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x18AC14AC,
	"http://webservices.seisint.com/WsDfu",
	L"http://webservices.seisint.com/WsDfu",
	sizeof("http://webservices.seisint.com/WsDfu")-1
};


struct __CWsDfu_DFUSpace_struct
{
	BSTR CountBy;
	BSTR ScopeUnder;
	BSTR OwnerUnder;
	BSTR Interval;
	BSTR StartDate;
	BSTR EndDate;
	ArrayOfEspException Exceptions;
	DFUSpaceItem *DFUSpaceItems;
	int __DFUSpaceItems_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CWsDfu_DFUSpace_entries[] =
{

	{
		0x30EDFF64, 
		"CountBy", 
		L"CountBy", 
		sizeof("CountBy")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSpace_struct, CountBy),
		NULL,
		NULL,
		-1,
	},
	{
		0x6B6BD5F8, 
		"ScopeUnder", 
		L"ScopeUnder", 
		sizeof("ScopeUnder")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSpace_struct, ScopeUnder),
		NULL,
		NULL,
		-1,
	},
	{
		0x584B0489, 
		"OwnerUnder", 
		L"OwnerUnder", 
		sizeof("OwnerUnder")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSpace_struct, OwnerUnder),
		NULL,
		NULL,
		-1,
	},
	{
		0x88466905, 
		"Interval", 
		L"Interval", 
		sizeof("Interval")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSpace_struct, Interval),
		NULL,
		NULL,
		-1,
	},
	{
		0x0F44464C, 
		"StartDate", 
		L"StartDate", 
		sizeof("StartDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSpace_struct, StartDate),
		NULL,
		NULL,
		-1,
	},
	{
		0xC7425D55, 
		"EndDate", 
		L"EndDate", 
		sizeof("EndDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSpace_struct, EndDate),
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
		offsetof(__CWsDfu_DFUSpace_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0x402B3FCD, 
		"DFUSpaceItems", 
		L"DFUSpaceItems", 
		sizeof("DFUSpaceItems")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUSpace_struct, DFUSpaceItems),
		NULL,
		&__DFUSpaceItem_map,
		7+1,
	},
	{
		0x402B3FCD,
		"__DFUSpaceItems_nSizeIs",
		L"__DFUSpaceItems_nSizeIs",
		sizeof("__DFUSpaceItems_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUSpace_struct, __DFUSpaceItems_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsDfu_DFUSpace_map =
{
	0xE3CC693A,
	"DFUSpaceRequest",
	L"DFUSpaceResponse",
	sizeof("DFUSpaceRequest")-1,
	sizeof("DFUSpaceResponse")-1,
	SOAPMAP_FUNC,
	__CWsDfu_DFUSpace_entries,
	sizeof(__CWsDfu_DFUSpace_struct),
	8,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x18AC14AC,
	"http://webservices.seisint.com/WsDfu",
	L"http://webservices.seisint.com/WsDfu",
	sizeof("http://webservices.seisint.com/WsDfu")-1
};


struct __CWsDfu_DFUInfo_struct
{
	BSTR Name;
	BSTR Cluster;
	bool UpdateDescription;
	BSTR FileName;
	BSTR FileDesc;
	ArrayOfEspException Exceptions;
	DFUFileDetail FileDetail;
};

extern __declspec(selectany) const _soapmapentry __CWsDfu_DFUInfo_entries[] =
{

	{
		0x002C70A1, 
		"Name", 
		L"Name", 
		sizeof("Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUInfo_struct, Name),
		NULL,
		NULL,
		-1,
	},
	{
		0x29F147E2, 
		"Cluster", 
		L"Cluster", 
		sizeof("Cluster")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUInfo_struct, Cluster),
		NULL,
		NULL,
		-1,
	},
	{
		0x9699CF67, 
		"UpdateDescription", 
		L"UpdateDescription", 
		sizeof("UpdateDescription")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUInfo_struct, UpdateDescription),
		NULL,
		NULL,
		-1,
	},
	{
		0x2FE06021, 
		"FileName", 
		L"FileName", 
		sizeof("FileName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUInfo_struct, FileName),
		NULL,
		NULL,
		-1,
	},
	{
		0x2FDAF61F, 
		"FileDesc", 
		L"FileDesc", 
		sizeof("FileDesc")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUInfo_struct, FileDesc),
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
		offsetof(__CWsDfu_DFUInfo_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0x92718BB3, 
		"FileDetail", 
		L"FileDetail", 
		sizeof("FileDetail")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUInfo_struct, FileDetail),
		NULL,
		&__DFUFileDetail_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsDfu_DFUInfo_map =
{
	0x6EB8487A,
	"DFUInfoRequest",
	L"DFUInfoResponse",
	sizeof("DFUInfoRequest")-1,
	sizeof("DFUInfoResponse")-1,
	SOAPMAP_FUNC,
	__CWsDfu_DFUInfo_entries,
	sizeof(__CWsDfu_DFUInfo_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x18AC14AC,
	"http://webservices.seisint.com/WsDfu",
	L"http://webservices.seisint.com/WsDfu",
	sizeof("http://webservices.seisint.com/WsDfu")-1
};


struct __CWsDfu_DFUQuery_struct
{
	BSTR Prefix;
	BSTR ClusterName;
	BSTR LogicalName;
	BSTR Description;
	BSTR Owner;
	BSTR StartDate;
	BSTR EndDate;
	BSTR FileType;
	__int64 FileSizeFrom;
	__int64 FileSizeTo;
	int FirstN;
	BSTR FirstNType;
	int PageSize;
	int PageStartFrom;
	BSTR Sortby;
	bool Descending;
	bool OneLevelDirFileReturn;
	ArrayOfEspException Exceptions;
	DFULogicalFile *DFULogicalFiles;
	int __DFULogicalFiles_nSizeIs;
	int LastPageFrom;
	int PageEndAt;
	int PrevPageFrom;
	int NextPageFrom;
	int NumFiles;
	BSTR BasicQuery;
	BSTR ParametersForPaging;
	BSTR Filters;
};

extern __declspec(selectany) const _soapmapentry __CWsDfu_DFUQuery_entries[] =
{

	{
		0xC2E4BDEE, 
		"Prefix", 
		L"Prefix", 
		sizeof("Prefix")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUQuery_struct, Prefix),
		NULL,
		NULL,
		-1,
	},
	{
		0xCF62D983, 
		"ClusterName", 
		L"ClusterName", 
		sizeof("ClusterName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUQuery_struct, ClusterName),
		NULL,
		NULL,
		-1,
	},
	{
		0xCC052EFC, 
		"LogicalName", 
		L"LogicalName", 
		sizeof("LogicalName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUQuery_struct, LogicalName),
		NULL,
		NULL,
		-1,
	},
	{
		0xFAB05784, 
		"Description", 
		L"Description", 
		sizeof("Description")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUQuery_struct, Description),
		NULL,
		NULL,
		-1,
	},
	{
		0x05D8B24B, 
		"Owner", 
		L"Owner", 
		sizeof("Owner")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUQuery_struct, Owner),
		NULL,
		NULL,
		-1,
	},
	{
		0x0F44464C, 
		"StartDate", 
		L"StartDate", 
		sizeof("StartDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUQuery_struct, StartDate),
		NULL,
		NULL,
		-1,
	},
	{
		0xC7425D55, 
		"EndDate", 
		L"EndDate", 
		sizeof("EndDate")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUQuery_struct, EndDate),
		NULL,
		NULL,
		-1,
	},
	{
		0x2FE410E2, 
		"FileType", 
		L"FileType", 
		sizeof("FileType")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUQuery_struct, FileType),
		NULL,
		NULL,
		-1,
	},
	{
		0x0EFBFD6F, 
		"FileSizeFrom", 
		L"FileSizeFrom", 
		sizeof("FileSizeFrom")-1, 
		SOAPTYPE_LONG, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUQuery_struct, FileSizeFrom),
		NULL,
		NULL,
		-1,
	},
	{
		0xB5BAA7BE, 
		"FileSizeTo", 
		L"FileSizeTo", 
		sizeof("FileSizeTo")-1, 
		SOAPTYPE_LONG, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUQuery_struct, FileSizeTo),
		NULL,
		NULL,
		-1,
	},
	{
		0xAAF5A496, 
		"FirstN", 
		L"FirstN", 
		sizeof("FirstN")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUQuery_struct, FirstN),
		NULL,
		NULL,
		-1,
	},
	{
		0xBFF220F8, 
		"FirstNType", 
		L"FirstNType", 
		sizeof("FirstNType")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUQuery_struct, FirstNType),
		NULL,
		NULL,
		-1,
	},
	{
		0xF6F9E158, 
		"PageSize", 
		L"PageSize", 
		sizeof("PageSize")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUQuery_struct, PageSize),
		NULL,
		NULL,
		-1,
	},
	{
		0xA9A72FFF, 
		"PageStartFrom", 
		L"PageStartFrom", 
		sizeof("PageStartFrom")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUQuery_struct, PageStartFrom),
		NULL,
		NULL,
		-1,
	},
	{
		0xC9B549E3, 
		"Sortby", 
		L"Sortby", 
		sizeof("Sortby")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUQuery_struct, Sortby),
		NULL,
		NULL,
		-1,
	},
	{
		0xE1D773F4, 
		"Descending", 
		L"Descending", 
		sizeof("Descending")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUQuery_struct, Descending),
		NULL,
		NULL,
		-1,
	},
	{
		0x6AC296D9, 
		"OneLevelDirFileReturn", 
		L"OneLevelDirFileReturn", 
		sizeof("OneLevelDirFileReturn")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUQuery_struct, OneLevelDirFileReturn),
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
		offsetof(__CWsDfu_DFUQuery_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0x3935BACD, 
		"DFULogicalFiles", 
		L"DFULogicalFiles", 
		sizeof("DFULogicalFiles")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUQuery_struct, DFULogicalFiles),
		NULL,
		&__DFULogicalFile_map,
		18+1,
	},
	{
		0x3935BACD,
		"__DFULogicalFiles_nSizeIs",
		L"__DFULogicalFiles_nSizeIs",
		sizeof("__DFULogicalFiles_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUQuery_struct, __DFULogicalFiles_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xA075B205, 
		"LastPageFrom", 
		L"LastPageFrom", 
		sizeof("LastPageFrom")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUQuery_struct, LastPageFrom),
		NULL,
		NULL,
		-1,
	},
	{
		0xD53B1169, 
		"PageEndAt", 
		L"PageEndAt", 
		sizeof("PageEndAt")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUQuery_struct, PageEndAt),
		NULL,
		NULL,
		-1,
	},
	{
		0xEFD0230E, 
		"PrevPageFrom", 
		L"PrevPageFrom", 
		sizeof("PrevPageFrom")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUQuery_struct, PrevPageFrom),
		NULL,
		NULL,
		-1,
	},
	{
		0x2C041770, 
		"NextPageFrom", 
		L"NextPageFrom", 
		sizeof("NextPageFrom")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUQuery_struct, NextPageFrom),
		NULL,
		NULL,
		-1,
	},
	{
		0x2DDD0D43, 
		"NumFiles", 
		L"NumFiles", 
		sizeof("NumFiles")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUQuery_struct, NumFiles),
		NULL,
		NULL,
		-1,
	},
	{
		0xCF002458, 
		"BasicQuery", 
		L"BasicQuery", 
		sizeof("BasicQuery")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUQuery_struct, BasicQuery),
		NULL,
		NULL,
		-1,
	},
	{
		0x345CECF1, 
		"ParametersForPaging", 
		L"ParametersForPaging", 
		sizeof("ParametersForPaging")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUQuery_struct, ParametersForPaging),
		NULL,
		NULL,
		-1,
	},
	{
		0x093DF5F9, 
		"Filters", 
		L"Filters", 
		sizeof("Filters")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUQuery_struct, Filters),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsDfu_DFUQuery_map =
{
	0xA5995324,
	"DFUQueryRequest",
	L"DFUQueryResponse",
	sizeof("DFUQueryRequest")-1,
	sizeof("DFUQueryResponse")-1,
	SOAPMAP_FUNC,
	__CWsDfu_DFUQuery_entries,
	sizeof(__CWsDfu_DFUQuery_struct),
	26,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x18AC14AC,
	"http://webservices.seisint.com/WsDfu",
	L"http://webservices.seisint.com/WsDfu",
	sizeof("http://webservices.seisint.com/WsDfu")-1
};


struct __CWsDfu_DFUGetDataColumns_struct
{
	BSTR OpenLogicalName;
	BSTR LogicalName;
	BSTR FilterBy;
	BSTR ShowColumns;
	int ChooseFile;
	BSTR Cluster;
	BSTR ClusterType;
	__int64 StartIndex;
	__int64 EndIndex;
	ArrayOfEspException Exceptions;
	DFUDataColumn *DFUDataKeyedColumns1;
	int __DFUDataKeyedColumns1_nSizeIs;
	DFUDataColumn *DFUDataKeyedColumns2;
	int __DFUDataKeyedColumns2_nSizeIs;
	DFUDataColumn *DFUDataKeyedColumns3;
	int __DFUDataKeyedColumns3_nSizeIs;
	DFUDataColumn *DFUDataKeyedColumns4;
	int __DFUDataKeyedColumns4_nSizeIs;
	DFUDataColumn *DFUDataKeyedColumns5;
	int __DFUDataKeyedColumns5_nSizeIs;
	DFUDataColumn *DFUDataKeyedColumns6;
	int __DFUDataKeyedColumns6_nSizeIs;
	DFUDataColumn *DFUDataKeyedColumns7;
	int __DFUDataKeyedColumns7_nSizeIs;
	DFUDataColumn *DFUDataKeyedColumns8;
	int __DFUDataKeyedColumns8_nSizeIs;
	DFUDataColumn *DFUDataKeyedColumns9;
	int __DFUDataKeyedColumns9_nSizeIs;
	DFUDataColumn *DFUDataKeyedColumns10;
	int __DFUDataKeyedColumns10_nSizeIs;
	DFUDataColumn *DFUDataKeyedColumns11;
	int __DFUDataKeyedColumns11_nSizeIs;
	DFUDataColumn *DFUDataKeyedColumns12;
	int __DFUDataKeyedColumns12_nSizeIs;
	DFUDataColumn *DFUDataKeyedColumns13;
	int __DFUDataKeyedColumns13_nSizeIs;
	DFUDataColumn *DFUDataKeyedColumns14;
	int __DFUDataKeyedColumns14_nSizeIs;
	DFUDataColumn *DFUDataKeyedColumns15;
	int __DFUDataKeyedColumns15_nSizeIs;
	DFUDataColumn *DFUDataKeyedColumns16;
	int __DFUDataKeyedColumns16_nSizeIs;
	DFUDataColumn *DFUDataKeyedColumns17;
	int __DFUDataKeyedColumns17_nSizeIs;
	DFUDataColumn *DFUDataKeyedColumns18;
	int __DFUDataKeyedColumns18_nSizeIs;
	DFUDataColumn *DFUDataKeyedColumns19;
	int __DFUDataKeyedColumns19_nSizeIs;
	DFUDataColumn *DFUDataKeyedColumns20;
	int __DFUDataKeyedColumns20_nSizeIs;
	DFUDataColumn *DFUDataNonKeyedColumns1;
	int __DFUDataNonKeyedColumns1_nSizeIs;
	DFUDataColumn *DFUDataNonKeyedColumns2;
	int __DFUDataNonKeyedColumns2_nSizeIs;
	DFUDataColumn *DFUDataNonKeyedColumns3;
	int __DFUDataNonKeyedColumns3_nSizeIs;
	DFUDataColumn *DFUDataNonKeyedColumns4;
	int __DFUDataNonKeyedColumns4_nSizeIs;
	DFUDataColumn *DFUDataNonKeyedColumns5;
	int __DFUDataNonKeyedColumns5_nSizeIs;
	DFUDataColumn *DFUDataNonKeyedColumns6;
	int __DFUDataNonKeyedColumns6_nSizeIs;
	DFUDataColumn *DFUDataNonKeyedColumns7;
	int __DFUDataNonKeyedColumns7_nSizeIs;
	DFUDataColumn *DFUDataNonKeyedColumns8;
	int __DFUDataNonKeyedColumns8_nSizeIs;
	DFUDataColumn *DFUDataNonKeyedColumns9;
	int __DFUDataNonKeyedColumns9_nSizeIs;
	DFUDataColumn *DFUDataNonKeyedColumns10;
	int __DFUDataNonKeyedColumns10_nSizeIs;
	DFUDataColumn *DFUDataNonKeyedColumns11;
	int __DFUDataNonKeyedColumns11_nSizeIs;
	DFUDataColumn *DFUDataNonKeyedColumns12;
	int __DFUDataNonKeyedColumns12_nSizeIs;
	DFUDataColumn *DFUDataNonKeyedColumns13;
	int __DFUDataNonKeyedColumns13_nSizeIs;
	DFUDataColumn *DFUDataNonKeyedColumns14;
	int __DFUDataNonKeyedColumns14_nSizeIs;
	DFUDataColumn *DFUDataNonKeyedColumns15;
	int __DFUDataNonKeyedColumns15_nSizeIs;
	DFUDataColumn *DFUDataNonKeyedColumns16;
	int __DFUDataNonKeyedColumns16_nSizeIs;
	DFUDataColumn *DFUDataNonKeyedColumns17;
	int __DFUDataNonKeyedColumns17_nSizeIs;
	DFUDataColumn *DFUDataNonKeyedColumns18;
	int __DFUDataNonKeyedColumns18_nSizeIs;
	DFUDataColumn *DFUDataNonKeyedColumns19;
	int __DFUDataNonKeyedColumns19_nSizeIs;
	DFUDataColumn *DFUDataNonKeyedColumns20;
	int __DFUDataNonKeyedColumns20_nSizeIs;
	__int64 RowCount;
};

extern __declspec(selectany) const _soapmapentry __CWsDfu_DFUGetDataColumns_entries[] =
{

	{
		0x743A238E, 
		"OpenLogicalName", 
		L"OpenLogicalName", 
		sizeof("OpenLogicalName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, OpenLogicalName),
		NULL,
		NULL,
		-1,
	},
	{
		0xCC052EFC, 
		"LogicalName", 
		L"LogicalName", 
		sizeof("LogicalName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, LogicalName),
		NULL,
		NULL,
		-1,
	},
	{
		0x30FCAF41, 
		"FilterBy", 
		L"FilterBy", 
		sizeof("FilterBy")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, FilterBy),
		NULL,
		NULL,
		-1,
	},
	{
		0x66378EE2, 
		"ShowColumns", 
		L"ShowColumns", 
		sizeof("ShowColumns")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, ShowColumns),
		NULL,
		NULL,
		-1,
	},
	{
		0x45EAA401, 
		"ChooseFile", 
		L"ChooseFile", 
		sizeof("ChooseFile")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, ChooseFile),
		NULL,
		NULL,
		-1,
	},
	{
		0x29F147E2, 
		"Cluster", 
		L"Cluster", 
		sizeof("Cluster")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, Cluster),
		NULL,
		NULL,
		-1,
	},
	{
		0xCF668A44, 
		"ClusterType", 
		L"ClusterType", 
		sizeof("ClusterType")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, ClusterType),
		NULL,
		NULL,
		-1,
	},
	{
		0xF82E67A6, 
		"StartIndex", 
		L"StartIndex", 
		sizeof("StartIndex")-1, 
		SOAPTYPE_LONG, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, StartIndex),
		NULL,
		NULL,
		-1,
	},
	{
		0xAFEF5FCF, 
		"EndIndex", 
		L"EndIndex", 
		sizeof("EndIndex")-1, 
		SOAPTYPE_LONG, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, EndIndex),
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
		offsetof(__CWsDfu_DFUGetDataColumns_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0xA1D0505D, 
		"DFUDataKeyedColumns1", 
		L"DFUDataKeyedColumns1", 
		sizeof("DFUDataKeyedColumns1")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, DFUDataKeyedColumns1),
		NULL,
		&__DFUDataColumn_map,
		10+1,
	},
	{
		0xA1D0505D,
		"__DFUDataKeyedColumns1_nSizeIs",
		L"__DFUDataKeyedColumns1_nSizeIs",
		sizeof("__DFUDataKeyedColumns1_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, __DFUDataKeyedColumns1_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xA1D0505E, 
		"DFUDataKeyedColumns2", 
		L"DFUDataKeyedColumns2", 
		sizeof("DFUDataKeyedColumns2")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, DFUDataKeyedColumns2),
		NULL,
		&__DFUDataColumn_map,
		12+1,
	},
	{
		0xA1D0505E,
		"__DFUDataKeyedColumns2_nSizeIs",
		L"__DFUDataKeyedColumns2_nSizeIs",
		sizeof("__DFUDataKeyedColumns2_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, __DFUDataKeyedColumns2_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xA1D0505F, 
		"DFUDataKeyedColumns3", 
		L"DFUDataKeyedColumns3", 
		sizeof("DFUDataKeyedColumns3")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, DFUDataKeyedColumns3),
		NULL,
		&__DFUDataColumn_map,
		14+1,
	},
	{
		0xA1D0505F,
		"__DFUDataKeyedColumns3_nSizeIs",
		L"__DFUDataKeyedColumns3_nSizeIs",
		sizeof("__DFUDataKeyedColumns3_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, __DFUDataKeyedColumns3_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xA1D05060, 
		"DFUDataKeyedColumns4", 
		L"DFUDataKeyedColumns4", 
		sizeof("DFUDataKeyedColumns4")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, DFUDataKeyedColumns4),
		NULL,
		&__DFUDataColumn_map,
		16+1,
	},
	{
		0xA1D05060,
		"__DFUDataKeyedColumns4_nSizeIs",
		L"__DFUDataKeyedColumns4_nSizeIs",
		sizeof("__DFUDataKeyedColumns4_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, __DFUDataKeyedColumns4_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xA1D05061, 
		"DFUDataKeyedColumns5", 
		L"DFUDataKeyedColumns5", 
		sizeof("DFUDataKeyedColumns5")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, DFUDataKeyedColumns5),
		NULL,
		&__DFUDataColumn_map,
		18+1,
	},
	{
		0xA1D05061,
		"__DFUDataKeyedColumns5_nSizeIs",
		L"__DFUDataKeyedColumns5_nSizeIs",
		sizeof("__DFUDataKeyedColumns5_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, __DFUDataKeyedColumns5_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xA1D05062, 
		"DFUDataKeyedColumns6", 
		L"DFUDataKeyedColumns6", 
		sizeof("DFUDataKeyedColumns6")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, DFUDataKeyedColumns6),
		NULL,
		&__DFUDataColumn_map,
		20+1,
	},
	{
		0xA1D05062,
		"__DFUDataKeyedColumns6_nSizeIs",
		L"__DFUDataKeyedColumns6_nSizeIs",
		sizeof("__DFUDataKeyedColumns6_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, __DFUDataKeyedColumns6_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xA1D05063, 
		"DFUDataKeyedColumns7", 
		L"DFUDataKeyedColumns7", 
		sizeof("DFUDataKeyedColumns7")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, DFUDataKeyedColumns7),
		NULL,
		&__DFUDataColumn_map,
		22+1,
	},
	{
		0xA1D05063,
		"__DFUDataKeyedColumns7_nSizeIs",
		L"__DFUDataKeyedColumns7_nSizeIs",
		sizeof("__DFUDataKeyedColumns7_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, __DFUDataKeyedColumns7_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xA1D05064, 
		"DFUDataKeyedColumns8", 
		L"DFUDataKeyedColumns8", 
		sizeof("DFUDataKeyedColumns8")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, DFUDataKeyedColumns8),
		NULL,
		&__DFUDataColumn_map,
		24+1,
	},
	{
		0xA1D05064,
		"__DFUDataKeyedColumns8_nSizeIs",
		L"__DFUDataKeyedColumns8_nSizeIs",
		sizeof("__DFUDataKeyedColumns8_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, __DFUDataKeyedColumns8_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xA1D05065, 
		"DFUDataKeyedColumns9", 
		L"DFUDataKeyedColumns9", 
		sizeof("DFUDataKeyedColumns9")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, DFUDataKeyedColumns9),
		NULL,
		&__DFUDataColumn_map,
		26+1,
	},
	{
		0xA1D05065,
		"__DFUDataKeyedColumns9_nSizeIs",
		L"__DFUDataKeyedColumns9_nSizeIs",
		sizeof("__DFUDataKeyedColumns9_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, __DFUDataKeyedColumns9_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xDBDA5C2D, 
		"DFUDataKeyedColumns10", 
		L"DFUDataKeyedColumns10", 
		sizeof("DFUDataKeyedColumns10")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, DFUDataKeyedColumns10),
		NULL,
		&__DFUDataColumn_map,
		28+1,
	},
	{
		0xDBDA5C2D,
		"__DFUDataKeyedColumns10_nSizeIs",
		L"__DFUDataKeyedColumns10_nSizeIs",
		sizeof("__DFUDataKeyedColumns10_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, __DFUDataKeyedColumns10_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xDBDA5C2E, 
		"DFUDataKeyedColumns11", 
		L"DFUDataKeyedColumns11", 
		sizeof("DFUDataKeyedColumns11")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, DFUDataKeyedColumns11),
		NULL,
		&__DFUDataColumn_map,
		30+1,
	},
	{
		0xDBDA5C2E,
		"__DFUDataKeyedColumns11_nSizeIs",
		L"__DFUDataKeyedColumns11_nSizeIs",
		sizeof("__DFUDataKeyedColumns11_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, __DFUDataKeyedColumns11_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xDBDA5C2F, 
		"DFUDataKeyedColumns12", 
		L"DFUDataKeyedColumns12", 
		sizeof("DFUDataKeyedColumns12")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, DFUDataKeyedColumns12),
		NULL,
		&__DFUDataColumn_map,
		32+1,
	},
	{
		0xDBDA5C2F,
		"__DFUDataKeyedColumns12_nSizeIs",
		L"__DFUDataKeyedColumns12_nSizeIs",
		sizeof("__DFUDataKeyedColumns12_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, __DFUDataKeyedColumns12_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xDBDA5C30, 
		"DFUDataKeyedColumns13", 
		L"DFUDataKeyedColumns13", 
		sizeof("DFUDataKeyedColumns13")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, DFUDataKeyedColumns13),
		NULL,
		&__DFUDataColumn_map,
		34+1,
	},
	{
		0xDBDA5C30,
		"__DFUDataKeyedColumns13_nSizeIs",
		L"__DFUDataKeyedColumns13_nSizeIs",
		sizeof("__DFUDataKeyedColumns13_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, __DFUDataKeyedColumns13_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xDBDA5C31, 
		"DFUDataKeyedColumns14", 
		L"DFUDataKeyedColumns14", 
		sizeof("DFUDataKeyedColumns14")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, DFUDataKeyedColumns14),
		NULL,
		&__DFUDataColumn_map,
		36+1,
	},
	{
		0xDBDA5C31,
		"__DFUDataKeyedColumns14_nSizeIs",
		L"__DFUDataKeyedColumns14_nSizeIs",
		sizeof("__DFUDataKeyedColumns14_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, __DFUDataKeyedColumns14_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xDBDA5C32, 
		"DFUDataKeyedColumns15", 
		L"DFUDataKeyedColumns15", 
		sizeof("DFUDataKeyedColumns15")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, DFUDataKeyedColumns15),
		NULL,
		&__DFUDataColumn_map,
		38+1,
	},
	{
		0xDBDA5C32,
		"__DFUDataKeyedColumns15_nSizeIs",
		L"__DFUDataKeyedColumns15_nSizeIs",
		sizeof("__DFUDataKeyedColumns15_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, __DFUDataKeyedColumns15_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xDBDA5C33, 
		"DFUDataKeyedColumns16", 
		L"DFUDataKeyedColumns16", 
		sizeof("DFUDataKeyedColumns16")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, DFUDataKeyedColumns16),
		NULL,
		&__DFUDataColumn_map,
		40+1,
	},
	{
		0xDBDA5C33,
		"__DFUDataKeyedColumns16_nSizeIs",
		L"__DFUDataKeyedColumns16_nSizeIs",
		sizeof("__DFUDataKeyedColumns16_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, __DFUDataKeyedColumns16_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xDBDA5C34, 
		"DFUDataKeyedColumns17", 
		L"DFUDataKeyedColumns17", 
		sizeof("DFUDataKeyedColumns17")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, DFUDataKeyedColumns17),
		NULL,
		&__DFUDataColumn_map,
		42+1,
	},
	{
		0xDBDA5C34,
		"__DFUDataKeyedColumns17_nSizeIs",
		L"__DFUDataKeyedColumns17_nSizeIs",
		sizeof("__DFUDataKeyedColumns17_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, __DFUDataKeyedColumns17_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xDBDA5C35, 
		"DFUDataKeyedColumns18", 
		L"DFUDataKeyedColumns18", 
		sizeof("DFUDataKeyedColumns18")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, DFUDataKeyedColumns18),
		NULL,
		&__DFUDataColumn_map,
		44+1,
	},
	{
		0xDBDA5C35,
		"__DFUDataKeyedColumns18_nSizeIs",
		L"__DFUDataKeyedColumns18_nSizeIs",
		sizeof("__DFUDataKeyedColumns18_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, __DFUDataKeyedColumns18_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xDBDA5C36, 
		"DFUDataKeyedColumns19", 
		L"DFUDataKeyedColumns19", 
		sizeof("DFUDataKeyedColumns19")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, DFUDataKeyedColumns19),
		NULL,
		&__DFUDataColumn_map,
		46+1,
	},
	{
		0xDBDA5C36,
		"__DFUDataKeyedColumns19_nSizeIs",
		L"__DFUDataKeyedColumns19_nSizeIs",
		sizeof("__DFUDataKeyedColumns19_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, __DFUDataKeyedColumns19_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xDBDA5C4E, 
		"DFUDataKeyedColumns20", 
		L"DFUDataKeyedColumns20", 
		sizeof("DFUDataKeyedColumns20")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, DFUDataKeyedColumns20),
		NULL,
		&__DFUDataColumn_map,
		48+1,
	},
	{
		0xDBDA5C4E,
		"__DFUDataKeyedColumns20_nSizeIs",
		L"__DFUDataKeyedColumns20_nSizeIs",
		sizeof("__DFUDataKeyedColumns20_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, __DFUDataKeyedColumns20_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x1CCB8E28, 
		"DFUDataNonKeyedColumns1", 
		L"DFUDataNonKeyedColumns1", 
		sizeof("DFUDataNonKeyedColumns1")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, DFUDataNonKeyedColumns1),
		NULL,
		&__DFUDataColumn_map,
		50+1,
	},
	{
		0x1CCB8E28,
		"__DFUDataNonKeyedColumns1_nSizeIs",
		L"__DFUDataNonKeyedColumns1_nSizeIs",
		sizeof("__DFUDataNonKeyedColumns1_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, __DFUDataNonKeyedColumns1_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x1CCB8E29, 
		"DFUDataNonKeyedColumns2", 
		L"DFUDataNonKeyedColumns2", 
		sizeof("DFUDataNonKeyedColumns2")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, DFUDataNonKeyedColumns2),
		NULL,
		&__DFUDataColumn_map,
		52+1,
	},
	{
		0x1CCB8E29,
		"__DFUDataNonKeyedColumns2_nSizeIs",
		L"__DFUDataNonKeyedColumns2_nSizeIs",
		sizeof("__DFUDataNonKeyedColumns2_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, __DFUDataNonKeyedColumns2_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x1CCB8E2A, 
		"DFUDataNonKeyedColumns3", 
		L"DFUDataNonKeyedColumns3", 
		sizeof("DFUDataNonKeyedColumns3")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, DFUDataNonKeyedColumns3),
		NULL,
		&__DFUDataColumn_map,
		54+1,
	},
	{
		0x1CCB8E2A,
		"__DFUDataNonKeyedColumns3_nSizeIs",
		L"__DFUDataNonKeyedColumns3_nSizeIs",
		sizeof("__DFUDataNonKeyedColumns3_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, __DFUDataNonKeyedColumns3_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x1CCB8E2B, 
		"DFUDataNonKeyedColumns4", 
		L"DFUDataNonKeyedColumns4", 
		sizeof("DFUDataNonKeyedColumns4")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, DFUDataNonKeyedColumns4),
		NULL,
		&__DFUDataColumn_map,
		56+1,
	},
	{
		0x1CCB8E2B,
		"__DFUDataNonKeyedColumns4_nSizeIs",
		L"__DFUDataNonKeyedColumns4_nSizeIs",
		sizeof("__DFUDataNonKeyedColumns4_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, __DFUDataNonKeyedColumns4_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x1CCB8E2C, 
		"DFUDataNonKeyedColumns5", 
		L"DFUDataNonKeyedColumns5", 
		sizeof("DFUDataNonKeyedColumns5")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, DFUDataNonKeyedColumns5),
		NULL,
		&__DFUDataColumn_map,
		58+1,
	},
	{
		0x1CCB8E2C,
		"__DFUDataNonKeyedColumns5_nSizeIs",
		L"__DFUDataNonKeyedColumns5_nSizeIs",
		sizeof("__DFUDataNonKeyedColumns5_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, __DFUDataNonKeyedColumns5_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x1CCB8E2D, 
		"DFUDataNonKeyedColumns6", 
		L"DFUDataNonKeyedColumns6", 
		sizeof("DFUDataNonKeyedColumns6")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, DFUDataNonKeyedColumns6),
		NULL,
		&__DFUDataColumn_map,
		60+1,
	},
	{
		0x1CCB8E2D,
		"__DFUDataNonKeyedColumns6_nSizeIs",
		L"__DFUDataNonKeyedColumns6_nSizeIs",
		sizeof("__DFUDataNonKeyedColumns6_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, __DFUDataNonKeyedColumns6_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x1CCB8E2E, 
		"DFUDataNonKeyedColumns7", 
		L"DFUDataNonKeyedColumns7", 
		sizeof("DFUDataNonKeyedColumns7")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, DFUDataNonKeyedColumns7),
		NULL,
		&__DFUDataColumn_map,
		62+1,
	},
	{
		0x1CCB8E2E,
		"__DFUDataNonKeyedColumns7_nSizeIs",
		L"__DFUDataNonKeyedColumns7_nSizeIs",
		sizeof("__DFUDataNonKeyedColumns7_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, __DFUDataNonKeyedColumns7_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x1CCB8E2F, 
		"DFUDataNonKeyedColumns8", 
		L"DFUDataNonKeyedColumns8", 
		sizeof("DFUDataNonKeyedColumns8")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, DFUDataNonKeyedColumns8),
		NULL,
		&__DFUDataColumn_map,
		64+1,
	},
	{
		0x1CCB8E2F,
		"__DFUDataNonKeyedColumns8_nSizeIs",
		L"__DFUDataNonKeyedColumns8_nSizeIs",
		sizeof("__DFUDataNonKeyedColumns8_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, __DFUDataNonKeyedColumns8_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x1CCB8E30, 
		"DFUDataNonKeyedColumns9", 
		L"DFUDataNonKeyedColumns9", 
		sizeof("DFUDataNonKeyedColumns9")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, DFUDataNonKeyedColumns9),
		NULL,
		&__DFUDataColumn_map,
		66+1,
	},
	{
		0x1CCB8E30,
		"__DFUDataNonKeyedColumns9_nSizeIs",
		L"__DFUDataNonKeyedColumns9_nSizeIs",
		sizeof("__DFUDataNonKeyedColumns9_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, __DFUDataNonKeyedColumns9_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xB63D5358, 
		"DFUDataNonKeyedColumns10", 
		L"DFUDataNonKeyedColumns10", 
		sizeof("DFUDataNonKeyedColumns10")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, DFUDataNonKeyedColumns10),
		NULL,
		&__DFUDataColumn_map,
		68+1,
	},
	{
		0xB63D5358,
		"__DFUDataNonKeyedColumns10_nSizeIs",
		L"__DFUDataNonKeyedColumns10_nSizeIs",
		sizeof("__DFUDataNonKeyedColumns10_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, __DFUDataNonKeyedColumns10_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xB63D5359, 
		"DFUDataNonKeyedColumns11", 
		L"DFUDataNonKeyedColumns11", 
		sizeof("DFUDataNonKeyedColumns11")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, DFUDataNonKeyedColumns11),
		NULL,
		&__DFUDataColumn_map,
		70+1,
	},
	{
		0xB63D5359,
		"__DFUDataNonKeyedColumns11_nSizeIs",
		L"__DFUDataNonKeyedColumns11_nSizeIs",
		sizeof("__DFUDataNonKeyedColumns11_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, __DFUDataNonKeyedColumns11_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xB63D535A, 
		"DFUDataNonKeyedColumns12", 
		L"DFUDataNonKeyedColumns12", 
		sizeof("DFUDataNonKeyedColumns12")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, DFUDataNonKeyedColumns12),
		NULL,
		&__DFUDataColumn_map,
		72+1,
	},
	{
		0xB63D535A,
		"__DFUDataNonKeyedColumns12_nSizeIs",
		L"__DFUDataNonKeyedColumns12_nSizeIs",
		sizeof("__DFUDataNonKeyedColumns12_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, __DFUDataNonKeyedColumns12_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xB63D535B, 
		"DFUDataNonKeyedColumns13", 
		L"DFUDataNonKeyedColumns13", 
		sizeof("DFUDataNonKeyedColumns13")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, DFUDataNonKeyedColumns13),
		NULL,
		&__DFUDataColumn_map,
		74+1,
	},
	{
		0xB63D535B,
		"__DFUDataNonKeyedColumns13_nSizeIs",
		L"__DFUDataNonKeyedColumns13_nSizeIs",
		sizeof("__DFUDataNonKeyedColumns13_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, __DFUDataNonKeyedColumns13_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xB63D535C, 
		"DFUDataNonKeyedColumns14", 
		L"DFUDataNonKeyedColumns14", 
		sizeof("DFUDataNonKeyedColumns14")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, DFUDataNonKeyedColumns14),
		NULL,
		&__DFUDataColumn_map,
		76+1,
	},
	{
		0xB63D535C,
		"__DFUDataNonKeyedColumns14_nSizeIs",
		L"__DFUDataNonKeyedColumns14_nSizeIs",
		sizeof("__DFUDataNonKeyedColumns14_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, __DFUDataNonKeyedColumns14_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xB63D535D, 
		"DFUDataNonKeyedColumns15", 
		L"DFUDataNonKeyedColumns15", 
		sizeof("DFUDataNonKeyedColumns15")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, DFUDataNonKeyedColumns15),
		NULL,
		&__DFUDataColumn_map,
		78+1,
	},
	{
		0xB63D535D,
		"__DFUDataNonKeyedColumns15_nSizeIs",
		L"__DFUDataNonKeyedColumns15_nSizeIs",
		sizeof("__DFUDataNonKeyedColumns15_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, __DFUDataNonKeyedColumns15_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xB63D535E, 
		"DFUDataNonKeyedColumns16", 
		L"DFUDataNonKeyedColumns16", 
		sizeof("DFUDataNonKeyedColumns16")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, DFUDataNonKeyedColumns16),
		NULL,
		&__DFUDataColumn_map,
		80+1,
	},
	{
		0xB63D535E,
		"__DFUDataNonKeyedColumns16_nSizeIs",
		L"__DFUDataNonKeyedColumns16_nSizeIs",
		sizeof("__DFUDataNonKeyedColumns16_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, __DFUDataNonKeyedColumns16_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xB63D535F, 
		"DFUDataNonKeyedColumns17", 
		L"DFUDataNonKeyedColumns17", 
		sizeof("DFUDataNonKeyedColumns17")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, DFUDataNonKeyedColumns17),
		NULL,
		&__DFUDataColumn_map,
		82+1,
	},
	{
		0xB63D535F,
		"__DFUDataNonKeyedColumns17_nSizeIs",
		L"__DFUDataNonKeyedColumns17_nSizeIs",
		sizeof("__DFUDataNonKeyedColumns17_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, __DFUDataNonKeyedColumns17_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xB63D5360, 
		"DFUDataNonKeyedColumns18", 
		L"DFUDataNonKeyedColumns18", 
		sizeof("DFUDataNonKeyedColumns18")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, DFUDataNonKeyedColumns18),
		NULL,
		&__DFUDataColumn_map,
		84+1,
	},
	{
		0xB63D5360,
		"__DFUDataNonKeyedColumns18_nSizeIs",
		L"__DFUDataNonKeyedColumns18_nSizeIs",
		sizeof("__DFUDataNonKeyedColumns18_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, __DFUDataNonKeyedColumns18_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xB63D5361, 
		"DFUDataNonKeyedColumns19", 
		L"DFUDataNonKeyedColumns19", 
		sizeof("DFUDataNonKeyedColumns19")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, DFUDataNonKeyedColumns19),
		NULL,
		&__DFUDataColumn_map,
		86+1,
	},
	{
		0xB63D5361,
		"__DFUDataNonKeyedColumns19_nSizeIs",
		L"__DFUDataNonKeyedColumns19_nSizeIs",
		sizeof("__DFUDataNonKeyedColumns19_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, __DFUDataNonKeyedColumns19_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xB63D5379, 
		"DFUDataNonKeyedColumns20", 
		L"DFUDataNonKeyedColumns20", 
		sizeof("DFUDataNonKeyedColumns20")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, DFUDataNonKeyedColumns20),
		NULL,
		&__DFUDataColumn_map,
		88+1,
	},
	{
		0xB63D5379,
		"__DFUDataNonKeyedColumns20_nSizeIs",
		L"__DFUDataNonKeyedColumns20_nSizeIs",
		sizeof("__DFUDataNonKeyedColumns20_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, __DFUDataNonKeyedColumns20_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x2827B2C1, 
		"RowCount", 
		L"RowCount", 
		sizeof("RowCount")-1, 
		SOAPTYPE_LONG, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUGetDataColumns_struct, RowCount),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsDfu_DFUGetDataColumns_map =
{
	0xAA083E69,
	"DFUGetDataColumnsRequest",
	L"DFUGetDataColumnsResponse",
	sizeof("DFUGetDataColumnsRequest")-1,
	sizeof("DFUGetDataColumnsResponse")-1,
	SOAPMAP_FUNC,
	__CWsDfu_DFUGetDataColumns_entries,
	sizeof(__CWsDfu_DFUGetDataColumns_struct),
	49,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x18AC14AC,
	"http://webservices.seisint.com/WsDfu",
	L"http://webservices.seisint.com/WsDfu",
	sizeof("http://webservices.seisint.com/WsDfu")-1
};


struct __CWsDfu_DFUBrowseData_struct
{
	BSTR LogicalName;
	BSTR FilterBy;
	BSTR ShowColumns;
	bool SchemaOnly;
	__int64 StartForGoback;
	int CountForGoback;
	int ChooseFile;
	BSTR Cluster;
	BSTR ClusterType;
	BSTR ParentName;
	__int64 Start;
	int Count;
	ArrayOfEspException Exceptions;
	BSTR Name;
	BSTR FilterForGoBack;
	DFUDataColumn *ColumnsHidden;
	int __ColumnsHidden_nSizeIs;
	int ColumnCount;
	int PageSize;
	__int64 Total;
	BSTR Result;
	BSTR MsgToDisplay;
};

extern __declspec(selectany) const _soapmapentry __CWsDfu_DFUBrowseData_entries[] =
{

	{
		0xCC052EFC, 
		"LogicalName", 
		L"LogicalName", 
		sizeof("LogicalName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUBrowseData_struct, LogicalName),
		NULL,
		NULL,
		-1,
	},
	{
		0x30FCAF41, 
		"FilterBy", 
		L"FilterBy", 
		sizeof("FilterBy")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUBrowseData_struct, FilterBy),
		NULL,
		NULL,
		-1,
	},
	{
		0x66378EE2, 
		"ShowColumns", 
		L"ShowColumns", 
		sizeof("ShowColumns")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUBrowseData_struct, ShowColumns),
		NULL,
		NULL,
		-1,
	},
	{
		0xBA4FFE53, 
		"SchemaOnly", 
		L"SchemaOnly", 
		sizeof("SchemaOnly")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUBrowseData_struct, SchemaOnly),
		NULL,
		NULL,
		-1,
	},
	{
		0x30DB669C, 
		"StartForGoback", 
		L"StartForGoback", 
		sizeof("StartForGoback")-1, 
		SOAPTYPE_LONG, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUBrowseData_struct, StartForGoback),
		NULL,
		NULL,
		-1,
	},
	{
		0xAD8D1397, 
		"CountForGoback", 
		L"CountForGoback", 
		sizeof("CountForGoback")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUBrowseData_struct, CountForGoback),
		NULL,
		NULL,
		-1,
	},
	{
		0x45EAA401, 
		"ChooseFile", 
		L"ChooseFile", 
		sizeof("ChooseFile")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUBrowseData_struct, ChooseFile),
		NULL,
		NULL,
		-1,
	},
	{
		0x29F147E2, 
		"Cluster", 
		L"Cluster", 
		sizeof("Cluster")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUBrowseData_struct, Cluster),
		NULL,
		NULL,
		-1,
	},
	{
		0xCF668A44, 
		"ClusterType", 
		L"ClusterType", 
		sizeof("ClusterType")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUBrowseData_struct, ClusterType),
		NULL,
		NULL,
		-1,
	},
	{
		0x0375F04B, 
		"ParentName", 
		L"ParentName", 
		sizeof("ParentName")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUBrowseData_struct, ParentName),
		NULL,
		NULL,
		-1,
	},
	{
		0x061F398E, 
		"Start", 
		L"Start", 
		sizeof("Start")-1, 
		SOAPTYPE_LONG, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUBrowseData_struct, Start),
		NULL,
		NULL,
		-1,
	},
	{
		0x04FB4829, 
		"Count", 
		L"Count", 
		sizeof("Count")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUBrowseData_struct, Count),
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
		offsetof(__CWsDfu_DFUBrowseData_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0x002C70A1, 
		"Name", 
		L"Name", 
		sizeof("Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUBrowseData_struct, Name),
		NULL,
		NULL,
		-1,
	},
	{
		0x47241CB4, 
		"FilterForGoBack", 
		L"FilterForGoBack", 
		sizeof("FilterForGoBack")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUBrowseData_struct, FilterForGoBack),
		NULL,
		NULL,
		-1,
	},
	{
		0x7C9CAD0D, 
		"ColumnsHidden", 
		L"ColumnsHidden", 
		sizeof("ColumnsHidden")-1, 
		SOAPTYPE_UNK, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUBrowseData_struct, ColumnsHidden),
		NULL,
		&__DFUDataColumn_map,
		15+1,
	},
	{
		0x7C9CAD0D,
		"__ColumnsHidden_nSizeIs",
		L"__ColumnsHidden_nSizeIs",
		sizeof("__ColumnsHidden_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUBrowseData_struct, __ColumnsHidden_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x928CF017, 
		"ColumnCount", 
		L"ColumnCount", 
		sizeof("ColumnCount")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUBrowseData_struct, ColumnCount),
		NULL,
		NULL,
		-1,
	},
	{
		0xF6F9E158, 
		"PageSize", 
		L"PageSize", 
		sizeof("PageSize")-1, 
		SOAPTYPE_INT, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUBrowseData_struct, PageSize),
		NULL,
		NULL,
		-1,
	},
	{
		0x062EE2C4, 
		"Total", 
		L"Total", 
		sizeof("Total")-1, 
		SOAPTYPE_LONG, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUBrowseData_struct, Total),
		NULL,
		NULL,
		-1,
	},
	{
		0xC6ABBE1F, 
		"Result", 
		L"Result", 
		sizeof("Result")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUBrowseData_struct, Result),
		NULL,
		NULL,
		-1,
	},
	{
		0x5B3F5AA0, 
		"MsgToDisplay", 
		L"MsgToDisplay", 
		sizeof("MsgToDisplay")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUBrowseData_struct, MsgToDisplay),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsDfu_DFUBrowseData_map =
{
	0x99F1A3BA,
	"DFUBrowseDataRequest",
	L"DFUBrowseDataResponse",
	sizeof("DFUBrowseDataRequest")-1,
	sizeof("DFUBrowseDataResponse")-1,
	SOAPMAP_FUNC,
	__CWsDfu_DFUBrowseData_entries,
	sizeof(__CWsDfu_DFUBrowseData_struct),
	20,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x18AC14AC,
	"http://webservices.seisint.com/WsDfu",
	L"http://webservices.seisint.com/WsDfu",
	sizeof("http://webservices.seisint.com/WsDfu")-1
};


struct __CWsDfu_DFUDefFile_struct
{
	BSTR Name;
	BSTR Format;
	ArrayOfEspException Exceptions;
	ATLSOAP_BLOB defFile;
};

extern __declspec(selectany) const _soapmapentry __CWsDfu_DFUDefFile_entries[] =
{

	{
		0x002C70A1, 
		"Name", 
		L"Name", 
		sizeof("Name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUDefFile_struct, Name),
		NULL,
		NULL,
		-1,
	},
	{
		0xAB621BC9, 
		"Format", 
		L"Format", 
		sizeof("Format")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUDefFile_struct, Format),
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
		offsetof(__CWsDfu_DFUDefFile_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0x04B6B5CF, 
		"defFile", 
		L"defFile", 
		sizeof("defFile")-1, 
		SOAPTYPE_BASE64BINARY, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUDefFile_struct, defFile),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsDfu_DFUDefFile_map =
{
	0xB1A2AF9D,
	"DFUDefFileRequest",
	L"DFUDefFileResponse",
	sizeof("DFUDefFileRequest")-1,
	sizeof("DFUDefFileResponse")-1,
	SOAPMAP_FUNC,
	__CWsDfu_DFUDefFile_entries,
	sizeof(__CWsDfu_DFUDefFile_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x18AC14AC,
	"http://webservices.seisint.com/WsDfu",
	L"http://webservices.seisint.com/WsDfu",
	sizeof("http://webservices.seisint.com/WsDfu")-1
};


struct __CWsDfu_DFUArrayAction_struct
{
	BSTR Type;
	bool NoDelete;
	BSTR BackToPage;
	BSTR *LogicalFiles;
	int __LogicalFiles_nSizeIs;
	ArrayOfEspException Exceptions;
	BSTR DFUArrayActionResult;
};

extern __declspec(selectany) const _soapmapentry __CWsDfu_DFUArrayAction_entries[] =
{

	{
		0x00302162, 
		"Type", 
		L"Type", 
		sizeof("Type")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUArrayAction_struct, Type),
		NULL,
		NULL,
		-1,
	},
	{
		0x028E9FD0, 
		"NoDelete", 
		L"NoDelete", 
		sizeof("NoDelete")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUArrayAction_struct, NoDelete),
		NULL,
		NULL,
		-1,
	},
	{
		0x83E470F1, 
		"BackToPage", 
		L"BackToPage", 
		sizeof("BackToPage")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUArrayAction_struct, BackToPage),
		NULL,
		NULL,
		-1,
	},
	{
		0x4C1EA9AE, 
		"LogicalFiles", 
		L"LogicalFiles", 
		sizeof("LogicalFiles")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUArrayAction_struct, LogicalFiles),
		NULL,
		NULL,
		3+1,
	},
	{
		0x4C1EA9AE,
		"__LogicalFiles_nSizeIs",
		L"__LogicalFiles_nSizeIs",
		sizeof("__LogicalFiles_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_DFUArrayAction_struct, __LogicalFiles_nSizeIs),
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
		offsetof(__CWsDfu_DFUArrayAction_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0xF0BD219B, 
		"DFUArrayActionResult", 
		L"DFUArrayActionResult", 
		sizeof("DFUArrayActionResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_DFUArrayAction_struct, DFUArrayActionResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsDfu_DFUArrayAction_map =
{
	0x143314AB,
	"DFUArrayActionRequest",
	L"DFUArrayActionResponse",
	sizeof("DFUArrayActionRequest")-1,
	sizeof("DFUArrayActionResponse")-1,
	SOAPMAP_FUNC,
	__CWsDfu_DFUArrayAction_entries,
	sizeof(__CWsDfu_DFUArrayAction_struct),
	3,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x18AC14AC,
	"http://webservices.seisint.com/WsDfu",
	L"http://webservices.seisint.com/WsDfu",
	sizeof("http://webservices.seisint.com/WsDfu")-1
};


struct __CWsDfu_AddtoSuperfile_struct
{
	BSTR Superfile;
	BSTR Subfiles;
	BSTR *names;
	int __names_nSizeIs;
	bool ExistingFile;
	ArrayOfEspException Exceptions;
	BSTR *SubfileNames;
	int __SubfileNames_nSizeIs;
};

extern __declspec(selectany) const _soapmapentry __CWsDfu_AddtoSuperfile_entries[] =
{

	{
		0x5FCB2F2F, 
		"Superfile", 
		L"Superfile", 
		sizeof("Superfile")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_AddtoSuperfile_struct, Superfile),
		NULL,
		NULL,
		-1,
	},
	{
		0xB3C086DD, 
		"Subfiles", 
		L"Subfiles", 
		sizeof("Subfiles")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_AddtoSuperfile_struct, Subfiles),
		NULL,
		NULL,
		-1,
	},
	{
		0x07FD9554, 
		"names", 
		L"names", 
		sizeof("names")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_AddtoSuperfile_struct, names),
		NULL,
		NULL,
		2+1,
	},
	{
		0x07FD9554,
		"__names_nSizeIs",
		L"__names_nSizeIs",
		sizeof("__names_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_AddtoSuperfile_struct, __names_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0x59C84D4B, 
		"ExistingFile", 
		L"ExistingFile", 
		sizeof("ExistingFile")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_AddtoSuperfile_struct, ExistingFile),
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
		offsetof(__CWsDfu_AddtoSuperfile_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0x19CC309E, 
		"SubfileNames", 
		L"SubfileNames", 
		sizeof("SubfileNames")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_AddtoSuperfile_struct, SubfileNames),
		NULL,
		NULL,
		6+1,
	},
	{
		0x19CC309E,
		"__SubfileNames_nSizeIs",
		L"__SubfileNames_nSizeIs",
		sizeof("__SubfileNames_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_AddtoSuperfile_struct, __SubfileNames_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsDfu_AddtoSuperfile_map =
{
	0x3B4BDD4A,
	"AddtoSuperfileRequest",
	L"AddtoSuperfileResponse",
	sizeof("AddtoSuperfileRequest")-1,
	sizeof("AddtoSuperfileResponse")-1,
	SOAPMAP_FUNC,
	__CWsDfu_AddtoSuperfile_entries,
	sizeof(__CWsDfu_AddtoSuperfile_struct),
	3,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x18AC14AC,
	"http://webservices.seisint.com/WsDfu",
	L"http://webservices.seisint.com/WsDfu",
	sizeof("http://webservices.seisint.com/WsDfu")-1
};


struct __CWsDfu_Savexml_struct
{
	BSTR name;
	ArrayOfEspException Exceptions;
	ATLSOAP_BLOB xmlmap;
};

extern __declspec(selectany) const _soapmapentry __CWsDfu_Savexml_entries[] =
{

	{
		0x003DFCC1, 
		"name", 
		L"name", 
		sizeof("name")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_Savexml_struct, name),
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
		offsetof(__CWsDfu_Savexml_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{
		0x1FDC8FEF, 
		"xmlmap", 
		L"xmlmap", 
		sizeof("xmlmap")-1, 
		SOAPTYPE_BASE64BINARY, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_Savexml_struct, xmlmap),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsDfu_Savexml_map =
{
	0xC9FFF46F,
	"SavexmlRequest",
	L"SavexmlResponse",
	sizeof("SavexmlRequest")-1,
	sizeof("SavexmlResponse")-1,
	SOAPMAP_FUNC,
	__CWsDfu_Savexml_entries,
	sizeof(__CWsDfu_Savexml_struct),
	2,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x18AC14AC,
	"http://webservices.seisint.com/WsDfu",
	L"http://webservices.seisint.com/WsDfu",
	sizeof("http://webservices.seisint.com/WsDfu")-1
};


struct __CWsDfu_SuperfileAddRaw_struct
{
	BSTR superfile;
	BSTR subfiles;
	BSTR before;
	ArrayOfEspException Exceptions;
	int retcode;
};

extern __declspec(selectany) const _soapmapentry __CWsDfu_SuperfileAddRaw_entries[] =
{

	{
		0xEF594F4F, 
		"superfile", 
		L"superfile", 
		sizeof("superfile")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_SuperfileAddRaw_struct, superfile),
		NULL,
		NULL,
		-1,
	},
	{
		0x3BFB22FD, 
		"subfiles", 
		L"subfiles", 
		sizeof("subfiles")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_SuperfileAddRaw_struct, subfiles),
		NULL,
		NULL,
		-1,
	},
	{
		0xEBF70E73, 
		"before", 
		L"before", 
		sizeof("before")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_SuperfileAddRaw_struct, before),
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
		offsetof(__CWsDfu_SuperfileAddRaw_struct, Exceptions),
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
		offsetof(__CWsDfu_SuperfileAddRaw_struct, retcode),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsDfu_SuperfileAddRaw_map =
{
	0x6F93DD91,
	"SuperfileAddRawRequest",
	L"SuperfileAddRawResponse",
	sizeof("SuperfileAddRawRequest")-1,
	sizeof("SuperfileAddRawResponse")-1,
	SOAPMAP_FUNC,
	__CWsDfu_SuperfileAddRaw_entries,
	sizeof(__CWsDfu_SuperfileAddRaw_struct),
	3,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x18AC14AC,
	"http://webservices.seisint.com/WsDfu",
	L"http://webservices.seisint.com/WsDfu",
	sizeof("http://webservices.seisint.com/WsDfu")-1
};


struct __CWsDfu_SuperfileAction_struct
{
	BSTR action;
	BSTR superfile;
	BSTR *subfiles;
	int __subfiles_nSizeIs;
	BSTR before;
	bool ___delete;
	ArrayOfEspException Exceptions;
	int retcode;
};

extern __declspec(selectany) const _soapmapentry __CWsDfu_SuperfileAction_entries[] =
{

	{
		0xE985483E, 
		"action", 
		L"action", 
		sizeof("action")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_SuperfileAction_struct, action),
		NULL,
		NULL,
		-1,
	},
	{
		0xEF594F4F, 
		"superfile", 
		L"superfile", 
		sizeof("superfile")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_SuperfileAction_struct, superfile),
		NULL,
		NULL,
		-1,
	},
	{
		0x3BFB22FD, 
		"subfiles", 
		L"subfiles", 
		sizeof("subfiles")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_DYNARR | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_SuperfileAction_struct, subfiles),
		NULL,
		NULL,
		2+1,
	},
	{
		0x3BFB22FD,
		"__subfiles_nSizeIs",
		L"__subfiles_nSizeIs",
		sizeof("__subfiles_nSizeIs")-1,
		SOAPTYPE_INT,
		SOAPFLAG_NOMARSHAL,
		offsetof(__CWsDfu_SuperfileAction_struct, __subfiles_nSizeIs),
		NULL,
		NULL,
		-1
	},
	{
		0xEBF70E73, 
		"before", 
		L"before", 
		sizeof("before")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_SuperfileAction_struct, before),
		NULL,
		NULL,
		-1,
	},
	{
		0xF0A47FB3, 
		"delete", 
		L"delete", 
		sizeof("delete")-1, 
		SOAPTYPE_BOOLEAN, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_SuperfileAction_struct, ___delete),
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
		offsetof(__CWsDfu_SuperfileAction_struct, Exceptions),
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
		offsetof(__CWsDfu_SuperfileAction_struct, retcode),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsDfu_SuperfileAction_map =
{
	0x4AF2E9BC,
	"SuperfileActionRequest",
	L"SuperfileActionResponse",
	sizeof("SuperfileActionRequest")-1,
	sizeof("SuperfileActionResponse")-1,
	SOAPMAP_FUNC,
	__CWsDfu_SuperfileAction_entries,
	sizeof(__CWsDfu_SuperfileAction_struct),
	3,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x18AC14AC,
	"http://webservices.seisint.com/WsDfu",
	L"http://webservices.seisint.com/WsDfu",
	sizeof("http://webservices.seisint.com/WsDfu")-1
};


struct __CWsDfu_Add_struct
{
	BSTR dstname;
	ATLSOAP_BLOB xmlmap;
	ArrayOfEspException Exceptions;
};

extern __declspec(selectany) const _soapmapentry __CWsDfu_Add_entries[] =
{

	{
		0x267212EC, 
		"dstname", 
		L"dstname", 
		sizeof("dstname")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_Add_struct, dstname),
		NULL,
		NULL,
		-1,
	},
	{
		0x1FDC8FEF, 
		"xmlmap", 
		L"xmlmap", 
		sizeof("xmlmap")-1, 
		SOAPTYPE_BASE64BINARY, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CWsDfu_Add_struct, xmlmap),
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
		offsetof(__CWsDfu_Add_struct, Exceptions),
		NULL,
		&__ArrayOfEspException_map,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CWsDfu_Add_map =
{
	0x1181BCF8,
	"AddRequest",
	L"AddResponse",
	sizeof("AddRequest")-1,
	sizeof("AddResponse")-1,
	SOAPMAP_FUNC,
	__CWsDfu_Add_entries,
	sizeof(__CWsDfu_Add_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0x18AC14AC,
	"http://webservices.seisint.com/WsDfu",
	L"http://webservices.seisint.com/WsDfu",
	sizeof("http://webservices.seisint.com/WsDfu")-1
};

extern __declspec(selectany) const _soapmap * __CWsDfu_funcs[] =
{
	&__CWsDfu_DFUSearchData_map,
	&__CWsDfu_AddRemote_map,
	&__CWsDfu_SuperfileList_map,
	&__CWsDfu_DFUSearch_map,
	&__CWsDfu_DFUFileView_map,
	&__CWsDfu_DFUSpace_map,
	&__CWsDfu_DFUInfo_map,
	&__CWsDfu_DFUQuery_map,
	&__CWsDfu_DFUGetDataColumns_map,
	&__CWsDfu_DFUBrowseData_map,
	&__CWsDfu_DFUDefFile_map,
	&__CWsDfu_DFUArrayAction_map,
	&__CWsDfu_AddtoSuperfile_map,
	&__CWsDfu_Savexml_map,
	&__CWsDfu_SuperfileAddRaw_map,
	&__CWsDfu_SuperfileAction_map,
	&__CWsDfu_Add_map,
	NULL
};

template <typename TClient>
inline HRESULT CWsDfuT<TClient>::DFUSearchData(
		BSTR* Cluster, 
		BSTR* ClusterType, 
		BSTR* OpenLogicalName, 
		BSTR* FilterBy, 
		BSTR* ShowColumns, 
		int* ChooseFile, 
		__int64* StartIndex, 
		__int64* EndIndex, 
		BSTR* LogicalName, 
		BSTR* ParentName, 
		__int64* StartForGoback, 
		int* CountForGoback, 
		__int64* Start, 
		int* Count, 
		BSTR* File, 
		BSTR* Key, 
		bool* SchemaOnly, 
		ArrayOfEspException* Exceptions, 
		DFUDataColumn** DFUDataKeyedColumns1, int* __DFUDataKeyedColumns1_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns2, int* __DFUDataKeyedColumns2_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns3, int* __DFUDataKeyedColumns3_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns4, int* __DFUDataKeyedColumns4_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns5, int* __DFUDataKeyedColumns5_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns6, int* __DFUDataKeyedColumns6_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns7, int* __DFUDataKeyedColumns7_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns8, int* __DFUDataKeyedColumns8_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns9, int* __DFUDataKeyedColumns9_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns10, int* __DFUDataKeyedColumns10_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns11, int* __DFUDataKeyedColumns11_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns12, int* __DFUDataKeyedColumns12_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns13, int* __DFUDataKeyedColumns13_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns14, int* __DFUDataKeyedColumns14_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns15, int* __DFUDataKeyedColumns15_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns16, int* __DFUDataKeyedColumns16_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns17, int* __DFUDataKeyedColumns17_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns18, int* __DFUDataKeyedColumns18_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns19, int* __DFUDataKeyedColumns19_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns20, int* __DFUDataKeyedColumns20_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns1, int* __DFUDataNonKeyedColumns1_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns2, int* __DFUDataNonKeyedColumns2_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns3, int* __DFUDataNonKeyedColumns3_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns4, int* __DFUDataNonKeyedColumns4_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns5, int* __DFUDataNonKeyedColumns5_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns6, int* __DFUDataNonKeyedColumns6_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns7, int* __DFUDataNonKeyedColumns7_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns8, int* __DFUDataNonKeyedColumns8_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns9, int* __DFUDataNonKeyedColumns9_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns10, int* __DFUDataNonKeyedColumns10_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns11, int* __DFUDataNonKeyedColumns11_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns12, int* __DFUDataNonKeyedColumns12_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns13, int* __DFUDataNonKeyedColumns13_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns14, int* __DFUDataNonKeyedColumns14_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns15, int* __DFUDataNonKeyedColumns15_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns16, int* __DFUDataNonKeyedColumns16_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns17, int* __DFUDataNonKeyedColumns17_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns18, int* __DFUDataNonKeyedColumns18_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns19, int* __DFUDataNonKeyedColumns19_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns20, int* __DFUDataNonKeyedColumns20_nSizeIs, 
		__int64* RowCount, 
		BSTR* Name, 
		BSTR* FilterForGoBack, 
		DFUDataColumn** ColumnsHidden, int* __ColumnsHidden_nSizeIs, 
		int* ColumnCount, 
		int* PageSize, 
		__int64* Total, 
		BSTR* Result, 
		BSTR* MsgToDisplay
	)
{
    if ( Cluster == NULL )
		return E_POINTER;
    if ( ClusterType == NULL )
		return E_POINTER;
    if ( OpenLogicalName == NULL )
		return E_POINTER;
    if ( FilterBy == NULL )
		return E_POINTER;
    if ( ShowColumns == NULL )
		return E_POINTER;
    if ( ChooseFile == NULL )
		return E_POINTER;
    if ( StartIndex == NULL )
		return E_POINTER;
    if ( EndIndex == NULL )
		return E_POINTER;
    if ( LogicalName == NULL )
		return E_POINTER;
    if ( ParentName == NULL )
		return E_POINTER;
    if ( StartForGoback == NULL )
		return E_POINTER;
    if ( CountForGoback == NULL )
		return E_POINTER;
    if ( Start == NULL )
		return E_POINTER;
    if ( Count == NULL )
		return E_POINTER;
    if ( File == NULL )
		return E_POINTER;
    if ( Key == NULL )
		return E_POINTER;
    if ( SchemaOnly == NULL )
		return E_POINTER;
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( DFUDataKeyedColumns1 == NULL )
		return E_POINTER;
	if( __DFUDataKeyedColumns1_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataKeyedColumns2 == NULL )
		return E_POINTER;
	if( __DFUDataKeyedColumns2_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataKeyedColumns3 == NULL )
		return E_POINTER;
	if( __DFUDataKeyedColumns3_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataKeyedColumns4 == NULL )
		return E_POINTER;
	if( __DFUDataKeyedColumns4_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataKeyedColumns5 == NULL )
		return E_POINTER;
	if( __DFUDataKeyedColumns5_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataKeyedColumns6 == NULL )
		return E_POINTER;
	if( __DFUDataKeyedColumns6_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataKeyedColumns7 == NULL )
		return E_POINTER;
	if( __DFUDataKeyedColumns7_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataKeyedColumns8 == NULL )
		return E_POINTER;
	if( __DFUDataKeyedColumns8_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataKeyedColumns9 == NULL )
		return E_POINTER;
	if( __DFUDataKeyedColumns9_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataKeyedColumns10 == NULL )
		return E_POINTER;
	if( __DFUDataKeyedColumns10_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataKeyedColumns11 == NULL )
		return E_POINTER;
	if( __DFUDataKeyedColumns11_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataKeyedColumns12 == NULL )
		return E_POINTER;
	if( __DFUDataKeyedColumns12_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataKeyedColumns13 == NULL )
		return E_POINTER;
	if( __DFUDataKeyedColumns13_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataKeyedColumns14 == NULL )
		return E_POINTER;
	if( __DFUDataKeyedColumns14_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataKeyedColumns15 == NULL )
		return E_POINTER;
	if( __DFUDataKeyedColumns15_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataKeyedColumns16 == NULL )
		return E_POINTER;
	if( __DFUDataKeyedColumns16_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataKeyedColumns17 == NULL )
		return E_POINTER;
	if( __DFUDataKeyedColumns17_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataKeyedColumns18 == NULL )
		return E_POINTER;
	if( __DFUDataKeyedColumns18_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataKeyedColumns19 == NULL )
		return E_POINTER;
	if( __DFUDataKeyedColumns19_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataKeyedColumns20 == NULL )
		return E_POINTER;
	if( __DFUDataKeyedColumns20_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataNonKeyedColumns1 == NULL )
		return E_POINTER;
	if( __DFUDataNonKeyedColumns1_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataNonKeyedColumns2 == NULL )
		return E_POINTER;
	if( __DFUDataNonKeyedColumns2_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataNonKeyedColumns3 == NULL )
		return E_POINTER;
	if( __DFUDataNonKeyedColumns3_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataNonKeyedColumns4 == NULL )
		return E_POINTER;
	if( __DFUDataNonKeyedColumns4_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataNonKeyedColumns5 == NULL )
		return E_POINTER;
	if( __DFUDataNonKeyedColumns5_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataNonKeyedColumns6 == NULL )
		return E_POINTER;
	if( __DFUDataNonKeyedColumns6_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataNonKeyedColumns7 == NULL )
		return E_POINTER;
	if( __DFUDataNonKeyedColumns7_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataNonKeyedColumns8 == NULL )
		return E_POINTER;
	if( __DFUDataNonKeyedColumns8_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataNonKeyedColumns9 == NULL )
		return E_POINTER;
	if( __DFUDataNonKeyedColumns9_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataNonKeyedColumns10 == NULL )
		return E_POINTER;
	if( __DFUDataNonKeyedColumns10_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataNonKeyedColumns11 == NULL )
		return E_POINTER;
	if( __DFUDataNonKeyedColumns11_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataNonKeyedColumns12 == NULL )
		return E_POINTER;
	if( __DFUDataNonKeyedColumns12_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataNonKeyedColumns13 == NULL )
		return E_POINTER;
	if( __DFUDataNonKeyedColumns13_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataNonKeyedColumns14 == NULL )
		return E_POINTER;
	if( __DFUDataNonKeyedColumns14_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataNonKeyedColumns15 == NULL )
		return E_POINTER;
	if( __DFUDataNonKeyedColumns15_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataNonKeyedColumns16 == NULL )
		return E_POINTER;
	if( __DFUDataNonKeyedColumns16_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataNonKeyedColumns17 == NULL )
		return E_POINTER;
	if( __DFUDataNonKeyedColumns17_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataNonKeyedColumns18 == NULL )
		return E_POINTER;
	if( __DFUDataNonKeyedColumns18_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataNonKeyedColumns19 == NULL )
		return E_POINTER;
	if( __DFUDataNonKeyedColumns19_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataNonKeyedColumns20 == NULL )
		return E_POINTER;
	if( __DFUDataNonKeyedColumns20_nSizeIs == NULL )
		return E_POINTER;
    if ( RowCount == NULL )
		return E_POINTER;
    if ( Name == NULL )
		return E_POINTER;
    if ( FilterForGoBack == NULL )
		return E_POINTER;
    if ( ColumnsHidden == NULL )
		return E_POINTER;
	if( __ColumnsHidden_nSizeIs == NULL )
		return E_POINTER;
    if ( ColumnCount == NULL )
		return E_POINTER;
    if ( PageSize == NULL )
		return E_POINTER;
    if ( Total == NULL )
		return E_POINTER;
    if ( Result == NULL )
		return E_POINTER;
    if ( MsgToDisplay == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsDfu_DFUSearchData_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Cluster = *Cluster;
	__params.ClusterType = *ClusterType;
	__params.OpenLogicalName = *OpenLogicalName;
	__params.FilterBy = *FilterBy;
	__params.ShowColumns = *ShowColumns;
	__params.ChooseFile = *ChooseFile;
	__params.StartIndex = *StartIndex;
	__params.EndIndex = *EndIndex;
	__params.LogicalName = *LogicalName;
	__params.ParentName = *ParentName;
	__params.StartForGoback = *StartForGoback;
	__params.CountForGoback = *CountForGoback;
	__params.Start = *Start;
	__params.Count = *Count;
	__params.File = *File;
	__params.Key = *Key;
	__params.SchemaOnly = *SchemaOnly;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsDfu/DFUSearchData\?ver_=1.16\"\r\n"));
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

	*Cluster = __params.Cluster;
	*ClusterType = __params.ClusterType;
	*OpenLogicalName = __params.OpenLogicalName;
	*FilterBy = __params.FilterBy;
	*ShowColumns = __params.ShowColumns;
	*ChooseFile = __params.ChooseFile;
	*StartIndex = __params.StartIndex;
	*EndIndex = __params.EndIndex;
	*LogicalName = __params.LogicalName;
	*ParentName = __params.ParentName;
	*StartForGoback = __params.StartForGoback;
	*CountForGoback = __params.CountForGoback;
	*Start = __params.Start;
	*Count = __params.Count;
	*File = __params.File;
	*Key = __params.Key;
	*SchemaOnly = __params.SchemaOnly;
	*Exceptions = __params.Exceptions;
	*DFUDataKeyedColumns1 = __params.DFUDataKeyedColumns1;
	*__DFUDataKeyedColumns1_nSizeIs = __params.__DFUDataKeyedColumns1_nSizeIs;
	*DFUDataKeyedColumns2 = __params.DFUDataKeyedColumns2;
	*__DFUDataKeyedColumns2_nSizeIs = __params.__DFUDataKeyedColumns2_nSizeIs;
	*DFUDataKeyedColumns3 = __params.DFUDataKeyedColumns3;
	*__DFUDataKeyedColumns3_nSizeIs = __params.__DFUDataKeyedColumns3_nSizeIs;
	*DFUDataKeyedColumns4 = __params.DFUDataKeyedColumns4;
	*__DFUDataKeyedColumns4_nSizeIs = __params.__DFUDataKeyedColumns4_nSizeIs;
	*DFUDataKeyedColumns5 = __params.DFUDataKeyedColumns5;
	*__DFUDataKeyedColumns5_nSizeIs = __params.__DFUDataKeyedColumns5_nSizeIs;
	*DFUDataKeyedColumns6 = __params.DFUDataKeyedColumns6;
	*__DFUDataKeyedColumns6_nSizeIs = __params.__DFUDataKeyedColumns6_nSizeIs;
	*DFUDataKeyedColumns7 = __params.DFUDataKeyedColumns7;
	*__DFUDataKeyedColumns7_nSizeIs = __params.__DFUDataKeyedColumns7_nSizeIs;
	*DFUDataKeyedColumns8 = __params.DFUDataKeyedColumns8;
	*__DFUDataKeyedColumns8_nSizeIs = __params.__DFUDataKeyedColumns8_nSizeIs;
	*DFUDataKeyedColumns9 = __params.DFUDataKeyedColumns9;
	*__DFUDataKeyedColumns9_nSizeIs = __params.__DFUDataKeyedColumns9_nSizeIs;
	*DFUDataKeyedColumns10 = __params.DFUDataKeyedColumns10;
	*__DFUDataKeyedColumns10_nSizeIs = __params.__DFUDataKeyedColumns10_nSizeIs;
	*DFUDataKeyedColumns11 = __params.DFUDataKeyedColumns11;
	*__DFUDataKeyedColumns11_nSizeIs = __params.__DFUDataKeyedColumns11_nSizeIs;
	*DFUDataKeyedColumns12 = __params.DFUDataKeyedColumns12;
	*__DFUDataKeyedColumns12_nSizeIs = __params.__DFUDataKeyedColumns12_nSizeIs;
	*DFUDataKeyedColumns13 = __params.DFUDataKeyedColumns13;
	*__DFUDataKeyedColumns13_nSizeIs = __params.__DFUDataKeyedColumns13_nSizeIs;
	*DFUDataKeyedColumns14 = __params.DFUDataKeyedColumns14;
	*__DFUDataKeyedColumns14_nSizeIs = __params.__DFUDataKeyedColumns14_nSizeIs;
	*DFUDataKeyedColumns15 = __params.DFUDataKeyedColumns15;
	*__DFUDataKeyedColumns15_nSizeIs = __params.__DFUDataKeyedColumns15_nSizeIs;
	*DFUDataKeyedColumns16 = __params.DFUDataKeyedColumns16;
	*__DFUDataKeyedColumns16_nSizeIs = __params.__DFUDataKeyedColumns16_nSizeIs;
	*DFUDataKeyedColumns17 = __params.DFUDataKeyedColumns17;
	*__DFUDataKeyedColumns17_nSizeIs = __params.__DFUDataKeyedColumns17_nSizeIs;
	*DFUDataKeyedColumns18 = __params.DFUDataKeyedColumns18;
	*__DFUDataKeyedColumns18_nSizeIs = __params.__DFUDataKeyedColumns18_nSizeIs;
	*DFUDataKeyedColumns19 = __params.DFUDataKeyedColumns19;
	*__DFUDataKeyedColumns19_nSizeIs = __params.__DFUDataKeyedColumns19_nSizeIs;
	*DFUDataKeyedColumns20 = __params.DFUDataKeyedColumns20;
	*__DFUDataKeyedColumns20_nSizeIs = __params.__DFUDataKeyedColumns20_nSizeIs;
	*DFUDataNonKeyedColumns1 = __params.DFUDataNonKeyedColumns1;
	*__DFUDataNonKeyedColumns1_nSizeIs = __params.__DFUDataNonKeyedColumns1_nSizeIs;
	*DFUDataNonKeyedColumns2 = __params.DFUDataNonKeyedColumns2;
	*__DFUDataNonKeyedColumns2_nSizeIs = __params.__DFUDataNonKeyedColumns2_nSizeIs;
	*DFUDataNonKeyedColumns3 = __params.DFUDataNonKeyedColumns3;
	*__DFUDataNonKeyedColumns3_nSizeIs = __params.__DFUDataNonKeyedColumns3_nSizeIs;
	*DFUDataNonKeyedColumns4 = __params.DFUDataNonKeyedColumns4;
	*__DFUDataNonKeyedColumns4_nSizeIs = __params.__DFUDataNonKeyedColumns4_nSizeIs;
	*DFUDataNonKeyedColumns5 = __params.DFUDataNonKeyedColumns5;
	*__DFUDataNonKeyedColumns5_nSizeIs = __params.__DFUDataNonKeyedColumns5_nSizeIs;
	*DFUDataNonKeyedColumns6 = __params.DFUDataNonKeyedColumns6;
	*__DFUDataNonKeyedColumns6_nSizeIs = __params.__DFUDataNonKeyedColumns6_nSizeIs;
	*DFUDataNonKeyedColumns7 = __params.DFUDataNonKeyedColumns7;
	*__DFUDataNonKeyedColumns7_nSizeIs = __params.__DFUDataNonKeyedColumns7_nSizeIs;
	*DFUDataNonKeyedColumns8 = __params.DFUDataNonKeyedColumns8;
	*__DFUDataNonKeyedColumns8_nSizeIs = __params.__DFUDataNonKeyedColumns8_nSizeIs;
	*DFUDataNonKeyedColumns9 = __params.DFUDataNonKeyedColumns9;
	*__DFUDataNonKeyedColumns9_nSizeIs = __params.__DFUDataNonKeyedColumns9_nSizeIs;
	*DFUDataNonKeyedColumns10 = __params.DFUDataNonKeyedColumns10;
	*__DFUDataNonKeyedColumns10_nSizeIs = __params.__DFUDataNonKeyedColumns10_nSizeIs;
	*DFUDataNonKeyedColumns11 = __params.DFUDataNonKeyedColumns11;
	*__DFUDataNonKeyedColumns11_nSizeIs = __params.__DFUDataNonKeyedColumns11_nSizeIs;
	*DFUDataNonKeyedColumns12 = __params.DFUDataNonKeyedColumns12;
	*__DFUDataNonKeyedColumns12_nSizeIs = __params.__DFUDataNonKeyedColumns12_nSizeIs;
	*DFUDataNonKeyedColumns13 = __params.DFUDataNonKeyedColumns13;
	*__DFUDataNonKeyedColumns13_nSizeIs = __params.__DFUDataNonKeyedColumns13_nSizeIs;
	*DFUDataNonKeyedColumns14 = __params.DFUDataNonKeyedColumns14;
	*__DFUDataNonKeyedColumns14_nSizeIs = __params.__DFUDataNonKeyedColumns14_nSizeIs;
	*DFUDataNonKeyedColumns15 = __params.DFUDataNonKeyedColumns15;
	*__DFUDataNonKeyedColumns15_nSizeIs = __params.__DFUDataNonKeyedColumns15_nSizeIs;
	*DFUDataNonKeyedColumns16 = __params.DFUDataNonKeyedColumns16;
	*__DFUDataNonKeyedColumns16_nSizeIs = __params.__DFUDataNonKeyedColumns16_nSizeIs;
	*DFUDataNonKeyedColumns17 = __params.DFUDataNonKeyedColumns17;
	*__DFUDataNonKeyedColumns17_nSizeIs = __params.__DFUDataNonKeyedColumns17_nSizeIs;
	*DFUDataNonKeyedColumns18 = __params.DFUDataNonKeyedColumns18;
	*__DFUDataNonKeyedColumns18_nSizeIs = __params.__DFUDataNonKeyedColumns18_nSizeIs;
	*DFUDataNonKeyedColumns19 = __params.DFUDataNonKeyedColumns19;
	*__DFUDataNonKeyedColumns19_nSizeIs = __params.__DFUDataNonKeyedColumns19_nSizeIs;
	*DFUDataNonKeyedColumns20 = __params.DFUDataNonKeyedColumns20;
	*__DFUDataNonKeyedColumns20_nSizeIs = __params.__DFUDataNonKeyedColumns20_nSizeIs;
	*RowCount = __params.RowCount;
	*Name = __params.Name;
	*FilterForGoBack = __params.FilterForGoBack;
	*ColumnsHidden = __params.ColumnsHidden;
	*__ColumnsHidden_nSizeIs = __params.__ColumnsHidden_nSizeIs;
	*ColumnCount = __params.ColumnCount;
	*PageSize = __params.PageSize;
	*Total = __params.Total;
	*Result = __params.Result;
	*MsgToDisplay = __params.MsgToDisplay;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsDfuT<TClient>::AddRemote(
		BSTR dstname, 
		BSTR srcname, 
		BSTR srcdali, 
		BSTR srcusername, 
		BSTR srcpassword, 
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
	__CWsDfu_AddRemote_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.dstname = dstname;
	__params.srcname = srcname;
	__params.srcdali = srcdali;
	__params.srcusername = srcusername;
	__params.srcpassword = srcpassword;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsDfu/AddRemote\?ver_=1.16\"\r\n"));
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
inline HRESULT CWsDfuT<TClient>::SuperfileList(
		BSTR* superfile, 
		ArrayOfEspException* Exceptions, 
		BSTR** subfiles, int* __subfiles_nSizeIs
	)
{
    if ( superfile == NULL )
		return E_POINTER;
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( subfiles == NULL )
		return E_POINTER;
	if( __subfiles_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsDfu_SuperfileList_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.superfile = *superfile;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsDfu/SuperfileList\?ver_=1.16\"\r\n"));
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

	*superfile = __params.superfile;
	*Exceptions = __params.Exceptions;
	*subfiles = __params.subfiles;
	*__subfiles_nSizeIs = __params.__subfiles_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsDfuT<TClient>::DFUSearch(
		BSTR* ShowExample, 
		ArrayOfEspException* Exceptions, 
		BSTR** ClusterNames, int* __ClusterNames_nSizeIs, 
		BSTR** FileTypes, int* __FileTypes_nSizeIs
	)
{
    if ( ShowExample == NULL )
		return E_POINTER;
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( ClusterNames == NULL )
		return E_POINTER;
	if( __ClusterNames_nSizeIs == NULL )
		return E_POINTER;
    if ( FileTypes == NULL )
		return E_POINTER;
	if( __FileTypes_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsDfu_DFUSearch_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.ShowExample = *ShowExample;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsDfu/DFUSearch\?ver_=1.16\"\r\n"));
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

	*ShowExample = __params.ShowExample;
	*Exceptions = __params.Exceptions;
	*ClusterNames = __params.ClusterNames;
	*__ClusterNames_nSizeIs = __params.__ClusterNames_nSizeIs;
	*FileTypes = __params.FileTypes;
	*__FileTypes_nSizeIs = __params.__FileTypes_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsDfuT<TClient>::DFUFileView(
		BSTR* Scope, 
		ArrayOfEspException* Exceptions, 
		int* NumFiles, 
		DFULogicalFile** DFULogicalFiles, int* __DFULogicalFiles_nSizeIs
	)
{
    if ( Scope == NULL )
		return E_POINTER;
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( NumFiles == NULL )
		return E_POINTER;
    if ( DFULogicalFiles == NULL )
		return E_POINTER;
	if( __DFULogicalFiles_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsDfu_DFUFileView_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Scope = *Scope;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsDfu/DFUFileView\?ver_=1.16\"\r\n"));
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

	*Scope = __params.Scope;
	*Exceptions = __params.Exceptions;
	*NumFiles = __params.NumFiles;
	*DFULogicalFiles = __params.DFULogicalFiles;
	*__DFULogicalFiles_nSizeIs = __params.__DFULogicalFiles_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsDfuT<TClient>::DFUSpace(
		BSTR* CountBy, 
		BSTR* ScopeUnder, 
		BSTR* OwnerUnder, 
		BSTR* Interval, 
		BSTR* StartDate, 
		BSTR* EndDate, 
		ArrayOfEspException* Exceptions, 
		DFUSpaceItem** DFUSpaceItems, int* __DFUSpaceItems_nSizeIs
	)
{
    if ( CountBy == NULL )
		return E_POINTER;
    if ( ScopeUnder == NULL )
		return E_POINTER;
    if ( OwnerUnder == NULL )
		return E_POINTER;
    if ( Interval == NULL )
		return E_POINTER;
    if ( StartDate == NULL )
		return E_POINTER;
    if ( EndDate == NULL )
		return E_POINTER;
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( DFUSpaceItems == NULL )
		return E_POINTER;
	if( __DFUSpaceItems_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsDfu_DFUSpace_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.CountBy = *CountBy;
	__params.ScopeUnder = *ScopeUnder;
	__params.OwnerUnder = *OwnerUnder;
	__params.Interval = *Interval;
	__params.StartDate = *StartDate;
	__params.EndDate = *EndDate;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsDfu/DFUSpace\?ver_=1.16\"\r\n"));
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

	*CountBy = __params.CountBy;
	*ScopeUnder = __params.ScopeUnder;
	*OwnerUnder = __params.OwnerUnder;
	*Interval = __params.Interval;
	*StartDate = __params.StartDate;
	*EndDate = __params.EndDate;
	*Exceptions = __params.Exceptions;
	*DFUSpaceItems = __params.DFUSpaceItems;
	*__DFUSpaceItems_nSizeIs = __params.__DFUSpaceItems_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsDfuT<TClient>::DFUInfo(
		BSTR Name, 
		BSTR Cluster, 
		bool UpdateDescription, 
		BSTR FileName, 
		BSTR FileDesc, 
		ArrayOfEspException* Exceptions, 
		DFUFileDetail* FileDetail
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( FileDetail == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsDfu_DFUInfo_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Name = Name;
	__params.Cluster = Cluster;
	__params.UpdateDescription = UpdateDescription;
	__params.FileName = FileName;
	__params.FileDesc = FileDesc;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsDfu/DFUInfo\?ver_=1.16\"\r\n"));
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
	*FileDetail = __params.FileDetail;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsDfuT<TClient>::DFUQuery(
		BSTR* Prefix, 
		BSTR* ClusterName, 
		BSTR* LogicalName, 
		BSTR* Description, 
		BSTR* Owner, 
		BSTR* StartDate, 
		BSTR* EndDate, 
		BSTR* FileType, 
		__int64* FileSizeFrom, 
		__int64* FileSizeTo, 
		int* FirstN, 
		BSTR* FirstNType, 
		int* PageSize, 
		int* PageStartFrom, 
		BSTR* Sortby, 
		bool* Descending, 
		bool OneLevelDirFileReturn, 
		ArrayOfEspException* Exceptions, 
		DFULogicalFile** DFULogicalFiles, int* __DFULogicalFiles_nSizeIs, 
		int* LastPageFrom, 
		int* PageEndAt, 
		int* PrevPageFrom, 
		int* NextPageFrom, 
		int* NumFiles, 
		BSTR* BasicQuery, 
		BSTR* ParametersForPaging, 
		BSTR* Filters
	)
{
    if ( Prefix == NULL )
		return E_POINTER;
    if ( ClusterName == NULL )
		return E_POINTER;
    if ( LogicalName == NULL )
		return E_POINTER;
    if ( Description == NULL )
		return E_POINTER;
    if ( Owner == NULL )
		return E_POINTER;
    if ( StartDate == NULL )
		return E_POINTER;
    if ( EndDate == NULL )
		return E_POINTER;
    if ( FileType == NULL )
		return E_POINTER;
    if ( FileSizeFrom == NULL )
		return E_POINTER;
    if ( FileSizeTo == NULL )
		return E_POINTER;
    if ( FirstN == NULL )
		return E_POINTER;
    if ( FirstNType == NULL )
		return E_POINTER;
    if ( PageSize == NULL )
		return E_POINTER;
    if ( PageStartFrom == NULL )
		return E_POINTER;
    if ( Sortby == NULL )
		return E_POINTER;
    if ( Descending == NULL )
		return E_POINTER;
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( DFULogicalFiles == NULL )
		return E_POINTER;
	if( __DFULogicalFiles_nSizeIs == NULL )
		return E_POINTER;
    if ( LastPageFrom == NULL )
		return E_POINTER;
    if ( PageEndAt == NULL )
		return E_POINTER;
    if ( PrevPageFrom == NULL )
		return E_POINTER;
    if ( NextPageFrom == NULL )
		return E_POINTER;
    if ( NumFiles == NULL )
		return E_POINTER;
    if ( BasicQuery == NULL )
		return E_POINTER;
    if ( ParametersForPaging == NULL )
		return E_POINTER;
    if ( Filters == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsDfu_DFUQuery_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Prefix = *Prefix;
	__params.ClusterName = *ClusterName;
	__params.LogicalName = *LogicalName;
	__params.Description = *Description;
	__params.Owner = *Owner;
	__params.StartDate = *StartDate;
	__params.EndDate = *EndDate;
	__params.FileType = *FileType;
	__params.FileSizeFrom = *FileSizeFrom;
	__params.FileSizeTo = *FileSizeTo;
	__params.FirstN = *FirstN;
	__params.FirstNType = *FirstNType;
	__params.PageSize = *PageSize;
	__params.PageStartFrom = *PageStartFrom;
	__params.Sortby = *Sortby;
	__params.Descending = *Descending;
	__params.OneLevelDirFileReturn = OneLevelDirFileReturn;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsDfu/DFUQuery\?ver_=1.16\"\r\n"));
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

	*Prefix = __params.Prefix;
	*ClusterName = __params.ClusterName;
	*LogicalName = __params.LogicalName;
	*Description = __params.Description;
	*Owner = __params.Owner;
	*StartDate = __params.StartDate;
	*EndDate = __params.EndDate;
	*FileType = __params.FileType;
	*FileSizeFrom = __params.FileSizeFrom;
	*FileSizeTo = __params.FileSizeTo;
	*FirstN = __params.FirstN;
	*FirstNType = __params.FirstNType;
	*PageSize = __params.PageSize;
	*PageStartFrom = __params.PageStartFrom;
	*Sortby = __params.Sortby;
	*Descending = __params.Descending;
	*Exceptions = __params.Exceptions;
	*DFULogicalFiles = __params.DFULogicalFiles;
	*__DFULogicalFiles_nSizeIs = __params.__DFULogicalFiles_nSizeIs;
	*LastPageFrom = __params.LastPageFrom;
	*PageEndAt = __params.PageEndAt;
	*PrevPageFrom = __params.PrevPageFrom;
	*NextPageFrom = __params.NextPageFrom;
	*NumFiles = __params.NumFiles;
	*BasicQuery = __params.BasicQuery;
	*ParametersForPaging = __params.ParametersForPaging;
	*Filters = __params.Filters;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsDfuT<TClient>::DFUGetDataColumns(
		BSTR OpenLogicalName, 
		BSTR* LogicalName, 
		BSTR FilterBy, 
		BSTR* ShowColumns, 
		int* ChooseFile, 
		BSTR* Cluster, 
		BSTR* ClusterType, 
		__int64* StartIndex, 
		__int64* EndIndex, 
		ArrayOfEspException* Exceptions, 
		DFUDataColumn** DFUDataKeyedColumns1, int* __DFUDataKeyedColumns1_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns2, int* __DFUDataKeyedColumns2_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns3, int* __DFUDataKeyedColumns3_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns4, int* __DFUDataKeyedColumns4_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns5, int* __DFUDataKeyedColumns5_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns6, int* __DFUDataKeyedColumns6_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns7, int* __DFUDataKeyedColumns7_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns8, int* __DFUDataKeyedColumns8_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns9, int* __DFUDataKeyedColumns9_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns10, int* __DFUDataKeyedColumns10_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns11, int* __DFUDataKeyedColumns11_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns12, int* __DFUDataKeyedColumns12_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns13, int* __DFUDataKeyedColumns13_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns14, int* __DFUDataKeyedColumns14_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns15, int* __DFUDataKeyedColumns15_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns16, int* __DFUDataKeyedColumns16_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns17, int* __DFUDataKeyedColumns17_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns18, int* __DFUDataKeyedColumns18_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns19, int* __DFUDataKeyedColumns19_nSizeIs, 
		DFUDataColumn** DFUDataKeyedColumns20, int* __DFUDataKeyedColumns20_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns1, int* __DFUDataNonKeyedColumns1_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns2, int* __DFUDataNonKeyedColumns2_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns3, int* __DFUDataNonKeyedColumns3_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns4, int* __DFUDataNonKeyedColumns4_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns5, int* __DFUDataNonKeyedColumns5_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns6, int* __DFUDataNonKeyedColumns6_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns7, int* __DFUDataNonKeyedColumns7_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns8, int* __DFUDataNonKeyedColumns8_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns9, int* __DFUDataNonKeyedColumns9_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns10, int* __DFUDataNonKeyedColumns10_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns11, int* __DFUDataNonKeyedColumns11_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns12, int* __DFUDataNonKeyedColumns12_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns13, int* __DFUDataNonKeyedColumns13_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns14, int* __DFUDataNonKeyedColumns14_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns15, int* __DFUDataNonKeyedColumns15_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns16, int* __DFUDataNonKeyedColumns16_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns17, int* __DFUDataNonKeyedColumns17_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns18, int* __DFUDataNonKeyedColumns18_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns19, int* __DFUDataNonKeyedColumns19_nSizeIs, 
		DFUDataColumn** DFUDataNonKeyedColumns20, int* __DFUDataNonKeyedColumns20_nSizeIs, 
		__int64* RowCount
	)
{
    if ( LogicalName == NULL )
		return E_POINTER;
    if ( ShowColumns == NULL )
		return E_POINTER;
    if ( ChooseFile == NULL )
		return E_POINTER;
    if ( Cluster == NULL )
		return E_POINTER;
    if ( ClusterType == NULL )
		return E_POINTER;
    if ( StartIndex == NULL )
		return E_POINTER;
    if ( EndIndex == NULL )
		return E_POINTER;
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( DFUDataKeyedColumns1 == NULL )
		return E_POINTER;
	if( __DFUDataKeyedColumns1_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataKeyedColumns2 == NULL )
		return E_POINTER;
	if( __DFUDataKeyedColumns2_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataKeyedColumns3 == NULL )
		return E_POINTER;
	if( __DFUDataKeyedColumns3_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataKeyedColumns4 == NULL )
		return E_POINTER;
	if( __DFUDataKeyedColumns4_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataKeyedColumns5 == NULL )
		return E_POINTER;
	if( __DFUDataKeyedColumns5_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataKeyedColumns6 == NULL )
		return E_POINTER;
	if( __DFUDataKeyedColumns6_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataKeyedColumns7 == NULL )
		return E_POINTER;
	if( __DFUDataKeyedColumns7_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataKeyedColumns8 == NULL )
		return E_POINTER;
	if( __DFUDataKeyedColumns8_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataKeyedColumns9 == NULL )
		return E_POINTER;
	if( __DFUDataKeyedColumns9_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataKeyedColumns10 == NULL )
		return E_POINTER;
	if( __DFUDataKeyedColumns10_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataKeyedColumns11 == NULL )
		return E_POINTER;
	if( __DFUDataKeyedColumns11_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataKeyedColumns12 == NULL )
		return E_POINTER;
	if( __DFUDataKeyedColumns12_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataKeyedColumns13 == NULL )
		return E_POINTER;
	if( __DFUDataKeyedColumns13_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataKeyedColumns14 == NULL )
		return E_POINTER;
	if( __DFUDataKeyedColumns14_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataKeyedColumns15 == NULL )
		return E_POINTER;
	if( __DFUDataKeyedColumns15_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataKeyedColumns16 == NULL )
		return E_POINTER;
	if( __DFUDataKeyedColumns16_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataKeyedColumns17 == NULL )
		return E_POINTER;
	if( __DFUDataKeyedColumns17_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataKeyedColumns18 == NULL )
		return E_POINTER;
	if( __DFUDataKeyedColumns18_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataKeyedColumns19 == NULL )
		return E_POINTER;
	if( __DFUDataKeyedColumns19_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataKeyedColumns20 == NULL )
		return E_POINTER;
	if( __DFUDataKeyedColumns20_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataNonKeyedColumns1 == NULL )
		return E_POINTER;
	if( __DFUDataNonKeyedColumns1_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataNonKeyedColumns2 == NULL )
		return E_POINTER;
	if( __DFUDataNonKeyedColumns2_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataNonKeyedColumns3 == NULL )
		return E_POINTER;
	if( __DFUDataNonKeyedColumns3_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataNonKeyedColumns4 == NULL )
		return E_POINTER;
	if( __DFUDataNonKeyedColumns4_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataNonKeyedColumns5 == NULL )
		return E_POINTER;
	if( __DFUDataNonKeyedColumns5_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataNonKeyedColumns6 == NULL )
		return E_POINTER;
	if( __DFUDataNonKeyedColumns6_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataNonKeyedColumns7 == NULL )
		return E_POINTER;
	if( __DFUDataNonKeyedColumns7_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataNonKeyedColumns8 == NULL )
		return E_POINTER;
	if( __DFUDataNonKeyedColumns8_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataNonKeyedColumns9 == NULL )
		return E_POINTER;
	if( __DFUDataNonKeyedColumns9_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataNonKeyedColumns10 == NULL )
		return E_POINTER;
	if( __DFUDataNonKeyedColumns10_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataNonKeyedColumns11 == NULL )
		return E_POINTER;
	if( __DFUDataNonKeyedColumns11_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataNonKeyedColumns12 == NULL )
		return E_POINTER;
	if( __DFUDataNonKeyedColumns12_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataNonKeyedColumns13 == NULL )
		return E_POINTER;
	if( __DFUDataNonKeyedColumns13_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataNonKeyedColumns14 == NULL )
		return E_POINTER;
	if( __DFUDataNonKeyedColumns14_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataNonKeyedColumns15 == NULL )
		return E_POINTER;
	if( __DFUDataNonKeyedColumns15_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataNonKeyedColumns16 == NULL )
		return E_POINTER;
	if( __DFUDataNonKeyedColumns16_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataNonKeyedColumns17 == NULL )
		return E_POINTER;
	if( __DFUDataNonKeyedColumns17_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataNonKeyedColumns18 == NULL )
		return E_POINTER;
	if( __DFUDataNonKeyedColumns18_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataNonKeyedColumns19 == NULL )
		return E_POINTER;
	if( __DFUDataNonKeyedColumns19_nSizeIs == NULL )
		return E_POINTER;
    if ( DFUDataNonKeyedColumns20 == NULL )
		return E_POINTER;
	if( __DFUDataNonKeyedColumns20_nSizeIs == NULL )
		return E_POINTER;
    if ( RowCount == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsDfu_DFUGetDataColumns_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.OpenLogicalName = OpenLogicalName;
	__params.LogicalName = *LogicalName;
	__params.FilterBy = FilterBy;
	__params.ShowColumns = *ShowColumns;
	__params.ChooseFile = *ChooseFile;
	__params.Cluster = *Cluster;
	__params.ClusterType = *ClusterType;
	__params.StartIndex = *StartIndex;
	__params.EndIndex = *EndIndex;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsDfu/DFUGetDataColumns\?ver_=1.16\"\r\n"));
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

	*LogicalName = __params.LogicalName;
	*ShowColumns = __params.ShowColumns;
	*ChooseFile = __params.ChooseFile;
	*Cluster = __params.Cluster;
	*ClusterType = __params.ClusterType;
	*StartIndex = __params.StartIndex;
	*EndIndex = __params.EndIndex;
	*Exceptions = __params.Exceptions;
	*DFUDataKeyedColumns1 = __params.DFUDataKeyedColumns1;
	*__DFUDataKeyedColumns1_nSizeIs = __params.__DFUDataKeyedColumns1_nSizeIs;
	*DFUDataKeyedColumns2 = __params.DFUDataKeyedColumns2;
	*__DFUDataKeyedColumns2_nSizeIs = __params.__DFUDataKeyedColumns2_nSizeIs;
	*DFUDataKeyedColumns3 = __params.DFUDataKeyedColumns3;
	*__DFUDataKeyedColumns3_nSizeIs = __params.__DFUDataKeyedColumns3_nSizeIs;
	*DFUDataKeyedColumns4 = __params.DFUDataKeyedColumns4;
	*__DFUDataKeyedColumns4_nSizeIs = __params.__DFUDataKeyedColumns4_nSizeIs;
	*DFUDataKeyedColumns5 = __params.DFUDataKeyedColumns5;
	*__DFUDataKeyedColumns5_nSizeIs = __params.__DFUDataKeyedColumns5_nSizeIs;
	*DFUDataKeyedColumns6 = __params.DFUDataKeyedColumns6;
	*__DFUDataKeyedColumns6_nSizeIs = __params.__DFUDataKeyedColumns6_nSizeIs;
	*DFUDataKeyedColumns7 = __params.DFUDataKeyedColumns7;
	*__DFUDataKeyedColumns7_nSizeIs = __params.__DFUDataKeyedColumns7_nSizeIs;
	*DFUDataKeyedColumns8 = __params.DFUDataKeyedColumns8;
	*__DFUDataKeyedColumns8_nSizeIs = __params.__DFUDataKeyedColumns8_nSizeIs;
	*DFUDataKeyedColumns9 = __params.DFUDataKeyedColumns9;
	*__DFUDataKeyedColumns9_nSizeIs = __params.__DFUDataKeyedColumns9_nSizeIs;
	*DFUDataKeyedColumns10 = __params.DFUDataKeyedColumns10;
	*__DFUDataKeyedColumns10_nSizeIs = __params.__DFUDataKeyedColumns10_nSizeIs;
	*DFUDataKeyedColumns11 = __params.DFUDataKeyedColumns11;
	*__DFUDataKeyedColumns11_nSizeIs = __params.__DFUDataKeyedColumns11_nSizeIs;
	*DFUDataKeyedColumns12 = __params.DFUDataKeyedColumns12;
	*__DFUDataKeyedColumns12_nSizeIs = __params.__DFUDataKeyedColumns12_nSizeIs;
	*DFUDataKeyedColumns13 = __params.DFUDataKeyedColumns13;
	*__DFUDataKeyedColumns13_nSizeIs = __params.__DFUDataKeyedColumns13_nSizeIs;
	*DFUDataKeyedColumns14 = __params.DFUDataKeyedColumns14;
	*__DFUDataKeyedColumns14_nSizeIs = __params.__DFUDataKeyedColumns14_nSizeIs;
	*DFUDataKeyedColumns15 = __params.DFUDataKeyedColumns15;
	*__DFUDataKeyedColumns15_nSizeIs = __params.__DFUDataKeyedColumns15_nSizeIs;
	*DFUDataKeyedColumns16 = __params.DFUDataKeyedColumns16;
	*__DFUDataKeyedColumns16_nSizeIs = __params.__DFUDataKeyedColumns16_nSizeIs;
	*DFUDataKeyedColumns17 = __params.DFUDataKeyedColumns17;
	*__DFUDataKeyedColumns17_nSizeIs = __params.__DFUDataKeyedColumns17_nSizeIs;
	*DFUDataKeyedColumns18 = __params.DFUDataKeyedColumns18;
	*__DFUDataKeyedColumns18_nSizeIs = __params.__DFUDataKeyedColumns18_nSizeIs;
	*DFUDataKeyedColumns19 = __params.DFUDataKeyedColumns19;
	*__DFUDataKeyedColumns19_nSizeIs = __params.__DFUDataKeyedColumns19_nSizeIs;
	*DFUDataKeyedColumns20 = __params.DFUDataKeyedColumns20;
	*__DFUDataKeyedColumns20_nSizeIs = __params.__DFUDataKeyedColumns20_nSizeIs;
	*DFUDataNonKeyedColumns1 = __params.DFUDataNonKeyedColumns1;
	*__DFUDataNonKeyedColumns1_nSizeIs = __params.__DFUDataNonKeyedColumns1_nSizeIs;
	*DFUDataNonKeyedColumns2 = __params.DFUDataNonKeyedColumns2;
	*__DFUDataNonKeyedColumns2_nSizeIs = __params.__DFUDataNonKeyedColumns2_nSizeIs;
	*DFUDataNonKeyedColumns3 = __params.DFUDataNonKeyedColumns3;
	*__DFUDataNonKeyedColumns3_nSizeIs = __params.__DFUDataNonKeyedColumns3_nSizeIs;
	*DFUDataNonKeyedColumns4 = __params.DFUDataNonKeyedColumns4;
	*__DFUDataNonKeyedColumns4_nSizeIs = __params.__DFUDataNonKeyedColumns4_nSizeIs;
	*DFUDataNonKeyedColumns5 = __params.DFUDataNonKeyedColumns5;
	*__DFUDataNonKeyedColumns5_nSizeIs = __params.__DFUDataNonKeyedColumns5_nSizeIs;
	*DFUDataNonKeyedColumns6 = __params.DFUDataNonKeyedColumns6;
	*__DFUDataNonKeyedColumns6_nSizeIs = __params.__DFUDataNonKeyedColumns6_nSizeIs;
	*DFUDataNonKeyedColumns7 = __params.DFUDataNonKeyedColumns7;
	*__DFUDataNonKeyedColumns7_nSizeIs = __params.__DFUDataNonKeyedColumns7_nSizeIs;
	*DFUDataNonKeyedColumns8 = __params.DFUDataNonKeyedColumns8;
	*__DFUDataNonKeyedColumns8_nSizeIs = __params.__DFUDataNonKeyedColumns8_nSizeIs;
	*DFUDataNonKeyedColumns9 = __params.DFUDataNonKeyedColumns9;
	*__DFUDataNonKeyedColumns9_nSizeIs = __params.__DFUDataNonKeyedColumns9_nSizeIs;
	*DFUDataNonKeyedColumns10 = __params.DFUDataNonKeyedColumns10;
	*__DFUDataNonKeyedColumns10_nSizeIs = __params.__DFUDataNonKeyedColumns10_nSizeIs;
	*DFUDataNonKeyedColumns11 = __params.DFUDataNonKeyedColumns11;
	*__DFUDataNonKeyedColumns11_nSizeIs = __params.__DFUDataNonKeyedColumns11_nSizeIs;
	*DFUDataNonKeyedColumns12 = __params.DFUDataNonKeyedColumns12;
	*__DFUDataNonKeyedColumns12_nSizeIs = __params.__DFUDataNonKeyedColumns12_nSizeIs;
	*DFUDataNonKeyedColumns13 = __params.DFUDataNonKeyedColumns13;
	*__DFUDataNonKeyedColumns13_nSizeIs = __params.__DFUDataNonKeyedColumns13_nSizeIs;
	*DFUDataNonKeyedColumns14 = __params.DFUDataNonKeyedColumns14;
	*__DFUDataNonKeyedColumns14_nSizeIs = __params.__DFUDataNonKeyedColumns14_nSizeIs;
	*DFUDataNonKeyedColumns15 = __params.DFUDataNonKeyedColumns15;
	*__DFUDataNonKeyedColumns15_nSizeIs = __params.__DFUDataNonKeyedColumns15_nSizeIs;
	*DFUDataNonKeyedColumns16 = __params.DFUDataNonKeyedColumns16;
	*__DFUDataNonKeyedColumns16_nSizeIs = __params.__DFUDataNonKeyedColumns16_nSizeIs;
	*DFUDataNonKeyedColumns17 = __params.DFUDataNonKeyedColumns17;
	*__DFUDataNonKeyedColumns17_nSizeIs = __params.__DFUDataNonKeyedColumns17_nSizeIs;
	*DFUDataNonKeyedColumns18 = __params.DFUDataNonKeyedColumns18;
	*__DFUDataNonKeyedColumns18_nSizeIs = __params.__DFUDataNonKeyedColumns18_nSizeIs;
	*DFUDataNonKeyedColumns19 = __params.DFUDataNonKeyedColumns19;
	*__DFUDataNonKeyedColumns19_nSizeIs = __params.__DFUDataNonKeyedColumns19_nSizeIs;
	*DFUDataNonKeyedColumns20 = __params.DFUDataNonKeyedColumns20;
	*__DFUDataNonKeyedColumns20_nSizeIs = __params.__DFUDataNonKeyedColumns20_nSizeIs;
	*RowCount = __params.RowCount;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsDfuT<TClient>::DFUBrowseData(
		BSTR* LogicalName, 
		BSTR* FilterBy, 
		BSTR ShowColumns, 
		bool* SchemaOnly, 
		__int64* StartForGoback, 
		int* CountForGoback, 
		int* ChooseFile, 
		BSTR* Cluster, 
		BSTR* ClusterType, 
		BSTR* ParentName, 
		__int64* Start, 
		int* Count, 
		ArrayOfEspException* Exceptions, 
		BSTR* Name, 
		BSTR* FilterForGoBack, 
		DFUDataColumn** ColumnsHidden, int* __ColumnsHidden_nSizeIs, 
		int* ColumnCount, 
		int* PageSize, 
		__int64* Total, 
		BSTR* Result, 
		BSTR* MsgToDisplay
	)
{
    if ( LogicalName == NULL )
		return E_POINTER;
    if ( FilterBy == NULL )
		return E_POINTER;
    if ( SchemaOnly == NULL )
		return E_POINTER;
    if ( StartForGoback == NULL )
		return E_POINTER;
    if ( CountForGoback == NULL )
		return E_POINTER;
    if ( ChooseFile == NULL )
		return E_POINTER;
    if ( Cluster == NULL )
		return E_POINTER;
    if ( ClusterType == NULL )
		return E_POINTER;
    if ( ParentName == NULL )
		return E_POINTER;
    if ( Start == NULL )
		return E_POINTER;
    if ( Count == NULL )
		return E_POINTER;
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( Name == NULL )
		return E_POINTER;
    if ( FilterForGoBack == NULL )
		return E_POINTER;
    if ( ColumnsHidden == NULL )
		return E_POINTER;
	if( __ColumnsHidden_nSizeIs == NULL )
		return E_POINTER;
    if ( ColumnCount == NULL )
		return E_POINTER;
    if ( PageSize == NULL )
		return E_POINTER;
    if ( Total == NULL )
		return E_POINTER;
    if ( Result == NULL )
		return E_POINTER;
    if ( MsgToDisplay == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsDfu_DFUBrowseData_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.LogicalName = *LogicalName;
	__params.FilterBy = *FilterBy;
	__params.ShowColumns = ShowColumns;
	__params.SchemaOnly = *SchemaOnly;
	__params.StartForGoback = *StartForGoback;
	__params.CountForGoback = *CountForGoback;
	__params.ChooseFile = *ChooseFile;
	__params.Cluster = *Cluster;
	__params.ClusterType = *ClusterType;
	__params.ParentName = *ParentName;
	__params.Start = *Start;
	__params.Count = *Count;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsDfu/DFUBrowseData\?ver_=1.16\"\r\n"));
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

	*LogicalName = __params.LogicalName;
	*FilterBy = __params.FilterBy;
	*SchemaOnly = __params.SchemaOnly;
	*StartForGoback = __params.StartForGoback;
	*CountForGoback = __params.CountForGoback;
	*ChooseFile = __params.ChooseFile;
	*Cluster = __params.Cluster;
	*ClusterType = __params.ClusterType;
	*ParentName = __params.ParentName;
	*Start = __params.Start;
	*Count = __params.Count;
	*Exceptions = __params.Exceptions;
	*Name = __params.Name;
	*FilterForGoBack = __params.FilterForGoBack;
	*ColumnsHidden = __params.ColumnsHidden;
	*__ColumnsHidden_nSizeIs = __params.__ColumnsHidden_nSizeIs;
	*ColumnCount = __params.ColumnCount;
	*PageSize = __params.PageSize;
	*Total = __params.Total;
	*Result = __params.Result;
	*MsgToDisplay = __params.MsgToDisplay;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsDfuT<TClient>::DFUDefFile(
		BSTR Name, 
		BSTR Format, 
		ArrayOfEspException* Exceptions, 
		ATLSOAP_BLOB* defFile
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( defFile == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsDfu_DFUDefFile_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Name = Name;
	__params.Format = Format;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsDfu/DFUDefFile\?ver_=1.16\"\r\n"));
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
	*defFile = __params.defFile;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsDfuT<TClient>::DFUArrayAction(
		BSTR Type, 
		bool NoDelete, 
		BSTR* BackToPage, 
		BSTR* LogicalFiles, int __LogicalFiles_nSizeIs, 
		ArrayOfEspException* Exceptions, 
		BSTR* DFUArrayActionResult
	)
{
    if ( BackToPage == NULL )
		return E_POINTER;
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( DFUArrayActionResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsDfu_DFUArrayAction_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Type = Type;
	__params.NoDelete = NoDelete;
	__params.BackToPage = *BackToPage;
	__params.LogicalFiles = LogicalFiles;
	__params.__LogicalFiles_nSizeIs = __LogicalFiles_nSizeIs;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsDfu/DFUArrayAction\?ver_=1.16\"\r\n"));
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

	*BackToPage = __params.BackToPage;
	*Exceptions = __params.Exceptions;
	*DFUArrayActionResult = __params.DFUArrayActionResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsDfuT<TClient>::AddtoSuperfile(
		BSTR Superfile, 
		BSTR* Subfiles, 
		BSTR* names, int __names_nSizeIs, 
		bool ExistingFile, 
		ArrayOfEspException* Exceptions, 
		BSTR** SubfileNames, int* __SubfileNames_nSizeIs
	)
{
    if ( Subfiles == NULL )
		return E_POINTER;
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( SubfileNames == NULL )
		return E_POINTER;
	if( __SubfileNames_nSizeIs == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsDfu_AddtoSuperfile_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.Superfile = Superfile;
	__params.Subfiles = *Subfiles;
	__params.names = names;
	__params.__names_nSizeIs = __names_nSizeIs;
	__params.ExistingFile = ExistingFile;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsDfu/AddtoSuperfile\?ver_=1.16\"\r\n"));
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

	*Subfiles = __params.Subfiles;
	*Exceptions = __params.Exceptions;
	*SubfileNames = __params.SubfileNames;
	*__SubfileNames_nSizeIs = __params.__SubfileNames_nSizeIs;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsDfuT<TClient>::Savexml(
		BSTR name, 
		ArrayOfEspException* Exceptions, 
		ATLSOAP_BLOB* xmlmap
	)
{
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( xmlmap == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsDfu_Savexml_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.name = name;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsDfu/Savexml\?ver_=1.16\"\r\n"));
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
	*xmlmap = __params.xmlmap;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsDfuT<TClient>::SuperfileAddRaw(
		BSTR* superfile, 
		BSTR subfiles, 
		BSTR before, 
		ArrayOfEspException* Exceptions, 
		int* retcode
	)
{
    if ( superfile == NULL )
		return E_POINTER;
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( retcode == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsDfu_SuperfileAddRaw_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.superfile = *superfile;
	__params.subfiles = subfiles;
	__params.before = before;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsDfu/SuperfileAddRaw\?ver_=1.16\"\r\n"));
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

	*superfile = __params.superfile;
	*Exceptions = __params.Exceptions;
	*retcode = __params.retcode;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsDfuT<TClient>::SuperfileAction(
		BSTR action, 
		BSTR* superfile, 
		BSTR* subfiles, int __subfiles_nSizeIs, 
		BSTR before, 
		bool ___delete, 
		ArrayOfEspException* Exceptions, 
		int* retcode
	)
{
    if ( superfile == NULL )
		return E_POINTER;
    if ( Exceptions == NULL )
		return E_POINTER;
    if ( retcode == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CWsDfu_SuperfileAction_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.action = action;
	__params.superfile = *superfile;
	__params.subfiles = subfiles;
	__params.__subfiles_nSizeIs = __subfiles_nSizeIs;
	__params.before = before;
	__params.___delete = ___delete;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsDfu/SuperfileAction\?ver_=1.16\"\r\n"));
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

	*superfile = __params.superfile;
	*Exceptions = __params.Exceptions;
	*retcode = __params.retcode;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
inline HRESULT CWsDfuT<TClient>::Add(
		BSTR dstname, 
		ATLSOAP_BLOB xmlmap, 
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
	__CWsDfu_Add_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.dstname = dstname;
	__params.xmlmap = xmlmap;

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
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsDfu/Add\?ver_=1.16\"\r\n"));
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
ATL_NOINLINE inline const _soapmap ** CWsDfuT<TClient>::GetFunctionMap()
{
	return __CWsDfu_funcs;
}

template <typename TClient>
ATL_NOINLINE inline const _soapmap ** CWsDfuT<TClient>::GetHeaderMap()
{
	static const _soapmapentry __CWsDfu_DFUSearchData_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsDfu_DFUSearchData_atlsoapheader_map = 
	{
		0x5325B93E,
		"DFUSearchDataRequest",
		L"DFUSearchDataResponse",
		sizeof("DFUSearchDataRequest")-1,
		sizeof("DFUSearchDataResponse")-1,
		SOAPMAP_HEADER,
		__CWsDfu_DFUSearchData_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x18AC14AC,
		"http://webservices.seisint.com/WsDfu",
		L"http://webservices.seisint.com/WsDfu",
		sizeof("http://webservices.seisint.com/WsDfu")-1
	};

	static const _soapmapentry __CWsDfu_AddRemote_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsDfu_AddRemote_atlsoapheader_map = 
	{
		0xB5FBD704,
		"AddRemoteRequest",
		L"AddRemoteResponse",
		sizeof("AddRemoteRequest")-1,
		sizeof("AddRemoteResponse")-1,
		SOAPMAP_HEADER,
		__CWsDfu_AddRemote_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x18AC14AC,
		"http://webservices.seisint.com/WsDfu",
		L"http://webservices.seisint.com/WsDfu",
		sizeof("http://webservices.seisint.com/WsDfu")-1
	};

	static const _soapmapentry __CWsDfu_SuperfileList_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsDfu_SuperfileList_atlsoapheader_map = 
	{
		0x621FC69A,
		"SuperfileListRequest",
		L"SuperfileListResponse",
		sizeof("SuperfileListRequest")-1,
		sizeof("SuperfileListResponse")-1,
		SOAPMAP_HEADER,
		__CWsDfu_SuperfileList_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x18AC14AC,
		"http://webservices.seisint.com/WsDfu",
		L"http://webservices.seisint.com/WsDfu",
		sizeof("http://webservices.seisint.com/WsDfu")-1
	};

	static const _soapmapentry __CWsDfu_DFUSearch_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsDfu_DFUSearch_atlsoapheader_map = 
	{
		0x07CF4704,
		"DFUSearchRequest",
		L"DFUSearchResponse",
		sizeof("DFUSearchRequest")-1,
		sizeof("DFUSearchResponse")-1,
		SOAPMAP_HEADER,
		__CWsDfu_DFUSearch_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x18AC14AC,
		"http://webservices.seisint.com/WsDfu",
		L"http://webservices.seisint.com/WsDfu",
		sizeof("http://webservices.seisint.com/WsDfu")-1
	};

	static const _soapmapentry __CWsDfu_DFUFileView_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsDfu_DFUFileView_atlsoapheader_map = 
	{
		0x77DA5A09,
		"DFUFileViewRequest",
		L"DFUFileViewResponse",
		sizeof("DFUFileViewRequest")-1,
		sizeof("DFUFileViewResponse")-1,
		SOAPMAP_HEADER,
		__CWsDfu_DFUFileView_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x18AC14AC,
		"http://webservices.seisint.com/WsDfu",
		L"http://webservices.seisint.com/WsDfu",
		sizeof("http://webservices.seisint.com/WsDfu")-1
	};

	static const _soapmapentry __CWsDfu_DFUSpace_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsDfu_DFUSpace_atlsoapheader_map = 
	{
		0xE3CC693A,
		"DFUSpaceRequest",
		L"DFUSpaceResponse",
		sizeof("DFUSpaceRequest")-1,
		sizeof("DFUSpaceResponse")-1,
		SOAPMAP_HEADER,
		__CWsDfu_DFUSpace_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x18AC14AC,
		"http://webservices.seisint.com/WsDfu",
		L"http://webservices.seisint.com/WsDfu",
		sizeof("http://webservices.seisint.com/WsDfu")-1
	};

	static const _soapmapentry __CWsDfu_DFUInfo_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsDfu_DFUInfo_atlsoapheader_map = 
	{
		0x6EB8487A,
		"DFUInfoRequest",
		L"DFUInfoResponse",
		sizeof("DFUInfoRequest")-1,
		sizeof("DFUInfoResponse")-1,
		SOAPMAP_HEADER,
		__CWsDfu_DFUInfo_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x18AC14AC,
		"http://webservices.seisint.com/WsDfu",
		L"http://webservices.seisint.com/WsDfu",
		sizeof("http://webservices.seisint.com/WsDfu")-1
	};

	static const _soapmapentry __CWsDfu_DFUQuery_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsDfu_DFUQuery_atlsoapheader_map = 
	{
		0xA5995324,
		"DFUQueryRequest",
		L"DFUQueryResponse",
		sizeof("DFUQueryRequest")-1,
		sizeof("DFUQueryResponse")-1,
		SOAPMAP_HEADER,
		__CWsDfu_DFUQuery_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x18AC14AC,
		"http://webservices.seisint.com/WsDfu",
		L"http://webservices.seisint.com/WsDfu",
		sizeof("http://webservices.seisint.com/WsDfu")-1
	};

	static const _soapmapentry __CWsDfu_DFUGetDataColumns_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsDfu_DFUGetDataColumns_atlsoapheader_map = 
	{
		0xAA083E69,
		"DFUGetDataColumnsRequest",
		L"DFUGetDataColumnsResponse",
		sizeof("DFUGetDataColumnsRequest")-1,
		sizeof("DFUGetDataColumnsResponse")-1,
		SOAPMAP_HEADER,
		__CWsDfu_DFUGetDataColumns_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x18AC14AC,
		"http://webservices.seisint.com/WsDfu",
		L"http://webservices.seisint.com/WsDfu",
		sizeof("http://webservices.seisint.com/WsDfu")-1
	};

	static const _soapmapentry __CWsDfu_DFUBrowseData_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsDfu_DFUBrowseData_atlsoapheader_map = 
	{
		0x99F1A3BA,
		"DFUBrowseDataRequest",
		L"DFUBrowseDataResponse",
		sizeof("DFUBrowseDataRequest")-1,
		sizeof("DFUBrowseDataResponse")-1,
		SOAPMAP_HEADER,
		__CWsDfu_DFUBrowseData_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x18AC14AC,
		"http://webservices.seisint.com/WsDfu",
		L"http://webservices.seisint.com/WsDfu",
		sizeof("http://webservices.seisint.com/WsDfu")-1
	};

	static const _soapmapentry __CWsDfu_DFUDefFile_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsDfu_DFUDefFile_atlsoapheader_map = 
	{
		0xB1A2AF9D,
		"DFUDefFileRequest",
		L"DFUDefFileResponse",
		sizeof("DFUDefFileRequest")-1,
		sizeof("DFUDefFileResponse")-1,
		SOAPMAP_HEADER,
		__CWsDfu_DFUDefFile_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x18AC14AC,
		"http://webservices.seisint.com/WsDfu",
		L"http://webservices.seisint.com/WsDfu",
		sizeof("http://webservices.seisint.com/WsDfu")-1
	};

	static const _soapmapentry __CWsDfu_DFUArrayAction_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsDfu_DFUArrayAction_atlsoapheader_map = 
	{
		0x143314AB,
		"DFUArrayActionRequest",
		L"DFUArrayActionResponse",
		sizeof("DFUArrayActionRequest")-1,
		sizeof("DFUArrayActionResponse")-1,
		SOAPMAP_HEADER,
		__CWsDfu_DFUArrayAction_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x18AC14AC,
		"http://webservices.seisint.com/WsDfu",
		L"http://webservices.seisint.com/WsDfu",
		sizeof("http://webservices.seisint.com/WsDfu")-1
	};

	static const _soapmapentry __CWsDfu_AddtoSuperfile_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsDfu_AddtoSuperfile_atlsoapheader_map = 
	{
		0x3B4BDD4A,
		"AddtoSuperfileRequest",
		L"AddtoSuperfileResponse",
		sizeof("AddtoSuperfileRequest")-1,
		sizeof("AddtoSuperfileResponse")-1,
		SOAPMAP_HEADER,
		__CWsDfu_AddtoSuperfile_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x18AC14AC,
		"http://webservices.seisint.com/WsDfu",
		L"http://webservices.seisint.com/WsDfu",
		sizeof("http://webservices.seisint.com/WsDfu")-1
	};

	static const _soapmapentry __CWsDfu_Savexml_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsDfu_Savexml_atlsoapheader_map = 
	{
		0xC9FFF46F,
		"SavexmlRequest",
		L"SavexmlResponse",
		sizeof("SavexmlRequest")-1,
		sizeof("SavexmlResponse")-1,
		SOAPMAP_HEADER,
		__CWsDfu_Savexml_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x18AC14AC,
		"http://webservices.seisint.com/WsDfu",
		L"http://webservices.seisint.com/WsDfu",
		sizeof("http://webservices.seisint.com/WsDfu")-1
	};

	static const _soapmapentry __CWsDfu_SuperfileAddRaw_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsDfu_SuperfileAddRaw_atlsoapheader_map = 
	{
		0x6F93DD91,
		"SuperfileAddRawRequest",
		L"SuperfileAddRawResponse",
		sizeof("SuperfileAddRawRequest")-1,
		sizeof("SuperfileAddRawResponse")-1,
		SOAPMAP_HEADER,
		__CWsDfu_SuperfileAddRaw_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x18AC14AC,
		"http://webservices.seisint.com/WsDfu",
		L"http://webservices.seisint.com/WsDfu",
		sizeof("http://webservices.seisint.com/WsDfu")-1
	};

	static const _soapmapentry __CWsDfu_SuperfileAction_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsDfu_SuperfileAction_atlsoapheader_map = 
	{
		0x4AF2E9BC,
		"SuperfileActionRequest",
		L"SuperfileActionResponse",
		sizeof("SuperfileActionRequest")-1,
		sizeof("SuperfileActionResponse")-1,
		SOAPMAP_HEADER,
		__CWsDfu_SuperfileAction_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x18AC14AC,
		"http://webservices.seisint.com/WsDfu",
		L"http://webservices.seisint.com/WsDfu",
		sizeof("http://webservices.seisint.com/WsDfu")-1
	};

	static const _soapmapentry __CWsDfu_Add_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CWsDfu_Add_atlsoapheader_map = 
	{
		0x1181BCF8,
		"AddRequest",
		L"AddResponse",
		sizeof("AddRequest")-1,
		sizeof("AddResponse")-1,
		SOAPMAP_HEADER,
		__CWsDfu_Add_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0x18AC14AC,
		"http://webservices.seisint.com/WsDfu",
		L"http://webservices.seisint.com/WsDfu",
		sizeof("http://webservices.seisint.com/WsDfu")-1
	};


	static const _soapmap * __CWsDfu_headers[] =
	{
		&__CWsDfu_DFUSearchData_atlsoapheader_map,
		&__CWsDfu_AddRemote_atlsoapheader_map,
		&__CWsDfu_SuperfileList_atlsoapheader_map,
		&__CWsDfu_DFUSearch_atlsoapheader_map,
		&__CWsDfu_DFUFileView_atlsoapheader_map,
		&__CWsDfu_DFUSpace_atlsoapheader_map,
		&__CWsDfu_DFUInfo_atlsoapheader_map,
		&__CWsDfu_DFUQuery_atlsoapheader_map,
		&__CWsDfu_DFUGetDataColumns_atlsoapheader_map,
		&__CWsDfu_DFUBrowseData_atlsoapheader_map,
		&__CWsDfu_DFUDefFile_atlsoapheader_map,
		&__CWsDfu_DFUArrayAction_atlsoapheader_map,
		&__CWsDfu_AddtoSuperfile_atlsoapheader_map,
		&__CWsDfu_Savexml_atlsoapheader_map,
		&__CWsDfu_SuperfileAddRaw_atlsoapheader_map,
		&__CWsDfu_SuperfileAction_atlsoapheader_map,
		&__CWsDfu_Add_atlsoapheader_map,
		NULL
	};
	
	return __CWsDfu_headers;
}

template <typename TClient>
ATL_NOINLINE inline void * CWsDfuT<TClient>::GetHeaderValue()
{
	return this;
}

template <typename TClient>
ATL_NOINLINE inline const wchar_t * CWsDfuT<TClient>::GetNamespaceUri()
{
	return L"http://webservices.seisint.com/WsDfu";
}

template <typename TClient>
ATL_NOINLINE inline const char * CWsDfuT<TClient>::GetServiceName()
{
	return NULL;
}

template <typename TClient>
ATL_NOINLINE inline const char * CWsDfuT<TClient>::GetNamespaceUriA()
{
	return "http://webservices.seisint.com/WsDfu";
}

template <typename TClient>
ATL_NOINLINE inline HRESULT CWsDfuT<TClient>::CallFunction(
	void *, 
	const wchar_t *, int,
	size_t)
{
	return E_NOTIMPL;
}

template <typename TClient>
ATL_NOINLINE inline HRESULT CWsDfuT<TClient>::GetClientReader(ISAXXMLReader **ppReader)
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

} // namespace WsDfu

template<>
inline HRESULT AtlCleanupValue<WsDfu::EspException>(WsDfu::EspException *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Code);
	AtlCleanupValue(&pVal->Audience);
	AtlCleanupValue(&pVal->Source);
	AtlCleanupValue(&pVal->Message);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsDfu::EspException>(WsDfu::EspException *pVal, IAtlMemMgr *pMemMgr)
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
inline HRESULT AtlCleanupValue<WsDfu::ArrayOfEspException>(WsDfu::ArrayOfEspException *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Source);
	AtlCleanupArray(pVal->Exception, pVal->__Exception_nSizeIs);	
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsDfu::ArrayOfEspException>(WsDfu::ArrayOfEspException *pVal, IAtlMemMgr *pMemMgr)
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
inline HRESULT AtlCleanupValue<WsDfu::DFUFileStat>(WsDfu::DFUFileStat *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->MinSkew);
	AtlCleanupValue(&pVal->MaxSkew);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsDfu::DFUFileStat>(WsDfu::DFUFileStat *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->MinSkew, pMemMgr);
	AtlCleanupValueEx(&pVal->MaxSkew, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsDfu::DFULogicalFile>(WsDfu::DFULogicalFile *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Prefix);
	AtlCleanupValue(&pVal->ClusterName);
	AtlCleanupValue(&pVal->Directory);
	AtlCleanupValue(&pVal->Description);
	AtlCleanupValue(&pVal->Parts);
	AtlCleanupValue(&pVal->Name);
	AtlCleanupValue(&pVal->Owner);
	AtlCleanupValue(&pVal->Totalsize);
	AtlCleanupValue(&pVal->RecordCount);
	AtlCleanupValue(&pVal->Modified);
	AtlCleanupValue(&pVal->LongSize);
	AtlCleanupValue(&pVal->LongRecordCount);
	AtlCleanupValue(&pVal->isSuperfile);
	AtlCleanupValue(&pVal->isZipfile);
	AtlCleanupValue(&pVal->isDirectory);
	AtlCleanupValue(&pVal->Replicate);
	AtlCleanupValue(&pVal->IntSize);
	AtlCleanupValue(&pVal->IntRecordCount);
	AtlCleanupValue(&pVal->FromRoxieCluster);
	AtlCleanupValue(&pVal->BrowseData);
	AtlCleanupValue(&pVal->IsKeyFile);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsDfu::DFULogicalFile>(WsDfu::DFULogicalFile *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Prefix, pMemMgr);
	AtlCleanupValueEx(&pVal->ClusterName, pMemMgr);
	AtlCleanupValueEx(&pVal->Directory, pMemMgr);
	AtlCleanupValueEx(&pVal->Description, pMemMgr);
	AtlCleanupValueEx(&pVal->Parts, pMemMgr);
	AtlCleanupValueEx(&pVal->Name, pMemMgr);
	AtlCleanupValueEx(&pVal->Owner, pMemMgr);
	AtlCleanupValueEx(&pVal->Totalsize, pMemMgr);
	AtlCleanupValueEx(&pVal->RecordCount, pMemMgr);
	AtlCleanupValueEx(&pVal->Modified, pMemMgr);
	AtlCleanupValueEx(&pVal->LongSize, pMemMgr);
	AtlCleanupValueEx(&pVal->LongRecordCount, pMemMgr);
	AtlCleanupValueEx(&pVal->isSuperfile, pMemMgr);
	AtlCleanupValueEx(&pVal->isZipfile, pMemMgr);
	AtlCleanupValueEx(&pVal->isDirectory, pMemMgr);
	AtlCleanupValueEx(&pVal->Replicate, pMemMgr);
	AtlCleanupValueEx(&pVal->IntSize, pMemMgr);
	AtlCleanupValueEx(&pVal->IntRecordCount, pMemMgr);
	AtlCleanupValueEx(&pVal->FromRoxieCluster, pMemMgr);
	AtlCleanupValueEx(&pVal->BrowseData, pMemMgr);
	AtlCleanupValueEx(&pVal->IsKeyFile, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsDfu::DFUSpaceItem>(WsDfu::DFUSpaceItem *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Name);
	AtlCleanupValue(&pVal->NumOfFiles);
	AtlCleanupValue(&pVal->NumOfFilesUnknown);
	AtlCleanupValue(&pVal->TotalSize);
	AtlCleanupValue(&pVal->LargestFile);
	AtlCleanupValue(&pVal->LargestSize);
	AtlCleanupValue(&pVal->SmallestFile);
	AtlCleanupValue(&pVal->SmallestSize);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsDfu::DFUSpaceItem>(WsDfu::DFUSpaceItem *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Name, pMemMgr);
	AtlCleanupValueEx(&pVal->NumOfFiles, pMemMgr);
	AtlCleanupValueEx(&pVal->NumOfFilesUnknown, pMemMgr);
	AtlCleanupValueEx(&pVal->TotalSize, pMemMgr);
	AtlCleanupValueEx(&pVal->LargestFile, pMemMgr);
	AtlCleanupValueEx(&pVal->LargestSize, pMemMgr);
	AtlCleanupValueEx(&pVal->SmallestFile, pMemMgr);
	AtlCleanupValueEx(&pVal->SmallestSize, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsDfu::DFUPart>(WsDfu::DFUPart *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Id);
	AtlCleanupValue(&pVal->Copy);
	AtlCleanupValue(&pVal->ActualSize);
	AtlCleanupValue(&pVal->Ip);
	AtlCleanupValue(&pVal->Partsize);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsDfu::DFUPart>(WsDfu::DFUPart *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Id, pMemMgr);
	AtlCleanupValueEx(&pVal->Copy, pMemMgr);
	AtlCleanupValueEx(&pVal->ActualSize, pMemMgr);
	AtlCleanupValueEx(&pVal->Ip, pMemMgr);
	AtlCleanupValueEx(&pVal->Partsize, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsDfu::DFUDataColumn>(WsDfu::DFUDataColumn *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->ColumnLabel);
	AtlCleanupValue(&pVal->ColumnType);
	AtlCleanupValue(&pVal->ColumnValue);
	AtlCleanupValue(&pVal->ColumnSize);
	AtlCleanupValue(&pVal->MaxSize);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsDfu::DFUDataColumn>(WsDfu::DFUDataColumn *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->ColumnLabel, pMemMgr);
	AtlCleanupValueEx(&pVal->ColumnType, pMemMgr);
	AtlCleanupValueEx(&pVal->ColumnValue, pMemMgr);
	AtlCleanupValueEx(&pVal->ColumnSize, pMemMgr);
	AtlCleanupValueEx(&pVal->MaxSize, pMemMgr);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValue<WsDfu::DFUFileDetail>(WsDfu::DFUFileDetail *pVal)
{
	pVal;
	AtlCleanupValue(&pVal->Name);
	AtlCleanupValue(&pVal->Filename);
	AtlCleanupValue(&pVal->Description);
	AtlCleanupValue(&pVal->Dir);
	AtlCleanupValue(&pVal->PathMask);
	AtlCleanupValue(&pVal->Filesize);
	AtlCleanupValue(&pVal->ActualSize);
	AtlCleanupValue(&pVal->RecordSize);
	AtlCleanupValue(&pVal->RecordCount);
	AtlCleanupValue(&pVal->Wuid);
	AtlCleanupValue(&pVal->Owner);
	AtlCleanupValue(&pVal->Cluster);
	AtlCleanupValue(&pVal->JobName);
	AtlCleanupValue(&pVal->Persistent);
	AtlCleanupValue(&pVal->Format);
	AtlCleanupValue(&pVal->MaxRecordSize);
	AtlCleanupValue(&pVal->CsvSeparate);
	AtlCleanupValue(&pVal->CsvQuote);
	AtlCleanupValue(&pVal->CsvTerminate);
	AtlCleanupValue(&pVal->Modified);
	AtlCleanupValue(&pVal->Ecl);
	AtlCleanupValue(&pVal->ZipFile);
	AtlCleanupValue(&pVal->Stat);
	AtlCleanupArray(pVal->DFUFileParts, pVal->__DFUFileParts_nSizeIs);	
	AtlCleanupValue(&pVal->isSuperfile);
	AtlCleanupValue(&pVal->ShowFileContent);
	AtlCleanupArray(pVal->subfiles, pVal->__subfiles_nSizeIs);	
	AtlCleanupArray(pVal->Superfiles, pVal->__Superfiles_nSizeIs);	
	AtlCleanupValue(&pVal->FromRoxieCluster);
	AtlCleanupArray(pVal->Graphs, pVal->__Graphs_nSizeIs);	
	AtlCleanupValue(&pVal->UserPermission);
	return S_OK;
}

template<>
inline HRESULT AtlCleanupValueEx<WsDfu::DFUFileDetail>(WsDfu::DFUFileDetail *pVal, IAtlMemMgr *pMemMgr)
{
	pVal;
	pMemMgr;
	
	AtlCleanupValueEx(&pVal->Name, pMemMgr);
	AtlCleanupValueEx(&pVal->Filename, pMemMgr);
	AtlCleanupValueEx(&pVal->Description, pMemMgr);
	AtlCleanupValueEx(&pVal->Dir, pMemMgr);
	AtlCleanupValueEx(&pVal->PathMask, pMemMgr);
	AtlCleanupValueEx(&pVal->Filesize, pMemMgr);
	AtlCleanupValueEx(&pVal->ActualSize, pMemMgr);
	AtlCleanupValueEx(&pVal->RecordSize, pMemMgr);
	AtlCleanupValueEx(&pVal->RecordCount, pMemMgr);
	AtlCleanupValueEx(&pVal->Wuid, pMemMgr);
	AtlCleanupValueEx(&pVal->Owner, pMemMgr);
	AtlCleanupValueEx(&pVal->Cluster, pMemMgr);
	AtlCleanupValueEx(&pVal->JobName, pMemMgr);
	AtlCleanupValueEx(&pVal->Persistent, pMemMgr);
	AtlCleanupValueEx(&pVal->Format, pMemMgr);
	AtlCleanupValueEx(&pVal->MaxRecordSize, pMemMgr);
	AtlCleanupValueEx(&pVal->CsvSeparate, pMemMgr);
	AtlCleanupValueEx(&pVal->CsvQuote, pMemMgr);
	AtlCleanupValueEx(&pVal->CsvTerminate, pMemMgr);
	AtlCleanupValueEx(&pVal->Modified, pMemMgr);
	AtlCleanupValueEx(&pVal->Ecl, pMemMgr);
	AtlCleanupValueEx(&pVal->ZipFile, pMemMgr);
	AtlCleanupValueEx(&pVal->Stat, pMemMgr);
	if (pVal->DFUFileParts != NULL)
	{
		AtlCleanupArrayEx(pVal->DFUFileParts, pVal->__DFUFileParts_nSizeIs, pMemMgr);		
		pMemMgr->Free(pVal->DFUFileParts);
	}
	AtlCleanupValueEx(&pVal->isSuperfile, pMemMgr);
	AtlCleanupValueEx(&pVal->ShowFileContent, pMemMgr);
	if (pVal->subfiles != NULL)
	{
		AtlCleanupArrayEx(pVal->subfiles, pVal->__subfiles_nSizeIs, pMemMgr);		
		pMemMgr->Free(pVal->subfiles);
	}
	if (pVal->Superfiles != NULL)
	{
		AtlCleanupArrayEx(pVal->Superfiles, pVal->__Superfiles_nSizeIs, pMemMgr);		
		pMemMgr->Free(pVal->Superfiles);
	}
	AtlCleanupValueEx(&pVal->FromRoxieCluster, pMemMgr);
	if (pVal->Graphs != NULL)
	{
		AtlCleanupArrayEx(pVal->Graphs, pVal->__Graphs_nSizeIs, pMemMgr);		
		pMemMgr->Free(pVal->Graphs);
	}
	AtlCleanupValueEx(&pVal->UserPermission, pMemMgr);
	return S_OK;
}
