#pragma once

#include "resource.h"
#include "MultiTreeNode.h"
#include "RepositoryView.h"
#include "WorkspaceTreeNode.h"
#include <Thread.h> //clib
#include "Combo.h"
#include "util.h"
#include "utilDateTime.h"
#include <CustomMessages.h>

template<typename T>
class CWorkspaceViewT
{
protected:
    IWorkspaceSlot *m_owner;
    IRepositorySlot *m_repOwner;
    IDaliSlot *m_daliOwner;
    CString m_label;
    CComPtr<CWorkspaceRootNode> m_Root;
    CComPtr<IWorkspace> m_currWorkspace;

public:
    CMultiTreeNodeViewCtrl m_Tree;
        
    void SetRootLabel(const CString & label)
    {
        m_label = label;
    }

    void SetCurrentWorkspace(IWorkspace * workspace)
    {
        m_currWorkspace = workspace;
        if (!m_currWorkspace)
            m_currWorkspace = m_repOwner->GetRepository()->CreateIWorkspace();

        bool found = false;
        for (WTL::CTreeItem pos = m_Tree.GetRootItem(); pos != NULL; pos = m_Tree.GetNextItem(pos, TVGN_EX_ALL))
        {
            if (CComQIPtr<CWorkspaceNode> node = (CUnknown *)pos.GetData())
            {
                if (node->GetWorkspace() == m_currWorkspace)
                {
                    pos.SetState(TVIS_BOLD, TVIS_BOLD);
                    found = true;
                }
                else
                    pos.SetState(0, TVIS_BOLD);
            }
        }
        if (!found)
        {
            CWorkspaceNode * newNode = new CWorkspaceNode(m_owner, m_repOwner, m_daliOwner, workspace);
            WTL::CTreeItem newItem(newNode->InsertBelow(m_Tree, TVI_ROOT), &m_Tree);
            newNode->operator ()(workspace);
            newItem.SetState(TVIS_BOLD, TVIS_BOLD);
        }
    }

    IWorkspace * GetCurrentWorkspace() 
    {
        if (!m_currWorkspace)
            m_currWorkspace = m_repOwner->GetRepository()->CreateIWorkspace();
        return m_currWorkspace;
    }

    void RemoveWorkspace(IWorkspace * workspace)
    {
        bool found = false;
        for (WTL::CTreeItem pos = m_Tree.GetRootItem(); pos != NULL; pos = m_Tree.GetNextItem(pos, TVGN_EX_ALL))
        {
            if (CComQIPtr<CWorkspaceNode> node = (CUnknown *)pos.GetData())
            {
                if (node->GetWorkspace() == workspace)
                {
                    m_Tree.DeleteItem(pos);
                    break;
                }
            }
        }
        if (m_currWorkspace == workspace)
            m_currWorkspace = m_repOwner->GetRepository()->CreateIWorkspace();
    }

    UINT GetSelectedCount()
    {
        return m_Tree.GetSelectedCount();
    }

    CAttributeNode *GetAttributeNode(WTL::CTreeItem &item)
    {
        CComQIPtr<CAttributeNode> attrNode = reinterpret_cast<CTreeNode*>(item.GetData());
        return attrNode.p;
    }

    bool GetSelected(CTreeNodeVector &s)
    {
        for(WTL::CTreeItem item = m_Tree.GetFirstSelectedItem(); item; item = m_Tree.GetNextSelectedItem(item))
        {
            DWORD_PTR data = item.GetData();
            s.push_back(reinterpret_cast<CTreeNode*>(data));
        }
        return true;
    }

    IAttribute *GetFirstSelectedAttr()
    {
        CComPtr<CAttributeNode> node = m_Tree.GetFirstSelected<CAttributeNode>();
        if (node)
            return node->GetAttribute();
        return NULL;
    }

    IModule *GetFirstSelectedModule()
    {
        CComPtr<CModuleNode> node = m_Tree.GetFirstSelected<CModuleNode>();
        if (node)
            return node->GetModule();
        return NULL;
    }

