#include "stdafx.h"
#include "resource.h"

#include <atlmisc.h>
#include "GraphTypes.h"
//NOTE - if defined, has to be defined in DDrawSupport.cpp too
#ifdef __HAVE_DIRECTX_HEADERS__
#include "DDrawSupport.h"
#endif
#include "AggCommon.h"
#include "AggDC.h"

//*****************************************************************************
//#define __16BIT_COLOR555__

#ifdef __16BIT_COLOR555__
#include "agg_pixfmt_rgb_packed.h"
#define pix_format agg::pix_format_rgb555
typedef agg::pixfmt_rgb555 pixfmt;
typedef agg::pixfmt_rgb555_pre pixfmt_pre;
typedef agg::rgba8 color_type;
typedef agg::pixfmt_rgb555 pixel_format;
#elif defined(__16BIT_COLOR565__)
//#define pix_format agg::pix_format_rgb565
//typedef agg::pixfmt_rgb565 pixfmt;
//typedef agg::pixfmt_rgb565_pre pixfmt_pre;
//typedef agg::rgba8 color_type;
//typedef agg::pixfmt_rgb565 pixel_format;
#else
typedef agg::pixfmt_bgra32 pixel_format;
#endif

typedef agg::renderer_base<pixel_format> ren_base;
typedef agg::rasterizer_scanline_aa<agg::rasterizer_sl_clip_int_sat> rasterizer_aa;
typedef agg::renderer_scanline_aa_solid<ren_base> solid_renderer;
typedef agg::renderer_outline_aa<ren_base> outline_renderer;
typedef agg::rasterizer_outline_aa<outline_renderer> outline_rasterizer_aa;

typedef agg::renderer_primitives<ren_base> primitive_renderer; // not AA

//*****************************************************************************

class CAggMemoryDC : public CAggDC
{
	class CGammaThreshold
	{
		double m_threshold;
	public:
		CGammaThreshold(double threshold) : m_threshold(threshold) {};
		void SetThreshold(double threshold) { m_threshold=threshold; }
		double operator ()(double g) const
		{ 
			if(m_threshold==-1)
				return g;
			else
				return (g < m_threshold) ? 0 : 1; 
		}
	};

protected:
	static bool s_bInited;
	static bool	s_bTransOk; // true if OS version has non-leaky TransparentBlt
	static agg::line_profile_aa s_lineprof;
	static void OneTimeInit(CAggMemoryDC* myself);
	CAggFonts*		m_fonts;
#ifdef __PROFILE_DIRECTX__
	static bool ddrawprofilingtoggle; // for toggling ddraw and ordinary blits
#endif

	bool CreateAggBitmap(int width, int height, void** ppbuffer);
	void PrepareBufferMemory(CDC& dc);

	HBITMAP m_hbmp;
	void* m_buf;
	agg::rendering_buffer m_rbuf;
	agg::scanline_p8 m_sl;
	agg::path_storage m_path;
	agg::path_storage m_pathCache;
	agg::rgba8* m_colorsCache;
	unsigned m_npaths, m_countpaths;
	unsigned* m_path_idxCache;
    agg::trans_affine m_mtx;
	std::stack<agg::trans_affine> m_transformStack;
	conv_path_trans_type m_transpath;
	conv_path_trans_type m_transpathCache;
	conv_path_clip_trans_type m_clippath;
	rasterizer_aa m_ras_aa;
	HBITMAP m_oldhbmp;
	CRect m_rcUpdate;
	BYTE m_Alpha;
	CGammaThreshold* m_gammaThreshold;

public:
	CAggMemoryDC(CDC& dc, CAggFonts * fonts, CRect* rc=0, BYTE alpha=255, bool usebmpmem=true);
	virtual ~CAggMemoryDC();

	void GetUpdateRect(CRect* rc) { *rc=m_rcUpdate; }
	void SetAlpha(BYTE alpha) { m_Alpha=alpha; }
	void TransformPush() { m_transformStack.push(m_mtx); }
	void TransformPop() { m_mtx=m_transformStack.top(); m_transformStack.pop(); }
	void Transform(GraphTypes::REAL scale);
	void Transform(const GraphTypes::PointF& offset); 
	void TransformReset() { m_mtx.reset(); }
	void BeginPathCache(unsigned npaths);
	void EndPathCache(); 
	void SetGammaThreshold(double thr/*=-1*/);

	void Line(
		const GraphTypes::PointF& p1,
		const GraphTypes::PointF& p2, 
		const GraphTypes::Color& clr);

	void Lines(
		const GraphTypes::PointFVector& pts,
		const GraphTypes::Color& clr,
		const GraphTypes::REAL width=1.0);

	void Rect(
		const GraphTypes::PointF& tl, 
		const GraphTypes::PointF& br,
		const GraphTypes::Color& clrfill,
		const GraphTypes::Color& clroutline,
		GraphTypes::REAL outline_width=1.0,
		bool bAA=false);

	void SolidPolygon(
		const GraphTypes::PointF* points, 
		const GraphTypes::Color& clr,
		unsigned point_count);

	void SolidPolygon(
		const GraphTypes::PointF* points, 
		const GraphTypes::Color& clr,
		unsigned point_count,
		GraphTypes::REAL outline_width,
		const GraphTypes::Color& clroutline);

	void Bezier4(
		const GraphTypes::PointF& p1,
		const GraphTypes::PointF& p2,
		const GraphTypes::PointF& p3,
		const GraphTypes::PointF& p4,
		const GraphTypes::Color& clr,
		double width=1.0,
		bool bArrow=true,
		GraphTypes::REAL scale=1.0);

	void Bezier(
		const GraphTypes::PointFVector& points,
		const GraphTypes::Color& clr,
		double width=1.0,
		bool bArrow=true,
		bool bDashed=false,
		GraphTypes::REAL scale=1.0, 
		const GraphTypes::Color& firstClrControlPoint = 0x00000000, 
		const GraphTypes::Color& lastClrControlPoint = 0x00000000);

