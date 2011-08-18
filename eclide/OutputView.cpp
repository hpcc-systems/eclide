#include "stdafx.h"
#include "..\en_us\resource.h"

#include "OutputView.h"
#include "global.h"
#include "Util.h"

void CMyEdit::OnContextMenu(HWND phWnd, CPoint pPoint)
{
	WTL::CMenu m;
	m.LoadMenu(IDR_POPUP_ERRORLOG);
	m.EnableMenuItem(ID_EDIT_COPY, !CanCopy());
	unsigned int id = m.GetSubMenu(0).TrackPopupMenuEx(TPM_RETURNCMD, pPoint.x, pPoint.y, m_hWnd, NULL);
	switch (id)
	{
	case ID_EDIT_COPY:
		Copy();
		break;
	case ID_EDIT_CLEARALL:
		SetWindowText(_T(""));
		break;
	case ID_EDIT_SELECT_ALL:
		SetSelAll(true);
		break;
	}
}

CStreamEditorView::CStreamEditorView() : m_EditStrBuf(m_editLog), m_EditStrBufW(m_editLog)
{
}

CStreamEditorView::~CStreamEditorView()
{
}

BOOL CStreamEditorView::PreTranslateMessage(MSG* pMsg)
{
	return IsDialogMessage(pMsg);
}

LRESULT CStreamEditorView::OnInitDialog(HWND /*hWnd*/, LPARAM /*lParam*/)
{
	SetMsgHandled(false);
	DlgResize_Init(false,false);

	m_edit.SubclassWindow(GetDlgItem(IDC_EDIT_OUTPUT));
	m_editLog.SetEditCtrl(m_edit);
	initStream();

	return 0;
}

void CStreamEditorView::OnClose()
{
	SetMsgHandled(false);
	killStream();
}

void CStreamEditorView::OnContextMenu(HWND /*phWnd*/, CPoint /*pt*/)
{
	SetMsgHandled(false);
}

LRESULT CStreamEditorView::OnClear(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	m_edit.SetWindowText(_T(""));
	return 0;
}

LRESULT CStreamEditorView::OnReset(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	m_edit.SetWindowText(_T(""));
	return 0;
}

void CStreamEditorView::AddText(const TCHAR* text, bool bClear)
{
	if ( bClear )
		m_edit.SetWindowText(_T(""));

	m_editLog.AddText(text);
}

CErrorView::CErrorView() : CStreamEditorView()
{
}

void CErrorView::initStream()
{
	m_pOldBuf = std::cerr.rdbuf(&m_EditStrBuf);
	m_pOldBufW = std::wcerr.rdbuf(&m_EditStrBufW);
}

void CErrorView::killStream()
{
	std::cerr.rdbuf( m_pOldBuf );
	std::wcerr.rdbuf( m_pOldBufW );
}
