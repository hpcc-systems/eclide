// GraphViewControl.h : Declaration of the CGraphViewControl
#pragma once
#include "resource.h"       // main symbols
#include ".\vertex.h"
#include ".\edge.h"
#include ".\subgraph.h"
#include ".\hourglass.h"
#include <atlctl.h>
#include "GraphContainer.h"
#include "GraphViewContainer.h"
#include "GraphViewCtl.h"
#include "DSeisintGraphControlEvents_CP.H"
#include "util.h"
#include "XmlAnbSerializer.h"
#include "atlGraphView.h"
#include "GraphViewCtlApi.h"

__interface IVertices;

// CGraphViewControl
class ATL_NO_VTABLE CGraphViewControl : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IGraphViewControl, &IID_IGraphViewControl>,
	public IPersistStreamInitImpl<CGraphViewControl>,
	public IOleControlImpl<CGraphViewControl>,
	public IOleObjectImpl<CGraphViewControl>,
	public IOleInPlaceActiveObjectImpl<CGraphViewControl>,
	public IViewObjectExImpl<CGraphViewControl>,
	public IOleInPlaceObjectWindowlessImpl<CGraphViewControl>,
	public IConnectionPointContainerImpl<CGraphViewControl>,
	public CProxyDSeisintGraphControlEvents<CGraphViewControl>,
	public IObjectWithSiteImpl<CGraphViewControl>,
	public IPersistStorageImpl<CGraphViewControl>,
	public ISpecifyPropertyPagesImpl<CGraphViewControl>,
	public IQuickActivateImpl<CGraphViewControl>,
	public IDataObjectImpl<CGraphViewControl>,
	public IPropertyNotifySinkCP<CGraphViewControl>,
	public IProvideClassInfo2Impl<&CLSID_GraphViewControl, &__uuidof(DSeisintGraphControlEvents)>,
	public CComCoClass<CGraphViewControl, &CLSID_GraphViewControl>,
	public CComCompositeControl<CGraphViewControl>,
	public IObjectSafetyImpl<CGraphViewControl, INTERFACESAFE_FOR_UNTRUSTED_DATA | INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
	public CDialogResize<CGraphViewControl>,
	public SGV::IGraphViewSlot
{
protected:
	CUniqueID m_centerOn;

public:
	CGraphViewControl() : m_currentLayoutType(LayoutNone), m_bInitialized(false), m_meta(NULL)
	{
		m_bWindowOnly = TRUE;
		CalcExtent(m_sizeExtent);
	}

	DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE | 
	OLEMISC_CANTLINKINSIDE | 
		OLEMISC_INSIDEOUT | 
		OLEMISC_ACTIVATEWHENVISIBLE | 
		OLEMISC_SETCLIENTSITEFIRST
		)

		DECLARE_REGISTRY_RESOURCEID(IDR_GRAPHVIEWCONTROL)
	DECLARE_AGGREGATABLE(CGraphViewControl)

	BEGIN_COM_MAP(CGraphViewControl)
		COM_INTERFACE_ENTRY(IGraphViewControl)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(IViewObjectEx)
		COM_INTERFACE_ENTRY(IViewObject2)
		COM_INTERFACE_ENTRY(IViewObject)
		COM_INTERFACE_ENTRY(IOleInPlaceObjectWindowless)
		COM_INTERFACE_ENTRY(IOleInPlaceObject)
		COM_INTERFACE_ENTRY2(IOleWindow, IOleInPlaceObjectWindowless)
		COM_INTERFACE_ENTRY(IOleInPlaceActiveObject)
		COM_INTERFACE_ENTRY(IOleControl)
		COM_INTERFACE_ENTRY(IOleObject)
		COM_INTERFACE_ENTRY(IPersistStreamInit)
		COM_INTERFACE_ENTRY2(IPersist, IPersistStreamInit)
		COM_INTERFACE_ENTRY(IConnectionPointContainer)
		COM_INTERFACE_ENTRY(ISpecifyPropertyPages)
		COM_INTERFACE_ENTRY(IQuickActivate)
		COM_INTERFACE_ENTRY(IPersistStorage)
		COM_INTERFACE_ENTRY(IDataObject)
		COM_INTERFACE_ENTRY(IProvideClassInfo)
		COM_INTERFACE_ENTRY(IProvideClassInfo2)
		COM_INTERFACE_ENTRY(IObjectWithSite)
		COM_INTERFACE_ENTRY(IObjectSafety)
	END_COM_MAP()

	BEGIN_PROP_MAP(CGraphViewControl)
		//PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
		//PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
		//PROP_ENTRY_TYPE("Appearance", DISPID_APPEARANCE, CLSID_NULL)
		//PROP_ENTRY_TYPE("AutoSize", DISPID_AUTOSIZE, CLSID_NULL)
		//PROP_ENTRY_TYPE("BackColor", DISPID_BACKCOLOR, CLSID_StockColorPage)
		//PROP_ENTRY_TYPE("BackStyle", DISPID_BACKSTYLE, CLSID_NULL)
		//PROP_ENTRY_TYPE("BorderColor", DISPID_BORDERCOLOR, CLSID_StockColorPage)
		//PROP_ENTRY_TYPE("BorderStyle", DISPID_BORDERSTYLE, CLSID_NULL)
		//PROP_ENTRY_TYPE("Valid", DISPID_VALID, CLSID_NULL)
		// Example entries
		// PROP_ENTRY_TYPE("Property Description", dispid, clsid)
		// PROP_PAGE(CLSID_StockColorPage)
	END_PROP_MAP()

	BEGIN_MSG_MAP(CGraphViewControl)
		MSG_WM_INITDIALOG(OnInitDialog)
		NOTIFY_CODE_HANDLER(SCW_NOTIFYMINCLICKED, OnNotifyMinClicked)
		NOTIFY_CODE_HANDLER(SCW_NOTIFYLAYOUTFINISHED, OnNotifyLayoutFinished)
		CHAIN_MSG_MAP(CComCompositeControl<CGraphViewControl>)
		CHAIN_MSG_MAP(CDialogResize<CGraphViewControl>)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(CGraphViewControl)
		DLGRESIZE_CONTROL(IDC_STATIC_PLACEHOLDER, DLSZ_SIZE_X | DLSZ_SIZE_Y)
	END_DLGRESIZE_MAP()

	// Handler prototypes:
	//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	BEGIN_SINK_MAP(CGraphViewControl)
		//Make sure the Event Handlers have __stdcall calling convention
	END_SINK_MAP()

	BEGIN_CONNECTION_POINT_MAP(CGraphViewControl)
		CONNECTION_POINT_ENTRY(IID_IPropertyNotifySink)
		CONNECTION_POINT_ENTRY(__uuidof(DSeisintGraphControlEvents))
	END_CONNECTION_POINT_MAP()

	STDMETHOD(OnAmbientPropertyChange)(DISPID dispid)
	{
		if (dispid == DISPID_AMBIENT_BACKCOLOR)
		{
			SetBackgroundColorFromAmbient();
			FireViewChange();
		}
		else if (dispid == DISPID_AMBIENT_UIDEAD)
		{
			// UIDEAD state changed, check if UI active now
			BOOL bUIDead;
			if(SUCCEEDED(GetAmbientUIDead(bUIDead)))
			{
			}
		}
		return IOleControlImpl<CGraphViewControl>::OnAmbientPropertyChange(dispid);
	}
	// IViewObjectEx
	DECLARE_VIEW_STATUS(VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE)

	// IGraphViewControl

	enum { IDD = IDD_GRAPHVIEWCONTROL };

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	void FinalRelease() 
	{
		if (m_meta)
			delete m_meta;
	}

