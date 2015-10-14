#pragma once

#include "clib.h"

namespace clib
{
class lock_error : public std::logic_error
{
public:
    lock_error() : std::logic_error("Error Locking clib::recursive_mutex")
    {
    }
};

//This class is better for debugging as it allows a timeout and detects abandonment
//it will be slightly slower at runtime than a critical section
class CMutexLock
{
public:
    typedef unsigned cv_state;

    CMutexLock()
    {
        m_mutex = ::CreateMutex(0,FALSE,NULL);
#if _DEBUG
        m_maxWait = 25 * 1000;
#else
        m_maxWait = (DWORD)-1;
#endif
    }
    ~CMutexLock()
    {
        ::CloseHandle(m_mutex);
    }

    bool do_trylock()
    {
        return (WAIT_OBJECT_0 == ::WaitForSingleObject(m_mutex, 0));
    }
    void do_lock()
    {
        //ATLTRACE(_T("CMutex:do_lock(%0x)\n"), ::GetCurrentThreadId());
        if ( WAIT_TIMEOUT == ::WaitForSingleObject(m_mutex, m_maxWait) )
        {
            ATLASSERT(!_T("Wait Timed Out"));
        }
    }
    void do_unlock()
    {
        //ATLTRACE(_T("CMutex:do_unlock(%0x)\n"), ::GetCurrentThreadId());
        ATLASSERT(::ReleaseMutex(m_mutex));
    }
    void do_lock(unsigned /*state*/)
    {
        ATLASSERT(!_T("NOT IMPLEMENTED"));
    }
    void do_unlock(unsigned /*state*/)
    {
        ATLASSERT(!_T("NOT IMPLEMENTED"));
    }
protected:
    HANDLE m_mutex;
    DWORD  m_maxWait;
};

template <typename Mutex>
class lock_ops : private boost::noncopyable
{
private:
    lock_ops() { }

public:
    //typedef typename Mutex::cv_state lock_state;

    static void lock(Mutex& m)
    {
        m.do_lock();
    }
    static void lockr(Mutex& m)
    {
        m.do_lockr();
    }
    static void lockw(Mutex& m)
    {
        m.do_lockw();
    }
    static void unlock(Mutex& m)
    {
        m.do_unlock();
    }
    static void unlockr(Mutex& m)
    {
        m.do_unlockr();
    }
    static void unlockw(Mutex& m)
    {
        m.do_unlockw();
    }
};

template <typename Mutex>
class scoped_lock : private boost::noncopyable
{
public:
    typedef Mutex mutex_type;

    explicit scoped_lock(Mutex& mx, bool initially_locked=true) : m_mutex(mx), m_locked(false)
    {
        if (initially_locked) 
            lock();
    }
    ~scoped_lock()
    {
        if (m_locked) 
            unlock();
    }

    void lock()
    {
        if (m_locked) 
            throw lock_error();
        lock_ops<Mutex>::lock(m_mutex);
        m_locked = true;
    }

    void unlock()
    {
        if (!m_locked) 
            throw lock_error();
        lock_ops<Mutex>::unlock(m_mutex);
        m_locked = false;
    }

    bool locked() const 
    { 
        return m_locked; 
    }

    operator const void*() const 
    { 
        return m_locked ? this : 0; 
    }

    operator mutex_type&()
    {
        return m_mutex;
    }

private:
    //friend class boost::condition;

    Mutex& m_mutex;
    bool m_locked;
};

template <typename Mutex>
class scoped_lockw : private boost::noncopyable
{
public:
    typedef Mutex mutex_type;

    explicit scoped_lockw(Mutex& mx, bool initially_locked=true) : m_mutex(mx), m_locked(false)
    {
        if (initially_locked) 
            lockw();
    }
    ~scoped_lockw()
    {
        if (m_locked) 
            unlockw();
    }

    void lockw()
    {
        if (m_locked) 
            throw lock_error();
        lock_ops<Mutex>::lockw(m_mutex);
        m_locked = true;
    }

    void unlockw()
    {
        if (!m_locked) 
            throw lock_error();
        lock_ops<Mutex>::unlockw(m_mutex);
        m_locked = false;
    }

    bool locked() const 
    { 
        return m_locked; 
    }

    operator const void*() const 
    { 
        return m_locked ? this : 0; 
    }

    operator mutex_type&()
    {
        return m_mutex;
    }

private:
    //friend class boost::condition;

    Mutex& m_mutex;
    bool m_locked;
};

template <typename Mutex>
class scoped_lockr : private boost::noncopyable
{
public:
    typedef Mutex mutex_type;

