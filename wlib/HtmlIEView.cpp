#include "stdafx.h"

#include "HTMLView.h"
#include <exdispid.h>
#include <Autoupdate.h>

const int _nDispatchID = 1;

#define WM_BROWSERTITLECHANGE        (WM_APP)
#define WM_BROWSERDOCUMENTCOMPLETE   (WM_APP + 1)
#define WM_BROWSERSTATUSTEXTCHANGE   (WM_APP + 2)

class CHtmlIEView:
    public CWindowImpl<CHtmlIEView, CAxWindow>,
    public IDispEventSimpleImpl<_nDispatchID, CHtmlIEView, &DIID_DWebBrowserEvents2>,
    public IHtmlView
{
    typedef CHtmlIEView thisClass;
    typedef CWindowImpl<CHtmlIEView, CAxWindow> baseClass;

protected:
    CComPtr<IUnknown> m_punkBrowser;
    CComPtr<IUnknown> m_punkContainer;
    CComPtr<IWebBrowser2> m_spBrowser;

    std::_tstring m_url;
    std::_tstring m_user;
    std::_tstring m_pw;

    bool m_bCanGoBack;
    bool m_bCanGoForward;

    DWORD m_nProfferAuthenticateCookie;

public:
    DECLARE_WND_SUPERCLASS(_T("TabBrowser_TabPageWindow"), CAxWindow::GetWndClassName())
    // IDispatch events function info
    static _ATL_FUNC_INFO DocumentComplete2_Info;
    static _ATL_FUNC_INFO Navigate2Complete_Info;
    static _ATL_FUNC_INFO TitleChange_Info;
    static _ATL_FUNC_INFO StatusTextChange_Info;
    static _ATL_FUNC_INFO CommandStateChange_Info;

    CHtmlIEView() : m_bCanGoBack(false), m_bCanGoForward(false)//, m_first(true)
    {
        m_nProfferAuthenticateCookie = 0;
    }

    virtual void OnFinalMessage(_In_ HWND /*hWnd*/)
    {
        delete this;
    }

    HWND Create(HWND hWndParent, _U_RECT rect = NULL, LPCTSTR szWindowName = NULL, DWORD dwStyle = 0, DWORD dwExStyle = 0)
    {
        return baseClass::Create(hWndParent, rect, szWindowName, dwStyle, dwExStyle);
    }

    BOOL SetWindowPos(HWND hWndInsertAfter, int x, int y, int cx, int cy, UINT nFlags) throw()
    {
        return baseClass::SetWindowPos(hWndInsertAfter, x, y, cx, cy, nFlags);
    }

    bool IsLoaded()
    {
        return m_punkBrowser != 0;
    }

    inline bool BackEnabled() const { return m_bCanGoBack; }
    inline bool ForwardEnabled() const { return m_bCanGoForward; }

    virtual HRESULT Navigate(const TCHAR *pszUrl, bool newWindow = false)
    {
        m_url = pszUrl;
        DNSLookup(m_url);
        CString extraHeaders;
        CString auth = m_user.c_str();
        auth += ":";
        auth += m_pw.c_str();
        char buff[256];
        int buffLen = sizeof(buff);
        std::string narrowAuth = CT2A(auth);
        Base64Encode((BYTE *)narrowAuth.c_str(), narrowAuth.length(), buff, &buffLen);
        extraHeaders += "Authorization: Basic ";
        extraHeaders += CString(buff, buffLen);
        //extraHeaders += "\r\n";

        _bstr_t url(m_url.c_str());
        CComVariant vTargetFrameName = newWindow ? _T("_blank") : _T("_top");
        CComVariant vFlags;
        CComVariant vPostData;
        CComVariant vHeaders(extraHeaders);
        return m_spBrowser->Navigate(url, &vFlags, &vTargetFrameName, &vPostData, &vHeaders);
    }

    virtual void CreateHTMLControl(const TCHAR* pszUrl, const TCHAR* pszUserID, const TCHAR* pszPassword)
    {
        m_url = pszUrl;
        m_user = pszUserID;
        m_pw = pszPassword;
        if ( !m_punkBrowser )
        {
            CreateControlEx( L"{8856F961-340A-11D0-A96B-00C04FD705A2}", NULL, &m_punkContainer, &m_punkBrowser);
            if(m_punkBrowser)
            {
                DispEventAdvise(m_punkBrowser, &DIID_DWebBrowserEvents2);
            }
        }

        if ( !m_spBrowser )
        {
            get_Browser(&m_spBrowser);
        }
        SetUserAgentString();
        Navigate(pszUrl);
    }

    HRESULT get_Control(IUnknown** ppControl)
    {
        if(m_hWnd != NULL)
        {
            return CAxWindow::QueryControl(ppControl);
        }
        return NULL;
    }

    HRESULT get_Browser(IWebBrowser2** ppBrowser)
    {
        if(m_hWnd != NULL)
        {
            return CAxWindow::QueryControl(ppBrowser);
        }
        return NULL;
    }

    HRESULT put_ServiceProvider(IUnknown* pUnkSite)
    {
        //this will provide a service provider for all sites
        CComPtr<IObjectWithSite> spHost;
        HRESULT hr = QueryHost(__uuidof(IObjectWithSite), (void**)&spHost);
        if (SUCCEEDED(hr))
            hr = spHost->SetSite(pUnkSite);
        return hr;
    }

    HRESULT put_ProfferService(REFGUID rguidService, IServiceProvider *pServiceProvider)
    {
        //this will provide a service provider for the current host only
        CComPtr<IServiceProvider> spHost;
        HRESULT hr = m_punkBrowser->QueryInterface(IID_IServiceProvider, (void**)&spHost);
        if (SUCCEEDED(hr))
        {
            CComPtr<IProfferService> spService;
            hr = spHost->QueryService(SID_SProfferService, IID_IProfferService, (void**)&spService);
            if (SUCCEEDED(hr))
            {
                hr = spService->ProfferService(rguidService, pServiceProvider, &m_nProfferAuthenticateCookie);
            }
        }
        return hr;
    }

    void BrowserBack()
    {
        if (IsLoaded()) {
            m_spBrowser->GoBack();
        }
    }

    void BrowserForward()
    {
        if (IsLoaded()) {
            m_spBrowser->GoForward();
        }
    }

    void BrowserStop()
    {
        if (IsLoaded()) {
            m_spBrowser->Stop();
        }
    }

    void BrowserRefresh()
    {
        if (IsLoaded()) {
            m_spBrowser->Refresh();
        }
    }

    void BrowserCmdCopy()
    {
        if (IsLoaded()) {
            m_spBrowser->ExecWB(OLECMDID_COPY, OLECMDEXECOPT_DONTPROMPTUSER, NULL, NULL);
        }
    }

    void BrowserCmdSelectAll()
    {
        if (IsLoaded()) {
            m_spBrowser->ExecWB(OLECMDID_SELECTALL, OLECMDEXECOPT_DONTPROMPTUSER, NULL, NULL);
        }
    }
    
    bool SetUserAgentString()
    {
        static std::string g_userAgent;

        if (g_userAgent.empty())
        {
            char * buff = NULL;
            DWORD pdwBufferLengthOut = 0;
            HRESULT hr = UrlMkGetSessionOption(URLMON_OPTION_USERAGENT, NULL, 0, &pdwBufferLengthOut, 0);
            buff = (char *)malloc(pdwBufferLengthOut);
            hr = UrlMkGetSessionOption(URLMON_OPTION_USERAGENT, buff, pdwBufferLengthOut, &pdwBufferLengthOut, 0);
            //  if (hr == S_OK)  API Bug returns E_OUTOFMEMORY on success  
            {
                g_userAgent = buff;
                std::_tstring ver;
                boost::algorithm::replace_last(g_userAgent, ")", (boost::format("; eclide/%1%)") % static_cast<const char *>(CT2A(GetAppVersion(ver)))).str());
            }
            free(buff);
        }

        if (!g_userAgent.empty())
        {
            UrlMkSetSessionOption(URLMON_OPTION_USERAGENT, (LPVOID)g_userAgent.c_str(), g_userAgent.length(), 0);
            return true;
        }
        return false;
    }

    BOOL PreTranslateMessage(MSG* pMsg)
    {
        if((pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) &&
           (pMsg->message < WM_MOUSEFIRST || pMsg->message > WM_MOUSELAST))
            return FALSE;

        BOOL bRet = FALSE;
        // give HTML page a chance to translate this message
        if(pMsg->hwnd == m_hWnd || IsChild(pMsg->hwnd))
            bRet = (BOOL)SendMessage(WM_FORWARDMSG, 0, (LPARAM)pMsg);

        return bRet;
    }

    void SetFocusToHTML()
    {
        CComPtr<IWebBrowser2> spWebBrowser;
        HRESULT hRet = QueryControl(IID_IWebBrowser2, (void**)&spWebBrowser);
        if(SUCCEEDED(hRet) && spWebBrowser != NULL)
        {
            CComPtr<IDispatch> spDocument;
            hRet = spWebBrowser->get_Document(&spDocument);
            if(SUCCEEDED(hRet) && spDocument != NULL)
            {
                CComQIPtr<IHTMLDocument2> spHtmlDoc = spDocument;
                if(spHtmlDoc != NULL)
                {
                    CComPtr<IHTMLWindow2> spParentWindow;
                    hRet = spHtmlDoc->get_parentWindow(&spParentWindow);
                    if(spParentWindow != NULL)
                        spParentWindow->focus();
                }
            }
        }
    }

// Event map and handlers
    BEGIN_SINK_MAP(CHtmlIEView)
        SINK_ENTRY_INFO(_nDispatchID, DIID_DWebBrowserEvents2, DISPID_DOCUMENTCOMPLETE, OnEventDocumentComplete, &DocumentComplete2_Info)
        SINK_ENTRY_INFO(_nDispatchID, DIID_DWebBrowserEvents2, DISPID_NAVIGATECOMPLETE2, OnEventNavigate2Complete, &Navigate2Complete_Info)
        SINK_ENTRY_INFO(_nDispatchID, DIID_DWebBrowserEvents2, DISPID_TITLECHANGE, OnEventTitleChange, &TitleChange_Info)
        SINK_ENTRY_INFO(_nDispatchID, DIID_DWebBrowserEvents2, DISPID_STATUSTEXTCHANGE, OnEventStatusTextChange, &StatusTextChange_Info)
        SINK_ENTRY_INFO(_nDispatchID, DIID_DWebBrowserEvents2, DISPID_COMMANDSTATECHANGE, OnEventCommandStateChange, &CommandStateChange_Info)
    END_SINK_MAP()

    void __stdcall OnEventDocumentComplete(IDispatch* /*pDisp*/, VARIANT* URL)
    {
        // Send message to the main frame
        ATLASSERT(V_VT(URL) == VT_BSTR);
        USES_CONVERSION;
        SendMessage(GetTopLevelWindow(), WM_BROWSERDOCUMENTCOMPLETE, (WPARAM)m_hWnd, (LPARAM)OLE2T(URL->bstrVal));

        //SetFocusToHTML();
        //std::_tstring url = CW2T(URL->bstrVal);
        //if (boost::algorithm::equals(m_url, url))
        //{
        //	SetFocusToHTML();
        //	if (m_first)
        //	{
        //		m_first = false;
        //		BrowserRefresh();
        //	}
        //}
    }

    void __stdcall OnEventNavigate2Complete(IDispatch* /*pDisp*/, VARIANT* URL)
    {
        // Send message to the main frame
        ATLASSERT(V_VT(URL) == VT_BSTR);
        USES_CONVERSION;
    }

    void __stdcall OnEventTitleChange(BSTR Text)
    {
        USES_CONVERSION;
        SendMessage(GetTopLevelWindow(), WM_BROWSERTITLECHANGE, (WPARAM)m_hWnd, (LPARAM)OLE2CT(Text));
    }

    void __stdcall OnEventStatusTextChange(BSTR Text)
    {
        USES_CONVERSION;
        SendMessage(GetTopLevelWindow(), WM_BROWSERSTATUSTEXTCHANGE, (WPARAM)m_hWnd, (LPARAM)OLE2CT(Text));
    }

    void __stdcall OnEventCommandStateChange(long Command, VARIANT_BOOL Enable)
    {
        if(Command == CSC_NAVIGATEBACK)
            m_bCanGoBack = (Enable != VARIANT_FALSE);
        else if(Command == CSC_NAVIGATEFORWARD)
            m_bCanGoForward = (Enable != VARIANT_FALSE);
    }

// Message map and handlers
    BEGIN_MSG_MAP(CBrowserView)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
        MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
    END_MSG_MAP()

    LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
    {
        LRESULT lRet = DefWindowProc(uMsg, wParam, lParam);

        // Connect events
        CComPtr<IWebBrowser2> spWebBrowser2;
        HRESULT hRet = QueryControl(IID_IWebBrowser2, (void**)&spWebBrowser2);
        if(SUCCEEDED(hRet))
        {
            if(FAILED(DispEventAdvise(spWebBrowser2, &DIID_DWebBrowserEvents2)))
                ATLASSERT(FALSE);
        }

        // Set host flag to indicate that we handle themes
        CComPtr<IAxWinAmbientDispatch> spHost;
        hRet = QueryHost(IID_IAxWinAmbientDispatch, (void**)&spHost);
        if(SUCCEEDED(hRet))
        {
            const DWORD _DOCHOSTUIFLAG_THEME = 0x40000;
            hRet = spHost->put_DocHostFlags(DOCHOSTUIFLAG_NO3DBORDER | _DOCHOSTUIFLAG_THEME);
            ATLASSERT(SUCCEEDED(hRet));
        }

        return lRet;
    }
    
    LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        // Disconnect events
        CComPtr<IWebBrowser2> spWebBrowser2;
        HRESULT hRet = QueryControl(IID_IWebBrowser2, (void**)&spWebBrowser2);
        if(SUCCEEDED(hRet))
            DispEventUnadvise(spWebBrowser2, &DIID_DWebBrowserEvents2);

        bHandled=FALSE;
        return 1;
    }

    LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
    {
        LRESULT lRet = DefWindowProc(uMsg, wParam, lParam);
        SetFocusToHTML();

        return lRet;
    }
    //////////////////////////////////////////////////////////
    //
    static CString GetResourceURL(unsigned nResId) 
    {
        #define fileNameLen 258
        TCHAR fileName[fileNameLen];
        ::GetModuleFileName(ATL::_AtlBaseModule.GetResourceInstance(), fileName, fileNameLen);
        TCHAR szURL[360];
        ::wsprintf( szURL, _T("res://%s/%u"), fileName, nResId );

        return szURL;

    } //GetResourceURL
};

__declspec(selectany) _ATL_FUNC_INFO CHtmlIEView::DocumentComplete2_Info = { CC_STDCALL, VT_EMPTY, 2, { VT_DISPATCH, VT_BYREF | VT_VARIANT } };
__declspec(selectany) _ATL_FUNC_INFO CHtmlIEView::Navigate2Complete_Info = { CC_STDCALL, VT_EMPTY, 2, { VT_DISPATCH, VT_BYREF | VT_VARIANT } };
__declspec(selectany) _ATL_FUNC_INFO CHtmlIEView::TitleChange_Info = { CC_STDCALL, VT_EMPTY, 1, { VT_BSTR } };
__declspec(selectany) _ATL_FUNC_INFO CHtmlIEView::StatusTextChange_Info = { CC_STDCALL, VT_EMPTY, 1, { VT_BSTR } };
__declspec(selectany) _ATL_FUNC_INFO CHtmlIEView::CommandStateChange_Info = { CC_STDCALL, VT_EMPTY, 2, { VT_I4, VT_BOOL } };

IHtmlView * CreateIEView()
{
    return static_cast<IHtmlView*>(new CHtmlIEView());
}
