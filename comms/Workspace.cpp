#include "StdAfx.h"
//#include "Workspace.h"

#include "clib.h"
#include "cache.h"
#include "global.h"
#include "repository.h"
#include "md5.hpp"
#include "logger.h"
#include <UtilFilesystem.h>

const TCHAR * const DEFAULT_LABEL = _T("Default");

IWorkspaceItem * CreateIWorkspaceItem(IRepository * repository, const std::string & data);
//  ===========================================================================
class CWorkspaceItemCompare
{
public:
    bool operator ()(const IWorkspaceItemAdapt & l, const IWorkspaceItemAdapt & r)
    {
        if (l->GetGroupIndex() == r->GetGroupIndex())
            return l->GetTabIndex() < r->GetTabIndex();
        return l->GetGroupIndex() < r->GetGroupIndex();
    }
};
//  ===========================================================================
#define BACKUP_EVERY 6 * 60
#define BACKUP_TOTAL 7 * 24
boost::posix_time::ptime g_lastBackupTime = boost::posix_time::second_clock::local_time();

template<typename T>
void save(const T &s, const char * filename)
{
    boost::filesystem::path filePath(filename, boost::filesystem::native);
    if ((boost::posix_time::second_clock::local_time() - g_lastBackupTime) > boost::posix_time::time_duration(boost::posix_time::seconds(BACKUP_EVERY)))
    {
        g_lastBackupTime = boost::posix_time::second_clock::local_time();
        for (int i = BACKUP_TOTAL; i > 0; --i)
        {
            boost::filesystem::path bakFilepath = filePath;
            bakFilepath.replace_extension((boost::format("bak%03i") % i).str());
            boost::filesystem::path prevBakFilepath = filePath;
            if (i > 1)	
                prevBakFilepath.replace_extension((boost::format("bak%03i") % (i - 1)).str());

            if (clib::filesystem::exists(bakFilepath))
            {
                try {
                    boost::filesystem::remove(bakFilepath);
                } catch (const boost::filesystem::filesystem_error & ex) {
                    _DBGLOG(LEVEL_WARNING, ex.what());
                    _DBGLOG(LEVEL_WARNING, pathToWString(bakFilepath).c_str());
                }
            }

            if (clib::filesystem::exists(prevBakFilepath))
            {
                try
                {
                    boost::filesystem::rename(prevBakFilepath, bakFilepath);
                } catch (const boost::filesystem::filesystem_error & ex) {
                    _DBGLOG(LEVEL_WARNING, ex.what());
                }
            }
        }
    }
    // make an archive
    std::ofstream ofs(filename);
    if (ofs.good()) {
        try {
            boost::archive::xml_oarchive oa(ofs);
            oa << BOOST_SERIALIZATION_NVP(s);
        } catch (boost::archive::archive_exception & e) {
            _DBGLOG(LEVEL_WARNING, e.what());
            ATLASSERT(false);  //  TODO:  Why is there an infrequent exception.
        } catch (std::exception & e) {
            _DBGLOG(LEVEL_WARNING, e.what());
            ATLASSERT(false);  //  TODO:  Why is there an infrequent exception.
        }
        ofs.flush();
        ofs.close();
    } else {
        _DBGLOG(LEVEL_WARNING, (boost::format("Failed to open %1% for writing.") % filename).str().c_str());
    }
}

template<typename T>
void restore(T &s, const char * filename)
{
    // open the archive
    std::ifstream ifs(filename);
    if (ifs.good()) {
        try {
            boost::archive::xml_iarchive ia(ifs);
            // restore the schedule from the archive
            ia >> BOOST_SERIALIZATION_NVP(s);
            s.UpdateID();
        } catch (boost::archive::archive_exception & e) {
            _DBGLOG(LEVEL_WARNING, e.what());
            ATLASSERT(false);
            //boost::filesystem::remove();
        } catch (std::exception & e) {
            _DBGLOG(LEVEL_WARNING, e.what());
            ATLASSERT(false);
        }
    } else {
        _DBGLOG(LEVEL_WARNING, (boost::format("Failed to open %1% for reading.") % filename).str().c_str());
    }
}
//  ===========================================================================
typedef std::vector<std::_tstring> WorkspaceVector;
typedef std::vector<std::string> WindowsVector;
//  ===========================================================================
class CWorkspace : public IWorkspace, public clib::CLockableUnknown
{
protected:
    std::_tstring m_id;
    std::_tstring m_label;
    CComPtr<IRepository> m_repository;
    boost::filesystem::path m_environmentFolder;
    std::_tstring m_folder;
    mutable std::_tstring m_checksum;
    bool m_showInRoot;
    bool m_placeholder;
    bool m_attributesOnly;

    WorkspaceVector m_childWorkspaces;
    WindowsVector m_windows;

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(IWorkspace)
    END_CUNKNOWN(clib::CLockableUnknown)

