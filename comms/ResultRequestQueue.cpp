#include "StdAfx.h"
#include <LockableEventQueue.h> //clib
#include "RequestQueue.h"
#include "ResultRequestQueue.h"

class CResultRequestQueue : public IResultRequestQueue
{
protected:
    typedef CLockableEventQueue<Dali::CResultDataRequestAdapt> QueueType;
    typedef StlLinked<QueueType>  QueueAdapt;

    typedef Dali::CRequestQueue<Dali::CResultDataRequestAdapt> Queue;
    StlLinked<Queue> m_queue;

public:
    CResultRequestQueue()
    {
        m_queue = new Queue(&QueueHandler);
    }

    ~CResultRequestQueue(void)
    {
    }

    void SetServerConfig(const Dali::CServerConfig &config)
    {
        m_queue->SetServerConfig(config);
    }

protected:
    static void QueueHandler(Dali::CResultDataRequestAdapt req, Dali::CServerConfig& config)
    {
        Dali::GetResultData(config, req);
    }

public: //IResultRequestQueue
    void Clear()
    {
        m_queue->clear();
    }

    void GetResultData(Dali::CResultDataRequest* request, bool bHighPriority)
    {
        Dali::CResultDataRequestAdapt r = request;
        if ( bHighPriority )
            m_queue->addToFront(r);
        else
            m_queue->addToBack(r);
    }

};
typedef CUnknownT<CResultRequestQueue> CResultRequestQueuePtr;

IResultRequestQueue* CreateIResultRequestQueue(const TCHAR* url, const TCHAR* user, const TCHAR* password)
{
    Dali::CServerConfig config(url,_T("Dali"));
    config.m_userId = user;
    config.m_password = password;
    CResultRequestQueue* q = new CResultRequestQueuePtr();
    q->SetServerConfig(config);
    return q;
}
