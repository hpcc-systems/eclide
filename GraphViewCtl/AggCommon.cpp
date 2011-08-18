#include "stdafx.h"

//#include "Canvas.h"
#include "AggDC.h"
#include "AggCommon.h"

using namespace agg;

//*****************************************************************************
// 
//	Agg support classes - not part of official AGG distribution 
//
//*****************************************************************************
//
//		curve_casteljau is the result of work with Agg's author Maxim Shemanarev.
//		Not optimized or official part of Agg yet.
//
//*****************************************************************************

//------------------------------------------------------------------------
curve_casteljau::curve_casteljau() : 
m_scale(1.0), m_length(0.0), m_num_steps(0), m_step(0) 
{
}

curve_casteljau::curve_casteljau(unsigned int num_points) : 
m_scale(1.0), m_length(0.0), m_num_steps(0), m_step(0) 
{
	m_vertices.resize(num_points);
}

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
		const point_d& p = m_vertices[m_vertices.size() - 1];
		m_length += calc_distance(x, y, p.x, p.y);
	}
	m_vertices.add(point_d(x, y));
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
		*x = m_vertices[m_vertices.size() - 1].x;
		*y = m_vertices[m_vertices.size() - 1].y;
		--m_step;
		return path_cmd_line_to;
	}

	m_tree = m_vertices;
	for(unsigned n = m_tree.size(); n > 1; n--)
	{
		for(unsigned i = 0; i < n - 1; i++)
		{
			point_d& p1 = m_tree[i];
			const point_d& p2 = m_tree[i + 1];
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

//-----------------------------------------------------------------------------
//
//	scale -1 means don't apply scale

#ifndef _WIN32_WCE
GraphTypes::SizeF CAggDC::GetScaledTextExtent(
		const TCHAR* text,
		const char* font,
		CAggFonts* fonts, 
		int size,
		GraphTypes::REAL scale/*=-1*/)
{
	GraphTypes::SizeF renderSize;

	fonts->m_feng.flip_y(true);
	fonts->m_feng.hinting(true);

	if(fonts->m_feng.create_font(
		font, 
		agg::glyph_ren_outline,
		size,
		0.0,
		FW_NORMAL,
		false,
		ANSI_CHARSET,
		DEFAULT_PITCH | FF_SWISS ))
	{
		double x = 0.0;
		double y = 0.0;
		const TCHAR* p = text;

		//agg::trans_affine mtx;
		//mtx *= agg::trans_affine_scaling(scale);

		conv_font_curve_type fcurves(fonts->m_fman.path_adaptor());
		conv_font_segm_type  fsegm(fcurves);
		//conv_font_trans_type ftrans(fsegm, mtx);
		agg::rasterizer_scanline_aa<> ras_aa;

		while(*p)
		{
			const agg::glyph_cache* glyph = fonts->m_fman.glyph(*p);
			if(glyph)
			{
				fonts->m_fman.add_kerning(&x, &y);
				fonts->m_fman.init_embedded_adaptors(glyph, x, y);

				if(glyph->data_type == agg::glyph_data_outline)
				{
					ras_aa.add_path(fsegm);
				}

				// increment pen position
				x += glyph->advance_x;
				y += glyph->advance_y;
			}
			++p;
		}

		renderSize.Width=GraphTypes::REAL(x); 
		renderSize.Height=GraphTypes::REAL(fonts->m_feng.height());

		// unfortunately, the measure above is unreliable
		GraphTypes::REAL pathwidth=ras_aa.max_x()-ras_aa.min_x()+1;
		if(pathwidth>renderSize.Width)
			renderSize.Width=pathwidth;

		GraphTypes::REAL pathheight=ras_aa.max_y()-ras_aa.min_y()+1;
		if(pathheight>renderSize.Height)
			renderSize.Height=pathheight;
	}

	return renderSize;
}
#endif

//-----------------------------------------------------------------------------

