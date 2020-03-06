#pragma once

#include "clib.h"

typedef std::pair<std::_tstring, std::_tstring> SectionLabel;
typedef std::pair<SectionLabel, _variant_t> SectionLabelDefault;

static const SectionLabelDefault GLOBAL_APP_VERSION(SectionLabel(_T("General"), _T("Version")), _T(""));
static const SectionLabelDefault GLOBAL_USER(SectionLabel(_T("General"), _T("User")), _T(""));
static const SectionLabelDefault GLOBAL_PASSWORD(SectionLabel(_T("General"), _T("Password")), _T(""));
static const SectionLabelDefault GLOBAL_DEBUG_LOGRESULTXML(SectionLabel(_T("Debug"), _T("LogResultXML")), false);

typedef boost::signal<void(SectionLabel *)> globaldata_signal_type;
typedef globaldata_signal_type::slot_type globaldata_slot_type;

__interface IConfig : public IUnknown
{
    const TCHAR * GetLabel();
    void SetLabel(const std::_tstring & label, const std::_tstring & ext = _T("cfg"));
    const boost::filesystem::path & GetEnvironmentFolder(boost::filesystem::path & path);
    _variant_t Get(const std::_tstring & section, const std::_tstring & label, const _variant_t & defaultVal);
    _variant_t Get(const SectionLabel & sectionLabel, const _variant_t & defaultVal);
    _variant_t Get(const SectionLabelDefault & sectionLabel);
    void Set(const std::_tstring & section, const std::_tstring & label, const _variant_t & val);
    void Set(const SectionLabel & sectionLabel, const _variant_t & val);
    void Set(const SectionLabelDefault & sectionLabelDefault, const _variant_t & val);
    void Set(const SectionLabelDefault & sectionLabelDefault, const TCHAR * val);
    void Set(const SectionLabelDefault & sectionLabelDefault, const std::_tstring & val);
    boost::signals::connection ConnectSlot(const globaldata_slot_type & slot);
    void Disconnect(boost::signals::connection& slot);

    //void SetLabel(const CString & label);
    //void Reset(const CString & newLabel);
    //void SaveAs(const CString & newLabel);
};

typedef StlLinked<IConfig> IConfigAdapt;

CLIB_API void SetApplicationName(const std::_tstring & appName);
namespace global
{
CLIB_API const TCHAR * GetApplicationName();
}

CLIB_API const boost::filesystem::path & GetProgramPath(boost::filesystem::path & path);
CLIB_API const boost::filesystem::path & GetProgramFolder(boost::filesystem::path & path);
CLIB_API const boost::filesystem::path & GetProgramFilesX86Folder(boost::filesystem::path & path);
CLIB_API const boost::filesystem::path & GetProgramFilesFolder(boost::filesystem::path & path);
CLIB_API const boost::filesystem::path & GetAppDataFolder(boost::filesystem::path & path);
CLIB_API const boost::filesystem::path & GetDocumentsFolder(boost::filesystem::path & path);
CLIB_API const boost::filesystem::path & GetMyDocumentsFolder(boost::filesystem::path & path);
CLIB_API const boost::filesystem::path & GetTempFolder(boost::filesystem::path & path);
CLIB_API const boost::filesystem::path & GetCompanyFolder(boost::filesystem::path & path);
CLIB_API const boost::filesystem::path & GetApplicationFolder(boost::filesystem::path & path);
CLIB_API const boost::filesystem::path & GetUserFolder(boost::filesystem::path & path, const std::_tstring & user);
CLIB_API const boost::filesystem::path & GetEnvironmentFolder(boost::filesystem::path & path, const std::_tstring & user, const std::_tstring & environment);
CLIB_API const boost::filesystem::path & GetConfigPath(const std::_tstring & environment, boost::filesystem::path & path);
CLIB_API const boost::filesystem::path & GetIniPath(boost::filesystem::path & path);

/*
CLIB_API IConfig * CreateIConfig(HINSTANCE hInstance, const TCHAR* altWorkspace=0); //global - in workspace
CLIB_API void ChangeIConfig(const CString & label);
CLIB_API IConfig * GetIConfig(QUERYBUILDER_CFG);
CLIB_API IConfig * CreateIniIConfig(HINSTANCE hInstance, const TCHAR* altWorkspace=0); //global - in local directory
CLIB_API IConfig * CreatePrivateIConfig(const TCHAR* altWorkspace=0);
CLIB_API IConfig * CreatePrivateIniIConfig(const TCHAR* altWorkspace=0);

CLIB_API void DeleteAndChangeIConfig(const CString & label);
CLIB_API void SaveAsIConfig(const CString & newLabel);
*/

const TCHAR * const QUERYBUILDER_INI = _T("eclide_ini");
const TCHAR * const QUERYBUILDER_CFG = _T("default_cfg");

const TCHAR * const GRETL_INI = _T("GraphicalQueryBuilder_ini");
const TCHAR * const GRETL_CFG = _T("default_cfg");

const TCHAR * const AMT_INI = _T("amt_ini");
const TCHAR * const AMT_CFG_LHS = _T("default_cfg");
const TCHAR * const AMT_CFG_RHS = _T("default_rhs_cfg");

CLIB_API IConfig * CreateIConfig(const std::_tstring & id, const boost::filesystem::path & path, bool forcePathFlag=false);
CLIB_API IConfig * CreateIConfig(const std::_tstring & id, const std::_tstring & env);
CLIB_API IConfig * CreateIConfig(const std::_tstring & id);
CLIB_API IConfig * GetIConfig(const std::_tstring & id);
#ifdef __IServiceProvider_INTERFACE_DEFINED__
    CLIB_API IServiceProvider * GetIServiceProvider(const std::_tstring & id = QUERYBUILDER_CFG);
#endif		
