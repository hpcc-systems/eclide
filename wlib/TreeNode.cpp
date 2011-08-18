#include "stdafx.h"
#include "..\en_us\resource.h"

#include "TreeNode.h"
#include "ModuleHelper.h"

StateIDS GetStateIcon(Dali::WUState state)
{
	switch(state)
	{
	case Dali::WUStateCompiled:
		return SID_GREENCIRCLE;
	case Dali::WUStateRunning:
		return SID_GREENARROW;
	case Dali::WUStateCompleted:
		return SID_GREENTICK;
	case Dali::WUStateFailed:
		return SID_REDX;
	case Dali::WUStateArchived:
		return SID_REDFLAG;
	case Dali::WUStateAborting:
		return SID_REDARROW;
	case Dali::WUStateAborted:
		return SID_REDX;
	case Dali::WUStateBlocked:
		return SID_REDSTOP;
	case Dali::WUStateSubmitted:
		return SID_BLUEARROW;
	case Dali::WUStateScheduled:
		return SID_WARNING;
	case Dali::WUStateCompiling:
		return SID_BLUECIRCLE;
	case Dali::WUStateWait:
		return SID_REDSTOP;
	case Dali::WUStateWaitingForUpload:
		return SID_WARNING;
	case Dali::WUStateDebugRunning:
		return SID_WARNING;
	case Dali::WUStateDebugPaused:
		return SID_REDSTOP;
	default:
		break;
	}
	return SID_NONE;
}

StateIDS GetStateIcon(ATTRSTATE state)
{
	switch(state)
	{
	case ATTRSTATE_LOCKED:
		return SID_LOCK;
	case ATTRSTATE_CHECKEDOUT:
		return SID_GREENTICK;
	case ATTRSTATE_FLAGGED:
		return SID_REDFLAG;
	case ATTRSTATE_EDITED:
		return SID_PENCIL;
	case ATTRSTATE_NONE:
		return SID_BLANK;
	case ATTRSTATE_LOCKED2:
		return SID_LOCK;
	case ATTRSTATE_LOCKED3:
		return SID_REDHALT;
	case ATTRSTATE_CHECKEDOUT_SANDBOXED:
		return SID_GREENTICKWARNING;
	case ATTRSTATE_SANDBOXED:
		return SID_WARNING;
	case ATTRSTATE_CHECKEDOUT_OTHER5:
		return SID_REDHALT;
	case ATTRSTATE_ORPHANED:
		return SID_REDSTOP;
	case ATTRSTATE_PLACEHOLDER:
		return SID_REDFLAG;
	default:
		break;
	}
	return SID_NONE;
}
//  ===========================================================================
CTreeNode::CTreeNode()
{
}
CTreeNode::~CTreeNode()
{
}
HTREEITEM CTreeNode::Insert(WTL::CTreeViewCtrlEx &tc, HTREEITEM hParent, HTREEITEM hAfter)
{
	if (!tc.IsWindow())
		return *this;

	AddRef();	//Release is on Delete Node Message.
	TVINSERTSTRUCT insertStruct;
	insertStruct.hParent = hParent;
	ATLASSERT(hAfter == TVI_LAST || hAfter == TVI_FIRST);
	insertStruct.hInsertAfter = hAfter;
	insertStruct.item.mask = TVIF_CHILDREN | TVIF_PARAM | TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_STATE;
	insertStruct.item.stateMask = TVIS_STATEIMAGEMASK;
	insertStruct.item.state = INDEXTOSTATEIMAGEMASK(0);
	insertStruct.item.pszText = LPSTR_TEXTCALLBACK;
	insertStruct.item.cchTextMax = 256;
	insertStruct.item.iImage = I_IMAGECALLBACK;
	insertStruct.item.iSelectedImage = I_IMAGECALLBACK;
	insertStruct.item.cChildren = I_CHILDRENCALLBACK;
	insertStruct.item.lParam = reinterpret_cast<LPARAM>(this);
	WTL::CTreeItem::operator=(tc.InsertItem(&insertStruct));
	return *this;
}
HTREEITEM CTreeNode::InsertBelow(WTL::CTreeViewCtrlEx &tc, HTREEITEM parent, bool bLast)
{
	return Insert(tc, parent, bLast ? TVI_LAST : TVI_SORT);
}

CTreeNode *CTreeNode::GetParentNode() const
{
	WTL::CTreeItem item( GetParent() );
	if ( item.IsNull() )
		return 0;
	else
		return (CTreeNode *)item.GetData();
}

CTreeNode *CTreeNode::GetChildNode() const
{
	WTL::CTreeItem item( GetChild() );
	if ( item.IsNull() )
		return 0;
	else
		return (CTreeNode *)item.GetData();
}

