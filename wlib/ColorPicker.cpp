//-----------------------------------------------------------------------------
// 
// @doc
//
// @module	ColorButton.cpp - Color button and popup |
//
// This module contains the definition of color button and popup
//
// Copyright (c) 2000-2002 - Descartes Systems Sciences, Inc.
//
// Based on work by Chris Maunder, Alexander Bischofberger and James White.
//
// http://www.codetools.com/miscctrl/colorbutton.asp
// http://www.codetools.com/miscctrl/colour_picker.asp
//
// Copyright (c) 2000-2002 - Descartes Systems Sciences, Inc.
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are 
// met:
// 
// 1.Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer.
// 2.Neither the name of Descartes Systems Sciences, Inc nor the names of 
//    its contributors may be used to endorse or promote products derived 
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
// A PARTICULAR PURPOSE ARE DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT 
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED 
// TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// --- ORIGINAL COPYRIGHT STATEMENT ---
//
// Written by Chris Maunder (chrismaunder@codeguru.com)
// Extended by Alexander Bischofberger (bischofb@informatik.tu-muenchen.de)
// Copyright (c) 1998.
//
// Updated 30 May 1998 to allow any number of colours, and to
//                     make the appearance closer to Office 97.
//                     Also added "Default" text area.        (CJM)
//
//         13 June 1998 Fixed change of focus bug (CJM)
//         30 June 1998 Fixed bug caused by focus bug fix (D'oh!!)
//                      Solution suggested by Paul Wilkerson.
//
// ColourPopup is a helper class for the colour picker control
// CColourPicker.Check out the header file or the accompanying 
// HTML doc file for details.
//
// This code may be used in compiled form in any way you desire.This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included.
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to you or your
// computer whatsoever.It's free, so don't hassle me about it.
//
// Expect bugs.
// 
// Please use and enjoy.Please let me know of any bugs/mods/improvements 
// that you have found/implemented and I will fix/incorporate them into this
// file.
//
// @end
//
// $History: ColorButton.cpp $
//
//      *****************  code break *****************
//      User: TJ    Date: 1/27/07    Time: quite late
//      Removed all references to button in order
//		to use picker from toolbar dropdown notification.
//		CColorPicker is a dummy message pumping object
//		shows a real picker window via ShowPicker
//
//      *****************  Version 4  *****************
//      User: Tim Smith    Date: 1/21/02    Time: 2:53p
//      Updated in $/Omni_V2/exe_cnf
//      Ported code to VC7 compiler
//      
//      *****************  Version 3  *****************
//      User: Tim Smith    Date: 9/10/01    Time: 9:05a
//      Updated in $/Omni_V2/exe_cnf
//      
//      *****************  Version 2  *****************
//      User: Tim Smith    Date: 8/28/01    Time: 4:25p
//      Updated in $/Omni_V2/exe_cnf
//      Updated copyright dates.
//      
//      *****************  Version 1  *****************
//      User: Tim Smith    Date: 8/28/01    Time: 3:19p
//      Created in $/Omni_V2/exe_cnf
//
//-----------------------------------------------------------------------------

#include "stdafx.h"

#include "wlib.h"
#include "ColorPicker.h"

//
// Color table
//

CColorPicker::ColorTableEntry CColorPicker::gm_sColors [] =
{
    { RGB(0x00, 0x00, 0x00),    _T("Black")             },
    { RGB(0xA5, 0x2A, 0x00),    _T("Brown")             },
    { RGB(0x00, 0x40, 0x40),    _T("Dark Olive Green")  },
    { RGB(0x00, 0x55, 0x00),    _T("Dark Green")        },
    { RGB(0x00, 0x00, 0x5E),    _T("Dark Teal")         },
    { RGB(0x00, 0x00, 0x8B),    _T("Dark blue")         },
    { RGB(0x4B, 0x00, 0x82),    _T("Indigo")            },
    { RGB(0x28, 0x28, 0x28),    _T("Dark grey")         },

    { RGB(0x8B, 0x00, 0x00),    _T("Dark red")          },
    { RGB(0xFF, 0x68, 0x20),    _T("Orange")            },
    { RGB(0x8B, 0x8B, 0x00),    _T("Dark yellow")       },
    { RGB(0x00, 0x93, 0x00),    _T("Green")             },
    { RGB(0x38, 0x8E, 0x8E),    _T("Teal")              },
    { RGB(0x00, 0x00, 0xFF),    _T("Blue")              },
    { RGB(0x7B, 0x7B, 0xC0),    _T("Blue-grey")         },
    { RGB(0x66, 0x66, 0x66),    _T("Grey - 40")         },

    { RGB(0xFF, 0x00, 0x00),    _T("Red")               },
    { RGB(0xFF, 0xAD, 0x5B),    _T("Light orange")      },
    { RGB(0x32, 0xCD, 0x32),    _T("Lime")              }, 
    { RGB(0x3C, 0xB3, 0x71),    _T("Sea green")         },
    { RGB(0x7F, 0xFF, 0xD4),    _T("Aqua")              },
    { RGB(0x7D, 0x9E, 0xC0),    _T("Light blue")        },
    { RGB(0x80, 0x00, 0x80),    _T("Violet")            },
    { RGB(0x7F, 0x7F, 0x7F),    _T("Grey - 50")         },

    { RGB(0xFF, 0xC0, 0xCB),    _T("Pink")              },
    { RGB(0xFF, 0xD7, 0x00),    _T("Gold")              },
    { RGB(0xFF, 0xFF, 0x00),    _T("Yellow")            },    
    { RGB(0x00, 0xFF, 0x00),    _T("Bright green")      },
    { RGB(0x40, 0xE0, 0xD0),    _T("Turquoise")         },
    { RGB(0xC0, 0xFF, 0xFF),    _T("Skyblue")           },
    { RGB(0x48, 0x00, 0x48),    _T("Plum")              },
    { RGB(0xC0, 0xC0, 0xC0),    _T("Light grey")        },

    { RGB(0xFF, 0xE4, 0xE1),    _T("Rose")              },
    { RGB(0xD2, 0xB4, 0x8C),    _T("Tan")               },
    { RGB(0xFF, 0xFF, 0xE0),    _T("Light yellow")      },
    { RGB(0x98, 0xFB, 0x98),    _T("Pale green ")       },
    { RGB(0xAF, 0xEE, 0xEE),    _T("Pale turquoise")    },
    { RGB(0x68, 0x83, 0x8B),    _T("Pale blue")         },
	{ RGB(0xE6, 0xE6, 0xFA),    _T("Lavender")          },
    { RGB(0xFF, 0xFF, 0xFF),    _T("White")             }
};

