#pragma once

//  ===========================================================================
#define TVGN_EX_ALL 0x000F
#define TCEX_EDITLABEL 1		// Edit label timer event

#define MSG_WM_KEYDOWN2(func) \
    if (uMsg == WM_KEYDOWN) \
    { \
        SetMsgHandled(TRUE); \
        func((TCHAR)wParam, (UINT)lParam & 0xFFFF, (UINT)((lParam & 0xFFFF0000) >> 16), uMsg, wParam, lParam); \
        lResult = 0; \
        if(IsMsgHandled()) \
            return TRUE; \
    }
//  ===========================================================================
template <class T, class TBase = WTL::CTreeViewCtrlEx, class TWinTraits = ATL::CControlWinTraits>
class CMultiTreeViewCtrlImpl : public CWindowImpl<T, TBase, TWinTraits>
{
    typedef CMultiTreeViewCtrlImpl thisClass;
    typedef CWindowImpl<T, TBase, TWinTraits> baseClass;
protected:
    BOOL		m_bSelectPending;
    CPoint		m_ptClick;
    WTL::CTreeItem	m_hClickedItem;
    WTL::CTreeItem	m_hFirstSelectedItem;
    BOOL		m_bSelectionComplete;
    BOOL		m_bEditLabelPending;
    UINT		m_idTimer;

    enum FOLDER_ACTION
    {
        FOLDER_ACTION_NONE = 0,
        FOLDER_ACTION_OPEN,
        FOLDER_ACTION_CLOSE
    };

public:
    UINT GetSelectedCount() const
    {
        // Only visible items should be selected!
        UINT uCount=0;
        for (WTL::CTreeItem hItem = GetRootItem(); hItem!=NULL; hItem = GetNextVisibleItem(hItem))
            if (GetItemState(hItem, TVIS_SELECTED) & TVIS_SELECTED)
                uCount++;

        return uCount;
    }
    WTL::CTreeItem GetNextItem(WTL::CTreeItem hItem, UINT nCode) const
    {
        if (nCode==TVGN_EX_ALL)
        {
            // This special code lets us iterate through ALL tree items regardless 
            // of their parent/child relationship (very handy)
            WTL::CTreeItem hNextItem;

            // If it has a child node, this will be the next item
            hNextItem = GetChildItem(hItem);
            if (hNextItem)
                return hNextItem;

            // Otherwise, see if it has a next sibling item
            hNextItem = GetNextSiblingItem(hItem);
            if (hNextItem)
                return hNextItem;

            // Finally, look for next sibling to the parent item
            WTL::CTreeItem hParentItem=hItem;
            while (!hNextItem && hParentItem)
            {
                // No more children: Get next sibling to parent
                hParentItem = GetParentItem(hParentItem);
                hNextItem = GetNextSiblingItem(hParentItem);
            }

            return hNextItem; // will return NULL if no more parents
        }
        else
            return baseClass::GetNextItem(hItem, nCode);	// standard processing
    }

    WTL::CTreeItem GetNextTreeItem(WTL::CTreeItem hItem) const
    {
        return GetNextItem(hItem, TVGN_EX_ALL);
    }

    WTL::CTreeItem GetFirstSelectedItem() const
    {
        for (WTL::CTreeItem hItem = GetRootItem(); hItem!=NULL; hItem = GetNextVisibleItem(hItem))
            if (GetItemState(hItem, TVIS_SELECTED) & TVIS_SELECTED)
                return hItem;

        return NULL;
    }

    WTL::CTreeItem GetNextSelectedItem(WTL::CTreeItem hItem) const
    {
        for (hItem = GetNextVisibleItem(hItem); hItem!=NULL; hItem = GetNextVisibleItem(hItem))
            if (GetItemState(hItem, TVIS_SELECTED) & TVIS_SELECTED)
                return hItem;

        return NULL;
    }

