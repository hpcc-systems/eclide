#include "StdAfx.h"
#include "..\en_us\resource.h"

#include "MainFrm.h"
#include "historyview.h"
#include "repository.h"
#include "SourceView.h"
#include "PreferenceDlg.h"
#include "InsAttrDlg.h"
#include <utilDateTime.h> //clib

const TCHAR * const NOW = _T("Now");

class CCurrent : public IAttributeHistory, public clib::CLockableUnknown
{
private:
    CString m_text;
    CString m_modifiedBy;
    IAttributeType *m_type;

public:
    IMPLEMENT_CLOCKABLEUNKNOWN;
    CCurrent(IAttribute *attr, const CString & curr) : m_type(attr ? attr->GetType() : NULL), m_text(curr), m_modifiedBy(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_USER))
    {
    }
    const TCHAR *GetID() const
    {
        return _T("");
    }
    const TCHAR *GetModuleQualifiedLabel(bool excludeRoot = false) const
    {
        return _T("");
    }
    const TCHAR *GetLabel() const
    {
        return _T("");
    }
    const TCHAR *GetQualifiedLabel(bool excludeRoot = false, bool includeExtension = false) const
    {
        return _T("");
    }
    IAttributeType * GetType() const
    {
        return m_type;
    }
    const TCHAR *GetText(bool refresh, bool noBroadcast = false) const
    {
        return m_text;
    }
    const TCHAR *GetModifiedDate() const
    {
        return NOW;
    }
    const TCHAR *GetModifiedBy() const
    {
        return m_modifiedBy;
    }
    const TCHAR *GetDescription() const
    {
        return _T("Text In Open Window");
    }
    int GetVersion() const
    {
        return 0;
    }
    IAttribute * GetAttribute() const
    {
        return NULL;
    }
    bool SetLabel(const std::_tstring & label)
    {
        return false;
    }
};

class CSandbox : public IAttributeHistory, public clib::CLockableUnknown
{
private:
    CComPtr<IAttribute> m_attr;

public:
    IMPLEMENT_CLOCKABLEUNKNOWN;
    CSandbox(IAttribute *attr) : m_attr(attr)
    {
    }
    const TCHAR *GetID() const
    {
        return m_attr->GetID();
    }
    const TCHAR *GetModuleQualifiedLabel(bool excludeRoot = false) const
    {
        return m_attr->GetModuleQualifiedLabel();
    }
    const TCHAR *GetLabel() const
    {
        return m_attr->GetLabel();
    }
    const TCHAR *GetQualifiedLabel(bool excludeRoot = false, bool includeExtension = false) const
    {
        return m_attr->GetQualifiedLabel(excludeRoot, includeExtension);
    }
    IAttributeType * GetType() const
    {
        return m_attr->GetType();
    }
    const TCHAR *GetText(bool refresh, bool noBroadcast = false) const
    {
        return m_attr->GetText(refresh, noBroadcast);
    }
    const TCHAR *GetModifiedDate() const
    {
        return m_attr->GetModifiedDate();
    }
    const TCHAR *GetModifiedBy() const
    {
        return m_attr->GetLockedBy();
    }
    const TCHAR *GetDescription() const
    {
        return _T("Sandboxed");
    }
    int GetVersion() const
    {
        return 0;
    }
    IAttribute * GetAttribute() const
    {
        return NULL;
    }
    bool SetLabel(const std::_tstring & label)
    {
        return false;
    }
};

//=========================================================================
CHistoryView::CHistoryView(IHistorySlot *owner) : m_owner(owner), m_ecl(this)
{
    m_DiffPath.Create();
    m_LeftPath.Create();
    m_RightPath.Create();
    m_DiffPath.HandsOff();
    m_LeftPath.HandsOff();
    m_RightPath.HandsOff();
    m_SelCount = 0;
    m_CompareMode = false;
    m_FocusOnEcl = false;
    m_FocusOnDiff = false;
    m_loadedVersion = -1;
}

CHistoryView::~CHistoryView()
{
    m_DiffPath.HandsOn();
    m_LeftPath.HandsOn();
    m_RightPath.HandsOn();
}

void CHistoryView::SetAttribute(const CString & module, const CString & attribute, IAttributeType * type)
{
    CComPtr<IRepository> rep = ::AttachRepository();
    CComPtr<IAttribute> attr = rep->GetAttribute(module, attribute, type);
    SetAttribute(attr);
}

