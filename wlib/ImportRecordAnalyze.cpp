#include "StdAfx.h"
#include ".\importrecordanalyze.h"

CImportRecordAnalyze& CImportRecordAnalyze::operator=(const CImportRecordAnalyze& other)
{
	CImportRecordInfo::operator=(other);

	m_prefix = _T("unknown");
	m_rowWidth = other.m_rowWidth;
	m_nTerminators = other.m_nTerminators;
	m_constRowWidth = other.m_constRowWidth;
	m_columnWidth = other.m_columnWidth;
	m_columnName = other.m_columnName;
	m_rows = other.m_rows;
	m_binaryData = other.m_binaryData;
	m_maxVarRecord = other.m_maxVarRecord;
	m_columns = other.m_columns;
	m_blanks = other.m_blanks;
	m_commas = other.m_commas;
	m_tabs = other.m_tabs;
	m_otherDelimiter = other.m_otherDelimiter;
	m_delimitedColumnInfo = other.m_delimitedColumnInfo;
	m_defaults = other.m_defaults;
	return *this;
}

void CImportRecordAnalyze::Clear()
{
	m_prefix = _T("unknown");
	CImportRecordInfo::Clear();
	m_nTerminators = 0;
	m_rowWidth.clear();
	m_constRowWidth = true;
	m_binaryData = false;
	m_rows = 0;
	m_columns = 1;
	m_maxVarRecord = (8*1204);
	m_columnName.clear();
	m_columnWidth.clear();
	m_blanks.clear();
	m_commas.Reset();
	m_tabs.Reset();
	m_otherDelimiter.Reset();
	m_delimitedColumnInfo.Reset();
	m_defaults.clear();
}

void CImportRecordAnalyze::SetPrefix(const TCHAR * label)
{
	m_prefix = label;
}

void buildUpVector(std::vector<unsigned> &v, int index, unsigned value)
{
	//build up the vector if out of space
	int capacity = v.capacity();
	if ( capacity <= index )
	{
		v.reserve(index+16);
	}

	if ( v.size() <= (unsigned)index )
	{
		v.resize(index+1);
		v[index] = value;
	}
	else
	{
		//save the max value
		if ( v[index] < value )
			v[index] = value;
	}
}

void CImportRecordAnalyze::CompletedScan(int rows, int sampleSize, __int64 fileSize)
{
	m_fileSize = fileSize;
	if ( m_nTerminators > 0 )
		m_rows = m_nTerminators; //possibly +1, but don't worry about it
	else
		m_rows = rows;

	//calculate a delimiter (look at tabs first, you could have commas in a tab delimited file
	if ( m_tabs.GoodData() )
	{
		m_delimiter = _T("\t");
		m_delimitedColumnInfo = m_tabs;
	}
	else if ( m_otherDelimiter.GoodData() )
	{
		ATLASSERT(!m_delimiter.empty()); //should be set already
		m_delimitedColumnInfo = m_otherDelimiter;
	}
	else if ( m_commas.GoodData() )
	{
		m_delimiter = _T(",");
		m_delimitedColumnInfo = m_commas;
	}
	else
	{
		m_delimitedColumnInfo.Reset();
	}

	bool bCheckForFixedButCouldBeDelimited = false;
	if ( m_delimitedColumnInfo.GoodData() )
	{
		m_columns = (int)m_delimitedColumnInfo.Columns();
		SetDelimitedFileFormat();
		m_maxVarRecord = (m_delimitedColumnInfo.RowWidth() * 4); //add big fudge factor
		//If we found good delimiters use them, however, if not tab delimiters it may be
		//the case that it is fixed but with a few commas sprinkled in
		bCheckForFixedButCouldBeDelimited = !IsTabSeparated();
	}	
	
	if ( !m_delimitedColumnInfo.GoodData() || bCheckForFixedButCouldBeDelimited )
	{
		int tempFixedWidth = m_fixedWidth; //cache it
		SetFixedFileFormat();

		//See if there is an EOL marker and use that width first
		bool bGoodEolFixedWidth;
		if ( m_fixedWidth <= 1 || !m_constRowWidth )
		{
			m_fixedWidth = sampleSize; //one row unless proven other wise
			bGoodEolFixedWidth = false;
		}
		else
		{
			m_fixedWidth += m_terminator.length();
			bGoodEolFixedWidth = true;
		}

		//the first call is hoping to find good eol markers
		Scores scores;
		unsigned score = 100;
		if ( !bGoodEolFixedWidth || !IdFixedColumnBreaks(sampleSize,m_fixedWidth,score) )
		{
			//no good EOL markers, so start guessing unless we already found delimiter
			if ( bCheckForFixedButCouldBeDelimited )
			{
				m_columns = (int)m_delimitedColumnInfo.Columns();
				SetDelimitedFileFormat();
				return;
			}

			//look for a possible record size other than 1 row!
			for (int i=9; i<sampleSize; i++ )
			{
				int r = (m_fileSize % i);
				if ( !r )
				{
					if ( IdFixedColumnBreaks(sampleSize,i,score) )
					{
						scores[i] = score;
						ATLTRACE(_T("\tscore=%d, recordWidth=%u\r\n"), score, i );
					}
				}
			}
			if ( scores.size()  )
			{
				SelectBestScore(scores,sampleSize);
			}
			else 
			{
				SelectDefault(tempFixedWidth,sampleSize);
			}
		}
	}
}

