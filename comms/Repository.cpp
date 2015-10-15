#include "StdAfx.h"
#include "repositoryImpl.h"

#include "Module.h"
#include "gSoapUtil.h"
#include "clib.h"
#include "cache.h"
#include "logger.h"
#include "global.h"
#include "util.h"
#include "utilDateTime.h"
#include "thread.h"
#include "ModuleHelper.h"
#include "DiskRepository.h"
#include "EclCC.h"
#include <UtilFilesystem.h>

void ClearAttributeTypeCache();
void ClearAttributeCache();
void ClearDiskAttributeCache();
void ClearModuleCache();
void ClearDiskModuleCache();
void ClearWorkspaceCache();
void ClearWorkspaceItemCache();

PasswordCacheT g_passwordCache;

IModule * CreateModule(const IRepository *rep, const ns2__ECLModule *data, bool noBroadcast=false);
IModule * CreateModulePlaceholder(const IRepository *rep, const std::_tstring &label);
IAttribute * CreateAttribute(const IRepository *rep, const ns2__ECLAttribute * data, bool noBroadcast=false);
IAttributeHistory * CreateAttributeHistory(const IRepository *rep, IAttributeType * type, const ns2__ECLAttribute * data);

#define ESP_EXCEPTION_LOG3(T) CReportingGSoapEspException<ns2__ArrayOfEspException> exceptions(T, __FILE__, __LINE__)

class CRepository : public CRepositoryBase
{
    std::_tstring m_url;

public:
    IMPLEMENT_CUNKNOWN;

    CRepository(const TCHAR* url, const TCHAR* userId, const TCHAR* password, const TCHAR* label, const TCHAR* instance) : m_url(url), CRepositoryBase(userId, password, label, instance)
    {
        if (g_passwordCache.find(m_url) == g_passwordCache.end())
        {
            g_passwordCache[m_url].first = userId;
            g_passwordCache[m_url].second = password;
        }
    }

    ~CRepository()
    {
    }

