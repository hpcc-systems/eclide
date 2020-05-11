#pragma once

#include "comms.h"
#include "DiskModule.h"
#include "global.h"
#include "StringSet.h"
#include "workspace.h"
#include "Logger.h"

class CRepLabel : public CUnknown
{
public:
    std::_tstring m_label;
    std::_tstring m_datetime;
    boost::gregorian::date m_date;
    boost::posix_time::ptime m_time;
    int m_version;

    BEGIN_CUNKNOWN
    END_CUNKNOWN(CUnknown)

    CRepLabel() : 
            m_date(boost::gregorian::day_clock::local_day()),
            m_time(boost::posix_time::second_clock::universal_time())
    {
        m_version = -1;
    }
};
typedef StlLinked<CRepLabel> CRepLabelAdapt;
typedef std::vector<CRepLabelAdapt> CRepLabelVector;
typedef std::map<IAttribute *, CString> CAttrMsg;

enum SEARCHMODE
{
    SEARCHMODE_UNKNOWN = -1,
    SEARCHMODE_PLAIN = 0,
    SEARCHMODE_WILDCARD,
    SEARCHMODE_REGEXP,
    SEARCHMODE_LAST
};

__interface IRepository : public IUnknown
{
    void SetRepositoryLabel(const std::_tstring & label);
    const TCHAR * GetRepositoryLabel() const;

    const TCHAR *GetID() const;
    const TCHAR *GetLabel() const;
    const TCHAR *GetUserId() const;
    const TCHAR *GetPassword() const;

    IModule * GetModule(const TCHAR* label, bool GetChecksum = false, bool noBroadcast = false) const;
    IModule * GetModulePlaceholder(const TCHAR* label) const;
    unsigned GetModules(const std::_tstring & module, IModuleVector & modules, bool GetChecksum = false, bool noRefresh = true) const;
    unsigned GetAllModules(IModuleVector & modules, bool GetChecksum = false, bool noRefresh = true, bool noBroadcast = false) const;
    unsigned GetModulesAutoC(const std::_tstring & module, StdStringVector &modules) const;
    IModule * InsertModule(const TCHAR* label) const;

    unsigned int GetAttributeTypes(IAttributeTypeVector & types) const;
    unsigned FindAttributes(const std::_tstring & searchText, const std::_tstring & module, const std::_tstring & user, SEARCHMODE searchMode, bool sandboxed, bool checkedout, bool locked, bool orphaned, const  TCHAR* utcDate, IAttributeVector & attributes) const;
    bool Label(const TCHAR* label, std::_tstring &error_message) const;
    bool GetLabels(CRepLabelVector & labels) const;
    bool GetLabels(const std::_tstring & module, const std::_tstring & attr, CRepLabelVector & globalLabels, CRepLabelVector & localLabels) const;
    bool GetAttributeLabels(const std::_tstring & module, const std::_tstring & attr, CRepLabelVector & labels) const;
    bool UpdateLabel(const std::_tstring & label, const std::_tstring & module, const std::_tstring & attr, IAttributeType * type, unsigned int version, std::_tstring &error_message) const;

    //  Module short cuts  ---
    unsigned GetAttributes(const TCHAR* module, const IAttributeTypeVector & types, IAttributeVector & attributes, bool noRefresh=false, bool noBroadcast=false) const;
    unsigned GetAttributes(const TCHAR* module, IAttributeVector & attributes, bool noRefresh=false, bool noBroadcast=false) const;
    unsigned GetAttributesAutoC(const std::_tstring & module, StdStringVector & attributes) const;
    IModule * RenameModule(const TCHAR* module, const TCHAR* label) const;
    bool DeleteModule(const TCHAR* label) const;

    //  Attribute short cuts  ---
    IAttribute * GetAttribute(const TCHAR* module, const TCHAR* attribute, IAttributeType * type, int version = 0, bool sandbox = true, bool text = false, bool noBroadcast = false) const;
    IAttribute * GetAttributeFast(const TCHAR* module, const TCHAR* attribute, IAttributeType * type, int version = 0, bool sandbox = true, bool text = false, bool noBroadcast = false) const;
    IAttribute * GetAttributePlaceholder(const std::_tstring & module, const std::_tstring & attribute, IAttributeType * type) const;
    IAttribute * GetAttribute(const TCHAR* moduleAttribute, IAttributeType * type, int version = 0, bool sandbox = true, bool text = true) const;
    IAttribute * GetAttributeFuzzy(const std::_tstring & moduleAttribute, const std::_tstring & module, IAttributeType * type, int version = 0, bool sandbox = true, bool text = true) const;
    bool GetAttributeText(const TCHAR* module, const TCHAR* attribute, IAttributeType * type, int version, bool sandbox, CString &text) const;
    IAttribute * InsertAttribute(const TCHAR* module, const TCHAR* attribute, IAttributeType * type) const;
    unsigned GetAttributeHistory(const TCHAR* module, const TCHAR* attribute, IAttributeType * type, IAttributeHistoryVector & attributes) const;
    IAttribute * RenameAttribute(const TCHAR* module, const TCHAR* attribute, IAttributeType * type, const TCHAR* label) const;
    IAttribute * RenameAttribute(IAttribute * attr, const TCHAR* label) const;
    IAttribute * ChangeAttributeType(IAttribute * attr, const TCHAR* ext) const;

    bool DeleteAttributes(const IAttributeVector & attributes) const;
    bool Checkin(IAttributeVector & attributes, const TCHAR* comment, bool andRevert);
    bool Checkout(IAttributeVector & attributes);
    bool Save(IAttributeVector & attributes, bool noBroadcast = false);
    bool Move(IAttributeVector & attributes, const TCHAR* module);
//	bool GetEcl(const TCHAR* module, const TCHAR* attr);
    bool Rollback(IAttributeVector & attributes);
    CAttrMsg AttrMessages();

    //  Attribute History short cuts ----
    IAttributeHistory * GetAttributeHistory(const TCHAR* module, const TCHAR* attribute, IAttributeType * type, int version = 0, bool text = false) const;

    //  General  ---
    bool ModuleExists(const TCHAR* label);
    bool AttributeExists(const TCHAR* module, const TCHAR* label, IAttributeType * type);

    //  Workspace Helpers  ---
    IWorkspace * CreateIWorkspace(const std::_tstring & label = _T(""), bool showInRoot = true, bool placeholder = false);
    unsigned int GetWorkspaces(IWorkspaceVector * results, bool attributesOnly);
    bool WorkspaceExists(const std::_tstring & label);
    bool RemoveWorkspace(const std::_tstring & label);

    //  AMT Overrides  ---
    void SupressModuleChecksum(bool supressModuleChecksum);

    IWorkspaceItem * CreateIWorkspaceItem(WORKSPACE_ITEM_TYPE type, const std::_tstring & label, const std::_tstring & path);
    IWorkspaceItem * CreateIWorkspaceItem(IAttribute * attr, const std::_tstring & path);
    IWorkspaceItem * CreateIWorkspaceItem(WORKSPACE_ITEM_TYPE type, IAttribute * attr);
    IWorkspaceItem * CreateIWorkspaceItem(WORKSPACE_ITEM_TYPE type, const CPersistMap & pm);

    const boost::filesystem::path & GetEnvironmentFolder(boost::filesystem::path & path) const;
    void ClearShortTermCache();
};

