#pragma once

#include "GraphTypes.h"

//*****************************************************************************

bool IntersectRectCenter(
    const GraphTypes::RectF& rc,
    const GraphTypes::PointF& src,
    GraphTypes::PointF& intersect);

#ifdef __TEST__
void IntersectRectCenterTest();
#endif

bool LineIntersectsRect(
    const GraphTypes::PointF& p1,
    const GraphTypes::PointF& p2,
    const GraphTypes::RectF& vp);

static const GraphTypes::REAL EPSILON=1.0f/16.0f;

//*****************************************************************************
// class using general formula for line: ax + by + c =0 

struct GenLine 
{
    GraphTypes::REAL a;               /* x-coefficient */
    GraphTypes::REAL b;               /* y-coefficient */
    GraphTypes::REAL c;               /* constant term */

    GenLine(
        const GraphTypes::PointF& p1,
        const GraphTypes::PointF& p2);

    GenLine(
        const GraphTypes::PointF& p,
        GraphTypes::REAL slope);

    GraphTypes::REAL X(GraphTypes::REAL y) const;
    GraphTypes::REAL Y(GraphTypes::REAL x) const;
    bool IsVertical() const { return fabs(b)<EPSILON; }
    bool IsHorizontal() const { return fabs(a)<EPSILON; } 
    bool Parallel(const GenLine& l) const;
    bool IntersectionPoint(const GenLine& l, GraphTypes::PointF& p) const;
    bool ClosestPoint(
        const GraphTypes::PointF& ptest, GraphTypes::PointF& pc) const;
};

//*****************************************************************************
// Approximate (not using square root) Euclidean distance between two points
//
int idist(int x1, int y1, int x2, int y2);

//******************************************************************************
//
// Given 4 control points of Bezier curve find if test point lies on the curve
//
bool PointOnBezier4(const GraphTypes::PointF& p1,
                    const GraphTypes::PointF& p2,
                    const GraphTypes::PointF& p3,
                    const GraphTypes::PointF& p4,
                    const GraphTypes::PointF& test,
                    GraphTypes::REAL range=2.0);

//******************************************************************************
//
// Solutions for a*x^3 + b*x^2 + c*x + d = 0
// Return the number of real roots ( 1 or 3 )

int CubicSolver(double a, double b, double c, double d,
    double& x1, double& x2, double& x3);
