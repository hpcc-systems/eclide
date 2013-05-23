#include "StdAfx.h"
#include "..\en_us\resource.h"

#include "AttrListDlg.h"
#include "global.h"
#include "RepositoryView.h"
#include "WorkspaceView.h"
#include "UnicodeFile.h"
#include "Migration.h"
#include "EclCC.h"
#include "HListBox.h"
#include <Logger.h>
#include <UtilFilesystem.h>

//  ===========================================================================
class CAttrDlg : 
	public CDialogImpl<CAttrDlg>, 
	public CWorkspaceSlotImpl,
	public CRepositorySlotImpl
{
	typedef CAttrDlg thisClass;
	typedef CDialogImpl<thisClass> baseClass;

public:
	CProjectVectorView m_view;
	CString m_title;
	CString m_prompt;
	IRepositoryAdapt & m_rep;
	CButton m_checkSandbox; 
	CButton m_checkWorkspaces; 
	bool & m_sandbox;
	bool m_disableWorkspaceCheck;
	bool & m_workspaces;

	CAttrDlg(IRepositoryAdapt rep, IWorkspaceVector & workspaces, IAttributeHistoryVector & attrs, bool & sandbox, bool & includeWorkspaces, const TCHAR* title=0, const TCHAR* prompt=0) 
		: m_view(this, this, workspaces, attrs), m_rep(rep), m_sandbox(sandbox), m_workspaces(includeWorkspaces)
	{
		if ( title )
			m_title = title;
		if ( prompt )
			m_prompt = prompt;
		m_disableWorkspaceCheck = workspaces.empty();
	}

	IRepository * GetRepository()
	{
		//must return a "high" ref counted object
		IRepositoryAdapt rep = m_rep;
		return rep.Detach();
	}

	enum { IDD = IDD_ATTRVERIFY };

	BEGIN_MSG_MAP(thisClass)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		COMMAND_ID_HANDLER_EX(IDOK, OnOk)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
		COMMAND_ID_HANDLER_EX(IDC_BUTTON_CHECKSYNTAX, OnCheckSyntax)
	END_MSG_MAP()

	LRESULT OnInitDialog(HWND /*wParam*/, LPARAM /*lParam*/)
	{
		CWaitCursor cursor;
		CenterWindow(GetParent());

		CWindow wndPlaceholder = GetDlgItem(IDC_STATIC_PLACEHOLDER);
		CRect rc;
		wndPlaceholder.GetWindowRect(rc);
		ScreenToClient(rc);
		wndPlaceholder.DestroyWindow();
		m_view.Create(*this, rc);
		m_view.SetDlgCtrlID(IDC_STATIC_PLACEHOLDER);
		m_view.SetWindowPos(HWND_TOP, rc, 0);

		m_checkSandbox = GetDlgItem(IDC_CHECK_SANDBOX);
		m_checkWorkspaces = GetDlgItem(IDC_CHECK_WORKSPACES);

		if ( !m_title.IsEmpty() )
			SetWindowText(m_title);

		SetDlgItemText(IDC_STATIC_CAPTION, m_prompt);

		m_checkSandbox.SetCheck(m_sandbox);
		if (m_disableWorkspaceCheck)
		{
			m_workspaces = false;
			m_checkWorkspaces.EnableWindow(FALSE);
		}
		else
			m_checkWorkspaces.SetCheck(m_workspaces);

		return TRUE;
	}
	void OnClose()
	{
		SetMsgHandled(false);
	}
	void OnOk(UINT /*uNotifyCode*/, int nID, HWND /*hWnd*/)
	{
		m_sandbox = m_checkSandbox.GetCheck() == BST_CHECKED;
		m_workspaces = m_checkWorkspaces.GetCheck() == BST_CHECKED;
		EndDialog(IDOK);
	}
	void OnCancel(UINT /*uNotifyCode*/, int nID, HWND /*hWnd*/)
	{
		EndDialog(IDCANCEL);
	}

	void OnCheckSyntax(UINT /*uNotifyCode*/, int nID, HWND /*hWnd*/)
	{
		m_view.CheckSyntax();
	}
};

