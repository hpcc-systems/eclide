#pragma once

#include "DataView.h"
#include "SourceDataView.h"
#include "PropertyGridView.h"
#include "DefinitionParser.h"
#include "atlGraphView.h"
#include "DebugGraphView.h"

//  ===========================================================================
class CDockablePropertyGridViews : public CDockablePane//, public CResultSlotImpl
{
// Construction
public:
	CDockablePropertyGridViews();
	DECLARE_DYNCREATE(CDockablePropertyGridViews)

// Attributes
public:
	IGraphViewSlot * m_owner;

	CMFCTabCtrl	m_wndTab;

	typedef std::map<std::_tstring, CPropertyGridViewAdapt> StringPropertyGridMap;
	StringPropertyGridMap m_propertyGridTabs;

// Operations
public:
	void SetOwner(IGraphViewSlot * owner);

	void ActivateTab(const CWnd * wnd);

	void PrepTabPropertyGrid(const SGV::CGraphViewCtrl * globalGraphView, const SGV::CGraphViewCtrl * activeGraphView);
	void SetTabPropertyGrid(const std::_tstring & label, const ITable* data);
	void ClearTabSetTabPropertyGrid();

	void CloseAllTabs();

	void Send_Clear();
	void Post_Reset();

// Overrides
protected:
	BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CDockablePropertyGridViews();
	//void SetWU(Dali::IWorkunit * wu);
	//bool WorkunitDeleted(Dali::IWorkunit *);

	// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC*);
	afx_msg void OnSize(UINT nType, int cx, int cy);

protected:
	DECLARE_MESSAGE_MAP()
};
//  ===========================================================================
