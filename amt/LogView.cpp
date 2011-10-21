#include "StdAfx.h"
#include "LogView.h"

CLogView * g_logView = NULL;

BOOL CLogView::PreTranslateMessage(MSG* pMsg)
{
	pMsg;
	return FALSE;
}

int CLogView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	SetMsgHandled(false);
	ATLASSERT(g_logView == NULL);
	g_logView = this;
	return 0;
}
void CLogView::OnDestroy()
{
	SetMsgHandled(false);
	ATLASSERT(g_logView != NULL);
	g_logView = NULL;
}

LRESULT CLogView::OnApp(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CTime t;
	t = t.GetCurrentTime();
	std::_tstring * newMsg = (std::_tstring *)wParam;
	ATLASSERT(newMsg);
	std::_tstring msg = t.Format(_T("%X - "));
	msg += *newMsg;
	delete newMsg;

	int sel = AddString(msg.c_str());
	SetCurSel(sel);
	return 0;
}

//  ===========================================================================
void LogMsg(const std::_tstring & msg)
{
	if(g_logView)
	{
		std::_tstring * newMsg = new std::_tstring();
		*newMsg = msg;
		g_logView->PostMessage(WM_APP, (WPARAM)newMsg);
	}
}
