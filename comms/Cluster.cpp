#include "StdAfx.h"
#include "cluster.h"
#include "dali.h"
#include "SoapUtil.h"
#include "clib.h"
#include "cache.h"
#include "logger.h"

#if _COMMS_VER < 68200
using namespace WsTopology;
#elif _COMMS_VER < 700000
#else
using namespace WsEnvironment;
#endif

namespace Topology
{
class CCluster : public ICluster, public CUnknown
{
protected:
    std::_tstring m_Name;
    std::_tstring m_Queue;

    std::_tstring m_Url;
    std::_tstring m_Type;
    std::_tstring m_Directory;
    std::_tstring m_Desc;
    std::_tstring m_Prefix;
    std::_tstring m_Path;
    std::_tstring m_DataModel;

public:
    IMPLEMENT_CUNKNOWN;

    CCluster(const std::_tstring & url, const std::_tstring & name, const std::_tstring & queue = _T("")) : m_Url(url), m_Name(name), m_Queue(queue)
    {
    }

    virtual ~CCluster()
    {
    }

    const TCHAR *GetID() const
    {
        return m_Name.c_str();
    }
    const TCHAR * GetCacheID() const
    {
        return m_Name.c_str();
    }
    const TCHAR *GetName() const
    {
        return m_Name.c_str();
    }
    const TCHAR *GetQueue() const
    {
        return m_Queue.c_str();
    }
    const TCHAR *GetType() const
    {
        return m_Type.c_str();
    }
    const TCHAR *GetDirectory() const
    {
        return m_Directory.c_str();
    }
    const TCHAR *GetDesc() const
    {
        return m_Desc.c_str();
    }
    const TCHAR *GetPrefix() const
    {
        return m_Prefix.c_str();
    }
    const TCHAR *GetPath() const
    {
        return m_Path.c_str();
    }
    const TCHAR *GetDataModel() const
    {
        return m_DataModel.c_str();
    }

#if _COMMS_VER < 68200
    void Update(const TpLogicalCluster * c)
    {
        m_Name = CW2T(c->Name, CP_UTF8);
#if _COMMS_VER < 55100
#else
        if(c->Queue)
        {
            m_Queue = CW2T(c->Queue, CP_UTF8);
        }
#endif
        Refresh();
    }
    void Update(const TpCluster * c)
    {
        m_Name = CW2T(c->Name, CP_UTF8);
        m_Type = CW2T(c->Type, CP_UTF8);
        m_Directory = CW2T(c->Directory, CP_UTF8);
        m_Desc = CW2T(c->Desc, CP_UTF8);
        m_Prefix = CW2T(c->Prefix, CP_UTF8);
        m_Path = CW2T(c->Path, CP_UTF8);
        m_DataModel = CW2T(c->DataModel, CP_UTF8);
        Refresh();
    }
#elif _COMMS_VER < 700000
    void Update(const ns5__TpLogicalCluster * c)
    {
        SAFE_ASSIGN(m_Name, c->Name);
        SAFE_ASSIGN(m_Queue, c->Queue);
        Refresh();
    }
#else
    void Update(const LogicalCluster * c)
    {
        m_Name = CW2T(c->Name, CP_UTF8);
        if(c->Queue)
        {
            m_Queue = CW2T(c->Queue, CP_UTF8);
        }
        Refresh();
    }
#endif

    void Refresh()
    {
    }
};

CacheT<std::_tstring, CCluster> ClusterCache;

void ClearClusterCache()
{
    ClusterCache.Clear();
}

CCluster * CreateClusterRaw(const CString & url, const CString & wuid, const std::_tstring & queue = _T(""))
{
    return ClusterCache.Get(new CCluster(static_cast<const TCHAR *>(url), static_cast<const TCHAR *>(wuid), queue));
}

ICluster * CreateCluster(const CString & url, const CString & wuid, const std::_tstring & queue)
{
    return CreateClusterRaw(url, wuid, queue);
}

#if _COMMS_VER < 68200
ICluster * CreateCluster(const CString & url, const TpLogicalCluster * data)
{
    CCluster * attr = CreateClusterRaw(url, data->Name);
    ATLASSERT(attr);
    attr->Update(data);
    return attr;
}
ICluster * CreateCluster(const CString & url, const TpCluster * data)
{
    ATLASSERT(!"This is not used any more - please tell Gordon");
    //CCluster * attr = CreateClusterRaw(url, data->Name);
    //ATLASSERT(attr);
    //attr->Update(data);
    return NULL;//attr;
}
#elif _COMMS_VER < 700000
ICluster * CreateCluster(const CString & url, const ns5__TpLogicalCluster * data)
{
    CCluster * attr = CreateClusterRaw(url, data->Name->c_str());
    ATLASSERT(attr);
    attr->Update(data);
    return attr;
}
ICluster * CreateCluster(const CString & url, const ns5__TpCluster * data)
{
    ATLASSERT(!"This is not used any more - please tell Gordon");
    //CCluster * attr = CreateClusterRaw(url, data->Name);
    //ATLASSERT(attr);
    //attr->Update(data);
    return NULL;//attr;
}
#else
ICluster * CreateCluster(const CString & url, const LogicalCluster * data)
{
    CCluster * attr = CreateClusterRaw(url, data->Name);
    ATLASSERT(attr);
    attr->Update(data);
    return attr;
}
ICluster * CreateCluster(const CString & url, const Cluster * data)
{
    ATLASSERT(!"This is not used any more - please tell Gordon");
    //CCluster * attr = CreateClusterRaw(url, data->Name);
    //ATLASSERT(attr);
    //attr->Update(data);
    return NULL;//attr;
}
#endif
void ClearClusterSingletons()
{
    ClusterCache.clear();
}
}