void CImportRecordAnalyze::SelectDefault(int tempFixedWidth, int sampleSize)
{
	m_fixedWidth = tempFixedWidth; //reset it
	//no delimiter found, but none needed for one column with EOL, so special case 
	if ( m_fixedWidth < sampleSize && !m_terminator.empty() && m_columnWidth.size() <= 1 )
	{
		m_columnWidth.resize(1);
		m_columnWidth[0] = m_fixedWidth;
		ATLTRACE(_T("Found Single Column File width = %d\r\n"), m_fixedWidth );
		//check to see if width is fixed or variable
		if ( !m_constRowWidth )
		{
			m_maxVarRecord = m_fixedWidth * 2; //we may not have seen largest field
			m_delimitedColumnInfo.SetSingleColumnVariable(m_maxVarRecord);
			SetDelimiter(_T(",")); //any delimiter will do - there isn't one
			SetDelimitedFileFormat();
		}
	}
	else
	{
		m_columns = 1;
		m_rows = 1;
		m_fixedWidth = sampleSize;
	}
}

void CImportRecordAnalyze::SelectBestScore(const Scores& scores, int sampleSize)
{
	unsigned bestScore = sampleSize; //worst case, width of 1
	for (Scores::const_iterator i=scores.begin(); i != scores.end(); i++)
	{
//	if ( i->second <= bestScore ) // gp::replace only if score is better
		if ( i->second <  bestScore ) //     this favors smaller records
		{
			bestScore = i->second;
			m_fixedWidth = i->first;
		}
	}
	//this will set the column widths (again!)
	IdFixedColumnBreaks(sampleSize,m_fixedWidth,bestScore);
}

void CImportRecordAnalyze::SetColumn(const TCHAR* columnName, unsigned width)
{
	SetColumnWidth(width);
	m_columnName.push_back(columnName);
	ATLASSERT(m_columns==m_columnName.size());
}

void CImportRecordAnalyze::SetColumnWidth(unsigned width)
{
	buildUpVector(m_columnWidth, m_columns, width );
	m_columns++;
}

