#pragma once

#include "layoutImpl.h"
#include "Renderer.h"
#include <GraphTypes.h>
#include "AttVisGI.h"
#include "GraphViewCanvas.h"

class CCanvas;

enum NEATO_MODE
{
	NEATO_MODE_NEATO,
	NEATO_MODE_FDP,
	NEATO_MODE_TWOPI,
	NEATO_MODE_DOT,
	NEATO_MODE_CIRCO,
	NEATO_MODE_EDGES,
	NEATO_MODE_SUBGRAPH_TREE
};

class CGraphATTLayoutImpl : public CLayout
{
private:
	CGraphViewCanvas * m_view;
	StlLinked<CCanvas> m_canvas;
	bool m_bigHack;

	std::string & EncodeForDOT(const TCHAR *csv, std::string & encoded);
	void mergeSVG(const TCHAR* svg, IGraph* graph, NEATO_MODE mode);
	void AppendPos(IGraphItem* item, std::stringstream& dotStream, bool prependComma = true);
	void AppendSize(IGraphItem* item, std::stringstream& dotStream, bool prependComma = true);
	void AppendFont(IGraphItem* item, std::stringstream& dotStream);
	void foreach(IGraphItem* item, std::stringstream& dotStream, unsigned int depth);
	void foreach_GrETL(IGraphItem* item, std::stringstream& dotStream, unsigned int depth);
	void foreach_routeEdge(IGraphItem* item, std::stringstream& dotStream, unsigned int depth);
protected:
	NEATO_MODE m_mode;
public:
	CGraphATTLayoutImpl(CGraphViewCanvas * view, IGraph * graph, CCanvas* canvas, CMeta & meta, NEATO_MODE mode);
	LAYOUTEVENT pulse(const GraphTypes::RectF & extent);
	bool pulse2(const GraphTypes::RectF & extent);
	void halt();
};


