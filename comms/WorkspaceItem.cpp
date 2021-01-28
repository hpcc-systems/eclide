#include "StdAfx.h"

#include "Repository.h"
#include "WorkspaceItem.h"
#include "ModuleHelper.h"
#include "Thread.h"
#include "cache.h"
#include <UtilFilesystem.h>

//  ===========================================================================
class CWorkspaceItem : public IWorkspaceItem, public clib::CLockableUnknown
{
    friend class CWorkspaceItemCompare;

public:
    CComPtr<IRepository> m_repository;
    WORKSPACE_ITEM_TYPE m_type;
    int m_groupIdx;
    int m_tabIdx;
    bool m_active;
    enum LOADING
    {
        LOADING_UNKNOWN,
        LOADING_STARTED,
        LOADING_FINISHED,
        LOADING_LAST
    } m_loaded;

    CPersistMap m_props;
    CComPtr<IAttribute> m_attribute;
    bool m_attributeLoaded;
    Dali::IWorkunitVector m_workunits;
    bool m_workunitLoaded;
    std::_tstring m_id;
    std::_tstring m_label;
    std::_tstring m_moduleLabel;
    std::_tstring m_attributeLabel;
    std::_tstring m_attributeType;
    std::_tstring m_builder;
    std::_tstring m_graph;
    std::_tstring m_blankString;

public:
    BEGIN_CLOCKABLEUNKNOWN
        IMPLEMENT_INTERFACE(IWorkspaceItem)
    END_CUNKNOWN(clib::CLockableUnknown)

    static void threadLoadWorkunit(CWorkspaceItem * self, CString wuids)
    {
        StlLinked<Dali::IDali> server = Dali::AttachDali();

        int wuCap = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_WORKUNIT_PERSISTLIMIT);
        StdStringVector wus;
        SortedDeserialize(static_cast<const TCHAR *>(wuids), wus, _T(","));
        for (StdStringVector::const_reverse_iterator ritr = wus.rbegin(); ritr != wus.rend(); ++ritr)
        {
            CString wuid = ritr->c_str();
            if (!wuid.IsEmpty() && !self->HasWorkunit(wuid))
            {
                StlLinked<Dali::IWorkunit> workunit = server->GetWorkunitFast(wuid, true);
                if (workunit.get())
                    self->AppendWorkunit(workunit.get());
            }

            if (--wuCap <= 0)
                break;
        }

