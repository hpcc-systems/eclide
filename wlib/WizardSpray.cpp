#include "StdAfx.h"
#include "..\en_us\resource.h"

#include "WizardSpray.h"
#include "WizardSpraySheet.h"
#include "InvokeSprayWizard.h"

bool InvokeSprayWizard(std::_tstring& transformLabel, bool gretlFlag)
{
	CWizardSpray wizardSpray;
	return wizardSpray.ExecuteWizard(transformLabel, gretlFlag);
}

CWizardSpray::CWizardSpray()
{
}

CWizardSpray::~CWizardSpray()
{
}

bool CWizardSpray::ExecuteWizard(std::_tstring & ecl, bool gretlFlag)
{
	// You could also pass in parameters here
	// (or something generic like a name/value map).
	m_data.SetGrETLFlag(gretlFlag);

	bool success = false;

	InitializeDefaultValues();

	CWizardSpraySheet wizard(&m_data, gretlFlag);
	INT_PTR result = wizard.DoModal();
	if(result == IDOK)
	{
		// You could either do the work here, or in
		// OnWizardFinish in the completion page class.
		success = true;
		//std::_tstring label;
		//m_data.GetEclAttributeLabel(CWizardSprayData::ATTRIBUTELABEL_TRANSFORM, true, label);
		//transformLabel = label;
		//std::_tstring ecl;
		m_data.GetSprayBuilderEcl(ecl);

		StoreDefaultValues();
	}

	return success;
}

void CWizardSpray::InitializeDefaultValues()
{
}

void CWizardSpray::StoreDefaultValues()
{
}


