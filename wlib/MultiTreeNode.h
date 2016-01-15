#pragma once

#include "wlib.h"
#include "MultiTree.h"
#include "TreeNode.h"
#include "MouseTooltip.h"

class CPersistTreeState;
//  ===========================================================================
template <class T, class TBase = WTL::CTreeViewCtrlEx, class TWinTraits = WTL::CControlWinTraits>
class CMultiTreeNodeViewCtrlImpl : public CMultiTreeViewCtrlImpl<T, TBase, TWinTraits>, public CMouseTooltip<CMultiTreeNodeViewCtrlImpl<T, TBase, TWinTraits> >
{
    typedef CMultiTreeNodeViewCtrlImpl thisClass;
    typedef CMultiTreeViewCtrlImpl<T, TBase, TWinTraits> baseClass;
    typedef CMouseTooltip<CMultiTreeNodeViewCtrlImpl<T, TBase, TWinTraits> > baseClass2;

protected:
    WTL::CImageList m_ImgList;
    WTL::CImageList m_StateList;
    boost::ptr_deque<CPersistTreeState> m_state;

public:
    thisClass& operator=(HWND hWnd)
    {
        if ( m_hWnd )
            DestroyWindow();
        if ( ::IsWindow(hWnd) )
            SubclassWindow(hWnd);

        //		this->GetToolTips().SetDelayTime(TTDT_AUTOMATIC, -1);
        //		this->GetToolTips().AddTool(m_hWnd);

        return *this;
    }

    void SetImageLists(const WTL::CImageList &imgList, const WTL::CImageList &stateList)
    {
        m_ImgList = imgList;
        m_StateList = stateList;

        SetImageList(m_ImgList, TVSIL_NORMAL);
        SetImageList(m_StateList, TVSIL_STATE);
    }

    void PushState()
    {
        m_state.push_front(new CPersistTreeState(*this));
    }

    void PopState()
    {
        m_state.pop_front();
    }

    unsigned GetSelected(CTreeNodeVector &items)
    {
        for(WTL::CTreeItem pos = GetFirstSelectedItem(); pos != (WTL::CTreeItem)0; pos = GetNextSelectedItem(pos))
        {
            WTL::CTreeItem node(pos, this);
            CTreeNode *item = (CTreeNode*)node.GetData();
            ATLASSERT(item);
            items.push_back(item);
        }
        return items.size();
    }

    template<typename T>
    T * GetFirstSelected()
    {
        for(WTL::CTreeItem pos = GetFirstSelectedItem(); pos != (WTL::CTreeItem)0; pos = GetNextSelectedItem(pos))
        {
            WTL::CTreeItem node(pos, this);
            CTreeNode *item = (CTreeNode*)node.GetData();
            CComQIPtr<T> retVal = item;
            if (retVal)
                return retVal;
        }
        return NULL;
    }

    void ShowToolTip(const CPoint & point)
    {
        UINT nHF = 0;
        WTL::CTreeItem hItem = NULL;

        hItem = HitTest(point, &nHF);

        TVITEM item;
        item.mask = TVIF_HANDLE;
        item.hItem = hItem;
        if (GetItem(&item))
        {
            CComPtr<CTreeNode> tn = (CTreeNode *)item.lParam;
            if (tn)
            {
                std::_tstring tip;
                baseClass2::ShowToolTip(tn->GetToolTip(tip));
            }
        }
    } 

    BEGIN_MSG_MAP(thisClass)
        REFLECTED_NOTIFY_CODE_HANDLER(TVN_GETDISPINFO, OnGetDispInfo)
        REFLECTED_NOTIFY_CODE_HANDLER(TVN_ITEMEXPANDING, OnItemExpanding)
        REFLECTED_NOTIFY_CODE_HANDLER(TVN_DELETEITEM, OnDeleteItem)
        REFLECTED_NOTIFY_CODE_HANDLER(TVN_BEGINLABELEDIT, OnBeginLabelEdit)
        REFLECTED_NOTIFY_CODE_HANDLER(TVN_ENDLABELEDIT, OnEndLabelEdit)
        REFLECTED_NOTIFY_CODE_HANDLER(NM_DBLCLK, OnDblClk)
        CHAIN_MSG_MAP(baseClass2)
        CHAIN_MSG_MAP(baseClass)	//  Handles relected notification defaults
    END_MSG_MAP()

