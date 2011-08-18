#pragma once

typedef CWinTraits<WS_POPUP | WS_BORDER, 0> CTimelineDateTimeCtrlTraits;
class CTimelineDateTimeCtrl :
	public CWindowImpl<CTimelineDateTimeCtrl, CMonthCalendarCtrl, CTimelineDateTimeCtrlTraits>
{
public:
	virtual ~CTimelineDateTimeCtrl() {};

	BEGIN_MSG_MAP_EX(CTimelineDateTimeCtrl)
		MESSAGE_HANDLER(WM_ACTIVATE, OnActivate)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
	END_MSG_MAP()


	LRESULT OnActivate(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		if(WA_INACTIVE==LOWORD(wParam))
			PostMessage(WM_CLOSE);
		return 0;
	}

	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		DestroyWindow();
		return 0;
	}

	void OnFinalMessage(HWND /*hWnd*/)
	{
		delete this;
	}
};
