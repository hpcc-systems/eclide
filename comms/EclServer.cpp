#include "StdAfx.h"
#include "EclServer.h"
#include "dali.h"
#include "gSoapUtil.h"
#include "clib.h"
#include "cache.h"
#include "logger.h"

#if _COMMS_VER < 68200
using namespace WsTopology;
#elif _COMMS_VER < 70000
#else
using namespace WsEnvironment;
#endif

namespace Topology
{
class CEclServer : public IEclServer, public CUnknown
{
public:
    std::_tstring m_Url;
    std::_tstring m_Name;
    std::_tstring m_Queue;

public:
	IMPLEMENT_CUNKNOWN;

	CEclServer(const std::_tstring & url, const std::_tstring & name) : m_Url(url), m_Name(name)
	{
	}

	virtual ~CEclServer()
	{
	}

	const TCHAR *GetID() const
	{
		return m_Name.c_str();
	}
	const TCHAR * GetCacheID() const
	{
		return m_Name.c_str();
	}
	const TCHAR *GetName() const
	{
		return m_Name.c_str();
	}
	const TCHAR *GetQueue() const
	{
		return m_Queue.c_str();
	}
#if _COMMS_VER < 68200
	void Update(const TpEclServer * c)
	{
		m_Name = CW2T(c->Name, CP_UTF8);
		m_Queue = CW2T(c->Queue, CP_UTF8);
		Refresh();
	}
#elif _COMMS_VER < 68201
	void Update(const ns5__TpEclServer * c)
	{
		SAFE_ASSIGN(m_Name, c->Name);
		SAFE_ASSIGN(m_Queue, c->Queue);
		Refresh();
	}
#else
	void Update(const ns5__TpEclServer * c)
	{
		SAFE_ASSIGN(m_Name, c->Name);
		m_Queue = _T("");
		Refresh();
	}
#endif
	void Refresh()
	{
	}
};

CacheT<std::_tstring, CEclServer> EclServerCache;
CEclServer * CreateEclServerRaw(const CString & url, const CString & wuid)
{
	return EclServerCache.Get(new CEclServer(static_cast<const TCHAR *>(url), static_cast<const TCHAR *>(wuid)));
}

IEclServer * CreateEclServer(const CString & url, const CString & wuid)
{
	CEclServer * attr = CreateEclServerRaw(url, wuid);
	attr->m_Queue = _T("Local");
	return attr;
}

#if _COMMS_VER < 68200
IEclServer * CreateEclServer(const CString & url, const TpEclServer * data)
{
	CEclServer * attr = CreateEclServerRaw(url, data->Name);
	ATLASSERT(attr);
	attr->Update(data);
	return attr;
}
#elif _COMMS_VER < 70000
IEclServer * CreateEclServer(const CString & url, const ns5__TpEclServer * data)
{
	CEclServer * attr = CreateEclServerRaw(url, data->Name->c_str());
	ATLASSERT(attr);
	attr->Update(data);
	return attr;
}
#else
IEclServer * CreateEclServer(const CString & url, const	ServiceDetail * data)
{
	CEclServer * attr = CreateEclServerRaw(url, data->Name);
	ATLASSERT(attr);
	attr->Update(data);
	return attr;
}
#endif

void ClearEclServerSingletons()
{
	EclServerCache.clear();
}
}
