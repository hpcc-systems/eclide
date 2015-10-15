#include "stdafx.h"
#include "resource.h"

#include "Layout.h"
#include "Util.h"
#include "ItemEditor.h"
#include "AggDC.h"
#include "AggCommon.h"
#include "utilDateTime.h"
#include "Canvas.h"
#include "table.h"
#include "XGMMLSaxHandler.h"

const unsigned int	MAIN_RENDERER_IDX=0;
const unsigned int	ALTERNATE_RENDERER_IDX=1;
const unsigned int	OVERVIEW_RENDERER_IDX=2;

using namespace GraphTypes;

static const TCHAR * const tmp = _T("Test");
//-----------------------------------------------------------------------------
CCanvas::CCanvas(IGraph * graph, CMeta & meta)
    : m_graph(graph),
    m_meta(meta),
    m_running(false),
    m_printing(false),
    m_dragDropNotifications(false),
    m_edit(0),
    m_currentHot(0),
    m_currentCapture(0),	
    m_currentFocus(0),
    m_currentKbdHook(0),
    m_timerTarget(0),
    m_fonts(0),
    m_mouseOut(true),
    m_appZoomMode(AZMSelection),
    m_timeline(0),
    hInstRich(0)
#ifdef _DEBUG_THEME
    ,bdebugTheme(false)
#endif
#ifdef _DEBUG
    ,inpaint(false)
#endif
{
    m_gridPen.CreatePen(PS_DASHDOT, 1, RGB(192,192,192));
    m_HandClosedCursor = LoadCursor(ATL::_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(IDC_HAND_CLOSED));
    m_HandOpenCursor = LoadCursor(ATL::_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(IDC_HAND_OPEN));
    m_ZoomInCursor = LoadCursor(ATL::_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(IDC_ZOOM_IN));
    m_ZoomOutCursor = LoadCursor(ATL::_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(IDC_ZOOM_OUT));
    m_HandArrowCursor = LoadCursor(ATL::_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(IDC_HAND_ARROW));
    m_mouseMode=m_meta.GetMouseMode();
    SetApplicationMode(AppModeRelavint);
    SetApplicationVisMode(RelavintStandard);
    // assume constructor is execuited on GUI thread
    m_hGUIThreadId = ::GetCurrentThreadId();
}

CCanvas::~CCanvas()
{
    m_currentHot = NULL;
    m_currentFocus = NULL;
    m_graph->SetInternalUnknown(MAIN_RENDERER_IDX, NULL);
    m_graph->SetInternalUnknown(ALTERNATE_RENDERER_IDX, NULL);
    m_graph->SetInternalUnknown(OVERVIEW_RENDERER_IDX, NULL);	//GJS:  Not sure why this is needed, but without it there seems to be a double delete of the overview renderer...
    delete m_fonts;
    if(hInstRich)
        ::FreeLibrary(hInstRich);
}

//-----------------------------------------------------------------------------

#ifdef _DEBUG_DESTRUCT

ULONG STDMETHODCALLTYPE CCanvas::AddRef() throw()
{
    ULONG ret=CUnknown::AddRef();
    return ret;
}

ULONG STDMETHODCALLTYPE CCanvas::Release() throw()
{
    ULONG ret=CUnknown::Release();
    return ret;
}

#endif // _DEBUG_DESTRUCT

//-----------------------------------------------------------------------------

void CCanvas::Clear()
{
    PointF orig;
    DeselectAll();
    SetOffset(orig);
}

//-----------------------------------------------------------------------------

void CCanvas::SetApplicationMode(AppMode mode)
{
    m_appMode=mode;

    // except for Relavint all other apps don't use app visual mode for now
    if(mode!=AppModeRelavint)
        SetApplicationVisMode(Default);

    IGraphRenderer* ir=0;

    switch(mode)
    {
    case AppModeRelavint:
        ir=CreateLegacyGraphRenderer(*this, m_graph);
        break;

    case AppModeGrETL:
        ir=CreateNewGraphRenderer(*this, m_graph);
        break;

    case AppModeQBGraph:
        ir=CreateAttGraphRenderer(*this, m_graph);
        break;

    default:
        ATLASSERT(0);
    }

    ATLASSERT(ir);

    if(mode==AppModeRelavint)
    {
        m_graph->SetInternalUnknown(MAIN_RENDERER_IDX, ir);
        ir=CreateTimelineGraphRenderer(*this, m_graph);
        ATLASSERT(ir);
        m_graph->SetInternalUnknown(ALTERNATE_RENDERER_IDX, ir);
    }
    else
    {
        m_graph->SetInternalUnknown(MAIN_RENDERER_IDX, ir);
    }

    GetOverviewRenderer(); // will create one instance, if needed
}

//-----------------------------------------------------------------------------

AppMode CCanvas::GetApplicationMode() const
{
    return m_appMode;
}

//-----------------------------------------------------------------------------

void CCanvas::SetApplicationVisMode(AppVisMode mode)
{
    m_appVisMode=mode;
}

//-----------------------------------------------------------------------------

AppVisMode CCanvas::GetApplicationVisMode() const
{
    return m_appVisMode;
}

//-----------------------------------------------------------------------------

void CCanvas::SetApplicationZoomMode(AppZoomMode mode) 
{ 
    m_appZoomMode=mode; 

    if(m_appZoomMode!=AZMSelection)
        ClearCurrentHot();

    SetZoomModeCursor();
}

//-----------------------------------------------------------------------------

