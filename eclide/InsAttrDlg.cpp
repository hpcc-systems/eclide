#include "StdAfx.h"
#include "..\en_us\resource.h"

#include "InsAttrDlg.h"
#include "repository.h"
#include "SourceView.h"
#include "EclParser.h"
#include "workspace.h"
#include "StdString.h"

const TCHAR * const ERR_INVALID_IDENTIFIER_TITLE = _T("Invalid Identifier");
const TCHAR * const ERR_INVALID_IDENTIFIER = _T("Identifier must start with a character or '_' and can contain characters, numbers and '_'.");
const TCHAR * const ERR_IDENTIFIER_EXISTS_AS_MODULE = _T("Identifier already exists as a folder label.");
const TCHAR * const ERR_IDENTIFIER_EXISTS_AS_ATTRIBUTE = _T("Identifier already exists as a file label.");
const TCHAR * const ERR_IDENTIFIER_EXISTS_AS_WORKSPACE = _T("Identifier already exists as a workspace label, Overwrite?");
const TCHAR * const ERR_INVALID_IDENTIFIER_WORKSPACE = _T("Identifier cannot contain any of the following characters - '< > : \" / \\ | ? * .'.");
//  ===========================================================================
__interface ILabelValidator
{
	bool OnOk(const std::_tstring & label, std::_tstring &error_title, std::_tstring &error_msg);
	bool MessageBox(HWND hwnd, std::_tstring &error_title, std::_tstring &error_msg);
};

__interface IAttrLabelValidator : public ILabelValidator
{
	bool OnOk(const std::_tstring & label, IAttributeType * type, std::_tstring &error_title, std::_tstring &error_msg);
};

class CModuleIdentifierValidator : public ILabelValidator
{
protected:
	CComPtr<IModule> m_parent;

public:
	CModuleIdentifierValidator(IModule * parent)
	{
		m_parent = parent;
	}

	bool OnOk(const std::_tstring & label, std::_tstring &error_title, std::_tstring &error_msg)
	{
		if (IsValidIdentifier(label))
		{
			std::_tstring qualifiedLabel;
			if (m_parent)
				qualifiedLabel = std::_tstring(m_parent->GetQualifiedLabel()) + _T(".");
			qualifiedLabel += label;
			CComPtr<IRepository> rep = ::AttachRepository();
			if (!rep->ModuleExists(qualifiedLabel.c_str()))
			{
				return true;
			}
			error_title = ERR_INVALID_IDENTIFIER_TITLE;
			error_msg = ERR_IDENTIFIER_EXISTS_AS_MODULE;
		}
		else
		{
			error_title = ERR_INVALID_IDENTIFIER_TITLE;
			error_msg = ERR_INVALID_IDENTIFIER;
		}
		return false;
	}

	bool MessageBox(HWND hwnd, std::_tstring &error_title, std::_tstring &error_msg)
	{
		::MessageBox(hwnd, error_title.c_str(), error_msg.c_str(), MB_ICONASTERISK);
		return false;
	}
};

class CAttributeIdentifierValidator : public IAttrLabelValidator
{
protected:
	const std::_tstring & m_module;
	CComPtr<IAttributeType> m_type;

public:
	CAttributeIdentifierValidator(const std::_tstring & module, IAttributeType * type) : m_module(module), m_type(type)
	{
	}

	bool OnOk(const std::_tstring & label, std::_tstring &error_title, std::_tstring &error_msg)
	{
		ATLASSERT(m_type);
		return OnOk(label, m_type, error_title, error_msg);
	}

	bool OnOk(const std::_tstring & label, IAttributeType * type, std::_tstring &error_title, std::_tstring &error_msg)
	{
		if (IsValidIdentifier(label))
		{
			CComPtr<IRepository> rep = ::AttachRepository();
			ATLASSERT(m_module.length());
			if (!rep->AttributeExists(m_module.c_str(), label.c_str(), type))
			{
				return true;
			}
			error_title = ERR_INVALID_IDENTIFIER_TITLE;
			error_msg = ERR_IDENTIFIER_EXISTS_AS_ATTRIBUTE;
		}
		else
		{
			error_title = ERR_INVALID_IDENTIFIER_TITLE;
			error_msg = ERR_INVALID_IDENTIFIER;
		}
		return false;
	}

