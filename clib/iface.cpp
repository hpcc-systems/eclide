#include "stdafx.h"
#include "clib.h"
#include "iface.h"

CInterface::~CInterface()
{
}

void CInterface::beforeDispose()
{
}

bool CInterface::Release(void)
{
#ifdef _DEBUG
	assert(xxcount != 0xdddddddd);        // microsoft freed memory value
#endif
	if (atomic_dec(&xxcount) < 0)
	{
		xxcount = 0x3fffffff; // yuk, to avoid introducing another member+condition, use xxcount to prevent Link/Release'ing of 'this' during
		// beforeDispose causing this block to become circular. High mid-point positive number to avoid poss. of xxcount<0 !
		beforeDispose();
		delete this;
		return true;
	}
	return false;
}

