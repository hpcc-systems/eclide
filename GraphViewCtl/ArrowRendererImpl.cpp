#include "StdAfx.h"

#include "Canvas.h"
#include "ScaledWindows.h"
#include "Renderer.h"
#include "AggDC.h"
#include "XMLWriter.h"

class CArrowTipRenderer : public IArrowTipRenderer,	public CScaledWindow
{
    IGraphVertex* m_v;

    void Draw(CAggDC &dc);

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(IArrowTipRenderer)
    END_CUNKNOWN(CUnknown)

    CArrowTipRenderer(CCanvas& canvas); 

    // IRenderer

    CUniqueID GetID();
    void SetGraphItem(IGraphItem* item);
    void GetBoundsBox(GraphTypes::RectF& box, GraphTypes::Coordinates co=GraphTypes::screen);
    void SetBoundsBox(const GraphTypes::RectF& box, unsigned flags, GraphTypes::Coordinates co=GraphTypes::screen);
    void GetPosition(GraphTypes::PointF& pt, GraphTypes::Coordinates co);
    void SetPosition(const GraphTypes::PointF& pt, GraphTypes::Coordinates co);
    void SetSelected(bool bSelected=true, bool bAdd=false);
    void SetFocus(bool bSet=true);
    LRESULT SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    GraphTypes::SizeF CalcDisplaySizes(CDCHandle& dc,
        bool bForce=false,
        bool bIgnoreExtent=false);	
    void SetRedraw(bool bEnable);
    void ClearCaches();


    // IArrowTipRenderer
    void SetVertex(IGraphVertex* v);

protected:
    BEGIN_MSG_MAP(CArrowTipRenderer)
        MESSAGE_HANDLER(WM_PAINT, OnPaint)
    END_MSG_MAP()

    LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};

//*****************************************************************************

using namespace GraphTypes;

CArrowTipRenderer::CArrowTipRenderer(
    CCanvas & canvas)
    : CScaledWindow(&canvas)
{

}

//-----------------------------------------------------------------------------

CUniqueID CArrowTipRenderer::GetID()
{
    return m_v->GetID();
}

//-----------------------------------------------------------------------------

void CArrowTipRenderer::SetGraphItem(IGraphItem* item)
{
    m_v=static_cast<IGraphVertex*>(item);
}

//-----------------------------------------------------------------------------

LRESULT CArrowTipRenderer::SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return CScaledWindow::SendMessage(uMsg, wParam, lParam);
}

//-----------------------------------------------------------------------------

void CArrowTipRenderer::SetSelected(bool bSelected, bool bAdd)
{
    // nothing to do for now
}

//-----------------------------------------------------------------------------

void CArrowTipRenderer::SetFocus(bool bSet)
{
    // nothing to do for now
}

//-----------------------------------------------------------------------------

void CArrowTipRenderer::GetBoundsBox(RectF& box, GraphTypes::Coordinates co)
{
    CComQIPtr<ILegacyRenderer> il;
    CComQIPtr<IListWindowRenderer> lw;
    CComQIPtr<IDatasetRenderer> ds;
    CComQIPtr<IIOSubgraphRenderer> io;
    CComQIPtr<ITextboxRenderer> itb;
    CComPtr<IGraphSubgraph> g;
    RectF left;

    CComPtr<IRenderer> ir = m_canvas->GetRenderer(m_v);
    if(ir!=0)	
    {
        itb=ir.p;
        if(itb!=0)
        {
            g=m_v->GetParent();
            if(g!=0)
            {
                ir=m_canvas->GetRenderer(g);
                lw=ir.p;
                if(lw!=0)
                {
                    lw->GetConnectBounds(left, box, m_v->GetOrder());
                }
                else
                {
                    ds=ir.p;
                    if(ds!=0)
                        ds->GetConnectBounds(left, box, m_v->GetOrder());
                    else
                    {
                        io=ir.p;
                        if(io!=0)
                            io->GetConnectBounds(left, box, m_v->GetOrder());
                    }
                }
            }
        }
        else
        {
            il=ir.p;
            if(il!=0)
            {
                il->GetBoundsBox(box);	
            }
        }
    }
    if(co==world)
    {
        m_canvas->RAllTransform(box);
    }
}

