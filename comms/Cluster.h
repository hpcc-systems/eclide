#pragma once

#include "comms.h"
#include "clib.h"

namespace Topology
{
__interface ICluster : public IUnknown
{
	const TCHAR *GetID() const;
	const TCHAR *GetName() const;
	const TCHAR *GetQueue() const;

	const TCHAR *GetType() const;
	const TCHAR *GetDirectory() const;
	const TCHAR *GetDesc() const;
	const TCHAR *GetPrefix() const;
	const TCHAR *GetPath() const;
	const TCHAR *GetDataModel() const;
};

typedef StlLinked<ICluster> IClusterAdapt;
typedef std::vector<IClusterAdapt> IClusterVector;
//typedef CUnknownT<IClusterVector> IClusterVectorCom;
}

