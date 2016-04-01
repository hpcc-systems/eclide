#include "StdAfx.h"
#include "..\en_us\resource.h"

#include "DebugGraphView.h"
#include "Notifications.h"
#include "thread.h"
#include "mainfrm.h"

// CDebugGraphView
void CreateTooltip(IGraphVertex * vertex, CComQIPtr<IGraphVertex> globalVertex)
{
    CComPtr<CStringStringMap> attrs = new CStringStringMap();
    if (globalVertex)
    {
        CComQIPtr<CStringStringMap> g_attrs = globalVertex->GetPropertyUnknown(PROP_ATTRIBUTES);
        attrs->m_attr = g_attrs->m_attr;
        attrs->m_attr[_T("Global Label")] = globalVertex->GetLabel();

    }
    CComQIPtr<CStringStringMap> l_attrs = vertex->GetPropertyUnknown(PROP_ATTRIBUTES);
    attrs->m_attr.insert(l_attrs->m_attr.begin(), l_attrs->m_attr.end());
    std::_tstring tooltip;
    vertex->SetProperty(PROP_TOOLTIP, CreateTooltip(vertex, attrs, tooltip));
}

void CreateTooltip(IGraphEdge * edge, CComQIPtr<IGraphEdge> globalEdge)
{
    CComPtr<CStringStringMap> attrs = new CStringStringMap();
    if (globalEdge)
    {
        CComQIPtr<CStringStringMap> g_attrs = globalEdge->GetPropertyUnknown(PROP_ATTRIBUTES);
        attrs->m_attr = g_attrs->m_attr;
    }
    CComQIPtr<CStringStringMap> l_attrs = edge->GetPropertyUnknown(PROP_ATTRIBUTES);
    attrs->m_attr.insert(l_attrs->m_attr.begin(), l_attrs->m_attr.end());

    std::_tstring tooltip;
    edge->SetProperty(PROP_TOOLTIP, CreateTooltip(edge, attrs, tooltip));
}

CDebugGraphView::CDebugGraphView()
{
    m_xgmml = _T("");
    m_redundantSubgraphs = false;
    m_passthroughVertices = false;
    m_viewRunningOnly = false;
    m_viewFoundOnly = false;
    m_viewGlobal = NULL;
}

CDebugGraphView::~CDebugGraphView()
{
}

void CDebugGraphView::SetOwner(IGraphViewSlot * owner, SGV::CGraphViewCtrl * viewGlobal)
{
    m_owner = owner;
    m_viewGlobal = viewGlobal;
}

//void CDebugGraphView::DoMinClicked(bool expand)
//{
//	CUniqueIDVector subgraphs;
//	m_view.GetSubgraphs(&subgraphs);
//	subgraphs.Reset();
//	while(subgraphs.MoveNext())
//	{
//		SGV::CGraphSubgraph subgraph(m_view, subgraphs.Get());
//		subgraph.SetMinimized(!expand);
//	}
//	CalcVisibility();
//	m_inLayout = true;
//	EnableWindow(false);
//	m_view.SetLayout(SGV::LAYOUT_DOT);
//}

void PrepFound(IGraphEdge * edge, const roxie::CSearchMap & sm)
{
    if (!edge)
        return;
    CComVariant propFound;
    edge->GetProperty(PROP_FOUND, propFound);
    if (propFound.vt != VT_BOOL)
        propFound = false;
    CComVariant found = (sm.find(edge->GetID()) != sm.end());
    if (found != propFound)
        edge->PropagateProperty(PROP_FOUND, found);
}

VISIBLE CalcItemVisibility(IGraphItem * item, PROP prop, bool filter, VISIBLE defaultState, const CUniqueID & activeItem)
{
    VISIBLE retVal = defaultState;
    if (filter && item->GetID() != activeItem)
    {
        CComVariant val;
        item->GetProperty(prop, val);
        if (val.vt != VT_BOOL || !val.boolVal)
            retVal = VISIBLE_HIDE;
    }
    return retVal;
}

