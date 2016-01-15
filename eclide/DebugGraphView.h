#pragma once

#include "ResultView.h"
#include "TabPane.h"
#include "atlGraphView.h"
#include "Notifications.h"
#include "XGMMLSaxHandler.h"
#include "RoxieDebugSession.h"

//  ===========================================================================
class CDebugGraphView;

struct GlobalSearchConditions
{
    ::CString m_field;
    ::CString m_condition;
    ::CString m_value;
    int m_caseSensitive;
};

__interface IGraphViewSlot
{
    const CUniqueID & GetCurrentID();
    void NewSelection(const CDebugGraphView * from, const CUniqueID & newSel, bool recenterGraph, bool forceDataLoad, bool dontShowDefinition = false);
    void ScaleChanged(int zoom);

    unsigned int ProcessContextMenu(WTL::CMenu & menu, CPoint & pt, CUniqueIDVector & subgraphs);
    unsigned int ProcessContextMenu(WTL::CMenu & menu, CPoint & pt, IGraphSubgraph * subgraph);
    unsigned int ProcessContextMenu(WTL::CMenu & menu, CPoint & pt, SGV::CGraphVertex & vertex);
    unsigned int ProcessContextMenu(WTL::CMenu & menu, CPoint & pt, SGV::CGraphEdge & edge);

    void DeleteBreakpoint(const ::CString & idx);
    void GetAllBreakpoints();

    const roxie::CSearchMap & GetSearchResults() const;
    const CUniqueID & GetGlobalID(const CUniqueID & id) const;

    GlobalSearchConditions * GetGlobalSearchConditions();

    const SGV::CGraphViewCtrl & GetGlobalGraphView();
    const SGV::CGraphViewCtrl & GetActiveGraphView();
    Dali::IWorkunit * GetWorkunit();
    CRoxieDebugSession * GetDebugSession();
};
//  ===========================================================================
class CDebugGraphView : 
    public WTL::CDialogImpl<CDebugGraphView>,
    public WTL::CDialogResize<CDebugGraphView>,
    public WTL::CWinDataExchange<CDebugGraphView>,
    public boost::signals::trackable
{
    typedef CDebugGraphView thisClass;
    typedef WTL::CDialogImpl<CDebugGraphView> baseClass;

public:
    enum {IDD = IDD_DEBUGVIEW};

    CDebugGraphView();
    IResultSlot *m_resultSlot;
    boost::signals::connection m_graphConnection;

// Attributes
public:
    //CGraphView m_dlgview;
    SGV::CGraphViewCtrl m_view; 
    SGV::CGraphViewCtrl *m_viewGlobal; 

    std::_tstring m_xgmml;
    bool m_redundantSubgraphs;

    CString m_prevFind;
    BOOL m_prevMatchCase;
    BOOL m_prevWholeWord;
    BOOL m_prevSearchTooltips;
    CUniqueIDVector m_found;

    bool m_passthroughVertices;

    IGraphViewSlot * m_owner;

    CUniqueID m_notificationItem;
    CUniqueID m_centerOnItem;
    int m_actionID;

    bool m_viewRunningOnly;
    bool m_viewFoundOnly;

    // Operations
public:
    void SetOwner(IGraphViewSlot * owner, SGV::CGraphViewCtrl * viewGlobal = NULL);
    //void DoMinClicked(bool expand);
    void DoMinClicked(const CUniqueID & id, bool expand);

    bool CalcVisibility(const CUniqueID & activeItem = CUniqueID());
    void CalcVisibilityAndLayout(const CUniqueID & activeItem = CUniqueID(), bool forceLayout = false);
    void UpdateLabels();

    bool LoadXGMML(const std::_tstring & xgmml, IXGMMLRenderCallback * callback, const CUniqueID & selection, bool redundantSubgraphs, bool passthroughVertices);
    bool MergeXGMML(const std::_tstring & xgmml, IXGMMLRenderCallback * callback, const CUniqueID & selection, bool appendMissing, bool redundantSubgraphs, bool passthroughVertices);
    void Clear();
    void EnsureVisible(const CUniqueID & id, bool centerOnItem, bool selectItem = false, bool appendSelection = false);
    void GetSelectedEdges(IUniqueIDContainer * results);
    void SetSourceTargetSelected(const CUniqueID & edgeID);
    void SetZoom(int zoom);
    int GetZoom();

    BEGIN_MSG_MAP(thisClass)
        MSG_WM_INITDIALOG(OnInitDialog);

        NOTIFY_CODE_HANDLER(SCW_NOTIFYMINCLICKED, OnNotifyMinClicked)
        NOTIFY_CODE_HANDLER(SCW_NOTIFYLAYOUTFINISHED, OnLayoutFinished)
        NOTIFY_CODE_HANDLER(SCW_NOTIFYGRAPHCONTEXT, OnGraphContext)
        NOTIFY_CODE_HANDLER(SCW_NOTIFYSUBGRAPHCONTEXT, OnSubgraphContext)
        NOTIFY_CODE_HANDLER(SCW_NOTIFYVERTEXCONTEXT, OnVertexContext)
        NOTIFY_CODE_HANDLER(SCW_NOTIFYEDGECONTEXT, OnEdgeContext)
        NOTIFY_CODE_HANDLER(SCW_NOTIFYSELECTED, OnSelChanged)
        NOTIFY_CODE_HANDLER(SCW_NOTIFYSCALECHANGE, OnScaleChanged)
        NOTIFY_CODE_HANDLER(SCW_NOTIFYCALCTOOLTIP, OnCalcTooltip)

        MSG_WM_ERASEBKGND(OnEraseBkgnd)

        //MSG_WM_PAINT(OnPaint);
        //MSG_WM_CONTEXTMENU(OnContextMenu);
        //MSG_WM_TIMER(OnTimer);

        //MSG_WM_VSCROLL(OnVScroll);
        //MSG_WM_FORWARDMSG(OnForwardMsg)

        //COMMAND_ID_HANDLER(ID_EDIT_FIND, OnEditFind)
        //COMMAND_ID_HANDLER(ID_EDIT_FINDNEXT, OnEditFindNext)
        //COMMAND_ID_HANDLER(ID_EDIT_FINDPREV, OnEditFindPrev)
        //MESSAGE_HANDLER(WM_COMMAND, OnFindReplaceCmd)

        //COMMAND_ID_HANDLER(ID_BROWSER_BACK, OnBrowserBack);
        //COMMAND_ID_HANDLER(ID_BROWSER_FORWARD, OnBrowserForward);

        //COMMAND_ID_HANDLER(ID_GRAPH_ZOOMTOFIT, OnGraphZoomToFit);
        //COMMAND_ID_HANDLER(ID_GRAPH_ZOOMTOWIDTH, OnGraphZoomToWidth);
        //COMMAND_ID_HANDLER(ID_GRAPH_LAYOUT, OnGraphLayout);

        //NOTIFY_CODE_HANDLER(LVN_ITEMCHANGED, OnListItemChanged)

        REFLECT_NOTIFICATIONS()
        CHAIN_MSG_MAP(WTL::CDialogResize<thisClass>)
    END_MSG_MAP()

    BEGIN_DLGRESIZE_MAP(thisClass)
        DLGRESIZE_CONTROL(IDC_STATIC_PLACEHOLDER, DLSZ_SIZE_X | DLSZ_SIZE_Y)
    END_DLGRESIZE_MAP()

    BEGIN_DDX_MAP(thisClass)
    END_DDX_MAP()

    int GetLargestVertexIDInt();
    BOOL PreTranslateMessage(MSG* pMsg);
// Implementation
public:
    virtual ~CDebugGraphView();

    // Generated message map functions
    LRESULT OnInitDialog(HWND /*hWnd*/, LPARAM /*lParam*/);

    LRESULT OnNotifyMinClicked(int nId, LPNMHDR nmhdr, BOOL& bHandled); 
    LRESULT OnLayoutFinished(int nId, LPNMHDR nmhdr, BOOL& bHandled); 
    LRESULT OnGraphContext(int nId, LPNMHDR nmhdr, BOOL& bHandled); 
    LRESULT OnSubgraphContext(int nId, LPNMHDR nmhdr, BOOL& bHandled); 
    LRESULT OnVertexContext(int nId, LPNMHDR nmhdr, BOOL& bHandled); 
    LRESULT OnEdgeContext(int nId, LPNMHDR nmhdr, BOOL& bHandled); 
    LRESULT OnSelChanged(int nId, LPNMHDR nmhdr, BOOL& bHandled); 
    LRESULT OnScaleChanged(int nId, LPNMHDR nmhdr, BOOL& bHandled); 
    LRESULT OnCalcTooltip(int nId, LPNMHDR nmhdr, BOOL& bHandled); 

    BOOL OnEraseBkgnd(WTL::CDCHandle dc);
    //afx_msg BOOL OnEraseBkgnd(CDC*);
    //afx_msg void OnSize(UINT nType, int cx, int cy);

    void DoEditFind();
    void DoEditFindAgain(bool /*bNext*/);

    //  --  ISkip  --
    bool SkipEdge(IGraphEdge * e) const;
    bool SkipVertex(IGraphVertex * v) const;
    bool SkipSubgraph(IGraphSubgraph * sg) const;
};

