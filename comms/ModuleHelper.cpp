#include "StdAfx.h"
#include "ModuleHelper.h"

//#define DELIMTER _T(".")

const TCHAR * const CModuleHelper::DELIMTER = _T(".");

void CModuleHelper::ParseQualifiedLabel(const std::_tstring & modAttrLabel)
{
    typedef boost::tokenizer<boost::char_separator<TCHAR>, std::_tstring::const_iterator, std::_tstring> tokenizer;
    boost::char_separator<TCHAR> sep(DELIMTER);
    tokenizer tokens(modAttrLabel, sep);
    bool first = true;
    for (tokenizer::iterator tok_itr = tokens.begin(); tok_itr != tokens.end(); ++tok_itr)
    {
        m_labels.push_back(*tok_itr);
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
        m_attrLabel = *tok_itr;
    }
    m_qualifiedLabel = m_moduleLabel + DELIMTER + m_attrLabel;
    m_qualifiedLabelNoRoot = m_moduleLabelNoRoot + DELIMTER + m_attrLabel;
}

CModuleHelper::CModuleHelper(const TCHAR * modAttrLabel)
{
    ParseQualifiedLabel(modAttrLabel);
}

CModuleHelper::CModuleHelper(const std::_tstring & modAttrLabel)
{
    ParseQualifiedLabel(modAttrLabel);
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
