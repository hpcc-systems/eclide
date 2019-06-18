#include "stdafx.h"

#include "Geometry.h"
#include "agg_basics.h"

using namespace GraphTypes;

//*****************************************************************************
//
// Finds intersection between line and rectangle's outline. Line starts at
// point src, with endpoint in rectangle's center. The speedup is in finding
// outside point code which tells us which rectangle edges need to be tested
// for intersection, instead of calculating intersection with all 4 edges.
// This can be generalized to any endpoint inside rectangle, not just the center.
// NOTE: we are reusing Agg and Agg uses y positive up coordinate system

bool IntersectRectCenter(
    const RectF& rc,
    const PointF& src,
    PointF& intersect)
{
    RectF fliprc(rc.x, -(rc.y+rc.Height), rc.Width, rc.Height);
    PointF rcborder1, rcborder2; // first line segment to test
    PointF rcborder1a, rcborder2a; // second to test if bTwo=true
    bool bIntersect=true;
    bool bTwo=false;
    agg::rect_base<REAL> cliprc(fliprc.x, fliprc.y, fliprc.x+fliprc.Width, fliprc.y+fliprc.Height);

    //----------------------------------------------------------clipping_flags
    // Determine the clipping code of the vertex according to the 
    // Cyrus-Beck line clipping algorithm
    //
    //        |        |
    //  0110  |  0010  | 0011
    //        |        |
    // -------+--------+-------- clip_box.y2
    //        |        |
    //  0100  |  0000  | 0001
    //        |        |
    // -------+--------+-------- clip_box.y1
    //        |        |
    //  1100  |  1000  | 1001
    //        |        |
    //  clip_box.x1  clip_box.x2

    unsigned clipcode=agg::clipping_flags<REAL>(src.x, -(src.y), cliprc);

    switch(clipcode)
    {
    case 1:	// only right border line can be intersected
        {
            rcborder1.x=rcborder2.x=fliprc.x+fliprc.Width;
            rcborder1.y=fliprc.y;
            rcborder2.y=fliprc.y+fliprc.Height;
        }
        break;

    case 4: // only left border line can be intersected
        {
            rcborder1.x=rcborder2.x=rc.x;
            rcborder1.y=fliprc.y;
            rcborder2.y=fliprc.y+fliprc.Height;
        }
        break;


    case 2: // top line
        {
            rcborder1.x=fliprc.x;
            rcborder2.x=fliprc.x+fliprc.Width;
            rcborder1.y=rcborder2.y=fliprc.y+fliprc.Height;
        }
        break;

    case 8: // bottom line
        {
            rcborder1.x=fliprc.x;
            rcborder2.x=fliprc.x+fliprc.Width;
            rcborder1.y=rcborder2.y=fliprc.y;
        }
        break;

    case 3: // top right
        {
            bTwo=true;

            rcborder1.x=fliprc.x;
            rcborder2.x=fliprc.x+fliprc.Width;
            rcborder1.y=rcborder2.y=fliprc.y+fliprc.Height;

            rcborder1a.x=rcborder2a.x=fliprc.x+fliprc.Width;
            rcborder1a.y=fliprc.y;
            rcborder2a.y=fliprc.y+fliprc.Height;
        }
        break;

    case 9: // bottom right
        {
            bTwo=true;

            rcborder1.x=fliprc.x;
            rcborder2.x=fliprc.x+fliprc.Width;
            rcborder1.y=rcborder2.y=fliprc.y;

            rcborder1a.x=rcborder2a.x=fliprc.x+fliprc.Width;
            rcborder1a.y=fliprc.y;
            rcborder2a.y=fliprc.y+fliprc.Height;
        }
        break;

    case 6: // top left
        {
            bTwo=true;

            rcborder1.x=fliprc.x;
            rcborder2.x=fliprc.x+fliprc.Width;
            rcborder1.y=rcborder2.y=fliprc.y+fliprc.Height;

            rcborder1a.x=rcborder2a.x=fliprc.x;
            rcborder1a.y=fliprc.y;
            rcborder2a.y=fliprc.y+fliprc.Height;
        }
        break;

    case 12: // bottom left
        {
            bTwo=true;

            rcborder1.x=fliprc.x;
            rcborder2.x=fliprc.x+fliprc.Width;
            rcborder1.y=rcborder2.y=fliprc.y;

            rcborder1a.x=rcborder2a.x=fliprc.x;
            rcborder1a.y=fliprc.y;
            rcborder2a.y=fliprc.y+fliprc.Height;
        }
        break;

    default:
        bIntersect=false;
        break;
    }

    if(bIntersect)
    {
        double ix, iy;
        PointF center(fliprc.x+fliprc.Width/2, fliprc.y+fliprc.Height/2);

        bIntersect=agg::calc_intersection(
            src.x, -(src.y), center.x, center.y,
            rcborder1.x, rcborder1.y, rcborder2.x, rcborder2.y,
            &ix, &iy);

        if(!bIntersect && bTwo)
        {
            bIntersect=agg::calc_intersection(
                src.x, -(src.y), center.x, center.y,
                rcborder1a.x, rcborder1a.y, rcborder2a.x, rcborder2a.y,
                &ix, &iy);
        }

        if(bIntersect)
        {
            intersect.x=static_cast<REAL>(ix);
            intersect.y=static_cast<REAL>(-(iy));
        }
    }

    return bIntersect;
}

