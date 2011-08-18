#pragma once

//  ===========================================================================
template<typename G, typename C, typename IT, typename I>
class IteratorC : public I, public CUnknown
{
protected:
	const G &m_graph;

	IT m_itr;
	IT m_end;

public:
	IMPLEMENT_CUNKNOWN;

	IteratorC(const G &graph) : m_graph(graph)
	{
	}

	IteratorC(const G &graph, const std::pair<IT, IT> &itr) : m_graph(graph)
	{
		boost::tie(m_itr, m_end) = itr;
	}

	void Init()
	{
		GotoNext();
	}

	bool IsValidItem()
	{
		return (m_itr != m_end);
	}

	void NextItem()
	{
		++m_itr;
		GotoNext();
	}

	void GotoNext()
	{
		C* pT = static_cast<C*>(this);
		while(IsValidItem())
		{
			if (!pT->SkipItem())
				break;
			++m_itr;
		}
	}
};
//  ===========================================================================
template<typename G, typename C, typename T, typename IT, typename I>
class IteratorT : public IteratorC<G, C, IT, I>
{
	typedef IteratorC<G, C, IT, I> baseClass;
public:
	IteratorT(const G &graph, const std::pair<IT, IT> &itr) : baseClass(graph, itr)
	{
	}

	const T Get()
	{
		return *m_itr;
	}

};
//  ===========================================================================
template<typename G, typename C, typename I>
class StackIteratorC : public I, public CUnknown
{
protected:
	const G &m_graph;
	std::stack<StlLinked<I> > m_itr;

public:
	IMPLEMENT_CUNKNOWN;

	StackIteratorC(const G &graph) : m_graph(graph)
	{
	}

	void Init()
	{
		GotoNext();
	}

	bool IsValidItem()
	{
		while (m_itr.size())
		{
			if (m_itr.top()->IsValidItem())
			{
				return true;
			}
			else
			{
				m_itr.pop();
			}
		}
		return false;
	}

	void NextItem()
	{
		m_itr.top()->NextItem();
		GotoNext();
	}

	void GotoNext()
	{
		C* pT = static_cast<C*>(this);
		while(IsValidItem())
		{
			if (!pT->SkipItem())
				break;
			m_itr.top()->NextItem();
		}
	}

};
//  ===========================================================================
template<typename G, typename C,  typename T, typename I>
class StackIteratorT : public StackIteratorC<G, C, I>
{
	typedef StackIteratorC<G, C, I> baseClass;
public:
	StackIteratorT(const G &graph) : baseClass(graph)
	{
	}
	const T Get()
	{
		return m_itr.top()->Get();
	}
};
//  ===========================================================================
template<typename G, typename C, typename IT, typename I>
class IteratorTVertex : public IteratorC<G, C, IT, I>
{
	typedef IteratorC<G, C, IT, I> baseClass;
public:
	IteratorTVertex(const G &graph, const std::pair<IT, IT> &itr) : baseClass(graph, itr)
	{
	}

	IGraphVertex * Get()
	{
		return GetItem();
	}
	IGraphVertex * GetItem()
	{
		//G & gd = const_cast<G &>(m_graph);
		return m_graph.GetVertex(*m_itr);
	}
};
//  ===========================================================================
template<typename G, typename I = typename G::vertex_iterator>
class UserVertexIterator : public IteratorTVertex<G, UserVertexIterator<G, I>, I, IVertexIterator>
{
	typedef IteratorTVertex<G, UserVertexIterator<G, I>, I, IVertexIterator> baseClass;
protected:
	CComPtr<ISkipVertex> m_callback_func;

public:
	UserVertexIterator(const G &graph, ISkipVertex * callback_func) : m_callback_func(callback_func), baseClass(graph, vertices(graph.m_graph))
	{
	}
	bool SkipItem()
	{
		CComPtr<IGraphVertex> v = GetItem();
		return (m_callback_func->SkipVertex(v));
	}
};
//  ===========================================================================
template<typename G, typename C, typename IT, typename I>
class IteratorTEdge : public IteratorC<G, C, IT, I>
{
	typedef IteratorC<G, C, IT, I> baseClass;
public:
	IteratorTEdge(const G &graph, const std::pair<IT, IT> &itr) : baseClass(graph, itr)
	{
	}

	IGraphEdge * Get()
	{
		//G &graph = const_cast<G &>(m_graph);
		return m_graph.GetEdge(*m_itr);
	}

};
//  ===========================================================================
template<typename G, typename I = typename G::edge_iterator>
class UserEdgeIterator : public IteratorTEdge<G, UserEdgeIterator<G, I>, I, IEdgeIterator>
{
	typedef IteratorTEdge<G, UserEdgeIterator<G, I>, I, IEdgeIterator> baseClass;
protected:
	CComPtr<ISkipEdge> m_callback_func;

public:
	UserEdgeIterator(const G &graph, const std::pair<I, I> &itr, ISkipEdge * callback_func) : m_callback_func(callback_func), baseClass(graph, itr)
	{
	}
	bool SkipItem()
	{
		return (m_callback_func->SkipEdge(Get()));
	}
};
//  ===========================================================================
template<typename G, typename C, typename I>
class StackIteratorTEdge : public StackIteratorC<G, C, I>
{
	typedef StackIteratorC<G, C, I> baseClass;
public:
	StackIteratorTEdge(const G &graph) : baseClass(graph)
	{
	}
	IGraphEdge * Get()
	{
		ATLASSERT(!m_itr.empty());
		return m_itr.top()->Get();
	}
};
//  ===========================================================================
//class SubgraphIterator : public IteratorC<SubgraphIterator, children_iterator, IGraphSubgraphIterator>
//{
//	typedef IteratorC<SubgraphIterator, children_iterator, IGraphSubgraphIterator> baseClass;
//public:
//	SubgraphIterator(const CGraphData &graph, const std::pair<children_iterator, children_iterator> &itr) : baseClass(graph, itr)
//	{
//	}
//	IGraphSubgraph * Get()
//	{
//		return CreateIGraphSubgraph(const_cast<CGraphData &>(m_graph), &*m_itr);
//	}
//	bool SkipItem()
//	{
//		return false;
//	}
//};
//  ===========================================================================
class AllVertex : public ISkipVertex, public CUnknown
{
public:
	IMPLEMENT_CUNKNOWN;

