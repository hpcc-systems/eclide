#pragma once

#include "Canvas.h"
#include "Renderer.h"
#include "ScaledWindows.h"
#include "ScaledScrollBar.h"

class CScaledList: public CScaledWindow
{
    IGraphSubgraph* m_g;
    CPartTheme& m_Theme;
    CSize m_border;
    size_t	m_numitems;
    CRect m_rcTextArea;
    CScaledScrollBar m_vSB;
    int m_topidx;
    int m_selectedidx;
    int m_visibleRows;
    int m_whDividerLine;

protected:
    void DrawListBorder(CDCHandle& dc, BORDER_STYLE style);
    void DrawListItems(WPARAM wParam);

    LRESULT ForwardMsgToChildren(UINT uMsg, WPARAM wParam, LPARAM lParam);
    int CalcVisibleItems(int height);
    void AdjustItemsBounds();
    IRenderer* GetItem(int pos) const;

public:
    enum eLocation
    {
        top,
        bottom,
        middle
    };

public:
    CScaledList(CCanvas* canvas);

    void SetGraphItem(IGraphItem* item);
    CSize CalcListMaxSize(CDCHandle& dc);
    void SetWindowPos(const CRect& pos, UINT flags);
    virtual void Invalidate(CRect* rc=0, bool bErase=false) 
    { 
        m_canvas->Invalidate(rc,bErase);
    }
    int GetRowHeight(unsigned row=0) const;
    void SetFocus(bool bSet=true);
    int SetTopIndex(int index);
    int GetTopIndex() const;
    bool HitTest(const CPoint& p) const;
    IGraphItem* HitTestItems(const GraphTypes::PointF& point) const;
    eLocation GetConnectBounds(GraphTypes::RectF& box, unsigned order) const;
    bool OnEditItem(IGraphItem* gi);

protected:
    BEGIN_MSG_MAP(CScaledList)
        MESSAGE_HANDLER(WM_PAINT, OnPaint)
        MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
        MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
        MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
        MESSAGE_HANDLER(WM_VSCROLL, OnVScroll)
        MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
        MESSAGE_HANDLER(WM_NCHITTEST, OnNcHitTest)
        MESSAGE_HANDLER(LBN_LABELEDIT, OnLabelEdit)
    END_MSG_MAP()

    LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnVScroll(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnNcHitTest(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnLabelEdit(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};