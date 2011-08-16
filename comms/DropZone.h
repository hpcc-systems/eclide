#pragma once

#include "comms.h"
#include "clib.h"

namespace Topology
{
enum OS
{
	OS_UNKNOWN = -1,
	OS_WIN = 0,
	OS_LINUX = 1,
	OS_WIN2 = 2,
	OS_LINUX2 = 3
};

__interface IMachine;
__interface __declspec(uuid("78da599d-2bba-438c-8f32-014179c3aa2e")) IDropZone : public IUnknown
{
	const TCHAR *GetID() const;
	const TCHAR *GetName(std::_tstring &name) const;
	const TCHAR *GetDescription(std::_tstring &desc) const;
	const TCHAR *GetPath(std::_tstring &path) const;
	const TCHAR *GetBuild(std::_tstring &build) const;
	IMachine *GetMachine() const;
};

typedef StlLinked<IDropZone> IDropZoneAdapt;
typedef std::vector<IDropZoneAdapt> IDropZoneVector;
typedef CUnknownT<IDropZoneVector> IDropZoneVectorCom;
}

