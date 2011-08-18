#include "StdAfx.h"

#include "RoxieDebugSession.h"
#include "ResponseSaxHandler.h"
#include "global.h"
//#include "XGMMLSaxHandler.h"

//  ===========================================================================
class ResponseString : public std::_tstring
{
	typedef std::_tstring baseClass;
	typedef ResponseString thisClass;
public:
	StlLinked<roxie::CDebug> m_debug;
	StlLinked<roxie::CException> m_exception;
	StlLinked<roxie::CException> m_warning;

	ResponseString()
	{
		m_debug = new roxie::CDebug();
		m_exception = new roxie::CException();
		m_warning = new roxie::CException();
	}

	ResponseString(const std::_tstring str) : baseClass(str)
	{
		m_debug = new roxie::CDebug();
		m_exception = new roxie::CException();
		m_warning = new roxie::CException();
	}

	void Detach(roxie::Response & response, CUniqueID & active)
	{
		response.m_debug = m_debug.get();
		response.m_exception = m_exception.get();
		response.m_warning = m_warning.get();
		if (!m_debug->GetActiveID().isEmpty())
			active = m_debug->GetActiveID();
		m_debug = NULL;
		m_exception = NULL;
		m_warning = NULL;
	}

	bool Parse()
	{
		bool retVal = false;
		CComInitialize com;
		{
			CComPtr<ISAXXMLReader> pRdr; 
			pRdr.CoCreateInstance(CLSID_SAXXMLReader30); 
			CComPtr<RoxieDebugResponseHandler> pMc = new RoxieDebugResponseHandler(m_debug, m_exception, m_warning); 
			HRESULT hr = pRdr->putContentHandler(pMc); 
			CComVariant varXML = CT2W((boost::_tformat(_T("<Result>%1%</Result>")) % c_str()).str().c_str(), CP_UTF8);
			hr = pRdr->parse(varXML);
			retVal = hr == S_OK;
		}

		if (m_debug->m_finalData)
		{
			CComPtr<ISAXXMLReader> pDatasetRdr; 
			pDatasetRdr.CoCreateInstance(CLSID_SAXXMLReader30); 
			CComPtr<SeisintDatasetHandlerImpl> pDatasetMc = new SeisintDatasetHandlerImpl(m_debug->m_data); 
			HRESULT hr = pDatasetRdr->putContentHandler(pDatasetMc); 
			CComVariant varXML = CT2W((boost::_tformat(_T("<Debug>%1%</Debug>")) % c_str()).str().c_str(), CP_UTF8);
			hr = pDatasetRdr->parse(varXML);
		}

		if (m_debug->m_hasData)
		{
			CComPtr<ISAXXMLReader> pDatasetRdr; 
			pDatasetRdr.CoCreateInstance(CLSID_SAXXMLReader30); 
			CComPtr<SeisintDatasetHandlerImpl> pDatasetMc = new SeisintDatasetHandlerImpl(m_debug->m_data); 
			HRESULT hr = pDatasetRdr->putContentHandler(pDatasetMc); 
			CComVariant varXML = CT2W(c_str(), CP_UTF8);
			hr = pDatasetRdr->parse(varXML);
		}
		if (m_debug->m_hasGraph)
		{
			m_debug->m_graph = c_str();
		}
		return retVal;
	}
};
//  ===========================================================================
class CRoxieResult : public Dali::IResult, public clib::CLockableUnknown, public boost::noncopyable
{
protected:
	CString m_id;
	CString m_url;
	CString m_wuid;
	CString m_name;
	int m_sequence;
	int m_graphSequenceNum;
	CString m_value;
	CString m_link;
	CString m_fileName;
	bool IsSupplied;
	__int64 m_total;
	StlLinked<ITable> m_results;

public:
	IMPLEMENT_CLOCKABLEUNKNOWN;

	CRoxieResult()
	{
		ATLASSERT(false);
	}
	CRoxieResult(const CString & url, const CString & wuid, int sequence, int graphSequenceNum, const CString name) : m_url(url), m_wuid(wuid), m_sequence(sequence), m_graphSequenceNum(graphSequenceNum), m_name(name)
	{
		m_id = m_url + _T("/") + m_wuid + _T("/") + name;
		m_total = 0;
		m_results = CreateISparseTable();
		m_results->SetColumn(0, _T("##"), CT_UINT);
	}
	virtual ~CRoxieResult()
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
	const TCHAR *GetName() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_name;
	}
	int GetSequence() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_sequence;
	}
	int GetGraphSequenceNum() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_graphSequenceNum;
	}
	const TCHAR *GetValue() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_value;
	}
	const TCHAR *GetLink() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_link;
	}
	const TCHAR *GetFileName() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_fileName;
	}
	__int64 GetTotal() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_total;
	}
	void SetResultData(ITable * data)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		m_results->DoClear();
		m_results->LoadFrom(data);
		m_total = m_results->GetRowCount();
	}
	unsigned GetResultData(__int64 start, int count, ITable * result) const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		__int64 total;
		unsigned retVal = GetResultData(start, count, result, total);
		if (m_total && m_total < total)
			total = m_total;
		return retVal;
	}
	unsigned GetResultData(__int64 start, int count, ITable * result, __int64 & total) const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		result->CopyFrom(m_results, start, count);
		total = m_total;
		return count;
	}
};
//  ===========================================================================

