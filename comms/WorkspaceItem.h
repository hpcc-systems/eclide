#pragma once

#include "comms.h"
#include "Dali.h"
#include "Module.h"
#include "PersistMap.h"

enum WORKSPACE_ITEM_TYPE 
{
    WORKSPACE_ITEM_UNKNOWN,
    WORKSPACE_ITEM_BUILDER,
    WORKSPACE_ITEM_ATTRIBUTE,
    WORKSPACE_ITEM_GRAPH,
    WORKSPACE_ITEM_LAST
};

__interface __declspec(uuid("A4A4FFF2-0DED-422F-B06B-B0FE4CDB6C52")) IWorkspaceItem : public IUnknown
{
    const char * Serialize(std::string & result) const;
    IRepository * GetRepository() const;
    const TCHAR * GetID() const;
    const TCHAR * GetLabel() const;
    WORKSPACE_ITEM_TYPE GetType() const;
    int GetGroupIndex() const;
    int GetTabIndex() const;
    bool IsActive() const;
    const TCHAR *GetChecksum() const;

    void SetContent(const CPersistMap & pm);
    void Restore(IPersistable * window) const;

    //  Attribute Helpers  ---
    bool Exists() const;
    IModule * GetModule() const;
    IAttribute * GetAttribute() const;
    IAttribute * GetAttributePointer() const;
    std::wstring GetModuleLabel() const;
    std::wstring GetAttributeLabel() const;
    std::wstring GetAttributeType() const;
    AttrInfo GetAttrInfo() const;

    //  Builder Helpers  ---
    unsigned int GetWorkunits(Dali::IWorkunitVector * results) const;
    unsigned int GetWorkunitCount() const;

    //  Graph Helpers  ---
};
typedef StlLinked<IWorkspaceItem> IWorkspaceItemAdapt;
typedef std::vector<IWorkspaceItemAdapt> IWorkspaceItemVector;

IWorkspaceItem * CreateIWorkspaceItem(IRepository * repository, WORKSPACE_ITEM_TYPE type, IAttribute * attr, const std::_tstring & label, const std::_tstring & path);
IWorkspaceItem * CreateIWorkspaceItem(IRepository * repository, IAttribute * attr, const std::_tstring & path);
IWorkspaceItem * CreateIWorkspaceItem(IRepository * repository, WORKSPACE_ITEM_TYPE type, IAttribute * attr);
IWorkspaceItem * CreateIWorkspaceItem(IRepository * repository, WORKSPACE_ITEM_TYPE type, const CPersistMap & pm);

