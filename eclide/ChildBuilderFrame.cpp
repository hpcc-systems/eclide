#include "StdAfx.h"
#include "..\en_us\resource.h"

#include "ChildBuilderFrame.h"
#include "WtlMDIChildFrame.h"
#include "ChildEclFrame.h"
#include "MainFrm.h"
#include "preferencedlg.h"
#include "global.h"
#include "wutil.h"
#include "resultview.h"
#include "eclexec.h"
#include "TabbedChildWindowEx.h"
#include "EclDlgBuilder.h"
#include <EclCC.h>
#include "DiskAttribute.h"

enum UM2
{
    UM2_FIRST = UM_LAST + 1,
    UM2_CLOSETABWINDOW,
    UM2_LAST
};

class CBuilderFrame;
class CChildBuilderFrm;
typedef IWorkspaceItem * WorksaceID;
typedef std::pair<CChildBuilderFrm *, CBuilderFrame *> FramePair;
std::map<WorksaceID, FramePair> g_builder_window;

class CBuilderFrame : 
    public CChildFrame, 
    public IEclBuilderSlot, 
    public CResultSlotImpl,
    public boost::signals::trackable
{
    typedef CBuilderFrame thisClass;
    typedef CChildFrame baseClass;
    friend void SlowRefresh(CBuilderFrame * bf);

public:
    CTabbedChildWindowEx< CColDotNetButtonTabCtrl<CTabViewTabItem> > m_tabbedChildWindow;
    CBuilderDlg m_dlgview;
    typedef std::vector<IResultViewerAdapt> Results;
    Results m_results;
    int		m_contextTab;
    CComPtr<Dali::IWorkunit> m_recentDebugWorkunit;
    CComPtr<Dali::IWorkunit> m_recentWorkunit;
    boost::signals::connection m_attrConnection;

    IResultViewer *FindResult(const TCHAR *tcpszWuid)
    {
        CString wuid(tcpszWuid);
        IResultViewer *pResults = 0;
        for (Results::iterator i = m_results.begin(); i != m_results.end();	++i)
        {
            pResults = i->get();
            CString otherWuid;
            if ( wuid == pResults->GetID(otherWuid) )
            {
                return pResults;
            }
        }
        return 0;
    }

    int FindResultPos(const TCHAR *tcpszWuid)
    {
        CString wuid(tcpszWuid);
        int pResults = 1;
        for (Results::iterator i = m_results.begin(); i != m_results.end();	++i, ++pResults)
        {
            CString otherWuid;
            if (wuid == i->get()->GetID(otherWuid))
            {
                return pResults;
            }
        }
        return pResults;
    }

public: 
    DECLARE_FRAME_WND_CLASS(NULL, IDR_BUILDERWINDOW)
    CBuilderFrame(const AttrInfo & attrInfo, IWorkspaceItem * workspaceItem) : baseClass(attrInfo, workspaceItem), m_dlgview(attrInfo, this)
    {
    }

    ~CBuilderFrame()
    {
    }

    BOOL PreTranslateMessage(MSG* pMsg)
    {
        if ((pMsg->message == WM_KEYUP ) && (pMsg->wParam == VK_MENU))
        {
            int d = 0;
        }

        if (pMsg->message == WM_KEYDOWN && (::GetAsyncKeyState(VK_CONTROL) & 0x8000))  // Let frame handle some messages.
        {
            switch (pMsg->wParam)
            {
            case VK_F4:
                return 0;
                break;
            case VK_F6:
                if (::GetAsyncKeyState(VK_SHIFT) & 0x8000)
                    ::GetIMainFrame()->MDIPrev();
                else
                    ::GetIMainFrame()->MDINext();
                return 1;
                break;
            case VK_TAB:
                return 0;
                break;
            }
        }
        return m_tabbedChildWindow.PreTranslateMessage(pMsg);
    }

    void UpdateAttribute(IAttribute * attr)
    {
        CComPtr<IWorkspaceItem> fromID = m_workspaceItem;
        CComQIPtr<IAttribute> test = attr;
        CComQIPtr<IDiskAttribute> dattr = attr;
        ATLASSERT(dattr);
        if (dattr)
        {
            CComPtr<IWorkspaceItem> toID = m_workspaceItem->GetRepository()->CreateIWorkspaceItem(attr, dattr->GetPath());
            g_builder_window[toID] = g_builder_window[fromID];
            //g_attr_window[toID] = this;
            g_builder_window[fromID] = std::make_pair<CChildBuilderFrm *, CBuilderFrame *>(NULL, NULL);
            m_dlgview.SetNamePath(dattr->GetPath());
            UIUpdateTitle();
            (*this)(attr, false, NULL, false);
        }
    }

    LRESULT OnCreate(LPCREATESTRUCT lParam);
    void OnSize(UINT nType, CSize size)
    {
        CRect rectClient;
        GetClientRect(rectClient);

        m_tabbedChildWindow.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
        if (nType == SIZE_MAXIMIZED)
        {

        }
    }

    void OnSetFocus(HWND /*hWndOther*/)
    {
        //SetMsgHandled(false);
        CWindow activeWnd = m_tabbedChildWindow.GetActiveView();
        if (activeWnd.IsWindow() && activeWnd.IsWindowVisible())
            activeWnd.SetFocus();
    }

    void SetAttribute(IAttribute *attribute)
    {
        m_dlgview.SetAttribute(attribute);
    }

    void SetEcl(const CString & ecl, bool resetSavePoint)
    {
        m_dlgview.SetSource(ecl, resetSavePoint, true);
    }

    void SetSyntax(const CSyntaxErrorVector & errors)
    {
        m_dlgview.SetSyntax(errors);
        GetIMainFrame()->Send_RefreshSyntax(&m_dlgview.GetView(), m_dlgview.GetView().SyntaxGetCurrent());
        PostMessage(WM_COMMAND, ID_ECL_NEXTERROR);
    }

    void SetQueueCluster(const CString & queue, const CString & cluster)
    {
        m_dlgview.SetQueueCluster(queue, cluster);
    }

    void SetSel(int row, int col, int len)
    {
        unsigned int pos = m_dlgview.m_view.PositionFromLine(row - 1) + col - 1;
        m_dlgview.m_view.SetSel(pos, pos + len);
    }

    enum UM
    {
        UM_FIRST = CWM_LAST + 1,
        CWM_ADDWORKUNIT,
        UM_LAST
    };

    void PostSelectRibbon()
    {
        if (IResultViewer *mrv = ActiveResultsWindow())
            mrv->PostSelectRibbon();
        else
            GetIMainFrame()->PostSelectRibbon(RIBBON_HOME);
    }

    BEGIN_MSG_MAP(thisClass)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_SIZE(OnSize)
        MSG_WM_SETFOCUS(OnSetFocus)

        MSG_WM_CONTEXTMENU(OnContextMenu)
        NOTIFY_CODE_HANDLER(CTCN_SELCHANGE, OnTabSelected)
        MESSAGE_HANDLER(UM2_CLOSETABWINDOW, OnCloseTabWindow)

        COMMAND_ID_HANDLER_EX(ID_TAB_REMOVE, OnRemoveActiveTab)
        COMMAND_ID_HANDLER_EX(ID_TAB_REMOVE_ALL, OnRemoveAllTabs)
        COMMAND_ID_HANDLER_EX(ID_ECL_SYNCTOC, OnEclSyncToc)
        COMMAND_ID_HANDLER_EX(ID_HELP, OnEclHelp)
        COMMAND_ID_HANDLER_EX(ID_ECL_GOTO, OnEclGoto)
        COMMAND_ID_HANDLER_EX(ID_ECL_GOTOSYNCTOC, OnEclGotoSyncToc)

        MESSAGE_HANDLER_EX(CWM_SUBMITDONE, OnSubmitDone)
        MESSAGE_HANDLER_EX(CWM_SYNCCLUSTER, OnSyncCluster)
        MESSAGE_HANDLER_EX(CWM_ADDWORKUNIT, OnAddWorkunit)
    
        CHAIN_MSG_MAP(baseClass)
        REFLECT_NOTIFICATIONS()
        CHAIN_COMMANDS_MEMBER(m_tabbedChildWindow)
    END_MSG_MAP()

    LRESULT OnCloseTabWindow(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        Dali::IWorkunit * workunit = (Dali::IWorkunit *)wParam;
        RemoveTab(FindResultPos(workunit->GetWuid()));
        return 0;
    }

    void OnRemoveActiveTab(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
    {
        RemoveActiveTab();
    }

    void OnRemoveAllTabs(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
    {
        if(MessageBox(_T("Are you sure you want to close all workunits"), CString(MAKEINTRESOURCE(IDR_MAINFRAME)), MB_YESNO | MB_DEFBUTTON1 | MB_ICONQUESTION) == IDYES)
            RemoveAllTabs();
    }

    void OnEclSyncToc(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
    {
        unsigned int curTab = m_tabbedChildWindow.GetTabCtrl().GetCurSel();
        if (curTab > 0 && curTab <= m_results.size())
        {
            CString wuid;
            GetIMainFrame()->SyncWorkunitTOC(m_results[curTab - 1].get()->GetID(wuid));
        }
        else if (m_workspaceItem->GetAttribute())
        {
            if (IAttribute * currAttr = m_workspaceItem->GetAttribute())
                GetIMainFrame()->SyncTOC(currAttr);
        }
    }

    void OnEclHelp(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
    {
        CString message;
        m_dlgview.GetWordAtCurPosNoPeriodPlusHash(message);
        if (message[0])
            ShowHelp((const TCHAR *)message);
    }

    void OnEclGoto(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
    {
        CString message;
        m_dlgview.GetWordAtCurPos(message);
        if (message[0])
        {
            if (IAttribute * currAttr = m_workspaceItem->GetAttribute())
                GetIMainFrame()->OpenAttribute(message, currAttr->GetType(), currAttr->GetModuleQualifiedLabel());
            else
                GetIMainFrame()->OpenAttribute(message, CreateIAttributeECLType());
        }
    }

    void OnEclGotoSyncToc(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
    {
        CString message;
        m_dlgview.GetWordAtCurPos(message);
        if (message[0])
        {
            if (IAttribute * currAttr = m_workspaceItem->GetAttribute())
                GetIMainFrame()->SyncTOC(message, CreateIAttributeECLType(), currAttr->GetModuleQualifiedLabel());
            else
                GetIMainFrame()->SyncTOC(message, CreateIAttributeECLType());
        }
    }

    LRESULT	OnSubmitDone(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT	OnSyncCluster(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT	OnAddWorkunit(UINT uMsg, WPARAM wParam, LPARAM lParam);

    virtual void UIUpdateTitle();
    virtual bool UIUpdateMenuItems(CCmdUI * cui);
    virtual void Close();

    //  File Access  ---
    bool DoFileOpen(const CString & sPathName);	
    void operator()(IAttribute * attribute, bool eclChanged, IAttribute * newAttrAsOldOneMoved, bool deleted);

    virtual void SavePersistInfo(CPersistMap & persistInfo);
    virtual void RestorePersistInfo(const CPersistMap & persistInfo);

    LRESULT OnForwardMsg(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
    {
        LPMSG pMsg = (LPMSG)lParam;

        //if(baseClass::PreTranslateMessage(pMsg))
        //	return TRUE;

        return m_tabbedChildWindow.PreTranslateMessage(pMsg);
/*
        LPMSG pMsg = (LPMSG)lParam;

        //ATLTRACE(_T("CBuilderFrame::OnForwardMsg %0x\r\n"), pMsg->message );

        if(__super::PreTranslateMessage(pMsg))
            return TRUE;

        return m_tabbedChildWindow.PreTranslateMessage(pMsg);
*/
    }

    //  EclView Notifications  ---
    void LinesAdded(int lines)
    {
        m_dlgview.AnnotationClearAll();
    }

    void DoCheckAttributeSyntax(const CString &attr, IAttributeType * type, const CSyntaxErrorVector & errors)
    {
        GetIMainFrame()->OpenSyntaxAttribute(attr, type, errors);
    }

    void RefreshSyntax(ISciSyntaxMarker *errors, size_t curErr, bool forceShow)
    {
        GetIMainFrame()->Send_RefreshSyntax(errors, curErr, forceShow);
    }

    bool Drilldown(const CString &ecl)
    {
        ExecEcl(Dali::WUActionRun, ecl, false, false, false, true);
        return true;
    }

    void OnButtonGo(Dali::WUAction action, bool selected)
    {
        CString ecl;
        if (selected)
            m_dlgview.GetSource(ecl, selected);
        else
            m_dlgview.GetSource(ecl);
        ExecEcl(action, ecl, m_dlgview.IsScheduled(), m_dlgview.IsLabeled());
    }

    void OnButtonDebug()
    {
        CString ecl;
        m_dlgview.GetSource(ecl);
        ExecEcl(Dali::WUActionRun, ecl, m_dlgview.IsScheduled(), m_dlgview.IsLabeled(), true);
    }

    void ExecEcl(Dali::WUAction action, const CString &_ecl, bool isScheduled=false, bool isLabeled=false, bool isDebug=false, bool supressPath=false)
    {
        bool isSaved = false;
        if (CComPtr<IEclCC> eclcc = CreateIEclCC())
        {
            GetIMainFrame()->DoFileSaveAll(true);
            if (m_dlgview.IsDirty())
            {
                CString path = m_dlgview.GetPath();
                if (!path.IsEmpty())
                {
                    m_dlgview.DoFileSave(path);
                    isSaved = true;
                }
            }
        }
        m_dlgview.SyntaxClearAll();
        PostStatus(_T("Submitting..."));

        std::_tstring when;
        CString label;
        if (isScheduled)
            m_dlgview.GetSchedule(when);
        if (isLabeled)
            m_dlgview.GetLabel(label);

        StlLinked<IResultViewer> result = ::CreateIResultViewer(this);
        m_results.insert(m_results.begin(), result);
        result->Create(m_tabbedChildWindow);
        std::_tstring debugStr = m_dlgview.GetDebug();
        std::_tstring attrQualifiedLabel;
        bool isDesdl = false;
        if (CComPtr<IAttribute> attr = m_dlgview.GetAttribute()) {
            attrQualifiedLabel = attr->GetQualifiedLabel(true);
            isDesdl = attr->GetType()->IsTypeOf(ATTRIBUTE_TYPE_ESDL);
        }
        m_tabbedChildWindow.AddTab(result->GetHwnd(), isDesdl ? attrQualifiedLabel.c_str() :_T("Submitted"), 0, 1);

        CString ecl = _ecl;
        CComPtr<IAttribute> attr = m_dlgview.GetAttribute();
        Dali::CEclExceptionVector errors;
        MetaInfo metaInfo;
        if (attr != NULL && !attr->GetType()->IsTypeOf(ATTRIBUTE_TYPE_ECL))
        {
            IAttributeVector attrs;
            IAttributeBookkeep attrProcessed;
            PREPROCESS_TYPE processType = PREPROCESS_UNKNOWN;
            switch (action) {
            case Dali::WUActionGenerate:
                if (!isSaved)
                    m_dlgview.DoGenerate();
                processType = PREPROCESS_GENERATE;
                break;
            case Dali::WUActionRun:
                processType = PREPROCESS_SUBMIT;
                break;
            case Dali::WUActionCustom1:
                processType = PREPROCESS_CUSTOM1;
                break;
            case Dali::WUActionCustom2:
                processType = PREPROCESS_CUSTOM2;
                break;
            case Dali::WUActionCustom3:
                processType = PREPROCESS_CUSTOM3;
                break;
            case Dali::WUActionCustom4:
                processType = PREPROCESS_CUSTOM4;
                break;
            }
            attr->PreProcess(processType, _ecl, attrs, attrProcessed, errors, metaInfo);
            if (!attrs.empty())
            {
                ecl = attrs[0]->GetText(false, true);
            }
            supressPath = true;
        }
        if (isDesdl)
        {
            result->PublishESDL(metaInfo[MetaInfoItemDesdlID], metaInfo[MetaInfoItemDesdlVersion]);
            if (!errors.empty())
                m_dlgview.SendMessage(CWM_SUBMITDONE, (WPARAM)Dali::WUActionCheck, (LPARAM)&errors);
        }
        else
        {
            result->ExecEcl(m_dlgview.GetCluster(), m_dlgview.GetQueue(), action, attrQualifiedLabel.c_str(), ecl, supressPath ? _T("") : m_dlgview.GetPath(), when.c_str(), label, m_dlgview.GetResultLimit(), debugStr.c_str(), m_dlgview.IsArchive(), m_dlgview.GetMaxRuntime(), isDebug);
            GetIConfig(QUERYBUILDER_CFG)->Set(GLOBAL_QUEUE, m_dlgview.GetQueue());
            GetIConfig(QUERYBUILDER_CFG)->Set(GLOBAL_CLUSTER, m_dlgview.GetCluster());
        }
        PostStatus(_T(""));
    }

    const TCHAR * GetPath()
    {
        return m_dlgview.GetPath();
    }

    const TCHAR * GetTabTip(std::_tstring & tabTip)
    {
        if (CComPtr<IAttribute> attr = m_dlgview.GetAttribute())
        {
            tabTip = std::_tstring(_T("[")) + attr->GetQualifiedLabel() + _T("] ");
        }
        tabTip += GetPath();
        return tabTip.c_str();
    }

    //  IResultSlot override  ---
    bool WorkunitUpdated(Dali::IWorkunit * workunit, bool bFirst)
    {
        if (bFirst)
        {
            m_recentDebugWorkunit = workunit;
            m_recentWorkunit = workunit;
            m_dlgview.SyntaxClearAll();
            GetIMainFrame()->Post_RefreshWorkunitWindow(workunit);
        }

        if (m_recentWorkunit == workunit && m_recentWorkunit->IsComplete())
        {
            Dali::CEclExceptionVector errors;
            if (m_recentWorkunit->GetExceptions(errors))
            {
                m_dlgview.SendMessage(CWM_SUBMITDONE, (WPARAM)Dali::WUActionCheck, (LPARAM)&errors);
            }
            m_recentWorkunit = NULL;
            m_recentDebugWorkunit = NULL;
        }

        IResultViewerAdapt result = FindResult(workunit->GetWuid());
        if (m_recentDebugWorkunit == workunit && m_recentDebugWorkunit->IsDebugging())
        {
            m_tabbedChildWindow.DisplayTab(result->GetHwnd(), FALSE);
            m_recentDebugWorkunit = NULL;
        }

        if (result.get())
        {
            m_tabbedChildWindow.UpdateTabText(result->GetHwnd(), workunit->GetLabel());
            m_tabbedChildWindow.UpdateTabImage(result->GetHwnd(), GetStateIcon(workunit->GetState()));
            return true;
        }
        return false;
    }

    //  IResultSlot override  ---
    bool WorkunitDeleted(Dali::IWorkunit * workunit)
    {
        PostMessage(UM2_CLOSETABWINDOW, (WPARAM)workunit);
        return true;
    }

    //  IResultSlot override  ---
    void PostStatus(const TCHAR *status, PANE pane = PANE_DEFAULT)
    {
        ::GetIMainFrame()->PostStatus(status, pane);
    }

    void NewSel()
    {
        ::GetIMainFrame()->m_wndRibbonBar.HideKeyTips();
    }

    void SetReadOnly(bool readOnly)
    {
        m_dlgview.SetReadOnly(readOnly);
    }

    void AddWorkUnit(Dali::IWorkunit *workunit, BuilderStartup startWith, bool stayOnBuilder)
    {
        StateIDS wuImgID = GetStateIcon(workunit->GetState());
        IResultViewerAdapt result = FindResult(workunit->GetWuid());
        if ( result.get() )
        {
            m_tabbedChildWindow.UpdateTabImage(result->GetHwnd(), wuImgID);
        }
        else
        {
            StlLinked<IResultViewer> result = ::CreateIResultViewer(this, workunit, startWith);
            m_results.insert(m_results.begin(), result);
            result->Create(m_tabbedChildWindow);
            int tab = m_tabbedChildWindow.AddTab(result->GetHwnd(), workunit->GetLabel(), wuImgID, 1);
            switch(startWith)
            {
            case StartExceptions:
            case StartEclWatch:
            case StartGraph:
            case StartFirstResult:
                if (!stayOnBuilder)
                    m_tabbedChildWindow.GetTabCtrl().SetCurSel(tab);
                break;
            }
        }
        m_dlgview.SetQueueCluster(workunit->GetQueue(), workunit->GetCluster());
        m_dlgview.SetLabel(workunit->GetSnapshot());
        m_dlgview.SetResultLimit(workunit->GetResultLimit());
        m_dlgview.SetMaxRuntime(workunit->GetMaxRuntime());
        m_dlgview.SetSchedule(workunit->GetScheduledDateTime());
        m_dlgview.SetDebug(workunit->GetDebugString());
        //m_dlgview.SetArchive(workunit->GetArchiveCpp());
        m_dlgview.SetNoCommonPrivateAttributes(workunit->GetNoCommonPrivateAttributes());
    }

    void AddWorkUnit(const CString &wuid, BuilderStartup startWith)
    {
        StlLinked<Dali::IDali> server = Dali::AttachDali(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT), _T("Dali"));
        StlLinked<Dali::IWorkunit> workunit = server->GetWorkunit(wuid);
        if (workunit.get())
        {
            AddWorkUnit(workunit.get(), startWith, false);
        }
    }

private:
    bool ECLViewActive()
    {
        return m_tabbedChildWindow.GetTabCtrl().GetCurSel() == 0;
    }

    IResultViewer * ActiveResultsWindow()
    {
        int nTab = m_tabbedChildWindow.GetTabCtrl().GetCurSel();
        if ( nTab <= 0 )
        {
            return 0;
        }
        else
        {
            IResultViewer *pResults = 0;
            for (Results::iterator i = m_results.begin();
                 i != m_results.end() && nTab > 0;
                 ++i,nTab--)
            {
                pResults = i->get();
            }
            return pResults;
        }
    }

    LRESULT OnTabSelected(int /*idCtrl*/, LPNMHDR pnmh, BOOL& bHandled)
    {
        bHandled = true;
        NMCTC2ITEMS *pItems = (NMCTC2ITEMS *)pnmh;
        if ( pItems->iItem1 != pItems->iItem2)
        {
            if ( pItems->iItem2 > 0 )
            {
                IResultViewer *mrv = ActiveResultsWindow();
                if ( mrv )
                {
                    mrv->PrepareForDisplay();
                }
                mrv->PostSelectRibbon();
            }
        }
        PostSelectRibbon();
        return 0;
    }

    void RemoveAllTabs()
    {
        m_contextTab = 0;
        m_tabbedChildWindow.GetTabCtrl().SetCurSel(0);
        while(m_results.size())
        {
            RemoveTab(1);
        }
    }

    bool RemoveTab(int pos)
    {
        if (pos > 0)
        {
            m_tabbedChildWindow.GetTabCtrl().DeleteItem(pos, false, false);
            if (pos == m_contextTab)
            {
                m_tabbedChildWindow.GetTabCtrl().SetCurSel(0);
            }
            //if it is a result tab, remove from list of results
            int r = pos - 1;
            for (Results::iterator i = m_results.begin(); i != m_results.end() && r >= 0; ++i, --r)
            {
                if ( r == 0 )
                {
                    IResultViewerAdapt pResults = *i;
                    if ( pResults.get() )
                    {
                        pResults->DestroyWindow();
                    }
                    m_results.erase(i);
                    return true;
                }
            }
        }
        return false;
    }

    void RemoveActiveTab()
    {
        RemoveTab(m_contextTab);
    }

public:
    void OnContextMenu(HWND phWnd, CPoint pt)
    {
        m_contextTab = m_tabbedChildWindow.GetTabCtrl().GetCurSel();
        if ( m_contextTab <= 0 )
        {
            m_dlgview.OnContextMenu(phWnd,pt);
        }
        else
        {
            BOOL handled;
            MSG msg = { phWnd, WM_CONTEXTMENU, 0, MAKELPARAM(pt.x, pt.y), 0, { 0, 0 } };
            m_tabbedChildWindow.OnForwardMsg( 0,0, (LPARAM)&msg, handled);
        }
    }

    //IResultSlot
    bool OnDoubleClickResult()
    {
        return false;
    }

    bool OnContextResult(const CResultContext &context)
    {
        int id = 0;
        {
            static TCHAR CopyWuid[] = _T("Copy WUID");
            static TCHAR SaveAs[] = _T("Save &As Excel Workbook...");
            static TCHAR SyncTOC[] = _T("Sync Workunit");
            static TCHAR CloseItem[] = _T("Close Workunit");
            static TCHAR CloseAllItems[] = _T("Close All Workunits");
            static TCHAR DeleteItem[] = _T("Delete Workunit");
            static TCHAR LocateInExplorer[] = _T("Locate WU in Explorer");
            //TODO:  UIUpdateMenuItems(NULL);
            WTL::CMenu popup = context.m_menu.GetSubMenu(0);
            GetIMainFrame()->CloneState(popup);
            InsertSeparator(popup);
            InsertMenuItem(popup,ID_COPY_WUID,CopyWuid);
            InsertMenuItem(popup,ID_WORKUNIT_SAVEAS,SaveAs);
            InsertMenuItem(popup,ID_ECL_SYNCTOC, SyncTOC);
            InsertMenuItem(popup,ID_EDIT_LOCATEFILEINEXPLORER, LocateInExplorer);
            InsertSeparator(popup);
            InsertMenuItem(popup,ID_TAB_REMOVE,CloseItem);
            InsertSeparator(popup);
            InsertMenuItem(popup,ID_WORKUNIT_DELETE, DeleteItem);
            if ( m_results.size() > 1 )
            {
                InsertMenuItem(popup,ID_TAB_REMOVE_ALL, CloseAllItems);
            }

            id = popup.TrackPopupMenuEx(TPM_RETURNCMD, context.m_pt.x, context.m_pt.y, m_hWnd, NULL);
            ::DestroyMenu(context.m_menu);
        }
        switch ( id )
        {
        case 0:
            break;
        case ID_EDIT_LOCATEFILEINEXPLORER:
            if (CComPtr<IEclCC> eclcc = CreateIEclCC())
            {
                std::_tstring path = eclcc->GetWorkingFolder();
                if (!path.empty())
                    ::ShellExecute(NULL, _T("open"), _T(""), _T(""), path.c_str(), SW_SHOWNORMAL);
            }
            break;
        case ID_COPY_WUID:	
            {
                CString wuid;
                ::SetClipboard(ActiveResultsWindow()->GetID(wuid));
            }
            break;
        case ID_WORKUNIT_SAVEAS:	
            {
                CString wuid;
                GetIMainFrame()->SaveWorkunitAs(ActiveResultsWindow()->GetID(wuid));	
            }
            break;
        case ID_WORKUNIT_DELETE:	
            {
                if (MessageBox(_T("Are you sure?"), _T("Delete Workunit"), MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) == IDYES)	//Keep in sync with WorkunitFrame.h
                {
                    ActiveResultsWindow()->SetDeleteWorkunit();
                    PostMessage(WM_COMMAND, ID_TAB_REMOVE);
                }
            }
            break;
        default:
            //TODO this is a rather arbitrary limit
            if (id > ID_RESULT_DRILLDOWN_NONE + 100 && id < ID_RESULT_DRILLDOWN_NONE + 200)
            {
                ::PostMessage(context.m_view->GetHwnd(), CWM_DRILLDOWN, id - ID_RESULT_DRILLDOWN_NONE - 100, 0);
            }
            else if (id > ID_RESULT_DRILLDOWN_NONE + 200 && id < ID_RESULT_DRILLDOWN_NONE + 300)  //Should not be ID_RESULT_DRILLDOWNSINGLE_NONE
            {
                ::PostMessage(context.m_view->GetHwnd(), CWM_DRILLDOWNSINGLE, id - ID_RESULT_DRILLDOWN_NONE - 200, 0);  //Should not be ID_RESULT_DRILLDOWNSINGLE_NONE
            }
            else if (id > ID_RESULT_DRILLDOWN_NONE + 300 && id < ID_RESULT_DRILLDOWN_NONE + 400)  //Should not be ID_RESULT_DRILLDOWNSINGLE_NONE
            {
                ::PostMessage(context.m_view->GetHwnd(), CWM_DRILLDOWN2, id - ID_RESULT_DRILLDOWN_NONE - 300, 0);  //Should not be ID_RESULT_DRILLDOWNSINGLE_NONE
            }
            else
            {
                PostMessage(WM_COMMAND, id);
            }
        }
        return true;
    }
};

LRESULT CBuilderFrame::OnCreate(LPCREATESTRUCT lParam)
{
    m_tabbedChildWindow.SetImageWidth(8);
    m_tabbedChildWindow.SetReflectNotifications(true);
    m_tabbedChildWindow.SetTabStyles(CTCS_BOTTOM | CTCS_TOOLTIPS);
    m_tabbedChildWindow.Create(m_hWnd, rcDefault);

    WTL::CImageList state;
    state.CreateFromImage(IDB_REP_MASK,8,0,RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
    state.SetBkColor(GetSysColor(COLOR_WINDOW));
    m_tabbedChildWindow.AddBitmap(IDB_REP_MASK,RGB(255,0,255));

    m_dlgview.Create(m_tabbedChildWindow, rcDefault);

    m_tabbedChildWindow.AddTab(m_dlgview, _T("Builder"));
    m_dlgview.CustomMenu(m_workspaceItem->GetAttrInfo());

    WTL::CIcon icon = LoadIcon(_Module.m_hInstResource, MAKEINTRESOURCE(IDR_MDICHILD));
    GetParent().SendMessage(UM_ICONCHANGED, (WPARAM)(HICON)icon);

    baseClass::OnCreate(lParam);

    if (m_workspaceItem->GetAttribute())
        m_attrConnection = m_workspaceItem->GetAttribute()->on_refresh_connect(boost::ref(*this));

    m_dlgview.SetFocus();

    return 0;
}

LRESULT	CBuilderFrame::OnSubmitDone(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam)
{
    Dali::WUAction action = static_cast<Dali::WUAction>(wParam);
    if (action != Dali::WUActionRun)
    {
        SetMsgHandled(false);
        return 0;
    }

    StlLinked<Dali::IWorkunit> workunit = reinterpret_cast<Dali::IWorkunit *>(lParam);

    AddWorkUnit(workunit.get(), StartNoChange, false);

    return 0;
}

LRESULT	CBuilderFrame::OnSyncCluster(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    m_dlgview.SetQueueCluster(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_CLUSTER), _T("hthor"));
    return 0;
}

LRESULT	CBuilderFrame::OnAddWorkunit(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/)
{
    CComPtr<Dali::IWorkunit> wu = reinterpret_cast<Dali::IWorkunit *>(wParam);
    AddWorkUnit(wu, StartFirstResult, true);
    return 0;
}

void CBuilderFrame::UIUpdateTitle()
{
    CString title;
    m_dlgview.GetTitle(title);
    GetParent().SendMessage(UM_TITLECHANGED, (WPARAM)(const TCHAR *)title);
}

bool CBuilderFrame::UIUpdateMenuItems(CCmdUI * cui)
{
    GetIMainFrame()->DisableAllMenuItems();
    IResultViewer *pResult = ActiveResultsWindow();
    if (pResult)
    {
        if (pResult->UpdateUI(cui))
            return true;
    }
    else
    {
        if (m_dlgview.UIUpdateMenuItems(cui))
            return true;

        bool eclType = true;

        CString name;
        m_dlgview.GetName(name);
        std::_tstring ext = ExtensionWithoutDot(name.GetString());

        if (ext.length()) {
            if (boost::algorithm::iequals(ext, ATTRIBUTE_TYPE_ESDL))
            {
                UPDATEUI(cui, ID_GO_GENERATE, true);
                UPDATEUI(cui, ID_GO_CUSTOM1, true);
                UPDATEUI(cui, ID_GO_CUSTOM2, true);
                UPDATEUI(cui, ID_GO_CUSTOM3, true);
                UPDATEUI(cui, ID_GO_CUSTOM4, true);
                eclType = false;
            }
            else if (!boost::algorithm::iequals(ext, ATTRIBUTE_TYPE_ECL))
            {
                if (m_dlgview.m_goButton.IsWindowEnabled())
                    m_dlgview.m_goButton.EnableWindow(false);
            }
        }

        if (eclType) {
            UPDATEUI(cui, ID_FILE_SAVE_AS, TRUE);
            UPDATEUI(cui, ID_HELP, TRUE);
            UPDATEUI(cui, ID_ECL_GO, m_dlgview.CanExecute());
            UPDATEUI(cui, ID_GO_SUBMIT, m_dlgview.CanExecute());
            UPDATEUI(cui, ID_GO_SUBMITSELECTED, m_dlgview.m_view.IsTextSelected());
            UPDATEUI(cui, ID_GO_COMPILE, m_dlgview.CanExecute());
            UPDATEUI(cui, ID_GO_DEBUG, IsRemoteDaliEnabled());

            if (m_dlgview.CanExecute())
            {
                if (!m_dlgview.m_goButton.IsWindowEnabled())
                    m_dlgview.m_goButton.EnableWindow(true);
                if (!m_dlgview.m_debugButton.IsWindowEnabled())
                    m_dlgview.m_debugButton.EnableWindow(true);
                if (!m_dlgview.m_archiveButton.IsWindowEnabled())
                    m_dlgview.m_archiveButton.EnableWindow(true);
            }
            else
            {
                if (m_dlgview.m_goButton.IsWindowEnabled())
                    m_dlgview.m_goButton.EnableWindow(false);
                if (m_dlgview.m_debugButton.IsWindowEnabled())
                    m_dlgview.m_debugButton.EnableWindow(false);
                if (m_dlgview.m_archiveButton.IsWindowEnabled())
                    m_dlgview.m_archiveButton.EnableWindow(false);
            }
        }

        UPDATEUI(cui, ID_ECL_SYNCTOC, m_dlgview.GetAttribute() != NULL);
    }
    return false;
}

void CBuilderFrame::Close()
{
    GetParent().SendMessage(WM_CLOSE);
}

bool CBuilderFrame::DoFileOpen(const CString & sPathNameX)
{
    return m_dlgview.DoFileOpen(sPathNameX);
}

void CBuilderFrame::operator()(IAttribute *attr, bool eclChanged, IAttribute * newAttrAsOldOneMoved, bool deleted)
{
    WTL::CIcon icon;
    switch(attr->GetState())
    {
    case ATTRSTATE_CHECKEDOUT_SANDBOXED:
        icon = LoadIcon(_Module.m_hInstResource, MAKEINTRESOURCE(IDI_CHECKEDOUT_SANDBOXED));
        break;
    case ATTRSTATE_CHECKEDOUT:
        icon = LoadIcon(_Module.m_hInstResource, MAKEINTRESOURCE(IDI_CHECKEDOUT));
        break;
    case ATTRSTATE_LOCKED:
        icon = LoadIcon(_Module.m_hInstResource, MAKEINTRESOURCE(IDI_LOCKED));
        break;
    case ATTRSTATE_SANDBOXED:
        icon = LoadIcon(_Module.m_hInstResource, MAKEINTRESOURCE(IDI_SANDBOXED));
        break;
    case ATTRSTATE_ORPHANED:
        icon = LoadIcon(_Module.m_hInstResource, MAKEINTRESOURCE(IDI_ORPHANED));
        break;
    default:
        icon = LoadIcon(_Module.m_hInstResource, MAKEINTRESOURCE(IDR_MDICHILD));
        break;
    }
    GetParent().SendMessage(UM_ICONCHANGED, (WPARAM)(HICON)icon);
    if (newAttrAsOldOneMoved)
    {
        UpdateAttribute(newAttrAsOldOneMoved);
    }
}

void CBuilderFrame::SavePersistInfo(CPersistMap & persistInfo)
{
    baseClass::SavePersistInfo(persistInfo);
    switch(m_workspaceItem->GetType())
    {
    case WORKSPACE_ITEM_BUILDER:
        persistInfo.Set(PERSIST_TYPE, PERSISTVAL_BUILDER);
        break;
    case WORKSPACE_ITEM_ATTRIBUTE:
        persistInfo.Set(PERSIST_TYPE, PERSISTVAL_ATTRIBUTE);
        {
            CComPtr<IAttribute> attr = m_workspaceItem->GetAttribute();
            if (attr)
            {
                persistInfo.Set(PERSIST_MODULE, attr->GetModuleQualifiedLabel());
                persistInfo.Set(PERSIST_ATTRIBUTE, attr->GetLabel());
                persistInfo.Set(PERSIST_ATTRIBUTETYPE, attr->GetType()->GetRepositoryCode());
            }
        }
        break;
    default:
        persistInfo.Set(PERSIST_TYPE, PERSISTVAL_BUILDER);
    }
    persistInfo.Set(PERSIST_QUEUE, m_dlgview.GetQueue());
    persistInfo.Set(PERSIST_CLUSTER, m_dlgview.GetCluster());
    persistInfo.Set(PERSIST_LIMITRESULT, boost::lexical_cast<std::_tstring>(m_dlgview.GetResultLimit()));
    persistInfo.Set(PERSIST_DEBUG, m_dlgview.GetDebug());
    if (m_dlgview.IsScheduled())
    {
        std::_tstring utc;
        m_dlgview.GetSchedule(utc);
        persistInfo.Set(PERSIST_SCHEDULE, utc);

    }
    if (m_dlgview.IsLabeled())
    {
        CString label;
        m_dlgview.GetLabel(label);
        persistInfo.Set(PERSIST_REPOSITORY_LABEL, static_cast<const TCHAR *>(label));
    }

    CString wuids;
    for (Results::reverse_iterator i = m_results.rbegin(); i != m_results.rend(); ++i)
    {
        if ( !wuids.IsEmpty() )
            wuids += _T(",");
        CString wuid;
        wuids += i->get()->GetID(wuid);
    }
    persistInfo.Set(PERSIST_WUID, static_cast<const TCHAR *>(wuids));

    m_dlgview.SavePersistInfo(persistInfo);
}

void threadRestoreWorkunit(CBuilderFrame * self, CString wuid, Dali::IWorkunitVector * workunits)
{
    StlLinked<Dali::IDali> server = Dali::AttachDali(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT), _T("Dali"));
    StlLinked<Dali::IWorkunit> workunit = server->GetWorkunitFast(wuid);
    if (workunit.get())
        workunits->push_back(workunit);
}

void threadRestoreWorkunits(CBuilderFrame * self, CString wuids)
{
    clib::CThreadQueue loadWorkunits(0);
    Dali::IWorkunitVector workunits;

    int wuCap = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_WORKUNIT_PERSISTLIMIT);
    StdStringVector wus;
    SortedDeserialize(static_cast<const TCHAR *>(wuids), wus, _T(","));
    for (StdStringVector::const_reverse_iterator ritr = wus.rbegin(); ritr != wus.rend(); ++ritr)
    {
        if (!ritr->empty()) 
        {
            loadWorkunits.Append(__FUNCTION__, boost::bind(&threadRestoreWorkunit, self, ritr->c_str(), &workunits));

            if (--wuCap <= 0)
                break;
        }
    }
    loadWorkunits.SetMaxThreadCount(30);
    loadWorkunits.Join();

    std::sort(workunits.begin(), workunits.end(), Dali::IWorkunitCompare());
    for(Dali::IWorkunitVector::const_iterator itr = workunits.begin(); itr != workunits.end(); ++itr)
        self->PostMessage(CBuilderFrame::CWM_ADDWORKUNIT, (WPARAM)(itr->get()), StartNoChange);
}

void CBuilderFrame::RestorePersistInfo(const CPersistMap & persistInfo)
{
    std::_tstring queue = persistInfo.Get(PERSIST_QUEUE);
    std::_tstring cluster = persistInfo.Get(PERSIST_CLUSTER);
    if (queue.length() && cluster.length())
        m_dlgview.SetQueueCluster(queue.c_str(), cluster.c_str());
    else if (queue.length())
        m_dlgview.SetQueueCluster(queue.c_str(), _T("hthor"));
    else
        m_dlgview.SetQueueCluster(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_QUEUE), GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_CLUSTER));


    if (std::_tstring(persistInfo.Get(PERSIST_SCHEDULE)).length())
        m_dlgview.SetSchedule(persistInfo.Get(PERSIST_SCHEDULE));

    if (std::_tstring(persistInfo.Get(PERSIST_REPOSITORY_LABEL)).length())
        m_dlgview.SetLabel(persistInfo.Get(PERSIST_REPOSITORY_LABEL));

    CString wuids ( persistInfo.Get(PERSIST_WUID) );

    clib::thread run(__FUNCTION__, boost::bind(&threadRestoreWorkunits, this, wuids));

    if (std::_tstring(persistInfo.Get(PERSIST_LIMITRESULT)).length())
    {
        try
        {
            m_dlgview.SetResultLimit(boost::lexical_cast<int>(persistInfo.Get(PERSIST_LIMITRESULT)));
        }
        catch (boost::bad_lexical_cast)
        {
        }
    }

    m_dlgview.SetDebug(persistInfo.Get(PERSIST_DEBUG));

    m_dlgview.RestorePersistInfo(persistInfo);
    UIUpdateTitle();
}
//  ===========================================================================
class CChildBuilderFrm : public CWtlMDIChildFrame<StlLinked<CBuilderFrame> >
{
    typedef CChildBuilderFrm thisClass;
    typedef CWtlMDIChildFrame<StlLinked<CBuilderFrame> > baseClass;

    CComPtr<IWorkspaceItem> m_workspaceItem;
public:
    CChildBuilderFrm(const AttrInfo & attrInfo, IWorkspaceItem * workspaceItem) : m_workspaceItem(workspaceItem)
    {
        m_view = new CBuilderFrame(attrInfo, workspaceItem);
        g_builder_window[workspaceItem].first = this;
        g_builder_window[workspaceItem].second = m_view;
    }
    CChildBuilderFrm(IWorkspaceItem * workspaceItem) : m_workspaceItem(workspaceItem)
    {
        AttrInfo attrInfo;
        CChildBuilderFrm(attrInfo, workspaceItem);
    }
    virtual ~CChildBuilderFrm()
    {
        g_builder_window[m_workspaceItem] = std::make_pair<CChildBuilderFrm *, CBuilderFrame *>(NULL, NULL);
    }

#ifdef _DEBUG
    virtual void AssertValid() const
    {
        CMDIChildWndEx::AssertValid();
    }
    virtual void Dump(CDumpContext& dc) const
    {
        CMDIChildWndEx::Dump(dc);
    }
#endif

// Generated message map functions
protected:

    DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CChildBuilderFrm, baseClass)
END_MESSAGE_MAP()

// CChildBuilderFrm message handlers
//  ===========================================================================
bool RestoreExisting(IWorkspaceItem * workspaceItem, CChildBuilderFrm** pChild)
{
    FramePair win = g_builder_window[workspaceItem];
    if (win.first && win.second && win.second->IsWindow())
    {
        if (win.first->IsIconic())
            win.first->ShowWindow(SW_RESTORE);
        win.first->BringWindowToTop();

        *pChild = win.first;
        return true;
    }
    return false;
}

HWND OpenBuilderMDI(CMainFrame* pFrame, const CString &ecl, IWorkspaceItem * workspaceItem)
{
    CChildBuilderFrm* pChild = NULL;
    if (!RestoreExisting(workspaceItem, &pChild))
    {
        pChild = new CChildBuilderFrm(workspaceItem);
        CreateNewChild(pFrame, pChild, IDR_BUILDERWINDOW, _T("builder.ecl"));
        pChild->m_view->SetEcl(ecl, true);
    }
    return ((CMDIChildWnd *)pChild)->GetSafeHwnd();
}

HWND OpenBuilderMDI(CMainFrame* pFrame, Dali::IWorkunit * src, IWorkspaceItem * workspaceItem, BuilderStartup startWith)
{
    CChildBuilderFrm* pChild = NULL;
    if (!RestoreExisting(workspaceItem, &pChild))
    {
        pChild = new CChildBuilderFrm(workspaceItem);
        CreateNewChild(pFrame, pChild, IDR_BUILDERWINDOW, _T("builder.ecl"));
        pChild->m_view->SetEcl(src->GetEcl(), true);
    }
    pChild->m_view->AddWorkUnit(src, startWith, false);
    return ((CMDIChildWnd *)pChild)->GetSafeHwnd();
}

HWND OpenBuilderMDI(CMainFrame* pFrame, IWorkspaceItem * workspaceItem)
{
    CChildBuilderFrm* pChild = NULL;
    if (!RestoreExisting(workspaceItem, &pChild))
    {
        std::_tstring attributeType = workspaceItem->GetAttributeType();
        IAttribute *attr = workspaceItem->GetAttribute();
        AttrInfo attrInfo;
        attrInfo.Attribute = attr;
        if (!attr && !boost::algorithm::iequals(attributeType, ATTRIBUTE_TYPE_ECL))
        {
            attrInfo.AttributeType = workspaceItem->GetAttributeType();
        }
        else if (attr)
        {
            attrInfo.AttributeType = attr->GetType()->GetRepositoryCode();
        }
        else
        {
            attrInfo.AttributeType = _T("ecl");
        }
        pChild = new CChildBuilderFrm(attrInfo, workspaceItem);
        CreateNewChild(pFrame, pChild, IDR_BUILDERWINDOW, _T("builder.ecl"));
        if (attr)
            pChild->m_view->SetReadOnly(attr->IsLocked());
    }
    return ((CMDIChildWnd *)pChild)->GetSafeHwnd();
}

HWND OpenBuilderMDI(CMainFrame* pFrame, IAttribute *src, IWorkspaceItem * workspaceItem)
{
    CChildBuilderFrm* pChild = NULL;
    if (!RestoreExisting(workspaceItem, &pChild))
    {
        pChild = new CChildBuilderFrm(src->AttributeToInfo(), workspaceItem);
        CreateNewChild(pFrame, pChild, IDR_BUILDERWINDOW, _T("builder.ecl"));
        pChild->m_view->SetAttribute(src);
        pChild->m_view->SetEcl(src->GetText(true, true), true);
    }
    return ((CMDIChildWnd *)pChild)->GetSafeHwnd();
}

bool OpenFileBuilderMDI(CMainFrame* pFrame, IAttribute * src, const CString & filePath, IWorkspaceItem * workspaceItem, bool locked, int row, int col, int len)
{
    CChildBuilderFrm* pChild = NULL;
    if (!RestoreExisting(workspaceItem, &pChild))
    {
        ATLASSERT(!filePath.IsEmpty());
        pChild = new CChildBuilderFrm(src->AttributeToInfo(), workspaceItem);
        CreateNewChild(pFrame, pChild, IDR_BUILDERWINDOW, workspaceItem->GetLabel());
        if (!pChild->m_view->DoFileOpen(filePath))
        {
            pChild->DestroyWindow();
            return false;
        }
        pChild->m_view->SetReadOnly(locked);
    }
    pChild->m_view->SetSel(row, col, len);
    return true;
}

bool OpenFileBuilderMDI(CMainFrame* pFrame, IAttribute * src, const CString & filePath, IWorkspaceItem * workspaceItem, bool locked)
{
    CChildBuilderFrm* pChild = NULL;
    if (!RestoreExisting(workspaceItem, &pChild))
    {
        ATLASSERT(!filePath.IsEmpty());
        pChild = new CChildBuilderFrm(src->AttributeToInfo(), workspaceItem);
        CreateNewChild(pFrame, pChild, IDR_BUILDERWINDOW, workspaceItem->GetLabel());
        if (!pChild->m_view->DoFileOpen(filePath))
        {
            pChild->DestroyWindow();
            return false;
        }
        pChild->m_view->SetReadOnly(locked);
    }
    return true;
}

bool OpenFileBuilderMDI(CMainFrame* pFrame, IAttribute * src, const CString & filePath, IWorkspaceItem * workspaceItem, bool locked, const CSyntaxErrorVector & errors)
{
    CChildBuilderFrm* pChild = NULL;
    if (!RestoreExisting(workspaceItem, &pChild))
    {
        ATLASSERT(!filePath.IsEmpty());
        pChild = new CChildBuilderFrm(src->AttributeToInfo(), workspaceItem);
        CreateNewChild(pFrame, pChild, IDR_BUILDERWINDOW, workspaceItem->GetLabel());
        if (!pChild->m_view->DoFileOpen(filePath))
        {
            pChild->DestroyWindow();
            return false;
        }
        pChild->m_view->SetReadOnly(locked);
    }
    if (errors.size())
    {
        if (pChild && pChild->m_view && pChild->m_view->IsWindow())
            pChild->m_view->SetSyntax(errors);
    }
    return true;
}

bool OpenFileBuilderMDI(CMainFrame* pFrame, IAttribute * src, const CString & filePath, IWorkspaceItem * workspaceItem, bool locked, Dali::IWorkunit *wu)
{
    CChildBuilderFrm* pChild = NULL;
    if (!RestoreExisting(workspaceItem, &pChild))
    {
        ATLASSERT(!filePath.IsEmpty());
        pChild = new CChildBuilderFrm(src->AttributeToInfo(), workspaceItem);
        CreateNewChild(pFrame, pChild, IDR_BUILDERWINDOW, workspaceItem->GetLabel());
        if (!pChild->m_view->DoFileOpen(filePath))
        {
            pChild->DestroyWindow();
            return false;
        }
        pChild->m_view->SetReadOnly(locked);
    }
    if (wu)
    {
        if (pChild && pChild->m_view && pChild->m_view->IsWindow()) {
            pChild->m_view->AddWorkUnit(wu, StartFirstResult, false);
        }
    }
    return true;
}