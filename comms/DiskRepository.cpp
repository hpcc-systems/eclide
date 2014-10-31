#include "StdAfx.h"

#include "repositoryImpl.h"
#include "Module.h"
#include "cache.h"
#include "UnicodeFile.h"
#include "md5.hpp"
#include "EclCC.h"
#include <UtilFilesystem.h>

#if _COMMS_VER < 68200
using namespace WsAttributes;
#else
#endif
namespace algo = boost::algorithm;

IModule * CreateDiskModule(const IRepository *rep, const std::_tstring &label, const boost::filesystem::wpath & path, bool noBroadcast = false);
IAttribute * CreateDiskAttribute(const IRepository *rep, const std::_tstring &moduleLabel, const std::_tstring &label, const std::_tstring &type, const boost::filesystem::wpath & path, const std::_tstring & ecl, bool noBroadcast);
IAttribute * GetDiskAttribute(const IRepository *rep, const TCHAR* module, const TCHAR* label, IAttributeType * type, unsigned version, bool sandboxed);
IAttribute * CreateDiskAttributePlaceholder(const IRepository *rep, const TCHAR* module, const TCHAR* label, const TCHAR* type, const boost::filesystem::wpath & path);
//IAttributeHistory * CreateAttributeHistory(const IRepository *rep, const std::_tstring &moduleLabel, const std::_tstring & label, const ECLAttribute * data);
void ClearDiskAttributeCache();
void ClearDiskModuleCache();

//  ===========================================================================
class CDiskRepository;
class CMonitorFolder : public CUnknown
{
public:
	const boost::filesystem::wpath m_path;
	const std::_tstring m_leaf;

private:
	CDiskRepository * m_repository;
	bool m_doMonitor;

	static void thread_MonitorFolder1(CComPtr<CMonitorFolder> self)
	{
		HANDLE file = FindFirstChangeNotification(pathToWString(self->m_path).c_str(), FALSE, FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE);
		while (self->m_doMonitor && WaitForSingleObject(file, 500) != WAIT_TIMEOUT) {
			FindNextChangeNotification(file);
		}
	}

	static void thread_MonitorFolder2(CComPtr<CMonitorFolder> self)	
	{
		USES_CONVERSION;

		HANDLE hDir = CreateFile(pathToWString(self->m_path).c_str(), FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL);

		FILE_NOTIFY_INFORMATION Buffer[1024];
		DWORD BytesReturned;
		while( ReadDirectoryChangesW(
			hDir,                                  // handle to directory
			&Buffer,                                    // read results buffer
			sizeof(Buffer),                                // length of buffer
			TRUE,                                 // monitoring option
			FILE_NOTIFY_CHANGE_SECURITY|
			FILE_NOTIFY_CHANGE_CREATION|
			FILE_NOTIFY_CHANGE_LAST_ACCESS|
			FILE_NOTIFY_CHANGE_LAST_WRITE|
			FILE_NOTIFY_CHANGE_SIZE|
			FILE_NOTIFY_CHANGE_ATTRIBUTES|
			FILE_NOTIFY_CHANGE_DIR_NAME|
			FILE_NOTIFY_CHANGE_FILE_NAME,             // filter conditions
			&BytesReturned,              // bytes returned
			NULL,                          // overlapped buffer
			NULL// completion routine
			))
		{
			for (int i = 0; !Buffer[i].NextEntryOffset; ++i)
			{
				if (!self->m_doMonitor)
					return;

				CString helper_txt;
				switch(Buffer[i].Action)
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

				std::_tstring filename(Buffer[i].FileName, Buffer[i].FileNameLength / sizeof(_T(' ')));
				if (HasValidExtension(filename))
				{
					boost::filesystem::wpath fullPath = self->m_path / filename;
					boost::filesystem::wpath relativePath = boost::filesystem::wpath(self->m_path.filename()) / filename;
					std::_tstring type = pathToWString(relativePath.extension());
					std::_tstring moduleLabel = pathToWString(relativePath.parent_path());
					algo::replace_all(moduleLabel, _T("\\"), _T("."));
					std::_tstring attrLabel = stringToWString(boost::filesystem::basename(relativePath)).c_str();
					std::_tstring ecl;
					CComPtr<IAttribute> attr = GetDiskAttribute((IRepository *)self->m_repository, moduleLabel.c_str(), attrLabel.c_str(), CreateIAttributeType(type), 0, false);
					if (attr)
						attr->GetText();
				}
			}
		}
	}

public:
	CMonitorFolder(CDiskRepository * repository, const boost::filesystem::path & path) : m_repository(repository), m_path(pathToWPath(path)), m_leaf(pathToWString(path.filename()))
	{
		ATLASSERT(boost::filesystem::exists(m_path) && boost::filesystem::is_directory(m_path));
		m_doMonitor = true;
		clib::thread run(__FUNCTION__, boost::bind(&thread_MonitorFolder2, this));
	}

