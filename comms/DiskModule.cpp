#include "StdAfx.h"

#include "module.h"
#include "DiskRepository.h"
#include "clib.h"
#include "cache.h"
#include "ModuleHelper.h"
#include <Aclapi.h>
#include <authz.h>
#include <UtilFilesystem.h>

//  ===========================================================================
class CFileAccess
{
public:
    bool m_read;
    bool m_write;
    bool m_execute;

protected:
    LPTSTR lpServerName;

public:
    CFileAccess(const std::_tstring & path)
    {
        m_read = false;
        m_write = false;
        m_execute = false;
        lpServerName = NULL;
        TCHAR userName[1024];
        ULONG userNameSize = sizeof(userName);
        GetUserName(userName, &userNameSize);

        DWORD                dw;
        PACL                 pacl;
        PSECURITY_DESCRIPTOR psd;
        PSID                 psid = NULL; 

        dw = GetNamedSecurityInfo(path.c_str(), SE_FILE_OBJECT, DACL_SECURITY_INFORMATION | OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION, NULL, NULL, &pacl, NULL, &psd);
        if (dw != ERROR_SUCCESS)
        {  
            printf("couldn't do getnamedsecinfo \n");
            DisplayError("GetNamedSecurityInfo", dw);
        }

        UseAuthzSolution(psd, userName);

        if(psid != NULL)
        {
            LocalFree(psid);
            psid = NULL;
        };

        LocalFree(psd);
    }

protected:
    PSID ConvertNameToBinarySid(LPTSTR pAccountName)
    {
        LPTSTR pDomainName = NULL;
        DWORD dwDomainNameSize = 0;
        PSID pSid = NULL;
        DWORD dwSidSize = 0;
        SID_NAME_USE sidType;
        BOOL fSuccess = FALSE;
        HRESULT hr = S_OK;

        __try
        {
            LookupAccountName(
                lpServerName,      // look up on local system
                pAccountName,
                pSid,              // buffer to receive name
                &dwSidSize,
                pDomainName,
                &dwDomainNameSize,
                &sidType);

            //  If the Name cannot be resolved, LookupAccountName will fail with
            //  ERROR_NONE_MAPPED.
            if (GetLastError() == ERROR_NONE_MAPPED)
            {
                wprintf_s(_T("LookupAccountName failed with %d\n"), GetLastError());
                __leave;
            }
            else if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
            {
                pSid = (LPTSTR)LocalAlloc(LPTR, dwSidSize * sizeof(TCHAR));
                if (pSid == NULL)
                {
                    wprintf_s(_T("LocalAlloc failed with %d\n"), GetLastError());
                    __leave;
                }

                pDomainName = (LPTSTR)LocalAlloc(LPTR, dwDomainNameSize * sizeof(TCHAR));
                if (pDomainName == NULL)
                {
                    wprintf_s(_T("LocalAlloc failed with %d\n"), GetLastError());
                    __leave;
                }

                if (!LookupAccountName(
                    lpServerName,      // look up on local system
                    pAccountName,
                    pSid,              // buffer to receive name
                    &dwSidSize,
                    pDomainName,
                    &dwDomainNameSize,
                    &sidType))
                {
                    wprintf_s(_T("LookupAccountName failed with %d\n"), GetLastError());
                    __leave;
                }
            }

            //  Any other error code
            else
            {
                wprintf_s(_T("LookupAccountName failed with %d\n"), GetLastError());
                __leave;
            }

            fSuccess = TRUE;
        }
        __finally
        {
            if(pDomainName != NULL)
            {
                LocalFree(pDomainName);
                pDomainName = NULL;
            }

            //  Free pSid only if failed;
            //  otherwise, the caller has to free it after use.
            if (fSuccess == FALSE)
            {
                if(pSid != NULL)
                {
                    LocalFree(pSid);
                    pSid = NULL;
                }
            }
        }

        return pSid;
    }


