#pragma once

#include "libagraph.h"

#include <graph.h> // needed to cure VS7 linker error
#include <pathgeom.h> // required types for ShortestRoute

extern "C" LIBAGRAPH_API int shortest_route(Ppoly_t **polys, int num_polys, Ppoint_t * src, Ppoint_t * dst, Ppolyline_t* path);

