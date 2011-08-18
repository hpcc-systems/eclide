#include "stdafx.h"

#include "global.h"
#include "CmdLine.h"
#include "RecursiveMutex.h"
#include "Cache.h"

const TCHAR * const COMPANY = _T("HPCC Systems");
const TCHAR * const CFG = _T("cfg");
const TCHAR * const INI = _T("ini");

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)

std::_tstring g_appName;
CLIB_API void SetApplicationName(const std::_tstring & appName)
{
	g_appName = appName;
}

namespace global
{
CLIB_API const TCHAR * GetApplicationName()
{
	return g_appName.c_str();
}
}

CLIB_API const boost::filesystem::path & GetProgramPath(boost::filesystem::path & path)
{
	TCHAR programPath[_MAX_PATH];
	::GetModuleFileName(0, programPath, _MAX_PATH);
	path = boost::filesystem::path(CT2A(programPath), boost::filesystem::native);
	if(!g_appName.empty())
	{
		path = path.branch_path() / boost::filesystem::path(CT2A(g_appName.c_str()), boost::filesystem::native);
	}
	return path;
}
CLIB_API const boost::filesystem::path & GetProgramFolder(boost::filesystem::path & path)
{
	boost::filesystem::path p;
	path = GetProgramPath(p).branch_path();
	return path;
}
CLIB_API const boost::filesystem::path & GetAppDataFolder(boost::filesystem::path & path)
{
	TCHAR appDataPath[_MAX_PATH];
	SHGetSpecialFolderPath(NULL, appDataPath, CSIDL_APPDATA, true); 
	path = boost::filesystem::path(CT2A(appDataPath), boost::filesystem::native);
	boost::filesystem::create_directories(path);
	return path;
}
CLIB_API const boost::filesystem::path & GetMyDocumentsFolder(boost::filesystem::path & path)
{
	TCHAR appDataPath[_MAX_PATH];
	SHGetSpecialFolderPath(NULL, appDataPath, CSIDL_MYDOCUMENTS, true); 
	path = boost::filesystem::path(CT2A(appDataPath), boost::filesystem::native);
	boost::filesystem::create_directories(path);
	return path;
}
CLIB_API const boost::filesystem::path & GetCompanyFolder(boost::filesystem::path & path)
{
	boost::filesystem::path p;
	path = GetAppDataFolder(p) / boost::filesystem::path(CT2A(COMPANY), boost::filesystem::native);
	boost::filesystem::create_directories(path);
	return path;
}
CLIB_API const boost::filesystem::path & GetApplicationFolder(boost::filesystem::path & path)
{
	CCmdLineParser parser(::GetCommandLine());
	if (parser.HasKey(_T("WF")))
	{
		std::_tstring workspace = parser.GetVal(_T("WF"));
		path = boost::filesystem::path(CT2A(workspace.c_str()), boost::filesystem::native);
	}
	else
	{
		boost::filesystem::path programPath;
		std::string leaf = GetProgramPath(programPath).leaf();
		TCHAR szFileName[_MAX_FNAME];
		TCHAR szExt[_MAX_FNAME];
		_tsplitpath(CA2T(leaf.c_str()), NULL, NULL, szFileName, szExt);		
		boost::filesystem::path companyFolder;
		path = GetCompanyFolder(companyFolder) / boost::filesystem::path(CT2A(szFileName), boost::filesystem::native);
	}
	boost::filesystem::create_directories(path);
	return path;
}
CLIB_API const boost::filesystem::path & GetUserFolder(boost::filesystem::path & path, const std::_tstring & user)
{
	boost::filesystem::path applicationFolder;
	path = GetApplicationFolder(applicationFolder) / boost::filesystem::path(CT2A(user.c_str()), boost::filesystem::native);
	boost::filesystem::create_directories(path);
	return path;
}
CLIB_API const boost::filesystem::path & GetEnvironmentFolder(boost::filesystem::path & path, const std::_tstring & user, const std::_tstring & environment)
{
	boost::filesystem::path userFolder;
	path = GetUserFolder(userFolder, user) / boost::filesystem::path(CT2A(environment.c_str()), boost::filesystem::native);
	boost::filesystem::create_directories(path);
	return path;
}
CLIB_API const boost::filesystem::path & GetConfigPath(const std::_tstring & environment, boost::filesystem::path & path)
{
	boost::filesystem::path applicationFolder;
	std::_tstring CfgFileName = environment;
	if (CfgFileName.find('.') == std::_tstring::npos)
	{
		CfgFileName += _T(".");
		CfgFileName += CFG;
	}
	path = GetApplicationFolder(applicationFolder) / boost::filesystem::path(CT2A(CfgFileName.c_str()), boost::filesystem::native);
	return path;
}
CLIB_API const boost::filesystem::path & GetIniPath(boost::filesystem::path & path)
{
	boost::filesystem::path programPath;
	std::string leaf = GetProgramPath(programPath).leaf();
	TCHAR szFileName[_MAX_FNAME];
	TCHAR szExt[_MAX_FNAME];
	_tsplitpath(CA2T(leaf.c_str()), NULL, NULL, szFileName, szExt);
	std::_tstring iniName = szFileName;
	iniName += _T(".");
	iniName += INI;
	boost::filesystem::path appFolder;
	path = GetApplicationFolder(appFolder) / boost::filesystem::path(CT2A(iniName.c_str()), boost::filesystem::native);
	return path;
}

