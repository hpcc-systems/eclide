#include "StdAfx.h"
#include "resource.h"
#include "GraphViewCanvas.h"
#include "QueryBuilderUI.h"

class CNodePopupWnd;
typedef std::map<unsigned int, CNodePopupWnd*> NodePopupMap;
static NodePopupMap s_nodePopupMap;

//*****************************************************************************

typedef CWinTraits<
    WS_POPUP |
    WS_VISIBLE |
    WS_CAPTION |
    WS_THICKFRAME |
    WS_CLIPCHILDREN |
    WS_SYSMENU |
    WS_MINIMIZEBOX |
    WS_MAXIMIZEBOX,
    WS_EX_CLIENTEDGE> CNodePopupTraits;

class CNodePopupWnd : public CWindowImpl<CNodePopupWnd, CWindow, CNodePopupTraits>
{
    enum 
    {
        DEFAULT_MARGIN=16,
        DEFAULT_WIDTH=800,
        DEFAULT_HEIGHT=600
    };

    struct RtfStream
    {
        LPCSTR pstr;
        DWORD pos;
    };

    CComQIPtr<IGraphVertex> m_vertex;
    CComQIPtr<IGraphEdge> m_edge;
    CRichEditCtrl m_wndRTF;
    CRect m_rcRTF;

public:
    CNodePopupWnd(IGraphItem* item) : m_vertex(item), m_edge(item) {};

    void Init()
    {
        CRect rccreate(1, 1, 300, 10);
        DWORD dwStyle = WS_VISIBLE | WS_CHILD | ES_MULTILINE | ES_READONLY | ES_AUTOHSCROLL | ES_AUTOVSCROLL;
        m_wndRTF.Create(m_hWnd, &rccreate, NULL, dwStyle);

        ATLASSERT(m_wndRTF.IsWindow());
        if(!m_wndRTF.IsWindow())
            return;

        m_wndRTF.SendMessage(
            EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, 
            MAKELONG(DEFAULT_MARGIN, DEFAULT_MARGIN)); // does nothing?!

        m_wndRTF.SetEventMask(ENM_REQUESTRESIZE);
        m_wndRTF.SetTargetDevice(NULL, 0);

        if(m_vertex)
            SetWindowText(_T("Graph Node Information"));
        if(m_edge)
            SetWindowText(_T("Graph Edge Information"));

        Update();
    }

    void Update()
    {
        // maintain top/left pos
        CRect rcwnd;
        GetWindowRect(rcwnd);

        // Clear text
        m_wndRTF.SetWindowText(NULL);

        // Stream in new text...
        CComVariant bstr;
        std::string text;
        if(m_vertex)
            m_vertex->GetProperty(PROP_TOOLTIP, bstr);
        if(m_edge)
            m_edge->GetProperty(PROP_TOOLTIP, bstr);
        if (bstr.vt == VT_BSTR)
            text = CW2A(bstr.bstrVal);
        LPCSTR pstrText=text.c_str();
        RtfStream st = { pstrText, 0 }; 
        EDITSTREAM es = { 0 };
        es.dwCookie = (DWORD) &st;
        es.dwError = 0;
        es.pfnCallback = _StreamReadCallback;
        UINT uFormat = pstrText[0]=='{' ? SF_RTF : SF_TEXT;
        m_wndRTF.StreamIn(uFormat, es);

        // Request new layout
        int cx = DEFAULT_WIDTH;
        CRect rc( 0, 0, cx, DEFAULT_HEIGHT);
        m_wndRTF.ResizeClient(cx, DEFAULT_HEIGHT, FALSE);
        m_wndRTF.SetRect(&rc);
        m_wndRTF.RequestResize();
        CRect rcRTF(m_rcRTF);
        CRect rcOrig(m_rcRTF);

        // Resize RTF control until the height changes. This will indicate
        // the minimum/optimal width of the text.
        int iStep = 30;
        while( true ) 
        {
            ATLASSERT(cx>0);
            CRect rc( 0, 0, cx - iStep, DEFAULT_HEIGHT);
            m_wndRTF.ResizeClient(cx - iStep, DEFAULT_HEIGHT, FALSE);
            m_wndRTF.SetRect(&rc);
            m_wndRTF.RequestResize();
            if(rcRTF.Height() != m_rcRTF.Height()) 
            {
                if( iStep <= 1 ) break;
                iStep /= 2;
            }
            else 
            {
                cx -= iStep;
                if( cx <= 0 ) 
                {
                    rcRTF = rcOrig;
                    break;
                }
                rcRTF = m_rcRTF; // Apply better width
            }
        }

        bool notnormal=IsIconic() || IsZoomed();
        m_wndRTF.SetWindowPos(0, &rcRTF, (notnormal?0:SWP_SHOWWINDOW) | SWP_NOACTIVATE | SWP_NOZORDER);
        if(!notnormal)
        {
            DWORD exstyle=GetWndExStyle(0);
            rcwnd.right=rcwnd.left+rcRTF.Width();
            rcwnd.bottom=rcwnd.top+rcRTF.Height();
            ::AdjustWindowRectEx(&rcwnd, GetWndStyle(0), FALSE, exstyle);
            // finally, keep it inside screen bounds
            CRect rcScreen;
            ::SystemParametersInfo(SPI_GETWORKAREA, 0, &rcScreen, 0);
            if(rcwnd.right > rcScreen.right) 
                rcwnd.OffsetRect(-(rcwnd.right-rcScreen.right), 0);
            if(rcwnd.top < rcScreen.top) 
                rcwnd.OffsetRect(0, rcScreen.top-rcwnd.top);

            SetWindowPos(0, rcwnd, SWP_NOZORDER);
        }
    }

