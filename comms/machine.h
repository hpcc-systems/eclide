#include "StdAfx.h"
#include "topology.h"

#if _COMMS_VER < 68200
using namespace WsTopology;
#elif _COMMS_VER < 700000
#else
using namespace WsEnvironment;
#endif

namespace Topology
{
class CMachine : public IMachine, public CUnknown
{
protected:
	std::_tstring m_Name;
	std::_tstring m_Netaddress;
	std::_tstring m_Domain;
	std::_tstring m_Directory;
	std::_tstring m_Type;
	std::_tstring m_Available;
	int       m_OSType;
	std::_tstring m_OS;
	std::_tstring m_Path;
	int		  m_Port;

public:
	IMPLEMENT_CUNKNOWN;

public: //IDropZone
	const TCHAR *GetID() const
	{
		return m_Name.c_str();
	}
	const TCHAR *GetName(std::_tstring &name) const
	{
		name = m_Name;
		return name.c_str();
	}
	int GetPort() const
	{
		return m_Port;
	}

	int GetOSType() const
	{
		return m_OSType;
	}

	const TCHAR * GetOS(std::_tstring &os) const
	{
		os = m_OS;
		return os.c_str();
	}

	const TCHAR *GetNetaddress(std::_tstring &address) const
	{
		address = m_Netaddress;
		return address.c_str();
	}
	const TCHAR *GetDirectory(std::_tstring &dir) const
	{
		dir = m_Directory;
		return dir.c_str();
	}
	const TCHAR *GetAvailable(std::_tstring &avail) const
	{
		avail = m_Available;
		return avail.c_str();
	}
	const TCHAR *GetDomain(std::_tstring &domain) const
	{
		domain = m_Domain;
		return domain.c_str();
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

public:
#if _COMMS_VER < 68200
	void Update(const TpMachine * c)
	{
		if ( c )
		{
			m_Name = CW2T(c->Name, CP_UTF8);
			m_Netaddress = CW2T(c->Netaddress, CP_UTF8);
			m_Domain = CW2T(c->Domain, CP_UTF8);
			m_Directory = CW2T(c->Directory, CP_UTF8);
			m_Available = CW2T(c->Available, CP_UTF8);
			m_Type = CW2T(c->Type, CP_UTF8);
			m_Path = CW2T(c->Path, CP_UTF8);
			m_OS = c->OS;
			m_Port = c->Port;
		}
		Refresh();
	}
#elif _COMMS_VER < 700000
	void Update(const ns5__TpMachine * c)
	{
		if ( c )
		{
			SAFE_ASSIGN(m_Name, c->Name);
			SAFE_ASSIGN(m_Netaddress, c->Netaddress);
			SAFE_ASSIGN(m_Domain, c->Domain);
			SAFE_ASSIGN(m_Directory, c->Directory);
			SAFE_ASSIGN(m_Available, c->Available);
			SAFE_ASSIGN(m_Type, c->Type);
			SAFE_ASSIGN(m_Path, c->Path);
			SAFE_ASSIGN(m_OS, c->OS);
			SAFE_ASSIGN(m_Port, c->Port);
		}
		Refresh();
	}
#else
	void Update(const Machine * c)
	{
		if ( c )
		{
			m_Name = CW2T(c->Name, CP_UTF8);
			m_Netaddress = CW2T(c->NetAddress, CP_UTF8);
			m_Domain = CW2T(c->Domain, CP_UTF8);
			m_Directory = CW2T(c->Directory, CP_UTF8);
			m_Available = CW2T(c->Available, CP_UTF8);
			m_Type = CW2T(c->Type, CP_UTF8);
			//m_Path = CW2T(c->Path, CP_UTF8);
			m_OS = c->OSType;
			m_Port = c->Port;
		}
		Refresh();
	}
#endif
	void Refresh()
	{
	}
};
}