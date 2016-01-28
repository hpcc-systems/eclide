#include "stdafx.h"
#include "..\en_us\resource.h"

#include "DialogAddBreakpoint.h"
#include "TestSocket.h"

#include "msxml2.h"
#include "SAXHandlers.h"

CDialogAddBreakpoint::CDialogAddBreakpoint(CWnd* pParent /*=NULL*/)
{
    m_stringMode = _T("Break");
    m_stringConditionMode = _T("Always");
    m_stringCountMode = _T("Always");
    m_caseSensitive = 0;
}

CDialogAddBreakpoint::~CDialogAddBreakpoint()
{
}

LRESULT CDialogAddBreakpoint::OnInitDialog(HWND /*wParam*/, LPARAM /*lParam*/)
{
    WTL::CComboBox combo = GetDlgItem(IDC_COMBOBOXEX_ACTION);
    combo.LimitText(256);
    combo.AddString(_T("Break"));
    combo.AddString(_T("Skip"));
    combo.AddString(_T("Limit"));
    combo.AddString(_T("Continue"));

    m_comboConditionMode = GetDlgItem(IDC_COMBOBOXEX_CONDITION);
    m_comboConditionMode.LimitText(256);
    m_comboConditionMode.AddString(_T("Always"));
    m_comboConditionMode.AddString(_T("Contains"));
    m_comboConditionMode.AddString(_T("Equals"));
    m_comboConditionMode.AddString(_T("StartsWith"));
    //m_comboConditionMode.AddString(_T("EndsWith"));
    m_comboConditionMode.AddString(_T("<"));
    m_comboConditionMode.AddString(_T("<="));
    m_comboConditionMode.AddString(_T(">"));
    m_comboConditionMode.AddString(_T(">="));
    m_comboConditionMode.AddString(_T("!="));
    m_comboConditionMode.AddString(_T("eog"));
    m_comboConditionMode.AddString(_T("eof"));

    m_comboCountMode = GetDlgItem(IDC_COMBOBOXEX_COUNT);
    m_comboCountMode.LimitText(256);
    m_comboCountMode.AddString(_T("Always"));
    m_comboCountMode.AddString(_T("Equals"));
    m_comboCountMode.AddString(_T("Atleast"));

    m_editField = GetDlgItem(IDC_EDIT_FIELD);
    m_editCondition = GetDlgItem(IDC_EDIT_CONDITION);
    m_buttonCaseSensitive = GetDlgItem(IDC_CHECK_CASESENSITIVE);
    m_editCount= GetDlgItem(IDC_EDIT_COUNT);

    DoDataExchange();
    CenterWindow(GetParent());
    BOOL b;
    OnUpdateUI(0, 0, 0, b);
    return TRUE;
}

LRESULT CDialogAddBreakpoint::OnUpdateUI(WORD wNotifyCode, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    DoDataExchange(true);

    bool conditionAlways = boost::algorithm::equals(m_stringConditionMode, _T("Always"));
    m_editField.EnableWindow(!conditionAlways);
    m_editCondition.EnableWindow(!conditionAlways);
    m_buttonCaseSensitive.EnableWindow(!conditionAlways);

    bool countAlways = boost::algorithm::equals(m_stringCountMode, _T("Always"));
    m_editCount.EnableWindow(!countAlways);
    return 0;
}

void CDialogAddBreakpoint::OnOk(UINT /*uNotifyCode*/, int nID, HWND /*hWnd*/)
{
    DoDataExchange(true);
    EndDialog(nID);
}

void CDialogAddBreakpoint::OnCancel(UINT /*uNotifyCode*/, int nID, HWND /*hWnd*/)
{
    EndDialog(nID);
}