IRenderer* CCanvas::CreateGraphItemRenderer(IGraphItem* gi, RendererTypes type)
{
    GraphTypes::PointF previousPos;
    CComPtr<IRenderer> renderer = GetRenderer(gi, false);
    if (renderer)
    {
        renderer->GetPosition(previousPos);
    }

    // NB - rather cheesy way of forcing edge type in GrETL mode
    // that's because type is not sent over the SGV interface but
    // there is default type param to AddEdge method which is legacy edge

    if(m_appMode==AppModeGrETL && type==rtLegacyEdge)
        type=rtListEdge;

#ifdef _DEBUG
    switch(m_appMode)
    {
    case AppModeRelavint:
        ATLASSERT(type==rtLegacyVertex || type==rtLegacyEdge || type==rtLegacySubgraph);
        break;

    case AppModeGrETL:
        ATLASSERT(type==rtListItemVertex || type==rtListEdge ||
            type==rtIOSubgraph || type==rtInputVertex || type==rtOutputVertex ||
            type==rtDatasetSubgraph);
        break;

    case AppModeQBGraph:
        ATLASSERT(type==rtAttSubgraph || type==rtAttVertex || type==rtAttEdge);
        break;

    default:
        ATLASSERT(!"Unknown App mode");
    }
#endif

    IRenderer* ir=0;
    switch(type)
    {
    case rtLegacyVertex:
        ir=CreateLegacyVertexRenderer(*this);
        break;

    case rtLegacyEdge:
        ir=CreateLegacyEdgeRenderer(*this);
        break;

    case rtLegacySubgraph:
        ir=CreateLegacySubgraphRenderer(*this);
        break;

    case rtListItemVertex:
        ir=CreateListItemRenderer(*this);
        break;

    case rtListEdge:
        ir=CreateEdgeRenderer(*this);
        break;

    case rtIOSubgraph:
        ir=CreateIOSubgraphRenderer(*this);
        break;

    case rtInputVertex:
        ir=CreateInputRenderer(*this);
        break;

    case rtOutputVertex:
        ir=CreateOutputRenderer(*this);
        break;

    case rtDatasetSubgraph:
        ir=CreateDatasetRenderer(*this);
        break;

    case rtAttSubgraph:
        ir=CreateAttSubgraphRenderer(*this);
        break;

    case rtAttVertex:
        ir=CreateAttVertexRenderer(*this);
        break;

    case rtAttEdge:
        ir=CreateAttEdgeRenderer(*this);
        break;
    }

    ATLASSERT(ir);

    if(ir!=0)
    {
        ir->SetGraphItem(gi);
        SetRenderer(gi, ir);
        ir->SetPosition(previousPos);

        if(type==rtLegacyEdge)
        {
            ir=CreateTimelineEdgeRenderer(*this);
            ATLASSERT(ir);
            ir->SetGraphItem(gi);
            SetRenderer(gi, ir);
            ir->SetPosition(previousPos);
        }
    }

    return ir;
}

//-----------------------------------------------------------------------------

IRenderer * CCanvas::GetVertexRenderer(const CUniqueID & id) const
{
    ATLASSERT(!"todo");
    return NULL;
}

//-----------------------------------------------------------------------------

IRenderer * CCanvas::GetRenderer(IGraphItem * item, bool createIfNeeded) const
{
    //  resolve which renderer is appropriate...
    IUnknown* retVal;
    if(m_appMode==AppModeRelavint)
    {
#ifdef _DEBUG
        CComQIPtr<IGraph> g=item;
        CComQIPtr<IGraphVertex> v=item;
        CComQIPtr<IGraphEdge> e=item;
        CComQIPtr<IGraphSubgraph> sg=item;
        ATLASSERT(g || v || e || sg);
#endif
        if(m_appVisMode==RelavintStandard)
            retVal = item->GetInternalUnknown(MAIN_RENDERER_IDX);
        else
            retVal = item->GetInternalUnknown(ALTERNATE_RENDERER_IDX);
    }
    else if(m_appMode==AppModeQBGraph)
    {
        retVal = item->GetInternalUnknown(MAIN_RENDERER_IDX);
        if (!retVal && createIfNeeded)
        {
            CCanvas * self = const_cast<CCanvas * >(this);
            if (CComQIPtr<IGraphEdge> e = item)
                retVal = self->CreateGraphItemRenderer(item, rtAttEdge);
            else if (CComQIPtr<IGraphVertex> v = item)
                retVal = self->CreateGraphItemRenderer(item, rtAttVertex);
            else if (CComQIPtr<IGraphSubgraph> sg = item)
                retVal = self->CreateGraphItemRenderer(item, rtAttSubgraph);
            else if (CComQIPtr<IGraph> g = item)
                retVal = self->CreateGraphItemRenderer(item, rtAttGraph);
        }
    }
    else if(m_appMode==AppModeGrETL)
    {
        retVal = item->GetInternalUnknown(MAIN_RENDERER_IDX);
        if (!retVal && createIfNeeded)
        {
            CCanvas * self = const_cast<CCanvas * >(this);
            if (CComQIPtr<IGraphEdge> e = item)
                retVal = self->CreateGraphItemRenderer(item, rtListEdge);
            else if (CComQIPtr<IGraphVertex> v = item)
                retVal = self->CreateGraphItemRenderer(item, rtListItemVertex);
            else if (CComQIPtr<IGraphSubgraph> sg = item)
                retVal = self->CreateGraphItemRenderer(item, rtDatasetSubgraph);
            else if (CComQIPtr<IGraph> g = item)
                retVal = self->CreateGraphItemRenderer(item, rtAttGraph);
        }
    }
    else
    {
        retVal = item->GetInternalUnknown(MAIN_RENDERER_IDX);
    }

    return reinterpret_cast<IRenderer*>(retVal);
}

//-----------------------------------------------------------------------------

void CCanvas::SetRenderer(IGraphItem * item, IRenderer * renderer)
{
    //  resolve which renderer is appropriate...
    if(m_appMode==AppModeRelavint)
    {
        CComQIPtr<IGraph> g=item;
        if(g)
        {
            CComQIPtr<ILegacyGraphRenderer> r=renderer;
            if(r)
            {
                item->SetInternalUnknown(MAIN_RENDERER_IDX, renderer);
            }
            else
            {
                CComQIPtr<ITimelineGraphRenderer> t=renderer;
                if(t)
                {
                    item->SetInternalUnknown(ALTERNATE_RENDERER_IDX, renderer);
                }
                else
                {
                    CComQIPtr<IOverviewRenderer> o=renderer;
                    if(o)
                    {
                        item->SetInternalUnknown(OVERVIEW_RENDERER_IDX, renderer);
                    }
                    else
                    {
                        ATLASSERT(!"Unknown renderer");
                    }
                }
            }
        }
        else
        {
            CComQIPtr<IGraphVertex> v=item;
            CComQIPtr<IGraphEdge> e=item;
            CComQIPtr<IGraphSubgraph> sg=item;

            ATLASSERT(v || e || sg);

            if(e)
            {
                CComQIPtr<ILegacyEdgeRenderer> le=renderer;
                if(le)
                {
                    item->SetInternalUnknown(MAIN_RENDERER_IDX, renderer);
                }
                else
                {
                    CComQIPtr<ITimelineEdgeRenderer> te=renderer;
                    ATLASSERT(te);
                    item->SetInternalUnknown(ALTERNATE_RENDERER_IDX, renderer);
                }
            }
            else
            {
                item->SetInternalUnknown(MAIN_RENDERER_IDX, renderer);
            }
        }
    }
    else // otrher apps except Relavint
    {
        item->SetInternalUnknown(MAIN_RENDERER_IDX, renderer);
    }
}

