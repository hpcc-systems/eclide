#pragma once

#include "comms.h"

__interface IAutoUpdate : public IUnknown
{
    bool HasUpdate(const std::_tstring & _url, const std::_tstring & _product, const std::_tstring & currentVersion);
    const TCHAR * GetResponse() const;
};

COMMS_API IAutoUpdate * CreateIAutoUpdate();
COMMS_API bool DNSLookup(std::_tstring & url);