    void DoCopy(const IAttributeVector & attrs, const std::_tstring & delim = _T(", "))
    {
        CString txt;

        for(IAttributeVector::const_iterator itr = attrs.begin(); itr != attrs.end(); ++itr)
        {
            if (itr != attrs.begin())
                txt += delim.c_str();
            txt += CString(itr->get()->GetModuleLabel()) + "." + CString(itr->get()->GetLabel());
        }

        SetClipboard((const TCHAR * )txt);
        return;
    }
    void DoCheckout(IAttributeVector & attrs)
    {
        CComPtr<IProject> rep = m_Owner->GetProject();
        rep->Checkout(attrs);
    }
    void DoCheckin(IAttributeVector & attrs, const CString & comment)
    {
        CComPtr<IProject> rep = m_Owner->GetProject();
        rep->Checkin(attrs, comment);
    }
    void DoRollback(IAttributeVector & attrs)
    {
        CComPtr<IProject> rep = m_Owner->GetProject();
        if (rep->Rollback(attrs))
        {
            for(IAttributeVector::iterator itr = attrs.begin(); itr != attrs.end(); ++itr)
                m_Owner->SetAttributeText(itr->get(), itr->get()->GetText(), true, false);
        }
    }
    void DoInsertModule(const CString label)
    {
        CComPtr<IProject> rep = m_Owner->GetProject();
        IModule *mod = rep->InsertModule(label);
        if (mod)
        {
            RefreshProject();
            Select(mod);
        }
    }
    void DoRenameModule(IModule * mod, const CString & label)
    {
        CComPtr<IModule> newMod = mod->Rename(label);
        if (newMod)
        {
            RefreshProject();
            Select(newMod);
            m_Owner->UpdateModule(mod, newMod);
        }
    }
    void DoDeleteModule(IModuleVector & mods)
    {
        T* pT = static_cast<T*>(this);
        std::_tstring title = _T("Delete Folder");
        if (mods.size() > 1)
            title += _T("s");
        std::_tstring warning = _T("The following folders will be deleted, are you sure?\r\n");
        for(IModuleVector::iterator itr = mods.begin(); itr != mods.end(); ++itr)
        {
            warning += _T("\t");
            warning += itr->get()->GetLabel();
            warning += _T("\r\n");
        }

        if (pT->MessageBox(warning.c_str(), title.c_str(), MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) == IDYES)
        {
            CComPtr<IProject> rep = m_Owner->GetProject();

            std::map<CString, bool> modDedup;
            for(IModuleVector::iterator itr = mods.begin(); itr != mods.end(); ++itr)
                itr->get()->Delete();

            RefreshProject();
        }
    }
    bool DoInsertAttribute(IModule *mod, const CString label)
    {
        CComPtr<IAttribute> attr = mod->InsertAttribute(label);
        ClearShortTermCache();
        TreeNode::RefreshChildren(mod->GetLabel());
        if (attr)
        {
            attr->Checkout();
            CString ecl;
            ecl.Format(_T("export %s := 'todo';"), attr->GetLabel());
            attr->SetText(ecl);
            Select(attr);
            return true;
        }
        return false;

    }
    void DoRenameAttribute(IAttribute * attr, const CString & label)
    {
        CString moduleLabel = attr->GetModuleLabel();
        CComPtr<IAttribute> newAttr = attr->Rename(label);
        if (newAttr)
        {
            ClearShortTermCache();
            TreeNode::RefreshChildren(mod->GetLabel());
            Select(newAttr);
            m_Owner->UpdateAttribute(attr, newAttr);
        }
    }
    void DoMoveAttributeNoRefreshTarget(IAttributeVector & attrs, const CString & target)
    {
        bool retVal = false;
        CComPtr<IProject> rep = m_Owner->GetProject();
        if (rep->Move(attrs, target))
        {
            std::map<CString, bool> modDedup;
            for(IAttributeVector::iterator itr = attrs.begin(); itr != attrs.end(); ++itr)
            {
                modDedup[itr->get()->GetModuleLabel()] = true;
                CComPtr<IAttribute> newAttr = rep->GetAttribute(target, itr->get()->GetLabel());
                m_Owner->UpdateAttribute(itr->get(), newAttr);
            }
            //  No longer needed as attribute nodes now self delete on a  move.
            //for(std::map<CString, bool>::iterator itr = modDedup.begin(); itr != modDedup.end(); ++itr)
            //	RefreshModule(itr->first);
            retVal = true;
        }
        return retVal;
    }
    void DoMoveAttribute(IAttributeVector & attrs, const CString & target)
    {
        DoMoveAttributeNoRefreshTarget(attrs, target);
        ClearShortTermCache();
        TreeNode::RefreshChildren(target);
    }
    void DoDeleteAttribute(IAttributeVector & attrs)
    {
        T* pT = static_cast<T*>(this);
        std::_tstring plurral = attrs.size() > 1 ? _T("s") : _T("");
        std::_tstring title = (boost::_tformat(_T("Delete File%1%")) % plurral).str();
        std::_tstring warning = (boost::_tformat(_T("Are you sure you want to move %1% file%2% to the Trash folder?")) % attrs.size() % plurral).str();
        if (pT->MessageBox(warning.c_str(), title.c_str(), MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) == IDYES)
        {
            std::_tstring dateTime;
            CurrentDateTimeUTCString(dateTime);
            for(IAttributeVector::iterator itr = attrs.begin(); itr != attrs.end(); ++itr)
            {
                std::_tstring modLabel = itr->get()->GetModuleLabel();
                std::_tstring attrLabel = itr->get()->GetLabel();
                *itr = itr->get()->Rename((boost::_tformat(_T("%1%::%2%::%3%")) % dateTime % modLabel % attrLabel).str().c_str());
            }
            //Could now be NULLS from failed renames!
            bool failed = false;
            IAttributeVector cleaned_attrs;
            for(IAttributeVector::iterator itr = attrs.begin(); itr != attrs.end(); ++itr)
            {
                if (itr->get())
                {
                    cleaned_attrs.push_back(*itr);
                }
                else
                {
                    failed = true;
                }
            }
            DoMoveAttributeNoRefreshTarget(cleaned_attrs, TRASH);
            CString prog_title;
            prog_title.LoadString(IDR_MAINFRAME);
            pT->MessageBox(_T("WARNING:  Some files could not be moved."), prog_title, MB_ICONEXCLAMATION);
        }
    }
    bool Select(const TCHAR* label, bool bExpand=false)
    {
        return TreeNode::Select(label, m_Root, bExpand);
    }
    bool Select(IModule * mod)
    {
        return Select(mod->GetLabel());
    }
    bool Select(IAttribute * attr)
    {
        return Select(attr->GetQualifiedLabel());
    }
    bool RefreshProject()
    {
        T* pT = static_cast<T*>(this);
        WTL::CLockWindowUpdate lock(pT->m_hWnd);
        CComPtr<IRepository> rep = AttachRepository();
        rep->ClearShortTermCache();
        m_Root->Expand(TVE_COLLAPSE | TVE_COLLAPSERESET);
        m_Root->Expand();
        return true;
    }
};
//  ===========================================================================
class CWorkspaceView :
    public CWorkspaceViewT<CWorkspaceView>,
    public WTL::CDialogImpl<CWorkspaceView>,
    public WTL::CDialogResize<CWorkspaceView>,
    public WTL::CWinDataExchange<CWorkspaceView>
{
    typedef CWorkspaceView thisClass;
    typedef WTL::CDialogImpl<CWorkspaceView> baseClass;

public:
    enum { IDD = IDD_REPOSITORY };

    CWorkspaceView();
    ~CWorkspaceView();

    void Init(IWorkspaceSlot * pOwner, IRepositorySlot * pRepOwner, IDaliSlot * pDaliOwner);

    BOOL PreTranslateMessage(MSG* pMsg);

    void DoClear();
    void DoReset(IWorkspace * currWorkspace);
    
    BEGIN_MSG_MAP(thisClass)
        MSG_WM_INITDIALOG(OnInitDialog)
        MSG_WM_CONTEXTMENU(OnContextMenu)

        MESSAGE_HANDLER_EX(CWM_CLEAR, OnClear)
        MESSAGE_HANDLER_EX(CWM_RESET, OnReset)
        MESSAGE_HANDLER_EX(CWM_REFRESH, OnRefresh)
        MESSAGE_HANDLER_EX(CWM_INITIALIZE, OnInitialize)

        MSG_WM_PAINT(OnPaint)

        //MESSAGE_HANDLER_EX(UM_TREESELECTIONUPDATE, OnSelectionUpdated)

        REFLECT_NOTIFICATIONS()
        CHAIN_MSG_MAP(CDialogResize<thisClass>)
    END_MSG_MAP()

    BEGIN_DLGRESIZE_MAP(thisClass)
        DLGRESIZE_CONTROL(IDC_TREE_REPOSITORY, DLSZ_SIZE_X | DLSZ_SIZE_Y)
    END_DLGRESIZE_MAP()

    BEGIN_DDX_MAP(thisClass)
    END_DDX_MAP()

    LRESULT OnInitDialog(HWND hWnd, LPARAM lParam);
    LRESULT OnClear(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
    LRESULT OnReset(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
    LRESULT OnRefresh(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
    LRESULT OnInitialize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
    LRESULT OnSelectionUpdated(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
    void OnPaint(HDC hDc); 

    //void OnRefresh(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/);
    void Refresh(bool activeOnly);

    IAttribute * GetFirstSelectedAttribute();
    IModule * GetFirstSelectedModule();
    HWND GetTreeHandle();

    //  IWorkspaceSlot
    void OnContextMenu(HWND phWnd, CPoint pPoint);
    //CWorkspaceRootNode * GetRootNode() { return m_Root; };

    //IRepositoryPairedSlot *m_pPairedOwner;

    virtual void OnItemSelectionChanged(CTreeNode *, bool bSelected);
    virtual void OnItemExpanding(CTreeNode *, bool bExpanding);
    virtual void HandleNmClick(LPNMTREEVIEW);
};
//  ---------------------------------------------------------------------------
class CProjectVectorView : public CVectorView
{
protected:
    clib::CThreadQueue m_threads;
    IWorkspaceSlot * m_pOwner;
    IWorkspaceVector & m_workspaces;
    IAttributeHistoryVector & m_attrHistories;

public:
    CProjectVectorView (IWorkspaceSlot * pOwner, IRepositorySlot * pRepOwner, IWorkspaceVector & m_workspaceHistories, IAttributeHistoryVector & attrs);
    void Refresh();
    void CheckSyntax();
};
//  ===========================================================================
