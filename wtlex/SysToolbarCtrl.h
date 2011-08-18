// SysToolbarCtrl.h: Toolbar helper class CSysToolbarCtrl.
// Should compile only when used as a mix-in class for a
// WTL::CFrameWindowImpl<T> based class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYSTOOLBARCTRL_H__ABD82C39_37E1_4A9B_903A_14C1DFADA70F__INCLUDED_)
#define AFX_SYSTOOLBARCTRL_H__ABD82C39_37E1_4A9B_903A_14C1DFADA70F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// padding for TBBUTTON alignment:
#ifdef _WIN64
#define TBBPADDING 0, 0, 0, 0, 0, 0,
#elif defined(_WIN32)
#define TBBPADDING 0, 0,
#else
#define TBBPADDING
#endif

// Generic button:
#define STBB(Img, Cmd, Bmp) {Img, Cmd, TBSTATE_ENABLED, TBSTYLE_BUTTON, TBBPADDING Bmp, 0}

// Separator:
#define STBB_SEPARATOR {0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, TBBPADDING 0L, 0}

// History buttons (all without standard command IDs):
#define STBB_HIST_ADDTOFAVORITES(Cmd)	STBB(HIST_ADDTOFAVORITES, Cmd, IDB_HIST_SMALL_COLOR)
#define STBB_HIST_BACK(Cmd)				STBB(HIST_BACK, Cmd, IDB_HIST_SMALL_COLOR)
#define STBB_HIST_FAVORITES(Cmd)		STBB(HIST_FAVORITES, Cmd, IDB_HIST_SMALL_COLOR)
#define STBB_HIST_FORWARD(Cmd)			STBB(HIST_FORWARD, Cmd, IDB_HIST_SMALL_COLOR)
#define STBB_HIST_VIEWTREE(Cmd)			STBB(HIST_VIEWTREE, Cmd, IDB_HIST_SMALL_COLOR)

// Standard buttons (based on command IDs in <atlres.h>):
#define STBB_STD_COPY				STBB(STD_COPY, ID_EDIT_COPY, IDB_STD_SMALL_COLOR)
#define STBB_STD_CUT				STBB(STD_CUT, ID_EDIT_CUT, IDB_STD_SMALL_COLOR)
// (No standard Delete command in <atlres.h>)
#define STBB_STD_DELETE(Cmd)		STBB(STD_DELETE, Cmd, IDB_STD_SMALL_COLOR)
#define STBB_STD_FILENEW			STBB(STD_FILENEW, ID_FILE_NEW, IDB_STD_SMALL_COLOR)
#define STBB_STD_FILEOPEN			STBB(STD_FILEOPEN, ID_FILE_OPEN, IDB_STD_SMALL_COLOR)
#define STBB_STD_FILESAVE			STBB(STD_FILESAVE, ID_FILE_SAVE, IDB_STD_SMALL_COLOR)
#define STBB_STD_FIND				STBB(STD_FIND, ID_EDIT_FIND, IDB_STD_SMALL_COLOR)
#define STBB_STD_HELP				STBB(STD_HELP, ID_CONTEXT_HELP, IDB_STD_SMALL_COLOR)
#define STBB_STD_PASTE				STBB(STD_PASTE, ID_EDIT_PASTE, IDB_STD_SMALL_COLOR)
#define STBB_STD_PRINT				STBB(STD_PRINT, ID_FILE_PRINT, IDB_STD_SMALL_COLOR)
#define STBB_STD_PRINTPRE			STBB(STD_PRINTPRE, ID_FILE_PRINT_PREVIEW, IDB_STD_SMALL_COLOR)
// (No standard Properties command in <atlres.h>)
#define STBB_STD_PROPERTIES(Cmd)	STBB(STD_PROPERTIES, Cmd, IDB_STD_SMALL_COLOR)
#define STBB_STD_REDOW				STBB(STD_REDOW, ID_EDIT_REDO, IDB_STD_SMALL_COLOR)
#define STBB_STD_REPLACE			STBB(STD_REPLACE, ID_EDIT_REPLACE, IDB_STD_SMALL_COLOR)
#define STBB_STD_UNDO				STBB(STD_UNDO, ID_EDIT_UNDO, IDB_STD_SMALL_COLOR)

