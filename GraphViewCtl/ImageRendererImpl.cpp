
#include "StdAfx.h"

#include "Canvas.h"
#include "ScaledWindows.h"
#include "XMLWriter.h"
#include "AggDC.h"
#include "IPictureHelper.h"

//*****************************************************************************

class CImageRenderer : public IImageRenderer, public CScaledWindow
{
	IGraphItem * m_gi; 
	GraphTypes::PointF m_position;
	IRenderer* m_parentRenderer; // must not be CComPtr to avoid circular refs
	CButtonState m_hotTracking;

	// cached hit-test data
	IMAGE_POSITION m_pos, m_hotPos;
	int	m_overlayIdx, m_hotOverlayIdx;
	bool m_sharedhit; // if hit-test on shared or item-specific image

	unsigned m_hitCode;
	// original and resized selected image rect in screen coordinates
	CRect m_rcSelImg, m_rcSelImgResized; 
	bool m_keepAspectRatio;

	CPoint m_capturePos;

	static int m_handleSize;

	void Draw(CAggDC& dc);

	GraphTypes::RectF DrawBitmap(
		CAggDC& dc,
		const ImageInfo& iinfo,
		const GraphTypes::RectF& refrc, // reference rect in screen coordinates
		bool scale,
		int idx,
		bool sizinghandles=false);

	void CalcImagesBoundsBox(GraphTypes::RectF& box);

	void AdjustOffsets(
		GraphTypes::RectF& rOut,
		const GraphTypes::RectF& rIn,
		bool scale,
		IMAGE_POSITION pos,
		int marginX,
		int marginY) const;

	unsigned Position2Code(IMAGE_POSITION pos) const;

	void DrawSizingHandles(
		CAggDC& dc, 
		const CRect& rcimg, 
		IMAGE_POSITION pos);

	unsigned HitTestHandles(
		const CPoint& pt,
		const CRect& rcimg, 
		IMAGE_POSITION pos);

	CRect CalcMinImageRect();

public:
	BEGIN_CUNKNOWN
		IMPLEMENT_INTERFACE(IImageRenderer)
		IMPLEMENT_INTERFACE(IRenderer)
	END_CUNKNOWN(CScaledWindow)

	CImageRenderer(
		CCanvas & canvas,
		IGraphItem* item);

	virtual ~CImageRenderer();

	virtual void Invalidate(CRect* rc=0, bool bErase=false); 

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

