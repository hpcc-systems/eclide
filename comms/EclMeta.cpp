#include "StdAfx.h"

#include "EclMeta.h"
#include "SAXHandlers.h"

class CECLCCMetaHandler : public StackHandlerImpl
{
private:

protected:
    CEclMeta *m_meta;
    WPathVector m_folders;
    CComPtr<CEclFile> m_currSource;
    std::stack<CEclDefinitionAdapt> m_defStack;
    std::stack<CEclParamAdapt> m_paramStack;
    std::stack<CEclImportAdapt> m_importStack;

public:
    CECLCCMetaHandler(const WPathVector & folders, CEclMeta *meta) :m_folders(folders), m_meta(meta) {};
    ~CECLCCMetaHandler() {};

    typedef std::map<std::_tstring, std::_tstring> edge_type;

    virtual HRESULT STDMETHODCALLTYPE endDocument()
    {
        return StackHandlerImpl::endDocument();
    }
    virtual HRESULT STDMETHODCALLTYPE startElement(
        const wchar_t *pwchNamespaceUri,
        int cchNamespaceUri,
        const wchar_t *pwchLocalName,
        int cchLocalName,
        const wchar_t *pwchQName,
        int cchQName,
        ISAXAttributes *pAttributes)
    {
        HRESULT retval = __super::startElement(pwchNamespaceUri, cchNamespaceUri, pwchLocalName, cchLocalName, pwchQName, cchQName, pAttributes);
        Element &e = m_stack.top();
        if (e.m_tag.compare(_T("Source")) == 0) {
            m_meta->PopulateMetaUpwards(m_folders, e.m_attr[_T("sourcePath")]);
            m_currSource = m_meta->GetSourceFromPath(e.m_attr[_T("sourcePath")]);
        }
        else if (e.m_tag.compare(_T("Query")) == 0) {
            m_currSource = m_meta->CreateMetaFile(_T("query"), _T(".\\"));
        }
        else if (e.m_tag.compare(_T("Definition")) == 0)
        {
            m_defStack.push(new CEclDefinition());
        }
        else if (e.m_tag.compare(_T("Import")) == 0)
        {
            m_importStack.push(new CEclImport());
        }
        else if (e.m_tag.compare(_T("Param")) == 0)
        {
            m_paramStack.push(new CEclParam());
        }
        return retval;
    }

    virtual HRESULT STDMETHODCALLTYPE endElement(
        const wchar_t *pwchNamespaceUri,
        int cchNamespaceUri,
        const wchar_t *pwchLocalName,
        int cchLocalName,
        const wchar_t *pwchQName,
        int cchQName)
    {
        Element &e = m_stack.top();
        if (e.m_tag.compare(_T("Definition")) == 0) {
            CEclDefinition *def = m_defStack.top();
            def->UpdateAttrs(e);
            if (m_defStack.size() < 2)
            {
                if (m_currSource)
                    m_currSource->UpdateDef(def);
                m_defStack.pop();
            }
            else
            {
                CEclDefinition *defNew = new CEclDefinition(*def);
                m_defStack.pop();
                CEclDefinition *defParent = m_defStack.top();
                defParent->AddDef(defNew);
            }
        }
        else if (e.m_tag.compare(_T("Field")) == 0) {
            CEclDefinition *def = m_defStack.top();
            def->AddField(e);
        }
        else if (e.m_tag.compare(_T("Type")) == 0) {
            if (m_paramStack.size() > 0)
            {
                CEclParam *param = m_paramStack.top();
                param->AddType(e);
            }
            else
            {
                CEclDefinition *def = m_defStack.top();
                def->AddFunctionType(e);
            }
        }
        else if (e.m_tag.compare(_T("Param")) == 0) {
            CEclParam *param = m_paramStack.top();
            param->UpdateAttrs(e);
            CEclDefinition *def = m_defStack.top();
            def->AddParam(param);
            m_paramStack.pop();
        }
        else if (e.m_tag.compare(_T("content")) == 0) {
            CEclDefinition *def = m_defStack.top();
            def->AddDocumentation(e);
        }
        else if (e.m_tag.compare(_T("Import")) == 0) {
            CEclImport *import = m_importStack.top();
            import->UpdateAttrs(e);
            if (m_currSource)
                m_currSource->UpdateImport(import);
            m_importStack.pop();
        }

        return __super::endElement(pwchNamespaceUri, cchNamespaceUri, pwchLocalName, cchLocalName, pwchQName, cchQName);
    }
};

bool CEclMeta::MetaExists(const std::_tstring & key)
{
    return m_masterMeta.find(key) != m_masterMeta.end();
}

