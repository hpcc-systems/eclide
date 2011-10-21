#include "StdAfx.h"

#include "Selection.h"

//  ===========================================================================
class NoCaseCompare
{
public:
	bool operator ()(const std::_tstring & l, const std::_tstring & r) const
	{
		return _tcsicmp(l.c_str(), r.c_str()) < 0;
	}
};
//  ===========================================================================
CSelection::CSelection(WTL::CTreeViewCtrlEx * tree)
{
	m_tree = tree;
}

CSelection::~CSelection(void)
{
}

void CSelection::Clear()
{
	CWaitCursor wait;
	m_attrs.clear();
	WTL::CTreeItem item = m_tree->GetRootItem();
	while (item)
	{
		InitState((CTreeNode*)item.GetData(), THREESTATE_UNCHECKED);
		item = item.GetNextVisible();
	}
}

THREESTATE CSelection::CalcWorkspaceState(IWorkspace * workspace) const
{
	bool unchecked = false;
	bool checked = false;

	IWorkspaceVector workspaces;
	workspace->GetChildWorkspaces(workspaces);
	for(IWorkspaceVector::const_iterator itr = workspaces.begin(); itr != workspaces.end(); ++itr)
	{
		switch(CalcWorkspaceState(itr->get()))
		{
		case THREESTATE_CHECKED:
			checked = true;
			break;
		case THREESTATE_UNCHECKED:
			unchecked = true;
			break;
		case THREESTATE_PARTIALCHECKED:
			return THREESTATE_PARTIALCHECKED;
		}
	}

	IWorkspaceItemVector workspaceItems;
	workspace->GetWindows(workspaceItems);
	for(IWorkspaceItemVector::const_iterator itr = workspaceItems.begin();itr != workspaceItems.end(); ++itr)
	{
		if (itr->get()->GetType() == WORKSPACE_ITEM_ATTRIBUTE)
		{
			AttributeStateMap::const_iterator found = m_attrs.find(itr->get()->GetAttribute());
			if (found != m_attrs.end() && found->second.m_checked == true)
				checked = true;
			else
				unchecked = true;
		}
		if (checked && unchecked)
			return THREESTATE_PARTIALCHECKED;
	}
	if (checked)
		return THREESTATE_CHECKED;
	return THREESTATE_UNCHECKED;
}

THREESTATE CSelection::CalcModuleState(IModule * module) const
{
	bool unchecked = false;
	bool checked = false;

	IModuleVector modules;
	module->GetModules(modules);
	for(IModuleVector::iterator itr = modules.begin(); itr != modules.end(); ++itr)
	{
		switch (CalcModuleState(itr->get()))
		{
		case THREESTATE_CHECKED:
			checked = true;
			break;
		case THREESTATE_UNCHECKED:
			unchecked = true;
			break;
		default:
			return THREESTATE_PARTIALCHECKED;
		}
		if (checked && unchecked)
			return THREESTATE_PARTIALCHECKED;
	}

	unsigned int attrCount = 0;
	for(AttributeStateMap::const_iterator itr = m_attrs.begin(); itr != m_attrs.end(); ++itr)
	{
		if (boost::algorithm::iequals(module->GetQualifiedLabel(), itr->first->GetModule()->GetQualifiedLabel()))
		{
			attrCount++;
			if (itr->second.m_checked == true)
				checked = true;
			else
				unchecked = true;
		}
		if (checked && unchecked)
			return THREESTATE_PARTIALCHECKED;
	}
	if (checked)
	{
		IAttributeVector attributes;
		return module->GetAttributes(attributes, true) - attrCount == 0 ? THREESTATE_CHECKED : THREESTATE_PARTIALCHECKED;	//(Module could contain more attrs than the ones we know about in m_attrs)
	}
	return THREESTATE_UNCHECKED;
}

THREESTATE CSelection::CalcAttributeState(IAttribute * attribute) const
{
	if (!attribute->Exists())
		return THREESTATE_BLANK;

	AttributeStateMap::const_iterator itr = m_attrs.find(attribute);
	if (itr != m_attrs.end())
	{
		return itr->second.m_checked ? THREESTATE_CHECKED : THREESTATE_UNCHECKED;
	}
	return THREESTATE_UNCHECKED;
}

THREESTATE CSelection::CalcAttributeHistoryState(IAttribute * attribute, int version) const
{
	AttributeStateMap::const_iterator itr = m_attrs.find(attribute);
	if (itr != m_attrs.end() && itr->second.m_version == version && itr->second.m_checked)
	{
		return THREESTATE_RADIO_CHECKED;
	}
	return THREESTATE_RADIO_UNCHECKED;
}

