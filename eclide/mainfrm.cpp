#include "stdafx.h"
#include "..\en_us\resource.h"

#include "QueryBuilder.h"

#include "MainFrm.h"

#include "RepositoryFrame.h"
#include "WorkunitFrame.h"
#include "WorkspaceFrame.h"
#include "SyntaxFrame.h"
#include "BookmarksFrame.h"
#include "ErrorFrame.h"

#include "ChangePasswordDlg.h"
#include "OpenWorkunit.h"
#include "CommentDlg.h"
#include "BasicExcel.hpp"
//#include "InvokeSprayWizard.h"
#include "AttrListDlg.h"
#include <UtilFilesystem.h>

enum UM
{
    UM_FIRST = CWM_LAST + 1,
    UM_MDICHILDCLOSE,
    UM_MDICHILDACTIVATE,
    UM_RESTORESTATE,
    UM_SELECTRIBBON,
    UM_LAST
};

static const SectionLabelDefault GLOBAL_TABBEDSTYLE(SectionLabel(_T("General"), _T("TabStyle")), (int)CMFCTabCtrl::STYLE_3D_VS2005);
static const SectionLabelDefault GLOBAL_CLOSEONTAB(SectionLabel(_T("General"), _T("CloseOnTab")), false);

static const SectionLabelDefault WORKSPACE_CURRENT(SectionLabel(_T("Workspace"), _T("Current")), _T("Default"));

#define SAVE_PAGE_SIZE 1000
#define MAX_EXCEL_ROWS 65535//4	//65535 is for excel - 1 for headers.

typedef std::pair<std::_tstring, HWND> TStringHWND;
typedef std::vector<TStringHWND> TStringHWNDVector;

static CMainFrame *g_MainFrame = 0;
// CMainFrame
BOOL CALLBACK ChildEnumProc(HWND hwnd, LPARAM lParam);
BOOL CALLBACK ChildEnumProcForSaveWorkspace(HWND hwnd, LPARAM lParam);
//  ===========================================================================
void DockToMainframe(const WinPaneVector & panes)
{
    for(WinPaneVector::const_iterator itr = panes.begin(); itr != panes.end(); ++itr)
    {
        itr->m_pane->DockToFrameWindow(itr->m_pos);
        if (!itr->m_pinned)
            itr->m_pane->SetAutoHideMode(TRUE, itr->m_pos, NULL, FALSE);
    }
}
void DockToPane(const NeighbourPaneVector & panes)
{
    for(NeighbourPaneVector::const_iterator itr = panes.begin(); itr != panes.end(); ++itr)
    {
        itr->m_childPane->DockToWindow(itr->m_mainPane, itr->m_pos);
    }
}
void TabDockToPane(const TabbedPaneVector & panes)
{
    for(TabbedPaneVector::const_iterator itr = panes.begin(); itr != panes.end(); ++itr)
    {
        CDockablePane * tb = NULL;
        itr->m_childPane->AttachToTabWnd(itr->m_mainPane, DM_SHOW, FALSE, &tb);
    }
}
void HidePane(const HiddenPaneVector & panes)
{
    for(HiddenPaneVector::const_iterator itr = panes.begin(); itr != panes.end(); ++itr)
    {
        itr->m_pane->SetAutoHideMode(FALSE, CBRS_ALIGN_ANY, NULL, FALSE);
        itr->m_pane->ShowPane(FALSE, FALSE, TRUE);
    }
}
//  ===========================================================================
IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
    ON_WM_CREATE()
    ON_WM_CLOSE()
    ON_WM_TIMER()
    ON_WM_ACTIVATEAPP()
    ON_WM_SIZE()

    ON_REGISTERED_MESSAGE(AFX_WM_TOOLBARMENU, OnToolbarContextMenu)

    ON_MESSAGE(CWM_INITIALIZE, OnInitialize)
    ON_MESSAGE(CWM_STATUSUPDATE, OnStatusUpdate)
    ON_MESSAGE(UM_RESTORESTATE, OnRestoreState)
    ON_MESSAGE(UM_SELECTRIBBON, OnSelectRibbon)
    ON_MESSAGE(UM_MDICHILDACTIVATE, OnMDIChildActivate)

    ON_COMMAND(ID_FILE_NEW, OnFileNew)
    ON_COMMAND(ID_FILE_NEWBUILDER, OnFileNew)
    ON_COMMAND(ID_WINDOW_NEW, OnFileNew)
    //ON_COMMAND(ID_FILE_NEWSPRAY, OnFileNewSpray)
    ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
    ON_COMMAND(ID_FILE_OPENBUILDER, OnFileOpen)
    ON_COMMAND(ID_FILE_OPENATTRIBUTE, OnFileOpenAttribute)
    ON_COMMAND(ID_FILE_OPENWORKUNIT, OnFileOpenWorkunit)
    ON_COMMAND(ID_FILE_SAVE_ALL, OnFileSaveAll)
    ON_COMMAND(ID_FILE_NEWWORKSPACE, OnFileNewWorkspace)

    ON_COMMAND(ID_FILE_PRINT_SETUP, OnFilePageSetup)

    ON_COMMAND(ID_FILE_PREFERENCES, OnFilePreferences)
    ON_COMMAND(ID_FILE_LOGOUT, OnFileLogin)
    ON_COMMAND(ID_FILE_CHANGEPASSWORD, OnFileChangePassword)
    ON_COMMAND(ID_APP_EXIT, OnFileExit)

    ON_COMMAND(ID_EDIT_ADVANCED, OnEditAdvanced)

    ON_COMMAND(ID_MDI_CLOSEALLOTHERS, OnMdiCloseRest)
    ON_UPDATE_COMMAND_UI(ID_MDI_CLOSEALLOTHERS, OnUpdateCloseRest)
    ON_COMMAND(ID_MDI_CLOSEALLLEFT, OnMdiCloseLeft)
    ON_UPDATE_COMMAND_UI(ID_MDI_CLOSEALLLEFT, OnUpdateCloseLeft)
    ON_COMMAND(ID_MDI_CLOSEALLRIGHT, OnMdiCloseRight)
    ON_UPDATE_COMMAND_UI(ID_MDI_CLOSEALLRIGHT, OnUpdateCloseRight)

    ON_COMMAND(ID_MDI_MOVE_TO_NEXT_GROUP, OnMdiMoveToNextGroup)
    ON_COMMAND(ID_MDI_MOVE_TO_PREV_GROUP, OnMdiMoveToPrevGroup)
    ON_COMMAND(ID_MDI_NEW_HORZ_TAB_GROUP, OnMdiNewHorzTabGroup)
    ON_COMMAND(ID_MDI_NEW_VERT_GROUP, OnMdiNewVertGroup)
    ON_COMMAND(ID_MDI_CANCEL, OnMdiCancel)

    ON_COMMAND(ID_WINDOW_CLOSEALL, OnWindowsCloseAll)

    ON_COMMAND(ID_SORTING_SORTALPHABETIC, OnWindowsSort)
    ON_UPDATE_COMMAND_UI(ID_SORTING_SORTALPHABETIC, OnUpdateWindowsSort)
    ON_COMMAND(ID_SORTING_GROUPBYTYPE, OnWindowsGroup)
    ON_UPDATE_COMMAND_UI(ID_SORTING_GROUPBYTYPE, OnUpdateWindowsSort)
    ON_COMMAND(ID_SORTING_DEGROUP, OnWindowsDegroup)
    ON_UPDATE_COMMAND_UI(ID_SORTING_DEGROUP, OnUpdateWindowsSort)

    ON_UPDATE_COMMAND_UI(ID_VIEW_PROJECT, OnUpdateViewDockedPane)
    ON_COMMAND_EX(ID_VIEW_PROJECT, OnViewDockedPane)
    ON_UPDATE_COMMAND_UI(ID_VIEW_REPOSITORY, OnUpdateViewDockedPane)
    ON_COMMAND_EX(ID_VIEW_REPOSITORY, OnViewDockedPane)
    ON_UPDATE_COMMAND_UI(ID_VIEW_REPOSITORYFILTER, OnUpdateViewDockedPane)
    ON_COMMAND_EX(ID_VIEW_REPOSITORYFILTER, OnViewDockedPane)
    ON_UPDATE_COMMAND_UI(ID_VIEW_WORKUNITSTREE, OnUpdateViewDockedPane)
    ON_COMMAND_EX(ID_VIEW_WORKUNITSTREE, OnViewDockedPane)
    ON_UPDATE_COMMAND_UI(ID_VIEW_ACTIVEWORKUNITSTREE, OnUpdateViewDockedPane)
    ON_COMMAND_EX(ID_VIEW_ACTIVEWORKUNITSTREE, OnViewDockedPane)
    ON_UPDATE_COMMAND_UI(ID_VIEW_FILTEREDWORKUNITTREEFRAME, OnUpdateViewDockedPane)
    ON_COMMAND_EX(ID_VIEW_FILTEREDWORKUNITTREEFRAME, OnViewDockedPane)
    ON_UPDATE_COMMAND_UI(ID_VIEW_DFU, OnUpdateViewDockedPane)
    ON_COMMAND_EX(ID_VIEW_DFU, OnViewDockedPane)
    ON_UPDATE_COMMAND_UI(ID_VIEW_DFUFILTER, OnUpdateViewDockedPane)
    ON_COMMAND_EX(ID_VIEW_DFUFILTER, OnViewDockedPane)
    ON_UPDATE_COMMAND_UI(ID_VIEW_SYNTAX, OnUpdateViewDockedPane)
    ON_COMMAND_EX(ID_VIEW_SYNTAX, OnViewDockedPane)
    ON_UPDATE_COMMAND_UI(ID_VIEW_ERROR, OnUpdateViewDockedPane)
    ON_COMMAND_EX(ID_VIEW_ERROR, OnViewDockedPane)

    ON_UPDATE_COMMAND_UI(ID_VIEW_DEBUGBREAKPOINTS, OnUpdateViewDockedPane)
    ON_COMMAND_EX(ID_VIEW_DEBUGBREAKPOINTS, OnViewDockedPane)
    ON_UPDATE_COMMAND_UI(ID_VIEW_DEBUGPROPERTYGRID, OnUpdateViewDockedPane)
    ON_COMMAND_EX(ID_VIEW_DEBUGPROPERTYGRID, OnViewDockedPane)
    ON_UPDATE_COMMAND_UI(ID_VIEW_DEBUGDATA, OnUpdateViewDockedPane)
    ON_COMMAND_EX(ID_VIEW_DEBUGDATA, OnViewDockedPane)
    ON_UPDATE_COMMAND_UI(ID_VIEW_DEBUGPROPERTIES, OnUpdateViewDockedPane)
    ON_COMMAND_EX(ID_VIEW_DEBUGPROPERTIES, OnViewDockedPane)
    ON_UPDATE_COMMAND_UI(ID_VIEW_DEBUGSEARCH, OnUpdateViewDockedPane)
    ON_COMMAND_EX(ID_VIEW_DEBUGSEARCH, OnViewDockedPane)
    ON_UPDATE_COMMAND_UI(ID_VIEW_BOOKMARKS, OnUpdateViewDockedPane)
    ON_COMMAND_EX(ID_VIEW_BOOKMARKS, OnViewDockedPane)

    ON_COMMAND_EX(ID_VIEW_RESETDOCKEDWINDOWS_DEFAULT_LEFT, OnViewResetDockedToolbars)
    ON_COMMAND_EX(ID_VIEW_RESETDOCKEDWINDOWS_DEFAULT_RIGHT, OnViewResetDockedToolbars)
    ON_COMMAND_EX(ID_VIEW_RESETDOCKEDWINDOWS_ALLPINNED_LEFT, OnViewResetDockedToolbars)
    ON_COMMAND_EX(ID_VIEW_RESETDOCKEDWINDOWS_ALLPINNED_RIGHT, OnViewResetDockedToolbars)
    ON_COMMAND_EX(ID_VIEW_RESETDOCKEDWINDOWS_ALLUNPINNED_LEFT, OnViewResetDockedToolbars)
    ON_COMMAND_EX(ID_VIEW_RESETDOCKEDWINDOWS_ALLUNPINNED_RIGHT, OnViewResetDockedToolbars)

    ON_COMMAND(ID_WINDOW_MANAGER, OnWindowManager)
    ON_COMMAND(ID_WINDOW_CASCADE, OnWindowCascade)
    ON_UPDATE_COMMAND_UI(ID_WINDOW_CASCADE, OnUpdateWindowCascade)
    ON_COMMAND(ID_WINDOW_TILE_HORZ, OnWindowTile)
    ON_COMMAND(ID_WINDOW_ARRANGE, OnWindowArrange)

    ON_COMMAND(ID_WORKSPACE_ADD, OnWorkspaceNew)
    ON_COMMAND(ID_WORKSPACE_REMOVE, OnWorkspaceRemove)
    ON_COMMAND(ID_WORKSPACE_LIST, OnWorkspaceSelEndOk)

    ON_COMMAND(ID_HELP_LANGUAGEREFERENCE, OnHelpLanguageReference)

    ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WIN7_SCENIC, OnApplicationLook)
    ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WIN7_SCENIC, OnUpdateApplicationLook)

    ON_COMMAND_RANGE(ID_VIEW_TABLOOK_STYLE_3D, ID_VIEW_TABLOOK_STYLE_3D_ROUNDED_SCROLL, OnTabLook)
    ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_TABLOOK_STYLE_3D, ID_VIEW_TABLOOK_STYLE_3D_ROUNDED_SCROLL, OnUpdateTabLook)

    ON_COMMAND(ID_VIEW_TABLOOK_STYLE_CLOSE_ON_TAB, OnCloseOnTab)
    ON_UPDATE_COMMAND_UI(ID_VIEW_TABLOOK_STYLE_CLOSE_ON_TAB, OnUpdateCloseOnTab)

    ON_REGISTERED_MESSAGE(AFX_WM_ON_GET_TAB_TOOLTIP, OnGetTabToolTip)
END_MESSAGE_MAP()

// CMainFrame construction/destruction

CMainFrame::CMainFrame() : m_threadSave(5)
{
    // TODO: add member initialization code here
    theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_OFF_2007_BLUE);

    //this is the INI file for items that aren't server specific
    boost::filesystem::path iniPath;
    GetIniPath(iniPath);
    m_iniFile = CreateIConfig(QUERYBUILDER_INI, iniPath);

#ifdef WORKSPACE_WINDOW
    m_Workspace = new CWorkspaceFrame();
#endif
    m_Repository = new CRepositoryFrame();
    m_RepositoryFilter = new CRepositoryFilterFrame();
    m_Workunits = new CWorkunitTreeFrame();
    m_WorkunitsActive = new CActiveWorkunitTreeFrame();
    m_WorkunitsFilter = new CFilteredWorkunitTreeFrame();
#ifdef DFU_WINDOWS
    m_Dfu = new CDfuFrame();
    m_DfuFilter = new CDfuFilterFrame();
#endif
    m_Syntax = new CSyntaxFrame();
    m_Bookmarks = new CBookmarksFrame();
    m_debugDataViews = new CDockableDataViews();
    m_debugBreakpointView = new CDockableBreakpointView();
    m_debugPropertiesView = new CDockablePropertiesView();
    m_debugPropertyGridViews = new CDockablePropertyGridViews();
    m_debugSearchView = new CDockableSearchView();
    m_Error = new CErrorFrame();

    m_tabbedMDI = true;
    m_tabbedStyle = (CMFCTabCtrl::Style)(int)m_iniFile->Get(GLOBAL_TABBEDSTYLE);
    m_closeOnTab = (bool)m_iniFile->Get(GLOBAL_CLOSEONTAB);
    m_workspaceMode = WORKSPACE_NORMAL;
    m_supressSyncTOC = true;

    m_prevRibbon = RIBBON_HOME;

    g_MainFrame = this;
}

CMainFrame::~CMainFrame()
{
    m_iniFile->Set(GLOBAL_CLOSEONTAB, m_closeOnTab);
    m_iniFile->Set(GLOBAL_TABBEDSTYLE, m_tabbedStyle);

    g_MainFrame = NULL;
    delete m_Error;
    delete m_debugDataViews;
    delete m_debugBreakpointView;
    delete m_debugPropertiesView;
    delete m_debugPropertyGridViews;
    delete m_debugSearchView;	//Causes leak, but it is preventing the clean closedown...
    delete m_Syntax;
    delete m_Bookmarks;
#ifdef DFU_WINDOWS
    delete m_DfuFilter;
    delete m_Dfu;
#endif
    delete m_WorkunitsFilter;
    delete m_WorkunitsActive;
    delete m_Workunits;
    delete m_RepositoryFilter;
    delete m_Repository;
#ifdef WORKSPACE_WINDOW
    delete m_Workspace;
#endif
}

HWND CMainFrame::GetMDINext()
{
    bool first = true;
    CMDIChildWndEx* pFramePrev = NULL;
    HWND hwndT = ::GetWindow(m_hWndMDIClient, GW_CHILD);
    while (hwndT != NULL)
    {
        CMDIChildWndEx* pFrame = DYNAMIC_DOWNCAST(CMDIChildWndEx, CWnd::FromHandle(hwndT));
        if (pFrame == NULL)
        {
            hwndT = ::GetWindow(hwndT, GW_HWNDNEXT);
            continue;
        }
        if (first)
            first = false;
        else
        {
            return hwndT;
        }
        pFramePrev = pFrame;
        hwndT = ::GetWindow(hwndT, GW_HWNDNEXT);
        if (!hwndT)
            hwndT = ::GetWindow(pFrame->GetSafeHwnd(), GW_HWNDFIRST);
    }
    return NULL;
}

void CMainFrame::MDINext()
{
    if (m_tabbedMDI)
    {
        bool first = true;
        CMDIChildWndEx* pFramePrev = NULL;
        HWND hwndT = ::GetWindow(m_hWndMDIClient, GW_CHILD);
        while (hwndT != NULL)
        {
            CMDIChildWndEx* pFrame = DYNAMIC_DOWNCAST(CMDIChildWndEx, CWnd::FromHandle(hwndT));
            if (pFrame == NULL)
            {
                hwndT = ::GetWindow(hwndT, GW_HWNDNEXT);
                continue;
            }
            if (first)
                first = false;
            else if (pFramePrev)
            {
                m_wndClientArea.SetActiveTab(hwndT);
                pFramePrev->SetWindowPos(&CWnd::wndBottom, 0, 0, 0, 0, SWP_ASYNCWINDOWPOS | SWP_DEFERERASE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_NOREDRAW | SWP_NOSENDCHANGING);
                break;
            }
            pFramePrev = pFrame;
            hwndT = ::GetWindow(hwndT, GW_HWNDNEXT);
            if (!hwndT)
                hwndT = ::GetWindow(pFrame->GetSafeHwnd(), GW_HWNDFIRST);
        }
    }
    else
        CMDIFrameWndEx::MDINext();
}

void CMainFrame::MDIPrev()
{
    if (m_tabbedMDI)
    {
        bool first = true;
        HWND hwndT = ::GetWindow(m_hWndMDIClient, GW_CHILD);
        while (hwndT != NULL)
        {
            CMDIChildWndEx* pFrame = DYNAMIC_DOWNCAST(CMDIChildWndEx, CWnd::FromHandle(hwndT));
            if (pFrame == NULL)
            {
                hwndT = ::GetWindow(hwndT, GW_HWNDPREV);
                continue;
            }
            if (first)
                first = false;
            else 
            {
                m_wndClientArea.SetActiveTab(pFrame->GetSafeHwnd());
                break;
            }
            hwndT = ::GetWindow(hwndT, GW_HWNDPREV);
            if (!hwndT)
                hwndT = ::GetWindow(pFrame->GetSafeHwnd(), GW_HWNDLAST);
        }
    }
    else
        CMDIFrameWndEx::MDIPrev();
}

void CMainFrame::SetMDIMode()
{
    if (m_tabbedMDI)
    {
        CMDITabInfo mdiTabParams;
        mdiTabParams.m_bTabIcons = TRUE;    // set to TRUE to enable document icons on MDI taba
        mdiTabParams.m_bTabCloseButton = TRUE;
        mdiTabParams.m_style = m_tabbedStyle; // other styles available...
        mdiTabParams.m_bAutoColor = FALSE;    // set to FALSE to disable auto-coloring of MDI tabs
        mdiTabParams.m_bDocumentMenu = FALSE; // enable the document menu at the right edge of the tab area
        mdiTabParams.m_bActiveTabCloseButton = m_closeOnTab;      // set to FALSE to place close button at right of tab area
        mdiTabParams.m_bTabCustomTooltips = true;
        EnableMDITabbedGroups(TRUE, mdiTabParams);
    }
    else
    {
        CMDITabInfo mdiTabParams;
        if (IsMDITabbedGroup())
        {
            EnableMDITabbedGroups(FALSE, mdiTabParams);
            HWND hwndActive = (HWND) m_wndClientArea.SendMessage(WM_MDIGETACTIVE);
            ::BringWindowToTop (hwndActive);
        }
    }
}

void PreAdjustPane(CDockablePane * pane)
{
    while (pane->IsTabbed())
        pane = pane->GetParentTabbedPane();

    CFrameWnd* pFrame = pane->GetParentFrame();
    if (pFrame == NULL)
        return;
    ASSERT_VALID(pFrame);

    CPaneDivider* pSlider = pane->GetDefaultPaneDivider();
    if (!pSlider)
        return;

    CRect rectContainer = pSlider->GetRootContainerRect();
    if (rectContainer.IsRectEmpty())
        return;
    pFrame->ScreenToClient(rectContainer);

    CRect rectWnd;
    pane->GetWindowRect(rectWnd);
    pFrame->ScreenToClient(rectWnd);

    CRect rectUnion;
    rectUnion.UnionRect(rectWnd, rectContainer);
    if (rectUnion != rectContainer)
    {
        rectWnd.OffsetRect(rectContainer.left - rectWnd.left, rectContainer.top - rectWnd.top);
        if (rectWnd.Width() > rectContainer.Width())
        {
            rectWnd.right = rectWnd.left + rectContainer.Width();
        }
        if (rectWnd.Height() > rectContainer.Height())
        {
            rectWnd.bottom = rectWnd.top + rectContainer.Height();
        }

        pane->SetWindowPos(NULL, rectWnd.left, rectWnd.top, rectContainer.Width(), rectContainer.Height(), SWP_NOZORDER | SWP_NOACTIVATE);
    }
}

