#include "StdAfx.h"
#include "Graph.h"

#include "workunit.h"
#include "dali.h"
#include "SoapUtil.h"
#include "cache.h"

#if _COMMS_VER < 68200
using namespace WsWorkunits;
#elif _COMMS_VER < 700000
#else
using namespace WsECLWorkunits;
typedef ECLWUGraph ECLGraphEx;
#endif

const TCHAR * const WUVS_RUNNING = _T("Running");
const TCHAR * const WUVS_COMPLETED = _T("Completed");
const TCHAR * const WUVS_FAILED = _T("Failed");
const TCHAR * const WUVS_UNKNOWN = _T("");

namespace Dali
{

class CGraph : public IGraph, public clib::CLockableUnknown, public boost::noncopyable
{
protected:
	int m_seq;
	CString m_id;
	CString m_url;
	CString m_wuid;
    CString m_name;
    CString m_label;
    CString m_graph;
    CString m_type;
    bool m_running;
    bool m_complete;
    bool m_failed;
    bool m_loaded;
    __int64 m_runningId;
	graph_signal_type on_refresh;

public:
	IMPLEMENT_CLOCKABLEUNKNOWN;

	CGraph()
	{
		ATLASSERT(false);
	}
	CGraph(const CString & url, const CString & wuid, const CString & name, int seq) : m_url(url), m_wuid(wuid), m_name(name), m_seq(seq)
	{
		m_id = m_url + _T("/") + m_wuid + _T("/") + m_name;
	    m_running = false;
	    m_complete = false;
	    m_failed = false;
	    m_loaded = false;
		m_runningId = 0;
	}
	virtual ~CGraph()
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
	int GetSequence() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_seq;
	}
	const TCHAR *GetName() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_name;
	}
	const TCHAR *GetLabel() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_label;
	}
	const TCHAR *GetType() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_type;
	}
	const TCHAR *GetGraph()
	{
		if (!IsLoaded())
		{
			StlLinked<IDali> server;
			server = AttachDali(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT), _T("Dali"));
			server->GetGraph(m_wuid, m_name);
		}
		return m_graph;
	}
	WUVisualState GetState() const
	{
		if (m_running)
			return WUVisualState_running;
		if (m_complete)
			return WUVisualState_completed;
		if (m_failed)
			return WUVisualState_failed;
		return WUVisualState_unknown;
	}
	const TCHAR *GetStateString() const
	{
		switch(GetState())
		{
		case WUVisualState_running:
			return WUVS_RUNNING;
		case WUVisualState_completed:
			return WUVS_COMPLETED;
		case WUVisualState_failed:
			return WUVS_FAILED;
		}
		return WUVS_UNKNOWN;
	}
	bool IsRunning() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return GetState() == WUVisualState_running;
	}
	bool IsLoaded() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return !m_graph.IsEmpty();
	}
	bool IsComplete() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_complete;
	}
	bool Refresh()
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		if (!IsComplete() || !IsLoaded())
		{
			StlLinked<IDali> server;
			server = AttachDali(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT), _T("Dali"));
			server->GetGraph(m_wuid, m_name);
			return true;
		}
		return false;
	}
#if _COMMS_VER < 68200
	void Update(const ECLGraph * c)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		bool changed = false;
		if (m_name.Compare(CW2T(c->Name, CP_UTF8)) != 0 ||
			m_label.Compare(CW2T(c->Label, CP_UTF8)) != 0 ||
			m_type.Compare(CW2T(c->Type, CP_UTF8)) != 0 ||
#if _COMMS_VER < 700000
			m_running != c->Running ||
			m_runningId != c->RunningId)
#else
			m_graph.Compare(CW2T(c->XGMMLTree, CP_UTF8)) != 0 ||
			m_running != c->IsRunning ||
			m_runningId != c->RunningID)
#endif
		{
			changed = true;
		}
		m_name = CW2T(c->Name, CP_UTF8);
		m_label = CW2T(c->Label, CP_UTF8);
		m_type = CW2T(c->Type, CP_UTF8);
#if _COMMS_VER < 700000
		m_running = c->Running;
		m_complete = c->Complete;
		m_failed = c->Failed;
		m_runningId = c->RunningId;
#else
		m_graph = CW2T(c->XGMMLTree, CP_UTF8);
		m_running = c->IsRunning;
		m_runningId = c->RunningID;
#endif
		if (changed)
		{
			proc.unlock();
			Broadcast();
		}
	}
	void Update(const ECLGraphEx * c)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		bool changed = false;
		if (m_name.Compare(CW2T(c->Name, CP_UTF8)) != 0 ||
			m_label.Compare(CW2T(c->Label, CP_UTF8)) != 0 ||
			m_type.Compare(CW2T(c->Type, CP_UTF8)) != 0 ||
#if _COMMS_VER < 700000
			m_graph.Compare(CW2T(c->Graph, CP_UTF8)) != 0 ||
			m_running != c->Running ||
			m_runningId != c->RunningId)
#else
			m_graph.Compare(CW2T(c->XGMMLTree, CP_UTF8)) != 0 ||
			m_running != c->IsRunning ||
			m_runningId != c->RunningID)
