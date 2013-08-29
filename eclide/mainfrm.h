#pragma once

#define WORKSPACE_WINDOW

#include "loginDlg.h" //wlib
#include "global.h"
#include "repository.h"
#include "ChildBuilderFrame.h"
#include "SourceView.h"
#include "ChildAttributeFrame.h"
#include "ChildFrame.h"
#include "DfuFrame.h"
#include "LoginDlg.h"
#include "LimitSingleInstance.h"
#include "Thread.h"
#include "ChildGraphFrame.h"

#include "DockableDataViews.h"
#include "DockableBreakpointView.h"
#include "DockableSearchView.h"
#include "DockablePropertiesView.h"
#include "DockablePropertyGridViews.h"

#include "Workspace.h"

class CRepositoryFrame;
class CWorkspaceFrame;
class CRepositoryFilterFrame;
class CWorkunitTreeFrame;
class CActiveWorkunitTreeFrame;
class CFilteredWorkunitTreeFrame;
class CSyntaxFrame;
class CErrorFrame;
class CDfuFrame;
class CDfuFilterFrame;

const TCHAR * const ROLLBACK_MESSAGE = _T("Are you sure?\r\n\r\nNote:  Any open windows will be updated to reflect the current ecl.");
//  ===========================================================================
enum PERSIST_ITEM_TYPE 
{
	PERSIST_ITEM_UNKNOWN,
	PERSIST_ITEM_BUILDER,
	PERSIST_ITEM_ATTRIBUTE,
	PERSIST_ITEM_GRAPH,
	PERSIST_ITEM_LAST
};

class CPersistedItem : public clib::CLockableUnknown
{
	friend class CPersistedItemCompare;

protected:
	int m_groupIdx;
	int m_tabIdx;
	bool m_active;
	std::string m_filePath;
	CPersistMap m_props;
	CComPtr<IAttribute> m_attribute;
	CMainFrame * m_frame;

public:
	static void threadLoad(CPersistedItem * self)
	{
		clib::recursive_mutex::scoped_lock lock(self->m_mutex);
		switch (self->GetType())
		{
		case PERSIST_ITEM_BUILDER:
			break;
		case PERSIST_ITEM_ATTRIBUTE:
			{
				CComPtr<IRepository> rep = ::AttachRepository();
				self->m_attribute = rep->GetAttribute(self->m_props.Get(PERSIST_MODULE), self->m_props.Get(PERSIST_ATTRIBUTE), CreateIAttributeType(self->m_props.Get(PERSIST_ATTRIBUTETYPE)), 0, true, true);
			}
			break;
		case PERSIST_ITEM_GRAPH:
			break;
		}
	}

	CPersistedItem(const CPersistMap &props) : m_props(props)
	{
	}