const TCHAR * const _STATE_DEBUG = _T("DebugWorkspace");
const TCHAR * const _STATE_GRAPH = _T("GraphWorkspace");
const TCHAR * const _STATE_NORMAL = _T("Workspace");
const TCHAR * const getWorkspaceString(WORKSPACE workspaceMode) {
    switch(workspaceMode)
    {
    case WORKSPACE_NORMAL:
        return _STATE_NORMAL;
    case WORKSPACE_GRAPH:
        return _STATE_GRAPH;
    case WORKSPACE_DEBUG:
        return _STATE_DEBUG;
    default:
        ATLASSERT(false);
    }
    return _STATE_NORMAL;
}

void MoveToFront(CDockablePane * pane, bool setActive)
{
    if (pane->IsTabbed())
    {
        HWND parentTabWnd;
        CMFCBaseTabCtrl* parentTabCtrl = pane->GetParentTabWnd(parentTabWnd);
        if (parentTabCtrl)
        {
            int tab = parentTabCtrl->GetTabFromHwnd(pane->GetSafeHwnd());
            parentTabCtrl->MoveTab(tab, 0);
            if (setActive)
                parentTabCtrl->SetActiveTab(0);
        }
    }
}

void CMainFrame::SetWorkspaceMode(WORKSPACE workspaceMode)
{
    if (m_workspaceMode != workspaceMode)
    {
        CString oldSection = getWorkspaceString(m_workspaceMode);
        m_workspaceMode = workspaceMode;
        CString newSection = getWorkspaceString(m_workspaceMode);
        theApp.SaveState(this, oldSection);
        if (!theApp.IsStateExists(newSection))
        {
            switch (m_workspaceMode)
            {
            case WORKSPACE_DEBUG:
                m_debugDataViews->ShowPane(TRUE, FALSE, TRUE);
                m_debugPropertyGridViews->ShowPane(TRUE, FALSE, TRUE);
                m_debugBreakpointView->ShowPane(TRUE, FALSE, TRUE);
                m_debugPropertiesView->ShowPane(TRUE, FALSE, TRUE);
                m_debugSearchView->ShowPane(TRUE, FALSE, TRUE);
                m_debugSearchView->ShowPane(TRUE, FALSE, TRUE);

#ifdef WORKSPACE_WINDOW
                m_Workspace->ShowPane(FALSE, FALSE, TRUE);
#endif
                m_Repository->ShowPane(FALSE, FALSE, TRUE);
                m_RepositoryFilter->ShowPane(FALSE, FALSE, TRUE);
                m_Workunits->ShowPane(FALSE, FALSE, TRUE);
                m_WorkunitsActive->ShowPane(FALSE, FALSE, TRUE);
                m_WorkunitsFilter->ShowPane(FALSE, FALSE, TRUE);
#ifdef DFU_WINDOWS
                m_Dfu->ShowPane(FALSE, FALSE, TRUE);
                m_DfuFilter->ShowPane(FALSE, FALSE, TRUE);
#endif
                m_Syntax->ShowPane(FALSE, FALSE, TRUE);
                m_Bookmarks->ShowPane(FALSE, FALSE, TRUE);
                m_Error->ShowPane(FALSE, FALSE, TRUE);
                break;

            case WORKSPACE_GRAPH:
                m_debugDataViews->ShowPane(FALSE, FALSE, TRUE);
                m_debugPropertyGridViews->ShowPane(FALSE, FALSE, TRUE);
                m_debugBreakpointView->ShowPane(FALSE, FALSE, TRUE);
                m_debugPropertiesView->ShowPane(TRUE, FALSE, TRUE);
                m_debugSearchView->ShowPane(FALSE, FALSE, TRUE);
                m_debugSearchView->ShowPane(FALSE, FALSE, TRUE);

#ifdef WORKSPACE_WINDOW
                m_Workspace->ShowPane(FALSE, FALSE, TRUE);
#endif
                m_Repository->ShowPane(FALSE, FALSE, TRUE);
                m_RepositoryFilter->ShowPane(TRUE, FALSE, TRUE);
                m_Workunits->ShowPane(FALSE, FALSE, TRUE);
                m_WorkunitsActive->ShowPane(FALSE, FALSE, TRUE);
                m_WorkunitsFilter->ShowPane(FALSE, FALSE, TRUE);
#ifdef DFU_WINDOWS
                m_Dfu->ShowPane(FALSE, FALSE, TRUE);
                m_DfuFilter->ShowPane(TRUE, FALSE, TRUE);
#endif
                m_Syntax->ShowPane(TRUE, FALSE, TRUE);
                m_Bookmarks->ShowPane(TRUE, FALSE, TRUE);
                m_Error->ShowPane(TRUE, FALSE, TRUE);
                break;

            default:
                ATLASSERT(false);
            }
        }
        else
        {
            SetRedraw(FALSE);
            theApp.EnableLoadWindowPlacement(FALSE);
            theApp.LoadState(this, newSection);

            CWnd* pWndChild=CWnd::FromHandle(m_hWndMDIClient); 
            pWndChild=pWndChild->GetWindow(GW_CHILD); 
            while(pWndChild) 
            { 
                if (pWndChild->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)))
                    ((CMDIChildWnd*)pWndChild)->SetHandles(NULL, this->m_hAccelTable);
                pWndChild=pWndChild->GetWindow(GW_HWNDNEXT); 
            } 

            SetRedraw(TRUE);
            RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_ALLCHILDREN);
            AdjustClientArea();
        }
    }
}

void CMainFrame::SetProgressBar(int nID, int pos, int layoutProgress)
{
    CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> ar;
    m_wndRibbonBar.GetElementsByID(nID, ar);

    for (int i = 0; i < ar.GetSize (); i++)
    {
        CMFCRibbonProgressBar* pProgress = DYNAMIC_DOWNCAST (CMFCRibbonProgressBar, ar[i]);
        if (pProgress != NULL && pProgress->GetPos () != layoutProgress)
            pProgress->SetPos(pos);
    }
}

BOOL CMainFrame::OnShowMDITabContextMenu(CPoint point, DWORD dwAllowedItems, BOOL bDrop)
{
    CMenu menu;
    VERIFY(menu.LoadMenu(bDrop ? IDR_POPUP_DROP_MDITABS : IDR_POPUP_MDITABS));

    CMenu* pPopup = menu.GetSubMenu(0);
    ASSERT(pPopup != NULL);

    if (pPopup)
    {
        if ((dwAllowedItems & AFX_MDI_CREATE_HORZ_GROUP) == 0)
        {
            pPopup->DeleteMenu(ID_MDI_NEW_HORZ_TAB_GROUP, MF_BYCOMMAND);
        }

        if ((dwAllowedItems & AFX_MDI_CREATE_VERT_GROUP) == 0)
        {
            pPopup->DeleteMenu(ID_MDI_NEW_VERT_GROUP, MF_BYCOMMAND);
        }

        if ((dwAllowedItems & AFX_MDI_CAN_MOVE_NEXT) == 0)
        {
            pPopup->DeleteMenu(ID_MDI_MOVE_TO_NEXT_GROUP, MF_BYCOMMAND);
        }

        if ((dwAllowedItems & AFX_MDI_CAN_MOVE_PREV) == 0)
        {
            pPopup->DeleteMenu(ID_MDI_MOVE_TO_PREV_GROUP, MF_BYCOMMAND);
        }

        if ((dwAllowedItems & AFX_MDI_CAN_BE_DOCKED) == 0)
        {
            pPopup->DeleteMenu(ID_MDI_TABBED_DOCUMENT, MF_BYCOMMAND);
        }

        //CMDIChildWndEx* pMDIChild = DYNAMIC_DOWNCAST(CMDIChildWndEx, MDIGetActive());
        //UINT command = pPopup->TrackPopupMenu(TPM_LEFTBUTTON | TPM_VERTICAL | TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD, point.x, point.y, pMDIChild);
        CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;
        if (pPopupMenu)
        {
            pPopupMenu->SetAutoDestroy(FALSE);
            pPopupMenu->Create(this, point.x, point.y, pPopup->GetSafeHmenu());
        }
    }

    return TRUE;

    //return CMDIFrameWndEx::OnShowMDITabContextMenu(point, AFX_MDI_CAN_BE_DOCKED, bDrop);

    //CMDIChildWndEx* pMDIChild = DYNAMIC_DOWNCAST(CMDIChildWndEx, MDIGetActive());
    //if (pMDIChild != NULL && !bDrop)
    //{
    //	CMenu * menu = this->GetSystemMenu(FALSE);

    //	UINT command = (UINT)menu->TrackPopupMenu(TPM_LEFTBUTTON | TPM_VERTICAL | TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD, point.x, point.y, pMDIChild);

    //	// See MSDN about "GetSystemMenu".  Returns codes greater than
    //	//  0xF000 (which happens to be SC_SIZE) are sent with WM_SYSCOMMAND
    //	if(command >= SC_SIZE)
    //	{
    //		::PostMessage(pMDIChild->GetSafeHwnd(), WM_SYSCOMMAND, command, 0L);
    //	}
    //	else if(command != 0)
    //	{
    //		// Non SC_* commands don't work with WM_SYSCOMMAND.  We could handle
    //		// the situation here by using WM_COMMAND, but there's other places
    //		// where the "window menu" is dealt with that wouldn't have the same handling
    //		// (like CMDICommandBarCtrl::OnNcLButtonDown).  To help prevent
    //		// errors, do an assert to warn.  Instead of depending on this base
    //		// implementation, you should override handling UWM_MDICHILDSHOWTABCONTEXTMENU
    //		// in a derived class, and do your own context menu there.
    //		// See the "TabDemo" sample for an example.
    //		ATLASSERT(0 && 
    //			"You've tried to put a non SC_* command in the window menu.  "
    //			"Please override the default context menu handling, and use a custom menu.");
    //	}
    //}

    //return 0;

    ////CMenu menu;
    ////VERIFY(menu.LoadMenu (IDR_POPUP_TOOLBAR));

    ////CMenu* pPopup = menu.GetSubMenu(0);
    ////ASSERT(pPopup != NULL);

    ////if (pPopup)
    ////{
    ////	CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;
    ////	pPopupMenu->Create (this, point.x, point.y, pPopup->Detach ());
    ////}

    ////return 0;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
        return -1;

    GetIConfig(QUERYBUILDER_CFG)->ConnectSlot(boost::ref(*this));

    // set the visual manager and style based on persisted value
    OnApplicationLook(theApp.m_nAppLook);

    SetMDIMode();

    m_wndRibbonBar.Create(this);
    InitializeRibbon();

    if (!m_wndStatusBar.Create(this))
    {
        TRACE0("Failed to create status bar\n");
        return -1;      // fail to create
    }

    m_wndStatusBar.AddElement(new CMFCRibbonStatusBarPane(PANE_DEFAULT, _T(""), TRUE), _T("Default"));
    m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(PANE_POS, _T(""), TRUE), _T("Cursor Pos"));
    m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(PANE_USER, _T(""), TRUE), _T("User"));
    m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(PANE_LABEL, _T(""), TRUE), _T("Environment"));
    m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(PANE_QUEUE, _T(""), TRUE), _T("Queue"));
    m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(PANE_CLUSTER, _T(""), TRUE), _T("Cluster"));

    //m_sbar.SetPaneWidth(PANE_LABEL, 60);
    //m_sbar.SetPaneWidth(PANE_USER, 60);
    //m_sbar.SetPaneWidth(PANE_CLUSTER, 60);
    //m_sbar.SetPaneWidth(PANE_POS, 30);


    // enable Visual Studio 2005 style docking window behavior
    CDockingManager::SetDockingMode(DT_SMART);
    // enable Visual Studio 2005 style docking window auto-hide behavior
    EnableAutoHidePanes(CBRS_ALIGN_ANY);

    // Load menu item image (not placed on any standard toolbars):
    CMFCToolBar::AddToolBarForImageCollection(IDR_MENU_IMAGES, theApp.m_bHiColorIcons ? IDB_MENU_IMAGES_24 : 0);

    // create docking windows
    if (!CreateDockingWindows())
    {
        TRACE0("Failed to create docking windows\n");
        return -1;
    }

    InitializeDocking(ID_VIEW_RESETDOCKEDWINDOWS_DEFAULT_RIGHT);

    // Enable enhanced windows management dialog
    EnableWindowsDialog(ID_WINDOW_MANAGER, IDS_WINDOWS_MANAGER, TRUE);

    ATLASSERT(m_hWnd != NULL);
    m_fr.m_hWndOwner = m_hWnd;
    m_fr.m_findReplaceMsg = WM_COMMAND;
    m_fr.m_findReplaceMsgParam = ID_EDIT_FINDREPLACECMD;

    theApp.GetContextMenuManager()->AddMenu(_T("Advanced"), IDR_POPUP_ADVANCED);
    PostMessage(CWM_INITIALIZE);

    return 0;
}

void CMainFrame::OnClose()
{
    if (m_threadSave.Size() > 0)
    {
        CString prog_title;
        prog_title.LoadString(IDR_MAINFRAME);
        switch(MessageBox(_T("Warning:  Workunits are currently being saved in the background, are you sure you want to exit?"), prog_title, MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON1))
        {
        case IDYES:
            break;
        case IDNO:
            return;
        }
    }
    StlLinked<Dali::IDali> server = Dali::AttachDali(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT), _T("Dali"));
    Dali::IWorkunitVector workunits;
    server->GetWorkunits(_T(""), GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_USER), _T("debugging"), _T(""), _T(""), 100, &workunits);
    server->GetWorkunits(_T(""), GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_USER), _T("debugging_running"), _T(""), _T(""), 100, &workunits);
    if (workunits.size())
    {
        CString prog_title;
        prog_title.LoadString(IDR_MAINFRAME);
        switch(MessageBox(_T("Warning:  You have left workunits in the debugging state.  Do you want to abort prior to exiting?"), prog_title, MB_YESNOCANCEL | MB_ICONQUESTION | MB_DEFBUTTON1))
        {
        case IDYES:
            server->AbortWorkunits(&workunits);
            break;
        case IDNO:
            break;
        case IDCANCEL:
            return;
        }
    }

    DoWorkspaceSave();
    EnumChildWindows(GetSafeHwnd(), ChildEnumProc, CWM_CLOSEALL);
    ReleaseAllSingletons();

    OnSelectRibbon((WPARAM)RIBBON_HOME, 0L);

    CMDIFrameWndEx::OnClose();
}

void CMainFrame::OnTimer(UINT nID)
{
    switch (nID)
    {
    case TIMER_AUTOSAVE:
#ifndef _DEBUG
        DoWorkspaceSave();
#endif
        break;
    case TIMER_RESTORESTATE:
        if (m_persistedWindows.size())
            PostMessage(UM_RESTORESTATE);
        else
        {
            for(CPersistedItemVector::iterator itr = m_activeWindows.begin(); itr != m_activeWindows.end(); ++itr)
            {
                if (itr->get()->IsActive())
                {
                    int targetGroup = itr->get()->GetGroupIndex();
                    const CObList * tabbedGroups = &GetMDITabGroups();
                    for (POSITION pos = tabbedGroups->GetHeadPosition(); pos != NULL;)
                    {
                        CMFCTabCtrl* tabCtrl = DYNAMIC_DOWNCAST(CMFCTabCtrl, tabbedGroups->GetNext(pos));
                        if (--targetGroup < 0)
                        {
                            tabCtrl->SetActiveTab(itr->get()->GetTabIndex());
                            break;
                        }
                    }
                }
            }
            m_activeWindows.clear();
            DoWorkspaceLoad(m_Workspace->GetCurrentWorkspace());
            KillTimer(TIMER_RESTORESTATE);
        }
        break;
    }
}

void CMainFrame::OnActivateApp(BOOL bActive, DWORD dwThreadID)
{
    CMDIFrameWndEx::OnActivateApp(bActive, dwThreadID);
    //static HWND hWnd = NULL;
    //if (bActive)
    //{
    //	if (hWnd && ::IsWindow(hWnd))
    //	{
    //		::SetFocus(hWnd);
    //	}
    //}
    //else
    //{
    //	hWnd = ::GetFocus();
    //}
}
void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
    CMDIFrameWndEx::OnSize(nType, cx, cy);
    //if (m_fullRegDebugPath.GetLength() > 0)
    //	m_CategoryDebug->IsVisible() ? m_dockManager.SaveState(m_fullRegDebugPath, IDR_MAINFRAME) : m_dockManager.SaveState(m_fullRegWorkspacePath, IDR_MAINFRAME);
}

LRESULT CMainFrame::OnInitialize(WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);

    EnableMDITabsLastActiveActivation();
    //m_stateMgr.Initialize(regPath, m_hWnd, SW_SHOWMAXIMIZED);
    //m_stateMgr.Restore();

    //DoLogout();
    DoLogin();

    //UISetCheck(ID_EDIT_SHOW_ORPHANS,1);
    //UISetCheck(ID_EDIT_SHOW_MATCHING,1);
    //UISetCheck(ID_EDIT_SHOW_NOT_MATCHING,1);

   return 0;
}

LRESULT CMainFrame::OnStatusUpdate(WPARAM wParam, LPARAM lParam)
{
    CString *status = reinterpret_cast<CString *>(lParam);
    std::_tstring text = _T("Ready");
    if (status)
    {
        text = *status;
        delete status;
    }
    CMFCRibbonBaseElement * elem = wParam == 0 ? m_wndStatusBar.GetElement(wParam) : m_wndStatusBar.GetExElement(wParam - 1);
    elem->SetText(text.c_str());

    //if (wParam > 0)
    {
        if (text.length())
            m_wndStatusBar.RecalcLayout();
        m_wndStatusBar.RedrawWindow();
    }

    return 0;
}

LRESULT CMainFrame::OnRestoreState(WPARAM wParam, LPARAM lParam)
{
    CPersistedItemVector::const_iterator itr = m_persistedWindows.begin();
    if (itr != m_persistedWindows.end())
    {
        HWND hWnd = itr->get()->OpenMDIChild(m_hWndMDIClient);
        if (hWnd)
        {
            int groupIdx = itr->get()->GetGroupIndex();
            const CObList * tabbedGroups = &GetMDITabGroups();
            while (tabbedGroups->GetSize() <= groupIdx)
            {
                MDITabNewGroup();
                tabbedGroups = &GetMDITabGroups();
            }
            m_activeWindows.push_back(itr->get());
            m_persistedWindows.erase(itr);
        }
    }
    return 0;
}

LRESULT CMainFrame::OnSelectRibbon(WPARAM wParam, LPARAM lParam)
{
    BOOL bRecalc = FALSE; 
    BOOL isHidden = m_wndRibbonBar.GetHideFlags() != 0;

    if (m_wndCaptionBar.IsWindowVisible())
    {
        m_wndCaptionBar.ShowWindow(SW_HIDE);
        RecalcLayout(FALSE);
    }

    switch((RIBBON)wParam)
    {
    case RIBBON_HOME:
        bRecalc = m_wndRibbonBar.HideAllContextCategories();
        if (!isHidden)
            m_wndRibbonBar.SetActiveCategory(m_CategoryHome);
        break;
    case RIBBON_RESULT:
        if (!m_CategoryResult->IsVisible())
        {
            bRecalc = m_wndRibbonBar.HideAllContextCategories();
            m_wndRibbonBar.ShowContextCategories(wParam);
            if (!isHidden)
                m_wndRibbonBar.ActivateContextCategory(wParam);
            bRecalc = TRUE;
        }
        break;
    case RIBBON_GRAPH:
        if (!m_CategoryGraph->IsVisible())
        {
            bRecalc = m_wndRibbonBar.HideAllContextCategories();
            m_wndRibbonBar.ShowContextCategories(wParam);
            if (!isHidden)
                m_wndRibbonBar.ActivateContextCategory(wParam);
            bRecalc = TRUE;
        }
        break;
    case RIBBON_GRAPH2:
        if (!m_CategoryGraph2->IsVisible())
        {
            bRecalc = m_wndRibbonBar.HideAllContextCategories();
            m_wndRibbonBar.ShowContextCategories(wParam);
            if (!isHidden)
                m_wndRibbonBar.ActivateContextCategory(wParam);
            bRecalc = TRUE;
        }
        break;
    case RIBBON_DEBUG:
        if (!m_CategoryDebug->IsVisible())
        {
            bRecalc = m_wndRibbonBar.HideAllContextCategories();
            m_wndRibbonBar.ShowContextCategories(wParam);
            if (!isHidden)
                m_wndRibbonBar.ActivateContextCategory(wParam);
            bRecalc = TRUE;
        }
        if (!m_CategoryDebugAdvanced->IsVisible())
        {
            bRecalc = m_wndRibbonBar.HideAllContextCategories();
            m_wndRibbonBar.ShowContextCategories(wParam);
            if (!isHidden)
                m_wndRibbonBar.ActivateContextCategory(wParam);
            bRecalc = TRUE;
        }
        break;
    case RIBBON_BROWSER:
        if (!m_CategoryBrowser->IsVisible())
        {
            bRecalc = m_wndRibbonBar.HideAllContextCategories();
            m_wndRibbonBar.ShowContextCategories(wParam);
            if (!isHidden)
                m_wndRibbonBar.ActivateContextCategory(wParam);
            bRecalc = TRUE;
        }
    case RIBBON_COMPARE:
        if (!m_CategoryCompare->IsVisible())
        {
            bRecalc = m_wndRibbonBar.HideAllContextCategories();
            m_wndRibbonBar.ShowContextCategories(wParam);
            if (!isHidden)
                m_wndRibbonBar.ActivateContextCategory(wParam);
            bRecalc = TRUE;
        }
        break;
    }

    if (bRecalc)
    {
        m_wndRibbonBar.RecalcLayout();
        m_wndRibbonBar.RedrawWindow();
        SendMessage(WM_NCPAINT, 0, 0);

        switch((RIBBON)wParam)
        {
        case RIBBON_DEBUG:
            SetWorkspaceMode(WORKSPACE_DEBUG);
            break;
        case RIBBON_GRAPH:
        case RIBBON_GRAPH2:
            SetWorkspaceMode(WORKSPACE_GRAPH);
            break;
        default:
            SetWorkspaceMode(WORKSPACE_NORMAL);
        }
    }
    m_prevRibbon = (RIBBON)wParam;
    return 0;
}

