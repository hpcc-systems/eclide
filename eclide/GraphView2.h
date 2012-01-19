#pragma once

#include <npHPCCSystemsGraphViewControl.h>
#include "ResultView.h"
#include "TabPane.h"
#include "atlGraphView.h"
#include "Notifications.h"
#include "CtlColor.h"
#include "XGMMLSaxHandler.h"
#include <CustomMessages.h>

//  ===========================================================================
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

protected:
	bool InvokeScript(const std::_tstring & proc, CComVariant & vaResult) const;
	bool InvokeScript(const std::_tstring & proc, const std::_tstring & param1, CComVariant & vaResult) const;
	bool InvokeScript(const std::_tstring & proc, const std::_tstring & param1, const std::_tstring & param2, CComVariant & vaResult) const;
	bool InvokeScript(const std::_tstring & proc, const std::_tstring & param1, const std::_tstring & param2, const std::_tstring & param3, CComVariant & vaResult) const;
	bool InvokeScript(const std::_tstring & proc, const std::_tstring & param1, const std::_tstring & param2, const std::_tstring & param3, const std::_tstring & param4, CComVariant & vaResult) const;
	bool InvokeScript(const std::_tstring & proc, const StringVector & params, CComVariant & vaResult) const;

	DISPID GetDispID(const std::_tstring & name);
	const TCHAR * GetName(DISPID dispid) const;
};

extern DISPID g_dispScaled;
extern DISPID g_dispLayoutFinished;
extern DISPID g_dispMouseDoubleClick;
extern DISPID g_dispSelectionChanged;

class CGraphViewCtrl2 : public CLbAxWindow
{
protected:

public:
	void InitDispID()
	{
		if (g_dispScaled == NULL)
		{
			g_dispScaled = GetDispID(_T("onScaled"));
			ATLASSERT(g_dispScaled);
			g_dispLayoutFinished = GetDispID(_T("onLayoutFinished"));
			ATLASSERT(g_dispLayoutFinished);
			g_dispMouseDoubleClick = GetDispID(_T("onMouseDoubleClick"));
			ATLASSERT(g_dispMouseDoubleClick);
			g_dispSelectionChanged = GetDispID(_T("onSelectionChanged"));
			ATLASSERT(g_dispSelectionChanged);
		}
	}

	void SetAppMode(AppMode mode)
	{
	}

	void Clear()
	{
		CComVariant vaResult;
		bool retVal = InvokeScript(_T("clear"), vaResult);
		ATLASSERT(retVal);
	}

	bool HasItems()
	{
		CComVariant vaResult;
		bool retVal = InvokeScript(_T("hasItems"), vaResult);
		return vaResult.boolVal == VARIANT_TRUE;
	}

	void SetMessage(const std::_tstring & msg)
	{
		CComVariant vaResult;
		bool retVal = InvokeScript(_T("setMessage"), msg, vaResult);
		ATLASSERT(retVal);
	}

	void LoadXML(const std::_tstring & verticesXML, const std::_tstring & edgesXML)
	{
		CComVariant vaResult;
		bool retVal = InvokeScript(_T("loadXML"), verticesXML, edgesXML, vaResult);
		ATLASSERT(retVal);
	}
	
	void LoadXGMML(const TCHAR* xgmml, WUVisualState state, bool redundantSubgraphs, bool passthroughVertices, bool minimizeIfLarge, const IXGMMLRenderCallback * callback)
	{
		CComVariant vaResult;
		bool retVal = InvokeScript(_T("loadXGMML"), xgmml, vaResult);
		ATLASSERT(retVal);
	}
	
	void MergeXGMML(const TCHAR* xgmml, WUVisualState state, bool appendMissing, bool redundantSubgraphs, bool passthroughVertices, const IXGMMLRenderCallback * callback)
	{
		CComVariant vaResult;
		bool retVal = InvokeScript(_T("mergeXGMML"), xgmml, vaResult);
		ATLASSERT(retVal);
	}

	void StartLayout(const std::_tstring & layout)
	{
		CComVariant vaResult;
		bool retVal = false;
		retVal = InvokeScript(_T("startLayout"), layout, vaResult);
		ATLASSERT(retVal);
	}

