// EclLib.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "EclLib.h"
BOOL APIENTRY DllMain( HANDLE  /*hModule*/, 
                       DWORD  ul_reason_for_call, 
                       LPVOID  /*lpReserved*/
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

#ifdef _DEBUG
#include "EclErrorParser.h"
class CTest {
public:
    CTest::CTest() {
        ParsedEclError test;
        ATLASSERT(ParseEclError(_T("C:\\Users\\bloodea\\AppData\\Local\\Temp\\TFRC987.tmp:10,32:error K2003 - current input doesn't match any of the available syntax paths at 's'"), test));
        ATLASSERT(ParseEclError(_T("C:\\Users\\bloodea\\AppData\\Local\\Temp\\TFRC987.tmp:15,44:error K2003 - current input doesn't match any of the available syntax paths at ':='"), test));
        ATLASSERT(ParseEclError(_T("C:\\Users\\bloodea\\AppData\\Local\\Temp\\TFRC987.tmp:16,45:error K2003 - current input doesn't match any of the available syntax paths at ':='"), test));
        ATLASSERT(ParseEclError(_T("\\machine\\temp\\test.ecl(7,13) : error C007 : Hello and Welcome"), test));
        ATLASSERT(ParseEclError(_T("c:\\temp\\test.ecl(7,13) : error C007 : Hello and Welcome"), test));
        ATLASSERT(ParseEclError(_T("\\machine\\temp\\test.ecl : 7,13 : error C007 - Hello and Welcome"), test));
        ATLASSERT(ParseEclError(_T("c:\\temp\\test.ecl : 7,13 : error C007 - Hello and Welcome"), test));
        ATLASSERT(ParseEclError(_T("(7) : error C007 stuff : Hello and Welcome"), test));
        ATLASSERT(ParseEclError(_T("error : Hello and Welcome"), test));
    }
} _eclErrorParserTest;
#endif