	void LollyPop(
		const GraphTypes::RectF& rc, 
		const GraphTypes::Color& clr,
		const GraphTypes::Color& clroutline,
		bool bLeft);

	void LinearGradientRect(
		const GraphTypes::RectF& rc,
		const GraphTypes::Color& clrstart,
		const GraphTypes::Color& clrend,
		bool bHorizontal=true);

	unsigned DrawScaledText(
		const TCHAR* text,
		const GraphTypes::RectF& rc,
		const GraphTypes::Color& clr,
		const char* font,
		int size,
		const GraphTypes::PointF & descsubtract);

	bool DrawBitmap(
		HBITMAP hBitmap,
		int x, 
		int y,
		int width,
		int height,
		int bmpWidth,
		int bmpHeight);

	bool DrawTransparent(
		HBITMAP hBitmap,
		int x, 
		int y,
		int width,
		int height,
		int bmpWidth,
		int bmpHeight,
		COLORREF color);

	int SetDIBitsToDevice(
		int x, 
		int y, 
		DWORD dwWidth, 
		DWORD dwHeight, 
		int xSrc, 
		int ySrc, 
		UINT uStartScan, 
		UINT cScanLines, 
		CONST VOID* lpvBits, 
		CONST BITMAPINFO* lpbmi, 
		UINT uColorUse);

	bool DrawAlphaBlend(
		int x, 
		int y, 
		int nWidth, 
		int nHeight, 
		HDC hSrcDC, 
		int xSrc, 
		int ySrc, 
		int nSrcWidth, 
		int nSrcHeight, 
		BLENDFUNCTION bf);

	HBITMAP DetachBitmap();

};

//*****************************************************************************
// statics

bool CAggMemoryDC::s_bInited;
bool CAggMemoryDC::s_bTransOk;
agg::line_profile_aa CAggMemoryDC::s_lineprof;

//*****************************************************************************

#ifdef __PROFILE_DIRECTX__
#define PROFILE	&Profile
#include "HWProf.h"
CHWProfile Profile;
bool CAggMemoryDC::ddrawprofilingtoggle;
#endif

//*****************************************************************************

using namespace GraphTypes;

//*****************************************************************************

CAggMemoryDC::CAggMemoryDC(CDC& dc, CAggFonts * fonts, CRect* rc/*=0*/, BYTE alpha/*=255*/, bool usebmpmem/*=true*/)
	: m_hbmp(0), m_buf(0), m_gammaThreshold(0), m_Alpha(alpha), m_npaths(0), m_countpaths(0), m_colorsCache(0), m_path_idxCache(0),
	m_transpath(m_path, m_mtx), m_clippath(m_transpath), m_transpathCache(m_pathCache, m_mtx)

{
	m_fonts=fonts;

	OneTimeInit(this);

	if(rc==0)
		dc.GetClipBox(&m_rcUpdate);
	else
		m_rcUpdate=*rc;	

	if(usebmpmem)
		PrepareBufferMemory(dc);
}

//-----------------------------------------------------------------------------

CAggMemoryDC::~CAggMemoryDC()
{
	if(m_hbmp)
	{
		::SelectObject(m_hDC, m_oldhbmp);
		::DeleteObject(m_hbmp);
	}

	delete m_gammaThreshold;
}

//*****************************************************************************

CAggDC* CAggDC::CreateMemoryDC(
	CDC& dc, CAggFonts * fonts, CRect* rc/*=0*/, BYTE alpha/*=255*/)
{
	CAggDC* aggdc=0;

	aggdc=new CAggMemoryDC(dc, fonts, rc, alpha);

	return aggdc;
}

//-----------------------------------------------------------------------------

void CAggMemoryDC::Transform(REAL scale)
{
	m_mtx *= agg::trans_affine_scaling(scale);
}

//-----------------------------------------------------------------------------

void CAggMemoryDC::Transform(const PointF& offset)
{
	m_mtx *= agg::trans_affine_translation(offset.x, offset.y);
}

//-----------------------------------------------------------------------------

void CAggMemoryDC::BeginPathCache(unsigned npaths)
{
#ifdef __PROFILE_DIRECTX__
	NEXT_PROF(_T("PATH CACHE"));
#endif
	if(npaths>0)
	{
		m_colorsCache = new agg::rgba8[npaths];
		m_npaths=npaths;
		m_path_idxCache = new unsigned[npaths];
	}
}

//-----------------------------------------------------------------------------

void CAggMemoryDC::EndPathCache()
{
	if(m_countpaths>0)
	{
		pixel_format pixf(m_rbuf);
		ren_base renb(pixf);
		solid_renderer ren_solid(renb);

		m_ras_aa.clip_box(0, 0, m_rcUpdate.Width(), m_rcUpdate.Height()); 
		// m_pathCache.arrange_orientations_all_paths(agg::path_flags_cw); //?crashes?

		for(unsigned i=0; i<m_countpaths; ++i)
		{
			m_ras_aa.reset();
			m_ras_aa.add_path(m_transpathCache, m_path_idxCache[i]);
			ren_solid.color(m_colorsCache[i]);
			agg::render_scanlines(m_ras_aa, m_sl, ren_solid);
		}
	}

	if(m_npaths>0)
	{
		delete [] m_colorsCache;
		m_colorsCache=0;
		delete [] m_path_idxCache;
		m_path_idxCache=0;
	}

#ifdef __PROFILE_DIRECTX__
	END_PROF();
#endif
}

//-----------------------------------------------------------------------------
//
// -1 resets the threshold

void CAggMemoryDC::SetGammaThreshold(double thr/*=-1*/) 
{
	if(m_gammaThreshold==0)
	{
		m_gammaThreshold = new CGammaThreshold(thr);
		m_ras_aa.gamma(*m_gammaThreshold);
	}
	else
	{
		m_gammaThreshold->SetThreshold(thr);
	}

}

//-----------------------------------------------------------------------------

