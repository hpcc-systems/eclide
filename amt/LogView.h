#pragma once

class CLogView : public CWindowImpl<CLogView, WTL::CListBox>
{
public:
	DECLARE_WND_SUPERCLASS(NULL, WTL::CListBox::GetWndClassName())

	BOOL PreTranslateMessage(MSG* pMsg);

	BEGIN_MSG_MAP(CLogView)
		MSG_WM_CREATE(OnCreate);
		MSG_WM_DESTROY(OnDestroy);

		MESSAGE_HANDLER(WM_APP, OnApp)
	END_MSG_MAP()

	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnDestroy();
	LRESULT OnApp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};

void LogMsg(const std::_tstring & msg);
