
#include "StdAfx.h"
#include "resource.h"

#include "MDIFrame.h"
#include "ChildFrame.h"
#include "ResultView.h"
#include "MainFrm.h"
#include "htmlview.h"

//  ======================================================================================================================================================
enum MDI_TYPE
{
	MDI_TYPE_RESULT,
	MDI_TYPE_ECLWATCH,
	MDI_TYPE_COMPARE,
	MDI_TYPE_GRAPH
};

typedef std::pair<MDI_TYPE, CString> WinID;

std::map<WinID, HWND> g_window;
//  ======================================================================================================================================================

template<typename T>
class CChildFrameT : public CTabbedMDIChildWindowImpl<CChildFrameT<T>, CMDIWindow, CMDIChildWinTraits>, 
	//class CChildFrame : public CMDIChildWindowImpl<CChildFrame>,
	public CIdleHandler
{
	typedef CTabbedMDIChildWindowImpl<CChildFrameT, CMDIWindow, CMDIChildWinTraits> baseClass;
	//	typedef CMDIChildWindowImpl<CChildFrame> baseClass;
	typedef CChildFrameT thisClass;

public:
	DECLARE_FRAME_WND_CLASS(NULL, IDR_MDICHILD)

	void OnFinalMessage(HWND /*hWnd*/)
	{
		T *pT = static_cast<T*>(this);
		g_window[pT->ID] = 0;
		delete this;
	}

	BOOL OnIdle()
	{
		T *pT = static_cast<T*>(this);
		pT->UIUpdateMenuItems();
		return FALSE;
	}

	BEGIN_MSG_MAP(thisClass)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		CHAIN_MSG_MAP(baseClass)
	END_MSG_MAP()

	LRESULT OnCreate(LPCREATESTRUCT lParam)
	{
		SetMsgHandled(false);
		T *pT = static_cast<T*>(this);
		g_window[pT->ID] = m_hWnd;

		pT->DoCreateView();
		ATLASSERT(m_hWndClient);

		CMessageLoop * pLoop = _Module.GetMessageLoop();
		ATLASSERT(NULL != pLoop);
		pLoop->AddIdleHandler(this);

		UpdateTitle();

		return 0;
	}

	LRESULT OnNotify(int wParam, LPNMHDR lParam)
	{
		SetMsgHandled(false);
		return 0;
	}

	void OnDestroy()
	{
		SetMsgHandled(false);
		CMessageLoop * pLoop = _Module.GetMessageLoop();
		ATLASSERT(NULL != pLoop);
		pLoop->RemoveIdleHandler(this);		
	}

	//  Helpers  ---

	void UpdateTitle()
	{
		T *pT = static_cast<T*>(this);
		SetTitle(pT->GetTitle());
	}

	void ShowLastError(const CString & sMessage, DWORD dwError)
	{
		ATLTRACE( _T("Entering : ShowLastError ( %s, 0x%x )\n" ), sMessage, dwError);

		CString sErrorMessage;
		sErrorMessage = sMessage;

		// get the error message
		LPTSTR lpMessageBuffer;
		::FormatMessage ( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwError, MAKELANGID ( LANG_ENGLISH, SUBLANG_ENGLISH_US ), (LPTSTR)&lpMessageBuffer, 0, NULL);
		sErrorMessage += lpMessageBuffer;
		::LocalFree ( lpMessageBuffer ); // FormatMessage() requires this

		// display the error message
		::MessageBox ( NULL, sErrorMessage, QUERY_BUILDER, MB_OK | MB_ICONEXCLAMATION );	
	}

	//	void SetTitle(const CString & txt) { };
};

//  ======================================================================================================================================================
template<typename T, MDI_TYPE T2>
void OpenSingletonMDI(const CString & wuid, const CString &url, HWND hWndClient)
{
	CWindow win = g_window[WinID(T2, wuid)];
	if (win.IsWindow())
	{
		if (win.IsIconic())
			win.ShowWindow(SW_RESTORE);
		win.BringWindowToTop();
		return;
	}

	T* pChild = new T(pMenuManager, wuid, url);
	g_window[WinID(T2, wuid)] = pChild->CreateEx(hWndClient, NULL, NULL, GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_OPENMDICHILDMAX) ? WS_MAXIMIZE : 0);
}

