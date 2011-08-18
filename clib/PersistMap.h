#pragma once

#include "RecursiveMutex.h"
#include "unicodefile.h"

//  Common ---
#define PERSIST_TYPE _T("type")
#define PERSIST_PLACEMENT _T("placement")
#define PERSIST_TABINDEX _T("tabIndex")
#define PERSIST_GROUPINDEX _T("groupIndex")
#define PERSIST_ACTIVE _T("active")

//  ECL Windows ---
#define PERSIST_ECL _T("ecl")
#define PERSIST_DIRTY _T("dirty")

//  Builder Window  ---
#define PERSIST_LABEL _T("label")
#define PERSIST_FILEPATH _T("filepath")
#define PERSIST_QUEUE _T("queue")
#define PERSIST_CLUSTER _T("cluster")
#define PERSIST_REPOSITORY_LABEL _T("repositoryLabel")
#define PERSIST_SCHEDULE _T("schedule")
#define PERSIST_LIMITRESULT _T("resultLimit")
#define PERSIST_DEBUG _T("debug")
#define PERSIST_MAXRUNTIME _T("maxRunTime")

//  Attribute Window  ---
#define PERSIST_MODULE _T("module")
#define PERSIST_ATTRIBUTE _T("attribute")
#define PERSIST_ATTRIBUTETYPE _T("attributeType")

//  Graph Window  ---
#define PERSIST_PATH _T("path")

//  Result Window ---
#define PERSIST_WUID _T("wuid")

//  ---
#define PERSISTVAL_BUILDER _T("builder")
#define PERSISTVAL_ATTRIBUTE _T("attribute")
#define PERSISTVAL_GRAPH _T("graph")
//#define PERSISTVAL_RESULT _T("result")
#define PERSISTVAL_HISTORY _T("history")
#define PERSISTVAL_ECLWATCH _T("eclwatch")

typedef std::map<std::_tstring, std::_tstring> PersistMap;

class CLIB_API CPersistMap : public CUnknown
{
protected:
	PersistMap m_map;

public:
	BEGIN_CUNKNOWN
	END_CUNKNOWN(CUnknown)

	CPersistMap();

	void Set(const std::_tstring & key, const std::_tstring & value);
	const TCHAR * Get(const std::_tstring & key) const;
	bool HasContent() const;

	bool serialize(std::_tstring & resultXml) const;
	bool deserialize(const std::_tstring & xmlPath);
	bool deserializeXML(const std::_tstring & xml);

protected:
	bool deserialize(const std::_tstring & xml_path, bool isPath);
};
typedef StlLinked<CPersistMap> CPersistMapAdapt;
typedef std::vector<CPersistMapAdapt> CPersistMapVector;

__interface __declspec(uuid("B117F331-C28C-4CBE-94DD-6696D0123546")) IPersistable
{
	void SavePersistInfo(CPersistMap & persistInfo);
	void RestorePersistInfo(const CPersistMap & persistInfo);
	HWND GetHwnd();
};


class CLIB_API CPersistWindow
{
protected:
	IPersistable * m_owner;
	boost::filesystem::path m_filePath;
	CUnicodeFile m_file;
	clib::recursive_mutex m_mutex;

public:
	CPersistWindow(IPersistable * m_owner);
	void SetPath(const std::string & path, const boost::filesystem::path & folder);
	void Save();
	void Restore();
	void Remove();

protected:
	void GenerateTmpFile(const boost::filesystem::path & folder);
	static void DoAutoSave(CUnicodeFile *f, CPersistMap *persistInfo);
};
