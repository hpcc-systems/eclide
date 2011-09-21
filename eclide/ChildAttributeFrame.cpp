#include "StdAfx.h"
#include "..\en_us\resource.h"

#include "ChildAttributeFrame.h"
#include "WtlMDIChildFrame.h"
#include "ChildEclFrame.h"
#include "MainFrm.h"
#include "Repository.h"
#include "attribute.h"
#include "HistoryView.h"
#include "attributehistory.h"
#include "TabbedChildWindowEx.h"
#include <preferencedlg.h> //wlib
#include "EclDlgAttribute.h"

class CAttributeFrame;
class CChildAttributeFrm;
typedef std::pair<std::_tstring, std::_tstring> WinID;
typedef std::pair<CChildAttributeFrm *, CAttributeFrame *> FramePair;
std::map<WinID, FramePair> g_attr_window;
std::_tstring blankString;

class CAttributeFrame : public CChildFrame, public boost::signals::trackable,
	public IAttributeHistory, 
	public ISourceSlot,
	public IHistorySlot
{
	typedef CAttributeFrame thisClass;
	typedef CChildFrame baseClass;

public:
	CTabbedChildWindowEx< CColDotNetButtonTabCtrl<CTabViewTabItem> > m_tabbedChildWindow;
	CString m_NullStr;
	mutable CString m_Ecl;
	CString m_CurrentStr;
	mutable CAttributeDlg m_dlgview;
	CHistoryView m_historyView;
	bool m_defaultToHistory;
	boost::signals::connection m_attrConnection;

public:
	BEGIN_CLOCKABLEUNKNOWN
	END_CUNKNOWN(CChildFrame)

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
		return m_tabbedChildWindow.PreTranslateMessage(pMsg);
	}

	void UpdateAttribute(IAttribute * attr)
	{
		WinID fromID(m_dlgview.GetAttribute()->GetModuleQualifiedLabel(), m_dlgview.GetAttribute()->GetLabel());
		WinID toID(attr->GetModuleQualifiedLabel(), attr->GetLabel());
		m_dlgview.SetAttribute(attr);
		g_attr_window[toID] = g_attr_window[fromID];
		//g_attr_window[toID] = this;
		g_attr_window[fromID] = std::make_pair<CChildAttributeFrm *, CAttributeFrame *>(NULL, NULL);
		UIUpdateTitle();
		(*this)(attr, false, NULL, false);
	}

	void DefaultToHistory(bool defaultToHistory)
	{
		m_defaultToHistory = defaultToHistory;
	}

	void GetText(CString & txt)
	{
		m_dlgview.GetSource(txt);
	}

	void SetText(const CString & ecl, bool resetSavePoint, bool clearUndo)
	{
		m_dlgview.SetSource(ecl, resetSavePoint, clearUndo);
	}

	void SetText(const CString & ecl)
	{
		m_dlgview.SetSource(ecl);
	}

	void SetSyntax(const CSyntaxErrorVector & errors)
	{
		m_dlgview.SetSyntax(errors);
		GetIMainFrame()->Send_RefreshSyntax(&m_dlgview.GetView(), m_dlgview.GetView().SyntaxGetCurrent());
		PostMessage(WM_COMMAND, ID_ECL_NEXTERROR);
	}

	void SetSyntax(Dali::CEclExceptionVector * errors)
	{
		BOOL bHandled = false;
		m_dlgview.OnSubmitDone(0, (WPARAM)Dali::WUActionCheck, (LPARAM)errors, bHandled);
		GetIMainFrame()->Send_RefreshSyntax(&m_dlgview.GetView(), m_dlgview.GetView().SyntaxGetCurrent());
	}

	void ShowHistory()
	{
		m_tabbedChildWindow.GetTabCtrl().SetCurSel(1);
	}

	void ShowEcl(const std::_tstring & searchTerm, FINDMODE findmode)
	{
		m_tabbedChildWindow.GetTabCtrl().SetCurSel(0);
		if (searchTerm.length())
		{
			int flags = 0;
			m_dlgview.GetView().DoFind(searchTerm, flags, findmode, true);
		}
	}

	bool ECLViewActive()
	{
		return 0 == m_tabbedChildWindow.GetTabCtrl().GetCurSel();
	}

	void PostSelectRibbon()
	{
		unsigned n = m_tabbedChildWindow.GetTabCtrl().GetCurSel();
		switch(n)
		{
		case 0:
			GetIMainFrame()->PostSelectRibbon(RIBBON_HOME);
			break;
		case 1:
			GetIMainFrame()->PostSelectRibbon(RIBBON_COMPARE);
			break;
		default:
			GetIMainFrame()->PostSelectRibbon(RIBBON_HOME);
			break;
		}
	}

	const TCHAR * GetPath()
	{
		return blankString.c_str();
	}

	BEGIN_MSG_MAP(thisClass)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_SIZE(OnSize)
		MSG_WM_SETFOCUS(OnSetFocus)
		NOTIFY_CODE_HANDLER(CTCN_SELCHANGE, OnTabSelected)

		CHAIN_COMMANDS_MEMBER(m_tabbedChildWindow)
		CHAIN_MSG_MAP(baseClass)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	#define	TABID_ATTRIBUTE 0
	#define	TABID_HISTORY 1

	void OnSize(UINT nType, CSize size)
	{
		CRect rectClient;
		GetClientRect(rectClient);

		m_tabbedChildWindow.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
	}

	void OnSetFocus(HWND /*hWndOther*/)
	{
		//SetMsgHandled(false);
		CWindow activeWnd = m_tabbedChildWindow.GetActiveView();
		if (activeWnd.IsWindow() && activeWnd.IsWindowVisible())
			activeWnd.SetFocus();

		int syncRepository = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SYNCREPOSITORY);
		if (syncRepository)
			GetIMainFrame()->SyncTOC(m_dlgview.GetAttribute());
	}

	LRESULT OnTabSelected(int /*idCtrl*/, LPNMHDR pnmh, BOOL& bHandled)
	{
		bHandled = true;
		NMCTC2ITEMS *pItems = (NMCTC2ITEMS *)pnmh;
		if ( pItems->iItem1 != pItems->iItem2)
		{
			if ( pItems->iItem2 == TABID_HISTORY )
			{
				RefreshHistory();
				m_historyView.SetCompareMode(true);
			}
		}
		if (IsWindowVisible())
			PostSelectRibbon();
		return 0;
	}

	void RefreshHistory()
	{
		GetText();
		m_historyView.SetAttribute(m_dlgview.GetAttribute(), m_Ecl);
		if ( m_historyView.IsWindow() )
			m_historyView.DoRefresh();
	}


	DECLARE_FRAME_WND_CLASS(NULL, IDR_ATTRIBUTEWINDOW);

	CAttributeFrame(IAttribute * attribute, IWorkspaceItem * workspaceItem);
	virtual ~CAttributeFrame(void);

	virtual LRESULT OnCreate(LPCREATESTRUCT lParam);

	//IEclSlot
	virtual void DoCheckSyntax();
	virtual void UIUpdateTitle();
	virtual bool UIUpdateMenuItems(CCmdUI * cui);
	virtual void LinesAdded(int lines);
	void DoCheckAttributeSyntax(const CString &attr, IAttributeType * type, const CSyntaxErrorVector & errors);
	void RefreshSyntax(ISciSyntaxMarker *errors, size_t curErr, bool forceShow);
	void PostStatus(const TCHAR *status, PANE pane = PANE_DEFAULT);
	void NewSel();
	void SetReadOnly(bool readOnly);
	void operator()(IAttribute * attribute, bool eclChanged, IAttribute * newAttrAsOldOneMoved, bool deleted);

	virtual void SavePersistInfo(CPersistMap & persistInfo);
	virtual void RestorePersistInfo(const CPersistMap & persistInfo);

	LRESULT OnForwardMsg(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		LPMSG pMsg = (LPMSG)lParam;

		//if(__super::PreTranslateMessage(pMsg))
		//	return TRUE;

		return m_tabbedChildWindow.PreTranslateMessage(pMsg);
	}

	//IAttributeHistory ---
	const TCHAR *GetID() const
	{
		return m_NullStr;
	}
	const TCHAR *GetModuleQualifiedLabel() const
	{
		return m_NullStr;
	}
	const TCHAR *GetLabel() const
	{
		return m_NullStr;
	}
	const TCHAR *GetQualifiedLabel(bool excludeRoot = false) const
	{
		return m_NullStr;
	}
	IAttributeType * GetType() const
	{
		return NULL;
	}
	const TCHAR *GetText(bool refresh = true, bool noBroadcast = false) const
	{
		m_dlgview.GetSource(m_Ecl);
		return m_Ecl;
	}
	const TCHAR *GetModifiedDate() const
	{
		return m_CurrentStr;
	}
	const TCHAR *GetModifiedBy() const
	{
		return m_NullStr;
	}
	const TCHAR *GetDescription() const
	{
		return m_NullStr;
	}
	int GetVersion() const
	{
		return 0;
	}
	IAttribute * GetAttribute() const
	{
		return NULL;
	}
	bool SetLabel(const std::_tstring & label)
	{
		return false;
	}
	//  ---  IHistorySlot  ---
	bool OnHistoryContext(WTL::CMenu &/*menu*/, CPoint &/*pt*/)
	{
		return false;
	}
	void SetAttributeText(IAttribute *attr, const CString &ecl, bool resetSavePoint, bool clearUndo)
	{
		ATLASSERT(m_dlgview.GetAttribute() == attr);
		SetText(ecl, resetSavePoint, clearUndo);
	}
	void OnRefresh()
	{
	}
};

