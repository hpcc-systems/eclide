#pragma once

#include "RecursiveMutex.h"
#include "Dali.h"

namespace SMC
{
const TCHAR * const DEFAULT_VERSION = _T("build_0_0");
__interface IVersion : public IUnknown
{
	float Get();
	const TCHAR * GetString();
	bool IsCompatable();
};
}
COMMS_API SMC::IVersion * GetCommsVersion();

