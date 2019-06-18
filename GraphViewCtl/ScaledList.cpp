#include "StdAfx.h"

#include "Canvas.h"
#include "AggDC.h"
#include "Util.h" // Clib GetClipboardText
#include "ScaledList.h"

using namespace GraphTypes;

CScaledList::CScaledList(CCanvas* canvas)
    : CScaledWindow(canvas),
    m_vSB(canvas, this),
    m_Theme(canvas->GetPartTheme(_T("WINDOW"))),
    m_border(0,0),
    m_numitems(0),
    m_topidx(0),
    m_selectedidx(-1),
    m_visibleRows(0),
    m_whDividerLine(1)

{
//	m_border.cx=m_border.cy=1;
}

//-----------------------------------------------------------------------------

void CScaledList::SetGraphItem(IGraphItem* item)
{
    m_g=static_cast<IGraphSubgraph*>(item);
}

//-----------------------------------------------------------------------------

void CScaledList::SetWindowPos(const CRect& pos, UINT flags)
{
    bool bRedraw=(flags&SWP_NOREDRAW)==0; 
    // parent redraw sends paint message to all children,
    // so here we avoid sending redraw to child scrollbar again
    flags|=SWP_NOREDRAW;

    if(flags&SWP_NOSIZE)
    {
        // calling us with no move and no size makes no sense but,
        // for now we have CalcDisplaySizes invoked before each paint
        // and it invokes us, so return for now
        if(flags&SWP_NOMOVE)
            return;

        ATLASSERT(!(flags&SWP_NOMOVE)); 
        CPoint offset=pos.TopLeft()-m_rcWnd.TopLeft();
        m_rcWnd.OffsetRect(offset);

        m_rcClient.OffsetRect(offset);
        m_rcTextArea.OffsetRect(offset);

        if(m_vSB.IsVisible())
        {
            m_vSB.SetWindowPos(m_rcClient, flags);
        }
    }
    else
    {
        if(flags&SWP_NOMOVE)
        {
            m_rcWnd.right=m_rcWnd.left+pos.Width();
            m_rcWnd.bottom=m_rcWnd.top+pos.Height();			
        }
        else
        {
            m_rcWnd=pos;
        }

        m_rcClient=m_rcWnd;
        m_rcClient.DeflateRect(m_border.cx,m_border.cy);

        if(m_topidx>=(int)m_numitems)
            m_topidx=m_numitems>0?m_numitems-1:0;

        CalcVisibleItems(m_rcClient.Height());

        // here scrollbar may go from visible to invisible and viceversa

        SCROLLINFO si;
        si.fMask=SIF_RANGE|SIF_PAGE|SIF_POS;
        m_vSB.GetScrollInfo(&si);

        if(m_numitems>0 && m_visibleRows>0)
            si.nPage=m_visibleRows;
        else
            si.nPage=0;
        si.nPos=m_topidx;

        m_vSB.SetScrollInfo(&si, false);

        m_rcTextArea=m_rcClient;
        CRect rcsb(m_rcClient);

        int whScrollbar=GetSystemMetrics(SM_CYHSCROLL);
        SizeF scaled(REAL(whScrollbar),0);
        m_canvas->ScaleTransform(scaled);
        int iscaled=int(scaled.Width);
        rcsb.right=rcsb.left+iscaled;
        m_vSB.SetWindowPos(rcsb, flags);

        if(m_vSB.IsVisible())
            m_rcTextArea.left+=iscaled;
    }

    AdjustItemsBounds();

    if(bRedraw)
        Invalidate();
}

//-----------------------------------------------------------------------------
// 
// Returns height of the first item

int CScaledList::GetRowHeight(unsigned row/*=0*/) const
{
    int height=0;

    if(m_numitems>0 && row<m_numitems)
    {
        CComQIPtr<ITextboxRenderer> vr;

        vr=GetItem(row);
        ATLASSERT(vr);
        RectF rcitem;
        vr->GetBoundsBox(rcitem);
        height=static_cast<int>(rcitem.Height);
    }

    return height;
}

//-----------------------------------------------------------------------------

void CScaledList::SetFocus(bool bSet/*=true*/)
{
    // this object never takes the focus
    // so it can not lose it
    ATLASSERT(bSet);
    if(m_numitems>0)
    {
        if(m_selectedidx==-1)
        {
            // don't want to take it but the other must lose it
            m_canvas->ClearFocus();
        }
        else
        {
            CComQIPtr<ITextboxRenderer> vr=GetItem(m_selectedidx);
            if (vr)
                vr->SetFocus(bSet);
        }

        m_canvas->HookKbdMsgs(this);
    }
    else
    {
        // don't want to take it but the other must lose it
        m_canvas->ClearFocus();
    }
}

