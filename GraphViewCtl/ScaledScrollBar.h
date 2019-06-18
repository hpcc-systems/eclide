#pragma once

#include "ScaledWindows.h"
#include "Canvas.h"

class CScaledScrollBar : public CScaledWindow
{
    CPartTheme& m_Theme;
    CRect m_rcULAB; // Up/Left Arrow Button
    CRect m_rcDRAB; // Down/Right Arrow Button
    CRect m_rcThumb;
    HIT_TEST_CODE m_capturedPart;
    SCROLLINFO si;
    bool m_bVisible;
    bool m_bVertical;
    UINT m_TimerCode;
    static CBrushHandle m_lightTrackBrush;

    // dragging vars
    bool m_bDragging;
    int m_dragPosOld, m_dragPosNew, m_dragThumbOffset;
    int	m_dragPix, m_dragPixOld;

    // workspace vars
    int wtop, wtopArrow, wminpix, wtopThumb, wbottomThumb, wmaxpix,
        wbottomArrow, wbottom, wthumbsize; 
    CButtonState m_stateULAB;
    CButtonState m_stateDRAB;
    CButtonState m_stateULTrack;
    CButtonState m_stateDRTrack;
    CButtonState m_stateThumb;

    HIT_TEST_CODE HitTestParts(const CPoint& p);
    CButtonState* Code2State(HIT_TEST_CODE code);
    void CalcWorkVars(void);
    int PosFromPix(int pix);
    LRESULT SendScrollMsg(int scrollMsg);

public:
    CScaledScrollBar(CCanvas* canvas, CScaledWindow* parent=0, bool bVert=true);
    ~CScaledScrollBar();

    void SetWindowPos(const CRect& pos, UINT flags);
    int DrawDecodeState(HIT_TEST_CODE htcode, bool bTheme);
    bool HitTest(const CPoint& p);
    virtual void Invalidate(CRect* rc=0, bool bErase=false) 
    { 
        m_canvas->Invalidate(rc,bErase);
    }

    int SetScrollInfo(SCROLLINFO* lpsi,	bool fRedraw);
    bool GetScrollInfo(SCROLLINFO* lpsi);
    void SetScrollPos(int pos, bool fRedraw);
    bool IsVisible(void) { return m_bVisible; }


protected:
    BEGIN_MSG_MAP(CScaledScrollBar)
        MESSAGE_HANDLER(WM_PAINT, OnPaint)
        MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
        MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
        MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
        MESSAGE_HANDLER(WM_TIMER, OnTimer)
    END_MSG_MAP()

    LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};
