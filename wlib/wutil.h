#pragma once

#include "wlib.h"

bool WLIB_API FindDialogControlPlaceholder(HWND hwndDlg, int controlID, CRect &rc, DWORD &styles);
bool WLIB_API FindDialogControlPlaceholder(HWND hwnd, CRect &rc, DWORD &styles);
WLIB_API void CloneMenuState(const WTL::CMenu &from, WTL::CMenu &to);

class WLIB_API CWindowNoRedraw
{
public:
   CWindowNoRedraw(HWND hWnd)
   {
	  m_hWnd = hWnd;
      ATLASSERT(::IsWindow(m_hWnd));
      ::SendMessage(m_hWnd,WM_SETREDRAW,0,0);
   }
   ~CWindowNoRedraw()
   {
      ::SendMessage(m_hWnd,WM_SETREDRAW,1,0);
	  ::InvalidateRect(m_hWnd,0,1);
   }
protected:
	HWND m_hWnd;
};


