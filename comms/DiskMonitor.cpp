#include "StdAfx.h"

#include "DiskMonitor.h"
#include <UtilFilesystem.h>
#include <Thread.h>
#include "DiskAttribute.h"
#include "Logger.h"

CDiskMonitor::CDiskMonitor() : m_doMonitor(false)
{
}

void CDiskMonitor::monitor(const boost::filesystem::path & path)
{
    ATLASSERT(!clib::filesystem::is_directory(path));
    if (clib::filesystem::exists(path))
    {
        m_file = path;
        m_folder = path;
        m_folder.remove_filename();
        m_doMonitor = true;
        clib::thread run(__FUNCTION__, boost::bind(&thread_MonitorFolder, this));
    }
    else
    {
        _DBGLOG(LEVEL_WARNING, (boost::format("File not found - %1%") % path).str().c_str());
    }
}

CDiskMonitor::~CDiskMonitor()
{
    ATLASSERT(m_doMonitor == false);
}

void CDiskMonitor::modified(const boost::filesystem::path & path) 
{
    clib::recursive_mutex::scoped_lock proc(m_mutex);
    m_modified.insert(path);
}

void CDiskMonitor::broadcast() 
{
    clib::recursive_mutex::scoped_lock proc(m_mutex);
    if (!m_modified.empty()) 
    {
        notify();
        m_modified.clear();
    }
}

void CDiskMonitor::notify() 
{
}

ULONG STDMETHODCALLTYPE CDiskMonitor::Release() throw()
{
    ULONG refCount = CUnknown::Release();
    if (refCount == 1 && m_doMonitor == true)  //  One background thread.
    {
        m_doMonitor = false;

        //  Touch the folder to trigger the clean exit of the monitor thread  ---
        CAtlTemporaryFile temp;
        temp.Create(pathToWString(m_folder).c_str());
    }
    return refCount;
}

void CDiskMonitor::thread_MonitorFolder(CComPtr<CDiskMonitor> self)
{
    HANDLE hDir = CreateFile(pathToWString(self->m_folder).c_str(), FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL);

    bool first = true;
    FILE_NOTIFY_INFORMATION Buffer[1024];
    DWORD BytesReturned;
    while (ReadDirectoryChangesW(
        hDir,
        &Buffer,
        sizeof(Buffer),
        FALSE,
        FILE_NOTIFY_CHANGE_FILE_NAME |
        FILE_NOTIFY_CHANGE_DIR_NAME |
        FILE_NOTIFY_CHANGE_ATTRIBUTES |
        FILE_NOTIFY_CHANGE_SIZE |
        FILE_NOTIFY_CHANGE_LAST_WRITE |
        FILE_NOTIFY_CHANGE_LAST_ACCESS |
        FILE_NOTIFY_CHANGE_CREATION |
        FILE_NOTIFY_CHANGE_SECURITY,
        &BytesReturned,
        NULL,
        NULL
        ))
    {
        if (first) {
            first = false;
            continue;
        }
        else if (!self->m_doMonitor) {
            return;
        }
        bool modified = false;
        for (int i = 0; !Buffer[i].NextEntryOffset; ++i)
        {
#ifdef _DEBUG
            CString helper_txt;
            switch (Buffer[i].Action)
            {
            case FILE_ACTION_ADDED:
                helper_txt = "The file was added to the directory"; break;
            case FILE_ACTION_REMOVED:
                helper_txt = "The file was removed from the directory"; break;
            case FILE_ACTION_MODIFIED:
                helper_txt = "The file was modified. This can be a change in the time stamp or attributes."; break;
            case FILE_ACTION_RENAMED_OLD_NAME:
                helper_txt = "The file was renamed and this is the old name."; break;
            case FILE_ACTION_RENAMED_NEW_NAME:
                helper_txt = "The file was renamed and this is the new name."; break;
            }
#endif
            std::_tstring filename(Buffer[i].FileName, Buffer[i].FileNameLength / sizeof(_T(' ')));
            boost::filesystem::wpath filePath = self->m_folder / filename;
            if (self->m_file.compare(filePath) == 0)
            {
                self->modified(self->m_file);
                modified = true;
            }
        }
        if (modified)
            self->broadcast();
    }
}

//  ===========================================================================
CAttributeMonitor::CAttributeMonitor(IAttribute* attr) : m_attr(attr), CDiskMonitor()
{
    if (CComQIPtr<IDiskAttribute> diskAttr = attr) {
        monitor(diskAttr->GetPath());
    }
}

void CAttributeMonitor::notify()
{
    if (!m_attr->Exists()) {
        m_attr->Refresh(false, NULL, true);
    }
    else {
        m_attr->GetText();
    }
}
