#pragma once

#include "GraphContainer.h"
#include "GraphSortedIndexContainer.h"
#include "cache.h"
#include "GraphSubgraphImpl.h"
#include "GraphVertexImpl.h"
#include "GraphEdgeImpl.h"
#include "GraphIteratorImpl.h"

#include <boost/progress.hpp>
#include <boost/graph/filtered_graph.hpp>
//  ===========================================================================
class CNoSkipEdge : public ISkipEdge, public CUnknown
{
public:
	BEGIN_CUNKNOWN
	END_CUNKNOWN(CUnknown)

	bool SkipEdge(IGraphEdge * e) const
	{
		return false;
	}
};

class CNoSkipSubgraph : public ISkipSubgraph, public CUnknown
{
public:
	BEGIN_CUNKNOWN
	END_CUNKNOWN(CUnknown)

	bool SkipSubgraph(IGraphSubgraph * sg) const
	{
		return false;
	}
};

struct VertexProps
{
	CUniqueID id;
	boost::default_color_type color;
};

struct EdgeProps
{
	CUniqueID id;
};

struct GraphProps
{
	CUniqueID id;
};

template<typename V>
class Path : public std::map<V, V>
{
	typedef std::map<V, V> baseClass;
public:
	bool ContainsStep(V u, V v) const
	{
		const V UNKNOWN_VERTEX = static_cast<V>(NULL);
		if (u == UNKNOWN_VERTEX || v == UNKNOWN_VERTEX)
			return false;

		if (u == v)
			return false;

		if (!size())
			return false;

		const_iterator u_itr = find(u);
		if (u_itr != end() && u_itr->second == v)
		{
			return true;
		}

		const_iterator v_itr = find(v);
		if (v_itr != end() && v_itr->second == u)
		{
			return true;
		}
		return false;
	}
};

template<typename V>
class ShortestPath : public std::map<V, V>
{
	typedef std::map<V, V> baseClass;
public:
	bool CalculateShortestRoute(V from, Path<V> &result) const
	{
		const V UNKNOWN_VERTEX = static_cast<V>(NULL);
		result.clear();
		const_iterator v_itr = find(from);
		if(v_itr != end())
		{
			while(v_itr->second != v_itr->first && v_itr->second != UNKNOWN_VERTEX)
			{
				result[v_itr->first] = v_itr->second;
				v_itr = find(v_itr->second);
			}
		}
		return true;
	}
};

template<typename G, typename V = typename G::vertex_descriptor>
class CVertexPath : public IVertexPath, public CUnknown
{
public:
	Path<V> m_path;

public:
	IMPLEMENT_CUNKNOWN;

	CVertexPath()
	{
	}

	bool ContainsStep(IGraphVertex * u, IGraphVertex * v)
	{
		return m_path.ContainsStep(GetVertexDescriptor<G>(u), GetVertexDescriptor<G>(v));
	}
};

template<typename G, typename V = typename G::vertex_descriptor>
class CVertexShortestPath : public IVertexShortestPath, public CUnknown
{
protected:
	G * m_graph;

public:
	ShortestPath<V> m_shortestPath;

	IMPLEMENT_CUNKNOWN;

	CVertexShortestPath(G * graph) : m_graph(graph)
	{
	}

	IVertexPath * CalculateShortestRoute(IGraphVertex * v)
	{
		if (!v)
		{
			return NULL;
		}
		CVertexPath<G> * path = new CVertexPath<G>();
		if (m_shortestPath.CalculateShortestRoute(GetVertexDescriptor<G>(v), path->m_path))
		{
			return path;
		}
		path->Release();
		return NULL;
	}

	IVertexPath * CalculateShortestRoute(const CUniqueID & id)
	{
		return CalculateShortestRoute(m_graph->GetVertex(id));
	}
};

