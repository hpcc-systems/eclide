#pragma once

#include "RoxieDebugSession.h"
#include "DebugGraphView.h"

class CBreakpointToolBar : public CMFCToolBar
{
    virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
    {
        CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
    }

    virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CDockableBreakpointView : public CDockablePane
{
public:
    CDockableBreakpointView();
    virtual ~CDockableBreakpointView();

    void SetOwner(IGraphViewSlot * owner);
    void LoadBreakpoints(const roxie::CBreakpointMap & bpmap);
    void AdjustLayout();
    void OnChangeVisualStyle();
    void Send_Clear();
    void Post_Reset();

protected:
    CBreakpointToolBar m_wndToolBar;
    CMFCListCtrl m_wndBreakpointList;
    CImageList m_BreakpointListImages;
    UINT m_nCurrSort;
    IGraphViewSlot * m_owner;

public:

// Overrides
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);

    afx_msg void OnButtonSetBreakpoint();
    afx_msg void OnUpdateButtonSetBreakpoint(CCmdUI* pCmdUI);
    afx_msg void OnButtonRemoveBreakpoint();
    afx_msg void OnUpdateButtonRemoveBreakpoint(CCmdUI* pCmdUI);

    afx_msg void OnClassAddMemberFunction();
    afx_msg void OnClassAddMemberVariable();
    afx_msg void OnClassDefinition();
    afx_msg void OnClassProperties();
    afx_msg void OnNewFolder();
    afx_msg void OnPaint();
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg LRESULT OnChangeActiveTab(WPARAM, LPARAM);
    afx_msg void OnDelete();
    afx_msg void OnUpdateDelete(CCmdUI* pCmdUI);

    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnLvnItemchangedListResults(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNmDblClk(NMHDR *pNMHDR, LRESULT *pResult);
};
