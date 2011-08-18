#include "StdAfx.h"
#include "resource.h"

#include "aggrenderer.h"
#include "graphtypes.h"

using namespace GraphTypes;

#define TEXT_PAD (3 * GetScale())
#define LINE_WIDTH (1 * GetScale())
enum MYCOLOR
{
    COLOR_FILL, 
	COLOR_BORDER, 
	COLOR_LINE,
	COLOR_TEXT
};

//=============================================================================
typedef agg::pixfmt_bgr24 pixfmt;
typedef agg::renderer_base<pixfmt> base_renderer;
typedef agg::renderer_primitives<base_renderer> primitives_renderer;

typedef agg::renderer_scanline_aa_solid<base_renderer>   solid_renderer;
typedef agg::renderer_scanline_bin_solid<base_renderer> draft_renderer;

typedef agg::gradient_circle gradient_function;
typedef agg::span_interpolator_linear<> interpolator;
typedef agg::span_gradient<agg::rgba8, interpolator, gradient_function, const agg::rgba8*> gradient_span_gen;
typedef agg::span_allocator<agg::rgba8> gradient_span_alloc;

typedef agg::renderer_scanline_aa<base_renderer, gradient_span_gen> gradient_renderer;

typedef agg::rasterizer_scanline_aa<> scanline_rasterizer;
typedef agg::rasterizer_outline<primitives_renderer> outline_rasterizer;
//=============================================================================
struct line
{
    double x1, y1, x2, y2;
    int f;

    line(double x1_, double y1_, double x2_, double y2_) : x1(x1_), y1(y1_), x2(x2_), y2(y2_), f(0) 
	{
	}
    void rewind(unsigned) 
	{ 
		f = 0; 
	}
    unsigned vertex(double* x, double* y)
    {
        if(f == 0) 
		{ 
			++f; 
			*x = x1; 
			*y = y1; 
			return agg::path_cmd_move_to; 
		}
        if(f == 1) 
		{ 
			++f; 
			*x = x2; 
			*y = y2; 
			return agg::path_cmd_line_to; 
		}
        return agg::path_cmd_stop;
    }
};
//=============================================================================
struct curve
{
    agg::curve4 c;

    curve(double x1, double y1, double x2, double y2, double k=0.5) : c(x1, y1, x1 - (y2 - y1) * k, y1 + (x2 - x1) * k, x2 + (y2 - y1) * k, y2 - (x2 - x1) * k, x2, y2)
    {
    }