INT_PTR DoShowAttributesDlg(HWND hwndParent, IRepositoryAdapt rep, IWorkspaceVector & workspaces, IAttributeHistoryVector & attrs, bool & sandbox, bool & includeWorkspaces, const TCHAR* title, const TCHAR* prompt)
{
	CAttrDlg dlg(rep, workspaces, attrs, sandbox, includeWorkspaces, title, prompt);
	return dlg.DoModal(hwndParent);
}
//  ===========================================================================
class CAttrSyntaxDlg : 
	public CDialogImpl<CAttrSyntaxDlg>, 
	public CWorkspaceSlotImpl
{
	typedef CAttrDlg thisClass;
	typedef CDialogImpl<thisClass> baseClass;

public:
	CProjectVectorView m_view;
	IRepositoryAdapt & m_rep;
	IWorkspaceVector m_ws;

	CAttrSyntaxDlg(IRepositoryAdapt rep, IAttributeHistoryVector & attrs, IRepositorySlot * repSlot) 
		: m_view(this, repSlot, m_ws, attrs), m_rep(rep)
	{
	}

	IRepository * GetRepository()
	{
		//must return a "high" ref counted object
		IRepositoryAdapt rep = m_rep;
		return rep.Detach();
	}

	enum { IDD = IDD_ATTRSYNTAX };

	BEGIN_MSG_MAP(thisClass)
		MSG_WM_INITDIALOG(OnInitDialog)

		COMMAND_ID_HANDLER_EX(IDOK, OnOk)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
		COMMAND_ID_HANDLER_EX(ID_OPENALLWITHERRORS, OnOpenAllWithErrors)
	END_MSG_MAP()

	LRESULT OnInitDialog(HWND /*wParam*/, LPARAM /*lParam*/)
	{
		CWaitCursor cursor;
		CenterWindow(GetParent());

		CWindow wndPlaceholder = GetDlgItem(IDC_STATIC_PLACEHOLDER);
		CRect rc;
		wndPlaceholder.GetWindowRect(rc);
		ScreenToClient(rc);
		wndPlaceholder.DestroyWindow();
		m_view.Create(*this, rc);
		m_view.SetDlgCtrlID(IDC_STATIC_PLACEHOLDER);
		m_view.SetWindowPos(HWND_TOP, rc, 0);

		m_view.CheckSyntax();

		return TRUE;
	}
	void OnOk(UINT /*uNotifyCode*/, int nID, HWND /*hWnd*/)
	{
		EndDialog(IDOK);
	}
	void OnCancel(UINT /*uNotifyCode*/, int nID, HWND /*hWnd*/)
	{
		EndDialog(nID);
	}
	void OnOpenAllWithErrors(UINT /*uNotifyCode*/, int nID, HWND /*hWnd*/)
	{
		CWaitCursor wait;
		for (WTL::CTreeItem pos = m_view.m_Tree.GetRootItem(); pos != NULL; pos = m_view.m_Tree.GetNextTreeItem(pos))
		{
			WTL::CTreeItem item(pos, &m_view.m_Tree);
			if (CComQIPtr<CAttributeHistoryNode> node = reinterpret_cast<CTreeNode *>(item.GetData()))
			{
				if (node->GetSyntaxState() == SYNTAX_CHECK_FAIL)
					node->OnDblclk((HTREEITEM)0);
			}
		}
	}
};

