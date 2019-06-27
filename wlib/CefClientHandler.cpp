#include "stdafx.h"

#include "CefClientHandler.h"
#include "include/wrapper/cef_helpers.h"
#include "logger.h"

CefClientHandler::CefClientHandler(ICefDelegate* delegate) : m_delegate(delegate)
{
}

CefClientHandler::~CefClientHandler()
{
}

void CefClientHandler::CreateBrowser(CefWindowInfo const & info, CefBrowserSettings const & settings, CefString const & url)
{
    _DBGLOG(LEVEL_DEBUG, _T("CefClientHandler::CreateBrowser"));
    CefBrowserHost::CreateBrowser(info, this, url, settings, nullptr);
}

bool CefClientHandler::IsLoaded() {
    return m_Browser != nullptr;
}

void CefClientHandler::OnAddressChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url)
{
    CEF_REQUIRE_UI_THREAD();
    _DBGLOG(LEVEL_DEBUG, (boost::_tformat(_T("CefClientHandler::OnAddressChange - %1%")) % browser->GetIdentifier()).str().c_str());

    // Only update the address for the main (top-level) frame.
    if (frame->IsMain())
    {
        if (m_delegate != nullptr)
            m_delegate->OnSetAddress(url);
    }
}

void CefClientHandler::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title)
{
    CEF_REQUIRE_UI_THREAD();
    _DBGLOG(LEVEL_DEBUG, (boost::_tformat(_T("CefClientHandler::OnTitleChange - %1%")) % browser->GetIdentifier()).str().c_str());

    if (m_delegate != nullptr)
        m_delegate->OnSetTitle(title);
}

void CefClientHandler::OnFullscreenModeChange(CefRefPtr<CefBrowser> browser, bool fullscreen)
{
    CEF_REQUIRE_UI_THREAD();
    _DBGLOG(LEVEL_DEBUG, (boost::_tformat(_T("CefClientHandler::OnFullscreenModeChange - %1%")) % browser->GetIdentifier()).str().c_str());

    if (m_delegate != nullptr)
        m_delegate->OnSetFullscreen(fullscreen);
}

void CefClientHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
    CEF_REQUIRE_UI_THREAD();
    _DBGLOG(LEVEL_DEBUG, (boost::_tformat(_T("CefClientHandler::OnAfterCreated - %1%")) % browser->GetIdentifier()).str().c_str());

    if (!m_Browser.get()) {
        // Keep a reference to the main browser.
        m_Browser = browser;
        m_BrowserId = browser->GetIdentifier();
    }

    if (m_delegate != nullptr)
        m_delegate->OnBrowserCreated(browser);
}

bool CefClientHandler::DoClose(CefRefPtr<CefBrowser> browser)
{
    CEF_REQUIRE_UI_THREAD();
    _DBGLOG(LEVEL_DEBUG, (boost::_tformat(_T("CefClientHandler::OnBeforeClose - %1%")) % browser->GetIdentifier()).str().c_str());
    // Closing the main window requires special handling. See the DoClose()
    // documentation in the CEF header for a detailed description of this
    // process.
    if (m_BrowserId == browser->GetIdentifier()) {
        // Set a flag to indicate that the window close should be allowed.
        m_bIsClosing = true;
    }

    // Allow the close. For windowed browsers this will result in the OS close
    // event being sent.
    if (m_delegate != nullptr)
        m_delegate->OnBrowserClosing(browser);

    return false;
}

void CefClientHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
    CEF_REQUIRE_UI_THREAD();
    _DBGLOG(LEVEL_DEBUG, (boost::_tformat(_T("CefClientHandler::OnBeforeClose - %1%")) % browser->GetIdentifier()).str().c_str());
    
    m_bIsClosing = true;

    if (m_delegate != nullptr)
        m_delegate->OnBrowserClosed(browser);

    if (m_BrowserId == browser->GetIdentifier()) {
        // Free the browser pointer so that the browser can be destroyed.
        m_Browser = nullptr;
    }
}

void CefClientHandler::OnLoadError(CefRefPtr<CefBrowser> browser,CefRefPtr<CefFrame> frame, ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl)
{
    CEF_REQUIRE_UI_THREAD();
    _DBGLOG(LEVEL_DEBUG, (boost::_tformat(_T("CefClientHandler::OnLoadError - %1%")) % browser->GetIdentifier()).str().c_str());

    // Don't display an error for downloaded files.
    if (errorCode == ERR_ABORTED)
        return;

    // Display a load error message.
    std::stringstream ss;
    ss << "<html><body bgcolor=\"white\">"
        "<h2>Failed to load URL " << std::string(failedUrl) <<
        " with error " << std::string(errorText) << " (" << errorCode <<
        ").</h2></body></html>";
    frame->LoadString(ss.str(), failedUrl);
}

void CefClientHandler::OnLoadingStateChange(CefRefPtr<CefBrowser> browser, bool isLoading, bool canGoBack, bool canGoForward)
{
    CEF_REQUIRE_UI_THREAD();
    _DBGLOG(LEVEL_DEBUG, (boost::_tformat(_T("CefClientHandler::OnLoadingStateChange - %1%")) % browser->GetIdentifier()).str().c_str());

    if (m_delegate != nullptr)
        m_delegate->OnSetLoadingState(isLoading, canGoBack, canGoForward);
}

CefRequestHandler::ReturnValue CefClientHandler::OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, CefRefPtr<CefRequestCallback> callback)
{
    _DBGLOG(LEVEL_DEBUG, (boost::_tformat(_T("CefClientHandler::OnBeforeResourceLoad - %1%")) % browser->GetIdentifier()).str().c_str());
    if (m_delegate != nullptr)
        m_delegate->OnBeforeResourceLoad(browser, frame, request, callback);

    return CefRequestHandler::OnBeforeResourceLoad(browser, frame, request, callback);
}

void CefClientHandler::DetachDelegate()
{
    _DBGLOG(LEVEL_DEBUG, _T("CefClientHandler::DetachDelegate"));
    m_delegate = nullptr;
}
