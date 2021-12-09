#pragma once

#include "wlib.h"
#include "global.h"
#include <Dali.h> //comms
#include <Repository.h> //comms

__interface ILoginConfigPreferences
{
    bool ShowConfigPreference(IConfig *config); //true if changed config
};

__interface IConfig;

struct STRUCT_DALI
{
    StlLinked<Dali::IDali> m_dali;
    std::_tstring cluster;
    std::_tstring queue;

    void Clear()
    {
        m_dali = NULL;
        cluster = _T("");
        queue = _T("");
    }
};

WLIB_API INT_PTR DoModalLogin(ILoginConfigPreferences *prefs, IConfig* configIni, IConfig* config, bool verifyUser=true, const TCHAR *title=0);
WLIB_API INT_PTR DoModalLoginAMT(ILoginConfigPreferences *prefs, IConfig* configIni, StlLinked<IRepository> & repositoryLHS, StlLinked<IRepository> & repositoryRHS, STRUCT_DALI & daliLHS, STRUCT_DALI & daliRHS);
WLIB_API bool PopulateConfigCombo(WTL::CComboBox &configCombo, const std::_tstring &defaultValue);
WLIB_API bool PopulateCTConfigCombo(WTL::CComboBox& configCombo, const std::_tstring& savedValue);
WLIB_API void SetDefaultConfig(IConfig *iniFile, const std::_tstring & defaultConfig);
WLIB_API const TCHAR * GetAboutVersion(std::_tstring &version);
WLIB_API const TCHAR* GetClientVersion(std::_tstring& version);
WLIB_API bool CheckPasswordExpiration(int passwordExpirationWarningDays, int passwordDaysRemaining, CString &retMsg);