	// IImageRenderer
	LRESULT HitTestDispatchMsg(const GraphTypes::PointF& point, bool& hit,
		 UINT uMsg, WPARAM wParam, LPARAM lParam);
	bool IsSelected() const;
	bool HasCapture() const;
	void SetParentRenderer(IRenderer* parent);

protected:
	BEGIN_MSG_MAP(CImageRenderer)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnMouseDblclk)
		MESSAGE_HANDLER(WM_NCHITTEST, OnNcHitTest)
		MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
	END_MSG_MAP()

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnMouseDblclk(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnNcHitTest(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSetCursor(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};

int CImageRenderer::m_handleSize=4;

//*****************************************************************************

using namespace GraphTypes;

CImageRenderer::CImageRenderer(
	CCanvas & canvas, IGraphItem* item)
	: CScaledWindow(&canvas),
	m_hotTracking(&canvas, this),
	m_gi(item),
	m_parentRenderer(0)
{
}

CImageRenderer::~CImageRenderer()
{
	if(m_hotTracking.IsHot())
		m_hotTracking.SetHot(false);

	if(m_canvas->m_selectedImage.m_image==this)
		m_canvas->m_selectedImage.Clear();
}

//-----------------------------------------------------------------------------

CUniqueID CImageRenderer::GetID()
{
	return m_gi->GetID();
}

//-----------------------------------------------------------------------------

void CImageRenderer::SetGraphItem(IGraphItem* item)
{
	m_gi=item;
}

//-----------------------------------------------------------------------------

void CImageRenderer::SetFocus(bool bSet)
{
	// CScaledWindow::SetFocus(bSet);
}

//-----------------------------------------------------------------------------

void CImageRenderer::GetBoundsBox(RectF& box, GraphTypes::Coordinates co)
{
	CalcImagesBoundsBox(box);
	if(co==world)
	{
		m_canvas->RAllTransform(box);
	}
}

//-----------------------------------------------------------------------------

void CImageRenderer::SetBoundsBox(const RectF& box, unsigned flags, GraphTypes::Coordinates co)
{
	// TODO Discover use for this
	assert(0);

	/*
	CRect rc=static_cast<CRect>(box);
	SetWindowPos(rc, flags);
	PointF ptnew(rc.TopLeft());
	m_canvas->RAllTransform(ptnew);
	m_position=ptnew;
	*/
}

//-----------------------------------------------------------------------------

void CImageRenderer::GetPosition(PointF& pt, Coordinates co/*=world*/)
{
//	TODO Discover use for this

	pt=m_position;

	if(co==screen)
	{
		m_canvas->AllTransform(pt);
	}
}

//-----------------------------------------------------------------------------

void CImageRenderer::SetPosition(const PointF& pt, Coordinates co/*=world*/)
{
	PointF ptnew=pt;
	if(co==screen)
	{
		m_canvas->RAllTransform(ptnew);		
	}

	m_position=ptnew;
	RectF rcnew(ptnew, m_rcWnd.Size());
	m_canvas->AllTransform(rcnew);
	SetWindowPos(static_cast<CRect>(rcnew), SWP_NOSIZE);
}

//-----------------------------------------------------------------------------
// Image selection is orthogonal to selecting vertices. 
// Selected image displays sizing handles

void CImageRenderer::SetSelected(bool bSelected/*=true*/, bool bAdd/*=false*/)
{
	if(m_bSelected!=bSelected)
	{
		m_bSelected=bSelected;
		Invalidate();
	}
}

//-----------------------------------------------------------------------------

LRESULT CImageRenderer::SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return CScaledWindow::SendMessage(uMsg, wParam, lParam);
}

//-----------------------------------------------------------------------------

GraphTypes::SizeF CImageRenderer::CalcDisplaySizes(
	CDCHandle& dc, 
	bool bForce,
	bool bIgnoreExtent)
{
	// TODO Discover use for this

	SizeF extentf;

	return extentf;
}

//-----------------------------------------------------------------------------

void CImageRenderer::SetRedraw(bool bEnable)
{
	CScaledWindow::SetRedraw(bEnable);
}

//-----------------------------------------------------------------------------

void CImageRenderer::ClearCaches()
{

}

//-----------------------------------------------------------------------------

LRESULT CImageRenderer::HitTestDispatchMsg(const GraphTypes::PointF& point,
	bool& hit, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	hit=false;
	LRESULT lRes=0;

	//TODO all this stuff requires generalization of image methods under
	// graph item and maybe some base renderer

	// for now this method is used only for images associated with legacy vertices
	CComQIPtr<ILegacyRenderer> ilr;
	CComQIPtr<IGraphVertex> v=m_gi;
	CComQIPtr<ILegacySubgraphRenderer> ilsgr;
	CComQIPtr<IGraphSubgraph> sg=m_gi;

	if(v!=0)
	{
		ilr=m_parentRenderer;
		if(ilr==0)
			return lRes;
	}
	else if(sg!=0)
	{
		ilsgr=m_parentRenderer;
		if(ilsgr==0)
			return lRes;
	}
	else
	{
		return lRes;
	}

	// funny 8.0 compiler won't return rbegin for const map pointer and funky
	// 7.0 compiler doesn't like const_reverse_iterator so we cast away constness
	ImageInfoMaps* imgmapsshared;
	ImageInfoMaps* imgmaps;

	if(v!=0)
	{
		imgmapsshared=const_cast<ImageInfoMaps*>(m_canvas->m_meta.GetVertexImageInfo(v->GetCategory()));
		imgmaps=const_cast<ImageInfoMaps*>(v->GetImages());
	}
	else
	{
		imgmapsshared=const_cast<ImageInfoMaps*>(m_canvas->m_meta.GetSubgraphImageInfo(sg->GetCategory()));
		imgmaps=const_cast<ImageInfoMaps*>(sg->GetImages());
	}

	RectF rcimg;
	RectF rctext;
	SizeF sizeimg;

	if(v!=0)
		ilr->GetTitlePos(rctext);
	else
		ilsgr->GetTitlePos(rctext);

	m_sharedhit=false;

	// Hit-test item-specific images first.
	// Hit-test in reverse to direction in which we draw images
	// so that, for overapping images, the image in front gets tested first.

	for(ImageInfoMaps::reverse_iterator itrnodemaps=imgmaps->rbegin();
		itrnodemaps != imgmaps->rend();
		++itrnodemaps)
	{
		if(m_canvas->m_meta.IsImageVisible(m_gi, static_cast<IMAGE_POSITION>(itrnodemaps->first)))
		{
			for(MapImageInfo::reverse_iterator itrnodeimages=itrnodemaps->second.rbegin();
				itrnodeimages!=itrnodemaps->second.rend();
				++itrnodeimages)
			{
				const ImageInfo& img=itrnodeimages->second;
				if (img.image==0)
					break;
				CSize imgsize;
				img.GetSize(imgsize);
				sizeimg.Width= static_cast<GraphTypes::REAL>(imgsize.cx);
				sizeimg.Height= static_cast<GraphTypes::REAL>(imgsize.cy);
				m_canvas->SizeTransform(sizeimg);

				rcimg.x=rcimg.y=0;
				rcimg.Width=sizeimg.Width;
				rcimg.Height=sizeimg.Height;

				AdjustOffsets(
					rcimg,	// in screen coordinates (initially contains only Width&Height)
					rctext, // in screen coordinates
					true,
					img.pos,
					img.marginX,
					img.marginY);

				if(IsSelected() && // resizing handles need additional hit-test
					m_canvas->m_selectedImage.m_pos==IMAGE_POSITION(itrnodemaps->first) &&
					m_canvas->m_selectedImage.m_overlayIdx==int(itrnodeimages->first))
				{
					CPoint pt=point;
					CRect imgrc=rcimg;
					m_hitCode=HitTestHandles(pt, imgrc, m_canvas->m_selectedImage.m_pos);
					if(m_hitCode!=0)
					{
						m_pos=m_canvas->m_selectedImage.m_pos;
						m_overlayIdx=m_canvas->m_selectedImage.m_overlayIdx;
						hit=true;
						break;
					}
				}
				else
				{
					m_hitCode=0;
				}

				if(m_hitCode==0 && rcimg.Contains(point))
				{
					m_pos=IMAGE_POSITION(itrnodemaps->first);
					m_overlayIdx=itrnodeimages->first;
					//ATLTRACE(_T("Hittest pos: %d, idx %d\r\n"), m_pos, m_overlayIdx); 
					hit=true;
					break;
				}
			}
		}

		if(hit)
			break;
	}

	// if no hit look at shared images but skip locationst occupied by item-specific images
	if(!hit && imgmapsshared!=0)
	{
		for(ImageInfoMaps::reverse_iterator itrnodemaps=imgmapsshared->rbegin();
			itrnodemaps != imgmapsshared->rend();
			++itrnodemaps)
		{
			if(m_canvas->m_meta.IsImageVisible(m_gi, static_cast<IMAGE_POSITION>(itrnodemaps->first)))
			{
				for(MapImageInfo::reverse_iterator itrnodeimages=itrnodemaps->second.rbegin();
					itrnodeimages!=itrnodemaps->second.rend();
					++itrnodeimages)
				{

					/// if there is corresponding item-specific image then ignore this shared one
					ImageInfoMaps::const_iterator itrnodespecmap=imgmaps->find(itrnodemaps->first);
					if(itrnodespecmap!=imgmaps->end())
					{
						MapImageInfo::const_iterator itrnodespecimage=itrnodespecmap->second.find(itrnodeimages->first);
						if(itrnodespecimage!=itrnodespecmap->second.end())
							continue;
					}

					const ImageInfo& img=itrnodeimages->second;
					if (img.image==0)
						break;

					CSize imgsize;
					img.GetSize(imgsize);
					sizeimg.Width= static_cast<GraphTypes::REAL>(imgsize.cx);
					sizeimg.Height= static_cast<GraphTypes::REAL>(imgsize.cy);
					m_canvas->SizeTransform(sizeimg);

					rcimg.x=rcimg.y=0;
					rcimg.Width=sizeimg.Width;
					rcimg.Height=sizeimg.Height;

					AdjustOffsets(
						rcimg,	// in screen coordinates (initially contains only Width&Height)
						rctext, // in screen coordinates
						true,
						img.pos,
						img.marginX,
						img.marginY);

					if(rcimg.Contains(point))
					{
						m_pos=IMAGE_POSITION(itrnodemaps->first);
						m_overlayIdx=itrnodeimages->first;
						m_sharedhit=true;
						hit=true;
						break;
					}
				}
			}

			if(hit)
				break;
		}
	}

	if(hit && uMsg!=WM_USER)
		lRes=SendMessage(uMsg, wParam, lParam);

	return lRes;
}

//-----------------------------------------------------------------------------

bool CImageRenderer::IsSelected() const
{
	return CScaledWindow::IsSelected();
}

//-----------------------------------------------------------------------------

bool CImageRenderer::HasCapture() const
{
	// this cast is needed to avoid strange bug in VS2003 where this pointer
	// used for comparison is incorrect but for const function only
	return (m_canvas->GetCapture()==static_cast<const CScaledWindow*>(this));
}

//-----------------------------------------------------------------------------

void CImageRenderer::SetParentRenderer(IRenderer* parent)
{
	m_parentRenderer=parent;
}

//-----------------------------------------------------------------------------


void CImageRenderer::Invalidate(CRect* rc/*=0*/, bool bErase/*=false*/) 
{
	m_canvas->Invalidate();
}

//-----------------------------------------------------------------------------

void CImageRenderer::Draw(CAggDC &dc) 
{
	CComQIPtr<IGraphVertex> v;
	CComQIPtr<IGraphEdge> e;
	CComQIPtr<IGraphSubgraph> sg;

	v=m_gi;
	e=m_gi;
	sg=m_gi;

	RectF rcf;

	CComQIPtr<ILegacyRenderer> lv;
	CComQIPtr<ILegacySubgraphRenderer> lsg;

	if(v)
	{
 		// vertices can have several renderers in timeline mode so we cant't use
		// lv=m_canvas->GetRenderer(v);
		lv=m_parentRenderer;
		if(lv) // for now only legacy vertices draw images
			lv->GetBoundsBox(rcf);
		CRect rc =rcf;
		if(rc.IsRectEmpty())
			return;
	}
	else if(e)
	{
		CComQIPtr<IEdgeRenderer> ir;

		ir=m_canvas->GetRenderer(e);
		assert(ir);
		ir->GetLabelPos(rcf);
		if(rcf.IsEmptyArea() || !m_canvas->m_meta.IsEdgeLabelVisible())
		{
			ir->GetBoundsBox(rcf);
			// empty rectangle with top/left point int bounding box center
			// means that image should be centered around that point 
			rcf.x+=rcf.Width/2;
			rcf.y+=rcf.Height/2;
			rcf.Height=rcf.Width=0;
		}
	}
	else if(sg)
	{
		lsg=m_canvas->GetRenderer(sg);
		if(lsg) 
			lsg->GetBoundsBox(rcf);
		CRect rc =rcf;
		if(rc.IsRectEmpty())
			return;
	}
	else
	{
		assert(0);
	}

	const ImageInfoMaps* imgmapsshared=0;
	const ImageInfoMaps* imgmaps=0;

	if(v)
	{
		imgmapsshared=m_canvas->m_meta.GetVertexImageInfo(v->GetCategory());
		imgmaps=v->GetImages();
	}
	else if(e)
	{
		imgmapsshared=m_canvas->m_meta.GetEdgeImageInfo(e->GetCategory());
		imgmaps=e->GetImages();
	}
	else if(sg)
	{
		imgmapsshared=m_canvas->m_meta.GetSubgraphImageInfo(sg->GetCategory());
		imgmaps=sg->GetImages();
	}

	RectF rcimg;
	SizeF sizeimg;
	bool sharedhot=false;

	if(imgmapsshared)
	{
		for(ImageInfoMaps::const_iterator itrnodemaps=imgmapsshared->begin();
			itrnodemaps != imgmapsshared->end();
			++itrnodemaps)
		{
			if(m_canvas->m_meta.IsImageVisible(m_gi, static_cast<IMAGE_POSITION>(itrnodemaps->first)))
			{
				for(MapImageInfo::const_iterator itrnodeimages=itrnodemaps->second.begin();
					itrnodeimages!=itrnodemaps->second.end();
					++itrnodeimages)
				{
					/// if there is corresponding item-specific image then ignore this shared one
					if(imgmaps)
					{
						ImageInfoMaps::const_iterator itrnodespecmap=imgmaps->find(itrnodemaps->first);
						if(itrnodespecmap!=imgmaps->end())
						{
							MapImageInfo::const_iterator itrnodespecimage=itrnodespecmap->second.find(itrnodeimages->first);
							if(itrnodespecimage!=itrnodespecmap->second.end())
								continue;
						}
					}

					// hot image always gets drawn last to be at the top
					if(m_hotTracking.IsHot())
					{
						if(m_hotPos==IMAGE_POSITION(itrnodemaps->first) &&
							m_hotOverlayIdx==int(itrnodeimages->first))
						{
							sharedhot=true;
							continue;
						}
					}

					const ImageInfo& img=itrnodeimages->second;

					if(img.image!=0 && img.image->IsValid())
						DrawBitmap(
							dc,
							img,
							rcf,
							true,
							itrnodeimages->first);
				}
			}
		}
	}

	if(sharedhot)
	{
		const ImageInfo& img=
			(*((*imgmapsshared->find(m_hotPos)).second).find(m_hotOverlayIdx)).second;
		if(img.image!=0 && img.image->IsValid())
				DrawBitmap(
				dc,
				img,
				rcf,
				false,
				m_hotOverlayIdx);
	}

	for(ImageInfoMaps::const_iterator itrnodemaps=imgmaps->begin();
		itrnodemaps != imgmaps->end();
		++itrnodemaps)
	{
		if(m_canvas->m_meta.IsImageVisible(m_gi, static_cast<IMAGE_POSITION>(itrnodemaps->first)))
		{
			for(MapImageInfo::const_iterator itrnodeimages=itrnodemaps->second.begin();
				itrnodeimages!=itrnodemaps->second.end();
				++itrnodeimages)
			{
				// skip drawing selected or hot image in this loop
				if(IsSelected())
				{
					if(m_canvas->m_selectedImage.m_pos==IMAGE_POSITION(itrnodemaps->first) &&
						m_canvas->m_selectedImage.m_overlayIdx==int(itrnodeimages->first))
						continue;
				}
				else if(m_hotTracking.IsHot())
				{
					if(m_hotPos==IMAGE_POSITION(itrnodemaps->first) &&
						m_hotOverlayIdx==int(itrnodeimages->first))
						continue;
				}

				const ImageInfo& img=itrnodeimages->second;

				if(img.image!=0 && img.image->IsValid())
					DrawBitmap(
						dc,
						img,
						rcf,
						true,
						itrnodeimages->first);
			}
		}
	}

	// draw selected image last so it appears topmost
	if(IsSelected())
	{
		// [] operator not available for constant pointer to the map
		const ImageInfo& img=
			(*((*imgmaps->find(m_canvas->m_selectedImage.m_pos)).second).find(m_canvas->m_selectedImage.m_overlayIdx)).second;
		if(img.image!=0 && img.image->IsValid())
				DrawBitmap(
				dc,
				img,
				rcf,
				true,
				m_canvas->m_selectedImage.m_overlayIdx,
				true);
	}
	else if (m_hotTracking.IsHot() && !sharedhot)
	{
		const ImageInfo& img=
			(*((*imgmaps->find(m_hotPos)).second).find(m_hotOverlayIdx)).second;
		if(img.image!=0 && img.image->IsValid())
				DrawBitmap(
				dc,
				img,
				rcf,
				false,
				m_hotOverlayIdx);
	}
}

//-----------------------------------------------------------------------------

RectF CImageRenderer::DrawBitmap(
	CAggDC& dc,
	const ImageInfo& iinfo,
	const RectF& refrc, // reference rect in screen coordinates
	bool scale,
	int idx,
	bool sizinghandles/*=false*/)
{
	CSize imgsize;
	iinfo.GetSize(imgsize);
	RectF rImg(0.0f, 0.0f, static_cast<GraphTypes::REAL>(imgsize.cx), static_cast<GraphTypes::REAL>(imgsize.cy));
	// true image size in pixels, not accounting for image resizing
	iinfo.GetSourceSize(imgsize); 

	if(scale)
	{
		SizeF scaled;
		rImg.GetSize(&scaled);
		m_canvas->SizeTransform(scaled);
		rImg.Width=scaled.Width;
		rImg.Height=scaled.Height;
	}

	AdjustOffsets(rImg, refrc, scale, iinfo.pos, iinfo.marginX, iinfo.marginY);
	CRect rcScaledImg=rImg;

	if(rcScaledImg.IsRectEmpty())
		return rImg;

	boost::scoped_ptr<CBitmap> cbmp(iinfo.image->GetCBitmap());
	if(cbmp->IsNull())
		return rImg;

	bool bBlitOk;

	if(iinfo.transcolor!=0)
	{
		bBlitOk=dc.DrawTransparent(
			(HBITMAP)(*cbmp),
			rcScaledImg.left,
			rcScaledImg.top, 
			rcScaledImg.Width(),
			rcScaledImg.Height(),
			imgsize.cx,
			imgsize.cy,
			iinfo.transcolor);
	}
	else
	{
		int iOldStretch=dc.SetStretchBltMode(COLORONCOLOR);

		bBlitOk=dc.StretchDIBits(
				rcScaledImg.left,
				rcScaledImg.top,
				rcScaledImg.Width(),
				rcScaledImg.Height(),
				0,
				0,
				imgsize.cx,
				imgsize.cy,
				iinfo.image->GetBitsPtr(),
				&iinfo.image->GetBitmapInfo(),
				DIB_RGB_COLORS,
				SRCCOPY) != GDI_ERROR;

		dc.SetStretchBltMode(iOldStretch);
	}

	ATLASSERT(bBlitOk);

	if(sizinghandles)
	{
		DrawSizingHandles(dc, rcScaledImg, iinfo.pos);
		// cache image rectangle but not while resizing it 
		if(m_canvas->GetCapture()!=this)
		{
			m_rcSelImg=rcScaledImg;
			m_rcSelImgResized=m_rcSelImg;
		}
	}

	return rImg;
}

//-----------------------------------------------------------------------------

void CImageRenderer::CalcImagesBoundsBox(RectF& box)
{
	// this method is implemented only for images associated
	// with legacy vertices and legacy subgraphs
	CComQIPtr<ILegacyRenderer> ilr;
	CComQIPtr<IGraphVertex> v=m_gi;
	CComQIPtr<ILegacySubgraphRenderer> ilsgr;
	CComQIPtr<IGraphSubgraph> sg=m_gi;

	if(v!=0)
	{
		// can't use ilr=m_canvas->GetRenderer(v) on timeline mode
		ilr=m_parentRenderer;
		if(ilr==0)
		{
			assert(0);
			return;
		}
	}
	else if(sg!=0)
	{
		ilsgr=m_canvas->GetRenderer(sg);
		if(ilsgr==0)
		{
			assert(0);
			return;
		}
	}
	else
	{
		assert(0); 
		return;
	}

	const CCategory& cat=v!=0 ? v->GetCategory() : sg->GetCategory();
	ImageInfoMaps maps;
	const ImageInfoMaps* imgmaps=m_canvas->m_meta.GetVertexImageInfo(cat);

	if(imgmaps!=0)
		maps=*imgmaps;

	if(v!=0)
		imgmaps=v->GetImages();
	else
		imgmaps=sg->GetImages();

	for(ImageInfoMaps::const_iterator itrnodemaps=imgmaps->begin();
		itrnodemaps != imgmaps->end();
		++itrnodemaps)
	{
		for(MapImageInfo::const_iterator itrnodeimages=itrnodemaps->second.begin();
			itrnodeimages!=itrnodemaps->second.end();
			++itrnodeimages)
		{
			maps[itrnodemaps->first][itrnodeimages->first] = itrnodeimages->second;
		}
	}

	RectF rctext;
	if(v!=0)
		ilr->GetTitlePos(rctext);
	else
		ilsgr->GetTitlePos(rctext);

	RectF rcunion; 
	bool first=true;

	for(ImageInfoMaps::const_iterator itrnodemaps=maps.begin();
		itrnodemaps != maps.end();
		++itrnodemaps)
	{
		if(m_canvas->m_meta.IsImageVisible(m_gi, static_cast<IMAGE_POSITION>(itrnodemaps->first)))
		{
			for(MapImageInfo::const_iterator itrnodeimages=itrnodemaps->second.begin();
				itrnodeimages!=itrnodemaps->second.end();
				++itrnodeimages)
			{
				const ImageInfo& img=itrnodeimages->second;
				if (img.image==0)
					break;

				CSize imgsize;
				img.GetSize(imgsize);
				SizeF sizeimg=imgsize;
				m_canvas->SizeTransform(sizeimg);
				RectF rcimg(0, 0, sizeimg.Width, sizeimg.Height);

				AdjustOffsets(
					rcimg,	// in screen coordinates (initially contains only Width&Height)
					rctext, // in screen coordinates
					true,
					img.pos,
					img.marginX,
					img.marginY);

				if(first)
				{
					rcunion=rcimg;
					first=false;
				}
				else
				{
					rcunion.Union(rcunion, rcimg, rcunion);
				}
			}
		}
	}

	box=rcunion;
}

//-----------------------------------------------------------------------------

void CImageRenderer::AdjustOffsets(
	RectF& rOut, // in screen coordinates (initially contains only Width&Height)
	const RectF& rIn, // in screen coordinates
	bool scale,
	IMAGE_POSITION pos,
	int marginX,
	int marginY) const
{
	if(scale)
	{
		SizeF scaledMargin;
		scaledMargin.Width= static_cast<GraphTypes::REAL>(marginX);
		scaledMargin.Height= static_cast<GraphTypes::REAL>(marginY);

		m_canvas->SizeTransform(scaledMargin);

		marginX=round_int(scaledMargin.Width);
		marginY=round_int(scaledMargin.Height);
	}

	// empty reference rectangle is taken to mean center around that point
	if(rIn.Width==0 || rIn.Height==0)
		pos=POSITION_CENTER;

	switch (pos)
	{
	case POSITION_NORTH_WEST:
		rOut.x = rIn.x - rOut.Width - marginX;
		rOut.y = rIn.y - rOut.Height - marginY;
		break;
	case POSITION_NORTH:
		rOut.x = rIn.x + (rIn.Width - rOut.Width)/2;
		rOut.y = rIn.y - rOut.Height - marginY;
		break;
	case POSITION_NORTH_EAST:
		rOut.x = rIn.x + rIn.Width + marginX;
		rOut.y = rIn.y - rOut.Height - marginY;
		break;
	case POSITION_WEST:
		rOut.x = rIn.x - rOut.Width - marginX;
		rOut.y = rIn.y + (rIn.Height - rOut.Height)/2;
		break;
	case POSITION_CENTER:
		rOut.x = rIn.x + (rIn.Width - rOut.Width)/2 - marginX;
		rOut.y = rIn.y + (rIn.Height - rOut.Height)/2 - marginY;
		break;
	case POSITION_EAST:
		rOut.x = rIn.x + rIn.Width + marginX;
		rOut.y = rIn.y + (rIn.Height - rOut.Height)/2;
		break;
	case POSITION_SOUTH_WEST:
		rOut.x = rIn.x - rOut.Width - marginX;
		rOut.y = rIn.y + rIn.Height + marginY;
		break;
	case POSITION_SOUTH:
		rOut.x = rIn.x + (rIn.Width - rOut.Width)/2;
		rOut.y = rIn.y + rIn.Height + marginY;
		break;
	case POSITION_SOUTH_EAST:
		rOut.x = rIn.x + rIn.Width + marginX;
		rOut.y = rIn.y + rIn.Height + marginY;
		break;
	default:
		assert(FALSE); // should never happen
		break;
	}
}

//-----------------------------------------------------------------------------

unsigned CImageRenderer::Position2Code(IMAGE_POSITION pos) const
{

	//-------------------------------------
	//
	//		bitfield codes
	//	
	//		1.......2.......3
	//		4...............5
	//		6.......7.......8
	//
	//-------------------------------------

	unsigned code=0;

	switch (pos)
	{
	case POSITION_NORTH_WEST:
		code=0x00001011;
		break;
	case POSITION_WEST:
		code=0x01101011;
		break;
	case POSITION_SOUTH_WEST:
		code=0x01101000;
		break;

	case POSITION_NORTH:
		code=0x00011111;
		break;

	case POSITION_NORTH_EAST:
		code=0x00010110;
		break;
	case POSITION_EAST:
		code=0x11010110;
		break;
	case POSITION_SOUTH_EAST:
		code=0x11010000;
		break;

	case POSITION_CENTER:
		code=0x11111111;
		break;

	case POSITION_SOUTH:
		code=0x11111000;
		break;
	default:
		assert(FALSE); // should never happen
		break;
	}

	return code;
}

//-----------------------------------------------------------------------------

void CImageRenderer::DrawSizingHandles(
	CAggDC& dc, 
	const CRect& rcimg, // image rect in screen coordinates
	IMAGE_POSITION pos)
{
	CDCHandle hdc(dc.m_hDC);
	CPen borderPen;
	borderPen.CreatePen(PS_SOLID, 1, GetSysColor(COLOR_WINDOWTEXT));
	CBrush fillBrush;
	fillBrush.CreateSolidBrush(GetSysColor(COLOR_WINDOW));

	CAutoPen p(borderPen, hdc);
	CAutoBrush b(fillBrush, hdc);

	unsigned code=0;
	ATLASSERT(!rcimg.IsRectEmpty());
	if(!rcimg.IsRectEmpty())
	{
		dc.DrawFocusRect(&rcimg);
		code=Position2Code(pos);
	}

	if(code&0x00000001)
		dc.Rectangle(
			rcimg.left - m_handleSize,
			rcimg.top - m_handleSize,
			rcimg.left + m_handleSize,
			rcimg.top + m_handleSize);

	if(code&0x00000010)
		dc.Rectangle(
			(rcimg.right + rcimg.left)/2 - m_handleSize,
			rcimg.top - m_handleSize,
			(rcimg.right + rcimg.left)/2 + m_handleSize,
			rcimg.top + m_handleSize);

	if(code&0x00000100)
		dc.Rectangle(
			rcimg.right - m_handleSize,
			rcimg.top - m_handleSize,
			rcimg.right + m_handleSize,
			rcimg.top + m_handleSize);

	if(code&0x00001000)
		dc.Rectangle(
			rcimg.left - m_handleSize,
			(rcimg.bottom + rcimg.top)/2 - m_handleSize,
			rcimg.left + m_handleSize,
			(rcimg.bottom + rcimg.top)/2 + m_handleSize);

	if(code&0x00010000)
		dc.Rectangle(
			rcimg.right - m_handleSize,
			(rcimg.bottom + rcimg.top)/2 - m_handleSize,
			rcimg.right + m_handleSize,
			(rcimg.bottom + rcimg.top)/2 + m_handleSize);

	if(code&0x00100000)
		dc.Rectangle(
			rcimg.left - m_handleSize,
			rcimg.bottom - m_handleSize,
			rcimg.left + m_handleSize,
			rcimg.bottom + m_handleSize);

	if(code&0x01000000)
		dc.Rectangle(
			(rcimg.right + rcimg.left)/2 - m_handleSize,
			rcimg.bottom - m_handleSize,
			(rcimg.right + rcimg.left)/2 + m_handleSize,
			rcimg.bottom + m_handleSize);

	if(code&0x10000000)
		dc.Rectangle(
			rcimg.right - m_handleSize,
			rcimg.bottom - m_handleSize,
			rcimg.right + m_handleSize,
			rcimg.bottom + m_handleSize);

}

//-----------------------------------------------------------------------------

unsigned CImageRenderer::HitTestHandles(
	const CPoint& pt,
	const CRect& rcimg, // image rect in screen coordinates
	IMAGE_POSITION pos)
{
	if(rcimg.IsRectEmpty())
		return 0;

	unsigned code=Position2Code(pos);
	CRect rc;

	if(code&0x00000001)
	{
		rc.SetRect(
			rcimg.left - m_handleSize,
			rcimg.top - m_handleSize,
			rcimg.left + m_handleSize,
			rcimg.top + m_handleSize);
		if(rc.PtInRect(pt))
			return 0x00000001;
	}

	if(code&0x00000010)
	{
		rc.SetRect(
			(rcimg.right + rcimg.left)/2 - m_handleSize,
			rcimg.top - m_handleSize,
			(rcimg.right + rcimg.left)/2 + m_handleSize,
			rcimg.top + m_handleSize);
		if(rc.PtInRect(pt))
			return 0x00000010;
	}

	if(code&0x00000100)
	{
		rc.SetRect(
			rcimg.right - m_handleSize,
			rcimg.top - m_handleSize,
			rcimg.right + m_handleSize,
			rcimg.top + m_handleSize);
		if(rc.PtInRect(pt))
			return 0x00000100;
	}

	if(code&0x00001000)
	{
		rc.SetRect(
			rcimg.left - m_handleSize,
			(rcimg.bottom + rcimg.top)/2 - m_handleSize,
			rcimg.left + m_handleSize,
			(rcimg.bottom + rcimg.top)/2 + m_handleSize);
		if(rc.PtInRect(pt))
			return 0x00001000;
	}

	if(code&0x00010000)
	{
		rc.SetRect(
			rcimg.right - m_handleSize,
			(rcimg.bottom + rcimg.top)/2 - m_handleSize,
			rcimg.right + m_handleSize,
			(rcimg.bottom + rcimg.top)/2 + m_handleSize);
		if(rc.PtInRect(pt))
			return 0x00010000;
	}

	if(code&0x00100000)
	{
		rc.SetRect(
			rcimg.left - m_handleSize,
			rcimg.bottom - m_handleSize,
			rcimg.left + m_handleSize,
			rcimg.bottom + m_handleSize);
		if(rc.PtInRect(pt))
			return 0x00100000;
	}

	if(code&0x01000000)
	{
		rc.SetRect(
			(rcimg.right + rcimg.left)/2 - m_handleSize,
			rcimg.bottom - m_handleSize,
			(rcimg.right + rcimg.left)/2 + m_handleSize,
			rcimg.bottom + m_handleSize);
		if(rc.PtInRect(pt))
			return 0x01000000;
	}

	if(code&0x10000000)
	{
		rc.SetRect(
			rcimg.right - m_handleSize,
			rcimg.bottom - m_handleSize,
			rcimg.right + m_handleSize,
			rcimg.bottom + m_handleSize);
		if(rc.PtInRect(pt))
			return 0x10000000;
	}

	return 0;
}

//-----------------------------------------------------------------------------

CRect CImageRenderer::CalcMinImageRect()
{
	CRect rc(0,0,0,0);

	CComQIPtr<IGraphVertex> v=m_gi;
	CComQIPtr<IGraphSubgraph> sg=m_gi;

	CComQIPtr<ILegacyRenderer> ilr;
	CComQIPtr<ILegacySubgraphRenderer> ilsgr;

	if(v!=0)
		ilr=m_parentRenderer;
	else if(sg!=0)
		ilsgr=m_parentRenderer;
	else
		return rc;

	const ImageInfoMaps* imgmaps;
	RectF rctext;

	if(v!=0)
	{
		imgmaps=v->GetImages();
		ilr->GetTitlePos(rctext);
	}
	else
	{
		imgmaps=sg->GetImages();
		ilsgr->GetTitlePos(rctext);
	}	

	bool bdone=false;	

	for(ImageInfoMaps::const_iterator itrnodemaps=imgmaps->begin();
		itrnodemaps != imgmaps->end();
		++itrnodemaps)
	{
		if(m_canvas->m_meta.IsImageVisible(m_gi, static_cast<IMAGE_POSITION>(itrnodemaps->first)))
		{
			for(MapImageInfo::const_iterator itrnodeimages=itrnodemaps->second.begin();
				itrnodeimages!=itrnodemaps->second.end();
				++itrnodeimages)
			{
				if(m_canvas->m_selectedImage.m_pos==IMAGE_POSITION(itrnodemaps->first) &&
						m_canvas->m_selectedImage.m_overlayIdx==int(itrnodeimages->first))
				{
					const ImageInfo& img=itrnodeimages->second;
					ATLASSERT(img.image);

					REAL minsize= static_cast<GraphTypes::REAL>(m_handleSize*3);
					SizeF sizeimg(minsize, minsize);
					m_canvas->SizeTransform(sizeimg);
					RectF rcimg(0, 0, sizeimg.Width, sizeimg.Height);

					AdjustOffsets(
						rcimg,	// in screen coordinates (initially contains only Width&Height)
						rctext, // in screen coordinates
						true,
						img.pos,
						img.marginX,
						img.marginY);

					rc=rcimg;
					bdone=true;
					break;
				} // was selected image
			} // is visible image
		}

		if(bdone)
			break;
	}

	return rc;
}

//-----------------------------------------------------------------------------

LRESULT CImageRenderer::OnPaint(
	UINT /*uMsg*/,
	WPARAM wParam,
	LPARAM lParam,
	BOOL& /*bHandled*/)
{
	CAggDC& dc=*(CAggDC*)wParam;

	Draw(dc);	

	return 0;
}

//-----------------------------------------------------------------------------

LRESULT CImageRenderer::OnLButtonDown(
	UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	// nobody can be hot when we clcik over image
	m_canvas->ClearCurrentHot();

	if(IsSelected())
	{
		if(m_sharedhit)
		{
			m_canvas->m_selectedImage.Clear();
			Invalidate();
		}
		else
		{
			CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)); 
			// test handles
			unsigned code=HitTestHandles(pt, m_rcSelImg, m_canvas->m_selectedImage.m_pos);
			if(code==0)
			{
				SetCursor(LoadCursor(NULL, IDC_ARROW));
				if(m_canvas->m_selectedImage.Select(this, m_pos, m_overlayIdx))
					Invalidate();
			}
			else
			{
				m_canvas->SetCapture(this);
				m_capturePos=pt;
				m_keepAspectRatio=(wParam&MK_SHIFT)!=0;
			}
		}
	}
	else
	{
		// don't allow selecting shared images
		if(!m_sharedhit && m_canvas->m_selectedImage.Select(this, m_pos, m_overlayIdx))
			Invalidate();
	}

	return 0;
}