CRoxieDebugSession::CRoxieDebugSession(IRoxieDebugSessionReceiver * owner) : m_owner(owner)
{
	m_activeThreadCount = 0;
	Clear();
}

void CRoxieDebugSession::Clear()
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	m_ip.clear();
	m_port.clear();

	m_state = DEBUGSTATE_COMPLETE;
	m_inDebugSession = false;
	m_session.SetID(_T(""));

	m_sequence = 0;
	m_graphSequenceNum = 0;

	m_breakpoints.clear();
	m_computedBreakpoints.clear();

	m_resultsVector.clear();
	m_resultsMap.clear();

	m_visibleResultsVector.clear();
	m_visibleResultsMap.clear();

	m_dali = NULL;
	m_wu = NULL;
	m_globalIDCache.clear();

	m_hasResponse = false;
	m_response.clear();

	m_currSel = CUniqueID();
	m_currSelSet.clear();
	m_currActive = CUniqueID();

	m_graphEnd = false;
	m_graphsMap.clear();
	m_graphsVector.clear();
}

int CRoxieDebugSession::GetActiveThreadCount() const
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	return m_activeThreadCount;
}

const CUniqueID & CRoxieDebugSession::GetActiveID()
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	return m_currActive;
}

const CUniqueID & CRoxieDebugSession::GetCurrentID()
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	return m_currSel;
}

const CUniqueID & CRoxieDebugSession::GetGlobalID(const CUniqueID & id) const
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	CUniqueIDCUniqueIDMap::const_iterator found = m_globalIDCache.find(id);
	if (found != m_globalIDCache.end())
		return found->second;

	typedef std::vector<std::_tstring> split_vector_type;
	split_vector_type SplitVec; 
	std::_tstring idStr = id.GetID();
	boost::algorithm::split(SplitVec, idStr, boost::algorithm::is_any_of("."), boost::algorithm::token_compress_on);
	if (SplitVec.size() == 2)
		m_globalIDCache[id] = CUniqueID(id.GetCategory(), SplitVec[0]);
	else
		m_globalIDCache[id] = id;
	m_activeIDCache[m_globalIDCache[id]].insert(id);
	return m_globalIDCache[id];
}

unsigned int CRoxieDebugSession::GetActiveID(const CUniqueID & id, CUniqueIDSet & results) const
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	results = m_activeIDCache[id];
	return results.size();
}

void CRoxieDebugSession::GetSearchResult(const CUniqueID & id, roxie::CSearchRowVector & rows) const
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	roxie::CSearchMap::const_iterator find = m_searchResults.find(id);
	if (find != m_searchResults.end())
		rows = find->second;
}

const roxie::CSearchMap & CRoxieDebugSession::GetSearchResults() const
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	return m_searchResults;
}

const roxie::CCountMap & CRoxieDebugSession::GetCountResults() const
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	return m_countResults;
}

void submitRequest(CRoxieDebugSession * self, std::string ip, std::string port, std::_tstring request, roxie::CALL callMode, roxie::Response & response)
{
	ATLTRACE(_T("Request:\n%s\n"), request.c_str());
	ResponseString rs;
	if (self->m_dali)
	{
		self->m_dali->SendDebugRequest(self->m_wuid, request, rs);
	}
	else
	{
		CTestSocket socket(ip, port);
		socket.SendRequest(request, rs);
	}
	ATLTRACE(_T("RequestResponse:\n%s\n%s\n"), request.c_str(), rs.c_str());
//	if (callMode == roxie::CALL_START)
//		rs = _T("<Debug>") + rs + _T("</Debug>");
	rs.Parse();
	switch(callMode)
	{
	case roxie::CALL_INFO:
		{
			CUniqueID id;
			rs.Detach(response, id);
		}
		break;
	default:
		rs.Detach(response, self->m_currActive);
	}
}

class CScopedInterlockedIncDec
{
protected:
	LONG & m_item;
public:
	CScopedInterlockedIncDec(LONG & item) : m_item(item)
	{
		::InterlockedIncrement(&m_item);
	}

	~CScopedInterlockedIncDec()
	{
		::InterlockedDecrement(&m_item);
	}
};

