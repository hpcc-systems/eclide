#include "stdafx.h"
#include "resource.h"

#include "AggDC.h"

#include "agg_renderer_outline_aa.h"

// these statics are ok for multiple instances
//bool CAggDC::s_bInited;
//agg::line_profile_aa CAggDC::s_lineprof;

using namespace GraphTypes;

//*****************************************************************************
// 
//	CAggDC support classes
//

template <class Rasterizer, class Path>
void AddPathWithArrowHead(
	Rasterizer& ras,
	Path& path,
	double length,
	double halfwidth,
	double inset,
	double scale=1.0)
{
    double k = ::pow(path.width(), 0.7)*scale;

	path.shorten(k*(length-inset));
	ras.add_path(path);

    agg::arrowhead ah;
    ah.head(0, (length-inset) * k, halfwidth * k, inset * k);
    agg::conv_marker<agg::vcgen_markers_term, agg::arrowhead> arrow(path.markers(), ah);
    ras.add_path(arrow);
}

//*****************************************************************************
//
//		curve_casteljau is result of work with Agg's author Maxim Shemanarev.
//		Not optimized or official part of Agg yet.
//
//*****************************************************************************

namespace agg
{
    //------------------------------------------------------------------------
    class curve_casteljau
    {
    public:
        curve_casteljau();

        void approximation_scale(double s) { m_scale = s; }
        double approximation_scale() const { return m_scale;  }

        void remove_all();
        void add_vertex(double x, double y);

        void rewind(unsigned);
        unsigned vertex(double* x, double* y);

        typedef curve_casteljau source_type;

    private:
        double                   m_scale;
        double                   m_length;
        pod_deque<point_type, 6> m_vertices;
        pod_deque<point_type, 6> m_tree;
        double                   m_mu;
        double                   m_dmu;
        int                      m_num_steps;
        int                      m_step;
    };


    //------------------------------------------------------------------------
    curve_casteljau::curve_casteljau() : 
        m_scale(1.0), m_length(0.0), m_num_steps(0), m_step(0) {}


    //------------------------------------------------------------------------
    void curve_casteljau::remove_all()
    {
        m_vertices.remove_all();
    }


    //------------------------------------------------------------------------
    void curve_casteljau::add_vertex(double x, double y)
    {
        if(m_vertices.size())
        {
            const point_type& p = m_vertices.last();
            m_length += calc_distance(x, y, p.x, p.y);
        }
        m_vertices.add(point_type(x, y));
    }


    //------------------------------------------------------------------------
    void curve_casteljau::rewind(unsigned)
    {
        if(m_vertices.size() < 3) 
        {
            m_step = -1;
            return;
        }
        m_num_steps = int(m_length * 0.25 * m_scale);
        if(m_num_steps < 2)
        {
            m_num_steps = 2;   
        }

        m_step = m_num_steps;
        m_mu = 0.0;
        m_dmu = 1.0 / (m_num_steps - 1);
    }


    //------------------------------------------------------------------------
    unsigned curve_casteljau::vertex(double* x, double* y)
    {
        if(m_step < 0) return path_cmd_stop;
        if(m_step == m_num_steps)
        {
            *x = m_vertices[0].x;
            *y = m_vertices[0].y;
            --m_step;
            return path_cmd_move_to;
        }

        if(m_step == 0)
        {
            *x = m_vertices.last().x;
            *y = m_vertices.last().y;
            --m_step;
            return path_cmd_line_to;
        }

        m_tree = m_vertices;
        for(unsigned n = m_tree.size(); n > 1; n--)
        {
		    for(unsigned i = 0; i < n - 1; i++)
		    {
                point_type& p1 = m_tree[i];
                const point_type& p2 = m_tree[i + 1];
                p1.x += (p2.x - p1.x) * m_mu;
                p1.y += (p2.y - p1.y) * m_mu;
		    }
        }

        *x = m_tree[0].x;
        *y = m_tree[0].y;

        --m_step;
        m_mu += m_dmu;
        return path_cmd_line_to;
    }

} // namespace agg

