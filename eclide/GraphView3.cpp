#include "StdAfx.h"
#include "..\en_us\resource.h"

#include "GraphView3.h"
#include "Notifications.h"
#include "thread.h"
#include "mainfrm.h"
#include "XGMMLWorkunit.h"
#include "GotoDlg.h"
#include "DefinitionParser.h"
#include "XGMMLSaxHandler.h"

#define PROP_TREEITEM 11
enum GV_TIMER
{
    GV_TIMER_UNKNOWN = 0,
    GV_TIMER_REFRESH,
    GV_TIMER_LAST
};

CGraphView3::CGraphView3(Dali::IWorkunit * wu, IResultSlot *resultSlot) : m_wu(wu)
{
    m_resultSlot = resultSlot;
    m_prevMatchCase = FALSE;
    m_prevWholeWord = FALSE;
    m_prevSearchTooltips = FALSE;
    m_loaded = false;
    m_buildingTree = false;
    m_buildingEdgeVisibility = false;
    m_followActive = false;
}

void threadRefreshGraph(CGraphView3 * self, CComPtr<Dali::IGraph> graph)
{
    if (graph && (!graph->IsLoaded() || graph->IsRunning()))
        graph->Refresh();
}

void threadRefreshTimings(CGraphView3 * self, CComPtr<Dali::IWorkunit> workunit)
{
workunit->RefreshGraph();
    workunit->RefreshGraphTiming();
    self->operator()(Dali::WUEventUpdated, workunit);
}


BOOL CGraphView3::PreTranslateMessage(MSG* pMsg)
{
    // This was stolen from an SDI app using a form view.
    //
    // Pass keyboard messages along to the child window that has the focus.
    // When the browser has the focus, the message is passed to its containing
    // CAxWindow, which lets the control handle the message.
    if((pMsg->message == WM_MOUSEWHEEL))
    {
        int d = 0;
    }
    if((pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) &&
        (pMsg->message < WM_MOUSEFIRST || pMsg->message > WM_MOUSELAST))
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

    // A normal control has the focus, so call IsDialogMessage() so that
    // the dialog shortcut keys work (TAB, etc.)
    return IsDialogMessage(pMsg);
}

void CGraphView3::LoadGraph()
{
    if (IsWindow())
    {
        if (m_graph != GetSelectedGraph())
        {
            if (m_graph)
                m_graphConnection.disconnect();

            m_graph = GetSelectedGraph();
            if (m_graph)
            {
                m_graphConnection = m_graph->on_refresh_connect(boost::ref(*this));
                m_wndLNGVC.SetMessage(_T("Loading..."));
                m_wndLNGVC.Clear();
                if (!m_graph->IsLoaded())
                    clib::thread run(__FUNCTION__, boost::bind(&threadRefreshGraph, this, m_graph));
                else
                    operator()(m_graph);
            }
        }
        else if (m_graph)	//Need to merge
        {
            operator()(m_graph);
        }
    }
}

bool CGraphView3::DoFileOpen(const CString & path)
{
    StlLinked<Dali::IDali> dali = Dali::AttachDali();
    m_wu = dali->GetXGMMLWorkunit(path);
    return true;
}

void CGraphView3::Clear()
{
    m_buildingTree = true;
    m_listGraph.DeleteAllItems();
    m_listTimings.DeleteAllItems();
    m_listAttributes.DeleteAllItems();
    m_wndLNGVC.Clear();
    m_loaded = false;
    m_buildingTree = false;
}

void buildTree3(WTL::CTreeViewCtrlEx & tree, HTREEITEM parent, IGraphItem * item, int depth = 0)
{
    if (!item)
        return;

    if (CComQIPtr<IGraphSubgraph> subgraph = item)
    {
        bool first = true;
        IGraphItemVector children;
        subgraph->GetChildren(children);
        for(IGraphItemVector::iterator itr = children.begin(); itr != children.end(); ++itr)
        {
            IGraphItem * graphItem = itr->get();
            WTL::CTreeItem treeItem = tree.InsertItem(graphItem->GetLabel(), parent, TVI_LAST);
            treeItem.SetData((DWORD_PTR)graphItem);
            //if (depth == 0 && first == false)
            //	tree.SetCheckState(treeItem, false);
            //else
            //	tree.SetCheckState(treeItem, true);
            graphItem->SetProperty(PROP_TREEITEM, (int)(HTREEITEM)treeItem);
            buildTree3(tree, treeItem, itr->get(), depth + 1);
            first = false;
        }
    }
}

void buildList3(WTL::CSortListViewCtrl & list, const Dali::IWorkunit * wu)
{
    list.DeleteAllItems();
    StdStringVector graphNames;
    wu->GetGraphNames(graphNames);
    int row = 0;
    for(StdStringVector::const_iterator itr = graphNames.begin(); itr != graphNames.end(); ++itr)
    {
        CComPtr<Dali::IGraph> graph = wu->GetGraph(*itr);
        list.InsertItem(row, boost::lexical_cast<std::_tstring>(graph->GetSequence()).c_str());
        list.SetItemText(row, 1, itr->c_str());
        list.SetItemText(row, 2, graph->GetStateString());
        list.SetItemData(row, (DWORD_PTR)graph.p);
        ++row;
    }
    list.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
    list.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
    list.SetColumnWidth(2, LVSCW_AUTOSIZE_USEHEADER);
    list.SortItems(0);
}

void updateList3(WTL::CSortListViewCtrl & list, const Dali::IWorkunit * wu)
{
    if (list.GetItemCount() != wu->GetGraphCount())
    {
        buildList3(list, wu);
        return;
    }
    for(int i = 0; i < list.GetItemCount(); ++i)
    {
        CComPtr<Dali::IGraph> graph = (Dali::IGraph*)list.GetItemData(i);
        list.SetItemText(i, 2, graph->GetStateString());
    }
}