bool CDebugGraphView::CalcVisibility(const CUniqueID & activeItem)
{
    if (!IsWindowEnabled())
        return false;
    bool modified = m_view.EnsureVisible(activeItem);
    const roxie::CSearchMap & sm = m_owner->GetSearchResults();
    CUniqueIDVector edges;
    m_view.GetEdges(&edges, TRISTATE_BOTH, TRISTATE_BOTH, true);
    edges.Reset();
    while(edges.MoveNext())
    {
        IGraphEdge * edge = m_view.GetEdge(edges.Get());
        ::PrepFound(edge, sm);
    }

    CUniqueIDVector vertices;
    m_view.GetVertices(&vertices);
    vertices.Reset();
    while(vertices.MoveNext())
    {
        IGraphVertex * vertex = m_view.GetVertex(vertices.Get());
        if (vertex)
        {
            VISIBLE was = vertex->GetVisible();
            VISIBLE willBe = CalcItemVisibility(vertex, PROP_ROXIESTARTED, m_viewRunningOnly, VISIBLE_DEFAULT, activeItem);
            willBe = CalcItemVisibility(vertex, PROP_FOUND, m_viewFoundOnly, willBe, activeItem);
            if (was != willBe)
            {
                vertex->SetVisible(willBe);
                modified = true;
            }
        }
    }

    CUniqueIDVector subgraphs;
    m_view.GetSubgraphs(&subgraphs);
    subgraphs.Reset();
    while(subgraphs.MoveNext())
    {
        IGraphSubgraph * sg= m_view.GetSubgraph(subgraphs.Get());
        if (sg)
        {
            VISIBLE was = sg->GetVisible();
            VISIBLE willBe = CalcItemVisibility(sg, PROP_ROXIESTARTED, m_viewRunningOnly, VISIBLE_DEFAULT, activeItem);
            willBe = CalcItemVisibility(sg, PROP_FOUND, m_viewFoundOnly, willBe, activeItem);
            if (was != willBe)
            {
                sg->SetVisible(willBe);
                modified = true;
            }
        }
    }
    return modified;
}

void CDebugGraphView::CalcVisibilityAndLayout(const CUniqueID & activeItem, bool forceLayout)
{
    bool modified = CalcVisibility(activeItem);
    if (modified || forceLayout)
    {
        m_view.SetMessage(_T("Layout..."));
        EnableWindow(false);
        m_view.SetLayout(SGV::LAYOUT_DOT);
    }
}

void CDebugGraphView::UpdateLabels()
{
    if (m_viewGlobal)
    {
        CUniqueIDVector vertices;
        m_view.GetVertices(&vertices, TRISTATE_BOTH, TRISTATE_BOTH, TRISTATE_BOTH, true);
        while(vertices.MoveNext())
        {
            CUniqueID id = vertices.Get();
            IGraphVertex * v = m_view.GetVertex(id);
            IGraphVertex * vg = m_viewGlobal->GetVertex(m_owner->GetGlobalID(id));
            if (v && vg)
                v->SetLabel(vg->GetLabel());
        }
    }
}

void CDebugGraphView::DoMinClicked(const CUniqueID & id, bool expand)
{
    SGV::CGraphSubgraph subgraph(m_view, id);
    subgraph.SetMinimized(!expand);
    m_centerOnItem = id;
    CalcVisibility();
    m_view.SetMessage(_T("Layout..."));
    EnableWindow(false);
    m_view.SetLayout(SGV::LAYOUT_DOT);
}

