#pragma once

#include "renderImpl.h"

class CLegacyRenderImpl : public CRender
{
protected:
    CComPtr<IGraphRenderer> m_graphRender;
    bool m_calculateExtent;
public:
    CLegacyRenderImpl(IGraph * graph, CMeta & meta);

    IRenderer * CreateISubgraphRenderer(IGraphSubgraph * sg);
    IRenderer * CreateIVertexRenderer(IGraphVertex * v);
    IRenderer * CreateIEdgeRenderer(IGraphEdge * e);
    LRESULT SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam); 
    void SetTopParent(HWND hwnd);

};



