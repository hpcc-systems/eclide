// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#pragma warning(disable:4251)
#pragma warning(disable:4275)
#pragma warning(disable:4503)
#pragma warning(disable:4127)

//  Leak Checking ---
#if defined(_DEBUG) && !defined(SEISINT_LIBEXPORTS)
#define _CRTDBG_MAP_ALLOC
#endif

#define _ATL_APARTMENT_THREADED
#define _ATL_ALL_WARNINGS
#define _ATL_DEBUG_QI
#define _ATL_DEBUG_INTERFACES

#include <yvals.h>              // warning numbers get enabled in yvals.h 

//  Target  ---
#include "stdversion.h"

// Platform SDK  ---
#include "stdplatform.h"

//  ATL  ---
#include "stdatl.h"

//  WTL  ---
#define _WTL_NO_WTYPES
#define _WTL_NO_CSTRING
#include <atlapp.h>
#define _Module (*_pModule)

#include "stdwtl.h"

//  WTLEX  ---
#include "atlgdix.h"
#include "membm.h"
#include "dialogex.h"

#undef _Module

//  XML ---
#include <msxml2.h>

//  STL  ---
#include "stdstl.h"

//  BOOST  ---
#include "stdboost.h"

// AGG  ---
#include <agg_basics.h>
#include <agg_pixfmt_rgba.h>
#include <agg_rendering_buffer.h>
#include <agg_renderer_scanline.h>
#include <agg_renderer_outline_aa.h>
#include <agg_rasterizer_scanline_aa.h>
#include <agg_rasterizer_outline.h>
#include <agg_rasterizer_outline_aa.h>
#include <agg_conv_stroke.h>
#include <agg_conv_dash.h>
#include <agg_conv_curve.h>
#include <agg_conv_contour.h>
#include <agg_conv_marker.h>
#include <agg_conv_shorten_path.h>
#include <agg_conv_marker_adaptor.h>
#include <agg_conv_concat.h>
#include <agg_conv_bspline.h>
#include <agg_conv_clip_polyline.h>
#include <agg_ellipse.h>
#include <agg_arrowhead.h>
#include <agg_vcgen_markers_term.h>
#include <agg_scanline_p.h>
#include <agg_scanline_u.h>
#include <agg_renderer_primitives.h>
#include <agg_span_allocator.h>
#include <agg_span_gradient.h>
#include <agg_span_interpolator_linear.h>
#include <agg_gsv_text.h>
#include <agg_bspline.h>
#include <agg_path_storage.h>
#include <agg_conv_smooth_poly1.h>
#include <agg_conv_segmentator.h>
#include <util/agg_color_conv.h>
#include <util/agg_color_conv_rgb8.h>
#include <platform/win32/agg_win32_bmp.h>
#include <./ctrl/agg_polygon_ctrl.h>
#include <../font_win32_tt/agg_font_win32_tt.h>

//  TimeLine  ---
#include <ATLComTime.h>
#include <TimeLineCtrl.h>
#include <TimeLineBand.h>
#include <TimeLineBandImpl.h>

//  Leak Checking  ---

#if defined(_DEBUG) && !defined(SEISINT_LIBEXPORTS)
# include <stdlib.h>
# include <crtdbg.h>
# define GJS_DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__) 
# define new GJS_DEBUG_NEW 
#endif

#if defined _M_IX86
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
