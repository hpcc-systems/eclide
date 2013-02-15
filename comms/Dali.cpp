#include "StdAfx.h"
#include "dali.h"

#include "cache.h"
#include "Result.h"
#include "gSoapUtil.h"
#include "RecursiveMutex.h"
#include "logger.h"
#include "thread.h"
#include "SAXHandlers.h"
#include "utilDateTime.h"
#include "EclCC.h"
#include "LocalWorkunit.h"

const SectionLabelDefault GLOBAL_SERVER_WORKUNIT(SectionLabel(_T("Server"), _T("Workunit")), _T(""));
const SectionLabelDefault GLOBAL_CLUSTER(SectionLabel(_T("Server"), _T("Cluster")), _T("thor"));
const SectionLabelDefault GLOBAL_GROUP(SectionLabel(_T("Server"), _T("Group")), _T(""));
const SectionLabelDefault GLOBAL_QUEUE(SectionLabel(_T("Server"), _T("Queue")), _T(""));
const SectionLabelDefault GLOBAL_WORKUNIT_RESULTLIMIT(SectionLabel(_T("Workunit"), _T("ResultLimit")), 100);

namespace Dali
{
void ClearWorkunitCache(const std::_tstring & id);
void ClearWorkunitCache();
void ClearLocalWorkunitCache();
//void Topology::ClearClusterCache();
void ClearXGMMLWorkunitCache(const std::_tstring & id);
void ClearXGMMLWorkunitCache();
void ClearResultCache();
void ClearLocalResultCache();

#if _COMMS_VER >= 68200
#define ESP_EXCEPTION_LOG3(T) CReportingGSoapEspException<ns6__ArrayOfEspException> exceptions(T, __FILE__, __LINE__)

ILocalFile * CreateLocalFile(const CString & url, const CString & wuid, const ns6__LogicalFileUpload * localfile);
IWorkunit * CreateWorkunit(const CServerConfig& config, const ns6__ECLWorkunit * data, bool noBroadcast);
IWorkunit * CreateWorkunit(const CServerConfig& config, const CString & wuid, ns6__ArrayOfECLTimingData *TimingData);
IWorkunit * CreateWorkunit(const CServerConfig& config, const CString & wuid, ns6__ArrayOfECLGraph *Graphs);
IWorkunit * GetCachedWorkunit(const CServerConfig& config, const CString & wuid);
IWorkunit * CreateXGMMLWorkunit(const CString & filePath, const CString & daliLabel, const CString & wuid);
IWorkunit * CreateLocalWorkunit(const CString & filePath, const CString & wuid = _T(""));
bool UpdateLocalWorkunit(IWorkunit * wu, const std::_tstring & wuXml, const std::_tstring & result, const Dali::CEclExceptionVector & errors);
bool UpdateLocalWorkunit(IWorkunit * wu, WUState state);
IWorkunit * GetCachedLocalWorkunit(const CString & filePath, const CString & wuid);
IGraph * CreateGraph(const CString& url, const CString & wuid, const ns6__ECLGraphEx * data);

CServerConfig::CServerConfig( const TCHAR* url, const TCHAR* label)
{
	if ( url )
		m_url = url;
	if ( label )
		m_label = label;
}
CServerConfig::CServerConfig(const CServerConfig& other)
{
	*this = other;
}

CString& CServerConfig::GetUrl(CString& url) const
{
	if ( m_url.IsEmpty() )
		url = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_ATTRIBUTE);
	else
		url = m_url;
	return url;
}
CString& CServerConfig::GetUserId(CString& userId) const
{
	if ( m_userId.IsEmpty() )
		userId = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_USER);
	else
		userId = m_userId;
	return userId;
}
CString& CServerConfig::GetPassword(CString& password) const
{
	if ( m_password.IsEmpty() )
		password = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_PASSWORD);
	else
		password = m_password;
	return password;
}

template<typename T>
static void InitDaliServer(const CServerConfig& config, T& server)
{
	CString url,userId,password;
	server.SetUrl(config.GetUrl(url));
	server.SetPassword(config.GetPassword(password));
	server.SetUser(config.GetUserId(userId));
}

class CStringBool : public clib::CLockableUnknown
{
protected:
	typedef std::map<std::_tstring, bool> MonitorMap;
	MonitorMap m_monitor;

public:
	void Clear()
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		m_monitor.clear();
	}
	void Clear(const std::_tstring & wuid)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		m_monitor.erase(wuid);
		ATLTRACE(_T("WU Monitor size(%u)\r\n"), m_monitor.size());
	}
	bool Start(const std::_tstring & wuid)	//returns false if wuid is already being monitored.
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		if (m_monitor[wuid] == true)
			return false;
		m_monitor[wuid] = true;
		return true;
	}
	void Stop(const std::_tstring & wuid)	
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		//set it to false only if found (don't create the map entry)
		MonitorMap::iterator f = m_monitor.find(wuid);
		if ( f != m_monitor.end() )
			f->second = false;
	}
	bool Active(const std::_tstring & wuid)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		MonitorMap::iterator f = m_monitor.find(wuid);
		return( f != m_monitor.end() && f->second == true );
	}
} g_monitor;

#define MAX_RESULTDEPTH 5
class ResultDepth
{
public:
	static LONG value;
	ResultDepth()
	{
		::InterlockedIncrement(&value);
	}
	~ResultDepth()
	{
		::InterlockedDecrement(&value);
	}
};
LONG ResultDepth::value = 0;

signed GetResultData(const CServerConfig& config, const CString & pWuid, int sequence, __int64 start, int count, ITable * result, __int64 &total) 
{
	CString url, user, password;
	CSoapInitialize<WsWorkunitsServiceSoapProxy> server(config.GetUrl(url), config.GetUserId(user), config.GetPassword(password));
	CStringPool stringPool;

	_ns6__WUResult request;
	request.Wuid = stringPool.Create(pWuid);
	request.Sequence = &sequence;
	request.Start = &start;
	request.Count = &count;

	_ns6__WUResultResponse response;
	ESP_EXCEPTION_LOG3(response.Exceptions);
	if (server.WUResult(&request, &response) == SOAP_OK)
	{
		SAFE_ASSIGN(total, response.Total);
		if (response.Result)
		{
			if (response.Count > 0)
			{
				CComBSTR xml = L"<Result>";
				xml += response.Result->c_str();
				xml += L"</Result>";
	#ifdef _DEBUG
				if (static_cast<bool>(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_DEBUG_LOGRESULTXML))==true)
				{
					std::_tstring name;
					SAFE_ASSIGN(name, response.Name);
					std::ofstream file((boost::format("c:\\temp\\resultlog_%1%_%2%_%3%.xml") % CT2A(name.c_str(), CP_UTF8) % start % count).str().c_str());
					file << CW2A(xml);
				}
	#endif
				if (result->LoadFrom(xml, start))
					return count;
			}
		}
		return 0;
	}
	else
		_DBGLOG(config.GetUrl(url), LEVEL_WARNING, server.GetClientErrorMsg());

	total = -1;
	count = -1;
	return -1;
}

COMMS_API void GetResultData(const CServerConfig& config, CResultDataRequestAdapt req)
{
	async_datacomplete_signal_type on_completion;
	on_completion.connect(req->m_slot);
	req->m_request_sent = true;
	req->m_count = req->m_result->GetResultData(req->m_start, req->m_count, req->m_resultTable);
	on_completion(req->m_start, req->m_count, req->m_result->GetTotal(), true);
}

int ParseDebugString(const CString & debugString, bool archive, bool noCommonPrivateAttributes, bool debug, CStringPool & stringPool, ns6__ArrayOfDebugValue & arrayOfDebugValue, CStructPool<ns6__DebugValue> & debugValuePool, ns6__ArrayOfApplicationValue & arrayOfApplicationValue, CStructPool<ns6__ApplicationValue> & applicationValuePool)
{
	std::_tstring debugStr;
	if (debug)
	{
		debugStr += _T("Debug=1\r\n");
	}
	if (archive)
	{
		debugStr += std::_tstring(APP_DATA_ARCHIVE) + _T("=1\r\n");
	}
	if (noCommonPrivateAttributes)
	{
		debugStr += std::_tstring(APP_DATA_COMMONPRIVATEATTRIBUTES) + _T("=0\r\n");
	}
	debugStr += static_cast<const TCHAR *>(debugString);
	boost::algorithm::trim_if(debugStr, boost::algorithm::is_any_of("\r\n"));

	typedef std::vector<std::_tstring> split_vector_type;
	split_vector_type SplitVec; 
	boost::algorithm::split(SplitVec, debugStr, boost::algorithm::is_any_of("\r\n"), boost::algorithm::token_compress_on);

	int i = 0;
	for(split_vector_type::iterator itr = SplitVec.begin(); itr != SplitVec.end(); ++itr)
	{
		std::_tstring lhs, rhs;
		split_vector_type LineVec; 
		boost::algorithm::split(LineVec, *itr, boost::algorithm::is_any_of("="), boost::algorithm::token_compress_on);
		split_vector_type::iterator l_itr = LineVec.begin(); 
		if (l_itr != LineVec.end())
		{
			lhs = *l_itr;
			++l_itr;
		}
		if (l_itr != LineVec.end())
		{
			rhs = *l_itr;
		}
		ns6__DebugValue debugValue;
		debugValue.Name = stringPool.Create(lhs);
		debugValue.Value = stringPool.Create(rhs);
		arrayOfDebugValue.DebugValue.push_back(debugValuePool.Create(debugValue));

		lhs = APP_DATA_DEBUG + lhs;
		ns6__ApplicationValue applicationValue;
		applicationValue.Application = stringPool.Create(APP_DATA_APPNAME);
		applicationValue.Name = stringPool.Create(lhs);
		applicationValue.Value = stringPool.Create(rhs);
		arrayOfApplicationValue.ApplicationValue.push_back(applicationValuePool.Create(applicationValue));
		++i;
	}
	return i;
}

class RoxieXMLHandler : public StackHandlerImpl
{
public:
	std::_tstring m_queryName;

	virtual HRESULT STDMETHODCALLTYPE startElement(
		const wchar_t *pwchNamespaceUri,
		int cchNamespaceUri,
		const wchar_t *pwchLocalName,
		int cchLocalName,
		const wchar_t *pwchQName,
		int cchQName,
		ISAXAttributes *pAttributes)
	{
		__super::startElement(pwchNamespaceUri, cchNamespaceUri, pwchLocalName, cchLocalName, pwchQName, cchQName, pAttributes);
		if (m_queryName.length() == 0)
		{
			const Element & e = m_stack.top();
			m_queryName = e.m_tag;
		}
		return S_OK;
	}
};

BSTR * CreateBstrArray(const StdStringVector &strArr)
{
	unsigned arrSize = strArr.size();
	BSTR *arr = (BSTR *)malloc(arrSize * sizeof(BSTR));
	for (unsigned i = 0; i < arrSize; ++i)
	{
		CString c(strArr[i].c_str());
		arr[i] = c.AllocSysString();
	}
	return arr;
}

BSTR * CreateBstrArray(IWorkunitVector *workunits)
{
	unsigned arrSize = workunits->size();
	BSTR *arr = (BSTR *)malloc(arrSize * sizeof(BSTR));
	for (unsigned i = 0; i < arrSize; ++i)
		arr[i] = ::SysAllocString(CT2W(workunits->operator [](i)->GetWuid(), CP_UTF8));
	return arr;
}

void DestroyBstrArray(BSTR * arr, unsigned arrSize)
{
	for (unsigned i = 0; i < arrSize; ++i)
		SysFreeString(arr[i]);
	free(arr);
}

bool ProtectWorkunits(const CString & url, IWorkunitVector *workunits, bool protect)
{
	bool retVal = false;

	CSoapInitialize<WsWorkunitsServiceSoapProxy> server(url);
	CStringPool stringPool;

	_ns6__WUProtect request;
	ns6__EspStringArray espStringArray;
	request.Wuids = &espStringArray;
	for (unsigned i = 0; i < workunits->size(); ++i)
		espStringArray.Item.push_back(stringPool.Create((*workunits)[i]->GetWuid()));

	_ns6__WUProtectResponse response;
	ESP_EXCEPTION_LOG3(response.Exceptions);
	if (server.WUProtect(&request, &response) == SOAP_OK)
	{
		retVal = true;
	}
	else
		_DBGLOG(url, LEVEL_WARNING, server.GetClientErrorMsg());

	return retVal;
}

bool FollowupWorkunits(const CString & url, IWorkunitVector *workunits, bool followup)
{
	bool retVal = false;
	for(IWorkunitVector::iterator itr = workunits->begin(); itr != workunits->end(); ++itr)
	{
		ATLASSERT(url.CompareNoCase(CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT))) == 0);
		CSoapInitialize<WsWorkunitsServiceSoapProxy> server(url);
		CStringPool stringPool;
		CStructPool<ns6__ApplicationValue> applicationValuePool;

		_ns6__WUUpdate request;
		request.Wuid = stringPool.Create(itr->get()->GetWuid());

		CComBSTR wuid = CT2W(itr->get()->GetWuid(), CP_UTF8);
		ns6__ArrayOfApplicationValue arrayOfApplicationValue;
		request.ApplicationValues =& arrayOfApplicationValue;
		ns6__ApplicationValue applicationValue;
		applicationValue.Application = stringPool.Create(APP_DATA_APPNAME);
		std::_tstring followup_key = APP_DATA_FOLLOWUP;
		followup_key += CString((GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_USER)));
		applicationValue.Name = stringPool.Create(followup_key);
		applicationValue.Value = stringPool.Create(followup ? _T("true") : _T("false"));

		_ns6__WUUpdateResponse response;
		ESP_EXCEPTION_LOG3(response.Exceptions);
		if (server.WUUpdate(&request, &response) == SOAP_OK)
		{
			retVal = true;
		}
		else
			_DBGLOG(url, LEVEL_WARNING, server.GetClientErrorMsg());
	}
	return retVal;
}

bool AbortWorkunits(const CString &url, IWorkunitVector *workunits)
{
	CSoapInitialize<WsWorkunitsServiceSoapProxy> server(url);
	CStringPool stringPool;

	_ns6__WUAbort request;
	ns6__EspStringArray espStringArray;
	request.Wuids = &espStringArray;
	for (unsigned i = 0; i < workunits->size(); ++i)
		espStringArray.Item.push_back(stringPool.Create((*workunits)[i]->GetWuid()));

	_ns6__WUAbortResponse response;
	ESP_EXCEPTION_LOG3(response.Exceptions);
	if (server.WUAbort(&request, &response) == SOAP_OK)
	{
		return true;
	}
	else
		_DBGLOG(url, LEVEL_WARNING, server.GetClientErrorMsg());

	return false;
}

bool ResubmitWorkunits(const CString &url, IWorkunitVector *workunits)
{
	CSoapInitialize<WsWorkunitsServiceSoapProxy> server(url);
	CStringPool stringPool;

	_ns6__WUResubmit request;
	ns6__EspStringArray espStringArray;
	request.Wuids = &espStringArray;
	for (unsigned i = 0; i < workunits->size(); ++i)
		espStringArray.Item.push_back(stringPool.Create((*workunits)[i]->GetWuid()));

	_ns6__WUResubmitResponse response;
	ESP_EXCEPTION_LOG3(response.Exceptions);
	if (server.WUResubmit(&request, &response) == SOAP_OK)
	{
		return true;
	}
	else
		_DBGLOG(url, LEVEL_WARNING, server.GetClientErrorMsg());

	return false;
}

class CMonitorCount
{
public:
	static long m_Count;
	CMonitorCount()
	{
		InterlockedIncrement(&m_Count);
	}
	~CMonitorCount()
	{
		InterlockedDecrement(&m_Count);
	}
};

long CMonitorCount::m_Count = 0;

#define MAX_TRY_COUNT 8
#define MINUTE 60000
#define SECOND 1000

class CDali : public IDali, public CUnknown
{
protected:
	CString m_id;
	CServerConfig m_config;

public:
	IMPLEMENT_CUNKNOWN

	CDali(const CString & url, const CString & label) : m_config(url,label)
	{ 
		SetID();
	}

	CDali(const CServerConfig& config) : m_config(config)
	{
		SetID();
	}

	~CDali()
	{
	}

	void SetID()
	{
		m_id = m_config.m_url + _T("/") + m_config.m_label;
	}

	const CServerConfig* GetServerConfig() const
	{
		return &m_config;
	}

	const TCHAR * GetID() const
	{
		return m_id;
	}

	const TCHAR * GetCacheID() const
	{
		return m_id;
	}

	const TCHAR * GetLabel() const
	{
		return m_config.m_label;
	}

	const TCHAR * GetUrl() const
	{
		return m_config.m_url;
	}

	void GetWorkunitsAsync(const CString &cluster, const CString &owner, const CString &state, const CString &fromUTC, const CString &toUTC, int count, const async_wucomplete_slot_type &s) const
	{
		clib::thread run(__FUNCTION__, boost::bind(&GetWorkunits2, m_config, cluster, owner, state, fromUTC, toUTC, count, s));
	}

	unsigned GetWorkunits(const CString &cluster, const CString &owner, const CString &state, const CString &fromUTC, const CString &toUTC, int count, IWorkunitVector *workunits) const
	{
		return GetWorkunits1(m_config, _T(""), cluster, owner, state, fromUTC, toUTC, count, workunits);
	}

	IWorkunit *GetWorkunitFast(const CString &wuid, bool noBroadcast = false) const
	{
		IWorkunit * retVal = GetCachedWorkunit(m_config, wuid);
		if (retVal)
			return retVal;

		CComPtr<IEclCC> eclcc = CreateIEclCC();
		if (eclcc)
		{
			boost::filesystem::wpath path = eclcc->GetWorkingFolder();
		}

		retVal = GetWorkunit(m_config, wuid, noBroadcast);
		if (retVal)
			return retVal;

		IWorkunitVector workunits;
		GetWorkunits1(m_config, wuid, _T(""), _T(""), _T(""), _T(""), _T(""), 1, &workunits, noBroadcast);
		if (workunits.size() > 0)
			return workunits[0];

		return NULL;
	}

	IWorkunit *GetWorkunit(const CString &wuid, bool noBroadcast = false) const
	{
		return GetWorkunit(m_config, wuid, noBroadcast);
	}

	IWorkunit *GetXGMMLWorkunit(const CString &filePath) const
	{
		return CreateXGMMLWorkunit(filePath, m_config.m_label, _T("007"));
	}

	signed GetResultData(const CString & wuid, int sequence, __int64 start, int count, ITable * result, __int64 &total) const
	{
		return Dali::GetResultData(m_config, wuid, sequence, start, count, result, total);
	}

	void MonitorState(const CString & _wuid) const
	{
		std::_tstring wuid(_wuid);
		clib::thread run(__FUNCTION__, boost::bind(&BlockUntilComplete, m_config, wuid));
	}

	IGraph * GetGraph(const CString &pWuid, const CString &graphName)
	{
		CString url, user, password;
		CSoapInitialize<WsWorkunitsServiceSoapProxy> server(m_config.GetUrl(url), m_config.GetUserId(user), m_config.GetPassword(password));
		CStringPool stringPool;

		_ns6__WUGetGraph request;
		request.Wuid = stringPool.Create(pWuid);
		request.GraphName = stringPool.Create(graphName);

		_ns6__WUGetGraphResponse response;
		ESP_EXCEPTION_LOG3(response.Exceptions);
		if (server.WUGetGraph(&request, &response) == SOAP_OK)
		{
			if (response.Graphs)
			{
				CString url;
				m_config.GetUrl(url);
				for (std::size_t i = 0; i < response.Graphs->ECLGraphEx.size(); ++i)
					CreateGraph(url, pWuid, response.Graphs->ECLGraphEx[i]);
			}
		}
		else
			_DBGLOG(url, LEVEL_WARNING, server.GetClientErrorMsg());

		return NULL;
	}

