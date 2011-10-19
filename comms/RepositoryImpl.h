#include "StdAfx.h"
#include "repository.h"

#include "Module.h"
#include "clib.h"
#include "cache.h"
#include "logger.h"
#include "global.h"
#include "util.h"
#include "utilDateTime.h"
#include "thread.h"
#include "ModuleHelper.h"
#include "DiskRepository.h"

class ILabelCompare
{
public:
	bool operator ()(StlLinked<CRepLabel> & l, StlLinked<CRepLabel> & r)
	{
		return l->m_time < r->m_time;
	}
};

typedef std::map<std::_tstring, std::pair<std::_tstring, std::_tstring> > PasswordCacheT;
extern PasswordCacheT g_passwordCache;
IAttribute * CreateAttributePlaceholder(const IRepository *rep, const std::_tstring &moduleLabel, const std::_tstring &label, IAttributeType * type);

IAttribute * GetAttribute(const IRepository *rep, const TCHAR* module, const TCHAR* label, IAttributeType * type, unsigned version = 0, bool sandboxed = true, bool placeholder = false);

#define ESP_EXCEPTION_LOG3(T) CReportingGSoapEspException<ns2__ArrayOfEspException> exceptions(T, __FILE__, __LINE__)

typedef std::map<std::_tstring, IModuleVector> IModuleHierarchy;
class CRepositoryBase : public IRepository, public CUnknown
{
protected:
	mutable CachePool<IModuleVector> m_cacheGetModules;
	mutable CachePool<IModuleHierarchy> m_cacheGetModulesHierarchy;
	mutable CachePoolMap<IAttributeVector> m_cacheGetAttributes;
	mutable CachePool<CRepLabelVector> m_cacheGetLabels;

	std::_tstring m_url;
	std::_tstring m_label;
	std::_tstring m_instance;
	std::_tstring m_id;
	std::_tstring m_repositoryLabel;

public:
	IMPLEMENT_CUNKNOWN;

	CRepositoryBase(const TCHAR* url, const TCHAR* userId, const TCHAR* password, const TCHAR* label, const TCHAR* instance) : m_label(label), m_url(url), m_instance(instance), 
		 m_cacheGetModules(60 * 5), m_cacheGetAttributes(55)
	{
		UpdateID();
		if (g_passwordCache.find(m_url) == g_passwordCache.end())
		{
			g_passwordCache[m_url].first = userId;
			g_passwordCache[m_url].second = password;
		}
	}

	~CRepositoryBase()
	{
	}

	void UpdateID()
	{
		m_id = m_url + _T("/") + m_label + _T("/") + m_repositoryLabel + _T("/") + m_instance;
		boost::algorithm::trim_right_if(m_id, boost::algorithm::is_any_of(_T("/")));
	}

	void SetRepositoryLabel(const std::_tstring & label)
	{
		//clib::recursive_mutex::scoped_lock proc(m_mutex);
		m_repositoryLabel = label;
	}

	const TCHAR * GetRepositoryLabel() const
	{
		//clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_repositoryLabel.c_str();
	}

	const TCHAR * GetID() const
	{
		//clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_id.c_str();
	}

	const TCHAR * GetCacheID() const
	{
		//clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_id.c_str();
	}

	const TCHAR * GetLabel() const
	{
		//clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_label.c_str();
	}

	const TCHAR * GetUrl() const
	{
		//clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_url.c_str();
	}

	const TCHAR * GetUserId() const
	{
		//clib::recursive_mutex::scoped_lock proc(m_mutex);
		return g_passwordCache[m_url].first.c_str();
	}

	const TCHAR * GetPassword() const
	{
		//clib::recursive_mutex::scoped_lock proc(m_mutex);
		return g_passwordCache[m_url].second.c_str();
	}

	bool GetModule(CComPtr<IModule> & /*modulePtr*/) const
	{
		//clib::recursive_mutex::scoped_lock proc(m_mutex);
		return false;
	}

	IModule * GetModule(const TCHAR* label, bool GetChecksum = false, bool noBroadcast = false) const
	{
		//clib::recursive_mutex::scoped_lock proc(m_mutex);
		StlLinked<IModule> retVal;
		IModuleVector modules;
		GetAllModules(modules, GetChecksum, true, noBroadcast);
		for(IModuleVector::iterator itr = modules.begin(); itr != modules.end(); ++itr)
		{
			if (_tcsicmp(label, itr->get()->GetQualifiedLabel()) == 0)
			{
				retVal = itr->get();
				break;
			}
		}
		return retVal.get();
	}

