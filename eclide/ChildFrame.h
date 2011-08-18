#pragma once

#include "FindReplace.h"
#include "PersistMap.h"
#include "global.h"
#include "UnicodeFile.h"
#include "WorkspaceItem.h"
#include <CustomMessages.h>

enum TIMER
{
	TIMER_FIRST = 200,
	TIMER_AUTOSAVE,
	TIMER_RESTORESTATE,
	TIMER_LAST
};

class CChildFrame : public CWindowImpl<CChildFrame>, public clib::CLockableUnknown, 
					public IPersistable
{
	typedef CChildFrame thisClass;
	typedef CWindowImpl<CChildFrame> baseClass;

protected:
	CComPtr<IWorkspaceItem> m_workspaceItem;

public:
	BEGIN_CLOCKABLEUNKNOWN
		IMPLEMENT_INTERFACE(IPersistable)
	END_CUNKNOWN(clib::CLockableUnknown)

	CChildFrame(IWorkspaceItem * workspaceItem);
	~CChildFrame();

	DECLARE_FRAME_WND_CLASS(NULL, IDR_MDICHILD)

	virtual void OnFinalMessage(HWND /*hWnd*/);

	int GetGroupIndex();
	int GetTabIndex();
	int GetActiveTab();
	void RemovePersistance();

	BEGIN_MSG_MAP(thisClass)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_TIMER(OnTimer)
		MSG_WM_INITMENU(OnInitMenu)

		MESSAGE_HANDLER_EX(CWM_SAVEPERSISTINFO, OnSavePersistInfo)

		MESSAGE_HANDLER(WM_FORWARDMSG, OnForwardMsg)

		//CHAIN_MSG_MAP(baseClass)
	END_MSG_MAP()

	LRESULT OnCreate(LPCREATESTRUCT lParam);
	virtual void OnDestroy();
	void OnTimer(UINT_PTR nIDEvent);

	LRESULT OnSavePersistInfo(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);

	virtual LRESULT OnForwardMsg(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/) = 0;
	virtual void OnInitMenu(HMENU hMenu);

	virtual void UIUpdateTitle() = 0;
	virtual void UIUpdateMenuItems();
	void ShowLastError(const CString & sMessage, DWORD dwError);

	//IPersitable
	HWND GetHwnd() { return m_hWnd; };
	virtual void SavePersistInfo(CPersistMap & persistInfo);
	virtual void RestorePersistInfo(const CPersistMap & persistInfo);
};

class CMainFrame;
CMDIChildWnd * CreateNewChild(CMainFrame* pFrame, CMDIChildWndEx * pChildFrame, UINT nResource, const std::_tstring & title);
