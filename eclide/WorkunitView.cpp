#include "stdafx.h"
#include "..\en_us\resource.h"

#include "WorkunitView.h"
#include "global.h"
#include "topology.h"
#include "RecursiveMutex.h"
#include "Logger.h"
#include <wlib.h>
#include <utilDateTime.h> //clib
#include <thread.h> //clib
#include <preferencedlg.h> //wlib

//  ===========================================================================
static const SectionLabelDefault GLOBAL_WORKUNITS_ALLUSERS(SectionLabel(_T("Workunits"), _T("AllUsers")), false);
static const SectionLabelDefault GLOBAL_WORKUNITS_CLUSTER(SectionLabel(_T("Workunits"), _T("Cluster")), ALL_CLUSTERS);

enum WORKUNIT_COLUMNS
{
    WORKUNIT_DATETIME,
    WORKUNIT_LABEL,
    WORKUNIT_OWNER,
    WORKUNIT_CLUSTER,
    WORKUNIT_STATE,
    WORKUNIT_PROTECTED,
    WORKUNIT_WUID
};
//  ===========================================================================
class CWorkunitRow : public CListRow, public boost::signals::trackable
{
protected:
    StlLinked<Dali::IWorkunit> m_workunit;

public:
    CWorkunitRow(WTL::CListViewCtrl &listctrl, Dali::IWorkunit * workunit) : m_workunit(workunit), CListRow(listctrl)
    {
        m_workunit->MonitorState();
        m_workunit->on_refresh_connect(boost::ref(*this));
    }

    ~CWorkunitRow()
    {
    }

    virtual void GetText(unsigned column, CString & result)
    {
        switch (column)
        {
        case WORKUNIT_DATETIME:
            result =  m_workunit->GetDateTime();
            break;
        case WORKUNIT_LABEL:
            result = m_workunit->GetLabel();
            break;
        case WORKUNIT_OWNER:
            result = m_workunit->GetOwner();
            break;
        case WORKUNIT_CLUSTER:
            result = m_workunit->GetCluster();
            break;
        case WORKUNIT_STATE:
            result = m_workunit->GetStateLabel();
            break;
        case WORKUNIT_PROTECTED:
            result = m_workunit->GetProtected() ? _T("P") : _T("");
            break;
        case WORKUNIT_WUID:
            result = m_workunit->GetWuid();
            break;
        default:
            result = m_nullstr;
            break;
        }
    }

    int GetDispImage()
    {
        return m_workunit->GetProtected() ? IID_DOCUMENT_LOCKED : IID_DOCUMENT;
    }

    Dali::IWorkunit * GetWorkunit()
    {
        return m_workunit.get();
    }

    void operator()(Dali::WUEvent evt, Dali::IWorkunit * /*src*/)
    {
        m_listCtrl.PostMessage(CWM_REFRESH, 0, reinterpret_cast<LPARAM>(this));
    }
};
//  ===========================================================================
CWorkunitTreeView::CWorkunitTreeView()
{
    m_comboClusterCtrl = new CComboCluster(true);
}

CWorkunitTreeView::~CWorkunitTreeView()
{
}

void CWorkunitTreeView::Init(IWorkunitSlot *owner)
{
    m_Owner = owner;
}

bool matches(CTreeNode * node, Dali::IWorkunit * wu)
{
    CComQIPtr<CWorkunitNode> wuNode = node;
    if (wuNode && wuNode->GetWorkunit() == wu)
    {
        return true;
    }
    return false;
}

bool CWorkunitTreeView::Locate(Dali::IWorkunit * wu, bool expandIfNeeded)
{
    for(CComPtr<CTreeNode> node = m_Root->GetChildNode(); node != NULL; node = node->GetNextVisibleItem())
    {
        if (matches(node, wu))
        {
            node->Select();
            node->EnsureVisible();
            m_syncToWU = NULL;
            return true;
        }
        else if (expandIfNeeded)
        {
            CComQIPtr<CDateFolderNode> dateNode = node;
            if (dateNode && dateNode->Contains(wu))
            {
                m_syncToWU = wu;
                //This call is now threaded hence the m_syncToWU;
                dateNode->Expand();	
                return false;
            }
        }
    }
    return false;
}

