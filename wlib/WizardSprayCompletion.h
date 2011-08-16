#pragma once

#include "WizardSprayData.h"

class CWizardSprayCompletion : public CWizard97ExteriorPageImpl<CWizardSprayCompletion>
{
	typedef CWizardSprayCompletion thisClass;
	typedef CWizard97ExteriorPageImpl<CWizardSprayCompletion> baseClass;

protected:
	CWizardSprayData *m_data;

	CFont m_fontSummary;
	CRichEditCtrl m_editSummary;

public:
	CWizardSprayCompletion(CWizardSprayData *data, _U_STRINGorID title = (LPCTSTR)NULL);
	~CWizardSprayCompletion();

	enum { IDD = IDD_WIZ97_COMPLETION };
	BEGIN_MSG_MAP(thisClass)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)

		CHAIN_MSG_MAP(baseClass)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

// Helpers
	void InitializeFont(void);
	void InitializeControls(void);
	void InitializeValues(void);
	void UpdateSummary(void);

// Overrides from base class
	int OnSetActive();
	int OnWizardBack();
	int OnWizardFinish();
	void OnHelp();
};
