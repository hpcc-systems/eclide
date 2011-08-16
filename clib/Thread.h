#pragma once

#include "RecursiveMutex.h"

namespace clib
{
struct xtime;

class CLIB_API thread : private boost::noncopyable
{
public:
    thread();
	explicit thread(const std::string & label, const boost::function0<void>& threadfunc,
		int nPriorityOfThread = THREAD_PRIORITY_NORMAL);
    ~thread();

    bool operator==(const thread& other) const;
    bool operator!=(const thread& other) const;

    void join();

    static void sleep(int milliseconds);
    static void yield();

private:
    void* m_thread;
    unsigned int m_id;
    bool m_joinable;
};

typedef boost::signal<void()> worker_signal_type;
typedef worker_signal_type::slot_type worker_slot_type;

class CThreadWorker : public CUnknown, boost::noncopyable
{
protected:
	std::string m_label;
    const boost::function0<void> m_threadfunc;
	worker_signal_type on_refresh;

public:
	IMPLEMENT_CUNKNOWN;
	CThreadWorker();
	CThreadWorker(const std::string & label, const boost::function0<void>& threadfunc, const worker_slot_type& s);
	const char* GetLabel();
	void Call();
};

typedef CAdapt<CComPtr<CThreadWorker> > CThreadWorkerAdapt;

enum THREAD_EVENT
{
	THREAD_EVENT_UNKNOWN = 0,
	THREAD_EVENT_BEGIN,
	THREAD_EVENT_END,
	THREAD_EVENT_LAST
};
class CThreadQueue;
typedef boost::signal<void(CThreadQueue *, THREAD_EVENT)> queue_signal_type;
typedef queue_signal_type::slot_type queue_slot_type;

class CLIB_API CThreadQueue : public boost::noncopyable, public boost::signals::trackable, CLockableUnknown
{
protected:
	LONG m_max_thread_count;
	volatile LONG m_thread_count;
	std::list<CThreadWorkerAdapt> m_jobs;
	queue_signal_type on_refresh;

public:
	IMPLEMENT_CLOCKABLEUNKNOWN
	CThreadQueue(const LONG max_thread_count);
	~CThreadQueue();
	void Push(const std::string & label, const boost::function0<void>& threadfunc);
	void Append(const std::string & label, const boost::function0<void>& threadfunc);
	size_t Size() const;
	void Clear();
	void Join();
	void SetMaxThreadCount(int maxThreadCount);
	void operator()();
	boost::signals::connection MonitorEvents(const queue_slot_type& s);

protected:
	void Start();
	static void Run(CThreadWorkerAdapt p);
};

}