BOOL CWorkunitTreeView::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN) 
    {
        switch(pMsg->wParam)
        {
        case VK_RETURN:
            {
                TVITEM item;
                item.mask = TVIF_HANDLE;
                item.hItem = m_Tree.GetSelectedItem();
                if (m_Tree.GetItem(&item))
                {
                    CComPtr<CTreeNode> tn = (CTreeNode *)item.lParam;
                    tn->OnDblclk(m_Tree.GetSelectedItem());
                }
            }
            break;
        case VK_F5:
            {
                CComPtr<Dali::IWorkunitVectorCom> wus = new Dali::IWorkunitVectorCom();
                GetSelected(*wus);
                DoRefresh(*wus);
            }
            break;
        default:
            break;
        }
    }

    return IsDialogMessage(pMsg);
}

void CWorkunitTreeView::OnDoubleClick(Dali::IWorkunit &wu)
{
    DoDoubleClick(wu);
}

HWND CWorkunitTreeView::GetHWND()
{
    return *this;
}
bool CWorkunitTreeView::ShowClusterInLabel(const TCHAR * /*cluster*/)
{
    return AllClusters();
}
bool CWorkunitTreeView::ShowUserInLabel(const TCHAR * /*user*/)
{
    return m_AllUsers;
}

LRESULT CWorkunitTreeView::OnInitDialog(HWND /*hWnd*/, LPARAM /*lParam*/)
{
    SetMsgHandled(false);
    DlgResize_Init(false,false);

    WTL::CImageList img, state;
    img.CreateFromImage(IDB_REP,16,0,RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
    img.SetBkColor(GetSysColor(COLOR_WINDOW));

    state.CreateFromImage(IDB_REP_MASK,8,0,RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
    state.SetBkColor(GetSysColor(COLOR_WINDOW));

    m_Tree = GetDlgItem(IDC_TREE_WORKUNIT);
    m_Tree.SetImageLists(img, state);

    m_comboClusterCtrl->Attach(GetDlgItem(IDC_COMBO_CLUSTERS));
    m_AllUsers = false;
    m_comboClusterCtrl->m_value = ALL_CLUSTERS;
    DoDataExchange();

    return 0;
}

void CWorkunitTreeView::OnTimer(UINT_PTR nIDEvent)
{
    //Checking for midnight rollover.
    if (m_date != boost::gregorian::day_clock::local_day())
    {
        m_date = boost::gregorian::day_clock::local_day();
        PostMessage(CWM_CLEAR);
        PostMessage(CWM_RESET);
    }
}

LRESULT CWorkunitTreeView::OnBnClickedCheckAllUsers(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    PostMessage(CWM_REFRESH);
    return 0;
}

LRESULT CWorkunitTreeView::OnCbnSelendokComboClusters(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    PostMessage(CWM_REFRESH);
    return 0;
}

LRESULT CWorkunitTreeView::OnClear(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    m_syncToWU = NULL;
    KillTimer(0);
    m_AllUsers = false;
    m_comboClusterCtrl->Clear();
    m_comboClusterCtrl->m_value = ALL_CLUSTERS;
    DoDataExchange();
    WTL::CLockWindowUpdate lock(m_hWnd);
    m_Tree.DeleteAllItems();
    return 0;
}

LRESULT CWorkunitTreeView::OnReset(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    m_syncToWU = NULL;
    m_AllUsers = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_WORKUNITS_ALLUSERS);
    m_comboClusterCtrl->Load();
    m_comboClusterCtrl->SelectDefault(CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_WORKUNITS_CLUSTER)));
    DoDataExchange();
    WTL::CLockWindowUpdate lock(m_hWnd);
    m_Tree.DeleteAllItems();
    //this is a "virtual" node
    m_Root = new CDaliNode(this, m_AllUsers, m_comboClusterCtrl->m_value == ALL_CLUSTERS ? _T("") : m_comboClusterCtrl->m_value, true);
    m_Root->AddItems(m_Tree);
    m_date = boost::gregorian::day_clock::local_day();
    SetTimer(0, 60000);		//Every Minute;
    return 0;
}