class CConfig : 
	public IConfig, 
	public IAuthenticate, 
	public IInternetSecurityManager,
	public IServiceProviderImpl<CConfig>,
	public CUnknown,
	public boost::noncopyable
{
protected:
	std::_tstring m_id;
	boost::filesystem::path m_CfgPath;
	CIniFile m_Cfg;
	std::_tstring m_CfgName;
	typedef std::map<SectionLabel, _variant_t> ConfigCacheT;
	ConfigCacheT m_cache;

	globaldata_signal_type on_refresh;
	mutable clib::recursive_mutex m_mutex;

public:
	IMPLEMENT_CUNKNOWNNOQI; //QueryInterface is specialized

	CConfig()
	{
		ATLASSERT(false);
	}
	CConfig(const std::_tstring & id, const boost::filesystem::path & path) : m_id(id), m_CfgPath(path)
	{
		TCHAR szFileName[_MAX_FNAME];
		TCHAR szExt[_MAX_FNAME];
		_tsplitpath(CA2T(m_CfgPath.native_file_string().c_str()), NULL, NULL, szFileName, szExt);
		if (szExt[0] == '.')
		{
			InitConfigPath(szFileName, &szExt[1]);
		}
		else
		{
			InitConfigPath(szFileName, szExt);
		}
	}
	~CConfig()
	{
	}
	const TCHAR * GetID()
	{
		return m_id.c_str();
	}
	const TCHAR * GetCacheID()
	{
		return m_id.c_str();
	}
	void InitConfigPath(const std::_tstring & cfgName, const std::_tstring & ext)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		m_CfgName = cfgName;
		ATLASSERT((cfgName.find('.') == std::_tstring::npos));
		ATLASSERT((ext.find('.') == std::_tstring::npos));
		std::_tstring cfgNameExt = cfgName;
		cfgNameExt += _T(".");
		cfgNameExt += ext;
		boost::filesystem::path appFolder;
		m_CfgPath = GetApplicationFolder(appFolder) / boost::filesystem::path(CT2A(cfgNameExt.c_str()), boost::filesystem::native);
		m_Cfg.SetFilename(CA2T(m_CfgPath.native_file_string().c_str()));
	}
	/*
	void InitUserFolder(const std::_tstring & user)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		TCHAR szFileName[_MAX_FNAME];
		TCHAR szExt[_MAX_FNAME];
		_tsplitpath(user.c_str(), NULL, NULL, szFileName, szExt);		
		ATLASSERT(!m_ApplicationFolder.empty());
		m_UserFolder = m_ApplicationFolder / boost::filesystem::path(CT2A(szFileName), boost::filesystem::native);
		boost::filesystem::create_directories(m_UserFolder);
	}
	void InitEnvironmentFolder(const std::_tstring & environment)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		TCHAR szFileName[_MAX_FNAME];
		TCHAR szExt[_MAX_FNAME];
		_tsplitpath(environment.c_str(), NULL, NULL, szFileName, szExt);		
		ATLASSERT(!m_UserFolder.empty());
		m_EnvironmentFolder = m_UserFolder / boost::filesystem::path(CT2A(szFileName), boost::filesystem::native);
		boost::filesystem::create_directories(m_EnvironmentFolder);
	}
	const boost::filesystem::path * GetProgramFolder()
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return &m_ProgramFolder;
	}
	const boost::filesystem::path * GetCompanyFolder()
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return &m_CompanyFolder;
	}
	const boost::filesystem::path * GetApplicationFolder()
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return &m_ApplicationFolder;
	}
	const boost::filesystem::path * GetUserFolder()
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return &m_UserFolder;
	}
	const boost::filesystem::path * GetEnvironmentFolder()
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return &m_EnvironmentFolder;
	}
	*/
	void Clear()
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		m_cache.clear();
	}
	void Reset(const CString & newLabel)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		boost::filesystem::remove(m_CfgPath);
		SetLabel(static_cast<const TCHAR *>(newLabel), _T("cfg"));
	}
	void SaveAs(const CString & newLabel)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		boost::filesystem::path fpFrom = m_CfgPath;
		SetLabel(static_cast<const TCHAR *>(newLabel), _T("cfg"));
		boost::filesystem::path fpTo = m_CfgPath;
		boost::filesystem::copy_file(fpFrom,fpTo);
	}
	virtual void SetLabel(const std::_tstring & label, const std::_tstring & ext)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		InitConfigPath(label, ext);
		m_cache.clear();
	}
	const TCHAR * GetLabel()
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_CfgName.c_str();
	}
	const boost::filesystem::path & GetEnvironmentFolder(boost::filesystem::path & path)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return ::GetEnvironmentFolder(path, static_cast<const TCHAR *>(CString(Get(GLOBAL_USER))), m_CfgName);
	}
	_variant_t Get(const std::_tstring & section, const std::_tstring & label, const _variant_t & defaultVal)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		SectionLabelDefault sl(SectionLabel(section, label), defaultVal);
		ConfigCacheT::const_iterator found = m_cache.find(sl.first);
		if(sl == GLOBAL_USER && found != m_cache.end())
			if (!CString(m_cache[sl.first]).IsEmpty())
				return m_cache[sl.first];
		if(sl == GLOBAL_PASSWORD && found != m_cache.end())
			if (!CString(m_cache[sl.first]).IsEmpty())
				return m_cache[sl.first];

		if (!m_CfgPath.empty())
		{
			if (found != m_cache.end())
				return found->second;

			CString _defaultVal = defaultVal, _val;
			if (!m_Cfg.GetString(section.c_str(), label.c_str(), _val, _defaultVal))
			{
				//ATLTRACE(_T("GetString(%s,%s,Err=%x)\r\n"), section, label, ::GetLastError() );
				_val = _defaultVal;
			}
			SectionLabel id(section, label);
			if (CString(m_cache[id]).CompareNoCase(_val) != 0)
			{
				m_cache[id] = _val;
				_variant_t retVal = m_cache[sl.first];
				proc.unlock();
				on_refresh(&id);
				return retVal;
			}
		}
		return m_cache[sl.first];
	}
	_variant_t Get(const SectionLabel & sectionLabel, const _variant_t & defaultVal)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return Get(sectionLabel.first, sectionLabel.second, defaultVal);
	}
	_variant_t Get(const SectionLabelDefault & sectionLabelDefault)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return Get(sectionLabelDefault.first, sectionLabelDefault.second);
	}
	void Set(const std::_tstring & section, const std::_tstring & label, const _variant_t & val)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		SectionLabelDefault sl(SectionLabel(section, label), _T(""));
		if(sl == GLOBAL_USER)
		{
			InitConfigPath(m_CfgName, _T("cfg"));
		}
		SectionLabel id(section, label);
		if (m_cache.find(id) == m_cache.end() || CString(m_cache[id]).CompareNoCase(CString(val)) != 0)
		{
			if (!m_CfgPath.empty() && sl != GLOBAL_PASSWORD)
			{
				CString _val = val;
				if ( !m_Cfg.PutString(section.c_str(), label.c_str(), _val) )
				{
					ATLTRACE(_T("PutString(%s,%s,Err=%x)\r\n"), section, label, ::GetLastError() );
				}
			}
			m_cache[id] = val;
			proc.unlock();
			on_refresh(&id);
		}
	}
	void Set(const SectionLabel & sectionLabel, const _variant_t & val)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		Set(sectionLabel.first, sectionLabel.second, val);
	}
	void Set(const SectionLabelDefault & sectionLabelDefault, const _variant_t & val)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		Set(sectionLabelDefault.first, val);
	}
	void Set(const SectionLabelDefault & sectionLabelDefault, const TCHAR * val)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		_variant_t variant(val);
		Set(sectionLabelDefault.first, variant);
	}
	void Set(const SectionLabelDefault & sectionLabelDefault, const std::_tstring & val)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		_variant_t variant(val.c_str());
		Set(sectionLabelDefault.first, variant);
	}
	boost::signals::connection ConnectSlot(const globaldata_slot_type & slot)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return on_refresh.connect(slot);
	}
	void Disconnect(boost::signals::connection& slot)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		slot.disconnect();
	}
    virtual HRESULT STDMETHODCALLTYPE Authenticate( 
            /* [out] */ HWND *phwnd,
            /* [out] */ LPWSTR *pszUsername,
            /* [out] */ LPWSTR *pszPassword)
	{
		*phwnd = 0;
		bstr_t user(Get(GLOBAL_USER));
		size_t size = sizeof(WCHAR) * (user.length()+1);
		*pszUsername = (LPWSTR)::CoTaskMemAlloc(size);
		memcpy(*pszUsername, (const wchar_t *)user, size);
		bstr_t password(Get(GLOBAL_PASSWORD));
		size = sizeof(WCHAR) * (password.length()+1);
		*pszPassword = (LPWSTR)::CoTaskMemAlloc(size);
		memcpy(*pszPassword, (const wchar_t *)password, size);

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE SetSecuritySite(IInternetSecurityMgrSite *)
	{
		return INET_E_DEFAULT_ACTION;
	}
	HRESULT STDMETHODCALLTYPE GetSecuritySite(IInternetSecurityMgrSite **)
	{
		return INET_E_DEFAULT_ACTION;
	}
	HRESULT STDMETHODCALLTYPE MapUrlToZone(LPCWSTR,DWORD *,DWORD)
	{
		return INET_E_DEFAULT_ACTION;
	}
	HRESULT STDMETHODCALLTYPE GetSecurityId(LPCWSTR,BYTE *,DWORD *,DWORD_PTR)
	{
		return INET_E_DEFAULT_ACTION;
	}
	HRESULT STDMETHODCALLTYPE ProcessUrlAction(LPCWSTR pwszUrl, DWORD dwAction, BYTE * pPolicy, DWORD cbPolicy,BYTE * pContext, DWORD cbContext, DWORD dwFlags, DWORD dwReserved)
	{
		return INET_E_DEFAULT_ACTION;
	}
	HRESULT STDMETHODCALLTYPE QueryCustomPolicy(LPCWSTR,const GUID &,BYTE **,DWORD *,BYTE *,DWORD,DWORD)
	{
		return INET_E_DEFAULT_ACTION;
	}
	HRESULT STDMETHODCALLTYPE SetZoneMapping(DWORD,LPCWSTR,DWORD)
	{
		return INET_E_DEFAULT_ACTION;
	}
	HRESULT STDMETHODCALLTYPE GetZoneMappings(DWORD,IEnumString **,DWORD)
	{
		return INET_E_DEFAULT_ACTION;
	}

	BEGIN_SERVICE_MAP(CConfig)
		SERVICE_ENTRY(IID_IAuthenticate)
		SERVICE_ENTRY(IID_IInternetSecurityManager)
	END_SERVICE_MAP()

	STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject)
	{
		HRESULT hr = E_NOINTERFACE;
		
		if (ppvObject != NULL)
		{
			*ppvObject = NULL;

			if (IID_IUnknown == riid)
				*ppvObject = static_cast<IUnknown *>(static_cast<CUnknown *>(this));

			else if (IID_IAuthenticate == riid)
				*ppvObject = static_cast<IAuthenticate*>(this);

			else if (IID_IInternetSecurityManager == riid)
				*ppvObject = static_cast<IInternetSecurityManager*>(this);

			else if (IID_IServiceProvider == riid)
				*ppvObject = static_cast<IServiceProvider*>(this);

			if (*ppvObject != NULL)
			{
				hr = S_OK;
				((LPUNKNOWN)*ppvObject)->AddRef();
			}
		}
		else
		{
			hr = E_POINTER;
		}
		
		return hr;
	}

};