	bool SkipVertex(IGraphVertex * /*v*/) const
	{
		return false;
	}
};

//  ===========================================================================
template<typename G, typename I = typename G::in_edge_iterator>
class InEdgeIterator : public IteratorTEdge<G, InEdgeIterator<G>, I, IEdgeIterator>
{
	typedef IteratorTEdge<G, InEdgeIterator<G>, I, IEdgeIterator> baseClass;
public:
	InEdgeIterator(const G &graph, const std::pair<I, I> & itr) : baseClass(graph, itr)
	{
	}
	bool SkipItem()
	{
		return false;
	}
};
//  ===========================================================================
template<typename G, typename I = typename G::out_edge_iterator>
class OutEdgeIterator : public IteratorTEdge<G, OutEdgeIterator<G>, I, IEdgeIterator>
{
	typedef IteratorTEdge<G, OutEdgeIterator<G>, I, IEdgeIterator> baseClass;
public:
	OutEdgeIterator(const G &graph, const std::pair<I, I> & itr) : baseClass(graph, itr)
	{
	}
	bool SkipItem()
	{
		return false;
	}
};
//  ===========================================================================
template<typename G, typename I = typename G::adjacency_iterator>
class AdjacentVertexIterator : public IteratorTVertex<G, AdjacentVertexIterator<G, I>, I, IVertexIterator>
{
	typedef IteratorTVertex<G, AdjacentVertexIterator<G, I>, I, IVertexIterator> baseClass;
public:
	AdjacentVertexIterator(const G &graph, const std::pair<I, I> &itr) : baseClass(graph, itr)
	{
	}
	bool SkipItem()
	{
		return false;
	}
};
//  ===========================================================================
template<typename G, typename V = typename G::vertex_descriptor>
class AdjacentEdgeIterator : public StackIteratorTEdge<G, AdjacentEdgeIterator<G>, IEdgeIterator>
{
	typedef StackIteratorTEdge<G, AdjacentEdgeIterator, IEdgeIterator> baseClass;
protected:
	std::map<CUniqueID, bool> m_dedup;

public:
	AdjacentEdgeIterator(const G &graph, V v) : baseClass(graph)
	{
		StlLinked<InEdgeIterator<G> > inItr = new InEdgeIterator<G>(graph, in_edges(v, graph.m_graph));
		inItr->Init();
		StlLinked<OutEdgeIterator<G> > outItr = new OutEdgeIterator<G>(graph, out_edges(v, graph.m_graph));
		outItr->Init();
		m_itr.push(inItr.get());
		m_itr.push(outItr.get());
	}

	bool SkipItem()
	{
		CComPtr<IGraphEdge> edge = Get();
		if (!edge)
			return true;

		if (m_dedup[edge->GetID()] == false)
		{
			m_dedup[edge->GetID()] = true;
			return false;
		}
		return true;
	}
};
//  ===========================================================================
template<typename G>
class SubgraphVertexIterator : public IteratorC<G, SubgraphVertexIterator<G>, IGraphItemVector::iterator, IVertexIterator>
{
	typedef IteratorC<G, SubgraphVertexIterator, IGraphItemVector::iterator, IVertexIterator> baseClass;

public:
	IGraphItemVector m_items;

	SubgraphVertexIterator(const G &graph) : baseClass(graph)
	{
		graph.GetChildren(m_items);
		m_itr = m_items.begin();
		m_end = m_items.end();
	}

	bool SkipItem()
	{
		CComQIPtr<IGraphVertex> retVal = m_itr->get();
		return (retVal == NULL);
	}

	IGraphVertex * Get()
	{
		CComQIPtr<IGraphVertex> retVal = m_itr->get();
		ATLASSERT(retVal);
		return retVal;
	}
};
//  ===========================================================================
template <typename G, typename T>
class EdgeIteratorAdapter : public CUnknown, public IEdgeIterator
{
private:
	G *m_data;
	T m_itr;
	T m_end;
	ISkipEdge * m_callback_func;

public:
	EdgeIteratorAdapter(G *data, const T & itr, const T & end, ISkipEdge * callback_func = NULL) : m_data(data), m_itr(itr), m_end(end), m_callback_func(callback_func)
	{
		GotoNext();
	}

	BEGIN_CUNKNOWN
	END_CUNKNOWN(CUnknown)

	bool IsValidItem()
	{
		return m_itr != m_end;
	}
	void NextItem()
	{
		++m_itr;
		GotoNext();
	}
	IGraphEdge * Get()
	{
		return m_data->GetEdge(*m_itr);
	}
	void GotoNext()
	{
		if (m_callback_func)
		{
			while(m_itr != m_end)
			{
				if (!m_callback_func->SkipEdge(m_data->GetEdge(*m_itr)))
					break;
				++m_itr;
			}
		}
	}
};
//  ===========================================================================
