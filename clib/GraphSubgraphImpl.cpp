#include "StdAfx.h"

#include "GraphSubgraphImpl.h"
#include "GraphVertexImpl.h"
#include "Cache.h"

template<typename G>
class CSubgraph : public IGraphSubgraph, public CUnknown
{
protected:
	CUniqueID m_id;
	std::_tstring m_label;

	GraphTypes::Color		m_color_border;
	GraphTypes::Color		m_color_fill;
	GraphTypes::Color		m_color_font;
	VISIBLE					m_visible;

	ImageInfoMaps			m_imageMaps;
	VISIBLE					m_imagevis[9];

	G & m_graph;

	PropertyMap m_graph_property;
	PropertyUnknownMap m_graph_propertyunknown;
	PropertyUnknownMap m_graph_internalunknown;

	CSortedGraphItem m_graph_sorteditems;
	IGraphSubgraph * m_parent;
	graph_signal_type m_signal;

public:
	BEGIN_CUNKNOWN
		IMPLEMENT_INTERFACE(IGraphItem)
		IMPLEMENT_INTERFACE(IGraphSubgraph)
	END_CUNKNOWN(CUnknown)

	CSubgraph(G & graph, IGraphSubgraph * parent, const CUniqueID & id) : m_graph_sorteditems(&m_graph.m_unifiedFilter), m_graph(graph), m_parent(parent)
	{
		m_id = id;
		m_visible = VISIBLE_DEFAULT;
		for(int i=0; i<9; ++i)
			m_imagevis[i]=VISIBLE_DEFAULT;
	}

	IGraphSubgraph * CreateSubgraph(const CUniqueID & id, const TCHAR* label)
	{
		CComPtr<IGraphSubgraph> retVal = CreateIGraphSubgraph(m_graph, this, id);
		retVal->SetLabel(label);
		m_graph_sorteditems.AddItem(retVal);
		Signal(GRAPHNOTIFICATION_SUBGRAPHADDED);
		return retVal;

		//ATLASSERT(!"TODO");
		////GraphType &subgraph = m_subgraph->create_subgraph();
		////m_graph.m_graph_id[&subgraph] = id;
		////m_graph.m_graph_label[&subgraph] = label;
		////CComPtr<IGraphSubgraph> retVal = CreateIGraphSubgraph(m_graph, &subgraph);
		////m_graph.m_graph_sorteditems[m_subgraph].AddItem(retVal);
		//Signal(GRAPHNOTIFICATION_SUBGRAPHADDED);
		//return NULL;
	}

	~CSubgraph()
	{
	}

	void Delete(bool promoteChildren)
	{
		if (promoteChildren)
		{
			CComPtr<IGraphItemIterator> itr = m_graph_sorteditems.GetItemIterator();
			while(itr->IsValidItem())
			{
				m_parent->AttachItem(itr->Get());
				DetachItem(itr->Get());
				itr = m_graph_sorteditems.GetItemIterator();
			}
		}
		AddRef();
		m_graph.m_subgraphCache.Clear(GetCacheID());
		ATLASSERT(!m_graph.m_subgraphCache.Exists(GetCacheID()));
		m_parent->RemoveChild(this);
		m_graph_propertyunknown.clear();
		m_graph_internalunknown.clear();
		m_parent->Signal(GRAPHNOTIFICATION_SUBGRAPHREMOVED);
		Release(); //  Release frees m_parent and must be called last
	}