LRESULT CMainFrame::OnMDIChildActivate(WPARAM wParam, LPARAM lParam)
{
    if (wParam)
    {
        ::SendMessage((HWND)wParam, WM_MDIACTIVATE, (WPARAM)GetActiveWindow(), 0);
        ::SetFocus((HWND)wParam);
    }
    return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    if( !CMDIFrameWndEx::PreCreateWindow(cs) )
        return FALSE;
    // TODO: Modify the Window class or styles here by modifying
    //  the CREATESTRUCT cs

    return TRUE;
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
    if (m_FindDlg.IsWindow() && m_FindDlg.IsDialogMessage(pMsg))
        return TRUE;

    if (m_ReplaceDlg.IsWindow() && m_ReplaceDlg.IsDialogMessage(pMsg)) 
        return TRUE;

    //if (DockedWindowPreTranslate(pMsg))
    //	return TRUE;

    if ((pMsg->message == WM_KEYDOWN )&& (pMsg->wParam == VK_F6))
    {
        int d = 0;
    }

    return CMDIFrameWndEx::PreTranslateMessage(pMsg);
}

void CMainFrame::InitializeRibbon()
{
    CString strTemp;
    strTemp.LoadString(IDS_RIBBON_FILE);

    // Load panel images:
    m_PanelImages.SetImageSize(CSize(16, 16));
    m_PanelImages.Load(IDB_BUTTONS);

    // Init main button:
    m_MainButton.SetImage(IDB_MAIN);
    m_MainButton.SetText(_T("\nf"));
    m_MainButton.SetToolTipText(strTemp);

    m_wndRibbonBar.SetApplicationButton(&m_MainButton, CSize (45, 45));

    {//  Main Panel
        CMFCRibbonMainPanel* pMainPanel = m_wndRibbonBar.AddMainCategory(strTemp, IDB_FILESMALL, IDB_FILELARGE);

        CMFCRibbonButton * pBtnNew = new CMFCRibbonButton(ID_FILE_NEW, _T("&New"), 0, 0);
        pBtnNew->AddSubItem (new CMFCRibbonButton (ID_FILE_NEWBUILDER, _T("New &Builder"), 0, 0, TRUE));
        //pBtnNew->AddSubItem (new CMFCRibbonButton (ID_FILE_NEWSPRAY, _T("New &Spray..."), 0, 0, TRUE));
        pMainPanel->Add(pBtnNew);

        m_openRibbonButton = new CMFCRibbonButton(ID_FILE_OPEN, _T("&Open...\no"), 1, 1);
        //pBtnOpen->AddSubItem(new CMFCRibbonLabel(_T("Open item")));
        m_openRibbonButton->AddSubItem(new CMFCRibbonButton(ID_FILE_OPENBUILDER, _T("Open &Builder..."), 1, 1, TRUE));
        m_openRibbonButton->AddSubItem(new CMFCRibbonButton(ID_FILE_OPENATTRIBUTE, _T("Open &Source..."), 1, 1, TRUE));
        m_openRibbonButton->AddSubItem(new CMFCRibbonButton(ID_FILE_OPENWORKUNIT, _T("Open &Workunit..."), 1, 1, TRUE));
        pMainPanel->Add(m_openRibbonButton);

        pMainPanel->Add(new CMFCRibbonButton(ID_FILE_SAVE, _T("&Save\ns"), 2, 2));
        pMainPanel->Add(new CMFCRibbonButton(ID_FILE_SAVE_AS, _T("Save &As..."), 3, 3));
        pMainPanel->Add(new CMFCRibbonButton(ID_FILE_NEWWORKSPACE, _T("New &Workspace"), 3, 3));
        pMainPanel->Add(new CMFCRibbonSeparator(TRUE));

        strTemp.LoadString(IDS_RIBBON_PRINT);
        CMFCRibbonButton* pBtnPrint = new CMFCRibbonButton(ID_FILE_PRINT, strTemp, 4, 4);
        pBtnPrint->SetKeys(_T("p"), _T("w"));
        strTemp.LoadString(IDS_RIBBON_PRINT_LABEL);
        pBtnPrint->AddSubItem(new CMFCRibbonLabel(strTemp));
        strTemp.LoadString(IDS_RIBBON_PRINT);
        pBtnPrint->AddSubItem(new CMFCRibbonButton(ID_FILE_PRINT, strTemp, 13, 13, TRUE));
        strTemp.LoadString(IDS_RIBBON_PRINT_SETUP);
        pBtnPrint->AddSubItem(new CMFCRibbonButton(ID_FILE_PRINT_SETUP, strTemp, 23, 23, TRUE));
        pMainPanel->Add(pBtnPrint);
        pMainPanel->Add(new CMFCRibbonSeparator(TRUE));

        strTemp.LoadString(IDS_RIBBON_CLOSE);
        pMainPanel->Add(new CMFCRibbonButton(ID_FILE_CLOSE, strTemp, 8, 8));

        pMainPanel->Add(new CMFCRibbonSeparator(TRUE));
        pMainPanel->Add(new CMFCRibbonButton(ID_FILE_LOGOUT, _T("&Logout"), 17, 17, TRUE));
        pMainPanel->Add(new CMFCRibbonButton(ID_FILE_CHANGEPASSWORD, _T("Change &Password..."), 25, 25, TRUE));

        strTemp.LoadString(IDS_RIBBON_RECENT_DOCS);
        pMainPanel->AddRecentFilesList(strTemp);

        pMainPanel->AddToBottom(new CMFCRibbonMainPanelButton(ID_FILE_PREFERENCES, _T("Pre&ferences..."), 26));

        strTemp.LoadString(IDS_RIBBON_EXIT);
        pMainPanel->AddToBottom(new CMFCRibbonMainPanelButton(ID_APP_EXIT, strTemp, 27));
    }

    {	//Home Ribbon
        m_CategoryHome = m_wndRibbonBar.AddCategory(_T("&Home"), IDB_WRITESMALL, IDB_WRITELARGE);

        CMFCRibbonPanel* pPanelBuilder = m_CategoryHome->AddPanel(_T("Current"), m_PanelImages.ExtractIcon(27));


        CMFCRibbonButton* pBtnSubmit = new CMFCRibbonButton(ID_ECL_GO, _T("Submit\ns"), 43,7);
        pBtnSubmit->SetMenu(IDR_POPUP_EEGO2, TRUE);
        pPanelBuilder->Add(pBtnSubmit);
        pPanelBuilder->Add(new CMFCRibbonSeparator());
        pPanelBuilder->Add(new CMFCRibbonButton(ID_ECL_SYNCTOC, _T("Locate\nl"), 42, 6));

        // Create "Clipboard" panel:
        strTemp.LoadString(IDS_RIBBON_CLIPBOARD);
        CMFCRibbonPanel* pPanelClipboard = m_CategoryHome->AddPanel(strTemp, m_PanelImages.ExtractIcon(27));

        strTemp.LoadString(IDS_RIBBON_PASTE);
        pPanelClipboard->Add(new CMFCRibbonButton(ID_EDIT_PASTE, strTemp, 0, 0));

        strTemp.LoadString(IDS_RIBBON_CUT);
        pPanelClipboard->Add(new CMFCRibbonButton(ID_EDIT_CUT, strTemp, 1));
        strTemp.LoadString(IDS_RIBBON_COPY);
        pPanelClipboard->Add(new CMFCRibbonButton(ID_EDIT_COPY, strTemp, 2));

        // Create "Undo/Redo" panel:
        CMFCRibbonPanel* pPanelAction = m_CategoryHome->AddPanel(_T("Editor\ne"), m_PanelImages.ExtractIcon(27));
        pPanelAction->Add(new CMFCRibbonButton(ID_EDIT_UNDO, _T("Undo\nu"), 11));
        pPanelAction->Add(new CMFCRibbonButton(ID_EDIT_REDO, _T("Redo\nr"), 12));
        pPanelAction->Add(new CMFCRibbonSeparator());
        pPanelAction->Add(new CMFCRibbonButton(ID_EDIT_SELECT_ALL, _T("Select All\ns")));
        pPanelAction->Add(new CMFCRibbonButton(ID_EDIT_DELETE, _T("Delete\nd")));
#ifdef ENABLE_RIBBON_LAUNCH_BUTTON
        pPanelAction->EnableLaunchButton(ID_EDIT_ADVANCED);
#else
        pPanelAction->Add(new CMFCRibbonButton(ID_EDIT_ADVANCED, _T("Advanced...\nm")));
#endif
        pPanelAction->Add(new CMFCRibbonSeparator());
        m_buttMacroRecord = new CMFCRibbonButton(ID_MACRO_RECORD, _T("Record"), 47);
        pPanelAction->Add(m_buttMacroRecord);
        pPanelAction->Add(new CMFCRibbonButton(ID_MACRO_PLAY, _T("Play"), 50));

        // Create "Search" panel:
        CMFCRibbonPanel* pPanelLocate = m_CategoryHome->AddPanel(_T("Search\ns"), m_PanelImages.ExtractIcon(27));
        CMFCRibbonButton * pButtFind = new CMFCRibbonButton(ID_EDIT_FIND, _T("Find\nf"), 4, 3);
        pPanelLocate->Add(pButtFind);
        pPanelLocate->Add(new CMFCRibbonButton(ID_EDIT_FINDNEXT, _T("Next\nn"), 6));
        pPanelLocate->Add(new CMFCRibbonButton(ID_EDIT_FINDPREV, _T("Previous\nv"), 7));
        pPanelLocate->Add(new CMFCRibbonButton(ID_EDIT_REPLACE, _T("Replace\ne"), 5));
        pButtFind->AddSubItem(new CMFCRibbonButton(ID_EDIT_GOTO, _T("Goto\ng"), 30));

        CMFCRibbonPanel* pPanelSyntax = m_CategoryHome->AddPanel(_T("Syntax"), m_PanelImages.ExtractIcon(27));
        pPanelSyntax->Add(new CMFCRibbonButton(ID_ECL_CHECKSYNTAX, _T("&Check\nc"), 38, 5));
        pPanelSyntax->Add(new CMFCRibbonButton(ID_ECL_NEXTERROR, _T("Next\nn"), 41));
        pPanelSyntax->Add(new CMFCRibbonButton(ID_ECL_PREVIOUSERROR, _T("Previous\np"), 40));
        pPanelSyntax->Add(new CMFCRibbonButton(ID_ECL_CLEARERRORS, _T("Clear\nl"), 39));

        CMFCRibbonPanel* pPanelECL = m_CategoryHome->AddPanel(_T("ECL"), m_PanelImages.ExtractIcon(27));
        pPanelECL->Add(new CMFCRibbonButton(ID_ECL_CHECKCOMPLEXITY, _T("Show Complexity\no")));
        pPanelECL->Add(new CMFCRibbonButton(ID_ECL_CHECKDEPENDENCY, _T("Show Dependency\nd")));
        pPanelECL->Add(new CMFCRibbonButton(ID_REPOSITORY_COPY, _T("Copy Title\nt")));
        pPanelECL->Add(new CMFCRibbonSeparator());
        pPanelECL->Add(new CMFCRibbonButton(ID_ECL_SYNCTOC, _T("Locate Current\na")));
        pPanelECL->Add(new CMFCRibbonButton(ID_ECL_GOTOSYNCTOC, _T("Locate Selected\nt")));
        pPanelECL->Add(new CMFCRibbonButton(ID_ECL_GOTO, _T("Open Selected\np")));

        // Create and add a "Windows" panel:
        strTemp.LoadString(IDS_RIBBON_WINDOW);
        CMFCRibbonPanel* pPanelWindow = m_CategoryHome->AddPanel(strTemp, m_PanelImages.ExtractIcon (7));

        strTemp.LoadString(IDS_RIBBON_WINDOWS);
        CMFCRibbonButton* pBtnWindows = new CMFCRibbonButton(ID_WINDOW_MANAGER, strTemp, -1, 4);
        pBtnWindows->SetMenu(IDR_WINDOWS_MENU, TRUE);
        pPanelWindow->Add(pBtnWindows);
        pPanelWindow->Add(new CMFCRibbonSeparator());
        pPanelWindow->Add(new CMFCRibbonButton(ID_WINDOW_CASCADE, _T("Cascade\nc")));
        pPanelWindow->Add(new CMFCRibbonButton(ID_WINDOW_TILE_HORZ, _T("Tile\nt")));
        pPanelWindow->Add(new CMFCRibbonButton(ID_WINDOW_ARRANGE, _T("Arrange Icons\na")));
        pPanelWindow->Add(new CMFCRibbonSeparator());
        pPanelWindow->Add(new CMFCRibbonButton(ID_SORTING_SORTALPHABETIC, _T("Sort\ns")));
        pPanelWindow->Add(new CMFCRibbonButton(ID_SORTING_GROUPBYTYPE, _T("Group\ng")));
        pPanelWindow->Add(new CMFCRibbonButton(ID_SORTING_DEGROUP, _T("Degroup\nd")));

        CMFCRibbonPanel* pPanelWorkspace = m_CategoryHome->AddPanel(_T("Workspace(s)"), m_PanelImages.ExtractIcon(27));
        pPanelWorkspace->Add(new CMFCRibbonButton(ID_WORKSPACE_ADD, _T("Add\na")));
        pPanelWorkspace->Add(new CMFCRibbonButton(ID_WORKSPACE_REMOVE, _T("Remove\nr")));
        m_workspaceCombo = new CMFCRibbonComboBox(ID_WORKSPACE_LIST, FALSE); 
        pPanelWorkspace->Add(m_workspaceCombo);
    }

    {//  View Ribbon
        m_CategoryView = m_wndRibbonBar.AddCategory(_T("&View"), IDB_WRITESMALL, IDB_WRITELARGE);

        CMFCRibbonPanel* pPanelNormal = m_CategoryView->AddPanel(_T("Normal\nd"), m_PanelImages.ExtractIcon (7));
#ifdef WORKSPACE_WINDOW
        pPanelNormal->Add(new CMFCRibbonCheckBox(ID_VIEW_PROJECT, _T("Workspace\nw")));
#endif
        pPanelNormal->Add(new CMFCRibbonCheckBox(ID_VIEW_REPOSITORY, _T("Repository\nr")));
        pPanelNormal->Add(new CMFCRibbonCheckBox(ID_VIEW_REPOSITORYFILTER, _T("Repository Find\nf")));
        pPanelNormal->Add(new CMFCRibbonSeparator());
        pPanelNormal->Add(new CMFCRibbonCheckBox(ID_VIEW_WORKUNITSTREE, _T("Workunit History\nw")));
        pPanelNormal->Add(new CMFCRibbonCheckBox(ID_VIEW_ACTIVEWORKUNITSTREE, _T("Active Workunits\na")));
        pPanelNormal->Add(new CMFCRibbonCheckBox(ID_VIEW_FILTEREDWORKUNITTREEFRAME, _T("Workunit Find\nf")));
        pPanelNormal->Add(new CMFCRibbonSeparator());
        pPanelNormal->Add(new CMFCRibbonCheckBox(ID_VIEW_DFU, _T("Datasets\nd")));
        pPanelNormal->Add(new CMFCRibbonCheckBox(ID_VIEW_DFUFILTER, _T("Dataset Find\nf")));
        pPanelNormal->Add(new CMFCRibbonSeparator());
        pPanelNormal->Add(new CMFCRibbonCheckBox(ID_VIEW_SYNTAX, _T("Syntax Errors\ns")));
        pPanelNormal->Add(new CMFCRibbonSeparator());
        pPanelNormal->Add(new CMFCRibbonCheckBox(ID_VIEW_BOOKMARKS, _T("Bookmarks\ns")));
        pPanelNormal->Add(new CMFCRibbonSeparator());
        pPanelNormal->Add(new CMFCRibbonCheckBox(ID_VIEW_ERROR, _T("Error Log\ne")));
        pPanelNormal->Add(new CMFCRibbonSeparator());
        strTemp.LoadString(IDS_RIBBON_STATUSBAR);
        pPanelNormal->Add(new CMFCRibbonCheckBox(ID_VIEW_STATUS_BAR, strTemp));

        CMFCRibbonPanel* pPanelDebug = m_CategoryView->AddPanel(_T("Debug\nd"), m_PanelImages.ExtractIcon(7));
        pPanelDebug->Add(new CMFCRibbonCheckBox(ID_VIEW_DEBUGBREAKPOINTS, _T("Breakpoints\nw")));
        pPanelDebug->Add(new CMFCRibbonCheckBox(ID_VIEW_DEBUGPROPERTYGRID, _T("Property Grid")));
        pPanelDebug->Add(new CMFCRibbonCheckBox(ID_VIEW_DEBUGDATA, _T("Data\nf")));
        pPanelDebug->Add(new CMFCRibbonSeparator());
        pPanelDebug->Add(new CMFCRibbonCheckBox(ID_VIEW_DEBUGPROPERTIES, _T("Properties\nr")));
        pPanelDebug->Add(new CMFCRibbonCheckBox(ID_VIEW_DEBUGSEARCH, _T("Search\nr")));

        CMFCRibbonPanel* pPanelResetDockedLeft = m_CategoryView->AddPanel(_T("Reset Left"), m_PanelImages.ExtractIcon (7));
        pPanelResetDockedLeft->Add(new CMFCRibbonButton(ID_VIEW_RESETDOCKEDWINDOWS_DEFAULT_LEFT, _T("Default\nd")));
        pPanelResetDockedLeft->Add(new CMFCRibbonButton(ID_VIEW_RESETDOCKEDWINDOWS_ALLPINNED_LEFT, _T("All pinned\np")));
        pPanelResetDockedLeft->Add(new CMFCRibbonButton(ID_VIEW_RESETDOCKEDWINDOWS_ALLUNPINNED_LEFT, _T("All unpinned\nu")));

        CMFCRibbonPanel* pPanelResetDockedRight = m_CategoryView->AddPanel(_T("Reset Right"), m_PanelImages.ExtractIcon (7));
        pPanelResetDockedRight->Add(new CMFCRibbonButton(ID_VIEW_RESETDOCKEDWINDOWS_DEFAULT_RIGHT, _T("Default\nd")));
        pPanelResetDockedRight->Add(new CMFCRibbonButton(ID_VIEW_RESETDOCKEDWINDOWS_ALLPINNED_RIGHT, _T("All pinned\np")));
        pPanelResetDockedRight->Add(new CMFCRibbonButton(ID_VIEW_RESETDOCKEDWINDOWS_ALLUNPINNED_RIGHT, _T("All unpinned\nu")));
    }

    {//  Format Ribbon
        m_CategoryResult = m_wndRibbonBar.AddContextCategory(_T("F&ormat"), _T("Results"), RIBBON_RESULT, AFX_CategoryColor_Green, IDB_COLORSMALL, IDB_COLORLARGE);

        CMFCRibbonPanel* pPanelFont = m_CategoryResult->AddPanel(_T("Font"), m_PanelImages.ExtractIcon (7));
        pPanelFont->Add(new CMFCRibbonButton(ID_FORMAT_FONTSTYLE, _T("Font Style\ns")));
        pPanelFont->Add(new CMFCRibbonCheckBox(ID_FORMAT_FONTBOLD, _T("Bold\nb")));
        pPanelFont->Add(new CMFCRibbonCheckBox(ID_FORMAT_FONTITALIC, _T("Italic\ni")));
        pPanelFont->Add(new CMFCRibbonSeparator());
        //pPanelFont->Add(new CMFCRibbonButton(ID_FORMAT_FONTCOLOR, _T("Font Color")));
        //pPanelFont->Add(new CMFCRibbonButton(ID_FORMAT_FILLCOLOR, _T("Fill Color")));

        CMFCRibbonColorButton* colorButtonFont = new CMFCRibbonColorButton(ID_FORMAT_FONTCOLOR, _T("Font Color\nc"), 6, RGB(0, 0, 0));
        colorButtonFont->EnableAutomaticButton(_T("Automatic\na"), RGB(0, 0, 0));
        colorButtonFont->EnableOtherButton(_T("More Colors...\nm"), _T("More Colors"));
        pPanelFont->Add(colorButtonFont);

        CMFCRibbonColorButton* colorButtonFill = new CMFCRibbonColorButton(ID_FORMAT_FILLCOLOR, _T("Fill Color\nf"), 7, RGB(255, 255, 255));
        colorButtonFill->EnableAutomaticButton(_T("Automatic\na"), RGB(255, 255, 255));
        colorButtonFill->EnableOtherButton(_T("More Colors...\nm"), _T("More Colors"));
        pPanelFont->Add(colorButtonFill);
    }

    {//  Browser Ribbon
        m_CategoryBrowser = m_wndRibbonBar.AddContextCategory(_T("&Browser"), _T("ECL Watch"), RIBBON_BROWSER, AFX_CategoryColor_Blue, IDB_WRITESMALL, IDB_WRITELARGE);

        CMFCRibbonPanel* pPanelNavigation = m_CategoryBrowser->AddPanel(_T("Navigation"), m_PanelImages.ExtractIcon(7));
        CMFCRibbonButton * pButtLaunch = new CMFCRibbonButton(ID_BROWSER_NEW, _T("Launch\nl"), 44, 8);
        pPanelNavigation->Add(pButtLaunch);
        pButtLaunch->AddSubItem(new CMFCRibbonButton(ID_BROWSER_IE, _T("Force Internet Explorer\ni"), 44));
        pPanelNavigation->Add(new CMFCRibbonSeparator());
        pPanelNavigation->Add(new CMFCRibbonButton(ID_BROWSER_STOP, _T("Stop\ns"), 45));
        pPanelNavigation->Add(new CMFCRibbonButton(ID_BROWSER_REFRESH, _T("Refresh\nr"), 42));
        pPanelNavigation->Add(new CMFCRibbonSeparator());
        pPanelNavigation->Add(new CMFCRibbonButton(ID_BROWSER_BACK, _T("Back\nb"), 7));
        pPanelNavigation->Add(new CMFCRibbonButton(ID_BROWSER_FORWARD, _T("Forward\nf"), 6));
    }

    {//  Graph Ribbon
        m_CategoryGraph = m_wndRibbonBar.AddContextCategory(_T("&Graph"), _T("Graph"), RIBBON_GRAPH, AFX_CategoryColor_Orange, IDB_WRITESMALL, IDB_WRITELARGE);

        CMFCRibbonPanel* pPanelNavigation = m_CategoryGraph->AddPanel(_T("Navigation\nn"), m_PanelImages.ExtractIcon (7));
        pPanelNavigation->Add(new CMFCRibbonButton(ID_GRAPH_LAYOUT, _T("Layout\nl"), 42, 6));
        pPanelNavigation->Add(new CMFCRibbonSeparator());
        pPanelNavigation->Add(new CMFCRibbonButton(ID_BROWSER_BACK, _T("Back\nb"), 7));
        pPanelNavigation->Add(new CMFCRibbonButton(ID_BROWSER_FORWARD, _T("Forward\nf"), 6));

        CMFCRibbonPanel* pPanelZoom = m_CategoryGraph->AddPanel(_T("Zoom"), m_PanelImages.ExtractIcon (7));
        pPanelZoom->Add(new CMFCRibbonButton(ID_GRAPH_ZOOMTOFIT, _T("To Fit\nf"), 33));
        pPanelZoom->Add(new CMFCRibbonButton(ID_GRAPH_ZOOMTOWIDTH, _T("To Width\nw"), 46));

        CMFCRibbonPanel* pPanelRunning = m_CategoryGraph->AddPanel(_T("Running"), m_PanelImages.ExtractIcon (7));
        m_graphRefresh = new CMFCRibbonButton(ID_GRAPH_REFRESH, _T("Refresh (30)\nr"), 42);
        pPanelRunning->Add(m_graphRefresh);
        pPanelRunning->Add(new CMFCRibbonCheckBox(ID_GRAPH_FOLLOWACTIVE, _T("Follow Active\nf")));
        pPanelRunning->Add(new CMFCRibbonCheckBox(ID_GRAPH_MINIMIZEINACTIVE, _T("Minimize Inactive\nm")));
    }

    {//  Graph Ribbon 2
        m_CategoryGraph2 = m_wndRibbonBar.AddContextCategory(_T("&Graph"), _T("Graph"), RIBBON_GRAPH2, AFX_CategoryColor_Orange, IDB_WRITESMALL, IDB_WRITELARGE);

        CMFCRibbonPanel* pPanelNavigation = m_CategoryGraph2->AddPanel(_T("Layout\nl"), m_PanelImages.ExtractIcon (7));
        pPanelNavigation->Add(new CMFCRibbonButton(ID_GRAPH2_DOT, _T("Tree\nt")));
        pPanelNavigation->Add(new CMFCRibbonButton(ID_GRAPH2_NEATO, _T("Spring\ns")));
        pPanelNavigation->Add(new CMFCRibbonButton(ID_GRAPH2_TWOPI, _T("Radial\nr")));
        pPanelNavigation->Add(new CMFCRibbonButton(ID_GRAPH2_FDP, _T("Spring2\n2")));
        pPanelNavigation->Add(new CMFCRibbonButton(ID_GRAPH2_CIRCO, _T("Cluster\nc")));
        pPanelNavigation->Add(new CMFCRibbonButton(ID_GRAPH2_SFDP, _T("Spring3\n3")));
        pPanelNavigation->Add(new CMFCRibbonButton(ID_GRAPH2_OSAGE, _T("Osage\no")));
    }

    {//  Debug Ribbon
        m_CategoryDebug = m_wndRibbonBar.AddContextCategory(_T("&Basic"), _T("Debug"), RIBBON_DEBUG, AFX_CategoryColor_Red, IDB_DEBUGGER, IDB_WRITELARGE);

        //  Create a "Process" Panel.
        CMFCRibbonPanel* pPanelProcess = m_CategoryDebug->AddPanel(_T("Process"), m_PanelImages.ExtractIcon (7));
        pPanelProcess->Add(new CMFCRibbonButton(ID_DEBUGGER_DETACH, _T("Detach\nd"), 14));
        pPanelProcess->Add(new CMFCRibbonButton(ID_DEBUGGER_ABORT, _T("Abort\na"), 15));
        pPanelProcess->Add(new CMFCRibbonButton(ID_DEBUGGER_RESTART, _T("Restart\nr"), 19));
        pPanelProcess->Add(new CMFCRibbonSeparator());
        pPanelProcess->Add(new CMFCRibbonButton(ID_DEBUGGER_BREAK, _T("Break\nb"), 16));
        pPanelProcess->Add(new CMFCRibbonButton(ID_DEBUGGER_CONTINUE, _T("Continue\nc"), 13));

        //  Create a "Graph" Panel.
        CMFCRibbonPanel* pPanelGraph = m_CategoryDebug->AddPanel(_T("Graph"), m_PanelImages.ExtractIcon (7));
        pPanelGraph->Add(new CMFCRibbonButton(ID_DEBUGGER_STEPGRAPH, _T("Step\ng"), 18));
        pPanelGraph->Add(new CMFCRibbonButton(ID_DEBUGGER_STEPGRAPHSTART, _T("Step Start\ns"), 18));
        pPanelGraph->Add(new CMFCRibbonButton(ID_DEBUGGER_STEPGRAPHEND, _T("Step End\ne"), 18));

        //  Create a "Edge" Panel.
        CMFCRibbonPanel* pPanelEdge = m_CategoryDebug->AddPanel(_T("Edge"), m_PanelImages.ExtractIcon (7));
        pPanelEdge->Add(new CMFCRibbonButton(ID_DEBUGGER_STEP, _T("Step\ns"), 17));
        pPanelEdge->Add(new CMFCRibbonButton(ID_DEBUGGER_NEXT, _T("Next\nn"), 19));

        //  Create a "Breakpoints" Panel.
        CMFCRibbonPanel* pPanelBreakpoints = m_CategoryDebug->AddPanel(_T("Breakpoint(s)"), m_PanelImages.ExtractIcon (7));
        pPanelBreakpoints->Add(new CMFCRibbonButton(ID_DEBUGGER_SETGLOBALBREAKPOINT, _T("Add Global\na"), 20));
        pPanelBreakpoints->Add(new CMFCRibbonButton(ID_DEBUGGER_SETBREAKPOINT, _T("Add\na"), 20));
        pPanelBreakpoints->Add(new CMFCRibbonSeparator());
        pPanelBreakpoints->Add(new CMFCRibbonButton(ID_DEBUGGER_REMOVEBREAKPOINT, _T("Clear\nc"), 21));
        pPanelBreakpoints->Add(new CMFCRibbonButton(ID_DEBUGGER_REMOVEALLBREAKPOINTS, _T("Clear All\na"), 22));

        //  Create a "Find" Panel.
        CMFCRibbonPanel* pPanelFind = m_CategoryDebug->AddPanel(_T("Find"));
        pPanelFind->Add(new CMFCRibbonButton(ID_EDIT_FIND, _T("Find\nf"), 4));
        pPanelFind->Add(new CMFCRibbonButton(ID_EDIT_FINDNEXT, _T("Next\nn"), 11));
        pPanelFind->Add(new CMFCRibbonButton(ID_EDIT_FINDPREV, _T("Previous\np"), 10));
        pPanelFind->Add(new CMFCRibbonSeparator());
        pPanelFind->Add(new CMFCRibbonButton(ID_EDIT_GOTO, _T("Goto\ng"), 24));

        //  Create a "Graph" Panel.
        CMFCRibbonPanel* pPanelView = m_CategoryDebug->AddPanel(_T("View"), m_PanelImages.ExtractIcon (7));
        //pPanelView->Add(new CMFCRibbonCheckBox(ID_DEBUGGER_SHOWALL, _T("Show All")));
        //pPanelView->Add(new CMFCRibbonCheckBox(ID_DEBUGGER_SHOWACTIVE, _T("Show Active")));
        //pPanelView->Add(new CMFCRibbonCheckBox(ID_DEBUGGER_SHOWOPTIMIZED, _T("Show Optimized")));
        //pPanelView->Add(new CMFCRibbonSeparator());
        pPanelView->Add(new CMFCRibbonLabel(_T("Zoom:")));
        CMFCRibbonSlider* pBtnZoom = new CMFCRibbonSlider(ID_DEBUGGER_ZOOM, 70 /* Slider width */);
        pBtnZoom->SetZoomButtons();
        pBtnZoom->SetRange(10, 100);
        pBtnZoom->SetPos(100);
        pPanelView->Add(pBtnZoom);
        pPanelView->Add(new CMFCRibbonButton(ID_DEBUGGER_RECENTER, _T("Recenter Active\nr")));
        pPanelView->Add(new CMFCRibbonSeparator());
        pPanelView->Add(new CMFCRibbonButton(ID_DEBUGGER_LAYOUT, _T("Layout\nl"), 23));
        CMFCRibbonProgressBar* SocketProgress = new CMFCRibbonProgressBar(ID_DEBUGGER_SOCKETPROGRESS, 100 /* Bar width */);
        SocketProgress->SetInfiniteMode();
        pPanelView->Add(SocketProgress);
        CMFCRibbonProgressBar* LayoutProgress = new CMFCRibbonProgressBar(ID_DEBUGGER_LAYOUTPROGRESS, 100 /* Bar width */);
        LayoutProgress->SetInfiniteMode();
        pPanelView->Add(LayoutProgress);
    }

    {
        m_CategoryDebugAdvanced = m_wndRibbonBar.AddContextCategory(_T("&Advanced"), _T("Debug"), RIBBON_DEBUG, AFX_CategoryColor_Red, IDB_DEBUGGER, IDB_WRITELARGE);
        //  Create a "Filter" Panel.
        CMFCRibbonPanel* pPanelGlobalFilter = m_CategoryDebugAdvanced->AddPanel(_T("Global Filter"), m_PanelImages.ExtractIcon (7));
        pPanelGlobalFilter->Add(new CMFCRibbonCheckBox(ID_VIEW_RUNNINGONLYGLOBAL, _T("Running Only\nr")));
        pPanelGlobalFilter->Add(new CMFCRibbonCheckBox(ID_VIEW_FOUNDONLYGLOBAL, _T("Search Results Only\nf")));
        CMFCRibbonPanel* pPanelActiveFilter = m_CategoryDebugAdvanced->AddPanel(_T("Active Filter"), m_PanelImages.ExtractIcon (7));
        pPanelActiveFilter->Add(new CMFCRibbonCheckBox(ID_VIEW_RUNNINGONLYACTIVE, _T("Running Only\nr")));
        pPanelActiveFilter->Add(new CMFCRibbonCheckBox(ID_VIEW_FOUNDONLYACTIVE, _T("Search Results Only\nf")));
    }

    {//  Diff Ribbon
        m_CategoryCompare = m_wndRibbonBar.AddContextCategory(_T("&Compare"), _T("Compare"), RIBBON_COMPARE, AFX_CategoryColor_Yellow, IDB_WRITESMALL, IDB_WRITELARGE);

        CMFCRibbonPanel* pPanelDiferences = m_CategoryCompare->AddPanel(_T("Differences"));
        pPanelDiferences->Add(new CMFCRibbonButton(ID_DIFFERENCES_PREVIOUS, _T("Previous\nn")));
        pPanelDiferences->Add(new CMFCRibbonButton(ID_DIFFERENCES_NEXT, _T("Next\np")));
    }

    {// Add elements to the right side of tabs:
        strTemp.LoadString(IDS_RIBBON_STYLE);
        CMFCRibbonButton* pVisualStyleButton = new CMFCRibbonButton((UINT)-1, strTemp);

        pVisualStyleButton->SetMenu(IDR_THEME_MENU, FALSE /* No default command */, TRUE /* Right align */);

        strTemp.LoadString(IDS_RIBBON_STYLE_TIP);
        pVisualStyleButton->SetToolTipText(strTemp);
        strTemp.LoadString(IDS_RIBBON_STYLE_DESC);
        pVisualStyleButton->SetDescription(strTemp);
        m_wndRibbonBar.AddToTabs(pVisualStyleButton);
    }

    {
        CMFCRibbonButton * buttHelp = new CMFCRibbonButton(ID_APP_ABOUT, _T("About"), m_PanelImages.ExtractIcon(0));
        buttHelp->AddSubItem(new CMFCRibbonButton(ID_HELP_LANGUAGEREFERENCE, _T("&Download Resources")));
        buttHelp->AddSubItem(new CMFCRibbonSeparator());
        buttHelp->AddSubItem(new CMFCRibbonButton(ID_APP_ABOUT, _T("&About")));
        m_wndRibbonBar.AddToTabs(buttHelp);
    }
    // Add quick access toolbar commands:
    CList<UINT, UINT> lstQATCmds;
    lstQATCmds.AddTail(ID_FILE_NEWBUILDER);
    lstQATCmds.AddTail(ID_FILE_OPENBUILDER);
    lstQATCmds.AddTail(ID_FILE_SAVE);
    lstQATCmds.AddTail(ID_EDIT_UNDO);
    lstQATCmds.AddTail(ID_EDIT_REDO);
    lstQATCmds.AddTail(ID_FILE_PRINT);
    m_wndRibbonBar.SetQuickAccessCommands(lstQATCmds);
}