	unsigned GetGraphs(const CString &pWuid)
	{
		CString url, user, password;
		CSoapInitialize<WsWorkunitsServiceSoapProxy> server(m_config.GetUrl(url), m_config.GetUserId(user), m_config.GetPassword(password));
		CStringPool stringPool;

		_ns6__WUInfo request;
		request.Wuid = stringPool.Create(pWuid);
		request.IncludeApplicationValues = false;
		request.IncludeDebugValues = false;
		request.IncludeExceptions = false;
		request.IncludeGraphs = true;
		request.IncludeResults = false;
		request.IncludeSourceFiles = false;
		request.IncludeTimers = false;
		request.IncludeVariables = false;
		request.IncludeWorkflows = false;
		request.SuppressResultSchemas = true;

		_ns6__WUInfoResponse response;
		ESP_EXCEPTION_LOG3(response.Exceptions);
		if (server.WUInfo(&request, &response) == SOAP_OK)
		{
			if (response.Workunit)
			{
				if (response.Workunit->Wuid)
				{
					CreateWorkunit(m_config, pWuid, response.Workunit->Graphs);
					int graphCount = 0;
					SAFE_ASSIGN(graphCount, response.Workunit->GraphCount);
					return graphCount;
				}
			}
		}
		else
			_DBGLOG(url, LEVEL_WARNING, server.GetClientErrorMsg());

		return 0;
	}

	unsigned GetGraphTimings(const CString &pWuid, CGraphTimingVector & results)
	{
		CString url, user, password;
		CSoapInitialize<WsWorkunitsServiceSoapProxy> server(m_config.GetUrl(url), m_config.GetUserId(user), m_config.GetPassword(password));
		CStringPool stringPool;

		_ns6__WUGraphTiming request;
		request.Wuid = stringPool.Create(pWuid);

		_ns6__WUGraphTimingResponse response;
		ESP_EXCEPTION_LOG3(response.Exceptions);
		if (server.WUGraphTiming(&request, &response) == SOAP_OK)
		{
			if (response.Workunit)
			{
				CComPtr<IWorkunit> wu = CreateWorkunit(m_config, pWuid, response.Workunit->TimingData);
				wu->GetGraphTimings(results);
			}
		}
		else
			_DBGLOG(m_config.GetUrl(url), LEVEL_WARNING, server.GetClientErrorMsg());

		return results.size();
	}

	bool AddLocalFileToWorkunit(const std::_tstring & pWuid, const std::_tstring & pName, const std::_tstring & pVal)
	{
		CString url, user, password;
		CSoapInitialize<WsWorkunitsServiceSoapProxy> server(m_config.GetUrl(url), m_config.GetUserId(user), m_config.GetPassword(password));
		CStringPool stringPool;

		_ns6__WUAddLocalFileToWorkunit request;
		request.Wuid = stringPool.Create(pWuid);
		request.Name = stringPool.Create(pName);
		request.Val = stringPool.Create(pVal);
		int Length = request.Val->length();
		request.Length = &Length;

		_ns6__WUAddLocalFileToWorkunitResponse response;
		ESP_EXCEPTION_LOG3(response.Exceptions);
		if (server.WUAddLocalFileToWorkunit(&request, &response) == SOAP_OK)
		{
			return true;
		}
		else
			_DBGLOG(m_config.GetUrl(url), LEVEL_WARNING, server.GetClientErrorMsg());
		return false;
	}
	unsigned GetLocalFiles(const CString &pWuid, ILocalFileVector & localfiles)
	{
		CString url, user, password;
		CSoapInitialize<WsWorkunitsServiceSoapProxy> server(m_config.GetUrl(url), m_config.GetUserId(user), m_config.GetPassword(password));
		CStringPool stringPool;

		_ns6__WUListLocalFileRequired request;
		request.Wuid = stringPool.Create(pWuid);

		_ns6__WUListLocalFileRequiredResponse response;
		ESP_EXCEPTION_LOG3(response.Exceptions);
		if (server.WUListLocalFileRequired(&request, &response) == SOAP_OK)
		{
			if (response.LocalFileUploads)
			{
				for(std::size_t i = 0; i < response.LocalFileUploads->LogicalFileUpload.size(); ++i)
				{
					StlLinked<ILocalFile> localfile = CreateLocalFile(url, pWuid, response.LocalFileUploads->LogicalFileUpload[i]);
					localfiles.push_back(localfile);
				}
			}
		}
		else
			_DBGLOG(m_config.GetUrl(url), LEVEL_WARNING, server.GetClientErrorMsg());

		return localfiles.size();
	}

	IWorkunit* Submit(const CString &cluster, const CString & queue, WUAction action, const CString & attrQualifiedLabel, const CString & ecl, const CString & path, const CString & label, int resultLimit, const CString & debugSettings, bool archive, int maxRunTime, bool debug)
	{
		CComInitialize com;
		StlLinked<IWorkunit> resultWu;
		SubmitEcl(cluster, queue, action, attrQualifiedLabel, ecl, path, label, resultLimit, resultWu, debugSettings, archive, false, maxRunTime, debug);
		return resultWu.get();
	}

	IWorkunit* Schedule(const CString &cluster, const CString & queue, const CString & attrQualifiedLabel, const CString & ecl, const CString &when, const CString & label, int resultLimit, const CString & debugSettings, bool archive)
	{
		CComInitialize com;
		StlLinked<IWorkunit> resultWu;
		ScheduleEcl(cluster, queue, attrQualifiedLabel, ecl, when, label, resultLimit, resultWu, debugSettings, archive);
		return resultWu.get();
	}

	bool CheckComplexity(const CString &cluster, const CString & queue, const CString & ecl, const CString &module, const CString &attribute, std::_tstring & result, CEclExceptionVector & errorResults)
	{
		CComInitialize com;
		AttributeVector results;
		CComBSTR _result = _T("");
		if (CompileEcl((const TCHAR*)ecl, (const TCHAR*)module, (const TCHAR*)attribute, (const TCHAR*)queue, (const TCHAR*)cluster, _T(""), false, true, -1, _result, results, errorResults))
		{
			result = CW2T(_result);
			return true;
		}
		return false;
	}

	void GetDependancies(const CString &cluster, const CString & queue, const CString & ecl, const CString &module, const CString &attribute, AttributeVector & results, CEclExceptionVector & errorResults)
	{
		CComInitialize com;
		CComBSTR result;
		CompileEcl((const TCHAR*)ecl, (const TCHAR*)module, (const TCHAR*)attribute, (const TCHAR*)queue, (const TCHAR*)cluster, _T(""), true, false, -1, result, results, errorResults);
	}

	bool GetDependanciesTree(const std::_tstring & cluster, const std::_tstring & queue, const std::_tstring & snapshot, const std::_tstring & items, std::_tstring & xmlDepends, CEclExceptionVector & outExceptions)
	{
		CString url, user, password;
		CSoapInitialize<WsWorkunitsServiceSoapProxy> server(m_config.GetUrl(url), m_config.GetUserId(user), m_config.GetPassword(password), 0, 20 * RECV_TIMEOUT, 20 * RECV_TIMEOUT);
		CStringPool stringPool;

		_ns6__WUGetDependancyTrees request;
		request.Cluster = stringPool.Create(cluster);
		request.Queue = stringPool.Create(queue);
		request.Snapshot = stringPool.Create(snapshot);
		request.Items = stringPool.Create(items);
		request.TimeoutMilliSec = stringPool.Create(_T("3600000"));	//One Hour

		_ns6__WUGetDependancyTreesResponse response;
		ESP_EXCEPTION_LOG3(response.Exceptions);
		if (server.WUGetDependancyTrees(&request, &response) == SOAP_OK)
		{
			if (response.DependancyTrees)
			{
				std::string nxml;
				nxml.append((const char *)response.DependancyTrees->__ptr, response.DependancyTrees->__size);
				xmlDepends = CA2T(nxml.c_str(), CP_UTF8);
			}
			if (response.Errors)
			{
				processExceptions(*response.Errors, outExceptions);
			}
			return true;
		}
		else
			_DBGLOG(m_config.GetUrl(url), LEVEL_WARNING, server.GetClientErrorMsg());

		return false;
	}

	void processExceptions(ns6__ArrayOfECLException & errors, CEclExceptionVector & results)
	{
		for(std::size_t i = 0; i < errors.ECLException.size(); ++i)
		{
			StlLinked<CEclException> e = new CEclException();
			SAFE_ASSIGN(e->m_lineNo, errors.ECLException[i]->LineNo);
			SAFE_ASSIGN(e->m_code, errors.ECLException[i]->Code);
			SAFE_ASSIGN2CSTRING(e->m_message, errors.ECLException[i]->Message);
			SAFE_ASSIGN(e->m_column, errors.ECLException[i]->Column);
			SAFE_ASSIGN2CSTRING(e->m_severity, errors.ECLException[i]->Severity);
			SAFE_ASSIGN2CSTRING(e->m_fileName, errors.ECLException[i]->FileName);
			results.push_back(e);
		}
	}

	bool CheckSyntax(const CString &cluster, const CString & queue, const CString & module, const CString & attribute, const CString & path, const CString & ecl, int _timeToWait, const CString & debugString, bool archive, bool noCommonPrivateAttributes, CEclExceptionVector & results)
	{
		if (CComPtr<IEclCC> eclcc = CreateIEclCC())
		{
			eclcc->CheckSyntax(static_cast<const TCHAR *>(module), static_cast<const TCHAR *>(attribute), static_cast<const TCHAR *>(path), static_cast<const TCHAR *>(ecl), results);
			return true;
		}

		CString url, user, password;
		CSoapInitialize<WsWorkunitsServiceSoapProxy> server(m_config.GetUrl(url), m_config.GetUserId(user), m_config.GetPassword(password));
		CStringPool stringPool;

		_ns6__WUSyntaxCheckECL request;
		request.ECL = stringPool.Create(ecl);
		request.ModuleName = stringPool.Create(module);
		request.AttributeName = stringPool.Create(attribute);
		request.Queue = stringPool.Create(queue);
		request.Cluster = stringPool.Create(cluster);
		request.TimeToWait = _timeToWait;

		CStructPool<ns6__DebugValue> debugValuePool;
		ns6__ArrayOfDebugValue arrayOfDebugValues;
		request.DebugValues = &arrayOfDebugValues;

		CStructPool<ns6__ApplicationValue> applicationValuePool;
		ns6__ArrayOfApplicationValue arrayOfApplicationValues;

		ParseDebugString(debugString, archive, noCommonPrivateAttributes, false, stringPool, arrayOfDebugValues, debugValuePool, arrayOfApplicationValues, applicationValuePool);

		_ns6__WUSyntaxCheckResponse response;
		ESP_EXCEPTION_LOG3(response.Exceptions);
		if (server.WUSyntaxCheckECL(&request, &response) == SOAP_OK)
		{
			if (response.Errors)
				processExceptions(*response.Errors, results);

			if (response.Exceptions)
			{
				StlLinked<CEclException> e = new CEclException();
				e->m_severity = CEclException::ECL_EXCEPTION_ERROR;
				e->m_message = _T("ESP Exception(s).  See Warning window for more information...");
				e->m_fileName = path;
				results.push_back(e);
			}

			return true;
		}
		else
		{
			_DBGLOG(m_config.GetUrl(url), LEVEL_WARNING, server.GetClientErrorMsg());
			StlLinked<CEclException> e = new CEclException();
			e->m_severity = CEclException::ECL_EXCEPTION_ERROR;
			e->m_message = CString(_T("Network Exception(s).  See Warning window for more information...")) + server.GetClientErrorMsg();
			e->m_fileName = path;
			results.push_back(e);
		}
		return false;
	}

	bool AbortWorkunits(IWorkunitVector *workunits)
	{
		return Dali::AbortWorkunits(m_config.m_url, workunits);
	}

	bool DeleteWorkunits(IWorkunitVector *workunits)
	{
		return DeleteWorkunits(m_config, workunits);
	}

	bool ProtectWorkunits(IWorkunitVector *workunits)
	{
		return Dali::ProtectWorkunits(m_config.m_url, workunits, true);
	}

	bool UnprotectWorkunits(IWorkunitVector *workunits)
	{
		return Dali::ProtectWorkunits(m_config.m_url, workunits, false);
	}

	bool FlagFollowupWorkunits(IWorkunitVector *workunits)
	{
		return Dali::FollowupWorkunits(m_config.m_url, workunits, true);
	}

	bool ClearFollowupWorkunits(IWorkunitVector *workunits)
	{
		return Dali::FollowupWorkunits(m_config.m_url, workunits, false);
	}

	bool ResubmitWorkunits(IWorkunitVector *workunits)
	{
		return Dali::ResubmitWorkunits(m_config.m_url, workunits);
	}

	bool SetState(IWorkunitVector *workunits, WUState state)
	{
		return SetState(m_config, workunits, state);
	}
	void SetCredentials(const TCHAR* userId, const TCHAR* password)
	{
		m_config.m_userId = userId;
		m_config.m_password = password;
	}
	bool SendDebugRequest(const std::_tstring & wuid, const std::_tstring & _request, std::_tstring & result)
	{
		CString url, user, password;
		CSoapInitialize<WsWorkunitsServiceSoapProxy> server(m_config.GetUrl(url), m_config.GetUserId(user), m_config.GetPassword(password));
		CStringPool stringPool;

		_ns6__WUCDebug request;
		request.Wuid = stringPool.Create(wuid);
		request.Command = stringPool.Create(_request);

		_ns6__WUDebugResponse response;
		ESP_EXCEPTION_LOG3(response.Exceptions);
		if (server.WUCDebug(&request, &response) == SOAP_OK)
		{
			if (response.Result)
			{
				result = *response.Result;
				return true;
			}
			else
				_DBGLOG(m_config.GetUrl(url), LEVEL_WARNING, _T("WUCDebug returned NULL response."));
		}
		else
			_DBGLOG(m_config.GetUrl(url), LEVEL_WARNING, server.GetClientErrorMsg());

		return false;
	}

protected:
	bool SetState(const CServerConfig &config, IWorkunitVector *workunits, WUState state)
	{
		for(IWorkunitVector::iterator itr = workunits->begin(); itr != workunits->end(); ++itr)
		{
			Dali::IWorkunitAdapt& workunit = *itr;

			CString url, user, password;
			CSoapInitialize<WsWorkunitsServiceSoapProxy> server(m_config.GetUrl(url), m_config.GetUserId(user), m_config.GetPassword(password));
			CStringPool stringPool;

			_ns6__WUUpdate request;
			request.Wuid = stringPool.Create(workunit->GetWuid());
			int State = state;
			request.State = &State;

			_ns6__WUUpdateResponse response;
			ESP_EXCEPTION_LOG3(response.Exceptions);
			if (server.WUUpdate(&request, &response) == SOAP_OK)  
			{
				//GJS Check if GetWorkunit is actually needed or is the updated WU returned?
				IWorkunitAdapt _wu = GetWorkunit(config, workunit->GetWuid(), false);
			}
			else
			{
				_DBGLOG(config.GetUrl(url), LEVEL_WARNING, server.GetClientErrorMsg());
				return false;
			}
		}
		return true;
	}



	static unsigned GetWorkunits1(const CServerConfig& config, const CString &wuid, const CString &cluster, const CString &owner, const CString &state, const CString &fromUTC, const CString &toUTC, int _count, IWorkunitVector *workunits, bool noBroadcast = false) 
	{
		CComPtr<IEclCC> eclcc = CreateIEclCC();
		if (eclcc)
		{
			boost::filesystem::wpath path = eclcc->GetWorkingFolder();
			if (boost::filesystem::exists(path))
			{
				boost::filesystem::wdirectory_iterator attr_itr_end;
				for (boost::filesystem::wdirectory_iterator attr_itr(path); attr_itr != attr_itr_end; ++attr_itr)
				{
					if (!boost::filesystem::is_directory(*attr_itr))
					{  
						std::_tstring fromWU = fromUTC;
						std::_tstring toWU = toUTC;
						boost::algorithm::replace_all(fromWU, _T("-"), _T(""));
						boost::algorithm::replace_all(fromWU, _T("T"), _T("-"));
						boost::algorithm::replace_all(fromWU, _T("Z"), _T(""));
						fromWU = _T("L") + fromWU;
						boost::algorithm::replace_all(toWU, _T("-"), _T(""));
						boost::algorithm::replace_all(toWU, _T("T"), _T("-"));
						boost::algorithm::replace_all(toWU, _T("Z"), _T(""));
						toWU = _T("L") + toWU;
						std::_tstring basename = boost::filesystem::basename(*attr_itr).c_str();
						std::_tstring extension = boost::filesystem::extension(*attr_itr).c_str();
						if (boost::algorithm::iequals(extension, _T(".exe")))
						{
							if (wuid.IsEmpty() || boost::algorithm::iequals(basename, (const TCHAR *)wuid))
							{
								IWorkunitAdapt wu = CreateLocalWorkunit(eclcc->GetWorkingFolder(), basename.c_str());
								if (fromWU.compare(wu->GetWuid()) < 0 && toWU.compare(wu->GetWuid()) >= 0)
								{
									if (state.IsEmpty() || boost::algorithm::iequals(wu->GetStateLabel(), (const TCHAR *)state))
									{
										if (wu->GetGraphCount() == 0)
										{
											std::_tstring wuXml, results;
											eclcc->GetWorkunitXML(basename, wuXml);
											CEclExceptionVector dummy;
											UpdateLocalWorkunit(wu, wuXml, results, dummy);
										}
										workunits->push_back(wu);
									}
								}
							}
						}
					}
				}
				std::sort(workunits->rbegin(), workunits->rend(), IWorkunitCompare());
			}
		}

		CString url, user, password;
		CSoapInitialize<WsWorkunitsServiceSoapProxy> server(config.GetUrl(url), config.GetUserId(user), config.GetPassword(password));
		CStringPool stringPool;

		_ns6__WUQuery request;
		request.Cluster = stringPool.Create(cluster);
		request.Owner = stringPool.Create(owner);
		request.State = stringPool.Create(state);
		request.StartDate = stringPool.Create(fromUTC);
		request.EndDate = stringPool.Create(toUTC);
		request.Wuid = stringPool.Create(wuid);
		request.Descending = false;
		LONG64 start = 0;
		LONG64 end = _count;
		int count = _count;
		request.PageStartFrom = &start;
		request.PageSize = &end;
		request.Count = &count;

		_ns6__WUQueryResponse response;
		ESP_EXCEPTION_LOG3(response.Exceptions);
		if (server.WUQuery(&request, &response) == SOAP_OK)
		{
			if (response.Workunits)
			{
				for(std::size_t i = 0; i < response.Workunits->ECLWorkunit.size(); ++i)
				{
					IWorkunitAdapt workunit = CreateWorkunit(config, response.Workunits->ECLWorkunit[i], noBroadcast);
					workunits->push_back(workunit);
				}
			}
		}
		else
			_DBGLOG(config.GetUrl(url), LEVEL_WARNING, server.GetClientErrorMsg());

		return workunits->size();
	}

	static void GetWorkunits2(CServerConfig config, CString cluster, CString owner, CString state, CString fromUTC, CString toUTC, int count, async_wucomplete_slot_type s) 
	{
		CComInitialize com;
		StlLinked<IWorkunitVectorCom> workunits = new IWorkunitVectorCom;
		async_wucomplete_signal_type on_completion;
		on_completion.connect(s);
		GetWorkunits1(config, _T(""), cluster, owner, state, fromUTC, toUTC, count, workunits.get());
		on_completion(workunits.get());
		return;
	}

	static IWorkunit * GetWorkunit(CServerConfig config, CString wuid, bool noBroadcast) 
	{
		StlLinked<IWorkunit> retVal;

		if (boost::algorithm::istarts_with((const TCHAR *)wuid, _T("L")))
		{
			CComPtr<IEclCC> eclcc = CreateIEclCC();
			if (eclcc)
			{
				boost::filesystem::wpath path = eclcc->GetWorkingFolder();
				path /= (const TCHAR *)(wuid + _T(".exe"));
				if (boost::filesystem::exists(path))
				{
					IWorkunitAdapt wu = CreateLocalWorkunit(eclcc->GetWorkingFolder(), wuid);
					if (wu->GetGraphCount() == 0)
					{
						std::_tstring wuXml, results;
						eclcc->GetWorkunitXML((const TCHAR *)wuid, wuXml);
						CEclExceptionVector dummy;
						UpdateLocalWorkunit(wu, wuXml, results, dummy);
					}
					return wu;
				}
			}
			return NULL;
		}

		CString url, user, password;
		CSoapInitialize<WsWorkunitsServiceSoapProxy> server(config.GetUrl(url), config.GetUserId(user), config.GetPassword(password));
		CStringPool stringPool;

		_ns6__WUInfo request;
		request.Wuid = stringPool.Create(wuid);
		request.IncludeApplicationValues = true;
		request.IncludeDebugValues = true;
		request.IncludeExceptions = true;
		request.IncludeGraphs = true;
		request.IncludeResults = true;
		request.IncludeSourceFiles = false;
		request.IncludeTimers = true;
		request.IncludeVariables = false;
		request.IncludeWorkflows = false;
		request.SuppressResultSchemas = true;

		_ns6__WUInfoResponse response;
		ESP_EXCEPTION_LOG3(response.Exceptions);
		if (server.WUInfo(&request, &response) == SOAP_OK)
		{
			if (response.Workunit)
			{
				if (response.Workunit->Wuid)
					retVal = CreateWorkunit(config, response.Workunit, noBroadcast);
			}
		}
		else
			_DBGLOG(config.GetUrl(url), LEVEL_WARNING, server.GetClientErrorMsg());

		return retVal.get();
	}

