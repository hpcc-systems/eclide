#include "stdafx.h"

#include "AggHDCRasterizer.h"

using namespace agg;

//------------------------------------------------------------------------
hdc_rasterizer::~hdc_rasterizer()
{
    ::SelectObject(m_dc, m_old_pen);
    ::SelectObject(m_dc, m_old_brush);
    ::DeleteObject(m_null_brush);
    ::DeleteObject(m_null_pen);
    if (m_pen)
    {
        ::DeleteObject(m_pen);
    }
    if (m_brush)
    {
        ::DeleteObject(m_brush);
    }
    if (m_line_color_brush)
    {
        ::DeleteObject(m_line_color_brush);
    }
    delete [] m_closed;
    delete [] m_counts;
    delete [] m_points;
}

//------------------------------------------------------------------------
hdc_rasterizer::hdc_rasterizer(HDC dc) : 
m_dc(dc),
m_brush(0),
m_null_brush(0),
m_line_color_brush(0),
m_old_brush(0),
m_pen(0),
m_null_pen(0),
m_old_pen(0),
m_line_width(1),
m_line_cap(PS_ENDCAP_ROUND),
m_line_join(PS_JOIN_ROUND),
m_points(new POINT[points_cluster_size]),
m_counts(new int[polygons_cluster_size]),
m_closed(new bool[polygons_cluster_size]),
m_num_poly(0),
m_num_points(0),
m_max_poly(polygons_cluster_size),
m_max_points(points_cluster_size),
m_reset_flag(false)
{
    m_null_log_brush.lbStyle = BS_NULL;
    m_null_log_brush.lbColor = 0;
    m_null_log_brush.lbHatch = 0;
    m_null_brush = ::CreateBrushIndirect(&m_null_log_brush);
    m_old_brush = (HBRUSH)::SelectObject(m_dc, m_null_brush);
    m_null_pen = ::CreatePen(PS_NULL, 1, 0);
    m_old_pen  = (HPEN)::SelectObject(m_dc, m_null_pen);
    ::SetPolyFillMode(m_dc, WINDING);
    line_color(rgba8(0,0,0));
    fill_color(rgba8(255,255,255));
}


//------------------------------------------------------------------------
void hdc_rasterizer::line_color(agg::rgba8 c)
{
    m_line_color = c;
    create_pen();
    ::SelectObject(m_dc, m_null_brush);
    if (m_line_color_brush)
    {
        ::DeleteObject(m_line_color_brush);
    }
    m_line_color_brush = ::CreateSolidBrush(RGB(c.r, c.g, c.b));
}


//------------------------------------------------------------------------
void hdc_rasterizer::fill_color(agg::rgba8 c)
{
    m_fill_color = c;
    ::SelectObject(m_dc, m_null_brush);
    if (m_brush)
    {
        ::DeleteObject(m_brush);
    }
    m_brush = ::CreateSolidBrush(RGB(c.r, c.g, c.b));
}


//------------------------------------------------------------------------
void hdc_rasterizer::line_width(int width)
{
    m_line_width = width;
    if (m_line_width <= 0) m_line_width = 1;
    create_pen();
}


//------------------------------------------------------------------------
void hdc_rasterizer::line_cap(agg::line_cap_e cap)
{
    if (cap == agg::round_cap)  m_line_cap = PS_ENDCAP_ROUND;
    if (cap == agg::square_cap) m_line_cap = PS_ENDCAP_SQUARE;
    if (cap == agg::butt_cap)   m_line_cap = PS_ENDCAP_FLAT;
    create_pen();
}

//------------------------------------------------------------------------
void hdc_rasterizer::line_join(agg::line_join_e join)
{
    if (join == agg::bevel_join) m_line_join = PS_JOIN_BEVEL;
    if (join == agg::miter_join) m_line_join = PS_JOIN_MITER;
    if (join == agg::round_join) m_line_join = PS_JOIN_ROUND;
    create_pen();
}

//------------------------------------------------------------------------
void hdc_rasterizer::fill_even_odd(bool even_odd_flag)
{
    ::SetPolyFillMode(m_dc, even_odd_flag ? ALTERNATE : WINDING);
}

//------------------------------------------------------------------------
void hdc_rasterizer::create_pen()
{
    ::SelectObject(m_dc, m_null_pen);
    if (m_pen)
    {
        ::DeleteObject(m_pen);
    }
    m_line_log_brush.lbStyle = BS_SOLID;
    m_line_log_brush.lbColor = RGB(m_line_color.r, m_line_color.g,
        m_line_color.b); 
    m_line_log_brush.lbHatch = 0; 
    m_pen = ExtCreatePen(PS_GEOMETRIC | PS_SOLID | m_line_cap | m_line_join,
        m_line_width,
        &m_line_log_brush,
        0,
        0);
}


