#include "StdAfx.h"
#include "..\en_us\resource.h"

#include "GraphView2.h"
#include "Notifications.h"
#include "thread.h"
#include "mainfrm.h"
#include "XGMMLWorkunit.h"
#include "GotoDlg.h"
#include "DefinitionParser.h"
#include "XGMMLSaxHandler.h"

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

MIDL_DEFINE_GUID(IID, DIID_IFBComEventSource,0xE0C8280C,0x4448,0x45DF,0xA7,0x50,0xF1,0xC5,0xA8,0xED,0x7C,0x40);

#define PROP_TREEITEM 11
enum GV_TIMER
{
    GV_TIMER_UNKNOWN = 0,
    GV_TIMER_REFRESH,
    GV_TIMER_LAST
};

//  ===========================================================================
bool CLbAxWindow::InvokeScript(const std::_tstring & proc, CComVariant & vaResult) const
{
    StringVector params;
    return InvokeScript(proc, params, vaResult);
}

bool CLbAxWindow::InvokeScript(const std::_tstring & proc, const std::_tstring & param1, CComVariant & vaResult) const
{
    StringVector params;
    params.push_back(param1);
    return InvokeScript(proc, params, vaResult);
}

bool CLbAxWindow::InvokeScript(const std::_tstring & proc, const std::_tstring & param1, const std::_tstring & param2, CComVariant & vaResult) const
{
    StringVector params;
    params.push_back(param1);
    params.push_back(param2);
    return InvokeScript(proc, params, vaResult);
}

bool CLbAxWindow::InvokeScript(const std::_tstring & proc, const std::_tstring & param1, const std::_tstring & param2, const std::_tstring & param3, CComVariant & vaResult) const
{
    StringVector params;
    params.push_back(param1);
    params.push_back(param2);
    params.push_back(param3);
    return InvokeScript(proc, params, vaResult);
}

bool CLbAxWindow::InvokeScript(const std::_tstring & proc, const std::_tstring & param1, const std::_tstring & param2, const std::_tstring & param3, const std::_tstring & param4, CComVariant & vaResult) const
{
    StringVector params;
    params.push_back(param1);
    params.push_back(param2);
    params.push_back(param3);
    params.push_back(param4);
    return InvokeScript(proc, params, vaResult);
}

bool CLbAxWindow::InvokeScript(const std::_tstring & proc, const StringVector & params, CComVariant & vaResult) const
{
    CLbAxWindow * self = const_cast<CLbAxWindow *>(this);
    CComPtr<IDispatch> spScript;
    HRESULT hr = self->QueryControl(__uuidof(IDispatch), (void**)&spScript);
    if (spScript == NULL) 
        return false;

    //Find dispid for given function in the object
    //CComBSTR bstrMember(proc.c_str());
    DISPID dispid = self->GetDispID(proc);
    //hr = spScript->GetIDsOfNames(IID_NULL, &bstrMember, 1, LOCALE_SYSTEM_DEFAULT, &dispid);
    if(dispid == NULL)
        return false;

    const int arraySize = params.size();
    //Putting parameters  
    DISPPARAMS dispparams;
    memset(&dispparams, 0, sizeof dispparams);
    dispparams.cArgs = arraySize;
    dispparams.rgvarg = new VARIANT[dispparams.cArgs];
    dispparams.cNamedArgs = 0;

    for(int i = 0; i < arraySize; i++)
    {
        CComBSTR bstr = params[arraySize - i - 1].c_str();
        bstr.CopyTo(&dispparams.rgvarg[i].bstrVal);
        dispparams.rgvarg[i].vt = VT_BSTR;
    }
    EXCEPINFO excepInfo;
    memset(&excepInfo, 0, sizeof excepInfo);
    UINT nArgErr = (UINT)-1;  // initialize to invalid arg
    //Call JavaScript function         
    hr = spScript->Invoke(dispid, IID_NULL, 0, DISPATCH_METHOD, &dispparams, &vaResult, &excepInfo, &nArgErr);
    delete [] dispparams.rgvarg;
    if(FAILED(hr))
        return false;

    return true;
}

DISPID CLbAxWindow::GetDispID(const std::_tstring & name)
{
    if (m_dispid.find(name) == m_dispid.end())
    {
        CComPtr<IDispatch> spScript;
        HRESULT hr = QueryControl(__uuidof(IDispatch), (void**)&spScript);
        if (FAILED(hr)) 
            return NULL;

        //Find dispid for given function in the object
        CComBSTR bstrMember(name.c_str());
        DISPID dispid = NULL;
        hr = spScript->GetIDsOfNames(IID_NULL, &bstrMember, 1, LOCALE_SYSTEM_DEFAULT, &dispid);
        if(FAILED(hr))
            return NULL;
        m_dispid[name] = dispid;
    }

    return m_dispid[name];
}

