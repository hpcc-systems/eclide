#pragma once

#include "ResultView.h"
#include "TabPane.h"
#include "atlGraphView.h"
#include "Notifications.h"
#include "CtlColor.h"
#include "XGMMLSaxHandler.h"
#include <CustomMessages.h>

enum UMGV
{
	UMGV_FIRST = CWM_LAST + 1,
	UMGV_WU_UPDATE, 
	UMGV_GRAPH_UPDATE, 
	UMGV_INITIALIZE1,
	UMGV_INITIALIZE2,
	UMGV_INITIALIZE3,
	UMGV_INITIALIZE4,
	UMGV_LAST
};
//  ===========================================================================
class CGraphView : 
	public WTL::CDialogImpl<CGraphView>,
	public WTL::CDialogResize<CGraphView>,
	public WTL::CWinDataExchange<CGraphView>,
	public CTabPane,
	public IResultView,
	public CCtlColor,
	public boost::signals::trackable
{
	typedef CGraphView thisClass;
	typedef WTL::CDialogImpl<CGraphView> baseClass;

protected:
	CComPtr<Dali::IWorkunit> m_wu;
	CComPtr<Dali::IGraph> m_graph;
	IResultSlot *m_resultSlot;
	boost::signals::connection m_graphConnection;

	WTL::CSplitterWindow m_splitterV;
	WTL::CHorSplitterWindow m_splitterH;
	WTL::CHorSplitterWindow m_splitterH2;
	SGV::CGraphViewCtrl m_wndLNGVC; 
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
	bool m_minimizeInactive;

	std::_tstring m_prevGraph;

public:
	enum {IDD = IDD_GRAPHVIEW};

	CGraphView(Dali::IWorkunit * wu, IResultSlot *resultSlot);

	void LoadGraph();

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		return baseClass::IsDialogMessage(pMsg);
	}

	bool DoFileOpen(const CString & path);
	void Clear();

	bool MinimizeAll(bool minimized = true, const CUniqueID & except = CUniqueID());
	void DoMinClicked(const CUniqueID & id, bool expand);
	void CenterOnItem(const CUniqueID & id);
	void CenterOnTiming(const Dali::CGraphTiming * timing);
	void GetSelectedEdges(IUniqueIDContainer * results);
	Dali::IGraph * GetSelectedGraph();
	void SelectRunningGraph();

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

		MESSAGE_HANDLER_EX(UMGV_WU_UPDATE, OnWuUpdate);
		MESSAGE_HANDLER_EX(UMGV_GRAPH_UPDATE, OnGraphUpdate);

		NOTIFY_CODE_HANDLER(SCW_NOTIFYMINCLICKED, OnNotifyMinClicked)
		NOTIFY_CODE_HANDLER(SCW_NOTIFYLAYOUTFINISHED, OnLayoutFinished)
		NOTIFY_CODE_HANDLER(SCW_NOTIFYGRAPHCONTEXT, OnGraphContext)
		NOTIFY_CODE_HANDLER(SCW_NOTIFYSUBGRAPHCONTEXT, OnSubgraphContext)
		NOTIFY_CODE_HANDLER(SCW_NOTIFYVERTEXCONTEXT, OnVertexContext)
		NOTIFY_CODE_HANDLER(SCW_NOTIFYEDGECONTEXT, OnEdgeContext)
		NOTIFY_CODE_HANDLER(SCW_NOTIFYSELECTED, OnSelChanged)
		NOTIFY_CODE_HANDLER(SCW_NOTIFYSCALECHANGE, OnScaleChanged)
		NOTIFY_CODE_HANDLER(SCW_NOTIFYCALCTOOLTIP, OnCalcTooltip)
		NOTIFY_CODE_HANDLER(SCW_NOTIFYTOOLTIPLINKCLICK, OnTooltipLinkClick)

		MSG_WM_CONTEXTMENU(OnContextMenu);
		MSG_WM_TIMER(OnTimer);

		MSG_WM_VSCROLL(OnVScroll);
		MSG_WM_FORWARDMSG(OnForwardMsg)

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
		COMMAND_ID_HANDLER(ID_GRAPH_MINIMIZEINACTIVE, OnGraphMinimizeInactive);

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

	LRESULT OnInitDialog(HWND /*hWnd*/, LPARAM /*lParam*/);
	void OnDestroy();
	LRESULT OnWuUpdate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
	LRESULT OnGraphUpdate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);

	LRESULT OnNotifyMinClicked(int nId, LPNMHDR nmhdr, BOOL& bHandled); 
	LRESULT OnLayoutFinished(int nId, LPNMHDR nmhdr, BOOL& bHandled); 
	LRESULT OnGraphContext(int nId, LPNMHDR nmhdr, BOOL& bHandled); 
	LRESULT OnSubgraphContext(int nId, LPNMHDR nmhdr, BOOL& bHandled); 
	LRESULT OnVertexContext(int nId, LPNMHDR nmhdr, BOOL& bHandled); 
	LRESULT OnEdgeContext(int nId, LPNMHDR nmhdr, BOOL& bHandled); 
	LRESULT OnSelChanged(int nId, LPNMHDR nmhdr, BOOL& bHandled); 
	LRESULT OnScaleChanged(int nId, LPNMHDR nmhdr, BOOL& bHandled); 
	LRESULT OnCalcTooltip(int nId, LPNMHDR nmhdr, BOOL& bHandled); 
	LRESULT OnTooltipLinkClick(int nId, LPNMHDR nmhdr, BOOL& bHandled); 

	void OnContextMenu(HWND /*hWnd*/, CPoint pt);
	void OnTimer(UINT_PTR nIDEvent);
	void OnVScroll(int scrollRequest, short scrollPos, HWND hWnd);
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
	LRESULT OnGraphMinimizeInactive(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnListItemChanged(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& bHandled);
	LRESULT OnListItemDeleted(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& bHandled);

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