void thread_SubmitRequest(CRoxieDebugSession * self, std::string ip, std::string port, std::_tstring request, roxie::CALL callMode)
{
	if (self->m_state == CRoxieDebugSession::DEBUGSTATE_COMPLETE && callMode != roxie::CALL_START)
		return;

	CScopedInterlockedIncDec incDec(self->m_activeThreadCount);
	self->PostCallStart(callMode);
	roxie::Response response;
	submitRequest(self, ip, port, request, callMode, response);
	self->PostCallEnd(callMode, response);
}

void start_SubmitRequest(CRoxieDebugSession * self, std::string ip, std::string port, std::_tstring request, roxie::CALL callMode, bool threaded = true)
{
	if (threaded)
		boost::thread run(boost::bind(&thread_SubmitRequest, self, ip, port, request, callMode));
	else
		thread_SubmitRequest(self, ip, port, request, callMode);
}

void CRoxieDebugSession::StartSession(const std::string & ip, const std::string & port, const std::_tstring & queryLabel, const std::_tstring & queryBody, bool debug)
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	Clear();
	m_session.GenerateID();
	std::_tstring id = m_session.GetID();
	m_ip = ip;
	m_port = port;
	m_inDebugSession = true;
	std::_tstring request = (boost::_tformat(_T("<%1% uid='%2%' debug='1' break='1'>%3%</%1%>")) % queryLabel % id % queryBody).str();
	start_SubmitRequest(this, m_ip, m_port, request, roxie::CALL_START);
}

void CRoxieDebugSession::AttachSession(const std::string & ip, const std::string & port, const std::_tstring & session)
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	Clear();
	m_ip = ip;
	m_port = port;
	m_inDebugSession = true;
}

void CRoxieDebugSession::AttachEspSession(Dali::IWorkunit * wu)
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	Clear();
	m_dali = Dali::AttachDali();
	m_wu = wu;
	m_wuid = wu->GetWuid();
	m_inDebugSession = true;
	m_session.SetID(m_wuid);
}

void CRoxieDebugSession::AttachEspSession(const std::_tstring & cfg, const std::_tstring & id, const std::_tstring & pw, const std::_tstring & wuid)
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	Clear();
	m_inDebugSession = true;
	m_session.SetID(wuid);

	boost::filesystem::path configFile;
	GetConfigPath(cfg, configFile);
	CComPtr<IConfig> config = CreateIConfig(QUERYBUILDER_CFG, configFile);
	GetIConfig(QUERYBUILDER_CFG)->Set(GLOBAL_USER, id);
	GetIConfig(QUERYBUILDER_CFG)->Set(GLOBAL_PASSWORD, pw);
	m_dali = Dali::AttachDali(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT), _T("Dali4RD"));  //Must different from "Dali"
	m_wu = m_dali->GetWorkunit(wuid.c_str());
	m_wuid = wuid;
}

void CRoxieDebugSession::SetResponse(const std::_tstring & xml)
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	m_hasResponse = true;
	m_inDebugSession = false;
	m_response = xml;
	m_breakpoints.clear();
	m_computedBreakpoints.clear();
	m_resultsMap.clear();
	m_resultsVector.clear();
	LoadResults((boost::_tformat(_T("<Result>%1%</Result>")) % xml).str());
}

void CRoxieDebugSession::LoadResults(const std::_tstring & xml)
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	CComInitialize com;
	CComPtr<ISAXXMLReader> pRdr; 
	pRdr.CoCreateInstance(CLSID_SAXXMLReader30); 
	ITableMap resultTableMap;
	CComPtr<SeisintDatasetHandlerImpl> pMc = new SeisintDatasetHandlerImpl(resultTableMap); 
	HRESULT hr = pRdr->putContentHandler(pMc); 
	CComVariant varXML = CT2W(xml.c_str(), CP_UTF8);
	hr = pRdr->parse(varXML);
	std::_tstring id = m_session.GetID();
	int i = 0;
	for(ITableMap::iterator itr = resultTableMap.begin(); itr != resultTableMap.end(); ++itr)
	{
		//Does result already exist?
		Dali::IResultMap::iterator findResult = m_resultsMap.find(itr->first);
		if (findResult == m_resultsMap.end())
		{
			CComPtr<CRoxieResult> result = new CRoxieResult(_T(""), id.c_str(), i, 0, itr->first.c_str());
			result->SetResultData(itr->second.m_table);
			m_resultsVector.push_back(result.p);
			m_resultsMap[itr->first] = result.p;
		}
		else
		{
			//findResult->second->DClear();
			reinterpret_cast<CRoxieResult*>(findResult->second.get())->SetResultData(itr->second.m_table);
		}
		//m_breakpoints[CreateUniqueID(_T("edge"), itr->first)].push_back(i);//Force display
		++i;
	}
	scope_lock.unlock();
}

