#include "stdafx.h"

#include "AMT.h"
#include "PreferenceDlg.h"
#include "Status.h"

#include "MainFrm2.h"

#include <UtilFilesystem.h>

CMainFrame *g_MainFrame = 0;

static const TCHAR RepLabel1[] = _T("Source Repository");
static const TCHAR RepLabel2[] = _T("Target Repository");
static const TCHAR * regPath = _T("Software\\HPCC Systems\\AMT");

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_SETFOCUS()
	ON_MESSAGE(UM_INITIALIZE, OnInitialize)
	ON_MESSAGE(CWM_STATUSUPDATE, OnStatusUpdate)
	ON_MESSAGE(UM_PROGRESSUPDATE, OnProgressUpdate)
	ON_MESSAGE(UM_CLEAR_DIFF, OnClearDiff)
	ON_MESSAGE(UM_NEWSELECTION, OnNewSelection)
	//ON_MESSAGE(UM_NEWCHECK, OnNewCheck)
	ON_MESSAGE(UM2_GETECLLOADED, OnGetEclLoaded)

	ON_COMMAND(ID_FILE_LOGOUT, OnFileLogout)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_COMMAND(ID_EDIT_ADDTOREP, OnEditAddToRep)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADDTOREP, OnUpdateEditAddToRep)
	ON_COMMAND(ID_EDIT_SHOW_MATCHING, OnEditShowMatching)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SHOW_MATCHING, OnUpdateEditShowMatching)
	ON_COMMAND(ID_EDIT_SHOW_NOT_MATCHING, OnEditShowNotMatching)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SHOW_NOT_MATCHING, OnUpdateEditShowNotMatching)
	ON_COMMAND(ID_EDIT_SHOW_ORPHANS, OnEditShowOrphans)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SHOW_ORPHANS, OnUpdateEditShowOrphans)
	ON_COMMAND(ID_EDIT_SHOW_ALL, OnEditShowAll)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_COMMAND(ID_EDIT_CLEARALL, OnEditClearAll)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEARALL, OnUpdateEditClearAll)
	ON_COMMAND(ID_EDIT_EXPANDALL, OnEditExpandAll)
	ON_COMMAND(ID_EDIT_REFRESH, OnEditRefresh)

	ON_COMMAND(ID_VIEW_CRLF, OnViewCRLF)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CRLF, OnUpdateViewCRLF)
	ON_COMMAND(ID_VIEW_WHITESPACE, OnViewWhitespace)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WHITESPACE, OnUpdateViewWhitespace)

	ON_COMMAND(ID_DIFFERENCES_PREVIOUS, OnDiffPrevious)
	ON_UPDATE_COMMAND_UI(ID_DIFFERENCES_PREVIOUS, OnUpdateDiffPrevious)
	ON_COMMAND(ID_DIFFERENCES_NEXT, OnDiffNext)
	ON_UPDATE_COMMAND_UI(ID_DIFFERENCES_NEXT, OnUpdateDiffNext)

	ON_COMMAND(ID_DEPENDEES_FINDSERVICES, OnFindServices)
	ON_UPDATE_COMMAND_UI(ID_DEPENDEES_FINDSERVICES, OnUpdateFindServices)
	ON_COMMAND(ID_DEPENDEES_CALCULATEDEPENDENCIES, OnCalcDependencies)
	ON_UPDATE_COMMAND_UI(ID_DEPENDEES_CALCULATEDEPENDENCIES, OnUpdateCalcDependencies)
	ON_COMMAND(ID_DEPENDEES_CALCULATECROSSREFERENCE, OnCalcCrossreferences)
	ON_UPDATE_COMMAND_UI(ID_DEPENDEES_CALCULATECROSSREFERENCE, OnUpdateCalcCrossreferences)
	ON_COMMAND(ID_DEPENDEES_SYNCFROMSELECTION, OnSyncFromSelection)
	ON_UPDATE_COMMAND_UI(ID_DEPENDEES_SYNCFROMSELECTION, OnUpdateSyncFromSelection)
	ON_COMMAND(ID_DEPENDEES_SYNCTOSELECTION, OnSyncToSelection)
	ON_UPDATE_COMMAND_UI(ID_DEPENDEES_SYNCTOSELECTION, OnUpdateSyncToSelection)
	ON_COMMAND(ID_DEPENDEES_COPYCROSSREFERENCE, OnCopyCrossreference)
	ON_UPDATE_COMMAND_UI(ID_DEPENDEES_COPYCROSSREFERENCE, OnUpdateCopyCrossreference)

	ON_COMMAND(ID_EDIT_MODULE_CHECKSUM, OnEditModuleChecksum)
	ON_UPDATE_COMMAND_UI(ID_EDIT_MODULE_CHECKSUM, OnUpdateEditModuleChecksum)

	ON_COMMAND(ID_HELP_CLIENTTOOLS, OnHelpClientTools)

	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_OFF_2007_AQUA, OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_OFF_2007_AQUA, OnUpdateApplicationLook)
END_MESSAGE_MAP()

// CMainFrame construction/destruction

