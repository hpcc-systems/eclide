#include "StdAfx.h"

#include <algorithm>

#include "Canvas.h"
#include "ScaledWindows.h"
#include "AggDC.h"
#include "XMLWriter.h"

#ifdef __ATTVISG_TEST__
#include "AttVisGI.h"
// in neato.cpp
extern bool ShortestRoute(
    AttVisG::NPolygon& polys,
    const GraphTypes::PointF& src,
    const GraphTypes::PointF& dst,
    AttVisG::Polygon& route);
#endif // __ATTVISG_TEST__

class CEdgeRenderer : public IEdgeRenderer,	public CScaledWindow
{
    struct rcxorder
    {
        int item; // 0 or 1 for source or target vertex rectangle
        GraphTypes::RectF rc;  // left or right connect rectangles
    };

    typedef std::vector<rcxorder> vRectXOrder;

    struct Compare
    {
        bool operator() (const rcxorder& lhs, const rcxorder& rhs)
        {
            return lhs.rc.x<rhs.rc.x;
        }
    };

    IGraphEdge* m_edge;
    CComPtr<IArrowTipRenderer> m_arrowTip;
    CComPtr<IArrowTailRenderer> m_arrowTail;
    CComPtr<IImageRenderer> m_image;
    GraphTypes::PointFVector m_vp; // points are kept in screen/device coordinates
    mutable GraphTypes::PointFVector m_svp; // cached screen coordinates from last CalcVisbility
    GraphTypes::RectF m_rcLabel; // cached in screen coordinates from the last draw	

    void Draw(CAggDC& dc);
    void DrawLabel(CAggDC& dc);

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(IEdgeRenderer)
        IMPLEMENT_INTERFACE(IRenderer)
    END_CUNKNOWN(CScaledWindow)

    CEdgeRenderer(CCanvas& canvas); 

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

    // IEdgeRenderer
    bool GetConnectionPoints(GraphTypes::PointF& src, GraphTypes::PointF& tgt,
        GraphTypes::Coordinates co=GraphTypes::screen,
        GraphTypes::PointF* srcoff=0, GraphTypes::PointF* tgtoff=0);
    unsigned GetPoints(GraphTypes::PointFVector& vp, GraphTypes::Coordinates co);
    void SetPoints(const GraphTypes::PointFVector& vp, GraphTypes::Coordinates co);
    void GetLabelPos(GraphTypes::RectF& pos, GraphTypes::Coordinates co=GraphTypes::screen);
    void SetLabelPos(const GraphTypes::RectF& pos, GraphTypes::Coordinates co=GraphTypes::screen);
    bool IsSelected()
    {
        return CScaledWindow::IsSelected();
    }

protected:
    BEGIN_MSG_MAP(CEdgeRenderer)
        MESSAGE_HANDLER(WM_PAINT, OnPaint)
    END_MSG_MAP()

    LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};

//*****************************************************************************

using namespace GraphTypes;

CEdgeRenderer::CEdgeRenderer(CCanvas& canvas)
    : CScaledWindow(&canvas)
{
    CComPtr<IUnknown> unk=CreateArrowTipRenderer(canvas);
    CComQIPtr<IArrowTipRenderer> tip=unk.p;
    m_arrowTip=tip;

    unk=CreateArrowTailRenderer(canvas);
    CComQIPtr<IArrowTailRenderer> tail=unk.p;
    m_arrowTail=tail;
}

//-----------------------------------------------------------------------------

CUniqueID CEdgeRenderer::GetID()
{
    return m_edge->GetID();
}

//-----------------------------------------------------------------------------

void CEdgeRenderer::SetGraphItem(IGraphItem* item)
{
    m_edge=static_cast<IGraphEdge*>(item);

    IRenderer* ir=CreateImageRenderer(*m_canvas, item);
    m_image=static_cast<IImageRenderer*>(ir);
}

//-----------------------------------------------------------------------------

LRESULT CEdgeRenderer::SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return CScaledWindow::SendMessage(uMsg, wParam, lParam);
}

//-----------------------------------------------------------------------------