	CPersistedItem(CMainFrame * frame, const std::string & filePath)
	{
		m_frame = frame;
		m_filePath = filePath;
		m_groupIdx = -1;
		m_tabIdx = -1;
		std::_tstring csFilePath = CA2T(GetFilePath());
		if (m_props.deserialize(csFilePath))
		{
			int tabIdx = -1;
			try
			{
				tabIdx = boost::lexical_cast<int>(m_props.Get(PERSIST_TABINDEX));
			}
			catch(boost::bad_lexical_cast &)
			{
			}
			m_tabIdx = tabIdx;

			int groupIdx = -1;
			try
			{
				groupIdx = boost::lexical_cast<int>(m_props.Get(PERSIST_GROUPINDEX));
			}
			catch(boost::bad_lexical_cast &)
			{
			}
			m_groupIdx = groupIdx;

			bool active = false;
			try
			{
				active = boost::lexical_cast<bool>(m_props.Get(PERSIST_ACTIVE));
			}
			catch(boost::bad_lexical_cast &)
			{
			}
			m_active = active;
		}
		RemoveFile();
		clib::thread run(__FUNCTION__, boost::bind(&threadLoad, this));
	}
	~CPersistedItem()
	{
		clib::recursive_mutex::scoped_lock lock(m_mutex);
	}
	bool operator == (const CPersistedItem & c) const
	{
		clib::recursive_mutex::scoped_lock lock(m_mutex);
		return m_groupIdx == c.m_groupIdx && m_tabIdx == c.m_tabIdx;
	}
	bool operator != (const CPersistedItem & c) const
	{
		clib::recursive_mutex::scoped_lock lock(m_mutex);
		return m_groupIdx != c.m_groupIdx || m_tabIdx != c.m_tabIdx;
	}
	bool operator < (const CPersistedItem & c) const
	{
		clib::recursive_mutex::scoped_lock lock(m_mutex);
		if (m_groupIdx == c.m_groupIdx)
			return m_tabIdx < c.m_tabIdx;
		return m_groupIdx < c.m_groupIdx;
	}
	const char * GetFilePath()
	{
		clib::recursive_mutex::scoped_lock lock(m_mutex);
		return m_filePath.c_str();
	}
	HWND OpenMDIChild(HWND hWndClient)
	{
		if (m_mutex.GetLockCount())
			return false;

		clib::recursive_mutex::scoped_lock lock(m_mutex);
		HWND hWnd = 0;
		switch(GetType())
		{
		case PERSIST_ITEM_BUILDER:
			{
				CComPtr<IRepository> rep = AttachRepository();
				hWnd = OpenBuilderMDI(m_frame, rep->CreateIWorkspaceItem(WORKSPACE_ITEM_BUILDER, m_props));
			}
			break;
		case PERSIST_ITEM_ATTRIBUTE:
			if (m_attribute)
			{
				CComPtr<IRepository> rep = AttachRepository();
				hWnd = OpenAttributeMDI(m_frame, m_attribute, rep->CreateIWorkspaceItem(WORKSPACE_ITEM_ATTRIBUTE, m_props), false);
			}
			break;
		case PERSIST_ITEM_GRAPH:
			{
				CComPtr<IRepository> rep = AttachRepository();
				hWnd = OpenGraphMDI(m_frame, rep->CreateIWorkspaceItem(WORKSPACE_ITEM_GRAPH, m_props));
			}
			break;
		}
		if (!hWnd)
		{
			RemoveFile();
		}
		return hWnd;
	}
	void RemoveFile()
	{
		clib::recursive_mutex::scoped_lock lock(m_mutex);
		try
		{
			boost::filesystem::remove(boost::filesystem::path(m_filePath, boost::filesystem::native)); 
		}
		catch(std::exception &)
		{
			ATLASSERT(false);
		}
	}
	int GetGroupIndex()
	{
		clib::recursive_mutex::scoped_lock lock(m_mutex);
		return m_groupIdx;
	}
	int GetTabIndex()
	{
		clib::recursive_mutex::scoped_lock lock(m_mutex);
		return m_tabIdx;
	}
	bool IsActive()
	{
		clib::recursive_mutex::scoped_lock lock(m_mutex);
		return m_active;
	}

protected:
	PERSIST_ITEM_TYPE GetType() 
	{
		clib::recursive_mutex::scoped_lock lock(m_mutex);
		if (std::_tstring(m_props.Get(PERSIST_TYPE)).compare(PERSISTVAL_BUILDER) == 0)
		{
			return PERSIST_ITEM_BUILDER;
		}
		else if (std::_tstring(m_props.Get(PERSIST_TYPE)).compare(PERSISTVAL_ATTRIBUTE) == 0)
		{
			return PERSIST_ITEM_ATTRIBUTE;
		}
		else if (std::_tstring(m_props.Get(PERSIST_TYPE)).compare(PERSISTVAL_GRAPH) == 0)
		{
			return PERSIST_ITEM_GRAPH;
		}
		return PERSIST_ITEM_UNKNOWN;
	}
};

class CPersistedItemCompare
{
public:
	bool operator ()(const StlLinked<CPersistedItem> & l, const StlLinked<CPersistedItem> & r) const
	{
		if (l->m_groupIdx == r->m_groupIdx)
			return l->m_tabIdx < r->m_tabIdx;
		return l->m_groupIdx < r->m_groupIdx;
	}
};

typedef StlLinked<CPersistedItem> CPersistedItemAdapt;
typedef std::vector<CPersistedItemAdapt> CPersistedItemVector;
typedef std::vector<HWND> CChildFrameVector;
//  ===========================================================================
enum RIBBON
{
	RIBBON_UNKNOWN = 0,
	RIBBON_HOME,
	RIBBON_RESULT,
	RIBBON_GRAPH,
	RIBBON_GRAPH2,
	RIBBON_DEBUG,
	RIBBON_BROWSER,
	RIBBON_COMPARE,
	RIBBON_LAST
};

struct WinPane
{
	CDockablePane * m_pane;
	DWORD m_pos;
	bool m_pinned;
	WinPane(CDockablePane * pane, int pos, bool pinned = true)
	{
		m_pane = pane;
		m_pos = pos;
		m_pinned = pinned;
	}
};
typedef std::vector<WinPane> WinPaneVector;

