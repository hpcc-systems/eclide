// sgrid.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "sgrid.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define PAGE_LOADED_STATUS_WIDTH 12

static const TCHAR SEISINTGRIDCLASS[] = _T("SGRID_Grid");

typedef ATL::CWinTraits<WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_HSCROLL | WS_VSCROLL, WS_EX_CLIENTEDGE> CGraphWinTraits;
class CSeisintGrid : public WTL::CDoubleBufferWindowImpl<CSeisintGrid, ATL::CWindow, CGraphWinTraits>
{
	typedef CSeisintGrid thisclass;
	typedef WTL::CDoubleBufferWindowImpl<CSeisintGrid, CWindow, CGraphWinTraits> baseClass;

protected:
	CGridCtrl m_view;
	CImageList m_imageList;
	CWnd m_wnd;
	CPageLoadedView m_pageLoaded;

	SGRID::GRIDCALLBACK m_pfnCallback;  
	LPARAM m_lParam;

	SGRID::GRIDCALLBACK_PREPCACHE m_pfnCallbackPrepCache;  
	LPARAM m_lParamPrepCache;                             

	SGRID::GRIDCALLBACK_CELLCLICK m_pfnCallbackCellClick;  
	LPARAM m_lParamCellClick;

public:
	DECLARE_WND_CLASS(NULL)
	CSeisintGrid() 
	{
		m_pfnCallback = NULL;
		m_lParam = 0;

		m_pfnCallbackPrepCache = NULL;
		m_lParamPrepCache = 0;

		m_pfnCallbackCellClick = NULL;
		m_lParamCellClick = 0;
	}

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		pMsg;
		return FALSE;
	}

    static BOOL CALLBACK GridCallback(GV_DISPINFO *pDispInfo, LPARAM /*lParam*/)
	{
		//CSeisintGrid * self = (CSeisintGrid * )lParam;
		pDispInfo->item.strText.Format(_T("Default %d,%d"), pDispInfo->item.row, pDispInfo->item.col);
		return TRUE;
	}

	static int CALLBACK CellCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
	{
		UNUSED_ALWAYS(lParamSort);

		CGridCellBase* pCell1 = (CGridCellBase*) lParam1;
		CGridCellBase* pCell2 = (CGridCellBase*) lParam2;
		if (!pCell1 || !pCell2) return 0;

		int nValue1 = _ttol(pCell1->GetText());
		int nValue2 = _ttol(pCell2->GetText());

		if (nValue1 == nValue2 == 0)
		{
			if (nValue1 < nValue2)
				return -1;
			else if (nValue1 == nValue2)
				return 0;
			else
				return 1;
		}

		return _tcscmp(pCell1->GetText(), pCell2->GetText());
	}

	BEGIN_MSG_MAP_EX(thisclass)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MSG_WM_NOTIFY(OnNotify)
		MESSAGE_HANDLER(WM_SETFONT, OnSetFont)
		MESSAGE_RANGE_HANDLER(SGRID::SGRID_START, SGRID::SGRID_LAST, OnSGridMessage);
		MSG_WM_SIZE(OnSize)
		MSG_WM_SETFOCUS(OnSetFocus)
		REFLECT_NOTIFICATIONS()
		CHAIN_MSG_MAP(baseClass)
	END_MSG_MAP()

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		m_wnd.Attach(*this);
		RECT r = {0, 0, 300, 200};
		m_view.Create(r, &m_wnd, 0, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_TABSTOP);
		m_view.EnableTitleTips(true);
		m_view.SetAutoSizeStyle(GVS_BOTH);
		m_view.SetFixedColumnSelection(false);
		m_view.SetFixedRowSelection(true);
		//m_view.EnableDragAndDrop(true);

		ModifyStyle(WS_BORDER, 0);
		ModifyStyleEx(WS_EX_CLIENTEDGE, 0);
		bHandled = false;
		return 0;
	}

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		m_view.DestroyWindow();
		m_wnd.Detach();
		return 0;
	}

	LRESULT OnNotify(int nId, LPNMHDR nmhdr)
	{
		SetMsgHandled(false);
		if (nId == m_view.GetDlgCtrlID())
		{
			SetMsgHandled(true);
			GV_DISPINFO *pDispInfo = (GV_DISPINFO*)nmhdr;
			switch(pDispInfo->hdr.code)
			{
			case GVN_GETDISPINFO:
				if(m_pfnCallback)
				{
					m_pfnCallback((SGRID::GV_DISPINFO*)pDispInfo, m_lParamPrepCache);
				}
				break;
			case GVN_ODCACHEHINT:
				if(m_pfnCallbackPrepCache)
				{
					SGRID::GV_CACHEHINT* cacheInfo = (SGRID::GV_CACHEHINT*)pDispInfo;
					m_pfnCallbackPrepCache(cacheInfo , m_lParamPrepCache);
				}
				break;
			case GVN_SELCHANGING:
				if (m_pfnCallbackCellClick)
				{
					CCellID cell(pDispInfo->item.row, pDispInfo->item.col);
					m_pfnCallbackCellClick((SGRID::GV_CELL*)&cell, m_lParamCellClick);
				}
				break;
			default:
				break;
			}
			return TRUE;
		}
		return FALSE;
	}

	void DoPaint(WTL::CDCHandle dc)
	{
		SetMsgHandled(false);
		if (m_pageLoaded.IsFullyLoaded())
		{
			return;
		}

		CRect r;
		GetClientRect(r);

		int yborder = GetSystemMetrics(SM_CYBORDER);

		int scrollbuttonheight = 0;
		if (m_view.IsVisibleVScroll())
		{
			scrollbuttonheight = GetSystemMetrics(SM_CYVSCROLL) + (2 * yborder);
		}
		else
		{
			scrollbuttonheight = yborder;
		}

		int hscrollwidth = 0;
		if (m_view.IsVisibleHScroll())
		{
			hscrollwidth = GetSystemMetrics(SM_CYHSCROLL) + (2 * yborder);
		}

		CRect loadedTop    (r.right - PAGE_LOADED_STATUS_WIDTH, r.top,															r.right, r.top + (yborder + scrollbuttonheight) - 1);
		CRect loadedContent(r.right - PAGE_LOADED_STATUS_WIDTH, r.top + (yborder + scrollbuttonheight) - 1,						r.right, r.bottom - (hscrollwidth + yborder + scrollbuttonheight) + 1);
		CRect loadedBottom (r.right - PAGE_LOADED_STATUS_WIDTH, r.bottom - (hscrollwidth + yborder + scrollbuttonheight) + 1,	r.right, r.bottom);

		DWORD win_color = GetSysColor(COLOR_3DFACE);
		dc.FillSolidRect(loadedTop, win_color);
		dc.FillSolidRect(loadedBottom , win_color);
		m_pageLoaded.DoPaint(dc, loadedContent);
	}

	LRESULT OnSetFont(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		LRESULT retVal = m_view.SendMessage(uMsg, wParam, lParam);
		return retVal;
	}

	LRESULT OnSGridMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		ATLASSERT(::IsWindow(m_view.GetSafeHwnd()));
		LRESULT lResult = 0;
		switch (uMsg)
		{
		case SGRID::SGRID_SETCALLBACKFUNC:
			m_pfnCallback = (SGRID::GRIDCALLBACK)wParam; 
			m_lParam = lParam; 
			m_view.SetVirtualMode(true);
			m_view.SetAutoSizeStyle(GVS_BOTH);
			m_view.SetFixedColumnSelection(FALSE);
			m_view.SetFixedRowSelection(true);
			break;
		case SGRID::SGRID_SETCALLBACKFUNC_PREPCACHE:
			m_pfnCallbackPrepCache = (SGRID::GRIDCALLBACK_PREPCACHE)wParam; 
			m_lParamPrepCache = lParam; 
			break;
		case SGRID::SGRID_SETCALLBACKFUNC_CELLCLICK:
			m_pfnCallbackCellClick = (SGRID::GRIDCALLBACK_CELLCLICK)wParam; 
			m_lParamCellClick = lParam; 
			break;
		case SGRID::SGRID_DOCOPY:
			m_view.OnEditCopy();
			break;
		case SGRID::SGRID_SETROWCOUNT:	//  Includes header
			if (wParam > 999999)
			{
				//Todo
				wParam = 999999;
				m_view.SetRowCount((int)wParam);
				m_pageLoaded.SetTotal((int)wParam);
			}
			else
			{
				m_view.SetRowCount((int)wParam);
				m_pageLoaded.SetTotal((int)wParam - 1);
			}
			break;
		case SGRID::SGRID_SETCOLUMNCOUNT:
			m_view.SetColumnCount((int)wParam);
			break;
		case SGRID::SGRID_SETFIXEDROWCOUNT:
			m_view.SetFixedRowCount((int)wParam);
			break;
		case SGRID::SGRID_SETFIXEDCOLUMNCOUNT:
			m_view.SetFixedColumnCount((int)wParam);
			break;
		case SGRID::SGRID_REDRAW:
			m_view.Refresh();
			break;
		case SGRID::SGRID_REDRAWROWS:
			for (int i = (int)wParam; i <= (int)lParam; ++i)
			{
				if (i >= 0 && i < m_pageLoaded.GetTotal())
					m_view.RedrawRow(i);
			}
			m_view.Invalidate();
			break;
		case SGRID::SGRID_SETROWSREQUESTED:
			m_pageLoaded.SetRequested((int)wParam, (int)lParam);
			Invalidate();
			break;
		case SGRID::SGRID_SETROWSLOADED:
			m_pageLoaded.SetLoaded((int)wParam, (int)lParam);
			Invalidate();
			if (m_pageLoaded.IsFullyLoaded())
			{
				m_view.SetHeaderSort(true);
				CRect r;
				GetClientRect(r);
				DoSize(r.Size());
			}
			break;
		case SGRID::SGRID_ISFULLYLOADED:
			lResult = m_pageLoaded.IsFullyLoaded();
			break;
		case SGRID::SGRID_GETITEMCOUNT:
			lResult = m_view.GetRowCount();
			break;
		case SGRID::SGRID_ENSUREVISIBLE:
			if (wParam >= (WPARAM)m_view.GetRowCount())
			{
				wParam = m_view.GetRowCount() - 1;
			}
			m_view.EnsureVisible((int)wParam, (int)lParam);

			break;
		case SGRID::SGRID_GETVISIBLEROWCOUNT:
			{
				RECT r;
				CCellRange range = m_view.GetVisibleNonFixedCellRange(&r, true);
				lResult = range.GetRowSpan();
			}
			break;
		case SGRID::SGRID_SELECTNONE:
			m_view.SelectNone();
			break;
		case SGRID::SGRID_SELECTALL:
			m_view.OnEditSelectAll();
			break;
		case SGRID::SGRID_SELECTROW:
			m_view.SelectRow((int)wParam, lParam != 0);
			break;
		case SGRID::SGRID_SELECTCOLUMN:
			m_view.SelectColumn((int)wParam);
			break;
		case SGRID::SGRID_GETCELLFROMPOINT:
			{
				SGRID::GV_CELL * cellStruct = (SGRID::GV_CELL * )wParam;
				PPOINT point = (PPOINT)lParam;
				cellStruct->cell = m_view.GetCellFromPt(*point);
			}
			break;
		case SGRID::SGRID_GETFOCUSCELL:
			{
				SGRID::GV_CELL * cellStruct = (SGRID::GV_CELL * )wParam;
				cellStruct->cell = m_view.GetFocusCell();
			}
			break;
		case SGRID::SGRID_SETFOCUSCELL:
			m_view.SetFocusCell((int)wParam, (int)lParam);
			break;
		case SGRID::SGRID_GETSELECTED:
			{
				SGRID::GV_CELLRANGE * cellRangeStruct = (SGRID::GV_CELLRANGE * )wParam;
				cellRangeStruct->range = m_view.GetSelectedCellRange();
			}
			break;
		case SGRID::SGRID_AUTOSIZEROWS:
			for (int i = (int)wParam; i <= (int)lParam; ++i)
			{
				m_view.AutoSizeRow(i);
			}
			m_view.Invalidate();
			break;
		case SGRID::SGRID_AUTOSIZECOLUMNS:
			for (int i = (int)wParam; i < (int)lParam; ++i)
			{
				m_view.AutoSizeColumn(i, GVS_DEFAULT, i == lParam - 1 ? true : false);
			}
			m_view.Invalidate();
			break;
		case SGRID::SGRID_SETFONT:
			{
				CString face = (const TCHAR *)wParam;
				CFont font;
				if (font.CreatePointFont((int)lParam*10, (const TCHAR *)wParam))
				{
					m_view.SetCellFont((HFONT)font.Detach());
				}
			}
			break;
		case SGRID::SGRID_SETIMAGELIST:
			{
				m_imageList.Attach((HIMAGELIST)wParam);
				m_view.SetImageList(&m_imageList);
			}
			break;
		default:
			ATLASSERT(!"Unknown Message");
			break;
		}
		return lResult;
	}

	void DoSize(const CSize & size)
	{
		UINT flags = SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSENDCHANGING;
		m_view.SetWindowPos(NULL, 0, 0, size.cx - (m_pageLoaded.IsFullyLoaded() ? 0 : PAGE_LOADED_STATUS_WIDTH), size.cy, flags);
	}

	void OnSize(UINT wParam, const CSize & size)
	{
		if( wParam != SIZE_MINIMIZED )
		{
			DoSize(size);
		}
	}

	void OnSetFocus(HWND)
	{
		m_view.SetFocus();
	}