	virtual IModule * GetModulePlaceholder(const TCHAR* label) const
	{
		ATLASSERT(false);
		return NULL;
	}

	unsigned GetModules(const std::_tstring & module, IModuleVector & modules, bool GetChecksum = false, bool noRefresh=true) const
	{
		IModuleVector allModules;
		IModuleHierarchy moduleHierarchy;
		GetAllModules(allModules, moduleHierarchy, GetChecksum, noRefresh);
		modules = moduleHierarchy[module];
		//for(IModuleVector::iterator itr = allModules.begin(); itr != allModules.end(); ++itr)
		//{
		//	IModule * parent = itr->get()->GetParentModule();
		//	if ((!parent && module.empty()) || (parent && boost::algorithm::iequals(module, parent->GetQualifiedLabel())))
		//		modules.push_back(itr->get());
		//}
		return 0;
	}

	unsigned GetAllModules(IModuleVector & modules, bool GetChecksum = false, bool noRefresh = true, bool noBroadcast = false) const
	{
		IModuleHierarchy moduleHierarchy;
		return GetAllModules(modules, moduleHierarchy, GetChecksum, noRefresh, noBroadcast);
	}

	virtual unsigned GetAllModules(IModuleVector & modules, IModuleHierarchy & moduleHierarchy, bool GetChecksum = false, bool noRefresh = true, bool noBroadcast = false) const
	{
		ATLASSERT(false);
		return modules.size();
	}

	virtual unsigned GetModulesAutoC(const std::_tstring & module, StdStringVector &result) const
	{
		//clib::recursive_mutex::scoped_lock proc(m_mutex);
		IModuleVector modules;
		GetModules(module, modules);
		for(IModuleVector::const_iterator itr = modules.begin(); itr != modules.end(); ++itr)
		{
			result.push_back(itr->get()->GetLabel());
		}
		return result.size();
	}

	virtual IModule * InsertModule(const TCHAR* moduleName) const
	{
		ATLASSERT(false);
		return NULL;
	}

	const TCHAR * TypesToSingleString(const IAttributeTypeVector & types, std::_tstring & result) const
	{
		for(IAttributeTypeVector::const_iterator itr = types.begin(); itr != types.end(); ++itr)
		{
			if (result.size())
				result += _T(";");
			result += itr->get()->GetRepositoryCode();
		}
		return result.c_str();
	}

	virtual unsigned GetAttributes(const TCHAR* moduleName, const IAttributeTypeVector & types, IAttributeVector & attributes, bool noRefresh=false, bool noBroadcast=false) const
	{
		ATLASSERT(false);
		return attributes.size();
	}

	unsigned GetAttributes(const TCHAR* module, IAttributeVector & attributes, bool noRefresh=false, bool noBroadcast=false) const
	{
		//clib::recursive_mutex::scoped_lock proc(m_mutex);
		IAttributeTypeVector allTypes;
		GetAttributeTypes(allTypes);
		return GetAttributes(module, allTypes, attributes, noRefresh, noBroadcast);
	}

	virtual unsigned int GetAttributeTypes(IAttributeTypeVector & types) const
	{
		ATLASSERT(false);
		return types.size();
	}

	void LoadStringTypes(const IAttributeTypeVector & types, StdStringVector & strTypes) const
	{
		for(IAttributeTypeVector::const_iterator itr = types.begin(); itr != types.end(); ++itr)
			strTypes.push_back(itr->get()->GetRepositoryCode());
	}

	void LoadStringTypes(StdStringVector & strTypes) const
	{
		IAttributeTypeVector types;
		GetAttributeTypes(types);
		LoadStringTypes(types, strTypes);
	}

	virtual unsigned FindAttributes(const std::_tstring & searchText, const std::_tstring & module, const std::_tstring & user, bool regExp, bool sandboxed, bool checkedout, bool locked, bool orphaned, const TCHAR* utcDate, IAttributeVector & attributes) const
	{
		ATLASSERT(false);
		return attributes.size();
	}

	virtual bool Label(const TCHAR* label, std::_tstring &error_message) const
	{
		ATLASSERT(false);
		return false;
	}

	virtual bool GetLabels(CRepLabelVector & results) const
	{
		ATLASSERT(false);
		return false;
	}

