#pragma once

#include "RecursiveMutex.h"
#include "Dali.h"

namespace SMC
{
__interface IVersion : public IUnknown
{
	float Get();
	const TCHAR * GetString();
	bool IsCompatable();
};
}
COMMS_API SMC::IVersion * GetCommsVersion();

