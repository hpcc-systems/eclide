#include "StdAfx.h"
#include "resource.h"

#include "ChildResultFrame.h"
#include "ChildFrame.h"
#include "resultview.h"
#include "menumanager.h"
#include <util.h>

std::map<CString, CChildFrame *> g_result_window;

class CChildResultFrame : 
	public CChildFrame,
	public IResultSlot
{
	typedef CChildResultFrame thisClass;
	typedef CChildFrame baseClass;

protected:
	CMenuManager * m_MenuManager;
	CMultiResultView m_view;
	CComPtr<IWorkunit> m_wu;
	CSearchableInfo m_searchInfo;
	CSearchableResult m_searchResult;

public:
	CChildResultFrame(CMenuManager *pMenuManager, IWorkunit *wu) : m_view(this), baseClass(pMenuManager)
	{
		m_wu = wu;
		m_MenuManager = pMenuManager;
	}
	~CChildResultFrame()
	{
	}
	LRESULT OnCreate(LPCREATESTRUCT lParam)
	{
		m_view.SetWorkunit(m_wu);
		m_hWndClient = m_view.Create(m_hWnd, rcDefault);
		return baseClass::OnCreate(lParam);
	}
	void OnDestroy()
	{
		g_result_window[m_wu->GetID()] = NULL;
		m_view.DestroyWindow();
		baseClass::OnDestroy();
	}

	BEGIN_MSG_MAP(thisClass)
		MSG_WM_CREATE(OnCreate)
		MESSAGE_HANDLER(WM_FORWARDMSG, OnForwardMsg)
		MESSAGE_HANDLER(FindReplace::GetFindReplaceMsg(), OnFindReplaceCmd)
		COMMAND_ID_HANDLER_EX(ID_EDIT_FIND, OnEditFind)
		COMMAND_ID_HANDLER_EX(ID_EDIT_FINDNEXT, OnEditFindNext)
		COMMAND_ID_HANDLER_EX(ID_EDIT_FINDPREV, OnEditFindPrev)

		CHAIN_MSG_MAP(baseClass)
	END_MSG_MAP()

	LRESULT OnForwardMsg(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		LPMSG pMsg = (LPMSG)lParam;

		if(CMDIChildWindowImpl<CChildFrame>::PreTranslateMessage(pMsg))
			return TRUE;

		return m_view.PreTranslateMessage(pMsg);
	}

	bool OnDoubleClickResult()
	{
		return false;
	}

	bool OnContextResult(CWindow &context, CMenu &m, CPoint &pt)
	{
		UIUpdateMenuItems();
		m_MenuManager->CloneState(m);

		int id = m.GetSubMenu(0).TrackPopupMenuEx(TPM_RETURNCMD, pt.x, pt.y, m_view, NULL);
		m.DestroyMenu();
		if (id)
			PostMessage(WM_COMMAND, id);
		return true;
	}

	bool Drilldown(const CString &ecl)
	{
		MessageBox(_T("Deprecated Window"));
		return false;
	}

	void ResetSearch()
	{
		m_searchResult.SetResult(0,false);
		m_searchInfo.Init( m_MenuManager->m_fr.m_szFindWhat, m_MenuManager->m_fr.m_dwFlags );
	}

	void DoEditFind()
	{
		m_searchInfo.SetPosition(-1);
		m_MenuManager->OnEditFind(_T(""));
	}

	void OnEditFind(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
	{
		DoEditFind();
	}

	LRESULT OnFindReplaceCmd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if(m_MenuManager->m_fr.FindNext())
		{
			//search up would be previous
			DoEditFindAgain(m_MenuManager->m_fr.SearchDown() == TRUE);
		}
		return 0;
	}

	void DoEditFindAgain(bool bNext)
	{
		if(m_MenuManager->m_fr.m_szFindWhat[0] == 0)
		{
			DoEditFind();
			return;
		}

		ResetSearch();

		if ( !m_view.Search(m_searchInfo,m_searchResult))
			::MessageBeep(MB_ICONEXCLAMATION);
		else
		{	
			m_searchInfo.SetPosition(m_searchResult.m_row);
			m_view.GotoLine(m_searchResult.m_row, m_searchResult.m_col);
		}
	}

	void OnEditFindNext(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
	{
		DoEditFindAgain(true);
	}

	void OnEditFindPrev(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
	{
		DoEditFindAgain(false);
	}

	void UIUpdateMenuItems()
	{
		if (m_MenuManager->GetActive() == m_hWnd) 
		{
			bool didFind = m_searchResult.m_found; 
			m_MenuManager->m_UpdateUI->UIEnable(ID_EDIT_FINDPREV, didFind);
			m_MenuManager->m_UpdateUI->UIEnable(ID_EDIT_FINDNEXT, didFind);

			m_MenuManager->m_UpdateUI->UIEnable(ID_FILE_CLOSE, TRUE);
			m_view.UpdateUI(m_MenuManager->m_UpdateUI);
		}
	}

	virtual void UIUpdateTitle()
	{
		SetWindowText(GetTitle());
	}

	CString GetTitle()
	{
		CString title = m_view.IsDirty() ? "*" : "";
		title += m_view.GetTitle();
		return title;
	}

	virtual void SavePersistInfo(CPersistMap & persistInfo)
	{
		persistInfo.m_map[PERSIST_TYPE] = PERSISTVAL_RESULT;
		persistInfo.m_map[PERSIST_WUID] = m_wu->GetID();
		baseClass::SavePersistInfo(persistInfo);
	}

};

void OpenResultMDI(CMenuManager *pMenuManager, const CString & wuid, HWND hWndClient, const CString & autoSavePath)
{
	CChildFrame * win = g_result_window[wuid];
	if (win && win->IsWindow())
	{
		if (win->IsIconic())
			win->ShowWindow(SW_RESTORE);
		win->BringWindowToTop();
		return;
	}

	StlOwned<IDali> dali = AttachDali(GetIConfig()->Get(GLOBAL_SERVER_WORKUNIT), _T("Dali"));
	CComPtr<IWorkunit> wu = dali->GetWorkunit(wuid);
	if (wu)
	{
		CChildResultFrame * pChild = new CChildResultFrame(pMenuManager, wu);
		pChild->SetAutoSavePath(autoSavePath);
		pChild->CreateEx(hWndClient, NULL, NULL, GetIConfig()->Get(GLOBAL_OPENMDICHILDMAX) ? WS_MAXIMIZE : 0);
		g_result_window[wuid] = pChild;
	}
}


