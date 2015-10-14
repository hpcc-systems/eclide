#include "stdafx.h"
#include "..\en_us\resource.h"

#include "DockablePropertiesView.h"

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar

CDockablePropertiesView::CDockablePropertiesView()
{
}

CDockablePropertiesView::~CDockablePropertiesView()
{
}

BEGIN_MESSAGE_MAP(CDockablePropertiesView, CDockablePane)
    ON_WM_CREATE()
    ON_WM_SIZE()
    //ON_COMMAND(ID_SORTINGPROP, OnSortingprop)
    //ON_UPDATE_COMMAND_UI(ID_SORTINGPROP, OnUpdateSortingprop)
    //ON_COMMAND(ID_PROPERIES1, OnProperies1)
    //ON_UPDATE_COMMAND_UI(ID_PROPERIES1, OnUpdateProperies1)
    //ON_COMMAND(ID_PROPERIES2, OnProperies2)
    //ON_UPDATE_COMMAND_UI(ID_PROPERIES2, OnUpdateProperies2)
    //ON_COMMAND(ID_EXPAND, OnExpand)
    //ON_UPDATE_COMMAND_UI(ID_EXPAND, OnUpdateExpand)
    ON_WM_SETFOCUS()
    ON_WM_SETTINGCHANGE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar message handlers

void CDockablePropertiesView::AdjustLayout()
{
    if (GetSafeHwnd() == NULL)
    {
        return;
    }

    CRect rectClient,rectCombo;
    GetClientRect(rectClient);

    m_wndObjectCombo.GetWindowRect(&rectCombo);

    int cyCmb = rectCombo.Size().cy;
    int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

    m_wndObjectCombo.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), 200, SWP_NOACTIVATE | SWP_NOZORDER);
    m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top + cyCmb, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
    m_wndPropList.SetWindowPos(NULL, rectClient.left, rectClient.top + cyCmb + cyTlb, rectClient.Width(), rectClient.Height() -(cyCmb+cyTlb), SWP_NOACTIVATE | SWP_NOZORDER);
}

void CDockablePropertiesView::Send_Clear()
{
    m_wndObjectCombo.ResetContent();
    m_wndPropList.RemoveAll();
}

void CDockablePropertiesView::Post_Reset()
{
    Send_Clear();
}

void CDockablePropertiesView::LoadProperties(const std::_tstring & label, Element::StringStringMap & local, Element::StringStringMap & global)
{
    Send_Clear();
    m_wndObjectCombo.AddString(label.c_str());
    m_wndObjectCombo.SetCurSel(0);

    CMFCPropertyGridProperty* pDebug = new CMFCPropertyGridProperty(_T("Debug"));
    CMFCPropertyGridProperty* pOther = new CMFCPropertyGridProperty(_T("Other"));

    for(Element::StringStringMap::const_iterator itr = local.begin(); itr != local.end(); ++itr)
    {
        global[itr->first] = itr->second;
    }

    for(Element::StringStringMap::const_iterator itr = global.begin(); itr != global.end(); ++itr)
    {
        if (boost::algorithm::starts_with(itr->first, _T("_")))
            pDebug->AddSubItem(new CMFCPropertyGridProperty(itr->first.c_str(), (_variant_t)itr->second.c_str(), itr->second.c_str()));
        else
            pOther->AddSubItem(new CMFCPropertyGridProperty(itr->first.c_str(), (_variant_t)itr->second.c_str(), itr->second.c_str()));
    }

    m_wndPropList.AddProperty(pOther);
    m_wndPropList.AddProperty(pDebug);
}

void CDockablePropertiesView::LoadProperties(const CComSafeArray<VARIANT> & properties)
{
    Send_Clear();
    CMFCPropertyGridProperty* pDebug = new CMFCPropertyGridProperty(_T("Debug"));
    CMFCPropertyGridProperty* pOther = new CMFCPropertyGridProperty(_T("Other"));

    std::_tstring label;
    for(unsigned int i = 0; i < properties.GetCount(); ++i)
    {
        CString key = properties.GetAt(i);
        CString val = properties.GetAt(++i);
        if (key.CompareNoCase(_T("Label")) == 0)
            label = (const TCHAR *)val;
        else if (label.empty() && key.CompareNoCase(_T("ID")) == 0)
            label = (const TCHAR *)val;

        if (boost::algorithm::starts_with((const TCHAR *)key, _T("_")))
            pDebug->AddSubItem(new CMFCPropertyGridProperty((const TCHAR *)key, (_variant_t)val, (const TCHAR *)val));
        else
            pOther->AddSubItem(new CMFCPropertyGridProperty((const TCHAR *)key, (_variant_t)val, (const TCHAR *)val));
    }

    m_wndObjectCombo.AddString(label.c_str());
    m_wndObjectCombo.SetCurSel(0);

    m_wndPropList.AddProperty(pOther);
    m_wndPropList.AddProperty(pDebug);
}

