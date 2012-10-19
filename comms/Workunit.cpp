#include "StdAfx.h"

#include "workunit.h"
#include "dali.h"
#include "SoapUtil.h"
#include "clib.h"
#include "cache.h"
#include "logger.h"
#include "result.h"
#include "graph.h"
#include "unicodefile.h"
#include "utilDateTime.h"

#if _COMMS_VER < 68200
using namespace WsWorkunits;
using namespace WsSMC;
#elif _COMMS_VER < 700000
#else
using namespace WsECLWorkunits;
using namespace WsEnvironment;
typedef ECLWUResult ECLResult;
typedef ECLWUDetail ECLWorkunit;
typedef ECLWUException ECLException;
typedef ECLWUApplicationValue ApplicationValue;
typedef ECLWUDebugValue DebugValue;
typedef ActiveOrQueuedWorkunit ActiveWorkunit;
#endif

namespace Dali
{
#if _COMMS_VER < 68200
IResult * CreateResult(const CString & url, const CString & wuid, const ECLResult * result);
IGraph * CreateGraph(const CString& url, const CString & wuid, const ECLGraph * data, int seq);
#else
IResult * CreateResult(const CString & url, const CString & wuid, const ns6__ECLResult * result);
IGraph * CreateGraph(const CString& url, const CString & wuid, const ns6__ECLGraph * data, int seq);
#endif

//  ===========================================================================
const TCHAR * const CEclException::ECL_EXCEPTION_ERROR = _T("Error");
const TCHAR * const CEclException::ECL_EXCEPTION_WARNING = _T("Warning");

CEclException::CEclException()
{
	m_code = 0;
	m_lineNo = -1;
	m_column = -1;
}
//  ===========================================================================
class CWorkunit : public IWorkunit, public clib::CLockableUnknown
{
protected:
	CString m_id;
	CString m_url;
	CString m_daliLabel;
	CString m_label;
	CString m_jobName;
	CString m_dateTime;
	CString m_owner;
	CString m_queue;
	CString m_cluster;
	WUState m_stateID;
	CString m_state;
	CString m_wuid;
	CString m_ecl;
	CString m_snapShot;
	std::_tstring m_schedule;
	bool m_protected;
	bool m_followup;
	bool m_debug;
	bool m_archived;
	int m_resultLimit;
	int m_maxRuntime;
	CString m_debugString;
	bool m_archiveCpp;
	bool m_noCommonPrivateAttributes;
	bool m_resultsLoaded;

	IResultMap m_resultsMap;
	IResultVector m_resultsVector;
	IGraphMap m_graphsMap;
	ILocalFileVector m_localFiles;
	CGraphTimingVector m_graphTimings;
	CEclExceptionVector m_exceptions;
	workunit_signal_type on_refresh;
	int m_filesToUploadCount;

public:
	IMPLEMENT_CLOCKABLEUNKNOWN;

	CWorkunit(const CString & url, const CString daliLabel, const CString & wuid) : m_url(url), m_daliLabel(daliLabel), m_wuid(wuid)
	{
		m_id = m_url + _T("/") + m_daliLabel + _T("/") + m_wuid;
		m_resultLimit = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_WORKUNIT_RESULTLIMIT);
		m_maxRuntime = 0;
		m_stateID = WUStateUnknown;
		m_protected = false;
		m_followup = false;
		m_resultsLoaded = false;
		m_archived = false;
		m_filesToUploadCount = 0;
		m_debug = false;
	}
	virtual ~CWorkunit()
	{
	}
	const TCHAR *GetID() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_id;
	}
	const TCHAR * GetCacheID() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_id;
	}
	const TCHAR *GetWuid() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_wuid;
	}
	const TCHAR *GetDateTime() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_dateTime;
	}
	const TCHAR *GetScheduledDateTime() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_schedule.c_str();
	}
	const TCHAR *GetLabel() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_label;
	}
	const TCHAR *GetOwner() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_owner;
	}
	const TCHAR *GetQueue() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_queue;
	}
	const TCHAR *GetCluster() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_cluster;
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
		return m_state;
	}
	const TCHAR *GetEcl() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_ecl;
	}
	const TCHAR *GetSnapshot() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_snapShot;
	}
	bool GetProtected() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_protected;
	}
	bool GetFollowup() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_followup;
	}
	bool GetArchived() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_archived;
	}
	bool IsComplete() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		if (m_archived)
			return true;

		switch (m_stateID)
		{
		case WUStateCompleted:
		case WUStateFailed:
		case WUStateAborted:
		case WUStateArchived:
			return true;
		}
		return false;
	}
	bool IsDebugging() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		switch (m_stateID)
		{
		case WUStateDebugRunning:
		case WUStateDebugPaused:
			return true;
		}
		return false;
	}
	int GetResultLimit() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_resultLimit;
	}
	int GetMaxRuntime() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_maxRuntime;
	}
	const TCHAR *GetDebugString() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_debugString;
	}
	bool GetArchiveCpp() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_archiveCpp;
	}
	bool GetNoCommonPrivateAttributes() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_noCommonPrivateAttributes;
	}
