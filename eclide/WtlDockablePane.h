#pragma once

#include "WorkunitView.h"

template<typename ViewT>
class CWtlDockablePane : public CDockablePane
{
public:
	ViewT m_view;

public:
	CWtlDockablePane();
	~CWtlDockablePane();
	BOOL PreTranslateMessage(MSG* pMsg);
	void AdjustLayout();

	DECLARE_MESSAGE_MAP()

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);

	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
	{
		//if (m_view->IsWindow())
		//{
		//	switch(nCode)
		//	{
		//	case ID_NEXT_PANE:
		//	case ID_PREV_PANE:
		//		break;
		//	case CN_UPDATE_COMMAND_UI:
		//		{
		//			CCmdUI* cui = static_cast<CCmdUI*>(pExtra);
		//			if (m_view->UIUpdateMenuItems(cui))
		//			{
		//				m_viewHandlesCommand[cui->m_nID] = true;
		//				return TRUE;
		//			}
		//		}
		//		break;
		//	case CN_COMMAND:
		//		if (pExtra == NULL)
		//		{
		//			m_view->SendMessage(WM_COMMAND, nID);
		//			if (m_viewHandlesCommand[nID] == true)
		//				return TRUE;
		//		}
		//	}
		//}

		return CDockablePane::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
	}
};

BEGIN_TEMPLATE_MESSAGE_MAP(CWtlDockablePane, ViewT, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

template<typename ViewT>
CWtlDockablePane<ViewT>::CWtlDockablePane()
{
}

template<typename ViewT>
CWtlDockablePane<ViewT>::~CWtlDockablePane()
{
}

template<typename ViewT>
BOOL CWtlDockablePane<ViewT>::PreTranslateMessage(MSG* pMsg)
{
	if (m_view.PreTranslateMessage(pMsg))
		return TRUE;

	return CDockablePane::PreTranslateMessage(pMsg);
}

template<typename ViewT>
void CWtlDockablePane<ViewT>::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	m_view.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
}

template<typename ViewT>
int CWtlDockablePane<ViewT>::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_view.Create(m_hWnd, ATL::CWindow::rcDefault);

	AdjustLayout();
	return 0;
}

template<typename ViewT>
void CWtlDockablePane<ViewT>::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

template<typename ViewT>
void CWtlDockablePane<ViewT>::OnSetFocus(CWnd* pOldWnd) 
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_view.SetFocus();
}