//*****************************************************************************
//
//	And now .... AggDC itself
//

CAggDC::CAggDC(CDC& dc, CCanvas* canvas, CRect* rc/*=0*/, BYTE alpha/*=255*/)
	: m_hbmp(0), m_buf(0), m_orgdc(dc), m_Alpha(alpha)
{
	m_canvas=canvas;

	OneTimeInit(this);

	if(rc==0)
		dc.GetClipBox(&m_rcUpdate);
	else
		m_rcUpdate=*rc;	

	CreateCompatibleDC(dc);

	CreateAggBitmap(m_rcUpdate.Width(), m_rcUpdate.Height(), &m_buf);

	m_oldhbmp=(HBITMAP)::SelectObject(m_hDC, m_hbmp);

/*
	RECT bkrc;
	bkrc.left=0;
	bkrc.top=0;
	bkrc.right=m_rcUpdate.Width();
	bkrc.bottom=m_rcUpdate.Height();
	::SetBkColor(m_hDC, GetSysColor(COLOR_WINDOW));
	::ExtTextOut(m_hDC, 0, 0, ETO_OPAQUE, &bkrc, NULL, 0, NULL);
*/
	FillSolidRect(0,0,m_rcUpdate.Width(),m_rcUpdate.Height(),GetSysColor(COLOR_WINDOW));

#ifdef _DEBUG_MAPPING
	CPoint tvo;
	GetViewportOrg(&tvo);
	CSize tvex;
	GetViewportExt(&tvex);
	CPoint two;
	GetWindowOrg(&two);
	CSize twex;
	GetWindowExt(&twex);

	CPoint ovo;
	m_orgdc.GetViewportOrg(&ovo);
	CSize ovex;
	m_orgdc.GetViewportExt(&ovex);
	CPoint owo;
	m_orgdc.GetWindowOrg(&owo);
	CSize owex;
	m_orgdc.GetWindowExt(&owex);
#endif
}

//-----------------------------------------------------------------------------

CAggDC::~CAggDC()
{
	if(m_Alpha<255)
	{
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
			0,      
			0,      
			m_rcUpdate.Width(),  
			m_rcUpdate.Height(), 
			bf);
	}
	else
		m_orgdc.BitBlt(
			m_rcUpdate.left,      
			m_rcUpdate.top,      
			m_rcUpdate.Width(),  
			m_rcUpdate.Height(), 
			m_hDC,
			0,
			0,     
			SRCCOPY);

	::SelectObject(m_hDC, m_oldhbmp);
	if(m_hbmp)
		::DeleteObject(m_hbmp);
}


//-----------------------------------------------------------------------------
void CAggDC::Line(
	const PointF& p1,
	const PointF& p2, 
	const Color& clr)
{
	agg::rendering_buffer rbuf;

	rbuf.attach(
		(unsigned char*)m_buf,
		m_rcUpdate.Width(),
		m_rcUpdate.Height(),
		-m_rcUpdate.Width()*4); // Use negative stride for Y-axis going down.

	pfbgra pixf(rbuf);
	ren_base renb(pixf);

	outline_renderer outline_rd(renb, s_lineprof);
	outline_rasterizer_aa outline_rt(outline_rd);

	outline_rd.profile(s_lineprof);
	outline_rd.color(agg::rgba8(clr.GetR(), clr.GetG(), clr.GetB(), clr.GetA()));

	outline_rt.move_to_d(p1.x, p1.y);
	outline_rt.line_to_d(p2.x, p2.y);
	//outline_rt.round_cap( 1 );
	outline_rt.render(false);
}

//-----------------------------------------------------------------------------
// 
// Multiple (may or may not overlap, be connected or whatever) lines. 
//

