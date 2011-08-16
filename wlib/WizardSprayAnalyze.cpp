#include "StdAfx.h"
#include "..\en_us\resource.h"

#include "WizardSprayAnalyze.h"
#include "Util.h"

CWizardSprayAnalyze::CWizardSprayAnalyze(CWizardSprayData *data, _U_STRINGorID title) : 
baseClass(title), 
m_data(data), 
m_analyzer(data->m_analyzerInfo),
m_bNext(false)
{
	baseClass::SetHeaderTitle(_T("Analyze File"));
	baseClass::SetHeaderSubTitle(_T("Auto analyze file"));
}

CWizardSprayAnalyze::~CWizardSprayAnalyze()
{
}


LRESULT CWizardSprayAnalyze::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CWaitCursor waitCursor;

	InitializeControls();
	InitializeValues();

	return 1;
}

LRESULT CWizardSprayAnalyze::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	// Be sure the base gets the message too
	bHandled = FALSE;

	UninitializeControls();	
	return 0;
}

void CWizardSprayAnalyze::InitializeControls(void)
{
	m_analyzeStatus = GetDlgItem(IDC_RICHEDIT2_STATUS);
}

void CWizardSprayAnalyze::UninitializeControls(void)
{
}

void CWizardSprayAnalyze::InitializeValues(void)
{
	//DoDataExchange();
}

bool CWizardSprayAnalyze::StoreValues(void)
{
	//DoDataExchange(true);
	return true;
}

// Overrides from base class
int CWizardSprayAnalyze::OnSetActive()
{
	if(m_bNext) // coming back from the next page
	{
		SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);
		m_bNext=false;
	}
	else // coming here from previous page
	{
		SetWizardButtons(0);
		LogMessage(_T("\r\n---- Analysis started ----\r\n"));
		PostMessage(UM_START_ANALYSIS);
	}

	// 0 = allow activate
	// -1 = go back to page that was active
	// page ID = jump to page
	return 0;
}

int CWizardSprayAnalyze::OnWizardNext()
{
	int ret=0;
	bool success = this->StoreValues();
	if(!success)
	{
		// Any errors are already reported, and if appropriate,
		// the control that needs attention has been given focus.
		ret=-1;
	}

	// 0  = goto next page
	// -1 = prevent page change
	// >0 = jump to page by dlg ID

	ret=m_data->FindNextPage(IDD);

	if(ret==0)
		m_bNext=true; // remember we went to the next page

	return ret;
}

int CWizardSprayAnalyze::OnWizardBack()
{
	return m_data->FindPreviousPage(IDD);
}

void CWizardSprayAnalyze::OnHelp()
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

LRESULT CWizardSprayAnalyze::OnStartAnalysis(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_analyzer.NewAnalysis();
	m_analyzer.m_fileSize = m_data->GetFileSize();
	m_data->m_analyzerInfo.Clear();
	std::_tstring filePath = _T("~file::");
	filePath += m_data->GetSource();
	m_data->m_analyzerInfo.SetFile(filePath.c_str());
	m_data->m_analyzerInfo.SetFileSize(m_data->GetFileSize());
	DoAnalyzeFile();
	return 0;
}

LRESULT CWizardSprayAnalyze::OnFileError(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	//just found a file error, so now we know more and can exec again with better info
	DoAnalyzeFile();
	return 0;
}

LRESULT CWizardSprayAnalyze::OnFileNotFoundError(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	MessageBox(_T("File Not Found. Please verify the file name."), _T(""));
	return 0;
}

LRESULT CWizardSprayAnalyze::OnWorkUnitCreated(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CString msg;
	msg = _T("Workunit submitted:  ");
	msg += m_workunit->GetWuid();
	msg += _T("\r\n");
	LogMessage(static_cast<const TCHAR *>(msg));
	return 0;
}

LRESULT CWizardSprayAnalyze::OnResultsComplete(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	DoAnalyzeFile();
	if (m_data->GetGrETLFlag())
		SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);
	else
		SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);
	return 0;
}

