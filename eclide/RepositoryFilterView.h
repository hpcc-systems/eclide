#pragma once

#include "RepositoryView.h"
#include "CtlColor.h"
#include "FindReplace.h"

//  ===========================================================================
enum RFUM
{
	RFUM_FIRST = CWM_LAST + 1,
	RFUM_REFRESHSTATE,
	RFUM_LAST
};

static const TCHAR RepositoryFind[] = _T("RepositoryFind");
static const SectionLabelDefault GLOBAL_REPOSITORYFIND_TEXT(SectionLabel(RepositoryFind, _T("text")), _T(""));
static const SectionLabelDefault GLOBAL_REPOSITORYFIND_USER(SectionLabel(RepositoryFind, _T("user")), _T(""));
static const SectionLabelDefault GLOBAL_REPOSITORYFIND_MODE(SectionLabel(RepositoryFind, _T("mode")), static_cast<int>(SEARCHMODE_UNKNOWN));
static const SectionLabelDefault GLOBAL_REPOSITORYFIND_SANDBOXED(SectionLabel(RepositoryFind, _T("sandboxed")), false);
static const SectionLabelDefault GLOBAL_REPOSITORYFIND_CHECKEDOUT(SectionLabel(RepositoryFind, _T("checkedout")), false);
static const SectionLabelDefault GLOBAL_REPOSITORYFIND_LOCKED(SectionLabel(RepositoryFind, _T("locked")), false);
static const SectionLabelDefault GLOBAL_REPOSITORYFIND_ORPHANED(SectionLabel(RepositoryFind, _T("orphaned")), false);
static const SectionLabelDefault GLOBAL_REPOSITORYFIND_MODIFIEDSINCE(SectionLabel(RepositoryFind, _T("modifiedsince")), false);
static const SectionLabelDefault GLOBAL_REPOSITORYFIND_DATETIME(SectionLabel(RepositoryFind, _T("datetime")), _T("2000-01-01T00:00:00Z"));

