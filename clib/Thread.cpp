#include "StdAfx.h"

#include "thread.h"
#include "logger.h"
//
// Usage: SetThreadName (-1, "MainThread");
//
typedef struct tagTHREADNAME_INFO
{
	DWORD dwType; // must be 0x1000
	LPCSTR szName; // pointer to name (in user addr space)
	DWORD dwThreadID; // thread ID (-1=caller thread)
	DWORD dwFlags; // reserved for future use, must be zero
} THREADNAME_INFO;

void SetThreadName(DWORD dwThreadID, LPCSTR szThreadName) 
{
	THREADNAME_INFO info;
	{
		info.dwType = 0x1000;
		info.szName = szThreadName;
		info.dwThreadID = dwThreadID;
		info.dwFlags = 0;
	}
	__try
	{
		RaiseException(0x406D1388, 0, sizeof(info)/sizeof(DWORD), (ULONG_PTR*)&info);
	}
	__except (EXCEPTION_CONTINUE_EXECUTION)
	{
	}
}

namespace {

class thread_param
{
public:
	std::string m_name;
	thread_param(const boost::function0<void>& threadfunc, const std::string & name) : m_threadfunc(threadfunc), m_started(false)
    {
		m_name = name;
    }
    void wait()
    {
        //clib::recursive_mutex::scoped_lock scoped_lock(m_mutex);
        while (!m_started)
			Sleep(50);
        //m_condition.wait(scoped_lock);
    }
    void started()
    {
        //clib::recursive_mutex::scoped_lock scoped_lock(m_mutex);
        m_started = true;
        //m_condition.notify_one();
    }

    clib::recursive_mutex m_mutex;
    //boost::condition m_condition;
    const boost::function0<void>& m_threadfunc;
    bool m_started;
};

} // unnamed namespace

extern "C" {
    unsigned __stdcall thread_proxy(void* param)
    {
		thread_param* p = static_cast<thread_param*>(param);
#ifndef _DEBUG	//Let debugger react to Exception
		try
        {
#endif
            boost::function0<void> threadfunc = p->m_threadfunc;
            p->started();
            threadfunc();
#ifndef _DEBUG
        }
        catch(...)
        {
			_DBGLOG(LEVEL_SEVERE, (boost::_tformat(_T("GPF - %1%")) % p->m_name.c_str()).str().c_str());
			ATLASSERT(false);
        }
#endif
        return 0;
    }
}

namespace clib
{
class thread_resource_error : public std::logic_error
{
public:
	thread_resource_error() : std::logic_error("Thread Resource Error")
	{
	}
};

thread::thread() : m_joinable(false)
{
    m_thread = reinterpret_cast<void*>(GetCurrentThread());
    m_id = GetCurrentThreadId();
}

thread::thread(const std::string & label, const boost::function0<void>& threadfunc, int nPriorityOfThread)
{
    thread_param param(threadfunc, label);
	m_thread = reinterpret_cast<void*>(_beginthreadex(0, 0, &thread_proxy, &param, CREATE_SUSPENDED, &m_id));
    if (!m_thread)
        throw thread_resource_error();
	// set lower thread priority
	::SetThreadPriority(reinterpret_cast<HANDLE>(m_thread), nPriorityOfThread);
	// KPD: now creating suspended, let's see what havoc this has :-)
	::ResumeThread(reinterpret_cast<HANDLE>(m_thread));
	// wait?
    param.wait();
	SetThreadName(m_id, label.c_str());
}

thread::~thread()
{
    if (m_joinable)
    {
        int res = 0;
        res = CloseHandle(reinterpret_cast<HANDLE>(m_thread));
        assert(res);
    }
}

bool thread::operator==(const thread& other) const
{
    return other.m_id == m_id;
}

bool thread::operator!=(const thread& other) const
{
    return !operator==(other);
}

void thread::join()
{
    int res = 0;
    res = WaitForSingleObject(reinterpret_cast<HANDLE>(m_thread), INFINITE);
    assert(res == WAIT_OBJECT_0);
	res = CloseHandle(reinterpret_cast<HANDLE>(m_thread));
    assert(res);
    // This isn't a race condition since any race that could occur would
    // have us in undefined behavior territory any way.
    m_joinable = false;
}

void thread::sleep(int milliseconds)
{
	Sleep(milliseconds);
}

void thread::yield()
{
    Sleep(0);
}
//  ===========================================================================
CThreadWorker::CThreadWorker()
{
	ATLASSERT(false);
}
CThreadWorker::CThreadWorker(const std::string & label, const boost::function0<void>& threadfunc, const worker_slot_type& s) : m_label(label), m_threadfunc(threadfunc)
{
	on_refresh.connect(s);
}
const char * CThreadWorker::GetLabel()
{
	return m_label.c_str();
}
void CThreadWorker::Call()
{
	m_threadfunc();
	on_refresh();
}
//  ===========================================================================
CThreadQueue::CThreadQueue(const LONG max_thread_count) : m_max_thread_count(max_thread_count)
{
	m_thread_count = 0;
}

CThreadQueue::~CThreadQueue()
{
	ATLTRACE("CThreadQueue::~CThreadQueue() called with %d running jobs and %d outstanding jobs\n", m_thread_count, m_jobs.size());
}

void CThreadQueue::Push(const std::string & label, const boost::function0<void>& threadfunc)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	CThreadWorkerAdapt pw;
	pw.m_T = new CThreadWorker(label, threadfunc, boost::ref(*this));
	m_jobs.push_front(pw);
	Start();
}

void CThreadQueue::Append(const std::string & label, const boost::function0<void>& threadfunc)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	CThreadWorkerAdapt pw;
	pw.m_T = new CThreadWorker(label, threadfunc, boost::ref(*this));
	m_jobs.push_back(pw);
	Start();
}

size_t CThreadQueue::Size() const
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	return m_jobs.size() + m_thread_count;
}

void CThreadQueue::Clear()
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	m_jobs.clear();
}

void CThreadQueue::Join()
{
	//  No lock here as it is in Size already....
	while(Size())
	{
		Sleep(20);
	}
}

void CThreadQueue::SetMaxThreadCount(int maxThreadCount)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	m_max_thread_count = maxThreadCount;
	Start();
}

void CThreadQueue::operator()()
{
	clib::recursive_mutex::scoped_lock proc(m_mutex);
	m_thread_count--;
	on_refresh(this, THREAD_EVENT_END);
	Start();
}

boost::signals::connection CThreadQueue::MonitorEvents(const queue_slot_type& s)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	return on_refresh.connect(s); 
}

void CThreadQueue::Start()
{
	clib::recursive_mutex::scoped_lock proc(m_mutex);
	while (m_jobs.size() && m_thread_count < m_max_thread_count)
	{
		CThreadWorkerAdapt top = m_jobs.front();
		std::string method = top.m_T->GetLabel();
		method += "(";
		method += __FUNCTION__;
		method += ")";
		ATLTRACE("%s %d, %d\n", method.c_str(), m_thread_count, m_max_thread_count);
		m_jobs.pop_front();
		m_thread_count++;
		thread run(top.m_T->GetLabel(), boost::bind(&Run, top));
		on_refresh(this, THREAD_EVENT_BEGIN);
	}
}
void CThreadQueue::Run(CThreadWorkerAdapt p)
{
	p.m_T->Call();
}
}