void CSelection::InitState(CTreeNode * item, THREESTATE knownState)
{
	if (CComQIPtr<CWorkspacePairNode> node = item)
	{
		SetCheckThreeState(*item, knownState != THREESTATE_UNKNOWN ? knownState : CalcWorkspaceState(node->m_lhs));
	}
	else if (CComQIPtr<CModulePairNode> node = item)
	{
		SetCheckThreeState(*item, knownState != THREESTATE_UNKNOWN ? knownState : CalcModuleState(node->m_lhs));
	}
	else if (CComQIPtr<CAttributePairNode> node = item)
	{
		THREESTATE state = knownState != THREESTATE_UNKNOWN ? knownState : CalcAttributeState(node->m_lhs);
		SetState(node->m_lhs, state == THREESTATE_CHECKED);
		SetCheckThreeState(*item, state);
	}
	else if (CComQIPtr<CAttributeHistoryPairNode> node = item)
	{
		CComQIPtr<CAttributePairNode> parent = item->GetParentNode();
		IAttribute * attr = parent->m_lhs;
		if (m_attrs[attr].m_version == 0 && *parent->GetChildNode() == *item && !attr->IsSandboxed() && m_attrs[attr].m_checked)
			SetCheckThreeState(*item, THREESTATE_RADIO_CHECKED);
		else
			SetCheckThreeState(*item, CalcAttributeHistoryState(parent->m_lhs, node->m_lhs->GetVersion()));
	}
}

void CSelection::SetState(IAttribute * attribute, bool checked)
{
	ATLASSERT(attribute);
	if (!attribute->Exists())
		return;

	AttributeStateMap::const_iterator itr = m_attrs.find(attribute);
	if (itr == m_attrs.end())	//Does not exist...
	{
		m_attrs[attribute].m_version = 0;
		m_attrs[attribute].m_history = NULL;
		m_attrs[attribute].m_checked = false;
	}
	if (m_attrs[attribute].m_checked != checked)
	{
		m_attrs[attribute].m_version = 0;
		m_attrs[attribute].m_history = NULL;
		m_attrs[attribute].m_checked = checked;
	}
}

void CSelection::ItemClicked(CTreeNode * item, IAttributeVector * attrs, IAttributeVector * dependants)
{
	THREESTATE curState = GetCheckThreeState(*item);
	THREESTATE newState = curState == THREESTATE_BUSY_CHECKED ? THREESTATE_UNCHECKED : THREESTATE_CHECKED;
	if (CComQIPtr<CWorkspacePairNode> node = item)
	{
		IWorkspace * ws = node->m_lhs;
		SetSelection(item, *attrs, newState == THREESTATE_CHECKED);
	}
	else if (CComQIPtr<CModulePairNode> node = item)
	{
		IModule * mod = node->m_lhs;
		SetSelection(item, *attrs, newState == THREESTATE_CHECKED);
	}
	else if (CComQIPtr<CAttributePairNode> node = item)
	{
		attrs->push_back(node->m_lhs.p);
		SetSelection(item, *attrs, newState == THREESTATE_CHECKED);
	}
	else if (CComQIPtr<CAttributeHistoryPairNode> node = item)
	{
		CComQIPtr<CAttributePairNode> parent = node->GetParentNode();
		CComQIPtr<CModulePairNode> gparent = parent->GetParentNode();
		IAttribute * attr = parent->m_lhs;
		m_attrs[attr].m_version = node->m_lhs->GetVersion();
		m_attrs[attr].m_history = node->m_lhs;
		m_attrs[attr].m_checked = true;
		//  TODO:  GJS - This doesn't refresh to the top of nested modules...
		Refresh(parent);
	}

	//  Dependents could be anywhere...
	if (!dependants->empty())
		SetSelection(*dependants, newState == THREESTATE_CHECKED);
}

void CSelection::Refresh(CAttributePairNode * node)
{
	SetCheckThreeState(*node, CalcAttributeState(node->m_lhs));
	//  Recalc Parents.
	if (CComQIPtr<CWorkspacePairNode> parent = node->GetParentNode())
		SetCheckThreeState(*parent, CalcWorkspaceState(parent->m_lhs));
	else if (CComQIPtr<CModulePairNode> parent = node->GetParentNode())
		SetCheckThreeState(*parent, CalcModuleState(parent->m_lhs));
	//  Recalc Children.
	for(CComQIPtr<CAttributeHistoryPairNode> child = node->GetChildNode(); child; child = child->GetNextSiblingItem())
	{
		InitState(child);
	}
}

bool CSelection::HasSelection() const
{
	for(AttributeStateMap::const_iterator itr = m_attrs.begin(); itr != m_attrs.end(); ++itr)
	{
		if (itr->second.m_checked)
			return true;
	}
	return false;
}

