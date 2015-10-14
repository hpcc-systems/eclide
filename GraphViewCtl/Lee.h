#pragma once

#include "layoutImpl.h"

#ifdef LEE_LAYOUT
//-----------------------------------------------------------------------------
class CGraphLeeLayoutImpl : public CLayout
{
public:
    CGraphLeeLayoutImpl(IGraph * graph, CMeta & meta, CCanvas * canvas);
    LAYOUTEVENT pulse(const GraphTypes::RectF & extent);
    void halt();

    bool GetShortestPath( 
        const GraphTypes::PointF& start,
        const GraphTypes::PointF& end,
        GraphTypes::PointFVector& path );
};

//-----------------------------------------------------------------------------

class LeeRoutePlanner
{
protected:
    LeeRoutePlanner() {};
    ~LeeRoutePlanner() {};

public:
    static LeeRoutePlanner* Create(
        const GraphTypes::SizeF& gridbox=GraphTypes::SizeF(16.0,16.0),
        int border=4 );

    virtual void Dispose()=0;

    virtual void BeginAddPoints( void )=0;

    virtual void AddPoint(
        const GraphTypes::PointF& pt,
        GraphTypes::PointF* aligned_pt=0 )=0;	

    virtual bool EndAddPoints( void )=0;

    virtual bool GetShortestPath( 
        const GraphTypes::PointF& start,
        const GraphTypes::PointF& end,
        GraphTypes::PointFVector& path )=0;
};
#endif