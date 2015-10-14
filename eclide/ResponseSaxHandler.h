#pragma once

#include "RoxieDebugSession.h"
#include "SeisintDatasetSAXHandler.h"

//  ===========================================================================
namespace roxie
{
//  ===========================================================================
class CException : public IException, public CUnknown
{
public:
    std::_tstring m_source;
    int m_code;
    std::_tstring m_message;

    BEGIN_CUNKNOWN
    END_CUNKNOWN(CUnknown)

    CException();
    CException(const std::_tstring & source, int code, const std::_tstring & message);

    //  IException  ---
    const TCHAR * GetSource() const;
    bool HasCode() const;
    int GetCode() const;
    const TCHAR * GetMessage() const;
};
//  ===========================================================================
class CDebug : public IDebug, public CUnknown
{
public:
    std::_tstring m_roxieCall;
    Element::StringStringMap m_attrs;

    bool m_hasData;
    bool m_finalData;
    ITableMap m_data;

    bool m_hasGraph;
    std::_tstring m_graph;

    roxie::CBreakpointMap m_breakpoints;
    roxie::CBreakpointMap m_impliedBreakpoints;
    roxie::CSearchMap m_searchResults;
    roxie::CCountMap m_countResults;

    BEGIN_CUNKNOWN
    END_CUNKNOWN(CUnknown)

    CDebug();
    ~CDebug();

    bool HasState() const;
    const TCHAR * GetState() const;
    CUniqueID GetActiveID() const;
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
//  ===========================================================================
}
//  ===========================================================================
class RoxieDebugResponseHandler : public StackHandlerImpl
{
protected:
    enum BRANCH
    {
        BRANCH_FIRST = 0,
        BRANCH_DEBUG,
        BRANCH_EXCEPTION,
        BRANCH_DATASET,
        BRANCH_WARNING,
        BRANCH_UNKNOWN
    } m_branch;
    StlLinked<roxie::CException> m_exception;
    StlLinked<roxie::CException> m_warning;
    StlLinked<roxie::CDebug> m_debug;

    CUniqueID m_breakID;
    roxie::CBreakpoint m_bp;

    CUniqueID m_searchEdgeID;
    roxie::CSearchRow m_searchRow;

public:
    RoxieDebugResponseHandler(roxie::CDebug * debug, roxie::CException *exception, roxie::CException *warning);
    virtual HRESULT STDMETHODCALLTYPE startElement(const wchar_t *pwchNamespaceUri, int cchNamespaceUri, const wchar_t *pwchLocalName, int cchLocalName, const wchar_t *pwchQName, int cchQName, ISAXAttributes *pAttributes);
    virtual HRESULT STDMETHODCALLTYPE endElement(const wchar_t *pwchNamespaceUri, int cchNamespaceUri, const wchar_t *pwchLocalName, int cchLocalName, const wchar_t *pwchQName, int cchQName);
};
//  ===========================================================================
