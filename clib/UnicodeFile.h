#pragma once

#include "clib.h"
#include <atlfile.h>

class CLIB_API CUnicodeFile
{
public:
	enum ENCODING
	{
		ENCODING_UNKNOWN,
		ENCODING_ANSI,
		ENCODING_UTF_8,
		ENCODING_UTF_16
	};

protected:
	std::_tstring m_filePath;
	CAtlFile m_file;
	int m_encoding;

	bool Create(int readWriteMode = GENERIC_WRITE, int createMode = CREATE_ALWAYS, ENCODING encoding = ENCODING_UTF_8);

public:
	CUnicodeFile();
	virtual ~CUnicodeFile();

	bool IsOpen() const;
	bool Create(const boost::filesystem::path & filePath, int readWriteMode = GENERIC_WRITE, int createMode = CREATE_ALWAYS, ENCODING encoding = ENCODING_UTF_8);
	bool Create(const TCHAR * filePath, int readWriteMode = GENERIC_WRITE, int createMode = CREATE_ALWAYS, ENCODING encoding = ENCODING_UTF_8);
	bool Open(const boost::filesystem::path & filePath, int readWriteMode = GENERIC_READ);
	bool Open(const TCHAR * filePath, int readWriteMode = GENERIC_READ);
	virtual void Close();
	bool Empty();
	void Write(const TCHAR* data);
	void Write(const std::_tstring & data);
	bool Read(CComVariant & data);
	bool Read(std::_tstring & data);
	const TCHAR * GetFilePath() { return m_filePath.c_str(); };
	void Append();
	void Attach(HANDLE hFile);
	HANDLE Detach();
};