    void rewind(unsigned id) 
	{ 
		c.rewind(id); 
	}
    unsigned vertex(double* x, double* y) 
	{ 
		return c.vertex(x, y); 
	}
};
//=============================================================================
template<class Source> struct stroke_draft_simple
{
    Source& s;
    stroke_draft_simple(Source& src, double w) : s(src)
    {
    }

    void rewind(unsigned id) 
	{ 
		s.rewind(id); 
	}
    unsigned vertex(double* x, double* y) 
	{ 
		return s.vertex(x, y); 
	}
};
//=============================================================================
template<class Source> struct stroke_draft_arrow
{
    typedef agg::conv_marker_adaptor<Source, agg::vcgen_markers_term> stroke_type;
    typedef agg::conv_marker<typename stroke_type::marker_type, agg::arrowhead> marker_type;
    typedef agg::conv_concat<stroke_type, marker_type> concat_type;

    stroke_type s;
    agg::arrowhead ah;
    marker_type m;
    concat_type c;

    stroke_draft_arrow(Source& src, double w) : 
		s(src),
        ah(),
        m(s.markers(), ah),
        c(s, m)
    {
        ah.head(0, 10, 5, 0);
        s.shorten(10.0);
    }

    void rewind(unsigned id) 
	{ 
		c.rewind(id); 
	}
    unsigned vertex(double* x, double* y) 
	{ 
		return c.vertex(x, y); 
	}
};
//=============================================================================
template<class Source> struct stroke_fine_simple
{
    typedef agg::conv_stroke<Source> stroke_type;

    stroke_type s;

    stroke_fine_simple(Source& src, double w) : s(src)
    {
        s.width(w); 
    }
    void rewind(unsigned id) 
	{ 
		s.rewind(id); 
	}
    unsigned vertex(double* x, double* y) 
	{ 
		return s.vertex(x, y); 
	}
};
//=============================================================================
template<class Source> struct stroke_fine_arrow
{
    typedef agg::conv_stroke<Source, agg::vcgen_markers_term> stroke_type;
    typedef agg::conv_marker<typename stroke_type::marker_type, agg::arrowhead> marker_type;
    typedef agg::conv_concat<stroke_type, marker_type> concat_type;

    stroke_type s;
    agg::arrowhead ah;
    marker_type m;
    concat_type c;

    stroke_fine_arrow(Source& src, double w) : 
        s(src),
        ah(),
        m(s.markers(), ah),
        c(s, m)
    {
        s.width(w); 
        ah.head(0, w * 10, w * 5, 0);
        s.shorten(w * 2.0);
    }

    void rewind(unsigned id) 
	{ 
		c.rewind(id); 
	}
    unsigned vertex(double* x, double* y) 
	{ 
		return c.vertex(x, y); 
	}
};
//=============================================================================
template<class Source> struct dash_stroke_draft_simple
{
    typedef agg::conv_dash<Source, agg::vcgen_markers_term> dash_type;

    dash_type d;
    
    dash_stroke_draft_simple(Source& src, double dash_len, double gap_len, double w) : d(src)
    {
        d.add_dash(dash_len, gap_len);
    }

    void rewind(unsigned id) 
	{ 
		d.rewind(id); 
	}
    unsigned vertex(double* x, double* y) 
	{ 
		return d.vertex(x, y); 
	}
};
//=============================================================================
template<class Source> struct dash_stroke_draft_arrow
{
    typedef agg::conv_dash<Source, agg::vcgen_markers_term> dash_type;
    typedef agg::conv_marker<typename dash_type::marker_type, agg::arrowhead> marker_type;
    typedef agg::conv_concat<dash_type, marker_type> concat_type;

    dash_type d;
    agg::arrowhead ah;
    marker_type m;
    concat_type c;

    dash_stroke_draft_arrow(Source& src, double dash_len, double gap_len, double w) : 
        d(src),
        ah(),
        m(d.markers(), ah),
        c(d, m)
    {
        d.add_dash(dash_len, gap_len);
        ah.head(0, 10, 5, 0);
        d.shorten(10.0);
    }

    void rewind(unsigned id) 
	{ 
		c.rewind(id); 
	}
    unsigned vertex(double* x, double* y) 
	{ 
		return c.vertex(x, y); 
	}
};
//=============================================================================
template<class Source> struct dash_stroke_fine_simple
{
    typedef agg::conv_dash<Source> dash_type;
    typedef agg::conv_stroke<dash_type> stroke_type;

    dash_type d;
    stroke_type s;

    dash_stroke_fine_simple(Source& src, double dash_len, double gap_len, double w) : 
        d(src),
        s(d)
    {
        d.add_dash(dash_len, gap_len);
        s.width(w);
    }

    void rewind(unsigned id) 
	{ 
		s.rewind(id); 
	}
    unsigned vertex(double* x, double* y) 
	{ 
		return s.vertex(x, y); 
	}
};
//=============================================================================
template<class Source> struct dash_stroke_fine_arrow
{
    typedef agg::conv_dash<Source, agg::vcgen_markers_term> dash_type;
    typedef agg::conv_stroke<dash_type> stroke_type;
    typedef agg::conv_marker<typename dash_type::marker_type, agg::arrowhead> marker_type;
    typedef agg::conv_concat<stroke_type, marker_type> concat_type;

    dash_type d;
    stroke_type s;
    agg::arrowhead ah;
    marker_type m;
    concat_type c;

    dash_stroke_fine_arrow(Source& src, double dash_len, double gap_len, double w) : 
        d(src),
        s(d),
        ah(),
        m(d.markers(), ah),
        c(s, m)
    {
        d.add_dash(dash_len, gap_len);
        s.width(w);
        ah.head(0, 10, 5, 0);
        d.shorten(w * 2.0);
    }

    void rewind(unsigned id) 
	{ 
		c.rewind(id); 
	}
    unsigned vertex(double* x, double* y) 
	{ 
		return c.vertex(x, y); 
	}
};
//=============================================================================
//#define stroke_draft      stroke_draft_simple
//#define dash_stroke_draft dash_stroke_draft_simple
//#define stroke_fine       stroke_fine_simple
//#define dash_stroke_fine  dash_stroke_fine_simple

#define stroke_draft      stroke_draft_arrow
#define dash_stroke_draft dash_stroke_draft_arrow
#define stroke_fine       stroke_fine_arrow
#define dash_stroke_fine  dash_stroke_fine_arrow
//=============================================================================
class CAggRenderImpl : public CRender
{
protected:
	CDCHandle *m_dc;

public:
	CAggRenderImpl(CGraphData &graph);

