#pragma once

#include "WizardSprayData.h"

class CWizardSprayWelcome :	public CWizard97ExteriorPageImpl<CWizardSprayWelcome>
{
	typedef CWizardSprayWelcome thisClass;
	typedef CWizard97ExteriorPageImpl<CWizardSprayWelcome> baseClass;

protected:
	CWizardSprayData *m_data;

	CButton m_buttonSkipWelcome;
	bool m_allowWelcomeToHide;

public:
	CWizardSprayWelcome(CWizardSprayData *data, _U_STRINGorID title = (LPCTSTR)NULL);
	~CWizardSprayWelcome();

	enum { IDD = IDD_WIZ97_WELCOME };
	BEGIN_MSG_MAP(thisClass)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)

		CHAIN_MSG_MAP(baseClass)
	END_MSG_MAP()

// Message Handlers
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

// Helper methods
	void InitializeControls(void);
	void InitializeValues(void);
	bool StoreValues(void);

// Overrides from base class
	int OnSetActive();
	int OnWizardNext();
	void OnHelp();
};

