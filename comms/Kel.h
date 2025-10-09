#pragma once

#include "comms.h"
#include <global.h>
#include "SMCVersion.h"

extern COMMS_API const SectionLabelDefault GLOBAL_KEL_OVERRIDEDEFAULTSELECTION;
extern COMMS_API const SectionLabelDefault GLOBAL_KEL_LOCATION;

__interface IKel : public IUnknown
{
    const TCHAR *GetVersionString(std::_tstring & version) const;
    const bool IsExecutableBad() const;
    SMC::IVersion *GetBuild() const;
    const TCHAR *GetPrefWarnings(std::_tstring & warnings) const;
    const TCHAR *GetPrefErrors(std::_tstring & warnings) const;

    boost::filesystem::path GetWorkingFolder() const;
    boost::filesystem::path GetFilePath() const;
    boost::filesystem::path GetFolderPath() const;
};

namespace KEL
{
    COMMS_API IKel *CreateIKel(bool force = false);
    COMMS_API void RescanClients();
    COMMS_API IKel *MatchVersion();
    COMMS_API IKel *GetBestMatch();
    COMMS_API IKel *SetComboToBest();
    COMMS_API IKel *GetCurrentTool();
    COMMS_API IKel *SetCurrentClient();
    COMMS_API void ClearKelSingletons();
    COMMS_API std::wstring GetAll(std::vector<std::wstring> &strVec);
    COMMS_API IKel *ToolFromVersion(const std::_tstring &versionStr);
    COMMS_API IKel *ToolFromExePath(const std::_tstring &exePath);
}