	void StartLayout(SGV::LAYOUT layout)
	{
		switch(layout)
		{
		case SGV::LAYOUT_DOT:
			StartLayout(_T("dot"));
			break;
		case SGV::LAYOUT_NEATO:
			StartLayout(_T("neato"));
			break;
		case SGV::LAYOUT_TWOPI:
			StartLayout(_T("twopi"));
			break;
		case SGV::LAYOUT_FDP:
			StartLayout(_T("fdp"));
			break;
		case SGV::LAYOUT_CIRCO:
			StartLayout(_T("circo"));
			break;
		case SGV::LAYOUT_SFDP:
			StartLayout(_T("sfdp"));
			break;
		case SGV::LAYOUT_OSAGE:
			StartLayout(_T("osage"));
			break;
		default:
			ATLASSERT(false);
			break;
		}
	}

	bool EnsureVisible(const CUniqueID & id)
	{
		return false;	//All items are visible, so no item was "exposed" so return false;
	}

	void InvalidateLayout()
	{
		bool retVal = false;
		ATLASSERT(retVal);
	}

	void SetScale(float scale)
	{
		CComVariant vaResult;
		bool retVal = InvokeScript(_T("setScale"), boost::lexical_cast<std::_tstring>(scale * 100), vaResult);
		ATLASSERT(retVal);
	}

	float GetScale()
	{
		CComVariant vaResult;
		bool retVal = InvokeScript(_T("getScale"), vaResult);
		ATLASSERT(retVal);
		return vaResult.fltVal;
	}

	void GetSubgraphs(IUniqueIDContainer * results, TRISTATE visible=TRISTATE_BOTH, TRISTATE pinned=TRISTATE_BOTH, TRISTATE selected=TRISTATE_BOTH, bool includeHidden = false) const
	{
		bool retVal = false;
		ATLASSERT(retVal);
	}

	void GetVertices(IUniqueIDContainer * results, TRISTATE visible=TRISTATE_BOTH, TRISTATE pinned=TRISTATE_BOTH, TRISTATE selected=TRISTATE_BOTH, bool includeHidden = false) const
	{
		CComVariant vaResult;
		bool retVal = InvokeScript(_T("getVertices"), vaResult);
		ATLASSERT(retVal);
		CComSafeArray<VARIANT> vertices = vaResult.parray;
		for(ULONG i = 0; i < vertices.GetCount(); ++i)
			results->Add(CUniqueID(guidDefault, XGMML_CAT_EDGE, boost::lexical_cast<std::_tstring>(vertices.GetAt(i).lVal)));
		ATLASSERT(retVal);
	}

	void GetEdges(IUniqueIDContainer * edges, TRISTATE visible=TRISTATE_BOTH, TRISTATE selected=TRISTATE_BOTH, bool includeHidden = false)
	{
		bool retVal = false;
		ATLASSERT(retVal);
	}

	int GetItem(const CUniqueID& uid)
	{
		CComVariant vaResult;
		bool retVal = InvokeScript(_T("getItem"), uid.GetID(), vaResult);
		ATLASSERT(retVal);
		return vaResult.intVal;
	}

	CComVariant GetGlobalID(int item)
	{
		CComVariant vaResult;
		bool retVal = InvokeScript(_T("getGlobalID"), boost::lexical_cast<std::_tstring>(item), vaResult);
		ATLASSERT(retVal);
		return vaResult;
	}

	void CenterGraphItem(int item, bool scaleToFit = true)
	{
		CComVariant vaResult;
		bool retVal = InvokeScript(_T("centerOnItem"), boost::lexical_cast<std::_tstring>(item), boost::lexical_cast<std::_tstring>(scaleToFit), vaResult);
		ATLASSERT(retVal);
	}

	void CenterGraphItem(const CUniqueID& uid, bool scaleToFit = true)
	{
		CenterGraphItem(GetItem(uid), scaleToFit);
	}

	void CenterVertex(const CUniqueID& uid, bool scaleToFit = true)
	{
		CenterGraphItem(GetItem(uid), scaleToFit);
	}

	IGraphSubgraph* GetSubgraph(const CUniqueID& uid)
	{
		bool retVal = false;
		ATLASSERT(retVal);
		return NULL;
	}

	CUniqueID GetRunningSubgraph()
	{
		CComVariant vaResult;
		bool retVal = InvokeScript(_T("getRunningSubgraph"), vaResult);
		if (retVal && vaResult.intVal)
		{
			CString globaID = GetGlobalID(vaResult.intVal);
			return CUniqueID(guidDefault, XGMML_CAT_SUBGRAPH, (const TCHAR *)globaID);
		}
		return CUniqueID();
	}