CAttributeFrame::CAttributeFrame(IAttribute * attribute, IWorkspaceItem * workspaceItem) : baseClass(workspaceItem), m_dlgview(attribute, this), m_historyView(this)
{
	m_CurrentStr = _T("Current");
	m_defaultToHistory = false;
}

CAttributeFrame::~CAttributeFrame()
{
	m_attrConnection.disconnect();
}

LRESULT CAttributeFrame::OnCreate(LPCREATESTRUCT lParam)
{
	m_tabbedChildWindow.SetReflectNotifications(true);
	m_tabbedChildWindow.SetTabStyles(CTCS_BOTTOM | CTCS_TOOLTIPS);
	m_tabbedChildWindow.Create(m_hWnd, rcDefault);
	m_dlgview.Create(m_tabbedChildWindow, rcDefault);
	operator()(m_dlgview.GetAttribute(), false, NULL, false);
	m_tabbedChildWindow.AddTab(m_dlgview, _T("Source"));

	m_historyView.Create(m_tabbedChildWindow, rcDefault);
	int nTab = m_tabbedChildWindow.AddTab(m_historyView, _T("History"));
	if ( m_defaultToHistory )
		m_tabbedChildWindow.GetTabCtrl().SetCurSel(nTab);

	m_Ecl = m_dlgview.GetAttribute()->GetText(false);  //Text has already been fetched...
	m_dlgview.SetSource(m_Ecl, true, true);
	LRESULT retVal = baseClass::OnCreate(lParam);

	m_attrConnection = m_dlgview.GetAttribute()->on_refresh_connect(boost::ref(*this));

	m_dlgview.SetFocus();

	return retVal;
}