template <typename HierarchyMap> 
class ShortestPathRecorder : public boost::default_bfs_visitor
{
public:
	ShortestPathRecorder(HierarchyMap dist) : d(dist) 
	{
	}

	template <typename Edge, typename Graph>
	void tree_edge(Edge e, const Graph & g) const
	{
		typedef boost::graph_traits< Graph > Traits;
		typedef Traits::vertex_descriptor vertex_descriptor;
		typedef Traits::edge_descriptor edge_descriptor;
		typedef Traits::vertices_size_type vertices_size_type;

		vertex_descriptor u = source(e, g);
		vertex_descriptor v = target(e, g);
		d->operator[](v) = u;
	}
private:
	HierarchyMap d;
};
//  ===========================================================================
struct NoEdgeFilter
{
	NoEdgeFilter()
	{
	}
	template <typename Edge>
	bool operator()(const Edge& e) const 
	{
		return true;
	}
};

struct NoVertexFilter
{
	NoVertexFilter() 
	{
	}
	template <typename Vertex>
	bool operator()(const Vertex& v) const 
	{
		return true;
	}
};
//  ===========================================================================
template <typename G>
class UserEdgeFilter
{
public:
	G * m_graph;
	ISkipEdge * m_skipper;
	UserEdgeFilter()
	{
 		m_graph = NULL;
		m_skipper = NULL;
	}
	UserEdgeFilter(const UserEdgeFilter & c)
	{
		m_graph = c.m_graph;
		m_skipper = c.m_skipper;
	}
	UserEdgeFilter(G * graph, ISkipEdge * skipper)
	{ 
 		m_graph = graph;
		m_skipper = skipper;
	}
	template <typename Edge>
	bool operator()(const Edge& e) const 
	{
		return m_skipper == NULL ? true : !m_skipper->SkipEdge(m_graph->GetEdge(e));
	}
};

template <typename G>
class UserVertexFilter
{
public:
	G * m_graph;
	ISkipVertex * m_skipper;
	UserVertexFilter()
	{
		m_graph = NULL;
		m_skipper = NULL;
	}
	UserVertexFilter(const UserVertexFilter & c)
	{
		m_graph = c.m_graph;
		m_skipper = c.m_skipper;
	}
	UserVertexFilter(G * graph, ISkipVertex * skipper)
	{ 
		m_graph = graph;
		m_skipper = skipper;
	}
	template <typename Vertex>
	bool operator()(const Vertex& v) const 
	{
		return m_skipper == NULL ? true : !m_skipper->SkipVertex(m_graph->GetVertex(v));
	}
};

template <typename G>
class UserSubgraphFilter
{
public:
	G * m_graph;
	ISkipSubgraph * m_skipper;
	UserSubgraphFilter()
	{
		m_graph = NULL;
		m_skipper = NULL;
	}
	UserSubgraphFilter(const UserSubgraphFilter & c)
	{
		m_graph = c.m_graph;
		m_skipper = c.m_skipper;
	}
	UserSubgraphFilter(G * graph, ISkipSubgraph * skipper)
	{ 
		m_graph = graph;
		m_skipper = skipper;
	}
	template <typename Subgraph>
	bool operator()(const Subgraph& sg) const 
	{
		return m_skipper == NULL ? true : !m_skipper->SkipSubgraph(m_graph->GetSubgraph(sg));
	}
};

template <typename G>
class UserUnifiedFilter : public ISkipItem, public CUnknown
{
public:
	G * m_graph;

	BEGIN_CUNKNOWN
	END_CUNKNOWN(CUnknown)