CMainFrame::CMainFrame() : m_sourceEclView(this), m_targetEclView(this), m_EditStrBuf(m_warnView), m_EditStrBufW(m_warnView), m_bStreamInit(false)
{
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_OFF_2007_BLUE);

	g_MainFrame = this;

	boost::filesystem::path iniPath;
	GetIniPath(iniPath);
	m_iniFile = CreateIConfig(AMT_INI, iniPath);
	m_cfgLHS = CreateIConfig(AMT_CFG_LHS);
	m_cfgRHS = CreateIConfig(AMT_CFG_RHS);

	m_bCheckBoxMatching = true;
	m_bCheckBoxNotMatching = true;
	m_bCheckBoxOrphans = true;
	m_bCheckBoxModuleChecksum = true;

	m_bCheckBoxCRLF = false;
	m_bCheckBoxWhitespace = false;

	m_progress = 0;
}

CMainFrame::~CMainFrame()
{
	g_MainFrame = NULL;
}

void CMainFrame::RecalcLayout(BOOL bNotify)
{
	__super::RecalcLayout(bNotify);
	if (m_splitter.IsWindow())
	{
		//CRect rectClientAreaBounds = m_dockManager.GetClientAreaBounds();

		//rectClientAreaBounds.left += m_rectBorder.left;
		//rectClientAreaBounds.top  += m_rectBorder.top;
		//rectClientAreaBounds.right -= m_rectBorder.right;
		//rectClientAreaBounds.bottom -= m_rectBorder.bottom;

		//if (m_wndClientArea.GetSafeHwnd() != NULL)
		//{
		//	m_wndClientArea.CalcWindowRect(rectClientAreaBounds, 0);
		//}

		CRect rectClientAreaBounds;
		m_wndClientArea.GetClientRect(rectClientAreaBounds);

		m_splitter.SetWindowPos(wndBottom.GetSafeHwnd(), rectClientAreaBounds.left, rectClientAreaBounds.top, rectClientAreaBounds.Width(), rectClientAreaBounds.Height(), SWP_NOACTIVATE);
		m_splitter.UpdateWindow();
	}
}

void CMainFrame::DoLogin()
{
	StlLinked<IRepository> repLHS;
	StlLinked<IRepository> repRHS;
	if (DoModalLoginAMT(this, m_iniFile, repLHS, repRHS, m_daliLHS, m_daliRHS) != IDOK)
	{
		DestroyWindow();
		return;
	}

	std::_tstring source = _T("Source (");
	source += repLHS->GetLabel();
	source += _T(")");

	std::_tstring target = _T("Target (");
	target += repRHS->GetLabel();
	target += _T(")");

	m_repositoryDlg.Init(source, target, repLHS, repRHS, m_daliLHS);
	m_dependees.SetConfig(m_cfgRHS);
}

void CMainFrame::DoLogout()
{
	ShowDiff();
	m_repositoryDlg.Kill();
	::ReleaseAllSingletons();

	m_dependees.SetConfig(NULL);
}

bool CMainFrame::ShowConfigPreference(IConfig *config)
{
	return ::ShowPreferencesDlg(config);
}

void thread_GetText(IAttribute * attr, std::_tstring * ecl)
{
	if (attr->Exists())
		*ecl = attr->GetText(false);
}

void thread_GetHistoryEcl(IAttributeHistory * hist, std::_tstring * ecl)
{
	*ecl = hist->GetText();
}

void thread_GetBothEcl(CMainFrame * self, CAttributePairNode * attributeNode)
{
	LogMsg((boost::_tformat(_T("Getting ECL (%1%) - Start")) % attributeNode->m_lhs->GetQualifiedLabel()).str());
	std::_tstring lhs;
	std::_tstring rhs;
	clib::thread lhsRun(__FUNCTION__, boost::bind(&thread_GetText, attributeNode->m_lhs, &lhs));
	clib::thread rhsRun(__FUNCTION__, boost::bind(&thread_GetText, attributeNode->m_rhs, &rhs));
	lhsRun.join();
	rhsRun.join();
	LogMsg((boost::_tformat(_T("Getting ECL (%1%) - End")) % attributeNode->m_lhs->GetQualifiedLabel()).str());
	self->PostMessage(UM2_GETECLLOADED, (WPARAM)attributeNode);
}

void thread_GetHistoryBothEcl(CMainFrame * self, CAttributeHistoryPairNode * historyNode)
{
	CComQIPtr<CAttributePairNode> attrNode = historyNode->GetParentNode();
	LogMsg((boost::_tformat(_T("Getting ECL (%1%) - Start")) % attrNode->m_lhs->GetQualifiedLabel()).str());
	std::_tstring lhs;
	std::_tstring rhs;
	clib::thread lhsRun(__FUNCTION__, boost::bind(&thread_GetHistoryEcl, historyNode->m_lhs, &lhs));
	clib::thread rhsRun(__FUNCTION__, boost::bind(&thread_GetText, historyNode->m_rhs, &rhs));
	lhsRun.join();
	rhsRun.join();
	LogMsg((boost::_tformat(_T("Getting ECL (%1%) - End")) % attrNode->m_lhs->GetQualifiedLabel()).str());
	self->PostMessage(UM2_GETECLLOADED, (WPARAM)historyNode);
}

