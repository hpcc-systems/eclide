//$Id: DelimitedColumnInfo.cpp 3310 2007-02-13 15:22:24Z tjuricic $ 
#include "StdAfx.h"
#include ".\DelimitedColumnInfo.h"

CDelimitedColumnInfo::CDelimitedColumnInfo()
{
	Reset();
}

CDelimitedColumnInfo& CDelimitedColumnInfo::operator=(const CDelimitedColumnInfo& other)
{
	m_columnWidth = other.m_columnWidth;
	m_rowColumnCount = other.m_rowColumnCount;
	m_rowColumnMismatches = other.m_rowColumnMismatches;
	m_columns = other.m_columns;
	m_runningColumnWidth = other.m_runningColumnWidth;
	m_columnCount = other.m_columnCount;
	m_goodData = other.m_goodData;
	return *this;
}

void CDelimitedColumnInfo::Reset()
{
	m_columnWidth.clear();
	m_rowColumnCount.clear();
	m_columns = INVALID_COLUMN;
	m_rowColumnMismatches = 0;
	m_columnCount = 0;
	m_runningColumnWidth = 0;
	m_goodData = false;
}

void CDelimitedColumnInfo::SetSingleColumnVariable(unsigned maxWidth)
{
	Reset();
	FoundDelimiter(maxWidth);
	m_goodData = true;
	m_columns = 1;
}

void CDelimitedColumnInfo::FoundEndOfLine(unsigned rowWidth)
{
	//width will include delimiters, so subtract them if possible
	if ( rowWidth > m_columnCount )
		rowWidth -= m_columnCount;

	//if have found 1 column, then treat end of line as a delimiter
	if ( m_runningColumnWidth )
	{
		unsigned columnWidth = 1;
		if ( rowWidth > m_runningColumnWidth )
			columnWidth = (rowWidth - m_runningColumnWidth);
		FoundDelimiter(columnWidth);
	}

	//just count the number of columns per row
	//assume this is called in proper row order
	if ( m_columnCount )
		m_rowColumnCount.push_back(m_columnCount);
	m_columnCount = 0;
	m_runningColumnWidth = 0;
}

void CDelimitedColumnInfo::FoundDelimiter(unsigned distance)
{
	m_runningColumnWidth += distance;

	if ( m_columnWidth.size() <= m_columnCount )
	{
		m_columnWidth.resize(m_columnCount+1);
		m_columnWidth[m_columnCount] = distance;
	}
	else
	{
		//save the max value
		if ( m_columnWidth[m_columnCount] < distance )
			m_columnWidth[m_columnCount] = distance;
	}
	m_columnCount++; //track columns per row
}

unsigned MostCommonNumColumns(const UnsignedVector& rowColumnCount, unsigned& mismatches)
{
	//build a map that tracks the occurances of each Column Count
	typedef std::map<unsigned,unsigned> Counts;
	Counts counts;
	for (UnsignedVector::const_iterator i=rowColumnCount.begin();
		 i!=rowColumnCount.end(); i++ )
	{
		unsigned nColumns = *i;
		counts[nColumns]++; //vote for this one
	}
	//find the count of columns with the largest value
	unsigned max = 0;
	unsigned columns = 0;
	for (Counts::const_iterator i=counts.begin(); i!=counts.end(); i++ )
	{
		if ( i->second > max )
		{
			columns = i->first;
			max = i->second;
		}
	}
	//if this is at least 67% of the rows, then accept it
	unsigned rows = rowColumnCount.size();
	mismatches = rows - max;
	if ( rows > 5 && max > ((rows*2) / 3) )
	{
		return columns;
	}
	else
		return 0;
}

bool CDelimitedColumnInfo::GoodData()
{
	//calculate the data if we haven't already
	if ( m_columns == INVALID_COLUMN )
	{
		m_columns = MostCommonNumColumns(m_rowColumnCount,m_rowColumnMismatches);
		m_goodData = (m_columns > 1);
	}
	return m_goodData;
}

unsigned CDelimitedColumnInfo::BadRows()
{
	ATLASSERT(m_goodData);
	return m_rowColumnMismatches;
}

unsigned CDelimitedColumnInfo::Columns()
{
	ATLASSERT(m_goodData);
	return m_columns;
}

unsigned CDelimitedColumnInfo::ColumnWidth(int column)
{
	ATLASSERT((unsigned)column < m_columns);
	return m_columnWidth[column];
}


bool CDelimitedColumnInfo::SplitColumn(int column, int leftHandColumnSize)
{
	if ( column >= (int)m_columns || leftHandColumnSize <= 0 )
		return false;

	unsigned width = m_columnWidth[column];
	if ( leftHandColumnSize >= (int)width )
		return false;

	UnsignedVector::iterator i = m_columnWidth.begin()+column;
	*i -= leftHandColumnSize;
	m_columnWidth.insert(i,leftHandColumnSize);
	m_columns++;
	return true;
}

bool CDelimitedColumnInfo::MergeColumns(int column1)
{
	if ( column1+1 >= (int)m_columns )
		return false;

	UnsignedVector::iterator i = m_columnWidth.begin()+column1;
	*i += m_columnWidth[column1+1];
	m_columnWidth.erase(i+1);
	m_columns--;
	return true;
}

unsigned CDelimitedColumnInfo::RowWidth()
{
	ATLASSERT(m_goodData);
	unsigned width = 0;
	for (UnsignedVector::const_iterator i=m_columnWidth.begin(); i!=m_columnWidth.end(); i++ )
	{
		width += *i;
	}
	return width;
}
