#include "StdAfx.h"
#include "..\en_us\resource.h"

#include "attributedlg.h"
#include ".\simplepromptdlg.h"
#include "global.h"
#include "RepositoryView.h"
#include "dali.h"
#include "wutil.h"
#include <CustomMessages.h>

//  ===========================================================================
class CSelAttrDlg : 
	public CDialogImpl<CSelAttrDlg>,
	public CRepositorySlotImpl
{
	typedef CSelAttrDlg thisClass;
	typedef CDialogImpl<thisClass> baseClass;

protected:
	IModuleAdapt		 m_module;
	CComPtr<IAttribute> &m_attr;
	IRepositoryAdapt	 m_repository;
	CRepositoryView		 m_view;
	CString				 m_prompt;

public:
	CSelAttrDlg(IRepository *rep, CComPtr<IAttribute> &attr) 
		: m_repository(rep), m_attr(attr)
	{
		m_view.Init(this);
		if ( !m_repository.isLinked() )
			m_repository = attr->GetRepository();
	}

	enum { IDD = IDD_ATTRLOOKUP };

	void SetPrompt(const TCHAR *prompt)
	{
		m_prompt = prompt;
	}

	//IRepositorySlot
	void OnDoubleClick(IAttribute &attr)
	{
		m_attr = &attr;
		EndDialog(IDOK);
	}

	//IRepositorySlot
	void OnItemSelectionChanged(CTreeNode * /*node*/, bool bSelected)
	{
		//enable the Create button if only one module selected
		if ( m_view.GetSelectedCount() == 1 )
		{
			m_module = m_view.GetFirstSelectedModule();
			bSelected = m_module.isLinked();
		}
		else
			bSelected = false;

		GetDlgItem(IDC_CREATE_ATTR).EnableWindow(bSelected);
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

	BEGIN_MSG_MAP(thisClass)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		COMMAND_ID_HANDLER_EX(IDOK, OnOk)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
		COMMAND_ID_HANDLER_EX(IDC_CREATE_ATTR, OnCreateAttr )
	END_MSG_MAP()

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
		m_view.Select(m_attr);
		m_view.Invalidate();
		m_view.SetFocus();

		return TRUE;
	}

	void OnClose()
	{
		SetMsgHandled(false);
	}

	void OnCreateAttr(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
	{
		CString name;
		if ( ::ShowSimplePrompt(m_hWnd,_T("Create File"),_T("Name:"),name) )
		{
			//because this is in EclLib, don't want to use it at this time
			//if ( !::IsValidIdentifier(std::_tstring(name)) )

			//just do the spaces check
			if ( name.Find(' ') >= 0 )
			{
				CString prompt;
				prompt.FormatMessage(IDS_BAD_ATTRNAME, (const TCHAR *)name );
				MessageBox(prompt);
			}
			else
			{
				m_attr = m_module->InsertAttribute(name, CreateIAttributeECLType());
				if (m_attr.p)
				{
					CWaitCursor cursor;
					m_view.SendMessage(CWM_RESET);
					m_view.Select(m_attr);
					m_view.Invalidate(); //doesn't seem to paint without this
					m_view.UpdateWindow();
				}
				else
				{
					CString prompt;
					prompt.LoadString(IDS_GQB_NOCREATEATTR);
					MessageBox(prompt);
				}
			}
		}
	}
	void OnOk(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
	{
		m_attr = m_view.GetFirstSelectedAttribute();
		EndDialog(m_attr ? IDOK : IDCANCEL);
	}
	void OnCancel(UINT /*uNotifyCode*/, int nID, HWND /*hWnd*/)
	{
		EndDialog(nID);
	}
	BOOL EndDialog(int nRetCode)
	{
		return baseClass::EndDialog(nRetCode);
	}
};

//  ===========================================================================
INT_PTR DoGetAttr(CComPtr<IAttribute> &attr)
{
	CSelAttrDlg dlg(0, attr);
	return dlg.DoModal();
}

INT_PTR DoGetAttr(IRepository *rep, CComPtr<IAttribute> &attr, const TCHAR *prompt)
{
	CSelAttrDlg dlg(rep, attr);
	if ( prompt )
		dlg.SetPrompt(prompt);
	return dlg.DoModal();
}
//  ===========================================================================