//  ===========================================================================
DISPID g_dispScaled = NULL;
DISPID g_dispLayoutFinished = NULL;
DISPID g_dispMouseDoubleClick = NULL;
DISPID g_dispSelectionChanged = NULL;

CGraphView2::CGraphView2(Dali::IWorkunit * wu, IResultSlot *resultSlot) : m_wu(wu)
{
    m_resultSlot = resultSlot;
    m_unhide = 0;
}

void DownloadData(Dali::IResult * daliResult, std::_tstring * xml)
{
    CComPtr<ITable> result = CreateISparseTable();
    daliResult->GetResultData(0, 10000, result);
    result->ToXML(_T("Dataset"), _T("Row"), *xml, false);
}

void DownloadAllData(CGraphView2 * self)
{
    clib::thread * threadV;
    clib::thread * threadE;
    std::_tstring * verticesXML = new std::_tstring;
    std::_tstring * edgesXML = new std::_tstring;
    Dali::IResultVector::const_iterator itr, end;
    boost::tie(itr, end) = self->m_wu->GetResultIterator();
    for (; itr != end; ++itr)
    {
        CComPtr<ITable> result = CreateISparseTable();
        itr->get()->GetResultData(0, 1, result);
        if (result->FindColumn(_T("vertex_id")) != -1)
            threadV = new clib::thread(__FUNCTION__, boost::bind(&DownloadData, itr->get(), verticesXML));
        else if (result->FindColumn(_T("edge_from_id")) != -1)
            threadE = new clib::thread(__FUNCTION__, boost::bind(&DownloadData, itr->get(), edgesXML));
    }
    threadV->join();
    threadE->join();
    delete threadE;
    delete threadV;
    self->PostMessage(UMGV2_DATALOAD_COMPLETE, (WPARAM)verticesXML, (LPARAM)edgesXML);
}

BOOL CGraphView2::PreTranslateMessage(MSG* pMsg)
{
    // This was stolen from an SDI app using a form view.
    //
    // Pass keyboard messages along to the child window that has the focus.
    // When the browser has the focus, the message is passed to its containing
    // CAxWindow, which lets the control handle the message.

    if((pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) &&
        (pMsg->message < WM_MOUSEFIRST || pMsg->message > WM_MOUSELAST))
        return FALSE;

    HWND hWndCtl = ::GetFocus();

    if(IsChild(hWndCtl))
    {
        // find a direct child of the dialog from the window that has focus
        while(::GetParent(hWndCtl) != m_hWnd)
            hWndCtl = ::GetParent(hWndCtl);

        // give control a chance to translate this message
        if(::SendMessage(hWndCtl, WM_FORWARDMSG, 0, (LPARAM)pMsg) != 0)
            return TRUE;
    }

    // A normal control has the focus, so call IsDialogMessage() so that
    // the dialog shortcut keys work (TAB, etc.)
    return IsDialogMessage(pMsg);
}

void CGraphView2::LoadGraph()
{
    if (IsWindow())
    {
        m_wndLNGVC.SetMessage(_T("Downloading Data..."));
        clib::thread run(__FUNCTION__, boost::bind(&DownloadAllData, this));
    }
}

HRESULT CGraphView2::GetFuncInfoFromId(const IID& iid, DISPID dispidMember, LCID lcid, _ATL_FUNC_INFO& info)
{
    if (InlineIsEqualGUID(iid, DIID_IFBComEventSource))
    {
        info.cc = CC_STDCALL;
        if (dispidMember == g_dispScaled)
        {
            info.vtReturn = VT_EMPTY;
            info.nParams = 2;
            info.pVarTypes[0] = VT_I4;
            info.pVarTypes[1] = VT_DISPATCH;
            return S_OK;
        }
        else if (dispidMember == g_dispLayoutFinished)
        {
            info.vtReturn = VT_EMPTY;
            info.nParams = 1;
            info.pVarTypes[0] = VT_DISPATCH;
            return S_OK;
        }
        else if (dispidMember == g_dispMouseDoubleClick)
        {
            info.vtReturn = VT_I4;
            info.nParams = 3;
            info.pVarTypes[0] = VT_I4;
            info.pVarTypes[1] = VT_I4;
            info.pVarTypes[2] = VT_DISPATCH;
            return S_OK;
        }
        else if (dispidMember == g_dispSelectionChanged)
        {
            info.vtReturn = VT_EMPTY;
            info.nParams = 2;
            info.pVarTypes[0] = VT_VARIANT;
            info.pVarTypes[1] = VT_DISPATCH;
            return S_OK;
        }

        //case DISP_MOUSE_DBLCLICK:
        //	info.vtReturn = VT_I4;
        //	info.nParams = 1;
        //	info.pVarTypes[0] = VT_I4;
        //	return S_OK;

        //case DISP_UNKNOWN:
        //case DISP_UNKNOWN1:
        //case DISP_UNKNOWN2:
        //case DISP_UNKNOWN3:
        //case DISP_UNKNOWN4:
        //case DISP_UNKNOWN5:
        //case DISP_UNKNOWN6:
        //case DISP_UNKNOWN7:
        //case DISP_UNKNOWN8:
        //case DISP_UNKNOWN9:
        //	info.vtReturn = VT_I4;
        //	info.nParams = 0;
        //	return S_OK;

        else
        {
            ATLASSERT(false);
            return E_FAIL;
        }
    }
    ATLASSERT(false);
    return E_FAIL;
}

