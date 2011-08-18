#pragma once

#include "comms.h"
#include "StringSet.h"

class COMMS_API CModuleHelper
{
protected:
	StdStringVector m_labels;
	std::_tstring m_moduleLabel;
	std::_tstring m_moduleLabelNoRoot;
	std::_tstring m_attrLabel;
	std::_tstring m_qualifiedLabel;
	std::_tstring m_qualifiedLabelNoRoot;

	void ParseQualifiedLabel(const std::_tstring & modAttrLabel);

public:
	static const TCHAR * const DELIMTER;

	CModuleHelper(const TCHAR * modAttrLabel);
	CModuleHelper(const std::_tstring & modAttrLabel);

	bool HasModuleLabel() const;
	const TCHAR * GetModuleLabel() const;
	const TCHAR * GetModuleLabelNoRoot() const;

	const TCHAR * GetAttributeLabelIfDataset(std::_tstring & moduleLabel) const;
	const TCHAR * GetAttributeLabel() const;

	const TCHAR * GetQualifiedLabel() const;
	const TCHAR * GetQualifiedLabelNoRoot() const;
	unsigned int GetQualifiedDepth() const;
	const TCHAR * GetQualifiedLabel(unsigned int depth) const;
	unsigned int GetQualifiedLabel(StdStringVector & results) const;
};