	bool BlockingSubmitEcl( const CString &cluster, const CString & queue, WUAction action, const CString & attrQualifiedLabel, const CString & ecl, const CString & path, const CString &label, int resultLimit, const CString & debugSettings, bool archive, bool noCommonPrivateAttributes, StlLinked<IWorkunit> &resultWu, bool deleteAfter, int maxRunTime, bool debug)
	{
		CComInitialize com;
		bool retVal = false;
		if (SubmitEcl(cluster, queue, action, attrQualifiedLabel, ecl, path, label, resultLimit, resultWu, debugSettings, archive, noCommonPrivateAttributes, maxRunTime, debug))
		{
			retVal = BlockUntilComplete(m_config, resultWu->GetWuid());

			if (retVal && deleteAfter)
				DeleteWorkunit(resultWu.get());
		}

		return retVal;
	}

	bool ScheduleEcl(const CString &cluster, const CString & queue, const CString & attrQualifiedLabel, const CString & ecl, const CString & when, const CString & label, int resultLimit, StlLinked<IWorkunit> &resultWu, const CString & debugString, bool archive)
	{
		bool retVal = false;
		CString url, user, password;
		CSoapInitialize<WsWorkunitsServiceSoapProxy> server(m_config.GetUrl(url), m_config.GetUserId(user), m_config.GetPassword(password));
		CStringPool stringPool;

		_ns6__WUCreate request;

		_ns6__WUCreateResponse response;
		ESP_EXCEPTION_LOG3(response.Exceptions);
		if (server.WUCreate(&request, &response) == SOAP_OK)
		{
			if (response.Workunit)
			{
				if (response.Workunit->Wuid)
				{
					std::_tstring wuid = *response.Workunit->Wuid;
					CSoapInitialize<WsWorkunitsServiceSoapProxy> server(m_config.GetUrl(url), m_config.GetUserId(user), m_config.GetPassword(password));

					_ns6__WUUpdate request;
					request.Wuid = &wuid;
					request.QueryText = stringPool.Create(ecl);
					int action = WUActionRun;
					request.Action = &action;
					request.ResultLimit = &resultLimit;

					CStructPool<ns6__DebugValue> debugValuePool;
					ns6__ArrayOfDebugValue arrayOfDebugValues;
					request.DebugValues = &arrayOfDebugValues;

					CStructPool<ns6__ApplicationValue> applicationValuePool;
					ns6__ArrayOfApplicationValue arrayOfApplicationValues;
					request.ApplicationValues = &arrayOfApplicationValues;

					ParseDebugString(debugString, archive, false, false, stringPool, arrayOfDebugValues, debugValuePool, arrayOfApplicationValues, applicationValuePool);

					ns6__ApplicationValue applicationValue;
					applicationValue.Application = stringPool.Create(APP_DATA_APPNAME);
					applicationValue.Name = stringPool.Create(APP_DATA_VERSION);
					applicationValue.Value = stringPool.Create(CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_APP_VERSION)));

#if _COMMS_VER < 300682
#else
					request.QueryMainDefinition = stringPool.Create(attrQualifiedLabel);
#endif

					_ns6__WUUpdateResponse response;
					ESP_EXCEPTION_LOG3(response.Exceptions);
					if (server.WUUpdate(&request, &response) == SOAP_OK)
					{
						CSoapInitialize<WsWorkunitsServiceSoapProxy> server(m_config.GetUrl(url), m_config.GetUserId(user), m_config.GetPassword(password));

						_ns6__WUSchedule request;
						request.Wuid = &wuid;
						request.Cluster = stringPool.Create(cluster);
						request.Queue = stringPool.Create(queue);
						request.Snapshot = stringPool.Create(label);
						time_t time;
						UTCToTimeT((const TCHAR *)when, time);
						request.When = &time;

						_ns6__WUScheduleResponse response;
						ESP_EXCEPTION_LOG3(response.Exceptions);
						if (server.WUSchedule(&request, &response) == SOAP_OK)
						{
							resultWu = GetWorkunit(m_config, wuid.c_str(), false);
							retVal = true;
						}
						else
							_DBGLOG(m_config.GetUrl(url), LEVEL_WARNING, server.GetClientErrorMsg());
					}
					else
						_DBGLOG(m_config.GetUrl(url), LEVEL_WARNING, server.GetClientErrorMsg());
				}
			}
		}
		else
			_DBGLOG(m_config.GetUrl(url), LEVEL_WARNING, server.GetClientErrorMsg());
		return retVal;
	}

	static void threadSubmitLocalWorkunit(IWorkunit * resultWu, CString path, CString ecl, int resultLimit, CString debugString, WUAction _action)
	{
		if (CComPtr<IEclCC> eclcc = CreateIEclCC())
		{
			std::_tstring wuXml, results;
			bool hasErrors = false;
			UpdateLocalWorkunit(resultWu, WUStateCompiling);
			Dali::CEclExceptionVector errors;
			eclcc->Compile(static_cast<const TCHAR *>(path), static_cast<const TCHAR *>(ecl), resultWu->GetWuid(), resultLimit, static_cast<const TCHAR *>(debugString), wuXml, hasErrors, errors);
			if (!hasErrors)
			{
				UpdateLocalWorkunit(resultWu, WUStateRunning);
				eclcc->GetWorkunitResults(resultWu->GetWuid(), _action == WUActionCompile, results, hasErrors, errors);
			}
			UpdateLocalWorkunit(resultWu, WUStateCompleted);
			UpdateLocalWorkunit(resultWu, wuXml, results, errors);
		}
	}

	bool SubmitEcl(const CString &cluster, const CString & queue, WUAction _action, const CString & attrQualifiedLabel, const CString & _ecl, const CString & path, const CString label, int resultLimit, StlLinked<IWorkunit> &resultWu, const CString & debugString, bool archive, bool noCommonPrivateAttributes, int maxRunTime, bool debug)
	{
		bool isXml = false;
		CString ecl = _ecl;
		std::_tstring queryName;
		{//  ECL or XML
			CComInitialize com;
			CComPtr<ISAXXMLReader> pRdr; 
			HRESULT hr = pRdr.CoCreateInstance(CLSID_SAXXMLReader30);
			CComPtr <RoxieXMLHandler> pMc = new RoxieXMLHandler();
			hr = pRdr->putContentHandler(pMc); 
			CComPtr<SAXErrorHandler> pErr = new SAXErrorHandler(true);
			if(SUCCEEDED(hr))
				pRdr->putErrorHandler(pErr);

			if (hr == S_OK)
			{
				hr = pRdr->parse(CComVariant(ecl));
				if (SUCCEEDED(hr))
				{
					isXml = true;
					queryName = pMc->m_queryName;
				}
			}
		}

		if (!isXml && !boost::algorithm::iequals(static_cast<const TCHAR *>(cluster), _T("Local")))
		{
			CComPtr<IEclCC> eclcc = CreateIEclCC();
			if (_action != WUActionExecuteExisting && eclcc)
			{
				std::_tstring result;
				Dali::CEclExceptionVector errors;
				if(eclcc->GetArchive(static_cast<const TCHAR *>(path), static_cast<const TCHAR *>(ecl), result, errors))
					ecl = result.c_str();
				else
					ecl = _T("");
				if (ecl.IsEmpty())
				{
					resultWu = CreateLocalWorkunit(eclcc->GetWorkingFolder());
					std::_tstring wuXml, results;
					UpdateLocalWorkunit(resultWu, wuXml, results, errors);
					return false;
				}
			}
		}

		if (boost::algorithm::iequals(static_cast<const TCHAR *>(cluster), _T("Local")))
		{
			CComPtr<IEclCC> eclcc = CreateIEclCC();
			if (eclcc)
			{
				resultWu = CreateLocalWorkunit(eclcc->GetWorkingFolder());
				clib::thread run(__FUNCTION__, boost::bind(&threadSubmitLocalWorkunit, resultWu, path, ecl, resultLimit, debugString, _action));
				return true;
			}
			return false;
		}
		bool retVal = false;

		CString url, user, password;
		CSoapInitialize<WsWorkunitsServiceSoapProxy> server(m_config.GetUrl(url), m_config.GetUserId(user), m_config.GetPassword(password));
		CStringPool stringPool;

		_ns6__WUCreate request;

		_ns6__WUCreateResponse response;
		ESP_EXCEPTION_LOG3(response.Exceptions);
		if (server.WUCreate(&request, &response) == SOAP_OK)
		{
			if (response.Workunit)
			{
				if (response.Workunit->Wuid)
				{
					std::_tstring wuid = *response.Workunit->Wuid;
					CSoapInitialize<WsWorkunitsServiceSoapProxy> server(m_config.GetUrl(url), m_config.GetUserId(user), m_config.GetPassword(password));

					_ns6__WUUpdate request;
					request.Wuid = &wuid;
					int action = _action;
					request.Action = &action;
					request.ResultLimit = &resultLimit;

					if (!path.IsEmpty())
					{
						boost::filesystem::wpath p = path;
						request.Jobname = stringPool.Create(p.stem());
					}

					CStructPool<ns6__DebugValue> debugValuePool;
					ns6__ArrayOfDebugValue arrayOfDebugValues;
					request.DebugValues = &arrayOfDebugValues;

					CStructPool<ns6__ApplicationValue> applicationValuePool;
					ns6__ArrayOfApplicationValue arrayOfApplicationValues;
					request.ApplicationValues = &arrayOfApplicationValues;

					ParseDebugString(debugString, archive, noCommonPrivateAttributes, debug, stringPool, arrayOfDebugValues, debugValuePool, arrayOfApplicationValues, applicationValuePool);

					ns6__ApplicationValue applicationValue;
					applicationValue.Application = stringPool.Create(APP_DATA_APPNAME);
					applicationValue.Name = stringPool.Create(APP_DATA_VERSION);
					applicationValue.Value = stringPool.Create(CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_APP_VERSION)));

					if (isXml)
					{
						request.XmlParams = stringPool.Create(ecl);
						action = WUActionExecuteExisting;
						request.Action = &action;
						request.Jobname = stringPool.Create(queryName);
					}
					else
						request.QueryText = stringPool.Create(ecl);

#if _COMMS_VER < 300682
#else
					request.QueryMainDefinition = stringPool.Create(attrQualifiedLabel);
#endif

					_ns6__WUUpdateResponse response;
					ESP_EXCEPTION_LOG3(response.Exceptions);
					if (server.WUUpdate(&request, &response) == SOAP_OK)
					{
						CSoapInitialize<WsWorkunitsServiceSoapProxy> server(m_config.GetUrl(url), m_config.GetUserId(user), m_config.GetPassword(password));

						_ns6__WUSubmit request;
						request.Wuid = &wuid;
						request.Cluster = stringPool.Create(cluster);
						request.Queue = stringPool.Create(queue);
						request.Snapshot = stringPool.Create(label);
						request.MaxRunTime = &maxRunTime;

						_ns6__WUSubmitResponse response;
						ESP_EXCEPTION_LOG3(response.Exceptions);
						if (server.WUSubmit(&request, &response) == SOAP_OK)
						{
							resultWu = GetWorkunit(m_config, wuid.c_str(), false);
							retVal = true;
						}
						else
							_DBGLOG(m_config.GetUrl(url), LEVEL_WARNING, server.GetClientErrorMsg());
					}
					else
						_DBGLOG(m_config.GetUrl(url), LEVEL_WARNING, server.GetClientErrorMsg());
				}
			}
		}
		else
			_DBGLOG(m_config.GetUrl(url), LEVEL_WARNING, server.GetClientErrorMsg());
		return retVal;
	}

	bool CompileEcl(const CComBSTR& ecl, const CComBSTR &module, const CComBSTR &attribute, const CComBSTR &queue, const CComBSTR &cluster, const CComBSTR &snapshot, bool includeDependancies, bool includeComplexity, int timeToWait, CComBSTR &outComplexity, AttributeVector & outDependicies, CEclExceptionVector & outExceptions)
	{
		CString url, user, password;
		CSoapInitialize<WsWorkunitsServiceSoapProxy> server(m_config.GetUrl(url), m_config.GetUserId(user), m_config.GetPassword(password));
		CStringPool stringPool;

		_ns6__WUCompileECL request;
		request.ECL = stringPool.Create(ecl);
		request.ModuleName = stringPool.Create(module);
		request.AttributeName = stringPool.Create(attribute);
		request.Queue = stringPool.Create(queue);
		request.Cluster = stringPool.Create(cluster);
		request.Snapshot = stringPool.Create(snapshot);
		request.IncludeDependencies = includeDependancies;
		request.IncludeComplexity = &includeComplexity;
		request.TimeToWait = timeToWait;

		_ns6__WUCompileECLResponse response;
		ESP_EXCEPTION_LOG3(response.Exceptions);
		if (server.WUCompileECL(&request, &response) == SOAP_OK)
		{
			if (response.Errors)
				processExceptions(*response.Errors, outExceptions);
			if (response.Dependencies)
			{
				for(std::size_t i = 0; i < response.Dependencies->ECLAttribute.size(); ++i)
				{
					std::_tstring module, attr;
					SAFE_ASSIGN(module, response.Dependencies->ECLAttribute[i]->ModuleName);
					SAFE_ASSIGN(attr, response.Dependencies->ECLAttribute[i]->AttributeName);
					outDependicies.push_back(Attribute(module, attr));
				}
			}
			return true;
		}
		else
			_DBGLOG(m_config.GetUrl(url), LEVEL_WARNING, server.GetClientErrorMsg());
		return false;
	}

	static bool BlockUntilComplete(CServerConfig config, std::_tstring wuid)
	{
		//don't bother if somebody else is doing it.
		if (!g_monitor.Start(wuid))
			return true;

		ATLTRACE(_T("BlockUntilComplete_Start:  %s\n"), wuid.c_str());

		IWorkunitAdapt workunit;
		std::map<WUState, boost::posix_time::ptime> delayMap;
		CString me = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_USER);
		Sleep(1 * SECOND);	//Always Sleep for 1 second before start monitoring (in general the user will have just done a WUInfo prior to asking for a monitor...

		//if they log off while we are waiting, it will no longer be active
		while(g_monitor.Active(wuid))
		{
			CMonitorCount count;
			IWorkunitVector results;
			workunit = NULL;
			int resultCount;
			{
				//lock to prevent refresh (we will do it manually in a minute...)
				resultCount = GetWorkunits1(config, wuid.c_str(), _T(""), _T(""), _T(""), _T(""), _T(""), 999, &results, true);
			}

			if (resultCount == 1)
			{
				workunit = results[0];
				//check again after the delay of getting the server response
				//and check to see if could have been deleted!
				if (!g_monitor.Active(wuid))
				{
					break;
				}

				WUState state = workunit->GetState();
				if (workunit->IsComplete())
				{
					workunit = GetWorkunit(config, workunit->GetWuid(), false);	//Need to do a full get to update the results list.
					break;
				}
				else
				{
					//  Manually update if not complete...
					workunit->ForceBroadcast();
				}

				if (delayMap.find(state) == delayMap.end())
					delayMap[state] = boost::posix_time::second_clock::local_time();

				int sleep_time = 0;
				if ((boost::posix_time::second_clock::local_time() - delayMap[state]) < boost::posix_time::time_duration(boost::posix_time::minutes(1)))
					sleep_time = 3 * SECOND;
				else if ((boost::posix_time::second_clock::local_time() - delayMap[state]) < boost::posix_time::time_duration(boost::posix_time::minutes(2)))
					sleep_time = 6 * SECOND;
				else if ((boost::posix_time::second_clock::local_time() - delayMap[state]) < boost::posix_time::time_duration(boost::posix_time::minutes(3)))
					sleep_time = 12 * SECOND;
				else if ((boost::posix_time::second_clock::local_time() - delayMap[state]) < boost::posix_time::time_duration(boost::posix_time::minutes(4)))
					sleep_time = 24 * SECOND;
				else if ((boost::posix_time::second_clock::local_time() - delayMap[state]) < boost::posix_time::time_duration(boost::posix_time::minutes(5)))
					sleep_time = 48 * SECOND;
				else
					sleep_time = 60 * SECOND;

				switch(state)
				{
				case WUStateDebugPaused:
					sleep_time = std::min<int>(sleep_time, 10 * SECOND);
					break;
				case WUStateDebugRunning:
					sleep_time = std::min<int>(sleep_time, 3 * SECOND);
					break;
				}
				ATLTRACE(_T("BlockUntilComplete_Sleep:  %s, %i\n"), wuid.c_str(), sleep_time);
				//don't sleep too long without checking active bit
				while (sleep_time > 0 && g_monitor.Active(wuid))
				{
					Sleep(3 * SECOND);
					sleep_time -= 3 * SECOND;
				}
			}
			else
			{
				break;
			}
		}
		ATLTRACE(_T("BlockUntilComplete_Finish:  %s\n"), wuid.c_str());
		g_monitor.Stop(wuid);
		return true;
	}

	bool DeleteWorkunit(IWorkunit *workunit)
	{
		IWorkunitVector workunits;
		workunits.push_back(workunit);
		return DeleteWorkunits(&workunits);
	}

	static bool DeleteFile(const boost::filesystem::wpath & file)
	{
		if (boost::filesystem::exists(file))
		{
			try {
				if (boost::filesystem::remove(file))
					return true;
			} catch (const boost::filesystem::filesystem_error & ex) {
				_DBGLOG(LEVEL_WARNING, ex.what());
				_DBGLOG(LEVEL_WARNING, file.native_file_string().c_str());
			}
		}
		return false;
	}

	static bool DeleteWorkunits(const CServerConfig &config, IWorkunitVector *workunits)
	{
		CString url, user, password;
		CSoapInitialize<WsWorkunitsServiceSoapProxy> server(config.GetUrl(url), config.GetUserId(user), config.GetPassword(password));
		CStringPool stringPool;

		IWorkunitVector localWUs;

		_ns6__WUDelete request;
		ns6__EspStringArray espStringArray;
		request.Wuids = &espStringArray;
		for (unsigned i = 0; i < workunits->size(); ++i)
		{
			if (boost::algorithm::istarts_with((*workunits)[i]->GetWuid(), _T("L")))
				localWUs.push_back((*workunits)[i]);
			else
				espStringArray.Item.push_back(stringPool.Create((*workunits)[i]->GetWuid()));
		}

		if (CComPtr<IEclCC> eclcc = CreateIEclCC())
		{
			for(IWorkunitVector::const_iterator itr = localWUs.begin(); itr != localWUs.end(); ++itr)
			{
				boost::filesystem::wpath path = eclcc->GetWorkingFolder();
				std::_tstring wuid = itr->get()->GetWuid();
				if (DeleteFile(path / (wuid + _T(".exe"))))
				{
					itr->get()->ForceBroadcast(WUEventDeleted);
					std::_tstring id = itr->get()->GetID();
					ClearWorkunitCache(id);
					g_monitor.Clear(wuid);

					DeleteFile(path / (wuid + _T(".exe.manifest")));
					DeleteFile(path / (wuid + _T(".log")));
					DeleteFile(path / (wuid + _T("_LINK.log")));
					DeleteFile(path / (wuid + _T(".xml")));
					DeleteFile(path / (wuid + _T("-result.xml")));
				}
			}
		}

		_ns6__WUDeleteResponse response;
		ESP_EXCEPTION_LOG3(response.Exceptions);
		if (server.WUDelete(&request, &response) == SOAP_OK)
		{
			//Notify listeners about deletion.
			for (IWorkunitVector::iterator itr = workunits->begin(); itr != workunits->end(); ++itr)
			{
				itr->get()->ForceBroadcast(WUEventDeleted);
			}

			//remove these guys from global cache(s)
			for (std::size_t i = 0; i < workunits->size(); ++i)
			{
				std::_tstring id = workunits->operator [](i)->GetID();
				ClearWorkunitCache(id);

				std::_tstring wuid = workunits->operator [](i)->GetWuid();
				g_monitor.Clear(wuid);
			}
			return true;
		}
		else
			_DBGLOG(config.GetUrl(url), LEVEL_WARNING, server.GetClientErrorMsg());

		return false;
	}
};
#else
#if _COMMS_VER < 700000
using namespace WsWorkunits;
//  ===========================================================================
template <typename TClient = CSoapSocketClientT<> >
class CWsWorkunitsT2 : 
	public CWsWorkunitsT<TClient>
{
public:
	inline HRESULT WUUpdate2(
		BSTR Wuid, 
		int State, bool IncludeState,  
		int StateOrig, bool IncludeStateOrig, 
		BSTR Jobname, bool IncludeJobname, 
		BSTR JobnameOrig, bool IncludeJobnameOrig, 
		BSTR QueryText, bool IncludeQueryText, 
		int Action, bool IncludeAction, 
		BSTR Description, bool IncludeDescription, 
		BSTR DescriptionOrig, bool IncludeDescriptionOrig, 
		bool AddDrilldownFields, bool IncludeAddDrilldownFields, 
		int ResultLimit, bool IncludeResultLimit, 
		bool Protected, bool IncludeProtected, 
		bool ProtectedOrig, bool IncludeProtectedOrig, 
		int PriorityClass, bool IncludePriorityClass, 
		int PriorityLevel, bool IncludePriorityLevel, 
		BSTR Scope, bool IncludeScope, 
		BSTR ScopeOrig, bool IncludeScopeOrig, 
		BSTR ClusterSelection, bool IncludeClusterSelection, 
		BSTR ClusterOrig, bool IncludeClusterOrig, 
		DebugValue* DebugValues, int __DebugValues_nSizeIs, bool IncludeDebugValues, 
		ApplicationValue* ApplicationValues, int __ApplicationValues_nSizeIs, bool IncludeApplicationValues,
#if _COMMS_VER < 67205
		EspException** Exceptions, int* __Exceptions_nSizeIs, 
#else
		ArrayOfEspException* Exceptions, 
#endif
		ECLWorkunit* Workunit
		)
	{
#if _COMMS_VER < 67205
		if ( Exceptions == NULL )
			return E_POINTER;
		if( __Exceptions_nSizeIs == NULL )
			return E_POINTER;
#endif
		if ( Workunit == NULL )
			return E_POINTER;

		HRESULT __atlsoap_hr = InitializeSOAP(NULL);
		if (FAILED(__atlsoap_hr))
		{
			SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
			return __atlsoap_hr;
		}

		CleanupClient();

		CComPtr<IStream> __atlsoap_spReadStream;
		__CWsWorkunits_WUUpdate_struct __params;
		memset(&__params, 0x00, sizeof(__params));
		__params.Wuid = Wuid;
		if (IncludeState)
			__params.State = State;
#if _COMMS_VER < 55400
#else
		if (IncludeStateOrig)
			__params.StateOrig = StateOrig;
		if (IncludeJobnameOrig)
			__params.JobnameOrig = JobnameOrig;
		if (IncludeDescriptionOrig)
			__params.DescriptionOrig = DescriptionOrig;
		if (IncludeProtectedOrig)
			__params.ProtectedOrig = ProtectedOrig;
		if (IncludeScopeOrig)
			__params.ScopeOrig = ScopeOrig;
#endif
		if (IncludeJobname)
			__params.Jobname = Jobname;
		if (IncludeQueryText)
			__params.QueryText = QueryText;
		if (IncludeAction)
			__params.Action = Action;
		if (IncludeDescription)
			__params.Description = Description;
		if (IncludeAddDrilldownFields)
			__params.AddDrilldownFields = AddDrilldownFields;
		if (IncludeResultLimit)
			__params.ResultLimit = ResultLimit;
		if (IncludeProtected)
			__params.Protected = Protected;
		if (IncludePriorityClass)
			__params.PriorityClass = PriorityClass;
		if (IncludePriorityLevel)
			__params.PriorityLevel = PriorityLevel;
		if (IncludeScope)
			__params.Scope = Scope;
#if _COMMS_VER < 58800
#else
		if (IncludeClusterSelection)
			__params.ClusterSelection = ClusterSelection;
		if (IncludeClusterOrig)
			__params.ClusterOrig = ClusterOrig;
#endif
		if (IncludeDebugValues)
		{
			__params.DebugValues = DebugValues;
			__params.__DebugValues_nSizeIs = __DebugValues_nSizeIs;
		}
		if (IncludeApplicationValues)
		{
			__params.ApplicationValues = ApplicationValues;
			__params.__ApplicationValues_nSizeIs = __ApplicationValues_nSizeIs;
		}

		__atlsoap_hr = SetClientStruct(&__params, 3);
		if (FAILED(__atlsoap_hr))
		{
			SetClientError(SOAPCLIENT_OUTOFMEMORY);
			goto __skip_cleanup;
		}

		__atlsoap_hr = GenerateResponse(GetWriteStream());
		if (FAILED(__atlsoap_hr))
		{
			SetClientError(SOAPCLIENT_GENERATE_ERROR);
			goto __skip_cleanup;
		}

		__atlsoap_hr = SendRequest(_T("SOAPAction: \"WsWorkunits/WUUpdate\?ver_=1.12\"\r\n"));
		if (FAILED(__atlsoap_hr))
		{
			goto __skip_cleanup;
		}
		__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
		if (FAILED(__atlsoap_hr))
		{
			SetClientError(SOAPCLIENT_READ_ERROR);
			goto __skip_cleanup;
		}

		// cleanup any in/out-params and out-headers from previous calls
		Cleanup();
		__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
		if (FAILED(__atlsoap_hr))
		{
			SetClientError(SOAPCLIENT_PARSE_ERROR);
			goto __cleanup;
		}

#if _COMMS_VER < 67205
		*Exceptions = __params.Exceptions;
		*__Exceptions_nSizeIs = __params.__Exceptions_nSizeIs;
#else
		*Exceptions = __params.Exceptions;
#endif
		*Workunit = __params.Workunit;
		goto __skip_cleanup;

__cleanup:
		Cleanup();
__skip_cleanup:
		ResetClientState(true);
		memset(&__params, 0x00, sizeof(__params));
		return __atlsoap_hr;
	}
};
typedef CWsWorkunitsT2<CCustomActionSecureSoapSocketClient> ServerT;
//  ===========================================================================
#else
using namespace WsECLWorkunits;
typedef CWsECLWorkunitsT<CCustomActionSecureSoapSocketClient> ServerT;
#endif

