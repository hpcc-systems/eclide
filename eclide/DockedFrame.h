#pragma once

class CMainFrame;

template <typename T>
class CDockedFrameT : public CTabbedAutoHideDockingWindow
{
	typedef CTabbedAutoHideDockingWindow baseClass;
	typedef CDockedFrameT thisClass;

public:
	CDockedFrameT()
	{
	}

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		T* pT = static_cast<T*>(this);
		return pT->m_view.PreTranslateMessage(pMsg);
	}

	BEGIN_MSG_MAP(thisClass)
		MSG_WM_CREATE(OnCreate)
		CHAIN_MSG_MAP(baseClass)
	END_MSG_MAP()

	LRESULT OnCreate(LPCREATESTRUCT /*lParam*/)
	{
		T* pT = static_cast<T*>(this);
		pT->m_view.Create(m_hWnd);
		SetReflectNotifications(false);
		SetClient(pT->m_view);
		SetMsgHandled(false);
		return 0;
	}
};



