#pragma once

#include "RoxieDebugSession.h"
#include "DebugGraphView.h"

class CDockableSearchView : public CPaneDialog
{
	DECLARE_DYNAMIC(CDockableSearchView)

public:
	GlobalSearchConditions * m_searchConditions;

	CMFCListCtrl m_listResults;

	IGraphViewSlot * m_owner;

	CDockableSearchView();   // standard constructor
	virtual ~CDockableSearchView();

	void SetOwner(IGraphViewSlot * owner);   // standard constructor

// Dialog Data
	enum { IDD = IDD_DIALOG_SEARCH };

	void Send_Clear();
	void Post_Reset();
	void LoadResults(const roxie::CSearchMap & results);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnInitialize(WPARAM wParam, LPARAM lParam);
	afx_msg void OnDestroy();

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg LRESULT OnCtlColor2(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCtlColor3(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

	void AdjustWidth(int cx, UINT uiCtrlID, int nMargin = 10);
	void AdjustHeight(int cy, UINT uiCtrlID, int nMargin = 10);
public:
	afx_msg void OnLvnItemchangedListResults(NMHDR *pNMHDR, LRESULT *pResult);
};
