#pragma once

#define DFU_WINDOWS
#ifdef DFU_WINDOWS

#include "MainFrm.h"
#include "WTLDockablePane.h"
#include "DfuView.h"
#include "EclParser.h"
#include "InsAttrDlg.h"
#include "ChildAttributeFrame.h"
#include "RecursiveMutex.h"

struct DFUContextState
{
    bool CanCopy;
};

template<typename T>
class CDfuFrameT : public CDfuSlotImpl
{
    typedef CDfuFrameT<T> thisClass;
public:
    Dfu::IServer * GetDfu()
    {
        return Dfu::AttachDfu( CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_DFU)), _T("Dfu"));
    }

    void OnDoubleClick(IAttribute &attr)
    {
    }

    void OnContext(CDfuSelections &s, CPoint &pt)
    {
        T * pT = static_cast<T*>(this);
        DFUContextState state;
        CalculateContextMenuState(s,state);

        WTL::CMenu m;
        m.LoadMenu(IDR_POPUP_DFU);
        m.SetMenuDefaultItem(ID_DFU_COPY);
        m.EnableMenuItem(ID_DFU_COPY, state.CanCopy ? MF_ENABLED : MF_GRAYED);
        m.EnableMenuItem(ID_DFU_REFRESH, MF_ENABLED);

        unsigned pick = m.GetSubMenu(0).TrackPopupMenuEx(TPM_RETURNCMD, pt.x, pt.y, *pT);
        m.DestroyMenu();
        switch(pick)
        {
        case ID_DFU_COPY:
            pT->m_view.DoCopy(s.files, _T("\r\n"));
            break;
        case ID_DFU_REFRESH:
            pT->m_view.RefreshDfu();
            break;
        default:
            break;
        }
    }

    void OnInitMenuPopup(HMENU hMenu, UINT pos, BOOL /*isWinMenu*/)
    {
        ATLTRACE(_T("OnInitMenuPopup(pos=%d,isMenu=%u)\n"), pos, ::IsMenu(hMenu));
    }

    void CalculateContextMenuState(CDfuSelections &s, DFUContextState &state)
    {
        #define WRITEACCESS(mod) ((mod->GetAccess() & SecAccess_Write) == SecAccess_Write)
        state.CanCopy = s.files.size() > 0;
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

    void Send_Refresh()
    {
        T * pT = static_cast<T*>(this);
        pT->m_view.SendMessage(CWM_REFRESH);
    }

    void Post_Refresh()
    {
        m_view.PostMessage(CWM_REFRESH);
    }

    void operator()(SectionLabel * label)
    {
        T * pT = static_cast<T*>(this);
        if (GLOBAL_SERVER_DFU.first == *label || GLOBAL_USER.first == *label || GLOBAL_PASSWORD.first == *label)
            pT->m_view.PostMessage(CWM_RESET);
    }
};

class CDfuFrame :  
    public CWtlDockablePane<CDfuView>, 
    public CDfuFrameT<CDfuFrame>
{
    typedef CWtlDockablePane<CDfuView> baseClass;
    typedef CDfuFrame thisClass;

public:
    CDfuFrame();

    afx_msg LRESULT OnSyncToc(WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()
};
//  ===========================================================================
class CDfuFilterFrame :  
    public CDfuFrameT<CDfuFilterFrame>, 
    public CWtlDockablePane<CDfuFilterView>, 
    public boost::signals::trackable
{
    typedef CDfuFilterFrame thisClass;
    typedef CWtlDockablePane<CDfuFilterView> baseClass;

public:
    CDfuFilterFrame();

    DECLARE_MESSAGE_MAP()
};

//  ===========================================================================
#endif  //DFU_WINDOWS