void CMainFrame::InitializeDocking(UINT nID)
{
    WinPaneVector mainPanes;
    NeighbourPaneVector neighbourPanes;
    TabbedPaneVector tabbedPanes;
    HiddenPaneVector hiddenPanes;

    switch(nID)
    {
    case ID_VIEW_RESETDOCKEDWINDOWS_DEFAULT_LEFT:
        InitializeDockingDefaultLeft(mainPanes, neighbourPanes, tabbedPanes, hiddenPanes);
        break;
    case ID_VIEW_RESETDOCKEDWINDOWS_DEFAULT_RIGHT:
        InitializeDockingDefaultRight(mainPanes, neighbourPanes, tabbedPanes, hiddenPanes);
        break;
    case ID_VIEW_RESETDOCKEDWINDOWS_ALLPINNED_LEFT:
        InitializeDockingPinnedLeft(mainPanes, neighbourPanes, tabbedPanes, hiddenPanes);
        break;
    case ID_VIEW_RESETDOCKEDWINDOWS_ALLPINNED_RIGHT:
        InitializeDockingPinnedRight(mainPanes, neighbourPanes, tabbedPanes, hiddenPanes);
        break;
    case ID_VIEW_RESETDOCKEDWINDOWS_ALLUNPINNED_LEFT:
        InitializeDockingUnpinnedLeft(mainPanes, neighbourPanes, tabbedPanes, hiddenPanes);
        break;
    case ID_VIEW_RESETDOCKEDWINDOWS_ALLUNPINNED_RIGHT:
        InitializeDockingUnpinnedRight(mainPanes, neighbourPanes, tabbedPanes, hiddenPanes);
        break;
    default:
        break;
    }

    if (!theApp.IsStateExists(getWorkspaceString(WORKSPACE_NORMAL)))
    {
        ATLASSERT(m_workspaceMode == WORKSPACE_NORMAL);
        hiddenPanes.push_back(HiddenPane(m_debugDataViews));
        hiddenPanes.push_back(HiddenPane(m_debugPropertyGridViews));
        hiddenPanes.push_back(HiddenPane(m_debugBreakpointView));
        hiddenPanes.push_back(HiddenPane(m_debugPropertiesView));
        hiddenPanes.push_back(HiddenPane(m_debugSearchView));
    }
    DockToMainframe(mainPanes);
    DockToPane(neighbourPanes);
    TabDockToPane(tabbedPanes);
    HidePane(hiddenPanes);
}

void CMainFrame::InitializeDockingDefaultRight(WinPaneVector & mainPanes, NeighbourPaneVector & neighbourPanes, TabbedPaneVector & tabbedPanes, HiddenPaneVector & hiddenPanes)
{
    mainPanes.push_back(WinPane(m_RepositoryFilter, CBRS_ALIGN_LEFT, false));
    mainPanes.push_back(WinPane(m_WorkunitsFilter, CBRS_ALIGN_LEFT, false));
    mainPanes.push_back(WinPane(m_DfuFilter, CBRS_ALIGN_LEFT, false));

    mainPanes.push_back(WinPane(m_Repository, CBRS_ALIGN_RIGHT));
    {
        neighbourPanes.push_back(NeighbourPane(m_Repository, m_Workunits, CBRS_ALIGN_TOP));
        {
            tabbedPanes.push_back(TabbedPane(m_Workunits, m_WorkunitsActive));
            tabbedPanes.push_back(TabbedPane(m_Workunits, m_debugPropertiesView));
        }
#ifdef WORKSPACE_WINDOW
        tabbedPanes.push_back(TabbedPane(m_Repository, m_Workspace));
#endif
        tabbedPanes.push_back(TabbedPane(m_Repository, m_Dfu));
        tabbedPanes.push_back(TabbedPane(m_Repository, m_debugSearchView));
    }

    mainPanes.push_back(WinPane(m_Syntax, CBRS_ALIGN_BOTTOM));
    {
        neighbourPanes.push_back(NeighbourPane(m_Syntax, m_Error, CBRS_ALIGN_RIGHT));
        {
            tabbedPanes.push_back(TabbedPane(m_Error, m_debugDataViews));
        }
        tabbedPanes.push_back(TabbedPane(m_Syntax, m_Bookmarks));
        tabbedPanes.push_back(TabbedPane(m_Syntax, m_debugBreakpointView));
        tabbedPanes.push_back(TabbedPane(m_Syntax, m_debugPropertyGridViews));
    }
}

void CMainFrame::InitializeDockingPinnedRight(WinPaneVector & mainPanes, NeighbourPaneVector & neighbourPanes, TabbedPaneVector & tabbedPanes, HiddenPaneVector & hiddenPanes)
{
    mainPanes.push_back(WinPane(m_RepositoryFilter, CBRS_ALIGN_LEFT));
    {
        tabbedPanes.push_back(TabbedPane(m_RepositoryFilter, m_WorkunitsFilter));
        tabbedPanes.push_back(TabbedPane(m_RepositoryFilter, m_DfuFilter));
    }

    mainPanes.push_back(WinPane(m_Repository, CBRS_ALIGN_RIGHT));
    {
        neighbourPanes.push_back(NeighbourPane(m_Repository, m_Workunits, CBRS_ALIGN_TOP));
        {
            tabbedPanes.push_back(TabbedPane(m_Workunits, m_WorkunitsActive));
            tabbedPanes.push_back(TabbedPane(m_Workunits, m_debugPropertiesView));
        }
#ifdef WORKSPACE_WINDOW
        tabbedPanes.push_back(TabbedPane(m_Repository, m_Workspace));
#endif
        tabbedPanes.push_back(TabbedPane(m_Repository, m_Dfu));
        tabbedPanes.push_back(TabbedPane(m_Repository, m_debugSearchView));
    }

    mainPanes.push_back(WinPane(m_Syntax, CBRS_ALIGN_BOTTOM));
    {
        neighbourPanes.push_back(NeighbourPane(m_Syntax, m_Error, CBRS_ALIGN_RIGHT));
        {
            tabbedPanes.push_back(TabbedPane(m_Error, m_debugDataViews));
        }
        tabbedPanes.push_back(TabbedPane(m_Syntax, m_Bookmarks));
        tabbedPanes.push_back(TabbedPane(m_Syntax, m_debugBreakpointView));
        tabbedPanes.push_back(TabbedPane(m_Syntax, m_debugPropertyGridViews));
    }
}

void CMainFrame::InitializeDockingUnpinnedRight(WinPaneVector & mainPanes, NeighbourPaneVector & neighbourPanes, TabbedPaneVector & tabbedPanes, HiddenPaneVector & hiddenPanes)
{
    mainPanes.push_back(WinPane(m_RepositoryFilter, CBRS_ALIGN_LEFT, false));
    mainPanes.push_back(WinPane(m_WorkunitsFilter, CBRS_ALIGN_LEFT, false));
    mainPanes.push_back(WinPane(m_DfuFilter, CBRS_ALIGN_LEFT, false));

    mainPanes.push_back(WinPane(m_Workunits, CBRS_ALIGN_RIGHT, false));
    mainPanes.push_back(WinPane(m_WorkunitsActive, CBRS_ALIGN_RIGHT, false));
    mainPanes.push_back(WinPane(m_debugPropertiesView, CBRS_ALIGN_RIGHT, false));

    mainPanes.push_back(WinPane(m_Repository, CBRS_ALIGN_RIGHT, false));
    mainPanes.push_back(WinPane(m_Workspace, CBRS_ALIGN_RIGHT, false));
    mainPanes.push_back(WinPane(m_Dfu, CBRS_ALIGN_RIGHT, false));
    mainPanes.push_back(WinPane(m_debugSearchView, CBRS_ALIGN_RIGHT, false));

    mainPanes.push_back(WinPane(m_Syntax, CBRS_ALIGN_BOTTOM, false));
    mainPanes.push_back(WinPane(m_Bookmarks, CBRS_ALIGN_BOTTOM, false));
    mainPanes.push_back(WinPane(m_debugBreakpointView, CBRS_ALIGN_BOTTOM, false));
    mainPanes.push_back(WinPane(m_debugPropertyGridViews, CBRS_ALIGN_BOTTOM, false));

    mainPanes.push_back(WinPane(m_Error, CBRS_ALIGN_BOTTOM, false));
    mainPanes.push_back(WinPane(m_debugDataViews, CBRS_ALIGN_BOTTOM, false));
}

void CMainFrame::InitializeDockingDefaultLeft(WinPaneVector & mainPanes, NeighbourPaneVector & neighbourPanes, TabbedPaneVector & tabbedPanes, HiddenPaneVector & hiddenPanes)
{
    mainPanes.push_back(WinPane(m_RepositoryFilter, CBRS_ALIGN_RIGHT, false));
    mainPanes.push_back(WinPane(m_WorkunitsFilter, CBRS_ALIGN_RIGHT, false));
    mainPanes.push_back(WinPane(m_DfuFilter, CBRS_ALIGN_RIGHT, false));

    mainPanes.push_back(WinPane(m_Repository, CBRS_ALIGN_LEFT));
    {
        neighbourPanes.push_back(NeighbourPane(m_Repository, m_Workunits, CBRS_ALIGN_TOP));
        {
            tabbedPanes.push_back(TabbedPane(m_Workunits, m_WorkunitsActive));
            tabbedPanes.push_back(TabbedPane(m_Workunits, m_debugPropertiesView));
        }
#ifdef WORKSPACE_WINDOW
        tabbedPanes.push_back(TabbedPane(m_Repository, m_Workspace));
#endif
        tabbedPanes.push_back(TabbedPane(m_Repository, m_Dfu));
        tabbedPanes.push_back(TabbedPane(m_Repository, m_debugSearchView));
    }

    mainPanes.push_back(WinPane(m_Syntax, CBRS_ALIGN_BOTTOM));
    {
        neighbourPanes.push_back(NeighbourPane(m_Syntax, m_Error, CBRS_ALIGN_RIGHT));
        {
            tabbedPanes.push_back(TabbedPane(m_Error, m_debugDataViews));
        }
        tabbedPanes.push_back(TabbedPane(m_Syntax, m_Bookmarks));
        tabbedPanes.push_back(TabbedPane(m_Syntax, m_debugBreakpointView));
        tabbedPanes.push_back(TabbedPane(m_Syntax, m_debugPropertyGridViews));
    }
}

