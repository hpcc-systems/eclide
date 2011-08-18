#include "StdAfx.h"
#include "..\en_us\resource.h"

#include "commentdlg.h"
#include "attribute.h"
#include "HistoryView.h"
#include "RepositoryView.h"
#include "ChildAttributeFrame.h"
#include "RepositoryFrame.h"

class CCommentDlg : public WTL::CDialogImpl<CCommentDlg>, 
	public WTL::CWinDataExchange<CCommentDlg>, WTL::CMessageFilter
{
protected:
	CString &m_Comment;

public:
	bool m_ok;

	enum { IDD = IDD_COMMENT_NOHISTORY };

	CCommentDlg(CString &comment) : m_Comment(comment)
	{
		m_ok = false;
	}

	~CCommentDlg(void)
	{
	}

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		return IsDialogMessage(pMsg);
	}

	BEGIN_MSG_MAP(CCommentDlg)
		MSG_WM_INITDIALOG(OnInitDialog)

		COMMAND_ID_HANDLER_EX(IDOK, OnOk)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
	END_MSG_MAP()

	BEGIN_DDX_MAP(CCommentDlg)
		DDX_TEXT(IDC_EDIT_COMMENT, m_Comment)
	END_DDX_MAP()

	LRESULT OnInitDialog(HWND /*wParam*/, LPARAM /*lParam*/)
	{
		DoDataExchange();
		CenterWindow(GetParent());
		return TRUE;
	}

	void OnOk(UINT /*uNotifyCode*/, int nID, HWND /*hWnd*/)
	{
		DoDataExchange(true);
		m_ok = true;
		EndDialog(nID);
	}

	void OnCancel(UINT /*uNotifyCode*/, int nID, HWND /*hWnd*/)
	{
		m_ok = false;
		EndDialog(nID);
	}
};

