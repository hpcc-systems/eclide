#include "StdAfx.h"

#include "repositoryImpl.h"
#include "Module.h"
#include "cache.h"
#include "UnicodeFile.h"
#include "md5.hpp"
#include "EclCC.h"

#if _COMMS_VER < 68200
using namespace WsAttributes;
#else
#endif
namespace fs = boost::filesystem;

IModule * CreateDiskModule(const IRepository *rep, const std::_tstring &label, const boost::filesystem::wpath & path, bool noBroadcast = false);
IModule * CreateDiskModulePlaceholder(const IRepository *rep, const std::_tstring &label, const boost::filesystem::wpath & path);
IAttribute * CreateDiskAttribute(const IRepository *rep, const std::_tstring &moduleLabel, const std::_tstring &label, const std::_tstring &type, const boost::filesystem::wpath & path, const std::_tstring & ecl);
IAttribute * GetDiskAttribute(const IRepository *rep, const TCHAR* module, const TCHAR* label, IAttributeType * type, unsigned version, bool sandboxed, bool placeholder);
IAttribute * CreateDiskAttributePlaceholder(const IRepository *rep, const TCHAR* module, const TCHAR* label, const TCHAR* type, const boost::filesystem::wpath & path);
//IAttributeHistory * CreateAttributeHistory(const IRepository *rep, const std::_tstring &moduleLabel, const std::_tstring & label, const ECLAttribute * data);
void ClearDiskAttributeCache();
void ClearDiskModuleCache();

typedef std::map<std::_tstring, fs::wpath> StringPathMap;
typedef std::vector<fs::wpath> PathVector;
class CDiskRepository : public CRepositoryBase
{
protected:
	StringPathMap m_paths;
	PathVector m_pathOrder;

public:
	IMPLEMENT_CUNKNOWN;

	CDiskRepository(const TCHAR* url, const TCHAR* label) : CRepositoryBase(url, _T(""), _T(""), label, _T(""))
	{
		if (CComPtr<IEclCC> eclcc = CreateIEclCC())
		{
			for (int i = 0; i < eclcc->GetEclFolderCount(); ++i)
			{
				fs::wpath path = fs::wpath(eclcc->GetEclFolder(i), fs::native);
				m_paths[path.leaf()] = path;
				m_pathOrder.push_back(path);
			}
		}
		else
		{
			fs::wpath path = fs::wpath(url, fs::native);
			m_paths[path.leaf()] = path;
			m_pathOrder.push_back(path);
		}
	}

	~CDiskRepository()
	{
	}

	unsigned GetModulesIgnoreRoot(const std::_tstring & module, IModuleVector & modules, bool GetChecksum = false, bool noRefresh=true) const
	{
		IModuleVector allModules;
		IModuleHierarchy moduleHierarchy;
		GetAllModules(allModules, moduleHierarchy, GetChecksum, noRefresh);
		for(IModuleHierarchy::const_iterator itr = moduleHierarchy.begin(); itr != moduleHierarchy.end(); ++itr)
		{
			if (!itr->first.empty())
			{
				CModuleHelper modHelper(itr->first + _T(".dummyAttr"));
				if (boost::iequals(modHelper.GetModuleLabelNoRoot(), module))
				{
					for(IModuleVector::const_iterator modItr = itr->second.begin(); modItr != itr->second.end(); ++modItr)
						modules.push_back(modItr->get());
				}
			}
		}
		return 0;
	}

