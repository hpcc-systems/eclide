#pragma once

#include "wlib.h"

#include "include/base/cef_lock.h"
#include "include/cef_client.h"

__interface ICefDelegate
{
    // Called when the browser is created.
    void OnBrowserCreated(CefRefPtr<CefBrowser> browser);

    // Called when the browser is closing.
    void OnBrowserClosing(CefRefPtr<CefBrowser> browser);

    // Called when the browser has been closed.
    void OnBrowserClosed(CefRefPtr<CefBrowser> browser);

    // Set the window URL address.
    void OnSetAddress(std::string const & url);

    // Set the window title.
    void OnSetTitle(std::string const & title);

    // Set fullscreen mode.
    void OnSetFullscreen(bool const fullscreen);

    // Set the loading state.
    void OnSetLoadingState(bool const isLoading, bool const canGoBack, bool const canGoForward);

    // Called before server request (set headers) 
    void OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, CefRefPtr<CefRequestCallback> callback);
};

class WLIB_API CefClientHandler : public CefClient,
    public CefDisplayHandler,
    public CefLifeSpanHandler,
    public CefLoadHandler,
    public CefRequestHandler
{

public:
    // Implement this interface to receive notification of ClientHandler
    // events. The methods of this class will be called on the main thread.

private:
    ICefDelegate* m_delegate;
    CefRefPtr<CefBrowser> m_Browser;
    int m_BrowserId = 0;
    bool m_bIsClosing = false;

public:
    CefClientHandler(ICefDelegate* delegate);
    ~CefClientHandler();

    void CreateBrowser(CefWindowInfo const & info, CefBrowserSettings const & settings, CefString const & url);
    bool IsLoaded();
    CefRefPtr<CefBrowser> GetBrowser() { return m_Browser; }
    bool IsClosing() { return m_bIsClosing; }

    // CefClient methods:
    virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() override { return this; }
    virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override { return this; }
    virtual CefRefPtr<CefLoadHandler> GetLoadHandler() override { return this; }
    virtual CefRefPtr<CefRequestHandler> GetRequestHandler() override { return this; }

    // CefDisplayHandler methods:
    virtual void OnAddressChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url) override;
    virtual void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) override;
    virtual void OnFullscreenModeChange(CefRefPtr<CefBrowser> browser, bool fullscreen) override;

    // CefLifeSpanHandler methods:
    virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
    virtual bool DoClose(CefRefPtr<CefBrowser> browser) override;
    virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

    // CefLoadHandler methods:
    virtual void OnLoadingStateChange(CefRefPtr<CefBrowser> browser,bool isLoading,bool canGoBack,bool canGoForward) override;
    virtual void OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl) override;

    // CefRequestHandler methods:
    virtual CefRequestHandler::ReturnValue OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, CefRefPtr<CefRequestCallback> callback);

    // This object may outlive the Delegate object so it's necessary for the
    // Delegate to detach itself before destruction.
    void DetachDelegate();

private:

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(CefClientHandler);
    // Include the default locking implementation.
    IMPLEMENT_LOCKING(CefClientHandler);
};