// View buttons (all without standard command IDs):
#define STBB_VIEW_DETAILS(Cmd)			STBB(VIEW_DETAILS, Cmd, IDB_VIEW_SMALL_COLOR)
#define STBB_VIEW_LARGEICONS(Cmd)		STBB(VIEW_LARGEICONS, Cmd, IDB_VIEW_SMALL_COLOR)
#define STBB_VIEW_LIST(Cmd)				STBB(VIEW_LIST, Cmd, IDB_VIEW_SMALL_COLOR)
#define STBB_VIEW_NETCONNECT(Cmd)		STBB(VIEW_NETCONNECT, Cmd, IDB_VIEW_SMALL_COLOR)
#define STBB_VIEW_NETDISCONNECT(Cmd)	STBB(VIEW_NETDISCONNECT, Cmd, IDB_VIEW_SMALL_COLOR)
#define STBB_VIEW_NEWFOLDER(Cmd)		STBB(VIEW_NEWFOLDER, Cmd, IDB_VIEW_SMALL_COLOR)
#define STBB_VIEW_PARENTFOLDER(Cmd)		STBB(VIEW_PARENTFOLDER, Cmd, IDB_VIEW_SMALL_COLOR)
#define STBB_VIEW_SMALLICONS(Cmd)		STBB(VIEW_SMALLICONS, Cmd, IDB_VIEW_SMALL_COLOR)
#define STBB_VIEW_SORTDATE(Cmd)			STBB(VIEW_SORTDATE, Cmd, IDB_VIEW_SMALL_COLOR)
#define STBB_VIEW_SORTNAME(Cmd)			STBB(VIEW_SORTNAME, Cmd, IDB_VIEW_SMALL_COLOR)
#define STBB_VIEW_SORTSIZE(Cmd)			STBB(VIEW_SORTSIZE, Cmd, IDB_VIEW_SMALL_COLOR)
#define STBB_VIEW_SORTTYPE(Cmd)			STBB(VIEW_SORTTYPE, Cmd, IDB_VIEW_SMALL_COLOR)
#if (_WIN32_IE >= 0x0400)
#define STBB_VIEW_VIEWMENU(Cmd)			STBB(VIEW_VIEWMENU, Cmd, IDB_VIEW_SMALL_COLOR)
#endif

typedef dockwins::CDockingFrameTraitsT< dockwins::CVC7LikeAutoHidePaneTraits, dockwins::CSimpleSplitterBar<3>,
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		WS_EX_APPWINDOW | WS_EX_WINDOWEDGE> CMainFrameTraits;

