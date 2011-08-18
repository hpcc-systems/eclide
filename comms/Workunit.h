#pragma once

#include "comms.h"
#include "clib.h"
#include "result.h"
#include "graph.h"
#include "localfile.h"
#include "table.h"
#include "GraphContainer.h"
#include "StringSet.h"

namespace Dali
{
const TCHAR * const RESULTLIMIT_KEY = _T("QueryBuilderResultLimit");
const TCHAR * const MAXRUNTIME_KEY = _T("MAxRunTime");
__interface IWorkunit;

enum WUEvent
{
	WUEventUnknown = 0,
	WUEventUpdated,
	WUEventDeleted,
	WUEventLast
};

typedef boost::signal<void(WUEvent, IWorkunit *)> workunit_signal_type;
typedef workunit_signal_type::slot_type workunit_slot_type;

enum WUState 
{
	WUStateUnknown = 0,
	WUStateCompiled,
	WUStateRunning,
	WUStateCompleted,
	WUStateFailed,
	WUStateArchived,
	WUStateAborting,
	WUStateAborted,
	WUStateBlocked,
	WUStateSubmitted,
	WUStateScheduled,
	WUStateCompiling,
	WUStateWait,
	WUStateWaitingForUpload,
	WUStateDebugPaused,
	WUStateDebugRunning,
	WUStateSize	//Don't forget to update the string table below..
};

const TCHAR * const WUState_TEXT[] =
{
	_T("Unknown"), 
	_T("Compiled"), 
	_T("Running"), 
	_T("Completed"), 
	_T("Failed"), 
	_T("Archived"), 
	_T("Aborting"), 
	_T("Aborted"), 
	_T("Blocked"), 
	_T("Submitted"), 
	_T("Scheduled"), 
	_T("Compiling"), 
	_T("Waiting"), 
	_T("uploading_files"), 
	_T("Debug Paused"), 
	_T("Debug Running"), 
	_T("ERROR-Please Report"),
};

struct CGraphTiming : public CUnknown
{
public:
	std::_tstring m_name;
	int m_graphNum;
	int m_subGraphNum;
    int m_gid;
    int m_minutes;
    int m_milliseconds;

	IMPLEMENT_CUNKNOWN;
};
typedef StlLinked<CGraphTiming> CGraphTimingAdapt;
typedef std::vector<CGraphTimingAdapt>CGraphTimingVector;

struct CEclException : public CUnknown
{
public:
    CString m_severity;
    int m_code;
    CString m_message;
    CString m_fileName;
    int m_lineNo;
    int m_column;

	IMPLEMENT_CUNKNOWN;
};

typedef StlLinked<CEclException> CEclExceptionAdapt;
typedef std::vector<CEclExceptionAdapt> CEclExceptionVector;
typedef CUnknownT<CEclExceptionVector> CEclExceptionVectorCom;

__interface IWorkunit : public clib::ILockableUnknown
{
	const TCHAR *GetID() const;
	const TCHAR *GetWuid() const;
	const TCHAR *GetDateTime() const;
	const TCHAR *GetScheduledDateTime() const;
	const TCHAR *GetLabel() const;
	const TCHAR *GetOwner() const;
	const TCHAR *GetQueue() const;
	const TCHAR *GetCluster() const;
	WUState GetState() const;
	const TCHAR *GetStateLabel() const;
	const TCHAR *GetEcl() const;
	const TCHAR *GetSnapshot() const;
	bool GetProtected() const;
	bool GetFollowup() const;
	bool GetArchived() const;
	bool IsComplete() const;
	bool IsDebugging() const;
	int GetResultLimit() const;
	int GetMaxRuntime() const;
	const TCHAR *GetDebugString() const;
	bool GetArchiveCpp() const;
	bool GetNoCommonPrivateAttributes() const;

	unsigned int GetResultCount() const;
	std::pair<IResultVector::const_iterator, IResultVector::const_iterator> GetResultIterator() const;
	IResult * GetResult(const std::_tstring & id) const;

	void Refresh(bool noBroadcast);
	void RefreshGraph();	
	unsigned int GetGraphCount() const;	
	unsigned int GetGraphNames(StdStringVector & results) const;
	IGraph * GetGraph(const std::_tstring & graphName) const;
	const TCHAR * GetGraphXGMML(const std::_tstring & graphName, std::_tstring & xgmml) const;
	const TCHAR * GetAllGraphXGMML(std::_tstring & xgmml) const;
	WUVisualState GetVisualState() const;

	void RefreshGraphTiming();
	int GetGraphTimingCount() const;
	const CGraphTiming * GetGraphTiming(unsigned item) const;
	int GetGraphTimings(CGraphTimingVector & results) const;

	int GetExceptionCount() const;
	const CEclException * GetException(unsigned item) const;
	const CEclException * GetCurrException(unsigned lineNo) const;
	const CEclException * GetNextException(unsigned lineNo) const;
	const CEclException * GetPrevException(unsigned lineNo) const;
	int GetExceptions(CEclExceptionVector & results) const;

	void MonitorState();	//Notifications come thru "on_refresh_connect"
	void ForceBroadcast(WUEvent evt = WUEventUpdated);	

	boost::signals::connection on_refresh_connect(const workunit_slot_type& s);
};

typedef StlLinked<IWorkunit> IWorkunitAdapt;
typedef std::vector<IWorkunitAdapt> IWorkunitVector;
typedef CUnknownT<IWorkunitVector> IWorkunitVectorCom;
typedef CComPtr<IWorkunitVectorCom> IWorkunitVectorAdapt;

class IWorkunitCompare
{
public:
	bool operator ()(IWorkunitAdapt & l, IWorkunitAdapt & r)
	{
		int compare = _tcsicmp(l->GetWuid(), r->GetWuid());
		if (compare < 0)
			return true;

		return false;
	}
};

}