//-----------------------------------------------------------------------------

IOverviewRenderer* CCanvas::GetOverviewRenderer()
{
    IOverviewRenderer* ior=0;
    if(m_meta.IsEmulateDecoration())
    {
        ior=reinterpret_cast<IOverviewRenderer*>(m_graph->GetInternalUnknown(OVERVIEW_RENDERER_IDX));
        if(ior==0) // only one overview renderer
        {
            ior=CreateOverviewRenderer(*this, m_graph);
            ATLASSERT(ior);
            m_graph->SetInternalUnknown(OVERVIEW_RENDERER_IDX, ior);
        }
    }
    return ior;
}

//-----------------------------------------------------------------------------

void CCanvas::SetTopParent(HWND hwnd) 
{ 
    m_wndCanvas.Attach(hwnd); 

    m_fonts=new CAggFonts(m_wndCanvas.GetDC());
    ATLASSERT(m_fonts);

    // add tooltip
    if(hInstRich==0)
        hInstRich = ::LoadLibrary(CRichEditCtrl::GetLibraryName());
    ATLASSERT(hInstRich);
    DWORD dwStyle = WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP;
    RECT rcDefault2 = { CW_USEDEFAULT, CW_USEDEFAULT, 0, 0 };
}

//-----------------------------------------------------------------------------

LRESULT CCanvas::SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRes=0;
    RectF box;
    CPoint pt;
    bool bMouseMsg=uMsg>=WM_MOUSEFIRST && uMsg<=WM_MOUSELAST;
    IRenderer* igr=GetRenderer(m_graph);
    IOverviewRenderer* ior=GetOverviewRenderer();

    if(uMsg==WM_MOUSELEAVE)
    {
        m_mouseOut=true;
        if(ClearCurrentHot())
            Invalidate();
    }

    if(bMouseMsg)
    {
        pt.x=GET_X_LPARAM(lParam);
        pt.y=GET_Y_LPARAM(lParam);

        if(m_edit)
        {
            m_wndCanvas.MapWindowPoints(m_edit->m_hWnd, &pt, 1);
            LPARAM newlp=MAKELPARAM(pt.x, pt.y);
            if(m_edit->MouseMessage(uMsg, wParam, newlp))
                return lRes;
        }

        if(uMsg==WM_MOUSEMOVE && m_mouseOut)
        {
            m_mouseOut=false;
            TRACKMOUSEEVENT tm;
            tm.cbSize=sizeof(tm);
            tm.dwFlags=TME_LEAVE;
            tm.hwndTrack=m_wndCanvas.m_hWnd;
            ::_TrackMouseEvent(&tm);
        }

        if(ior && m_meta.IsEmulateDecoration())
        {
            // TODO this dirt here is due to the 'lazy' approach which has pros and cons.
            // Con is that it is codewise and conceptually dirty. IOW, instead of these hooplas
            // with message passing when overview is visible, overview renderer should be
            // handled inside the graph renderer which has clear message passing rules.
            // Pro is that we keep all the overview code in canvas instead of doubling it in legacy
            // and non-legacy graph renderer.

            if(m_meta.IsOverviewVisible())
            {
                ior->GetBoundsBox(box);
                if(box.Contains(PointF(pt.x,pt.y)))
                {
                    if(m_currentCapture==0 || m_currentCapture==reinterpret_cast<CScaledWindow*>(ior))
                        lRes=ior->SendMessage(uMsg, wParam, lParam); 
                    else
                        lRes=igr->SendMessage(uMsg, wParam, lParam); 
                }
                else
                    lRes=igr->SendMessage(uMsg, wParam, lParam); 
            }
            else
            {
                lRes=igr->SendMessage(uMsg, wParam, lParam); 
            }
        }
        else
        {
            lRes=igr->SendMessage(uMsg, wParam, lParam); 
        }
    }
    else
    {

#ifdef _DEBUG
        inpaint=uMsg==WM_PAINT;
#endif

        // non-mouse message - send one message from posted message list
        if(!m_postedMessages.empty() && uMsg!=WM_PAINT)
        {
            PostedMsg& msg=*m_postedMessages.begin();

            msg.ir->SendMessage(msg.uMsg, msg.wParam, msg.lParam);
            m_postedMessages.pop_front();
        }

        // TODO clear this conceptually since it is not obvious what's the best way to do this
        // Let renderer know its bounds box before redraw

        int dctech=DT_RASDISPLAY; // don't want to draw overview if printer DC

        if(uMsg==WM_PAINT)
        {
            CRect rcdraw;
            CAggDC* aggdc=(CAggDC*)wParam;

            dctech=aggdc->GetDeviceCaps(TECHNOLOGY);
            m_printing=dctech!=DT_RASDISPLAY;
            m_wndCanvas.GetClientRect(&rcdraw);
            box=rcdraw;
            igr->SetBoundsBox(box, SWP_NOREDRAW);
        }

        lRes=igr->SendMessage(uMsg, wParam, lParam); 
        if(ior && !m_printing && m_meta.IsOverviewVisible() && m_meta.IsEmulateDecoration())
        {
            ior->SendMessage(uMsg, wParam, lParam);
        }
    }

#ifdef _DEBUG
    inpaint=false;
#endif

    return lRes;
}

//-----------------------------------------------------------------------------
//
// posted messages are added to one list so they can be retrieved later

void CCanvas::PostMsg(IRenderer* ir, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if(!m_postedMessages.empty())
    {
        // don't post the same message twice
        PostedMsg last=*m_postedMessages.rbegin();	
        if(last.ir==ir && last.uMsg==uMsg && last.wParam==wParam && last.lParam==lParam)
            return;
    }

    PostedMsg newmsg;

    newmsg.ir=ir;
    newmsg.uMsg=uMsg;
    newmsg.wParam=wParam;
    newmsg.lParam=lParam;

    m_postedMessages.push_back(newmsg);
}