//IDaliSlot
void CWorkunitTreeView::UpdateWorkunits(HTREEITEM hItem, Dali::IWorkunitVectorAdapt wus)
{
    _DBGLOG(LEVEL_INFO, (boost::_tformat(_T("All Users: passed:%1% button:%2%")) % CString(allUsers == 1 ? _T("true") : _T("false")) % CString(m_AllUsers ? _T("true") : _T("false"))).str().c_str());
    if (allUsers == -1 || allUsers == (int)m_AllUsers) {
        PostMessage(CWM_NODEUPDATECOMPLETE, WPARAM(hItem), LPARAM(wus.Detach()));
    }
}


LRESULT CWorkunitTreeView::OnNodeUpdate(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam)
{
    Dali::IWorkunitVectorAdapt wus;
    wus.Attach((Dali::IWorkunitVectorCom*)lParam);
    Update(HTREEITEM(wParam),wus);
    Locate(m_syncToWU, false);
    m_syncToWU = NULL;
    return 0;
}

void CWorkunitTreeView::Update(HTREEITEM hItem, Dali::IWorkunitVectorCom* wus)
{
    WTL::CLockWindowUpdate lock(m_hWnd);

    WTL::CTreeItem node( hItem, &m_Tree );
    if ( wus->empty() && hItem != TVI_ROOT )
    {
        //by doing this, it will look normal, but won't get filled on a refresh
        //since we only refresh nodes that are expanded
        node.Expand(TVE_COLLAPSE|TVE_COLLAPSERESET);
    }
    else
    {
        //delete all the workunits that are under this parent node
        for(WTL::CTreeItem curr = node.GetChild(); !curr.IsNull(); )
        {
            CTreeNode *node = reinterpret_cast<CTreeNode *>(curr.GetData());
            CComQIPtr<CWorkunitNode> wuNode = node;
            if(wuNode)
            {
                curr = curr.GetNextSibling();
                wuNode->Delete();
            }
            else
                break;
        }

        CWaitCursor cursor;
        for (Dali::IWorkunitVector::reverse_iterator itr = wus->rbegin(); itr != wus->rend(); ++itr)
        {
            CWorkunitNode * newNode = new CWorkunitNode(this, *itr);
            //ATLTRACE(_T("Add to tree(%x) %s\r\n"), hItem, newNode->GetID());
            newNode->Insert(m_Tree, hItem, TVI_FIRST);
            newNode->operator ()(Dali::WUEventUpdated, NULL);
        }
    }
}


LRESULT CWorkunitTreeView::OnRefresh(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam)
{
    CComPtr<Dali::IWorkunit> wu = reinterpret_cast<Dali::IWorkunit *>(lParam);
    Refresh(wu);
    return 0;
}

void CWorkunitTreeView::Refresh(Dali::IWorkunit * wu)
{
    if (wu)
    {
        if (!Locate(wu, false))
        {
            CString user = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_USER);
            ATLTRACE(_T("CWorkunitTreeView::Refresh(%s) %s == %s && %s == %s\n"), wu->GetWuid(), static_cast<const TCHAR *>(m_comboClusterCtrl->m_value), wu->GetCluster(), user, wu->GetOwner());
            if ((m_comboClusterCtrl->m_value.Compare(ALL_CLUSTERS) == 0 || m_comboClusterCtrl->m_value.Compare(wu->GetCluster()) == 0) && (m_AllUsers == true || user.Compare(wu->GetOwner()) == 0))
            {
                m_Root->AddItem(m_Tree, wu);
                m_Tree.Invalidate();
            }
        }
    }
    else
    {
        Refresh();
    }
}