void DoShowSyntaxDlg(HWND hwndParent, IRepositoryAdapt rep, IAttributeVector & _attrs, IRepositorySlot * repSlot)
{
	IAttributeHistoryVector attrs;
	for(IAttributeVector::const_iterator itr = _attrs.begin(); itr != _attrs.end(); ++itr)
		attrs.push_back(itr->get()->GetAsHistory());

	CAttrSyntaxDlg dlg(rep, attrs, repSlot);
	dlg.DoModal(hwndParent);
}
//  ===========================================================================
typedef std::pair<std::_tstring, std::_tstring> CommentEclPair;
typedef std::pair<std::_tstring, IAttributeTypeAdapt> ModAttrTypePair;
typedef std::map<ModAttrTypePair, CommentEclPair> AttrMap;
class CAttrImportDlg : 
	public CDialogImpl<CAttrImportDlg>, 
	public CRepositorySlotImpl,
	public IMigrationCallback
{
	typedef CAttrImportDlg thisClass;
	typedef CDialogImpl<thisClass> baseClass;

public:
	IRepositoryAdapt & m_rep;
	const AttrMap & m_attrs;
	CHListBox m_listAttrs;
	CComPtr<CComboModule> m_comboModuleCtrl;
	IModuleAdapt m_targetModule;
	CButton m_checkSandbox; 
	CComPtr<IMigration> m_migrator;
	bool m_defaultToSandbox;

	CAttrImportDlg(IRepositoryAdapt target, const AttrMap & attrs, bool defaultToSandbox)
		: m_rep(target), m_attrs(attrs)
	{
		m_comboModuleCtrl = new CComboModule();
		m_defaultToSandbox = defaultToSandbox;
	}

	IRepository * GetRepository()
	{
		//must return a "high" ref counted object
		IRepositoryAdapt rep = m_rep;
		return rep.Detach();
	}

	IModule * GetTargetModule() 
	{
		if (IsLocalRepositoryEnabled())
			return m_comboModuleCtrl->GetSelectedIModule();
		return NULL;
	}

	enum { IDD = IDD_ATTRVERIFY2 };

	BEGIN_MSG_MAP(thisClass)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_TIMER(OnTimer)
		COMMAND_ID_HANDLER_EX(IDOK, OnOk)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
	END_MSG_MAP()

	LRESULT OnInitDialog(HWND /*wParam*/, LPARAM /*lParam*/)
	{
		CWaitCursor cursor;
		CenterWindow(GetParent());

		m_listAttrs = GetDlgItem(IDC_LIST_ATTRIBUTES);
		for(AttrMap::const_iterator itr = m_attrs.begin(); itr != m_attrs.end(); ++itr)
		{
			m_listAttrs.AddString(itr->first.first.c_str());
		}

		*m_comboModuleCtrl = GetDlgItem(IDC_COMBO_MODULE);
		m_checkSandbox = GetDlgItem(IDC_CHECK_SANDBOX);

		if (IsLocalRepositoryEnabled())
		{
			m_comboModuleCtrl->Load(_T(""), false, false);

			m_defaultToSandbox = false;
			m_checkSandbox.EnableWindow(False);
		}
		else
		{
			GetDlgItem(IDC_STATIC_TO).EnableWindow(false);
			m_comboModuleCtrl->EnableWindow(false);
			if (m_defaultToSandbox)
				m_checkSandbox.SetCheck(true);
		}

		return TRUE;
	}
	void OnClose()
	{
		SetMsgHandled(false);
	}
	void OnOk(UINT /*uNotifyCode*/, int nID, HWND /*hWnd*/)
	{
		m_targetModule = GetTargetModule();

		CComPtr<IRepository> rep = AttachRepository();
		m_migrator = CreateIMigration(rep, false, this);
		m_migrator->Stop();
		for(AttrMap::const_iterator itr = m_attrs.begin(); itr != m_attrs.end(); ++itr)
			m_migrator->AddEclToModule(m_targetModule, itr->first.first, itr->first.second, itr->second.first, itr->second.second, static_cast<const TCHAR *>(CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_USER))), m_checkSandbox.GetCheck() != 0);

		GetDlgItem(IDOK).EnableWindow(FALSE);
		GetDlgItem(IDCANCEL).EnableWindow(FALSE);
		m_migrator->Start();
		SetTimer(0, 200);
		m_listAttrs.ResetContent();
		//m_sandbox = m_checkSandbox.GetCheck() == BST_CHECKED;
		//EndDialog(IDOK);
	}
	void OnTimer(UINT_PTR nIDEvent)
	{
		if (m_migrator->GetActiveMigrationCount() == 0)
		{
			GetDlgItem(IDC_STATIC_CAPTION).SetWindowText(_T("Finished."));	
			GetDlgItem(IDOK).ShowWindow(SW_HIDE);
			GetDlgItem(IDCANCEL).SetWindowText(_T("Cl&ose"));
			GetDlgItem(IDCANCEL).EnableWindow(TRUE);
		}
	}

	void OnCancel(UINT /*uNotifyCode*/, int nID, HWND /*hWnd*/)
	{
		EndDialog(IDCANCEL);
	}
	BOOL Invalidate(BOOL bErase = TRUE) 
	{
		return baseClass::Invalidate(bErase);
	}
	void LogMsg(const std::_tstring & msg) 
	{
		m_listAttrs.AddString(msg.c_str());
		if (boost::algorithm::istarts_with(msg, _T("Failed")))
			_DBGLOG(LEVEL_WARNING, msg.c_str());
	}
	void PostStatus(const TCHAR* pStr) 
	{
	}
	void PostProgress(int progress) 
	{
	}
};