    virtual void OnFinalMessage(HWND /*hWnd*/)
    {
        unsigned int hash;
        
        if(m_vertex)
            hash=m_vertex->GetID().GetHash();
        else if(m_edge)
            hash=m_edge->GetID().GetHash();

        NodePopupMap::iterator itr=s_nodePopupMap.find(hash);
        ATLASSERT(itr!=s_nodePopupMap.end());
        s_nodePopupMap.erase(itr);

        delete this;
    }

    static DWORD CALLBACK _StreamReadCallback(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG* pcb)
    {
        RtfStream *pS = reinterpret_cast<RtfStream *>(dwCookie);
        ATLASSERT(pS);
        LPCSTR pstr = pS->pstr + pS->pos;
        ATLASSERT(!::IsBadStringPtr(CA2T(pstr, CP_UTF8), (UINT_PTR)-1));
        LONG len = ::lstrlenA(pstr);
        if( cb>len ) cb = len;
        ::CopyMemory(pbBuff, pstr, cb);
        pS->pos += cb;
        *pcb = cb;
        return 0;
    }

protected:
    BEGIN_MSG_MAP_EX(CNodePopupWnd)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_CLOSE(OnClose)
        NOTIFY_CODE_HANDLER(EN_REQUESTRESIZE, OnRequestResize);
    END_MSG_MAP()

    LRESULT OnCreate(LPCREATESTRUCT lParam)
    {
        LRESULT lRet = DefWindowProc();
        if(!lRet)
            Init();
        return lRet;
    }

    void OnClose()
    {
        DestroyWindow();
    }

    // RichEdit notifications

    LRESULT OnRequestResize(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
    {
        REQRESIZE* prr = (REQRESIZE*) pnmh;
        m_rcRTF = prr->rc;
        return 0;
    }
};

//*****************************************************************************

void QBUI::CreateNodePopup(HWND hwndOwner, const CPoint& pos, IGraphItem* item)
{
    unsigned int hash=item->GetID().GetHash();
    NodePopupMap::iterator itr=s_nodePopupMap.find(hash);

    if(itr==s_nodePopupMap.end())
    {
        ATLASSERT(hwndOwner);
        CWindow owner(hwndOwner);
        CWindow wndTL = owner.GetTopLevelWindow();
        HICON ico=0;
        if(wndTL.IsWindow())
            ico=(HICON)::GetClassLong(wndTL.m_hWnd, GCL_HICON);

        CNodePopupWnd* np=new CNodePopupWnd(item);
        if(np)
        {
            s_nodePopupMap[hash]=np;
            CRect rcwnd(pos.x, 8, CW_USEDEFAULT, CW_USEDEFAULT);
            np->Create(hwndOwner, rcwnd);
            if(ico)
                np->SetIcon(ico);
        }
    }
    else
    {
        CNodePopupWnd* np=s_nodePopupMap[hash];
        np->BringWindowToTop();
        np->SetActiveWindow();
    }
}

//-----------------------------------------------------------------------------

void QBUI::UpdateNodePopups()
{
    NodePopupMap::iterator itr=s_nodePopupMap.begin();

    while(itr!=s_nodePopupMap.end())
    {
        if((*itr).second->IsWindow())
            (*itr).second->Update();		
        ++itr;
    }
}

//-----------------------------------------------------------------------------

void QBUI::DeleteNodePopups()
{
    NodePopupMap::iterator itr=s_nodePopupMap.begin();

    while(itr!=s_nodePopupMap.end())
    {
        if((*itr).second->IsWindow())
            (*itr).second->PostMessage(WM_CLOSE);		
        ++itr;
    }
}
