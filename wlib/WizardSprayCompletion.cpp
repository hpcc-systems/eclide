#include "StdAfx.h"
#include "..\en_us\resource.h"

#include "WizardSprayCompletion.h"

CWizardSprayCompletion::CWizardSprayCompletion(CWizardSprayData *data, _U_STRINGorID title) : baseClass(title)
{
	m_data = data;
}

CWizardSprayCompletion::~CWizardSprayCompletion()
{
}

LRESULT CWizardSprayCompletion::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	InitializeControls();
	//InitializeValues(); // redundant; called by UpdateSummary() when activated

	return 1;
}

void CWizardSprayCompletion::InitializeFont(void)
{
	// Set the font
	CLogFont logFont;
	CClientDC dc(m_hWnd);
	logFont.SetHeight(8, dc);
	::lstrcpy(logFont.lfFaceName, _T("Courier New"));

	m_fontSummary.Attach(logFont.CreateFontIndirect());
	m_editSummary.SetFont(m_fontSummary);

	// Set the tab stops to 4 characters.
	// Tab stops are in dialog units.
	TEXTMETRIC tm = {0};
	CFontHandle oldFont = dc.SelectFont(m_fontSummary);
	dc.GetTextMetrics(&tm);
	dc.SelectFont(oldFont);

	int dialogUnitsX = ::MulDiv(4, tm.tmAveCharWidth, LOWORD(GetDialogBaseUnits()));
	int tabStops = 4*dialogUnitsX;

// TJ	m_editSummary.SetTabStops(tabStops);
}

void CWizardSprayCompletion::InitializeControls(void)
{
	CFontHandle fontExteriorPageTitleFont(baseClass::GetExteriorPageTitleFont());

	CWindow title = GetDlgItem(IDC_WIZ97_EXTERIOR_TITLE);
	title.SetFont(fontExteriorPageTitleFont);

	m_editSummary = GetDlgItem(IDC_WIZ97_SUMMARY);

	InitializeFont();
}

void CWizardSprayCompletion::InitializeValues(void)
{
	m_editSummary.Clear();
	m_editSummary.SetWindowText(_T(""));
	m_editSummary.AppendText(_T("Source:  "));
	m_editSummary.AppendText(m_data->GetSource());
	m_editSummary.AppendText(_T("\r\n"));

	std::_tstring format;
	m_editSummary.AppendText(_T("Format:  "));
	m_editSummary.AppendText(m_data->m_analyzerInfo.GetEclFormat(format));
	m_editSummary.AppendText(_T("\r\n"));

	std::_tstring recordSummary;
	m_data->GetEclRecord(recordSummary);
	m_editSummary.AppendText(recordSummary.c_str());

	std::_tstring datasetSummary;
	m_data->GetEclDataset(datasetSummary);
	m_editSummary.AppendText(datasetSummary.c_str());

	std::_tstring spraySummary;
	m_data->GetEclSpray(spraySummary);
	m_editSummary.AppendText(spraySummary.c_str());
}

void CWizardSprayCompletion::UpdateSummary(void)
{
	InitializeValues();
	/*
	CString path = m_data->GetPath();
	bool recurse = m_data->GetRecurse();
	CString filter = m_data->GetFilter();
	TestWizardOutputType outputType = m_data->GetOutputType();

	CString text;
	text.Format(
	_T("Test Wizard: \r\n")
	_T("· Find files in the directory:\r\n")
	_T("\t%s\r\n")
	_T("· %s\r\n")
	_T("· Find files matching the filter '%s'\r\n"),
	path,
	recurse ? _T("Also search sub-directories") : _T("Only search that directory"),
	filter);
	m_editSummary.SetWindowText(text);

	CString outputDescription;
	switch(outputType)
	{
	case eOutput_SendEMail:
	outputDescription = 
	_T("· Send the file list in an e-mail\r\n")
	_T("  (using the default mail client)\r\n");
	break;
	case eOutput_SaveToFile:
	{
	CString outputFileName = m_data->GetOutputFileName();
	TestWizardOutputFileEncoding outputFileEncoding = m_data->GetOutputFileEncoding();

	outputDescription.Format(
	_T("· Save the file list to the file:\r\n")
	_T("\t%s\r\n"),
	outputFileName);
	switch(outputFileEncoding)
	{
	case eEncoding_ASCII:
	outputDescription += _T("  with ASCII encoding\r\n");
	break;
	case eEncoding_UCS2:
	outputDescription += _T("  with Unicode (UCS-2) encoding\r\n");
	break;
	case eEncoding_UTF8:
	outputDescription += _T("  with Unicode (UTF-8) encoding\r\n");
	break;
	}
	}
	break;
	case eOutput_Clipboard:
	default:
	outputDescription = _T("· Copy the file list to the clipboard\r\n");
	break;
	}
	m_editSummary.AppendText(outputDescription);
	*/
}

int CWizardSprayCompletion::OnSetActive()
{
	SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);

	// Don't remember any previous updates to the summary,
	//  and just regenerate the whole summary
	UpdateSummary();

	// 0 = allow activate
	// -1 = go back to page that was active
	// page ID = jump to page
	return 0;
}

int CWizardSprayCompletion::OnWizardBack()
{
	// 0  = goto previous page
	// -1 = prevent page change
	// >0 = jump to page by dlg ID
	return m_data->FindPreviousPage(IDD);
}

INT_PTR CWizardSprayCompletion::OnWizardFinish()
{
	// We could either do the work here, or in the place that
	// called DoModal on our Sheet (which for this example is CTestWizard).
	// The advantage of doing the work here is that we can prevent
	// the finish, and tell the user to go back and correct something.
	// The advantage of doing it in the caller of DoModal is
	// that the wizard isn't visible while the work is being done.

	// For this example, we'll do the work here (or rather call back into
	// the info class to do the work), and prevent finish if something fails.

	CWaitCursor waitCursor;

	bool success = m_data->FinishWizard(m_hWnd);
	return success ? FALSE : TRUE;
}

void CWizardSprayCompletion::OnHelp()
{
	m_data->ShowHelp(IDD);
}