void CAggMemoryDC::Line(
	const PointF& p1,
	const PointF& p2, 
	const Color& clr)
{
	if (m_buf==0)
		return;

	CPoint _p1 = p1;
	CPoint _p2 = p2;

	pixel_format pixf(m_rbuf);
	ren_base renb(pixf);

	outline_renderer outline_rd(renb, s_lineprof);
	outline_rasterizer_aa outline_rt(outline_rd);

	outline_rd.profile(s_lineprof);
	outline_rd.color(agg::rgba8(clr.GetR(), clr.GetG(), clr.GetB(), clr.GetA()));
	outline_rd.clip_box(0, 0, m_rcUpdate.Width(), m_rcUpdate.Height());

	double p1x(p1.x), p1y(p1.y), p2x(p2.x), p2y(p2.y);
	m_mtx.transform(&p1x,&p1y);
	m_mtx.transform(&p2x,&p2y);

	outline_rt.move_to_d(p1x, p1y);
	outline_rt.line_to_d(p2x, p2y);
	//outline_rt.round_cap( 1 );
	outline_rt.render(false);
}

//-----------------------------------------------------------------------------
// 
// Multiple (may or may not overlap, be connected or whatever) lines. 
//

void CAggMemoryDC::Lines(
	const PointFVector& pts,
	const Color& clr,
	const REAL width/*=1.0*/)
{
	if (m_buf==0)
		return;

	unsigned count=pts.size()/2;
	ATLASSERT(count>0);
	ATLASSERT((float)count/2.0f>0);

	pixel_format pixf(m_rbuf);
	ren_base renb(pixf);
	solid_renderer ren_solid(renb);

	m_path.remove_all();

	for(unsigned i=0; i<count; ++i)
	{
		m_path.move_to(pts[i*2].x, pts[i*2].y);
		m_path.line_to(pts[i*2+1].x, pts[i*2+1].y);
	}

	typedef agg::conv_stroke<conv_path_trans_type> conv_stroke_outline;
	conv_stroke_outline stroke(m_transpath);
	stroke.width(width);

	m_ras_aa.add_path(stroke);

	ren_solid.color(agg::rgba8(clr.GetR(), clr.GetG(), clr.GetB(), clr.GetA()));
	agg::render_scanlines(m_ras_aa, m_sl, ren_solid);
}

//-----------------------------------------------------------------------------

void CAggMemoryDC::Rect(
	const GraphTypes::PointF& tl, 
	const GraphTypes::PointF& br,
	const GraphTypes::Color& clrfill,
	const GraphTypes::Color& clroutline,
	GraphTypes::REAL outline_width/*=1.0*/,
	bool bAA/*=false*/)
{
	if (m_buf==0)
		return;

	if(bAA)
	{
		PointF points[4];
		points[0].x=tl.x;
		points[0].y=tl.y;
		points[1].x=br.x;
		points[1].y=tl.y;
		points[2].x=br.x;
		points[2].y=br.y;
		points[3].x=tl.x;
		points[3].y=br.y;

		SolidPolygon(points, clrfill, 4, outline_width, clroutline); 
	}
	else
	{
		pixel_format pixf(m_rbuf);
		ren_base renb(pixf);

		primitive_renderer pren(renb);

		pren.fill_color(agg::rgba8(
			clrfill.GetR(),
			clrfill.GetG(),
			clrfill.GetB(),
			clrfill.GetA()));
		pren.line_color(agg::rgba8(
			clroutline.GetR(),
			clroutline.GetG(),
			clroutline.GetB(),
			clroutline.GetA()));

		double topx(tl.x), topy(tl.y), bottomx(br.x), bottomy(br.y);
		m_mtx.transform(&topx,&topy);
		m_mtx.transform(&bottomx,&bottomy);
		int x1=round_int(topx); 
		int y1=round_int(topy); 
		int x2=round_int(bottomx); 
		int y2=round_int(bottomy); 

		pren.solid_rectangle(x1, y1, x2, y2);
		pren.rectangle(x1, y1, x2, y2);
	}
}

//-----------------------------------------------------------------------------

void CAggMemoryDC::SolidPolygon(
	const PointF* points, 
	const Color& clr,
	unsigned point_count)
{
	if (m_buf==0)
		return;

	if(m_npaths>0)
	{
		m_pathCache.close_polygon();
        m_colorsCache[m_countpaths] = agg::rgba8(clr.GetR(), clr.GetG(), clr.GetB(), clr.GetA());
		m_path_idxCache[m_countpaths] = m_pathCache.start_new_path();
		m_countpaths++;
		m_pathCache.move_to(points->x, points->y);
		while(--point_count>0)
		{
			points++;
			m_pathCache.line_to(points->x, points->y);
		}
	}
	else
	{
		pixel_format pixf(m_rbuf);
		ren_base renb(pixf);
		solid_renderer ren_solid(renb);


		ATLASSERT(point_count>0);

		m_path.remove_all();
		m_path.move_to(points->x, points->y);
		while(--point_count>0)
		{
			points++;
			m_path.line_to(points->x, points->y);
		}

		m_ras_aa.reset();
		m_ras_aa.clip_box(0, 0, m_rcUpdate.Width(), m_rcUpdate.Height()); 
		m_ras_aa.add_path(m_transpath);

		ren_solid.color(agg::rgba8(clr.GetR(), clr.GetG(), clr.GetB(), clr.GetA()));
		agg::render_scanlines(m_ras_aa, m_sl, ren_solid);
	}
}

//-----------------------------------------------------------------------------

void CAggMemoryDC::SolidPolygon(
	const PointF* points, 
	const Color& clr,
	unsigned point_count,
	REAL widthoutline,
	const Color& clroutline )
{
	if (m_buf==0)
		return;

	ATLASSERT(point_count>0);

	pixel_format pixf(m_rbuf);
	ren_base renb(pixf);

	solid_renderer ren_solid(renb);
	m_ras_aa.reset();

	m_path.remove_all();
	m_path.move_to(points->x, points->y);
	while(--point_count>0)
	{
		points++;
		m_path.line_to(points->x, points->y);
	}
	m_path.close_polygon();

	m_ras_aa.add_path(m_transpath);
	ren_solid.color(agg::rgba8(clr.GetR(), clr.GetG(), clr.GetB(), clr.GetA()));
	agg::render_scanlines(m_ras_aa, m_sl, ren_solid);

	if(widthoutline>REAL(0))
	{
		typedef agg::conv_stroke<conv_path_trans_type> conv_trans_stroke_outline;
		ren_solid.color(agg::rgba8(clroutline.GetR(),
			clroutline.GetG(), clroutline.GetB(), clroutline.GetA()));
		conv_trans_stroke_outline stroke(m_transpath);
		stroke.width(widthoutline);
		m_ras_aa.add_path(stroke);
		agg::render_scanlines(m_ras_aa, m_sl, ren_solid);
	}
}

