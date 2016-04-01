#pragma once

#include "SortList.h"
#include "Util.h"
#include "SciSyntaxMArker.h"
#include <CustomMessages.h>

__interface ISciSyntaxMarker;

__interface ISyntaxSlot
{
    bool OnDoubleClick();
    bool OnContext(CMenu &menu, CPoint &pt);
};

class CSyntaxView:
    public WTL::CDialogImpl<CSyntaxView>,
    public WTL::CDialogResize<CSyntaxView>,
    public WTL::CWinDataExchange<CSyntaxView>
{
    typedef CSyntaxView thisClass;
    typedef WTL::CDialogImpl<thisClass> baseClass;

protected:
    ISyntaxSlot *m_Owner;
    WTL::CListViewCtrl m_list;
    bool m_checkErrors;
    bool m_checkWarnings;
    bool m_checkInformation;

    CString m_module;
    CString m_attribute;
    CString m_ecl;
    HWND m_hWndOrigin;
    ISciSyntaxMarker * m_prevSyntaxMarker;

public:
    CSyntaxView();
    ~CSyntaxView();

    void Init(ISyntaxSlot *pMenuManager);

    enum { IDD = IDD_SYNTAX };

    BOOL PreTranslateMessage(MSG* pMsg);

    BEGIN_MSG_MAP(thisClass)
        MSG_WM_INITDIALOG(OnInitDialog)
        MSG_WM_DESTROY(OnDestroy)
        MSG_WM_CONTEXTMENU(OnContextMenu)

        NOTIFY_HANDLER(IDC_LIST_SYNTAX, LVN_ITEMCHANGED, OnLvnItemchangedListHistory)
        NOTIFY_HANDLER(IDC_LIST_SYNTAX, NM_DBLCLK, OnNmDblCLickListSyntax)

        COMMAND_HANDLER(IDC_CHECK_ERRORS, BN_CLICKED, OnBnClickedCheckFilter)
        COMMAND_HANDLER(IDC_CHECK_WARNINGS, BN_CLICKED, OnBnClickedCheckFilter)
        COMMAND_HANDLER(IDC_CHECK_INFORMATION, BN_CLICKED, OnBnClickedCheckFilter)

        MESSAGE_HANDLER_EX(CWM_CLEAR, OnClear)
        MESSAGE_HANDLER_EX(CWM_RESET, OnReset)
        MESSAGE_HANDLER_EX(CWM_REFRESH, OnRefresh)

        CHAIN_MSG_MAP(WTL::CDialogResize<thisClass>)
        REFLECT_NOTIFICATIONS()
    END_MSG_MAP()

    BEGIN_DLGRESIZE_MAP(thisClass)
        DLGRESIZE_CONTROL(IDC_LIST_SYNTAX, DLSZ_SIZE_X | DLSZ_SIZE_Y)
        DLGRESIZE_CONTROL(IDC_CHECK_ERRORS, DLSZ_MOVE_Y)
        DLGRESIZE_CONTROL(IDC_CHECK_WARNINGS, DLSZ_MOVE_Y)
        DLGRESIZE_CONTROL(IDC_CHECK_INFORMATION, DLSZ_MOVE_Y)
    END_DLGRESIZE_MAP()

    BEGIN_DDX_MAP(thisClass)
        DDX_CHECK(IDC_CHECK_ERRORS, m_checkErrors)
        DDX_CHECK(IDC_CHECK_WARNINGS, m_checkWarnings)
        DDX_CHECK(IDC_CHECK_INFORMATION, m_checkInformation)
    END_DDX_MAP()

protected:
    LRESULT OnInitDialog(HWND hWnd, LPARAM lParam);
    void OnDestroy();
    void OnContextMenu(HWND phWnd, CPoint pPoint);

    LRESULT OnLvnItemchangedListHistory(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);
    LRESULT OnNmDblCLickListSyntax(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);

    LRESULT OnBnClickedCheckFilter(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

    LRESULT OnClear(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
    LRESULT OnReset(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
    LRESULT OnRefresh(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);

    void DoRefresh(ISciSyntaxMarker *errors, int nSel);
};