class CCommentHistoryDlg : 
	public CRepositoryFrameT<CCommentHistoryDlg>, 
	public WTL::CDialogImpl<CCommentHistoryDlg>, 
	public WTL::CDialogResize<CCommentHistoryDlg>,
	public WTL::CWinDataExchange<CCommentHistoryDlg>,
	public IHistorySlot
{
	typedef WTL::CDialogImpl<CCommentHistoryDlg> thisClass;

protected:
	CTabbedChildWindow< CDotNetTabCtrl<CTabViewTabItem> > m_tabbedChildWindow;
	CComPtr<IAttribute> m_Attribute;
	CHistoryView m_historyView;
	IAttributeVectorPtr m_attrs;
	bool m_loaded;
	CString &m_Comment;
	WTL::CButton m_buttPrevious;
	WTL::CButton m_buttNext;

public:
	CAttributeVectorView m_view;
	bool m_ok;

	enum { IDD = IDD_COMMENT_HISTORY };

	CCommentHistoryDlg(IAttribute * attribute, CString &comment) :
		m_historyView(this), 
		m_view(this, m_attrs), 
		m_Attribute(attribute), 
		m_Comment(comment)
	{
		m_ok = false;
		m_loaded = false;
	}

	~CCommentHistoryDlg(void)
	{
	}

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		return IsDialogMessage(pMsg);
	}

	BEGIN_MSG_MAP(thisClass)
		MSG_WM_INITDIALOG(OnInitDialog)

		COMMAND_ID_HANDLER_EX(IDOK, OnOk)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
		COMMAND_ID_HANDLER_EX(ID_DIFFERENCES_PREVIOUS, OnDiffPrevious)
		COMMAND_ID_HANDLER_EX(ID_DIFFERENCES_NEXT, OnDiffNext)

		NOTIFY_CODE_HANDLER(CTCN_SELCHANGE, OnTabSelChange)
		//CHAIN_COMMANDS_MEMBER(m_tabbedChildWindow)
		CHAIN_MSG_MAP(WTL::CDialogResize<CCommentHistoryDlg>)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(thisClass)
		DLGRESIZE_CONTROL(IDC_STATIC_HISTORY, DLSZ_SIZE_X | DLSZ_SIZE_Y)
		DLGRESIZE_CONTROL(IDC_STATIC_COMMENT, DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDC_EDIT_COMMENT, DLSZ_SIZE_X | DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(ID_DIFFERENCES_PREVIOUS, DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(ID_DIFFERENCES_NEXT, DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDOK, DLSZ_MOVE_X | DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDCANCEL, DLSZ_MOVE_X | DLSZ_MOVE_Y)
	END_DLGRESIZE_MAP()

	BEGIN_DDX_MAP(thisClass)
		DDX_TEXT(IDC_EDIT_COMMENT, m_Comment)
	END_DDX_MAP()

	LRESULT OnInitDialog(HWND /*wParam*/, LPARAM /*lParam*/)
	{
		DoDataExchange();
		CenterWindow(GetParent());
		DlgResize_Init(true,false);

		CWindow wndPlaceholder = GetDlgItem(IDC_STATIC_HISTORY);
		CRect rc;
		wndPlaceholder.GetWindowRect(rc);
		ScreenToClient(rc);
		wndPlaceholder.DestroyWindow();

		m_buttPrevious = GetDlgItem(ID_DIFFERENCES_PREVIOUS);
		m_buttNext = GetDlgItem(ID_DIFFERENCES_NEXT);

		m_tabbedChildWindow.SetForwardNotifications(true);
		m_tabbedChildWindow.SetTabStyles(CTCS_TOOLTIPS);
		m_tabbedChildWindow.Create(m_hWnd, rc);
		m_tabbedChildWindow.SetWindowPos(HWND_TOP, rc, 0);
		m_tabbedChildWindow.SetDlgCtrlID(IDC_STATIC_HISTORY);

		CString ecl;
		GetAttributeText(m_Attribute, ecl);
		if (ecl.GetLength())
			m_historyView.SetCurrent(ecl);

		m_loaded = true;
		m_view.Create(m_tabbedChildWindow, rcDefault);
		m_tabbedChildWindow.AddTab(m_view, _T("Dependencies"));

		m_historyView.SetAttribute(m_Attribute);
		m_historyView.SetCompareMode(true);
		m_historyView.Create(m_tabbedChildWindow, rcDefault);
		m_tabbedChildWindow.AddTab(m_historyView, _T("Compare"), -1, 0);
		m_tabbedChildWindow.GetTabCtrl().SetCurSel(0);
		m_loaded = false;	//  Stop premature loading of dependancies...

		UpdateUI();

		return TRUE;
	}

	void OnOk(UINT /*uNotifyCode*/, int nID, HWND /*hWnd*/)
	{
		DoDataExchange(true);
		m_ok = true;
		EndDialog(nID);
	}

	void OnCancel(UINT /*uNotifyCode*/, int nID, HWND /*hWnd*/)
	{
		m_ok = false;
		EndDialog(nID);
	}

	void OnDiffPrevious(UINT uNotifyCode, int nID, HWND hWnd)
	{
		m_historyView.OnDifferencePrevious(uNotifyCode, nID, hWnd);
		UpdateUI();
	}

	void OnDiffNext(UINT uNotifyCode, int nID, HWND hWnd)
	{
		m_historyView.OnDifferenceNext(uNotifyCode, nID, hWnd);
		UpdateUI();
	}

	void UpdateUI()
	{
		if (!m_buttPrevious.IsWindowEnabled() && m_historyView.GetDiffPrevious() >= 0)
			m_buttPrevious.EnableWindow(true);
		else if (m_buttPrevious.IsWindowEnabled() && m_historyView.GetDiffPrevious() < 0)
			m_buttPrevious.EnableWindow(false);

		if (!m_buttNext.IsWindowEnabled() && m_historyView.GetDiffNext() >= 0)
			m_buttNext.EnableWindow(true);
		else if (m_buttNext.IsWindowEnabled() && m_historyView.GetDiffNext() < 0)
			m_buttNext.EnableWindow(false);
	}

	LRESULT OnTabSelChange(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
	{
		if(pnmh->hwndFrom == m_tabbedChildWindow.GetTabCtrl())
		{
			int index = m_tabbedChildWindow.GetTabCtrl().GetCurSel();
			if(index == 1 && m_loaded == false)
			{
				CWaitCursor wait;
				CString ecl;
				GetAttributeText(m_Attribute, ecl);
				if (ecl.GetLength())
				{
					Dali::AttributeVector daliAttrs;
					Dali::CEclExceptionVector errorResults;
					StlLinked<IRepository> rep = AttachRepository();
					StlLinked<Dali::IDali> dali = Dali::AttachDali();
					dali->GetDependancies(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_CLUSTER), GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_QUEUE), ecl, m_Attribute->GetModuleLabel(), m_Attribute->GetLabel(), daliAttrs, errorResults);
					//TODO:  Do somthing with the errors...
					for(Dali::AttributeVector::iterator itr = daliAttrs.begin(); itr != daliAttrs.end(); ++itr)
					{
						m_attrs.push_back(rep->GetAttribute(itr->first.c_str(), itr->second.c_str(), CreateIAttributeECLType()));
					}
					m_view.Refresh();
				}
				m_loaded = true;
			}
		}
		return 0;
	}


	//  ---  IHistorySlot  ---
	bool OnHistoryContext(WTL::CMenu &/*menu*/, CPoint &/*pt*/)
	{
		return false;
	}

	void SetAttributeText(IAttribute *attr, const CString &ecl, bool resetSavePoint, bool clearUndo)
	{
	}

	void OnRefresh()
	{
		UpdateUI();
	}
};

bool ShowComment(IAttribute * attribute, CString & comment)
{
	CCommentHistoryDlg dlg(attribute, comment);
	dlg.DoModal();
	return dlg.m_ok;
}

bool ShowComment(CString & comment)
{
	CCommentDlg dlg(comment);
	dlg.DoModal();
	return dlg.m_ok;
}
