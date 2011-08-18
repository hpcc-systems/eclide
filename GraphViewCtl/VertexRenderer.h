#pragma once

#include "clib.h"
#include "GraphContainer.h"
#include "GraphTypes.h"
#include "Transform.h"

class CGraphFastRenderImpl;

__interface __declspec(uuid("269E2234-EA73-4e22-8FCE-93E125B0AD6F")) IRenderer
	: public IUnknown
{
	CUniqueID GetID();

	void GetBoundsBox(GraphTypes::RectF& box);

	const GraphTypes::RectF & GetTitlePos();
	void SetTitlePos(const GraphTypes::RectF & r);
	const GraphTypes::RectF & GetIconPos();
	void SetIconPos(const GraphTypes::RectF & r);
	const GraphTypes::RectF & GetImagePos();
	void SetImagePos(const GraphTypes::RectF & r);
	bool IsVisible();
	bool IsSelected();
	void PaintNormVertex(CDCHandle &dc);
	void PaintSelVertex(CDCHandle &dc);
	void PaintOverVertex(CDCHandle &dc);
	void SetFocus(bool bSet=true);

	LRESULT SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	GraphTypes::SizeF CalcDisplaySizes(CDCHandle& dc, bool bForce=false, bool bIgnoreExtent=false);
};

__interface __declspec(uuid("2D6BB81C-810B-44b6-97D8-FB33A098DFBE")) IBaseRenderer
	: public IRenderer
{
};


__interface __declspec(uuid("DC69DC4C-BDF3-45e8-A842-B160CDFB6430")) ILegacyRenderer
	: public IRenderer
{
	void SetBoundsBox(const GraphTypes::RectF& box, unsigned flags);
	void SetSelected(bool bSelected=true);
	void GetTitlePos(GraphTypes::RectF& pos);
	void GetIconPos(GraphTypes::RectF& pos);
	void GetImagePos(GraphTypes::RectF& pos);
};

__interface __declspec(uuid("752E70E5-EDA9-40ce-97DB-AF90E119F233")) IGraphRenderer
	: public IBaseRenderer
{
	IRenderer* CreateRenderer(IGraphSubgraph * g);
	IRenderer* CreateRenderer(IGraphVertex * v);
	void SetTopParent(HWND hwnd);
};

__interface __declspec(uuid("A53D950D-9D72-4b5a-B09E-51447FCDEF18")) IEdgeRenderer 
	: public IBaseRenderer
{
};

__interface __declspec(uuid("39028DAB-57E8-45cf-8C86-9AD5BD5900BD")) IListItemRenderer 
	: public IBaseRenderer
{
	void SetBoundsBox(const GraphTypes::RectF& box, unsigned flags);
	void SetSelected(bool bSelected=true);
};

__interface __declspec(uuid("BBE2AB1F-6BB4-40e2-80A3-A687EF572B45")) IListWindowRenderer 
	: public IBaseRenderer
{
	void GetConnectBounds(GraphTypes::RectF& left, GraphTypes::RectF& right, unsigned order);
};

__interface __declspec(uuid("A9D327C3-31F6-4a97-81DB-D8B3ECA9742D")) IArrowTipRenderer 
	: public IBaseRenderer
{
	void SetBoundsBox(const GraphTypes::RectF& box, unsigned flags);
	void SetVertex(IGraphVertex* v);
	void SetColor(GraphTypes::Color color);
};

__interface __declspec(uuid("A87D501A-7FBC-49a2-826D-DC4FFE7C49A2")) IArrowTailRenderer 
	: public IBaseRenderer
{
	void SetBoundsBox(const GraphTypes::RectF& box, unsigned flags);
	void SetVertex(IGraphVertex* v);
	void SetColor(GraphTypes::Color color);
};

__interface __declspec(uuid("47B5B8E8-C6EA-4fff-B692-15B9400F243A")) IIconRenderer 
	: public IBaseRenderer
{
	void SetBoundsBox(const GraphTypes::RectF& box, unsigned flags);
	void SetSelected(bool bSelected=true);
};

__interface __declspec(uuid("394371AA-5A46-4d14-B9C7-AEFDEDF5396D")) IImageRenderer 
	: public IBaseRenderer
{
	void SetBoundsBox(const GraphTypes::RectF& box, unsigned flags);
	void SetSelected(bool bSelected=true);
};

typedef StlLinked<IRenderer> IRendererAdapt;
typedef std::map<CUniqueID, IRendererAdapt> IRendererMap;
typedef std::vector<IRendererAdapt> IRendererVector;

typedef std::map<DpiScale, GraphTypes::RectF> RectFMap;
typedef std::map<DpiScale, GraphTypes::SizeF> SizeFMap;

typedef std::map<DpiScale, GraphTypes::CRectEx> CRectMap;
typedef std::map<DpiScale, CSize> CSizeMap;
typedef std::map<DpiScale, int> DimensionMap;
typedef std::map<CUniqueID, GraphTypes::CRectEx> IDRectMap;
typedef std::map<CUniqueID, GraphTypes::RectF> IDRectFMap;
typedef std::map<int, CUniqueID> intIDMap;
typedef std::map<CUniqueID, int> IDintMap;


IGraphRenderer *CreateRenderer(CGraphFastRenderImpl & parent, IGraph * graph);

IRenderer *CreateRenderer(CGraphFastRenderImpl & parent, IGraphSubgraph * g);
IRenderer* CreateListItemRenderer(CGraphFastRenderImpl & parent,IGraphVertex * v);

IRenderer* CreateLegacyRenderer(CGraphFastRenderImpl & parent, IGraphVertex * v);
IRenderer* CreateIconRenderer(CGraphFastRenderImpl & parent, IGraphVertex * v);
IRenderer* CreateImageRenderer(CGraphFastRenderImpl & parent, IGraphVertex * v);

IRenderer* CreateRenderer(CGraphFastRenderImpl & parent, IGraphEdge * e);
IRenderer* CreateArrowTipRenderer(CGraphFastRenderImpl & parent, IGraphVertex * v=0);
IRenderer* CreateArrowTailRenderer(CGraphFastRenderImpl & parent, IGraphVertex * v=0);

