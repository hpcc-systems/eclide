#pragma once

#include "GraphContainer.h"

class CSortedGraphItem
{
    struct GraphItem
    {
        CUniqueID m_id;
        CComPtr<IGraphItem> m_item;
        GraphItem(const CUniqueID & id, IGraphItem * item) : m_id(id), m_item(item)
        {
        }
    };
    struct m_id{};

    typedef boost::multi_index_container<
    GraphItem,
    boost::multi_index::indexed_by<
        boost::multi_index::sequenced<>, // list-like index
        boost::multi_index::ordered_unique<boost::multi_index::tag<m_id>, boost::multi_index::member<GraphItem, CUniqueID, & GraphItem::m_id> > 
    >
    > GraphItemContainer;
    typedef GraphItemContainer::nth_index<0>::type GraphItemContainerSequenceIndex;
    typedef GraphItemContainer::nth_index<1>::type GraphItemContainerUniqueIDIndex;

protected:
    GraphItemContainer m_items;
    ISkipItem * m_graphSkipper;

public:
    CSortedGraphItem(ISkipItem * graphSkipper);
    void AddItem(IGraphItem * item);
    void InsertItem(IGraphItem * item, int pos);
    void UpdateItem(IGraphItem * item);
    void RemoveItem(const CUniqueID &id);
    IGraphItem * GetItem(const CUniqueID &id) const;
    IGraphItem * GetItem(unsigned int pos) const;
    unsigned int GetItems(IGraphItemVector & results) const;
    unsigned int GetItems(IGraphItemVector & results, ISkipItem * skipper) const;
    IGraphItemIterator * GetItemIterator();
    IGraphItemIterator * GetItemIterator(ISkipItem * callback_func);
    IGraphItemIterator * GetItemRIterator(ISkipItem * callback_func);
    void MoveToEnd(const CUniqueID &id);
    void SetIndex(const CUniqueID &id, int pos);
    int GetIndex(const CUniqueID &id) const;
};
