#pragma once

#include "GraphViewCtl.h"
#include "GraphView.h"

class ATL_NO_VTABLE GraphViewCtlCommon : public IGraphViewSlot
{
public:
	GraphViewCtlCommon() : m_currentLayoutType(layoutNone) {};
	~GraphViewCtlCommon() {};
protected:
	CComPtr<IGraphView> m_view;
	LayoutType m_currentLayoutType;
public:
	// IGraphViewSlot
	virtual void OnDoubleClick() = 0;
	virtual void OnDoubleClick(const VertexID &v) = 0;

	// IGraphViewControl
	STDMETHOD(AddVertex)(long Category, BSTR Identity, IVertex** Vertex);
	STDMETHOD(AddEdge)(long Category, IVertex* From, IVertex* To, IEdge** Edge);
	STDMETHOD(GetVertex)(long Category, BSTR Identity, IVertex** Vertex);
	STDMETHOD(GetEdge)(long Category, IVertex* From, IVertex* To, IEdge** Edge);
	STDMETHOD(Clear)();
	STDMETHOD(GenerateRandomData)();
	STDMETHOD(get_Layout)(LayoutType* pVal);
	STDMETHOD(put_Layout)(LayoutType newVal);
	STDMETHOD(StopLayout)();
	STDMETHOD(Recenter)();
	STDMETHOD(get_Zoom)(long* pVal);
	STDMETHOD(put_Zoom)(long newVal);
	STDMETHOD(SetDefaultVertexIcon)(long Category, long hBitmap);
	STDMETHOD(SetDefaultVertexIconFile)(long Category, BSTR FileName);
	STDMETHOD(SetDefaultVertexLabel)(long Category, BSTR Label);
	STDMETHOD(SetDefaultVertexColors)(long Category, long NormalFill, long BorderFill, long SelectedFill, long SelectedBorder, long OverFill, long OverBorder);
	STDMETHOD(SetDefaultEdgeColors)(long Category, long Normal, long ShortestPath);
	STDMETHOD(SetDefaultEdgeLabel)(long Category, BSTR Label);
	// IGraphViewControlProperties
	STDMETHOD(GetVertexProperty)(long Category, BSTR Identity, long key, BSTR* pVal);
	STDMETHOD(SetVertexProperty)(long Category, BSTR Identity, long key, BSTR newVal);
	STDMETHOD(GetEdgeProperty)(long Category, long FromCategory, BSTR FromIdentity, long ToCategory, BSTR ToIdentity, long key, BSTR* pVal);
	STDMETHOD(SetEdgeProperty)(long Category, long FromCategory, BSTR FromIdentity, long ToCategory, BSTR ToIdentity, long key, BSTR newVal);
};
