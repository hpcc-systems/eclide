#include "StdAfx.h"
#include "Dfu.h"

#include "clib.h"
#include "gSoapUtil.h"
#include "cache.h"
#include "logger.h"
#include "global.h"
#include "util.h"
#include "utilDateTime.h"
#include "thread.h"


namespace Dfu
{
#if _COMMS_VER >= 68200
#define ESP_EXCEPTION_LOG3(T) CReportingGSoapEspException<ns3__ArrayOfEspException> exceptions(T, __FILE__, __LINE__)
//  ===========================================================================
IFile * CreateIFile(IServer * server, const std::_tstring & scope, ns3__DFULogicalFile * info);
void ClearDfuFileCache();
//  ===========================================================================

class CFileStructCompare
{
public:
    bool operator ()(IFileAdapt & l, IFileAdapt & r)
    {
        ATLASSERT(l.get() && r.get());
        if (l->IsDir() != r->IsDir())
        {
            return l->IsDir() > r->IsDir();
        }
        CString lhs = l->GetLeaf();
        return lhs.CompareNoCase(r->GetLeaf()) > 0 ? false : true;
    }
};

//  ===========================================================================

class CDfu : public IServer, public CUnknown
{
protected:
    CString m_label;
    CString m_url;

public:
    IMPLEMENT_CUNKNOWN;

    CDfu(const CString & url, const CString & label) : m_label(label), m_url(url)
    {
    }

    ~CDfu()
    {
    }

    const TCHAR * GetID() const
    {
        return m_url;
    }

    const TCHAR * GetCacheID() const
    {
        return m_url;
    }

    const TCHAR * GetLabel() const
    {
        return m_label;
    }

    const TCHAR * GetUrl() const
    {
        return m_url;
    }

    unsigned int GetFiles(const std::_tstring & _scope, IFileVector * results)
    {
        CSoapInitialize<WsDfuServiceSoapProxy> server(m_url);

        _ns3__DFUFileViewRequest request;
        CStringAssign Scope(request.Scope, _scope);

        _ns3__DFUFileViewResponse response;
        ESP_EXCEPTION_LOG3(response.Exceptions);
        if (server.DFUFileView(&request, &response) == SOAP_OK)
        {
            if (response.DFULogicalFiles)
            {
                int count = response.DFULogicalFiles->DFULogicalFile.size();
                for(int i = 0; i < count; ++i)
                {
                    StlLinked<IFile> file = CreateIFile(this, _scope, response.DFULogicalFiles->DFULogicalFile[i]);
                    results->push_back(file);
                }
            }
        }
        else
        {
            _DBGLOG(m_url, LEVEL_WARNING, server.GetClientErrorMsg());
        }
        return results->size();
    }

    unsigned int FindFiles(const std::_tstring & searchPrefix, const std::_tstring & searchCluster, const std::_tstring & searchLogicalName, const std::_tstring & searchDescription, const std::_tstring & searchOwner, IFileVector * results)
    {
        CSoapInitialize<WsDfuServiceSoapProxy> server(m_url);

        _ns3__DFUQueryRequest request;
        CStringAssign Prefix(request.Prefix, searchPrefix); 
#if _COMMS_VER < 604000
        CStringAssign ClusterName(request.ClusterName, searchCluster); 
#else
        CStringAssign ClusterName(request.NodeGroup, searchCluster);
#endif
        CStringAssign LogicalName(request.LogicalName, searchLogicalName); 
        CStringAssign Description(request.Description, searchDescription); 
        CStringAssign Owner(request.Owner, searchOwner); 
        request.FileSizeFrom = 0; 
        request.FileSizeTo = 0; 
        request.FirstN = 99; 
        request.PageSize = 0; 
        request.PageStartFrom = 0; 
        request.Descending = false; 
        request.OneLevelDirFileReturn = false; 

        _ns3__DFUQueryResponse response;
        ESP_EXCEPTION_LOG3(response.Exceptions);
        if (server.DFUQuery(&request, &response) == SOAP_OK)
        {
            if (response.DFULogicalFiles)
            {
                int count = response.DFULogicalFiles->DFULogicalFile.size();
                for(int i = 0; i < count; ++i)
                {
                    StlLinked<IFile> file = CreateIFile(this, _T(""), response.DFULogicalFiles->DFULogicalFile[i]);
                    results->push_back(file);
                }
            }
        }
        else
        {
            _DBGLOG(m_url, LEVEL_WARNING, server.GetClientErrorMsg());
        }
        //std::sort(results->begin(), results->end(), CFileStructCompare());
        return results->size();
    }
};

static CacheT<std::_tstring, CDfu> DfuCache;

COMMS_API IServer * AttachDfu()
{
    CString url(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_DFU)); 
    return DfuCache.Get(new CDfu(url, _T("Dfu")));
}

