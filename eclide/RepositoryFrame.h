#pragma once

#include "MainFrm.h"
#include "WTLDockablePane.h"
#include "RepositoryFilterView.h"
#include "EclParser.h"
#include "InsAttrDlg.h"
#include "ChildAttributeFrame.h"
#include "RecursiveMutex.h"
#include "PreferenceDlg.h"
#include <EclCC.h>
#include <ShellContextMenu.h>  //wlib

struct ContextState
{
    bool CanOpen;
    bool CanOpenBuilder;
    bool CanOpenInExplorer;
    bool CanPrint;
    bool CanCopy;
    bool CanPaste;
    bool CanCheckin;
    bool CanCheckout;
    bool CanCheckSyntax;
    bool CanRollback;
    bool CanHistory;
    bool CanInvoke;
    bool CanInsertModule;
    bool CanRenameModule;
    bool CanDeleteModule;
    bool CanInsertAttribute;
    bool CanRenameAttribute;
    bool CanMoveAttribute;
    bool CanDeleteAttribute;
    bool CanLabel;
    bool CanCombine;
};

template<typename T>
class CRepositoryFrameT :	public CRepositorySlotImpl, 
                            public boost::signals::trackable
{
    typedef CRepositoryFrameT<T> thisClass;
public:
    IRepository * GetRepository()
    {
        return AttachRepository();
    }

    void OnDoubleClick(IAttribute &attr)
    {
        GetIMainFrame()->OpenAttribute(&attr);
    }

    void OnDoubleClick(IAttributeHistory & attrHistory, Dali::CEclExceptionVector * errors) 
    {
        GetIMainFrame()->OpenSyntaxAttribute(attrHistory.GetAttribute(), errors);
    }

    void OnContext(CRepositorySelections &s, CPoint &pt)
    {
        T * pT = static_cast<T*>(this);
        ContextState state;
        CalculateContextMenuState(s,state);

        WTL::CMenu m;
        CShellContextMenu scm(*pT);
        if (IsLocalRepositoryEnabled() == TRI_BOOL_TRUE) {
            m.LoadMenu(IDR_POPUP_REPOSITORY_LOCAL);
            if (!s.diskReps.empty()) 
            {
                scm.InsertTortoiseGitMenuItems(s.diskReps.at(0)->GetPath(), 4, m.GetSubMenu(0));
            }
            else if (!s.mods.empty()) 
            {
                scm.InsertTortoiseGitMenuItems(s.mods.at(0)->GetPath(), 4, m.GetSubMenu(0));
            }
            else if (!s.attrs.empty()) 
            {
                scm.InsertTortoiseGitMenuItems(s.attrs.at(0)->GetPath(), 4, m.GetSubMenu(0));
            }
        }
        else
            m.LoadMenu(IDR_POPUP_REPOSITORY);
        m.SetMenuDefaultItem(ID_REPOSITORY_OPEN);
        m.EnableMenuItem(ID_REPOSITORY_OPEN, state.CanOpen ? MF_ENABLED : MF_GRAYED);
        m.EnableMenuItem(ID_REPOSITORY_OPENBUILDER, state.CanOpenBuilder ? MF_ENABLED : MF_GRAYED);
        m.EnableMenuItem(ID_EDIT_LOCATEFILEINEXPLORER, state.CanOpenInExplorer ? MF_ENABLED : MF_GRAYED);
        m.EnableMenuItem(ID_REPOSITORY_COPY, state.CanCopy ? MF_ENABLED : MF_GRAYED);
        m.EnableMenuItem(ID_REPOSITORY_PASTE, state.CanPaste ? MF_ENABLED : MF_GRAYED);
        m.EnableMenuItem(ID_REPOSITORY_CHECKOUT, state.CanCheckout ? MF_ENABLED : MF_GRAYED);
        m.EnableMenuItem(ID_REPOSITORY_CHECKIN, state.CanCheckin ? MF_ENABLED : MF_GRAYED);
        m.EnableMenuItem(ID_REPOSITORY_CHECKINANDREVERT, state.CanCheckin ? MF_ENABLED : MF_GRAYED);
        m.EnableMenuItem(ID_REPOSITORY_CHECKSYNTAX, state.CanCheckSyntax ? MF_ENABLED : MF_GRAYED);
        m.EnableMenuItem(ID_REPOSITORY_ROLLBACK, state.CanRollback ? MF_ENABLED : MF_GRAYED);
        m.EnableMenuItem(ID_REPOSITORY_HISTORY, state.CanHistory ? MF_ENABLED : MF_GRAYED);
        m.EnableMenuItem(ID_REPOSITORY_INSERTMODULE, state.CanInsertModule ? MF_ENABLED : MF_GRAYED);
        m.EnableMenuItem(ID_REPOSITORY_RENAMEMODULE, state.CanRenameModule ? MF_ENABLED : MF_GRAYED);
        m.EnableMenuItem(ID_REPOSITORY_DELETEMODULE, state.CanDeleteModule ? MF_ENABLED : MF_GRAYED);
        m.EnableMenuItem(ID_REPOSITORY_INSERTATTRIBUTE, state.CanInsertAttribute ? MF_ENABLED : MF_GRAYED);
        m.EnableMenuItem(ID_CHANGETYPELOCAL_DUD, IsLocalRepositoryEnabled() == TRI_BOOL_TRUE && state.CanRenameAttribute ? MF_ENABLED : MF_GRAYED);
        m.EnableMenuItem(ID_CHANGETYPELOCAL_ECL, IsLocalRepositoryEnabled() == TRI_BOOL_TRUE && state.CanRenameAttribute ? MF_ENABLED : MF_GRAYED);
        m.EnableMenuItem(ID_CHANGETYPELOCAL_ESDL, IsLocalRepositoryEnabled() == TRI_BOOL_TRUE && state.CanRenameAttribute ? MF_ENABLED : MF_GRAYED);
        m.EnableMenuItem(ID_CHANGETYPELOCAL_KEL, IsLocalRepositoryEnabled() == TRI_BOOL_TRUE && state.CanRenameAttribute ? MF_ENABLED : MF_GRAYED);
        m.EnableMenuItem(ID_CHANGETYPELOCAL_SALT, IsLocalRepositoryEnabled() == TRI_BOOL_TRUE && state.CanRenameAttribute ? MF_ENABLED : MF_GRAYED);
        m.EnableMenuItem(ID_REPOSITORY_RENAMEATTRIBUTE, state.CanRenameAttribute ? MF_ENABLED : MF_GRAYED);
        m.EnableMenuItem(ID_REPOSITORY_MOVEATTRIBUTE, state.CanMoveAttribute ? MF_ENABLED : MF_GRAYED);
        m.EnableMenuItem(ID_REPOSITORY_COPYATTRIBUTE, state.CanMoveAttribute ? MF_ENABLED : MF_GRAYED);
        m.EnableMenuItem(ID_REPOSITORY_DELETEATTRIBUTE, state.CanDeleteAttribute ? MF_ENABLED : MF_GRAYED);
        m.EnableMenuItem(ID_FILE_CREATEMODFILE, state.CanCombine ? MF_ENABLED : MF_GRAYED);
        m.EnableMenuItem(ID_REPOSITORY_LABEL, state.CanLabel ? MF_ENABLED : MF_GRAYED);
        m.EnableMenuItem(ID_REPOSITORY_REFRESH, MF_ENABLED);

        if(s.attrs.size() == 1)
        {
            CString lockedby = s.attrs.begin()->get()->GetLockedBy();
            if (lockedby.GetLength())
            {
                CString lockedbyStr;
                lockedbyStr.Format(_T("%s by \"%s\""), s.attrs.begin()->get()->GetStateLabel(), lockedby);
                unsigned int nill = 0;
                m.ModifyMenu(ID_REPOSITORY_CHECKOUT, MF_BYCOMMAND | MF_STRING, nill, lockedbyStr); 
                m.EnableMenuItem(ID_REPOSITORY_CHECKOUT, MF_GRAYED);
            }

            CString copyStr;
            copyStr.Format(_T("&Copy \"%s\""), s.attrs.begin()->get()->GetQualifiedLabel(true));
            m.ModifyMenu(ID_REPOSITORY_COPY, MF_BYCOMMAND | MF_STRING, ID_REPOSITORY_COPY, copyStr); 

            CString attrType = s.attrs.begin()->get()->GetType()->GetRepositoryCode();
            if (boost::algorithm::iequals(attrType.GetString(), ATTRIBUTE_TYPE_DUD)) {
                m.EnableMenuItem(ID_CHANGETYPELOCAL_DUD, MF_GRAYED);
            }
            else if (boost::algorithm::iequals(attrType.GetString(), ATTRIBUTE_TYPE_ECL)) {
                m.EnableMenuItem(ID_CHANGETYPELOCAL_ECL,  MF_GRAYED);
            }
            else if (boost::algorithm::iequals(attrType.GetString(), ATTRIBUTE_TYPE_ECM) || boost::algorithm::iequals(attrType.GetString(), ATTRIBUTE_TYPE_ESDL)) {
                m.EnableMenuItem(ID_CHANGETYPELOCAL_ESDL, MF_GRAYED);
            }
            else if (boost::algorithm::iequals(attrType.GetString(), ATTRIBUTE_TYPE_KEL)) {
                m.EnableMenuItem(ID_CHANGETYPELOCAL_KEL, MF_GRAYED);
            }
            else if (boost::algorithm::iequals(attrType.GetString(), ATTRIBUTE_TYPE_SALT)) {
                m.EnableMenuItem(ID_CHANGETYPELOCAL_SALT, MF_GRAYED);
            }
        }

        unsigned pick = m.GetSubMenu(0).TrackPopupMenuEx(TPM_RETURNCMD, pt.x, pt.y, *pT);
        m.DestroyMenu();
        switch(pick)
        {
        case ID_REPOSITORY_OPEN:
            for(IAttributeVector::iterator itr = s.attrs.begin(); itr != s.attrs.end(); ++itr)
            {
                GetIMainFrame()->OpenAttribute(itr->get());
            }
            break;
        case ID_REPOSITORY_OPENBUILDER:
            for(IAttributeVector::iterator itr = s.attrs.begin(); itr != s.attrs.end(); ++itr)
            {
                GetIMainFrame()->OpenBuilder(itr->get());
            }
            break;
        case ID_EDIT_LOCATEFILEINEXPLORER:
            {
                ATLASSERT(s.mods.size() + s.attrs.size() == 1);
                std::_tstring path;
                if (s.mods.size())
                {
                    path = s.mods[0]->GetPath();
                    if (!path.empty())
                        ::ShellExecute(NULL, _T("open"), _T(""), _T(""), path.c_str(), SW_SHOWNORMAL);
                }
                else if (s.attrs.size())
                {
                    path = s.attrs[0]->GetModule()->GetPath();
                    if (!path.empty())
                        ::ShellExecute(NULL, _T("open"), _T(""), _T(""), path.c_str(), SW_SHOWNORMAL);
                }
            }
            break;
        case ID_REPOSITORY_COPY:
            pT->m_view.DoCopy(s.attrs, _T("\r\n"));
            break;
        case ID_REPOSITORY_PASTE:
            {
                std::_tstring attrs;
                GetClipboardText(attrs);
                pT->m_view.DoPaste(attrs);
            }
            break;
        case ID_REPOSITORY_CHECKOUT:
            pT->m_view.DoCheckout(s.attrs);
            break;
        case ID_REPOSITORY_CHECKIN:
        case ID_REPOSITORY_CHECKINANDREVERT:
            {
                CString comment;
                if (s.attrs.size() == 1)
                {
                    if (GetIMainFrame()->ShowComment(s.attrs[0].get(), comment))
                    {
                        WTL::CWaitCursor waitCursor;
                        GetIMainFrame()->SaveAttribute(s.attrs[0].get());
                        pT->m_view.DoCheckin(s.attrs, (const TCHAR *)comment, pick == ID_REPOSITORY_CHECKINANDREVERT);
                    }
                }
                else
                {
                    if (GetIMainFrame()->ShowComment(comment))
                    {
                        WTL::CWaitCursor waitCursor;
                        for(IAttributeVector::iterator itr = s.attrs.begin(); itr != s.attrs.end(); ++itr)
                            GetIMainFrame()->SaveAttribute(itr->get());
                        pT->m_view.DoCheckin(s.attrs, (const TCHAR *)comment, pick == ID_REPOSITORY_CHECKINANDREVERT);
                    }
                }
            }
            break;
        case ID_REPOSITORY_CHECKSYNTAX:
            pT->m_view.DoCheckSyntax(s.attrs);
            break;
        case ID_REPOSITORY_ROLLBACK:
            if (pT->MessageBox(ROLLBACK_MESSAGE, _T("Rollback"), MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDYES)
                pT->m_view.DoRollback(s.attrs);
            break;
        case ID_REPOSITORY_HISTORY:
            ATLASSERT(s.attrs.size() == 1);
            GetIMainFrame()->ShowHistory(s.attrs.begin()->get());
            break;
        case ID_REPOSITORY_INSERTMODULE:
            if (IsLocalRepositoryEnabled())
            {
                ATLASSERT((s.mods.size() + s.plugins.size()) == 1);
                CString label;
                CComPtr<IModule> mod = s.mods.size() ? s.mods[0] : s.plugins[0];
                ATLASSERT(mod);
                if (GetInsertModule(mod, label, true))
                    pT->m_view.DoInsertModule((const TCHAR *)label);
            }
            else
            {
                CString label;
                if (GetInsertModule(NULL, label, true))
                    pT->m_view.DoInsertModule((const TCHAR *)label);
            }
            break;
        case ID_REPOSITORY_RENAMEMODULE:
            ATLASSERT(s.mods.size() == 1);
            {
                CString label = s.mods[0]->GetLabel();
                if (GetRenameModule(s.mods[0]->GetParentModule(), label, true))
                    pT->m_view.DoRenameModule(s.mods[0].get(), (const TCHAR *)label);
            }
            break;
        case ID_REPOSITORY_DELETEMODULE:
            ATLASSERT(s.mods.size() > 0);
            pT->m_view.DoDeleteModule(s.mods);
            break;
        case ID_REPOSITORY_INSERTATTRIBUTE:
            ATLASSERT(s.mods.size() == 1);
            DoInsertAttribute(s.mods[0].get());
            break;
        case ID_REPOSITORY_RENAMEATTRIBUTE:
            ATLASSERT(s.attrs.size() == 1);
            {
                CString label = s.attrs[0]->GetLabel();
                if (GetRenameAttribute(s.attrs[0]->GetModuleQualifiedLabel(), label, s.attrs[0]->GetType(), true))
                    pT->m_view.DoRenameAttribute(s.attrs[0].get(), (const TCHAR *)label);
            }
            break;
        case ID_REPOSITORY_MOVEATTRIBUTE:
            ATLASSERT(s.attrs.size() > 0);
            {
                std::_tstring label;
                if (GetModuleLabel(label))
                    pT->m_view.DoMoveAttribute(s.attrs, label);
            }
            break;
        case ID_REPOSITORY_COPYATTRIBUTE:
            ATLASSERT(s.attrs.size() > 0);
            {
                std::_tstring label;
                if (GetModuleLabel(label))
                {
                    CWaitCursor wait;
                    pT->m_view.DoCopyAttribute(s.attrs, label);
                }
            }
            break;
        case ID_REPOSITORY_DELETEATTRIBUTE:
            pT->m_view.DoDeleteSelectedAttributes();
            break;
        case ID_REPOSITORY_LABEL:
            {
                CString label;
                SetVersionLabel(label, true);
            }
            break;
        case ID_REPOSITORY_REFRESH:
            pT->m_view.RefreshRepository();
            break;
        case ID_REPOSITORY_COLLAPSEALL:
            pT->m_view.CollapseAll();
            break;
        case ID_CHANGETYPELOCAL_DUD:
            pT->m_view.DoChangeAttributeType(s.attrs[0].get(), s.attrs[0]->GetLabel(), ATTRIBUTE_TYPE_DUD);
            break;
        case ID_CHANGETYPELOCAL_ECL:
            pT->m_view.DoChangeAttributeType(s.attrs[0].get(), s.attrs[0]->GetLabel(), ATTRIBUTE_TYPE_ECL);
            break;
        case ID_CHANGETYPELOCAL_ESDL:
            pT->m_view.DoChangeAttributeType(s.attrs[0].get(), s.attrs[0]->GetLabel(), ATTRIBUTE_TYPE_ESDL);
            break;
        case ID_CHANGETYPELOCAL_KEL:
            pT->m_view.DoChangeAttributeType(s.attrs[0].get(), s.attrs[0]->GetLabel(), ATTRIBUTE_TYPE_KEL);
            break;
        case ID_CHANGETYPELOCAL_SALT:
            pT->m_view.DoChangeAttributeType(s.attrs[0].get(), s.attrs[0]->GetLabel(), ATTRIBUTE_TYPE_SALT);
            break;
        case ID_FILE_CREATEMODFILE:
            pT->m_view.DoCreateModFile(s.attrs);
            break;
        default:
            if (pick >= ID_REPOSITORY_INVOKE + 100)
            {
                if (static_cast<bool>(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_DISABLEINVOKE)) == false)
                {
                    CWaitCursor wait;
                    StlLinked<IEclProgram> program = ParseEcl(s.attrs[0]->GetText());
                    pick -= ID_REPOSITORY_INVOKE + 100;
                    if (pick >=0 && pick < program->GetElementCount())
                    {
                        CComQIPtr<IEclInvokeComment> invoke = program->GetElement(pick);
                        if (invoke)
                        {
                            CString ecl = invoke->GetEcl();
                            ecl.Replace(_T("%cluster%"), CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_CLUSTER)));
                            ecl.Replace(_T("%module%"), s.attrs[0]->GetModuleQualifiedLabel(true));
                            ecl.Replace(_T("%attribute%"), s.attrs[0]->GetLabel());
                            GetIMainFrame()->OpenBuilder(ecl); 
                        }
                    }
                }
            }
            else 
            {
                scm.InvokeCommand(pick, pt);
            }
        }
    }

    void SetAttributeText(IAttribute *attr, const CString & ecl, bool resetSavePoint, bool clearUndo)
    {
        ::SetAttributeText(attr, static_cast<const TCHAR *>(ecl), resetSavePoint, clearUndo);
    }

    void UpdateAttribute(IAttribute *from, IAttribute *to)
    {
        ::UpdateAttributeMDI(from, to);
    }

    bool DoInsertAttribute(CComPtr<IModule> module)
    {
        T * pT = static_cast<T*>(this);
        CString label;
        CComPtr<IAttributeType> type;
        if (GetInsertAttribute(module->GetQualifiedLabel(), label, type, true))
        {
            if (pT->m_view.DoInsertAttribute(module, (const TCHAR *)label, type)) {
                GetIMainFrame()->OpenAttribute(label, type, module->GetQualifiedLabel());
                return true;
            }
        }
        return false;
    }

    void CalculateContextMenuState(CRepositorySelections &s, ContextState &state)
    {
        bool RemoteRepository = true;
        if (IsLocalRepositoryEnabled() == TRI_BOOL_TRUE)
            RemoteRepository = false;

        #define WRITEACCESS(mod) ((mod->GetAccess() & SecAccess_Write) == SecAccess_Write)

        state.CanOpen = s.attrs.size() > 0;
        state.CanOpenBuilder = s.attrs.size() > 0;
        state.CanCombine = s.attrs.size() > 1;
        state.CanOpenInExplorer = (s.mods.size() + s.attrs.size() == 1);
        state.CanPrint = false;
        state.CanCopy = s.mods.size() == 0 && s.attrs.size() > 0;
        if (IsLocalRepositoryEnabled() == TRI_BOOL_TRUE)
            state.CanPaste = false;
        else
            state.CanPaste = HasClipboardText();
        state.CanCheckin = false;
        state.CanCheckout = RemoteRepository;
        state.CanCheckSyntax = s.attrs.size() > 0;
        state.CanRollback = false;
        state.CanInvoke = s.attrs.size() == 1;
        state.CanHistory = s.attrs.size() == 1;
        if (IsLocalRepositoryEnabled() == TRI_BOOL_TRUE)
            state.CanInsertModule = (s.mods.size() + s.plugins.size()) == 1 && s.attrs.size() == 0 && WRITEACCESS((s.mods.size() ? s.mods[0] : s.plugins[0]));
        else
            state.CanInsertModule = true;
        state.CanRenameModule = s.mods.size() == 1 && s.attrs.size() == 0 && WRITEACCESS(s.mods[0]);
        state.CanDeleteModule = s.mods.size() > 0 && s.attrs.size() == 0 && WRITEACCESS(s.mods[0]);
        state.CanInsertAttribute = s.mods.size() == 1 && !s.mods[0]->IsPlugin() && WRITEACCESS(s.mods[0]) && s.attrs.size() == 0;
        state.CanRenameAttribute = s.mods.size() == 0 && s.attrs.size() == 1;
        state.CanMoveAttribute = s.mods.size() == 0 && s.attrs.size() > 0;
        state.CanDeleteAttribute = s.mods.size() == 0 && s.attrs.size() > 0;
        state.CanLabel = RemoteRepository;
        for(IAttributeVector::iterator itr = s.attrs.begin(); itr != s.attrs.end(); ++itr)
        {
            CComPtr<IAttribute> n(itr->get());
            switch (n->GetState())
            {
            case ATTRSTATE_CHECKEDOUT:
                state.CanCheckout = false;
                state.CanRollback = true;
                state.CanRenameAttribute = false;
                state.CanMoveAttribute = false;
                break;

            case ATTRSTATE_CHECKEDOUT_SANDBOXED:
                state.CanCheckout = false;
                state.CanCheckin = true;
                state.CanRollback = true;
                state.CanRenameAttribute = false;
                state.CanMoveAttribute = false;
                break;

            case ATTRSTATE_SANDBOXED:
                state.CanCheckin = true;
                state.CanRollback = true;
                state.CanRenameAttribute = false;
                state.CanMoveAttribute = false;
                break;

            case ATTRSTATE_LOCKED:
                state.CanRollback = false;
                state.CanCheckout = false;
                state.CanRenameAttribute = false;
                state.CanMoveAttribute = false;
                state.CanDeleteAttribute = false;
                break;

            case ATTRSTATE_ORPHANED:
                state.CanRollback = true;
                state.CanCheckout = false;
                state.CanRenameAttribute = false;
                state.CanMoveAttribute = false;
                break;

            default:
                break;
            }
        }
    }
    void Send_Clear()
    {
        T * pT = static_cast<T*>(this);
        pT->m_view.SendMessage(CWM_CLEAR);
    }
    void Post_Clear()
    {
        T * pT = static_cast<T*>(this);
        pT->m_view.PostMessage(CWM_CLEAR);
    }
    void Send_Reset()
    {
        T * pT = static_cast<T*>(this);
        pT->m_view.SendMessage(CWM_RESET);
    }
    void Post_Reset()
    {
        T * pT = static_cast<T*>(this);
        pT->m_view.PostMessage(CWM_RESET);
    }
    void Send_Refresh(IModule * module)
    {
        T * pT = static_cast<T*>(this);
        pT->m_view.SendMessage(CWM_REFRESH, (WPARAM)module);
    }
    void Post_Refresh()
    {
        T * pT = static_cast<T*>(this);
        pT->m_view.PostMessage(CWM_REFRESH);
    }
    void operator()(SectionLabel * label)
    {
        T * pT = static_cast<T*>(this);
        if (GLOBAL_SERVER_ATTRIBUTE.first == *label || GLOBAL_USER.first == *label || GLOBAL_PASSWORD.first == *label)
            pT->m_view.PostMessage(CWM_RESET);
    }
};

//  ===========================================================================
class CRepositoryFrame :  
    public CWtlDockablePane<CRepositoryView>,
    public CRepositoryFrameT<CRepositoryFrame>
{
    typedef CWtlDockablePane<CRepositoryView> baseClass;
public:
    CRepositoryFrame();

    afx_msg LRESULT OnSyncToc(WPARAM wParam, LPARAM lParam);
    afx_msg void OnInitMenuPopup(CMenu*, UINT, BOOL);

    DECLARE_MESSAGE_MAP()
};
//  ===========================================================================
class CRepositoryFilterFrame :  
    public CWtlDockablePane<CRepositoryFilterView>,
    public CRepositoryFrameT<CRepositoryFilterFrame>
{
    typedef CWtlDockablePane<CRepositoryFilterView> baseClass;
public:
    CRepositoryFilterFrame();

    DECLARE_MESSAGE_MAP()

    void OnDoubleClick(IAttribute &attr)
    {
        GetIMainFrame()->OpenAttribute(&attr, m_view.GetSearchText(), m_view.GetFindMode());
    }
};
//  ===========================================================================

