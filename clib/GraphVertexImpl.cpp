#include "StdAfx.h"

#include "GraphVertexImpl.h"
#include "GraphSubgraphImpl.h"
#include "GraphEdgeImpl.h"
#include "IPictureHelper.h"
#include "GraphIteratorImpl.h"

template<typename G>
class CVertex : public IGraphVertex, public CUnknown
{
protected:
	GraphTypes::Color		m_color_border;
	GraphTypes::Color		m_color_fill;
	GraphTypes::Color		m_color_font;
	VISIBLE					m_visible;

	ImageInfoMaps			m_imageMaps;
	VISIBLE					m_imagevis[9];

	std::_tstring m_label;
	G & m_graph;
	IGraphSubgraph * m_parent;
	typename G::vertex_descriptor m_vertex;
	CUniqueID m_id;
	PropertyUnknownMap m_property_unknown;
	PropertyUnknownMap m_internal_unknown;
	PropertyMap m_property_map;
	graph_signal_type m_signal;

public:
	BEGIN_CUNKNOWN
		IMPLEMENT_INTERFACE(IGraphItem)
		IMPLEMENT_INTERFACE(IGraphVertex)
	END_CUNKNOWN(CUnknown)

	CVertex(G & graph, IGraphSubgraph * parent, typename G::vertex_descriptor vertex, const CUniqueID & id) : m_graph(graph), m_parent(parent), m_vertex(vertex), m_id(id)
	{
		ATLASSERT(m_parent);

		m_graph.m_graph[m_vertex].id = id;
		m_visible = VISIBLE_DEFAULT;
		for(int i=0; i<9; ++i)
			m_imagevis[i]=VISIBLE_DEFAULT;
	}
	void Delete()
	{
		ATLASSERT(m_vertex != m_graph.UNKNOWN_VERTEX); 
		AddRef();
		m_graph.m_vertexCache.Clear(GetCacheID());
		ATLASSERT(!m_graph.m_vertexCache.Exists(GetCacheID()));
		m_parent->RemoveChild(this);
		clear_vertex(m_vertex, m_graph.m_fullGraph);
		remove_vertex(m_vertex, m_graph.m_fullGraph);
		m_property_unknown.clear();
		m_internal_unknown.clear();
		m_vertex = m_graph.UNKNOWN_VERTEX; 
		Release();
		m_parent->Signal(GRAPHNOTIFICATION_VERTEXREMOVED);
	}
	const CCategory & GetCategory() const
	{
		ATLASSERT(m_vertex != m_graph.UNKNOWN_VERTEX); 
		return m_id.GetCategory();
	}
	const CUniqueID & GetID() const
	{
		ATLASSERT(m_vertex != m_graph.UNKNOWN_VERTEX); 
		return m_id;
	}
	int GetCacheID() const
	{
		ATLASSERT(m_vertex != m_graph.UNKNOWN_VERTEX); 
		return m_id.GetHash();
	}
	const TCHAR * GetLabel() const
	{
		ATLASSERT(m_vertex != m_graph.UNKNOWN_VERTEX); 
		return m_label.c_str();
	}
	void SetLabel(const TCHAR* label)
	{
		ATLASSERT(m_vertex != m_graph.UNKNOWN_VERTEX); 
		if (label != NULL && _tcslen(label))
		{
			m_label = label;
			m_signal(GRAPHNOTIFICATION_LABELCHANGED, this);
		}
	}
	bool Equals(IGraphItem * other) const
	{
		ATLASSERT(m_vertex != m_graph.UNKNOWN_VERTEX); 
		return (GetID() == other->GetID());
	}
	VISIBLE GetVisible() const
	{
		ATLASSERT(m_vertex != m_graph.UNKNOWN_VERTEX); 
		return m_visible;
	}
	void SetVisible(VISIBLE visible)
	{
		ATLASSERT(m_vertex != m_graph.UNKNOWN_VERTEX); 
		m_visible=visible;
	}
	bool IsVisible() const
	{
		return !m_graph.m_vertexFilter.m_skipper || m_graph.m_vertexFilter.m_skipper->SkipVertex((IGraphVertex *)this) == false;
	}
	void GetProperty(int key, CComVariant& result) const
	{
		ATLASSERT(m_vertex != m_graph.UNKNOWN_VERTEX); 
		const PropertyMap & map = m_property_map;
		PropertyMap::const_iterator itr = map.find(key);
		if (itr != map.end())
			result = itr->second;
		else
			result.ChangeType(VT_EMPTY);
	}
	void PropagateProperty(int key, const CComVariant & val)
	{
		SetProperty(key, val);
		if (IGraphSubgraph * parent = GetParent())
			parent->PropagateProperty(key, val);
	}
	void SetProperty(int key, const CComVariant& val)
	{
		ATLASSERT(m_vertex != m_graph.UNKNOWN_VERTEX); 
		m_property_map[key] = val;
	}
	IUnknown * GetPropertyUnknown(int key) const
	{
		ATLASSERT(m_vertex != m_graph.UNKNOWN_VERTEX); 
		const PropertyUnknownMap & map = m_property_unknown; 
		PropertyUnknownMap::const_iterator itr = map.find(key);
		if (itr != map.end())
			return itr->second;
		return NULL;
	}
	void SetPropertyUnknown(int key, IUnknown * val)
	{
		ATLASSERT(m_vertex != m_graph.UNKNOWN_VERTEX); 
		m_property_unknown[key] = val;
	}
	IUnknown * GetInternalUnknown(int key) const
	{
		ATLASSERT(m_vertex != m_graph.UNKNOWN_VERTEX); 
		PropertyUnknownMap::const_iterator itr = m_internal_unknown.find(key);
		if (itr != m_internal_unknown.end())
			return itr->second;
		return NULL;
	}
	void SetInternalUnknown(int key, IUnknown * val)
	{
		ATLASSERT(m_vertex != m_graph.UNKNOWN_VERTEX); 
		m_internal_unknown[key] = val;
	}
	unsigned int GetGlobalIndex()
	{
		ATLASSERT(m_vertex != m_graph.UNKNOWN_VERTEX); 
		return reinterpret_cast<unsigned int>(m_vertex);
	}
	void SetOrder(unsigned int pos)
	{
		ATLASSERT(m_vertex != m_graph.UNKNOWN_VERTEX); 
		m_parent->SetChildIndex(this, pos);
	}
	unsigned int GetOrder() const
	{
		ATLASSERT(m_vertex != m_graph.UNKNOWN_VERTEX); 
		return m_parent->GetChildIndex(this);
	}
	void MoveToEnd()
	{
		ATLASSERT(m_vertex != m_graph.UNKNOWN_VERTEX); 
		return m_parent->MoveToEnd(this);
	}
	const GraphTypes::Color & GetBorderColor()
	{
		ATLASSERT(m_vertex != m_graph.UNKNOWN_VERTEX); 
		return m_color_border;
	}
	void SetBorderColor(const GraphTypes::Color & border)
	{
		ATLASSERT(m_vertex != m_graph.UNKNOWN_VERTEX); 
		m_color_border = border;
	}
	const GraphTypes::Color & GetFillColor()
	{
		ATLASSERT(m_vertex != m_graph.UNKNOWN_VERTEX); 
		return m_color_fill;
	}
	void SetFillColor(const GraphTypes::Color & fill)
	{
		ATLASSERT(m_vertex != m_graph.UNKNOWN_VERTEX); 
		m_color_fill = fill;
	}
	const GraphTypes::Color & GetFontColor()
	{
		ATLASSERT(m_vertex != m_graph.UNKNOWN_VERTEX); 
		return m_color_font;
	}
	void SetFontColor(const GraphTypes::Color & font)
	{
		ATLASSERT(m_vertex != m_graph.UNKNOWN_VERTEX); 
		m_color_font = font;
	}

