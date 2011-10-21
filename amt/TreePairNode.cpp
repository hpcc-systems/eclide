#include "StdAfx.h"

#include "TreePairNode.h"
#include "Logger.h"

//  ===========================================================================
CWorkspacePairNode::CWorkspacePairNode(IWorkspace * lhs, IWorkspace * rhs)
{
	m_lhs = lhs;
	m_rhs = rhs;
	m_label = m_lhs->GetLabel();
	m_id = m_lhs->GetID();
	m_id += m_rhs->GetID();
}

bool CWorkspacePairNode::hasLHS()
{
	return m_lhs->Exists();
}

bool CWorkspacePairNode::hasRHS()
{
	return m_rhs->Exists();
}

const TCHAR * CWorkspacePairNode::GetID()
{
	return m_id.c_str();
}

const TCHAR * CWorkspacePairNode::GetDispText()
{
	return m_label.c_str();
}

int CWorkspacePairNode::GetDispChildren()
{
	return m_lhs->Exists() ? 1 : 0;
}

int CWorkspacePairNode::GetDispImage(const IWorkspace * ws, const IWorkspace * otherWs)
{
	if (!ws->Exists())
		return IID_FOLDER_PLACEHOLDER;

	std::_tstring myCrc = ws->GetChecksum();
	std::_tstring otherCrc = otherWs->GetChecksum();
	if (!boost::algorithm::iequals(myCrc, otherCrc))
		return IID_PLUGINFOLDER_UNMATCHED;
	return IID_PLUGINFOLDER_CLOSED;//__super::GetDispImage();
}

int CWorkspacePairNode::GetDispImage()
{
	return GetDispImage(m_lhs, m_rhs);
}

int CWorkspacePairNode::GetDispImageRHS()
{
	return GetDispImage(m_rhs, m_lhs);
}

int CWorkspacePairNode::GetDispSelectedImage(const IWorkspace * ws, const IWorkspace * otherWs)
{
	if (!ws->Exists())
		return IID_FOLDER_PLACEHOLDER;

	std::_tstring myCrc = ws->GetChecksum();
	std::_tstring otherCrc = otherWs->GetChecksum();
	if (!boost::algorithm::iequals(myCrc, otherCrc))
		return IID_PLUGINFOLDER_OPEN_UNMATCHED;

	return IID_PLUGINFOLDER_CLOSED;//__super::GetDispSelectedImage();
}
int CWorkspacePairNode::GetDispSelectedImage()
{
	return GetDispSelectedImage(m_lhs, m_rhs);
}
int CWorkspacePairNode::GetDispSelectedImageRHS()
{
	return GetDispSelectedImage(m_rhs, m_lhs);
}
ATTRSTATE CWorkspacePairNode::GetState()
{
	return ATTRSTATE_NONE;
}
ATTRSTATE CWorkspacePairNode::GetStateRHS()
{
	return ATTRSTATE_NONE;
}
//  ===========================================================================
CModulePairNode::CModulePairNode(IModule * lhs, IModule * rhs)
{
	m_lhs = lhs;
	m_rhs = rhs;
	m_label = m_lhs->GetLabel();
	m_id = m_lhs->GetID();
	m_id += m_rhs->GetID();
}

bool CModulePairNode::hasLHS()
{
	return m_lhs->Exists();
}

bool CModulePairNode::hasRHS()
{
	return m_rhs->Exists();
}

const TCHAR * CModulePairNode::GetID()
{
	return m_id.c_str();
}

const TCHAR * CModulePairNode::GetDispText()
{
	return m_label.c_str();
}

int CModulePairNode::GetDispChildren()
{
	return m_lhs->Exists() ? 1 : 0;
}

int CModulePairNode::GetDispImage(const IModule * mod, const IModule * otherMod)
{
	if (!mod->Exists())
		return IID_FOLDER_PLACEHOLDER;

	//if (mod->IsMissingPair())
	//	return IID_FOLDER_MISSING;

	if (mod->IsPlugin())
	{
		return IID_PLUGINFOLDER_CLOSED;
	}
	else if (mod->IsTrash())
	{
		if (GetDispChildren())
		{
			return IID_TRASH_FULL;
		}
		return IID_TRASH_EMPTY;
	}
	std::_tstring myCrc = mod->GetChecksum();
	std::_tstring otherCrc = otherMod->GetChecksum();
	if (!boost::algorithm::iequals(myCrc, otherCrc))
		return IID_FOLDER_UNMATCHED;
	return __super::GetDispImage();
}

int CModulePairNode::GetDispImage()
{
	return GetDispImage(m_lhs, m_rhs);
}

int CModulePairNode::GetDispImageRHS()
{
	return GetDispImage(m_rhs, m_lhs);
}

