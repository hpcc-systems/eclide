#include "StdAfx.h"

#include "module.h"
#include "repository.h"
#include "clib.h"
#include "cache.h"
#include "SoapUtil.h"
#include "ModuleHelper.h"

#if _COMMS_VER < 68200
using namespace WsAttributes;
#elif _COMMS_VER < 700000
#else
using namespace WsRepository;
#endif

std::_tstring modEmptyString;

//  ===========================================================================
class CModule;
void DeleteModule(CModule * module);
class CModule : public IModule, public clib::CLockableUnknown
{
protected:
    bool m_placeholder;
    IRepository * m_repository;
    CComPtr<IModule> m_parent;
    CString m_label;
    CString m_labelLeaf;
    CString m_id;
    bool m_plugin;
    CString m_checksum;
    mutable CString m_fullChecksum;
    SecAccessFlags m_access;

    module_refresh_signal_type on_refresh;

public:
    IMPLEMENT_CLOCKABLEUNKNOWN;

    CModule(const IRepository *rep, const IModule * parent, const std::_tstring & label, const std::_tstring & labelLeaf, bool placeholder) : m_label(label.c_str()), m_labelLeaf(labelLeaf.c_str()), m_placeholder(placeholder)
    {
        m_repository = const_cast<IRepository *>(rep);
        m_parent = const_cast<IModule *>(parent);
        m_id = rep->GetID();
        m_id += _T("/") + m_label + (m_placeholder ? _T("/placeholder") : _T(""));
        m_plugin = false;
        m_access = SecAccess_Full;
        m_checksum = _T("unknown");
    }

    ~CModule()
    {
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

    const TCHAR *GetLabel() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_labelLeaf;
    }

    const TCHAR *GetQualifiedLabel(bool excludeRoot = false) const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        //  Remote repos never have root...
        return m_label;
    }

    const TCHAR *GetPath() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return modEmptyString.c_str();
    }

    const SecAccessFlags GetAccess() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_access;
    }

    IModule * GetRootModule() const
    {
        return NULL;
    }

    IModule * GetParentModule() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_parent;
    }

    unsigned GetModules(IModuleVector & modules, bool noRefresh=false) const
    {
        // No need for mutex as this is just a call through...
        return m_repository->GetModules(GetQualifiedLabel(), modules, noRefresh);
    }

    unsigned GetAttributes(IAttributeVector & attributes, bool noRefresh=false) const
    {
        // No need for mutex as this is just a call through...
        return m_repository->GetAttributes(GetQualifiedLabel(), attributes, noRefresh);
    }

    IAttribute * GetAttribute(const TCHAR* attribute, IAttributeType * type) const
    {
        // No need for mutex as this is just a call through...
        return m_repository->GetAttribute(GetQualifiedLabel(), attribute, type);
    }

    IAttribute * GetAttributePlaceholder(const TCHAR* attribute, IAttributeType * type) const
    {
        // No need for mutex as this is just a call through...
        return m_repository->GetAttributePlaceholder(GetQualifiedLabel(), attribute, type);
    }

    IAttribute * InsertAttribute(const TCHAR* attribute, IAttributeType * type) const
    {
        //  No need for mutex as it should really be a static method.
        return m_repository->InsertAttribute(GetQualifiedLabel(), attribute, type);
    }

    bool IsPlugin() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_plugin && IsTopFolder();
    }

    bool IsTopFolder() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_parent == NULL;
    }

    bool IsTrash() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return boost::algorithm::iequals(GetQualifiedLabel(), TRASH);
    }

    const TCHAR *GetChecksum() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        if (m_fullChecksum.GetLength() == 0)
        {
            m_fullChecksum = m_checksum;
            IModuleVector children;
            GetModules(children, true);
            for(IModuleVector::iterator itr = children.begin(); itr != children.end(); ++itr)
                m_fullChecksum += itr->get()->GetChecksum();
        }
        return m_fullChecksum;
    }

    IModule * Rename(const TCHAR* label)
    {
        IModuleVector children;
        GetModules(children);
        for(IModuleVector::iterator itr = children.begin(); itr != children.end(); ++itr)
        {
            std::_tstring qualifiedLabel = label;
            qualifiedLabel += _T(".");
            qualifiedLabel += itr->get()->GetLabel();
            itr->get()->Rename(qualifiedLabel.c_str());
        }
        return m_repository->RenameModule(GetQualifiedLabel(), label);
    }
    bool Delete()
    {
        IModuleVector children;
        GetModules(children);
        for(IModuleVector::iterator itr = children.begin(); itr != children.end(); ++itr)
            itr->get()->Delete();

        IAttributeVector childAttrs;
        GetAttributes(childAttrs, true);
        for(IAttributeVector::iterator itr = childAttrs.begin(); itr != childAttrs.end(); ++itr)
            itr->get()->Refresh(false, NULL, true);

        if (m_repository->DeleteModule(GetQualifiedLabel()))
        {
            DeleteModule(this);
            return true;
        }
        return false;
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
            if (m_parent)
                m_parent->Create();
            m_placeholder = false;
            proc.unlock();
            if (m_repository->InsertModule(GetQualifiedLabel()) != NULL)
                return true;
        }
        return false;
    }

    bool HasChildren() const
    {
        return true;
    }

    IRepository * GetRepository() const
    {
        return m_repository;
    }

