#pragma once

#include "clib.h"
#include "GridMixIn.h"
#include "GridSelection.h"
//#include "GridFocus.h"
#include "ResultFormat.h"
#include "Dali.h"
#include "MouseTooltip.h"
#include "atlsgrid.h"

#define PAGE_SIZE 100
#define PROP_TEXT_INDENT 2

__interface IDataSourceListener : public IUnknown
{
	bool DataLoaded(__int64 start, int count, __int64 total);
};
typedef StlLinked<IDataSourceListener> IDataSourceListenerAdapt;

__interface IDataSource : public IUnknown
{
	void SetLoaded(__int64 start, int count);
	const TCHAR * GetCell(__int64 row, int col);
	std::pair<__int64, __int64> PrepCache(__int64 rowFrom, __int64 rowTo, bool bHighPriority);
	__int64 GetRowCount();
	__int64 GetPhysicalRow(__int64 disprow);
	__int64 GetPhysicalRowCount() const;
	void AddRows(int count);
	IUnknown * GetCellUnknown(__int64 row, unsigned col) const;
	void SetCellUnknown(__int64 row, unsigned col, IUnknown * data);
	void SetCell(__int64 row, int col, const std::_tstring & val);
	void SetColumn(unsigned col, const std::_tstring & val, COLUMN_TYPE_ENUM type = CT_STRING);
	__int64 GetRowsLoaded();
	bool IsFullyLoaded();
	int GetColCount();
	void AddListener(IDataSourceListener *listener, bool bAdd=true);
	void Sort(unsigned int col);
	bool IsColQuoted(int col) const;
};

class IDataSourceImpl : public IDataSource, CUnknown
{
public:
	IMPLEMENT_CUNKNOWN;

	void SetLoaded(__int64 /*start*/, int /*count*/) {}
	const TCHAR * GetCell(__int64 /*row*/, int /*col*/) { return 0; }
	std::pair<__int64, __int64> PrepCache(__int64 /*rowFrom*/, __int64 /*rowTo*/, bool /*bHighPriority*/) { return std::make_pair(0, 0); }
	__int64 GetRowCount() { return 0; }
	__int64 GetPhysicalRow(__int64 disprow) { return 0; };
	__int64 GetPhysicalRowCount() const { return 0; };
	void AddRows(int count) { };
	IUnknown * GetCellUnknown(__int64 row, unsigned col) const { return 0; }
	void SetCellUnknown(__int64 row, unsigned col, IUnknown * data) { };
	void SetCell(__int64 row, int col, const CString & val) { };
	void SetColumn(unsigned col, const std::_tstring & val, COLUMN_TYPE_ENUM type = CT_STRING) { };
	__int64 GetRowsLoaded() { return 0; }
	bool IsFullyLoaded() {return false; };
	int GetColCount() { return 0; }
	void AddListener(IDataSourceListener * /*listener*/, bool /*bAdd*/) { return; }
	void Sort(unsigned int /*col*/) { } ;
};

//  ===========================================================================
class CCellState : public CUnknown
{
	WTL::CLogFont m_LogFont;
    COLORREF m_TextColor, m_BkColor;
public:
	IMPLEMENT_CUNKNOWN;

	CCellState(const WTL::CLogFont& font, COLORREF fgcol, COLORREF bgcol)
		: m_LogFont(font), m_TextColor(fgcol), m_BkColor(bgcol)
	{
	}
	
	void SetLogFont(const WTL::CLogFont& font)
	{
		m_LogFont=font;
	}

	LOGFONT& GetLogFont()
	{
		return m_LogFont;
	}

	bool IsBold()
	{
		return m_LogFont.IsBold();
	}

	bool IsItalic()
	{
		return m_LogFont.lfItalic != 0;
	}

	void SetTextColor(COLORREF fgcol)
	{
		m_TextColor=fgcol;
	}

	COLORREF GetTextColor()
	{
		return m_TextColor;
	}

	void SetBkColor(COLORREF bgcol)
	{
		m_BkColor=bgcol;
	}

	COLORREF GetBkColor()
	{
		return m_BkColor;
	}
};
//  ===========================================================================
typedef std::vector<int> IntVector;
class CGridCtrlEx : 
	public SGRID::CGridCtrl,
	public IDataSourceListener,
	public CUnknown,
	public boost::signals::trackable