//-----------------------------------------------------------------------------

void CCanvas::Invalidate(CRect* rc/*=0*/, bool bErase/*=false*/)
{
#ifdef _DEBUG
    ATLASSERT(!inpaint);
#endif

    if(m_wndCanvas.IsWindow() && !m_running)
        m_wndCanvas.InvalidateRect(rc, bErase);
}

//-----------------------------------------------------------------------------

void CCanvas::SetTimer(UINT nIDEvent, UINT uElapse, CScaledWindow* target)
{
    if(m_wndCanvas.IsWindow())
    {
        if(m_wndCanvas.SetTimer(nIDEvent, uElapse))
            m_timerTarget=target;
    }
}

//-----------------------------------------------------------------------------

void CCanvas::KillTimer(UINT uIDEvent)
{
    if(m_wndCanvas.IsWindow())
        m_wndCanvas.KillTimer(uIDEvent);

    m_timerTarget=0;
}

//-----------------------------------------------------------------------------

bool CCanvas::DragDropNotifications()
{
    return m_dragDropNotifications;
}

//-----------------------------------------------------------------------------

void CCanvas::DragDropNotifications(bool value)
{
    m_dragDropNotifications = value;
}

//-----------------------------------------------------------------------------

void CCanvas::ClientCursorPos(CPoint* pt) const
{
    if(pt->x==-1 && pt->y==-1) // keyboard
        GetCursorPos(pt);

    m_wndCanvas.ScreenToClient(pt);
    if(pt->x<0)
        pt->x=0;
    if(pt->y<0)
        pt->y=0;
}

//-----------------------------------------------------------------------------

GraphTypes::REAL CCanvas::ScreenPosFromTime(
    const FILETIME& time, 
    const FILETIME& screenleft, 
    const FILETIME& screenright) const
{
    double pos=0;
    RectF rc;
    GetClient(rc); // N.B. scaling is in terms of timeline width so ignore scrollbars

    ATLASSERT(rc.Width>0); // what do to in this case?
    if(rc.Width==0)
        return static_cast<GraphTypes::REAL>(pos);

    // formula is: pos = width*(t-t1)/(t2-t1)

    __int64 t2_t1=*(__int64*)&screenright-*(__int64*)&screenleft;
    __int64 t_t1=*(__int64*)&time-*(__int64*)&screenleft;
    // So far no visible difference between multiplication first and division first
    //pos=rc.Width*double(t_t1)/double(t2_t1);
    pos=(double(t_t1)/double(t2_t1))*rc.Width;

    return static_cast<GraphTypes::REAL>(pos);
}

//-----------------------------------------------------------------------------

bool CCanvas::IsTimelineRangeWithinLimits(const FILETIME& begin, const FILETIME& end)
{
    using namespace boost::posix_time;
    using namespace boost::gregorian;

    bool itis=false;

    FILETIME ftp1;
    FILETIME ftp2;

    GraphTypes::PointF p1, p2;
    unsigned count=0;
    CComPtr<IEdgeIterator> itr = m_graph->GetEdgeIterator();
    for ( ; itr->IsValidItem(); itr->NextItem())
    {
        CComPtr<IGraphEdge> e = itr->Get();
        ATLASSERT(e);
        time_period edgeSpan = e->GetDateRange();

        ptime beginE=edgeSpan.begin();
        ptime endE=edgeSpan.end();
        ATLASSERT(beginE<=endE);

        PtimeToFileTime(beginE, ftp1);
        p1.x=ScreenPosFromTime(ftp1, begin, end);

        if(p1.x <= static_cast<GraphTypes::REAL> (INT_MIN / 4) + 1.0)
            break;
        if(p1.x >= static_cast<GraphTypes::REAL> (INT_MAX / 4) - 1.0)
            break;

        PtimeToFileTime(endE, ftp2);
        p2.x=ScreenPosFromTime(ftp2, begin, end);

        if(p2.x >= static_cast<GraphTypes::REAL> (INT_MAX / 4) - 1.0)
            break;
        if(p2.x <= static_cast<GraphTypes::REAL> (INT_MIN / 4) + 1.0)
            break;
        count++;
    }

    if(count==m_graph->GetEdgeCount())
        itis=true;

    return itis;
}

//-----------------------------------------------------------------------------

LRESULT CCanvas::NotifySelection(const CUniqueID& selected,	const CUniqueID& deselected/*=CUniqueID()*/)
{
    NMSELECTIONCHANGE nmh;
    nmh.idSelect=selected;
    nmh.idDeselect=deselected;
    return NotifyParent(SCW_NOTIFYSELECTED, &nmh);
}
//-----------------------------------------------------------------------------

LRESULT CCanvas::NotifyItemsConnected(const CUniqueID& from, const CUniqueID& to)
{
    // don't send notification for the same item
    if(from!=to)
    {
        NMITEMCONN nmh;
        nmh.idfrom=from;
        nmh.idto=to;
        return NotifyParent(SCW_NOTIFYITEMSCONNECTED, &nmh);
    }

    return 0;
}

//-----------------------------------------------------------------------------

LRESULT CCanvas::NotifyLabelEdited(const CUniqueID& parentid, const CUniqueID& id, const std::_tstring& newlabel)
{
    NMLABELEDITED nmh;
    nmh.parentid=parentid;
    nmh.id=id;
    nmh.newlabel=newlabel;
    return NotifyParent(SCW_NOTIFYLABELEDITED, &nmh);
}

//-----------------------------------------------------------------------------

LRESULT CCanvas::NotifyRecalcEdgePoints()
{
    NMRECALCEDGEPOINTS nmh;
    return NotifyParent(SCW_NOTIFYRECALCEDGEPOINTS, &nmh);
}

//-----------------------------------------------------------------------------

LRESULT CCanvas::NotifyFocusChanged(const CUniqueID& id, bool bGained)
{
    NMFOCUSCHANGED nmh;
    nmh.id=id;
    nmh.bGained=bGained;
    LRESULT lRet = NotifyParent(SCW_NOTIFYFOCUSCHANGED, &nmh);

    if (bGained)
    {
        m_allPathsToFocus = m_graph->CalculateShortestPaths(id);
    }
    else
    {
        m_allPathsToFocus.Release();
        m_overPathToFocus.Release();
    }

    return lRet;
}

//-----------------------------------------------------------------------------

