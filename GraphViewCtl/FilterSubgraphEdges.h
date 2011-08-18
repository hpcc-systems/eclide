#pragma once

#include "GraphViewCanvas.h"

class CFilterSubgraphEdges : public ISkipEdge, public CUnknown
{
protected:
	CGraphViewCanvas & m_view;

	typedef std::pair<StlLinked<IGraphItem>, StlLinked<IGraphItem> > ToFromPair;
	typedef std::pair<bool, ToFromPair> BoolToFromPair;
	typedef std::map<StlLinked<IGraphEdge>, BoolToFromPair> ShowMap;
	mutable ShowMap m_show;
	mutable ShowMap m_noshow;

	typedef std::map<ToFromPair, bool> DedupMap;
	mutable DedupMap m_dedup;

public:
	BEGIN_CUNKNOWN
	END_CUNKNOWN(CUnknown)

	CFilterSubgraphEdges(CGraphViewCanvas & view);
	~CFilterSubgraphEdges();

	bool EnsureVisible(IGraphSubgraph * sg) const;
	bool EnsureVisible(IGraphVertex * v) const;
	bool EnsureVisible(IGraphEdge * e) const;
	bool IsMinimized(IGraphSubgraph * sg) const;
	void GetAncestors(IGraphItem * item, IGraphSubgraphVector & ancestors) const;
	IGraphItem * GetHighestMinimizedSubgraph(IGraphSubgraphVector & ancestors, IGraphItem * defItem) const;
	IGraphItem * GetHighestMinimizedItem(IGraphVertex * vertex) const;
	bool IsAncestorHidden(IGraphSubgraphVector & ancestors) const;
	void CalcEdge(IGraphEdge * e, CComPtr<IGraphItem> & ss, CComPtr<IGraphItem> & st) const;
	void CalcEdge(IGraphEdge * e) const;
	void Reset();
	bool SkipEdge(IGraphEdge * e) const;
};