//
// Other definitions
//

#define DEFAULT_BOX_VALUE	-3
#define CUSTOM_BOX_VALUE	-2
#define INVALID_COLOR		-1
#define MAX_COLORS			100

//
// Sizing definitions
//

static const CSize s_sizeTextHiBorder (3, 3);
static const CSize s_sizeTextMargin (2, 2);
static const CSize s_sizeBoxHiBorder (2, 2);
static const CSize s_sizeBoxMargin (0, 0);
static const CSize s_sizeBoxCore (14, 14);

//
// Ok, here is how all the sizing works. All picker elements use the
// exact same rules. They all have 3 elements, their core size, the size
// of the highlight border, an the size of the margin.
//
// For text, the core size is just the extent of the text to be drawn.
// For the color boxes, it is the s_sizeBoxCore value. (For pointless
// jollies, the box size was modified so it doesn't have to be square. We
// are talking about changing 3-4 lines of code. No biggie.)
//
// Also, just a point of note. Each area has a well defined rectagle.
// (m_rectDefaultText, m_rectCustomText, and m_rectBoxes)
// Even if there isn't any default or custom text, the rectagles are still
// well defined, but they have no height. Their UpperLeft () is at the
// proper position and their Width () is valid. In the case of the boxes,
// m_rectBoxes is the rectagle that contains all the boxes. These changes
// made drawing and hit test 100000 times easier.
//


//-----------------------------------------------------------------------------
//
// @mfunc Exported function to invoke color picker
//
// @parm HWND | hwndParent | parent to picker window
// @parm CPoint& | topleft | screen position of top left window corner
// @parm COLORREF& | clr | input (initial) and output (picked) color
//
// @rdesc Return value
//
//		@parm true | A new color was selected
//		@parm false | The user canceled picking
//
//-----------------------------------------------------------------------------