struct NeighbourPane
{
	CDockablePane * m_mainPane;
	CDockablePane * m_childPane;
	DWORD m_pos;
	NeighbourPane(CDockablePane * mainPane, CDockablePane * childPane, DWORD pos)
	{
		m_mainPane = mainPane;
		m_childPane = childPane;
		m_pos = pos;
	}
};
typedef std::vector<NeighbourPane> NeighbourPaneVector;

struct TabbedPane
{
	CDockablePane * m_mainPane;
	CDockablePane * m_childPane;
	TabbedPane(CDockablePane * mainPane, CDockablePane * childPane)
	{
		m_mainPane = mainPane;
		m_childPane = childPane;
	}
};
typedef std::vector<TabbedPane> TabbedPaneVector;
struct HiddenPane
{
	CDockablePane * m_pane;
	HiddenPane(CDockablePane * pane)
	{
		m_pane = pane;
	}
};
typedef std::vector<HiddenPane> HiddenPaneVector;

enum WORKSPACE
{
	WORKSPACE_UNKNOWN = 0,
	WORKSPACE_NORMAL,
	WORKSPACE_GRAPH,
	WORKSPACE_DEBUG,
	WORKSPACE_LAST
};

class CMainFrame :	public CMDIFrameWndEx, 
					public IFindReplace,
					public ILoginConfigPreferences,
					public boost::signals::trackable
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	BOOL PreTranslateMessage(MSG* pMsg);
	//BOOL DockedWindowPreTranslate(MSG* pMsg);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	CMFCRibbonBar     m_wndRibbonBar;
protected:  // control bar embedded members
	CMFCRibbonApplicationButton m_MainButton;
	CMFCToolBarImages m_PanelImages;
	CMFCRibbonStatusBar  m_wndStatusBar;

	CMFCRibbonCategory* m_CategoryHome;
	CMFCRibbonCategory* m_CategoryView;
	CMFCRibbonCategory* m_CategoryResult;
	CMFCRibbonCategory* m_CategoryBrowser;
	CMFCRibbonCategory* m_CategoryGraph;
	CMFCRibbonCategory* m_CategoryGraph2;
	CMFCRibbonCategory* m_CategoryDebug;
	CMFCRibbonCategory* m_CategoryDebugAdvanced;
	CMFCRibbonCategory* m_CategoryCompare;
public:
	CMFCRibbonButton* m_graphRefresh;
	CMFCRibbonButton * m_buttMacroRecord;
	CMFCRibbonComboBox * m_workspaceCombo;

	RIBBON m_prevRibbon;
	CDockState m_dockStateDebug;
	CDockState m_dockStateWorkspace;

protected:
#ifdef WORKSPACE_WINDOW
	CWorkspaceFrame *m_Workspace;
#endif
	CRepositoryFrame *m_Repository;
	CRepositoryFilterFrame *m_RepositoryFilter;
	CWorkunitTreeFrame *m_Workunits;
	CActiveWorkunitTreeFrame *m_WorkunitsActive;
	CFilteredWorkunitTreeFrame *m_WorkunitsFilter;
#ifdef DFU_WINDOWS
	CDfuFrame *m_Dfu;
	CDfuFilterFrame *m_DfuFilter;
#endif
	CSyntaxFrame *m_Syntax;
public:
	CErrorFrame *m_Error;

	CMFCCaptionBar m_wndCaptionBar;
	CDockableDataViews *m_debugDataViews;
	CDockableBreakpointView *m_debugBreakpointView;
	CDockablePropertiesView *m_debugPropertiesView;
	CDockablePropertyGridViews *m_debugPropertyGridViews;
	CDockableSearchView *m_debugSearchView;

	bool m_tabbedMDI;
	CMFCTabCtrl::Style m_tabbedStyle;
	bool m_closeOnTab;
	WORKSPACE m_workspaceMode;
	bool m_supressSyncTOC;

// Generated message map functions
protected:
	void SetMDIMode();
	void SetWorkspaceMode(WORKSPACE workspaceMode);
public:
	void SetProgressBar(int nID, int pos, int layoutProgress);
