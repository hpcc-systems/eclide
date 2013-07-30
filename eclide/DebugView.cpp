#include "StdAfx.h"
#include "..\en_us\resource.h"

#include "DebugView.h"
#include "MainFrm.h"
#include "GotoDlg.h"
#include "DialogAddBreakpoint.h"

enum DTIMER
{
	DTIMER_UNKNOWN = 0,
	DTIMER_DETACH,
	DTIMER_LAYOUT,
	DTIMER_LAST
};

bool ParseUniqueID(const CUniqueID & id, std::_tstring & activity, std::_tstring & outputIdx)
{
	typedef boost::tokenizer<boost::char_separator<TCHAR>, std::_tstring::const_iterator, std::_tstring> tokenizer;
	boost::char_separator<TCHAR> sep(_T("_"));
	std::_tstring strId = id.GetID();
	tokenizer tokens(strId, sep);
	tokenizer::iterator itr = tokens.begin();
	if (itr != tokens.end())
	{
		activity = (*itr++).c_str();
		if (itr != tokens.end())
		{
			outputIdx = (*itr++).c_str();
			return true;
		}
	}
	return false;
}

static const SectionLabelDefault GLOBAL_ROXIE_IP(SectionLabel(_T("RoxieDebugger"), _T("IP")), _T("10.173.217.1"));
static const SectionLabelDefault GLOBAL_ROXIE_PORT(SectionLabel(_T("RoxieDebugger"), _T("Port")), _T("9876"));
static const SectionLabelDefault GLOBAL_ROXIE_QUERYLABEL(SectionLabel(_T("RoxieDebugger"), _T("QueryLabel")), _T("graphrpn2"));
static const SectionLabelDefault GLOBAL_ROXIE_QUERYPARAMS(SectionLabel(_T("RoxieDebugger"), _T("QueryParams")), _T(""));
static const SectionLabelDefault GLOBAL_ROXIE_POPUPEXCEPTIONS(SectionLabel(_T("RoxieDebugger"), _T("PopupExceptions")), true);
static const SectionLabelDefault GLOBAL_ROXIE_RECENTERONACTIVE(SectionLabel(_T("RoxieDebugger"), _T("RecenterOnActive")), true);
static const SectionLabelDefault GLOBAL_ROXIE_REDUNDANTSUBGRAPHS(SectionLabel(_T("RoxieDebugger"), _T("RedundantSubgraphs")), false);
static const SectionLabelDefault GLOBAL_ROXIE_PASSTHROUGHVERTICES(SectionLabel(_T("RoxieDebugger"), _T("PassthroughVertices")), false);
static const SectionLabelDefault GLOBAL_ROXIE_RUNNINGONLY(SectionLabel(_T("RoxieDebugger"), _T("RunningOnly")), false);
static const SectionLabelDefault GLOBAL_ROXIE_MDILAYOUT(SectionLabel(_T("RoxieDebugger"), _T("MDILayout")), _T(""));
static const SectionLabelDefault GLOBAL_ROXIE_SPLITTERPOS(SectionLabel(_T("RoxieDebugger"), _T("SplitterPos")), 50);

CDebugView::CDebugView(Dali::IWorkunit * wu, IResultSlot *resultSlot) : m_wu(wu)
{
	m_resultSlot = resultSlot;

	m_ip = CT2A(CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_ROXIE_IP)));
	m_port = CT2A(CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_ROXIE_PORT)));
	m_queryLabel = CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_ROXIE_QUERYLABEL));//_T("casestmt");
	m_queryParams = CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_ROXIE_QUERYPARAMS));
	m_debugSession = new CRoxieDebugSession(this);
	m_showMode = SHOW_MODE_ALL;
	m_popupExceptions = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_ROXIE_POPUPEXCEPTIONS);
	m_viewRecenterOnActive = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_ROXIE_RECENTERONACTIVE);
	m_viewRedundantSubgraphs = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_ROXIE_REDUNDANTSUBGRAPHS);
	m_viewPassthroughVertices = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_ROXIE_PASSTHROUGHVERTICES);
	//m_viewRunningOnly = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_ROXIE_RUNNINGONLY);
	m_layoutProgress = 0;
	m_prevActiveWnd = NULL;
	m_searchConditions.m_condition = _T("Equals");
	m_searchConditions.m_caseSensitive = false;
	m_supressRefresh = false;

	m_frmGlobalGraph.SetOwner(this);
	m_frmActiveGraph.SetOwner(this, &m_frmGlobalGraph.m_view);
}

CDebugView::~CDebugView()
{
	GetIConfig(QUERYBUILDER_CFG)->Set(GLOBAL_ROXIE_POPUPEXCEPTIONS, m_popupExceptions);
	GetIConfig(QUERYBUILDER_CFG)->Set(GLOBAL_ROXIE_RECENTERONACTIVE, m_viewRecenterOnActive);
	GetIConfig(QUERYBUILDER_CFG)->Set(GLOBAL_ROXIE_REDUNDANTSUBGRAPHS, m_viewRedundantSubgraphs);
	GetIConfig(QUERYBUILDER_CFG)->Set(GLOBAL_ROXIE_PASSTHROUGHVERTICES, m_viewPassthroughVertices);
	//GetIConfig(QUERYBUILDER_CFG)->Set(GLOBAL_ROXIE_RUNNINGONLY, m_viewRunningOnly);
}

void CDebugView::PostCallStart(roxie::CALL callMode) 
{
	::PostMessage(m_hWnd, DUM_CALLSTART, (WPARAM)callMode, NULL);
}

void CDebugView::PostCallEnd(roxie::CALL callMode, const roxie::Response & response) 
{
	if (callMode == roxie::CALL_START)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		m_sessionResponse = response;
	}
	::PostMessage(m_hWnd, DUM_CALLEND, (WPARAM) callMode, (LPARAM)new roxie::Response(response));
}

#define	UPDATECHECKUI(cui, id, val)	if (cui->m_nID == id){\
						cui->Enable(val);\
						return true;}