//  ===================================================================
	static LONG_PTR PASCAL SWndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LONG_PTR lParam)
	{
		// Find C++ object associated with window.
		CSeisintGrid *sci = reinterpret_cast<CSeisintGrid *>(::GetWindowLongPtr(hWnd, 0));
		// sci will be zero if WM_CREATE not seen yet
		if (sci == 0) 
		{
			if (iMessage == WM_CREATE) 
			{
				// Create C++ object associated with window
				sci = new CSeisintGrid();
				sci->Attach(hWnd);
				::SetWindowLongPtr(hWnd, 0, reinterpret_cast<LONG_PTR>(sci));
				LRESULT lResult;
				if (!sci->ProcessWindowMessage(hWnd, iMessage, wParam, lParam, lResult, 0))
				{
					return ::DefWindowProc(hWnd, iMessage, wParam, lParam);
				}
				return lResult;
			} 
			else 
			{
				return ::DefWindowProc(hWnd, iMessage, wParam, lParam);
			}
		} 
		else 
		{
			if (iMessage == WM_NCDESTROY) 
			{
				::SetWindowLong(hWnd, 0, 0);
				sci->Detach();
				delete sci;
				return ::DefWindowProc(hWnd, iMessage, wParam, lParam);
			} 
			else 
			{
				LRESULT lResult;
				if (!sci->ProcessWindowMessage(hWnd, iMessage, wParam, lParam, lResult, 0))
				{
					return ::DefWindowProc(hWnd, iMessage, wParam, lParam);
				}
				return lResult;
			}
		}
	}
};
//  ===========================================================================
namespace SGRID
{
static HINSTANCE hInstance;
bool Register(HINSTANCE hInstance_)
{
	hInstance = hInstance_;

	WNDCLASSEX wndclass;
	//Check weather the class is registerd already
	if (!(::GetClassInfoEx(hInstance, SEISINTGRIDCLASS, &wndclass)))
	{
		//If not then we have to register the new class
		wndclass.cbSize = sizeof(wndclass);
		wndclass.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
		wndclass.lpfnWndProc = ::CSeisintGrid::SWndProc;
		wndclass.cbClsExtra = 0;
		wndclass.cbWndExtra = sizeof(CSeisintGrid *);
		wndclass.hInstance = hInstance;
		wndclass.hIcon = NULL;
		wndclass.hCursor = LoadCursor(0, IDC_ARROW);
		wndclass.hbrBackground = NULL;
		wndclass.lpszMenuName = NULL;
		wndclass.lpszClassName = SEISINTGRIDCLASS;
		wndclass.hIconSm = 0;

		if (!::RegisterClassEx(&wndclass))
		{
			return FALSE;
		}
	}

	return TRUE;
}

bool Unregister()
{
	return ::UnregisterClass(SEISINTGRIDCLASS, hInstance) != 0;
}
}