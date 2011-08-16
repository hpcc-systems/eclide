#pragma once

#include <clib.h>
#include "StdString.h"
#include ".\DelimitedColumnInfo.h"

class CEclProgramManager;

class WLIB_API CImportRecordInfo : public CUnknown
{
public:
	BEGIN_CUNKNOWN
	END_CUNKNOWN(CUnknown)

	CImportRecordInfo();
	virtual ~CImportRecordInfo() {}

	virtual CImportRecordInfo& operator=(const CImportRecordInfo& other);
	virtual void Clear();
	bool IsEmpty();

	//this will use MetaData comment tags to store data in ecl code*/
	void Serialize(CString& ecl) const;
	void Serialize(CEclProgramManager& program) const;
	bool Deserialize(CEclProgramManager& program);

	typedef enum { eUnknown, eFixed, eDelimited, eXml } eFileFormat;

	const TCHAR* GetFile() const;

	bool IsVariableFileFormat() const;
	bool IsFixedFileFormat() const;
	bool IsXmlFileFormat() const;

	bool IsCommaSeparated() const;
	bool IsTabSeparated() const;
	TCHAR GetDelimiter() const;
	const TCHAR * GetDelimiter(StdString& delimiter) const;
	TCHAR GetQuote() const;
	void GetQuote(StdString& quote) const;
	const TCHAR * GetTerminator(StdString& terminator) const;

	const TCHAR* GetEclFormat(std::_tstring & format, bool bParams=true) const;

	int GetFixedWidth() const;
	int GetHeaderRows() const;
	__int64 GetFileSize() const;

	void GetXMLRowTag(StdString& tag) const;
	void SetXmlRowTag(const TCHAR* tag);

	void SetFile(const TCHAR *file);
	void SetFileSize(__int64 fileSize);
	void SetTerminator(const TCHAR *terminator);
	void SetQuote(const TCHAR* quote);
	void SetHeaderRows(unsigned rows);
	void SetDelimiter(const TCHAR *delimiter);
	void SetFixedWidth(int width);

	void SetEclFormat(const TCHAR* fileType);
	void SetFileFormat(eFileFormat eFormat);
	void SetFixedFileFormat();
	void SetXmlFileFormat();
	void SetDelimitedFileFormat();

protected:

	StdString	m_file;
	__int64		m_fileSize;		//if known

	int			m_headerRows;	//may need to skip some rows - need terminator though

	//eDelimited
	StdString	m_terminator;
	StdString   m_quote;
	StdString	m_delimiter;

	//eXML
	StdString	m_rowTag;

	//eFixed
	int			m_fixedWidth;

	eFileFormat m_eFormat;
};
typedef StlLinked<CImportRecordInfo> CImportRecordInfoAdapt;

//use these to fixup the Delimiter and Terminators in dialogs
void WLIB_API FixupParamForDisplay(StdString& str);
void WLIB_API FixupParamForUse(StdString& str);


