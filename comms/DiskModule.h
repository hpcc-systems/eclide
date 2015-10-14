#pragma once

#include "Module.h"

__interface __declspec(uuid("571F940D-8EE9-4132-869C-304886284256")) IDiskModule : public IModule
{
    const TCHAR * GetPath() const;
};

typedef StlLinked<IDiskModule> IDiskModuleAdapt;
typedef std::vector<IDiskModuleAdapt> IDiskModuleVector;