typedef WUECLAttribute ECLAttribute;
ILocalFile * CreateLocalFile(const CString & url, const CString & wuid, const LogicalFileUpload * localfile);
IWorkunit * CreateWorkunit(const CServerConfig& config, const CString & wuid, ECLTimingData *TimingData, int __TimingData_nSizeIs);
IWorkunit * CreateWorkunit(const CServerConfig& config, const CString & wuid, ECLGraph *Graphs, int __Graphs_nSizeIs);
IGraph * CreateGraph(const CString& url, const CString & wuid, const ECLGraphEx * data);

#if _COMMS_VER < 54400
#elif  _COMMS_VER < 700000
typedef WUECLAttribute ECLAttribute;
#else
typedef ECLWUECLAttribute ECLAttribute;
typedef ECLWUDetail ECLWorkunit;
typedef ECLWUDebugValue DebugValue;
typedef ECLWUApplicationValue ApplicationValue;
typedef ECLWUActionResult WUActionResult;
typedef ECLWUGraph ECLGraphEx;
typedef ECLWUException ECLException;
#endif

#if _COMMS_VER < 58800
#elif _COMMS_VER < 700000
ILocalFile * CreateLocalFile(const CString & url, const CString & wuid, const LogicalFileUpload * localfile);
#endif
IWorkunit * CreateWorkunit(const CServerConfig& config, const ECLWorkunit * data, bool resultsFetched, bool noBroadcast);
IWorkunit * CreateWorkunit(const CServerConfig& config, const CString & wuid, ECLTimingData *TimingData, int __TimingData_nSizeIs);
IWorkunit * CreateWorkunit(const CServerConfig& config, const CString & wuid, ECLGraph *Graphs, int __Graphs_nSizeIs);
IWorkunit * CreateXGMMLWorkunit(const CString & filePath, const CString & daliLabel, const CString & wuid);
IGraph * CreateGraph(const CString& url, const CString & wuid, const ECLGraphEx * data);

CServerConfig::CServerConfig( const TCHAR* url, const TCHAR* label)
{
	if ( url )
		m_url = url;
	if ( label )
		m_label = label;
}
CServerConfig::CServerConfig(const CServerConfig& other)
{
	*this = other;
}

CString& CServerConfig::GetUrl(CString& url) const
{
	if ( m_url.IsEmpty() )
		url = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_ATTRIBUTE);
	else
		url = m_url;
	return url;
}
CString& CServerConfig::GetUserId(CString& userId) const
{
	if ( m_userId.IsEmpty() )
		userId = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_USER);
	else
		userId = m_userId;
	return userId;
}
CString& CServerConfig::GetPassword(CString& password) const
{
	if ( m_password.IsEmpty() )
		password = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_PASSWORD);
	else
		password = m_password;
	return password;
}

template<typename T>
static void InitDaliServer(const CServerConfig& config, T& server)
{
	CString url,userId,password;
	server.SetUrl(config.GetUrl(url));
	server.SetPassword(config.GetPassword(password));
	server.SetUser(config.GetUserId(userId));
}

class CStringBool : public clib::CLockableUnknown
{
protected:
	typedef std::map<std::_tstring, bool> MonitorMap;
	MonitorMap m_monitor;

public:
	void Clear()
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		m_monitor.clear();
	}
	void Clear(const std::_tstring & wuid)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		m_monitor.erase(wuid);
		ATLTRACE(_T("WU Monitor size(%u)\r\n"), m_monitor.size());
	}
	bool Start(const std::_tstring & wuid)	//returns false if wuid is already being monitored.
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		if (m_monitor[wuid] == true)
			return false;
		m_monitor[wuid] = true;
		return true;
	}
	void Stop(const std::_tstring & wuid)	
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		//set it to false only if found (don't create the map entry)
		MonitorMap::iterator f = m_monitor.find(wuid);
		if ( f != m_monitor.end() )
			f->second = false;
	}
	bool Active(const std::_tstring & wuid)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		MonitorMap::iterator f = m_monitor.find(wuid);
		return( f != m_monitor.end() && f->second == true );
	}
} g_monitor;

#define MAX_RESULTDEPTH 5
class ResultDepth
{
public:
	static LONG value;
	ResultDepth()
	{
		::InterlockedIncrement(&value);
	}
	~ResultDepth()
	{
		::InterlockedDecrement(&value);
	}
};
LONG ResultDepth::value = 0;

signed GetResultData(const CServerConfig& config, const CString & pWuid, int sequence, __int64 start, int count, ITable * result, __int64 &total) 
{
	CComInitialize com;
	ServerT server;
	InitDaliServer(config, server);

	CComBSTR wuid = CT2W(pWuid, CP_UTF8);
	CComBSTR datasetXML;

	CComBSTR logicalName = _T("");
	CComBSTR cluster = _T("");
	CComBSTR name = _T("");

	CStructArrayOut<ECLWorkunit> results;
#if _COMMS_VER < 491
	if (server.WUResult(&wuid, &sequence, &start, &count, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &datasetXML, &total) == S_OK)
#elif _COMMS_VER < 50514
	if (server.WUResult(&wuid, &sequence, &start, &count, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &total, &datasetXML) == S_OK)
#elif _COMMS_VER < 53306
	if (server.WUResult(&wuid, &sequence, &start, &count, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &name, &total, &datasetXML) == S_OK)
#elif _COMMS_VER < 53600
	if (server.WUResult(&wuid, &sequence, logicalName, &start, &count, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &name, &total, &datasetXML) == S_OK)
#elif _COMMS_VER < 54400
	if (server.WUResult(&wuid, &sequence, &logicalName, &start, &count, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &name, &requested, &total, &datasetXML) == S_OK)
#elif _COMMS_VER < 60600
	int requested = 0;
	ESP_EXCEPTION_LOG2(EspException);
	if (server.WUResult(&wuid, &sequence, &cluster, &logicalName, &start, &count, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &name, &requested, &total, &datasetXML) == S_OK)
#elif _COMMS_VER < 67205
	int requested = 0;
	CComBSTR resultName = CT2W(_T(""));
	ESP_EXCEPTION_LOG(EspException);
	if (server.WUResult(&wuid, &sequence, resultName, &logicalName, &cluster, &start, &count, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &name, &requested, &total, &datasetXML) == S_OK)
#elif _COMMS_VER < 700000
	int requested = 0;
	CComBSTR resultName = CT2W(_T(""));
	ESP_EXCEPTION_LOG2(EspException);
	if (server.WUResult(&wuid, &sequence, resultName, &logicalName, &cluster, &start, &count, exceptions, &name, &requested, &total, &datasetXML) == S_OK)
#else
	CComBSTR resultName = CT2W(_T(""));
	ESP_STATUS_LOG;
	if (server.RetrieveResult(wuid, sequence, resultName, logicalName, false, start, count, &status.m_statusCode, &status.m_statusMessage, &datasetXML, &total) == S_OK)
		//BSTR WUID, 
		//int ResultSequenceNumber, 
		//BSTR ResultName, 
		//BSTR LogicalFileName, 
		//bool ReturnRawData, 
		//__int64 StartFrom, 
		//__int64 Count, 
		//int* StatusCode, 
		//BSTR* StatusMessage, 
		//BSTR* Result, 
		//__int64* TotalRecords

#endif
	{
		if (count > 0)
		{
			CComBSTR xml = L"<Result>";
			xml += datasetXML;
			xml += L"</Result>";
#ifdef _DEBUG
			if (static_cast<bool>(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_DEBUG_LOGRESULTXML))==true)
			{
				std::ofstream file((boost::format("c:\\temp\\resultlog_%1%_%2%_%3%.xml") % CW2A(name, CP_UTF8) % start % count).str().c_str());
				file << CW2A(xml);
			}
#endif
			if (result->LoadFrom(xml, start))
				return count;
		}
		return 0;
	}
	else
		_DBGLOG(config.GetUrl(url), LEVEL_WARNING, server.GetClientError());

	total = -1;
	count = -1;
	return -1;
}

COMMS_API void GetResultData(const CServerConfig& config, CResultDataRequestAdapt req)
{
	async_datacomplete_signal_type on_completion;
	on_completion.connect(req->m_slot);
	req->m_request_sent = true;
	req->m_count = req->m_result->GetResultData(req->m_start, req->m_count, req->m_resultTable);
	on_completion(req->m_start, req->m_count, req->m_result->GetTotal(), true);
}

int ParseDebugString(const CString & debugString, bool archive, bool noCommonPrivateAttributes, CStructArrayIn<DebugValue> & debugValues, CStructArrayIn<ApplicationValue> & appValues)
{
	std::_tstring debugStr;
	if (archive)
	{
		debugStr += std::_tstring(APP_DATA_ARCHIVE) + _T("=1\r\n");
	}
	if (noCommonPrivateAttributes)
	{
		debugStr += std::_tstring(APP_DATA_COMMONPRIVATEATTRIBUTES) + _T("=0\r\n");
	}
	debugStr += static_cast<const TCHAR *>(debugString);

	typedef std::vector<std::_tstring> split_vector_type;
	split_vector_type SplitVec; 
	boost::algorithm::split(SplitVec, debugStr, boost::algorithm::is_any_of("\r\n"), boost::algorithm::token_compress_on);

	debugValues.Resize(SplitVec.size() + 1);
	appValues.Resize(SplitVec.size() + 1);
	int i = 0;
	for(split_vector_type::iterator itr = SplitVec.begin(); itr != SplitVec.end(); ++itr)
	{
		std::_tstring lhs, rhs;
		split_vector_type LineVec; 
		boost::algorithm::split(LineVec, *itr, boost::algorithm::is_any_of("="), boost::algorithm::token_compress_on);
		split_vector_type::iterator l_itr = LineVec.begin(); 
		if (l_itr != LineVec.end())
		{
			lhs = *l_itr;
			++l_itr;
		}
		if (l_itr != LineVec.end())
		{
			rhs = *l_itr;
		}
		debugValues.SetBSTR(debugValues.GetItem(i)->Name, lhs);
		debugValues.SetBSTR(debugValues.GetItem(i)->Value, rhs);
		lhs = APP_DATA_DEBUG + lhs;
		appValues.SetBSTR(appValues.GetItem(i)->Application, APP_DATA_APPNAME);
#if _COMMS_VER < 700000
		appValues.SetBSTR(appValues.GetItem(i)->Name, lhs);
		appValues.SetBSTR(appValues.GetItem(i)->Value, rhs);
#else
		appValues.SetBSTR(appValues.GetItem(i)->KeyName, lhs);
		appValues.SetBSTR(appValues.GetItem(i)->KeyValue, rhs);
#endif
		++i;
	}
	return i;
}

class RoxieXMLHandler : public StackHandlerImpl
{
public:
	std::_tstring m_queryName;

	virtual HRESULT STDMETHODCALLTYPE startElement(
		const wchar_t *pwchNamespaceUri,
		int cchNamespaceUri,
		const wchar_t *pwchLocalName,
		int cchLocalName,
		const wchar_t *pwchQName,
		int cchQName,
		ISAXAttributes *pAttributes)
	{
		__super::startElement(pwchNamespaceUri, cchNamespaceUri, pwchLocalName, cchLocalName, pwchQName, cchQName, pAttributes);
		if (m_queryName.length() == 0)
		{
			const Element & e = m_stack.top();
			m_queryName = e.m_tag;
		}
		return S_OK;
	}
};

bool UpdateWU(const CServerConfig& config, ECLWorkunit &wu, WUState state, const CString & /*cluster*/, const CString & /*queue*/, WUAction action, const CString & ecl, int resultLimit, const CString & _debugString, bool archive, bool noCommonPrivateAttributes, bool debug)
{
	ServerT server;
	InitDaliServer(config, server);

	CComBSTR eclStr = CT2W(ecl, CP_UTF8);
#if _COMMS_VER < 505
	ECLWorkunit resultWU;
	if (server.WUUpdate(wu.Wuid, state, wu.Jobname, eclStr, action , wu.Description, wu.AddDrilldownFields, resultLimit, wu.Protected, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &resultWU) == S_OK)
	{
		AtlCleanupValue(&resultWU);
#else
	CComBSTR Scope = _T("");
	CStructArrayIn<DebugValue> debugValues;
	CStructArrayIn<ApplicationValue> appValues;
	CString debugString = _debugString;
	if (debug)
	{
		if (debugString.GetLength())
			debugString += _T("\r\n");
		debugString += _T("Debug=1");
	}
	int i = ParseDebugString(debugString, archive, noCommonPrivateAttributes, debugValues, appValues);

#if _COMMS_VER < 51300
	debugValues.SetBSTR(debugValues.GetItem(i)->Name, RESULTLIMIT_KEY);
	debugValues.SetBSTR(debugValues.GetItem(i)->Value, boost::lexical_cast<std::_tstring>(resultLimit).c_str());
#endif
	debugValues.SetBSTR(debugValues.GetItem(i)->Name, _T(""));
	debugValues.SetBSTR(debugValues.GetItem(i)->Value, _T(""));
	appValues.SetBSTR(appValues.GetItem(i)->Application, APP_DATA_APPNAME);

#if _COMMS_VER < 700000
	appValues.SetBSTR(appValues.GetItem(i)->Name, APP_DATA_VERSION);
	appValues.SetBSTR(appValues.GetItem(i)->Value, static_cast<const TCHAR *>(CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_APP_VERSION))));
#else
	appValues.SetBSTR(appValues.GetItem(i)->KeyName, APP_DATA_VERSION);
	appValues.SetBSTR(appValues.GetItem(i)->KeyValue, static_cast<const TCHAR *>(CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_APP_VERSION))));
