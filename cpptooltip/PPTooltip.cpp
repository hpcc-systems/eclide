//
//--- History ------------------------------ 
// 2004/03/01 *** Releases version 2.0 ***
//------------------------------------------
// 2004/04/04 [ADD] Added method SetCssStyles(DWORD dwIdCssStyle, LPCTSTR lpszPathDll /* = NULL */)
// 2004/04/14 [FIX] Fixed correct drawing for some tooltip's directions
// 2004/04/15 [FIX] Fixed changing a z-order of the some windows by show a tooltip on Win9x
// 2004/04/27 [FIX] Corrected a work with a tooltip's directions with a large tooltip
// 2004/04/28 [ADD] Disables a message translation if object was't created (thanks to Stoil Todorov)
// 2004/07/02 [UPD] Changes a GetWndFromPoint mechanism of the window's searching
// 2004/09/01 [ADD] New SetMaxTipWidth method was added
// 2004/10/12 [FIX] Now a tooltip has a different methods to show a menu's tooltip and other 
//					control's tooltip
////////////////////////////////////////////////////////////////////
//
// "SmoothMaskImage" and "GetPartialSums" functions by Denis Sarazhinsky (c)2003
// Modified by Eugene Pustovoyt to use with image's mask instead of full color image.
//
/////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PPTooltip.h"

/////////////////////////////////////////////////////////////////////
// CPPToolTipImpl::IsNotify()
//		This function determines will be send the notification messages from 
//	the control or not before display.
//-------------------------------------------------------------------
// Return value:
//	TRUE if the control will be notified the specified window
///////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//
//      +-----------------+    +-------------------+   +-----------------+  
//   +->|     Screen      +--->| m_hUnderTooltipBk |   |   m_hBitmapBk   |
//   |  +--------+--------+    +-------------------+   +--------+--------+
//   |           |                                            |
//   |  +--------V--------+                          +--------V--------+
//   |  |                 |     +--------------+     |                 |
//   |  |                 |     |   DrawHTML   |---->|                 |
//   |  |                 |     +--------------+     |                 |
//   |  |                 |                          |     MemDC       |
//   |  |                 |     +--------------+     |                 |
//   |  |                 |     | OnDrawBorder |---->|                 |
//   |  |     TempDC      |     +--------------+     +--------+--------+
//   |  |                 |                                   |         
//   |  |                 |     +--------------+              |         
//   |  |                 |<----+  DrawShadow  |              |         
//   |  |                 |     +--------------+              |         
//   |  |                 |                                   |         
//   |  |                 |<--------ALPHA---------------------+         
//   |  |                 |
//   |  +--------+--------+
//   |           |          
//   +-----------+
//
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// CPPToolTipImpl::GetTooltipDirection()
//		Gets a real direction of a tooltip.
//------------------------------------------------------------------
// Parameters:
//		dwDirection		- A default direction of a tooltip. 
//		lpPoint			- A mouse position in the screen coordinates.
//		lpAnchor		- An anchor position in the client coordinates
//      rcBody			- A rectangle of a tooltip's body in the client coordinates
//		rcFull			- A rectangle of a full tooltip in the client coordinates
//		rcTipArea		- A rectangle of a tooltip's info area in the client coordinates
// Return values:
//		A real direction of a tooltip
//------------------------------------------------------------------
// Explanation:
//    0
//  0 +------------------------------------+
//    |                                    |
//    |             rcBody                 |
//    |                                    |
//    |  +------------------------------+  |
//    |  |                              |  |
//    |  |         rcTipArea            |  |
//    |  |                              |  |
//    |  +------------------------------+  |
//    |                                    |
//    +--+...------------------------------+
//    :  |  /                              :
//    :  | /        rcFull                 :
//    :..|/................................:
//       +- lpAnchor
//
////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////
// *** public methods ***
////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// CPPToolTipImpl::AddTool()
// Registers a tool with the tooltip control.
//------------------------------------------------------------------
// Parameters:
//		pWnd			- Pointer to the window that contains the tool.  
//		lpszString		- Pointer to the text for the tool. 
//      dwIdString		- ID of string resource
//		hIcon			- Handle of the icon
//		dwIdIcon		- ID of icon resource
//		szIcon			- Specifies the width and the height, in pixels, of the icon to load.
//		lpRectBounds	- Pointer to a RECT structure containing coordinates of the tool's bounding rectangle. 
//						  The coordinates are relative to the upper-left corner of the client area of the window identified by pWnd. 
//					      NULL if bounding rectangle don't uses for specified window
//		dwIdTool		- ID of the tool
//		ti				- Reference to PPTOOLTIP_INFO structure containing the parameters of the tooltip 
//
// Remarks:
//		  A tooltip control can be associated with more than one tool. Call this function to register a tool 
//		with the tooltip control, so that the information stored in the tooltip is displayed when the cursor is on the tool.
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// CPPToolTipImpl::RemoveTool()
//   Removes the tool specified by pWnd and lpRectBounds from the collection of 
// tools supported by a tooltip control.
//------------------------------------------------------------------
// Parameters:
//		pWnd			- Pointer to the window that contains the tool.  
//		lpRectBounds	- Pointer to a RECT structure containing coordinates of the tool's bounding rectangle. 
//						  The coordinates are relative to the upper-left corner of the client area of the window identified by pWnd. 
//					      NULL if bounding rectangle don't uses for specified window
////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//// CPPToolTipImpl::RemoveAllTools()
////   Removes all tools from the collection of tools supported by a tooltip control.
//////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// CPPToolTipImpl::AddToolBar()
// Registers a toolbar to the tooltip control.
//------------------------------------------------------------------
// Parameters:
//		pBar			- Pointer to the toolbar window.  
////////////////////////////////////////////////////////////////////
//template< class T, class TBase, class TWinTraits >
//void CPPToolTipImpl<T, TBase, TWinTraits>::AddToolBar(CToolBar * pBar)
//{
//	ATLTRACE (_T("CPPToolTipImpl::AddToolBar(hWnd=0x%08X)\n"), pBar->GetSafeHwnd());
//	ATLASSERT(pBar);
//
//	//ENG: Gets HWND toolbar's window
//	//RUS: ѕолучаем HWND окна панели инструментов
//	HWND hWnd = pBar->GetSafeHwnd();
//
//	//ENG: Searching a clone of a toolbar
//	//RUS: ѕоиск дубликата указанной панели инструментов
//	arToolBarWnd::iterator iter;
//	for (iter = m_wndToolBars.begin(); iter != m_wndToolBars.end(); ++iter)
//	{
//		if (*iter == hWnd)
//		{
//			//ENG: A clone was found
//			//RUS: ƒубликат найден
//			return;
//		} //if
//	} //for
//
//	//ENG: Stores HWND toolbar's window
//	//RUS: «апоминаем HWND окна панели инструментов
//	m_wndToolBars.push_back(hWnd);
//
//	//ENG: Disables a standard tooltip for the specified toolbar
//	//RUS: «апрещаем стандартные подсказки дл€ указаной панели инструментов
//	DWORD dwStyles = pBar->GetBarStyle();
//	dwStyles &= ~CBRS_TOOLTIPS;
//	pBar->SetBarStyle(dwStyles);
//} //End of AddToolBar