void buildList3Timings(WTL::CSortListViewCtrl & list, const Dali::IGraph * graph, const Dali::CGraphTimingVector & timings, bool & followActive)
{
    list.DeleteAllItems();
    if (!graph)
        return;
    int row = 0;
    for(Dali::CGraphTimingVector::const_iterator itr = timings.begin(); itr != timings.end(); ++itr)
    {
        if (graph->GetSequence() == itr->get()->m_graphNum)
        {
            list.InsertItem(row, boost::lexical_cast<std::_tstring>(itr->get()->m_gid).c_str());
            list.SetItemText(row, 1, boost::lexical_cast<std::_tstring>(itr->get()->m_minutes).c_str());
            list.SetItemText(row, 2, boost::lexical_cast<std::_tstring>(itr->get()->m_milliseconds).c_str());
            list.SetItemData(row, (DWORD_PTR)itr->get());
            ++row;
        }
    }
    list.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
    list.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
    list.SetColumnWidth(2, LVSCW_AUTOSIZE_USEHEADER);
    list.SortItems(0);
    if (followActive)
    {
        list.SelectItem(row - 1);
        followActive = true;
    }
}

void buildList3Attributes(WTL::CSortListViewCtrl & list, CGraphViewCtrl2 * graphView)
{
    list.DeleteAllItems();
    CUniqueIDVector vertices;
    graphView->GetVertices(&vertices);
    int row = 0;
    while(vertices.MoveNext())
    {
        CString def = graphView->GetProperty(vertices.Get(), _T("definition"));
        ParsedDefinition pd;
        if (DefinitionToLocation(static_cast<const TCHAR *>(def), pd))
        {
            list.InsertItem(row, pd.module.c_str());
            list.SetItemText(row, 1, pd.attribute.c_str());
            list.SetItemText(row, 2, boost::lexical_cast<std::_tstring>(pd.row).c_str());
            list.SetItemText(row, 3, boost::lexical_cast<std::_tstring>(pd.col).c_str());
            CUniqueID * id = new CUniqueID(vertices.Get());
            list.SetItemData(row, (DWORD_PTR)id);
            ++row;
        }
    }
    list.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
    list.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
    list.SetColumnWidth(2, LVSCW_AUTOSIZE_USEHEADER);
    list.SetColumnWidth(3, LVSCW_AUTOSIZE_USEHEADER);
    list.SortItems(3);
    list.SortItems(2);
    list.SortItems(1);
    list.SortItems(0);
}

bool CGraphView3::MinimizeAll(bool minimized, const CUniqueID & except)
{
    bool retVal = false;
    CUniqueIDVector subgraphs;
    m_wndLNGVC.GetSubgraphs(&subgraphs);
    subgraphs.Reset();
    while(subgraphs.MoveNext())
    {
        SGV::CGraphSubgraph subgraph(m_wndLNGVC, subgraphs.Get());
        if (subgraph.GetMinimized() != minimized && subgraphs.Get() != except)
        {
            subgraph.SetMinimized(minimized);
            retVal = true;
        }
    }
    return retVal;
}

void CGraphView3::DoMinClicked(const CUniqueID & id, bool expand)
{
    m_notificationItem = id;
    m_actionID = ID_GRAPH_MINIMIZE;
    SGV::CGraphSubgraph subgraph(m_wndLNGVC, m_notificationItem);
    subgraph.SetMinimized(!expand);
    m_wndLNGVC.SetMessage(_T("Layout 2..."));
    m_wndLNGVC.StartLayout(SGV::LAYOUT_DOT);
}

void CGraphView3::CenterOnItem(const CUniqueID & id)
{
    if (m_wndLNGVC.EnsureVisible(id))
    {
        m_notificationItem = id;
        m_actionID = ID_GRAPH_MINIMIZE;
        m_wndLNGVC.SetMessage(_T("Layout 10..."));
        m_wndLNGVC.StartLayout(SGV::LAYOUT_DOT);
    }
    else
    {
        m_wndLNGVC.CenterGraphItem(id);
    }
}

void CGraphView3::CenterOnTiming(const Dali::CGraphTiming * timing)
{
    CUniqueID id(guidDefault, XGMML_CAT_SUBGRAPH, boost::lexical_cast<std::_tstring>(m_wndLNGVC.GetItem(timing->m_gid)));
    CenterOnItem(id);
}

void CGraphView3::GetSelectedEdges(IUniqueIDContainer * results)
{
    m_wndLNGVC.GetEdges(results, TRISTATE_BOTH, TRISTATE_TRUE);
}

Dali::IGraph * CGraphView3::GetSelectedGraph()
{
    int selItem = m_listGraph.GetSelectedIndex();
    if (selItem >= 0)
        return (Dali::IGraph *)m_listGraph.GetItemData(selItem);
    return NULL;
}

void CGraphView3::SelectRunningGraph()
{
    bool followActive = m_followActive;
    for(int i = 0; i < m_listGraph.GetItemCount(); ++i)
    {
        CComPtr<Dali::IGraph> graph = (Dali::IGraph*)m_listGraph.GetItemData(i);
        if (graph->IsRunning())
        {
            m_listGraph.SelectItem(i);
            break;
        }
    }
    m_followActive = followActive;
}

void CGraphView3::SelectRecentTimingGraph()
{
    bool followActive = m_followActive;
    CUniqueID subgraphID = m_wndLNGVC.GetRunningSubgraph();
    if (!subgraphID.isEmpty())
        m_wndLNGVC.CenterGraphItem(subgraphID);
    else if (m_listTimings.GetItemCount() > 0)
        m_listTimings.SelectItem(m_listTimings.GetItemCount() - 1);
    m_followActive = followActive;
}

