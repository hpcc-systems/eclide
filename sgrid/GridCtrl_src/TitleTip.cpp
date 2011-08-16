////////////////////////////////////////////////////////////////////////////
// TitleTip.cpp : implementation file
//
// Based on code by Zafir Anjum
//
// Adapted by Chris Maunder <cmaunder@mail.com>
// Copyright (c) 1998-2002. All Rights Reserved.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name and all copyright 
// notices remains intact. 
//
// An email letting me know how you are using it would be nice as well. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability for any damage/loss of business that
// this product may cause.
//
// For use with CGridCtrl v2.20+
//
// History
//         10 Apr 1999  Now accepts a LOGFONT pointer and 
//                      a tracking rect in Show(...)  (Chris Maunder)
//         18 Apr 1999  Resource leak in Show fixed by Daniel Gehriger
//          8 Mar 2000  Added double-click fix found on codeguru
//                      web site but forgot / can't find who contributed it
//         28 Mar 2000  Aqiruse (marked with //FNA)
//                      Titletips now use cell color
//         18 Jun 2000  Delayed window creation added
//
/////////////////////////////////////////////////////////////////////////////
 
#include "stdafx.h"
#include "gridctrl.h"

#ifndef GRIDCONTROL_NO_TITLETIPS

#include "TitleTip.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTitleTip

CTitleTip::CTitleTip()
{
	// Register the window class if it has not already been registered.
	WNDCLASS wndcls;
	HINSTANCE hInst = AfxGetInstanceHandle();
	if(!(::GetClassInfo(hInst, TITLETIP_CLASSNAME, &wndcls)))
	{
		// otherwise we need to register a new class
		wndcls.style			= CS_SAVEBITS;
		wndcls.lpfnWndProc		= ::DefWindowProc;
		wndcls.cbClsExtra		= wndcls.cbWndExtra = 0;
		wndcls.hInstance		= hInst;
		wndcls.hIcon			= NULL;
		wndcls.hCursor			= LoadCursor( hInst, IDC_ARROW );
		wndcls.hbrBackground	= (HBRUSH)(COLOR_INFOBK +1);
		wndcls.lpszMenuName		= NULL;
		wndcls.lpszClassName	= TITLETIP_CLASSNAME;

		if (!AfxRegisterClass(&wndcls))
			AfxThrowResourceException();
	}

    m_dwLastLButtonDown = ULONG_MAX;
    m_dwDblClickMsecs   = GetDoubleClickTime();
    m_bCreated          = FALSE;
    m_pParentWnd        = NULL;
}

CTitleTip::~CTitleTip()
{
}


BEGIN_MESSAGE_MAP(CTitleTip, CWnd)
	//{{AFX_MSG_MAP(CTitleTip)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_MBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_SYSKEYDOWN()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTitleTip message handlers

BOOL CTitleTip::Create(CWnd * pParentWnd)
{
	ASSERT_VALID(pParentWnd);

    // Already created?
    if (m_bCreated)
        return TRUE;

	DWORD dwStyle = WS_BORDER | WS_POPUP; 
	DWORD dwExStyle = WS_EX_TOOLWINDOW | WS_EX_TOPMOST;
	m_pParentWnd = pParentWnd;

	m_bCreated = CreateEx(dwExStyle, TITLETIP_CLASSNAME, NULL, dwStyle, 
                          CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
		                  NULL, NULL, NULL );

    return m_bCreated;
}

BOOL CTitleTip::DestroyWindow() 
{
    m_bCreated = FALSE;
	
	return CWnd::DestroyWindow();
}