#if _COMMS_VER < 51300
	void SetResultLimit(int resultLimit) 
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		m_resultLimit = resultLimit;
	}
#endif
	void LoadResults() const
	{
		if (!m_resultsLoaded)
		{
			StlLinked<IDali> server;
			server = AttachDali(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT), _T("Dali"));
			CComPtr<IWorkunit> forceRefresh = server->GetWorkunit(GetWuid(), true);  //no Broadcast otherwise we will deadlock.
		}
	}
	unsigned int GetResultCount() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		LoadResults();
		return m_resultsMap.size();
	}
	std::pair<IResultVector::const_iterator, IResultVector::const_iterator> GetResultIterator() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		LoadResults();
		return std::make_pair(m_resultsVector.begin(), m_resultsVector.end());
	}
	IResult * GetResult(const std::_tstring & id) const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		LoadResults();
		IResultMap::const_iterator itr = m_resultsMap.find(id);
		if (itr != m_resultsMap.end())
		{
			return itr->second;
		}
		return NULL;
	}
	void Refresh(bool noBroadcast) 
	{
		if (!IsComplete())
		{
			StlLinked<IDali> server = AttachDali(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT), _T("Dali"));
			server->GetWorkunit(m_wuid, noBroadcast);
		}
	}
	void RefreshGraph() 
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		StlLinked<IDali> server;
		server = AttachDali(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT), _T("Dali"));
		server->GetGraphs(m_wuid); //Updates via Update call
	}
	unsigned int GetGraphCount() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_graphsMap.size();
	}
	unsigned int GetGraphNames(StdStringVector & results) const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		for (IGraphMap::const_iterator itr = m_graphsMap.begin(); itr != m_graphsMap.end(); ++itr)
			results.push_back(itr->first);
		return results.size();
	}
	IGraph * GetGraph(const std::_tstring & graphName) const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		IGraphMap::const_iterator itr = m_graphsMap.find(graphName);
		if (itr != m_graphsMap.end())
		{
			return itr->second;
		}
		return NULL;
	}
	const TCHAR * GetGraphXGMML(const std::_tstring & graphName, std::_tstring & xgmml) const 
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		std::_tstring xml;
		CComPtr<IGraph> graph = GetGraph(graphName);
		if (graph)
			xgmml = (boost::_tformat(_T("<?xml version=\"1.0\" encoding=\"utf-8\"?><GJS SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:control=\"http://www.w3.org/1999/XMLSchema\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">%1%</GJS>")) % graph->GetGraph()).str();
		return xgmml.c_str();
	}
	const TCHAR * GetAllGraphXGMML(std::_tstring & xgmml) const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		std::_tstring xml;
		StdStringVector names;
		GetGraphNames(names);
		for(StdStringVector::const_iterator itr = names.begin(); itr != names.end(); ++itr)
		{
			CComPtr<IGraph> graph = GetGraph(*itr);
			if (graph)
				xml += graph->GetGraph();
		}
		xgmml = _T("<?xml version=\"1.0\" encoding=\"utf-8\"?><GJS SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:control=\"http://www.w3.org/1999/XMLSchema\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">") + xml + _T("</GJS>");
		return xgmml.c_str();
	}
	WUVisualState GetVisualState() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		WUVisualState retVal = WUVisualState_unknown;
		switch (GetState())
		{
		case WUStateUnknown:
			break;
		case WUStateWait:
		case WUStateCompiling:
		case WUStateCompiled:
		case WUStateRunning:
		case WUStateBlocked:
		case WUStateSubmitted:
		case WUStateScheduled:
		case WUStateWaitingForUpload:
			retVal = WUVisualState_running;
			break;
		case WUStateCompleted:
		case WUStateArchived:
			retVal = WUVisualState_completed;
			break;
		case WUStateFailed:
		case WUStateAborting:
		case WUStateAborted:
			retVal = WUVisualState_failed;
			break;
		}
		return retVal;
	}
	void RefreshGraphTiming() 
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		StlLinked<IDali> server;
		server = AttachDali(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT), _T("Dali"));
		CGraphTimingVector tmp;
		server->GetGraphTimings(m_wuid, tmp);
	}
	int GetGraphTimingCount() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_graphTimings.size();
	}
	const CGraphTiming * GetGraphTiming(unsigned item) const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		ATLASSERT(item >= 0 && item < m_graphTimings.size());
		return m_graphTimings[item].get();
	}
	int GetGraphTimings(CGraphTimingVector & results) const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		results = m_graphTimings;
		return results.size();
	}
	int GetExceptionCount() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_exceptions.size();
	}
	const CEclException * GetException(unsigned item) const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		ATLASSERT(item >= 0 && item < m_exceptions.size());
		return m_exceptions[item].get();
	}
	const CEclException * GetCurrException(unsigned lineNo) const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		for(CEclExceptionVector::size_type i = 0; i < m_exceptions.size(); ++i)
		{
			unsigned int errorLine = (unsigned int)m_exceptions[i]->m_lineNo;
			if (errorLine == lineNo)
				return m_exceptions[i].get();
		}
		return NULL;
	}
	const CEclException * GetNextException(unsigned lineNo) const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		int found = -1;
		unsigned exceptionCount = m_exceptions.size();
		for(unsigned i = 0; i < exceptionCount; ++i)
		{
			unsigned errorLine = m_exceptions[exceptionCount - i - 1]->m_lineNo;
			if (errorLine <= lineNo)
				break;
			found = exceptionCount - i - 1;
		}
		return found == -1 ? NULL : m_exceptions[found].get();
	}
	const CEclException * GetPrevException(unsigned lineNo) const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		int found = -1;
		for(unsigned i = 0; i < m_exceptions.size(); ++i)
		{
			unsigned errorLine = m_exceptions[i]->m_lineNo;
			if (errorLine >= lineNo)
				break;
			found = i;
		}
		return found == -1 ? NULL : m_exceptions[found].get();
	}
	int GetExceptions(CEclExceptionVector & results) const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		for(unsigned i = 0; i < m_exceptions.size(); ++i)
		{
			results.push_back(m_exceptions[i]);
		}
		return results.size();
	}
	void GetLocalFilesToUpload()
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		if (m_filesToUploadCount == 0)
		{
			StlLinked<IDali> server;
			server = AttachDali(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT), _T("Dali"));
			m_localFiles.clear();
			if (server->GetLocalFiles(GetWuid(), m_localFiles) > 0)
				m_filesToUploadCount = m_localFiles.size();
			else
				m_filesToUploadCount = -1;

			UploadFiles();//TODO Should be moved to a GUI thread.
		}
	}
	void UploadFiles()
	{
		for(ILocalFileVector::iterator itr = m_localFiles.begin(); itr != m_localFiles.end(); ++itr)
		{
#define TEXTFILE_ONLY
#ifdef TEXTFILE_ONLY
			std::_tstring rawData;
			CUnicodeFile file;
			if (file.Open(itr->get()->GetSource()))
			{
				file.Read(rawData);
			}
		}
#else
#define BLOCKSIZE 32768
			std::vector<char> rawData;
			CAtlFile file;
			if (file.Create(itr->get()->GetSource(), GENERIC_READ, 0, OPEN_EXISTING) == S_OK)
			{
				ULONGLONG size, pos;
				if (file.GetSize(size) == S_OK && file.GetPosition(pos) == S_OK)
				{
					size -= pos;
					DWORD bytesRead;
					rawData.resize(static_cast<std::_tstring::size_type>(size));
					for(int i = 0; i < size; i += BLOCKSIZE)
					{
						unsigned int readSize = rawData.size() - i;
						if (readSize > BLOCKSIZE)
							readSize = BLOCKSIZE;
						file.Read(&rawData[0] + i, readSize, bytesRead);
						ATLASSERT(bytesRead == readSize);
					}
				}
			}
		}
