#include "StdAfx.h"
#include "smc.h"

#include "cache.h"
#include "gSoapUtil.h"
#include "clib.h"
#include "logger.h"
#include "global.h"

#if _COMMS_VER < 68200
using namespace WsSMC;
typedef CWsSMCT<CCustomActionSecureSoapSocketClient> ServerT;
#elif _COMMS_VER < 70000
#else
using namespace WsECLWorkunits;
using namespace WsEnvironment;
typedef ActiveOrQueuedWorkunit ActiveWorkunit;
typedef CWsECLWorkunitsT<CCustomActionSecureSoapSocketClient> ServerT;
#endif

namespace SMC
{
#if _COMMS_VER >= 68200
#define ESP_EXCEPTION_LOG3(T) CReportingGSoapEspException<ns4__ArrayOfEspException> exceptions(T, __FILE__, __LINE__)
ICluster * CreateCluster(const CString & url, const std::_tstring & cluster, const std::_tstring & status, const std::vector<ns4__ActiveWorkunit *> &running);
IVersion * CreateVersion(const CString & url, const CString & version);
void ClearVersionCache();
void ClearClusterCache();

class CSMC : public ISMC, public CUnknown
{
protected:
	CString m_label;
	CString m_url;
	CComPtr<IVersion> m_version;
	CComPtr<IVersion> m_unknown;

public:
	IMPLEMENT_CUNKNOWN;

	CSMC(const CString & url, const CString & label) : m_label(label), m_url(url)
	{
		m_unknown = CreateVersion(_T("http://www.seisint.com/"), _T("build_0_0"));
	}

	~CSMC()
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

	bool GetActive(IClusterVector * clusters)
	{
		CSoapInitialize<WsSMCServiceSoapProxy> server(m_url);

		_ns4__Activity request;

		_ns4__ActivityResponse response;
		ESP_EXCEPTION_LOG3(response.Exceptions);
		if (server.Activity(&request, &response) == SOAP_OK)
		{
			if (response.Build)
				m_version = CreateVersion(m_url, response.Build->c_str());

			//  Multi thor support - there could be some jobs running on a multi thor!
			if (response.Running)
			{
				for(std::size_t i = 0; i < response.Running->ActiveWorkunit.size(); ++i)
				{
					std::_tstring clusterName, queueStatus;
					SAFE_ASSIGN(clusterName,  response.Running->ActiveWorkunit[i]->Instance);
					SAFE_ASSIGN(queueStatus, response.Running->ActiveWorkunit[i]->State);

					StlLinked<ICluster> cluster = CreateCluster(m_url, clusterName, queueStatus, response.Running->ActiveWorkunit);
					if (std::find(clusters->begin(), clusters->end(), cluster) == clusters->end())
					{
						clusters->push_back(cluster.get());
					}
				}
			}
			return true;
		}
		else
			_DBGLOG(m_url, LEVEL_WARNING, server.GetClientErrorMsg());

		return false;
	}

	IVersion * GetBuild()
	{
		if (m_version)
			return m_version;
		IClusterVector results;
		if (GetActive(&results))
			return m_version;
		return m_unknown;
	}

	bool SetPriority(const std::_tstring &cluster, const std::_tstring &wuid, PRIORITY priority)
	{
		CSoapInitialize<WsSMCServiceSoapProxy> server(m_url);

		_ns4__SMCJobResponse response;
		ESP_EXCEPTION_LOG3(response.Exceptions);

		HRESULT result;
		switch (priority)
		{
		case PRIORITY_UP:
			{
				_ns4__MoveJobUp request;
				CStringAssign Cluster(request.Cluster, cluster);
				CStringAssign Wuid(request.Wuid, wuid);
				result = server.MoveJobUp(&request, &response);
			}
			break;
		case PRIORITY_DOWN:
			{
				_ns4__MoveJobDown request;
				CStringAssign Cluster(request.Cluster, cluster);
				CStringAssign Wuid(request.Wuid, wuid);
				result = server.MoveJobDown(&request, &response);
			}
			break;
		case PRIORITY_TOP:
			{
				_ns4__MoveJobFront request;
				CStringAssign Cluster(request.Cluster, cluster);
				CStringAssign Wuid(request.Wuid, wuid);
				result = server.MoveJobFront(&request, &response);
			}
			break;
		case PRIORITY_BOTTOM:
			{
				_ns4__MoveJobBack request;
				CStringAssign Cluster(request.Cluster, cluster);
				CStringAssign Wuid(request.Wuid, wuid);
				result = server.MoveJobBack(&request, &response);
			}
			break;
		default:
			_DBGLOG(m_url, LEVEL_WARNING, _T("Unknown Priority"));
			return false;
		}

		if(result == S_OK)
			return true;
		else
			_DBGLOG(m_url, LEVEL_WARNING, server.GetClientErrorMsg());
		return false;
	}