// Show 		 - Show the titletip if needed
// rectTitle	 - The rectangle within which the original 
//				    title is constrained - in client coordinates
// lpszTitleText - The text to be displayed
// xoffset		 - Number of pixel that the text is offset from
//				   left border of the cell
void CTitleTip::Show(CRect rectTitle, LPCTSTR lpszTitleText, int xoffset /*=0*/,
					 LPRECT lpHoverRect /*=NULL*/,
					 const LOGFONT* lpLogFont /*=NULL*/,
					 COLORREF crTextClr /* CLR_DEFAULT */,
					 COLORREF crBackClr /* CLR_DEFAULT */)
{
	if (!IsWindow(m_hWnd))
		Create(m_pParentWnd);

	ASSERT( ::IsWindow( GetSafeHwnd() ) );

	if (rectTitle.IsRectEmpty())
		return;

	// If titletip is already displayed, don't do anything.
	if( IsWindowVisible() ) 
		return;

	m_rectHover = (lpHoverRect != NULL)? lpHoverRect : rectTitle;
	m_rectHover.right++; m_rectHover.bottom++;

	m_pParentWnd->ClientToScreen( m_rectHover );
	ScreenToClient( m_rectHover );

	// Do not display the titletip is app does not have focus
	if( GetFocus() == NULL )
		return;

	// Define the rectangle outside which the titletip will be hidden.
	// We add a buffer of one pixel around the rectangle
	m_rectTitle.top    = -1;
	m_rectTitle.left   = -xoffset-1;
	m_rectTitle.right  = rectTitle.Width()-xoffset;
	m_rectTitle.bottom = rectTitle.Height()+1;

	// Determine the width of the text
	m_pParentWnd->ClientToScreen( rectTitle );

	CClientDC dc(this);
	m_strTitle = _T("");
	m_strTitle += _T(" ");
	m_strTitle += lpszTitleText; 
	m_strTitle += _T(" ");

	m_lpLogFont = const_cast<LOGFONT *>(lpLogFont);
	CFont font, *pOldFont = NULL;
	if (lpLogFont)
	{
		font.CreateFontIndirect(lpLogFont);
		pOldFont = dc.SelectObject( &font );
	}
	else
	{
		// use same font as ctrl
		pOldFont = dc.SelectObject( m_pParentWnd->GetFont() );
	}

	CSize size = dc.GetTextExtent( m_strTitle );

	TEXTMETRIC tm;
	dc.GetTextMetrics(&tm);
	size.cx += tm.tmOverhang;

	m_rectDisplay = rectTitle;
	//m_rectDisplay.left += xoffset-1;
	m_rectDisplay.top  += 2;
	m_rectDisplay.right = m_rectDisplay.left + size.cx + xoffset;
	m_rectDisplay.bottom = m_rectDisplay.top + size.cy;

	// Do not display if the text fits within available space
	ATLTRACE(_T("TitleTip:  %i <= %i\n"), m_rectDisplay.right, rectTitle.right-xoffset);
	if ( m_rectDisplay.right <= rectTitle.right-xoffset )
	{
		dc.SelectObject( pOldFont );
		return;
	}

	// We will use avg char width to set max tooltip width
	int nMaxTooltipWidth = -1;
	//   if (nMaxChars > 0)
	//   {
	//    int nMaxTooltipWidth = (tm.tmAveCharWidth * nMaxChars);
	//	if (nMaxTooltipWidth < 0)
	//		nMaxTooltipWidth *= -1;

	//    // Rect display to be set to max chars
	//    if (m_rectDisplay.Width() > nMaxTooltipWidth)
	//           m_rectDisplay.right = m_rectDisplay.left + nMaxTooltipWidth;
	//}

	//***************************************************************************************
	//Adjust the dimensions of the rect to fit within the client

	// Get the coordinates of the parents client area. (In this case the ListView's client
	// area) and convert coordinates to those of the tooltip.
	CRect rectClient;
	m_pParentWnd->GetClientRect( rectClient );
	m_pParentWnd->ClientToScreen( rectClient );


	// ------------------------------------------------------------------------------
	// Use the screen's right edge as the right hand border, not the right edge of the client.
	// You can comment this out to use the right client as the border.
	CWindowDC wdc(NULL);
	rectClient.right = GetDeviceCaps(wdc, HORZRES) - 8;
	rectClient.bottom = GetDeviceCaps(wdc, VERTRES) - 8;
	//---------------------------------------------------------------------------------------


	//If the right edge exceeds the right edge of the client:
	//          see how much room there is to move the display to the left and adjust the
	//          rectangle that far to the left. If the rect still exceeds the right edge, clip
	//          the right edge to match the client right edge.
	//
	// Does the right display edge exceed the right client edge?
	if (m_rectDisplay.right > rectClient.right)
	{
		// establish what is available left shift wise and what is needed
		int nAvail = 0;
		int nNeeded = m_rectDisplay.right - rectClient.right;

		if (m_rectDisplay.left > rectClient.left)
			nAvail = m_rectDisplay.left - rectClient.left;

		// is there room to move left?
		if (nAvail >= nNeeded)
		{
			m_rectDisplay.OffsetRect(-nNeeded,0);  // yes, move all that is needed
			// increase the size of the window that will be inspected to see if the 
			// cursor has gone outside of the tooltip area by the number of units we
			// offset our display rect.
			m_rectTitle.right += nNeeded;
		}
		else
		{
			m_rectDisplay.OffsetRect(-nAvail,0);   // no, at least move to left edge of client
			// increase the size of the window that will be inspected to see if the 
			// cursor has gone outside of the tooltip area by the number of units we
			// offset our display rect.
			m_rectTitle.right += nAvail;
		}

		// Did we move enough? If not, clip right edge to match client right edge
		if (m_rectDisplay.right > rectClient.right)
			m_rectDisplay.right = rectClient.right;
	}


	//If the left edge exceeds the left edge of the client:
	//          see how much room there is to move the display to the right and adjust the
	//          rectangle that far to the right. If the rect still exceeds the left edge, clip
	//          the left edge to match the client left edge.
	//
	// Does the left display edge exceed the left client edge?
	if (m_rectDisplay.left < rectClient.left)
	{
		// establish what is available right shift wise and what is needed
		int nAvail = 0;
		int nNeeded = rectClient.left - m_rectDisplay.left;

		if (m_rectDisplay.right < rectClient.right)
			nAvail = rectClient.right - m_rectDisplay.right;

		// is there room to move left?
		if (nAvail >= nNeeded)
		{
			m_rectDisplay.OffsetRect(+nNeeded,0);  // yes, move all that is needed
			// increase the size of the window that will be inspected to see if the 
			// cursor has gone outside of the tooltip area by the number of units we
			// offset our display rect.
			m_rectTitle.left -= nNeeded;
		}
		else
		{
			m_rectDisplay.OffsetRect(+nAvail,0);   // no, at least move to left edge of client
			// increase the size of the window that will be inspected to see if the 
			// cursor has gone outside of the tooltip area by the number of units we
			// offset our display rect.
			m_rectTitle.left -= nAvail;
		}

		// Did we move enough? If not, clip left edge to match client left edge
		if (m_rectDisplay.left < rectClient.left)
			m_rectDisplay.left = rectClient.left;        
	}


	// if the calculated width > maxwidth set above then truncate 
	if (nMaxTooltipWidth > 0 && m_rectDisplay.Width() > nMaxTooltipWidth)
		m_rectDisplay.right = m_rectDisplay.left + nMaxTooltipWidth;

	//***************************************************************************************

	// Use a "work" rect to calculate the bottom. This work rect will be inset 
	// slightly from the rect we have just created so the tooltip does not touch
	// the sides.
	CRect rectCalc = m_rectDisplay;

	rectCalc.top += 1;

	int nHeight = dc.DrawText(m_strTitle,&rectCalc,0 
		| DT_CALCRECT
		| DT_LEFT
		| DT_NOCLIP
		| DT_EDITCONTROL
		| DT_NOPREFIX
		| DT_WORDBREAK
		);

	// If this is a single line, shorten the display to get rid of any excess blank space
	if (nHeight == tm.tmHeight)
		rectCalc.right = rectCalc.left + size.cx + 3;

	m_rectDisplay.bottom = m_rectDisplay.top + nHeight;

	// ensure the tooltip does not exceed the bottom of the screen
	if (m_rectDisplay.bottom > rectClient.bottom)
	{
		m_rectDisplay.bottom = rectClient.bottom;
		rectCalc.bottom = rectClient.bottom; 
	}

	SetWindowPos( &wndTop, 
        m_rectDisplay.left-3, 
        m_rectDisplay.top-1, 
		m_rectDisplay.Width()+3, 
        m_rectDisplay.Height(),
		SWP_SHOWWINDOW|SWP_NOACTIVATE );

 //	// FNA - handle colors correctly
	//if (crBackClr != CLR_DEFAULT)
	//{
	//	CBrush backBrush(crBackClr);
	//	CBrush* pOldBrush = dc.SelectObject(&backBrush);
	//	CRect rect;
	//	dc.GetClipBox(&rect);     // Erase the area needed 

	//	dc.PatBlt(rect.left, rect.top, rect.Width(), rect.Height(),  PATCOPY);
	//	dc.SelectObject(pOldBrush);
	//}
	//// Set color
	//if (crTextClr != CLR_DEFAULT)//FNA
	//	dc.SetTextColor(crTextClr);//FA

	//dc.SetBkMode( TRANSPARENT );
	//dc.TextOut( 0, 0, m_strTitle );
	SetCapture();
	dc.SelectObject(pOldFont);
}

