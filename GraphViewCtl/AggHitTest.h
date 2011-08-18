#pragma once

#include "GraphTypes.h"

class CAggHitTest
{
public:
	virtual void SetRect(const CRect& rc)=0;
	virtual void ClearCaches()=0;
	virtual void Transform(GraphTypes::REAL scale)=0;
	virtual void Transform(const GraphTypes::PointF& offset)=0; 
	virtual void TransformReset()=0;

	virtual bool LineHitTest(
		const GraphTypes::PointF& p1,
		const GraphTypes::PointF& p2, 
		const GraphTypes::PointF& ptest,
		double tolerance=0)=0;

	virtual bool SolidPolygonHitTest(
		const GraphTypes::PointF* points, 
		unsigned point_count,
		const GraphTypes::PointF& ptest,
		double tolerance=0)=0;

	virtual bool Bezier4HitTest(
		const GraphTypes::PointF& p1,
		const GraphTypes::PointF& p2,
		const GraphTypes::PointF& p3,
		const GraphTypes::PointF& p4,
		const GraphTypes::PointF& ptest,
		double tolerance=0.0,
		double width=1.0,
		bool bArrow=true,
		GraphTypes::REAL scale=1.0)=0;

	virtual bool BezierHitTest(
		const GraphTypes::PointFVector& points,
		const GraphTypes::PointF& ptest,
		double tolerance=0.0,
		double width=2.0,
		bool bArrow=true,
		GraphTypes::REAL scale=1.0)=0;

	virtual bool LollyPopHitTest(
		const GraphTypes::RectF& rc, 
		bool bLeft,
		const GraphTypes::PointF& ptest,
		double tolerance=0)=0;

	static CAggHitTest* CreateAggHitTest();
};
