#pragma once

#include "ResultView.h"
#include "TabPane.h"
#include <CustomMessages.h>

class CExceptionView : 
	public WTL::CDialogImpl<CExceptionView>,
	public WTL::CDialogResize<CExceptionView>,
	public WTL::CWinDataExchange<CExceptionView>,
	public CTabPane,
	public IResultView
{
	typedef CExceptionView thisClass;
	typedef WTL::CDialogImpl<thisClass> baseClass;
private:
	CComPtr<Dali::IWorkunit> m_wu;
	CString m_exception;
	WTL::CEdit m_edit;
	IResultSlot *m_resultSlot;
	bool m_checkErrors;
	bool m_checkWarnings;
	bool m_checkInformation;

public:
	enum {IDD = IDD_EXCEPTIONVIEW};

	CExceptionView(Dali::IWorkunit * wu, IResultSlot *resultSlot);

	BEGIN_CUNKNOWN
		IMPLEMENT_INTERFACE(IResultView)
	END_CUNKNOWN(CTabPane)

	BEGIN_MSG_MAP(thisClass)
		MSG_WM_INITDIALOG(OnInitDialog)
		MESSAGE_HANDLER_EX(CWM_INITIALIZE, OnInitialize)

		COMMAND_HANDLER(IDC_CHECK_ERRORS, BN_CLICKED, OnBnClickedCheckFilter)
		COMMAND_HANDLER(IDC_CHECK_WARNINGS, BN_CLICKED, OnBnClickedCheckFilter)
		COMMAND_HANDLER(IDC_CHECK_INFORMATION, BN_CLICKED, OnBnClickedCheckFilter)

		MSG_WM_CONTEXTMENU(OnContextMenu)

		CHAIN_MSG_MAP(WTL::CDialogResize<thisClass>)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(thisClass)
		DLGRESIZE_CONTROL(IDC_EDIT_EXCEPTION, DLSZ_SIZE_X | DLSZ_SIZE_Y)
		DLGRESIZE_CONTROL(IDC_CHECK_ERRORS, DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDC_CHECK_WARNINGS, DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDC_CHECK_INFORMATION, DLSZ_MOVE_Y)
	END_DLGRESIZE_MAP()

	BEGIN_DDX_MAP(thisClass)
		DDX_TEXT(IDC_EDIT_EXCEPTION, m_exception)
		DDX_CHECK(IDC_CHECK_ERRORS, m_checkErrors)
		DDX_CHECK(IDC_CHECK_WARNINGS, m_checkWarnings)
		DDX_CHECK(IDC_CHECK_INFORMATION, m_checkInformation)
	END_DDX_MAP()

	LRESULT OnInitDialog(HWND /*hWnd*/, LPARAM /*lParam*/);
	LRESULT OnInitialize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);

	LRESULT OnBnClickedCheckFilter(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	void OnContextMenu(HWND /*hWnd*/, CPoint pt);

	//CTabPane
	virtual HWND GetHWND();
	bool CanCopy();
	bool HasResults() const;

	//ISearchable
	bool CanBeSearched() const;
	bool Search(const CSearchableInfo &/*info*/, CSearchableResult &/*result*/) const;
	void UpdateDisplay();

	//  --  IResultView  ---
	HWND GetHwnd() const; 
};
