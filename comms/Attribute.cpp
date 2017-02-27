#include "StdAfx.h"

#include "AttributeImpl.h"
#include "module.h"
#include "SoapUtil.h"
#include "clib.h"
#include "cache.h"
#include "logger.h"
#include "util.h"
#include "md5.hpp"
#include "Migration.h"

#if _COMMS_VER < 68200
using namespace WsAttributes;
#elif _COMMS_VER < 700000
#else
using namespace WsRepository;
#endif

IAttributeHistory * CreateDiskAttributeHistory(const boost::filesystem::path & path, const IAttribute * attribute, int version);

COMMS_API const TCHAR *GetAttrStateLabel(int nIndex)
{
    return ATTRSTATELABEL[nIndex];
}
//  ===========================================================================
class CAttribute;
void DeleteAttribute(CAttribute * attr);
class CAttribute : public CAttributeBase, public IAttribute, public IAttributeHistory
{
private:
    static std::_tstring attrEmptyString;

protected:
    bool m_placeholder;

    CString m_id;
    mutable CComPtr<IModule> m_module;
    CString m_moduleQualifiedLabel;
    CString m_label;
    CString m_qualifiedLabel;
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
    SecAccessFlags m_access;

    refresh_signal_type on_refresh;

public:
    BEGIN_CLOCKABLEUNKNOWN
        IMPLEMENT_INTERFACE(IAttribute)
        IMPLEMENT_INTERFACE(IAttributeHistory)
    END_CUNKNOWN(clib::CLockableUnknown)

    CAttribute(const IRepository *rep, const TCHAR* module, const TCHAR* label, IAttributeType * type, unsigned version, bool sandboxed, bool placeholder) 
        : m_moduleQualifiedLabel(module), m_label(label), m_type(type), m_version(version), m_sandboxed(sandboxed), m_placeholder(placeholder)
    {
        m_repository = const_cast<IRepository *>(rep);
        m_checkedOut = false;
        m_locked = false;
        m_orphaned = false;
        m_eclSet = false;
        m_access = SecAccess_Full;
        UpdateID();
    }

    virtual ~CAttribute()
    {
    }

    void UpdateID()
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        m_id = m_repository->GetID();
        m_id = m_id + _T("/") + m_moduleQualifiedLabel + _T("/") + m_label + _T("/") + m_type->GetRepositoryCode();
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
        //  Remote repos never have root...
        return m_moduleQualifiedLabel;
    }

    const TCHAR *GetLabel() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_label;
    }

    const TCHAR *GetQualifiedLabel(bool excludeRoot = false) const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_qualifiedLabel;
    }

    const TCHAR *GetPath() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return attrEmptyString.c_str();
    }

    const SecAccessFlags GetAccess() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_access;
    }

    IAttributeType *GetType() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_type;
    }

    const TCHAR *GetText(bool refresh, bool noBroadcast = false) const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        if (!m_placeholder && (refresh || !m_eclSet))
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
        m_ecl = ecl;
        //TidyCRLF(m_ecl);
        m_eclSet = true;
        UpdateChecksumLocal();

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
        //No lock needed - this is just a lazy way of calling into repository
        boost::filesystem::path path;
        GetIConfig(QUERYBUILDER_CFG)->GetEnvironmentFolder(path);
        path /= stringToPath(GetModuleQualifiedLabel());
        path /= stringToPath(GetLabel());
        if (exists(path))
        {
            int version = 0;
            boost::filesystem::directory_iterator end_itr; // default construction yields past-the-end
            for (boost::filesystem::directory_iterator itr(path); itr != end_itr; ++itr)
            {
                if (boost::algorithm::iequals(boost::filesystem::extension(itr->path()).c_str(), ".ecl"))
                {
                    attributes.push_back(CreateDiskAttributeHistory(itr->path(), this, --version));
                }
            }
        }

        unsigned int retVal = m_repository->GetAttributeHistory(GetModuleQualifiedLabel(), GetLabel(), m_type, attributes);
        IAttributeHistoryCompare compare;
        std::sort(attributes.rbegin(), attributes.rend(), compare);
        return retVal;
    }

    bool Checkout()
    {
        //No lock needed - this is just a lazy way of calling into repository
        IAttributeVector attrs;
        attrs.push_back(this);
        return m_repository->Checkout(attrs);
    }

    bool Checkin(const TCHAR* comments)
    {
        //No lock needed - this is just a lazy way of calling into repository
        IAttributeVector attrs;
        attrs.push_back(this);
        return m_repository->Checkin(attrs, comments, false);
    }

    IAttribute * Rename(const TCHAR* label)
    {
        //No lock needed - this is just a lazy way of calling into repository
        return m_repository->RenameAttribute(this, label);
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
            m_repository->InsertAttribute(GetModuleQualifiedLabel(), GetLabel(), m_type);
            return true;
        }
        return false;
    }

