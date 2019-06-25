#include "stdafx.h"

#include "HTMLView.h"
#include <Autoupdate.h>
#include "CefClientHandler.h"

#define WM_BROWSERTITLECHANGE        (WM_APP)
#define WM_BROWSERDOCUMENTCOMPLETE   (WM_APP + 1)
#define WM_BROWSERSTATUSTEXTCHANGE   (WM_APP + 2)

class CHtmlChromiumView :
    public CWindowImpl<CHtmlChromiumView>,
    public ICefDelegate,
    public IHtmlView
{
    typedef CHtmlChromiumView thisClass;
    typedef CWindowImpl<CHtmlChromiumView> baseClass;

protected:
    CefRefPtr<CefClientHandler> m_clientHandler;

    std::_tstring m_url;
    std::_tstring m_user;
    std::_tstring m_pw;

    bool m_bCanGoBack;
    bool m_bCanGoForward;
    bool m_bIsLoading;
    
public:
    // DECLARE_WND_CLASS(NULL)

    CHtmlChromiumView() : m_bCanGoBack(false), m_bCanGoForward(false), m_bIsLoading(true)
    {
    }

    ~CHtmlChromiumView() 
    {
        if (m_clientHandler != nullptr) {
            m_clientHandler->DetachDelegate();
        }
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
        return m_clientHandler != nullptr && m_clientHandler->IsLoaded();
    }

    inline bool BackEnabled() const { return m_bCanGoBack; }
    inline bool ForwardEnabled() const { return m_bCanGoForward; }

    virtual HRESULT Navigate(const TCHAR* pszUrl, bool newWindow = false)
    {
        m_url = pszUrl;
        DNSLookup(m_url);
        auto frame = m_clientHandler->GetBrowser()->GetMainFrame();
        if (frame != nullptr) {
            frame->LoadURL(CefString(m_url));
            return S_OK;
        }
        return E_FAIL;
    }

    virtual void CreateHTMLControl(const TCHAR * pszUrl, const TCHAR* pszUserID, const TCHAR* pszPassword)
    {
        m_url = pszUrl;
        m_user = pszUserID;
        m_pw = pszPassword;
        if (!m_clientHandler)
        {
            DNSLookup(m_url);

            auto rect = RECT{ 0 };
            GetClientRect(&rect);

            CefWindowInfo info;
            info.SetAsChild(m_hWnd, rect);

            CefBrowserSettings browserSettings;
            browserSettings.web_security = STATE_DISABLED;

            m_clientHandler = new CefClientHandler(this);
            m_clientHandler->CreateBrowser(info, browserSettings, CefString(m_url));
        }
        else 
        {
            Navigate(pszUrl);
        }
    }

    HRESULT put_ServiceProvider(IUnknown* pUnkSite)
    {
        return E_FAIL;
    }

    HRESULT put_ProfferService(REFGUID rguidService, IServiceProvider* pServiceProvider)
    {
        return E_FAIL;
    }

    void BrowserBack()
    {
        if (IsLoaded()) {
            m_clientHandler->GetBrowser()->GoBack();
        }
    }

    void BrowserForward()
    {
        if (IsLoaded()) {
            m_clientHandler->GetBrowser()->GoForward();
        }
    }

    void BrowserStop()
    {
        if (IsLoaded()) {
            m_clientHandler->GetBrowser()->StopLoad();
        }
    }

    void BrowserRefresh()
    {
        if (IsLoaded()) {
            m_clientHandler->GetBrowser()->Reload();
        }
    }

    void BrowserCmdCopy()
    {
        //  TODO:  Not sure its possible  ---
    }

    void BrowserCmdSelectAll()
    {
        //  TODO:  Not sure its possible  ---
    }

    BOOL PreTranslateMessage(MSG* pMsg)
    {
        if (pMsg->message == WM_KEYDOWN)
        {
            if (pMsg->wParam == VK_F5)
            {
                m_clientHandler->GetBrowser()->Reload();
            }
        }

        return FALSE;
    }

    // Message map and handlers
    BEGIN_MSG_MAP(thisClass)
        MSG_WM_SIZE(OnSize)
    END_MSG_MAP()

    void OnSize(UINT wParam, const CSize& size)
    {
        if (IsLoaded())
        {
            auto hwnd = m_clientHandler->GetBrowser()->GetHost()->GetWindowHandle();
            auto rect = RECT{ 0 };
            GetClientRect(&rect);
            ::SetWindowPos(hwnd, HWND_TOP, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER);
        }
    }

    //  CefClientHandler::Delegate  ---
    virtual void OnBrowserCreated(CefRefPtr<CefBrowser> browser) 
    {
    }

    virtual void OnBrowserClosing(CefRefPtr<CefBrowser> browser) 
    {
    }

    virtual void OnBrowserClosed(CefRefPtr<CefBrowser> browser) 
    {
        if (IsLoaded() && m_clientHandler->GetBrowser()->GetIdentifier() == browser->GetIdentifier())
        {
            m_clientHandler->DetachDelegate();
        }
    }

    virtual void OnSetAddress(std::string const& url) 
    {
    }

    virtual void OnSetTitle(std::string const& title) 
    {
    }

    virtual void OnSetFullscreen(bool const fullscreen) 
    {
    }

    virtual void OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, CefRefPtr<CefRequestCallback> callback)
    {
        std::_tstring auth = m_user +_T(":") + m_pw;
        char buff[256];
        int buffLen = sizeof(buff);
        std::string narrowAuth = CT2A(auth.c_str());
        Base64Encode((BYTE*)narrowAuth.c_str(), narrowAuth.length(), buff, &buffLen);

        std::_tstring extraHeaders = std::_tstring(_T("Basic ")) + static_cast<const TCHAR *>(CString(buff, buffLen));

        CefRequest::HeaderMap map;
        request->GetHeaderMap(map);
        map.insert(std::make_pair(CefString("Authorization"), CefString(extraHeaders)));
        request->SetHeaderMap(map);
    }

    virtual void OnSetLoadingState(bool const isLoading, bool const canGoBack, bool const canGoForward) 
    {
        m_bCanGoBack = canGoBack;
        m_bCanGoForward = canGoForward;
        m_bIsLoading = isLoading;
    }
};

IHtmlView* CreateChromiumView()
{
    return new CHtmlChromiumView();
}
