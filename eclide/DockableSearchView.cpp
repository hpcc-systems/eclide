#include "stdafx.h"
#include "..\en_us\resource.h"

#include "DockableSearchView.h"
#include <CustomMessages.h>

// CDockableSearchView dialog

IMPLEMENT_DYNAMIC(CDockableSearchView, CPaneDialog)

CDockableSearchView::CDockableSearchView()
{
	m_owner = NULL;
	m_searchConditions = NULL;
}

CDockableSearchView::~CDockableSearchView()
{
	ATLASSERT(!IsWindow(GetSafeHwnd()));
}

void CDockableSearchView::SetOwner(IGraphViewSlot * owner)
{
	if (m_owner != owner)
	{
		m_owner = owner;
		if (m_owner)
		{
			m_searchConditions = m_owner->GetGlobalSearchConditions();
			UpdateData(false);
			m_listResults.DeleteAllItems();
			const roxie::CSearchMap & results = m_owner->GetSearchResults();
			LoadResults(results);
		}
	}
}

void CDockableSearchView::Send_Clear()
{
	m_listResults.DeleteAllItems();
}

void CDockableSearchView::Post_Reset()
{
	Send_Clear();
}

int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CMFCListCtrl * list = (CMFCListCtrl *)lParamSort;
	std::_tstring lhs = list->GetItemText(lParam1, 0);
	std::_tstring rhs = list->GetItemText(lParam2, 0);
	typedef std::vector<std::_tstring> split_vector_type;
	split_vector_type lhsSplit, rhsSplit; 
	boost::algorithm::split(lhsSplit, lhs, boost::algorithm::is_any_of(_T("_")));
	boost::algorithm::split(rhsSplit, rhs, boost::algorithm::is_any_of(_T("_")));
	try
	{
		int lhsInt = boost::lexical_cast<int>(lhsSplit[0]);
		int rhsInt = boost::lexical_cast<int>(rhsSplit[0]);
		if (lhsInt == rhsInt)
		{
			lhsInt = 0;
			rhsInt = 0;
			if (lhsSplit.size() > 1)
				lhsInt = boost::lexical_cast<int>(lhsSplit[1]);
			if (rhsSplit.size() > 1)
				rhsInt = boost::lexical_cast<int>(rhsSplit[1]);
		}
		if (lhsInt < rhsInt)
			return -1;
		else if (lhsInt > rhsInt)
			return 1;
	}
	catch(boost::bad_lexical_cast &) 
	{
	}
	return 0;
}

void CDockableSearchView::LoadResults(const roxie::CSearchMap & results)
{
	m_listResults.DeleteAllItems();
	unsigned int i = 0;
	for(roxie::CSearchMap::const_iterator itr = results.begin(); itr != results.end(); ++itr)
	{
		m_listResults.InsertItem(i, itr->first.GetID());
		m_listResults.SetItemData(i, (DWORD_PTR)&itr->first);
		++i;
	}
	m_listResults.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
	m_listResults.SortItemsEx(&CompareFunc, (DWORD_PTR)&m_listResults);
}

void CDockableSearchView::DoDataExchange(CDataExchange* pDX)
{
	CPaneDialog::DoDataExchange(pDX);
	if (m_searchConditions)
	{
		DDX_Text(pDX, IDC_EDIT_FIELD, m_searchConditions->m_field);
		DDX_CBString(pDX, IDC_COMBO_CONDITION, m_searchConditions->m_condition);
		DDX_Text(pDX, IDC_EDIT_VALUE, m_searchConditions->m_value);
		DDX_Check(pDX, IDC_CHECK_CASESENSITIVE, m_searchConditions->m_caseSensitive);
	}
}