bool CDebugGraphView::LoadXGMML(const std::_tstring & xgmml, IXGMMLRenderCallback * callback, const CUniqueID & centerOnItem, bool redundantSubgraphs, bool passthroughVertices)
{
    ATLASSERT(callback);
    bool minLarge = true;
    bool retVal = true;
    if (xgmml.size() == 0)
    {
        m_view.SetMessage(_T("No Graph"));
        Clear();
    }
    else if (IsWindowEnabled() && (!boost::algorithm::equals(m_xgmml, xgmml) || m_redundantSubgraphs != redundantSubgraphs || m_passthroughVertices != passthroughVertices))
    {
        Clear();
        m_xgmml = xgmml;
        m_redundantSubgraphs = redundantSubgraphs;
        m_passthroughVertices = passthroughVertices;
        m_view.SetMessage(_T("Loading..."));
        m_view.LoadXGMML(xgmml.c_str(), WUVisualState_running, redundantSubgraphs, passthroughVertices, true, callback);
        m_centerOnItem = centerOnItem;
        UpdateLabels();
        CalcVisibility(centerOnItem);
        m_view.SetMessage(_T("Layout..."));
        EnableWindow(false);
        m_view.SetLayout(SGV::LAYOUT_DOT);
    }
    return retVal;
}

bool CDebugGraphView::MergeXGMML(const std::_tstring & xgmml, IXGMMLRenderCallback * callback, const CUniqueID & centerOnItem, bool appendMissing, bool redundantSubgraphs, bool passthroughVertices)
{
    if (!m_xgmml.length() || !xgmml.length())
        return false;
    m_view.SetMessage(_T("Merging..."));
    m_view.MergeXGMML(xgmml.c_str(), WUVisualState_unknown, appendMissing, redundantSubgraphs, passthroughVertices, callback);
    UpdateLabels();
    m_view.SetMessage(_T(""));
    if (CalcVisibility(centerOnItem))
    {
        m_centerOnItem = centerOnItem;
        m_view.SetMessage(_T("Layout..."));
        EnableWindow(false);
        m_view.SetLayout(SGV::LAYOUT_DOT);
    }
    else if (!centerOnItem.isEmpty())
        m_view.CenterGraphItem(centerOnItem);
    else
        m_view.InvalidateLayout();
    return true;
}

void CDebugGraphView::Clear()
{
    m_xgmml = _T("");
    m_redundantSubgraphs = false;
    m_passthroughVertices = false;
    m_view.Clear();
}

void CDebugGraphView::EnsureVisible(const CUniqueID & id, bool centerOnItem, bool selectItem, bool appendSelection)
{
    if (CalcVisibility(id))
    {
        if (centerOnItem)
            m_centerOnItem = id;
        m_view.SetMessage(_T("Layout..."));
        EnableWindow(false);
        m_view.SetLayout(SGV::LAYOUT_DOT);
    }
    else if (centerOnItem)
    {
        m_view.CenterGraphItem(id);
    }
    if (selectItem)
        m_view.SetSelected(id, true, appendSelection);
}

LRESULT CDebugGraphView::OnInitDialog(HWND /*hWnd*/, LPARAM /*lParam*/)
{
    SGV::Register();
    SetMsgHandled(false);
    DlgResize_Init(false, false, 0);
    SetMsgHandled(False);

    CRect rc(0, 0, 0, 0);
    CWindow wndPlaceholder = GetDlgItem(IDC_STATIC_PLACEHOLDER);
    wndPlaceholder.GetWindowRect(rc);
    ScreenToClient(rc);
    wndPlaceholder.DestroyWindow();
    m_view.Create(m_hWnd, rc, NULL, WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE | WS_TABSTOP, WS_EX_CLIENTEDGE);
    m_view.SetDlgCtrlID(IDC_STATIC_PLACEHOLDER);
    m_view.SetAppMode(AppModeQBGraph);

    return 0;
}

void CDebugGraphView::GetSelectedEdges(IUniqueIDContainer * results)
{
    m_view.GetEdges(results, TRISTATE_BOTH, TRISTATE_TRUE);
}

void CDebugGraphView::SetSourceTargetSelected(const CUniqueID & edgeID)
{
    if (IGraphEdge * e = m_view.GetEdge(edgeID))
    {
        m_view.SetSelected(e->GetSource()->GetID(), true, false);
        m_view.SetSelected(e->GetTarget()->GetID(), true, true);
    }
}