LRESULT CCanvas::NotifyHotChanged(const CUniqueID& id, bool bGained)
{
    NMFOCUSCHANGED nmh;
    nmh.id=id;
    nmh.bGained=bGained;
    LRESULT lRet = NotifyParent(SCW_NOTIFYHOTCHANGED, &nmh);

    if (m_allPathsToFocus)
    {
        if (bGained)
        {
            m_overPathToFocus = m_allPathsToFocus->CalculateShortestRoute(id);
        }
        else
        {
            m_overPathToFocus.Release();
        }
    }

    return lRet;
}

//-----------------------------------------------------------------------------

LRESULT CCanvas::NotifyDoDragDrop(const CPoint& dragStartPos)
{
    NMDODRAGDROP nmh;
    nmh.dragStartPos=dragStartPos;
    return NotifyParent(SCW_NOTIFYDODRAGDROP, &nmh);
}

//-----------------------------------------------------------------------------

LRESULT CCanvas::NotifyContextMenu(UINT uMsg, const CUniqueID& id, const CPoint& pt)
{
    NMCONTEXTMENU nmh;
    nmh.id=id;
    nmh.pt=pt;
    return NotifyParent(uMsg, &nmh);
}

//-----------------------------------------------------------------------------

LRESULT CCanvas::NotifyScroll(const CPoint& pt)
{
    NMSCROLL nmh;
    nmh.pt=pt;
    return NotifyParent(SCW_NOTIFYSCROLL, &nmh);
}

//-----------------------------------------------------------------------------

LRESULT CCanvas::NotifyMouseMessage(UINT uMsg, const CUniqueID& id, const CPoint& pt)
{
    NMMOUSEMESSAGE nmh;
    nmh.uMsg=uMsg;
    nmh.id=id;
    nmh.pt=pt;
    return NotifyParent(SCW_NOTIFYMOUSEMESSAGE, &nmh);
}

//-----------------------------------------------------------------------------

IGraphItem* CCanvas::FindGraphItem(const PointF& point)
{
    CComPtr<IGraphItem> ig;
    CComPtr<IUnknown> unk;
    CComQIPtr<IListWindowRenderer> lw;
    CComQIPtr<IIOSubgraphRenderer> io;
    CComQIPtr<ILegacyRenderer> ir;
    CComPtr<IGraphItem> item;
    CComPtr<MetaVisibleSkipper> skipper = new MetaVisibleSkipper(m_meta);

    RectF rc;
    for(CComPtr<IGraphItemIterator> itr = m_graph->GetChildren(skipper); itr->IsValidItem(); itr->NextItem())
    {
        item=itr->Get();	

        unk=GetRenderer(item);

        if(unk!=0)
        {
            io=unk.p;
            if(io!=0)
            {
                ig=io->HitTestChildren(point);

                if(ig!=0)
                    break;
            }
            else
            {
                if(GetApplicationMode()==AppModeRelavint)
                {
                    ir=unk.p;
                    if(ir!=0)
                    {
                        bool hit;
                        ir->HitTestDispatchMsg(point, hit, WM_USER, 0, 0);
                        if(hit)
                        {
                            ig = item;
                            break;
                        }
                    }
                }
                else // QBGraph
                {
                    lw=unk.p;
                    if(lw!=0)
                    {
                        ig=lw->HitTestChildren(point);

                        if(ig!=0)
                            break;
                    }
                }
            }
        }
    }

    return ig;
}

//-----------------------------------------------------------------------------

void CCanvas::OnRemoveGraphItem(IGraphItem* gi)
{
    // when vertex gets removed from subgraph inform subgraph,
    // unless it is the topmost graph
    CComQIPtr<IGraphVertex> v=gi;
    if(v!=0)
    {
        CComPtr<IGraphSubgraph> sg=v->GetParent();
        if(sg!=0)
        {
            CComPtr<IUnknown> unk=GetRenderer(sg);
            if(unk!=0)
            {
                CComQIPtr<IListWindowRenderer> ilw=unk.p;
                if(ilw!=0)
                    ilw->OnRemoveGraphItem(gi);
            }
        }
    }
}

//-----------------------------------------------------------------------------

CScaledWindow* CCanvas::SetCapture(CScaledWindow* capnew)
{
    CScaledWindow* capold=m_currentCapture;
    m_currentCapture=capnew;
    return capold;
}

//-----------------------------------------------------------------------------

bool CCanvas::ReleaseCapture()
{
    bool bSuccess=m_currentCapture!=0;

    if(bSuccess)
        m_currentCapture=0;

    ::ReleaseCapture();

    return bSuccess;
}

//-----------------------------------------------------------------------------

CScaledWindow* CCanvas::GetCapture()
{
    return m_currentCapture;
}

//-----------------------------------------------------------------------------

CScaledWindow* CCanvas::HookKbdMsgs(CScaledWindow* hooker)
{
    CScaledWindow* oldhook=m_currentKbdHook;

    m_currentKbdHook=hooker;

    return oldhook;
}

//-----------------------------------------------------------------------------

CScaledWindow* CCanvas::GetKbdHook()
{
    return m_currentKbdHook;
}

//-----------------------------------------------------------------------------

bool CCanvas::ClearCurrentHot(BUTTON_STATE newstate)
{
    bool bChange=false;

    if(m_currentHot)
        bChange=m_currentHot->SetHot(false, newstate);

    m_currentHot=0;
    m_overPathToFocus.Release();

    if(m_appVisMode==RelavintTimeline)
        m_hilightID=CUniqueID();

    return bChange;
}

//-----------------------------------------------------------------------------

CScaledWindow* CCanvas::GetCurrentHot()
{
    if (!m_currentHot)
        return NULL;
    if (!m_currentHot->IsHot())
        return NULL;
    return m_currentHot->m_parent;
}

//-----------------------------------------------------------------------------

bool CCanvas::ClearFocus()
{
    bool bFocusTaken=m_currentFocus!=0;

    if(bFocusTaken)
    {
        m_currentFocus->SetFocus(false);
        m_currentFocus=0;
        Invalidate();
    }

    // can't have keyboard hook without the focus
    m_currentKbdHook=0;

    return bFocusTaken;
}

//-----------------------------------------------------------------------------

