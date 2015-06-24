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
typedef std::map<std::_tstring, std::_tstring> StringStringMap;
class CResult : public IResult, public clib::CLockableUnknown, public boost::noncopyable
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
	StringStringMap m_schemaMap;

public:
	IMPLEMENT_CLOCKABLEUNKNOWN;

	CResult()
	{
		ATLASSERT(false);
	}
	CResult(const CString & url, const CString & wuid, int sequence) : m_url(url), m_wuid(wuid), m_sequence(sequence)
	{
		m_id = m_url + _T("/") + m_wuid + _T("/") + boost::lexical_cast<std::_tstring>(sequence).c_str();
		m_total = 0;
	}
	virtual ~CResult()
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
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		StringStringMap::const_iterator found = m_schemaMap.find(colID);
		if (found != m_schemaMap.end()) 
		{
			return found->second.c_str();
		}
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
		StlLinked<IDali> dali;
		dali = AttachDali((const TCHAR *)m_url, _T("Dali"));
		unsigned retVal = dali->GetResultData(m_wuid, m_sequence, start, count, result, total);
		if (m_total && m_total < total)
			total = m_total;
		return retVal;
	}

#if _COMMS_VER < 68200
	void Update(const ECLResult * c)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
#if _COMMS_VER < 700000
		m_name = CW2T(c->Name, CP_UTF8);
		m_sequence = c->Sequence;
		m_value = CW2T(c->Value, CP_UTF8);
		m_link = CW2T(c->Link, CP_UTF8);
		IsSupplied = c->IsSupplied;
		m_total = c->Total;
#else
		m_name = CW2T(c->ResultName, CP_UTF8);
		m_sequence = c->SequenceNumber;
		//m_value = CW2T(c->Value, CP_UTF8);
		//m_link = CW2T(c->Link, CP_UTF8);
		m_total = c->TotalRowCount;
#endif
		m_fileName = CW2T(c->FileName, CP_UTF8);
	}
#else
	void Update(const ns6__ECLResult * c)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		SAFE_ASSIGN2CSTRING(m_name, c->Name);
		SAFE_ASSIGN(m_sequence, c->Sequence);
		SAFE_ASSIGN2CSTRING(m_value, c->Value);
		SAFE_ASSIGN2CSTRING(m_link, c->Link);
		SAFE_ASSIGN(IsSupplied, c->IsSupplied);
		SAFE_ASSIGN(m_total, c->Total);
		SAFE_ASSIGN2CSTRING(m_fileName, c->FileName);
		if (c->ECLSchemas)
		{
			m_schemaMap.clear();
			for(std::size_t i = 0; i < c->ECLSchemas->ECLSchemaItem.size(); ++i)
			{
				if (c->ECLSchemas->ECLSchemaItem[i]->ColumnName && c->ECLSchemas->ECLSchemaItem[i]->ColumnType) 
				{
					m_schemaMap[*c->ECLSchemas->ECLSchemaItem[i]->ColumnName] = *c->ECLSchemas->ECLSchemaItem[i]->ColumnType;
				}
			}
		}
	}
#endif
};

	static CacheT<std::_tstring, CResult> ResultCache;

void ClearResultCache()
{
	ATLTRACE(_T("Result cache before clearing(size=%u)\r\n"), ResultCache.Size());
	ResultCache.Clear();
}

CResult * CreateResultRaw(const CString & url, const CString & wuid, int sequence)
{
	return ResultCache.Get(new CResult(url, wuid, sequence));
}

IResult * CreateResult(const CString & url, const CString & wuid, int sequence)
{
	return CreateResultRaw(url, wuid, sequence);
}

#if _COMMS_VER < 68200
IResult * CreateResult(const CString & url, const CString & wuid, const ECLResult * result)
{
#if _COMMS_VER < 700000
	CComPtr<CResult> attr = CreateResultRaw(url, wuid, result->Sequence);
#else
	CComPtr<CResult> attr = CreateResultRaw(url, wuid, result->SequenceNumber);
#endif
	ATLASSERT(attr);
	attr->Update(result);
	return attr;
}
#else
IResult * CreateResult(const CString & url, const CString & wuid, const ns6__ECLResult * result)
{
	CComPtr<CResult> attr = CreateResultRaw(url, wuid, *result->Sequence);
	ATLASSERT(attr);
	attr->Update(result);
	return attr;
}
#endif
} //namespace Dali

