#include "StdAfx.h"
#include "..\en_us\resource.h"

#include "WizardSprayTarget.h"
#include "Util.h"
#include "repository.h"
#include "attributeDlg.h"

CWizardSprayTarget::CWizardSprayTarget(CWizardSprayData *data, _U_STRINGorID title) : 
baseClass(title), 
m_data(data), 
m_analyzer(data->m_analyzerInfo)
{
	m_analyzeCombo = new CComboCluster();
	m_cluster = _T("hthor");
	m_data->SetCluster(m_cluster.GetString());

	baseClass::SetHeaderTitle(_T("Target Specification"));
	baseClass::SetHeaderSubTitle(_T("Select target record structure"));
}

CWizardSprayTarget::~CWizardSprayTarget()
{
}


LRESULT CWizardSprayTarget::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CWaitCursor waitCursor;

	InitializeControls();
	InitializeValues();

	return 1;
}

LRESULT CWizardSprayTarget::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	// Be sure the base gets the message too
	bHandled = FALSE;

	UninitializeControls();	
	return 0;
}

void CWizardSprayTarget::InitializeControls(void)
{
	m_analyzeCombo->Attach(GetDlgItem(IDC_COMBO_CLUSTER));
	m_targetRecordButton = GetDlgItem(IDC_BUTTON_TARGET);
}

void CWizardSprayTarget::UninitializeControls(void)
{
}

void CWizardSprayTarget::InitializeValues(void)
{
	std::_tstring filename;
	m_data->GetSourceFilename(filename);
	std::_tstring destination;
	m_destination = m_data->GetDestination(destination);
	if (!m_destination.GetLength())
	{
		std::_tstring filename;
		m_data->GetSourceFilename(filename);
		m_destination = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_USER);
		m_destination += _T("::");
		m_destination += filename.c_str();
	}
	std::_tstring module;
	m_module = m_data->GetModule(module);
	if (!m_module.GetLength())
	{
		std::_tstring filename;
		m_data->GetSourceFilename(filename);
		m_module = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_USER);
	}
	std::_tstring prefix;
	m_prefix = m_data->GetPrefix(prefix);
	if (!m_prefix.GetLength())
	{
		m_prefix = filename.c_str();
	}

	m_analyzeCombo->Load();
	m_analyzeCombo->SelectDefault(m_analyzeCombo->m_value);

	std::_tstring targetRecord;
	m_targetRecord = m_data->GetTarget(targetRecord);

	DoDataExchange();
}

bool CWizardSprayTarget::StoreValues(void)
{
	DoDataExchange(true);
	m_data->SetDestination(m_destination.GetString());
	m_data->SetModule(m_module.GetString());
	m_data->SetPrefix(m_prefix.GetString());
	m_data->SetCluster(m_cluster.GetString());
	m_data->SetTarget(m_targetRecord.GetString());
	return true;
}

// Overrides from base class
int CWizardSprayTarget::OnSetActive()
{
	SetWizardButtons(PSWIZB_BACK | (m_targetRecord.IsEmpty()? 0 : PSWIZB_NEXT));

	// 0 = allow activate
	// -1 = go back to page that was active
	// page ID = jump to page
	return 0;
}

int CWizardSprayTarget::OnWizardNext()
{
	bool success = this->StoreValues();
	if(!success)
	{
		// Any errors are already reported, and if appropriate,
		// the control that needs attention has been given focus.
		return -1;
	}

	// 0  = goto next page
	// -1 = prevent page change
	// >0 = jump to page by dlg ID

	return m_data->FindNextPage(IDD);
}

int CWizardSprayTarget::OnWizardBack()
{
	return m_data->FindPreviousPage(IDD);
}

void CWizardSprayTarget::OnHelp()
{
	// NOTE: Several controls on this dialog have been given
	//  context sensitive help descriptions, and the HtmlHelp
	//  file is setup to recognize their help IDs.  Please
	//  look at resource.hm, help\Context.h, help\Context.txt
	//  and the help project Wizard97Test.hhp.
	//
	// It's also important to note that context help doesn't
	//  come through this route, but rather goes to the page and
	//  then the sheet (if not handled) as WM_HELP with dwContextId
	//  in the HELPINFO structure set to a non-zero value.
	//  See the sheet for how it deals with context help.
	//  We get to this point if the user clicks on the help button
	//  at the bottom.

	m_data->ShowHelp(IDD);
}

