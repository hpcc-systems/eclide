#pragma once

#include <atlwinmisc.h>

__interface IOverviewRenderer;
class CGraphViewCanvas;

typedef CWinTraits<WS_POPUP | WS_CAPTION | WS_THICKFRAME | WS_SYSMENU, WS_EX_TOOLWINDOW> COverviewWndTraits;
class COverviewWnd :
    public CWindowImpl<COverviewWnd, CTransparentWindow, COverviewWndTraits>
{
    CComPtr<IOverviewRenderer> m_or;
    CGraphViewCanvas* m_canvas;
    bool m_layered;
    BYTE m_opacity;
    CPoint m_capturePos;
    GraphTypes::PointF	m_savePos;

public:
    COverviewWnd();
    ~COverviewWnd();

    void Init(CGraphViewCanvas* canvas, IGraph* graph);
    bool IsLayered() const { return m_layered; }
    BYTE SetOverviewOpacity(BYTE opacity);
    void SetOverviewPosition(const CPoint& pos);
    void SetOverviewSize(const CSize& size); 
    void Refresh();

protected:
    BEGIN_MSG_MAP_EX(COverviewWnd)
        MSG_WM_SIZE(OnSize)
        MSG_WM_CLOSE(OnClose)
        MSG_WM_NCHITTEST(OnNcHitTest)
        MSG_WM_SETCURSOR(OnSetCursor)
        MSG_WM_LBUTTONDOWN(OnLButtonDown)
        MSG_WM_LBUTTONUP(OnLButtonUp)
        MSG_WM_MOUSEMOVE(OnMouseMove)
        MESSAGE_HANDLER(WM_PAINT, OnPaint)
        MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
    END_MSG_MAP()

    void OnSize(UINT wParam, const CSize & size);
    void OnClose(); 
    int OnNcHitTest(CPoint pt);
    bool OnSetCursor(HWND, UINT htcode, UINT uMsg);
    void OnLButtonDown(UINT code, CPoint pt);
    void OnLButtonUp(UINT code, CPoint pt);
    void OnMouseMove(UINT code, CPoint pt);
    LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};

