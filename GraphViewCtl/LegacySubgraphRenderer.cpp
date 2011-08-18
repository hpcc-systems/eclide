#include "StdAfx.h"

#include "Canvas.h"
#include "ScaledWindows.h"
#include "AggDC.h"
#include "AggBitmap.h"
#include "DrawVertexHelper.h"
#include "SizeTracker.h"

#define __SKIP_INVISIBLE__

class CLegacySubgraphRenderer : public ILegacySubgraphRenderer, public CScaledWindow, public boost::signals::trackable
{
	IGraphSubgraph* m_g;

	MARGINS m_textMargins;
	CComPtr<IImageRenderer> m_image;
	CButtonState m_hotTracking;
	boost::signals::connection m_sigConn;

	GraphTypes::PointF m_position;
	bool m_bPinned;

	CAggBitmap m_textBmpNormal;
	CAggBitmap m_textBmpSelected;
	CSizeTracker m_sizeTracker;

	GraphTypes::PointF m_savePos;
	GraphTypes::PointF m_capturePos;
	bool m_hitSubgraph;

	void CalcColors(
		GraphTypes::Color& fillColor,
		GraphTypes::Color& borderColor,
		GraphTypes::Color& fontColor);

	void Draw(CAggDC& dc);
	bool CalcVisibility();
	virtual void SetWindowPos(const CRect& pos, UINT flags);
	void MoveAllChildren();

public:
	BEGIN_CUNKNOWN
		IMPLEMENT_INTERFACE(ILegacySubgraphRenderer)
		IMPLEMENT_INTERFACE(IListWindowRenderer)
		IMPLEMENT_INTERFACE(IRenderer)
	END_CUNKNOWN(CScaledWindow)

	CLegacySubgraphRenderer(CCanvas & canvas);
	~CLegacySubgraphRenderer();

	virtual void Invalidate(CRect* rc=0, bool bErase=false); 
	void operator()(GRAPHNOTIFICATION modification, IGraphItem * item);

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

	// IListWindowRenderer

	void GetContainerRect(GraphTypes::RectF& rc) const;
	IGraphItem* HitTestChildren(const GraphTypes::PointF& point);
	void GetConnectBounds(
		GraphTypes::RectF& left, 
		GraphTypes::RectF& right,
		unsigned order);
	bool IsSelected();
	bool IsMinimized() const;
	void OnRemoveGraphItem(IGraphItem* gi);
	bool OnEditItem(IGraphItem* gi);
	void SetWindowState(WINDOW_SIZE size, int topindex);
	void GetWindowState(WINDOW_SIZE& size, int& topindex) const;
	void SetWindowSize(const GraphTypes::SizeF& size);
	void GetWindowSize(GraphTypes::SizeF& size) const;

	// ILegacySubgraphRenderer

