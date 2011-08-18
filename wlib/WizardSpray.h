#pragma once

#include "WizardSprayData.h"

class CWizardSpray
{
protected:
	CWizardSprayData m_data;

public:
	CWizardSpray();
	~CWizardSpray();

	bool ExecuteWizard(std::_tstring & transformLabel, bool GrETL = true);

	void InitializeDefaultValues();
	void StoreDefaultValues();
};
