#pragma once

#include "GraphContainerImpl.h"

template<typename GraphData>
IGraphVertex * CreateIGraphVertex(GraphData & graph, IGraphSubgraph * parent, typename GraphData::vertex_descriptor vertex, const CUniqueID & id);

template<typename GraphData>
typename GraphData::vertex_descriptor GetVertexDescriptor(IGraphVertex * v);