    WTL::CTreeItem GetPrevSelectedItem(WTL::CTreeItem hItem) const 
    {
        for (hItem = GetPrevVisibleItem(hItem); hItem!=NULL; hItem = GetPrevVisibleItem(hItem))
            if (GetItemState(hItem, TVIS_SELECTED) & TVIS_SELECTED)
                return hItem;

        return NULL;
    }

    BOOL IsSelected(WTL::CTreeItem hItem) const
    {
        return !!(TVIS_SELECTED & baseClass::GetItemState(hItem, TVIS_SELECTED));
    }

    
    //HTREEITEM ItemFromData(DWORD dwData, HTREEITEM hStartAtItem=NULL) const;

    BOOL SelectItemEx(WTL::CTreeItem hItem, BOOL bSelect=TRUE)
    {
        WTL::CTreeItem hSelItem = GetSelectedItem();

        if (hItem==hSelItem)
        {
            if (!bSelect)
            {
                SelectItem(NULL);
                return TRUE;
            }

            return FALSE;
        }

        SelectItem(hItem);
        m_hFirstSelectedItem=hItem;

        // Reselect previous "real" selected item which was unselected byt SelectItem()
        if (hSelItem)
            SetItemState(hSelItem, TVIS_SELECTED, TVIS_SELECTED);

        return TRUE;
    }

    BOOL SelectItems(WTL::CTreeItem hFromItem, WTL::CTreeItem hToItem)
    {
        // Determine direction of selection 
        // (see what item comes first in the tree)
        WTL::CTreeItem hItem = GetRootItem();

        while (hItem && hItem!=hFromItem && hItem!=hToItem)
            hItem = GetNextVisibleItem(hItem);

        if (!hItem)
            return FALSE;	// Items not visible in tree

        BOOL bReverse = hItem==hToItem;

        // "Really" select the 'to' item (which will deselect 
        // the previously selected item)

        SelectItem(hToItem);

        // Go through all visible items again and select/unselect

        hItem = GetRootItem();
        BOOL bSelect = FALSE;

        while (hItem)
        {
            if (hItem == (bReverse ? hToItem : hFromItem))
                bSelect = TRUE;

            if (bSelect)
            {
                if (!(GetItemState(hItem, TVIS_SELECTED) & TVIS_SELECTED))
                    SetItemState(hItem, TVIS_SELECTED, TVIS_SELECTED);
            }
            else
            {
                if (GetItemState(hItem, TVIS_SELECTED) & TVIS_SELECTED)
                    SetItemState(hItem, 0, TVIS_SELECTED);
            }

            if (hItem == (bReverse ? hFromItem : hToItem))
                bSelect = FALSE;

            hItem = GetNextVisibleItem(hItem);
        }

        return TRUE;
    }
    void ClearSelection(BOOL bMultiOnly=FALSE)
    {
    //	if (!bMultiOnly)
    //		SelectItem(NULL);

        for (WTL::CTreeItem hItem=GetRootItem(); hItem!=NULL; hItem=GetNextVisibleItem(hItem))
            if (GetItemState(hItem, TVIS_SELECTED) & TVIS_SELECTED)
                SetItemState(hItem, 0, TVIS_SELECTED);
    }

