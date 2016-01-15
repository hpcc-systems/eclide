#pragma once

#include "comms.h"
#include "clib.h"

namespace Topology
{
__interface IGroup : public IUnknown
{
    const TCHAR *GetID() const;
    const TCHAR *GetName() const;
};

typedef StlLinked<IGroup> IGroupAdapt;
typedef std::vector<IGroupAdapt> IGroupVector;
}