	bool GetLabels(const std::_tstring & module, const std::_tstring & attr, CRepLabelVector & globalLabels, CRepLabelVector & localLabels) const
	{
		if (!GetLabels(globalLabels))
			return false;
		if (!GetAttributeLabels(module, attr, localLabels))
			return false;
		for (CRepLabelVector::iterator itr = localLabels.begin(); itr != localLabels.end(); ++itr)
		{
			for (CRepLabelVector::iterator itr2 = globalLabels.begin(); itr2 != globalLabels.end(); ++itr2)
			{
				if (boost::algorithm::iequals(itr->get()->m_label, itr2->get()->m_label))
				{
					globalLabels.erase(itr2);
					break;
				}
			}
		}
		return true;
	}

	virtual bool GetAttributeLabels(const std::_tstring & module, const std::_tstring & attr, CRepLabelVector & results) const
	{
		ATLASSERT(false);
		return false;
	}

	virtual bool UpdateLabel(const std::_tstring & label, const std::_tstring & module, const std::_tstring & attr, IAttributeType * type, unsigned int _version, std::_tstring &error_message) const
	{
		ATLASSERT(false);
		return false;
	}

	unsigned GetAttributesAutoC(const std::_tstring & module, StdStringVector & results) const
	{
		if (module.empty())
			return 0;

		IAttributeVector attrs;
		GetAttributes(module.c_str(), attrs, true);
		for(IAttributeVector::const_iterator itr = attrs.begin(); itr != attrs.end(); ++itr)
		{
			results.push_back(itr->get()->GetLabel());
		}
		return results.size();
	}

	virtual IModule * RenameModule(const TCHAR* module, const TCHAR* label) const
	{
		StlLinked<IModule> retVal;
		ATLASSERT(false);
		return retVal.get();
	}

	virtual bool DeleteModule(const TCHAR* module) const
	{
		ATLASSERT(false);
		return false;
	}

	virtual IAttribute * GetAttribute(const TCHAR* module, const TCHAR* attribute, IAttributeType * type, int version = 0, bool sandbox = true, bool text = true, bool noBroadcast = false) const
	{
		ATLASSERT(false);
		return NULL;
	}

	virtual IAttribute * GetAttributeFast(const TCHAR* module, const TCHAR* attribute, IAttributeType * type, int version = 0, bool sandbox = true, bool text = false, bool noBroadcast = false) const
	{
		IAttribute * retVal = ::GetAttribute(this, module, attribute, type, version, sandbox, false);
		if (retVal)
			return retVal;

		return GetAttribute(module, attribute, type, version, sandbox, text, noBroadcast);
	}

	virtual IAttributeHistory * GetAttributeHistory(const TCHAR* module, const TCHAR* attribute, IAttributeType * type, int version, bool text = true) const
	{
		ATLASSERT(false);
		return NULL;
	}

	virtual IAttribute * GetAttributePlaceholder(const std::_tstring & moduleLabel, const std::_tstring & attributeLabel, IAttributeType * type) const
	{
		IAttribute * attribute = CreateAttributePlaceholder(this, moduleLabel, attributeLabel, type);
		return attribute;
	}

	IAttribute * GetAttribute(const TCHAR* pModAttr, IAttributeType * type, int version = 0, bool sandbox = true, bool text = true) const
	{
		CModuleHelper moduleHelper(pModAttr);
		return GetAttribute(moduleHelper.GetModuleLabel(), moduleHelper.GetAttributeLabel(), type, version, sandbox, text);
	}