	void SetImage(const ImageInfo& iinfo, int overlay)
	{
		m_imageMaps[static_cast<int>(iinfo.pos)][overlay]=iinfo;
	}

	const ImageInfoMaps* GetImages() const
	{
		return &m_imageMaps;
	}

	void SetImageVisible(IMAGE_POSITION pos, VISIBLE visible)
	{
		ATLASSERT(m_vertex != m_graph.UNKNOWN_VERTEX); 
		m_imagevis[static_cast<unsigned int>(pos)]=visible;
	}

	VISIBLE GetImageVisible(IMAGE_POSITION pos) const
	{
		ATLASSERT(m_vertex != m_graph.UNKNOWN_VERTEX); 
		return m_imagevis[static_cast<unsigned int>(pos)];
	}

	void SetImageSize(IMAGE_POSITION pos, unsigned ovidx, const CSize& size)
	{
#ifdef _DEBUG
		ImageInfoMaps::const_iterator itr=m_imageMaps.find(pos);
		ATLASSERT(itr != m_imageMaps.end());
		MapImageInfo::const_iterator itr2=itr->second.find(ovidx);
		ATLASSERT(itr2 != itr->second.end());
#endif
		m_imageMaps[pos][ovidx].SetSize(size); 
	}

	void GetImageSize(IMAGE_POSITION pos, unsigned ovidx, CSize& size) const
	{
		// can't use [] in const method and don't want mutable to create empty map entry
		ImageInfoMaps::const_iterator itr=m_imageMaps.find(pos);
		ATLASSERT(itr != m_imageMaps.end());
		if(itr != m_imageMaps.end())
		{
			MapImageInfo::const_iterator itr2=itr->second.find(ovidx);
			ATLASSERT(itr2 != itr->second.end());
			if(itr2 != itr->second.end())
				itr2->second.GetSize(size);
		}
	}