IModule * DoConfirmImportDlg(HWND hwndParent, const boost::filesystem::path & path)
{
#define IMPORT_MARKER _T("//Import:")
#define COMMENT_MARKER _T("//Comment:")
	std::_tstring module;
	CUnicodeFile file;
	if (file.Open(pathToWString(path).c_str()))
	{
		file.Read(module);
		file.Close();

		AttrMap attrs;
		std::_tstring attributeLabel;
		std::_tstring attributeComment;
		std::_tstring attributeEcl;

		typedef std::vector<std::_tstring> split_vector_type;
		split_vector_type SplitVec; 
		boost::algorithm::split(SplitVec, module, boost::algorithm::is_any_of("\r\n"), boost::algorithm::token_compress_on);

		for (split_vector_type::const_iterator itr = SplitVec.begin(); itr != SplitVec.end(); ++itr)
		{
			std::_tstring line = *itr;
			if (boost::algorithm::istarts_with(line, IMPORT_MARKER))
			{
				if (attributeLabel.length() && attributeEcl.length())
					attrs[std::make_pair(attributeLabel, CreateIAttributeECLType())] = CommentEclPair(attributeComment, attributeEcl);

				boost::algorithm::ireplace_first(line, IMPORT_MARKER, _T(""));
				//  Fix DABs new module bracketing
				boost::algorithm::replace_all(line, _T("<"), _T(""));
				boost::algorithm::replace_all(line, _T(">"), _T("."));
				//  ---
				boost::algorithm::trim(line);
				attributeLabel = line;
				attributeComment.clear();
				attributeEcl.clear();
			}
			else if (boost::algorithm::istarts_with(line, COMMENT_MARKER))
			{
				boost::algorithm::ireplace_first(line, COMMENT_MARKER, _T(""));
				boost::algorithm::trim(line);
				attributeComment += line + _T("\r\n");
			}
			else
			{
				attributeEcl += line + _T("\r\n");
			}
		}
		if (attributeLabel.length() && attributeEcl.length())
			attrs[std::make_pair(attributeLabel, CreateIAttributeECLType())] = CommentEclPair(attributeComment, attributeEcl);

		CAttrImportDlg dlg(AttachRepository(), attrs, true);
		if (dlg.DoModal(hwndParent) == IDOK)
			return dlg.m_targetModule;
	}
	return NULL;
}