HRESULT CGraphView2::OnScaled(int val, IDispatch * oSender)
{
    m_view_zoom.SetPos(101 - val);
    return S_OK;
}

HRESULT CGraphView2::OnLayoutFinished(IDispatch * oSender)
{
    m_wndLNGVC.SetMessage(_T(""));
    m_wndLNGVC.CenterGraphItem(0);
    return S_OK;
}

HRESULT CGraphView2::OnMouseDoubleClick(int val, int keyState, IDispatch * oSender)
{
    m_wndLNGVC.CenterGraphItem(val);
    return S_OK;
}

LRESULT CGraphView2::OnInitDialog(HWND /*hWnd*/, LPARAM /*lParam*/)
{
    SetMsgHandled(false);
    DlgResize_Init(false, false, 0);
    m_name = _T("Data Visualization");
    SetWindowText(m_name);
    ModifyStyle(0, WS_CLIPCHILDREN);

    CRect rc(0, 0, 0, 0);
    CWindow wndPlaceholder = GetDlgItem(IDC_STATIC_PLACEHOLDER);
    wndPlaceholder.GetWindowRect(rc);
    ScreenToClient(rc);
    wndPlaceholder.DestroyWindow();

    m_wndLNGVC.Create(m_hWnd, rc, 0, WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE | WS_TABSTOP, NULL, IDC_STATIC_PLACEHOLDER);
    ATLASSERT(::IsWindow(m_wndLNGVC));
    CComPtr<IAxWinHostWindow> spAxWindow;
    HRESULT Hr = m_wndLNGVC.QueryHost(&spAxWindow);
    if( FAILED(Hr) ) 
        return 0;

    Hr = spAxWindow->CreateControl(CComBSTR(GUID_npHPCCSystemsGraphViewControl), m_wndLNGVC.m_hWnd, NULL);
    if( FAILED(Hr) ) 
        return 0;

    CComPtr<IUnknown> spUnk;
    Hr = m_wndLNGVC.QueryControl(__uuidof(IUnknown), (void**)&spUnk);
    if( FAILED(Hr) ) 
        return 0;

    DispEventAdvise(spUnk);

    m_view_zoom = GetDlgItem(IDC_SLIDER_ZOOM);
    m_view_zoom.SetRange(1, 100);
    m_view_zoom.SetPageSize(10);
    m_view_zoom.SetPos(1);
    m_view_zoom.SetTic(1);
    m_view_zoom.SetTic(25);
    m_view_zoom.SetTic(50);
    m_view_zoom.SetTic(75);
    m_view_zoom.SetTic(100);

    m_wndLNGVC.InitDispID();

    return 1;
}

void CGraphView2::OnDestroy()
{
    SetMsgHandled(false);

    CComPtr<IUnknown> spUnk;
    HRESULT Hr = m_wndLNGVC.QueryControl(__uuidof(IUnknown), (void**)&spUnk);
    if( FAILED(Hr) ) 
        return;

    DispEventUnadvise(spUnk);
}

LRESULT CGraphView2::OnPaint(HDC /*dc*/)
{
    SetMsgHandled(FALSE);
    if (::InterlockedIncrement(&m_unhide) == 1)
    {
        m_unhide = true;
        LoadGraph();
    }
    return 1;
}

