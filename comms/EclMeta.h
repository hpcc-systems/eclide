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

    const TCHAR * GetValue(const Element & e, const std::_tstring &key, std::_tstring & retVal) {
        Element::StringStringMap::const_iterator found = e.m_attr.find(key);
        if (found != e.m_attr.end())
            retVal = found->second;
        return retVal.c_str();
    }
    int GetIntValue(const Element & e, const std::_tstring &key, int & retVal)
    {
        retVal = 0;
        Element::StringStringMap::const_iterator found = e.m_attr.find(key);
        if (found != e.m_attr.end())
            retVal = std::stoi(found->second);
        return retVal;
    }

};

class CEclFolder : public CEclMetaData
{
public:
    CEclFolder(const boost::filesystem::wpath &path)
    {
        m_path = path;
    }
};

class CEclParam : public CEclMetaData
{
protected:
    std::_tstring m_name;
    std::_tstring m_type;
public:
    BEGIN_CUNKNOWN
    END_CUNKNOWN(CUnknown)

    CEclParam() { }
    std::_tstring GetName() { return m_name; }
    std::_tstring GetType() { return m_type; }
    void UpdateAttrs(Element &e)
    {
        GetValue(e, _T("name"), m_name);
        GetValue(e, _T("type"), m_type);
    };
    void AddType(Element e)
    {
        GetValue(e, _T("type"), m_type);
    }
    void AddName(Element e)
    {
        GetValue(e, _T("name"), m_name);
    }
};
typedef StlLinked<CEclParam> CEclParamAdapt;
typedef std::map<std::wstring, StlLinked<CEclParam> > EclParamMap;

class CEclDefinition : public CEclMetaData
{
protected:
    int m_body, m_end, m_line, m_start;
    std::_tstring m_name;
    std::_tstring m_type;
    std::_tstring m_functionType;
    std::_tstring m_content;
    std::map<std::wstring, StlLinked<CEclDefinition> > m_defs;
    EclParamMap m_params;

public:
    BEGIN_CUNKNOWN
    END_CUNKNOWN(CUnknown)

    std::map<std::wstring, std::wstring> m_fields;