//-----------------------------------------------------------------------------

#ifdef __TEST__
void IntersectRectCenterTest()
{
    RectF rc(10, 10, 10, 10);
    PointF src(15, 5); // top
    PointF intersect;

    bool bRet=IntersectRectCenter(rc, src, intersect);

    src.x=5;
    src.y=15; // left

    bRet&=IntersectRectCenter(rc, src, intersect);

    src.x=25;
    src.y=15; // right

    bRet&=IntersectRectCenter(rc, src, intersect);

    src.x=15;
    src.y=25; // bottom

    bRet&=IntersectRectCenter(rc, src, intersect);
}

#endif // __TEST__

//-----------------------------------------------------------------------------

bool LineIntersectsRect(const PointF& p1, const PointF& p2, const RectF& vp)
{
    bool itdoes=true;
    // determine if edge intersects or is contained within the viewport
    // using AGG requires us to switch to coordinate system where Y increments upward
    RectF fliprc(vp.x, -(vp.y+vp.Height), vp.Width, vp.Height);
    agg::rect_base<REAL> cliprc(fliprc.x, fliprc.y, fliprc.x+fliprc.Width, fliprc.y+fliprc.Height);

    //----------------------------------------------------------clipping_flags
    // Determine the clipping code of the point according to the 
    // Cyrus-Beck line clipping algorithm
    //
    //        |        |
    //  0110  |  0010  | 0011
    //        |        |
    // -------+--------+-------- clip_box.y2
    //        |        |
    //  0100  |  0000  | 0001
    //        |        |
    // -------+--------+-------- clip_box.y1
    //        |        |
    //  1100  |  1000  | 1001
    //        |        |
    //  clip_box.x1  clip_box.x2

    unsigned clipcode1=agg::clipping_flags<REAL>(p1.x, -p1.y, cliprc);
    unsigned clipcode2=agg::clipping_flags<REAL>(p2.x, -p2.y, cliprc);
    unsigned res=clipcode1 & clipcode2;

    if(res!=0) // no way edge can cross the rectangle (viewport)
    {
        itdoes=false;
    }
    else if(clipcode1!=0 && clipcode2!=0) // clipcode 0 implies whole or part of edge is visible
    {
        unsigned orcode=clipcode1 | clipcode2;
        // edges with ends in opposite octants are clearly visible so skip them
        if(orcode!=0xA /*1010*/&& orcode!=5/*101*/)
        {
            bool bIntersect=false;
            bool bTwo=false;

            PointF rcborder1, rcborder2; // first line segment to test
            PointF rcborder1a, rcborder2a; // second to test if bTwo=true
    
            switch(clipcode1)
            {
            case 1:	// only right border line intersection need be considered
                {
                    rcborder1.x=rcborder2.x=fliprc.x+fliprc.Width;
                    rcborder1.y=fliprc.y;
                    rcborder2.y=fliprc.y+fliprc.Height;
                }
                break;

            case 4: // only left border line need be considered
                {
                    rcborder1.x=rcborder2.x=fliprc.x;
                    rcborder1.y=fliprc.y;
                    rcborder2.y=fliprc.y+fliprc.Height;
                }
                break;


            case 2: // top line
                {
                    rcborder1.x=fliprc.x;
                    rcborder2.x=fliprc.x+fliprc.Width;
                    rcborder1.y=rcborder2.y=fliprc.y+fliprc.Height;
                }
                break;

            case 8: // bottom line
                {
                    rcborder1.x=fliprc.x;
                    rcborder2.x=fliprc.x+fliprc.Width;
                    rcborder1.y=rcborder2.y=fliprc.y;
                }
                break;

            case 3: // top right
                {
                    bTwo=true;

                    rcborder1.x=fliprc.x;
                    rcborder2.x=fliprc.x+fliprc.Width;
                    rcborder1.y=rcborder2.y=fliprc.y+fliprc.Height;

                    rcborder1a.x=rcborder2a.x=fliprc.x+fliprc.Width;
                    rcborder1a.y=fliprc.y;
                    rcborder2a.y=fliprc.y+fliprc.Height;
                }
                break;

            case 9: // bottom right
                {
                    bTwo=true;

                    rcborder1.x=fliprc.x;
                    rcborder2.x=fliprc.x+fliprc.Width;
                    rcborder1.y=rcborder2.y=fliprc.y;

                    rcborder1a.x=rcborder2a.x=fliprc.x+fliprc.Width;
                    rcborder1a.y=fliprc.y;
                    rcborder2a.y=fliprc.y+fliprc.Height;
                }
                break;

            case 6: // top left
                {
                    bTwo=true;

                    rcborder1.x=fliprc.x;
                    rcborder2.x=fliprc.x+fliprc.Width;
                    rcborder1.y=rcborder2.y=fliprc.y+fliprc.Height;

                    rcborder1a.x=rcborder2a.x=fliprc.x;
                    rcborder1a.y=fliprc.y;
                    rcborder2a.y=fliprc.y+fliprc.Height;
                }
                break;

            case 12: // bottom left
                {
                    bTwo=true;

                    rcborder1.x=fliprc.x;
                    rcborder2.x=fliprc.x+fliprc.Width;
                    rcborder1.y=rcborder2.y=fliprc.y;

                    rcborder1a.x=rcborder2a.x=fliprc.x;
                    rcborder1a.y=fliprc.y;
                    rcborder2a.y=fliprc.y+fliprc.Height;
                }
                break;

            default:
                ATLASSERT(0);
                break;
            }

            double ix, iy;

            bIntersect=agg::calc_intersection(
                p1.x, -p1.y, p2.x, p2.y,
                rcborder1.x, rcborder1.y, rcborder2.x, rcborder2.y,
                &ix, &iy);

            if(!bIntersect && bTwo)
            {
                bIntersect=agg::calc_intersection(
                    p1.x, -p1.y, p2.x, p2.y,
                    rcborder1a.x, rcborder1a.y, rcborder2a.x, rcborder2a.y,
                    &ix, &iy);
            }

            itdoes=bIntersect;

        } // two opcodes not in opposite octants
    } // none of opcodes is 0

    return itdoes;
}