int CImportRecordAnalyze::CheckTerminator(int rows, int columnPos, Frequency::const_iterator& term, int& lastColumnPos)
{
	//see if we just passed a terminator - count must be same as rows
	int terminatorColumns = 0;
	int termColumnPos = term->first;
	unsigned termCount = term->second;
	if ( columnPos >= termColumnPos && termCount == rows)
	{
		//ATLTRACE(_T("\ttermColumnPos=%d, termCount=%u\r\n"), termColumnPos, termCount ); 

		//split at this pos (not after like blanks)
		termColumnPos--;

		//if we just split because of blank analysis, then don't bother!
		int width = termColumnPos-lastColumnPos;
		if ( width >= 1 )
		{
			SetColumnWidth(width);
			terminatorColumns++;
			lastColumnPos = termColumnPos; 
		}

		//now add the terminator column itself
		termColumnPos += m_terminator.length();
		SetColumnWidth(termColumnPos-lastColumnPos);
		lastColumnPos = termColumnPos;
		terminatorColumns++;

		term++;
	}
	return terminatorColumns;
}

bool CImportRecordAnalyze::IdFixedColumnBreaks(int sampleSize, int recordWidth, unsigned& score)
{
	//scan the map of blanks and look for a high blank count followed by no blanks
  score = 100;
	int rows = int(sampleSize / recordWidth);
	if ( rows < 3 )
		return false;

  int tenPercent = rows / 10;
  if (tenPercent < 1)
    return false;

	int lastColumnPos = -1;
	m_columnWidth.clear();
	m_columns = 0;
	int terminatorColumns = 0;
	Frequency  blanks;
	Frequency  terminators;

	//recalc this for a new recordWidth
	RecalcRecordSize(m_blanks, blanks, recordWidth);
	RecalcRecordSize(m_terminators, terminators, recordWidth);

  //unsigned highCount = std::max(2, rows/2); //>50% blanks
  //unsigned lowCount = (highCount < 5) ? 0 : std::max(1, rows/8);
  unsigned highCount = std::max(2, 9 * tenPercent ); //>90% blanks
  unsigned lowCount = (highCount < 5) ? 0 : std::max(1, 6 * tenPercent); //<60% blanks
	ATLTRACE(_T("IdFixedColumnBreaks (width=%d, rows=%d, blank entries=%u, highCount=%u, lowCount=%u)\r\n"),
		recordWidth, rows, blanks.size(), highCount, lowCount );

	//treat terminators as column separators (of course)
	Frequency::const_iterator term;
	bool checkTerminators = terminators.size() > 1 && !m_constRowWidth;
	if ( checkTerminators )
		term = terminators.begin();

	for (Frequency::const_iterator i=blanks.begin(); i != blanks.end(); i++ )
	{
		int columnPos = i->first;
		if ( columnPos+1 < recordWidth ) //not last column
		{
			if ( checkTerminators )
			{
				terminatorColumns += CheckTerminator(rows, columnPos, term, lastColumnPos);
				checkTerminators = (term != terminators.end());
			}
			unsigned count = i->second;
			unsigned blanksAfter = FrequencyCount(blanks,columnPos+1);
			//has high count with no/few blanks in next column
			if ( count >= highCount && blanksAfter <= lowCount )
			{
				SetColumnWidth(columnPos-lastColumnPos);
				lastColumnPos = columnPos;

        float deltaPercentage = 100 - (100.0 * ((static_cast<float>(blanksAfter) / static_cast<float>(rows))));
        score = score + (100.0 - deltaPercentage);
			}
		}
	}
	//for the propertly terminated rows, check terminator now
	if ( !checkTerminators && terminators.size() == 1 && m_constRowWidth )
	{
		checkTerminators = true;
		term = terminators.begin();
	}
	//check for that last terminator
	if ( checkTerminators )
	{
		terminatorColumns += CheckTerminator(rows, (recordWidth-m_terminator.length()), term, lastColumnPos);
	}
	int nLastColumnWidth = (recordWidth-1)-lastColumnPos;
	if ( nLastColumnWidth > 0 )
		SetColumnWidth(nLastColumnWidth); 

  score *= recordWidth; // favor smaller record widths
  score /= m_columns-terminatorColumns; 
  
	if ( m_columns > 1 )
	{
		return true;
	}
	else
	{
		return false;
	}
}


