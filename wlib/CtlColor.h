#pragma once

class CCtlColor
{
public:
	CCtlColor() 
	{
	}
	~CCtlColor() 
	{
	}
 
	BEGIN_MSG_MAP(CCtlColor)
		MESSAGE_HANDLER(WM_CTLCOLORDLG, OnCtlColor)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColor)
	END_MSG_MAP()

	LRESULT OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		//T*   pT = static_cast<T*>(this);
		HDC hDC = (HDC)wParam;
		//HWND hWnd = (HWND)lParam;

		if(uMsg == WM_CTLCOLORDLG)
		{
			bHandled = TRUE;
			return (LRESULT)(HBRUSH)afxGlobalData.brBarFace;
		}
		else if(uMsg == WM_CTLCOLORSTATIC)
		{
			SetBkMode(hDC,TRANSPARENT);
			SetTextColor(hDC, afxGlobalData.clrBarText);
			bHandled = TRUE;
			return (LRESULT)(HBRUSH)afxGlobalData.brBarFace;
		}
		return 0;
	}
};

template <class TItem = CCustomTabItem>
class CColDotNetButtonTabCtrl :
	public CDotNetButtonTabCtrlImpl<CColDotNetButtonTabCtrl<TItem>, TItem>
{
protected:
	typedef CColDotNetButtonTabCtrl<TItem> thisClass;
	typedef CDotNetButtonTabCtrlImpl<CColDotNetButtonTabCtrl<TItem>, TItem> baseClass;

// Constructors:
public:
	CColDotNetButtonTabCtrl()
	{
	}

public:

	DECLARE_WND_CLASS_EX(_T("WTL_MyDotNetButtonTabCtrl"), CS_DBLCLKS, COLOR_WINDOW)

	//We have nothing special to add.
	//BEGIN_MSG_MAP(thisClass)
	//	CHAIN_MSG_MAP(baseClass)
	//END_MSG_MAP()

	void DoPrePaint(RECT rcClient, LPNMCTCCUSTOMDRAW lpNMCustomDraw)
	{
		lpNMCustomDraw->hBrushBackground = afxGlobalData.brBarFace;
		baseClass::DrawBackground(rcClient, lpNMCustomDraw);
	}
};

