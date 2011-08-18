#pragma once

#ifndef __WTL_TABBED_FRAME_H__
	#error HtmlFrame.h requires TabbedFrame.h to be included first
#endif

#include "IdlePump.h"
#include "MainFrm.h"
#include "htmlview.h"
#include "ChildFrame.h"
#include "global.h"

static _ATL_FUNC_INFO TitleChangeInfo = {CC_STDCALL, VT_EMPTY, 1, { VT_BSTR } };
static _ATL_FUNC_INFO CommandStateChangeInfo = {CC_STDCALL, VT_EMPTY, 2, { VT_I4, VT_BOOL } };
static _ATL_FUNC_INFO BeforeNavigate2Info = {CC_STDCALL, VT_EMPTY, 7, { VT_DISPATCH, (VT_BYREF | VT_VARIANT), (VT_BYREF | VT_VARIANT), (VT_BYREF | VT_VARIANT), (VT_BYREF | VT_VARIANT), (VT_BYREF | VT_VARIANT), (VT_BYREF | VT_BOOL)}};
static _ATL_FUNC_INFO NavigateComplete2Info = {CC_STDCALL, VT_EMPTY, 2, { VT_DISPATCH, (VT_BYREF | VT_VARIANT)}};
static _ATL_FUNC_INFO DocumentCompleteInfo = {CC_STDCALL, VT_EMPTY, 2, { VT_DISPATCH, (VT_BYREF | VT_VARIANT) } };

const int ID_VIEW_SOURCE = WM_USER;

typedef CWinTraits<WS_OVERLAPPEDWINDOW | WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_MAXIMIZE, WS_EX_MDICHILD>	CHtmlFrameWinTraits;

class CHtmlFrame : 
	public CTabbedFrameImpl<CHtmlFrame, CDotNetButtonTabCtrl<CTabViewTabItem>, CTabbedMDIChildWindowImpl<CHtmlFrame, CMDIWindow, CHtmlFrameWinTraits> >,
	public IDispEventSimpleImpl<0, CHtmlFrame, &DIID_DWebBrowserEvents2>,
	public CIdleHandler
{
protected:
	typedef CTabbedFrameImpl<CHtmlFrame, CDotNetButtonTabCtrl<CTabViewTabItem>, CTabbedMDIChildWindowImpl<CHtmlFrame, CMDIWindow, CHtmlFrameWinTraits> > baseClass;

public:
	DECLARE_FRAME_WND_CLASS(NULL, IDR_MDICHILD)

	CTabbedMDICommandBarCtrl* m_pCmdBar;
    
	CMenuManager * m_MenuManager;
	CHtmlView m_HtmlView;
	CPlainTextView m_HtmlSourceView;

	VARIANT_BOOL m_EnableBack;
	VARIANT_BOOL m_EnableForward;

	int m_nHtmlViewTabIndex;
	int m_nHtmlSourceViewTabIndex;
	CString m_Wuid;

	CComPtr<IUnknown> m_punkBrowser;
	CComPtr<IWebBrowser2> m_spBrowser;

	CHtmlFrame(CMenuManager * pMenuManager, const CString & pWuid) :
		m_pCmdBar(NULL),
		m_nHtmlViewTabIndex(-1),
		m_nHtmlSourceViewTabIndex(-1)
	{
		m_MenuManager = pMenuManager;
		m_Wuid = pWuid;

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
		CString url;
		url += GetIConfig()->Get(GLOBAL_SERVER_WORKUNIT);
		url += "/WUInfo?Wuid=";
		url += m_Wuid;
		LRESULT lRet = DefWindowProc(uMsg, wParam, lParam);
		bHandled = TRUE;

		CreateTabWindow(m_hWnd, rcDefault, (CTCS_BOTTOM | CTCS_TOOLTIPS | CTCS_HOTTRACK));

		m_HtmlView.Create(m_hWnd, rcDefault, url, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL, WS_EX_CLIENTEDGE);
		m_HtmlSourceView.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_NOHIDESEL, WS_EX_CLIENTEDGE);

		m_nHtmlViewTabIndex =        this->AddTab(m_HtmlView,       _T("HTML"));
		m_nHtmlSourceViewTabIndex =  this->AddTab(m_HtmlSourceView, _T("Source"));

		this->UpdateTabToolTip(m_HtmlView, _T("View the Rendered HTML"));
		this->UpdateTabToolTip(m_HtmlSourceView, _T("View the HTML Source"));

		m_HtmlView.get_Control(&m_punkBrowser);
		if(m_punkBrowser)
		{
			DispEventAdvise(m_punkBrowser, &DIID_DWebBrowserEvents2);
		}

		m_HtmlView.get_Browser(&m_spBrowser);

		baseClass::SetTitle(m_Wuid);

		// register ourselfs for idle updates
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
		{
			DispEventUnadvise(m_punkBrowser, &DIID_DWebBrowserEvents2);
		}

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
			return m_HtmlView.PreTranslateMessage(pMsg);
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
		ATLTRACE(_T("Send SC_CLOSE from %s(%u)\n"), __FILE__,__LINE__);
		this->PostMessage(WM_SYSCOMMAND, SC_CLOSE, 0L);
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
		if ( m_MenuManager->GetActive() == m_hWnd ) 
		{		
			m_MenuManager->DisableAll();

			int nActiveTab = this->GetTabCtrl().GetCurSel();
			if(nActiveTab == m_nHtmlViewTabIndex)
			{
				// Get the main window's UI updater
				CUpdateUIBase * pUpdateUI = m_MenuManager->m_UpdateUI;

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
		baseClass::SetTitle(COLE2T(sTitle));
	}

	void __stdcall OnCommandStateChange(long Command, VARIANT_BOOL Enable)
	{
		HRESULT hr = S_OK;
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

void __stdcall OnBeforeNavigate2(IDispatch *pDisp, VARIANT *&url, VARIANT *&Flags, VARIANT *&TargetFrameName, VARIANT *&PostData, VARIANT *&Headers, VARIANT_BOOL *&Cancel)
{
	HRESULT hr = S_OK;
}

void __stdcall OnNavigateComplete2(IDispatch* pDisp, VARIANT* vtURL)
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

	void __stdcall OnDocumentComplete(IDispatch* pDisp, VARIANT* vtURL)
	{
		HRESULT hr = S_OK;
		CComQIPtr<IWebBrowser2> pBrowser(pDisp);
		if(pBrowser)
		{
			//CComPtr<IHTMLDocument> pDocument;
			CComPtr<IDispatch> pDocument;
			hr = pBrowser->get_Document(&pDocument);

			CComQIPtr<IHTMLDocument2> doc(pDocument);
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
						LPTSTR sText = (LPTSTR)::GlobalLock(hGlobal);
						DWORD cbText = ::GlobalSize(hGlobal);

/*						if (ct == "unicode")
						{
							TCHAR * buff = (TCHAR *)::malloc(::WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)sText, -1, NULL, 0, NULL, NULL));
							::WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)sText, -1, buff, sizeof(buff), NULL, NULL);

							m_HtmlSourceView.SetWindowText(buff);

							free(buff);
						}
						else
*/
						m_HtmlSourceView.SetWindowText(sText);

						::GlobalUnlock(hGlobal);
					}

					::GlobalFree(hGlobal);
				}
			}
		}
	}
};
