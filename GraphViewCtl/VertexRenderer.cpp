#include "StdAfx.h"
#include "Resource.h"

#include "gdiimpl.h"
#include "ScaledWindows.h"
#include "ScaledList.h"

#include "VertexRenderer.h"


#ifdef DEBUG
void PrintRect(const CRect& rc)
{
	AtlTrace("Rect top:%d, left:%d, width:%d, height:%d\r\n", rc.top, rc.left, rc.Width(), rc.Height());
}
#endif

class CVertexRenderer : public IRenderer, public CUnknown, public CScaledWindow
{
protected:
	CGraphFastRenderImpl & m_renderParent;
	CMeta & m_meta;
	CComPtr<IGraph> m_graph;
	CTransform & m_transform;
	CComPtr<IGraphVertex> m_v;
	GraphTypes::PointF m_dragStartPos;
	CPoint m_capturePos;

	RectFMap m_title_pos;
	RectFMap m_icon_pos;
	RectFMap m_image_pos;

	std::_tstring m_label;

public:
	IMPLEMENT_CUNKNOWN;
	CVertexRenderer(CGraphFastRenderImpl & parent, IGraphVertex * v);
	~CVertexRenderer();

	CUniqueID GetID();
	const TCHAR * GetLabel();

	RECT CalcTextBounds(CDCHandle& dc);
	const GraphTypes::RectF CalculateTitleRect(CDCHandle& dc);

	const GraphTypes::RectF & GetTitlePos();
	void SetTitlePos(const GraphTypes::RectF & r);
	const GraphTypes::RectF & GetIconPos();
	void SetIconPos(const GraphTypes::RectF & r);
	const GraphTypes::RectF & GetImagePos();
	void SetImagePos(const GraphTypes::RectF & r);
	bool IsVisible();
	bool IsSelected();

	virtual void GetBoundsBox(GraphTypes::RectF& r);
	virtual void SetBoundingBox(const GraphTypes::RectF & r);

	virtual void SetFocus(bool bSet=true);

	virtual void PaintNormVertex(CDCHandle &dc);
	virtual void PaintSelVertex(CDCHandle &dc);
	virtual void PaintOverVertex(CDCHandle &dc);
	virtual void PaintVertex(CDCHandle &dc, GraphTypes::Color &fontColor, GraphTypes::Color &fill, GraphTypes::Color &border);
	virtual GraphTypes::RectF DrawText(CDCHandle& dc, const GraphTypes::RectF& rText);
	virtual GraphTypes::RectF DrawIcon(CDCHandle& dc, const GraphTypes::RectF& rText);
	virtual GraphTypes::RectF DrawImage(CDCHandle& dc, const GraphTypes::RectF& rText);
	virtual GraphTypes::RectF DrawBitmap(CDCHandle& dc, const GraphTypes::RectF& rText, IPicture* image, const HTMLElement* element, const PLACEMENT& placement);

	void OverrideColors(GraphTypes::Color &fontColor, GraphTypes::Color &fill, GraphTypes::Color &border);
//	static void AdjustOffsets(GraphTypes::RectF& rOut, const GraphTypes::RectF& rIn, PLACEMENT placement, bool isIPicture);

	LRESULT SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	GraphTypes::SizeF CalcDisplaySizes(CDCHandle& dc, bool bForce, bool bIgnoreExtent);

	BEGIN_MSG_MAP(CVertexRenderer)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
	END_MSG_MAP()

	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};

typedef StlLinked<CVertexRenderer> CVertexRendererAdapt;
typedef std::vector<CVertexRendererAdapt> CVertexRendererVector;

using namespace GraphTypes;

CVertexRenderer::CVertexRenderer(CGraphFastRenderImpl & parent, IGraphVertex * v) : m_renderParent(parent), m_meta(parent.m_meta), m_graph(parent.m_graph), m_transform(parent), m_v(v)
{
}

CVertexRenderer::~CVertexRenderer()
{
}

CUniqueID CVertexRenderer::GetID()
{
	return m_v->GetID();
}

