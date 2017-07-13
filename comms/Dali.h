#pragma once

#include "comms.h"
#include "workunit.h"
#include "table.h"
#include "global.h"
#include <StringSet.h> //clib

const SectionLabelDefault GLOBAL_WORKUNIT_FETCHLIMIT(SectionLabel(_T("Workunit"), _T("FetchLimit")), 999);
const SectionLabelDefault GLOBAL_WORKUNIT_PERSISTLIMIT(SectionLabel(_T("Workunit"), _T("PersistLimit")), 3);

namespace Dali
{
const TCHAR * const APP_DATA_DEBUG = _T("Debug_");
const TCHAR * const APP_DATA_APPNAME = _T("QueryBuilder");
const TCHAR * const APP_DATA_VERSION = _T("Version");
const TCHAR * const APP_DATA_QUALIFIEDLABEL = _T("QualifiedLabel");
const TCHAR * const APP_DATA_FOLLOWUP = _T("Followup_");
const TCHAR * const APP_DATA_ARCHIVE = _T("archiveToCpp");
const TCHAR * const APP_DATA_COMMONPRIVATEATTRIBUTES = _T("commonPrivateAttributes");
const TCHAR * const APP_DATA_DEBUGDEBUG = _T("Debug");

enum WUAction
{
    WUActionUnknown = 0, 
    WUActionCompile, 
    WUActionCheck, 
    WUActionRun, 
    WUActionExecuteExisting, 
    WUActionPause, 
    WUActionPauseNow, 
    WUActionResume, 
    WUActionSize,
    WUActionGenerate,
    WUActionCustom1,
    WUActionCustom2,
    WUActionCustom3,
    WUActionCustom4,
};

enum WUExceptionSeverity
{
    ExceptionSeverityInformation = 0,
    ExceptionSeverityWarning = 1,
    ExceptionSeverityError = 2,
    ExceptionSeveritySize = 3
};

typedef boost::signal<void(IWorkunitVectorAdapt workunits)> async_wucomplete_signal_type;
typedef async_wucomplete_signal_type::slot_type async_wucomplete_slot_type;

typedef boost::signal<void(__int64 start, int count, __int64 total, bool success)> async_datacomplete_signal_type;
typedef async_datacomplete_signal_type::slot_type async_datacomplete_slot_type;

typedef std::pair<std::_tstring, std::_tstring> Attribute;
typedef std::vector<Attribute> AttributeVector;

class COMMS_API CServerConfig
{
public:
    CServerConfig( const TCHAR* url=0, const TCHAR* label=0 );
    CServerConfig( const CServerConfig& other );

    CString& GetUrl(CString& url) const;
    CString& GetUserId(CString& userId) const;
    CString& GetPassword(CString& password) const;

    CString m_url;
    CString m_userId;
    CString m_password;
    CString m_label;
};

__interface IDali : public IUnknown
{
    const TCHAR *GetID() const;
    const TCHAR *GetLabel() const;
    const TCHAR *GetUrl() const;
    const CServerConfig* GetServerConfig() const;
    void GetWorkunitsAsync(const CString &cluster, const CString &owner, const CString &state, const CString &fromUTC, const CString &toUTC, int count, const async_wucomplete_slot_type &s) const;
    unsigned GetWorkunits(const CString &cluster, const CString &owner, const CString &state, const CString &fromUTC, const CString &toUTC, int count, IWorkunitVector *workunits) const;
    IWorkunit *GetWorkunitFast(const CString &wuid, bool noBroadcast = false) const;
    IWorkunit *GetWorkunit(const CString &wuid, bool noBroadcast = false) const;
    IWorkunit *GetXGMMLWorkunit(const CString &filePath) const;
    signed GetResultData(const CString & wuid, int sequence, __int64 start, int count, ITable * result, __int64 &total) const;
    void MonitorState(const CString & wuid) const;	//Notifications come thru "IWorkunit.on_refresh_connect"
    IWorkunit * Submit(const CString &cluster, const CString & queue, WUAction action, const CString & attrQualifiedLabel, const CString & ecl, const CString & path, const CString & label, int resultLimit, const CString & debugSettings, bool archive, int maxRunTime, bool debug);
    IWorkunit * Schedule(const CString &cluster, const CString & queue, const CString & attrQualifiedLabel, const CString & ecl, const CString & when, const CString & label, int resultLimit, const CString & debugSettings, bool archive);
    bool CheckComplexity(const CString &cluster, const CString & queue, const CString & ecl, const CString &module, const CString &attribute, std::_tstring & result, CEclExceptionVector & errorResults);
    void GetDependancies(const CString &cluster, const CString & queue, const CString & ecl, const CString &module, const CString &attribute, AttributeVector & results, CEclExceptionVector & errorResults);
    bool GetDependanciesTree(const std::_tstring & cluster, const std::_tstring & queue, const std::_tstring & snapshot, const std::_tstring & items, std::_tstring & xmlDepends, CEclExceptionVector & outExceptions);
    bool CheckSyntax(const CString &cluster, const CString & queue, const CString & module, const CString & attribute, const CString & path, const CString & ecl, int timeToWait, const CString & debugString, bool archive, bool noCommonPrivateAttributes, CEclExceptionVector & results);
    IGraph * GetGraph(const CString &wuid, const CString &graphName);
    unsigned GetGraphs(const CString &wuid);
    unsigned GetGraphTimings(const CString &wuid, CGraphTimingVector & results);
    unsigned GetLocalFiles(const CString &wuid, ILocalFileVector & results);
    bool AddLocalFileToWorkunit(const std::_tstring & pWuid, const std::_tstring & pName, const std::_tstring & pVal);
    bool AbortWorkunits(IWorkunitVector *workunits);
    bool ResubmitWorkunits(IWorkunitVector *workunits);
    bool DeleteWorkunits(IWorkunitVector *workunits);
    bool ProtectWorkunits(IWorkunitVector *workunits);
    bool UnprotectWorkunits(IWorkunitVector *workunits);
    bool FlagFollowupWorkunits(IWorkunitVector *workunits);
    bool ClearFollowupWorkunits(IWorkunitVector *workunits);
    bool SetState(IWorkunitVector *workunits, WUState state);
    void SetCredentials(const TCHAR* userId, const TCHAR* password);
    bool SendDebugRequest(const std::_tstring & wuid, const std::_tstring & request, std::_tstring & response);
};

class COMMS_API CResultDataRequest : public CUnknown, public boost::signals::trackable
{
public:
    CComPtr<Dali::IResult> m_result;
    __int64     m_start;
    int         m_count; 
    ITableAdapt m_resultTable; 
    bool		m_request_sent;
    Dali::async_datacomplete_slot_type m_slot;

public:
    IMPLEMENT_CUNKNOWN

    CResultDataRequest(const Dali::async_datacomplete_slot_type & slot);
    ~CResultDataRequest();
};

typedef StlLinked<CResultDataRequest> CResultDataRequestAdapt;
COMMS_API void GetResultData(const CServerConfig& config, CResultDataRequestAdapt req);

COMMS_API IDali * AttachDali();
COMMS_API IDali * AttachDali(const std::_tstring & url, const std::_tstring & label);
COMMS_API IDali * AttachDali(const _variant_t & url, const std::_tstring & label);
COMMS_API IDali * AttachDali(const TCHAR * url, const std::_tstring & label);
COMMS_API IDali * AttachDali(const CServerConfig& config);
void ClearSingletons();
}