	bool MessageBox(HWND hwnd, std::_tstring &error_title, std::_tstring &error_msg)
	{
		::MessageBox(hwnd, error_title.c_str(), error_msg.c_str(), MB_ICONASTERISK);
		return false;
	}
};

class CRepLabelValidator : public ILabelValidator
{
public:
	bool OnOk(const std::_tstring & label, std::_tstring &error_title, std::_tstring &error_msg)
	{
		CComPtr<IRepository> rep = ::AttachRepository();
		std::_tstring error;
		if (rep->Label(label.c_str(), error))
		{
			if (!error.length())
			{
				return true;
			}
		}
		error_title = ERR_INVALID_IDENTIFIER_TITLE;
        error_msg = error;
		return false;
	}

	bool MessageBox(HWND hwnd, std::_tstring &error_title, std::_tstring &error_msg)
	{
		::MessageBox(hwnd, error_title.c_str(), error_msg.c_str(), MB_ICONASTERISK);
		return false;
	}
};

class CWorkspaceIdentifierValidator : public ILabelValidator
{
protected:
	CComPtr<IRepository> m_repository;
	bool m_errExists;

public:
	CWorkspaceIdentifierValidator(IRepository * repository)
	{
		m_repository = repository;
		m_errExists = false;
	}

	bool IsValidIdentifier(const std::_tstring & label)
	{
		if (label.size() == 0)
			return false;
		for (std::_tstring::size_type i = 0; i < label.size(); ++i)
		{
			switch (label.at(i))
			{
			case '<':
			case '>':
			case ':':
			case '\"':
			case '/':
			case '\\':
			case '|':
			case '?':
			case '*':
			case '.':
				return false;
			}
		}
		return true;
	}

	bool OnOk(const std::_tstring & label, std::_tstring &error_title, std::_tstring &error_msg)
	{
		if (IsValidIdentifier(label))
		{
			if (!m_repository->WorkspaceExists(label))
				return true;

			error_title = ERR_INVALID_IDENTIFIER_TITLE;
			error_msg = ERR_IDENTIFIER_EXISTS_AS_WORKSPACE;
			m_errExists = true;
		}
		else
		{
			error_title = ERR_INVALID_IDENTIFIER_TITLE;
			error_msg = ERR_INVALID_IDENTIFIER_WORKSPACE;
		}
		return false;
	}

	bool MessageBox(HWND hwnd, std::_tstring &error_title, std::_tstring &error_msg)
	{
		if (m_errExists)
		{
			if (::MessageBox(hwnd, error_title.c_str(), error_msg.c_str(), MB_ICONASTERISK | MB_YESNO) == IDYES)
				return true;
			m_errExists = false;
		}
		else
			::MessageBox(hwnd, error_title.c_str(), error_msg.c_str(), MB_ICONASTERISK);
		return false;
	}
};
//  ===========================================================================
class CLabelDlg : public WTL::CDialogImpl<CLabelDlg>, public WTL::CWinDataExchange<CLabelDlg>
{
protected:
	CString m_Title;
	CString &m_Label;
	ILabelValidator *m_validator;

public:
	bool m_ok;
	enum { IDD = IDD_LABEL };

	CLabelDlg(const CString & title, CString &label, ILabelValidator *validator = NULL) : m_Title(title), m_Label(label)
	{
		m_ok = false;
		m_validator = validator;
	}

	~CLabelDlg(void)
	{
	}

	BEGIN_MSG_MAP(CLabelDlg)
		MSG_WM_INITDIALOG(OnInitDialog)

		COMMAND_ID_HANDLER_EX(IDOK, OnOk)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
	END_MSG_MAP()

	BEGIN_DDX_MAP(CLabelDlg)
		DDX_TEXT(IDC_EDIT_LABEL, m_Label)
	END_DDX_MAP()

