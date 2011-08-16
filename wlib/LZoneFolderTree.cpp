#include "StdAfx.h"
#include "..\en_us\resource.h"

#include "FileSpray.h"
#include "LZoneFolderTree.h"

using namespace WsFileSpray;

CLZoneFolderTree::CLZoneFolderTree(void)
{
}

CLZoneFolderTree::~CLZoneFolderTree(void)
{
}

//-----------------------------------------------------------------------------
//
//	Methods
//

void CLZoneFolderTree::SetupImageList(bool bSet/*= true*/) 
{
	if(bSet)
	{
//		if(m_ilist.CreateFromImage(IDB_FOLDERS, 24, 0, CLR_DEFAULT, IMAGE_BITMAP))
		BOOL create=m_ilist.Create(16, 16, ILC_MASK | ILC_COLOR32, 2, 0);
		ATLASSERT(create);
		if(create)
		{
			HICON hIcon=::LoadIcon(ATL::_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(IDI_FOLDER_OPEN)); 
			ATLASSERT(hIcon);
			m_ilist.AddIcon(hIcon);
			hIcon=::LoadIcon(ATL::_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(IDI_FOLDER_CLOSED)); 
			ATLASSERT(hIcon);
			m_ilist.AddIcon(hIcon);
			SetImageList(m_ilist, TVSIL_NORMAL);
		}
	}
	else
	{
		m_ilist.Destroy(); 
	}
}	

bool CLZoneFolderTree::InsertRootNode(const std::_tstring & ip, WsFileSpray::IFileSprayPath * currSel)
{
	m_netAddress = ip;//m_currSel->GetIP();
	m_currSel = currSel;
	if (m_currSel)
	{
		IFileSprayPathVector parentage;
		m_currSel->GetParentage(&parentage);
		IFileSprayPathVector::reverse_iterator itr = parentage.rbegin();
		if (itr != parentage.rend())
		{
			HTREEITEM hfound = InsertNode(*itr, 0, 0);
			Expand(hfound);
			++itr;
			for(; itr != parentage.rend(); ++itr)
			{
				hfound = Locate(*itr, hfound);
				if (!hfound)
				{
					break;
				}

				Expand(hfound);
				SelectItem(hfound);
			}
		}
	}

	return true;
}

HTREEITEM CLZoneFolderTree::InsertNode(IFileSprayPath* path, HTREEITEM hparent, HTREEITEM hinsertAfter)
{
	TV_INSERTSTRUCT tvis;

	tvis.hParent = hparent;
	tvis.hInsertAfter = hinsertAfter;
	tvis.item.mask =  TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN | TVIF_PARAM; 
	tvis.item.pszText = LPSTR_TEXTCALLBACK;
	tvis.item.iImage = I_IMAGECALLBACK; 
	tvis.item.iSelectedImage = I_IMAGECALLBACK;
	tvis.item.cChildren = I_CHILDRENCALLBACK;
	path->AddRef();
	tvis.item.lParam=reinterpret_cast<LPARAM>(path);
	HTREEITEM hthisItem=InsertItem(&tvis);
	if(hthisItem==0)
	{
		path->Release();
		return 0;
	}
	return hthisItem;
}

HTREEITEM CLZoneFolderTree::Locate(IFileSprayPath* path, HTREEITEM hitem)
{
	CTreeItem item(hitem, this);
	while(item)
	{
		DWORD_PTR data = item.GetData();
		IFileSprayPath* itemPath=reinterpret_cast<IFileSprayPath*>(data);
		if (path == itemPath)
		{
			return item;
		}
		item = item.GetNextVisible();
	}
	return NULL;
}

WsFileSpray::IFileSprayPath * CLZoneFolderTree::GetCurrSel()
{
	return m_currSel;
}

unsigned int CLZoneFolderTree::InsertChildren(
	const IFileSprayPathVector& paths, 
	HTREEITEM hparent,
	IFileSprayPath* expandchild)
{
	unsigned int retVal = 0;
	HTREEITEM hlastchild=0;

	for(IFileSprayPathVector::const_iterator itr=paths.begin(); itr != paths.end(); ++itr)
	{
		if((*itr)->IsDir())
		{
			hlastchild=InsertNode(*itr, hparent, hlastchild);
			++retVal;
		}
	}
	return retVal;
}

//-----------------------------------------------------------------------------
//
//	Messages
//

LRESULT CLZoneFolderTree::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	SetupImageList(false); // destroys image list
	DeleteAllItems(); // will cause delete notification to free data under LPARAM
	return 0;
}