void CEdgeRenderer::SetFocus(bool bSet)
{
    // nothing to do for now
}

//-----------------------------------------------------------------------------

void CEdgeRenderer::SetSelected(bool bSelected, bool bAdd)
{
    // nothing to do for now
}

//-----------------------------------------------------------------------------

SizeF CEdgeRenderer::CalcDisplaySizes(
    CDCHandle& dc, 
    bool bForce,
    bool bIgnoreExtent)
{
    // nothing to do for now
    SizeF size;

    return size;
}

//-----------------------------------------------------------------------------

void CEdgeRenderer::GetBoundsBox(RectF& box, GraphTypes::Coordinates co)
{
    RectF rcf;

    if(m_vp.size()>1)
    {
        CRect rc;
        PointF p1=m_vp[0], p2=m_vp[m_vp.size()-1];
        m_canvas->AllTransform(p1);
        m_canvas->AllTransform(p2);
        
        rc.left=int(p1.x);
        rc.top=int(p1.y);
        rc.right=int(p2.x);
        rc.bottom=int(p2.y);
        rc.NormalizeRect();
        rcf=rc;
    }

    box=rcf;
    if(co==world)
    {
        m_canvas->RAllTransform(box);
    }
}

//-----------------------------------------------------------------------------

void CEdgeRenderer::SetBoundsBox(const RectF& box, unsigned flags, GraphTypes::Coordinates co)
{
    // TODO discover use for this
}

//-----------------------------------------------------------------------------

void CEdgeRenderer::GetPosition(PointF& pt, Coordinates co/*=world*/)
{
    // this is not terribly useful but....
    if(m_vp.size()>0)
    {
        pt=m_vp[0];

        if(co==screen)
        {
            m_canvas->AllTransform(pt);
        }
    }
}

//-----------------------------------------------------------------------------

void CEdgeRenderer::SetPosition(const PointF& pt, Coordinates co/*=world*/)
{
    PointF pos=pt;

    if(co==screen)
    {
        m_canvas->RAllTransform(pos);		
    }

    m_vp.push_back(pos);
}

//-----------------------------------------------------------------------------

void CEdgeRenderer::SetRedraw(bool bEnable)
{
    CScaledWindow::SetRedraw(bEnable);
}

//-----------------------------------------------------------------------------

void CEdgeRenderer::ClearCaches()
{

}

//-----------------------------------------------------------------------------