	LRESULT HitTestDispatchMsg(const GraphTypes::PointF& point, bool& hit,
		 UINT uMsg, WPARAM wParam, LPARAM lParam);
	void GetTitlePos(GraphTypes::RectF& rcpos);
	void GetImagesBoundsBox(GraphTypes::RectF& box);
	bool GetPinned();
	void SetPinned(bool pinned);

protected:
	BEGIN_MSG_MAP(CLegacySubgraphRenderer)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_RBUTTONDOWN, OnRButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnMouseDblclk)
		MESSAGE_HANDLER(WM_NCHITTEST, OnNcHitTest)
		MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
	END_MSG_MAP()

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnRButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnMouseDblclk(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnNcHitTest(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSetCursor(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};

//-----------------------------------------------------------------------------

using namespace GraphTypes;

//*****************************************************************************

CLegacySubgraphRenderer::CLegacySubgraphRenderer(CCanvas & canvas)
	: CScaledWindow(&canvas), 
	m_g(0),
	m_hotTracking(&canvas, this),
	m_bPinned(false)
{
	m_textMargins.cxLeftWidth=4;
	m_textMargins.cxRightWidth=4;
	m_textMargins.cyTopHeight=4;
	m_textMargins.cyBottomHeight=4;
}

//-----------------------------------------------------------------------------

CLegacySubgraphRenderer::~CLegacySubgraphRenderer()
{
	if(m_hotTracking.IsHot())
	{
		m_hotTracking.SetHot(false);
		m_canvas->NotifyHotChanged(GetID(), false);
	}
}

//-----------------------------------------------------------------------------

CUniqueID CLegacySubgraphRenderer::GetID()
{
	return m_g->GetID();
}

//-----------------------------------------------------------------------------

void CLegacySubgraphRenderer::SetGraphItem(IGraphItem* item)
{
	CComQIPtr<IGraphSubgraph> g=item;
	ATLASSERT(g);

	if(m_g)
		m_g->Disconnect(m_sigConn);
	m_g=g;
	m_sigConn=m_g->Connect(boost::ref(*this));

	m_image=reinterpret_cast<IImageRenderer*>(CreateImageRenderer(*m_canvas, item));
	ATLASSERT(m_image);
	m_image->SetParentRenderer(this);
}

//-----------------------------------------------------------------------------

void CLegacySubgraphRenderer::GetBoundsBox(RectF& box, GraphTypes::Coordinates co)
{
	GetTitlePos(box);
	if(co==world)
	{
		m_canvas->RAllTransform(box);
	}
}

//-----------------------------------------------------------------------------

void CLegacySubgraphRenderer::SetBoundsBox(const RectF& box, unsigned flags, GraphTypes::Coordinates co)
{
	RectF rc = box;
	if(co==world)
	{
		m_canvas->AllTransform(rc);
	}
	SetWindowPos(rc, flags);
}

//-----------------------------------------------------------------------------

void CLegacySubgraphRenderer::GetPosition(PointF& pt, Coordinates co/*=world*/)
{
	pt=m_position;

	if(co==screen)
	{
		m_canvas->AllTransform(pt);
	}
}

//-----------------------------------------------------------------------------

void CLegacySubgraphRenderer::SetPosition(const PointF& pt, Coordinates co/*=world*/)
{
	m_position=pt;

	if(co==screen)
	{
		m_canvas->RAllTransform(m_position);		
	}

	MoveAllChildren();
}

//-----------------------------------------------------------------------------

LRESULT CLegacySubgraphRenderer::SendMessage(
	UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return CScaledWindow::SendMessage(uMsg, wParam, lParam);
}

//-----------------------------------------------------------------------------

void CLegacySubgraphRenderer::SetFocus(bool bSet)
{
	// parent will never have focus but the other guy must lose it 
	if(bSet)
		m_canvas->ClearFocus();
}

//-----------------------------------------------------------------------------

void CLegacySubgraphRenderer::GetContainerRect(GraphTypes::RectF& rc) const
{
	rc=m_rcWnd;
}

//-----------------------------------------------------------------------------

IGraphItem* CLegacySubgraphRenderer::HitTestChildren(const PointF& point)
{
	ATLASSERT(0); // don't know why would we be using this for now
	CComPtr<IGraphItem> gi;
	RectF rc;
	CComPtr<IRenderer> ir;
	CComPtr<IUnknown> unk;

	for(CComPtr<IGraphItemIterator> itr = m_g->GetChildren(); itr->IsValidItem(); itr->NextItem())
	{
		ir=m_canvas->GetRenderer(itr->Get());
		if(ir!=0)
		{
			ir->GetBoundsBox(rc);

			if(rc.Contains(point))
			{
				gi=itr->Get();
				break;
			}
		}
	}

	return gi;
}

//-----------------------------------------------------------------------------

void CLegacySubgraphRenderer::GetConnectBounds(
	GraphTypes::RectF& left, 
	GraphTypes::RectF& right,
	unsigned order)
{
	// nothing to do for now
}

//-----------------------------------------------------------------------------

void CLegacySubgraphRenderer::SetSelected(bool bSelected/*=true*/, bool bAdd/*=false*/)
{
	bool oldSelected = m_bSelected;
	CScaledWindow::SetSelected(GetID(), bSelected, bAdd);

	if (bSelected != oldSelected)
	{
		m_canvas->NotifySelection(GetID());
	}
}

//-----------------------------------------------------------------------------

bool CLegacySubgraphRenderer::IsSelected()
{
	return m_bSelected;
}

//-----------------------------------------------------------------------------

bool CLegacySubgraphRenderer::IsMinimized() const
{
	return true;
}

//-----------------------------------------------------------------------------

void CLegacySubgraphRenderer::OnRemoveGraphItem(IGraphItem* gi)
{
	// nothing to do for now
}

//-----------------------------------------------------------------------------

bool CLegacySubgraphRenderer::OnEditItem(IGraphItem* gi)
{
	bool bOk=false;

	return bOk;
}

//-----------------------------------------------------------------------------

void CLegacySubgraphRenderer::SetWindowState(WINDOW_SIZE size, int topindex)
{
	// nothing to do for now
}

//-----------------------------------------------------------------------------

void CLegacySubgraphRenderer::GetWindowState(WINDOW_SIZE& size, int& topindex) const
{
	// nothing to do for now
}

//-----------------------------------------------------------------------------

void CLegacySubgraphRenderer::SetWindowSize(const GraphTypes::SizeF& size)
{
	// nothing for this graph
}

//-----------------------------------------------------------------------------

void CLegacySubgraphRenderer::GetWindowSize(GraphTypes::SizeF& size) const
{
	// nothing for this graph
}

//-----------------------------------------------------------------------------

SizeF CLegacySubgraphRenderer::CalcDisplaySizes(
	CDCHandle& dc, 
	bool bForce,
	bool bIgnoreExtent)
{
	SizeF extentf;

	std::_tstring label = m_g->GetLabel();
	if (!label.length())
	{
		label = boost::lexical_cast<std::_tstring>(m_g->GetID().GetCategory().GetCategory()).c_str();
		label += _T(":");
		label += m_g->GetID().GetID();
	}

	if(m_sizeTracker.Update(m_canvas))
	{
		FontInfo fi(
			&m_canvas->GetListFont(m_canvas->IsPrinting()?m_canvas->GetFontPixHeight():DEFAULT_FONT_SIZE),
			0,
			m_canvas->m_fonts,
			(DEFAULT_FONT_SIZE-1)*m_canvas->GetDPIY()/72);

		CDrawVertexHelper tdh;
		tdh.m_borderwidth=1;
		tdh.m_text=label.c_str();
		tdh.m_margins=m_textMargins;
		tdh.m_printing=m_canvas->IsPrinting();
		tdh.m_scale=m_canvas->GetScale();

		if(m_canvas->IsPrinting())
		{
			extentf=m_sizeTracker.m_extentMap[m_canvas->GetDpiScale()];

			if(bForce || extentf.Height==0)
			{
				extentf=tdh.CalcSize(dc, fi);
				m_sizeTracker.m_extentMap[m_canvas->GetDpiScale()]=extentf;
			}
		}
		else
		{
			m_sizeTracker.m_scaledExtent=extentf=tdh.CalcSize(dc, fi);
			if(m_sizeTracker.m_extent.Width==0)
			{
				tdh.m_scale=1;
				m_sizeTracker.m_extent=tdh.CalcSize(dc, fi);
			}
		}

		m_textBmpNormal.Clear();
		m_textBmpSelected.Clear();
	}

	ATLASSERT(m_image);
	m_image->CalcDisplaySizes(dc, bForce, bIgnoreExtent);

	return extentf;
}

//-----------------------------------------------------------------------------

void CLegacySubgraphRenderer::SetRedraw(bool bEnable)
{
	CScaledWindow::SetRedraw(bEnable);
}

//-----------------------------------------------------------------------------

void CLegacySubgraphRenderer::ClearCaches()
{
	m_sizeTracker.Reset();
	m_textBmpNormal.Clear();
	m_textBmpSelected.Clear();
}

//-----------------------------------------------------------------------------

LRESULT CLegacySubgraphRenderer::HitTestDispatchMsg(
	const GraphTypes::PointF& point, bool& hit, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes=0;
	hit=false;

	RectF rctext;
	GetTitlePos(rctext);

	if(rctext.Contains(point))
	{
		hit=true;
		if(uMsg!=WM_USER) // known message we do not process, just hit testing
			lRes=CScaledWindow::SendMessage(uMsg, wParam, lParam);
		if(uMsg==WM_NCHITTEST)
			m_hitSubgraph=true;
		else if(uMsg==WM_LBUTTONDOWN)
		{
			// image selection is orthogonal to vertex selection so
			// images won't be deselected automaticaly when vertex is selected
			if(IsSelected())
				m_canvas->m_selectedImage.Clear();
		}
	}
	else
	{
		ATLASSERT(m_image);
		lRes=m_image->HitTestDispatchMsg(point, hit, uMsg, wParam, lParam);
		if(uMsg==WM_NCHITTEST)
		{
			m_hitSubgraph=false;
		}
		else if(uMsg==WM_RBUTTONDOWN)
		{
			if(hit)
				lRes=CScaledWindow::SendMessage(uMsg, wParam, lParam);
		}
		else if(uMsg==WM_LBUTTONDOWN)
		{
			if(hit && !m_image->HasCapture())
				lRes=CScaledWindow::SendMessage(uMsg, wParam, lParam);
		}
		else if(uMsg==WM_LBUTTONDBLCLK)
		{
			if(hit && lRes) // image render returns 1 if no resizing handle was hit
				lRes=CScaledWindow::SendMessage(uMsg, wParam, lParam);
		}
	}

	return lRes;

	//TODO don't dispatch to children for now since we don't know what it'll do

	RectF rc;
	IRenderer* ir;

	for(CComPtr<IGraphItemIterator> itr = m_g->GetChildren(); itr->IsValidItem(); itr->NextItem())
	{
		ir=m_canvas->GetRenderer(itr->Get());
		ATLASSERT(ir!=0);

		ir->GetBoundsBox(rc);

		if(rc.Contains(point))
		{
			lRes=ir->SendMessage(uMsg, wParam, lParam);
			hit=true;
			break;
		}
	}

	return lRes;
}

//-----------------------------------------------------------------------------
//
// text rectangle in screen coordinates
//
void CLegacySubgraphRenderer::GetTitlePos(GraphTypes::RectF& rcpos)
{
	SizeF size;

	if(m_canvas->IsPrinting())
	{
		size=m_sizeTracker.m_extentMap[m_canvas->GetDpiScale()];
	}
	else
	{
		if(m_hotTracking.IsHot())
			size=m_sizeTracker.m_extent;
		else
			size=m_sizeTracker.m_scaledExtent;
	}

	PointF pos=m_position;
	m_canvas->AllTransform(pos);
	RectF rcf(pos, size);
	rcf.Offset(-rcf.Width/2,-rcf.Height/2);
	m_rcWnd = static_cast<CRect>(rcf);
	rcpos=rcf;
}

//-----------------------------------------------------------------------------

void CLegacySubgraphRenderer::GetImagesBoundsBox(GraphTypes::RectF& box)
{
	m_image->GetBoundsBox(box);
}

//-----------------------------------------------------------------------------

bool CLegacySubgraphRenderer::GetPinned()
{
	return m_bPinned;
}

//-----------------------------------------------------------------------------

void CLegacySubgraphRenderer::SetPinned(bool pinned)
{
	if(pinned!=m_bPinned)
	{
		m_bPinned=pinned;
		Invalidate();
	}
}

//-----------------------------------------------------------------------------

bool CLegacySubgraphRenderer::CalcVisibility()
{
	bool visible=true;
#ifdef __SKIP_INVISIBLE__
	RectF rcunion, rcimages;
	GetTitlePos(rcunion);

	GetImagesBoundsBox(rcimages);

	if(!rcimages.IsEmptyArea())
		rcunion.Union(rcunion, rcunion, rcimages);

	RectF vp;
	m_canvas->GetViewport(vp);
	visible=vp.IntersectsWith(rcunion)!=FALSE;
#endif
	return visible;
}

//-----------------------------------------------------------------------------

void CLegacySubgraphRenderer::SetWindowPos(const CRect& pos, UINT flags)
{
	RectF rc(pos);
	m_position.x=rc.x+rc.Width/2.0f;
	m_position.y=rc.y+rc.Height/2.0f;
	m_canvas->RAllTransform(m_position);

	CScaledWindow::SetWindowPos(pos, flags);
	MoveAllChildren();
}

//-----------------------------------------------------------------------------

void CLegacySubgraphRenderer::MoveAllChildren()
{
	PointF pt=m_position;
	m_canvas->AllTransform(pt); // work in screen

	for(CComPtr<IGraphItemIterator> itr = m_g->GetChildren(); itr->IsValidItem(); itr->NextItem())
	{
		ILegacyRenderer* ir=reinterpret_cast<ILegacyRenderer*>(m_canvas->GetRenderer(itr->Get()));
		ATLASSERT(ir!=0);
		// maybe center on subgraph's center
		//ir->GetBoundsBox(bbox);
		//ir->GetImagesBoundsBox(imgbbox);
		//if(!imgbbox.IsEmptyArea())
		//	bbox.Union(bbox, bbox, imgbbox);
		ir->SetPosition(pt, screen);
	}
}

//-----------------------------------------------------------------------------

void CLegacySubgraphRenderer::Invalidate(CRect* rc/*=0*/, bool bErase/*=false*/) 
{
	if(IsRedraw())
		m_canvas->Invalidate();
}

//-----------------------------------------------------------------------------

void CLegacySubgraphRenderer::operator()(
	GRAPHNOTIFICATION notification, IGraphItem * item)
{
	switch(notification)
	{
	case GRAPHNOTIFICATION_LABELCHANGED:
		ClearCaches();
		break;
	}
}

//-----------------------------------------------------------------------------

void CLegacySubgraphRenderer::CalcColors(
	Color& fillColor, Color& borderColor, Color& fontColor)
{
	CCategory cat = m_g->GetCategory();

	fontColor=m_canvas->m_meta.GetMetaSubgraphFontColor(cat);

	// don't draw hot state on printer device
	if(m_hotTracking.IsHot() && !m_canvas->IsPrinting())
	{
		fillColor=m_canvas->m_meta.GetMetaSubgraphColorFillOver(cat);
		borderColor=m_canvas->m_meta.GetMetaSubgraphColorBorderOver(cat);
		fontColor=m_canvas->m_meta.GetMetaSubgraphFontColorSel(cat);
	}
	else
	{
		if(IsSelected())
		{
			fillColor=m_canvas->m_meta.GetMetaSubgraphColorFillSel(cat);
			borderColor=m_canvas->m_meta.GetMetaSubgraphColorBorderSel(cat);
			fontColor=m_canvas->m_meta.GetMetaSubgraphFontColorSel(cat);
		}
		else
		{
			// neither selected nor hot/over
			fillColor=m_canvas->m_meta.GetMetaSubgraphColorFillNorm(cat);
			borderColor=m_canvas->m_meta.GetMetaSubgraphColorBorderNorm(cat);

			if (m_g->GetBorderColor().GetValue() != 0x0)
				borderColor.SetValue(m_g->GetBorderColor().GetValue());
			if (m_g->GetFillColor().GetValue() != 0x0)
				fillColor.SetValue(m_g->GetFillColor().GetValue());
			if (m_g->GetFontColor().GetValue() != 0x0)
				fontColor.SetValue(m_g->GetFontColor().GetValue());
		}
	}
}

//-----------------------------------------------------------------------------

void CLegacySubgraphRenderer::Draw(CAggDC& dc) 
{
	std::_tstring label = m_g->GetLabel();
	if (!label.length())
	{
		label = boost::lexical_cast<std::_tstring>(m_g->GetID().GetCategory().GetCategory()).c_str();
		label += _T(":");
		label += m_g->GetID().GetID();
	}

	RectF rcf;
	GetTitlePos(rcf);
	CRect frameRect=static_cast<CRect>(rcf);

	if(!frameRect.Width() || !frameRect.Height())
		return;

	Color fillColor, borderColor, fontColor;
	CalcColors(fillColor, borderColor, fontColor);

	FontInfo fi(
		&m_canvas->GetListFont(m_canvas->IsPrinting()?m_canvas->GetFontPixHeight():DEFAULT_FONT_SIZE),
		fontColor,
		m_canvas->m_fonts,
		(DEFAULT_FONT_SIZE-1)*m_canvas->GetDPIY()/72);

	CDrawVertexHelper tdh;
	tdh.m_text=label.c_str();
	tdh.m_borderwidth=1.0;
	tdh.m_cbkgnd=fillColor;
	tdh.m_cborder=borderColor;
	tdh.m_margins=m_textMargins;
	tdh.m_printing=m_canvas->IsPrinting();
	tdh.m_scale=m_canvas->GetScale();
	tdh.m_focusRect=HasFocus()&& frameRect.Height()>8;
	tdh.m_pins=m_bPinned;

	if(m_canvas->IsPrinting())
	{
		tdh.Draw(dc, fi, rcf);
	}
	else
	{
		if(m_hotTracking.IsHot())
		{
			tdh.m_scale=1;
			RectF rchot(
				REAL(frameRect.left), 
				REAL(frameRect.top),
				REAL(frameRect.Width()), 
				REAL(frameRect.Height()));

			tdh.Draw(dc, fi, rchot);
		}
		else
		{
			if(IsSelected())
			{
				if(m_textBmpSelected.IsEmpty())
				{
					CSize size=m_sizeTracker.m_scaledExtent;

					boost::scoped_ptr<CAggDC> tmpaggdc(m_textBmpSelected.BeginPaint(dc, size, m_canvas->m_fonts));
					RectF rcf(0.0, 0.0, m_sizeTracker.m_scaledExtent.Width, m_sizeTracker.m_scaledExtent.Height);
					CDrawVertexHelper tmptdh(tdh);
					tmptdh.m_focusRect=false;
					tmptdh.m_pins=false;
					tmptdh.Draw(*tmpaggdc, fi, rcf);
					m_textBmpSelected.EndPaint(tmpaggdc.get());
				}

				m_textBmpSelected.Draw(dc, frameRect, 0);
				tdh.DrawFocusAndPin(dc, rcf);
			}
			else
			{
				if(m_textBmpNormal.IsEmpty())
				{
					CSize size=m_sizeTracker.m_scaledExtent;

					boost::scoped_ptr<CAggDC> tmpaggdc(m_textBmpNormal.BeginPaint(dc, size, m_canvas->m_fonts));
					RectF rcf(0.0, 0.0, m_sizeTracker.m_scaledExtent.Width, m_sizeTracker.m_scaledExtent.Height);
					CDrawVertexHelper tmptdh(tdh);
					tmptdh.m_focusRect=false;
					tmptdh.m_pins=false;
					tmptdh.Draw(*tmpaggdc, fi, rcf);
					m_textBmpNormal.EndPaint(tmpaggdc.get());
				}

				m_textBmpNormal.Draw(dc, frameRect, 0);
				tdh.DrawFocusAndPin(dc, rcf);
			}
		}
	}
}


//*****************************************************************************
//
// Message handlers
//

LRESULT CLegacySubgraphRenderer::OnPaint(
	UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(m_canvas->m_meta.IsVisible(m_g))
	{
		CAggDC& dc=*(CAggDC*)wParam;

		// for printer device we don't calculate visibility
		if(m_canvas->IsPrinting() || CalcVisibility())
		{
			Draw(dc);
			m_image->SendMessage(uMsg, wParam, lParam);
		}
	}

	return 0;
}

//-----------------------------------------------------------------------------

LRESULT CLegacySubgraphRenderer::OnLButtonDown(
	UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)); 

	// nobody can be hot when we get capture
	m_canvas->ClearCurrentHot();

	SetFocus();

	if((wParam & MK_CONTROL)==0 && m_canvas->CountSelected()<2)
	{
		SetSelected();
	}

	if(m_canvas->GetMouseMode()!=MOUSE_MODE_DRAW_LINKS)
	{
		m_canvas->SetCapture(this);
		m_capturePos=pt;

		RectF rcf;
		GetTitlePos(rcf);

		m_savePos.x=rcf.x;
		m_savePos.y=rcf.y;
	}

	return reinterpret_cast<LRESULT>(m_g);
}

