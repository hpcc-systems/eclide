#pragma once

#include "Repository.h"

COMMS_API IRepository * AttachModFileRepository(const TCHAR* filePath, bool bClearCache=false);