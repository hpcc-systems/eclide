#include "StdAfx.h"

#include "MultiTreeNode.h"

//  ===============================================================================================
CLoadingNode::CLoadingNode(CMultiTreeNodeViewCtrl * tree, const std::_tstring & label)
{
	m_id = _T("...Loading...");
	m_label = label;
	m_tree = tree;
	if (m_tree)
		m_tree->PushState();
}
CLoadingNode::~CLoadingNode()
{
	if (m_tree && m_tree->IsWindow())
		m_tree->PopState();
}
const TCHAR * CLoadingNode::GetID()
{
	return m_id.c_str();
}
const TCHAR * CLoadingNode::GetDispText()
{
	return m_label.c_str();
}
int CLoadingNode::GetDispChildren()
{
	return 0;
}

int CLoadingNode::GetDispImage()
{
	return -1;
}

int CLoadingNode::GetDispSelectedImage()
{
	return GetDispImage();
}
//  ===========================================================================
CPersistTreeState::CPersistTreeState(CMultiTreeNodeViewCtrl &tree) : m_tree(tree)
{
	CTreeNodeVector selected;
	m_tree.GetSelected(selected);
	for (CTreeItem pos = m_tree.GetRootItem(); pos != NULL; pos = m_tree.GetNextItem(pos, TVGN_EX_ALL))
	{
		CTreeItem node(pos, &m_tree);
		CTreeNode *item = (CTreeNode*)node.GetData();
		std::_tstring id = item->GetID();
		if (m_tree.IsSelected(pos))
		{
			m_selected_set.insert(id);  
		}
		if ((m_tree.GetItemState(pos, TVIS_EXPANDED) & TVIS_EXPANDED) == TVIS_EXPANDED)
		{
			m_expanded_set.insert(id);  
		}
		//if (m_tree.GetItemState(pos, TVIS_FOCUSED) & TVIS_FOCUSED)
		//{
		//	m_focus = id;
		//}
	}
}

CPersistTreeState::~CPersistTreeState()
{
	if (!m_tree || !m_tree.IsWindow())
		return;
	for (CTreeItem pos = m_tree.GetRootItem(); pos != NULL; pos = m_tree.GetNextItem(pos, TVGN_EX_ALL))
	{
		CTreeItem node(pos, &m_tree);
		CTreeNode *item = (CTreeNode*)node.GetData();
		ATLASSERT(item);
		std::_tstring id = item->GetID();
		if (m_selected_set.find(id) != m_selected_set.end())
		{
			m_tree.SetItemState(pos, TVIS_SELECTED, TVIS_SELECTED);
		}
		else
		{
			m_tree.SetItemState(pos, 0, TVIS_SELECTED);
		}
		if (m_expanded_set.find(id) != m_expanded_set.end())
		{
			item->Expand();
		}
		//if (m_focus.compare(id) == 0)
		//{
		//	m_tree.SetItemState(pos, TVIS_FOCUSED, TVIS_FOCUSED);
		//}
	}
}
//  ===========================================================================