#endif
	}
#if _COMMS_VER < 68200
	void Update(const ECLWorkunit * c, bool resultsFetched, bool noBroadcast)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
#if _COMMS_VER < 700000
		m_jobName = CW2T(c->Jobname, CP_UTF8);
		m_wuid = CW2T(c->Wuid, CP_UTF8);
		m_owner = CW2T(c->Owner, CP_UTF8);
		m_queue = CW2T(c->Queue, CP_UTF8);
		m_cluster = CW2T(c->Cluster, CP_UTF8);
		m_stateID = static_cast<WUState>(c->StateID);
		m_state = CW2T(c->State, CP_UTF8);
		if (c->Query.Text)
			m_ecl = CW2T(c->Query.Text, CP_UTF8);
		if (c->DateTimeScheduled)
		{
			m_schedule = CW2T(c->DateTimeScheduled, CP_UTF8);
		}
#if _COMMS_VER < 50515
#else
		m_snapShot = CW2T(c->Snapshot, CP_UTF8);
#endif
		m_protected = c->Protected;
#if _COMMS_VER < 51300
#else 
		m_resultLimit = c->ResultLimit;
#endif
#else
		m_wuid = CW2T(c->WUID, CP_UTF8);
		m_jobName = CW2T(c->Attributes.JobName, CP_UTF8);
		m_owner = CW2T(c->Attributes.Owner, CP_UTF8);
		m_queue = CW2T(c->Attributes.Queue, CP_UTF8);
		m_cluster = CW2T(c->Attributes.Cluster, CP_UTF8);
		ATLASSERT(!_T("TODO"));
		//m_stateID = static_cast<WUState>(c->Attributes.StateID);
		m_state = CW2T(c->Status.State, CP_UTF8);
		if (c->Query.ECLText)
			m_ecl = CW2T(c->Query.ECLText, CP_UTF8);
		if (c->Attributes.DateTimeScheduled)
		{
			m_schedule = CW2T(c->Attributes.DateTimeScheduled, CP_UTF8);
		}
		m_snapShot = CW2T(c->Attributes.Snapshot, CP_UTF8);
		m_protected = c->Status.IsProtected;
		m_resultLimit = c->Attributes.ResultLimit;
