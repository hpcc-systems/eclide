#pragma once

#include "comms.h"
#include "attribute.h"

static const TCHAR * const TRASH = _T("Trash");

__interface IModule;
class IModuleCompare;
typedef StlLinked<IModule> IModuleAdapt;
typedef std::vector<IModuleAdapt> IModuleVector;
typedef std::set<IModuleAdapt, IModuleCompare> IModuleSet;

typedef boost::signal<void(IModule *)> module_refresh_signal_type;
typedef module_refresh_signal_type::slot_type module_refresh_slot_type;

__interface IModule : public clib::ILockableUnknown
{
	const TCHAR *GetID() const;
	const TCHAR *GetLabel() const;
	const TCHAR *GetQualifiedLabel(bool excludeRoot = false) const;
	const TCHAR *GetPath() const;
	const SecAccessFlags GetAccess() const;
	IModule * GetParentModule() const;
	unsigned GetModules(IModuleVector & modules, bool noRefresh=false) const;
	unsigned GetAttributes(IAttributeVector & attributes, bool noRefresh=false) const;
	bool IsPlugin() const;
	bool IsTrash() const;
	const TCHAR *GetChecksum() const;
	IModule * Rename(const TCHAR* label);
	bool Delete();
	bool Exists() const;
	bool Create();
	bool HasChildren() const;

	IAttribute * GetAttribute(const TCHAR* attribute, IAttributeType * type) const;
	IAttribute * GetAttributePlaceholder(const TCHAR* attribute, IAttributeType * type) const;
	IAttribute * InsertAttribute(const TCHAR* attribute, IAttributeType * type) const;
	IRepository * GetRepository() const;

	boost::signals::connection on_refresh_connect(const module_refresh_slot_type& s);
};

class IModuleCompare
{
public:
	bool operator ()(IModuleAdapt & l, IModuleAdapt & r)
	{
		if (l->IsTrash() != r->IsTrash())
		{
			return l->IsTrash() < r->IsTrash();
		}

		if (l->IsPlugin() != r->IsPlugin())
		{
			return l->IsPlugin() < r->IsPlugin();
		}

		return _tcsicmp(l->GetQualifiedLabel(), r->GetQualifiedLabel()) < 0;
	}
};

COMMS_API IModule * Find(const IModuleVector * modules, const std::_tstring & module);

