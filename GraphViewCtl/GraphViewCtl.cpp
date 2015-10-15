// GraphViewCtl.cpp : Implementation of DLL Exports.

#include "stdafx.h"
#include "resource.h"

#include "GraphViewContainer.h"
#include "GraphViewCtl.h"
#include "GraphViewCtlApi.h"
#include "config.h"
#include <global.h>
#include "..\cpptooltip\PPTooltip.h"

#ifndef EXCLUDE_ATL_MODULE

class CGraphViewCtlModule : public CAtlDllModuleT< CGraphViewCtlModule >
{
public :
    DECLARE_LIBID(LIBID_GraphViewControlLib)
    DECLARE_REGISTRY_APPID_RESOURCEID(IDR_GRAPHVIEWCTL, "{71787E0F-82B1-40A2-AA4F-30795C2F5262}")
};

CGraphViewCtlModule _AtlModule;

// DLL Entry Point
CComPtr<IConfig> ini;
CComPtr<IConfig> config;
BOOL GRAPHVIEWCTL_API WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    switch(dwReason)
    {
    case DLL_PROCESS_ATTACH:
        {
            BOOL bRet = AtlInitCommonControls(	
                                    ICC_LISTVIEW_CLASSES |
                                    ICC_TREEVIEW_CLASSES |
                                    ICC_BAR_CLASSES |
                                    ICC_TAB_CLASSES |
                                    ICC_UPDOWN_CLASS |
                                    ICC_PROGRESS_CLASS |
                                    ICC_HOTKEY_CLASS |
                                    ICC_ANIMATE_CLASS |
                                    ICC_WIN95_CLASSES |
                                    ICC_DATE_CLASSES |
                                    ICC_USEREX_CLASSES |
                                    ICC_COOL_CLASSES |
                                    ICC_INTERNET_CLASSES |
                                    ICC_PAGESCROLLER_CLASS |
                                    ICC_NATIVEFNTCTL_CLASS);
            ATLASSERT(bRet == TRUE);
            SetApplicationName(_T("eclide.exe"));
            boost::filesystem::path iniPath;
            GetIniPath(iniPath);
            ini = CreateIConfig(QUERYBUILDER_INI, iniPath);
            config = CreateIConfig(QUERYBUILDER_CFG);
            SGV::CGraphView::Register(hInstance);
            CPPToolTip::Register(hInstance);
        }
        break;
    case DLL_PROCESS_DETACH:
        ini = NULL;
        config = NULL;
        SGV::CGraphView::Unregister();
        break;
    }

    return _AtlModule.DllMain(dwReason, lpReserved); 
}

// Used to determine whether the DLL can be unloaded by OLE
STDAPI DllCanUnloadNow(void)
{
    return _AtlModule.DllCanUnloadNow();
}


// Returns a class factory to create an object of the requested type
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
#ifdef SEISINT_LIBEXPORTS
    static Config config;
#endif
    return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}


// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer(void)
{
    // registers object, typelib and all interfaces in typelib
    HRESULT hr = _AtlModule.DllRegisterServer();
    return hr;
}


// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void)
{
    HRESULT hr = _AtlModule.DllUnregisterServer();
    return hr;
}

HRESULT SGV::RegisterServerGraphViewCtl(void)
{
    // registers object, typelib and all interfaces in typelib
    HRESULT hr = _AtlModule.DllRegisterServer();
    return hr;
}


// DllUnregisterServer - Removes entries from the system registry
HRESULT SGV::UnregisterServerGraphViewCtl(void)
{
    HRESULT hr = _AtlModule.DllUnregisterServer();
    return hr;
}

#endif