const TCHAR * const GLOBAL = _T("___GLOBAL_CONFIG_DEFAULT_LABEL__");
CacheT<std::_tstring, CConfig> ConfigCache;

/*
CConfig * CreateConfig(HINSTANCE hInstance, const TCHAR* altWorkspace)
{
	std::_tstring workspace;
	bool bRelative = true;
	CCmdLineParser parser(::GetCommandLine());
	if (parser.HasKey(_T("WF")))
	{
		workspace = parser.GetVal(_T("WF"));
		bRelative = false;
	}
	else if ( altWorkspace )
	{
		workspace = altWorkspace;
	}
	return new CConfig(GLOBAL, workspace, bRelative);
}

CLIB_API IConfig * CreateIConfig(HINSTANCE hInstance, const TCHAR* altWorkspace)
{
	g_hInstance = hInstance;
	return ConfigCache.Get(CreateConfig(hInstance,altWorkspace));
}

CLIB_API IConfig * CreatePrivateIConfig(const TCHAR* altWorkspace)
{
	ATLASSERT(g_hInstance);
	return CreateConfig(g_hInstance, altWorkspace);
}

CLIB_API IConfig * CreateIniIConfig(HINSTANCE hInstance, const TCHAR* altWorkspace)
{
	g_hInstance = hInstance;
	return ConfigCache.Get(new CConfig(GLOBAL, altWorkspace));
}

CLIB_API IConfig * CreatePrivateIniIConfig(const TCHAR* altWorkspace)
{
	return CreateConfig(g_hInstance,altWorkspace);
}

CLIB_API void ChangeIConfig(const CString & label)
{
	ATLASSERT(ConfigCache.find(GLOBAL) != ConfigCache.end());
	ConfigCache[GLOBAL]->SetLabel(label);
}

CLIB_API void DeleteAndChangeIConfig(const CString & label)
{
	ATLASSERT(ConfigCache.find(GLOBAL) != ConfigCache.end());
	ConfigCache[GLOBAL]->Reset(label);
}

CLIB_API void SaveAsIConfig(const CString & newLabel)
{
	ATLASSERT(ConfigCache.find(GLOBAL) != ConfigCache.end());
	ConfigCache[GLOBAL]->SaveAs(newLabel);
}

CLIB_API IAuthenticate * GetIAuthenticate()
{
	ATLASSERT(ConfigCache.find(GLOBAL) != ConfigCache.end());
	return ConfigCache[GLOBAL];
}
*/

