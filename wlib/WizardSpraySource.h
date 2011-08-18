#pragma once

#include "WizardSprayData.h"

class CWizardSpraySource :
	public CWizard97InteriorPageImpl<CWizardSpraySource>, 
	public CWinDataExchange<CWizardSpraySource>
{
	typedef CWizardSpraySource thisClass;
	typedef CWizard97InteriorPageImpl<CWizardSpraySource> baseClass;

protected:
	CWizardSprayData *m_data;

	CListViewCtrl m_ctrl_listview;
	CButton m_ctrl_gobutton;
	CButton m_ctrl_upbutton;

	CString m_ddx_ip;
	CString m_ddx_path;
    
public:
	CWizardSpraySource(CWizardSprayData *data, _U_STRINGorID title = (LPCTSTR)NULL);
	~CWizardSpraySource();

// Message Handlers
	enum { IDD = IDD_WIZ97_PATHFILTER };
	BEGIN_MSG_MAP(thisClass)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)

		COMMAND_HANDLER(IDC_EDIT_FILEPATH, EN_CHANGE, OnEnChangeEditFilepath)
		COMMAND_HANDLER(IDC_BUTTON_UP, BN_CLICKED, OnBnClickedButtonUp)

		NOTIFY_HANDLER(IDC_LIST_FILEBROWSER, NM_DBLCLK, OnNmDblCLickListBrowser)
		NOTIFY_CODE_HANDLER(LVN_DELETEITEM, OnNotifyDeleteItem)

		COMMAND_HANDLER(IDC_BUTTON_GO, BN_CLICKED, OnBnClickedButtonGo)

		CHAIN_MSG_MAP(baseClass)
	END_MSG_MAP()

	BEGIN_DDX_MAP(thisClass)
		DDX_TEXT(IDC_EDIT_IP, m_ddx_ip)
		DDX_TEXT(IDC_EDIT_FILEPATH, m_ddx_path)
	END_DDX_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

// Helpers
	void InitializeControls(void);
	void UninitializeControls(void);
	void InitializeValues(void);
	bool StoreValues(void);

// Overrides from base class
	int OnSetActive();
	int OnWizardNext();
	int OnWizardBack();
	void OnHelp();

//  ============
	void SetView(HWND hWndListView, DWORD dwView);
	void RefreshPath();
	void LoadRoot();
	void LoadFolder();

	LRESULT OnEnChangeEditFilepath(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonGo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonUp(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnNmDblCLickListBrowser(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);
	LRESULT OnNotifyDeleteItem(int idCtrl, LPNMHDR pnmh, BOOL& /*bHandled*/);
};
