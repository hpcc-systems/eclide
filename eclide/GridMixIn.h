#pragma once

#define PROP_TEXT_INDENT 2

template<typename T>
class CGridMixIn : public WTL::CCustomDraw<T>
{
	typedef CGridMixIn thisClass;
	typedef WTL::CCustomDraw<T> baseClass;

protected:
    COLORREF m_clrText;
    COLORREF m_clrBack;
    COLORREF m_clrSelText;
    COLORREF m_clrSelBack;
    COLORREF m_clrDisabled;
    COLORREF m_clrDisabledBack;
    COLORREF m_clrButtonText;
    COLORREF m_clrButtonBack;
	COLORREF m_clrGrid;
	bool m_grayCol0;
	RECT m_rcPrePaint;
	int  m_rightItemPaint;
	int  m_bottomItemPaint;

public:
	CGridMixIn()
	{
		m_clrText = ::GetSysColor(COLOR_WINDOWTEXT);
		m_clrBack = ::GetSysColor(COLOR_WINDOW);
		m_clrSelText = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
		m_clrSelBack = ::GetSysColor(COLOR_HIGHLIGHT);
		m_clrDisabled = ::GetSysColor(COLOR_GRAYTEXT);
		m_clrDisabledBack = ::GetSysColor(COLOR_BTNFACE);
		m_clrButtonText = ::GetSysColor(COLOR_BTNTEXT);
		m_clrButtonBack = ::GetSysColor(COLOR_BTNFACE);
		m_clrGrid = ::GetSysColor(COLOR_BTNFACE);
		m_grayCol0 = false;
	}

	void SetGrayCol0(bool dogray)
	{
		m_grayCol0 = dogray;
	}

	BEGIN_MSG_MAP(thisClass)
		CHAIN_MSG_MAP_ALT(CCustomDraw<T>, 1)
	END_MSG_MAP()

	// Custom painting

