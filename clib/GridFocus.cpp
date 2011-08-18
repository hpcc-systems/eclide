#include "StdAfx.h"

#include ".\gridfocus.h"

CGridFocus::CGridFocus(void)
{
	m_row = -1;
	m_column = 0;
}

CGridFocus::~CGridFocus(void)
{
}

void CGridFocus::SetFocus(__int64 row)
{
	m_row = row;
	m_column = 1;
}

void CGridFocus::SetFocus(__int64 row, int column)
{
	m_row = row;
	m_column = column;
}

void CGridFocus::SetFocusLeft()
{
	if ( m_column > 0 )
		m_column--;
}

void CGridFocus::SetFocusHome()
{
	m_column = 0;
}

void CGridFocus::SetFocusRight(int lastColumn)
{
	if ( m_column < lastColumn )
		m_column++;
}

void CGridFocus::SetFocusEnd(int lastColumn)
{
	m_column = lastColumn;
}

void CGridFocus::SetFocusUp()
{
	if ( m_row > 0 )
		m_row--;
}

void CGridFocus::SetFocusDown(__int64 lastRow)
{
	if ( m_row < lastRow )
		m_row++;
}


bool CGridFocus::HasFocus(__int64 row, int column)
{
	return row == m_row && m_column == column;
}
