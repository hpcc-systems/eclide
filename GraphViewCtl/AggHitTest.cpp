#include "stdafx.h"
#include "resource.h"

#include <atlmisc.h>
#include "GraphTypes.h"
#include "Canvas.h"
#include "AggCommon.h"
#include "AggHitTest.h"

//*****************************************************************************

typedef agg::rasterizer_scanline_aa<> rasterizer_aa;

//*****************************************************************************

class CAggHitTestImpl : public CAggHitTest
{
	agg::path_storage m_path;
    agg::trans_affine m_mtx;
	conv_path_trans_type m_transpath;
	rasterizer_aa m_ras_aa;
	CRect m_rcUpdate;
public:
	CAggHitTestImpl();

	void SetRect(const CRect& rc);
	void ClearCaches();
	void Transform(GraphTypes::REAL scale);
	void Transform(const GraphTypes::PointF& offset); 
	void TransformReset() { m_mtx.reset(); }

	bool LineHitTest(
		const GraphTypes::PointF& p1,
		const GraphTypes::PointF& p2, 
		const GraphTypes::PointF& ptest,
		double tolerance=0);

	bool SolidPolygonHitTest(
		const GraphTypes::PointF* points, 
		unsigned point_count,
		const GraphTypes::PointF& ptest,
		double tolerance=0);

	bool Bezier4HitTest(
		const GraphTypes::PointF& p1,
		const GraphTypes::PointF& p2,
		const GraphTypes::PointF& p3,
		const GraphTypes::PointF& p4,
		const GraphTypes::PointF& ptest,
		double tolerance=0.0,
		double width=1.0,
		bool bArrow=true,
		GraphTypes::REAL scale=1.0);

	bool CAggHitTestImpl::BezierHitTest(
		const GraphTypes::PointFVector& points,
		const GraphTypes::PointF& ptest,
		double tolerance=0.0,
		double width=2.0,
		bool bArrow=true,
		GraphTypes::REAL scale=1.0);

	bool LollyPopHitTest(
		const GraphTypes::RectF& rc, 
		bool bLeft,
		const GraphTypes::PointF& ptest,
		double tolerance=0);
};

//*****************************************************************************

using namespace GraphTypes;

//*****************************************************************************

CAggHitTestImpl::CAggHitTestImpl()
	: m_transpath(m_path, m_mtx)
{
	m_rcUpdate.SetRectEmpty();	
}

//*****************************************************************************

CAggHitTest* CAggHitTest::CreateAggHitTest()
{
	CAggHitTest* agght;

	agght=new CAggHitTestImpl();

	return agght;
}

//-----------------------------------------------------------------------------

void CAggHitTestImpl::SetRect(const CRect& rc)
{
	m_rcUpdate=rc;	
	m_ras_aa.clip_box(0, 0, m_rcUpdate.Width(), m_rcUpdate.Height()); 
}

//-----------------------------------------------------------------------------

void CAggHitTestImpl::ClearCaches()
{
	m_path.remove_all();
	m_ras_aa.reset();
}

//-----------------------------------------------------------------------------

void CAggHitTestImpl::Transform(REAL scale)
{
	m_mtx *= agg::trans_affine_scaling(scale);
}

//-----------------------------------------------------------------------------

void CAggHitTestImpl::Transform(const PointF& offset)
{
	m_mtx *= agg::trans_affine_translation(offset.x, offset.y);
}

//-----------------------------------------------------------------------------

bool CAggHitTestImpl::LineHitTest(
	const GraphTypes::PointF& p1,
	const GraphTypes::PointF& p2, 
	const GraphTypes::PointF& ptest,
	double tolerance/*=0*/)
{
	m_path.move_to(p1.x, p1.y);
	m_path.line_to(p2.x, p2.y);

	typedef agg::conv_stroke<conv_path_trans_type> conv_trans_stroke_outline;
	conv_trans_stroke_outline stroke(m_transpath);
	stroke.width(tolerance);
	m_ras_aa.add_path(stroke);
	return m_ras_aa.hit_test(round_int(ptest.x), round_int(ptest.y));
}

//-----------------------------------------------------------------------------

bool CAggHitTestImpl::SolidPolygonHitTest(
	const PointF* points, 
	unsigned point_count,
	const PointF& ptest,
	double tolerance/*=0*/)
{
	assert(point_count>0);

	m_path.move_to(points->x, points->y);
	while(--point_count>0)
	{
		points++;
		m_path.line_to(points->x, points->y);
	}
	m_path.close_polygon();

	typedef agg::conv_stroke<conv_path_trans_type> conv_trans_stroke_outline;
	conv_trans_stroke_outline stroke(m_transpath);
	stroke.width(tolerance);
	m_ras_aa.add_path(stroke);
	return m_ras_aa.hit_test(round_int(ptest.x), round_int(ptest.y));
}

//-----------------------------------------------------------------------------

bool CAggHitTestImpl::Bezier4HitTest(
	const GraphTypes::PointF& p1,
	const GraphTypes::PointF& p2,
	const GraphTypes::PointF& p3,
	const GraphTypes::PointF& p4,
	const GraphTypes::PointF& ptest,
	double tolerance/*=0.0*/,
	double width/*=1.0*/,
	bool bArrow/*=true*/,
	GraphTypes::REAL scale/*=1.0*/)
{
	agg::curve4 curve(
		p1.x, p1.y,
		p2.x, p2.y,
		p3.x, p3.y,
		p4.x, p4.y);

    typedef agg::conv_stroke<agg::curve4, agg::vcgen_markers_term> conv_stroke_type;
    conv_stroke_type stroke(curve);
	stroke.width(width+tolerance);

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

	return m_ras_aa.hit_test(round_int(ptest.x), round_int(ptest.y));
}

//-----------------------------------------------------------------------------

bool CAggHitTestImpl::BezierHitTest(
	const GraphTypes::PointFVector& points,
	const GraphTypes::PointF& ptest,
	double tolerance/*=0.0*/,
	double width/*=2.0*/,
	bool bArrow/*=true*/,
	REAL scale/*=1.0*/)
{
    agg::curve_casteljau curve(points.size());
    curve.approximation_scale(scale);
	GraphTypes::PointFVector::const_iterator itr=points.begin();
	for(; itr!=points.end();++itr)
	{
        curve.add_vertex((*itr).x, (*itr).y);
    }

    typedef agg::conv_stroke<agg::curve_casteljau, agg::vcgen_markers_term> conv_stroke_type;
    conv_stroke_type stroke(curve);
	stroke.width(width+tolerance);

	if(bArrow)
		AddPathWithArrowHead(m_ras_aa, stroke, m_mtx, 10, 3, 2, scale);
	else
	{
		typedef agg::conv_transform<conv_stroke_type, agg::trans_affine> conv_trans_type;
		conv_trans_type transstroke(stroke, m_mtx);
		m_ras_aa.add_path(transstroke);
	}

	return m_ras_aa.hit_test(round_int(ptest.x), round_int(ptest.y));
}

//-----------------------------------------------------------------------------

bool CAggHitTestImpl::LollyPopHitTest(
	const RectF& rc, 
	bool bLeft,
	const PointF& ptest,
	double tolerance/*=0*/)
{
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

	m_path.move_to(p1.x, p1.y);
	m_path.line_to(p2.x, p2.y);

	m_ras_aa.add_path(m_transcircle);
	m_ras_aa.add_path(m_transpath);
	return m_ras_aa.hit_test(round_int(ptest.x), round_int(ptest.y));
}

