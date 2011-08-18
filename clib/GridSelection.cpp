#include "StdAfx.h"

#include <limits.h>
#include "gridselection.h"

CGridSelection::CGridSelection()
{
}

CGridSelection::~CGridSelection()
{
}

class CGridSelectionAll : public CGridSelection
{
public:
	bool IsSelected(__int64 /*row*/, int /*column*/) { return true; };
	SelectionType GetType() const { return eAll; };
	bool operator ==(const CGridSelection *pOther) const
		{ return pOther->GetType() == eAll; };
	void ExtendBox(CSelectionBox &box) const
		{ box.Everywhere(); };
};

class CGridSelectionColumn : public CGridSelection
{
public:
	CGridSelectionColumn(int column, int fromColumn)
	{
		m_column = min(column,fromColumn);
		m_numAdditionalColumns = (column - fromColumn);
		if ( m_numAdditionalColumns < 0 )
		{
			m_numAdditionalColumns = -m_numAdditionalColumns;
		}
	}
	bool IsSelected(__int64 /*row*/, int column) 
	{ 
		return column >= m_column && column <= m_column + m_numAdditionalColumns;
	};
	SelectionType GetType() const { return eColumn; };
	bool operator ==(const CGridSelection *pOther) const
		{ return pOther->GetType() == eColumn && 
		  reinterpret_cast<const CGridSelectionColumn *>(pOther)->m_column == m_column; };
	void ExtendBox(CSelectionBox &box) const
		{   box.rowTop = 0; box.rowBottom = LONG_MAX;
			if ( box.columnLeft > GetFirstColumn() )
				box.columnLeft = GetFirstColumn();
			if ( box.columnRight < GetLastColumn() )
				box.columnRight = GetLastColumn();
		};

	int GetColumn() const { return m_column; };
	int GetNumColumns() const { return m_numAdditionalColumns; };
	int GetFirstColumn() const { return m_column; }
	int GetLastColumn() const { return m_column + m_numAdditionalColumns; }
	
protected:
	int m_column;
	int m_numAdditionalColumns;
};

class CGridSelectionRow : public CGridSelection
{
public:
	CGridSelectionRow(__int64 row, __int64 fromRow, int numColumnsInRow)
	{
		m_row = min(row,fromRow);
		m_numAdditionalRows = (row - fromRow);
		if ( m_numAdditionalRows < 0 )
		{
			m_numAdditionalRows = -m_numAdditionalRows;
		}
		m_numColumns = numColumnsInRow;

	}
	bool IsSelected(__int64 row, int /*column*/) 
	{ 
		return row >= m_row && row <= m_row + m_numAdditionalRows;
	};
	SelectionType GetType() const { return eRow; };
	bool operator ==(const CGridSelection *pOther) const
		{ return pOther->GetType() == eRow && 
		  reinterpret_cast<const CGridSelectionRow *>(pOther)->m_row == m_row; };
	void ExtendBox(CSelectionBox &box) const
		{ 
			__int64 firstRow = GetFirstRow();
			__int64 lastRow = GetLastRow();
			if ( firstRow < box.rowTop )
				box.rowTop = firstRow;
			if ( lastRow > box.rowBottom )
				box.rowBottom = lastRow;
			box.columnLeft = 0;
			box.columnRight = GetNumColumns();
		};

	__int64 GetRow() const { return m_row; };
	__int64 GetNumRows() const { return m_numAdditionalRows; };
	__int64 GetFirstRow() const { return m_row; }
	__int64 GetLastRow() const { return m_row + m_numAdditionalRows; }
	int GetNumColumns() const { return m_numColumns; };
protected:
	__int64 m_row;
	__int64	m_numAdditionalRows;
	int m_numColumns;
};


class CGridSelectionCell : public CGridSelection
{
public:
	CGridSelectionCell(__int64 row, int column)
	{
		m_row = row;
		m_column = column;
	}
	bool IsSelected(__int64 row, int column) { return row == m_row && column == m_column; };
	SelectionType GetType() const { return eCell; };
	bool operator ==(const CGridSelection *pOther) const
		{ return pOther->GetType() == eCell && 
		  reinterpret_cast<const CGridSelectionCell *>(pOther)->m_row == m_row &&
		  reinterpret_cast<const CGridSelectionCell *>(pOther)->m_column == m_column; };
	void ExtendBox(CSelectionBox &box) const
		{ 	if ( m_column > box.columnRight ) box.columnRight = m_column;
			if ( m_column < box.columnLeft ) box.columnLeft = m_column;
			if ( m_row > box.rowBottom ) box.rowBottom = m_row;
			if ( m_row < box.rowTop ) box.rowTop = m_row;
		};

	__int64 GetRow() const { return m_row; };
	int GetColumn() const { return m_column; };
protected:
	__int64 m_row;
	int m_column;
};

