#pragma once

// To use it, derive from it and chain it in the message map.
// Make sure to set bHandled to FALSE when handling WM_MOUSEHOVER message!
#define MIN_TIP_WIDTH 200

template< class T >
class CMouseTooltip
{
public:   
    WTL::CToolTipCtrl m_toolTip;
    bool m_bFirst;
    bool m_bMouseTracking;
    bool m_bToolTipShowing;
    CPoint m_tooltipMousePoint;
    CPoint m_movePos;

    CMouseTooltip() : m_bMouseTracking(false), m_bFirst(true), m_bToolTipShowing(false), m_movePos(-1,-1)
    {
    }

    void StartMouseTracking()
    {
        if (m_bFirst)
        {
            m_bFirst = false;
            T* pT = static_cast<T*>(this);
            m_toolTip.Create(pT->m_hWnd);

            WTL::CToolInfo ti(TTF_TRACK, pT->m_hWnd, 1);
            m_toolTip.AddTool(&ti);
        }

        if (!m_bMouseTracking)
        {
            T* pT = static_cast<T*>(this);

            m_bMouseTracking = true;
            TRACKMOUSEEVENT tracker;
            tracker.cbSize = sizeof(tracker);
            tracker.dwFlags = TME_LEAVE | TME_HOVER;
            tracker.hwndTrack = pT->m_hWnd;
            tracker.dwHoverTime = m_toolTip.GetDelayTime(TTDT_INITIAL);
            _TrackMouseEvent(&tracker);
        }
    }

    BOOL StopMouseTracking(bool stopHoverTracking=false)
    {
        T* pT = static_cast<T*>(this);
        m_bMouseTracking = false;
        TRACKMOUSEEVENT tme = { 0 };
        tme.cbSize = sizeof(tme);
        tme.dwFlags = TME_LEAVE | (stopHoverTracking?TME_HOVER:0) | TME_CANCEL;
        tme.hwndTrack = pT->m_hWnd;
        return _TrackMouseEvent(&tme);
    }

    void ShowToolTip(const std::_tstring & text)
    {
        T* pT = static_cast<T*>(this);
        ::GetCursorPos(&m_tooltipMousePoint);
        CWindowDC dcWindow(NULL);
        int screenWidth = dcWindow.GetDeviceCaps(HORZRES);

        CPoint mousePos;
        ::GetCursorPos(&mousePos);

        CPoint windowPos = mousePos;
        pT->ScreenToClient(&windowPos);

        TOOLINFO ti;
        ti.cbSize = sizeof(ti);
        ti.hwnd   = pT->m_hWnd;
        ti.uId    = 1;
        ti.lpszText = const_cast<TCHAR *>(text.c_str());

        int xpos = mousePos.x + 15;
        int width = screenWidth - xpos;
        if (width < MIN_TIP_WIDTH)
        {
            xpos = screenWidth - MIN_TIP_WIDTH;
            width = MIN_TIP_WIDTH;
        }

        m_toolTip.TrackPosition(xpos, mousePos.y + 12);
        m_toolTip.SetMaxTipWidth(width);
        m_toolTip.UpdateTipText(&ti);
        if (!m_bToolTipShowing)
        {
            m_toolTip.TrackActivate(&ti, TRUE);
            m_bToolTipShowing = true;
        }
    }

    void HideToolTip(bool bForce)
    {
        if ( m_bToolTipShowing )
        {
            T* pT = static_cast<T*>(this);
            //since we can get some bogus mouse move events, check to see if
            //if has really moved
            POINT pnt;
            ::GetCursorPos(&pnt);
            if (bForce || std::abs(pnt.x - m_tooltipMousePoint.x) > 3 || std::abs(pnt.y - m_tooltipMousePoint.y) > 3)
            {
                TOOLINFO ti;
                ti.cbSize = sizeof(ti);
                ti.hwnd   = pT->m_hWnd;
                ti.uId    = 1;
                m_toolTip.TrackActivate(&ti, FALSE);
                m_bToolTipShowing = false;
            }
        }
    }

    BEGIN_MSG_MAP(CMouseTooltip)
        MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
        MESSAGE_HANDLER(WM_SYSKEYDOWN, OnSysKeyDown)
        MESSAGE_HANDLER(WM_RBUTTONDOWN, OnKeyDown)
        MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
        MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
        MESSAGE_HANDLER(WM_MOUSEHOVER, OnMouseHover)
    END_MSG_MAP()

    LRESULT OnSysKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        bHandled = false;
        StopMouseTracking(true); // no HOVER tracking on Sys keys like Alt+Tab etc.
        HideToolTip(true);
        return 0;
    }

    LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        bHandled = false;
        HideToolTip(true);
        return 0;
    }

    LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
    {
        CPoint pos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        bHandled = false;
        HideToolTip(false);
        // the problem with starting tracking on WM_MOUSEMOVE is that message gets
        // sent by Windows in some cases (ALT+Tab app switch) even if you haven't touched
        // the mouse
        if(pos!=m_movePos)
            StartMouseTracking();
        m_movePos=pos;
        return 0;
    }

    LRESULT OnMouseHover(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        bHandled = false;
        T* pT = static_cast<T*>(this);
        StopMouseTracking();
        if ( (wParam & (MK_SHIFT | MK_CONTROL)) == 0 )
        {
            pT->ShowToolTip(CPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
        }
        return 0;
    }

    LRESULT OnMouseLeave(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        bHandled = false;
        StopMouseTracking();
        return 0;
    }
};