protected:
	SGV::CGraphViewCtrl m_view; 
	SGV::CGraphMeta* m_meta;
	LayoutType m_currentLayoutType;
	bool m_bInitialized;

	LRESULT OnInitDialog(HWND hWnd, LPARAM lParam)
	{
		SetMsgHandled(false);
		DlgResize_Init(false,true,0);

		CRect rc(0, 0, 0, 0);
		CWindow wndPlaceholder = GetDlgItem(IDC_STATIC_PLACEHOLDER);
		wndPlaceholder.GetWindowRect(rc);
		ScreenToClient(rc);
		wndPlaceholder.DestroyWindow();
		m_view.Create(m_hWnd, rc, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, WS_EX_CLIENTEDGE, 0U, ATL::_AtlBaseModule.GetModuleInstance());
		m_view.SetDlgCtrlID(IDC_STATIC_PLACEHOLDER);
		m_view.SetOwner(this);
		m_view.ShowOverview(true);
		m_meta = new SGV::CGraphMeta(m_view.GetHWND());
		m_bInitialized = true;
		return 0;
	}

	LRESULT OnNotifyMinClicked(WPARAM /*wParam*/, LPNMHDR lpnmhdr, BOOL& /*bHandled*/)
	{
		LPNMSGVHDR notification = (LPNMSGVHDR)lpnmhdr;
		//if (m_bInitialized)
		//	Fire_OnSubgraphMinClick(CreateSubgraph(notification->id));

		if (m_view.GetSubgraph(notification->id))
		{
			m_centerOn = notification->id;
			SGV::CGraphSubgraph sg(m_view, notification->id);
			sg.SetMinimized(!sg.GetMinimized());
			m_view.SetLayout(m_view.GetLayout());
		}
		return 0;
	}

	LRESULT OnNotifyLayoutFinished(WPARAM /*wParam*/, LPNMHDR lpnmhdr, BOOL& /*bHandled*/)
	{
		LPNMSGVHDR notification = (LPNMSGVHDR)lpnmhdr;
		//if (m_bInitialized)
		//	Fire_OnSubgraphMinClick(CreateSubgraph(notification->id));

		if (!m_centerOn.isEmpty())
		{
			m_view.CenterGraphItem(m_centerOn, m_view.GetScale());
			m_centerOn = CUniqueID();
			return 1;
		}
		return 0;
	}

protected:
	ISubgraph* CreateSubgraph(const CUniqueID& id) {
		CComObject<CSubgraph>* pSubgraph;
		CComObject<CSubgraph>::CreateInstance(&pSubgraph);
		pSubgraph->AddRef();
		pSubgraph->Init(id, &m_view);
		return pSubgraph;
	}
	IVertex* CreateVertex(const CUniqueID& id) {
		CComObject<CVertex>* pVertex;
		CComObject<CVertex>::CreateInstance(&pVertex);
		pVertex->AddRef();
		pVertex->Init(id, &m_view);
		return pVertex;
	}