    void SelectMultiple(WTL::CTreeItem hClickedItem, UINT nFlags, CPoint point)
    {
        // Start preparing an NM_TREEVIEW struct to send a notification after selection is done
        NM_TREEVIEW tv;
        memset(&tv.itemOld, 0, sizeof(tv.itemOld));

        CWindow pWnd = GetParent();

        WTL::CTreeItem hOldItem = GetSelectedItem();

        if (hOldItem)
        {
            tv.itemOld.hItem = hOldItem;
            tv.itemOld.state = GetItemState(hOldItem, 0xffffffff);
            tv.itemOld.lParam = GetItemData(hOldItem);
            tv.itemOld.mask = TVIF_HANDLE|TVIF_STATE|TVIF_PARAM;
        }

        // Flag signaling that selection process is NOT complete.
        // (Will prohibit TVN_SELCHANGED from being sent to parent)
        m_bSelectionComplete = FALSE;

        // Action depends on whether the user holds down the Shift or Ctrl key
        if (nFlags & MK_SHIFT)
        {
            // Select from first selected item to the clicked item
            if (!m_hFirstSelectedItem)
                m_hFirstSelectedItem = GetSelectedItem();

            SelectItems(m_hFirstSelectedItem, hClickedItem);
        }
        else if (nFlags & MK_CONTROL)
        {
            // Find which items are currently selected
            typedef std::vector<WTL::CTreeItem> TreeItemVector;

            TreeItemVector hSelectedItems;
            WTL::CTreeItem hSelectedItem = GetFirstSelectedItem();
            while (hSelectedItem)
            {
                if (hSelectedItem != hClickedItem)
                    hSelectedItems.push_back(hSelectedItem);
                hSelectedItem = GetNextSelectedItem(hSelectedItem);
            }

            // Is the clicked item already selected ?
            BOOL bIsClickedItemSelected = GetItemState(hClickedItem, TVIS_SELECTED) & TVIS_SELECTED;

            // Must synthesize a TVN_SELCHANGING notification
            if (pWnd)
            {
                tv.hdr.hwndFrom = pWnd.m_hWnd;
                tv.hdr.idFrom = GetWindowLong(GWL_ID);
                tv.hdr.code = TVN_SELCHANGING;

                tv.itemNew.hItem = hClickedItem;
                tv.itemNew.state = GetItemState(hClickedItem, 0xffffffff);
                tv.itemNew.lParam = GetItemData(hClickedItem);

                tv.itemOld.hItem = NULL;
                tv.itemOld.mask = 0;

                tv.action = TVC_BYMOUSE;

                tv.ptDrag.x = point.x;
                tv.ptDrag.y = point.y;

                pWnd.SendMessage(WM_NOTIFY, tv.hdr.idFrom, (LPARAM)&tv);
            }

            // We want the newly selected item to toggle its selected state,
            // so unselect now if it was already selected before
            if (bIsClickedItemSelected)
                SetItemState(hClickedItem, 0, TVIS_SELECTED);
            else
            {
                SelectItem(hClickedItem);
                SetItemState(hClickedItem, TVIS_SELECTED, TVIS_SELECTED);
            }

            // If the previously selected item was selected, re-select it
            for(TreeItemVector::const_iterator itr =  hSelectedItems.begin(); itr != hSelectedItems.end(); ++itr)
            {
                SetItemState(itr->m_hTreeItem, TVIS_SELECTED, TVIS_SELECTED);

            }

            // Store as first selected item (if not already stored)
            if (m_hFirstSelectedItem==NULL)
                m_hFirstSelectedItem = hClickedItem;
        }
        else
        {
            // Clear selection of all "multiple selected" items first
            ClearSelection();

            // Then select the clicked item
            SelectItem(hClickedItem);
            SetItemState(hClickedItem, TVIS_SELECTED, TVIS_SELECTED);

            // Store as first selected item
            m_hFirstSelectedItem = hClickedItem;
        }

        // Selection process is now complete. Since we have 'eaten' the TVN_SELCHANGED 
        // notification provided by Windows' treectrl, we must now produce one ourselves,
        // so that our parent gets to know about the change of selection.
        m_bSelectionComplete = TRUE;

        if (pWnd)
        {
            tv.hdr.hwndFrom = m_hWnd;
            tv.hdr.idFrom = GetWindowLong(GWL_ID);
            tv.hdr.code = TVN_SELCHANGED;

            tv.itemNew.hItem = m_hClickedItem;
            tv.itemNew.state = GetItemState(m_hClickedItem, 0xffffffff);
            tv.itemNew.lParam = GetItemData(m_hClickedItem);
            tv.itemNew.mask = TVIF_HANDLE|TVIF_STATE|TVIF_PARAM;

            tv.action = TVC_UNKNOWN;

            pWnd.SendMessage(WM_NOTIFY, tv.hdr.idFrom, (LPARAM)&tv);
        }
    }


