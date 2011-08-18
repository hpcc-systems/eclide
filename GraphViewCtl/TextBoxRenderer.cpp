
#include "StdAfx.h"

#include "Canvas.h"
#include "ScaledWindows.h"
#include "AggDC.h"

class CTextboxRenderer : public ITextboxRenderer, public CScaledWindow
{
	IGraphVertex* m_v;
	eIOMode m_ioMode;
	CButtonState m_hotTracking;
	bool m_bSingleLine;

	// sizes that vary with each DPI 
	SizeFMap m_extentMap;
	MARGINS m_textMargins; // currently not scaled with DPI

	// looks
	GraphTypes::Color m_colorSelectedFocusBkgnd;
	GraphTypes::Color m_colorSelectedNoFocusBkgnd;
	GraphTypes::Color m_hotColor;

	void Draw(CAggDC& dcdc);

public:
	BEGIN_CUNKNOWN
		IMPLEMENT_INTERFACE(ITextboxRenderer)
		IMPLEMENT_INTERFACE(IRenderer)
	END_CUNKNOWN(CScaledWindow)

	CTextboxRenderer(CCanvas & canvas);
	~CTextboxRenderer();

	virtual void Invalidate(CRect* rc=0, bool bErase=false) 
	{ 
		m_canvas->Invalidate(rc,bErase);
	}

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

	// ITextboxRenderer

	bool IsHot();
	bool AcceptEdge(IGraphItem* from);
	void SetIOMode(eIOMode iom);
	eIOMode GetIOMode();

protected:
	BEGIN_MSG_MAP(CTextboxRenderer)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_NCHITTEST, OnNcHitTest)
	END_MSG_MAP()

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnNcHitTest(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

};

//*****************************************************************************

using namespace GraphTypes;

CTextboxRenderer::CTextboxRenderer(CCanvas & canvas)
	: CScaledWindow(&canvas),
	m_ioMode(iomBoth),
	m_hotTracking(&canvas, this),
	m_bSingleLine(true)
{
	m_textMargins.cxLeftWidth=2;
	m_textMargins.cxRightWidth=2;
	m_textMargins.cyTopHeight=2;
	m_textMargins.cyBottomHeight=2;

	m_colorSelectedFocusBkgnd.SetFromCOLORREF(GetSysColor(COLOR_HIGHLIGHT));
	m_colorSelectedNoFocusBkgnd.SetFromCOLORREF(GetSysColor(COLOR_3DFACE));
	COLORREF hot=RGB(
		(m_colorSelectedFocusBkgnd.GetR()*60+m_colorSelectedNoFocusBkgnd.GetR()*40)/100,
		(m_colorSelectedFocusBkgnd.GetG()*60+m_colorSelectedNoFocusBkgnd.GetG()*40)/100,
		(m_colorSelectedFocusBkgnd.GetB()*60+m_colorSelectedNoFocusBkgnd.GetB()*40)/100);
	m_hotColor.SetFromCOLORREF(hot);	
}

//-----------------------------------------------------------------------------

CTextboxRenderer::~CTextboxRenderer()
{
	if(m_hotTracking.m_state==BS_HOT)
		m_hotTracking.SetHot(false);
}

//-----------------------------------------------------------------------------

CUniqueID CTextboxRenderer::GetID()
{
	return m_v->GetID();
}

//-----------------------------------------------------------------------------

void CTextboxRenderer::SetGraphItem(IGraphItem* item)
{
	m_v=static_cast<IGraphVertex*>(item);
}

//-----------------------------------------------------------------------------

void CTextboxRenderer::SetFocus(bool bSet)
{
	CScaledWindow::SetFocus(bSet);
}

//-----------------------------------------------------------------------------

void CTextboxRenderer::GetBoundsBox(RectF& box, GraphTypes::Coordinates co)
{
	RectF rc(m_rcWnd);
	box=rc;
	if(co==world)
	{
		m_canvas->RAllTransform(box);
	}
}