    explicit scoped_lockr(Mutex& mx, bool initially_locked=true) : m_mutex(mx), m_locked(false)
    {
        if (initially_locked) 
            lockr();
    }
    ~scoped_lockr()
    {
        if (m_locked) 
            unlockr();
    }

    void lockr()
    {
        if (m_locked) 
            throw lock_error();
        lock_ops<Mutex>::lockr(m_mutex);
        m_locked = true;
    }

    void unlockr()
    {
        if (!m_locked) 
            throw lock_error();
        lock_ops<Mutex>::unlockr(m_mutex);
        m_locked = false;
    }

    bool locked() const 
    { 
        return m_locked; 
    }

    operator const void*() const 
    { 
        return m_locked ? this : 0; 
    }

    operator mutex_type&()
    {
        return m_mutex;
    }

private:
    //friend class boost::condition;
    Mutex& m_mutex;
    bool m_locked;
};

class CLIB_API recursive_mutex : private boost::noncopyable
{
public:
    friend class clib::lock_ops<recursive_mutex>;
    typedef clib::scoped_lock<recursive_mutex> scoped_lock;
    recursive_mutex();
    ~recursive_mutex();

    LONG GetLockCount();

private:
    CComAutoCriticalSection m_critSec;
    typedef std::size_t cv_state;
    LONG m_lockCount;

    void do_lock();
    void do_unlock();
    //void do_lock(cv_state& state);
    //void do_unlock(cv_state& state);
};

typedef scoped_lock<recursive_mutex> scoped_mutex_lock;

class CLIB_API rwrecursive_mutex : private boost::noncopyable
{
protected:
    LONG m_nReaders;
    LONG m_nWriters;

public:
    friend class clib::lock_ops<rwrecursive_mutex>;
    typedef clib::scoped_lockw<rwrecursive_mutex> scoped_lockw;
    typedef clib::scoped_lockr<rwrecursive_mutex> scoped_lockr;
    rwrecursive_mutex();
    ~rwrecursive_mutex();

private:
    typedef std::size_t cv_state;

    void do_lockr();
    void do_lockw();
    void do_unlockr();
    void do_unlockw();
};

typedef scoped_lockr<rwrecursive_mutex> rwscoped_mutex_lock;
typedef rwrecursive_mutex CRWMutex; 

class rwscoped_lock_ref_counted : public CUnknown
{
protected:
    rwscoped_mutex_lock *m_lock;

public:
    IMPLEMENT_CUNKNOWN;

    rwscoped_lock_ref_counted(CRWMutex &m);
    virtual ~rwscoped_lock_ref_counted(); 
};

typedef CComPtr<rwscoped_lock_ref_counted> rwscoped_lock_ref_counted_adapt;
}

namespace clib
{
class CLIB_API scoped_lock_ref_counted : public CUnknown, public boost::noncopyable
{
protected:
    recursive_mutex::scoped_lock *m_lock;

public:
    IMPLEMENT_CUNKNOWN;
    scoped_lock_ref_counted(recursive_mutex & m) ;
    virtual ~scoped_lock_ref_counted(); 
};
typedef CComPtr<scoped_lock_ref_counted> scoped_lock_ref_counted_adapt;

__interface ILockableUnknown : public IUnknown
{
    void Lock(CComPtr<scoped_lock_ref_counted> & lock);
};

class CLIB_API CLockableUnknown : public CUnknown
{
protected:
    mutable clib::recursive_mutex m_mutex;

public:
    IMPLEMENT_CUNKNOWN;
    void Lock(CComPtr<scoped_lock_ref_counted> & lock);
};
}

namespace boost
{
class CLIB_API scoped_lock_ref_counted : public CUnknown, public boost::noncopyable
{
protected:
    recursive_mutex::scoped_lock *m_lock;

public:
    IMPLEMENT_CUNKNOWN;
    scoped_lock_ref_counted(recursive_mutex & m) ;
    virtual ~scoped_lock_ref_counted(); 
};
typedef CComPtr<scoped_lock_ref_counted> scoped_lock_ref_counted_adapt;

__interface ILockableUnknown : public IUnknown
{
    void Lock(CComPtr<scoped_lock_ref_counted> & lock);
};

}

#define IMPLEMENT_CLOCKABLEUNKNOWN \
    IMPLEMENT_CUNKNOWN; \
    void Lock(CComPtr<clib::scoped_lock_ref_counted> & lock) { return CLockableUnknown::Lock(lock); }

#define BEGIN_CLOCKABLEUNKNOWN \
    void Lock(CComPtr<clib::scoped_lock_ref_counted> & lock) { return CLockableUnknown::Lock(lock); } \
    BEGIN_CUNKNOWN

//  ===========================================================================
