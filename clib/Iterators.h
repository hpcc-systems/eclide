#pragma once

#include "UniqueID.h"

enum TRISTATE
{
    TRISTATE_BOTH,
    TRISTATE_TRUE,
    TRISTATE_FALSE
};

// enumerator matching .Net IEnumerator behaviour (i.e. MoveNext move's first the first time)
__interface ISGVEnumerator
{
    void Reset();
    bool MoveNext();
};

template<class T>
class ISGVContainer : public ISGVEnumerator
{
public:
    virtual void Add(const T& elem)=0;
    virtual T Get()=0;
};

template<class T>
class CContainer : public ISGVContainer<T>
{
    typedef std::vector<T> _T_items;
protected:
    bool m_first;
    typename _T_items m_items;
    typename _T_items::iterator m_itr;
#ifdef _DEBUG
    bool m_iter_inited;
#endif

public:
    CContainer() : m_first(true)
    {
#ifdef _DEBUG
        m_iter_inited=false;
#endif
    }

#ifdef _DEBUG
    virtual ~CContainer()
    {
        m_iter_inited=false;
    }
#endif
    // stl accessors
    void clear()
    {
        m_items.clear();
    }

    typename _T_items::size_type size()
    {
        return m_items.size();
    }

    typename _T_items::const_reference operator[] (typename _T_items::size_type _Pos) const
    {
        return m_items[_Pos];
    }

    // ISGVContainer

    void Add(const T& elem)
    {
        m_items.push_back(elem);
    }

    T Get()
    {
#ifdef _DEBUG
        ATLASSERT(m_iter_inited);
#endif
        return *m_itr;
    }

    // ISGVEnumerator

    void Reset()
    {
        m_first = true;
    }

    // MoveNext move's first the first time. (It's the new way of things)
    bool MoveNext()
    {
        if (m_first)
        {
            m_itr = m_items.begin();
            m_first = false;
#ifdef _DEBUG
            m_iter_inited=true;
#endif

        }
        else
        {
            ++m_itr;
        }
        return m_itr != m_items.end();
    }

    bool MovePrevious()
    {
        if (m_first)
        {
            m_itr = m_items.end();
            m_first = false;
#ifdef _DEBUG
            m_iter_inited=true;
#endif

        }
        if (m_itr == m_items.begin())
            return false;
        --m_itr;
        return true;
    }
};

typedef ISGVContainer<CCategory> ICategoryContainer;
typedef CContainer<CCategory> CCategoryVector;

typedef ISGVContainer<CUniqueID> IUniqueIDContainer;
typedef CContainer<CUniqueID> CUniqueIDVector;

typedef ISGVContainer<int> IPropertyIDContainer;
typedef CContainer<int> CPropertyIDVector;