	void render(CDCHandle &dc, const GraphTypes::PointF &origin, Path &overToSel, vertex_descriptor overNode, vertex_descriptor focusNode, bool calculateExtent);
	GraphTypes::PointF getCenter(const GraphTypes::RectF &r);
	CString & getText(const vertex_descriptor &v, CString & txt);
	RECT paintNormVertex(CDCHandle &dc, const vertex_descriptor &v, const GraphTypes::RectF &r, const Color &fontColor);
	RECT paintSelVertex(CDCHandle &dc, const vertex_descriptor &v, const GraphTypes::RectF &r, const Color &fontColor);
	RECT paintOverVertex(CDCHandle &dc, const vertex_descriptor &v, const GraphTypes::RectF &r, const Color &fontColor);
	RECT paintVertex(CDCHandle &dc, const vertex_descriptor &v, const GraphTypes::RectF &r, const Color &fontColor, const Color &fill, const Color &border);
	void paintArrow(CDCHandle &dc, const edge_descriptor &e, GraphTypes::PointF p1, GraphTypes::PointF p2, const CPen &p);
	RECT toRect(const GraphTypes::RectF &p);
	void adjustOffsets(GraphTypes::RectF& rOut, const GraphTypes::RectF& rIn, PLACEMENT placement, bool isIPicture);
	void drawIcon(CDCHandle& dc, const vertex_descriptor& v, const RectF& r);
	void drawImage(CDCHandle& dc, const vertex_descriptor& v, const RectF& r);
	void drawBitmap(CDCHandle& dc, const vertex_descriptor& v, const RectF& r, IPicture* image, HTMLElement* element, const PLACEMENT& placement);
	void drawLines(CDCHandle& dc);
	HPEN selectBorderPen(CDCHandle& dc, const vertex_descriptor& v);
	void drawVertices(CDCHandle& dc);
	void createFont(CDCHandle &dc, CFont &font, int size);
	GraphTypes::RectF calcTextRect(const vertex_descriptor &v, bool recalc = false);
	RECT calcTextBounds(const vertex_descriptor &v);

	void create_pmap(unsigned width, unsigned height, agg::rendering_buffer* wnd);
	void display_pmap(HDC dc, const agg::rendering_buffer* src);

    void on_draw();
	void draw_scene(scanline_rasterizer& ras, solid_renderer& solid, draft_renderer& draft);
	template<typename Renderer>
	void draw_lines(scanline_rasterizer& ras, Renderer& renderer)
	{
		CComPtr<clib::scoped_lock_ref_counted> lock;
		for (StlLinked<IEdgeIterator> e = m_graph.getVisibleEdgeIterator(lock); e->IsValidItem(); e->NextItem())
		{
			vertex_descriptor from = m_graph.getEdgeSource(e->Get());
			vertex_descriptor to = m_graph.getEdgeTarget(e->Get());

			GraphTypes::PointF n1 = m_graph.getVertexPoint(from);
			GraphTypes::PointF n2 = m_graph.getVertexPoint(to);
			GraphTypes::RectF r1 = calcTextRect(from);
			GraphTypes::RectF r2 = calcTextRect(to);
			int cat = m_graph.getEdgeCat(e->Get()).GetCategory();

			AllTransform(n1);
			AllTransform(n2);

			GraphTypes::PointF p1, p2;

			if (m_draft)
			{
				line l(n1.x, n1.y, n2.x, n2.y);

				stroke_fine_simple<line> s(l, LINE_WIDTH);
				ras.add_path(s);
				renderer.color(m_color_profile[cat][COLOR_LINE]);
				//ras.render(m_sl, renderer);
				agg::render_scanlines(ras, m_sl, renderer);
			}
			else 
			{
				agg::path_storage ps;
				ps.remove_all();
				int tot = m_graph.getEdgePointsCount(e->Get());
				if (tot)
				{
					GraphTypes::PointF p;
					for(int i = 0; i < tot; ++i)
					{
						p = m_graph.getEdgePoint(e->Get(), i);
						AllTransform(p);
						if (i == 0)
						{
							if (Intersect_Lines(r1, n1, p, p1))
								ps.move_to(p1.x, p1.y);
							else
								ps.move_to(p.x, p.y);
						}
						else
							ps.line_to(p.x, p.y);
					}
					if (Intersect_Lines(r2, p, n2, p2))
						ps.line_to(p2.x, p2.y);
				}
				else if(Intersect_Lines(r1, n1, n2, p1) && Intersect_Lines(r2, n1, n2, p2))
				{
					ps.move_to(p1.x, p1.y);
					ps.line_to(p2.x, p2.y);
				}
				/*
				else
				{
					line l(p1.x, p1.y, p2.x, p2.y);
					stroke_fine<line> s(l, LINE_WIDTH);
					ras.add_path(s);
					renderer.color(m_color_profile[cat][COLOR_LINE]);
					ras.render(m_sl, renderer);
				}
				*/

				agg::conv_smooth_poly1<agg::path_storage> smooth(ps);

				agg::conv_curve<agg::conv_smooth_poly1<agg::path_storage> > curve(smooth);

				smooth.smooth_value(1.0);

				stroke_fine<agg::conv_curve<agg::conv_smooth_poly1<agg::path_storage> > > s(curve, LINE_WIDTH);
				ras.add_path(s);
				renderer.color(m_color_profile[cat][COLOR_LINE]);
				//ras.render(m_sl, renderer);
				agg::render_scanlines(ras, m_sl, renderer);
			}
		}
	}
	template<typename Renderer>
	void draw_nodes(scanline_rasterizer& ras, Renderer& renderer)
	{
		agg::scanline_p8 sl;
		pixfmt pixf(rbuf_window());
		base_renderer rb(pixf);
		primitives_renderer prim(rb);

		CComPtr<clib::scoped_lock_ref_counted> lock;
		for (StlLinked<IVertexIterator> n = m_graph.getVisibleVertexIterator(lock); n->IsValidItem(); n->NextItem())
		{
			vertex_descriptor v = n->Get();
			int cat = m_graph.getVertexCat(v).GetCategory();
			GraphTypes::PointF p = m_graph.getVertexPoint(v);
			AllTransform(p);
			//GraphTypes::RectF r = calcTextRect(v);

			agg::trans_affine mtx;
			//mtx *= agg::trans_affine_translation(r.Width, r.Height);

			CString txt;
			getText(v, txt);
			agg::gsv_text label;
			label.flip(true);
			agg::conv_stroke<agg::gsv_text> ls(label);
			agg::conv_transform<agg::conv_stroke<agg::gsv_text> > lo(ls, mtx);
			ls.line_join(agg::round_join);
			ls.line_cap(agg::round_cap);
			//ls.approximation_scale(mtx.scale());
			ls.width(LINE_WIDTH);
			label.text(CT2A(txt, CP_UTF8));

			label.start_point(p.x, p.y);
			label.size(DEFAULT_FONT_SIZE * GetScale());

			double x;
			double y;
			double min_x = 1e100, min_y = 1e100;
			double max_x = -1e100, max_y = -1e100;
			unsigned cmd;
			label.rewind(0);

			while(!agg::is_stop(cmd = label.vertex(&x, &y)))
			{
				if(agg::is_vertex(cmd))
				{
					if(x < min_x) min_x = x;
					if(x > max_x) max_x = x;
					if(y < min_y) min_y = y;
					if(y > max_y) max_y = y;
				}
			}

			float w = static_cast<float>(max_x - min_x);
			float h = static_cast<float>(max_y - min_y);
			GraphTypes::RectF r(p.x - w / 2 - TEXT_PAD, p.y - h / 2 - TEXT_PAD, w + TEXT_PAD * 2, h + TEXT_PAD * 2);

			prim.fill_color(m_color_profile[cat][COLOR_FILL]);
			prim.line_color(m_color_profile[cat][COLOR_BORDER]);
			prim.outlined_rectangle(int(r.x), int(r.y), int(r.x + r.Width), int(r.y + r.Height));

			label.start_point(r.x + TEXT_PAD, r.y + r.Height - TEXT_PAD);

			ras.add_path(lo);
			renderer.color(m_color_profile[cat][COLOR_TEXT]);
			//ras.render(sl, renderer);
			agg::render_scanlines(ras, m_sl, renderer);

			setRect(v, r);
		}
	}