	DWORD OnPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW lpNMCustomDraw )
	{
		LPNMLVCUSTOMDRAW lpNMLVCustomDraw = (LPNMLVCUSTOMDRAW) lpNMCustomDraw;
		//just fill the non item parts
		m_rcPrePaint = lpNMLVCustomDraw->nmcd.rc;
		m_rightItemPaint = m_bottomItemPaint = 0;
		//if ( m_rcPrePaint.left + m_rcPrePaint.right > 0 )
		//{
		//	ATLTRACE(_T("OnPrePaint(%d,%d - %d,%d)\n"), 
		//		m_rcPrePaint.left,m_rcPrePaint.top, 
		//		m_rcPrePaint.right,m_rcPrePaint.bottom );
		//}
		return CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYPOSTPAINT;   // We need per-item notifications
	}
	DWORD OnPostPaint(int /*idCtrl*/, LPNMCUSTOMDRAW lpNMCustomDraw )
	{
		LPNMLVCUSTOMDRAW lpNMLVCustomDraw = (LPNMLVCUSTOMDRAW) lpNMCustomDraw;
		//paint the open area to the right
		if ( m_rcPrePaint.right > m_rightItemPaint )
		{
			RECT rcRight = m_rcPrePaint;
			rcRight.left = m_rightItemPaint;
			::FillRect( lpNMLVCustomDraw->nmcd.hdc, &rcRight,
						GetSysColorBrush(COLOR_WINDOW));
		}
		//paint the open area to the bottom
		if ( m_rcPrePaint.bottom > m_bottomItemPaint )
		{
			m_rcPrePaint.top = m_bottomItemPaint;
			::FillRect( lpNMLVCustomDraw->nmcd.hdc, &m_rcPrePaint,
						GetSysColorBrush(COLOR_WINDOW));
		}
		return CDRF_DODEFAULT;
	}
	DWORD OnItemPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW /*lpNMCustomDraw*/)
	{
		return CDRF_NOTIFYSUBITEMDRAW; // We need per-subitem notifications
	}
	DWORD OnSubItemPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW lpNMCustomDraw)
	{
		LPNMLVCUSTOMDRAW lpNMLVCustomDraw = (LPNMLVCUSTOMDRAW) lpNMCustomDraw;
		T* pT = static_cast<T*>(this);
		return pT->DrawItem(lpNMLVCustomDraw->nmcd.hdc, (int) lpNMLVCustomDraw->nmcd.dwItemSpec, lpNMLVCustomDraw->iSubItem, lpNMLVCustomDraw->nmcd.uItemState);
	}
	DWORD DrawItem(WTL::CDCHandle dc, int iItem, int iSubItem, UINT iState)
	{
		T* pT = static_cast<T*>(this);

		RECT rc;
		if( iSubItem == 0 ) 
		{
			pT->GetItemRect(iItem, &rc, LVIR_BOUNDS);
			if( pT->GetColCount() > 1 ) 
			{
				RECT rcSecondColumn;
				pT->GetSubItemRect(iItem, 1, LVIR_BOUNDS, &rcSecondColumn);
				rc.right = rcSecondColumn.left;// - 1;
			}
		}
		else 
			pT->GetSubItemRect(iItem, iSubItem, LVIR_BOUNDS, &rc);

		//track boundaries of what we paint
		if ( rc.right > m_rightItemPaint )
			m_rightItemPaint = rc.right;
		if ( rc.bottom > m_bottomItemPaint )
			m_bottomItemPaint = rc.bottom;

		//this is non standard call, must be implented
		UINT itemState = pT->GetItemState(iItem, iSubItem, LVIS_FOCUSED | LVIS_SELECTED);
		CString text;
		pT->GetItemText(iItem, iSubItem, text);

		if (m_grayCol0 && iSubItem == 0)
            dc.FillSolidRect(&rc, m_clrButtonBack);
		else if (itemState & LVIS_SELECTED)
            dc.FillSolidRect(&rc, m_clrSelBack);
		else
			dc.FillSolidRect(&rc, m_clrBack);

		// Draw item
		dc.SetBkMode(TRANSPARENT);
		if (m_grayCol0 && iSubItem == 0)
		{
			dc.SetTextColor(m_clrButtonText);
			dc.SetBkColor(m_clrButtonBack);
		}
		else if (itemState & LVIS_SELECTED)
		{
			dc.SetTextColor(m_clrSelText);
			dc.SetBkColor(m_clrSelBack);
		}
		else
		{
			dc.SetTextColor(m_clrText);
			dc.SetBkColor(m_clrBack);
		}

		RECT rcText = rc;
		rcText.left += PROP_TEXT_INDENT;
		dc.DrawText(text, -1, &rcText, DT_LEFT | DT_SINGLELINE | DT_EDITCONTROL | DT_NOPREFIX | DT_END_ELLIPSIS | DT_VCENTER);
		//dc.SelectFont(hOldFont);

		// Draw borders
		CPen pen;
		pen.CreatePen(PS_SOLID, 1, m_clrGrid);
		HPEN hOldPen = dc.SelectPen(pen);
		dc.MoveTo(rc.left, rc.bottom-1);
		dc.LineTo(rc.right - 1, rc.bottom - 1);
		dc.LineTo(rc.right - 1, rc.top - 1);
		dc.SelectPen(hOldPen);

		// Paint focus rectangle
		DWORD dwStyle = pT->GetStyle();
		if((::GetFocus() == pT->m_hWnd && (iState & CDIS_FOCUS) != 0) || (dwStyle & LVS_SHOWSELALWAYS) != 0) 
		{
			if (itemState & LVIS_FOCUSED)
			{
               rc.left++; rc.right--; rc.bottom--;
               CBrush brush;
               brush.CreateSolidBrush(m_clrSelBack);
               dc.DrawFocusRect(&rc);
			}
		}

		return CDRF_SKIPDEFAULT;
	}

};
