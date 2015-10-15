#pragma once

#include "comms.h"
#include "global.h"

#include "PersistMap.h"
#include "WorkspaceItem.h"

__interface IWorkspace;
typedef StlLinked<IWorkspace> IWorkspaceAdapt;
typedef std::vector<IWorkspaceAdapt> IWorkspaceVector;

__interface __declspec(uuid("E670F76D-B20A-4A05-80C6-33D66338B6DE")) IWorkspace : public IUnknown
{
    void Save() const;
    void Restore();
    bool Remove();

    const TCHAR * GetID() const;
    const TCHAR * GetLabel() const;
    const TCHAR *GetChecksum() const;
    bool IsDefault() const;

    unsigned int GetChildWorkspaces(IWorkspaceVector & results) const;
    void PasteWorkspace(IWorkspace * workspace);
    IWorkspace * CreateChildWorkspace(const std::_tstring & label);

    unsigned int GetWindows(IWorkspaceItemVector & results) const;
    void SetWindows(const IWorkspaceItemVector & windows);
    void SetWindows(const CPersistMapVector & windows);
    void AddWindow(const IWorkspaceItem & window);

    bool Exists() const;
    bool Create();
};

class IWorkspaceCompare
{
public:
    bool operator ()(IWorkspaceAdapt & l, IWorkspaceAdapt & r)
    {
        CString lhs = l->GetLabel();
        return lhs.CompareNoCase(r->GetLabel()) > 0 ? false : true;
    }
};

IWorkspace * CreateIWorkspace(IRepository * repository, const std::_tstring & label = _T(""), bool showInRoot = true, bool placeholder = false, bool attributesOnly = false);
unsigned int GetWorkspaces(IRepository * repository, IWorkspaceVector * results, bool attributesOnly);
bool WorkspaceExists(IRepository * repository, const std::_tstring & label);
bool RemoveWorkspace(IRepository * repository, const std::_tstring & label);