void CImportRecordAnalyze::FoundDelimiter(TCHAR delimiter, int distance)
{
	if ( delimiter == ',' )
		m_commas.FoundDelimiter(distance);
	else if ( delimiter == '\t' )
		m_tabs.FoundDelimiter(distance);
	else
		m_otherDelimiter.FoundDelimiter(distance);
}

void CImportRecordAnalyze::FoundCountedChar(Frequency& freq, int columnPos, unsigned count)
{
	typedef std::pair <int,unsigned> FreqPair;

	//keep count of number of blanks found in each position
	Frequency::iterator i = freq.find(columnPos);
	if ( i == freq.end() )
	{
		freq.insert( FreqPair(columnPos, count) );
	}
	else
	{
		i->second += count;
	}
}

unsigned CImportRecordAnalyze::FrequencyCount(Frequency& freq, int columnPos)
{
	Frequency::iterator i = freq.find(columnPos);
	return (i == freq.end() ? 0 : i->second);
}


void CImportRecordAnalyze::RecalcRecordSize(const Frequency& source, Frequency& alt, int recordWidth)
{
	for (Frequency::const_iterator i=source.begin(); i != source.end(); i++ )
	{
		int columnPos = i->first % recordWidth;
		FoundCountedChar(alt, columnPos, i->second);
	}
	ATLTRACE(_T("RecalcRecordSize (width=%d, entries=%u)\r\n"), recordWidth, alt.size() );
}


void CImportRecordAnalyze::FoundBinaryData()
{
	m_binaryData = true;
}

void CImportRecordAnalyze::FoundBlank(int columnPos)
{
	FoundCountedChar(m_blanks,columnPos,1);
}

void CImportRecordAnalyze::FoundDefaultData(int columnPos, unsigned value)
{
	FoundCountedChar(m_defaults,columnPos,value);
}

void CImportRecordAnalyze::FoundEndOfLine(int columnPos, unsigned width)
{
	FoundCountedChar(m_terminators,columnPos,1);

	if ( (int)width > m_fixedWidth )
		m_fixedWidth = (int)width;

	m_commas.FoundEndOfLine(width);
	m_tabs.FoundEndOfLine(width);
	m_otherDelimiter.FoundEndOfLine(width);

	unsigned row = (unsigned)m_nTerminators;

	buildUpVector(m_rowWidth, row, width );

	//see if all rows are the same width
	if ( m_constRowWidth && row )
	{
		m_constRowWidth = (m_rowWidth[row-1] == width);
	}

	m_nTerminators++;
}

int CImportRecordAnalyze::GetRows() const
{
	return m_rows;
}

bool CImportRecordAnalyze::HasBinaryData() const
{
	return m_binaryData; 
}

unsigned CImportRecordAnalyze::BadRows() 
{
	return m_delimitedColumnInfo.BadRows();
}


bool CImportRecordAnalyze::SetLastColumn(int column)
{
	if ( column < 0 || column >= (int)m_columns )
		return false;

	m_columns = column+1;
	m_columnWidth.resize(m_columns);
	m_fixedWidth = ColumnPosition(column);
	return true;
}

bool CImportRecordAnalyze::CanBeLastColumn(int column, int* pNewWidth)
{
	if ( column < 0 || column >= (int)m_columns )
		return false;

	unsigned colPos = ColumnPosition(column);
	if ( pNewWidth ) *pNewWidth = int(colPos);

	if ( IsVariableFileFormat() ) //variable can always be truncated?
		return true;
	
	unsigned r = m_fixedWidth % colPos;
	return r == 0;
}

unsigned CImportRecordAnalyze::ColumnPosition(int column)
{
	if ( column < 0 || column >= (int)m_columns )
		return 0;

	unsigned width = 0;
	Widths::const_iterator i = m_columnWidth.begin();
	for (int c=0; c<=column; c++,i++)
	{
		width += *i;
	}

	return width;
}

