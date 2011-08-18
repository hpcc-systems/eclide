#pragma once

#include "clib.h"

class CLIB_API CResultFormatter : public CUnknown
{
public:
	IMPLEMENT_CUNKNOWN;
	
	CResultFormatter();
	virtual ~CResultFormatter() {};

	//defalt implementation - do nothing
	virtual const TCHAR * Format(const TCHAR *rawString, std::_tstring &result, bool stringFlag) const;
	virtual void FormatEx(const TCHAR *rawString, std::_tstring &line, bool bFirstColumn, bool stringFlag) const;
	virtual void FormatEndOfLine(std::_tstring &line) const;
	virtual unsigned FileEncoding() const; //CUnicodeFile::ENCODING_UTF_8
	virtual bool FormatHeader(const TCHAR *rawString, std::_tstring &line, bool bFirstColumn) const;
	virtual bool FormatsAsXml() const; //silly but XML formatting not implemented yet
	virtual CResultFormatter *Clone() const;

protected:
	const TCHAR *m_separator;
	const TCHAR *m_endOfLine;
};

class CLIB_API CResultFormatterAsHex : public CResultFormatter
{
public:
	virtual CResultFormatter *Clone() const;
	virtual const TCHAR * Format(const TCHAR *rawString, std::_tstring &result, bool stringFlag) const;
};

class CLIB_API CResultFormatterAsECL : public CResultFormatter
{
public:
	CResultFormatterAsECL();
	virtual CResultFormatter *Clone() const;
	virtual const TCHAR * Format(const TCHAR *rawString, std::_tstring &result, bool stringFlag) const;
};

class CLIB_API CResultFormatterAsCSV : public CResultFormatter
{
public:
	CResultFormatterAsCSV();

	virtual CResultFormatter *Clone() const;
	virtual const TCHAR * Format(const TCHAR *rawString, std::_tstring &result, bool stringFlag) const;
	virtual unsigned FileEncoding() const; //ENCODING_UNKNOWN
	virtual bool FormatHeader(const TCHAR *rawString, std::_tstring &line, bool bFirstColumn) const;
};

class CLIB_API CResultFormatterAsTabDelimited : public CResultFormatter
{
public:
	CResultFormatterAsTabDelimited();

	virtual CResultFormatter *Clone() const;
};

class CLIB_API CResultFormatterAsXML : public CResultFormatter
{
public:
	CResultFormatterAsXML();

	virtual CResultFormatter *Clone() const;
	virtual const TCHAR * Format(const TCHAR *rawString, std::_tstring &result, bool stringFlag) const;
	virtual bool FormatsAsXml() const;
};