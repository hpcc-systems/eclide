#pragma once

#include "clib.h"
#include "UniqueID.h"
#include "GraphTypes.h"
#include "GraphContainer.h"

__interface IXMLWriter;
enum WUVisualState;

__interface __declspec(uuid("269E2234-EA73-4e22-8FCE-93E125B0AD6F")) IRenderer
    : public IUnknown
{
    CUniqueID GetID();
    void SetGraphItem(IGraphItem* item);
    void GetBoundsBox(GraphTypes::RectF& box, GraphTypes::Coordinates co=GraphTypes::screen);
    void SetBoundsBox(const GraphTypes::RectF& box, unsigned flags, GraphTypes::Coordinates co=GraphTypes::screen);
    void GetPosition(GraphTypes::PointF& pt, GraphTypes::Coordinates co=GraphTypes::world);
    void SetPosition(const GraphTypes::PointF& pt, GraphTypes::Coordinates co=GraphTypes::world);
    void SetSelected(bool bSelected=true, bool bAdd=false);
    void SetFocus(bool bSet=true);
    LRESULT SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    GraphTypes::SizeF CalcDisplaySizes(WTL::CDCHandle& dc, bool bForce=false, bool bIgnoreExtent=false);
    void SetRedraw(bool bEnable);
    void ClearCaches();
};

__interface __declspec(uuid("ACB1ECA6-D527-4a31-8041-126730A2CBA9")) IVertexRenderer
    : public IRenderer
{
    void GetTitlePos(GraphTypes::RectF& pos);
    void GetImagesBoundsBox(GraphTypes::RectF& box);
    bool IsSelected();
    bool GetPinned();
    bool IsHot() const;	
};

__interface __declspec(uuid("DC69DC4C-BDF3-45e8-A842-B160CDFB6430")) ILegacyRenderer
    : public IVertexRenderer
{
    void SetPinned(bool pinned);
    bool GetDragging();
    LRESULT HitTestDispatchMsg(const GraphTypes::PointF& point, bool& hit,
         UINT uMsg, WPARAM wParam, LPARAM lParam);
};

__interface __declspec(uuid("752E70E5-EDA9-40ce-97DB-AF90E119F233")) IGraphRenderer
    : public IRenderer
{
    bool GetFocus() const;
};

__interface __declspec(uuid("752E70E5-EDA9-40ce-97DB-AF90E119F233")) IOverviewRenderer
    : public IRenderer
{
    void SetInWindow(bool set); 
    bool CursorInViewport(const CPoint& p);
    void RSizeTransform(GraphTypes::PointF& point);
};

__interface __declspec(uuid("70680FBA-5127-46de-8F40-8F31EA66D683")) ILegacyGraphRenderer
    : public IGraphRenderer
{
};

__interface __declspec(uuid("B46133B3-B8CB-48ed-AC38-BAF914906DDA")) ITimelineGraphRenderer
    : public IGraphRenderer
{
};


__interface __declspec(uuid("A53D950D-9D72-4b5a-B09E-51447FCDEF18")) IEdgeRenderer 
    : public IRenderer
{
    bool GetConnectionPoints(GraphTypes::PointF& src, GraphTypes::PointF& tgt, GraphTypes::Coordinates co=GraphTypes::screen, GraphTypes::PointF* srcoff=0, GraphTypes::PointF* tgtoff=0);
    unsigned GetPoints(GraphTypes::PointFVector& vp, GraphTypes::Coordinates co=GraphTypes::world);
    void SetPoints(const GraphTypes::PointFVector& vp, GraphTypes::Coordinates co=GraphTypes::world);
    void GetLabelPos(GraphTypes::RectF& pos, GraphTypes::Coordinates co=GraphTypes::screen);
    void SetLabelPos(const GraphTypes::RectF& pos, GraphTypes::Coordinates co=GraphTypes::screen);
    bool IsSelected();
};

__interface __declspec(uuid("A53D950D-9D72-4b5a-B09E-51447FCDEE29")) ILegacyEdgeRenderer 
    : public IEdgeRenderer
{
};

__interface __declspec(uuid("ACBCC59B-B488-4fa9-A702-AC50638A1F17")) IAttEdgeRenderer 
    : public IEdgeRenderer
{
    bool IntersectsViewport() const;
};