template<typename T, MDI_TYPE T2>
void OpenSingletonMDI(const CString & wuid, HWND hWndClient)
{
	CWindow win = g_window[WinID(T2, wuid)];
	if (win.IsWindow())
	{
		if (win.IsIconic())
			win.ShowWindow(SW_RESTORE);
		win.BringWindowToTop();
		return;
	}

	StlLinked<IDali> dali = AttachDali(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT), _T("Dali"));
	CComPtr<IWorkunit> wu = dali->GetWorkunit(wuid);
	if (wu)
	{
		T* pChild = new T(pMenuManager, wu);
		g_window[WinID(T2, wuid)] = pChild->CreateEx(hWndClient, NULL, NULL, GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_OPENMDICHILDMAX) ? WS_MAXIMIZE : 0);
	}
}
//  ======================================================================================================================================================
static _ATL_FUNC_INFO TitleChangeInfo = {CC_STDCALL, VT_EMPTY, 1, { VT_BSTR } };
static _ATL_FUNC_INFO CommandStateChangeInfo = {CC_STDCALL, VT_EMPTY, 2, { VT_I4, VT_BOOL } };
static _ATL_FUNC_INFO BeforeNavigate2Info = {CC_STDCALL, VT_EMPTY, 7, { VT_DISPATCH, (VT_BYREF | VT_VARIANT), (VT_BYREF | VT_VARIANT), (VT_BYREF | VT_VARIANT), (VT_BYREF | VT_VARIANT), (VT_BYREF | VT_VARIANT), (VT_BYREF | VT_BOOL)}};
static _ATL_FUNC_INFO NavigateComplete2Info = {CC_STDCALL, VT_EMPTY, 2, { VT_DISPATCH, (VT_BYREF | VT_VARIANT)}};
static _ATL_FUNC_INFO DocumentCompleteInfo = {CC_STDCALL, VT_EMPTY, 2, { VT_DISPATCH, (VT_BYREF | VT_VARIANT) } };

