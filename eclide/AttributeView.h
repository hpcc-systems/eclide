// BuilderView.h : Declaration of the CAttributeView

#pragma once

//#include <atlhost.h>
#include "EclView.h"

class CAttributeView : public CDialogImpl<CAttributeView>,
					 public CDialogResize<CAttributeView>

{
	typedef CAttributeView thisClass;
	typedef CDialogImpl<CAttributeView> baseClass;

protected:

public:
	CEclView m_edit;

	enum { IDD = IDD_BUILDERVIEW };

	BOOL PreTranslateMessage(MSG* pMsg);

	BEGIN_MSG_MAP(thisClass)
		MSG_WM_INITDIALOG(OnInitDialog)
        MSG_WM_DESTROY(OnDestroy)
		MSG_WM_NOTIFY(OnNotify)
		//MESSAGE_HANDLER(OCM_NOTIFY, OnNotify)

		CHAIN_MSG_MAP(CDialogResize<CAttributeView>)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(thisClass)
		DLGRESIZE_CONTROL(IDC_EDIT_ECL, DLSZ_SIZE_X | DLSZ_SIZE_Y)
	END_DLGRESIZE_MAP()

// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	LRESULT OnInitDialog(HWND hWnd, LPARAM lParam)
	{
		SetMsgHandled(false);
		DlgResize_Init(false, true, 0);

		m_edit.Attach(GetDlgItem(IDC_EDIT_ECL));
		m_edit.DoInit();
		return 1;  // Let the system set the focus
	}

	void OnDestroy()
	{
		SetMsgHandled(false);
		m_edit.Detach();
	}

	LRESULT OnNotify(int wParam, LPNMHDR lParam)
	{
		return m_edit.HandleNotify((LPARAM)lParam);
	}
};


