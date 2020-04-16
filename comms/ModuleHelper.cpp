#include "StdAfx.h"
#include "ModuleHelper.h"
#include "Repository.h"
#include "RepositoryImpl.h"
#include "DiskRepository.h"
#include "DiskAttribute.h"
#include "EclCC.h"

//#define DELIMTER _T(".")

const TCHAR * const CModuleHelper::DELIMTER = _T(".");
const TCHAR * const TMPDELIMTER = _T("%%TMPDELIM%%");

void CModuleHelper::ParseQualifiedLabel(const std::_tstring & _modAttrLabel, bool hasExtension)
{
    std::_tstring modAttrLabel = _modAttrLabel;
    typedef boost::tokenizer<boost::char_separator<TCHAR>, std::_tstring::const_iterator, std::_tstring> tokenizer;
    if (hasExtension) {
        boost::algorithm::replace_last(modAttrLabel, DELIMTER, TMPDELIMTER);
    }
    boost::char_separator<TCHAR> sep(DELIMTER);
    tokenizer tokens(modAttrLabel, sep);
    bool first = true;
    for (tokenizer::iterator tok_itr = tokens.begin(); tok_itr != tokens.end(); ++tok_itr)
    {
        std::_tstring label = *tok_itr;
        if (hasExtension) {
            boost::algorithm::replace_last(label, TMPDELIMTER, DELIMTER);
        }
        m_labels.push_back(label);
        if (!m_attrLabel.empty())
        {
            if (!m_moduleLabel.empty())
                m_moduleLabel += DELIMTER;
            m_moduleLabel += m_attrLabel;

            if (!first)
            {
                if (!m_moduleLabelNoRoot.empty())
                    m_moduleLabelNoRoot += DELIMTER;
                m_moduleLabelNoRoot += m_attrLabel;
            }
            else
                first = false;
        }
        m_attrLabel = label;
    }
    m_qualifiedLabel = m_moduleLabel + DELIMTER + m_attrLabel;
    m_qualifiedLabelNoRoot = m_moduleLabelNoRoot + DELIMTER + m_attrLabel;
}

CModuleHelper::CModuleHelper(const TCHAR * modAttrLabel, bool hasExtension)
{
    m_config = GetIConfig(QUERYBUILDER_CFG);
    ParseQualifiedLabel(modAttrLabel, hasExtension);
}

CModuleHelper::CModuleHelper(const std::_tstring & modAttrLabel, bool hasExtension)
{
    m_config = GetIConfig(QUERYBUILDER_CFG);
    ParseQualifiedLabel(modAttrLabel, hasExtension);
}

bool CModuleHelper::HasModuleLabel() const
{
    return !m_moduleLabel.empty();
}

const TCHAR * CModuleHelper::GetModuleLabel() const
{
    return m_moduleLabel.c_str();
}

const TCHAR * CModuleHelper::GetModuleLabelNoRoot() const
{
    return m_moduleLabelNoRoot.c_str();
}

const TCHAR * CModuleHelper::GetAttributeLabelIfDataset(std::_tstring & moduleLabel) const
{
    CModuleHelper moduleHelper(m_moduleLabel);
    moduleLabel = moduleHelper.GetModuleLabel();
    return moduleLabel.c_str();
}

const TCHAR * CModuleHelper::GetAttributeLabel() const
{
    return m_attrLabel.c_str();
}

const TCHAR * CModuleHelper::GetQualifiedLabel() const
{
    return m_qualifiedLabel.c_str();
}

const TCHAR * CModuleHelper::GetQualifiedLabelNoRoot() const
{
    return m_qualifiedLabelNoRoot.c_str();
}

unsigned int CModuleHelper::GetQualifiedDepth() const
{
    return m_labels.size();
}

const TCHAR * CModuleHelper::GetQualifiedLabel(unsigned int depth) const
{
    if (depth < m_labels.size())
        return m_labels[depth].c_str();
    return NULL;
}

unsigned int CModuleHelper::GetQualifiedLabel(StdStringVector & results) const
{
    results = m_labels;
    return results.size();
}

void CModuleHelper::LoadFolders() const
{
    for (int i = 0; i < 10; ++i)
    {
        CString text;
        switch (i)
        {
        case 0:
            text = m_config->Get(GLOBAL_COMPILER_ECLFOLDER00);
            break;
        case 1:
            text = m_config->Get(GLOBAL_COMPILER_ECLFOLDER01);
            break;
        case 2:
            text = m_config->Get(GLOBAL_COMPILER_ECLFOLDER02);
            break;
        case 3:
            text = m_config->Get(GLOBAL_COMPILER_ECLFOLDER03);
            break;
        case 4:
            text = m_config->Get(GLOBAL_COMPILER_ECLFOLDER04);
            break;
        case 5:
            text = m_config->Get(GLOBAL_COMPILER_ECLFOLDER05);
            break;
        case 6:
            text = m_config->Get(GLOBAL_COMPILER_ECLFOLDER06);
            break;
        case 7:
            text = m_config->Get(GLOBAL_COMPILER_ECLFOLDER07);
            break;
        case 8:
            text = m_config->Get(GLOBAL_COMPILER_ECLFOLDER08);
            break;
        case 9:
            text = m_config->Get(GLOBAL_COMPILER_ECLFOLDER09);
            break;
        }
        if (text.GetLength() > 0 && clib::filesystem::exists(static_cast<const TCHAR *>(text)) && clib::filesystem::is_directory(static_cast<const TCHAR *>(text)))
            m_folders.push_back(std::make_pair(static_cast<const TCHAR *>(text), true));
    }
}

const TCHAR * CModuleHelper::ModuleAttrFromPath(const std::_tstring & pathStr, std::_tstring & retLabel, std::_tstring & retModuleName, std::_tstring & retAttrName, std::_tstring & retAttrType) const
{
    LoadFolders();
    boost::filesystem::path p = wpathToPath(pathStr);
    retAttrType = ExtensionWithoutDot(p.extension().c_str());
    retAttrName = pathToWString(p.stem());
    retLabel = retAttrName;
    std::_tstring attr = retAttrName;
    if (attr.size() == 0)
    {
        attr = pathToWString(p.leaf());
    }

    for (WPathVector::const_iterator itr = m_folders.begin(); itr != m_folders.end(); ++itr)
    {
        std::wstring folderPath = itr->first;
        boost::filesystem::path p2 = wpathToPath(folderPath);
        if (boost::algorithm::istarts_with(p.c_str(), folderPath))
        {
            folderPath = p2.parent_path().c_str();
            retModuleName = pathStr.substr(folderPath.size() + 1, pathStr.size() - folderPath.size() - p.filename().size() - 2);
            retLabel = pathStr.substr(p2.size() + 1);
            boost::algorithm::replace_all(retModuleName, _T("\\"), _T("."));
            boost::algorithm::replace_all(retLabel, _T("\\"), _T("."));
            break;
        }
    };
    // Return the label
    return retLabel.c_str();
}

const CComPtr<IAttribute> CModuleHelper::GetAttribute(const std::_tstring & module, const std::_tstring & attribute, const std::_tstring & attrExt, std::_tstring & path) const
{
    CComPtr<IRepository> rep = AttachRepository();
    CComPtr<IAttribute> attr = rep->GetAttributeFast(module.c_str(), attribute.c_str(), AttributeTypeFromExtension(attrExt));
    return attr;
}
