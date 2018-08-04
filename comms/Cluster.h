#pragma once

#include "comms.h"
#include "clib.h"

namespace Topology
{
enum CLUSTERTYPE {
    CLUSTERTYPE_UNKNOWN = 0,
    CLUSTERTYPE_HTHOR,
    CLUSTERTYPE_THOR,
    CLUSTERTYPE_ROXIE
};
__interface ICluster : public IUnknown
{
    const TCHAR *GetID() const;
    const TCHAR *GetName() const;
    const TCHAR *GetQueue() const;

    CLUSTERTYPE GetType() const;
    const TCHAR *GetTypeStr() const;
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