#if _COMMS_VER < 68200
    void Update(const ECLModule * c)
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        m_placeholder = false;
        m_label = CW2T(c->Name, CP_UTF8);
#if _COMMS_VER < 700000
        m_access = static_cast<SecAccessFlags>(c->Access);
#else
        ATLASSERT(!_T("Todo"));
        //m_access = static_cast<SecAccessFlags>(c->Access);
#endif
#if _COMMS_VER < 491
#else
        m_plugin = c->IsPlugin;
#endif
#if _COMMS_VER < 58800
#else
        if (c->Checksum)
        {
            m_checksum = CW2T(c->Checksum, CP_UTF8);
        }
#endif
        proc.unlock();
        Refresh();
    }
#else
    void Update(const ns2__ECLModule * c, bool noBroadcast = false)
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        m_placeholder = false;
        SAFE_ASSIGN2CSTRING(m_label, c->Name);
        if (c->Access)
            m_access = static_cast<SecAccessFlags>(*c->Access);
        SAFE_ASSIGN(m_plugin, c->IsPlugin);
        SAFE_ASSIGN2CSTRINGIFNOTBLANK(m_checksum, c->Checksum);

        if (!noBroadcast)
        {
            proc.unlock();
            Refresh();
        }
    }
#endif
    boost::signals::connection on_refresh_connect(const module_refresh_slot_type& s)
    { 
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return on_refresh.connect(s); 
    }

    void Refresh(REFRESH_MODULE refreshType = REFRESH_MODULE_UNKNOWN)
    {
        on_refresh(this, refreshType);
    }
};

//  ===========================================================================
static CacheT<std::_tstring, CModule> ModuleCache;

void ClearModuleCache()
{
    ATLTRACE(_T("Module cache before clearing(size=%u)\r\n"), ModuleCache.Size());
    ModuleCache.Clear();
}

void DeleteModule(CModule * module)
{
    ModuleCache.Clear(module->GetCacheID());
}

CModule * CreateModuleRaw(const IRepository *rep, const std::_tstring & label, bool placeholder)
{
    CModuleHelper modHelper(label);
    StdStringVector tokens;
    modHelper.GetQualifiedLabel(tokens);

    CModule * retVal = NULL;
    std::_tstring qualifiedLabel;
    for (StdStringVector::iterator itr = tokens.begin(); itr != tokens.end(); ++itr)
    {
        if (!qualifiedLabel.empty())
            qualifiedLabel += CModuleHelper::DELIMTER;
        qualifiedLabel += *itr;
        retVal = ModuleCache.Get(new CModule(rep, retVal, qualifiedLabel.c_str(), *itr, placeholder));
    }
    return retVal;
}

#if _COMMS_VER < 68200
IModule * CreateModule(const IRepository *rep, const std::_tstring &label, const ECLModule * data)
{
    CModule * mod = CreateModuleRaw(rep, label.c_str(), false);
    ATLASSERT(mod);
    mod->Update(data);
    return mod;
}
#else
IModule * CreateModule(const IRepository *rep, const ns2__ECLModule * data, bool noBroadcast)
{
    ATLASSERT(data->Name);
    ATLASSERT(!data->Name->empty());
    CModule * mod = CreateModuleRaw(rep, data->Name->c_str(), false);
    ATLASSERT(mod);
    mod->Update(data, noBroadcast);
    return mod;
}
#endif
IModule * CreateModulePlaceholder(const IRepository *rep, const std::_tstring &label)
{
    CModule * mod = CreateModuleRaw(rep, label.c_str(), true);
    ATLASSERT(mod);
    return mod;
}
//  ===========================================================================
IModule * Find(const IModuleVector * modules, const std::_tstring & module)
{
    for(IModuleVector::const_iterator itr = modules->begin(); itr != modules->end(); ++itr)
    {
        if (_tcsicmp(module.c_str(), itr->get()->GetQualifiedLabel()) == 0)
        {
            return itr->get();
        }
    }
    return NULL;
}

