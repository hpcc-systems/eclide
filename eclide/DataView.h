#pragma once

#include "TableGridCtrl.h"
#include "RoxieDebugSession.h"

//  ===========================================================================
class CDataView : public CWnd, public CUnknown
{
// Construction
public:
	BEGIN_CUNKNOWN
	END_CUNKNOWN(CUnknown)

	CDataView();
	DECLARE_DYNCREATE(CDataView)

// Attributes
public:
	CTableGridCtrl m_grid;
	StlLinked<ITable> m_data;

// Operations
public:
	void SetData(const ITable * data, bool hasRowNum, const roxie::CSearchRowVector & searchResults);
	void SetTabSelection(const roxie::CSearchRowVector & searchResults);

// Overrides
protected:
	BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CDataView();
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
typedef StlLinked<CDataView> CDataViewAdapt;
//  ===========================================================================
