#pragma once

#include "wlib.h"

//  ===========================================================================
#define GUID_npHPCCSystemsGraphViewControl _T("{2FF57548-1DE4-4F32-B133-2B3FBDDBF909}")
DEFINE_GUID(CLSID_npHPCCSystemsGraphViewControl, 0x2ff57548, 0x1de4, 0x4f32, 0xb1, 0x33, 0x2b, 0x3f, 0xbd, 0xdb, 0xf9, 0x09);
#define DLL_npHPCCSystemsGraphViewControl _T("..\\Graph Control\\npHPCCSystemsGraphViewControl.dll")
//  ===========================================================================
//bool GetDllVersionString(const std::_tstring & dllPath, std::_tstring & dllVersion);
bool WLIB_API GetCurrentVersionString(std::_tstring & dllVersion);
bool WLIB_API GetMyVersionString(std::_tstring & dllVersion);
bool WLIB_API UnregisterCurrentVersion(); 
bool WLIB_API RegisterMyVersion(); 