	LRESULT OnInitDialog(HWND /*wParam*/, LPARAM /*lParam*/)
	{
		SetWindowText(m_Title);
		DoDataExchange();
		CenterWindow(GetParent());
		return TRUE;
	}

	void OnOk(UINT /*uNotifyCode*/, int nID, HWND /*hWnd*/)
	{
		DoDataExchange(true);
		m_ok = true;
		if (!m_validator)
		{
			EndDialog(nID);
			return;
		}
		std::_tstring errTitle, errMsg;
		if (m_validator->OnOk(static_cast<const TCHAR *>(m_Label), errTitle, errMsg))
		{
			EndDialog(nID);
			return;
		}
		else
		{
			if (m_validator->MessageBox(*this, errMsg, errTitle))
			{
				EndDialog(nID);
				return;
			}
		}
		MessageBeep(MB_ICONEXCLAMATION);
	}

	void OnCancel(UINT /*uNotifyCode*/, int nID, HWND /*hWnd*/)
	{
		m_ok = false;
		EndDialog(nID);
	}
};
//  ===========================================================================
class CAttrLabelDlg : public WTL::CDialogImpl<CAttrLabelDlg>, public WTL::CWinDataExchange<CAttrLabelDlg>
{
protected:
	CString m_Title;
	CString &m_Label;
	CComPtr<IAttributeType> &m_type;
	IAttrLabelValidator *m_validator;
	WTL::CComboBox m_comboType;

public:
	bool m_ok;
	enum { IDD = IDD_INSERT_ATTRIBUTE };

	CAttrLabelDlg(const CString & title, CString &label, CComPtr<IAttributeType> & type, IAttrLabelValidator *validator = NULL) : m_Title(title), m_Label(label), m_type(type)
	{
		m_ok = false;
		m_validator = validator;
		m_type = CreateIAttributeECLType();
	}

	~CAttrLabelDlg(void)
	{
	}

	BEGIN_MSG_MAP(CAttrLabelDlg)
		MSG_WM_INITDIALOG(OnInitDialog)

		COMMAND_ID_HANDLER_EX(IDOK, OnOk)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
	END_MSG_MAP()

	BEGIN_DDX_MAP(CLabelDlg)
		DDX_TEXT(IDC_EDIT_LABEL, m_Label)
	END_DDX_MAP()

	LRESULT OnInitDialog(HWND /*wParam*/, LPARAM /*lParam*/)
	{
		SetWindowText(m_Title);
		DoDataExchange();
		CenterWindow(GetParent());
		m_comboType.Attach(GetDlgItem(IDC_COMBO_TYPE));
		CComPtr<IRepository> rep = ::AttachRepository();
		IAttributeTypeVector types;
		rep->GetAttributeTypes(types);
		for(IAttributeTypeVector::const_iterator itr = types.begin(); itr != types.end(); ++itr)
		{
			int item = m_comboType.AddString(itr->get()->GetDescription());
			m_comboType.SetItemData(item, (DWORD_PTR)itr->get());
		}
		int nSel = m_comboType.SelectString(0, m_type->GetDescription());
		ATLASSERT(nSel != CB_ERR);
		return TRUE;
	}

	void OnOk(UINT /*uNotifyCode*/, int nID, HWND /*hWnd*/)
	{
		DoDataExchange(true);
		m_ok = true;
		if (!m_validator)
		{
			EndDialog(nID);
			return;
		}
		std::_tstring errTitle, errMsg;
		m_type = (IAttributeType *)m_comboType.GetItemData(m_comboType.GetCurSel());
		ATLASSERT(m_type);
		if (m_validator->OnOk(static_cast<const TCHAR *>(m_Label), m_type, errTitle, errMsg))
		{
			EndDialog(nID);
			return;
		}
		else
		{
			if (m_validator->MessageBox(*this, errMsg, errTitle))
			{
				EndDialog(nID);
				return;
			}
		}
		MessageBeep(MB_ICONEXCLAMATION);
	}