CEclFile *CEclMeta::GetSourceFileFromPath(const std::wstring & path)
{
    boost::filesystem::path pathIn = path;
    std::wstring key = _T("");
    std::wstring stem = _T("");

    while (pathIn.size()) {
        stem = pathIn.stem().c_str();
        if (key.size())
            key = stem + _T(".") + key;
        else
            key = stem;
        if (MetaExists(key))
            return (CEclFile *)m_masterMeta[key].get();
        pathIn = pathIn.parent_path();
    }
    return NULL;
}

CEclFile *CEclMeta::GetSourceFromPath(const std::wstring & path)
{
    boost::filesystem::path pathIn = path;
    for (std::map<std::wstring, StlLinked<CEclMetaData>>::iterator itr = m_masterMeta.begin(); itr != m_masterMeta.end(); ++itr)
    {
        CEclMetaData *meta = itr->second;
        if (meta != NULL && meta->HasPath())
        {
            if (boost::filesystem::equivalent(meta->GetPath(), pathIn))
            {
                return (CEclFile *)meta;
            }
        }
    }
    return NULL;
}

bool CEclMeta::GetPathFromModule(const std::_tstring & module, const WPathVector & folders, std::_tstring & retPath, bool &retIsFolder)
{
    std::wstring dirpath = _T("");
    typedef boost::tokenizer<boost::char_separator<TCHAR>, std::_tstring::const_iterator, std::_tstring> tokenizer;
    boost::char_separator<TCHAR> sep(_T("."));
    tokenizer tokens(module, sep);
    tokenizer::iterator tok_iter = tokens.begin();
    std::wstring mod = _T("");
    bool recursing = false;

    for (WPathVector::const_iterator itr = folders.begin(); itr != folders.end(); ++itr)
    {
        CString pathstr = itr->first.c_str();
        if (clib::filesystem::is_directory(pathstr.GetString())) {
            boost::filesystem::directory_iterator end_itr;
            mod = tok_iter->c_str();
            while (tok_iter != tokens.end()) {
                for (boost::filesystem::directory_iterator itrDir(pathstr.GetString()); itrDir != end_itr; ++itrDir)
                {
                    if (boost::algorithm::iequals(boost::filesystem::basename(*itrDir), mod))
                    {
                        tok_iter++;
                        if (tok_iter != tokens.end())
                        {
                            pathstr += _T("\\");
                            pathstr += mod.c_str();
                            mod = tok_iter->c_str();
                            recursing = true;
                            break;
                        }
                        else
                        {
                            retPath = itrDir->path().c_str();
                            retIsFolder = boost::filesystem::is_directory(*itrDir) ? true : false;
                            return true;
                        }
                    }
                }
                if (!recursing)
                    break;
            }
        }
    }

    return false;
}

void CEclMeta::Update(const WPathVector & folders, const std::wstring & xml)
{
    CComInitialize com;
    CComPtr<ISAXXMLReader> reader;
    HRESULT hr = reader.CoCreateInstance(CLSID_SAXXMLReader30);

    CComPtr <CECLCCMetaHandler> handler = new CECLCCMetaHandler(folders, this);
    hr = reader->putContentHandler(handler);
    if (hr == S_OK)
    {
        reader->parse(CComVariant(xml.c_str()));
    }
}

bool CEclMeta::LoadImports(const std::_tstring & path, const WPathVector & folders)
{
    bool found = false;
    bool isFolder = false;
    if (CEclFile *file = GetSourceFileFromPath(path))
    {
        EclImportMap imports = file->GetImports();
        std::wstring dirpath = _T("");
        for (EclImportMap::iterator itrImport = imports.begin(); itrImport != imports.end(); ++itrImport)
        {
            std::wstring module = itrImport->second.get()->GetRef();
            if (!MetaExists(module) && module.size())
            {
                std::wstring dirPath = _T("");

                if (GetPathFromModule(module, folders, dirPath, isFolder))
                {
                    if (isFolder)
                    {
                        m_masterMeta[module] = new CEclFolder(stringToPath(dirPath));
                    }
                    else
                    {
                        m_masterMeta[module] = new CEclFile(stringToPath(dirPath));
                    }
                    found = true;
                }
            }
        }
    }

    return found;
}

bool CEclMeta::LoadFunctions(const std::_tstring & path)
{
    bool found = false;

    for (std::map<std::wstring, StlLinked<CEclMetaData> >::iterator itr = m_masterMeta.begin(); itr != m_masterMeta.end(); ++itr)
    {
        if (CEclFile *file = dynamic_cast<CEclFile*>(itr->second.get()))
        {
            found = file->GetFunctions(m_funcs) ? true : found;
        }
    }

    return found;
}