//******************************************************************************
// General line
// We establish a canonical line representation by insisting that 
// the y-coefficient b equal 1 if it is non-zero. Otherwise, we set
// the x-coefficient a to 1:

GenLine::GenLine(const PointF& p1, const PointF& p2)
{
    if(p1.x==p2.x)
    {
        a = 1;
        b = 0;
        c = -p1.x;
    } 
    else
    {
        b = 1;
        a = -(p1.y-p2.y)/(p1.x-p2.x);
        c = -(a * p1.x) - (b * p1.y);
    }
}

//------------------------------------------------------------------------------

GenLine::GenLine(const PointF& p, REAL slope)
{
    a = -slope;
    b = 1;
    c = -(a * p.x) - (b * p.y);
}

//------------------------------------------------------------------------------

REAL GenLine::X(REAL y) const
{
    REAL x;

    // ax+by+c=0;

    if(b==1)
        x=-((c+y)/a);
    else
        x=-c; // vertical line

    return x;
}

//------------------------------------------------------------------------------

REAL GenLine::Y(REAL x) const
{
    REAL y;

    // ax+by+c=0;

    if(b==1)
        y=-(c+a*x);
    else
        y=0; // vertical line

    return y;
}

//------------------------------------------------------------------------------

bool GenLine::Parallel(const GenLine& l) const
{
    return ( (fabs(l.a-a) <= EPSILON) &&
        (fabs(l.b-b) <= EPSILON) );
}

//------------------------------------------------------------------------------

bool GenLine::IntersectionPoint(const GenLine& l, PointF& p) const
{
    //     if (same_lineQ(l1,l2)) {
    //       printf("Warning: Identical lines, all points intersect.\n");
    //         p[X] = p[Y] = 0.0;
    //         return;
    //     }

    if(Parallel(l)==true)
        return false;

    p.x = (l.b * c - b * l.c) / (l.a * b - a * l.b);

    if (fabs(b) > EPSILON)       /* test for vertical line */
        p.y = -(a * p.x + c) / b;
    else
        p.y = -(l.a * p.x + l.c) / l.b;

    return true;
}

