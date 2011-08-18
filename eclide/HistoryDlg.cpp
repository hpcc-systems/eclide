#include "stdafx.h"
#include "resource.h"

#include "historydlg.h"
#include "attribute.h"
#include "eclview.h"
#include "vdiff.h"

class CCompareDlg : public CDialogImpl<CCompareDlg>, 
	public CDialogResize<CCompareDlg>,
	public CWinDataExchange<CCompareDlg>
{
	typedef CCompareDlg thisClass;
	typedef CDialogImpl<thisClass> baseClass;
	typedef CDialogResize<thisClass> baseClassResize;
	typedef CDialogResize<thisClass> baseClassDDX;

protected:
	CListViewCtrl m_HistoryList;
	CAxWindow m_wndIE;
	CComQIPtr<IWebBrowser2> m_pWB2;
	CEclView m_ecl;
	CAtlTemporaryFile m_DiffPath;
	CAtlTemporaryFile m_LeftPath;
	CAtlTemporaryFile m_RightPath;
	CComPtr<IAttribute> m_Attribute;
	IAttributeHistoryVector m_History;
	int m_HistorySel;
	int m_SelCount;
	int m_Sel1;
	int m_Sel2;
	bool m_InitSingleSelect;

public:
	CCompareDlg(IAttributeHistory * current, IAttribute * attribute, bool selectSingle) : m_Attribute(attribute), m_InitSingleSelect(selectSingle)
	{
		if (current)
			m_History.push_back(current);
		CCompareDlgConstruct();
	}

	CCompareDlg(IAttribute * attribute) : m_Attribute(attribute)
	{
		CCompareDlgConstruct();
		m_InitSingleSelect = true;
	}

	void CCompareDlgConstruct()
	{
		m_Attribute->GetHistory(m_History);
		m_DiffPath.Create();
		m_LeftPath.Create();
		m_RightPath.Create();
		m_DiffPath.HandsOff();
		m_LeftPath.HandsOff();
		m_RightPath.HandsOff();
		m_SelCount = 0;
	}

	~CCompareDlg()
	{
		m_DiffPath.HandsOn();
		m_LeftPath.HandsOn();
		m_RightPath.HandsOn();
	}

	enum { IDD = IDD_DIALOG_DIFF };

	BEGIN_MSG_MAP(thisClass)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER_EX(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCloseCmd)

		NOTIFY_HANDLER(IDC_LIST_HISTORY, LVN_ITEMCHANGED, OnLvnItemchangedListHistory)
		MESSAGE_HANDLER_EX(CWM_REFRESH, OnRefresh)
		CHAIN_MSG_MAP(baseClassDDX)
		CHAIN_MSG_MAP(baseClassResize)
	END_MSG_MAP()

	BEGIN_DDX_MAP(thisClass)
	END_DDX_MAP()

	BEGIN_DLGRESIZE_MAP(thisClass)
		DLGRESIZE_CONTROL(IDC_LIST_HISTORY, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_IE_PLACEHOLDER, DLSZ_SIZE_X | DLSZ_SIZE_Y)
		DLGRESIZE_CONTROL(IDC_ECL_PLACEHOLDER, DLSZ_SIZE_X | DLSZ_SIZE_Y)
        DLGRESIZE_CONTROL(IDOK, DLSZ_MOVE_X | DLSZ_MOVE_Y)
    END_DLGRESIZE_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	void initHistoryList();
	void initIEView();
	void initECLView();
	void OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/);
	LRESULT OnRefresh(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);

	void SetUrl(const CString & url);
	void DoRefresh();
	void DoShowSingle();
	void DoShowDouble();
	void DoShowNone();
	LRESULT OnLvnItemchangedListHistory(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);
};

LRESULT CCompareDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CenterWindow ( GetParent() );

	DlgResize_Init();

	initHistoryList();
	initIEView();
	initECLView();

	DoDataExchange();

	return TRUE;
}

void CCompareDlg::initHistoryList()
{
	int i = 0;
	m_HistoryList = GetDlgItem(IDC_LIST_HISTORY);
	RECT r;
	m_HistoryList.GetWindowRect(&r);
	m_HistoryList.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
	m_HistoryList.AddColumn(_T("Date"), 0);
	m_HistoryList.AddColumn(_T("User"), 1);
	m_HistoryList.AddColumn(_T("Comment"), 2);
	int width = r.right - r.left;
	m_HistoryList.SetColumnWidth(0, width / 4);
	m_HistoryList.SetColumnWidth(1, width / 4);
	m_HistoryList.SetColumnWidth(2, width / 2);

	for(IAttributeHistoryVector::iterator itr = m_History.begin(); itr != m_History.end(); ++itr)
	{
		m_HistoryList.AddItem(i, 0, itr->get()->GetModifiedDate());
		m_HistoryList.AddItem(i, 1, itr->get()->GetModifiedBy());
		m_HistoryList.AddItem(i, 2, itr->get()->GetDescription());
		++i;
	}

	m_HistoryList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
	if (!m_InitSingleSelect)
		m_HistoryList.SetItemState(1, LVIS_SELECTED, LVIS_SELECTED);
}

