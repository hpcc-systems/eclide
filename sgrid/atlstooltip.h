#pragma once

#include "sdkstooltip.h"

namespace STOOLTIP
{
//------------------------------------------------------------------------------
template< class TBase >
class CTooltipCtrlT : public TBase
{
public:
    CTooltipCtrlT(HWND hWnd = NULL) : TBase(hWnd) { }

    CTooltipCtrlT< TBase >& operator=(HWND hWnd)
    {
        m_hWnd = hWnd;
        return *this;
    }

    HWND Create(HWND hWndParent, ATL::_U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
        DWORD dwStyle = 0, DWORD dwExStyle = 0,
        ATL::_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL)
    {
        if(rect.m_lpRect == NULL)
            rect.m_lpRect = &rcDefault;
        m_hWnd = ::CreateWindowEx(dwExStyle, GetWndClassName(), szWindowName, dwStyle, rect.m_lpRect->left, rect.m_lpRect->top, (rect.m_lpRect->right - rect.m_lpRect->left) / 2, (rect.m_lpRect->bottom - rect.m_lpRect->top) / 2, hWndParent, MenuOrID.m_hMenu, NULL, lpCreateParam);
        return m_hWnd;
    }

    BOOL GetToolInfo(LPTOOLINFO lpToolInfo) const
    {
        ATLASSERT(::IsWindow(m_hWnd));
        return (BOOL)::SendMessage(m_hWnd, TTM_GETTOOLINFO, 0, (LPARAM)lpToolInfo);
    }

    BOOL GetToolInfo(HWND hWnd, UINT nIDTool, UINT* puFlags, LPRECT lpRect, LPTSTR lpstrText) const
    {
        ATLASSERT(::IsWindow(m_hWnd));
        ATLASSERT(hWnd != NULL);
        CToolInfo ti(0, hWnd, nIDTool, NULL, lpstrText);
        BOOL bRet = (BOOL)::SendMessage(m_hWnd, TTM_GETTOOLINFO, 0, ti);
        if(bRet)
        {
            *puFlags = ti.uFlags;
            memcpy(lpRect, &(ti.rect), sizeof(RECT));
        }
        return bRet;
    }

    void SetToolInfo(LPTOOLINFO lpToolInfo)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        ::SendMessage(m_hWnd, TTM_SETTOOLINFO, 0, (LPARAM)lpToolInfo);
    }

    void SetToolRect(LPTOOLINFO lpToolInfo)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        ::SendMessage(m_hWnd, TTM_NEWTOOLRECT, 0, (LPARAM)lpToolInfo);
    }

    void SetToolRect(HWND hWnd, UINT nIDTool, LPCRECT lpRect)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        ATLASSERT(hWnd != NULL);
        ATLASSERT(nIDTool != 0);

        CToolInfo ti(0, hWnd, nIDTool, (LPRECT)lpRect, NULL);
        ::SendMessage(m_hWnd, TTM_NEWTOOLRECT, 0, ti);
    }

    BOOL AddTool(LPTOOLINFO lpToolInfo)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        return (BOOL)::SendMessage(m_hWnd, TTM_ADDTOOL, 0, (LPARAM)lpToolInfo);
    }

    BOOL AddTool(HWND hWnd, ATL::_U_STRINGorID text = LPSTR_TEXTCALLBACK, LPCRECT lpRectTool = NULL, UINT nIDTool = 0)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        ATLASSERT(hWnd != NULL);
        // the toolrect and toolid must both be zero or both valid
        ATLASSERT((lpRectTool != NULL && nIDTool != 0) || (lpRectTool == NULL && nIDTool == 0));

        CToolInfo ti(0, hWnd, nIDTool, (LPRECT)lpRectTool, (LPTSTR)text.m_lpstr);
        return (BOOL)::SendMessage(m_hWnd, TTM_ADDTOOL, 0, ti);
    }

    void DelTool(LPTOOLINFO lpToolInfo)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        ::SendMessage(m_hWnd, TTM_DELTOOL, 0, (LPARAM)lpToolInfo);
    }

    void DelTool(HWND hWnd, UINT nIDTool = 0)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        ATLASSERT(hWnd != NULL);

        CToolInfo ti(0, hWnd, nIDTool, NULL, NULL);
        ::SendMessage(m_hWnd, TTM_DELTOOL, 0, ti);
    }
    //  -----------------------------------------------------------------------

    static HMODULE LoadDLL()
    {
        return ::LoadLibrary(GetLibraryName());
    }

    static LPCTSTR GetWndClassName()
    {
        return _T("SGRID_Tooltip");
    }

    static LPCTSTR GetLibraryName()
    {
        return _T("SGRID.DLL");
    }
};

typedef CTooltipCtrlT<CWindow> CTooltipCtrl;
} // namespace STOOLTIP