void CHistoryView::SetAttribute(IAttribute * attribute, const TCHAR *currEcl)
{
    if (m_loadedVersion == (int)attribute->GetVersion() && m_History.size() > 0)
    {
        m_eclStr = currEcl;
        m_Current = new CCurrent(attribute,currEcl);
        m_History[0] = (IAttributeHistory*)m_Current;
        m_Sel1 = m_Sel2 = -1;
        DoRefresh();
    }
    else
    {
        ATLASSERT(attribute);
        m_Attribute = attribute;
        m_eclStr = currEcl;
        m_History.clear();
        m_Attribute->GetHistory(m_History);
        if (m_Attribute->IsSandboxed())
        {
            m_Sandbox = new CSandbox(m_Attribute);
            m_History.insert(m_History.begin(), m_Sandbox.p);
        }
        if (currEcl)
        {
            m_Current = new CCurrent(m_Attribute,currEcl);
            m_History.insert(m_History.begin(), m_Current.p);
        }
        if (IsWindow())
        {
            initHistoryList();
        }
    }
}

void CHistoryView::SetCurrent(const CString & curr)
{
    m_Current = new CCurrent(m_Attribute,curr);
    m_History.push_back(m_Current.p);
}

void CHistoryView::SetCompareMode(bool compareMode)
{
    m_CompareMode = compareMode;
}

IAttribute * CHistoryView::GetAttribute() 
{
    return m_Attribute;
}

const TCHAR * CHistoryView::GetCluster()
{
    m_cluster = CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_CLUSTER));
    return m_cluster.c_str();
}

void CHistoryView::GetText(CString & txt)
{
    txt = m_eclStr;
}

bool CHistoryView::CanExecute()
{
    return false;
}

int CHistoryView::GetDiffPrevious()
{
    if (m_ecl.IsWindowVisible())
        return -1;

    return m_diffView.GetDiffPrevious();
}

int CHistoryView::GetDiffNext()
{
    if (m_ecl.IsWindowVisible())
        return -1;

    return m_diffView.GetDiffNext();
}

BOOL CHistoryView::PreTranslateMessage(MSG* pMsg)
{
    return CWindow::IsDialogMessage(pMsg);
}

LRESULT CHistoryView::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    SetMsgHandled(false);
    DlgResize_Init(false,false,WS_CLIPSIBLINGS);

    m_HistoryList = GetDlgItem(IDC_LIST_HISTORY);
    m_HistoryListFocus = m_HistoryList;
    RECT r;
    m_HistoryList.GetWindowRect(&r);
    m_HistoryList.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
    m_HistoryList.AddColumn(_T("Date"), 0);
    m_HistoryList.AddColumn(_T("User"), 1);
    m_HistoryList.AddColumn(_T("Labels"), 2);
    m_HistoryList.AddColumn(_T("Comment"), 3);
    int width = r.right - r.left;
    m_HistoryList.SetColumnWidth(0, width / 4);
    m_HistoryList.SetColumnWidth(1, width / 4);
    m_HistoryList.SetColumnWidth(2, width / 4);
    m_HistoryList.SetColumnWidth(3, width / 4);

    if (m_loadedVersion < 0 && m_Attribute)
    {
        initHistoryList();
    }
    initIEView();
    initECLView();

    return TRUE;
}