{
	typedef CGridCtrlEx thisClass;
	typedef SGRID::CGridCtrl baseClass;

protected:
	typedef StlLinked<CResultFormatter> CResultFormatterAdapt;
	typedef std::map<int, CResultFormatterAdapt> ColumnFormatter;
	ColumnFormatter m_columnFormatter;
	std::_tstring m_strCellData;

	CComPtr<IDataSource> m_data;
	bool m_trim;

	WTL::CLogFont m_cellLogFont;
	bool m_cellFontSet;
    COLORREF m_cellTextColor, m_cellBkColor;

	bool m_firstPageAutoSize;

	IntVector m_hiddenColMapping;

public:
	IMPLEMENT_CUNKNOWN;
	CGridCtrlEx()
	{
		m_cellFontSet = false;
		m_firstPageAutoSize = true;
		m_cellTextColor = ::GetSysColor(COLOR_WINDOWTEXT);
		m_cellBkColor = ::GetSysColor(COLOR_WINDOW);
	}

	~CGridCtrlEx()
	{
	}

	bool SetCellFont(const std::_tstring & font, int size)
	{
		m_cellFontSet = false;
		WTL::CFont cellFont;
		if(cellFont.CreatePointFont((int)size*10, font.c_str()))
		{
			cellFont.GetLogFont(m_cellLogFont);
			m_cellFontSet = true;
		}
		return m_cellFontSet;
	}

	int DisplayToDataColumn(int dispidx) const
	{
		//IntVector::const_iterator itr = m_hiddenColMapping.find(dispidx);
		if (dispidx >= 0 && dispidx < (int)m_hiddenColMapping.size())
			return m_hiddenColMapping[dispidx];
		else
			return dispidx;
	}

	int inline GetColCount()
	{
		return m_hiddenColMapping.size();
	}

	__int64 inline GetRowCount()
	{
		return m_data->GetRowCount();
	}

	void SetTrim(bool trim)
	{
		m_trim = trim;
	}

	// hex formatting is stored in ResultView array so assume we get physical column here
	void SetColumnFormatter(int column, CResultFormatter *pformatter)
	{
		StlLinked<CResultFormatter> formatter = pformatter;
		m_columnFormatter[column] = formatter;
	}

	const TCHAR *GetCellTextFormatted(__int64 row, int column)
	{
		const TCHAR *cellString = m_data->GetCell(row, column);
		// TJ don't format header row (looks funny when column header is in hex)
		if(row>=0)
		{
			if (column == 0 && *cellString == NULL)
			{
				m_data->SetCell(row, column, boost::lexical_cast<std::_tstring>(row + 1).c_str());
				cellString = m_data->GetCell(row, column);
			}
			ColumnFormatter::const_iterator i = m_columnFormatter.find(column);
			if ( i != m_columnFormatter.end() )
			{
				const CResultFormatter *f = i->second.get();
				if ( f )
					cellString = f->Format(cellString, m_strCellData, m_data->IsColQuoted(column));
			}
		}
		return cellString;
	}

	bool ConfirmFullyLoaded()
	{
		if(IsFullyLoaded())
			return true;

		CString msg;
		msg.Format(_T("This feature is unavailable until all results have been downloaded.\r\nDo you wish to download %I64d of %I64d in the background?"), m_data->GetRowCount() - m_data->GetRowsLoaded(), m_data->GetRowCount());
		if ( IDOK == MessageBox(msg, _T("ECL IDE"), MB_OKCANCEL | MB_ICONQUESTION | MB_DEFBUTTON1))
			DownloadAll();
		return false;
	}

	BOOL CALLBACK GridCallback(SGRID::GV_DISPINFO *pDispInfo)
	{
		int column=DisplayToDataColumn(pDispInfo->item.col);
		int row = pDispInfo->item.row - 1;
		//ATLTRACE(_T("Disp Row: %i, Actual Row:  %i\n"), pDispInfo->item.row - 1, row);

		if(row>=0 && column>0) // drawing a cell?
		{
			CCellState* cs=(CCellState*)(m_data->GetCellUnknown(row, column)); 
			if(cs!=0)
			{
				pDispInfo->item.crBkClr=cs->GetBkColor();
				pDispInfo->item.crFgClr=cs->GetTextColor();
				pDispInfo->item.lfFont=cs->GetLogFont();
			}
			else
			{
				pDispInfo->item.crBkClr=m_cellBkColor;
				pDispInfo->item.crFgClr=m_cellTextColor;
				pDispInfo->item.lfFont=m_cellLogFont;
			}
		}

		pDispInfo->item.strText = GetCellTextFormatted(row, column);
		if (m_trim)
		{
			pDispInfo->item.strText.TrimRight();
		}
		return TRUE;
	}

	BOOL CALLBACK GridCallbackPrepCache(SGRID::GV_CACHEHINT *pDispInfo)
	{
		int colcount = m_data->GetColCount();
		if(colcount > (int)m_hiddenColMapping.size()) // good time to allocate columns array?
		{
			for(int i = m_hiddenColMapping.size(); i < colcount; ++i)
				m_hiddenColMapping.push_back(i);
		}

		int from = pDispInfo->range.GetMinRow() - 1;
		int to = pDispInfo->range.GetMaxRow() - 1;

		if (from >= 0 && to >= 0)
		{
			std::pair<__int64, __int64> result = m_data->PrepCache(from, to, true);
			if (result.first != -1 && result.second != -1)
			{
				baseClass::SetRowsRequested(result.first, result.second);
			}
		}
		return TRUE;
	}

	void DownloadAll()
	{
		for(int i = 0; i < m_data->GetRowCount(); i += PAGE_SIZE)
		{
			std::pair<__int64, __int64> result = m_data->PrepCache(i, i + PAGE_SIZE - 1, false);
			if (result.first != -1 && result.second != -1)
			{
				baseClass::SetRowsRequested(result.first, result.second);
			}
		}
	}

	BOOL CALLBACK GridCallbackCellClick(SGRID::GV_CELL *pCell)
	{
		if (pCell->cell.row == 0)
		{
			if(ConfirmFullyLoaded())
			{
				m_data->Sort(DisplayToDataColumn(pCell->cell.col));
				AutoSizeRows(0, GetRowCount()-1);
			}
		}
		return TRUE;
	}

	void SelectRow()
	{
		__int64 row = GetFocusCell().row;
		baseClass::SelectRow(row);
	}

	const CCellRange GetGridSelection() const
	{ 
		return baseClass::GetSelected();
	}

	void SetDataSource(IDataSource * data)
	{
		m_data = data;
		m_data->AddListener(this,true);
	}

	void SelectColumn()
	{
		int column = GetFocusCell().col;
		baseClass::SelectColumn(column);
	}

	void SelectAll()
	{
		if(!ConfirmFullyLoaded())
			return;
		baseClass::SelectAll();
	}

	bool DataLoaded(__int64 start, int count, __int64 total) 
	{
		if ( count )
		{
			int from = start;
			int to = from + count - 1;
			baseClass::SetRowsLoaded(from, to);
			baseClass::RedrawRows(from, to);
			if (m_firstPageAutoSize && (start + count) >= GetVisibleItemCount())
			{
				AutoSizeColumns(0, m_data->GetColCount());
				m_firstPageAutoSize = false;
			}
			//if(from == to == 1)
			//{
				//AutoSizeColumns(0, m_data->GetColCount() - 1);
			//}
		}
		//return false when we want to stop listening (when we have all the data!)
		return (m_data->GetRowsLoaded() < total);
	}
	bool EnsureColumnVisible(int /*nColumn*/, bool /*bPartialOK*/)
	{
		/*
		CHeaderCtrl header(GetDlgItem(0));
		//ASSERT(pHeader->IsKindOf(RUNTIME_CLASS(CHeaderCtrl)));

		CRect rcHeader;
		Header_GetItemRect(header.m_hWnd, nColumn, &rcHeader);

		CRect rcClient;
		GetClientRect(&rcClient);

		int nOffset = GetScrollPos(SB_HORZ);

		if(bPartialOK)
		{
			if((rcHeader.left - nOffset < rcClient.right) && (rcHeader.right - nOffset > 0))
			{
				return true;
			}
		}

		int nScrollX = 0;

		if((rcHeader.Width() > rcClient.Width()) || (rcHeader.left - nOffset < 0))
		{
			nScrollX = rcHeader.left - nOffset;
		}
		else if(rcHeader.right - nOffset > rcClient.right)
		{
			nScrollX = rcHeader.right - nOffset - rcClient.right;
		}

		if(nScrollX != 0)
		{
			CSize size(nScrollX, 0);
			Scroll(size);
		}

		return true;
		*/

		return true;
	}

	void GotoLine(__int64 line, int col)
	{
		if ( line < 0 )	line = 0;
		if ( col < 0 ) col = 0;
		baseClass::SetFocusCell(line, col);
		baseClass::EnsureVisible(line, col);
	}

	__int64 CurrentLine()
	{ 
		return baseClass::GetFocusCell().row;
	}

	int HitTestCharInCell(const POINT &/*pnt*/, __int64 /*line*/, __int64 /*col*/)
	{
		/*
		POINT localPnt = pnt;
		ScreenToClient(&localPnt);

		//don't bother with this
		//localPnt.x += GetScrollPos(SB_HORZ);

		RECT rc;
		if ( GetSubItemRect(static_cast<int>(line), static_cast<int>(col), LVIR_BOUNDS, &rc) )
		{
			std::_tstring cell = GetCellTextFormatted(line,col);
			//draw the text until we pass the pt.x
			SIZE size;
			int  maxChars = 0;

			CFontHandle font(GetFont());
			CDC dc = GetDC();

			HFONT old = dc.SelectFont(font);
			BOOL found = dc.GetTextExtentExPoint(cell.c_str(),cell.length(),&size,localPnt.x-rc.left,&maxChars);
			dc.SelectFont(old);

			if ( found )
				return maxChars;
		}
		return -1; //not found
		*/
		return -1;
	}

	__int64 HitTestForColumn(const POINT &pnt, int *pCol)
	{
		CCellID cell = baseClass::GetCellFromPoint(pnt);
		*pCol = cell.col;
		return cell.row;
	}

	int FindColumnFromXPos(int /*x*/)
	{
		/*
		CHeaderCtrl header(GetDlgItem(0));
		x += GetScrollPos(SB_HORZ);

		CRect rcHeader;
		for (int i=0; i<GetColCount(); i++)
		{
			Header_GetItemRect(header.m_hWnd, i, &rcHeader);
			if ( x >= rcHeader.left && x < rcHeader.right )
			{
				return (int)i;
			}
		}
		return -1;
		*/
		return -1;
	}
	bool HasSelection()
	{
		CCellRange range = GetSelected();
		return range.IsValid() && range.Count() > 0;
	}

	void DeleteColumns(int startidx, int count)
	{
		if((int)m_hiddenColMapping.size() < startidx + count)
		{
			IntVector::iterator start_itr = m_hiddenColMapping.begin() + startidx;
			IntVector::iterator end_itr = m_hiddenColMapping.begin() + startidx + count;
			ATLASSERT(start_itr != m_hiddenColMapping.end());
			ATLASSERT(end_itr != m_hiddenColMapping.end());
			m_hiddenColMapping.erase(start_itr, end_itr);
			//memmove(m_hiddenColMapping+startidx,m_hiddenColMapping+startidx+count,sizeof(int)*(m_hiddenColMapping.size()-startidx-count));  
		}
		SetColumnCount(m_hiddenColMapping.size());
		Redraw();
	}

	void SetSelectionTextColor(COLORREF clr)
	{
		CCellRange gridSel = GetGridSelection();
		if (gridSel.IsValid())
		{
			for(int c = gridSel.GetMinCol(); c <= gridSel.GetMaxCol(); c++)
			{
				int col = DisplayToDataColumn(c);
				for(__int64 r = gridSel.GetMinRow(); r <= gridSel.GetMaxRow(); r++)
				{
					CCellState* oldcs=(CCellState*)(m_data->GetCellUnknown(r, col));
					CCellState* cs;
					if(oldcs!=0)
						cs=new CCellState(oldcs->GetLogFont(), clr, oldcs->GetBkColor()); 
					else
						cs=new CCellState(m_cellLogFont, clr, m_cellBkColor); 
					if(cs)
						m_data->SetCellUnknown(r, col, cs);
				}
			}
		}
		Redraw();
	}

	void SetSelectionBkColor(COLORREF clr)
	{
		CCellRange gridSel = GetGridSelection();
		if (gridSel.IsValid())
		{
			for(int c = gridSel.GetMinCol(); c <= gridSel.GetMaxCol(); c++)
			{
				int col = DisplayToDataColumn(c);
				for(__int64 r = gridSel.GetMinRow(); r <= gridSel.GetMaxRow(); r++)
				{
					CCellState* oldcs=(CCellState*)(m_data->GetCellUnknown(r, col));
					CCellState* cs;
					if(oldcs!=0)
						cs=new CCellState(oldcs->GetLogFont(), oldcs->GetTextColor(), clr); 
					else
						cs=new CCellState(m_cellLogFont, m_cellTextColor, clr); 
					if(cs)
						m_data->SetCellUnknown(r, col, cs);
				}
			}
		}
		Redraw();
	}

	void SetSelectionFont(const WTL::CLogFont& font)
	{
		CCellRange gridSel = GetGridSelection();
		if (gridSel.IsValid())
		{
			bool fullrow=gridSel.GetRowSpan()==GetRowCount();
				
			if(fullrow && !ConfirmFullyLoaded())
				return;

			for(int c = gridSel.GetMinCol(); c <= gridSel.GetMaxCol(); c++)
			{
				int col = DisplayToDataColumn(c);
				for(__int64 r = gridSel.GetMinRow(); r <= gridSel.GetMaxRow(); r++)
				{
					CCellState* oldcs=(CCellState*)(m_data->GetCellUnknown(r, col));
					CCellState* cs;
					if(oldcs!=0)
						cs=new CCellState(font, oldcs->GetTextColor(), oldcs->GetBkColor()); 
					else
						cs=new CCellState(font, m_cellTextColor, m_cellBkColor); 

					if(cs)
						m_data->SetCellUnknown(r, col, cs);
				}
			}
		}

		//AutoSizeColumns(gridSel.GetMinCol(), gridSel.GetMaxCol());
		AutoSizeRows(gridSel.GetMinRow(), gridSel.GetMaxRow());
		Redraw();
	}

	void BoldenSelection(bool bold)
	{
		CCellRange gridSel = GetGridSelection();
		if (gridSel.IsValid())
		{
			bool fullrow=gridSel.GetRowSpan()==GetRowCount();
				
			if(fullrow && !ConfirmFullyLoaded())
				return;

			for(int c = gridSel.GetMinCol(); c <= gridSel.GetMaxCol(); c++)
			{
				int col = DisplayToDataColumn(c);
				for(__int64 r = gridSel.GetMinRow(); r <= gridSel.GetMaxRow(); r++)
				{
					CCellState* oldcs=(CCellState*)(m_data->GetCellUnknown(r, col));
					CCellState* cs;
					if(oldcs!=0)
					{
						LOGFONT lfbold=oldcs->GetLogFont();
						lfbold.lfWeight=bold ? FW_BOLD : FW_NORMAL;
						cs=new CCellState(lfbold, oldcs->GetTextColor(), oldcs->GetBkColor()); 
					}
					else
					{
						LOGFONT lfbold=m_cellLogFont;
						lfbold.lfWeight=bold ? FW_BOLD : FW_NORMAL;
						cs=new CCellState(lfbold, m_cellTextColor, m_cellBkColor); 
					}

					if(cs)
						m_data->SetCellUnknown(r, col, cs);
				}
			}
		}

		AutoSizeRows(gridSel.GetMinRow(), gridSel.GetMaxRow());
		Redraw();
	}

	void ItalicizeSelection(bool italic)
	{
		CCellRange gridSel = GetGridSelection();
		if (gridSel.IsValid())
		{
			bool fullrow=gridSel.GetRowSpan()==GetRowCount();
				
			if(fullrow && !ConfirmFullyLoaded())
				return;
	
			for(int c = gridSel.GetMinCol(); c <= gridSel.GetMaxCol(); c++)
			{
				int col = DisplayToDataColumn(c);
				for(__int64 r = gridSel.GetMinRow(); r <= gridSel.GetMaxRow(); r++)
				{
					CCellState* oldcs=(CCellState*)(m_data->GetCellUnknown(r, col));
					CCellState* cs;
					if(oldcs!=0)
					{
						LOGFONT lfitalic=oldcs->GetLogFont();
						lfitalic.lfItalic=italic;
						cs=new CCellState(lfitalic, oldcs->GetTextColor(), oldcs->GetBkColor()); 
					}
					else
					{
						LOGFONT lfitalic=m_cellLogFont;
						lfitalic.lfItalic=italic;
						cs=new CCellState(lfitalic, m_cellTextColor, m_cellBkColor); 
					}

					if(cs)
						m_data->SetCellUnknown(r, col, cs);
				}
			}
		}

		//AutoSizeColumns(gridSel.GetMinCol(), gridSel.GetMaxCol());
		AutoSizeRows(gridSel.GetMinRow(), gridSel.GetMaxRow());
		Redraw();
	}

	LOGFONT& GetSelectedCellFont()
	{
		CCellRange gridSel = GetGridSelection();
		if (gridSel.IsValid())
		{
			int col=DisplayToDataColumn(gridSel.GetMinCol());
			CCellState* cs=(CCellState*)(m_data->GetCellUnknown(gridSel.GetMinRow(), col));
			if(cs!=0)
				return cs->GetLogFont();
			else
				return m_cellLogFont;
		}
		else
			return m_cellLogFont;
	}

	void DoPaste()
	{
		if(!ConfirmFullyLoaded())
			return;

		int startcol, startrow;
		CCellID cell=baseClass::GetFocusCell();
		if(cell.IsValid())
		{
			startcol=cell.col;	
			startrow=cell.row;
		}
		else
		{
			CCellRange gridSel = GetGridSelection();
			if (!gridSel.IsValid())
				return;

			startcol=gridSel.GetMinCol();
			startrow=gridSel.GetMinRow();
		}

		if(!::OpenClipboard(m_hWnd))
			return;

		HGLOBAL hmem =(HGLOBAL)::GetClipboardData(CF_TEXT);
		char* ascii=(char*)::GlobalLock(hmem);
		CString text=CA2T(ascii, CP_UTF8);
		::GlobalUnlock(hmem);
		::CloseClipboard();

		int len=text.GetLength();
		std::_tstring celltext;

		int orgphyscols=m_data->GetColCount(); // remember for later
		int orgphysrows=m_data->GetPhysicalRowCount(); 

		unsigned int col=startcol;
		unsigned int row=startrow;

		int pastecol=0, maxpastecols=0;

		for(int i=0; i<len; ++i)
		{
			switch(text[i])
			{
			case _T('\n'):
				if(col < (int)m_hiddenColMapping.size())
				{
					m_data->SetCell(row, DisplayToDataColumn(col), celltext);
				}
				else
				{
					if(col==m_hiddenColMapping.size())
						col=orgphyscols;
					m_data->SetCell(row, col, celltext);
				}
				pastecol++;
				if(pastecol>maxpastecols)
					maxpastecols=pastecol;
				pastecol=0;
				col=startcol;
				row++;
				celltext.clear();
				break;
			case _T('\r'):
				break;
			case _T('\t'):
				if(col < (int)m_hiddenColMapping.size())
				{
					m_data->SetCell(row, DisplayToDataColumn(col), celltext);
				}
				else
				{
					if(col==m_hiddenColMapping.size())
						col=orgphyscols;
					m_data->SetCell(row, col, celltext);
				}
				celltext.clear();
				col++;
				pastecol++;
				break;
			default:
				celltext.append(text[i], 1);
				break;
			}
		}

		int newcols=startcol+maxpastecols-m_hiddenColMapping.size();
		if(newcols>0)
		{
			for(int i=0; i<newcols;++i)
			{
				std::_tstring title(_T("col_"));
				title+=boost::lexical_cast<std::_tstring>(m_hiddenColMapping.size()+i);
				m_data->SetColumn(orgphyscols+i, title);
			}

			int j=orgphyscols;
			int newdispcols=m_hiddenColMapping.size()+newcols;
			for(int i = m_hiddenColMapping.size(); i < newdispcols; ++i, ++j)
				m_hiddenColMapping.push_back(j);

			// add columns on GUI side
			SetColumnCount(m_hiddenColMapping.size());
		}

		int newrows=row-orgphysrows;
		if(newrows>0)
		{
			for(int i=0; i<newrows;++i)
			{
				m_data->SetCell(orgphysrows + i, 0, boost::lexical_cast<std::_tstring>(orgphysrows + i + 1));
			}

			m_data->AddRows(newrows);
			// add rows on GUI side
			SetRowCount(orgphysrows+newrows);
		}
		Redraw();
	}

	//  Statics  ---

	static BOOL CALLBACK GridCallback(SGRID::GV_DISPINFO *pDispInfo, LPARAM lParam)
	{
		thisClass * self = (thisClass * )lParam;
		return self->GridCallback(pDispInfo);
	}

	static BOOL CALLBACK GridCallbackPrepCache(SGRID::GV_CACHEHINT *pDispInfo, LPARAM lParam)
	{
		thisClass * self = (thisClass * )lParam;
		return self->GridCallbackPrepCache(pDispInfo);
	}

	static BOOL CALLBACK GridCallbackCellClick(SGRID::GV_CELL *pCell, LPARAM lParam)
	{
		thisClass * self = (thisClass * )lParam;
		return self->GridCallbackCellClick(pCell);
	}
};