	bool SetPriority(Dali::IWorkunitVector *workunits, PRIORITY priority)
	{
		switch (priority)
		{
		case PRIORITY_UP:
		case PRIORITY_TOP:
			{
				for(Dali::IWorkunitVector::iterator itr = workunits->begin(); itr != workunits->end(); ++itr)
				{
					Dali::IWorkunitAdapt& workunit = *itr;
					SetPriority(workunit->GetCluster(), workunit->GetWuid(), priority);
				}
			}
			break;
		case PRIORITY_DOWN:
		case PRIORITY_BOTTOM:
			{
				for (Dali::IWorkunitVector::reverse_iterator itr = workunits->rbegin(); itr != workunits->rend(); ++itr)
				{
					Dali::IWorkunitAdapt& workunit = *itr;
					SetPriority(workunit->GetCluster(), workunit->GetWuid(), priority);
				}
			}
			break;
		}
		return true;
	}
};
#else
#if _COMMS_VER < 505
ICluster * CreateCluster(const CString & url, ThorCluster *thorCluster);
#else
ICluster * CreateCluster(const CString & url, ThorCluster *thorCluster, CStructArrayOut<ActiveWorkunit> &running);
#endif
IVersion * CreateVersion(const CString & url, const CString & version);
void ClearVersionCache();
void ClearClusterCache();

class CSMC : public ISMC, public CUnknown
{
protected:
	CString m_label;
	CString m_url;
	CComPtr<IVersion> m_version;
	CComPtr<IVersion> m_unknown;

public:
	IMPLEMENT_CUNKNOWN;

	CSMC(const CString & url, const CString & label) : m_label(label), m_url(url)
	{
		m_unknown = CreateVersion(_T("http://www.seisint.com/"), _T("build_0_0"));
	}

	~CSMC()
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

	bool GetActive(IClusterVector * clusters)
	{
		CComInitialize com;
		ServerT server;
		CString url = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_SMC);
		server.SetUrl(url);

