#pragma once

#include "WizardSprayData.h"
#include "FileAnalyzer.h"
#include "EclExec.h" //QueryBuilder
#include "Combo.h"

class CWizardSprayTarget :
	public CWizard97InteriorPageImpl<CWizardSprayTarget>, 
	public CWinDataExchange<CWizardSprayTarget>
{
	typedef CWizardSprayTarget thisClass;
	typedef CWizard97InteriorPageImpl<CWizardSprayTarget> baseClass;

protected:
	CWizardSprayData *m_data;
	CFileAnalyzer m_analyzer;

	CString m_destination;
	CString m_module;
	CString m_prefix;
	CComPtr<CComboCluster> m_analyzeCombo;
	CString m_cluster;

	CString m_targetRecord;
	CButton m_targetRecordButton;

	Dali::IWorkunitAdapt m_workunit;
	std::_tstring m_knownFile;
	__int64 m_knownFileSize;
	int m_nExceptionCount;

public:
	CWizardSprayTarget(CWizardSprayData *data, _U_STRINGorID title = (LPCTSTR)NULL);
	~CWizardSprayTarget();

// Message Handlers
	enum { IDD = IDD_WIZ97_TARGET };

	#define UM_FILE_ERROR			(CLIB_FIRST-1)
	#define UM_WORKUNIT_CREATED		(CLIB_FIRST-2)
	#define UM_RESULTS_COMPLETE		(CLIB_FIRST-3)
	#define UM_FILE_NOTFOUND_ERROR	(CLIB_FIRST-4)

	BEGIN_MSG_MAP(thisClass)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)

		COMMAND_HANDLER(IDC_BUTTON_TARGET, BN_CLICKED, OnBnClickedTargetRecord)

		CHAIN_MSG_MAP(baseClass)
	END_MSG_MAP()

	BEGIN_DDX_MAP(thisClass)
		DDX_TEXT(IDC_EDIT_DESTINATION, m_destination)
		DDX_TEXT(IDC_EDIT_MODULE, m_module)
		DDX_TEXT(IDC_EDIT_PREFIX, m_prefix)
		DDX_TEXT(IDC_COMBO_CLUSTER, m_cluster)
		DDX_TEXT(IDC_EDIT_TARGET, m_targetRecord)
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

//  Command handlers
	LRESULT OnBnClickedTargetRecord(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};

//  ===========================================================================
class CWizardSprayTargetQB :
	public CWizard97InteriorPageImpl<CWizardSprayTargetQB>, 
	public CWinDataExchange<CWizardSprayTargetQB>
{
	typedef CWizardSprayTargetQB thisClass;
	typedef CWizard97InteriorPageImpl<CWizardSprayTargetQB> baseClass;

protected:
	CWizardSprayData *m_data;
	CFileAnalyzer m_analyzer;

	CString m_destination;
	CComPtr<CComboCluster> m_analyzeCombo;
	CString m_cluster;

	Dali::IWorkunitAdapt m_workunit;
	std::_tstring m_knownFile;
	__int64 m_knownFileSize;
	int m_nExceptionCount;

public:
	CWizardSprayTargetQB(CWizardSprayData *data, _U_STRINGorID title = (LPCTSTR)NULL);
	~CWizardSprayTargetQB();

// Message Handlers
	enum { IDD = IDD_WIZ97_TARGETQB };

	#define UM_FILE_ERROR			(CLIB_FIRST-1)
	#define UM_WORKUNIT_CREATED		(CLIB_FIRST-2)
	#define UM_RESULTS_COMPLETE		(CLIB_FIRST-3)
	#define UM_FILE_NOTFOUND_ERROR	(CLIB_FIRST-4)

	BEGIN_MSG_MAP(thisClass)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)

		CHAIN_MSG_MAP(baseClass)
	END_MSG_MAP()

	BEGIN_DDX_MAP(thisClass)
		DDX_TEXT(IDC_EDIT_DESTINATION, m_destination)
		DDX_TEXT(IDC_COMBO_CLUSTER, m_cluster)
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
};