	UserUnifiedFilter()
	{
		m_graph = NULL;
	}
	UserUnifiedFilter(const UserUnifiedFilter & c)
	{
		m_graph = c.m_graph;
	}
	UserUnifiedFilter(G * graph)
	{ 
		m_graph = graph;
	}
	bool SkipItem(IGraphItem * item) const
	{
		if (m_graph && m_graph->m_edgeFilter.m_skipper)
			if (CComQIPtr<IGraphEdge> e = item)
				return m_graph->m_edgeFilter.m_skipper->SkipEdge(e);
		else if (m_graph && m_graph->m_vertexFilter.m_skipper)
			if (CComQIPtr<IGraphVertex> v = item)
				return m_graph->m_vertexFilter.m_skipper->SkipVertex(v);
		else if (m_graph && m_graph->m_subgraphFilter.m_skipper)
			if (CComQIPtr<IGraphSubgraph> sg = item)
				return m_graph->m_subgraphFilter.m_skipper->SkipSubgraph(sg);			
		return false;
	}
};
//  ===========================================================================
template<typename FullGraphType>
class CGraphDataT : public IGraph, public CUnknown
{
	typedef CGraphDataT<FullGraphType> thisClass;
protected:
	CUniqueID m_id;
	std::_tstring m_graph_label;
	GraphTypes::Color m_color_border;
	GraphTypes::Color m_color_fill;
	GraphTypes::Color m_color_font;
	PropertyMap m_graph_property;
	PropertyUnknownMap m_graph_propertyunknown;
	PropertyUnknownMap m_graph_internalunknown;
	CSortedGraphItem m_graph_sorteditems;
	VISIBLE m_graph_visible;
	GraphTypes::PointF m_graph_pos;

public:
	FullGraphType m_fullGraph;

	UserEdgeFilter<thisClass> m_edgeFilter;
	UserVertexFilter<thisClass> m_vertexFilter;
	UserSubgraphFilter<thisClass> m_subgraphFilter;
	UserUnifiedFilter<thisClass> m_unifiedFilter;
	typedef boost::filtered_graph<FullGraphType, UserEdgeFilter<thisClass>, UserVertexFilter<thisClass> > GraphType;
	GraphType m_graph;

	typedef boost::graph_traits<GraphType> Traits;
	typedef typename Traits::vertex_descriptor vertex_descriptor;
	typedef typename Traits::edge_descriptor edge_descriptor;
	typedef typename Traits::vertex_iterator vertex_iterator;
	typedef typename Traits::adjacency_iterator adjacency_iterator;
	typedef typename Traits::edge_iterator edge_iterator;
	typedef typename Traits::in_edge_iterator in_edge_iterator;
	typedef typename Traits::out_edge_iterator out_edge_iterator;
	const vertex_descriptor UNKNOWN_VERTEX;// = static_cast<vertex_descriptor>(NULL);
	const edge_descriptor UNKNOWN_EDGE;//(UNKNOWN_VERTEX, UNKNOWN_VERTEX, 0);

	HashCacheT<unsigned int, IGraphSubgraph> m_subgraphCache;
	HashCacheT<unsigned int, IGraphVertex> m_vertexCache;
	HashCacheT<unsigned int, IGraphEdge> m_edgeCache;
	graph_signal_type m_signal;
	VISIBLE m_imagevis[9];

	BEGIN_CUNKNOWN
		IMPLEMENT_INTERFACE(IGraphItem)
		IMPLEMENT_INTERFACE(IGraphSubgraph)
		IMPLEMENT_INTERFACE(IGraph)
	END_CUNKNOWN(CUnknown)

	CGraphDataT() : m_graph_sorteditems(&m_unifiedFilter), m_graph(m_fullGraph, m_edgeFilter, m_vertexFilter),  m_id(ROOT_ID), UNKNOWN_VERTEX(static_cast<vertex_descriptor>(NULL)), UNKNOWN_EDGE(UNKNOWN_VERTEX, UNKNOWN_VERTEX, 0)
	{
		m_graph_visible = VISIBLE_DEFAULT;
	}