bool CImportRecordAnalyze::CanSplitColumn(int column)
{
	return true;
}

bool CImportRecordAnalyze::SplitColumn(int column, int leftHandColumnSize)
{
	if ( IsVariableFileFormat() )
	{
		return m_delimitedColumnInfo.SplitColumn(column,leftHandColumnSize);
	}
	else
	{
		if ( column >= (int)m_columns || leftHandColumnSize <= 0 )
			return false;

		unsigned width = m_columnWidth[column];
		if ( leftHandColumnSize >= (int)width )
			return false;

		UnsignedVector::iterator i = m_columnWidth.begin()+column;
		*i = (*i)-leftHandColumnSize;
		m_columnWidth.insert(i,leftHandColumnSize);
		m_columns++;
		return true;
	}
}
bool CImportRecordAnalyze::MergeColumns(int column1)
{
	if ( IsVariableFileFormat() )
	{
		return m_delimitedColumnInfo.MergeColumns(column1);
	}
	else
	{
		if ( column1+1 >= (int)m_columns )
			return false;

		UnsignedVector::iterator i = m_columnWidth.begin()+column1;
		*i += m_columnWidth[column1+1];
		m_columnWidth.erase(i+1);
		m_columns--;
		return true;
	}
}

void CImportRecordAnalyze::GetEclRecordItemsDelimited(CString &items)
{
	CString item;
	const TCHAR *type = _T("string");

	for (int i=0; i<(int)m_delimitedColumnInfo.Columns(); i++)
	{
		//don't specifiy the width, don't need it at all
		item.FormatMessage(_T("\t%1 col%3!d!;\r\n"), 
			type, m_delimitedColumnInfo.ColumnWidth(i), i+1 );
		items += item;
	}
}

void CImportRecordAnalyze::GetEclRecordItemsFixed(CString &items)
{
	CString item;
	const TCHAR *type1 = _T("string");
	const TCHAR *type2 = _T("varstring");

	//if ( HasBinaryData() ) //not supported yet
	//	type = _T("data");

	if ( m_columns == 1 && m_terminator.empty())
	{
		item.FormatMessage(_T("\t%1%2!d! col1;\r\n"), type1, m_fixedWidth );
		items += item;
	}
	else
	{
		// TJ - this whole idea of using column width (where available) probably
		// makes no sense for CSV files and should be deleted after more testing
		// of column width production code

		int used = 0;
		for (int i=0; i<m_columns; i++)
		{
			unsigned int width=m_columnWidth.size() > i ? m_columnWidth[i] : 1;
			if(m_columnWidth.size() <= i)
				item.FormatMessage(_T("\t%1 col%2!d!;\r\n"), type2, i+1 );
			else
				item.FormatMessage(_T("\t%1%2!d! col%3!d!;\r\n"), type1, width, i+1 );
			items += item;
			used += width;
		}

		//add a filler for a terminator if there is one
		if ( used < m_fixedWidth && !m_terminator.empty() && m_constRowWidth )
		{
			item.FormatMessage(_T("\tstring%1!d! eol;\r\n"), m_terminator.size() );
			items += item;
		}
	}
}

void CImportRecordAnalyze::GetEclRecordItemsXml(CString &items)
{
	CString item;
	const TCHAR *type = _T("string");

	for (int i=0; i<(int)m_columns; i++)
	{
		item.FormatMessage(_T("\t%1 %3;\r\n"), //no width
			type, m_columnWidth[i], m_columnName[i].c_str() );
		items += item;
	}
}

const TCHAR* CImportRecordAnalyze::GetEclInlineDataset(CString &dataset)
{
	CString record;
	GetEclRecord(record);
	return GetEclDataset(GetFile(), record, dataset);
}