bool CDebugView::UpdateUI(CCmdUI * cui)
{
	if (true)
	{
		bool active = m_debugSession->GetDebugState() != CRoxieDebugSession::DEBUGSTATE_COMPLETE;
		bool paused = m_debugSession->GetDebugState() == CRoxieDebugSession::DEBUGSTATE_PAUSED;
		bool running = m_debugSession->GetDebugState() == CRoxieDebugSession::DEBUGSTATE_CONTINUE || m_debugSession->GetDebugState() == CRoxieDebugSession::DEBUGSTATE_DETACH;
		bool activeGraphHasSelectedBreakpoint = false;
		bool activeViewHasSearch = false;
		bool activeHasSelectedEdge = false;
		if (CDebugGraphView * activeWnd = GetActiveGraphFrame())
		{
			if (m_prevActiveWnd != activeWnd)
			{
				m_prevActiveWnd = activeWnd;
				CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> ar;
				GetIMainFrame()->m_wndRibbonBar.GetElementsByID(ID_DEBUGGER_ZOOM, ar);
				ATLASSERT(ar.GetSize() == 1);
				CMFCRibbonSlider* pSlider = DYNAMIC_DOWNCAST(CMFCRibbonSlider, ar[0]);
				if(pSlider)
					pSlider->SetPos(activeWnd->GetZoom());
			}

			CUniqueIDVector edges;
			activeWnd->GetSelectedEdges(&edges);
			edges.Reset();
			int selEdgeCount = 0;
			if (edges.MoveNext())
			{
				if (m_debugSession->HasBreakpoint(edges.Get().GetID()))
					activeGraphHasSelectedBreakpoint = true;
				++selEdgeCount;
			}
			activeHasSelectedEdge = selEdgeCount == 1;
			activeViewHasSearch = activeWnd->m_found.size() > 0;
		}
		bool hasBreakpoints = m_debugSession->GetBreakpointCount() > 0;

		UPDATEUI(cui, ID_EDIT_FIND, true);
		UPDATEUI(cui, ID_EDIT_FINDREPLACECMD, true);
		UPDATEUI(cui, ID_EDIT_FINDNEXT, activeViewHasSearch);
		UPDATEUI(cui, ID_EDIT_FINDPREV, activeViewHasSearch);
		UPDATEUI(cui, ID_EDIT_GOTO, true);
		UPDATEUI(cui, ID_DEBUGGER_ZOOM, true);
		UPDATEUI(cui, ID_DEBUGGER_RECENTER, true);
		UPDATEUI(cui, ID_DEBUGGER_LAYOUT, true);
		UPDATEUI(cui, ID_DEBUGGER_CONTINUE, paused);
		UPDATEUI(cui, ID_DEBUGGER_DETACH, paused);
		UPDATEUI(cui, ID_DEBUGGER_BREAK, running);
		UPDATEUI(cui, ID_DEBUGGER_ABORT, active);
		UPDATEUI(cui, ID_DEBUGGER_RESTART, active);
		UPDATEUI(cui, ID_DEBUGGER_STEP, paused);
		UPDATEUI(cui, ID_DEBUGGER_NEXT, paused);
		UPDATEUI(cui, ID_DEBUGGER_STEPGRAPH, paused);
		UPDATEUI(cui, ID_DEBUGGER_STEPGRAPHSTART, paused);
		UPDATEUI(cui, ID_DEBUGGER_STEPGRAPHEND, paused);
		UPDATEUI(cui, ID_DEBUGGER_SETGLOBALBREAKPOINT, active);
		UPDATEUI(cui, ID_DEBUGGER_SETBREAKPOINT, active && activeHasSelectedEdge);
		UPDATEUI(cui, ID_DEBUGGER_REMOVEBREAKPOINT, active && activeGraphHasSelectedBreakpoint);
		UPDATEUI(cui, ID_DEBUGGER_REMOVEALLBREAKPOINTS, hasBreakpoints);
		if (cui->m_nID == ID_VIEW_RUNNINGONLYGLOBAL)
		{
			cui->Enable(active);
			cui->SetCheck(m_frmGlobalGraph.m_viewRunningOnly);
			return true;
		}
		if (cui->m_nID == ID_VIEW_RUNNINGONLYACTIVE)
		{
			cui->Enable(active);
			cui->SetCheck(m_frmActiveGraph.m_viewRunningOnly);
			return true;
		}
		if (cui->m_nID == ID_VIEW_FOUNDONLYGLOBAL)
		{
			cui->Enable(active);
			cui->SetCheck(m_frmGlobalGraph.m_viewFoundOnly);
			return true;
		}
		if (cui->m_nID == ID_VIEW_FOUNDONLYACTIVE)
		{
			cui->Enable(active);
			cui->SetCheck(m_frmActiveGraph.m_viewFoundOnly);
			return true;
		}
		UPDATEUI(cui, IDC_BUTTON_SEARCH, paused);
		UPDATEUI(cui, IDC_BUTTON_CLEARSEARCH, paused);
		if (!active && m_wu->IsDebugging())
		{
			//StlLinked<Dali::IDali> server = Dali::AttachDali(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT), _T("Dali"));
			//StlLinked<Dali::IWorkunit> wu = server->GetWorkunit(m_wu->GetWuid());
		}
		else if (active && m_wu->IsComplete())
		{
			m_debugSession->SetDebugState(CRoxieDebugSession::DEBUGSTATE_COMPLETE);
		}
	}

//void CDebugView::OnUpdateModifyGraph(CCmdUI* pCmdUI)
//{
//	CDebugGraphView * activeWnd = GetActiveGraphFrame();
//	switch(pCmdUI->m_nID)
//	{
//	case ID_DEBUGGER_ZOOM:
//		if (m_prevActiveWnd != activeWnd)
//		{
//			CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> ar;
//			m_wndRibbonBar.GetElementsByID(ID_DEBUGGER_ZOOM, ar);
//			ATLASSERT(ar.GetSize() == 1);
//			CMFCRibbonSlider* pSlider = DYNAMIC_DOWNCAST(CMFCRibbonSlider, ar[0]);
//			ATLASSERT(pSlider);
//			pSlider->SetPos(activeWnd->GetZoom());
//		}
//	}
//	pCmdUI->Enable(paused);
//	m_prevActiveWnd = activeWnd;
//}
	return false;
}

void CDebugView::SetSupressRefresh(bool supress)
{
	m_supressRefresh = supress;
}

