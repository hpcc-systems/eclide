#include "StdAfx.h"
#include "..\en_us\resource.h"

#include "SyntaxView.h"
#include "RecursiveMutex.h"

static const SectionLabelDefault GLOBAL_SYNTAX_SHOWERRORS(SectionLabel(_T("Syntax"), _T("Errors")), true);
static const SectionLabelDefault GLOBAL_SYNTAX_SHOWWARNINGS(SectionLabel(_T("Syntax"), _T("Warnings")), true);
static const SectionLabelDefault GLOBAL_SYNTAX_SHOWINFORMATION(SectionLabel(_T("Syntax"), _T("Information")), false);

//  ===========================================================================
CSyntaxView::CSyntaxView() 
{
    m_prevSyntaxMarker = NULL;
}

CSyntaxView::~CSyntaxView()
{
}

void CSyntaxView::Init(ISyntaxSlot *owner) 
{
    m_Owner = owner;
}

BOOL CSyntaxView::PreTranslateMessage(MSG* pMsg)
{
    if((pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) && (pMsg->message < WM_MOUSEFIRST || pMsg->message > WM_MOUSELAST))
        return FALSE;

    HWND hWndCtl = ::GetFocus();
    if(IsChild(hWndCtl))
    {
        // find a direct child of the dialog from the window that has focus
        while(::GetParent(hWndCtl) != m_hWnd)
            hWndCtl = ::GetParent(hWndCtl);

        // give control a chance to translate this message
        if(::SendMessage(hWndCtl, WM_FORWARDMSG, 0, (LPARAM)pMsg) != 0)
            return TRUE;
    }

    return CWindow::IsDialogMessage(pMsg);
}

LRESULT CSyntaxView::OnInitDialog(HWND /*hWnd*/, LPARAM /*lParam*/)
{
    SetMsgHandled(false);
    DlgResize_Init(false,false);

    m_list = GetDlgItem(IDC_LIST_SYNTAX);
    m_list.AddColumn(_T("Message"), 0);
    m_list.AddColumn(_T("Code"), 1);
    m_list.AddColumn(_T("Location"), 2);
    m_list.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT);

    boost::filesystem::path iniPath;
    GetIniPath(iniPath);
    CComPtr<IConfig> iniFile = CreateIConfig(QUERYBUILDER_INI, iniPath);

    m_checkErrors = iniFile->Get(GLOBAL_SYNTAX_SHOWERRORS);
    m_checkWarnings = iniFile->Get(GLOBAL_SYNTAX_SHOWWARNINGS);
    m_checkInformation = iniFile->Get(GLOBAL_SYNTAX_SHOWINFORMATION);

    DoDataExchange();

    return 0;
}

void CSyntaxView::OnDestroy()
{
    DoDataExchange(true);

    boost::filesystem::path iniPath;
    GetIniPath(iniPath);
    CComPtr<IConfig> iniFile = CreateIConfig(QUERYBUILDER_INI, iniPath);

    iniFile->Set(GLOBAL_SYNTAX_SHOWERRORS, m_checkErrors);
    iniFile->Set(GLOBAL_SYNTAX_SHOWWARNINGS, m_checkWarnings);
    iniFile->Set(GLOBAL_SYNTAX_SHOWINFORMATION, m_checkInformation);

    SetMsgHandled(false);
}

void FormatRow(WTL::CListViewCtrl & m_list, int row, std::_tstring & clip)
{
    CString text;
    m_list.GetItemText(row, 0, text);
    clip += (const TCHAR *)text;
    m_list.GetItemText(row, 1, text);
    clip += _T(", ");
    clip += (const TCHAR *)text;
    m_list.GetItemText(row, 2, text);
    clip += _T(", ");
    clip += (const TCHAR *)text;
}

