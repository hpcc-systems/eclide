#pragma once

#include "clib.h"

class CLIB_API CLimitSingleInstance : public CUnknown
{
protected:
    DWORD  m_dwLastError;
    HANDLE m_hMutex;

public:

    BEGIN_CUNKNOWN
    END_CUNKNOWN(CUnknown)
    CLimitSingleInstance(std::_tstring strMutexName);
    ~CLimitSingleInstance();

    BOOL IsAnotherInstanceRunning();
};
