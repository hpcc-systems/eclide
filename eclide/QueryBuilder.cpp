// QueryBuilder.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "..\en_us\resource.h"

#include "afxwinappex.h"
#include "QueryBuilder.h"
#include "MainFrm.h"

//#include "ChildFrm.h"
#include <global.h>
#include "..\sgrid\atlsgrid.h"
#include "preferencedlg.h"
#include "aboutdlg.h"
#include <EclCC.h>

#include "include/cef_base.h"
#include "include/cef_app.h" 

class CefMfcCefApp : public CefApp, public CefBrowserProcessHandler {
public:
    CefMfcCefApp() {}

    virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() OVERRIDE {
        return this;
    }

private:
    IMPLEMENT_REFCOUNTING(CefMfcCefApp);
};

class CConfigCache 
{
public:
    CConfigCache()
    {
    }
    ~CConfigCache()
    {
        int d = 0;
    }
} test;

//HMODULE hScintilla = 0;

// CQueryBuilderApp

BEGIN_MESSAGE_MAP(CQueryBuilderApp, CWinAppEx)
    ON_COMMAND(ID_APP_ABOUT, &CQueryBuilderApp::OnAppAbout)
END_MESSAGE_MAP()


// CQueryBuilderApp construction

CQueryBuilderApp::CQueryBuilderApp()
{
    m_bHiColorIcons = TRUE;

    // TODO: add construction code here,
    // Place all significant initialization in InitInstance
}

// The one and only CQueryBuilderApp object

CQueryBuilderApp theApp;
WTL::CAppModule _Module;
HMODULE hGrid = 0;

// CQueryBuilderApp initialization

BOOL CQueryBuilderApp::PumpMessage()
{
    auto result = CWinAppEx::PumpMessage();

    // If there are other messages on queue then return right away
    // otherwise CEF has a habit of eating keystrokes not meant for it
    MSG msg;
    if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
        return result;

    // Allow Cef to do his thing
    CefDoMessageLoopWork();

    return result;
}