void CRoxieDebugSession::LoadResults(const std::_tstring & id, const ITableMap * resultTableMap)
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	std::_tstring uid = m_session.GetID();
	int i = 0;
	for(ITableMap::const_iterator itr = resultTableMap->begin(); itr != resultTableMap->end(); ++itr)
	{
		//Does result already exist?
		Dali::IResultMap::iterator findResult = m_resultsMap.find(id);
		if (findResult == m_resultsMap.end())
		{
			CComPtr<CRoxieResult> result = new CRoxieResult(_T(""), uid.c_str(), i, 0, id.c_str());
			result->SetResultData(itr->second.m_table);
			m_resultsVector.push_back(result.p);
			m_resultsMap[id] = result.p;
		}
		else
		{
			reinterpret_cast<CRoxieResult*>(findResult->second.get())->SetResultData(itr->second.m_table);
		}
		++i;
	}
	scope_lock.unlock();
}

bool CRoxieDebugSession::IsComplete() const
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	return true;
}

CRoxieDebugSession::DEBUGSTATE CRoxieDebugSession::GetDebugState() const
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	return m_state;
}

void CRoxieDebugSession::SetDebugState(const CRoxieDebugSession::DEBUGSTATE & state)
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	m_state = state;
}

void CRoxieDebugSession::PostCallStart(roxie::CALL callMode)
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	switch(callMode)
	{
	case roxie::CALL_START:
		m_state = CRoxieDebugSession::DEBUGSTATE_PAUSED;
		break;
	case roxie::CALL_ABORT:
	case roxie::CALL_DETACH:
	case roxie::CALL_CONTINUE:
	case roxie::CALL_STEP:
	case roxie::CALL_NEXT:
		m_state = CRoxieDebugSession::DEBUGSTATE_CONTINUE;
		break;
	}
	m_owner->PostCallStart(callMode);
}

void CRoxieDebugSession::PostCallEnd(roxie::CALL callMode, const roxie::Response & response)
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	switch (response.m_exception->GetCode())
	{
	case 1413:	//Context Unknown
		m_state = CRoxieDebugSession::DEBUGSTATE_PAUSED;
		//Fall through.
	default:
		if (boost::algorithm::iequals(response.m_debug->GetState(), _T("")))
		{
		}
		else if (boost::algorithm::iequals(response.m_debug->GetState(), _T("created")))
			m_state = CRoxieDebugSession::DEBUGSTATE_PAUSED;
		else if (boost::algorithm::iequals(response.m_debug->GetState(), _T("loading")))
			m_state = CRoxieDebugSession::DEBUGSTATE_PAUSED;
		else if (boost::algorithm::iequals(response.m_debug->GetState(), _T("running")))
			m_state = CRoxieDebugSession::DEBUGSTATE_CONTINUE;
		else if (boost::algorithm::iequals(response.m_debug->GetState(), _T("edge")))
			m_state = CRoxieDebugSession::DEBUGSTATE_PAUSED;
		else if (boost::algorithm::iequals(response.m_debug->GetState(), _T("breakpoint")))
			m_state = CRoxieDebugSession::DEBUGSTATE_PAUSED;
		else if (boost::algorithm::iequals(response.m_debug->GetState(), _T("created")))
			m_state = CRoxieDebugSession::DEBUGSTATE_PAUSED;
		else if (boost::algorithm::iequals(response.m_debug->GetState(), _T("graph create")))
			m_state = CRoxieDebugSession::DEBUGSTATE_PAUSED;
		else if (boost::algorithm::iequals(response.m_debug->GetState(), _T("graph start")))
		{
			m_state = CRoxieDebugSession::DEBUGSTATE_PAUSED;
			m_graphEnd = false;
		}
		else if (boost::algorithm::iequals(response.m_debug->GetState(), _T("graph end")))
		{
			m_state = CRoxieDebugSession::DEBUGSTATE_PAUSED;
			m_graphEnd = true;
		}
		else if (boost::algorithm::iequals(response.m_debug->GetState(), _T("graph abort")))
			m_state = CRoxieDebugSession::DEBUGSTATE_PAUSED;
		else if (boost::algorithm::iequals(response.m_debug->GetState(), _T("exception")))
			m_state = CRoxieDebugSession::DEBUGSTATE_PAUSED;
		else if (boost::algorithm::iequals(response.m_debug->GetState(), _T("failed")))
			m_state = CRoxieDebugSession::DEBUGSTATE_COMPLETE;
		else if (boost::algorithm::iequals(response.m_debug->GetState(), _T("finished")))
			m_state = CRoxieDebugSession::DEBUGSTATE_PAUSED;
		else if (boost::algorithm::iequals(response.m_debug->GetState(), _T("unloaded")))
			m_state = CRoxieDebugSession::DEBUGSTATE_COMPLETE;
		else if (boost::algorithm::iequals(response.m_debug->GetState(), _T("quit")))
			m_state = CRoxieDebugSession::DEBUGSTATE_COMPLETE;
		else if (boost::algorithm::iequals(response.m_debug->GetState(), _T("detached")))
			m_state = CRoxieDebugSession::DEBUGSTATE_DETACH;
		else if (boost::algorithm::iequals(response.m_debug->GetState(), _T("limit hit")))
			m_state = CRoxieDebugSession::DEBUGSTATE_PAUSED;
		else
		{
			const TCHAR * state = response.m_debug->GetState();
			ATLASSERT(!response.m_debug->GetState());
		}
		switch(callMode)
		{
		case roxie::CALL_ABORT:
		case roxie::CALL_START:
			break;
		case roxie::CALL_ALLGRAPH:
		case roxie::CALL_ACTIVEGRAPH:
		case roxie::CALL_INFO:
		case roxie::CALL_CHANGES:
		case roxie::CALL_NEXT:
		case roxie::CALL_STEP:
			break;
		case roxie::CALL_STEPGRAPH:
			break;
		case roxie::CALL_STEPGRAPHSTART:
			if (m_graphEnd != false)
			{
				StepGraphStart();
				return;
			}
			break;
		case roxie::CALL_STEPGRAPHEND:
			if (m_graphEnd != true)
			{
				StepGraphEnd();
				return;
			}
			break;
		case roxie::CALL_ADDBREAKPOINT:
			GetAllBreakpoints();
			break;
		case roxie::CALL_DELETEBREAKPOINT:
			GetAllBreakpoints();
			break;
		case roxie::CALL_GETBREAKPOINTS:
			response.m_debug->GetBreakpoints(m_breakpoints);
			m_computedBreakpoints.clear();
			for(roxie::CBreakpointMap::const_iterator itr = m_breakpoints.begin(); itr != m_breakpoints.end(); ++itr)
				m_computedBreakpoints[GetGlobalID(itr->first)] = itr->second;
			break;
		case roxie::CALL_DETACH:
			break;
		case roxie::CALL_COUNTS:
			response.m_debug->GetCountResults(m_countResults);
			break;
		case roxie::CALL_SEARCH:
			response.m_debug->GetSearchResults(m_searchResults);
			break;	
		default:
			break;
		}
	}
	m_owner->PostCallEnd(callMode, response);
}