	int GetAllModules(const fs::wpath & path, const std::_tstring & module, IModuleVector & modules, IModuleHierarchy & moduleHierarchy, bool noRefresh = true, bool noBroadcast = false) const
	{
		CachePoolAccessor<IModuleVector> modulesCache(m_cacheGetModules, GetCacheID(), path.native_file_string());
		CachePoolAccessor<IModuleHierarchy> moduleHierarchyCache(m_cacheGetModulesHierarchy, GetCacheID(), path.native_file_string());

		if (!modulesCache.needs_update(noRefresh) && !moduleHierarchyCache.needs_update(noRefresh))
		{
			modules = modulesCache.get();
			moduleHierarchy = moduleHierarchyCache.get();
			return modules.size();
		}

		if (fs::exists(path) && fs::is_directory(path))
		{
			std::_tstring leaf = path.leaf().c_str();
			if (!boost::algorithm::starts_with(leaf, _T(".")))
			{  
				std::_tstring label = module;
				if (!label.empty())
					label += _T(".");
				label += leaf;
				StlLinked<IModule> mod = CreateDiskModule(this, label.c_str(), path, noBroadcast);
				modules.push_back(mod);
				moduleHierarchy[module].push_back(mod);

				fs::wdirectory_iterator end_itr; // default construction yields past-the-end
				for (fs::wdirectory_iterator itr(path); itr != end_itr; ++itr)
				{
					GetAllModules(*itr, label, modules, moduleHierarchy, false, noBroadcast);
				}
			}
		}

		if (noRefresh)
		{
			m_cacheGetAttributes.clear();	//Need to clear attr cache when mod cache is cleared
			modulesCache.update(modules);
			moduleHierarchyCache.update(moduleHierarchy);
		}
		return modules.size();
	}

	virtual unsigned GetAllModules(IModuleVector & modules, IModuleHierarchy & moduleHierarchy, bool GetChecksum = false, bool noRefresh = true, bool noBroadcast = false) const
	{
		for(PathVector::const_iterator itr = m_pathOrder.begin(); itr != m_pathOrder.end(); ++itr)
			GetAllModules(*itr, _T(""), modules, moduleHierarchy, noRefresh, noBroadcast);
		//std::sort(modules.begin(), modules.end(), IModuleCompare());
		return modules.size();
	}

	virtual unsigned GetModulesAutoC(const std::_tstring & module, StdStringVector &result) const
	{
		IModuleVector modules;
		GetModulesIgnoreRoot(module, modules);
		for(IModuleVector::const_iterator itr = modules.begin(); itr != modules.end(); ++itr)
			result.push_back(itr->get()->GetLabel());
		return result.size();
	}

	virtual IModule * InsertModule(const TCHAR* moduleName) const
	{
		StlLinked<IModule> result = 0;

		CModuleHelper modHelper(moduleName);
		StdStringVector tokens;
		modHelper.GetQualifiedLabel(tokens);
		ATLASSERT(!tokens.empty());
		StringPathMap::const_iterator found = m_paths.find(tokens[0]);
		if (found != m_paths.end())
		{
			boost::filesystem::wpath path = found->second.parent_path();
			if (fs::exists(path))	//  Root repository folder
			{
				for (unsigned int i = 0; i < tokens.size(); ++i)
				{
					path /= tokens[i];
					try {
						if (!fs::exists(path))
							fs::create_directories(path);
					} catch (const std::exception & ex) {
						_DBGLOG(LEVEL_WARNING, ex.what());
					}

					if (fs::exists(path))
					{
						result = CreateDiskModule(this, moduleName, path);
						CachePoolAccessor<IModuleVector> modulesCache(m_cacheGetModules, GetCacheID(), m_repositoryLabel);
						modulesCache.clear();
					}
				}
			}
		}
		/*
		std::string nameDirectory = m_path.native_directory_string();
		nameDirectory.append("/");
		std::_tstring module = moduleName;
		boost::algorithm::replace_all(module, _T("."), _T("/"));
		nameDirectory.append(CT2A(module.c_str()));
		try
		{
			fs::path pathDirectory(nameDirectory.c_str(), fs::native);
			fs::create_directories(pathDirectory);
			if (fs::exists(pathDirectory))
			{
				result = CreateDiskModule(this, moduleName);
				CachePoolAccessor<IModuleVector> modulesCache(m_cacheGetModules, GetCacheID(), m_repositoryLabel);
				modulesCache.clear();
			}
		}
		catch (boost::filesystem::filesystem_error  &)
		{
			//  Path doesn't exist!
		}
		*/
		return result;
	}