	const CCategory & GetCategory() const
	{
		return m_id.GetCategory();
	}
	const CUniqueID & GetID() const
	{
		return m_id;
	}
	int GetCacheID() const
	{
		return m_id.GetHash();
	}
	void SetID(const CUniqueID & id)
	{
		m_id = id;
	}
	const TCHAR * GetLabel() const
	{
		return m_label.c_str();
	}
	void SetLabel(const TCHAR* label)
	{
		m_label = label;
		m_signal(GRAPHNOTIFICATION_LABELCHANGED, this);
	}
	VISIBLE GetVisible() const
	{
		return m_visible;
	}
	void SetVisible(VISIBLE visible)
	{
		m_visible = visible;
	}
	bool IsVisible() const
	{
		return !m_graph.m_subgraphFilter.m_skipper || m_graph.m_subgraphFilter.m_skipper->SkipSubgraph((IGraphSubgraph *)this) == false;
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

	IUnknown * GetPropertyUnknown(int key) const
	{
		PropertyUnknownMap::const_iterator itr = m_graph_propertyunknown.find(key);
		if (itr != m_graph_propertyunknown.end())
		{
			return itr->second;
		}
		return NULL;
	}
	void SetPropertyUnknown(int key, IUnknown * val)
	{
		m_graph_propertyunknown[key] = val;
	}
	IUnknown * GetInternalUnknown(int key) const
	{
		PropertyUnknownMap::const_iterator itr = m_graph_internalunknown.find(key);
		if (itr != m_graph_internalunknown.end())
		{
			return itr->second;
		}
		return NULL;
	}
	void SetInternalUnknown(int key, IUnknown * val)
	{
		m_graph_internalunknown[key] = val;
	}
	bool Equals(IGraphSubgraph * other) const
	{
		return (GetID() == other->GetID());
	}
	IGraphSubgraph * GetParent() const
	{
		return m_parent;
	}
	void SetParent(IGraphSubgraph* subgraph)
	{
		if(subgraph!=m_parent)
		{
			if(m_parent)
				m_parent->Signal(GRAPHNOTIFICATION_SUBGRAPHREMOVED);
			m_parent=subgraph;		
			if(subgraph)
				subgraph->Signal(GRAPHNOTIFICATION_SUBGRAPHADDED);
		}
	}
	void * GetIndex() const
	{
		return (void *)this;
	}
	unsigned int GetOrder() const
	{
		return m_parent->GetChildIndex(this);//m_graph.m_graph_sorteditems[m_parent].GetIndex(GetID());
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
	void MoveToEnd()
	{
		m_parent->MoveToEnd(this);
	}
	void MoveToEnd(const IGraphItem * child)
	{
		m_graph_sorteditems.MoveToEnd(child->GetID());
	}
	bool IsChild(IGraphVertex * vertex) const
	{
		if (!vertex)
		{
			return false;
		}
		CComPtr<IGraphSubgraph> parent = vertex->GetParent();
		return (GetID() == parent->GetID());
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
	IGraphVertex * CreateVertex(const CUniqueID &id)
	{
		CComPtr<IGraphVertex> retVal = CreateIGraphVertex(m_graph, this, add_vertex(m_graph.m_fullGraph), id);
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
		CComPtr<IGraphVertex> retVal = m_graph.GetVertex(id);
		if (IsChild(retVal))
		{
			return retVal;
		}
		return NULL;
	}

	IGraphVertex * GetVertex(unsigned int hash) const
	{
		CComPtr<IGraphVertex> retVal = m_graph.GetVertex(hash);
		if (IsChild(retVal))
		{
			return retVal;
		}
		return NULL;
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
		m_imagevis[static_cast<unsigned int>(pos)]=visible;
	}

	VISIBLE GetImageVisible(IMAGE_POSITION pos) const
	{
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

	//  Subgraph  ---
	IGraphSubgraph * GetSubgraph(const CUniqueID & id) const
	{
		IGraphSubgraph * retVal = m_graph.GetSubgraph(id);
		if (IsChild(retVal))
		{
			return retVal;
		}
		CComPtr<IGraphSubgraph> dispose = retVal;
		return NULL;
	}
	IGraphSubgraph * GetSubgraph(unsigned int hash) const
	{
		IGraphSubgraph * retVal = m_graph.GetSubgraph(hash);
		if (IsChild(retVal))
		{
			return retVal;
		}
		CComPtr<IGraphSubgraph> dispose = retVal;
		return NULL;
	}
	bool IsChild(IGraphSubgraph * subgraph) const
	{
		if (!subgraph)
		{
			return false;
		}
		CComPtr<IGraphSubgraph> parent = subgraph->GetParent();
		return (GetID() == parent->GetID());
	}

	//  Edge  ---
	IGraphEdge * GetEdge(const CUniqueID & id) const
	{
		CComPtr<IGraphEdge> retVal = m_graph.GetEdge(id);
		if (IsChild(retVal->GetSource()) || IsChild(retVal->GetTarget()))
		{
			return retVal;
		}
		return NULL;
	}

	IGraphEdge * GetEdge(unsigned int hash) const
	{
		CComPtr<IGraphEdge> retVal = m_graph.GetEdge(hash);
		if (IsChild(retVal->GetSource()) || IsChild(retVal->GetTarget()))
		{
			return retVal;
		}
		return NULL;
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
IGraphSubgraph * CreateIGraphSubgraph(GraphData & graph, IGraphSubgraph * parent, const CUniqueID & id)
{
	return graph.m_subgraphCache.Get(new CSubgraph<GraphData>(graph, parent, id));
}
//template IGraphSubgraph * CreateIGraphSubgraph<CGraphDataT<UndirectedGraph> >(CGraphDataT<UndirectedGraph> & graph, IGraphSubgraph * parent, const CUniqueID & id);
//template IGraphSubgraph * CreateIGraphSubgraph<CGraphDataT<DirectedGraph> >(CGraphDataT<DirectedGraph> & graph, IGraphSubgraph * parent, const CUniqueID & id);

void testSubgraph()
{
	CUndirectedGraphData g1;
	CreateIGraphSubgraph(g1, NULL, CUniqueID());

	CDirectedGraphData g2;
	CreateIGraphSubgraph(g2, NULL, CUniqueID());
}