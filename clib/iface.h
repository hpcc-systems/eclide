#pragma once

#include "clib.h"

//  scm  
#undef interface

#define implements   public
#define extends      public

#define interface	struct __declspec(novtable)

interface IInterface
{
	virtual void Link() = 0;
	virtual bool Release() = 0;
};

interface IInterfaceEx : extends IInterface
{
	virtual IInterfaceEx * queryInterface(const TCHAR * name) = 0;
};

typedef IInterface * IInterfacePtr;

inline void Link(IInterface * ptr)			{ if (ptr) ptr->Link(); }
inline void Release(IInterface * ptr)		{ if (ptr) ptr->Release(); }

template <class X> inline void Link(X * ptr)	{ if (ptr) ptr->Link(); }
template <class X> inline void Release(X * ptr) { if (ptr) ptr->Release(); }

#define QUERYINTERFACE(ptr, TYPE)	(dynamic_cast<TYPE *>(ptr))
#define NULLQUERYINTERFACE(ptr, TYPE) ((ptr) ? QUERYINTERFACE(ptr, TYPE) : (TYPE *)NULL)

template <class CLASS> class Owned
{
public:
	Owned()								{ ptr = NULL; }
	Owned(CLASS * _ptr)					{ ptr = _ptr; }
	Owned(const Owned<CLASS> & other)	{ ptr = other.getLink(); }

   ~Owned()								{ ::Release(ptr); }
	
private: 
	/* these overloaded operators are the devil of memory leak. Use set, setown instead. */
	CLASS * operator = (CLASS * _ptr)				 { set(_ptr); assertex(false); return get(); }
	Owned<CLASS> & operator = (const Owned<CLASS> & other) { set(other.get()); assertex(false); return *this; }

	/* this causes -ve memory leak */
	void setown(const Owned<CLASS> &other) { assertex(false); }

public:
	CLASS * operator -> () const		{ return ptr; } 
	operator CLASS *() const			{ return ptr; } 
	
	void clear()						{ ::Release(ptr); ptr = NULL; }
	CLASS * get() const					{ return ptr; }
	CLASS * getClear()					{ CLASS * temp = ptr; ptr = NULL; return temp; }
	CLASS * getLink() const				{ return PLINK(ptr); }
	void set(CLASS * _ptr)				{ ::Link(_ptr); ::Release(ptr); ptr = _ptr; }
	void setown(CLASS * _ptr)			{ ::Release(ptr); ptr = _ptr; }
	
	void set(const Owned<CLASS> &other) { set(other.get()); }
	
private:
	CLASS * ptr;
};

template <class CLASS> class Linked : public Owned<CLASS>
{
public:
	Linked()							{ }
	Linked(CLASS * _ptr) : Owned<CLASS>(PLINK(_ptr)) { }
	Linked(const Linked<CLASS> & other) : Owned<CLASS>(other) { }
};

template <class T> 
class StlLinked
{
public:
	StlLinked(T * c)
	{
		::Link(c);
		ptr = c;
	}
	StlLinked(const StlLinked<T>& c)
	{
		::Link(&*c);
		ptr = &*c;
	}
	~StlLinked()	
	{ 
		::Release(ptr);
	}
	StlLinked<T>& operator = (const StlLinked<T>& c)
	{
		::Link(*c);
		::Release(ptr);
		ptr = *c;
		return *this;
	}
	T& operator *() const
	{
		return *ptr;
	}
	T* operator ->() const
	{
		return ptr;
	}

protected:
	T * ptr;
};

template <class T> 
class StlOwned : public StlLinked<T>
{
private:
public:
	StlOwned(T * c) : StlLinked<T>(c)
	{
		::Release(ptr);
	}
	StlOwned(const StlOwned<T>& c) : StlLinked<T>(c)
	{
		::Release(ptr);
	}
};

interface IIterator : extends IInterface
{
	virtual bool first() = 0;
	virtual bool next() = 0;
	virtual bool isValid() = 0;
	virtual IInterface & query() = 0;
	virtual IInterface & get() = 0;
};

template <class C> 
interface IIteratorOf : public IInterface
{
public:
	virtual bool first() = 0;
	virtual bool next() = 0;
	virtual bool isValid() = 0;
	virtual C  & query() = 0;
			C  & get() { C &c = query(); c.Link(); return c; }
};

#define ForEach(i)              for((i).first();(i).isValid();(i).next())

template <class X> inline X * LINK(X * ptr)			{ ptr->Link(); return ptr; }
template <class X> inline X * PLINK(X * ptr)		{ if (ptr) ptr->Link(); return ptr; }
template <class X> inline X & OLINK(X & obj)		{ obj.Link(); return obj; }
template <class X> inline X * LINK(Owned<X> ptr)	{ return ptr.getLink(); }
template <class X> inline X * PLINK(Owned<X> ptr)	{ return ptr.getLink(); }

//  jiface  
typedef long atomic_t;
#define ATOMIC_INIT(i)  (i)
#define atomic_inc(x)   InterlockedIncrement(x)
#define atomic_dec(x)   InterlockedDecrement(x)
#define atomic_read(v)  (*v)
#define atomic_set(v,i) ((*v) = (i))

class CLIB_API CInterface
{
public:
			 CInterface()			{ atomic_set(&xxcount,0); }
	virtual ~CInterface();
	virtual void beforeDispose();
	bool	IsShared(void) const	{ return (atomic_read(&xxcount) != 0); }
	int		getCount(void) const	{ return atomic_read(&xxcount); }
//interface IInterface:
	void	Link()					{ atomic_inc(&xxcount); }
	bool	Release(void);
private:
	atomic_t	xxcount;
};

typedef class CInterface * CInterfacePtr;
typedef Linked<CInterface> LinkedCInterface;

inline void Link(CInterface * ptr)			{ if (ptr) ptr->Link(); }
inline void Release(CInterface * ptr)		{ if (ptr) ptr->Release(); }

#define IMPLEMENT_IINTERFACE													\
	virtual void Link(void)		{ CInterface::Link(); }                     \
	virtual bool Release(void)	{ return CInterface::Release(); }

#define BEGIN_IMPLEMENT_IINTERFACEEX											 \
	virtual void Link(void)		{ CInterface::Link(); }                  \
	virtual bool Release(void)	{ return CInterface::Release(); }        \
	virtual IInterfaceEx * queryInterface(const TCHAR *name)		{		 \

#define SUPPORTS_INTERFACE(_a) if (strcmp(name, #_a)==0) return static_cast <_a *> (this);

#define END_IMPLEMENT_IINTERFACEEX return NULL; }