	virtual IAttribute * GetAttributeFuzzy(const std::_tstring & _modAttrLabel, const std::_tstring & currModule, IAttributeType * type, int version, bool sandbox, bool text) const
	{
		StlLinked<IAttribute> attr;

		CString modAttrLabel(_modAttrLabel.c_str());
		modAttrLabel.TrimLeft('.'); //remove leading dots
		modAttrLabel.TrimRight('.'); //remove trailing dots

		CModuleHelper moduleHelper((const TCHAR *)modAttrLabel);

		if (moduleHelper.HasModuleLabel())
		{
			attr = GetAttribute(moduleHelper.GetModuleLabel(), moduleHelper.GetAttributeLabel(), type, version, sandbox, text);
			if (attr.get() == NULL && currModule.length() > 0)
			{
				std::_tstring attrLabelIfDataset;
				attr = GetAttribute(currModule.c_str(), moduleHelper.GetAttributeLabelIfDataset(attrLabelIfDataset), type, version, sandbox, text);		//currModule.dataset.field (with no module)
			}
			if (attr.get() == NULL && moduleHelper.GetQualifiedDepth() >= 2)
			{
				attr = GetAttribute(moduleHelper.GetQualifiedLabel(0), moduleHelper.GetQualifiedLabel(1), type, version, sandbox, text);	//Dataset.field (with no module)
			}
			if (attr.get() == NULL)
			{
				std::_tstring attrLabelIfDataset;
				attr = GetAttribute(_T("default"), moduleHelper.GetAttributeLabelIfDataset(attrLabelIfDataset), type, version, sandbox, text);	//Dataset.field (with no module)
			}
		}
		if (attr.get() == NULL && currModule.length() > 0)
			attr = GetAttribute(currModule.c_str(), moduleHelper.GetAttributeLabel(), type, version, sandbox, text);		
		if (attr.get() == NULL && moduleHelper.HasModuleLabel() && currModule.length() > 0)	//Find ucc.t_UCCReport2Record when it is really iesp.ucc.t_UCCReport2Record...
			attr = GetAttribute(currModule.c_str(), moduleHelper.GetModuleLabel(), type, version, sandbox, text);		
		if (attr.get() == NULL)
			attr = GetAttribute(_T("default"), moduleHelper.GetAttributeLabel(), type, version, sandbox, text);
		ATLASSERT(attr.get());
		return attr.get();
	}

	virtual bool GetAttributeText(const TCHAR* module, const TCHAR* attribute, IAttributeType * type, int version , bool sandbox, CString & text) const
	{
		ATLASSERT(false);
		return false;
	}

	virtual IAttribute * InsertAttribute(const TCHAR* module, const TCHAR* attribute, IAttributeType * type) const
	{
		ATLASSERT(false);
		return NULL;
	}

	virtual unsigned GetAttributeHistory(const TCHAR* module, const TCHAR* attribute, IAttributeType * type, IAttributeHistoryVector & attributes) const
	{
		ATLASSERT(false);
		return attributes.size();
	}

	virtual IAttribute * RenameAttribute(const TCHAR* module, const TCHAR* attribute, IAttributeType * type, const TCHAR* label) const
	{
		StlLinked<IAttribute> retVal;
		ATLASSERT(false);
		return retVal.get();
	}

	IAttribute * RenameAttribute(IAttribute * attr, const TCHAR* label) const
	{
		StlLinked<IAttribute> newAttr = RenameAttribute(attr->GetModuleQualifiedLabel(), attr->GetLabel(), attr->GetType(), label);
		if (newAttr)
			attr->Refresh(false, newAttr);
		return newAttr.get();
	}

	virtual bool DeleteAttributes(const IAttributeVector & pAttributes) const
	{
		ATLASSERT(false);
		return false;
	}