const TCHAR * CRoxieDebugSession::GetResponse() const
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	return m_response.c_str();
}

//void thread_SubmitContinue(CRoxieDebugSession * self, CTestSocket * socket)
//{
//	roxie::Response response;
//	ResponseString rs;
//	socket->SendRequest((boost::_tformat(_T("<debug:continue uid='%1%'/>")) % self->m_session.GetID()).str(), rs);
//	rs.Parse();
//	rs.Detach(response, self->m_currActive);
//	self->PostCallEnd(roxie::CALL_CONTINUE, response);
//}

//debug:continue
//  continues until breakpoint (or other interesting event)
void CRoxieDebugSession::Continue()
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	start_SubmitRequest(this, m_ip, m_port, (boost::_tformat(_T("<debug:continue uid='%1%'/>")) % m_session.GetID()).str(), roxie::CALL_CONTINUE);
}

void CRoxieDebugSession::Continue(const CUniqueID & id)
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	std::_tstring mode;
	switch(id.GetCategory().GetCategory())
	{
	case XGMML_CAT_GRAPH:
		mode = _T("graph");
		break;
	case XGMML_CAT_SUBGRAPH:
		mode = _T("graph");
		break;
	case XGMML_CAT_VERTEX:
		mode = _T("node");
		break;
	case XGMML_CAT_EDGE:
		mode = _T("edge");
		break;
	}
	start_SubmitRequest(this, m_ip, m_port, (boost::_tformat(_T("<debug:continue uid='%1%' mode='%2%' id='%3%'/>")) % m_session.GetID() % mode % id.GetID()).str(), roxie::CALL_CONTINUE);
}

void CRoxieDebugSession::Step()
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	start_SubmitRequest(this, m_ip, m_port, (boost::_tformat(_T("<debug:step uid='%1%' mode='edge'/>")) % m_session.GetID()).str(), roxie::CALL_STEP);
}

void CRoxieDebugSession::StepGraph()
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	start_SubmitRequest(this, m_ip, m_port, (boost::_tformat(_T("<debug:step uid='%1%' mode='graph'/>")) % m_session.GetID()).str(), roxie::CALL_STEPGRAPH);
}

void CRoxieDebugSession::StepGraphStart()
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	start_SubmitRequest(this, m_ip, m_port, (boost::_tformat(_T("<debug:step uid='%1%' mode='graph'/>")) % m_session.GetID()).str(), roxie::CALL_STEPGRAPHSTART);
}

void CRoxieDebugSession::StepGraphEnd()
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	start_SubmitRequest(this, m_ip, m_port, (boost::_tformat(_T("<debug:step uid='%1%' mode='graph'/>")) % m_session.GetID()).str(), roxie::CALL_STEPGRAPHEND);
}
//debug:next
//  continues until current edge has another row
void CRoxieDebugSession::Next()
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	start_SubmitRequest(this, m_ip, m_port, (boost::_tformat(_T("<debug:next uid='%1%'/>")) % m_session.GetID()).str(), roxie::CALL_NEXT);
}

