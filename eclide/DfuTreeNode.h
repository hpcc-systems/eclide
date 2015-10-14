#pragma once

#include "MultiTreeNode.h"
#include "Dfu.h"
#include "thread.h"

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

struct CDfuSelections;

//  ===========================================================================
__interface IDfuSlot
{
    void OnDoubleClick(Dfu::IServer &rep);
    void OnItemSelectionChanged(CTreeNode *node, bool bSelected);
    void OnExpandItem(CTreeNode *node, bool bExpanding);
    void OnContext(CDfuSelections &s, CPoint &pt);
    Dfu::IServer * GetDfu(); //AttachDfu( GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_ATTRIBUTE), _T("Dfu"))
};

class CDfuSlotImpl : public IDfuSlot
{
protected:
    CDfuSlotImpl() {};

public:
    void OnDoubleClick(Dfu::IServer & /*rep*/) {}
    void OnItemSelectionChanged(CTreeNode * /*node*/, bool /*bSelected*/) {}
    void OnExpandItem(CTreeNode * /*node*/, bool /*bExpanding*/) {}
    void OnContext(CDfuSelections & /*s*/, CPoint & /*pt*/) {}
    Dfu::IServer * GetDfu()
    {
        CString server(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_DFU));
        return Dfu::AttachDfu(static_cast<const TCHAR *>(server), _T("Dfu"));
    }
};

class __declspec(uuid("8175886e-3d91-4c35-b365-15fcb706bf90")) CDfuBaseNode : public CTreeNode, public boost::noncopyable
{
protected:
    IDfuSlot *m_owner;
public:
    CDfuBaseNode();
    CDfuBaseNode(IDfuSlot *pOwner);
};

class __declspec(uuid("8175886e-3d91-4c35-b365-15fcb706bf91")) CDfuNode : public CDfuBaseNode
{
protected:
    clib::recursive_mutex m_mutex;
    CComPtr<CLoadingNode> m_loading;

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(CDfuNode)
    END_CUNKNOWN(CDfuBaseNode)

    CDfuNode();
    CDfuNode(IDfuSlot *pOwner, const CString & label);

    virtual ~CDfuNode();
    virtual void ItemExpanding();
    static void GetFiles(CComPtr<CDfuNode> self, CComPtr<CLoadingNode> loading);
    void LoadFiles(Dfu::IFileVector & files, CLoadingNode * loading);
    virtual const TCHAR * GetID();
    virtual const TCHAR * GetDispText();
    Dfu::IServer * GetDfu();
};

typedef StlLinked<CDfuNode> CDfuNodeAdapt;
typedef std::vector<CDfuNodeAdapt> CDfuNodeVector;

class __declspec(uuid("8175886e-3d18-4c35-b365-15fcb706be26")) CFileNode : public CDfuBaseNode
{
protected:
    CComPtr<Dfu::IFile> m_file;

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(CFileNode)
    END_CUNKNOWN(CDfuBaseNode)

    CFileNode();
    CFileNode(IDfuSlot *pOwner, Dfu::IFile * mod);

    virtual void OnDblclk(const WTL::CTreeItem &ti);
    virtual void ItemExpanding();
    virtual const TCHAR * GetID();
    virtual const TCHAR * GetDispText();
    int GetDispImage();
    int GetDispSelectedImage();
    int GetDispChildren();
    void operator()(Dfu::IFile *File);

    Dfu::IFile * GetFile();
};

class __declspec(uuid("F629137B-0397-487f-8180-1F10EB54E55C")) CFilterFileNode : public CFileNode
{
public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(CFilterFileNode)
        IMPLEMENT_INTERFACE(CFileNode)
    END_CUNKNOWN(CDfuBaseNode)

    CFilterFileNode() : CFileNode()
    {
    }
    CFilterFileNode(IDfuSlot *pOwner, Dfu::IFile * mod) : CFileNode(pOwner, mod)
    {
    }

    virtual void ItemExpanding()
    {
    }
};

typedef StlLinked<CFileNode> CFileNodeAdapt;
typedef std::vector<CFileNodeAdapt> CFileNodeVector;

struct CDfuSelections
{
    Dfu::IServerVector dfus; 
    Dfu::IFileVector files;
    CDfuNodeVector dfuNodes;
    CFileNodeVector fileNodes;
};
//  ===========================================================================
typedef	StlLinked<CFilterFileNode> CFilterFileNodeAdapt;
typedef	std::map<std::_tstring, CFilterFileNodeAdapt> CFilterFileNodeMap;
class __declspec(uuid("8175886e-3d91-4c35-c361-15fcb706bd00")) CDfuFilterNode : public CDfuNode
{
protected:
    CComPtr<Dfu::IFileVectorPtr> m_results;
    CFilterFileNodeMap m_items;

public:
    CDfuFilterNode();
    CDfuFilterNode(IDfuSlot *pMenuManager, const CString & label, Dfu::IFileVectorPtr *results);

    virtual void ItemExpanding();
protected:
    CDfuBaseNode * CDfuFilterNode::RecursiveBuild(Dfu::IFile * item);
};
//  ===========================================================================
