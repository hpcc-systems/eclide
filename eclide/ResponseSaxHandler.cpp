#include "StdAfx.h"

#include "ResponseSaxHandler.h"

CUniqueID CreateUniqueID(const std::_tstring & mode, const std::_tstring & id)
{
    if (boost::algorithm::iequals(_T("edge"), mode))
        return CUniqueID(guidDefault, XGMML_CAT_EDGE, id);
    else if (boost::algorithm::iequals(_T("graph"), mode))
        return CUniqueID(guidDefault, XGMML_CAT_SUBGRAPH, id);
    else if (boost::algorithm::iequals(_T("global"), mode))
        return CUniqueID(guidDefault, XGMML_CAT_GRAPH, id);
    ATLASSERT(false);
    return CUniqueID();
}
//  ===========================================================================
#define DEBUG_ATTR_STATE _T("state")
#define DEBUG_ATTR_GRAPHID _T("graphId")
#define DEBUG_ATTR_EDGEID _T("edgeId")
#define DEBUG_ATTR_NODEID _T("nodeId")
#define DEBUG_ATTR_ACTIVITY _T("activity")
#define DEBUG_ATTR_OUTPUTIDX _T("outputIdx")
#define DEBUG_ATTR_IDX _T("idx")
#define DEBUG_ATTR_ID _T("id")
#define DEBUG_ATTR_ACTION _T("action")
#define DEBUG_ATTR_MODE _T("mode")
#define DEBUG_ATTR_FIELDNAME _T("fieldName")
#define DEBUG_ATTR_CONDITION _T("condition")
#define DEBUG_ATTR_CASESENSITIVE _T("caseSensitive")
#define DEBUG_ATTR_VALUE _T("value")
#define DEBUG_ATTR_ROWCOUNT _T("rowCount")
#define DEBUG_ATTR_ROWCOUNTMODE _T("rowCountMode")
#define DEBUG_ATTR_GRAPHCHANGED _T("graphChanged")
#define DEBUG_ATTR_SEQUENCE _T("sequence")
#define DEBUG_ATTR_GRAPHSEQUENCENUM _T("graphSequenceNum")
#define DEBUG_ATTR_COUNT _T("count")
//  ===========================================================================
namespace roxie
{
//  ===========================================================================
CException::CException()
{
    m_code = -1;
}
CException::CException(const std::_tstring & source, int code, const std::_tstring & message) : m_source(source), m_code(code), m_message(message)
{
}
//  IException  ---
const TCHAR * CException::GetSource() const
{
    return m_source.c_str();
}
bool CException::HasCode() const
{
    return m_code != -1;
}
int CException::GetCode() const
{
    return m_code;
}
const TCHAR * CException::GetMessage() const
{
    return m_message.c_str();
}
//  ===========================================================================
CDebug::CDebug()
{
    m_finalData = false;
    m_hasData = false;
    m_hasGraph = false;
    m_data[_T("_")].m_table = CreateISparseTable();
    m_data[_T("_")].m_table->SetColumn(0, _T("##"), CT_UINT);//Bit of a hack to get the row num in col 0;
}

CDebug::~CDebug()
{
}

bool CDebug::HasState() const
{
    Element::StringStringMap::const_iterator itr = m_attrs.find(DEBUG_ATTR_STATE);
    if (itr != m_attrs.end())
        return true;
    return false;
}

const TCHAR * CDebug::GetState() const
{
    Element::StringStringMap::const_iterator itr = m_attrs.find(DEBUG_ATTR_STATE);
    if (itr != m_attrs.end())
        return itr->second.c_str();
    return _T("");
}

CUniqueID CDebug::GetActiveID() const
{
    Element::StringStringMap::const_iterator findMode = m_attrs.find(DEBUG_ATTR_MODE);
    Element::StringStringMap::const_iterator findId = m_attrs.find(DEBUG_ATTR_ID);
    if (findMode != m_attrs.end() && findId != m_attrs.end())
    {
        return CreateUniqueID(findMode->second, findId->second);
    }
    Element::StringStringMap::const_iterator findEdgeId = m_attrs.find(DEBUG_ATTR_EDGEID);
    if (findEdgeId != m_attrs.end())
        return CUniqueID(guidDefault, XGMML_CAT_EDGE, findEdgeId->second);
    Element::StringStringMap::const_iterator findNodeId = m_attrs.find(DEBUG_ATTR_NODEID);
    if (findNodeId != m_attrs.end())
        return CUniqueID(guidDefault, XGMML_CAT_VERTEX, findNodeId->second);
    Element::StringStringMap::const_iterator findGraphId = m_attrs.find(DEBUG_ATTR_GRAPHID);
    if (findGraphId != m_attrs.end())
        return CUniqueID(guidDefault, XGMML_CAT_SUBGRAPH, findGraphId->second);
    return CUniqueID();
}

bool CDebug::HasData() const
{
    return m_hasData;
}

const ITableMap * CDebug::GetData() const
{
    return &m_data;
}

const ITable * CDebug::GetData(const std::_tstring & id) const
{
    ITableMap::const_iterator itr = m_data.find(id);
    if (itr != m_data.end())
        return itr->second.m_table.get();
    return NULL;
}

bool CDebug::HasGraph() const
{
    return m_hasGraph;
}

const TCHAR * CDebug::GetGraph() const
{
    return m_graph.c_str();
}

int CDebug::GetSequence() const
{
    Element::StringStringMap::const_iterator itr = m_attrs.find(DEBUG_ATTR_SEQUENCE);
    if (itr != m_attrs.end())
    {
        try
        {
            return boost::lexical_cast<int>(itr->second.c_str());
        }
        catch(boost::bad_lexical_cast &)
        {
        }
    }
    return 0;
}

int CDebug::GetGraphSequenceNum() const
{
    Element::StringStringMap::const_iterator itr = m_attrs.find(DEBUG_ATTR_GRAPHSEQUENCENUM);
    if (itr != m_attrs.end())
    {
        try
        {
            return boost::lexical_cast<int>(itr->second.c_str());
        }
        catch(boost::bad_lexical_cast &)
        {
        }
    }
    return 0;
}

void CDebug::GetBreakpoints(CBreakpointMap & breakpoints) const
{
    breakpoints = m_breakpoints;
}

void CDebug::GetSearchResults(CSearchMap & searchResults) const
{
    searchResults = m_searchResults;
}

void CDebug::GetCountResults(CCountMap & countResults) const
{
    countResults = m_countResults;
}
//  ===========================================================================
}
//  ===========================================================================
RoxieDebugResponseHandler::RoxieDebugResponseHandler(roxie::CDebug * debug, roxie::CException *exception, roxie::CException *warning)
{
    m_branch = BRANCH_UNKNOWN;
    m_debug = debug;
    m_exception = exception;
    m_warning = warning;
}

