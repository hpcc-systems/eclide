// Hourglass.h : Declaration of the CHourglass

#pragma once
#include "resource.h"       // main symbols

#include "GraphViewCtl.h"


// CHourglass

class ATL_NO_VTABLE CHourglass : 
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CHourglass, &CLSID_Hourglass>,
    public IObjectWithSiteImpl<CHourglass>,
    public IDispatchImpl<IHourglass, &IID_IHourglass, &LIBID_GraphViewControlLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
    CHourglass() : m_hWnd(NULL), m_pWaitCursor(NULL)
    {
    }

DECLARE_REGISTRY_RESOURCEID(IDR_HOURGLASS)


BEGIN_COM_MAP(CHourglass)
    COM_INTERFACE_ENTRY(IHourglass)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(IObjectWithSite)
END_COM_MAP()


    DECLARE_PROTECT_FINAL_CONSTRUCT()

    HRESULT FinalConstruct()
    {
        m_pWaitCursor = new CWaitCursor();
        return S_OK;
    }
    
    void FinalRelease() 
    {
        if (!m_pWaitCursor) delete m_pWaitCursor;
    }

protected:
    CWaitCursor* m_pWaitCursor;
    HWND m_hWnd;
public:
    void SetHWnd(HWND hWnd) { m_hWnd = hWnd; }
};

OBJECT_ENTRY_AUTO(__uuidof(Hourglass), CHourglass)
