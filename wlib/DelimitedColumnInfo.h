#pragma once

#include <clib.h>
#include "wlib.h"
#include "StdString.h"

typedef std::vector<unsigned> UnsignedVector;

class WLIB_API CDelimitedColumnInfo
{
public:
	CDelimitedColumnInfo();
	CDelimitedColumnInfo& operator=(const CDelimitedColumnInfo& other);

	//analysis time routines
	void Reset();
	void FoundEndOfLine(unsigned width);
	void FoundDelimiter(unsigned distance);
	void SetSingleColumnVariable(unsigned maxWidth); //special case!!!

	//completed analysis rountines
	bool GoodData();
	unsigned Columns();
	unsigned BadRows();
	unsigned ColumnWidth(int column);
	unsigned RowWidth();

	bool SplitColumn(int column, int leftHandColumnSize);
	bool MergeColumns(int column1);

protected:
	#define INVALID_COLUMN (unsigned(-1))

	UnsignedVector m_columnWidth;	//column width
	UnsignedVector m_rowColumnCount; //columns per row
	unsigned m_rowColumnMismatches;
	unsigned m_columns;
	unsigned m_columnCount;
	unsigned m_runningColumnWidth;
	bool	 m_goodData;
};

