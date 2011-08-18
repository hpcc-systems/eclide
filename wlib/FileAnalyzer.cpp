//$Id: FileAnalyzer.cpp 3999 2009-04-07 13:29:55Z gsmith $ 
#include "StdAfx.h"
#include "..\en_us\resource.h"

#include ".\fileanalyzer.h"
#include <UnicodeFile.h> //clib
#include <saxHandlers.h>
//TJ #include "SampleView.h"
//add
#include "ResultFormat.h"
#include "Util.h"
#include "AnalyzeXML.h"

#define FILEBUFFERSIZE (48*1024)
#define MAXFILEBUFFERSIZE (64*1024)

CFileAnalyzer::CFileAnalyzer(CImportRecordAnalyze &info)
: m_recordInfo(info)
{
	init();
}

CFileAnalyzer::~CFileAnalyzer(void)
{
}

void CFileAnalyzer::init()
{
	m_probeRecords = 1; // was 48, which is too many!
	m_fileSize = 0;
	m_recordSize = 0;
	m_table = 0;
	m_encoding = ENCODING_ASCII;
}

void CFileAnalyzer::NewAnalysis()
{
	init();
}

void CFileAnalyzer::SetKnownFileSize(__int64 knownFileSize)
{
	m_fileSize = knownFileSize;
}

bool CFileAnalyzer::IsFixedWidth()
{
	//the only way to know that it is fixed width is if the user told us
	//or we set it from analysis
	return (m_recordSize > 1);
}

int GetFileRecordSize(__int64 fileSize, int fixedWidth)
{
	if ( !fileSize )
	{
		if ( fixedWidth > 1 )
			return fixedWidth;
		else
			return 1024;  //probe value - expect error
	}
	else
	{
		//look for a big divisor, maybe multiple records - but will work!
		int start = MAXFILEBUFFERSIZE;
		if ( start > fileSize )
			start = (int)fileSize;
		for (int i=start; i; i-- )
		{
			__int64 r = (fileSize % i);
			if ( !r )
			{
				return i;
			}
		}
		return 1; //will never get here!
	}
}

TCHAR CFileAnalyzer::CharFromHex(const TCHAR *cell)
{
	TCHAR hex[5];
	hex[0] = cell[0];
	hex[1] = cell[1];
	hex[2] = 0;
	return TCHAR(::_tcstoul(hex,0,16));
}

bool CFileAnalyzer::HasBOM(const TCHAR *file, unsigned &nChars)
{
	bool isUTF8 = ::_tcsncmp(file,_T("EFBBBF"), 6) == 0;
	if ( isUTF8 )
	{
		m_encoding = ENCODING_UTF_8;
		nChars = 6;
		return true;
	}
	//TODO : UTF16, other formats?

	nChars = 0;
	return false;
}

bool CFileAnalyzer::ScanForXML(ITable *table)
{
	const TCHAR *cell = table->GetCell(0,0);
	unsigned len = ::_tcslen(cell);

	//look for a bom and <?xml at start of file
	#define XMLTAGLEN 5

	unsigned nBomChars = 0;
	bool bHasBOM = len > (XMLTAGLEN*2) && HasBOM(cell, nBomChars);

	TCHAR xml[11];
	unsigned c = 0;
	for (unsigned i=nBomChars; c < XMLTAGLEN && i<len; i+=2 )
	{
		TCHAR x = CharFromHex(cell+i);
		if ( !_istspace(TCHAR(x)) )
		{
			xml[c] = x;
			++c;
		}
	}
	xml[c] = 0;

	if ( ::_tcsnicmp(xml,_T("<?xml"), XMLTAGLEN) == 0 )
	{
		return ScanXML(cell, nBomChars, len);
	}
	else if ( xml[0] == '<' )
	{
		//if first char is a "<" then it could be xml, especially if there are more
		int openTags = 0;
		int closeTags = 0;
		for (unsigned i=nBomChars; i<len; i+=2 )
		{
			TCHAR x = CharFromHex(cell+i);
			if ( x == '<' )
				openTags++;
			else if ( x == '>' )
				closeTags++;
		}
		if ( openTags > 9 && closeTags > 9 )
		{
			//this just makes sure that there is a balance in open and close tags
			if ( abs(openTags - closeTags) < std::max<int>(2, openTags/10) )
				return ScanXML(cell, nBomChars, len);
		}
	}
	return false;
}

