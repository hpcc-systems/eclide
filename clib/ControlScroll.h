#pragma once

#include "clib.h"

__interface IScrollMsg
{
    bool OnScroll(HWND control, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

 class CWindowControlScrollExDeprecated : public CWindowImpl<CWindowControlScrollExDeprecated>
{
    typedef CWindowImpl<CWindowControlScrollExDeprecated> baseClass;

public:

    void SetNotificationHandler(IScrollMsg *notify)
    {
        m_Notify = notify;
    }

    baseClass& operator=(HWND hWnd)
    {
        if ( hWnd == 0 )
            baseClass::UnsubclassWindow();
        else
            baseClass::SubclassWindow(hWnd);
        return *this;
    }

    BEGIN_MSG_MAP(CWindowControlScrollExDeprecated)
        if (uMsg == WM_VSCROLL || uMsg == WM_MOUSEWHEEL)// || uMsg == WM_KEYDOWN || uMsg == WM_KEYUP)
        {	
            bHandled = true;
            m_Notify->OnScroll(HWND(lParam), uMsg, wParam, lParam); 
            lResult = 0;
        }
        else if (uMsg == WM_HSCROLL)
        {	
            m_Notify->OnScroll(HWND(lParam), uMsg, wParam, lParam); 
            lResult = 0;
        }
    END_MSG_MAP()

protected:
    IScrollMsg *m_Notify;
};


