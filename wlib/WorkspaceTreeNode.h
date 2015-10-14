#pragma once

#include "TreeNode.h"
#include "Thread.h"
#include "Workspace.h"
#include "RepositoryTreeNode.h"
#include "DaliTreeNode.h"

//  ===========================================================================
__interface IWorkspaceSlot
{
    void OnContext(CTreeNodeVector &s, CPoint &pt);
    void RestoreState();
    //void OnDoubleClick(IProject &rep);
    //void OnDoubleClick(IModule &mod);
    //void OnDoubleClick(IAttribute &wu);
    //void OnDoubleClick(IAttributeHistory &history);
    //void OnItemSelectionChanged(CTreeNode *node, bool bSelected);
    //void OnExpandItem(CTreeNode *node, bool bExpanding);
    //void OnContext(CProjectSelections &s, CPoint &pt);
    //void OnLoading();								//Not on main thread
    //void OnLoaded(IModuleVector & modules);			//Not on main thread
    //void OnLoaded(IModule * module, IAttributeVector & attributes);	//Not on main thread
    //void SetAttributeText(IAttribute *attr, const CString &ecl, bool resetSavePoint, bool clearUndo);
    //void UpdateAttribute(IAttribute *from, IAttribute *to);
    //void UpdateModule(IModule *from, IModule *to);
    //IProject * GetProject(); //AttachProject( GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_ATTRIBUTE), _T("Project"))
    //bool ExpandModule(IModule &mod);
};

class CWorkspaceSlotImpl : public IWorkspaceSlot
{
protected:
    CWorkspaceSlotImpl() {};

public:
    void OnContext(CTreeNodeVector &s, CPoint &pt)
    {
    }
    void RestoreState()
    {
    }
    //void OnDoubleClick(IProject & /*rep*/) {}
    //void OnDoubleClick(IModule & /*mod*/) {}
    //void OnDoubleClick(IAttribute & /*wu*/) {}
    //void OnDoubleClick(IAttributeHistory & /*history*/) {}
    //void OnItemSelectionChanged(CTreeNode * /*node*/, bool /*bSelected*/) {}
    //void OnExpandItem(CTreeNode * /*node*/, bool /*bExpanding*/) {}
    //void OnContext(CProjectSelections & /*s*/, CPoint & /*pt*/) {}
    //void OnLoading() {}
    //void OnLoaded(IModuleVector & modules) {} 
    //void OnLoaded(IModule * module, IAttributeVector & attributes) {} 
    //void SetAttributeText(IAttribute * /*attr*/, const CString & /*ecl*/, bool /*resetSavePoint*/, bool /*clearUndo*/) {}
    //void UpdateAttribute(IAttribute * /*from*/, IAttribute * /*to*/) {}
    //void UpdateModule(IModule * /*from*/, IModule * /*to*/) {}
    //bool ExpandModule(IModule & /*mod*/) 
    //{
    //	return true;
    //}
    //IRepository * GetRepository()
    //{
    //	return AttachRepository();
    //}
};
class WLIB_API __declspec(uuid("1275886e-3d91-4c35-b365-15fcb706be79")) CWorkspaceBaseNode : public CTreeNode, public boost::noncopyable
{
protected:
    clib::recursive_mutex m_mutex;
    IWorkspaceSlot * m_owner;
    IRepositorySlot * m_repOwner;
    IDaliSlot * m_daliOwner;
    CComPtr<CLoadingNode> m_loading;

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(CWorkspaceBaseNode)
    END_CUNKNOWN(CTreeNode)

    CWorkspaceBaseNode();
    CWorkspaceBaseNode(IWorkspaceSlot *pOwner, IRepositorySlot * pRepOwner, IDaliSlot * pDaliOwner);
};
//  ===========================================================================
class WLIB_API __declspec(uuid("46EEDCAE-FECD-4071-9BFE-542D7F15BAC1")) CWorkspaceRootNode : public CWorkspaceBaseNode //Virtual root node - just a place holder to load the workspaces
{
protected:
    clib::recursive_mutex m_mutex;
    CComPtr<CLoadingNode> m_loading;

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(CWorkspaceRootNode)
    END_CUNKNOWN(CWorkspaceBaseNode)

    CWorkspaceRootNode();
    CWorkspaceRootNode(IWorkspaceSlot *pOwner, IRepositorySlot * pRepOwner, IDaliSlot * pDaliOwner);
    virtual ~CWorkspaceRootNode();

    HTREEITEM InsertBelow(WTL::CTreeViewCtrlEx &tc, HTREEITEM parent, bool bLast = true);
    BOOL Expand(UINT nCode = TVE_EXPAND);

