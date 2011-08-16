#pragma once

#include "clib.h"
#include "unicodefile.h"

class CLIB_API CIECacheFile : public CUnicodeFile
{
public:
	CIECacheFile(const TCHAR *pszFileExtension, ENCODING encoding = ENCODING_UTF_8, unsigned nMaxLength=0);
	~CIECacheFile();

	virtual void Close();

protected:
	void closeCacheFile();
};


