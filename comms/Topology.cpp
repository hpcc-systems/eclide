#include "StdAfx.h"
#include "topology.h"

#include "gSoapUtil.h"
#include "clib.h"
#include "cache.h"
#include "logger.h"
#include "dfuServer.h"
#include "EclServer.h"
#include "global.h"
#include "EclCC.h"

static const TCHAR GLOBAL_SERVER_TOPOLOGY_DEFAULT[] = _T("");
const SectionLabelDefault GLOBAL_SERVER_TOPOLOGY(SectionLabel(_T("Server"), _T("Topology")), GLOBAL_SERVER_TOPOLOGY_DEFAULT);

#if _COMMS_VER < 68200
using namespace WsTopology;
typedef CWsTopologyT<CCustomActionSecureSoapSocketClient> ServerT;
#elif _COMMS_VER < 700000
#else
typedef WsTopology::CWsTopologyT<CCustomActionSecureSoapSocketClient> ServerOldT;
using namespace WsEnvironment;
typedef CWsEnvironmentT<CCustomActionSecureSoapSocketClient> ServerT;
#endif

namespace Topology
{
ICluster * CreateCluster(const CString & url, const CString & wuid, const std::_tstring & queue = _T(""));
IGroup * CreateGroup(const CString & url, const std::_tstring & data);
IEclServer * CreateEclServer(const CString & url, const CString & wuid);
#if _COMMS_VER < 68200
ICluster * CreateCluster(const CString & url, const TpLogicalCluster * data);
ICluster * CreateCluster(const CString & url, const TpCluster * data);
IDropZone * CreateDropZone(const CString & url, const TpDropZone * data);
IDfuServer * CreateDfuServer(const CString & url, const TpDfuServer * data);
IEclServer * CreateEclServer(const CString & url, const TpEclServer * data);
#elif _COMMS_VER < 700000
ICluster * CreateCluster(const CString & url, const ns5__TpLogicalCluster * data);
ICluster * CreateCluster(const CString & url, const ns5__TpCluster * data);
IDropZone * CreateDropZone(const CString & url, const ns5__TpDropZone * data);
IDfuServer * CreateDfuServer(const CString & url, const ns5__TpDfuServer * data);
IEclServer * CreateEclServer(const CString & url, const ns5__TpEclServer * data);
#else
ICluster * CreateCluster(const CString & url, const LogicalCluster * data);
ICluster * CreateCluster(const CString & url, const Cluster * data);
IEclServer * CreateEclServer(const CString & url, const	ServiceDetail * data);
IDropZone * CreateDropZone(const CString & url, const ServiceDetail * data);
IDfuServer * CreateDfuServer(const CString & url, const ServiceDetail * data);
#endif
void ClearClusterSingletons();
void ClearDfuServerSingletons();
void ClearDropZoneSingletons();
void ClearEclServerSingletons();
void ClearGroupSingletons();
static CachePoolMap<IClusterVector> g_knownClusters;

#if _COMMS_VER >= 68200
#define ESP_EXCEPTION_LOG3(T) CReportingGSoapEspException<ns5__ArrayOfEspException> exceptions(T, __FILE__, __LINE__)
unsigned GetClusters(const CString & url, const std::_tstring & queueFilter, IClusterVector * clusters) 
{
    CachePoolAccessor<IClusterVector> knownClusters(*g_knownClusters.get(queueFilter), (const TCHAR *)url, queueFilter);
    if (knownClusters.needs_update())
    {
        if (queueFilter.empty() || boost::algorithm::iequals(queueFilter, _T("Local")))
        {
            if (CComPtr<IEclCC> eclcc = CreateIEclCC())
            {
                StlLinked<ICluster> cluster = CreateCluster(url, _T("Local"), _T("Local"));
                clusters->push_back(cluster);
            }
            //if (!queueFilter.empty())	//  Hack
            //	return clusters->size();
        }

        CSoapInitialize<WsTopologyServiceSoapProxy> server(url);

        _ns5__TpLogicalClusterQueryRequest request;
        CStringAssign EclServerQueue(request.EclServerQueue, queueFilter);

        _ns5__TpLogicalClusterQueryResponse response;
        ESP_EXCEPTION_LOG3(response.Exceptions);
        if (server.TpLogicalClusterQuery(&request, &response) == SOAP_OK)
        {
            if (response.TpLogicalClusters)
            {
                std::map<std::_tstring, bool> dedupMap;
                for(std::size_t i = 0; i < response.TpLogicalClusters->TpLogicalCluster.size(); ++i)
                {
                    StlLinked<ICluster> cluster = CreateCluster(url, response.TpLogicalClusters->TpLogicalCluster[i]);
                    CString label = cluster->GetName();
                    CString labelLower = label.MakeLower();
                    std::_tstring queue = cluster->GetQueue();
                    if (dedupMap[static_cast<const TCHAR *>(labelLower)] == false)
                    {
                        clusters->push_back(cluster);
                        dedupMap[static_cast<const TCHAR *>(labelLower)] = true;
                    }
                }
            }
        }
        else
            _DBGLOG(url, LEVEL_WARNING, server.GetClientErrorMsg());
        knownClusters.update(*clusters);
    }
    else 
        *clusters = knownClusters.get();

    return clusters->size();
}

unsigned GetGroups(const CString & url, IGroupVector * groups) 
{
    CSoapInitialize<WsTopologyServiceSoapProxy> server(url);

    _ns5__TpGroupQueryRequest request;

    _ns5__TpGroupQueryResponse response;
    ESP_EXCEPTION_LOG3(response.Exceptions);
    if (server.TpGroupQuery(&request, &response) == SOAP_OK)
    {
        if (response.TpGroups)
        {
            for(std::size_t i = 0; i < response.TpGroups->TpGroup.size(); ++i)
            {
                std::_tstring name;
                SAFE_ASSIGN(name, response.TpGroups->TpGroup[i]->Name);
                StlLinked<IGroup> group = CreateGroup(url, name);
                groups->push_back(group);
            }
        }
    }
    else
        _DBGLOG(url, LEVEL_WARNING, server.GetClientErrorMsg());

    return groups->size();
}

class DropZoneCompare
{
public:
    bool operator ()(const IDropZoneAdapt & l, const IDropZoneAdapt & r)
    {
        std::_tstring l_name;
        l.get()->GetName(l_name);
        std::_tstring r_name;
        r.get()->GetName(r_name);
        CString lhs = l_name.c_str();
        return lhs.CompareNoCase(r_name.c_str()) > 0 ? false : true;
    }
};

CachePool<IEclServerVector> g_eclServers;
CachePool<IDropZoneVector> g_dropZones;
CachePool<IDfuServerVector> g_dfuServers;

void thread_PrimeAllServices(const std::_tstring url)
{
    CachePoolAccessor<IEclServerVector> eclServers(g_eclServers, url);
    CachePoolAccessor<IDropZoneVector> dropZones(g_dropZones, url);
    CachePoolAccessor<IDfuServerVector> dfuServers(g_dfuServers, url);
    if (eclServers.needs_update())
    {
        CSoapInitialize<WsTopologyServiceSoapProxy> server(url);

        _ns5__TpServiceQueryRequest request;
        CStringAssign Type(request.Type, L"ALLSERVICES");

        _ns5__TpServiceQueryResponse response;
        ESP_EXCEPTION_LOG3(response.Exceptions);
        if (server.TpServiceQuery(&request, &response) == SOAP_OK)
        {
            if (response.ServiceList)
            {
                IEclServerVector tmpEclServers;
                if (response.ServiceList->TpEclServers)
                {
                    if (CComPtr<IEclCC> eclcc = CreateIEclCC())
                    {
                        StlLinked<IEclServer> eclserver = CreateEclServer(url.c_str(), _T("Local"));
                        tmpEclServers.push_back(eclserver);
                    }
                    for(std::size_t i = 0; i < response.ServiceList->TpEclServers->TpEclServer.size(); ++i)
                    {
                        StlLinked<IEclServer> eclserver = CreateEclServer(url.c_str(), response.ServiceList->TpEclServers->TpEclServer[i]);
                        tmpEclServers.push_back(eclserver);
                    }
                }
                eclServers.update(tmpEclServers);

                if (response.ServiceList->TpDropZones)
                {
                    IDropZoneVector tmpDZ;
                    for(std::size_t i = 0; i < response.ServiceList->TpDropZones->TpDropZone.size(); ++i)
                    {
                        StlLinked<IDropZone> zone = CreateDropZone(url.c_str(), response.ServiceList->TpDropZones->TpDropZone[i]);
                        tmpDZ.push_back(zone);
                    }
                    DropZoneCompare cmp;
                    std::sort(tmpDZ.begin(), tmpDZ.end(), cmp); 
                    dropZones.update(tmpDZ);
                }
                if (response.ServiceList->TpDfuServers)
                {
                    IDfuServerVector tmpDfuServers;
                    for(std::size_t i = 0; i < response.ServiceList->TpDfuServers->TpDfuServer.size(); ++i)
                    {
                        StlLinked<IDfuServer> dfuserver = CreateDfuServer(url.c_str(), response.ServiceList->TpDfuServers->TpDfuServer[i]);
                        tmpDfuServers.push_back(dfuserver);
                    }
                    dfuServers.update(tmpDfuServers);
                }
            }
        }
        else
            _DBGLOG(url, LEVEL_WARNING, server.GetClientErrorMsg());
    }
}

unsigned GetEclServers(const std::_tstring & url, IEclServerVector * eclservers) 
{
    if (url.empty())
        return 0;
    thread_PrimeAllServices(url);
    CachePoolAccessor<IEclServerVector> eclServers(g_eclServers, url);
    *eclservers = eclServers.get();
    return eclservers->size();
}

unsigned GetClustersX(const ATL::CString & url, const TCHAR *pType, IClusterVector * clusters) 
{
    CSoapInitialize<WsTopologyServiceSoapProxy> server(url);

    _ns5__TpClusterQueryRequest request;
    CStringAssign Type(request.Type, pType);

    _ns5__TpClusterQueryResponse response;
    ESP_EXCEPTION_LOG3(response.Exceptions);
    if (server.TpClusterQuery(&request, &response) == SOAP_OK)
    {
        if (response.TpClusters)
        {
            for(std::size_t i = 0; i < response.TpClusters->TpCluster.size(); ++i)
            {
                StlLinked<ICluster> cluster = CreateCluster(url, response.TpClusters->TpCluster[i]);
                clusters->push_back(cluster);
            }
        }
    }
    else
        _DBGLOG(url, LEVEL_WARNING, server.GetClientErrorMsg());

    return clusters->size();
}

unsigned GetDropZones(const std::_tstring & url, IDropZoneVector * dropZones) 
{
    thread_PrimeAllServices(url);
    CachePoolAccessor<IDropZoneVector> dropZonesA(g_dropZones, url);
    *dropZones = dropZonesA.get();
    return dropZones->size();
}

unsigned GetDfuServers(const std::_tstring & url, IDfuServerVector * servers) 
{
    thread_PrimeAllServices(url);
    CachePoolAccessor<IDfuServerVector> dfuServers(g_dfuServers, url);
    *servers = dfuServers.get();
    return servers->size();
}

class CTopology : public ITopology, public CUnknown
{
private:
protected:
    CString m_label;
    std::_tstring m_url;
    bool m_hasEclServers;

public:
    IMPLEMENT_CUNKNOWN;

