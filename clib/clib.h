#pragma once

#ifdef SEISINT_LIBEXPORTS
#  define CLIB_API
#else
#  ifdef CLIB_EXPORTS
#    define CLIB_API __declspec(dllexport)
#  else
#    define CLIB_API __declspec(dllimport)
#  endif
#endif // SEISINT_LIBEXPORTS

class CLIB_API __declspec(uuid("3fd114a5-5ace-4cfb-ab10-38e58f23f664")) CUnknown : public IUnknown
{
private:
    unsigned long m_RefCount;

public:
    CUnknown(); 
    virtual ~CUnknown();

    virtual ULONG STDMETHODCALLTYPE AddRef() throw();
    virtual ULONG STDMETHODCALLTYPE Release() throw();
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(/* [in] */ REFIID riid, /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
    template<class Q>
    HRESULT STDMETHODCALLTYPE QueryInterface(Q** pp)
    {
        return IUnknown::QueryInterface(pp);
    }
};

#define IMPLEMENT_CUNKNOWNNOQI \
    virtual ULONG STDMETHODCALLTYPE AddRef() throw() { return CUnknown::AddRef(); } \
    virtual ULONG STDMETHODCALLTYPE Release() throw() {	return CUnknown::Release();	} \

#define IMPLEMENT_CUNKNOWN \
    IMPLEMENT_CUNKNOWNNOQI \
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject) { return CUnknown::QueryInterface(riid, ppvObject); } \
    template<class Q> HRESULT STDMETHODCALLTYPE QueryInterface(Q** pp) { return CUnknown::QueryInterface(pp); }

#define IMPLEMENT_CUNKNOWNQI(T) \
    IMPLEMENT_CUNKNOWNNOQI \
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void __RPC_FAR *__RPC_FAR *ppUnk){\
        *ppUnk = NULL;\
        if (::InlineIsEqualGUID(riid, IID_IUnknown) || ::InlineIsEqualGUID(riid, __uuidof(T))){\
            *ppUnk = (void*)(T*)this;\
            AddRef();\
            return S_OK;\
        }\
        return E_NOINTERFACE;\
    }\
    template<class Q> HRESULT STDMETHODCALLTYPE QueryInterface(Q** pp) { return CUnknown::QueryInterface(pp); }

#define BEGIN_CUNKNOWN \
    IMPLEMENT_CUNKNOWNNOQI \
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void __RPC_FAR *__RPC_FAR *ppUnk) \
    { \
        (riid); \
        *ppUnk = NULL; \
        if (false) \
        { \
        }

#define IMPLEMENT_INTERFACE(INTERFACE) \
        else if (::InlineIsEqualGUID(riid, IID_IUnknown) || ::InlineIsEqualGUID(riid, __uuidof(INTERFACE))) \
        { \
            *ppUnk = (void*)(INTERFACE*)this; \
            AddRef(); \
            return S_OK; \
        }

#define END_CUNKNOWN(BASE) \
        return E_NOINTERFACE; \
    } \
    template<class Q> HRESULT STDMETHODCALLTYPE QueryInterface(Q** pp) { return BASE::QueryInterface(pp); } 

inline void Link(IUnknown * ptr)		{ if (ptr) ptr->AddRef(); }
inline void Release(IUnknown * ptr)		{ if (ptr) ptr->Release(); }

template <typename T> inline void Link(T * ptr)	{ if (ptr) ptr->AddRef(); }
template <typename T> inline void Release(T * ptr) { if (ptr) ptr->Release(); }

template <typename T> 
class StlLinked
{
public:
    StlLinked()
    {
        m_T = NULL;
    }
    StlLinked(T * c)
    {
        ::Link(c);
        m_T = c;
    }
    StlLinked(const StlLinked<T>& c)
    {
        ::Link(&*c);
        m_T = &*c;
    }
    ~StlLinked()	
    { 
        ::Release(m_T);
    }
    StlLinked<T>& operator = (const StlLinked<T>& c)
    {
        ::Link(&*c);
        ::Release(m_T);
        m_T = &*c;
        return *this;
    }
    T *Detach()
    {
        T * temp = m_T;
        m_T = 0;
        return temp;
        
    }
    void Attach( const StlLinked<T>& c)
    {
        if ( m_T != &*c )
        {
            ::Release(m_T);
            m_T = &*c;
        }
    }
    bool operator ==(const StlLinked<T>& c) const
    {
        return m_T == c.m_T;
    }
    T& operator *() const
    {
        return *m_T;
    }
    inline operator T*() const
    {
        return m_T;
    }
    inline T* operator ->() const
    {
        return m_T;
    }
    inline T* get() const
    {
        return m_T;
    }

    inline bool isLinked() const
    {
        return m_T != 0;
    }

protected:
    T * m_T;
};

template<typename T>
class CUnknownT : public CUnknown, public T
{
public:
    IMPLEMENT_CUNKNOWN;
};

#ifndef _WIN32_WCE
class CComInitialize
{
private:
    HRESULT m_hr;

public:
    CComInitialize() : m_hr(E_FAIL)
    {
        ATLASSERT(FAILED(m_hr));
        m_hr = CoInitialize(NULL);
    }
    ~CComInitialize()
    {
        if (SUCCEEDED(m_hr))
            CoUninitialize();
    }
};
#endif