void CAttributeFrame::DoCheckSyntax()
{
	/*
	OnEclClearErrors(0, 0, 0);
	CString ecl;
	m_view.GetText(ecl);
	clib::thread run(boost::bind(&EclCheckSyntax, this, ecl));
	*/
}

void CAttributeFrame::UIUpdateTitle()
{
	CString title;
	m_dlgview.GetTitle(title);
	GetParent().SendMessage(UM_TITLECHANGED, (WPARAM)(const TCHAR *)title);
}

bool CAttributeFrame::UIUpdateMenuItems(CCmdUI * cui)
{
	GetIMainFrame()->DisableAllMenuItems();
	if (ECLViewActive())
	{
		if (m_dlgview.UIUpdateMenuItems(cui))
			return true;
		UPDATEUI(cui, ID_ECL_SYNCTOC, true);
		UPDATEUI(cui, ID_REPOSITORY_COPY, true);
	}
	else
	{
		if (m_historyView.UIUpdateMenuItems(cui))
			return true;
	}
	return false;
}

void CAttributeFrame::LinesAdded(int lines)
{
	m_dlgview.SyntaxClearAll();
}

void CAttributeFrame::DoCheckAttributeSyntax(const CString &attr, IAttributeType * type, const CSyntaxErrorVector & errors)
{
	if (attr.CompareNoCase(m_dlgview.GetAttribute()->GetQualifiedLabel()) != 0)
		GetIMainFrame()->OpenSyntaxAttribute(attr, type, errors);
}

