#pragma once

#include "TestSocket.h"
#include "dali.h"
#include "XGMMLSaxHandler.h"
//#include "atlGraphView.h"
//  ===========================================================================
namespace roxie
{
__interface IException : public IUnknown
{
	const TCHAR * GetSource() const;
	bool HasCode() const;
	int GetCode() const;
	const TCHAR * GetMessage() const;
};

class CBreakpoint
{
public:
	std::_tstring m_idx;
	std::_tstring m_id;
	std::_tstring m_action;
	std::_tstring m_fieldName;
	std::_tstring m_condition;
	std::_tstring m_caseSensitive;
	std::_tstring m_value;
	std::_tstring m_rowCount;
	std::_tstring m_rowCountMode;
};
typedef std::vector<CBreakpoint> CBreakpointVector;
typedef std::map<CUniqueID, CBreakpointVector> CBreakpointMap;

class CSearchRow
{
public:
	int m_count;
	Element::StringStringMap m_cols;
	CSearchRow()
	{
		m_count = -1;
	}
};
typedef std::vector<CSearchRow> CSearchRowVector;
typedef std::map<CUniqueID, CSearchRowVector> CSearchMap;
typedef std::map<CUniqueID, std::_tstring> CCountMap;

__interface IDebug : public IUnknown
{
	bool HasState() const;
	const TCHAR * GetState() const;

	bool HasData() const;
	const ITableMap * GetData() const;
	const ITable * GetData(const std::_tstring & id) const;

	bool HasGraph() const;
	const TCHAR * GetGraph() const;

	int GetSequence() const;
	int GetGraphSequenceNum() const;
	void GetBreakpoints(CBreakpointMap & breakpoints) const;
	void GetSearchResults(CSearchMap & searchResults) const;
	void GetCountResults(CCountMap & countResults) const;
};

class Response
{
public:
	StlLinked<IDebug> m_debug;
	StlLinked<IException> m_exception;
	StlLinked<IException> m_warning;

	Response()
	{
	}
};

enum CALL
{
	CALL_UNKNOWN = 0,
	CALL_START,
	CALL_BREAK,
	CALL_DETACH,
	CALL_ABORT,
	CALL_CONTINUE,
	CALL_STEP,
	CALL_NEXT,
	CALL_STEPGRAPH,
	CALL_STEPGRAPHSTART,
	CALL_STEPGRAPHEND,
	CALL_CHANGES,
	CALL_COUNTS,
	CALL_INFO,
	CALL_WHERE,
	CALL_ALLGRAPH,
	CALL_ACTIVEGRAPH,
	CALL_ADDBREAKPOINT,
	CALL_DELETEBREAKPOINT,
	CALL_GETBREAKPOINT,
	CALL_GETBREAKPOINTS,
	CALL_SET,
	CALL_STATUS,
	CALL_SEARCH,
	CALL_LAST
};

static TCHAR* CALL_DESC[] = 
{ 
	_T("Unknown"),
	_T("Start"),
	_T("Break"),
	_T("Detach"),
	_T("Abort"),
	_T("Continue"),
	_T("Step"),
	_T("Next"),
	_T("Step Graph"),
	_T("Step Graph Start"),
	_T("Step Graph End"),
	_T("Changes"),
	_T("Counts"),
	_T("Info"),
	_T("Where"),
	_T("All Graph"),
	_T("Active Graph"),
	_T("Add Breakpoint"),
	_T("Delete Breakpoint"),
	_T("Get Breakpoint"),
	_T("Get Breakpoints"),
	_T("Set"),
	_T("Status"),
	_T("Search"),
	_T("Last")
};
}

__interface IRoxieDebugSessionReceiver
{
	void PostCallStart(roxie::CALL callMode);
	void PostCallEnd(roxie::CALL callMode, const roxie::Response & response);
};

typedef std::map<CUniqueID, CUniqueID> CUniqueIDCUniqueIDMap;
typedef std::set<CUniqueID> CUniqueIDSet;
typedef std::map<CUniqueID, CUniqueIDSet> CUniqueIDCUniqueIDSetMap;
class CRoxieDebugSession : public CUnknown, public IXGMMLRenderCallback
{
friend void submitRequest(CRoxieDebugSession * self, std::string ip, std::string port, std::_tstring request, roxie::CALL callMode, roxie::Response & response);
friend void thread_SubmitRequest(CRoxieDebugSession * self, std::string ip, std::string port, std::_tstring request, roxie::CALL callMode);

protected:
	mutable boost::recursive_mutex m_mutex;
	IRoxieDebugSessionReceiver * m_owner;
	CSessionID m_session;
	std::string m_ip;
	std::string m_port;
	StlLinked<Dali::IDali> m_dali;
	StlLinked<Dali::IWorkunit> m_wu;
	std::_tstring m_wuid;

	bool m_hasResponse;
	std::_tstring m_response;

	bool m_inDebugSession;
	bool m_graphEnd;

	CUniqueID m_currSel;
	CUniqueIDSet m_currSelSet;
	CUniqueID m_currActive;
	int m_sequence;
	int m_graphSequenceNum;

	roxie::CBreakpointMap m_breakpoints;
	roxie::CBreakpointMap m_computedBreakpoints;	//When you set a global breakpoint, it returns the list of affected edges...
	roxie::CSearchMap m_searchResults;
	roxie::CCountMap m_countResults;

