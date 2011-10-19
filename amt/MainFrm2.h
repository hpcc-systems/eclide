#pragma once

#include "RepositoryCompareDlg.h"
#include "LogView.h"
#include "HtmlView.h"
#include "DiffView.h"
#include "DependeesWnd.h"

#include <sstate.h>
//#include "SourceViewer.h"
#include "..\EditLog\EditLog.h"
#include "..\EditLog\EditLog_Stream.h"

#include <FindReplace.h>
#include <Combo.h>	//wlib
#include <CtlColor.h>	//wlib

#include "TabbedFrame.h"
#include "..\eclide\TabbedChildWindowEx.h"
#include "SplitterWnd.h"

enum UM2
{
	UM2_FIRST = UM_LAST + 1,
	UM2_GETECLLOADED,
	UM2_LAST
};

class CMyTabView : public WTL::CTabViewImpl<CMyTabView>
{
	typedef CMyTabView thisClass;
	typedef WTL::CTabViewImpl<CMyTabView> baseClass;
public:
	DECLARE_WND_CLASS_EX(_T("WTL_MyTabView"), 0, COLOR_APPWORKSPACE)

	BEGIN_MSG_MAP(thisClass)
		//MSG_WM_DRAWITEM(OnDrawItem)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		MSG_WM_CTLCOLORSTATIC(OnCtlColorStatic)
		CHAIN_MSG_MAP(baseClass)
	ALT_MSG_MAP(1)   // tab control
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd2)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnCtlColorStatic2)
		CHAIN_MSG_MAP_ALT(baseClass, 1)
	END_MSG_MAP()

	void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
	{
		SetMsgHandled(false);
	}

	LRESULT OnEraseBkgnd2(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		WTL::CDCHandle dc = (HDC)wParam;
		CRect rect;
		GetClientRect(rect);
		dc.FillRect(rect, afxGlobalData.brBarFace);
		return TRUE;
	}

	LRESULT OnCtlColorStatic2(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		WTL::CDCHandle dc = (HDC)wParam;
		dc.SetTextColor(afxGlobalData.clrBarText);
		dc.SetBkMode(TRANSPARENT);
		return (LRESULT)(HBRUSH)afxGlobalData.brBarFace;
	}

	void UpdateLayout()
	{
		RECT rect;
		GetClientRect(&rect);

		if (m_tab.IsWindow() && ((m_tab.GetStyle() & WS_VISIBLE) != 0))
			m_tab.SetWindowPos(NULL, 0, 0, rect.right - rect.left, m_cyTabHeight, SWP_NOZORDER);

		if (m_nActivePage != -1)
			::SetWindowPos(GetPageHWND(m_nActivePage), NULL, 0, m_cyTabHeight, rect.right - rect.left, rect.bottom - rect.top - m_cyTabHeight, SWP_NOZORDER);
	}

	BOOL OnEraseBkgnd(WTL::CDCHandle dc)
	{
		CRect rect;
		GetClientRect(rect);
		dc.FillRect(rect, afxGlobalData.brBarFace);
		return TRUE;
	}

	HBRUSH OnCtlColorStatic(WTL::CDCHandle dc, WTL::CStatic wndStatic)
	{
		dc.SetTextColor(afxGlobalData.clrBarText);
		dc.SetBkMode(TRANSPARENT);
		return afxGlobalData.brBarFace;
	}
};
//  ===========================================================================
class CMainFrame : public CMDIFrameWndEx,
					public ILoginConfigPreferences,
					public CEclCommandMixin<CMainFrame>,
					public CSourceSlotImpl,
					public IFindReplace
{
	
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CMFCRibbonBar     m_wndRibbonBar;
	CMFCRibbonApplicationButton m_MainButton;
	CMFCToolBarImages m_PanelImages;
	CMFCRibbonStatusBar  m_wndStatusBar;

	CMySplitterWindow<false> m_splitter;
	CRepositoryCompareDlg m_repositoryDlg;

	CTabbedChildWindowEx<CColDotNetButtonTabCtrl<CTabViewTabItem> > m_detailSheetView;
	CSourceCtrl m_sourceEclView;
	CDiffView m_diffView;
	CSourceCtrl m_targetEclView;
	CDependeesWnd m_dependees;
	CLogView m_logView;

	CEditLog m_warnView;
	std::editstreambuf m_EditStrBuf;
	std::weditstreambuf m_EditStrBufW;
	std::basic_streambuf<char>* m_pOldBuf;
	std::basic_streambuf<wchar_t>* m_pOldBufW;
	bool m_bStreamInit;

	IConfigAdapt m_iniFile;
	CComPtr<IConfig> m_cfgLHS;
	CComPtr<IConfig> m_cfgRHS;
	STRUCT_DALI m_daliLHS;
	STRUCT_DALI m_daliRHS;

	CString m_pathLHS;
	CString m_pathRHS;

	BOOL m_bCheckBoxMatching;
	BOOL m_bCheckBoxNotMatching;
	BOOL m_bCheckBoxOrphans;

	BOOL m_bCheckBoxCRLF;
	BOOL m_bCheckBoxWhitespace;

	int m_progress;
// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg LRESULT OnInitialize(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnClearDiff(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnNewSelection(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnNewCheck(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnStatusUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnProgressUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetEclLoaded(WPARAM wParam, LPARAM lParam);
	afx_msg void OnFileLogout();
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnEditAddToRep();
	afx_msg void OnUpdateEditAddToRep(CCmdUI* pCmdUI);
	afx_msg void OnEditShow();
	afx_msg void OnEditShowMatching();
	afx_msg void OnUpdateEditShowMatching(CCmdUI* pCmdUI);
	afx_msg void OnEditShowNotMatching();
	afx_msg void OnUpdateEditShowNotMatching(CCmdUI* pCmdUI);
	afx_msg void OnEditShowOrphans();
	afx_msg void OnUpdateEditShowOrphans(CCmdUI* pCmdUI);
	afx_msg void OnEditShowAll();
	afx_msg void OnEditSelectAll();
	afx_msg void OnEditClearAll();
	afx_msg void OnUpdateEditClearAll(CCmdUI* pCmdUI);
	afx_msg void OnEditExpandAll();
	afx_msg void OnEditRefresh();
	afx_msg void OnViewCRLF();
	afx_msg void OnUpdateViewCRLF(CCmdUI* pCmdUI);
	afx_msg void OnViewWhitespace();
	afx_msg void OnUpdateViewWhitespace(CCmdUI* pCmdUI);
	afx_msg void OnDiffPrevious();
	afx_msg void OnUpdateDiffPrevious(CCmdUI* pCmdUI);
	afx_msg void OnDiffNext();
	afx_msg void OnUpdateDiffNext(CCmdUI* pCmdUI);
	afx_msg void OnFindServices();
	afx_msg void OnUpdateFindServices(CCmdUI* pCmdUI);
	afx_msg void OnCalcDependencies();
	afx_msg void OnUpdateCalcDependencies(CCmdUI* pCmdUI);
	afx_msg void OnCalcCrossreferences();
	afx_msg void OnUpdateCalcCrossreferences(CCmdUI* pCmdUI);
	afx_msg void OnSyncFromSelection();
	afx_msg void OnUpdateSyncFromSelection(CCmdUI* pCmdUI);
	afx_msg void OnSyncToSelection();
	afx_msg void OnUpdateSyncToSelection(CCmdUI* pCmdUI);
	afx_msg void OnCopyCrossreference();
	afx_msg void OnUpdateCopyCrossreference(CCmdUI* pCmdUI);
	afx_msg void OnHelpClientTools();
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()

	void InitializeRibbon();
	void RecalcLayout(BOOL bNotify = TRUE);

	void DoLogin();
	void DoLogout();
	bool ShowConfigPreference(IConfig *config); //true if changed config
	void OnItemSelectionChanged(CTreeNode *node, bool bSelected);
	void ShowDiff(const std::_tstring & ecl1 = _T(""), const std::_tstring & ecl1Header = _T(""), const std::_tstring & ecl2 = _T(""), const std::_tstring & ecl2Header = _T(""));
	bool GenerateHeader(IAttribute *attr, IAttribute *attrOther, std::_tstring & header);

public:
	void GetTitle(CString & title);
	IAttribute* GetAttribute() { return 0; }; //for CEclCommandMixin (OnSubmitDone)
	FindReplace	m_fr;

	//  IFindReplace
	FindReplace& GetFindReplace();
	void FindReplaceOnEditFind(const std::_tstring &findWhat, bool enableWildcardRegex, bool enableTooltip);
	void FindReplaceOnEditReplace(const std::_tstring &replaceWhat);
};

BOOL PostFrameMessage(UINT Msg, WPARAM wParam = 0, LPARAM lParam = 0);
LRESULT SendFrameMessage(UINT Msg, WPARAM wParam = 0, LPARAM lParam = 0);
WTL::CUpdateUIBase *GetUIUpdate();