void CAggDC::Lines(
	const PointFVector& pts,
	const Color& clr,
	const REAL width/*=1.0*/)
{
	unsigned count=pts.size()/2;
	ATLASSERT(count>0);
	ATLASSERT(count/2);

	agg::rendering_buffer rbuf;
	rbuf.attach(
		(unsigned char*)m_buf,
		m_rcUpdate.Width(),
		m_rcUpdate.Height(),
		-m_rcUpdate.Width()*4); // Use negative stride for Y-axis going down.

	agg::pixfmt_bgra32 pixf(rbuf);
	ren_base renb(pixf);
	solid_renderer ren_solid(renb);

	m_path.remove_all();

	for(unsigned i=0; i<count; ++i)
	{
		m_path.move_to(pts[i*2].x, pts[i*2].y);
		m_path.line_to(pts[i*2+1].x, pts[i*2+1].y);
	}

	typedef agg::conv_stroke<agg::path_storage> conv_stroke_outline;
	conv_stroke_outline stroke(m_path);
	stroke.width(width);

	m_ras_aa.add_path(stroke);

	ren_solid.color(agg::rgba8(clr.GetR(), clr.GetG(), clr.GetB(), clr.GetA()));
	agg::render_scanlines(m_ras_aa, m_sl, ren_solid);
}

//-----------------------------------------------------------------------------

void CAggDC::Triangle(
	const PointF& p1, 
	const PointF& p2, 
	const PointF& p3, 
	const Color& clr)
{
	agg::rendering_buffer rbuf;

	rbuf.attach(
		(unsigned char*)m_buf,
		m_rcUpdate.Width(),
		m_rcUpdate.Height(),
		-m_rcUpdate.Width()*4); // Use negative stride for Y-axis going down.

	agg::pixfmt_bgra32 pixf(rbuf);
	ren_base renb(pixf);
	solid_renderer ren_solid(renb);

	m_path.remove_all();
	m_path.move_to(p1.x, p1.y);
	m_path.line_to(p2.x, p2.y);
	m_path.line_to(p3.x, p3.y);

	m_ras_aa.reset();
	m_ras_aa.add_path(m_path);

	ren_solid.color(agg::rgba8(clr.GetR(), clr.GetG(), clr.GetB(), clr.GetA()));

	agg::render_scanlines(m_ras_aa, m_sl, ren_solid);
}

//-----------------------------------------------------------------------------

void CAggDC::Quadrangle(
	const PointF& p1, 
	const PointF& p2, 
	const PointF& p3, 
	const PointF& p4, 
	const Color& clr)
{
	agg::rendering_buffer rbuf;

	rbuf.attach(
		(unsigned char*)m_buf,
		m_rcUpdate.Width(),
		m_rcUpdate.Height(),
		-m_rcUpdate.Width()*4); // Use negative stride for Y-axis going down.

	agg::pixfmt_bgra32 pixf(rbuf);
	ren_base renb(pixf);
	solid_renderer ren_solid(renb);

	m_path.remove_all();
	m_path.move_to(p1.x, p1.y);
	m_path.line_to(p2.x, p2.y);
	m_path.line_to(p3.x, p3.y);
	m_path.line_to(p4.x, p4.y);

	m_ras_aa.reset();
	m_ras_aa.add_path(m_path);

	ren_solid.color(agg::rgba8(clr.GetR(), clr.GetG(), clr.GetB(), clr.GetA()));

	agg::render_scanlines(m_ras_aa, m_sl, ren_solid);
}

//-----------------------------------------------------------------------------

void CAggDC::Rect(
	const GraphTypes::PointF& tl, 
	const GraphTypes::PointF& br,
	const GraphTypes::Color& clrfill,
	const GraphTypes::Color& clroutline,
	GraphTypes::REAL outline_width/*=1.0*/,
	bool bAA/*=false*/)
{
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
		agg::rendering_buffer rbuf;

		rbuf.attach(
			(unsigned char*)m_buf,
			m_rcUpdate.Width(),
			m_rcUpdate.Height(),
			-m_rcUpdate.Width()*4);

		agg::pixfmt_bgra32 pixf(rbuf);
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

		int x1=static_cast<int>(tl.x); 
		int y1=static_cast<int>(tl.y); 
		int x2=static_cast<int>(br.x); 
		int y2=static_cast<int>(br.y); 

		pren.outlined_rectangle(x1, y1, x2, y2); 
	}
}