WUVisualState CGraphView3::DecodeState(Dali::WUState state)
{
    WUVisualState vizstate=WUVisualState_unknown;

    switch(state)
    {
    case Dali::WUStateCompiled:
    case Dali::WUStateRunning:
    case Dali::WUStateSubmitted:
    case Dali::WUStateScheduled:
    case Dali::WUStateCompiling:
    case Dali::WUStateBlocked:
    case Dali::WUStateWait:
        vizstate=WUVisualState_running;
        break;
    case Dali::WUStateCompleted:
    case Dali::WUStateArchived:
        vizstate=WUVisualState_completed;
        break;
    case Dali::WUStateFailed:
    case Dali::WUStateAborting:
    case Dali::WUStateAborted:
        vizstate=WUVisualState_failed;
        break;
    default:
        break;
    }
    return vizstate;	
}

HRESULT CGraphView3::GetFuncInfoFromId(const IID& iid, DISPID dispidMember, LCID lcid, _ATL_FUNC_INFO& info)
{
    if (InlineIsEqualGUID(iid, DIID_IFBComEventSource))
    {
        info.cc = CC_STDCALL;
        if (dispidMember == g_dispScaled)
        {
            info.vtReturn = VT_EMPTY;
            info.nParams = 2;
            info.pVarTypes[0] = VT_I4;
            info.pVarTypes[1] = VT_DISPATCH;
            return S_OK;
        }
        else if (dispidMember == g_dispLayoutFinished)
        {
            info.vtReturn = VT_EMPTY;
            info.nParams = 1;
            info.pVarTypes[0] = VT_DISPATCH;
            return S_OK;
        }
        else if (dispidMember == g_dispMouseDoubleClick)
        {
            info.vtReturn = VT_I4;
            info.nParams = 3;
            info.pVarTypes[0] = VT_I4;
            info.pVarTypes[1] = VT_I4;
            info.pVarTypes[2] = VT_DISPATCH;
            return S_OK;
        }
        else if (dispidMember == g_dispSelectionChanged)
        {
            info.vtReturn = VT_EMPTY;
            info.nParams = 2;
            info.pVarTypes[0] = VT_VARIANT;
            info.pVarTypes[1] = VT_DISPATCH;
            return S_OK;
        }

        //case DISP_MOUSE_DBLCLICK:
        //	info.vtReturn = VT_I4;
        //	info.nParams = 1;
        //	info.pVarTypes[0] = VT_I4;
        //	return S_OK;

        //case DISP_UNKNOWN:
        //case DISP_UNKNOWN1:
        //case DISP_UNKNOWN2:
        //case DISP_UNKNOWN3:
        //case DISP_UNKNOWN4:
        //case DISP_UNKNOWN5:
        //case DISP_UNKNOWN6:
        //case DISP_UNKNOWN7:
        //case DISP_UNKNOWN8:
        //case DISP_UNKNOWN9:
        //	info.vtReturn = VT_I4;
        //	info.nParams = 0;
        //	return S_OK;

        else
        {
            ATLASSERT(false);
            return E_FAIL;
        }
    }
    ATLASSERT(false);
    return E_FAIL;
}

HRESULT CGraphView3::OnMouseDoubleClick(int val, int keyState, IDispatch * oSender)
{
    m_wndLNGVC.CenterGraphItem(val);
    return S_OK;
}

bool CGraphView3::InitGraphControl()
{
    ATLASSERT(::IsWindow(m_wndLNGVC));
    CComPtr<IAxWinHostWindow> spAxWindow;
    HRESULT Hr = m_wndLNGVC.QueryHost(&spAxWindow);
    if(FAILED(Hr)) 
        return false;

    try
    {
        Hr = spAxWindow->CreateControl(CComBSTR(GUID_npHPCCSystemsGraphViewControl), m_wndLNGVC.m_hWnd, NULL);
        if(FAILED(Hr)) 
            return false;
    }	catch (const std::exception &) {
        return false;
    }

    CComPtr<IUnknown> spUnk;
    Hr = m_wndLNGVC.QueryControl(__uuidof(IUnknown), (void**)&spUnk);
    if(FAILED(Hr)) 
        return false;

    DispEventAdvise(spUnk);
    
    //m_wndLNGVC.SetAppMode(AppModeQBGraph);
    //m_wndLNGVC.SetDlgCtrlID(IDC_STATIC_GRAPH);
    //m_wndLNGVC.SetAppZoomMode(AZMMouseWheel);
    return true;
}

