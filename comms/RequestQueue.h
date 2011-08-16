#pragma once

#include "dali.h"
#include "thread.h"

namespace Dali
{
template <class R> //R should be an StlLinked<x> something
class CRequestQueue : CUnknown
{
protected:
	typedef StlLinked< CRequestQueue<R> > CRequestQueueAdapt;

	typedef CLockableEventQueue<R> Queue;
	typedef StlLinked<Queue>  QueueAdapt;
	QueueAdapt			      m_queue;

	typedef void (*WorkerFn)(R req, Dali::CServerConfig& config);

public:
	BEGIN_CUNKNOWN
	END_CUNKNOWN(CUnknown)

	CRequestQueue(WorkerFn workerFn, unsigned maxQueueSize=0)
	{
		m_handler = workerFn;
		m_maxQueueSize = maxQueueSize;
		m_handlersStarted = 0;
		m_queue = new Queue();
	}

	~CRequestQueue()
	{
	}

	void SetServerConfig(const Dali::CServerConfig &config)
	{
		m_config = config;
	}

	void clear()
	{
		//this should kill all threads
		m_queue->clear();
	}
	void addToFront(const R& req)
	{
		add(req,m_maxQueueSize,true);
	}
	void addToBack(const R& req)
	{
		add(req,m_maxQueueSize,false);
	}

protected:
	static void QueueHandler(CRequestQueueAdapt self, QueueAdapt queue, WorkerFn handler, Dali::CServerConfig config)
	{
		unsigned events = 0;
		Dali::CResultDataRequestAdapt req;
		while ( queue->RequestData(req) )
		{
			(*handler)(req,config);
			events++;
			req = 0;
		}
		self->ThreadEnding();
		ATLTRACE(_T("CRequestQueue(QueueHandler ended after %u events)\r\n"), events );
	}

	void ThreadEnding()
	{
		clib::scoped_mutex_lock scoped_lock(m_mutex);
		m_handlersStarted--;
		//there is the possiblity of the thread ending and stuff being in the queue
		if ( !m_handlersStarted && m_queue->size() )
		{
			StartThread();
		}
	}
	void StartThread()
	{
		//mutex must already be locked!!

		m_handlersStarted++;
		CRequestQueueAdapt self = this;
		//bind needs a pointer for the adapters on the other side
		clib::thread run(__FUNCTION__,boost::bind(&QueueHandler, self.get(), m_queue.get(), m_handler, m_config));
	}

	void add(const R& req, unsigned maxQueueSize, bool bAddFirst)
	{
		m_queue->push(req,maxQueueSize,bAddFirst);

		clib::scoped_mutex_lock scoped_lock(m_mutex);
		if ( !m_handlersStarted )
		{
			StartThread();
		}
	}

	clib::recursive_mutex m_mutex;
	int			 m_handlersStarted;	 
	unsigned	 m_maxQueueSize;
	Dali::CServerConfig m_config;
	WorkerFn			m_handler;
};

} //namespace Dali