public:
	// IGraphViewSlot
	virtual void OnContextMenu(const CPoint& pos) {
		if (m_bInitialized)
			Fire_OnGraphContextMenu(pos.x, pos.y);
	}
	virtual void OnDoubleClick(const CPoint& pos) {
		if (m_bInitialized)
			Fire_OnGraphDoubleClick(pos.x, pos.y);
	}

	virtual void OnDrop(HGLOBAL hg)
	{
		::GlobalUnlock(hg);
	}
	virtual void OnOver(const CPoint& pos)
	{
		if (m_bInitialized)
			Fire_OnGraphMouseOver(pos.x, pos.y);
	}

	virtual void OnDoDragDrop(const CPoint& dragStartPos)
	{
	}

	virtual bool OnLayoutFinished()
	{
		if (m_bInitialized)
			return Fire_OnLayoutFinished() != 0;
		return 0;
	}

	virtual void OnMetaLoaded()
	{
		if (m_bInitialized)
			Fire_OnMetaDataLoaded();
	}

	virtual void OnScaleChanged()
	{
	}

	virtual void OnSpacingChanged()
	{
	}

	virtual void OnSelected()
	{
	}

	virtual void SetStatus(const TCHAR* status)
	{
	}

	virtual void OnVertexClick(const CUniqueID &id, const CPoint& pos)
	{
		if (m_bInitialized)
			Fire_OnVertexSingleClick(pos.x, pos.y, CreateVertex(id));
	}

	virtual void OnVertexContextMenu(const CUniqueID &id, const CPoint& pos)
	{
		if (m_bInitialized)
			Fire_OnVertexContextMenu(pos.x, pos.y, CreateVertex(id));
	}
	
	virtual void OnVertexDoubleClick(const CUniqueID &id, const CPoint& pos)
	{
		if (m_bInitialized)
			Fire_OnVertexDoubleClick(pos.x, pos.y, CreateVertex(id));
	}
	
	virtual void OnVertexOver(const CUniqueID &id, const CPoint& pos)
	{
		if (m_bInitialized)
			Fire_OnVertexMouseOver(pos.x, pos.y, CreateVertex(id));
	}

	void OnVertexGotFocus(const CUniqueID &id)
	{
		// TODO: expose to ActiveX?
	}

	void OnVertexLostFocus(const CUniqueID &id)
	{
		// TODO: expose to ActiveX?
	}

	void OnVertexSelectionChanged(const CUniqueID &id)
	{
		// TODO: expose to ActiveX?
	}

	void OnVertexVisibilityChanged(const CUniqueID &id)
	{
		// TODO: expose to ActiveX?
	}
	
	void OnVertexCategoryVisibilityChanged(const CCategory &id)
	{
		// TODO: expose to ActiveX?
	}

	virtual void OnEdgeContextMenu(const CUniqueID &id, const CPoint& pos)
	{
		//if (m_bInitialized)
		//	Fire_OnEdgeContextMenu(pos.x, pos.y, CreateEdge(id));
	}
	virtual void OnEdgeDoubleClick(const CUniqueID &id, const CPoint& pos) {
		//if (m_bInitialized)
		//	Fire_OnEdgeDoubleClick(pos.x, pos.y, CreateEdge(id));
	}
	virtual void OnEdgeClick(const CUniqueID &id, const CPoint& pos)
	{
		//if (m_bInitialized)
		//	Fire_OnEdgeSingleClick(pos.x, pos.y, CreateEdge(id));
	}

	virtual void OnSubgraphClick(const CUniqueID &id, const CPoint& pos)
	{
	}

	virtual void OnSubgraphContextMenu(const CUniqueID &id, const CPoint& pos)
	{

	}

	virtual void OnSubgraphDoubleClick(const CUniqueID &id, const CPoint& pos)
	{

	}

	virtual void OnSubgraphGotFocus(const CUniqueID &id)
	{

	}

	virtual void OnSubgraphLostFocus(const CUniqueID &id)
	{

	}

	virtual void OnSubgraphOver(const CUniqueID &id, const CPoint& pos)
	{

	}

	virtual void OnSubgraphSelectionChanged(const CUniqueID &id)
	{

	}

	virtual void OnSubgraphVisibilityChanged(const CUniqueID &/*id*/)
	{

	}

	virtual void OnSubgraphCategoryVisibilityChanged(const CCategory &/*id*/) 
	{
	
	}

	virtual void OnTooltipLinkClick(const std::_tstring & linkAddress)
	{
	}


	// IMetaDataProperties
	STDMETHOD(GetVertexXmlName)(long category, BSTR* retVal) {
		*retVal = NULL;
		if (m_view.IsWindow()) {
			std::_tstring val;
			m_view.GetMetaVertexXmlCatCode(CCategory(guidDefault, category), val);
			if (val.length())
				*retVal = ::SysAllocString(CT2W(val.c_str(), CP_UTF8));
		}
		return S_OK;
	}
	STDMETHOD(GetVertexXmlAnbIconFile)(long category, BSTR* retVal) {
		*retVal = NULL;
		if (m_view.IsWindow()) {
			std::_tstring val;
			m_view.GetMetaVertexXmlAnbIconFile(CCategory(guidDefault, category), val);
			if (val.length())
				*retVal = ::SysAllocString(CT2W(val.c_str(), CP_UTF8));
		}
		return S_OK;
	}
	STDMETHOD(SetVertexImage)(long category, IHTMLElementRender* element, ImagePosition pos, long width, long height, long marginX, long marginY, long overlay)
	{
		if (m_view.IsWindow())
		{
			CComPtr<GraphTypes::IBitmap> ibmp=GetIBitmapFromIHTMLElement(element);
			m_meta->SetVertexImage(CCategory(guidDefault, category), ibmp, (IMAGE_POSITION)pos, marginX, marginY, (unsigned int)overlay);
		}
		return S_OK;
	}
	STDMETHOD(SetEdgeImage)(long category, IHTMLElementRender* element, ImagePosition pos, long width, long height, long marginX, long marginY, long overlay)
	{
		if (m_view.IsWindow())
		{
			CComPtr<GraphTypes::IBitmap> ibmp=GetIBitmapFromIHTMLElement(element);
			m_meta->SetEdgeImage(CCategory(guidDefault, category), ibmp, (IMAGE_POSITION)pos, marginX, marginY, (unsigned int)overlay);
		}
		return S_OK;
	}
	STDMETHOD(SetVertexCategoryLabel)(long category, BSTR label) {
		if (m_view.IsWindow())
			m_view.SetMetaVertexLabel(CCategory(guidDefault, category), CW2T(label, CP_UTF8));
		return S_OK;
	}
	STDMETHOD(SetVertexXmlData)(long category, BSTR name, BSTR anbIconFile) {
		if (m_view.IsWindow())
			m_view.SetMetaVertexXmlData(CCategory(guidDefault, category), CW2T(name, CP_UTF8), CW2T(anbIconFile, CP_UTF8));
		return S_OK;
	}
	STDMETHOD(SetVertexColors)(long category, ULONG NormalFill, ULONG BorderFill, ULONG SelectedFill, ULONG SelectedBorder, ULONG OverFill, ULONG OverBorder) {
		if (m_view.IsWindow())
			m_view.SetMetaVertexColor(CCategory(guidDefault, category), NormalFill, BorderFill, SelectedFill, SelectedBorder, OverFill, OverBorder);
		return S_OK;
	}
	STDMETHOD(SetVertexNormalBorderColor)(long category, ULONG Color) {
		if (m_view.IsWindow())
			m_view.SetMetaVertexColorBorderNorm(CCategory(guidDefault, category), Color);
		return S_OK;
	}
	STDMETHOD(SetVertexNormalFillColor)(long category, ULONG Color) {
		if (m_view.IsWindow())
			m_view.SetMetaVertexColorFillNorm(CCategory(guidDefault, category), Color);
		return S_OK;
	}
	STDMETHOD(SetVertexSelectedBorderColor)(long category, ULONG Color) {
		if (m_view.IsWindow())
			m_view.SetMetaVertexColorBorderSel(CCategory(guidDefault, category), Color);
		return S_OK;
	}
	STDMETHOD(SetVertexSelectedFillColor)(long category, ULONG Color) {
		if (m_view.IsWindow())
			m_view.SetMetaVertexColorFillSel(CCategory(guidDefault, category), Color);
		return S_OK;
	}
	STDMETHOD(SetVertexOverFillColor)(long category, ULONG Color) {
		if (m_view.IsWindow())
			m_view.SetMetaVertexColorFillOver(CCategory(guidDefault, category), Color);
		return S_OK;
	}
	STDMETHOD(SetVertexOverBorderColor)(long category, ULONG Color) {
		if (m_view.IsWindow())
			m_view.SetMetaVertexColorBorderOver(CCategory(guidDefault, category), Color);
		return S_OK;
	}
	STDMETHOD(SetVerticesVisibleByEdgeCount)(long edgeCount, BOOL visible) {
		if (m_view.IsWindow())
			// TODO: perhaps the AxControl client will also want to know what nodes where affected?
			m_view.SetVerticesVisibleByEdgeCount(NULL, edgeCount, visible != FALSE);
		return S_OK;
	}
	STDMETHOD(SetEdgeColors)(long category, ULONG Normal, ULONG ShortestPath) {
		if (m_view.IsWindow())
		{
			if ((Normal & 0xff000000) == 0)
			{
				Normal |= 0xff000000; // add alpha
			}
			m_view.SetMetaEdgeColors(CCategory(guidDefault, category), Normal, ShortestPath);
		}
		return S_OK;
	}
	STDMETHOD(SetEdgeCategoryLabel)(long category, BSTR label) {
		if (m_view.IsWindow())
			m_view.SetMetaEdgeLabel(CCategory(guidDefault, category), CW2T(label, CP_UTF8));
		return S_OK;
	}
	STDMETHOD(SetEdgeNormalColor)(long category, ULONG Color) {
		if (m_view.IsWindow())
			m_view.SetMetaEdgeColorNorm(CCategory(guidDefault, category), Color);
		return S_OK;
	}
	STDMETHOD(SetEdgeShortestPathColor)(long category, ULONG Color) {
		if (m_view.IsWindow())
			m_view.SetMetaEdgeColorShortest(CCategory(guidDefault, category), Color);
		return S_OK;
	}
	STDMETHOD(SetEdgeWidth)(long category, short Width) {
		if (m_view.IsWindow())
			m_view.SetMetaEdgeWidth(CCategory(guidDefault, category), Width);
		return S_OK;
	}
	STDMETHOD(SetEdgeXmlCategoryCode)(long category, BSTR categoryCode) {
		if (m_view.IsWindow())
			m_view.SetMetaEdgeXmlData(CCategory(guidDefault, category), CW2T(categoryCode, CP_UTF8));
		return S_OK;
	}

	// IGraphViewControl
	STDMETHOD(AddVertexSimple)(long category, BSTR identity, BSTR label, IVertex** vertex) {
		if (m_view == NULL)
			return E_FAIL;

		CUniqueID uniqueID(guidDefault, category, std::_tstring(CW2T(identity, CP_UTF8)));
		m_view.AddVertexNoPos(uniqueID, CW2T(label, CP_UTF8));
		*vertex = CreateVertex(uniqueID);
		return S_OK;
	}
	STDMETHOD(AddVertex)(long category, BSTR identity, BSTR label, long x, long y, IVertex** vertex) {
		if (m_view == NULL)
			return E_FAIL;
		// TODO: check return?
		CUniqueID uniqueID(guidDefault, category, std::_tstring(CW2T(identity, CP_UTF8)));
		m_view.AddVertex(uniqueID, CW2T(label, CP_UTF8), true, (int)x, (int)y);
		*vertex = CreateVertex(uniqueID);
		return S_OK;
	}
	STDMETHOD(GetVertex)(long category, BSTR identity, IVertex** vertex) {
		if (m_view.IsWindow() == FALSE)
			return E_FAIL;
		// later becomes a vertex exists call
		CUniqueID uniqueID(guidDefault, category, std::_tstring(CW2T(identity, CP_UTF8)));
		CComPtr<IGraphVertex> pVertex = m_view.GetVertex(uniqueID);
		if (pVertex == NULL) 
		{
			*vertex = NULL;
			return S_FALSE;
		}
		*vertex = CreateVertex(uniqueID);
		return S_OK;
	}
	STDMETHOD(AddEdge)(long category, IVertex* from, IVertex* to, BSTR label, IEdge** edge)
	{
		if (m_view == NULL)
			return E_FAIL;

		long fromCat, toCat;
		bstr_t fromId, toId;

		from->get_Category(&fromCat);
		from->get_Identity(fromId.GetAddress());
		CUniqueID fromUID(guidDefault, fromCat, (TCHAR*)fromId);

		to->get_Category(&toCat);
		to->get_Identity(toId.GetAddress());
		CUniqueID toUID(guidDefault, toCat, (TCHAR*)toId);

		CUniqueID edge_id = SGV::CreateEdgeID(CCategory(guidDefault, category), fromUID, toUID);
		m_view.AddEdge(edge_id, fromUID, toUID, CW2T(label, CP_UTF8));
		CComObject<CEdge>* pEdge;
		CComObject<CEdge>::CreateInstance(&pEdge);
		pEdge->AddRef();
		pEdge->Init(edge_id, &m_view);
		*edge = pEdge;
		return S_OK;
	}

	STDMETHOD(GetEdge)(long category, IVertex* from, IVertex* to, IEdge** edge)
	{
		if (m_view == NULL)
			return E_FAIL;
		_ASSERTE(m_view.IsWindow());

		long fromCat, toCat;
		bstr_t fromId, toId;

		from->get_Category(&fromCat);
		from->get_Identity(fromId.GetAddress());
		to->get_Category(&toCat);
		to->get_Identity(toId.GetAddress());

		CUniqueID id = SGV::CreateEdgeID(CCategory(guidDefault, category), CUniqueID(guidDefault, fromCat, (TCHAR*)fromId), CUniqueID(guidDefault, toCat, (TCHAR*)toId));
		bool exists = m_view.HasEdge(id);
		if (!exists) 
		{
			*edge = NULL;
			return S_FALSE;
		}
		CComObject<CEdge>* pEdge;
		CComObject<CEdge>::CreateInstance(&pEdge);
		pEdge->AddRef();
		pEdge->Init(id, &m_view);
		*edge = pEdge;
		return S_OK;
	}

	STDMETHOD(Clear)() {
		if (m_view.IsWindow())
			m_view.Clear();
		return S_OK;
	}
	STDMETHOD(GenerateRandomData)() {
		if (m_view.IsWindow() == FALSE)
			return E_FAIL;

#if _DEBUG
		m_view.AddRandom(20, 3);
#else
		m_view.AddRandom(100, 5);
#endif
		return S_OK;
	}
	STDMETHOD(GetRenderExtent)(OLE_HANDLE hDC, double * pX, double * pY, double * pWidth, double * pHeight) {
		*pWidth = *pHeight = 0.0;
		if (m_view != NULL)
		{
			GraphTypes::RectF r = m_view.GetRenderExtent((HDC)hDC);
			*pX = r.x;
			*pY = r.y;
			*pWidth = r.Width;
			*pHeight = r.Height;
			ATLTRACE(_T("GetRenderExtent:  %f, %f, %f, %f\n"), r.x, r.y, r.Width, r.Height);
		}
		return S_OK;
	}

	STDMETHOD(GetSelectedVertices)(IVertices **collection);

	STDMETHOD(HideCategory)(long category) {
		if (m_view.IsWindow())
			m_view.SetMetaVertexHide(CCategory(guidDefault, category), true);
		return S_OK;
	}

	STDMETHOD(LoadXml)(BSTR xml)
	{
		m_view.LoadXml(CW2T(xml, CP_UTF8));
		return S_OK;
	}

	STDMETHOD(LoadXmlFile)()
	{
		// uses a file dialog to help web devs
		m_view.LoadXmlFile();
		return S_OK;
	}

	STDMETHOD(SaveXml)(BOOL saveImages /*=FALSE*/, BSTR* retVal)
	{
		std::_tstring xml;
		m_view.SaveToXml(xml, saveImages ? SGV::XML_SAVE_FORMAT_ANB_COMPATIBLE : SGV::XML_SAVE_FORMAT_WITH_IMAGES);
		*retVal = bstr_t(xml.c_str()).copy();
		return S_OK;
	}

	STDMETHOD(SaveXmlAs)()
	{
		// uses a file dialog to help web devs
		m_view.SaveToXmlFile();
		return S_OK;
	}

	STDMETHOD(GetContainedGraphView)(IUnknown* *pVal) {
		return E_NOTIMPL;
	}
	STDMETHOD(get_Layout)(LayoutType* pVal) {
		*pVal = m_currentLayoutType;
		return S_OK;
	}
	STDMETHOD(put_Layout)(LayoutType newVal) {
		m_currentLayoutType = newVal;
		if (m_view.IsWindow())
			m_view.SetLayout((SGV::LAYOUT)m_currentLayoutType);
		return S_OK;
	}
	STDMETHOD(get_MouseMode)(MouseModes* pVal) {
		*pVal = MouseModeGrab;
		if (m_view.IsWindow())
			*pVal = (MouseModes) m_view.GetMouseMode();
		return S_OK;
	}
	STDMETHOD(put_MouseMode)(MouseModes newVal) {
		if (m_view.IsWindow())
			m_view.SetMouseMode((MOUSE_MODES)newVal);
		return S_OK;
	}
	STDMETHOD(Redraw)() {
		if (m_view.IsWindow())
			m_view.InvalidateLayout();
		return S_OK;
	}
	STDMETHOD(RenderToClipboard)() {
		if (m_view.IsWindow())
			m_view.RenderToClipboard();
		return S_OK;
	}
	STDMETHOD(get_Rotation)(long* pVal) {
		*pVal = 0;
		if (m_view.IsWindow())
			*pVal = (long)(m_view.GetRotation());
		return S_OK;
	}
	STDMETHOD(put_Rotation)(long newVal) {
		if (newVal < 0)
			newVal = 0;
		else if (newVal > 360)
			newVal = 360;
		if (m_view.IsWindow())
			m_view.SetRotation(static_cast<float>(newVal));
		return S_OK;
	}
	STDMETHOD(Save)(BSTR filename)
	{
		if (m_view.IsWindow())
			m_view.Save(bstr_t(filename));
		return S_OK;
	}
	STDMETHOD(SaveAs)()
	{
		if (m_view.IsWindow())
			m_view.SaveAs();
		return S_OK;
	}
	STDMETHOD(SetSelectedVertex)(long category, BSTR identity, BOOL bFocus, BOOL bAppend) {
		// NB: this is set focus not selected, and bFocus is ignored
		if (m_view.IsWindow())
			m_view.SetSelected(CUniqueID(guidDefault,category,(LPCTSTR)CW2T(identity, CP_UTF8)), bFocus != FALSE, bAppend != FALSE);
		return S_OK;
	}
	STDMETHOD(EnsureVisible)(long category, BSTR identity) {
		if (m_view.IsWindow())
		{
			CUniqueID id(guidDefault, category, (LPCTSTR)CW2T(identity, CP_UTF8));
			if (m_view.EnsureVisible(id))
			{
				m_centerOn = id;
				m_view.SetLayout(m_view.GetLayout());
			}
		}
		return S_OK;
	}
	STDMETHOD(SetVertexVisible)(long category, BOOL visible) {
		if (m_view.IsWindow())
			m_view.SetMetaVertexHide(CCategory(guidDefault, category), visible == FALSE);
		return S_OK;
	}
	STDMETHOD(SetVertexImageVisible)(long category, ImagePosition position, BOOL visible) {
		if (m_view.IsWindow())
			m_meta->SetVertexImageVisible(CCategory(guidDefault, category), (IMAGE_POSITION)position, visible != FALSE);
		return S_OK;
	}
	STDMETHOD(SetVertexImagesVisible)(long category, BOOL visible) {
		//if (m_view.IsWindow())
		//	m_view.SetVertexImagesVisible(visible != FALSE);
		return E_NOTIMPL;
	}
	STDMETHOD(SetEdgeVisible)(long category, BOOL visible) {
		//if (m_view.IsWindow())
		//	m_view.SetMetaEdgeHide(CCategory(guidDefault, category), visible == FALSE);
		return E_NOTIMPL;
	}
	STDMETHOD(SetEdgeImageVisible)(long category, ImagePosition position, BOOL visible) {
		if (m_view.IsWindow())
			m_meta->SetEdgeImageVisible(CCategory(guidDefault, category), (IMAGE_POSITION)position, visible != FALSE);
		return S_OK;
	}

	STDMETHOD(ScaleToFit)(long* pVal) {
		*pVal = (int)(100 * m_view.SetScaleToFit());
		return S_OK;
	}
	STDMETHOD(ScaleToFitSelected)(long* pVal) {
		*pVal = (int)(100 * m_view.SetScaleToFit(true));
		return S_OK;
	}
	STDMETHOD(ShowCategory)(long category) {
		if (m_view.IsWindow())
			m_view.SetMetaVertexHide(CCategory(guidDefault, category), false);
		return S_OK;
	}
	STDMETHOD(ShowHourglass)(IHourglass** pVal) {
		CComObject<CHourglass>* pHourglass;
		CComObject<CHourglass>::CreateInstance(&pHourglass);
		pHourglass->QueryInterface (pVal); // implicit AddRef
		return S_OK;
	}
	STDMETHOD(ShowMenu)(long x, long y, IDispatch* jsarrMenuStringNames, long* pVal) {
		HRESULT hr;
		DISPPARAMS noArgs = { NULL, NULL, 0, 0 };
		CComVariant resultV;
		CComPtr<IDispatch> pdisp = jsarrMenuStringNames;

		hr = pdisp->Invoke( DISPID_NEWENUM, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYGET,
			&noArgs, &resultV, NULL, NULL );
		if( FAILED( hr ) && FAILED( resultV.ChangeType( VT_UNKNOWN ) ) )
			return E_INVALIDARG;
		// Bug 37459, above Invoke succeeds, but returns resultV.vt == VT_EMPTY, resultV->other param unchanged
		if (resultV.vt != VT_UNKNOWN && resultV.vt != VT_DISPATCH)
			return E_INVALIDARG;

		CComQIPtr<IEnumVARIANT> pEnum ( resultV.punkVal );
		if( !pEnum )
			return E_INVALIDARG;

		CMenu menu (CreatePopupMenu());

		hr = S_OK;
		UINT id = 1;
		while( hr == S_OK ) {
			CComVariant elemV;
			hr = pEnum->Next( 1, &elemV, NULL );
			if (hr == S_OK) {
				if (elemV.vt != VT_BSTR)
					return E_INVALIDARG;
				bstr_t bstr = elemV;
				if (0==_tcscmp(bstr, _T("-")) || bstr.length() == 0)
					menu.AppendMenu(MF_SEPARATOR);
				else {
					InsertMenuItem(menu, id, (LPCTSTR)bstr);
					id++;
				}
			}
		}

#define HIJACK_MENUX
#ifdef HIJACK_MENU
		int hijackPos = id;
		menu.AppendMenu(MF_SEPARATOR);
		InsertMenuItem(menu, hijackPos++, _T("Force Directed 1"));
		InsertMenuItem(menu, hijackPos++, _T("Force Directed 2"));
		InsertMenuItem(menu, hijackPos++, _T("Circular Higherarchy"));
		InsertMenuItem(menu, hijackPos++, _T("Hierarchy"));
		InsertMenuItem(menu, hijackPos++, _T("Cluster"));
#endif

		*pVal = menu.TrackPopupMenu(TPM_RETURNCMD, x, y, m_hWnd);

#ifdef HIJACK_MENU
		if ((UINT)*pVal >= id)
		{
			if (*pVal == id)
			{
				m_view.SetLayout(SGV::LAYOUT_NEATO);
			}
			else if (*pVal == id + 1)
			{
				m_view.SetLayout(SGV::LAYOUT_FDP);
			}
			else if (*pVal == id + 2)
			{
				m_view.SetLayout(SGV::LAYOUT_TWOPI);
			}
			else if (*pVal == id + 3)
			{
				m_view.SetLayout(SGV::LAYOUT_DOT);
			}
			else if (*pVal == id + 4)
			{
				m_view.SetLayout(SGV::LAYOUT_CIRCO);
			}
			*pVal = -1;
		}
#endif

		return S_OK;
	}
	STDMETHOD(get_Spacing)(long* pVal) {
		*pVal = 100;
		if (m_view.IsWindow())
			*pVal = (long)(100 * m_view.GetSpacing());
		return S_OK;
	}
	STDMETHOD(put_Spacing)(long newVal) {
		if (newVal < 1)
			newVal = 1;
		else if (newVal > 1000)
			newVal = 1000;
		if (m_view.IsWindow())
			m_view.SetSpacing((float)newVal / 100.0f);
		return S_OK;
	}
	STDMETHOD(StopLayout)() {
		if (m_view.IsWindow())
		{
			m_view.SetLayout(SGV::LAYOUT_NULL);
		}
		return S_OK;
	}
	STDMETHOD(Recenter)() {
		if (m_view.IsWindow())
			m_view.CenterGraph();
		return S_OK;
	}
	STDMETHOD(RecenterSelected)() {
		if (m_view.IsWindow())
			m_view.CenterFocusNode();
		return S_OK;
	}
	STDMETHOD(RecenterOnVertex)(long category, BSTR identity) {
		if (m_view.IsWindow())
			m_view.CenterVertex(
				CUniqueID(guidDefault,category,(LPCTSTR)CW2T(identity, CP_UTF8)));
		return S_OK;
	}
	STDMETHOD(RemoveVertex)(long category, BSTR identity) {
		if (m_view.IsWindow())
			m_view.RemoveVertex(
				CUniqueID(guidDefault,category,(LPCTSTR)CW2T(identity, CP_UTF8)));
		return S_OK;
	}
	STDMETHOD(RenderTo)(OLE_HANDLE hDC, double X, double Y, double PageX, double PageY, double PageWidth, double PageHeight) {
		SetMapMode((HDC)hDC, MM_TEXT);
		if (m_view.IsWindow())
			m_view.RenderTo(CDCHandle((HDC)hDC), int(X), int(Y), int(PageX), int(PageY), int(PageWidth), int(PageHeight));
		return S_OK;
	}
	STDMETHOD(get_Zoom)(long* pVal) {
		*pVal = 100;
		if (m_view.IsWindow())
			*pVal = (long)ceil(100.0f * m_view.GetScale());
		return S_OK;
	}
	STDMETHOD(put_Zoom)(long newVal) {
		if (newVal < 1)
			newVal = 1;
		else if (newVal > 1000)
			newVal = 1000;
		if (m_view.IsWindow())
			m_view.SetScale((float)newVal / 100.0f);
		return S_OK;
	}
	HRESULT OnDrawAdvanced(ATL_DRAWINFO& di)
	{
		BOOL bDeleteDC = FALSE;
		if (di.hicTargetDev == NULL)
		{
			di.hicTargetDev = AtlCreateTargetDC(di.hdcDraw, di.ptd);
			bDeleteDC = (di.hicTargetDev != di.hdcDraw);
		}
		RECTL rectBoundsDP = *di.prcBounds;
		BOOL bMetafile = GetDeviceCaps(di.hdcDraw, TECHNOLOGY) == DT_METAFILE;
		if (!bMetafile)
		{
			::LPtoDP(di.hdcDraw, (LPPOINT)&rectBoundsDP, 2);
			SaveDC(di.hdcDraw);
			SetMapMode(di.hdcDraw, MM_TEXT);
			SetWindowOrgEx(di.hdcDraw, 0, 0, NULL);
			SetViewportOrgEx(di.hdcDraw, 0, 0, NULL);
			di.bOptimize = TRUE; //since we save the DC we can do this
		}
		di.prcBounds = &rectBoundsDP;
		GetZoomInfo(di);

		HRESULT hRes = OnDraw(di);
		if (bDeleteDC)
			::DeleteDC(di.hicTargetDev);
		if (!bMetafile)
			RestoreDC(di.hdcDraw, -1);
		return hRes;
	}
	virtual HRESULT OnDraw(ATL_DRAWINFO& di)
	{
		if (m_view.IsWindow())
			m_view.RenderTo(di.hdcDraw);
		return S_OK;
	}
	STDMETHOD(GetMeta)(BSTR url)
	{
		m_view.GetMeta(CW2T(url, CP_UTF8));
		return S_OK;
	}
	STDMETHOD(GetData)(BSTR url, LONG category, BSTR id)
	{
		m_view.GetData(CW2T(url, CP_UTF8), CUniqueID(guidDefault, category,  std::_tstring(CW2T(id, CP_UTF8))));
		return S_OK;
	}
	STDMETHOD(HideDisconnected)()
	{
		return SetVerticesVisibleByEdgeCount(0, FALSE);
	}
	STDMETHOD(HideSingletons)()
	{
		return SetVerticesVisibleByEdgeCount(1, FALSE);
	}
	STDMETHOD(SetSplinesMode)(long splines)
	{
		m_view.SetMetaSplines(splines == -1);
		return S_OK;
	}
	STDMETHOD(ShowOverview)(long show)
	{
		m_view.ShowOverview(show!=0);
		return S_OK;
	}
	STDMETHOD(SetOverviewOpacity)(long opacity)
	{
		m_view.SetOverviewOpacity(opacity);
		return S_OK;
	}
	STDMETHOD(ShowEdgeLabels)(long show)
	{
		m_view.SetEdgeLabelsVisible(show!=0);
		return S_OK;
	}

	//For Compatability with Relavint
	STDMETHOD(SetVertexIconHtmlElement)(long category, IHTMLElementRender* element)
	{
		return SetVertexImage(category, element, PositionWest, 0, 0, 0, 0, 0);
	}
	STDMETHOD(EmulateDecoration)(BYTE inplace)
	{
		m_view.EmulateDecoration(inplace != 0);
		return S_OK;
	}
	STDMETHOD(SetGraphStyle)(AppStyle style)
	{
		m_view.SetAppMode((AppMode)style);
		return S_OK;
	}
	STDMETHOD(LoadXgmml)(BSTR xml, LONG state, BOOL minimizeIfLarge)
	{
		m_view.LoadXGMML(CW2T(xml, CP_UTF8), WUVisualState(state), true, true, minimizeIfLarge != FALSE, NULL);
		return S_OK;
	}
	STDMETHOD(MergeXgmml)(BSTR xml, LONG state)
	{
		m_view.MergeXGMML(CW2T(xml, CP_UTF8), WUVisualState(state), true, true, true, NULL);
		return S_OK;
	}
	STDMETHOD(SelectFile)(BSTR filePath, BSTR* retVal)
	{
		CString strExporters = _T("XGMML Document|*.xml|All files|*.*||");
		LPTSTR pch = strExporters.GetBuffer(0); // modify the buffer in place
		while ((pch = _tcschr(pch, '|')) != NULL)
			*pch++ = '\0';

		CString strExporterDefault = _T("*.xml");
		CFileDialogEx wndFileDialog(TRUE, strExporterDefault, NULL, OFN_FILEMUSTEXIST, strExporters, m_hWnd );
		if ( IDOK == wndFileDialog.DoModal() ) 
		{
			*retVal = bstr_t(wndFileDialog.GetPathName().GetString()).copy();
		}
		return S_OK;
	}

	STDMETHOD(FindVertices)(BSTR label, /* [defaultvalue][in] */ BOOL searchLabel, /* [defaultvalue][in] */ BOOL searchProperties, IVertices** collection);

	STDMETHOD(SetApplicationZoomMode)(LONG mode)
	{
		m_view.SetAppZoomMode(AppZoomMode(mode));
		return S_OK;
	}

	STDMETHOD(FindGraphItemByID)()
	{
		m_view.FindItemByID();
		return S_OK;
	}

	STDMETHOD(GetVersion)(BSTR *pVal)
	{
		std::_tstring version;
		*pVal = bstr_t(SGV::GetServerGraphViewCtlVersion(version)).copy();
		return S_OK;
	}

};