const TCHAR * CVertexRenderer::GetLabel()
{
	// the first user property is the label
	if (m_meta.GetMetaDebug())
	{
		CXMLWriter writer;
		m_v->GetID().Serialize(writer);
		std::_tstring xml;
		writer.EndDocument(xml);
		m_label = xml;
	}
	else
	{
		std::_tstring str;
		m_label = m_v->GetProperty(0, str);
		if (m_label.length() == 0)
		{
			CXMLWriter writer;
			m_v->GetID().Serialize(writer);
			std::_tstring xml;
			writer.EndDocument(xml);
			m_label = xml;
		}
		/*
		PointF p = m_graph.getVertexPoint(m_v);
		txt += _T("(");
		txt += boost::lexical_cast<std::_tstring>(p.x).c_str();
		txt += _T(", ");
		txt += boost::lexical_cast<std::_tstring>(p.y).c_str();
		txt += _T(")");
		*/
	}
	return m_label.c_str();
}

RECT CVertexRenderer::CalcTextBounds(CDCHandle& dc)
{
	CString txt = GetLabel();
	RECT r = {0, 0, 0, 0};
	CAutoFont f(CUtilFactory::GetListFont(m_transform.GetFontSize() - 1), dc);
	dc.DrawText(txt, txt.GetLength(), &r, DT_CALCRECT|DT_NOPREFIX|DT_CENTER);
	RectF retVal(r);
	//ATLTRACE("'%s', %d, %d, %f, %f, %f, %f\n", static_cast<const TCHAR *>(txt), m_v, m_transform.GetFontSize(), retVal.x, retVal.y, retVal.Width, retVal.Height);
	return r;
}

const RectF CVertexRenderer::CalculateTitleRect(CDCHandle& dc)
{
	PointF p;
	m_v->GetPoint(p);
	RectF retVal = GetTitlePos();
	if (retVal.Width == 0 || retVal.Height == 0)
	{
		retVal.SetLocation(&p);
		m_transform.AllTransform(retVal);
		RECT bounds = CalcTextBounds(dc);
		retVal.Width = static_cast<float>(bounds.right + 4);
		retVal.Height = static_cast<float>(bounds.bottom + 4);
	}
	else
	{
		m_transform.AllTransform(p);
		retVal.SetLocation(&p);
	}
	retVal.x -= retVal.Width / 2.0f;
	retVal.y -= retVal.Height / 2.0f;
	return retVal;
}

const RectF & CVertexRenderer::GetTitlePos()
{
	return m_title_pos[m_transform.GetDpiScale()];
}

void CVertexRenderer::SetTitlePos(const RectF & r)
{
	m_title_pos[m_transform.GetDpiScale()] = r;
}

const RectF & CVertexRenderer::GetIconPos()
{
	return m_icon_pos[m_transform.GetDpiScale()];
}

void CVertexRenderer::SetIconPos(const RectF & r)
{
	m_icon_pos[m_transform.GetDpiScale()] = r;
}

const RectF & CVertexRenderer::GetImagePos()
{
	return m_image_pos[m_transform.GetDpiScale()];
}

void CVertexRenderer::SetImagePos(const RectF & r)
{
	m_image_pos[m_transform.GetDpiScale()] = r;
}

bool CVertexRenderer::IsVisible()
{
	return (m_meta.IsVisible(m_v));
}

bool CVertexRenderer::IsSelected()
{
	return m_v->GetSelected();
}

void CVertexRenderer::GetBoundsBox(RectF& r)
{
	// for base pretend that title is bounding box
	r=m_title_pos[m_transform.GetDpiScale()];
}

void CVertexRenderer::SetBoundingBox(const RectF & r)
{
	// default does nothing
	r;
}

void CVertexRenderer::SetFocus(bool bSet)
{
}

void CVertexRenderer::PaintNormVertex(CDCHandle &dc)
{
	CCategory cat = m_v->GetCategory();
	Color fill(m_meta.GetMetaVertexColorFillNorm(cat));
	Color border(m_meta.GetMetaVertexColorBorderNorm(cat));
	Color fontColor(m_meta.GetMetaVertexFontColor(cat));
	PaintVertex(dc, fontColor, fill, border);
}

void CVertexRenderer::PaintSelVertex(CDCHandle &dc)
{
	CCategory cat = m_v->GetCategory();
	Color fill(m_meta.GetMetaVertexColorFillSel(cat));
	Color border(m_meta.GetMetaVertexColorBorderSel(cat));
	Color fontColor(m_meta.GetMetaVertexFontColor(cat));
	PaintVertex(dc, fontColor, fill, border);
}

