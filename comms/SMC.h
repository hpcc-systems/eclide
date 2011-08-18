#pragma once

#include "comms.h"

#include "cluster.h"
#include "table.h"
#include "global.h"
#include "SMCCluster.h"
#include "SMCVersion.h"

//  ===========================================================================
enum PRIORITY
{
	PRIORITY_UP,
	PRIORITY_DOWN,
	PRIORITY_TOP,
	PRIORITY_BOTTOM
};
//  ===========================================================================

namespace SMC
{
__interface ISMC : public IUnknown
{
	const TCHAR *GetID() const;
	const TCHAR *GetLabel() const;
	const TCHAR *GetUrl() const;
	bool GetActive(IClusterVector * clusters);
	IVersion * GetBuild();
	bool SetPriority(const std::_tstring &cluster, const std::_tstring &wuid, PRIORITY priority);
	bool SetPriority(Dali::IWorkunitVector *workunits, PRIORITY priority);
};

COMMS_API ISMC * AttachSMC(const std::_tstring & url, const std::_tstring & label);
COMMS_API ISMC * AttachSMC(const _variant_t & url, const std::_tstring & label);
void ClearSingletons();
}