	virtual unsigned GetAttributes(const std::_tstring & _moduleName, const IAttributeTypeVector & types, IAttributeVector & attributes, bool noRefresh=false, bool noBroadcast=false) const
	{
		fs::wpath repositoryPath;
		std::_tstring moduleName = _moduleName;
		if (GetRepositoryPath(moduleName, repositoryPath))
		{
			CModuleHelper modHelper(moduleName + _T(".dummy"));
			std::_tstring module = modHelper.GetModuleLabelNoRoot();
			boost::algorithm::replace_all(module, _T("."), _T("/"));
			fs::wpath path = repositoryPath / module;
			if (fs::exists(path))
			{
				fs::wdirectory_iterator attr_itr_end;
				for (fs::wdirectory_iterator attr_itr(path); attr_itr != attr_itr_end; attr_itr++)
				{
					if (!fs::is_directory(*attr_itr))
					{  
						std::_tstring label = fs::basename(*attr_itr).c_str();
						std::_tstring type = fs::extension(*attr_itr).c_str();
						if (IsValidExtension(type))
						{
							StlLinked<IAttribute> attribute = CreateDiskAttribute(this, moduleName, label, type, *attr_itr, _T(""));
							if (attribute.isLinked())
								attributes.push_back(attribute);
						}
					}
				}
				std::sort(attributes.begin(), attributes.end(), IAttributeCompare());
			}
		}
		return attributes.size();
	}

	virtual unsigned int GetAttributeTypes(IAttributeTypeVector & types) const
	{
		types.push_back(CreateIAttributeECLType());
		types.push_back(CreateIAttributeESDLType());
		types.push_back(CreateIAttributeSALTType());
		return types.size();
	}

	virtual unsigned FindAttributes(const std::_tstring & searchText, const std::_tstring & module, const std::_tstring & user, bool regExp, bool sandboxed, bool checkedout, bool locked, bool orphaned, const TCHAR* utcDate, IAttributeVector & attributes) const
	{
		if (CComPtr<IEclCC> eclcc = CreateIEclCC())
		{
			IModuleVector modules;
			IModuleHierarchy moduleHierarchy;
			for(StringPathMap::const_iterator itr = m_paths.begin(); itr != m_paths.end(); ++itr)
				GetAllModules(itr->second, _T(""), modules, moduleHierarchy, true, true);

			for (IModuleVector::const_iterator itr = modules.begin(); itr != modules.end(); ++itr)
			{
				if (module.empty() || boost::algorithm::iequals(module, itr->get()->GetQualifiedLabel()))
				{
					IAttributeVector attrs;
					itr->get()->GetAttributes(attrs, true);
					for (IAttributeVector::const_iterator a_itr = attrs.begin(); a_itr != attrs.end(); ++a_itr)
					{
						std::_tstring ecl = a_itr->get()->GetText(false, true);
						if (boost::algorithm::icontains(ecl, searchText))
							attributes.push_back(a_itr->get());
					}
				}
			}
		}

		return 0;
	}

	virtual bool Label(const TCHAR* label, std::_tstring &error_message) const
	{
		ATLASSERT(!"CDiskRepository::Label not implemented");
		return false;
	}

	virtual bool GetLabels(CRepLabelVector & results) const
	{
		return true;
	}

	virtual bool GetAttributeLabels(const std::_tstring & module, const std::_tstring & attr, CRepLabelVector & results) const
	{
		return true;
	}
	
	unsigned GetAttributes(const TCHAR* module, IAttributeVector & attributes, bool noRefresh=false, bool noBroadcast=false) const
	{
		IAttributeTypeVector types;
		return GetAttributes(module, types, attributes, noRefresh, noBroadcast);
	}

	virtual bool UpdateLabel(const std::_tstring & label, const std::_tstring & module, const std::_tstring & attr, IAttributeType * type, unsigned int _version, std::_tstring &error_message) const
	{
		ATLASSERT(!"CDiskRepository::UpdateLabel not implemented");
		return false;
	}