void CVertexRenderer::PaintOverVertex(CDCHandle &dc) 
{
	CCategory cat = m_v->GetCategory();
	Color fill(m_meta.GetMetaVertexColorFillOver(cat));
	Color border(m_meta.GetMetaVertexColorBorderOver(cat));
	Color fontColor(m_meta.GetMetaVertexFontColor(cat));
	PaintVertex(dc, fontColor, fill, border);
}

void CVertexRenderer::PaintVertex(CDCHandle &dc, Color &fontColor, Color &fill, Color &border)
{
	const RectF r(CalculateTitleRect(dc));
	OverrideColors(fontColor, fill, border);
	CPen borderPen;
	borderPen.CreatePen(PS_SOLID, 1, border.ToCOLORREF());
	CBrush fillBrush;
	fillBrush.CreateSolidBrush(fill.ToCOLORREF());

	RECT frameRect = static_cast<CRect>(r);
	RECT textRect = {frameRect.left+2, frameRect.top+2, frameRect.right-2, frameRect.bottom-2};

	{
		CAutoPen p(borderPen, dc);
		CAutoBrush b(fillBrush, dc);
		dc.SetBkColor(fill.ToCOLORREF());
		dc.Rectangle(&frameRect);

		if(m_v->GetPinned())
		{
			int pinSize = 2;
			dc.Rectangle(frameRect.left - pinSize, frameRect.top - pinSize, frameRect.left + pinSize, frameRect.top + pinSize);
			dc.Rectangle(frameRect.right - pinSize, frameRect.top - pinSize, frameRect.right + pinSize, frameRect.top + pinSize);
			dc.Rectangle(frameRect.left - pinSize, frameRect.bottom - pinSize, frameRect.left + pinSize, frameRect.bottom + pinSize);
			dc.Rectangle(frameRect.right - pinSize, frameRect.bottom - pinSize, frameRect.right + pinSize, frameRect.bottom + pinSize);
		}

		dc.SetTextColor(fontColor.ToCOLORREF());
		DrawText(dc, textRect);

		//TODO focus node
		if (false/*v == m_focusNode && !m_graph.Selected(v)*/)
		{
			dc.DrawFocusRect(&frameRect);
		}
	}
	SetTitlePos(r);
	SetIconPos(DrawIcon(dc, r));
	if (m_meta.GetVertexImagesVisible())
		SetImagePos(DrawImage(dc, r));
}

RectF CVertexRenderer::DrawText(CDCHandle& dc, const RectF& rText)
{
	CString txt = GetLabel();
	CRectEx textRect(rText);
	CAutoFont f(CUtilFactory::GetListFont(m_transform.GetFontSize() - 1), dc);
	dc.DrawText(txt, txt.GetLength(), &textRect, DT_NOPREFIX);
	return rText;
}

RectF CVertexRenderer::DrawIcon(CDCHandle& dc, const RectF& rText)
{
	IPicture* icon = m_v->GetIcon();
	if (icon == NULL)
		icon = m_meta.GetMetaVertexIcon(m_v->GetCategory());

	return DrawBitmap(dc, rText, icon, m_meta.GetMetaVertexHtmlElement(m_v->GetCategory()), m_meta.GetMetaVertexIconPlacement());
}

RectF CVertexRenderer::DrawImage(CDCHandle& dc, const RectF& rText)
{
	return DrawBitmap(dc, rText, m_v->GetImage(), &m_v->GetHtmlElement(), m_meta.GetMetaVertexImagePlacement());
}

