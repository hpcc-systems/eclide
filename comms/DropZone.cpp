#include "StdAfx.h"

#include "DropZone.h"
#include "dali.h"
#include "gSoapUtil.h"
#include "clib.h"
#include "cache.h"
#include "logger.h"
#include "machine.h"

#if _COMMS_VER < 68200
using namespace WsTopology;
#elif _COMMS_VER < 70000
#else
using namespace WsEnvironment;
#endif

namespace Topology
{
class CDropZone : public IDropZone, public CUnknown
{
protected:
	std::_tstring m_Url;
	std::_tstring m_Name;
	std::_tstring m_Path;
	std::_tstring m_Build;
	std::_tstring m_Description;
	StlLinked<CMachine> m_machine; //only 1 per drop zone I think

public:
	IMPLEMENT_CUNKNOWNQI(IDropZone);

	CDropZone(const TCHAR* url, const TCHAR* name, const TCHAR* path) 
		: m_Url(url), m_Name(name), m_Path(path)
	{
	}

	virtual ~CDropZone()
	{
	}

public: //IDropZone
	const TCHAR *GetID() const
	{
		return m_Name.c_str();
	}
	const TCHAR * GetCacheID() const
	{
		return m_Name.c_str();
	}
	const TCHAR *GetName(std::_tstring &name) const
	{
		name = m_Name;
		return name.c_str();
	}
	const TCHAR *GetDescription(std::_tstring &desc) const
	{
		desc = m_Description;
		return desc.c_str();
	}
	const TCHAR *GetPath(std::_tstring &path) const
	{
		path = m_Path;
		return path.c_str();
	}
	const TCHAR *GetBuild(std::_tstring &build) const
	{
		build = m_Build;
		return build.c_str();
	}
	IMachine *GetMachine() const
	{
		return m_machine.get();
	}

#if _COMMS_VER < 68200
	void Update(const TpDropZone * c)
	{
		m_Name = CW2T(c->Name, CP_UTF8);
		m_Description = CW2T(c->Description, CP_UTF8);
		m_Path = CW2T(c->Path, CP_UTF8);
		m_Build = CW2T( c->Build, CP_UTF8);
		m_machine = new CMachine();
#if _COMMS_VER < 60000 
		m_machine->Update(c->TpMachines);
#else
		m_machine->Update(c->TpMachines._TpMachine);
#endif
		Refresh();
	}
#elif _COMMS_VER < 70000
	void Update(const ns5__TpDropZone * c)
	{
		SAFE_ASSIGN(m_Name, c->Name);
		SAFE_ASSIGN(m_Description, c->Description);
		SAFE_ASSIGN(m_Path, c->Path);
		SAFE_ASSIGN(m_Build, c->Build);
		m_machine = new CMachine();
		m_machine->Update(c->TpMachines->TpMachine[0]);
		Refresh();
	}
#else
	void Update(const ServiceDetail * c)
	{
		m_Name = CW2T(c->Name, CP_UTF8);
		m_Description = CW2T(c->Description, CP_UTF8);
		//m_Path = CW2T(c->Path, CP_UTF8);
		m_Build = CW2T( c->Build, CP_UTF8);
		m_machine = new CMachine();
		m_machine->Update(c->Machines._Machine);
		Refresh();
	}
#endif

	void Refresh()
	{
	}
};

CacheT<std::_tstring, CDropZone> DropZoneCache;
CDropZone * CreateDropZoneRaw(const TCHAR *url, const TCHAR *name, const TCHAR *path)
{
	return DropZoneCache.Get(new CDropZone(url, name, path));
}

IDropZone * CreateDropZone(const TCHAR *url, const TCHAR *name, const TCHAR *path)
{
	return CreateDropZoneRaw(url, name, path);
}

#if _COMMS_VER < 68200
IDropZone * CreateDropZone(const CString & url, const TpDropZone * data)
{
	CDropZone * zone = CreateDropZoneRaw(url, CW2T(data->Name, CP_UTF8), CW2T(data->Path, CP_UTF8));
	ATLASSERT(zone);
	zone->Update(data);
	return zone;
}
#elif _COMMS_VER < 70000
IDropZone * CreateDropZone(const CString & url, const ns5__TpDropZone * data)
{
	CDropZone * zone = CreateDropZoneRaw(url, data->Name->c_str(), data->Path->c_str());
	ATLASSERT(zone);
	zone->Update(data);
	return zone;
}
#else
IDropZone * CreateDropZone(const CString & url, const ServiceDetail * data)
{
	CDropZone * zone = CreateDropZoneRaw(url, CW2T(data->Name, CP_UTF8), CW2T(_T(""), CP_UTF8));
	ATLASSERT(zone);
	zone->Update(data);
	return zone;
}
#endif
void ClearDropZoneSingletons()
{
	DropZoneCache.clear();
}
}