    BOOL PreTranslateMessage(MSG* pMsg)
    {
        pMsg;
        return FALSE;
    }

    DECLARE_WND_SUPERCLASS(NULL, baseClass::GetWndClassName())

    BEGIN_MSG_MAP(thisClass)
        MSG_WM_LBUTTONDOWN(OnLButtonDown)
        MSG_WM_LBUTTONUP(OnLButtonUp)
        MSG_WM_MOUSEMOVE(OnMouseMove)
        MSG_WM_KEYDOWN2(OnKeyDown)
        REFLECTED_NOTIFY_CODE_HANDLER(TVN_ITEMEXPANDING, OnItemexpanding)
        REFLECTED_NOTIFY_CODE_HANDLER(NM_SETFOCUS, OnSetfocus)
        REFLECTED_NOTIFY_CODE_HANDLER(NM_KILLFOCUS, OnKillfocus)
        REFLECTED_NOTIFY_CODE_HANDLER(TVN_SELCHANGED, OnSelchanged)
        MSG_WM_RBUTTONDOWN(OnRButtonDown)
        MSG_WM_LBUTTONDBLCLK(OnLButtonDblClk)
        MSG_WM_TIMER(OnTimer)	
        DEFAULT_REFLECTION_HANDLER()
    END_MSG_MAP()

    void OnLButtonDown(UINT nFlags, CPoint point)
    {
        UINT nHitFlags = 0;
        WTL::CTreeItem hClickedItem = HitTest(point, &nHitFlags);

        // Must invoke label editing explicitly. The base class OnLButtonDown would normally
        // do this, but we can't call it here because of the multiple selection...
        if(!(nFlags&(MK_CONTROL|MK_SHIFT)) && (GetStyle() & TVS_EDITLABELS) && (nHitFlags & TVHT_ONITEMLABEL))
            if (hClickedItem == GetSelectedItem())
            {
                // Clear multple selection before label editing
                ClearSelection();
                SelectItem(hClickedItem);

                // Invoke label editing
                m_bEditLabelPending = TRUE;
                m_idTimer = SetTimer(TCEX_EDITLABEL, GetDoubleClickTime(), NULL);

                return;
            }

        m_bEditLabelPending = FALSE;

        if(nHitFlags & (TVHT_ONITEM | (GetStyle() & TVS_FULLROWSELECT ? TVHT_ONITEMRIGHT : 0)))
        {
            SetFocus();

            m_hClickedItem = hClickedItem;

            // Is the clicked item already selected ?
            BOOL bIsClickedItemSelected = GetItemState(hClickedItem, TVIS_SELECTED) & TVIS_SELECTED;

            if (bIsClickedItemSelected)
            {
                // Maybe user wants to drag/drop multiple items!
                // So, wait until OnLButtonUp() to do the selection stuff. 
                m_bSelectPending=TRUE;
            }
            else
            {
                SelectMultiple(hClickedItem, nFlags, point);
                m_bSelectPending=FALSE;
            }

            m_ptClick=point;
        }
        else
            SetMsgHandled(false);
    }

    void OnLButtonUp(UINT nFlags, CPoint point)
    {
        if (m_bSelectPending)
        {
            // A select has been waiting to be performed here
            SelectMultiple(m_hClickedItem, nFlags, point);
            m_bSelectPending=FALSE;
        }

        m_hClickedItem=NULL;
        SetMsgHandled(false);
    }