void CDebugGraphView::SetZoom(int zoom)
{
    m_view.SetScale((float)zoom / 100.0f);
}

int CDebugGraphView::GetZoom()
{
    return (int)(m_view.GetScale() * 100.0f);
}

int CDebugGraphView::GetLargestVertexIDInt()
{
    int largestInt = 0;
    CUniqueIDVector vertices;
    m_view.GetVertices(&vertices);
    while(vertices.MoveNext())
    {
        CUniqueID id = vertices.Get();
        try
        {
            int itrInt = boost::lexical_cast<int>(id.GetID());
            if (itrInt > largestInt)
                largestInt = itrInt;
        }
        catch (boost::bad_lexical_cast &)
        {
        }
    }
    return largestInt;
}

BOOL CDebugGraphView::PreTranslateMessage(MSG* pMsg)
{
    return baseClass::IsDialogMessage(pMsg);
}

LRESULT CDebugGraphView::OnNotifyMinClicked(int nId, LPNMHDR pNMHDR, BOOL& bHandled)
{
    LPNMSGVHDR notification = (LPNMSGVHDR)pNMHDR;
    CComPtr<IGraphSubgraph> subgraph = m_view.GetSubgraph(notification->id);
    if (subgraph)
    {
        SGV::CGraphSubgraph sg(m_view, subgraph->GetID());
        DoMinClicked(notification->id, sg.GetMinimized());
    }
    return 0;
}

LRESULT CDebugGraphView::OnLayoutFinished(int nId, LPNMHDR nmhdr, BOOL& bHandled)
{
    EnableWindow(true);
    m_view.SetMessage(_T(""));
    if (m_centerOnItem.isEmpty())
        m_view.CenterGraph();
    else
        m_view.CenterGraphItem(m_centerOnItem);
    m_centerOnItem = CUniqueID();
    m_view.InvalidateLayout();
    return 	1;
}

LRESULT CDebugGraphView::OnGraphContext(int nId, LPNMHDR pNMHDR, BOOL& bHandled)
{
    LPNMCONTEXTMENU notification = (LPNMCONTEXTMENU)pNMHDR;
    CPoint pt(notification->pt);
    m_view.ClientToScreen(&pt);
    CUniqueIDVector subgraphs;
    m_view.GetSubgraphs(&subgraphs);
    bool restore = true;
    while(subgraphs.MoveNext())
    {
        SGV::CGraphSubgraph subgraph(m_view, subgraphs.Get());
        if (restore && subgraph.GetMinimized() == false)
        {
            restore = false;
            break;
        }
    }
    WTL::CMenu m;
    m.LoadMenu(IDR_RD_POPUP_GRAPH_GRAPH);
    if (restore)
    {
        WTL::CMenuItemInfo mii;
        mii.fMask = MIIM_STRING;
        mii.dwTypeData = _T("&Restore All");
        m.SetMenuItemInfo(ID_GRAPH_MINIMIZE, false, &mii);
    }
    switch (m_owner->ProcessContextMenu(m, pt, subgraphs))
    {
    case ID_GRAPH_ZOOMTOFIT:
        {
            float f = m_view.SetScaleToFit();
            m_owner->ScaleChanged(f * 100);
        }
        break;
    case ID_GRAPH_MINIMIZE:
        {
            subgraphs.Reset();
            while(subgraphs.MoveNext())
            {
                SGV::CGraphSubgraph subgraph(m_view, subgraphs.Get());
                subgraph.SetMinimized(!restore);
            }
            CalcVisibility();
            m_view.SetMessage(_T("Layout..."));
            EnableWindow(false);
            m_view.SetLayout(SGV::LAYOUT_DOT);
        }
        break;
    }
    return 0;
}