#endif
		if (m_jobName.GetLength())
			m_label = m_jobName + _T(" (") + m_wuid + _T(")");
		else
			m_label = m_wuid;
		m_dateTime = m_wuid.Mid(1, 4) + _T("-") + m_wuid.Mid(5, 2) + _T("-") + m_wuid.Mid(7, 2) + _T(" ") + m_wuid.Mid(10, 2) + _T(":") + m_wuid.Mid(12, 2) + _T(":") + m_wuid.Mid(14, 2);
		if (resultsFetched)
		{
			m_resultsLoaded = true;
			m_resultsMap.clear();
			m_resultsVector.clear();
			ECLResult *ECLResult3 = c->Results;
			int __ECLResult3_nSizeIs = c->__Results_nSizeIs;
			for(int i = 0; i < __ECLResult3_nSizeIs; ++i)
			{
				StlLinked<IResult> r = CreateResult(m_url, m_wuid, &ECLResult3[i]);
				m_resultsMap[r->GetID()] = r;
				m_resultsVector.push_back(r);
			}
		}

		ECLException *ECLException0 = c->Exceptions;
		int __ECLException0_nSizeIs = c->__Exceptions_nSizeIs;
		m_exceptions.clear();
		for(int i = 0; i < __ECLException0_nSizeIs; ++i)
		{
			StlLinked<CEclException> error;
			error = new CEclException();
#if _COMMS_VER < 499
#else
			error->m_fileName = CW2T(ECLException0[i].FileName, CP_UTF8);
#endif
			error->m_severity = CW2T(ECLException0[i].Severity, CP_UTF8);
#if _COMMS_VER < 700000
			error->m_code = ECLException0[i].Code;
			error->m_message = CW2T(ECLException0[i].Message, CP_UTF8);
			error->m_lineNo = ECLException0[i].LineNo;
#else
			error->m_code = ECLException0[i].ExceptionCode;
			error->m_message = CW2T(ECLException0[i].ExceptionMessage, CP_UTF8);
			error->m_lineNo = ECLException0[i].Line;
#endif
			error->m_column = ECLException0[i].Column;
			m_exceptions.push_back(error);
		}
		m_debugString = _T("");
		m_followup = false;
		m_archiveCpp = false;
		m_noCommonPrivateAttributes = false;
		std::_tstring followup_key = APP_DATA_FOLLOWUP;
		followup_key += CString((GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_USER)));
		ApplicationValue *ApplicationValue6 = c->ApplicationValues;
		int __ApplicationValue6_nSizeIs = c->__ApplicationValues_nSizeIs;
		for(int i = 0; i < __ApplicationValue6_nSizeIs; ++i)
		{
			std::_tstring app = CW2T(ApplicationValue6[i].Application, CP_UTF8);
#if _COMMS_VER < 700000
			std::_tstring key = CW2T(ApplicationValue6[i].Name, CP_UTF8);
			std::_tstring val = CW2T(ApplicationValue6[i].Value, CP_UTF8);
#else
			std::_tstring key = CW2T(ApplicationValue6[i].KeyName, CP_UTF8);
			std::_tstring val = CW2T(ApplicationValue6[i].KeyValue, CP_UTF8);
#endif
			if (app.compare(APP_DATA_APPNAME) == 0)
			{
				if (boost::algorithm::starts_with(key, APP_DATA_DEBUG))
				{
					std::_tstring stripped_key = key.substr(_tcslen(APP_DATA_DEBUG));
					if (boost::algorithm::iequals(stripped_key, APP_DATA_ARCHIVE) && boost::algorithm::iequals(val, _T("1")))
						m_archiveCpp = true;
					else if (boost::algorithm::iequals(stripped_key, APP_DATA_COMMONPRIVATEATTRIBUTES) && boost::algorithm::iequals(val, _T("0")))
						m_noCommonPrivateAttributes = true;
					else if (boost::algorithm::iequals(stripped_key, APP_DATA_DEBUGDEBUG))
						m_debug = true;
					else if (stripped_key.length())
						m_debugString += std::_tstring((boost::_tformat(_T("%1%=%2%\r\n")) % stripped_key % val).str()).c_str();
				}
				else if (followup_key.compare(key) == 0)
					m_protected = true;
			}
		}
		m_maxRuntime = 0;
		DebugValue *DebugValue5 = c->DebugValues;
		int __DebugValue5_nSizeIs = c->__DebugValues_nSizeIs;
		for(int i = 0; i < __DebugValue5_nSizeIs; ++i)
		{
			CString key = DebugValue5[i].Name;
			CString val = DebugValue5[i].Value;
#if _COMMS_VER < 51300
			if (key.CompareNoCase(RESULTLIMIT_KEY) == 0)
			{
				try
				{
					SetResultLimit(boost::lexical_cast<int>(static_cast<const TCHAR *>(val)));
				}
				catch(boost::bad_lexical_cast)
				{
				}
			}
#endif
			if (key.CompareNoCase(MAXRUNTIME_KEY) == 0)
			{
				try
				{
					m_maxRuntime = boost::lexical_cast<int>(static_cast<const TCHAR *>(val));
				}
				catch(boost::bad_lexical_cast)
				{
				}
			}
		}
		ECLGraph * graphs = c->Graphs;
		int __Graphs_nSizeIs = c->__Graphs_nSizeIs;
		for(int i = 0; i < __Graphs_nSizeIs; ++i)
		{
			m_graphsMap[graphs[i].Name] = CreateGraph(m_url, m_wuid, &graphs[i], i+1);
		}
		if (m_stateID == WUStateWaitingForUpload)
		{
			GetLocalFilesToUpload();
		}
		if (!noBroadcast)
		{
			proc.unlock();
			Broadcast();
		}
	}