    void OnMouseMove(UINT nFlags, CPoint point)
    {
        // If there is a select pending, check if cursor has moved so much away from the 
        // down-click point that we should cancel the pending select and initiate
        // a drag/drop operation instead!

        if (m_hClickedItem)
        {
            CSize sizeMoved = m_ptClick-point;

            if (abs(sizeMoved.cx) > GetSystemMetrics(SM_CXDRAG) || abs(sizeMoved.cy) > GetSystemMetrics(SM_CYDRAG))
            {
                m_bSelectPending=FALSE;

                // Notify parent that he may begin drag operation
                // Since we have taken over OnLButtonDown(), the default handler doesn't
                // do the normal work when clicking an item, so we must provide our own
                // TVN_BEGINDRAG notification for the parent!

                CWindow pWnd = GetParent();
                if (pWnd && !(GetStyle() & TVS_DISABLEDRAGDROP))
                {
                    NM_TREEVIEW tv;

                    tv.hdr.hwndFrom = m_hWnd;
                    tv.hdr.idFrom = GetWindowLong(GWL_ID);
                    tv.hdr.code = TVN_BEGINDRAG;

                    tv.itemNew.hItem = m_hClickedItem;
                    tv.itemNew.state = GetItemState(m_hClickedItem, 0xffffffff);
                    tv.itemNew.lParam = GetItemData(m_hClickedItem);

                    tv.ptDrag.x = point.x;
                    tv.ptDrag.y = point.y;

                    pWnd.SendMessage(WM_NOTIFY, tv.hdr.idFrom, (LPARAM)&tv);
                }

                m_hClickedItem=NULL;
            }
        }
        SetMsgHandled(false);
    }

    void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        CWindow pWnd = GetParent();