void CAttributeFrame::RefreshSyntax(ISciSyntaxMarker *errors, size_t curErr, bool forceShow)
{
	GetIMainFrame()->Send_RefreshSyntax(errors, curErr, forceShow);
}

void CAttributeFrame::PostStatus(const TCHAR *status, PANE pane)
{
	::GetIMainFrame()->PostStatus(status, pane);
}

void CAttributeFrame::NewSel()
{
	::GetIMainFrame()->m_wndRibbonBar.HideKeyTips();
}

void CAttributeFrame::SetReadOnly(bool readOnly)
{
	m_dlgview.SetReadOnly(readOnly);
}

void CAttributeFrame::operator()(IAttribute *attr, bool eclChanged, IAttribute * newAttrAsOldOneMoved, bool deleted)
{
	WTL::CIcon icon;
	switch(attr->GetState())
	{
	case ATTRSTATE_CHECKEDOUT_SANDBOXED:
		icon = LoadIcon(_Module.m_hInstResource, MAKEINTRESOURCE(IDI_CHECKEDOUT_SANDBOXED));
		break;
	case ATTRSTATE_CHECKEDOUT:
		icon = LoadIcon(_Module.m_hInstResource, MAKEINTRESOURCE(IDI_CHECKEDOUT));
		break;
	case ATTRSTATE_LOCKED:
		icon = LoadIcon(_Module.m_hInstResource, MAKEINTRESOURCE(IDI_LOCKED));
		break;
	case ATTRSTATE_SANDBOXED:
		icon = LoadIcon(_Module.m_hInstResource, MAKEINTRESOURCE(IDI_SANDBOXED));
		break;
	case ATTRSTATE_ORPHANED:
		icon = LoadIcon(_Module.m_hInstResource, MAKEINTRESOURCE(IDI_ORPHANED));
		break;
	default:
		icon = LoadIcon(_Module.m_hInstResource, MAKEINTRESOURCE(IDR_MDICHILD));
		break;
	}
	GetParent().SendMessage(UM_ICONCHANGED, (WPARAM)(HICON)icon);
	if (newAttrAsOldOneMoved)
	{
		UpdateAttribute(newAttrAsOldOneMoved);
	}
	//TODO handle renamed and deleted.
	ATLASSERT(!deleted);
}

void CAttributeFrame::SavePersistInfo(CPersistMap & persistInfo)
{
	baseClass::SavePersistInfo(persistInfo);
	persistInfo.Set(PERSIST_TYPE, PERSISTVAL_ATTRIBUTE);
	persistInfo.Set(PERSIST_MODULE, m_dlgview.GetAttribute()->GetModuleQualifiedLabel());
	persistInfo.Set(PERSIST_ATTRIBUTE, m_dlgview.GetAttribute()->GetLabel());
	persistInfo.Set(PERSIST_ATTRIBUTETYPE, m_dlgview.GetAttribute()->GetType()->GetRepositoryCode());

	m_dlgview.SavePersistInfo(persistInfo);
}

