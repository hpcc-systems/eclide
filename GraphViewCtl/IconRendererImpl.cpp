
#include "StdAfx.h"

#include "Canvas.h"
#include "ScaledWindows.h"
#include "Renderer.h"

using namespace GraphTypes;

//*****************************************************************************

class CIconRenderer 
	: public IIconRenderer,
	public CUnknown,
	public CScaledWindow
{
	CCanvas & m_canvas;
	IGraphItem * m_gi; 

	void Draw(CDCHandle& dc);

public:
	IMPLEMENT_CUNKNOWNQI(IIconRenderer);

	CIconRenderer(
		CCanvas & canvas,
		IGraphItem* item);

	// IRenderer

	CUniqueID GetID();
	void SetGraphItem(IGraphItem* item);
	void GetBoundsBox(GraphTypes::RectF& box);
	void SetBoundsBox(const GraphTypes::RectF& box, unsigned flags);
	void GetPosition(GraphTypes::PointF& pt, GraphTypes::Coordinates co);
	void SetPosition(const GraphTypes::PointF& pt, GraphTypes::Coordinates co);
	void SetSelected(bool bSelected=true, bool bAdd=false);
	void SetFocus(bool bSet=true);
	LRESULT SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	GraphTypes::SizeF CalcDisplaySizes(CDCHandle& dc,
		bool bForce=false,
		bool bIgnoreExtent=false);	

	// IIconRenderer

protected:
	BEGIN_MSG_MAP(CIconRenderer)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
	END_MSG_MAP()

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};

//*****************************************************************************

CIconRenderer::CIconRenderer(
	CCanvas & canvas, IGraphItem* item)
	: CScaledWindow(&canvas), m_canvas(canvas), m_gi(item)
{
}

//-----------------------------------------------------------------------------

CUniqueID CIconRenderer::GetID()
{
	return m_gi->GetID();
}

//-----------------------------------------------------------------------------

void CIconRenderer::SetGraphItem(IGraphItem* item)
{
	m_gi=item;
}

//-----------------------------------------------------------------------------

void CIconRenderer::SetFocus(bool bSet)
{
	// CScaledWindow::SetFocus(bSet);
}

//-----------------------------------------------------------------------------

void CIconRenderer::GetBoundsBox(RectF& box)
{
// TODO Discover use for this
}

//-----------------------------------------------------------------------------

void CIconRenderer::SetBoundsBox(const RectF& box, unsigned flags)
{
// TODO Discover use for this
}

//-----------------------------------------------------------------------------

void CIconRenderer::GetPosition(PointF& pt, Coordinates co/*=world*/)
{
	// nothing to do for now
}

//-----------------------------------------------------------------------------

void CIconRenderer::SetPosition(const PointF& pt, Coordinates co/*=world*/)
{
	// nothing to do for now
}

//-----------------------------------------------------------------------------

void CIconRenderer::SetSelected(bool bSelected/*=true*/, bool bAdd/*=false*/)
{
//	TODO Discover use for this
//	CScaledWindow::SetSelected(bSelected, bAdd);
}

//-----------------------------------------------------------------------------

LRESULT CIconRenderer::SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return CScaledWindow::SendMessage(uMsg, wParam, lParam);
}

//-----------------------------------------------------------------------------

GraphTypes::SizeF CIconRenderer::CalcDisplaySizes(
	CDCHandle& dc, 
	bool bForce,
	bool bIgnoreExtent)
{
	// TODO Discover use for this

	SizeF extentf;

	return extentf;
}

//-----------------------------------------------------------------------------

void CIconRenderer::Draw(CDCHandle &dc) 
{
	CComQIPtr<IGraphVertex> v;
	CComQIPtr<IGraphEdge> e;

	v=m_gi;
	e=m_gi;

	// try to get overridden specific icon / image for a vertex
	// nb: an icon is used by the managed control, but the web page can only send
	// an HTMLElement

	// TODO: in future the web client might want to override the icon but for now they
	// cannot, and can only use the meta-data defined one.
	HTMLElement* iconAsHtmlElement = NULL;  // &m_v->GetIconHtmlElement();
	PLACEMENT placement=PLACEMENT_CENTER;
	VISIBLE vis=VISIBLE_HIDE;

	if(v)
	{
		CComQIPtr<IPicture> icon;
		icon = v->GetIcon();
		// if none found, try to get one for the meta-data defined category
		if (icon==0)
			icon=m_canvas.m_meta.GetMetaVertexIcon(v->GetCategory());
		if (iconAsHtmlElement==0)
			iconAsHtmlElement=m_canvas.m_meta.GetMetaVertexHtmlElement(v->GetCategory());
		// TODO: in theory the vertex could override the meta-data defined location for the
		// icon's position
		placement = m_canvas.m_meta.GetMetaVertexIconPlacement();
		if(icon!=0 || iconAsHtmlElement !=0)
		{
			CComPtr<IUnknown> unk;
			CComQIPtr<ILegacyRenderer> lv;

			unk=m_gi->GetPropertyUnknown(0);

			if(unk)
				lv=unk.p;

			if(lv!=0)
			{
				RectF rc;
				lv->GetBoundsBox(rc);
				rc=m_canvas.DrawBitmap(
					dc,
					rc,
					icon,
					iconAsHtmlElement,
					placement);
			}
		}
	}
	else if(e)
	{
		CComPtr<IRenderer> ir;
		CComPtr<IUnknown> ipichtml;
		SizeF size;
		RectF rcedge;

		ir=static_cast<IRenderer*>(e->GetPropertyUnknown(0));
		ATLASSERT(ir);
		ir->GetBoundsBox(rcedge);

		ipichtml=e->GetIcon();
		if(ipichtml!=0)
		{
			vis=e->GetIconVisible();
			if(vis!=VISIBLE_HIDE)
			{
				e->GetIconSize(size);
				placement=e->GetIconPlacement();
				if(placement==PLACEMENT_DEFAULT)
					placement=m_canvas.m_meta.GetMetaEdgeIconPlacement(e->GetCategory());
			}
		}
		else // look for meta information
		{
			const CCategory& ec=e->GetCategory();
			ipichtml=m_canvas.m_meta.GetMetaEdgeIcon(ec);	
			if(ipichtml!=0)
			{
				vis=m_canvas.m_meta.GetMetaEdgeIconVisible(ec);
				if(vis!=VISIBLE_HIDE)
				{
					m_canvas.m_meta.GetMetaEdgeIconSize(ec, size);
					placement=m_canvas.m_meta.GetMetaEdgeIconPlacement(ec);
				}
			}
		}

		if(vis!=VISIBLE_HIDE && (rcedge.x!=0 && rcedge.y!=0))
			m_canvas.DrawBitmap(dc, ipichtml, rcedge, size, placement);
	}
	else
	{
		ATLASSERT(0);
	}
}

//-----------------------------------------------------------------------------

LRESULT CIconRenderer::OnPaint(
	UINT /*uMsg*/,
	WPARAM wParam,
	LPARAM /*lParam*/,
	BOOL& /*bHandled*/)
{
	CDCHandle dc=*(CDCHandle*)wParam;
//	if(dc.RectVisible(&m_rcWnd))
		Draw(dc);	

	return 0;
}

//-----------------------------------------------------------------------------

LRESULT CIconRenderer::OnLButtonDown(
	UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	//SetFocus();
	//SetSelected();
	return 0;
}

//*****************************************************************************

IRenderer* CreateIconRenderer(
	CCanvas & parent,
	IGraphItem * item)
{
	CIconRenderer * retVal = new CIconRenderer(parent, item);
	return retVal;
}
