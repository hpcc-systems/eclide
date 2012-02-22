#pragma once

class CHListBox : public CListBox
{
protected:
	int m_width;

public:
	CHListBox() : CListBox()
	{
		m_width = 0;
	}
	CHListBox& operator =(HWND hWnd)
	{
		__super::operator =(hWnd);
		return *this;
	}

	void ResetContent()
	{
		UpdateWidth(_T(" "));
		__super::ResetContent();
	}
	int AddString(LPCTSTR lpszItem)
	{
		int result = __super::AddString(lpszItem);
		if(result < 0)
			return result;
		UpdateWidth(lpszItem);
		return result;
	}
	int DeleteString(UINT nIndex)
	{
		int result = __super::DeleteString(nIndex);
		if(result < 0)
			return result;
		CClientDC dc(m_hWnd);

		dc.SelectFont(GetFont());

		m_width = 0;
		for(int i = 0; i < CListBox::GetCount(); i++)
		{ /* scan strings */
			CString s;
			__super::GetText(i, s);
			CSize sz;
			dc.GetTextExtent(s, _tcslen(s), &sz);
			sz.cx += 3 * ::GetSystemMetrics(SM_CXBORDER);
			if(sz.cx > m_width)
				m_width = sz.cx;
		} /* scan strings */
		SetHorizontalExtent(m_width);
		return result;
	}
	void UpdateWidth(LPCTSTR s)
	{
		CClientDC dc(m_hWnd);

		dc.SelectFont(GetFont());

		CSize sz;
		dc.GetTextExtent(s, _tcslen(s), &sz);
		sz.cx += 3 * ::GetSystemMetrics(SM_CXBORDER);
		if(sz.cx > m_width)
		{ /* extend */
			m_width = sz.cx;
			__super::SetHorizontalExtent(m_width);
		} /* extend */
	}
};