		CStructArrayOut<ActiveWorkunit> running;
#if _COMMS_VER < 499
		bool thorQueueManagement;
		if (server.Index(exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &build, thorClusters.GetArrayAddress(), thorClusters.GetCountAddress(), holeClusters.GetArrayAddress() , holeClusters.GetCountAddress(), dfuJobs.GetArrayAddress(), dfuJobs.GetCountAddress(), &thorQueueManagement) == S_OK)
#elif _COMMS_VER < 505
		bool thorQueueManagement;
		CStructArrayOut<ActiveWorkunit> running;
		if (server.Index(exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &build, thorClusters.GetArrayAddress(), thorClusters.GetCountAddress(), holeClusters.GetArrayAddress() , holeClusters.GetCountAddress(), dfuJobs.GetArrayAddress(), dfuJobs.GetCountAddress(), running.GetArrayAddress(), running.GetCountAddress(), &thorQueueManagement) == S_OK)
#elif _COMMS_VER < 50509
		CStructArrayOut<ActiveWorkunit> running;
		if (server.Index(exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &build, thorClusters.GetArrayAddress(), thorClusters.GetCountAddress(), holeClusters.GetArrayAddress() , holeClusters.GetCountAddress(), dfuJobs.GetArrayAddress(), dfuJobs.GetCountAddress(), running.GetArrayAddress(), running.GetCountAddress()) == S_OK)
#elif _COMMS_VER < 51100
		CStructArrayOut<ActiveWorkunit> running;
		if (server.Activity(false, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &build, thorClusters.GetArrayAddress(), thorClusters.GetCountAddress(), holeClusters.GetArrayAddress() , holeClusters.GetCountAddress(), dfuJobs.GetArrayAddress(), dfuJobs.GetCountAddress(), running.GetArrayAddress(), running.GetCountAddress()) == S_OK)
#elif _COMMS_VER < 67205
		CComBSTR build;
		CStructArrayOut<ThorCluster> thorClusters;
		CStructArrayOut<HoleCluster> holeClusters;
		CStructArrayOut<DFUJob> dfuJobs;
		ESP_EXCEPTION_LOG(EspException);
		if (server.Activity(exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &build, thorClusters.GetArrayAddress(), thorClusters.GetCountAddress(), holeClusters.GetArrayAddress() , holeClusters.GetCountAddress(), dfuJobs.GetArrayAddress(), dfuJobs.GetCountAddress(), running.GetArrayAddress(), running.GetCountAddress()) == S_OK)
		{
			if (build)
				m_version = CreateVersion(url, CString(build));

			//  Multi thor support - there could be some jobs running on a multi thor!
			for(int i = 0; i < running.GetCount(); ++i)
			{
				CComBSTR thorClusterName = running.GetItem(i)->Instance;
#elif _COMMS_VER < 70000
		CComBSTR build;
		CStructArrayOut<ThorCluster> thorClusters;
		CStructArrayOut<HoleCluster> holeClusters;
		CStructArrayOut<DFUJob> dfuJobs;
		ESP_EXCEPTION_LOG2(EspException);
		if (server.Activity(exceptions, &build, thorClusters.GetArrayAddress(), thorClusters.GetCountAddress(), holeClusters.GetArrayAddress() , holeClusters.GetCountAddress(), dfuJobs.GetArrayAddress(), dfuJobs.GetCountAddress(), running.GetArrayAddress(), running.GetCountAddress()) == S_OK)
		{
			if (build)
				m_version = CreateVersion(url, CString(build));

			//  Multi thor support - there could be some jobs running on a multi thor!
			for(int i = 0; i < running.GetCount(); ++i)
			{
				CComBSTR thorClusterName = running.GetItem(i)->Instance;
#else
		ESP_STATUS_LOG;
		if (server.ListActiveOrQueuedWorkunits(running.GetArrayAddress(), running.GetCountAddress(), &status.m_statusCode, &status.m_statusMessage) == S_OK)
		{
			//  Multi thor support - there could be some jobs running on a multi thor!
			for(int i = 0; i < running.GetCount(); ++i)
			{
				CComBSTR thorClusterName = running.GetItem(i)->ServerInstance;
#endif
				CComBSTR thorClusterStatus = running.GetItem(i)->State;
				ThorCluster thorCluster;
				thorCluster.ClusterName = thorClusterName;
				thorCluster.QueueStatus = thorClusterStatus;
				StlLinked<ICluster> cluster = CreateCluster(m_url, &thorCluster, running);
				if (std::find(clusters->begin(), clusters->end(), cluster) == clusters->end())
				{
					clusters->push_back(cluster.get());
				}
			}
			return true;
		}
		else
			_DBGLOG(m_url, LEVEL_WARNING, server.GetClientError());

		return false;
	}

	IVersion * GetBuild()
	{
		if (m_version)
			return m_version;
		IClusterVector results;
		if (GetActive(&results))
			return m_version;
		return m_unknown;
	}

	bool SetPriority(const std::_tstring &cluster, const std::_tstring &wuid, PRIORITY priority)
	{
		CComInitialize com;
		ServerT server;
		server.SetUrl(CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_SMC)));

		CComBSTR inCluster = CT2W(cluster.c_str(), CP_UTF8);
		CComBSTR inWuid = CT2W(wuid.c_str(), CP_UTF8);

		HRESULT result;
#if _COMMS_VER < 67205 
		ESP_EXCEPTION_LOG(EspException);
#elif _COMMS_VER < 70000 
		ESP_EXCEPTION_LOG2(EspException);
#else
		ESP_STATUS_LOG;
#endif
		switch (priority)
		{
		case PRIORITY_UP:
			{
#if _COMMS_VER < 51100 
				result = server.MoveJobUp(inCluster, inWuid);
#elif _COMMS_VER < 67205
				result = server.MoveJobUp(inCluster, inWuid, exceptions.GetArrayAddress(), exceptions.GetCountAddress());
#elif _COMMS_VER < 70000 
				result = server.MoveJobUp(inCluster, inWuid, exceptions);
#else
				CComBSTR position = _T("up");
				result = server.SetWorkunitPositionInSamePriority(inCluster, inWuid, position, &status.m_statusCode, &status.m_statusMessage);
#endif
			}
			break;
		case PRIORITY_DOWN:
			{
#if _COMMS_VER < 51100 
				result = server.MoveJobDown(inCluster, inWuid);
#elif _COMMS_VER < 67205
				result = server.MoveJobDown(inCluster, inWuid, exceptions.GetArrayAddress(), exceptions.GetCountAddress());
#elif _COMMS_VER < 70000 
				result = server.MoveJobDown(inCluster, inWuid, exceptions);
#else
				CComBSTR position = _T("down");
				result = server.SetWorkunitPositionInSamePriority(inCluster, inWuid, position, &status.m_statusCode, &status.m_statusMessage);
#endif
			}
			break;
		case PRIORITY_TOP:
			{
#if _COMMS_VER < 51100 
				result = server.MoveJobFront(inCluster, inWuid);
#elif _COMMS_VER < 67205
				result = server.MoveJobFront(inCluster, inWuid, exceptions.GetArrayAddress(), exceptions.GetCountAddress());
#elif _COMMS_VER < 70000 
				result = server.MoveJobFront(inCluster, inWuid, exceptions);
#else
				CComBSTR position = _T("front");
				result = server.SetWorkunitPositionInSamePriority(inCluster, inWuid, position, &status.m_statusCode, &status.m_statusMessage);
#endif
			}
			break;
		case PRIORITY_BOTTOM:
			{
#if _COMMS_VER < 51100 
				result = server.MoveJobBack(inCluster, inWuid);
#elif _COMMS_VER < 67205
				result = server.MoveJobBack(inCluster, inWuid, exceptions.GetArrayAddress(), exceptions.GetCountAddress());
#elif _COMMS_VER < 70000 
				result = server.MoveJobBack(inCluster, inWuid, exceptions);
#else
				CComBSTR position = _T("end");
				result = server.SetWorkunitPositionInSamePriority(inCluster, inWuid, position, &status.m_statusCode, &status.m_statusMessage);
#endif
			}
			break;
		default:
			_DBGLOG(m_url, LEVEL_WARNING, server.GetClientError());
			return false;
		}
		if(result == S_OK)
		{
			return true;
		}
		else
			_DBGLOG(m_url, LEVEL_WARNING, server.GetClientError());
		return false;
	}

	bool SetPriority(Dali::IWorkunitVector *workunits, PRIORITY priority)
	{
		switch (priority)
		{
		case PRIORITY_UP:
		case PRIORITY_TOP:
			{
				for(Dali::IWorkunitVector::iterator itr = workunits->begin(); itr != workunits->end(); ++itr)
				{
					Dali::IWorkunitAdapt& workunit = *itr;
					SetPriority(workunit->GetCluster(), workunit->GetWuid(), priority);
				}
			}
			break;
		case PRIORITY_DOWN:
		case PRIORITY_BOTTOM:
			{
				for (Dali::IWorkunitVector::reverse_iterator itr = workunits->rbegin(); itr != workunits->rend(); ++itr)
				{
					Dali::IWorkunitAdapt& workunit = *itr;
					SetPriority(workunit->GetCluster(), workunit->GetWuid(), priority);
				}
			}
			break;
		}
		return true;
	}
};
#endif

CacheT<std::_tstring, CSMC> SMCCache;
ISMC * AttachSMC(const std::_tstring & url, const std::_tstring & label)
{
	return SMCCache.Get(new CSMC(url.c_str(), label.c_str()));
}
COMMS_API ISMC * AttachSMC(const _variant_t & url, const std::_tstring & label)
{
	return SMCCache.Get(new CSMC(url, label.c_str()));
}
void ClearSingletons()
{
	ClearVersionCache();
	ClearClusterCache();
	SMCCache.clear();
}
}