	IGraphSubgraph * GetParent() const
	{
		ATLASSERT(m_vertex != m_graph.UNKNOWN_VERTEX); 
		return m_parent;
	}

	void SetParent(IGraphSubgraph* subgraph)
	{
		if(subgraph!=m_parent)
		{
			if(m_parent)
				m_parent->Signal(GRAPHNOTIFICATION_VERTEXREMOVED);
			m_parent=subgraph;		
			if(subgraph)
				subgraph->Signal(GRAPHNOTIFICATION_VERTEXADDED);
		}
	}

	void GetProperties(IPropertyIDContainer* storage) const
	{
		for (PropertyMap::const_iterator itr = m_property_map.begin(); itr != m_property_map.end(); ++itr)
		{
			storage->Add(itr->first);
		}
	}

	unsigned int GetAdjacentInVertices(IGraphVertexVector & results) const
	{
		ATLASSERT(m_vertex != m_graph.UNKNOWN_VERTEX); 
		G::in_edge_iterator itr, end;
		for(boost::tie(itr, end) = in_edges(m_vertex, m_graph.m_graph); itr != end; ++itr)
		{
			CComPtr<IGraphEdge> edge = m_graph.GetEdge(*itr);
			ATLASSERT(edge);
			results.push_back(edge->GetSource());
		}
		return results.size();
	}

	unsigned int GetAdjacentOutVertices(IGraphVertexVector & results) const
	{
		ATLASSERT(m_vertex != m_graph.UNKNOWN_VERTEX); 
		G::out_edge_iterator itr, end;
		for(boost::tie(itr, end) = out_edges(m_vertex, m_graph.m_graph); itr != end; ++itr)
		{
			CComPtr<IGraphEdge> edge = m_graph.GetEdge(*itr);
			ATLASSERT(edge);
			results.push_back(edge->GetTarget());
		}
		return results.size();
	}

	unsigned int GetAdjacentVertices(IGraphVertexVector & results) const
	{
		ATLASSERT(m_vertex != m_graph.UNKNOWN_VERTEX); 
		IGraphVertexVector in_results, out_results;
		std::map<CUniqueID, bool> dedup;
		GetAdjacentOutVertices(out_results);
		for(IGraphVertexVector::iterator itr = out_results.begin(); itr != out_results.end(); ++itr)
		{
			IGraphVertex *vertex =  *itr;
			results.push_back(vertex);
			ATLASSERT(dedup[vertex->GetID()] == false);
			dedup[vertex->GetID()] = true;
		}
		GetAdjacentInVertices(in_results);
		for(IGraphVertexVector::iterator itr = in_results.begin(); itr != in_results.end(); ++itr)
		{
			IGraphVertex * vertex = *itr;
			if (dedup[vertex->GetID()] == false)
			{
				results.push_back(vertex);
				dedup[vertex->GetID()] = true;
			}
		}
		return results.size();
	}

	unsigned int GetEdgeCount() const
	{
		return GetInEdgeCount() + GetOutEdgeCount();
	}

	unsigned int GetInEdgeCount() const
	{
		return in_degree(m_vertex, m_graph.m_graph);
	}

	unsigned int GetOutEdgeCount() const
	{
		return out_degree(m_vertex, m_graph.m_graph);
	}

	unsigned int GetAdjacentInEdges(IGraphEdgeVector & results) const
	{
		ATLASSERT(m_vertex != m_graph.UNKNOWN_VERTEX); 
		G::in_edge_iterator itr, end;
		for(boost::tie(itr, end) = in_edges(m_vertex, m_graph.m_graph); itr != end; ++itr)
		{
			CComPtr<IGraphEdge> edge = m_graph.GetEdge(*itr);
			ATLASSERT(edge);
			results.push_back(edge.p);
		}
		return results.size();
	}