bool InvokeColorPicker(HWND hwndParent, const CPoint& topleft, COLORREF& clr)
{
	bool picked=false;
	CColorPicker cp(hwndParent, topleft);
	cp.SetColor(clr);
	cp.SetDefaultText(_T(""));
	if(cp.ShowPicker())
	{
		clr=cp.GetColor();
		picked=true;
	}
	return picked;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CColorPicker> constructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CColorPicker::CColorPicker (HWND hwndParent, const CPoint& pt)
: m_wndPicker (this, 1)
{
	m_Position=pt;
	m_hwndParent=hwndParent;
	m_clrCurrent = CLR_DEFAULT;
	m_clrDefault = ::GetSysColor (COLOR_APPWORKSPACE);
	m_pszDefaultText = _tcsdup (_T ("Automatic"));
	m_pszCustomText = _tcsdup (_T ("More Colors..."));
	m_bMouseDownReceived=false;
}

//-----------------------------------------------------------------------------
//
// @mfunc <c CColorPicker> destructor.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

CColorPicker::~CColorPicker ()
{
	if (m_pszDefaultText)
		free (m_pszDefaultText);
	if (m_pszCustomText)
		free (m_pszCustomText);
}

//-----------------------------------------------------------------------------
//
// @mfunc Set custom text
//

void CColorPicker::SetCustomText (UINT nID)
{
	if (nID == 0)
		SetCustomText ((LPCTSTR) NULL);
	else
	{
		TCHAR szText [256];
		if (LoadString (_AtlBaseModule.GetResourceInstance (),
			nID, szText, sizeof (szText) / sizeof (szText [0])) == 0)
			return;
		SetCustomText (szText);
	}
}

//-----------------------------------------------------------------------------
//
// @mfunc Set default text
//

void CColorPicker::SetDefaultText (UINT nID)
{
	if (nID == 0)
		SetDefaultText ((LPCTSTR) NULL);
	else
	{
		TCHAR szText [256];
		if (LoadString (_AtlBaseModule .GetResourceInstance (),
			nID, szText, sizeof (szText) / sizeof (szText [0])) == 0)
			return;
		SetDefaultText (szText);
	}
}

//-----------------------------------------------------------------------------
//
// @mfunc Display the picker popup
//
// @rdesc Return value
//
//		@parm TRUE | A new color was selected
//		@parm FALSE | The user canceled the picket
//
//-----------------------------------------------------------------------------

BOOL CColorPicker::ShowPicker()
{
	BOOL fOked = FALSE;

	//
	// See what version we are using
	//

	OSVERSIONINFO osvi;
	osvi .dwOSVersionInfoSize = sizeof (osvi);
	::GetVersionEx (&osvi);
	bool fIsXP = osvi .dwPlatformId == VER_PLATFORM_WIN32_NT &&
		(osvi .dwMajorVersion > 5 || (osvi .dwMajorVersion == 5 &&
		osvi .dwMinorVersion >= 1));

	//
	// Get the flat flag
	//

	m_fPickerFlat = FALSE;
#if (_WIN32_WINNT >= 0x0501)
 	if (fIsXP)
		::SystemParametersInfo (SPI_GETFLATMENU, 0, &m_fPickerFlat, FALSE);
#endif

	//
	// Get all the colors I need
	//

	int nAlpha = 48;
	m_clrBackground = ::GetSysColor (COLOR_MENU);
	m_clrHiLightBorder = ::GetSysColor (COLOR_HIGHLIGHT);
	m_clrHiLight = m_clrHiLightBorder;
#if (WINVER >= 0x0501)
	if (fIsXP)
		m_clrHiLight = ::GetSysColor (COLOR_MENUHILIGHT);
#endif
	m_clrHiLightText = ::GetSysColor (COLOR_HIGHLIGHTTEXT);
	m_clrText = ::GetSysColor (COLOR_MENUTEXT);
	m_clrLoLight = RGB (
		(GetRValue (m_clrBackground) * (255 - nAlpha) + 
			GetRValue (m_clrHiLightBorder) * nAlpha) >> 8,
		(GetGValue (m_clrBackground) * (255 - nAlpha) + 
			GetGValue (m_clrHiLightBorder) * nAlpha) >> 8,
		(GetBValue (m_clrBackground) * (255 - nAlpha) + 
			GetBValue (m_clrHiLightBorder) * nAlpha) >> 8);
   
	//
	// Get the margins
	//

	m_rectMargins .left = ::GetSystemMetrics (SM_CXEDGE);
	m_rectMargins .top = ::GetSystemMetrics (SM_CYEDGE);
	m_rectMargins .right = ::GetSystemMetrics (SM_CXEDGE);
	m_rectMargins .bottom = ::GetSystemMetrics (SM_CYEDGE);

	//
	// Initialize some sizing parameters
	//

	m_nNumColors = sizeof (gm_sColors) / sizeof (ColorTableEntry);
	_ASSERTE (m_nNumColors <= MAX_COLORS);
	if (m_nNumColors > MAX_COLORS)
		m_nNumColors = MAX_COLORS;

	//
	// Initialize our state
	// 

	m_nCurrentSel       = INVALID_COLOR;
	m_nChosenColorSel	= INVALID_COLOR;
	m_clrPicker			= m_clrCurrent;

	//
	// Create the font
	//

	NONCLIENTMETRICS ncm;
	ncm .cbSize = sizeof (NONCLIENTMETRICS);
	SystemParametersInfo (SPI_GETNONCLIENTMETRICS,
		sizeof (NONCLIENTMETRICS), &ncm, 0);
	m_font .CreateFontIndirect (&ncm .lfMessageFont);

	//
	// Create the palette
	//

	struct 
	{
		LOGPALETTE    LogPalette;
		PALETTEENTRY  PalEntry [MAX_COLORS];
	} pal;

	LOGPALETTE *pLogPalette = (LOGPALETTE *) &pal;
	pLogPalette ->palVersion    = 0x300;
	pLogPalette ->palNumEntries = (WORD) m_nNumColors; 

	for (int i = 0; i < m_nNumColors; i++)
	{
		pLogPalette ->palPalEntry [i] .peRed   = GetRValue (gm_sColors [i] .clrColor);
		pLogPalette ->palPalEntry [i] .peGreen = GetGValue (gm_sColors [i] .clrColor);
		pLogPalette ->palPalEntry [i] .peBlue  = GetBValue (gm_sColors [i] .clrColor);
		pLogPalette ->palPalEntry [i] .peFlags = 0;
	}
	m_palette .CreatePalette (pLogPalette);

	//
	// Register the window class used for the picker
	//

	WNDCLASSEX wc;
	wc .cbSize = sizeof (WNDCLASSEX);
	wc .style  = CS_CLASSDC | CS_SAVEBITS | CS_HREDRAW | CS_VREDRAW;
	wc .lpfnWndProc = CContainedWindow::StartWindowProc;
	wc .cbClsExtra  = 0;
	wc .cbWndExtra = 0;
	wc .hInstance = _AtlBaseModule .GetModuleInstance ();
	wc .hIcon = NULL;
	wc .hCursor = LoadCursor (NULL, IDC_ARROW);
	wc .hbrBackground = (HBRUSH) (COLOR_MENU + 1);
	wc .lpszMenuName = NULL;
	wc .lpszClassName = _T ("LN_ColorPicker");
	wc .hIconSm = NULL;
#if (_WIN32_WINNT >= 0x0501)
	if (fIsXP)
	{
		BOOL fDropShadow;
		::SystemParametersInfo (SPI_GETDROPSHADOW, 0, &fDropShadow, FALSE);
		if (fDropShadow)
			wc .style |= CS_DROPSHADOW;
	}
#endif
	ATOM atom = ::RegisterClassEx (&wc);

	//
	// Create the window
	//

	_AtlWinModule.AddCreateWndData (&m_wndPicker .m_thunk .cd, &m_wndPicker);
	m_wndPicker .m_hWnd = ::CreateWindowEx (0, (LPCTSTR) MAKELONG (atom, 0), 
		_T (""),  WS_POPUP, m_Position.x, m_Position.y, 100, 100,
		m_hwndParent, NULL, _AtlBaseModule.GetModuleInstance(), NULL);

	//
	// If we created the window
	//

	if (m_wndPicker .m_hWnd != NULL)
	{
        
		//
		// Set the window size
		//

	    SetPickerWindowSize ();

		//
	    // Create the tooltips
		//

		CToolTipCtrl sToolTip;
		CreatePickerToolTips (sToolTip);

		//
		// Find which cell (if any) corresponds to the initial color
		//

		FindPickerCellFromColor (m_clrCurrent);

		//
		// Make visible
		//
	
		m_wndPicker .ShowWindow (SW_SHOWNA);

		//
		// Purge the message queue of paints
		//

		MSG msg;
		while (::PeekMessage(&msg, NULL, WM_PAINT, WM_PAINT, PM_NOREMOVE))
		{
			if (!GetMessage(&msg, NULL, WM_PAINT, WM_PAINT))
				return FALSE;
			DispatchMessage(&msg);
		}

		// 
		// Set capture to the window which received this message
		//

		m_wndPicker .SetCapture ();
		_ASSERTE (m_wndPicker .m_hWnd == ::GetCapture ());

		//
		// Get messages until capture lost or cancelled/accepted
		//

		while (m_wndPicker .m_hWnd == ::GetCapture ())
		{
			MSG msg;
			if (!::GetMessage(&msg, NULL, 0, 0))
			{
				::PostQuitMessage (msg .wParam);
				break;
			}

			sToolTip .RelayEvent (&msg);

			switch (msg.message)
			{
				case WM_LBUTTONDOWN:
					m_bMouseDownReceived=true;
					break;

				case WM_LBUTTONUP:
					{
						if(m_bMouseDownReceived)
						{
							BOOL bHandled = TRUE;
							OnPickerLButtonUp (msg .message, 
								msg .wParam, msg .lParam, bHandled);
						}
					}
					break;

				case WM_MOUSEMOVE:
					{
						BOOL bHandled = TRUE;
						OnPickerMouseMove (msg .message, 
							msg .wParam, msg .lParam, bHandled);
					}
					break;

				case WM_KEYUP:
					break;

				case WM_KEYDOWN:
					{
						BOOL bHandled = TRUE;
						OnPickerKeyDown (msg .message, 
							msg .wParam, msg .lParam, bHandled);
					}
					break;

				case WM_RBUTTONDOWN:
					::ReleaseCapture ();
					m_fOked = FALSE;
					break;

				// just dispatch rest of the messages
				default:
					DispatchMessage (&msg);
					break;
			}
		}
		::ReleaseCapture ();
		fOked = m_fOked;

		//
		// Destroy the window
		//

		sToolTip.DestroyWindow ();
		m_wndPicker.DestroyWindow ();

		//
		// If needed, show custom
		//

		if (fOked)
		{
			if (fOked && m_nCurrentSel == CUSTOM_BOX_VALUE)
			{
				CColorDialog dlg (m_clrCurrent, 
					CC_FULLOPEN | CC_ANYCOLOR, m_hwndParent);

				if (dlg.DoModal() == IDOK)
					m_clrCurrent = dlg.GetColor();
				else
					fOked = FALSE;
			}
			else
				m_clrCurrent = m_clrPicker;
		}

		//
		// Clean up GDI objects
		//

		m_font .DeleteObject ();
		m_palette .DeleteObject ();
	}

	//
	// Unregister our class
	//

	::UnregisterClass ((LPCTSTR) MAKELONG (atom, 0),
		_AtlBaseModule .GetModuleInstance());
	return fOked;
}

//-----------------------------------------------------------------------------
//
// @mfunc Set the window size of the picker control
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CColorPicker::SetPickerWindowSize ()
{
	SIZE szText = { 0, 0 };

	//
    // If we are showing a custom or default text area, get the font and text size.
	//

    if (HasCustomText () || HasDefaultText ())
    {
		CClientDC dc (m_wndPicker);
		HFONT hfontOld = dc .SelectFont (m_font);

		//
		// Get the size of the custom text (if there IS custom text)
		//

		if (HasCustomText ())
		{
			dc .GetTextExtent (m_pszCustomText, 
				_tcslen (m_pszCustomText), &szText);
		}

		//
        // Get the size of the default text (if there IS default text)
		//

        if (HasDefaultText ())
        {
			SIZE szDefault;
			dc .GetTextExtent (m_pszDefaultText, 
				_tcslen (m_pszDefaultText), &szDefault);
			if (szDefault .cx > szText .cx)
				szText .cx = szDefault .cx;
			if (szDefault .cy > szText .cy)
				szText .cy = szDefault .cy;
        }
		dc .SelectFont (hfontOld);

		//
		// Commpute the final size
		//

		szText .cx += 2 * (s_sizeTextMargin .cx + s_sizeTextHiBorder .cx);
		szText .cy += 2 * (s_sizeTextMargin .cy + s_sizeTextHiBorder .cy);
    }

	//
	// Initiailize our box size
	//

	_ASSERTE (s_sizeBoxHiBorder .cx == s_sizeBoxHiBorder .cy);
	_ASSERTE (s_sizeBoxMargin .cx == s_sizeBoxMargin .cy);
	m_sizeBox .cx = s_sizeBoxCore .cx + (s_sizeBoxHiBorder .cx + s_sizeBoxMargin .cx) * 2;
	m_sizeBox .cy = s_sizeBoxCore .cy + (s_sizeBoxHiBorder .cy + s_sizeBoxMargin .cy) * 2;

	//
    // Get the number of columns and rows
	//

    m_nNumColumns = 8;
    m_nNumRows = m_nNumColors / m_nNumColumns;
    if ((m_nNumColors % m_nNumColumns) != 0) 
		m_nNumRows++;

	//
	// Compute the min width
	//

	int nBoxTotalWidth = m_nNumColumns * m_sizeBox .cx;
	int nMinWidth = nBoxTotalWidth;
	if (nMinWidth < szText .cx)
		nMinWidth = szText .cx;

	//
	// Create the rectangle for the default text
	//

	m_rectDefaultText = CRect (
		CPoint (0, 0), 
		CSize (nMinWidth, HasDefaultText () ? szText .cy : 0)
		);
		
	//
	// Initialize the color box rectangle
	//

	m_rectBoxes = CRect (
		CPoint ((nMinWidth - nBoxTotalWidth) / 2, m_rectDefaultText .bottom), 
		CSize (nBoxTotalWidth, m_nNumRows * m_sizeBox .cy)
		);

	//
	// Create the rectangle for the custom text
	//

 	m_rectCustomText = CRect (
		CPoint (0, m_rectBoxes .bottom), 
		CSize (nMinWidth, HasCustomText () ? szText .cy : 0)
		);

	//
    // Get the current window position, and set the new size
	//

	CRect rectWindow (
		m_rectDefaultText .TopLeft (), 
		m_rectCustomText .BottomRight ());
    CRect rect;
	m_wndPicker .GetWindowRect (&rect);
	rectWindow .OffsetRect (rect .TopLeft ());

	//
	// Adjust the rects for the border
	//

	rectWindow .right += m_rectMargins .left + m_rectMargins .right;
	rectWindow .bottom += m_rectMargins .top + m_rectMargins .bottom;
	::OffsetRect (&m_rectDefaultText, m_rectMargins .left, m_rectMargins .top);
	::OffsetRect (&m_rectBoxes, m_rectMargins .left, m_rectMargins .top);
	::OffsetRect (&m_rectCustomText, m_rectMargins .left, m_rectMargins .top);

	//
	// Get the screen rectangle
	//

	CRect rectScreen (CPoint (0, 0), CSize (
		::GetSystemMetrics (SM_CXSCREEN),
        ::GetSystemMetrics (SM_CYSCREEN)));
#if (WINVER >= 0x0500)
	HMODULE hUser32 = ::GetModuleHandleA ("USER32.DLL");
	if (hUser32 != NULL)
	{
		typedef HMONITOR (WINAPI *FN_MonitorFromWindow) (HWND hWnd, DWORD dwFlags);
		typedef BOOL (WINAPI *FN_GetMonitorInfo) (HMONITOR hMonitor, LPMONITORINFO lpmi);
		FN_MonitorFromWindow pfnMonitorFromWindow = (FN_MonitorFromWindow)
			::GetProcAddress (hUser32, "MonitorFromWindow");
		FN_GetMonitorInfo pfnGetMonitorInfo = (FN_GetMonitorInfo)
			::GetProcAddress (hUser32, "GetMonitorInfoA");
		if (pfnMonitorFromWindow != NULL && pfnGetMonitorInfo != NULL)
		{
			MONITORINFO mi;
			HMONITOR hMonitor = pfnMonitorFromWindow (m_hwndParent, 
				MONITOR_DEFAULTTONEAREST);
			mi .cbSize = sizeof (mi);
			pfnGetMonitorInfo (hMonitor, &mi);
			rectScreen = mi .rcWork;
		}
	}
#endif

	//
    // Need to check it'll fit on screen: Too far right?
	//

    if (rectWindow .right > rectScreen .right)
		::OffsetRect (&rectWindow, rectScreen .right - rectWindow .right, 0);

	//
    // Too far left?
	//

    if (rectWindow .left < rectScreen .left)
        ::OffsetRect (&rectWindow, rectScreen .left - rectWindow .left, 0);

	//
    // Bottom falling out of screen?  If so, the move
	// the whole popup above the parents window
	//

    if (rectWindow .bottom > rectScreen .bottom)
    {
        CRect rcParent;
		m_wndPicker.GetWindowRect (&rcParent);
        ::OffsetRect (&rectWindow, 0,
			- ((rcParent .bottom - rcParent .top) + 
			(rectWindow .bottom - rectWindow .top)));
    }

	//
    // Set the window size and position
	//

	m_wndPicker .MoveWindow (&rectWindow, TRUE);
}

//-----------------------------------------------------------------------------
//
// @mfunc Create the tooltips for the picker
//
// @parm CToolTipCtrl & | sToolTip | Tool tip control
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CColorPicker::CreatePickerToolTips (CToolTipCtrl &sToolTip)
{
	//
    // Create the tool tip
	//

    if (!sToolTip .Create (m_wndPicker .m_hWnd)) 
		return;

	//
    // Add a tool for each cell
	// 
    for (int i = 0; i < m_nNumColors; i++)
    {
        CRect rect;
        if (!GetPickerCellRect (i, &rect)) 
			continue;
		sToolTip .AddTool (m_wndPicker .m_hWnd, 
			gm_sColors [i] .pszName, &rect, 1);
    }
}

//-----------------------------------------------------------------------------
//
// @mfunc Gets the dimensions of the colour cell given by (row,col)
//
// @parm int | nIndex | Index of the cell
//
// @parm RECT * | pRect | Rectangle of the cell
//
// @rdesc Return value.
//
//		@flag TRUE | If the index is valid
//		@flag FALSE | If the index is not valid
//
//-----------------------------------------------------------------------------

BOOL CColorPicker::GetPickerCellRect (int nIndex, RECT *pRect) const
{

	//
	// If the custom box
	//

    if (nIndex == CUSTOM_BOX_VALUE)
    {
		*pRect = m_rectCustomText;
		return TRUE;
    }

	//
	// If the default box
	//

    else if (nIndex == DEFAULT_BOX_VALUE)
    {
		*pRect = m_rectDefaultText;
		return TRUE;
    }

	//
	// Validate the range
	//

    if (nIndex < 0 || nIndex >= m_nNumColors)
        return FALSE;

	//
	// Compute the value of the boxes
	//

    pRect ->left = (nIndex % m_nNumColumns) * m_sizeBox .cx + m_rectBoxes .left;
    pRect ->top  = (nIndex / m_nNumColumns) * m_sizeBox .cy + m_rectBoxes .top;
    pRect ->right = pRect ->left + m_sizeBox .cx;
    pRect ->bottom = pRect ->top + m_sizeBox .cy;
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Set the chosen color from the given color
//
// @parm COLORREF | clr | Color
//
// @rdesc None
//
//-----------------------------------------------------------------------------

void CColorPicker::FindPickerCellFromColor (COLORREF clr)
{
    if (clr == CLR_DEFAULT && HasDefaultText ())
    {
        m_nChosenColorSel = DEFAULT_BOX_VALUE;
        return;
    }

    for (int i = 0; i < m_nNumColors; i++)
    {
        if (gm_sColors [i] .clrColor == clr)
        {
            m_nChosenColorSel = i;
            return;
        }
    }

    if (HasCustomText ())
        m_nChosenColorSel = CUSTOM_BOX_VALUE;
    else
        m_nChosenColorSel = INVALID_COLOR;
}

//-----------------------------------------------------------------------------
//
// @mfunc Change the current selection
//
// @parm int | nIndex | New selection
//
// @rdesc None
//
//-----------------------------------------------------------------------------

void CColorPicker::ChangePickerSelection (int nIndex)
{
	CClientDC dc (m_wndPicker);

	//
	// Clamp the index
	//

    if (nIndex > m_nNumColors)
        nIndex = CUSTOM_BOX_VALUE; 

	//
	// If the current selection is valid, redraw old selection with out
	// it being selected
	//

    if ((m_nCurrentSel >= 0 && m_nCurrentSel < m_nNumColors) ||
        m_nCurrentSel == CUSTOM_BOX_VALUE || m_nCurrentSel == DEFAULT_BOX_VALUE)
    {
        int nOldSel = m_nCurrentSel;
        m_nCurrentSel = INVALID_COLOR;
        DrawPickerCell (dc, nOldSel);
    }

	//
    // Set the current selection as row/col and draw (it will be drawn selected)
	//

    m_nCurrentSel = nIndex;
    DrawPickerCell (dc, m_nCurrentSel);

	//
    // Store the current colour
	//

	BOOL fValid = TRUE;
	COLORREF clr;
    if (m_nCurrentSel == CUSTOM_BOX_VALUE)
		clr = m_clrDefault;
    else if (m_nCurrentSel == DEFAULT_BOX_VALUE)
        clr = m_clrPicker = CLR_DEFAULT;
    else if (m_nCurrentSel == INVALID_COLOR)
	{
		clr = RGB (0, 0, 0);
		fValid = FALSE;
	}
	else
        clr = m_clrPicker = gm_sColors [m_nCurrentSel] .clrColor;
}

//-----------------------------------------------------------------------------
//
// @mfunc End the selection
//
// @parm BOOL | fOked | If TRUE, the user has selected a new color.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CColorPicker::EndPickerSelection (BOOL fOked)
{
    ::ReleaseCapture ();
	m_fOked = fOked;
}

//-----------------------------------------------------------------------------
//
// @mfunc Draw the given cell
//
// @parm CDC & | dc | Destination cell
//
// @parm int | nIndex | Index of the cell
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CColorPicker::DrawPickerCell (CDC &dc, int nIndex)
{

	//
	// Get the drawing rect
	//

	CRect rect;
	if (!GetPickerCellRect (nIndex, &rect)) 
		return;

	//
	// Get the text pointer and colors
	//

	LPCTSTR pszText;
	COLORREF clrBox;
	SIZE sizeMargin;
	SIZE sizeHiBorder;
	if (nIndex == CUSTOM_BOX_VALUE)
	{
		pszText = m_pszCustomText;
		sizeMargin = s_sizeTextMargin;
		sizeHiBorder = s_sizeTextHiBorder;
	}
	else if (nIndex == DEFAULT_BOX_VALUE)
	{
		pszText = m_pszDefaultText;
		sizeMargin = s_sizeTextMargin;
		sizeHiBorder = s_sizeTextHiBorder;
	}
	else
	{
		pszText = NULL;
		clrBox = gm_sColors [nIndex] .clrColor;
		sizeMargin = s_sizeBoxMargin;
		sizeHiBorder = s_sizeBoxHiBorder;
	}

	//
	// Based on the selectons, get our colors
	//

	COLORREF clrHiLight;
	COLORREF clrText;
	bool fSelected;
	if (m_nCurrentSel == nIndex)
	{
		fSelected = true;
		clrHiLight = m_clrHiLight;
		clrText = m_clrHiLightText;
	}
	else if (m_nChosenColorSel == nIndex)
	{
		fSelected = true;
		clrHiLight = m_clrLoLight;
		clrText = m_clrText;
	}
	else
	{
		fSelected = false;
		clrHiLight = m_clrLoLight;
		clrText = m_clrText;
	}

	//
	// Select and realize the palette
	//

	HPALETTE hpalOld = NULL;
	if (pszText == NULL)
	{
		if (m_palette .m_hPalette != NULL && 
			(dc .GetDeviceCaps (RASTERCAPS) & RC_PALETTE) != 0)
		{
			hpalOld = dc .SelectPalette (m_palette, FALSE);
			dc .RealizePalette ();
		}
	}

	//
	// If we are currently selected
	//

	if (fSelected)
	{

		//
		// If we have a background margin, then draw that
		//

		if (sizeMargin .cx > 0 || sizeMargin .cy > 0)
		{
			dc .SetBkColor (m_clrBackground);
			dc .ExtTextOut (0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);
			rect .InflateRect (- sizeMargin .cx, - sizeMargin .cy);
		}

		//
		// Draw the selection rectagle
		//

		dc .SetBkColor (m_clrHiLightBorder);
		dc .ExtTextOut (0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);
		rect .InflateRect (-1, -1);

		//
		// Draw the inner coloring
		//

		dc .SetBkColor (clrHiLight);
		dc .ExtTextOut (0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);
		rect .InflateRect (- (sizeHiBorder .cx - 1), - (sizeHiBorder .cy - 1));
	}

	//
	// Otherwise, we are not selected
	//

	else
	{
		
		//
		// Draw the background
		//

		dc .SetBkColor (m_clrBackground);
		dc .ExtTextOut (0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);
		rect .InflateRect ( 
			- (sizeMargin .cx + sizeHiBorder .cx), 
			- (sizeMargin .cy + sizeHiBorder .cy));
	}

	//
	// Draw custom text
	//

	if (pszText)
	{
		HFONT hfontOld = dc .SelectFont (m_font);
		dc .SetTextColor (clrText);
		dc .SetBkMode (TRANSPARENT);
		dc .DrawText (pszText, _tcslen (pszText), 
			&rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		dc .SelectFont (hfontOld);
    }        

	//
	// Otherwise, draw color
	//

	else
	{

		//
		// Draw color (ok, this code is bit sleeeeeezy. But the
		// area's that are being drawn are SO small, that nobody
		// will notice.)
		//

		dc .SetBkColor (::GetSysColor (COLOR_3DSHADOW));
		dc .ExtTextOut (0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);
		rect .InflateRect (-1, -1);
		dc .SetBkColor (gm_sColors [nIndex] .clrColor);
		dc .ExtTextOut (0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);
	}

	//
	// Restore the pallete
	//

	if (hpalOld && (dc .GetDeviceCaps (RASTERCAPS) & RC_PALETTE) != 0)
		dc .SelectPalette (hpalOld, FALSE);
}

//-----------------------------------------------------------------------------
//
// @mfunc Set the chosen color from the given color
//
// @parm UINT | uMsg | Message
//
// @parm WPARAM | wParam | Message w-parameter
//
// @parm LPARAM | lParam | Message l-parameter
//
// @parm BOOL & | bHandled | If handled, set to true
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

LRESULT CColorPicker::OnPickerKeyDown (UINT uMsg, 
	WPARAM wParam, LPARAM lParam, BOOL &bHandled) 
{

	//
	// Get the key data
	//
	
	UINT nChar = wParam;

	//
	// Get the offset for movement
	//

	int nOffset = 0;
	switch (nChar)
	{
		case VK_DOWN:
			nOffset = m_nNumColumns;
			break;

		case VK_UP:
			nOffset = -m_nNumColumns;
			break;

		case VK_RIGHT:
			nOffset = 1;
			break;

		case VK_LEFT:
			nOffset = -1;
			break;

		case VK_ESCAPE:
			m_clrPicker = m_clrCurrent;
			EndPickerSelection (FALSE);
			break;

		case VK_RETURN:
		case VK_SPACE:
			if (m_nCurrentSel == INVALID_COLOR)
				m_clrPicker = m_clrCurrent;
	        EndPickerSelection (m_nCurrentSel != INVALID_COLOR);
			break;
	}

	//
	// If we have an offset
	//

	if (nOffset != 0)
	{

		//
		// Based on our current position, compute a new position
		//

		int nNewSel;
		if (m_nCurrentSel == INVALID_COLOR)
			nNewSel = nOffset > 0 ? DEFAULT_BOX_VALUE : CUSTOM_BOX_VALUE;
		else if (m_nCurrentSel == DEFAULT_BOX_VALUE)
			nNewSel = nOffset > 0 ? 0 : CUSTOM_BOX_VALUE;
		else if (m_nCurrentSel == CUSTOM_BOX_VALUE)
			nNewSel = nOffset > 0 ? DEFAULT_BOX_VALUE : m_nNumColors - 1;
		else
		{
			nNewSel = m_nCurrentSel + nOffset;
			if (nNewSel < 0)
				nNewSel = DEFAULT_BOX_VALUE;
			else if (nNewSel >= m_nNumColors)
				nNewSel = CUSTOM_BOX_VALUE;
		}

		//
		// Now, for simplicity, the previous code blindly set new 
		// DEFAUT/CUSTOM indexes without caring if we really have those boxes.
		// The following code makes sure we actually map those values into
		// their proper locations. This loop will run AT the most, twice.
		//

		while (true)
		{
			if (nNewSel == DEFAULT_BOX_VALUE && !HasDefaultText ())
				nNewSel = nOffset > 0 ? 0 : CUSTOM_BOX_VALUE;
			else if (nNewSel == CUSTOM_BOX_VALUE && !HasCustomText ())
				nNewSel = nOffset > 0 ? DEFAULT_BOX_VALUE : m_nNumColors - 1;
			else
				break;
		}

		//
		// Set the new location
		//

        ChangePickerSelection (nNewSel);
	}
	bHandled = FALSE;
	return FALSE;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle a button up event
//
// @parm UINT | uMsg | Message
//
// @parm WPARAM | wParam | Message w-parameter
//
// @parm LPARAM | lParam | Message l-parameter
//
// @parm BOOL & | bHandled | If handled, set to true
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

LRESULT CColorPicker::OnPickerLButtonUp (UINT uMsg, 
	WPARAM wParam, LPARAM lParam, BOOL &bHandled) 
{

	//
	// Where did the button come up at?
	//

	CPoint pt (GET_X_LPARAM (lParam), GET_Y_LPARAM (lParam));
    int nNewSelection = PickerHitTest (pt);

	//
	// If valid, then change selection and end
	//

	if (nNewSelection != m_nCurrentSel)
		ChangePickerSelection (nNewSelection);
	EndPickerSelection (nNewSelection != INVALID_COLOR);
	return 0;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle mouse move
//
// @parm UINT | uMsg | Message
//
// @parm WPARAM | wParam | Message w-parameter
//
// @parm LPARAM | lParam | Message l-parameter
//
// @parm BOOL & | bHandled | If handled, set to true
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

LRESULT CColorPicker::OnPickerMouseMove (UINT uMsg, WPARAM wParam, 
	LPARAM lParam, BOOL &bHandled) 
{

	//
	// Do a hit test
	//

	CPoint pt (GET_X_LPARAM (lParam), GET_Y_LPARAM (lParam));
    int nNewSelection = PickerHitTest (pt);

	//
    // OK - we have the row and column of the current selection 
	// (may be CUSTOM_BOX_VALUE) Has the row/col selection changed? 
	// If yes, then redraw old and new cells.
	//

    if (nNewSelection != m_nCurrentSel)
        ChangePickerSelection (nNewSelection);
	return 0;
}

//-----------------------------------------------------------------------------
//
// @mfunc Handle a paint event
//
// @parm UINT | uMsg | Message
//
// @parm WPARAM | wParam | Message w-parameter
//
// @parm LPARAM | lParam | Message l-parameter
//
// @parm BOOL & | bHandled | If handled, set to true
//
// @rdesc Routine results
//
//-----------------------------------------------------------------------------

LRESULT CColorPicker::OnPickerPaint (UINT uMsg, 
	WPARAM wParam, LPARAM lParam, BOOL &bHandled) 
{
	CPaintDC dc (m_wndPicker);

	//
    // Draw raised window edge (ex-window style WS_EX_WINDOWEDGE is sposed to do this,
    // but for some reason isn't
	//

	CRect rect;
	m_wndPicker .GetClientRect (&rect);
	if (m_fPickerFlat)
	{
		CPen pen;
		pen .CreatePen (PS_SOLID, 0, ::GetSysColor (COLOR_GRAYTEXT));
		HPEN hpenOld = dc .SelectPen (pen);
		dc .Rectangle (rect .left, rect .top, 
			rect .Width (), rect .Height ());
		dc .SelectPen (hpenOld);
	}
	else
	{
		dc .DrawEdge (&rect, EDGE_RAISED, BF_RECT);
	}

	//
    // Draw the Default Area text
	// 
    if (HasDefaultText ())
        DrawPickerCell (dc, DEFAULT_BOX_VALUE);
 
	//
    // Draw colour cells
	// 

    for (int i = 0; i < m_nNumColors; i++)
        DrawPickerCell (dc, i);
    
	//
    // Draw custom text
	//

    if (HasCustomText ())
        DrawPickerCell (dc, CUSTOM_BOX_VALUE);
	return 0;
}

//-----------------------------------------------------------------------------
//
// @mfunc Do a hit test
//
// @parm const POINT & | pt | Point inside the window
//
// @rdesc Index/Item over or INVALID_COLOR
//
//-----------------------------------------------------------------------------

int CColorPicker::PickerHitTest (const POINT &pt)
{

	//
    // If we are in the custom text
	//

    if (m_rectCustomText .PtInRect (pt))
        return CUSTOM_BOX_VALUE;

	//
	// If we are in the default text
	//

    if (m_rectDefaultText .PtInRect (pt))
        return DEFAULT_BOX_VALUE;

	//
	// If the point isn't in the boxes, return invalid color
	//

	if (!m_rectBoxes .PtInRect (pt))
		return INVALID_COLOR;

	//
    // Convert the point to an index
	//

	int nRow = (pt .y - m_rectBoxes .top) / m_sizeBox .cy;
	int nCol = (pt .x - m_rectBoxes .left) / m_sizeBox .cx;
	if (nRow < 0 || nRow >= m_nNumRows || nCol < 0 || nCol >= m_nNumColumns)
		return INVALID_COLOR;
	int nIndex = nRow * m_nNumColumns + nCol;
	if (nIndex >= m_nNumColors)
		return INVALID_COLOR;
	return nIndex;
}