bool CDebugView::SupressRefresh() const
{
	return m_supressRefresh;
}

LRESULT CDebugView::OnInitDialog(HWND /*hWnd*/, LPARAM /*lParam*/)
{
	SetMsgHandled(false);
	DlgResize_Init(false, true, 0);

	m_name = _T("Debugger");
	SetWindowText(m_name);

	CRect rc(0, 0, 0, 0);
	CWindow wndPlaceholder = GetDlgItem(IDC_STATIC_PLACEHOLDER);
	wndPlaceholder.GetWindowRect(rc);
	ScreenToClient(rc);
	wndPlaceholder.DestroyWindow();
	m_splitter.Create(m_hWnd, rc);
	m_splitter.SetDlgCtrlID(IDC_STATIC_PLACEHOLDER);

	m_paneGlobal.Create(m_splitter, _T("Global"));
	//m_paneGlobal.SetPaneContainerExtendedStyle(PANECNT_NOCLOSEBUTTON, PANECNT_NOCLOSEBUTTON);

	m_paneActive.Create(m_splitter, _T("Active"));
	//m_paneActive.SetPaneContainerExtendedStyle(PANECNT_NOCLOSEBUTTON, PANECNT_NOCLOSEBUTTON);

	m_frmGlobalGraph.Create(m_paneGlobal.m_hWnd);
	m_paneGlobal.SetClient(m_frmGlobalGraph);

	m_frmActiveGraph.Create(m_paneActive);
	m_paneActive.SetClient(m_frmActiveGraph);

	m_splitter.SetSplitterPanes(m_paneGlobal, m_paneActive);
	m_splitter.SetSplitterPosPct((int)GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_ROXIE_SPLITTERPOS));

	Init();
	return 0;
}

void CDebugView::Init() 
{
	m_sequence = 0;
	m_sequenceGraph = 0;
	m_showMode = SHOW_MODE_ALL;
	m_frmGlobalGraph.Clear();
	m_frmActiveGraph.Clear();
	GetIMainFrame()->m_debugDataViews->Send_Clear();
	GetIMainFrame()->m_debugSearchView->Send_Clear();

	m_debugSession->AttachEspSession(m_wu);
	m_active = m_debugSession->GetActiveID();

	m_debugSession->SetDebugState(CRoxieDebugSession::DEBUGSTATE_PAUSED);
	m_debugSession->GetStatus();
#ifdef _DEBUG
//	m_debugSession->Set(_T("executeSequentially"), _T("1"));
//	m_debugSession->Set(_T("stopOnLimits"), _T("0"));
#endif

	PostMessage(CWM_INITIALIZE);
}

LRESULT CDebugView::OnInitialize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam)
{
	UpdateDisplay();
	::SetFocus(m_frmGlobalGraph.m_view.GetHWND());
	m_frmGlobalGraph.m_view.SetMessage(_T("Loading..."));
	m_debugSession->GetGlobalGraph();
	m_frmActiveGraph.m_view.SetMessage(_T("Loading..."));
	m_debugSession->GetActiveGraph();

	m_nTimerDetach = SetTimer(DTIMER_DETACH, 10000, NULL);
	m_nTimerLayout = SetTimer(DTIMER_LAYOUT, 100, NULL);
	return 0;
}

void CDebugView::OnDestroy()
{
	SetMsgHandled(false);
	GetIMainFrame()->m_debugSearchView->SetOwner(NULL);
	GetIMainFrame()->m_debugBreakpointView->SetOwner(NULL);
	GetIMainFrame()->m_debugDataViews->SetOwner(NULL);
	GetIMainFrame()->m_debugPropertyGridViews->SetOwner(NULL);

	KillTimer(DTIMER_DETACH);
	KillTimer(DTIMER_LAYOUT);
	CRect clientRect;
	m_splitter.GetClientRect(clientRect);
	GetIConfig(QUERYBUILDER_CFG)->Set(GLOBAL_ROXIE_SPLITTERPOS, MulDiv(m_splitter.GetSplitterPos(), 100, clientRect.Width()));
	//m_frmGlobalGraph.DestroyWindow();
	//m_frmActiveGraph.DestroyWindow();
	//m_paneGlobal.DestroyWindow();
	//m_paneActive.DestroyWindow();
	//m_splitter.DestroyWindow();
}

void CDebugView::OnContextMenu(HWND /*hWnd*/, CPoint pt)
{
	WTL::CMenu m;
	m.LoadMenu(IDR_POPUP_RESULTS_EXCEPTION);
	CResultContext context(this, m,pt);
	if ( !m_resultSlot || !m_resultSlot->OnContextResult(context) )
	{
		//if nobody handled it, then do it ourselves
		BOOL id = m.GetSubMenu(0).TrackPopupMenuEx(TPM_RETURNCMD, pt.x, pt.y, m_hWnd, NULL);
		if (id)
			PostMessage(WM_COMMAND, id);
	}
}

void CDebugView::OnSetFocus(CWindow wndOld)
{
	GetIMainFrame()->m_debugSearchView->SetOwner(this);
	GetIMainFrame()->m_debugBreakpointView->SetOwner(this);
	GetIMainFrame()->m_debugDataViews->SetOwner(this);
	GetIMainFrame()->m_debugPropertyGridViews->SetOwner(this);
}

BOOL CDebugView::OnEraseBkgnd(WTL::CDCHandle dc)
{
	return 1;
}

const CUniqueID & CDebugView::GetCurrentID()
{
	return m_debugSession->GetCurrentID();
}