#else
	void Update(const ns6__ECLWorkunit * c, bool noBroadcast)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		SAFE_ASSIGN2CSTRING(m_jobName, c->Jobname);
		SAFE_ASSIGN2CSTRING(m_wuid, c->Wuid);
		SAFE_ASSIGN2CSTRING(m_owner, c->Owner);
		SAFE_ASSIGN2CSTRING(m_queue, c->Queue);
		SAFE_ASSIGN2CSTRING(m_cluster, c->Cluster);
		if (c->StateID)
			m_stateID = static_cast<WUState>(*c->StateID);
		SAFE_ASSIGN2CSTRING(m_state, c->State);
		if (c->Query)
			SAFE_ASSIGN2CSTRING(m_ecl, c->Query->Text);
		if (c->DateTimeScheduled)
			TimeTToUTC(*c->DateTimeScheduled, m_schedule);
		SAFE_ASSIGN2CSTRING(m_snapShot, c->Snapshot);
		SAFE_ASSIGN(m_protected, c->Protected);
		SAFE_ASSIGN(m_resultLimit, c->ResultLimit);
		SAFE_ASSIGN(m_archived, c->Archived);
		if (m_jobName.GetLength())
			m_label = m_jobName + _T(" (") + m_wuid + _T(")");
		else
			m_label = m_wuid;
		m_dateTime = m_wuid.Mid(1, 4) + _T("-") + m_wuid.Mid(5, 2) + _T("-") + m_wuid.Mid(7, 2) + _T(" ") + m_wuid.Mid(10, 2) + _T(":") + m_wuid.Mid(12, 2) + _T(":") + m_wuid.Mid(14, 2);
		if (c->Results)
		{
			m_resultsLoaded = true;
			m_resultsMap.clear();
			m_resultsVector.clear();
			for(std::size_t i = 0; i < c->Results->ECLResult.size(); ++i)
			{
				StlLinked<IResult> r = CreateResult(m_url, m_wuid, c->Results->ECLResult[i]);
				m_resultsMap[r->GetID()] = r;
				m_resultsVector.push_back(r);
			}
		}

		if (c->Exceptions)
		{
			m_exceptions.clear();
			for(std::size_t i = 0; i < c->Exceptions->ECLException.size(); ++i)
			{
				StlLinked<CEclException> error;
				error = new CEclException();
				SAFE_ASSIGN2CSTRING(error->m_fileName, c->Exceptions->ECLException[i]->FileName);
				if (error->m_fileName.CompareNoCase(_T("stdin")) == 0)
					error->m_fileName = _T("");
				SAFE_ASSIGN2CSTRING(error->m_severity, c->Exceptions->ECLException[i]->Severity);
				SAFE_ASSIGN(error->m_code, c->Exceptions->ECLException[i]->Code);
				SAFE_ASSIGN2CSTRING(error->m_message, c->Exceptions->ECLException[i]->Message);
				SAFE_ASSIGN(error->m_lineNo, c->Exceptions->ECLException[i]->LineNo);
				SAFE_ASSIGN(error->m_column, c->Exceptions->ECLException[i]->Column);
				m_exceptions.push_back(error);
			}
		}
		m_debugString = _T("");
		m_followup = false;
		m_archiveCpp = false;
		m_noCommonPrivateAttributes = false;
		if (c->ApplicationValues)
		{
			std::_tstring followup_key = APP_DATA_FOLLOWUP;
			followup_key += CString((GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_USER)));
			for(std::size_t i = 0; i < c->ApplicationValues->ApplicationValue.size(); ++i)
			{
				std::_tstring app, key, val;
				SAFE_ASSIGN(app, c->ApplicationValues->ApplicationValue[i]->Application);
				SAFE_ASSIGN(key, c->ApplicationValues->ApplicationValue[i]->Name);
				SAFE_ASSIGN(val, c->ApplicationValues->ApplicationValue[i]->Value);
				if (app.compare(APP_DATA_APPNAME) == 0)
				{
					if (boost::algorithm::starts_with(key, APP_DATA_DEBUG))
					{
						std::_tstring stripped_key = key.substr(_tcslen(APP_DATA_DEBUG));
						if (boost::algorithm::iequals(stripped_key, APP_DATA_ARCHIVE) && boost::algorithm::iequals(val, _T("1")))
							m_archiveCpp = true;
						else if (boost::algorithm::iequals(stripped_key, APP_DATA_COMMONPRIVATEATTRIBUTES) && boost::algorithm::iequals(val, _T("0")))
							m_noCommonPrivateAttributes = true;
						else if (boost::algorithm::iequals(stripped_key, APP_DATA_DEBUGDEBUG))
							m_debug = true;
						else if (stripped_key.length())
							m_debugString += std::_tstring((boost::_tformat(_T("%1%=%2%\r\n")) % stripped_key % val).str()).c_str();
					}
					else if (followup_key.compare(key) == 0)
						m_protected = true;
				}
			}
		}
		m_maxRuntime = 0;
		if (c->DebugValues)
		{
			for(std::size_t i = 0; i < c->DebugValues->DebugValue.size(); ++i)
			{
				CString key, val;
				SAFE_ASSIGN2CSTRING(key, c->DebugValues->DebugValue[i]->Name);
				SAFE_ASSIGN2CSTRING(val, c->DebugValues->DebugValue[i]->Value);
				if (key.CompareNoCase(MAXRUNTIME_KEY) == 0)
				{
					try
					{
						m_maxRuntime = boost::lexical_cast<int>(static_cast<const TCHAR *>(val));
					}
					catch(boost::bad_lexical_cast)
					{
					}
				}
			}
		}
		if (c->Graphs)
		{
			for(std::size_t i = 0; i < c->Graphs->ECLGraph.size(); ++i)
			{
				m_graphsMap[*c->Graphs->ECLGraph[i]->Name] = CreateGraph(m_url, m_wuid, c->Graphs->ECLGraph[i], i+1);
			}
		}
		if (m_stateID == WUStateWaitingForUpload)
		{
			GetLocalFilesToUpload();
		}
		if (!noBroadcast)
		{
			proc.unlock();
			Broadcast();
		}
	}
