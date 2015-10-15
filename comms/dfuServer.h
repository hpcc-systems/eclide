#pragma once

#include "comms.h"
#include "clib.h"

namespace Topology
{
    __interface IMachine;

    __interface IDfuServer : public IUnknown
    {
        const TCHAR *GetID() const;
        const TCHAR *GetName(std::_tstring &name) const;
        const TCHAR *GetDescription(std::_tstring &desc) const;
        const TCHAR *GetBuild(std::_tstring &build) const;
        const TCHAR *GetPath(std::_tstring &path) const;
        const TCHAR *GetQueue(std::_tstring &queue) const;
        const TCHAR *GetType(std::_tstring &type) const;
        const IMachine *GetMachine() const;
    };

    typedef StlLinked<IDfuServer> IDfuServerAdapt;
    typedef std::vector<IDfuServerAdapt> IDfuServerVector;
}