CTreeNode *CTreeNode::GetNextVisibleItem() const
{
	WTL::CTreeItem next(GetNextVisible());
	if ( next.IsNull() )
		return 0;
	else
		return (CTreeNode *)next.GetData();
}

CTreeNode *CTreeNode::GetNextSiblingItem() const
{
	WTL::CTreeItem next(GetNextSibling());
	if ( next.IsNull() )
		return 0;
	else
		return (CTreeNode *)next.GetData();
}

void CTreeNode::GetDispInfo(TVITEM & item)
{
	if (item.mask & TVIF_CHILDREN)
		item.cChildren = GetDispChildren();
	if (item.mask & TVIF_TEXT)
		item.pszText = const_cast<TCHAR *>(GetDispText());
	if (item.mask & TVIF_IMAGE)
		item.iImage = GetDispImage();
	if (item.mask & TVIF_SELECTEDIMAGE)
		item.iSelectedImage = GetDispSelectedImage();
}

int CTreeNode::GetDispChildren()
{
	return 1;
}

int CTreeNode::GetDispImage()
{
	return IID_FOLDER_CLOSED;
}

int CTreeNode::GetDispSelectedImage()
{
	return IID_FOLDER_OPEN;
}

void CTreeNode::OnDblclk(const WTL::CTreeItem & /*ti*/)
{
}

void CTreeNode::OnContextMenu(WTL::CTreeViewCtrlEx & /*tc*/, CPoint & /*point*/)
{
}

bool CTreeNode::OnCommand(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	return false;
}

void CTreeNode::Refresh()
{
}

bool CTreeNode::CanEdit()
{
	return false;
}

bool CTreeNode::Rename(const std::_tstring & /*txt*/)
{
	ATLASSERT(false);
	return false;
}

const TCHAR * CTreeNode::GetToolTip(std::_tstring & result)
{
	result = GetDispText();
	return result.c_str();
}

void CTreeNode::ReplaceWith(CTreeNode *newNode)
{
	newNode->AddRef();  //Same as Insert
	newNode->m_hTreeItem = m_hTreeItem;
	newNode->m_pTreeView = m_pTreeView;
	newNode->SetData((DWORD_PTR)newNode);
	Release();
}

unsigned int CTreeNode::GetDepth() const
{
	CTreeNode * parent = GetParentNode();
	if (parent)
		return parent->GetDepth() + 1;
	return 0;
}

bool CTreeNode::IsExpanded()
{
	return (WTL::CTreeItem::GetState(TVIS_EXPANDED) & (TVIS_EXPANDED | TVIS_EXPANDEDONCE)) != 0;
}

bool CTreeNode::IsSelected()
{
	return (WTL::CTreeItem::GetState(TVIS_SELECTED) & TVIS_SELECTED) != 0;
}
//  ===============================================================================================
namespace TreeNode
{
//  CTreeNode Helper Functions  ---
CTreeNode * Locate(const CModuleHelper & modHelper, CTreeNode * root, unsigned int depth)
{
	if (depth >= modHelper.GetQualifiedDepth())
		return false;

	root->Expand();
	for (WTL::CTreeItem itr = root->GetChild(); itr != NULL; itr = itr.GetNextSibling())
	{
		CString text;
		itr.GetText(text);
		if (_tcsicmp(modHelper.GetQualifiedLabel(depth), text) == 0)
		{
			if (depth == modHelper.GetQualifiedDepth() - 1)
			{
				return reinterpret_cast<CTreeNode *>(itr.GetData());
			}
			return Locate(modHelper, reinterpret_cast<CTreeNode *>(itr.GetData()), depth + 1);
		}
	}
	return NULL;
}

CTreeNode * Locate(const std::_tstring & qualifiedLabel, CTreeNode * startingNode)
{
	return Locate(qualifiedLabel, startingNode, 0);
}

bool Select(const std::_tstring & qualifiedLabel, CTreeNode * startingNode, bool bExpand)
{
	CTreeNode * found = Locate(qualifiedLabel, startingNode, 0);
	if (found)
	{
		found->Select();
		if (bExpand)
			found->Expand();
		found->EnsureVisible();
		return true;
	}
	return false;
}

bool RefreshChildren(const std::_tstring & qualifiedLabel, CTreeNode * startingNode)
{
	CTreeNode * found = Locate(qualifiedLabel, startingNode, 0);
	if (found)
	{
		found->Expand(TVE_COLLAPSE | TVE_COLLAPSERESET);
		found->Expand();
		return true;
	}
	return false;
}

int GetNodeIndex(CTreeNode *node)
{
	int index = 0;
	CTreeNode *parent = node->GetParentNode();
	if ( parent )
	{
		for (WTL::CTreeItem ti = parent->GetChild(); ti != NULL; ti = ti.GetNextSibling())
		{
			if ( ti == *node )
				return index;
			index++;
		}
	}
	return -1;
}
}
//  ===============================================================================================