LRESULT CLZoneFolderTree::OnTreeGetDispInfo(WPARAM /*wParam*/, LPNMHDR lpnmhdr, BOOL& /*bHandled*/)
{
	LPNMTVDISPINFO lptvdi=reinterpret_cast<LPNMTVDISPINFO>(lpnmhdr); 
	CTreeItem item(lptvdi->item.hItem, this);
	DWORD_PTR data=item.GetData();

	if(lptvdi->item.hItem==GetRootItem())
	{
		if((lptvdi->item.mask & TVIF_IMAGE) || (lptvdi->item.mask & TVIF_SELECTEDIMAGE))
		{
			bool bExpanded=(GetItemState(lptvdi->item.hItem, 0xFF) & TVIS_EXPANDED)!= 0;
			lptvdi->item.iImage=lptvdi->item.iSelectedImage = bExpanded?1:0;
		}

		if(lptvdi->item.mask & TVIF_TEXT)
		{
			lptvdi->item.pszText=const_cast<TCHAR*>(m_netAddress.c_str());
			lptvdi->item.cchTextMax=m_netAddress.length();
		}
	}
	else
	{
		IFileSprayPath* path=reinterpret_cast<IFileSprayPath*>(data);

		if(!path)
			return 0;

		if((lptvdi->item.mask & TVIF_IMAGE) || (lptvdi->item.mask & TVIF_SELECTEDIMAGE))
		{
			bool bExpanded=(GetItemState(lptvdi->item.hItem, 0xFF) & TVIS_EXPANDED)!= 0;
			lptvdi->item.iImage=lptvdi->item.iSelectedImage = bExpanded?1:0;
		}

		if(lptvdi->item.mask & TVIF_TEXT)
		{
			TCHAR* name=const_cast<TCHAR*>(path->GetName());
			lptvdi->item.pszText=name;
			lptvdi->item.cchTextMax=lstrlen(name);
		}

		if(lptvdi->item.mask & TVIF_CHILDREN)
		{
			if (m_hasNoChildren.find(path) == m_hasNoChildren.end())
			{
				lptvdi->item.cChildren=1;
			}
			else
			{
				ATLASSERT(m_hasNoChildren[path] == TRUE);
				lptvdi->item.cChildren=0;
			}
		}
	}

	return 0;
}

LRESULT CLZoneFolderTree::OnTreeItemExpanding(WPARAM /*wParam*/, LPNMHDR lpnmhdr, BOOL& /*bHandled*/)
{
	LPNMTREEVIEW pnmtv = reinterpret_cast<LPNMTREEVIEW>(lpnmhdr);
	TVITEM* pItem = &pnmtv->itemNew;
	
	if(!pItem || !pItem->hItem)
		return 1;

	if(pnmtv->action==TVE_EXPAND)
	{
		CWaitCursor cursor;
		CTreeItem item(pItem->hItem, this);
		DWORD_PTR data=item.GetData();

		IFileSprayPath* path=reinterpret_cast<IFileSprayPath*>(data);
		if(path)
		{
			IFileSprayPathVector paths;
			path->GetChildren(&paths, _T("*."));
			unsigned addedcount=InsertChildren(paths, pItem->hItem);
			if(!addedcount)
			{
				m_hasNoChildren[path] = true;
				Invalidate();
			}
		}
	}

	return 0;
}

LRESULT CLZoneFolderTree::OnTreeItemExpanded(WPARAM /*wParam*/, LPNMHDR lpnmhdr, BOOL& /*bHandled*/)
{
	LPNMTREEVIEW pnmtv = reinterpret_cast<LPNMTREEVIEW>(lpnmhdr);

	if(pnmtv->action & TVE_COLLAPSE)
	{
		TVITEM* pItem = &pnmtv->itemNew;
	
		if(!pItem || !pItem->hItem)
			return 0;

		Expand(pItem->hItem, TVE_COLLAPSE | TVE_COLLAPSERESET);
		// TVE_COLLAPSERESET should clear TVIS_EXPANDEDONCE if it was set
		ATLASSERT(!(GetItemState(pItem->hItem, 0xFF) & TVIS_EXPANDEDONCE));
	}
	return 0;
}

LRESULT CLZoneFolderTree::OnTreeItemDeleted(WPARAM /*wParam*/, LPNMHDR lpnmhdr, BOOL& /*bHandled*/)
{
	LPNMTREEVIEW pnmtv = reinterpret_cast<LPNMTREEVIEW>(lpnmhdr);
	TVITEM* pItem = &pnmtv->itemOld;
	
	if(pItem)
	{
		IUnknown* iface=reinterpret_cast<IUnknown*>(pItem->lParam);
		if(iface)
			iface->Release(); 
	}
	return 0;
}

LRESULT CLZoneFolderTree::OnTreeItemSelChanged(WPARAM wParam, LPNMHDR lpnmhdr, BOOL& bHandled)
{
	LPNMTREEVIEW pnmtv = reinterpret_cast<LPNMTREEVIEW>(lpnmhdr);
	CTreeItem item(pnmtv->itemNew.hItem, this);
	if (item)
	{
		m_currSel = reinterpret_cast<IFileSprayPath*>(item.GetData());
	}
	return 0;
}