    virtual unsigned GetAllModules(IModuleVector & modules, IModuleHierarchy & moduleHierarchy, bool GetChecksum = false, bool noRefresh = true, bool noBroadcast = false) const
    {
        //clib::recursive_mutex::scoped_lock proc(m_mutex);
        if (m_supressModuleChecksum)
            GetChecksum = false;

        CachePoolAccessor<IModuleVector> modulesCache(m_cacheGetModules, GetCacheID(), m_repositoryLabel, boost::lexical_cast<std::_tstring>(GetChecksum));
        CachePoolAccessor<IModuleHierarchy> moduleHierarchyCache(m_cacheGetModulesHierarchy, GetCacheID(), m_repositoryLabel, boost::lexical_cast<std::_tstring>(GetChecksum));

        if (!modulesCache.needs_update(noRefresh) && !moduleHierarchyCache.needs_update(noRefresh))
        {
            modules = modulesCache.get();
            moduleHierarchy = moduleHierarchyCache.get();
            return modules.size();
        }
        //m_cacheGetModulesHierarchy.clear();
        CSoapInitialize<WsAttributesServiceSoapProxy> server(m_url, g_passwordCache[m_url].first.c_str(), g_passwordCache[m_url].second.c_str());

        _ns2__GetModules request;
        request.IncludeDeleted = false;
        CStringAssign Label(request.Label, m_repositoryLabel);
        request.GetChecksum = GetChecksum;

        _ns2__GetModulesResponse response;
        ESP_EXCEPTION_LOG3(response.Exceptions);
        if (server.GetModules(&request, &response) == SOAP_OK)
        {
            if (response.outModules)
            {
                for(std::size_t i = 0; i < response.outModules->ECLModule.size(); ++i)
                {
                    if (response.outModules->ECLModule[i]->Name && !response.outModules->ECLModule[i]->Name->empty())
                    {
                        StlLinked<IModule> module = CreateModule(this, response.outModules->ECLModule[i], noBroadcast);
                        modules.push_back(module);
                        StlLinked<IModule> parentModule = module->GetParentModule();
                        moduleHierarchy[parentModule ? parentModule->GetQualifiedLabel() : _T("")].push_back(module);
                    }
                }
                std::sort(modules.begin(), modules.end(), IModuleCompare());
                for(IModuleHierarchy::iterator itr = moduleHierarchy.begin(); itr != moduleHierarchy.end(); ++itr)
                {
                    std::sort(itr->second.begin(), itr->second.end(), IModuleCompare());
                }
            }
        }
        else
            _DBGLOG(m_url, LEVEL_WARNING, server.GetClientErrorMsg());

        m_cacheGetAttributes.clear();	//Need to clear attr cache when mod cache is cleared
        modulesCache.update(modules);
        moduleHierarchyCache.update(moduleHierarchy);
        return modules.size();
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

    IModule * GetModulePlaceholder(const TCHAR* label) const
    {
        //clib::recursive_mutex::scoped_lock proc(m_mutex);
        IModule * module = CreateModulePlaceholder(this, label);
        return module;
    }

    virtual IModule * InsertModule(const TCHAR* moduleName) const
    {
        //clib::recursive_mutex::scoped_lock proc(m_mutex);
        CSoapInitialize<WsAttributesServiceSoapProxy> server(m_url, g_passwordCache[m_url].first.c_str(), g_passwordCache[m_url].second.c_str());

        _ns2__CreateModule request;
        CStringAssign ModuleName(request.ModuleName, moduleName);

        _ns2__CreateModuleResponse response;
        ESP_EXCEPTION_LOG3(response.Exceptions);
        if (server.CreateModule(&request, &response) == SOAP_OK)
        {
            if (response.ModuleInfo)
            {
                StlLinked<IModule> module = CreateModule(this, response.ModuleInfo);
                {
                    CachePoolAccessor<IModuleVector> modulesCache(m_cacheGetModules, GetCacheID(), m_repositoryLabel);
                    modulesCache.clear();
                }
                return module.get();
            }
        }
        else
        {
            _DBGLOG(m_url, LEVEL_WARNING, server.GetClientErrorMsg());
        }
        return NULL;
    }

    virtual unsigned GetAttributes(const TCHAR* moduleName, const IAttributeTypeVector & types, IAttributeVector & attributes, bool noRefresh=false, bool noBroadcast=false) const
    {
        //clib::recursive_mutex::scoped_lock proc(m_mutex);
        std::_tstring tmpStr;
        CachePoolAccessor<IAttributeVector> attributesCache(*m_cacheGetAttributes.get(moduleName), GetCacheID(), m_repositoryLabel, moduleName, TypesToSingleString(types, tmpStr));
        if (!attributesCache.needs_update(noRefresh))
        {
            attributes = attributesCache.get();
            return attributes.size();
        }

        CSoapInitialize<WsAttributesServiceSoapProxy> server(m_url, g_passwordCache[m_url].first.c_str(), g_passwordCache[m_url].second.c_str());

        _ns2__GetAttributes request;
        CStringAssign ModuleName(request.ModuleName, moduleName);
        if (request.ModuleName->length() == 0)
            return 0;
        CStringAssign Label(request.Label, m_repositoryLabel);
        CStringArrayAssign<ns2__EspStringArray> TypeList(request.TypeList);
        for(IAttributeTypeVector::const_iterator itr = types.begin(); itr != types.end(); ++itr)
            TypeList.Append(itr->get()->GetRepositoryCode());

        _ns2__GetAttributesResponse response;
        ESP_EXCEPTION_LOG3(response.Exceptions);
        if (server.GetAttributes(&request, &response) == SOAP_OK)
        {
            if (response.outAttributes)
            {
                for(std::size_t i = 0; i < response.outAttributes->ECLAttribute.size(); ++i)
                {
                    StlLinked<IAttribute> attribute = ::CreateAttribute(this, response.outAttributes->ECLAttribute[i], noBroadcast);
                    if (attribute.isLinked())
                        attributes.push_back(attribute);
                }
                std::sort(attributes.begin(), attributes.end(), IAttributeCompare());
            }
        }
        else
            _DBGLOG(m_url, LEVEL_WARNING, server.GetClientErrorMsg());

        attributesCache.update(attributes);
        return attributes.size();
    }

    virtual unsigned int GetAttributeTypes(IAttributeTypeVector & types) const
    {
        //clib::recursive_mutex::scoped_lock proc(m_mutex);
        static CachePool<IAttributeTypeVector> g_knownTypes;
        CachePoolAccessor<IAttributeTypeVector> knownTypes(g_knownTypes, m_url);
        if (knownTypes.needs_update())
        {
            CSoapInitialize<WsAttributesServiceSoapProxy> server(m_url, g_passwordCache[m_url].first.c_str(), g_passwordCache[m_url].second.c_str());

            _ns2__ListAttributeTypes request;

            _ns2__ListAttributeTypesResponse response;
            //ESP_EXCEPTION_LOG3(response.Exceptions);
            if (server.ListAttributeTypes(&request, &response) == SOAP_OK)
            {
                if (response.Types)
                {
                    for(std::size_t i = 0; i < response.Types->Type.size(); ++i)
                        types.push_back(CreateIAttributeType(*response.Types->Type[i]->Name, *response.Types->Type[i]->Description));
                }
            }
            else
                _DBGLOG(m_url, LEVEL_WARNING, server.GetClientErrorMsg());
            if (types.empty())
                types.push_back(CreateIAttributeECLType());

            knownTypes.update(types);
        }
        else
            types = knownTypes.get();

        return types.size();
    }

    virtual unsigned FindAttributes(const std::_tstring & searchText, const std::_tstring & module, const std::_tstring & user, SEARCHMODE searchMode, bool sandboxed, bool checkedout, bool locked, bool orphaned, const TCHAR* utcDate, IAttributeVector & attributes) const
    {
        //clib::recursive_mutex::scoped_lock proc(m_mutex);
        CSoapInitialize<WsAttributesServiceSoapProxy> server(m_url, g_passwordCache[m_url].first.c_str(), g_passwordCache[m_url].second.c_str());

        _ns2__FindAttributes request;
        CStringAssign ModuleName(request.ModuleName, module);
        CStringAssign UserName(request.UserName, user);
        CStringAssign Pattern(request.Pattern, searchMode == SEARCHMODE_WILDCARD ? searchText :  _T(""));
        CStringAssign Regexp(request.Regexp, searchMode == SEARCHMODE_REGEXP ? searchText :  _T(""));
#if _COMMS_VER < 402682
#elif _COMMS_VER < 700000
        CStringAssign Plain(request.Plain, searchMode == SEARCHMODE_PLAIN ? searchText :  _T(""));
#else
#endif
        CStringAssign ChangedSince(request.ChangedSince, utcDate);
        IAttributeTypeVector types;
        GetAttributeTypes(types);
        CStringArrayAssign<ns2__EspStringArray> TypeList(request.TypeList);
        for(IAttributeTypeVector::const_iterator itr = types.begin(); itr != types.end(); ++itr)
            TypeList.Append(itr->get()->GetRepositoryCode());
        request.Sandboxed = sandboxed;
        request.Locked = locked;
        request.CheckedOut = checkedout;
        request.Orphaned = orphaned;
        request.GetText = false;
        request.GetMatchedLines = false;

        _ns2__FindAttributesResponse response;
        ESP_EXCEPTION_LOG3(response.Exceptions);
        if (server.FindAttributes(&request, &response) == SOAP_OK)
        {
            if (response.outAttributes)
            {
                for(std::size_t i = 0; i < response.outAttributes->ECLAttribute.size(); ++i)
                {
                    StlLinked<IAttribute> attribute = CreateAttribute(this, response.outAttributes->ECLAttribute[i]);
                    attributes.push_back(attribute);
                }
                std::sort(attributes.begin(), attributes.end(), IAttributeCompare());
            }
        }
        else
        {
            _DBGLOG(m_url, LEVEL_WARNING, server.GetClientErrorMsg());
            return -1;
        }

        return attributes.size();
    }

    virtual bool Label(const TCHAR* label, std::_tstring &error_message) const
    {
        //clib::recursive_mutex::scoped_lock proc(m_mutex);
        CSoapInitialize<WsAttributesServiceSoapProxy> server(m_url, g_passwordCache[m_url].first.c_str(), g_passwordCache[m_url].second.c_str());

        _ns2__CreateLabel request;
        CStringAssign Name(request.Name, label);

        _ns2__CreateLabelResponse response;
        ESP_EXCEPTION_LOG3(response.Exceptions);
        if (server.CreateLabel(&request, &response) == SOAP_OK)
        {
            if (response.Exceptions)
            {
                if (response.Exceptions->Exception.size())
                {
                    SAFE_ASSIGN(error_message, response.Exceptions->Exception[0]->Message);
                }
            }
            CachePoolAccessor<CRepLabelVector> labelsCache(m_cacheGetLabels, GetCacheID());
            labelsCache.clear();
            return true;
        }
        else
            _DBGLOG(m_url, LEVEL_WARNING, server.GetClientErrorMsg());

        return false;
    }

    virtual bool GetLabels(CRepLabelVector & results) const
    {
        //clib::recursive_mutex::scoped_lock proc(m_mutex);
        CachePoolAccessor<CRepLabelVector> labelsCache(m_cacheGetLabels, GetCacheID());
        if (!labelsCache.needs_update())
        {
            results = labelsCache.get();
            return true;
        }

        CSoapInitialize<WsAttributesServiceSoapProxy> server(m_url, g_passwordCache[m_url].first.c_str(), g_passwordCache[m_url].second.c_str());

        _ns2__GetLabels request;

        _ns2__GetLabelsResponse response;
        ESP_EXCEPTION_LOG3(response.Exceptions);
        if (server.GetLabels(&request, &response) == SOAP_OK)
        {
            if (response.outLabels)
            {
                for(std::size_t i = 0; i < response.outLabels->ECLLabel.size(); ++i)
                {
                    StlLinked<CRepLabel> label = new CRepLabel;
                    SAFE_ASSIGN(label->m_label, response.outLabels->ECLLabel[i]->Name);
                    SAFE_ASSIGN(label->m_datetime, response.outLabels->ECLLabel[i]->ModifiedDate);
                    UTCToDateTime(label->m_datetime.c_str(), label->m_date, label->m_time);
                    results.push_back(label);
                }
                std::sort(results.begin(), results.end(), ILabelCompare());
                labelsCache.update(results);
            }
            return true;
        }
        else
            _DBGLOG(m_url, LEVEL_WARNING, server.GetClientErrorMsg());
        return false;
    }

    virtual bool GetAttributeLabels(const std::_tstring & module, const std::_tstring & attr, CRepLabelVector & results) const
    {
        //clib::recursive_mutex::scoped_lock proc(m_mutex);
        CachePoolAccessor<CRepLabelVector> labelsCache(m_cacheGetLabels, GetCacheID(), module, attr);
        if (!labelsCache.needs_update())
        {
            results = labelsCache.get();
            return true;
        }

        CSoapInitialize<WsAttributesServiceSoapProxy> server(m_url, g_passwordCache[m_url].first.c_str(), g_passwordCache[m_url].second.c_str());

        _ns2__GetAttributeLabels request;
        CStringAssign ModuleName(request.ModuleName, module);
        CStringAssign AttributeName(request.AttributeName, attr);

        _ns2__GetAttributeLabelsResponse response;
        ESP_EXCEPTION_LOG3(response.Exceptions);
        if (server.GetAttributeLabels(&request, &response) == SOAP_OK)
        {
            if (response.Labels)
            {
                for(std::size_t i = 0; i < response.Labels->ECLAttributeLabel.size(); ++i)
                {
                    StlLinked<CRepLabel> label = new CRepLabel;
                    SAFE_ASSIGN(label->m_label, response.Labels->ECLAttributeLabel[i]->LabelName);
                    SAFE_ASSIGN(label->m_datetime, response.Labels->ECLAttributeLabel[i]->ModifiedDate);
                    SAFE_ASSIGN(label->m_version, response.Labels->ECLAttributeLabel[i]->AttributeVersion);
                    UTCToDateTime(label->m_datetime.c_str(), label->m_date, label->m_time);
                    results.push_back(label);
                }
                std::sort(results.begin(), results.end(), ILabelCompare());
                labelsCache.update(results);
                return true;
            }
        }
        else
            _DBGLOG(m_url, LEVEL_WARNING, server.GetClientErrorMsg());
        return false;
    }

    virtual bool UpdateLabel(const std::_tstring & label, const std::_tstring & module, const std::_tstring & attr, IAttributeType * type, unsigned int _version, std::_tstring &error_message) const
    {
        //clib::recursive_mutex::scoped_lock proc(m_mutex);
        CSoapInitialize<WsAttributesServiceSoapProxy> server(m_url, g_passwordCache[m_url].first.c_str(), g_passwordCache[m_url].second.c_str());

        _ns2__UpdateLabel request;
        CStringAssign Name(request.Name, label);
        CStringAssign ModuleName(request.ModuleName, module);
        CStringAssign AttributeName(request.AtributeName, attr);
        int version = _version;
        request.AttrbuteVersion = &version;

        _ns2__UpdateLabelResponse response;
        ESP_EXCEPTION_LOG3(response.Exceptions);
        if (server.UpdateLabel(&request, &response) == SOAP_OK)
        {
            if (response.Exceptions)
            {
                if (response.Exceptions->Exception.size())
                {
                    SAFE_ASSIGN(error_message, response.Exceptions->Exception[0]->Message);
                }
                CachePoolAccessor<CRepLabelVector> labelsCache(m_cacheGetLabels, GetCacheID());
                labelsCache.clear();
                return true;
            }
        }
        else
            _DBGLOG(m_url, LEVEL_WARNING, server.GetClientErrorMsg());
        return false;
    }

    virtual IModule * RenameModule(const TCHAR* module, const TCHAR* label) const
    {
        StlLinked<IModule> retVal;
        CSoapInitialize<WsAttributesServiceSoapProxy> server(m_url, g_passwordCache[m_url].first.c_str(), g_passwordCache[m_url].second.c_str());

        _ns2__RenameModule request;
        CStringAssign ModuleName(request.ModuleName, module);
        CStringAssign NewModuleName(request.NewModuleName, label);

        _ns2__RenameModuleResponse response;
        ESP_EXCEPTION_LOG3(response.Exceptions);
        if (server.RenameModule(&request, &response) == SOAP_OK)
        {
            if (response.ModuleInfo)
            {
                StlLinked<IModule> module = CreateModule(this, response.ModuleInfo);
                retVal = module;
            }
        }
        else
            _DBGLOG(m_url, LEVEL_WARNING, server.GetClientErrorMsg());

        CachePoolAccessor<IModuleVector> modulesCache(m_cacheGetModules, GetCacheID(), m_repositoryLabel);
        modulesCache.clear();
        return retVal.get();
    }

    virtual bool DeleteModule(const TCHAR* module) const
    {
        StlLinked<IModule> retVal;
        CSoapInitialize<WsAttributesServiceSoapProxy> server(m_url, g_passwordCache[m_url].first.c_str(), g_passwordCache[m_url].second.c_str());

        _ns2__DeleteModule request;
        CStringAssign ModuleName(request.ModuleName, module);

        _ns2__DeleteModuleResponse response;
        ESP_EXCEPTION_LOG3(response.Exceptions);
        if (server.DeleteModule(&request, &response) == SOAP_OK)
        {
            CachePoolAccessor<IModuleVector> modulesCache(m_cacheGetModules, GetCacheID(), m_repositoryLabel);
            modulesCache.clear();
            return true;
        }
        else
            _DBGLOG(m_url, LEVEL_WARNING, server.GetClientErrorMsg());

        return false;
    }

    virtual IAttribute * GetAttribute(const TCHAR* module, const TCHAR* attribute, IAttributeType * type, int version = 0, bool sandbox = true, bool text = true, bool noBroadcast = false) const
    {
        ATLASSERT(version == 0);//, _T("I am not sure we should allow this, I am hoping it is not called with a version > 0."));
        CSoapInitialize<WsAttributesServiceSoapProxy> server(m_url, g_passwordCache[m_url].first.c_str(), g_passwordCache[m_url].second.c_str());

        _ns2__GetAttribute request;
        CStringAssign ModuleName(request.ModuleName, module);
        CStringAssign AttributeName(request.AttributeName, attribute);
        if (request.ModuleName->empty() || request.AttributeName->empty())
            return NULL;
        CStringAssign Type(request.Type, type->GetRepositoryCode());
        request.Version = &version;
        request.GetSandbox = sandbox;
        request.GetText = text;
        CStringAssign Label(request.Label, m_repositoryLabel);

        _ns2__GetAttributeResponse response;
        ESP_EXCEPTION_LOG3(response.Exceptions);
        if (server.GetAttribute(&request, &response) == SOAP_OK)
        {
            if (response.outAttribute)
            {
                if (response.outAttribute->ModuleName && response.outAttribute->Name)	//  Succeeded but no attribute.
                {
                    CComPtr<IAttribute> attribute = CreateAttribute(this, response.outAttribute, noBroadcast);
                    return attribute;
                }
            }
        }
        else
            _DBGLOG(m_url, LEVEL_WARNING, server.GetClientErrorMsg());

        return NULL;
    }

    virtual IAttributeHistory * GetAttributeHistory(const TCHAR* module, const TCHAR* attribute, IAttributeType * type, int version, bool text = true) const
    {
        ATLASSERT(version > 0);//, _T("I am not sure we should allow this, I am hoping it is not called with a version > 0."));
        CSoapInitialize<WsAttributesServiceSoapProxy> server(m_url, g_passwordCache[m_url].first.c_str(), g_passwordCache[m_url].second.c_str());

        _ns2__GetAttribute request;
        CStringAssign ModuleName(request.ModuleName, module);
        CStringAssign AttributeName(request.AttributeName, attribute);
        if (request.ModuleName->empty() || request.AttributeName->empty())
            return NULL;
        CStringAssign Type(request.Type, type->GetRepositoryCode());
        request.Version = &version;
        request.GetSandbox = false;
        request.GetText = text;
        CStringAssign Label(request.Label, m_repositoryLabel);

        _ns2__GetAttributeResponse response;
        ESP_EXCEPTION_LOG3(response.Exceptions);
        if (server.GetAttribute(&request, &response) == SOAP_OK)
        {
            if (response.outAttribute)
            {
                if (response.outAttribute->ModuleName && response.outAttribute->Name)	//  Succeeded but no attribute.
                {
                    CComPtr<IAttributeHistory> attribute = CreateAttributeHistory(this, type, response.outAttribute);
                    return attribute;
                }
            }
        }
        else
            _DBGLOG(m_url, LEVEL_WARNING, server.GetClientErrorMsg());

        return NULL;
    }

    virtual bool GetAttributeText(const TCHAR* module, const TCHAR* attribute, IAttributeType * type, int version , bool sandbox, CString & text) const
    {
        text = _T("");
        CSoapInitialize<WsAttributesServiceSoapProxy> server(m_url, g_passwordCache[m_url].first.c_str(), g_passwordCache[m_url].second.c_str());

        _ns2__GetAttribute request;
        CStringAssign ModuleName(request.ModuleName, module);
        CStringAssign AttributeName(request.AttributeName, attribute);
        if (request.ModuleName->empty() || request.AttributeName->empty())
            return NULL;
        CStringAssign Type(request.Type, type->GetRepositoryCode());
        request.Version = &version;
        request.GetSandbox = sandbox;
        request.GetText = true;
        CStringAssign Label(request.Label, m_repositoryLabel);

        _ns2__GetAttributeResponse response;
        ESP_EXCEPTION_LOG3(response.Exceptions);
        if (server.GetAttribute(&request, &response) == SOAP_OK)
        {
            if (response.outAttribute)
            {
                SAFE_ASSIGN2CSTRING(text, response.outAttribute->Text);
            }
            return true;
        }
        else
            _DBGLOG(m_url, LEVEL_WARNING, server.GetClientErrorMsg());

        return false;
    }

    virtual IAttribute * InsertAttribute(const TCHAR* module, const TCHAR* attribute, IAttributeType * type) const
    {
        CSoapInitialize<WsAttributesServiceSoapProxy> server(m_url, g_passwordCache[m_url].first.c_str(), g_passwordCache[m_url].second.c_str());

        _ns2__CreateAttribute request;
        CStringAssign ModuleName(request.ModuleName, module);
        CStringAssign AttributeName(request.AttributeName, attribute);
        if (request.ModuleName->empty() || request.AttributeName->empty())
            return NULL;
        CStringAssign Type(request.Type, type->GetRepositoryCode());

        _ns2__CreateAttributeResponse response;
        ESP_EXCEPTION_LOG3(response.Exceptions);
        if (server.CreateAttribute(&request, &response) == SOAP_OK)
        {
            if (response.AttributeInfo)
            {
                StlLinked<IAttribute> attribute;
                if (response.AttributeInfo->Name)
                {
                    attribute = CreateAttribute(this, response.AttributeInfo);
                    {
                        //clib::recursive_mutex::scoped_lock proc(m_mutex);
                        m_cacheGetAttributes.clear(module);
                    }
                }
                return attribute.get();
            }
        }
        else
            _DBGLOG(m_url, LEVEL_WARNING, server.GetClientErrorMsg());

        ClearAttributeCache();
        return NULL;
    }

    virtual unsigned GetAttributeHistory(const TCHAR* module, const TCHAR* attribute, IAttributeType * type, IAttributeHistoryVector & attributes) const
    {
        CSoapInitialize<WsAttributesServiceSoapProxy> server(m_url, g_passwordCache[m_url].first.c_str(), g_passwordCache[m_url].second.c_str());

        _ns2__GetAttributeHistory request;
        CStringAssign ModuleName(request.ModuleName, module);
        CStringAssign AttributeName(request.AttributeName, attribute);
        if (request.ModuleName->empty() || request.AttributeName->empty())
            return NULL;

        _ns2__GetAttributeHistoryResponse response;
        ESP_EXCEPTION_LOG3(response.Exceptions);
        if (server.GetAttributeHistory(&request, &response) == SOAP_OK)
        {
            if (response.outAttributes)
            {
                for(std::size_t i = 0; i < response.outAttributes->ECLAttribute.size(); ++i)
                {
                    StlLinked<IAttributeHistory> attribute = CreateAttributeHistory(this, type, response.outAttributes->ECLAttribute[i]);
                    attributes.push_back(attribute);
                }
            }
        }
        else
            _DBGLOG(m_url, LEVEL_WARNING, server.GetClientErrorMsg());

        return attributes.size();
    }

    virtual IAttribute * RenameAttribute(const TCHAR* module, const TCHAR* attribute, IAttributeType * type, const TCHAR* label) const
    {
        StlLinked<IAttribute> retVal;
        CSoapInitialize<WsAttributesServiceSoapProxy> server(m_url, g_passwordCache[m_url].first.c_str(), g_passwordCache[m_url].second.c_str());

        CStructPool<ns2__RenameAttributeRequest> renameAttributeRequestPool;
        CStringPool stringPool;

        _ns2__RenameAttributes request;
        ns2__ArrayOfRenameAttributeRequest arrayOfRenameAttributeRequest;
        request.Attributes = &arrayOfRenameAttributeRequest;

        ns2__RenameAttributeRequest renameAttributeRequest;
        renameAttributeRequest.ModuleName = stringPool.Create(module);
        renameAttributeRequest.AttributeName = stringPool.Create(attribute);
        renameAttributeRequest.NewModuleName = stringPool.Create(module);
        renameAttributeRequest.NewAttributeName = stringPool.Create(label);

        request.Attributes->RenameAttributeRequest.push_back(renameAttributeRequestPool.Create(renameAttributeRequest));

        _ns2__UpdateAttributesResponse response;
        ESP_EXCEPTION_LOG3(response.Exceptions);
        if (server.RenameAttributes(&request, &response) == SOAP_OK)
        {
            if (response.outAttributes)
            {
                for(std::size_t i = 0; i < response.outAttributes->ECLAttribute.size(); ++i)
                {
                    StlLinked<IAttribute> attribute = CreateAttribute(this, response.outAttributes->ECLAttribute[i]);
                    if (i == 0)
                        retVal = attribute.get();
                }
            }
        }
        else
            _DBGLOG(m_url, LEVEL_WARNING, server.GetClientErrorMsg());

        return retVal.get();
    }

    virtual bool DeleteAttributes(const IAttributeVector & pAttributes) const
    {
        CSoapInitialize<WsAttributesServiceSoapProxy> server(m_url, g_passwordCache[m_url].first.c_str(), g_passwordCache[m_url].second.c_str());
        CStructPool<ns2__DeleteAttributeRequest> deleteAttributeRequestPool;
        CStringPool stringPool;

        _ns2__DeleteAttributes request;
        ns2__ArrayOfDeleteAttributeRequest arrayOfDeleteAttributeRequest;
        request.Attributes = &arrayOfDeleteAttributeRequest;

        for(std::size_t i = 0 ; i < pAttributes.size(); ++i)
        {
            ns2__DeleteAttributeRequest deleteAttributeRequest;
            deleteAttributeRequest.ModuleName = stringPool.Create(pAttributes[i]->GetModuleQualifiedLabel());
            deleteAttributeRequest.AttributeName = stringPool.Create(pAttributes[i]->GetLabel());

            arrayOfDeleteAttributeRequest.DeleteAttributeRequest.push_back(deleteAttributeRequestPool.Create(deleteAttributeRequest));
        }

        _ns2__UpdateAttributesResponse response;
        ESP_EXCEPTION_LOG3(response.Exceptions);
        if (server.DeleteAttributes(&request, &response) == SOAP_OK)
        {
            if (response.outAttributes)
            {
                for(std::size_t i = 0; i < response.outAttributes->ECLAttribute.size(); ++i)
                {
                    CComPtr<IAttribute> attribute = CreateAttribute(this, response.outAttributes->ECLAttribute[i]);
                    attribute->Refresh(false, NULL, true);
                }
                return true;
            }
            else
                _DBGLOG(m_url, LEVEL_WARNING, server.GetClientErrorMsg());
        }

        return false;
    }

    virtual bool Checkin(IAttributeVector & pAttributes, const TCHAR* comment)
    {
        CSoapInitialize<WsAttributesServiceSoapProxy> server(m_url, g_passwordCache[m_url].first.c_str(), g_passwordCache[m_url].second.c_str());
        CStructPool<ns2__CheckinAttributeRequest> checkinAttributeRequestPool;
        CStringPool stringPool;

        _ns2__CheckinAttributes request;
        ns2__ArrayOfCheckinAttributeRequest arrayOfCheckinAttributeRequest;
        request.Attributes = &arrayOfCheckinAttributeRequest;

        for(std::size_t i = 0 ; i < pAttributes.size(); ++i)
        {
            ns2__CheckinAttributeRequest checkinAttributeRequest;
            checkinAttributeRequest.ModuleName = stringPool.Create(pAttributes[i]->GetModuleQualifiedLabel());
            checkinAttributeRequest.AttributeName = stringPool.Create(pAttributes[i]->GetLabel());
            checkinAttributeRequest.Description = stringPool.Create(comment);

            arrayOfCheckinAttributeRequest.CheckinAttributeRequest.push_back(checkinAttributeRequestPool.Create(checkinAttributeRequest));
        }

        _ns2__UpdateAttributesResponse response;
        ESP_EXCEPTION_LOG3(response.Exceptions);
        if (server.CheckinAttributes(&request, &response) == SOAP_OK)
        {
            if (response.outAttributes)
            {
                for(std::size_t i = 0; i < response.outAttributes->ECLAttribute.size(); ++i)
                    CComPtr<IAttribute> attribute = CreateAttribute(this, response.outAttributes->ECLAttribute[i]);
                return true;
            }
            else
                _DBGLOG(m_url, LEVEL_WARNING, server.GetClientErrorMsg());
        }

        return false;
    }

    virtual bool Checkout(IAttributeVector & attributes)
    {
        CSoapInitialize<WsAttributesServiceSoapProxy> server(m_url, g_passwordCache[m_url].first.c_str(), g_passwordCache[m_url].second.c_str());
        CStructPool<ns2__CheckoutAttributeRequest> checkoutAttributeRequestPool;
        CStringPool stringPool;

        _ns2__CheckoutAttributes request;
        ns2__ArrayOfCheckoutAttributeRequest ns2__arrayOfCheckoutAttributeRequest;
        request.Attributes = &ns2__arrayOfCheckoutAttributeRequest;

        for(std::size_t i = 0 ;i < attributes.size(); ++i)
        {
            ns2__CheckoutAttributeRequest checkoutAttributeRequest;
            checkoutAttributeRequest.ModuleName = stringPool.Create(attributes[i]->GetModuleQualifiedLabel());
            checkoutAttributeRequest.AttributeName = stringPool.Create(attributes[i]->GetLabel());
            request.Attributes->CheckoutAttributeRequest.push_back(checkoutAttributeRequestPool.Create(checkoutAttributeRequest));
        }

        _ns2__UpdateAttributesResponse response;
        ESP_EXCEPTION_LOG3(response.Exceptions);
        if (server.CheckoutAttributes(&request, &response) == SOAP_OK)
        {
            if (response.outAttributes)
            {
                for(std::size_t i = 0; i < response.outAttributes->ECLAttribute.size(); ++i)
                    CComPtr<IAttribute> attribute = CreateAttribute(this, response.outAttributes->ECLAttribute[i]);
                return true;
            }
            else
                _DBGLOG(m_url, LEVEL_WARNING, server.GetClientErrorMsg());
        }
        return false;
    }

    virtual bool Save(IAttributeVector & attributes, bool noBroadcast = false)
    {
        CSoapInitialize<WsAttributesServiceSoapProxy> server(m_url, g_passwordCache[m_url].first.c_str(), g_passwordCache[m_url].second.c_str());
        CStructPool<ns2__SaveAttributeRequest> saveAttributeRequestPool;
        CStringPool stringPool;

        _ns2__SaveAttributes request;
        ns2__ArrayOfSaveAttributeRequest arrayOfSaveAttributeRequest;
        request.Attributes = &arrayOfSaveAttributeRequest;

        for(std::size_t i = 0 ; i < attributes.size(); ++i)
        {
            ns2__SaveAttributeRequest saveAttributeRequest;
            saveAttributeRequest.ModuleName = stringPool.Create(attributes[i]->GetModuleQualifiedLabel());
            saveAttributeRequest.AttributeName = stringPool.Create(attributes[i]->GetLabel());
            saveAttributeRequest.Text = stringPool.Create(attributes[i]->GetText(false));
            request.Attributes->SaveAttributeRequest.push_back(saveAttributeRequestPool.Create(saveAttributeRequest));
        }

        _ns2__UpdateAttributesResponse response;
        ESP_EXCEPTION_LOG3(response.Exceptions);
        if (server.SaveAttributes(&request, &response) == SOAP_OK)
        {
            if (response.outAttributes)
            {
                for(std::size_t i = 0; i < response.outAttributes->ECLAttribute.size(); ++i)
                    CComPtr<IAttribute> attribute = CreateAttribute(this, response.outAttributes->ECLAttribute[i], noBroadcast);
                return true;
            }
            else
                _DBGLOG(m_url, LEVEL_WARNING, server.GetClientErrorMsg());
        }
        return false;
    }

    virtual bool Move(IAttributeVector & attributes, const TCHAR* module)
    {
        CSoapInitialize<WsAttributesServiceSoapProxy> server(m_url, g_passwordCache[m_url].first.c_str(), g_passwordCache[m_url].second.c_str());
        CStructPool<ns2__RenameAttributeRequest> renameAttributeRequestPool;
        CStringPool stringPool;

        _ns2__RenameAttributes request;
        ns2__ArrayOfRenameAttributeRequest arrayOfRenameAttributeRequest;
        request.Attributes = &arrayOfRenameAttributeRequest;

        for(std::size_t i = 0 ; i < attributes.size(); ++i)
        {
            ns2__RenameAttributeRequest saveAttributeRequest;
            saveAttributeRequest.ModuleName = stringPool.Create(attributes[i]->GetModuleQualifiedLabel());
            saveAttributeRequest.AttributeName = stringPool.Create(attributes[i]->GetLabel());
            saveAttributeRequest.NewModuleName = stringPool.Create(module);
            saveAttributeRequest.NewAttributeName = stringPool.Create(attributes[i]->GetLabel());
            request.Attributes->RenameAttributeRequest.push_back(renameAttributeRequestPool.Create(saveAttributeRequest));
        }

        _ns2__UpdateAttributesResponse response;
        ESP_EXCEPTION_LOG3(response.Exceptions);
        if (server.RenameAttributes(&request, &response) == SOAP_OK)
        {
            if (response.outAttributes)
            {
                for(std::size_t i = 0; i < response.outAttributes->ECLAttribute.size(); ++i)
                {
                    CComPtr<IAttribute> attribute = CreateAttribute(this, response.outAttributes->ECLAttribute[i]);
                    for(std::size_t i = 0 ; i < attributes.size(); ++i)
                    {
                        if (boost::algorithm::iequals(attribute->GetModuleQualifiedLabel(), module) && boost::algorithm::iequals(attribute->GetLabel(), attributes[i]->GetLabel()))
                        {
                            attributes[i]->Refresh(false, attribute);
                            break;
                        }
                    }
                }
                return true;
            }
            else
                _DBGLOG(m_url, LEVEL_WARNING, server.GetClientErrorMsg());
        }
        return false;
    }

    virtual bool Rollback(IAttributeVector & attributes)
    {
        CSoapInitialize<WsAttributesServiceSoapProxy> server(m_url, g_passwordCache[m_url].first.c_str(), g_passwordCache[m_url].second.c_str());
        CStructPool<ns2__RollbackAttributeRequest> rollbackAttributeRequestPool;
        CStringPool stringPool;

        _ns2__RollbackAttributes request;
        ns2__ArrayOfRollbackAttributeRequest arrayOfRollbackAttributeRequest;
        request.Attributes = &arrayOfRollbackAttributeRequest;

        for(std::size_t i = 0 ; i < attributes.size(); ++i)
        {
            ns2__RollbackAttributeRequest rollbackAttributeRequest;
            rollbackAttributeRequest.ModuleName = stringPool.Create(attributes[i]->GetModuleQualifiedLabel());
            rollbackAttributeRequest.AttributeName = stringPool.Create(attributes[i]->GetLabel());
            request.Attributes->RollbackAttributeRequest.push_back(rollbackAttributeRequestPool.Create(rollbackAttributeRequest));
        }

        _ns2__UpdateAttributesResponse response;
        ESP_EXCEPTION_LOG3(response.Exceptions);
        if (server.RollbackAttributes(&request, &response) == SOAP_OK)
        {
            if (response.outAttributes)
            {
                for(std::size_t i = 0; i < response.outAttributes->ECLAttribute.size(); ++i)
                    CComPtr<IAttribute> attribute = CreateAttribute(this, response.outAttributes->ECLAttribute[i]);
                return true;
            }
            else
                _DBGLOG(m_url, LEVEL_WARNING, server.GetClientErrorMsg());
        }
        return false;
    }

    virtual const boost::filesystem::path & GetEnvironmentFolder(boost::filesystem::path & path) const
    {
        boost::filesystem::path userFolder;
        path = GetUserFolder(userFolder, GetUserId()) / stringToPath(GetLabel());

        try {
            boost::filesystem::create_directories(path);
        } catch (const boost::filesystem::filesystem_error & ex) {
            _DBGLOG(LEVEL_WARNING, ex.what());
        }

        return path;
    }
};

CacheT<std::_tstring, CRepository> RepositoryCache;

void ClearRepositorySingletons(bool childrenOnly)
{
    ClearAttributeTypeCache();
    ClearAttributeCache();
    ClearDiskAttributeCache();
    ClearModuleCache();
    ClearDiskModuleCache();
    ClearDiskRepositorySingletons();
    ClearModFileRepositorySingletons();
    ClearWorkspaceCache();
    ClearWorkspaceItemCache();
    if (!childrenOnly)
    {
        RepositoryCache.Clear();
        g_passwordCache.clear();
    }
    ClearEclCCSingletons();
}

//static void RollbackAsync(CString url, CString user, CString password, IAttributeVectorAdapt attributes, async_rollback_complete_slot_type s)
//{
//	async_rollback_complete_signal_type on_completion;
//	on_completion.connect(s);
//
//	CRollbackAttributes server;
//	server.SetUrl(url);
//	server.SetUserPassword(user,password);
//	bool bOK = server.Call(*(attributes.p));
//
//	on_completion(attributes,bOK);
//}
//
//void Rollback(const TCHAR* url, const TCHAR* user, const TCHAR* password, IAttributeVectorPtr& attributes, const async_rollback_complete_slot_type &s)
//{
//	clib::thread run(__FUNCTION__, boost::bind(&RollbackAsync, url, user, password, &attributes, s));
//}

IRepository * AttachRepository()
{
    CString url(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_ATTRIBUTE)); 
    CString user(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_USER));
    CString password(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_PASSWORD));
    CString label(GetIConfig(QUERYBUILDER_CFG)->GetLabel());
    return AttachRepository(url, user, password, label, _T(""));
}

IRepository * AttachRepository(const TCHAR* url, const TCHAR* userId, const TCHAR* password, const TCHAR* label)
{
    return AttachRepository(url, userId, password, label, _T(""));
}

IRepository * AttachRepository(const TCHAR* url, const TCHAR* userId, const TCHAR* password, const TCHAR* label, const TCHAR* instance)
{
    if (IsLocalRepositoryEnabled() == TRI_BOOL_TRUE)
        return AttachDiskRepository(label);

    if (IsLocalRepositoryEnabled() == TRI_BOOL_UNKNOWN)
    {
        CComPtr<CRepository> rep = new CRepository(url, userId, password, label, instance);
        CRepLabelVector labels;
        if (RepositoryCache.Get(rep)->GetLabels(labels))
            EnableLocalRepository(TRI_BOOL_FALSE);
        else
            EnableLocalRepository(TRI_BOOL_TRUE);
    }

    if (IsLocalRepositoryEnabled() == TRI_BOOL_TRUE)
        return AttachDiskRepository(label);

    return RepositoryCache.Get(new CRepository(url, userId, password, label, instance));
}