RectF CVertexRenderer::DrawBitmap(CDCHandle& dc, const RectF& rText, IPicture* image, const HTMLElement * element, const PLACEMENT& placement)
{
	if (image == NULL && (element == NULL || element->GetHtmlElement() == NULL))
		return rText;

	LONG width, height;
	RectF rImg; 
	if (image != NULL) 
	{
		image->get_Width( &width );
		image->get_Height( &height );
		CSize szPic(width, height);
		// convert OLE size into pixels
		dc.HIMETRICtoDP( &szPic );
		rImg.Width = static_cast<float>(szPic.cx);
		rImg.Height = static_cast<float>(szPic.cy);
	} 
	else if (element != NULL)
	{
		rImg = element->GetSize();
	}

	HRESULT hr = S_OK;
	//COLORREF transparent = RGB(253,0,200);
	COLORREF transparent = RGB(0,0,211); // C# transparent gif background color
	CMemBm bmp(dc, 0, 0, 0, (int)rImg.Width, (int)rImg.Height);
	CDCHandle hdcMem = bmp;
	hdcMem.FillSolidRect(0, 0, (int)rImg.Width, (int)rImg.Height, transparent);
	if (image != NULL) 
	{
		hr = image->Render(hdcMem, 0, long(rImg.Height), long(rImg.Width), long(-rImg.Height), 0, 0, width, height, NULL );
	} 
	else if (element != NULL && element->GetHtmlElement() != NULL) 
	{
		hr = element->GetHtmlElement()->DrawToDC(hdcMem);
	}
	//TODO over support
	if (/*m_overNode != v ||*/ m_transform.GetScale() > 1.0f) 
	{
		m_transform.ScaleTransform(rImg);
	}
	//AdjustOffsets(rImg, rText, placement, false);
	bmp.DrawTrans(dc, int(rImg.x), int(rImg.y), int(rImg.Width), int(rImg.Height), transparent);
	return rImg;
}

void CVertexRenderer::OverrideColors(Color &fontColor, Color &fill, Color &border)
{
	if (m_v->GetBorderColor().GetValue() != 0x0)
		border.SetValue(m_v->GetBorderColor().GetValue());
	if (m_v->GetFillColor().GetValue() != 0x0)
		fill.SetValue(m_v->GetFillColor().GetValue());
	if (m_v->GetFontColor().GetValue() != 0x0)
		fontColor.SetValue(m_v->GetFontColor().GetValue());
}

/*
void CVertexRenderer::AdjustOffsets(RectF& rOut, const RectF& rIn, PLACEMENT placement, bool isIPicture)
{
	switch (placement)
	{
	case PLACEMENT_LEFT:
		rOut.x = rIn.x - rOut.Width;
		rOut.y = rIn.y;
		if (isIPicture)
			rOut.y += rOut.Height;
		break;
	case PLACEMENT_RIGHT:
		rOut.x = rIn.x + rIn.Width;
		rOut.y = rIn.y;
		if (isIPicture)
			rOut.y += rOut.Height;
		break;
	case PLACEMENT_TOP:
		rOut.x = rIn.x + rIn.Width / 2 - rOut.Width / 2;
		if (isIPicture)
			rOut.y = rIn.y;
		else
			rOut.y = rIn.y - rOut.Height + 1;
		break;
	case PLACEMENT_BOTTOM:
		rOut.x = rIn.x + rIn.Width / 2 - rOut.Width / 2;
		rOut.y = rIn.y + rIn.Height;
		if (isIPicture)
			rOut.y += rOut.Height;
		break;
	}
}
*/

LRESULT CVertexRenderer::SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return CScaledWindow::SendMessage(uMsg, wParam, lParam);
}

SizeF CVertexRenderer::CalcDisplaySizes(CDCHandle& dc, bool bForce, bool bIgnoreExtent)
{
	SizeF size;

	return size;
}

LRESULT CVertexRenderer::OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)); 

	m_capturePos=pt;
	m_v->GetPoint(m_dragStartPos);

	return 0;
}

LRESULT CVertexRenderer::OnMouseMove(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)); 

	// left button down mode
	if (wParam & MK_LBUTTON) 
	{
		GraphTypes::PointF from(REAL(m_capturePos.x), REAL(m_capturePos.y));
		m_transform.RAllTransform(from);
		PointF to(REAL(pt.x), REAL(pt.y));
		m_transform.RAllTransform(to);
		m_v->SetPoint(m_dragStartPos + PointF(to.x - from.x, to.y - from.y), true);
	}

	return 0;
}

//*****************************************************************************

IRenderer *CreateRenderer(CGraphFastRenderImpl & parent, IGraphVertex * v)
{
	CVertexRenderer * retVal = new CVertexRenderer(parent, v);
	return retVal;
}