IServer * AttachDfu(const TCHAR* url, const TCHAR* label)
{
    return DfuCache.Get(new CDfu(url, label));
}

void ClearSingletons()
{
    ClearDfuFileCache();
    DfuCache.clear();
}
#else
#if _COMMS_VER < 58800
COMMS_API IServer * AttachDfu()
{
    return NULL;
}

IServer * AttachDfu(const TCHAR* url, const TCHAR* label)
{
    return NULL;
}
#else
using namespace WsDfu;
//  ===========================================================================

//IFile * CreateIFile(IServer * server, PhysicalFileStruct * info);
IFile * CreateIFile(IServer * server, const std::_tstring & scope, DFULogicalFile * info);
void ClearDfuFileCache();
//  ===========================================================================

class CFileStructCompare
{
public:
    bool operator ()(IFileAdapt & l, IFileAdapt & r)
    {
        ATLASSERT(l.get() && r.get());
        if (l->IsDir() != r->IsDir())
        {
            return l->IsDir() > r->IsDir();
        }
        CString lhs = l->GetLeaf();
        return lhs.CompareNoCase(r->GetLeaf()) > 0 ? false : true;
    }
};

//  ===========================================================================

class CDfu : public IServer, public CUnknown
{
protected:
    CString m_label;
    CString m_url;

public:
    IMPLEMENT_CUNKNOWN;

    CDfu(const CString & url, const CString & label) : m_label(label), m_url(url)
    {
    }

    ~CDfu()
    {
    }

    const TCHAR * GetID() const
    {
        return m_url;
    }

    const TCHAR * GetCacheID() const
    {
        return m_url;
    }

    const TCHAR * GetLabel() const
    {
        return m_label;
    }

    const TCHAR * GetUrl() const
    {
        return m_url;
    }

    unsigned int GetFiles(const std::_tstring & _scope, IFileVector * results)
    {
        CComInitialize com;
        CWsDfuT<CCustomActionSecureSoapSocketClient> server;
        server.SetUrl(m_url);

        CComBSTR scope = CT2W(_scope.c_str(), CP_UTF8);
        int NumFiles = 0;
        CStructArrayOut<DFULogicalFile> response;
#if _COMMS_VER < 67205
        ESP_EXCEPTION_LOG(EspException);
        if (server.DFUFileView(&scope, exceptions.GetArrayAddress(), exceptions.GetCountAddress(), &NumFiles, response.GetArrayAddress(), response.GetCountAddress()) == S_OK)
#else
        ESP_EXCEPTION_LOG2(EspException);
        if (server.DFUFileView(&scope, exceptions, &NumFiles, response.GetArrayAddress(), response.GetCountAddress()) == S_OK)
#endif
        {
            int count = response.GetCount();
            for(int i = 0; i < count; ++i)
            {
                StlLinked<IFile> file = CreateIFile(this, _scope, response.GetItem(i));
                results->push_back(file);
            }
        }
        else
        {
            _DBGLOG(m_url, LEVEL_WARNING, server.GetClientError());
            ////  Temp for testing
            //for(int i = 0; i < 10; ++i)
            //{
            //	PhysicalFileStruct info;
            //	std::_tstring name = _scope + _T("::testing") + boost::lexical_cast<std::_tstring>(i);
            //	info.name = ::SysAllocString(CT2W(name.c_str(), CP_UTF8));
            //	info.isDir = i % 3;
            //	info.filesize = 100000;
            //	info.modifiedtime = ::SysAllocString(L"");
            //	StlLinked<IFile> file = CreateIFile(this, &info);
            //	results->push_back(file);
            //	::SysFreeString(info.name);
            //	::SysFreeString(info.modifiedtime);
            //}
        }
        //std::sort(results->begin(), results->end(), CFileStructCompare());
        return results->size();
    }