class CRepositoryFilterView :
	public CRepositoryViewT<CRepositoryFilterView>,
	public WTL::CDialogImpl<CRepositoryFilterView>,
	public WTL::CDialogResize<CRepositoryFilterView>,
	public WTL::CWinDataExchange<CRepositoryFilterView>,
	public CCtlColor,
	public CUnknown
{
	typedef CRepositoryFilterView thisClass;
	typedef WTL::CDialogImpl<CRepositoryFilterView> baseClass;

protected:
	CString m_searchText;
	CString m_searchModule;
	CString m_searchUser;
	SEARCHMODE m_mode;
	bool m_sandboxed;
	bool m_checkedout;
	bool m_locked;
	bool m_orphaned;
	bool m_modifiedSince;
	std::_tstring m_dateTime;

	CComPtr<CComboModule> m_comboModuleCtrl;
	WTL::CComboBox m_modeCtrl;
	WTL::CDateTimePickerCtrl m_dateCtrl;
	WTL::CDateTimePickerCtrl m_timeCtrl;
	WTL::CTreeViewCtrl m_results;
	WTL::CButton m_search;

public:
	enum { IDD = IDD_REPOSITORYFILTER };

	IMPLEMENT_CUNKNOWN

	CRepositoryFilterView();
	~CRepositoryFilterView();

	void Init(IRepositorySlot * owner);

	BOOL PreTranslateMessage(MSG* pMsg);

	const TCHAR * GetSearchText();
	FINDMODE GetFindMode();

	BEGIN_MSG_MAP(thisClass)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CONTEXTMENU(OnContextMenu)

		MESSAGE_HANDLER_EX(CWM_CLEAR, OnClear)
		MESSAGE_HANDLER_EX(CWM_RESET, OnReset)
		MESSAGE_HANDLER_EX(CWM_SUBMITDONE, OnSubmitDone)
		MESSAGE_HANDLER_EX(RFUM_REFRESHSTATE, OnRefreshState)

		COMMAND_HANDLER(IDC_EDIT_SEARCH, EN_CHANGE, OnEnChangeEditSearch)
		COMMAND_HANDLER(IDC_EDIT_USER, EN_CHANGE, OnEnChangeUserSearch)
		COMMAND_HANDLER(IDC_COMBO_MODULE, CBN_SELENDOK, OnCbnSelendokComboModule)
		COMMAND_HANDLER(IDC_COMBO_MODE, CBN_SELENDOK, OnCbnSelendokComboMode)
		COMMAND_HANDLER(IDC_CHECK_SANDBOXED, BN_CLICKED, OnBnClickedCheckSandboxed)
		COMMAND_HANDLER(IDC_CHECK_CHECKEDOUT, BN_CLICKED, OnBnClickedCheckCheckedout)
		COMMAND_HANDLER(IDC_CHECK_LOCKED, BN_CLICKED, OnBnClickedCheckLocked)
		COMMAND_HANDLER(IDC_CHECK_ORPHANED, BN_CLICKED, OnBnClickedCheckOrphaned)
		COMMAND_HANDLER(IDC_CHECK_MODIFIEDSINCE, BN_CLICKED, OnBnClickedCheckModifiedsince)
		COMMAND_HANDLER(IDC_BUTTON_SEARCH, BN_CLICKED, OnBnClickedButtonSearch)
		COMMAND_HANDLER(IDC_BUTTON_CLEAR, BN_CLICKED, OnBnClickedButtonClear)
		COMMAND_HANDLER(IDC_BUTTON_COPY, BN_CLICKED, OnBnClickedButtonCopy)

		CHAIN_MSG_MAP(CCtlColor)
		REFLECT_NOTIFICATIONS()
		CHAIN_MSG_MAP(CDialogResize<CRepositoryFilterView>)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(thisClass)
		DLGRESIZE_CONTROL(IDC_EDIT_SEARCH, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_EDIT_USER, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_COMBO_MODULE, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_COMBO_MODE, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_DATETIMEPICKER_DATE, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_DATETIMEPICKER_TIME, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_BUTTON_SEARCH, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_BUTTON_CLEAR, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_TREE_REPOSITORY, DLSZ_SIZE_X | DLSZ_SIZE_Y)
	END_DLGRESIZE_MAP()

	BEGIN_DDX_MAP(thisClass)
		DDX_TEXT(IDC_EDIT_SEARCH, m_searchText)
		DDX_TEXT(IDC_EDIT_USER, m_searchUser)
		DDX_CHECK(IDC_CHECK_SANDBOXED, m_sandboxed)
		DDX_CHECK(IDC_CHECK_CHECKEDOUT, m_checkedout)
		DDX_CHECK(IDC_CHECK_LOCKED, m_locked)
		DDX_CHECK(IDC_CHECK_ORPHANED, m_orphaned)
		DDX_CHECK(IDC_CHECK_MODIFIEDSINCE, m_modifiedSince)
	END_DDX_MAP()

	LRESULT OnInitDialog(HWND hWnd, LPARAM lParam);
	//LRESULT OnTreeNotify(LPNMHDR nmhdr); 
	void OnContextMenu(HWND phWnd, CPoint pPoint);
	LRESULT OnClear(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
	LRESULT OnReset(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
	LRESULT OnSubmitDone(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
	LRESULT OnRefreshState(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);

	LRESULT OnEnChangeEditSearch(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnEnChangeUserSearch(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCbnSelendokComboModule(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCbnSelendokComboMode(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedCheckSandboxed(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedCheckCheckedout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedCheckLocked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedCheckOrphaned(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedCheckModifiedsince(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonSearch(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonClear(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonCopy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	bool UpdateUI(CCmdUI * cui);

	void DoSaveState();
	void DoRestoreState();
	void DoRefreshState();

	virtual void OnItemSelectionChanged(CTreeNode *, bool bSelected );
	virtual void OnItemExpanding(CTreeNode *, bool bExpanding);
	virtual void HandleNmClick(LPNMTREEVIEW);

protected:
	struct DoSearchOptions
	{
		SEARCHMODE mode;
		bool sandboxed;
		bool checkedout;
		bool locked;
		bool orphaned;
		bool modifiedSince;
	};
	static void DoSearch(CRepositoryFilterView *self, std::_tstring searchText, std::_tstring searchModule, std::_tstring searchUser, DoSearchOptions searchOptions, std::_tstring dateTime);

};