	CGraphDataT(ISkipEdge * edgeFilter, ISkipVertex * vertexFilter, ISkipSubgraph * subgraphFilter) : m_graph_sorteditems(&m_unifiedFilter), m_unifiedFilter(this), m_edgeFilter(this, edgeFilter), m_vertexFilter(this, vertexFilter), m_subgraphFilter(this, subgraphFilter), m_graph(m_fullGraph, m_edgeFilter, m_vertexFilter), m_id(ROOT_ID), UNKNOWN_VERTEX(static_cast<vertex_descriptor>(NULL)), UNKNOWN_EDGE(UNKNOWN_VERTEX, UNKNOWN_VERTEX, 0)
	{
		m_graph_visible = VISIBLE_DEFAULT;
	}

	~CGraphDataT()
	{
	}

	//  Subgraph  ---
	IGraphSubgraph * CreateSubgraph(const CUniqueID &id, const TCHAR* label)
	{
		CComPtr<IGraphSubgraph> retVal = CreateIGraphSubgraph(*this, this, id);
		retVal->SetLabel(label);
		m_graph_sorteditems.AddItem(retVal);
		Signal(GRAPHNOTIFICATION_SUBGRAPHADDED);
		return retVal;
	}

	void Delete(bool promoteChildren) 
	{
	}

	VISIBLE GetVisible() const
	{
		return m_graph_visible;
	}

	bool IsVisible() const
	{
		return true;
	}	

	void SetVisible(VISIBLE visible)
	{
		m_graph_visible = visible;
	}

	IGraphSubgraph * GetParent() const
	{
		return NULL;
	}

	void SetParent(IGraphSubgraph* subgraph)
	{
	}

	unsigned int GetOrder() const
	{
		return 0;
	}

	void MoveToEnd()
	{	
	}

	void MoveToEnd(const IGraphItem * child)
	{
		m_graph_sorteditems.MoveToEnd(child->GetID());
	}

	const CCategory & GetCategory() const
	{
		return m_id.GetCategory();
	}

	const CUniqueID & GetID() const
	{
		return m_id;
	}

	const TCHAR * GetLabel() const
	{
		return m_graph_label.c_str();
	}

	void SetLabel(const TCHAR* label)
	{
		m_graph_label = label;
		Signal(GRAPHNOTIFICATION_LABELCHANGED);
	}

	const GraphTypes::Color & GetBorderColor()
	{
		return m_color_border;
	}
	void SetBorderColor(const GraphTypes::Color & border)
	{
		m_color_border = border;
	}
	const GraphTypes::Color & GetFillColor()
	{
		return m_color_fill;
	}
	void SetFillColor(const GraphTypes::Color & fill)
	{
		m_color_fill = fill;
	}
	const GraphTypes::Color & GetFontColor()
	{
		return m_color_font;
	}
	void SetFontColor(const GraphTypes::Color & font)
	{
		m_color_font = font;
	}

	bool Equals(IGraphItem * other) const
	{
		return (GetID() == other->GetID());
	}

	void GetProperties(IPropertyIDContainer* storage) const
	{
		for (PropertyMap::const_iterator itr = m_graph_property.begin(); itr != m_graph_property.end(); ++itr)
		{
			storage->Add(itr->first);
		}
	}

	void GetProperty(int key, CComVariant& result) const
	{
		PropertyMap::const_iterator itr = m_graph_property.find(key);
		if (itr != m_graph_property.end())
			result = itr->second;
		else
			result.ChangeType(VT_EMPTY);
	}

	void SetProperty(int key, const CComVariant& val)
	{
		m_graph_property[key] = val;
	}

	void PropagateProperty(int key, const CComVariant & val)
	{
		SetProperty(key, val);
		if (IGraphSubgraph * parent = GetParent())
			parent->PropagateProperty(key, val);
	}

	IUnknown * GetPropertyUnknown(int key) const
	{
		PropertyUnknownMap::const_iterator itr = m_graph_propertyunknown.find(key);
		if (itr == m_graph_propertyunknown.end())
		{
			return NULL;
		}
		return itr->second;
	}

