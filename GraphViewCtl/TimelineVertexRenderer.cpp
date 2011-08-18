
#include "StdAfx.h"

#include "Canvas.h"
#include "ScaledWindows.h"
#include "AggDC.h"
#include "AggMemText.h"

#define __SKIP_INVISIBLE__
//#define LINE_WRAP

class CTimelineVertexRenderer 
	: public ITimelineVertexRenderer,
	public CUnknown,
	public CScaledWindow,
	public boost::signals::trackable
{
	IGraphVertex * m_v;	//Should not be a CComPtr as this would create a circular reference.
	MARGINS m_textMargins;
	CComPtr<IImageRenderer> m_image;
	//CButtonState m_hotTracking;
	boost::signals::connection m_sigConn;

	GraphTypes::PointF m_position;
	bool m_bPinned;

	CAggMemText m_textBmpNormal;
	CAggMemText m_textBmpSelected;
	// sizes that vary with each DPI 
	SizeFMap m_extentMap;

	GraphTypes::PointF m_savePos;
	GraphTypes::PointF m_capturePos;


	void Draw(CAggDC& dc);
	bool CalcVisibility();
	virtual void SetWindowPos(const CRect& pos, UINT flags);

public:
	BEGIN_CUNKNOWN
		IMPLEMENT_INTERFACE(ITimelineVertexRenderer)
		IMPLEMENT_INTERFACE(ILegacyRenderer)
		IMPLEMENT_INTERFACE(IRenderer)
	END_CUNKNOWN(CUnknown)

	CTimelineVertexRenderer(CCanvas & canvas);
	~CTimelineVertexRenderer();

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

	// ILegacyRenderer
	void GetTitlePos(GraphTypes::RectF& rcpos);
	void GetImagesBoundsBox(GraphTypes::RectF& box);
	bool IsSelected();
	bool IsHot();
	bool GetPinned();
	void SetPinned(bool pinned);
	bool GetDragging();
	bool HitTest(const GraphTypes::PointF& point);

protected:
	BEGIN_MSG_MAP(CTimelineVertexRenderer)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_RBUTTONDOWN, OnRButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
	END_MSG_MAP()

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnRButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};

//*****************************************************************************

using namespace GraphTypes;

CTimelineVertexRenderer::CTimelineVertexRenderer(CCanvas & canvas)
	: CScaledWindow(&canvas),
	m_v(0),
	//m_hotTracking(&canvas, this),
	m_bPinned(false)
{
	m_textMargins.cxLeftWidth=4;
	m_textMargins.cxRightWidth=4;
	m_textMargins.cyTopHeight=4;
	m_textMargins.cyBottomHeight=4;

	m_textBmpNormal.SetMargins(m_textMargins);
	m_textBmpSelected.SetMargins(m_textMargins);
}

//-----------------------------------------------------------------------------

CTimelineVertexRenderer::~CTimelineVertexRenderer()
{
	//if(m_hotTracking.m_state==BS_HOT)
	//{
	//	m_hotTracking.SetHot(false);
	//	m_canvas->NotifyHotChanged(GetID(), false);
	//}
}

//-----------------------------------------------------------------------------

CUniqueID CTimelineVertexRenderer::GetID()
{
	return m_v->GetID();
}

//-----------------------------------------------------------------------------

void CTimelineVertexRenderer::SetGraphItem(IGraphItem* item)
{
	CComQIPtr<IGraphVertex> v=item;
	ATLASSERT(v);

	if(m_v)
		m_v->Disconnect(m_sigConn);
	m_v=v;
	m_sigConn=m_v->Connect(boost::ref(*this));

	m_image=static_cast<IImageRenderer*>(CreateImageRenderer(*m_canvas, item));
}

//-----------------------------------------------------------------------------

void CTimelineVertexRenderer::GetBoundsBox(RectF& box, GraphTypes::Coordinates co)
{
	GetTitlePos(box);
	if(co==world)
	{
		m_canvas->RAllTransform(box);
	}
}

//-----------------------------------------------------------------------------

void CTimelineVertexRenderer::SetBoundsBox(const RectF& box, unsigned flags, GraphTypes::Coordinates co)
{
	RectF rc = box;
	if(co==world)
	{
		m_canvas->AllTransform(rc);
	}
	SetWindowPos(rc, flags);
}

//-----------------------------------------------------------------------------

void CTimelineVertexRenderer::GetPosition(PointF& pt, Coordinates co/*=world*/)
{
	pt=m_position;

	if(co==screen)
	{
		m_canvas->AllTransform(pt);
	}
}