void CDebugView::NewSelection(const CDebugGraphView * from, const CUniqueID & newSel, bool recenterGraph, bool forceDataLoad, bool dontShowDefinition)
{
	if (m_debugSession->GetCurrentID() != newSel)
		GetIMainFrame()->m_debugDataViews->ClearTabData();
	m_debugSession->SetSelected(newSel, forceDataLoad);

	Element::StringStringMap active, global;
	if (IGraphItem * item = m_frmGlobalGraph.m_view.GetGraphItem(m_debugSession->GetGlobalID(newSel)))
	{
		if (CComQIPtr<CStringStringMap> _global = item->GetPropertyUnknown(PROP_ATTRIBUTES))
			global = _global->m_attr;
	}
	if (IGraphItem * item = m_frmActiveGraph.m_view.GetGraphItem(newSel))
	{
		if (CComQIPtr<CStringStringMap> _active = item->GetPropertyUnknown(PROP_ATTRIBUTES))
			active = _active->m_attr;
	}

	std::_tstring label;
	switch(newSel.GetCategory().GetCategory())
	{
	case XGMML_CAT_GRAPH:
		label = _T("Graph:  ");
		break;
	case XGMML_CAT_SUBGRAPH:
		label = _T("Subgraph:  ");
		break;
	case XGMML_CAT_VERTEX:
		label = _T("Vertex:  ");
		break;
	case XGMML_CAT_EDGE:
		label = _T("Edge:  ");
		break;
	default:
		break;
	};
	label += newSel.GetID();
	GetIMainFrame()->m_debugPropertiesView->LoadProperties(label, active, global);

	GetIMainFrame()->m_debugDataViews->ClearTabSelection();
	if (!dontShowDefinition && newSel.GetCategory().GetCategory() == XGMML_CAT_VERTEX)
	{
		std::_tstring def = active[_T("definition")];
		if (!def.length())
			def = global[_T("definition")];
		ParsedDefinition pd;
		if (DefinitionToLocation(def, pd))
		{
			if (pd.IsBuilder())
				GetIMainFrame()->m_debugDataViews->SetTabSelection(_T("Builder"), pd.row, pd.col);
			else
				GetIMainFrame()->m_debugDataViews->SetTabSelection((pd.module + _T(".") + pd.attribute).c_str(), pd.row, pd.col);
		}
	}

	if (from == &m_frmActiveGraph || from == NULL)
	{
		m_frmGlobalGraph.EnsureVisible(m_debugSession->GetGlobalID(newSel), recenterGraph);
		m_frmGlobalGraph.m_view.InvalidateLayout();
	}
	if (from == &m_frmGlobalGraph || from == NULL)
	{
		m_frmActiveGraph.EnsureVisible(newSel, recenterGraph);
		m_frmActiveGraph.m_view.InvalidateLayout();
	}
}

void CDebugView::ScaleChanged(int zoom)
{
	CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> ar;
	GetIMainFrame()->m_wndRibbonBar.GetElementsByID(ID_DEBUGGER_ZOOM, ar);
	ATLASSERT(ar.GetSize() == 1);
	CMFCRibbonSlider* pSlider = DYNAMIC_DOWNCAST(CMFCRibbonSlider, ar[0]);
	ATLASSERT(pSlider);
	pSlider->SetPos(zoom);
}

unsigned int CDebugView::ProcessContextMenu(WTL::CMenu & menu, CPoint & pt, CUniqueIDVector & subgraphs)
{
	unsigned int retVal = menu.GetSubMenu(0).TrackPopupMenuEx(TPM_RETURNCMD, pt.x, pt.y, m_frmGlobalGraph, NULL);
	switch(retVal)
	{
	case ID_GRAPH_ADDBREAKPOINT:
		DoAddBreakpoint(CUniqueID());
		break;
	}
	return retVal;
}

const IGraphItem * GetTopGraph(IGraphItem * item)	//Not top graph as it is the global hidden one.
{
	IGraphItem * retVal = item;
	IGraphItem * retValParent = item;
	while(IGraphItem * parent = retValParent->GetParent())
	{
		retVal = retValParent;
		retValParent = parent;
	}
	return retVal;
}

unsigned int CDebugView::ProcessContextMenu(WTL::CMenu & menu, CPoint & pt, IGraphSubgraph * subgraph)
{
	CString strCurrent, strNew;
	menu.GetMenuString(ID_GRAPH_CONTINUEHERE, strCurrent, MF_BYCOMMAND);
	const IGraphItem * topGraph = GetTopGraph(subgraph);
	if (topGraph)
	{
		strNew.FormatMessage(strCurrent, topGraph->GetID().GetID());
		menu.ModifyMenu(ID_GRAPH_CONTINUEHERE, MF_BYCOMMAND, ID_GRAPH_CONTINUEHERE, strNew);
	}
	else
	{
		strNew.FormatMessage(strCurrent, _T("..."));
		menu.ModifyMenu(ID_GRAPH_CONTINUEHERE, MF_BYCOMMAND, ID_GRAPH_CONTINUEHERE, strNew);
		menu.EnableMenuItem(ID_GRAPH_CONTINUEHERE, MF_GRAYED);
	}

	unsigned int retVal = menu.GetSubMenu(0).TrackPopupMenuEx(TPM_RETURNCMD, pt.x, pt.y, m_frmGlobalGraph, NULL);
	switch(retVal)
	{
	case ID_GRAPH_CONTINUEHERE:
		{
			if (topGraph)
				m_debugSession->Continue(topGraph->GetID());
		}
		break;
	case ID_GRAPH_ADDBREAKPOINT:
		DoAddBreakpoint(subgraph->GetID());
		break;
	}
	return retVal;
}

unsigned int CDebugView::ProcessContextMenu(WTL::CMenu & menu, CPoint & pt, SGV::CGraphVertex & vertex)
{
	unsigned int retVal = menu.GetSubMenu(0).TrackPopupMenuEx(TPM_RETURNCMD, pt.x, pt.y, m_frmGlobalGraph, NULL);
	//switch(retVal)
	//{
	//case ID_GRAPH_CONTINUEHERE:
	//	m_debugSession->Continue(vertex.GetID());
	//	break;
	//}
	return retVal;
}