void CMainFrame::OnItemSelectionChanged(CTreeNode *node, bool bSelected)
{
    if ( !bSelected )
        return;

    if (CComQIPtr<CModulePairNode> moduleNode = node)
    {
		std::_tstring lhsInfo, rhsInfo;
		if (moduleNode->hasLHS())
			lhsInfo = (boost::_tformat(_T("%1% (crc:  %2%)\r\n")) % moduleNode->m_lhs->GetQualifiedLabel() % moduleNode->m_lhs->GetChecksum()).str();
		if (moduleNode->hasRHS())
			rhsInfo = (boost::_tformat(_T("%1% (crc:  %2%)\r\n")) % moduleNode->m_rhs->GetQualifiedLabel() % moduleNode->m_rhs->GetChecksum()).str();

		ShowDiff(_T(""), lhsInfo, _T(""), rhsInfo);
    }
    else if (CComQIPtr<CAttributePairNode> attributeNode = node)
    {
		clib::thread run(__FUNCTION__, boost::bind(&thread_GetBothEcl, this, attributeNode));
    }
    else if (CComQIPtr<CAttributeHistoryPairNode> historyNode = node)
    {
		clib::thread run(__FUNCTION__, boost::bind(&thread_GetHistoryBothEcl, this, historyNode));
    }
    else
    {
		//m_other->SelectNode(node, m_view->GetNodeIndex(node));
        //Repository Node
        //m_diffViewer->ShowLog(); 
    }
}

void CMainFrame::ShowDiff(const std::_tstring & lhs, const std::_tstring & lhsHeader, const std::_tstring & rhs, const std::_tstring & rhsHeader)
{
	m_sourceEclView.SetReadOnly(false);
	m_sourceEclView.SetText(lhs.c_str());
	m_sourceEclView.SetReadOnly(true);
	m_targetEclView.SetReadOnly(false);
	m_targetEclView.SetText(rhs.c_str());
	m_targetEclView.SetReadOnly(true);

	m_diffView.SetText(lhs, rhs);
}

bool CMainFrame::GenerateHeader(IAttribute *attr, IAttribute *attrOther, std::_tstring & header)
{
	bool retVal = false;
	header += (boost::_tformat(_T("Server CRC:\t%1%\r\nLocal CRC:\t%2%\r\n")) % attr->GetChecksum() % attr->GetChecksumLocal()).str();
	if (!boost::algorithm::equals(attr->GetChecksumLocal(), attrOther->GetChecksumLocal()) && boost::algorithm::equals(attr->GetChecksumLocalTidied(), attrOther->GetChecksumLocalTidied()))
	{
		header += _T("\r\nWarning:  Carriage Return and Linefeeds do not match.\r\n");
		retVal = true;
	}
	return retVal;
}

void CMainFrame::GetTitle(CString & title)
{
	title = CMDIFrameWndEx::GetTitle();
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_wndClientArea.ModifyStyleEx(WS_EX_CLIENTEDGE, 0);

	// create a view to occupy the client area of the frame
	//if (!m_wndView.Create(IDD_ABOUTBOX, this))
	//m_wndView.Create(GetSafeHwnd(), CRect(10,300,100,330), _T("My button"), WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON, NULL, 2);
	//if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	//{
	//	TRACE0("Failed to create view window\n");
	//	return -1;
	//}
	// set the visual manager and style based on persisted value
	OnApplicationLook(theApp.m_nAppLook);

	//// create a view to occupy the client area of the frame
	//if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	//{
	//	TRACE0("Failed to create view window\n");
	//	return -1;
	//}

	boost::filesystem::path iniPath;
	GetIniPath(iniPath);
	CComPtr<IConfig> ini = CreateIConfig(AMT_INI, iniPath);
	CString config1( ini->Get(g_szConfigSection, RepLabel1, _T("0")) );
	CString config2( ini->Get(g_szConfigSection, RepLabel2, _T("1")) );

	m_splitter.Create(m_hWndMDIClient, ATL::CWindow::rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_repositoryDlg.Create(m_splitter);

	//m_detailSheetView.SetReflectNotifications(true);
	m_detailSheetView.SetTabStyles(CTCS_BOTTOM | CTCS_TOOLTIPS);
	m_detailSheetView.Create(m_splitter, ATL::CWindow::rcDefault);
	//m_detailView.SetFont(WTL::AtlGetDefaultGuiFont());

	m_sourceEclView.Create(m_detailSheetView, ATL::CWindow::rcDefault);
	m_sourceEclView.DoInit();
	m_sourceEclView.SetReadOnly(true);
	//TODO:  InitEclCommandMixin(this, this, &m_sourceEclView, this);
	m_detailSheetView.AddTab(m_sourceEclView, _T("Source ECL"));

	m_diffView.Create(m_detailSheetView, ATL::CWindow::rcDefault, _T(""), WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, WS_EX_CLIENTEDGE);
	m_detailSheetView.AddTab(m_diffView, _T("Differences"));

	m_targetEclView.Create(m_detailSheetView, ATL::CWindow::rcDefault);
	m_targetEclView.DoInit();
	m_targetEclView.SetReadOnly(true);
	//TODO:  InitEclCommandMixin(this, this, &m_targetEclView, this);
	m_detailSheetView.AddTab(m_targetEclView, _T("Target ECL"));

	m_dependees.Create(m_detailSheetView, ATL::CWindow::rcDefault, NULL, WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, WS_EX_CLIENTEDGE);
	m_dependees.SetFont(WTL::AtlGetDefaultGuiFont());
	m_detailSheetView.AddTab(m_dependees, _T("Dependee Check"));

	m_logView.Create(m_detailSheetView, ATL::CWindow::rcDefault, NULL, WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VSCROLL | LBS_NOINTEGRALHEIGHT, WS_EX_CLIENTEDGE);
	m_logView.SetFont(WTL::AtlGetDefaultGuiFont());
	m_detailSheetView.AddTab(m_logView, _T("Log"));

	WTL::CEdit edit;
	edit.Create(m_detailSheetView, ATL::CWindow::rcDefault, NULL, ES_MULTILINE | ES_READONLY | WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_HSCROLL | WS_VSCROLL, WS_EX_CLIENTEDGE);

	CString face = _T("MS Shell Dlg");
	int nPointSize = 8;

	WTL::CFont font;
	WTL::CLogFont logFont;
	logFont.lfCharSet = DEFAULT_CHARSET;
	logFont.lfHeight = nPointSize*10;
	::_tcsncpy(logFont.lfFaceName, face, LF_FACESIZE);
	logFont.lfWeight = FW_LIGHT;
	logFont.lfQuality = PROOF_QUALITY;
	logFont.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
	logFont.lfOutPrecision = OUT_TT_PRECIS;
	if ( font.CreatePointFontIndirect(&logFont,0) )
	{
		edit.SetFont(font.Detach(),false);
	}
	m_warnView.SetEditCtrl(edit.Detach());

	m_detailSheetView.AddTab(m_warnView, _T("Warnings"));
	m_detailSheetView.GetTabCtrl().SetCurSel(1);

	m_splitter.SetSplitterPanes(m_repositoryDlg, m_detailSheetView);
	m_splitter.SetSplitterPosPct(50);

	m_wndRibbonBar.Create(this);
	InitializeRibbon();

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	m_wndStatusBar.AddElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE1, _T(""), TRUE), _T(""));
	m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE2, _T(""), TRUE), _T(""));
	CMFCRibbonProgressBar * elem = new CMFCRibbonProgressBar(ID_STATUSBAR_PANE3);
	m_wndStatusBar.AddExtendedElement(elem, _T("Progress"));
	elem->SetRange(1, 1);
	elem->SetPos(0);

	// enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode(DT_SMART);
	// enable Visual Studio 2005 style docking window auto-hide behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// Enable enhanced windows management dialog
	//EnableWindowsDialog(ID_WINDOW_MANAGER, IDS_WINDOWS_MANAGER, TRUE);

	HICON newIcon = LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME_AMT));
	HICON oldIcon = SetIcon(newIcon, false);
	HICON oldIcon2 = SetIcon(newIcon, true);

	RecalcLayout();
	if ( !m_bStreamInit )
	{
		m_pOldBuf = std::cerr.rdbuf(&m_EditStrBuf);
		m_pOldBufW = std::wcerr.rdbuf(&m_EditStrBufW);
		m_bStreamInit = true;
	}

	PostMessage(UM_INITIALIZE);
	return 0;
}