        self->m_loaded = LOADING_FINISHED;
    }

    static void threadLoadAttribute(CWorkspaceItem * self, std::_tstring moduleLabel, std::_tstring attributeLabel, std::_tstring attributeType)
    {
        self->m_loaded = LOADING_STARTED;
        CComPtr<IAttribute> attribute = NULL;
        if (!moduleLabel.empty() && !attributeLabel.empty())
        {
            attribute = self->m_repository->GetAttributeFast(moduleLabel.c_str(), attributeLabel.c_str(), CreateIAttributeType(attributeType), 0, true, true, true);
            if (!attribute)
                attribute = self->m_repository->GetAttributePlaceholder(moduleLabel.c_str(), attributeLabel.c_str(), CreateIAttributeType(attributeType));
        }
        self->SetAttr(attribute);
        self->m_loaded = LOADING_FINISHED;
    }

    CWorkspaceItem(IRepository * repository, WORKSPACE_ITEM_TYPE type, const std::_tstring & label, const std::_tstring & path)
    {
        m_repository = repository;
        m_type = type;
        m_id = path;
        boost::algorithm::to_lower(m_id);
        m_label = label;
        m_props.Set(PERSIST_LABEL, label);
        m_attributeLoaded = false;
        m_workunitLoaded = false;
        m_loaded = LOADING_UNKNOWN;
    }

    CWorkspaceItem(IRepository * repository, const std::string & data)
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        m_repository = repository;
        m_props.deserializeXML(static_cast<const TCHAR *>(CA2T(data.c_str(), CP_UTF8)));
        m_id = m_props.Get(PERSIST_FILEPATH);
        UpdateID();
        m_attributeLoaded = false;
        m_workunitLoaded = false;
        m_loaded = LOADING_UNKNOWN;
    }

    void Load()
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        if (m_loaded != LOADING_UNKNOWN)		// Already started loading, but could get here via the way the cache works....
            return;

        switch (m_type)
        {
        case WORKSPACE_ITEM_BUILDER:
            {
                if (m_workunitLoaded)
                {
                    m_loaded = LOADING_FINISHED;
                    break;
                }
                m_loaded = LOADING_STARTED;
                CString wuids = m_props.Get(PERSIST_WUID);
                if (!wuids.IsEmpty()) {
                    clib::thread run(__FUNCTION__, boost::bind(&threadLoadWorkunit, this, wuids));
                    //threadLoadWorkunit(this, wuids);
                }
                else
                {
                    m_loaded = LOADING_FINISHED;
                }
            }
            break;
        case WORKSPACE_ITEM_ATTRIBUTE:
            {
                if (m_attributeLoaded)	// Already loaded, but could get here via the way the cache works....
                {
                    m_loaded = LOADING_FINISHED;
                    break;
                }
                if (m_moduleLabel.size() && m_attributeLabel.size())
                    clib::thread run(__FUNCTION__, boost::bind(&threadLoadAttribute, this, m_moduleLabel, m_attributeLabel, m_attributeType));
                else
                    m_loaded = LOADING_FINISHED;
                //threadLoadAttribute(this, m_moduleLabel, m_attributeLabel, m_attributeType);
            }
            break;
        default:
            m_loaded = LOADING_FINISHED;
        }
    }

    void UpdateID()
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        m_type = CalcType(m_props.Get(PERSIST_TYPE));
        bool recalcID = m_id.empty();
        if (recalcID)
            m_id = m_props.Get(PERSIST_TYPE);
        //std::_tstring filePath = m_props.Get(PERSIST_FILEPATH);
        //if (filePath.empty())
        //	m_props.Set(PERSIST_FILEPATH, m_id);

        switch(m_type)
        {
        case WORKSPACE_ITEM_BUILDER:
            m_builder = m_props.Get(PERSIST_LABEL);
            m_moduleLabel = m_props.Get(PERSIST_MODULE);
            m_attributeLabel = m_props.Get(PERSIST_ATTRIBUTE);
            m_attributeType = m_props.Get(PERSIST_ATTRIBUTETYPE);
            if (recalcID)
                m_id += _T("/") + m_builder;
            m_label = m_builder;
            break;
        case WORKSPACE_ITEM_ATTRIBUTE:
            m_moduleLabel = m_props.Get(PERSIST_MODULE);
            m_attributeLabel = m_props.Get(PERSIST_ATTRIBUTE);
            m_attributeType = m_props.Get(PERSIST_ATTRIBUTETYPE);
            if (recalcID)
                m_id += _T("/") + m_moduleLabel + _T("/") + m_attributeLabel + _T("/") + m_attributeType;
            if (m_moduleLabel.empty() && !m_id.empty()) {
                std::_tstring label, module;
                CModuleHelper modHelper(_T(""));
                modHelper.ModuleAttrFromPath(m_id, label, module, m_moduleLabel, m_attributeLabel);
            }
            m_label = m_moduleLabel + _T(".") + m_attributeLabel;
            break;
        case WORKSPACE_ITEM_GRAPH:
            m_graph = m_props.Get(PERSIST_PATH);
            if (recalcID)
                m_id += _T("/") + m_graph;
            m_label = m_graph;
            break;
        default:
            ATLASSERT(!"Unknown persist type.");
            break;
        }
        try
        {
            m_groupIdx = boost::lexical_cast<int>(m_props.Get(PERSIST_GROUPINDEX));
            m_tabIdx = boost::lexical_cast<int>(m_props.Get(PERSIST_TABINDEX));
            m_active = boost::lexical_cast<bool>(m_props.Get(PERSIST_ACTIVE));
        }
        catch (boost::bad_lexical_cast &)
        {
        }
        boost::algorithm::to_lower(m_id);
    }
    bool HasWorkunit(const CString & wuid)
    {
        clib::recursive_mutex::scoped_lock lock(m_mutex);
        for(Dali::IWorkunitVector::const_iterator itr = m_workunits.begin(); itr != m_workunits.end(); ++itr)
        {
            if (wuid.Compare(itr->get()->GetWuid()) == 0)
                return true;
        }
        return false;
    }
    void AppendWorkunit(Dali::IWorkunit * workunit)
    {
        clib::recursive_mutex::scoped_lock lock(m_mutex);
        m_workunits.push_back(workunit);
        m_workunitLoaded = true;
    }
    void SetAttr(IAttribute * attribute)
    {
        clib::recursive_mutex::scoped_lock lock(m_mutex);
        m_attribute = attribute;
        m_attributeLoaded = true;
        m_loaded = LOADING_FINISHED;
    }
    bool operator == (const CWorkspaceItem & c) const
    {
        return m_groupIdx == c.m_groupIdx && m_tabIdx == c.m_tabIdx;
    }
    bool operator != (const CWorkspaceItem & c) const
    {
        return m_groupIdx != c.m_groupIdx || m_tabIdx != c.m_tabIdx;
    }
    bool operator < (const CWorkspaceItem & c) const
    {
        if (m_groupIdx == c.m_groupIdx)
            return m_tabIdx < c.m_tabIdx;
        return m_groupIdx < c.m_groupIdx;
    }
    const TCHAR * GetCacheID() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_id.c_str();
    }
    //  IWorkspaceItem  ---
    IRepository * GetRepository() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_repository;
    }
    const TCHAR * GetID() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_id.c_str();
    }
    const TCHAR * GetLabel() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_label.c_str();
    }
    const char * Serialize(std::string & result) const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        std::_tstring data;
        m_props.serialize(data);
        result = CT2A(data.c_str(), CP_UTF8);
        return result.c_str();
    }
    WORKSPACE_ITEM_TYPE GetType() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_type;
    }

    int GetGroupIndex() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_groupIdx;
    }

    int GetTabIndex() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_tabIdx;
    }

    bool IsActive() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_active;
    }

    const TCHAR *GetChecksum() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        switch(GetType())
        {
        case WORKSPACE_ITEM_ATTRIBUTE:
            return Exists() ? m_attribute->GetChecksum() : m_blankString.c_str(); 
            break;
        }
        return m_blankString.c_str();
    }

    bool Exists() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return (m_attribute != NULL);
    }

    IModule * GetModule() const
    {
        while(m_loaded != LOADING_FINISHED)
            ::Sleep(200);
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        ATLASSERT(m_attribute);
        return m_attribute->GetModule();
    }

    IAttribute * GetAttribute() const
    {
        if (m_type != WORKSPACE_ITEM_ATTRIBUTE)
            return NULL;

        while(m_loaded != LOADING_FINISHED)
            ::Sleep(200);
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_attribute;
    }

    IAttribute * GetAttributePointer() const
    {
        return m_attribute;
    }

    std::wstring GetModuleLabel() const
    {
        return m_moduleLabel;
    }

    std::wstring GetAttributeLabel() const
    {
        return m_attributeLabel;
    }

    std::wstring GetAttributeType() const
    {
        return m_attributeType;
    }

    AttrInfo GetAttrInfo() const
    {
        AttrInfo attrInfo;
        if (m_attributeType.length())
        {
            attrInfo.AttributeType = m_attributeType;
        }
        else
        {
            attrInfo.AttributeType = ExtensionWithoutDot(m_id);
        }

        return attrInfo;
    }

    unsigned int GetWorkunits(Dali::IWorkunitVector * results) const
    {
        while(m_loaded != LOADING_FINISHED)
            ::Sleep(200);
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        *results = m_workunits;
        return results->size();
    }

    unsigned int GetWorkunitCount() const
    {
        while(m_loaded != LOADING_FINISHED)
            ::Sleep(200);
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_workunits.size();
    }

    void SetContent(const CPersistMap & pm)
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        m_props = pm;
        m_id = m_props.Get(PERSIST_FILEPATH);
        UpdateID();
        m_loaded = LOADING_UNKNOWN;
        Load();
    }

    void SetAttribute(IAttribute * attribute)
    {
        m_attribute = attribute;
    }

    void SetLabel(const std::_tstring & label)
    {
        m_label = label;
    }

    void Restore(IPersistable * window) const
    {
        clib::recursive_mutex::scoped_lock lock(m_mutex);
        WINDOWPLACEMENT placement;
        std::string encodedStr = CT2A(m_props.Get(PERSIST_PLACEMENT));
        int decodedLen = sizeof(WINDOWPLACEMENT);
        BOOL hr = Base64Decode(encodedStr.c_str(), encodedStr.length(), reinterpret_cast<BYTE *>(&placement), &decodedLen);
        if (hr && placement.length == sizeof(WINDOWPLACEMENT))
        {
            if (placement.showCmd == SW_SHOWMINIMIZED)
            {
                //(Prevent Launch To Minimized)
                placement.showCmd = SW_SHOWNORMAL;
            }
            RECT rMaxWA;
            ::SystemParametersInfo(SPI_GETWORKAREA, 0, &rMaxWA, 0);
            if( (placement.rcNormalPosition.left >= rMaxWA.left) &&
                (placement.rcNormalPosition.top >= rMaxWA.top) &&
                (placement.rcNormalPosition.right <= rMaxWA.right) &&
                (placement.rcNormalPosition.bottom <= rMaxWA.bottom) )
            {
                // If All Values Within The Visible Screen (Taking Into Account Only The Primary Monitor...!), Set The Placement
                ::SetWindowPlacement(window->GetHwnd(), &placement);
            }
        }
        window->RestorePersistInfo(m_props);
    }

    int GetGroupIndex()
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_groupIdx;
    }
    int GetTabIndex()
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_tabIdx;
    }
    bool IsActive()
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_active;
    }

    static WORKSPACE_ITEM_TYPE CalcType(const std::_tstring & type) 
    {
        if (type.compare(PERSISTVAL_BUILDER) == 0)
        {
            return WORKSPACE_ITEM_BUILDER;
        }
        else if (type.compare(PERSISTVAL_ATTRIBUTE) == 0)
        {
            return WORKSPACE_ITEM_ATTRIBUTE;
        }
        else if (type.compare(PERSISTVAL_GRAPH) == 0)
        {
            return WORKSPACE_ITEM_GRAPH;
        }
        return WORKSPACE_ITEM_UNKNOWN;
    }

    static const TCHAR * CalcType(WORKSPACE_ITEM_TYPE type)
    {
        switch(type)
        {
        case WORKSPACE_ITEM_BUILDER:
            return PERSISTVAL_BUILDER;
        case WORKSPACE_ITEM_ATTRIBUTE:
            return PERSISTVAL_ATTRIBUTE;
        case WORKSPACE_ITEM_GRAPH:
            return PERSISTVAL_GRAPH;
        }
        return _T("");
    }
};