void CHistoryView::initHistoryList()
{
    m_loadedVersion = m_Attribute->GetVersion();
    m_labels.clear();
    CRepLabelVector localLabels;
    CComPtr<IRepository> rep = ::AttachRepository();
    rep->GetLabels(m_Attribute->GetModuleQualifiedLabel(), m_Attribute->GetLabel(), m_labels, localLabels);

    m_HistoryList.DeleteAllItems();
    boost::gregorian::date prevDate(boost::gregorian::day_clock::local_day());
    boost::posix_time::ptime prevTime(boost::posix_time::second_clock::universal_time());

    int i = 0;
    int recentServerRow = 0;  //No point in comparing to local only history items.
    CRepLabelVector::reverse_iterator itrLabels = m_labels.rbegin();
    for(IAttributeHistoryVector::iterator itrHistory = m_History.begin(); itrHistory != m_History.end(); ++itrHistory)
    {
        IAttributeHistory *h = itrHistory->get();
        m_HistoryList.AddItem(i, 0, h->GetModifiedDate());
        m_HistoryList.AddItem(i, 1, h->GetModifiedBy());
        boost::gregorian::date modDate(boost::gregorian::day_clock::local_day());
        boost::posix_time::ptime modTime(boost::posix_time::second_clock::universal_time());
        if (h->GetModifiedDate() != NOW)
            UTCToDateTime(h->GetModifiedDate(), modDate, modTime);

        CString labelList;
        for(; itrLabels != m_labels.rend() && itrLabels->get()->m_time >= modTime; ++itrLabels)
        {
            if (labelList.GetLength() > 0)
                labelList += _T(", ");
            labelList += itrLabels->get()->m_label.c_str();
            labelList += _T("(");
            labelList += itrLabels->get()->m_datetime.c_str();
            labelList += _T(")");
        }
        for(CRepLabelVector::iterator itrLocalLabels = localLabels.begin(); itrLocalLabels != localLabels.end(); ++itrLocalLabels)
        {
            if (h->GetVersion() >= 0 &&  h->GetVersion() == itrLocalLabels->get()->m_version)
            {
                if (labelList.GetLength() > 0)
                    labelList += _T(", ");
                labelList += itrLocalLabels->get()->m_label.c_str();
            }
        }

        m_HistoryList.AddItem(i, 2, labelList);
        m_HistoryList.AddItem(i, 3, h->GetDescription());
        if (h->GetModifiedDate() != NOW)
            UTCToDateTime(h->GetModifiedDate(), prevDate, prevTime);
        if (!recentServerRow && h->GetVersion() > 0)
            recentServerRow = i;
        ++i;
    }

    m_Sel1 = m_Sel2 = -1;
    m_HistoryList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
    if (m_CompareMode)
        m_HistoryList.SetItemState(recentServerRow, LVIS_SELECTED, LVIS_SELECTED);
}

void CHistoryView::initIEView()
{
    m_editLHS = GetDlgItem(IDC_EDIT_LHS);
    m_editRHS = GetDlgItem(IDC_EDIT_RHS);
    CWindow wndPlaceholder = GetDlgItem(IDC_IE_PLACEHOLDER);
    CRect rc;
    wndPlaceholder.GetWindowRect(rc);
    ScreenToClient(rc);
    wndPlaceholder.DestroyWindow();
    m_diffView.Create(*this, rc, _T(""), WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, WS_EX_CLIENTEDGE, IDC_IE_PLACEHOLDER);
    m_diffView.SetViewEOL(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SHOWCRLF));
}

void CHistoryView::initECLView()
{
    CWindow wndPlaceholder = GetDlgItem(IDC_ECL_PLACEHOLDER);
    CRect rc;
    wndPlaceholder.GetWindowRect ( rc );
    ScreenToClient(rc);
    wndPlaceholder.DestroyWindow();
    m_ecl.Create(*this, rc, _T(""), WS_CHILD | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE, IDC_ECL_PLACEHOLDER);
    m_ecl.DoInit();
    m_ecl.SetReadOnly(true);
    InitEclCommandMixin(::GetUIUpdate(), ::GetIMainFrame(), &m_ecl, this);
}

LRESULT CHistoryView::OnRefresh(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    DoRefresh();
    return 0;
}

void CHistoryView::SetUrl(const CString & url)
{
    //m_wndIE.Navigate(url);
}

void GetHeader(IAttributeHistory * attr, CString & header)
{
    header = (boost::_tformat(_T("%1% (%2%)\n%3%\n")) % attr->GetModifiedDate() % attr->GetModifiedBy() % attr->GetDescription()).str().c_str();
}

