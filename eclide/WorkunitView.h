#pragma once

#include "SortList.h"
#include "Util.h"
#include "MultiTreeNode.h"
#include "DaliTreeNode.h"
#include "Combo.h"
#include "CtlColor.h"

enum WFUM
{
	WFUM_FIRST = CWM_LAST + 1,
	WFUM_REFRESHSTATE,
	WFUM_LAST
};

__interface IWorkunitSlot
{
	bool OnDoubleClick(Dali::IWorkunit &wu);
	bool OnContext(CMenu &menu, CPoint &pt);
};
//  ===========================================================================
template<typename T>
class CWorkunitViewAction 
{
public:
	void DoCopy(Dali::IWorkunitVector &wus)
	{
		CString txt;
		for (unsigned i = 0; i < wus.size(); ++i)
		{
			if (i > 0)
				txt += ", ";
			txt += wus[i]->GetWuid();
		}

		::SetClipboard((const TCHAR * )txt);
		return;
	}
	void DoAbort(Dali::IWorkunitVector &wus)
	{
		T * pT = static_cast<T*>(this);
		StlLinked<Dali::IDali> server = Dali::AttachDali(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT), _T("Dali"));
		server->AbortWorkunits(&wus);
		pT->Refresh();
	}
	void DoDelete(Dali::IWorkunitVector &wus)
	{
		StlLinked<Dali::IDali> server = Dali::AttachDali(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT), _T("Dali"));
		server->DeleteWorkunits(&wus);
	}
	void DoProtect(Dali::IWorkunitVector &wus, bool protect = true)
	{
		T * pT = static_cast<T*>(this);
		StlLinked<Dali::IDali> server = Dali::AttachDali(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT), _T("Dali"));
		if (protect)
			server->ProtectWorkunits(&wus);
		else
			server->UnprotectWorkunits(&wus);
		pT->Refresh();
	}
	void DoFollowup(Dali::IWorkunitVector &wus, bool protect = true)
	{
		T * pT = static_cast<T*>(this);
		StlLinked<Dali::IDali> server = Dali::AttachDali(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT), _T("Dali"));
		if (protect)
			server->FlagFollowupWorkunits(&wus);
		else
			server->ClearFollowupWorkunits(&wus);
		pT->Refresh();
	}
	void DoResubmit(Dali::IWorkunitVector &wus)
	{
		T * pT = static_cast<T*>(this);
		StlLinked<Dali::IDali> server = Dali::AttachDali(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT), _T("Dali"));
		server->ResubmitWorkunits(&wus);
		pT->Refresh();
	}
	void DoRefresh(Dali::IWorkunitVector &/*wus*/)
	{
		T * pT = static_cast<T*>(this);
		//StlLinked<Dali::IDali> server = Dali::AttachDali(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT), _T("Dali"));
		//TODO:  server->RefreshWorkunits(&wus);
		pT->Refresh();
	}
	void DoSetPriority(Dali::IWorkunitVector &wus, PRIORITY priority)
	{
		T * pT = static_cast<T*>(this);
		StlLinked<SMC::ISMC> server = SMC::AttachSMC(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_SMC), _T("SMC"));
		server->SetPriority(&wus, priority);
		pT->Refresh();
	}
	void DoSetState(Dali::IWorkunitVector &wus, Dali::WUState state)
	{
		T * pT = static_cast<T*>(this);
		StlLinked<Dali::IDali> server = Dali::AttachDali(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT), _T("Dali"));
		server->SetState(&wus, state);
		pT->Refresh();
	}
	void DoDoubleClick(Dali::IWorkunit &wu)
	{
		T * pT = static_cast<T*>(this);
		pT->m_Owner->OnDoubleClick(wu);
	}
	void OnContextMenu(HWND phWnd, CPoint pt)
	{
		T * pT = static_cast<T*>(this);
		pT->SetMsgHandled(true);
		if (phWnd == (HWND)pT->m_Tree)
		{
			CMenu m;
			if (!pT->m_Owner->OnContext(m, pt))
			{
				ATLASSERT(!"ToDo");
			}
		}
	}
	bool GetSelected(CComPtr<Dali::IWorkunit> &wu)
	{
		T * pT = static_cast<T*>(this);
		Dali::IWorkunitVector wus;
		if (GetSelected(wus))
		{
			wu = wus[0].get();
			return true;
		}
		return false;
	}
	unsigned GetSelected(Dali::IWorkunitVector &wus)
	{
		T * pT = static_cast<T*>(this);
		CTreeNodeVector nodes;
		pT->m_Tree.GetSelected(nodes);
		for(CTreeNodeVector::iterator itr = nodes.begin(); itr != nodes.end(); ++itr)
		{
			CComQIPtr<CWorkunitNode> wuNode = itr->get();
			if (wuNode)
				wus.push_back(wuNode->GetWorkunit());
		}
		return wus.size();
	}
	void HandleNmClick(LPNMTREEVIEW /*pNmHdr*/)
	{
	}
	void OnItemSelectionChanged(CTreeNode * /*node*/, bool /*bSelected*/)
	{
	}
	void OnItemExpanding(CTreeNode *, bool /*bExpanding*/)
	{
	}
	BOOL PreTranslateMessage(MSG* pMsg)
	{
		BOOL retVal = IsDialogMessage(pMsg);
		return retVal;
	}
};
//  ===========================================================================
class CWorkunitTreeView :
	public IDaliSlot, 
	public CWorkunitViewAction<CWorkunitTreeView>,
	public WTL::CDialogImpl<CWorkunitTreeView>,
	public WTL::CDialogResize<CWorkunitTreeView>,
	public WTL::CWinDataExchange<CWorkunitTreeView>,
	public CCtlColor
{
	typedef CWorkunitTreeView thisClass;
	typedef WTL::CDialogImpl<CWorkunitTreeView> baseClass;

protected:
	bool m_AllUsers;
	CComPtr<CComboCluster> m_comboClusterCtrl;
	CComPtr<CDaliNode> m_Root;
	CComPtr<Dali::IWorkunit> m_syncToWU;
	boost::gregorian::date m_date;

	bool AllClusters() const;

public:
	IWorkunitSlot *m_Owner;
	CMultiTreeNodeViewCtrl m_Tree;

	enum { IDD = IDD_WORKUNITTREE };

	CWorkunitTreeView();
	~CWorkunitTreeView();

	void Init(IWorkunitSlot *owner);
	bool Locate(Dali::IWorkunit * wu, bool expandIfNeeded);

	BOOL PreTranslateMessage(MSG* pMsg);
	//IDaliSlot
	void OnDoubleClick(Dali::IWorkunit &wu);
	HWND GetHWND();
	bool ShowClusterInLabel(const TCHAR *cluster);
	bool ShowUserInLabel(const TCHAR *user);
	void UpdateWorkunits(HTREEITEM hItem, Dali::IWorkunitVectorAdapt wus);

	BEGIN_MSG_MAP(thisClass)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CONTEXTMENU(OnContextMenu)
		MSG_WM_TIMER(OnTimer)

		COMMAND_HANDLER(IDC_CHECK_ALLUSERS, BN_CLICKED, OnBnClickedCheckAllUsers)
		COMMAND_HANDLER(IDC_COMBO_CLUSTERS, CBN_SELENDOK, OnCbnSelendokComboClusters)
		MESSAGE_HANDLER_EX(CWM_CLEAR, OnClear)
		MESSAGE_HANDLER_EX(CWM_RESET, OnReset)
		MESSAGE_HANDLER_EX(CWM_REFRESH, OnRefresh)
		MESSAGE_HANDLER_EX(CWM_NODEUPDATECOMPLETE, OnNodeUpdate)
		
		CHAIN_MSG_MAP(CCtlColor)
		REFLECT_NOTIFICATIONS()
		CHAIN_MSG_MAP(WTL::CDialogResize<thisClass>)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(thisClass)
		DLGRESIZE_CONTROL(IDC_COMBO_CLUSTERS, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_TREE_WORKUNIT, DLSZ_SIZE_X | DLSZ_SIZE_Y)
	END_DLGRESIZE_MAP()

	BEGIN_DDX_MAP(thisClass)
		DDX_CHECK(IDC_CHECK_ALLUSERS, m_AllUsers)
		DDX_TEXT(IDC_COMBO_CLUSTERS, m_comboClusterCtrl->m_value)
	END_DDX_MAP()

	LRESULT OnInitDialog(HWND hWnd, LPARAM lParam);
	void OnTimer(UINT_PTR nIDEvent);

	LRESULT OnBnClickedCheckAllUsers(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCbnSelendokComboClusters(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
		
	LRESULT OnClear(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
	LRESULT OnReset(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
	LRESULT OnRefresh(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
	LRESULT OnNodeUpdate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
	void Update(HTREEITEM hItem, Dali::IWorkunitVectorCom* wus);
	void Refresh();
	void Refresh(Dali::IWorkunit * wu);
};
//  ===========================================================================
class CActiveWorkunitTreeView :
	public IDaliSlot, 
	public CWorkunitViewAction<CActiveWorkunitTreeView>,
	public WTL::CDialogImpl<CActiveWorkunitTreeView>,
	public WTL::CDialogResize<CActiveWorkunitTreeView>,
	public WTL::CWinDataExchange<CActiveWorkunitTreeView>,
	public CCtlColor
{
	typedef CActiveWorkunitTreeView thisClass;
	typedef WTL::CDialogImpl<thisClass> baseClass;

protected:
	clib::recursive_mutex m_mutex;
	int	m_ActiveRefreshCount; //queue size sort of
	bool m_active;

public:
	CMultiTreeNodeViewCtrl m_Tree;
	IWorkunitSlot *m_Owner;

	enum { IDD = IDD_WORKUNITACTIVE };

	CActiveWorkunitTreeView();
	~CActiveWorkunitTreeView();

	void Init(IWorkunitSlot *owner);

	BOOL PreTranslateMessage(MSG* pMsg);

	//IDaliSlot
	void OnDoubleClick(Dali::IWorkunit &wu);
	HWND GetHWND();
	bool ShowClusterInLabel(const TCHAR *cluster);
	bool ShowUserInLabel(const TCHAR *user);
	void UpdateWorkunits(HTREEITEM /*hItem*/, Dali::IWorkunitVectorAdapt wus) 
	{ ATLASSERT(!_T("NOT IMPLEMENTED!"));
	}

	BEGIN_MSG_MAP(thisClass)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CONTEXTMENU(OnContextMenu)
		MSG_WM_TIMER(OnTimer)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)

		COMMAND_HANDLER(IDC_CHECK_ALLUSERS, BN_CLICKED, OnBnClickedCheckAllUsers)
		COMMAND_HANDLER(IDC_COMBO_CLUSTERS, CBN_SELENDOK, OnCbnSelendokComboClusters)
		MESSAGE_HANDLER_EX(CWM_CLEAR, OnClear)
		MESSAGE_HANDLER_EX(CWM_RESET, OnReset)
		MESSAGE_HANDLER_EX(CWM_REFRESH, OnRefresh)
		MESSAGE_HANDLER_EX(CWM_SUBMITDONE, OnSubmitDone)

		CHAIN_MSG_MAP(CCtlColor)
		REFLECT_NOTIFICATIONS()
		CHAIN_MSG_MAP(WTL::CDialogResize<thisClass>)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(thisClass)
		DLGRESIZE_CONTROL(IDC_TREE_WORKUNIT, DLSZ_SIZE_X | DLSZ_SIZE_Y)
	END_DLGRESIZE_MAP()

	BEGIN_DDX_MAP(thisClass)
	END_DDX_MAP()

	LRESULT OnInitDialog(HWND hWnd, LPARAM lParam);
	void OnTimer(UINT_PTR nIDEvent);
	LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled);

	LRESULT OnBnClickedCheckAllUsers(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCbnSelendokComboClusters(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
		
	LRESULT OnClear(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
	LRESULT OnReset(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
	LRESULT OnRefresh(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
	LRESULT OnSubmitDone(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
	void Refresh();
	void Refresh(SMC::IClusterVectorCom *clusters);

protected:
	static void DoItemExpanding(CActiveWorkunitTreeView * owner, WTL::CTreeViewCtrlEx * tv);
};
//  ===========================================================================
static const SectionLabelDefault GLOBAL_WORKUNITFIND_CHECKUSER(SectionLabel(_T("WorkunitFind"), _T("checkuser")), false);
static const SectionLabelDefault GLOBAL_WORKUNITFIND_USER(SectionLabel(_T("WorkunitFind"), _T("user")), _T(""));
static const SectionLabelDefault GLOBAL_WORKUNITFIND_CHECKCLUSTER(SectionLabel(_T("WorkunitFind"), _T("checkcluster")), false);
static const SectionLabelDefault GLOBAL_WORKUNITFIND_CLUSTER(SectionLabel(_T("WorkunitFind"), _T("cluster")), _T(""));
static const SectionLabelDefault GLOBAL_WORKUNITFIND_CHECKSTATE(SectionLabel(_T("WorkunitFind"), _T("checkstate")), false);
static const SectionLabelDefault GLOBAL_WORKUNITFIND_STATE(SectionLabel(_T("WorkunitFind"), _T("state")), _T(""));
static const SectionLabelDefault GLOBAL_WORKUNITFIND_CHECKDATETIME(SectionLabel(_T("WorkunitFind"), _T("checkdatetime")), false);
static const SectionLabelDefault GLOBAL_WORKUNITFIND_DATETIMEFROM(SectionLabel(_T("WorkunitFind"), _T("datetimefrom")), _T("2000-01-01T00:00:00Z"));
static const SectionLabelDefault GLOBAL_WORKUNITFIND_DATETIMETO(SectionLabel(_T("WorkunitFind"), _T("datetimeto")), _T("2010-01-01T00:00:00Z"));
static const SectionLabelDefault GLOBAL_WORKUNITFIND_MAXCOUNT(SectionLabel(_T("WorkunitFind"), _T("maxcount")), (int)50);

class CWorkunitFilterView :
	public IDaliSlot, 
	public CWorkunitViewAction<CWorkunitFilterView>,
	public WTL::CDialogImpl<CWorkunitFilterView>,
	public WTL::CDialogResize<CWorkunitFilterView>,
	public WTL::CWinDataExchange<CWorkunitFilterView>,
	public CCtlColor,
	public boost::signals::trackable
{
	typedef CWorkunitFilterView thisClass;
	typedef WTL::CDialogImpl<CWorkunitFilterView> baseClass;

protected:
	bool m_checkUser;
	CString m_user;
	bool m_checkCluster;
	bool m_checkState;
	bool m_checkDateTime;
	std::_tstring m_dateTimeFrom;
	std::_tstring m_dateTimeTo;

	WTL::CEdit m_editUserCtrl;
	CComPtr<CComboCluster> m_comboClusterCtrl;
	CComPtr<CComboState> m_comboStateCtrl;
	WTL::CDateTimePickerCtrl m_dateFromCtrl;
	WTL::CDateTimePickerCtrl m_timeFromCtrl;
	WTL::CDateTimePickerCtrl m_dateToCtrl;
	WTL::CDateTimePickerCtrl m_timeToCtrl;
	WTL::CTreeViewCtrl m_results;
	WTL::CButton m_search;
	WTL::CUpDownCtrl m_spinMaxCount;

public:
	CMultiTreeNodeViewCtrl m_Tree;
	IWorkunitSlot *m_Owner;

	enum { IDD = IDD_WORKUNITFILTER };

	CWorkunitFilterView();
	~CWorkunitFilterView();

	void Init(IWorkunitSlot * owner);

	BOOL PreTranslateMessage(MSG* pMsg);

	BEGIN_MSG_MAP(thisClass)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CONTEXTMENU(OnContextMenu)

		MESSAGE_HANDLER_EX(CWM_CLEAR, OnClear)
		MESSAGE_HANDLER_EX(CWM_RESET, OnReset)
		MESSAGE_HANDLER_EX(CWM_SUBMITDONE, OnSubmitDone)
		MESSAGE_HANDLER_EX(WFUM_REFRESHSTATE, OnRefreshState)

		COMMAND_HANDLER(IDC_CHECK_USER, BN_CLICKED, OnBnClickedCheckUser)
		COMMAND_HANDLER(IDC_EDIT_USER, EN_CHANGE, OnEnChangeEditUser)
		COMMAND_HANDLER(IDC_CHECK_CLUSTER, BN_CLICKED, OnBnClickedCheckCluster)
		COMMAND_HANDLER(IDC_COMBO_CLUSTER, CBN_SELENDOK, OnCbnSelendokComboCluster)
		COMMAND_HANDLER(IDC_CHECK_STATE, BN_CLICKED, OnBnClickedCheckState)
		COMMAND_HANDLER(IDC_COMBO_STATE, CBN_SELENDOK, OnCbnSelendokComboState)
		COMMAND_HANDLER(IDC_CHECK_DATETIMERANGE, BN_CLICKED, OnBnClickedCheckDateTimeRange)
		COMMAND_HANDLER(IDC_BUTTON_SEARCH, BN_CLICKED, OnBnClickedButtonSearch)
		COMMAND_HANDLER(IDC_BUTTON_CLEAR, BN_CLICKED, OnBnClickedButtonClear)

		CHAIN_MSG_MAP(CCtlColor)
		REFLECT_NOTIFICATIONS()
		CHAIN_MSG_MAP(WTL::CDialogResize<CWorkunitFilterView>)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(thisClass)
		DLGRESIZE_CONTROL(IDC_EDIT_USER, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_COMBO_CLUSTER, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_COMBO_STATE, DLSZ_SIZE_X)
		//DLGRESIZE_CONTROL(IDC_DATETIMEPICKER_DATEFROM, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_DATETIMEPICKER_TIMEFROM, DLSZ_SIZE_X)
		//DLGRESIZE_CONTROL(IDC_DATETIMEPICKER_DATETO, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_DATETIMEPICKER_TIMETO, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_BUTTON_SEARCH, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_BUTTON_CLEAR, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_TREE_WORKUNIT, DLSZ_SIZE_X | DLSZ_SIZE_Y)
	END_DLGRESIZE_MAP()

	BEGIN_DDX_MAP(thisClass)
		DDX_CHECK(IDC_CHECK_USER, m_checkUser)
		DDX_CHECK(IDC_CHECK_CLUSTER, m_checkCluster)
		DDX_TEXT(IDC_COMBO_CLUSTER, m_comboClusterCtrl->m_value)
		DDX_CHECK(IDC_CHECK_STATE, m_checkState)
		DDX_TEXT(IDC_COMBO_STATE, m_comboStateCtrl->m_value)
		DDX_CHECK(IDC_CHECK_DATETIMERANGE, m_checkDateTime)
		DDX_TEXT(IDC_EDIT_USER, m_user)
	END_DDX_MAP()

	LRESULT OnInitDialog(HWND hWnd, LPARAM lParam);
	LRESULT OnClear(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
	LRESULT OnReset(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
	LRESULT OnSubmitDone(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
	LRESULT OnRefreshState(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);

	LRESULT OnBnClickedCheckUser(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnEnChangeEditUser(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedCheckCluster(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCbnSelendokComboCluster(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedCheckState(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCbnSelendokComboState(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnBnClickedCheckDateTimeRange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonSearch(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonClear(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	void operator()(Dali::IWorkunitVectorCom *workunits);

	void OnDoubleClick(Dali::IWorkunit &wu);
	HWND GetHWND();
	bool ShowClusterInLabel(const TCHAR *cluster);
	bool ShowUserInLabel(const TCHAR *user);
	void UpdateWorkunits(HTREEITEM /*hItem*/, Dali::IWorkunitVectorAdapt wus)
	{ ATLASSERT(!_T("NOT IMPLEMENTED!"));
	}

	void DoSaveState();
	void DoRestoreState();
	void DoRefreshState();
	void Refresh();

protected:
	static void DoSearch(CWorkunitFilterView *self, std::_tstring cluster, std::_tstring user, std::_tstring state, std::_tstring dateTimeFrom, std::_tstring dateTimeTo, int spinMaxCount);
};
//  ===========================================================================
