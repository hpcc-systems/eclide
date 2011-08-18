#pragma once

#include "RecursiveMutex.h"
#include "Dali.h"

namespace SMC
{
	__interface ICluster : public clib::ILockableUnknown
{
	const TCHAR * GetID();
	const TCHAR * GetLabel();
	Dali::IWorkunit *GetRunning();
	int GetQueueCount();
	Dali::IWorkunit *GetQueueItem(unsigned item);
};

typedef StlLinked<ICluster> IClusterAdapt;
typedef std::vector<IClusterAdapt> IClusterVector;
typedef CUnknownT<IClusterVector> IClusterVectorCom;
}