	void SetPropertyUnknown(int key, IUnknown * val)
	{
		m_graph_propertyunknown[key] = val;
	}

	IUnknown * GetInternalUnknown(int key) const
	{
		PropertyUnknownMap::const_iterator itr = m_graph_internalunknown.find(key);
		if (itr == m_graph_internalunknown.end())
		{
			return NULL;
		}
		return itr->second;
	}

	void SetInternalUnknown(int key, IUnknown * val)
	{
		m_graph_internalunknown[key] = val;
	}

	void SetPoint(const GraphTypes::PointF & pos)
	{
		m_graph_pos = pos;
	}

	GraphTypes::PointF GetPoint() const
	{
		return m_graph_pos;
	}

	//  Subgraph  ---
	IGraphSubgraph * GetSubgraph(const CUniqueID & id) const
	{
		if (m_id == id)
		{
			return const_cast<IGraphSubgraph *>((IGraphSubgraph *)this);
		}
		return m_subgraphCache.Exists(id.GetHash());
	}

	IGraphSubgraph * GetSubgraph(unsigned int hash) const
	{
		if (m_id.GetHash() == hash)
		{
			return const_cast<IGraphSubgraph *>((IGraphSubgraph *)this);
		}
		return m_subgraphCache.Exists(hash);
	}

	bool IsChild(IGraphSubgraph * subgraph) const
	{
		CComPtr<IGraphSubgraph> parent = subgraph->GetParent();
		if (parent && parent->GetID() == GetID())
		{
			return true;
		}
		return false;
	}

	bool Load(const std::_tstring & xgmml, const IXGMMLRenderCallback * callback, bool redundantSubgraphs, bool passthroughVertices);
	bool Merge(const std::_tstring & xgmml, const IXGMMLRenderCallback * callback, bool appendMissing, bool redundantSubgraphs, bool passthroughVertices);

	bool HasItems()
	{
		return (m_vertexCache.Size() || m_subgraphCache.Size() || m_edgeCache.size());
	}

	unsigned int GetAllChildren(IGraphItemVector & results) const
	{
		for(CComPtr<ISubgraphIterator> itr = GetSubgraphIterator(); itr->IsValidItem(); itr->NextItem())
		{
			results.push_back(itr->Get());
		}
		for(CComPtr<IVertexIterator> itr = GetVertexIterator(); itr->IsValidItem(); itr->NextItem())
		{
			results.push_back(itr->Get());
		}
		return results.size();
	}

	unsigned int GetChildren(IGraphItemVector & results) const
	{
		return m_graph_sorteditems.GetItems(results);
	}

	unsigned int GetChildren(IGraphItemVector & results, ISkipItem * callback_func) const
	{
		return m_graph_sorteditems.GetItems(results, callback_func);
	}

	IGraphItemIterator * GetChildren()
	{
		return m_graph_sorteditems.GetItemIterator();
	}

	IGraphItemIterator * GetChildren(ISkipItem * callback_func)
	{
		return m_graph_sorteditems.GetItemIterator(callback_func);
	}

	IGraphItemIterator * GetRChildren(ISkipItem * callback_func)
	{
		return m_graph_sorteditems.GetItemRIterator(callback_func);
	}

	void SetChildIndex(const IGraphItem * child, unsigned int pos)
	{
		m_graph_sorteditems.SetIndex(child->GetID(), pos);
	}

	unsigned int GetChildIndex(const IGraphItem * child) const
	{
		return m_graph_sorteditems.GetIndex(child->GetID());
	}

	IGraphItem * GetChild(unsigned int pos) const
	{
		return m_graph_sorteditems.GetItem(pos);
	}

	void RemoveChild(const IGraphItem * child)
	{
		m_graph_sorteditems.RemoveItem(child->GetID());
	}