void CTitleTip::Hide()
{
  	if (!::IsWindow(GetSafeHwnd()))
        return;

    if (GetCapture()->GetSafeHwnd() == GetSafeHwnd())
        ReleaseCapture();

	ShowWindow( SW_HIDE );
}

void CTitleTip::OnMouseMove(UINT nFlags, CPoint point) 
{
    if (!m_rectHover.PtInRect(point)) 
    {
        Hide();
        
        // Forward the message
        ClientToScreen( &point );
        CWnd *pWnd = WindowFromPoint( point );
        if ( pWnd == this ) 
            pWnd = m_pParentWnd;
        
        int hittest = (int)pWnd->SendMessage(WM_NCHITTEST,0,MAKELONG(point.x,point.y));
        
        if (hittest == HTCLIENT) {
            pWnd->ScreenToClient( &point );
            pWnd->PostMessage( WM_MOUSEMOVE, nFlags, MAKELONG(point.x,point.y) );
        } else {
            pWnd->PostMessage( WM_NCMOUSEMOVE, hittest, MAKELONG(point.x,point.y) );
        }
    }
}

void CTitleTip::OnLButtonDown(UINT nFlags, CPoint point)
{
	MSG msg;
	msg.hwnd = GetSafeHwnd();
	msg.message = WM_LBUTTONDOWN;
	msg.wParam = nFlags;
	msg.lParam = MAKELONG(point.x,point.y);
	ProcessMouseMessage(&msg);
}

