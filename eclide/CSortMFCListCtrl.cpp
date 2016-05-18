#include "CSortMFCListCtrl.h"

BEGIN_MESSAGE_MAP(CSortMFCListCtrl, CMFCListCtrl)
    ON_NOTIFY(HDN_ITEMCLICK, 0, OnItemclickList)
END_MESSAGE_MAP()

CSortMFCListCtrl::CSortMFCListCtrl()
{
    m_sortedCol = -1;
    m_sortAscending = TRUE;
}

BM_TYPE CSortMFCListCtrl::StringToType(std::_tstring bmStr)
{
    BM_TYPE bmType = BM_NONE;

    for (int b = 0; b < m_bookmarks.size(); b++)
    {
        if (m_bookmarks[b] == bmStr)
        {
            bmType = (BM_TYPE)b;
            break;
        }
    }

    return bmType;
}

void CSortMFCListCtrl::OnDeleteItem(int row)
{
    int count = GetItemCount();
    if (count > 0 && row < count)
    {
        BookmarkItemData *data = reinterpret_cast<BookmarkItemData *>(GetItemData(row));
        if (data)
        {
            delete data;
            SetItemData(row, NULL);
        }
    }
}

void CSortMFCListCtrl::SetMark(int row, bool val)
{
    if (GetItemCount() > 0)
    {
        BookmarkItemData *data = reinterpret_cast<BookmarkItemData *>(GetItemData(row));
        if (data)
        {
            data->marked = val;
        }
    }
}

int CALLBACK SortMe(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
    CSortMFCListCtrl* pListCtrl = (CSortMFCListCtrl*)lParamSort;

    int ret = 0;
    int col = pListCtrl->m_sortedCol;

    CString strItem1 = pListCtrl->GetItemText(static_cast<int>(lParam1), col);
    CString strItem2 = pListCtrl->GetItemText(static_cast<int>(lParam2), col);

    if (col == 4 && strItem1 == strItem2) {
        strItem1 = pListCtrl->GetItemText(static_cast<int>(lParam1), 0);
        strItem2 = pListCtrl->GetItemText(static_cast<int>(lParam2), 0);
        col = 0;
    }

    if (!pListCtrl->m_sortAscending) {
        CString switchit = strItem1;
        strItem1 = strItem2;
        strItem2 = switchit;
    }

    if (col == 0) {
        int i1 = _ttoi(strItem1);
        int i2 = _ttoi(strItem2);
        if (i1 > i2)
            ret = -1;
        else if (i1 < i2)
            ret = 1;
    }
    else
        ret = lstrcmp(strItem2, strItem1);

    return ret;
}

void CSortMFCListCtrl::DoSort(int col, bool ascending)
{
    m_sortedCol = col;
    m_sortAscending = ascending;

    CMFCHeaderCtrl &headerCntrl = GetHeaderCtrl();
    headerCntrl.SetSortColumn(col, !ascending);

    SortItemsEx(SortMe, (LPARAM) this);
}

void CSortMFCListCtrl::OnItemclickList(NMHDR* pNMHDR, LRESULT* pResult)
{
    NMLISTVIEW *pLV = (NMLISTVIEW *)pNMHDR;
    HD_NOTIFY *phdn = (HD_NOTIFY *)pNMHDR;

    if (phdn->iButton == 0)
    {
        m_sortAscending = !m_sortAscending;
        DoSort(pLV->iItem, m_sortAscending);
    }

    *pResult = 0;
}