void CWorkunitTreeView::Refresh()
{
    DoDataExchange(true);
    GetIConfig(QUERYBUILDER_CFG)->Set(GLOBAL_WORKUNITS_ALLUSERS, m_AllUsers);
    GetIConfig(QUERYBUILDER_CFG)->Set(GLOBAL_WORKUNITS_CLUSTER, std::_tstring(m_comboClusterCtrl->m_value));
    WTL::CLockWindowUpdate lock(m_hWnd);
    //CPersistTreeState state(m_Tree); //doesn't work with threaded model
    m_Root->Refresh(m_AllUsers, AllClusters() ? _T("") : m_comboClusterCtrl->m_value);
}

bool CWorkunitTreeView::AllClusters() const
{
    return m_comboClusterCtrl->m_value == ALL_CLUSTERS;
}

//  ===========================================================================
CActiveWorkunitTreeView::CActiveWorkunitTreeView() 
{
    m_ActiveRefreshCount = 0;
    m_active = false;
}

CActiveWorkunitTreeView::~CActiveWorkunitTreeView()
{
}

void CActiveWorkunitTreeView::Init(IWorkunitSlot *owner)
{
    m_Owner = owner;
}

BOOL CActiveWorkunitTreeView::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN) 
    {
        switch(pMsg->wParam)
        {
        case VK_RETURN:
            {
                TVITEM item;
                item.mask = TVIF_HANDLE;
                item.hItem = m_Tree.GetSelectedItem();
                if (m_Tree.GetItem(&item))
                {
                    CComPtr<CTreeNode> tn = (CTreeNode *)item.lParam;
                    tn->OnDblclk(m_Tree.GetSelectedItem());
                }
            }
            break;
        case VK_F5:
            {
                CComPtr<Dali::IWorkunitVectorCom> wus = new Dali::IWorkunitVectorCom();
                GetSelected(*wus);
                DoRefresh(*wus);
            }
            break;
        default:
            break;
        }
    }

    return IsDialogMessage(pMsg);
}

void CActiveWorkunitTreeView::OnDoubleClick(Dali::IWorkunit &wu)
{
    DoDoubleClick(wu);
}
HWND CActiveWorkunitTreeView::GetHWND()
{
    return *this;
}
bool CActiveWorkunitTreeView::ShowClusterInLabel(const TCHAR * /*cluster*/)
{
    return true;
}
bool CActiveWorkunitTreeView::ShowUserInLabel(const TCHAR * /*user*/)
{
    return true;
}

LRESULT CActiveWorkunitTreeView::OnInitDialog(HWND /*hWnd*/, LPARAM /*lParam*/)
{
    SetMsgHandled(false);
    DlgResize_Init(false,false);

    WTL::CImageList img, state;
    img.CreateFromImage(IDB_REP,16,0,RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
    img.SetBkColor(GetSysColor(COLOR_WINDOW));

    state.CreateFromImage(IDB_REP_MASK,8,0,RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
    state.SetBkColor(GetSysColor(COLOR_WINDOW));

    m_Tree = GetDlgItem(IDC_TREE_WORKUNIT);
    m_Tree.SetImageLists(img, state);

    DoDataExchange();

    return 0;
}

void CActiveWorkunitTreeView::OnTimer(UINT_PTR nIDEvent)
{
    Refresh();
}

LRESULT CActiveWorkunitTreeView::OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
    bHandled = FALSE;
    //post a fake timer message to get an immediate refresh
    PostMessage(WM_TIMER, 0 );
    return 0;
}
    
LRESULT CActiveWorkunitTreeView::OnBnClickedCheckAllUsers(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    PostMessage(CWM_REFRESH);
    return 0;
}

LRESULT CActiveWorkunitTreeView::OnCbnSelendokComboClusters(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    PostMessage(CWM_REFRESH);
    return 0;
}

LRESULT CActiveWorkunitTreeView::OnClear(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    ATLTRACE("CActiveWorkunitTreeView::OnClear\r\n");
    DoDataExchange();
    KillTimer(0);
    m_active = false;
    WTL::CLockWindowUpdate lock(m_hWnd);
    m_Tree.DeleteAllItems();
    return 0;
}

LRESULT CActiveWorkunitTreeView::OnReset(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    ATLTRACE("CActiveWorkunitTreeView::OnReset\r\n");
    DoDataExchange();
    //this used to be saved in milliseconds, so keep that if it was, else treat it
    //as seconds
    int freq = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_ACTIVEWORKUNIT_REFRESH);
    if ( freq < 1000 )
        freq *= 1000;
    m_active = true;
    SetTimer(0, freq);
    Refresh();
    return 0;
}

