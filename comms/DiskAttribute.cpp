#include "StdAfx.h"

#include "AttributeImpl.h"
#include "DiskAttribute.h"
#include "module.h"
#include "SoapUtil.h"
#include "clib.h"
#include "cache.h"
#include "logger.h"
#include "util.h"
#include "UnicodeFile.h"
#include "md5.hpp"
#include "ModuleHelper.h"
#include <UtilFilesystem.h>

#if _COMMS_VER < 68200
using namespace WsAttributes;
#else
#endif

//  ===========================================================================
class CDiskAttribute;
void DeleteAttribute(CDiskAttribute * attr);
class CDiskAttribute : public CAttributeBase, public IDiskAttribute, public IAttributeHistory
{
protected:
    CString m_id;
    mutable CComPtr<IModule> m_module;
    boost::filesystem::wpath m_path;
    std::_tstring m_pathStr;
    CString m_moduleQualifiedLabel;
    CString m_moduleQualifiedLabelNoRoot;
    CString m_label;
    CString m_qualifiedLabel;
    CString m_qualifiedLabelNoRoot;
    CComPtr<IAttributeType> m_type;
    CString m_ecl;
    CString m_description;
    CString m_checksum;
    CString m_checksumLocal;
    CString m_checksumLocalTidied;
    bool m_checkedOut;
    bool m_sandboxed;
    bool m_locked;
    bool m_orphaned;
    unsigned m_version;
    CString m_lockedby;
    CString m_modifiedDate;
    CString m_modifiedBy;
    bool m_eclSet;

    refresh_signal_type on_refresh;

public:
    BEGIN_CLOCKABLEUNKNOWN
        IMPLEMENT_INTERFACE(IDiskAttribute)
        IMPLEMENT_INTERFACE(IAttribute)
        IMPLEMENT_INTERFACE(IAttributeHistory)
    END_CUNKNOWN(clib::CLockableUnknown)

    CDiskAttribute(const IRepository *rep, const TCHAR* module, const TCHAR* label, const TCHAR* type, const boost::filesystem::wpath & path, unsigned version, bool sandboxed) 
        : m_moduleQualifiedLabel(module), m_label(label), m_type(CreateIAttributeType(type)), m_version(version), m_sandboxed(sandboxed)
    {
        ATLASSERT(!boost::algorithm::contains(module, _T("\\")));
        ATLASSERT(!boost::algorithm::contains(module, _T("/")));
        m_repository = const_cast<IRepository *>(rep);
        m_path = path;
        m_pathStr = pathToWString(path);
        m_checkedOut = false;
        m_locked = false;
        m_orphaned = false;
        m_eclSet = false;
        UpdateID();
    }

    virtual ~CDiskAttribute()
    {
    }

