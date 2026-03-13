#include "StdAfx.h"
#include "..\en_us\resource.h"

#include "listrow.h"

CListRow::CListRow(WTL::CListViewCtrl &listCtrl) : m_listCtrl(listCtrl)
{
}

int CListRow::InsertItem(WTL::CListViewCtrl *lc, int nItem)
{
    return lc->InsertItem(LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM, nItem, LPSTR_TEXTCALLBACK, 0, 0, I_IMAGECALLBACK, reinterpret_cast<LPARAM>(this));
}

void CListRow::GetDispInfo(NMLVDISPINFO* plvdi)
{
    if (-1 == plvdi->item.iItem){
        //                OutputDebugString("LVOWNER: Request for -1 item?\n");
        //            DebugBreak();
    }
    // Retrieve information for item at index iItem.
    //            RetrieveItem( &rndItem, plvdi->item.iItem );

    if(plvdi->item.mask & LVIF_STATE)
    {
        // Fill in the state information.
        //                plvdi->item.state |= rndItem.state;
    }
    if(plvdi->item.mask & LVIF_IMAGE)
    {
        plvdi->item.iImage = GetDispImage();
    }
    if(plvdi->item.mask & LVIF_TEXT){
        CString txt;
        GetText(plvdi->item.iSubItem, txt);
        if (plvdi->item.pszText != NULL && plvdi->item.cchTextMax > 0)
            _tcsncpy_s(plvdi->item.pszText, plvdi->item.cchTextMax, txt, _TRUNCATE);
    }

}

void CListRow::GetText(unsigned /*column*/, CString & result)
{
    result = m_nullstr;
}

int CListRow::GetDispImage()
{
    return 0;
}