#endif
#if _COMMS_VER <53306
	if (server.WUUpdate(wu.Wuid, state, wu.Jobname, eclStr, action, wu.Description, wu.AddDrilldownFields, resultLimit, wu.Protected, 0, 0, Scope, debugValues.GetArray(), debugValues.GetCount(), appValues.GetArray(), appValues.GetCount(), exceptions.GetArrayAddress(), exceptions.GetCountAddress()) == S_OK)
	{
#elif _COMMS_VER < 55800
	ECLWorkunit outWorkunit;
	ESP_EXCEPTION_LOG2(EspException);
	if (server.WUUpdate(wu.Wuid, state, wu.Jobname, eclStr, action, wu.Description, wu.AddDrilldownFields, resultLimit, wu.Protected, 0, 0, Scope, debugValues.GetArray(), debugValues.GetCount(), appValues.GetArray(), appValues.GetCount(), exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &outWorkunit) == S_OK)
	{
		AtlCleanupValueEx(&outWorkunit, &crtHeap);
#elif _COMMS_VER < 58300
	ECLWorkunit outWorkunit;
	int StateOrig = 0;
	CComBSTR JobnameOrig = _T("");
	CComBSTR DescriptionOrig = _T("");
	bool ProtectedOrig = false;
	int PriorityClass = 0;
	int PriorityLevel = 0;
	CComBSTR ScopeOrig = _T("");
	ESP_EXCEPTION_LOG2(EspException);
	if (server.WUUpdate(
		wu.Wuid, 
		state, 
		StateOrig, 
		wu.Jobname, 
		JobnameOrig, 
		eclStr, 
		action, 
		wu.Description, 
		DescriptionOrig, 
		wu.AddDrilldownFields, 
		resultLimit, 
		wu.Protected, 
		ProtectedOrig, 
		PriorityClass, 
		PriorityLevel, 
		Scope, 
		ScopeOrig, 
		debugValues.GetArray(), 
		debugValues.GetCount(), 
		appValues.GetArray(), 
		appValues.GetCount(), 
		exceptions.GetArrayAddress(), 
		exceptions.GetCountAddress(), 
		&outWorkunit
		) == S_OK)
	{
		AtlCleanupValueEx(&outWorkunit, &crtHeap);
#elif _COMMS_VER < 67205
	ECLWorkunit outWorkunit;
	int StateOrig = 0;
	CComBSTR Jobname = wu.Jobname;
	CComBSTR JobnameOrig = _T("");
	CComBSTR DescriptionOrig = _T("");
	bool ProtectedOrig = false;
	int PriorityClass = 0;
	int PriorityLevel = 0;
	CComBSTR ScopeOrig = _T("");
	CComBSTR ClusterSelection = _T("");	
	CComBSTR ClusterOrig = _T("");

	ESP_EXCEPTION_LOG(EspException);
	if (server.WUUpdate(
		wu.Wuid, 
		state, 
		StateOrig, 
		Jobname, 
		JobnameOrig, 
		eclStr, 
		action, 
		wu.Description, 
		DescriptionOrig, 
		wu.AddDrilldownFields, 
		resultLimit, 
		wu.Protected, 
		ProtectedOrig, 
		PriorityClass, 
		PriorityLevel, 
		Scope, 
		ScopeOrig, 
		ClusterSelection,		
		ClusterOrig,		
		debugValues.GetArray(), 
		debugValues.GetCount(), 
		appValues.GetArray(), 
		appValues.GetCount(), 
		exceptions.GetArrayAddress(), 
		exceptions.GetCountAddress(), 
		&outWorkunit
		) == S_OK)
	{
		AtlCleanupValueEx(&outWorkunit, &crtHeap);
#elif _COMMS_VER < 68200
	ECLWorkunit outWorkunit;
	int StateOrig = 0;
	CComBSTR Jobname = wu.Jobname;
	CComBSTR JobnameOrig = _T("");
	CComBSTR DescriptionOrig = _T("");
	bool ProtectedOrig = false;
	int PriorityClass = 0;
	int PriorityLevel = 0;
	CComBSTR ScopeOrig = _T("");
	CComBSTR ClusterSelection = _T("");	
	CComBSTR ClusterOrig = _T("");
	CComBSTR xmlParams = _T("");

	//  ECL or XML
	CComInitialize com;
	CComPtr<ISAXXMLReader> pRdr; 
	HRESULT hr = pRdr.CoCreateInstance(CLSID_SAXXMLReader30);
	CComPtr <RoxieXMLHandler> pMc = new RoxieXMLHandler();
	hr = pRdr->putContentHandler(pMc); 
	CComPtr<SAXErrorHandler> pErr = new SAXErrorHandler();
	if(SUCCEEDED(hr))
		pRdr->putErrorHandler(pErr);

	if (hr == S_OK)
	{
		hr = pRdr->parse(CComVariant(ecl));
		if (SUCCEEDED(hr))
		{
			xmlParams = CT2W(ecl, CP_UTF8);
			action = WUActionExecuteExisting;
			Jobname = CT2W(pMc->m_queryName.c_str(), CP_UTF8);
		}
	}
	ESP_EXCEPTION_LOG2(EspException);
	if (server.WUUpdate(
		wu.Wuid, 
		state, 
		StateOrig, 
		Jobname, 
		JobnameOrig, 
		eclStr, 
		action, 
		wu.Description, 
		DescriptionOrig, 
		wu.AddDrilldownFields, 
		resultLimit, 
		wu.Protected, 
		ProtectedOrig, 
		PriorityClass, 
		PriorityLevel, 
		Scope, 
		ScopeOrig, 
		ClusterSelection,		
		ClusterOrig,		
		xmlParams,
		debugValues.GetArray(), debugValues.GetCount(), 
		appValues.GetArray(), appValues.GetCount(), 
		exceptions, 
		&outWorkunit
		) == S_OK)
	{
		AtlCleanupValueEx(&outWorkunit, &crtHeap);
#elif _COMMS_VER < 700000
	ECLWorkunit outWorkunit;
	int StateOrig = 0;
	CComBSTR Jobname = wu.Jobname;
	CComBSTR JobnameOrig = _T("");
	CComBSTR DescriptionOrig = _T("");
	bool ProtectedOrig = false;
	int PriorityClass = 0;
	int PriorityLevel = 0;
	CComBSTR ScopeOrig = _T("");
	CComBSTR ClusterSelection = _T("");	
	CComBSTR ClusterOrig = _T("");
	CComBSTR xmlParams = _T("");
	CComBSTR thorSlaveIP = _T("");

	//  ECL or XML
	CComInitialize com;
	CComPtr<ISAXXMLReader> pRdr; 
	HRESULT hr = pRdr.CoCreateInstance(CLSID_SAXXMLReader30);
	CComPtr <RoxieXMLHandler> pMc = new RoxieXMLHandler();
	hr = pRdr->putContentHandler(pMc); 
	CComPtr<SAXErrorHandler> pErr = new SAXErrorHandler();
	if(SUCCEEDED(hr))
		pRdr->putErrorHandler(pErr);

	if (hr == S_OK)
	{
		hr = pRdr->parse(CComVariant(ecl));
		if (SUCCEEDED(hr))
		{
			xmlParams = CT2W(ecl, CP_UTF8);
			action = WUActionExecuteExisting;
			Jobname = CT2W(pMc->m_queryName.c_str(), CP_UTF8);
		}
	}
	ESP_EXCEPTION_LOG2(EspException);
	if (server.WUUpdate(
		wu.Wuid, 
		state, 
		StateOrig, 
		Jobname, 
		JobnameOrig, 
		eclStr, 
		action, 
		wu.Description, 
		DescriptionOrig, 
		wu.AddDrilldownFields, 
		resultLimit, 
		wu.Protected, 
		ProtectedOrig, 
		PriorityClass, 
		PriorityLevel, 
		Scope, 
		ScopeOrig, 
		ClusterSelection,		
		ClusterOrig,		
		xmlParams,
		thorSlaveIP,
		debugValues.GetArray(), debugValues.GetCount(), 
		appValues.GetArray(), appValues.GetCount(), 
		exceptions, 
		&outWorkunit
		) == S_OK)
	{
		AtlCleanupValueEx(&outWorkunit, &crtHeap);
#else
	ECLWorkunit outWorkunit;
	CComBSTR StateOrig = _T("");
	CComBSTR JobnameOrig = _T("");
	CComBSTR DescriptionOrig = _T("");
	bool ProtectedOrig = false;
	CComBSTR ScopeOrig = _T("");
	CComBSTR ClusterSelection = _T("");	
	CComBSTR ClusterOrig = _T("");
	CComBSTR eclStrOrig = _T("");

	ECLWUUpdatableAttributes OldAttributes;
	OldAttributes.Cluster = ClusterOrig;
	OldAttributes.JobName = JobnameOrig;
	OldAttributes.Description = DescriptionOrig;
	OldAttributes.Scope = ScopeOrig;
	OldAttributes.State = StateOrig;
	OldAttributes.QueryText = eclStrOrig;
	OldAttributes.PriorityClass = 0;
	OldAttributes.IsProtected = ProtectedOrig;

	ECLWUUpdatableAttributes NewAttributes; 
	NewAttributes.Cluster = ClusterSelection;
	NewAttributes.JobName = wu.Attributes.JobName;
	NewAttributes.Description = wu.Attributes.Description;
	NewAttributes.Scope = Scope;
	ATLASSERT(!_T("todo"));
	//NewAttributes.State = State;
	NewAttributes.QueryText = eclStr;
	ATLASSERT(!_T("todo"));
	//NewAttributes.PriorityClass = PriorityClass;
	NewAttributes.IsProtected = wu.Status.IsProtected;

	ESP_STATUS_LOG;
	if (server.UpdateWorkunit(
		wu.WUID, 
		OldAttributes,
		NewAttributes,
		&status.m_statusCode,
		&status.m_statusMessage,
		&outWorkunit
		) == S_OK)
	{
		AtlCleanupValueEx(&outWorkunit, &crtHeap);
#endif
#endif
		return true;
	}
	else
		_DBGLOG(config.GetUrl(url), LEVEL_WARNING, server.GetClientError());
	return false;
}

BSTR * CreateBstrArray(const StdStringVector &strArr)
{
	unsigned arrSize = strArr.size();
	BSTR *arr = (BSTR *)malloc(arrSize * sizeof(BSTR));
	for (unsigned i = 0; i < arrSize; ++i)
	{
		CString c(strArr[i].c_str());
		arr[i] = c.AllocSysString();
	}
	return arr;
}

BSTR * CreateBstrArray(IWorkunitVector *workunits)
{
	unsigned arrSize = workunits->size();
	BSTR *arr = (BSTR *)malloc(arrSize * sizeof(BSTR));
	for (unsigned i = 0; i < arrSize; ++i)
		arr[i] = ::SysAllocString(CT2W(workunits->operator [](i)->GetWuid(), CP_UTF8));
	return arr;
}

void DestroyBstrArray(BSTR * arr, unsigned arrSize)
{
	for (unsigned i = 0; i < arrSize; ++i)
		SysFreeString(arr[i]);
	free(arr);
}

bool ProtectWorkunits(const CString & url, IWorkunitVector *workunits, bool protect)
{
	bool retVal = false;
#if _COMMS_VER < 491
	_DBGLOG(config.GetUrl(url), LEVEL_WARNING, _T("Protect unsupported on this server."));
#else
	CComInitialize com;
	ServerT server;
	ATLASSERT(url.CompareNoCase(CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT))) == 0);
	server.SetUrl(url);

	BSTR *arr = CreateBstrArray(workunits);
#if _COMMS_VER < 55000
	if (server.WUProtect(arr, workunits->size(), protect, exceptions.GetArrayAddress(), exceptions.GetCountAddress()) == S_OK)
#elif _COMMS_VER < 67205
	CStructArrayOut<WUActionResult> actionResults;
	ESP_EXCEPTION_LOG(EspException);
	if (server.WUProtect(arr, workunits->size(), protect, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), actionResults.GetArrayAddress(), actionResults.GetCountAddress()) == S_OK)
#elif _COMMS_VER < 700000
	CStructArrayOut<WUActionResult> actionResults;
	ESP_EXCEPTION_LOG2(EspException);
	if (server.WUProtect(arr, workunits->size(), protect, exceptions, actionResults.GetArrayAddress(), actionResults.GetCountAddress()) == S_OK)
#else
	CStructArrayOut<WUActionResult> actionResults;
	ESP_STATUS_LOG;
	if (server.ProtectWorkunits(arr, workunits->size(), protect, &status.m_statusCode, &status.m_statusMessage, actionResults.GetArrayAddress(), actionResults.GetCountAddress()) == S_OK)
#endif
	{
		retVal = true;
	}
	else
		_DBGLOG(config.GetUrl(url), LEVEL_WARNING, server.GetClientError());

	DestroyBstrArray(arr, workunits->size());
#endif
	return retVal;
}

bool FollowupWorkunits(const CString & url, IWorkunitVector *workunits, bool followup)
{
	bool retVal = false;
	CComInitialize com;

	for(IWorkunitVector::iterator itr = workunits->begin(); itr != workunits->end(); ++itr)
	{
		ServerT server;
		ATLASSERT(url.CompareNoCase(CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT))) == 0);
		server.SetUrl(url);

		CComBSTR wuid = CT2W(itr->get()->GetWuid(), CP_UTF8);
		CStructArrayIn<WsWorkunits::ApplicationValue> appValues(1);
		appValues.SetBSTR(appValues.GetItem(0)->Application, APP_DATA_APPNAME);
		std::_tstring followup_key = APP_DATA_FOLLOWUP;
		followup_key += CString((GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_USER)));
		appValues.SetBSTR(appValues.GetItem(0)->Name, followup_key);
		appValues.SetBSTR(appValues.GetItem(0)->Value, followup ? _T("true") : _T("false"));

#if _COMMS_VER < 53306
		ESP_EXCEPTION_LOG(EspException);
		if (server.WUUpdate(wuid, followup, appValues.GetArray(), appValues.GetCount(), exceptions.GetArrayAddress(), exceptions.GetCountAddress()) == S_OK)
		{
#elif _COMMS_VER < 67205
		WsWorkunits::ECLWorkunit outWorkunit;
		ESP_EXCEPTION_LOG(EspException);
		if (server.WUUpdate2(
			//Required
			wuid, 
			//Optional
			0, false,
			0, false,
			NULL, false,
			NULL, false,
			NULL, false,
			0, false,
			NULL, false,
			NULL, false,
			NULL, false,
			0, false,
			followup, true,
			false, false,
			0, false,
			0, false,
			NULL, false,
			NULL, false,
			NULL, false, 
			NULL, false,
			NULL, 0, false, 
			appValues.GetArray(), appValues.GetCount(), true, 
			//Output
			exceptions.GetArrayAddress(), exceptions.GetCountAddress(), 
			&outWorkunit) == S_OK)
		{
			AtlCleanupValueEx(&outWorkunit, &crtHeap);
#elif _COMMS_VER < 700000
		WsWorkunits::ECLWorkunit outWorkunit;
		ESP_EXCEPTION_LOG2(EspException);
		if (server.WUUpdate2(
			//Required
			wuid, 
			//Optional
			0, false,
			0, false,
			NULL, false,
			NULL, false,
			NULL, false,
			0, false,
			NULL, false,
			NULL, false,
			NULL, false,
			0, false,
			followup, true,
			false, false,
			0, false,
			0, false,
			NULL, false,
			NULL, false,
			NULL, false, 
			NULL, false,
			NULL, 0, false, 
			appValues.GetArray(), appValues.GetCount(), true, 
			//Output
			exceptions, 
			&outWorkunit) == S_OK)
		{
			AtlCleanupValueEx(&outWorkunit, &crtHeap);
#else
		ATLASSERT(!_T("todo"));
		if (false)
		{
#endif
			retVal = true;
		}
		else
			_DBGLOG(config.GetUrl(url), LEVEL_WARNING, server.GetClientError());
	}
	return retVal;
}

bool AbortWorkunits(const CString &url, IWorkunitVector *workunits)
{
	CComInitialize com;
	ServerT server;
	ATLASSERT(url.CompareNoCase(CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT))) == 0);
	server.SetUrl(url);

	BSTR *arr = CreateBstrArray(workunits);
#if _COMMS_VER < 55000
	if (server.WUAbort(arr, workunits->size(), exceptions.GetArrayAddress(), exceptions.GetCountAddress()) == S_OK)
#elif _COMMS_VER < 57600
	CStructArrayOut<WUActionResult> actionResults;
	ESP_EXCEPTION_LOG2(EspException);
	if (server.WUAbort(arr, workunits->size(), exceptions.GetArrayAddress(), exceptions.GetCountAddress(), actionResults.GetArrayAddress(), actionResults.GetCountAddress()) == S_OK)
#elif _COMMS_VER < 67205
	int BlockTillFinishTimer = false;
	CStructArrayOut<WUActionResult> actionResults;
	ESP_EXCEPTION_LOG(EspException);
	if (server.WUAbort(arr, workunits->size(), BlockTillFinishTimer, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), actionResults.GetArrayAddress(), actionResults.GetCountAddress()) == S_OK)
#elif _COMMS_VER < 700000
	int BlockTillFinishTimer = false;
	CStructArrayOut<WUActionResult> actionResults;
	ESP_EXCEPTION_LOG2(EspException);
	if (server.WUAbort(arr, workunits->size(), BlockTillFinishTimer, exceptions, actionResults.GetArrayAddress(), actionResults.GetCountAddress()) == S_OK)
#else
	int BlockTillFinishTimer = false;
	CStructArrayOut<WUActionResult> actionResults;
	ESP_STATUS_LOG;
	if (server.AbortWorkunits(arr, workunits->size(), BlockTillFinishTimer, &status.m_statusCode, &status.m_statusMessage, actionResults.GetArrayAddress(), actionResults.GetCountAddress()) == S_OK)
#endif
	{
		DestroyBstrArray(arr, workunits->size());
		return true;
	}
	else
		_DBGLOG(config.GetUrl(url), LEVEL_WARNING, server.GetClientError());

	DestroyBstrArray(arr, workunits->size());
	return false;
}

bool ResubmitWorkunits(const CString &url, IWorkunitVector *workunits)
{
	CComInitialize com;
	ServerT server;
	ATLASSERT(url.CompareNoCase(CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT))) == 0);
	server.SetUrl(url);

	BSTR *arr = CreateBstrArray(workunits);
	bool Recompile = false;
#if _COMMS_VER < 57600
	ESP_EXCEPTION_LOG2(EspException);
	if (server.WUResubmit(arr, workunits->size(), Recompile, exceptions.GetArrayAddress(), exceptions.GetCountAddress()) == S_OK)
#elif _COMMS_VER < 67205
	int BlockTillFinishTimer = 0;
	ESP_EXCEPTION_LOG(EspException);
	if (server.WUResubmit(arr, workunits->size(), Recompile, BlockTillFinishTimer, exceptions.GetArrayAddress(), exceptions.GetCountAddress()) == S_OK)
#elif _COMMS_VER < 700000
	int BlockTillFinishTimer = 0;
	ESP_EXCEPTION_LOG2(EspException);
	if (server.WUResubmit(arr, workunits->size(), Recompile, BlockTillFinishTimer, exceptions) == S_OK)
#else
	int BlockTillFinishTimer = 0;
	ESP_STATUS_LOG;
	if (server.ResubmitWorkunits(arr, workunits->size(), Recompile, BlockTillFinishTimer, &status.m_statusCode, &status.m_statusMessage) == S_OK)
#endif
	{
		DestroyBstrArray(arr, workunits->size());
		return true;
	}
	else
		_DBGLOG(config.GetUrl(url), LEVEL_WARNING, server.GetClientError());

	DestroyBstrArray(arr, workunits->size());
	return false;
}

class CMonitorCount
{
public:
	static long m_Count;
	CMonitorCount()
	{
		InterlockedIncrement(&m_Count);
	}
	~CMonitorCount()
	{
		InterlockedDecrement(&m_Count);
	}
};

long CMonitorCount::m_Count = 0;

#define MAX_TRY_COUNT 8
#define MINUTE 60000
#define SECOND 1000

class CDali : public IDali, public CUnknown
{
protected:
	CString m_id;
	CServerConfig m_config;

public:
	IMPLEMENT_CUNKNOWN

	CDali(const CString & url, const CString & label) : m_config(url,label)
	{ 
		SetID();
	}

	CDali(const CServerConfig& config) : m_config(config)
	{
		SetID();
	}

