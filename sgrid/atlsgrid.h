#pragma once

#include "sdksgrid.h"

namespace SGRID
{
//------------------------------------------------------------------------------
template< class TBase >
class CGridCtrlT : public TBase
{
public:
	CGridCtrlT(HWND hWnd = NULL) : TBase(hWnd) { }

	CGridCtrlT< TBase >& operator=(HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}

	static HMODULE LoadDLL()
	{
		return ::LoadLibrary(GetLibraryName());
	}

	HWND Create(HWND hWndParent, ATL::_U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
		DWORD dwStyle = 0, DWORD dwExStyle = 0,
		ATL::_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL)
	{
		if(rect.m_lpRect == NULL)
			rect.m_lpRect = &rcDefault;
		m_hWnd = ::CreateWindowEx(dwExStyle, GetWndClassName(), szWindowName, dwStyle, rect.m_lpRect->left, rect.m_lpRect->top, (rect.m_lpRect->right - rect.m_lpRect->left) / 2, (rect.m_lpRect->bottom - rect.m_lpRect->top) / 2, hWndParent, MenuOrID.m_hMenu, NULL, lpCreateParam);
		return m_hWnd;
	}

    void SetCallbackFunc(GRIDCALLBACK pCallback, LPARAM lParam)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SGRID_SETCALLBACKFUNC,(WPARAM)pCallback,lParam);
	}

	void SetCallbackFuncPrepCache(GRIDCALLBACK_PREPCACHE pCallback, LPARAM lParam)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SGRID_SETCALLBACKFUNC_PREPCACHE,(WPARAM)pCallback,lParam);
	}

    void SetCallbackFuncCellClick(GRIDCALLBACK_CELLCLICK pCallback, LPARAM lParam)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SGRID_SETCALLBACKFUNC_CELLCLICK,(WPARAM)pCallback,lParam);
	}

	void DoCopy()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SGRID_DOCOPY, 0, 0);
	}

	void SetRowCount(unsigned int rows)	
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SGRID_SETROWCOUNT, rows + 1, 0);
	}

	void SetColumnCount(unsigned int cols)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SGRID_SETCOLUMNCOUNT, cols, 0);
	}

	void SetFixedRowCount(unsigned int fixedRows)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SGRID_SETFIXEDROWCOUNT, fixedRows, 0);
	}

	void SetFixedColumnCount(unsigned int fixedCols)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SGRID_SETFIXEDCOLUMNCOUNT, fixedCols, 0);
	}

    void Redraw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SGRID_REDRAW, 0, 0);
	}

	BOOL RedrawRows(int rowFrom, int rowTo)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SGRID_REDRAWROWS, rowFrom + 1, rowTo + 1);
	}

	BOOL SetRowsRequested(int rowFrom, int rowTo)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SGRID_SETROWSREQUESTED, rowFrom + 1, rowTo + 1);
	}

	BOOL SetRowsLoaded(int rowFrom, int rowTo)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SGRID_SETROWSLOADED, rowFrom + 1, rowTo + 1);
	}

	BOOL IsFullyLoaded()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, SGRID_ISFULLYLOADED, 0, 0);
	}

    void SelectNone()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SGRID_SELECTNONE, 0, 0);
	}

	void SelectAll()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SGRID_SELECTALL, 0, 0);
	}

    void SelectRow(int row, bool clearPrevious = true)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SGRID_SELECTROW, row + 1, (LPARAM)clearPrevious);
	}

    void SelectColumn(int col)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SGRID_SELECTCOLUMN, col, 0);
	}

    void SelectCell(int row, int col)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SGRID_SELECTCELL, row + 1, col);
	}

	CCellID GetFocusCell() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		GV_CELL cellStruct;
		::SendMessage(m_hWnd, SGRID_GETFOCUSCELL, (WPARAM)&cellStruct, 0);
		cellStruct.cell.row--;
		return cellStruct.cell;
	}

	void SetFocusCell(int row, int col)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SGRID_SETFOCUSCELL, (WPARAM)row + 1, (LPARAM)col);
	}

	CCellID GetCellFromPoint(const CPoint & pt)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		GV_CELL cellStruct;
		POINT point;
		point.x = pt.x;
		point.y = pt.y;
		::SendMessage(m_hWnd, SGRID_GETCELLFROMPOINT, (WPARAM)&cellStruct, (LPARAM)&point);
		cellStruct.cell.row--;
		return cellStruct.cell;
	}

	CCellRange GetSelected() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		GV_CELLRANGE cellRangeStruct;
		::SendMessage(m_hWnd, SGRID_GETSELECTED, (WPARAM)&cellRangeStruct, 0);
		cellRangeStruct.range.Set(cellRangeStruct.range.GetMinRow() - 1, cellRangeStruct.range.GetMinCol(), cellRangeStruct.range.GetMaxRow() - 1, cellRangeStruct.range.GetMaxCol());
		return cellRangeStruct.range;
	}

	void AutoSizeRows(int from, int to)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SGRID_AUTOSIZEROWS, from + 1, to + 1);
	}

	void AutoSizeColumns(int fromCol, int toCol)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SGRID_AUTOSIZECOLUMNS, fromCol, toCol);
	}

	void SetFont(const std::_tstring & font, int size)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SGRID_SETFONT, (WPARAM)font.c_str(), size);
	}

	void SetImageList(HIMAGELIST imgList)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SGRID_SETIMAGELIST, (WPARAM)imgList, 0);
	}

	//  --ListViewCtrl style msgs----------------------------------------------
	int GetItemCount() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, SGRID_GETITEMCOUNT, 0, 0L) - 1;
	}
	void EnsureVisible(int nRow, int nCol)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, SGRID_ENSUREVISIBLE, nRow + 1, nCol);
	}
	int GetVisibleItemCount() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, SGRID_GETVISIBLEROWCOUNT, 0, 0L);
	}
	void SetColumnWidth(int cxWidth)
	{
		/*
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, LB_SETCOLUMNWIDTH, cxWidth, 0L);
		*/
	}

	//  -----------------------------------------------------------------------

	static LPCTSTR GetWndClassName()
	{
		return _T("SGRID_Grid");
	}

	static LPCTSTR GetLibraryName()
	{
		return _T("SGRID.DLL");
	}
};

typedef CGridCtrlT<CWindow> CGridCtrl;

} // namespace SGRID