//-----------------------------------------------------------------------------

void CAggDC::SolidPolygon(
	const PointF* points, 
	const Color& clr,
	unsigned num_points)
{
	agg::rendering_buffer rbuf;

	rbuf.attach(
		(unsigned char*)m_buf,
		m_rcUpdate.Width(),
		m_rcUpdate.Height(),
		-m_rcUpdate.Width()*4); // Use negative stride for Y-axis going down.

	agg::pixfmt_bgra32 pixf(rbuf);
	ren_base renb(pixf);
	solid_renderer ren_solid(renb);

	m_path.remove_all();

	ATLASSERT(num_points>0);

	m_path.move_to(points->x, points->y);
	while(--num_points>0)
	{
		points++;
		m_path.line_to(points->x, points->y);
	}

	m_ras_aa.reset();
	m_ras_aa.add_path(m_path);

	ren_solid.color(agg::rgba8(clr.GetR(), clr.GetG(), clr.GetB(), clr.GetA()));
	agg::render_scanlines(m_ras_aa, m_sl, ren_solid);
}

//-----------------------------------------------------------------------------

void CAggDC::SolidPolygon(
	const PointF* points, 
	const Color& clr,
	unsigned num_points,
	REAL widthoutline,
	const Color& clroutline )
{
	agg::rendering_buffer rbuf;

	ATLASSERT(num_points>0);

	rbuf.attach(
		(unsigned char*)m_buf,
		m_rcUpdate.Width(),
		m_rcUpdate.Height(),
		-m_rcUpdate.Width()*4); // Use negative stride for Y-axis going down.

	agg::pixfmt_bgra32 pixf(rbuf);
	ren_base renb(pixf);

	agg::line_profile_aa lnprof;
	lnprof.width(widthoutline);

	solid_renderer ren_solid(renb);
	outline_renderer outline_rd(renb, lnprof);

	outline_rasterizer_aa outline_rt(outline_rd);
	outline_rd.color(agg::rgba8(clroutline.GetR(),
	clroutline.GetG(), clroutline.GetB(), clroutline.GetA()));

	m_ras_aa.reset();

	// add path

		/* consider changing signature to add path this way
		m_path.add_poly(const double* vertices, unsigned num, 
                      bool solid_path = false,
                      unsigned end_flags = path_flagm_none);
		*/

	m_path.remove_all();
	m_path.move_to(points->x, points->y);
	while(--num_points>0)
	{
		points++;
		m_path.line_to(points->x, points->y);
	}
	m_path.close_polygon();

	m_ras_aa.add_path(m_path);
	outline_rt.add_path(m_path);

	ren_solid.color(agg::rgba8(clr.GetR(), clr.GetG(), clr.GetB(), clr.GetA()));
	agg::render_scanlines(m_ras_aa, m_sl, ren_solid);
	outline_rt.render(true);

	//  code below is if using strokes instead of outline rasterizer for outline
	//	typedef agg::conv_stroke<agg::path_storage> conv_stroke_outline;
	//	conv_stroke_outline stroke(m_path);
	//	stroke.width(widthoutline);

	//	m_ras_aa.add_path(stroke);
	//	ren_solid.color(agg::rgba8(clroutline.GetR(),
	//		clroutline.GetG(), clroutline.GetB(), clroutline.GetA()));
	//	agg::render_scanlines(m_ras_aa, m_sl, ren_solid);

}

//-----------------------------------------------------------------------------
//
//	Bezier defined by 4 control points