LRESULT CMainFrame::OnInitialize(WPARAM wParam, LPARAM lParam)
{
   UNREFERENCED_PARAMETER(wParam);
   UNREFERENCED_PARAMETER(lParam);

	//m_stateMgr.Initialize(regPath, m_hWnd, SW_SHOWMAXIMIZED);
	//m_stateMgr.Restore();

	DoLogout();
	DoLogin();

	//UISetCheck(ID_EDIT_SHOW_ORPHANS,1);
	//UISetCheck(ID_EDIT_SHOW_MATCHING,1);
	//UISetCheck(ID_EDIT_SHOW_NOT_MATCHING,1);

   return 0;
}

LRESULT CMainFrame::OnClearDiff(WPARAM wParam, LPARAM lParam)
{
	ShowDiff();
	return 0;
}

LRESULT CMainFrame::OnNewSelection(WPARAM wParam, LPARAM lParam)
{
	OnItemSelectionChanged((CTreeNode *)wParam, (bool)(lParam != 0));
	return 0;
}

LRESULT CMainFrame::OnNewCheck(WPARAM wParam, LPARAM lParam)
{
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

	if (wParam > 0)
	{
		if (text.length())
			m_wndStatusBar.RecalcLayout();
		m_wndStatusBar.RedrawWindow();
	}
	return 0;
}

LRESULT CMainFrame::OnProgressUpdate(WPARAM wParam, LPARAM lParam)
{

	CMFCRibbonProgressBar * elem = reinterpret_cast<CMFCRibbonProgressBar * >(m_wndStatusBar.GetExElement(1));
	if (lParam == 0)
	{
		elem->SetRange(1, 1);
		elem->SetPos(0);
	}
	else
	{
		int max = elem->GetRangeMax();
		if (max < lParam)
		{
			elem->SetRange(1, lParam);
		}
		elem->SetPos(max-lParam);
	}
	return 0;
}

