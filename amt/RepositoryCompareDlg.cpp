#include "StdAfx.h"
#include "..\en_us\resource.h"

#include "RepositoryCompareDlg.h"
#include "MainFrm2.h"

CRepositoryCompareDlg::CRepositoryCompareDlg()
{
	m_repositoryView = new CRepositoryCompareView();
}

CRepositoryCompareDlg::~CRepositoryCompareDlg()
{
}

BOOL CRepositoryCompareDlg::PreTranslateMessage(MSG* pMsg)
{
	return CWindow::IsDialogMessage(pMsg);
}

void CRepositoryCompareDlg::SetStaticText(WTL::CStatic & staticCtrl, const std::_tstring & text)
{
	staticCtrl.SetWindowText(text.c_str());
	WTL::CDCHandle dc = GetDC();
	CRect rc;
	dc.DrawText(text.c_str(), -1, rc, DT_CALCRECT);

	UINT flags = SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSENDCHANGING | SWP_NOCOPYBITS;
	CRect rcStatic;
	staticCtrl.GetWindowRect(rcStatic);
	ScreenToClient(rcStatic);
	rcStatic.right = rcStatic.left + rc.Width();
	staticCtrl.SetWindowPos(NULL, rcStatic, flags);
}

void CRepositoryCompareDlg::Init(const std::_tstring & lhsTitle, const std::_tstring & rhsTitle, IRepository * repLHS, IRepository * repRHS, const STRUCT_DALI & dali)
{
	m_repositoryView->Init(repLHS, repRHS, dali);
	SetStaticText(m_staticLHS, lhsTitle);
	SetStaticText(m_staticRHS, rhsTitle);

	PositionCombo(m_staticLHS, m_comboLHS);
	PositionCombo(m_staticRHS, m_comboRHS);

	m_comboLHS->Load(repLHS, _T(""));
	m_comboLHS->m_value = _T("");
	m_comboLHS->SelectDefault(m_comboLHS->m_value);

	m_comboRHS->Load(repRHS, _T(""));
	m_comboRHS->m_value = _T("");
	m_comboRHS->SelectDefault(m_comboRHS->m_value);
}

void CRepositoryCompareDlg::Kill()
{
	m_repositoryView->Kill();
}

void CRepositoryCompareDlg::DoCopy() const
{
	m_repositoryView->DoCopy();
}

void CRepositoryCompareDlg::DoPaste(const std::_tstring & attributes)
{
	m_repositoryView->DoPaste(attributes);
}

void CRepositoryCompareDlg::Load(CTreeNode * item, bool noRefresh)
{
	m_repositoryView->Load(item, noRefresh);
}

void CRepositoryCompareDlg::Clear()
{
	m_repositoryView->Clear();
}

void CRepositoryCompareDlg::SelectAll()
{
	m_repositoryView->SelectAll();
}

void CRepositoryCompareDlg::ClearSelection()
{
	m_repositoryView->ClearSelection();
}

void CRepositoryCompareDlg::ExpandAll()
{
	m_repositoryView->ExpandAll();
}

bool CRepositoryCompareDlg::SetFilter(int filter)
{
	return m_repositoryView->SetFilter(filter);
}

void CRepositoryCompareDlg::SupressModuleChecksum(bool supressModuleChecksum)
{
	m_repositoryView->SupressModuleChecksum(supressModuleChecksum);
}

bool CRepositoryCompareDlg::DoMigration()
{
	return m_repositoryView->DoMigration();
}

int CRepositoryCompareDlg::GetMigrationCount() const
{
	return m_repositoryView->GetMigrationCount();
}

CTreePairNode * CRepositoryCompareDlg::GetSelectedItem() const
{
	return (CTreePairNode *)m_repositoryView->GetSelectedItem().GetData();
}

void CRepositoryCompareDlg::UpdateUI(WTL::CUpdateUIBase * pUIBase)
{
	pUIBase->UIEnable(ID_EDIT_COPY, m_repositoryView->CanCopy());
	pUIBase->UIEnable(ID_EDIT_PASTE, m_repositoryView->CanPaste());
	pUIBase->UIEnable(ID_EDIT_ADDTOREP, m_comboRHS->m_value.IsEmpty() && m_repositoryView->CanCopy());
}

