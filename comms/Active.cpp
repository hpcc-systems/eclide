#include "StdAfx.h"
#include "active.h"

#include "smc.h"
#include "SoapUtil.h"
#include "Cache.h"

using namespace WsSMC;

class CActive : public IActive, public CUnknown
{
protected:
	CString m_id;
	CString m_q;
	CString m_wuid;

public:
	IMPLEMENT_CUNKNOWN;

	CActive(const CString &q, const CString &wuid) : m_wuid(wuid), m_q(q)
	{
		m_id = m_q + _T("\\") + m_wuid;
	}

	const TCHAR * GetID()
	{
		return m_id;
	}
};

CActive * CreateActiveRaw(const CString & url, const CString & q, const CString & wuid)
{
	static CacheT<CActive> ActiveCache;
	return ActiveCache.Get(new CActive(url, wuid));
}

IActive * CreateActive(const CString & url, const CString & q, const CString & wuid)
{
	return CreateActiveRaw(url, q, wuid);
}

IActive * CreateActive(const CString & url, const CString &cluster, const ActiveWorkunit *wu)
{
	CActive * retVal = CreateActiveRaw(url, cluster, wu->Wuid);
	ATLASSERT(retVal);
	return retVal;
}