////////////////////////////////////////////////////////////////////
// CPPToolTipImpl::EnableHyperlink()
// Enables redrawing hyperlinks and hot areas.
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// CPPToolTipImpl::SetCallbackHyperlink()
//   Sets the callback message that will be sent to the specified window 
// if user clicks a hyperlink or hotareas with a msg parameter.
//------------------------------------------------------------------
// Parameters:
//		hWnd			- Handle of the window that will receive the callback message.
//						  Pass NULL to remove any previously specified callback message.
//		nMessage		- Callback message to send to window.
//		lParam			- A 32 bits user specified value that will be passed to the callback function.
//
// Remarks:
//    The callback function must be in the form:
//  LRESULT On_MenuCallback(WPARAM wParam, LPARAM lParam)
//		wParam			- Pointer to the string specified as parameter in <a msg=> tag.
//		lParam			- The 32 bits user specified value.
////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// CPPToolTipImpl::SetNotify()
// This function sets or removes the notification messages from the control before display.
//-------------------------------------------------------------------
// Parameters:
//		bParentNotify	- If TRUE the tooltip will be send the notification to parent window
//						  Else the notification will not send
//		hWnd			- If non-NULL the tooltip will be send the notification to specified window
//						  Else the notification will not send
///////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
//  CPPToolTipImpl::SetSize()
//    Sets the specified size
//---------------------------------------------------------------------------
//  Parameters :
//		nSizeIndex		- index of the size. This parameter can be one 
//						  of the following values:
//							PPTTSZ_ROUNDED_CX - The width of the ellipse used 
//												to draw the rounded corners, in logical units.
//							PPTTSZ_ROUNDED_CY - The height of the ellipse used 
//												to draw the rounded corners, in logical units.
//							PPTTSZ_MARGIN_CX  - The left and right margins of the tooltip's 
//												text from the tooltip's edges. 
//							PPTTSZ_MARGIN_CY  - The top and bottom margins of the tooltip's 
//												text from the tooltip's edges.
//							PPTTSZ_WIDTH_ANCHOR - The width of the tooltip's anchor
//							PPTTSZ_HEIGHT_ANCHOR - The height of the tooltip's anchor 
//							PPTTSZ_MARGIN_ANCHOR - The margin of the tooltip's anchor from 
//												   his edge.
//							PPTTSZ_OFFSET_ANCHOR_CX - The horizontal offset of the tooltip's anchor
//													  from the hot spot of a cursor
//							PPTTSZ_OFFSET_ANCHOR_CY - The vertical offset of the tooltip's anchor
//													  from the hot spot of a cursor
//		nValue			- size's value
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//  CPPTootTip::GetSize()
//    Gets the specified size
//---------------------------------------------------------------------------
//  Parameters :
//		nSizeIndex		- An index of the sizes. See CPPToolTipImpl::SetSize for a 
//						  description of the valid values.
//  Returns :
//		size's value
//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//  CPPToolTipImpl::SetDefaultSizes()
//    Sets all sizes to default values
//---------------------------------------------------------------------------
//  Parameters:
//		bBalloonSize	- If TRUE all sizes will be sets for balloon tooltip
//						  otherwise tooltip will look as standard 
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CPPToolTipImpl::SetColorBk()
//   Sets background's colors 
//---------------------------------------------------------------------------
//  Parameters:
//		color			- A solid color for background's effect 
//		clrBegin		- A begin color for background's effect
//		clrMid			- A middle color for background's effect
//		clrEnd			- A end color for background's effect
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CPPToolTipImpl::SetEffectBk()
//   Sets a background's effect 
//---------------------------------------------------------------------------
//  Parameters:
//		dwEffect		- A background's effect 
//		nGranularity	- Adds an uniform noise to the effect. 
//						  A good value is from 5 to 20; 0 to disable the effect. 
//						  The noise has a positive effect because it hides the palette steps.
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CPPToolTipImpl::SetBehaviour()
//   Sets a tooltip's behaviour 
//---------------------------------------------------------------------------
//  Parameters:
//		dwBehaviour		- A tooltip's behaviour. 0 for normal tooltip without 
//						  specific behaviours. This parameter can be any combination 
//						  of CPPToolTip behaviours:
//							PPTOOLTIP_MULTIPLE_SHOW		- Multiple show for single control
//							PPTOOLTIP_TRACKING_MOUSE	- Tracking for mouse
//							PPTOOLTIP_CLOSE_LEAVEWND	- Close tooltip if mouse leave the control
//							PPTOOLTIP_NOCLOSE_OVER		- No close tooltip if mouse over him
//							PPTOOLTIP_DISABLE_AUTOPOP	- Disables autopop tooltip from timer
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CPPToolTipImpl::GetBehaviour()
//   Gets a tooltip's behaviour 
//---------------------------------------------------------------------------
// Return value:
//		A tooltip's behaviour. See CPPToolTipImpl::SetBehaviour for a description of the 
//	valid values.
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//  CPPToolTipImpl::SetDelayTime()
//   Call this function to set the delay time for a tooltip control. 
// The delay time is the length of time the cursor must remain on a tool 
// before the tooltip window appears. The default delay time is 500 milliseconds.
//-------------------------------------------------------------------
// Parameters:
//		dwDuration		- Flag that specifies which duration value will be retrieved. 
//						  This parameter can be one of the following values:
//							PPTOOLTIP_TIME_AUTOPOP  - Retrieve the length of time the tooltip 
//													  window remains visible if the pointer is 
//													  stationary within a tool's bounding rectangle. 
//							PPTOOLTIP_TIME_INITIAL  - Retrieve the length of time the pointer 
//			 										  must remain stationary within a tool's bounding 
//													  rectangle before the tool tip window appears. 
//							PPTOOLTIP_TIME_FADEIN	- Retrieve the length of time for each step of
//													  fade-in effect
//							PPTOOLTIP_TIME_FADEOUT	- Retrieve the length of time for each step of
//													  fade-out effect
//							PPTOOLTIP_TIME_ANIMATION  Retrieve the speed for the animation
//						  For compatibility with 1.x versions of CPPToolTip a following values
//						  are available also:
//							TTDT_AUTOPOP			- Same PPTOOLTIP_TIME_AUTOPOP 
//							TTDT_INITIAL			- Same PPTOOLTIP_TIME_INITIAL 
//		nTime			- The specified delay time, in milliseconds.
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
// CPPToolTipImpl::GetDelayTime()
// Retrieves the initial, pop-up, and reshow durations currently set 
// for a CPPToolTip control
//-------------------------------------------------------------------
// Parameters:
//		dwDuration		- Flag that specifies which duration value will be retrieved. 
//						  See CPPToolTipImpl::SetDelayTime for a description of the valid values. 
// Return value:
//	The specified delay time, in milliseconds
///////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//  CPPToolTipImpl::SetDirection()
//    Sets a placement of the tooltip's anchor
//---------------------------------------------------------------------------
//  Parameters :
//		dwDirection		- A placement of the tooltip's anchor. This parameter 
//					      can be one of the following values:
//							PPTOOLTIP_TOPEDGE_LEFT			- A left corner of the top edge
//							PPTOOLTIP_TOPEDGE_RIGHT			- A right corner of the top edge
//							PPTOOLTIP_TOPEDGE_CENTER		- By center of the top edge
//							PPTOOLTIP_BOTTOMEDGE_LEFT		- A left corner of the bottom edge
//							PPTOOLTIP_BOTTOMEDGE_RIGHT		- A right corner of the bottom edge
//							PPTOOLTIP_BOTTOMEDGE_CENTER		- By center of the bottom edge
//							PPTOOLTIP_LEFTEDGE_TOP			- A top corner of the left edge
//							PPTOOLTIP_LEFTEDGE_BOTTOM		- A bottom corner of the left edge
//							PPTOOLTIP_LEFTEDGE_VCENTER		- By center of the left edge
//							PPTOOLTIP_RIGHTEDGE_TOP			- A top corner of the right edge
//							PPTOOLTIP_RIGHTEDGE_BOTTOM		- A bottom corner of the right edge
//							PPTOOLTIP_RIGHTEDGE_VCENTER		- By center of the right edge
//						  For compatibility with 1.x versions of CPPToolTip a following values
//						  are available also:
//							PPTOOLTIP_LEFT_TOP				- Same PPTOOLTIP_TOPEDGE_LEFT
//							PPTOOLTIP_RIGHT_TOP				- Same PPTOOLTIP_TOPEDGE_RIGHT
//							PPTOOLTIP_LEFT_BOTTOM			- Same PPTOOLTIP_BOTTOMEDGE_LEFT
//							PPTOOLTIP_RIGHT_BOTTOM			- Same PPTOOLTIP_BOTTOMEDGE_RIGHT
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//  CPPToolTipImpl::GetDirection()
//    Gets a placement of the tooltip's anchor
//---------------------------------------------------------------------------
//  Returns :
//	  A placement of the tooltip's anchor. See CPPToolTipImpl::SetDirection for a description of 
//	the valid values.
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//  CPPToolTipImpl::SetTextStyles()
//    Applies a CSS-like style for the tooltip's HTML
//---------------------------------------------------------------------------
//  Parameters:
//		lpszStyleName	- Pointer to a null-terminated string that specifies
//						  a name of CSS style
//		lpszStyleValue  - Pointer to a null-terminated string that specifies 
//						  CSS-lite style for drawing a tooltip text.
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//  CPPToolTipImpl::SetCssStyles()
//    Applies a CSS-like styles for the tooltip's HTML
//---------------------------------------------------------------------------
//  Parameters:
//		lpszCssStyles	- Pointer to a null-terminated string that specifies 
//						  CSS-lite styles for drawing a tooltip text.
/////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////// 
//  CPPToolTipImpl::SetCssStyles() 
//    Applies a CSS-like styles for the tooltip's HTML 
//--------------------------------------------------------------------------- 
//  Parameters: 
//      dwIdCssStyle    - ID of string resource 
//      lpszPathDll		- 
///////////////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////////////////
//  CPPToolTipImpl::GetCssStyles()
//    Applies a CSS-like styles for the tooltip's HTML
//---------------------------------------------------------------------------
//  Return value:
//		Pointer to a null-terminated string that specifies CSS-lite styles 
//  for drawing a tooltip text.
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//  CPPToolTipImpl::SetDebugMode()
//    Sets a debug mode. In this mode tooltip will display for any control
// and will contain debug info about control.
//---------------------------------------------------------------------------
//  Parameters:
//		bDebug			- TRUE set a debug mode.
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// CPPToolTipImpl::ShowHelpTooltip()
// Shows the help tooltip in any place of screen.
//------------------------------------------------------------------
// Parameters:
//		pt				- Pointer to a POINT structure that receives the screen coordinates of the tooltip's anchor  
//		lpszString		- Pointer to the text for the help tooltip. 
//      dwIdString		- ID of string resource
//		hIcon			- Handle of the icon
//		dwIdIcon		- ID of icon resource
//		szIcon			- Specifies the width and the height, in pixels, of the icon to load.
//		ti				- Reference to PPTOOLTIP_INFO structure containing the parameters of the tooltip 
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// CPPToolTipImpl::SetBorder()
// Sets a border of the tooltip.
//------------------------------------------------------------------
// Parameters:
//		color			- Color of the tooltip's border
//		hbr				- Brush for drawing tooltip's border
//      nWidth			- A width of the brush
//		nHeight			- A height of the brush
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// CPPToolTipImpl::HideBorder()
// Hides border of the tooltip.
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Begin of the menu methods block. Build-in support for menu
#ifdef PPTOOLTIP_USE_MENU
//////////////////
// Need to handle WM_ENTERIDLE to cancel the tip if the user 
// moved the mouse off the popup menu. For main menus, Windows 
// will send a WM_MENUSELECT message for the parent menu when 
// this happens, but for context menus there's no other way to 
// know the user moved the mouse off the menu.
//
template< class T, class TBase, class TWinTraits >
void CPPToolTipImpl<T, TBase, TWinTraits>::OnEnterIdle(UINT nWhy, CWindow* pWho)
{
	if ((MSGF_MENU == nWhy))
	{
		if (m_nTooltipType == PPTOOLTIP_MENU)
		{
			if (PPTOOLTIP_STATE_SHOWN == m_nTooltipState)
			{
				CPoint pt;
				GetCursorPos(&pt);
				if (pWho->GetSafeHwnd() != ::WindowFromPoint(pt)) 
				{
					HideTooltip();
				} //if
			} //if
		} //if
	} //if
} //End of OnEnterIdle