class CGridSelectionBox : public CGridSelection
{
public:
	CGridSelectionBox(const CSelectionBox &box)
	{
		m_box = box;
	}
	bool IsSelected(__int64 row, int column) { 
		return row >= m_box.rowTop && row <= m_box.rowBottom &&
			   column >= m_box.columnLeft && column <= m_box.columnRight; 
	};
	SelectionType GetType() const { return eBox; };
	bool operator ==(const CGridSelection *pOther) const
	{ 
		if ( pOther->GetType() == eBox )
		{
			const CGridSelectionBox *pBox = reinterpret_cast<const CGridSelectionBox *>(pOther);
			const CSelectionBox &oBox = pBox->GetBox();
			return oBox.columnLeft == m_box.columnLeft &&
				   oBox.columnRight == m_box.columnRight &&
				   oBox.rowBottom == m_box.rowBottom &&
				   oBox.rowTop == m_box.rowTop;
		}
		return false;
	}
	void ExtendBox(CSelectionBox &box) const
		{ box = m_box; };

	const CSelectionBox& GetBox() const { return m_box; };
protected:
	CSelectionBox m_box;
};

//=========================================================================
CGridSelectionCollection::CGridSelectionCollection(void)
{
	m_bSelectChanged = false;
}
CGridSelectionCollection::~CGridSelectionCollection()
{
	ClearList();
}

CGridSelectionCollection & CGridSelectionCollection::operator =(const CGridSelectionCollection &other)
{ 
	m_selectionList = other.m_selectionList;
	m_bSelectChanged = true;
	return *this;
}

void CGridSelectionCollection::ClearList(void)
{
	m_selectionList.clear();
}

bool CGridSelectionCollection::HasSelection() const
{
	return m_selectionList.size() > 0;
}

bool CGridSelectionCollection::IsSelected(__int64 row, int column) const
{
    GridSelectionList::const_iterator i;
	for ( i=m_selectionList.begin(); i != m_selectionList.end(); i++)
		if ( (*i)->IsSelected(row,column) )
			return true;
	return false;
}

bool CGridSelectionCollection::Exists(const CGridSelectionAdapt item) const
{
    GridSelectionList::const_iterator i;
	for ( i=m_selectionList.begin(); i != m_selectionList.end(); i++)
		if ( *item == *i )
			return true;
	return false;
}

void CGridSelectionCollection::RemoveOtherSelectionTypes(CGridSelection::SelectionType gridType)
{
    GridSelectionList::const_iterator i;
	for ( i=m_selectionList.begin(); i != m_selectionList.end(); i++)
	{
		//just look at the first one, they must all be the same
		if ( (*i)->GetType() != gridType )
		{
			ClearList();
			break;
		}
	}
}

void CGridSelectionCollection::Add(bool bAdd, CGridSelectionAdapt item)
{
	if ( bAdd )
	{
		//don't add it twice
		if ( Exists(item) )
		{
			return;
		}
		else
		{
			//can't mix the types even on add
			RemoveOtherSelectionTypes(item->GetType());
		}
	}
	else
	{
		ClearList();
	}
	m_selectionList.insert(m_selectionList.end(), item);
	m_bSelectChanged = true;
}

void CGridSelectionCollection::SelectNothing()
{
	m_bSelectChanged = true;
	ClearList();
}

void CGridSelectionCollection::SelectAll()
{
	CGridSelectionAdapt newItem = new CGridSelectionAll();
	Add(false, newItem);
}

void CGridSelectionCollection::SelectRow(bool bAdd, __int64 row, __int64 fromRow, int numColumnsInRow)
{
	if ( fromRow < 0 ) 
		fromRow = row;
	CGridSelectionAdapt newItem = new CGridSelectionRow(row,fromRow,numColumnsInRow);
	Add(bAdd, newItem);
}

void CGridSelectionCollection::SelectColumn(bool bAdd, int column, int fromColumn)
{
	if ( fromColumn < 0 )
		fromColumn = column;
	CGridSelectionAdapt newItem = new CGridSelectionColumn(column,fromColumn);
	Add(bAdd, newItem);
}

void CGridSelectionCollection::SelectCell(bool bAdd, __int64 row, int column)
{
	CGridSelectionAdapt newItem = new CGridSelectionCell(row, column);
	Add(bAdd, newItem);
}

void CGridSelectionCollection::SelectBox(__int64 row, int column)
{
	//This can be a single cell, or the extension to an existing box
	SelectionBox box;
	if ( GetSelectionBox(box) )
	{
		ExtendBox(box, row, column); 
		CGridSelectionAdapt newItem = new CGridSelectionBox(box);
		Add(false, newItem);
	}
	else
	{
		CGridSelectionAdapt newItem = new CGridSelectionCell(row, column);
		Add(false, newItem);
	}
}