//-----------------------------------------------------------------------------
//
//	Bezier defined by 4 control points

void CAggMemoryDC::Bezier4(
	const GraphTypes::PointF& p1,
	const GraphTypes::PointF& p2,
	const GraphTypes::PointF& p3,
	const GraphTypes::PointF& p4,
	const GraphTypes::Color& clr,
	double width/*=1.0*/,
	bool bArrow/*=true*/,
	GraphTypes::REAL scale/*=1.0*/)
{
	if (m_buf==0)
		return;

	pixel_format pixf(m_rbuf);
	ren_base renb(pixf);
	solid_renderer ren_solid(renb);
 
	agg::curve4 curve(
		p1.x, p1.y,
		p2.x, p2.y,
		p3.x, p3.y,
		p4.x, p4.y);

    typedef agg::conv_stroke<agg::curve4, agg::vcgen_markers_term> conv_stroke_type;
    conv_stroke_type stroke(curve);
	stroke.width(width);

	if(bArrow)
	{
		AddPathWithArrowHead(m_ras_aa, stroke, m_mtx, 10, 3, 2, scale);
	}
	else
	{
		typedef agg::conv_transform<conv_stroke_type, agg::trans_affine> conv_trans_type;
		conv_trans_type transstroke(stroke, m_mtx);
		m_ras_aa.add_path(transstroke);
	}

	ren_solid.color(agg::rgba8(clr.GetR(), clr.GetG(), clr.GetB(), clr.GetA()));
	agg::render_scanlines(m_ras_aa, m_sl, ren_solid);
}

//-----------------------------------------------------------------------------
//
// (num_points-1) degree Bezier drawn using de Casteljau subdivision

void CAggMemoryDC::Bezier(
	const GraphTypes::PointFVector& points,
	const Color& clr,
	double width/*=2.0*/,
	bool bArrow/*=true*/,
	bool bDashed/*=false*/,
	REAL scale/*=1.0*/,
	const GraphTypes::Color& firstClrControlPoint, 
	const GraphTypes::Color& lastClrControlPoint)
{
	if (m_buf==0)
		return;

	pixel_format pixf(m_rbuf);
	ren_base renb(pixf);
	solid_renderer ren_solid(renb);

	GraphTypes::PointFVector::const_iterator itr=points.begin();
    agg::curve_casteljau curve(points.size());
    curve.approximation_scale(scale);
	for(; itr!=points.end();++itr)
	{
        curve.add_vertex((*itr).x, (*itr).y);
    }

	m_ras_aa.clip_box(0, 0, m_rcUpdate.Width(), m_rcUpdate.Height()); 

    typedef agg::conv_dash<agg::curve_casteljau, agg::vcgen_markers_term> conv_dash_type;
    typedef agg::conv_stroke<agg::curve_casteljau, agg::vcgen_markers_term> conv_stroke_type;
    typedef agg::conv_stroke<conv_dash_type, agg::vcgen_markers_term> conv_dash_stroke_type;

	if(bDashed)
	{
		conv_dash_type dash(curve);
		conv_dash_stroke_type stroke(dash);
		stroke.width(width);

	    double k = ::pow(stroke.width(), 0.7)*scale;

		if(bArrow)
		{
			// can't use AddPathWithArrowHead(m_ras_aa, stroke, m_mtx, 10, 3, 2, scale);
		    agg::arrowhead ah;
			ah.head(0, (10-2) * k, 3 * k, 2 * k);
			typedef agg::conv_marker<agg::vcgen_markers_term, agg::arrowhead> arrow_type;
			// arrow must take markers before dash markers are added
			arrow_type arrow(dash.markers(), ah);

		    dash.add_dash(k*4, k*4);
			dash.dash_start(k*4);

			stroke.shorten(k*(10-2));
			typedef agg::conv_transform<conv_dash_stroke_type, agg::trans_affine> conv_trans_type;
			conv_trans_type pathtrans(stroke, m_mtx);
			m_ras_aa.add_path(pathtrans);

			typedef agg::conv_transform<arrow_type, agg::trans_affine> arrow_trans_type;
			arrow_trans_type arrowtrans(arrow, m_mtx); 
			m_ras_aa.add_path(arrowtrans);
		}
		else
		{
			typedef agg::conv_transform<conv_dash_stroke_type, agg::trans_affine> conv_trans_type;
		    dash.add_dash(k*4, k*4);
			dash.dash_start(k*4);
			conv_trans_type transstroke(stroke, m_mtx);
			m_ras_aa.add_path(transstroke);
		}
	}
	else
	{
		conv_stroke_type stroke(curve);
		stroke.width(width);

		if(bArrow)
			AddPathWithArrowHead(m_ras_aa, stroke, m_mtx, 10, 3, 2, scale);
		else
		{
			typedef agg::conv_transform<conv_stroke_type, agg::trans_affine> conv_trans_type;
			conv_trans_type transstroke(stroke, m_mtx);
			m_ras_aa.add_path(transstroke);
		}
	}

	ren_solid.color(agg::rgba8(clr.GetR(), clr.GetG(), clr.GetB(), clr.GetA()));
	agg::render_scanlines(m_ras_aa, m_sl, ren_solid);

	if (points.size() > 1)
	{
		if (firstClrControlPoint.GetValue())
		{
			agg::ellipse el_start(points[0].x, points[0].y, 4 * scale, 4 * scale, 16);
			m_ras_aa.add_path(el_start);
			ren_solid.color(agg::rgba8(firstClrControlPoint.GetR(), firstClrControlPoint.GetG(), firstClrControlPoint.GetB(), firstClrControlPoint.GetA()));
			agg::render_scanlines(m_ras_aa, m_sl, ren_solid);
		}
		if (lastClrControlPoint.GetValue())
		{
			agg::ellipse el_end(points[points.size() - 1].x, points[points.size() - 1].y, 4 * scale, 4 * scale, 16);
			m_ras_aa.add_path(el_end);
			ren_solid.color(agg::rgba8(lastClrControlPoint.GetR(), lastClrControlPoint.GetG(), lastClrControlPoint.GetB(), lastClrControlPoint.GetA()));
			agg::render_scanlines(m_ras_aa, m_sl, ren_solid);
		}
	}
}

