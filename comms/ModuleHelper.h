#pragma once

#include "comms.h"
#include "StringSet.h"
#include "Attribute.h"
#include "EclMeta.h"
#include <UtilFilesystem.h>

class COMMS_API CModuleHelper
{
protected:
    StdStringVector m_labels;
    CComPtr<IConfig> m_config;
    mutable WPathVector m_folders;
    std::_tstring m_moduleLabel;
    std::_tstring m_moduleLabelNoRoot;
    std::_tstring m_attrLabel;
    std::_tstring m_qualifiedLabel;
    std::_tstring m_qualifiedLabelNoRoot;

    void ParseQualifiedLabel(const std::_tstring & modAttrLabel, bool hasExtension);

public:
    static const TCHAR * const DELIMTER;

    CModuleHelper(const TCHAR * modAttrLabel, bool hasExtension = false);
    CModuleHelper(const std::_tstring & modAttrLabel, bool hasExtension = false);

    bool HasModuleLabel() const;
    void LoadFolders() const;
    const TCHAR * GetModuleLabel() const;
    const TCHAR * GetModuleLabelNoRoot() const;

    const TCHAR * GetAttributeLabelIfDataset(std::_tstring & moduleLabel) const;
    const TCHAR * GetAttributeLabel() const;

    const TCHAR * GetQualifiedLabel() const;
    const TCHAR * GetQualifiedLabelNoRoot() const;
    unsigned int GetQualifiedDepth() const;
    const TCHAR * GetQualifiedLabel(unsigned int depth) const;
    unsigned int GetQualifiedLabel(StdStringVector & results) const;

    const TCHAR * ModuleAttrFromPath(const std::_tstring & pathStr, std::_tstring & retLabel, std::_tstring & retModuleName, std::_tstring & retAttrName, std::_tstring & retAttrType) const;
    const CComPtr<IAttribute> GetAttribute(const std::_tstring & module, const std::_tstring & attribute, const std::_tstring & attrExt, std::_tstring & path) const;
};