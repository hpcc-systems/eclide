#pragma once

#include "ResultView.h"
#include "TabPane.h"
#include "GraphView2.h"
#include "Notifications.h"
#include "CtlColor.h"
#include "XGMMLSaxHandler.h"
#include <CustomMessages.h>

enum UMGV3
{
	UMGV3_FIRST = CWM_LAST + 1,
	UMGV3_WU_UPDATE, 
	UMGV3_GRAPH_UPDATE, 
	UMGV3_INITIALIZE1,
	UMGV3_INITIALIZE2,
	UMGV3_INITIALIZE3,
	UMGV3_INITIALIZE4,
	UMGV3_LAST
};
//  ===========================================================================
class CGraphView3 : 
	public WTL::CAxDialogImpl<CGraphView3>,
	public WTL::CDialogResize<CGraphView3>,
	public WTL::CWinDataExchange<CGraphView3>,
	public IDispEventSimpleImpl<IDC_STATIC_PLACEHOLDER, CGraphView3, &DIID_IFBComEventSource>,
	public CTabPane,
	public IResultView,
	public CCtlColor,
	public boost::signals::trackable
{
	typedef CGraphView3 thisClass;
	typedef WTL::CAxDialogImpl<CGraphView3> baseClass;

protected:
	CComPtr<Dali::IWorkunit> m_wu;
	CComPtr<Dali::IGraph> m_graph;
	IResultSlot *m_resultSlot;
	boost::signals::connection m_graphConnection;

	WTL::CSplitterWindow m_splitterV;
	WTL::CHorSplitterWindow m_splitterH;
	WTL::CHorSplitterWindow m_splitterH2;
	CGraphViewCtrl2 m_wndLNGVC; 
	WTL::CTrackBarCtrl m_view_zoom;
	WTL::CSortListViewCtrl m_listGraph;
	WTL::CSortListViewCtrl m_listTimings;
	WTL::CSortListViewCtrl m_listAttributes;

	bool m_loaded;
	unsigned m_mergeCount;

	CString m_prevFind;
	BOOL m_prevMatchCase;
	BOOL m_prevWholeWord;
	BOOL m_prevSearchTooltips;
	CUniqueIDVector m_found;

	WUVisualState DecodeState(Dali::WUState state);

	CUniqueID m_notificationItem;
	int m_actionID;

	bool m_buildingTree;
	bool m_buildingEdgeVisibility;

	int m_timerCountdown;
	int m_prevTimerCountdown;
	bool m_followActive;

	std::_tstring m_prevGraph;

public:
	enum {IDD = IDD_GRAPHVIEW};

	CGraphView3(Dali::IWorkunit * wu, IResultSlot *resultSlot);

	void LoadGraph();

	BOOL PreTranslateMessage(MSG* pMsg);

	bool DoFileOpen(const CString & path);
	void Clear();

	bool MinimizeAll(bool minimized = true, const CUniqueID & except = CUniqueID());
	void DoMinClicked(const CUniqueID & id, bool expand);
	void CenterOnItem(const CUniqueID & id);
	void CenterOnTiming(const Dali::CGraphTiming * timing);
	void GetSelectedEdges(IUniqueIDContainer * results);
	Dali::IGraph * GetSelectedGraph();
	void SelectRunningGraph();
	void SelectRecentTimingGraph();

	bool DoQuerySave()
	{
		return true;
	}

	BEGIN_CUNKNOWN
		IMPLEMENT_INTERFACE(IResultView)
	END_CUNKNOWN(CTabPane)

	BEGIN_MSG_MAP(thisClass)
		MSG_WM_INITDIALOG(OnInitDialog);
		MSG_WM_DESTROY(OnDestroy);

		MESSAGE_HANDLER_EX(UMGV3_WU_UPDATE, OnWuUpdate);
		MESSAGE_HANDLER_EX(UMGV3_GRAPH_UPDATE, OnGraphUpdate);

		NOTIFY_CODE_HANDLER(SCW_NOTIFYMINCLICKED, OnNotifyMinClicked)
		//NOTIFY_CODE_HANDLER(SCW_NOTIFYLAYOUTFINISHED, OnLayoutFinished)
		NOTIFY_CODE_HANDLER(SCW_NOTIFYGRAPHCONTEXT, OnGraphContext)
		NOTIFY_CODE_HANDLER(SCW_NOTIFYSUBGRAPHCONTEXT, OnSubgraphContext)
		NOTIFY_CODE_HANDLER(SCW_NOTIFYVERTEXCONTEXT, OnVertexContext)
		NOTIFY_CODE_HANDLER(SCW_NOTIFYEDGECONTEXT, OnEdgeContext)
		NOTIFY_CODE_HANDLER(SCW_NOTIFYCALCTOOLTIP, OnCalcTooltip)
		NOTIFY_CODE_HANDLER(SCW_NOTIFYTOOLTIPLINKCLICK, OnTooltipLinkClick)

		MSG_WM_CONTEXTMENU(OnContextMenu);
		MSG_WM_TIMER(OnTimer);

		MSG_WM_VSCROLL(OnVScroll);
		MSG_WM_FORWARDMSG(OnForwardMsg)

		MSG_WM_MOUSEWHEEL(OnMouseWheel)

		COMMAND_ID_HANDLER(ID_EDIT_FIND, OnEditFind)
		COMMAND_ID_HANDLER(ID_EDIT_FINDNEXT, OnEditFindNext)
		COMMAND_ID_HANDLER(ID_EDIT_FINDPREV, OnEditFindPrev)
		MESSAGE_HANDLER(WM_COMMAND, OnFindReplaceCmd)
		COMMAND_ID_HANDLER(ID_EDIT_GOTO, OnEditGoto)

		COMMAND_ID_HANDLER(ID_BROWSER_BACK, OnBrowserBack);
		COMMAND_ID_HANDLER(ID_BROWSER_FORWARD, OnBrowserForward);

		COMMAND_ID_HANDLER(ID_GRAPH_ZOOMTOFIT, OnGraphZoomToFit);
		COMMAND_ID_HANDLER(ID_GRAPH_ZOOMTOWIDTH, OnGraphZoomToWidth);
		COMMAND_ID_HANDLER(ID_GRAPH_LAYOUT, OnGraphLayout);

		COMMAND_ID_HANDLER(ID_GRAPH_REFRESH, OnGraphRefresh);
		COMMAND_ID_HANDLER(ID_GRAPH_FOLLOWACTIVE, OnGraphFollowActive);

		NOTIFY_CODE_HANDLER(LVN_ITEMCHANGED, OnListItemChanged)
		NOTIFY_CODE_HANDLER(LVN_DELETEITEM, OnListItemDeleted)

		CHAIN_MSG_MAP(CCtlColor)
		REFLECT_NOTIFICATIONS()
		CHAIN_MSG_MAP(WTL::CDialogResize<thisClass>)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(thisClass)
		DLGRESIZE_CONTROL(IDC_STATIC_PLACEHOLDER, DLSZ_SIZE_X | DLSZ_SIZE_Y)
		DLGRESIZE_CONTROL(IDC_SLIDER_ZOOM, DLSZ_MOVE_X | DLSZ_SIZE_Y)
	END_DLGRESIZE_MAP()

	BEGIN_DDX_MAP(thisClass)
	END_DDX_MAP()

	BEGIN_SINK_MAP(thisClass)
		SINK_ENTRY_EX(IDC_STATIC_PLACEHOLDER, DIID_IFBComEventSource, g_dispScaled, OnScaleChanged)
		SINK_ENTRY_EX(IDC_STATIC_PLACEHOLDER, DIID_IFBComEventSource, g_dispLayoutFinished, OnLayoutFinished)
		SINK_ENTRY_EX(IDC_STATIC_PLACEHOLDER, DIID_IFBComEventSource, g_dispMouseDoubleClick, OnMouseDoubleClick)
		SINK_ENTRY_EX(IDC_STATIC_PLACEHOLDER, DIID_IFBComEventSource, g_dispSelectionChanged, OnSelChanged)
	END_SINK_MAP()

	bool InitGraphControl();
	LRESULT OnInitDialog(HWND /*hWnd*/, LPARAM /*lParam*/);
	void OnDestroy();
	LRESULT OnWuUpdate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
	LRESULT OnGraphUpdate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);

	LRESULT OnNotifyMinClicked(int nId, LPNMHDR nmhdr, BOOL& bHandled); 
	//LRESULT OnLayoutFinished(int nId, LPNMHDR nmhdr, BOOL& bHandled); 
	LRESULT OnGraphContext(int nId, LPNMHDR nmhdr, BOOL& bHandled); 
	LRESULT OnSubgraphContext(int nId, LPNMHDR nmhdr, BOOL& bHandled); 
	LRESULT OnVertexContext(int nId, LPNMHDR nmhdr, BOOL& bHandled); 
	LRESULT OnEdgeContext(int nId, LPNMHDR nmhdr, BOOL& bHandled); 
	LRESULT OnCalcTooltip(int nId, LPNMHDR nmhdr, BOOL& bHandled); 
	LRESULT OnTooltipLinkClick(int nId, LPNMHDR nmhdr, BOOL& bHandled); 

	void OnContextMenu(HWND /*hWnd*/, CPoint pt);
	void OnTimer(UINT_PTR nIDEvent);
	void OnVScroll(int scrollRequest, short scrollPos, HWND hWnd);
	BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	BOOL OnForwardMsg(LPMSG Msg, DWORD nUserData);

	void DoEditFind();
	LRESULT OnEditFind(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFindReplaceCmd(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled);
	void DoEditFindAgain(bool /*bNext*/);
	LRESULT OnEditFindNext(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnEditFindPrev(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnEditGoto(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	
	LRESULT OnBrowserBack(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBrowserForward(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnGraphZoomToFit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnGraphZoomToWidth(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnGraphLayout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnGraphRefresh(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnGraphFollowActive(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnListItemChanged(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& bHandled);
	LRESULT OnListItemDeleted(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& bHandled);
	
	HRESULT GetFuncInfoFromId(const IID& iid, DISPID dispidMember, LCID lcid, _ATL_FUNC_INFO& info);
	STDMETHOD(OnScaleChanged)(int val, IDispatch * oSender);
	STDMETHOD(OnLayoutFinished)(IDispatch * oSender);
	STDMETHOD(OnMouseDoubleClick)(int item, int keyState, IDispatch * oSender);
	STDMETHOD(OnSelChanged)(VARIANT selection, IDispatch * oSender);

	//CTabPane
	virtual HWND GetHWND();
	bool CanCopy();
	bool HasResults() const;
	bool UpdateUI(CCmdUI * cui);

	//ISearchable
	bool CanBeSearched() const;
	bool Search(const CSearchableInfo &/*info*/, CSearchableResult &/*result*/) const;

	//Callbacks
	void operator()(Dali::WUEvent evt, Dali::IWorkunit * wu);
	void operator()(Dali::IGraph * graph);

	//  ---  IResultView  ---
	HWND GetHwnd() const; 
};