	virtual IModule * RenameModule(const TCHAR* _moduleName, const TCHAR* label) const
	{
		fs::wpath repositoryPath;
		std::_tstring moduleName = _moduleName;
		if (GetRepositoryPath(moduleName, repositoryPath))
		{
			CModuleHelper fromHelper(moduleName + _T(".dummy"));
			std::_tstring fromModule = fromHelper.GetModuleLabelNoRoot();
			boost::algorithm::replace_all(fromModule, _T("."), _T("/"));
			fs::wpath fromPath = repositoryPath / fromModule;

			CModuleHelper toHelper(std::_tstring(label) + _T(".dummy"));
			std::_tstring toModule = toHelper.GetModuleLabelNoRoot();
			boost::algorithm::replace_all(toModule, _T("."), _T("/"));
			fs::wpath toPath = repositoryPath / toModule;
			try
			{
				boost::filesystem::rename(fromPath, toPath);
			} catch (const std::exception & ex) {
				_DBGLOG(LEVEL_WARNING, ex.what());
				return NULL;
			}
			CachePoolAccessor<IModuleVector> modulesCache(m_cacheGetModules, GetCacheID(), m_repositoryLabel);
			modulesCache.clear();
			return CreateDiskModule(this, label, toPath);
		}
		return NULL;
	}

	virtual bool DeleteModule(const TCHAR* _moduleName) const
	{
		fs::wpath repositoryPath;
		std::_tstring moduleName = _moduleName;
		if (GetRepositoryPath(moduleName, repositoryPath))
		{
			CModuleHelper modHelper(moduleName + _T(".dummy"));
			std::_tstring module = modHelper.GetModuleLabelNoRoot();
			boost::algorithm::replace_all(module, _T("."), _T("/"));
			fs::wpath path = repositoryPath / module;
			if (fs::exists(path))
			{
				try {
					fs::remove_all(path);
				} catch (const std::exception & ex) {
					_DBGLOG(LEVEL_WARNING, ex.what());
					return false;
				}
				CachePoolAccessor<IModuleVector> modulesCache(m_cacheGetModules, GetCacheID(), m_repositoryLabel);
				modulesCache.clear();
				return true;
			}
		}
		return false;
	}

	bool GetRepositoryPath(std::_tstring & module, fs::wpath & path) const
	{
		CModuleHelper modHelper(module);
		StdStringVector tokens;
		modHelper.GetQualifiedLabel(tokens);
		ATLASSERT(!tokens.empty());
		StringPathMap::const_iterator found = m_paths.find(tokens[0]);
		if (found != m_paths.end())
		{
			path = found->second;
			if (fs::exists(path))
				return true;
		}
		else
		{
			for(StringPathMap::const_iterator itr = m_paths.begin(); itr != m_paths.end(); ++itr)
			{
				if (fs::exists(itr->second / tokens[0]))
				{
					path = itr->second;
					if (fs::exists(path))
					{
						module = itr->first + _T(".") + module;
						return true;
					}
				}
			}
		}
		return false;
	}

	bool GetPath(std::_tstring & moduleName, const std::_tstring & attributeName, IAttributeType * type, fs::wpath & path) const
	{
		fs::wpath repositoryPath;
		if (GetRepositoryPath(moduleName, repositoryPath))
		{
			CModuleHelper modHelper(moduleName + _T(".") + attributeName);
			std::_tstring module = modHelper.GetModuleLabelNoRoot();
			boost::algorithm::replace_all(module, _T("."), _T("/"));
			std::wstring attr = attributeName;
			attr += _T(".");
			attr += type->GetRepositoryCode();
			path = repositoryPath / module / attr;
			if (fs::exists(path))
				return true;
		}
		return false;
	}

	virtual IAttribute * GetAttribute(const TCHAR* _module, const TCHAR* attribute, IAttributeType * type, int version = 0, bool sandbox = true, bool text = true, bool noBroadcast = false) const
	{
		IAttribute* result = NULL;

		if (0 == attribute)
			return result;

		fs::wpath path;
		std::_tstring module = _module;
		if (!GetPath(module, attribute, type, path))
			return result;

		if (fs::exists(path))
		{
			CUnicodeFile file;
			file.Open(path.native_file_string().c_str());
			if (file.IsOpen())
			{
				std::_tstring data;
				file.Read(data);
				file.Close();
				result = ::CreateDiskAttribute(this, module, attribute, type->GetRepositoryCode(), path, data);
				return result;
			}
		}
		return result;
	}