#if _COMMS_VER < 68200
    void Update(const ECLAttribute * c, bool noBroadcast)
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        m_placeholder = false;
        m_moduleQualifiedLabel = CW2T(c->ModuleName, CP_UTF8);
        m_label = CW2T(c->Name, CP_UTF8);
        m_qualifiedLabel = m_moduleQualifiedLabel + _T(".") + m_label;
#if _COMMS_VER < 64801
#elif _COMMS_VER < 700000
        if (c->Type)
        {
            ATLASSERT(m_type == CreateIAttributeType((const TCHAR *)CW2T(c->Type ? c->Type : L"", CP_UTF8)));
        }
#else
#endif
        m_checkedOut = c->IsCheckedOut;
        m_sandboxed = c->IsSandbox;
        m_locked = c->IsLocked;
        m_orphaned = c->IsOrphaned;
        bool eclChanged = false;
        if (c->Text)//SysStringLen(c->Text))
        {
            eclChanged = m_ecl.Compare(CW2T(c->Text, CP_UTF8)) != 0;
            m_ecl = CW2T(c->Text, CP_UTF8);
            m_eclSet = true;
            UpdateChecksumLocal();
        }
        m_description = CW2T(c->Description, CP_UTF8);
        m_lockedby = CW2T(c->LockedBy, CP_UTF8);
        m_version = c->Version;
#if _COMMS_VER < 700000
        m_modifiedDate = CW2T(c->ModifiedDate, CP_UTF8);
#else
        ATLASSERT(!_T("TODO"));
        //m_modifiedDate = CW2T(c->ModifiedDate, CP_UTF8);
#endif
        m_modifiedBy = CW2T(c->ModifiedBy, CP_UTF8);
        m_checksum = c->Checksum;
        UpdateID();
        if (!noBroadcast)
        {
            proc.unlock();
            Refresh(eclChanged);
        }
    }
#else
    void Update(const ns2__ECLAttribute * c, bool noBroadcast)
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        m_placeholder = false;
        SAFE_ASSIGN2CSTRING(m_moduleQualifiedLabel, c->ModuleName);
        SAFE_ASSIGN2CSTRING(m_label, c->Name);
        m_qualifiedLabel = m_moduleQualifiedLabel + _T(".") + m_label;
        if (c->Access)
            m_access = static_cast<SecAccessFlags>(*c->Access);
        if (c->Type)
        {
            ATLASSERT(m_type == CreateIAttributeType(*c->Type));
        }
        SAFE_ASSIGN(m_checkedOut, c->IsCheckedOut);
        SAFE_ASSIGN(m_sandboxed, c->IsSandbox);
        SAFE_ASSIGN(m_locked, c->IsLocked);
        SAFE_ASSIGN(m_orphaned, c->IsOrphaned);
        bool eclChanged = false;
        if (c->Text)//SysStringLen(c->Text))
        {
            eclChanged = m_ecl.Compare(c->Text->c_str()) != 0;
            SAFE_ASSIGN2CSTRING(m_ecl, c->Text);
            m_eclSet = true;
            UpdateChecksumLocal();
        }
        SAFE_ASSIGN2CSTRING(m_description, c->Description);
        SAFE_ASSIGN2CSTRING(m_lockedby, c->LockedBy);
        if (!IsLocked())
            m_lockedby = _T("");
        SAFE_ASSIGN(m_version, c->Version);
        SAFE_ASSIGN2CSTRING(m_modifiedDate, c->ModifiedDate);
        SAFE_ASSIGN2CSTRING(m_modifiedBy, c->ModifiedBy);
        SAFE_ASSIGN2CSTRING(m_checksum, c->Checksum);
        UpdateID();
        if (!noBroadcast)
        {
            proc.unlock();
            Refresh(eclChanged, NULL, false);
        }
    }
