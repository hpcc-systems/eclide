#include "stdafx.h"

#include "DaFileSrv.h"

static HMODULE hDaFileSvr = NULL;

CDaFileSrv::CDaFileSrv()
{
	if (hDaFileSvr == NULL)
	{
		hDaFileSvr = ::LoadLibrary(_T("dafilesrv.dll"));
		if (Available())
		{
			m_start = (LPFNDLLSTART)GetProcAddress(hDaFileSvr, "_Start@0");
			m_stop = (LPFNDLLSTART)GetProcAddress(hDaFileSvr, "_Stop@0");
		}
	}
}

CDaFileSrv::~CDaFileSrv()
{
}

bool CDaFileSrv::Available()
{
	return hDaFileSvr != NULL;
}

bool CDaFileSrv::Start()
{
	return m_start();
}

bool CDaFileSrv::Stop()
{
	return m_stop();
}