LRESULT CDebugGraphView::OnSubgraphContext(int nId, LPNMHDR pNMHDR, BOOL& bHandled)
{
    LPNMCONTEXTMENU notification = (LPNMCONTEXTMENU)pNMHDR;
    CPoint pt(notification->pt);
    m_view.ClientToScreen(&pt);
    SGV::CGraphSubgraph subgraph(m_view, notification->id);
    m_notificationItem = notification->id;
    WTL::CMenu m;
    m.LoadMenu(IDR_RD_POPUP_GRAPH_SUBGRAPH);
    if (subgraph.GetMinimized())
    {
        WTL::CMenuItemInfo mii;
        mii.fMask = MIIM_STRING;
        mii.dwTypeData = _T("&Restore");
        m.SetMenuItemInfo(ID_GRAPH_MINIMIZE, false, &mii);
        m.EnableMenuItem(ID_GRAPH_RESTORESINGLE, MF_GRAYED);
    }
    switch (m_owner->ProcessContextMenu(m, pt, m_view.GetSubgraph(notification->id)))
    {
    case ID_GRAPH_ZOOMTOFIT:
        {
            float f = m_view.SetItemScaleToFit(notification->id);
            m_owner->ScaleChanged(f * 100);
        }
        break;
    case ID_GRAPH_MINIMIZE:
        {
            subgraph.SetMinimized(!subgraph.GetMinimized());
            CalcVisibility(subgraph.GetID());
            m_view.SetMessage(_T("Layout..."));
            EnableWindow(false);
            m_view.SetLayout(SGV::LAYOUT_DOT);
        }
        break;
    case ID_GRAPH_RESTORESINGLE:
        {
            CUniqueIDVector subgraphs;
            subgraph.GetChildren(&subgraphs);
            subgraphs.Reset();
            while(subgraphs.MoveNext())
            {
                CUniqueID id = subgraphs.Get();
                if (id.GetCategory().GetCategory() == XGMML_CAT_SUBGRAPH)
                {
                    SGV::CGraphSubgraph subgraph(m_view, id);
                    subgraph.SetMinimized(false);
                }
            }
            CalcVisibility();
            m_view.SetMessage(_T("Layout..."));
            EnableWindow(false);
            m_view.SetLayout(SGV::LAYOUT_DOT);
        }
        break;
    }
    return 0;
}

LRESULT CDebugGraphView::OnVertexContext(int nId, LPNMHDR pNMHDR, BOOL& bHandled)
{
    LPNMCONTEXTMENU notification = (LPNMCONTEXTMENU)pNMHDR;
    SGV::CGraphVertex vertex(m_view.GetHWND(), notification->id);
    CPoint pt(notification->pt);
    m_view.ClientToScreen(&pt);
    WTL::CMenu m;
    m.LoadMenu(IDR_RD_POPUP_GRAPH_VERTEX);
    switch (m_owner->ProcessContextMenu(m, pt, vertex))
    {
        case ID_GRAPH_ZOOMTOFIT:
            {
                float f = m_view.SetItemScaleToFit(notification->id);
                m_owner->ScaleChanged(f * 100);
            }
            break;
    }
    return 0;
}

LRESULT CDebugGraphView::OnEdgeContext(int nId, LPNMHDR pNMHDR, BOOL& bHandled)
{
    LPNMCONTEXTMENU notification = (LPNMCONTEXTMENU)pNMHDR;
    SGV::CGraphEdge edge(m_view.GetHWND(), notification->id);
    CPoint pt(notification->pt);
    m_view.ClientToScreen(&pt);
    WTL::CMenu m;
    m.LoadMenu(IDR_RD_POPUP_GRAPH_EDGE);
    switch (m_owner->ProcessContextMenu(m, pt, edge))
    {
    case ID_GRAPH_ZOOMTOFIT:
        {
            float f = m_view.SetItemScaleToFit(notification->id);
            m_owner->ScaleChanged(f * 100);
            CUniqueID sid, tid;
            m_view.SetSelected(edge.GetSource(sid), true, false);
            m_view.SetSelected(edge.GetTarget(tid), true, true);
        }
        break;
    case ID_GRAPH_GOTOSOURCE:
        {
            CUniqueID id;
            m_view.CenterVertex(edge.GetSource(id));
            m_view.SetSelected(id, true, false);
        }
        break;
    case ID_GRAPH_GOTOTARGET:
        {
            CUniqueID id;
            m_view.CenterVertex(edge.GetTarget(id));
            m_view.SetSelected(id, true, false);
        }
        break;
    }
    return 0;
}