#endif

#if _COMMS_VER < 700000
#else
    void Update(const SaveAttributeRequest * data)
    {
        ATLASSERT(!_T("TODO"));
    }

    void Update(const CheckoutAttributeRequest * data)
    {
        ATLASSERT(!_T("TODO"));
    }

    void Update(const CheckinAttributeRequest * data)
    {
        ATLASSERT(!_T("TODO"));
    }

    void Update(const RollbackAttributeRequest * data)
    {
        ATLASSERT(!_T("TODO"));
    }

    void Update(const MoveAttributeRequest * data)
    {
        ATLASSERT(!_T("TODO"));
    }
#endif

    int PreProcess(PREPROCESS_TYPE action, const TCHAR * overrideEcl, IAttributeVector & attrs, Dali::CEclExceptionVector & errs) const
    {
        return CAttributeBase::PreProcess(action, overrideEcl, attrs, errs);
    }

    boost::signals::connection on_refresh_connect(const refresh_slot_type& s)
    { 
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return on_refresh.connect(s); 
    }

    void Refresh(bool eclChanged, IAttribute * newAttrAsOldOneMoved, bool deleted)
    {
        if (deleted || newAttrAsOldOneMoved) 
            DeleteAttribute(this);

        on_refresh(this, eclChanged, newAttrAsOldOneMoved, deleted);
    }

    //  IAttributeHistory interface  ---
    IAttributeHistory * GetAsHistory()	
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return this;
    }
    //const TCHAR *GetEcl()
    //{
    //	clib::recursive_mutex::scoped_lock proc(m_mutex);
    //	return GetText(false);
    //}
    const TCHAR *GetDescription() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return _T("");
    }
    IAttribute * GetAttribute() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        IAttribute * self = const_cast<CAttribute *>(this);
        return self;
    }
    bool SetLabel(const std::_tstring & label)
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return false;
    }
};
std::_tstring CAttribute::attrEmptyString;
//  ===========================================================================
static CacheT<std::_tstring, CAttribute> AttributeCache;

void ClearAttributeCache()
{
    ATLTRACE(_T("File cache before clearing(size=%u)\r\n"), AttributeCache.Size());
    AttributeCache.Clear();
}

IAttribute * GetAttribute(const IRepository *rep, const TCHAR* module, const TCHAR* label, IAttributeType * type, unsigned version, bool sandboxed, bool placeholder)
{
    CComPtr<CAttribute> attr = new CAttribute(rep, module, label, type, version, sandboxed, placeholder);
    return AttributeCache.Exists(attr->GetCacheID());
}

void DeleteAttribute(CAttribute * attr)
{
    AttributeCache.Clear(attr->GetCacheID());
}

CAttribute * CreateAttributeRaw(const IRepository *rep, const TCHAR* module, const TCHAR* label, IAttributeType * type, unsigned version, bool sandboxed, bool placeholder)
{
    return AttributeCache.Get(new CAttribute(rep, module, label, type, version, sandboxed, placeholder));
}

//IAttribute * CreateAttribute(const IRepository *rep, const TCHAR* module, const TCHAR* label, unsigned version, bool sandboxed)
//{
//	return CreateAttributeRaw(rep, module, label, version, sandboxed, false);
//}