const int ID_VIEW_SOURCE = WM_USER;
class CHtmlFrame :
	public CTabbedFrameImpl<CHtmlFrame, CDotNetButtonTabCtrl<CTabViewTabItem>, CTabbedMDIChildWindowImpl<CHtmlFrame, CMDIWindow, CMDIMaxChildWinTraits> >,
	public IDispEventSimpleImpl<0, CHtmlFrame, &DIID_DWebBrowserEvents2>,
	public CIdleHandler
{
	typedef CTabbedFrameImpl<CHtmlFrame, CDotNetButtonTabCtrl<CTabViewTabItem>, CTabbedMDIChildWindowImpl<CHtmlFrame, CMDIWindow, CMDIMaxChildWinTraits> > baseClass;

protected:
	CHtmlView m_view;

public:
	WinID ID;

	DECLARE_FRAME_WND_CLASS(NULL, IDR_MDICHILD)

	CTabbedMDICommandBarCtrl* m_pCmdBar;

	CPlainTextView m_HtmlSourceView;

	VARIANT_BOOL m_EnableBack;
	VARIANT_BOOL m_EnableForward;

	int m_nHtmlViewTabIndex;
	int m_nHtmlSourceViewTabIndex;
	CString m_Title;
	CString m_Url;

	CComPtr<IUnknown> m_punkBrowser;
	CComPtr<IWebBrowser2> m_spBrowser;

	CHtmlFrame(const CString & pTitle, const CString & pUrl) : ID(MDI_TYPE_ECLWATCH, pTitle),
		m_pCmdBar(NULL),
		m_nHtmlViewTabIndex(-1),
		m_nHtmlSourceViewTabIndex(-1)
	{
		m_Title = pTitle;
		m_Url = pUrl;

		m_EnableBack = false;
		m_EnableForward = false;

	}

	virtual void OnFinalMessage(HWND /*hWnd*/)
	{
		delete this;
	}

	void SetCommandBarCtrlForContextMenu(CTabbedMDICommandBarCtrl* pCmdBar)
	{
		m_pCmdBar = pCmdBar;
	}

	BEGIN_MSG_MAP(CHtmlFrame)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_SETTINGCHANGE, OnSettingChange)
		MESSAGE_HANDLER(WM_FORWARDMSG, OnForwardMsg)
		MESSAGE_HANDLER(UWM_MDICHILDSHOWTABCONTEXTMENU, OnShowTabContextMenu)
		COMMAND_ID_HANDLER(ID_FILE_SAVE, OnFileSave)
		COMMAND_ID_HANDLER(ID_FILE_CLOSE, OnFileClose)

		COMMAND_ID_HANDLER(ID_BROWSER_NEW, OnBrowserNew)
		COMMAND_ID_HANDLER(ID_BROWSER_BACK, OnBrowserBack)
		COMMAND_ID_HANDLER(ID_BROWSER_FORWARD, OnBrowserForward)
		COMMAND_ID_HANDLER(ID_BROWSER_STOP, OnBrowserStop)
		COMMAND_ID_HANDLER(ID_BROWSER_REFRESH, OnBrowserRefresh)

		COMMAND_ID_HANDLER(ID_VIEW_SOURCE, OnViewSource)

		CHAIN_MSG_MAP(baseClass)
	END_MSG_MAP()

	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		DefWindowProc(uMsg, wParam, lParam);
		bHandled = TRUE;

		CreateTabWindow(m_hWnd, rcDefault, (CTCS_BOTTOM | CTCS_TOOLTIPS | CTCS_HOTTRACK));

		m_view.Create(m_hWnd, rcDefault, m_Url, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL, WS_EX_CLIENTEDGE);
		m_HtmlSourceView.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_NOHIDESEL, WS_EX_CLIENTEDGE);

		m_nHtmlViewTabIndex = this->AddTab(m_view,       _T("HTML"));
		m_nHtmlSourceViewTabIndex =  this->AddTab(m_HtmlSourceView, _T("Source"));

		this->UpdateTabToolTip(m_view, _T("View the Rendered HTML"));
		this->UpdateTabToolTip(m_HtmlSourceView, _T("View the HTML Source"));

		m_view.get_Control(&m_punkBrowser);
		if(m_punkBrowser)
		{
			DispEventAdvise(m_punkBrowser, &DIID_DWebBrowserEvents2);
		}

		m_view.get_Browser(&m_spBrowser);

		baseClass::SetTitle(m_Title);

		CMessageLoop * pLoop = _Module.GetMessageLoop();
		ATLASSERT(NULL != pLoop);
		pLoop->AddIdleHandler(this);

		return 0;
	}

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		CMessageLoop * pLoop = _Module.GetMessageLoop();
		ATLASSERT(NULL != pLoop);
		pLoop->RemoveIdleHandler(this);		

		if(m_punkBrowser)
			DispEventUnadvise(m_punkBrowser, &DIID_DWebBrowserEvents2);

		// Let anybody else see this that wants to
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnSettingChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		// System settings or metrics have changed.  Propogate this message
		// to all the child windows so they can update themselves as appropriate.
		this->SendMessageToDescendants(uMsg, wParam, lParam, TRUE);

		return 0;
	}

	LRESULT OnForwardMsg(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		LPMSG pMsg = (LPMSG)lParam;

		if(baseClass::PreTranslateMessage(pMsg))
			return TRUE;

		int nActiveTab = this->GetTabCtrl().GetCurSel();
		if(nActiveTab == m_nHtmlViewTabIndex)
		{
			return m_view.PreTranslateMessage(pMsg);
		}
		else if(nActiveTab == m_nHtmlSourceViewTabIndex)
		{
			return m_HtmlSourceView.PreTranslateMessage(pMsg);
		}

		return FALSE;
	}

	LRESULT OnShowTabContextMenu(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = TRUE;

		POINT ptPopup = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};

		// Build up the menu to show
		CMenu mnuContext;

		// Load from resource
		//mnuContext.LoadMenu(IDR_CONTEXT);

		// or build dynamically
		// (being sure to enable/disable menu items as appropriate,
		// and giving the appropriate IDs)
		if(mnuContext.CreatePopupMenu())
		{
			int cchWindowText = this->GetWindowTextLength();
			CString sWindowText;
			this->GetWindowText(sWindowText.GetBuffer(cchWindowText+1), cchWindowText+1);
			sWindowText.ReleaseBuffer();

			CString sSave(_T("&Save '"));
			sSave += sWindowText;
			sSave += _T("'");

			mnuContext.AppendMenu((MF_ENABLED | MF_STRING), ID_FILE_SAVE, sSave);
			mnuContext.AppendMenu((MF_ENABLED | MF_STRING), ID_FILE_CLOSE, _T("&Close\tCtrl+F4"));
			mnuContext.AppendMenu(MF_SEPARATOR);
			mnuContext.AppendMenu((MF_ENABLED | MF_STRING), ID_VIEW_SOURCE, _T("&View Source"));

			if(m_pCmdBar != NULL)
			{
				// NOTE: The CommandBarCtrl in our case is the mainframe's, so the commands
				//  would actually go to the main frame if we don't specify TPM_RETURNCMD.
				//  In the main frame's message map, if we don't specify
				//  CHAIN_MDI_CHILD_COMMANDS, we are not going to see those command
				//  messages. We have 2 choices here - either specify TPM_RETURNCMD,
				//  then send/post the message to our window, or don't specify
				//  TPM_RETURNCMD, and be sure to have CHAIN_MDI_CHILD_COMMANDS
				//  in the main frame's message map.

				//m_pCmdBar->TrackPopupMenu(mnuContext, TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_TOPALIGN | TPM_VERTICAL,
				//	ptPopup.x, ptPopup.y);

				DWORD nSelection = m_pCmdBar->TrackPopupMenu(mnuContext, TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_TOPALIGN | TPM_VERTICAL | TPM_RETURNCMD,
					ptPopup.x, ptPopup.y);
				if(nSelection != 0)
				{
					this->PostMessage(WM_COMMAND, MAKEWPARAM(nSelection, 0));
				}
			}
			else
			{
				mnuContext.TrackPopupMenuEx(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_TOPALIGN | TPM_VERTICAL,
					ptPopup.x, ptPopup.y, m_hWnd, NULL);
			}
		}

		return 0;
	}

	LRESULT OnFileSave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		this->MessageBox(_T("OnFileSave"));
		return 0;
	}

	LRESULT OnFileClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		ATLTRACE(_T("Post SC_CLOSE from %s(%u)\n"), __FILE__,__LINE__);
		PostMessage(WM_SYSCOMMAND, SC_CLOSE, 0L);
		return 0;
	}

	LRESULT OnBrowserNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		DWORD_PTR dwRet = (DWORD_PTR)::ShellExecute(0, _T("open"), m_Url, 0, 0, SW_SHOWNORMAL);
		bool bRet = (dwRet > 32);
		ATLASSERT(bRet);
		return 0;
	}

	LRESULT OnBrowserBack(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		m_spBrowser->GoBack();
		return 0;
	}

	LRESULT OnBrowserForward(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		m_spBrowser->GoForward();
		return 0;
	}

	LRESULT OnBrowserStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		m_spBrowser->Stop();
		return 0;
	}

	LRESULT OnBrowserRefresh(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		m_spBrowser->Refresh();
		return 0;
	}

	LRESULT OnViewSource(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		this->DisplayTab(m_HtmlSourceView, FALSE);
		return 0;
	}