class CWorkspaceItemCompare
{
public:
    bool operator ()(const StlLinked<CWorkspaceItem> & l, const StlLinked<CWorkspaceItem> & r) const
    {
        if (l->m_groupIdx == r->m_groupIdx)
            return l->m_tabIdx < r->m_tabIdx;
        return l->m_groupIdx < r->m_groupIdx;
    }
};

typedef StlLinked<CWorkspaceItem> CWorkspaceItemAdapt;
typedef std::vector<CWorkspaceItemAdapt> CWorkspaceItemVector;
typedef std::vector<HWND> CChildFrameVector;
//  ===========================================================================
static CacheT<std::_tstring, CWorkspaceItem> CWorkspaceItemCache;

IWorkspaceItem * CreateIWorkspaceItem(IRepository * repository, const std::string & data)
{
    StlLinked<CWorkspaceItem> workspaceItem = CWorkspaceItemCache.Get(new CWorkspaceItem(repository, data));
    workspaceItem->Load();
    return workspaceItem.get();
}

IWorkspaceItem * CreateIWorkspaceItem(IRepository * repository, WORKSPACE_ITEM_TYPE type, IAttribute *attr, const std::_tstring & label, const std::_tstring & path)
{
    StlLinked<CWorkspaceItem> workspaceItem = CWorkspaceItemCache.Get(new CWorkspaceItem(repository, type, label, path));
    if (!workspaceItem->m_attribute)
    {
        workspaceItem->m_attribute = attr;
    }
    return workspaceItem.get();
}