    virtual void ItemExpanding();
    static void GetWorkspaces(CComPtr<CWorkspaceRootNode> self, CComPtr<CLoadingNode> loading);
    void LoadWorkspaces(IWorkspaceVector & workspaces, CLoadingNode * loading);
    virtual const TCHAR * GetID();
    virtual const TCHAR * GetDispText();
    //IProject * GetProject();
};

typedef StlLinked<CWorkspaceRootNode> CWorkspaceRootNodeAdapt;
typedef std::vector<CWorkspaceRootNodeAdapt> CWorkspaceRootNodeVector;
//  ===========================================================================
class WLIB_API __declspec(uuid("6959D10A-215A-425F-8443-45D1171EA643")) CWorkspaceNode : public CWorkspaceBaseNode 
{
protected:
    clib::recursive_mutex m_mutex;
    CComPtr<IWorkspace> m_workspace;
    bool m_showChildren;

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(CWorkspaceNode)
    END_CUNKNOWN(CWorkspaceBaseNode)

    CWorkspaceNode();
    CWorkspaceNode(IWorkspaceSlot *pOwner, IRepositorySlot * pRepOwner, IDaliSlot * pDaliOwner, IWorkspace * workspace, bool showChildren = true);
    virtual ~CWorkspaceNode();

    virtual void ItemExpanding();
    static void GetWorkspaces(CComPtr<CWorkspaceNode> self, IWorkspaceVector * workspaces, CComPtr<CLoadingNode> loading);
    static void GetWindows(CComPtr<CWorkspaceNode> self, CPersistMapVector * pmaps, CComPtr<CLoadingNode> loading);
    static void GetWorkspaceAttributes(CComPtr<CWorkspaceNode> self, CComPtr<CLoadingNode> loading);
    void LoadWorkspacesAttributes(IWorkspaceVector & workspaces, CPersistMapVector * pmaps, CLoadingNode * loading);
    virtual const TCHAR * GetID();
    virtual int GetDispChildren();
    virtual const TCHAR * GetDispText();
    virtual int GetDispImage();
    virtual int GetDispSelectedImage();

    IWorkspace * GetWorkspace();

    void operator()(IWorkspace * workspace);
};

typedef StlLinked<CWorkspaceNode> CWorkspaceNodeAdapt;
typedef std::vector<CWorkspaceNodeAdapt> CWorkspaceNodeVector;
//  ===========================================================================
class WLIB_API __declspec(uuid("7059D10A-215A-425F-8443-45D1171EA654")) CBuilderWindowNode : public CWorkspaceBaseNode, public CDaliSlotImpl
{
protected:
    clib::recursive_mutex m_mutex;
    CComPtr<IWorkspaceItem> m_window;

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(CBuilderWindowNode)
    END_CUNKNOWN(CWorkspaceBaseNode)

    CBuilderWindowNode();
    CBuilderWindowNode(IWorkspaceSlot *pOwner, IRepositorySlot * pRepOwner, IDaliSlot * pDaliOwner, IWorkspaceItem * workspace);
    virtual ~CBuilderWindowNode();

    virtual void ItemExpanding();
    virtual const TCHAR * GetID();
    virtual int GetDispChildren();
    virtual const TCHAR * GetDispText();
    virtual int GetDispImage();
    virtual int GetDispSelectedImage();

    IWorkspaceItem * GetWorkspaceItem();

    void operator()(IWorkspaceItem * window);
};

typedef StlLinked<CBuilderWindowNode> CBuilderWindowNodeAdapt;
typedef std::vector<CBuilderWindowNodeAdapt> CBuilderWindowNodeVector;
//  ===========================================================================
class WLIB_API __declspec(uuid("E6CA8967-0AC9-4D34-8838-8262E7AC6DCC")) CAttributeWindowNode : public CAttributeNode 
{
protected:
    clib::recursive_mutex m_mutex;
    CComPtr<IWorkspaceItem> m_window;

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(CAttributeWindowNode)
    END_CUNKNOWN(CAttributeNode)

    CAttributeWindowNode();
    CAttributeWindowNode(IWorkspaceSlot *pOwner, IRepositorySlot * pRepOwner, IDaliSlot * pDaliOwner, IWorkspaceItem * workspace);
    virtual ~CAttributeWindowNode();

    IWorkspaceItem * GetWorkspaceItem();

    void operator()(IWorkspaceItem * window);
};

typedef StlLinked<CAttributeWindowNode> CAttributeWindowNodeAdapt;
typedef std::vector<CAttributeWindowNodeAdapt> CAttributeWindowNodeVector;
//  ===========================================================================