	virtual IAttribute * GetAttributeFast(const TCHAR* module, const TCHAR* attribute, IAttributeType * type, int version = 0, bool sandbox = true, bool text = false, bool noBroadcast = false) const
	{
		IAttribute * retVal = ::GetDiskAttribute(this, module, attribute, type, version, sandbox, false);
		if (retVal)
			return retVal;

		return GetAttribute(module, attribute, type, version, sandbox, text, noBroadcast);
	}

	virtual IAttributeHistory * GetAttributeHistory(const TCHAR* module, const TCHAR* attribute, IAttributeType * type, int version, bool text = true) const
	{
		ATLASSERT(!"CDiskRepository::GetAttributeH not implemented");
		return NULL;
	}

	virtual IAttribute * GetAttributePlaceholder(const std::_tstring & _moduleName, const std::_tstring & attributeName, IAttributeType * type) const
	{
		fs::wpath repositoryPath;
		std::_tstring moduleName = _moduleName;
		if (GetRepositoryPath(moduleName, repositoryPath))
		{
			CModuleHelper modHelper(moduleName + _T(".") + attributeName);
			std::_tstring module = modHelper.GetModuleLabelNoRoot();
			boost::algorithm::replace_all(module, _T("."), _T("/"));
			fs::wpath path = repositoryPath / module / attributeName;
			if (fs::exists(path))
			{
				IAttribute * attribute = CreateDiskAttributePlaceholder(this, moduleName.c_str(), attributeName.c_str(), type->GetRepositoryCode(), path);
				return attribute;
			}
		}
		return NULL;
	}

	virtual IAttribute * GetAttributeFuzzy(const std::_tstring & moduleAttribute, const std::_tstring & module, IAttributeType * type, int version = 0, bool sandbox = true, bool text = true) const
	{
		std::_tstring modAttr = moduleAttribute;
		if (boost::algorithm::iends_with(modAttr, _T(".ecl")))
			modAttr.resize(modAttr.size() - strlen(".ecl"));
		else if (boost::algorithm::iends_with(modAttr, _T(".eclattr")))
			modAttr.resize(modAttr.size() - strlen(".eclattr"));
		boost::algorithm::replace_all(modAttr, _T("\\"), _T("."));

		IAttribute * retVal = __super::GetAttributeFuzzy(modAttr, module, type, version, sandbox, text);
		ATLASSERT(retVal);  // Must check all fuzzies
		return retVal;
	}

	virtual bool GetAttributeText(const TCHAR* module, const TCHAR* attribute, IAttributeType * type, int version , bool sandbox, CString & text) const
	{
		ATLASSERT(!"CDiskRepository::GetAttributeText not implemented");
		return false;
	}

	virtual IAttribute * InsertAttribute(const TCHAR* module, const TCHAR* attribute, IAttributeType * type) const
	{
		ATLASSERT(module);
		ATLASSERT(attribute);

		CModuleHelper modHelper(module);
		StdStringVector tokens;
		modHelper.GetQualifiedLabel(tokens);
		ATLASSERT(!tokens.empty());
		StringPathMap::const_iterator found = m_paths.find(tokens[0]);
		if (found != m_paths.end())
		{
			boost::filesystem::wpath path = found->second.parent_path();
			if (fs::exists(path))	//  Root repository folder
			{
				for (unsigned int i = 0; i < tokens.size(); ++i)
				{
					path /= tokens[i];
					if (!fs::exists(path))
						fs::create_directories(path);
				}
				std::_tstring filename = attribute;
				filename += _T(".");
				filename += type->GetRepositoryCode();
				path /= filename;

				CUnicodeFile file;
				file.Create(path.native_file_string().c_str(),GENERIC_WRITE,CREATE_ALWAYS,CUnicodeFile::ENCODING_ANSI);
				file.Close();
			}
		}

		/*
		CString filePath = m_path.native_file_string().c_str();
		filePath.Append(_T("/"));
		CString moduleLabel = module;
		moduleLabel.Replace(_T("."), _T("/"));
		filePath.Append(moduleLabel);
		filePath.Append(_T("/"));
		filePath.Append(attribute);
		filePath.Append(_T("."));
		filePath.Append(type->GetRepositoryCode());
		CUnicodeFile file;
		file.Create(filePath,GENERIC_WRITE,CREATE_ALWAYS,CUnicodeFile::ENCODING_ANSI);
		file.Close();
		*/
		return GetAttribute(module, attribute, type);
	}

