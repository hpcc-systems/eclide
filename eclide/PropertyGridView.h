#pragma once

#include "TableGridCtrl.h"
#include "RoxieDebugSession.h"
#include "DebugGraphView.h"

class CMyTableGridCtrl : public CTableGridCtrl
{
protected:
	XGMML_CAT m_cat;
	IGraphViewSlot * m_owner;

public:
	CMyTableGridCtrl();
	CMyTableGridCtrl(XGMML_CAT cat);
	void SetOwner(IGraphViewSlot * owner);

	virtual BOOL CallbackCellClick(SGRID::GV_CELL *pCell);
};
//  ===========================================================================
class CPropertyGridView : public CWnd, public CUnknown
{
protected:
	IGraphViewSlot * m_owner;

	CMyTableGridCtrl m_grid;
	StlLinked<ITable> m_data;

public:
	BEGIN_CUNKNOWN
	END_CUNKNOWN(CUnknown)

	CPropertyGridView();
	CPropertyGridView(XGMML_CAT m_cat);
	virtual ~CPropertyGridView();
	DECLARE_DYNCREATE(CPropertyGridView)

	void SetOwner(IGraphViewSlot * owner);
	void SetData(const ITable * data);
	void SetTabSelection(const roxie::CSearchRowVector & searchResults);

// Overrides
protected:
	BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	//void SetWU(Dali::IWorkunit * wu);
	//bool WorkunitDeleted(Dali::IWorkunit *);

	// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC*);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonUp(UINT, CPoint);

protected:
	DECLARE_MESSAGE_MAP()
};
typedef StlLinked<CPropertyGridView> CPropertyGridViewAdapt;
//  ===========================================================================
