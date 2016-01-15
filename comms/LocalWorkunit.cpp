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
#include "SeisintDatasetSaxHandler.h"
#include "EclCC.h"
#include <thread.h>

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
IResult * CreateLocalResult(const CString & url, const CString & wuid, const std::_tstring & label, int sequence, ITable * result);
IGraph * CreateGraph(const CString& url, const CString & wuid, const std::_tstring & xgmml);

class CLocalWorkunit : public IWorkunit, public clib::CLockableUnknown
{
    friend void thread_LoadResults(const CLocalWorkunit * self)
    {
        if (!self->m_resultsLoaded)
        {
            self->m_resultsLoaded = true;
            CComPtr<IEclCC> eclcc = CreateIEclCC();
            if (eclcc)
            {
                std::_tstring results;
                bool hasErrors = false;
                CEclExceptionVector errors;
                eclcc->GetWorkunitResults(self->GetWuid(), false, results, hasErrors, errors);
                CLocalWorkunit * self2 = const_cast<CLocalWorkunit * >(self);
                self2->Update(_T(""), results, errors);
            }
        }
    }
protected:
    CString m_id;
    CString m_url;
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
    CString m_schedule;
    bool m_protected;
    bool m_followup;
    bool m_archived;
    int m_resultLimit;
    int m_maxRuntime;
    CString m_debugString;
    bool m_archiveCpp;
    bool m_noCommonPrivateAttributes;
    mutable bool m_resultsLoaded;

    IResultMap m_resultsMap;
    IResultVector m_resultsVector;
    IGraphMap m_graphsMap;
    IGraphVector m_graphsVector;
    ILocalFileVector m_localFiles;
    CEclExceptionVector m_exceptions;
    workunit_signal_type on_refresh;
    int m_filesToUploadCount;

public:
    IMPLEMENT_CLOCKABLEUNKNOWN;

    static const TCHAR * CreateWUID(CString & wuid)
    {
        static CString prevWuid;
        static int prevSuffix = 0;

        boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
        wuid = _T("L");
        wuid += boost::posix_time::to_iso_wstring(now).c_str();
        wuid.Replace(_T("T"), _T("-"));

        if (wuid.Compare(prevWuid) == 0)
        {
            wuid += _T("-");
            wuid += boost::lexical_cast<std::_tstring>(++prevSuffix).c_str();
        }
        else
        {
            prevWuid = wuid;
            prevSuffix = 0;
        }
        return wuid;
    }

