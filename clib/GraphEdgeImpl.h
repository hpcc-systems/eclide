#pragma once

#include "GraphContainerImpl.h"

template<typename GraphData>
IGraphEdge * CreateIGraphEdge(GraphData & graph, IGraphVertex * source, IGraphVertex * target, const typename GraphData::edge_descriptor & edge, const CUniqueID & id);
