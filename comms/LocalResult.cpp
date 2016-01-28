#include "StdAfx.h"
#include "result.h"

#include "workunit.h"
#include "dali.h"
#include "SoapUtil.h"
#include "cache.h"

#if _COMMS_VER < 68200
using namespace WsWorkunits;
#elif _COMMS_VER < 700000
#else
using namespace WsECLWorkunits;
typedef ECLWUResult ECLResult;
#endif

namespace Dali
{

class CLocalResult : public IResult, public clib::CLockableUnknown, public boost::noncopyable
{
protected:
    CString m_id;
    CString m_url;
    CString m_wuid;
    CString m_name;
    int m_sequence;
    CString m_value;
    CString m_link;
    CString m_fileName;
    bool IsSupplied;
    __int64 m_total;
    StlLinked<ITable> m_result;

public:
    IMPLEMENT_CLOCKABLEUNKNOWN;

    CLocalResult()
    {
        ATLASSERT(false);
    }
    CLocalResult(const CString & url, const CString & wuid, int sequence) : m_url(url), m_wuid(wuid), m_sequence(sequence)
    {
        m_id = m_url + _T("/") + m_wuid + _T("/") + boost::lexical_cast<std::_tstring>(sequence).c_str();
        m_total = 0;
        m_result = CreateISparseTable();
        //m_result->AddColumn(_T("Row"));
    }
    virtual ~CLocalResult()
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
    const TCHAR *GetECLType(const std::_tstring & colID) const
    {
        return NULL;
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
        if (m_result->GetRowCount() - start < count)
            count = m_result->GetRowCount() - start;
        for(__int64 i = start; i < start + count; ++i)
        {
            for (unsigned int j = 0; j < m_result->GetColumnCount(); ++j)
            {
                int newCol = result->FindColumn(m_result->GetColumn(j), true, m_result->GetColumnType(j));
                if (newCol >= 0)
                    result->SetCell(i, newCol, m_result->GetCell(i, j));
                else
                    result->SetCell(i, j, m_result->GetCell(i, j));
            }
        }
        return count;
    }

    void Update(const std::_tstring & name, ITable * result)
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);

        m_name = name.c_str();

        m_result->LoadFrom(result);
        m_total = result->GetRowCount();
    }
};

static CacheT<std::_tstring, CLocalResult> LocalResultCache;

void ClearLocalResultCache()
{
    ATLTRACE(_T("Result cache before clearing(size=%u)\r\n"), LocalResultCache.Size());
    LocalResultCache.Clear();
}

CLocalResult * CreateLocalResultRaw(const CString & url, const CString & wuid, int sequence)
{
    return LocalResultCache.Get(new CLocalResult(url, wuid, sequence));
}

IResult * CreateLocalResult(const CString & url, const CString & wuid, int sequence)
{
    return CreateLocalResultRaw(url, wuid, sequence);
}

#if _COMMS_VER < 68200
#else
IResult * CreateLocalResult(const CString & url, const CString & wuid, const std::_tstring & label, int sequence, ITable * result)
{
    CComPtr<CLocalResult> attr = CreateLocalResultRaw(url, wuid, sequence);
    ATLASSERT(attr);
    attr->Update(label, result);
    return attr;
}
#endif
} //namespace Dali

