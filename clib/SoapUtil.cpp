#include "stdafx.h"

#include "SoapUtil.h"

const TCHAR * const SOAPCLIENT_ERROR_TEXT[] =
{
	_T("SUCCESS"),
	_T("INITIALIZE_ERROR"),
	_T("OUTOFMEMORY"),
	_T("GENERATE_ERROR"),
	_T("CONNECT_ERROR"),
	_T("SEND_ERROR"),
	_T("SERVER_ERROR"),
	_T("SOAPFAULT"),
	_T("PARSEFAULT_ERROR"),
	_T("READ_ERROR"),
	_T("PARSE_ERROR")
};

CLIB_API const TCHAR * GetSoapError(int code)
{
	ATLASSERT(code >= 0 && code < 10);
	return SOAPCLIENT_ERROR_TEXT[code];
}

CLIB_API BSTR * CreateBstrArray(const StdStringVector &strArr)
{
	unsigned arrSize = strArr.size();
	if (arrSize == 0)
		return NULL;

	BSTR *arr = (BSTR *)malloc(arrSize * sizeof(BSTR));
	for (unsigned i = 0; i < arrSize; ++i)
	{
		CString c(strArr[i].c_str());
		arr[i] = c.AllocSysString();
	}
	return arr;
}

CLIB_API void DestroyBstrArray(BSTR * arr, unsigned arrSize)
{
	if (arrSize == 0)
		return;

	for (unsigned i = 0; i < arrSize; ++i)
		SysFreeString(arr[i]);
	free(arr);
}
