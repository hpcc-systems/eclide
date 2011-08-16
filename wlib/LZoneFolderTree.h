#pragma once

#include "Topology.h"
#include "FileSpray.h"

//// forward declarations
//
//namespace WsFileSpray
//{
//__interface IFileSprayPath;
//}
//
//namespace Topology
//{
//__interface IMachine;
//}

//------------------------------------------------------------------------

class CLZoneFolderTree :
	public CWindowImpl<CLZoneFolderTree, CTreeViewCtrlEx, CWinTraitsOR<>  >
{
	typedef CLZoneFolderTree thisClass;

	CImageList m_ilist;
	std::map<WsFileSpray::IFileSprayPath *, bool> m_hasNoChildren;

	unsigned int InsertChildren(const WsFileSpray::IFileSprayPathVector& paths, HTREEITEM hparent, WsFileSpray::IFileSprayPath* expandchild=0);
	WsFileSpray::IFileSprayPath* GetRootPath(WsFileSpray::IFileSprayPath* startpath);

	std::_tstring m_netAddress;
	CComPtr<WsFileSpray::IFileSprayPath> m_currSel;

public:
	CLZoneFolderTree(void);
	~CLZoneFolderTree(void);

	void SetupImageList( bool bSet=true); 
	bool InsertRootNode(const std::_tstring & ip, WsFileSpray::IFileSprayPath * currSel);
	HTREEITEM InsertNode(WsFileSpray::IFileSprayPath* path, HTREEITEM hparent, HTREEITEM hinsertAfter);
	HTREEITEM Locate(WsFileSpray::IFileSprayPath* path, HTREEITEM hitem);
	WsFileSpray::IFileSprayPath * GetCurrSel();

	BEGIN_MSG_MAP(thisClass)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy);
		REFLECTED_NOTIFY_CODE_HANDLER(TVN_GETDISPINFO, OnTreeGetDispInfo);
		REFLECTED_NOTIFY_CODE_HANDLER(TVN_ITEMEXPANDING, OnTreeItemExpanding); 
		REFLECTED_NOTIFY_CODE_HANDLER(TVN_ITEMEXPANDED, OnTreeItemExpanded); 
		REFLECTED_NOTIFY_CODE_HANDLER(TVN_DELETEITEM, OnTreeItemDeleted); 
		REFLECTED_NOTIFY_CODE_HANDLER(TVN_SELCHANGED, OnTreeItemSelChanged); 
	END_MSG_MAP()

	LRESULT CLZoneFolderTree::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnTreeGetDispInfo(WPARAM wParam, LPNMHDR lpnmhdr, BOOL& bHandled);
	LRESULT OnTreeItemExpanding(WPARAM wParam, LPNMHDR lpnmhdr, BOOL& bHandled);
	LRESULT OnTreeItemExpanded(WPARAM wParam, LPNMHDR lpnmhdr, BOOL& bHandled);
	LRESULT OnTreeItemDeleted(WPARAM wParam, LPNMHDR lpnmhdr, BOOL& bHandled);
	LRESULT OnTreeItemSelChanged(WPARAM wParam, LPNMHDR lpnmhdr, BOOL& bHandled);
};