	unsigned int GetAdjacentOutEdges(IGraphEdgeVector & results) const
	{
		ATLASSERT(m_vertex != m_graph.UNKNOWN_VERTEX); 
		G::out_edge_iterator itr, end;
		for(boost::tie(itr, end) = out_edges(m_vertex, m_graph.m_graph); itr != end; ++itr)
		{
			results.push_back(m_graph.GetEdge(*itr));
		}
		return results.size();
	}
	
	unsigned int GetAdjacentEdges(IGraphEdgeVector & results) const
	{
		ATLASSERT(m_vertex != m_graph.UNKNOWN_VERTEX); 
		IGraphEdgeVector in_results, out_results;
		std::map<CUniqueID, bool> dedup;
		GetAdjacentOutEdges(out_results);
		for(IGraphEdgeVector::iterator itr = out_results.begin(); itr != out_results.end(); ++itr)
		{
			IGraphEdge * edge =  *itr;
			results.push_back(edge);
			ATLASSERT(dedup[edge->GetID()] == false);
			dedup[edge->GetID()] = true;
		}
		GetAdjacentInEdges(in_results);
		for(IGraphEdgeVector::iterator itr = in_results.begin(); itr != in_results.end(); ++itr)
		{
			IGraphEdge * edge = *itr;
			if (dedup[edge->GetID()] == false)
			{
				results.push_back(edge);
				dedup[edge->GetID()] = true;
			}
		}
		return results.size();
	}

	IEdgeIterator * GetAdjacentInEdges()
	{
		G::in_edge_iterator itr, end;
		boost::tie(itr, end) = in_edges(m_vertex, m_graph.m_graph);
		return new EdgeIteratorAdapter<G, G::in_edge_iterator>(&m_graph, itr, end);
	}

	IEdgeIterator * GetAdjacentOutEdges()
	{
		G::out_edge_iterator itr, end;
		boost::tie(itr, end) = out_edges(m_vertex, m_graph.m_graph);
		return new EdgeIteratorAdapter<G, G::out_edge_iterator>(&m_graph, itr, end);
	}

	IEdgeIterator * GetAdjacentEdges()
	{
		AdjacentEdgeIterator<G> * retVal = new AdjacentEdgeIterator<G>(m_graph, m_vertex);
		retVal->Init();
		return retVal;
	}
	
	typename G::vertex_descriptor GetVertexDescriptor() const
	{
		ATLASSERT(m_vertex != m_graph.UNKNOWN_VERTEX); 
		return m_vertex;
	}

	boost::signals::connection Connect(const graph_slot_type& s)
	{
		return m_signal.connect(s);
	}

	void Disconnect(boost::signals::connection& sc)
	{
		sc.disconnect();
	}

	void Signal(GRAPHNOTIFICATION notification)
	{
		m_signal(notification, this);
	}
};

template<typename GraphData>
IGraphVertex * CreateIGraphVertex(GraphData & graph, IGraphSubgraph * parent, typename GraphData::vertex_descriptor vertex, const CUniqueID & id)
{
	return graph.m_vertexCache.Get(new CVertex<GraphData>(graph, parent, vertex, id));
}
//template IGraphVertex * CreateIGraphVertex<CGraphDataT<UndirectedGraph> >(CGraphDataT<UndirectedGraph> & graph, IGraphSubgraph * parent, CGraphDataT<UndirectedGraph>::vertex_descriptor vertex, const CUniqueID & id);
//template IGraphVertex * CreateIGraphVertex<CGraphDataT<DirectedGraph> >(CGraphDataT<DirectedGraph> & graph, IGraphSubgraph * parent, CGraphDataT<DirectedGraph>::vertex_descriptor vertex, const CUniqueID & id);

template<typename GraphData>
typename GraphData::vertex_descriptor GetVertexDescriptor(IGraphVertex * _v)
{
	CVertex<GraphData> * v = reinterpret_cast<CVertex<GraphData> *>(_v);
	return v->GetVertexDescriptor();
}
//template CGraphDataT<UndirectedGraph>::vertex_descriptor GetVertexDescriptor<CGraphDataT<UndirectedGraph> >(IGraphVertex * _v);
//template CGraphDataT<DirectedGraph>::vertex_descriptor GetVertexDescriptor<CGraphDataT<DirectedGraph> >(IGraphVertex * _v);


void testVertex()
{
	CUndirectedGraphData g1;
	CreateIGraphVertex(g1, NULL, 0, CUniqueID());

	CDirectedGraphData g2;
	CreateIGraphVertex(g2, NULL, 0, CUniqueID());

	GetVertexDescriptor<CUndirectedGraphData>(0);
	GetVertexDescriptor<CDirectedGraphData>(0);
}