//------------------------------------------------------------------------
void hdc_rasterizer::reset()
{
    m_num_poly = 0;
    m_num_points = 0;
    m_reset_flag = false;
}


//------------------------------------------------------------------------
void hdc_rasterizer::realloc_points()
{
    if (m_num_points >= m_max_points)
    {
        POINT* tp = new POINT[m_max_points + points_cluster_size];
        memcpy(tp, m_points, m_num_points * sizeof(POINT));
        delete [] m_points;
        m_points = tp;
        m_max_points += points_cluster_size;
    }
}

//------------------------------------------------------------------------
void hdc_rasterizer::realloc_polygons()
{
    if (m_num_poly >= m_max_poly)
    {
        int* tcnt = new int[m_max_poly + polygons_cluster_size];
        memcpy(tcnt, m_counts, m_num_poly * sizeof(int));
        delete [] m_counts;
        m_counts = tcnt;

        bool* tclosed = new bool[m_max_poly + polygons_cluster_size];
        memcpy(tclosed, m_closed, m_num_poly * sizeof(bool));
        delete [] m_closed;
        m_closed = tclosed;

        m_max_poly += polygons_cluster_size;
    }
}


//------------------------------------------------------------------------
void hdc_rasterizer::move_to(int x, int y) 
{ 
    if (m_reset_flag) reset();

    if (m_num_poly && m_counts[m_num_poly-1] < 2)
    {
        m_points[m_num_points-1].x = x;
        m_points[m_num_points-1].y = y;
    }
    else
    {
        realloc_points();
        realloc_polygons();
        m_points[m_num_points].x = x;
        m_points[m_num_points].y = y;
        m_counts[m_num_poly] = 1;
        m_closed[m_num_poly] = false;
        m_num_points++;
        m_num_poly++;
    }
}

//------------------------------------------------------------------------
void hdc_rasterizer::line_to(int x, int y) 
{ 
    if (m_num_poly)
    {
        realloc_points();
        m_points[m_num_points].x = x;
        m_points[m_num_points].y = y;
        m_counts[m_num_poly-1]++;
        m_num_points++;
    }
}

//------------------------------------------------------------------------
void hdc_rasterizer::close_polygon()
{
    if (m_num_poly)
    {
        m_closed[m_num_poly-1] = true;
    }
}


//------------------------------------------------------------------------
void hdc_rasterizer::render_fill_only()
{
    if (m_num_poly && m_brush && m_fill_color.a > 250)
    {
        if (m_counts[m_num_poly-1] < 3) --m_num_poly;
        if (m_num_poly)
        {
            ::SelectObject(m_dc, m_brush);
            ::SelectObject(m_dc, m_null_pen);
            ::PolyPolygon(m_dc, m_points, m_counts, m_num_poly);
        }
    }
    m_reset_flag = true;
}


//------------------------------------------------------------------------
void hdc_rasterizer::render_fill_and_stroke()
{
    if (m_fill_color.a > 250)
    {
        if (m_num_poly && m_pen && m_brush)
        {
            if (m_counts[m_num_poly-1] < 3) --m_num_poly;
            if (m_num_poly)
            {
                ::SelectObject(m_dc, m_brush);
                ::SelectObject(m_dc, m_pen);
                ::PolyPolygon(m_dc, m_points, m_counts, m_num_poly);
            }
        }
    }
    else
    {
        render_stroke_only();
    }
    m_reset_flag = true;
}


//------------------------------------------------------------------------
void hdc_rasterizer::render_fill_with_line_color()
{
    if (m_num_poly && m_line_color_brush && m_line_color.a > 250)
    {
        if (m_counts[m_num_poly-1] < 3) --m_num_poly;
        if (m_num_poly)
        {
            ::SelectObject(m_dc, m_null_pen);
            ::SelectObject(m_dc, m_line_color_brush);
            ::PolyPolygon(m_dc, m_points, m_counts, m_num_poly);
        }
    }
    m_reset_flag = true;
}


//------------------------------------------------------------------------
void hdc_rasterizer::render_stroke_only()
{
    if (m_num_poly && m_pen && m_line_color.a)
    {
        ::SelectObject(m_dc, m_null_brush);
        ::SelectObject(m_dc, m_pen);
        int i;
        int count = 0;
        for (i = 0; i < m_num_poly; i++)
        {
            if (m_counts[i] > 1)
            {
                if (m_closed[i])
                {
                    ::Polygon(m_dc, m_points + count, m_counts[i]);
                }
                else
                {
                    ::Polyline(m_dc, m_points + count, m_counts[i]);
                }
            }
            count += m_counts[i];
        }
    }
    m_reset_flag = true;
}