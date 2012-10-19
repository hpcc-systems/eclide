#include "StdAfx.h"
#include "dali.h"
#include "SoapUtil.h"
#include "clib.h"
#include "cache.h"
#include "logger.h"
#include "machine.h"
#include "DfuServer.h"

#if _COMMS_VER < 68200
using namespace WsTopology;
#elif _COMMS_VER < 700000
#else
using namespace WsEnvironment;
#endif

namespace Topology
{
class CDfuServer : public IDfuServer, public CUnknown
{
protected:
	std::_tstring m_Url;
	std::_tstring m_Name;
	std::_tstring m_Path;
	std::_tstring m_Build;
	std::_tstring m_Description;
	std::_tstring m_Type;
	std::_tstring m_Queue;
	StlLinked<CMachine> m_machine; //only 1 per server I would think

public:
	IMPLEMENT_CUNKNOWN;

	CDfuServer(const TCHAR* url, const TCHAR* name) 
		: m_Url(url), m_Name(name)
	{
	}

	virtual ~CDfuServer()
	{
	}

public: //IDfuServer
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
	const TCHAR *GetType(std::_tstring &type) const
	{
		type = m_Type;
		return type.c_str();
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
	const TCHAR *GetQueue(std::_tstring &queue) const
	{
		queue = m_Queue;
		return queue.c_str();
	}
	const IMachine *GetMachine() const
	{
		return m_machine.get();
	}

#if _COMMS_VER < 68200 
	void Update(const TpDfuServer * c)
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
#elif _COMMS_VER < 700000
	void Update(const ns5__TpDfuServer * c)
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

CacheT<std::_tstring, CDfuServer> DfuServerCache;
CDfuServer * CreateDfuServerRaw(const TCHAR *url, const TCHAR *name)
{
	return DfuServerCache.Get(new CDfuServer(url, name));
}

IDfuServer * CreateDfuServer(const TCHAR *url, const TCHAR *name)
{
	return CreateDfuServerRaw(url, name);
}

#if _COMMS_VER < 68200
IDfuServer * CreateDfuServer(const CString & url, const TpDfuServer * data)
{
	CDfuServer * server = CreateDfuServerRaw(url, CW2T(data->Name, CP_UTF8));
	ATLASSERT(server);
	server->Update(data);
	return server;
}
#elif _COMMS_VER < 700000
IDfuServer * CreateDfuServer(const CString & url, const ns5__TpDfuServer * data)
{
	CDfuServer * server = CreateDfuServerRaw(url, data->Name->c_str());
	ATLASSERT(server);
	server->Update(data);
	return server;
}
#else
IDfuServer * CreateDfuServer(const CString & url, const ServiceDetail * data)
{
	CDfuServer * server = CreateDfuServerRaw(url, CW2T(data->Name, CP_UTF8));
	ATLASSERT(server);
	server->Update(data);
	return server;
}
#endif

void ClearDfuServerSingletons()
{
	DfuServerCache.clear();
}
}