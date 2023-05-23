#include "stdafx.h"

#include "HTMLView.h"
#include "Logger.h"
#include <Global.h>

using namespace Microsoft::WRL;

#define WM_BROWSERTITLECHANGE (WM_APP)
#define WM_BROWSERDOCUMENTCOMPLETE (WM_APP + 1)
#define WM_BROWSERSTATUSTEXTCHANGE (WM_APP + 2)

class CHtmlWebView2View : public CWindowImpl<CHtmlWebView2View>,
                          public IHtmlView
{
    typedef CHtmlWebView2View thisClass;
    typedef CWindowImpl<CHtmlWebView2View> baseClass;

protected:
    wil::com_ptr<ICoreWebView2Environment> m_environment;
    wil::com_ptr<ICoreWebView2> m_webview;
    wil::com_ptr<ICoreWebView2Controller> m_controller;

    EventRegistrationToken m_basicAuthenticationRequestedToken;
    EventRegistrationToken m_contentLoadingToken;
    EventRegistrationToken m_navigationStartingToken;
    EventRegistrationToken m_navigationCompletedToken;
    EventRegistrationToken m_documentTitleChangedToken;
    EventRegistrationToken m_historyChangedToken;
    EventRegistrationToken m_webMessageReceivedToken;

    std::_tstring m_url;
    std::_tstring m_user;
    std::_tstring m_pw;

    bool m_bCanGoBack;
    bool m_bCanGoForward;
    bool m_bIsLoading;
    bool m_bIsAuthenticating;

public:
    // DECLARE_WND_CLASS(NULL)

    CHtmlWebView2View() : m_bCanGoBack(false), m_bCanGoForward(false), m_bIsLoading(true)
    {
    }

    ~CHtmlWebView2View()
    {
        if (m_webview)
        {
            m_webview->remove_WebMessageReceived(m_webMessageReceivedToken);
            m_webview->remove_HistoryChanged(m_historyChangedToken);
            m_webview->remove_DocumentTitleChanged(m_documentTitleChangedToken);
            m_webview->remove_NavigationCompleted(m_navigationCompletedToken);
            m_webview->remove_NavigationStarting(m_navigationStartingToken);
            m_webview->remove_WebResourceRequested(m_contentLoadingToken);
            if (auto webView10 = m_webview.try_query<ICoreWebView2_10>())
            {
                webView10->remove_BasicAuthenticationRequested(m_basicAuthenticationRequestedToken);
            }
        }

        if (m_environment != nullptr)
        {
            m_environment = nullptr;
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
        return m_webview != nullptr;
    }

    inline bool BackEnabled() const { return m_bCanGoBack; }
    inline bool ForwardEnabled() const { return m_bCanGoForward; }

    virtual HRESULT Navigate(const TCHAR *pszUrl, bool newWindow = false)
    {
        m_url = pszUrl;
        if (m_webview)
        {
            return m_webview->Navigate(pszUrl);
        }
        return S_OK;
    }

    void UpdateButtons()
    {
        if (m_webview)
        {
            BOOL canGoForward;
            BOOL canGoBack;
            m_webview->get_CanGoForward(&canGoForward);
            m_webview->get_CanGoBack(&canGoBack);
            m_bCanGoBack = canGoBack;
            m_bCanGoForward = canGoForward;
        }
    }

    std::wstring getAuth()
    {
        std::wstring authHeader;
        CString auth = m_user.c_str();
        auth += ":";
        auth += m_pw.c_str();
        char buff[256];
        int buffLen = sizeof(buff);
        std::string narrowAuth = CT2A(auth);
        Base64Encode((BYTE *)narrowAuth.c_str(), narrowAuth.length(), buff, &buffLen);
        authHeader += L"Basic ";
        authHeader += CString(buff, buffLen);
        return authHeader;
    }

    HRESULT OnCreateEnvironmentCompleted(HRESULT result, ICoreWebView2Environment *env)
    {
        m_environment = env;
        return env->CreateCoreWebView2Controller(
            m_hWnd, Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(this, &CHtmlWebView2View::OnCreateCoreWebView2Controller).Get());
    }

    HRESULT OnCreateCoreWebView2Controller(HRESULT result, ICoreWebView2Controller *controller)
    {
        if (controller != nullptr)
        {
            m_controller = controller;
            m_controller->get_CoreWebView2(&m_webview);
        }

        if (m_webview)
        {

            wil::com_ptr<ICoreWebView2Settings> settings;
            m_webview->get_Settings(&settings);
            settings->put_IsScriptEnabled(TRUE);
            settings->put_AreDefaultScriptDialogsEnabled(TRUE);
            settings->put_IsWebMessageEnabled(TRUE);

            RECT bounds;
            GetClientRect(&bounds);
            m_controller->put_Bounds(bounds);

            m_webview->AddWebResourceRequestedFilter(L"http://localhost:8010/*", COREWEBVIEW2_WEB_RESOURCE_CONTEXT_IMAGE);

            m_webview->add_NavigationStarting(
                Callback<ICoreWebView2NavigationStartingEventHandler>(this, &CHtmlWebView2View::OnNavigationStarting).Get(), &m_navigationStartingToken);

            m_webview->add_ContentLoading(
                Callback<ICoreWebView2ContentLoadingEventHandler>(this, &CHtmlWebView2View::OnContentLoading).Get(), &m_contentLoadingToken);

            m_webview->add_NavigationCompleted(
                Callback<ICoreWebView2NavigationCompletedEventHandler>(this, &CHtmlWebView2View::OnNavigationCompleted).Get(), &m_navigationCompletedToken);

            m_webview->add_DocumentTitleChanged(
                Callback<ICoreWebView2DocumentTitleChangedEventHandler>(this, &CHtmlWebView2View::OnDocumentTitleChanged).Get(), &m_documentTitleChangedToken);

            m_webview->add_HistoryChanged(
                Callback<ICoreWebView2HistoryChangedEventHandler>(this, &CHtmlWebView2View::OnHistoryChanged).Get(), &m_historyChangedToken);

            m_webview->add_WebMessageReceived(
                Callback<ICoreWebView2WebMessageReceivedEventHandler>(this, &CHtmlWebView2View::OnWebMessageReceived).Get(), &m_webMessageReceivedToken);

            m_bIsAuthenticating = true;
            std::size_t position = m_url.find(L":");
            position = m_url.find(L":", position + 1);
            position = m_url.find(L"/", position + 1);
            std::wstring loginUrl = m_url.substr(0, position + 1);
            loginUrl += L"esp/login?username=" + m_user + L"&password=" + m_pw;
            m_webview->Navigate(loginUrl.c_str());
        }
        else
        {
            _DBGLOG(LEVEL_SEVERE, _T("WebView2 failed to instantiate"));
        }

        return S_OK;
    }

    HRESULT OnNavigationStarting(ICoreWebView2 *webview, ICoreWebView2NavigationStartingEventArgs *args)
    {
        if (m_bIsAuthenticating)
        {
            args->put_Cancel(true);
        }
        else
        {
            UpdateButtons();
            m_bIsLoading = true;
        }
        return S_OK;
    }

    HRESULT OnContentLoading(ICoreWebView2 *webview, ICoreWebView2ContentLoadingEventArgs *args)
    {
        return S_OK;
    }

    HRESULT OnNavigationCompleted(ICoreWebView2 *webview, ICoreWebView2NavigationCompletedEventArgs *args)
    {
        UpdateButtons();
        m_bIsLoading = false;

        if (m_bIsAuthenticating)
        {
            m_bIsAuthenticating = false;
            Navigate(m_url.c_str());
        }
        else
        {
            SendMessage(GetTopLevelWindow(), WM_BROWSERDOCUMENTCOMPLETE, (WPARAM)m_hWnd, (LPARAM)m_url.c_str());
        }
        return S_OK;
    }

    wil::unique_cotaskmem_string title;
    HRESULT OnDocumentTitleChanged(ICoreWebView2 *webview, IUnknown *args)
    {
        webview->get_DocumentTitle(&title);
        SendMessage(GetTopLevelWindow(), WM_BROWSERTITLECHANGE, (WPARAM)m_hWnd, (LPARAM)title.get());
        return S_OK;
    }

    HRESULT OnHistoryChanged(ICoreWebView2 *webview, IUnknown *args)
    {
        UpdateButtons();
        return S_OK;
    }

    wil::unique_cotaskmem_string message;
    HRESULT OnWebMessageReceived(ICoreWebView2 *webview, ICoreWebView2WebMessageReceivedEventArgs *args)
    {
        args->TryGetWebMessageAsString(&message);
        // processMessage(&message);
        webview->PostWebMessageAsString(message.get());
        return S_OK;
    }

    virtual void CreateHTMLControl(const TCHAR *pszUrl, const TCHAR *pszUserID, const TCHAR *pszPassword)
    {
        m_url = pszUrl;
        m_user = pszUserID;
        m_pw = pszPassword;

        boost::filesystem::path appDataFolder;
        GetApplicationFolder(appDataFolder);

        auto options = Make<CoreWebView2EnvironmentOptions>();
        options->put_AdditionalBrowserArguments(L"--disable-web-security");
        CreateCoreWebView2EnvironmentWithOptions(
            nullptr,
            appDataFolder.c_str(),
            options.Get(),
            Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(this, &CHtmlWebView2View::OnCreateEnvironmentCompleted).Get());
    }

    HRESULT put_ServiceProvider(IUnknown *pUnkSite)
    {
        return E_FAIL;
    }

    HRESULT put_ProfferService(REFGUID rguidService, IServiceProvider *pServiceProvider)
    {
        return E_FAIL;
    }

    void BrowserBack()
    {
        if (IsLoaded())
        {
            m_webview->GoBack();
        }
    }

    void BrowserForward()
    {
        if (IsLoaded())
        {
            m_webview->GoForward();
        }
    }

    void BrowserStop()
    {
        if (IsLoaded())
        {
            m_webview->Stop();
        }
    }

    void BrowserRefresh()
    {
        if (IsLoaded())
        {
            m_webview->Reload();
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

    BOOL PreTranslateMessage(MSG *pMsg)
    {
        if (pMsg->message == WM_KEYDOWN)
        {
            if (pMsg->wParam == VK_F5)
            {
            }
        }

        return FALSE;
    }

    // Message map and handlers
    BEGIN_MSG_MAP(thisClass)
    MSG_WM_SIZE(OnSize)
    END_MSG_MAP()

    void OnSize(UINT wParam, const CSize &size)
    {
        if (IsLoaded())
        {
            RECT bounds;
            GetClientRect(&bounds);
            m_controller->put_Bounds(bounds);
        }
    }
};

IHtmlView *CreateWebView2View()
{
    return new CHtmlWebView2View();
}
