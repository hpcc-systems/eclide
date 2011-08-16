#pragma once

#include "clib.h"

typedef bool (__stdcall * LPFNDLLSTART)();
typedef bool (__stdcall * LPFNDLLSTOP)();

class CLIB_API CDaFileSrv : CUnknown
{
protected:
	LPFNDLLSTART m_start;
	LPFNDLLSTOP m_stop;

public:
	BEGIN_CUNKNOWN;
	END_CUNKNOWN(CUnkown);

	CDaFileSrv();
	~CDaFileSrv();

	bool Available();
	bool Start();
	bool Stop();
};