#endif
#if _COMMS_VER < 68200
	void Update(const ActiveWorkunit * c, bool noBroadcast)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
#if (_COMMS_VER < 53306)
#elif _COMMS_VER < 700000
		m_jobName = CW2T(c->Jobname, CP_UTF8);
		m_wuid = CW2T(c->Wuid, CP_UTF8);
		m_stateID = static_cast<WUState>(c->StateID);
#else
		m_jobName = CW2T(c->JobName, CP_UTF8);
		m_wuid = CW2T(c->WUID, CP_UTF8);
		ATLASSERT(!_T("TODO"));
		//m_stateID = static_cast<WUState>(c->Attributes.StateID);
#endif
		if (m_jobName.GetLength())
			m_label = m_jobName + _T(" (") + m_wuid + _T(")");
		else
			m_label = m_wuid;
		m_dateTime = m_wuid.Mid(1, 4) + _T("-") + m_wuid.Mid(5, 2) + _T("-") + m_wuid.Mid(7, 2) + _T(" ") + m_wuid.Mid(10, 2) + _T(":") + m_wuid.Mid(12, 2) + _T(":") + m_wuid.Mid(14, 2);
		m_owner = CW2T(c->Owner, CP_UTF8);
		m_state = CW2T(c->State, CP_UTF8);

		if (m_stateID == WUStateWaitingForUpload)
		{
			GetLocalFilesToUpload();
		}
		if (!noBroadcast)
		{
			proc.unlock();
			Broadcast();
		}
	}

	void Update(ECLTimingData *TimingData, int __TimingData_nSizeIs)
	{
		m_graphTimings.clear();
		while(--__TimingData_nSizeIs >= 0)
		{
			StlLinked<CGraphTiming> gt = new CGraphTiming();
			gt->m_name = TimingData->Name;
			gt->m_graphNum = TimingData->GraphNum;
			gt->m_subGraphNum = TimingData->SubGraphNum;
			gt->m_gid = TimingData->GID;
			gt->m_minutes = TimingData->Min;
			gt->m_milliseconds = TimingData->MS;
			m_graphTimings.push_back(gt);
			++TimingData;
		}
	}

	void Update(ECLGraph *Graphs, int __Graphs_nSizeIs)
	{
		int i = 0;
		while(--__Graphs_nSizeIs >= 0)
		{
			std::_tstring name = CW2T(Graphs->Name, CP_UTF8);
			m_graphsMap[name] = CreateGraph(m_url, m_wuid, Graphs, i+1);
			++Graphs;
			++i;
		}
	}