//-----------------------------------------------------------------------------

void CTimelineVertexRenderer::SetPosition(const PointF& pt, Coordinates co/*=world*/)
{
	m_position=pt;

	if(co==screen)
	{
		m_canvas->RAllTransform(m_position);		
	}
}

//-----------------------------------------------------------------------------

void CTimelineVertexRenderer::SetSelected(bool bSelected/*=true*/, bool bAdd/*=false*/)
{
//	bool oldSelected = m_bSelected;
//	CScaledWindow::SetSelected(GetID(), bSelected, bAdd);
//	if (bSelected != oldSelected)
//		m_canvas->NotifySelection(GetID());
}

//-----------------------------------------------------------------------------

void CTimelineVertexRenderer::SetFocus(bool bSet/*=true*/)
{
	//bool bChange=bSet!=HasFocus();
	//CScaledWindow::SetFocus(bSet);
	//if(bChange)
	//	m_canvas->NotifyFocusChanged(GetID(), bSet);
	//if(bSet)
	//	m_v->MoveToEnd();
}

//-----------------------------------------------------------------------------

LRESULT CTimelineVertexRenderer::SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return CScaledWindow::SendMessage(uMsg, wParam, lParam);
}

//-----------------------------------------------------------------------------

GraphTypes::SizeF CTimelineVertexRenderer::CalcDisplaySizes(
	CDCHandle& dc, 
	bool bForce,
	bool bIgnoreExtent)
{
	SizeF extentf;

	std::_tstring label = m_v->GetLabel();
	if (!label.length())
	{
		label = boost::lexical_cast<std::_tstring>(m_v->GetID().GetCategory().GetCategory()).c_str();
		label += _T(":");
		label += m_v->GetID().GetID();
	}

	const TCHAR *lstr=label.c_str();

	if(m_canvas->IsPrinting())
	{
		extentf=m_extentMap[m_canvas->GetDpiScale()];

		if(bForce || extentf.Height==0)
		{
			CAutoFont f(m_canvas->GetListFont(m_canvas->GetFontPixHeight()), dc);
			CSize szExtent;

#ifndef LINE_WRAP
			dc.GetTextExtent(lstr, label.length(), &szExtent);
#else
			CRect rctext(0,0,0,0);
			dc.DrawText(
				lstr,
				label.length(),
				&rctext,
				DT_CALCRECT |
				DT_EDITCONTROL |
				DT_CENTER |
				DT_NOPREFIX);

			szExtent.cx=rctext.Width();
			szExtent.cy=rctext.Height();
#endif
			extentf.Width=REAL(szExtent.cx);
			extentf.Height=REAL(szExtent.cy);

			MARGINS scaled=m_textMargins;
			m_canvas->ScaleMargins(scaled);

			extentf.Width+=scaled.cxLeftWidth+scaled.cxRightWidth;
			extentf.Height+=scaled.cyTopHeight+scaled.cyBottomHeight;
			m_extentMap[m_canvas->GetDpiScale()]=extentf;
		}
	}
	else
	{
		m_textBmpNormal.CalcSize(lstr, m_canvas);
		m_textBmpSelected.CalcSize(lstr, m_canvas);
		extentf=m_textBmpNormal.GetScaledSize();

	}

	ATLASSERT(m_image!=0);
	m_image->CalcDisplaySizes(dc, bForce, bIgnoreExtent);

	return extentf;
}

//-----------------------------------------------------------------------------

void CTimelineVertexRenderer::SetRedraw(bool bEnable)
{
	CScaledWindow::SetRedraw(bEnable);
}

//-----------------------------------------------------------------------------

void CTimelineVertexRenderer::ClearCaches()
{
	m_extentMap.clear();
	m_textBmpNormal.Clear();
	m_textBmpSelected.Clear();
}

//-----------------------------------------------------------------------------
//
// text rectangle in screen coordinates
//
void CTimelineVertexRenderer::GetTitlePos(GraphTypes::RectF& rcpos)
{
	SizeF size;

	if(m_canvas->IsPrinting())
	{
		size=m_extentMap[m_canvas->GetDpiScale()];
	}
	else
	{
		//if(m_hotTracking.m_state==BS_HOT)
		//	size=m_textBmpNormal.GetSize();
		//else
			size=m_textBmpNormal.GetScaledSize();
	}

	PointF pos=m_position;
	m_canvas->AllTransform(pos);
	RectF rcf(pos, size);
	rcf.Offset(-rcf.Width/2,-rcf.Height/2);
	m_rcWnd = static_cast<CRect>(rcf);
	rcpos=rcf;
}