void CGridSelectionCollection::SelectBox(SelectionBox &box)
{
	CGridSelectionAdapt newItem = new CGridSelectionBox(box);
	Add(false, newItem);
}

// Keyboard interface
void CGridSelectionCollection::SelectLeft(bool extendedSelect, __int64 row,int column)
{
	if ( extendedSelect )
	{
		SelectionBox box;
		if ( GetSelectionBox(box) && box.columnLeft > 0)
		{
			if ( column < box.columnLeft ) 
				box.columnLeft = column;
			else 
				box.columnRight = column;  
			CGridSelectionAdapt newItem = new CGridSelectionBox(box);
			Add(false, newItem);
		}
	}
	else
	{
		CGridSelectionAdapt newItem = new CGridSelectionCell(row, column);
		Add(false, newItem);
	}
}

void CGridSelectionCollection::SelectHome(bool extendedSelect, __int64 row, int /*fromColumn*/)
{
	if ( extendedSelect )
	{
		SelectionBox box;
		if (GetSelectionBox(box) && box.columnLeft > 0)
		{
			box.columnLeft = 0;
			CGridSelectionAdapt newItem = new CGridSelectionBox(box);
			Add(false, newItem);
		}
	}
	else
	{
		CGridSelectionAdapt newItem = new CGridSelectionCell(row,0);
		Add(false, newItem);
	}
}

void CGridSelectionCollection::SelectRight(bool extendedSelect, __int64 row, int column, int lastColumn)
{
	if ( extendedSelect )
	{
		SelectionBox box;
		if ( GetSelectionBox(box) && box.columnRight < lastColumn)
		{
			if ( column > box.columnRight ) 
				box.columnRight = column;
			else 
				box.columnLeft = column;  
			CGridSelectionAdapt newItem = new CGridSelectionBox(box);
			Add(false, newItem);
		}
	}
	else
	{
		CGridSelectionAdapt newItem = new CGridSelectionCell(row, column);
		Add(false, newItem);
	}
}

void CGridSelectionCollection::SelectEnd(bool extendedSelect, __int64 row, int /*fromColumn*/, int lastColumn)
{
	if ( extendedSelect )
	{
		SelectionBox box;
		if ( GetSelectionBox(box) && box.columnRight < lastColumn)
		{
			box.columnRight = lastColumn;
			CGridSelectionAdapt newItem = new CGridSelectionBox(box);
			Add(false, newItem);
		}
	}
	else
	{
		CGridSelectionAdapt newItem = new CGridSelectionCell(row,lastColumn);
		Add(false, newItem);
	}
}

void CGridSelectionCollection::SelectUp(bool extendedSelect, __int64 row, int column)
{
	if ( extendedSelect )
	{
		SelectionBox box;
		if ( GetSelectionBox(box) && box.rowBottom > 0)
		{
			if ( row < box.rowTop ) 
				box.rowTop = row;
			else 
				box.rowBottom = row;  
			CGridSelectionAdapt newItem = new CGridSelectionBox(box);
			Add(false, newItem);
		}
	}
	else
	{
		CGridSelectionAdapt newItem = new CGridSelectionCell(row,column);
		Add(false, newItem);
	}
}

void CGridSelectionCollection::SelectDown(bool extendedSelect,__int64 row, int column, __int64 lastRow)
{
	if ( extendedSelect )
	{
		SelectionBox box;
		if ( GetSelectionBox(box) && box.rowBottom < lastRow)
		{
			if ( row > box.rowBottom ) 
				box.rowBottom = row;
			else 
				box.rowTop = row;  
			CGridSelectionAdapt newItem = new CGridSelectionBox(box);
			Add(false, newItem);
		}
	}
	else
	{
		CGridSelectionAdapt newItem = new CGridSelectionCell(row,column);
		Add(false, newItem);
	}
}

void CGridSelectionCollection::ExtendBox(SelectionBox &box, __int64 row, int column)
{
	if ( column > box.columnRight ) box.columnRight = column;
	if ( column < box.columnLeft ) box.columnLeft = column;
	if ( row > box.rowBottom ) box.rowBottom = row;
	if ( row < box.rowTop ) box.rowTop = row;
}

bool CGridSelectionCollection::AllRowsSelected(__int64 rows) const
{
	if ( m_selectionList.size() > 0 )
	{
		CGridSelection::SelectionType type = m_selectionList.front()->GetType();
		if (type == CGridSelection::eColumn || type == CGridSelection::eAll)
			return true;
		SelectionBox box;
		if ( GetSelectionBox(box,rows,1) && box.rowBottom+1 == rows )
			return true;
	}
	return false;
}


bool CGridSelectionCollection::GetSelectionAll() const
{
	return m_selectionList.size() > 0 &&
		m_selectionList.front()->GetType() == CGridSelection::eAll;
}

