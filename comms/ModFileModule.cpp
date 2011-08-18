#include "StdAfx.h"

#include "module.h"
#include "ModFileRepository.h"
#include "clib.h"
#include "cache.h"
#include "ModuleHelper.h"

#if _COMMS_VER < 68200
using namespace WsAttributes;
#else
#endif

std::_tstring modFileEmptyString;

class CModFileModule : public IModule, public clib::CLockableUnknown
{
protected:
	bool m_placeholder;
	CComPtr<IRepository> m_repository;
	CComPtr<IModule> m_parent;
	CString m_url;
	CString m_label;
	CString m_labelLeaf;
	CString m_id;
	bool m_plugin;
	CString m_checksum;
	SecAccessFlags m_access;

	module_refresh_signal_type on_refresh;

public:
	IMPLEMENT_CLOCKABLEUNKNOWN;

	CModFileModule(const IRepository *rep, const IModule * parent, const std::_tstring & label, const std::_tstring & labelLeaf, bool placeholder) : m_label(label.c_str()), m_labelLeaf(labelLeaf.c_str()), m_placeholder(placeholder)
	{
		m_repository = const_cast<IRepository *>(rep);
		m_parent = const_cast<IModule *>(parent);
		m_url = rep->GetUrl();
		m_id = rep->GetID();
		m_id += _T("/") + m_label + (m_placeholder ? _T("/placeholder") : _T(""));
		m_plugin = false;
		m_access = SecAccess_Full;
	}

	~CModFileModule()
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

	const TCHAR *GetQualifiedLabel() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_label;
	}

	const TCHAR *GetPath() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return modFileEmptyString.c_str();
	}

	const SecAccessFlags GetAccess() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_access;
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
		return m_plugin;
	}

	bool IsTrash() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return boost::algorithm::iequals(GetQualifiedLabel(), TRASH);
	}

	const TCHAR *GetChecksum() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_checksum;
	}

	IModule * Rename(const TCHAR* label)
	{
		return m_repository->RenameModule(GetQualifiedLabel(), label);
	}

	bool Delete()
	{
		return m_repository->DeleteModule(GetQualifiedLabel());
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
			if (m_repository->InsertModule(GetQualifiedLabel()) != NULL)
			{
				return true;
			}
		}
		return false;
	}

	bool HasChildren() const
	{
		return true;
	}

	IRepository * GetRepository() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_repository;
	}

	void Update(bool noBroadcast = false)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		if (!noBroadcast)
		{
			proc.unlock();
			Refresh();
		}
	}

	boost::signals::connection on_refresh_connect(const module_refresh_slot_type& s)
    { 
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return on_refresh.connect(s); 
	}

	void Refresh()
	{
		on_refresh(this);
	}
};

static CacheT<std::_tstring, CModFileModule> ModFileModuleCache;

void ClearModFileModuleCache()
{
	ATLTRACE(_T("Module cache before clearing(size=%u)\r\n"), ModFileModuleCache.Size());
	ModFileModuleCache.Clear();
}

CModFileModule * CreateModFileModuleRaw(const IRepository *rep, const std::_tstring & label, bool placeholder)
{
	CModuleHelper modHelper(label);
	StdStringVector tokens;
	modHelper.GetQualifiedLabel(tokens);

	CModFileModule * retVal = NULL;
	std::_tstring qualifiedLabel;
	for (StdStringVector::iterator itr = tokens.begin(); itr != tokens.end(); ++itr)
	{
		if (!qualifiedLabel.empty())
			qualifiedLabel += CModuleHelper::DELIMTER;
		qualifiedLabel += *itr;
		retVal = ModFileModuleCache.Get(new CModFileModule(rep, retVal, qualifiedLabel.c_str(), *itr, placeholder));
	}
	return retVal;
}

IModule * CreateModFileModule(const IRepository *rep, const std::_tstring &label, bool noBroadcast = false)
{
	CModFileModule * mod = CreateModFileModuleRaw(rep, label.c_str(), false);
	ATLASSERT(mod);
	mod->Update(noBroadcast);
	return mod;
}

IModule * CreateModFileModulePlaceholder(const IRepository *rep, const std::_tstring &label)
{
	CModFileModule * mod = CreateModFileModuleRaw(rep, label.c_str(), true);
	return mod;
}

