#pragma once

#include "CtlColor.h"
#include "ResultView.h"
#include "TabPane.h"
#include "DebugGraphView.h"
#include <CustomMessages.h>

// CDebugView
enum DUM
{
	DUM_FIRST = CWM_LAST + 1,
	DUM_INITIALIZE,
	DUM_LOADGRAPH,
	DUM_CALLSTART,
	DUM_CALLEND,
	DUM_LAST
};

class CDebugView : 
	public WTL::CDialogImpl<CDebugView>,
	public WTL::CDialogResize<CDebugView>,
	public WTL::CWinDataExchange<CDebugView>,
	public CTabPane,
	public CCtlColor,
	public IGraphViewSlot, 
	public IRoxieDebugSessionReceiver,
	public IResultView
{
	typedef CDebugView thisClass;
	typedef WTL::CDialogImpl<thisClass> baseClass;
private:
	mutable clib::recursive_mutex m_mutex;

	CComPtr<Dali::IWorkunit> m_wu;
	WTL::CSplitterWindow m_splitter;
	WTL::CPaneContainer m_paneGlobal;
	CDebugGraphView m_frmGlobalGraph;
	WTL::CPaneContainer m_paneActive;
	CDebugGraphView m_frmActiveGraph;
	CDebugGraphView * m_prevActiveWnd;

	IResultSlot *m_resultSlot;
	std::string m_ip;
	std::string m_port;
	std::_tstring m_queryLabel;
	std::_tstring m_queryParams;
	std::_tstring m_captionText;
	UINT_PTR m_nTimerDetach;
	UINT_PTR m_nTimerLayout;
	int m_layoutProgress;

	std::_tstring m_xgmmlAll;
	std::_tstring m_xgmmlActive;

	roxie::Response m_sessionResponse;  //Only used to catch failed sessions.

	enum SHOW_MODE
	{
		SHOW_MODE_ALL,
		SHOW_MODE_ACTIVE,
		SHOW_MODE_OPTIMIZED
	};
	SHOW_MODE m_showMode;

	StlLinked<CRoxieDebugSession> m_debugSession;
	int m_sequence;
	int m_sequenceGraph;
	CUniqueID m_active;
	CUniqueID m_selected;
	bool m_popupExceptions;
	bool m_viewRecenterOnActive;
	bool m_viewZoomactiveToFit;
	bool m_viewDebugAttributes;
	bool m_viewRedundantSubgraphs;
	bool m_viewPassthroughVertices;
	bool m_supressRefresh;

	GlobalSearchConditions m_searchConditions;

	void PostCallStart(roxie::CALL callMode);
	void PostCallEnd(roxie::CALL callMode, const roxie::Response & response);
	void DoAddBreakpoint(const CUniqueID & id);
	CDebugGraphView * GetActiveGraphFrame();

	//  IGraphViewSlot  ---
	const CUniqueID & GetCurrentID();
	void NewSelection(const CDebugGraphView * from, const CUniqueID & newSel, bool recenterGraph, bool forceDataLoad, bool dontShowDefinition = false);
	void ScaleChanged(int zoom);
	unsigned int ProcessContextMenu(WTL::CMenu & menu, CPoint & pt, CUniqueIDVector & subgraphs);
	unsigned int ProcessContextMenu(WTL::CMenu & menu, CPoint & pt, IGraphSubgraph * subgraph);
	unsigned int ProcessContextMenu(WTL::CMenu & menu, CPoint & pt, SGV::CGraphVertex & vertex);
	unsigned int ProcessContextMenu(WTL::CMenu & menu, CPoint & pt, SGV::CGraphEdge & edge);
	void DeleteBreakpoint(const ::CString & idx);
	void GetAllBreakpoints();
	const roxie::CSearchMap & GetSearchResults() const;
	const CUniqueID & GetGlobalID(const CUniqueID & id) const;
	GlobalSearchConditions * GetGlobalSearchConditions();
	const SGV::CGraphViewCtrl & GetGlobalGraphView();
	const SGV::CGraphViewCtrl & GetActiveGraphView();
	Dali::IWorkunit * GetWorkunit();
	CRoxieDebugSession * GetDebugSession();

public:
	enum {IDD = IDD_DEBUGVIEW};

	CDebugView(Dali::IWorkunit * wu, IResultSlot *resultSlot);
	~CDebugView();

	void Init();
	bool UpdateUI(CCmdUI * cui);
	void SetSupressRefresh(bool supress);
	bool SupressRefresh() const;

	BEGIN_CUNKNOWN
		IMPLEMENT_INTERFACE(IResultView)
	END_CUNKNOWN(CTabPane)

	BEGIN_MSG_MAP(thisClass)
		MSG_WM_INITDIALOG(OnInitDialog)
		MESSAGE_HANDLER_EX(CWM_INITIALIZE, OnInitialize)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_CONTEXTMENU(OnContextMenu)
		MSG_WM_SETFOCUS(OnSetFocus)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)

		MESSAGE_HANDLER_EX(DUM_CALLSTART, OnCallStart)
		MESSAGE_HANDLER_EX(DUM_CALLEND, OnCallEnd)

		MSG_WM_TIMER(OnTimer)

		COMMAND_ID_HANDLER_EX(ID_PANE_CLOSE, OnPaneClose)

		COMMAND_ID_HANDLER_EX(ID_EDIT_FIND, OnEditFind)
		COMMAND_ID_HANDLER_EX(ID_EDIT_FINDNEXT, OnEditFindNext)
		COMMAND_ID_HANDLER_EX(ID_EDIT_FINDPREV, OnEditFindPrevious)
		MESSAGE_HANDLER_EX(WM_COMMAND, OnFindReplaceCmd)
		COMMAND_ID_HANDLER_EX(ID_EDIT_GOTO, OnEditGoto)

		COMMAND_ID_HANDLER_EX(ID_DEBUGGER_ZOOM, OnSliderZoom)
		COMMAND_ID_HANDLER_EX(ID_DEBUGGER_RECENTER, OnButtonRecenter)
		COMMAND_ID_HANDLER_EX(ID_DEBUGGER_LAYOUT, OnButtonLayout)

		COMMAND_ID_HANDLER_EX(ID_DEBUGGER_CONTINUE, OnButtonContinue)
		COMMAND_ID_HANDLER_EX(ID_DEBUGGER_DETACH, OnButtonDetach)
		COMMAND_ID_HANDLER_EX(ID_DEBUGGER_BREAK, OnButtonBreak)
		COMMAND_ID_HANDLER_EX(ID_DEBUGGER_ABORT, OnButtonAbort)
		COMMAND_ID_HANDLER_EX(ID_DEBUGGER_RESTART, OnButtonRestart)

		COMMAND_ID_HANDLER_EX(ID_DEBUGGER_STEP, OnButtonStep)
		COMMAND_ID_HANDLER_EX(ID_DEBUGGER_NEXT, OnButtonNext)

		COMMAND_ID_HANDLER_EX(ID_DEBUGGER_STEPGRAPH, OnButtonStepGraph)
		COMMAND_ID_HANDLER_EX(ID_DEBUGGER_STEPGRAPHSTART, OnButtonStepGraphStart)
		COMMAND_ID_HANDLER_EX(ID_DEBUGGER_STEPGRAPHEND, OnButtonStepGraphEnd)

		COMMAND_ID_HANDLER_EX(ID_DEBUGGER_SETGLOBALBREAKPOINT, OnButtonSetGlobalBreakpoint)
		COMMAND_ID_HANDLER_EX(ID_DEBUGGER_SETBREAKPOINT, OnButtonSetBreakpoint)
		COMMAND_ID_HANDLER_EX(ID_DEBUGGER_REMOVEBREAKPOINT, OnButtonRemoveBreakpoint)
		COMMAND_ID_HANDLER_EX(ID_DEBUGGER_REMOVEALLBREAKPOINTS, OnButtonRemoveAllBreakpoints)

		COMMAND_ID_HANDLER_EX(ID_VIEW_RUNNINGONLYGLOBAL, OnViewGlobalRunningonly)
		COMMAND_ID_HANDLER_EX(ID_VIEW_FOUNDONLYGLOBAL, OnViewGlobalFoundonly)
		COMMAND_ID_HANDLER_EX(ID_VIEW_RUNNINGONLYACTIVE, OnViewActiveRunningonly)
		COMMAND_ID_HANDLER_EX(ID_VIEW_FOUNDONLYACTIVE, OnViewActiveFoundonly)

		COMMAND_ID_HANDLER_EX(IDC_BUTTON_SEARCH, OnBnClickedSearch)
		COMMAND_ID_HANDLER_EX(IDC_BUTTON_CLEARSEARCH, OnBnClickedClearSearch)

		CHAIN_MSG_MAP(WTL::CDialogResize<thisClass>)
		CHAIN_MSG_MAP(CCtlColor)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(thisClass)
		DLGRESIZE_CONTROL(IDC_STATIC_PLACEHOLDER, DLSZ_SIZE_X | DLSZ_SIZE_Y)
	END_DLGRESIZE_MAP()

	BEGIN_DDX_MAP(thisClass)
		//DDX_TEXT(IDC_EDIT_EXCEPTION, m_exception)
	END_DDX_MAP()

	LRESULT OnInitDialog(HWND /*hWnd*/, LPARAM /*lParam*/);
	LRESULT OnInitialize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
	void OnDestroy();

	LRESULT OnCallStart(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCallEnd(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void OnTimer(UINT nIDEvent); 
	void OnContextMenu(HWND /*hWnd*/, CPoint pt);
	void OnSetFocus(CWindow wndOld);
	BOOL OnEraseBkgnd(WTL::CDCHandle dc);

	void OnPaneClose(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnEditFind(UINT uNotifyCode, int nID, CWindow wndCtl);
	LRESULT OnFindReplaceCmd(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void OnEditFindNext(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnEditFindPrevious(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnEditGoto(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnButtonContinue(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnButtonDetach(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnButtonBreak(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnButtonAbort(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnButtonRestart(UINT uNotifyCode, int nID, CWindow wndCtl);	

	void OnButtonStep(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnButtonNext(UINT uNotifyCode, int nID, CWindow wndCtl);

	void OnButtonStepGraph(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnButtonStepGraphStart(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnButtonStepGraphEnd(UINT uNotifyCode, int nID, CWindow wndCtl);

	void OnButtonSetGlobalBreakpoint(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnButtonSetBreakpoint(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnButtonRemoveBreakpoint(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnButtonRemoveAllBreakpoints(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnSliderZoom(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnButtonRecenter(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnButtonLayout(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnViewGlobalRunningonly(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnViewGlobalFoundonly(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnViewActiveRunningonly(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnViewActiveFoundonly(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnBnClickedSearch(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnBnClickedClearSearch(UINT uNotifyCode, int nID, CWindow wndCtl);
	//CTabPane
	virtual HWND GetHWND();
	bool CanCopy();
	bool HasResults() const;

	//ISearchable
	bool CanBeSearched() const;
	bool Search(const CSearchableInfo &/*info*/, CSearchableResult &/*result*/) const;
	void UpdateDisplay();

	//  --  IResultView  ---
	HWND GetHwnd() const; 
};