//debug:interrupt
//  breaks in to running query
void CRoxieDebugSession::BreakAll()
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	start_SubmitRequest(this, m_ip, m_port, (boost::_tformat(_T("<debug:interrupt uid='%1%'/>")) % m_session.GetID()).str(), roxie::CALL_BREAK);
}

//debug:quit
//  aborts query being debugged
void CRoxieDebugSession::Abort()
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	start_SubmitRequest(this, m_ip, m_port, (boost::_tformat(_T("<debug:quit uid='%1%'/>")) % m_session.GetID()).str(), roxie::CALL_ABORT);
}

//debug:run
//  detaches debugger and runs query to completion
void CRoxieDebugSession::Detach()
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	start_SubmitRequest(this, m_ip, m_port, (boost::_tformat(_T("<debug:run uid='%1%'/>")) % m_session.GetID()).str(), roxie::CALL_DETACH);
}

void CRoxieDebugSession::RefreshCounts(int sequence, roxie::Response & rChanges, bool detach)
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	roxie::Response rBreak, rDetach;
	submitRequest(this, m_ip, m_port, (boost::_tformat(_T("<debug:interrupt uid='%1%'/>")) % m_session.GetID()).str(), roxie::CALL_BREAK, rBreak);
	submitRequest(this, m_ip, m_port, (boost::_tformat(_T("<debug:changes uid='%1%' sequence='%2%'/>")) % m_session.GetID() % sequence).str(), roxie::CALL_CHANGES, rChanges);
	if (detach)
		submitRequest(this, m_ip, m_port, (boost::_tformat(_T("<debug:run uid='%1%'/>")) % m_session.GetID()).str(), roxie::CALL_DETACH, rDetach);
	else
		Continue();
}

//debug:delete idx='nn'
//  set idx to 0 to delete ALL breakpoints
void CRoxieDebugSession::DeleteBreakpoint(const CUniqueID & id)
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	roxie::CBreakpointMap::const_iterator found = m_breakpoints.find(id); 
	if (found != m_breakpoints.end())
	{
		for(roxie::CBreakpointVector::const_iterator itr = found->second.begin(); itr != found->second.end(); ++itr)
		{
			DeleteBreakpoint(itr->m_idx);
		}
	}
}

void CRoxieDebugSession::DeleteBreakpoint(const std::_tstring & idx, bool threaded)
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	start_SubmitRequest(this, m_ip, m_port, (boost::_tformat(_T("<debug:delete uid='%1%' idx='%2%'/>")) % m_session.GetID() % idx).str(), roxie::CALL_DELETEBREAKPOINT, threaded);
}

void CRoxieDebugSession::DeleteAllBreakpoints()
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	start_SubmitRequest(this, m_ip, m_port, (boost::_tformat(_T("<debug:delete uid='%1%' idx='0'/>")) % m_session.GetID()).str(), roxie::CALL_DELETEBREAKPOINT);
}

//debug:breakpoint activity='nn' outputIdx='nn'
//  default to current edge if activity not specified
//  outputIdx defaults to 0 if not specified (multiple outputIdx support is TBD)
void CRoxieDebugSession::AddBreakpoint(const std::_tstring & id, std::_tstring action, std::_tstring expressionMode, std::_tstring field, std::_tstring expression, std::_tstring countMode, std::_tstring count, bool caseSensitive)
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	boost::algorithm::to_lower(action);
	boost::algorithm::to_lower(expressionMode);
	boost::algorithm::to_lower(countMode);
	std::_tstring expr;
	if (!boost::algorithm::equals(expressionMode, _T("always")))
		expr = (boost::_tformat(_T("fieldName='%1%' condition='%2%' caseSensitive='%3%' value='%4%'")) % field % expressionMode % (caseSensitive ? _T("true") : _T("false")) % expression).str();
	std::_tstring cnt;
	if (!boost::algorithm::equals(countMode, _T("always")))
		cnt = (boost::_tformat(_T("rowCountMode='%1%' rowCount='%2%'")) % countMode % count).str();
	start_SubmitRequest(this, m_ip, m_port, (boost::_tformat(_T("<debug:breakpoint uid='%1%' mode='%2%' id='%3%' action='%4%' %5% %6%/>")) % m_session.GetID() % (id.length() ? _T("edge") : _T("global")) % id % action % expr % cnt).str(), roxie::CALL_ADDBREAKPOINT);
}

bool CRoxieDebugSession::HasBreakpoint(const std::_tstring & id) const
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	roxie::CBreakpointMap::const_iterator itr = m_breakpoints.find(CUniqueID(guidDefault, XGMML_CAT_EDGE, id)); 
	if (itr != m_breakpoints.end() && itr->second.size() > 0)
		return true;
	return false;
}