//-----------------------------------------------------------------------------

LRESULT CLegacySubgraphRenderer::OnRButtonDown(
	UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	m_canvas->ClearCurrentHot();

	// like in Windows Explorer selection does not change if CTRL key
	// is pressed and if user clicks over already selected item
	if((wParam & MK_CONTROL)==0)
	{
		if(!IsSelected())
			SetSelected();
	}

	return reinterpret_cast<LRESULT>(m_g);
}

//-----------------------------------------------------------------------------

LRESULT CLegacySubgraphRenderer::OnLButtonUp(
	UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)); 

	if(m_canvas->GetCapture()==this)
	{
		m_canvas->ReleaseCapture();

		RectF rcf;
		GetTitlePos(rcf);
		m_rcWnd=static_cast<CRect>(rcf);

		if(m_rcWnd.PtInRect(pt))
		{
			m_hotTracking.SetHot(true);			
			m_canvas->NotifyHotChanged(GetID(), true);
			// when hot become topmost to avoid other vertices showing through
			m_g->MoveToEnd();
			if(m_canvas->GetApplicationVisMode()==RelavintTimeline)
				m_canvas->SetHilightID(GetID());
		}

		if((wParam & MK_CONTROL)!=0)
		{
			SetSelected(!IsSelected(), true);
		}
		else if(m_canvas->CountSelected()>1)
		{
			SetSelected(true, (wParam & MK_SHIFT)!=0 );
		}
	}

	m_canvas->NotifyMouseMessage(uMsg, GetID(), pt);

	return 0;
}

