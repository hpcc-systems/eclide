#pragma once

#include "TabPane.h"
#include "ResultView.h"
#include <HtmlView.h>	//wlib
#include <CustomMessages.h>

class CSummaryView : 
    public CWindowImpl<CSummaryView>,
    public CTabPane,
    public IResultView
{
    typedef CSummaryView thisClass;
    typedef CWindowImpl<CSummaryView> baseClass;

private:
    IResultSlot *m_resultSlot;
    StlLinked<Dali::IWorkunit> m_wu;
    CString m_Url;
    CString m_FramedUrl;
    CHtmlView m_view;

public:
    enum {IDD = IDD_SUMMARYVIEW};

    CSummaryView(Dali::IWorkunit * wu, IResultSlot *resultSlot);
    ~CSummaryView();

    void Unhide();

    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(IResultView)
    END_CUNKNOWN(CTabPane)

    BEGIN_MSG_MAP(thisClass)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        MSG_WM_PAINT(OnPaint)
        MSG_WM_CONTEXTMENU(OnContextMenu)
        MSG_WM_SIZE(OnSize)
        MESSAGE_HANDLER(WM_FORWARDMSG, OnForwardMsg)
        MESSAGE_HANDLER_EX(CWM_INITIALIZE, OnInitialize)
        MESSAGE_HANDLER_EX(CWM_REFRESH, OnRefresh)
        COMMAND_ID_HANDLER(ID_BROWSER_NEW, OnBrowserNew)
        COMMAND_ID_HANDLER(ID_BROWSER_IE, OnBrowserIE)
        COMMAND_ID_HANDLER(ID_BROWSER_BACK, OnBrowserBack)
        COMMAND_ID_HANDLER(ID_BROWSER_FORWARD, OnBrowserForward)
        COMMAND_ID_HANDLER(ID_BROWSER_STOP, OnBrowserStop)
        COMMAND_ID_HANDLER(ID_BROWSER_REFRESH, OnBrowserRefresh)
        COMMAND_ID_HANDLER_EX(ID_EDIT_COPY, OnEditCopy)
        COMMAND_ID_HANDLER_EX(ID_EDIT_SELECT_ALL, OnEditSelectAll)

        //CHAIN_MSG_MAP_MEMBER(m_view)
        REFLECT_NOTIFICATIONS()
    END_MSG_MAP()

    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnPaint(HDC /*dc*/);
    void OnSize(UINT wParam, const CSize & size);
    LRESULT OnForwardMsg(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled);
    void DoRefresh();
    LRESULT OnRefresh(UINT /*uMsg*/, WPARAM /*bCreated*/, LPARAM /*lParam*/);
    LRESULT OnBrowserNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnBrowserIE(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnBrowserBack(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnBrowserForward(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnBrowserStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnBrowserRefresh(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    void OnEditCopy(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/);
    void OnEditSelectAll(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/);
    LRESULT OnInitialize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
    void OnContextMenu(HWND /*hWnd*/, CPoint pt);

    //CTabPane
    void Refresh();
    virtual HWND GetHWND();
    bool CanCopy();
    bool HasResults() const;
    bool UpdateUI(CCmdUI * cui);

    //ISearchable
    bool CanBeSearched() const; 
    bool Search(const CSearchableInfo & /*info*/, CSearchableResult &/*result*/) const;

    //  --  IResultView  ---
    HWND GetHwnd() const;
};
