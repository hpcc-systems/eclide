#include "StdAfx.h"

#include "LimitSingleInstance.h"

CLimitSingleInstance::CLimitSingleInstance(std::_tstring strMutexName)
{
    //Make sure that you use a name that is unique for this application otherwise
    //two apps may think they are the same if they are using same name for
    //3rd parm to CreateMutex
    boost::algorithm::replace_all(strMutexName, _T("\\"), _T("/"));	//mutexes cant have "\"
    m_hMutex = CreateMutex(NULL, FALSE, strMutexName.c_str()); //do early
    m_dwLastError = GetLastError(); //save for use later...
}

CLimitSingleInstance::~CLimitSingleInstance() 
{
    if (m_hMutex)  //Do not forget to close handles.
    {
        CloseHandle(m_hMutex); //Do as late as possible.
        m_hMutex = NULL; //Good habit to be in.
    }
}

BOOL CLimitSingleInstance::IsAnotherInstanceRunning() 
{
    return (ERROR_ALREADY_EXISTS == m_dwLastError);
}