#else
	void Update(const ns4__ActiveWorkunit * c, bool noBroadcast)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		if (c->Jobname)
			m_jobName = c->Jobname->c_str();
		if (c->Wuid)
			m_wuid = c->Wuid->c_str();
		if (c->StateID)
			m_stateID = static_cast<WUState>(*c->StateID);
		if (m_jobName.GetLength())
			m_label = m_jobName + _T(" (") + m_wuid + _T(")");
		else
			m_label = m_wuid;
		m_dateTime = m_wuid.Mid(1, 4) + _T("-") + m_wuid.Mid(5, 2) + _T("-") + m_wuid.Mid(7, 2) + _T(" ") + m_wuid.Mid(10, 2) + _T(":") + m_wuid.Mid(12, 2) + _T(":") + m_wuid.Mid(14, 2);
		if (c->Owner)
			m_owner = c->Owner->c_str();
		if (c->State)
			m_state = c->State->c_str();

		if (m_stateID == WUStateWaitingForUpload)
		{
			GetLocalFilesToUpload();
		}
		if (!noBroadcast)
		{
			proc.unlock();
			Broadcast();
		}
	}

	void Update(ns6__ArrayOfECLTimingData *TimingData)
	{
		if (TimingData)
		{
			m_graphTimings.clear();
			for(std::size_t i = 0; i < TimingData->ECLTimingData.size(); ++i)
			{
				StlLinked<CGraphTiming> gt = new CGraphTiming();
				SAFE_ASSIGN(gt->m_name, TimingData->ECLTimingData[i]->Name);
				SAFE_ASSIGN(gt->m_graphNum, TimingData->ECLTimingData[i]->GraphNum);
				SAFE_ASSIGN(gt->m_subGraphNum, TimingData->ECLTimingData[i]->SubGraphNum);
				SAFE_ASSIGN(gt->m_gid, TimingData->ECLTimingData[i]->GID);
				SAFE_ASSIGN(gt->m_minutes, TimingData->ECLTimingData[i]->Min);
				SAFE_ASSIGN(gt->m_milliseconds, TimingData->ECLTimingData[i]->MS);
				m_graphTimings.push_back(gt);
			}
		}
	}

	void Update(ns6__ArrayOfECLGraph *Graphs)
	{
		if (Graphs)
		{
			for(std::size_t i = 0; i < Graphs->ECLGraph.size(); ++i)
			{
				m_graphsMap[*Graphs->ECLGraph[i]->Name] = CreateGraph(m_url, m_wuid, Graphs->ECLGraph[i], i+1);
			}
		}
	}
#endif
	void Broadcast(WUEvent evt = WUEventUpdated) 
	{
		on_refresh(evt, this);
	}

	void MonitorState() 
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		if (IsComplete())
		{
			proc.unlock();
			Broadcast();
			return;
		}
		StlLinked<IDali> server;
		server = AttachDali(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT), _T("Dali"));
		server->MonitorState(GetWuid());
	}

	void ForceBroadcast(WUEvent evt)
	{
		//  No lock here please...  
		Broadcast(evt);
	}

	boost::signals::connection on_refresh_connect(const workunit_slot_type& s)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return on_refresh.connect(s); 
	}
};
//  ===========================================================================
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

