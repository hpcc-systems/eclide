#pragma once

#include "wlib.h"

__interface IHtmlView {
    bool IsLoaded();
    bool BackEnabled() const;
    bool ForwardEnabled() const;
    HRESULT Navigate(const TCHAR* pszUrl, bool newWindow = false);
    void CreateHTMLControl(const TCHAR* pszUrl, const TCHAR* pszUserID, const TCHAR* pszPassword);
    void BrowserBack();
    void BrowserForward();
    void BrowserStop();
    void BrowserRefresh();
    void BrowserCmdCopy();
    void BrowserCmdSelectAll();
    HWND Create(HWND hWndParent, _U_RECT rect = NULL, LPCTSTR szWindowName = NULL, DWORD dwStyle = 0, DWORD dwExStyle = 0);
    BOOL PreTranslateMessage(MSG* pMsg);
    BOOL SetWindowPos(HWND hWndInsertAfter, int x, int y, int cx, int cy, UINT nFlags) throw();
    HRESULT put_ServiceProvider(IUnknown* pUnkSite);
    HRESULT put_ProfferService(REFGUID rguidService, IServiceProvider* pServiceProvider);
};

WLIB_API IHtmlView * CreateIEView();
WLIB_API IHtmlView * CreateChromiumView();