	//  Vertex  ---
	IGraphVertex * CreateVertex(const CUniqueID &id)
	{
		vertex_descriptor newV = add_vertex(m_fullGraph);
		CComPtr<IGraphVertex> retVal = CreateIGraphVertex(*this, this, newV, id);
		m_graph_sorteditems.AddItem(retVal);
		Signal(GRAPHNOTIFICATION_VERTEXADDED);
		return retVal;
	}

	void AttachItem(IGraphItem* item)
	{
		m_graph_sorteditems.AddItem(item);
		item->SetParent(this);
	}

	void DetachItem(IGraphItem* item)
	{
		m_graph_sorteditems.RemoveItem(item->GetID());
	}

	IGraphVertex * GetVertex(const CUniqueID & id) const
	{
		return m_vertexCache.Exists(id.GetHash());
	}

	IGraphVertex * GetVertex(unsigned int hash) const
	{
		return m_vertexCache.Exists(hash);
	}

	IGraphVertex * GetVertex(const vertex_descriptor & v) const
	{
		return GetVertex(m_graph[v].id);
	}

	bool IsChild(IGraphVertex * vertex) const
	{
		CComPtr<IGraphSubgraph> parent = vertex->GetParent();
		if (parent->GetID() == GetID())
		{
			return true;
		}
		return false;
	}

	unsigned int GetVertexCount() const
	{
		return (unsigned int) num_vertices(m_graph);
	}

	//  Edge  ---
	IGraphEdge * GetEdge(const CUniqueID & id) const
	{
		return m_edgeCache.Exists(id.GetHash());
	}

	IGraphEdge * GetEdge(unsigned int hash) const
	{
		return m_edgeCache.Exists(hash);
	}

	IGraphEdge * GetEdge(const edge_descriptor & e) const
	{
		return GetEdge(m_graph[e].id);
	}

	IVertexIterator * GetVertexIterator() const
	{
		UserVertexIterator<thisClass> * retVal = new UserVertexIterator<thisClass>(*this, new AllVertex());
		retVal->Init();
		return retVal;
	}

	IVertexIterator * GetUserVertexIterator(ISkipVertex *callback_func) const
	{
		UserVertexIterator<thisClass> * retVal = new UserVertexIterator<thisClass>(*this, callback_func);
		retVal->Init();
		return retVal;
	}

	template<typename G>
	class SubgraphSubgraphIterator : public IteratorC<G, SubgraphSubgraphIterator<G>, IGraphSubgraphVector::iterator, ISubgraphIterator>
	{
		typedef IteratorC<G, SubgraphSubgraphIterator<G>, IGraphSubgraphVector::iterator, ISubgraphIterator> baseClass;
	protected:
		CComPtr<ISkipSubgraph> m_callback_func;

	public:
		IGraphSubgraphVector m_items;

		SubgraphSubgraphIterator(const G &graph, ISkipSubgraph * callback_func) : baseClass(graph), m_callback_func(callback_func)
		{
			graph.GetSubgraphs(m_items);
			m_itr = m_items.begin();
			m_end = m_items.end();
		}

		bool SkipItem()
		{
			CComQIPtr<IGraphSubgraph> sg = m_itr->get();
			if (!sg)
			{
				return true;
			}
			return (m_callback_func->SkipSubgraph(sg));
		}

		IGraphSubgraph * Get()
		{
			CComQIPtr<IGraphSubgraph> retVal = m_itr->get();
			ATLASSERT(retVal);
			return retVal;
		}
	};

	void RecursiveGetSubgraphs(const IGraphSubgraph * parent, IGraphSubgraphVector & results) const
	{
		IGraphItemVector children;
		parent->GetChildren(children);
		for(IGraphItemVector::iterator itr = children.begin(); itr != children.end(); ++itr)
		{
			CComQIPtr<IGraphSubgraph> childSubgraph = itr->get();
			if (childSubgraph)
			{
				results.push_back(childSubgraph.p);
				RecursiveGetSubgraphs(childSubgraph, results);
			}
		}
	}
	