HRESULT STDMETHODCALLTYPE RoxieDebugResponseHandler::startElement( 
    /* [in] */ const wchar_t *pwchNamespaceUri,
    /* [in] */ int cchNamespaceUri,
    /* [in] */ const wchar_t *pwchLocalName,
    /* [in] */ int cchLocalName,
    /* [in] */ const wchar_t *pwchQName,
    /* [in] */ int cchQName,
    /* [in] */ ISAXAttributes *pAttributes)
{ 
    __super::startElement(pwchNamespaceUri, cchNamespaceUri, pwchLocalName, cchLocalName, pwchQName, cchQName, pAttributes);
    Element &e = m_stack.top();

    switch (m_stack.size())
    {
    case 1: //Debug
        break;
    case 2: //Debug, Exception
        if (e.m_tag.compare(_T("Exception")) == 0)
            m_branch = BRANCH_EXCEPTION;
        else if (e.m_tag.compare(_T("Debug")) == 0)
            m_branch = BRANCH_DEBUG;
        else if (e.m_tag.compare(_T("Dataset")) == 0)
        {
            m_branch = BRANCH_DATASET;
            m_debug->m_finalData = true;
        }
        else
            m_branch = BRANCH_UNKNOWN;
        break;
    case 3: //step, continue etc.
        switch (m_branch)
        {
        case BRANCH_DEBUG:
            m_debug->m_roxieCall = e.m_tag;
            break;
        }
        break;
    case 4: 
        switch (m_branch)
        {
        case BRANCH_DEBUG:
            if (e.m_tag.compare(_T("Exception")) == 0)
                m_branch = BRANCH_WARNING;
            else if (e.m_tag.compare(_T("break")) == 0)
            {
                try
                {
                    m_breakID = CreateUniqueID(e.m_attr[DEBUG_ATTR_MODE], e.m_attr[DEBUG_ATTR_ID]);
                    m_bp.m_idx = e.m_attr[DEBUG_ATTR_IDX];
                    m_bp.m_id = e.m_attr[DEBUG_ATTR_ID];
                    m_bp.m_action = e.m_attr[DEBUG_ATTR_ACTION];
                    m_bp.m_fieldName = e.m_attr[DEBUG_ATTR_FIELDNAME];
                    m_bp.m_condition = e.m_attr[DEBUG_ATTR_CONDITION];
                    m_bp.m_caseSensitive = e.m_attr[DEBUG_ATTR_CASESENSITIVE];
                    m_bp.m_value = e.m_attr[DEBUG_ATTR_VALUE];
                    m_bp.m_rowCount = e.m_attr[DEBUG_ATTR_ROWCOUNT];
                    m_bp.m_rowCountMode = e.m_attr[DEBUG_ATTR_ROWCOUNTMODE];
                }
                catch(boost::bad_lexical_cast &)
                {
                    ATLASSERT(!"Unexpected Data");
                }
            }
            else if (boost::algorithm::iequals(e.m_tag, _T("edge")))
            {
                m_searchEdgeID = CreateUniqueID(e.m_tag, e.m_attr[DEBUG_ATTR_EDGEID]);
                Element::StringStringMap::const_iterator found = e.m_attr.find(DEBUG_ATTR_COUNT);
                if (found != e.m_attr.end())
                    m_debug->m_countResults[m_searchEdgeID] = found->second;
            }
        }
        break;
    case 5: 
        switch (m_branch)
        {
        case BRANCH_DEBUG:
            if (e.m_tag.compare(_T("Row")) == 0)
                m_searchRow = roxie::CSearchRow();
            break;
        }
        break;
    case 6:
        break;
    }
    return S_OK; 
}