void CTitleTip::OnMButtonDown(UINT nFlags, CPoint point)
{
	MSG msg;
	msg.hwnd = GetSafeHwnd();
	msg.message = WM_MBUTTONDOWN;
	msg.wParam = nFlags;
	msg.lParam = MAKELONG(point.x,point.y);
	ProcessMouseMessage(&msg);
}

void CTitleTip::OnRButtonDown(UINT nFlags, CPoint point)
{
	MSG msg;
	msg.hwnd = GetSafeHwnd();
	msg.message = WM_RBUTTONDOWN;
	msg.wParam = nFlags;
	msg.lParam = MAKELONG(point.x,point.y);
	ProcessMouseMessage(&msg);
}

void CTitleTip::OnRButtonUp(UINT nFlags, CPoint point)
{
	MSG msg;
	msg.hwnd = GetSafeHwnd();
	msg.message = WM_RBUTTONUP;
	msg.wParam = nFlags;
	msg.lParam = MAKELONG(point.x,point.y);
	ProcessMouseMessage(&msg);
}

void CTitleTip::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	Hide();
	m_pParentWnd->PostMessage(WM_KEYDOWN, nChar, MAKELPARAM(nRepCnt,nFlags));
}

void CTitleTip::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	Hide();
	m_pParentWnd->PostMessage(WM_SYSKEYDOWN, nChar, MAKELPARAM(nRepCnt,nFlags));
}

void CTitleTip::ProcessMouseMessage(MSG* pMsg) 
{
    // Used to qualify WM_LBUTTONDOWN messages as double-clicks
    DWORD dwTick=0;
    BOOL bDoubleClick=FALSE;

 	int hittest;
	switch (pMsg->message)
	{
	case WM_LBUTTONDOWN:
       // Get tick count since last LButtonDown
        dwTick = GetTickCount();
        bDoubleClick = ((dwTick - m_dwLastLButtonDown) <= m_dwDblClickMsecs);
        m_dwLastLButtonDown = dwTick;
        // NOTE: DO NOT ADD break; STATEMENT HERE! Let code fall through

	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
		{
		POINTS pts = MAKEPOINTS( pMsg->lParam );
		POINT  point;
		point.x = pts.x;
		point.y = pts.y;

		ClientToScreen( &point );
        Hide();

		hittest = (int)m_pParentWnd->SendMessage(WM_NCHITTEST,0,MAKELONG(point.x,point.y));

		if (hittest == HTCLIENT) 
		{
			m_pParentWnd->ScreenToClient( &point );
			pMsg->lParam = MAKELONG(point.x,point.y);
		}
		else 
		{
			switch (pMsg->message) {
			case WM_LBUTTONDOWN: 
				pMsg->message = WM_NCLBUTTONDOWN;
				break;
			case WM_RBUTTONDOWN: 
				pMsg->message = WM_NCRBUTTONDOWN;
				break;
			case WM_RBUTTONUP: 
				pMsg->message = WM_NCRBUTTONUP;
				break;
			case WM_MBUTTONDOWN: 
				pMsg->message = WM_NCMBUTTONDOWN;
				break;
			}
			pMsg->wParam = hittest;
			pMsg->lParam = MAKELONG(point.x,point.y);
		}


        // If this is the 2nd WM_LBUTTONDOWN in x milliseconds,
        // post a WM_LBUTTONDBLCLK message instead of a single click.
        m_pParentWnd->PostMessage(  bDoubleClick ? WM_LBUTTONDBLCLK : pMsg->message,
                            pMsg->wParam,
                            pMsg->lParam);
		}
	}
}

