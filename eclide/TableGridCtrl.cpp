#include "StdAfx.h"

#include "TableGridCtrl.h"

CTableGridCtrl::CTableGridCtrl()
{
}

CTableGridCtrl::~CTableGridCtrl()
{
}

void CTableGridCtrl::SetData(const ITable* data, bool hasRowNum)
{
    m_data = const_cast<ITable *>(data);
    if (m_data)
    {
        SetFixedColumnCount(hasRowNum);
        SetFixedRowCount(1);
        SetRowCount(m_data->GetRowCount());
        SetColumnCount(m_data->GetColumnCount());
        AutoSizeColumns(0, m_data->GetColumnCount());
    }
    else
    {
        SetFixedColumnCount(0);
        SetFixedRowCount(0);
        SetRowCount(0);
        SetColumnCount(0);
    }
    SelectNone();
}

BOOL CTableGridCtrl::Callback(SGRID::GV_DISPINFO *pDispInfo)
{
    if (!m_data)
        return TRUE;

    int column = pDispInfo->item.col;
    int row = pDispInfo->item.row - 1;
    ATLTRACE(_T("Row: %i, Col:  %i\n"), row, column);

    //if(row>=0 && column>0) // drawing a cell?
    //{
    //	CCellState* cs=(CCellState*)(m_data->GetCellUnknown(row, column)); 
    //	if(cs!=0)
    //	{
    //		pDispInfo->item.crBkClr=cs->GetBkColor();
    //		pDispInfo->item.crFgClr=cs->GetTextColor();
    //		pDispInfo->item.lfFont=cs->GetLogFont();
    //	}
    //	else
    //	{
    //		pDispInfo->item.crBkClr=m_cellBkColor;
    //		pDispInfo->item.crFgClr=m_cellTextColor;
    //		pDispInfo->item.lfFont=m_cellLogFont;
    //	}
    //}

    if (row == -1)
        pDispInfo->item.strText = m_data->GetColumn(column);
    else
        pDispInfo->item.strText = m_data->GetCell(row, column);
    //if (m_trim)
    //{
    //	pDispInfo->item.strText.TrimRight();
    //}
    return TRUE;
}

BOOL CTableGridCtrl::CallbackPrepCache(SGRID::GV_CACHEHINT *pDispInfo)
{
    return TRUE;
}

BOOL CTableGridCtrl::CallbackCellClick(SGRID::GV_CELL *pCell)
{
    if (pCell->cell.row == 0)
    {
        m_data->Sort(pCell->cell.col);
        AutoSizeRows(0, m_data->GetRowCount()-1);
    }
    else if(pCell->cell.row > 0)
    {
    }
    return TRUE;
}

void CTableGridCtrl::GotoLine(__int64 line, int col)
{
    if ( line < 0 )	line = 0;
    if ( col < 0 ) col = 0;
    baseClass::SetFocusCell(line, col);
    baseClass::EnsureVisible(line, col);
}

__int64 CTableGridCtrl::CurrentLine()
{ 
    return baseClass::GetFocusCell().row;
}

__int64 CTableGridCtrl::HitTestForColumn(const POINT &pnt, int *pCol)
{
    CCellID cell = baseClass::GetCellFromPoint(pnt);
    *pCol = cell.col;
    return cell.row;
}

bool CTableGridCtrl::HasSelection()
{
    CCellRange range = GetSelected();
    return range.IsValid() && range.Count() > 0;
}

//  Statics  ---
BOOL CALLBACK CTableGridCtrl::GridCallback(SGRID::GV_DISPINFO *pDispInfo, LPARAM lParam)
{
    thisClass * self = (thisClass * )lParam;
    return self->Callback(pDispInfo);
}

BOOL CALLBACK CTableGridCtrl::GridCallbackPrepCache(SGRID::GV_CACHEHINT *pDispInfo, LPARAM lParam)
{
    thisClass * self = (thisClass * )lParam;
    return self->CallbackPrepCache(pDispInfo);
}

BOOL CALLBACK CTableGridCtrl::GridCallbackCellClick(SGRID::GV_CELL *pCell, LPARAM lParam)
{
    thisClass * self = (thisClass * )lParam;
    return self->CallbackCellClick(pCell);
}
