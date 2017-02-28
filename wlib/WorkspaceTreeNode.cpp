#include "StdAfx.h"
#include "..\en_us\resource.h"

#include "WorkspaceTreeNode.h"

#include <util.h> //clib
#include <utilDateTime.h> //clib
#include <thread.h>

const TCHAR * const SYNTAXWARNING = _T("Warning");
const TCHAR * const SYNTAXERROR = _T("Error");
const TCHAR * const NULL_STRING = _T("");
static const CString LF(_T("\n"));

//  ===============================================================================================
CWorkspaceBaseNode::CWorkspaceBaseNode()
{
	ATLASSERT(false);
}
CWorkspaceBaseNode::CWorkspaceBaseNode(IWorkspaceSlot *pOwner, IRepositorySlot * pRepOwner, IDaliSlot * pDaliOwner) : m_owner(pOwner), m_repOwner(pRepOwner), m_daliOwner(pDaliOwner)
{
}
//  ===============================================================================================
CWorkspaceRootNode::CWorkspaceRootNode()
{
	ATLASSERT(false);
}

CWorkspaceRootNode::CWorkspaceRootNode(IWorkspaceSlot *pOwner, IRepositorySlot * pRepOwner, IDaliSlot * pDaliOwner) : CWorkspaceBaseNode(pOwner, pRepOwner, pDaliOwner)
{
}

CWorkspaceRootNode::~CWorkspaceRootNode()
{
}

HTREEITEM CWorkspaceRootNode::InsertBelow(WTL::CTreeViewCtrlEx &tc, HTREEITEM parent, bool bLast)
{
	m_pTreeView = &tc;
	return NULL;
}

BOOL CWorkspaceRootNode::Expand(UINT nCode)
{
	if ((nCode & TVE_COLLAPSE) == TVE_COLLAPSE)
		GetTreeView()->DeleteAllItems();
	else if (GetTreeView()->GetCount() == 0)
		ItemExpanding();
	return true;
}

void CWorkspaceRootNode::ItemExpanding()
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	m_loading = new CLoadingNode();
	m_loading->InsertBelow(*GetTreeView(), TVI_ROOT);
	clib::thread run(__FUNCTION__, boost::bind(&GetWorkspaces, this, m_loading));
}

void CWorkspaceRootNode::GetWorkspaces(CComPtr<CWorkspaceRootNode> self, CComPtr<CLoadingNode> loading)
{
	IWorkspaceVector workspaces;
	self->m_repOwner->GetRepository()->GetWorkspaces(&workspaces, false);
	self->LoadWorkspaces(workspaces, loading);
}

void CWorkspaceRootNode::LoadWorkspaces(IWorkspaceVector & workspaces, CLoadingNode * loading)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	if (m_loading == loading)
	{
		m_loading->Delete();
		m_loading = NULL;
		WTL::CLockWindowUpdate lock(*GetTreeView());
		for (IWorkspaceVector::const_iterator itr = workspaces.begin(); itr != workspaces.end(); ++itr)
		{
			CWorkspaceNode * newNode = new CWorkspaceNode(m_owner, m_repOwner, m_daliOwner, itr->get());
			newNode->InsertBelow(*GetTreeView(), TVI_ROOT);
			newNode->operator ()(itr->get());
		}
		m_owner->RestoreState();
	}
}

const TCHAR * CWorkspaceRootNode::GetID()
{
	return _T("Workspaces");
}

const TCHAR * CWorkspaceRootNode::GetDispText()
{
	return _T("Workspaces");
}

//  ===============================================================================================
CWorkspaceNode::CWorkspaceNode()
{
}
CWorkspaceNode::CWorkspaceNode(IWorkspaceSlot *pOwner, IRepositorySlot * pRepOwner, IDaliSlot * pDaliOwner, IWorkspace * workspace, bool showChildren) : CWorkspaceBaseNode(pOwner, pRepOwner, pDaliOwner), m_workspace(workspace), m_showChildren(showChildren)
{
}
CWorkspaceNode::~CWorkspaceNode()
{
}

int CWorkspaceNode::GetDispChildren()
{
	if (!m_showChildren)
		return 0;
	return __super::GetDispChildren();
}