template< class T, class TBase, class TWinTraits >
void CPPToolTipImpl<T, TBase, TWinTraits>::OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSubMenu)
{
	if (((nFlags & 0xFFFF) == 0xFFFF) || (nFlags & MF_POPUP) || (nFlags & MF_SEPARATOR))
	{
		//HideTooltip();
		Pop();
	} 
	else if (nItemID && hSubMenu) 
	{
		HWND hwndMenu = GetRunningMenuWnd(); //CWindow::WindowFromPoint(pt);
		if (NULL != hwndMenu)
		{
			CRect rcMenu;
			::GetWindowRect(hwndMenu, rcMenu); // whole menu rect
			
			// find Item Rectangle and position
			int count = ::GetMenuItemCount(hSubMenu);
			int cy = rcMenu.top + GetSystemMetrics(SM_CYEDGE) + 1;
			for(int nItem = 0; nItem < count; nItem++) 
			{
				CRect rect;
				::GetMenuItemRect(m_hParentWnd, hSubMenu, nItem, &rect);
				if(nItemID == ::GetMenuItemID(hSubMenu, nItem)) 
				{
					UINT nState = GetMenuState(hSubMenu, nItemID, MF_BYCOMMAND);
					CString str;
					if (MF_DISABLED & nState)
						str = m_drawer.GetResCommandPrompt(nItemID, 2); //String for disabled item
					else
						str = m_drawer.GetResCommandPrompt(nItemID, 0);

					CPoint pt;
					// found menu item: adjust rectangle to right and down
					pt.x = rcMenu.left;
					pt.y = cy;
					if (m_dwMenuToolPos & PPTOOLTIP_MENU_CENTER)
						pt.x += rect.Width() / 2;
					else if (m_dwMenuToolPos & PPTOOLTIP_MENU_RIGHT)
						pt.x += rect.Width();
					
					if (m_dwMenuToolPos & PPTOOLTIP_MENU_VCENTER)
						pt.y += rect.Height() / 2;
					else if (m_dwMenuToolPos & PPTOOLTIP_MENU_BOTTOM)
						pt.y += rect.Height();

					PPTOOLTIP_INFO ti;
					ti.rectBounds = rect;
					ti.nMask = 0;
					ti.sTooltip = str;
					m_nNextTooltipType = PPTOOLTIP_MENU;
					m_ptOriginal = pt;
					SetNewTooltip(hwndMenu, ti, TRUE, PPTOOLTIP_MENU);

					return;
				} //if
				cy += rect.Height(); // add height
			} //for
		} //if
		//ENG: Menu item was not found
		//RUS: Ёлемент меню не найден.
		Pop();
	} //if
} //End of OnMenuSelect

