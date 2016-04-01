#ifdef SEISINT_LIBEXPORTS

#pragma once

#include "global.h"

class Config
{
protected:
    static CComPtr<IConfig> m_config;
public:
    Config();
};

#endif