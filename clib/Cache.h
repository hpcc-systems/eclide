#pragma once

#include "RecursiveMutex.h"
#include "UniqueID.h"

template <typename _Kty, typename _Ty>
struct SupportsGetCacheIDConcept
{
	void constraints()
	{
		_Kty key = a->GetCacheID();
		key;
	}
	_Ty *a;
};

template<typename _Kty, typename _Ty, typename _Pr = std::less<_Kty> >
class CacheT : public std::map<_Kty, StlLinked<_Ty>, _Pr>
{
	typedef std::map<_Kty, StlLinked<_Ty>, _Pr> MapType;

	//BOOST_CLASS_REQUIRES(_Ty, SupportsStlLinkedConcept);
	//BOOST_CLASS_REQUIRES2(_Kty, _Ty, SupportsGetCacheIDConcept);

protected:
	mutable boost::recursive_mutex m_mutex;

public:
	CacheT()
	{
	}

	~CacheT()
	{
	}

	unsigned Size() const
	{
		boost::recursive_mutex::scoped_lock proc(m_mutex);
		return size();
	}

	void Clear()
	{
		boost::recursive_mutex::scoped_lock proc(m_mutex);
		clear();
	}

	void Clear(const _Kty & id)
	{
		boost::recursive_mutex::scoped_lock proc(m_mutex);
		erase(id);
	}
	
	void Clear(_Ty *item)
	{
		boost::recursive_mutex::scoped_lock proc(m_mutex);
		erase(item->GetCacheID());
	}

	_Ty* Get(_Ty * newItem)
	{
		boost::recursive_mutex::scoped_lock proc(m_mutex);
		StlLinked<_Ty> retVal = newItem;
		const _Kty & id = newItem->GetCacheID();
		MapType::iterator itr = find(id);
		if (itr == end())
		{
			this->operator[](id) = retVal;
		}
		else
		{
			retVal = itr->second.get();	//Bye Bye newItem...
		}

		return retVal;
	}

	_Ty* Exists(const _Kty & id) const
	{
		boost::recursive_mutex::scoped_lock proc(m_mutex);
		MapType::const_iterator itr = find(id);
		if (itr != end())
		{
			return itr->second.get();
		}

		return NULL;
	}
};

template<typename _Kty, typename _Ty>
class HashCacheT : public stdext::hash_map<_Kty, StlLinked<_Ty> >
{
	typedef stdext::hash_map<_Kty, StlLinked<_Ty> > MapType;

protected:
public:
	HashCacheT()
	{
	}

	~HashCacheT()
	{
	}

	unsigned Size() const
	{
		return size();
	}

	void Clear()
	{
		clear();
	}

	void Clear(const _Kty & id)
	{
		erase(id);
	}
	
	void Clear(_Ty *item)
	{
		erase(item->GetCacheID());
	}

	_Ty* Get(_Ty * newItem)
	{
		StlLinked<_Ty> retVal = newItem;
		const _Kty & id = newItem->GetCacheID();
		MapType::iterator itr = find(id);
		if (itr == end())
		{
			this->operator[](id) = retVal;
		}
		else
		{
			retVal = itr->second.get();	//Bye Bye newItem...
		}

		return retVal;
	}

	_Ty* Exists(const _Kty & id) const
	{
		MapType::const_iterator itr = find(id);
		if (itr != end())
		{
			return itr->second.get();
		}

		return NULL;
	}
};

//  ===========================================================================
template <typename T>
class CacheItem : public CUnknown
{
protected:
	boost::posix_time::ptime m_creationTime;
	const T m_item;

public:
	CacheItem(const T & item) : m_item(item)
	{
		m_creationTime = boost::posix_time::second_clock::local_time();
	}

	BEGIN_CUNKNOWN
	END_CUNKNOWN(CUnknown)

	const T & get()
	{
		return m_item;
	}

	const boost::posix_time::ptime & getCreationTime()
	{
		return m_creationTime;
	}
};