    CTopology(const std::_tstring & url, const CString & label) : m_label(label), m_url(url)
    {
        m_hasEclServers = false;
    }

    ~CTopology()
    {
    }

    const TCHAR * GetID() const
    {
        return m_url.c_str();
    }

    const TCHAR * GetCacheID() const
    {
        return m_url.c_str();
    }

    const TCHAR * GetLabel() const
    {
        return m_label;
    }

    const TCHAR * GetUrl() const
    {
        return m_url.c_str();
    }

    unsigned GetClusters(const _variant_t & _queueFilter, IClusterVector & clusters) const
    {
        //clib::recursive_mutex::scoped_lock proc(m_mutex);
        CString queueFilter = _queueFilter;
        Topology::GetClusters(m_url.c_str(), (const TCHAR *)queueFilter, &clusters);
        return clusters.size();
    }

    unsigned GetClustersX(const TCHAR *type, IClusterVector & clusters) const
    {
        //clib::recursive_mutex::scoped_lock proc(m_mutex);
        Topology::GetClustersX(m_url.c_str(), type, &clusters);
        return clusters.size();
    }

    unsigned GetGroups(IGroupVector & groups) const
    {
        //clib::recursive_mutex::scoped_lock proc(m_mutex);
        Topology::GetGroups(m_url.c_str(), &groups);
        return groups.size();
    }