	bool GetProperties(int item, CComSafeArray<VARIANT> & properties) const
	{
		CComVariant vaResult;
		bool retVal = InvokeScript(_T("getProperties"), boost::lexical_cast<std::_tstring>(item), vaResult);
		if (retVal)
			properties = vaResult.parray;
		return retVal;
	}

	CComVariant GetProperty(const CUniqueID& uid, int key) const
	{
		CComVariant vaResult;
		bool retVal = InvokeScript(_T("getProperty"), uid.GetID(), boost::lexical_cast<std::_tstring>(key), vaResult);
		ATLASSERT(retVal);
		return vaResult;
	}

	CComVariant GetProperty(const CUniqueID& uid, const std::_tstring & key) const
	{
		CComVariant vaResult;
		bool retVal = InvokeScript(_T("getProperty"), uid.GetID(), key, vaResult);
		ATLASSERT(retVal);
		return vaResult;
	}

	BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
	{
		CComVariant vaResult;
		bool retVal = InvokeScript(_T("onMouseWheel"), boost::lexical_cast<std::_tstring>(nFlags), boost::lexical_cast<std::_tstring>(zDelta), boost::lexical_cast<std::_tstring>(pt.x), boost::lexical_cast<std::_tstring>(pt.y), vaResult);
		ATLASSERT(retVal);
		return TRUE;
	}

	void SetItemScaleToFit(const CUniqueID& id, float maxSize = 1.0f)
	{
		CenterGraphItem(id);
		return;
	}

	void CenterGraph()
	{
		CComVariant vaResult;
		bool retVal = InvokeScript(_T("centerOnItem"), _T("0"), _T("1"), vaResult);
		ATLASSERT(retVal);
	}

	bool SaveAs()
	{
		bool retVal = false;
		ATLASSERT(retVal);
		return retVal;
	}

	void SetScaleToFit(bool selectedOnly = false, float maxSize = 1.0f)
	{
		CenterGraph();
	}

	IGraphItem* GetGraphItem(const CUniqueID& uid)
	{
		bool retVal = false;
		ATLASSERT(retVal);
		return NULL;
	}

	IUnknown * GetPropertyUnknown(const CUniqueID& uid, int key) const
	{
		bool retVal = false;
		ATLASSERT(retVal);
		return NULL;
	}

	HWND GetHWND()
	{
		bool retVal = false;
		ATLASSERT(retVal);
		return NULL;
	}

	void SetSelected(const CUniqueID & uid, bool select, bool append)
	{
		bool retVal = false;
		ATLASSERT(retVal);
	}

	void Find(IUniqueIDContainer* results, const TCHAR* searchText, bool searchLabel = true, bool searchProperties = false, bool caseSensitive = false, bool visibleOnly = true, bool wholeWord = false, const GUID& pluginId = GUID_NULL, const int categoryId = -1, const int propertyId = -1)
	{
		bool retVal = false;
		ATLASSERT(retVal);
	}

	IGraphVertex* GetVertex(const CUniqueID& uid)
	{
		bool retVal = false;
		ATLASSERT(retVal);
		return NULL;
	}

	IGraphEdge * GetEdge(const CUniqueID & uid)
	{
		bool retVal = false;
		ATLASSERT(retVal);
		return NULL;
	}

	void GetAdjacentVertices(IUniqueIDContainer * results, const CUniqueID& uid, bool in = true, bool out = true) const
	{
		bool retVal = false;
		ATLASSERT(retVal);
	}

	void SetScaleToWidth(bool selectedOnly = false, float maxSize = 1.0f, bool centerVScroll = true)
	{
		CComVariant vaResult;
		bool retVal = InvokeScript(_T("centerOnItem"), _T("0"), _T("1"), _T("1"), vaResult);
		ATLASSERT(retVal);
	}
};
//  ===========================================================================