void CWizardSprayAnalyze::DoAnalyzeFile()
{
	CWaitCursor cursor;
	std::_tstring queue;
	m_data->GetQueue(queue);
	std::_tstring cluster;
	m_data->GetCluster(cluster);
	CString ecl;
	bool bNewEcl = m_analyzer.GetAnalyzerEcl(ecl, cluster.c_str(), m_workunit);

	//we are done with this old workunit, so delete it
	if ( m_workunit.get() )
	{
		WorkunitDelete(m_workunit);
	}

	if ( bNewEcl )
	{
		//TODO:  ::UpdateStatus(_T("Requesting Analysis"));
		LogMessage(_T("Submitting ECL\r\n"));
		m_nExceptionCount = 0;
		ExecEclNoRefCount(ecl, queue.c_str(),cluster.c_str());
	}
	else
	{
		m_workunit = 0;

		//save this so we don't have to recalc it every time
		m_knownFile = m_data->m_analyzerInfo.GetFile();
		m_knownFileSize = m_data->m_analyzerInfo.GetFileSize();

		LogMessage(_T("Analysis Complete\r\n"));
		CString msg;
		if ( m_data->m_analyzerInfo.HasBinaryData() )
		{
			msg.FormatMessage(_T("Results: Found Binary Data(unknown number of columns)\r\n"));
		}
		else if ( m_data->m_analyzerInfo.IsXmlFileFormat() )
		{
			msg.FormatMessage(_T("Results: XML File Type(columns=%1!d!)\r\n"), m_data->m_analyzerInfo.Columns());
		}
		else if ( m_data->m_analyzerInfo.IsFixedFileFormat() )
		{
			if ( m_data->m_analyzerInfo.Columns() <= 1 )
				msg.FormatMessage(_T("Results: Fixed File Type(unknown number of columns)\r\n"));
			else
				msg.FormatMessage(_T("Results: Fixed File Type(width=%1!d!, columns=%2!d!)\r\n"),
				m_data->m_analyzerInfo.GetFixedWidth(), m_data->m_analyzerInfo.Columns());
		}
		else
		{
			if ( m_data->m_analyzerInfo.IsCommaSeparated() )
				msg.FormatMessage(_T("Results: CSV File Type(columns=%1!d!)\r\n"), m_data->m_analyzerInfo.Columns() );
			else if ( m_data->m_analyzerInfo.IsTabSeparated() )
				msg.FormatMessage(_T("Results: Tab Separated File Type(columns=%1!d!)\r\n"), m_data->m_analyzerInfo.Columns() );
			else
				msg.FormatMessage(_T("Results: Unrecognized Variable Record File Type\r\n") );
			if ( m_data->m_analyzerInfo.BadRows() )
			{
				LogMessage(static_cast<const TCHAR *>(msg));
				msg.FormatMessage(_T("Results: Variable record file had %1!d! mismatched rows in the sample!\r\nData may not be reliable.\r\n"), m_data->m_analyzerInfo.Columns() );
			}

		}
		LogMessage(static_cast<const TCHAR *>(msg));
		LogMessage(_T("Analysis Complete."));
	}
}

//  CExecEcl override  ---
void CWizardSprayAnalyze::WorkunitUpdated(Dali::IWorkunit * workunit)
{
	//ignore everything but current request
	if ( workunit != m_workunit )
		return;

	if ( m_workunit->GetExceptionCount() != m_nExceptionCount)
	{
		ATLTRACE(_T("DataImport::Exceptions(%s - %s - exceptions=%d, results=%d)\n"), m_workunit->GetWuid(), m_workunit->GetStateLabel(), m_workunit->GetExceptionCount(), m_workunit->GetResultCount());
		if ( ProcessExceptions() )
		{
			//WorkunitDelete(m_workunit); //don't delete so they can go look at it in GAB
			m_workunit = 0; //done with this workunit
		}
	}

	if ( !m_workunit.get() )
	{
		ATLTRACE(_T("DataImport::Workunit is gone\n"));
		return;
	}

	if ( m_workunit->IsComplete() )
	{
		ATLTRACE(_T("DataImport::Complete(%s - %s - exceptions=%d, results=%d)\n"), m_workunit->GetWuid(), m_workunit->GetStateLabel(), m_workunit->GetExceptionCount(), m_workunit->GetResultCount());
		if( m_workunit->GetResultCount() >= 1 )
			PostMessage(UM_RESULTS_COMPLETE, 0);
	}
	else
	{
		ATLTRACE(_T("DataImport::Incomplete(%s - %s - exceptions=%d, results=%d)\n"), m_workunit->GetWuid(), m_workunit->GetStateLabel(), m_workunit->GetExceptionCount(), m_workunit->GetResultCount());
		CString msg( m_workunit->GetStateLabel() );
		msg += _T(", ");
		PostLogMessage(msg);
	}
}

