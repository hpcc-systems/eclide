#include "stdafx.h"
#include "resource.h"

#include "MainFrm.h"
#include "MenuManager.h"
#include "global.h"

CDockedFrame::CDockedFrame(CMenuManager * pMenuManager)
{
	m_MenuManager = pMenuManager;
}

void CDockedFrame::OnSetFocus(HWND phWnd)
{
	// Set focus to last known control
	if( !::IsWindow(m_hWndFocus) || !IsChild(m_hWndFocus) ) 
	{
		SetMsgHandled(false);
		return;
	}
	::SetFocus(m_hWndFocus);
}
void CDockedFrame::OnKillFocus(HWND phWnd)
{
	HWND hWndFocus = ::GetFocus();
	if( hWndFocus!=NULL && IsChild(hWndFocus) ) 
		m_hWndFocus = hWndFocus;
}

