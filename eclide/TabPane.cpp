#include "StdAfx.h"

#include "TabPane.h"

void CTabPane::SetNamePath(const CString & sPathName)
{	
    CString pathName = sPathName;
    TCHAR szTitle [_MAX_FNAME], szExt[_MAX_FNAME];
    _tsplitpath(pathName.GetBuffer(_MAX_PATH), NULL, NULL, szTitle, szExt);
    pathName.ReleaseBuffer();
    lstrcat(szTitle, szExt);			

    m_path = sPathName;
    if (szTitle) 
        m_name = szTitle;
}
