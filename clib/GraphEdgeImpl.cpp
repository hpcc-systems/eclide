#include "StdAfx.h"

#include "GraphEdgeImpl.h"
#include "GraphVertexImpl.h"
#include "Cache.h"
#include "IPictureHelper.h"

template<typename G>
class CEdge : public IGraphEdge, public CUnknown
{
	typedef CEdge<G> thisClass;
protected:
	GraphTypes::Color		m_color;

	GraphTypes::Color		m_color_border;
	GraphTypes::Color		m_color_fill;
	GraphTypes::Color		m_color_font;

	VISIBLE					m_visible;

	CUniqueID				m_id;
	std::_tstring			m_label;
	G &						m_graph;
	CComPtr<IGraphVertex>	m_source;
	CComPtr<IGraphVertex>	m_target;
	typename G::edge_descriptor	m_edge;
	PropertyUnknownMap		m_property_unknown;
	PropertyUnknownMap		m_internal_unknown;
	PropertyMap				m_property_map;
	ImageInfoMaps			m_imageMaps;
	VISIBLE					m_imagevis[9];
	boost::posix_time::time_period m_timeRange;
	graph_signal_type		m_signal;

public:
	BEGIN_CUNKNOWN
		IMPLEMENT_INTERFACE(IGraphItem)
		IMPLEMENT_INTERFACE(IGraphEdge)
	END_CUNKNOWN(CUnknown)

