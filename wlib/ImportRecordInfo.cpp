//$Id: ImportRecordInfo.cpp 3310 2007-02-13 15:22:24Z tjuricic $ 
#include "StdAfx.h"
#include "ImportRecordInfo.h"
#include "eclprogrammanager.h"
#include "util.h"

CImportRecordInfo::CImportRecordInfo()
{
	Clear();
}

CImportRecordInfo& CImportRecordInfo::operator=(const CImportRecordInfo& other)
{
	m_headerRows = other.m_headerRows;
	m_terminator = other.m_terminator;
	m_quote = other.m_quote;
	m_delimiter = other.m_delimiter;
	m_file = other.m_file;
	m_fileSize = other.m_fileSize;
	m_fixedWidth = other.m_fixedWidth;
	m_eFormat = other.m_eFormat;
	return *this;
}

void CImportRecordInfo::Clear()
{
	m_headerRows = 0;
	m_eFormat = eFixed;
	m_terminator.clear();
	m_delimiter.clear();
	m_quote.clear();
	m_file.clear();
	m_fixedWidth = 1;
	m_fileSize = 0;
}

bool CImportRecordInfo::IsEmpty()
{
	return (m_eFormat == eUnknown);
}

const TCHAR TAG_SOURCEFILE[] = _T("SourceFile");
const TCHAR TAG_SOURCEFILETYPE[] = _T("SourceFileType");
const TCHAR TAG_SOURCEFILEDELIMITER[] = _T("SourceFileDelimiter");
const TCHAR TAG_SOURCEFILEQUOTECHAR[] = _T("SourceFileQuoteChar");
const TCHAR TAG_SOURCEFILETERMINATOR[] = _T("SourceFileTerminator");
const TCHAR TAG_SOURCEFILEXMLPATH[] = _T("SourceFileXmlPath");
const TCHAR TAG_SOURCEFILEFIXED[] = _T("SourceFileRecordWidth");

void SetGrETLCommentString(CEclProgramManager& program, const TCHAR* tag, const StdString& value)
{
	ATLASSERT(!"Not Used");
	/*
	if ( value.empty() )
		program.DeleteGrETLComment(tag);
	else
		program.UpdateGrETLComment(tag,value.c_str(),true);
		*/
}


enum { eDelimiter, eQuoteChar, eTerminator, eVariableTagsSize };
static const TCHAR* VariableTags[eVariableTagsSize] = 
{
	TAG_SOURCEFILEDELIMITER,
	TAG_SOURCEFILEQUOTECHAR,
	TAG_SOURCEFILETERMINATOR
};

enum { eRecordWidth, eFixedTagsSize };
static const TCHAR* FixedTags[eFixedTagsSize] = 
{
	TAG_SOURCEFILEFIXED
};


bool CImportRecordInfo::Deserialize(CEclProgramManager& program)
{
	ATLASSERT(!"Not Used");
	return false;
	/*
	program.CheckForGrETLComment(TAG_SOURCEFILE,m_file);

	StdString type;
	if ( program.CheckForGrETLComment(TAG_SOURCEFILETYPE,type) )
	{
		SetEclFormat(type.c_str());
		if ( IsVariableFileFormat() )
		{
			program.CheckForGrETLComment(VariableTags[eDelimiter],m_delimiter);
			program.CheckForGrETLComment(VariableTags[eQuoteChar],m_quote);
			program.CheckForGrETLComment(VariableTags[eTerminator],m_terminator);			
		}
		else if ( IsFixedFileFormat() )
		{
			StdString width;
			if ( program.CheckForGrETLComment(FixedTags[eRecordWidth],width) )
			{
				::StringToInt(width.c_str(),m_fixedWidth);
			}
		}
	}

	return !m_file.empty();
	*/
}

void ClearTags(CEclProgramManager& program, const TCHAR* tags[], unsigned nSize )
{
	ATLASSERT(!"Not Used");
	/*
	for (unsigned i=0; i<nSize; i++ )
		program.DeleteGrETLComment(tags[i]);
		*/
}

void CImportRecordInfo::Serialize(CEclProgramManager& program) const
{
	ATLASSERT(!"Not Used");
	/*
	CString format;
	GetEclFormat(format,false);

	if ( !program.GetProgram() )
	{
		//no existing program? - make one
		CString GrETLComment;
		Serialize(GrETLComment);
		program.SetEcl(GrETLComment);
	}
	else
	{
		program.UpdateGrETLComment(TAG_SOURCEFILETYPE,format,true);
		if ( IsVariableFileFormat() )
		{
			SetGrETLCommentString(program, VariableTags[eDelimiter], m_delimiter);
			SetGrETLCommentString(program, VariableTags[eQuoteChar], m_quote);
			SetGrETLCommentString(program, VariableTags[eTerminator], m_terminator);
		}
		else
		{
			ClearTags(program,VariableTags,eVariableTagsSize);
		}

		if ( IsFixedFileFormat() )
		{
			StdString width;
			::IntToString(GetFixedWidth(),width);
			SetGrETLCommentString(program, FixedTags[eRecordWidth], width);
		}
		else
		{
			ClearTags(program,FixedTags,eFixedTagsSize);
		}

		program.UpdateGrETLComment(TAG_SOURCEFILE,m_file.c_str(),true);
	}
	*/
}

