#pragma once

#include "MultiTreeNode.h"
#include "repository.h"
#include <CustomMessages.h>

//MaskIcons
#define STATE_UNKNOWN 0
#define STATE_LOCKED 1
#define STATE_CHECKEDOUT 2
#define STATE_FLAGGED 3 
#define STATE_EDITED 4
#define STATE_NONE 5
#define STATE_LOCKED2 6
#define STATE_LOCKED3 7
#define STATE_CHECKEDOUT_SANDBOXED 8
#define STATE_SANDBOXED 9
#define STATE_CHECKEDOUT_OTHER5 10
#define STATE_ORPHANED 11
#define STATE_PLACEHOLDER 12 

struct CRepositorySelections;

enum RVUM
{
    RVUM_FIRST = CWM_LAST + 1,
    RVUM_REFRESHMODULENODE,
    RVUM_LAST
};

//  ===========================================================================
__interface IRepositorySlot
{
    void OnDoubleClick(IRepository &rep);
    void OnDoubleClick(IModule &mod);
    void OnDoubleClick(IAttribute &wu);
    void OnDoubleClick(IAttributeHistory &history, Dali::CEclExceptionVector * errors);
    void OnItemSelectionChanged(CTreeNode *node, bool bSelected);
    void OnExpandItem(CTreeNode *node, bool bExpanding);
    void OnContext(CRepositorySelections &s, CPoint &pt);
    void OnLoading();								//Not on main thread
    void OnLoaded(IModuleVector & modules);			//Not on main thread
    void OnLoaded(IModule * module, IAttributeVector & attributes);	//Not on main thread
    void SetAttributeText(IAttribute *attr, const std::_tstring &ecl, bool resetSavePoint, bool clearUndo);
    void UpdateAttribute(IAttribute *from, IAttribute *to);
    void UpdateModule(IModule *from, IModule *to);
    IRepository * GetRepository(); 
    bool ExpandModule(IModule &mod);
    bool DoInsertAttribute(CComPtr<IModule> module);
};

class CRepositorySlotImpl : public IRepositorySlot
{
protected:
    CRepositorySlotImpl() {};

public:
    void OnDoubleClick(IRepository & /*rep*/) {}
    void OnDoubleClick(IModule & /*mod*/) {}
    void OnDoubleClick(IAttribute & /*wu*/) {}
    void OnDoubleClick(IAttributeHistory & /*history*/, Dali::CEclExceptionVector * errors) {}
    void OnItemSelectionChanged(CTreeNode * /*node*/, bool /*bSelected*/) {}
    void OnExpandItem(CTreeNode * /*node*/, bool /*bExpanding*/) {}
    void OnContext(CRepositorySelections & /*s*/, CPoint & /*pt*/) {}
    void OnLoading() {}
    void OnLoaded(IModuleVector & modules) {} 
    void OnLoaded(IModule * module, IAttributeVector & attributes) {} 
    void SetAttributeText(IAttribute * /*attr*/, const std::_tstring & /*ecl*/, bool /*resetSavePoint*/, bool /*clearUndo*/) {}
    void UpdateAttribute(IAttribute * /*from*/, IAttribute * /*to*/) {}
    void UpdateModule(IModule * /*from*/, IModule * /*to*/) {}
    bool ExpandModule(IModule & /*mod*/) 
    {
        return true;
    }
    bool DoInsertAttribute(CComPtr<IModule> module)
    {
        return false;
    }
    IRepository * GetRepository()
    {
        return AttachRepository();
    }
};

class WLIB_API __declspec(uuid("8175886e-3d91-4c35-b365-15fcb706bf90")) CRepositoryBaseNode : public CTreeNode, public boost::noncopyable
{
protected:
    clib::recursive_mutex m_mutex;
    IRepositorySlot *m_owner;

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(CRepositoryBaseNode)
    END_CUNKNOWN(CTreeNode)

    CRepositoryBaseNode();
    CRepositoryBaseNode(IRepositorySlot *pOwner);
};