bool CEdgeRenderer::GetConnectionPoints(
    PointF& src, PointF& tgt,
    Coordinates co/*=screen*/,
    PointF* srcoff/*=0*/, PointF* tgtoff/*=0*/)
{
    bool bOk=true;
    CComPtr<IGraphVertex> vs, vt;
    rcxorder tmp;
    vRectXOrder vrco;

    REAL maxWidth;
    SizeF width;
    width.Width=m_canvas->m_meta.GetMetaEdgeWidth(m_edge->GetCategory());
    m_canvas->SizeTransform(width);
    maxWidth=width.Width/2;

    vs=m_edge->GetSource();
    vt=m_edge->GetTarget();

    tmp.item=0;

    m_arrowTip->SetGraphItem(vs);
    m_arrowTip->GetBoundsBox(tmp.rc); // right	
    vrco.push_back(tmp);

    m_arrowTail->SetGraphItem(vs);
    m_arrowTail->GetBoundsBox(tmp.rc);	// left
    vrco.push_back(tmp);

    tmp.item=1;

    m_arrowTip->SetGraphItem(vt);
    m_arrowTip->GetBoundsBox(tmp.rc); // right 	
    vrco.push_back(tmp);

    m_arrowTail->SetGraphItem(vt);
    m_arrowTail->GetBoundsBox(tmp.rc);	// left 
    vrco.push_back(tmp);

    std::sort(vrco.begin(), vrco.end(), Compare());

    RectF source, target;
    bool bSourceRight=false, bTargetRight=false;

    if(vrco[0].item==vrco[1].item) // items not overlapping in x extent
    {
        // maintain directionality

        if(vrco[0].item==0)
        {
            bSourceRight=true;
            source=vrco[1].rc;
            target=vrco[2].rc;
        }
        else
        {
            bTargetRight=true;
            source=vrco[2].rc;
            target=vrco[1].rc;
        }
    }
    else // items overlapping in x direction
    {
        if(vrco[0].item==0)
        {
            source=vrco[0].rc;
            target=vrco[1].rc;
        }
        else
        {
            source=vrco[1].rc;
            target=vrco[0].rc;
        }
    }

    if(source.Height>maxWidth)
    {
        source.y+=(source.Height-maxWidth)/2;
        source.Height=maxWidth;
    }
//	else
//	{
//		maxWidth=source.Height;
//	}

    m_arrowTail->SetBoundsBox(source, SWP_NOREDRAW);

    // target is thin

    if(target.Height>maxWidth)
    {
        target.y+=(target.Height-maxWidth/2)/2;
        if(maxWidth>1)
            target.Height=maxWidth/2;
        else
            target.Height=1;
    }

    m_arrowTip->SetBoundsBox(target, SWP_NOREDRAW);

    // right side of item is selected for source
    if(bSourceRight)
    {
        src.x=source.x+source.Width-source.Height/2;
    }
    else
    {
        src.x=source.x+source.Height/2;
    }

    src.y=source.y+source.Height/2;

    // right side of item is selected for target

    if(bTargetRight)
    {
        tgt.x=target.x+target.Width-target.Height/2;
    }
    else
    {
        tgt.x=target.x+target.Height/2;
    }

    tgt.y=target.y+target.Height/2;

    CComQIPtr<ITextboxRenderer> ir;
    ITextboxRenderer::eIOMode mode;

    if(srcoff)
    {
        srcoff->x=0;
        srcoff->y=0; // always the case for horizontal connectors

        ir=m_canvas->GetRenderer(vs);
        if(ir!=0)
        {
            srcoff->x=source.Width-source.Height/2;

            mode=ir->GetIOMode();

            if(mode==ITextboxRenderer::iomIn)
                srcoff->x=-srcoff->x;
        }
    }

    if(tgtoff)
    {
        tgtoff->x=0;
        tgtoff->y=0;

        ir=m_canvas->GetRenderer(vt);
        if(ir!=0)
        {
            tgtoff->x=target.Width-target.Height/2;

            mode=ir->GetIOMode();

            if(mode==ITextboxRenderer::iomIn)
                tgtoff->x=-tgtoff->x;
        }
    }

    return bOk;
}

//-----------------------------------------------------------------------------

unsigned CEdgeRenderer::GetPoints(PointFVector& vp, Coordinates co/*=world*/)
{
    vp.reserve(m_vp.size());
    vp = m_vp;
    if(co==world)
    {
        PointFVector::iterator itr=vp.begin();
        while(itr!=vp.end())
        {
            m_canvas->RAllTransform(*itr);
            ++itr;
        }
    }

    return m_vp.size();
}

//-----------------------------------------------------------------------------

void CEdgeRenderer::SetPoints(const PointFVector& vp, Coordinates co/*=world*/)
{
    m_vp.clear();
    m_vp.reserve(vp.size());
    m_vp = vp;

    if(co==screen)
    {
        for(PointFVector::iterator itr=m_vp.begin(); itr!=m_vp.end(); ++itr)
        {
            m_canvas->RAllTransform(*itr);
        }
    }

    m_svp=m_vp;
}

//-----------------------------------------------------------------------------

void CEdgeRenderer::GetLabelPos(RectF& pos, Coordinates co/*=screen*/)
{
    if(co==world)
    {
        RectF rc=m_rcLabel;
        m_canvas->RAllTransform(rc);
        pos=rc;
    }
    else
    {
        pos=m_rcLabel;
    }
}

//-----------------------------------------------------------------------------

void CEdgeRenderer::SetLabelPos(const RectF& pos, Coordinates co/*=screen*/)
{
    if(co==world)
    {
        RectF rc=pos;
        m_canvas->AllTransform(rc);
        m_rcLabel=rc;
    }
    else
    {
        m_rcLabel=pos;
    }
}