LRESULT CGraphView3::OnInitDialog(HWND /*hWnd*/, LPARAM /*lParam*/)
{
    m_loaded = false;
    m_mergeCount=0;
    SGV::Register();
    SetMsgHandled(false);
    DlgResize_Init(false, false, 0);
    m_name = _T("Graphs");
    SetWindowText(m_name);
    ModifyStyle(0, WS_CLIPCHILDREN);

    CRect rc(0, 0, 0, 0);
    CWindow wndPlaceholder = GetDlgItem(IDC_STATIC_PLACEHOLDER);
    wndPlaceholder.GetWindowRect(rc);
    ScreenToClient(rc);
    wndPlaceholder.DestroyWindow();
    m_splitterV.Create(m_hWnd, rc);
    m_splitterV.SetDlgCtrlID(IDC_STATIC_PLACEHOLDER);

    m_splitterH.Create(m_splitterV);

    m_listGraph.Create(m_splitterH, rcDefault, NULL, WS_CHILD | WS_VISIBLE | LVS_SINGLESEL | LVS_REPORT | LVS_SHOWSELALWAYS, WS_EX_CLIENTEDGE);
    m_listGraph.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
    m_listGraph.InsertColumn(0, _T("ID"));
    m_listGraph.InsertColumn(1, _T("Name"));
    m_listGraph.InsertColumn(2, _T("State"));
    m_listGraph.SetColumnSortType(0, WTL::LVCOLSORT_LONG);
    m_listGraph.SetColumnSortType(1, WTL::LVCOLSORT_TEXTNOCASE);
    m_listGraph.SetColumnSortType(2, WTL::LVCOLSORT_TEXTNOCASE);
    m_listGraph.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
    m_listGraph.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
    m_listGraph.SetColumnWidth(2, LVSCW_AUTOSIZE_USEHEADER);

    m_splitterH2.Create(m_splitterH);

    m_listTimings.Create(m_splitterH2, rcDefault, NULL, WS_CHILD | WS_VISIBLE | LVS_SINGLESEL | LVS_REPORT | LVS_SHOWSELALWAYS, WS_EX_CLIENTEDGE);
    m_listTimings.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
    m_listTimings.InsertColumn(0, _T("ID"));
    m_listTimings.InsertColumn(1, _T("Minutes"));
    m_listTimings.InsertColumn(2, _T("Milliseconds"));
    m_listTimings.SetColumnSortType(0, WTL::LVCOLSORT_LONG);
    m_listTimings.SetColumnSortType(1, WTL::LVCOLSORT_LONG);
    m_listTimings.SetColumnSortType(2, WTL::LVCOLSORT_LONG);
    m_listTimings.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
    m_listTimings.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
    m_listTimings.SetColumnWidth(2, LVSCW_AUTOSIZE_USEHEADER);

    m_listAttributes.Create(m_splitterH2, rcDefault, NULL, WS_CHILD | WS_VISIBLE | LVS_SINGLESEL | LVS_REPORT | LVS_SHOWSELALWAYS, WS_EX_CLIENTEDGE);
    m_listAttributes.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
    m_listAttributes.InsertColumn(0, _T("Folder"));
    m_listAttributes.InsertColumn(1, _T("File"));
    m_listAttributes.InsertColumn(2, _T("Row"));
    m_listAttributes.InsertColumn(3, _T("Column"));
    m_listAttributes.SetColumnSortType(0, WTL::LVCOLSORT_TEXTNOCASE);
    m_listAttributes.SetColumnSortType(1, WTL::LVCOLSORT_TEXTNOCASE);
    m_listAttributes.SetColumnSortType(2, WTL::LVCOLSORT_LONG);
    m_listAttributes.SetColumnSortType(3, WTL::LVCOLSORT_LONG);
    m_listAttributes.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
    m_listAttributes.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
    m_listAttributes.SetColumnWidth(2, LVSCW_AUTOSIZE_USEHEADER);
    m_listAttributes.SetColumnWidth(3, LVSCW_AUTOSIZE_USEHEADER);

    m_wndLNGVC.Create(m_splitterV, rcDefault, NULL, WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE | WS_TABSTOP, NULL);
    if (!InitGraphControl())
    {
        if (RegisterMyVersion())
            InitGraphControl();
    }

    m_splitterV.SetSplitterPanes(m_splitterH, m_wndLNGVC);
    m_splitterV.SetSplitterPosPct(25);

    m_splitterH.SetSplitterPanes(m_listGraph, m_splitterH2);
    m_splitterH.SetSplitterPosPct(25);

    m_splitterH2.SetSplitterPanes(m_listTimings, m_listAttributes);
    m_splitterH2.SetSplitterPosPct(50);

    m_view_zoom = GetDlgItem(IDC_SLIDER_ZOOM);
    m_view_zoom.SetRange(1, 100);
    m_view_zoom.SetPageSize(10);
    m_view_zoom.SetPos(1);
    m_view_zoom.SetTic(1);
    m_view_zoom.SetTic(25);
    m_view_zoom.SetTic(50);
    m_view_zoom.SetTic(75);
    m_view_zoom.SetTic(100);

    m_timerCountdown = 0;
    m_prevTimerCountdown = 0;
    SetTimer(GV_TIMER_REFRESH, 1000);
    m_wndLNGVC.InitDispID();
    m_wndLNGVC.SetMessage(_T("No Selected Graph"));

    return 1;
}

void CGraphView3::OnDestroy()
{
    SetMsgHandled(false);
    Clear();
    
    CComPtr<IUnknown> spUnk;
    HRESULT Hr = m_wndLNGVC.QueryControl(__uuidof(IUnknown), (void**)&spUnk);
    if( FAILED(Hr) ) 
        return;

    DispEventUnadvise(spUnk);
}

LRESULT CGraphView3::OnWuUpdate(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/)
{
    if (IsWindow())
    {
        updateList3(m_listGraph, m_wu);
        if (m_followActive)
            SelectRunningGraph();
        Dali::CGraphTimingVector timings;
        m_wu->GetGraphTimings(timings);
        buildList3Timings(m_listTimings, GetSelectedGraph(), timings, m_followActive);
        LoadGraph();
    }
    return 0;
}

LRESULT CGraphView3::OnGraphUpdate(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/)
{
    CComPtr<Dali::IGraph> graph = (Dali::IGraph *)wParam;
    if (m_graph == graph)
    {
#ifdef _DEBUG
        if (static_cast<bool>(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_DEBUG_LOGRESULTXML))==true)
        {
            std::string path = "c:\\temp\\xgmml_merge";
            path += boost::lexical_cast<std::string>(m_mergeCount+1);
            path += ".xml";

            std::ofstream file(path.c_str());
            file << std::string(CT2A(graph->GetGraph(), CP_UTF8));
        }
#endif

        if (!boost::algorithm::equals(m_prevGraph, m_graph->GetGraph()))
        {
            bool needsLayout = false;
            m_prevGraph = graph->GetGraph();
            if (m_wndLNGVC.HasItems())
            {
                m_wndLNGVC.MergeXGMML(m_prevGraph.c_str(), m_graph->GetState(), false, false, false, NULL);
                CUniqueID subgraphID = m_wndLNGVC.GetRunningSubgraph();
                if (subgraphID.isEmpty())
                {
                    if (m_listTimings.GetItemCount() > 0)
                    {
                        Dali::CGraphTiming * timing = (Dali::CGraphTiming *)m_listTimings.GetItemData(m_listTimings.GetItemCount() - 1);
                        std::_tstring gid = boost::lexical_cast<std::_tstring>(timing->m_gid);
                        subgraphID = CUniqueID(guidDefault, XGMML_CAT_SUBGRAPH, gid);
                    }
                }
                needsLayout = m_wndLNGVC.EnsureVisible(subgraphID);
                if (m_followActive)
                {
                    if (needsLayout)
                        m_notificationItem = subgraphID;
                    else
                        m_wndLNGVC.CenterGraphItem(subgraphID);
                }
            }
            else 
            {
                m_wndLNGVC.SetMessage(_T("Loading..."));
                m_wndLNGVC.Clear();
                m_wndLNGVC.LoadXGMML(m_prevGraph.c_str(), m_graph->GetState(), false, false, true, NULL);
                CUniqueID subgraphID = m_wndLNGVC.GetRunningSubgraph();
                if (subgraphID.isEmpty())
                {
                    if (m_listTimings.GetItemCount() > 0)
                    {
                        Dali::CGraphTiming * timing = (Dali::CGraphTiming *)m_listTimings.GetItemData(m_listTimings.GetItemCount() - 1);
                        subgraphID = CUniqueID(guidDefault, XGMML_CAT_SUBGRAPH, boost::lexical_cast<std::_tstring>(timing->m_gid));
                    }
                }
                m_wndLNGVC.EnsureVisible(subgraphID);
                if (m_followActive)
                {
                    m_notificationItem = subgraphID;
                    m_actionID = ID_GRAPH_MINIMIZE;
                }
                needsLayout = true;
            }

            if (needsLayout)
            {
                m_wndLNGVC.SetMessage(_T("Layout..."));
                buildList3Attributes(m_listAttributes, &m_wndLNGVC);
                m_wndLNGVC.StartLayout(SGV::LAYOUT_DOT);
            }
        }
    }
    return 0;
}

