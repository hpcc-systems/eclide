#pragma once

#include "SourceView.h"
#include "RoxieDebugSession.h"
#include "DebugGraphView.h"

//  ===========================================================================
class CSourceDataView : public CWnd, public CSourceSlotImpl, public CUnknown
{
protected:
	IGraphViewSlot * m_owner;

	typedef std::map<int, CUniqueIDVector> BreakPointPosMap;

	BreakPointPosMap m_breakpointPos;
	bool m_loaded;
// Construction
public:
	BEGIN_CUNKNOWN
	END_CUNKNOWN(CUnknown)

	CSourceDataView();
	DECLARE_DYNCREATE(CSourceDataView)

// Attributes
public:
	CSourceCtrl m_eclView;

// Operations
public:

// Overrides
protected:
	BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	void CalcEdgesForBreakpoints(CUniqueIDVector & vertices, CUniqueIDVector & edges);

// Implementation
public:
	virtual ~CSourceDataView();
	void SetOwner(IGraphViewSlot * owner);

	void SetText(const std::_tstring & text);
	void SetBreakpointLocation(int row, const CUniqueID & id);
	void SelectLine(int row, int col);
	void ClearSelection();
	virtual void LoadText();

	//void SetWU(Dali::IWorkunit * wu);
	//bool WorkunitDeleted(Dali::IWorkunit *);

	// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC*);
	afx_msg void OnSetFocus(CWnd*);
	afx_msg void OnSize(UINT nType, int cx, int cy);

protected:
	DECLARE_MESSAGE_MAP()
};
typedef StlLinked<CSourceDataView> CSourceDataViewAdapt;
//  ===========================================================================
class CBuilderDataView : public CSourceDataView
{
protected:
	StlLinked<Dali::IWorkunit> m_wu;
public:
	void SetSource(Dali::IWorkunit * wu);
	void LoadText();
};
//  ===========================================================================
class CAttributeDataView : public CSourceDataView
{
protected:
	std::_tstring m_module;
	std::_tstring m_attribute;
	StlLinked<IAttribute> m_attr;
public:
	void SetSource(const std::_tstring & module, const std::_tstring & attribute);
	void LoadText();
};
//  ===========================================================================
class CLocalDataView : public CSourceDataView
{
protected:
	std::_tstring m_filePath;
public:
	void SetSource(const std::_tstring & filePath);
	void LoadText();
};
//  ===========================================================================
