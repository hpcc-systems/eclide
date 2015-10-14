#pragma once

#include "clib.h"
#include "dali.h"

class CListRow : public CUnknown
{
protected:
    CString m_nullstr;
    WTL::CListViewCtrl &m_listCtrl;

public:
    CListRow(WTL::CListViewCtrl &listctrl);
    int InsertItem(WTL::CListViewCtrl *lc, int nItem = INT_MAX);//, WU_STATE state);
    void GetDispInfo(NMLVDISPINFO* plvdi);
    virtual void GetText(unsigned column, CString & result);
    virtual int GetDispImage();
};

//class CCellRow : public CListRow
//{
//	CellT * m_cell;
//
//public:
//	CCellRow(WTL::CListViewCtrl &listctrl, CellT * cell) : CListRow(listctrl)
//	{
//		m_cell = cell;
//	}
//
//	virtual void GetText(unsigned column, CString & result)
//	{
//		result = m_cell->operator [](column);
//	}
//};
//
//