void CMainFrame::InitializeDockingPinnedLeft(WinPaneVector & mainPanes, NeighbourPaneVector & neighbourPanes, TabbedPaneVector & tabbedPanes, HiddenPaneVector & hiddenPanes)
{
    mainPanes.push_back(WinPane(m_RepositoryFilter, CBRS_ALIGN_RIGHT));
    {
        tabbedPanes.push_back(TabbedPane(m_RepositoryFilter, m_WorkunitsFilter));
        tabbedPanes.push_back(TabbedPane(m_RepositoryFilter, m_DfuFilter));
    }

    mainPanes.push_back(WinPane(m_Repository, CBRS_ALIGN_LEFT));
    {
        neighbourPanes.push_back(NeighbourPane(m_Repository, m_Workunits, CBRS_ALIGN_TOP));
        {
            tabbedPanes.push_back(TabbedPane(m_Workunits, m_WorkunitsActive));
            tabbedPanes.push_back(TabbedPane(m_Workunits, m_debugPropertiesView));
        }
#ifdef WORKSPACE_WINDOW
        tabbedPanes.push_back(TabbedPane(m_Repository, m_Workspace));
#endif
        tabbedPanes.push_back(TabbedPane(m_Repository, m_Dfu));
        tabbedPanes.push_back(TabbedPane(m_Repository, m_debugSearchView));
    }

    mainPanes.push_back(WinPane(m_Syntax, CBRS_ALIGN_BOTTOM));
    {
        neighbourPanes.push_back(NeighbourPane(m_Syntax, m_Error, CBRS_ALIGN_RIGHT));
        {
            tabbedPanes.push_back(TabbedPane(m_Error, m_debugDataViews));
        }
        tabbedPanes.push_back(TabbedPane(m_Syntax, m_Bookmarks));
        tabbedPanes.push_back(TabbedPane(m_Syntax, m_debugBreakpointView));
        tabbedPanes.push_back(TabbedPane(m_Syntax, m_debugPropertyGridViews));
    }
}

void CMainFrame::InitializeDockingUnpinnedLeft(WinPaneVector & mainPanes, NeighbourPaneVector & neighbourPanes, TabbedPaneVector & tabbedPanes, HiddenPaneVector & hiddenPanes)
{
    mainPanes.push_back(WinPane(m_RepositoryFilter, CBRS_ALIGN_RIGHT, false));
    mainPanes.push_back(WinPane(m_WorkunitsFilter, CBRS_ALIGN_RIGHT, false));
    mainPanes.push_back(WinPane(m_DfuFilter, CBRS_ALIGN_RIGHT, false));

    mainPanes.push_back(WinPane(m_Workunits, CBRS_ALIGN_LEFT, false));
    mainPanes.push_back(WinPane(m_WorkunitsActive, CBRS_ALIGN_LEFT, false));
    mainPanes.push_back(WinPane(m_debugPropertiesView, CBRS_ALIGN_LEFT, false));

    mainPanes.push_back(WinPane(m_Repository, CBRS_ALIGN_LEFT, false));
    mainPanes.push_back(WinPane(m_Workspace, CBRS_ALIGN_LEFT, false));
    mainPanes.push_back(WinPane(m_Dfu, CBRS_ALIGN_LEFT, false));
    mainPanes.push_back(WinPane(m_debugSearchView, CBRS_ALIGN_LEFT, false));

    mainPanes.push_back(WinPane(m_Syntax, CBRS_ALIGN_BOTTOM, false));
    mainPanes.push_back(WinPane(m_Bookmarks, CBRS_ALIGN_BOTTOM, false));
    mainPanes.push_back(WinPane(m_debugBreakpointView, CBRS_ALIGN_BOTTOM, false));
    mainPanes.push_back(WinPane(m_debugPropertyGridViews, CBRS_ALIGN_BOTTOM, false));

    mainPanes.push_back(WinPane(m_Error, CBRS_ALIGN_BOTTOM, false));
    mainPanes.push_back(WinPane(m_debugDataViews, CBRS_ALIGN_BOTTOM, false));
}

BOOL CMainFrame::CreateDockingWindows()
{
#ifdef WORKSPACE_WINDOW
    if (!m_Workspace->Create(_T("Workspace"), this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_PROJECT, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
    {
        TRACE0("Failed to create Project window\n");
        return FALSE; // failed to create
    }
    m_Workspace->EnableDocking(CBRS_ALIGN_ANY);
#endif
    if (!m_Repository->Create(_T("Repository"), this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_REPOSITORY, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
    {
        TRACE0("Failed to create Repository window\n");
        return FALSE; // failed to create
    }
    m_Repository->EnableDocking(CBRS_ALIGN_ANY);
    if (!m_RepositoryFilter->Create(_T("Find (Repository)"), this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_REPOSITORYFILTER, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_HIDE_INPLACE))
    {
        TRACE0("Failed to create Repository Search window\n");
        return FALSE; // failed to create
    }
    m_RepositoryFilter->EnableDocking(CBRS_ALIGN_ANY);
    if (!m_Workunits->Create(_T("Workunits"), this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_WORKUNITSTREE, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_HIDE_INPLACE))
    {
        TRACE0("Failed to create Workunit History window\n");
        return FALSE; // failed to create
    }
    m_Workunits->EnableDocking(CBRS_ALIGN_ANY);
    if (!m_debugPropertiesView->Create(_T("Properties"), this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_DEBUGPROPERTIES, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_HIDE_INPLACE))
    {
        TRACE0("Failed to create Properties window\n");
        return FALSE; // failed to create
    }
    m_debugPropertiesView->EnableDocking(CBRS_ALIGN_ANY);
    if (!m_debugSearchView->Create(_T("Search"), this, TRUE, MAKEINTRESOURCE(IDD_DIALOG_SEARCH), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_HIDE_INPLACE, ID_VIEW_DEBUGSEARCH))
    {
        TRACE0("Failed to create Search window\n");
        return FALSE; // failed to create
    }
    m_debugSearchView->EnableDocking(CBRS_ALIGN_ANY);
    if (!m_WorkunitsActive->Create(_T("Active Workunits"), this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_ACTIVEWORKUNITSTREE, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_HIDE_INPLACE))
    {
        TRACE0("Failed to create Active Workunits window\n");
        return FALSE; // failed to create
    }
    m_WorkunitsActive->EnableDocking(CBRS_ALIGN_ANY);
    if (!m_WorkunitsFilter->Create(_T("Find (Workunit)"), this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_FILTEREDWORKUNITTREEFRAME, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_HIDE_INPLACE))
    {
        TRACE0("Failed to create Workunit Search window\n");
        return FALSE; // failed to create
    }
    m_WorkunitsFilter->EnableDocking(CBRS_ALIGN_ANY);
    if (!m_Dfu->Create(_T("Datasets"), this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_DFU, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
    {
        TRACE0("Failed to create Dataset window\n");
        return FALSE; // failed to create
    }
    m_Dfu->EnableDocking(CBRS_ALIGN_ANY);
    if (!m_DfuFilter->Create(_T("Find (Datasets)"), this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_DFUFILTER, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_HIDE_INPLACE))
    {
        TRACE0("Failed to create Datasets Search window\n");
        return FALSE; // failed to create
    }
    m_DfuFilter->EnableDocking(CBRS_ALIGN_ANY);
    if (!m_Syntax->Create(_T("Syntax Errors"), this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_SYNTAX, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_HIDE_INPLACE))
    {
        TRACE0("Failed to create Syntax window\n");
        return FALSE; // failed to create
    }
    m_Syntax->EnableDocking(CBRS_ALIGN_ANY);
    if (!m_debugDataViews->Create(_T("Debug Information"), this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_DEBUGDATA, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_HIDE_INPLACE))
    {
        TRACE0("Failed to create Debug Data window\n");
        return FALSE; // failed to create
    }
    m_debugDataViews->EnableDocking(CBRS_ALIGN_ANY);
    if (!m_debugBreakpointView->Create(_T("Breakpoints"), this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_DEBUGBREAKPOINTS, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_HIDE_INPLACE))
    {
        TRACE0("Failed to create Breakpoints window\n");
        return FALSE; // failed to create
    }
    m_debugBreakpointView->EnableDocking(CBRS_ALIGN_ANY);
    if (!m_Bookmarks->Create(_T("Bookmarks"), this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_BOOKMARKS, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_HIDE_INPLACE))
    {
        TRACE0("Failed to create Bookmarks window\n");
        return FALSE; // failed to create
    }
    m_Bookmarks->EnableDocking(CBRS_ALIGN_ANY);
    if (!m_Error->Create(_T("Error Log"), this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_ERROR, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_HIDE_INPLACE))
    {
        TRACE0("Failed to create Error window\n");
        return FALSE; // failed to create
    }
    m_Error->EnableDocking(CBRS_ALIGN_ANY);
    if (!m_debugPropertyGridViews->Create(_T("Properties"), this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_DEBUGPROPERTYGRID, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_HIDE_INPLACE))
    {
        TRACE0("Failed to create Breakpoints window\n");
        return FALSE; // failed to create
    }
    m_debugPropertyGridViews->EnableDocking(CBRS_ALIGN_ANY);
    SetDockingWindowIcons(theApp.m_bHiColorIcons);

    if (!m_wndCaptionBar.Create(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, this, ID_VIEW_CAPTIONBAR, -1, TRUE))
    {
        TRACE0("Failed to create caption bar\n");
        return FALSE;
    }

    //m_wndCaptionBar.SetButton(_T("Options..."), ID_TOOLS_OPTIONS, CMFCCaptionBar::ALIGN_LEFT, FALSE);
    m_wndCaptionBar.SetButtonToolTip(_T("Exceptions."));

    m_wndCaptionBar.SetText(_T(""), CMFCCaptionBar::ALIGN_LEFT);

    m_wndCaptionBar.SetBitmap(IDB_INFO, RGB(255, 255, 255), FALSE, CMFCCaptionBar::ALIGN_LEFT);
    m_wndCaptionBar.SetImageToolTip(_T("Exception"), _T("Server errors here!"));

    m_wndCaptionBar.ShowWindow(SW_HIDE);

    return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
    //HICON hFileViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_FILE_VIEW_HC : IDI_FILE_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
    //m_wndFileView.SetIcon(hFileViewIcon, FALSE);

    //HICON hClassViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_CLASS_VIEW_HC : IDI_CLASS_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
    //m_wndClassView.SetIcon(hClassViewIcon, FALSE);

    //HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
    //m_wndOutput.SetIcon(hOutputBarIcon, FALSE);

    //HICON hPropertiesBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PROPERTIES_WND_HC : IDI_PROPERTIES_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
    //m_Repository->SetIcon(hPropertiesBarIcon, FALSE);

    HICON hPropertiesBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PROPERTIES_WND_HC : IDI_PROPERTIES_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
#ifdef WORKSPACE_WINDOW
    m_Workspace->SetIcon(hPropertiesBarIcon, FALSE);
#endif
    m_Repository->SetIcon(hPropertiesBarIcon, FALSE);
    m_RepositoryFilter->SetIcon(hPropertiesBarIcon, FALSE);
    m_Workunits->SetIcon(hPropertiesBarIcon, FALSE);
    m_WorkunitsActive->SetIcon(hPropertiesBarIcon, FALSE);
    m_WorkunitsFilter->SetIcon(hPropertiesBarIcon, FALSE);
    m_Dfu->SetIcon(hPropertiesBarIcon, FALSE);
    m_DfuFilter->SetIcon(hPropertiesBarIcon, FALSE);
    m_Syntax->SetIcon(hPropertiesBarIcon, FALSE);
    m_Bookmarks->SetIcon(hPropertiesBarIcon, FALSE);
    m_debugDataViews->SetIcon(hPropertiesBarIcon, FALSE);
    m_debugBreakpointView->SetIcon(hPropertiesBarIcon, FALSE);
    m_debugPropertiesView->SetIcon(hPropertiesBarIcon, FALSE);
    m_debugSearchView->SetIcon(hPropertiesBarIcon, FALSE);
    m_debugPropertyGridViews->SetIcon(hPropertiesBarIcon, FALSE);
    m_Error->SetIcon(hPropertiesBarIcon, FALSE);

    UpdateMDITabbedBarsIcons();
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
    CMDIFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
    CMDIFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnWindowManager()
{
    ShowWindowsDialog();
}

void CMainFrame::OnWindowCascade()
{
    m_tabbedMDI = false;
    SetMDIMode();
    MDICascade(); 
}

void CMainFrame::OnUpdateWindowCascade(CCmdUI* pCmdUI)
{
    HWND hWnd = GetActive();
    if (::IsWindow(hWnd))
    {
        pCmdUI->Enable();
        if (!m_tabbedMDI && ::IsZoomed(hWnd))
        {
            m_tabbedMDI = true;
            SetMDIMode();
        }
    }
    else
        pCmdUI->Enable(false);
}

void CMainFrame::OnWindowTile()
{
    m_tabbedMDI = false;
    SetMDIMode();
    MDITile(MDITILE_HORIZONTAL); 
}

void CMainFrame::OnWindowArrange()
{
    m_tabbedMDI = false;
    SetMDIMode();
    MDIIconArrange(); 
}

void CMainFrame::OnWorkspaceNew()
{
    DoWorkspaceNew();
}

void CMainFrame::OnWorkspaceRemove()
{
    IWorkspaceVector workspaceToRemove;
    workspaceToRemove.push_back((IWorkspace *)m_workspaceCombo->GetItemData(m_workspaceCombo->GetCurSel()));
    if (MessageBox(_T("Are you sure?"), _T("Delete Workspace"), MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) == IDYES)
        DoWorkspaceRemove(workspaceToRemove);
}

void CMainFrame::OnWorkspaceSelEndOk()
{
    DoWorkspaceSwitch((IWorkspace *)m_workspaceCombo->GetItemData(m_workspaceCombo->GetCurSel()), true);
}

void CMainFrame::OnHelpLanguageReference()
{
    ::ShellExecute(0, _T("open"), _T("http://hpccsystems.com/download"), 0, 0, SW_SHOWNORMAL);
}

LRESULT CMainFrame::OnGetTabToolTip(WPARAM /*wp*/, LPARAM lp)
{
    CMFCTabToolTipInfo* pInfo = (CMFCTabToolTipInfo*) lp;
    ASSERT (pInfo != NULL);

    if (pInfo)
    {
        ASSERT_VALID(pInfo->m_pTabWnd);
        if (!pInfo->m_pTabWnd->IsMDITab())
            return 0;

        HWND hWnd = NULL;
        CWnd * wnd = pInfo->m_pTabWnd->GetTabWnd(pInfo->m_nTabIndex);
        if (wnd)
            hWnd = wnd->GetSafeHwnd();
        if (hWnd == NULL || !::IsWindow(hWnd))
            hWnd = GetActive();

        if (::IsWindow(hWnd))
        {
            std::_tstring tooltip;
            ::SendMessage(hWnd, CWM_GETTOOLTIPTEXT, (WPARAM)&tooltip, NULL);
            if (!tooltip.empty())
                pInfo->m_strText = tooltip.c_str();
        }
    }

    return 0;
}

void CMainFrame::OnApplicationLook(UINT id)
{
    CWaitCursor wait;

    theApp.m_nAppLook = id;

    switch (theApp.m_nAppLook)
    {
    case ID_VIEW_APPLOOK_WIN_2000:
        CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
#if _MFC_VER >= 0x0A00
        m_wndRibbonBar.SetWindows7Look(FALSE);
#endif
        break;

    case ID_VIEW_APPLOOK_OFF_XP:
        CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
#if _MFC_VER >= 0x0A00
        m_wndRibbonBar.SetWindows7Look(FALSE);
#endif
        break;

    case ID_VIEW_APPLOOK_WIN_XP:
        CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
        CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
#if _MFC_VER >= 0x0A00
        m_wndRibbonBar.SetWindows7Look(FALSE);
#endif
        break;

    case ID_VIEW_APPLOOK_OFF_2003:
        CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
        CDockingManager::SetDockingMode(DT_SMART);
#if _MFC_VER >= 0x0A00
        m_wndRibbonBar.SetWindows7Look(FALSE);
#endif
        break;

    case ID_VIEW_APPLOOK_VS_2005:
        CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
        CDockingManager::SetDockingMode(DT_SMART);
#if _MFC_VER >= 0x0A00
        m_wndRibbonBar.SetWindows7Look(FALSE);
#endif
        break;

#if _MFC_VER >= 0x0A00
    case ID_VIEW_APPLOOK_VS_2008:
        CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
        CDockingManager::SetDockingMode(DT_SMART);
        m_wndRibbonBar.SetWindows7Look(FALSE);
        break;
#endif

#if _MFC_VER >= 0x0A00
    case ID_VIEW_APPLOOK_WIN7_SCENIC:
        CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
        CDockingManager::SetDockingMode(DT_SMART);
        m_wndRibbonBar.SetWindows7Look(TRUE);
        break;
#endif

    default:
        switch (theApp.m_nAppLook)
        {
        case ID_VIEW_APPLOOK_OFF_2007_BLUE:
            CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
            break;

        case ID_VIEW_APPLOOK_OFF_2007_BLACK:
            CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
            break;

        case ID_VIEW_APPLOOK_OFF_2007_SILVER:
            CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
            break;

        case ID_VIEW_APPLOOK_OFF_2007_AQUA:
            CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
            break;
        }

        CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
        CDockingManager::SetDockingMode(DT_SMART);
#if _MFC_VER >= 0x0A00
        m_wndRibbonBar.SetWindows7Look(FALSE);
#endif
    }

    RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

    theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
    pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

void CMainFrame::OnTabLook(UINT id)
{
    m_tabbedStyle = (CMFCTabCtrl::Style)(id - ID_VIEW_TABLOOK_STYLE_3D);
    SetMDIMode();
}

void CMainFrame::OnUpdateTabLook(CCmdUI* pCmdUI)
{
    pCmdUI->SetRadio(ID_VIEW_TABLOOK_STYLE_3D + m_tabbedStyle == pCmdUI->m_nID);
}

void CMainFrame::OnCloseOnTab()
{
    m_closeOnTab = !m_closeOnTab;
    SetMDIMode();
}

void CMainFrame::OnUpdateCloseOnTab(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_closeOnTab);
}

CMainFrame * CreateIMainFrame()
{
    ATLASSERT(!g_MainFrame);
    g_MainFrame = new CMainFrame();
    return g_MainFrame;
}

void DeleteIMainFrame()
{
    ATLASSERT(g_MainFrame);
    delete g_MainFrame;
    g_MainFrame = NULL;
}

CMainFrame * GetIMainFrame()
{
    ATLASSERT(g_MainFrame);
    return g_MainFrame;
}

void CMainFrame::DoLogout()
{
    EnableLocalRepository(TRI_BOOL_UNKNOWN);
    EnableRemoteDaliEnabled(TRI_BOOL_UNKNOWN);
    EnableRemoteQueueEnabled(TRI_BOOL_UNKNOWN);

    m_supressSyncTOC = true;
    KillTimer(TIMER_AUTOSAVE);

    CString title;
    title.LoadString(IDR_MAINFRAME);
    SetTitle(title);
    UpdateFrameTitleForDocument(NULL);

    DoWorkspaceSave();
    EnumChildWindows(GetSafeHwnd(), ChildEnumProc, CWM_CLOSEALL);

#ifdef WORKSPACE_WINDOW
    m_Workspace->Send_Clear();
    m_workspaceCombo->RemoveAllItems();
#endif
    m_Repository->Send_Clear();
    m_RepositoryFilter->Send_Clear();
    m_Workunits->Send_Clear();
    m_WorkunitsActive->Send_Clear();
    m_WorkunitsFilter->Send_Clear();
#ifdef DFU_WINDOWS
    m_Dfu->Send_Clear();
    m_DfuFilter->Send_Clear();
#endif

    m_Syntax->Send_Clear();
    m_debugDataViews->Send_Clear();
    m_debugBreakpointView->Send_Clear();
    m_debugPropertiesView->Send_Clear();
    m_debugSearchView->Send_Clear();
    m_debugPropertyGridViews->Send_Clear();
    m_Error->Send_Clear();

    ReleaseAllSingletons();

    m_wndRibbonBar.SendMessage(WM_IDLEUPDATECMDUI, true);
}

void CMainFrame::DoLogin(bool SkipLoginWindow, const CString & previousPassword)
{
    if (!SkipLoginWindow)
    {
        if (DoModalLogin(this, m_iniFile, GetIConfig(QUERYBUILDER_CFG), TRUE) != IDOK)
        {
            DestroyWindow();
            return;
        }
        if (IsLocalRepositoryEnabled())
        {
            int i = m_openRibbonButton->FindSubItemIndexByID(ID_FILE_OPENATTRIBUTE);
            if (i >= 0)
            {
                m_openRibbonButton->RemoveSubItem(i);
            }
        }
    }

    boost::filesystem::path path;
    GetIConfig(QUERYBUILDER_CFG)->GetEnvironmentFolder(path);
    m_singleInstance = NULL;	//kills the previous mutex...
    m_singleInstance = new CLimitSingleInstance(pathToWString(path));
    if (m_singleInstance->IsAnotherInstanceRunning())
    {
        MessageBox(_T("Only one instance of ECL IDE is permitted with the same \"Configuration\" and \"Login ID\"."), CString(MAKEINTRESOURCE(IDR_MAINFRAME)), MB_OK | MB_ICONEXCLAMATION);
        DoLogin();
        return;
    }
    if (SkipLoginWindow)
    {
        GetIConfig(QUERYBUILDER_CFG)->Set(GLOBAL_PASSWORD, previousPassword);
    }

    m_Error->Send_Reset();

    if (CComPtr<IEclCC> eclcc = CreateIEclCC())
    {
        std::_tstring warnings;
        eclcc->GetPrefWarnings(warnings);
        if (!warnings.empty())
        {
            _DBGLOG(LEVEL_WARNING, warnings.c_str());
        }

        std::_tstring errors;
        eclcc->GetPrefErrors(errors);
        if (!errors.empty())
        {
            errors = _T("Compiler options specified are invalid:\r\n\r\n") + errors;
            MessageBox(errors.c_str(), CString(MAKEINTRESOURCE(IDR_MAINFRAME)), MB_OK | MB_ICONEXCLAMATION);
        }
    }

    {	//  Following call triggers the fetching of queues / clusters which is slow on first call (threaded)
        StlLinked<Topology::ITopology> server = AttachTopology(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_TOPOLOGY), _T("Topology"));
    }
    m_tabbedMDI = true;
    SetMDIMode();

    CString title;
    title.LoadString(IDR_MAINFRAME);
    std::_tstring environment = GetIConfig(QUERYBUILDER_CFG)->GetLabel();
    environment += _T(" - ");
    environment += title;
    SetTitle(environment.c_str());
    UpdateFrameTitleForDocument(NULL);

    std::_tstring version;
    GetAboutVersion(version);
    GetIConfig(QUERYBUILDER_CFG)->Set(GLOBAL_APP_VERSION, version);

    CString accountServer = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_ACCOUNT);
    if (accountServer.IsEmpty())
        EnableRemoteDaliEnabled(TRI_BOOL_FALSE);
    else
        EnableRemoteDaliEnabled(TRI_BOOL_TRUE);

#ifdef _DEBUGXXX
    UINT state = ::GetKeyState(VK_SHIFT);
    bool shiftKeyDown = ((state & 0x8000) != 0);
    if (!shiftKeyDown)
#endif
    {
#ifdef WORKSPACE_WINDOW
        CComPtr<IRepository> rep = AttachRepository();
        CString workspaceLabel = GetIConfig(QUERYBUILDER_CFG)->Get(WORKSPACE_CURRENT);
        m_Workspace->Send_Reset(rep->CreateIWorkspace(static_cast<const TCHAR *>(workspaceLabel)));  //Has to be send for the default workspace to make it to the correct place.
        DoWorkspaceRefresh();
#endif
        m_Repository->Post_Reset();
        m_RepositoryFilter->Post_Reset();
        m_Workunits->Post_Reset();
        m_WorkunitsActive->Post_Reset();
        m_WorkunitsFilter->Post_Reset();
#ifdef DFU_WINDOWS
        m_Dfu->Post_Reset();
        m_DfuFilter->Post_Reset();
#endif

        m_Syntax->Post_Reset();
        m_Bookmarks->Post_Reset();
        m_debugDataViews->Post_Reset();
        m_debugBreakpointView->Post_Reset();
        m_debugPropertiesView->Post_Reset();
        m_debugSearchView->Post_Reset();
        m_debugPropertyGridViews->Post_Reset();
    }

    SetTimer(TIMER_AUTOSAVE, static_cast<UINT>(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_AUTOSAVEFREQ)) * 1000, NULL);

    RestoreState();
    m_supressSyncTOC = false;
}

void CMainFrame::RestoreState()
{
    //  Workspace auto restore  ---
    m_persistedWindows.clear();
    m_activeWindows.clear();

    boost::filesystem::path folder;
    GetEnvironmentFolder(folder, static_cast<const TCHAR *>(CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_USER))), GetIConfig(QUERYBUILDER_CFG)->GetLabel());

    boost::filesystem::directory_iterator end_itr; // default construction yields past-the-end
    for (boost::filesystem::directory_iterator itr(folder); itr != end_itr; ++itr)
    {
        if (!clib::filesystem::is_directory(*itr))
        {
            if (boost::algorithm::iequals(pathToWString(itr->path().extension()), _T(".xml")))
                m_persistedWindows.push_back(new CPersistedItem(this, itr->path().string()));
        }
    }
    std::sort(m_persistedWindows.begin(), m_persistedWindows.end(), CPersistedItemCompare());
    SetTimer(TIMER_RESTORESTATE, 1, NULL);
}

void CMainFrame::PostSelectRibbon(RIBBON ribbon)
{
    PostMessage(UM_SELECTRIBBON, (WPARAM)ribbon);
}

void CMainFrame::DoPopupFileNew(LPNMTOOLBAR /*lpnmtb*/)
{
    WTL::CMenu m;
    m.LoadMenu(IDR_POPUP_NEW);
    POINT pt;
    ::GetCursorPos(&pt);
    BOOL id = m.GetSubMenu(0).TrackPopupMenuEx(TPM_RETURNCMD, pt.x, pt.y, m_hWnd);
    m.DestroyMenu();
    if (id)
        PostMessage(WM_COMMAND, id);
}

void CMainFrame::DoPopupFileOpen(LPNMTOOLBAR /*lpnmtb*/)
{
    WTL::CMenu m;
    m.LoadMenu(IDR_POPUP_OPEN);
    POINT pt;
    ::GetCursorPos(&pt);
    BOOL id = m.GetSubMenu(0).TrackPopupMenuEx(TPM_RETURNCMD, pt.x, pt.y, m_hWnd);
    m.DestroyMenu();
    if (id)
        PostMessage(WM_COMMAND, id);
}

void CMainFrame::DoPopupEclGo(LPNMTOOLBAR /*lpnmtb*/)
{
    WTL::CMenu pPopup; //= menu.GetSubMenu(0);
    pPopup.CreatePopupMenu();

    StlLinked<Topology::ITopology> server = AttachTopology(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_TOPOLOGY), _T("Topology"));
    Topology::IClusterVector clusters;
    server->GetClusters(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_QUEUE), clusters);
    unsigned int i = 1;
    for(Topology::IClusterVector::iterator itr = clusters.begin(); itr != clusters.end(); ++itr)
    {
        CString d = itr->get()->GetName();
        pPopup.AppendMenu((MF_ENABLED | MF_STRING | MF_BYPOSITION), i++, itr->get()->GetName());
    }

    POINT pt;
    ::GetCursorPos(&pt);
    i = pPopup.TrackPopupMenuEx(TPM_RETURNCMD, pt.x, pt.y, m_hWnd);
    if (i > 0 && i <= clusters.size())
    {
        std::_tstring cluster = clusters[i - 1].get()->GetName();
        GetIConfig(QUERYBUILDER_CFG)->Set(GLOBAL_CLUSTER, cluster);

        ::SendMessage(GetActive(), CWM_SYNCCLUSTER, 0, 0);
        PostMessage(WM_COMMAND, ID_ECL_GO);
    }
}

    void CMainFrame::DisableAllMenuItems()
    {
        //TODO
//		const WTL::_AtlUpdateUIMap* pMap = m_pUIMap;
//		WTL::_AtlUpdateUIData* pUIData = m_pUIData;
//		ATLASSERT(pUIData);
//
//		for( ; pMap->m_nID != (WORD)-1; pMap++, pUIData++)
//		{
//			if( pMap->m_wType & UPDUI_MENUPOPUP )
//			{
//				if( !(pUIData->m_wState & UPDUI_DISABLED) )
//				{
//					pUIData->m_wState |= pMap->m_wType;
//					pUIData->m_wState |= UPDUI_DISABLED;
//				}
//				if(pUIData->m_wState & pMap->m_wType)
//					m_wDirtyType |= pMap->m_wType;
//			}
//		}
//
//		//now turn these on all the time
//		UIEnable(ID_VIEW_TOOLBAR_MAIN, TRUE);
//		UIEnable(ID_VIEW_TOOLBAR_EDIT, TRUE);
//		UIEnable(ID_VIEW_TOOLBAR_ECL, TRUE);
//		UIEnable(ID_VIEW_TOOLBAR_BROWSER, TRUE);
//		UIEnable(ID_VIEW_TOOLBAR_FORMAT, TRUE);
//		UIEnable(ID_VIEW_TOOLBAR_GRAPH, TRUE);
//		UIEnable(ID_VIEW_STATUS_BAR, TRUE);
//		UIEnable(ID_VIEW_REPOSITORY, TRUE);
//		UIEnable(ID_VIEW_REPOSITORYFILTER, TRUE);
//#ifdef DFU_WINDOWS
//		UIEnable(ID_VIEW_DFU, TRUE);
//		UIEnable(ID_VIEW_DFUFILTER, TRUE);
//#endif
//		UIEnable(ID_VIEW_WORKUNITSTREE, TRUE);
//		UIEnable(ID_VIEW_ACTIVEWORKUNITSTREE, TRUE);
//		UIEnable(ID_VIEW_FILTEREDWORKUNITTREEFRAME, TRUE);
//		UIEnable(ID_VIEW_SYNTAX, TRUE);
//		UIEnable(ID_VIEW_ERROR, TRUE);
//
//		//and now tweak these
//		int nChildWindows = MDIGetCount();
//
//		UIEnable( ID_FILE_SAVE_ALL, nChildWindows > 0 );
//		UIEnable( ID_FILE_PRINT, nChildWindows > 0 );
//
//		UISetCheck(ID_VIEW_REPOSITORY,  m_Repository->IsWindow() && m_Repository->IsWindowVisible());
//		UISetCheck(ID_VIEW_REPOSITORYFILTER,  m_RepositoryFilter->IsWindow() && m_RepositoryFilter->IsWindowVisible());
//#ifdef DFU_WINDOWS
//		UISetCheck(ID_VIEW_DFU,  m_Dfu->IsWindow() && m_Dfu->IsWindowVisible());
//		UISetCheck(ID_VIEW_DFUFILTER,  m_DfuFilter->IsWindow() && m_DfuFilter->IsWindowVisible());
//#endif
//		UISetCheck(ID_VIEW_WORKUNITSTREE,  m_Workunits->IsWindow() && m_Workunits->IsWindowVisible());
//		UISetCheck(ID_VIEW_ERROR,  m_Error->IsWindow() && m_Error->IsWindowVisible());
//		UISetCheck(ID_VIEW_SYNTAX,  m_Syntax->IsWindow() && m_Syntax->IsWindowVisible());
//		UISetCheck(ID_VIEW_ACTIVEWORKUNITSTREE,  m_WorkunitsActive->IsWindow() && m_WorkunitsActive->IsWindowVisible());
//		UISetCheck(ID_VIEW_FILTEREDWORKUNITTREEFRAME,  m_FilteredWorkunitTreeFrame->IsWindow() && m_FilteredWorkunitTreeFrame->IsWindowVisible());
//
//		UIEnable( ID_WINDOW_CASCADE, nChildWindows > 0 );
//		UIEnable( ID_WINDOW_TILE_HORZ, nChildWindows > 0 );
//		UIEnable( ID_WINDOW_ARRANGE, nChildWindows > 0 );
//		UIEnable( ID_WINDOW_WINDOWS, nChildWindows > 0 );
//		UIEnable( ID_WINDOW_CLOSEALL, nChildWindows > 0 );
    }

