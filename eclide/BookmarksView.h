#pragma once

#include "Util.h"
#include "DebugGraphView.h"
#include "Attribute.h"
#include "SciSyntaxMarker.h"
#include "CSortMFCListCtrl.h"
#include <CustomMessages.h>

const TCHAR * const CLEAR_BOOKMARKS_MSG = _T("Are you sure you want clear the displayed bookmarks?");
const TCHAR * const SAVE_BOOKMARKS_MSG = _T("Are you sure you want overwrite the current bookmarks file?");
const TCHAR * const LOAD_BOOKMARKS_MSG = _T("Are you sure you want replace the current list of bookmarks with what's saved?");
const TCHAR * const LOAD_MERGE_BOOKMARKS_MSG = _T("Are you sure you want to merge your saved bookmarks with the current bookmark list?");
const TCHAR * const LOAD_UNFOUND_BOOKMARKS_MSG = _T("No bookmarks file found");
const TCHAR * const DELETE_BOOKMARKS_MSG = _T("Do you want to delete the selected bookmark(s) from the list?");

enum BM_SERVER_TYPE
{
    BM_SERVER_TYPE_UNKNOWN = 0,
    BM_SERVER_TYPE_LOCAL,
    BM_SERVER_TYPE_REMOTE,
};

__interface ISciBookmarksMarker;

__interface IBookmarksSlot
{
    bool OnDoubleClick();
    bool OnContext(CMenu &menu, CPoint &pt);
};

class CBookmarksView:
    public WTL::CDialogImpl<CBookmarksView>,
    public WTL::CDialogResize<CBookmarksView>,
    public WTL::CWinDataExchange<CBookmarksView>
{
    typedef CBookmarksView thisClass;
    typedef WTL::CDialogImpl<thisClass> baseClass;

protected:
    IBookmarksSlot *m_Owner;
    CSortMFCListCtrl m_list;
    CSortMFCListCtrl m_listMaster;
    bool m_checkMine;
    bool m_checkTodos;
    bool m_checkHacks;
    BM_SERVER_TYPE m_serverType;

    HWND m_hWndOrigin;
    ISciBookmarksMarker *m_prevBookmarksMarker;

public:
    CBookmarksView();
    virtual ~CBookmarksView();
    CSortMFCListCtrl *GetListCtrl();

    void Init(IBookmarksSlot *pMenuManager);

    enum { IDD = IDD_BOOKMARKS };

    BEGIN_MSG_MAP(thisClass)
        MSG_WM_INITDIALOG(OnInitDialog)
        MSG_WM_DESTROY(OnDestroy)
        MSG_WM_CONTEXTMENU(OnContextMenu)

        NOTIFY_HANDLER(IDC_BOOKMARKS_PLACEHOLDER, LVN_ITEMCHANGED, OnLvnItemchangedListHistory)
        NOTIFY_HANDLER(IDC_BOOKMARKS_PLACEHOLDER, LVN_DELETEITEM, OnLvnItemDelete)
        NOTIFY_HANDLER(IDC_BOOKMARKS_PLACEHOLDER, NM_DBLCLK, OnNmDblClk)

        COMMAND_HANDLER(IDC_CHECK_MINE, BN_CLICKED, OnBnClickedCheckFilter)
        COMMAND_HANDLER(IDC_CHECK_TODOS, BN_CLICKED, OnBnClickedCheckFilter)
        COMMAND_HANDLER(IDC_CHECK_HACKS, BN_CLICKED, OnBnClickedCheckFilter)

        MESSAGE_HANDLER_EX(CWM_CLEAR, OnClear)
        MESSAGE_HANDLER_EX(CWM_RESET, OnReset)
        MESSAGE_HANDLER_EX(CWM_REFRESH, OnRefresh)

        CHAIN_MSG_MAP(WTL::CDialogResize<thisClass>)
        REFLECT_NOTIFICATIONS()
    END_MSG_MAP()

    BEGIN_DLGRESIZE_MAP(thisClass)
        DLGRESIZE_CONTROL(IDC_BOOKMARKS_PLACEHOLDER, DLSZ_SIZE_X | DLSZ_SIZE_Y)
        DLGRESIZE_CONTROL(IDC_CHECK_MINE, DLSZ_MOVE_Y)
        DLGRESIZE_CONTROL(IDC_CHECK_TODOS, DLSZ_MOVE_Y)
        DLGRESIZE_CONTROL(IDC_CHECK_HACKS, DLSZ_MOVE_Y)
    END_DLGRESIZE_MAP()

    BEGIN_DDX_MAP(thisClass)
        DDX_CHECK(IDC_CHECK_MINE, m_checkMine)
        DDX_CHECK(IDC_CHECK_TODOS, m_checkTodos)
        DDX_CHECK(IDC_CHECK_HACKS, m_checkHacks)
    END_DDX_MAP()

    std::_tstring FindTag(std::_tstring line, std::_tstring tag, int &index, bool zeroIndex = false);
    void SetMarks(bool val);
    void DeleteMarkedBookmarks(bool val);
    void SetMarks(std::_tstring inModule, std::_tstring inAttributeName, bool val);
    void DeleteMarkedBookmarks(std::_tstring inModule, std::_tstring inAttributeName, bool val);
    void ParseBookmarks(IAttribute *attribute);
    void ParseBookmarksEcl(std::_tstring ecl, std::_tstring user, std::_tstring id, std::_tstring inModule, std::_tstring inAttributeName, IAttributeType *attrType);
    void OpenAttribute(std::_tstring line, int column, int len, std::_tstring module, std::_tstring attribute, std::_tstring attributeType);
    boost::filesystem::path BookmarksFilePath();

    virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
    LRESULT OnInitDialog(HWND hWnd, LPARAM lParam);
    void OnDestroy();
    void OnContextMenu(HWND phWnd, CPoint pPoint);

    LRESULT OnLvnItemchangedListHistory(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);
    LRESULT OnLvnItemDelete(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);
    LRESULT OnNmDblClk(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);
    LRESULT OnBnClickedCheckFilter(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

    afx_msg void OnButtonSetBookmarks();
    afx_msg void OnUpdateButtonSetBookmarks(CCmdUI* pCmdUI);
    afx_msg void OnButtonRemoveBookmarks();
    afx_msg void OnUpdateButtonRemoveBookmarks(CCmdUI* pCmdUI);

    afx_msg void OnOpen();
    afx_msg void OpenMarkedBookmarks(bool val);
    afx_msg void OnUpdateOpen(CCmdUI* pCmdUI);
    afx_msg void OnSaveFile();
    afx_msg void OnUpdateSaveFile(CCmdUI* pCmdUI);
    afx_msg void OnLoadFile(bool mergeFlag=false);
    afx_msg void OnDeleteLines();
    afx_msg void OnUpdateLoadFile(CCmdUI* pCmdUI);

    LRESULT OnClear(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
    LRESULT OnReset(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
    LRESULT OnRefresh(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);

    void DoRefresh(ISciBookmarksMarker *errors, int nSel);
};