BOOL CTitleTip::PreTranslateMessage(MSG* pMsg) 
{
    // Used to qualify WM_LBUTTONDOWN messages as double-clicks
    DWORD dwTick=0;
    BOOL bDoubleClick=FALSE;

    CWnd *pWnd;
	int hittest;
	switch (pMsg->message)
	{
	case WM_LBUTTONDOWN:
       // Get tick count since last LButtonDown
        dwTick = GetTickCount();
        bDoubleClick = ((dwTick - m_dwLastLButtonDown) <= m_dwDblClickMsecs);
        m_dwLastLButtonDown = dwTick;
        // NOTE: DO NOT ADD break; STATEMENT HERE! Let code fall through

	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
		{
		POINTS pts = MAKEPOINTS( pMsg->lParam );
		POINT  point;
		point.x = pts.x;
		point.y = pts.y;

		ClientToScreen( &point );
        Hide();

		pWnd = WindowFromPoint( point );
		if (!pWnd)
			return CWnd::PreTranslateMessage(pMsg);

		if( pWnd->GetSafeHwnd() == GetSafeHwnd()) 
			pWnd = m_pParentWnd;

		hittest = (int)pWnd->SendMessage(WM_NCHITTEST,0,MAKELONG(point.x,point.y));

		if (hittest == HTCLIENT) 
		{
			pWnd->ScreenToClient( &point );
			pMsg->lParam = MAKELONG(point.x,point.y);
		}
		else 
		{
			switch (pMsg->message) {
			case WM_LBUTTONDOWN: 
				pMsg->message = WM_NCLBUTTONDOWN;
				break;
			case WM_RBUTTONDOWN: 
				pMsg->message = WM_NCRBUTTONDOWN;
				break;
			case WM_MBUTTONDOWN: 
				pMsg->message = WM_NCMBUTTONDOWN;
				break;
			}
			pMsg->wParam = hittest;
			pMsg->lParam = MAKELONG(point.x,point.y);
		}


        // If this is the 2nd WM_LBUTTONDOWN in x milliseconds,
        // post a WM_LBUTTONDBLCLK message instead of a single click.
        pWnd->PostMessage(  bDoubleClick ? WM_LBUTTONDBLCLK : pMsg->message,
                            pMsg->wParam,
                            pMsg->lParam);
		return TRUE;
		}
		
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
        Hide();
		m_pParentWnd->PostMessage( pMsg->message, pMsg->wParam, pMsg->lParam );
		return TRUE;
	}

	if( GetFocus() == NULL )
	{
        Hide();
		return TRUE;
	}

	return CWnd::PreTranslateMessage(pMsg);
}

void CTitleTip::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

    TEXTMETRIC tm;
	dc.GetTextMetrics(&tm);

	CFont font, *pOldFont = NULL;
	if (m_lpLogFont)
	{
		font.CreateFontIndirect(m_lpLogFont);
		pOldFont = dc.SelectObject(&font);
	}
	else
	{
		// use same font as ctrl
		pOldFont = dc.SelectObject( m_pParentWnd->GetFont() );
	}
	int nHeight=0;

	CRect rect = m_rectDisplay;
	ScreenToClient(rect);

	dc.SetBkMode( TRANSPARENT ); 

	nHeight = dc.DrawText(m_strTitle, rect, 0 
		| DT_LEFT
		| DT_EDITCONTROL
		| DT_NOPREFIX
		| DT_WORDBREAK
		);


	dc.SelectObject( pOldFont );
	//m_font.DeleteObject();

	// Do not call CWnd::OnPaint() for painting messages
}
#endif // GRIDCONTROL_NO_TITLETIPS