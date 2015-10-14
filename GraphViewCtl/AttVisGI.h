#pragma once

#include "GraphTypes.h"

//*****************************************************************************
//
//	Interface to AT&T VisGraph data structs
//

namespace AttVisG
{

struct PolygonPoint
{
    double x, y;
    PolygonPoint() : x(0), y(0) {};
};

class Polygon
{
    PolygonPoint* m_points;
    unsigned m_count;

public:
    Polygon() : m_points(0), m_count(0) {};
    Polygon(const GraphTypes::PointF* points, unsigned count);
    Polygon(const GraphTypes::RectF& rc,
        GraphTypes::REAL marginx=0,
        GraphTypes::REAL marginy=0,
        bool ccw=true);
    ~Polygon() { free(m_points); m_count=0; }

    unsigned count() const { return m_count; }
    void* memory() { return m_points; }
    void transfer(void* memory, unsigned count);
    void GetPoint(GraphTypes::PointF& pt, unsigned index) const;
};

//*****************************************************************************

class NPolygon
{
    AttVisG::Polygon** m_polygons;
    unsigned m_count, m_curcount;
    bool m_autodelete;
public:
    NPolygon() : m_polygons(0), m_count(0), m_curcount(0), m_autodelete(false) {};
    NPolygon(unsigned count, bool autodelete=false); 
    ~NPolygon(); 

    unsigned Append(AttVisG::Polygon* poly);
    void Autodelete(bool autodelete) { m_autodelete=autodelete; }

    // just so we are forced to access data explicitly
    unsigned reserved() const { return m_count; }
    unsigned count() const { return m_curcount; }
    void* memory() { return m_polygons; }
};


}; // namespace