void CAttributeFrame::RestorePersistInfo(const CPersistMap & persistInfo)
{
	//  Module/Attribute get restore on construction...

	try
	{
		if (boost::lexical_cast<int, const TCHAR *>(persistInfo.Get(PERSIST_DIRTY)) == 1)
			m_dlgview.RestorePersistInfo(persistInfo);
	}
	catch (boost::bad_lexical_cast &)
	{
	}
}

//  ===========================================================================
class CChildAttributeFrm : public CWtlMDIChildFrame<StlLinked<CAttributeFrame> >
{
	typedef CChildAttributeFrm thisClass;
	typedef CWtlMDIChildFrame<StlLinked<CAttributeFrame> > baseClass;

public:
	CChildAttributeFrm(IAttribute * attr, IWorkspaceItem * workspaceItem)
	{
		m_view = new CAttributeFrame(attr, workspaceItem);
		WinID id(m_view->m_dlgview.GetAttribute()->GetModuleQualifiedLabel(), m_view->m_dlgview.GetAttribute()->GetLabel());
		g_attr_window[id].first = this;
		g_attr_window[id].second = m_view;
	}
	virtual ~CChildAttributeFrm()
	{
		WinID id(m_view->m_dlgview.GetAttribute()->GetModuleQualifiedLabel(), m_view->m_dlgview.GetAttribute()->GetLabel());
		g_attr_window[id] = std::make_pair<CChildAttributeFrm *, CAttributeFrame *>(NULL, NULL);
	}

#ifdef _DEBUG
	virtual void AssertValid() const
	{
		baseClass::AssertValid();
	}
	virtual void Dump(CDumpContext& dc) const
	{
		baseClass::Dump(dc);
	}
#endif

	// Generated message map functions
protected:

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CChildAttributeFrm, baseClass)
END_MESSAGE_MAP()

// CChildAttributeFrm message handlers
//  ===========================================================================
HWND OpenAttributeMDI(CMainFrame* pFrame, IWorkspaceItem * workspaceItem)
{
	return OpenAttributeMDI(pFrame, workspaceItem->GetAttribute(), workspaceItem);
}

HWND OpenAttributeMDI(CMainFrame* pFrame, IAttribute * attribute, const std::_tstring & searchTerm, FINDMODE findmode, IWorkspaceItem * workspaceItem)
{
	CComPtr<IRepository> rep = ::AttachRepository();
	return OpenAttributeMDI(pFrame, attribute, workspaceItem, false, CSyntaxErrorVector(), NULL, searchTerm, findmode);
}

HWND OpenAttributeMDI(CMainFrame* pFrame, const std::_tstring & module, const std::_tstring & attr, IAttributeType * type, IWorkspaceItem * workspaceItem, bool bHistoryView, const CSyntaxErrorVector & errors)
{
	CComPtr<IRepository> rep = ::AttachRepository();
	CComPtr<IAttribute> attribute = rep->GetAttribute(module.c_str(), attr.c_str(), type, 0, true, true);
	if (!attribute)
		return 0;

	return OpenAttributeMDI(pFrame, attribute, workspaceItem, bHistoryView, errors);
}

HWND OpenAttributeMDI(CMainFrame* pFrame, IAttribute * attribute, IWorkspaceItem * workspaceItem, bool bHistoryView, Dali::CEclExceptionVector * errors)
{
	return OpenAttributeMDI(pFrame, attribute, workspaceItem, bHistoryView, CSyntaxErrorVector(), errors);
}

HWND OpenAttributeMDI(CMainFrame* pFrame, const std::_tstring & module, const std::_tstring & attr, IAttributeType * type, unsigned int row, unsigned int col, IWorkspaceItem * workspaceItem)
{
	CComPtr<IRepository> rep = ::AttachRepository();
	CComPtr<IAttribute> attribute = rep->GetAttribute(module.c_str(), attr.c_str(), type, 0, true, true);
	if (!attribute)
		return 0;

	HWND retVal = OpenAttributeMDI(pFrame, attribute, workspaceItem, false, CSyntaxErrorVector(), NULL, _T(""), FINDMODE_NONE, row, col);

	return retVal;
}