//class IAttributeSelectionCompare
//{
//public:
//	bool operator ()(IAttributeSelection & l, IAttributeSelection & r)
//	{
//		CString lhsModule = l.m_moduleLabel.c_str();
//		int compare = lhsModule.CompareNoCase(r.m_moduleLabel.c_str());
//		if (compare == 0)
//		{
//			CString lhs = l.m_attrLabel.c_str();
//			return lhs.CompareNoCase(r.m_attrLabel.c_str()) > 0 ? false : true;
//		}
//		else
//			return compare > 0 ? false : true;
//	}
//};

int CSelection::GetSelection(IRepository * rep, IWorkspaceVector & workspaces, IAttributeHistoryVector & attrs) const
{
	WTL::CTreeItem item = m_tree->GetRootItem();
	while(item)
	{
		CComPtr<CTreeNode> node = (CTreeNode *)item.GetData();
		if (CComQIPtr<CWorkspacePairNode> ws_node = node)
		{
			switch (GetCheckThreeState(item))
			{
			case THREESTATE_CHECKED:
			case THREESTATE_PARTIALCHECKED:
				workspaces.push_back(ws_node->m_lhs.p);
				break;
			}
		}
		item = item.GetNextVisible();
	}

	for(AttributeStateMap::const_iterator itr = m_attrs.begin(); itr != m_attrs.end(); ++itr)
	{
		if (itr->second.m_checked)
		{
			if (itr->second.m_version == 0)
				attrs.push_back(itr->first->GetAsHistory());
			else
				attrs.push_back(itr->second.m_history.p);
		}
	}
	IAttributeHistoryCompare compare;
	std::sort(attrs.begin(), attrs.end(), compare);
	return attrs.size();
}

void CSelection::SetSelection(IAttributeVector & attrs, bool checked) 
{
	if (attrs.empty())
		return;

	CWaitCursor wait;
	for(IAttributeVector::const_iterator itr = attrs.begin(); itr != attrs.end(); ++itr)
	{
		SetState(*itr, checked);
	}
	WTL::CTreeItem item = m_tree->GetRootItem();
	while(item)
	{
		InitState((CTreeNode *)item.GetData());
		item = item.GetNextVisible();
	}
}

void RecursiveRefreshChildren(CSelection * self, WTL::CTreeItem * _item, THREESTATE knownState)
{
	WTL::CTreeItem childItem = _item->GetChild();
	while(childItem)
	{
		self->InitState((CTreeNode *)childItem.GetData(), knownState);
		RecursiveRefreshChildren(self, &childItem, knownState);
		childItem = childItem.GetNextSibling();
	}
}

void CSelection::SetSelection(CTreeNode * _item, IAttributeVector & attrs, bool checked) 
{
	for(IAttributeVector::const_iterator itr = attrs.begin(); itr != attrs.end(); ++itr)
	{
		SetState(*itr, checked);  //GJS this does not work for dependent attrs!!!!
	}
	SetCheckThreeState(*_item, checked ? THREESTATE_CHECKED : THREESTATE_UNCHECKED);
	//  Refresh Ancestors
	WTL::CTreeItem item = _item->GetParent();
	while(item)
	{
		InitState((CTreeNode *)item.GetData());
		item = item.GetParent();
	}

	//  Refresh Children
	if (_item->IsExpanded())
		RecursiveRefreshChildren(this, _item, checked ? THREESTATE_CHECKED : THREESTATE_UNCHECKED);

	SetSelection(attrs, checked);
}

int CSelection::GetSelection(std::_tstring & attrs) const
{
	for(AttributeStateMap::const_iterator itr = m_attrs.begin(); itr != m_attrs.end(); ++itr)
	{
		if (itr->second.m_checked)
		{
			if (attrs.length())
				attrs += _T("\r\n");
			attrs += itr->first->GetQualifiedLabel();
		}
	}
	return attrs.size();
}

THREESTATE CSelection::GetCheckThreeState(HTREEITEM hItem) const
{
	ATLASSERT(m_tree && m_tree->IsWindow());
	UINT uRet = m_tree->GetItemState(hItem, TVIS_STATEIMAGEMASK);
	return (THREESTATE)((uRet >> 12) - 1);
}

BOOL CSelection::SetCheckThreeState(HTREEITEM hItem, THREESTATE state)
{
	ATLASSERT(m_tree && m_tree->IsWindow());
	int nCheck = (int)state;
	ATLASSERT(nCheck > THREESTATE_UNKNOWN && nCheck < THREESTATE_LAST);
	return m_tree->SetItemState(hItem, INDEXTOSTATEIMAGEMASK(nCheck+1), TVIS_STATEIMAGEMASK);
}
