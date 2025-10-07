#include "StdAfx.h"

#include "Kel.h"
#include "comms.h"
#include "SMC.h"
#include <UtilFilesystem.h>
#include <cache.h>
#include <Logger.h>
#include <cmdProcess.h>

// Global constants
static const TCHAR KELSection[] = _T("KEL");
const SectionLabelDefault GLOBAL_KEL_OVERRIDEDEFAULTSELECTION(SectionLabel(KELSection, _T("OverrideDefaultSelection")), false);
const SectionLabelDefault GLOBAL_KEL_LOCATION(SectionLabel(KELSection, _T("Location")), _T(""));

namespace algo = boost::algorithm;

class CKel : public IKel, public clib::CLockableUnknown
{
protected:
    CComPtr<IConfig> m_config;
    std::_tstring m_toolFile;
    boost::filesystem::path m_toolFilePath;
    boost::filesystem::path m_toolFolderPath;

    std::_tstring m_workingFolder;
    boost::filesystem::path m_workingFolderPath;

    mutable CComPtr<SMC::IVersion> m_toolVersion;

public:
    mutable std::_tstring m_version;
    std::_tstring m_errors;
    std::_tstring m_warnings;

    BEGIN_CLOCKABLEUNKNOWN
    END_CUNKNOWN(CUnknown)

    CKel(IConfig *config, const std::_tstring &toolFile) : m_config(config), m_toolFile(toolFile)
    {
        ATLASSERT(m_config);
        m_toolFilePath = stringToPath(m_toolFile);
        m_toolFolderPath = m_toolFilePath.parent_path();
        m_workingFolder = CString(m_config->Get(GLOBAL_KEL_LOCATION));
        m_workingFolderPath = stringToPath(m_workingFolder);

        std::_tstring in;
        std::_tstring err;
        std::_tstring command = _T("java -jar KEL.jar --version");
        int result = runProcess(command, m_toolFolderPath.wstring(), _T(""), in, m_version, err);

        if (m_version.empty())
        {
            boost::filesystem::path currentPath = m_toolFilePath;
            boost::filesystem::path parent = m_toolFolderPath;

            // Walk up the path to find HPCCSystems folder
            while (!parent.empty())
            {
                boost::filesystem::path grandParent = parent.parent_path();
                if (grandParent.filename().wstring() == _T("HPCCSystems"))
                {
                    // parent is the version folder
                    m_version = parent.filename().wstring();
                    break;
                }
                parent = grandParent;
            }
        }

        GetBuild();
    }

    const TCHAR *GetCacheID() const
    {
        return m_toolFile.c_str();
    }

    const TCHAR *GetVersionString(std::_tstring &version) const
    {
        return m_toolVersion->GetString(version);
    }

    const bool IsExecutableBad() const
    {
        return m_toolVersion->IsExecutableBad();
    }

    SMC::IVersion *GetBuild() const
    {
        if (!m_toolVersion)
        {
            CString version = m_version.c_str();
            m_toolVersion = SMC::CreateVersion(m_toolFile.c_str(), version, true);
        }

        return m_toolVersion;
    }

    const TCHAR *GetPrefWarnings(std::_tstring &warnings) const
    {
        warnings = m_warnings;
        return warnings.c_str();
    }

    const TCHAR *GetPrefErrors(std::_tstring &errors) const
    {
        errors = m_errors;
        if (!errors.empty())
            errors += _T("\r\n");
        if (!clib::filesystem::exists(m_toolFilePath))
            errors += (boost::_tformat(_T("KEL path invalid:  %1%")) % m_toolFile).str() + _T("\r\n");
        else if (clib::filesystem::is_directory(m_toolFilePath))
            errors += (boost::_tformat(_T("KEL path does not specify a KEL executable:  %1%")) % m_toolFile).str() + _T("\r\n");

        if (!m_workingFolder.empty() && !clib::filesystem::exists(m_workingFolder))
        {
            std::_tstring error_msg = (boost::_tformat(_T("Working folder invalid:  %1%")) % m_workingFolder).str() + _T("\r\nCreate?");
            if (::MessageBox(NULL, error_msg.c_str(), _T("ECL IDE"), MB_ICONASTERISK | MB_YESNO) == IDYES)
            {
                try
                {
                    boost::filesystem::create_directories(m_workingFolder);
                }
                catch (const boost::filesystem::filesystem_error &)
                {
                    errors += (boost::_tformat(_T("Failed to create folder:  %1%")) % m_workingFolder).str() + _T("\r\n");
                }
            }
            else
                errors += (boost::_tformat(_T("Working folder invalid:  %1%")) % m_workingFolder).str() + _T("\r\n");
        }

        return errors.c_str();
    }