//-----------------------------------------------------------------------------

void CAggMemoryDC::LollyPop(
	const GraphTypes::RectF& rc, 
	const GraphTypes::Color& clr,
	const GraphTypes::Color& clroutline,
	bool bLeft)
{
	if (m_buf==0)
		return;

	pixel_format pixf(m_rbuf);
	ren_base renb(pixf);

	agg::line_profile_aa lnprof;
	lnprof.width(1.0);

	solid_renderer ren_solid(renb);
	outline_renderer outline_rd(renb, lnprof);

	outline_rasterizer_aa outline_rt(outline_rd);
	outline_rd.color(agg::rgba8(clroutline.GetR(),
		clroutline.GetG(), clroutline.GetB(), clroutline.GetA()));

	REAL radius=rc.Height/2;
	REAL split=rc.Width-rc.Height;
	REAL xcenter=bLeft? (rc.x+radius) : (rc.x+rc.Width-radius);
	REAL ycenter=rc.y+radius;

	agg::ellipse circle(xcenter, ycenter, radius, radius, 10);
	typedef agg::conv_transform<agg::ellipse, agg::trans_affine> conv_ellipse_trans_type;
	conv_ellipse_trans_type m_transcircle(circle, m_mtx);

	// connector line
	PointF p1, p2;
	p1.x=bLeft ? (rc.x+rc.Width-split) : rc.x; 
	p1.y=rc.y+radius; 

	p2.x=bLeft ? (rc.x+rc.Width) : (rc.x+split); 
	p2.y=p1.y; 

	m_path.remove_all();
	m_path.move_to(p1.x, p1.y);
	m_path.line_to(p2.x, p2.y);

	m_ras_aa.reset();

	m_ras_aa.add_path(m_transcircle);

	outline_rt.add_path(m_transcircle);
	outline_rt.add_path(m_transpath);

	ren_solid.color(agg::rgba8(clr.GetR(), clr.GetG(), clr.GetB(), clr.GetA()));
	agg::render_scanlines(m_ras_aa, m_sl, ren_solid);
	outline_rt.render(true);
}

//-----------------------------------------------------------------------------

void CAggMemoryDC::LinearGradientRect(
	const RectF& rc,
	const Color& clrstart,
	const Color& clrend,
	bool bHorizontal/*=true*/)
{
  // The gradient color array
    typedef agg::pod_auto_array<agg::rgba8, 256> color_array_type;

    // Gradient shape function (linear, radial, custom, etc)
    //-----------------
    typedef agg::gradient_xy gradient_func_type;   


    // Span interpolator. This object is used in all span generators 
    // that operate with transformations during iterating of the spans,
    // for example, image transformers use the interpolator too.
    //-----------------
    typedef agg::span_interpolator_linear<> interpolator_type;


    // Span allocator is an object that allocates memory for 
    // the array of colors that will be used to render the 
    // color spans. One object can be shared between different 
    // span generators.
    //-----------------
    typedef agg::span_allocator<agg::rgba8> span_allocator_type;


    // Finally, the gradient span generator working with the agg::rgba8 
    // color type. 
    // The 4-th argument is the color function that should have 
    // the [] operator returning the color in range of [0...255].
    // In our case it will be a simple look-up table of 256 colors.
    //-----------------
    typedef agg::span_gradient<agg::rgba8, 
                               interpolator_type, 
                               gradient_func_type, 
                               color_array_type> span_gradient_type;


    // The gradient scanline renderer type
    //-----------------
    typedef agg::renderer_scanline_aa<ren_base, 
									span_allocator_type,	
                                    span_gradient_type> renderer_gradient_type;


    // Common declarations (pixel format and basic renderer).
    //----------------
	pixel_format pixf(m_rbuf);
	ren_base renb(pixf);

    // The gradient objects declarations
    //----------------
    gradient_func_type  gradient_func;                   // The gradient function
    agg::trans_affine   gradient_mtx;                    // Affine transformer
    interpolator_type   span_interpolator(gradient_mtx); // Span interpolator
    span_allocator_type span_allocator;                  // Span Allocator
    color_array_type    color_array;                     // Gradient colors


    // Declare the gradient span itself. 
    // The last two arguments are so called "d1" and "d2" 
    // defining two distances in pixels, where the gradient starts
    // and where it ends. The actual meaning of "d1" and "d2" depands
    // on the gradient function.
    //----------------
    span_gradient_type span_gradient(span_interpolator, 
                                     gradient_func, 
                                     color_array, 
									 bHorizontal ? rc.x : rc.y,
									 bHorizontal ? rc.x+rc.Width : rc.y+rc.Height);

    // The gradient renderer
    //----------------
    renderer_gradient_type ren_gradient(renb, span_allocator, span_gradient);

    fill_color_array(color_array, 
                     agg::rgba8(clrstart.GetR(), clrstart.GetG(), clrstart.GetB(), clrstart.GetA()),
                     agg::rgba8(clrend.GetR(), clrend.GetG(), clrend.GetB(), clrend.GetA()));

	m_path.remove_all();
	m_path.move_to(rc.x, rc.y);
	m_path.line_to(rc.x+rc.Width, rc.y);
	m_path.line_to(rc.x+rc.Width, rc.y+rc.Height);
	m_path.line_to(rc.x, rc.y+rc.Height);
	m_path.close_polygon();

	m_ras_aa.reset();
	m_ras_aa.clip_box(0, 0, m_rcUpdate.Width(), m_rcUpdate.Height()); 
	m_ras_aa.add_path(m_transpath);
	agg::render_scanlines(m_ras_aa, m_sl, ren_gradient);
}

