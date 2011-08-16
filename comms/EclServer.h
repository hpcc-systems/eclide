#pragma once

#include "comms.h"
#include "clib.h"

namespace Topology
{
__interface IEclServer : public IUnknown
{
	const TCHAR *GetID() const;
	const TCHAR *GetName() const;
	const TCHAR *GetQueue() const;
};

typedef StlLinked<IEclServer> IEclServerAdapt;
typedef std::vector<IEclServerAdapt> IEclServerVector;
//typedef CUnknownT<IEclServerVector> IEclServerVectorCom;
}