EXTERN_C const IID DIID_IFBComEventSource;
class CGraphView2 : 
	public WTL::CAxDialogImpl<CGraphView2>,
	public WTL::CDialogResize<CGraphView2>,
	public WTL::CWinDataExchange<CGraphView2>,
	public IDispEventSimpleImpl<IDC_STATIC_PLACEHOLDER, CGraphView2, &DIID_IFBComEventSource>,
	public CTabPane,
	public IResultView,
	public CCtlColor,
	public boost::signals::trackable
{
	typedef CGraphView2 thisClass;
	typedef WTL::CAxDialogImpl<CGraphView2> baseClass;

	friend void DownloadAllData(CGraphView2 * self);

protected:
	CComPtr<Dali::IWorkunit> m_wu;
	IResultSlot *m_resultSlot;

	CGraphViewCtrl2 m_wndLNGVC;
	WTL::CTrackBarCtrl m_view_zoom;

	LONG m_unhide;

public:
	enum {IDD = IDD_GRAPHVIEW};

	CGraphView2(Dali::IWorkunit * wu, IResultSlot *resultSlot);

	BOOL PreTranslateMessage(MSG* pMsg);

	void LoadGraph();

	BEGIN_CUNKNOWN
		IMPLEMENT_INTERFACE(IResultView)
	END_CUNKNOWN(CTabPane)

	BEGIN_MSG_MAP(thisClass)
		MSG_WM_INITDIALOG(OnInitDialog);
		MSG_WM_DESTROY(OnDestroy);

		MSG_WM_PAINT(OnPaint);

		MSG_WM_VSCROLL(OnVScroll);
		MSG_WM_FORWARDMSG(OnForwardMsg)

		MESSAGE_HANDLER_EX(UMGV2_DATALOAD_COMPLETE, OnDataLoadComplete);

		COMMAND_ID_HANDLER(ID_GRAPH2_DOT, OnGraphLayout);
		COMMAND_ID_HANDLER(ID_GRAPH2_NEATO, OnGraphLayout);
		COMMAND_ID_HANDLER(ID_GRAPH2_TWOPI, OnGraphLayout);
		COMMAND_ID_HANDLER(ID_GRAPH2_FDP, OnGraphLayout);
		COMMAND_ID_HANDLER(ID_GRAPH2_CIRCO, OnGraphLayout);
		COMMAND_ID_HANDLER(ID_GRAPH2_SFDP, OnGraphLayout);
		COMMAND_ID_HANDLER(ID_GRAPH2_OSAGE, OnGraphLayout);

		CHAIN_MSG_MAP(WTL::CDialogResize<thisClass>)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(thisClass)
		DLGRESIZE_CONTROL(IDC_STATIC_PLACEHOLDER, DLSZ_SIZE_X | DLSZ_SIZE_Y)
		DLGRESIZE_CONTROL(IDC_SLIDER_ZOOM, DLSZ_MOVE_X | DLSZ_SIZE_Y)
	END_DLGRESIZE_MAP()

	BEGIN_DDX_MAP(thisClass)
	END_DDX_MAP()

	BEGIN_SINK_MAP(thisClass)
		SINK_ENTRY_EX(IDC_STATIC_PLACEHOLDER, DIID_IFBComEventSource, g_dispScaled, OnScaled)
		SINK_ENTRY_EX(IDC_STATIC_PLACEHOLDER, DIID_IFBComEventSource, g_dispLayoutFinished, OnLayoutFinished)
		SINK_ENTRY_EX(IDC_STATIC_PLACEHOLDER, DIID_IFBComEventSource, g_dispMouseDoubleClick, OnMouseDoubleClick)
		SINK_ENTRY_EX(IDC_STATIC_PLACEHOLDER, DIID_IFBComEventSource, g_dispSelectionChanged, OnSelectionChanged)
	END_SINK_MAP()

	LRESULT OnInitDialog(HWND /*hWnd*/, LPARAM /*lParam*/);
	void OnDestroy();
	LRESULT OnPaint(HDC /*dc*/);
	void OnVScroll(int scrollRequest, short scrollPos, HWND hWnd);
	LRESULT OnGraphLayout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	BOOL OnForwardMsg(LPMSG Msg, DWORD nUserData);

	LRESULT OnDataLoadComplete(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);

	HRESULT GetFuncInfoFromId(const IID& iid, DISPID dispidMember, LCID lcid, _ATL_FUNC_INFO& info);
	STDMETHOD(OnScaled)(int val, IDispatch * oSender);
	STDMETHOD(OnLayoutFinished)(IDispatch * oSender);
	STDMETHOD(OnMouseDoubleClick)(int item, IDispatch * oSender);
	STDMETHOD(OnSelectionChanged)(VARIANT selection, IDispatch * oSender);

	//CTabPane
	virtual HWND GetHWND();
	bool CanCopy();
	bool HasResults() const;
	bool UpdateUI(CCmdUI * cui);

	//ISearchable
	bool CanBeSearched() const;
	bool Search(const CSearchableInfo &/*info*/, CSearchableResult &/*result*/) const;

	//Callbacks
	void operator()(Dali::WUEvent evt, Dali::IWorkunit * wu);

	//  ---  IResultView  ---
	HWND GetHwnd() const; 
};
