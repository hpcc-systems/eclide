
#include "gdiimpl.h"
#include "VertexRenderer.h"
#include "ScaledWindows.h"

class CGraphRenderer 
    : public IGraphRenderer, public CUnknown, public CScaledWindow
{
    friend class CListWindowRenderer;

protected:
    CGraphFastRenderImpl & m_renderParent;
    CComPtr<IGraph> m_graph;

    bool DispatchMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult);
    bool HandleCapture(UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
    IMPLEMENT_CUNKNOWNQI(IGraphRenderer);

    CGraphRenderer(CGraphFastRenderImpl & parent, IGraph * graph);

    // begin old IRenderer support
    static GraphTypes::RectF rs;

    CUniqueID GetID();
    void GetBoundsBox(GraphTypes::RectF& box);
    const GraphTypes::RectF & GetTitlePos();
    void SetTitlePos(const GraphTypes::RectF & r);
    const GraphTypes::RectF & GetIconPos();
    void SetIconPos(const GraphTypes::RectF & r);
    const GraphTypes::RectF & GetImagePos();
    void SetImagePos(const GraphTypes::RectF & r);
    bool IsVisible();
    bool IsSelected();
    void PaintNormVertex(CDCHandle &dc);
    void PaintSelVertex(CDCHandle &dc);
    void PaintOverVertex(CDCHandle &dc);
    void SetFocus(bool bSet=true);
    LRESULT SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    GraphTypes::SizeF CalcDisplaySizes(CDCHandle& dc, bool bForce, bool bIgnoreExtent);
    // end old IRenderer support

    // IBaseRenderer

    // IGraphRenderer
    IRenderer* CreateRenderer(IGraphSubgraph * g);
    IRenderer* CreateRenderer(IGraphVertex * v);

    void SetTopParent(HWND hwnd);

protected:
    BEGIN_MSG_MAP(CGraphRenderer)
        MESSAGE_HANDLER(WM_PAINT, OnPaint)
        MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
        MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
        MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
        MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
        MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnMouseDblclk);
        MESSAGE_HANDLER(WM_TIMER, OnTimer);
    END_MSG_MAP()

    LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnMouseDblclk(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};
