#include "StdAfx.h"

#include "dfuworkunit.h"
#include "filespray.h"
#include "cache.h"

using namespace FileSpray;

namespace WsFileSpray
{
class CWorkunit : public IWorkunit, public clib::CLockableUnknown
{
protected:
	std::_tstring m_id;
	std::_tstring m_url;
	std::_tstring m_wuid;
	WUState m_stateID;
	std::_tstring m_state;
	int m_percentDone;
	std::_tstring m_progressMessage;

	workunit_signal_type on_refresh;

public:
	IMPLEMENT_CLOCKABLEUNKNOWN;

	CWorkunit(const std::_tstring & url, const std::_tstring & wuid) : m_url(url), m_wuid(wuid)
	{
		m_id = m_url + _T("/") + m_wuid;
		m_stateID = DFUstate_unknown;
		m_percentDone = 0;
	}
	virtual ~CWorkunit()
	{
	}
	const TCHAR *GetID() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_id.c_str();
	}
	const TCHAR * GetCacheID() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_id.c_str();
	}
	const TCHAR *GetWuid() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_wuid.c_str();
	}
	WUState GetState() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_stateID;
	}
	void SetState(WUState state)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		m_stateID = state;
	}
	const TCHAR *GetStateLabel() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_state.c_str();
	}
	bool IsComplete() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		switch (m_stateID)
		{
		case DFUstate_finished:
		case DFUstate_failed:
		case DFUstate_aborted:
			return true;
		}
		return false;
	}

	int GetPercentDone() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_percentDone;
	}

	const TCHAR * GetProgressMessage() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_progressMessage.c_str();
	}

	void Update(const DFUWorkunit * c)
	{
		/*
    BSTR ID;
    BSTR ClusterName;
    BSTR JobName;
    BSTR Queue;
    BSTR User;
    bool isProtected;
    int Command;
    int PercentDone;
    int SecsLeft;
    BSTR ProgressMessage;
    BSTR SummaryMessage;
    int State;
    BSTR SourceLogicalName;
    BSTR SourceIP;
    BSTR SourceFilePath;
    BSTR SourceDali;
    int SourceRecordSize;
    int SourceFormat;
    BSTR RowTag;
    int SourceNumParts;
    BSTR SourceDirectory;
    BSTR DestLogicalName;
    BSTR DestGroupName;
    BSTR DestDirectory;
    BSTR DestIP;
    BSTR DestFilePath;
    int DestFormat;
    int DestNumParts;
    int DestRecordSize;
    bool Replicate;
    bool Overwrite;
    bool Compress;
    BSTR TimeStarted;
    BSTR TimeStopped;
    BSTR StateMessage;
    BSTR MonitorEventName;
    bool MonitorSub;
    int MonitorShotLimit;
*/
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		m_wuid = CW2T(c->ID, CP_UTF8);
		m_stateID = static_cast<WUState>(c->State);
		m_state = CW2T(c->StateMessage, CP_UTF8);
		m_percentDone = c->PercentDone;
		m_progressMessage = CW2T(c->ProgressMessage, CP_UTF8);
		proc.unlock();
		Refresh();
	}

	void Refresh() 
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		if (m_mutex.GetLockCount() == 1)	//  If there are other locks, we had better not refresh!
		{
			proc.unlock();
			on_refresh(this);
		}
	}

	void MonitorState() 
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		if (IsComplete())
		{
			proc.unlock();
			Refresh();
			return;
		}
		StlLinked<IFileSpray> server = AttachFileSpray();
		std::_tstring wuid;
		server->MonitorState(m_wuid);
	}

	boost::signals::connection on_refresh_connect(const workunit_slot_type& s)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return on_refresh.connect(s); 
	}

	bool GetScopedLock(clib::scoped_lock_ref_counted_adapt &lock) const
	{
		lock = new clib::scoped_lock_ref_counted(m_mutex);
		return true;
	}
};

static CacheT<std::_tstring, CWorkunit> WorkunitCache;

void ClearWorkunitCache()
{
	WorkunitCache.Clear();
}

void ClearWorkunitCache(const std::_tstring & id)
{
	ATLTRACE(_T("Workunit cache before clearing(size=%u)\r\n"), WorkunitCache.Size());
	WorkunitCache.Clear(id);
}

CWorkunit * CreateIWorkunitRaw(const std::_tstring & url, const std::_tstring & wuid)
{
	CWorkunit *wu = WorkunitCache.Get(new CWorkunit(url, wuid));
	//ATLTRACE(_T("Workunit cache(size=%u)\r\n"), WorkunitCache.Size());
	return wu;
}

IWorkunit * CreateIWorkunit(const std::_tstring & url, DFUWorkunit *data)
{
	CWorkunit *wu = WorkunitCache.Get(new CWorkunit(url, std::_tstring(CW2T(data->ID, CP_UTF8))));
	ATLASSERT(wu);
	wu->Update(data);
	return wu;
}

//IWorkunit * CreateWorkunit(const CServerConfig& config, const ECLWorkunit * data, clib::scoped_lock_ref_counted_adapt * lock)
//{
//	CWorkunit * wu = CreateWorkunitRaw(config, data->Wuid);
//	ATLASSERT(wu);
//	if (lock != NULL)
//	{
//		wu->GetScopedLock(*lock);
//	}
//	wu->Update(data);
//	return wu;
//}
//
//IWorkunit * CreateWorkunit(const std::_tstring & url, const std::_tstring & daliLabel, const std::_tstring & wuid)
//{
//	return CreateWorkunitRaw(CServerConfig(url, daliLabel), wuid);
//}
//
//IWorkunit * CreateWorkunit(const std::_tstring & url, const std::_tstring & daliLabel, const ECLWorkunit * data)
//{
//	return CreateWorkunit(CServerConfig(url, daliLabel), data, NULL);
//}

}