//-----------------------------------------------------------------------------

int CScaledList::SetTopIndex(int index)
{
    int oldidx=m_topidx;
    if(index>=0 && index<(int)m_numitems && index!=oldidx)
    {
        m_topidx=index;
        CalcVisibleItems(m_rcClient.Height());
        m_vSB.SetScrollPos(m_topidx, false);
        AdjustItemsBounds();
    }
    return oldidx;
}

//-----------------------------------------------------------------------------

int CScaledList::GetTopIndex() const
{
    return m_topidx;
}

//-----------------------------------------------------------------------------

bool CScaledList::HitTest(const CPoint& p) const
{
    bool bHit=false;

    if(m_rcWnd.PtInRect(p))
        bHit=true;

    return bHit;
}

//-----------------------------------------------------------------------------

IGraphItem* CScaledList::HitTestItems(const PointF& point) const
{
    CComPtr<IGraphItem> item;
    RectF rcitem;
    IGraphItemVector results;

    m_g->GetChildren(results);
    int idx=0;
    
    for(IGraphItemVector::iterator itr=results.begin();
        itr!=results.end();
        ++itr, ++idx)
    {
        if(idx>=m_topidx && idx<=m_topidx+m_visibleRows)
        {
            item=*itr;			
            CComQIPtr<ITextboxRenderer> vr=m_canvas->GetRenderer(item);

            if(vr!=0)
            {
                vr->GetBoundsBox(rcitem);
                // because of the divider space we may have no item
                // containing click point
                rcitem.Inflate(0,REAL(m_whDividerLine));

                if(rcitem.Contains(point))
                    break;
                else
                    item=0;
            }
        }
    }

    return item;
}

//-----------------------------------------------------------------------------

CScaledList::eLocation CScaledList::GetConnectBounds(RectF& box, unsigned order) const
{
    eLocation loc=middle;

    if(order<(unsigned)m_topidx)
    {
        loc=top;
    }
    else if(order>(unsigned)(m_topidx+m_visibleRows))
    {
        loc=bottom;
    }
    else
    {
        CComQIPtr<IRenderer> ir;

        ir=GetItem(order);
        if(ir!=0)
            ir->GetBoundsBox(box);	
    }

    return loc;
}

//-----------------------------------------------------------------------------

bool CScaledList::OnEditItem(IGraphItem* gi)
{
    IGraphItemVector results;
    bool bOk=m_g->GetChildren(results)>0;

    if(bOk)
    {
        CComQIPtr<IGraphVertex> v=gi;
        CComQIPtr<ITextboxRenderer> tbr;
        ATLASSERT(v);
        unsigned order=v->GetOrder();
        int idx=order;
    
        if(idx!=m_selectedidx)
        {
            if(m_selectedidx!=-1)
            {
                tbr=m_canvas->GetRenderer(results[m_selectedidx]);
                ATLASSERT(tbr);
                tbr->SetSelected(false);
            }

            tbr=m_canvas->GetRenderer(results[idx]);
            ATLASSERT(tbr);
            tbr->SetSelected(true);
            tbr->SetFocus(true);
            m_selectedidx=idx;
            Invalidate();
        }
    }

    return bOk;
}

//-----------------------------------------------------------------------------

void CScaledList::DrawListBorder(
    CDCHandle& dc,
    BORDER_STYLE style)
{
    bool bThemeOk=false;
    UINT flags=BF_RECT;

    if(style==LINE)
        flags |= BF_FLAT;

    if(!m_Theme.IsThemeNull())
    {
        if(SUCCEEDED(m_Theme.DrawThemeEdge(
            dc,
            WP_DIALOG,
            1,
            &m_rcWnd,
            EDGE_SUNKEN,
            flags)))
            bThemeOk=true;
    }

    if(!bThemeOk)
    {
        dc.DrawEdge(
            &m_rcWnd,
            EDGE_SUNKEN,
            flags);
    }
}

//-----------------------------------------------------------------------------

