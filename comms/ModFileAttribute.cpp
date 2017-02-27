#include "StdAfx.h"
#include "attribute.h"
#include "module.h"
#include "ModFileRepository.h"
#include "SoapUtil.h"
#include "clib.h"
#include "cache.h"
#include "logger.h"
#include "util.h"
#include "UnicodeFile.h"
#include "md5.hpp"

#if _COMMS_VER < 68200
using namespace WsAttributes;
#else
#endif

//  ===========================================================================
class CModFileAttribute : public IAttribute, public IAttributeHistory, public clib::CLockableUnknown
{
private:
    static std::_tstring attrEmptyString;
protected:
    bool m_placeholder;
    CComPtr<IRepository> m_repository;
    CString m_id;
    mutable CComPtr<IModule> m_module;
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
    IMPLEMENT_CLOCKABLEUNKNOWN;

    CModFileAttribute(const IRepository *rep, const TCHAR* module, const TCHAR* label, const TCHAR * ecl, unsigned version, bool sandboxed, bool placeholder) 
        : m_moduleQualifiedLabel(module), m_label(label), m_version(version), m_sandboxed(sandboxed), m_placeholder(placeholder)
    {
        m_repository = const_cast<IRepository *>(rep);
        m_type = CreateIAttributeECLType();
        m_checkedOut = false;
        m_locked = false;
        m_orphaned = false;
        m_ecl = ecl;
        m_eclSet = true;
        m_qualifiedLabel = m_moduleQualifiedLabel + _T(".") + m_label;
        UpdateChecksumLocal();
        UpdateID();
    }

    virtual ~CModFileAttribute()
    {
    }

    void UpdateID()
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        m_id = m_repository->GetID();
        m_id = m_id + _T("/") + m_moduleQualifiedLabel + _T("/") + m_label + _T("/") + m_type->GetRepositoryCode() + (m_placeholder ? _T("/placeholder") : _T(""));
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
        return attrEmptyString.c_str();
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
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        ATLASSERT(ecl);

        m_eclSet = true;
        m_ecl = ecl;
        UpdateChecksumLocal();
        m_checksum = m_checksumLocal;

        proc.unlock();	//No need for lock here as this is just a short cut.
        IAttributeVector attrs;
        attrs.push_back(IAttributeAdapt((IAttribute*)this));
        return m_repository->Save(attrs, noBroadcast);
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
        //No lock needed - this is just a lazy way of calling into repository
        return m_repository->RenameAttribute(GetModuleQualifiedLabel(), GetLabel(), GetType(), label);
    }

    IAttribute * ChangeAttributeType(const TCHAR* ext)
    {
        //No lock needed - this is just a lazy way of calling into repository
        return m_repository->ChangeAttributeType(this, ext);
    }

    bool Delete()
    {
        //No lock needed - this is just a lazy way of calling into repository
        IAttributeVector attrs;
        attrs.push_back(this);
        return m_repository->DeleteAttributes(attrs);
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
            if (m_repository->InsertAttribute(GetModuleQualifiedLabel(), GetLabel(), m_type) != NULL)
            {
                return true;
            }
        }
        return false;
    }

#if _COMMS_VER < 68200
    void Update(const WsAttributes::ECLAttribute * c)
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        m_placeholder = false;
        m_moduleLabel = CW2T(c->ModuleName, CP_UTF8);
        m_label = CW2T(c->Name, CP_UTF8);
        m_qualifiedLabel = m_moduleLabel + _T(".") + m_label;
#if _COMMS_VER < 64801
#elif _COMMS_VER < 700000
        m_type = CreateIAttributeType((const TCHAR *)CW2T(c->Type, CP_UTF8));
#else
#endif
        m_checkedOut = c->IsCheckedOut;
        m_sandboxed = c->IsSandbox;
        m_locked = c->IsLocked;
        m_orphaned = c->IsOrphaned;
        bool eclChanged = false;
        if (SysStringLen(c->Text))
        {
            eclChanged = m_ecl.Compare(CW2T(c->Text, CP_UTF8)) != 0;
            m_ecl = CW2T(c->Text, CP_UTF8);
            m_eclSet = true;
            UpdateChecksumLocal();
        }
        m_description = CW2T(c->Description, CP_UTF8);
        m_lockedby = CW2T(c->LockedBy, CP_UTF8);
        m_version = c->Version;
        m_modifiedDate = CW2T(c->ModifiedDate, CP_UTF8);
        m_modifiedBy = CW2T(c->ModifiedBy, CP_UTF8);
        m_checksum = c->Checksum;
        UpdateID();
        proc.unlock();
        Refresh(eclChanged);
    }
#else
#endif

    int PreProcess(PREPROCESS_TYPE action, const TCHAR * overrideEcl, IAttributeVector & attrs, Dali::CEclExceptionVector & errs) const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return false;
    }

    boost::signals::connection on_refresh_connect(const refresh_slot_type& s)
    { 
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return on_refresh.connect(s); 
    }

    void Refresh(bool eclChanged, IAttribute * newAttrAsOldOneMoved, bool deleted)
    {
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
        IAttribute * self = const_cast<CModFileAttribute *>(this);
        return self;
    }
    bool SetLabel(const std::_tstring & label)
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return false;
    }
};
std::_tstring CModFileAttribute::attrEmptyString;
//  ===========================================================================
static CacheT<std::_tstring, CModFileAttribute> ModFileAttributeCache;

void ClearModFileAttributeCache()
{
    ATLTRACE(_T("File cache before clearing(size=%u)\r\n"), ModFileAttributeCache.Size());
    ModFileAttributeCache.Clear();
}

IAttribute * GetModFileAttribute(const IRepository *rep, const TCHAR* module, const TCHAR* label, const TCHAR* ecl, unsigned version, bool sandboxed, bool placeholder)
{
    CComPtr<CModFileAttribute> attr = new CModFileAttribute(rep, module, label, ecl, version, sandboxed, placeholder);
    return ModFileAttributeCache.Exists(attr->GetCacheID());
}

CModFileAttribute * CreateModFileAttributeRaw(const IRepository *rep, const TCHAR* module, const TCHAR* label, const TCHAR* ecl, unsigned version, bool sandboxed, bool placeholder)
{
    return ModFileAttributeCache.Get(new CModFileAttribute(rep, module, label, ecl, version, sandboxed, placeholder));
}

IAttribute * CreateModFileAttribute(const IRepository *rep, const TCHAR* module, const TCHAR* label, const std::_tstring &ecl, unsigned version, bool sandboxed)
{
    return CreateModFileAttributeRaw(rep, module, label, ecl.c_str(), version, sandboxed, false);
}

IAttribute * CreateModFileAttributePlaceholder(const IRepository *rep, const TCHAR* module, const TCHAR* label, const TCHAR* ecl)
{
    return CreateModFileAttributeRaw(rep, module, label, ecl, 0, false, true);
}

IAttribute * CreateModFileAttribute(const IRepository *rep, const std::_tstring &moduleLabel, const std::_tstring &label, const std::_tstring &ecl)
{
    if (moduleLabel.empty() || label.empty())
        return 0;

    CModFileAttribute * attr = CreateModFileAttributeRaw(rep, moduleLabel.c_str(), label.c_str(), ecl.c_str(), 0, false, false);
    ATLASSERT(attr);
    return attr;
}