        if (nChar==VK_NEXT || nChar==VK_PRIOR)
        {
            if (!(GetKeyState(VK_SHIFT)&0x8000))
            {
                // User pressed Pg key without holding 'Shift':
                // Clear multiple selection (if multiple) and let base class do 
                // normal selection work!
                if (GetSelectedCount()>1)
                    ClearSelection(TRUE);

                baseClass::DefWindowProc(uMsg, wParam, lParam);
                m_hFirstSelectedItem = GetSelectedItem();
                return;
            }

            // Flag signaling that selection process is NOT complete.
            // (Will prohibit TVN_SELCHANGED from being sent to parent)
            m_bSelectionComplete = FALSE;

            // Let base class select the item
            baseClass::DefWindowProc(uMsg, wParam, lParam);
            WTL::CTreeItem hSelectedItem = GetSelectedItem();

            // Then select items in between
            SelectItems(m_hFirstSelectedItem, hSelectedItem);

            // Selection process is now complete. Since we have 'eaten' the TVN_SELCHANGED 
            // notification provided by Windows' treectrl, we must now produce one ourselves,
            // so that our parent gets to know about the change of selection.
            m_bSelectionComplete = TRUE;

            if (pWnd)
            {
                NM_TREEVIEW tv;
                memset(&tv.itemOld, 0, sizeof(tv.itemOld));

                tv.hdr.hwndFrom = m_hWnd;
                tv.hdr.idFrom = GetWindowLong(GWL_ID);
                tv.hdr.code = TVN_SELCHANGED;

                tv.itemNew.hItem = hSelectedItem;
                tv.itemNew.state = GetItemState(hSelectedItem, 0xffffffff);
                tv.itemNew.lParam = GetItemData(hSelectedItem);
                tv.itemNew.mask = TVIF_HANDLE|TVIF_STATE|TVIF_PARAM;

                tv.action = TVC_UNKNOWN;

                pWnd.SendMessage(WM_NOTIFY, tv.hdr.idFrom, (LPARAM)&tv);
            }
        }
        else if (nChar==VK_UP || nChar==VK_DOWN)
        {
            // Find which item is currently selected
            WTL::CTreeItem hSelectedItem = GetSelectedItem();

            WTL::CTreeItem hNextItem;
            if (nChar==VK_UP)
                hNextItem = GetPrevVisibleItem(hSelectedItem);
            else
                hNextItem = GetNextVisibleItem(hSelectedItem);

            if (!(GetKeyState(VK_SHIFT)&0x8000))
            {
                // User pressed arrow key without holding 'Shift':
                // Clear multiple selection (if multiple) and let base class do 
                // normal selection work!
                if (GetSelectedCount()>1)
                    ClearSelection(TRUE);

                if (hNextItem)
                {
                    baseClass::DefWindowProc(uMsg, wParam, lParam);
                }
                m_hFirstSelectedItem = GetSelectedItem();
                return;
            }

            if (hNextItem)
            {
                // Flag signaling that selection process is NOT complete.
                // (Will prohibit TVN_SELCHANGED from being sent to parent)
                m_bSelectionComplete = FALSE;

                // If the next item is already selected, we assume user is
                // "moving back" in the selection, and thus we should clear 
                // selection on the previous one
                BOOL bSelect = !(GetItemState(hNextItem, TVIS_SELECTED) & TVIS_SELECTED);

                // Select the next item (this will also deselect the previous one!)
                SelectItem(hNextItem);

                // Now, re-select the previously selected item
                if (bSelect || (!(GetItemState(hSelectedItem, TVIS_SELECTED) & TVIS_SELECTED)))
                    SelectItems(m_hFirstSelectedItem, hNextItem);

                // Selection process is now complete. Since we have 'eaten' the TVN_SELCHANGED 
                // notification provided by Windows' treectrl, we must now produce one ourselves,
                // so that our parent gets to know about the change of selection.
                m_bSelectionComplete = TRUE;

                if (pWnd)
                {
                    NM_TREEVIEW tv;
                    memset(&tv.itemOld, 0, sizeof(tv.itemOld));

                    tv.hdr.hwndFrom = m_hWnd;
                    tv.hdr.idFrom = GetWindowLong(GWL_ID);
                    tv.hdr.code = TVN_SELCHANGED;

                    tv.itemNew.hItem = hNextItem;
                    tv.itemNew.state = GetItemState(hNextItem, 0xffffffff);
                    tv.itemNew.lParam = GetItemData(hNextItem);
                    tv.itemNew.mask = TVIF_HANDLE|TVIF_STATE|TVIF_PARAM;

                    tv.action = TVC_UNKNOWN;

                    pWnd.SendMessage(WM_NOTIFY, tv.hdr.idFrom, (LPARAM)&tv);
                }
            }

            // Since the base class' OnKeyDown() isn't called in this case,
            // we must provide our own TVN_KEYDOWN notification to the parent

            CWindow pWnd = GetParent();
            if (pWnd)
            {
                NMTVKEYDOWN tvk;

                tvk.hdr.hwndFrom = m_hWnd;
                tvk.hdr.idFrom = GetWindowLong(GWL_ID);
                tvk.hdr.code = TVN_KEYDOWN;

                tvk.wVKey = nChar;
                tvk.flags = 0;

                pWnd.SendMessage(WM_NOTIFY, tvk.hdr.idFrom, (LPARAM)&tvk);
            }
        }
        else
        {
            // Behave normally
            SetMsgHandled(false);
            //CTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
        }
    }
    void ChangeFolder(NM_TREEVIEW* pNMTreeView, FOLDER_ACTION action)
    {
        int img, imgSel;
        GetItemImage(pNMTreeView->itemNew.hItem, img, imgSel);

        int newImg = -1;
        int newImgSel = -1;

        if (img == IID_FOLDER_CLOSED || imgSel == IID_FOLDER_OPEN)
        {
            newImg = IID_FOLDER_CLOSED;
            newImgSel = IID_FOLDER_OPEN;
        }
        else if (img == IID_PLUGINFOLDER_CLOSED || imgSel == IID_PLUGINFOLDER_OPEN)
        {
            newImg = IID_PLUGINFOLDER_CLOSED;
            newImgSel = IID_PLUGINFOLDER_OPEN;
        }
        else if (img == IID_TOPFOLDER_CLOSED || imgSel == IID_TOPFOLDER_OPEN)
        {
            newImg = IID_TOPFOLDER_CLOSED;
            newImgSel = IID_TOPFOLDER_OPEN;
        }
        else if (img == IID_PLUGINFOLDER_UNMATCHED || imgSel == IID_PLUGINFOLDER_OPEN_UNMATCHED)
        {
            newImg = IID_PLUGINFOLDER_UNMATCHED;
            newImgSel = IID_PLUGINFOLDER_OPEN_UNMATCHED;
        }
        if (newImg >= 0 && action != FOLDER_ACTION_NONE)
        {
            if (action == FOLDER_ACTION_OPEN)
                SetItemImage(pNMTreeView->itemNew.hItem, newImgSel, newImgSel);
            else if (action == FOLDER_ACTION_CLOSE)
                SetItemImage(pNMTreeView->itemNew.hItem, newImg, newImg);
        }
    }
    LRESULT OnItemexpanding(WPARAM wParam, LPNMHDR pNMHDR, BOOL& bHandled)
    {
        NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

        if (pNMTreeView->action == TVE_COLLAPSE)
        {
            WTL::CTreeItem hItem = GetChildItem(pNMTreeView->itemNew.hItem);
            ChangeFolder(pNMTreeView, FOLDER_ACTION_CLOSE);

            while (hItem)
            {
                if (GetItemState(hItem, TVIS_SELECTED) & TVIS_SELECTED)
                    SetItemState(hItem, 0, TVIS_SELECTED);

                // Get the next node: First see if current node has a child
                WTL::CTreeItem hNextItem = GetChildItem(hItem);
                if (!hNextItem)
                {
                    // No child: Get next sibling item
                    if (!(hNextItem = GetNextSiblingItem(hItem)))
                    {
                        WTL::CTreeItem hParentItem = hItem;
                        while (!hNextItem)
                        {
                            // No more children: Get parent
                            if (!(hParentItem = GetParentItem(hParentItem)))
                                break;

                            // Quit when parent is the collapsed node
                            // (Don't do anything to siblings of this)
                            if (hParentItem == pNMTreeView->itemNew.hItem)
                                break;

                            // Get next sibling to parent
                            hNextItem = GetNextSiblingItem(hParentItem);
                        }

                        // Quit when parent is the collapsed node
                        if (hParentItem == pNMTreeView->itemNew.hItem)
                            break;
                    }
                }

                hItem = hNextItem;
            }
        }
        else if (pNMTreeView->action == TVE_EXPAND)
        {
            ChangeFolder(pNMTreeView, FOLDER_ACTION_OPEN);
        }
        bHandled = FALSE;
        return 0;
    }
    LRESULT OnSetfocus(WPARAM wParam, LPNMHDR lpnmhdr, BOOL& bHandled)
    {
        Invalidate();
        bHandled = FALSE;
        return 0;
    }
    LRESULT OnKillfocus(WPARAM wParam, LPNMHDR lpnmhdr, BOOL& bHandled)
    {
        Invalidate();
        bHandled = FALSE;
        return 0;
    }
    LRESULT OnSelchanged(WPARAM wParam, LPNMHDR lpnmhdr, BOOL& bHandled)
    {
        // Return TRUE if selection is not complete. This will prevent the 
        // notification from being sent to parent.
        return !m_bSelectionComplete;
    }

    void OnRButtonDown(UINT nFlags, CPoint point)
    {
        UINT nHitFlags = 0;
        WTL::CTreeItem hClickedItem = HitTest(point, &nHitFlags);

        if (nHitFlags & (TVHT_ONITEM | (GetStyle() & TVS_FULLROWSELECT ? TVHT_ONITEMRIGHT : 0)))
            if (GetSelectedCount() < 2)
                SelectItem(hClickedItem);

        SetMsgHandled(false);
    }

    void OnLButtonDblClk(UINT nFlags, CPoint point)
    {
        // We stop label editing.
        m_bEditLabelPending = FALSE;
        SetMsgHandled(false);
    }

    void OnTimer(UINT_PTR nIDEvent)
    {
        if (nIDEvent == TCEX_EDITLABEL)
        {
            // Stop the timer.
            KillTimer(m_idTimer);

            // Invoke label editing.
            if (m_bEditLabelPending)
                EditLabel(GetSelectedItem());

            m_bEditLabelPending = FALSE;
            return;
        }

        SetMsgHandled(false);
    }
};

typedef CMultiTreeViewCtrlImpl<ATL::CWindow> CMultiTreeViewCtrl;
//  ===========================================================================