void CScaledList::DrawListItems(WPARAM wParam)
{
    CComQIPtr<ITextboxRenderer> vr;
    CComPtr<IUnknown> iunk;
    CComPtr<IGraphItem> item;
    RectF itemrc;
    CRect divider;
    IGraphItemVector results;
    unsigned count=m_g->GetChildren(results);
    int idx=0;
    CDCHandle dc(((CAggDC*)wParam)->m_hDC);

    ATLASSERT(count==m_numitems);

    for(
        IGraphItemVector::iterator itr=results.begin();
        itr!=results.end();
        ++itr, ++idx)
    {
        item=*itr;			
        vr=m_canvas->GetRenderer(item);

        if(vr!=0)
        {
            vr->SendMessage(WM_PAINT, wParam, 0);

            // draw divider line
            if(m_numitems>1)
            {
                if(idx>=m_topidx && 
                    idx<(m_topidx+m_visibleRows) && 
                    idx<(int)(m_numitems-1) )
                {
                    vr->GetBoundsBox(itemrc);
                    divider=itemrc;
                    divider.top=divider.bottom;
                    divider.bottom+=m_whDividerLine;
                    dc.FillSolidRect(&divider,::GetSysColor(COLOR_BTNFACE));
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------

LRESULT CScaledList::ForwardMsgToChildren(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CPoint p(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)); 
    LRESULT lRes=0;
    CComQIPtr<ITextboxRenderer> vr;
    CComPtr<IGraphItem> item;
    RectF rcitem;
    PointF pf(p);
    IGraphItemVector results;
    unsigned count=m_g->GetChildren(results);
    int idx=0;
    

    ATLASSERT(count==m_numitems);
    ATLASSERT(uMsg==WM_LBUTTONDOWN||uMsg==WM_MOUSEMOVE||uMsg==WM_LBUTTONUP||uMsg==WM_NCHITTEST); // for now

    for(IGraphItemVector::iterator itr=results.begin();
        itr!=results.end();
        ++itr, ++idx)
    {
        item=*itr;			
        vr=m_canvas->GetRenderer(item);

        if(vr!=0)
        {
            vr->GetBoundsBox(rcitem);
            if(rcitem.Height<2) // should be a compressed row, skip it
                continue;

            // because of the divider space we may have no item
            // containing click point
            rcitem.Inflate(0,REAL(m_whDividerLine));

            if(rcitem.Contains(pf))
            {
                lRes=vr->SendMessage(uMsg, wParam, lParam);

                if(uMsg==WM_LBUTTONDOWN) // can change selected index
                {
                    if(m_selectedidx!=idx)
                    {
                        if(m_selectedidx>=0)
                        {
                            vr=m_canvas->GetRenderer(results[m_selectedidx]);
                            if(vr!=0)
                                vr->SetSelected(false);
                        }
                        m_selectedidx=idx;
                        Invalidate(&m_rcTextArea);
                    }
                }

                break;
            }
        }
    }

    return lRes;
}

//-----------------------------------------------------------------------------

CSize CScaledList::CalcListMaxSize(CDCHandle& dc)
{
    SizeF maxsize; 
    SizeF extent, itemextent;
    unsigned realcount=0;
    RectF rcitem;
    CComQIPtr<ITextboxRenderer> ili;
    CComQIPtr<IGraphVertex> vertex;
    IGraphItemVector results;
    unsigned count = m_g->GetChildren(results);

    for(IGraphItemVector::iterator itr=results.begin();	itr!=results.end();	++itr)
    {
        vertex=*itr;			
        ATLASSERT(vertex);
        ili=m_canvas->GetRenderer(vertex);
        if(ili)
        {
            itemextent=ili->CalcDisplaySizes(dc, true, true);
            if(m_canvas->m_meta.GetRowCompress() && vertex->GetEdgeCount()>0)
                itemextent.Height=1;

            rcitem=itemextent;

            ili->SetBoundsBox(rcitem, SWP_NOREDRAW); // initial bounds correspond to extent

            maxsize.Height+=itemextent.Height;
            if(itemextent.Width>maxsize.Width)
                maxsize.Width=itemextent.Width;
            realcount++;
        }
    }

    ATLASSERT(realcount==count);
    m_numitems=count;

    if(count>0)
        maxsize.Height+=static_cast<REAL>(m_whDividerLine)*(count-1);

    SCROLLINFO si;
    m_vSB.GetScrollInfo(&si);
    si.fMask=SIF_RANGE;
    si.nMin=0;
    si.nMax=m_numitems-1;
    m_vSB.SetScrollInfo(&si, false);

    if(m_numitems==0)
        m_selectedidx=-1;
    else if(m_selectedidx>=(int)m_numitems)
        m_selectedidx=m_numitems-1; 

    CSize isize=static_cast<CSize>(maxsize);
    return isize;
}

//-----------------------------------------------------------------------------
//
//	This method calculates correct m_visibleRows and m_topidx. Starting
//	with a given m_topidx it will adjust it downward if more items can fit

int CScaledList::CalcVisibleItems(int height)
{
    int count=0;

    if(m_numitems>0)
    {
        int oldcount;
        int curtopidx=m_topidx;
        bool done=false;

        CComQIPtr<ITextboxRenderer> vr;
        RectF rcitem;

        do
        {
            oldcount=count;
            count=0;

            int cumulheight=0;
            int i=curtopidx;

            for(; i<(int)m_numitems; ++i)
            {
                vr=GetItem(i);
                ATLASSERT(vr);
                vr->GetBoundsBox(rcitem);
                cumulheight+=static_cast<int>(rcitem.Height);
                if(i<(int)(m_numitems-1)) // not the last one
                    cumulheight+=static_cast<int>(m_whDividerLine);
                if(cumulheight>height)
                    break;
                count++;
            }

            if(curtopidx<m_topidx)// we scrolled up in previous iteration
            {
                if(count<=oldcount) // but we haven't uncovered more items
                {
                    curtopidx++; // so go back and scroll down
                    done=true;
                }
            }

            if(i<(int)(m_numitems-1)) // there are items under the bottom visible row
            {
                done=true;

            }
            else if(curtopidx==0) // can't do anything else
            {
                done=true;
            }

            if(oldcount>0 && !done) // not the first pass
            {
                curtopidx--; // try to fit one more 
            }

        } while(!done);

        m_topidx=curtopidx;
    }

    m_visibleRows=count;

    return count;
}

//-----------------------------------------------------------------------------

void CScaledList::AdjustItemsBounds()
{
    CComPtr<IGraphItem> item;
    CComPtr<IUnknown> iunk;
    CComQIPtr<ITextboxRenderer> vr;
    RectF rcclip(m_rcTextArea);
    RectF rcitem;
    REAL prevheight=0;
    IGraphItemVector results;
    int idx=0;
    size_t count=m_g->GetChildren(results);
    ATLASSERT(count==m_numitems);

    for(
        IGraphItemVector::iterator itr=results.begin();
        itr!=results.end();
        ++itr, ++idx)
    {
        item=*itr;			
        vr=m_canvas->GetRenderer(item);

        if(vr!=0)
        {
            vr->GetBoundsBox(rcitem);

            // keep the height but adjust position and width
            if(idx>=m_topidx && idx<(m_topidx+m_visibleRows))
            {
                rcitem.x=rcclip.x;
                rcitem.y=rcclip.y+prevheight;
                prevheight+=rcitem.Height+m_whDividerLine;
                rcitem.Width=rcclip.Width;
            }
            else
            {
                // gather all invisible items at the bottom of the list
                rcitem.y=rcclip.y+rcclip.Height+1;				
            }

            vr->SetBoundsBox(rcitem, SWP_NOREDRAW);	
        }
    }
}

//-----------------------------------------------------------------------------

IRenderer* CScaledList::GetItem(int pos) const
{
    CComPtr<IRenderer> retVal;
    IGraphItem* item=m_g->GetChild(pos);
    if(item)
        retVal=m_canvas->GetRenderer(item);
    return retVal;
}

//*****************************************************************************
//
//	Message handlers
//

LRESULT CScaledList::OnPaint(
    UINT /*uMsg*/,
    WPARAM wParam,
    LPARAM /*lParam*/,
    BOOL& /*bHandled*/)
{
    if(m_numitems>0)
    {
        CAggDC& dc=*(CAggDC*)wParam;
        CDCHandle hdc(dc.m_hDC);

        // draw list border, background, scrollbar(s) and items
        DrawListBorder(hdc, LINE/*CLIENT_SUNKEN*/); 

        dc.FillSolidRect(&m_rcClient, GetSysColor(COLOR_WINDOW));

        int isave=hdc.SaveDC();
        dc.IntersectClipRect(m_rcTextArea);
        DrawListItems(wParam);
        hdc.RestoreDC(isave);

        if(m_vSB.IsVisible())
            m_vSB.SendMessage(WM_PAINT, wParam, 0);
    }

    return 0;
}

//-----------------------------------------------------------------------------

LRESULT CScaledList::OnLButtonDown(
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    BOOL& /*bHandled*/)
{
    ATLASSERT(m_numitems>0);

    LRESULT lRes=0;
    CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)); 

    SetFocus();

    if(m_vSB.IsVisible() && m_vSB.HitTest(pt))
    {
        m_vSB.SendMessage(uMsg, wParam, lParam);
    }
    else // in list
    {
        // no object can be hot in this case
        m_canvas->ClearCurrentHot();
        lRes=ForwardMsgToChildren(uMsg, wParam, lParam);
    }

    // this must happen after ForwardMsgToChildren as it may change m_selectedidx
    //CComQIPtr<IRenderer> ir=m_g->GetPropertyUnknown(0);
    //m_canvas->PostMsg(ir, LBN_LABELEDIT);
    return lRes;
}

//-----------------------------------------------------------------------------

LRESULT CScaledList::OnLButtonUp(
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    BOOL& /*bHandled*/)
{
    CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)); 

    ATLASSERT(m_numitems>0);

    if(m_vSB.IsVisible() && m_vSB.HitTest(pt))
    {
        m_vSB.SendMessage(uMsg, wParam, lParam);
    }
    else // in list
    {
        // no object can be hot in this case
        m_canvas->ClearCurrentHot();
        ForwardMsgToChildren(uMsg, wParam, lParam);
    }

    return 0;
}

