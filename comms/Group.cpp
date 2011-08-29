#include "StdAfx.h"
#include "Group.h"
#include "dali.h"
#include "gSoapUtil.h"
#include "clib.h"
#include "cache.h"
#include "logger.h"

#if _COMMS_VER < 68200
using namespace WsTopology;
#endif

namespace Topology
{
class CGroup : public IGroup, public CUnknown
{
protected:
    std::_tstring m_Url;
    std::_tstring m_Name;

public:
	IMPLEMENT_CUNKNOWN;

	CGroup(const std::_tstring & url, const std::_tstring & name) : m_Url(url), m_Name(name)
	{
	}

	virtual ~CGroup()
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

	void Update(const std::_tstring & name)
	{
		m_Name = name;
		Refresh();
	}

	void Refresh()
	{
	}
};

CacheT<std::_tstring, CGroup> GroupCache;
CGroup * CreateGroupRaw(const CString & url, const CString & wuid)
{
	return GroupCache.Get(new CGroup(static_cast<const TCHAR *>(url), static_cast<const TCHAR *>(wuid)));
}

IGroup * CreateGroup(const CString & url, const CString & wuid)
{
	return CreateGroupRaw(url, wuid);
}

IGroup * CreateGroup(const CString & url, const std::_tstring & data)
{
	CGroup * attr = CreateGroupRaw(url, data.c_str());
	ATLASSERT(attr);
	attr->Update(data);
	return attr;
}
void ClearGroupSingletons()
{
	GroupCache.clear();
}
}