void CMainFrame::OnClose()
{
	if (m_repositoryDlg.GetMigrationCount() > 0)
	{
		CString prog_title;
		prog_title.LoadString(IDR_MAINFRAME_AMT);
		switch(MessageBox(_T("Warning:  Files are currently being migrated in the background, are you sure you want to exit?"), prog_title, MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON1))
		{
		case IDYES:
			break;
		case IDNO:
			return;//bypass super call
		}
	}
	DoLogout();
	__super::OnClose();
}
LRESULT CMainFrame::OnGetEclLoaded(WPARAM wParam, LPARAM lParam)
{
	bool showHiddenChar = false;
	m_repositoryDlg.m_repositoryView->Invalidate();
	std::_tstring lhs, lhsHeader;
	std::_tstring rhs, rhsHeader;
	if (CComQIPtr<CAttributePairNode> attrNode = m_repositoryDlg.GetSelectedItem())
	{
		CComQIPtr<CAttributePairNode> attributeNode  = (CTreePairNode *)wParam;
		if (attrNode.p != attributeNode)
			return 0;

		if (attributeNode->hasLHS())
			showHiddenChar = GenerateHeader(attributeNode->m_lhs, attributeNode->m_rhs, lhsHeader);
		if (attributeNode->hasRHS())
			showHiddenChar = GenerateHeader(attributeNode->m_rhs, attributeNode->m_lhs, rhsHeader) ? true : showHiddenChar;

		lhs = attributeNode->m_lhs->GetText(false);
		rhs = attributeNode->m_rhs->GetText(false);
	}
	else if (CComQIPtr<CAttributeHistoryPairNode> histNode = m_repositoryDlg.GetSelectedItem())
	{
		CComQIPtr<CAttributeHistoryPairNode> historyNode = (CTreePairNode *)wParam;
		if (histNode.p != historyNode)
			return 0;

		CComQIPtr<CAttributePairNode> attributeNode = histNode->GetParentNode();

		lhsHeader = (boost::_tformat(_T("%1% (When:  %2%, By:  %3%)\r\n")) % attributeNode->m_lhs->GetQualifiedLabel() % historyNode->m_lhs->GetModifiedDate() % historyNode->m_lhs->GetModifiedBy()).str();
		if (historyNode->hasLHS())
			rhsHeader = (boost::_tformat(_T("%1% (crc:  %2%)\r\n")) % historyNode->m_rhs->GetQualifiedLabel() % historyNode->m_rhs->GetChecksum()).str();

		lhs = historyNode->m_lhs->GetText();
		rhs = historyNode->m_rhs->GetText(false);
	}
	if (lhs.length() || rhs.length() || lhsHeader.length() || rhsHeader.length())
	{
		ShowDiff(lhs, lhsHeader, rhs, rhsHeader);
	}
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}