	agg::rendering_buffer& rbuf_window()          { return m_rbuf_window; } 

protected:
    enum { max_images = 16 };
    enum pix_format_e
    {
        pix_format_undefined = 0,  // By default. No conversions are applied 
        pix_format_gray8,          // Simple 256 level grayscale
        pix_format_rgb555,         // 15 bit rgb. Depends on the byte ordering!
        pix_format_rgb565,         // 16 bit rgb. Depends on the byte ordering!
        pix_format_rgb24,          // R-G-B, one byte per color component
        pix_format_bgr24,          // B-G-R, native win32 BMP format.
        pix_format_rgba32,         // R-G-B-A, one byte per color component
        pix_format_argb32,         // A-R-G-B, native MAC format
        pix_format_abgr32,         // A-B-G-R, one byte per color component
        pix_format_bgra32,         // B-G-R-A, native win32 BMP format
  
        end_of_pix_formats
    };

	pix_format_e m_format;
    unsigned m_bpp;
	agg::rendering_buffer m_rbuf_window;
    bool m_flip_y;
    int m_initial_width;
    int m_initial_height;
	agg::trans_affine m_resize_mtx;
    agg::rgba8 m_color_profile[256][256];
    agg::scanline_p8 m_sl;

	//platform_specific
    pix_format_e		m_sys_format;
    unsigned			m_sys_bpp;
	agg::pixel_map		m_pmap_window;
};

using namespace boost;

CAggRenderImpl::CAggRenderImpl(CGraphData &graph) : CRender(graph), 
        m_sys_format(pix_format_bgr24),
        m_format(pix_format_bgr24),
		m_sys_bpp(24),
        m_flip_y(false),
        m_bpp(24),
		m_initial_width(0), 
		m_initial_height(0)
{
}