IWorkunit * GetWorkunit(const CServerConfig& config, const CString & wuid)
{
	CComPtr<CWorkunit> wu = new CWorkunit(config.m_url, config.m_label, wuid);
	return WorkunitCache.Exists(wu->GetCacheID());
}

CWorkunit * CreateWorkunitRaw(const CServerConfig& config, const CString & wuid)
{
	CWorkunit *wu = WorkunitCache.Get(new CWorkunit(config.m_url, config.m_label, wuid));
	//ATLTRACE(_T("Workunit cache(size=%u)\r\n"), WorkunitCache.Size());
	return wu;
}

#if _COMMS_VER < 68200
IWorkunit * CreateWorkunit(const CServerConfig& config, const ECLWorkunit * data, bool resultsFetched, bool noBroadcast)
{
#if _COMMS_VER < 700000
	CWorkunit * wu = CreateWorkunitRaw(config, data->Wuid);
#else
	CWorkunit * wu = CreateWorkunitRaw(config, data->WUID);
#endif
	ATLASSERT(wu);
	wu->Update(data, resultsFetched, noBroadcast);
	return wu;
}
IWorkunit * CreateWorkunit(const CServerConfig& config, const CString & wuid, ECLTimingData *TimingData, int __TimingData_nSizeIs)
{
#if _COMMS_VER < 700000
	CWorkunit * wu = CreateWorkunitRaw(config, wuid);
#else
	CWorkunit * wu = CreateWorkunitRaw(config, wuid);
#endif
	ATLASSERT(wu);
	wu->Update(TimingData, __TimingData_nSizeIs);
	return wu;
}
IWorkunit * CreateWorkunit(const CServerConfig& config, const CString & wuid, ECLGraph *Graphs, int __Graphs_nSizeIs)
{
#if _COMMS_VER < 700000
	CWorkunit * wu = CreateWorkunitRaw(config, wuid);
#else
	CWorkunit * wu = CreateWorkunitRaw(config, wuid);
#endif
	ATLASSERT(wu);
	wu->Update(Graphs, __Graphs_nSizeIs);
	return wu;
}
IWorkunit * CreateWorkunit(const CString & url, const CString & daliLabel, const ActiveWorkunit *data, bool noBroadcast)
{
#if _COMMS_VER < 700000
	CWorkunit *wu = WorkunitCache.Get(new CWorkunit(url, daliLabel, data->Wuid));
#else
	CWorkunit *wu = WorkunitCache.Get(new CWorkunit(url, daliLabel, data->WUID));
#endif
	ATLASSERT(wu);
	wu->Update(data, noBroadcast);
	return wu;
}
#else
IWorkunit * CreateWorkunit(const CServerConfig& config, const ns6__ECLWorkunit * data, bool noBroadcast)
{
	CWorkunit * wu = CreateWorkunitRaw(config, data->Wuid->c_str());
	ATLASSERT(wu);
	wu->Update(data, noBroadcast);
	return wu;
}
IWorkunit * CreateWorkunit(const CServerConfig& config, const CString & wuid, ns6__ArrayOfECLTimingData *TimingData)
{
	CWorkunit * wu = CreateWorkunitRaw(config, wuid);
	ATLASSERT(wu);
	wu->Update(TimingData);
	return wu;
}
IWorkunit * CreateWorkunit(const CServerConfig& config, const CString & wuid, ns6__ArrayOfECLGraph *Graphs)
{
	CWorkunit * wu = CreateWorkunitRaw(config, wuid);
	ATLASSERT(wu);
	wu->Update(Graphs);
	return wu;
}
IWorkunit * CreateWorkunit(const CString & url, const CString & daliLabel, const ns4__ActiveWorkunit *data, bool noBroadcast)
{
	CWorkunit *wu = WorkunitCache.Get(new CWorkunit(url, daliLabel, data->Wuid->c_str()));
	ATLASSERT(wu);
	wu->Update(data, noBroadcast);
	return wu;
}
#endif
IWorkunit * CreateWorkunit(const CString & url, const CString & daliLabel, const CString & wuid)
{
	return CreateWorkunitRaw(CServerConfig(url, daliLabel), wuid);
}
IWorkunit * GetCachedWorkunit(const CServerConfig& config, const CString & wuid)
{
	CComPtr<CWorkunit> wu = new CWorkunit(config.m_url, config.m_label, wuid);
	return WorkunitCache.Exists(wu->GetCacheID());
}
}
