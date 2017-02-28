#pragma once

#include "MainFrm.h"
#include "DockedFrame.h"
#include "WorkunitView.h"
#include "WTLDockablePane.h"
#include "EclCC.h"

template<typename T>
class CWorkunitFrameT :	public CWtlDockablePane<T>, 
                        public boost::signals::trackable, 
                        public IWorkunitSlot
{
    typedef CWtlDockablePane<T> baseClass;
    typedef CWorkunitFrameT thisClass;

protected:
    boost::signals::connection m_globalConnection;

public:
    CWorkunitFrameT()
    {
        m_view.Init(this);
    }
    void Send_Clear()
    {
        m_globalConnection.disconnect();
        m_view.SendMessage(CWM_CLEAR);
    }
    void Post_Clear()
    {
        m_globalConnection.disconnect();
        m_view.PostMessage(CWM_CLEAR);
    }
    void Send_Reset()
    {
        m_view.SendMessage(CWM_RESET);
        m_globalConnection = GetIConfig(QUERYBUILDER_CFG)->ConnectSlot(boost::ref(*this));
    }
    void Post_Reset()
    {
        m_view.PostMessage(CWM_RESET);
        m_globalConnection = GetIConfig(QUERYBUILDER_CFG)->ConnectSlot(boost::ref(*this));
    }
    void Send_Refresh()
    {
        m_view.SendMessage(CWM_REFRESH);
    }
    void Post_Refresh(Dali::IWorkunit * wu)
    {
        m_view.PostMessage(CWM_REFRESH, 0, wu ? reinterpret_cast<LPARAM>(wu) : 0);
    }
    bool OnDoubleClick(Dali::IWorkunit &wu)
    {
        GetIMainFrame()->OpenBuilder(&wu, StartFirstResult);
        return true;
    }
    bool OnContext(CMenu &/*menu*/, CPoint &pt)
    {
        //SetMsgHandled(true);
        CComPtr<Dali::IWorkunitVectorCom> wus = new Dali::IWorkunitVectorCom();
        if (m_view.GetSelected(*wus) == 0 && m_view.m_loadingItemFound)
            return true;
        WTL::CMenu m;
        bool hasServerWU = false;
        for(Dali::IWorkunitVectorCom::const_iterator itr = wus->begin(); itr != wus->end(); ++itr)
        {
            if (boost::algorithm::istarts_with(itr->get()->GetWuid(), _T("W")))
            {
                hasServerWU = true;
                break;
            }
        }
        if (hasServerWU)
            m.LoadMenu(IDR_POPUP_WORKUNIT);
        else
            m.LoadMenu(IDR_POPUP_WORKUNIT_LOCAL);
        m.SetMenuDefaultItem(ID_WORKUNIT_OPENRESULTS);
        UINT nEnable = wus->size() > 0 ? MF_ENABLED : MF_GRAYED;
        m.EnableMenuItem(ID_WORKUNIT_OPENRESULTS, nEnable);
        m.EnableMenuItem(ID_WORKUNIT_OPENGRAPH, nEnable);
        m.EnableMenuItem(ID_WORKUNIT_BROWSEECLWATCH, nEnable);
        m.EnableMenuItem(ID_WORKUNIT_VIEWEXCEPTIONS, nEnable);
        m.EnableMenuItem(ID_WORKUNIT_OPENECL, nEnable);
        m.EnableMenuItem(ID_EDIT_LOCATEFILEINEXPLORER, nEnable && !hasServerWU);
        m.EnableMenuItem(ID_WORKUNIT_SAVEAS, nEnable);
        m.EnableMenuItem(ID_EDIT_COPY, nEnable);
        m.EnableMenuItem(ID_WORKUNIT_ABORT, nEnable);
        m.EnableMenuItem(ID_WORKUNIT_RESUBMIT, nEnable);
        m.EnableMenuItem(ID_WORKUNIT_DELETE, nEnable);
        m.EnableMenuItem(ID_WORKUNIT_PROTECT, nEnable);
        m.EnableMenuItem(ID_WORKUNIT_UNPROTECT, nEnable);
        m.EnableMenuItem(ID_WORKUNIT_FOLLOWUP, nEnable);
        m.EnableMenuItem(ID_WORKUNIT_UNFOLLOWUP, nEnable);
        m.EnableMenuItem(ID_SETPRIORITY_UP, nEnable);
        m.EnableMenuItem(ID_SETPRIORITY_DOWN, nEnable);
        m.EnableMenuItem(ID_SETPRIORITY_TOP, nEnable);
        m.EnableMenuItem(ID_SETPRIORITY_BOTTOM, nEnable);
        m.EnableMenuItem(ID_SETSTATE_COMPILED, nEnable);
        m.EnableMenuItem(ID_SETSTATE_RUNNING, nEnable);
        m.EnableMenuItem(ID_SETSTATE_COMPLETED, nEnable);
        m.EnableMenuItem(ID_SETSTATE_FAILED, nEnable);
        m.EnableMenuItem(ID_SETSTATE_ARCHIVED, nEnable);
        m.EnableMenuItem(ID_SETSTATE_ABORTING, nEnable);
        m.EnableMenuItem(ID_SETSTATE_ABORTED, nEnable);
        m.EnableMenuItem(ID_SETSTATE_BLOCKED, nEnable);
        m.EnableMenuItem(ID_SETSTATE_SUBMITTED, nEnable);
        m.EnableMenuItem(ID_SETSTATE_SCHEDULED, nEnable);
        m.EnableMenuItem(ID_SETSTATE_COMPILING, nEnable);
        m.EnableMenuItem(ID_SETSTATE_WAITING, nEnable);

        int menu_id = m.GetSubMenu(0).TrackPopupMenuEx(TPM_RETURNCMD, pt.x, pt.y, m_view, NULL);
        switch(menu_id)
        {
        case ID_WORKUNIT_OPENRESULTS:
            OpenWorkunit(*wus,StartFirstResult);
            break;
        case ID_WORKUNIT_OPENGRAPH:
            OpenWorkunit(*wus,StartGraph);
            break;
        case ID_WORKUNIT_BROWSEECLWATCH:
            OpenWorkunit(*wus,StartEclWatch);
            break;
        case ID_WORKUNIT_VIEWEXCEPTIONS:
            OpenWorkunit(*wus,StartExceptions);
            break;
        case ID_WORKUNIT_OPENECL:
            OpenWorkunit(*wus,StartEcl);
            break;
        case ID_EDIT_LOCATEFILEINEXPLORER:
            if (CComPtr<IEclCC> eclcc = CreateIEclCC())
            {
                std::_tstring path = eclcc->GetWorkingFolder();
                if (!path.empty())
                    ::ShellExecute(NULL, _T("open"), _T(""), _T(""), path.c_str(), SW_SHOWNORMAL);
            }
            break;
        case ID_WORKUNIT_SAVEAS:
            SaveAs(wus);
            break;
        case ID_EDIT_COPY:
            m_view.DoCopy(*wus);
            break;
        case ID_WORKUNIT_ABORT:
            m_view.DoAbort(*wus);
            break;
        case ID_WORKUNIT_RESUBMIT:
            m_view.DoResubmit(*wus);
            break;
        case ID_WORKUNIT_DELETE:
            if (MessageBox(_T("Are you sure?"), _T("Delete Workunit"), MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) == IDYES)	//Keep in sync with ChildBduilderFrame.cpp
                m_view.DoDelete(*wus);
            break;
        case ID_WORKUNIT_PROTECT:
            m_view.DoProtect(*wus);
            break;
        case ID_WORKUNIT_UNPROTECT:
            m_view.DoProtect(*wus, false);
            break;
        case ID_WORKUNIT_FOLLOWUP:
            m_view.DoFollowup(*wus);
            break;
        case ID_WORKUNIT_UNFOLLOWUP:
            m_view.DoFollowup(*wus, false);
            break;
        case ID_WORKUNIT_REFRESH:
            m_view.DoRefresh(*wus);
            break;
        case ID_SETPRIORITY_UP:
            m_view.DoSetPriority(*wus, PRIORITY_UP);
            break;
        case ID_SETPRIORITY_DOWN:
            m_view.DoSetPriority(*wus, PRIORITY_DOWN);
            break;
        case ID_SETPRIORITY_TOP:
            m_view.DoSetPriority(*wus, PRIORITY_TOP);
            break;
        case ID_SETPRIORITY_BOTTOM:
            m_view.DoSetPriority(*wus, PRIORITY_BOTTOM);
            break;
        case ID_SETSTATE_COMPILED:
        case ID_SETSTATE_RUNNING:
        case ID_SETSTATE_COMPLETED:
        case ID_SETSTATE_FAILED:
        case ID_SETSTATE_ARCHIVED:
        case ID_SETSTATE_ABORTING:
        case ID_SETSTATE_ABORTED:
        case ID_SETSTATE_BLOCKED:
        case ID_SETSTATE_SUBMITTED:
        case ID_SETSTATE_SCHEDULED:
        case ID_SETSTATE_COMPILING:
        case ID_SETSTATE_WAITING:
            m_view.DoSetState(*wus, static_cast<Dali::WUState>(menu_id - ID_SETSTATE_COMPILED + 1));
            break;
        }

        m.DestroyMenu();

        return true;
    }
    void OpenWorkunit(Dali::IWorkunitVector & wus, BuilderStartup startWith)
    {
        for(unsigned i = 0; i < wus.size(); ++i)
            GetIMainFrame()->OpenBuilder(wus[i], startWith);
    }
    void SaveAs(CComPtr<Dali::IWorkunitVectorCom> wus)
    {
        GetIMainFrame()->SaveWorkunitsAs(wus);	
    }
    void operator()(SectionLabel * /*label*/)
    {
        /*
        if (GLOBAL_SERVER_WORKUNIT.first == *label || 
            (GLOBAL_USER.first == *label && m_view.m_list.m_UserFilter.CompareNoCase(label->second) != 0) ||
            (GLOBAL_CLUSTER.first == *label && m_view.m_list.m_ClusterFilter.CompareNoCase(label->second) != 0))
            m_view.PostMessage(CWM_RESET);
            */
    }
};
//  ===========================================================================
class CWorkunitTreeFrame : public CWorkunitFrameT<CWorkunitTreeView>
{
    typedef CWorkunitFrameT<CWorkunitTreeView> baseClass;

public:
    CWorkunitTreeFrame();
    afx_msg LRESULT OnSyncToc(WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()
};
//  ===========================================================================
class CActiveWorkunitTreeFrame : public CWorkunitFrameT<CActiveWorkunitTreeView>
{
};
//  ===========================================================================
class CFilteredWorkunitTreeFrame : public CWorkunitFrameT<CWorkunitFilterView>
{
};