	CEdge(G & graph, IGraphVertex * source, IGraphVertex * target, const typename G::edge_descriptor & edge, const CUniqueID & id) : 	m_graph(graph), m_edge(edge), m_source(source), m_target(target), m_id(id), m_timeRange(boost::posix_time::ptime(), boost::posix_time::ptime())
	{
		m_visible = VISIBLE_DEFAULT;
		for(int i=0; i<9; ++i)
			m_imagevis[i]=VISIBLE_DEFAULT;
		m_graph.m_graph[m_edge].id = id;
	}
	void Delete()
	{
		ATLASSERT(m_edge != m_graph.UNKNOWN_EDGE);
		AddRef();
		m_graph.m_edgeCache.Clear(GetCacheID());
		ATLASSERT(!m_graph.m_edgeCache.Exists(GetCacheID()));
		remove_edge(m_edge, m_graph.m_fullGraph);
		m_edge = m_graph.UNKNOWN_EDGE; 
		Release();
		m_source->Signal(GRAPHNOTIFICATION_EDGEREMOVED);
		m_target->Signal(GRAPHNOTIFICATION_EDGEREMOVED);
	}
	const CCategory & GetCategory() const
	{
		ATLASSERT(m_edge != m_graph.UNKNOWN_EDGE);
		return GetID().GetCategory();	
	}
	const CUniqueID & GetID() const
	{
		ATLASSERT(m_edge != m_graph.UNKNOWN_EDGE);
		return m_id;
	}
	int GetCacheID() const
	{
		ATLASSERT(m_edge != m_graph.UNKNOWN_EDGE);
		return m_id.GetHash();
	}
	const TCHAR * GetLabel() const
	{
		ATLASSERT(m_edge != m_graph.UNKNOWN_EDGE);
		return m_label.c_str();
	}
	void SetLabel(const TCHAR* label)
	{
		ATLASSERT(m_edge != m_graph.UNKNOWN_EDGE);
		m_label = label;
		m_signal(GRAPHNOTIFICATION_LABELCHANGED, this);
	}
	bool Equals(IGraphItem * other) const
	{
		ATLASSERT(m_edge != m_graph.UNKNOWN_EDGE);
		return (GetID() == other->GetID());
	}
	void GetProperties(IPropertyIDContainer* storage) const
	{
		for (PropertyMap::const_iterator itr = m_property_map.begin(); itr != m_property_map.end(); ++itr)
		{
			storage->Add(itr->first);
		}
	}
	void CopyProperties(PropertyMap & props) const
	{
		ATLASSERT(m_edge != m_graph.UNKNOWN_EDGE);
		props = m_property_map;
	}
	void PasteProperties(const PropertyMap & props)
	{
		ATLASSERT(m_edge != m_graph.UNKNOWN_EDGE);
		m_property_map = props;
	}
	void CopyPropertyUnknowns(PropertyUnknownMap & props) const
	{
		ATLASSERT(m_edge != m_graph.UNKNOWN_EDGE);
		props = m_property_unknown;
	}
	void PastePropertyUnknowns(const PropertyUnknownMap & props)
	{
		ATLASSERT(m_edge != m_graph.UNKNOWN_EDGE);
		m_property_unknown = props;
	}
	void GetProperty(int key, CComVariant& result) const
	{
		ATLASSERT(m_edge != m_graph.UNKNOWN_EDGE);
		const PropertyMap & map = m_property_map;
		PropertyMap::const_iterator itr = map.find(key);
		if (itr != map.end())
			result = itr->second;
		else
			result.ChangeType(VT_EMPTY);
	}
	void SetProperty(int key, const CComVariant& val)
	{
		ATLASSERT(m_edge != m_graph.UNKNOWN_EDGE);
		m_property_map[key] = val;
	}
	void PropagateProperty(int key, const CComVariant & val)
	{
		SetProperty(key, val);
		if (IGraphVertex * source = GetSource())
			source->PropagateProperty(key, val);
		if (IGraphVertex * target = GetTarget())
			target->PropagateProperty(key, val);
	}
	IUnknown * GetPropertyUnknown(int key) const
	{
		ATLASSERT(m_edge != m_graph.UNKNOWN_EDGE);
		PropertyUnknownMap::const_iterator itr = m_property_unknown.find(key);
		if (itr != m_property_unknown.end())
			return itr->second;
		return NULL;
	}
	void SetPropertyUnknown(int key, IUnknown * val)
	{
		ATLASSERT(m_edge != m_graph.UNKNOWN_EDGE);
		m_property_unknown[key] = val;
	}
	IUnknown * GetInternalUnknown(int key) const
	{
		ATLASSERT(m_edge != m_graph.UNKNOWN_EDGE);
		PropertyUnknownMap::const_iterator itr = m_internal_unknown.find(key);
		if (itr != m_internal_unknown.end())
			return itr->second;
		return NULL;
	}
	void SetInternalUnknown(int key, IUnknown * val)
	{
		ATLASSERT(m_edge != m_graph.UNKNOWN_EDGE);
		m_internal_unknown[key] = val;
	}
	VISIBLE GetVisible() const
	{
		ATLASSERT(m_edge != m_graph.UNKNOWN_EDGE);
		return m_visible;
	}
	void SetVisible(VISIBLE visible)
	{
		ATLASSERT(m_edge != m_graph.UNKNOWN_EDGE);
		m_visible = visible;
	}
	bool IsVisible() const
	{
		return !m_graph.m_edgeFilter.m_skipper || m_graph.m_edgeFilter.m_skipper->SkipEdge((IGraphEdge *)this) == false;
	}
	IGraphVertex * GetSource()
	{
		ATLASSERT(m_edge != m_graph.UNKNOWN_EDGE);
		return m_source;
	}
	IGraphVertex * GetTarget()
	{
		ATLASSERT(m_edge != m_graph.UNKNOWN_EDGE);
		return m_target;
	}
	void SetTarget(IGraphVertex * target)
	{
		ATLASSERT(m_edge != m_graph.UNKNOWN_EDGE);
		remove_edge(m_edge, m_graph.m_fullGraph);
		std::pair<G::edge_descriptor, bool> newE = add_edge(GetVertexDescriptor<G>(m_source), GetVertexDescriptor<G>(m_target), m_graph.m_fullGraph);
		ATLASSERT(newE.second);
		m_edge = newE.first;
		m_target = target;
	}
	IGraphVertex * GetAdjacentVertex(IGraphVertex * from)
	{
		ATLASSERT(m_edge != m_graph.UNKNOWN_EDGE);
		ATLASSERT(m_source->GetID() == from->GetID() || m_target->GetID() == from->GetID());
		return m_source->GetID() == from->GetID() ? m_target : m_source;
	}
	void SetWeight(const double & /*weight*/)
	{
		ATLASSERT(m_edge != m_graph.UNKNOWN_EDGE);
		//TODO:  GJS
		//m_graph.m_edge_weight3[m_edge] = weight;
	}

