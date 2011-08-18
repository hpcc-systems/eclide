#pragma once

#include "EclDlg.h"
#include "CtlColor.h"
#include "ButtonMenu.h"

enum BUM
{
	BUM_FIRST = CWM_LAST + 1,
	BUM_REFRESHQUEUECLUSTER,
	BUM_LAST
};
//  ===========================================================================
__interface IEclBuilderSlot : public ISourceSlot
{
	void OnButtonGo(Dali::WUAction action);
	void OnButtonDebug();
};

class CBuilderDlg : 
	public CEclDlgQBImpl<CBuilderDlg>,
	public WTL::CDialogResize<CBuilderDlg>,
	public WTL::CWinDataExchange<CBuilderDlg>,
	public CCtlColor,
	public boost::signals::trackable
{
	typedef CBuilderDlg thisClass;
	typedef CEclDlgQBImpl<thisClass> baseClass;
protected:
	CComPtr<CComboQueueCluster> m_comboQueueClusterCtrl;
	CComPtr<CComboLabel> m_comboLabelCtrl;

	WTL::CDateTimePickerCtrl m_dateCtrl;
	WTL::CDateTimePickerCtrl m_timeCtrl;
	WTL::CButton m_advancedCtrl;

	WTL::CUpDownCtrl m_limitResult;
	bool m_schedule;
	bool m_archive;
	bool m_noCommonPrivateAttributes;
	IEclBuilderSlot * m_owner;
	WTL::CUpDownCtrl m_maxRuntime;

	CString m_name;
	CString m_path;

	CString m_debug;

	bool m_advanced;

public:
	WTL::CButtonMenuCtrl m_goButton;
	WTL::CButton m_debugButton;
	WTL::CButton m_archiveButton;
	
	enum { IDD = IDD_BUILDERVIEW };

	CBuilderDlg(IEclBuilderSlot * owner);

	//  ---  CEclDlgQBImpl Virtuals  ---
	bool DoSave(bool attrOnly);
	void GetTitle(CString & title);
	//  ---  ---  ---

	bool DoFileOpen(const CString & sPathName);
	bool DoFileSave(const CString & sPathName);
	bool DoFileSaveAs();
	void SetNamePath(const CString & sPathName);
	const TCHAR * GetPath() const;

	void SetQueueCluster(const CString & queue, const CString & cluster);
	const TCHAR * GetQueue();
	const TCHAR * GetCluster();

	bool CanExecute();

	bool IsScheduled();
	void SetSchedule(const std::_tstring & utc);
	void GetSchedule(std::_tstring & utc);

	bool IsLabeled();
	void SetLabel(const CString & label);
	void GetLabel(CString & label);

	void GetName(CString & name);

	void SetResultLimit(int limit);
	int GetResultLimit();

	void SetDebug(const std::_tstring & debug);
	const TCHAR * GetDebug();

	bool IsArchive();
	//void SetArchive(bool archive);

	int GetMaxRuntime();
	void SetMaxRuntime(int maxRuntime);

	bool IsNoCommonPrivateAttributes();
	void SetNoCommonPrivateAttributes(bool cpa);

	void SavePersistInfo(CPersistMap & persistInfo);
	void RestorePersistInfo(const CPersistMap & persistInfo);
	void DoCheckSyntax();
	void DoCheckComplexity();
	void DoCheckDependency();
	void DoUpdateScheduleInfo();
	IAttribute * GetAttribute() {return NULL;};

	void ShowHide(UINT nID, bool hide);
	void ShowAdvanced();
	void HideAdvanced();

	LRESULT OnForwardMsg(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		LPMSG pMsg = (LPMSG)lParam;
		return CWindow::IsDialogMessage(pMsg);
	}

	BEGIN_MSG_MAP(thisClass)
		MSG_WM_INITDIALOG(OnInitDialog)
		MESSAGE_HANDLER_EX(CWM_INITIALIZE, OnInitialize)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_SIZE(OnSize)

		MESSAGE_HANDLER_EX(BUM_REFRESHQUEUECLUSTER, OnRefreshQueueCluster)

		COMMAND_ID_HANDLER_EX(ID_FILE_SAVE_AS, OnFileSaveAs)
		COMMAND_ID_HANDLER_EX(ID_ECL_GO, OnEclGo)
		COMMAND_ID_HANDLER_EX(ID_GO_SUBMIT, OnEclGo)
		COMMAND_ID_HANDLER_EX(ID_GO_COMPILE, OnEclGo)
		COMMAND_ID_HANDLER_EX(ID_EDIT_LOCATEFILEINEXPLORER, OnLocateFileInExplorer)
		COMMAND_ID_HANDLER_EX(ID_ECL_SYNCTOC, OnEclSyncToc)
		COMMAND_ID_HANDLER_EX(ID_ECL_GOTO, OnEclGoto)
		COMMAND_ID_HANDLER_EX(ID_ECL_GOTOSYNCTOC, OnEclGotoSyncToc)

		COMMAND_HANDLER(IDC_CHECK_SCHEDULE, BN_CLICKED, OnBnClickedCheckSchedule)
		COMMAND_HANDLER(IDC_BUTTON_GO, BN_CLICKED, OnBnClickedButtonGo)
 		COMMAND_HANDLER(IDC_BUTTON_DEBUG, BN_CLICKED, OnBnClickedButtonDebug)
		COMMAND_HANDLER(IDC_BUTTON_ADVANCED, BN_CLICKED, OnBnClickedButtonAdvanced)
		COMMAND_HANDLER(IDC_BUTTON_ARCHIVE, BN_CLICKED, OnBnClickedButtonArchive)
		COMMAND_HANDLER(IDC_COMBO_QUEUECLUSTER, CBN_SELENDOK, OnCbnSelendokComboCluster)

		MESSAGE_HANDLER(WM_FORWARDMSG, OnForwardMsg)

		CHAIN_MSG_MAP(CCtlColor)
		CHAIN_MSG_MAP(WTL::CDialogResize<thisClass>)
		CHAIN_MSG_MAP(CEclDlgQBImpl<thisClass>)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(thisClass)
		DLGRESIZE_CONTROL(IDC_STATIC_QUEUECLUSTER, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_COMBO_QUEUECLUSTER, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_BUTTON_ADVANCED, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_EDIT_DEBUG, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_BUTTON_DEBUG, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_BUTTON_ARCHIVE, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_EDIT_ECL, DLSZ_SIZE_X | DLSZ_SIZE_Y)
	END_DLGRESIZE_MAP()

	BEGIN_DDX_MAP(thisClass)
		DDX_TEXT(IDC_COMBO_LABEL, m_comboLabelCtrl->m_value)
		DDX_CHECK(IDC_CHECK_SCHEDULE, m_schedule)
		//DDX_CHECK(IDC_CHECK_ARCHIVE, m_archive)
		DDX_CHECK(IDC_CHECK_SYNTAXPLUS, m_noCommonPrivateAttributes)
		DDX_TEXT(IDC_EDIT_DEBUG, m_debug)
	END_DDX_MAP()

	LRESULT OnInitDialog(HWND hWnd, LPARAM lParam);
	LRESULT OnInitialize(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void OnDestroy();
	void OnSize(UINT nType, CSize size);
	LRESULT OnRefreshQueueCluster(UINT uMsg, WPARAM wParam, LPARAM lParam);

	void OnFileSaveAs(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/);

	void OnEclGo(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/);
	void OnLocateFileInExplorer(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/);
	void OnEclSyncToc(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/);
	void OnEclGoto(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/);
	void OnEclGotoSyncToc(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/);

	LRESULT OnBnClickedCheckSchedule(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonGo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonDebug(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonAdvanced(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonArchive(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnCbnSelendokComboCluster(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};
//  ===========================================================================