    CWorkspace()
    {
    }
    CWorkspace(IRepository * repository, const std::_tstring & label, bool showInRoot, bool placeholder, bool attributesOnly) : m_label(label), m_showInRoot(showInRoot), m_placeholder(placeholder), m_attributesOnly(attributesOnly)
    {
        m_repository = repository;
        m_repository->GetEnvironmentFolder(m_environmentFolder);
        UpdateID();
    }
    void UpdateID()
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        m_folder = pathToWString(m_environmentFolder);
        m_id = m_folder + _T("\\") + m_label + _T(".ecl_ws");
    }
    const TCHAR * GetCacheID() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_id.c_str();
    }
    const TCHAR * GetID() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_id.c_str();
    }
    const TCHAR * GetLabel() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_label.c_str();
    }
    const TCHAR *GetChecksum() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        std::string checksum;
        std::string premd5;
        IWorkspaceVector childWorkspaces;
        GetChildWorkspaces(childWorkspaces);
        for (IWorkspaceVector::const_iterator itr = childWorkspaces.begin(); itr != childWorkspaces.end(); ++itr)
            premd5 += CT2A(itr->get()->GetChecksum());

        IWorkspaceItemVector workspaceItems;
        GetWindows(workspaceItems);
        for (IWorkspaceItemVector::const_iterator itr = workspaceItems.begin(); itr != workspaceItems.end(); ++itr)
            premd5 += CT2A(itr->get()->GetChecksum());

        md5_string(premd5, checksum);
        m_checksum = CA2T(checksum.c_str());
        return m_checksum.c_str();
    }
    bool IsDefault() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return boost::algorithm::iequals(GetLabel(), DEFAULT_LABEL);
    }
    bool ShowInRoot() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_showInRoot;
    }
    void Save() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        if (!m_placeholder)
            save(this, CT2A(m_id.c_str()));
    }
    void Restore()
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        if (clib::filesystem::exists(stringToPath(m_id)))
            restore(*this, CT2A(m_id.c_str()));
    }
    bool Remove()
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return RemoveWorkspace(m_repository, GetLabel());
    }
    unsigned int GetChildWorkspaces(IWorkspaceVector & results) const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        for(WorkspaceVector::const_iterator itr = m_childWorkspaces.begin(); itr != m_childWorkspaces.end(); ++itr)
        {
            StlLinked<IWorkspace> workspace = CreateIWorkspace(m_repository, itr->c_str(), false);
            if (workspace)
                results.push_back(workspace);
        }
        return results.size();
    }
    void PasteWorkspace(IWorkspace * workspace)
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        ATLASSERT(workspace);
        if (workspace && std::find(m_childWorkspaces.begin(), m_childWorkspaces.end(), workspace->GetLabel()) == m_childWorkspaces.end())
        {
            m_childWorkspaces.push_back(workspace->GetLabel());
            Save();
        }
    }
    IWorkspace * CreateChildWorkspace(const std::_tstring & label)
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        IWorkspace * retVal = CreateIWorkspace(m_repository, label, false);
        if (retVal && std::find(m_childWorkspaces.begin(), m_childWorkspaces.end(), retVal->GetLabel()) == m_childWorkspaces.end())
        {
            m_childWorkspaces.push_back(retVal->GetLabel());
            Save();
        }
        return retVal;
    }
    unsigned int GetWindows(IWorkspaceItemVector & results) const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        for(WindowsVector::const_iterator itr = m_windows.begin(); itr != m_windows.end(); ++itr)
        {
            CComPtr<IWorkspaceItem> item = CreateIWorkspaceItem(m_repository, *itr);
            if (!m_attributesOnly || (m_attributesOnly && item->GetType() == WORKSPACE_ITEM_ATTRIBUTE))
                results.push_back(item.p);
        }
        std::sort(results.begin(), results.end(), CWorkspaceItemCompare());
        return results.size();
    }
    void SetWindows(const IWorkspaceItemVector & windows)
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        m_windows.clear();
        for(IWorkspaceItemVector::const_iterator itr = windows.begin(); itr != windows.end(); ++itr)
        {
            std::string data;
            m_windows.push_back(itr->get()->Serialize(data));
        }
        Save();
    }
    void SetWindows(const CPersistMapVector & windows)
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        m_windows.clear();
        for(CPersistMapVector::const_iterator itr = windows.begin(); itr != windows.end(); ++itr)
        {
            std::_tstring data;
            if (itr->get()->serialize(data))
                m_windows.push_back(static_cast<const char *>(CT2A(data.c_str(), CP_UTF8)));
        }
        Save();
    }
    void AddWindow(const IWorkspaceItem & window)
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        std::string data;
        m_windows.push_back(window.Serialize(data));
        Save();
    }
    bool Exists() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return !m_placeholder;
    }
    bool Create()
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        if (!Exists())	
        {
            m_placeholder = false;
            proc.unlock();
            Save();
            return true;
        }
        return false;
    }


    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        std::_tstring id = m_id;
        std::_tstring folder = m_folder;
        try {
            ar	& BOOST_SERIALIZATION_NVP(m_id)
                & BOOST_SERIALIZATION_NVP(m_folder)
                & BOOST_SERIALIZATION_NVP(m_label)
                & BOOST_SERIALIZATION_NVP(m_childWorkspaces)
                & BOOST_SERIALIZATION_NVP(m_windows)
                & BOOST_SERIALIZATION_NVP(m_showInRoot)
                ;
        } catch (boost::archive::archive_exception & e) {
            _DBGLOG(LEVEL_WARNING, e.what());
            ATLASSERT(false);  //  TODO:  Why is there an infrequent exception.
        } catch (std::exception & e) {
            _DBGLOG(LEVEL_WARNING, e.what());
            ATLASSERT(false);  //  TODO:  Why is there an infrequent exception.
        }
        m_id = id;
        m_folder = folder;
    }
};
BOOST_CLASS_VERSION(CWorkspace, 0)
//  ===========================================================================
static CacheT<std::_tstring, CWorkspace> CWorkspaceCache;