    boost::filesystem::path GetWorkingFolder() const
    {
        return m_workingFolder;
    }

    boost::filesystem::path GetFilePath() const
    {
        return m_toolFilePath;
    }

    boost::filesystem::path GetFolderPath() const
    {
        return m_toolFolderPath;
    }
};

typedef StlLinked<CKel> CKelAdapt;
typedef std::vector<CKelAdapt> CKelVector;

class CKelCompare
{
protected:
    SMC::IVersionCompare m_versionCompare;

public:
    bool operator()(CKelAdapt &l, CKelAdapt &r)
    {
        return m_versionCompare(l->GetBuild(), r->GetBuild());
    }
};

CacheT<std::_tstring, CKel> KelCache;
CKelVector g_foundKel;
CComPtr<IKel> g_kel;
CComPtr<IKel> g_kelBest;
CComPtr<IKel> g_kelCombo;

CKel *CreateKelRaw(IConfig *config, const std::_tstring &toolFile)
{
    return KelCache.Get(new CKel(config, toolFile));
}

boost::filesystem::path KelSubPath(const boost::filesystem::path &directory)
{
    // KEL is installed in pattern: HPCCSystems\<version>\KEL\KEL.bat
    return directory / "KEL" / "KEL.bat";
}

void PushUniqueKel(CKelVector &results, const boost::filesystem::path &kelPath)
{
    bool found = std::any_of(results.begin(), results.end(), [&kelPath](const CKelAdapt &obj)
                             { return (_tcsicmp(obj.get()->GetFilePath().c_str(), kelPath.c_str()) == 0); });
    if (!found)
    {
        CComPtr<IConfig> config = GetIConfig(QUERYBUILDER_CFG);
        results.push_back(CreateKelRaw(config, pathToWString(kelPath)));
    }
}

