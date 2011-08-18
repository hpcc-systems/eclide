#pragma once
#include ".\ImportRecordInfo.h"
#include ".\StringSet.h"
#include "eclparser.h"

class WLIB_API CImportRecordAnalyze : public CImportRecordInfo
{
protected:
	std::_tstring m_prefix;

	typedef std::map<unsigned,unsigned> Scores;
	typedef std::map<int,unsigned> Frequency; //column pos, count of rows with blank in pos

	Frequency m_blanks;
	Frequency m_terminators;
	Frequency m_defaults;

	bool m_binaryData;
	int  m_rows;
	int  m_nTerminators;    //terminators per record/cell (1 cell per record)
	bool m_constRowWidth;  //do records have constant distance between them
	int  m_maxVarRecord;

	typedef std::vector<unsigned> Widths;  //column width
	Widths m_rowWidth;
	Widths m_columnWidth;

	//typedef std::vector<StdString> Names;  //column names
	StdStringVector	m_columnName;
	int		m_columns;

public:
	CImportRecordAnalyze& operator=(const CImportRecordAnalyze& other);
	void Clear();

	void SetPrefix(const TCHAR * label);

	CDelimitedColumnInfo  m_commas;
	CDelimitedColumnInfo  m_tabs;
	CDelimitedColumnInfo  m_otherDelimiter;	//other delimiter (user specified)
	CDelimitedColumnInfo  m_delimitedColumnInfo;

	void SetXmlFileFormat(bool bResetColumns);

	void FoundEndOfLine(int columnPos, unsigned width);
	void FoundDelimiter(TCHAR delimiter, int distance);
	void FoundBlank(int columnPos);
	void FoundBinaryData();
	void FoundDefaultData(int columnPos, unsigned value);
	void CompletedScan(int rows, int sampleSize, __int64 fileSize);
	int GetRows() const;
	void SetColumn(const TCHAR* columnName, unsigned nWidth); //for XML files

	unsigned Columns() const; //0 or 1 if unknown
	int ColumnWidth(int column);
	unsigned BadRows();
	bool HasBinaryData() const;

	const TCHAR* GetEclInlineDataset(CString &dataset);
	const TCHAR* GetEclDataset(const CString &location, const CString &record, CString &dataset);
	const TCHAR* GetEclRecord(CString &record);
	IEclRecord*  GetEclRecord(IEclRecord* record);
	const TCHAR* GetEclRecordItems(CString &items);
	IEclRecord*  GetEclRecordItems(IEclRecord *record);
	//const TCHAR* GetEclSpray(CString &spray);
	void SetMaxVarRecordSize(unsigned nMax);
	bool SplitColumn(int column, int leftHandColumnSize);
	bool MergeColumns(int column1);
	bool CanBeLastColumn(int column, int* pNewWidth=0);
	bool SetLastColumn(int column);
	unsigned ColumnPosition(int column);
	bool CanSplitColumn(int column);

protected:
	void RecalcRecordSize(const Frequency& source, Frequency& alt, int recordWidth);
	static void FoundCountedChar(Frequency& freq, int columnPos, unsigned count);
	static unsigned FrequencyCount(Frequency& freq, int columnPos);
	bool IdFixedColumnBreaks(int sampleSize, int recordWidth, unsigned& score);
	int  CheckTerminator(int rows, int columnPos, Frequency::const_iterator& term, int& lastColumnPos);

	void SelectBestScore(const Scores& scores, int sampleSize);
	void SelectDefault(int tempFixedWidth, int sampleSize);

	void SetColumnWidth(unsigned width);
	void GetEclRecordItemsXml(CString &items);
	void GetEclRecordItemsFixed(CString &items);
	void GetEclRecordItemsDelimited(CString &items);
};