bool CFileAnalyzer::ScanXML(const TCHAR *cell, unsigned nBomChars, unsigned nLen )
{
	//decode the data string
	StdString xml;
	xml.resize( (nLen-nBomChars) / 2 );
	unsigned c=0;
	for (unsigned i=nBomChars; i<nLen; i+=2 )
	{
		xml[c++] = CharFromHex(cell+i);
	}

	CComPtr<IXMLAnalyzer> ia=CreateIXMLAnalyzer();
	ATLASSERT(ia);
	ia->Analyze(xml);

	if(ia->IsXML())
	{
		std::_tstring buff;
		m_recordInfo.SetXmlFileFormat(true);
		ia->GetXPath(buff);
		m_recordInfo.SetXmlRowTag(buff.c_str()); //"dataroot/table")); // TODO - hardcoded for now
		for ( unsigned i=0; i<ia->CountColumns(); i++ )
		{
			buff.clear();
			ia->GetColumnName(i, buff);
			m_recordInfo.SetColumn(buff.c_str(), 255);
		}
		return true;
	}

	return false;
}

void CFileAnalyzer::ScanRow(ITable *table, int row, CImportRecordAnalyze &rowInfo)
{
	int lastTerminator = 0;
	int lastCommaDelimiter = -1;
	int lastTabDelimiter = -1;
	int lastDelimiter = -1;
	int lastQuote = -1;
	bool bQuoteInFirstPos = false;
	StdString terminator;

	//there really should be only 1 column!
	ATLASSERT( table->GetColumnCount() == 1 );

	const TCHAR *cell = table->GetCell(row,0);
	unsigned len = ::_tcslen(cell);
	bool bOnTerminator = false;

	//look for a bom and <?xml at start of file
	unsigned nBomChars = 0;
	bool bHasBOM = len > 6 && HasBOM(cell, nBomChars);
	(bHasBOM);

#ifdef _DEBUG
	{
		CUnicodeFile file;
		if (file.Create(_T("\\temp\\analyzeHex.txt")))
		{
			file.Write(cell);
			file.Close();
		}
		CString decoded;
		for (unsigned i=nBomChars; i+1<len; i+=2 )
		{ 
			decoded += CharFromHex(cell+i);
		}
		if (file.Create(_T("\\temp\\analyze.txt")))
		{
			file.Write(decoded);
			file.Close();
		}
	}
#endif

	unsigned c = 0; //column
	for (unsigned i=nBomChars; i+1<len; i+=2,c++ )
	{ 
		TCHAR x = CharFromHex(cell+i);
		switch ( x )
		{
		case ' ' :
			rowInfo.FoundBlank(c); //blank analysis just wants absolute column!
			break;

		case '"' :
			if ( !bQuoteInFirstPos )
			{
				bQuoteInFirstPos = (!c || c == static_cast<unsigned>(lastDelimiter));
			}
			lastQuote = c+1;
			break;

		case ',' :
			//don't treat this as a separator if comma is in quoted value
			if ( bQuoteInFirstPos && static_cast<unsigned>(lastQuote) != c )
			{
				rowInfo.SetQuote(_T("\""));
			}
			else
			{
				rowInfo.FoundDelimiter(TCHAR(x), c-lastCommaDelimiter);
				lastDelimiter = lastCommaDelimiter = c+1;
				bQuoteInFirstPos = false;
			}
			break;

		case '\t' :
			rowInfo.FoundDelimiter(TCHAR(x), c-lastTabDelimiter);
			lastDelimiter = lastTabDelimiter = c+1;
			break;

		case '\r':
		case '\n':
			if ( !bOnTerminator )
			{
				bOnTerminator = true;
				rowInfo.FoundEndOfLine(c, c - lastTerminator);
				terminator = TCHAR(x);
			}
			else
			{
				terminator += TCHAR(x);
			}
			lastTerminator = lastDelimiter = lastCommaDelimiter = lastTabDelimiter = c+1;
			bQuoteInFirstPos = false;
			break;
		default:
			bOnTerminator = false;
			if ( !_istascii(TCHAR(x)) )
			{
				rowInfo.FoundBinaryData();
				return; //we don't do binary (yet?)
			}
      else
        rowInfo.FoundDefaultData(c,static_cast<unsigned>(x));

			if ( IsFixedWidth() && (c - lastTerminator) == static_cast<unsigned>(m_recordSize))
			{
				//this char should be the LAST char in the row
				rowInfo.FoundEndOfLine(c, c - lastTerminator);
				lastTerminator = lastDelimiter = lastCommaDelimiter = lastTabDelimiter = (c-1);
			}

			break;
		}
	}
	if ( !terminator.empty() )
	{
		rowInfo.SetTerminator(terminator.c_str());
	}
}

