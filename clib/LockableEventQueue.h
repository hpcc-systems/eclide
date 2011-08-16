#pragma once
#include <RecursiveMutex.h> //clib

template <class T>
class CLockableEventQueue : public CUnknown
{
public:
	BEGIN_CUNKNOWN
	END_CUNKNOWN(CUnknown)

	CLockableEventQueue()
	{
		m_active = true;
		m_event = ::CreateEvent(NULL, FALSE, FALSE, NULL); //auto reset
	}
	~CLockableEventQueue()
	{
		::CloseHandle(m_event);
	}

	void push(const T& t, unsigned maxQueueSize=0, bool bAddFirst=false)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		if ( maxQueueSize > 0 && m_queue.size() >= maxQueueSize )
			m_queue.pop_back();
		if ( bAddFirst )
			m_queue.push_front(t);
		else
			m_queue.push_back(t);
		ATLTRACE(_T("CLockableEventQueue(push,size=%u)\r\n"), m_queue.size() );
		m_active = true;
        ::SetEvent(m_event);
	}
	void clear()
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		m_active = false;
		::SetEvent(m_event); //wake up anybody waiting
		m_queue.clear();
	}
	unsigned size()
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_queue.size();
	}
	bool RequestData(T &t)
	{
		if ( isActive() && getQueueTop(t) )
			return true;

		//we can always make more threads, so die off if no activity
		DWORD wait = ::WaitForSingleObject(m_event, 30 * 1000);
		if ( WAIT_OBJECT_0 == wait )
		{	
			if ( isActive() && getQueueTop(t) )
				return true;
		}
		return false;
	}

protected:
	bool getQueueTop(T &t)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		if ( m_queue.size() > 0 )
		{
			t = m_queue.front();
			m_queue.pop_front();

			//reset the event when we get to zero so threads have to wait!
			if ( m_queue.size() == 0 )
				::ResetEvent(m_event);

			ATLTRACE(_T("CLockableEventQueue(request,size=%u)\r\n"), m_queue.size() );
			return true;
		}
		return false;
	}
	bool isActive()
	{
		return m_active;
	}
	//because we can add to front or back of queue, can't use std::queue
	std::list<T>					m_queue;
	volatile bool					m_active;
	mutable clib::recursive_mutex	m_mutex;
	HANDLE							m_event;
};