	~CMonitorFolder()
	{
	}

	virtual ULONG STDMETHODCALLTYPE Release() throw()
	{
		ULONG refCount = CUnknown::Release();
		if (refCount == 1)
		{
			m_doMonitor = false;
			m_repository = NULL;
			//  Touch the folder to trigger the clean exit of the monitor thread  ---
			CAtlTemporaryFile temp;
			temp.Create(pathToWString(m_path).c_str());
		}
		return refCount;
	}
};
//  ===========================================================================
typedef std::map<std::_tstring, StlLinked<CMonitorFolder> > StringCMonitorFolderMap;
typedef std::vector<boost::filesystem::wpath> PathVector;
class CDiskRepository : public CRepositoryBase
{
protected:
	boost::filesystem::wpath m_amtRoot;
	StringCMonitorFolderMap m_paths;
	PathVector m_pathOrder;
	mutable clib::recursive_mutex m_mutex;

public:
	IMPLEMENT_CUNKNOWN;

	CDiskRepository(const TCHAR* label) : CRepositoryBase(_T(""), _T(""), label, _T(""))
	{
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

	int GetAllModules(const boost::filesystem::wpath & path, const std::_tstring & module, IModuleVector & modules, IModuleHierarchy & moduleHierarchy, bool noRefresh = true, bool noBroadcast = false) const
	{
		if (boost::filesystem::exists(path) && boost::filesystem::is_directory(path))
		{
			std::_tstring leaf = pathToWString(path.leaf());
			if (!algo::contains(leaf, _T(".")))
			{  
				std::_tstring label = module;
				if (!label.empty())
					label += _T(".");
				label += leaf;
				StlLinked<IModule> mod = CreateDiskModule(this, label.c_str(), path, noBroadcast);
				modules.push_back(mod);
				moduleHierarchy[module].push_back(mod);

				boost::filesystem::directory_iterator end_itr; // default construction yields past-the-end
				for (boost::filesystem::directory_iterator itr(wpathToPath(path)); itr != end_itr; ++itr)
				{
					GetAllModules(pathToWString(*itr), label, modules, moduleHierarchy, false, noBroadcast);
				}
			}
		}
		return modules.size();
	}

	virtual unsigned GetAllModules(IModuleVector & _modules, IModuleHierarchy & _moduleHierarchy, bool GetChecksum = false, bool noRefresh = true, bool noBroadcast = false) const
	{
		CachePoolAccessor<IModuleVector> modulesCache(m_cacheGetModules, GetCacheID(), m_repositoryLabel, boost::lexical_cast<std::_tstring>(GetChecksum));
		CachePoolAccessor<IModuleHierarchy> moduleHierarchyCache(m_cacheGetModulesHierarchy, GetCacheID(), m_repositoryLabel, boost::lexical_cast<std::_tstring>(GetChecksum));
		if (!modulesCache.needs_update(noRefresh) && !moduleHierarchyCache.needs_update(noRefresh))
		{
			_modules = modulesCache.get();
			_moduleHierarchy = moduleHierarchyCache.get();
			return _modules.size();
		}

		clib::recursive_mutex::scoped_lock proc(m_mutex);
		IModuleVector modules;
		IModuleHierarchy moduleHierarchy;
		for(PathVector::const_iterator itr = m_pathOrder.begin(); itr != m_pathOrder.end(); ++itr)
			GetAllModules(*itr, _T(""), modules, moduleHierarchy, noRefresh, noBroadcast);

		if (m_amtRoot.empty())
		{
			_modules = modules;
			_moduleHierarchy = moduleHierarchy;
		}
		else
		{
			for (IModuleHierarchy::const_iterator itr = moduleHierarchy.begin(); itr != moduleHierarchy.end(); ++itr) 
			{
				if (itr->first.empty())
					continue;
				else if (algo::equals(pathToWString(m_amtRoot.leaf()), itr->first))
					_moduleHierarchy[_T("")] = itr->second;
				else
					_moduleHierarchy[itr->first] = itr->second;
				_modules.insert(_modules.end(), itr->second.begin(), itr->second.end());
			}
		}
		modulesCache.update(_modules);
		moduleHierarchyCache.update(_moduleHierarchy);
		return _modules.size();
	}

	virtual unsigned GetModulesAutoC(const std::_tstring & module, StdStringVector &result) const
	{
		IModuleVector modules;
		GetModulesIgnoreRoot(module, modules);
		for(IModuleVector::const_iterator itr = modules.begin(); itr != modules.end(); ++itr)
			result.push_back(itr->get()->GetLabel());
		return result.size();
	}

	IModule * GetModulePlaceholder(const TCHAR* _moduleName) const
	{
		boost::filesystem::wpath repositoryPath;
		std::_tstring moduleName = _moduleName;
		if (GetRepositoryPath(moduleName, repositoryPath))
		{
			CModuleHelper modHelper(moduleName);
			StdStringVector tokens;
			modHelper.GetQualifiedLabel(tokens);
			ATLASSERT(!tokens.empty());
			StringCMonitorFolderMap::const_iterator found = m_paths.find(tokens[0]);
			if (found != m_paths.end())
			{
				boost::filesystem::wpath path = found->second->m_path.parent_path();
				for (unsigned int i = 0; i < tokens.size(); ++i)
					path /= tokens[i];
				IModule * module = CreateDiskModule(this, moduleName, path, true);
				return module;
			}
		}
		return NULL;
	}

	virtual IModule * InsertModule(const TCHAR* moduleName) const
	{
		StlLinked<IModule> result = 0;

		CModuleHelper modHelper(moduleName);
		StdStringVector tokens;
		modHelper.GetQualifiedLabel(tokens);
		ATLASSERT(!tokens.empty());
		StringCMonitorFolderMap::const_iterator found = m_paths.find(tokens[0]);
		if (found != m_paths.end())
		{
			boost::filesystem::wpath path = found->second->m_path.parent_path();
			if (boost::filesystem::exists(path))	//  Root repository folder
			{
				for (unsigned int i = 0; i < tokens.size(); ++i)
				{
					path /= tokens[i];
					try {
						if (!boost::filesystem::exists(path))
							boost::filesystem::create_directories(path);
					} catch (const boost::filesystem::filesystem_error & ex) {
						_DBGLOG(LEVEL_WARNING, ex.what());
					}

					if (boost::filesystem::exists(path))
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
		algo::replace_all(module, _T("."), _T("/"));
		nameDirectory.append(CT2A(module.c_str()));
		try
		{
			boost::filesystem::path pathDirectory(nameDirectory.c_str(), boost::filesystem::native);
			boost::filesystem::create_directories(pathDirectory);
			if (boost::filesystem::exists(pathDirectory))
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
		boost::filesystem::wpath repositoryPath;
		std::_tstring moduleName = _moduleName;
		if (GetRepositoryPath(moduleName, repositoryPath))
		{
			CModuleHelper modHelper(moduleName + _T(".dummy"));
			std::_tstring module = modHelper.GetModuleLabelNoRoot();
			algo::replace_all(module, _T("."), _T("/"));
			boost::filesystem::wpath path = repositoryPath / module;
			if (boost::filesystem::exists(path))
			{
				boost::filesystem::directory_iterator attr_itr_end;
				for (boost::filesystem::directory_iterator attr_itr(wpathToPath(path)); attr_itr != attr_itr_end; attr_itr++)
				{
					if (!boost::filesystem::is_directory(*attr_itr))
					{  
						std::_tstring label = CA2T(boost::filesystem::basename(*attr_itr).c_str());
						std::_tstring type = CA2T(boost::filesystem::extension(*attr_itr).c_str());
						if (IsValidExtension(type))
						{
							StlLinked<IAttribute> attribute = CreateDiskAttribute(this, moduleName, label, type, pathToWPath(*attr_itr), _T(""), noBroadcast);
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
		return ::GetAttributeTypes(types);
	}

	virtual unsigned FindAttributes(const std::_tstring & searchText, const std::_tstring & module, const std::_tstring & user, SEARCHMODE searchMode, bool sandboxed, bool checkedout, bool locked, bool orphaned, const TCHAR* utcDate, IAttributeVector & attributes) const
	{
		if (CComPtr<IEclCC> eclcc = CreateIEclCC())
		{
			for(StringCMonitorFolderMap::const_iterator itr = m_paths.begin(); itr != m_paths.end(); ++itr) 
			{
				IModuleVector modules;
				IModuleHierarchy moduleHierarchy;
				GetAllModules(itr->second->m_path, _T(""), modules, moduleHierarchy, true, true);

				for (IModuleVector::const_iterator m_itr = modules.begin(); m_itr != modules.end(); ++m_itr)
				{
					if (module.empty() || algo::iequals(module, m_itr->get()->GetQualifiedLabel()))
					{
						IAttributeVector attrs;
						m_itr->get()->GetAttributes(attrs, true);
						for (IAttributeVector::const_iterator a_itr = attrs.begin(); a_itr != attrs.end(); ++a_itr)
						{
							std::_tstring ecl = a_itr->get()->GetText(false, true);
							if (algo::icontains(ecl, searchText))
								attributes.push_back(a_itr->get());
						}
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
		boost::filesystem::wpath repositoryPath;
		std::_tstring moduleName = _moduleName;
		if (GetRepositoryPath(moduleName, repositoryPath))
		{
			CModuleHelper fromHelper(moduleName + _T(".dummy"));
			std::_tstring fromModule = fromHelper.GetModuleLabelNoRoot();
			algo::replace_all(fromModule, _T("."), _T("/"));
			boost::filesystem::wpath fromPath = repositoryPath / fromModule;

			CModuleHelper toHelper(std::_tstring(label) + _T(".dummy"));
			std::_tstring toModule = toHelper.GetModuleLabelNoRoot();
			algo::replace_all(toModule, _T("."), _T("/"));
			boost::filesystem::wpath toPath = repositoryPath / toModule;
			try
			{
				boost::filesystem::rename(fromPath, toPath);
			} catch (const boost::filesystem::filesystem_error & ex) {
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
		boost::filesystem::wpath repositoryPath;
		std::_tstring moduleName = _moduleName;
		if (GetRepositoryPath(moduleName, repositoryPath))
		{
			CModuleHelper modHelper(moduleName + _T(".dummy"));
			std::_tstring module = modHelper.GetModuleLabelNoRoot();
			algo::replace_all(module, _T("."), _T("/"));
			boost::filesystem::wpath path = repositoryPath / module;
			if (boost::filesystem::exists(path))
			{
				try {
					int retVal = MoveToRecycleBin(pathToWString(path));
					if (retVal != 0)
						throw std::exception("Unknown Error During Folder Delete.", retVal);
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

	bool GetRepositoryPath(std::_tstring & module, boost::filesystem::wpath & path) const
	{
		if (!m_amtRoot.empty() && !algo::istarts_with(module, (pathToWString(m_amtRoot.leaf()) + _T("."))))
			module = pathToWString(m_amtRoot.leaf()) + _T(".") + module;

		CModuleHelper modHelper(module);
		StdStringVector tokens;
		modHelper.GetQualifiedLabel(tokens);
		if (tokens.empty())
			return false;

		StringCMonitorFolderMap::const_iterator found = m_paths.find(tokens[0]);
		if (found != m_paths.end())
		{
			path = found->second->m_path;
			if (boost::filesystem::exists(path))
				return true;
		}
		else
		{
			for(StringCMonitorFolderMap::const_iterator itr = m_paths.begin(); itr != m_paths.end(); ++itr)
			{
				if (boost::filesystem::exists(itr->second->m_path / tokens[0]))
				{
					path = itr->second->m_path;
					if (boost::filesystem::exists(path))
					{
						module = itr->first + _T(".") + module;
						return true;
					}
				}
			}
		}
		return false;
	}

	bool GetPath(std::_tstring & moduleName, const std::_tstring & attributeName, IAttributeType * type, boost::filesystem::wpath & path) const
	{
		boost::filesystem::wpath repositoryPath;
		if (GetRepositoryPath(moduleName, repositoryPath))
		{
			CModuleHelper modHelper(moduleName + _T(".") + attributeName);
			std::_tstring module = modHelper.GetModuleLabelNoRoot();
			algo::replace_all(module, _T("."), _T("/"));
			std::wstring attr = attributeName;
			attr += _T(".");
			attr += type->GetRepositoryCode();
			path = repositoryPath / module / attr;
			if (boost::filesystem::exists(path))
				return true;
		}
		return false;
	}

	virtual IAttribute * GetAttribute(const TCHAR* _module, const TCHAR* attribute, IAttributeType * type, int version = 0, bool sandbox = true, bool text = true, bool noBroadcast = false) const
	{
		IAttribute* result = NULL;

		if (0 == attribute)
			return result;

		boost::filesystem::wpath path;
		std::_tstring module = _module;
		if (!GetPath(module, attribute, type, path))
			return result;

		if (boost::filesystem::exists(path))
		{
			CUnicodeFile file;
			file.Open(pathToWString(path).c_str());
			if (file.IsOpen())
			{
				std::_tstring data;
				file.Read(data);
				file.Close();
				result = ::CreateDiskAttribute(this, module, attribute, type->GetRepositoryCode(), path, data, noBroadcast);
				return result;
			}
		}
		return result;
	}

	virtual IAttribute * GetAttributeFast(const TCHAR* module, const TCHAR* attribute, IAttributeType * type, int version = 0, bool sandbox = true, bool text = false, bool noBroadcast = false) const
	{
		IAttribute * retVal = ::GetDiskAttribute(this, module, attribute, type, version, sandbox);
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
		boost::filesystem::wpath repositoryPath;
		std::_tstring moduleName = _moduleName;
		if (GetRepositoryPath(moduleName, repositoryPath))
		{
			CModuleHelper modHelper(moduleName + _T(".") + attributeName);
			std::_tstring module = modHelper.GetModuleLabelNoRoot();
			algo::replace_all(module, _T("."), _T("/"));
			std::_tstring fileName = attributeName + type->GetFileExtension();
			boost::filesystem::wpath path = repositoryPath / module / fileName;
			if (!boost::filesystem::exists(path))
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
		//  Check moduleAttribute contains full path
		for(StringCMonitorFolderMap::const_iterator itr = m_paths.begin(); itr != m_paths.end(); ++itr) 
		{
			std::_tstring knownPath = pathToWString(itr->second->m_path);
			if (boost::algorithm::istarts_with(modAttr, knownPath))
			{
				modAttr = itr->first + modAttr.substr(knownPath.length());
				break;
			}
		}
		//  Convert moduleAttribute partial path to mod.attr
		if (algo::iends_with(modAttr, _T(".ecl")))
			modAttr.resize(modAttr.size() - strlen(".ecl"));
		else if (algo::iends_with(modAttr, _T(".eclattr")))
			modAttr.resize(modAttr.size() - strlen(".eclattr"));
		algo::replace_all(modAttr, _T("\\"), _T("."));

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
		StringCMonitorFolderMap::const_iterator found = m_paths.find(tokens[0]);
		if (found != m_paths.end())
		{
			boost::filesystem::wpath path = found->second->m_path.parent_path();
			if (boost::filesystem::exists(path))	//  Root repository folder
			{
				for (unsigned int i = 0; i < tokens.size(); ++i)
				{
					path /= tokens[i];
					try {
						if (!boost::filesystem::exists(path))
							boost::filesystem::create_directories(path);
					} catch (const boost::filesystem::filesystem_error & ex) {
						_DBGLOG(LEVEL_WARNING, ex.what());
					}
				}
				std::_tstring filename = attribute;
				filename += _T(".");
				filename += type->GetRepositoryCode();
				path /= filename;

				CUnicodeFile file;
				file.Create(pathToWString(path).c_str(),GENERIC_WRITE,CREATE_ALWAYS);
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
			boost::filesystem::wpath fromPath;
			std::_tstring fromModule = itr->get()->GetModuleQualifiedLabel();
			GetPath(fromModule, itr->get()->GetLabel(), itr->get()->GetType(), fromPath);
			if (boost::filesystem::exists(fromPath))
			{
				boost::filesystem::wpath toPath;
				std::_tstring toModule = module;
				GetPath(toModule, itr->get()->GetLabel(), itr->get()->GetType(), toPath);
				try
				{
					boost::filesystem::rename(fromPath, toPath);
				} catch (const boost::filesystem::filesystem_error & ex) {
					_DBGLOG(LEVEL_WARNING, ex.what());
				}
				CComPtr<IAttribute> attribute = CreateDiskAttribute(this, toModule, itr->get()->GetLabel(), itr->get()->GetType()->GetRepositoryCode(), toPath, itr->get()->GetText(), false);
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

	virtual const boost::filesystem::path & GetEnvironmentFolder(boost::filesystem::path & path) const
	{
		if (CComPtr<IEclCC> eclcc = CreateIEclCC())
			path = stringToPath(eclcc->GetWorkingFolder());
		else
		{		
			boost::filesystem::path userFolder;
			path = GetUserFolder(userFolder, GetUserId());/*  GJS / boost::filesystem::path(CT2A(GetLabel()), boost::filesystem::native);*/
		}

		try {
			boost::filesystem::create_directories(path);
		} catch (const boost::filesystem::filesystem_error & ex) {
			_DBGLOG(LEVEL_WARNING, ex.what());
		}

		return path;
	}

	void Update(IEclCC * eclcc)
	{
		ATLASSERT(eclcc);
		if (!eclcc)
			return;

		for (int i = 0; i < eclcc->GetEclFolderCount(); ++i)
		{
			boost::filesystem::wpath path = boost::filesystem::wpath(eclcc->GetEclFolder(i), boost::filesystem::native);
			if (boost::filesystem::exists(path) && boost::filesystem::is_directory(path))
			{
				m_paths[pathToWString(path.leaf())] = new CMonitorFolder(this, wpathToPath(path));
				m_pathOrder.push_back(path);
			}
		}
	}

	void Update(const TCHAR* url)
	{
		m_amtRoot = boost::filesystem::wpath(url, boost::filesystem::native);
		ATLASSERT(boost::filesystem::exists(m_amtRoot));
		m_paths[pathToWString(m_amtRoot.leaf())] = new CMonitorFolder(this, wpathToPath(m_amtRoot));
		m_pathOrder.push_back(m_amtRoot);
	}

};
static CacheT<std::_tstring, CDiskRepository> DiskRepositoryCache;
//  ===========================================================================
void ClearDiskRepositorySingletons()
{
	ClearDiskAttributeCache();
	ClearDiskModuleCache();
	DiskRepositoryCache.Clear();
}

COMMS_API IRepository * AttachDiskRepository(const TCHAR * label, bool bClearCache)
{
	CComPtr<CDiskRepository> rep = new CDiskRepository(label);
	if ( bClearCache )
		DiskRepositoryCache.Clear(rep);
	CDiskRepository * retVal = DiskRepositoryCache.Exists(rep->GetCacheID());
	if (!retVal)
	{
		retVal = DiskRepositoryCache.Get(rep);
		retVal->Update(CreateIEclCC());
	}
	return retVal;
}

COMMS_API IRepository * AttachDiskRepository(const TCHAR* url, const TCHAR * label, bool bClearCache)
{
	CDiskRepository *rep = new CDiskRepository(label);
	if ( bClearCache )
		DiskRepositoryCache.Clear(rep);
	CDiskRepository * retVal = DiskRepositoryCache.Exists(rep->GetCacheID());
	if (!retVal)
	{
		retVal = DiskRepositoryCache.Get(rep);
		retVal->Update(url);
	}
	return retVal;
}