template <class T>
class ATL_NO_VTABLE CSysToolbarCtrl
{
	typedef dockwins::CAutoHideMDIDockingFrameImpl<T, WTL::CMDIWindow, CMainFrameTraits, dockwins::CVC7LikeAutoHidePaneTraits> frameWndClass;
	typedef typename frameWndClass::_AtlToolBarData toolbarData;

protected:
	static const SIZE BMPSIZES[2];
	enum {BMP_SMALL, BMP_LARGE};

public:
	HWND CreateSysToolbarCtrl(HWND hWndParent,			// Parent window handle
	  TBBUTTON* pItems,									// An array of toolbar items
	  int nItems,										// Number of array items
	  CTabbedMDICommandBarCtrl* pCmdBar = NULL,			// Optional command bar pointer
	  int bmpSize = BMP_SMALL,							// Bitmap size
	  DWORD dwStyle = ATL_SIMPLE_TOOLBAR_PANE_STYLE,	// Toolbar style
	  UINT nID = ATL_IDW_TOOLBAR)						// Control ID
	{
		// create toolbar control
		AtlInitCommonControls(ICC_BAR_CLASSES);			// (just making sure)
		HWND hWnd = ::CreateWindowEx(0, TOOLBARCLASSNAME, NULL, dwStyle, 0, 0, 100, 100, hWndParent, (HMENU)LongToHandle(nID), _Module.GetResourceInstance(), NULL);
		if (hWnd == NULL)
			return NULL;
		CToolBarCtrl wndToolbar = hWnd;
		wndToolbar.SetButtonStructSize();

		// Set initial button size
		wndToolbar.SetBitmapSize(BMPSIZES[bmpSize].cx, BMPSIZES[bmpSize].cy);
		wndToolbar.SetButtonSize(BMPSIZES[bmpSize].cx + 7, BMPSIZES[bmpSize].cy + 7);

		// build toolbar buttons
		int iImgListOffsets[3] = {-1, -2, -3},		// Offsets to std bitmaps in toolbar's image list
			iStdImgList;							// selector for iImgListOffsets
		for (int i = 0; i < nItems; i++)
		{
			if (pItems[i].idCommand != 0)
			{
				// Select large or small bitmap using bmpSize
				pItems[i].dwData &= ~1;
				pItems[i].dwData |= bmpSize;

				// Determine which image list offset to use (modulo 3 just in case...)
				iStdImgList = (pItems[i].dwData >> 2) % 3;

				// if the standard bitmap isn't loaded yet, load it
				// TO DO: Does toolbar control release bitmaps when destroyed?
				if (iImgListOffsets[iStdImgList] < 0)
					iImgListOffsets[iStdImgList] = wndToolbar.LoadStdImages(pItems[i].dwData);

				// button image index = standard bitmap index + image index
				pItems[i].iBitmap += iImgListOffsets[iStdImgList];

				// Add the current bitmap to the command bar
				if (pCmdBar && pCmdBar->m_hWnd)
				{
					HDC hdc = GetDC(pCmdBar->m_hWnd);
					HBITMAP hBitmap = CreateCompatibleBitmap(hdc, BMPSIZES[bmpSize].cx, BMPSIZES[bmpSize].cy);
					HDC hdcMem = CreateCompatibleDC(hdc);
					HANDLE hOld = SelectObject(hdcMem, hBitmap);
					CImageList cImgList = wndToolbar.GetImageList();
					cImgList.SetBkColor(pCmdBar->GetImageMaskColor());
					cImgList.Draw(hdcMem, pItems[i].iBitmap, 0, 0, 0);
					SelectObject(hdcMem, hOld);
					DeleteDC(hdcMem);
					pCmdBar->SetImageSize(BMPSIZES[bmpSize].cx,	BMPSIZES[bmpSize].cy);
					pCmdBar->AddBitmap(hBitmap, pItems[i].idCommand);
					DeleteObject(hBitmap);
				}
			}
		}
		wndToolbar.AddButtons(nItems, pItems);

		// check if font is taller than our bitmaps
		CFontHandle font = (HFONT)::SendMessage(hWnd, WM_GETFONT, 0, 0L);
		if(font.IsNull())
			font = AtlGetDefaultGuiFont();
		LOGFONT lf;
		font.GetLogFont(lf);
		WORD cyFontHeight = (WORD)abs(lf.lfHeight);
		int cyBmp = std::max<int>(BMPSIZES[bmpSize].cy, cyFontHeight);

		// Size the buttons
		wndToolbar.SetBitmapSize(BMPSIZES[bmpSize].cx, cyBmp);
		wndToolbar.SetButtonSize(BMPSIZES[bmpSize].cx + 7, cyBmp + 7);

		return hWnd;
	}
};

template <class T>
const SIZE CSysToolbarCtrl<T>::BMPSIZES[2] = {
	{16, 15},
	{24, 23}
};


#endif // !defined(AFX_SYSTOOLBARCTRL_H__ABD82C39_37E1_4A9B_903A_14C1DFADA70F__INCLUDED_)
