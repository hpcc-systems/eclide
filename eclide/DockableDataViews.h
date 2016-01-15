#pragma once

#include "DataView.h"
#include "SourceDataView.h"
#include "PropertyGridView.h"
#include "DefinitionParser.h"
#include "atlGraphView.h"
#include "DebugGraphView.h"

//  ===========================================================================
class CDockableDataViews : public CDockablePane//, public CResultSlotImpl
{
// Construction
public:
    CDockableDataViews();
    DECLARE_DYNCREATE(CDockableDataViews)

// Attributes
public:
    IGraphViewSlot * m_owner;

    CMFCTabCtrl	m_wndTab;
    typedef std::map<std::_tstring, CDataViewAdapt> StringResultMap;
    StringResultMap m_dataTabs;

    typedef std::map<std::_tstring, CSourceDataViewAdapt> StringSourceMap;
    StringSourceMap m_sourceTabs;

// Operations
public:
    void SetOwner(IGraphViewSlot * owner);

    void ActivateTab(const CWnd * wnd);

    void SetTabData(const ITableMap * data, const roxie::CSearchRowVector & searchResults);
    void SetTabData(const std::_tstring & label, const ITable * data, bool hasRowNum, const roxie::CSearchRowVector & searchResults);
    void ClearTabData();

    void PrepTabEcl(const SGV::CGraphViewCtrl & graphView, Dali::IWorkunit * wu);
    void SetTabEcl(Dali::IWorkunit * wu, int row, const CUniqueID & id);
    void SetTabEcl(const std::_tstring & moduleLabel, const std::_tstring & attributeLabel, int row, const CUniqueID & id);
    void SetTabEcl(const std::_tstring & localFile, int row, const CUniqueID & id);

    void SetTabSelection(const std::_tstring & label, const roxie::CSearchRowVector & searchResults);
    void SetTabSelection(const std::_tstring & label, int row, int col);
    void ClearTabSelection();

    void CloseAllTabs();

    void Send_Clear();
    void Post_Reset();

// Overrides
protected:
    BOOL PreTranslateMessage(MSG* pMsg);
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
    virtual ~CDockableDataViews();
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