//-----------------------------------------------------------------------------

LRESULT CScaledList::OnMouseMove(
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    BOOL& /*bHandled*/)
{
    CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)); 

    ATLASSERT(m_numitems>0);
    
    if(m_vSB.IsVisible() && m_vSB.HitTest(pt))
    {
        m_vSB.SendMessage(uMsg, wParam, lParam);
    }
    else // in list
    {
        // no object can be hot in this case
        m_canvas->ClearCurrentHot();
        ForwardMsgToChildren(uMsg, wParam, lParam);
    }

    return 0;
}

//-----------------------------------------------------------------------------

LRESULT CScaledList::OnVScroll(
    UINT /*uMsg*/,
    WPARAM wParam,
    LPARAM /*lParam*/,
    BOOL& /*bHandled*/)
{
    if(m_numitems==0)
        return 0;

    int code=(int)LOWORD(wParam);
    int maxpos=m_numitems-m_visibleRows;
    ATLASSERT(maxpos>=0);
    int oldtop=m_topidx;

    switch(code)
    {
    case SB_LINEDOWN:
        if(m_topidx < maxpos)
            m_topidx++;
        break;

    case SB_LINEUP:
        if(m_topidx > 0)
            m_topidx--;
        break;

    case SB_PAGEDOWN:
        m_topidx+=m_visibleRows-1;
        if(m_topidx > maxpos)
            m_topidx=maxpos;
        break;

    case SB_PAGEUP:
        m_topidx-=m_visibleRows-1;
        if(m_topidx<0)
            m_topidx=0;
        break;

    case SB_TOP:
        m_topidx=0;
        break;

    case SB_BOTTOM:
        m_topidx=maxpos;
        break;

    case SB_THUMBPOSITION:
    case SB_THUMBTRACK:
        {
            int topidx=HIWORD(wParam);
            ATLASSERT(topidx>=0 && topidx<=maxpos);
            if(topidx<0)
                topidx=0;
            if(topidx>maxpos)
                topidx=maxpos;

            m_topidx=topidx;
            ATLASSERT(m_topidx >= 0 && m_topidx < (int)m_numitems);
        }
        break;
    }

    if(m_topidx!=oldtop)
    {
        AdjustItemsBounds();
        Invalidate();
    }

    return 0;
}