void CWorkspaceNode::ItemExpanding()
{
	if (!m_showChildren)
		return;
	IWorkspaceVector workspaces;
	IWorkspaceItemVector windows;
	//m_owner->OnLoading();
	m_workspace->GetChildWorkspaces(workspaces);
	m_workspace->GetWindows(windows);
	//m_owner->OnLoaded(m_module, attributes);
	WTL::CLockWindowUpdate lock(*GetTreeView());
	for (IWorkspaceVector::const_iterator itr = workspaces.begin(); itr != workspaces.end(); ++itr)
	{
		CWorkspaceNode * newNode = new CWorkspaceNode(m_owner, m_repOwner, m_daliOwner, itr->get());
		newNode->InsertBelow(*GetTreeView(), *this);
		newNode->operator ()(itr->get());
	}
	for (IWorkspaceItemVector::const_iterator itr = windows.begin(); itr != windows.end(); ++itr)
	{
		switch(itr->get()->GetType())
		{
		case WORKSPACE_ITEM_BUILDER:
			{
				CBuilderWindowNode * newNode = new CBuilderWindowNode(m_owner, m_repOwner, m_daliOwner, itr->get());
				newNode->InsertBelow(*GetTreeView(), *this);
				newNode->operator ()(itr->get());
			}
			break;
		case WORKSPACE_ITEM_ATTRIBUTE:
			{
				CAttributeWindowNode * newNode = new CAttributeWindowNode(m_owner, m_repOwner, m_daliOwner, itr->get());
				newNode->InsertBelow(*GetTreeView(), *this);
				newNode->operator ()(itr->get());
			}
			break;
		case WORKSPACE_ITEM_GRAPH:
			break;
		}
	}
}
const TCHAR * CWorkspaceNode::GetID()
{
	return m_workspace->GetID();
}
const TCHAR * CWorkspaceNode::GetDispText()
{
	return m_workspace->GetLabel();
}
int CWorkspaceNode::GetDispImage()
{
	return IID_PLUGINFOLDER_CLOSED;
}
int CWorkspaceNode::GetDispSelectedImage()
{
	return IID_PLUGINFOLDER_CLOSED;
}
IWorkspace * CWorkspaceNode::GetWorkspace()
{
	return m_workspace;
}
void CWorkspaceNode::operator()(IWorkspace * workspace)
{
	if (!m_showChildren && GetTreeView())
	{
		SetState(-1, TVIS_STATEIMAGEMASK);
		SetState(INDEXTOSTATEIMAGEMASK(SID_BLANK), TVIS_STATEIMAGEMASK);
	}
}
//  ===========================================================================
CBuilderWindowNode::CBuilderWindowNode()
{
}
CBuilderWindowNode::CBuilderWindowNode(IWorkspaceSlot *pOwner, IRepositorySlot * pRepOwner, IDaliSlot * pDaliOwner, IWorkspaceItem * window) : CWorkspaceBaseNode(pOwner, pRepOwner, pDaliOwner), m_window(window)
{
}

CBuilderWindowNode::~CBuilderWindowNode()
{
}

int CBuilderWindowNode::GetDispChildren()
{
	return m_window->GetWorkunitCount() > 0 ? 1 : 0;
}

void CBuilderWindowNode::ItemExpanding()
{
	Dali::IWorkunitVector workunits;
	m_window->GetWorkunits(&workunits);
	for(Dali::IWorkunitVector::const_iterator itr = workunits.begin(); itr != workunits.end(); ++itr)
	{
		CWorkunitNode * newNode = new CWorkunitNode(this, itr->get());
		newNode->InsertBelow(*GetTreeView(), *this);
		newNode->operator ()(Dali::WUEventUpdated, itr->get());
	}
}

const TCHAR * CBuilderWindowNode::GetID()
{
	return m_window->GetID();
}

const TCHAR * CBuilderWindowNode::GetDispText()
{
	return m_window->GetLabel();
}

int CBuilderWindowNode::GetDispImage()
{
	return IID_DOCUMENT;
}

int CBuilderWindowNode::GetDispSelectedImage()
{
	return GetDispImage();
}

IWorkspaceItem * CBuilderWindowNode::GetWorkspaceItem()
{
	return m_window;
}

void CBuilderWindowNode::operator()(IWorkspaceItem * window)
{
	if (GetTreeView())
	{
		SetState(-1, TVIS_STATEIMAGEMASK);
		SetState(INDEXTOSTATEIMAGEMASK(SID_BLANK), TVIS_STATEIMAGEMASK);
	}
}
//  ===========================================================================
CAttributeWindowNode::CAttributeWindowNode()
{
}

CAttributeWindowNode::CAttributeWindowNode(IWorkspaceSlot *pOwner, IRepositorySlot * pRepOwner, IDaliSlot * pDaliOwner, IWorkspaceItem * window) : CAttributeNode(pRepOwner, window->GetAttribute(), true), m_window(window)
{
}

CAttributeWindowNode::~CAttributeWindowNode()
{
}

IWorkspaceItem * CAttributeWindowNode::GetWorkspaceItem()
{
	return m_window;
}

void CAttributeWindowNode::operator()(IWorkspaceItem * window)
{
	CAttributeNode::operator()(window->GetAttribute(), false, NULL, false);
}
//  ===========================================================================
