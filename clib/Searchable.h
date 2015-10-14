#pragma once

#include "clib.h"

class CSearchableResult;
class CSearchableInfo;

__interface ISearchable
{
    bool CanBeSearched() const;
    bool Search(const CSearchableInfo &info, CSearchableResult &result) const;
};

#ifndef _INC_COMMDLG
//search flags are from common dialog if that isn't included, these will do
#define FR_DOWN                         0x00000001
#define FR_WHOLEWORD                    0x00000002
#define FR_MATCHCASE                    0x00000004
#define FR_FINDNEXT                     0x00000008
#endif

class CSearchableInfo
{
public:
    CSearchableInfo()
    {
        m_searchFrom = -1;
        m_searchFlags = 0;
    }
protected:
    __int64	 m_searchFrom; //-1 is beginning
    unsigned m_searchFlags;
    std::_tstring m_casedSearchString;

public:
    void Init(const TCHAR *searchString, 
              __int64	 searchFrom, 
              unsigned	 searchFlags)
    {
        m_searchFrom = searchFrom;
        Init(searchString,searchFlags);
    }
    void Init(const TCHAR *searchString, 
              unsigned	  searchFlags)
    {
        m_searchFlags = searchFlags;
        m_casedSearchString = searchString;
        if (!MatchCase())
            boost::algorithm::to_lower(m_casedSearchString);
    }
    void SetPosition(__int64 searchFrom)
    {
        m_searchFrom = searchFrom;
    }
    bool inline SearchDown() const { return (m_searchFlags & FR_DOWN) != 0; }
    bool inline FindNext() const {	return (m_searchFlags & FR_FINDNEXT) != 0; }
    bool inline MatchCase() const { return (m_searchFlags & FR_MATCHCASE) != 0; }
    bool inline MatchWholeWord() const { return (m_searchFlags & FR_WHOLEWORD) != 0; }
    __int64 StartRow() const 
    { 
        if ( m_searchFrom < 0 )
            return 0;
        else if ( FindNext() )
            return SearchDown() ? m_searchFrom + 1 : m_searchFrom - 1;
        else
            return m_searchFrom;
    }

    bool Find(const std::_tstring & strValue) const
    {
        if (MatchCase())
            return (boost::algorithm::contains(strValue, m_casedSearchString));
        else
        {
            return (boost::algorithm::icontains(strValue, m_casedSearchString));
        }
    }
};

class CSearchableResult
{
public:
    CSearchableResult()
    {
        m_found = false;
        m_row = m_col = -1;
        m_item = 0;
    }
    void SetResult(	__int64		row,
                    __int64		col = 0,
                    bool		found = true)
    {
        m_found = found;
        m_row = row;
        m_col = col;
        m_item = 0;
    }
    void SetResult(	CUnknown	*item,
                    bool		found = true)
    {
        m_found = found;
        m_row = m_col = -1;
        m_item = item;
    }

    bool		m_found;
    __int64		m_row;
    __int64		m_col;
    CUnknown	*m_item;
};