//-----------------------------------------------------------------------------

void CTextboxRenderer::SetBoundsBox(const RectF& box, unsigned flags, GraphTypes::Coordinates co)
{
	RectF r = box;
	if(co==world)
	{
		m_canvas->AllTransform(r);
	}
	CRect rc=r;
	SetWindowPos(rc, flags);
}

//-----------------------------------------------------------------------------

void CTextboxRenderer::GetPosition(PointF& pt, Coordinates co/*=world*/)
{
	// nothing to do for now
}

//-----------------------------------------------------------------------------

void CTextboxRenderer::SetPosition(const PointF& pt, Coordinates co/*=world*/)
{
	// nothing to do for now
}

//-----------------------------------------------------------------------------

void CTextboxRenderer::SetSelected(bool bSelected/*=true*/, bool bAdd/*=false*/)
{
	// NOTE : must not be passed to CScaledWindow base because
	// then it would affect selection state of all the parent windows
	m_bSelected=bSelected;
}

//-----------------------------------------------------------------------------

LRESULT CTextboxRenderer::SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return CScaledWindow::SendMessage(uMsg, wParam, lParam);
}

//-----------------------------------------------------------------------------

GraphTypes::SizeF CTextboxRenderer::CalcDisplaySizes(
	CDCHandle& dc, 
	bool bForce,
	bool bIgnoreExtent)
{
	SizeF extentf=m_extentMap[m_canvas->GetDpiScale()];

	if(bForce || extentf.Height==0)
	{
		CSize szExtent;
		GraphTypes::CRectEx rctext;
		std::_tstring label = m_v->GetLabel();

		CAutoFont f(m_canvas->GetListFont(m_canvas->GetFontPixHeight()), dc);
		m_bSingleLine=label.find_first_of(_T("\n\r"))==std::_tstring::npos;

		if(m_bSingleLine)
		{
			dc.GetTextExtent(label.c_str(), label.length(), &szExtent);
		}
		else
		{
			dc.DrawText(
				label.c_str(),
				label.length(),
				&rctext,
				DT_CALCRECT |
				DT_NOPREFIX |
				DT_EDITCONTROL );

			szExtent.cx=rctext.Width();
			szExtent.cy=rctext.Height();
		}

		ATLASSERT(szExtent.cy>0);
		extentf.Width=REAL(szExtent.cx);
		extentf.Height=REAL(szExtent.cy);

		extentf.Width+=m_textMargins.cxLeftWidth+m_textMargins.cxRightWidth;
		extentf.Height+=m_textMargins.cyTopHeight+m_textMargins.cyBottomHeight;

		m_extentMap[m_canvas->GetDpiScale()]=extentf;
		m_rcWnd.right=int(extentf.Width);
		m_rcWnd.bottom=int(extentf.Height);
	}
	
	return extentf;
}

//-----------------------------------------------------------------------------

void CTextboxRenderer::SetRedraw(bool bEnable)
{
	CScaledWindow::SetRedraw(bEnable);
}

//-----------------------------------------------------------------------------

void CTextboxRenderer::ClearCaches()
{
	m_extentMap.clear();
}

//-----------------------------------------------------------------------------

bool CTextboxRenderer::IsHot()
{
	return m_hotTracking.m_state==BS_HOT;
}

//-----------------------------------------------------------------------------

bool CTextboxRenderer::AcceptEdge(IGraphItem* from)
{
	bool bAccept=false;
	if(from)
	{
		// accept from any vertex
		CComQIPtr<IGraphVertex> v=from;
		if(v!=0)
			bAccept=true;
	}

	return bAccept;
}

//-----------------------------------------------------------------------------

void CTextboxRenderer::SetIOMode(eIOMode iom)
{
	m_ioMode=iom;
}

//-----------------------------------------------------------------------------

ITextboxRenderer::eIOMode CTextboxRenderer::GetIOMode()
{
	return m_ioMode;
}

//-----------------------------------------------------------------------------

