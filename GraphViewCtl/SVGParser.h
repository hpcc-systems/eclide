#pragma once

#include "SAXHandlers.h"
#include "Neato.h"

class CGraphATTSVGHandler : public StackHandlerImpl
{
private:
	bool m_valid;
	NEATO_MODE m_mode;
	enum e_class { unknown, graph, cluster, node, edge, dummy_node, last } m_class;
	CComPtr<IGraphSubgraph> m_subgraph;
	CComPtr<IGraphVertex> m_vertex;
	CComPtr<IGraphEdge> m_edge;
	std::_tstring m_title;
	std::_tstring m_text;
	int m_text_count;
	GraphTypes::REAL m_text_x;
	GraphTypes::REAL m_text_y;
	typedef std::list<std::pair<GraphTypes::REAL, GraphTypes::REAL> > PointList;
	PointList m_points;
	void setBoundingBox(IGraphItem* item);
	//typedef std::stack<IGraphEdgeAdapt> IGraphEdgeStack;
	//typedef std::map<std::_tstring, IGraphEdgeStack> HashGraphEdgeMap;
	//HashGraphEdgeMap m_hashEdges;

protected:
	std::stack<IGraphSubgraphAdapt> m_stackSubgraph;
	mutable clib::recursive_mutex m_mutex;
	CComPtr<IGraph> m_graph;
	StlLinked<CCanvas> m_canvas;
	CMeta * m_meta;
	int m_dpi;

public:
	CGraphATTSVGHandler(IGraph* graph, CCanvas* canvas, CMeta * meta, NEATO_MODE mode);
	~CGraphATTSVGHandler();

	typedef std::map<std::_tstring, std::_tstring> edge_type;

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
