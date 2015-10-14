#include "stdafx.h"
#include "clib.h"

#define DESTRUCTOR_REFCOUNT 1042 

CUnknown::CUnknown() : m_RefCount(0)
{
}

CUnknown::~CUnknown()
{
    ATLASSERT(m_RefCount == 0 || m_RefCount == DESTRUCTOR_REFCOUNT);
}

ULONG STDMETHODCALLTYPE CUnknown::AddRef() throw()
{
    InterlockedIncrement(reinterpret_cast<long*>(&m_RefCount));
    return m_RefCount;
}

ULONG STDMETHODCALLTYPE CUnknown::Release() throw()
{
    if (!InterlockedDecrement(reinterpret_cast<long*>(&m_RefCount))) 
    {
        m_RefCount = DESTRUCTOR_REFCOUNT;
        delete this;
        return 0;
    }
    ATLASSERT(m_RefCount > 0);
    return m_RefCount;
}

HRESULT STDMETHODCALLTYPE CUnknown::QueryInterface(/* [in] */ REFIID riid, /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppUnk)
{
    *ppUnk = NULL;
    if (::InlineIsEqualGUID(riid, IID_IUnknown) ||
        ::InlineIsEqualGUID(riid, __uuidof(CUnknown)))
    {
        *ppUnk = (void*)(IStream*)this;
        AddRef();
        return S_OK;
    }
    return E_NOINTERFACE;
}