BOOL CRepositoryCompareDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	DlgResize_Init(false, false);
	m_staticLHS = GetDlgItem(IDC_STATIC_LHS);
	m_comboLHS = new CComboLabel();
	m_comboLHS->Attach(GetDlgItem(IDC_COMBO_LHS));
	m_comboLHS->SetFont(WTL::AtlGetDefaultGuiFont());

	m_staticRHS = GetDlgItem(IDC_STATIC_RHS);
	m_comboRHS = new CComboLabel();
	m_comboRHS->Attach(GetDlgItem(IDC_COMBO_RHS));
	m_comboRHS->SetFont(WTL::AtlGetDefaultGuiFont());

	m_repositoryView->SubclassWindow(GetDlgItem(IDC_TREE_COMPARE));
	m_repositoryView->ModifyStyle(TVS_HASLINES, TVS_FULLROWSELECT);
	m_repositoryView->ModifyStyleEx(WS_BORDER, WS_EX_CLIENTEDGE);
#if (_WIN32_WINNT >= 0x0600)
	m_repositoryView->ModifyStyleEx(0, TVS_EX_DOUBLEBUFFER);
#endif
	m_repositoryView->SetFont(WTL::AtlGetDefaultGuiFont());
	m_repositoryView->PostMessage(UM_INITIALIZE);

	return 0;
}

void CRepositoryCompareDlg::PositionCombo(WTL::CStatic & staticCtrl, CComboLabel * comboCtrl)
{
	UINT flags = SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSENDCHANGING;
	CRect rcStatic;
	staticCtrl.GetWindowRect(rcStatic);
	ScreenToClient(rcStatic);

	CRect rcCombo;
	comboCtrl->GetWindowRect(rcCombo);
	ScreenToClient(rcCombo);
	rcCombo.MoveToX(rcStatic.right + 2);
	comboCtrl->SetWindowPos(NULL, rcCombo, flags);
}

void CRepositoryCompareDlg::OnSize(UINT nType, CSize size)
{
	SetMsgHandled(false);
	if( nType != SIZE_MINIMIZED )
	{
		UINT flags = SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSENDCHANGING;
		CRect rcStatic;
		m_staticRHS.GetWindowRect(rcStatic);
		ScreenToClient(rcStatic);
		rcStatic.MoveToX(size.cx / 2);
		m_staticRHS.SetWindowPos(NULL, rcStatic, flags);

		PositionCombo(m_staticLHS, m_comboLHS);
		PositionCombo(m_staticRHS, m_comboRHS);
	}
}

void CRepositoryCompareDlg::OnEditCopy(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
{
	m_repositoryView->DoCopy();
}

void CRepositoryCompareDlg::OnEditPaste(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
{
	std::_tstring attrs;
	GetClipboardText(attrs);
	m_repositoryView->DoPaste(attrs);
}

LRESULT CRepositoryCompareDlg::OnCbnSelendokComboLHS(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_comboLHS->GetLBText(m_comboLHS->GetCurSel(), m_comboLHS->m_value);
	m_repositoryView->m_repLHS->SetRepositoryLabel(static_cast<const TCHAR *>(m_comboLHS->m_value));
	PostFrameMessage(UM_CLEAR_DIFF);
	m_repositoryView->Clear();
	m_repositoryView->ClearSelection();
	m_repositoryView->Load(NULL, false);
	return 0;
}

LRESULT CRepositoryCompareDlg::OnCbnSelendokComboRHS(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_comboRHS->GetLBText(m_comboRHS->GetCurSel(), m_comboRHS->m_value);
	m_repositoryView->m_repRHS->SetRepositoryLabel(static_cast<const TCHAR *>(m_comboRHS->m_value));
	PostFrameMessage(UM_CLEAR_DIFF);
	m_repositoryView->Clear();
	m_repositoryView->ClearSelection();
	m_repositoryView->Load(NULL, false);
	return 0;
}
