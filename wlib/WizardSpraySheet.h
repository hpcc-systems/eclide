#pragma once

#include "WizardSprayData.h"
#include "WizardSprayWelcome.h"
#include "WizardSpraySource.h"
#include "WizardSprayAnalyze.h"
#include "WizardSprayTarget.h"
//TJ#include "WizardSpraySample.h"
#include "WizardSprayCompletion.h"

class CWizardSpraySheet : public CWizard97SheetImpl<CWizardSpraySheet>
{
	typedef CWizardSpraySheet thisClass;
	typedef CWizard97SheetImpl<CWizardSpraySheet> baseClass;

protected:
	CWizardSprayData *m_data;

	CWizardSprayWelcome m_pageWelcome;
	CWizardSpraySource m_pageSource;
	CWizardSprayTarget m_pageTarget;
	CWizardSprayTargetQB m_pageTargetQB;
	CWizardSprayAnalyze m_pageAnalyze;
// TJ	CWizardSpraySample m_pageSample;
	CWizardSprayCompletion m_pageCompletion;

public:
	CWizardSpraySheet(CWizardSprayData *data, UINT uStartPage = 0, HWND hWndParent = NULL);
	~CWizardSpraySheet();

// Message Handlers
	BEGIN_MSG_MAP(thisClass)
		MESSAGE_HANDLER(WM_HELP, OnHelp)
		CHAIN_MSG_MAP(baseClass)
	END_MSG_MAP()

	LRESULT OnHelp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};