//  CExecEcl override  ---
void CWizardSprayAnalyze::WorkunitCreated(Dali::IWorkunit * workunit)
{
	ATLTRACE(_T("CMultiResultView::WorkunitCreated(%s - %d)\n"), workunit->GetWuid(), workunit->GetState());
	m_workunit = workunit;
	PostMessage(UM_WORKUNIT_CREATED, TRUE);
}

bool CWizardSprayAnalyze::ProcessExceptions()
{
	bool bStopProcessing = false;
	for(int i = 0; i < m_workunit->GetExceptionCount(); ++i)
	{
		//skip the ones we have already seen
		if (i == m_nExceptionCount)
		{
			m_nExceptionCount++;
			const Dali::CEclException *pExcp = m_workunit->GetException(i);
			bStopProcessing |= ProcessErrorMsg(pExcp->m_message);
		}
	}
	return bStopProcessing;
}

bool CWizardSprayAnalyze::ProcessErrorMsg(const TCHAR *pMsg)
{
	CString msg( pMsg );
	msg += _T("\r\n");

	PostLogMessage(msg);
	ProcessFileSizeError(msg) || ProcessCSVLineError(msg) || ProcessFileNotFoundError(msg);

	SetWizardButtons(PSWIZB_BACK);
	return true; //stop on any error
}

bool CWizardSprayAnalyze::ProcessFileNotFoundError(CString& msg)
{
	const TCHAR error[] = _T("Could not open file ");
	int index = msg.Find(error);
	if ( index > 0 )
	{
		PostMessage(UM_FILE_NOTFOUND_ERROR);
		return true;
	}
	return false;
}

bool CWizardSprayAnalyze::ProcessFileSizeError(CString& msg)
{
	//look for the size complaint error!!!
	//Error: Error 1: File \\10.150.50.53:7100\c$\import\471z_despray_fixed.txt size is 693579 which is not a multiple of 1025
	int index = msg.Find(_T(" which is not a multiple of "));
	if ( index > 0)
	{
		//move backwards past the number
		int last = index;
		do
		{
			index--;
		}while ( index && msg[index] <= '9' && msg[index] >= '0');
		CString strFileSize = msg.Mid(index, last-index);
		__int64 fileSize;
		if ( ::StringToInt(strFileSize, fileSize) )
		{
			m_analyzer.m_fileSize = fileSize;
			PostMessage(UM_FILE_ERROR);
		}
		return true;
	}
	return false;
}

bool CWizardSprayAnalyze::ProcessCSVLineError(CString& msg)
{
	//file ... contained a line larger than the declared maximum (12 cf 10) 
	const TCHAR error[] = _T(" contained a line larger than the declared maximum (");
	int index = msg.Find(error);
	if ( index > 0 )
	{
		//pull record size out of the error message
		int recordSize;
		int start = index + _tcslen(error);
		int count=0;
		for (; msg[start+count] <= '9' && msg[start+count] >= '0'; count++)
		{
		}
		CString strRecordSize = msg.Mid(start, count);
		if ( ::StringToInt(strRecordSize, recordSize) )
		{
			m_data->m_analyzerInfo.SetMaxVarRecordSize(recordSize+1024); //add a bit, it's a max
			PostMessage(UM_FILE_ERROR);
		}
		return true;
	}
	return false;
}

void CWizardSprayAnalyze::PostLogMessage(const TCHAR* pStr)
{
	PostMessage(CWM_STATUSUPDATE, 0, LPARAM(new CString(pStr)));
}

LRESULT CWizardSprayAnalyze::OnLogMessage(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	CString *msg = reinterpret_cast<CString *>(lParam);
	if ( msg )
	{
		LogMessage(static_cast<const TCHAR *>(*msg));
		delete msg;
	}
	return 0;
}

void CWizardSprayAnalyze::LogMessage(const std::_tstring &msg)
{
	m_analyzeStatus.AppendText(msg.c_str());
}