void CMainFrame::InitializeRibbon()
{
	CString strTemp;
	strTemp.LoadString(IDS_RIBBON_FILE);

	// Load panel images:
	m_PanelImages.SetImageSize(CSize(16, 16));
	m_PanelImages.Load(IDB_BUTTONS);

	// Init main button:
	m_MainButton.SetImage(IDB_MAIN_AMT);
	m_MainButton.SetText(_T("\nf"));
	m_MainButton.SetToolTipText(strTemp);

	m_wndRibbonBar.SetApplicationButton(&m_MainButton, CSize (45, 45));
	CMFCRibbonMainPanel* pMainPanel = m_wndRibbonBar.AddMainCategory(strTemp, IDB_AMT_FILESMALL, IDB_AMT_FILELARGE);

	strTemp.LoadString(IDS_RIBBON_NEW);
	pMainPanel->Add(new CMFCRibbonButton(ID_EDIT_REFRESH, _T("Refresh\nr"), 25, 14));
	pMainPanel->Add(new CMFCRibbonSeparator(TRUE));

	strTemp.LoadString(IDS_RIBBON_PRINT);
	CMFCRibbonButton* pBtnPrint = new CMFCRibbonButton(ID_FILE_PRINT, strTemp, 6, 6);
	pBtnPrint->SetKeys(_T("p"), _T("w"));
	strTemp.LoadString(IDS_RIBBON_PRINT_LABEL);
	pBtnPrint->AddSubItem(new CMFCRibbonLabel(strTemp));
	strTemp.LoadString(IDS_RIBBON_PRINT);
	pBtnPrint->AddSubItem(new CMFCRibbonButton(ID_FILE_PRINT, strTemp, 7, 7, TRUE));
	strTemp.LoadString(IDS_RIBBON_PRINT_SETUP);
	pBtnPrint->AddSubItem(new CMFCRibbonButton(ID_FILE_PRINT_SETUP, strTemp, 11, 11, TRUE));
	pMainPanel->Add(pBtnPrint);

	pMainPanel->Add(new CMFCRibbonSeparator(TRUE));
	pMainPanel->Add(new CMFCRibbonButton(ID_FILE_LOGOUT, _T("&Logout"), 0, 0, TRUE));
	//pMainPanel->Add(new CMFCRibbonSeparator(TRUE));

	//strTemp.LoadString(IDS_RIBBON_CLOSE);
	//pMainPanel->Add(new CMFCRibbonButton(ID_FILE_CLOSE, strTemp, 9, 9));

	strTemp.LoadString(IDS_RIBBON_RECENT_DOCS);
	pMainPanel->AddRecentFilesList(strTemp);

	strTemp.LoadString(IDS_RIBBON_EXIT);
	pMainPanel->AddToBottom(new CMFCRibbonMainPanelButton(ID_APP_EXIT, strTemp, 15));

	// Add "Home" category with "Clipboard" panel:
	strTemp.LoadString(IDS_RIBBON_HOME);
	CMFCRibbonCategory* pCategoryHome = m_wndRibbonBar.AddCategory(strTemp, IDB_AMT_WRITESMALL, IDB_AMT_WRITELARGE);

	// Create "Clipboard" panel:
	strTemp.LoadString(IDS_RIBBON_CLIPBOARD);
	CMFCRibbonPanel* pPanelClipboard = pCategoryHome->AddPanel(strTemp, m_PanelImages.ExtractIcon(27));

	strTemp.LoadString(IDS_RIBBON_PASTE);
	CMFCRibbonButton* pBtnPaste = new CMFCRibbonButton(ID_EDIT_PASTE, strTemp, 0, 0);
	pPanelClipboard->Add(pBtnPaste);

	strTemp.LoadString(IDS_RIBBON_CUT);
	pPanelClipboard->Add(new CMFCRibbonButton(ID_EDIT_CUT, strTemp, 1));
	strTemp.LoadString(IDS_RIBBON_COPY);
	pPanelClipboard->Add(new CMFCRibbonButton(ID_EDIT_COPY, strTemp, 2));

	// Create "Filter" panel:
	CMFCRibbonPanel* pPanelFilter = pCategoryHome->AddPanel(_T("Filter"));
	pPanelFilter->Add(new CMFCRibbonButton(ID_EDIT_SHOW_ALL, _T("Show All\ns"), 16));
	pPanelFilter->AddSeparator();
	pPanelFilter->Add(new CMFCRibbonCheckBox(ID_EDIT_SHOW_ORPHANS, _T("Show Orphans\no")));
	pPanelFilter->Add(new CMFCRibbonCheckBox(ID_EDIT_SHOW_NOT_MATCHING, _T("Show Differences\nd")));
	pPanelFilter->Add(new CMFCRibbonCheckBox(ID_EDIT_SHOW_MATCHING, _T("Show Matching\nm")));

	CMFCRibbonPanel* pPanelSelection = pCategoryHome->AddPanel(_T("Selection"));
	strTemp.LoadString(IDS_RIBBON_SELECTALL);
	pPanelSelection->Add(new CMFCRibbonButton(ID_EDIT_SELECT_ALL, strTemp, 21));
	pPanelSelection->Add(new CMFCRibbonButton(ID_EDIT_CLEARALL, _T("Clear All\na"), 22));
	pPanelSelection->Add(new CMFCRibbonButton(ID_EDIT_EXPANDALL, _T("Expand All"), 23));
	pPanelSelection->AddSeparator();
	pPanelSelection->Add(new CMFCRibbonButton(ID_EDIT_ADDTOREP, _T("Migrate\nm"), 14));

	CMFCRibbonPanel* pPanelView = pCategoryHome->AddPanel(_T("View"));
	pPanelView->Add(new CMFCRibbonCheckBox(ID_VIEW_CRLF, _T("CR/LF")));
	pPanelView->Add(new CMFCRibbonCheckBox(ID_VIEW_WHITESPACE, _T("Whitespace")));

	CMFCRibbonPanel* pPanelDiferences = pCategoryHome->AddPanel(_T("Differences"));
	pPanelDiferences->Add(new CMFCRibbonButton(ID_DIFFERENCES_PREVIOUS, _T("Previous\nn")));
	pPanelDiferences->Add(new CMFCRibbonButton(ID_DIFFERENCES_NEXT, _T("Next\np")));

	CMFCRibbonPanel* pPanelDependees = pCategoryHome->AddPanel(_T("Dependees"));
	pPanelDependees->Add(new CMFCRibbonButton(ID_DEPENDEES_FINDSERVICES, _T("Find Services")));
	pPanelDependees->Add(new CMFCRibbonButton(ID_DEPENDEES_CALCULATEDEPENDENCIES, _T("Recalculate Cache")));
	pPanelDependees->Add(new CMFCRibbonButton(ID_DEPENDEES_CALCULATECROSSREFERENCE, _T("Calculate Crossreference")));
	pPanelDependees->AddSeparator();
	pPanelDependees->Add(new CMFCRibbonButton(ID_DEPENDEES_SYNCFROMSELECTION, _T("Sync From Selection")));
	pPanelDependees->Add(new CMFCRibbonButton(ID_DEPENDEES_SYNCTOSELECTION, _T("Sync To Selection")));
	pPanelDependees->Add(new CMFCRibbonButton(ID_DEPENDEES_COPYCROSSREFERENCE, _T("Copy Crossreference")));

	CMFCRibbonPanel* pPanelAdvanced = pCategoryHome->AddPanel(_T("Advanced"));
	pPanelAdvanced->Add(new CMFCRibbonCheckBox(ID_EDIT_MODULE_CHECKSUM, _T("Module Checksums\nu")));

	// Add elements to the right side of tabs:
	strTemp.LoadString(IDS_RIBBON_STYLE);
	CMFCRibbonButton* pVisualStyleButton = new CMFCRibbonButton(-1, strTemp, -1, -1);

	pVisualStyleButton->SetMenu(IDR_THEME_MENU, FALSE /* No default command */, TRUE /* Right align */);

	strTemp.LoadString(IDS_RIBBON_STYLE_TIP);
	pVisualStyleButton->SetToolTipText(strTemp);
	strTemp.LoadString(IDS_RIBBON_STYLE_DESC);
	pVisualStyleButton->SetDescription(strTemp);
	m_wndRibbonBar.AddToTabs(pVisualStyleButton);

	// Add quick access toolbar commands:
	CList<UINT, UINT> lstQATCmds;

	lstQATCmds.AddTail(ID_EDIT_REFRESH);
	lstQATCmds.AddTail(ID_FILE_LOGOUT);
	lstQATCmds.AddTail(ID_FILE_PRINT);

	m_wndRibbonBar.SetQuickAccessCommands(lstQATCmds);
	m_wndRibbonBar.AddToTabs(new CMFCRibbonButton(ID_APP_ABOUT, _T("\na"), m_PanelImages.ExtractIcon(0)));
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

void CMainFrame::OnFileLogout()
{
	DoLogout();
	DoLogin();
}

void CMainFrame::OnEditCopy()
{
	m_repositoryDlg.DoCopy();
}

void CMainFrame::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_repositoryDlg.m_repositoryView->CanCopy());
}