void CCompareDlg::initIEView()
{
	CWindow wndPlaceholder = GetDlgItem ( IDC_IE_PLACEHOLDER );
	CRect rc;

	// Get the rect of the placeholder group box, then destroy that window 
	// because we don't need it anymore.
	wndPlaceholder.GetWindowRect ( rc );
	ScreenToClient ( rc );
	wndPlaceholder.DestroyWindow();

	CComPtr<IUnknown> punkCtrl;

	// Create the AX host window.
	m_wndIE.Create ( *this, rc, _T(""), WS_CHILD | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE, IDC_IE_PLACEHOLDER );

	// Create the browser control using its GUID.
	m_wndIE.CreateControlEx ( L"{8856F961-340A-11D0-A96B-00C04FD705A2}", NULL, NULL, &punkCtrl );

	/*
	NOTE: You could also use the control's ProgID: Shell.Explorer:
	m_wndIE.CreateControlEx ( L"Shell.Explorer", NULL,
	NULL, &punkCtrl );
	*/

	// Get an IWebBrowser2 interface on the control and navigate to a page.
	m_pWB2 = punkCtrl;
}

void CCompareDlg::initECLView()
{
	CWindow wndPlaceholder = GetDlgItem(IDC_ECL_PLACEHOLDER);
	CRect rc;
	wndPlaceholder.GetWindowRect ( rc );
	ScreenToClient(rc);
	wndPlaceholder.DestroyWindow();
	m_ecl.Create(*this, rc, _T(""), WS_CHILD | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE, IDC_ECL_PLACEHOLDER);
	m_ecl.DoInit();
	m_ecl.SetReadOnly(true);
}

void CCompareDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	EndDialog(wID);
}

LRESULT CCompareDlg::OnRefresh(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	DoRefresh();
	return 0;
}

void CCompareDlg::SetUrl(const CString & url)
{
	CComVariant v;
	ATLASSERT(m_pWB2);
	m_pWB2->Navigate(CComBSTR(CT2W(url, CP_UTF8)), &v, &v, &v, &v);
}

void GetHeader(IAttributeHistory * attr, CString & header)
{
	header = (boost::_tformat(_T("%1% (%2%)\n%3%\n")) % attr->GetModifiedDate() % attr->GetModifiedBy() % attr->GetDescription()).str().c_str();
}

void CCompareDlg::DoRefresh()
{
	int currentCount = m_HistoryList.GetSelectedCount();
	if (currentCount != m_SelCount)
	{
		m_SelCount = currentCount;
		DoDataExchange(true);
		switch (m_HistoryList.GetSelectedCount())
		{
		case 1:
			DoShowSingle();
			break;
		case 2:
			DoShowDouble();
			break;
		default:
			DoShowDouble();
			break;
		}
	}

	int sel1 = -1, sel2 = -1;
	sel1 = m_HistoryList.GetNextItem(sel1, LVNI_SELECTED);
	sel2 = m_HistoryList.GetNextItem(sel1, LVNI_SELECTED);
	if (m_Sel1 != sel1 || m_Sel2 != sel2)
	{
		m_Sel1 = sel1;
		m_Sel2 = sel2;
		if (m_Sel1 != -1 && m_Sel2 != -1)
		{
			CString left, leftHeader, right, rightHeader;
			CWaitCursor();
			GetHeader(m_History[m_Sel1].get(), leftHeader);
			left = m_History[m_Sel1]->GetEcl();
			GetHeader(m_History[m_Sel2].get(), rightHeader);
			right = m_History[m_Sel2]->GetEcl();
			Compare(leftHeader, left, rightHeader, right, m_DiffPath.TempFileName(), m_LeftPath.TempFileName(), m_RightPath.TempFileName());
			SetUrl(m_DiffPath.TempFileName());
		}
		else if (m_Sel1 != -1)
		{
			CString txtHeader;
			CWaitCursor();
			GetHeader(m_History[m_Sel1].get(), txtHeader);
			CString txt = "/*";
			txt += txtHeader + _T("*/\r\n");
			txt += m_History[m_Sel1]->GetEcl();
			m_ecl.SetReadOnly(false);
			m_ecl.SetText(txt);  //TODO:  Retest
			m_ecl.SetReadOnly(true);
		}
	}
}

void CCompareDlg::DoShowSingle()
{
	m_pWB2->put_Visible(false);
	m_wndIE.ShowWindow(SW_HIDE);
	m_ecl.ShowWindow(SW_SHOW);
}
void CCompareDlg::DoShowDouble()
{
	m_pWB2->put_Visible(true);
	m_wndIE.ShowWindow(SW_SHOW);
	m_ecl.ShowWindow(SW_HIDE);
}
void CCompareDlg::DoShowNone()
{
	m_pWB2->put_Visible(false);
	m_wndIE.ShowWindow(SW_HIDE);
	m_ecl.ShowWindow(SW_HIDE);
}

LRESULT CCompareDlg::OnLvnItemchangedListHistory(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if (pNMLV->uChanged & LVIF_STATE)
		PostMessage(CWM_REFRESH);

	return 0;
}

void ShowHistory(IAttribute * attr)
{
	CCompareDlg dlg(attr);
	dlg.DoModal();
}

void ShowHistory(IAttributeHistory * current, IAttribute * attr)
{
	CCompareDlg dlg(current, attr, true);
	dlg.DoModal();
}

void ShowCompare(IAttributeHistory * current, IAttribute * attr)
{
	CCompareDlg dlg(current, attr, false);
	dlg.DoModal();
}