//-----------------------------------------------------------------------------

void CTimelineVertexRenderer::GetImagesBoundsBox(GraphTypes::RectF& box)
{
	m_image->GetBoundsBox(box);
}

//-----------------------------------------------------------------------------

bool CTimelineVertexRenderer::IsSelected()
{
	//return m_bSelected; // member in CScaledWindow 
	return false;
}

//-----------------------------------------------------------------------------

bool CTimelineVertexRenderer::IsHot()	
{
//	return m_hotTracking.m_state==BS_HOT;
	return false;
}

//-----------------------------------------------------------------------------

bool CTimelineVertexRenderer::GetPinned()
{
	return m_bPinned;
}

//-----------------------------------------------------------------------------

void CTimelineVertexRenderer::SetPinned(bool pinned)
{
	if(pinned!=m_bPinned)
	{
		m_bPinned=pinned;
		Invalidate();
	}
}
//-----------------------------------------------------------------------------

bool CTimelineVertexRenderer::GetDragging()
{
	if(m_canvas->GetCapture()==this && IsSelected())
	{
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------

bool CTimelineVertexRenderer::HitTest(const PointF& point)
{
	bool bHit=false;

	RectF rctext;
	GetTitlePos(rctext);

	if(rctext.Contains(point))
		bHit=true;
	else
		bHit=m_image->HitTest(point);

	return bHit;
}

//-----------------------------------------------------------------------------

bool CTimelineVertexRenderer::CalcVisibility()
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

void CTimelineVertexRenderer::SetWindowPos(const CRect& pos, UINT flags)
{
	RectF rc(pos);
	m_position.x=rc.x+rc.Width/2.0f;
	m_position.y=rc.y+rc.Height/2.0f;
	m_canvas->RAllTransform(m_position);

	CScaledWindow::SetWindowPos(pos, flags);
}

//-----------------------------------------------------------------------------

void CTimelineVertexRenderer::Invalidate(CRect* rc/*=0*/, bool bErase/*=false*/) 
{
	if(IsRedraw())
		m_canvas->Invalidate();
}

//-----------------------------------------------------------------------------

void CTimelineVertexRenderer::operator()(
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

void CTimelineVertexRenderer::Draw(CAggDC& dc) 
{
	std::_tstring label = m_v->GetLabel();
	if (!label.length())
	{
		label = boost::lexical_cast<std::_tstring>(m_v->GetID().GetCategory().GetCategory()).c_str();
		label += _T(":");
		label += m_v->GetID().GetID();
	}

	Color fillColor, borderColor, fontColor;
	CCategory cat = m_v->GetCategory();
	fontColor=m_canvas->m_meta.GetMetaVertexFontColor(cat);

	// don't draw hot state on printer device
	//if(m_hotTracking.m_state==BS_HOT && !m_canvas->IsPrinting())
	//{
	//	fillColor=m_canvas->m_meta.GetMetaVertexColorFillOver(cat);
	//	borderColor=m_canvas->m_meta.GetMetaVertexColorBorderOver(cat);
	//	fontColor=m_canvas->m_meta.GetMetaVertexFontColorSel(cat);
	//}
	//else
	{
		if(IsSelected())
		{
			fillColor=m_canvas->m_meta.GetMetaVertexColorFillSel(cat);
			borderColor=m_canvas->m_meta.GetMetaVertexColorBorderSel(cat);
			fontColor=m_canvas->m_meta.GetMetaVertexFontColorSel(cat);
		}
		else
		{
			// neither selected nor hot/over
			fillColor=m_canvas->m_meta.GetMetaVertexColorFillNorm(cat);
			borderColor=m_canvas->m_meta.GetMetaVertexColorBorderNorm(cat);

			if (m_v->GetBorderColor().GetValue() != 0x0)
				borderColor.SetValue(m_v->GetBorderColor().GetValue());
			if (m_v->GetFillColor().GetValue() != 0x0)
				fillColor.SetValue(m_v->GetFillColor().GetValue());
			if (m_v->GetFontColor().GetValue() != 0x0)
				fontColor.SetValue(m_v->GetFontColor().GetValue());
		}

	}

	RectF rcf;
	GetTitlePos(rcf);
	CRect frameRect=static_cast<CRect>(rcf);

	if(!frameRect.Width() || !frameRect.Height())
		return;

//	CPoint pc(-1,-1);
//	m_canvas->ClientCursorPos(&pc);
//	bool hot=frameRect.PtInRect(pc);
	bool hot=false;
	if(hot && !m_canvas->IsPrinting())
	{
		fillColor=m_canvas->m_meta.GetMetaVertexColorFillOver(cat);
		borderColor=m_canvas->m_meta.GetMetaVertexColorBorderOver(cat);
		fontColor=m_canvas->m_meta.GetMetaVertexFontColorSel(cat);
	}

	if(!m_canvas->IsPrinting())
	{
		REAL lineWidth=REAL(1.0);

		//if(m_hotTracking.m_state!=BS_HOT)
		if(!hot)
		{
			if(IsSelected())
				m_textBmpSelected.Prepare(dc, label.c_str(), fontColor, fillColor, borderColor, lineWidth, m_canvas);
			else
				m_textBmpNormal.Prepare(dc, label.c_str(), fontColor, fillColor, borderColor, lineWidth, m_canvas);
		}

		//if(m_hotTracking.m_state!=BS_HOT)
		if(!hot)
		{
			if(IsSelected())
				m_textBmpSelected.Draw(dc, frameRect, 0);
			else
				m_textBmpNormal.Draw(dc, frameRect, 0);
		}
		else
		{
			RectF rcframe(
				REAL(frameRect.left),
				REAL(frameRect.top),
				REAL(frameRect.Width()),
				REAL(frameRect.Height()));

			REAL halfLineWidth=lineWidth/2;

			PointF tl(rcframe.x+halfLineWidth, rcframe.y+halfLineWidth);
			PointF br(rcframe.x+rcframe.Width-halfLineWidth, rcframe.y+rcframe.Height-halfLineWidth);

			dc.Rect(
				tl, 
				br,
				fillColor,
				borderColor,
				lineWidth,
				true);

			rcframe.Inflate(
				-REAL(m_textMargins.cxLeftWidth),
				-REAL(m_textMargins.cyTopHeight));

			PointF descsubtract(REAL(0), REAL(2));
			m_canvas->DpiTransform(descsubtract);
			dc.DrawScaledText(
				label.c_str(),
				rcframe,
				fontColor,
				"Verdana", 
				(DEFAULT_FONT_SIZE-1)*m_canvas->GetDPIY()/72,
				descsubtract);
		}
	}
	else
	{
		int isave=dc.SaveDC();
		CDCHandle hdc(dc.m_hDC);

		CPen borderPen;
		borderPen.CreatePen(PS_SOLID, 1, borderColor.ToCOLORREF());
		CBrush fillBrush;
		fillBrush.CreateSolidBrush(fillColor.ToCOLORREF());

		CAutoPen p(borderPen, hdc);
		CAutoBrush b(fillBrush, hdc);
		CAutoFont f(m_canvas->GetListFont(m_canvas->GetFontPixHeight()), hdc);

		dc.SetTextColor(fontColor.ToCOLORREF());
		dc.SetBkColor(fillColor.ToCOLORREF());
		dc.Rectangle(&frameRect);

		CRect textRect=frameRect;

		MARGINS scaled=m_textMargins;

		// don't scale margins for scale factors >1.0 since it would clip the text
		if(m_canvas->GetScale()<1.0)
			m_canvas->ScaleMargins(scaled);

		textRect.DeflateRect(
			scaled.cxLeftWidth,
			scaled.cyTopHeight,
			scaled.cxLeftWidth,
			scaled.cyBottomHeight);

		dc.DrawText(
			label.c_str(),
			label.length(),
			&textRect,
#ifndef LINE_WRAP
			DT_SINGLELINE |
			DT_VCENTER |
#else
			DT_EDITCONTROL |
#endif
			DT_CENTER |
			DT_NOPREFIX);

		dc.RestoreDC(isave);
	}

	if(m_bPinned)
	{
		int pinSize = 2;
		dc.Rectangle(
			frameRect.left - pinSize,
			frameRect.top - pinSize,
			frameRect.left + pinSize,
			frameRect.top + pinSize);
		dc.Rectangle(
			frameRect.right - pinSize,
			frameRect.top - pinSize,
			frameRect.right + pinSize,
			frameRect.top + pinSize);
		dc.Rectangle(
			frameRect.left - pinSize,
			frameRect.bottom - pinSize,
			frameRect.left + pinSize,
			frameRect.bottom + pinSize);
		dc.Rectangle(
			frameRect.right - pinSize,
			frameRect.bottom - pinSize,
			frameRect.right + pinSize,
			frameRect.bottom + pinSize);
	}

	if(HasFocus()&& frameRect.Height()>8 )
	{
		frameRect.DeflateRect(2,2);
		dc.DrawFocusRect(&frameRect);
	}
}

//-----------------------------------------------------------------------------

LRESULT CTimelineVertexRenderer::OnPaint(
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam,
	BOOL& /*bHandled*/)
{
	if(m_canvas->m_meta.IsVisible(m_v))
	{
		CAggDC& dc=*(CAggDC*)wParam;

		// for printer device we don't calculate visibility
		if(m_canvas->IsPrinting() || CalcVisibility())
		{
			Draw(dc);
			m_image->SendMessage(uMsg, wParam, /*m_hotTracking.m_state==BS_HOT ? 0 : 1*/ 1);
		}
	}

	return 0;
}

//-----------------------------------------------------------------------------
//
//	The very fact thatwe are getting this message means that upper level code
//	found mouse clicked either over this vertex or over one of its images.
//

LRESULT CTimelineVertexRenderer::OnLButtonDown(
	UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)); 

	// nobody can be hot when we get capture
	m_canvas->ClearCurrentHot();

	//SetFocus();

	//if((wParam & MK_CONTROL)==0 && m_canvas->CountSelected()<2)
	//{
	//	SetSelected();
	//}

	if (m_canvas->CalcMouseMode() != MOUSE_MODE_DRAW_LINKS)
	{
		m_canvas->SetCapture(this);
		m_capturePos=pt;

		RectF rcf;
		GetTitlePos(rcf);

		m_savePos.x=rcf.x;
		m_savePos.y=rcf.y;

		SetCursor(LoadCursor(NULL, IDC_ARROW));
	}

	return reinterpret_cast<LRESULT>(m_v);
}

//-----------------------------------------------------------------------------

LRESULT CTimelineVertexRenderer::OnRButtonDown(
	UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	return reinterpret_cast<LRESULT>(m_v);
}

//-----------------------------------------------------------------------------

LRESULT CTimelineVertexRenderer::OnLButtonUp(
	UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(m_canvas->GetCapture()==this)
	{
		CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)); 

		m_canvas->ReleaseCapture();

		RectF rcf;
		GetTitlePos(rcf);
		m_rcWnd=static_cast<CRect>(rcf);

		if(m_rcWnd.PtInRect(pt))
		{
			//m_hotTracking.SetHot(true);			
			//m_canvas->NotifyHotChanged(GetID(), true);
			// when hot become topmost to avoid other vertices showing through
			//m_v->MoveToEnd();
		}

		//if((wParam & MK_CONTROL)!=0)
		//{
		//	SetSelected(!IsSelected(), true);
		//}
		//else if(m_canvas->CountSelected()>1)
		//{
		//	SetSelected(true, (wParam & MK_SHIFT)!=0 );
		//}
	}

	return 0;
}

