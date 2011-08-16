#pragma once

#include "wlib.h"
#include <Module.h>

__interface IRepository;

WLIB_API bool PickModule(IRepository *rep, IModuleAdapt &mod, const TCHAR *prompt=0);