	~CDali()
	{
	}

	void SetID()
	{
		m_id = m_config.m_url + _T("/") + m_config.m_label;
	}

	void InitServer(ServerT& server)
	{
		InitDaliServer(m_config,server);
	}

	const CServerConfig* GetServerConfig() const
	{
		return &m_config;
	}

	const TCHAR * GetID() const
	{
		return m_id;
	}

	const TCHAR * GetCacheID() const
	{
		return m_id;
	}

	const TCHAR * GetLabel() const
	{
		return m_config.m_label;
	}

	const TCHAR * GetUrl() const
	{
		return m_config.m_url;
	}

	void GetWorkunitsAsync(const CString &cluster, const CString &owner, const CString &state, const CString &fromUTC, const CString &toUTC, int count, const async_wucomplete_slot_type &s) const
	{
		clib::thread run(__FUNCTION__, boost::bind(&GetWorkunits2, m_config, cluster, owner, state, fromUTC, toUTC, count, s));
	}

	unsigned GetWorkunits(const CString &cluster, const CString &owner, const CString &state, const CString &fromUTC, const CString &toUTC, int count, IWorkunitVector *workunits) const
	{
		return GetWorkunits1(m_config, _T(""), cluster, owner, state, fromUTC, toUTC, count, workunits);
	}

	IWorkunit *GetWorkunitFast(const CString &wuid, bool noBroadcast = false) const
	{
		IWorkunitVector workunits;
		GetWorkunits1(m_config, wuid, _T(""), _T(""), _T(""), _T(""), _T(""), 1, &workunits, noBroadcast);
		if (workunits.size() > 0)
			return workunits[0];
		return NULL;
	}

	IWorkunit *GetWorkunit(const CString &wuid, bool noBroadcast = false) const
	{
		return GetWorkunit(m_config, wuid, noBroadcast);
	}

	IWorkunit *GetXGMMLWorkunit(const CString &filePath) const
	{
		return CreateXGMMLWorkunit(filePath, m_config.m_label, _T("007"));
	}

	signed GetResultData(const CString & wuid, int sequence, __int64 start, int count, ITable * result, __int64 &total) const
	{
		return Dali::GetResultData(m_config, wuid, sequence, start, count, result, total);
	}

	void MonitorState(const CString & _wuid) const
	{
		std::_tstring wuid(_wuid);
		clib::thread run(__FUNCTION__, boost::bind(&BlockUntilComplete, m_config, wuid));
	}

	IGraph * GetGraph(const CString &pWuid, const CString &graphName)
	{
		CComInitialize com;
		ServerT server;
		InitDaliServer(m_config, server);

		CComBSTR wuid = CT2W(pWuid, CP_UTF8);
		CStructArrayOut<ECLGraphEx> results;
#if _COMMS_VER < 65100
		ESP_EXCEPTION_LOG2(EspException);
		if (server.WUGetGraph(wuid, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), results.GetArrayAddress(), results.GetCountAddress()) == S_OK)
#elif _COMMS_VER < 67205
		CComBSTR bstrGraphName = CT2W(graphName, CP_UTF8);
		ESP_EXCEPTION_LOG(EspException);
		if (server.WUGetGraph(wuid, bstrGraphName, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), results.GetArrayAddress(), results.GetCountAddress()) == S_OK)
#elif _COMMS_VER < 700000
		CComBSTR bstrGraphName = CT2W(graphName, CP_UTF8);
		CComBSTR bstrSubGraphId = CT2W(_T(""), CP_UTF8);
		ESP_EXCEPTION_LOG2(EspException);
		if (server.WUGetGraph(wuid, bstrGraphName, bstrSubGraphId, exceptions, results.GetArrayAddress(), results.GetCountAddress()) == S_OK)
#else
		ESP_STATUS_LOG;
		if (server.RetrieveECLWUGraphInfo(wuid, &status.m_statusCode, &status.m_statusMessage, results.GetArrayAddress(), results.GetCountAddress()) == S_OK)
#endif
		{
			CString url;
			m_config.GetUrl(url);
			for (int i = 0; i < results.GetCount(); ++i)
			{
				CreateGraph(url, pWuid, results.GetItem(i));
			}
		}
		else
			_DBGLOG(config.GetUrl(url), LEVEL_WARNING, server.GetClientError());

		return NULL;
	}

	unsigned GetGraphs(const CString &pWuid)
	{
		CComInitialize com;
		ServerT server;
		InitDaliServer(m_config, server);

		CComBSTR wuid = CT2W(pWuid, CP_UTF8);
		ECLWorkunit wu;
#if _COMMS_VER < 65100
		ESP_EXCEPTION_LOG2(EspException);
		if (server.WUGetGraph(wuid, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), results.GetArrayAddress(), results.GetCountAddress()) == S_OK)
#elif _COMMS_VER < 67205
		bool canCompile = false;
		CComBSTR bstrType = _T("");
		int autoRefresh = 0;
		ESP_EXCEPTION_LOG(EspException);
		if (server.WUInfo(wuid, bstrType, false, true, false, false, false, false, false, false, false, true, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &wu, &autoRefresh, &canCompile) == S_OK)
#elif _COMMS_VER < 68200
		bool canCompile = false;
		CComBSTR bstrType = _T("");
		int autoRefresh = 0;
		ESP_EXCEPTION_LOG2(EspException);
		if (server.WUInfo(wuid, bstrType, false, true, false, false, false, false, false, false, false, true, exceptions, &wu, &autoRefresh, &canCompile) == S_OK)
#elif _COMMS_VER < 700000
		bool canCompile = false;
		CComBSTR bstrType = _T("");
		CComBSTR thorSlaveIP = _T("");
		int autoRefresh = 0;
		ESP_EXCEPTION_LOG2(EspException);
		if (server.WUInfo(wuid, bstrType, false, true, false, false, false, false, false, false, false, true, &thorSlaveIP, exceptions, &wu, &autoRefresh, &canCompile) == S_OK)
#else
		ESP_STATUS_LOG;
		if (server.RetrieveECLWUGraphInfo(wuid, &status.m_statusCode, &status.m_statusMessage, results.GetArrayAddress(), results.GetCountAddress()) == S_OK)
#endif
		{
			if (wu.Wuid)
			{
				CreateWorkunit(m_config, pWuid, wu.Graphs, wu.__Graphs_nSizeIs);
				return wu.__Graphs_nSizeIs;
			}
		}
		else
			_DBGLOG(config.GetUrl(url), LEVEL_WARNING, server.GetClientError());

		return 0;
	}

	unsigned GetGraphTimings(const CString &pWuid, CGraphTimingVector & results)
	{
		CComInitialize com;
		ServerT server;
		InitDaliServer(m_config, server);

		CComBSTR wuid = CT2W(pWuid, CP_UTF8);
		ECLWorkunit workunit;
#if _COMMS_VER < 67205
		ESP_EXCEPTION_LOG(EspException);
		if (server.WUGraphTiming(wuid, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &workunit) == S_OK)
#elif _COMMS_VER < 700000
		ESP_EXCEPTION_LOG2(EspException);
		if (server.WUGraphTiming(wuid, exceptions, &workunit) == S_OK)
#else
		ESP_STATUS_LOG;
		if (server.RetrieveECLWUGraphInfo(wuid, &status.m_statusCode, &status.m_statusMessage, results.GetArrayAddress(), results.GetCountAddress()) == S_OK)
#endif
		{
			CComPtr<IWorkunit> wu = CreateWorkunit(m_config, pWuid, workunit.TimingData, workunit.__TimingData_nSizeIs);
			wu->GetGraphTimings(results);
		}
		else
			_DBGLOG(config.GetUrl(url), LEVEL_WARNING, server.GetClientError());

		return results.size();
	}

	bool AddLocalFileToWorkunit(const std::_tstring & pWuid, const std::_tstring & pName, const std::_tstring & pVal)
	{
#if _COMMS_VER < 55800
		ATLASSERT(!"Not Supported");
#elif _COMMS_VER < 67205
		CComInitialize com;
		ServerT server;
		InitDaliServer(m_config, server);

		CComBSTR Wuid = CT2W(pWuid.c_str(), CP_UTF8);
		CComBSTR Name = CT2W(pName.c_str(), CP_UTF8);
		CComBSTR Val = CT2W(pVal.c_str(), CP_UTF8);
		CComBSTR DefVal = _T("");
		int Type = 0;
		int Length = pVal.length();
		ESP_EXCEPTION_LOG(EspException);
		CComBSTR result;
		if (server.WUAddLocalFileToWorkunit(&Wuid, &Name, Val, DefVal, Type, Length, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &result) == S_OK)
		{
			return true;
		}
		else
			_DBGLOG(config.GetUrl(url), LEVEL_WARNING, server.GetClientError());
#else
		CComInitialize com;
		ServerT server;
		InitDaliServer(m_config, server);

		CComBSTR Wuid = CT2W(pWuid.c_str(), CP_UTF8);
		CComBSTR Name = CT2W(pName.c_str(), CP_UTF8);
		CComBSTR Val = CT2W(pVal.c_str(), CP_UTF8);
		CComBSTR DefVal = _T("");
		int Type = 0;
		int Length = pVal.length();
		ESP_EXCEPTION_LOG2(EspException);
		CComBSTR result;
		if (server.WUAddLocalFileToWorkunit(&Wuid, &Name, Val, DefVal, Type, Length, exceptions, &result) == S_OK)
		{
			return true;
		}
		else
			_DBGLOG(config.GetUrl(url), LEVEL_WARNING, server.GetClientError());
#endif

		return false;
	}
	unsigned GetLocalFiles(const CString &pWuid, ILocalFileVector & localfiles)
	{
		CComInitialize com;
		ServerT server;
		InitDaliServer(m_config, server);

#if _COMMS_VER < 58800
		ATLASSERT(!"Not Supported");
		return 0;
#elif _COMMS_VER < 67205
		CComBSTR wuid = CT2W(pWuid, CP_UTF8);
		ESP_EXCEPTION_LOG(EspException);
		CStructArrayOut<LogicalFileUpload> results;
		if (server.WUListLocalFileRequired(wuid, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), results.GetArrayAddress(), results.GetCountAddress()) == S_OK)
		{
			for(int i = 0; i < results.GetCount(); ++i)
			{
				CString url;
				StlLinked<ILocalFile> localfile = CreateLocalFile(m_config.GetUrl(url), pWuid, results.GetItem(i));
				localfiles.push_back(localfile);
			}
		}
		else
			_DBGLOG(config.GetUrl(url), LEVEL_WARNING, server.GetClientError());

		return results.GetCount();
#elif _COMMS_VER < 700000
		CComBSTR wuid = CT2W(pWuid, CP_UTF8);
		ESP_EXCEPTION_LOG2(EspException);
		CStructArrayOut<LogicalFileUpload> results;
		if (server.WUListLocalFileRequired(wuid, exceptions, results.GetArrayAddress(), results.GetCountAddress()) == S_OK)
		{
			for(int i = 0; i < results.GetCount(); ++i)
			{
				CString url;
				StlLinked<ILocalFile> localfile = CreateLocalFile(m_config.GetUrl(url), pWuid, results.GetItem(i));
				localfiles.push_back(localfile);
			}
		}
		else
			_DBGLOG(config.GetUrl(url), LEVEL_WARNING, server.GetClientError());

		return results.GetCount();
#else
		ATLASSERT(!_T("TODO"));
		return 0;
#endif
	}

	IWorkunit* Submit(const CString &cluster, const CString & queue, const CString & ecl, const CString & label, int resultLimit, const CString & debugSettings, bool archive, int maxRunTime, bool debug)
	{
		CComInitialize com;
		StlLinked<IWorkunit> resultWu;
		SubmitEcl(cluster, queue, WUActionRun, ecl, label, resultLimit, resultWu, debugSettings, archive, false, maxRunTime, debug);
		return resultWu.get();
	}

	IWorkunit* Schedule(const CString &cluster, const CString & queue, const CString & ecl, const CString &when, const CString & label, int resultLimit, const CString & debugSettings, bool archive)
	{
		CComInitialize com;
		StlLinked<IWorkunit> resultWu;
		ScheduleEcl(cluster, queue, ecl, when, label, resultLimit, resultWu, debugSettings, archive);
		return resultWu.get();
	}

	bool CheckComplexity(const CString &cluster, const CString & queue, const CString & ecl, const CString &module, const CString &attribute, std::_tstring & result, CEclExceptionVector & errorResults)
	{
		CComInitialize com;
		AttributeVector results;
		CComBSTR _result = _T("");
		if (CompileEcl((const TCHAR*)ecl, (const TCHAR*)module, (const TCHAR*)attribute, (const TCHAR*)queue, (const TCHAR*)cluster, _T(""), false, true, -1, _result, results, errorResults))
		{
			result = CW2T(_result);
			return true;
		}
		return false;
	}

	void GetDependancies(const CString &cluster, const CString & queue, const CString & ecl, const CString &module, const CString &attribute, AttributeVector & results, CEclExceptionVector & errorResults)
	{
		CComInitialize com;
		CComBSTR result;
		CompileEcl((const TCHAR*)ecl, (const TCHAR*)module, (const TCHAR*)attribute, (const TCHAR*)queue, (const TCHAR*)cluster, _T(""), true, false, -1, result, results, errorResults);
	}

	bool GetDependanciesTree(const std::_tstring & cluster, const std::_tstring & queue, const std::_tstring & snapshot, const std::_tstring & items, CEclExceptionVector & outExceptions)
	{
		CComInitialize com;
		ServerT server;
		InitServer(server);

		CComBSTR clusterStr = CT2W(cluster.c_str(), CP_UTF8);
		CComBSTR queueStr = CT2W(queue.c_str(), CP_UTF8);
		CComBSTR snapshotStr = CT2W(snapshot.c_str(), CP_UTF8);
		CComBSTR itemsStr = CT2W(items.c_str(), CP_UTF8);

		CStructArrayOut<ECLException> errors;
		ATLSOAP_BLOB results = {0, NULL};
#if _COMMS_VER < 62808
		if (server.WUGetDependancyTrees(clusterStr, queueStr, snapshotStr, itemsStr, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), errors.GetArrayAddress(), errors.GetCountAddress(), &results) == S_OK)
#elif _COMMS_VER < 67205
		CComBSTR timeout = _T("");
		ESP_EXCEPTION_LOG(EspException);
		if (server.WUGetDependancyTrees(clusterStr, queueStr, snapshotStr, itemsStr, timeout, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), errors.GetArrayAddress(), errors.GetCountAddress(), &results) == S_OK)
#else
		CComBSTR timeout = _T("");
		ESP_EXCEPTION_LOG2(EspException);
		if (server.WUGetDependancyTrees(clusterStr, queueStr, snapshotStr, itemsStr, timeout, exceptions, errors.GetArrayAddress(), errors.GetCountAddress(), &results) == S_OK)
#endif
		{
			processExceptions(errors, outExceptions);
			return true;
		}
		else
			_DBGLOG(config.GetUrl(url), LEVEL_WARNING, server.GetClientError());

		return false;
	}

	void processExceptions(CStructArrayOut<ECLException> & errors, CEclExceptionVector & results)
	{
		for(int i = 0; i < errors.GetCount(); ++i)
		{
			StlLinked<CEclException> e = new CEclException();
#if _COMMS_VER < 700000
			e->m_lineNo = errors.GetItem(i)->LineNo;
			e->m_code = errors.GetItem(i)->Code;
			e->m_message = errors.GetItem(i)->Message;
#else
			e->m_lineNo = errors.GetItem(i)->Line;
			e->m_code = errors.GetItem(i)->ExceptionCode;
			e->m_message = errors.GetItem(i)->ExceptionMessage;
#endif
			e->m_column = errors.GetItem(i)->Column;
			e->m_severity = errors.GetItem(i)->Severity;
#if _COMMS_VER < 499
#else
			e->m_fileName = errors.GetItem(i)->FileName;
#endif
			results.push_back(e);
		}
	}

	bool CheckSyntax(const CString &cluster, const CString & queue, const CString & module, const CString & attribute, const CString & ecl, int timeToWait, const CString & debugString, bool archive, bool noCommonPrivateAttributes, CEclExceptionVector & results)
	{
		CComInitialize com;
		ServerT server;
		InitServer(server);

		CComBSTR bsEcl = CT2W(ecl, CP_UTF8);
		CComBSTR bsModule = CT2W(module, CP_UTF8);
		CComBSTR bsAttribute = CT2W(attribute, CP_UTF8);
		CComBSTR bsQueue = CT2W(queue, CP_UTF8);
		CComBSTR bsCluster = CT2W(cluster, CP_UTF8);
		CComBSTR SnapShot = _T("");
		CStructArrayIn<DebugValue> debugValues;
		CStructArrayIn<ApplicationValue> appValues;
		ParseDebugString(debugString, archive, noCommonPrivateAttributes, debugValues, appValues);

		CStructArrayOut<ECLException> errors;
#if _COMMS_VER < 505
		if (server.WUSyntaxCheckECL(bsEcl, bsModule, bsAttribute, bsQueue, bsCluster, timeToWait, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), errors.GetArrayAddress(), errors.GetCountAddress()) == S_OK)
#elif _COMMS_VER < 58300
		ESP_EXCEPTION_LOG2(EspException);
		if (server.WUSyntaxCheckECL(bsEcl, bsModule, bsAttribute, bsQueue, bsCluster, SnapShot, timeToWait, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), errors.GetArrayAddress(), errors.GetCountAddress()) == S_OK)
#elif _COMMS_VER < 67205
		ESP_EXCEPTION_LOG(EspException);
		if (server.WUSyntaxCheckECL(bsEcl, bsModule, bsAttribute, bsQueue, bsCluster, SnapShot, timeToWait, debugValues.GetArray(), debugValues.GetCount(), exceptions.GetArrayAddress(), exceptions.GetCountAddress(), errors.GetArrayAddress(), errors.GetCountAddress()) == S_OK)
#elif _COMMS_VER < 700000
		ESP_EXCEPTION_LOG2(EspException);
		if (server.WUSyntaxCheckECL(bsEcl, bsModule, bsAttribute, bsQueue, bsCluster, SnapShot, timeToWait, debugValues.GetArray(), debugValues.GetCount(), exceptions, errors.GetArrayAddress(), errors.GetCountAddress()) == S_OK)
#else
		ESP_STATUS_LOG;
		if (server.CheckECLSyntax(bsEcl, bsModule, bsAttribute, bsQueue, bsCluster, SnapShot, timeToWait, debugValues.GetArray(), debugValues.GetCount(), &status.m_statusCode, &status.m_statusMessage, errors.GetArrayAddress(), errors.GetCountAddress()) == S_OK)
#endif
		{
			processExceptions(errors, results);
			return true;
		}
		else
			_DBGLOG(config.GetUrl(url), LEVEL_WARNING, server.GetClientError());
		return false;
	}

	bool AbortWorkunits(IWorkunitVector *workunits)
	{
		return Dali::AbortWorkunits(m_config.m_url, workunits);
	}

	bool DeleteWorkunits(IWorkunitVector *workunits)
	{
		return DeleteWorkunits(m_config, workunits);
	}

	bool ProtectWorkunits(IWorkunitVector *workunits)
	{
		return Dali::ProtectWorkunits(m_config.m_url, workunits, true);
	}

	bool UnprotectWorkunits(IWorkunitVector *workunits)
	{
		return Dali::ProtectWorkunits(m_config.m_url, workunits, false);
	}

	bool FlagFollowupWorkunits(IWorkunitVector *workunits)
	{
		return Dali::FollowupWorkunits(m_config.m_url, workunits, true);
	}

	bool ClearFollowupWorkunits(IWorkunitVector *workunits)
	{
		return Dali::FollowupWorkunits(m_config.m_url, workunits, false);
	}

	bool ResubmitWorkunits(IWorkunitVector *workunits)
	{
		return Dali::ResubmitWorkunits(m_config.m_url, workunits);
	}

	bool SetState(IWorkunitVector *workunits, WUState state)
	{
		return SetState(m_config, workunits, state);
	}
	void SetCredentials(const TCHAR* userId, const TCHAR* password)
	{
		m_config.m_userId = userId;
		m_config.m_password = password;
	}
	bool SendDebugRequest(const std::_tstring & wuid, const std::_tstring & request, std::_tstring & response)
	{
#if _COMMS_VER < 67205
		return false;
#else
		CComInitialize com;
		ServerT server;
		InitDaliServer(m_config, server);

		CComBSTR bstrWuid = CT2W(wuid.c_str(), CP_UTF8);
		CComBSTR bstrRequest = CT2W(request.c_str(), CP_UTF8);
		ESP_EXCEPTION_LOG2(EspException);
		CComBSTR bstrResponse = CT2W(_T(""), CP_UTF8);
		server.SetTimeout(12 * 60 * 60 * 1000);
		if (server.WUCDebug(bstrWuid, bstrRequest, exceptions, &bstrResponse) == S_OK)
		{
			if (bstrResponse)
			{
				response = CW2T(bstrResponse, CP_UTF8);
				return true;
			}
			else
				_DBGLOG(config.GetUrl(url), LEVEL_WARNING, _T("WUCDebug returned NULL response."));
		}
		else
			_DBGLOG(config.GetUrl(url), LEVEL_WARNING, server.GetClientError());

		return false;
#endif
	}