HWND CMainFrame::GetActive()
{
    HWND hWndFocus = ::GetFocus();
    HWND hWndMDIActive = MDIGetActive()->GetSafeHwnd();

    if ((hWndMDIActive == hWndFocus) || (::IsChild(hWndMDIActive, hWndFocus)))
        return hWndMDIActive;

    return NULL;
}

HWND CMainFrame::DockedGetActive()
{
    HWND hWndFocus = ::GetFocus();

    //a little caching to speed this up
    if ( hWndFocus == m_lastWindowWithFocus )
    {
        return m_lastDockedWindowFrameWithFocus;
    }
    m_lastWindowWithFocus = hWndFocus;
    m_lastDockedWindowFrameWithFocus = 0;

    //if(m_Repository->IsChild(hWndFocus))
    //{
    //	m_lastDockedWindowFrameWithFocus = m_Repository->m_hWnd;
    //}
    //if(m_RepositoryFilter->IsChild(hWndFocus))
    //{
    //	m_lastDockedWindowFrameWithFocus = m_RepositoryFilter->m_hWnd;
    //}
#ifdef DFU_WINDOWS
    //if(m_Dfu->IsChild(hWndFocus))
    //{
    //	m_lastDockedWindowFrameWithFocus = m_Dfu->m_hWnd;
    //}
    //else if(m_DfuFilter->IsChild(hWndFocus))
    //{
    //	m_lastDockedWindowFrameWithFocus = m_DfuFilter->m_hWnd;
    //}
#endif
    //if(m_Workunits->IsChild(hWndFocus))
    //{
    //	m_lastDockedWindowFrameWithFocus = m_Workunits->m_hWnd;
    //}
    //if(m_Error->IsChild(hWndFocus))
    //{
    //	m_lastDockedWindowFrameWithFocus = m_Error->m_hWnd;
    //}
    //	else if(m_Output->IsChild(hWndFocus))
    //	{
    //		m_lastDockedWindowFrameWithFocus = m_Output->m_hWnd;
    //	}
    //	else if(m_Log->IsChild(hWndFocus))
    //	{
    //		m_lastDockedWindowFrameWithFocus = m_Log->m_hWnd;
    //	}
    //else if(m_Syntax->IsChild(hWndFocus))
    //{
    //	m_lastDockedWindowFrameWithFocus = m_Syntax->m_hWnd;
    //}
    //else if(m_WorkunitsActive->IsChild(hWndFocus))
    //{
    //	m_lastDockedWindowFrameWithFocus = m_WorkunitsActive->m_hWnd;
    //}
    //else if(m_FilteredWorkunitTreeFrame->IsChild(hWndFocus))
    //{
    //	m_lastDockedWindowFrameWithFocus = m_FilteredWorkunitTreeFrame->m_hWnd;
    //}
    return m_lastDockedWindowFrameWithFocus;
}

void CMainFrame::PostStatus(const TCHAR* pStr, PANE pane) 
{
    if ( !pStr )
        PostMessage(CWM_STATUSUPDATE, pane, 0);
    else
        PostMessage(CWM_STATUSUPDATE, pane, LPARAM(new CString(pStr)));
}

void CMainFrame::Send_RefreshStatusBar1(const TCHAR* pStr)
{
    ATLASSERT(pStr != 0);
    
    va_list args;	
    va_start(args, pStr);

    TCHAR buffer[512 * sizeof(TCHAR)] = {0};

    int nLen = wvsprintf(buffer, pStr, args);
    nLen;

    ATLASSERT(nLen < 512);
    ATLASSERT(nLen == lstrlen(buffer));

    CMFCRibbonBaseElement * elem = m_wndStatusBar.GetExElement(0);
    elem->SetText(buffer);

    va_end(args);
}

void CMainFrame::operator()(SectionLabel * label)
{
    //  Not called on main thread...
    PostMessage(CWM_STATUSUPDATE, PANE_LABEL, LPARAM(new CString(GetIConfig(QUERYBUILDER_CFG)->GetLabel())));
    if(GLOBAL_USER.first == *label)
    {
        PostMessage(CWM_STATUSUPDATE, PANE_USER, LPARAM(new CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_USER))));
    }
    else if (GLOBAL_CLUSTER.first == *label)
    {
        PostMessage(CWM_STATUSUPDATE, PANE_CLUSTER, LPARAM(new CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_CLUSTER))));
    }
    else if (GLOBAL_QUEUE.first == *label)
    {
        PostMessage(CWM_STATUSUPDATE, PANE_QUEUE, LPARAM(new CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_QUEUE))));
    }
}

bool CMainFrame::UIUpdateMenuItems()
{
    int nChildWindows = MDIGetCount();

    bool mdiActive = (0 != nChildWindows) && !DockedGetActive();
    if ( !mdiActive )
    {
        DisableAllMenuItems();
    }

    return mdiActive; //
}

BOOL CMainFrame::DoFileOpen(const CString & sPathName, int row, int col, int len)
{
    boost::filesystem::path path = stringToPath(static_cast<const TCHAR *>(sPathName));
    std::_tstring filename = pathToWString(path.leaf());
    CComPtr<IRepository> rep = AttachRepository();
    AttrInfo attrInfo;
    attrInfo.AttributeType = ExtensionWithoutDot(filename);
    return OpenFileBuilderMDI(this, attrInfo, sPathName, rep->CreateIWorkspaceItem(WORKSPACE_ITEM_BUILDER, filename, static_cast<const TCHAR *>(sPathName)), false, row, col, len);
}

BOOL CMainFrame::DoFileOpen(const CString & sPathName)
{
    boost::filesystem::path path = stringToPath(static_cast<const TCHAR *>(sPathName));
    std::_tstring filename = pathToWString(path.leaf());
    if (boost::algorithm::iequals(boost::filesystem::extension(path), ".xgmml"))
    {
        CComPtr<IRepository> rep = AttachRepository();
        return OpenFileGraphMDI(this, sPathName, rep->CreateIWorkspaceItem(WORKSPACE_ITEM_GRAPH, filename, static_cast<const TCHAR *>(sPathName)));
    }
    else if (boost::algorithm::iequals(boost::filesystem::extension(path), ".mod"))
    {
        IModuleAdapt targetModule = DoConfirmImportDlg(*this, path);
        m_Repository->Send_Refresh(targetModule);
        return TRUE;
    }
    CComPtr<IRepository> rep = AttachRepository();
    AttrInfo attrInfo;
    attrInfo.AttributeType = ExtensionWithoutDot(filename);
    return OpenFileBuilderMDI(this, attrInfo, sPathName, rep->CreateIWorkspaceItem(WORKSPACE_ITEM_BUILDER, filename, static_cast<const TCHAR *>(sPathName)), false);
}

BOOL CMainFrame::DoFileSaveAll(bool attrsOnly)
{
    BOOL retVal = EnumChildWindows(GetSafeHwnd(), ChildEnumProc, attrsOnly ? CWM_GLOBALSAVEATTRSONLY : CWM_GLOBALSAVE);
    const CObList * tabbedGroups = &GetMDITabGroups();
    if (!tabbedGroups)
        return retVal;

    for (POSITION pos = tabbedGroups->GetHeadPosition(); pos != NULL;)
    {
        CMFCTabCtrl* tabCtrl = DYNAMIC_DOWNCAST(CMFCTabCtrl, tabbedGroups->GetNext(pos));
        ASSERT_VALID(tabCtrl);
        int total = tabCtrl->GetTabsNum();
        for (int i = 0; i < total; ++i)
        {
            tabCtrl->GetThisClass();
            tabCtrl->SendMessageToDescendants(attrsOnly ? CWM_GLOBALSAVEATTRSONLY : CWM_GLOBALSAVE, 0, 0, FALSE, FALSE);
        }
    }
    return retVal;
}

void CMainFrame::DoWorkspaceRefresh()
{
    IWorkspaceVector workspaces;
    CComPtr<IRepository> rep = AttachRepository();
    rep->GetWorkspaces(&workspaces, false);

    CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> ar;
    m_wndRibbonBar.GetElementsByID (ID_WORKSPACE_LIST, ar);
    for (int i = 0; i < ar.GetSize (); i++)
    {
        CMFCRibbonComboBox * pWorkspace = DYNAMIC_DOWNCAST(CMFCRibbonComboBox, ar[i]);
        if (pWorkspace)
        {
            pWorkspace->RemoveAllItems();
            for(IWorkspaceVector::const_iterator itr =  workspaces.begin(); itr != workspaces.end(); ++itr)
                pWorkspace->AddItem(itr->get()->GetLabel(), (DWORD_PTR)itr->get());
            pWorkspace->SelectItem(m_Workspace->GetCurrentWorkspace()->GetLabel());
        }
    }
}

void CMainFrame::DoWorkspaceNew()
{
#ifdef WORKSPACE_WINDOW
    CString label = _T("Workspace Label");
    if (SaveWorkspaceLabel(AttachRepository(), label))
    {
        CComPtr<IRepository> rep = AttachRepository();
        CComPtr<IWorkspace> workspace = rep->CreateIWorkspace(static_cast<const TCHAR *>(label));
        DoWorkspaceSwitch(workspace, true);
    }
#endif
}

void CMainFrame::DoWorkspaceSave()
{
    if (!m_persistedWindows.size())
    {
        IWorkspaceItemVector wsItems;
        EnumChildWindows(GetSafeHwnd(), ChildEnumProcForSaveWorkspace, (LPARAM)&wsItems);
        CComPtr<IWorkspace> workspace = m_Workspace->GetCurrentWorkspace();
        ATLASSERT(workspace);
        workspace->SetWindows(wsItems);
        m_Workspace->Post_Refresh(true);
    }
}

void CMainFrame::DoWorkspaceLoad(IWorkspace * workspace)
{
    m_supressSyncTOC = true;
    typedef std::vector<std::pair<int, int> >ActiveTypeT;
    ActiveTypeT activeTabs;
    IWorkspaceItemVector windows;
    workspace->GetWindows(windows);
    for(IWorkspaceItemVector::const_iterator itr = windows.begin(); itr != windows.end(); ++itr)
    {
        if (itr->get()->IsActive())
            activeTabs.push_back(std::make_pair(itr->get()->GetGroupIndex(), itr->get()->GetTabIndex()));

        int groupIdx = itr->get()->GetGroupIndex();
        HWND newWin = NULL;
        switch(itr->get()->GetType())
        {
        case WORKSPACE_ITEM_BUILDER:
            newWin = OpenBuilderMDI(this, itr->get());
            break;
        case WORKSPACE_ITEM_ATTRIBUTE:
            if (CComPtr<IEclCC> eclcc = CreateIEclCC())
                newWin = OpenBuilderMDI(this, itr->get());
            else
                newWin = OpenAttributeMDI(this, itr->get());
            break;
        case WORKSPACE_ITEM_GRAPH:
            //OpenGraphMDI(this, itr->get());
            break;
        default:
            break;
        }
        //UpdateWindow();
        if (newWin)
        {
            const CObList * tabbedGroups = &GetMDITabGroups();
            while (tabbedGroups->GetSize() <= groupIdx)
            {
                MDITabNewGroup();
                tabbedGroups = &GetMDITabGroups();
            }
            ::UpdateWindow(newWin);
        }
    }
    for(ActiveTypeT::const_iterator itr = activeTabs.begin(); itr != activeTabs.end(); ++itr)
    {
        int targetGroup = itr->first;
        const CObList * tabbedGroups = &GetMDITabGroups();
        for (POSITION pos = tabbedGroups->GetHeadPosition(); pos != NULL;)
        {
            CMFCTabCtrl* tabCtrl = DYNAMIC_DOWNCAST(CMFCTabCtrl, tabbedGroups->GetNext(pos));
            if (--targetGroup < 0)
            {
                tabCtrl->SetActiveTab(itr->second);
                break;
            }
        }
    }
    IWorkspaceVector workspaceItems;
    workspace->GetChildWorkspaces(workspaceItems);
    for(IWorkspaceVector::const_iterator itr = workspaceItems.begin(); itr != workspaceItems.end(); ++itr)
    {
        DoWorkspaceLoad(itr->get());
    }
    m_supressSyncTOC = false;
}

void CMainFrame::DoWorkspaceRemove(const IWorkspaceVector & workspaces)
{
    bool removed = false;
    bool removedCurrent = false;
    for(IWorkspaceVector::const_iterator itr = workspaces.begin(); itr != workspaces.end(); ++itr)
    {
        if (itr->get()->IsDefault())
            continue;

        if (m_Workspace->GetCurrentWorkspace() == itr->get()) {
            EnumChildWindows(GetSafeHwnd(), ChildEnumProc, CWM_CLOSEALL);
            removedCurrent = true;
        }

        m_Workspace->RemoveWorkspace(itr->get());
        itr->get()->Remove();
        removed = true;
    }

    if (removedCurrent) {
        CComPtr<IRepository> rep = AttachRepository();
        DoWorkspaceSwitch(rep->CreateIWorkspace(), false);
    }

    if (removed)
        DoWorkspaceRefresh();
}