//-----------------------------------------------------------------------------

void CEdgeRenderer::Draw(CAggDC &dc) 
{
    Color color=m_edge->GetColor();
    if(color.GetAlpha()==0)
        color=m_canvas->m_meta.GetMetaEdgeColorNorm(m_edge->GetCategory());

    WORD hls[3];
    color.GetHLS(hls);
    if(hls[1]>230) // almost white
        color.LessBright(50);

    SizeF width;
    REAL maxWidth;

    width.Width=m_canvas->m_meta.GetMetaEdgeWidth(m_edge->GetCategory());
    m_canvas->SizeTransform(width);
    maxWidth=width.Width/2;

    CComPtr<IGraphVertex> vs, vt;
    rcxorder tmp;
    vRectXOrder vrco;
    
    vs=m_edge->GetSource();
    vt=m_edge->GetTarget();

    CComQIPtr<ITextboxRenderer> itbr=m_canvas->GetRenderer(vs);
    if(itbr!=0 && itbr->IsHot())
        color.LessBright(50);

    tmp.item=0;

    m_arrowTip->SetGraphItem(vs);
    m_arrowTip->GetBoundsBox(tmp.rc); // right	
    vrco.push_back(tmp);

    m_arrowTail->SetGraphItem(vs);
    m_arrowTail->GetBoundsBox(tmp.rc);	// left
    vrco.push_back(tmp);

    tmp.item=1;

    m_arrowTip->SetGraphItem(vt);
    m_arrowTip->GetBoundsBox(tmp.rc); // right 	
    vrco.push_back(tmp);

    m_arrowTail->SetGraphItem(vt);
    m_arrowTail->GetBoundsBox(tmp.rc);	// left 
    vrco.push_back(tmp);

    std::sort(vrco.begin(), vrco.end(), Compare());

    PointF pFrom;
    PointF pTo;
    PointF p3, p4;
    RectF source, target;
    bool bSourceRight=false, bTargetRight=false;

    if(vrco[0].item==vrco[1].item) // items not overlapping in x extent
    {
        // maintain directionality

        if(vrco[0].item==0)
        {
            bSourceRight=true;
            source=vrco[1].rc;
            target=vrco[2].rc;
        }
        else
        {
            bTargetRight=true;
            source=vrco[2].rc;
            target=vrco[1].rc;
        }
    }
    else // items overlapping in x direction
    {
        if(vrco[0].item==0)
        {
            source=vrco[0].rc;
            target=vrco[1].rc;
        }
        else
        {
            source=vrco[1].rc;
            target=vrco[0].rc;
        }
    }

    if(source.Height>maxWidth)
    {
        source.y+=(source.Height-maxWidth)/2;
        source.Height=maxWidth;
    }
//	else
//	{
//		maxWidth=source.Height;
//	}

    m_arrowTail->SetBoundsBox(source, SWP_NOREDRAW);

    // target is thin

    if(target.Height>maxWidth)
    {
        target.y+=(target.Height-maxWidth/2)/2;
        if(maxWidth>1)
            target.Height=maxWidth/2;
        else
            target.Height=1;
    }

    m_arrowTip->SetBoundsBox(target, SWP_NOREDRAW);

    // right side of item is selected for source
    if(bSourceRight)
    {
        pFrom.x=source.x+source.Width-source.Height/2;
    }
    else
    {
        pFrom.x=source.x+source.Height/2;
    }

    pFrom.y=source.y+source.Height/2;

    // right side of item is selected for target

    if(bTargetRight)
    {
        pTo.x=target.x+target.Width-target.Height/2;
    }
    else
    {
        pTo.x=target.x+target.Height/2;
    }

    pTo.y=target.y+target.Height/2;

    p3.x=pTo.x;
    p3.y=target.y;

    p4.x=pTo.x;
    p4.y=target.y+target.Height;

    if(m_canvas->m_meta.IsAvoidObstacles()==false || m_vp.size()<2)
    {
        PointF p2, p3;

        if(pFrom.x<pTo.x)
        {
            p2.x=pFrom.x+(pTo.x-pFrom.x)/2;
        }
        else if (pFrom.x>pTo.x)
        {
            p2.x=pTo.x+(pFrom.x-pTo.x)/2;
        }
        else
        {
            p2.x=pTo.x;
        }

        p3.x=p2.x;

        p2.y=pFrom.y;
        p3.y=pTo.y;

        dc.Bezier4(
            pFrom,
            p2,
            p3,
            pTo,
            color,
            1,
            true,
            m_canvas->GetScale());

        m_vp.clear();
        m_vp.push_back(pFrom);
        m_vp.push_back(p2);
        m_vp.push_back(p3);
        m_vp.push_back(pTo);
    }
    else if(m_vp.size()>2)
    {
//#define __INTERP_SPLINE__
#ifdef __INTERP_SPLINE__
        PointF point;
        std::vector<double> line; 

        for(unsigned i=0; i<m_vp.size(); ++i)
        {
            point=m_vp[i];
            m_canvas->AllTransform(point);
            line.push_back(point.x);
            line.push_back(point.y);
        }

        dc.InterpolatingSpline(
            &line[0], 
            line.size()/2,
            color,
            true);
#else
        PointFVector v(m_vp);
        for(PointFVector::iterator itr=v.begin(); itr!=v.end(); ++itr)
        {
            m_canvas->AllTransform(*itr);
        }
        dc.Bezier(
            v, 
            color,
            1,
            true,
            false,
            m_canvas->GetScale());
#endif
    }

    DrawLabel(dc);
}

