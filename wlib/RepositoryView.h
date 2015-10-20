#pragma once

#include "MultiTreeNode.h"
#include "Repository.h"
#include "RepositoryTreeNode.h"
#include <Thread.h> //clib
#include "Combo.h"
#include "util.h"
#include "utilDateTime.h"
#include <ModuleHelper.h>
#include "AttrListDlg.h"

template<typename T>
class CRepositoryViewT
{
protected:
    IRepositorySlot *m_Owner;
    std::_tstring m_label;
    CComPtr<CRepositoryNode> m_Root;

public:
    CMultiTreeNodeViewCtrl m_Tree;

    void SetRootLabel(const std::_tstring & label)
    {
        m_label = label;
    }

    //void SelectAll(BOOL bSelect /*= TRUE*/)
    //{
    //	m_Tree.SelectAll(bSelect);
    //}

    UINT GetSelectedCount()
    {
        return m_Tree.GetSelectedCount();
    }

    CAttributeNode *GetAttributeNode(WTL::CTreeItem &item)
    {
        CComQIPtr<CAttributeNode> attrNode = reinterpret_cast<CTreeNode*>(item.GetData());
        return attrNode.p;
    }

    bool GetSelected(CRepositorySelections &s)
    {
        IRepositoryAdapt rep;
        for(WTL::CTreeItem item = m_Tree.GetFirstSelectedItem(); item; item = m_Tree.GetNextSelectedItem(item))
        {
            DWORD_PTR data = item.GetData();
            if (CComQIPtr<CModuleNode> modNode = reinterpret_cast<CTreeNode*>(data))
            {
                if (modNode->GetModule()->IsPlugin()) 
                {
                    s.plugins.push_back(modNode->GetModule());
                    if (CComQIPtr<IDiskModule> diskModule = reinterpret_cast<IDiskModule*>(modNode->GetModule()))
                        s.diskReps.push_back(diskModule.p);
                }
                else 
                    s.mods.push_back(modNode->GetModule());
            }
            else if (CComQIPtr<CAttributeNode> attrNode = reinterpret_cast<CTreeNode*>(data))
            {
                s.attrs.push_back(attrNode->GetAttribute());
            }
            else if(CComQIPtr<CAttributeHistoryNode> historyNode = reinterpret_cast<CTreeNode*>(data))
            {
                s.histories.push_back(historyNode->GetAttributeHistory());
            }
            //else if(CComQIPtr<CWorkspaceNode> workspaceNode = reinterpret_cast<CTreeNode*>(data))
            //{
            //	s.workspaces.push_back(workspaceNode->GetWorkspace());
            //}
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
        std::_tstring txt;

        for(IAttributeVector::const_iterator itr = attrs.begin(); itr != attrs.end(); ++itr)
        {
            if (itr != attrs.begin())
                txt += delim.c_str();
            txt += std::_tstring(itr->get()->GetQualifiedLabel(true));
        }

        SetClipboard(txt);
        return;
    }
    static void thread_PreloadModule(CComPtr<IRepository> rep, std::_tstring module)
    {
        IAttributeVector attrs;
        rep->GetAttributes(module.c_str(), attrs, true, true);
    }
    void DoPaste(const std::_tstring & attrs)
    {
        CWaitCursor wait;
        CComPtr<IRepository> rep = m_Owner->GetRepository();

        //  For each attribute, ensure its module has preloaded all its attributes (speed up selection)...
        clib::CThreadQueue preloadModules(0);
        std::set<std::_tstring> modules;
        std::set<std::_tstring> attributes;

        typedef std::vector<std::_tstring> split_vector_type;
        split_vector_type lines; 
        boost::algorithm::split(lines, attrs, boost::algorithm::is_any_of(_T("\r\n")), boost::algorithm::token_compress_on); 
        for(split_vector_type::iterator itr = lines.begin(); itr != lines.end(); ++itr)
        {
            boost::algorithm::to_lower(*itr);
            attributes.insert(*itr);
            split_vector_type labels; 
            boost::algorithm::split(labels, *itr, boost::algorithm::is_any_of(_T(".")), boost::algorithm::token_compress_on); 
            if (labels.size() == 2 || labels.size() == 3)  //  Check for blank line
            {
                boost::algorithm::to_lower(labels[0]);
                modules.insert(labels[0]);
            }
        }

        for(std::set<std::_tstring>::const_iterator itr = modules.begin(); itr != modules.end(); ++itr)
        {
            preloadModules.Append(__FUNCTION__, boost::bind(thread_PreloadModule, rep, *itr));
        }

        preloadModules.SetMaxThreadCount(30);
        preloadModules.Join();

        //  Select Attributes and Expand modules.
        for (WTL::CTreeItem pos = m_Tree.GetRootItem(); pos != NULL; pos = m_Tree.GetNextItem(pos, TVGN_EX_ALL))
        {
            WTL::CTreeItem node(pos, &m_Tree);
            CTreeNode *item = (CTreeNode*)node.GetData();
            ATLASSERT(item);
            if (CComQIPtr<CModuleNode> modNode = item)
            {
                modNode->SetNextExpandShouldBeFast();
                std::_tstring qualifiedLabel = modNode->GetModule()->GetQualifiedLabel();
                boost::algorithm::to_lower(qualifiedLabel);
                if (modules.find(qualifiedLabel) != modules.end())
                    item->Expand();
            }
            else if(CComQIPtr<CAttributeNode> attrNode = item)
            {
                std::_tstring qualifiedLabel = attrNode->GetAttribute()->GetQualifiedLabel();
                boost::algorithm::to_lower(qualifiedLabel);
                if (attributes.find(qualifiedLabel) != attributes.end())
                    m_Tree.SetItemState(pos, TVIS_SELECTED, TVIS_SELECTED);
                else
                    m_Tree.SetItemState(pos, 0, TVIS_SELECTED);
            }
        }

    }
    void DoCheckout(IAttributeVector & attrs)
    {
        CComPtr<IRepository> rep = m_Owner->GetRepository();
        rep->Checkout(attrs);
    }
    void DoCheckin(IAttributeVector & attrs, const std::_tstring & comment, bool andRevert)
    {
        CComPtr<IRepository> rep = m_Owner->GetRepository();
        rep->Checkin(attrs, comment.c_str(), andRevert);
    }
    void DoCheckSyntax(IAttributeVector & attrs)
    {
        T* pT = static_cast<T*>(this);
        IRepositoryAdapt rep = m_Owner->GetRepository();
        DoShowSyntaxDlg(*pT, rep, attrs, m_Owner);
    }
    void DoRollback(IAttributeVector & attrs)
    {
        CComPtr<IRepository> rep = m_Owner->GetRepository();
        if (rep->Rollback(attrs))
        {
            for(IAttributeVector::iterator itr = attrs.begin(); itr != attrs.end(); ++itr)
                m_Owner->SetAttributeText(itr->get(), itr->get()->GetText(), true, false);
        }
    }
    void DoInsertModule(const std::_tstring & label)
    {
        CComPtr<IRepository> rep = m_Owner->GetRepository();
        IModule *mod = rep->InsertModule(label.c_str());
        if (mod)
        {
            RefreshRepository();
            Select(mod);
        }
        else
        {
            T* pT = static_cast<T*>(this);
            CString prog_title;
            prog_title.LoadString(IDR_MAINFRAME);
            pT->MessageBox(_T("WARNING:  Insert Folder failed, see error window for further details."), prog_title, MB_ICONEXCLAMATION);
        }
    }
    void DoRenameModule(IModule * mod, const std::_tstring & label)
    {
        CComPtr<IModule> newMod = mod->Rename(label.c_str());
        if (newMod)
        {
            RefreshRepository();
            Select(newMod);
            m_Owner->UpdateModule(mod, newMod);
        }
        else
        {
            T* pT = static_cast<T*>(this);
            CString prog_title;
            prog_title.LoadString(IDR_MAINFRAME);
            pT->MessageBox(_T("WARNING:  Rename Folder failed, see error window for further details."), prog_title, MB_ICONEXCLAMATION);
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
            warning += itr->get()->GetQualifiedLabel();
            warning += _T("\r\n");
        }

        if (pT->MessageBox(warning.c_str(), title.c_str(), MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDYES)
        {
            CComPtr<IRepository> rep = m_Owner->GetRepository();

            std::map<std::_tstring, bool> modDedup;
            for(IModuleVector::iterator itr = mods.begin(); itr != mods.end(); ++itr)
                itr->get()->Delete();

            RefreshRepository();
        }
    }
    bool DoInsertAttribute(IModule *mod, const std::_tstring & label, IAttributeType * type)
    {
        CComPtr<IAttribute> attr = mod->InsertAttribute(label.c_str(), type);
        CComPtr<IRepository> rep = m_Owner->GetRepository();
        rep->ClearShortTermCache();
        TreeNode::RefreshChildren(mod->GetQualifiedLabel(), m_Root);
        if (attr)
        {
            attr->Checkout();
            std::_tstring ecl = attr->GetType()->GetDefaultText(label);
            attr->SetText(ecl.c_str());
            Select(attr);
            return true;
        }
        else
        {
            T* pT = static_cast<T*>(this);
            CString prog_title;
            prog_title.LoadString(IDR_MAINFRAME);
            pT->MessageBox(_T("WARNING:  Insert File failed, see error window for further details."), prog_title, MB_ICONEXCLAMATION);
        }
        return false;
    }
    void DoRenameAttribute(IAttribute * attr, const std::_tstring & label)
    {
        std::_tstring moduleLabel = attr->GetModuleQualifiedLabel();
        CComPtr<IAttribute> newAttr = attr->Rename(label.c_str());
        if (newAttr)
        {
            CComPtr<IRepository> rep = m_Owner->GetRepository();
            rep->ClearShortTermCache();
            TreeNode::RefreshChildren(moduleLabel, m_Root);
            Select(newAttr);
            m_Owner->UpdateAttribute(attr, newAttr);
        }
        else
        {
            T* pT = static_cast<T*>(this);
            CString prog_title;
            prog_title.LoadString(IDR_MAINFRAME);
            pT->MessageBox(_T("WARNING:  Rename File failed, see error window for further details."), prog_title, MB_ICONEXCLAMATION);
        }
    }
    bool DoMoveAttributeNoRefreshTarget(IAttributeVector & attrs, const std::_tstring & target)
    {
        bool retVal = false;
        CComPtr<IRepository> rep = m_Owner->GetRepository();
        if (rep->Move(attrs, target.c_str()))
        {
            std::map<std::_tstring, bool> modDedup;
            for(IAttributeVector::iterator itr = attrs.begin(); itr != attrs.end(); ++itr)
            {
                modDedup[itr->get()->GetModuleQualifiedLabel()] = true;
                CComPtr<IAttribute> newAttr = rep->GetAttribute(target.c_str(), itr->get()->GetLabel(), itr->get()->GetType());
                m_Owner->UpdateAttribute(itr->get(), newAttr);
            }
            //  No longer needed as attribute nodes now self delete on a  move.
            //for(std::map<std::_tstring, bool>::iterator itr = modDedup.begin(); itr != modDedup.end(); ++itr)
            //	RefreshModule(itr->first);
            retVal = true;
        }
        else
        {
            T* pT = static_cast<T*>(this);
            CString prog_title;
            prog_title.LoadString(IDR_MAINFRAME);
            pT->MessageBox(_T("WARNING:  Move File failed, see error window for further details."), prog_title, MB_ICONEXCLAMATION);
        }
        return retVal;
    }
    void DoMoveAttribute(IAttributeVector & attrs, const std::_tstring & target)
    {
        DoMoveAttributeNoRefreshTarget(attrs, target);
        TreeNode::RefreshChildren(target, m_Root);
    }
    void DoCopyAttribute(IAttributeVector & attrs, const std::_tstring & target)
    {
        T* pT = static_cast<T*>(this);
        CComPtr<IRepository> rep = m_Owner->GetRepository();
        for(IAttributeVector::iterator itr = attrs.begin(); itr != attrs.end(); ++itr)
        {
            CComPtr<IAttribute> newAttr = rep->GetAttribute(target.c_str(), itr->get()->GetLabel(), itr->get()->GetType());
            if (newAttr)
            {
                if (IDYES == pT->MessageBox((boost::_tformat(_T("\"%1%.%2%\" Already exists, overwrite?")) % target.c_str() % itr->get()->GetLabel()).str().c_str(), CString(MAKEINTRESOURCE(IDR_MAINFRAME)), MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION)) 
                    newAttr = rep->GetAttribute(target.c_str(), itr->get()->GetLabel(), itr->get()->GetType());
            }
            else
                newAttr = rep->InsertAttribute(target.c_str(), itr->get()->GetLabel(), itr->get()->GetType());
            if (newAttr)
            {
                newAttr->SetText(itr->get()->GetText());
                newAttr->Checkin((boost::_tformat(_T("Copied from %1%")) % itr->get()->GetModuleQualifiedLabel()).str().c_str());
            }
        }
        TreeNode::RefreshChildren(target, m_Root);
    }
    void DoDeleteAttribute(IAttributeVector & attrs)
    {
        if (attrs.empty())
            return;

        T* pT = static_cast<T*>(this);
        std::_tstring plurral = attrs.size() > 1 ? _T("s") : _T("");
        std::_tstring title = (boost::_tformat(_T("Delete File%1%")) % plurral).str();
        std::_tstring warning = (boost::_tformat(_T("Are you sure you want to delete %1% file%2% permanently?")) % attrs.size() % plurral).str();
        if (pT->MessageBox(warning.c_str(), title.c_str(), MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) == IDYES)
        {
            CComPtr<IRepository> rep = AttachRepository();
            rep->DeleteAttributes(attrs);
        }
    }
    void DoMoveAttributeToTrash(IAttributeVector & attrs)
    {
        if (attrs.empty())
            return;

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
                std::_tstring modLabel = itr->get()->GetModuleQualifiedLabel();
                boost::algorithm::replace_all(modLabel, _T("."), _T("_"));
                std::_tstring attrLabel = itr->get()->GetLabel();
                std::_tstring userID = static_cast<const TCHAR * >(CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_USER)));
                *itr = itr->get()->Rename((boost::_tformat(_T("%1%_%2%_%3%_%4%")) % userID % dateTime % modLabel % attrLabel).str().c_str());
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
            if (!DoMoveAttributeNoRefreshTarget(cleaned_attrs, TRASH))
            {
                CString prog_title;
                prog_title.LoadString(IDR_MAINFRAME);
                pT->MessageBox(_T("WARNING:  Some files could not be moved."), prog_title, MB_ICONEXCLAMATION);
            }
        }
    }
    bool Select(const TCHAR* label, bool bExpand=false)
    {
        return TreeNode::Select(label, m_Root, bExpand);
    }
    bool Select(IModule * mod)
    {
        return TreeNode::Select(mod->GetQualifiedLabel(), m_Root);
    }
    bool Select(IAttribute * attr)
    {
        if (!attr)
            return false;
        return TreeNode::Select(attr->GetQualifiedLabel(), m_Root);
    }
    bool RefreshRepository()
    {
        T* pT = static_cast<T*>(this);
        WTL::CLockWindowUpdate lock(pT->m_hWnd);
        CComPtr<IRepository> rep = m_Owner->GetRepository();
        rep->ClearShortTermCache();
        CComPtr<CLoadingNode> loadingNode = new CLoadingNode(&m_Tree);
        m_Root->SetLoadingNode(loadingNode);
        m_Root->Expand(TVE_COLLAPSE | TVE_COLLAPSERESET);
        m_Root->Expand();
        return true;
    }
};
//  ===========================================================================
class CRepositoryView :
    public CRepositoryViewT<CRepositoryView>,
    public WTL::CDialogImpl<CRepositoryView>,
    public WTL::CDialogResize<CRepositoryView>,
    public WTL::CWinDataExchange<CRepositoryView>
{
    typedef CRepositoryView thisClass;
    typedef WTL::CDialogImpl<CRepositoryView> baseClass;

public:
    enum { IDD = IDD_REPOSITORY };

    CRepositoryView();
    ~CRepositoryView();

    void Init(IRepositorySlot * pOwner);

    BOOL PreTranslateMessage(MSG* pMsg);

    void DoClear();
    void DoReset();
    
    BEGIN_MSG_MAP(thisClass)
        MSG_WM_INITDIALOG(OnInitDialog)
        MSG_WM_CONTEXTMENU(OnContextMenu)

        MESSAGE_HANDLER_EX(CWM_CLEAR, OnClear)
        MESSAGE_HANDLER_EX(CWM_RESET, OnReset)
        MESSAGE_HANDLER_EX(CWM_REFRESH, OnRefresh)
        MESSAGE_HANDLER_EX(RVUM_REFRESHMODULENODE, OnRefreshModuleNode)
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
    LRESULT OnRefreshModuleNode(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
    LRESULT OnInitialize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
    LRESULT OnSelectionUpdated(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
    void OnPaint(HDC hDc); 

    //void OnRefresh(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/);
    void Refresh(IModule * module);

    IAttribute * GetFirstSelectedAttribute();
    IModule * GetFirstSelectedModule();
    HWND GetTreeHandle();
    void OnContextMenu(HWND phWnd, CPoint pPoint);

    CRepositoryNode * GetRootNode() { return m_Root; };

    virtual void OnItemSelectionChanged(CTreeNode *, bool bSelected );
    virtual void OnItemExpanding(CTreeNode *, bool bExpanding);
    virtual void HandleNmClick(LPNMTREEVIEW);
};
//  ===========================================================================
class CVectorView :
    public CRepositoryViewT<CVectorView>,
    public WTL::CDialogImpl<CVectorView>,
    public WTL::CDialogResize<CVectorView>,
    public WTL::CWinDataExchange<CVectorView>
{
    typedef CVectorView thisClass;
    typedef WTL::CDialogImpl<CVectorView> baseClass;

public:
    enum { IDD = IDD_REPOSITORY };

    CVectorView(IRepositorySlot * owner);
    ~CVectorView();

    BOOL PreTranslateMessage(MSG* pMsg);

    BEGIN_MSG_MAP(thisClass)
        MSG_WM_INITDIALOG(OnInitDialog)
        MSG_WM_CONTEXTMENU(OnContextMenu)

        MESSAGE_HANDLER_EX(CWM_CLEAR, OnClear)
        MESSAGE_HANDLER_EX(CWM_RESET, OnReset)

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
    LRESULT OnInitRoot(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
    LRESULT OnSelectionUpdated(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
    void OnPaint(HDC hDc); 

    void OnRefresh(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/);
    virtual void Refresh() = 0;

    IAttribute * GetFirstSelectedAttribute();
    HWND GetTreeHandle();
    void OnContextMenu(HWND phWnd, CPoint pPoint);

    CRepositoryNode * GetRootNode() { return m_Root; };

    virtual void OnItemSelectionChanged(CTreeNode *, bool bSelected );
    virtual void OnItemExpanding(CTreeNode *, bool bExpanding);
    virtual void HandleNmClick(LPNMTREEVIEW);
};
//  ---------------------------------------------------------------------------
class CAttributeVectorView : public CVectorView
{
protected:
    clib::CThreadQueue m_threads;
    IAttributeVector & m_attrs;

public:
    CAttributeVectorView(IRepositorySlot * owner, IAttributeVector & attrs);
    void Refresh();
    void CheckSyntax();
    void DoCopyAll(const std::_tstring & delim = _T(", "));
};
//  ---------------------------------------------------------------------------
class CAttributeHistoryVectorView : public CVectorView
{
protected:
    clib::CThreadQueue m_threads;
    IAttributeHistoryVector & m_attrHistories;

public:
    CAttributeHistoryVectorView(IRepositorySlot * owner, IAttributeHistoryVector & attrs);
    void Refresh();
    void CheckSyntax();
};
//  ---------------------------------------------------------------------------
class CSingleAttributeHistoryVectorView : public CVectorView
{
protected:
    clib::CThreadQueue m_threads;
    IAttributeHistoryVector & m_attrHistories;

public:
    CSingleAttributeHistoryVectorView(IRepositorySlot * owner, IAttributeHistoryVector & attrs);
    void Refresh();
    void CheckSyntax();
};
//  ---------------------------------------------------------------------------
template<typename T>
void CheckSyntaxT(CComPtr<T> node)
{
    ATLASSERT(node);
    node->CheckSyntax();
    if (node->GetTreeView()->IsWindow())
    {
        node->EnsureVisible();
        node->GetTreeView()->Invalidate();
    }
}
//  ===========================================================================