void CAggRenderImpl::render(CDCHandle &dc, const GraphTypes::PointF &origin, Path &overToSel, vertex_descriptor overNode, vertex_descriptor focusNode, bool calculateExtent)
{
	// TODO: maybe have caller set these (have public accessors) and remove from method
	m_dc = &dc;
	m_overToSel = overToSel;
	m_overNode = overNode;
	m_focusNode = focusNode;
	SetOffset(origin);

	SetDPI(GetDeviceCaps(dc, LOGPIXELSX), GetDeviceCaps(dc, LOGPIXELSY));

	CRect clientRect;
	dc.GetClipBox(clientRect);

	CBrush desktop;
	desktop.CreateSolidBrush(0xffffff);
	dc.FillRect(clientRect, desktop);

	//drawLines(dc);
	//drawVertices(dc);

	RECT rt;
	dc.GetClipBox(&rt);

	int width = rt.right - rt.left;
	int height = rt.bottom - rt.top;

	for (int cat = 0; cat < 10/*m_meta.GetMetaCount()*/; ++cat)
	{
		unsigned long tmp = m_meta.GetMetaVertexColorFillNorm(cat);
		m_color_profile[cat][COLOR_FILL] = agg::rgba8(GetBValue(tmp), GetGValue(tmp), GetRValue(tmp));
		
		tmp = m_meta.GetMetaVertexColorBorderNorm(cat);
		m_color_profile[cat][COLOR_BORDER] = agg::rgba8(GetBValue(tmp), GetGValue(tmp), GetRValue(tmp));

		tmp = m_meta.GetMetaEdgeColorNorm(cat);
		m_color_profile[cat][COLOR_LINE] = agg::rgba8(GetBValue(tmp), GetGValue(tmp), GetGValue(tmp));

		m_color_profile[cat][COLOR_TEXT] = agg::rgba8(0, 0, 0);
	}


	//============================================================ 
	//Creating compatible DC and a bitmap to render the image 

    create_pmap(width, height, &m_rbuf_window);
    m_initial_width = width;
    m_initial_height = height;

    on_draw();

	display_pmap(dc, &rbuf_window());
}

CString & CAggRenderImpl::getText(const vertex_descriptor &v, CString & txt)
{
	// the first user property is the label
	if (m_meta.GetMetaDebug())
	{
		txt = boost::lexical_cast<std::_tstring>(m_graph.getVertexCat(v).GetCategory()).c_str();
		txt += _T(":");
		txt += m_graph.getVertexIdentity(v);
		txt += _T(" ");
		txt += m_graph.getVertexProperty(v, 0);
	}
	else
	{
		txt = m_graph.getVertexProperty(v, 0);
		if (txt.GetLength() == 0)
		{
			txt = boost::lexical_cast<std::_tstring>(m_graph.getVertexCat(v).GetCategory()).c_str();
			txt += _T(":");
			txt += m_graph.getVertexIdentity(v);
		}
	}
	return txt;
	/*
	switch(m_graph.getVertexExecute(v))
	{
	case UNKNOWN:
	txt += _T(" [+]");
	break;
	case EXECUTING:
	txt += _T(" [?]");
	break;
	case EXECUTED:
	txt += _T(" [-]");
	break;
	}
	*/
}

GraphTypes::PointF CAggRenderImpl::getCenter(const GraphTypes::RectF &r)
{
	//AllTransform(r);
	return PointF(r.x + r.Width / 2, r.y + r.Height / 2);
}

RECT CAggRenderImpl::paintNormVertex(CDCHandle &dc, const vertex_descriptor &v, const GraphTypes::RectF &r, const Color &fontColor)
{
	CCategory cat = m_graph.getVertexID(v).GetCategory();
	Color fill(m_meta.GetMetaVertexColorFillNorm(cat));
	Color border(m_meta.GetMetaVertexColorBorderNorm(cat));
	return paintVertex(dc, v, r, fontColor, fill, border);
}

RECT CAggRenderImpl::paintSelVertex(CDCHandle &dc, const vertex_descriptor &v, const GraphTypes::RectF &r, const Color &fontColor)
{
	CCategory cat = m_graph.getVertexID(v).GetCategory();
	Color fill(m_meta.GetMetaVertexColorFillSel(cat));
	Color border(m_meta.GetMetaVertexColorBorderSel(cat));
	return paintVertex(dc, v, r, fontColor, fill, border);
}

RECT CAggRenderImpl::paintOverVertex(CDCHandle &dc, const vertex_descriptor &v, const GraphTypes::RectF &r, const Color &fontColor) 
{
	CCategory cat = m_graph.getVertexID(v).GetCategory();
	Color fill(m_meta.GetMetaVertexColorFillOver(cat));
	Color border(m_meta.GetMetaVertexColorBorderOver(cat));
	return paintVertex(dc, v, r, fontColor, fill, border);
}