LRESULT CActiveWorkunitTreeView::OnRefresh(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    Refresh();
    return 0;
}

LRESULT CActiveWorkunitTreeView::OnSubmitDone(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/)
{
    StlLinked<SMC::IClusterVectorCom> clusters = (SMC::IClusterVectorCom *)wParam;
    Refresh(clusters.get());
    return 0;
}

void CActiveWorkunitTreeView::Refresh()
{
    if (IsWindowVisible())
    {
        ATLTRACE("CActiveWorkunitTreeView::Refresh\r\n");
        if ( m_ActiveRefreshCount == 0 && m_active)
        {
            clib::recursive_mutex::scoped_lock lock(m_mutex);
            clib::thread run(__FUNCTION__, boost::bind(&DoItemExpanding, this, &m_Tree));
        }
    }
}

void CActiveWorkunitTreeView::Refresh(SMC::IClusterVectorCom *clusters)
{
    if (m_active)
    {
        DoDataExchange(true);
        m_Tree.PushState();
        WTL::CLockWindowUpdate lock(m_hWnd);
        m_Tree.DeleteAllItems();
        for(SMC::IClusterVector::iterator itr = clusters->begin(); itr != clusters->end(); ++itr)
        {
            CSMCClusterNode *newNode = new CSMCClusterNode(this, itr->get());
            ATLASSERT(newNode);
            newNode->InsertBelow(m_Tree, TVI_ROOT);
            newNode->Expand();
        }
        m_Tree.PopState();
    }
}

void CActiveWorkunitTreeView::DoItemExpanding(CActiveWorkunitTreeView *owner, WTL::CTreeViewCtrlEx * /*tv*/)
{
    /* simple way to prevent multiple calls to this from the timer, 
    although it would not be a big deal if multiple call occured once
    in a while, but would be bad if they started to	queue up excessively */
    owner->m_ActiveRefreshCount++;
    ATLTRACE(_T("Active Workunit Pool=%d\n"), owner->m_ActiveRefreshCount);

    CComPtr<SMC::ISMC> smc = SMC::AttachSMC(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_SMC), _T("SMC"));
    SMC::IClusterVectorCom * clusters = new SMC::IClusterVectorCom;	//This is released in the submit done.
    smc->GetActive(clusters);
    ::PostMessage(owner->GetHWND(), CWM_SUBMITDONE, (WPARAM)(clusters), 0);

    owner->m_ActiveRefreshCount--;
    ATLTRACE(_T("Active Workunit Pool=%d\n"), owner->m_ActiveRefreshCount);
}

//  ===========================================================================
CWorkunitFilterView::CWorkunitFilterView()
{
    m_comboClusterCtrl = new CComboCluster(true);
    m_comboStateCtrl = new CComboState();
}

CWorkunitFilterView::~CWorkunitFilterView()
{
}

void CWorkunitFilterView::Init(IWorkunitSlot *owner)
{
    m_Owner = owner;
}

BOOL CWorkunitFilterView::PreTranslateMessage(MSG* pMsg)
{
    BOOL retVal = IsDialogMessage(pMsg);
    return retVal;
}