////////////////////////////////////////////////////////////////////
// CPPToolTipImpl::GetRunningMenuWnd()
// Get running menu window.
////////////////////////////////////////////////////////////////////
template< class T, class TBase, class TWinTraits >
HWND CPPToolTipImpl<T, TBase, TWinTraits>::GetRunningMenuWnd()
{
	HWND hwnd = NULL;
	EnumWindows(MyEnumProc,(LPARAM)&hwnd);
	return hwnd;
} //End of GetRunningMenuWnd

////////////////////////////////////////////////////////////////////
// CPPToolTipImpl::MenuToolPosition()
// Sets a position of the tooltip's anchor about menu item.
//------------------------------------------------------------------
// Parameters:
//		nPos			- A tooltip's position. This parameter can be any combination 
//						  of single horizontal value and single vertical value of CPPToolTip:
//							--- Horizontal position ---
//							PPTOOLTIP_MENU_LEFT		0x00
//							PPTOOLTIP_MENU_RIGHT	0x01
//							PPTOOLTIP_MENU_CENTER	0x02
//							--- Vertical position ---
//							PPTOOLTIP_MENU_TOP		0x00
//							PPTOOLTIP_MENU_BOTTOM	0x10
//							PPTOOLTIP_MENU_VCENTER  0x20
////////////////////////////////////////////////////////////////////
template< class T, class TBase, class TWinTraits >
void CPPToolTipImpl<T, TBase, TWinTraits>::MenuToolPosition(DWORD nPos /* = PPTOOLTIP_MENU_LEFT | PPTOOLTIP_MENU_TOP */)
{
	m_dwMenuToolPos = nPos;
} //End of MenuToolPosition