//-----------------------------------------------------------------------------

void CEdgeRenderer::DrawLabel(CAggDC& dc)
{
    // If the flag has been set to skip edge label rendering, then get out
    if (!m_canvas->m_meta.IsEdgeLabelVisible())
        return;

    CSize szExtent;
    std::_tstring label = m_edge->GetLabel();
    if(label.length()==0)
    {
        m_canvas->m_meta.GetMetaEdgeLabel(m_edge->GetCategory(), label);
    }

    int len = label.length();
    if (len && m_vp.size()>1)
    {
        CDCHandle hdc(dc.m_hDC);
        CAutoFont f(m_canvas->GetListFont(m_canvas->GetFontPixHeight()), hdc);

        dc.GetTextExtent(label.c_str(), len, &szExtent);

        PointF p1=m_vp[0], p2=m_vp[m_vp.size()-1];
    //	m_canvas->AllTransform(p1);
    //	m_canvas->AllTransform(p2);

        m_rcLabel.Width=static_cast<REAL>(szExtent.cx);
        m_rcLabel.Height=static_cast<REAL>(szExtent.cy);
        m_rcLabel.x=(p1.x+p2.x-m_rcLabel.Width)/2;
        m_rcLabel.y=(p1.y+p2.y-m_rcLabel.Height)/2;

        CRect rctext=static_cast<CRect>(m_rcLabel);
        Color fcolor=m_canvas->m_meta.GetMetaEdgeFontColor(m_edge->GetCategory());
        dc.SetTextColor(fcolor.ToCOLORREF());
        //dc.SetBkColor(fillColor.ToCOLORREF());
        dc.DrawText(
            label.c_str(),
            len,
            &rctext,
            DT_NOPREFIX);
    }
    else
    {
        memset(&m_rcLabel, 0, sizeof m_rcLabel);
    }
}

//-----------------------------------------------------------------------------

LRESULT CEdgeRenderer::OnPaint(
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    BOOL& /*bHandled*/)
{
    bool bvis=m_canvas->m_meta.IsVisible(m_edge);

    if(bvis)
    {
        CAggDC& dc=*(CAggDC*)wParam;

        Draw(dc);	
        m_image->SendMessage(uMsg, wParam, lParam);
    }

    return 0;
}

//-----------------------------------------------------------------------------

IRenderer* CreateEdgeRenderer(CCanvas& parent)
{
    CEdgeRenderer * retVal = new CEdgeRenderer(parent);
    return retVal;
}