RECT CAggRenderImpl::paintVertex(CDCHandle &dc, const vertex_descriptor &v, const GraphTypes::RectF &r, const Color &fontColor, const Color &fill, const Color &border)
{
	CPen borderPen;
	borderPen.CreatePen(PS_SOLID, 1, border.ToCOLORREF());
	CBrush fillBrush;
	fillBrush.CreateSolidBrush(fill.ToCOLORREF());

	RECT frameRect = toRect(r);
	RECT textRect = {frameRect.left+2, frameRect.top+2, frameRect.right-2, frameRect.bottom-2};

	{
		CAutoPen p(borderPen, dc);
		CAutoBrush b(fillBrush, dc);
		dc.SetBkColor(fill.ToCOLORREF());
		dc.Rectangle(&frameRect);

		CString txt;
		getText(v, txt);
		//PointF& p = m_graph.getVertexPoint(v);
		//txt = _T("X:"); txt += boost::lexical_cast<std::_tstring>(int(r.x)).c_str();
		//txt += _T(" Y:"); txt += boost::lexical_cast<std::_tstring>(int(r.y)).c_str();
		//txt += _T(" WX:"); txt += boost::lexical_cast<std::_tstring>(int(p.x)).c_str();
		//txt += _T(" WY:"); txt += boost::lexical_cast<std::_tstring>(int(p.y)).c_str();
		dc.DrawText(txt, txt.GetLength(), &textRect, DT_NOPREFIX | DT_CENTER);
	}

	setRect(v, r);
	drawIcon(dc, v, r);
	drawImage(dc, v, r);
	return frameRect;
}

void CAggRenderImpl::paintArrow(CDCHandle &dc, const edge_descriptor &e, GraphTypes::PointF p1, GraphTypes::PointF p2, const CPen &p) 
{
	AllTransform(p1);
	AllTransform(p2);
	dc.MoveTo((int)p1.x, (int)p1.y);
	dc.LineTo((int)p2.x, (int)p2.y);
	if (m_meta.GetMetaDebug())
	{
		CFont scaledFont;
		createFont(dc, scaledFont, GetFontSize());
		CAutoFont f(scaledFont, dc);
		CString txt = boost::lexical_cast<std::_tstring>(m_graph.getEdgeCat(e).GetCategory()).c_str();

		RECT bounds = {0, 0, 0, 0};
		bounds.left = (int)((p1.x + p2.x) / 2);
		bounds.top = (int)((p1.y + p2.y) / 2);
		dc.DrawText(txt, txt.GetLength(), &bounds, DT_CALCRECT|DT_NOPREFIX|DT_CENTER);
		dc.DrawText(txt, txt.GetLength(), &bounds, DT_NOPREFIX | DT_CENTER);
	}
}

RECT CAggRenderImpl::toRect(const GraphTypes::RectF &p)
{
	CRect retVal((int)p.x, (int)p.y, (int)(p.x + p.Width), (int)(p.y + p.Height));
	return retVal;
}

void CAggRenderImpl::adjustOffsets(GraphTypes::RectF& rOut, const GraphTypes::RectF& rIn, PLACEMENT placement, bool isIPicture)
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

void CAggRenderImpl::drawIcon(CDCHandle& dc, const vertex_descriptor& v, const RectF& rText)
{
	IPicture* icon = m_graph.getVertexIcon(v);
	if (icon == NULL)
		icon = m_meta.GetMetaVertexIcon(m_graph.getVertexID(v).GetCategory());

	drawBitmap(dc, v, rText, icon, m_meta.GetMetaVertexHtmlElement(m_graph.getVertexID(v).GetCategory()), m_meta.GetMetaVertexIconPlacement());
}

void CAggRenderImpl::drawImage(CDCHandle& dc, const vertex_descriptor& v, const RectF& rText)
{
	drawBitmap(dc, v, rText, m_graph.getVertexImage(v), m_graph.getVertexHtmlElement(v), m_meta.GetMetaVertexImagePlacement());
}