unsigned int CRoxieDebugSession::GetBreakpointCount() const
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	return m_breakpoints.size();
}

//debug:list idx='nn'
//  if idx omitted, lists all breakpoints
void CRoxieDebugSession::GetBreakpoint(unsigned int idx)
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	start_SubmitRequest(this, m_ip, m_port, (boost::_tformat(_T("<debug:list uid='%1%' mode='edge' id='%2%'/>")) % m_session.GetID() % idx).str(), roxie::CALL_GETBREAKPOINT);
}

//debug:list idx='nn'
//  if idx omitted, lists all breakpoints
void CRoxieDebugSession::GetAllBreakpoints()
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	start_SubmitRequest(this, m_ip, m_port, (boost::_tformat(_T("<debug:list uid='%1%'/>")) % m_session.GetID()).str(), roxie::CALL_GETBREAKPOINTS);
}

void CRoxieDebugSession::Search(std::_tstring fieldName, std::_tstring condition, std::_tstring value, bool caseSensitive, bool fullRow)
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	boost::algorithm::to_lower(condition);
	start_SubmitRequest(this, m_ip, m_port,  
		(boost::_tformat(_T("<debug:search uid='%1%' fieldName='%2%' condition='%3%' caseSensitive='%4%' value='%5%' fullrows='%6%'/>"))
		% m_session.GetID() % fieldName % condition % (caseSensitive ? _T("true") : _T("false")) % value % (fullRow ? _T("true") : _T("false"))).str(), roxie::CALL_SEARCH);
}

void CRoxieDebugSession::Set(const std::_tstring & key, const std::_tstring & value)
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	start_SubmitRequest(this, m_ip, m_port, (boost::_tformat(_T("<debug:set uid='%1%' name='%2%' value='%3%'/>")) % m_session.GetID() % key % value).str(), roxie::CALL_SET);
}

void CRoxieDebugSession::GetStatus()
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	start_SubmitRequest(this, m_ip, m_port, (boost::_tformat(_T("<debug:status uid='%1%'/>")) % m_session.GetID()).str(), roxie::CALL_STATUS);
}

void CRoxieDebugSession::GetStatus(roxie::Response & response)
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	submitRequest(this, m_ip, m_port, (boost::_tformat(_T("<debug:status uid='%1%'/>")) % m_session.GetID()).str(), roxie::CALL_STATUS, response);
}

void CRoxieDebugSession::GetGlobalGraph()
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	start_SubmitRequest(this, m_ip, m_port, (boost::_tformat(_T("<debug:graph uid='%1%' name='all'/>")) % m_session.GetID()).str(), roxie::CALL_ALLGRAPH);
}

void CRoxieDebugSession::GetActiveGraph()
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	start_SubmitRequest(this, m_ip, m_port, (boost::_tformat(_T("<debug:graph uid='%1%' original='0'/>")) % m_session.GetID() ).str(), roxie::CALL_ACTIVEGRAPH);
}

//debug:print activity='nn' outputIdx='nn' numRows='n' startRow='n'
//  if activity omitted, assumes current. 
//  numRows will assume 1 if omitted
//  other info about current activity will be added over time...
void CRoxieDebugSession::GetActiveInfo(unsigned int outputIdx, unsigned int numRows, unsigned int startRow)
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	start_SubmitRequest(this, m_ip, m_port, (boost::_tformat(_T("<debug:print uid='%1%' outputIdx='%2%' numRows='%3%' startRow='%4%'/>")) % m_session.GetID() % outputIdx % numRows % startRow).str(), roxie::CALL_INFO);
}

void CRoxieDebugSession::LoadInfo(const CUniqueID & id, unsigned int numRows, unsigned int startRow)
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	if (id.GetCategory().GetCategory() == XGMML_CAT_EDGE)
		start_SubmitRequest(this, m_ip, m_port, (boost::_tformat(_T("<debug:print uid='%1%' edgeId='%2%' numRows='%3%' startRow='%4%'/>")) % m_session.GetID() % id.GetID() % numRows % startRow).str(), roxie::CALL_INFO);
}

void CRoxieDebugSession::GetChanges(int sequence)
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	start_SubmitRequest(this, m_ip, m_port, (boost::_tformat(_T("<debug:changes uid='%1%' sequence='%2%'/>")) % m_session.GetID() % sequence).str(), roxie::CALL_CHANGES);
}

void CRoxieDebugSession::GetCounts(int sequence)
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	start_SubmitRequest(this, m_ip, m_port, (boost::_tformat(_T("<debug:counts uid='%1%' sequence='%2%'/>")) % m_session.GetID() % sequence).str(), roxie::CALL_COUNTS);
}