HRESULT STDMETHODCALLTYPE RoxieDebugResponseHandler::endElement( 
    /* [in] */ const wchar_t *pwchNamespaceUri,
    /* [in] */ int cchNamespaceUri,
    /* [in] */ const wchar_t *pwchLocalName,
    /* [in] */ int cchLocalName,
    /* [in] */ const wchar_t *pwchQName,
    /* [in] */ int cchQName)
{ 
    Element & e = m_stack.top();
    switch (m_branch)
    {
    case BRANCH_DEBUG:
        switch (m_stack.size())
        {
        case 1:
        case 2:
            break;
        case 3:
            m_debug->m_attrs = e.m_attr;
            break;
        case 4:
            if (e.m_tag.compare(_T("Row")) == 0)
                m_debug->m_hasData = true;
            else if (e.m_tag.compare(_T("break")) == 0)
            {
                m_debug->m_breakpoints[m_breakID].push_back(m_bp);
                m_breakID = CUniqueID();
                m_bp = roxie::CBreakpoint();
            }
            else if (e.m_tag.compare(_T("Graph")) == 0)
                m_debug->m_hasGraph = true;
            else if (e.m_tag.compare(_T("edge")) == 0)
                m_searchEdgeID = CUniqueID();
            break;
        case 5://AllGraph
            if (e.m_tag.compare(_T("Graph")) == 0)
                m_debug->m_hasGraph = true;
            else if (e.m_tag.compare(_T("edge")) == 0)
                m_debug->m_impliedBreakpoints[CUniqueID(guidDefault, XGMML_CAT_EDGE, e.m_attr[DEBUG_ATTR_EDGEID])].push_back(m_bp);
            else if (e.m_tag.compare(_T("Row")) == 0)
                try
            {
                m_searchRow.m_count = boost::lexical_cast<int>(e.m_attr[DEBUG_ATTR_COUNT]);
            }
            catch(boost::bad_lexical_cast &)
            {
            }
            m_debug->m_searchResults[m_searchEdgeID].push_back(m_searchRow);
            m_searchRow = roxie::CSearchRow();
            break;
        case 6:
            m_searchRow.m_cols[e.m_tag] = e.GetContent();
            break;
        }
        break;
    case BRANCH_EXCEPTION:
    case BRANCH_WARNING:
        {
            roxie::CException * result = m_branch == BRANCH_EXCEPTION ? m_exception : m_warning;
            if (e.m_tag.compare(_T("Source")) == 0)
                result->m_source = e.GetContent();
            else if(e.m_tag.compare(_T("Code")) == 0)
            {
                try
                {
                    result->m_code = boost::lexical_cast<int>(e.GetContent());
                }
                catch (boost::bad_lexical_cast &)
                {
                }
            }
            else if(e.m_tag.compare(_T("Message")) == 0)
                result->m_message = e.GetContent();

            switch (m_stack.size())
            {
            case 4:
                if (e.m_tag.compare(_T("Exception")) == 0)
                    m_branch = BRANCH_DEBUG;
                break;
            }
        }
        break;
    }
    return __super::endElement(pwchNamespaceUri, cchNamespaceUri, pwchLocalName, cchLocalName, pwchQName, cchQName);
}
//  ===========================================================================