const TCHAR* CImportRecordAnalyze::GetEclDataset(const CString &location, const CString &record, CString &dataset)
{
	std::_tstring format;
	GetEclFormat(format);
	dataset.FormatMessage(_T("dataset(%1, %2, %3)"), location, record, format);
	return dataset;
}

const TCHAR* CImportRecordAnalyze::GetEclRecord(CString &record)
{
	CString items;
	GetEclRecordItems(items);
	record.FormatMessage(_T("\trecord\r\n%1\r\n\tend\r\n"), items);
	return record;
}

const TCHAR* CImportRecordAnalyze::GetEclRecordItems(CString &items)
{
	if ( IsVariableFileFormat() )
		GetEclRecordItemsDelimited(items);
	else if ( IsXmlFileFormat() )
		GetEclRecordItemsXml(items);
	else
		GetEclRecordItemsFixed(items);

	return items;
}

IEclRecord* CImportRecordAnalyze::GetEclRecord(IEclRecord *record)
{
  return GetEclRecordItems(record);
}

IEclRecord* CImportRecordAnalyze::GetEclRecordItems(IEclRecord *record)
{
  ATLASSERT(record);
  CString name;

	if ( m_columns == 1 && m_terminator.empty())
	{
		name.FormatMessage(_T("col1"));

		CComPtr<IEclString> type = CreateIEclString();
		CComPtr<IEclIdentifier> identifier = CreateIEclIdentifier(name.GetString());
		CComPtr<IEclDeclaration> declaration = CreateIEclDeclaration(identifier,type);
		type->setCharCount(1);
		record->AddField(declaration);
	}
	else
	{
		int used = 0;
		// TJ - this whole idea of using column width (where available) probably
		// makes no sense for CSV files and should be deleted after more testing
		// of column width production code
		for (int i=0; i<m_columns; i++)
		{
			// TJ TODO all of this stuff should be cleaned, lots of unused code
			if(IsXmlFileFormat())
				name=m_columnName[i].c_str();
			else
				name.FormatMessage(_T("col%1!d!"), i+1 );
			unsigned int width=m_columnWidth.size() > i ? m_columnWidth[i] : 1;
			used += width;
			CComPtr<IEclString> type = CreateIEclString();

			if(IsXmlFileFormat() || m_columnWidth.size() <= i)
				type->setVariable(true);
			CComPtr<IEclIdentifier> identifier = CreateIEclIdentifier(name.GetString());
			CComPtr<IEclDeclaration> declaration = CreateIEclDeclaration(identifier,type);
			if(!IsXmlFileFormat() && m_columnWidth.size() > i)
				type->setCharCount(width);
			record->AddField(declaration);
		}

		//add a filler for a terminator if there is one
		if (used < m_fixedWidth && !m_terminator.empty() && m_constRowWidth)
		{
			name.FormatMessage(_T("eol"));
			CComPtr<IEclString> type = CreateIEclString();
			CComPtr<IEclIdentifier> identifier = CreateIEclIdentifier(name.GetString());
			CComPtr<IEclDeclaration> declaration = CreateIEclDeclaration(identifier,type);
			type->setCharCount(m_terminator.size());
			record->AddField(declaration);
		}
	}

	return record;
}

void CImportRecordAnalyze::SetXmlFileFormat(bool bResetColumns)
{
	m_eFormat = eXml; 
	if ( bResetColumns )
	{
		m_columns = 0; //caller will have to set each column
		m_columnWidth.clear();
		m_columnName.clear();
	}
}
unsigned CImportRecordAnalyze::Columns() const
{
	return m_columns;
}

int CImportRecordAnalyze::ColumnWidth(int column)
{
	if ( IsVariableFileFormat() )
	{
		return m_delimitedColumnInfo.ColumnWidth(column);
	}
	else
	{
		if ( column >= (int)m_columns )
			return 0;

		return (int)m_columnWidth[column];
	}
}

void CImportRecordAnalyze::SetMaxVarRecordSize(unsigned nMax)
{
	m_maxVarRecord = nMax;
}