OBJECT_ENTRY_AUTO(__uuidof(GraphViewControl), CGraphViewControl)

namespace VerticesCollection
{
	typedef StlLinked<IVertex>				IVertexAdapt;
	typedef std::vector<IVertexAdapt>		IVertexVector;

	typedef IVertexVector					ContainerType;
	typedef VARIANT							EnumeratorExposedType;
	typedef IEnumVARIANT					EnumeratorInterface;
	typedef IVertex*						CollectionExposedType;
	typedef IVertices						CollectionInterface;


	class IVertexCopy
	{
	public :
		static void init(IVertex** p);
		static void destroy(IVertex** p);
		static HRESULT copy(IVertex** pTo, const IVertexAdapt* pFrom);
	}; 

	class IVertexCopy2Variant
	{
	public :
		static void init(VARIANT* p);
		static void destroy(VARIANT* p); 
		static HRESULT copy(VARIANT* dest, const IVertexAdapt* src);
	}; 

	typedef CComEnumOnSTL< EnumeratorInterface, &__uuidof(EnumeratorInterface), EnumeratorExposedType,
							IVertexCopy2Variant, ContainerType > EnumeratorType;

	typedef ICollectionOnSTLImpl< CollectionInterface, ContainerType, CollectionExposedType,
							IVertexCopy, EnumeratorType > CollectionType;

}; // namespace VerticesCollection;

class ATL_NO_VTABLE CVertices : 
    public CComObjectRootEx<CComSingleThreadModel>,
    //public CComCoClass<CVertices, &CLSID_Vertices>,
	public IDispatchImpl<VerticesCollection::CollectionType, &IID_IVertices>
{
	typedef CComObjectRootEx<CComSingleThreadModel> objBase;
public:
	CVertices() {};

    DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CVertices)
    COM_INTERFACE_ENTRY(IVertices)
    COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

#ifdef _DEBUG
	ULONG InternalAddRef()
	{
		return objBase::InternalAddRef();
	}

	ULONG InternalRelease()
	{
		return objBase::InternalRelease();
	}

	void FinalRelease()
	{
	}
#endif

	void AddVertex(IVertex* v);
};