    CLocalWorkunit(const CString & url, const CString & wuid) : m_url(url), m_wuid(wuid)
    {
        if (m_wuid.IsEmpty())
            CreateWUID(m_wuid);

        m_id = m_url + _T("/") + m_wuid;
        m_label = m_wuid;
        m_resultLimit = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_WORKUNIT_RESULTLIMIT);
        m_maxRuntime = 0;
        m_stateID = WUStateCompleted;
        m_protected = false;
        m_followup = false;
        m_archived = false;
        m_filesToUploadCount = 0;
        m_resultsLoaded = false;
    }
    virtual ~CLocalWorkunit()
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
        return m_schedule;
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
    bool GetArchived() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_archived;
    }
    bool GetFollowup() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_followup;
    }
    bool IsComplete() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
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
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        clib::thread run(__FUNCTION__, boost::bind(&thread_LoadResults, this));
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
        clib::recursive_mutex::scoped_lock proc(m_mutex);
    }
    void RefreshGraph() 
    {
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
    }
    int GetGraphTimingCount() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return 0;
    }
    const CGraphTiming * GetGraphTiming(unsigned item) const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return NULL;
    }
    int GetGraphTimings(CGraphTimingVector & results) const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
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
    void Update(const std::_tstring & wuXml, const std::_tstring & _results, const Dali::CEclExceptionVector & errors)
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        if (!errors.empty())
            m_exceptions = errors;

        for(Dali::CEclExceptionVector::const_iterator itr = errors.begin(); itr != errors.end(); ++itr)
        {
            if (boost::algorithm::iequals(static_cast<const TCHAR *>(itr->get()->m_severity), _T("Error")))
            {
                m_stateID = WUStateFailed;
                break;
            }
        }

        if (m_stateID != WUStateFailed && !_results.empty())
        {
            CComInitialize com;
            ITableMap resultMap;
            CComPtr<ISAXXMLReader> pRdr; 

            HRESULT hr = pRdr.CoCreateInstance(CLSID_SAXXMLReader30); 

            CComPtr<SeisintDatasetHandlerImpl> pMc = new SeisintDatasetHandlerImpl(resultMap); 
            hr = pRdr->putContentHandler(pMc); 

            std::_tstring results = _T("<Results>") + _results + _T("</Results>");

            CComVariant xml(results.c_str());
            hr = pRdr->parse(xml);
            ATLASSERT(hr == S_OK);
            int i = 0;
            for (ITableMap::const_iterator itr = resultMap.begin();itr != resultMap.end(); ++itr)
            {
                m_resultsMap[itr->first] = CreateLocalResult(m_url, m_wuid, itr->first, i, itr->second.m_table);
                m_resultsVector.push_back(m_resultsMap[itr->first]);
                ++i;
            }
            m_resultsLoaded = true;
        }

        if (m_stateID != WUStateFailed && !wuXml.empty())
        {
            CComBSTR bstrXML = wuXml.c_str();

            CComInitialize com;
            CComPtr<IXMLDOMDocument> pDoc;
            pDoc.CoCreateInstance(CLSID_DOMDocument30);
            VARIANT_BOOL r;
            pDoc->put_async(VARIANT_FALSE);
            pDoc->loadXML(bstrXML, &r);
            if (r != VARIANT_TRUE)
            {
                IXMLDOMParseError *pXMLErr=NULL;
                CComBSTR bstr = NULL;

                pDoc->get_parseError(&pXMLErr);
                pXMLErr->get_reason(&bstr);
                //_DBGLOG(m_url, LEVEL_WARNING, (boost::_tformat(_T("Restoring Workspace:  %1% %2%")) % std::_tstring(filePath) % std::_tstring(CW2T(bstr))).str().c_str());
                return;
            }

            CComPtr<IXMLDOMNodeList> graphs;
            HRESULT hr = pDoc->selectNodes(CComBSTR(_T("//Graphs/Graph")), &graphs);
            if (hr != S_OK)
                return;

            long length;
            graphs->get_length(&length);
            for(long i = 0; i < length;  ++i)
            {
                CComPtr<IXMLDOMNode> graph;
                hr = graphs->get_item(i, &graph);
                if (hr != S_OK)
                    return;

                CComPtr<IXMLDOMNamedNodeMap> attrs;
                graph->get_attributes(&attrs);

                long attrLength;
                graphs->get_length(&attrLength);

                CComBSTR bstrName = _T("id");
                CComVariant idValue;
                CComPtr<IXMLDOMNode> idNode;
                attrs->getNamedItem(bstrName, &idNode);
                if (idNode)
                {
                    idNode->get_nodeValue(&idValue);
                }

                CComBSTR bstrXML;
                graph->get_xml(&bstrXML);
                std::_tstring xml2 = CW2T(bstrXML);
                boost::algorithm::replace_all(xml2, "\n", "&#10;");
                bstrXML = CT2W(xml2.c_str());

                CComBSTR bstrLabel = _T("");
                CComBSTR bstrType = _T("");

                idValue.ChangeType(VT_BSTR);
                m_graphsMap[idValue.bstrVal] = CreateGraph(m_url, m_wuid, (const TCHAR *)bstrXML);
                m_graphsVector.push_back(m_graphsMap[idValue.bstrVal]);
                //CComBSTR name;
                //CComVariant value;
                //hr = graph->get_nodeName(&name);
                //if (hr != S_OK)
                //	return false;

                //CComPtr<IXMLDOMNode> val;
                //if (graph->get_firstChild(&val) == S_OK)
                //{
                //	hr = val->get_nodeValue(&value);
                //	value.ChangeType(VT_BSTR);
                //	std::_tstring str = CW2T(value.bstrVal, CP_UTF8);
                //}
            }

            CComPtr<IXMLDOMNodeList> texts;
            hr = pDoc->selectNodes(CComBSTR(_T("//Query/Text")), &texts);
            if (hr != S_OK)
                return;

            length = 0;
            texts->get_length(&length);
            if (length > 1)
                length = 1;
            for(long i = 0; i < length;  ++i)
            {
                CComPtr<IXMLDOMNode> text;
                hr = texts->get_item(i, &text);
                if (hr != S_OK)
                    return;
                CComBSTR ecl;
                hr = text->get_text(&ecl);
                if (hr == S_OK)
                    m_ecl = ecl;
            }
        }
        proc.unlock();
        Broadcast();
    }

    void Update(WUState state)
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        m_stateID = state;
        proc.unlock();
        Broadcast();
    }

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

static CacheT<std::_tstring, CLocalWorkunit> LocalWorkunitCache;

void ClearLocalWorkunitCache()
{
    LocalWorkunitCache.Clear();
}

void ClearLocalWorkunitCache(const std::_tstring & id)
{
    ATLTRACE(_T("Workunit cache before clearing(size=%u)\r\n"), LocalWorkunitCache.Size());
    LocalWorkunitCache.Clear(id);
}

IWorkunit * CreateLocalWorkunit(const CString & filePath, const CString & wuid)
{
    CLocalWorkunit *retVal = LocalWorkunitCache.Get(new CLocalWorkunit(filePath, wuid));
    return retVal;
}

bool UpdateLocalWorkunit(IWorkunit * wu, const std::_tstring & wuXml, const std::_tstring & results, const Dali::CEclExceptionVector & errors)
{
    CLocalWorkunit * cwu = reinterpret_cast<CLocalWorkunit * >(wu);
    cwu->Update(wuXml, results, errors);
    return true;
}

bool UpdateLocalWorkunit(IWorkunit * wu, WUState state)
{
    CLocalWorkunit * cwu = reinterpret_cast<CLocalWorkunit * >(wu);
    cwu->Update(state);
    return true;
}

IWorkunit * GetCachedLocalWorkunit(const CString & filePath, const CString & wuid)
{
    CComPtr<CLocalWorkunit> wu = new CLocalWorkunit(filePath, wuid);
    return LocalWorkunitCache.Exists(wu->GetCacheID());
}
}