void CHistoryView::DoRefresh()
{
    int currentCount = m_HistoryList.GetSelectedCount();
    if (currentCount != m_SelCount)
    {
        m_SelCount = currentCount;
        switch (m_HistoryList.GetSelectedCount())
        {
        case 1:
            DoShowSingle();
            break;
        case 2:
            DoShowDouble();
            break;
        default:
            DoShowDouble();
            break;
        }
    }

    int sel1 = -1, sel2 = -1;
    sel1 = m_HistoryList.GetNextItem(sel1, LVNI_SELECTED);
    sel2 = m_HistoryList.GetNextItem(sel1, LVNI_SELECTED);	
    if (m_Sel1 != sel1 || m_Sel2 != sel2)
    {
        m_Sel1 = sel1;
        m_Sel2 = sel2;
        if (m_Sel1 != -1 && m_Sel2 != -1)
        {
            //CWaitCursor();
            //GetHeader(m_History[m_Sel1].get(), leftHeader);
            //GetHeader(m_History[m_Sel2].get(), rightHeader);
            //Compare(leftHeader, left, rightHeader, right, m_DiffPath.TempFileName(), m_LeftPath.TempFileName(), m_RightPath.TempFileName(), GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_TAB_WIDTH), GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_IGNOREWHITESPACE), GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SHOWCRLF));
            //SetUrl(m_DiffPath.TempFileName());
            m_editLHS.SetWindowText((boost::_tformat(_T("%1% - %2%")) % m_History[m_Sel2]->GetModifiedDate() % m_History[m_Sel2]->GetModifiedBy()).str().c_str());
            m_editRHS.SetWindowText((boost::_tformat(_T("%1% - %2%")) % m_History[m_Sel1]->GetModifiedDate() % m_History[m_Sel1]->GetModifiedBy()).str().c_str());
            m_diffView.SetText(m_History[m_Sel2]->GetText(false), m_History[m_Sel1]->GetText(false), m_History[m_Sel2]->GetType(), m_History[m_Sel1]->GetType());
        }
        else if (m_Sel1 != -1)
        {
            CString txtHeader;
            CWaitCursor();
            GetHeader(m_History[m_Sel1].get(), txtHeader);
            CString txt = _T("/*");
            txt += txtHeader + _T("*/\r\n");
            txt += m_History[m_Sel1]->GetText(false);
            m_ecl.InitLanguage(m_History[m_Sel1]->GetType());
            m_ecl.SetReadOnly(false);
            m_ecl.SetText(txt);  //TODO:  Retest
            m_ecl.SetReadOnly(true);
        }
    }
    m_owner->OnRefresh();
}

void CHistoryView::DoShowSingle()
{
    //m_pWB2->put_Visible(false);
    m_editLHS.ShowWindow(SW_HIDE);
    m_editRHS.ShowWindow(SW_HIDE);
    m_diffView.ShowWindow(SW_HIDE);
    m_ecl.ShowWindow(SW_SHOW);
}
void CHistoryView::DoShowDouble()
{
    //m_pWB2->put_Visible(true);
    m_editLHS.ShowWindow(SW_SHOW);
    m_editRHS.ShowWindow(SW_SHOW);
    m_diffView.ShowWindow(SW_SHOW);
    m_ecl.ShowWindow(SW_HIDE);
}
void CHistoryView::DoShowNone()
{
    //m_pWB2->put_Visible(false);
    m_editLHS.ShowWindow(SW_HIDE);
    m_editRHS.ShowWindow(SW_HIDE);
    m_diffView.ShowWindow(SW_HIDE);
    m_ecl.ShowWindow(SW_HIDE);
}

LRESULT CHistoryView::OnLvnItemchangedListHistory(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    if (pNMLV->uChanged & LVIF_STATE)
        PostMessage(CWM_REFRESH);

    return 0;
}

