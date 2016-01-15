#pragma once

#include <thread.h>  //CLIB

class CCustomActionSecureSoapSocketClient :	public CSecureSoapSocketClient
{
    typedef CSecureSoapSocketClient baseClass;

protected:
    CString m_strAction;

public:
    CCustomActionSecureSoapSocketClient(LPCTSTR szUrl) : 
      baseClass(szUrl, CW2T(_bstr_t(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_USER)), CP_UTF8), CW2T(_bstr_t(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_PASSWORD)), CP_UTF8))
    {
    }

    HRESULT SetAction(LPCTSTR szAction)
    {
        _ATLTRY
        {
            m_strAction.Format(_T("SOAPAction: \"%s\"\r\n"), szAction);
        }
        _ATLCATCHALL()
        {
            return E_OUTOFMEMORY;
        }
        return S_OK;
    }

    HRESULT SendRequest(LPCTSTR szAction)
    {
        return baseClass::SendRequest(m_strAction.IsEmpty() ? szAction : m_strAction);
    }
};
