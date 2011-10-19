#include "StdAfx.h"
#include "..\en_us\resource.h"

#include "GraphView2.h"
#include "thread.h"
//#include "mainfrm.h"
//#include "XGMMLWorkunit.h"
//#include "GotoDlg.h"
//#include "DefinitionParser.h"
//#include "XGMMLSaxHandler.h"

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
bool CLbAxWindow::InvokeScript(const std::_tstring & proc)
{
	StringVector params;
	return InvokeScript(proc, params);
}

bool CLbAxWindow::InvokeScript(const std::_tstring & proc, const std::_tstring & param1)
{
	StringVector params;
	params.push_back(param1);
	return InvokeScript(proc, params);
}

bool CLbAxWindow::InvokeScript(const std::_tstring & proc, const std::_tstring & param1, const std::_tstring & param2)
{
	StringVector params;
	params.push_back(param1);
	params.push_back(param2);
	return InvokeScript(proc, params);
}

bool CLbAxWindow::InvokeScript(const std::_tstring & proc, const std::_tstring & param1, const std::_tstring & param2, const std::_tstring & param3)
{
	StringVector params;
	params.push_back(param1);
	params.push_back(param2);
	params.push_back(param3);
	return InvokeScript(proc, params);
}

bool CLbAxWindow::InvokeScript(const std::_tstring & proc, const StringVector & params)
{
	CComPtr<IDispatch> spScript;
	HRESULT hr = QueryControl(__uuidof(IDispatch), (void**)&spScript);
	if (spScript == NULL) 
		return false;

	//Find dispid for given function in the object
	//CComBSTR bstrMember(proc.c_str());
	DISPID dispid = GetDispID(proc);
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
	CComVariant vaResult;
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
DISPID CGraphView2::m_dispScaled = NULL;
DISPID CGraphView2::m_dispLayoutFinished = NULL;

CGraphView2::CGraphView2()
{
}

void CGraphView2::LoadGraph(const std::_tstring & xml)
{
	if (m_dispScaled == NULL)
	{
		m_dispScaled = m_wndLNGVC.GetDispID(_T("onScaled"));
		m_dispLayoutFinished = m_wndLNGVC.GetDispID(_T("onLayoutFinished"));
		ATLASSERT(m_dispScaled);
		ATLASSERT(m_dispLayoutFinished);
	}

	m_wndLNGVC.InvokeScript(_T("loadXML2"), xml);
	m_wndLNGVC.InvokeScript(_T("setMessage"), _T("Performing Layout..."));
	m_wndLNGVC.InvokeScript(_T("startLayout"), _T("twopi"));
}

HRESULT CGraphView2::GetFuncInfoFromId(const IID& iid, DISPID dispidMember, LCID lcid, _ATL_FUNC_INFO& info)
{
	if (InlineIsEqualGUID(iid, DIID_IFBComEventSource))
	{
		info.cc = CC_STDCALL;
		if (dispidMember == m_dispScaled)
		{
			info.vtReturn = VT_I4;
			info.nParams = 1;
			info.pVarTypes[0] = VT_I4;
			return S_OK;
		}
		else if (dispidMember == m_dispLayoutFinished)
		{
			info.vtReturn = VT_I4;
			info.nParams = 0;
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
			return E_FAIL;
	}
	return E_FAIL;
}

HRESULT CGraphView2::OnScaled(int val)
{
	m_view_zoom.SetPos(101 - val);
	return S_OK;
}

HRESULT CGraphView2::OnLayoutFinished()
{
	m_wndLNGVC.InvokeScript(_T("setMessage"), _T(""));
	return S_OK;
}

HRESULT CGraphView2::OnMouseDblClick(int val)
{
	int d = 0;
	return S_OK;
}

HRESULT CGraphView2::OnUnknown()
{
	ATLASSERT(false);
	return S_OK;
}

LRESULT CGraphView2::OnInitDialog(HWND /*hWnd*/, LPARAM /*lParam*/)
{
	SetMsgHandled(false);
	DlgResize_Init(false, false, 0);
	ModifyStyle(0, WS_CLIPCHILDREN);

	CRect rc(0, 0, 0, 0);
	CWindow wndPlaceholder = GetDlgItem(IDC_STATIC_PLACEHOLDER);
	wndPlaceholder.GetWindowRect(rc);
	ScreenToClient(rc);
	wndPlaceholder.DestroyWindow();

	m_wndLNGVC.Create(m_hWnd, rc, 0, WS_CHILD | WS_VISIBLE | WS_TABSTOP, NULL, IDC_STATIC_PLACEHOLDER);
	ATLASSERT(::IsWindow(m_wndLNGVC));
	CComPtr<IAxWinHostWindow> spAxWindow;
	HRESULT Hr = m_wndLNGVC.QueryHost(&spAxWindow);
	if( FAILED(Hr) ) 
		return 0;

	Hr = spAxWindow->CreateControl(CComBSTR("{2FF57548-1DE4-4F32-B133-2B3FBDDBF909}"), m_wndLNGVC.m_hWnd, NULL);
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

void CGraphView2::OnVScroll(int scrollRequest, short scrollPos, HWND hWnd)
{
	SetMsgHandled(true);
	if (hWnd == m_view_zoom.m_hWnd)
	{
		SetMsgHandled(true);
		switch(scrollRequest)
		{
		case SB_ENDSCROLL:
			m_wndLNGVC.InvokeScript(_T("setScale"), boost::lexical_cast<std::_tstring>(101 - m_view_zoom.GetPos()));
			break;
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			m_wndLNGVC.InvokeScript(_T("setScale"), boost::lexical_cast<std::_tstring>(101 - scrollPos));
			break;
		default:
			break;
		}
	}
}

LRESULT CGraphView2::OnGraphLayout(WORD wNotifyCode, WORD wID, HWND /*hWndCtl*/, BOOL& bHandled)
{
	bHandled = true;
	m_wndLNGVC.InvokeScript(_T("setMessage"), _T("Performing Layout..."));
	switch(wID)
	{
	case ID_GRAPH2_DOT:
		m_wndLNGVC.InvokeScript(_T("startLayout"), _T("dot"));
		break;
	case ID_GRAPH2_NEATO:
		m_wndLNGVC.InvokeScript(_T("startLayout"), _T("neato"));
		break;
	case ID_GRAPH2_TWOPI:
		m_wndLNGVC.InvokeScript(_T("startLayout"), _T("twopi"));
		break;
	case ID_GRAPH2_FDP:
		m_wndLNGVC.InvokeScript(_T("startLayout"), _T("fdp"));
		break;
	case ID_GRAPH2_CIRCO:
		m_wndLNGVC.InvokeScript(_T("startLayout"), _T("circo"));
		break;
	}
	return 0;
}

BOOL CGraphView2::OnForwardMsg(LPMSG Msg, DWORD nUserData)
{
	return CWindow::IsDialogMessage(Msg);
}

LRESULT CGraphView2::OnDataLoadComplete(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam)
{
	ATLASSERT(wParam && lParam);
	m_wndLNGVC.InvokeScript(_T("setMessage"), _T("Loading Data..."));
	std::_tstring * verticesXML = (std::_tstring *)wParam;
	std::_tstring * edgesXML = (std::_tstring *)lParam;
	m_wndLNGVC.InvokeScript(_T("loadXML"), *verticesXML, *edgesXML);
	m_wndLNGVC.InvokeScript(_T("setMessage"), _T("Performing Layout..."));
	m_wndLNGVC.InvokeScript(_T("startLayout"), _T("neato"));
	delete verticesXML;
	delete edgesXML;
	return 0;
}