void CCanvas::DeselectAll()
{
    WindowList::iterator itr=m_selectedList.begin();

    // deselect all others and remove them from the list
    while(itr!=m_selectedList.end())
    {
        itr->wnd->m_bSelected=false;
        itr->wnd->Invalidate();
        itr=m_selectedList.erase(itr);
    }
    m_selectedImage.Clear();
    NotifySelection(CUniqueID());
}

//-----------------------------------------------------------------------------

unsigned CCanvas::CountSelected() const
{
    return m_selectedList.size();
}

//-----------------------------------------------------------------------------

unsigned CCanvas::GetSelected(IUniqueIDContainer* results)
{
    ATLASSERT(results);
    unsigned count=m_selectedList.size();
    if(count>0)
    {
        WindowList::iterator itr=m_selectedList.begin();
        while(itr!=m_selectedList.end())
        {
            results->Add(itr->id);
            ++itr;
        }
    }

    return count;
}

//-----------------------------------------------------------------------------

void CCanvas::MoveAllSelected(const CPoint& offset, CScaledWindow* skip)
{
    WindowList::iterator itr=m_selectedList.begin();
    CRect rc;

    while(itr!=m_selectedList.end())
    {
        if(itr->wnd!=skip)
        {
            itr->wnd->GetWindowRect(rc);
            rc.OffsetRect(offset);
            itr->wnd->SetWindowPos(rc, SWP_NOSIZE);
        }
        ++itr;
    }
}

//-----------------------------------------------------------------------------

bool CCanvas::SetHilightID(const CUniqueID& id)
{
    bool bChange=id!=m_hilightID;

    if(bChange)
        m_hilightID=id;

    return bChange;
}

//-----------------------------------------------------------------------------

const CUniqueID& CCanvas::GetHilightID() const
{
    return m_hilightID;
}

//-----------------------------------------------------------------------------

void CCanvas::SelectItemsWithin(const RectF &r)
{
    CComPtr<IRenderer> ir;
    CComQIPtr<IGraphSubgraph> sg;
    RectF box;
    CComPtr<MetaVisibleSkipper> skipper = new MetaVisibleSkipper(m_meta);

    for(CComPtr<IGraphItemIterator> itr = m_graph->GetChildren(skipper); itr->IsValidItem(); itr->NextItem())
    {
        ir=GetRenderer(itr->Get());			

        if(ir!=0)
        {
            ir->GetBoundsBox(box);
            if(box.IntersectsWith(r))
            {
                sg=itr->Get();
                if(sg)
                {
                    if(GetApplicationMode()==AppModeGrETL)
                    {
                        // multiple selection of subgraphs is not allowed
                        unsigned selcount=m_selectedList.size();
                        if(selcount>0)
                            continue; 
                        else
                            ir->SetSelected(true, true);
                    }
                    else
                        ir->SetSelected(true, true);
                }
                else
                    ir->SetSelected(true, true);
            }
            else
                ir->SetSelected(false);
        }
    }
}

//-----------------------------------------------------------------------------

GraphTypes::RectF CCanvas::getGraphExtent(bool selectedOnly, GraphTypes::Coordinates co) const
{
    RectF retVal;

    if(!selectedOnly)
    {
        // this extent is stored in canvas whenever topmost graph draws itself
        GetGraphExtent(retVal, co);
    }
    else if(GetApplicationMode() == AppModeQBGraph)
    {
        bool first = true;
        CComPtr<MetaStateSkipper> skipper = new MetaStateSkipper(m_meta, this, TRISTATE_BOTH, TRISTATE_BOTH, TRISTATE_TRUE);
        for(CComPtr<IVertexIterator> itr = m_graph->GetUserVertexIterator(skipper); itr->IsValidItem(); itr->NextItem())
        {
            CComPtr<IGraphVertex> vertex = itr->Get();
            CComQIPtr<IAttVertexRenderer> vr = GetRenderer(vertex);
            if (first)
            {
                vr->GetBoundsBox(retVal, co);
                first = false;
            }
            else
            {
                RectF rc;
                vr->GetBoundsBox(rc, co);
                RectF::Union(retVal, retVal, rc);
            }
        }
    }
    else
    {
        RectF rc;
        bool first = true;

        // BUG 14490 : Recenter / Size was incorrectly including invisible nodes
        // so now we skip them.

        CComPtr<IRenderer> ir;
        CComQIPtr<IListWindowRenderer> lw;
        CComQIPtr<IVertexRenderer> lv;

        for(CComPtr<IGraphItemIterator> itr = m_graph->GetChildren(new MetaStateSkipper(m_meta, this, TRISTATE_TRUE, TRISTATE_BOTH, TRISTATE_TRUE)); itr->IsValidItem(); itr->NextItem())
        {
            CComPtr<IGraphItem> item = itr->Get();
            ir=GetRenderer(item);	
            if(ir!=0)
            {
                lv=ir.p;
                lw=ir.p;

                if(lv!=0)
                {
                    if (first)
                    {
                        lv->GetTitlePos(retVal);
                        ATLASSERT(!retVal.IsEmptyArea());
                        first = false;
                    }
                    else
                    {
                        lv->GetTitlePos(rc);
                        ATLASSERT(!rc.IsEmptyArea());
                        retVal.Union(retVal, retVal, rc);
                    }

                    // images rect could be empty
                    lv->GetImagesBoundsBox(rc);
                    if(!rc.IsEmptyArea())
                        retVal.Union(retVal, retVal, rc);
                }
                else if (lw!=0)
                {
                    if (first)
                    {
                        lw->GetBoundsBox(retVal);
                        first = false;
                    }
                    else
                    {
                        lw->GetBoundsBox(rc);
                        retVal.Union(retVal, retVal, rc);
                    }
                }
            }
        }
    }

    if (co == world)
        RAllTransform(retVal);

    return retVal;
}

//-----------------------------------------------------------------------------

GraphTypes::RectF CCanvas::getGraphExtent(bool selectedOnly) const
{
    return getGraphExtent(selectedOnly, GraphTypes::screen);
}

//-----------------------------------------------------------------------------

int CCanvas::GetFontPixHeight() const
{
    int pixheight=::MulDiv(GetFontPointSize(), GetDPIY(), 72);
    if(pixheight<1)
        pixheight=1;
    return pixheight;
}

//-----------------------------------------------------------------------------

void CCanvas::SetRunning(bool running)
{
    m_running = running;
}

//-----------------------------------------------------------------------------
//
// get the center of the screen in world coordinates

