#include "StdAfx.h"
#include "Cluster.h"

#include "smc.h"
#include "gSoapUtil.h"
#include "Cache.h"

#if _COMMS_VER < 68200
using namespace WsSMC;
#elif _COMMS_VER < 70000
#else
using namespace WsECLWorkunits;
using namespace WsEnvironment;
typedef ActiveOrQueuedWorkunit ActiveWorkunit;
#endif

namespace Dali
{
#if _COMMS_VER < 68200
	IWorkunit * CreateWorkunit(const CString & url, const CString & daliLabel, const ActiveWorkunit *data, bool noBroadcast = false);
#elif _COMMS_VER < 70000
	IWorkunit * CreateWorkunit(const CString & url, const CString & daliLabel, const ns4__ActiveWorkunit *data, bool noBroadcast = false);
#endif
}

namespace SMC
{
class CCluster : public ICluster, public clib::CLockableUnknown
{
protected:
	CString m_id;
	CString m_url;
	CString m_cluster;
	CString m_status;
	bool m_paused;
	bool m_stopped;
	CComPtr<Dali::IWorkunit> m_running;
	Dali::IWorkunitVector m_queue;

public:
	IMPLEMENT_CLOCKABLEUNKNOWN;

	CCluster(const CString & url, const CString &cluster) : m_url(url), m_cluster(cluster)
	{
		m_id = m_url + _T("/") + m_cluster;
	}

	const TCHAR * GetID()
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_id;
	}

	const TCHAR * GetCacheID()
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_id;
	}

	const TCHAR * GetLabel()
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_cluster;
	}

	Dali::IWorkunit *GetRunning()
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_running;
	}

	int GetQueueCount()
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_queue.size();
	}

	Dali::IWorkunit *GetQueueItem(unsigned item)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		ATLASSERT(item >= 0 && item < m_queue.size());
		return m_queue[item];
	}

#if _COMMS_VER < 68200
#if _COMMS_VER < 505
	void Update(const ThorCluster *thorCluster)
#else
	void Update(const ThorCluster *thorCluster, CStructArrayOut<ActiveWorkunit> &running)
#endif
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		m_cluster = thorCluster->ClusterName;
		m_queue.clear();
#if _COMMS_VER < 505
		StlLinked<Dali::IDali> server = Dali::AttachDali(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT), _T("Dali4Smc"));  //Must different from "Dali"
		m_paused = thorCluster->QueuePaused;
		m_stopped = thorCluster->QueueStopped;
		CString wuid = thorCluster->Running.Wuid;
		if (wuid.IsEmpty())
			m_running = NULL;
		else
			m_running = server->GetWorkunit(wuid);

		ActiveWorkunit * wuq = thorCluster->Queue;
		for(int i = 0; i < thorCluster->__Queue_nSizeIs; ++i, ++wuq)
			m_queue.push_back(server->GetWorkunit(wuq->Wuid));
#else
		m_status = thorCluster->QueueStatus;
		for(int i = 0; i < running.GetCount(); ++i)
		{
#if _COMMS_VER < 70000
			if (m_cluster.CompareNoCase(CW2T(running.GetItem(i)->Instance, CP_UTF8)) == 0)
#else
			if (m_cluster.CompareNoCase(CW2T(running.GetItem(i)->ServerInstance, CP_UTF8)) == 0)
#endif
			{
				//  Creating a workunit can cause a deadlock with the active tree, so set noBroadcast to true.
				CComPtr<Dali::IWorkunit> wu  = Dali::CreateWorkunit(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT), _T("Dali"), running.GetItem(i), true);
				ATLASSERT(wu);
				m_queue.push_back(wu.p);
			}
		}
#endif
	}
#elif _COMMS_VER < 70000
	void Update(const std::_tstring &cluster, const std::_tstring &status, const std::vector<ns4__ActiveWorkunit *> &running)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		m_cluster = cluster.c_str();
		m_queue.clear();
		m_status = status.c_str();
		for(std::size_t i = 0; i < running.size(); ++i)
		{
			if (running[i]->Instance && m_cluster.CompareNoCase(running[i]->Instance->c_str()) == 0)
			{
				//  Creating a workunit can cause a deadlock with the active tree, so set noBroadcast to true.
				CComPtr<Dali::IWorkunit> wu  = Dali::CreateWorkunit(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT), _T("Dali"), running[i], true);
				ATLASSERT(wu);
				m_queue.push_back(wu.p);
			}
		}
	}
#endif
};

CacheT<std::_tstring, CCluster> ClusterCache;
CCluster * CreateClusterRaw(const CString & url, const CString & cluster)
{
	return ClusterCache.Get(new CCluster(url, cluster));
}

ICluster * CreateCluster(const CString & url, const CString &cluster)
{
	return CreateClusterRaw(url, cluster);
}

#if _COMMS_VER < 505
ICluster * CreateCluster(const CString & url, ThorCluster *thorCluster)
{
	CCluster * retVal = CreateClusterRaw(url, thorCluster->ClusterName);
	ATLASSERT(retVal);
	retVal->Update(thorCluster);
	return retVal;
}
#elif _COMMS_VER < 68200
ICluster * CreateCluster(const CString & url, ThorCluster *thorCluster, CStructArrayOut<ActiveWorkunit> &running)
{
	CCluster * retVal = CreateClusterRaw(url, thorCluster->ClusterName);
	ATLASSERT(retVal);
	retVal->Update(thorCluster, running);
	return retVal;
}
#elif _COMMS_VER < 70000
ICluster * CreateCluster(const CString & url, const std::_tstring & cluster, const std::_tstring & status, const std::vector<ns4__ActiveWorkunit *> &running)
{
	CCluster * retVal = CreateClusterRaw(url, cluster.c_str());
	ATLASSERT(retVal);
	retVal->Update(cluster, status, running);
	return retVal;
}
#endif
void ClearClusterCache()
{
	ClusterCache.clear();
}

}