LRESULT CWizardSprayTarget::OnBnClickedTargetRecord(WORD /*wNotifyCode*/, WORD /*wID*/, HWND  /*hWndCtl*/, BOOL& /*bHandled*/)
{
	DoDataExchange(true);
	CComPtr<IRepository> rep = AttachRepository();
	CComPtr<IAttribute> attr = rep->GetAttribute(m_targetRecord);
	if (DoGetAttr(rep, attr) == IDOK)
	{
		m_targetRecord = attr->GetQualifiedLabel();
	}
	DoDataExchange();
	SetWizardButtons(PSWIZB_BACK | (m_targetRecord.IsEmpty()? 0 : PSWIZB_NEXT));

	return 0;
}

//  ===========================================================================
CWizardSprayTargetQB::CWizardSprayTargetQB(CWizardSprayData *data, _U_STRINGorID title) : 
baseClass(title), 
m_data(data), 
m_analyzer(data->m_analyzerInfo)
{
	m_analyzeCombo = new CComboCluster();
	m_cluster = _T("hthor");
	m_data->SetCluster(m_cluster.GetString());

	baseClass::SetHeaderTitle(_T("Target Specification"));
	baseClass::SetHeaderSubTitle(_T("Select target record structure"));
}

CWizardSprayTargetQB::~CWizardSprayTargetQB()
{
}


LRESULT CWizardSprayTargetQB::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CWaitCursor waitCursor;

	InitializeControls();
	InitializeValues();

	return 1;
}

LRESULT CWizardSprayTargetQB::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	// Be sure the base gets the message too
	bHandled = FALSE;

	UninitializeControls();	
	return 0;
}

void CWizardSprayTargetQB::InitializeControls(void)
{
	m_analyzeCombo->Attach(GetDlgItem(IDC_COMBO_CLUSTER));
}

void CWizardSprayTargetQB::UninitializeControls(void)
{
}

void CWizardSprayTargetQB::InitializeValues(void)
{
	std::_tstring filename;
	m_data->GetSourceFilename(filename);
	std::_tstring destination;
	m_destination = m_data->GetDestination(destination);
	if (!m_destination.GetLength())
	{
		std::_tstring filename;
		m_data->GetSourceFilename(filename);
		m_destination = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_USER);
		m_destination += _T("::");
		m_destination += filename.c_str();
	}
	m_analyzeCombo->Load();
	m_analyzeCombo->SelectDefault(m_analyzeCombo->m_value);

	DoDataExchange();
}

bool CWizardSprayTargetQB::StoreValues(void)
{
	DoDataExchange(true);
	m_data->SetDestination(m_destination.GetString());
	m_data->SetCluster(m_cluster.GetString());
	return true;
}

// Overrides from base class
int CWizardSprayTargetQB::OnSetActive()
{
	SetWizardButtons(PSWIZB_BACK | (m_destination.IsEmpty()? 0 : PSWIZB_NEXT));

	// 0 = allow activate
	// -1 = go back to page that was active
	// page ID = jump to page
	return 0;
}

int CWizardSprayTargetQB::OnWizardNext()
{
	bool success = this->StoreValues();
	if(!success)
	{
		// Any errors are already reported, and if appropriate,
		// the control that needs attention has been given focus.
		return -1;
	}

	// 0  = goto next page
	// -1 = prevent page change
	// >0 = jump to page by dlg ID

	return m_data->FindNextPage(IDD);
}

int CWizardSprayTargetQB::OnWizardBack()
{
	return m_data->FindPreviousPage(IDD);
}

void CWizardSprayTargetQB::OnHelp()
{
	// NOTE: Several controls on this dialog have been given
	//  context sensitive help descriptions, and the HtmlHelp
	//  file is setup to recognize their help IDs.  Please
	//  look at resource.hm, help\Context.h, help\Context.txt
	//  and the help project Wizard97Test.hhp.
	//
	// It's also important to note that context help doesn't
	//  come through this route, but rather goes to the page and
	//  then the sheet (if not handled) as WM_HELP with dwContextId
	//  in the HELPINFO structure set to a non-zero value.
	//  See the sheet for how it deals with context help.
	//  We get to this point if the user clicks on the help button
	//  at the bottom.

	m_data->ShowHelp(IDD);
}