void CCanvas::GetScreenCenter(GraphTypes::PointF& center) const
{
    RectF rc;
    GetClient(rc);
    center.x=rc.Width/2.0f;
    center.y=rc.Height/2.0f;
    RAllTransform(center); 
}

//-----------------------------------------------------------------------------
//
// center param is desired new center point in world coordinates

bool CCanvas::CenterAt(const PointF& center)
{
    bool bChanged=false;
    PointF centerscr=center;
    AllTransform(centerscr); // screen coords of desired center

    RectF rc;
    GetClient(rc);

    // difference to add to current offset in order to get to the center
    PointF diffscr(centerscr.x-rc.Width/2, centerscr.y-rc.Height/2);

    if(diffscr.x!=0 || diffscr.y!=0)
    {
        PointF cur=GetOffset();

        cur.x+=diffscr.x;
        cur.y+=diffscr.y;

        SetOffset(cur);
#ifdef _DEBUG
        // check that new screen center is the desired one
        cur.x=rc.Width/2;
        cur.y=rc.Height/2;
        RAllTransform(cur); // current screen center in world coordinates
        diffscr.x=center.x-cur.x;
        diffscr.y=center.y-cur.y;
        ATLASSERT(fabs(diffscr.x)<0.1 && fabs(diffscr.y)<0.1);
#endif
        bChanged=true;
    }
    return bChanged;
}

//-----------------------------------------------------------------------------

void CCanvas::ClearCaches()
{
    IGraphRenderer* igr=reinterpret_cast<IGraphRenderer*>(m_graph->GetInternalUnknown(MAIN_RENDERER_IDX));
    igr->ClearCaches();
}

//-----------------------------------------------------------------------------

void CCanvas::ScaleMargins(MARGINS& margins)
{
    SizeF scaledtl(REAL(margins.cxLeftWidth), REAL(margins.cyTopHeight));
    SizeF scaledbr(REAL(margins.cxRightWidth), REAL(margins.cyBottomHeight));

    SizeTransform(scaledtl);
    SizeTransform(scaledbr);

    margins.cxLeftWidth=int(scaledtl.Width);
    margins.cyTopHeight=int(scaledtl.Height);
    margins.cxRightWidth=int(scaledbr.Width);
    margins.cyBottomHeight=int(scaledbr.Height);
}

//-----------------------------------------------------------------------------

MOUSE_MODES CCanvas::CalcMouseMode()
{
    bool bCtrl=(::GetAsyncKeyState(VK_CONTROL)&0x8000)!=0;
    bool bShift=(::GetAsyncKeyState(VK_SHIFT)&0x8000)!=0;
    if(bShift)
    {
        if(bCtrl)
        {
            //ATLTRACE(_T("CalcMouseMode: SHIFT and CTRL\r\n"));
            m_mouseMode=MOUSE_MODE_DRAW_LINKS;
        }
        else
        {
            //ATLTRACE(_T("CalcMouseMode: SHIFT\r\n"));
            m_mouseMode=MOUSE_MODE_SELECTION;
        }
    }
    else
    {
        //ATLTRACE(_T("CalcMouseMode: Neither SHIFT nor CTRL\r\n"));
        m_mouseMode=m_meta.GetMouseMode();
    }

    return m_mouseMode;
}

//-----------------------------------------------------------------------------

void CCanvas::SetModeCursor()
{
    switch(m_mouseMode)
    {
    case MOUSE_MODE_DRAW_LINKS:
        //ATLTRACE(_T("SetModeCursor: Draw Links\r\n"));
        SetCursor(LoadCursor(NULL, IDC_CROSS));
        break;
    case MOUSE_MODE_SELECTION:
        //ATLTRACE(_T("SetModeCursor: Selection\r\n"));
        SetCursor(LoadCursor(NULL, IDC_ARROW));
        break;
    case MOUSE_MODE_GRAB:
        bool bButtonDown=(::GetAsyncKeyState(VK_LBUTTON)&0x8000)!=0;
        //if(bButtonDown)
        //	ATLTRACE(_T("SetModeCursor: Grab down\r\n"));
        //else
        //	ATLTRACE(_T("SetModeCursor: Grab up\r\n"));
        SetCursor(bButtonDown?m_HandClosedCursor:m_HandOpenCursor);
        break;
    }
}

//-----------------------------------------------------------------------------

void CCanvas::SetZoomModeCursor()
{
    switch(m_appZoomMode)
    {
    case AZMSelection:
        SetCursor(LoadCursor(NULL, IDC_ARROW));
        break;
    case AZMZoomIn:
        SetCursor(m_ZoomInCursor);
        break;
    case AZMZoomOut:
        SetCursor(m_ZoomOutCursor);
        break;
    case AZMMouseWheel:
        SetCursor(LoadCursor(NULL, IDC_CROSS));
        break;
    }
}

//-----------------------------------------------------------------------------

bool CCanvas::AttachEditor(IGraphItem* item)
{
    bool bSuccess=false;
    CComPtr<IUnknown> unk;
    CComPtr<ITextboxRenderer> ir;

    ATLASSERT(m_edit==0);

    // for now only vertices
    CComQIPtr<IGraphVertex> v=item;
    ATLASSERT(v);

    // ask parent (if any) if it allows editing this item
    CComPtr<IGraphSubgraph> parent=v->GetParent();
    if(parent)
    {
        CComPtr<IListWindowRenderer> ir=
            static_cast<IListWindowRenderer*>(GetRenderer(parent));
        bSuccess=ir->OnEditItem(item);
    }

    if(bSuccess)
    {
        unk=GetRenderer(item);
        bSuccess=unk!=0;
    }

    if(bSuccess)
    {
        RectF rcf;
        ir=static_cast<ITextboxRenderer*>(unk.p);
        ir->GetBoundsBox(rcf);
        // account for scrollbars
        RectF vp;
        GetViewport(vp);
        if(vp.Contains(rcf))
        {
            CRect rc=static_cast<CRect>(rcf);
            m_edit=new CItemEdit(item, this);

            m_edit->Create(
                m_wndCanvas,
                &rc,
                0,
                WS_CHILD | WS_VISIBLE,
                0,
                100,
                0);

            bSuccess=m_edit->IsWindow()!=0;
            if(bSuccess)
            {
                ReleaseCapture();
                m_edit->SetFocus();
                m_wndCanvas.SetCapture();
            }
        }
        else
            bSuccess=false;
    }

    return bSuccess;
}

