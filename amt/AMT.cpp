// AMT.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "AMT.h"
#include "MainFrm2.h"

#include <global.h>

const TCHAR *g_szConfigSection = _T("Configs");
//const TCHAR *g_Workspace = _T("QueryBuilder");

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif

BEGIN_MESSAGE_MAP(CAMTApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CAMTApp::OnAppAbout)
END_MESSAGE_MAP()


//extern int AFXAPI AfxWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, int nCmdShow);
//int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpstrCmdLine, int nCmdShow)
//{
//	int retVal = AfxWinMain(hInstance, hPrevInstance, lpstrCmdLine, nCmdShow);
//	return retVal;
//}

CAMTApp::CAMTApp()
{
	// afxMemDF = 0;
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only CAMTApp object

CAMTApp theApp;
WTL::CAppModule _Module;

// CAMTApp initialization

BOOL CAMTApp::InitInstance()
{
	HRESULT hRes = _Module.Init(NULL, m_hInstance);
	ATLASSERT(SUCCEEDED(hRes));

#ifndef _DEBUG
	// Setup exception handler
	BT_SetAppName(_T("Attribute Migration Tool"));
	BT_SetSupportEMail(_T("gsmith@seisint.com"));
	BT_SetFlags(BTF_DETAILEDMODE | BTF_EDITMAIL | BTF_ATTACHREPORT);
    BT_SetSupportURL(_T("http://hpccsystems.com/support"));


	// = BugTrapServer ===========================================
	//BT_SetSupportServer(_T("localhost"), 9999);
	// - or -
	//BT_SetSupportServer(_T("127.0.0.1"), 9999);

	// = BugTrapWebServer ========================================
	//BT_SetSupportServer(_T("http://localhost/BugTrapWebServer/RequestHandler.aspx"), BUGTRAP_HTTP_PORT);

	// required since VS 2005 SP1
	BT_InstallSehFilter();
#endif

	::LoadLibrary(_T("sgrid.dll"));
	HINSTANCE hInstanceResources = ::LoadLibrary(_T("en_us.dll"));
	_Module.SetResourceInstance(hInstanceResources);
	AfxSetResourceHandle(hInstanceResources);

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetApplicationName(_T("eclide.exe"));	//AMT should share the same cfg folder as QB
	boost::filesystem::path iniPath;
	GetIniPath(iniPath);
	CComPtr<IConfig> ini = CreateIConfig(AMT_INI, iniPath);
	CComPtr<IConfig> cfgLHS = CreateIConfig(AMT_CFG_LHS);
	CComPtr<IConfig> cfgRHS = CreateIConfig(AMT_CFG_RHS);

	std::_tstring majorVersion;
	GetAppMajorVersion(majorVersion);

	SetRegistryKey((boost::_tformat(_T("HPCC Systems\\Version%1%")) % majorVersion).str().c_str());
	LoadStdProfileSettings(0);  // Load standard INI file options

	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL, RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object
	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	// create and load the frame with its resources
	if (!pFrame->LoadFrame(IDR_MAINFRAME_AMT))
		return FALSE;
	// try to load shared MDI menus and accelerator table
	//TODO: add additional member variables and load calls for
	//	additional menu types your application may need
	//HINSTANCE hInst = AfxGetResourceHandle();
	//m_hMDIMenu  = ::LoadMenu(hInst, MAKEINTRESOURCE(IDR_QueryBuilderTYPE));
	//m_hMDIAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_QueryBuilderTYPE));


	// The one and only window has been initialized, so show and update it
	pFrame->ShowWindow(m_nCmdShow);
	pFrame->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	return TRUE;
}


// CAMTApp message handlers

int CAMTApp::ExitInstance() 
{
    // Terminate ATL
    _Module.Term();
	return CWinAppEx::ExitInstance();
}

// App command to run the dialog
void CAMTApp::OnAppAbout()
{
	DoModalAbout(_T("Attribute Migration Tool"), IDR_MAINFRAME_AMT);
}

// CAMTApp customization load/save methods

void CAMTApp::PreLoadState()
{
	CString strName;
	strName.LoadString(IDS_EDIT_MENU);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
}

void CAMTApp::LoadCustomState()
{
}

void CAMTApp::SaveCustomState()
{
}

// CAMTApp message handlers



