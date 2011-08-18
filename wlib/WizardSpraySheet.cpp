#include "StdAfx.h"
#include "..\en_us\resource.h"

#include ".\wizardspraysheet.h"

CWizardSpraySheet::CWizardSpraySheet(CWizardSprayData *data, UINT uStartPage, HWND hWndParent) : 
	baseClass(_T("Spray Wizard"), IDB_WIZ97_HEADER, IDB_WIZ97_WATERMARK, uStartPage, hWndParent), 
	m_pageWelcome(data),
	m_pageSource(data),
	m_pageTarget(data),
	m_pageTargetQB(data),
	m_pageAnalyze(data),
//TJ	m_pageSample(data),
	m_pageCompletion(data)
{
	m_data = data;
	AddPage(m_pageWelcome);
	AddPage(m_pageSource);
	if (m_data->GetGrETLFlag())
		AddPage(m_pageTarget);
	else
		AddPage(m_pageTargetQB);
	AddPage(m_pageAnalyze);
//	AddPage(m_pageSample);
	if (m_data->GetGrETLFlag())
		AddPage(m_pageCompletion);
}

CWizardSpraySheet::~CWizardSpraySheet()
{
}

LRESULT CWizardSpraySheet::OnHelp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	LPHELPINFO helpInfo = (LPHELPINFO)lParam;
	if(helpInfo)
	{
		if(helpInfo->dwContextId != 0)
		{
			// If dwContextId is set, then the control with
			// focus has a help context ID, so we'll show context help.
			m_data->ShowContextHelp(helpInfo);
		}
		else
		{
			int currentIndex = this->GetActiveIndex();
			if(currentIndex >= 0)
			{
				int pageDialogId = this->IndexToId(currentIndex);
				if(pageDialogId != 0)
				{
					m_data->ShowHelp(pageDialogId, helpInfo->iCtrlId);
				}
			}
		}
	}

	return 0;
}