    unsigned GetDropZones(IDropZoneVector & dropZones) const
    {
        //clib::recursive_mutex::scoped_lock proc(m_mutex);
        Topology::GetDropZones(m_url, &dropZones);
        return dropZones.size();
    }

    IDropZone * GetDropZone(const std::_tstring & ip) const
    {
        //clib::recursive_mutex::scoped_lock proc(m_mutex);
        IDropZoneVector dropZones;
        Topology::GetDropZones(m_url, &dropZones);
        for(IDropZoneVector::iterator itr = dropZones.begin(); itr != dropZones.end(); ++itr)
        {
            CComPtr<IDropZone> dropZone = itr->get();
            CComPtr<IMachine> machine = dropZone->GetMachine();
            std::_tstring mip;
            if (boost::algorithm::equals(machine->GetNetaddress(mip), ip))
            {
                return dropZone;
            }
        }
        return NULL;
    }

    unsigned GetDfuServers(IDfuServerVector & dfuServers) const
    {
        //clib::recursive_mutex::scoped_lock proc(m_mutex);
        Topology::GetDfuServers(m_url, &dfuServers);
        return dfuServers.size();
    }
    unsigned GetEclServers(IEclServerVector & eclservers) const
    {
        //clib::recursive_mutex::scoped_lock proc(m_mutex);
        return Topology::GetEclServers(m_url, &eclservers);
    }
    bool HasEclServers() const
    {
        IEclServerVector eclservers;
        return GetEclServers(eclservers) > 1;	//  Always has a "Local"
    }
};

CacheT<std::_tstring, CTopology> TopologyCache;
void ClearSingletons()
{
    ClearClusterSingletons();
    ClearDfuServerSingletons();
    ClearDropZoneSingletons();
    ClearEclServerSingletons();
    ClearGroupSingletons();
    TopologyCache.clear();
    g_knownClusters.clear();
}
#else
unsigned GetClusters(const CString & url, const std::_tstring & queueFilter, IClusterVector * clusters) 
{
    CComInitialize com;
    ServerT server;
    server.SetUrl(url);

#if _COMMS_VER < 54400
    CStructArrayOut<TpLogicalCluster> results;
    ESP_EXCEPTION_LOG(EspException);
    if (server.TpLogicalClusterQuery(exceptions.GetArrayAddress(), exceptions.GetCountAddress(), results.GetArrayAddress(), results.GetCountAddress()) == S_OK)
#elif  _COMMS_VER < 67205
    CStructArrayOut<TpLogicalCluster> results;
    ESP_EXCEPTION_LOG(EspException);
    //CString csQueue = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_QUEUE);
    CComBSTR queue = CT2W(queueFilter.c_str(), CP_UTF8);
    if (server.TpLogicalClusterQuery(queue, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), results.GetArrayAddress(), results.GetCountAddress()) == S_OK)
    {
        std::map<std::_tstring, bool> dedupMap;
        for(int i = 0; i < results.GetCount(); ++i)
        {
            StlLinked<ICluster> cluster = CreateCluster(url, results.GetItem(i));
            CString label = cluster->GetName();
            CString labelLower = label.MakeLower();
            std::_tstring queue = cluster->GetQueue();
            if (dedupMap[static_cast<const TCHAR *>(labelLower)] == false)
            {
                clusters->push_back(cluster);
                dedupMap[static_cast<const TCHAR *>(labelLower)] = true;
            }
        }
    }
#elif  _COMMS_VER < 700000
    CStructArrayOut<TpLogicalCluster> results;
    ESP_EXCEPTION_LOG2(EspException);
    //CString csQueue = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_QUEUE);
    CComBSTR queue = CT2W(queueFilter.c_str(), CP_UTF8);
    if (server.TpLogicalClusterQuery(queue, exceptions, results.GetArrayAddress(), results.GetCountAddress()) == S_OK)
    {
        std::map<std::_tstring, bool> dedupMap;
        for(int i = 0; i < results.GetCount(); ++i)
        {
            StlLinked<ICluster> cluster = CreateCluster(url, results.GetItem(i));
            CString label = cluster->GetName();
            CString labelLower = label.MakeLower();
            std::_tstring queue = cluster->GetQueue();
            if (dedupMap[static_cast<const TCHAR *>(labelLower)] == false)
            {
                clusters->push_back(cluster);
                dedupMap[static_cast<const TCHAR *>(labelLower)] = true;
            }
        }
    }
#else
    CString csQueue = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_QUEUE);
    CComBSTR queue = csQueue;
    CStructArrayOut<LogicalCluster> results;
    ESP_STATUS_LOG;
    if (server.ListLogicalClusters(queue, results.GetArrayAddress(), results.GetCountAddress(), &status.m_statusCode, &status.m_statusMessage) == S_OK)
    {
        std::map<std::_tstring, bool> dedupMap;
        for(int i = 0; i < results.GetCount(); ++i)
        {
            StlLinked<ICluster> cluster = CreateCluster(url, results.GetItem(i));
            CString label = cluster->GetName();
            CString labelLower = label.MakeLower();
            std::_tstring queue = cluster->GetQueue();
            if (!queue.length() || boost::algorithm::iequals(queue, queueFilter))
            {
                if (dedupMap[static_cast<const TCHAR *>(labelLower)] == false)
                {
                    clusters->push_back(cluster);
                    dedupMap[static_cast<const TCHAR *>(labelLower)] = true;
                }
            }
        }
    }
