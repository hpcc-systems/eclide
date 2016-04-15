#include "stdafx.h"
#include <afxlistctrl.h>

enum BM_TYPE
{
    BM_NONE = 0,
    BM_MINE = 1,
    BM_TODO = 2,
    BM_HACK = 3
};

struct BookmarkItemData
{
    bool marked;
    BM_TYPE bookmarkType;
    int column;
};

class CSortMFCListCtrl :public CMFCListCtrl
{
public:

    std::vector<std::_tstring> m_bookmarks = { _T(""), _T("[BM]"), _T("TODO:"), _T("HACK:") };

    int m_sortedCol;
    bool m_sortAscending;

    CSortMFCListCtrl();

    void DoSort(int col, bool ascending);
    void OnDeleteItem(int row);
    BM_TYPE StringToType(std::_tstring bmStr);

    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnItemclickList(NMHDR* pNMHDR, LRESULT* pResult);

};