typedef StlLinked<IRepository> IRepositoryAdapt;
typedef std::vector<IRepositoryAdapt> IRepositoryVector;

COMMS_API IRepository * AttachRepository();
//COMMS_API IRepository * AttachRepository(const TCHAR* url, const TCHAR* label);
//COMMS_API IRepository * AttachRepository(const TCHAR* url, const TCHAR* label, const TCHAR* instance);
COMMS_API IRepository * AttachRepository(const TCHAR* url, const TCHAR* userId, const TCHAR* password, const TCHAR* label);
COMMS_API IRepository * AttachRepository(const TCHAR* url, const TCHAR* userId, const TCHAR* password, const TCHAR* label, const TCHAR* instance);
COMMS_API void ClearRepositorySingletons(bool childrenOnly);

COMMS_API IRepository * AttachDiskRepository(const TCHAR* url, const TCHAR * label, bool bClearCache=false);
COMMS_API IRepository * AttachDiskRepository(const TCHAR * label, bool bClearCache=false);
void ClearDiskRepositorySingletons();

COMMS_API IRepository * AttachModFileRepository(const TCHAR* url, bool bClearCache);
void ClearModFileRepositorySingletons();

//an ansync rollback
//typedef boost::signal<void(IAttributeVectorAdapt attrs, bool bOK)> async_rollback_complete_signal_type;
//typedef async_rollback_complete_signal_type::slot_type async_rollback_complete_slot_type;
//COMMS_API void Rollback(const TCHAR* url, const TCHAR* user, const TCHAR* password, IAttributeVectorPtr & attributes, const async_rollback_complete_slot_type &s);

struct CRepositorySelections
{
    IRepositoryVector		reps;
    IDiskModuleVector		diskReps;
    IModuleVector			plugins;
    IModuleVector			mods;
    IAttributeVector		attrs;
    IAttributeHistoryVector histories;
    //IWorkspaceVector		workspaces;

    //CRepositoryNodeVector		repNodes;
    //CModuleNodeVector			modNodes;
    //CAttributeNodeVector		attrNodes;
    //CAttributeHistoryNodeVector historyNodes;
};

