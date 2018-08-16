#pragma once

#include <global.h>
#include <UtilFilesystem.h>
#include <StringSet.h>
#include "AttributeType.h"
#include "RecursiveMutex.h"
#include "SAXHandlersBase.h"
#include "DiskAttribute.h"

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

    const boost::filesystem::wpath & GetPath() { return m_path; }
    bool HasPath() { return m_path.size() > 0; }

    std::wstring GetValue(Element e, const std::_tstring key)
    {
        std::wstring value = _T("");
        Element::StringStringMap::const_iterator found = e.m_attr.find(key);
        if (found != e.m_attr.end())
            value = found->second;
        return value;
    }
    int GetIntValue(Element e, const std::_tstring key)
    {
        int value = 0;
        Element::StringStringMap::const_iterator found = e.m_attr.find(key);
        if (found != e.m_attr.end())
            value = std::stoi(found->second);
        return value;
    }
};

class CEclFolder : public CEclMetaData
{
public:
    CEclFolder(const boost::filesystem::wpath path)
    {
        m_path = path;
    }
};

class CEclDefinition : public CEclMetaData
{
protected:
    int m_body, m_end, m_line, m_start;
    std::_tstring m_name;
    std::_tstring m_type;
    std::map<std::wstring, StlLinked<CEclDefinition> > m_defs;
public:
    BEGIN_CUNKNOWN

    END_CUNKNOWN(CUnknown)
    std::map<std::wstring, std::wstring> m_fields;

    CEclDefinition() { m_body = m_end = m_line = m_start = 0; }
    std::_tstring GetName() { return m_name; }
    std::_tstring GetType() { return m_type; }
    void UpdateAttrs(Element &e)
    {
        std::wstring name, type;
        m_name = GetValue(e, _T("name"));
        m_type = GetValue(e, _T("type"));
        m_body = GetIntValue(e, _T("body"));
        m_end = GetIntValue(e, _T("end"));
        m_line = GetIntValue(e, _T("line"));
        m_start = GetIntValue(e, _T("start"));
    };
    void AddField(Element e)
    {
        std::wstring name, type;
        name = GetValue(e, _T("name"));
        type = GetValue(e, _T("type"));
        m_fields[name] = type;
    };
};
typedef StlLinked<CEclDefinition> CEclDefinitionAdapt;
typedef std::map<std::wstring, StlLinked<CEclDefinition> > EclDefinitionMap;

class CEclImport : public CEclMetaData
{
protected:
    int m_body, m_end, m_line, m_start;
    std::_tstring m_name;
    std::_tstring m_type;
    std::_tstring m_ref;
public:
    BEGIN_CUNKNOWN
    END_CUNKNOWN(CUnknown)

    CEclImport() { m_end = m_line = m_start = 0; }
    std::_tstring GetName() { return m_name; }
    std::_tstring GetType() { return m_type; }
    std::_tstring GetRef() { return m_ref; }
    void UpdateAttrs(Element &e)
    {
        std::wstring name, type;
        m_end = GetIntValue(e, _T("end"));
        m_line = GetIntValue(e, _T("line"));
        m_name = GetValue(e, _T("name"));
        m_ref = GetValue(e, _T("ref"));
        m_start = GetIntValue(e, _T("start"));
        m_type = GetValue(e, _T("type"));
    };
};
typedef StlLinked<CEclImport> CEclImportAdapt;
typedef std::map<std::wstring, StlLinked<CEclImport> > EclImportMap;

class CEclFile : public CEclMetaData
{
protected:
    EclDefinitionMap m_defs;
    EclImportMap m_imports;

public:

    BEGIN_CUNKNOWN
    END_CUNKNOWN(CUnknown)

    CEclFile(const boost::filesystem::wpath path)
    {
        m_path = path;
        m_defs.clear();
        m_imports.clear();
    }
    void UpdateDef(CEclDefinition *def)
    {
        m_defs[def->GetName()] = def;
    }
    void UpdateImport(CEclImport *import)
    {
        m_imports[import->GetName()] = import;
    }
    /*
        Scenarios:
        "MyMod" and presses ctrl+space
        "MyModule." (should auto popup suggestions)
        "MyModule.MyAttr" and presses ctrl+space
        "MyModule.MyAttribute." (should auto popup suggestions)
        Cursor in "MyModule.MyAttribute" and user presses F12
        Mouse over tooltip "MyModule.MyAttribute"
        
        IMPORT $.MyModule;
        IMPORT MyModule AS MyMod;
        IMPORT $ AS MyMod;
        (Where MyModule could be a folder an attr file or a library (like std))
        */
    bool GetDefStrings(const std::_tstring & definitionName, StdStringVector &set)
    {
        bool found = false;
        for (std::map<std::wstring, StlLinked<CEclDefinition>>::iterator itr = m_defs.begin(); itr != m_defs.end(); ++itr)
        {
            if (boost::algorithm::equals(definitionName, itr->first)) {
                for (std::map<std::wstring, std::wstring>::iterator fieldItr = itr->second->m_fields.begin(); fieldItr != itr->second->m_fields.end(); ++fieldItr)
                {
                    if (std::find(set.begin(), set.end(), fieldItr->first) == set.end())
                    {
                        set.push_back(fieldItr->first);
                        found = true;
                    }
                }
            }
        }
        return found;
    }
    CEclImport * GetImportRefs(const std::_tstring & importAsName)
    {
        for (std::map<std::wstring, StlLinked<CEclImport>>::iterator itr = m_imports.begin(); itr != m_imports.end(); ++itr)
        {
            if (boost::algorithm::iequals(importAsName, itr->first) && !boost::algorithm::equals(importAsName, itr->second->GetRef()))
            {
                return dynamic_cast<CEclImport*>(itr->second.get());
            }
        }
        return NULL;
    }
    CEclDefinition * GetDefinitions(const std::_tstring & defName)
    {
        for (std::map<std::wstring, StlLinked<CEclDefinition>>::iterator itr = m_defs.begin(); itr != m_defs.end(); ++itr)
        {
            if (boost::algorithm::iequals(defName, itr->first))
            {
                return dynamic_cast<CEclDefinition*>(itr->second.get());
            }
        }
        return NULL;
    }
};

typedef std::map<std::wstring, StlLinked<CEclMetaData> > EclMetaDataMap;

class CEclMeta
{
public:
    EclMetaDataMap m_masterMeta;
    std::vector<CString> m_autoTokens;
    std::_tstring m_autoString;
    std::_tstring m_autoLast;

    void LoadMetaData(const WPathVector *folders);
    void PopulateMeta(const boost::filesystem::wpath & fileOrDir, const std::wstring path, int level=0);
    void CEclMeta::Update(const std::wstring & xml);
    CEclFile *GetSourceFromPath(const std::_tstring & path);
    bool GetMetaModuleInfo(IAttribute *attr, const std::_tstring & token, StdStringVector &set);
    bool AddAutoStr(StdStringVector &set, const std::_tstring & str);
    bool GetMetaParseInfo(const std::_tstring & token, const std::_tstring & key, StdStringVector &set);
    bool GetAutoC(IAttribute *attr, const std::_tstring & partialLabel, StdStringVector & set);
    int NormalizeAutoC(IAttribute *attr);
    bool FindImportAs();
    void BuildTokenStr();
};