unsigned int CDebugView::ProcessContextMenu(WTL::CMenu & menu, CPoint & pt, SGV::CGraphEdge & edge)
{
	CString strCurrent, strNew;
	menu.GetMenuString(ID_GRAPH_CONTINUEHERE, strCurrent, MF_BYCOMMAND);
	strNew.FormatMessage(strCurrent, edge.GetID().GetID());
	menu.ModifyMenu(ID_GRAPH_CONTINUEHERE, MF_BYCOMMAND, ID_GRAPH_CONTINUEHERE, strNew);
	if (!m_debugSession->HasBreakpoint(edge.GetID().GetID()))
		menu.EnableMenuItem(ID_GRAPH_CLEARBREAKPOINT, MF_GRAYED);

	unsigned int retVal = menu.GetSubMenu(0).TrackPopupMenuEx(TPM_RETURNCMD, pt.x, pt.y, m_frmGlobalGraph, NULL);
	switch(retVal)
	{
	case ID_GRAPH_CONTINUEHERE:
		m_debugSession->Continue(edge.GetID());
		break;
	case ID_GRAPH_SETBREAKPOINT:
		m_debugSession->AddBreakpoint(edge.GetID().GetID(), _T("Break"), _T("Always"), _T(""), _T(""), _T("Always"), _T(""), false);
		break;
	case ID_GRAPH_ADDBREAKPOINT:
		DoAddBreakpoint(edge.GetID());
		break;
	case ID_GRAPH_CLEARBREAKPOINT:
		m_debugSession->DeleteBreakpoint(edge.GetID());
		break;
	}
	return retVal;
}

void CDebugView::DeleteBreakpoint(const ::CString & idx)
{
	m_debugSession->DeleteBreakpoint(static_cast<const TCHAR *>(idx));
}

void CDebugView::GetAllBreakpoints() 
{
	m_debugSession->GetAllBreakpoints();
}

const roxie::CSearchMap & CDebugView::GetSearchResults() const
{
	return m_debugSession->GetSearchResults();
}

const CUniqueID & CDebugView::GetGlobalID(const CUniqueID & id) const
{
	return m_debugSession->GetGlobalID(id);
}

GlobalSearchConditions * CDebugView::GetGlobalSearchConditions() 
{
	return &m_searchConditions;
}

const SGV::CGraphViewCtrl & CDebugView::GetGlobalGraphView()
{
	return m_frmGlobalGraph.m_view;
}

const SGV::CGraphViewCtrl & CDebugView::GetActiveGraphView()
{
	return m_frmActiveGraph.m_view;
}

Dali::IWorkunit * CDebugView::GetWorkunit()
{
	return m_wu;
}

CRoxieDebugSession * CDebugView::GetDebugSession()
{
	return m_debugSession;
}

LRESULT CDebugView::OnCallStart(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam)
{
	roxie::CALL callMode = static_cast<roxie::CALL>(wParam);
	switch(callMode)
	{
	case roxie::CALL_START:
	case roxie::CALL_STEP:
	case roxie::CALL_NEXT:
	case roxie::CALL_CONTINUE:
		//TODO
		m_captionText = _T("");
		GetIMainFrame()->m_wndCaptionBar.RemoveText();
		if (m_popupExceptions)
		{
			if (GetIMainFrame()->m_wndCaptionBar.IsWindowVisible())
			{
				GetIMainFrame()->m_wndCaptionBar.ShowWindow(SW_HIDE);
				GetIMainFrame()->RecalcLayout(FALSE);
			}
		}
		break;

	default:
		break;
	}
	return 0;
}

class CResponsePointer
{
public:
	roxie::Response * m_response;

	CResponsePointer(roxie::Response * response)
	{
		m_response = response;
	}

	~CResponsePointer()
	{
		delete m_response;
	}
};