LRESULT CGraphView2::OnGraphLayout(WORD wNotifyCode, WORD wID, HWND /*hWndCtl*/, BOOL& bHandled)
{
    bHandled = true;
    m_wndLNGVC.SetMessage(_T("Performing Layout..."));
    switch(wID)
    {
    case ID_GRAPH2_DOT:
        m_wndLNGVC.StartLayout(SGV::LAYOUT_DOT);
        break;
    case ID_GRAPH2_NEATO:
        m_wndLNGVC.StartLayout(SGV::LAYOUT_NEATO);
        break;
    case ID_GRAPH2_TWOPI:
        m_wndLNGVC.StartLayout(SGV::LAYOUT_TWOPI);
        break;
    case ID_GRAPH2_FDP:
        m_wndLNGVC.StartLayout(SGV::LAYOUT_FDP);
        break;
    case ID_GRAPH2_CIRCO:
        m_wndLNGVC.StartLayout(SGV::LAYOUT_CIRCO);
        break;
    case ID_GRAPH2_SFDP:
        m_wndLNGVC.StartLayout(SGV::LAYOUT_SFDP);
        break;
    case ID_GRAPH2_OSAGE:
        m_wndLNGVC.StartLayout(SGV::LAYOUT_OSAGE);
        break;
    }
    return 0;
}

HRESULT CGraphView2::OnSelectionChanged(VARIANT selection, IDispatch * oSender)
{
    CComVariant varSel(selection);
    CComSafeArray<VARIANT> selArray = selection.parray;
    for(unsigned int i = 0; i < selArray.GetCount(); ++i)
    {
        CComVariant item = selArray.GetAt(i);
        CComSafeArray<VARIANT> properties;
        m_wndLNGVC.GetProperties(item.uintVal, properties);
    }
    return S_OK;
}

void CGraphView2::OnVScroll(int scrollRequest, short scrollPos, HWND hWnd)
{
    SetMsgHandled(false);
    if (hWnd == m_view_zoom.m_hWnd)
    {
        SetMsgHandled(true);
        switch(scrollRequest)
        {
        case SB_TOP:
        case SB_BOTTOM:
        case SB_ENDSCROLL:
        case SB_LINEDOWN:
        case SB_LINEUP:
        case SB_PAGEDOWN:
        case SB_PAGEUP:
            m_wndLNGVC.SetScale((101 - m_view_zoom.GetPos()) / 100.0f);
            break;
        case SB_THUMBPOSITION:
        case SB_THUMBTRACK:
            m_wndLNGVC.SetScale((101 - scrollPos) / 100.0f);
            break;
        default:
            break;
        }
    }
}

BOOL CGraphView2::OnForwardMsg(LPMSG Msg, DWORD nUserData)
{
    return CWindow::IsDialogMessage(Msg);
}

LRESULT CGraphView2::OnDataLoadComplete(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam)
{
    ATLASSERT(wParam && lParam);
    m_wndLNGVC.SetMessage(_T("Loading Data..."));
    std::_tstring * verticesXML = (std::_tstring *)wParam;
    std::_tstring * edgesXML = (std::_tstring *)lParam;
    m_wndLNGVC.LoadXML(*verticesXML, *edgesXML);
    m_wndLNGVC.SetMessage(_T("Performing Layout..."));
    m_wndLNGVC.StartLayout(SGV::LAYOUT_SFDP);
    delete verticesXML;
    delete edgesXML;
    return 0;
}

//CTabPane
HWND CGraphView2::GetHWND()
{
    return m_hWnd;
}

bool CGraphView2::CanCopy()
{
    return false;
}
bool CGraphView2::HasResults() const
{
    return false;
}

bool CGraphView2::UpdateUI(CCmdUI * cui)
{
    UPDATEUI(cui, ID_GRAPH2_DOT, true);
    UPDATEUI(cui, ID_GRAPH2_NEATO, true);
    UPDATEUI(cui, ID_GRAPH2_TWOPI, true);
    UPDATEUI(cui, ID_GRAPH2_FDP, true);
    UPDATEUI(cui, ID_GRAPH2_CIRCO, true);
    UPDATEUI(cui, ID_GRAPH2_SFDP, true);
    UPDATEUI(cui, ID_GRAPH2_OSAGE, true);
    return false;
}

//ISearchable
bool CGraphView2::CanBeSearched() const
{
    return HasResults();
}

bool CGraphView2::Search(const CSearchableInfo &/*info*/, CSearchableResult &/*result*/) const
{
    return false;
}

void CGraphView2::operator()(Dali::WUEvent evt, Dali::IWorkunit * wu)
{
    switch (evt)
    {
    case Dali::WUEventUpdated:
        PostMessage(UMGV2_WU_UPDATE, (WPARAM)wu);
        break;
    }
}

//  --  IResultView  ---
HWND CGraphView2::GetHwnd() const 
{
    return m_hWnd;
}
