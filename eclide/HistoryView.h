#pragma once

#include "attribute.h"

#include <DiffView.h>
#include <ControlFocus.h>
#include "EclCommand.h"
#include "Repository.h"
#include "CtlColor.h"

__interface IHistorySlot
{
	bool OnHistoryContext(WTL::CMenu &menu, CPoint &pt);
	void SetAttributeText(IAttribute *attr, const CString &ecl, bool resetSavePoint, bool clearUndo);
	void OnRefresh();
};

class CHistoryView : 
	public WTL::CDialogImpl<CHistoryView>, 
	public WTL::CDialogResize<CHistoryView>,
	public WTL::CWinDataExchange<CHistoryView>,
	public CSourceSlotImpl,
	public CEclCommandMixin<CHistoryView>,
	public CCtlColor
{
	typedef CHistoryView thisClass;
	typedef WTL::CDialogImpl<thisClass> baseClass;
	typedef WTL::CDialogResize<thisClass> baseClassResize;
	typedef CEclCommandMixin<CHistoryView> baseClassEclCmd;

protected:
	IHistorySlot *m_owner;
	WTL::CListViewCtrl m_HistoryList;
	CWindowControlFocusEx m_HistoryListFocus;
	CSourceCtrl m_ecl;
	WTL::CEdit m_editLHS;
	WTL::CEdit m_editRHS;
	CDiffView m_diffView;
	CAtlTemporaryFile m_DiffPath;
	CAtlTemporaryFile m_LeftPath;
	CAtlTemporaryFile m_RightPath;
	CComPtr<IAttribute> m_Attribute;
	IAttributeHistoryVector m_History;
	int m_HistorySel;
	int m_SelCount;
	int m_Sel1;
	int m_Sel2;
	CString m_eclStr;
	CComPtr<IAttributeHistory> m_Current;
	CComPtr<IAttributeHistory> m_Sandbox;
	bool m_CompareMode;
	bool m_FocusOnEcl;
	bool m_FocusOnDiff;
	std::_tstring m_cluster;
	CRepLabelVector m_labels;
	int m_loadedVersion;

public:
	CHistoryView(IHistorySlot *owner);
	~CHistoryView();

	void SetCurrent(const CString & curr);
	void SetAttribute(const CString & mod, const CString & attr, IAttributeType * type);
	void SetAttribute(IAttribute * attribute, const TCHAR *currEcl=0);
	void SetCompareMode(bool compareMode);
	IAttribute * GetAttribute();
	const TCHAR * GetCluster();
	void GetText(CString & txt);
	bool CanExecute();
	int GetDiffPrevious();
	int GetDiffNext();

	enum { IDD = IDD_DIALOG_DIFF };

	BOOL PreTranslateMessage(MSG* pMsg);

	#define COMMAND_MAP_ID 9

	BEGIN_MSG_MAP(thisClass)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		NOTIFY_HANDLER(IDC_LIST_HISTORY, LVN_ITEMCHANGED, OnLvnItemchangedListHistory)
		MESSAGE_HANDLER_EX(CWM_REFRESH, OnRefresh)
		MSG_WM_CONTEXTMENU(OnContextMenu)
		MSG_WM_SETFOCUS(OnSetFocus)
		MSG_WM_KILLFOCUS(OnKillFocus)
		MSG_WM_SIZE(OnSize)
		CHAIN_MSG_MAP(baseClassResize)
		if ( m_FocusOnEcl )
			CHAIN_MSG_MAP_ALT(baseClassEclCmd, CEclCommandMixinMAP_ID)
		else
			CHAIN_MSG_MAP_ALT(thisClass, COMMAND_MAP_ID)

		CHAIN_MSG_MAP(CCtlColor)
		//put generic command handlers in the alt map
	ALT_MSG_MAP(COMMAND_MAP_ID)
		COMMAND_ID_HANDLER_EX(ID_HISTORY_ROLLBACK, OnHistoryRollback)
		COMMAND_ID_HANDLER_EX(ID_HISTORY_LABEL, OnHistoryLabel)
		COMMAND_ID_HANDLER_EX(ID_DIFFERENCES_PREVIOUS, OnDifferencePrevious)
		COMMAND_ID_HANDLER_EX(ID_DIFFERENCES_NEXT, OnDifferenceNext)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(thisClass)
		DLGRESIZE_CONTROL(IDC_LIST_HISTORY, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_IE_PLACEHOLDER, DLSZ_SIZE_X | DLSZ_SIZE_Y)
		DLGRESIZE_CONTROL(IDC_ECL_PLACEHOLDER, DLSZ_SIZE_X | DLSZ_SIZE_Y)
    END_DLGRESIZE_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLvnItemchangedListHistory(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);
	void OnContextMenu(HWND phWnd, CPoint pPoint);
	void OnHistoryRollback(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/);
	void OnHistoryLabel(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/);
	void OnDifferencePrevious(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/);
	void OnDifferenceNext(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/);

	void OnSetFocus(HWND other);
	void OnKillFocus(HWND other);
	void OnSize(UINT nType, CSize size);

	void initHistoryList();
	void initIEView();
	void initECLView();
	LRESULT OnRefresh(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);

	void SetUrl(const CString & url);
	void DoRefresh();
	void DoShowSingle();
	void DoShowDouble();
	void DoShowNone();

	void GetTitle(CString & title);
	bool IsDirty();

	bool UIUpdateMenuItems(CCmdUI * cui);
	void UIUpdateTitle();
	//ISourceSlot
	void PostStatus(const TCHAR *status);
};

