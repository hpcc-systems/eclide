#pragma once

#include "global.h"
#include "TreePairNode.h"
#include "Selection.h"
#include "Dali.h"
#include "Combo.h"
#include "MultiTreeNode.h"
#include "Migration.h"
#include "Filter.h"

#include <LoginDlg.h>	//wlib

enum UM
{
    UM_FIRST = CWM_LAST + 1,
    UM_INITIALIZE,
    UM_MODULESLOADED,
    UM_ATTRIBUTESLOADED,
    UM_ATTRIBUTEHISTORIESLOADED,
    UM_ITEMCLICKED,
    UM_PASTE,
    UM_INVALIDATE,
//  Frame Messages
    UM_CLEAR_DIFF,
    UM_NEWSELECTION,
    //UM_NEWCHECK,
    UM_PROGRESSUPDATE,
    UM_LAST
};

struct STRUCT_ATTRIBUTEHISTORIESLOADED
{
    CAttributePairNode * treeItem;
    IAttributeHistoryVector m_lhs;
    IAttribute * m_rhs;
};

struct STRUCT_ITEMCLICKED
{
    CTreeNode * m_item;
    IAttributeVector m_attrs;
    IAttributeVector m_depAttrs;
};

struct STRUCT_PASTE
{
    IAttributeVector m_attrs;
    std::_tstring m_errors;
};

#define NSUPPORT_WORKSPACE
class CRepositoryCompareView : 
    public CMultiTreeNodeViewCtrl, 
    public WTL::CCustomDraw<CRepositoryCompareView>,
    //public WTL::CThemeImpl<CRepositoryCompareView>,
    public CUnknown,
    public IMigrationCallback
{
    typedef CRepositoryCompareView thisClass;
    typedef CMultiTreeNodeViewCtrl baseClass;
    typedef WTL::CCustomDraw<CRepositoryCompareView> customClass;
    //typedef WTL::CThemeImpl<CRepositoryCompareView> themeClass;
#ifdef _SUPPORT_WORKSPACE
    friend void thread_LoadWorkspaceItems(CRepositoryCompareView * self, CWorkspacePairNode * ws);
#endif
    friend void thread_LoadAttributes(CRepositoryCompareView * self, CModulePairNode * mod);
    friend void thread_LoadWorkspacesAndModules(CRepositoryCompareView * self, bool noRefresh);
    friend void thread_ItemClicked(CRepositoryCompareView * self, CTreeNode * tn, bool shiftKey);
    friend void thread_Paste(CRepositoryCompareView * self, IRepository * rep, std::_tstring attrStrings);

public:
    WTL::CImageList m_ilNormal;
    WTL::CImageList m_ilCheck;
    WTL::CImageList m_ilState;

    LONG m_loadingModules;
    int m_filter;

    CSelection m_sel;
    CComPtr<IMigration> m_migrator;
    clib::CThreadQueue m_selectThreads;
    clib::CThreadQueue m_expandThreads;

    int m_jobCount;

public:
    BEGIN_CUNKNOWN
    END_CUNKNOWN(CUnknown)

    STRUCT_DALI m_daliLHS;
    CComPtr<IRepository> m_repLHS;
    CComPtr<IRepository> m_repRHS;

    DECLARE_WND_SUPERCLASS(NULL, CMultiTreeNodeViewCtrl::GetWndClassName())

    CRepositoryCompareView();

    BOOL PreTranslateMessage(MSG* pMsg);

    void CreateData();	//For Testing Only
    void Init(IRepository * repLHS, IRepository * repRHS, const STRUCT_DALI & dali);
    void Kill();
    void Load(CTreeNode * item, bool noRefresh);
    void SelectAll();
    void Clear();
    void ClearSelection();
    void ExpandAll();
    bool SetFilter(int filter = AMT_SHOW_MATCHING & AMT_SHOW_NOT_MATCHING & AMT_SHOW_ORPHANS);
    void SupressModuleChecksum(bool supressModuleChecksum);

    bool CanCopy() const;
    bool CanPaste() const;
    void DoCopy() const;
    void DoPaste(const std::_tstring & attributes);

    bool DoMigration();
    int GetMigrationCount() const;

    BEGIN_MSG_MAP(thisClass)
        MSG_WM_CREATE(OnCreate)
        MESSAGE_HANDLER(UM_INITIALIZE, OnInitialize)
        MSG_WM_SIZE(OnSize)
        MSG_WM_TIMER(OnTimer)
        MSG_WM_LBUTTONDOWN(OnLButtonDown)
        MSG_WM_RBUTTONDOWN(OnRButtonDown)
        //MSG_WM_CONTEXTMENU(OnContextMenu)

        MESSAGE_HANDLER(UM_MODULESLOADED, OnModulesLoaded)
        MESSAGE_HANDLER(UM_ATTRIBUTESLOADED, OnAttributesLoaded)
        MESSAGE_HANDLER(UM_ATTRIBUTEHISTORIESLOADED, OnAttributeHistoriesLoaded)
        MESSAGE_HANDLER(UM_ITEMCLICKED, OnItemClicked)
        MESSAGE_HANDLER(UM_PASTE, OnPaste)
        MESSAGE_HANDLER(UM_INVALIDATE, OnInvalidate)

        REFLECTED_NOTIFY_CODE_HANDLER(TVN_SELCHANGED, OnSelChanged)
        REFLECTED_NOTIFY_CODE_HANDLER(TVN_ITEMEXPANDING, OnItemExpanding)
        REFLECTED_NOTIFY_CODE_HANDLER(TVN_DELETEITEM, OnDeleteItem)

        CHAIN_MSG_MAP_ALT(customClass, 1)
        CHAIN_MSG_MAP(baseClass)
    END_MSG_MAP()

    int OnCreate(LPCREATESTRUCT lpCreateStruct);
    LRESULT OnInitialize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    void OnSize(UINT nType, CSize size);
    void OnTimer(UINT_PTR nIDEvent);

    void OnLButtonDown(UINT nFlags, CPoint point);
    void OnRButtonDown(UINT nFlags, CPoint point);
    void OnContextMenu(CWindow wnd, CPoint point);

    LRESULT OnModulesLoaded(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnAttributesLoaded(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnAttributeHistoriesLoaded(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnItemClicked(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnPaste(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnInvalidate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

    LRESULT OnSelChanged(WPARAM wParam, LPNMHDR pNMHDR, BOOL& bHandled);
    LRESULT OnItemExpanding(WPARAM wParam, LPNMHDR pNMHDR, BOOL& bHandled);
    LRESULT OnDeleteItem(WPARAM wParam, LPNMHDR pNMHDR, BOOL& bHandled);

    DWORD OnPrePaint(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw);
    DWORD OnItemPrePaint(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw);
    DWORD OnItemPostPaint(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw);

    //  IMigrationCallback
    BOOL Invalidate(BOOL bErase = TRUE) throw();
    void LogMsg(const std::_tstring & msg);
    void PostStatus(const TCHAR* pStr);
    void PostProgress(int progress);
};