#endif
    else
        _DBGLOG(m_url, LEVEL_WARNING, server.GetClientError());

    return results.GetCount();
}

unsigned GetGroups(const CString & url, IGroupVector * groups) 
{
    CComInitialize com;
    ServerT server;
    server.SetUrl(url);

#if _COMMS_VER < 67205
    CStructArrayOut<TpGroup> results;
    ESP_EXCEPTION_LOG(EspException);
    if (server.TpGroupQuery(exceptions.GetArrayAddress(), exceptions.GetCountAddress(), results.GetArrayAddress(), results.GetCountAddress()) == S_OK)
    {
        for(int i = 0; i < results.GetCount(); ++i)
        {
            std::_tstring name = CW2T(results.GetItem(i)->Name, CP_UTF8);
            StlLinked<IGroup> group = CreateGroup(url, name);
            groups->push_back(group);
        }
    }
#elif _COMMS_VER < 700000
    CStructArrayOut<TpGroup> results;
    ESP_EXCEPTION_LOG2(EspException);
    if (server.TpGroupQuery(exceptions, results.GetArrayAddress(), results.GetCountAddress()) == S_OK)
    {
        for(int i = 0; i < results.GetCount(); ++i)
        {
            std::_tstring name = CW2T(results.GetItem(i)->Name, CP_UTF8);
            StlLinked<IGroup> group = CreateGroup(url, name);
            groups->push_back(group);
        }
    }
#else
    BSTR * results = NULL;
    int resultsCount = 0;
    ESP_STATUS_LOG;
    if (server.ListComponentGroups(&results, &resultsCount, &status.m_statusCode, &status.m_statusMessage) == S_OK)
    {
        for(int i = 0; i < resultsCount; ++i)
        {
            std::_tstring result = CW2T(results[i], CP_UTF8);
            StlLinked<IGroup> group = CreateGroup(url, result);
            groups->push_back(group);
        }
        AtlCleanupValue(results);
    }
#endif
    else
        _DBGLOG(m_url, LEVEL_WARNING, server.GetClientError());

    return groups->size();
}