void CMainFrame::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_repositoryDlg.m_repositoryView->CanPaste());
}

void CMainFrame::OnUpdateEditAddToRep(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_repositoryDlg.m_comboRHS->m_value.IsEmpty() && m_repositoryDlg.m_repositoryView->CanCopy());
}

void CMainFrame::OnUpdateEditClearAll(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_repositoryDlg.m_repositoryView->CanCopy());
}

void CMainFrame::OnEditPaste()
{
	std::_tstring attrs;
	GetClipboardText(attrs);
	m_repositoryDlg.DoPaste(attrs);
}

void CMainFrame::OnEditAddToRep()
{
	m_repositoryDlg.DoMigration();
}

void CMainFrame::OnEditShow()
{
	int show = AMT_SHOW_NONE;

	if (m_bCheckBoxMatching)
		show |= AMT_SHOW_MATCHING;

	if (m_bCheckBoxNotMatching)
		show |= AMT_SHOW_NOT_MATCHING;

	if (m_bCheckBoxOrphans)
		show |= AMT_SHOW_ORPHANS;

	if (m_repositoryDlg.SetFilter(show))
		ShowDiff();
}

void CMainFrame::OnEditShowMatching()
{
	m_bCheckBoxMatching = !m_bCheckBoxMatching;
	OnEditShow();
}
void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// forward focus to the view window
	m_splitter.SetFocus();
}

void CMainFrame::OnUpdateEditShowMatching(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bCheckBoxMatching);	
}

void CMainFrame::OnEditShowNotMatching()
{
	m_bCheckBoxNotMatching = !m_bCheckBoxNotMatching;
	OnEditShow();
}

void CMainFrame::OnUpdateEditShowNotMatching(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bCheckBoxNotMatching);	
}

void CMainFrame::OnEditShowOrphans()
{
	m_bCheckBoxOrphans = !m_bCheckBoxOrphans;
	OnEditShow();
}

void CMainFrame::OnUpdateEditShowOrphans(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck (m_bCheckBoxOrphans);	
}

void CMainFrame::OnEditShowAll()
{
	m_bCheckBoxMatching = true;
	m_bCheckBoxNotMatching = true;
	m_bCheckBoxOrphans = true;
	OnEditShow();
}

void CMainFrame::OnEditSelectAll()
{
	m_repositoryDlg.SelectAll();
}

void CMainFrame::OnEditClearAll()
{
	m_repositoryDlg.ClearSelection();
}

void CMainFrame::OnEditExpandAll()
{
	m_repositoryDlg.ExpandAll();
}

void CMainFrame::OnEditRefresh()
{
	ShowDiff();
	m_repositoryDlg.Clear();
	ClearRepositorySingletons(true);
	m_repositoryDlg.Load(NULL, false);
	m_repositoryDlg.ClearSelection();
}

void CMainFrame::OnViewCRLF()
{
	m_bCheckBoxCRLF = !m_bCheckBoxCRLF;
	m_diffView.SetViewEOL(m_bCheckBoxCRLF != FALSE);
	m_sourceEclView.SetViewEOL(m_bCheckBoxCRLF);
	m_targetEclView.SetViewEOL(m_bCheckBoxCRLF);
}

void CMainFrame::OnUpdateViewCRLF(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bCheckBoxCRLF);	
}

void CMainFrame::OnViewWhitespace()
{
	m_bCheckBoxWhitespace = !m_bCheckBoxWhitespace;
	m_diffView.SetViewWS(m_bCheckBoxWhitespace != FALSE);
	m_sourceEclView.SetViewWS(m_bCheckBoxWhitespace);
	m_targetEclView.SetViewWS(m_bCheckBoxWhitespace);
}

void CMainFrame::OnUpdateViewWhitespace(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bCheckBoxWhitespace);	
}

void CMainFrame::OnDiffPrevious()
{
	m_diffView.GotoDiffPrevious();
}

void CMainFrame::OnUpdateDiffPrevious(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_diffView.GetDiffPrevious() >= 0);
}

void CMainFrame::OnDiffNext()
{
	m_diffView.GotoDiffNext();
}

void CMainFrame::OnUpdateDiffNext(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_diffView.GetDiffNext() >= 0);
}

void CMainFrame::OnFindServices()
{
	IAttributeVector repAttrs;
	std::_tstring changedSince;
	m_repositoryDlg.m_repositoryView->m_repRHS->FindAttributes(_T("--SOAP--"), _T(""), _T(""), false, false, false, false, false, changedSince.c_str(), repAttrs);
	std::_tstring items;
	for (IAttributeVector::const_iterator itr = repAttrs.begin(); itr != repAttrs.end(); ++itr)
	{
		if (boost::algorithm::starts_with(itr->get()->GetQualifiedLabel(), _T("Trash")) ||
			boost::algorithm::starts_with(itr->get()->GetQualifiedLabel(), _T("zz_")))
			continue;
		if (!items.empty())
			items += _T("\r\n");
		items += itr->get()->GetQualifiedLabel();
	}

	repAttrs.clear();
	m_repositoryDlg.m_repositoryView->m_repRHS->FindAttributes(_T("--LIBRARY--"), _T(""), _T(""), false, false, false, false, false, changedSince.c_str(), repAttrs);
	for (IAttributeVector::const_iterator itr = repAttrs.begin(); itr != repAttrs.end(); ++itr)
	{
		if (boost::algorithm::starts_with(itr->get()->GetQualifiedLabel(), _T("Trash")) ||
			boost::algorithm::starts_with(itr->get()->GetQualifiedLabel(), _T("zz_")))
			continue;
		if (!items.empty())
			items += _T("\r\n");
		items += itr->get()->GetQualifiedLabel();
	}

	m_dependees.SetServices(items);
}

