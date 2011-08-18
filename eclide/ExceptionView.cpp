#include "StdAfx.h"
#include "..\en_us\resource.h"

#include "ExceptionView.h"

static const SectionLabelDefault GLOBAL_EXCEPTION_SHOWERRORS(SectionLabel(_T("Exception"), _T("Errors")), true);
static const SectionLabelDefault GLOBAL_EXCEPTION_SHOWWARNINGS(SectionLabel(_T("Exception"), _T("Warnings")), true);
static const SectionLabelDefault GLOBAL_EXCEPTION_SHOWINFORMATION(SectionLabel(_T("Exception"), _T("Information")), false);

CExceptionView::CExceptionView(Dali::IWorkunit * wu, IResultSlot *resultSlot) : m_wu(wu)
{
	m_resultSlot = resultSlot;
}

LRESULT CExceptionView::OnInitDialog(HWND /*hWnd*/, LPARAM /*lParam*/)
{
	SetMsgHandled(false);
	DlgResize_Init(false, true, 0);
	m_name = _T("Exceptions");
	SetWindowText(m_name);

	m_edit = GetDlgItem(IDC_EDIT_EXCEPTION);

	boost::filesystem::path iniPath;
	GetIniPath(iniPath);
	CComPtr<IConfig> iniFile = CreateIConfig(QUERYBUILDER_INI, iniPath);

	m_checkErrors = iniFile->Get(GLOBAL_EXCEPTION_SHOWERRORS);
	m_checkWarnings = iniFile->Get(GLOBAL_EXCEPTION_SHOWWARNINGS);
	m_checkInformation = iniFile->Get(GLOBAL_EXCEPTION_SHOWINFORMATION);

	DoDataExchange();

	PostMessage(CWM_INITIALIZE);
	return 0;
}

LRESULT CExceptionView::OnInitialize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	UpdateDisplay();
	return 0;
}

LRESULT CExceptionView::OnBnClickedCheckFilter(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	DoDataExchange(true);

	UpdateDisplay();

	boost::filesystem::path iniPath;
	GetIniPath(iniPath);
	CComPtr<IConfig> iniFile = CreateIConfig(QUERYBUILDER_INI, iniPath);

	iniFile->Set(GLOBAL_EXCEPTION_SHOWERRORS, m_checkErrors);
	iniFile->Set(GLOBAL_EXCEPTION_SHOWWARNINGS, m_checkWarnings);
	iniFile->Set(GLOBAL_EXCEPTION_SHOWINFORMATION, m_checkInformation);

	return 0;
}

void CExceptionView::OnContextMenu(HWND /*hWnd*/, CPoint pt)
{
	WTL::CMenu m;
	m.LoadMenu(IDR_POPUP_RESULTS_EXCEPTION);
	CResultContext context(this, m,pt);
	if ( !m_resultSlot || !m_resultSlot->OnContextResult(context) )
	{
		//if nobody handled it, then do it ourselves
		BOOL id = m.GetSubMenu(0).TrackPopupMenuEx(TPM_RETURNCMD, pt.x, pt.y, m_hWnd, NULL);
		if (id)
			PostMessage(WM_COMMAND, id);
	}
}

//CTabPane
HWND CExceptionView::GetHWND()
{
	return m_hWnd;
}

bool CExceptionView::CanCopy()
{
	return true;
}
bool CExceptionView::HasResults() const
{
	return m_edit.GetWindowTextLength() > 0;
}

//ISearchable
bool CExceptionView::CanBeSearched() const
{
	return HasResults();
}

bool CExceptionView::Search(const CSearchableInfo &/*info*/, CSearchableResult &/*result*/) const
{
	return false;
}

void CExceptionView::UpdateDisplay()
{
	m_exception = "";
	CComPtr<clib::scoped_lock_ref_counted> lock;
	m_wu->Lock(lock);

	unsigned int errorCount = 0;
	unsigned int warningCount = 0;
	unsigned int infoCount = 0;

	for(int i = 0; i < m_wu->GetExceptionCount(); ++i)
	{
		bool showMsg = false;
		const Dali::CEclException *pExcp = m_wu->GetException(i);
		if (boost::algorithm::iequals(_T("Error"), (const TCHAR *)pExcp->m_severity))
		{
			++errorCount;
			if (m_checkErrors)
				showMsg = true;
		}
		else if (boost::algorithm::iequals(_T("Warning"), (const TCHAR *)pExcp->m_severity))
		{
			++warningCount;
			if (m_checkWarnings)
				showMsg = true;
		}
		else if (boost::algorithm::iequals(_T("Info"), (const TCHAR *)pExcp->m_severity))
		{
			++infoCount;
			if (m_checkInformation)
				showMsg = true;
		}

		if (showMsg)
		{
			if (i > 0)
				m_exception += _T("\r\n");

			if (pExcp->m_fileName.IsEmpty() && pExcp->m_lineNo == 0 && pExcp->m_column == 0)
				m_exception += (boost::_tformat(_T("%1%: %2%")) % static_cast<const TCHAR *>(pExcp->m_severity) % static_cast<const TCHAR *>(pExcp->m_message)).str().c_str();
			else if (pExcp->m_fileName.IsEmpty())
				m_exception += (boost::_tformat(_T("%1%: %2% (%3%, %4%)")) % static_cast<const TCHAR *>(pExcp->m_severity) % static_cast<const TCHAR *>(pExcp->m_message) % pExcp->m_lineNo % pExcp->m_column).str().c_str();
			else
				m_exception += (boost::_tformat(_T("%1%: %2% (%3%, %4% - %5%)")) % static_cast<const TCHAR *>(pExcp->m_severity) % static_cast<const TCHAR *>(pExcp->m_message) % pExcp->m_lineNo % pExcp->m_column % static_cast<const TCHAR *>(pExcp->m_fileName)).str().c_str();
		}
	}

	::SetWindowText(GetDlgItem(IDC_CHECK_ERRORS), (boost::_tformat(_T("%1% Errors")) % errorCount).str().c_str());
	::SetWindowText(GetDlgItem(IDC_CHECK_WARNINGS), (boost::_tformat(_T("%1% Warnings")) % warningCount).str().c_str());
	::SetWindowText(GetDlgItem(IDC_CHECK_INFORMATION), (boost::_tformat(_T("%1% Information")) % infoCount).str().c_str());

	DoDataExchange();
}

//  --  IResultView  ---
HWND CExceptionView::GetHwnd() const 
{
	return m_hWnd;
}