    LRESULT OnGetDispInfo(WPARAM wParam, LPNMHDR pNMHDR, BOOL& bHandled)
    {
        LPNMTVDISPINFO pTVDispInfo = (LPNMTVDISPINFO)pNMHDR;
        if (pTVDispInfo->item.lParam)
        {
            CComPtr<CTreeNode> tn = (CTreeNode *)pTVDispInfo->item.lParam;
            tn->GetDispInfo(pTVDispInfo->item);
        }
        return 0;
    }
    LRESULT OnItemExpanding(WPARAM wParam, LPNMHDR pNMHDR, BOOL& bHandled)
    {
        CWaitCursor w;
        LPNMTREEVIEW pNMTreeView = (LPNMTREEVIEW)pNMHDR;
        ATLASSERT(pNMTreeView->itemNew.lParam);
        CComPtr<CTreeNode> tn = (CTreeNode *)pNMTreeView->itemNew.lParam;
        if (pNMTreeView->action == TVE_EXPAND)
        {
            if (!(pNMTreeView->itemNew.state & TVIS_EXPANDEDONCE))
                tn->ItemExpanding();
        }
        else
        {
            CLoadingNode * loadingNode = new CLoadingNode();
            loadingNode->Insert(*this, pNMTreeView->itemNew.hItem, TVI_FIRST);
            Expand(pNMTreeView->itemNew.hItem, TVE_COLLAPSE | TVE_COLLAPSERESET);
        }
        bHandled = FALSE;
        return 0;
    }
    LRESULT OnDeleteItem(WPARAM wParam, LPNMHDR pNMHDR, BOOL& bHandled)
    {
        LPNMTREEVIEW pNMTreeView = (LPNMTREEVIEW)pNMHDR;
        ATLASSERT(pNMTreeView->itemOld.lParam);
        CTreeNode * tn = (CTreeNode *)pNMTreeView->itemOld.lParam;
        tn->SetData(NULL);
        tn->Release();
        return 0;
    }
    LRESULT OnBeginLabelEdit(WPARAM wParam, LPNMHDR pNMHDR, BOOL& bHandled)
    {
        LPNMTVDISPINFO dispInfo = (LPNMTVDISPINFO)pNMHDR;
        ATLASSERT(dispInfo->item.lParam);
        CComPtr<CTreeNode> tn = (CTreeNode *)dispInfo->item.lParam;
        if (tn->CanEdit())
        {
            bHandled = FALSE;
            return 0;
        }
        else
        {
            return 1;
        }
    }
    LRESULT OnEndLabelEdit(WPARAM wParam, LPNMHDR pNMHDR, BOOL& bHandled)
    {
        LPNMTVDISPINFO dispInfo = (LPNMTVDISPINFO)pNMHDR;
        ATLASSERT(dispInfo->item.lParam);
        if(dispInfo->item.pszText != NULL)
        {
            CComPtr<CTreeNode> tn = (CTreeNode *)dispInfo->item.lParam;
            return tn->Rename(dispInfo->item.pszText);
        }
        return 0;
    }
    LRESULT OnDblClk(WPARAM wParam, LPNMHDR pNMHDR, BOOL& bHandled)
    {
        TVITEM item;
        item.mask = TVIF_HANDLE;
        item.hItem = GetSelectedItem();
        if (GetItem(&item))
        {
            CComPtr<CTreeNode> tn = (CTreeNode *)item.lParam;
            tn->OnDblclk(item.hItem);
        }
        return 0;
    }
};

typedef CMultiTreeNodeViewCtrlImpl<ATL::CWindow> CMultiTreeNodeViewCtrl;
//  ===========================================================================
class WLIB_API __declspec(uuid("8175886e-3d91-4c35-b365-15fbc70b6f19")) CLoadingNode : public CTreeNode
{
protected:
    std::_tstring m_id;
    std::_tstring m_label;
    CMultiTreeNodeViewCtrl * m_tree;

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(CLoadingNode)
    END_CUNKNOWN(CTreeNode)

    CLoadingNode(CMultiTreeNodeViewCtrl * tree = NULL, const std::_tstring & label = _T("...Loading..."));
    ~CLoadingNode();

    virtual const TCHAR * GetID();
    virtual const TCHAR * GetDispText();
    int GetDispImage();
    int GetDispSelectedImage();
    int GetDispChildren();
};
//  ===========================================================================
class WLIB_API CPersistTreeState
{
protected:
    CMultiTreeNodeViewCtrl &m_tree;
    std::set<std::_tstring> m_selected_set;
    std::set<std::_tstring> m_expanded_set;
    std::_tstring m_focus;

public:
    CPersistTreeState(CMultiTreeNodeViewCtrl &tree);
    ~CPersistTreeState();
};
//  ===========================================================================
