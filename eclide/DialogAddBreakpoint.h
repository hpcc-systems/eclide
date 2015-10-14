#pragma once

#include "StdString.h"

class CDialogAddBreakpoint : 
    public WTL::CDialogImpl<CDialogAddBreakpoint>, 
    public CWinDataExchangeStd<CDialogAddBreakpoint>
{
public:
    WTL::CComboBox m_comboConditionMode;
    WTL::CComboBox m_comboCountMode;
    WTL::CEdit m_editField;
    WTL::CEdit m_editCondition;
    WTL::CButton m_buttonCaseSensitive;
    WTL::CEdit m_editCount;

    std::_tstring m_stringMode;
    std::_tstring m_stringConditionMode;
    std::_tstring m_stringField;
    std::_tstring m_stringCondition;
    std::_tstring m_stringCountMode;
    std::_tstring m_stringCount;
    int m_caseSensitive;

    CDialogAddBreakpoint(CWnd* pParent = NULL);   // standard constructor
    virtual ~CDialogAddBreakpoint();

// Dialog Data
    enum { IDD = IDD_ROXIE_ADDBREAKPOINT };

    BEGIN_MSG_MAP(CDialogAddBreakpoint)
        MSG_WM_INITDIALOG(OnInitDialog)

        COMMAND_HANDLER(IDC_COMBOBOXEX_CONDITION, CBN_CLOSEUP, OnUpdateUI)		
        COMMAND_HANDLER(IDC_COMBOBOXEX_COUNT, CBN_CLOSEUP, OnUpdateUI)		
        COMMAND_ID_HANDLER_EX(IDOK, OnOk)
        COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
    END_MSG_MAP()

    BEGIN_DDX_MAP(CDialogAddBreakpoint)
        DDX_COMBO_VALUE(IDC_COMBOBOXEX_ACTION, m_stringMode);
        DDX_COMBO_VALUE(IDC_COMBOBOXEX_CONDITION, m_stringConditionMode);
        DDX_TEXT(IDC_EDIT_FIELD, m_stringField);
        DDX_TEXT(IDC_EDIT_CONDITION, m_stringCondition);
        DDX_COMBO_VALUE(IDC_COMBOBOXEX_COUNT, m_stringCountMode);
        DDX_TEXT(IDC_EDIT_COUNT, m_stringCount);
        DDX_CHECK(IDC_CHECK_CASESENSITIVE, m_caseSensitive);
    END_DDX_MAP()

public:
    LRESULT OnInitDialog(HWND /*wParam*/, LPARAM /*lParam*/);
    LRESULT OnUpdateUI(WORD wNotifyCode, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    void OnOk(UINT /*uNotifyCode*/, int nID, HWND /*hWnd*/);
    void OnCancel(UINT /*uNotifyCode*/, int nID, HWND /*hWnd*/);
};