LRESULT CWorkunitFilterView::OnInitDialog(HWND /*hWnd*/, LPARAM /*lParam*/)
{
    SetMsgHandled(false);
    DlgResize_Init(false,false);

    m_editUserCtrl = GetDlgItem(IDC_EDIT_USER);
    m_comboClusterCtrl->Attach( GetDlgItem(IDC_COMBO_CLUSTER) );
    m_comboStateCtrl->Attach( GetDlgItem(IDC_COMBO_STATE) );

    m_dateFromCtrl = GetDlgItem(IDC_DATETIMEPICKER_DATEFROM);
    m_timeFromCtrl = GetDlgItem(IDC_DATETIMEPICKER_TIMEFROM);
    m_timeFromCtrl.ModifyStyle(0, DTS_TIMEFORMAT);
    m_dateToCtrl = GetDlgItem(IDC_DATETIMEPICKER_DATETO);
    m_timeToCtrl = GetDlgItem(IDC_DATETIMEPICKER_TIMETO);
    m_timeToCtrl.ModifyStyle(0, DTS_TIMEFORMAT);

    m_spinMaxCount = GetDlgItem(IDC_SPIN_MAXCOUNT);
    m_spinMaxCount.SetRange(1, 999999);

    m_results = GetDlgItem(IDC_TREE_WORKUNIT);

    WTL::CImageList img, state;
    img.CreateFromImage(IDB_REP,16,0,RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
    img.SetBkColor(GetSysColor(COLOR_WINDOW));

    state.CreateFromImage(IDB_REP_MASK,8,0,RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
    state.SetBkColor(GetSysColor(COLOR_WINDOW));

    m_search = GetDlgItem(IDC_BUTTON_SEARCH);

    m_Tree = GetDlgItem(IDC_TREE_WORKUNIT);
    m_Tree.SetImageLists(img, state);

    return 0;
}

LRESULT CWorkunitFilterView::OnClear(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    m_checkUser = GLOBAL_WORKUNITFIND_CHECKUSER.second;
    m_user = GLOBAL_WORKUNITFIND_USER.second;
    m_checkCluster = GLOBAL_WORKUNITFIND_CHECKCLUSTER.second;
    m_comboClusterCtrl->m_value = GLOBAL_WORKUNITFIND_CLUSTER.second;
    m_checkState = GLOBAL_WORKUNITFIND_CHECKSTATE.second;
    m_comboStateCtrl->m_value = GLOBAL_WORKUNITFIND_STATE.second;
    m_checkDateTime = GLOBAL_WORKUNITFIND_CHECKDATETIME.second;
    m_dateTimeFrom = CString(GLOBAL_WORKUNITFIND_DATETIMEFROM.second);
    m_dateTimeTo = CString(GLOBAL_WORKUNITFIND_DATETIMETO.second);
    m_spinMaxCount.SetPos(GLOBAL_WORKUNITFIND_MAXCOUNT.second);
    m_comboClusterCtrl->Clear();
    m_comboStateCtrl->Clear();
    DoDataExchange();
    DoRefreshState();
    WTL::CLockWindowUpdate lock(m_hWnd);
    m_Tree.DeleteAllItems();
    return 0;
}

LRESULT CWorkunitFilterView::OnReset(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    DoRestoreState();
    return 0;
}

LRESULT CWorkunitFilterView::OnSubmitDone(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/)
{
    ATLASSERT(wParam);
    StlLinked<Dali::IWorkunitVectorCom> results = reinterpret_cast<Dali::IWorkunitVectorCom *>(wParam);
    WTL::CLockWindowUpdate lock(m_hWnd);
    m_Tree.DeleteAllItems();
    for(Dali::IWorkunitVector::iterator itr = results->begin(); itr != results->end(); ++itr)
    {
        CWorkunitNode *newNode = new CWorkunitNode(this, *itr); 
        newNode->InsertBelow(m_Tree, TVI_ROOT);
        newNode->operator ()(Dali::WUEventUpdated, *itr);
    }
    m_search.EnableWindow(true);
    return 0;
}
LRESULT CWorkunitFilterView::OnRefreshState(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/)
{
    DoDataExchange(true);
    DoRefreshState();
    return 0;
}

LRESULT CWorkunitFilterView::OnBnClickedCheckUser(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    PostMessage(WFUM_REFRESHSTATE);
    return 0;
}
LRESULT CWorkunitFilterView::OnEnChangeEditUser(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    PostMessage(WFUM_REFRESHSTATE);
    return 0;
}
LRESULT CWorkunitFilterView::OnBnClickedCheckCluster(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    PostMessage(WFUM_REFRESHSTATE);
    return 0;
}

LRESULT CWorkunitFilterView::OnCbnSelendokComboCluster(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    /*
    switch(m_modeCtrl.GetCurSel())
    {
    case 0:
    case 1:
        m_mode = static_cast<SEARCHMODE>(m_modeCtrl.GetCurSel());
        break;
    default:
        m_mode = SEARCHMODE_UNKNOWN;
        break;
    }
    */
    return 0;
}

LRESULT CWorkunitFilterView::OnBnClickedCheckState(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    PostMessage(WFUM_REFRESHSTATE);
    return 0;
}

LRESULT CWorkunitFilterView::OnCbnSelendokComboState(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    PostMessage(CWM_REFRESH);
    return 0;
}

LRESULT CWorkunitFilterView::OnBnClickedCheckDateTimeRange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    PostMessage(WFUM_REFRESHSTATE);
    return 0;
}

LRESULT CWorkunitFilterView::OnBnClickedButtonSearch(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    m_search.EnableWindow(false);
    DoSaveState();
    WTL::CLockWindowUpdate lock(m_hWnd);
    m_Tree.DeleteAllItems();
    clib::thread run(__FUNCTION__, boost::bind(&DoSearch, this, 
        m_checkCluster ? static_cast<const TCHAR *>(m_comboClusterCtrl->m_value) : _T(""), 
        m_checkUser ? static_cast<const TCHAR *>(m_user) : _T(""), 
        m_checkState ? static_cast<const TCHAR *>(m_comboStateCtrl->m_value.MakeLower()) : _T(""), 
        m_checkDateTime ? m_dateTimeFrom : _T(""), 
        m_checkDateTime ? m_dateTimeTo : _T(""), 
        m_spinMaxCount.GetPos()));
    return 0;
}

LRESULT CWorkunitFilterView::OnBnClickedButtonClear(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    DoSaveState();
    WTL::CLockWindowUpdate lock(m_hWnd);
    m_Tree.DeleteAllItems();
    return 0;
}

void CWorkunitFilterView::operator()(Dali::IWorkunitVectorCom * /*workunits*/)
{
}

void CWorkunitFilterView::OnDoubleClick(Dali::IWorkunit &wu)
{
    DoDoubleClick(wu);
}

HWND CWorkunitFilterView::GetHWND()
{
    return m_hWnd;
}

bool CWorkunitFilterView::ShowClusterInLabel(const TCHAR * /*cluster*/)
{
    return (!m_checkCluster);
}

bool CWorkunitFilterView::ShowUserInLabel(const TCHAR * /*user*/)
{
    return (!m_checkUser);
}

void CWorkunitFilterView::DoSaveState()
{
    DoDataExchange(true);
    GetIConfig(QUERYBUILDER_CFG)->Set(GLOBAL_WORKUNITFIND_CHECKUSER, m_checkUser);
    GetIConfig(QUERYBUILDER_CFG)->Set(GLOBAL_WORKUNITFIND_USER, (const TCHAR *)m_user);
    GetIConfig(QUERYBUILDER_CFG)->Set(GLOBAL_WORKUNITFIND_CHECKCLUSTER, m_checkCluster);
    GetIConfig(QUERYBUILDER_CFG)->Set(GLOBAL_WORKUNITFIND_CLUSTER, (const TCHAR *)m_comboClusterCtrl->m_value);
    GetIConfig(QUERYBUILDER_CFG)->Set(GLOBAL_WORKUNITFIND_CHECKSTATE, m_checkState);
    GetIConfig(QUERYBUILDER_CFG)->Set(GLOBAL_WORKUNITFIND_STATE, (const TCHAR *)m_comboStateCtrl->m_value);
    GetIConfig(QUERYBUILDER_CFG)->Set(GLOBAL_WORKUNITFIND_CHECKDATETIME, m_checkDateTime);
    SYSTEMTIME date, time;
    m_dateFromCtrl.GetSystemTime(&date);
    m_timeFromCtrl.GetSystemTime(&time);
    DateTimeToUTC(date, time, m_dateTimeFrom);
    GetIConfig(QUERYBUILDER_CFG)->Set(GLOBAL_WORKUNITFIND_DATETIMEFROM, m_dateTimeFrom);
    m_dateToCtrl.GetSystemTime(&date);
    m_timeToCtrl.GetSystemTime(&time);
    DateTimeToUTC(date, time, m_dateTimeTo);
    GetIConfig(QUERYBUILDER_CFG)->Set(GLOBAL_WORKUNITFIND_DATETIMETO, m_dateTimeTo);
    GetIConfig(QUERYBUILDER_CFG)->Set(GLOBAL_WORKUNITFIND_MAXCOUNT, m_spinMaxCount.GetPos());
}

void CWorkunitFilterView::DoRestoreState()
{
    m_checkUser = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_WORKUNITFIND_CHECKUSER);
    m_user = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_WORKUNITFIND_USER);
    m_checkCluster = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_WORKUNITFIND_CHECKCLUSTER);
    m_comboClusterCtrl->Load();
    m_comboClusterCtrl->m_value = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_WORKUNITFIND_CLUSTER);
    m_checkState = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_WORKUNITFIND_CHECKSTATE);
    m_comboStateCtrl->Load();
    m_comboStateCtrl->m_value = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_WORKUNITFIND_STATE);
    m_checkDateTime = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_WORKUNITFIND_CHECKDATETIME);
    SYSTEMTIME dateTime;
    memset(&dateTime, 0, sizeof(dateTime));
    std::_tstring utcDateTime = CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_WORKUNITFIND_DATETIMEFROM));
    UTCToSystemTime(utcDateTime, dateTime, true);
    m_dateFromCtrl.SetSystemTime(GDT_VALID, &dateTime);
    m_timeFromCtrl.SetSystemTime(GDT_VALID, &dateTime);
    utcDateTime = CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_WORKUNITFIND_DATETIMETO));
    UTCToSystemTime(utcDateTime, dateTime, true);
    m_dateToCtrl.SetSystemTime(GDT_VALID, &dateTime);
    m_timeToCtrl.SetSystemTime(GDT_VALID, &dateTime);
    m_spinMaxCount.SetPos(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_WORKUNITFIND_MAXCOUNT));
    DoDataExchange();
    DoRefreshState();
}