unsigned GetEclServers(const CString & url, IEclServerVector * eclservers) 
{
    CComInitialize com;
    ServerT server;
    server.SetUrl(url);

    CComBSTR type = L"ALLSERVICES";
    int MemThreshold = 0;
    int DiskThreshold = 0;
    int CpuThreshold = 0;
    CComBSTR MemThresholdType = _T("");
    CComBSTR DiskThresholdType = _T("");
#if _COMMS_VER < 700000
#if _COMMS_VER < 58800
    TpServices results;
    ESP_EXCEPTION_LOG(EspException);
    if (server.TpServiceQuery(type, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &results) == S_OK)
#elif _COMMS_VER < 64801
    TpServices results;
    ESP_EXCEPTION_LOG(EspException);
    if (server.TpServiceQuery(type, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &MemThreshold, &DiskThreshold, &CpuThreshold, &MemThresholdType, &DiskThresholdType, &results) == S_OK)
#elif _COMMS_VER < 67205
    bool SystemUseRewrite = false;
    TpServices results;
    ESP_EXCEPTION_LOG(EspException);
    if (server.TpServiceQuery(type, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &MemThreshold, &DiskThreshold, &CpuThreshold, &SystemUseRewrite, &MemThresholdType, &DiskThresholdType, &results) == S_OK)
#else
    bool SystemUseRewrite = false;
    TpServices results;
    ESP_EXCEPTION_LOG2(EspException);
    if (server.TpServiceQuery(type, exceptions, &MemThreshold, &DiskThreshold, &CpuThreshold, &SystemUseRewrite, &MemThresholdType, &DiskThresholdType, &results) == S_OK)
#endif
    {
        TpEclServer *TpEclServer2 = results.TpEclServers;
        int __TpEclServer2_nSizeIs = results.__TpEclServers_nSizeIs;
        for(int i = 0; i < __TpEclServer2_nSizeIs; ++i)
        {
            StlLinked<IEclServer> eclserver = CreateEclServer(url, &TpEclServer2[i]);
            eclservers->push_back(eclserver);
        }
        AtlCleanupValueEx(&results, &crtHeap);
    }
#else
    CStructOut<Services> results;
    ESP_STATUS_LOG;
    if (server.ListServices(type, &MemThreshold, &DiskThreshold, &CpuThreshold, &MemThresholdType, &DiskThresholdType, &results, &status.m_statusCode, &status.m_statusMessage) == S_OK)
    {
        ServiceDetail *detail = results.ECLServers;
        int detailCount = results.__ECLServers_nSizeIs;
        for(int i = 0; i < detailCount; ++i)
        {
            StlLinked<IEclServer> eclserver = CreateEclServer(url, &detail[i]);
            eclservers->push_back(eclserver);
        }
    }
#endif
    else
        _DBGLOG(m_url, LEVEL_WARNING, server.GetClientError());

    return eclservers->size();
}