//-----------------------------------------------------------------------------

#ifndef _WIN32_WCE
unsigned CAggMemoryDC::DrawScaledText(
		const TCHAR* text,
		const GraphTypes::RectF& rc,
		const GraphTypes::Color& clr,
		const char* font,
		int size,
		const PointF & descsubtract)
{
	if (m_buf==0)
		return 0;

	unsigned len=0;

	pixel_format pixf(m_rbuf);
	ren_base renb(pixf);
	solid_renderer ren_solid(renb);

	ATLASSERT(m_buf);
	conv_font_curve_type fcurves(m_fonts->m_fman.path_adaptor());
    conv_font_segm_type  fsegm(fcurves);
    conv_font_trans_type ftrans(fsegm, m_mtx);
 //   fsegm.approximation_scale(3.0);
 //   fcurves.approximation_scale(2.0);

	m_fonts->m_feng.flip_y(true);
	m_fonts->m_feng.hinting(true);

	if(m_fonts->m_feng.create_font(
		font, 
		agg::glyph_ren_outline,
		size,
		0.0,
		FW_NORMAL,
		false,
		ANSI_CHARSET,
		DEFAULT_PITCH | FF_SWISS
		))
	{
		double x = 0.0;
		double y = 0.0;
		const TCHAR* p = text;

		TEXTMETRIC tm;
		GetTextMetrics(&tm);

		//double descent=(tm.tmDescent>descsubtract.y)?(tm.tmDescent-descsubtract.y):(tm.tmDescent-1);
		//ATLASSERT(tm.tmDescent>descsubtract.y);
		//ATLASSERT(descent>0);
		//descent *= m_mtx.scale(); 

		//m_mtx *= agg::trans_affine_translation(rc.x-m_rcUpdate.left, rc.y+rc.Height-descent-m_rcUpdate.top);

		double ascent=tm.tmAscent-descsubtract.y;
		ATLASSERT(ascent>0.0f);
		ascent *= m_mtx.scale(); 
		m_mtx *= agg::trans_affine_translation(rc.x-m_rcUpdate.left, rc.y+ascent-m_rcUpdate.top);

		ren_solid.color(agg::rgba8(clr.GetR(), clr.GetG(), clr.GetB(), clr.GetA()));

		while(*p)
		{
			const agg::glyph_cache* glyph = m_fonts->m_fman.glyph(*p);
			if(glyph)
			{
				m_fonts->m_fman.add_kerning(&x, &y);
				m_fonts->m_fman.init_embedded_adaptors(glyph, x, y);

				if(glyph->data_type == agg::glyph_data_outline)
				{
					m_ras_aa.add_path(ftrans);
				}

				// increment pen position
				x += glyph->advance_x;
				y += glyph->advance_y;
			}
			++p;
			++len;
		}

		agg::render_scanlines(m_ras_aa, m_sl, ren_solid);
		ATLASSERT(m_buf);
	}

	return len;
}
#endif

//-----------------------------------------------------------------------------

bool CAggMemoryDC::DrawBitmap(
	HBITMAP hBitmap,
	int x, 
	int y,
	int width,
	int height,
	int bmpWidth,
	int bmpHeight)
{
	ATLASSERT(width>0&&height>0);

	CDC dcImage;
	bool bOk=dcImage.CreateCompatibleDC(m_hDC) != NULL;
	if(bOk)
	{
		HBITMAP oldbmp=dcImage.SelectBitmap(hBitmap);
		if(bmpWidth==width && bmpHeight==height)
			bOk=BitBlt(x, y, width, height, dcImage, 0, 0, SRCCOPY) != 0;
		else
			bOk=StretchBlt(x, y, width, height, dcImage, 0, 0, bmpWidth, bmpHeight, SRCCOPY) != 0;
		ATLASSERT(bOk);
		dcImage.SelectBitmap(oldbmp);
	}

	return bOk;
}

//-----------------------------------------------------------------------------

bool CAggMemoryDC::DrawTransparent(
	HBITMAP hBitmap,
	int x, 
	int y,
	int width,
	int height,
	int bmpWidth,
	int bmpHeight,
	COLORREF color)
{
	ATLASSERT(width>0&&height>0);
	m_mtx.transform(&x,&y);

	CDC dcImage;
	bool bOk=dcImage.CreateCompatibleDC(m_hDC) != NULL;
	HBITMAP oldbmp=dcImage.SelectBitmap(hBitmap);

	// if OS supports non-leaky TransparentBlt use it (also on printer)
	if(s_bTransOk)
	{
#ifndef _WIN32_WCE
		bOk=TransparentBlt(x, y, width, height, dcImage, 0, 0, bmpWidth, bmpHeight, color) != 0;
		if (!bOk) // just in case (e.g. printing), try emulation
			goto tryEmulate;
#endif
	}
	else
	{
tryEmulate:
		CBitmap bmpMono;
		bOk=bmpMono.CreateBitmap(width, height, 1, 1, NULL) != NULL;
		if (!bOk)
			return bOk;

		CDC dcMono;
		// Create dc for the mask
		bOk = dcMono.CreateCompatibleDC(m_hDC) != NULL;
		if (!bOk)
			return bOk;

		// Select the mask bitmap into its dc
		HBITMAP oldBitmapMono = (HBITMAP)::SelectObject(dcMono, bmpMono);

		if(bOk)
		{
			// Build mask based on transparent colour

			COLORREF crOldImg=dcImage.SetBkColor(color);
			bOk=dcMono.StretchBlt(0, 0, width, height, dcImage, 0, 0, bmpWidth, bmpHeight, SRCCOPY) != 0;
			dcImage.SetBkColor(crOldImg);

			// True Mask method - no flicker if destination is not actual display
			if(bOk)
				bOk=StretchBlt(x, y, width, height, dcImage, 0, 0, bmpWidth, bmpHeight, SRCINVERT) != 0;

			if(bOk) 
			{
				// setup the destination for monochrome blit
				COLORREF crOldBack = SetBkColor(RGB(255,255,255));
				COLORREF crOldText = SetTextColor(RGB(0,0,0));

				bOk=BitBlt(x, y, width, height, dcMono, 0, 0, SRCAND) != 0;

				SetBkColor(crOldBack);
				SetTextColor(crOldText);
			}

			if(bOk)
				bOk=StretchBlt(x, y, width, height, dcImage, 0, 0, bmpWidth, bmpHeight, SRCINVERT) != 0;
		}

		dcMono.SelectBitmap(oldBitmapMono);
	}

	dcImage.SelectBitmap(oldbmp);

	return bOk;
}

