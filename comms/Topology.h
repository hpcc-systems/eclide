#pragma once

#include "comms.h"

#include "global.h"
#include "cluster.h"
#include "group.h"
#include "dropZone.h"
#include "dfuServer.h"
#include "eclServer.h"

namespace Topology
{
__interface ITopology : public IUnknown
{
	const TCHAR *GetID() const;
	const TCHAR *GetLabel() const;
	const TCHAR *GetUrl() const;
	unsigned GetClusters(const _variant_t & queueFilter, IClusterVector & clusters) const;
	unsigned GetGroups(IGroupVector & groups) const;
	unsigned GetClustersX(const TCHAR *type, IClusterVector & clusters) const;
	unsigned GetDropZones(IDropZoneVector & dropZones) const;
	IDropZone * GetDropZone(const std::_tstring & ip) const;
	unsigned GetDfuServers(IDfuServerVector & servers) const;
	unsigned GetEclServers(IEclServerVector & eclservers) const;
	bool HasEclServers() const;
};

__interface IMachine : public IUnknown
{
	const TCHAR *GetID() const;
	const TCHAR *GetName(std::_tstring &name) const;
	const TCHAR *GetDirectory(std::_tstring &dir) const;
	const TCHAR *GetAvailable(std::_tstring &avail) const;
	const TCHAR *GetNetaddress(std::_tstring &netaddress) const;
	const TCHAR *GetDomain(std::_tstring &domain) const;
	const TCHAR *GetType(std::_tstring &type) const;
	const TCHAR *GetPath(std::_tstring &path) const;
	int GetPort() const;
#if _COMMS_VER < 700000 
	int GetOSType() const;
#else
	const TCHAR *GetOS(std::_tstring &os) const;
#endif
};
typedef StlLinked<IMachine> IMachineAdapt;

void ClearSingletons();
}
COMMS_API Topology::ITopology * AttachTopology(const std::_tstring & url, const std::_tstring & label);
COMMS_API Topology::ITopology * AttachTopology(const _variant_t & url, const std::_tstring & label);
