#pragma once

#include "RepositoryCompareView.h"
#include "CtlColor.h"

//  ===========================================================================
class CRepositoryCompareDlg : 
	public WTL::CDialogImpl<CRepositoryCompareDlg>, 	
	public WTL::CDialogResize<CRepositoryCompareDlg>,
	public CCtlColor
{
	typedef CRepositoryCompareDlg thisClass;
	typedef WTL::CDialogImpl<CRepositoryCompareDlg> baseClass;

public:
	WTL::CStatic m_staticLHS;
	CComPtr<CComboLabel> m_comboLHS;

	WTL::CStatic m_staticRHS;
	CComPtr<CComboLabel> m_comboRHS;

	StlLinked<CRepositoryCompareView> m_repositoryView;
public:
	enum { IDD = IDD_AMT_COMPARE };
	CRepositoryCompareDlg();
	~CRepositoryCompareDlg();

	BOOL PreTranslateMessage(MSG* pMsg);

	void SetStaticText(WTL::CStatic & staticCtrl, const std::_tstring & text);
	void Init(const std::_tstring & lhsTitle, const std::_tstring & rhsTitle, IRepository * repLHS, IRepository * repRHS, const STRUCT_DALI & dali);
	void Kill();
	void DoCopy() const;
	void DoPaste(const std::_tstring & attributes);
	void Load(CTreeNode * item, bool noRefresh);
	void Clear();
	void SelectAll();
	void ClearSelection();
	void ExpandAll();
	bool SetFilter(int filter = AMT_SHOW_MATCHING & AMT_SHOW_NOT_MATCHING & AMT_SHOW_ORPHANS);
	bool DoMigration();
	int GetMigrationCount() const;
	CTreePairNode * GetSelectedItem() const;
	void UpdateUI(WTL::CUpdateUIBase * pUIBase);

	BEGIN_MSG_MAP(thisClass)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_SIZE(OnSize)

		COMMAND_ID_HANDLER_EX(ID_EDIT_COPY, OnEditCopy)
		COMMAND_ID_HANDLER_EX(ID_EDIT_PASTE, OnEditPaste)

		COMMAND_HANDLER(IDC_COMBO_LHS, CBN_SELENDOK, OnCbnSelendokComboLHS)
		COMMAND_HANDLER(IDC_COMBO_RHS, CBN_SELENDOK, OnCbnSelendokComboRHS)

		REFLECT_NOTIFICATIONS_HWND_FILTERED(*m_repositoryView)
		CHAIN_MSG_MAP(CCtlColor)
		CHAIN_MSG_MAP(CDialogResize<thisClass>)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(thisClass)
		DLGRESIZE_CONTROL(IDC_TREE_COMPARE, DLSZ_SIZE_X | DLSZ_SIZE_Y)
	END_DLGRESIZE_MAP()

	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void PositionCombo(WTL::CStatic & staticCtrl, CComboLabel * comboCtrl);
	void OnSize(UINT nType, CSize size);
	void OnEditCopy(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/);
	void OnEditPaste(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/);
	LRESULT OnCbnSelendokComboLHS(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCbnSelendokComboRHS(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};