//-----------------------------------------------------------------------------
int CAggMemoryDC::SetDIBitsToDevice(
	int x, 
	int y, 
	DWORD dwWidth, 
	DWORD dwHeight, 
	int xSrc, 
	int ySrc, 
	UINT uStartScan, 
	UINT cScanLines, 
	CONST VOID* lpvBits, 
	CONST BITMAPINFO* lpbmi, 
	UINT uColorUse)
{
	m_mtx.transform(&x,&y);
	return ::SetDIBitsToDevice(m_hDC, x, y, dwWidth, dwHeight, xSrc, ySrc, uStartScan, cScanLines, lpvBits, lpbmi, uColorUse);
}
//-----------------------------------------------------------------------------

bool CAggMemoryDC::DrawAlphaBlend(
	int x, 
	int y, 
	int nWidth, 
	int nHeight, 
	HDC hSrcDC, 
	int xSrc, 
	int ySrc, 
	int nSrcWidth, 
	int nSrcHeight, 
	BLENDFUNCTION bf)
{
	bool bOk=false;

	 // if OS supports AlphaBlend we will use it
	if(s_bTransOk && bf.SourceConstantAlpha < 255)
	{
#ifndef _WIN32_WCE
		bOk=AlphaBlend(x, y, nWidth, nHeight, hSrcDC, xSrc, ySrc, nSrcWidth, nSrcHeight, bf) != 0;
#endif
	}
	else
	{
		bOk=BitBlt(x, y, nWidth, nHeight, hSrcDC, xSrc, ySrc, SRCCOPY) != 0;
	}
	return bOk;
}

//-----------------------------------------------------------------------------

void CAggMemoryDC::OneTimeInit(CAggMemoryDC* /*myself*/)
{
	if(s_bInited)
		return;

	s_lineprof.width(1.0);

	OSVERSIONINFO osv;
	memset(&osv, 0, sizeof osv);
	osv.dwOSVersionInfoSize=sizeof(osv);
	GetVersionEx(&osv);
	if(osv.dwPlatformId == VER_PLATFORM_WIN32_NT ||
		(osv.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS &&
			osv.dwMajorVersion >= 4 && osv.dwMinorVersion >= 90))
		s_bTransOk=true;

	s_bInited=true;
}

//-----------------------------------------------------------------------------

bool CAggMemoryDC::CreateAggBitmap(int width, int height, void** ppbuffer)
{
	*ppbuffer = 0;

	if(height==0 || width==0)
		return false;

	BITMAPINFO bmp_info; 
#ifdef __16BIT_COLOR555__
	DWORD dsBitfields[3]; 
	//  5-5-5 16-bit image, where the blue mask is 0x001F, the green mask is 0x03E0, and the red mask is 0x7C00
	dsBitfields[0]=0x001F;
	dsBitfields[1]=0x03E0;
	dsBitfields[2]=0x7C00;
#elif defined(__16BIT_COLOR565__)
	// 5-6-5 16-bit image, where the blue mask is 0x001F, the green mask is 0x07E0, and the red mask is 0xF800.
	//dsBitfields[0]=0x001F;
	//dsBitfields[1]=0x07E0;
	//dsBitfields[2]=0xF800;
#endif
	ZeroMemory(&bmp_info, sizeof(BITMAPINFO));	
	bmp_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
	bmp_info.bmiHeader.biWidth = width; 
	bmp_info.bmiHeader.biHeight = height; 
	bmp_info.bmiHeader.biPlanes = 1; 
#if defined(__16BIT_COLOR555__) || defined(__16BIT_COLOR565__)
	bmp_info.bmiHeader.biBitCount = 16; 
	bmp_info.bmiHeader.biCompression = BI_RGB; 
//	bmp_info.bmiHeader.biCompression = BI_BITFIELDS; // doesn't work with all drivers
	bmp_info.bmiHeader.biSizeImage = ((width*2+3)/4)*4*height; 
#else
	bmp_info.bmiHeader.biBitCount = 32; 
	bmp_info.bmiHeader.biCompression = BI_RGB; 
	bmp_info.bmiHeader.biSizeImage = 0; 
#endif
	bmp_info.bmiHeader.biXPelsPerMeter = 0; 
	bmp_info.bmiHeader.biYPelsPerMeter = 0; 
	bmp_info.bmiHeader.biClrUsed = 0; 
	bmp_info.bmiHeader.biClrImportant = 0; 
	bmp_info.bmiHeader.biClrImportant = 0; 

	m_hbmp = ::CreateDIBSection( 
		m_hDC, 
		&bmp_info, 
		DIB_RGB_COLORS, 
		ppbuffer, 
		0, 
		0 
		); 

	ATLASSERT(m_hbmp);
	return m_hbmp!=0;
}

//-----------------------------------------------------------------------------