//debug:where
//  displays history for all edges that input to current
void CRoxieDebugSession::Where()
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	start_SubmitRequest(this, m_ip, m_port, (boost::_tformat(_T("<debug:where uid='%1%'/>")) % m_session.GetID()).str(), roxie::CALL_WHERE);
}

void CRoxieDebugSession::Lock(CComPtr<clib::scoped_lock_ref_counted> & lock)
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
}

GraphTypes::Color CRoxieDebugSession::GetColor(IGraphSubgraph * subgraph) const
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	if (m_state == DEBUGSTATE_COMPLETE)
		return GraphTypes::Color::Gray;
	else if (m_currActive == subgraph->GetID())
		return m_graphEnd ? GraphTypes::Color::DarkGreen : GraphTypes::Color::Green;

	CComVariant val;
	subgraph->GetProperty(PROP_ROXIESTARTED, val);
	if (val.vt == VT_BOOL && val.boolVal)
		return GraphTypes::Color::Black;
	return GraphTypes::Color::DarkGray;
}

void CRoxieDebugSession::GetColor(IGraphVertex * vertex, GraphTypes::Color &borderColor, GraphTypes::Color & fillColor, GraphTypes::Color & fontColor) const
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	borderColor = GraphTypes::Color::DarkGray;
	fillColor = GraphTypes::Color::White;
	fontColor = GraphTypes::Color::DarkGray;
	if (m_currActive == vertex->GetID())
	{
		borderColor = GraphTypes::Color::Green;
		fontColor = GraphTypes::Color::DarkGreen;
	}
	else if (m_currSel == vertex->GetID())
	{
		borderColor = GraphTypes::Color::DarkBlue;
		fillColor = GraphTypes::Color::LightBlue;
		fontColor = GraphTypes::Color::DarkBlue;
	}
	else if(m_currSelSet.find(vertex->GetID()) != m_currSelSet.end())
	{
		borderColor = GraphTypes::Color::Blue;
		fillColor = GraphTypes::Color::LightBlue;
		fontColor = GraphTypes::Color::Blue;
	}
	else if (m_state == DEBUGSTATE_COMPLETE)
	{
		borderColor = GraphTypes::Color::Gray;
		fontColor = GraphTypes::Color::Gray;
	}
	else
	{
		CComVariant val;
		vertex->GetProperty(PROP_ROXIESTARTED, val);
		if (val.vt == VT_BOOL && val.boolVal)
		{
			borderColor = GraphTypes::Color::Black;
			fontColor = GraphTypes::Color::Black;
		}
	}
}

void CRoxieDebugSession::GetColor(IGraphEdge * edge, GraphTypes::Color & edgeColor, GraphTypes::Color & fontColor, GraphTypes::Color & firstControlPointColor, GraphTypes::Color & lastControlPointColor) const
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	edgeColor = GraphTypes::Color::DarkGray;
	if (m_currActive == edge->GetID())
		edgeColor = GraphTypes::Color::Green;
	else if(m_currSel == edge->GetID())
		edgeColor = GraphTypes::Color::DarkBlue;
	else if(m_currSelSet.find(edge->GetID()) != m_currSelSet.end())
		edgeColor = GraphTypes::Color::Blue;
	else if (m_state == DEBUGSTATE_COMPLETE)
		edgeColor = GraphTypes::Color::Gray;
	else
	{
		CComVariant val;
		edge->GetProperty(PROP_ROXIESTARTED, val);
		if (val.vt == VT_BOOL && val.boolVal)
			edgeColor = GraphTypes::Color::Black;
	}

	roxie::CBreakpointMap::const_iterator find = m_breakpoints.find(edge->GetID());
	if (find != m_breakpoints.end() && find->second.size() > 0)
		lastControlPointColor = 0xFFFF0000;
	else
	{
		find = m_breakpoints.find(GetGlobalID(edge->GetID()));
		if (find != m_breakpoints.end() && find->second.size() > 0)
			lastControlPointColor = 0x60FF0000;
		else
		{
			find = m_computedBreakpoints.find(GetGlobalID(edge->GetID()));
			if (find != m_computedBreakpoints.end() && find->second.size() > 0)
				lastControlPointColor = 0x60FF0000;
		}
	}
	roxie::CSearchMap::const_iterator sfind = m_searchResults.find(edge->GetID());
	if (sfind != m_searchResults.end() && sfind->second.size() > 0)
		firstControlPointColor = 0x900000FF;

	fontColor = edgeColor;
}

void CRoxieDebugSession::SetSelected(const CUniqueID & id, bool forceDataLoad)
{
	boost::recursive_mutex::scoped_lock scope_lock(m_mutex);
	if (m_currSel != id || forceDataLoad)
	{
		m_currSel = id;
		m_currSelSet.clear();
		GetActiveID(m_currSel, m_currSelSet);
		m_currSelSet.insert(GetGlobalID(id));
		LoadInfo(id, 10, 0);
	}
}