const TCHAR * CEclMeta::GetFunctionTooltip(const std::_tstring & key, std::_tstring & tooltip)
{
    tooltip = _T("");
    std::_tstring content = _T("");

    CEclDefinition *func = NULL;
    for (EclDefinitionMap::iterator itr = m_funcs.begin(); itr != m_funcs.end(); ++itr)
    {
        if (boost::algorithm::iequals(itr->second.get()->GetFullname(), key) || boost::algorithm::iends_with(itr->second.get()->GetName(), key))
            {
            std::_tstring content = itr->second.get()->GetContent();
            if (content.size())
            {
                tooltip += _T("\n");
            }
            tooltip += itr->second.get()->GetFunctionType();
            tooltip += _T(" ") + (std::_tstring)itr->second.get()->GetName();

            CEclParam *param = NULL;
            EclParamMap params = itr->second.get()->GetParams();
            if (params.size())
            {
                tooltip += _T(" (");
            }
            for (EclParamMap::iterator itrp = params.begin(); itrp != params.end();)
            {
                param = itrp->second.get();
                tooltip += (std::_tstring)param->GetType() + _T(" ") + param->GetName();
                if (++itrp != params.end())
                {
                    tooltip += _T(", ");
                }
            }
            if (params.size())
            {
                tooltip += _T(")");
            }

            if (content.size())
            {
                tooltip += _T("\n") + content;
            }
            break;
        }
    }

    return tooltip.c_str();
}

void CEclMeta::PopulateMetaUpwards(const WPathVector & folders, const std::_tstring & path)
{
    boost::filesystem::path p = wpathToPath(path);
    bool found = false;
    std::_tstring pather = _T("");
    std::vector<std::_tstring> tokens;
    int c = 0;

    while (p.parent_path().size() > 0 && !found)
    {
        found = false;
        for (WPathVector::const_iterator itr = folders.begin(); itr != folders.end(); ++itr)
        {
            std::wstring pathstr = itr->first;
            if (boost::algorithm::iequals(pathstr, p.c_str()))
            {
                pather = pathstr;
                found = true;
                break;
            }
        }
        if (!found)
        {
            tokens.insert(tokens.begin(), p.stem().c_str());
            c++;
        }
        p = p.parent_path();
    }

    int count = 1;
    p = wpathToPath(path);
    std::_tstring dottedPath = _T("");
    typedef  std::vector <std::_tstring>::iterator tIntIter;

    for (tIntIter iter = tokens.begin(); iter != tokens.end(); iter++)
    {
        if (dottedPath.size())
            dottedPath += _T(".") + *iter;
        else
            dottedPath = *iter;
        pather += _T("\\") + *iter;
        if (!MetaExists(dottedPath))
        {
            if (count < c)
            {
                m_masterMeta[dottedPath] = new CEclFolder(pather);
            }
            else
            {
                m_masterMeta[dottedPath] = new CEclFile(path);
            }
        }
        count++;
    }
}

void CEclMeta::PopulateMeta(const boost::filesystem::wpath & fileOrDir, const std::_tstring & dottedPath, int level)
{
    if (clib::filesystem::exists(fileOrDir))
    {
        std::wstring newDottedPath;

        if (level)
        {
            if (dottedPath.empty())
                newDottedPath = fileOrDir.stem().c_str();
            else
                newDottedPath = dottedPath + _T(".") + fileOrDir.stem().c_str();
        }

        if (boost::filesystem::is_directory(fileOrDir))
        {
            if (level && !MetaExists(newDottedPath))
                m_masterMeta[newDottedPath] = new CEclFolder(fileOrDir);
            boost::filesystem::directory_iterator end_itr;
            for (boost::filesystem::directory_iterator itr(wpathToPath(fileOrDir)); itr != end_itr; ++itr)
            {
                PopulateMeta(pathToWString(*itr), newDottedPath, ++level);
            }
        }
        else if (HasValidExtension(pathToWString(fileOrDir)) && !MetaExists(newDottedPath))
        {
            m_masterMeta[newDottedPath] = new CEclFile(fileOrDir);
        }
    }
}

CEclFile *CEclMeta::CreateMetaFile(const std::_tstring & dottedPath, const std::_tstring & path)
{
    CEclFile *file = new CEclFile(path);
    m_masterMeta[dottedPath] = file;
    return file;
}

int CEclMeta::NormalizeAutoC(IAttribute *attr)
{
    typedef boost::tokenizer<boost::char_separator<TCHAR>, std::_tstring::const_iterator, std::_tstring> tokenizer;
    boost::char_separator<TCHAR> sep(_T("."));
    tokenizer tokens(m_autoString, sep);
    int tokenCount = 0;
    std::_tstring tok;
    std::_tstring autoString;
    m_autoTokens.clear();

    for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter)
    {
        tok = tok_iter->c_str();
        if (boost::algorithm::equals(tok, _T("$")))
        {
            tok = attr->GetModuleQualifiedLabel(true);
        }
        m_autoTokens.push_back(tok);
        if (tokenCount)
        {
            autoString += _T(".");
        }
        autoString += tok;
        tokenCount++;
    }
    m_autoLast = tok;
    m_autoString = autoString;
    FindImportAs();

    return tokenCount;
}