	typename G::edge_descriptor GetIndex()
	{
		ATLASSERT(m_edge != m_graph.UNKNOWN_EDGE);
		return m_edge;
	}

	void SetColor(const GraphTypes::Color & newVal)
	{
		ATLASSERT(m_edge != m_graph.UNKNOWN_EDGE);
		m_color=newVal;
	}

	const GraphTypes::Color & GetColor() const
	{
		ATLASSERT(m_edge != m_graph.UNKNOWN_EDGE);
		return m_color;
	}

	const GraphTypes::Color & GetBorderColor()
	{
		ATLASSERT(m_edge != m_graph.UNKNOWN_EDGE);
		return m_color_border;
	}
	IGraphSubgraph * GetParent() const
	{
		return NULL;
	}
	void SetParent(IGraphSubgraph* subgraph)
	{
	}
	void SetBorderColor(const GraphTypes::Color & border)
	{
		ATLASSERT(m_edge != m_graph.UNKNOWN_EDGE);
		m_color_border = border;
	}
	const GraphTypes::Color & GetFillColor()
	{
		ATLASSERT(m_edge != m_graph.UNKNOWN_EDGE);
		return m_color_fill;
	}
	void SetFillColor(const GraphTypes::Color & fill)
	{
		ATLASSERT(m_edge != m_graph.UNKNOWN_EDGE);
		m_color_fill = fill;
	}
	const GraphTypes::Color & GetFontColor()
	{
		ATLASSERT(m_edge != m_graph.UNKNOWN_EDGE);
		return m_color_font;
	}
	void SetFontColor(const GraphTypes::Color & font)
	{
		ATLASSERT(m_edge != m_graph.UNKNOWN_EDGE);
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
		ATLASSERT(m_edge != m_graph.UNKNOWN_EDGE);
		m_imagevis[static_cast<unsigned int>(pos)]=visible;
	}

	VISIBLE GetImageVisible(IMAGE_POSITION pos) const
	{
		ATLASSERT(m_edge != m_graph.UNKNOWN_EDGE);
		return m_imagevis[static_cast<unsigned int>(pos)];
	}

	typename G::edge_descriptor GetEdgeDescriptor() const
	{
		ATLASSERT(m_edge != m_graph.UNKNOWN_EDGE);
		return m_edge;
	}

	void SetDateRange(const boost::posix_time::time_period & timeRange)
	{
		m_timeRange = timeRange;
	}

	const boost::posix_time::time_period & GetDateRange()
	{
		return m_timeRange;
	}

	boost::signals::connection Connect(const graph_slot_type& s)
	{
		return m_signal.connect(s);
	}

	void Disconnect(boost::signals::connection& sc)
	{
		sc.disconnect();
	}
};

template<typename GraphData>
IGraphEdge * CreateIGraphEdge(GraphData & graph, IGraphVertex * source, IGraphVertex * target, const typename GraphData::edge_descriptor & edge, const CUniqueID & id)
{
	return graph.m_edgeCache.Get(new CEdge<GraphData>(graph, source, target, edge, id));
}
//template IGraphEdge * CreateIGraphEdge<CGraphDataT<UndirectedGraph> >(CGraphDataT<UndirectedGraph> & graph, IGraphVertex * source, IGraphVertex * target, const CGraphDataT<UndirectedGraph>::edge_descriptor & edge, const CUniqueID & id);
//template IGraphEdge * CreateIGraphEdge<CGraphDataT<DirectedGraph> >(CGraphDataT<DirectedGraph> & graph, IGraphVertex * source, IGraphVertex * target, const CGraphDataT<DirectedGraph>::edge_descriptor & edge, const CUniqueID & id);

void testEdge()
{
	{
		CUndirectedGraphData g1;
		CreateIGraphEdge(g1, NULL, NULL, g1.UNKNOWN_EDGE, CUniqueID());
	}

	{
		CDirectedGraphData g2;
		CreateIGraphEdge(g2, NULL, NULL, g2.UNKNOWN_EDGE, CUniqueID());
	}
}