public:
	BOOL OnIdle()
	{
		if ( GetIMainFrame()->GetActive() == m_hWnd ) 
		{		
			GetIMainFrame()->DisableAllMenuItems();

			int nActiveTab = this->GetTabCtrl().GetCurSel();
			if(nActiveTab == m_nHtmlViewTabIndex)
			{
				// Get the main window's UI updater
				CUpdateUIBase * pUpdateUI = GetIMainFrame()->GetUpdateUIBase();

				pUpdateUI->UIEnable(ID_BROWSER_NEW, true);
				pUpdateUI->UIEnable(ID_BROWSER_BACK, m_EnableBack);
				pUpdateUI->UIEnable(ID_BROWSER_FORWARD, m_EnableForward);
				VARIANT_BOOL e;
				m_spBrowser->get_Busy(&e);
				pUpdateUI->UIEnable(ID_BROWSER_STOP, (e != false));
				pUpdateUI->UIEnable(ID_BROWSER_REFRESH, true);
			}
		}

		return FALSE;
	}

	BEGIN_SINK_MAP(CHtmlFrame)
		SINK_ENTRY_INFO(0, DIID_DWebBrowserEvents2, DISPID_TITLECHANGE, OnTitleChange, &TitleChangeInfo)
		SINK_ENTRY_INFO(0, DIID_DWebBrowserEvents2, DISPID_COMMANDSTATECHANGE, OnCommandStateChange, &CommandStateChangeInfo)
		SINK_ENTRY_INFO(0, DIID_DWebBrowserEvents2, DISPID_BEFORENAVIGATE2, OnBeforeNavigate2, &BeforeNavigate2Info)
		SINK_ENTRY_INFO(0, DIID_DWebBrowserEvents2, DISPID_NAVIGATECOMPLETE2, OnNavigateComplete2, &NavigateComplete2Info)
		SINK_ENTRY_INFO(0, DIID_DWebBrowserEvents2, DISPID_DOCUMENTCOMPLETE, OnDocumentComplete, &DocumentCompleteInfo)
	END_SINK_MAP()

	void __stdcall OnTitleChange(BSTR sTitle)
	{
		baseClass::SetTitle(CW2T(sTitle));
	}

	void __stdcall OnCommandStateChange(long Command, VARIANT_BOOL Enable)
	{
		switch(Command)
		{
		case CSC_NAVIGATEFORWARD:
			m_EnableForward = Enable;
			break;
		case CSC_NAVIGATEBACK:
			m_EnableBack = Enable;
			break;
		};
	}

	void __stdcall OnBeforeNavigate2(IDispatch * /*pDisp*/, VARIANT *& /*url*/, VARIANT *& /*Flags*/, VARIANT *& /*TargetFrameName*/, VARIANT *& /*PostData*/, VARIANT *& /*Headers*/, VARIANT_BOOL *& /*Cancel*/)
	{
	}

	void __stdcall OnNavigateComplete2(IDispatch* pDisp, VARIANT* /*vtURL*/)
	{
		HRESULT hr = S_OK;
		CComQIPtr<IWebBrowser2> pBrowser(pDisp);
		if(pBrowser)
		{
			//CComPtr<IHTMLDocument> pDocument;
			CComPtr<IDispatch> pDocument;
			hr = pBrowser->get_Document(&pDocument);

			CComQIPtr<IHTMLDocument2> pHtmlDocument(pDocument);
			if(pHtmlDocument)
			{
				CComPtr<IHTMLLocation> pHtmlLocation;
				pHtmlDocument->get_location(&pHtmlLocation);
				if(pHtmlLocation)
				{
					CComBSTR sHref;
					pHtmlLocation->get_href(&sHref);
					if(sHref.Length() > 0)
					{
						baseClass::SetTabToolTip(COLE2T(sHref));
					}
				}
			}
		}
	}

	void __stdcall OnDocumentComplete(IDispatch* pDisp, VARIANT* /*vtURL*/)
	{
		HRESULT hr = S_OK;
		CComQIPtr<IWebBrowser2> pBrowser(pDisp);
		if(pBrowser)
		{
			//CComPtr<IHTMLDocument> pDocument;
			CComPtr<IDispatch> pDocument;
			hr = pBrowser->get_Document(&pDocument);

			CComQIPtr<IHTMLDocument2> doc(pDocument);
			if ((IHTMLDocument2*)doc != NULL)
			{
				CComBSTR ct;
				doc->get_charset(&ct);

				CComQIPtr<IPersistStreamInit> pPersistStreamInit(pDocument);
				if(pPersistStreamInit)
				{
					ULARGE_INTEGER cbSize = {0};
					pPersistStreamInit->GetSizeMax(&cbSize);

					HGLOBAL hGlobal = ::GlobalAlloc(GHND, (DWORD)cbSize.QuadPart);

					if(hGlobal != NULL)
					{
						CComPtr<IStream> pStream;
						hr = ::CreateStreamOnHGlobal(hGlobal, FALSE, &pStream);
						if(pStream)
						{
							hr = pPersistStreamInit->Save(pStream, TRUE);

							// TODO: Figure out the character encoding,
							//  and translate to LPTSTR
							char * sText = (char *)::GlobalLock(hGlobal);
	/*						DWORD cbText = ::GlobalSize(hGlobal);

							if (ct == "utf-8")
							{
								char * buff = (char *)::malloc(::WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)sText, -1, NULL, 0, NULL, NULL));
								::WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)sText, -1, buff, sizeof(buff), NULL, NULL);
								m_HtmlSourceView.SetWindowText(A2CT(buff));
								free(buff);
							}
							else
	*/
							m_HtmlSourceView.SetWindowText(CA2T(sText));

							::GlobalUnlock(hGlobal);
						}

						::GlobalFree(hGlobal);
					}
				}
			}
		}
	}

	virtual void UIUpdateMenuItems()
	{
		GetIMainFrame()->DisableAllMenuItems();
	}

	void DoCreateView()
	{
	}

	CString GetTitle()
	{
		return m_Title;
	}
};

//now using Builder
//void OpenEclWatchMDI(const CString & wuid, HWND hWndClient)
//{
//	CString url;
//	url += GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT);
//	url += _T("/WUInfo?Wuid=");
//	url += wuid;
//	OpenSingletonMDI<CHtmlFrame, MDI_TYPE_ECLWATCH>(pMenuManager, wuid, url, hWndClient);
//}
//  ======================================================================================================================================================
