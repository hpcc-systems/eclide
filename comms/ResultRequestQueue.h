#pragma once
#include "comms.h"
#include <table.h> //clib
#include "dali.h"

__interface IResultRequestQueue : public IUnknown
{
	void Clear();
	void GetResultData(Dali::CResultDataRequest* request, bool bHighPriority=true);
};

typedef StlLinked<IResultRequestQueue> IResultRequestQueueAdapt;

COMMS_API IResultRequestQueue* CreateIResultRequestQueue(const TCHAR* url, const TCHAR* user, const TCHAR* password);