__interface __declspec(uuid("02649D5A-C96A-4cba-A5F5-8C1DB0C86396")) ITimelineEdgeRenderer 
    : public IEdgeRenderer
{
    void GetFromPosition(GraphTypes::PointF& pt, GraphTypes::Coordinates co=GraphTypes::world);
    void SetFromPosition(const GraphTypes::PointF& pt, GraphTypes::Coordinates co=GraphTypes::world);
    void GetToPosition(GraphTypes::PointF& pt, GraphTypes::Coordinates co=GraphTypes::world);
    void SetToPosition(const GraphTypes::PointF& pt, GraphTypes::Coordinates co=GraphTypes::world);
    bool IsHot();
};

__interface __declspec(uuid("EBC37C74-2D44-428e-AD6F-EA46F7E0015C")) IAttVertexRenderer 
    : public IVertexRenderer
{
};

__interface __declspec(uuid("DAE832F8-25C6-45e1-AC35-0A4A49F537B8")) ITextboxRenderer 
    : public IRenderer
{
    enum eIOMode
    {
        iomIn,  
        iomOut,
        iomBoth
    };

    bool IsHot();
    bool AcceptEdge(IGraphItem* from);
    void SetIOMode(eIOMode iom);
    eIOMode GetIOMode();
};


__interface __declspec(uuid("BBE2AB1F-6BB4-40e2-80A3-A687EF572B45")) IListWindowRenderer 
    : public IRenderer
{
    enum WINDOW_SIZE
    {
        WINDOW_SIZE_MIN,
        WINDOW_SIZE_NORM,
        WINDOW_SIZE_MAX
    };

    void GetContainerRect(GraphTypes::RectF& rc) const;
    IGraphItem* HitTestChildren(const GraphTypes::PointF& point);
    void GetConnectBounds(GraphTypes::RectF& left, GraphTypes::RectF& right, unsigned order);
    bool IsSelected();
    bool IsMinimized() const;
    void OnRemoveGraphItem(IGraphItem* gi);
    bool OnEditItem(IGraphItem* gi);
    void SetWindowState(WINDOW_SIZE size=WINDOW_SIZE_NORM, int topindex=0);
    void GetWindowState(WINDOW_SIZE& size, int& topindex) const;
    void SetWindowSize(const GraphTypes::SizeF& size);
    void GetWindowSize(GraphTypes::SizeF& size) const;
};

__interface __declspec(uuid("5585486D-1034-492f-8E76-47CACB02A14D")) IDatasetRenderer 
    : public IListWindowRenderer
{
};

__interface __declspec(uuid("A7A81CF4-3166-44a4-B969-23202D33BB91")) IIOSubgraphRenderer
    : public IListWindowRenderer
{
};

__interface __declspec(uuid("0DA5218D-2584-44ea-9424-5AEB267F3DA5")) IAttSubgraphRenderer
    : public IListWindowRenderer
{
    LRESULT HitTestDispatchMsg(const GraphTypes::PointF& point, bool& hit,
         UINT uMsg, WPARAM wParam, LPARAM lParam);
};

__interface __declspec(uuid("48C7AF36-C1AA-44c4-993C-F55D661E9BED")) ILegacySubgraphRenderer 
    : public IListWindowRenderer
{
    LRESULT HitTestDispatchMsg(const GraphTypes::PointF& point, bool& hit,
         UINT uMsg, WPARAM wParam, LPARAM lParam);
    void GetTitlePos(GraphTypes::RectF& pos);
    void GetImagesBoundsBox(GraphTypes::RectF& box);
    //TODO isolate this into pinnable interface otherwise code must
    //test both vertex and subgraph to ask for pinned state
    bool GetPinned();
    void SetPinned(bool pinned);
};

__interface __declspec(uuid("A9D327C3-31F6-4a97-81DB-D8B3ECA9742D")) IArrowTipRenderer 
    : public IRenderer
{
};

__interface __declspec(uuid("A87D501A-7FBC-49a2-826D-DC4FFE7C49A2")) IArrowTailRenderer 
    : public IRenderer
{
};

__interface __declspec(uuid("394371AA-5A46-4d14-B9C7-AEFDEDF5396D")) IImageRenderer 
    : public IRenderer
{
    LRESULT HitTestDispatchMsg(const GraphTypes::PointF& point, bool& hit,
         UINT uMsg, WPARAM wParam, LPARAM lParam);
    bool IsSelected() const;
    bool HasCapture() const;
    void SetParentRenderer(IRenderer* parent);
};