LRESULT CGraphView3::OnNotifyMinClicked(int nId, LPNMHDR nmhdr, BOOL& bHandled)
{
    LPNMSGVHDR notification = (LPNMSGVHDR)nmhdr;
    CComPtr<IGraphSubgraph> subgraph = m_wndLNGVC.GetSubgraph(notification->id);
    if (subgraph)
    {
        SGV::CGraphSubgraph sg(m_wndLNGVC, subgraph->GetID());
        DoMinClicked(notification->id, sg.GetMinimized());
    }
    return 1;
}

HRESULT CGraphView3::OnLayoutFinished(IDispatch * oSender)
{
    CComVariant vaResult;
    m_wndLNGVC.SetMessage(_T(""));
    if (m_actionID == ID_GRAPH_MINIMIZE)
    {
        SGV::CGraphSubgraph subgraph(m_wndLNGVC, m_notificationItem);
        if (subgraph.GetMinimized())
            m_wndLNGVC.CenterGraphItem(m_notificationItem);
        else
            m_wndLNGVC.SetItemScaleToFit(m_notificationItem);
    }
    else
        m_wndLNGVC.CenterGraph();

    m_notificationItem = CUniqueID();
    m_actionID = 0;
    return S_OK;
}

LRESULT CGraphView3::OnGraphContext(int nId, LPNMHDR nmhdr, BOOL& bHandled)
{
    LPNMCONTEXTMENU notification = (LPNMCONTEXTMENU)nmhdr;
    CPoint pt(notification->pt);
    m_wndLNGVC.ClientToScreen(&pt);

    CUniqueIDVector subgraphs;
    m_wndLNGVC.GetSubgraphs(&subgraphs);
    bool restore = true;
    while(subgraphs.MoveNext())
    {
        SGV::CGraphSubgraph subgraph(m_wndLNGVC, subgraphs.Get());
        if (subgraph.GetMinimized() == false)
        {
            restore = false;
            break;
        }
    }

    WTL::CMenu m;
    m.LoadMenu(IDR_POPUP_GRAPH_GRAPH);
    if (restore)
    {
        WTL::CMenuItemInfo mii;
        mii.fMask = MIIM_STRING;
        mii.dwTypeData = _T("&Restore All");
        m.SetMenuItemInfo(ID_GRAPH_MINIMIZE, false, &mii);
    }
    unsigned int id = m.GetSubMenu(0).TrackPopupMenuEx(TPM_RETURNCMD, pt.x, pt.y, m_hWnd, NULL);
    switch (id)
    {
    case ID_GRAPH_SAVEAS:
        m_wndLNGVC.SaveAs();
        break;
    case ID_GRAPH_ZOOMTOFIT:
        {
            m_wndLNGVC.SetScaleToFit();
        }
        break;
    case ID_GRAPH_MINIMIZE:
        {
            subgraphs.Reset();
            while(subgraphs.MoveNext())
            {
                SGV::CGraphSubgraph subgraph(m_wndLNGVC, subgraphs.Get());
                subgraph.SetMinimized(!restore);
            }
            m_wndLNGVC.SetMessage(_T("Layout 4..."));
            m_wndLNGVC.StartLayout(SGV::LAYOUT_DOT);
        }
        break;
    }
    return 1;
}
LRESULT CGraphView3::OnSubgraphContext(int nId, LPNMHDR nmhdr, BOOL& bHandled)
{
    LPNMCONTEXTMENU notification = (LPNMCONTEXTMENU)nmhdr;
    CPoint pt(notification->pt);
    m_wndLNGVC.ClientToScreen(&pt);
    SGV::CGraphSubgraph subgraph(m_wndLNGVC, notification->id);
    m_notificationItem = notification->id;
    WTL::CMenu m;
    m.LoadMenu(IDR_POPUP_GRAPH_SUBGRAPH);
    if (subgraph.GetMinimized())
    {
        WTL::CMenuItemInfo mii;
        mii.fMask = MIIM_STRING;
        mii.dwTypeData = _T("&Restore");
        m.SetMenuItemInfo(ID_GRAPH_MINIMIZE, false, &mii);
    }
    m_actionID = m.GetSubMenu(0).TrackPopupMenuEx(TPM_RETURNCMD, pt.x, pt.y, m_hWnd, NULL);
    switch (m_actionID)
    {
    case ID_GRAPH_ZOOMTOFIT:
        m_wndLNGVC.SetItemScaleToFit(notification->id);
        break;
    case ID_GRAPH_MINIMIZE:
        {
            subgraph.SetMinimized(!subgraph.GetMinimized());
            m_wndLNGVC.SetMessage(_T("Layout 5..."));
            m_wndLNGVC.StartLayout(SGV::LAYOUT_DOT);
        }
        break;
    }
    return 1;
}
LRESULT CGraphView3::OnVertexContext(int nId, LPNMHDR nmhdr, BOOL& bHandled)
{
    LPNMCONTEXTMENU notification = (LPNMCONTEXTMENU)nmhdr;
    CPoint pt(notification->pt);
    m_wndLNGVC.ClientToScreen(&pt);
    WTL::CMenu m;
    m.LoadMenu(IDR_POPUP_GRAPH_VERTEX);
    unsigned int id = m.GetSubMenu(0).TrackPopupMenuEx(TPM_RETURNCMD, pt.x, pt.y, m_hWnd, NULL);
    switch (id)
    {
    case ID_GRAPH_COPYTOOLTIP:
        {
            if (CComQIPtr<IGraphVertex> iVertex = m_wndLNGVC.GetGraphItem(notification->id))
            {
                CComQIPtr<CStringStringMap> attrs = m_wndLNGVC.GetPropertyUnknown(notification->id, PROP_ATTRIBUTES);
                std::_tstring tooltip;
                CreateTooltip(iVertex, attrs, tooltip, true);
                SetClipboard(tooltip);
            }
        }
        break;
    case ID_GRAPH_ZOOMTOFIT:
        m_wndLNGVC.SetItemScaleToFit(notification->id);
        break;
    }
    return 1;
}
LRESULT CGraphView3::OnEdgeContext(int nId, LPNMHDR nmhdr, BOOL& bHandled)
{
    LPNMCONTEXTMENU notification = (LPNMCONTEXTMENU)nmhdr;
    SGV::CGraphEdge edge(m_wndLNGVC.GetHWND(), notification->id);
    CPoint pt(notification->pt);
    m_wndLNGVC.ClientToScreen(&pt);
    WTL::CMenu m;
    m.LoadMenu(IDR_POPUP_GRAPH_EDGE);
    unsigned int id = m.GetSubMenu(0).TrackPopupMenuEx(TPM_RETURNCMD, pt.x, pt.y, m_hWnd, NULL);
    switch (id)
    {
    case ID_GRAPH_COPYTOOLTIP:
        {
            if (CComQIPtr<IGraphEdge> iEdge = m_wndLNGVC.GetGraphItem(notification->id))
            {
                CComQIPtr<CStringStringMap> attrs = m_wndLNGVC.GetPropertyUnknown(notification->id, PROP_ATTRIBUTES);
                std::_tstring tooltip;
                CreateTooltip(iEdge, attrs, tooltip, true);
                SetClipboard(tooltip);
            }
        }
        break;
    case ID_GRAPH_ZOOMTOFIT:
        {
            m_wndLNGVC.SetItemScaleToFit(notification->id);
            CUniqueID sid, tid;
            m_wndLNGVC.SetSelected(edge.GetSource(sid), true, false);
            m_wndLNGVC.SetSelected(edge.GetTarget(tid), true, true);
        }
        break;
    case ID_GRAPH_GOTOSOURCE:
        {
            CUniqueID id;
            m_wndLNGVC.CenterVertex(edge.GetSource(id));
            m_wndLNGVC.SetSelected(id, true, false);
        }
        break;
    case ID_GRAPH_GOTOTARGET:
        {
            CUniqueID id;
            m_wndLNGVC.CenterVertex(edge.GetTarget(id));
            m_wndLNGVC.SetSelected(id, true, false);
        }
        break;
    }
    return 1;
}

