#pragma once

#include "Repository.h"

COMMS_API IRepository * AttachDepository(const TCHAR* url,bool bClearCache=false);