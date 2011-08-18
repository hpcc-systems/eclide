#include "StdAfx.h"
#include "..\en_us\resource.h"

#include "ModSelectDlg.h"
#include "RepositoryView.h"
#include <dali.h>
#include <CustomMessages.h>

class CModuleSelectDlg : 
	public CDialogImpl<CModuleSelectDlg>, 
	public CWinDataExchange<CModuleSelectDlg>,
	public CRepositorySlotImpl
{
	typedef CModuleSelectDlg thisClass;
	typedef CDialogImpl<thisClass> baseClass;

protected:
	IModuleAdapt    &m_mod;
	IRepositoryAdapt m_repository;
	CRepositoryView  m_view;
	CString			 m_prompt;

public:
	CModuleSelectDlg(IRepository *rep, IModuleAdapt &mod) 
		: m_repository(rep), m_mod(mod)
	{
		m_view.Init(this);
		ATLASSERT(m_repository.isLinked());
	}

	enum { IDD = IDD_ATTRLOOKUP };

	void SetPrompt(const TCHAR *prompt)
	{
		m_prompt = prompt;
	}

	//IRepositorySlot
	void OnDoubleClick(IModule &mod)
	{
		m_mod = &mod;
		EndDialog(IDOK);
	}

	//IRepositorySlot
	IRepository * GetRepository()
	{
		if ( !m_repository.isLinked() )
			return CRepositorySlotImpl::GetRepository();
		else
		{
			//this must return an Inc'd interface as AttachRepository does
			m_repository->AddRef();
			return m_repository.get();
		}
	}

	//IRepositorySlot
	bool ExpandModule(IModule & /*mod*/) 
	{
		return false;
	}

protected:
	BEGIN_MSG_MAP(thisClass)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		COMMAND_ID_HANDLER_EX(IDOK, OnOk)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
	END_MSG_MAP()

	BEGIN_DDX_MAP(thisClass)
	END_DDX_MAP()

	LRESULT OnInitDialog(HWND /*wParam*/, LPARAM /*lParam*/)
	{
		CWaitCursor cursor;
		CenterWindow(GetParent());

		if ( !m_prompt.IsEmpty() )
		{
			SetWindowText(m_prompt);
		}

		CWindow wndPlaceholder = GetDlgItem(IDC_STATIC_PLACEHOLDER);
		CRect rc;
		wndPlaceholder.GetWindowRect(rc);
		ScreenToClient(rc);
		wndPlaceholder.DestroyWindow();
		m_view.Create(*this, rc);
		m_view.SetDlgCtrlID(IDC_STATIC_PLACEHOLDER);
		m_view.SetWindowPos(HWND_TOP, rc, 0);
		m_view.SendMessage(CWM_RESET);
		m_view.Select(m_mod.get());

		DoDataExchange();

		return TRUE;
	}

	void OnClose()
	{
		SetMsgHandled(false);
	}

	void OnOk(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
	{
		IModule *mod = m_view.GetFirstSelectedModule();
		if ( mod )
		{
			m_mod = mod;
			EndDialog(IDOK);
		}
	}
	void OnCancel(UINT /*uNotifyCode*/, int nID, HWND /*hWnd*/)
	{
		EndDialog(nID);
	}
	//BOOL EndDialog(int nRetCode)
	//{
	//	return baseClass::EndDialog(nRetCode);
	//}
};

bool PickModule(IRepository *rep, IModuleAdapt &mod, const TCHAR *prompt)
{
	CModuleSelectDlg dlg(rep, mod);
	if ( prompt )
		dlg.SetPrompt(prompt);
	return IDOK == dlg.DoModal();
}
