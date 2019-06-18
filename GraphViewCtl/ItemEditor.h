#pragma once

__interface IGraphItem;
class CCanvas;

class CItemEdit : 
    public CWindowImpl< CItemEdit, CEdit, CWinTraitsOR<WS_BORDER|ES_CENTER|ES_AUTOHSCROLL> >
{
    CCanvas* m_canvas;
    IGraphItem* m_gi;
    CComPtr<IGraphSubgraph> m_parent;
    unsigned m_index; // of graph item in its parent
    CString m_curText;

    bool ChangeLabel(bool bAsk);
    void UpdateText();
    bool EditNext(unsigned newindex);
    void RestoreState();

public:

    CItemEdit(IGraphItem* item, CCanvas* canvas);

    LRESULT MouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

    virtual void OnFinalMessage(HWND /*hWnd*/);

    BEGIN_MSG_MAP(CItemEdit)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
        MESSAGE_HANDLER(WM_CHAR, OnChar)
        MESSAGE_HANDLER(WM_GETDLGCODE, OnGetDlgCode)
    END_MSG_MAP()

    LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);
    LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled);
    LRESULT OnChar(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled);
    LRESULT OnGetDlgCode(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);
};