IAttribute * CreateAttributePlaceholder(const IRepository *rep, const std::_tstring &moduleLabel, const std::_tstring &label, IAttributeType * type)
{
    return CreateAttributeRaw(rep, moduleLabel.c_str(), label.c_str(), type, 0, false, true);
}

#if _COMMS_VER < 68200
IAttribute * CreateAttribute(const IRepository *rep, const std::_tstring &moduleLabel, const std::_tstring &label, IAttributeType * type, const ECLAttribute * data, bool noBroadcast)
{
    if (moduleLabel.empty() || label.empty())
        return NULL;

    CAttribute * attr = CreateAttributeRaw(rep, moduleLabel.c_str(), label.c_str(), type, data->Version, data->IsSandbox, false);
    ATLASSERT(attr);
    attr->Update(data, noBroadcast);
    return attr;
}
#else
IAttribute * CreateAttribute(const IRepository *rep, const ns2__ECLAttribute * data, bool noBroadcast=false)
{
    std::_tstring moduleName;
    SAFE_ASSIGN(moduleName, data->ModuleName);
    if (moduleName.empty())
        return NULL;
    std::_tstring name;
    SAFE_ASSIGN(name, data->Name);
    if (name.empty())
        return NULL;

    std::_tstring type;
    SAFE_ASSIGN(type, data->Type);
    unsigned int version = 0;
    SAFE_ASSIGN(version, data->Version);
    bool isSandbox = false;
    SAFE_ASSIGN(isSandbox, data->IsSandbox);
    CAttribute * attr = CreateAttributeRaw(rep, moduleName.c_str(), name.c_str(), CreateIAttributeType(type), version, isSandbox, false);
    ATLASSERT(attr);
    attr->Update(data, noBroadcast);
    return attr;
}
#endif

#if _COMMS_VER < 700000
#else
IAttribute * CreateAttribute(const IRepository *rep, const std::_tstring &moduleLabel, const std::_tstring &label, const SaveAttributeRequest * data)
{
    if (moduleLabel.empty() || label.empty())
        return NULL;

    CAttribute * attr = CreateAttributeRaw(rep, moduleLabel.c_str(), label.c_str(), 0, false, false);
    ATLASSERT(attr);
    attr->Update(data);
    return attr;
}
IAttribute * CreateAttribute(const IRepository *rep, const std::_tstring &moduleLabel, const std::_tstring &label, const CheckoutAttributeRequest * data)
{
    if (moduleLabel.empty() || label.empty())
        return NULL;

    CAttribute * attr = CreateAttributeRaw(rep, moduleLabel.c_str(), label.c_str(), 0, false, false);
    ATLASSERT(attr);
    attr->Update(data);
    return attr;
}
IAttribute * CreateAttribute(const IRepository *rep, const std::_tstring &moduleLabel, const std::_tstring &label, const CheckinAttributeRequest * data)
{
    if (moduleLabel.empty() || label.empty())
        return NULL;

    CAttribute * attr = CreateAttributeRaw(rep, moduleLabel.c_str(), label.c_str(), 0, false, false);
    ATLASSERT(attr);
    attr->Update(data);
    return attr;
}
IAttribute * CreateAttribute(const IRepository *rep, const std::_tstring &moduleLabel, const std::_tstring &label, const RollbackAttributeRequest * data)
{
    if (moduleLabel.empty() || label.empty())
        return NULL;

    CAttribute * attr = CreateAttributeRaw(rep, moduleLabel.c_str(), label.c_str(), 0, false, false);
    ATLASSERT(attr);
    attr->Update(data);
    return attr;
}
IAttribute * CreateAttribute(const IRepository *rep, const std::_tstring &moduleLabel, const std::_tstring &label, const MoveAttributeRequest * data)
{
    if (moduleLabel.empty() || label.empty())
        return NULL;

    CAttribute * attr = CreateAttributeRaw(rep, moduleLabel.c_str(), label.c_str(), 0, false, false);
    ATLASSERT(attr);
    attr->Update(data);
    return attr;
}
#endif