void CAggRenderImpl::drawBitmap(CDCHandle& dc, const vertex_descriptor& v, const RectF& rText, IPicture* image, HTMLElement* element, const PLACEMENT& placement)
{
	if (image == NULL && (element == NULL || element->GetHtmlElement() == NULL))
		return;
	LONG width, height;
	GraphTypes::RectF rImg; 
	if (image != NULL) {
		image->get_Width( &width );
		image->get_Height( &height );
		CSize szPic(width, height);
		// convert OLE size into pixels
		dc.HIMETRICtoDP( &szPic );
		rImg.Width = REAL(szPic.cx);
		rImg.Height = REAL(szPic.cy);
	} else if (element != NULL)
		rImg = element->GetSize();
	HRESULT hr = S_OK;
	//COLORREF transparent = RGB(253,0,200);
	COLORREF transparent = RGB(0,0,211); // C# transparent gif background color
	CMemBm bmp(dc, 0, 0, 0, (int)rImg.Width, (int)rImg.Height);
	CDCHandle hdcMem = bmp;
	hdcMem.FillSolidRect(0, 0, (int)rImg.Width, (int)rImg.Height, transparent);
	if (image != NULL) {
		hr = image->Render(hdcMem, 0, long(rImg.Height), long(rImg.Width), long(-rImg.Height), 0, 0, width, height, NULL );
	} else if (element != NULL && element->GetHtmlElement() != NULL) {
		hr = element->GetHtmlElement()->DrawToDC(hdcMem);
	}
	if (m_overNode != v || GetScale() > 1.0f) {
		ScaleTransform(rImg);
	}
	adjustOffsets(rImg, rText, placement, false);
	bmp.DrawTrans(dc, int(rImg.x), int(rImg.y), int(rImg.Width), int(rImg.Height), transparent);
}

void CAggRenderImpl::drawLines(CDCHandle& dc)
{
	CComPtr<clib::scoped_lock_ref_counted> lock;

	for (StlLinked<IEdgeIterator> e = m_graph.getVisibleEdgeIterator(lock); e->IsValidItem(); e->NextItem())
	{
		CPen pNormal, pShortest;
		Color lineNormal(m_meta.GetMetaEdgeColorNorm(m_graph.getEdgeCat(e->Get())));
		pNormal.CreatePen(PS_SOLID, m_meta.GetMetaEdgeWidth(m_graph.getEdgeCat(e->Get())), lineNormal.ToCOLORREF());
		Color lineShortestPath(m_meta.GetMetaEdgeColorShortest(m_graph.getEdgeCat(e->Get())));
		pShortest.CreatePen(PS_SOLID, m_meta.GetMetaEdgeWidth(m_graph.getEdgeCat(e->Get())), lineShortestPath.ToCOLORREF());

		vertex_descriptor from = m_graph.getEdgeSource(e->Get());
		vertex_descriptor to = m_graph.getEdgeTarget(e->Get());
		if (m_overNode != -1 && m_overToSel.ContainsStep(from, to))
		{
			CAutoPen p(pShortest, dc);
			paintArrow(dc, e->Get(), m_graph.getVertexPoint(from), m_graph.getVertexPoint(to), pShortest);
		}
		else
		{
			CAutoPen p(pNormal, dc);
			paintArrow(dc, e->Get(), m_graph.getVertexPoint(from), m_graph.getVertexPoint(to), pNormal);
		}
	}
}

HPEN CAggRenderImpl::selectBorderPen(CDCHandle& dc, const vertex_descriptor& v)
{
	ATLASSERT(false);
	CPen pNormal;
	if (m_overNode != -1 && m_overNode == v && m_overNode < m_graph.getVertexCount())
		pNormal.CreatePen(0, 1, Color(m_meta.GetMetaVertexColorBorderOver(m_graph.getVertexCat(v))).ToCOLORREF());
	else if (m_focusNode != -1 && m_focusNode == v && m_focusNode < m_graph.getVertexCount())
		pNormal.CreatePen(0, 1, Color(m_meta.GetMetaVertexColorBorderSel(m_graph.getVertexCat(v))).ToCOLORREF());
	else
		pNormal.CreatePen(0, 1, Color(m_meta.GetMetaVertexColorBorderNorm(m_graph.getVertexCat(v))).ToCOLORREF());
	return dc.SelectPen(pNormal);
}

void CAggRenderImpl::drawVertices(CDCHandle& dc)
{
	static int prevSize = 0;
	static vertex_descriptor prevOver = -1;
	//GetFontSize() = (int)(DEFAULT_FONT_SIZE * GetScale());

	CFont scaledFont;
	createFont(dc, scaledFont, GetFontSize());
	CAutoFont f(scaledFont, dc);

	CComPtr<clib::scoped_lock_ref_counted> lock;
	for (StlLinked<IVertexIterator> n = m_graph.getVisibleVertexIterator(lock); n->IsValidItem(); n->NextItem())
	{
		vertex_descriptor v = n->Get();

		if (v != m_focusNode && v != m_overNode)
		{
			GraphTypes::RectF r = calcTextRect(v, (v == prevOver));
			paintNormVertex(dc, v, r, Color(128, 0, 0, 0));
		}
		else if (v == m_focusNode && v != m_overNode)
		{
			GraphTypes::RectF r = calcTextRect(v, (v == prevOver));
			paintSelVertex(dc, v, r, Color(128, 0, 0, 0));
		}
		// Paint overNode outside of loop so it's always on top.
		//else if (v == m_overNode)
	}

	if (m_overNode != -1)
	{
		bool recalc = false;
		if (GetScale() < 1.0f) {
			recalc = true;
			CFont fullSizeFont;
			createFont(dc, fullSizeFont, DEFAULT_FONT_SIZE);
			dc.SelectFont(fullSizeFont);
		}
		GraphTypes::RectF r = calcTextRect(m_overNode, recalc);
		paintOverVertex(dc, m_overNode, r, Color(128, 0, 0, 0));
		prevOver = m_overNode;
	}

	prevSize = GetFontSize();
}