//-----------------------------------------------------------------------------

LRESULT CImageRenderer::OnLButtonUp(
	UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(m_canvas->GetCapture()==this)
	{
		m_canvas->ReleaseCapture();
	}
	else
	{
		if(!IsSelected())
			m_hotTracking.SetHot(true);			
	}

	return 0;
}

//-----------------------------------------------------------------------------

LRESULT CImageRenderer::OnMouseMove(
	UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)); 

	if(m_canvas->GetCapture()==this)
	{
		// nobody could be hot during capture
		m_canvas->ClearCurrentHot();

		//TODO this really asks for unified image interface to IGraphItem
		CComQIPtr<IGraphVertex> v=m_gi;
		CComQIPtr<IGraphSubgraph> sg=m_gi;
		if(v || sg)
		{
			CSize diff(pt.x-m_capturePos.x, pt.y-m_capturePos.y); 
			// restrict movement along x or y axis based on handle code when mouse captured
			if(m_hitCode==0x00001000 || m_hitCode==0x00010000)
				diff.cy=0;
			if(m_hitCode==0x00000010 || m_hitCode==0x01000000)
				diff.cx=0;

			//ATLTRACE(_T("Orig rect			left:%d, top:%d, bottom:%d, right:%d, width:%d, height:%d\r\n"), 
			//	m_rcSelImg.left, m_rcSelImg.top, m_rcSelImg.bottom, m_rcSelImg.right, m_rcSelImg.Width(), m_rcSelImg.Height()); 

			m_rcSelImgResized=m_rcSelImg;
			if(m_hitCode==0x00000001 || m_hitCode==0x00001000 || m_hitCode==0x00100000)
				diff.cx=-diff.cx;

			if(m_hitCode<=0x00001000)
			{
				diff.cy=-diff.cy;
				m_rcSelImgResized.InflateRect(diff.cx, diff.cy, 0, 0);
			}
			else
			{
				m_rcSelImgResized.InflateRect(0, 0, diff.cx, diff.cy);
			}

			CRect minrect=CalcMinImageRect();
			if(m_rcSelImgResized.Width()<minrect.Width())
			{
				m_rcSelImgResized.left=minrect.left;
				m_rcSelImgResized.right=minrect.right;
			}

			if(m_rcSelImgResized.Height()<minrect.Height())
			{
				m_rcSelImgResized.top=minrect.top;
				m_rcSelImgResized.bottom=minrect.bottom;
			}

			//ATLTRACE(_T("New rect			left:%d, top:%d, bottom:%d, right:%d, width:%d, height:%d\r\n"), 
			//	m_rcSelImgResized.left, m_rcSelImgResized.top, m_rcSelImgResized.bottom, m_rcSelImgResized.right, m_rcSelImgResized.Width(), m_rcSelImgResized.Height()); 

			if(m_keepAspectRatio)
			{
				// is dominant move vertical or horizontal?
				bool bVert=abs(m_rcSelImgResized.Height()-m_rcSelImg.Height())>abs(m_rcSelImgResized.Width()-m_rcSelImg.Width());
				if(bVert)
				{
					REAL ratio=REAL(m_rcSelImg.Width())/REAL(m_rcSelImg.Height());
					int resizewidth = round_int(REAL(m_rcSelImgResized.Height())*ratio);
					int diff=resizewidth-m_rcSelImgResized.Width();
					m_rcSelImgResized.InflateRect(0, 0, diff, 0);
				}
				else
				{
					REAL ratio=REAL(m_rcSelImg.Height())/REAL(m_rcSelImg.Width());
					int resizeheight = round_int(REAL(m_rcSelImgResized.Width())*ratio);
					int diff=resizeheight-m_rcSelImgResized.Height();
					m_rcSelImgResized.InflateRect(0, 0, 0, diff);
				}
				//ATLTRACE(_T("New rect(aspect)	left:%d, top:%d, bottom:%d, right:%d, width:%d, height:%d\r\n"), 
				//m_rcSelImgResized.left, m_rcSelImgResized.top, m_rcSelImgResized.bottom, m_rcSelImgResized.right, m_rcSelImgResized.Width(), m_rcSelImgResized.Height()); 
			}

			SizeF newsizef(m_rcSelImgResized.Size());
			m_canvas->RSizeTransform(newsizef);
			CSize newsize=newsizef;
			if(v!=0)
				v->SetImageSize(m_canvas->m_selectedImage.m_pos, m_canvas->m_selectedImage.m_overlayIdx, newsize);
			else
				sg->SetImageSize(m_canvas->m_selectedImage.m_pos, m_canvas->m_selectedImage.m_overlayIdx, newsize);
			Invalidate();
		}
	}
	else
	{
		if(m_hotTracking.SetHot(true))
		{
			m_hotPos=m_pos;
			m_hotOverlayIdx=m_overlayIdx;
		}
		else
		{
		// TODO we have only one hot detector for 8 images
		// so this dirt is required to track changes. Separate hot
		// state tracker for each individual image may be cleaner
			if(m_hotPos!=m_pos || m_hotOverlayIdx!=m_overlayIdx)
			{
				Invalidate();
				m_hotPos=m_pos;
				m_hotOverlayIdx=m_overlayIdx;
			}
		}
	}
	return 0;
}

