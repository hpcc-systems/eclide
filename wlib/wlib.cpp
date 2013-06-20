#include "stdafx.h"
#include "wlib.h"
#include "file_ver.h"
#include "../config.h"
#include <UtilFilesystem.h>

const TCHAR *GetRegPathQB(const std::_tstring & overideVersion)
{
	static std::_tstring regPath;
	boost::filesystem::path p = stringToPath(static_cast<const TCHAR *>(GetApplicationName()));
	std::_tstring app = CA2T(boost::filesystem::basename(p).c_str());
	std::_tstring ver = overideVersion.length() ? overideVersion : static_cast<const TCHAR *>(GetApplicationVersion());
	regPath = (boost::_tformat(_T("Software\\HPCC Systems\\%1%\\Ver %2%")) % app % ver).str();
	return regPath.c_str();
}

const TCHAR *GetAppVersion(std::_tstring & version)
{
	version = static_cast<const TCHAR *>(GetApplicationVersion());
	return version.c_str();
}

const TCHAR *GetAppMajorVersion(std::_tstring & version)
{
	version = static_cast<const TCHAR *>(GetApplicationMajorVersion());
	return version.c_str();
}

const TCHAR *GetBuildVersion(std::_tstring & version)
{
	version = CA2T(BUILD_TAG);
	return version.c_str();
}

#ifndef SEISINT_LIBEXPORTS
class CATLDllModule : public CAtlDllModuleT< CATLDllModule >
{
};

CATLDllModule _AtlModule;

BOOL APIENTRY DllMain(HANDLE /*hModule*/, DWORD dwReason, LPVOID lpReserved)
{
    BOOL retVal = _AtlModule.DllMain(dwReason, lpReserved); 
	HINSTANCE hInstanceResources = ::LoadLibrary(_T("en_us.dll"));
	_AtlBaseModule.SetResourceInstance(hInstanceResources);
	return retVal;
}
#endif