void CDockableSearchView::AdjustWidth(int cx, UINT uiCtrlID, int nMargin)
{
	CWnd* pWnd = GetDlgItem(uiCtrlID);
	if (pWnd == NULL)
	{
		return;
	}

	CRect rect;
	pWnd->GetWindowRect(rect);
	ScreenToClient(rect);

	rect.right = cx - nMargin;

	pWnd->SetWindowPos(NULL, -1, -1, rect.Width(), rect.Height(), SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
}

void CDockableSearchView::AdjustHeight(int cy, UINT uiCtrlID, int nMargin)
{
	CWnd* pWnd = GetDlgItem(uiCtrlID);
	if (pWnd == NULL)
	{
		return;
	}

	CRect rect;
	pWnd->GetWindowRect(rect);
	ScreenToClient(rect);

	rect.bottom = cy - nMargin;

	pWnd->SetWindowPos(NULL, -1, -1, rect.Width(), rect.Height(), SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
}

enum UM
{
	UM_FIRST = CWM_LAST + 1,
	UM_INITIALIZE,
	UM_LAST
};

BEGIN_MESSAGE_MAP(CDockableSearchView, CPaneDialog)
	ON_WM_CREATE()
	ON_MESSAGE(UM_INITIALIZE, OnInitialize)
	ON_WM_DESTROY()

	ON_WM_SIZE()
	//ON_WM_ERASEBKGND()
	//ON_WM_CTLCOLOR()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_RESULTS, &CDockableSearchView::OnLvnItemchangedListResults)
END_MESSAGE_MAP()

int CDockableSearchView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CPaneDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	PostMessage(UM_INITIALIZE);

	return 0;
}

LRESULT CDockableSearchView::OnInitialize(WPARAM wParam, LPARAM lParam)
{
	CComboBox * combo = (CComboBox *)GetDlgItem(IDC_COMBO_CONDITION);
	combo->LimitText(256);
	combo->AddString(_T("Contains"));
	combo->AddString(_T("Equals"));
	combo->AddString(_T("StartsWith"));
	//combo->AddString(_T("EndsWith"));
	combo->AddString(_T("<"));
	combo->AddString(_T("<="));
	combo->AddString(_T(">"));
	combo->AddString(_T(">="));
	combo->AddString(_T("!="));
	combo->AddString(_T("eog"));
	combo->AddString(_T("eof"));
	combo->SelectString(0, _T("Equals"));

	m_listResults.Attach(GetDlgItem(IDC_LIST_RESULTS)->GetSafeHwnd());
	m_listResults.InsertColumn(0, _T("Edge ID"));
	return 0;
}

void CDockableSearchView::OnDestroy()
{
	m_listResults.Detach();
	CPaneDialog::OnDestroy();
}

void CDockableSearchView::OnSize(UINT nType, int cx, int cy)
{
	CPaneDialog::OnSize(nType, cx, cy);

	AdjustWidth(cx, IDC_EDIT_FIELD);
	AdjustWidth(cx, IDC_COMBO_CONDITION);
	AdjustWidth(cx, IDC_EDIT_VALUE);
	AdjustWidth(cx, IDC_LIST_RESULTS, 0);
	AdjustHeight(cy, IDC_LIST_RESULTS, 0);
}

BOOL CDockableSearchView::OnEraseBkgnd(CDC* pDC)
{
	CRect rectClient;
	GetClientRect(rectClient);

	CMFCVisualManager::GetInstance()->OnFillBarBackground(pDC, this, rectClient, rectClient);

	return TRUE;
}

LRESULT CDockableSearchView::OnCtlColor2(WPARAM wParam, LPARAM lParam)
{
	HDC hDC = (HDC)wParam;
	return (LRESULT)(HBRUSH)afxGlobalData.brBarFace;
}

LRESULT CDockableSearchView::OnCtlColor3(WPARAM wParam, LPARAM lParam)
{
	HDC hDC = (HDC)wParam;
	SetBkMode(hDC,TRANSPARENT);
	SetTextColor(hDC, afxGlobalData.clrBarText);
	return (LRESULT)(HBRUSH)::GetStockObject(HOLLOW_BRUSH);
}

HBRUSH CDockableSearchView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
#define MAX_CLASS_NAME 255
#define STATIC_CLASS _T("Static")
#define BUTTON_CLASS _T("Button")
#define CHECKBOX_CLASS _T("Check")

	if (nCtlColor == CTLCOLOR_STATIC || nCtlColor == CTLCOLOR_BTN)
	{
		TCHAR lpszClassName [MAX_CLASS_NAME + 1];

		::GetClassName(pWnd->GetSafeHwnd(), lpszClassName, MAX_CLASS_NAME);
		::CString strClass = lpszClassName;

		if (strClass == STATIC_CLASS || strClass == BUTTON_CLASS || strClass == CHECKBOX_CLASS)
		{

			//CMFCVisualManager::GetInstance()->OnFillBarBackground(pDC, this, rectClient, rectClient);
			pDC->SetTextColor(afxGlobalData.clrBarText);
			pDC->SetBkMode(TRANSPARENT);
			return(HBRUSH) ::GetStockObject(HOLLOW_BRUSH);
		}
	}

	return CPaneDialog::OnCtlColor(pDC, pWnd, nCtlColor);
}

// CDockableSearchView message handlers

void CDockableSearchView::OnLvnItemchangedListResults(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if ((pNMLV->uChanged & LVIF_STATE) && (pNMLV->uNewState & LVIS_SELECTED))
		m_owner->NewSelection(NULL, *(CUniqueID *)m_listResults.GetItemData(pNMLV->iItem), true, false);
	*pResult = 0;
}
