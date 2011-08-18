#pragma once

#include "WizardSprayData.h"
#include "FileAnalyzer.h"
#include "EclExec.h" //QueryBuilder
#include "Combo.h"

class CWizardSprayAnalyze :
	public CWizard97InteriorPageImpl<CWizardSprayAnalyze>, 
	public CWinDataExchange<CWizardSprayAnalyze>,
	public CEclExec
{
	typedef CWizardSprayAnalyze thisClass;
	typedef CWizard97InteriorPageImpl<CWizardSprayAnalyze> baseClass;

protected:
	CWizardSprayData *m_data;
	CFileAnalyzer m_analyzer;
	// true if we left this page and went to the next one
	// this way we know not to start analysis if we come back
	bool m_bNext; 

	CRichEditCtrl m_analyzeStatus;

	Dali::IWorkunitAdapt m_workunit;
	std::_tstring m_knownFile;
	__int64 m_knownFileSize;
	int m_nExceptionCount;


public:
	CWizardSprayAnalyze(CWizardSprayData *data, _U_STRINGorID title = (LPCTSTR)NULL);
	~CWizardSprayAnalyze();

// Message Handlers
	enum { IDD = IDD_WIZ97_ANALYZE };

	enum UM
	{
		UM_FIRST = CWM_LAST + 1,
		UM_FILE_ERROR,
		UM_WORKUNIT_CREATED,
		UM_RESULTS_COMPLETE,
		UM_FILE_NOTFOUND_ERROR,
		UM_START_ANALYSIS,
		UM_LAST
	};

	BEGIN_MSG_MAP(thisClass)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)

		MESSAGE_HANDLER(UM_FILE_ERROR, OnFileError)
		MESSAGE_HANDLER(UM_FILE_NOTFOUND_ERROR, OnFileNotFoundError)
		MESSAGE_HANDLER(UM_WORKUNIT_CREATED, OnWorkUnitCreated)
		MESSAGE_HANDLER(UM_RESULTS_COMPLETE, OnResultsComplete)
		MESSAGE_HANDLER(CWM_STATUSUPDATE, OnLogMessage)
		MESSAGE_HANDLER(UM_START_ANALYSIS, OnStartAnalysis);

		CHAIN_MSG_MAP(baseClass)
	END_MSG_MAP()

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
	LRESULT OnStartAnalysis(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnFileError(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnFileNotFoundError(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnWorkUnitCreated(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnResultsComplete(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	//  CExecEcl override  ---
	void DoAnalyzeFile();
	void WorkunitUpdated(Dali::IWorkunit * workunit);
	void WorkunitCreated(Dali::IWorkunit * workunit);
	bool ProcessExceptions();
	bool ProcessErrorMsg(const TCHAR *pMsg);
	bool ProcessFileNotFoundError(CString& msg);
	bool ProcessFileSizeError(CString& msg);
	bool ProcessCSVLineError(CString& msg);

	void PostLogMessage(const TCHAR* pStr);
	LRESULT OnLogMessage(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	void LogMessage(const std::_tstring &msg);
};
