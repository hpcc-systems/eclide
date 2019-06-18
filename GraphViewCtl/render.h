#pragma once

#include "layout.h"
#include "Renderer.h"

enum HITTEST
{
    HITTEST_UNKNOWN,
    HITTEST_LABEL,
    HITTEST_ICON,
    HITTEST_IMAGE,
    HITTEST_BBOX
};

typedef std::pair<CUniqueID, HITTEST> VertexAt;

__interface IRender : public IUnknown
{
public:
    float getDraft() const;
    void setDraft(bool draft);

    IRenderer * CreateISubgraphRenderer(IGraphSubgraph * );
    IRenderer * CreateIVertexRenderer(IGraphVertex * );
    IRenderer * CreateIEdgeRenderer(IGraphEdge * );

    LRESULT SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void SetTopParent(HWND hwnd);

    VertexAt getVertexAt(const GraphTypes::PointF &p, const CUniqueID & overNode);
    VertexAt getVertexAt(const CPoint &p, const CUniqueID & overNode);
    void SelectItemsWithin(const GraphTypes::RectF &r);
    GraphTypes::RectF getGraphExtent(bool selectedOnly) const;

    float getScale() const;
    void setScale(float scale);
    float getSpacing() const;
    void setSpacing(float spacing);
    int getRotation() const;
    void setRotation(int rotation);
    int getDpiX() const;
    int getDpiY() const;
    void AllTransform(GraphTypes::PointF &p) const;
    void AllTransform(GraphTypes::RectF &p) const;
    void RAllTransform(GraphTypes::PointF &p) const;
    void RAllTransform(GraphTypes::RectF &p) const;
    void RDpiTransform(GraphTypes::RectF &p) const;

};