//-----------------------------------------------------------------------------

void CArrowTipRenderer::SetBoundsBox(
    const GraphTypes::RectF& box, unsigned flags, GraphTypes::Coordinates co)
{
    CRect rc;
    if(co==world)
    {
        RectF r = box;
        m_canvas->AllTransform(r);
        rc = r;
    }
    else
    {
        rc = box;
    }
    SetWindowPos(rc, flags);
}

//-----------------------------------------------------------------------------

void CArrowTipRenderer::GetPosition(PointF& pt, Coordinates co/*=world*/)
{
    // nothing to do for now
}

//-----------------------------------------------------------------------------

void CArrowTipRenderer::SetPosition(const PointF& pt, Coordinates co/*=world*/)
{
    // nothing to do for now
}

//-----------------------------------------------------------------------------

SizeF CArrowTipRenderer::CalcDisplaySizes(
    CDCHandle& dc, 
    bool bForce,
    bool bIgnoreExtent)
{
    // nothing to do for now
    SizeF size;

    return size;
}

//-----------------------------------------------------------------------------

void CArrowTipRenderer::SetRedraw(bool bEnable)
{
    CScaledWindow::SetRedraw(bEnable);
}

//-----------------------------------------------------------------------------

void CArrowTipRenderer::ClearCaches()
{
}

//-----------------------------------------------------------------------------

void CArrowTipRenderer::Draw(CAggDC &dc) 
{
}

//-----------------------------------------------------------------------------

LRESULT CArrowTipRenderer::OnPaint(
    UINT /*uMsg*/,
    WPARAM wParam,
    LPARAM /*lParam*/,
    BOOL& /*bHandled*/)
{
    CAggDC &dc=*(CAggDC*)wParam;

    Draw(dc);	

    return 0;
}

//*****************************************************************************

IRenderer* CreateArrowTipRenderer(
    CCanvas& parent)
{
    CArrowTipRenderer * retVal = new CArrowTipRenderer(parent);
    return retVal;
}

//*****************************************************************************

class CArrowTailRenderer : public IArrowTailRenderer, public CScaledWindow
{
    IGraphVertex* m_v;

    void Draw(CAggDC &dc);

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(IArrowTailRenderer)
    END_CUNKNOWN(CScaledWindow)

    CArrowTailRenderer(CCanvas & canvas); 

    // IRenderer

    CUniqueID GetID();
    void SetGraphItem(IGraphItem* item);
    void GetBoundsBox(GraphTypes::RectF& box, GraphTypes::Coordinates co=GraphTypes::screen);
    void SetBoundsBox(const GraphTypes::RectF& box, unsigned flags, GraphTypes::Coordinates co=GraphTypes::screen);
    void GetPosition(GraphTypes::PointF& pt, GraphTypes::Coordinates co);
    void SetPosition(const GraphTypes::PointF& pt, GraphTypes::Coordinates co);
    void SetSelected(bool bSelected=true, bool bAdd=false);
    void SetFocus(bool bSet=true);
    LRESULT SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    GraphTypes::SizeF CalcDisplaySizes(CDCHandle& dc,
        bool bForce=false,
        bool bIgnoreExtent=false);	
    void SetRedraw(bool bEnable);
    void ClearCaches();

    // IArrowTailRenderer
    void SetColor(GraphTypes::Color color);

protected:
    BEGIN_MSG_MAP(CArrowTailRenderer)
        MESSAGE_HANDLER(WM_PAINT, OnPaint)
    END_MSG_MAP()

    LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};

//*****************************************************************************