protected:
	bool SetState(const CServerConfig &config, IWorkunitVector *workunits, WUState state)
	{
		CComInitialize com;
		bool retVal = true;
		for(IWorkunitVector::iterator itr = workunits->begin(); itr != workunits->end(); ++itr)
		{
			ServerT server;
			InitDaliServer(config, server);

			Dali::IWorkunitAdapt& workunit = *itr;
			CComBSTR _wuid = CT2W(workunit->GetWuid(), CP_UTF8);
			ECLWorkunit wu;

			CStructArrayOut<ECLWorkunit> results;
#if _COMMS_VER < 55000
			if (server.WUInfo(_wuid, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &wu, &canCompile) == S_OK)
#elif _COMMS_VER < 57600
			int autoRefresh = 0;
			bool canCompile = false;
			ESP_EXCEPTION_LOG2(EspException);
			if (server.WUInfo(_wuid, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &wu, &autoRefresh, &canCompile) == S_OK)
#elif _COMMS_VER < 64801
			bool canCompile = false;
			CComBSTR Type = _T("");
			int autoRefresh = 0;
			ESP_EXCEPTION_LOG2(EspException);
			if (server.WUInfo(_wuid, Type, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &wu, &autoRefresh, &canCompile) == S_OK)
#elif _COMMS_VER < 67205
			bool canCompile = false;
			CComBSTR Type = _T("");
			int autoRefresh = 0;
			ESP_EXCEPTION_LOG(EspException);
			if (server.WUInfo(_wuid, Type, true, true, true, true, true, true, true, true, true, true, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &wu, &autoRefresh, &canCompile) == S_OK)
#elif _COMMS_VER < 68200
			bool canCompile = false;
			CComBSTR Type = _T("");
			int autoRefresh = 0;
			ESP_EXCEPTION_LOG2(EspException);
			if (server.WUInfo(_wuid, Type, true, true, true, true, true, true, true, true, true, true, exceptions, &wu, &autoRefresh, &canCompile) == S_OK)
#elif _COMMS_VER < 700000
			bool canCompile = false;
			CComBSTR Type = _T("");
			CComBSTR thorSlaveIP = _T("");
			int autoRefresh = 0;
			ESP_EXCEPTION_LOG2(EspException);
			if (server.WUInfo(_wuid, Type, true, true, true, true, true, true, true, true, true, true, &thorSlaveIP, exceptions, &wu, &autoRefresh, &canCompile) == S_OK)
#else
			ESP_STATUS_LOG;
			bool ArchivedOnly = false;
			if (server.GetWorkunitDetail(_wuid, ArchivedOnly, &status.m_statusCode, &status.m_statusMessage, &wu) == S_OK)
#endif
			{
#if _COMMS_VER < 700000
				retVal = UpdateWU(config, wu, state, wu.Cluster, wu.Queue, static_cast<WUAction>(wu.Action), wu.Query.Text, 100, _T(""), false, false, false) != NULL;
#else
				retVal = UpdateWU(config, wu, state, wu.Attributes.Cluster, wu.Attributes.Queue, WUActionUnknown, wu.Query.ECLText, 100, _T(""), false, false) != NULL;
#endif
				if (retVal)
				{
					//why?
#if _COMMS_VER < 700000
					IWorkunitAdapt _wu = GetWorkunit(config, wu.Wuid, false);
#else
					IWorkunitAdapt _wu = GetWorkunit(config, wu.WUID, false);
#endif
				}
			}
			else
			{
				_DBGLOG(config.GetUrl(url), LEVEL_WARNING, server.GetClientError());
				retVal = false;
			}
			if (!retVal)
				return retVal;
		}
		return retVal;
	}

	static unsigned GetWorkunits1(const CServerConfig& config, const CString &wuid, const CString &cluster, const CString &owner, const CString &state, const CString &fromUTC, const CString &toUTC, int count, IWorkunitVector *workunits, bool noBroadcast = false) 
	{
		CComInitialize com;
		ServerT server;
		InitDaliServer(config, server);

		CComBSTR outCluster = CT2W(cluster, CP_UTF8);
		CComBSTR outOwner = CT2W(owner, CP_UTF8);
		CComBSTR outState = CT2W(state, CP_UTF8);
		CComBSTR startDate = CT2W(fromUTC, CP_UTF8);
		CComBSTR endDate = CT2W(toUTC, CP_UTF8);
		CComBSTR current = _T("");
		CComBSTR next = _T("");
		CComBSTR prev = _T("");

		CStructArrayOut<ECLWorkunit> results;
#if _COMMS_VER < 51400
		ATLASSERT(wuid.GetLength() == 0);
		if (server.WUQuery(&outCluster, &outOwner, &outState, &startDate, &endDate, current, &count, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &next, &prev, results.GetArrayAddress(), results.GetCountAddress()) == S_OK)
#elif _COMMS_VER < 53306
		CComBSTR outECL = _T("");
		CComBSTR outJobname = _T("");
		ATLASSERT(wuid.GetLength() == 0);
		if (server.WUQuery(&outCluster, &outOwner, &outState, &startDate, &endDate, &outECL, &outJobname, current, &count, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &next, &prev, results.GetArrayAddress(), results.GetCountAddress()) == S_OK)
#elif _COMMS_VER < 55000
		CComBSTR outECL = _T("");
		CComBSTR outJobname = _T("");
		CComBSTR bstrWuid = CT2W(wuid, CP_UTF8);;
		CComBSTR bstrAppName = _T("");
		CComBSTR bstrAppKey = _T("");
		CComBSTR bstrAppData = _T("");
		CComBSTR bstrAfter = _T("");
		CComBSTR bstrBefore = _T("");
		if (server.WUQuery(bstrWuid, &outCluster, &outOwner, &outState, &startDate, &endDate, &outECL, &outJobname, bstrAppName, bstrAppKey, bstrAppData, bstrAfter, bstrBefore, &count, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &current, &next, results.GetArrayAddress(), results.GetCountAddress()) == S_OK)
#elif _COMMS_VER < 55100
		CComBSTR outECL = _T("");
		CComBSTR outJobname = _T("");
		CComBSTR bstrWuid = CT2W(wuid, CP_UTF8);;
		CComBSTR bstrAppName = _T("");
		CComBSTR bstrAppKey = _T("");
		CComBSTR bstrAppData = _T("");
		CComBSTR bstrAfter = _T("");
		CComBSTR bstrBefore = _T("");
		CComBSTR bstrSortBy = _T("");
		bool boolDescenfing = false;
		bool boolFirst = false;
		CComBSTR bstrBasicQuery = _T("");
		CComBSTR bstrFilters = _T("");
		if (server.WUQuery(bstrWuid, &outCluster, &outOwner, &outState, &startDate, &endDate, &outECL, &outJobname, bstrAppName, bstrAppKey, bstrAppData, bstrAfter, bstrBefore, &count, &bstrSortBy, &boolDescenfing, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &current, &next, &boolFirst, &bstrBasicQuery, &bstrFilters, results.GetArrayAddress(), results.GetCountAddress()) == S_OK)
#elif  _COMMS_VER < 57600
		CComBSTR outECL = _T("");
		CComBSTR outJobname = _T("");
		CComBSTR bstrWuid = CT2W(wuid, CP_UTF8);;
		CComBSTR bstrAppName = _T("");
		CComBSTR bstrAppKey = _T("");
		CComBSTR bstrAppData = _T("");
		CComBSTR bstrAfter = _T("");
		CComBSTR bstrBefore = _T("");
		CComBSTR bstrSortBy = _T("");
		bool boolDescenfing = false;
		int numWUs = 999;
		bool boolFirst = false;
		CComBSTR bstrBasicQuery = _T("");
		CComBSTR bstrFilters = _T("");
		ESP_EXCEPTION_LOG2(EspException);
		if (server.WUQuery(bstrWuid, &outCluster, &outOwner, &outState, &startDate, &endDate, &outECL, &outJobname, bstrAppName, bstrAppKey, bstrAppData, bstrAfter, bstrBefore, &count, &bstrSortBy, &boolDescenfing, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &current, &next, &numWUs, &boolFirst, &bstrBasicQuery, &bstrFilters, results.GetArrayAddress(), results.GetCountAddress()) == S_OK)
#elif _COMMS_VER < 58300
		CComBSTR Type = _T("");
		CComBSTR outECL = _T("");
		CComBSTR outJobname = _T("");
		CComBSTR LogicalFile = _T("");
		CComBSTR LogicalFileSearchType = _T("");
		CComBSTR bstrWuid = CT2W(wuid, CP_UTF8);
		CComBSTR bstrAppName = _T("");
		CComBSTR bstrAppKey = _T("");
		CComBSTR bstrAppData = _T("");
		CComBSTR bstrAfter = _T("");
		CComBSTR bstrBefore = _T("");
		CComBSTR bstrSortBy = _T("");
		bool boolDescenfing = false;
		int numWUs = 999;
		bool boolFirst = false;
		CComBSTR bstrBasicQuery = _T("");
		CComBSTR bstrFilters = _T("");
		if (server.WUQuery(bstrWuid, &Type, &outCluster, &outOwner, &outState, &startDate, &endDate, &outECL, &outJobname, &LogicalFile, &LogicalFileSearchType, bstrAppName, bstrAppKey, bstrAppData, bstrAfter, bstrBefore, &count, &bstrSortBy, &boolDescenfing, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &current, &next, &numWUs, &boolFirst, &bstrBasicQuery, &bstrFilters, results.GetArrayAddress(), results.GetCountAddress()) == S_OK)
#elif _COMMS_VER < 58800
		CComBSTR Type = _T("");
		CComBSTR outECL = _T("");
		CComBSTR outJobname = _T("");
		CComBSTR LogicalFile = _T("");
		CComBSTR LogicalFileSearchType = _T("");
		CComBSTR bstrWuid = CT2W(wuid, CP_UTF8);
		CComBSTR bstrAppName = _T("");
		CComBSTR bstrAppKey = _T("");
		CComBSTR bstrAppData = _T("");
		CComBSTR bstrAfter = _T("");
		CComBSTR bstrBefore = _T("");
		CComBSTR bstrSortBy = _T("");
		bool boolDescenfing = false;
		int numWUs = 999;
		bool boolFirst = false;
		CComBSTR bstrBasicQuery = _T("");
		CComBSTR bstrFilters = _T("");
		__int64 PageSize = 0;
		__int64 PageStartFrom = 0;
		__int64 PageEndAt = 0;
		__int64 PrevPage = 0;
		__int64 NextPage = 0;
		__int64 LastPage = 0;
		if (server.WUQuery(bstrWuid, &Type, &outCluster, &outOwner, &outState, &startDate, &endDate, &outECL, &outJobname, &LogicalFile, &LogicalFileSearchType, bstrAppName, bstrAppKey, bstrAppData, bstrAfter, bstrBefore, &count, 
			&PageSize,
			&PageStartFrom,
			&PageEndAt,
			&bstrSortBy, &boolDescenfing, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &current, &next, 
			&PrevPage,
			&NextPage,
			&LastPage,
			&numWUs, &boolFirst, &bstrBasicQuery, &bstrFilters, results.GetArrayAddress(), results.GetCountAddress()) == S_OK)
#elif _COMMS_VER < 67205
		CComBSTR outRoxieCluster = _T("");
		CComBSTR Type = _T("");
		CComBSTR outECL = _T("");
		CComBSTR outJobname = _T("");
		CComBSTR LogicalFile = _T("");
		CComBSTR LogicalFileSearchType = _T("");
		CComBSTR bstrWuid = CT2W(wuid, CP_UTF8);
		CComBSTR bstrAppName = _T("");
		CComBSTR bstrAppKey = _T("");
		CComBSTR bstrAppData = _T("");
		CComBSTR bstrAfter = _T("");
		CComBSTR bstrBefore = _T("");
		CComBSTR bstrSortBy = _T("");
		bool boolDescenfing = false;
		int numWUs = 999;
		bool boolFirst = false;
		CComBSTR bstrBasicQuery = _T("");
		CComBSTR bstrFilters = _T("");
		__int64 PageSize = 999999;
		__int64 PageStartFrom = 0;
		__int64 PageEndAt = 0;
		__int64 PrevPage = 0;
		__int64 NextPage = 0;
		__int64 LastPage = 0;
		ESP_EXCEPTION_LOG(EspException);
		if (server.WUQuery(bstrWuid, &Type, &outCluster, &outRoxieCluster, &outOwner, &outState, &startDate, &endDate, &outECL, &outJobname, &LogicalFile, &LogicalFileSearchType, bstrAppName, bstrAppKey, bstrAppData, bstrAfter, bstrBefore, &count, 
			&PageSize,
			&PageStartFrom,
			&PageEndAt,
			&bstrSortBy, &boolDescenfing, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &current, &next, 
			&PrevPage,
			&NextPage,
			&LastPage,
			&numWUs, &boolFirst, &bstrBasicQuery, &bstrFilters, results.GetArrayAddress(), results.GetCountAddress()) == S_OK)
#elif _COMMS_VER < 700000
		CComBSTR outRoxieCluster = _T("");
		CComBSTR Type = _T("");
		CComBSTR outECL = _T("");
		CComBSTR outJobname = _T("");
		CComBSTR LogicalFile = _T("");
		CComBSTR LogicalFileSearchType = _T("");
		CComBSTR bstrWuid = CT2W(wuid, CP_UTF8);
		CComBSTR bstrAppName = _T("");
		CComBSTR bstrAppKey = _T("");
		CComBSTR bstrAppData = _T("");
		CComBSTR bstrAfter = _T("");
		CComBSTR bstrBefore = _T("");
		CComBSTR bstrSortBy = _T("");
		bool boolDescenfing = false;
		int numWUs = 999;
		bool boolFirst = false;
		CComBSTR bstrBasicQuery = _T("");
		CComBSTR bstrFilters = _T("");
		__int64 PageSize = 999999;
		__int64 PageStartFrom = 0;
		__int64 PageEndAt = 0;
		__int64 PrevPage = 0;
		__int64 NextPage = 0;
		__int64 LastPage = 0;
		ESP_EXCEPTION_LOG2(EspException);
		if (server.WUQuery(bstrWuid, &Type, &outCluster, &outRoxieCluster, &outOwner, &outState, &startDate, &endDate, &outECL, &outJobname, &LogicalFile, &LogicalFileSearchType, bstrAppName, bstrAppKey, bstrAppData, bstrAfter, bstrBefore, &count, 
			&PageSize,
			&PageStartFrom,
			&PageEndAt,
			&bstrSortBy, &boolDescenfing, exceptions, &current, &next, 
			&PrevPage,
			&NextPage,
			&LastPage,
			&numWUs, &boolFirst, &bstrBasicQuery, &bstrFilters, results.GetArrayAddress(), results.GetCountAddress()) == S_OK)
#else
		ATLASSERT(!_T("TODO"));
		bool Archived = false;
		CComBSTR WUID = CT2W(wuid, CP_UTF8); 
		CComBSTR Cluster = _T("");
		CComBSTR RoxieCluster = _T("");
		CComBSTR Owner = _T("");
		CComBSTR JobName = _T("");
		CComBSTR State = _T("");
		ECLWUDateTime StartDate;
		ECLWUDateTime EndDate;
		CComBSTR LogicalFile = _T("");
		CComBSTR LogicalFileSearchType = _T("");
		CComBSTR ECL = _T("");
		CComBSTR ApplicationName = _T("");
		CComBSTR ApplicationKeyName = _T("");
		CComBSTR ApplicationKeyValue = _T("");
		CComBSTR FirstWUID = _T("");
		CComBSTR LastWUID = _T("");
		__int64 ResultsCount = -1;
		__int64 ResultsStartFrom = 0;
		CComBSTR Sortby = _T("");
		bool Descending = false;
		ESP_STATUS_LOG;
		if (server.SearchWorkunits(Archived, WUID, Cluster, RoxieCluster, Owner, JobName, State, StartDate, EndDate, LogicalFile, LogicalFileSearchType, 
			ECL, ApplicationName, ApplicationKeyName, ApplicationKeyValue, FirstWUID, LastWUID, 
			ResultsCount, ResultsStartFrom,
			Sortby, 
			Descending, &status.m_statusCode, &status.m_statusMessage, results.GetArrayAddress(), results.GetCountAddress()) == S_OK)
#endif
		{
			for(int i = 0; i < results.GetCount(); ++i)
			{
				//pull down the workunit details and update it
				IWorkunitAdapt workunit = CreateWorkunit(config, results.GetItem(i), false, noBroadcast);
				workunits->push_back(workunit);
			}
		}
		else
			_DBGLOG(config.GetUrl(url), LEVEL_WARNING, server.GetClientError());

		return results.GetCount();
	}

	static void GetWorkunits2(CServerConfig config, CString cluster, CString owner, CString state, CString fromUTC, CString toUTC, int count, async_wucomplete_slot_type s) 
	{
		CComInitialize com;
		StlLinked<IWorkunitVectorCom> workunits = new IWorkunitVectorCom;
		async_wucomplete_signal_type on_completion;
		on_completion.connect(s);
		GetWorkunits1(config, _T(""), cluster, owner, state, fromUTC, toUTC, count, workunits.get());
		on_completion(workunits.get());
		return;
	}

	static IWorkunit * GetWorkunit(CServerConfig config, CString wuid, bool noBroadcast) 
	{
		CComInitialize com;
		ServerT server;
		InitDaliServer(config, server);

		CComBSTR _wuid = CT2W(wuid, CP_UTF8);
		ECLWorkunit result;
		StlLinked<IWorkunit> retVal;

		CStructArrayOut<ECLWorkunit> results;
#if _COMMS_VER < 55000
		if (server.WUInfo(_wuid, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &result, &canCompile) == S_OK)
#elif _COMMS_VER < 57600
		int autoRefresh = 0;
		bool canCompile;
		ESP_EXCEPTION_LOG2(EspException);
		if (server.WUInfo(_wuid, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &result, &autoRefresh, &canCompile) == S_OK)
		{
			if ( result.Wuid )
#elif _COMMS_VER < 64801
		CComBSTR Type = _T("");

		int autoRefresh = 0;
		bool canCompile;
		ESP_EXCEPTION_LOG2(EspException);
		if (server.WUInfo(_wuid, Type, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &result, &autoRefresh, &canCompile) == S_OK)
		{
			if ( result.Wuid )
#elif _COMMS_VER < 67205
		CComBSTR Type = _T("");

		int autoRefresh = 0;
		bool canCompile;
		ESP_EXCEPTION_LOG(EspException);
		if (server.WUInfo(_wuid, Type, true, true, true, true, true, true, true, true, true, true, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &result, &autoRefresh, &canCompile) == S_OK)
		{
			if ( result.Wuid )
#elif _COMMS_VER < 68200
		CComBSTR Type = _T("");

		int autoRefresh = 0;
		bool canCompile;
		ESP_EXCEPTION_LOG2(EspException);
		if (server.WUInfo(_wuid, Type, true, true, true, true, true, true, true, true, true, true, exceptions, &result, &autoRefresh, &canCompile) == S_OK)
		{
			if ( result.Wuid )
#elif _COMMS_VER < 700000
		CComBSTR Type = _T("");
		CComBSTR thorSlaveIP = _T("");
		int autoRefresh = 0;
		bool canCompile;
		ESP_EXCEPTION_LOG2(EspException);
		if (server.WUInfo(_wuid, Type, true, true, true, true, true, true, true, true, true, true, &thorSlaveIP, exceptions, &result, &autoRefresh, &canCompile) == S_OK)
		{
			if ( result.Wuid )
#else
		bool ArchivedOnly = false;
		ESP_STATUS_LOG;
		if (server.GetWorkunitDetail(_wuid, ArchivedOnly, &status.m_statusCode, &status.m_statusMessage, &result) == S_OK)
		{
			if (result.WUID)
#endif
			{
				retVal = CreateWorkunit(config, &result, true, noBroadcast);
			}
			AtlCleanupValueEx(&result, &crtHeap);
		}
		else
			_DBGLOG(config.GetUrl(url), LEVEL_WARNING, server.GetClientError());

		return retVal.get();
	}

	bool BlockingSubmitEcl( const CString &cluster, const CString & queue, WUAction action, const CString & ecl, const CString &label, int resultLimit, const CString & debugSettings, bool archive, bool noCommonPrivateAttributes, StlLinked<IWorkunit> &resultWu, bool deleteAfter, int maxRunTime, bool debug)
	{
		CComInitialize com;
		bool retVal = false;
		if (SubmitEcl(cluster, queue, action, ecl, label, resultLimit, resultWu, debugSettings, archive, noCommonPrivateAttributes, maxRunTime, debug))
		{
			retVal = BlockUntilComplete(m_config, resultWu->GetWuid());

			if (retVal && deleteAfter)
				DeleteWorkunit(resultWu.get());
		}

		return retVal;
	}

	bool ScheduleEcl(const CString &cluster, const CString & queue, const CString & ecl, const CString & when, const CString & label, int resultLimit, StlLinked<IWorkunit> &resultWu, const CString & debugSettings, bool archive)
	{
	#if _COMMS_VER < 491
		_DBGLOG(config.GetUrl(url), LEVEL_WARNING, _T("Schedule is unsupported on this server."));
	#else
		CComInitialize com;
		ServerT server;
		InitServer(server);

		bool retVal = false;
		ECLWorkunit wu = {0};
#if _COMMS_VER < 67205
		ESP_EXCEPTION_LOG(EspException);
		if (server.WUCreate(exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &wu) == S_OK)
		{
			if (wu.Wuid)
			{
#elif _COMMS_VER < 700000
		ESP_EXCEPTION_LOG2(EspException);
		if (server.WUCreate(exceptions, &wu) == S_OK)
		{
			if (wu.Wuid)
			{
#else
		ESP_STATUS_LOG;
		CComBSTR wuid;
		if (server.CreateWorkunit(&status.m_statusCode, &status.m_statusMessage, &wuid) == S_OK)
		{
			wu.WUID = wuid.Detach();
			if (wu.WUID)
			{
#endif
				if (UpdateWU(m_config, wu, WUStateUnknown, cluster, queue, WUActionRun, ecl, resultLimit, debugSettings, archive, false, false))
				{
					ServerT server;
					server.SetUrl(CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT)));
					CComBSTR clusterStr = CT2W(cluster, CP_UTF8);
					CComBSTR queueStr = CT2W(queue, CP_UTF8);
					CComBSTR whenStr = CT2W(when, CP_UTF8);
#if _COMMS_VER < 50515
					if (server.WUSchedule(wu.Wuid, clusterStr, queueStr, whenStr, exceptions.GetArrayAddress(), exceptions.GetCountAddress()) == S_OK)
#elif _COMMS_VER < 52501
					CComBSTR snapshotStr = label;
					if (server.WUSchedule(wu.Wuid, clusterStr, queueStr, snapshotStr, whenStr, exceptions.GetArrayAddress(), exceptions.GetCountAddress()) == S_OK)
#elif _COMMS_VER < 67205
					CComBSTR snapshotStr = label;
					ESP_EXCEPTION_LOG(EspException);
					if (server.WUSchedule(wu.Wuid, clusterStr, queueStr, snapshotStr, whenStr, 0, exceptions.GetArrayAddress(), exceptions.GetCountAddress()) == S_OK)
					{
						resultWu = GetWorkunit(m_config, wu.Wuid, false);
#elif _COMMS_VER < 700000
					CComBSTR snapshotStr = label;
					ESP_EXCEPTION_LOG2(EspException);
					if (server.WUSchedule(wu.Wuid, clusterStr, queueStr, snapshotStr, whenStr, 0, exceptions) == S_OK)
					{
						resultWu = GetWorkunit(m_config, wu.Wuid, false);
#else
					CComBSTR snapshotStr = label;
					ATLASSERT(!_T("TODO"));
					ECLWUDateTime When;
					ESP_STATUS_LOG;
					if (server.ScheduleWorkunit(wu.WUID, clusterStr, queueStr, snapshotStr, 0, When, &status.m_statusCode, &status.m_statusMessage) == S_OK)
					{
						resultWu = GetWorkunit(m_config, wu.WUID, false);
#endif
						retVal = true;
					}
					else
						_DBGLOG(config.GetUrl(url), LEVEL_WARNING, server.GetClientError());
				}
			}

			AtlCleanupValue(&wu);
		}
		else
			_DBGLOG(config.GetUrl(url), LEVEL_WARNING, server.GetClientError());
	#endif
		return retVal;
	}

	bool SubmitEcl(const CString &cluster, const CString & queue, WUAction action, const CString & ecl, const CString label, int resultLimit, StlLinked<IWorkunit> &resultWu, const CString & debugSettings, bool archive, bool noCommonPrivateAttributes, int maxRunTime, bool debug)
	{
		CComInitialize com;
		ServerT server;
		InitServer(server);

		bool retVal = false;
		ECLWorkunit wu = {0};
#if _COMMS_VER < 67205
		ESP_EXCEPTION_LOG(EspException);
		if (server.WUCreate(exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &wu) == S_OK)
		{
			if (wu.Wuid)
			{
#elif _COMMS_VER < 700000
		ESP_EXCEPTION_LOG2(EspException);
		if (server.WUCreate(exceptions, &wu) == S_OK)
		{
			if (wu.Wuid)
			{
#else
		ESP_STATUS_LOG;
		CComBSTR wuid;
		if (server.CreateWorkunit(&status.m_statusCode, &status.m_statusMessage, &wuid) == S_OK)
		{
			wu.WUID = wuid.Detach();
			if (wu.WUID)
			{
#endif
				if (UpdateWU(m_config, wu, WUStateUnknown, cluster, queue, action, ecl, resultLimit, debugSettings, archive, noCommonPrivateAttributes, debug))
				{
					ServerT server;
					InitServer(server);
					CComBSTR clusterStr = CT2W(cluster, CP_UTF8);
					CComBSTR queueStr = CT2W(queue, CP_UTF8);
#if _COMMS_VER < 505
					if (server.WUSubmit(wu.Wuid, clusterStr, queueStr, exceptions.GetArrayAddress(), exceptions.GetCountAddress()) == S_OK)
#elif _COMMS_VER < 52501
					CComBSTR SnapShot = label;
					if (server.WUSubmit(wu.Wuid, clusterStr, queueStr, SnapShot, exceptions.GetArrayAddress(), exceptions.GetCountAddress()) == S_OK)
#elif _COMMS_VER < 57600
					CComBSTR SnapShot = label;
					int MaxRunTime = 0;
					ESP_EXCEPTION_LOG2(EspException);
					if (server.WUSubmit(wu.Wuid, clusterStr, queueStr, SnapShot, MaxRunTime, exceptions.GetArrayAddress(), exceptions.GetCountAddress()) == S_OK)
					{
						resultWu = GetWorkunit(m_config, wu.Wuid, false);
#elif _COMMS_VER < 65100
					CComBSTR SnapShot = label;
					int BlockTillFinishTimer = 0;
					ESP_EXCEPTION_LOG2(EspException);
					if (server.WUSubmit(wu.Wuid, clusterStr, queueStr, SnapShot, maxRunTime, BlockTillFinishTimer, exceptions.GetArrayAddress(), exceptions.GetCountAddress()) == S_OK)
					{
						resultWu = GetWorkunit(m_config, wu.Wuid, false);
#elif _COMMS_VER < 67205
					CComBSTR SnapShot = label;
					int BlockTillFinishTimer = 0;
					bool syntaxCheck = false;
					ESP_EXCEPTION_LOG(EspException);
					if (server.WUSubmit(wu.Wuid, clusterStr, queueStr, SnapShot, maxRunTime, BlockTillFinishTimer, syntaxCheck, exceptions.GetArrayAddress(), exceptions.GetCountAddress()) == S_OK)
					{
						resultWu = GetWorkunit(m_config, wu.Wuid, false);
#elif _COMMS_VER < 700000
					CComBSTR SnapShot = label;
					int BlockTillFinishTimer = 0;
					bool syntaxCheck = false;
					ESP_EXCEPTION_LOG2(EspException);
					if (server.WUSubmit(wu.Wuid, clusterStr, queueStr, SnapShot, maxRunTime, BlockTillFinishTimer, syntaxCheck, exceptions) == S_OK)
					{
						resultWu = GetWorkunit(m_config, wu.Wuid, false);
#else
					CComBSTR SnapShot = label;
					int BlockTillFinishTimer = 0;
					ESP_STATUS_LOG;
					if (server.SubmitWorkunit(wu.WUID, clusterStr, queueStr, SnapShot, maxRunTime, BlockTillFinishTimer, &status.m_statusCode, &status.m_statusMessage) == S_OK)
					{
						resultWu = GetWorkunit(m_config, wu.WUID, false);
#endif
						retVal = true;
					}
					else
						_DBGLOG(config.GetUrl(url), LEVEL_WARNING, server.GetClientError());
				}
			}

			AtlCleanupValue(&wu);
		}
		else
			_DBGLOG(config.GetUrl(url), LEVEL_WARNING, server.GetClientError());
		return retVal;
	}

	bool CompileEcl(const CComBSTR& ecl, const CComBSTR &module, const CComBSTR &attribute, const CComBSTR &queue, const CComBSTR &cluster, const CComBSTR &snapshot, bool includeDependancies, bool includeComplexity, int timeToWait, CComBSTR &outComplexity, AttributeVector & outDependicies, CEclExceptionVector & outExceptions)
	{
#if _COMMS_VER < 505
		_DBGLOG(config.GetUrl(url), LEVEL_WARNING, _T("WUCompileECL is unsupported on this server version."));
#else
		CComInitialize com;
		ServerT server;
		InitServer(server);

		CStructArrayOut<ECLException> errors;
		CStructArrayOut<ECLAttribute> dependancies;

#if _COMMS_VER < 50514
		if (server.WUCompileECL(ecl, queue, cluster, snapshot, includeDependancies, includeComplexity, timeToWait, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &outComplexity, errors.GetArrayAddress(), errors.GetCountAddress(), dependancies.GetArrayAddress(), dependancies.GetCountAddress()) == S_OK)
#elif _COMMS_VER < 67205
		ESP_EXCEPTION_LOG(EspException);
		if (server.WUCompileECL(ecl, module, attribute, queue, cluster, snapshot, includeDependancies, includeComplexity, timeToWait, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &outComplexity, errors.GetArrayAddress(), errors.GetCountAddress(), dependancies.GetArrayAddress(), dependancies.GetCountAddress()) == S_OK)
#elif _COMMS_VER < 700000
		ESP_EXCEPTION_LOG2(EspException);
		if (server.WUCompileECL(ecl, module, attribute, queue, cluster, snapshot, includeDependancies, includeComplexity, timeToWait, exceptions, &outComplexity, errors.GetArrayAddress(), errors.GetCountAddress(), dependancies.GetArrayAddress(), dependancies.GetCountAddress()) == S_OK)
#else
		ESP_STATUS_LOG;
		if (server.CompileECLStatements(ecl, module, attribute, queue, cluster, snapshot, includeDependancies, includeComplexity, timeToWait, &status.m_statusCode, &status.m_statusMessage, &outComplexity, errors.GetArrayAddress(), errors.GetCountAddress(), dependancies.GetArrayAddress(), dependancies.GetCountAddress()) == S_OK)
#endif
		{
			processExceptions(errors, outExceptions);
			for(int i = 0; i < dependancies.GetCount(); ++i)
			{
				std::_tstring module = CW2T(dependancies.GetItem(i)->ModuleName, CP_UTF8);
				std::_tstring attr = CW2T(dependancies.GetItem(i)->AttributeName, CP_UTF8);
				outDependicies.push_back(Attribute(module, attr));
			}
			return true;
		}
		else
			_DBGLOG(config.GetUrl(url), LEVEL_WARNING, server.GetClientError());
#endif
		return false;
	}

	static bool BlockUntilComplete(CServerConfig config, std::_tstring wuid)
	{
		//don't bother if somebody else is doing it.
		if (!g_monitor.Start(wuid))
			return true;

		CComInitialize com;
		ATLTRACE(_T("BlockUntilComplete_Start:  %s\n"), wuid.c_str());

		IWorkunitAdapt workunit;
		std::map<WUState, int> delayMap;
		CString me = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_USER);
		Sleep(1 * SECOND);	//Always Sleep for 1 second before start monitoring (in general the user will have just done a WUInfo prior to asking for a monitor...

		//if they log off while we are waiting, it will no longer be active
		while(g_monitor.Active(wuid))
		{
			CMonitorCount count;
#if _COMMS_VER < 53306
			ServerT server;
			InitDaliServer(config,server);

			CComBSTR inWuid = CT2W(wuid.c_str(), CP_UTF8);
			CReportingStructArrayOut<EspException> exceptions(_T("WUWaitComplete"));
			ECLWorkunit result;
			bool canCompile;
			if (server.WUInfo(inWuid, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &result, &canCompile) == S_OK)
			{
				//check again after the delay of getting the server response
				//and check to see if could have been deleted!
				if ( !g_monitor.Active(wuid) || !result.Wuid  )
				{
					AtlCleanupValueEx(&result, &crtHeap);
					break;
				}

				WUState state;
				workunit = CreateWorkunit(config,&result);

				state = workunit->GetState();

				AtlCleanupValueEx(&result, &crtHeap);

				if (workunit->IsComplete())
					break;

				if (delayMap[state] == 0)
					delayMap[state] = (me.CompareNoCase(workunit->GetOwner()) == 0 ? 1 : 5) * SECOND;
#else
			IWorkunitVector results;
			workunit = NULL;
			int resultCount;
			{
				//lock to prevent refresh (we will do it manually in a minute...)
				resultCount = GetWorkunits1(config, wuid.c_str(), _T(""), _T(""), _T(""), _T(""), _T(""), 999, &results, true);
			}

			if (resultCount == 1)
			{
				workunit = results[0];
				//check again after the delay of getting the server response
				//and check to see if could have been deleted!
				if (!g_monitor.Active(wuid))
				{
					break;
				}

				WUState state = workunit->GetState();
				if (workunit->IsComplete())
				{
					workunit = GetWorkunit(config, workunit->GetWuid(), false);	//Need to do a full get to update the results list.
					break;
				}
				else
				{
					//  Manually update if not complete...
					workunit->ForceBroadcast();
				}

				if (delayMap[state] == 0)
				{
					delayMap[state] = (me.CompareNoCase(workunit->GetOwner()) == 0 ? 1 : 5) * SECOND;
				}
#endif
				int sleep_time = 3 * SECOND;
				switch(state)
				{
				case WUStateDebugPaused:
					sleep_time = std::min<int>(delayMap[state], 10 * SECOND);
					break;
				case WUStateDebugRunning:
					sleep_time = std::min<int>(delayMap[state], 3 * SECOND);
					break;
				default:
					sleep_time = std::min<int>(delayMap[state], 1 * MINUTE);
					break;
				}
				ATLTRACE(_T("BlockUntilComplete_Sleep:  %s, %i\n"), wuid.c_str(), sleep_time);
				delayMap[state] = sleep_time * 2;
				//don't sleep too long without checking active bit
				while (sleep_time > 0 && g_monitor.Active(wuid) )
				{
					int time = min(3 * SECOND, sleep_time);
					Sleep(time);
					sleep_time -= time;
				}
			}
			else
			{
				break;
			}
		}
		ATLTRACE(_T("BlockUntilComplete_Finish:  %s\n"), wuid.c_str());
		g_monitor.Stop(wuid);
		return true;
	}

	bool DeleteWorkunit(IWorkunit *workunit)
	{
		IWorkunitVector workunits;
		workunits.push_back(workunit);
		return DeleteWorkunits(&workunits);
	}

	static bool DeleteWorkunits(const CServerConfig &config, IWorkunitVector *workunits)
	{
		CComInitialize com;
		ServerT server;
		InitDaliServer(config, server);

		unsigned units = workunits->size();
		BSTR *arr =	CreateBstrArray(workunits);

#if _COMMS_VER < 55000
		if (server.WUDelete(arr, units, exceptions.GetArrayAddress(), exceptions.GetCountAddress()) == S_OK)
#elif _COMMS_VER < 57600
		CStructArrayOut<WUActionResult> actionResults;
		ESP_EXCEPTION_LOG2(EspException);
		if (server.WUDelete(arr, units, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), actionResults.GetArrayAddress(), actionResults.GetCountAddress()) == S_OK)
#elif _COMMS_VER < 67205
		int BlockTillFinishTimer = false;
		CStructArrayOut<WUActionResult> actionResults;
		ESP_EXCEPTION_LOG(EspException);
		if (server.WUDelete(arr, units, BlockTillFinishTimer, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), actionResults.GetArrayAddress(), actionResults.GetCountAddress()) == S_OK)
#elif _COMMS_VER < 700000
		int BlockTillFinishTimer = false;
		CStructArrayOut<WUActionResult> actionResults;
		ESP_EXCEPTION_LOG2(EspException);
		if (server.WUDelete(arr, units, BlockTillFinishTimer, exceptions, actionResults.GetArrayAddress(), actionResults.GetCountAddress()) == S_OK)
#else
		int BlockTillFinishTimer = false;
		CStructArrayOut<WUActionResult> actionResults;
		ESP_STATUS_LOG;
		if (server.DeleteWorkunits(arr, units, BlockTillFinishTimer, &status.m_statusCode, &status.m_statusMessage, actionResults.GetArrayAddress(), actionResults.GetCountAddress()) == S_OK)
#endif
		{
			//Notify listeners about deletion.
			for (IWorkunitVector::iterator itr = workunits->begin(); itr != workunits->end(); ++itr)
			{
				itr->get()->ForceBroadcast(WUEventDeleted);
			}

			//remove these guys from global cache(s)
			for (unsigned i=0; i<units; i++ )
			{
				std::_tstring id = workunits->operator [](i)->GetID();
				ClearWorkunitCache(id);

				std::_tstring wuid = workunits->operator [](i)->GetWuid();
				g_monitor.Clear(wuid);
			}

			DestroyBstrArray(arr, units);
			return true;
		}
		else
			_DBGLOG(config.GetUrl(url), LEVEL_WARNING, server.GetClientError());

		DestroyBstrArray(arr, units);
		return false;
	}
};
#endif

//  ===========================================================================
CResultDataRequest::CResultDataRequest(const Dali::async_datacomplete_slot_type& slot) : m_slot(slot)
{
	m_request_sent = false;
}
CResultDataRequest::~CResultDataRequest()
{
	if (!m_request_sent)  
	{
		async_datacomplete_signal_type on_completion;
		on_completion.connect(m_slot);
		on_completion(m_start, m_count, 0, false);
	}
}
//  ===========================================================================

static CacheT<std::_tstring, CDali> DaliCache;

void ClearSingletons()
{
	g_monitor.Clear(); //this should kill all monitor threads

	ClearResultCache();
	ClearWorkunitCache();
	ClearXGMMLWorkunitCache();

	ClearLocalResultCache();
	ClearLocalWorkunitCache();

	DaliCache.Clear();
}

IDali * AttachDali()
{
	return DaliCache.Get(new CDali(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT), _T("Dali")));
}

IDali * AttachDali(const std::_tstring & url, const std::_tstring & label)
{
	return DaliCache.Get(new CDali(url.c_str(), label.c_str()));
}

IDali * AttachDali(const _variant_t & url, const std::_tstring & label)
{
	return DaliCache.Get(new CDali((const TCHAR *)CString(url), label.c_str()));
}

IDali * AttachDali(const TCHAR * url, const std::_tstring & label)
{
	return DaliCache.Get(new CDali(url, label.c_str()));
}

IDali * AttachDali(const CServerConfig& config)
{
	return DaliCache.Get(new CDali(config));
}
}