unsigned int FindAllKel(const boost::filesystem::path &progFiles, CKelVector &results)
{
    if (exists(progFiles))
    {
        using namespace boost::filesystem;
        { //  Locate KEL installs in pattern: HPCCSystems\<version>\KEL\KEL.bat
            path testFolder = progFiles / "HPCCSystems";
            if (exists(testFolder))
            {
                directory_iterator end_itr;
                for (directory_iterator itr(testFolder); itr != end_itr; ++itr)
                {
                    // Each subdirectory under HPCCSystems is a version folder (e.g., 1.0.0, 2.0.0)
#if (BOOST_FILESYSTEM_VERSION == 3)
                    if (is_directory(itr->path()))
                    {
                        // Search in: <version>\KEL\KEL.bat
                        path kelPath = KelSubPath(itr->path());
#else
                    if (is_directory(*itr))
                    {
                        path kelPath = KelSubPath(*itr);
#endif
                        if (exists(kelPath))
                        {
                            PushUniqueKel(results, kelPath);
                        }

                        // Also search in: <version>\clienttools\IDEPlugins\KEL\KEL.bat
#if (BOOST_FILESYSTEM_VERSION == 3)
                        path clienttoolsKelPath = itr->path() / "clienttools" / "IDEPlugins" / "KEL" / "KEL.bat";
#else
                        path clienttoolsKelPath = (*itr) / "clienttools" / "IDEPlugins" / "KEL" / "KEL.bat";
#endif
                        if (exists(clienttoolsKelPath))
                        {
                            PushUniqueKel(results, clienttoolsKelPath);
                        }
                    }
                }
            }
        }
        std::sort(results.begin(), results.end(), CKelCompare());
    }
    return results.size();
}

unsigned int FindRelative(const boost::filesystem::path &progFiles, CKelVector &results)
{
    using namespace boost::filesystem;
    boost::filesystem::path current = boost::filesystem::current_path();
    CComPtr<IConfig> config = GetIConfig(QUERYBUILDER_CFG);

    // Search up to 3 parent directories for KEL\KEL.bat
    for (int i = 0; i < 3; i++)
    {
        current = current.parent_path();
        path kelPath = KelSubPath(current);
        if (exists(kelPath))
        {
            _DBGLOG(LEVEL_INFO, (boost::_tformat(_T("KEL.bat relative path:  %1%")) % kelPath.c_str()).str().c_str());
            PushUniqueKel(results, kelPath);
        }
    }
    return results.size();
}

unsigned int FindAllKel(CKelVector &results)
{
    boost::filesystem::path progFiles;
    GetProgramFilesX86Folder(progFiles);
    FindAllKel(progFiles, results);
    GetProgramFilesFolder(progFiles);
    FindAllKel(progFiles, results);
    FindRelative(progFiles, results);
    return results.size();
}

const TCHAR *const warningTpl = _T("KEL/Server mismatch:\r\nKEL:\t%1%\r\nServer:\t%2%");

namespace KEL
{

    void RescanClients()
    {
        g_foundKel.clear();
        FindAllKel(g_foundKel);
    }

    void ClearKelSingletons()
    {
        KelCache.Clear();
        g_kel = NULL;
    }

    IKel *CreateIKel(bool force)
    {
        if (g_kel)
            return g_kel;

        CComPtr<IConfig> config = GetIConfig(QUERYBUILDER_CFG);
        CString client = CString(config->Get(GLOBAL_KEL_LOCATION));
        bool override = config->Get(GLOBAL_KEL_OVERRIDEDEFAULTSELECTION);
        if (override && !client.IsEmpty())
        {
            g_kel = CreateKelRaw(config, (const TCHAR *)client);
        }
        else
        {
            if (!g_foundKel.size())
                FindAllKel(g_foundKel);

            g_kel = GetBestMatch();
        }
        return g_kel;
    }

    IKel *GetBestMatch()
    {
        if (!g_kelBest)
        {
            g_kelBest = MatchVersion();
        }
        return g_kelBest;
    }

    IKel *SetComboToBest()
    {
        if (g_kelBest)
        {
            g_kelCombo = g_kelBest;
        }
        return g_kelCombo;
    }

    IKel *GetCurrentTool()
    {
        return g_kel;
    }

    IKel *SetCurrentClient()
    {
        CComPtr<IConfig> config = GetIConfig(QUERYBUILDER_CFG);
        CString client = CString(config->Get(GLOBAL_KEL_LOCATION));
        bool override = config->Get(GLOBAL_KEL_OVERRIDEDEFAULTSELECTION);

        if (!override && g_kelBest)
            g_kel = g_kelBest;
        else if (override && client.GetLength())
        {
            CComPtr<IKel> kel = ToolFromExePath(client.GetString());
            if (!kel)
                g_kel = CreateKelRaw(config, (const TCHAR *)client);
        }

        return g_kel;
    }

    IKel *MatchVersion()
    {
        // Always select the latest version
        if (g_foundKel.size())
        {
            // The vector is sorted in ascending order, so the latest is at the end
            for (CKelVector::reverse_iterator itr = g_foundKel.rbegin(); itr != g_foundKel.rend(); ++itr)
            {
                if (!itr->get()->IsExecutableBad())
                {
                    g_kelBest = itr->get();
                    return g_kelBest;
                }
            }

            // If all KEL versions are bad, just return the latest one anyway
            if (g_foundKel.size() > 0)
            {
                g_kelBest = g_foundKel.back().get();
                return g_kelBest;
            }
        }
        return NULL;
    }

    std::wstring GetAll(std::vector<std::wstring> &strVec)
    {
        StlLinked<CKel> matchedKel;
        if (g_foundKel.size() > 0)
        {
            std::wstring version;
            for (CKelVector::const_iterator itr = g_foundKel.begin(); itr != g_foundKel.end(); ++itr)
            {
                itr->get()->GetVersionString(version);
                if (version.length())
                {
                    strVec.push_back(version);
                }
            }
            if (g_kel)
            {
                g_kel->GetVersionString(version);
                boost::algorithm::trim(version);
            }
            std::sort(strVec.begin(), strVec.end());
            return version;
        }
        return _T("");
    }

    IKel *ToolFromVersion(const std::_tstring &versionStr)
    {
        std::wstring version;
        for (CKelVector::const_iterator itr = g_foundKel.begin(); itr != g_foundKel.end(); ++itr)
        {
            itr->get()->GetVersionString(version);
            if (version.length() && _tcsicmp(version.c_str(), versionStr.c_str()) == 0)
            {
                g_kelCombo = *itr;
                return *itr;
            }
        }
        return NULL;
    }

    IKel *ToolFromExePath(const std::_tstring &exePath)
    {
        boost::filesystem::path path;
        for (CKelVector::const_iterator itr = g_foundKel.begin(); itr != g_foundKel.end(); ++itr)
        {
            path = itr->get()->GetFilePath();
            if (!path.empty() && _tcsicmp(path.c_str(), exePath.c_str()) == 0)
            {
                return *itr;
            }
        }
        return NULL;
    }

} // namespace KEL