CArrowTailRenderer::CArrowTailRenderer(
    CCanvas & canvas)
    : CScaledWindow(&canvas) 
{

}

//-----------------------------------------------------------------------------

CUniqueID CArrowTailRenderer::GetID()
{
    return m_v->GetID();
}

//-----------------------------------------------------------------------------

void CArrowTailRenderer::SetGraphItem(IGraphItem* item)
{
    m_v=static_cast<IGraphVertex*>(item);
}

//-----------------------------------------------------------------------------

LRESULT CArrowTailRenderer::SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return CScaledWindow::SendMessage(uMsg, wParam, lParam);
}

//-----------------------------------------------------------------------------

void CArrowTailRenderer::SetFocus(bool bSet)
{
    // nothing to do for now
}

//-----------------------------------------------------------------------------

void CArrowTailRenderer::SetSelected(bool bSelected, bool bAdd)
{
    // nothing to do for now
}

//-----------------------------------------------------------------------------

SizeF CArrowTailRenderer::CalcDisplaySizes(
    CDCHandle& dc, 
    bool bForce,
    bool bIgnoreExtent)
{
    // nothing to do for now
    SizeF size;

    return size;
}

//-----------------------------------------------------------------------------

void CArrowTailRenderer::GetBoundsBox(RectF& box, GraphTypes::Coordinates co)
{
    CComQIPtr<ILegacyRenderer> il;
    CComQIPtr<IListWindowRenderer> lw;
    CComQIPtr<ITextboxRenderer> itb;
    CComPtr<IGraphSubgraph> g;
    CComPtr<IRenderer> ir;
    RectF right;

    ir=m_canvas->GetRenderer(m_v);
    if(ir!=0)	
    {
        itb=ir.p;
        if(itb!=0)
        {
            g=m_v->GetParent();
            if(g!=0)
            {
                ir=m_canvas->GetRenderer(g);
                lw=ir.p;
                if(lw!=0)
                    lw->GetConnectBounds(box, right, m_v->GetOrder());
            }
        }
        else
        {
            il=ir.p;
            if(il!=0)
            {
                il->GetBoundsBox(box);	
            }
        }
    }
    if(co==world)
    {
        m_canvas->RAllTransform(box);
    }
}

//-----------------------------------------------------------------------------

void CArrowTailRenderer::SetBoundsBox(
    const GraphTypes::RectF& box, unsigned flags, GraphTypes::Coordinates co)
{
    CRect rc;
    if(co==world)
    {
        RectF r = box;
        m_canvas->AllTransform(r);
        rc = r;
    }
    else
    {
        rc = box;
    }
    SetWindowPos(rc, flags);
}

//-----------------------------------------------------------------------------

void CArrowTailRenderer::GetPosition(PointF& pt, Coordinates co/*=world*/)
{
    // nothing to do for now
}

//-----------------------------------------------------------------------------

void CArrowTailRenderer::SetPosition(const PointF& pt, Coordinates co/*=world*/)
{
    // nothing to do for now
}

//-----------------------------------------------------------------------------

void CArrowTailRenderer::SetRedraw(bool bEnable)
{
    CScaledWindow::SetRedraw(bEnable);
}

//-----------------------------------------------------------------------------

void CArrowTailRenderer::ClearCaches()
{

}

//-----------------------------------------------------------------------------

void CArrowTailRenderer::Draw(CAggDC &dc) 
{

}

//-----------------------------------------------------------------------------

LRESULT CArrowTailRenderer::OnPaint(
    UINT /*uMsg*/,
    WPARAM wParam,
    LPARAM /*lParam*/,
    BOOL& /*bHandled*/)
{
    CAggDC &dc=*(CAggDC*)wParam;

    Draw(dc);	

    return 0;
}

//*****************************************************************************

IRenderer* CreateArrowTailRenderer(CCanvas& parent)
{
    CArrowTailRenderer * retVal = new CArrowTailRenderer(parent);
    return retVal;
}