    unsigned int FindFiles(const std::_tstring & searchPrefix, const std::_tstring & searchCluster, const std::_tstring & searchLogicalName, const std::_tstring & searchDescription, const std::_tstring & searchOwner, IFileVector * results)
    {
        CComInitialize com;
        CWsDfuT<CCustomActionSecureSoapSocketClient> server;
        server.SetUrl(m_url);

        CComBSTR Prefix = CT2W(searchPrefix.c_str(), CP_UTF8); 
        CComBSTR ClusterName = CT2W(searchCluster.c_str(), CP_UTF8); 
        CComBSTR LogicalName = CT2W(searchLogicalName.c_str(), CP_UTF8); 
        CComBSTR Description = CT2W(searchDescription.c_str(), CP_UTF8); 
        CComBSTR Owner = CT2W(searchOwner.c_str(), CP_UTF8); 
        CComBSTR StartDate = CT2W(_T(""), CP_UTF8); 
        CComBSTR EndDate = CT2W(_T(""), CP_UTF8); 
        CComBSTR FileType = CT2W(_T(""), CP_UTF8); 

#if _COMMS_VER < 58600
        int FileSizeFrom = 0; 
        int FileSizeTo = 0; 
#else
        __int64 FileSizeFrom = 0; 
        __int64 FileSizeTo = 0; 
#endif
        int FirstN = 99; 
        CComBSTR FirstNType = CT2W(_T(""), CP_UTF8); 
        int PageSize = 0; 
        int PageStartFrom = 0; 
        CComBSTR Sortby = CT2W(_T(""), CP_UTF8); 
        bool Descending = false; 
        bool OneLevelDirFileReturn = false; 
        int LastPageFrom = 0; 
        int PageEndAt = 0; 
        int PrevPageFrom = 0; 
        int NextPageFrom = 0; 
        int NumFiles = 0; 
        CComBSTR BasicQuery = CT2W(_T(""), CP_UTF8); 
        CComBSTR ParametersForPaging = CT2W(_T(""), CP_UTF8); 
        CComBSTR Filters = CT2W(_T(""), CP_UTF8);

        CStructArrayOut<DFULogicalFile> response;

#if _COMMS_VER < 67205
        ESP_EXCEPTION_LOG(EspException);
        if (server.DFUQuery(
            &Prefix, 
            &ClusterName, 
            &LogicalName, 
            &Description, 
            &Owner, 
            &StartDate, 
            &EndDate, 
            &FileType, 
            &FileSizeFrom, 
            &FileSizeTo, 
            &FirstN, 
            &FirstNType, 
            &PageSize, 
            &PageStartFrom, 
            &Sortby, 
            &Descending, 
            OneLevelDirFileReturn, 
            exceptions.GetArrayAddress(), 
            exceptions.GetCountAddress(), 
            response.GetArrayAddress(), 
            response.GetCountAddress(),
            &LastPageFrom, 
            &PageEndAt, 
            &PrevPageFrom, 
            &NextPageFrom, 
            &NumFiles, 
            &BasicQuery, 
            &ParametersForPaging, 
            &Filters) == S_OK)
        {
#else
        ESP_EXCEPTION_LOG2(EspException);
        if (server.DFUQuery(
            &Prefix, 
            &ClusterName, 
            &LogicalName, 
            &Description, 
            &Owner, 
            &StartDate, 
            &EndDate, 
            &FileType, 
            &FileSizeFrom, 
            &FileSizeTo, 
            &FirstN, 
            &FirstNType, 
            &PageSize, 
            &PageStartFrom, 
            &Sortby, 
            &Descending, 
            OneLevelDirFileReturn, 
            exceptions, 
            response.GetArrayAddress(), 
            response.GetCountAddress(),
            &LastPageFrom, 
            &PageEndAt, 
            &PrevPageFrom, 
            &NextPageFrom, 
            &NumFiles, 
            &BasicQuery, 
            &ParametersForPaging, 
            &Filters) == S_OK)
        {
#endif
            int count = response.GetCount();
            for(int i = 0; i < count; ++i)
            {
                StlLinked<IFile> file = CreateIFile(this, _T(""), response.GetItem(i));
                results->push_back(file);
            }
        }
        else
        {
            _DBGLOG(m_url, LEVEL_WARNING, server.GetClientError());
        }
        //std::sort(results->begin(), results->end(), CFileStructCompare());
        return results->size();
    }
};

static CacheT<std::_tstring, CDfu> DfuCache;

COMMS_API IServer * AttachDfu()
{
    CString url(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_DFU)); 
    return DfuCache.Get(new CDfu(url, _T("Dfu")));
}

IServer * AttachDfu(const TCHAR* url, const TCHAR* label)
{
    return DfuCache.Get(new CDfu(url, label));
}

void ClearSingletons()
{
    ClearDfuFileCache();
    DfuCache.clear();
}
#endif
#endif
}