void CWorkunitFilterView::DoRefreshState()
{
    m_editUserCtrl.EnableWindow(m_checkUser);
    m_comboClusterCtrl->EnableWindow(m_checkCluster);
    m_comboStateCtrl->EnableWindow(m_checkState);
    m_dateFromCtrl.EnableWindow(m_checkDateTime);
    m_timeFromCtrl.EnableWindow(m_checkDateTime);
    m_dateToCtrl.EnableWindow(m_checkDateTime);
    m_timeToCtrl.EnableWindow(m_checkDateTime);
}
void CWorkunitFilterView::Refresh()
{
}

void CWorkunitFilterView::DoSearch(CWorkunitFilterView *self, std::_tstring cluster, std::_tstring user, std::_tstring state, std::_tstring dateTimeFrom, std::_tstring dateTimeTo, int spinMaxCount)
{
    StlLinked<Dali::IDali> server = Dali::AttachDali(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT), _T("Dali"));
    Dali::IWorkunitVectorCom *results = new Dali::IWorkunitVectorCom(); //This is released in the submit done.
    server->GetWorkunits(cluster.c_str(), user.c_str(), state.c_str(), dateTimeFrom.c_str(), dateTimeTo.c_str(), spinMaxCount, results);
    ::PostMessage(self->m_hWnd, CWM_SUBMITDONE, (WPARAM)(results), 0);
}
//  ===========================================================================