void CTextboxRenderer::Draw(CAggDC &dc) 
{
	const TCHAR* label = m_v->GetLabel(); 

	CDCHandle hdc(dc.m_hDC);

	if(m_rcWnd.Height()<2)
	{
		hdc.FillSolidRect(&m_rcWnd,::GetSysColor(COLOR_WINDOW));
		return;
	}

	int isave=hdc.SaveDC();

	if(m_bSelected)
	{
		if(HasFocus())
		{
			hdc.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
			if(m_hotTracking.m_state==BS_HOT) 
				hdc.FillSolidRect(&m_rcWnd, m_hotColor.ToCOLORREF());
			else
				hdc.FillSolidRect(&m_rcWnd, m_colorSelectedFocusBkgnd.ToCOLORREF());
		}
		else
		{
			hdc.SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
			if(m_hotTracking.m_state==BS_HOT) 
				hdc.FillSolidRect(&m_rcWnd, m_hotColor.ToCOLORREF());
			else
				hdc.FillSolidRect(&m_rcWnd, m_colorSelectedNoFocusBkgnd.ToCOLORREF());
		}
	}
	else
	{
		hdc.SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
		if(m_hotTracking.m_state==BS_HOT) 
			hdc.FillSolidRect(&m_rcWnd, m_hotColor.ToCOLORREF());
		else
			hdc.FillSolidRect(&m_rcWnd, ::GetSysColor(COLOR_WINDOW));
	}


	CAutoFont f(m_canvas->GetListFont(m_canvas->GetFontPixHeight()), hdc);

	CRect rcText=m_rcWnd;

	rcText.DeflateRect(
		m_textMargins.cxLeftWidth>1?m_textMargins.cxLeftWidth-1:0,
		m_textMargins.cyTopHeight>1?m_textMargins.cyTopHeight-1:0,
		m_textMargins.cxRightWidth,
		m_textMargins.cyBottomHeight);

	hdc.SetBkMode(TRANSPARENT);

	UINT flags=DT_NOPREFIX|DT_CENTER;
	flags|=m_bSingleLine?(DT_SINGLELINE|DT_VCENTER):DT_EDITCONTROL;

	hdc.DrawText(
		label,
		-1,
		&rcText,
		flags);

	hdc.RestoreDC(isave);
}

//-----------------------------------------------------------------------------

LRESULT CTextboxRenderer::OnPaint(
	UINT /*uMsg*/,
	WPARAM wParam,
	LPARAM /*lParam*/,
	BOOL& /*bHandled*/)
{
	CAggDC& dc= *(CAggDC*)wParam;
	if(dc.RectVisible(&m_rcWnd))
		Draw(dc);	

	return 0;
}

//-----------------------------------------------------------------------------

LRESULT CTextboxRenderer::OnLButtonDown(
	UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	bool selectable=true;
	CComQIPtr<IGraphSubgraph> isg=m_v->GetParent();
	if(isg)
	{
		CComQIPtr<IIOSubgraphRenderer> ior=m_canvas->GetRenderer(isg);
		if(ior)
			selectable=false;	
	}

	if(selectable)
	{
		SetFocus();
		SetSelected();
	}

	// nobody can be hot now
	m_canvas->ClearCurrentHot();
	return reinterpret_cast<LRESULT>(m_v);
}

//-----------------------------------------------------------------------------

LRESULT CTextboxRenderer::OnLButtonUp(
	UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_hotTracking.SetHot(true);
	return 0;
}

//-----------------------------------------------------------------------------

LRESULT CTextboxRenderer::OnMouseMove(
	UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_hotTracking.SetHot(true);
	return 0;
}

//-----------------------------------------------------------------------------

LRESULT CTextboxRenderer::OnNcHitTest(
	UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return reinterpret_cast<LRESULT>(m_v);
}

//*****************************************************************************

IRenderer* CreateTextboxRenderer(CCanvas & parent)
{
	CTextboxRenderer * retVal = new CTextboxRenderer(parent);
	return retVal;
}
