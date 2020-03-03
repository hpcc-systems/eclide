#pragma once

#include "WorkspaceView.h"
#include "MainFrm.h"

#ifdef WORKSPACE_WINDOW
//  ===========================================================================
struct ProjectContextState
{
    bool CanNew;
    bool CanSwitch;
    bool CanRemove;
    bool CanRefresh;
};
//  ===========================================================================
template<typename T>
class CWorkspaceFrameT :	public CWorkspaceSlotImpl, public CRepositorySlotImpl, public CDaliSlotImpl, public boost::signals::trackable
{
    typedef CWorkspaceFrameT<T> thisClass;
public:
    //IProject * GetProject()
    //{
    //	return AttachProject( static_cast<const TCHAR *>(CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_ATTRIBUTE))),
    //							 static_cast<const TCHAR *>(CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_USER))),
    //							 static_cast<const TCHAR *>(CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_PASSWORD))), _T("Project"));
    //}

    void OnDoubleClick(IAttribute &attr)
    {
        GetIMainFrame()->OpenAttribute(&attr);
    }

    void OnContext(CTreeNodeVector &s, CPoint &pt)
    {
        T * pT = static_cast<T*>(this);
        ProjectContextState state;
        CalculateContextMenuState(s,state);

        CTreeNodeVector::const_iterator itr = s.begin();
        if (itr != s.end())
        {
            CTreeNode * node = itr->get();
            if (CComQIPtr<CWorkspaceNode> ws_node = node)
            {
                if (ws_node->GetWorkspace() == pT->m_view.GetCurrentWorkspace())
                {
                    state.CanSwitch = false;
                }
            }
        }

        WTL::CMenu m;
        m.LoadMenu(IDR_POPUP_PROJECT);
        m.SetMenuDefaultItem(ID_PROJECT_SWITCH);
        m.EnableMenuItem(ID_PROJECT_NEW, state.CanNew ? MF_ENABLED : MF_GRAYED);
        m.EnableMenuItem(ID_PROJECT_SWITCH, state.CanSwitch ? MF_ENABLED : MF_GRAYED);
        m.EnableMenuItem(ID_PROJECT_REMOVE, state.CanRemove ? MF_ENABLED : MF_GRAYED);
        m.EnableMenuItem(ID_PROJECT_REFRESH, state.CanRefresh ? MF_ENABLED : MF_GRAYED);

        unsigned pick = m.GetSubMenu(0).TrackPopupMenuEx(TPM_RETURNCMD, pt.x, pt.y, *pT);
        m.DestroyMenu();
        switch(pick)
        {
        case ID_PROJECT_NEW:
            GetIMainFrame()->DoWorkspaceNew();
            break;
        case ID_PROJECT_SWITCH:
            {
                if (itr != s.end())
                {
                    CTreeNode * node = itr->get();
                    if (CComQIPtr<CWorkspaceNode> ws_node = node)
                    {
                        if (ws_node->GetDepth() == 0)
                            GetIMainFrame()->DoWorkspaceSwitch(ws_node->GetWorkspace(), true);
                    }
                }
            }
            break;
        case ID_PROJECT_REMOVE:
            {
                IWorkspaceVector workspacesToRemove;
                if (itr != s.end())
                {
                    CTreeNode * node = itr->get();
                    if (CComQIPtr<CWorkspaceNode> ws_node = node)
                    {
                        if (ws_node->GetDepth() == 0)
                            workspacesToRemove.push_back(ws_node->GetWorkspace());
                    }
                }
                if (pT->MessageBox(_T("Are you sure?"), _T("Delete Workspace"), MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) == IDYES)
                    GetIMainFrame()->DoWorkspaceRemove(workspacesToRemove);
            }
            break;
        case ID_PROJECT_REFRESH:
            pT->m_view.Refresh(false);
            break;
        }
    }

    void RestoreState()
    {
        T * pT = static_cast<T*>(this);
        pT->m_view.m_Tree.PopState();
        pT->m_view.SetCurrentWorkspace(pT->m_view.GetCurrentWorkspace());
    }

    void SetAttributeText(IAttribute *attr, const CString & ecl, bool resetSavePoint, bool clearUndo)
    {
        ::SetAttributeText(attr, static_cast<const TCHAR *>(ecl), resetSavePoint, clearUndo);
    }

    void UpdateAttribute(IAttribute *from, IAttribute *to)
    {
        ::UpdateAttributeMDI(from, to);
    }

    void CalculateContextMenuState(CTreeNodeVector &s, ProjectContextState &state)
    {
        unsigned int WorkspaceNodeCount = 0;
        unsigned int BuilderNodeCount = 0;
        unsigned int AttributeNodeCount = 0;
        unsigned int OtherNodeCount = 0;
        for(CTreeNodeVector::const_iterator itr = s.begin(); itr != s.end(); ++itr)
        {
            if (CComQIPtr<CWorkspaceNode> ws_node = itr->get())
            {
                ++WorkspaceNodeCount;
            }
            else if (CComQIPtr<CBuilderWindowNode> builder_node = itr->get())
            {
                ++BuilderNodeCount;
            }
            else if (CComQIPtr<CAttributeWindowNode> builder_node = itr->get())
            {
                ++AttributeNodeCount;
            }
            else
            {
                ++OtherNodeCount;
            }
        }
        state.CanNew = true;
        state.CanSwitch = WorkspaceNodeCount == 1 && BuilderNodeCount == 0 && AttributeNodeCount == 0 && OtherNodeCount == 0;
        state.CanRemove = WorkspaceNodeCount > 0 && BuilderNodeCount == 0 && AttributeNodeCount == 0 && OtherNodeCount == 0;
        state.CanRefresh = true;
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
    void Send_Reset(IWorkspace * workspace)
    {
        T * pT = static_cast<T*>(this);
        pT->m_view.SendMessage(CWM_RESET, (WPARAM)workspace);
    }
    void Post_Reset(IWorkspace * workspace)
    {
        T * pT = static_cast<T*>(this);
        pT->m_view.PostMessage(CWM_RESET, (WPARAM)workspace);
    }
    void Send_Refresh(bool activeOnly)
    {
        T * pT = static_cast<T*>(this);
        pT->m_view.SendMessage(CWM_REFRESH, (WPARAM)activeOnly);
    }
    void Post_Refresh(bool activeOnly)
    {
        T * pT = static_cast<T*>(this);
        pT->m_view.PostMessage(CWM_REFRESH, (WPARAM)activeOnly);
    }
    void operator()(SectionLabel * label)
    {
        T * pT = static_cast<T*>(this);
        if (GLOBAL_SERVER_ATTRIBUTE.first == *label || GLOBAL_USER.first == *label || GLOBAL_PASSWORD.first == *label)
            pT->m_view.PostMessage(CWM_RESET);
    }
};

//  ===========================================================================
class CWorkspaceFrame :  
    public CWtlDockablePane<CWorkspaceView>,
    public CWorkspaceFrameT<CWorkspaceFrame>
{
    typedef CWtlDockablePane<CWorkspaceView> baseClass;
public:
    CWorkspaceFrame();

    void SetCurrentWorkspace(IWorkspace * workspace);
    IWorkspace * GetCurrentWorkspace();
    void RemoveWorkspace(IWorkspace * workspace);

    afx_msg LRESULT OnSyncToc(WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()
};
//  ===========================================================================
#endif