	void OnCancel(UINT /*uNotifyCode*/, int nID, HWND /*hWnd*/)
	{
		m_ok = false;
		EndDialog(nID);
	}
};
//  ===========================================================================
class CModuleDlg : public WTL::CDialogImpl<CModuleDlg>, public CWinDataExchangeStd<CModuleDlg>
{
protected:
	std::_tstring &m_Label;
	WTL::CComboBox m_Combo;

public:
	bool m_ok;
	enum { IDD = IDD_SELECT_MODULE };

	CModuleDlg(std::_tstring &label) : m_Label(label)
	{
		m_ok = false;
	}

	~CModuleDlg(void)
	{
	}

	BEGIN_MSG_MAP(CModuleDlg)
		MSG_WM_INITDIALOG(OnInitDialog)

		COMMAND_ID_HANDLER_EX(IDOK, OnOk)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
	END_MSG_MAP()

	BEGIN_DDX_MAP(CModuleDlg)
		DDX_COMBO_VALUE(IDC_COMBO_MODULE, m_Label);
	END_DDX_MAP()

	LRESULT OnInitDialog(HWND /*wParam*/, LPARAM /*lParam*/)
	{
		m_Combo = GetDlgItem(IDC_COMBO_MODULE);
		CComPtr<IRepository> rep = ::AttachRepository();
		IModuleVector modules;
		rep->GetAllModules(modules);
		for(IModuleVector::iterator itr = modules.begin(); itr != modules.end(); ++itr)
		{
			m_Combo.AddString(itr->get()->GetQualifiedLabel());
		}
		DoDataExchange();
		m_Combo.SetCurSel(0);
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

bool GetInsertModule(IModule * parentModule, CString & label, bool validate)
{
	CModuleIdentifierValidator validator(parentModule);
	CLabelDlg dlg(_T("Insert Folder"), label, validate ? &validator : NULL);
	dlg.DoModal();
	if (parentModule)
		label = CString(parentModule->GetQualifiedLabel()) + _T(".") + label;
	return dlg.m_ok;
}

bool GetRenameModule(IModule * parentModule, CString & label, bool validate)
{
	CModuleIdentifierValidator validator(parentModule);
	CLabelDlg dlg(_T("Rename Folder"), label, validate ? &validator : NULL);
	dlg.DoModal();
	if (parentModule)
		label = CString(parentModule->GetQualifiedLabel()) + _T(".") + label;
	return dlg.m_ok;
}

bool GetInsertAttribute(const std::_tstring & module, CString & label, CComPtr<IAttributeType> & type, bool validate)
{
	CAttributeIdentifierValidator validator(module, NULL);
	CString title;
	title.Format(_T("Insert File into %s"), module.c_str());
	CAttrLabelDlg dlg(title, label, type, validate ? &validator : NULL);
	dlg.DoModal();
	return dlg.m_ok;
}

bool GetRenameAttribute(const std::_tstring & module, CString & label, IAttributeType * type, bool validate)
{
	CAttributeIdentifierValidator validator(module, type);
	CLabelDlg dlg(_T("Rename File"), label, validate ? &validator : NULL);
	dlg.DoModal();
	return dlg.m_ok;
}

bool SetVersionLabel(CString & label, bool SetAndValidate)
{
	CRepLabelValidator validator;
	CLabelDlg dlg(_T("Label Repository"), label, SetAndValidate ? &validator : NULL);
	dlg.DoModal();
	return dlg.m_ok;
}

bool GetModuleLabel(std::_tstring & label)
{
	CModuleDlg dlg(label);
	dlg.DoModal();
	return dlg.m_ok;
}

bool GetAttributeLabel(CString & label)
{
	CLabelDlg dlg(_T("Open File"), label, NULL);
	dlg.DoModal();
	return dlg.m_ok;
}

bool SaveWorkspaceLabel(IRepository * repository, CString & label)
{
	CWorkspaceIdentifierValidator validator(repository);
	CLabelDlg dlg(_T("Save Workspace"), label, &validator);
	dlg.DoModal();
	return dlg.m_ok;
}
