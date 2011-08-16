#include "StdAfx.h"

#include "UnicodeFile.h"

#define BLOCKSIZE 32768

CUnicodeFile::CUnicodeFile() 
{
}

CUnicodeFile::~CUnicodeFile()
{
}

bool CUnicodeFile::IsOpen() const
{
	return NULL != (HANDLE)m_file;
}

void CUnicodeFile::Attach(HANDLE hFile)
{
	ATLASSERT(!m_file.m_h);
	m_file.Attach(hFile);
}

HANDLE CUnicodeFile::Detach()
{
	ATLASSERT(m_file.m_h);
	return m_file.Detach();
}

bool CUnicodeFile::Create(const TCHAR * filePath, int readWriteMode, int createMode, ENCODING encoding)
{
	m_filePath = filePath;
	return Create(readWriteMode, createMode, encoding );
}

bool CUnicodeFile::Create(const boost::filesystem::path & filePath, int readWriteMode, int createMode, ENCODING encoding)
{
	std::_tstring _tfilePath = CA2T(filePath.native_file_string().c_str());
	return Create(_tfilePath.c_str(), readWriteMode, createMode, encoding);
}

bool CUnicodeFile::Create(int readWriteMode, int createMode, ENCODING encoding)
{
	if (m_file.Create(m_filePath.c_str(), readWriteMode, FILE_SHARE_READ, createMode) == S_OK)
	{
		m_encoding = encoding;
		char bom[4];
		DWORD bytesWritten;
		switch(encoding)
		{
		case ENCODING_UTF_16:
			bom[0] = (unsigned short)0xFF;
			bom[1] = (unsigned short)0xFE;
			m_file.Write(bom, 2, &bytesWritten);
			ATLASSERT(bytesWritten == 2);
			break;
		case ENCODING_UTF_8:
			bom[0] = (unsigned short)0xEF;
			bom[1] = (unsigned short)0xBB;
			bom[2] = (unsigned short)0xBF;
			m_file.Write(bom, 3, &bytesWritten);
			ATLASSERT(bytesWritten == 3);
			break;
		case ENCODING_ANSI:
		default:
			break;
		}
		return true;
	}
	return false;
}

bool CUnicodeFile::Open(const boost::filesystem::path & filePath, int readWriteMode)
{
	std::_tstring _tfilePath = CA2T(filePath.native_file_string().c_str());
	return Open(_tfilePath.c_str(), readWriteMode);
}

bool CUnicodeFile::Open(const TCHAR * filePath, int readWriteMode)
{
	m_filePath = filePath;
	if (m_file.Create(m_filePath.c_str(), readWriteMode, 0, OPEN_EXISTING) == S_OK)
	{
		char bom[4];
		m_file.Read(bom, 4);
		unsigned short byte1 = bom[0];
		unsigned short byte2 = bom[1];
		unsigned short byte3 = bom[2];
		if(byte1 == 0xFFEF && byte2 == 0xFFBB && byte3 == 0xFFBF)
		{
			m_file.Seek(3, FILE_BEGIN);
			m_encoding = ENCODING_UTF_8;
		}
		else if(byte1 == 0xFFFF && byte2 == 0xFFFE)
		{
			m_file.Seek(2, FILE_BEGIN);
			m_encoding = ENCODING_UTF_16;
		}
		else
		{
			m_file.Seek(0, FILE_BEGIN);
			m_encoding = ENCODING_ANSI;
		}
		return true;
	}
	return false;
}

void CUnicodeFile::Close()
{
	m_file.Close();
}

bool CUnicodeFile::Empty()
{
	return (m_file.SetSize(0) == S_OK);
}

void CUnicodeFile::Write(const std::_tstring & data)
{
	Write(data.c_str());
}

void CUnicodeFile::Write(const TCHAR* data)
{
	std::string convertedData;
	std::wstring convertedWideData;
	switch(m_encoding)
	{
	case ENCODING_UTF_16:
		convertedWideData = CT2W(data);
		break;
	case ENCODING_UTF_8:
		convertedData = CT2A(data, CP_UTF8);
		break;
	case ENCODING_ANSI:
	default:
		convertedData = CT2A(data);
		break;
	}

	DWORD bytesWritten;

	if(m_encoding==ENCODING_UTF_16)
	{
		unsigned writeSize = convertedWideData.length()*sizeof(wchar_t);
		m_file.Write(convertedWideData.c_str(), writeSize, &bytesWritten);
		ATLASSERT(bytesWritten == writeSize);
	}
	else
	{
		unsigned dataLength = convertedData.length();
		for(unsigned int i = 0; i < dataLength; i += BLOCKSIZE)
		{
			unsigned int writeSize = dataLength - i;
			if (writeSize > BLOCKSIZE)
				writeSize = BLOCKSIZE;
			m_file.Write(convertedData.c_str() + i, writeSize, &bytesWritten);
			ATLASSERT(bytesWritten == writeSize);
		}
	}
}

bool CUnicodeFile::Read(CComVariant & data)
{
	std::_tstring content;
	bool retVal = Read(content);
	data = content.c_str();
	return retVal;
}

bool CUnicodeFile::Read(std::_tstring & data)
{
	std::string rawData;
	std::wstring rawWideData;

	ULONGLONG size, pos;
	if (m_file.GetSize(size) == S_OK && m_file.GetPosition(pos) == S_OK)
	{
		size -= pos;
		DWORD bytesRead;
		if(m_encoding==ENCODING_UTF_16)
		{
			rawWideData.resize(size/sizeof(wchar_t));
			m_file.Read((LPVOID)rawWideData.c_str(), size, bytesRead);
			ATLASSERT(bytesRead == size);
		}
		else
		{
			rawData.resize(static_cast<std::_tstring::size_type>(size));
			for(int i = 0; i < size; i += BLOCKSIZE)
			{
				unsigned int readSize = rawData.length() - i;
				if (readSize > BLOCKSIZE)
					readSize = BLOCKSIZE;
				m_file.Read(const_cast<char *>(rawData.c_str()) + i, readSize, bytesRead);
				ATLASSERT(bytesRead == readSize);
			}
		}
		switch(m_encoding)
		{
		case ENCODING_UTF_16:
			data = CW2T(rawWideData.c_str());
			break;
		case ENCODING_UTF_8:
			data = CA2T(rawData.c_str(), CP_UTF8);
			break;
		case ENCODING_ANSI:
		default:
			data = CA2T(rawData.c_str());
			break;
		}
		return true;
	}
	return false;
}

void CUnicodeFile::Append()
{
	m_file.Seek(0, FILE_END);
}