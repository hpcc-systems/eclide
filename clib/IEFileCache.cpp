#include "StdAfx.h"

#include "iefilecache.h"

TCHAR * NewUniqueID(TCHAR *pszBuffer)
{
	//NOT DONE - NEED TO GENERATE A UNIQUE ID FOR THE FILE
   pszBuffer[0] = 'p';	
   pszBuffer[32] = 0;
   return pszBuffer;

} //NewUUIDAsString


CIECacheFile::CIECacheFile(const TCHAR *pszFileExtension, 
						   ENCODING encoding, 
						   unsigned nMaxLength)
{
	CString URL(_T("temp://"));
	TCHAR uuid[40];
	URL += NewUniqueID(uuid);
   
    BOOL bCreated = !::CreateUrlCacheEntry(URL, nMaxLength,
                                pszFileExtension ? pszFileExtension : _T("tmp"),
                                m_filePath.GetBuffer(MAX_PATH+4), 0);
	m_filePath.ReleaseBuffer();
	if ( !bCreated )
    {
        ATLTRACE(_T("CreateUrlCacheEntry(%s) failed(%x)\n"), (const TCHAR *)URL, ::GetLastError());
		//throw?
	}
	else
	{
		Create(GENERIC_WRITE, CREATE_ALWAYS,encoding);
	}

  
}

CIECacheFile::~CIECacheFile()
{
   closeCacheFile();
}

void CIECacheFile::Close()
{
   closeCacheFile();
}

void CIECacheFile::closeCacheFile()
{
	//NOT DONE - NEED TO FINISH THIS
	/*
	FILETIME ftExpire = {0,0};
	FILETIME ftModified = {0,0};
     if (!::CommitUrlCacheEntry(m_pszURL, m_szCacheFileName, ftExpire,
                                ftModified, NORMAL_CACHE_ENTRY, 0, 0, 0, 0) )
	*/
}