LRESULT CDebugView::OnCallEnd(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam)
{
	roxie::CALL callMode = static_cast<roxie::CALL>(wParam);
	CResponsePointer rptr(reinterpret_cast<roxie::Response *>(lParam));

	switch (rptr.m_response->m_exception->GetCode())
	{
	case -1:
		break;
	case 1458:	//No Current Graph
		break;
	default:
		m_captionText = (boost::_tformat(_T("%1% - %2%:  %3%")) % rptr.m_response->m_exception->GetSource() % rptr.m_response->m_exception->GetCode() % rptr.m_response->m_exception->GetMessage()).str() + _T("\n") + m_captionText;
	}

	GetIMainFrame()->m_wndCaptionBar.SetText(m_captionText.c_str(), CMFCCaptionBar::ALIGN_LEFT);
	if (m_popupExceptions)
	{
		if (!GetIMainFrame()->m_wndCaptionBar.IsWindowVisible() && !m_captionText.empty())
		{
			GetIMainFrame()->m_wndCaptionBar.ShowWindow(SW_SHOW);
			GetIMainFrame()->RecalcLayout(FALSE);
		}
		else if (GetIMainFrame()->m_wndCaptionBar.IsWindowVisible() && m_captionText.empty())
		{
			GetIMainFrame()->m_wndCaptionBar.ShowWindow(SW_HIDE);
			GetIMainFrame()->RecalcLayout(FALSE);
		}
		GetIMainFrame()->m_wndCaptionBar.RedrawWindow();
	}

	if (rptr.m_response->m_debug->HasState() && rptr.m_response->m_warning->HasCode())
		GetIMainFrame()->PostStatus((boost::_tformat(_T("%1%:  %2% - %3%")) % rptr.m_response->m_debug->GetState() % rptr.m_response->m_warning->GetCode() % rptr.m_response->m_warning->GetMessage()).str().c_str());
	else if (rptr.m_response->m_warning->HasCode())
		GetIMainFrame()->PostStatus((boost::_tformat(_T("%1% - %2%")) % rptr.m_response->m_warning->GetCode() % rptr.m_response->m_warning->GetMessage()).str().c_str());
	else if(rptr.m_response->m_debug->HasState())
		GetIMainFrame()->PostStatus((boost::_tformat(_T("%1%")) % rptr.m_response->m_debug->GetState()).str().c_str());

	int newSequence = rptr.m_response->m_debug->GetSequence();
	int newGraphSequenceNum = rptr.m_response->m_debug->GetGraphSequenceNum();
	switch(callMode)
	{
	case roxie::CALL_ABORT:
		return 0;

	case roxie::CALL_ALLGRAPH:
		{
			m_xgmmlAll = rptr.m_response->m_debug->GetGraph();
			m_frmGlobalGraph.LoadXGMML(m_xgmmlAll, m_debugSession, m_debugSession->GetGlobalID(m_debugSession->GetActiveID()), m_viewRedundantSubgraphs, m_viewPassthroughVertices);
			GetIMainFrame()->m_debugDataViews->PrepTabEcl(m_frmGlobalGraph.m_view, m_wu);
			GetIMainFrame()->m_debugPropertyGridViews->PrepTabPropertyGrid(&m_frmGlobalGraph.m_view, NULL);
//			newSequence = 1;
		}
		break;
	case roxie::CALL_ACTIVEGRAPH:
		{
			CUniqueID activeID = m_debugSession->GetActiveID();
			m_xgmmlActive = rptr.m_response->m_debug->GetGraph();
			m_frmGlobalGraph.MergeXGMML(m_xgmmlActive, m_debugSession, m_debugSession->GetGlobalID(m_debugSession->GetActiveID()), false, m_viewRedundantSubgraphs, m_viewPassthroughVertices);
			m_frmActiveGraph.LoadXGMML(m_xgmmlActive, m_debugSession, m_debugSession->GetActiveID(), m_viewRedundantSubgraphs, m_viewPassthroughVertices);
			NewSelection(NULL, activeID, true, true);
			GetIMainFrame()->m_debugPropertyGridViews->PrepTabPropertyGrid(&m_frmGlobalGraph.m_view, &m_frmActiveGraph.m_view);
//			newSequence = 1;
		}
		break;
	case roxie::CALL_GETBREAKPOINTS:
		{
			roxie::CBreakpointMap bpmap;
			rptr.m_response->m_debug->GetBreakpoints(bpmap);
			GetIMainFrame()->m_debugBreakpointView->LoadBreakpoints(bpmap);
		}
		break;
	case roxie::CALL_CHANGES:
		{
			std::_tstring xgmml = rptr.m_response->m_debug->GetGraph();
			m_frmGlobalGraph.MergeXGMML(xgmml, m_debugSession, m_debugSession->GetGlobalID(m_debugSession->GetActiveID()), false, m_viewRedundantSubgraphs, m_viewPassthroughVertices);
			m_frmActiveGraph.MergeXGMML(xgmml, m_debugSession, m_debugSession->GetActiveID(), true, m_viewRedundantSubgraphs, m_viewPassthroughVertices);
			GetIMainFrame()->m_debugPropertyGridViews->PrepTabPropertyGrid(&m_frmGlobalGraph.m_view, &m_frmActiveGraph.m_view);
		}
		break;
	case roxie::CALL_COUNTS:
		{
			const roxie::CCountMap & results = m_debugSession->GetCountResults();
			for(roxie::CCountMap::const_iterator itr = results.begin(); itr != results.end(); ++itr)
			{
				IGraphItem * i = m_frmGlobalGraph.m_view.GetGraphItem(itr->first);
				if (i)
				{
					i->SetLabel(itr->second.c_str());
					i->PropagateProperty(PROP_ROXIESTARTED, true);
				}
			}
			m_frmGlobalGraph.CalcVisibility();
		}
		break;
	case roxie::CALL_START:
		{
			GetIMainFrame()->m_debugDataViews->Send_Clear();
			GetIMainFrame()->m_debugDataViews->SetTabData(rptr.m_response->m_debug->GetData(), roxie::CSearchRowVector());
			GetIMainFrame()->m_debugSearchView->Send_Clear();
		}
		break;
	case roxie::CALL_INFO:
		{
			roxie::CSearchRowVector rows;
			m_debugSession->GetSearchResult(m_debugSession->GetCurrentID(), rows);
			GetIMainFrame()->m_debugDataViews->SetTabData(_T("Data"), rptr.m_response->m_debug->GetData(_T("_")), true, rows);
			//GetIMainFrame()->m_debugDataViews->SetTabData((boost::_tformat(_T("Active - %1%")) % m_debugSession->GetCurrentID().GetID()).str(), rptr.m_response->m_debug->GetData(_T("_")), true);
		}
		break;
	case roxie::CALL_SEARCH:
		{
			const roxie::CSearchMap & results = m_debugSession->GetSearchResults();
			bool modified = false;
			GetIMainFrame()->m_debugSearchView->LoadResults(results);

			m_frmGlobalGraph.CalcVisibilityAndLayout(m_debugSession->GetGlobalID(m_debugSession->GetCurrentID()), modified);
			m_frmActiveGraph.CalcVisibilityAndLayout(m_debugSession->GetCurrentID(), modified);

			roxie::CSearchRowVector rows;
			m_debugSession->GetSearchResult(m_debugSession->GetCurrentID(), rows);
			GetIMainFrame()->m_debugDataViews->SetTabSelection(_T("Data"), rows);
		}
		break;
	default:
		break;
	}

	if (m_sequence < newSequence)
	{
		if (m_sequenceGraph < newGraphSequenceNum)
		{
			m_frmActiveGraph.m_view.SetMessage(_T("Loading..."));
			m_debugSession->GetActiveGraph();
			m_sequenceGraph = newGraphSequenceNum;
		}
		else
		{
			m_debugSession->GetChanges(m_sequence);
			CUniqueID id = m_debugSession->GetActiveID();
			if (!id.isEmpty() && id.GetCategory().GetCategory() == XGMML_CAT_EDGE)
				NewSelection(&m_frmActiveGraph, id, false, true);
		}
		m_debugSession->GetCounts(m_sequence);		//Gets aggregated global counts
		m_sequence = newSequence;
	}

	if (!m_selected.isEmpty())
	{
		m_selected = CUniqueID();
	}
	else if (m_active != m_debugSession->GetActiveID())
	{
		m_active = m_debugSession->GetActiveID();
		m_frmGlobalGraph.EnsureVisible(m_debugSession->GetGlobalID(m_active), m_viewRecenterOnActive);
		m_frmActiveGraph.EnsureVisible(m_active, m_viewRecenterOnActive);
	}

	m_frmGlobalGraph.m_view.InvalidateLayout();
	m_frmActiveGraph.m_view.InvalidateLayout();
	return 0;
}