unsigned GetClustersX(const TCHAR* url, const TCHAR *pType, IClusterVector * clusters) 
{
    CComInitialize com;
    ServerT server;
    server.SetUrl(url);

#if _COMMS_VER < 67205
    CStructArrayOut<TpCluster> results;
    _bstr_t type(pType);
    ESP_EXCEPTION_LOG(EspException);
    if (server.TpClusterQuery(type, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), results.GetArrayAddress(), results.GetCountAddress()) == S_OK)
    {
        for(int i = 0; i < results.GetCount(); ++i)
        {
            StlLinked<ICluster> cluster = CreateCluster(url, results.GetItem(i));
            clusters->push_back(cluster);
        }
    }
#elif _COMMS_VER < 700000
    CStructArrayOut<TpCluster> results;
    _bstr_t type(pType);
    ESP_EXCEPTION_LOG2(EspException);
    if (server.TpClusterQuery(type, exceptions, results.GetArrayAddress(), results.GetCountAddress()) == S_OK)
    {
        for(int i = 0; i < results.GetCount(); ++i)
        {
            StlLinked<ICluster> cluster = CreateCluster(url, results.GetItem(i));
            clusters->push_back(cluster);
        }
    }
#else
    CComBSTR type(pType);

    CStructArrayOut<Cluster> results;
    ESP_STATUS_LOG;
    if (server.ListClusters(type, results.GetArrayAddress(), results.GetCountAddress(), &status.m_statusCode, &status.m_statusMessage) == S_OK)
    {
        for(int i = 0; i < results.GetCount(); ++i)
        {
            StlLinked<ICluster> cluster = CreateCluster(url, results.GetItem(i));
            clusters->push_back(cluster);
        }
    }
#endif
    else
        _DBGLOG(m_url, LEVEL_WARNING, server.GetClientError());

    return results.GetCount();
}

class DropZoneCompare
{
public:
    bool operator ()(const IDropZoneAdapt & l, const IDropZoneAdapt & r)
    {
        std::_tstring l_name;
        l.get()->GetName(l_name);
        std::_tstring r_name;
        r.get()->GetName(r_name);
        CString lhs = l_name.c_str();
        return lhs.CompareNoCase(r_name.c_str()) > 0 ? false : true;
    }
};