#endif
		{
			changed = true;
		}
		m_name = CW2T(c->Name, CP_UTF8);
		m_label = CW2T(c->Label, CP_UTF8);
		m_type = CW2T(c->Type, CP_UTF8);
#if _COMMS_VER < 700000
		m_graph = CW2T(c->Graph, CP_UTF8);
		m_running = c->Running;
		m_runningId = c->RunningId;
#else
		m_graph = CW2T(c->XGMMLTree, CP_UTF8);
		m_running = c->IsRunning;
		m_runningId = c->RunningID;
#endif
		if (changed)
		{
			proc.unlock();
			Broadcast();
		}
	}
#else
	void Update(const ns6__ECLGraph * c)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		CString prev_name = m_name;
		CString prev_label = m_label;
		CString prev_type = m_type;
		bool prev_running = m_running;
		bool prev_complete = m_complete;
		bool prev_failed= m_failed;
		__int64 prev_runningId = m_runningId;

		SAFE_ASSIGN2CSTRING(m_name, c->Name);
		SAFE_ASSIGN2CSTRING(m_label, c->Label);
		SAFE_ASSIGN2CSTRING(m_type, c->Type);
	    m_running = false;
	    m_complete = false;
	    m_failed = false;
		SAFE_ASSIGN(m_running, c->Running);
		SAFE_ASSIGN(m_complete, c->Complete);
		SAFE_ASSIGN(m_failed, c->Failed);
		SAFE_ASSIGN(m_runningId, c->RunningId);

		if (m_name.Compare(prev_name) != 0 ||
			m_label.Compare(prev_label) != 0 ||
			m_type.Compare(prev_type) != 0 ||
			m_running != prev_running ||
			m_complete != prev_complete ||
			m_failed != prev_failed ||
			m_runningId != prev_runningId)
		{
			proc.unlock();
			Broadcast();
		}
	}
	void Update(const ns6__ECLGraphEx * c)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		CString prev_name = m_name;
		CString prev_label = m_label;
		CString prev_type = m_type;
		CString prev_graph = m_graph;
		bool prev_running = m_running;
		__int64 prev_runningId = m_runningId;

		SAFE_ASSIGN2CSTRING(m_name, c->Name);
		SAFE_ASSIGN2CSTRING(m_label, c->Label);
		SAFE_ASSIGN2CSTRING(m_type, c->Type);
		SAFE_ASSIGN2CSTRING(m_graph, c->Graph);
		SAFE_ASSIGN(m_running, c->Running);
		SAFE_ASSIGN(m_runningId, c->RunningId);

		if (m_name.Compare(prev_name) != 0 ||
			m_label.Compare(prev_label) != 0 ||
			m_type.Compare(prev_type) != 0 ||
			m_graph.Compare(prev_graph) != 0 ||
			m_running != prev_running ||
			m_runningId != prev_runningId)
		{
			proc.unlock();
			Broadcast();
		}
	}
#endif
	void Update(const std::_tstring & xgmml)
	{
#if _COMMS_VER < 700000
		m_graph = xgmml.c_str();
		m_running = false;
		m_runningId = false;
		m_complete = true;
#endif
	}
	void Broadcast() 
	{
		on_refresh(this);
	}

	boost::signals::connection on_refresh_connect(const graph_slot_type& s)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return on_refresh.connect(s); 
	}
};

static CacheT<std::_tstring, CGraph> GraphCache;

void ClearGraphCache()
{
	ATLTRACE(_T("Graph cache before clearing(size=%u)\r\n"), GraphCache.Size());
	GraphCache.Clear();
}

CGraph * CreateGraphRaw(const CString & url, const CString & wuid, const CString & name, int seq)
{
	return GraphCache.Get(new CGraph(url, wuid, name, seq));
}

#if _COMMS_VER < 68200
IGraph * CreateGraph(const CString& url, const CString & wuid, const ECLGraph * data, int seq)
{
	CComPtr<CGraph> graph = CreateGraphRaw(url, wuid, data->Name, seq);
	ATLASSERT(graph);
	graph->Update(data);
	return graph;
}
IGraph * CreateGraph(const CString & url, const CString & wuid, const ECLGraphEx * data)
{
	CComPtr<CGraph> graph = CreateGraphRaw(url, wuid, data->Name, -1);
	ATLASSERT(graph);
	graph->Update(data);
	return graph;
}
#else
IGraph * CreateGraph(const CString& url, const CString & wuid, const ns6__ECLGraph * data, int seq)
{
	CComPtr<CGraph> graph = CreateGraphRaw(url, wuid, data->Name->c_str(), seq);
	ATLASSERT(graph);
	graph->Update(data);
	return graph;
}
IGraph * CreateGraph(const CString& url, const CString & wuid, const ns6__ECLGraphEx * data)
{
	CComPtr<CGraph> graph = CreateGraphRaw(url, wuid, data->Name->c_str(), -1);
	ATLASSERT(graph);
	graph->Update(data);
	return graph;
}
#endif
IGraph * CreateGraph(const CString& url, const CString & wuid, const std::_tstring & xgmml)
{
	CComPtr<CGraph> graph = CreateGraphRaw(url, wuid, _T("File"), 0);
	ATLASSERT(graph);
	graph->Update(xgmml);
	return graph;
}
} //namespace Dali