LRESULT CDebugGraphView::OnSelChanged(int nId, LPNMHDR pNMHDR, BOOL& bHandled)
{
    LPNMSELECTIONCHANGE notification = (LPNMSELECTIONCHANGE)pNMHDR;
    if (m_owner)
        m_owner->NewSelection(this, notification->idSelect, false, false);
    return 0;
}

LRESULT CDebugGraphView::OnScaleChanged(int nId, LPNMHDR nmhdr, BOOL& bHandled)
{
    m_owner->ScaleChanged(m_view.GetScale() * 100);
    return 0;
}

LRESULT CDebugGraphView::OnCalcTooltip(int nId, LPNMHDR pNMHDR, BOOL& bHandled)
{
    LPNMNOTIFYCALCTOOLTIP notification = (LPNMNOTIFYCALCTOOLTIP)pNMHDR;
    notification->supressTooltip = true;
    //if (CComQIPtr<IGraphVertex> v = m_view.GetGraphItem(notification->id))
    //	CreateTooltip(v, m_viewGlobal ? m_viewGlobal->GetGraphItem(notification->id) : NULL);
    //else if (CComQIPtr<IGraphEdge> e = m_view.GetGraphItem(notification->id))
    //	CreateTooltip(e, m_viewGlobal ? m_viewGlobal->GetGraphItem(notification->id) : NULL);
    //if (CComPtr<IGraphItem> item = m_view.GetGraphItem(notification->id))
    //	item->SetProperty(PROP_TOOLTIP, _T(""));
    return 0;
}

BOOL CDebugGraphView::OnEraseBkgnd(WTL::CDCHandle dc)
{
    return 1;
}

void CDebugGraphView::DoEditFind()
{
    //m_searchInfo.SetPosition(-1);
    FindReplaceOnEditFind(_T(""), false, true);
}

void CDebugGraphView::DoEditFindAgain(bool bNext)
{
    if(GetFindReplace().m_szFindWhat[0] == 0)
    {
        DoEditFind();
        return;
    }

    if (m_prevFind.CompareNoCase(GetFindReplace().m_szFindWhat) || 
        m_prevMatchCase != GetFindReplace().MatchCase() || 
        m_prevWholeWord != GetFindReplace().MatchWholeWord() ||
        false)//m_prevSearchTooltips != m_checkSearchTooltips.GetCheck())
    {
        m_found.clear();
        m_prevFind = GetFindReplace().m_szFindWhat;
        m_prevMatchCase = GetFindReplace().MatchCase();
        m_prevWholeWord = GetFindReplace().MatchWholeWord();
        m_prevSearchTooltips = true;//m_checkSearchTooltips.GetCheck();
        m_view.Find(&m_found, GetFindReplace().m_szFindWhat, true, true);//m_checkSearchTooltips.GetCheck());
        m_found.Reset();
    }
    if (bNext ? m_found.MoveNext() : m_found.MovePrevious())
    {
        CUniqueID id = m_found.Get();
        SGV::CGraphVertex v(m_view, id);
        if (v.EnsureVisible())
        {
            m_notificationItem = id;
            m_actionID = ID_GRAPH_MINIMIZE;
            m_view.SetMessage(_T("Layout 7..."));
            m_view.SetLayout(SGV::LAYOUT_DOT);
        }
        m_view.SetSelected(id, true, false);
        m_view.CenterVertex(id, 1.0f);
    }
    else
    {
        MessageBeep(MB_ICONASTERISK);
        m_found.Reset();
    }
}