template <typename T>
class CachePool : public CUnknown
{
protected:
	typedef std::map<std::_tstring, StlLinked<CacheItem<T> > > CachedMapT;
	CachedMapT m_cachedItems;
	T m_noItems;
	boost::posix_time::time_duration m_duration;

public:
	clib::recursive_mutex m_mutex;

	BEGIN_CUNKNOWN
	END_CUNKNOWN(CUnknown)

	CachePool(unsigned int timeout = 60 * 60) : m_duration(boost::posix_time::seconds(timeout))
	{
	}
	bool needs_update(const std::_tstring & cacheID, bool noRefresh = false) const
	{
		CachedMapT::const_iterator found = m_cachedItems.find(cacheID);
		if (found == m_cachedItems.end())
			return true;
		else if (noRefresh)
			return false;
		else if ((boost::posix_time::second_clock::local_time() - found->second->getCreationTime()) > m_duration)
			return true;
		return false;
	}
	void update(const std::_tstring & cacheID, const T & items)
	{
		m_cachedItems[cacheID] = new CacheItem<T>(items);
	}
	const T & get(const std::_tstring & cacheID) const
	{
		CachedMapT::const_iterator found = m_cachedItems.find(cacheID);
		if (found != m_cachedItems.end())
			return found->second->get();
		ATLASSERT(false);
		return m_noItems;
	}
	void clear()
	{
		m_cachedItems.clear();
	}
	void clear(const std::_tstring & cacheID)
	{
		m_cachedItems.erase(cacheID);
	}
};

template <typename T>
class CachePoolMap
{
protected:
	unsigned int m_timeout;
	typedef std::map<std::_tstring, StlLinked<CachePool<T> > > StringPoolMap;
	StringPoolMap m_map;
	clib::recursive_mutex m_mutex;

public:
	CachePoolMap(unsigned int timeout = 60 * 60)
	{
		m_timeout = timeout;
	}
	void clear()
	{
		clib::recursive_mutex::scoped_lock m_lock(m_mutex);
		m_map.clear();
	}
	void clear(const std::_tstring & label)
	{
		clib::recursive_mutex::scoped_lock m_lock(m_mutex);
		m_map[label] = NULL;
	}
	CachePool<T> * get(const std::_tstring & label)
	{
		clib::recursive_mutex::scoped_lock m_lock(m_mutex);
		if (m_map[label].get() == NULL)
			m_map[label] = new CachePool<T>(m_timeout);

		return m_map[label];
	}
};

template<typename T>
class CachePoolAccessor
{
protected:
	clib::recursive_mutex::scoped_lock m_lock;
	CachePool<T> & m_pool;
	std::_tstring m_id;

public:
	CachePoolAccessor(CachePool<T> & pool, const std::_tstring & id) : m_lock(pool.m_mutex), m_pool(pool)
	{
		m_id = id;
	}

	CachePoolAccessor(CachePool<T> & pool, const std::_tstring & id, const std::_tstring & id2) : m_lock(pool.m_mutex), m_pool(pool)
	{
		m_id = id + _T("::") + id2;
	}

	CachePoolAccessor(CachePool<T> & pool, const std::_tstring & id, const std::_tstring & id2, const std::_tstring & id3) : m_lock(pool.m_mutex), m_pool(pool)
	{
		m_id = id + _T("::") + id2 + _T("::") + id3;
	}

	CachePoolAccessor(CachePool<T> & pool, const std::_tstring & id, const std::_tstring & id2, const std::_tstring & id3, const std::_tstring & id4) : m_lock(pool.m_mutex), m_pool(pool)
	{
		m_id = id + _T("::") + id2 + _T("::") + id3 + _T("::") + id4;
	}

	bool needs_update(bool noRefresh = false) const
	{
		return m_pool.needs_update(m_id, noRefresh);
	}

	void update(const T & items)
	{
		m_pool.update(m_id, items);
	}

	const T & get() const
	{
		return m_pool.get(m_id);
	}

	void clear()
	{
		m_pool.clear(m_id);
	}
};
