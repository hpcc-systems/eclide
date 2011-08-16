#pragma once

#include "SAXHandlers.h"
#include "GraphTypes.h"
#include "GraphContainer.h"

enum PROP
{
	PROP_UNKNOWN = 500,
	PROP_CALLBACK,
	PROP_ATTRIBUTES,
	PROP_TOOLTIP,
	PROP_EDGE_VISIBLESOURCE, 
	PROP_EDGE_VISIBLETARGET, 
	PROP_EDGE_DOTTED, 
	PROP_EDGE_SLAVES,
	PROP_EDGE_STARTED,
	PROP_EDGE_STOPPED,
	PROP_EDGE_TARGETINDEX,
	PROP_EDGE_COMPASS,
	PROP_EDGE_LOOSE, 
	PROP_ROXIESTARTED,
	PROP_FOUND,
	PROP_VERTEX_ISPASSTHROUGH,
	PROP_VERTEX_KIND,
	PROP_VERTEX_DEFINITION,
	PROP_LAST
};

enum XGMML_CAT
{
	XGMML_CAT_UNKNOWN = 0,
	XGMML_CAT_GRAPH, 
	XGMML_CAT_SUBGRAPH, 
	XGMML_CAT_VERTEX, 
	XGMML_CAT_EDGE, 
	XGMML_CAT_LAST
};

typedef boost::tuples::tuple<std::_tstring, std::_tstring, std::_tstring, bool, int, int, int, bool> EdgeTuple;
typedef std::map<EdgeTuple, Element> EdgeMap;

class CLIB_API __declspec(uuid("046E6A5B-FB29-44cc-9E3B-44C729D2B2D3")) CStringStringMap : public CUnknown
{
public:
	Element::StringStringMap m_attr;

	BEGIN_CUNKNOWN
		IMPLEMENT_INTERFACE(CStringStringMap);
	END_CUNKNOWN(CUnknown)

	void MergeFrom(const CStringStringMap & other);
};

__interface __declspec(uuid("AF4A04F0-4AD8-4c58-9F2F-6487AF1D41F7")) IXGMMLRenderCallback : public IUnknown
{
	GraphTypes::Color GetColor(IGraphSubgraph * subgraph) const;
	void GetColor(IGraphVertex * vertex, GraphTypes::Color &borderColor, GraphTypes::Color & fillColor, GraphTypes::Color & fontColor) const;
	void GetColor(IGraphEdge * edge, GraphTypes::Color & edgeColor, GraphTypes::Color & fontColor, GraphTypes::Color & firstControlPointColor, GraphTypes::Color & lastControlPointColor) const;
};

class CLIB_API LazyCreateStack
{
protected:
	IGraph * m_graph;
	bool m_merge;
	bool m_appendMissing;
	const IXGMMLRenderCallback * m_callback;

	std::stack<std::pair<std::_tstring, IGraphSubgraphAdapt> > m_stack;

	IGraphSubgraph * Get(unsigned int depth);

public:
	LazyCreateStack(IGraph * graph, bool merge, bool appendMissing);

	void push(const std::_tstring & id, IGraphSubgraph * subgraph = NULL);
	void pop();
	IGraphSubgraph * top();
	bool topIsVertex();
	//unsigned int size() const;
};

class CLIB_API GraphViewXGMMLHandler : public StackHandlerImpl
{
protected:
	EdgeMap m_edges;
	IGraph * m_graph;
	LazyCreateStack m_stackSubgraph;
	const IXGMMLRenderCallback * m_callback;
	bool m_merge;
	bool m_appendMissing;
	bool m_redundantSubgraphs;
	bool m_passthroughVertices;

public:
	GraphViewXGMMLHandler(IGraph* graph, const IXGMMLRenderCallback * callback, bool merge, bool appendMissing, bool redundantSubgraphs, bool passthroughVertices);

	virtual HRESULT STDMETHODCALLTYPE endDocument();
	virtual HRESULT STDMETHODCALLTYPE startElement(
		const wchar_t *pwchNamespaceUri,
		int cchNamespaceUri,
		const wchar_t *pwchLocalName,
		int cchLocalName,
		const wchar_t *pwchQName,
		int cchQName,
		ISAXAttributes *pAttributes);
	virtual HRESULT STDMETHODCALLTYPE endElement( 
		const wchar_t *pwchNamespaceUri,
		int cchNamespaceUri,
		const wchar_t *pwchLocalName,
		int cchLocalName,
		const wchar_t *pwchQName,
		int cchQName);
};

CLIB_API const TCHAR * CreateTooltip(IGraphEdge * edge, CStringStringMap * attrs, std::_tstring & tooltip, bool textOnly = false);
CLIB_API const TCHAR * CreateTooltip(IGraphVertex * vertex, CStringStringMap * attrs, std::_tstring & tooltip, bool textOnly = false);
