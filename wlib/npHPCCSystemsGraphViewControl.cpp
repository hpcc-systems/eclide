#include "StdAfx.h"

#include "npHPCCSystemsGraphViewControl.h"

#include <Util.h>
#include <DLLVersion.h>

bool GetDllVersionString(const std::_tstring & dllPath, std::_tstring & dllVersion)
{
	CDLLVersion fvui;
	if(fvui.Open(dllPath.c_str()))
	{
		dllVersion = (boost::_tformat(_T("%1%.%2%.%3%.%4%")) %
			fvui.GetFileVersionMajor() %  // Major version
			fvui.GetFileVersionMinor() %  // Minor version
			fvui.GetFileVersionBuild() %  // Build number
			fvui.GetFileVersionQFE()      // QFE
			).str();
		fvui.Close();
		return true;
	}
	return false;
}

bool GetCurrentVersionString(std::_tstring & dllVersion)
{
	std::_tstring dllPath;
	GetActiveXDLLPath(GUID_npHPCCSystemsGraphViewControl, dllPath);	
	return GetDllVersionString(dllPath, dllVersion); 
}

bool GetMyVersionString(std::_tstring & dllVersion)
{
	return GetDllVersionString(DLL_npHPCCSystemsGraphViewControl, dllVersion);
}

bool RegisterActiveX(const std::_tstring & dllPath, bool unregister = false)
{
	HINSTANCE hInstDLL = ::LoadLibraryEx(dllPath.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	if(hInstDLL == NULL)
		return false;

	HRESULT hRet = E_FAIL;
	if (unregister == false)
	{
		typedef HRESULT (CALLBACK* DLLREGISTERSERVER)();
		DLLREGISTERSERVER pfnDllRegisterServer = (DLLREGISTERSERVER)::GetProcAddress(hInstDLL, "DllRegisterServer");
		if(pfnDllRegisterServer == NULL)
			return false;
		hRet = (*pfnDllRegisterServer)();
	}
	else
	{
		typedef HRESULT (CALLBACK* DLLUNREGISTERSERVER)();
		DLLUNREGISTERSERVER pfnDllUnregisterServer = (DLLUNREGISTERSERVER)::GetProcAddress(hInstDLL, "DllUnregisterServer");
		if(pfnDllUnregisterServer == NULL)
			return false;
		hRet = (*pfnDllUnregisterServer)();
	}

	::FreeLibrary(hInstDLL);
	if (hRet != S_OK)
		return false;
	return true;
}
