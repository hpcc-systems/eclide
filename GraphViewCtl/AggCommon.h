#pragma once

//-----------------------------------------------------------------------------

#ifndef _WIN32_WCE
typedef agg::font_engine_win32_tt_int32 font_engine_type;
typedef agg::font_cache_manager<font_engine_type> font_manager_type;
typedef agg::conv_curve<font_manager_type::path_adaptor_type>		conv_font_curve_type;
typedef agg::conv_segmentator<conv_font_curve_type>					conv_font_segm_type;
typedef agg::conv_transform<conv_font_segm_type, agg::trans_affine> conv_font_trans_type;
#endif
typedef agg::conv_transform<agg::path_storage, agg::trans_affine>	conv_path_trans_type;
typedef agg::conv_clip_polyline<conv_path_trans_type>				conv_path_clip_trans_type;

#ifndef _WIN32_WCE
struct CAggFonts
{
	CAggFonts(HDC hdc) : m_feng(hdc), m_fman(m_feng) {};

    font_engine_type	m_feng;
    font_manager_type	m_fman;
};
#else
struct CAggFonts
{
	CAggFonts(HDC hdc) {};
};
#endif

//*****************************************************************************
// 
//	Agg support classes - not part of official AGG distribution 
//

template <class Rasterizer, class Path>
void AddPathWithArrowHead(
	Rasterizer& ras,
	Path& path,
	agg::trans_affine& trans,
	double length,
	double halfwidth,
	double inset,
	double scale=1.0)
{
    double k = ::pow(path.width(), 0.7)*scale;

	path.shorten(k*(length-inset));

	typedef agg::conv_transform<Path, agg::trans_affine> conv_trans_type;
	conv_trans_type pathtrans(path, trans);
	ras.add_path(pathtrans);

    agg::arrowhead ah;
    ah.head(0, (length-inset) * k, halfwidth * k, inset * k);
	typedef agg::conv_marker<agg::vcgen_markers_term, agg::arrowhead> arrow_type;
    arrow_type arrow(path.markers(), ah);
	typedef agg::conv_transform<arrow_type, agg::trans_affine> arrow_trans_type;
	arrow_trans_type arrowtrans(arrow, trans); 
    ras.add_path(arrowtrans);
}

namespace agg
{
//------------------------------------------------------------------------
class curve_casteljau
{
public:
	curve_casteljau();
	curve_casteljau(unsigned int num_points);

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
	pod_vector<point_d>		 m_vertices;
	pod_vector<point_d>		 m_tree;
	double                   m_mu;
	double                   m_dmu;
	int                      m_num_steps;
	int                      m_step;
};


// A simple function to form the gradient color array 
// consisting of 2 colors, "begin" and "end"
//---------------------------------------------------

template<class Array>
void fill_color_array(Array& arr, 
                      agg::rgba8 begin, 
                      agg::rgba8 end)
{
    unsigned i;
    unsigned size = arr.size();
    for(i = 0; i < size; ++i)
    {
        arr[i] = begin.gradient(end, i / double(size));
    }
}


} // namespace agg