HWND OpenAttributeMDI(CMainFrame* pFrame, IAttribute * attribute, IWorkspaceItem * workspaceItem, bool bHistoryView, const CSyntaxErrorVector & errors, Dali::CEclExceptionVector * errors2, const std::_tstring & searchTerm, FINDMODE findmode, unsigned int row, unsigned int col)
{
	bool readOnly = attribute->GetAccess() < SecAccess_Write;
	WinID id(attribute->GetModuleQualifiedLabel(), attribute->GetLabel());

	FramePair win = g_attr_window[id];
	if (win.second && win.second->IsWindow())
	{
		if (win.first->IsIconic())
			win.first->ShowWindow(SW_RESTORE);
		win.first->BringWindowToTop();
		if (errors.size())
			win.second->SetSyntax(errors);
		if (bHistoryView)
			win.second->ShowHistory();
		else
			win.second->ShowEcl(searchTerm, findmode);
		if (row || col)
		{
			unsigned int pos = win.second->m_dlgview.m_view.PositionFromLine(row - 1) + col - 1;
			win.second->m_dlgview.m_view.SetSel(pos, pos);
		}

		return win.first->GetSafeHwnd();
	}

	CChildAttributeFrm* pChildFrame = new CChildAttributeFrm(attribute, workspaceItem);
	if (bHistoryView)
		pChildFrame->m_view->DefaultToHistory(true);
	CreateNewChild(pFrame, pChildFrame, IDR_ATTRIBUTEWINDOW, attribute->GetQualifiedLabel());
	if (readOnly)
		pChildFrame->m_view->SetReadOnly(true);
	if (errors.size())
		pChildFrame->m_view->SetSyntax(errors);
	if (errors2)
		pChildFrame->m_view->SetSyntax(errors2);
	if (searchTerm.length())
		pChildFrame->m_view->ShowEcl(searchTerm, findmode);
	if (row || col)
	{
		unsigned int pos = pChildFrame->m_view->m_dlgview.m_view.PositionFromLine(row - 1) + col - 1;
		pChildFrame->m_view->m_dlgview.m_view.SetSel(pos, pos);
		::SetFocus(pChildFrame->m_view->m_dlgview.m_view);
	}

	return pChildFrame->GetSafeHwnd();
}

void SaveAttributeMDI(IAttribute * attribute)
{
	WinID id(attribute->GetModuleQualifiedLabel(), attribute->GetLabel());

	CAttributeFrame * win = g_attr_window[id].second;
	if (win && win->IsWindow())
		win->SendMessage(CWM_GLOBALSAVE);
}

bool GetAttributeText(IAttribute * attribute, CString & result)
{
	WinID id(attribute->GetModuleQualifiedLabel(), attribute->GetLabel());

	CAttributeFrame * win = g_attr_window[id].second;
	if (win && win->IsWindow())
	{
		win->GetText(result);
		return true;
	}
	return false;
}

bool SetAttributeText(IAttribute * attribute, const std::_tstring & text, bool resetSavePoint, bool clearUndo)
{
	WinID id(attribute->GetModuleQualifiedLabel(), attribute->GetLabel());

	CAttributeFrame * win = g_attr_window[id].second;
	if (win && win->IsWindow())
	{
		win->SetText(text.c_str(), resetSavePoint, clearUndo);
		return true;
	}
	return false;
}

void UpdateAttributeMDI(IAttribute * from, IAttribute * to)
{
	WinID id(from->GetModuleQualifiedLabel(), from->GetLabel());

	CAttributeFrame * win = g_attr_window[id].second;
	if (win && win->IsWindow())
	{
		if (to)
			win->UpdateAttribute(to);
		else
			win->PostMessage(WM_SYSCOMMAND, SC_CLOSE, 0L);
	}
}