bool CGridSelectionCollection::GetSelectionRows(SelectionRows &rows) const
{
	if ( m_selectionList.size() > 0 &&
		 m_selectionList.front()->GetType() == CGridSelection::eRow )
	{
		rows.resize(m_selectionList.size());
		int r = 0;
		GridSelectionList::const_iterator i;
		for ( i=m_selectionList.begin(); i != m_selectionList.end(); i++)
		{
			const CGridSelectionRow *pRow = reinterpret_cast<const CGridSelectionRow *>(i->get());
			__int64 rowNumber = pRow->GetRow();
			unsigned numAdditionalRows = static_cast<unsigned>(pRow->GetNumRows());
			if ( numAdditionalRows > 0 )
				rows.resize(rows.size()+numAdditionalRows);
			for ( unsigned n=0; n<=numAdditionalRows; n++)
				rows[r++] = rowNumber+n;
		}
		std::sort(rows.begin(),rows.end());
		return true;
	}
	return false;
}

bool CGridSelectionCollection::GetSelectionColumns(SelectionColumns &columns) const
{
	if ( m_selectionList.size() > 0 &&
		m_selectionList.front()->GetType() == CGridSelection::eColumn )
	{
		columns.resize(m_selectionList.size());
		int c = 0;
		GridSelectionList::const_iterator i;
		for ( i=m_selectionList.begin(); i != m_selectionList.end(); i++)
		{
			const CGridSelectionColumn *pRow = reinterpret_cast<const CGridSelectionColumn *>(i->get());
			int columnNumber = pRow->GetColumn();
			int numAdditionalCols = pRow->GetNumColumns();
			if ( numAdditionalCols > 0 )
				columns.resize(columns.size()+numAdditionalCols);
			for ( int n=0; n<=numAdditionalCols; n++)
				columns[c++] = columnNumber+n;
		}
		std::sort(columns.begin(),columns.end());
		return true;
	}
	return false;
}

bool CGridSelectionCollection::GetSelectionCell(SelectionCell &cell) const
{
	if ( m_selectionList.size() == 1 )
	{
		if ( m_selectionList.front()->GetType() == CGridSelection::eCell )
		{
			const CGridSelectionCell *pCell = reinterpret_cast<const CGridSelectionCell *>(m_selectionList.front().get());
			cell.row = pCell->GetRow();
			cell.column = pCell->GetColumn();
			return true;
		}
	}
	return false;
}

bool CGridSelectionCollection::GetSelectionBox(SelectionBox &box) const
{
	if ( m_selectionList.size() > 0 )
	{
		box.ExtendableInit();
		GridSelectionList::const_iterator i;
		for ( i=m_selectionList.begin(); i != m_selectionList.end(); i++)
		{
			(*i)->ExtendBox(box); 
		}
		return true;
	}
	return false;
}

bool CGridSelectionCollection::GetSelectionBox(SelectionBox &box, __int64 maxRows, int maxColumns) const
{
	if ( !GetSelectionBox(box) )
	{
		return false;
	}
	else
	{
		if ( box.columnRight > maxColumns )
			box.columnRight = maxColumns;
		if ( box.rowBottom > maxRows )
			box.rowBottom = maxRows;
		return true;
	}
}

CGridSelection::SelectionType CGridSelectionCollection::GetSelectionType() const
{
	if ( m_selectionList.size() == 0 )
		return CGridSelection::eNone;
	else
		return m_selectionList.front()->GetType();
}

//=============================================================
CGridSelectionMarquee::CGridSelectionMarquee()
{
	m_startRow = m_endRow = -1;
	m_startColumn = m_endColumn = -1;
	m_bActive = false;
}

	 
void CGridSelectionMarquee::Start(__int64 startRow, int startCol)
{
	m_startRow = startRow;
	m_endRow = startRow;
	m_startColumn = startCol;
	m_endColumn = startCol;
	m_bActive = true;
}

bool CGridSelectionMarquee::Update(__int64 endRow, int endCol)
{
	bool bUpdated = false;
	if ( m_bActive )
	{
		if ( m_endRow != endRow )
		{
			m_endRow = endRow;
			bUpdated = true;
		}
		if ( m_endColumn != endCol )
		{
			m_endColumn = endCol;
			bUpdated = true;
		}
	}
	return bUpdated;
}

void CGridSelectionMarquee::End()
{
	m_bActive = false;
}

bool CGridSelectionMarquee::GetSelectionBox(CSelectionBox &box) const
{
	box.columnLeft = min(m_startColumn,m_endColumn);
	box.columnRight = std::max(m_startColumn,m_endColumn);
	box.rowTop = min(m_startRow,m_endRow);
	box.rowBottom = std::max(m_startRow,m_endRow);
	return m_bActive;
}