HRESULT CGraphView3::OnSelChanged(VARIANT selection, IDispatch * oSender)
{
    CComVariant varSel(selection);
    CComSafeArray<VARIANT> selArray = selection.parray;
    if (selArray.GetCount() > 0)
    {
        CComVariant item = selArray.GetAt(0);
        CComSafeArray<VARIANT> properties;
        m_wndLNGVC.GetProperties(item.uintVal, properties);
        GetIMainFrame()->m_debugPropertiesView->LoadProperties(properties);
    }
    else
    {
        CComSafeArray<VARIANT> properties;
        properties.Create();
        GetIMainFrame()->m_debugPropertiesView->LoadProperties(properties);
    }
    return S_OK;
}

HRESULT CGraphView3::OnScaleChanged(int val, IDispatch * oSender)
{
    m_view_zoom.SetPos(101 - val);
    return S_OK;
}


LRESULT CGraphView3::OnCalcTooltip(int nId, LPNMHDR nmhdr, BOOL& bHandled)
{
    return 0;
}

LRESULT CGraphView3::OnTooltipLinkClick(int nId, LPNMHDR nmhdr, BOOL& bHandled)
{
    LPNMTOOLTIPLINKCLICK notification = (LPNMTOOLTIPLINKCLICK)nmhdr;
    if (notification->target.isEmpty())
    {
        ParsedDefinition parsed_def;
        if (DefinitionToLocation(notification->definition, parsed_def))
        {
            if (parsed_def.module.size() && parsed_def.attribute.size())
            {
                CWaitCursor wait;
                GetIMainFrame()->OpenAttribute(parsed_def.module.c_str(), parsed_def.attribute.c_str(), CreateIAttributeECLType(), parsed_def.row, parsed_def.col, 0);
            }
        }
    }
    else
    {
        CenterOnItem(notification->target);
    }
    return 0;
}

void CGraphView3::OnContextMenu(HWND /*hWnd*/, CPoint pt)
{
    WTL::CMenu m;
    m.LoadMenu(IDR_POPUP_RESULTS_SUMMARY);
    CResultContext context(this, m,pt);
    if ( !m_resultSlot || !m_resultSlot->OnContextResult(context) )
    {
        //if nobody handled it, then do it ourselves
        BOOL id = m.GetSubMenu(0).TrackPopupMenuEx(TPM_RETURNCMD, pt.x, pt.y, m_hWnd, NULL);
        if (id)
            PostMessage(WM_COMMAND, id);
    }
}

