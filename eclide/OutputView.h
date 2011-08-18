#pragma once

#include <clib.h> //CWM_*
#include <EditLog.h>
#include <EditLog_Stream.h>
#include <CustomMessages.h>


class CMyEdit : public WTL::CWindowImpl<CMyEdit, WTL::CEdit>, public WTL::CEditCommands<CMyEdit>
{
public:
	BEGIN_MSG_MAP(CMyEdit)
		MSG_WM_CONTEXTMENU(OnContextMenu)

		CHAIN_MSG_MAP_ALT(CEditCommands<CMyEdit>, 1)	
	END_MSG_MAP()

	void OnContextMenu(HWND phWnd, CPoint pPoint);
};

class CStreamEditorView :
	public WTL::CDialogImpl<CStreamEditorView>,
	public WTL::CDialogResize<CStreamEditorView>
{
	typedef CStreamEditorView thisClass;
	typedef WTL::CDialogImpl<CStreamEditorView> baseClass;

protected:
	CMyEdit m_edit;
	CEditLog m_editLog;

	std::editstreambuf m_EditStrBuf;
	std::weditstreambuf m_EditStrBufW;
	std::basic_streambuf<char>* m_pOldBuf;
	std::basic_streambuf<wchar_t>* m_pOldBufW;

public:
	CStreamEditorView();
	~CStreamEditorView();

	enum { IDD = IDD_OUTPUT };

	BOOL PreTranslateMessage(MSG* pMsg);

	BEGIN_MSG_MAP(thisClass)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_CONTEXTMENU(OnContextMenu)
		MESSAGE_HANDLER_EX(CWM_CLEAR, OnClear)
		MESSAGE_HANDLER_EX(CWM_RESET, OnReset)

		CHAIN_MSG_MAP(WTL::CDialogResize<CStreamEditorView>)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(thisClass)
		DLGRESIZE_CONTROL(IDC_EDIT_OUTPUT, DLSZ_SIZE_X | DLSZ_SIZE_Y)
	END_DLGRESIZE_MAP()

	void AddText(const TCHAR* text, bool bClear=false);
	LRESULT OnInitDialog(HWND hWnd, LPARAM lParam);
	virtual void initStream() = 0;
	virtual void killStream() = 0;
	void OnClose();
	void OnContextMenu(HWND phWnd, CPoint pPoint);
	LRESULT OnClear(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
	LRESULT OnReset(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
};

class CErrorView : public CStreamEditorView
{
public:
	CErrorView();

	virtual void initStream();
	virtual void killStream();
};