//------------------------------------------------------------------------------

bool GenLine::ClosestPoint(const PointF& ptest, PointF& pc) const
{
    if (fabs(b) <= EPSILON) 
    {     
        /* vertical line */
        pc.x = -c;
        pc.y = ptest.y;
        return true;
    }

    if (fabs(a) <= EPSILON) 
    {   
        /* horizontal line */
        pc.x = ptest.x;
        pc.y = -c;
        return true;
    }

    GenLine perp(ptest, 1/a); // perpendicular to this line
    return IntersectionPoint(perp, pc);
}

//******************************************************************************
//
// Approximate (not using square root) Euclidean distance between two points
//
int idist(int x1, int y1, int x2, int y2)
{
    if((x2-=x1)<0) x2=-x2;
    if((y2-=y1)<0) y2=-y2;
    return (x2+y2-(((x2>y2) ? y2 : x2) >> 1));
}

//******************************************************************************

bool PointOnBezier4(const PointF& p1, const PointF& p2, const PointF& p3,
    const PointF& p4, const PointF& test, REAL range/*=2.0*/)
{
    bool onCurve=false;

    // we should be finding convex hull but currently we rely on the
    // fact that Bezier is always S-shaped between p1 and p4 so we
    // check for a rectangle bounded by these two points
    RectF chull;

    if(p1.x<=p4.x)
    {
        chull.x=p1.x;
        chull.Width=p4.x-p1.x;
    }
    else
    {
        chull.x=p4.x;
        chull.Width=p1.x-p4.x;
    }

    if(p1.y<=p4.y)
    {
        chull.y=p1.y;
        chull.Height=p4.y-p1.y;
    }
    else
    {
        chull.y=p4.y;
        chull.Height=p1.y-p4.y;
    }

    if(chull.Contains(test))
    {
    // Get cubic equation coefficients from control points
    //		| 1  0  0 0 |
    // MB =	| -3 3  0 0 |
    //		| 3 -6  3 0 |
    //		| -1 3 -3 1 |

        double a=-1*p1.x + 3*p2.x - 3*p3.x + p4.x;
        double b=3*p1.x - 6*p2.x + 3*p3.x;
        double c=-3*p1.x + 3*p2.x;
        double d=p1.x-test.x;

        double t[3];
        int roots=CubicSolver(a, b, c, d, t[0], t[1], t[2]);

        a=-1*p1.y + 3*p2.y - 3*p3.y + p4.y;
        b=3*p1.y - 6*p2.y + 3*p3.y;
        c=-3*p1.y + 3*p2.y;
        d=p1.y;

        double sqt;
        double fy;
        double diff;

        for(int i=0; i<roots; ++i)
        {
            if(roots==1 || (t[i]>=0&&t[i]<=1))
            {
                sqt=t[i]*t[i];
                fy=a*t[i]*sqt + b*sqt + c*t[i] + d;
                diff=fabs(fy-test.y);
#ifdef _DEBUG
//				AtlTrace("Cubic root closeness: %f\r\n", diff);
#endif
                if(diff<range)
                {
                    onCurve=true;
                    return onCurve;
                }
            }
        }
    }

    return onCurve;
}

//******************************************************************************
//
// Solutions for a*x^3 + b*x^2 + c*x + d = 0

int CubicSolver(double a, double b, double c, double d,
    double& x1, double& x2, double& x3)
{
    double a1 = b/a;
    double a2 = c/a;
    double a3 = d/a;

    double Q = (a1 * a1 - 3 * a2) / 9;
    double R = (2 * a1 * a1 * a1 - 9 * a1 * a2 + 27 * a3) / 54;
    double Qcubed = Q * Q * Q;
    double det = Qcubed - R * R;

    /* Three real roots */
    if(det >= 0) 
    {
        REAL theta = static_cast<REAL>(acos(R / sqrt(Qcubed)));
        REAL sqrtQ = static_cast<REAL>(sqrt(Q));
        x1 = -2 * sqrtQ * cos( theta           / 3) - a1 / 3;
        x2 = -2 * sqrtQ * cos((theta + 2 * M_PI) / 3) - a1 / 3;
        x3 = -2 * sqrtQ * cos((theta + 4 * M_PI) / 3) - a1 / 3;
        return 3;
    }
    else 	/* One real root */
    {
        double e = pow(sqrt(-det) + fabs(R), 1. / 3.);
        if (R > 0)
            e = -e;
        x1 = (e + Q / e) - a1 / 3.;
        return 1;
    }
}