void CGraphView3::OnTimer(UINT_PTR nIDEvent)
{
    switch(nIDEvent)
    {
    case GV_TIMER_REFRESH:
        if (m_wu->IsComplete())
        {
            KillTimer(GV_TIMER_REFRESH);
            m_timerCountdown = -3;
        }

        if (--m_timerCountdown <= 0)
        {
            clib::thread run1(__FUNCTION__, boost::bind(&threadRefreshTimings, this, m_wu));
            if (m_graph && m_graph->IsRunning())
                clib::thread run2(__FUNCTION__, boost::bind(&threadRefreshGraph, this, m_graph));
            m_timerCountdown = 30;
        }

        break;
    }
}

void CGraphView3::OnVScroll(int scrollRequest, short scrollPos, HWND hWnd)
{
    SetMsgHandled(false);
    if (hWnd == m_view_zoom.m_hWnd)
    {
        SetMsgHandled(true);
        switch(scrollRequest)
        {
        case SB_TOP:
        case SB_BOTTOM:
        case SB_ENDSCROLL:
        case SB_LINEDOWN:
        case SB_LINEUP:
        case SB_PAGEDOWN:
        case SB_PAGEUP:
            m_wndLNGVC.SetScale((101 - m_view_zoom.GetPos()) / 100.0f);
            break;
        case SB_THUMBPOSITION:
        case SB_THUMBTRACK:
            m_wndLNGVC.SetScale((101 - scrollPos) / 100.0f);
            break;
        default:
            break;
        }
    }
}

BOOL CGraphView3::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    return m_wndLNGVC.OnMouseWheel(nFlags, zDelta, pt);
}

BOOL CGraphView3::OnForwardMsg(LPMSG Msg, DWORD nUserData)
{
    return IsDialogMessage(Msg);
}

void CGraphView3::DoEditFind()
{
    //m_searchInfo.SetPosition(-1);
    FindReplaceOnEditFind(_T(""), false, true);
}

LRESULT CGraphView3::OnEditFind(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    DoEditFind();
    return 0;
}

LRESULT CGraphView3::OnFindReplaceCmd(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
    if ( wParam != ID_EDIT_FINDREPLACECMD )
    {
        bHandled = FALSE;
        return 1;
    }

    if(GetFindReplace().FindNext())
    {
        //search up would be previous
        DoEditFindAgain(GetFindReplace().SearchDown() == TRUE);
    }
    return 0;
}

void CGraphView3::DoEditFindAgain(bool bNext)
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
        m_wndLNGVC.Find(&m_found, GetFindReplace().m_szFindWhat, true, true);//m_checkSearchTooltips.GetCheck());
        m_found.Reset();
    }
    if (bNext ? m_found.MoveNext() : m_found.MovePrevious())
    {
        CUniqueID id = m_found.Get();
        m_wndLNGVC.SetSelected(id, true, false);
        CenterOnItem(id);
    }
    else
    {
        MessageBeep(MB_ICONASTERISK);
        m_found.Reset();
    }
}

LRESULT CGraphView3::OnEditFindNext(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    DoEditFindAgain(true);
    return 0;
}

LRESULT CGraphView3::OnEditFindPrev(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    DoEditFindAgain(false);
    return 0;
}

LRESULT CGraphView3::OnEditGoto(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    static int prevLine = 1;
    std::_tstring id;
    if (IDOK == DoModalGotoID(id))
    {
        CUniqueID uid;
        if (m_wndLNGVC.GetVertex(CUniqueID(guidDefault, XGMML_CAT_VERTEX, id)))
            uid = CUniqueID(guidDefault, XGMML_CAT_VERTEX, id);
        else if (m_wndLNGVC.GetEdge(CUniqueID(guidDefault, XGMML_CAT_EDGE, id)))
            uid = CUniqueID(guidDefault, XGMML_CAT_EDGE, id);
        else if (m_wndLNGVC.GetSubgraph(CUniqueID(guidDefault, XGMML_CAT_SUBGRAPH, id)))
            uid = CUniqueID(guidDefault, XGMML_CAT_SUBGRAPH, id);
        if (!uid.isEmpty())
            CenterOnItem(uid);
    }
    return 0;
}

LRESULT CGraphView3::OnBrowserBack(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    CUniqueIDVector vertices;
    m_wndLNGVC.GetVertices(&vertices, TRISTATE_BOTH, TRISTATE_BOTH, TRISTATE_TRUE);
    bool append = false;
    while(vertices.MoveNext())
    {
        CUniqueIDVector inVertices;
        m_wndLNGVC.GetAdjacentVertices(&inVertices, vertices.Get(), true, false);
        while(inVertices.MoveNext())
        {
            m_wndLNGVC.SetSelected(inVertices.Get(), true, append);
            append = true;
        }
    }
    m_wndLNGVC.SetScaleToFit(true);
    return 0;
}

LRESULT CGraphView3::OnBrowserForward(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    CUniqueIDVector vertices;
    m_wndLNGVC.GetVertices(&vertices, TRISTATE_BOTH, TRISTATE_BOTH, TRISTATE_TRUE);
    bool append = false;
    while(vertices.MoveNext())
    {
        CUniqueIDVector outVertices;
        m_wndLNGVC.GetAdjacentVertices(&outVertices, vertices.Get(), false, true);
        while(outVertices.MoveNext())
        {
            m_wndLNGVC.SetSelected(outVertices.Get(), true, append);
            append = true;
        }
    }
    m_wndLNGVC.SetScaleToFit(true);
    return 0;
}

LRESULT CGraphView3::OnGraphZoomToFit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    m_wndLNGVC.SetScaleToFit();
    return 0;
}