//-----------------------------------------------------------------------------

LRESULT CTimelineVertexRenderer::OnMouseMove(
	UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(m_canvas->GetCapture()==this)
	{
		CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)); 

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

		//if(IsSelected())
		//{
		//	m_canvas->MoveAllSelected(offset, this);
		//}
		//else
		//{
		//	singleItem = GetID();
		//}

		//RectF viewRect;
		//m_canvas->GetViewport(viewRect);
		//CRect rcv=static_cast<CRect>(viewRect); // for int instead of float comparisons

		//if (m_canvas->DragDropNotifications() && !rcv.PtInRect(pt))
		//{
		//	m_canvas->ReleaseCapture();
		//	if(IsSelected())
		//	{
		//		offset.x = round_int(m_savePos.x - rcf.x) - offset.x;
		//		offset.y = round_int(m_savePos.y - rcf.y) - offset.y;
		//		m_canvas->MoveAllSelected(offset, NULL);
		//	}
		//	m_canvas->NotifyDoDragDrop(m_capturePos);
		//	return 0;
		//}
	}
	else
	{
		// when hot become topmost to avoid other vertices showing through
		//m_v->MoveToEnd();
		//m_hotTracking.SetHot(true);
		//m_canvas->NotifyHotChanged(GetID(), true);
	}

	if (m_canvas->CalcMouseMode() != MOUSE_MODE_DRAW_LINKS)
		SetCursor(LoadCursor(NULL, IDC_ARROW));

	return 0;
}

//*****************************************************************************

IRenderer *CreateTimelineVertexRenderer(CCanvas & parent)
{
	CTimelineVertexRenderer * retVal = new CTimelineVertexRenderer(parent);
	return retVal;
}