//-----------------------------------------------------------------------------

LRESULT CScaledList::OnKeyDown(
    UINT /*uMsg*/,
    WPARAM wParam,
    LPARAM /*lParam*/,
    BOOL& /*bHandled*/)
{
    ATLASSERT(m_numitems>0);
    if(m_numitems==0)
        return 0;

    bool bScrollMsg=false;
    int maxidx=(int)m_numitems-1;
    int oldselected=m_selectedidx;

    switch(wParam)
    {
    case VK_F2:
        {
            IGraphItem* item=m_g->GetChild(m_selectedidx);
            if(item)
                m_canvas->AttachEditor(item);
        }
        break;

    case VK_PRIOR:
        if(m_selectedidx>0)
        {
            m_selectedidx-=m_visibleRows-1;
            if(m_selectedidx<0)
                m_selectedidx=0;

            if(m_selectedidx<m_topidx)
            {
                bScrollMsg=true;
                wParam=SB_PAGEUP;
            }
        }
        break;

    case VK_NEXT:
        if(m_selectedidx<maxidx)
        {
            m_selectedidx+=m_visibleRows-1;
            if(m_selectedidx>maxidx)
                m_selectedidx=maxidx;

            if(m_selectedidx>=(m_topidx+m_visibleRows))
            {
                bScrollMsg=true;
                wParam=SB_PAGEDOWN;
            }
        }
        break;

    case VK_END:
        if(m_selectedidx<maxidx)
        {
            m_selectedidx=maxidx;
            bScrollMsg=true;
            wParam=SB_BOTTOM;
        }
        break;

    case VK_HOME:
        if(m_selectedidx>0)
        {
            m_selectedidx=0;
            bScrollMsg=true;
            wParam=SB_TOP;
        }
        break;

    case VK_LEFT:
    case VK_UP:
        if(m_selectedidx>0)
        {
            m_selectedidx--;

            if(m_selectedidx<m_topidx)
            {
                bScrollMsg=true;
                wParam=SB_LINEUP;
            }
        }
        break;

    case VK_RIGHT:
    case VK_DOWN:
        if(m_selectedidx<maxidx)
        {
            m_selectedidx++;

            if(m_selectedidx>=(m_topidx+m_visibleRows))
            {
                bScrollMsg=true;
                wParam=SB_LINEDOWN;
            }
        }
        break;

    case 'V':
        {
            if((::GetAsyncKeyState(VK_CONTROL)&0x8000)!=0)
            {
                std::_tstring cbtext;
                if(GetClipboardText(cbtext)>0)
                {
/*
                    typedef boost::tokenizer<
                        boost::char_separator<TCHAR>,
                        std::_tstring::const_iterator,
                        std::_tstring> tokenizer;

                    boost::char_separator<TCHAR> sep(_T("\r\n"));
                    tokenizer tokens(cbtext,sep);
                    for (tokenizer::iterator tok_iter = tokens.begin();
                        tok_iter != tokens.end();
                        ++tok_iter)
                    {
                    }
*/
                    typedef std::vector<std::_tstring> split_vector_type;
                    split_vector_type splitvec; 
                    boost::algorithm::split(
                        splitvec,cbtext,boost::algorithm::is_any_of(_T("\r\n")),
                        boost::algorithm::token_compress_on); 

                    CComPtr<IGraphItem> item;

                    int idx=m_selectedidx==-1?m_topidx:m_selectedidx;
    
                    int itemcount=(int)m_numitems;

                    for(split_vector_type::const_iterator itrstrings=splitvec.begin();
                        idx<itemcount && itrstrings!=splitvec.end();
                        ++itrstrings,++idx)
                    {
                        item=m_g->GetChild(idx);
                        if(item)
                        {
                            if(m_canvas->NotifyLabelEdited(
                                m_g->GetID(),
                                item->GetID(),
                                *itrstrings)==-1) // controller didn't like it
                                break;
                        }
                        else
                            break;
                    }

                    // since all items get destroyed and re-created on NotifyLabelEdited
                    // we can not expect to maintain focus and selection except
                    // by setting it back explicitly

                    ATLASSERT(oldselected==m_selectedidx);
                    CComQIPtr<ITextboxRenderer> vr=GetItem(m_selectedidx);
                    if(vr!=0)
                    {
                        vr->SetFocus();
                        vr->SetSelected();
                    }
                }
            }
        }
        break;

    default:
        break;
    }

    if(oldselected!=m_selectedidx)
    {
        CComQIPtr<ITextboxRenderer> vr;

        vr=GetItem(oldselected);
        if(vr!=0)
            vr->SetSelected(false);
        vr=GetItem(m_selectedidx);
        if(vr!=0)
        {
            vr->SetFocus(true);
            vr->SetSelected(true);
        }
    }

    if(bScrollMsg)
    {
        SendMessage(WM_VSCROLL, wParam, (LPARAM)this);
        // NOTE: SB's nMin is assumed 0
        m_vSB.SetScrollPos(m_topidx, true);
    }

    return 0;
}

//-----------------------------------------------------------------------------

LRESULT CScaledList::OnNcHitTest(
    UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
    LRESULT lRes=0;
    ATLASSERT(m_numitems>0);

    lRes=ForwardMsgToChildren(uMsg, wParam, lParam);

    return lRes;
}

//-----------------------------------------------------------------------------

LRESULT CScaledList::OnLabelEdit(
    UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    IGraphItem* item=m_g->GetChild(m_selectedidx);
    if(item)
        m_canvas->AttachEditor(item);

    return 0;
}

