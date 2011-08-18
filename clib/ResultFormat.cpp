#include "StdAfx.h"

#include ".\resultformat.h"
#include ".\util.h"
#include ".\UnicodeFile.h"

CResultFormatter::CResultFormatter()
{
	m_separator = 0;
	m_endOfLine = 0;
}

CResultFormatter *CResultFormatter::Clone() const
{
	return new CResultFormatter();
}


const TCHAR * CResultFormatter::Format(const TCHAR *rawString, std::_tstring &result, bool stringFlag) const
{ 
	result = rawString;
	return result.c_str(); 
}

void CResultFormatter::FormatEx(const TCHAR *rawString, std::_tstring &line, bool bFirst, bool stringFlag) const
{
	std::_tstring formatted;
	Format(rawString, formatted, stringFlag);
	if ( !bFirst && m_separator )
		line += m_separator;
	line += formatted;
}

void CResultFormatter::FormatEndOfLine(std::_tstring &line) const
{
	if (line.length() && m_endOfLine)
		line += m_endOfLine;
}

unsigned CResultFormatter::FileEncoding() const
{
	return CUnicodeFile::ENCODING_UTF_8;
}

bool CResultFormatter::FormatHeader(const TCHAR * /*rawString*/, std::_tstring &/*line*/, bool /*bFirstColumn*/) const
{
	//don't do anything, don't want any header
	return false;
}

bool CResultFormatter::FormatsAsXml() const
{
	return false;
}

//==============================================================================
CResultFormatter *CResultFormatterAsHex::Clone() const
{
	return new CResultFormatterAsHex();
}

const TCHAR * CResultFormatterAsHex::Format(const TCHAR *rawString, std::_tstring &result, bool stringFlag) const
{ 
	//treat this as a byte array
	//const BYTE *raw = reinterpret_cast<const BYTE *>(rawString);
	int len = _tcslen(rawString);
	result.resize(len * 2);
	TCHAR *buffer = &result[0];
	for (int i=0; i<len; i++)
	{
		int index = i*2;
		_itot(rawString[i],buffer+index,16);
		//if only one char, add a leading zero
		if ( !buffer[index+1] )
		{
			buffer[index+1] = buffer[index];
			buffer[index] = '0';
			buffer[index+2] = 0;

		}
	}
	return result.c_str(); 
}

//==============================================================================
CResultFormatter *CResultFormatterAsECL::Clone() const
{
	return new CResultFormatterAsECL();
}

CResultFormatterAsECL::CResultFormatterAsECL()
{
	m_separator = _T(",");
	m_endOfLine = _T("];\r\n");
}

const TCHAR * CResultFormatterAsECL::Format(const TCHAR *rawString, std::_tstring &result, bool stringFlag) const
{
	if (stringFlag)
	{
		//simple case for a string
		return ::EncodeForEcl(rawString,result);
	}
	return ::EncodeForEclNoQuote(rawString,result);
}

//==============================================================================

CResultFormatterAsCSV::CResultFormatterAsCSV()
{
	m_separator = _T(",");
	m_endOfLine = _T("\r\n");
}

CResultFormatter *CResultFormatterAsCSV::Clone() const
{
	return new CResultFormatterAsCSV();
}

const TCHAR * CResultFormatterAsCSV::Format(const TCHAR *rawString, std::_tstring &result, bool stringFlag) const
{ 
	return ::EncodeForCSV(rawString,result);
}

unsigned CResultFormatterAsCSV::FileEncoding() const
{
	//excel doesn't want a BOM, so do unknown
	return CUnicodeFile::ENCODING_UNKNOWN;
}

bool CResultFormatterAsCSV::FormatHeader(const TCHAR *rawString, std::_tstring &line, bool bFirstColumn) const
{
	//for CSV, we write out the header
	FormatEx(rawString,line,bFirstColumn, true);
	return true;
}

//=============================================================================
CResultFormatterAsTabDelimited::CResultFormatterAsTabDelimited()
{
	m_separator = _T("\t");
	m_endOfLine = _T("\r\n");
}

CResultFormatter *CResultFormatterAsTabDelimited::Clone() const
{
	return new CResultFormatterAsTabDelimited();
}

//==============================================================================

//==============================================================================
CResultFormatterAsXML::CResultFormatterAsXML()
{
	m_endOfLine = _T("\r\n");
}

CResultFormatter *CResultFormatterAsXML::Clone() const
{
	return new CResultFormatterAsXML();
}

const TCHAR * CResultFormatterAsXML::Format(const TCHAR *rawString, std::_tstring &result, bool stringFlag) const
{ 
	//simple case for a string
	result = _T("\"");
	result += rawString;
	result += _T("\"");
	return result.c_str(); 
}

bool CResultFormatterAsXML::FormatsAsXml() const
{
	return true;
}
