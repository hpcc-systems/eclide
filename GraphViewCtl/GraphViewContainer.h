#pragma once

#include "clib.h"
#include "GraphViewCtlApi.h"
#include "Notifications.h"
#include "atlGraphView.h"

//-----------------------------------------------------------------------------

class CGraphViewCanvas;
class CTimelineCtrl;
class COverviewWnd;

namespace SGV
{
typedef CWinTraits<WS_CHILD | WS_VISIBLE /*| WS_CLIPCHILDREN | WS_CLIPSIBLINGS*/, WS_EX_CLIENTEDGE> CGraphWinTraits;
class CGraphView : 
    public CWindowImpl<CGraphView, CWindow, CGraphWinTraits>,
    public CUnknown
{
    typedef CGraphView thisclass;
    typedef CWindowImpl<CGraphView, CWindow, CGraphWinTraits> baseClass;

    StlLinked<CGraphViewCanvas> m_canvas;
    CTimelineCtrl* m_timeline;
    COverviewWnd* m_overview;

    static bool DoRegister(const TCHAR * className, HINSTANCE hInstance);
    BOOL PassToChildren(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult);
public:
    static HINSTANCE hInstance;

    IMPLEMENT_CUNKNOWN;

    CGraphView();
    virtual ~CGraphView();

    static bool Register();
    static bool Register(HINSTANCE hInstance_);
    static bool Unregister();
    static LONG_PTR PASCAL SWndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LONG_PTR lParam);

    BEGIN_MSG_MAP_EX(thisClass)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_DESTROY(OnDestroy)
        MSG_WM_SIZE(OnSize)
        MSG_WM_ERASEBKGND(OnEraseBkgnd)
        NOTIFY_CODE_HANDLER(SCW_NOTIFY_TIMELINE_RCLICK, OnNotifyTimelineRClick)
        NOTIFY_CODE_HANDLER(SCW_NOTIFY_TIMELINE_DBLCLK, OnNotifyTimelineDblClk)
        if(PassToChildren(hWnd, uMsg, wParam, lParam, lResult))
            return TRUE;
    END_MSG_MAP()

    LRESULT OnCreate(LPCREATESTRUCT lParam);
    void OnDestroy();
    void OnSize(UINT wParam, const CSize & size);
    LRESULT OnEraseBkgnd(HDC);
    LRESULT OnNotifyTimelineRClick(WPARAM /*wParam*/, LPNMHDR lpnmhdr, BOOL& /*bHandled*/);
    LRESULT OnNotifyTimelineDblClk(WPARAM /*wParam*/, LPNMHDR lpnmhdr, BOOL& /*bHandled*/);
};

}; // namespace SGV