CWorkspace * AttachWorkspaceRaw(IRepository * repository, const std::_tstring & label, bool showInRoot, bool placeholder, bool attributesOnly)
{
    StlLinked<CWorkspace> workspace = CWorkspaceCache.Get(new CWorkspace(repository, label, showInRoot, placeholder, attributesOnly));
    if (!placeholder && !workspace->Exists())
        workspace->Create();
    workspace->Restore();
    workspace->Save();
    return workspace;
}

#ifdef _UNICODE
CWorkspace * AttachWorkspaceRaw(IRepository * repository, const std::string & label, bool showInRoot, bool placeholder, bool attributesOnly)
{
    std::_tstring wlabel = CA2T(label.c_str());
    return AttachWorkspaceRaw(repository, wlabel, showInRoot, placeholder, attributesOnly);
}
#endif

IWorkspace * CreateIWorkspace(IRepository * repository, const std::_tstring & label, bool showInRoot, bool placeholder, bool attributesOnly)
{
    if (label.size() == 0)
        return AttachWorkspaceRaw(repository, DEFAULT_LABEL, showInRoot, placeholder, attributesOnly);
    return AttachWorkspaceRaw(repository, label, showInRoot, placeholder, attributesOnly);
}

unsigned int GetWorkspaces(IRepository * repository, IWorkspaceVector * results, bool attributesOnly)
{
    try
    {
        boost::filesystem::path environmentFolder;
        repository->GetEnvironmentFolder(environmentFolder);
        boost::filesystem::directory_iterator end_itr; // default construction yields past-the-end
        for (boost::filesystem::directory_iterator itr(environmentFolder); itr != end_itr; ++itr)
        {
            if (!clib::filesystem::is_directory(*itr))
            {
                if (boost::algorithm::iequals(pathToWString(itr->path().extension()), _T(".ecl_ws")))
                {
                    StlLinked<CWorkspace> workspace = AttachWorkspaceRaw(repository, pathToWString(itr->path().stem()), true, false, attributesOnly);
                    if (workspace->ShowInRoot())
                        results->push_back(workspace.get());
                }
            }
        }
    }
    catch (boost::filesystem::filesystem_error &)
    {
        ATLASSERT(false);	//  Path doesn't exist!
    }
    if (results->size() == 0)	//  Then create the default workspace.
        results->push_back(repository->CreateIWorkspace());
    return results->size();
}
bool WorkspaceExists(IRepository * repository, const std::_tstring & label)
{
    boost::filesystem::path environmentFolder;
    repository->GetEnvironmentFolder(environmentFolder);
    boost::filesystem::directory_iterator end_itr; // default construction yields past-the-end
    for (boost::filesystem::directory_iterator itr(environmentFolder); itr != end_itr; ++itr)
    {
        if (!clib::filesystem::is_directory(*itr))
        {
            if (boost::algorithm::iequals(pathToWString(itr->path().stem()), label))
                return true;
        }
    }
    return false;
}

bool RemoveWorkspace(IRepository * repository, const std::_tstring & label)
{
    bool retVal = false;
    boost::filesystem::path environmentFolder;
    repository->GetEnvironmentFolder(environmentFolder);
    boost::filesystem::directory_iterator end_itr; // default construction yields past-the-end
    for (boost::filesystem::directory_iterator itr(environmentFolder); itr != end_itr; ++itr)
    {
        if (!clib::filesystem::is_directory(*itr))
        {
            if (boost::algorithm::iequals(pathToWString(itr->path().stem()), label))
            {
                try {
                    boost::filesystem::remove(*itr);
                    CWorkspaceCache.erase(label);
                    retVal = true;
                } catch (const boost::filesystem::filesystem_error & ex) {
                    _DBGLOG(LEVEL_WARNING, ex.what());
                    _DBGLOG(LEVEL_WARNING, pathToWString(itr->path()).c_str());
                }
            }
        }
    }
    return retVal;
}

void ClearWorkspaceCache()
{
    ATLTRACE(_T("Workspace cache before clearing(size=%u)\r\n"), CWorkspaceCache.Size());
    CWorkspaceCache.Clear();
}