    CEclDefinition() { m_body = m_end = m_line = m_start = 0; }
    CEclDefinition(const CEclDefinition & def) { m_name = def.m_name; m_type = def.m_type; m_body = def.m_body; m_end = def.m_end; m_line = def.m_line; m_start = def.m_start; m_fields = def.m_fields; m_defs = def.m_defs; m_params = def.m_params; m_content = def.m_content; m_functionType = def.m_functionType; }
    std::_tstring GetName() { return m_name; }
    std::_tstring GetType() { return m_type; }
    void UpdateAttrs(Element &e)
    {
        std::wstring name, type;
        GetValue(e, _T("name"), m_name);
        GetValue(e, _T("type"), m_type);
        GetIntValue(e, _T("body"), m_body);
        GetIntValue(e, _T("end"), m_end);
        GetIntValue(e, _T("line"), m_line);
        GetIntValue(e, _T("start"), m_start);
    }
	bool HasDefinitions()
	{
		return m_defs.size() ? true : false;
	}
    void AddFunctionType(Element e)
    {
        GetValue(e, _T("type"), m_functionType);
    }
    void AddDocumentation(Element e)
    {
        m_content = e.GetContent();
    }
    void AddParam(CEclParam *param)
    {
        m_params[param->GetName()] = param;
    }
    void AddField(Element e)
    {
        std::wstring name, type;
        GetValue(e, _T("name"), name);
        GetValue(e, _T("type"), type);
        m_fields[name] = type;
    }
    void AddDef(CEclDefinition * def)
    {
        m_defs[def->GetName()] = def;
    }
    CEclDefinition * GetDefinition(const std::_tstring & defName)
    {
        if (boost::algorithm::iequals(defName, GetName()))
        {
            return this;
        }

        CEclDefinition *def = NULL;
        for (std::map<std::wstring, StlLinked<CEclDefinition>>::iterator itr = m_defs.begin(); itr != m_defs.end(); ++itr)
        {
            def = itr->second.get();
            if (CEclDefinition *subdef = def->GetDefinition(defName))
            {
                return subdef;
            }
        }

        return NULL;
    }
    bool GetRecordStrings(const std::_tstring & token, StdStringVector &set)
    {
        bool found = false;

        if (boost::algorithm::iequals(GetType(), "record"))
        {
            set.push_back(GetName().c_str());
            found = true;
        }
        else if (boost::algorithm::iequals(GetType(), "module"))
        {
            CEclDefinition *def = NULL;
            for (std::map<std::wstring, StlLinked<CEclDefinition> >::iterator itr = m_defs.begin(); itr != m_defs.end(); ++itr)
            {
                def = itr->second.get();
                if (def->GetRecordStrings(token, set))
                {
                    found = true;
                }
            }
        }

        return found;
    }
	bool GetFunctions(std::map<std::wstring, StlLinked<CEclDefinition> > & defs)
	{
		bool found = false;
		CEclDefinition *def = NULL;

		for (std::map<std::wstring, StlLinked<CEclDefinition>>::iterator itr = m_defs.begin(); itr != m_defs.end(); ++itr)
		{
			def = itr->second.get();
			if (boost::algorithm::iequals(def->GetType(), _T("module")) && def->HasDefinitions())
			{
				def->GetFunctions(defs);
			}
			else if (boost::algorithm::iequals(def->GetType(), _T("function")))
			{
				def = itr->second.get();
				CEclDefinition *defNew = new CEclDefinition(*def);
				defs[def->GetName()] = defNew;
				found = true;
			}
		}
		return found;
	}
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
        GetIntValue(e, _T("end"), m_end);
        GetIntValue(e, _T("line"), m_line);
        GetValue(e, _T("name"), m_name);
        GetValue(e, _T("ref"), m_ref);
        GetIntValue(e, _T("start"), m_start);
        GetValue(e, _T("type"), m_type);
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
    EclImportMap GetImports()
    {
        return m_imports;
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
    bool GetRecordStrings(const std::_tstring & token, const std::_tstring & last, StdStringVector &set)
    {
        bool found = false;
        CEclDefinition *def = NULL;

        for (EclDefinitionMap::iterator itr = m_defs.begin(); itr != m_defs.end(); ++itr)
        {
            def = itr->second.get();
            // reject if the token is a record itself
            if (boost::algorithm::iequals(def->GetType(), _T("record")) && boost::algorithm::iequals(def->GetName(), last))
            {
                return false;
            }
            if (def->GetRecordStrings(token, set))
            {
                found = true;
            }
        }
        return found;
    }
    CEclImport * GetImportRefs(const std::_tstring & importAsName)
    {
        for (EclImportMap::iterator itr = m_imports.begin(); itr != m_imports.end(); ++itr)
        {
            if (boost::algorithm::iequals(importAsName, itr->first) && !boost::algorithm::iequals(importAsName, itr->second->GetRef()))
            {
                return itr->second.get();
            }
        }
        return NULL;
    }
    CEclDefinition * GetDefinition(const std::_tstring & defName)
    {
        CEclDefinition *def = NULL;
        CEclDefinition *defFound = NULL;

        for (std::map<std::wstring, StlLinked<CEclDefinition>>::iterator itr = m_defs.begin(); itr != m_defs.end(); ++itr)
        {
            def = itr->second.get();
            if ((defFound = def->GetDefinition(defName)))
            {
                return defFound;
            }
        }
        return NULL;
    }
	bool GetFunctions(std::map<std::wstring, StlLinked<CEclDefinition> > & defs)
	{
		bool found = false;
		CEclDefinition *def = NULL;

		for (EclDefinitionMap::iterator itr = m_defs.begin(); itr != m_defs.end(); ++itr)
		{
			def = itr->second.get();
			found = def->GetFunctions(defs);
		}
		return found;
	}
};

typedef std::map<std::wstring, StlLinked<CEclMetaData> > EclMetaDataMap;

class CEclMeta
{
public:
    EclMetaDataMap m_masterMeta;
    std::vector<std::_tstring> m_autoTokens;
    std::_tstring m_autoString;
    std::_tstring m_autoLast;

    bool CEclMeta::MetaExists(const std::_tstring & key);
    void PopulateMeta(const boost::filesystem::wpath & fileOrDir, const std::_tstring & dottedPath = _T(""), int level = 0);
    void Update(const WPathVector & folders, const std::wstring & xml);
    void PopulateMetaUpwards(const WPathVector & folders, const std::_tstring & path);
    CEclFile * CreateMetaFile(const std::_tstring & dottedPath, const std::_tstring & path = _T(""));
    bool LoadImports(const std::_tstring & path, const WPathVector & folders);
	bool LoadTooltips(const std::_tstring & path);
    bool GetPathFromModule(const std::_tstring & module, const WPathVector & folders, CString &retPath, bool &retIsFolder);
    CEclFile *GetSourceFileFromPath(const std::wstring & path);
    CEclFile *GetSourceFromPath(const std::_tstring & path);
    bool GetMetaModuleInfo(IAttribute *attr, const std::_tstring & token, StdStringVector &set);
    std::_tstring AddAutoStr(StdStringVector &set, const std::_tstring & str);
    bool GetRecordFields(const std::_tstring & token, const std::_tstring & key, StdStringVector &set);
    bool GetAutoC(IAttribute *attr, const std::_tstring & partialLabel, StdStringVector & set);
    int NormalizeAutoC(IAttribute *attr);
    bool FindImportAs();
    void BuildTokenStr(int numTokens = 0);
};