unsigned GetDropZones(const CString & url, IDropZoneVector * dropZones) 
{
    CComInitialize com;
    ServerT server;
    server.SetUrl(url);

    CComBSTR type = _T("ALLSERVICES");

    int MemThreshold = 0;
    int DiskThreshold = 0;
    int CpuThreshold = 0;
    CComBSTR MemThresholdType = _T("");
    CComBSTR DiskThresholdType = _T("");

#if _COMMS_VER < 700000
    CStructOut<TpServices> results;
#if _COMMS_VER < 58800
    ESP_EXCEPTION_LOG(EspException);
    if (server.TpServiceQuery(type, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &results) == S_OK)
#elif _COMMS_VER < 64801
    ESP_EXCEPTION_LOG(EspException);
    if (server.TpServiceQuery(type, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &MemThreshold, &DiskThreshold, &CpuThreshold, &MemThresholdType, &DiskThresholdType, &results) == S_OK)
#elif _COMMS_VER < 67205
    bool SystemUseRewrite = false;
    ESP_EXCEPTION_LOG(EspException);
    if (server.TpServiceQuery(type, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &MemThreshold, &DiskThreshold, &CpuThreshold, &SystemUseRewrite, &MemThresholdType, &DiskThresholdType, results) == S_OK)
#else
    bool SystemUseRewrite = false;
    ESP_EXCEPTION_LOG2(EspException);
    if (server.TpServiceQuery(type, exceptions, &MemThreshold, &DiskThreshold, &CpuThreshold, &SystemUseRewrite, &MemThresholdType, &DiskThresholdType, results) == S_OK)
#endif
    {
        TpDropZone *TpDropZone10 = results->TpDropZones;
        int __TpDropZone10_nSizeIs = results->__TpDropZones_nSizeIs;
        for(int i = 0; i < __TpDropZone10_nSizeIs; ++i)
        {
            StlLinked<IDropZone> zone = CreateDropZone(url, TpDropZone10+i);
            dropZones->push_back(zone);
        }
        DropZoneCompare cmp;
        std::sort(dropZones->begin(), dropZones->end(), cmp); 
    }
#else
    CStructOut<Services> results;
    ESP_STATUS_LOG;
    if (server.ListServices(type, &MemThreshold, &DiskThreshold, &CpuThreshold, &MemThresholdType, &DiskThresholdType, &results, &status.m_statusCode, &status.m_statusMessage) == S_OK)
    {
        ServiceDetail *detail = results.DropZones;
        int detailCount = results.__DropZones_nSizeIs;
        for(int i = 0; i < detailCount; ++i)
        {
            StlLinked<IDropZone> dropZone = CreateDropZone(url, &detail[i]);
            dropZones->push_back(dropZone);
        }

        DropZoneCompare cmp;
        std::sort(dropZones->begin(), dropZones->end(), cmp); 
    }
#endif
    else
        _DBGLOG(m_url, LEVEL_WARNING, server.GetClientError());

    return dropZones->size();
}

unsigned GetDfuServers(const CString & url, IDfuServerVector * servers) 
{
    CComInitialize com;
    ServerT server;
    server.SetUrl(url);

    CComBSTR type = _T("ALLSERVICES");
    int MemThreshold = 0;
    int DiskThreshold = 0;
    int CpuThreshold = 0;
    CComBSTR MemThresholdType = _T("");
    CComBSTR DiskThresholdType = _T("");
#if _COMMS_VER < 700000
    CStructOut<TpServices> results;
#if _COMMS_VER < 58800
    ESP_EXCEPTION_LOG(EspException);
    if (server.TpServiceQuery(type, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &results) == S_OK)
#elif _COMMS_VER < 64801
    ESP_EXCEPTION_LOG(EspException);
    if (server.TpServiceQuery(type, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &MemThreshold, &DiskThreshold, &CpuThreshold, &MemThresholdType, &DiskThresholdType, &results) == S_OK)
#elif _COMMS_VER < 67205
    bool SystemUseRewrite = false;
    ESP_EXCEPTION_LOG(EspException);
    if (server.TpServiceQuery(type, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &MemThreshold, &DiskThreshold, &CpuThreshold, &SystemUseRewrite, &MemThresholdType, &DiskThresholdType, results) == S_OK)
#else
    bool SystemUseRewrite = false;
    ESP_EXCEPTION_LOG2(EspException);
    if (server.TpServiceQuery(type, exceptions, &MemThreshold, &DiskThreshold, &CpuThreshold, &SystemUseRewrite, &MemThresholdType, &DiskThresholdType, results) == S_OK)
#endif
    {
        TpDfuServer *TpDfuServer6 = results->TpDfuServers;
        int __TpDfuServer6_nSizeIs = results->__TpDfuServers_nSizeIs;
        for(int i = 0; i < __TpDfuServer6_nSizeIs; ++i)
        {
            StlLinked<IDfuServer> server = CreateDfuServer(url, TpDfuServer6+i);
            servers->push_back(server);
        }
    }
#else
    CStructOut<Services> results;
    ESP_STATUS_LOG;
    if (server.ListServices(type, &MemThreshold, &DiskThreshold, &CpuThreshold, &MemThresholdType, &DiskThresholdType, &results, &status.m_statusCode, &status.m_statusMessage) == S_OK)
    {
        ServiceDetail *detail = results.DFUServers;
        int detailCount = results.__DFUServers_nSizeIs;
        for(int i = 0; i < detailCount; ++i)
        {
            StlLinked<IDfuServer> dfuServer = CreateDfuServer(url, &detail[i]);
            servers->push_back(dfuServer);
        }
    }
#endif
    else
        _DBGLOG(m_url, LEVEL_WARNING, server.GetClientError());

    return servers->size();
}