class WLIB_API __declspec(uuid("8175886e-3d91-4c35-b365-15fcb706bf91")) CRepositoryNode : public CRepositoryBaseNode
{
protected:
    CComPtr<CLoadingNode> m_loading;
    bool m_bVirtualNode;

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(CRepositoryNode)
    END_CUNKNOWN(CRepositoryBaseNode)

    CRepositoryNode();
    CRepositoryNode(IRepositorySlot *pOwner, bool m_bVirtualNode);
    virtual ~CRepositoryNode();

    void SetLoadingNode(CLoadingNode * loading);
    HTREEITEM InsertBelow(WTL::CTreeViewCtrlEx &tc, HTREEITEM parent, bool bLast = true);
    BOOL Expand(UINT nCode = TVE_EXPAND);

    virtual void ItemExpanding();
    static void GetModules(CComPtr<CRepositoryNode> self, CComPtr<CLoadingNode> loading);
    void LoadModules(IModuleVector & modules, CLoadingNode * loading);
    virtual const TCHAR * GetID();
    virtual const TCHAR * GetDispText();
    IRepository * GetRepository();
};

typedef StlLinked<CRepositoryNode> CRepositoryNodeAdapt;
typedef std::vector<CRepositoryNodeAdapt> CRepositoryNodeVector;

class WLIB_API __declspec(uuid("8175886e-3d91-4c35-b365-15fcb706bf92")) CModuleNode : public CRepositoryBaseNode, public boost::signals::trackable
{
protected:
    CComPtr<IModule> m_module;
    boost::signals::connection m_sigConn;
    bool m_nextExpandShouldBeFast;

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(CModuleNode)
    END_CUNKNOWN(CRepositoryBaseNode)

    CModuleNode();
    CModuleNode(IRepositorySlot *pOwner, IModule * mod);
    ~CModuleNode();
    virtual void OnDblclk(const WTL::CTreeItem &ti);
    virtual void ItemExpanding();
    virtual const TCHAR * GetID();
    virtual const TCHAR * GetDispText();
    int GetDispImage();
    int GetDispSelectedImage();
    int GetDispChildren();
    ATTRSTATE GetState();
    void operator()(IModule *module, REFRESH_MODULE refreshType);

    IModule * GetModule();
    void SetNextExpandShouldBeFast();
};

typedef StlLinked<CModuleNode> CModuleNodeAdapt;
typedef std::vector<CModuleNodeAdapt> CModuleNodeVector;

enum SYNTAX_CHECK
{
    SYNTAX_CHECK_UNKNOWN,
    SYNTAX_CHECK_PENDING,
    SYNTAX_CHECK_FAIL,
    SYNTAX_CHECK_WARN,
    SYNTAX_CHECK_PASS
};

class WLIB_API __declspec(uuid("8175886e-3d91-4c35-b365-15fcb706bf93")) CAttributeNode : public CRepositoryBaseNode, public boost::signals::trackable
{
protected:
    CComPtr<IAttribute> m_attribute;
    boost::signals::connection m_sigConn;
    std::_tstring m_DispText;
    virtual void GenerateDispText();
    bool m_displayQualified;
    SYNTAX_CHECK m_syntaxCheck;
    std::_tstring m_syntaxSummary;

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(CAttributeNode)
    END_CUNKNOWN(CRepositoryBaseNode)

    CAttributeNode();
    CAttributeNode(IRepositorySlot *pOwner, IAttribute * attr, bool displayQualified = false);
    ~CAttributeNode();

    virtual const TCHAR * GetID();
    virtual const TCHAR * GetDispText();
    virtual int GetDispChildren();
    virtual int GetDispImage();
    virtual int GetDispSelectedImage();
    virtual void OnDblclk(const WTL::CTreeItem &ti);
    virtual const TCHAR * GetToolTip(std::_tstring & result);
    IAttribute * GetAttribute();
    ATTRSTATE GetState();
    void operator()(IAttribute *attr, bool eclChanged, IAttribute * newAttrAsOldOneMoved, bool deleted);
    void operator()(ATTRSTATE state);
    virtual void Refresh();
    virtual bool CanEdit();
    const TCHAR *GetText(bool bRefresh=false);
    void SetSyntaxState(SYNTAX_CHECK state);
    SYNTAX_CHECK CheckSyntax();
};

