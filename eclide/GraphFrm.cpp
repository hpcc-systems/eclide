#include "stdafx.h"
#include "resource.h"

using namespace boost;
using namespace Gdiplus;

#include "GraphFrm.h"

#define zoom 125
#define GETGATTR(y) (boost::lexical_cast<float>(m_graph->getAttr(y)) * zoom)
#define GETATTR(x, y) (boost::lexical_cast<float>(x->getAttr(y)) * zoom)
#define NEXT (long)(boost::lexical_cast<float>(*tok_iter++) * zoom)
	