class CTopology : public ITopology, public CUnknown
{
private:
protected:
    CString m_label;
    CString m_url;

public:
    IMPLEMENT_CUNKNOWN;

    CTopology(const CString & url, const CString & label) : m_label(label), m_url(url)
    {
    }

    ~CTopology()
    {
    }

    const TCHAR * GetID() const
    {
        return m_url;
    }

    const TCHAR * GetCacheID() const
    {
        return m_url;
    }

    const TCHAR * GetLabel() const
    {
        return m_label;
    }

    const TCHAR * GetUrl() const
    {
        return m_url;
    }

    unsigned GetClusters(const CString & queueFilter, IClusterVector & clusters) const
    {
        Topology::GetClusters(m_url, static_cast<const TCHAR *>(queueFilter), &clusters);
        return clusters.size();
    }

    unsigned GetClustersX(const TCHAR *type, IClusterVector & clusters) const
    {
        Topology::GetClustersX(m_url, type, &clusters);
        return clusters.size();
    }

    unsigned GetGroups(IGroupVector & groups) const
    {
        Topology::GetGroups(m_url, &groups);
        return groups.size();
    }

    unsigned GetDropZones(IDropZoneVector & dropZones) const
    {
        Topology::GetDropZones(m_url, &dropZones);
        return dropZones.size();
    }

    IDropZone * GetDropZone(const std::_tstring & ip) const
    {
        IDropZoneVector dropZones;
        Topology::GetDropZones(m_url, &dropZones);
        for(IDropZoneVector::iterator itr = dropZones.begin(); itr != dropZones.end(); ++itr)
        {
            CComPtr<IDropZone> dropZone = itr->get();
            CComPtr<IMachine> machine = dropZone->GetMachine();
            std::_tstring mip;
            if (boost::algorithm::equals(machine->GetNetaddress(mip), ip))
            {
                return dropZone;
            }
        }
        return NULL;
    }

    unsigned GetDfuServers(IDfuServerVector & dfuServers) const
    {
        Topology::GetDfuServers(m_url, &dfuServers);
        return dfuServers.size();
    }
    unsigned GetEclServers(IEclServerVector & eclservers) const
    {
        return Topology::GetEclServers(m_url, &eclservers);
    }
};

CacheT<std::_tstring, CTopology> TopologyCache;

void ClearSingletons()
{
    ClearClusterSingletons();
    ClearDfuServerSingletons();
    ClearDropZoneSingletons();
    ClearEclServerSingletons();
    ClearGroupSingletons();
    TopologyCache.clear();
}
#endif
} //namespace
COMMS_API Topology::ITopology * AttachTopology(const std::_tstring & url, const std::_tstring & label)
{
    boost::thread run(boost::bind(&Topology::thread_PrimeAllServices, url));
    return Topology::TopologyCache.Get(new Topology::CTopology(url, label.c_str()));
}

COMMS_API Topology::ITopology * AttachTopology(const _variant_t & _url, const std::_tstring & label)
{
    std::_tstring url = (const TCHAR *)CString(_url);
    boost::thread run(boost::bind(&Topology::thread_PrimeAllServices, url));
    return Topology::TopologyCache.Get(new Topology::CTopology(url, label.c_str()));
}
