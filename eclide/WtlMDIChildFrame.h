#pragma once

#include "clib.h"
#include <CustomMessages.h>

enum UM
{
    UM_FIRST = CWM_LAST + 1,
    UM_TITLECHANGED,
    UM_ICONCHANGED,
    UM_LAST
};

template<typename ViewT>
class CWtlMDIChildFrame : public CMDIChildWndEx
{
protected:
    std::map<int, bool> m_viewHandlesCommand;

public:
    ViewT m_view;

    virtual BOOL PreCreateWindow(CREATESTRUCT& cs)
    {
        // TODO: Modify the Window class or styles here by modifying the CREATESTRUCT cs
        if( !CMDIChildWndEx::PreCreateWindow(cs) )
            return FALSE;

        cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
        cs.lpszClass = AfxRegisterWndClass(0);
        return TRUE;
    }

    BOOL PreTranslateMessage(MSG* pMsg)
    {
        switch(pMsg->message)
        {
        case WM_SYSKEYDOWN:	
        case WM_SYSKEYUP:	
            return false;	// Let the ribbon get a chance also!

        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
            if (GetIMainFrame()->GetActivePopup())
                return false;
            break;
        }

        if (CMDIChildWndEx::PreTranslateMessage(pMsg))
            return TRUE;

        return m_view->PreTranslateMessage(pMsg);
    }

    virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
    {
        if (m_view->IsWindow())
        {
            switch(nCode)
            {
            case ID_NEXT_PANE:
            case ID_PREV_PANE:
                break;
            case CN_UPDATE_COMMAND_UI:
                {
                    CCmdUI* cui = static_cast<CCmdUI*>(pExtra);
                    if (m_view->UIUpdateMenuItems(cui))
                    {
                        m_viewHandlesCommand[cui->m_nID] = true;
                        return TRUE;
                    }
                }
                break;
            case CN_COMMAND:
                if (pExtra == NULL)
                {
                    int id = (nID == ID_HELP && GetIMainFrame()->m_helpFlag) ? ID_ECL_HELP : nID;
                    m_view->SendMessage(WM_COMMAND, id);
                    if (m_viewHandlesCommand[nID] == true)
                        return TRUE;
                }
            }
        }

        return CMDIChildWndEx::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
    }

    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnClose();
    afx_msg void OnSetFocus(CWnd* pOldWnd) 
    {
        CMDIChildWndEx::OnSetFocus(pOldWnd);
        m_view->SetFocus();
    }

    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);

    afx_msg LRESULT OnTitleChanged(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnIconChanged(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnCloseAllClearAutoSave(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnGetTooltip(WPARAM wParam, LPARAM lParam);

    afx_msg void OnFileClose();

    DECLARE_MESSAGE_MAP()
};

BEGIN_TEMPLATE_MESSAGE_MAP(CWtlMDIChildFrame, ViewT, CMDIChildWndEx)
    ON_WM_CREATE()
    ON_WM_CLOSE()
    ON_WM_SIZE()
    ON_WM_SETFOCUS()
    ON_WM_MDIACTIVATE()

    ON_MESSAGE(UM_TITLECHANGED, OnTitleChanged)
    ON_MESSAGE(UM_ICONCHANGED, OnIconChanged)
    ON_MESSAGE(CWM_CLOSEALLCLEARAUTOSAVE, OnCloseAllClearAutoSave)
    ON_MESSAGE(CWM_GETTOOLTIPTEXT, OnGetTooltip)

    ON_COMMAND(ID_FILE_CLOSE, &OnFileClose)
END_MESSAGE_MAP()

template<typename ViewT>
int CWtlMDIChildFrame<ViewT>::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CMDIChildWndEx::OnCreate(lpCreateStruct) == -1)
        return -1;
    
    // create a view to occupy the client area of the frame
    CRect rc(0, 0, 100, 100);
    if (!m_view->Create(m_hWnd, rc))
    {
        TRACE0("Failed to create view window\n");
        return -1;
    }
    CMFCTabCtrl* pTabWnd = GetRelatedTabGroup();
    if (pTabWnd)
    {
        pTabWnd->EnableActivateLastActive();
    }
    GetIMainFrame()->DoWorkspaceSave();
    return 0;
}

template<typename ViewT>
void CWtlMDIChildFrame<ViewT>::OnClose()
{
    if (!m_view->m_dlgview.DoQuerySave())
    {
    }
    else
    {
        m_view->RemovePersistance();
        //GetRelatedTabGroup()->m_bActivateLastActiveTab = FALSE;
        CMFCTabCtrl * relatedTabGroup = GetRelatedTabGroup();
        HWND hwnd = GetIMainFrame()->GetMDINext();
        CMDIChildWndEx::OnClose();
        if (hwnd)
        {
            if (relatedTabGroup)
            {
                int nextTab = relatedTabGroup->GetTabFromHwnd(hwnd);
                relatedTabGroup->SetActiveTab(nextTab);
            }
        }
        else
            GetIMainFrame()->PostSelectRibbon(RIBBON_HOME);
        GetIMainFrame()->DoWorkspaceSave();
    }
}

template<typename ViewT>
void CWtlMDIChildFrame<ViewT>::OnSize(UINT nType, int cx, int cy)
{
    CMDIChildWndEx::OnSize(nType, cx, cy);

    if (GetSafeHwnd() == NULL)
    {
        return;
    }

    CRect rectClient;
    GetClientRect(rectClient);

    m_view->SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
}

template<typename ViewT>
afx_msg void CWtlMDIChildFrame<ViewT>::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd)
{
    CMDIChildWndEx::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);
    if (bActivate)
        m_view->PostSelectRibbon();
}

template<typename ViewT>
afx_msg LRESULT CWtlMDIChildFrame<ViewT>::OnTitleChanged(WPARAM wParam, LPARAM lParam)
{
    SetTitle((const TCHAR *)wParam);
    OnUpdateFrameTitle(true);
    return 0;
}

template<typename ViewT>
afx_msg LRESULT CWtlMDIChildFrame<ViewT>::OnIconChanged(WPARAM wParam, LPARAM lParam)
{
    SetIcon((HICON)wParam, false);
    SendMessage(WM_NCPAINT, 0, 0);
    return 0;
}

template<typename ViewT>
afx_msg LRESULT CWtlMDIChildFrame<ViewT>::OnCloseAllClearAutoSave(WPARAM wParam, LPARAM lParam)
{
    m_view->RemovePersistance();
    return 0;
}

template<typename ViewT>
afx_msg LRESULT CWtlMDIChildFrame<ViewT>::OnGetTooltip(WPARAM wParam, LPARAM lParam)
{
    std::_tstring * tooltip = (std::_tstring *)wParam;
    if (tooltip) 
    {
        m_view->GetTabTip(*tooltip);
    }
    return 0;
}

template<typename ViewT>
void CWtlMDIChildFrame<ViewT>::OnFileClose()
{
    // To close the frame, just send a WM_CLOSE, which is the equivalent
    // choosing close from the system menu.
    SendMessage(WM_CLOSE);
}
