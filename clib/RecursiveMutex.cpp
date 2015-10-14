#include "StdAfx.h"

#include ".\recursivemutex.h"
#include "atlsync.h"

namespace clib
{
recursive_mutex::recursive_mutex() : m_lockCount(0)
{
}

recursive_mutex::~recursive_mutex()
{
}

LONG recursive_mutex::GetLockCount()
{
    return m_lockCount;
}

void recursive_mutex::do_lock()
{
    ::InterlockedIncrement(&m_lockCount);
    m_critSec.Lock();
}

void recursive_mutex::do_unlock()
{
    m_critSec.Unlock();
    ::InterlockedDecrement(&m_lockCount);
}

rwrecursive_mutex::rwrecursive_mutex()
{
    m_nReaders = 0;
    m_nWriters = 0;
}

rwrecursive_mutex::~rwrecursive_mutex()
{
}

void rwrecursive_mutex::do_lockr()
{
    for(;;)
    {
        ::InterlockedIncrement(&m_nReaders);
        if(m_nWriters == 0) 
            break;
        ::InterlockedDecrement(&m_nReaders);
        ::Sleep(0);
    }
}

void rwrecursive_mutex::do_lockw()
{
    for(;;) 
    {
        if( ::InterlockedExchange( &m_nWriters, 1 ) == 1 ) 
            ::Sleep(0);
        else 
        {
            while(m_nReaders != 0) 
                ::Sleep(0);
            break;
        }
    }
}

void rwrecursive_mutex::do_unlockr()
{
    //DWORD ThreadId = GetCurrentThreadId();
    ::InterlockedDecrement(&m_nReaders);
}

void rwrecursive_mutex::do_unlockw()
{
    //DWORD ThreadId = GetCurrentThreadId();
    ::InterlockedDecrement(&m_nWriters);
}

scoped_lock_ref_counted::scoped_lock_ref_counted(recursive_mutex & m)
{
    m_lock = new recursive_mutex::scoped_lock(m);
}

scoped_lock_ref_counted::~scoped_lock_ref_counted()
{
    delete(m_lock);
}

rwscoped_lock_ref_counted::rwscoped_lock_ref_counted(CRWMutex &m) 
{
    m_lock = new rwscoped_mutex_lock(m);
}

rwscoped_lock_ref_counted::~rwscoped_lock_ref_counted() 
{
    delete(m_lock);
}

void CLockableUnknown::Lock(CComPtr<clib::scoped_lock_ref_counted> & lock)
{
    lock = new clib::scoped_lock_ref_counted(m_mutex);
}


}

namespace boost
{
scoped_lock_ref_counted::scoped_lock_ref_counted(recursive_mutex & m)
{
    m_lock = new recursive_mutex::scoped_lock(m);
}

scoped_lock_ref_counted::~scoped_lock_ref_counted()
{
    delete(m_lock);
}

}