protected:
	virtual BOOL OnShowMDITabContextMenu(CPoint point, DWORD dwAllowedItems, BOOL bDrop);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT nID);
	afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg LRESULT OnInitialize(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnStatusUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRestoreState(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSelectRibbon(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMDIChildActivate(WPARAM wParam, LPARAM lParam);

	afx_msg void OnFileNew();
	afx_msg void OnFileNewSpray();
	afx_msg void OnFileOpen();
	afx_msg void OnFileOpenAttribute();
	afx_msg void OnFileOpenWorkunit();
	afx_msg void OnFileSaveAll();
	afx_msg void OnFileNewWorkspace();
	afx_msg void OnFilePageSetup();
	afx_msg void OnFilePreferences();
	afx_msg void OnFileLogin();
	afx_msg void OnFileChangePassword();
	afx_msg void OnFileExit();

	afx_msg void OnEditAdvanced();

	afx_msg void OnWindowManager();
	afx_msg void OnWindowCascade();
	afx_msg void OnUpdateWindowCascade(CCmdUI* pCmdUI);
	afx_msg void OnWindowTile();
	afx_msg void OnWindowArrange();

	afx_msg void OnWorkspaceNew();
	afx_msg void OnWorkspaceRemove();
	afx_msg void OnWorkspaceSelEndOk();

	afx_msg void OnMdiMoveToNextGroup();
	afx_msg void OnMdiMoveToPrevGroup();
	afx_msg void OnMdiNewHorzTabGroup();
	afx_msg void OnMdiNewVertGroup();
	afx_msg void OnMdiCancel();

	afx_msg void OnWindowsCloseAll();
	afx_msg void OnWindowsSort();
	afx_msg void OnUpdateWindowsSort(CCmdUI* pCmdUI);
	afx_msg void OnWindowsGroup();
	afx_msg void OnWindowsDegroup();

	afx_msg void OnViewFullScreen();
	afx_msg void OnUpdateViewDockedPane(CCmdUI* pCmdUI);
	afx_msg BOOL OnViewDockedPane(UINT nID);

	afx_msg BOOL OnViewResetDockedToolbars(UINT nID);

	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);

	afx_msg void OnTabLook(UINT id);
	afx_msg void OnUpdateTabLook(CCmdUI* pCmdUI);

	afx_msg void OnCloseOnTab();
	afx_msg void OnUpdateCloseOnTab(CCmdUI* pCmdUI);

	afx_msg void OnHelpLanguageReference();

	afx_msg LRESULT OnGetTabToolTip(WPARAM /*wp*/, LPARAM lp);

	DECLARE_MESSAGE_MAP()

	void InitializeRibbon();
	void InitializeDocking(UINT nID);
	void InitializeDockingDefaultLeft(WinPaneVector & mainPanes, NeighbourPaneVector & neighbourPanes, TabbedPaneVector & tabbedPanes, HiddenPaneVector & hiddenPanes);
	void InitializeDockingDefaultRight(WinPaneVector & mainPanes, NeighbourPaneVector & neighbourPanes, TabbedPaneVector & tabbedPanes, HiddenPaneVector & hiddenPanes);
	void InitializeDockingPinnedLeft(WinPaneVector & mainPanes, NeighbourPaneVector & neighbourPanes, TabbedPaneVector & tabbedPanes, HiddenPaneVector & hiddenPanes);
	void InitializeDockingPinnedRight(WinPaneVector & mainPanes, NeighbourPaneVector & neighbourPanes, TabbedPaneVector & tabbedPanes, HiddenPaneVector & hiddenPanes);
	void InitializeDockingUnpinnedLeft(WinPaneVector & mainPanes, NeighbourPaneVector & neighbourPanes, TabbedPaneVector & tabbedPanes, HiddenPaneVector & hiddenPanes);
	void InitializeDockingUnpinnedRight(WinPaneVector & mainPanes, NeighbourPaneVector & neighbourPanes, TabbedPaneVector & tabbedPanes, HiddenPaneVector & hiddenPanes);

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);

public:
//__interface IMainFrame : public IFindReplace
	FindReplace m_fr;
	FindDlg m_FindDlg;
	ReplaceDlg m_ReplaceDlg;
	CImageList m_formatImageList;

	HWND m_lastWindowWithFocus;
	HWND m_lastDockedWindowFrameWithFocus;

	clib::CThreadQueue m_threadSave;
	CComPtr<IConfig> m_iniFile;

	StlLinked<CLimitSingleInstance> m_singleInstance;

	CPersistedItemVector m_persistedWindows;
	CPersistedItemVector m_activeWindows;

	bool UIUpdateMenuItems();
	void DoLogout();
	void DoLogin(bool SkipLoginWindow = false, const CString & previousPassword = _T(""));
	void RestoreState();
	void PostSelectRibbon(RIBBON ribbon);
	void DoPopupFileNew(LPNMTOOLBAR /*lpnmtb*/);
	void DoPopupFileOpen(LPNMTOOLBAR /*lpnmtb*/);
	void DoPopupEclGo(LPNMTOOLBAR /*lpnmtb*/);
	HWND GetActive();
	HWND DockedGetActive();
	HWND GetMDINext();
	void MDINext();
	void MDIPrev();

	void operator()(SectionLabel * label);
	int MDIGetCount();
	BOOL DoFileOpen(const CString & sPathName);
	BOOL DoFileSaveAll(bool attrsOnly);

	void DoWorkspaceRefresh();
	void DoWorkspaceNew();
	void DoWorkspaceSave();
	void DoWorkspaceLoad(IWorkspace * newWorkspace);
	void DoWorkspaceRemove(const IWorkspaceVector & workspaces);
	void DoWorkspaceSwitch(IWorkspace * workspace, bool saveCurrent);

	void PostStatus(const TCHAR* pStr, PANE pane = PANE_DEFAULT); //thread safe call
	void DisableAllMenuItems();
	void OpenBuilder(const TCHAR *ecl);
	void OpenBuilder(const CString & wuid, BuilderStartup startWith);
	void OpenBuilder(Dali::IWorkunit *wu, BuilderStartup startWith);
	void OpenBuilder(IAttribute * attribute);
	void SaveWorkunitsAs(CComPtr<Dali::IWorkunitVectorCom> wus);
	void SaveWorkunitAs(CComPtr<Dali::IWorkunit> wu);
	void SaveWorkunitAs(const std::_tstring & wuid);

	void OpenSyntaxAttribute(const CString & modAttrLabel, IAttributeType * type, const CSyntaxErrorVector & attrSyntax);
	void OpenSyntaxAttribute(IAttribute * attribute, Dali::CEclExceptionVector * errors);

	void OpenAttribute(const CString & modAttrLabel, IAttributeType * type, const CString & currModule = _T(""));
	void OpenAttribute(const std::_tstring & module, const std::_tstring & attribute, IAttributeType * type, unsigned int row, unsigned int col);
	void OpenAttribute(IAttribute * attribute, bool bHistoryView=false);
	void OpenAttribute(IAttribute * attribute, const std::_tstring & searchTerm, FINDMODE findmode);
	void SaveAttribute(IAttribute * attribute);

	void InsertText(IAttribute * attribute);
	bool ShowComment(IAttribute * attribute, CString & comment);
	bool ShowComment(CString & comment);
	void ShowHistory(IAttribute * attr);
	void SyncTOC(const CString & modAttrLabel, IAttributeType * type, const CString & currModule = _T(""));
	void SyncTOC(IAttribute * attr);
	void SyncWorkunitTOC(const CString & wuid);
	IAttribute * FuzzyLocate(const CString & modAttrLabel, IAttributeType * type, const CString & currModule = _T(""));

	void OnEditFind(const std::_tstring & findWhat, bool enableWildcardRegex, bool enableTooltip);
	void OnEditReplace(const std::_tstring & replaceWhat);

	void CloneState(WTL::CMenu &m);

	void Send_RefreshStatusBar1(const TCHAR* pStr);
	void Send_RefreshWorkunitWindow();
	void Send_RefreshRepositoryWindow(IModule * module);
	void Send_ClearSyntax();
	void Send_RefreshSyntax(ISciSyntaxMarker *errors, size_t curErr, bool forceShow = false);
	void Post_RefreshWorkunitWindow(Dali::IWorkunit * wu);

	COLORREF GetTextColor();
	bool GetTextBold();
	bool GetTextItalic();
	COLORREF GetFillColor();


	//IFindReplace
	FindReplace& GetFindReplace();
	void FindReplaceOnEditFind(const std::_tstring &findWhat, bool enableWildcardRegex, bool enableTooltip);
	void FindReplaceOnEditReplace(const std::_tstring &replaceWhat);

	//ILogonConfigPreferences
	bool ShowConfigPreference(IConfig *config);

	//IPersistable
	void SavePersistInfo(CPersistMap & persistInfo);
	void RestorePersistInfo(const CPersistMap & persistInfo);
	HWND GetHwnd();
};

FindReplace& GetFindReplace();
void FindReplaceOnEditFind(const std::_tstring &findWhat, bool enableWildcardRegex, bool enableTooltip);
void FindReplaceOnEditReplace(const std::_tstring &replaceWhat);


WTL::CUpdateUIBase *GetUIUpdate();

CMainFrame * GetIMainFrame();