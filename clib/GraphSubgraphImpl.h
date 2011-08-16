#pragma once

#include "GraphContainerImpl.h"

template<typename GraphData>
IGraphSubgraph * CreateIGraphSubgraph(GraphData & graph, IGraphSubgraph * parent, const CUniqueID & id);

