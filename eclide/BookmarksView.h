#pragma once

#include "Util.h"
#include "Attribute.h"
#include "SciSyntaxMarker.h"
#include "CSortMFCListCtrl.h"
#include <CustomMessages.h>

const TCHAR * const CLEAR_BOOKMARKS_MSG = _T("Are you sure you want clear the displayed bookmarks?");
const TCHAR * const SAVE_BOOKMARKS_OVERWRITE_MSG = _T("Are you sure you want overwrite the master bookmarks file?");
const TCHAR * const SAVE_BOOKMARKS_MSG = _T("Are you sure you want to save the current bookmarks as a master bookmarks file?");
const TCHAR * const LOAD_BOOKMARKS_MSG = _T("Are you sure you want replace the current list of bookmarks with contents of the master?");
const TCHAR * const LOAD_MERGE_BOOKMARKS_MSG = _T("Are you sure you want to merge your saved bookmarks with the master list?");
const TCHAR * const DELETE_BOOKMARKS_MSG = _T("Do you want to delete the selected bookmark(s) from the list?");

enum BM_SERVER_TYPE
{
    BM_SERVER_TYPE_UNKNOWN = 0,
    BM_SERVER_TYPE_LOCAL,
    BM_SERVER_TYPE_REMOTE
};

enum BM_FILE_TYPE
{
    BM_FILE_MASTER = 0,
    BM_FILE_STATE
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
    bool m_checkMine;
    bool m_checkTodos;
    bool m_checkHacks;
    bool m_masterExists;
    bool m_stateExists;
    bool m_filesDiff;
    bool m_hasBookmarks;
    bool m_hasSelection;
    std::_tstring m_masterXML;
    std::_tstring m_stateXML;
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
    void SetMarks(const std::_tstring inPath, const std::_tstring inModule, const std::_tstring inAttributeName, bool val);
    void DeleteMarkedBookmarks(const std::_tstring inPath, const std::_tstring inModule, const std::_tstring inAttributeName, bool val);
    void ParseBookmarks(IAttribute *attribute);
    void ParseBookmarksEcl(const std::_tstring & ecl, const std::_tstring & user=_T(""), const std::_tstring & id=_T(""), const std::_tstring & inModule=_T(""), const std::_tstring & inAttributeName=_T(""), IAttributeType *attrType=NULL);
    boost::filesystem::path BookmarksFilePath(bool saveState=false);
    std::_tstring ModuleFromPath(const std::_tstring & pathStr, const std::_tstring & attrName);
    int GetCount();
    int GetSelectedCount();
    void Open();
    void Save(bool saveState=false);
    void Load(bool mergeFlag=false, BM_FILE_TYPE fileType=BM_FILE_MASTER);
    void Clear();
    void OpenAttribute(int listRow);
    bool BookmarkFilesState();
    bool FileText(BM_FILE_TYPE fileType, std::_tstring& result);
    bool FileDiff();
    bool HasSelection();
    bool CanSave();
    bool CanLoad();
    bool CanLoadMerge();
    bool HasBookmarks();

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
    afx_msg void OnUpdateOpen(CCmdUI* pCmdUI);
    afx_msg void OnSaveFile(bool saveState = false);
    afx_msg void OnUpdateSaveFile(CCmdUI* pCmdUI);
    afx_msg void OnLoadFile(bool mergeFlag=false , BM_FILE_TYPE fileType=BM_FILE_MASTER);
    afx_msg void OnDeleteLines();
    afx_msg void OnUpdateLoadFile(CCmdUI* pCmdUI);

    LRESULT OnClear(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
    LRESULT OnReset(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
    LRESULT OnRefresh(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);

    void DoRefresh(ISciBookmarksMarker *errors, int nSel);
};