    void UpdateID()
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        m_id = m_repository->GetID();
        m_id = m_id + _T("/") + m_moduleQualifiedLabel + _T("/") + m_label + _T("/") + m_type->GetRepositoryCode();
        m_id.MakeLower();
    }

    IRepository* GetRepository()
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_repository;
    }
    const TCHAR *GetChecksum() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_checksum.IsEmpty() ? m_checksumLocal : m_checksum;
    }
    const TCHAR *GetChecksumLocal() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_checksumLocal;
    }
    const TCHAR *GetChecksumLocalTidied() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_checksumLocalTidied;
    }
    void UpdateChecksumLocal()
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        std::_tstring tmp = m_ecl;

        std::string checksum;
        std::string premd5 = CT2A(tmp.c_str());
        md5_string(premd5, checksum);
        m_checksumLocal = checksum.c_str();
        UpdateChecksumLocalTidied();
    }
    void UpdateChecksumLocalTidied()
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        std::_tstring tmp = m_ecl;
        boost::algorithm::replace_all(tmp, _T("\r\n"), _T("\n"));
        std::string checksum;
        std::string premd5 = CT2A(tmp.c_str());
        md5_string(premd5, checksum);
        m_checksumLocalTidied = checksum.c_str();
    }
    const TCHAR *GetID() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_id;
    }
    const TCHAR * GetCacheID() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_id;
    }
    IModule *GetModule() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        if (!m_module)
            m_module = m_repository->GetModule(m_moduleQualifiedLabel);
        return m_module;
    }
    const TCHAR *GetModuleQualifiedLabel(bool excludeRoot = false) const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        if (excludeRoot)
            return m_moduleQualifiedLabelNoRoot;
        return m_moduleQualifiedLabel;
    }

    const TCHAR *GetLabel() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_label;
    }

    const TCHAR *GetQualifiedLabel(bool excludeRoot = false, bool includeExtension = false) const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return (excludeRoot ? m_qualifiedLabelNoRoot : m_qualifiedLabel) + (includeExtension ? m_type->GetFileExtension() : _T(""));
    }

    const TCHAR *GetPath() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_pathStr.c_str();
    }

    const SecAccessFlags GetAccess() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return SecAccess_Full;
    }

    IAttributeType *GetType() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_type;
    }

    const TCHAR *GetText(bool refresh, bool noBroadcast = false) const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        if (refresh || !m_eclSet)
        {
            proc.unlock();
            StlLinked<IAttribute> attr = m_repository->GetAttribute(GetModuleQualifiedLabel(), GetLabel(), GetType(), 0, true, true, noBroadcast);
            proc.lock();
        }
        return m_ecl;
    }

    int GetVersion() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_version;
    }

    bool SetText(const TCHAR* ecl, bool noBroadcast = false) 
    {
        bool result = false;
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        ATLASSERT(ecl);

        CUnicodeFile file;
        file.Create(pathToWString(m_path).c_str(), GENERIC_WRITE, CREATE_ALWAYS);
        if(file.IsOpen())
        {
            m_eclSet = true;
            m_ecl = ecl;
            file.Write(m_ecl);
            file.Close();
            UpdateChecksumLocal();
            m_checksum = m_checksumLocal;

            proc.unlock();
            if (!noBroadcast)
                Refresh(true, NULL, false);
            result = true;
        }
        return result;
    }

    void UnloadText()
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        m_ecl.Empty();
        m_ecl.FreeExtra();
        m_eclSet = false;
    }

    bool IsCheckedOut() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_checkedOut;
    }

    bool IsSandboxed() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_sandboxed;
    }

    bool IsLocked() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_locked;
    }

    bool IsOrphaned() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_orphaned;
    }
    
    const TCHAR *GetLockedBy() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_lockedby;
    }

    const TCHAR * GetModifiedDate() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_modifiedDate;
    }

    const TCHAR * GetModifiedBy() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_modifiedBy;
    }

    ATTRSTATE GetState() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        if (IsOrphaned())
            return ATTRSTATE_ORPHANED;
        else if (IsCheckedOut())
        {
            if (IsSandboxed())
                return ATTRSTATE_CHECKEDOUT_SANDBOXED;
            else
                return ATTRSTATE_CHECKEDOUT;
        }
        else if (IsSandboxed())
            return ATTRSTATE_SANDBOXED;
        else if (IsLocked())
            return ATTRSTATE_LOCKED;
        return ATTRSTATE_NONE;
    }

    virtual const TCHAR *GetStateLabel() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return ATTRSTATELABEL[GetState()];
    }

    unsigned GetHistory(IAttributeHistoryVector & attributes) const
    {
        return attributes.size();
    }

    bool Checkout()
    {
    return true;
    }

    bool Checkin(const TCHAR* comments)
    {
    return true;
  }

    IAttribute * Rename(const TCHAR* label)
    {
        std::_tstring newFilename = label;
        newFilename += _T(".");
        newFilename += GetType()->GetRepositoryCode();
        boost::filesystem::wpath newPath = m_path;
        newPath.remove_filename();
        newPath = newPath / newFilename;
        try
        {
            boost::filesystem::rename(m_path, newPath);
        } catch (const boost::filesystem::filesystem_error & ex) {
            _DBGLOG(LEVEL_WARNING, ex.what());
            return NULL;
        }
        IAttribute * newAttr = m_repository->GetAttribute(GetModuleQualifiedLabel(), label, GetType());
        Refresh(false, newAttr, false);
        return newAttr;
    }

    IAttribute * ChangeAttributeType(const TCHAR* ext)
    {
        if (IsValidExtension(ext)) {
            DeleteAttribute(this);
            m_type = CreateIAttributeType(ext);
        }
        else {
            return NULL;
        }

        return Rename(m_label);
    }

    bool Delete()
    {
        try {
            if (clib::filesystem::exists(m_path))
            {
                SetText(_T(""), true);
                int retVal = MoveToRecycleBin(pathToWString(m_path));
                if (retVal != 0)
                    throw std::exception("Unknown Error During Folder Delete.", retVal);
                Refresh(false, NULL, true);
                return true;
            }
        } catch (const boost::filesystem::filesystem_error & ex) {
            _DBGLOG(LEVEL_WARNING, ex.what());
            return false;
        }
        return false;
    }

    bool Exists() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return clib::filesystem::exists(m_path);
    }

    bool Create()
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        if (Exists())	
            return true;

        if (m_repository->InsertAttribute(GetModuleQualifiedLabel(), GetLabel(), m_type) != NULL)
            return true;

        return false;
    }

    void Update(const std::_tstring & moduleName, const std::_tstring & label, const std::_tstring & ecl, bool noBroadcast)
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        m_moduleQualifiedLabel = moduleName.c_str();
        m_label = label.c_str();
        m_qualifiedLabel = m_moduleQualifiedLabel + _T(".") + m_label;
        CModuleHelper modHelper(m_qualifiedLabel);
        m_moduleQualifiedLabelNoRoot = modHelper.GetModuleLabelNoRoot();
        m_qualifiedLabelNoRoot = modHelper.GetQualifiedLabelNoRoot();
        m_checkedOut = false;
        m_sandboxed = false;
        m_locked = false;

        if (m_type->IsReadOnly())
        {
            m_locked = true;
        }
        else
        {	//  Test for readonly - is there a betterway?
            HANDLE hFile = CreateFile(m_pathStr.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL); 
            if (hFile == INVALID_HANDLE_VALUE) 
                m_locked = true;
            else
                CloseHandle(hFile);
        }

        m_orphaned = false;
        bool eclChanged = false;
        if (!ecl.empty())
        {
            eclChanged = m_ecl.Compare(ecl.c_str()) != 0;
            m_ecl = ecl.c_str();
            m_eclSet = true;
            UpdateChecksumLocal();
        }
        m_description = _T("");
        m_lockedby = _T("");
        m_version = 0;
        m_modifiedDate = _T("");
        m_modifiedBy = _T("");
        UpdateID();
        if (!noBroadcast)
        {
            proc.unlock();
            Refresh(eclChanged, NULL, false);
        }
    }

    int PreProcess(PREPROCESS_TYPE action, const TCHAR * overrideEcl, IAttributeVector & attrs, IAttributeBookkeep & attrProcessed, Dali::CEclExceptionVector & errs) const
    {
        return CAttributeBase::PreProcess(action, overrideEcl, attrs, attrProcessed, errs);
    }

    boost::signals::connection on_refresh_connect(const refresh_slot_type& s)
    { 
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return on_refresh.connect(s); 
    }

    void Refresh(bool eclChanged, IAttribute * newAttrAsOldOneMoved, bool deleted)
    {
        if (deleted) 
            DeleteAttribute(this);

        on_refresh(this, eclChanged, newAttrAsOldOneMoved, deleted);
    }

    IAttributeHistory * GetAsHistory()	
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return this;
    }
    //const TCHAR *GetEcl()
    //{
    //	clib::recursive_mutex::scoped_lock proc(m_mutex);
    //	return GetEcl(false);
    //}
    const TCHAR *GetDescription() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return _T("");
    }
    IAttribute * GetAttribute() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        IAttribute * self = const_cast<CDiskAttribute *>(this);
        return self;
    }
    bool SetLabel(const std::_tstring & label)
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return false;
    }
    //  IDiskAttribute  ---
    const TCHAR * GetPath()
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_pathStr.c_str();
    }

    AttrInfo AttributeToInfo()
    {
        IAttribute * self = const_cast<CDiskAttribute *>(this);
        AttrInfo attrInfo;
        attrInfo.Attribute = self;
        attrInfo.AttributeType = self->GetType()->GetRepositoryCode();
        return attrInfo;
    }
};