void CAggDC::Bezier4(
		const GraphTypes::PointF& p1,
		const GraphTypes::PointF& p2,
		const GraphTypes::PointF& p3,
		const GraphTypes::PointF& p4,
		const GraphTypes::Color& clr,
		double width/*=1.0*/,
		bool bArrow/*=true*/,
		GraphTypes::REAL scale/*=1.0*/)
{
	agg::rendering_buffer rbuf;
	rbuf.attach((unsigned char*)m_buf,
		m_rcUpdate.Width(),
		m_rcUpdate.Height(),
		-m_rcUpdate.Width()*4); 
	agg::pixfmt_bgra32 pixf(rbuf);
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
		AddPathWithArrowHead(m_ras_aa, stroke, 10, 3, 2, scale);
	else
		m_ras_aa.add_path(stroke);

	ren_solid.color(agg::rgba8(clr.GetR(), clr.GetG(), clr.GetB(), clr.GetA()));
	agg::render_scanlines(m_ras_aa, m_sl, ren_solid);
}

//-----------------------------------------------------------------------------
//
// (num_points-1) degree Bezier drawn using de Casteljau subdivision
#define __SHOW_CPS__

void CAggDC::Bezier(
	const double* points,
	unsigned num_points,
	const Color& clr,
	double width/*=2.0*/,
	bool bArrow/*=true*/,
	REAL scale/*=1.0*/)
{
	agg::rendering_buffer rbuf;

	rbuf.attach(
		(unsigned char*)m_buf,
		m_rcUpdate.Width(),
		m_rcUpdate.Height(),
		-m_rcUpdate.Width()*4); // Use negative stride for Y-axis going down.

	agg::pixfmt_bgra32 pixf(rbuf);
	ren_base renb(pixf);
	solid_renderer ren_solid(renb);

    agg::curve_casteljau curve;
    //curve.approximation_scale(1.0);
	for(unsigned i=0; i<num_points; i++)
	{
#ifdef __SHOW_CPS__
		agg::ellipse el(*points, *(points+1), 4, 4, 16);
		m_ras_aa.add_path(el);
#endif
        curve.add_vertex(*points, *(points+1));
		points+=2;
    }

#ifdef __SHOW_CPS__
	ren_solid.color(agg::rgba8(0, 126, 0, 255));
	agg::render_scanlines(m_ras_aa, m_sl, ren_solid);
#endif

    typedef agg::conv_stroke<agg::curve_casteljau, agg::vcgen_markers_term> conv_stroke_type;
    conv_stroke_type stroke(curve);
	stroke.width(width);

	if(bArrow)
		AddPathWithArrowHead(m_ras_aa, stroke, 10, 3, 2, scale);
	else
		m_ras_aa.add_path(stroke);

	ren_solid.color(agg::rgba8(clr.GetR(), clr.GetG(), clr.GetB(), clr.GetA()));
	agg::render_scanlines(m_ras_aa, m_sl, ren_solid);
}

//-----------------------------------------------------------------------------
//
// Uniform Bspline

void CAggDC::InterpolatingSpline(
	const double* polygon,
	unsigned num_points,
	const Color& clr,
	bool bArrow/*=false*/)
{
	agg::rendering_buffer rbuf;

	rbuf.attach(
		(unsigned char*)m_buf,
		m_rcUpdate.Width(),
		m_rcUpdate.Height(),
		-m_rcUpdate.Width()*4); // Use negative stride for Y-axis going down.

	agg::pixfmt_bgra32 pixf(rbuf);
	ren_base renb(pixf);
	solid_renderer ren_solid(renb);

	agg::simple_polygon_vertex_source path(polygon, 
		num_points, 
		false, 
		false);

	typedef agg::conv_bspline<agg::simple_polygon_vertex_source> conv_bspline_type;
	conv_bspline_type bspline(path);
	bspline.interpolation_step(1.0 / num_points);

	typedef agg::conv_stroke<conv_bspline_type, agg::vcgen_markers_term > conv_stroke_type;
	conv_stroke_type stroke(bspline);

	stroke.width(1.0);
	m_ras_aa.reset();

	if(bArrow)
		AddPathWithArrowHead(m_ras_aa, stroke, 10, 3, 2);
	else
		m_ras_aa.add_path(stroke);

	ren_solid.color(agg::rgba8(clr.GetR(), clr.GetG(), clr.GetB(), clr.GetA()));
	agg::render_scanlines(m_ras_aa, m_sl, ren_solid);
}

//-----------------------------------------------------------------------------

