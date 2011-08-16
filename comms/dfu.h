#pragma once

#include "comms.h"
#include "global.h"

#include "dfuFile.h"

namespace Dfu
{
__interface IServer : public IUnknown
{
	const TCHAR *GetID() const;
	const TCHAR *GetLabel() const;

	unsigned int GetFiles(const std::_tstring & scope, IFileVector * results);
	unsigned int FindFiles(const std::_tstring & searchPrefix, const std::_tstring & searchCluster, const std::_tstring & searchLogicalName, const std::_tstring & searchDescription, const std::_tstring & searchOwner, IFileVector * results);
};

typedef StlLinked<IServer> IServerAdapt;
typedef std::vector<IServerAdapt> IServerVector;

COMMS_API IServer * AttachDfu();
COMMS_API IServer * AttachDfu(const TCHAR* url, const TCHAR* label);
void ClearSingletons();
}