void CAggMemoryDC::PrepareBufferMemory(CDC& dc)
{
	CreateCompatibleDC(dc);

	if(CreateAggBitmap(m_rcUpdate.Width(), m_rcUpdate.Height(), &m_buf))
	{

		m_oldhbmp=(HBITMAP)::SelectObject(m_hDC, m_hbmp);

#if defined(__16BIT_COLOR555__) || defined(__16BIT_COLOR565__)
		int stride=((m_rcUpdate.Width()*2+3)/4)*4; 
#else
		int stride=((m_rcUpdate.Width()*4+3)/4)*4; 
#endif
		m_rbuf.attach(
			(unsigned char*)m_buf,
			m_rcUpdate.Width(),
			m_rcUpdate.Height(),
			-stride); // Use negative stride for Y-axis going down.

		FillSolidRect(0,0,m_rcUpdate.Width(),m_rcUpdate.Height(),GetSysColor(COLOR_WINDOW));
	}
}

//-----------------------------------------------------------------------------

HBITMAP CAggMemoryDC::DetachBitmap()
{
	HBITMAP retVal = m_hbmp;
	m_hbmp = NULL;
	return retVal;
}

//*****************************************************************************

class CAggBlitMemoryDC : public CAggMemoryDC
{
	CDC& m_orgdc;

#ifdef __HAVE_DIRECTX_HEADERS__
	CDDrawSupport m_DDraw;
#endif
	bool m_ddrawUsed;

public:
	CAggBlitMemoryDC(CDC& dc, CAggFonts * fonts, HWND hWnd, CRect* rc=0, BYTE alpha=255);
	~CAggBlitMemoryDC();
};

//-----------------------------------------------------------------------------

CAggBlitMemoryDC::CAggBlitMemoryDC(
	CDC& dc, CAggFonts * fonts, HWND hWnd, CRect* rc/*=0*/, BYTE alpha/*=255*/)
	: CAggMemoryDC(dc, fonts, rc, alpha, false), m_orgdc(dc), m_ddrawUsed(false) 
{
#ifdef __PROFILE_DIRECTX__
	BEGIN_PROF(_T("CONSTRUCTOR ENTRY TILL DESTRUCTOR EXIT"));

	if(ddrawprofilingtoggle && alpha==255) // not handling alpha with DDRaw yet
	{
		BEGIN_PROF(_T("DDRAW BLIT PREPARATION"));
#else
	if(alpha==255) 
	{
#endif
		if(false) //m_DDraw.IsAvailable()) //***NOTE: FOR NOW DDRAW IS NOT HELPING SPEED****
		{
#ifdef __HAVE_DIRECTX_HEADERS__
			try
			{
				int pitch;
				m_buf=m_DDraw.GetSurface(hWnd, m_rcUpdate.Width(), m_rcUpdate.Height(), pitch);
				if(m_buf)
				{
					m_rbuf.attach((agg::int8u*)m_buf, m_rcUpdate.Width(), m_rcUpdate.Height(), pitch); 
					m_hDC=m_DDraw.GetDC();
					m_ddrawUsed=true;
				}
				else
				{
					CreateCompatibleDC(dc);	// still need a dc
				}
			}
			catch(TCHAR*)
			{
				PrepareBufferMemory(dc);
			}
#endif
		}
		else
		{
			PrepareBufferMemory(dc);
		}
	}
	else
	{
#ifdef __PROFILE_DIRECTX__
		BEGIN_PROF(_T("NORMAL BLIT PREPARATION"));
#endif
		PrepareBufferMemory(dc);
	}

#ifdef __PROFILE_DIRECTX__
	END_PROF();
#endif

	if(m_rcUpdate.left!=0 || m_rcUpdate.top!=0)
	{
		m_mtx *= agg::trans_affine_translation(-m_rcUpdate.left, -m_rcUpdate.top);
		SetViewportOrg(-m_rcUpdate.left, -m_rcUpdate.top);
	}
}

//-----------------------------------------------------------------------------

CAggBlitMemoryDC::~CAggBlitMemoryDC()
{
	if(m_buf==0)
		return;

	if(m_Alpha<255)
	{
#ifndef _WIN32_WCE
		BLENDFUNCTION bf;

		bf.BlendOp = AC_SRC_OVER;
		bf.BlendFlags = 0;
		bf.SourceConstantAlpha = m_Alpha;
		bf.AlphaFormat = 0; //AC_SRC_ALPHA;
		BOOL bResult=m_orgdc.AlphaBlend(
			m_rcUpdate.left,      
			m_rcUpdate.top,      
			m_rcUpdate.Width(),  
			m_rcUpdate.Height(), 
			m_hDC,
			m_rcUpdate.left,      
			m_rcUpdate.top,      
			m_rcUpdate.Width(),  
			m_rcUpdate.Height(), 
			bf);
		ATLASSERT(bResult);
#endif
	}
	else
	{
#ifdef __PROFILE_DIRECTX__
		if(ddrawprofilingtoggle && m_ddrawUsed)
		{
			BEGIN_PROF(_T("DDRAW BLIT"));
#else
		if(m_ddrawUsed)
		{
#endif
#ifdef __HAVE_DIRECTX_HEADERS__
			try
			{
				m_DDraw.ReleaseDC(m_hDC);
				m_DDraw.Blt(&m_rcUpdate);
			}
			catch(TCHAR*)
			{
			}
#endif
		}
		else
		{
#ifdef __PROFILE_DIRECTX__
			BEGIN_PROF(_T("NORMAL BLIT"));
#endif
			m_orgdc.BitBlt(
				m_rcUpdate.left,      
				m_rcUpdate.top,      
				m_rcUpdate.Width(),  
				m_rcUpdate.Height(), 
				m_hDC,
				m_rcUpdate.left,
				m_rcUpdate.top,     
				SRCCOPY);
		}

#ifdef __PROFILE_DIRECTX__
		END_PROF();
		ddrawprofilingtoggle=!ddrawprofilingtoggle;
#endif
	}

#ifdef __PROFILE_DIRECTX__
	END_PROF();
	Profile.dumpprint(0);
	Profile.reset();
#endif

}

//*****************************************************************************

CAggDC* CAggDC::CreateBlitMemoryDC(
	CDC& dc, CAggFonts * fonts, HWND hWnd, CRect* rc/*=0*/, BYTE alpha/*=255*/)
{
	CAggDC* aggdc=new CAggBlitMemoryDC(dc, fonts, hWnd, rc, alpha);
	return aggdc;
}