CLIB_API IConfig * CreateIConfig(const std::_tstring & id, const boost::filesystem::path & path)
{
	CConfig * retVal = ConfigCache.Get(new CConfig(id, path));
	TCHAR szFileName[_MAX_FNAME];
	TCHAR szExt[_MAX_FNAME];
	_tsplitpath(CA2T(path.leaf().c_str()), NULL, NULL, szFileName, szExt);		
	if (szExt[0] == '.')
		retVal->InitConfigPath(szFileName, &szExt[1]);
	else
		retVal->InitConfigPath(szFileName, &szExt[0]);
	retVal->Clear();
	return retVal;
}

CLIB_API IConfig * CreateIConfig(const std::_tstring & id, const std::_tstring & env)
{
	boost::filesystem::path defaultCfg;
	GetConfigPath(env, defaultCfg);
	return CreateIConfig(id, defaultCfg);
}

CLIB_API IConfig * CreateIConfig(const std::_tstring & id)
{
	return CreateIConfig(id, std::_tstring(_T("default")));
}

CLIB_API IConfig * GetIConfig(const std::_tstring & id)
{
	ATLASSERT(ConfigCache.find(id) != ConfigCache.end());
	return ConfigCache[id];
}

CLIB_API IServiceProvider * GetIServiceProvider(const std::_tstring & id)
{
	ATLASSERT(ConfigCache.find(id) != ConfigCache.end());
	return ConfigCache[id];
}