int CDockablePropertiesView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CDockablePane::OnCreate(lpCreateStruct) == -1)
        return -1;

    CRect rectDummy;
    rectDummy.SetRectEmpty();

    // Create combo:
    const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST
        | WS_BORDER | CBS_SORT | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

    if (!m_wndObjectCombo.Create(dwViewStyle, rectDummy, this, 1))
    {
        TRACE0("Failed to create Properies Combo \n");
        return -1;      // fail to create
    }
    m_wndObjectCombo.SetFont(CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT)));

    if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
    {
        TRACE0("Failed to create Properies Grid \n");
        return -1;      // fail to create
    }
    m_wndPropList.EnableHeaderCtrl(FALSE);
    m_wndPropList.EnableDescriptionArea();
    m_wndPropList.SetVSDotNetLook();
    m_wndPropList.MarkModifiedProperties();

    m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PROPERTIES);
    m_wndToolBar.LoadToolBar(IDR_PROPERTIES, 0, 0, TRUE /* Is locked */);

    OnChangeVisualStyle();

    m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
    m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
    m_wndToolBar.SetOwner(this);

    // All commands will be routed via this control , not via the parent frame:
    m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

    AdjustLayout();
    return 0;
}

void CDockablePropertiesView::OnSize(UINT nType, int cx, int cy)
{
    CDockablePane::OnSize(nType, cx, cy);
    AdjustLayout();
}

void CDockablePropertiesView::OnSortingprop()
{
    m_wndPropList.SetAlphabeticMode();
}

void CDockablePropertiesView::OnUpdateSortingprop(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_wndPropList.IsAlphabeticMode());
}

void CDockablePropertiesView::OnExpand()
{
    m_wndPropList.SetAlphabeticMode(FALSE);
}

void CDockablePropertiesView::OnUpdateExpand(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(!m_wndPropList.IsAlphabeticMode());
}

void CDockablePropertiesView::OnProperies1()
{
    // TODO: Add your command handler code here
}

void CDockablePropertiesView::OnUpdateProperies1(CCmdUI* /*pCmdUI*/)
{
    // TODO: Add your command update UI handler code here
}

void CDockablePropertiesView::OnProperies2()
{
    // TODO: Add your command handler code here
}

void CDockablePropertiesView::OnUpdateProperies2(CCmdUI* /*pCmdUI*/)
{
    // TODO: Add your command update UI handler code here
}


void CDockablePropertiesView::OnSetFocus(CWnd* pOldWnd)
{
    CDockablePane::OnSetFocus(pOldWnd);

    m_wndPropList.SetFocus();
}

void CDockablePropertiesView::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
    CDockablePane::OnSettingChange(uFlags, lpszSection);
    SetPropListFont();
}

void CDockablePropertiesView::SetPropListFont()
{
    //::DeleteObject (m_fntPropList.Detach ());

    //LOGFONT lf;
    //afxGlobalData.fontRegular.GetLogFont (&lf);

    //NONCLIENTMETRICS info;
    //info.cbSize = sizeof(info);

    //afxGlobalData.GetNonClientMetrics (info);

    //lf.lfHeight = info.lfMenuFont.lfHeight;
    //lf.lfWeight = info.lfMenuFont.lfWeight;
    //lf.lfItalic = info.lfMenuFont.lfItalic;

    //m_fntPropList.CreateFontIndirect (&lf);

    //m_wndPropList.SetFont(&m_fntPropList);
}

void CDockablePropertiesView::OnChangeVisualStyle()
{
    m_wndToolBar.CleanUpLockedImages();
    //m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_PROP24 : IDR_PROPERTIES, 0, 0, TRUE /* Locked */);
}
