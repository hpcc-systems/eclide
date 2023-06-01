#pragma once

#include <global.h>
#include "workunit.h"
#include "Attribute.h"
#include "SMCVersion.h"
#include "EclMeta.h"

static const TCHAR CompilerSection[](_T("Compiler"));
static const SectionLabelDefault GLOBAL_COMPILER_OVERRIDEDEFAULTSELECTION(SectionLabel(CompilerSection, _T("OverrideDefaultSelection")), false);
static const SectionLabelDefault GLOBAL_COMPILER_LOCATION(SectionLabel(CompilerSection, _T("Location")), _T(""));
static const SectionLabelDefault GLOBAL_COMPILER_STDLIB(SectionLabel(CompilerSection, _T("StdLib")), _T(""));
static const SectionLabelDefault GLOBAL_COMPILER_ARGUMENTS(SectionLabel(CompilerSection, _T("Arguments")), _T(""));
static const SectionLabelDefault GLOBAL_COMPILER_WUARGUMENTS(SectionLabel(CompilerSection, _T("WUArguments")), _T(""));
static const SectionLabelDefault GLOBAL_COMPILER_ECLWORKINGFOLDER(SectionLabel(CompilerSection, _T("EclWorkingFolder")), _T(""));
static const SectionLabelDefault GLOBAL_COMPILER_ECLFOLDER00(SectionLabel(CompilerSection, _T("EclFolder00")), _T(""));
static const SectionLabelDefault GLOBAL_COMPILER_ECLFOLDER01(SectionLabel(CompilerSection, _T("EclFolder01")), _T(""));
static const SectionLabelDefault GLOBAL_COMPILER_ECLFOLDER02(SectionLabel(CompilerSection, _T("EclFolder02")), _T(""));
static const SectionLabelDefault GLOBAL_COMPILER_ECLFOLDER03(SectionLabel(CompilerSection, _T("EclFolder03")), _T(""));
static const SectionLabelDefault GLOBAL_COMPILER_ECLFOLDER04(SectionLabel(CompilerSection, _T("EclFolder04")), _T(""));
static const SectionLabelDefault GLOBAL_COMPILER_ECLFOLDER05(SectionLabel(CompilerSection, _T("EclFolder05")), _T(""));
static const SectionLabelDefault GLOBAL_COMPILER_ECLFOLDER06(SectionLabel(CompilerSection, _T("EclFolder06")), _T(""));
static const SectionLabelDefault GLOBAL_COMPILER_ECLFOLDER07(SectionLabel(CompilerSection, _T("EclFolder07")), _T(""));
static const SectionLabelDefault GLOBAL_COMPILER_ECLFOLDER08(SectionLabel(CompilerSection, _T("EclFolder08")), _T(""));
static const SectionLabelDefault GLOBAL_COMPILER_ECLFOLDER09(SectionLabel(CompilerSection, _T("EclFolder09")), _T(""));
static const SectionLabelDefault GLOBAL_COMPILER_DISABLEMETA(SectionLabel(CompilerSection, _T("DisableMeta")), false);

static const SectionLabelDefault GLOBAL_DISABLE_SUBMIT(SectionLabel(_T("Disables"), _T("Submit")), _T(""));
static const SectionLabelDefault GLOBAL_DISABLE_GENERATE(SectionLabel(_T("Disables"), _T("Generate")), _T(""));
static const SectionLabelDefault GLOBAL_DISABLE_COMPILE(SectionLabel(_T("Disables"), _T("Compile")), _T(""));

__interface IEclCC : public IUnknown
{
    const TCHAR* GetVersionString(std::_tstring& version) const;
    const bool IsExecutableBad() const;
    SMC::IVersion * GetBuild() const;
    const TCHAR * GetPrefWarnings(std::_tstring & warnings) const;
    const TCHAR * GetPrefErrors(std::_tstring & warnings) const;
    void EclFoldersFromConfig();
    void PopulateMeta(IAttribute *attribute);

    const TCHAR * GetWorkingFolder() const;
    int GetEclFolderCount() const;
    const TCHAR * GetEclFolder(int i) const;
    std::_tstring GetCompilerFilePath() const;
    bool GetAutoC(IAttribute *attr, const std::_tstring & partialLabel, StdStringVector &set);
    const TCHAR * GetToolTip(const std::_tstring & key, std::_tstring & tooltip);

    void CheckSyntax(const std::_tstring & module, const std::_tstring & attribute, const std::_tstring & path, const std::_tstring & ecl, Dali::CEclExceptionVector & errors) const;
    const TCHAR * Compile(const std::_tstring & path, const std::_tstring & ecl, const std::_tstring & wuid, int resultLimit, const std::_tstring & debugString, std::_tstring & wuXml, bool & hasErrors, Dali::CEclExceptionVector & errors) const;
    bool GetArchive(const std::_tstring & path, const std::_tstring & ecl, std::_tstring & archive, Dali::CEclExceptionVector & errors) const;
    const TCHAR * GetWorkunitXML(const std::_tstring & wuid, std::_tstring & wuXml) const;
    const TCHAR * SaveWorkunitXML(const std::_tstring & wuid, std::_tstring & filePath) const;
    const TCHAR * GetWorkunitResults(const std::_tstring & wuid, bool compileOnly, std::_tstring & results, bool & hasErrors, Dali::CEclExceptionVector & errors) const;

    const TCHAR * GetAttributeLabel(IAttribute * attribute, std::_tstring & label) const;

    bool PluginFolderExists(const std::string & attrTypeStr, const std::string & batchFile, boost::filesystem::path & foundFolder, int level, bool pluginFolder) const;
    bool LocatePlugin(const std::string & attrTypeStr, const std::string & batchFile, boost::filesystem::path & foundFolder) const;
    boost::filesystem::wpath GetIDEPluginFolder() const;
};

enum TRI_BOOL
{
    TRI_BOOL_UNKNOWN = -1,
    TRI_BOOL_FALSE,
    TRI_BOOL_TRUE,
    TRI_BOOL_LAST
};
COMMS_API void EnableLocalRepository(TRI_BOOL enable = TRI_BOOL_TRUE);
COMMS_API TRI_BOOL IsLocalRepositoryEnabled();

COMMS_API void EnableRemoteDaliEnabled(TRI_BOOL enable = TRI_BOOL_TRUE);
COMMS_API TRI_BOOL IsRemoteDaliEnabled();

COMMS_API void EnableRemoteQueueEnabled(TRI_BOOL enable = TRI_BOOL_TRUE);
COMMS_API TRI_BOOL IsRemoteQueueEnabled();

COMMS_API IEclCC* CreateIEclCC(bool force = false);
COMMS_API void RescanClients();
COMMS_API IEclCC* MatchVersion();
COMMS_API IEclCC* GetBestMatch();
COMMS_API IEclCC* SetComboToBest();
COMMS_API IEclCC* GetCurrentCompiler();
COMMS_API IEclCC* SetCurrentClient();
COMMS_API void ClearEclCCSingletons();
COMMS_API std::wstring GetCompilers(std::vector<std::wstring> & strVec);
COMMS_API IEclCC* CompilerFromVersion(const std::_tstring& versionStr);
COMMS_API IEclCC* CompilerFromExePath(const std::_tstring& exePath);