	unsigned int GetSubgraphs(IGraphSubgraphVector & results) const
	{
		RecursiveGetSubgraphs(this, results);
		return results.size();
	}

	ISubgraphIterator * GetSubgraphIterator() const
	{
		SubgraphSubgraphIterator<thisClass> * retVal = new SubgraphSubgraphIterator<thisClass>(*this, new CNoSkipSubgraph());
		retVal->Init();
		return retVal;
	}

	ISubgraphIterator * GetUserSubgraphIterator(ISkipSubgraph * callback_func) const
	{
		SubgraphSubgraphIterator<thisClass> * retVal = new SubgraphSubgraphIterator<thisClass>(*this, callback_func);
		retVal->Init();
		return retVal;
	}

	IGraphEdge * CreateEdge(const CUniqueID & id, IGraphVertex * from, IGraphVertex * to)
	{
		std::pair<edge_descriptor, bool> newE = add_edge(GetVertexDescriptor<thisClass>(from), GetVertexDescriptor<thisClass>(to), m_fullGraph);
		if (newE.second)
		{
			CComPtr<IGraphEdge> retVal = CreateIGraphEdge(*this, from, to, newE.first, id);
			//Signal(GRAPHNOTIFICATION_EDGEADDED);
			from->Signal(GRAPHNOTIFICATION_EDGEADDED);
			to->Signal(GRAPHNOTIFICATION_EDGEADDED);
			return retVal;
		}
		return NULL;
	}

	IGraphEdge * CreateEdge(const CUniqueID & id, const CUniqueID & from, const CUniqueID & to)
	{
		CComPtr<IGraphVertex> fromV = GetVertex(from);
		CComPtr<IGraphVertex> toV = GetVertex(to);
		if (fromV && toV)
			return CreateEdge(id, fromV, toV);
		return NULL;
	}

	// images are dummy at root graph level (no useful functionality for now)

	void SetImage(const ImageInfo& /*iinfo*/, int /*overlay*/)
	{
		//m_imageMaps[static_cast<int>(iinfo.pos)][overlay]=iinfo;
	}

	const ImageInfoMaps* GetImages() const
	{
		//return &m_imageMaps;
		return 0;
	}

	void SetImageVisible(IMAGE_POSITION pos, VISIBLE visible)
	{
		m_imagevis[static_cast<unsigned int>(pos)]=visible;
	}

	VISIBLE GetImageVisible(IMAGE_POSITION pos) const
	{
		return m_imagevis[static_cast<unsigned int>(pos)];
	}

	void SetImageSize(IMAGE_POSITION pos, unsigned ovidx, const CSize& size)
	{

	}

	void GetImageSize(IMAGE_POSITION pos, unsigned ovidx, CSize& size) const
	{
		size.cx=size.cy=0;
	}


	unsigned int GetEdges(IGraphEdgeVector & results) const
	{
		edge_iterator itr, end;
		for(boost::tie(itr, end) = edges(m_graph); itr != end; ++itr)
		{
			results.push_back(GetEdge(*itr));
		}
		return results.size();
	}

	unsigned int GetEdges(IGraphEdgeVector & results, ISkipEdge * callback_func) const
	{
		edge_iterator itr, end;
		for(boost::tie(itr, end) = edges(m_graph); itr != end; ++itr)
		{
			if (!callback_func->SkipEdge(GetEdge(*itr)))
			{
				results.push_back(GetEdge(*itr));
			}
		}
		return results.size();
	}

	IEdgeIterator * GetEdgeIterator()
	{
		return GetUserEdgeIterator(NULL);
	}

	IEdgeIterator * GetUserEdgeIterator(ISkipEdge * callback_func)
	{
		edge_iterator itr, end;
		boost::tie(itr, end) = edges(m_graph);
		return new EdgeIteratorAdapter<thisClass, edge_iterator>(this, itr, end, callback_func);
	}