void CAggRenderImpl::createFont(CDCHandle &dc, CFont &font, int size)
{
	size++;
	int fontHeight = ::MulDiv(size, GetDPIY(), NINTY_SIX);
	font.CreateFont(
		fontHeight,				   // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_THIN,                   // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		fontHeight <= 7 ? _T("Small Fonts") : _T("Verdana"));            // lpszFacename
}

GraphTypes::RectF CAggRenderImpl::calcTextRect(const vertex_descriptor &v, bool recalc)
{
	GraphTypes::RectF r(m_graph.getVertexPoint(v), getSize(v));
	AllTransform(r);
	if (recalc || r.Width == 0 || r.Height == 0)
	{
		RECT bounds = calcTextBounds(v);
		r.Width = REAL(bounds.right + 4);
		r.Height = REAL(bounds.bottom + 4);
	}
	else
	{
		GraphTypes::RectF prevRect = getRect(v);
		r.Width = prevRect.Width;
		r.Height = prevRect.Height;
	}
	r.x -= r.Width / 2.0f;
	r.y -= r.Height / 2.0f;
	return r;
}

RECT CAggRenderImpl::calcTextBounds(const vertex_descriptor &v)
{
	CString txt;
	getText(v, txt);
	RECT bounds = {0, 0, 0, 0};
	m_dc->DrawText(txt, txt.GetLength(), &bounds, DT_CALCRECT|DT_NOPREFIX|DT_CENTER);
	return bounds;
}

void CAggRenderImpl::create_pmap(unsigned width, unsigned height, agg::rendering_buffer* wnd)
{
    m_pmap_window.create(width, height, agg::org_e(m_bpp));
    wnd->attach(m_pmap_window.buf(), m_pmap_window.width(), m_pmap_window.height(), m_flip_y ? m_pmap_window.stride() : -m_pmap_window.stride());
}

void CAggRenderImpl::display_pmap(HDC dc, const agg::rendering_buffer* src)
{
    if(m_sys_format == m_format)
    {
        m_pmap_window.draw(dc);
    }
    else
    {
		agg::pixel_map pmap_tmp;
		pmap_tmp.create(m_pmap_window.width(), m_pmap_window.height(), agg::org_e(m_sys_bpp));

		agg::rendering_buffer rbuf_tmp;
        rbuf_tmp.attach(pmap_tmp.buf(), pmap_tmp.width(), pmap_tmp.height(), m_flip_y ? pmap_tmp.stride() :-pmap_tmp.stride());

        switch(m_format)
        {
        case pix_format_gray8:
            return;

        case pix_format_rgb565:
			agg::color_conv(&rbuf_tmp, src, agg::color_conv_rgb565_to_rgb555());
            break;

        case pix_format_rgb24:
            color_conv(&rbuf_tmp, src, agg::color_conv_rgb24_to_bgr24());
            break;

        case pix_format_abgr32:
			color_conv(&rbuf_tmp, src, agg::color_conv_abgr32_to_bgra32());
            break;

        case pix_format_argb32:
            color_conv(&rbuf_tmp, src, agg::color_conv_argb32_to_bgra32());
            break;

        case pix_format_rgba32:
            color_conv(&rbuf_tmp, src, agg::color_conv_rgba32_to_bgra32());
            break;
        }
        pmap_tmp.draw(dc);
    }
}
void CAggRenderImpl::on_draw()
{
    scanline_rasterizer ras;
    pixfmt pixf(rbuf_window());
    base_renderer rb(pixf);
    solid_renderer solid(rb);
    draft_renderer draft(rb);

    rb.clear(agg::rgba(1, 1, 1));

	SetFontSize((int)(DEFAULT_FONT_SIZE * GetScale()));
	CFont scaledFont;
	createFont(*m_dc, scaledFont, GetFontSize());
	CAutoFont f(scaledFont, *m_dc);

    draw_scene(ras, solid, draft);
}

void CAggRenderImpl::draw_scene(scanline_rasterizer& ras, solid_renderer& fine, draft_renderer& draft)
{
	ras.gamma(agg::gamma_none());
	if (m_draft)
	{
		draw_lines(ras, draft);  
		draw_nodes(ras, draft);
	}
	else
	{
		draw_lines(ras, fine);  
		draw_nodes(ras, fine);
	}
}

IRender * CreateAggRenderer(CGraphData &graph)
{
	return new CAggRenderImpl(graph);
}
