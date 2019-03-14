#include "StdAfx.h"

#include "EclMeta.h"
#include "SAXHandlers.h"

class CECLCCMetaHandler : public StackHandlerImpl
{
private:

protected:
    CEclMeta *m_meta;
    CComPtr<CEclFile> m_currSource;
    std::stack<CEclDefinitionAdapt> m_defStack;
    std::stack<CEclImportAdapt> m_importStack;

public:
    CECLCCMetaHandler(CEclMeta *meta) :m_meta(meta) {};
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
            m_currSource = m_meta->GetSourceFromPath(e.m_attr[_T("sourcePath")]);
        }
        else if (e.m_tag.compare(_T("Definition")) == 0)
        {
            m_defStack.push(new CEclDefinition());
        }
        else if (e.m_tag.compare(_T("Import")) == 0)
        {
            m_importStack.push(new CEclImport());
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
            if (m_currSource)
                m_currSource->UpdateDef(def);
            m_defStack.pop();
        }
        else if (e.m_tag.compare(_T("Field")) == 0) {
            CEclDefinition *def = m_defStack.top();
            def->AddField(e);
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

CEclFile *CEclMeta::GetSourceFromPath(const std::wstring & path)
{
    boost::filesystem::path pathIn = path;
    for (std::map<std::wstring, StlLinked<CEclMetaData>>::iterator itr = m_masterMeta.begin(); itr != m_masterMeta.end(); ++itr)
    {
        CEclMetaData *meta = itr->second;
        if (meta != NULL && meta->HasPath())
        {
            if (boost::filesystem::equivalent(meta->GetPath(),pathIn))
            {
                return (CEclFile *)meta;
            }
        }
    }
    return NULL;
}

void CEclMeta::Update(const std::wstring & xml)
{
    CComInitialize com;
    CComPtr<ISAXXMLReader> reader;
    HRESULT hr = reader.CoCreateInstance(CLSID_SAXXMLReader30);

    CComPtr <CECLCCMetaHandler> handler = new CECLCCMetaHandler(this);
    hr = reader->putContentHandler(handler);
    if (hr == S_OK)
    {
        reader->parse(CComVariant(xml.c_str()));
    }
}

void CEclMeta::LoadMetaData(const WPathVector & folders)
{
    for (WPathVector::const_iterator itr = folders.begin(); itr != folders.end(); ++itr)
    {
        PopulateMeta(itr->first);
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
            if (level)
                m_masterMeta[newDottedPath] = new CEclFolder(fileOrDir);
            boost::filesystem::directory_iterator end_itr;
            for (boost::filesystem::directory_iterator itr(wpathToPath(fileOrDir)); itr != end_itr; ++itr)
            {
                PopulateMeta(pathToWString(*itr), newDottedPath, ++level);
            }
        }
        else if (HasValidExtension(pathToWString(fileOrDir)))
        {
            m_masterMeta[newDottedPath] = new CEclFile(fileOrDir);
        }
    }
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
        if (boost::algorithm::equals(tok,_T("$")))
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

void CEclMeta::BuildTokenStr()
{
    std::_tstring newTokenStr= _T("");
    typedef  std::vector <std::_tstring>::iterator tIntIter;
    bool first = true;
    for (tIntIter iter = m_autoTokens.begin(); iter != m_autoTokens.end(); iter++)
    {
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

    if (m_masterMeta.find(m_autoString) == m_masterMeta.end() && m_autoTokens.size())
    {
        for (std::map<std::wstring, StlLinked<CEclMetaData> >::iterator itr = m_masterMeta.begin(); itr != m_masterMeta.end(); ++itr)
        {
            if (CEclFile *file = dynamic_cast<CEclFile*>(itr->second.get()))
            {
                if (CEclImport *ref = file->GetImportRefs(m_autoTokens[0]))
                {
                    m_autoTokens[0] = ref->GetRef().c_str();
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
    m_autoString = token;
    int startSize = set.size();
    NormalizeAutoC(attr);

    // Look for module chaining
    if (m_masterMeta.find(m_autoString) != m_masterMeta.end())
    {
        const StlLinked<CEclMetaData> meta = m_masterMeta[m_autoString];
        if (meta)
        {
            boost::filesystem::directory_iterator end_itr;
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
        }
    }

    // Look for fields
    if (m_masterMeta.find(m_autoString) != m_masterMeta.end())
    {
        for (std::map<std::wstring, StlLinked<CEclMetaData> >::iterator itr = m_masterMeta.begin(); itr != m_masterMeta.end(); ++itr)
        {
            if (CEclFile *file = dynamic_cast<CEclFile*>(itr->second.get()))
            {
                if (CEclDefinition *def = file->GetDefinitions(m_autoLast))
                {
                    for (std::map<std::wstring, std::wstring>::iterator fieldItr = def->m_fields.begin(); fieldItr != def->m_fields.end(); ++fieldItr) {
                        if (boost::algorithm::iends_with(_T(".") + m_autoString, fieldItr->first))
                        {
                            break;
                        }
                        AddAutoStr(set, fieldItr->first);
                    }
                }
            }
        }
    }

     return (int)set.size() > startSize;
}

bool CEclMeta::GetMetaParseInfo(const std::_tstring & token, const std::_tstring & key, StdStringVector &set)
{
    const StlLinked<CEclMetaData> meta = m_masterMeta[key];
    if (!meta)
        return false;

    CEclFile *file = (CEclFile *)meta.get();
    if (file->GetDefStrings(token, set))
        return true;

    return false;
}

bool CEclMeta::GetAutoC(IAttribute *attr, const std::_tstring & token, StdStringVector &set)
{
    if (token.size()) {
        GetMetaModuleInfo(attr, token, set);
        std::_tstring key = attr ? attr->GetQualifiedLabel(true) : token;
        GetMetaParseInfo(token, key, set);
    }

    return false;
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