	unsigned int GetEdgeCount() const
	{
		return (unsigned int) num_edges(m_graph);
	}

	IVertexShortestPath * CalculateShortestPaths(const CUniqueID & _from)
	{
		CVertexShortestPath<thisClass> * retVal = new CVertexShortestPath<thisClass>(this);
		CComPtr<IGraphVertex> from = GetVertex(_from);
		CalculateShortestPaths(GetVertexDescriptor<thisClass>(from), retVal->m_shortestPath);
		return retVal;
	}

	void CalculateShortestPaths(vertex_descriptor from, ShortestPath<vertex_descriptor> &result)
	{
		if (from == UNKNOWN_VERTEX)
		{
			return;
		}

		result[from] = from;
		ShortestPathRecorder<ShortestPath<vertex_descriptor> *> vis(&result);
		breadth_first_search(m_graph, from, color_map(get(&VertexProps::color, m_graph)).visitor(vis));
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

	int GetCacheID() const
	{
		return m_id.GetHash();
	}

#if BOOST_VERSION >= 103300
	//  Layouts
	void circleLayout(int radius)
	{
	//	boost::circle_graph_layout(m_graph, m_vertex_pos, radius);
	}

	class progress_cooling : public boost::linear_cooling<double>
	{
	typedef boost::linear_cooling<double> inherited;

	public:
		explicit progress_cooling(std::size_t iterations) : inherited(iterations) 
		{
			display.reset(new boost::progress_display(iterations + 1, std::cerr));
		}

		double operator()()
		{
			++(*display);
			return inherited::operator()();
		}

	private:
		boost::shared_ptr<boost::progress_display> display;
	};

	void fruchterman_reingold_force_directed_layout(IPositionHelper * posHelper, double width, double height, int iterations = 100)
	{
		/*  More work
		typedef std::vector<boost::simple_point<double> > PositionVec;
		PositionVec position_vec(num_vertices(m_graph));
		typedef boost::iterator_property_map<PositionVec::iterator, boost::property_map<GraphType, boost::vertex_index_t>::type> PositionMap;
		PositionMap position(position_vec.begin(), get(boost::vertex_index, m_graph));

		typedef std::vector<double> WeightVec;
		WeightVec weight_vec(num_vertices(m_graph));
		typedef boost::iterator_property_map<WeightVec::iterator, boost::property_map<GraphType, boost::vertex_index_t>::type> WeightMap;
		WeightMap weight(weight_vec.begin(), get(boost::vertex_index, m_graph));

		vertex_iterator vi, vi_end;
		for (boost::tie(vi, vi_end) = vertices(m_graph); vi != vi_end; ++vi) 
		{
			posHelper->GetPosition(GetVertex(*vi), position[*vi].x, position[*vi].y);
		}

		//boost::kamada_kawai_spring_layout(m_graph, position, weight, boost::side_length(50.0));
		boost::fruchterman_reingold_force_directed_layout(m_graph, position, width, height, cooling(progress_cooling(iterations)).force_pairs(boost::make_grid_force_pairs(width, height, position, m_graph)));

		for (boost::tie(vi, vi_end) = vertices(m_graph); vi != vi_end; ++vi) 
		{
			posHelper->SetPosition(GetVertex(*vi), position[*vi].x, position[*vi].y);
		}
		*/
	}
#endif
};

typedef boost::adjacency_list<boost::vecS, boost::listS, boost::bidirectionalS, VertexProps, EdgeProps, GraphProps> DirectedGraph;
typedef CGraphDataT<DirectedGraph> CDirectedGraphData;
typedef boost::adjacency_list<boost::vecS, boost::listS, boost::undirectedS, VertexProps, EdgeProps, GraphProps> UndirectedGraph;
typedef CGraphDataT<UndirectedGraph> CUndirectedGraphData;