void CAggDC::SmoothCurve(
   const PointFVector& points, 
   const Color& clr,
   bool bArrow/*=false*/)
{
	unsigned size=points.size();
	agg::rendering_buffer rbuf;

	rbuf.attach(
		(unsigned char*)m_buf,
		m_rcUpdate.Width(),
		m_rcUpdate.Height(),
		-m_rcUpdate.Width()*4); // Use negative stride for Y-axis going down.

	agg::pixfmt_bgra32 pixf(rbuf);
	ren_base renb(pixf);
	solid_renderer ren_solid(renb);

	m_path.remove_all();
	m_path.move_to(points[0].x, points[0].y);

	for(unsigned i=1; i<size; ++i)
	{
		m_path.line_to(points[i].x, points[i].y);
	}

	typedef agg::conv_smooth_poly1<agg::path_storage> conv_smooth_type;
	typedef agg::conv_curve<conv_smooth_type> conv_curve_type;
	conv_smooth_type smooth(m_path);
	conv_curve_type curve(smooth);
	smooth.smooth_value(1.0);
	typedef agg::conv_stroke<conv_curve_type, agg::vcgen_markers_term > conv_stroke_type2;
	conv_stroke_type2 stroke(curve);

	stroke.width(1.0);
	m_ras_aa.reset();

	if(bArrow)
		AddPathWithArrowHead(m_ras_aa, stroke, 10, 3, 2);
	else
		m_ras_aa.add_path(stroke);

	ren_solid.color(agg::rgba8(clr.GetR(), clr.GetG(), clr.GetB(), clr.GetA()));
	agg::render_scanlines(m_ras_aa, m_sl, ren_solid);
}

//-----------------------------------------------------------------------------

void CAggDC::LollyPop(
	const GraphTypes::RectF& rc, 
	const GraphTypes::Color& clr,
	const GraphTypes::Color& clroutline,
	bool bLeft)
{
	agg::rendering_buffer rbuf;
	agg::line_profile_aa lnprof;
	lnprof.width(1.0);

	rbuf.attach(
		(unsigned char*)m_buf,
		m_rcUpdate.Width(),
		m_rcUpdate.Height(),
		-m_rcUpdate.Width()*4); // Use negative stride for Y-axis going down.

	agg::pixfmt_bgra32 pixf(rbuf);
	ren_base renb(pixf);

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

	m_ras_aa.add_path(circle);

	outline_rt.add_path(circle);
	outline_rt.add_path(m_path);

	ren_solid.color(agg::rgba8(clr.GetR(), clr.GetG(), clr.GetB(), clr.GetA()));
	agg::render_scanlines(m_ras_aa, m_sl, ren_solid);
	outline_rt.render(true);
}

//-----------------------------------------------------------------------------

void CAggDC::OneTimeInit(CAggDC* myself)
{
	if(s_bInited)
		return;

	s_lineprof.width(1.0);

	s_bInited=true;
}

//-----------------------------------------------------------------------------

bool CAggDC::CreateAggBitmap(int width, int height, void** ppbuffer)
{
	*ppbuffer = 0; 

	BITMAPINFO bmp_info; 
	bmp_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
	bmp_info.bmiHeader.biWidth = width; 
	bmp_info.bmiHeader.biHeight = height; 
	bmp_info.bmiHeader.biPlanes = 1; 
	bmp_info.bmiHeader.biBitCount = 32; 
	bmp_info.bmiHeader.biCompression = BI_RGB; 
	bmp_info.bmiHeader.biSizeImage = 0; 
	bmp_info.bmiHeader.biXPelsPerMeter = 0; 
	bmp_info.bmiHeader.biYPelsPerMeter = 0; 
	bmp_info.bmiHeader.biClrUsed = 0; 
	bmp_info.bmiHeader.biClrImportant = 0; 

	m_hbmp = ::CreateDIBSection( 
		m_hDC, 
		&bmp_info, 
		DIB_RGB_COLORS, 
		ppbuffer, 
		0, 
		0 
		); 

	return m_hbmp!=0;
}
