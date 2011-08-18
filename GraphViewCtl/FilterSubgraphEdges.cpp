#include "StdAfx.h"

#include "FilterSubgraphEdges.h"

//  ===========================================================================
CFilterSubgraphEdges::CFilterSubgraphEdges(CGraphViewCanvas & view) : m_view(view)
{
}

CFilterSubgraphEdges::~CFilterSubgraphEdges()
{
}

bool CFilterSubgraphEdges::EnsureVisible(IGraphSubgraph * sg) const
{
	bool retVal = false;
	if (!sg)
		return retVal;
	IGraphSubgraphVector ancestors;
	GetAncestors(sg, ancestors);
	ancestors.push_back(sg);
	for(IGraphSubgraphVector::const_iterator itr = ancestors.begin(); itr != ancestors.end(); ++itr)
	{
		SGV::CGraphSubgraph subgraph(m_view, itr->get()->GetID());
		if (subgraph.GetMinimized())
		{
			subgraph.SetMinimized(false);
			retVal = true;
		}
	}
	return retVal;
}

bool CFilterSubgraphEdges::EnsureVisible(IGraphVertex * v) const
{
	bool retVal = false;
	if (!v)
		return retVal;
	IGraphSubgraphVector ancestors;
	GetAncestors(v, ancestors);
	for(IGraphSubgraphVector::const_iterator itr = ancestors.begin(); itr != ancestors.end(); ++itr)
	{
		SGV::CGraphSubgraph subgraph(m_view, itr->get()->GetID());
		if (subgraph.GetMinimized())
		{
			subgraph.SetMinimized(false);
			retVal = true;
		}
	}
	return retVal;
}

bool CFilterSubgraphEdges::EnsureVisible(IGraphEdge * e) const
{
	bool retVal = false;
	if (!e)
		return retVal;
	retVal = EnsureVisible(e->GetSource()) || retVal;
	retVal = EnsureVisible(e->GetTarget()) || retVal;
	return retVal;
}

bool CFilterSubgraphEdges::IsMinimized(IGraphSubgraph * sg) const
{
	SGV::CGraphSubgraph subgraph(m_view, sg->GetID());
	return subgraph.GetMinimized();
}

void CFilterSubgraphEdges::GetAncestors(IGraphItem * item, IGraphSubgraphVector & ancestors) const
{
	IGraphSubgraph * subgraph = item->GetParent();
	while(subgraph)
	{
		ancestors.push_back(subgraph);
		subgraph = subgraph->GetParent();
	}
}

IGraphItem * CFilterSubgraphEdges::GetHighestMinimizedSubgraph(IGraphSubgraphVector & ancestors, IGraphItem * defItem) const
{
	for(IGraphSubgraphVector::reverse_iterator itr = ancestors.rbegin(); itr != ancestors.rend(); ++itr)
	{
		if (IsMinimized(itr->get()))
			return itr->get();
	}
	return defItem;
}

IGraphItem * CFilterSubgraphEdges::GetHighestMinimizedItem(IGraphVertex * vertex) const
{
	IGraphSubgraphVector ancestors;
	GetAncestors(vertex, ancestors);
	return GetHighestMinimizedSubgraph(ancestors, vertex);
}

bool CFilterSubgraphEdges::IsAncestorHidden(IGraphSubgraphVector & ancestors) const
{
	for(IGraphSubgraphVector::iterator itr = ancestors.begin(); itr != ancestors.end(); ++itr)
	{
		if (!itr->get()->IsVisible())
			return true;
	}
	return false;
}

void CFilterSubgraphEdges::CalcEdge(IGraphEdge * e, CComPtr<IGraphItem> & ss, CComPtr<IGraphItem> & st) const
{
	e->SetPropertyUnknown(PROP_EDGE_VISIBLESOURCE, NULL);
	e->SetPropertyUnknown(PROP_EDGE_VISIBLETARGET, NULL);

	ToFromPair toFrom = std::make_pair(e->GetSource(), e->GetTarget());
	if (!toFrom.first->IsVisible())
	{
		m_show[e] = BoolToFromPair(false, toFrom);
		m_noshow[e] = BoolToFromPair(true, toFrom);
		return;
	}
	if (!toFrom.second->IsVisible())
	{
		m_show[e] = BoolToFromPair(false, toFrom);
		m_noshow[e] = BoolToFromPair(true, toFrom);
		return;
	}

	IGraphSubgraphVector s_ancestors;
	GetAncestors(toFrom.first.get(), s_ancestors);
	if (IsAncestorHidden(s_ancestors))
	{
		m_show[e] = BoolToFromPair(false, toFrom);
		m_noshow[e] = BoolToFromPair(true, toFrom);
		return;
	}

	IGraphSubgraphVector t_ancestors;
	GetAncestors(toFrom.second.get(), t_ancestors);
	if (IsAncestorHidden(t_ancestors))
	{
		m_show[e] = BoolToFromPair(false, toFrom);
		m_noshow[e] = BoolToFromPair(true, toFrom);
		return;
	}

	//  Locate highest minimized ancestor
	toFrom.first = GetHighestMinimizedSubgraph(s_ancestors, toFrom.first);
	toFrom.second = GetHighestMinimizedSubgraph(t_ancestors, toFrom.second);
	bool ancestorIsMinimized = (toFrom.first != e->GetSource() || toFrom.second != e->GetTarget());

	//  Skip all edges contained within a common minimized subgraph
	if (toFrom.first == toFrom.second)
	{
		m_show[e] = BoolToFromPair(false, toFrom);
		m_noshow[e] = BoolToFromPair(true, toFrom);
		return;
	}

	if (!m_noshow[e].first)
	{
		if (!m_show[e].first && (!ancestorIsMinimized || m_dedup[toFrom] == false))
		{
			m_dedup[toFrom] = true;
			m_show[e] = BoolToFromPair(true, toFrom);
			e->SetPropertyUnknown(PROP_EDGE_VISIBLESOURCE, toFrom.first);
			e->SetPropertyUnknown(PROP_EDGE_VISIBLETARGET, toFrom.second);
		}
		else
		{
			m_show[e] = BoolToFromPair(false, toFrom);
			m_noshow[e] = BoolToFromPair(true, toFrom);
		}
	}
}

void CFilterSubgraphEdges::CalcEdge(IGraphEdge * e) const
{
	CComPtr<IGraphItem> ss;
	CComPtr<IGraphItem> st;
	CalcEdge(e, ss, st);
}

void CFilterSubgraphEdges::Reset()
{
	m_show.clear();
	m_noshow.clear();
	m_dedup.clear();
}

bool CFilterSubgraphEdges::SkipEdge(IGraphEdge * e) const
{
	return m_show[e].first == false; 
}