//  ===========================================================================
static CacheT<std::_tstring, CDiskAttribute> DiskAttributeCache;
void ClearDiskAttributeCache()
{
    ATLTRACE(_T("File cache before clearing(size=%u)\r\n"), DiskAttributeCache.Size());
    DiskAttributeCache.Clear();
}

IAttribute * GetDiskAttribute(const IRepository *rep, const TCHAR* module, const TCHAR* label, IAttributeType * type, unsigned version, bool sandboxed)
{
    CComPtr<CDiskAttribute> attr = new CDiskAttribute(rep, module, label, type->GetRepositoryCode(), boost::filesystem::wpath(), version, sandboxed);
    IAttribute * retVal = DiskAttributeCache.Exists(attr->GetCacheID());
    if (retVal && !retVal->Exists())
    {
        DiskAttributeCache.Clear(attr->GetCacheID());
        return NULL;
    }
    return retVal;
}

void DeleteAttribute(CDiskAttribute * attr)
{
    DiskAttributeCache.Clear(attr->GetCacheID());
}

CDiskAttribute * CreateDiskAttributeRaw(const IRepository *rep, const TCHAR* module, const TCHAR* label, const TCHAR* type, const boost::filesystem::wpath & path, unsigned version, bool sandboxed)
{
    return DiskAttributeCache.Get(new CDiskAttribute(rep, module, label, type, path, version, sandboxed));
}

IAttribute * CreateDiskAttribute(const IRepository *rep, const TCHAR* module, const TCHAR* label, IAttributeType * type, const boost::filesystem::wpath & path)
{
    return CreateDiskAttributeRaw(rep, module, label, type->GetRepositoryCode(), path, 0, false);
}

IAttribute * CreateDiskAttributePlaceholder(const IRepository *rep, const TCHAR* module, const TCHAR* label, const TCHAR* type, const boost::filesystem::wpath & path)
{
    return CreateDiskAttributeRaw(rep, module, label, type, path, 0, false);
}

IAttribute * CreateDiskAttribute(const IRepository *rep, const std::_tstring &moduleLabel, const std::_tstring &label, const std::_tstring &type, const boost::filesystem::wpath & path, const std::_tstring & ecl, bool noBroadcast)
{
    if (moduleLabel.empty() || label.empty())
        return 0;

    CDiskAttribute * attr = CreateDiskAttributeRaw(rep, moduleLabel.c_str(), label.c_str(), type.c_str(), path, 0, false);
    ATLASSERT(attr);
    attr->Update(moduleLabel, label, ecl, noBroadcast);
    return attr;
}