//-----------------------------------------------------------------------------

LRESULT CImageRenderer::OnMouseDblclk(
	UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	LRESULT lRes=1; // 1 tells upper-level code that no reizing handle was hit

	if(IsSelected())
	{
		CComQIPtr<IGraphVertex> v=m_gi;
		CComQIPtr<IGraphSubgraph> sg=m_gi;
		if(v || sg)
		{
			CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)); 

			unsigned code=HitTestHandles(pt, m_rcSelImgResized, m_canvas->m_selectedImage.m_pos);
			if(code!=0)
			{
				const ImageInfoMaps* imgmaps=v->GetImages();
				if(v!=0)
					imgmaps=v->GetImages();
				else
					imgmaps=sg->GetImages();

				for(ImageInfoMaps::const_iterator itrnodemaps=imgmaps->begin();
					itrnodemaps != imgmaps->end();
					++itrnodemaps)
				{
					for(MapImageInfo::const_iterator itrnodeimages=itrnodemaps->second.begin();
						itrnodeimages!=itrnodemaps->second.end();
						++itrnodeimages)
					{
						if(m_canvas->m_selectedImage.m_pos==IMAGE_POSITION(itrnodemaps->first) &&
							m_canvas->m_selectedImage.m_overlayIdx==int(itrnodeimages->first))
						{
							const ImageInfo& img=itrnodeimages->second;

							CSize newsize;
							img.GetSourceSize(newsize);
							if(v)
								v->SetImageSize(m_canvas->m_selectedImage.m_pos, m_canvas->m_selectedImage.m_overlayIdx, newsize);
							else
								sg->SetImageSize(m_canvas->m_selectedImage.m_pos, m_canvas->m_selectedImage.m_overlayIdx, newsize);
							Invalidate();
							lRes=0;
							break;
						}
					}
				}
			}
		}
	}
	return lRes;	
}

//-----------------------------------------------------------------------------

LRESULT CImageRenderer::OnNcHitTest(
	UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return HTCLIENT;
}

//-----------------------------------------------------------------------------

LRESULT CImageRenderer::OnSetCursor(
	UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	LRESULT lRes=1;

	switch(m_hitCode)
	{
	case 0x00000001:
	case 0x10000000:
		SetCursor(LoadCursor(NULL, IDC_SIZENWSE));
		break;

	case 0x00000100:
	case 0x00100000:
		SetCursor(LoadCursor(NULL, IDC_SIZENESW));
		break;

	case 0x00000010:
	case 0x01000000:
		SetCursor(LoadCursor(NULL, IDC_SIZENS));
		break;

	case 0x00001000:
	case 0x00010000:
		SetCursor(LoadCursor(NULL, IDC_SIZEWE));
		break;

	default:
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		break;
	}

	return lRes;
}

//*****************************************************************************

IRenderer* CreateImageRenderer(
	CCanvas & parent,
	IGraphItem * item)
{
	CImageRenderer * retVal = new CImageRenderer(parent, item);
	return retVal;
}
