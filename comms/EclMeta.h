#pragma once

#include <global.h>
#include <UtilFilesystem.h>
#include <StringSet.h>
#include "AttributeType.h"
#include "RecursiveMutex.h"

typedef std::pair<std::_tstring, bool> StringBoolPair;	//  Path, include in -I options
typedef std::vector<StringBoolPair> WPathVector;

class CEclMetaData : public clib::CLockableUnknown
{
protected:
    boost::filesystem::wpath m_path;
    std::_tstring m_moduleLabel;
public:
    BEGIN_CUNKNOWN
    END_CUNKNOWN(CUnknown)

    boost::filesystem::wpath GetPath() { return m_path; }
    bool HasPath() { return m_path.size() > 0; }
};

class CEclFolder : public CEclMetaData
{
public:
    CEclFolder(const boost::filesystem::wpath path)
    {
        m_path = path;
    }
};

class CEclFile : public CEclMetaData
{
protected:
    std::_tstring m_metaXML;
public:
    CEclFile(const boost::filesystem::wpath path)
    {
        m_path = path;
    }
};

typedef std::map<std::wstring, StlLinked<CEclMetaData> > EclMetaDataMap;

class CEclMeta
{
public:
    EclMetaDataMap m_masterMeta;

    void LoadMetaData(const WPathVector *folders);
    void PopulateMeta(const boost::filesystem::wpath & fileOrDir, const std::wstring path, int level=0);
    const StlLinked<CEclMetaData> GetMetaInfo(const std::_tstring & moduleFile);
    bool AddAutoStr(StdStringVector &set, const std::_tstring & str);
    bool GetAutoC(const std::_tstring & partialLabel, StdStringVector &set);
};