//void CDebugView::OnStartTimer() 
//{
//   m_nTimerDetach = SetTimer(TIMER_DETACH, 10000, 0);
//   m_nTimerLayout = SetTimer(TIMER_LAYOUT, 100, 0);
//}
//
//void CDebugView::OnStopTimer() 
//{
//   KillTimer(m_nTimerLayout);   
//   KillTimer(m_nTimerDetach);   
//}
//
void CDebugView::OnTimer(UINT nIDEvent) 
{
	switch(nIDEvent)
	{
	case DTIMER_DETACH:
		{
			bool detach = false;
			switch(m_debugSession->GetDebugState())
			{
			case CRoxieDebugSession::DEBUGSTATE_DETACH:
			//	detach = true;
			////	//Fall through
			////case CRoxieDebugSession::DEBUGSTATE_CONTINUE:
			//	{
			//		CWaitCursor wait;
			//		roxie::Response response;
			//		m_debugSession->RefreshCounts(m_sequence, response, detach);
			//		std::_tstring xgmml = response.m_debug->GetGraph();
			//		m_frmGlobalGraph.MergeXGMML(xgmml, m_debugSession, GetActiveID(), m_viewRedundantSubgraphs, m_viewPassthroughVertices);
			//	}
				break;
			}
		}
		break;
	case DTIMER_LAYOUT:
		{
			if (!m_frmGlobalGraph.IsWindowEnabled() || !m_frmActiveGraph.IsWindowEnabled())
			{
				++m_layoutProgress;
				GetIMainFrame()->SetProgressBar(ID_DEBUGGER_LAYOUTPROGRESS, m_layoutProgress, m_layoutProgress);
				GetIMainFrame()->SetProgressBar(ID_DEBUGGER_SOCKETPROGRESS, 0, m_layoutProgress);
			}
			else if (m_debugSession->GetDebugState() == CRoxieDebugSession::DEBUGSTATE_CONTINUE || m_debugSession->GetActiveThreadCount() > 0)
			{
				++m_layoutProgress;
				GetIMainFrame()->SetProgressBar(ID_DEBUGGER_SOCKETPROGRESS, m_layoutProgress, m_layoutProgress);
				GetIMainFrame()->SetProgressBar(ID_DEBUGGER_LAYOUTPROGRESS, 0, m_layoutProgress);
			}
			else
			{
				m_layoutProgress = 0;
				GetIMainFrame()->SetProgressBar(ID_DEBUGGER_LAYOUTPROGRESS, 0, m_layoutProgress);
				GetIMainFrame()->SetProgressBar(ID_DEBUGGER_SOCKETPROGRESS, 0, m_layoutProgress);
			}
		}
		break;
	}
	//CMDIFrameWndEx::OnTimer(nIDEvent);
}

//LRESULT CDebugView::OnFindReplace(WPARAM wParam, LPARAM lParam)
//{
//	LPFINDREPLACE fr = (LPFINDREPLACE)lParam;
//    if (((LPFINDREPLACE)lParam)->Flags & FR_FINDNEXT)
//    {
//		bool matchCase = (fr->Flags & FR_MATCHCASE) == FR_MATCHCASE;
//		bool matchWholeWord = (fr->Flags & FR_WHOLEWORD) == FR_WHOLEWORD;
//
//		if (CDebugGraphView * activeWnd = GetActiveGraphFrame())
//			activeWnd->DoFind(fr->lpstrFindWhat, matchCase, matchWholeWord);
//
//		m_searchString = fr->lpstrFindWhat;
//    }
//	else if (fr->Flags & FR_DIALOGTERM)
//		m_fr = NULL;
//
//    return 0;
//}

void CDebugView::OnPaneClose(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (wndCtl == m_paneGlobal)
		m_splitter.SetSplitterPosPct(0);
	else if (wndCtl == m_paneActive)
		m_splitter.SetSplitterPosPct(100);
}

void CDebugView::OnEditFind(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (CDebugGraphView * activeWnd = GetActiveGraphFrame())
		activeWnd->DoEditFind();
}

LRESULT CDebugView::OnFindReplaceCmd(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SetMsgHandled(true);
	if (wParam != ID_EDIT_FINDREPLACECMD)
	{
		SetMsgHandled(false);
		//bHandled = FALSE;
		return 1;
	}

	if(GetFindReplace().FindNext())
	{
		if (CDebugGraphView * activeWnd = GetActiveGraphFrame())
			activeWnd->DoEditFindAgain(GetFindReplace().SearchDown() == TRUE);
	}
	return 0;
}
void CDebugView::OnEditFindNext(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (CDebugGraphView * activeWnd = GetActiveGraphFrame())
		activeWnd->DoEditFindAgain(true);
}

void CDebugView::OnEditFindPrevious(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (CDebugGraphView * activeWnd = GetActiveGraphFrame())
		activeWnd->DoEditFindAgain(false);
}

void CDebugView::OnEditGoto(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	static int prevLine = 1;
	std::_tstring id;
	CDebugGraphView * activeWnd = GetActiveGraphFrame();
	if (activeWnd && IDOK == DoModalGotoID(id))
	{
		if (activeWnd->m_view.GetVertex(CUniqueID(guidDefault, XGMML_CAT_VERTEX, id)))
			activeWnd->EnsureVisible(CUniqueID(guidDefault, XGMML_CAT_VERTEX, id), true, true);
		else if (activeWnd->m_view.GetEdge(CUniqueID(guidDefault, XGMML_CAT_EDGE, id)))
			activeWnd->EnsureVisible(CUniqueID(guidDefault, XGMML_CAT_EDGE, id), true, true);
		else if (activeWnd->m_view.GetSubgraph(CUniqueID(guidDefault, XGMML_CAT_SUBGRAPH, id)))
			activeWnd->EnsureVisible(CUniqueID(guidDefault, XGMML_CAT_SUBGRAPH, id), true);
	}
}

void CDebugView::OnButtonContinue(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_debugSession->Continue();
}

void CDebugView::OnButtonDetach(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_debugSession->Detach();
}

void CDebugView::OnButtonBreak(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_debugSession->BreakAll();
}

void CDebugView::OnButtonAbort(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_debugSession->Abort();
}

void CDebugView::OnButtonRestart(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	WTL::CWaitCursor wait;
	SetSupressRefresh(true);
	StlLinked<Dali::IDali> server = Dali::AttachDali(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT), _T("Dali"));
	Dali::IWorkunitVector wus;
	wus.push_back(m_wu.p);
	server->AbortWorkunits(&wus);
	while(!m_wu->IsComplete())
	{
		m_wu->Refresh(true);
		::Sleep(200);
	}
	server->ResubmitWorkunits(&wus);
	while(!m_wu->IsDebugging())
	{
		server->GetWorkunit(m_wu->GetWuid(), true);  //Force Refresh (m_wu->Refresh will not work for complete WUs)
		::Sleep(200);
	}
	SetSupressRefresh(false);

	Init();
}