void CMainFrame::OnUpdateFindServices(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_daliRHS.m_dali != NULL);
}

void CMainFrame::OnCalcDependencies()
{
	if (m_daliRHS.m_dali)
	{
		Dali::CEclExceptionVector errorResults2;
		std::_tstring items;
		m_dependees.GetServices(items, _T(","));
		std::_tstring xmlDepends;
		m_daliRHS.m_dali->GetDependanciesTree(m_daliRHS.cluster, m_daliRHS.queue, _T(""), items, xmlDepends, errorResults2);
		m_dependees.SetDependencies(xmlDepends);
	}
}

void CMainFrame::OnUpdateCalcDependencies(CCmdUI* pCmdUI)
{
	if (m_dependees.HasDependency())
		pCmdUI->SetText(_T("Recalculate Cache"));
	else
		pCmdUI->SetText(_T("Calculate Cache"));
	pCmdUI->Enable(m_daliRHS.m_dali != NULL && m_dependees.HasServices());
}

void CMainFrame::OnCalcCrossreferences()
{
	m_dependees.CalcCrossreferences();
}

void CMainFrame::OnUpdateCalcCrossreferences(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_dependees.HasDependency());
}

void CMainFrame::OnSyncFromSelection()
{
	std::_tstring attrs;
	m_repositoryDlg.m_repositoryView->m_sel.GetSelection(attrs);
	m_dependees.SetAttributes(attrs);
}

void CMainFrame::OnUpdateSyncFromSelection(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_repositoryDlg.m_repositoryView->CanCopy());
}

void CMainFrame::OnSyncToSelection()
{
	std::_tstring attrs;
	m_dependees.GetAttributes(attrs, _T("\r\n"));
	m_repositoryDlg.DoPaste(attrs);
}

void CMainFrame::OnUpdateSyncToSelection(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_dependees.HasAttributes());
}

void CMainFrame::OnCopyCrossreference()
{
	std::_tstring xref;
	SetClipboard(m_dependees.GetCrossreferenced(xref));
}

void CMainFrame::OnUpdateCopyCrossreference(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_dependees.HasCrossreferenced());
}

void CMainFrame::OnEditModuleChecksum()
{
	m_bCheckBoxModuleChecksum = !m_bCheckBoxModuleChecksum;
	m_repositoryDlg.SupressModuleChecksum(!m_bCheckBoxModuleChecksum);
	OnEditRefresh();
}

void CMainFrame::OnUpdateEditModuleChecksum(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck (m_bCheckBoxModuleChecksum);	
}

void CMainFrame::OnHelpClientTools()
{
	boost::filesystem::path folder;
	GetProgramFolder(folder);
	boost::filesystem::path file = folder / boost::filesystem::path("ClientTools.pdf", boost::filesystem::native);
	::ShellExecute(m_hWnd, _T("open"), pathToWString(file).c_str(), 0, pathToWString(folder).c_str(), SW_MAXIMIZE);
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	//if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
	//	return TRUE;

	// otherwise, do default handling
	return CMDIFrameWndEx::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
//	if (m_detailSheetView.PreTranslateMessage(pMsg))
//		return TRUE;
	if (CMDIFrameWndEx::PreTranslateMessage(pMsg))
		return TRUE;
	return FALSE;
}

void CMainFrame::OnApplicationLook(UINT id)
{
	WTL::CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

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
	}

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}
//  ===========================================================================
FindReplace& CMainFrame::GetFindReplace()
{
	return m_fr;
}

void CMainFrame::FindReplaceOnEditFind(const std::_tstring &findWhat, bool enableWildcardRegex, bool enableTooltip)
{
	//OnEditFind(findWhat);
}

void CMainFrame::FindReplaceOnEditReplace(const std::_tstring &replaceWhat)
{
	//OnEditReplace(replaceWhat);
}


void PostStatus(const TCHAR* pStr, AMTPANE pane)
{
	if ( !pStr )
		g_MainFrame->PostMessage(CWM_STATUSUPDATE, pane, 0);
	else
		g_MainFrame->PostMessage(CWM_STATUSUPDATE, pane, LPARAM(new CString(pStr)));
}

void PostProgress(int progress)
{
	g_MainFrame->PostMessage(UM_PROGRESSUPDATE, 0, progress);
}

FindReplace& GetFindReplace()
{
	return g_MainFrame->m_fr;
}

void FindReplaceOnEditFind(const std::_tstring &findWhat, bool enableWildcardRegex, bool enableTooltip)
{
	//g_MainFrame->OnEditFind(findWhat);
}

void FindReplaceOnEditReplace(const std::_tstring &findWhat)
{
}

BOOL PostFrameMessage(UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if (g_MainFrame)
		return g_MainFrame->PostMessage(Msg, wParam, lParam);
	return FALSE;
}

LRESULT SendFrameMessage(UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if (g_MainFrame)
		return g_MainFrame->SendMessage(Msg, wParam, lParam);
	return 0;
}
