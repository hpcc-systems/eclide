#include "StdAfx.h"

#include "global.h"
#include "Logger.h"

void TraceSoapRequest(const char *szURL, const char* szRequest)
{
	HANDLE hStdOut = ::GetStdHandle(STD_OUTPUT_HANDLE);
	if (hStdOut && (hStdOut != INVALID_HANDLE_VALUE))
	{
		if ( szURL )
		{
			DWORD dwWritten = ::strlen(szURL);
			::WriteFile(hStdOut, szURL, dwWritten, &dwWritten, 0);
		}

		//start at soap:body
		const char* start = ::strstr(szRequest, "<soap:Body>");
		if ( start )
			szRequest = start;
		DWORD dwWritten = ::strlen(szRequest);

		static const char EndEnvelope[] = "</soap:Envelope>";
		static const int LenEndEnvelope = ::strlen(EndEnvelope);
		if ( start && ::strstr(szRequest, EndEnvelope) )
			dwWritten -= LenEndEnvelope; 

		::WriteFile(hStdOut, szRequest, dwWritten, &dwWritten, 0);
	}
}

void TraceSoapResultCode(int code)
{
	HANDLE hStdOut = ::GetStdHandle(STD_OUTPUT_HANDLE);
	if (hStdOut && hStdOut != INVALID_HANDLE_VALUE)
	{
		CStringA strCode;
		strCode.Format(" %i\r\n", code);
		DWORD dwWritten = strCode.GetLength();
		::WriteFile(hStdOut, (const char*)strCode, dwWritten, &dwWritten, 0);
	}
}

void TraceSoapResult(const TCHAR* szResult)
{
	HANDLE hStdOut = ::GetStdHandle(STD_OUTPUT_HANDLE);
	if (hStdOut && hStdOut != INVALID_HANDLE_VALUE)
	{
		LPSTR aResult = CT2A(szResult,CP_UTF8);
		DWORD dwWritten = ::_tcslen(szResult);
		::WriteFile(hStdOut, aResult, dwWritten, &dwWritten, 0);
	}
}