void CImportRecordInfo::Serialize(CString& ecl) const
{
	ATLASSERT(!"Not Used");
	/*
	CString format;
	GetEclFormat(format,false);

	CEclProgramManager::CreateGrETLComment(ecl,TAG_SOURCEFILE, m_file.c_str());
	CEclProgramManager::CreateGrETLComment(ecl,TAG_SOURCEFILETYPE, format);
	if ( IsVariableFileFormat() )
	{
		CEclProgramManager::CreateGrETLComment(ecl,VariableTags[eDelimiter], m_delimiter.c_str());
		CEclProgramManager::CreateGrETLComment(ecl,VariableTags[eQuoteChar], m_quote.c_str());
		CEclProgramManager::CreateGrETLComment(ecl,VariableTags[eTerminator], m_terminator.c_str());
	}
	if ( IsFixedFileFormat() )
	{
		StdString width;
		::IntToString(GetFixedWidth(),width);
		CEclProgramManager::CreateGrETLComment(ecl, FixedTags[eRecordWidth], width.c_str());
	}
	*/
}

int CImportRecordInfo::GetFixedWidth() const
{
	return m_fixedWidth;
}

void CImportRecordInfo::SetFixedWidth(int width)
{
	m_fixedWidth = width;
}

void CImportRecordInfo::SetFile(const TCHAR *file)
{
	m_file = file;
}

const TCHAR* CImportRecordInfo::GetFile() const
{
	return m_file.c_str();
}

void CImportRecordInfo::SetFileSize(__int64 fileSize)
{
	m_fileSize = fileSize;
}

__int64 CImportRecordInfo::GetFileSize() const
{
	return m_fileSize;
}

int CImportRecordInfo::GetHeaderRows() const
{
	return m_headerRows;
}

void CImportRecordInfo::GetXMLRowTag(StdString& tag) const
{
	tag = m_rowTag;
}

void CImportRecordInfo::SetXmlRowTag(const TCHAR* tag)
{
	m_rowTag = tag;
}

void CImportRecordInfo::SetQuote(const TCHAR* quote)
{
	m_quote = quote;
}

void CImportRecordInfo::SetTerminator(const TCHAR *terminator)
{
	ATLASSERT(terminator[0]); //can't be empty
	m_terminator = terminator;
}

const TCHAR * CImportRecordInfo::GetTerminator(StdString& terminator) const
{
	if ( m_terminator.size() )
		terminator = m_terminator;
	else
		terminator = _T("\\n,\\r\\n");
	return terminator.c_str();
}

TCHAR CImportRecordInfo::GetDelimiter() const
{
	if ( m_delimiter.size() )
		return m_delimiter[0];
	else
		return ',';
}

const TCHAR * CImportRecordInfo::GetDelimiter(StdString& delimiter) const
{
	delimiter = m_delimiter;
	return delimiter.c_str();
}


TCHAR CImportRecordInfo::GetQuote() const
{
	if ( !m_quote.empty() )
		return m_quote[0];
	else
		return '"';
}

void CImportRecordInfo::GetQuote(StdString& quote) const
{
	quote = m_quote;
}

void CImportRecordInfo::SetDelimiter(const TCHAR *delimiter)
{
	m_delimiter = delimiter;
}

void CImportRecordInfo::SetHeaderRows(unsigned rows)
{
	m_headerRows = rows;
}

void CImportRecordInfo::SetFileFormat(eFileFormat eFormat)
{
	m_eFormat = eFormat; 
}
void CImportRecordInfo::SetXmlFileFormat()
{
	m_eFormat = eXml; 
}
void CImportRecordInfo::SetDelimitedFileFormat()
{
	m_eFormat = eDelimited; 
}
void CImportRecordInfo::SetFixedFileFormat()
{
	m_eFormat = eFixed; 
}

bool CImportRecordInfo::IsVariableFileFormat() const
{
	return m_eFormat == eDelimited;
}

bool CImportRecordInfo::IsFixedFileFormat() const
{
	return m_eFormat == eFixed;
}

bool CImportRecordInfo::IsXmlFileFormat() const
{
	return m_eFormat == eXml;
}

bool CImportRecordInfo::IsCommaSeparated() const
{
	return (m_delimiter.size() == 1 && m_delimiter[0] == ',');
}

bool CImportRecordInfo::IsTabSeparated() const
{
	return (m_delimiter.size() == 1 && m_delimiter[0] == '\t');
}

const TCHAR* CImportRecordInfo::GetEclFormat(std::_tstring & format, bool bParams) const
{
	if ( IsXmlFileFormat() )
	{
		format = _T("XML");
	}
	else if ( IsVariableFileFormat() )
	{
		if ( bParams )
			CEclProgramManager::GetEclFormatVariable(m_headerRows, m_delimiter, m_terminator, m_quote, format);
		else
			format = _T("CSV");
	}
	else
	{
		format = _T("Flat"); //this is really same as THOR
	}
	return format.c_str();
}

void CImportRecordInfo::SetEclFormat(const TCHAR* fileType)
{
	CString type(fileType);
	if ( type.CompareNoCase(_T("XML")) == 0 )
		SetXmlFileFormat();
	else if ( type.CompareNoCase(_T("CSV")) == 0 )
		SetDelimitedFileFormat();
	else
		SetFixedFileFormat();
}

//show non printable chars
void FixupParamForDisplay(StdString& str)
{
	::Replace(str, '\t', _T("\\t"));
	::Replace(str, '\n', _T("\\n"));
	::Replace(str, '\r', _T("\\r"));
}

void FixupParamForUse(StdString& str)
{
	::Replace(str, _T("\\t"), _T("\t"));
	::Replace(str, _T("\\n"), _T("\n"));
	::Replace(str, _T("\\r"), _T("\r"));
}