	bool Checkin(IAttributeVector & pAttributes, const TCHAR* comment, bool andRevert)
	{
		IAttributeVector attrToProcess, attrToCheckin, attrToCheckoutBeforeCheckin, attrToRollback;
		for(IAttributeVector::iterator itr = pAttributes.begin(); itr != pAttributes.end(); ++itr)
		{
			if (std::find(attrToProcess.begin(), attrToProcess.end(), *itr) == attrToProcess.end())
				attrToProcess.push_back(*itr);
			IAttributeVector dependantAttrs;	//  Don't forget these are ModFileAttributes...
			Dali::CEclExceptionVector errors;
			itr->get()->PreProcess(PREPROCESS_COMMIT, NULL, dependantAttrs, errors);
			for(IAttributeVector::const_iterator itr = dependantAttrs.begin(); itr != dependantAttrs.end(); ++itr)
			{
				StlLinked<IAttribute> attr = GetAttribute(itr->get()->GetModuleQualifiedLabel(), itr->get()->GetLabel(), itr->get()->GetType());
				if (attr)
					attrToProcess.push_back(attr);
			}
		}
		for(IAttributeVector::iterator itr = attrToProcess.begin(); itr != attrToProcess.end(); ++itr)
		{
			if(itr->get()->IsSandboxed())
			{
				if (!itr->get()->IsCheckedOut())
				{
					attrToCheckoutBeforeCheckin.push_back(*itr);
				}
				attrToCheckin.push_back(*itr);
			}
			else if(itr->get()->IsCheckedOut())
			{
				attrToRollback.push_back(*itr);
			}
		}
		bool retVal = false;

		if (attrToCheckoutBeforeCheckin.size())
			retVal = Checkout(attrToCheckoutBeforeCheckin);
		else
			retVal = true;

		if (!retVal)
			return false;

		if (attrToCheckin.size())
		{
			IAttributeVector attrsToRevert;
			if (andRevert)
			{
				for (IAttributeVector::iterator itr = attrToCheckin.begin(); itr != attrToCheckin.end(); ++itr)
				{
					CComPtr<IAttribute> attr = GetAttribute(itr->get()->GetModuleQualifiedLabel(), itr->get()->GetLabel(), itr->get()->GetType(), 0, false, true, true);
					if (attr)
						attrsToRevert.push_back(attr.p);
				}
			}

			retVal = Checkin(attrToCheckin, comment);

			if (retVal && andRevert)
			{
				retVal = false;
				if (Save(attrsToRevert))
				{
					if (Checkin(attrsToRevert, _T("Checkin for Review:  Previous version ready for review."), false))
					{
						for (IAttributeVector::const_iterator itr = attrsToRevert.begin(); itr != attrsToRevert.end(); ++itr)
						{
							IAttributeHistoryVector attrHistory;
							itr->get()->GetHistory(attrHistory);
							if (attrHistory.size() >= 1)
								itr->get()->SetText(attrHistory[0]->GetText());
						}
						retVal = true;
					}
				}
			}
		}

		if (!retVal)
			return false;

		if (attrToRollback.size())
			retVal = Rollback(attrToRollback);

		return retVal;
	}

	virtual bool Checkin(IAttributeVector & pAttributes, const TCHAR* comment)
	{
		ATLASSERT(false);
		return false;
	}

	virtual bool Checkout(IAttributeVector & attributes)
	{
		ATLASSERT(false);
		return false;
	}

	virtual bool Save(IAttributeVector & attributes, bool noBroadcast = false)
	{
		ATLASSERT(false);
		return false;
	}

	virtual bool Move(IAttributeVector & attributes, const TCHAR* module)
	{
		ATLASSERT(false);
		return false;
	}

	virtual bool Rollback(IAttributeVector & attributes)
	{
		ATLASSERT(false);
		return false;
	}

	bool ModuleExists(const TCHAR* label)
	{
		return (GetModule(label) != NULL);
	}

	bool AttributeExists(const TCHAR* module, const TCHAR* label, IAttributeType * type)
	{
		return (GetAttribute(module, label, type) != NULL);
	}

	IWorkspace * CreateIWorkspace(const std::_tstring & label = _T(""), bool showInRoot = true, bool placeholder = false)
	{
		return ::CreateIWorkspace(this, label, showInRoot, placeholder);
	}

	unsigned int GetWorkspaces(IWorkspaceVector * results, bool attributesOnly)
	{
		return ::GetWorkspaces(this, results, attributesOnly);
	}

	bool WorkspaceExists(const std::_tstring & label)
	{
		return ::WorkspaceExists(this, label);
	}

	bool RemoveWorkspace(const std::_tstring & label)
	{
		return ::RemoveWorkspace(this, label);
	}

	IWorkspaceItem * CreateIWorkspaceItem(WORKSPACE_ITEM_TYPE type, const std::_tstring & label, const std::_tstring & path)
	{
		return ::CreateIWorkspaceItem(this, type, label, path);
	}

	IWorkspaceItem * CreateIWorkspaceItem(IAttribute * attr, const std::_tstring & path)
	{
		return ::CreateIWorkspaceItem(this, attr, path);
	}

	IWorkspaceItem * CreateIWorkspaceItem(WORKSPACE_ITEM_TYPE type)
	{
		return  ::CreateIWorkspaceItem(this, type);
	}

	IWorkspaceItem * CreateIWorkspaceItem(WORKSPACE_ITEM_TYPE type, const CPersistMap & pm)
	{
		return  ::CreateIWorkspaceItem(this, type, pm);
	}

	const boost::filesystem::path & GetEnvironmentFolder(boost::filesystem::path & path) const = 0;

	void Refresh()
	{
	}

	void ClearShortTermCache()
	{
		m_cacheGetModules.clear();
		m_cacheGetModulesHierarchy.clear();
		m_cacheGetAttributes.clear();
	}
};