LRESULT CGraphView3::OnGraphZoomToWidth(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    m_wndLNGVC.SetScaleToWidth(false, 1.0, false);
    return 0;
}

LRESULT CGraphView3::OnGraphLayout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    m_wndLNGVC.SetMessage(_T("Layout 8..."));
    m_wndLNGVC.StartLayout(SGV::LAYOUT_DOT);
    return 0;
}

LRESULT CGraphView3::OnGraphRefresh(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    m_timerCountdown = 30;
    m_prevTimerCountdown = 0;
    clib::thread run1(__FUNCTION__, boost::bind(&threadRefreshTimings, this, m_wu));
    if (m_graph)
        clib::thread run2(__FUNCTION__, boost::bind(&threadRefreshGraph, this, m_graph));
    return 0;
}

LRESULT CGraphView3::OnGraphFollowActive(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    m_followActive = !m_followActive;
    if (m_followActive)
    {		
        SelectRunningGraph();
        SelectRecentTimingGraph();
    }
    return 0;
}

LRESULT CGraphView3::OnListItemChanged(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
    bHandled = false;
    LPNMLISTVIEW pnmlv = (LPNMLISTVIEW)pnmh;
    if (pnmlv->uChanged & LVIF_STATE && pnmlv->uNewState != 0)
    {
        if (pnmlv->hdr.hwndFrom == m_listGraph.m_hWnd)
        {
            m_followActive = false;
            Dali::CGraphTimingVector timings;
            m_wu->GetGraphTimings(timings);
            buildList3Timings(m_listTimings, GetSelectedGraph(), timings, m_followActive);
            LoadGraph();
        }
        else if (pnmlv->hdr.hwndFrom == m_listTimings.m_hWnd)
        {
            m_followActive = false;
            LVITEM item;
            m_listTimings.GetSelectedItem(&item);
            Dali::CGraphTiming * timing = (Dali::CGraphTiming *)m_listTimings.GetItemData(item.iItem);
            if (timing)
                CenterOnTiming(timing);
        }
        else if (pnmlv->hdr.hwndFrom == m_listAttributes.m_hWnd)
        {
            m_followActive = false;
            LVITEM item;
            m_listAttributes.GetSelectedItem(&item);
            CUniqueID * id = (CUniqueID *)m_listAttributes.GetItemData(item.iItem);
            if (id)
                CenterOnItem(*id);
        }
    }
    return 0;
}

LRESULT CGraphView3::OnListItemDeleted(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
    bHandled = false;
    LPNMLISTVIEW pnmlv = (LPNMLISTVIEW)pnmh;
    if (pnmlv->hdr.hwndFrom == m_listGraph.m_hWnd)
    {
    }
    else if (pnmlv->hdr.hwndFrom == m_listTimings.m_hWnd)
    {
    }
    else if (pnmlv->hdr.hwndFrom == m_listAttributes.m_hWnd)
    {
        CUniqueID * id = (CUniqueID *)m_listAttributes.GetItemData(pnmlv->iItem);
        if (id)
            delete id;
    }
    return 0;
}

//CTabPane
HWND CGraphView3::GetHWND()
{
    return m_hWnd;
}

bool CGraphView3::CanCopy()
{
    return false;
}
bool CGraphView3::HasResults() const
{
    return false;
}

bool CGraphView3::UpdateUI(CCmdUI * cui)
{
    if (m_graph)
    {
        UPDATEUI(cui, ID_EDIT_FIND, true);
        UPDATEUI(cui, ID_EDIT_FINDREPLACECMD, true);
        bool hasFound = m_found.size() > 0;
        UPDATEUI(cui, ID_EDIT_FINDNEXT, hasFound);
        UPDATEUI(cui, ID_EDIT_FINDPREV, hasFound);
        UPDATEUI(cui, ID_EDIT_GOTO, true);

        bool hasSelection = true;//m_wndLNGVC.HasSelectedVertex();
        UPDATEUI(cui, ID_BROWSER_BACK, false);
        UPDATEUI(cui, ID_BROWSER_FORWARD, false);

        UPDATEUI(cui, ID_GRAPH_ZOOMTOFIT, true);
        UPDATEUI(cui, ID_GRAPH_ZOOMTOWIDTH, true);
        UPDATEUI(cui, ID_GRAPH_LAYOUT, true);
    }

    bool isComplete = !m_wu->IsComplete();
    bool isRunning = m_graph ? m_graph->GetState() == WUVisualState_running : false;

    if (cui->m_nID == ID_GRAPH_REFRESH && m_timerCountdown != m_prevTimerCountdown)
    {
        cui->SetText((boost::_tformat(_T("Refresh (%1%)\nr")) % m_timerCountdown).str().c_str());
        m_prevTimerCountdown = m_timerCountdown;
    }
    UPDATEUI(cui, ID_GRAPH_REFRESH, isComplete);

    if (cui->m_nID == ID_GRAPH_FOLLOWACTIVE)
        cui->SetCheck(m_followActive);
    UPDATEUI(cui, ID_GRAPH_FOLLOWACTIVE, isComplete);

    UPDATEUI(cui, ID_GRAPH_MINIMIZEINACTIVE, false);
    return false;
}

//ISearchable
bool CGraphView3::CanBeSearched() const
{
    return HasResults();
}

bool CGraphView3::Search(const CSearchableInfo &/*info*/, CSearchableResult &/*result*/) const
{
    return false;
}

void CGraphView3::operator()(Dali::WUEvent evt, Dali::IWorkunit * wu)
{
    switch (evt)
    {
    case Dali::WUEventUpdated:
        PostMessage(UMGV3_WU_UPDATE, (WPARAM)wu);
        break;
    }
}

void CGraphView3::operator()(Dali::IGraph * graph)
{
    if (m_graph == graph)
        PostMessage(UMGV3_GRAPH_UPDATE, (WPARAM)graph);
}

//  --  IResultView  ---
HWND CGraphView3::GetHwnd() const 
{
    return m_hWnd;
}
