#pragma once

#include "MultiTreeNode.h"
#include "Dfu.h"
#include "DfuTreeNode.h"
#include <Thread.h> //clib
#include "..\wlib\Combo.h"
#include "util.h"
#include "CtlColor.h"

enum DFUM
{
	DFUM_FIRST = CWM_LAST + 1,
	DFUM_REFRESHSTATE,
	DFUM_LAST
};

template<typename T>
class CDfuViewT
{
protected:
	IDfuSlot *m_Owner;
	CString m_label;
	CComPtr<CDfuNode> m_Root;

public:
	CMultiTreeNodeViewCtrl m_Tree;

	void SetRootLabel(const CString & label)
	{
		m_label = label;
	}

	void SelectAll(BOOL bSelect /*= TRUE*/)
	{
		m_Tree.SelectAll(bSelect);
	}

	UINT GetSelectedCount()
	{
		return m_Tree.GetSelectedCount();
	}

	bool GetSelected(CDfuSelections &s)
	{
		for(HTREEITEM pos = m_Tree.GetFirstSelectedItem(); pos != (HTREEITEM)0; pos = m_Tree.GetNextSelectedItem(pos))
		{
			WTL::CTreeItem item(pos, &m_Tree);

			DWORD_PTR data = item.GetData();
			if (CComQIPtr<CDfuNode> dfuNode = reinterpret_cast<CTreeNode*>(data))
			{
				s.dfuNodes.push_back(dfuNode.p);
				Dfu::IServerAdapt dfu = dfuNode->GetDfu();
				s.dfus.push_back(dfu);
			}
			else if (CComQIPtr<CFileNode> fileNode = reinterpret_cast<CTreeNode*>(data))
			{
				s.fileNodes.push_back(fileNode.p);
				CComPtr<Dfu::IFile> file = fileNode->GetFile();
				s.files.push_back(file.p);
			}
		}
		return true;
	}
	bool Select(const TCHAR* label, bool bExpand=false)
	{
		return TreeNode::Select(label, m_Root, bExpand);
	}
	void DoCopy(const Dfu::IFileVector & files, const std::_tstring & delim = _T(", "))
	{
		CString txt;

		for(Dfu::IFileVector::const_iterator itr = files.begin(); itr != files.end(); ++itr)
		{
			if (itr != files.begin())
				txt += delim.c_str();
			txt += CString(itr->get()->GetLabel());
		}

		SetClipboard((const TCHAR * )txt);
		return;
	}
	bool RefreshDfu()
	{
		T* pT = static_cast<T*>(this);
		WTL::CLockWindowUpdate lock(pT->m_hWnd);

		m_Tree.PushState();
		m_Root->Expand(TVE_COLLAPSE | TVE_COLLAPSERESET);
		m_Tree.PopState();
		return true;
	}
};
//  ===========================================================================
class CDfuView :
	public CDfuViewT<CDfuView>,
	public WTL::CDialogImpl<CDfuView>,
	public WTL::CDialogResize<CDfuView>,
	public WTL::CWinDataExchange<CDfuView>,
	public CCtlColor
{
	typedef CDfuView thisClass;
	typedef WTL::CDialogImpl<CDfuView> baseClass;

public:
	enum { IDD = IDD_DFU };

	CDfuView();
	~CDfuView();

	void Init(IDfuSlot * pOwner);
	BOOL PreTranslateMessage(MSG* pMsg);

	void DoClear();
	void DoReset();

	BEGIN_MSG_MAP(thisClass)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CONTEXTMENU(OnContextMenu)

		MESSAGE_HANDLER_EX(CWM_CLEAR, OnClear)
		MESSAGE_HANDLER_EX(CWM_RESET, OnReset)
		MESSAGE_HANDLER_EX(CWM_INITIALIZE, OnInitialize)

		MSG_WM_PAINT(OnPaint)

		//MESSAGE_HANDLER_EX(UM_TREESELECTIONUPDATE, OnSelectionUpdated)

		CHAIN_MSG_MAP(CCtlColor)
		REFLECT_NOTIFICATIONS()
		CHAIN_MSG_MAP(CDialogResize<thisClass>)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(thisClass)
		DLGRESIZE_CONTROL(IDC_TREE_DFU, DLSZ_SIZE_X | DLSZ_SIZE_Y)
	END_DLGRESIZE_MAP()

	BEGIN_DDX_MAP(thisClass)
	END_DDX_MAP()

	LRESULT OnInitDialog(HWND hWnd, LPARAM lParam);
	LRESULT OnClear(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
	LRESULT OnReset(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
	LRESULT OnInitialize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
	LRESULT OnSelectionUpdated(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
	void OnPaint(HDC hDc); 

	void OnRefresh(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/);
	void Refresh();

	HWND GetTreeHandle();
	void OnContextMenu(HWND phWnd, CPoint pPoint);

	CDfuNode * GetRootNode() { return m_Root; };

	virtual void OnItemSelectionChanged(CTreeNode *, bool bSelected );
	virtual void OnItemExpanding(CTreeNode *, bool bExpanding);
	virtual void HandleNmClick(LPNMTREEVIEW);
};
//  ===========================================================================
static const TCHAR DfuFind[] = _T("DfuFind");
static const SectionLabelDefault GLOBAL_DFUFIND_PREFIX(SectionLabel(DfuFind, _T("prefix")), _T(""));
static const SectionLabelDefault GLOBAL_DFUFIND_CLUSTER(SectionLabel(DfuFind, _T("cluster")), _T(""));
static const SectionLabelDefault GLOBAL_DFUFIND_LOGICALNAME(SectionLabel(DfuFind, _T("logicalname")), _T(""));
static const SectionLabelDefault GLOBAL_DFUFIND_DESCRIPTION(SectionLabel(DfuFind, _T("description")), _T(""));
static const SectionLabelDefault GLOBAL_DFUFIND_OWNER(SectionLabel(DfuFind, _T("owner")), _T(""));

class CDfuFilterView :
	public CDfuViewT<CDfuFilterView>,
	public WTL::CDialogImpl<CDfuFilterView>,
	public WTL::CDialogResize<CDfuFilterView>,
	public WTL::CWinDataExchange<CDfuFilterView>,
	public CCtlColor,
	public CUnknown
{
	typedef CDfuFilterView thisClass;
	typedef WTL::CDialogImpl<CDfuFilterView> baseClass;

protected:
	CString m_searchPrefix;
	CString m_searchCluster;
	CString m_searchLogicalName;
	CString m_searchDescription;
	CString m_searchOwner;

	WTL::CTreeViewCtrl m_results;
	WTL::CButton m_search;

public:
	enum { IDD = IDD_DFUFILTER };

	IMPLEMENT_CUNKNOWN

	CDfuFilterView();
	~CDfuFilterView();

	void Init(IDfuSlot * owner);

	BOOL PreTranslateMessage(MSG* pMsg);

	BEGIN_MSG_MAP(thisClass)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CONTEXTMENU(OnContextMenu)

		MESSAGE_HANDLER_EX(CWM_CLEAR, OnClear)
		MESSAGE_HANDLER_EX(CWM_RESET, OnReset)
		MESSAGE_HANDLER_EX(CWM_SUBMITDONE, OnSubmitDone)
		MESSAGE_HANDLER_EX(DFUM_REFRESHSTATE, OnRefreshState)

		COMMAND_HANDLER(IDC_EDIT_PREFIX, EN_CHANGE, OnEnChangeEdit)
		COMMAND_HANDLER(IDC_EDIT_CLUSTER, EN_CHANGE, OnEnChangeEdit)
		COMMAND_HANDLER(IDC_EDIT_LOGICAL, EN_CHANGE, OnEnChangeEdit)
		COMMAND_HANDLER(IDC_EDIT_DESCRIPTION, EN_CHANGE, OnEnChangeEdit)
		COMMAND_HANDLER(IDC_EDIT_OWNER, EN_CHANGE, OnEnChangeEdit)
		COMMAND_HANDLER(IDC_BUTTON_SEARCH, BN_CLICKED, OnBnClickedButtonSearch)
		COMMAND_HANDLER(IDC_BUTTON_CLEAR, BN_CLICKED, OnBnClickedButtonClear)

		CHAIN_MSG_MAP(CCtlColor)
		REFLECT_NOTIFICATIONS()
		CHAIN_MSG_MAP(CDialogResize<CDfuFilterView>)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(thisClass)
		DLGRESIZE_CONTROL(IDC_EDIT_PREFIX, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_EDIT_CLUSTER, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_EDIT_LOGICAL, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_EDIT_DESCRIPTION, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_EDIT_OWNER, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_BUTTON_SEARCH, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_BUTTON_CLEAR, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_TREE_DFU, DLSZ_SIZE_X | DLSZ_SIZE_Y)
	END_DLGRESIZE_MAP()

	BEGIN_DDX_MAP(thisClass)
		DDX_TEXT(IDC_EDIT_PREFIX, m_searchPrefix)
		DDX_TEXT(IDC_EDIT_CLUSTER, m_searchCluster)
		DDX_TEXT(IDC_EDIT_LOGICAL, m_searchLogicalName)
		DDX_TEXT(IDC_EDIT_DESCRIPTION, m_searchDescription)
		DDX_TEXT(IDC_EDIT_OWNER, m_searchOwner)
	END_DDX_MAP()

	LRESULT OnInitDialog(HWND hWnd, LPARAM lParam);
	void OnContextMenu(HWND phWnd, CPoint pPoint);
	LRESULT OnClear(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
	LRESULT OnReset(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
	LRESULT OnSubmitDone(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
	LRESULT OnRefreshState(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);

	LRESULT OnEnChangeEdit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonSearch(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonClear(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	void DoSaveState();
	void DoRestoreState();
	void DoRefreshState();

	virtual void OnItemSelectionChanged(CTreeNode *, bool bSelected );
	virtual void OnItemExpanding(CTreeNode *, bool bExpanding);
	virtual void HandleNmClick(LPNMTREEVIEW);

protected:
	static void DoSearch(CDfuFilterView *self, std::_tstring searchPrefix, std::_tstring searchCluster, std::_tstring searchLogicalName, std::_tstring searchDescription, std::_tstring searchOwner);
};
//  ===========================================================================