void CSyntaxView::OnContextMenu(HWND /*phWnd*/, CPoint pPoint)
{
    WTL::CMenu m;
    m.LoadMenu(IDR_POPUP_SYNTAX);
    m.EnableMenuItem(ID_EDIT_COPY, m_list.GetSelectedCount() == 0);
    m.EnableMenuItem(ID_EDIT_COPYALL, m_list.GetItemCount() == 0);
    m.EnableMenuItem(ID_SYNTAX_LOCATEERROR, m_list.GetItemCount() == 0);
    unsigned int id = m.GetSubMenu(0).TrackPopupMenuEx(TPM_RETURNCMD, pPoint.x, pPoint.y, m_hWnd, NULL);
    switch (id)
    {
    case ID_EDIT_COPY:
        {
            int row = m_list.GetSelectedIndex();
            std::_tstring clip;
            FormatRow(m_list, row, clip);
            SetClipboard(clip);
        }
        break;
    case ID_EDIT_COPYALL:
        {
            std::_tstring clip;
            for (int i = 0; i < m_list.GetItemCount(); ++i)
            {
                if (!clip.empty())
                    clip += _T("\r\n");
                FormatRow(m_list, i, clip);
            }
            SetClipboard(clip);
        }
        break;
    case ID_SYNTAX_LOCATEERROR:	
        {
            int row = m_list.GetSelectedIndex();
            if (row > -1)
            {
                CString code;
                m_list.GetItemText(row, 1, code);
                ShowHelp((const TCHAR *)code);
            }
        }
        break;
    }
}

LRESULT CSyntaxView::OnLvnItemchangedListHistory(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    int d;
    if (pNMLV->uChanged & LVIF_STATE)
        d = m_list.GetSelectedIndex();

    return 0;
}

LRESULT CSyntaxView::OnNmDblCLickListSyntax(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
{
    LPNMITEMACTIVATE lpnmitem = (LPNMITEMACTIVATE) pNMHDR;

    CWindow wnd(m_hWndOrigin);
    if (wnd.IsWindow())
        wnd.PostMessage(CWM_SYNTAXSYNC, lpnmitem->iItem);

    return 0;
}

LRESULT CSyntaxView::OnBnClickedCheckFilter(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    DoDataExchange(true);

    DoRefresh(m_prevSyntaxMarker, 0);
    return 0;
}

LRESULT CSyntaxView::OnClear(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    DoDataExchange();
    m_list.DeleteAllItems();
    m_prevSyntaxMarker = NULL;
    return 0;
}

LRESULT CSyntaxView::OnReset(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    DoDataExchange();
    m_list.DeleteAllItems();
    m_prevSyntaxMarker = NULL;
    return 0;
}

LRESULT CSyntaxView::OnRefresh(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam)
{
    DoRefresh(reinterpret_cast<ISciSyntaxMarker *>(wParam), lParam);
    return 0;
}

void CSyntaxView::DoRefresh(ISciSyntaxMarker *errors, int nSel)
{
    DoDataExchange(true);
    m_prevSyntaxMarker = errors;
    m_list.DeleteAllItems();
    if (errors)
    {
        m_hWndOrigin = errors->SyntaxGetParentHwnd();
        size_t item = 0;
        unsigned int errorCount = 0;
        unsigned int warningCount = 0;
        unsigned int infoCount = 0;
        for (; item < errors->SyntaxGetCount(); ++item)
        {
            SYNTAX_TYPE type;
            CString msg, code, source;
            errors->SyntaxGet(item, type, msg, code, source);
            switch (type)
            {
            case SYNTAX_ERROR:
                ++errorCount;
                if (!m_checkErrors)
                    continue;
                msg = _T("Error:    ") + msg;
                break;
            case SYNTAX_WARNING:
                ++warningCount;
                if (!m_checkWarnings)
                    continue;
                msg = _T("Warning:  ") + msg;
                break;
            case SYNTAX_INFO:
                ++infoCount;
                if (!m_checkInformation)
                    continue;
                msg = _T("Information:  ") + msg;
                break;
            }
            m_list.AddItem(item, 0, msg);
            switch (type)
            {
            case SYNTAX_ERROR:
            case SYNTAX_WARNING:
            case SYNTAX_INFO:
                m_list.AddItem(item, 1, code);
                m_list.AddItem(item, 2, source);
                break;
            }
        }
        if (errors->SyntaxGetCount() == 0)
        {
            m_list.AddItem(item, 0, _T("No Errors..."));
        }

        ::SetWindowText(GetDlgItem(IDC_CHECK_ERRORS), (boost::_tformat(_T("%1% Errors")) % errorCount).str().c_str());
        ::SetWindowText(GetDlgItem(IDC_CHECK_WARNINGS), (boost::_tformat(_T("%1% Warnings")) % warningCount).str().c_str());
        ::SetWindowText(GetDlgItem(IDC_CHECK_INFORMATION), (boost::_tformat(_T("%1% Information")) % infoCount).str().c_str());
    }
    else
    {
        m_list.AddItem(0, 0, _T("Checking..."));
    }

    for(int i = 0; i < 3; ++i)
        m_list.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);

    m_list.SelectItem(nSel);
}

//  ===========================================================================