void CDebugView::OnButtonStep(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_debugSession->Step();
}

void CDebugView::OnButtonStepGraph(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_debugSession->StepGraph();
}

void CDebugView::OnButtonStepGraphStart(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_debugSession->StepGraphStart();
}

void CDebugView::OnButtonStepGraphEnd(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_debugSession->StepGraphEnd();
}

void CDebugView::OnButtonNext(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_debugSession->Next();
}

void CDebugView::DoAddBreakpoint(const CUniqueID & id)
{
	CDebugGraphView * activeWnd = GetActiveGraphFrame();
	CDialogAddBreakpoint dlg;
	if (activeWnd && dlg.DoModal() == IDOK)
	{
		m_debugSession->AddBreakpoint(id.isEmpty() ? _T("") : id.GetID(), dlg.m_stringMode.c_str(), dlg.m_stringConditionMode.c_str(), dlg.m_stringField.c_str(), dlg.m_stringCondition.c_str(), dlg.m_stringCountMode.c_str(), dlg.m_stringCount.c_str(), dlg.m_caseSensitive != 0);
	}
}

CDebugGraphView * CDebugView::GetActiveGraphFrame()
{
	switch (m_splitter.GetActivePane())
	{
	case SPLIT_PANE_NONE:
		break;
	case 0:
		return &m_frmGlobalGraph;
		break;
	case 1:
		return &m_frmActiveGraph;
		break;
	}
	return m_prevActiveWnd;
}

void CDebugView::OnButtonSetGlobalBreakpoint(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	DoAddBreakpoint(CUniqueID());
}

void CDebugView::OnButtonSetBreakpoint(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	DoAddBreakpoint(m_debugSession->GetCurrentID());
}

void CDebugView::OnButtonRemoveBreakpoint(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (CDebugGraphView * activeWnd = GetActiveGraphFrame())
	{
		CUniqueIDVector edges;
		activeWnd->GetSelectedEdges(&edges);
		edges.Reset();
		while(edges.MoveNext())
		{
			m_debugSession->DeleteBreakpoint(edges.Get().GetID());
		}
	}
}

void CDebugView::OnButtonRemoveAllBreakpoints(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_debugSession->DeleteAllBreakpoints();
}

void CDebugView::OnSliderZoom(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> ar;
	GetIMainFrame()->m_wndRibbonBar.GetElementsByID(ID_DEBUGGER_ZOOM, ar);
	ATLASSERT(ar.GetSize() == 1);
	CMFCRibbonSlider* pSlider = DYNAMIC_DOWNCAST(CMFCRibbonSlider, ar[0]);
	ATLASSERT(pSlider);
	if (CDebugGraphView * activeWnd = GetActiveGraphFrame())
		activeWnd->SetZoom(pSlider->GetPos());
}

void CDebugView::OnButtonRecenter(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (CDebugGraphView * activeWnd = GetActiveGraphFrame())
	{
		activeWnd->EnsureVisible(m_debugSession->GetActiveID(), true);
	}
}

void CDebugView::OnButtonLayout(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (CDebugGraphView * activeWnd = GetActiveGraphFrame())
	{
		activeWnd->EnableWindow(false);
		activeWnd->m_view.SetLayout(SGV::LAYOUT_DOT);
	}
}

void CDebugView::OnViewGlobalRunningonly(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (CDebugGraphView * activeWnd = &m_frmGlobalGraph)
	{
		activeWnd->m_viewRunningOnly = !activeWnd->m_viewRunningOnly;
		activeWnd->CalcVisibilityAndLayout(m_debugSession->GetActiveID());
	}
}

void CDebugView::OnViewGlobalFoundonly(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (CDebugGraphView * activeWnd = &m_frmGlobalGraph)
	{
		activeWnd->m_viewFoundOnly = !activeWnd->m_viewFoundOnly;
		activeWnd->CalcVisibilityAndLayout(m_debugSession->GetActiveID());
	}
}

void CDebugView::OnViewActiveRunningonly(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (CDebugGraphView * activeWnd = &m_frmActiveGraph)
	{
		activeWnd->m_viewRunningOnly = !activeWnd->m_viewRunningOnly;
		activeWnd->CalcVisibilityAndLayout(m_debugSession->GetActiveID());
	}
}

void CDebugView::OnViewActiveFoundonly(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (CDebugGraphView * activeWnd = &m_frmActiveGraph)
	{
		activeWnd->m_viewFoundOnly = !activeWnd->m_viewFoundOnly;
		activeWnd->CalcVisibilityAndLayout(m_debugSession->GetActiveID());
	}
}

void CDebugView::OnBnClickedSearch(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	GetIMainFrame()->m_debugSearchView->UpdateData();
	m_debugSession->Search((const TCHAR *)m_searchConditions.m_field, (const TCHAR *)m_searchConditions.m_condition, (const TCHAR *)m_searchConditions.m_value, m_searchConditions.m_caseSensitive != 0, false);
}

void CDebugView::OnBnClickedClearSearch(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	GetIMainFrame()->m_debugSearchView->UpdateData();
	//Quick hack to clear search...
	m_debugSession->Search(_T("{8004AA4E-45CE-4655-B7CF-D586BB5DCFF8}"), _T("equals"), _T("{E3C8150C-CF83-491a-8185-012A14766AC1}"), true, false);
}


//CTabPane
HWND CDebugView::GetHWND()
{
	return m_hWnd;
}

bool CDebugView::CanCopy()
{
	return true;
}
bool CDebugView::HasResults() const
{
	return false;
}

//ISearchable
bool CDebugView::CanBeSearched() const
{
	return HasResults();
}

bool CDebugView::Search(const CSearchableInfo &/*info*/, CSearchableResult &/*result*/) const
{
	return false;
}

void CDebugView::UpdateDisplay()
{
	CComPtr<clib::scoped_lock_ref_counted> lock;
	m_wu->Lock(lock);
	for(int i = 0; i < m_wu->GetExceptionCount(); ++i)
	{
	}
	DoDataExchange();
}

//  --  IResultView  ---
HWND CDebugView::GetHwnd() const 
{
	return m_hWnd;
}
