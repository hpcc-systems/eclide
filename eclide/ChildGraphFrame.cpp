#include "StdAfx.h"
#include "..\en_us\resource.h"

#include "ChildGraphFrame.h"
#include "WtlMDIChildFrame.h"
#include "ChildFrame.h"
#include "MainFrm.h"
#include "preferencedlg.h"
#include "global.h"
#include "util.h"
#include "GraphView.h"

class CGraphFrame : 
	public CChildFrame
{
	typedef CGraphFrame thisClass;
	typedef CChildFrame baseClass;

public:
	CGraphView m_dlgview;
	std::_tstring m_path;

public: 
	DECLARE_FRAME_WND_CLASS(NULL, IDR_GRAPHWINDOW)
	CGraphFrame(const CString & filePath, IWorkspaceItem * workspaceItem) : baseClass(workspaceItem), m_dlgview(NULL, NULL)
	{
		m_path = filePath;
		m_dlgview.DoFileOpen(filePath);
	}

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		if ((pMsg->message == WM_KEYDOWN )&& (pMsg->wParam == VK_F4))
		{
			int d = 0;
		}
		if (pMsg->message == WM_KEYDOWN && (::GetAsyncKeyState(VK_CONTROL) & 0x8000))  // Let frame handle some messages.
		{
			switch (pMsg->wParam)
			{
			case VK_F4:
				return 0;
				break;
			case VK_F6:
				if (::GetAsyncKeyState(VK_SHIFT) & 0x8000)
					::GetIMainFrame()->MDIPrev();
				else
					::GetIMainFrame()->MDINext();
				return 1;
				break;
			case VK_TAB:
				return 0;
				break;
			}
		}
		return m_dlgview.PreTranslateMessage(pMsg);
	}

	void OnSize(UINT nType, CSize size)
	{
		CRect rectClient;
		GetClientRect(rectClient);

		m_dlgview.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
	}

	void OnSetFocus(HWND /*hWndOther*/)
	{
		if (m_dlgview.IsWindow() && m_dlgview.IsWindowVisible())
			m_dlgview.SetFocus();
	}

	void PostSelectRibbon()
	{
		GetIMainFrame()->PostSelectRibbon(RIBBON_GRAPH);
	}

	BEGIN_MSG_MAP(thisClass)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_SIZE(OnSize)
		MSG_WM_SETFOCUS(OnSetFocus)
		MSG_WM_CONTEXTMENU(OnContextMenu)

		CHAIN_MSG_MAP(baseClass)
		REFLECT_NOTIFICATIONS()
		CHAIN_COMMANDS_MEMBER(m_dlgview)
	END_MSG_MAP()

	LRESULT OnCreate(LPCREATESTRUCT lParam)
	{
		m_dlgview.Create(m_hWnd, rcDefault);

		baseClass::OnCreate(lParam);

		return 0;
	}

	virtual void UIUpdateTitle()
	{
		CString title = _T("XGMML File");
		//m_dlgview.GetTitle(title);
		GetParent().SendMessage(UM_TITLECHANGED, (WPARAM)(const TCHAR *)title);
	}

	virtual bool UIUpdateMenuItems(CCmdUI * cui)
	{
		if (m_dlgview.UpdateUI(cui))
			return true;
		return false;
	}
	const TCHAR * GetPath()
	{
		return m_path.c_str();
	}

	//  File Access  ---
	bool DoFileOpen(const CString & sPathName);	

	virtual void SavePersistInfo(CPersistMap & persistInfo);
	virtual void RestorePersistInfo(const CPersistMap & persistInfo);

	LRESULT OnForwardMsg(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		LPMSG pMsg = (LPMSG)lParam;

		//if(baseClass::PreTranslateMessage(pMsg))
		//	return TRUE;

		return m_dlgview.PreTranslateMessage(pMsg);
	}


	//  IResultSlot override  ---
	void PostStatus(const TCHAR *status, PANE pane = PANE_DEFAULT)
	{
		::GetIMainFrame()->PostStatus(status, pane);
	}

public:
	void OnContextMenu(HWND phWnd, CPoint pt)
	{
		ATLASSERT(!"Check This");
		m_dlgview.OnContextMenu(phWnd,pt);
	}
};

bool CGraphFrame::DoFileOpen(const CString & sPathNameX) 
{
	m_path = sPathNameX;
	return m_dlgview.DoFileOpen(sPathNameX);
}

void CGraphFrame::SavePersistInfo(CPersistMap & persistInfo)
{
	baseClass::SavePersistInfo(persistInfo);
	persistInfo.Set(PERSIST_TYPE, PERSISTVAL_GRAPH);
	persistInfo.Set(PERSIST_PATH, m_path);
}

void CGraphFrame::RestorePersistInfo(const CPersistMap & persistInfo)
{
	if (std::_tstring(persistInfo.Get(PERSIST_PATH)).length())
		m_dlgview.DoFileOpen(persistInfo.Get(PERSIST_PATH));
}

//  ===========================================================================
class CChildGraphFrm : public CWtlMDIChildFrame<StlLinked<CGraphFrame> >

{
	typedef CChildGraphFrm thisClass;
	typedef CWtlMDIChildFrame<StlLinked<CGraphFrame> > baseClass;

public:
	CChildGraphFrm(const CString & filePath, IWorkspaceItem * workspaceItem)
	{
		m_view = new CGraphFrame(filePath, workspaceItem);
	}

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CChildGraphFrm, baseClass)
END_MESSAGE_MAP()

// CChildGraphFrm diagnostics
#ifdef _DEBUG
void CChildGraphFrm::AssertValid() const
{
	CMDIChildWndEx::AssertValid();
}

void CChildGraphFrm::Dump(CDumpContext& dc) const
{
	CMDIChildWndEx::Dump(dc);
}
#endif //_DEBUG
//  ===========================================================================
HWND OpenGraphMDI(CMainFrame* pFrame, IWorkspaceItem * workspaceItem)
{
	CChildGraphFrm* pChild = new CChildGraphFrm(_T(""), workspaceItem);
	CMDIChildWnd * retVal = CreateNewChild(pFrame, pChild, IDR_GRAPHWINDOW, _T("todo"));
	return retVal->GetSafeHwnd();
}

bool OpenFileGraphMDI(CMainFrame* pFrame, const CString & filePath, IWorkspaceItem * workspaceItem)
{
	ATLASSERT(!filePath.IsEmpty());
	CChildGraphFrm* pChild = new CChildGraphFrm(filePath, workspaceItem);
	CMDIChildWnd * retVal = CreateNewChild(pFrame, pChild, IDR_GRAPHWINDOW, _T("todo"));
	return true;
}