//-----------------------------------------------------------------------------

void CCanvas::DestroyEditor()
{
    m_edit->DestroyWindow();
    delete m_edit;
    m_edit=0;
    ::ReleaseCapture();
}

//-----------------------------------------------------------------------------

void CCanvas::GetViewport(RectF& vp) const
{
    IRenderer* igr=GetRenderer(m_graph);
    // this way viewport rectangle does not include scrollbars
    igr->GetBoundsBox(vp);
}

//-----------------------------------------------------------------------------

void CCanvas::GetClient(GraphTypes::RectF& cl) const
{
    CRect rc;
    m_wndCanvas.GetClientRect(&rc);
    cl.x=REAL(rc.left);
    cl.y=REAL(rc.top);
    cl.Width=REAL(rc.Width());
    cl.Height=REAL(rc.Height());
}

//-----------------------------------------------------------------------------

void CCanvas::DrawGrid(CDCHandle& dc)
{
    CAutoPen pen(m_gridPen, dc);
    PointF pointsf[4];
    CPoint points[5];

    // x-axis

    pointsf[0].x=-32000;
    pointsf[0].y=0;
    pointsf[1].x=32000;
    pointsf[1].y=0;

    AllTransform(pointsf[0]);
    AllTransform(pointsf[1]);

    points[0]=static_cast<CPoint>(pointsf[0]);
    points[1]=static_cast<CPoint>(pointsf[1]);

    dc.MoveTo(points[0]);
    dc.LineTo(points[1]);

    // y-axis

    pointsf[0].x=0;
    pointsf[0].y=-32000;
    pointsf[1].x=0;
    pointsf[1].y=32000;

    AllTransform(pointsf[0]);
    AllTransform(pointsf[1]);

    points[0]=static_cast<CPoint>(pointsf[0]);
    points[1]=static_cast<CPoint>(pointsf[1]);

    dc.MoveTo(points[0]);
    dc.LineTo(points[1]);
}

//-----------------------------------------------------------------------------

void CCanvas::Scroll(const CPoint& scroll)
{
    NotifyScroll(scroll);

    if(!m_meta.IsEmulateDecoration())
        return;

    RectF vp;
    GetViewport(vp);
    RectF newextent(m_graphExtent);
    newextent.Offset(REAL(scroll.x),REAL(scroll.y));

    BOOL bOk=FALSE;

    if(!m_meta.IsOverviewVisible() && vp.Contains(m_graphExtent))
    {
        m_graphExtent=newextent;

        CClientDC dc(m_wndCanvas);

        bOk=dc.ScrollDC(
            scroll.x,
            scroll.y,
            0,
            0,
            0,
            0);
    }

    if(!bOk)
        Invalidate();

    //RectF vp;
    //GetViewport(vp);

    //CRect scrollrc=static_cast<CRect>(vp);
    //CRect update;

    //CClientDC dc(m_wndCanvas);

    //if(m_meta.IsOverviewVisible())
    //{
    //	RectF box;
    //	m_overviewRenderer->GetBoundsBox(box);
    //	CRect rco=static_cast<CRect>(box);
    //	dc.ExcludeClipRect(&rco);
    //}

    //BOOL bOk=dc.ScrollDC(
    //		scroll.x,
    //		scroll.y,
    //		&scrollrc,
    //		&scrollrc,
    //		0,
    //		&update);

    //
    //ATLASSERT(bOk);
    //if(bOk)
    //{
    //	if(scroll.x!=0 && scroll.y!=0)
    //	{
    //		if(m_meta.IsOverviewVisible())
    //		{
    //			Invalidate(&update);
    //		}
    //		else
    //		{	
    //			CRect r1(update), r2(update);
    //			if(scroll.x>0)
    //				r1.right=r1.left+scroll.x;
    //			else
    //				r1.left=r1.right+scroll.x;

    //			if(scroll.y>0)
    //				r2.bottom=r1.top+scroll.y;
    //			else
    //				r2.top=r2.bottom+scroll.y;

    //			Invalidate(&r1);
    //			m_wndCanvas.UpdateWindow();
    //			Invalidate(&r2);
    //			m_wndCanvas.UpdateWindow();
    //		}
    //	}
    //	else
    //	{
    //		Invalidate(&update);
    //	}
    //}
    //else
    //{
    //	Invalidate();
    //}
}


//-----------------------------------------------------------------------------

#ifdef _DEBUG_THEME
void CCanvas::FlipDebugTheme()
{
    bdebugTheme=!bdebugTheme;	
}
#endif

//-----------------------------------------------------------------------------

CPartTheme& CCanvas::GetPartTheme(const TCHAR* part)
{
    CPartTheme* theme;

#ifdef _DEBUG_THEME
    static CPartTheme dumbTheme(_T(""));
    if(bdebugTheme)
        return dumbTheme;
#endif

    theme=m_themeCache.Exists(part);
    if(!theme)
        theme = m_themeCache.Get(new CPartTheme(part));
    ATLASSERT(theme);

    return *theme;
}

//-----------------------------------------------------------------------------

CCaptionFont& CCanvas::GetCaptionFont(int size)
{
    CCaptionFont* font;
    font=m_captionFontCache.Exists(size);
    if(!font)
        font = m_captionFontCache.Get(new CCaptionFont(size));
    ATLASSERT(font);

    return *font;
}

//-----------------------------------------------------------------------------

CListFont& CCanvas::GetListFont(int size)
{
    CListFont* font;
    font=m_listFontCache.Exists(size);
    if(!font)
        font = m_listFontCache.Get(new CListFont(size));
    ATLASSERT(font);

    return *font;
}

//-----------------------------------------------------------------------------

void CCanvas::NotifyThemeChanged()
{
    PartThemeCacheMap::iterator itr=m_themeCache.begin();

    while(itr!=m_themeCache.end())
    {
        (*itr).second->NotifyThemeChanged();
        ++itr;
    }
}

//-----------------------------------------------------------------------------

bool CCanvas::HasVertexFocus()
{
    if (!m_currentFocus)
        return false;

    IGraphRenderer* igr=reinterpret_cast<IGraphRenderer*>(m_graph->GetInternalUnknown(MAIN_RENDERER_IDX));
    return !igr->GetFocus();
}