bool CFileAnalyzer::AnalyzeResults()
{
	/* We would expect to have just one big cell/row of data.
	*/
	if ( ScanForXML(m_table) )
	{
		m_recordInfo.SetXmlFileFormat(false);
	}
	else
	{
		__int64 numRows = m_table->GetRowCount();
		for (int i=m_recordInfo.GetHeaderRows(); i<numRows; i++ )
		{
			ScanRow(m_table,i,m_recordInfo);
		}
		m_recordInfo.CompletedScan(int(numRows-m_recordInfo.GetHeaderRows()), int(m_recordSize*numRows), m_fileSize);
	}

	return true;
}

bool CFileAnalyzer::CompletedAnalysis(Dali::IWorkunit *workunit)
{
	// TODO TJ - exception count can be !=0 for certain benign warnings
	// so here we should exclude them, after finding out how to distinguish them

	if ( workunit && workunit->GetExceptionCount() == 0 )
	{
		m_table = ::CreateISparseTable();

		Dali::IResultVector::const_iterator itr, end;
		boost::tie(itr, end) = workunit->GetResultIterator();
		if (itr != end)
		{
			if ( itr->get()->GetResultData(m_recordInfo.GetHeaderRows(), m_records, m_table) )
			{
				if(m_table->GetColumnCount()==1)
				{
					CString buffer;
					CResultFormatter dummyfmt;
					m_table->SaveAs(buffer, dummyfmt);
					int len = buffer.GetLength();
					m_table->DoClear();
					m_table->SetCell(0, 0, buffer);
				}

				AnalyzeResults();
			}
		}

		return true; 
	}
	return false;
}

bool CFileAnalyzer::GetAnalyzerEcl(CString &ecl, const TCHAR *cluster, Dali::IWorkunit *workunit)
{
	if ( CompletedAnalysis(workunit) )
		return false;

// TJ testing code commented out from SampleView::GetEclHeader(ecl, cluster);
	/*
	#workunit('name','GEtl Sample')
	#workunit('cluster','thor')

	r := record 
		data1024 col1; 
	end;
	d := DATASET('~file::10.150.50.53::c$::import::471Z_despray_fixed.txt', r, THOR);
	OUTPUT(CHOOSEN(d,2048));

	*/
	m_recordSize = GetFileRecordSize(m_fileSize, m_recordInfo.GetFixedWidth());
	//get a few records, but not too many or too few
	m_records = m_probeRecords;
	int querySize = m_records * m_recordSize;
	if ( querySize < (9*1024))
		m_records = (16*1024) / m_recordSize; //bump it up
	else if ( querySize > MAXFILEBUFFERSIZE)
		m_records = MAXFILEBUFFERSIZE / m_recordSize; //take it down
	ATLASSERT( m_records );
	ATLTRACE(_T("CFileAnalyzer::GetAnalyzerEcl(recordSize=%d, totalSize=%d)\r\n"), m_recordSize, m_records * m_recordSize );

	m_records += m_recordInfo.GetHeaderRows();

	std::_tstring format;
	CString query;
	CString encodedFileName;

	query.FormatMessage(_T("\r\nr := record\tdata%1!d! col1;\r\nend;\r\nd := DATASET(%3, r, %4);\r\nOUTPUT(CHOOSEN(d,%2!d!));\r\n"),
		m_recordSize, m_records, ::EncodeForEcl(m_recordInfo.GetFile(), encodedFileName, true), 
		m_recordInfo.GetEclFormat(format) );
	ecl += query;
	return true;
}


