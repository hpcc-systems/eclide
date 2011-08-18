#include "StdAfx.h"
#include "wutil.h"

bool FindDialogControlPlaceholder(HWND hwndDlg, int controlID, CRect &rc, DWORD &styles )
{
	return FindDialogControlPlaceholder(::GetDlgItem(hwndDlg, controlID), rc, styles);
}

bool FindDialogControlPlaceholder(HWND wndPlaceholder, CRect &rc, DWORD &styles )
{
	if (!wndPlaceholder)
	{
		return false;
	}
	else
	{
		CWindow wnd(wndPlaceholder);
		styles = (DWORD)::GetWindowLong(wndPlaceholder, GWL_STYLE);
		wnd.GetWindowRect(&rc);
		wnd.ScreenToClient(rc);
		wnd.DestroyWindow();
		return true;
	}
}

void CloneMenuState(const CMenu &from, CMenu &to)
{
	for(int n = 0; n < to.GetMenuItemCount(); n++)
	{
		UINT id = to.GetMenuItemID(n);

		// menu separator or invalid cmd - ignore it
		if (id == 0) 
			continue; 

		if (id == (UINT)-1)
		{
			// possibly a popup menu, route to child menu if so
			CMenuHandle sub = to.GetSubMenu(n);
			if(sub) 
			{
				CMenu mSub(sub);
				CloneMenuState( from, mSub );
				mSub.Detach(); //don't destroy the menu
			}
		}
		else 
		{
			UINT state = from.GetMenuState(id,MF_BYCOMMAND);
			if ( state != (0xFFFFFFFF) && (state & (MF_GRAYED|MF_DISABLED)) )
				to.EnableMenuItem(id, MF_GRAYED);
		}
	}
}
