#ifndef _SINGLETON_H_
#define _SINGLETON_H_

template<class T>
class CSingleton
{
private:
    class InstPtr
    {
    public:
        InstPtr() : m_ptr(0) {}
        ~InstPtr() { delete m_ptr; }
        T* Get() { return m_ptr; }
        void Set(T* p)
        {
            if(p!= 0)
            {
                delete m_ptr;
                m_ptr = p;
            }
        }
    private:
        T* m_ptr;
    };

    static InstPtr sm_ptr;
    CSingleton();
    CSingleton(const CSingleton&);
    CSingleton& operator=(const CSingleton&);

public:
    static T* Instance()
    {
        if(sm_ptr.Get() == 0)
        {
            sm_ptr.Set(new T());
        }
        return sm_ptr.Get();
    }
};

#endif // #ifndef _SINGLETON_H_
