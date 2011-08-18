#include "StdAfx.h"

#include "AttVisGI.h"

using namespace AttVisG;
using namespace GraphTypes;

//-----------------------------------------------------------------------------

Polygon::Polygon(const PointF* points, unsigned count) : m_count(count)
{
	m_points=static_cast<PolygonPoint*>(malloc(sizeof(PolygonPoint)*count));

	for(unsigned i=0; i<count; ++i)
	{
		m_points[i].x=points[i].x;
		m_points[i].y=points[i].y;
	}
}

//-----------------------------------------------------------------------------

Polygon::Polygon(const RectF& rc,
	REAL marginx/*=0*/,
	REAL marginy/*=0*/,
	bool ccw/*=true*/)
	: m_count(4)
{
	m_points=static_cast<PolygonPoint*>(malloc(sizeof(PolygonPoint)*4));
	if(ccw)
	{
		m_points[0].x=rc.x-marginx;
		m_points[0].y=rc.y-marginy;

		m_points[1].x=rc.x-marginx;
		m_points[1].y=rc.y+rc.Height+marginy;

		m_points[2].x=rc.x+rc.Width+marginx;
		m_points[2].y=rc.y+rc.Height+marginy;

		m_points[3].x=rc.x+rc.Width+marginx;
		m_points[3].y=rc.y-marginy;
	}
	else
	{
		m_points[0].x=rc.x-marginx;
		m_points[0].y=rc.y-marginy;

		m_points[1].x=rc.x+rc.Width+marginx;
		m_points[1].y=rc.y-marginy;

		m_points[2].x=rc.x+rc.Width+marginx;
		m_points[2].y=rc.y+rc.Height+marginy;

		m_points[3].x=rc.x-marginx;
		m_points[3].y=rc.y+rc.Height+marginy;
	}
}

//-----------------------------------------------------------------------------

void Polygon::transfer(void* memory, unsigned count)
{
	if(m_points)
		free(m_points);
	m_points=static_cast<PolygonPoint*>(memory);
	m_count=count;
}

//-----------------------------------------------------------------------------

void Polygon::GetPoint(GraphTypes::PointF& pt, unsigned index) const
{
	if(index<m_count)
	{
		pt.x=static_cast<REAL>(m_points[index].x);
		pt.y=static_cast<REAL>(m_points[index].y);
	}
}

//*****************************************************************************

NPolygon::NPolygon(unsigned count, bool autodelete/*=false*/)
	: m_count(count), m_curcount(0), m_autodelete(autodelete) 
{
	m_polygons=static_cast<Polygon**>(malloc(sizeof(Polygon*)*count));
}

//-----------------------------------------------------------------------------

NPolygon::~NPolygon() 
{ 
	if(m_polygons && m_autodelete)
	{
		for(unsigned i=0; i<m_curcount; ++i)
			delete m_polygons[i];
	}

	free(m_polygons); 
	m_count=m_curcount=0; 
}

//-----------------------------------------------------------------------------

unsigned NPolygon::Append(AttVisG::Polygon* poly)
{
	ATLASSERT(m_polygons);
	ATLASSERT(m_curcount<m_count);
	m_polygons[m_curcount]=poly;
	return ++m_curcount;
}