	mutable Dali::IResultMap m_visibleResultsMap;
	mutable Dali::IResultVector m_visibleResultsVector;

	Dali::IResultMap m_resultsMap;
	Dali::IResultVector m_resultsVector;

	Dali::IGraphMap m_graphsMap;
	Dali::IGraphVector m_graphsVector;

	LONG m_activeThreadCount;
	
	mutable CUniqueIDCUniqueIDMap m_globalIDCache;
	mutable CUniqueIDCUniqueIDSetMap m_activeIDCache;

public:
	enum DEBUGSTATE
	{
		DEBUGSTATE_UNKNOWN = 0,
		DEBUGSTATE_CONTINUE,
		DEBUGSTATE_DETACH,
		DEBUGSTATE_PAUSED,
		DEBUGSTATE_COMPLETE,
		DEBUGSTATE_LAST
	} m_state;
	BEGIN_CUNKNOWN
		IMPLEMENT_INTERFACE(IXGMMLRenderCallback)
		//IMPLEMENT_INTERFACE(Dali::IWorkunit)
	END_CUNKNOWN(CUnknown)
	//10.173.217.1:9876
	//std::string requestText = "<full_test><_debugId>99</_debugId></full_test>";
	CRoxieDebugSession(IRoxieDebugSessionReceiver * owner);

	void Clear();
	int GetActiveThreadCount() const;

	const CUniqueID & GetActiveID();
	const CUniqueID & GetCurrentID();
	const CUniqueID & GetGlobalID(const CUniqueID & id) const;
	unsigned int GetActiveID(const CUniqueID & id, CUniqueIDSet & results) const;

	void GetSearchResult(const CUniqueID & id, roxie::CSearchRowVector & rows) const;
	const roxie::CSearchMap & GetSearchResults() const;
	const roxie::CCountMap & GetCountResults() const;

	//Main Query
	void StartSession(const std::string & ip, const std::string & port, const std::_tstring & queryLabel, const std::_tstring & queryBody = _T(""), bool debug = false);
	void AttachSession(const std::string & ip, const std::string & port, const std::_tstring & session);
	void AttachEspSession(const std::_tstring & cfg, const std::_tstring & id, const std::_tstring & pw, const std::_tstring & session);
	void AttachEspSession(Dali::IWorkunit * wu);
	bool CanStartSessionXXX() const;
	bool IsSessionCompleteXXX() const;
	bool IsComplete() const;
	bool IsInDebugSessionXXX() const;
	DEBUGSTATE GetDebugState() const;
	void SetDebugState(const DEBUGSTATE & state);
	void PostCallStart(roxie::CALL callMode);
	void PostCallEnd(roxie::CALL callMode, const roxie::Response & response);
	const TCHAR * GetResponse() const;

	void Set(const std::_tstring & key, const std::_tstring & value);
	void GetStatus();
	void GetStatus(roxie::Response & response);
	void GetGlobalGraph();
	void GetActiveGraph();

	void Abort();
	void Detach();
	void BreakAll();
	void Continue();
	void Continue(const CUniqueID & id);

	void Step();
	void Next();

	void StepGraph();
	void StepGraphStart();
	void StepGraphEnd();

	void RefreshCounts(int sequence, roxie::Response & response, bool detach);

	void AddBreakpoint(const std::_tstring & id, std::_tstring action, std::_tstring expressionMode, std::_tstring field, std::_tstring expression, std::_tstring countMode, std::_tstring count, bool caseSensitive);
	bool HasBreakpoint(const std::_tstring & id) const;
	unsigned int GetBreakpointCount() const;
	void DeleteAllBreakpoints();
	void DeleteBreakpoint(const CUniqueID & id);
	void DeleteBreakpoint(const std::_tstring & id, bool threaded = true);
	void GetBreakpoint(unsigned int idx);
	void GetAllBreakpoints();

	void Search(std::_tstring fieldName, std::_tstring condition, std::_tstring value, bool caseSensitive, bool fullRow);

	void GetActiveInfo(unsigned int outputIdx, unsigned int numRows, unsigned int startRow);
	void SetSelected(const CUniqueID & id, bool forceDataLoad);
	void LoadInfo(const CUniqueID & id, unsigned int numRows, unsigned int startRow);
	void GetChanges(int sequence);
	void GetCounts(int sequence);
	void Where();

	//  ILockableUnknown Virtuals
	void Lock(CComPtr<clib::scoped_lock_ref_counted> & lock);

	//  IXGMMLRenderCallback Virtuals
	GraphTypes::Color GetColor(IGraphSubgraph * subgraph) const;
	void GetColor(IGraphVertex * vertex, GraphTypes::Color &borderColor, GraphTypes::Color & fillColor, GraphTypes::Color & fontColor) const;
	void GetColor(IGraphEdge * edge, GraphTypes::Color & edgeColor, GraphTypes::Color & fontColor, GraphTypes::Color & firstControlPointColor, GraphTypes::Color & lastControlPointColor) const;

	const TCHAR * GetEdgeTooltip(const CUniqueID & id, std::_tstring & tooltip) const;

protected:
	void SetResponse(const std::_tstring & response);
	void LoadResults(const std::_tstring & xml);
	void LoadResults(const std::_tstring & id, const ITableMap * resultTableMap);
};
//  ===========================================================================
