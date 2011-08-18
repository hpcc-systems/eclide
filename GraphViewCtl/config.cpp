
#include "StdAfx.h"

#ifdef SEISINT_LIBEXPORTS

#include ".\config.h"

CComPtr<IConfig> Config::m_config;

Config::Config()
{
	if (m_config == NULL)
	{
		m_config = CreateIConfig(QUERYBUILDER_CFG);
		m_config->SetLabel(_T("GraphViewControl"));
	}
}

#endif