typedef StlLinked<CAttributeNode> CAttributeNodeAdapt;
typedef std::vector<CAttributeNodeAdapt> CAttributeNodeVector;
//  ===========================================================================
class WLIB_API __declspec(uuid("8175886e-3d91-4c35-b365-15fcb706bf94")) CModuleFilterNode : public CModuleNode
{
protected:
    IAttributeVector m_attrs;

public:
    CModuleFilterNode(IRepositorySlot *owner, IModule * mod);
    void AddAttribute(IAttribute *attr);
    bool HasAttribute(IAttribute *attr);
    virtual void ItemExpanding();

    virtual const TCHAR * GetDispText();
};

typedef StlLinked<CModuleFilterNode> CModuleFilterNodeAdapt;
typedef std::vector<CModuleFilterNodeAdapt> CModuleFilterNodeVector;
typedef	std::map<std::_tstring, CModuleFilterNodeAdapt> CModuleFilterMap;

class WLIB_API __declspec(uuid("8175886e-3d91-4c35-b365-15fcb706bf95")) CRepositoryFilterNode : public CRepositoryNode
{
protected:
    CComPtr<IAttributeVectorPtr> m_results;
    CModuleFilterMap m_modules;

public:
    CRepositoryFilterNode();
    CRepositoryFilterNode(IRepositorySlot *pMenuManager, const std::_tstring & label, IAttributeVectorPtr *results);
    virtual void ItemExpanding();
};
//  ===========================================================================
class WLIB_API __declspec(uuid("893E784A-FD84-4449-8169-D380DB02C1F4")) CAttributeHistoryNode : public CRepositoryBaseNode, public boost::signals::trackable
{
protected:
    CComPtr<IAttributeHistory> m_attributeHistory;
    std::_tstring m_DispText;
    void virtual GenerateDispText(void);
    SYNTAX_CHECK m_syntaxCheck;
    bool m_singleDisplayMode;
    Dali::CEclExceptionVector m_errors;

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(CAttributeHistoryNode)
    END_CUNKNOWN(CRepositoryBaseNode)

    CAttributeHistoryNode();
    CAttributeHistoryNode(IRepositorySlot *pOwner, IAttributeHistory * hist, bool singleDisplayMode);

    virtual const TCHAR * GetID();
    virtual const TCHAR * GetDispText();
    virtual int GetDispChildren();
    virtual int GetDispImage();
    virtual int GetDispSelectedImage();
    virtual void OnDblclk(const WTL::CTreeItem &ti);
    IAttributeHistory * GetAttributeHistory() { return m_attributeHistory; };
    const TCHAR *GetText();
    void SetSyntaxState(SYNTAX_CHECK state);
    SYNTAX_CHECK GetSyntaxState() const;
    SYNTAX_CHECK CheckSyntax();
    void operator()(IAttribute *attr, bool eclChanged, IAttribute * newAttrAsOldOneMoved, bool deleted);
    virtual void Refresh();
};
typedef StlLinked<CAttributeHistoryNode> CAttributeHistoryNodeAdapt;
typedef std::vector<CAttributeHistoryNodeAdapt> CAttributeHistoryNodeVector;

class WLIB_API __declspec(uuid("6F2ACD26-5E0F-4536-9202-3C73A2316DE6")) CAttributeHistoryNodePlaceHolder : public CAttributeHistoryNode
{
public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(CAttributeHistoryNodePlaceHolder)	
    END_CUNKNOWN(CAttributeHistoryNode)

    CAttributeHistoryNodePlaceHolder(IRepositorySlot *pOwner, IAttributeHistory * hist, bool singleDisplayMode);

    virtual const TCHAR * GetID();
    virtual const TCHAR * GetDispText();
};