//-----------------------------------------------------------------------------

LRESULT CLegacySubgraphRenderer::OnMouseMove(
	UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)); 

	if(m_canvas->GetCapture()==this)
	{
		// nobody could be hot during capture
		m_canvas->ClearCurrentHot();

		RectF rcf;
		GetTitlePos(rcf);
		CRect rc=static_cast<CRect>(rcf);

		CPoint offset(
			pt.x+round_int(m_savePos.x-m_capturePos.x)-rc.left,
			pt.y+round_int(m_savePos.y-m_capturePos.y)-rc.top);

		if (offset.x == 0 && offset.y == 0)
			return 0;

		rc.OffsetRect(offset);
		SetWindowPos(rc, SWP_NOSIZE);

		CUniqueID singleItem; // starts empty, i.e. item(s) selected

		if(IsSelected())
		{
			m_canvas->MoveAllSelected(offset, this);
		}
		else
		{
			singleItem = GetID();
		}

		RectF viewRect;
		m_canvas->GetViewport(viewRect);
		CRect rcv=static_cast<CRect>(viewRect); // for int instead of float comparisons

		if (m_canvas->DragDropNotifications() && !rcv.PtInRect(pt))
		{
			m_canvas->ReleaseCapture();
			if(IsSelected())
			{
				offset.x = round_int(m_savePos.x - rcf.x) - offset.x;
				offset.y = round_int(m_savePos.y - rcf.y) - offset.y;
				m_canvas->MoveAllSelected(offset, NULL);
			}
			m_canvas->NotifyDoDragDrop(m_capturePos);
			return 0;
		}
	}
	else
	{
		// when hot become topmost to avoid other vertices showing through
		m_g->MoveToEnd();
		m_hotTracking.SetHot(true);
		m_canvas->NotifyHotChanged(GetID(), true);
	}

	return 0;
}

//-----------------------------------------------------------------------------

LRESULT CLegacySubgraphRenderer::OnMouseDblclk(
	UINT uMsg, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)); 
	m_canvas->NotifyMouseMessage(uMsg, GetID(), pt);
	return 0;
}

//-----------------------------------------------------------------------------

LRESULT CLegacySubgraphRenderer::OnNcHitTest(
	UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	return HTCLIENT;
}

//-----------------------------------------------------------------------------

LRESULT CLegacySubgraphRenderer::OnSetCursor(
	UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	LRESULT lRes=1;

	if(m_hitSubgraph)
	{
		// No links for subgraph?
		//MOUSE_MODES mode=m_canvas->CalcMouseMode();
		//if(mode==MOUSE_MODE_DRAW_LINKS)
		//	m_canvas->SetModeCursor();
		//else
		SetCursor(LoadCursor(NULL, IDC_ARROW));
	}
	else
	{
		lRes=m_image->SendMessage(uMsg, wParam, lParam);
	}

	return lRes;
}


//*****************************************************************************

IRenderer* CreateLegacySubgraphRenderer(CCanvas & canvas)
{
	CLegacySubgraphRenderer * retVal = new CLegacySubgraphRenderer(canvas);
	return retVal;
}