    void DisplayError(char* pszAPI, DWORD dwError)
    {
        LPTSTR lpvMessageBuffer = NULL;

        if (!FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_FROM_SYSTEM,
            GetModuleHandle(L"Kernel32.dll"), dwError, 
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),  // the user default language
            (LPTSTR)&lpvMessageBuffer, 0, NULL))
        {
            wprintf_s(L"FormatMessage failed with %d\n", GetLastError());
            ExitProcess(GetLastError());
        }

        //  ...now display this string.
        wprintf_s(L"ERROR: API        = %s.\n", (TCHAR *)CA2T(pszAPI));
        wprintf_s(L"       error code = %08X.\n", dwError);
        wprintf_s(L"       message    = %s.\n", lpvMessageBuffer);

        //  Free the buffer allocated by the system.
        LocalFree(lpvMessageBuffer);

        ExitProcess(GetLastError());
    }

    void DisplayAccessMask(ACCESS_MASK Mask)
    {
        if (((Mask & GENERIC_ALL) == GENERIC_ALL) || ((Mask & FILE_ALL_ACCESS) == FILE_ALL_ACCESS))
        {
            m_read = true;
            m_write = true;
            m_execute = true;
            return;
        }
        if (((Mask & GENERIC_READ) == GENERIC_READ) || ((Mask & FILE_GENERIC_READ) == FILE_GENERIC_READ))
            m_read = true;
        if (((Mask & GENERIC_WRITE) == GENERIC_WRITE) || ((Mask & FILE_GENERIC_WRITE) == FILE_GENERIC_WRITE))
            m_write = true;
        if (((Mask & GENERIC_EXECUTE) == GENERIC_EXECUTE) || ((Mask & FILE_GENERIC_EXECUTE) == FILE_GENERIC_EXECUTE))
            m_execute = true;

    }

    void GetAccess(AUTHZ_CLIENT_CONTEXT_HANDLE hAuthzClient, PSECURITY_DESCRIPTOR psd)
    {
        AUTHZ_ACCESS_REQUEST AccessRequest = {0};
        AUTHZ_ACCESS_REPLY AccessReply = {0};
        BYTE     Buffer[1024];
        BOOL bRes = FALSE;  // assume error

        //  Do AccessCheck.
        AccessRequest.DesiredAccess = MAXIMUM_ALLOWED;
        AccessRequest.PrincipalSelfSid = NULL;
        AccessRequest.ObjectTypeList = NULL;
        AccessRequest.ObjectTypeListLength = 0;
        AccessRequest.OptionalArguments = NULL; 

        RtlZeroMemory(Buffer, sizeof(Buffer));
        AccessReply.ResultListLength = 1;
        AccessReply.GrantedAccessMask = (PACCESS_MASK) (Buffer);
        AccessReply.Error = (PDWORD) (Buffer + sizeof(ACCESS_MASK));


        if (!AuthzAccessCheck( 0,
            hAuthzClient,
            &AccessRequest,
            NULL,
            psd,
            NULL,
            0,
            &AccessReply,
            NULL) ) {
                wprintf_s(_T("AuthzAccessCheck failed with %d\n"), GetLastError());
        }
        else 
            DisplayAccessMask(*(PACCESS_MASK)(AccessReply.GrantedAccessMask));

        return;
    }

    BOOL GetEffectiveRightsForUser(AUTHZ_RESOURCE_MANAGER_HANDLE hManager,
        PSECURITY_DESCRIPTOR psd,
        LPTSTR lpszUserName)
    {
        PSID pSid = NULL;
        BOOL bResult = FALSE;
        LUID unusedId = { 0 };
        AUTHZ_CLIENT_CONTEXT_HANDLE hAuthzClientContext = NULL;

        pSid = ConvertNameToBinarySid(lpszUserName);
        if (pSid != NULL)
        {
            bResult = AuthzInitializeContextFromSid(0,
                pSid,
                hManager,
                NULL,
                unusedId,
                NULL,
                &hAuthzClientContext);
            if (bResult)
            {
                GetAccess(hAuthzClientContext, psd);
                AuthzFreeContext(hAuthzClientContext);
            }
            else
                wprintf_s(_T("AuthzInitializeContextFromSid failed with %d\n"), GetLastError());
        }
        if(pSid != NULL)
        {
            LocalFree(pSid);
            pSid = NULL;
        }

        return bResult;
    }

    void UseAuthzSolution(PSECURITY_DESCRIPTOR psd, LPTSTR lpszUserName)
    {
        AUTHZ_RESOURCE_MANAGER_HANDLE hManager;
        BOOL bResult = FALSE;

        bResult = AuthzInitializeResourceManager(AUTHZ_RM_FLAG_NO_AUDIT,
            NULL, NULL, NULL, NULL, &hManager);
        if (bResult)
        {
            bResult = GetEffectiveRightsForUser(hManager, psd, lpszUserName);
            AuthzFreeResourceManager(hManager);
        }
        else
            wprintf_s(_T("AuthzInitializeResourceManager failed with %d\n"), GetLastError());
    }
};
//  ===========================================================================

#if _COMMS_VER < 68200
using namespace WsAttributes;
#else
#endif

//  ===========================================================================
class CDiskModule;
void DeleteModule(CDiskModule * module);

class CDiskModule : public IModule, public clib::CLockableUnknown
{
protected:
    IRepository * m_repository;
    CComPtr<IModule> m_parent;
    boost::filesystem::wpath m_path;
    std::_tstring m_pathStr;
    CString m_url;
    CString m_qualifiedLabel;
    CString m_qualifiedLabelNoRoot;
    CString m_labelLeaf;
    CString m_id;
    bool m_plugin;
    CString m_checksum;
    SecAccessFlags m_access;

