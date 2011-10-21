#pragma once

#include "CtlColor.h"
#include <CustomMessages.h>

enum UMGV2
{
	UMGV2_FIRST = CWM_LAST + 1,
	UMGV2_WU_UPDATE, 
	UMGV2_DATALOAD_COMPLETE,
	UMGV2_LAST
};
//  ===========================================================================
typedef std::vector<std::_tstring> StringVector;
typedef std::map<std::_tstring, DISPID> StringDispIdMap;

// Latebound CAxWindow
class CLbAxWindow : public CAxWindow
{
protected:
	StringDispIdMap m_dispid;

public:
	bool InvokeScript(const std::_tstring & proc);
	bool InvokeScript(const std::_tstring & proc, const std::_tstring & param1);
	bool InvokeScript(const std::_tstring & proc, const std::_tstring & param1, const std::_tstring & param2);
	bool InvokeScript(const std::_tstring & proc, const std::_tstring & param1, const std::_tstring & param2, const std::_tstring & param3);
	bool InvokeScript(const std::_tstring & proc, const StringVector & params);

	DISPID GetDispID(const std::_tstring & name);
	const TCHAR * GetName(DISPID dispid) const;
};
//  ===========================================================================

EXTERN_C const IID DIID_IFBComEventSource;
class CGraphView2 : 
	public WTL::CAxDialogImpl<CGraphView2>,
	public WTL::CDialogResize<CGraphView2>,
	public WTL::CWinDataExchange<CGraphView2>,
	public IDispEventSimpleImpl<IDC_STATIC_PLACEHOLDER, CGraphView2, &DIID_IFBComEventSource>,
	public CCtlColor,
	public boost::signals::trackable
{
	typedef CGraphView2 thisClass;
	typedef WTL::CAxDialogImpl<CGraphView2> baseClass;

	friend void DownloadAllData(CGraphView2 * self);

protected:
	CLbAxWindow m_wndLNGVC;
	WTL::CTrackBarCtrl m_view_zoom;

public:
	enum {IDD = IDD_GRAPHVIEW};

	CGraphView2();

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		return baseClass::IsDialogMessage(pMsg);
	}

	void LoadGraph(const std::_tstring & xml);

	BEGIN_MSG_MAP(thisClass)
		MSG_WM_INITDIALOG(OnInitDialog);
		MSG_WM_DESTROY(OnDestroy);

		MSG_WM_VSCROLL(OnVScroll);

		MESSAGE_HANDLER_EX(UMGV2_DATALOAD_COMPLETE, OnDataLoadComplete);

		COMMAND_ID_HANDLER(ID_GRAPH2_DOT, OnGraphLayout);
		COMMAND_ID_HANDLER(ID_GRAPH2_NEATO, OnGraphLayout);
		COMMAND_ID_HANDLER(ID_GRAPH2_TWOPI, OnGraphLayout);
		COMMAND_ID_HANDLER(ID_GRAPH2_FDP, OnGraphLayout);
		COMMAND_ID_HANDLER(ID_GRAPH2_CIRCO, OnGraphLayout);

		MSG_WM_FORWARDMSG(OnForwardMsg)

		CHAIN_MSG_MAP(WTL::CDialogResize<thisClass>)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(thisClass)
		DLGRESIZE_CONTROL(IDC_STATIC_PLACEHOLDER, DLSZ_SIZE_X | DLSZ_SIZE_Y)
		DLGRESIZE_CONTROL(IDC_SLIDER_ZOOM, DLSZ_MOVE_X | DLSZ_SIZE_Y)
	END_DLGRESIZE_MAP()

	BEGIN_DDX_MAP(thisClass)
	END_DDX_MAP()

	static DISPID m_dispScaled;
	static DISPID m_dispLayoutFinished;

	BEGIN_SINK_MAP(thisClass)
		SINK_ENTRY_EX(IDC_STATIC_PLACEHOLDER, DIID_IFBComEventSource, m_dispScaled, OnScaled)
		SINK_ENTRY_EX(IDC_STATIC_PLACEHOLDER, DIID_IFBComEventSource, m_dispLayoutFinished, OnLayoutFinished)
	END_SINK_MAP()

	LRESULT OnInitDialog(HWND /*hWnd*/, LPARAM /*lParam*/);
	void OnDestroy();

	void OnVScroll(int scrollRequest, short scrollPos, HWND hWnd);
	LRESULT OnGraphLayout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	BOOL OnForwardMsg(LPMSG Msg, DWORD nUserData);

	LRESULT OnDataLoadComplete(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);

	HRESULT GetFuncInfoFromId(const IID& iid, DISPID dispidMember, LCID lcid, _ATL_FUNC_INFO& info);

	STDMETHOD(OnScaled)(int val);
	STDMETHOD(OnLayoutFinished)();
	STDMETHOD(OnMouseDblClick)(int val);
	STDMETHOD(OnUnknown)();
};