BOOL CQueryBuilderApp::InitInstance()
{
    HRESULT hRes = _Module.Init(NULL, m_hInstance);
    ATLASSERT(SUCCEEDED(hRes));

#ifndef _DEBUG
    // Setup exception handler
    BT_SetAppName(_T("ECL IDE"));
    BT_SetSupportEMail(_T("support_eclide@hpccsystems.com"));
    BT_SetFlags(BTF_DETAILEDMODE | BTF_EDITMAIL | BTF_ATTACHREPORT | BTF_SHOWADVANCEDUI);
    BT_SetSupportURL(_T("https://hpccsystems.com/community"));

    // = BugTrapServer ===========================================
    //BT_SetSupportServer(_T("localhost"), 9999);
    // - or -
    //BT_SetSupportServer(_T("127.0.0.1"), 9999);

    // = BugTrapWebServer ========================================
    //BT_SetSupportServer(_T("http://localhost/BugTrapWebServer/RequestHandler.aspx"), BUGTRAP_HTTP_PORT);

    // required since VS 2005 SP1
    BT_InstallSehFilter();
#endif
    // InitCommonControlsEx() is required on Windows XP if an application
    // manifest specifies use of ComCtl32.dll version 6 or later to enable
    // visual styles.  Otherwise, any window creation will fail.
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    // Set this to include all the common control classes you want to use
    // in your application.
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    if (!InitializeCef())
        return FALSE;

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
    HINSTANCE hInstanceResources = ::LoadLibrary(_T("en_us.dll"));
    _Module.SetResourceInstance(hInstanceResources);
    AfxSetResourceHandle(hInstanceResources);

    SetApplicationName(_T("eclide.exe"));
    boost::filesystem::path iniPath;
    GetIniPath(iniPath);
    CComPtr<IConfig> ini = CreateIConfig(QUERYBUILDER_INI, iniPath);
    CComPtr<IConfig> config = CreateIConfig(QUERYBUILDER_CFG);

    if (!hGrid)
        hGrid = ::LoadLibrary(SGRID::CGridCtrl::GetLibraryName());

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
    theApp.EnableTaskbarInteraction(false);

    //const TCHAR * path = _tgetenv(_T("PATH"));
    //if (path)
    //{
    //	const TCHAR * hpccbin = _tgetenv(_T("HPCCBIN"));
    //	if (hpccbin)
    //	{
    //		boost::filesystem::wpath eclccPath = hpccbin;
    //		eclccPath /= _T("eclcc.exe");
    //		if (clib::filesystem::exists(eclccPath))
    //		{
    //			std::_tstring envVar = _T("PATH=");
    //			envVar += hpccbin;
    //			envVar += _T(";");
    //			envVar += path;
    //			_tputenv(envVar.c_str());
    //			ini->Set(GLOBAL_COMPILER_LOCATION, eclccPath.native_file_string());
    //		}

    //		boost::filesystem::wpath stdLibPath = hpccbin;
    //		stdLibPath /= _T("ecllib");
    //		if (clib::filesystem::exists(stdLibPath))
    //		{
    //			ini->Set(GLOBAL_COMPILER_STDLIB, stdLibPath.native_file_string());
    //		}
    //	}
    //}

    // To create the main window, this code creates a new frame window
    // object and then sets it as the application's main window object
    CMDIFrameWnd* pFrame = new CMainFrame;
    if (!pFrame)
        return FALSE;
    m_pMainWnd = pFrame;
    // create main MDI frame window
    if (!pFrame->LoadFrame(IDR_MAINFRAME))
        return FALSE;
    // try to load shared MDI menus and accelerator table
    //TODO: add additional member variables and load calls for
    //	additional menu types your application may need
    //HINSTANCE hInst = AfxGetResourceHandle();
    //m_hMDIMenu  = ::LoadMenu(hInst, MAKEINTRESOURCE(IDR_QueryBuilderTYPE));
    //m_hMDIAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_QueryBuilderTYPE));

    // The main window has been initialized, so show and update it
    pFrame->ShowWindow(m_nCmdShow);
    pFrame->UpdateWindow();

    return TRUE;
}

int CQueryBuilderApp::ExitInstance() 
{
#ifdef _DEBUG
    AfxEnableMemoryLeakDump(false);
#endif
    //TODO: handle additional resources you may have added
    if (m_hMDIMenu != NULL)
        FreeResource(m_hMDIMenu);
    if (m_hMDIAccel != NULL)
        FreeResource(m_hMDIAccel);

    if (hGrid)
        ::FreeLibrary(hGrid);

    // Terminate ATL
    _Module.Term();

    UninitializeCef();

    ResetIConfigs();

    int retVal = CWinAppEx::ExitInstance();

    return retVal;
}

// App command to run the dialog
void CQueryBuilderApp::OnAppAbout()
{
    DoModalAbout((const TCHAR *)CString(MAKEINTRESOURCE(IDR_MAINFRAME)), IDR_MAINFRAME);
}

// CQueryBuilderApp customization load/save methods

void CQueryBuilderApp::PreLoadState()
{
    CString strName;
    strName.LoadString(IDS_EDIT_MENU);
    GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
    strName.LoadString(IDS_EXPLORER);
    GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
}

void CQueryBuilderApp::LoadCustomState()
{
}

void CQueryBuilderApp::SaveCustomState()
{
}

bool CQueryBuilderApp::InitializeCef()
{
    m_cefApp = new CefMfcCefApp();
    CefMainArgs mainargs(m_hInstance);

    const auto exit_code = CefExecuteProcess(mainargs, m_cefApp.get(), nullptr);
    if (exit_code >= 0)
        return false;

    CefSettings settings;
    settings.multi_threaded_message_loop = false;
    settings.no_sandbox = true;
    settings.ignore_certificate_errors = true;
    return CefInitialize(mainargs, settings, m_cefApp.get(), nullptr);
}

void CQueryBuilderApp::UninitializeCef()
{
    CefShutdown();
}