void CMainFrame::DoWorkspaceSwitch(IWorkspace * newWorkspace, bool saveCurrent)
{
    CWaitCursor wait;
    if (saveCurrent)
        DoWorkspaceSave();
    EnumChildWindows(GetSafeHwnd(), ChildEnumProc, CWM_CLOSEALL);
    m_Workspace->SetCurrentWorkspace(newWorkspace);
    DoWorkspaceLoad(newWorkspace);
    DoWorkspaceRefresh();

    CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> ar;
    m_wndRibbonBar.GetElementsByID (ID_WORKSPACE_LIST, ar);
    for (int i = 0; i < ar.GetSize (); i++)
    {
        CMFCRibbonComboBox * pWorkspace = DYNAMIC_DOWNCAST(CMFCRibbonComboBox, ar[i]);
        if (pWorkspace)
            pWorkspace->SelectItem(newWorkspace->GetLabel());
    }

    GetIConfig(QUERYBUILDER_CFG)->Set(WORKSPACE_CURRENT, std::_tstring(newWorkspace->GetLabel()));
}

int CMainFrame::MDIGetCount(int direction)
{
    int nCount = 0;
    int rCount = 0;
    int lCount = 0;

    CString activetab, tabstr;

    CMDIChildWnd* child = MDIGetActive();
    if (child == NULL)
    {
        return 0;
    }

    child->GetWindowText(activetab);

    const CObList * tabbedGroups = &GetMDITabGroups();
    bool found = false;

    for (POSITION pos = tabbedGroups->GetHeadPosition(); pos != NULL;)
    {
        CMFCTabCtrl* tabCtrl = DYNAMIC_DOWNCAST(CMFCTabCtrl, tabbedGroups->GetNext(pos));
        int activeTab = tabCtrl->GetActiveTab();
        int total = tabCtrl->GetTabsNum();

        for (int i = 0; i < total; i++)
        {
            tabCtrl->GetTabLabel(i, tabstr);
            if (tabstr == activetab)

            {
                found = true;
            }
            else if (found)
            {
                rCount++;
            }
            else
            {
                lCount++;
            }
        }
        if (found) {
            nCount = total;
            break;
        }
        else
        {
            nCount = 0;
            rCount = 0;
            lCount = 0;
        }
    }

    return direction == 0 ? nCount : direction < 0 ? lCount : rCount;
}

//BOOL CMainFrame::DockedWindowPreTranslate(MSG* pMsg)
//{
//	HWND hWndFocus = ::GetFocus();
//
//	//quick out if we already know this isn't a docked window
//	if ( !hWndFocus ||
//		(hWndFocus == m_lastWindowWithFocus && 
//		m_lastDockedWindowFrameWithFocus == 0) )
//	{
//		//isDockedWindow = false;
//		return FALSE;
//	}
//
//	//isDockedWindow = true; //default
//
//	m_lastWindowWithFocus = hWndFocus;
//	m_lastDockedWindowFrameWithFocus = 0;
//
//	if(m_Repository->IsChild(CWnd::FromHandle(hWndFocus)))
//	{
//		m_lastDockedWindowFrameWithFocus = m_Repository->m_hWnd;
//		return m_Repository->m_view.PreTranslateMessage(pMsg);
//	}
//	else if(m_RepositoryFilter->IsChild(CWnd::FromHandle(hWndFocus)))
//	{
//		m_lastDockedWindowFrameWithFocus = m_RepositoryFilter->m_hWnd;
//		return m_RepositoryFilter->m_view.PreTranslateMessage(pMsg);
//	}		
//#ifdef DFU_WINDOWS
//	else if(m_Dfu->IsChild(CWnd::FromHandle(hWndFocus)))
//	{
//		m_lastDockedWindowFrameWithFocus = m_Dfu->m_hWnd;
//		return m_Dfu->PreTranslateMessage(pMsg);
//	}
//	else if(m_DfuFilter->IsChild(CWnd::FromHandle(hWndFocus)))
//	{
//		m_lastDockedWindowFrameWithFocus = m_DfuFilter->m_hWnd;
//		return m_DfuFilter->m_view.PreTranslateMessage(pMsg);
//	}		
//#endif
//	else if(m_Workunits->IsChild(CWnd::FromHandle(hWndFocus)))
//	{
//		m_lastDockedWindowFrameWithFocus = m_Workunits->m_hWnd;
//		return m_Workunits->m_view.PreTranslateMessage(pMsg);
//	}
//	else if(m_Error->IsChild(CWnd::FromHandle(hWndFocus)))
//	{
//		m_lastDockedWindowFrameWithFocus = m_Error->m_hWnd;
//		return m_Error->m_view.PreTranslateMessage(pMsg);
//	}
//	//	else if(m_Output->IsChild(hWndFocus))
//	//	{
//	//		m_lastDockedWindowFrameWithFocus = m_Output->m_hWnd;
//	//		return m_Output->PreTranslateMessage(pMsg);
//	//	}
//	//	else if(m_Log->IsChild(hWndFocus))
//	//	{
//	//		m_lastDockedWindowFrameWithFocus = m_Log->m_hWnd;
//	//		return m_Log->PreTranslateMessage(pMsg);
//	//	}
//	else if(m_Syntax->IsChild(CWnd::FromHandle(hWndFocus)))
//	{
//		m_lastDockedWindowFrameWithFocus = m_Syntax->m_hWnd;
//		return m_Syntax->m_view.PreTranslateMessage(pMsg);
//	}
//	else if(m_WorkunitsActive->IsChild(CWnd::FromHandle(hWndFocus)))
//	{
//		m_lastDockedWindowFrameWithFocus = m_WorkunitsActive->m_hWnd;
//		return m_WorkunitsActive->m_view.PreTranslateMessage(pMsg);
//	}
//	else if(m_WorkunitsFilter->IsChild(CWnd::FromHandle(hWndFocus)))
//	{
//		m_lastDockedWindowFrameWithFocus = m_WorkunitsFilter->m_hWnd;
//		return m_WorkunitsFilter->m_view.PreTranslateMessage(pMsg);
//	}
//
//	return FALSE;
//}
//
void CMainFrame::OpenBuilder(const TCHAR *ecl)
{
    CComPtr<IRepository> rep = AttachRepository();
    ::OpenBuilderMDI(this, ecl, rep->CreateIWorkspaceItem(WORKSPACE_ITEM_BUILDER, NULL));
}

void CMainFrame::OpenBuilder(const CString & wuid, BuilderStartup startWith)
{
    StlLinked<Dali::IDali> server = Dali::AttachDali(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT), _T("Dali"));
    StlLinked<Dali::IWorkunit> workunit = server->GetWorkunit(wuid);
    if (workunit.get())
    {
        CComPtr<IRepository> rep = AttachRepository();
        ::OpenBuilderMDI(this, workunit.get(), rep->CreateIWorkspaceItem(WORKSPACE_ITEM_BUILDER, NULL), startWith);
    }
}

void CMainFrame::OpenBuilder(Dali::IWorkunit *wu, BuilderStartup startWith)
{
    //sometimes these WU don't have the result count, so do a full get
    Dali::IResultVector::const_iterator itr, end;
    boost::tie(itr, end) = wu->GetResultIterator();
    if (itr == end)
    {
        OpenBuilder(wu->GetWuid(), startWith);
    }
    else
    {
        CComPtr<IRepository> rep = AttachRepository();
        if (CComPtr<IEclCC> eclcc = CreateIEclCC())
        {
            const TCHAR * qualifiedLabel = wu->GetAppData(Dali::APP_DATA_QUALIFIEDLABEL);
            if (qualifiedLabel != NULL) {
                CComPtr<IRepository> rep = AttachRepository();
                CComPtr<IAttribute> attr = rep->GetAttribute(qualifiedLabel, CreateIAttributeECLType());
                if (attr) {
                    OpenAttribute(attr, wu);
                    return;
                }
            }
        }
        ::OpenBuilderMDI(this, wu, rep->CreateIWorkspaceItem(WORKSPACE_ITEM_BUILDER, NULL), startWith);
    }
}

void CMainFrame::OpenBuilder(IAttribute * attribute)
{
    CComPtr<IRepository> rep = AttachRepository();
    if (CComPtr<IEclCC> eclcc = CreateIEclCC())
    {
        std::_tstring label, path;
        ::OpenFileBuilderMDI(this, attribute->AttributeToInfo(), eclcc->GetAttributeFilePath(attribute, path), rep->CreateIWorkspaceItem(attribute, eclcc->GetAttributeFilePath(attribute, path)), attribute->IsLocked());
    }
    else
        ::OpenBuilderMDI(this, attribute, rep->CreateIWorkspaceItem(WORKSPACE_ITEM_BUILDER, attribute));
}

void threadSaveAs(CMainFrame * self, StlLinked<Dali::IWorkunit> wu, CString filePath)
{
    self->PostStatus((boost::_tformat(_T("Starting %1%.")) % wu->GetWuid()).str().c_str(), PANE_DEFAULT);
    using namespace YExcel;
    int sheetCount = wu->GetResultCount();
    BasicExcel e;
    e.New(sheetCount);
    Dali::IResultVector::const_iterator itr, end;
    //  Check for largest result set...
    int rowCount = 0;
    for(boost::tie(itr, end) = wu->GetResultIterator(); itr != end; ++itr)
    {
        CComPtr<Dali::IResult> result = itr->get();
        CComPtr<ITable> data = CreateISparseTable();
        __int64 total;
        result->GetResultData(0, 1, data, total);
        rowCount = total > rowCount ? total : rowCount;
    }
    bool continueSave = true;
    CString prog_title;
    prog_title.LoadString(IDR_MAINFRAME);
    if (rowCount > MAX_EXCEL_ROWS)
    {
        if (IDNO == self->MessageBox(_T("Warning:  Some results exceed 65535 rows and will be truncated.  Are you sure you want to continue?"), prog_title, MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON1))
        {
            continueSave = false;
        }
    }
    else if (rowCount > 10000)
    {
        if (IDNO == self->MessageBox((boost::_tformat(_T("Warning:  Some results exceed 10000 rows (largest is %1% rows) and will be slow to save.  Are you sure you want to continue?")) % rowCount).str().c_str(), prog_title, MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON1))
        {
            continueSave = false;
        }		
    }
    if (continueSave)
    {
        int i = 0;
        for(boost::tie(itr, end) = wu->GetResultIterator(); itr != end; ++i, ++itr)
        {
            CComPtr<Dali::IResult> result = itr->get();
            BasicExcelWorksheet* sheet = e.GetWorksheet(i);
            sheet->Rename(result->GetName());
            __int64 total;
            CComPtr<ITable> data = CreateISparseTable();
            result->GetResultData(0, 1, data, total);
            total = total > MAX_EXCEL_ROWS ? MAX_EXCEL_ROWS : total;
            for (int row = 0; row < total; row += SAVE_PAGE_SIZE)
            {
                self->PostStatus((boost::_tformat(_T("Saving %1%-%2% (%3% of %4%).")) % wu->GetWuid() % result->GetName() % row % total).str().c_str(), PANE_DEFAULT);
                __int64 tmpTotal;
                result->GetResultData(row, SAVE_PAGE_SIZE, data, tmpTotal);				
            }
            for(int row = 0; row < total; ++row)	//data may actualy hold more rows than total...
            {
                for(unsigned int col = 0; col < data->GetColumnCount(); ++col)
                {
                    sheet->Cell(row, col)->SetString(CT2A(data->GetCell(row, col), CP_UTF8));
                }
            }
        }
        e.SaveAs(CT2A(filePath));
    }
    self->PostStatus((boost::_tformat(_T("Finished %1%.")) % wu->GetWuid()).str().c_str(), PANE_DEFAULT);
}

void CMainFrame::SaveWorkunitsAs(CComPtr<Dali::IWorkunitVectorCom> wus)
{
    if (wus->size() == 1)
    {
        SaveWorkunitAs(wus->operator[](0).get());
    }
    else if(wus->size() > 1)
    {
        WTL::CFolderDialog wndFolderDialog(m_hWnd);	
        if ( IDOK == wndFolderDialog.DoModal () ) 
        {
            for(Dali::IWorkunitVector::iterator itr = wus->begin(); itr != wus->end(); ++itr)
            {
                m_threadSave.Append(__FUNCTION__, boost::bind(&threadSaveAs, this, *itr, (boost::_tformat(_T("%1%\\%2%.xls")) % wndFolderDialog.GetFolderPath() % itr->get()->GetWuid()).str().c_str()));
            }
        }
    }
}

void CMainFrame::SaveWorkunitAs(CComPtr<Dali::IWorkunit> wu)
{
    if (!wu)
    {
        ATLASSERT(false);
        return;
    }

    static const TCHAR szExcelFilter[] = _T("Excel Files (*.xls)\0*.xls\0All Files (*.*)\0*.*\0\0");
    std::_tstring wuid = wu->GetWuid();
    wuid += _T(".xls");
    CFileDialogEx wndFileDialog(FALSE, _T(".xls"), wuid.c_str(), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szExcelFilter, m_hWnd);	
    if ( IDOK == wndFileDialog.DoModal() ) 
    {
        m_threadSave.Append(__FUNCTION__, boost::bind(&threadSaveAs, this, wu.p, wndFileDialog.GetPathName()));
    }
}

void CMainFrame::SaveWorkunitAs(const std::_tstring & wuid)
{
    StlLinked<Dali::IDali> server = Dali::AttachDali(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT), _T("Dali"));
    SaveWorkunitAs(server->GetWorkunit(wuid.c_str()));
}

void CMainFrame::OpenSyntaxAttribute(const CString & modAttrLabel, IAttributeType * type, const CSyntaxErrorVector & errors)
{
    StlLinked<IAttribute> attribute = FuzzyLocate(modAttrLabel, type);
    if (attribute.get() != NULL)
    {
        CComPtr<IRepository> rep = AttachRepository();
        if (CComPtr<IEclCC> eclcc = CreateIEclCC())
        {
            std::_tstring label, path;
            ::OpenFileBuilderMDI(this, attribute->AttributeToInfo(), eclcc->GetAttributeFilePath(attribute, path), rep->CreateIWorkspaceItem(attribute, eclcc->GetAttributeFilePath(attribute, path)), attribute->IsLocked(), errors);
        }
        else
        {
            HWND hwnd = ::OpenAttributeMDI(this, attribute->GetModuleQualifiedLabel(), attribute->GetLabel(), type, rep->CreateIWorkspaceItem(WORKSPACE_ITEM_ATTRIBUTE, NULL), false, errors);
            if (hwnd)
                PostMessage(UM_MDICHILDACTIVATE, (WPARAM)hwnd);
        }
    }
    else
        MessageBeep(MB_ICONEXCLAMATION);
}

void CMainFrame::OpenSyntaxAttribute(IAttribute * attr, Dali::CEclExceptionVector * errors)
{
    CComPtr<IRepository> rep = AttachRepository();
    HWND hwnd = ::OpenAttributeMDI(this, attr, rep->CreateIWorkspaceItem(WORKSPACE_ITEM_ATTRIBUTE, NULL), false, errors);
    if (hwnd)
        PostMessage(UM_MDICHILDACTIVATE, (WPARAM)hwnd);
}

void CMainFrame::OpenAttribute(const CString & modAttrLabel, IAttributeType * type, const CString & currModule)
{
    StlLinked<IAttribute> attr = FuzzyLocate(modAttrLabel, type, currModule);
    if (attr.get() != NULL)
        OpenAttribute(attr);
    else
        MessageBeep(MB_ICONEXCLAMATION);
}

void CMainFrame::OpenAttribute(const std::_tstring & module, const std::_tstring & attribute, IAttributeType * type, unsigned int row, unsigned int col, unsigned int len)
{
    CComPtr<IRepository> rep = AttachRepository();
    if (CComPtr<IEclCC> eclcc = CreateIEclCC())
    {
        ATLASSERT(_T("TODO"));
    }
    else
    {
        HWND hwnd = ::OpenAttributeMDI(this, module, attribute, type, row, col, len, rep->CreateIWorkspaceItem(WORKSPACE_ITEM_ATTRIBUTE, NULL));
        if (hwnd)
            PostMessage(UM_MDICHILDACTIVATE, (WPARAM)hwnd);
    }
}

void CMainFrame::OpenAttribute(IAttribute * attribute, bool bHistoryView)
{
    CComPtr<IRepository> rep = AttachRepository();
    HWND hwnd = NULL;
    if (CComPtr<IEclCC> eclcc = CreateIEclCC())
    {
        std::_tstring label, path;
        ::OpenFileBuilderMDI(this, attribute->AttributeToInfo(), eclcc->GetAttributeFilePath(attribute, path), rep->CreateIWorkspaceItem(attribute, eclcc->GetAttributeFilePath(attribute, path)), attribute->IsLocked());
    }
    else
    {
        if (CComPtr<IEclCC> eclcc = CreateIEclCC())
        {
            ATLASSERT(_T("TODO"));
        }
        else
        {
            hwnd = ::OpenAttributeMDI(this, attribute->GetModuleQualifiedLabel(), attribute->GetLabel(), attribute->GetType(), rep->CreateIWorkspaceItem(WORKSPACE_ITEM_ATTRIBUTE, NULL), bHistoryView);
        }
    }
    if (hwnd)
        PostMessage(UM_MDICHILDACTIVATE, (WPARAM)hwnd);
}

void CMainFrame::OpenAttribute(IAttribute * attribute, const std::_tstring & searchTerm, FINDMODE findmode)
{
    _tcsncpy(m_fr.m_szFindWhat, searchTerm.c_str(), sizeof(m_fr.m_szFindWhat));
    m_fr.m_mode = findmode;
    CComPtr<IRepository> rep = AttachRepository();
    if (CComPtr<IEclCC> eclcc = CreateIEclCC())
    {
        ATLASSERT(_T("TODO"));
    }
    else
    {
        HWND hwnd = ::OpenAttributeMDI(this, attribute, searchTerm, findmode, rep->CreateIWorkspaceItem(WORKSPACE_ITEM_ATTRIBUTE, NULL));
        if (hwnd)
            PostMessage(UM_MDICHILDACTIVATE, (WPARAM)hwnd);
    }
}

void CMainFrame::OpenAttribute(IAttribute * attribute, Dali::IWorkunit * wu)
{
    CComPtr<IRepository> rep = AttachRepository();
    if (CComPtr<IEclCC> eclcc = CreateIEclCC())
    {
        std::_tstring label, path;
        ::OpenFileBuilderMDI(this, attribute->AttributeToInfo(), eclcc->GetAttributeFilePath(attribute, path), rep->CreateIWorkspaceItem(attribute, eclcc->GetAttributeFilePath(attribute, path)), attribute->IsLocked(), wu);
    }
    else
    {
        ATLASSERT(_T("NOT SUPPORTED"));
    }
}

void CMainFrame::SaveAttribute(IAttribute * attribute)
{
    ::SaveAttributeMDI(attribute);
}
void CMainFrame::InsertText(IAttribute * attribute)
{
    CString label = CString(attribute->GetModuleQualifiedLabel()) + "." + CString(attribute->GetLabel());
    ::SendMessage(MDIGetActive()->GetSafeHwnd(), CWM_INSERTTEXT, 0, (LPARAM)&label);
}
bool CMainFrame::ShowComment(IAttribute * attribute, CString & comment)
{
    return ::ShowComment(attribute, comment);
}

bool CMainFrame::ShowComment(CString & comment)
{
    return ::ShowComment(comment);
}
void CMainFrame::ShowHistory(IAttribute * attr)
{
    OpenAttribute(attr, true);
}
void CMainFrame::SyncTOC(const CString & modAttrLabel, IAttributeType * type, const CString & currModule)
{
    StlLinked<IAttribute> attr = FuzzyLocate(modAttrLabel, type, currModule);
    if (attr.get() != NULL)
        m_Repository->SendMessage(CWM_SYNCTOC, (WPARAM)attr.get());
    else
        MessageBeep(MB_ICONEXCLAMATION);
}
void CMainFrame::SyncTOC(IAttribute * attr)
{
    if (!m_supressSyncTOC)
        m_Repository->PostMessage(CWM_SYNCTOC, (WPARAM)attr);
}
void CMainFrame::SyncWorkunitTOC(const CString & wuid)
{
    StlLinked<Dali::IDali> server = Dali::AttachDali(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT), _T("Dali"));
    StlLinked<Dali::IWorkunit> workunit = server->GetWorkunit(wuid);
    if (workunit)
    {
        m_Workunits->SendMessage(CWM_SYNCTOC, (WPARAM)workunit.get());
    }
    else
    {
        MessageBeep(MB_ICONEXCLAMATION);
    }
}
IAttribute * CMainFrame::FuzzyLocate(const CString & _modAttrLabel, IAttributeType * type, const CString & currModule)
{
    CComPtr<IRepository> rep = AttachRepository();
    return rep->GetAttributeFuzzy(static_cast<const TCHAR * >(_modAttrLabel), static_cast<const TCHAR * >(currModule), type);
}

void CMainFrame::OnEditFind(const std::_tstring & findWhat, bool enableWildcardRegex, bool enableTooltip)
{
    _tcsncpy(m_fr.m_szFindWhat, findWhat.c_str(), sizeof(m_fr.m_szFindWhat));
    if (m_FindDlg.IsWindow()) 
    {
        m_FindDlg.SetFocus();
        return;
    }

    if (m_ReplaceDlg.IsWindow())
    {
        m_ReplaceDlg.SendMessage(WM_CLOSE);
    }

    m_fr.m_enableWildcardRegex = enableWildcardRegex;
    if(m_FindDlg.CreateEx(&m_fr) == 0)
    {		
        ::MessageBeep(MB_ICONEXCLAMATION);
        return;
    }

    m_FindDlg.ShowWindow(SW_NORMAL);
    return;
}

void CMainFrame::OnEditReplace(const std::_tstring & replaceWhat)
{
    _tcsncpy(m_fr.m_szFindWhat, replaceWhat.c_str(), sizeof(m_fr.m_szFindWhat));
    if (m_ReplaceDlg.IsWindow()) 
    {
        m_ReplaceDlg.SetFocus();
        return;
    }

    if (m_FindDlg.IsWindow())
    {
        m_FindDlg.SendMessage(WM_CLOSE);
    }

    if(m_ReplaceDlg.CreateEx(&m_fr) == 0)
    {		
        ::MessageBeep(MB_ICONEXCLAMATION);
        return;
    }

    m_ReplaceDlg.ShowWindow(SW_NORMAL);
    return;
}

