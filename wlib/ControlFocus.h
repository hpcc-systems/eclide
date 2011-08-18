#pragma once

 class CWindowControlFocusEx : public CWindowImpl<CWindowControlFocusEx>
{
	typedef CWindowImpl<CWindowControlFocusEx> baseClass;

public:
	CWindowControlFocusEx()
	{
		m_bFocus = false;
	}

	baseClass& operator=(HWND hWnd)
	{
		baseClass::SubclassWindow(hWnd);
		return *this;
	}

	BEGIN_MSG_MAP(CWindowControlFocusEx)
		if (uMsg == WM_SETFOCUS)
		{	
			m_bFocus = true; 
			lResult = 0;
		}
		else if (uMsg == WM_KILLFOCUS)
		{	
			m_bFocus = false; 
			lResult = 0;
		}
	END_MSG_MAP()

	inline bool HasFocus() { return m_bFocus; }

protected:
	bool m_bFocus;
};