void CHistoryView::OnHistoryRollback(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
{
    if (MessageBox(ROLLBACK_MESSAGE, _T("Reinstate"), MB_YESNO | MB_DEFBUTTON1 | MB_ICONQUESTION) == IDYES)
    {
        int sel = -1;
        sel = m_HistoryList.GetNextItem(sel, LVNI_SELECTED);
        if (sel >= 0)
        {
            CWaitCursor wait;
            m_Attribute->Checkout();
            m_Attribute->SetText(m_History[sel]->GetText(false));
            CString comment;
            comment.Format(_T("Automated reinstate from %s"), m_History[sel]->GetModifiedDate());
            m_Attribute->Checkin(comment);

            SetAttribute(m_Attribute, m_Attribute->GetText(false));
            m_owner->SetAttributeText(m_Attribute, m_Attribute->GetText(false), true, false);
            DoRefresh();
        }
    }
}

void CHistoryView::OnHistoryLabel(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
{
    CString label;
    if (SetVersionLabel(label, false))
    {
        int sel = -1;
        sel = m_HistoryList.GetNextItem(sel, LVNI_SELECTED);
        if (sel >= 0)
        {
            CWaitCursor wait;
            m_History[sel]->SetLabel((const TCHAR *)label);
            initHistoryList();
            DoRefresh();
        }
    }
}

void CHistoryView::OnDifferencePrevious(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
{
    m_diffView.GotoDiffPrevious();
}

void CHistoryView::OnDifferenceNext(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
{
    m_diffView.GotoDiffNext();
}

void CHistoryView::OnContextMenu(HWND phWnd, CPoint pt)
{
    // on a "sudden" right click into the list we may have the
    // menus set for the wrong pane
    //UIUpdateMenuItems(GetIMainFrame()->GetUpdateUIBase());
    if ( m_FocusOnDiff )
    {
        SetMsgHandled(false);
        return;
    }

    if (phWnd == (HWND)m_HistoryList)
    {
        WTL::CMenu m;
        m.LoadMenu(IDR_POPUP_HISTORY);
        m.EnableMenuItem(ID_HISTORY_ROLLBACK, m_HistoryList.GetSelectedCount() == 1 ? MF_ENABLED : MF_GRAYED);
        m.EnableMenuItem(ID_HISTORY_LABEL, m_HistoryList.GetSelectedCount() == 1 ? MF_ENABLED : MF_GRAYED);
        if (!m_owner->OnHistoryContext(m, pt))
        {
            unsigned id = m.GetSubMenu(0).TrackPopupMenuEx(TPM_RETURNCMD, pt.x, pt.y, *this);
            m.DestroyMenu();
            if (id)
                PostMessage(WM_COMMAND, id);
        }
    }
    else
    {
        WTL::CMenu m, edit;
        m.LoadMenu(IDR_MAINFRAME);
        edit = m.GetSubMenu(1);
        GetIMainFrame()->CloneState(edit);
        BOOL id = edit.TrackPopupMenuEx(TPM_RETURNCMD, pt.x, pt.y, phWnd, NULL);
        m.DestroyMenu();
        if (id)
            PostMessage(WM_COMMAND, id);
    }
}

void CHistoryView::GetTitle(CString & title)
{
    title = m_Attribute->GetQualifiedLabel();
}

bool CHistoryView::IsDirty()
{
    return false;
}

void CHistoryView::PostStatus(const TCHAR *status)
{
    ::GetIMainFrame()->PostStatus(status);
}

void CHistoryView::OnSetFocus(HWND /*other*/)
{
    //ATLTRACE(_T("OnSetFocus=%d\n"), m_FocusOnEcl );
    if( m_FocusOnEcl && ::GetFocus() != m_ecl.m_hWnd )
        SendMessage(WM_NEXTDLGCTL, WPARAM(m_ecl.m_hWnd), LPARAM(TRUE) );
}

void CHistoryView::OnKillFocus(HWND /*other*/)
{
    //ATLTRACE(_T("OnKillFocus=%d\n"), m_FocusOnEcl );
}
void CHistoryView::OnSize(UINT nType, CSize size)
{
    SetMsgHandled(false);
    CRect rc;
    m_editLHS.GetWindowRect(rc);
    ScreenToClient(rc);
    m_editLHS.MoveWindow(rc.left, rc.top, size.cx / 2 - 1, rc.Height());
    m_editRHS.GetWindowRect(rc);
    ScreenToClient(rc);
    m_editRHS.MoveWindow(size.cx / 2 + 1, rc.top, size.cx / 2 - 1, rc.Height());
}

void CHistoryView::UIUpdateTitle()
{
}

bool CHistoryView::UIUpdateMenuItems(CCmdUI * cui)
{
    m_FocusOnEcl = false;
    m_FocusOnDiff = false;

    if ( m_HistoryListFocus.HasFocus() )
    {
        UPDATEUI(cui, ID_EDIT_SELECT_ALL, true);
        UPDATEUI(cui, ID_EDIT_COPY, true);
    }
    else if ( m_ecl.IsWindowVisible() )
    {
        m_FocusOnEcl = true;
    }
    else 
    {
        m_FocusOnDiff = true;
    }

    if ( m_ecl.IsWindowVisible() )
    {
        if (baseClassEclCmd::UIUpdateMenuItems(cui))
            return true;
        UPDATEUI(cui, ID_ECL_CHECKSYNTAX, false);
        UPDATEUI(cui, ID_FILE_SAVE, false);	
        UPDATEUI(cui, ID_FILE_SAVE_AS, false);	
        UPDATEUI(cui, ID_ECL_CHECKCOMPLEXITY, false);
        UPDATEUI(cui, ID_ECL_CHECKDEPENDENCY, false);
        UPDATEUI(cui, ID_ECL_SYNCTOC, false);
        UPDATEUI(cui, ID_ECL_GOTOSYNCTOC, false);
        UPDATEUI(cui, ID_ECL_GOTO, false);
    }
    else 
    {
        UPDATEUI(cui, ID_DIFFERENCES_PREVIOUS, m_diffView.GetDiffPrevious() >= 0);
        UPDATEUI(cui, ID_DIFFERENCES_NEXT, m_diffView.GetDiffNext() >= 0);
    }

    return false;
}