void CMainFrame::CloneState(WTL::CMenu &m)
{
    for(int n = 0; n < m.GetMenuItemCount(); n++)
    {
        UINT id = m.GetMenuItemID(n);

        // menu separator or invalid cmd - ignore it
        if (id == 0) 
            continue; 

        if (id == (UINT)-1)
        {
            // possibly a popup menu, route to child menu if so
            WTL::CMenuHandle sub = m.GetSubMenu(n);
            if(sub) 
            {
                WTL::CMenu mSub(sub);
                CloneState( mSub );
                mSub.Detach(); //don't destroy the menu
            }
        }
        else 
        {
            //TODO
            //if (WTL::UIGetState(id) & WTL::CUpdateUIBase::UPDUI_DISABLED)
            //	m.EnableMenuItem(id, MF_GRAYED);
        }
    }
}

void CMainFrame::Send_RefreshWorkunitWindow()
{
    m_Workunits->Send_Refresh();
}
void CMainFrame::Send_RefreshRepositoryWindow(IModule * module)
{
    m_Repository->Send_Refresh(module);
}
void CMainFrame::Send_ClearSyntax()
{
    m_Syntax->Send_Clear();
}
void CMainFrame::Send_RefreshSyntax(ISciSyntaxMarker *errors, size_t curErr, bool forceShow)
{
    if (forceShow)// && !m_Syntax->IsWindowVisible())
    {
        if (!m_Syntax->IsVisible())
            m_Syntax->ShowPane(TRUE, FALSE, TRUE);

        m_Syntax->SetAutoHideMode(FALSE, CBRS_BOTTOM);
    }
    m_Syntax->Send_Refresh(errors, curErr);
}
void CMainFrame::Post_RefreshWorkunitWindow(Dali::IWorkunit * wu)
{
    m_Workunits->Post_Refresh(wu);
    m_WorkunitsActive->Post_Refresh(wu);
}
COLORREF CMainFrame::GetTextColor()
{
    CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> ar;
    m_wndRibbonBar.GetElementsByID (ID_FORMAT_FONTCOLOR, ar);

    for (int i = 0; i < ar.GetSize (); i++)
    {
        CMFCRibbonColorButton* pColor = DYNAMIC_DOWNCAST(CMFCRibbonColorButton, ar[i]);
        if (pColor != NULL)
        {
            return pColor->GetColor();
        }
    }

    return RGB(0, 0, 0);
}
bool CMainFrame::GetTextBold()
{
    CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> ar;
    m_wndRibbonBar.GetElementsByID (ID_FORMAT_FONTBOLD, ar);

    for (int i = 0; i < ar.GetSize (); i++)
    {
        CMFCRibbonCheckBox* pBold = DYNAMIC_DOWNCAST(CMFCRibbonCheckBox, ar[i]);
        if (pBold != NULL)
        {
            return pBold->IsChecked() == TRUE;
        }
    }

    return false;
}
bool CMainFrame::GetTextItalic()
{
    CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> ar;
    m_wndRibbonBar.GetElementsByID (ID_FORMAT_FONTITALIC, ar);

    for (int i = 0; i < ar.GetSize (); i++)
    {
        CMFCRibbonCheckBox* pItalic = DYNAMIC_DOWNCAST(CMFCRibbonCheckBox, ar[i]);
        if (pItalic != NULL)
        {
            return pItalic->IsChecked() == TRUE;
        }
    }

    return false;
}
COLORREF CMainFrame::GetFillColor()
{
    CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> ar;
    m_wndRibbonBar.GetElementsByID (ID_FORMAT_FILLCOLOR, ar);

    for (int i = 0; i < ar.GetSize (); i++)
    {
        CMFCRibbonColorButton* pColor = DYNAMIC_DOWNCAST(CMFCRibbonColorButton, ar[i]);
        if (pColor != NULL)
        {
            return pColor->GetColor();
        }
    }

    return RGB(255, 255, 255);
}

//  IFindReplace
FindReplace& CMainFrame::GetFindReplace()
{
    return m_fr;
}

void CMainFrame::FindReplaceOnEditFind(const std::_tstring &findWhat, bool enableWildcardRegex, bool enableTooltip)
{
    OnEditFind(findWhat, enableWildcardRegex, enableTooltip);
}

void CMainFrame::FindReplaceOnEditReplace(const std::_tstring &replaceWhat)
{
    OnEditReplace(replaceWhat);
}

void CMainFrame::OnFileNew()
{
    CComPtr<IRepository> rep = AttachRepository();
    OpenBuilderMDI(this, rep->CreateIWorkspaceItem(WORKSPACE_ITEM_BUILDER, NULL));
}

void CMainFrame::OnFileNewSpray()
{
    //std::_tstring ecl;
    //if (InvokeSprayWizard(ecl, false))
    //{
    //	CComPtr<IRepository> rep = AttachRepository();
    //	OpenBuilderMDI(this, ecl.c_str(), rep->CreateIWorkspaceItem(WORKSPACE_ITEM_BUILDER), _T(""));
    //}
}

void appendFileToFilterString(const std::_tstring & description, const std::_tstring & extension, std::_tstring & filterString, std::_tstring & extensionSring)
{
    filterString += (boost::_tformat(_T("%1% (*%2%)")) % description % extension).str();
    filterString.append(_T("\0"), 1);
    filterString += (boost::_tformat(_T("*%1%")) % extension).str();
    filterString.append(_T("\0"), 1);

    if (!extensionSring.empty())
        extensionSring += _T("; ");
    extensionSring += (boost::_tformat(_T("*%1%")) % extension).str();
}

void CMainFrame::OnFileOpen()
{
    //static const TCHAR szEclFilter[] = _T("Supported Files (*.ecl; *.mod; *.xgmml)\0*.ecl;*.mod;*.xgmml\0ECL Files (*.ecl)\0*.ecl\0Module Files (*.mod)\0*.mod\0XGMML Files (*.xgmml)\0*.xgmml\0All Files (*.*)\0*.*\0\0");
    IAttributeTypeVector types;
    GetAttributeTypes(types);
    std::_tstring tmpFilter;
    std::_tstring supportedFileExts;
    for(IAttributeTypeVector::const_iterator itr = types.begin(); itr != types.end(); ++itr)
    {
        appendFileToFilterString(itr->get()->GetDescription(), itr->get()->GetFileExtension(), tmpFilter, supportedFileExts);
    }
    appendFileToFilterString(_T("Module File"), _T(".mod"), tmpFilter, supportedFileExts);
    appendFileToFilterString(_T("XGMML File"), _T(".xgmml"), tmpFilter, supportedFileExts);

    std::_tstring openFilter = std::_tstring(_T("Supported Files (")) + supportedFileExts + _T(")");
    openFilter.append(_T("\0"), 1);
    openFilter += supportedFileExts;
    openFilter.append(_T("\0"), 1);
    openFilter += tmpFilter;
    static const TCHAR allFiles[] = _T("All Files (*.*)\0*.*\0\0");
    int sizeOfAllFiles = sizeof(allFiles) / sizeof(("A"));
    openFilter.append(allFiles, sizeOfAllFiles);

    CFileDialogEx wndFileDialog(TRUE, NULL, NULL, OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST, openFilter.c_str(), m_hWnd);	
    if ( IDOK == wndFileDialog.DoModal () ) 
    {
        // get the starting position on the opended files
        DWORD dwPos = wndFileDialog.GetStartPosition();
        while ( NULL != dwPos ) 
        {
            // extract the next file name...
            CString sPathName = wndFileDialog.GetNextPathName(dwPos);
            DoFileOpen(sPathName);
        }		
    }
}

void CMainFrame::OnFileOpenAttribute()
{
    CString attr;
    if (GetAttributeLabel(attr))
        OpenAttribute(attr, CreateIAttributeECLType());
}

void CMainFrame::OnFileOpenWorkunit()
{
    CString wu;
    if (GetWorkunit(wu, GetSafeHwnd()))
        OpenBuilder(wu, StartFirstResult);
}

void CMainFrame::OnFileSaveAll()
{
    DoFileSaveAll(false);
}

void CMainFrame::OnFileNewWorkspace()
{
    DoWorkspaceNew();
}

void CMainFrame::OnFilePageSetup()
{
    CPageSetupDialog dlg;
    dlg.DoModal();
}
    //ILogonConfigPreferences
bool CMainFrame::ShowConfigPreference(IConfig *config)
{
    return ::ShowPreferencesDlg(config);
}

void CMainFrame::OnFilePreferences()
{
    CString previousPassword = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_PASSWORD);

    DoLogout();

    if (::ShowPreferencesDlg(GetIConfig(QUERYBUILDER_CFG), true))
    {
        //if they changed the config file, set it as the default, so the login uses it
        ::SetDefaultConfig(m_iniFile, ::GetIConfig(QUERYBUILDER_CFG)->GetLabel());
    }

    DoLogin(true, previousPassword);
}

void CMainFrame::OnFileLogin()
{
    DoLogout();
    DoLogin();
}

void CMainFrame::OnFileChangePassword()
{
    DoModalChangePassword();
}

void CMainFrame::OnFileExit()
{
    PostMessage(WM_CLOSE);
}

void CMainFrame::OnEditAdvanced()
{
    CPoint pt;
    GetCursorPos(&pt);

    theApp.ShowPopupMenu(IDR_POPUP_ADVANCED, pt, this);
}

CMFCTabCtrl* CMainFrame::GetActiveTabGroup()
{
    const CObList * tabbedGroups = &GetMDITabGroups();
    bool found = false;

    for (POSITION pos = tabbedGroups->GetHeadPosition(); pos != NULL;)
    {
        CMFCTabCtrl* tabGroup = DYNAMIC_DOWNCAST(CMFCTabCtrl, tabbedGroups->GetNext(pos));
        if (tabGroup->IsActiveInMDITabGroup())
        {
            return tabGroup;
        }
    }
    return NULL;
}

void CMainFrame::OnMdiCloseRest() 
{
    CMFCTabCtrl* tabGroup = GetActiveTabGroup();
    int active = tabGroup->GetActiveTab();

    for (int i = tabGroup->GetTabsNum() - 1; i >= 0; i--)
    {
        if (i == active)
        {
            tabGroup->SetActiveTab(i);
        }
        else
        {
            tabGroup->RemoveTab(i);
        }
    }
}

void CMainFrame::OnMdiCloseLeft()
{
    bool found = false;

    CMFCTabCtrl* tabGroup = GetActiveTabGroup();
    int active = tabGroup->GetActiveTab();

    for (int i = tabGroup->GetTabsNum() - 1; i >= 0; i--)
    {
        if (i == active)
        {
            found = true;
        }
        else if (found)
        {
            tabGroup->RemoveTab(i);
        }
    }
    if (found)
    {
        tabGroup->SetActiveTab(0);
    }
}

void CMainFrame::OnMdiCloseRight()
{
    CMFCTabCtrl* tabGroup = GetActiveTabGroup();
    int active = tabGroup->GetActiveTab();

    for (int i = tabGroup->GetTabsNum() - 1; i >= 0; i--)
    {
        if (i == active)
        {
            tabGroup->SetActiveTab(i);
            break;
        }
        else
        {
            tabGroup->RemoveTab(i);
        }
    }
}

void CMainFrame::OnMdiMoveToNextGroup()
{
    MDITabMoveToNextGroup();
}

void CMainFrame::OnMdiMoveToPrevGroup()
{
    MDITabMoveToNextGroup(FALSE);
}

void CMainFrame::OnMdiNewHorzTabGroup()
{
    MDITabNewGroup(FALSE);
}

void CMainFrame::OnMdiNewVertGroup()
{
    MDITabNewGroup();
}

void CMainFrame::OnMdiCancel()
{
}

void CMainFrame::OnWindowsCloseAll()
{
    if (EnumChildWindows(GetSafeHwnd(), ChildEnumProc, CWM_GLOBALQUERYSAVE))  
        EnumChildWindows(GetSafeHwnd(), ChildEnumProc, CWM_CLOSEALLCLEARAUTOSAVE);
}

void Tidy(::CString & str)
{
    str.TrimLeft(_T("*"));
    str.MakeLower();
}

int CompareNoCase(const ::CString & lhs, const ::CString & rhs)
{
    bool lhsIsBuilder = (lhs.Find(_T(".ecl")) >= (int)0);
    bool rhsIsBuilder = (rhs.Find(_T(".ecl")) >= (int)0);
    if (lhsIsBuilder && !rhsIsBuilder)
        return -1;
    else if (!lhsIsBuilder && rhsIsBuilder)
        return 1;
    return lhs.CompareNoCase(rhs);
}

void CMainFrame::OnWindowsSort()
{
    const CObList * tabbedGroups = &GetMDITabGroups();
    POSITION pos = NULL;
    for (pos = tabbedGroups->GetHeadPosition(); pos != NULL;)
    {
        CMFCTabCtrl* tabCtrl = DYNAMIC_DOWNCAST(CMFCTabCtrl, tabbedGroups->GetNext(pos));
        ASSERT_VALID(tabCtrl);

        int activeTab = tabCtrl->GetActiveTab();
        int total = tabCtrl->GetTabsNum();
        bool repeat = true;
        while(repeat)  //bad bubble sort
        {
            repeat = false;
            for(int i = 0; i < total - 1; ++i)
            {
                ::CString lhs, rhs;
                tabCtrl->GetTabLabel(i, lhs);
                tabCtrl->GetTabLabel(i + 1, rhs);
                Tidy(lhs);
                Tidy(rhs);
                if (CompareNoCase(lhs, rhs) > (int)0)
                {
                    tabCtrl->SwapTabs(i, i + 1);
                    if (activeTab == i)
                        activeTab = i + 1;
                    else if (activeTab == i + 1)
                        activeTab = i;
                    repeat = true;
                }
            }
        }
        tabCtrl->RecalcLayout();
        if (activeTab >= 0)
            tabCtrl->SetActiveTab(activeTab);
        tabCtrl->RedrawWindow();
    }
}

void CMainFrame::OnWindowsGroup()
{
    const CObList * origTabbedGroups = &GetMDITabGroups();
    if (origTabbedGroups->GetSize() == 0)
        return;
    else if (origTabbedGroups->GetSize() <= 1)
    {
        POSITION pos = origTabbedGroups->GetHeadPosition();
        CMFCTabCtrl* tabCtrl = DYNAMIC_DOWNCAST(CMFCTabCtrl, origTabbedGroups->GetNext(pos));
        if (tabCtrl->GetTabsNum() == 1)
            return;
        MDITabNewGroup();
    }

    const CObList * tabbedGroups = &GetMDITabGroups();
    ATLASSERT(tabbedGroups->GetSize() >= 2);

    POSITION pos = tabbedGroups->GetHeadPosition();
    CMFCTabCtrl* builderTabs = DYNAMIC_DOWNCAST(CMFCTabCtrl, tabbedGroups->GetNext(pos));
    CMFCTabCtrl* attrTabs = DYNAMIC_DOWNCAST(CMFCTabCtrl, tabbedGroups->GetNext(pos));

    for (pos = tabbedGroups->GetHeadPosition(); pos != NULL;)
    {
        CMFCTabCtrl* tabCtrl = DYNAMIC_DOWNCAST(CMFCTabCtrl, tabbedGroups->GetNext(pos));
        ASSERT_VALID(tabCtrl);

        int total = tabCtrl->GetTabsNum();
        for (int i = 0; i < total; ++i)
        {
            ::CString label;
            tabCtrl->GetTabLabel(i, label);
            if (label.Find(_T(".ecl")) >= (int)0)
            {
                if (tabCtrl != builderTabs)
                {
                    m_wndClientArea.MoveWindowToTabGroup(tabCtrl, builderTabs, i);
                    --total;
                    --i;
                }
            }
            else
            {
                if (tabCtrl != attrTabs)
                {
                    m_wndClientArea.MoveWindowToTabGroup(tabCtrl, attrTabs, i);
                    --total;
                    --i;
                }
            }
        }
    }
}

void CMainFrame::OnWindowsDegroup()
{
    const CObList * tabbedGroups = &GetMDITabGroups();
    if (tabbedGroups->GetSize() < 2)
        return;

    POSITION pos = tabbedGroups->GetHeadPosition();
    CMFCTabCtrl* firstTab = DYNAMIC_DOWNCAST(CMFCTabCtrl, tabbedGroups->GetNext(pos));
    while(pos != NULL)
    {
        CMFCTabCtrl* tabCtrl = DYNAMIC_DOWNCAST(CMFCTabCtrl, tabbedGroups->GetNext(pos));
        ASSERT_VALID(tabCtrl);

        while (tabCtrl->GetTabsNum())
        {
            m_wndClientArea.MoveWindowToTabGroup(tabCtrl, firstTab, 0);
        }
    }
}

void CMainFrame::OnUpdateCloseRest(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(m_tabbedMDI && MDIGetCount() > 1);
}

void CMainFrame::OnUpdateCloseLeft(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(m_tabbedMDI && MDIGetCount(-1) > 0);
}

void CMainFrame::OnUpdateCloseRight(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(m_tabbedMDI && MDIGetCount(1) > 0);
}

void CMainFrame::OnUpdateWindowsSort(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(m_tabbedMDI && MDIGetCount() > 1);
}

void CMainFrame::OnUpdateViewDockedPane(CCmdUI* pCmdUI)
{
    CBasePane* pBar = GetPane(pCmdUI->m_nID);
    if (pBar != NULL)
    {
        pCmdUI->SetCheck(pBar->IsVisible());
        return;
    }

    pCmdUI->ContinueRouting();
}

BOOL CMainFrame::OnViewDockedPane(UINT nID)
{
    ASSERT_VALID(this);

    CBasePane* pBar = GetPane(nID);
    if (pBar != NULL)
    {
        pBar->ShowPane(!pBar->IsVisible(), FALSE, TRUE);
        return TRUE;
    }

    return FALSE;
}

void Undock(CDockablePane * dockedWindow)
{
    if (dockedWindow->IsAutoHideMode())
        dockedWindow->UnSetAutoHideMode(NULL);

    HWND hWndTab;
    CMFCBaseTabCtrl* tabbedWindow = dockedWindow->GetParentTabWnd(hWndTab);
    if (tabbedWindow)
        tabbedWindow->DetachTab(DM_STANDARD);

    if (dockedWindow->IsDocked())
        dockedWindow->UndockPane();

    dockedWindow->ShowPane(TRUE, FALSE, TRUE);
}

BOOL CMainFrame::OnViewResetDockedToolbars(UINT nID)
{
    Undock(m_Repository);
    Undock(m_RepositoryFilter);
    Undock(m_Workunits);
    Undock(m_WorkunitsActive);
    Undock(m_WorkunitsFilter);
#ifdef WORKSPACE_WINDOW
    Undock(m_Workspace);
#endif
    Undock(m_Dfu);
    Undock(m_DfuFilter);
    Undock(m_Syntax);
    Undock(m_Bookmarks);
    Undock(m_debugDataViews);
    Undock(m_debugBreakpointView);
    Undock(m_debugPropertiesView);
    Undock(m_debugSearchView);
    Undock(m_debugPropertyGridViews);
    Undock(m_Error);

    WORKSPACE workspaceMode = m_workspaceMode;
    SetWorkspaceMode(WORKSPACE_NORMAL);
    theApp.CleanState(getWorkspaceString(WORKSPACE_DEBUG));
    theApp.CleanState(getWorkspaceString(WORKSPACE_GRAPH));
    theApp.CleanState(getWorkspaceString(WORKSPACE_NORMAL));

    InitializeDocking(nID);

    SetWorkspaceMode(workspaceMode);
    return TRUE;
}

HWND CMainFrame::GetHwnd()
{
    return GetSafeHwnd();
}

//  ===========================================================================
BOOL CALLBACK ChildEnumProcForSaveWorkspace(HWND hwnd, LPARAM lParam)
{
    IWorkspaceItemVector * wsItems = (IWorkspaceItemVector *)lParam;
    SendMessage(hwnd, CWM_SAVEPERSISTINFO, 0, (LPARAM)wsItems);
    return TRUE;
}

BOOL CALLBACK ChildEnumProc(HWND hwnd, LPARAM lParam)
{
    HWND hwndClient, hwndFrame;

    if (::GetWindow(hwnd, GW_OWNER))
        return TRUE;

    hwndClient = ::GetParent(hwnd);
    hwndFrame = ::GetParent(hwndClient);
    CMDIChildWndEx* pFrame = DYNAMIC_DOWNCAST(CMDIChildWndEx, CWnd::FromHandle(hwnd));

    switch (lParam)
    {
    case CWM_CLOSEALLCLEARAUTOSAVE:
        if (pFrame == NULL)
            return true;
        SendMessage(hwnd, CWM_CLOSEALLCLEARAUTOSAVE, 0, 0);
        SendMessage(hwndClient, WM_MDIDESTROY, (WPARAM) (HWND) hwnd, 0);
        break;

    case CWM_CLOSEALL:
        if (pFrame == NULL)
            return true;
        SendMessage(hwndClient, WM_MDIDESTROY, (WPARAM) (HWND) hwnd, 0);
        break;

    case CWM_GLOBALSAVE:
    case CWM_GLOBALSAVEATTRSONLY:
    case CWM_GLOBALQUERYSAVE:
        {
            bool bClose = TRUE;
            SendMessage(hwnd, lParam, 0, (LPARAM) &bClose);	
            if (!bClose)
                return false;
        }
        break;
    }
    return true;
}

BOOL CALLBACK ChildEnumProcInfo(HWND hwnd, LPARAM lParam)
{
    CWindow wnd(hwnd);
    TStringHWNDVector * childWindows = (TStringHWNDVector *)lParam;
    HWND hwndClient, hwndFrame;

    if (::GetWindow(hwnd, GW_OWNER))
        return TRUE;

    hwndClient = ::GetParent(hwnd);
    hwndFrame = ::GetParent(hwndClient);

    CString title;
    wnd.GetWindowText(title);
    childWindows->push_back(std::make_pair((const TCHAR *)title, hwnd));
    return true;
}

WTL::CUpdateUIBase *GetUIUpdate()
{
    return NULL;//TODO:  g_MainFrame;
}

FindReplace& GetFindReplace()
{
    return g_MainFrame->m_fr;
}

void FindReplaceOnEditFind(const std::_tstring &findWhat, bool enableWildcardRegex, bool enableTooltip)
{
    g_MainFrame->OnEditFind(findWhat, enableWildcardRegex, enableTooltip);
}

void FindReplaceOnEditReplace(const std::_tstring &replaceWhat)
{
    g_MainFrame->OnEditReplace(replaceWhat);
}