// End of menu methods block
///////////////////////////////////////////////////////////
#endif //PPTOOLTIP_USE_MENU

////////////////////////////////////////////////////////////////////
// CPPToolTipImpl::EnableEscapeSequences()
//		Enables the escape sequences. If the escape sequences was disabled
//	HTML-lite compiler will ignore the codes less then 0x20 (such \n, \r, \t).
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// CPPToolTipImpl::SetImageList()
//		Sets an image list for using it into the HTML string with <ilst> tag.
//------------------------------------------------------------------
// Parameters:
//		nIdBitmap		- Resource IDs of the bitmap to be associated with the image list.
//		hBitmap			- Handle of the bitmap to be associated with the image list.
//      cx				- Dimensions of each image, in pixels.
//		cy				- Dimensions of each image, in pixels.
//		nCount			- The number of images in the image list
//		crMask			- Color used to generate a mask. Each pixel of this color in 
//						  the specified bitmap is changed to transparent, and the 
//						  corresponding bit in the mask is set to one.
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// CPPToolTipImpl::SetTransparency()
//		Sets a transparency of the tooltip.
//------------------------------------------------------------------
// Parameters:
//		nTransparency	- A transparency value to be used on the tooltip. 
//						  The default 0 assumes that your tooltip is opaque and 0xFF (255) 
//						  for full transparency of the tooltip.
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// CPPToolTipImpl::SetTooltipShadow()
//		Sets a tooltip's shadow.
//------------------------------------------------------------------
// Parameters:
//		nOffsetX, 
//		nOffsetY		- The offsets of the tooltip's shadow from the tooltip's window.
//		nDarkenPercent	- So far as colors under the shadow will be darken (0 - 100)
//      bGradient		- TRUE to use a gradient shadow.
//		nDepthX,
//		nDepthY			- The gradient depths of the tooltip's shadow.
////////////////////////////////////////////////////////////////////