    module_refresh_signal_type on_refresh;

public:
    IMPLEMENT_CLOCKABLEUNKNOWN;

    CDiskModule(const IRepository *rep, const IModule * parent, const boost::filesystem::wpath & path, const std::_tstring & label, const std::_tstring & labelLeaf) : m_qualifiedLabel(label.c_str()), m_labelLeaf(labelLeaf.c_str())
    {
        m_repository = const_cast<IRepository *>(rep);
        m_parent = const_cast<IModule *>(parent);
        m_path = path;
        m_pathStr = pathToWString(path);
        m_url = pathToWString(path).c_str();
        m_id = m_url;
        m_id += _T("/") + m_qualifiedLabel;
        m_id.MakeLower();
        m_plugin = false;
        m_access = SecAccess_Full;

        CModuleHelper modHelper(m_qualifiedLabel + _T(".dummy"));
        m_qualifiedLabelNoRoot = modHelper.GetModuleLabelNoRoot();
    }

    ~CDiskModule()
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
        //clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_labelLeaf;
    }

    const TCHAR *GetQualifiedLabel(bool excludeRoot = false) const
    {
        //clib::recursive_mutex::scoped_lock proc(m_mutex);
        if (excludeRoot)
            return m_qualifiedLabelNoRoot;
        return m_qualifiedLabel;
    }

    const TCHAR *GetPath() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_pathStr.c_str();
    }

    const SecAccessFlags GetAccess() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_access;
    }

    IModule * GetRootModule() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        const IModule * module = this;
        while(module->GetParentModule() != NULL) {
            module = module->GetParentModule();
        }
        return const_cast<IModule *>(module);
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
        return m_plugin || m_parent == NULL;
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
        return boost::filesystem::exists(m_path);
    }

    bool Create()
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        if (!Exists())	
        {
            proc.unlock();
            if (m_repository->InsertModule(GetQualifiedLabel()) != NULL)
                return true;
        }
        return false;
    }

    bool HasChildren() const
    {
        if (!boost::filesystem::exists(m_path))
            return false;

        boost::filesystem::directory_iterator end_itr;
        for (boost::filesystem::directory_iterator itr(wpathToPath(m_path)); itr != end_itr; ++itr)
        {
            if (boost::filesystem::is_directory(*itr))
                return true;
            else
            {
                std::_tstring type = CA2T(boost::filesystem::extension(*itr).c_str());
                if (IsValidExtension(type))
                    return true;
            }
        }
        return false;
    }

    IRepository * GetRepository() const
    {
        return m_repository;
    }

    void Update(bool noBroadcast = false)
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        //CFileAccess fileAccess((const TCHAR *)m_url);

        m_access = SecAccess_Full;
        {
            CAtlTemporaryFile temp;
            HRESULT hr = temp.Create(m_url);
            if (hr == S_OK) 
                m_access = SecAccess_Full;
            else
                m_access = SecAccess_Read;
        }

        m_plugin = false;

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

    void Refresh(REFRESH_MODULE refreshType = REFRESH_MODULE_UNKNOWN)
    {
        on_refresh(this, refreshType);
    }
};

//  ===========================================================================
static CacheT<std::_tstring, CDiskModule> DiskModuleCache;
void ClearDiskModuleCache()
{
    ATLTRACE(_T("Module cache before clearing(size=%u)\r\n"), DiskModuleCache.Size());
    DiskModuleCache.Clear();
}

void DeleteModule(CDiskModule * module)
{
    DiskModuleCache.Clear(module->GetCacheID());
}

CDiskModule * CreateDiskModuleRaw(const IRepository *rep, const std::_tstring & label, boost::filesystem::wpath path)
{
    CModuleHelper modHelper(label);
    StdStringVector tokens;
    modHelper.GetQualifiedLabel(tokens);
    for(StdStringVector::size_type i = 0; i < tokens.size(); ++i)
        path = path.parent_path();

    CDiskModule * retVal = NULL;
    std::_tstring qualifiedLabel;
    for (StdStringVector::iterator itr = tokens.begin(); itr != tokens.end(); ++itr)
    {
        if (!qualifiedLabel.empty())
            qualifiedLabel += CModuleHelper::DELIMTER;
        qualifiedLabel += *itr;
        path /= *itr;
        retVal = DiskModuleCache.Get(new CDiskModule(rep, retVal, path, qualifiedLabel.c_str(), *itr));
    }
    return retVal;
}

IModule * CreateDiskModule(const IRepository *rep, const std::_tstring &label, const boost::filesystem::wpath & path, bool noBroadcast = false)
{
    CDiskModule * mod = CreateDiskModuleRaw(rep, label, path);
    ATLASSERT(mod);
    mod->Update(noBroadcast);
    return mod;
}

