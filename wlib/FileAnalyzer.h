#pragma once

#include <workunit.h> //comms
#include ".\ImportRecordAnalyze.h"

class WLIB_API CFileAnalyzer
{
public:
	CFileAnalyzer(CImportRecordAnalyze &info);
	virtual ~CFileAnalyzer(void);

	void NewAnalysis();
	void SetKnownFileSize(__int64 knownFileSize);

	bool IsFixedWidth();

	__int64		m_fileSize;
	int			m_recordSize;
	int			m_records;
	int			m_probeRecords;  //how many rows to analyze - 48?
	ITableAdapt m_table;

	CImportRecordAnalyze& m_recordInfo;

	bool GetAnalyzerEcl(CString &ecl, const TCHAR *cluster, Dali::IWorkunit *workunit);

	typedef enum _ENCODING
	{
		ENCODING_UNKNOWN,
		ENCODING_ASCII,
		ENCODING_UTF_8
	} ENCODING;

	ENCODING m_encoding;
	
protected:
	bool ScanForXML(ITable *table);
	bool ScanXML(const TCHAR *cell, unsigned nBomChars, unsigned nLen );
	void ScanRow(ITable *table, int row, CImportRecordAnalyze &rowInfo);
	bool CompletedAnalysis(Dali::IWorkunit *workunit); //true if got good fix on data
	bool AnalyzeResults();
	bool HasBOM(const TCHAR *file, unsigned &nChars);
	TCHAR CharFromHex(const TCHAR *cell);
	void init();
};
