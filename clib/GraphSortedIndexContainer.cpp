#include "stdafx.h"

#include "GraphSortedIndexContainer.h"

template<class T>
class SortedItemIteratorAdapter : public CUnknown, public IGraphItemIterator
{
private:
	T m_itr;
	T m_end;
	ISkipItem * m_callback_func;
	ISkipItem * m_graphSkipper;

public:
	SortedItemIteratorAdapter(const T & itr, const T & end, ISkipItem * graphSkipper, ISkipItem * callback_func = NULL) : m_itr(itr), m_end(end), m_graphSkipper(graphSkipper), m_callback_func(callback_func)
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
	IGraphItem * Get()
	{
		return m_itr->m_item;
	}
	void GotoNext()
	{
		while(m_itr != m_end)
		{
			bool skip = false;
			if (m_graphSkipper->SkipItem(m_itr->m_item))
				skip = true;
			if (m_callback_func && m_callback_func->SkipItem(m_itr->m_item))
				skip = true;
			if (!skip)
				break;
			++m_itr;
		}
	}
};

CSortedGraphItem::CSortedGraphItem(ISkipItem * graphSkipper) 	
{
	m_graphSkipper = graphSkipper;
}
void CSortedGraphItem::AddItem(IGraphItem * item)
{
	m_items.push_back(GraphItem(item->GetID(), item));
}
void CSortedGraphItem::InsertItem(IGraphItem * item, int pos)
{
	for(GraphItemContainerSequenceIndex::iterator itr = m_items.get<0>().begin(); itr != m_items.get<0>().end(); ++itr, --pos)
	{
		if(pos == 0)
		{
			m_items.insert(itr, GraphItem(item->GetID(), item));
			return;
		}
	}
	AddItem(item);
}
void CSortedGraphItem::UpdateItem(IGraphItem * item)
{
	GraphItemContainerUniqueIDIndex::iterator itr = m_items.get<1>().find(item->GetID());
	ATLASSERT(itr != m_items.get<1>().end());
	GraphItem updatedItem = *itr;
	updatedItem.m_item = item;
	m_items.get<1>().replace(itr, updatedItem);
}
void CSortedGraphItem::RemoveItem(const CUniqueID &id)
{
	//ATLASSERT(GetItem(id) != NULL);
	GraphItemContainerUniqueIDIndex::iterator it1 = m_items.get<1>().find(id);
	if (it1 != m_items.get<1>().end())
	{
		m_items.get<1>().erase(it1);
	}
}
IGraphItem * CSortedGraphItem::GetItem(const CUniqueID &id) const
{
	GraphItemContainerUniqueIDIndex::iterator it1 = m_items.get<1>().find(id);
	if (it1 != m_items.get<1>().end())
	{
		return it1->m_item;
	}
	return NULL;
}
IGraphItem * CSortedGraphItem::GetItem(unsigned int pos) const
{
	if(pos < 0 || pos >= m_items.get<0>().size())
	{
		return NULL;
	}

	GraphItemContainerSequenceIndex::iterator itr = m_items.get<0>().begin();
	while(pos > 0)
	{
		++itr;
		pos--;
	}
	return itr->m_item;
}
unsigned int CSortedGraphItem::GetItems(IGraphItemVector & results) const
{
	for(GraphItemContainerSequenceIndex::iterator itr = m_items.get<0>().begin(); itr != m_items.get<0>().end(); ++itr)
	{
		if (!m_graphSkipper->SkipItem(itr->m_item))
			results.push_back(IGraphItemAdapt(itr->m_item));
	}
	return results.size();
}
unsigned int CSortedGraphItem::GetItems(IGraphItemVector & results, ISkipItem * skipper) const
{
	for(GraphItemContainerSequenceIndex::iterator itr = m_items.get<0>().begin(); itr != m_items.get<0>().end(); ++itr)
	{
		if (!m_graphSkipper->SkipItem(itr->m_item) && !skipper->SkipItem(itr->m_item))
		{
			results.push_back(IGraphItemAdapt(itr->m_item));
		}
	}
	return results.size();
}
IGraphItemIterator * CSortedGraphItem::GetItemIterator()
{
	//  7.1 Compiler workaround - place itr/end on own lines;
	GraphItemContainerSequenceIndex::iterator itr = m_items.get<0>().begin();
	GraphItemContainerSequenceIndex::iterator end = m_items.get<0>().end();
	return new SortedItemIteratorAdapter<GraphItemContainerSequenceIndex::iterator>(itr, end, m_graphSkipper);
}
IGraphItemIterator * CSortedGraphItem::GetItemIterator(ISkipItem * callback_func)
{
	//  7.1 Compiler workaround - place itr/end on own lines;
	GraphItemContainerSequenceIndex::iterator itr = m_items.get<0>().begin();
	GraphItemContainerSequenceIndex::iterator end = m_items.get<0>().end();
	return new SortedItemIteratorAdapter<GraphItemContainerSequenceIndex::iterator>(itr, end, m_graphSkipper, callback_func);
}
IGraphItemIterator * CSortedGraphItem::GetItemRIterator(ISkipItem * callback_func)
{
	//  7.1 Compiler workaround - place itr/end on own lines;
	GraphItemContainerSequenceIndex::reverse_iterator itr = m_items.get<0>().rbegin();
	GraphItemContainerSequenceIndex::reverse_iterator end = m_items.get<0>().rend();
	return new SortedItemIteratorAdapter<GraphItemContainerSequenceIndex::reverse_iterator>(itr, end, m_graphSkipper, callback_func);
}
void CSortedGraphItem::MoveToEnd(const CUniqueID &id)
{
	CComPtr<IGraphItem> item = GetItem(id);
	RemoveItem(item->GetID());
	AddItem(item);
}
void CSortedGraphItem::SetIndex(const CUniqueID &id, int pos)
{
	CComPtr<IGraphItem> item = GetItem(id);
	RemoveItem(item->GetID());
	InsertItem(item, pos);
}
int CSortedGraphItem::GetIndex(const CUniqueID &id) const
{
	GraphItemContainerUniqueIDIndex::iterator it1 = m_items.get<1>().find(id);
	ATLASSERT(it1 != m_items.get<1>().end());
	GraphItemContainerSequenceIndex::iterator it2 = m_items.project<0>(it1);
	return std::distance(m_items.begin(), it2);
}
