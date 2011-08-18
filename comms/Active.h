#pragma once

#include "clib.h"

__interface IActive : public IUnknown
{
	const TCHAR * GetID();
};

typedef StlLinked<IActive> IActiveAdapt;
typedef std::vector<IActiveAdapt> IActiveVector;