int CModulePairNode::GetDispSelectedImage(const IModule * mod, const IModule * otherMod)
{
	if (!mod->Exists())
		return IID_FOLDER_PLACEHOLDER;

	//if (mod->IsMissingPair())
	//	return IID_FOLDER_MISSING;

	if (mod->IsPlugin())
	{
		return IID_PLUGINFOLDER_OPEN;
	}
	else if (mod->IsTrash())
	{
		if (GetDispChildren())
		{
			return IID_TRASH_FULL;
		}
		return IID_TRASH_EMPTY;
	}

	std::_tstring myCrc = mod->GetChecksum();
	std::_tstring otherCrc = otherMod->GetChecksum();
	if (!boost::algorithm::iequals(myCrc, otherCrc))
		return IID_FOLDER_UNMATCHED;

	return __super::GetDispSelectedImage();
}
int CModulePairNode::GetDispSelectedImage()
{
	return GetDispSelectedImage(m_lhs, m_rhs);
}
int CModulePairNode::GetDispSelectedImageRHS()
{
	return GetDispSelectedImage(m_rhs, m_lhs);
}
ATTRSTATE CModulePairNode::GetState()
{
	switch (m_lhs->GetAccess())
	{
	case SecAccess_Write:
	case SecAccess_Full:
		return ATTRSTATE_NONE;
	}
	return ATTRSTATE_LOCKED;
}
ATTRSTATE CModulePairNode::GetStateRHS()
{
	switch (m_rhs->GetAccess())
	{
	case SecAccess_Write:
	case SecAccess_Full:
		return ATTRSTATE_NONE;
	}
	return ATTRSTATE_LOCKED;
}
//  ===========================================================================
CAttributePairNode::CAttributePairNode(IAttribute * lhs, IAttribute * rhs)
{
	ATLASSERT(lhs);
	ATLASSERT(rhs);
	m_lhs = lhs;
	m_rhs = rhs;
	m_label = m_lhs->GetLabel();
	m_id = m_lhs->GetID();
	m_id += m_rhs->GetID();
}

bool CAttributePairNode::hasLHS()
{
	return m_lhs->Exists();
}

bool CAttributePairNode::hasRHS()
{
	return m_rhs->Exists();
}

const TCHAR * CAttributePairNode::GetID()
{
	return m_id.c_str();
}

const TCHAR * CAttributePairNode::GetDispText()
{
	return m_label.c_str();
}

int CAttributePairNode::GetDispChildren()
{
	return m_lhs->Exists() ? 1 : 0;
}

int CAttributePairNode::GetDispImage(const IAttribute * attr, const IAttribute * otherAttr)
{
	if (!attr->Exists())
		return IID_DOCUMENT_PLACEHOLDER;

	//if (attr->IsMissingPair())
	//	return IID_DOCUMENT_MISSING;

	//if (!attr->IsMatchedPair())
	//	return IID_DOCUMENT_UNMATCHED;

	//  Trust local CRC first (if they have been calculated, ie we have the ECL text).	
	std::_tstring myLocalCrc = attr->GetChecksumLocal();
	std::_tstring otherLocalCrc = otherAttr->GetChecksumLocal();
	std::_tstring myCrc = attr->GetChecksum();
	std::_tstring otherCrc = otherAttr->GetChecksum();
	if (myLocalCrc.length() && otherLocalCrc.length() && boost::algorithm::equals(myLocalCrc, otherLocalCrc))
	{
		if (!myCrc.length() || !myLocalCrc.length() || !boost::algorithm::equals(myCrc, myLocalCrc))
		{
			return IID_DOCUMENT_FOLLOWUP;
		}
		return IID_DOCUMENT;
	}

	if (myCrc.length() && otherCrc.length() && boost::algorithm::iequals(myCrc, otherCrc))
		return IID_DOCUMENT;

	return IID_DOCUMENT_UNMATCHED;
}

int CAttributePairNode::GetDispImage()
{
	return GetDispImage(m_lhs, m_rhs);
}

int CAttributePairNode::GetDispImageRHS()
{
	return GetDispImage(m_rhs, m_lhs);
}

int CAttributePairNode::GetDispSelectedImage(const IAttribute * attr, const IAttribute * otherAttr)
{
	return GetDispImage(attr, otherAttr);
}
int CAttributePairNode::GetDispSelectedImage()
{
	return GetDispSelectedImage(m_lhs, m_rhs);
}
int CAttributePairNode::GetDispSelectedImageRHS()
{
	return GetDispSelectedImage(m_rhs, m_lhs);
}
ATTRSTATE CAttributePairNode::GetState() 
{
	return m_lhs->GetState();
}
ATTRSTATE CAttributePairNode::GetStateRHS() 
{
	return m_rhs->GetState();
}
//  ===========================================================================
CAttributeHistoryPairNode::CAttributeHistoryPairNode(IAttributeHistory * lhs, IAttribute * rhs)
{
	m_lhs = lhs;
	m_rhs = rhs;
	m_label = m_lhs->GetModifiedDate();
	m_id = m_lhs->GetID();
}

bool CAttributeHistoryPairNode::hasLHS()
{
	return true;
}

bool CAttributeHistoryPairNode::hasRHS()
{
	return false;
}

const TCHAR * CAttributeHistoryPairNode::GetID()
{
	return m_id.c_str();
}

const TCHAR * CAttributeHistoryPairNode::GetDispText()
{
	return m_label.c_str();
}

int CAttributeHistoryPairNode::GetDispChildren()
{
	return 0;
}

int CAttributeHistoryPairNode::GetDispImage()
{
	return IID_DOCUMENT_HISTORY;
}

int CAttributeHistoryPairNode::GetDispImageRHS()
{
	return IID_DOCUMENT_BLANK;
}

int CAttributeHistoryPairNode::GetDispSelectedImage()
{
	return IID_DOCUMENT_HISTORY;
}
int CAttributeHistoryPairNode::GetDispSelectedImageRHS()
{
	return IID_DOCUMENT_BLANK;
}
ATTRSTATE CAttributeHistoryPairNode::GetState() 
{
	return ATTRSTATE_NONE;
}
ATTRSTATE CAttributeHistoryPairNode::GetStateRHS() 
{
	return ATTRSTATE_NONE;
}
//  ===========================================================================