void CEclMeta::BuildTokenStr(int numTokens)
{
    std::_tstring newTokenStr = _T("");
    typedef  std::vector <std::_tstring>::iterator tIntIter;
    bool first = true;
    int count = 0;

    for (tIntIter iter = m_autoTokens.begin(); iter != m_autoTokens.end(); iter++)
    {
        if (numTokens && count++ >= numTokens)
            break;
        if (first)
        {
            m_autoString = *iter;
        }
        else
        {
            m_autoString += _T(".") + *iter;
        }
        first = false;
    }
}

bool CEclMeta::FindImportAs()
{
    bool found = false;

    if (!MetaExists(m_autoString) && m_autoTokens.size())
    {
        for (std::map<std::wstring, StlLinked<CEclMetaData> >::iterator itr = m_masterMeta.begin(); itr != m_masterMeta.end(); ++itr)
        {
            if (CEclFile *file = dynamic_cast<CEclFile*>(itr->second.get()))
            {
                if (CEclImport *ref = file->GetImportRefs(m_autoTokens[0]))
                {
                    m_autoTokens[0] = ref->GetRef();
                    BuildTokenStr();
                    found = true;
                    break;
                }
            }
        }
    }
    return found;
}

bool CEclMeta::GetMetaModuleInfo(IAttribute *attr, const std::_tstring & token, StdStringVector &set)
{
    if (!attr)
        return false;

    m_autoString = token;
    int startSize = set.size();
    NormalizeAutoC(attr);

    if (!MetaExists(m_autoString))
        return false;

    const StlLinked<CEclMetaData> meta = m_masterMeta[m_autoString];
    boost::filesystem::directory_iterator end_itr;

    // Find attribute files
    if (boost::filesystem::is_directory(meta.get()->GetPath())) {
        for (boost::filesystem::directory_iterator itr(meta.get()->GetPath()); itr != end_itr; ++itr)
        {
            boost::filesystem::wpath path = *itr;
            if (boost::filesystem::is_directory(path) || boost::algorithm::ends_with(path.extension().c_str(), ATTRIBUTE_TYPE_ECL))
            {
                AddAutoStr(set, path.stem().c_str());
            }
        }
    }

    // Check for records
    if (CEclFile *file = dynamic_cast<CEclFile*>(meta.get()))
    {
        file->GetRecordStrings(token, m_autoLast, set);
    }

    return (int)set.size() > startSize;
}

bool CEclMeta::GetRecordFields(const std::_tstring & token, const std::_tstring & key, StdStringVector &set)
{
    bool found = false;

    if (!MetaExists(key))
        return false;
    const StlLinked<CEclMetaData> meta = m_masterMeta[key];

    if (CEclFile *file = dynamic_cast<CEclFile*>(meta.get()))
    {
        CEclDefinition *def = NULL;
        if (def = file->GetDefinition(token))
        {
            for (std::map<std::wstring, std::wstring>::iterator fieldItr = def->m_fields.begin(); fieldItr != def->m_fields.end(); ++fieldItr) {
                if (boost::algorithm::iends_with(_T(".") + m_autoString, fieldItr->first))
                {
                    break;
                }
                AddAutoStr(set, fieldItr->first);
                found = true;
            }
        }
    }

    return found;
}

bool CEclMeta::GetAutoC(IAttribute *attr, const std::_tstring & token, StdStringVector &set)
{
    int startSize = set.size();

    if (token.size()) {
        bool fieldsFound = false;

        GetMetaModuleInfo(attr, token, set);

        // Look for fields in current file
        if (attr)
            fieldsFound = GetRecordFields(m_autoLast, attr->GetQualifiedLabel(true), set);

        // Check for fields in meta
        if (!fieldsFound && !GetRecordFields(m_autoLast, m_autoString, set))
        {
            // Check for mod files with records and fields
            if (m_autoTokens.size() > 1)
            {
                BuildTokenStr(m_autoTokens.size() - 1);
                GetRecordFields(m_autoLast, m_autoString, set);
            }
            // Check for fields in builder windows
            else
            {
                GetRecordFields(token, _T("query"), set);
            }
        }
    }

    return (int)set.size() > startSize;;
}

std::_tstring CEclMeta::AddAutoStr(StdStringVector &set, const std::_tstring & str)
{
    if (std::find(set.begin(), set.end(), str) == set.end())
    {
        set.push_back(str);
        return str;
    }
    return _T("");
}