IWorkspaceItem * CreateIWorkspaceItem(IRepository * repository, IAttribute * attr, const std::_tstring & path)
{
    StlLinked<CWorkspaceItem> workspaceItem = CWorkspaceItemCache.Get(new CWorkspaceItem(repository, WORKSPACE_ITEM_ATTRIBUTE, attr->GetQualifiedLabel(), path));
    workspaceItem->m_attribute = attr;
    workspaceItem->m_loaded = CWorkspaceItem::LOADING_FINISHED;
    return workspaceItem.get();
}

IWorkspaceItem * CreateIWorkspaceItem(IRepository * repository, WORKSPACE_ITEM_TYPE type, IAttribute *attr)
{
    std::_tstring wideFileName, wideFilePath;
    boost::filesystem::path filePath, envFolder;
    GetIConfig(QUERYBUILDER_CFG)->GetEnvironmentFolder(envFolder);
    srand((unsigned)time(NULL));
    for(;;)
    {
        wideFileName = CWorkspaceItem::CalcType(type);
        wideFileName += _T("_");
        std::string fileName = CT2A(wideFileName.c_str());
        fileName += boost::lexical_cast<std::string>(rand() % 999999);
        if (attr)
        {
            fileName += CT2A(attr->GetType()->GetFileExtension());
        }
        else
        {
            fileName += ".ecl";
        }
        wideFileName = CA2T(fileName.c_str());
        filePath = envFolder / fileName;
        wideFilePath = pathToWString(filePath);
        if (!CWorkspaceItemCache.Exists(wideFilePath))
            break;
    }
    return CreateIWorkspaceItem(repository, type, attr, wideFileName, wideFilePath);
}

IWorkspaceItem * CreateIWorkspaceItem(IRepository * repository, WORKSPACE_ITEM_TYPE type, const CPersistMap & pm)
{
    IWorkspaceItem * retVal = CreateIWorkspaceItem(repository, type, NULL);
    ATLASSERT(retVal);
    retVal->SetContent(pm);
    return retVal;
}

void ClearWorkspaceItemCache()
{
    ATLTRACE(_T("WorkspaceItem cache before clearing(size=%u)\r\n"), CWorkspaceItemCache.Size());
    CWorkspaceItemCache.Clear();
}
