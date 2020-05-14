#include "StdAfx.h"
#include "..\en_us\resource.h"

#include "ChildFrame.h"
#include "MainFrm.h"
#include "global.h"

#include <iostream>
#include <time.h>
#include <logger.h>
#include <preferencedlg.h> //wlib

CChildFrame::CChildFrame(const AttrInfo & attrInfo, IWorkspaceItem * workspaceItem)
{
    m_workspaceItem = workspaceItem;
    m_attrInfo = attrInfo;
    AddRef(); // Final Release actually comes from Frame Window.
}

CChildFrame::~CChildFrame()
{
}

void CChildFrame::OnFinalMessage(HWND /*hWnd*/)
{
    Release();
}

void CChildFrame::OnInitMenu(HMENU /*hMenu*/) 
{ 
    UIUpdateMenuItems();
}

LRESULT CChildFrame::OnCreate(LPCREATESTRUCT /*lParam*/)
{
    //SetMsgHandled(false);
    UIUpdateTitle();

    m_workspaceItem->Restore(this);
    SetTimer( TIMER_AUTOSAVE, static_cast<UINT>(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_AUTOSAVEFREQ)) * 1000);

    return 0;
}
void CChildFrame::OnDestroy()
{
    SetMsgHandled(false);
}

void CChildFrame::RemovePersistance()
{
    KillTimer(TIMER_AUTOSAVE);
    //m_autosave.Remove(); //they don't want to persist anymore
}

LRESULT CChildFrame::OnSavePersistInfo(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    IWorkspaceItemVector * wsItems = (IWorkspaceItemVector *)lParam;
    CPersistMap pmap;
    if (m_attrInfo.AttributeType.length())
    {
        pmap.Set(PERSIST_ATTRIBUTETYPE, m_attrInfo.AttributeType);
    }
    SavePersistInfo(pmap);
    if (!boost::filesystem::exists(m_workspaceItem->GetID()))
    {
        m_workspaceItem->SetContent(pmap);
    }
    wsItems->push_back(m_workspaceItem.p);
    return 0;
}

void CChildFrame::OnTimer(UINT_PTR nIDEvent)
{
    //m_autosave.Save();
}

int CChildFrame::GetGroupIndex()
{
    CMDIChildWndEx * mfcChild = DYNAMIC_DOWNCAST(CMDIChildWndEx, CMDIChildWndEx::FromHandle(::GetParent(m_hWnd)));
    unsigned int groupNum = 0;
    const CObList * tabbedGroups = &GetIMainFrame()->GetMDITabGroups();
    for (POSITION pos = tabbedGroups->GetHeadPosition(); pos != NULL;)
    {
        CMFCTabCtrl* tabCtrl = DYNAMIC_DOWNCAST(CMFCTabCtrl, tabbedGroups->GetNext(pos));
        if (tabCtrl == mfcChild->GetRelatedTabGroup())
            return groupNum;
        ++groupNum;
    }
    return -1;
}

int CChildFrame::GetTabIndex()
{
    CMDIChildWndEx * mfcChild = DYNAMIC_DOWNCAST(CMDIChildWndEx, CMDIChildWndEx::FromHandle(::GetParent(m_hWnd)));
    if (CMFCTabCtrl * tabCtrl = mfcChild->GetRelatedTabGroup())
        return tabCtrl->GetTabFromHwnd(mfcChild->GetSafeHwnd());
    return -1;
}

int CChildFrame::GetActiveTab()
{
    CMDIChildWndEx * mfcChild = DYNAMIC_DOWNCAST(CMDIChildWndEx, CMDIChildWndEx::FromHandle(::GetParent(m_hWnd)));
    if (CMFCTabCtrl * tabCtrl = mfcChild->GetRelatedTabGroup())
        return tabCtrl->GetActiveTab();
    return -2;
}

void CChildFrame::SavePersistInfo(CPersistMap & persistInfo)
{
    persistInfo.Set(PERSIST_GROUPINDEX, boost::lexical_cast<std::_tstring>(GetGroupIndex()));
    int tabIndex = GetTabIndex();
    persistInfo.Set(PERSIST_TABINDEX, boost::lexical_cast<std::_tstring>(tabIndex));
    persistInfo.Set(PERSIST_ACTIVE, boost::lexical_cast<std::_tstring>(GetActiveTab() == tabIndex));
}

void CChildFrame::RestorePersistInfo(const CPersistMap & /*persistInfo*/)
{
}

void CChildFrame::UIUpdateMenuItems()
{
}

void CChildFrame::ShowLastError(const CString & sMessage, DWORD dwError)
{
    ATLTRACE( _T("Entering : ShowLastError ( %s, 0x%x )\n" ), sMessage, dwError);
    
    CString sErrorMessage;
    sErrorMessage = sMessage;
    
    // get the error message
    LPTSTR lpMessageBuffer;
    ::FormatMessage ( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                      NULL, dwError,
                      MAKELANGID ( LANG_ENGLISH, SUBLANG_ENGLISH_US ),
                      (LPTSTR)&lpMessageBuffer, 0, NULL);			
    sErrorMessage += lpMessageBuffer;
    ::LocalFree ( lpMessageBuffer ); // FormatMessage() requires this
    
    // display the error message
    CString title;
    title.LoadString(IDR_MAINFRAME);
    ::MessageBox ( NULL, sErrorMessage, title, MB_OK | MB_ICONEXCLAMATION );	
}
//  ===========================================================================
CMDIChildWnd * CreateNewChild(CMainFrame* pFrame, CMDIChildWndEx * pChildFrame, UINT nResource, const std::_tstring & title)
{
    ASSERT_KINDOF(CMDIChildWnd, pChildFrame);

    // load the frame
    CCreateContext context;
    context.m_pCurrentFrame = pFrame;

    pChildFrame->SetHandles(NULL, pFrame->m_hAccelTable);
    if (!pChildFrame->LoadFrame(nResource, WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL, &context))
    {
        TRACE(traceAppMsg, 0, "Couldn't load frame window.\n");
        return NULL;
    }

    pChildFrame->InitialUpdateFrame(NULL, TRUE);
    return pChildFrame;
}