	virtual unsigned GetAttributeHistory(const TCHAR* module, const TCHAR* attribute, IAttributeType * type, IAttributeHistoryVector & attributes) const
	{
		return 0;
	}

	virtual IAttribute * RenameAttribute(const TCHAR* module, const TCHAR* attribute, IAttributeType * type, const TCHAR* label) const
	{
		ATLASSERT(!"CDiskRepository::RenameAttribute not implemented");
		return 0;
	}
	IAttribute * RenameAttribute(IAttribute * attribute, const TCHAR* label) const
	{
		ATLASSERT(!"CDiskRepository::RenameAttribute not implemented");
		return 0;
	}
	
	virtual bool DeleteAttributes(const IAttributeVector & pAttributes) const
	{
		for(IAttributeVector::const_iterator itr = pAttributes.begin(); itr != pAttributes.end(); ++itr)
		{
			itr->get()->Delete();
			itr->get()->Refresh(false, NULL, true);
		}
		return false;
	}

	virtual bool Checkin(IAttributeVector & pAttributes, const TCHAR* comment)
	{
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
		for(IAttributeVector::const_iterator itr = attributes.begin(); itr != attributes.end(); ++itr)
		{
			fs::wpath fromPath;
			std::_tstring fromModule = itr->get()->GetModuleLabel();
			GetPath(fromModule, itr->get()->GetLabel(), itr->get()->GetType(), fromPath);
			if (fs::exists(fromPath))
			{
				fs::wpath toPath;
				std::_tstring toModule = module;
				GetPath(toModule, itr->get()->GetLabel(), itr->get()->GetType(), toPath);
				try
				{
					fs::rename(fromPath, toPath);
				} catch (const std::exception & ex) {
					_DBGLOG(LEVEL_WARNING, ex.what());
				}
				CComPtr<IAttribute> attribute = CreateDiskAttribute(this, toModule, itr->get()->GetLabel(), itr->get()->GetType()->GetRepositoryCode(), toPath, itr->get()->GetText());
				attribute->Refresh(false, attribute);
				return true;
			}
		}
		return false;
	}

	virtual bool Rollback(IAttributeVector & attributes)
	{
		ATLASSERT(false);
		return false;
	}
};

static CacheT<std::_tstring, CDiskRepository> DiskRepositoryCache;

void ClearDiskRepositorySingletons()
{
	ClearDiskAttributeCache();
	ClearDiskModuleCache();
	DiskRepositoryCache.Clear();
}

COMMS_API IRepository * AttachDiskRepository(const TCHAR* url, const TCHAR * label, bool bClearCache)
{
	CDiskRepository *rep = new CDiskRepository(url, label);
	if ( bClearCache )
		DiskRepositoryCache.Clear(rep);
	return DiskRepositoryCache.Get(rep);
}

COMMS_API IRepository * AttachDiskRepository(const TCHAR * label, bool bClearCache)
{
	if (CComPtr<IEclCC> eclcc = CreateIEclCC())
	{
		std::_tstring url;
		for (int i = 0; i < eclcc->GetEclFolderCount(); ++i)
		{
			url += eclcc->GetEclFolder(i);
		}
		CComPtr<CDiskRepository> rep = new CDiskRepository(url.c_str(), label);
		if ( bClearCache )
			DiskRepositoryCache.Clear(rep);
		return DiskRepositoryCache.Get(rep);
	}
	return NULL;
}


