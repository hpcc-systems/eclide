#pragma once

#include "MultiTreeNode.h"
#include "Dali.h"
#include "Workunit.h"
#include "smc.h"

__interface IDaliSlot
{
    void OnDoubleClick(Dali::IWorkunit &wu);
    HWND GetHWND();
    bool ShowClusterInLabel(const TCHAR *cluster);
    bool ShowUserInLabel(const TCHAR *user);
    void UpdateWorkunits(HTREEITEM hItem, Dali::IWorkunitVectorAdapt wus, int allUsers = -1);
};
class CDaliSlotImpl : public IDaliSlot
{
protected:
    CDaliSlotImpl() {};

public:
    void OnDoubleClick(Dali::IWorkunit &wu)
    {
    }
    HWND GetHWND()
    {
        return NULL;
    }
    bool ShowClusterInLabel(const TCHAR *cluster)
    {
        return false;
    }
    bool ShowUserInLabel(const TCHAR *user)
    {
        return false;
    }
    void UpdateWorkunits(HTREEITEM hItem, Dali::IWorkunitVectorAdapt wus, int allUsers = -1)
    {
    }
};
//  ===========================================================================
class WLIB_API __declspec(uuid("d985a935-4152-4703-8e5a-0e3bc9268ff0")) CDaliBaseNode : public CTreeNode
{
protected:
    IDaliSlot *m_Owner;
    bool m_AllUsers;
    std::_tstring m_Cluster;

public:
    CDaliBaseNode(){}
    CDaliBaseNode(IDaliSlot *owner, bool allUsers, const std::_tstring & cluster);
};

class CWorkunitNode;

class WLIB_API __declspec(uuid("d985a935-4152-4703-8e5a-0e3bc9268ff1")) CDaliNode : public CDaliBaseNode
{
protected:
    CComPtr<CLoadingNode> m_loadingNode;

    void UpdateToday();
    bool m_bVirtualNode;

    HTREEITEM GetRoot();

public:
    CDaliNode(IDaliSlot *owner, bool allUsers, const std::_tstring &cluster, bool bVirtualNode = false);
    ~CDaliNode();
    virtual void ItemExpanding();
    virtual const TCHAR * GetID();
    virtual const TCHAR * GetDispText();
    void Refresh(bool allUsers, const std::_tstring & cluster);
    void AddItems(WTL::CTreeViewCtrlEx &tv);
    void AddItem(WTL::CTreeViewCtrlEx &tv, Dali::IWorkunit * wu);
    void operator()(Dali::IWorkunitVectorAdapt wus);
	void CDaliNode::callback(Dali::IWorkunitVectorAdapt wus, int allUsers);
};

class WLIB_API __declspec(uuid("d985a935-4152-4703-8e5a-0e3bc9268ff2")) CDateFolderNode : public CDaliBaseNode, public boost::signals::trackable
{
protected:
    boost::gregorian::date m_from;
    boost::gregorian::date_duration m_duration;
    std::_tstring m_id;
    std::_tstring m_label;
    CComPtr<CLoadingNode> m_loadingNode;

public:
    CDateFolderNode(IDaliSlot *owner, bool allUsers, const std::_tstring & cluster, const std::_tstring & label, const boost::gregorian::date &from, const boost::gregorian::date_duration &duration);
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(/* [in] */ REFIID riid, /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppUnk);
    virtual void ItemExpanding();
    virtual const TCHAR * GetID();
    virtual const TCHAR * GetDispText();
    bool Contains(Dali::IWorkunit * wu);
    void Refresh(bool allUsers, const std::_tstring & cluster);

    void AddItems(const std::_tstring & fromUTC, const std::_tstring & toUTC);

    void operator()(Dali::IWorkunitVectorAdapt workunits);
};

class WLIB_API __declspec(uuid("d985a935-4152-4703-8e5a-0e3bc9268ff3")) CWorkunitNode : public CDaliBaseNode, public boost::signals::trackable
{
protected:
    CComPtr<Dali::IWorkunit> m_wu;
    std::_tstring m_label;
    boost::signals::connection m_sc;

public:
    CWorkunitNode(){}
    CWorkunitNode(IDaliSlot *owner, Dali::IWorkunit * wu);
    ~CWorkunitNode();
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(/* [in] */ REFIID riid, /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppUnk);
    virtual int GetDispChildren();
    virtual int GetDispImage();
    virtual int GetDispSelectedImage();
    Dali::WUState GetState();
    void operator()(Dali::WUEvent evt, Dali::IWorkunit * wu);
    virtual const TCHAR * GetID();
    virtual const TCHAR * GetDispText();
    Dali::IWorkunit * GetWorkunit();
    void OnDblclk(const WTL::CTreeItem &ti);
    const TCHAR * GetToolTip(std::_tstring & result);
};
//  ===========================================================================
class WLIB_API __declspec(uuid("d985a935-4152-4703-8e5a-0e3bc9268ff4")) CSMCNode : public CTreeNode
{
protected:
    IDaliSlot *m_Owner;
public:
    CSMCNode(IDaliSlot *owner);
    virtual void ItemExpanding();
    virtual const TCHAR * GetID();
    virtual const TCHAR * GetDispText();
    void Refresh();
};
//  ===========================================================================
class WLIB_API __declspec(uuid("d985a935-4152-4703-8e5a-0e3bc9268ff5")) CSMCClusterNode : public CTreeNode
{
protected:
    IDaliSlot *m_Owner;
    CComPtr<SMC::ICluster> m_cluster;

public:
    CSMCClusterNode(IDaliSlot *owner, SMC::ICluster *cluster);
    virtual void ItemExpanding();
    virtual const TCHAR * GetID();
    virtual const TCHAR * GetDispText();
    int GetDispChildren();
    void Refresh();
};
//  ===========================================================================
