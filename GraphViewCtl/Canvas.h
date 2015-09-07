#pragma once

#include "meta.h"
#include "Cache.h"
#include "GraphContainer.h"
#include "MetaContainer.h"
#include "Renderer.h"
#include "Transform.h"
#include "ScaledWindows.h"
#include "GraphViewCtlApi.h"
#include "Notifications.h"

class CItemEdit;
struct CAggFonts;
class CTimelineCtrl;

//*****************************************************************************

// NOTE: *MUST* match definition in GraphViewCtl.idl
enum HITTEST
{
	HITTEST_UNKNOWN,
	HITTEST_LABEL,
	HITTEST_ICON,
	HITTEST_IMAGE,
	HITTEST_BBOX
};

typedef std::pair<CUniqueID, HITTEST> VertexAt;


//*****************************************************************************

class CCanvas : public CTransform, public CUnknown
{
	//---------------------------------------------------------------------------
	// types

public:
	struct PostedMsg
	{
		CComPtr<IRenderer> ir;
		UINT uMsg;
		WPARAM wParam;
		LPARAM lParam;

		PostedMsg() : uMsg(0), wParam(0), lParam(0)
		{
		}

		PostedMsg(const PostedMsg& pm)
		{
			ir=pm.ir; // the reason for implementing copy constructor
			uMsg=pm.uMsg;
			wParam=pm.wParam;
			lParam=pm.lParam;
		}
	};

	struct WndGlue
	{
		CUniqueID id;
		CScaledWindow* wnd;
	};

	struct ImageSelection
	{
		IImageRenderer* m_image;
		IMAGE_POSITION m_pos;
		int	m_overlayIdx;

		ImageSelection() : m_image(0), m_pos(IMAGE_POSITION(-1)), m_overlayIdx(-1) {}

		bool Select(IImageRenderer* image, IMAGE_POSITION pos, int idx)
		{
			bool change=m_image!=image;

			if(change)
			{
				if(m_image)
					m_image->SetSelected(false);
				m_image=image;
				m_image->SetSelected(true);
			}

			if(!change)
				change=m_pos!=pos || m_overlayIdx!=idx;

			m_pos=pos;
			m_overlayIdx=idx;
			return change;
		}

		void Clear()
		{
			if(m_image)
			{
				m_image->SetSelected(false);
				m_image=0;
			}
			m_pos=IMAGE_POSITION(-1);
			m_overlayIdx=-1;
		}
	};

	//-----------------------------------------------------------------------------

	typedef std::list<WndGlue> WindowList;
	typedef std::list<PostedMsg> MessageList;
	typedef CacheT<std::_tstring, CPartTheme> PartThemeCacheMap;
	typedef HashCacheT<int, CListFont> ListFontCacheMap;
	typedef HashCacheT<int, CCaptionFont> CaptionFontCacheMap;

	//-----------------------------------------------------------------------------

private:
	IGraph* m_graph;

	bool volatile m_running;
	bool m_printing;
	AppMode m_appMode;
	AppVisMode m_appVisMode;
	AppZoomMode m_appZoomMode;
	bool m_dragDropNotifications;
	bool m_mouseOut; 
	MOUSE_MODES m_mouseMode;

	PartThemeCacheMap m_themeCache;
	CaptionFontCacheMap m_captionFontCache;
	ListFontCacheMap m_listFontCache;

	// TODO: do these belong here?
	CComPtr<IVertexPath> m_overToSel;

	CUniqueID m_hilightID;

	CScaledWindow* m_timerTarget;
	CScaledWindow* m_currentCapture;
	CScaledWindow* m_currentKbdHook;

	CWindow m_wndCanvas;
	CItemEdit* m_edit;
	GraphTypes::RectF m_graphExtent;
	WTL::CPen m_gridPen;
	HINSTANCE hInstRich;

	DWORD m_hGUIThreadId;

	CComPtr<IVertexShortestPath> m_allPathsToFocus;

public:
	CComPtr<IVertexPath> m_overPathToFocus;

#ifdef _DEBUG
	bool inpaint;
	GraphTypes::REAL m_gammathr;
#endif

#ifdef _DEBUG_DESTRUCT
	virtual ULONG STDMETHODCALLTYPE AddRef() throw();
	virtual ULONG STDMETHODCALLTYPE Release() throw();
#else
	IMPLEMENT_CUNKNOWN;
#endif

	CMeta&			m_meta;
	CButtonState*	m_currentHot;
	CScaledWindow*	m_currentFocus;
	WindowList		m_selectedList;
	ImageSelection  m_selectedImage;
	MessageList		m_postedMessages;
	HCURSOR			m_HandClosedCursor, m_HandOpenCursor,
					m_ZoomInCursor, m_ZoomOutCursor, m_HandArrowCursor;
	CTimelineCtrl*  m_timeline;

	//------------------------------------------------------------------------

	CAggFonts*		m_fonts;

	//------------------------------------------------------------------------

	CCanvas(IGraph * graph, CMeta & meta);
	virtual ~CCanvas();

	void Clear();

	//------------------------------------------------------------------------
	// AppMode and renderer support, renderers Factory 

	void SetApplicationMode(AppMode mode);
	AppMode GetApplicationMode() const;
	void SetApplicationVisMode(AppVisMode vismode);
	AppVisMode GetApplicationVisMode() const;
	void SetApplicationZoomMode(AppZoomMode mode);
	AppZoomMode GetApplicationZoomMode() const { return m_appZoomMode; }
#ifdef _DEBUG
	void SetGammaThreshold(GraphTypes::REAL thr) 
	{
		m_gammathr=thr;
	}

	GraphTypes::REAL GetGammaThreshold() const
	{
		return m_gammathr;
	}
#endif


	void AttachTimeline(CTimelineCtrl* tl) { m_timeline=tl; }

	IRenderer* CreateGraphItemRenderer(IGraphItem* gi, RendererTypes type);

	IOverviewRenderer* GetOverviewRenderer();
	IRenderer * GetVertexRenderer(const CUniqueID & id) const;
	IRenderer * GetRenderer(IGraphItem * item, bool createIfNeeded = true) const;
	void SetRenderer(IGraphItem * item, IRenderer * renderer);


	//------------------------------------------------------------------------
	// Renderers ScaledWindows support

	void SetTopParent(HWND hwnd);
	LRESULT SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void PostMsg(IRenderer* ir, UINT uMsg, WPARAM wParam=0, LPARAM lParam=0);

	void Invalidate(CRect* rc=0, bool bErase=false);
	void SetTimer(UINT nIDEvent, UINT uElapse, CScaledWindow* target);
	void KillTimer(UINT uIDEvent);
	CScaledWindow* GetTimerTarget() { return m_timerTarget; }
	IGraphItem* FindGraphItem(const GraphTypes::PointF& point);
	void OnRemoveGraphItem(IGraphItem* gi);

	CScaledWindow* SetCapture(CScaledWindow* capnew);
	bool ReleaseCapture();
	CScaledWindow* GetCapture();

	CScaledWindow* HookKbdMsgs(CScaledWindow* hooker);
	CScaledWindow* GetKbdHook();

	bool ClearCurrentHot(BUTTON_STATE newstate=BS_NORMAL); 
	CScaledWindow* GetCurrentHot(); 
	bool ClearFocus();
	void DeselectAll();
	unsigned CountSelected() const;
	unsigned GetSelected(IUniqueIDContainer * results);
	void MoveAllSelected(const CPoint& offset, CScaledWindow* skip);
	bool SetHilightID(const CUniqueID& id);
	const CUniqueID& CCanvas::GetHilightID() const;

	bool DragDropNotifications();
	void DragDropNotifications(bool set);
	void ClientCursorPos(CPoint* pt) const;
	GraphTypes::REAL ScreenPosFromTime(
		const FILETIME& time, 
		const FILETIME& screenleft, 
		const FILETIME& screenright) const;
	bool IsTimelineRangeWithinLimits(const FILETIME& begin, const FILETIME& end); 


	//------------------------------------------------------------------------
	// Notifications sent by canvas or renderers

	template<typename N>
	LRESULT NotifyParent(const UINT code, N * nmh)
	{
		ATLASSERT(m_wndCanvas.IsWindow());

		nmh->hdr.hwndFrom = m_wndCanvas.m_hWnd; 
		nmh->hdr.idFrom = m_wndCanvas.GetDlgCtrlID();
		nmh->hdr.code = code;

		LRESULT lRet = 0;
		CWindow target = m_wndCanvas.GetParent();
		CWindow ptarget = target.GetParent();
		CWindow gptarget = ptarget.GetParent();
		if(lRet == 0 && gptarget.IsWindow())
			lRet = gptarget.SendMessage(WM_NOTIFY, 0, (LPARAM) nmh);

		if(lRet == 0 && ptarget.IsWindow())
			lRet = ptarget.SendMessage(WM_NOTIFY, 0, (LPARAM) nmh);

		if(lRet == 0 && target.IsWindow())
			lRet = target.SendMessage(WM_NOTIFY, 0, (LPARAM) nmh);

		if(lRet == 0)
			lRet = m_wndCanvas.SendMessage(WM_NOTIFY, 0, (LPARAM) nmh);

		return lRet;
	}
	LRESULT NotifySelection(const CUniqueID& selected, const CUniqueID& deselected=CUniqueID());
	LRESULT NotifyItemsConnected(const CUniqueID& from, const CUniqueID& to);
	LRESULT NotifyLabelEdited(const CUniqueID& parentid, const CUniqueID& id, const std::_tstring& newlabel);
	LRESULT NotifyRecalcEdgePoints();
	LRESULT NotifyFocusChanged(const CUniqueID& id, bool bGained);
	LRESULT NotifyHotChanged(const CUniqueID& id, bool bGained);
	LRESULT NotifyDoDragDrop(const CPoint& dragStartPos);
	LRESULT NotifyContextMenu(UINT uMsg, const CUniqueID& id, const CPoint& pt);
	LRESULT NotifyScroll(const CPoint& pt);
	LRESULT NotifyMouseMessage(UINT uMsg, const CUniqueID& id, const CPoint& pt);


	//-----------------------------------------------------------
	// Legacy methods

	void SelectItemsWithin(const GraphTypes::RectF &r);
	GraphTypes::RectF getGraphExtent(bool selectedOnly, GraphTypes::Coordinates co) const;
	GraphTypes::RectF getGraphExtent(bool selectedOnly) const;
	void SetRunning(bool running);

	//-----------------------------------------------------------
	// in-line editing

	bool AttachEditor(IGraphItem* item);
	void DestroyEditor();

	//-----------------------------------------------------------

	void GetViewport(GraphTypes::RectF& vp) const;
	void GetClient(GraphTypes::RectF& cl) const;
	void DrawGrid(CDCHandle& dc);
	void Scroll(const CPoint& scroll);

	void StoreGraphExtent(const GraphTypes::RectF& extent)
	{
		m_graphExtent=extent;
	}

	void GetGraphExtent(GraphTypes::RectF& extent, GraphTypes::Coordinates co = GraphTypes::screen) const
	{
		extent=m_graphExtent;
		if(co==GraphTypes::world)
		{
			RAllTransform(extent);
		}
	}

	bool IsPrinting() const { return m_printing; }

	int GetFontPixHeight() const;

	// get the center of the screen in world coordinates
	void GetScreenCenter(GraphTypes::PointF& center) const;

	// set screen center to point passed in world coordinates
	bool CenterAt(const GraphTypes::PointF& center);

	void ClearCaches();

	//-----------------------------------------------------------
	// Utility methods

	void ScaleMargins(MARGINS& margins);

	//-----------------------------------------------------------
	// Mouse mode methods

	MOUSE_MODES CalcMouseMode();
	MOUSE_MODES GetMouseMode() const { return m_mouseMode; }
	void SetModeCursor();
	void SetZoomModeCursor();

	// data singletons
	CPartTheme& GetPartTheme(const TCHAR* part);
	CCaptionFont& GetCaptionFont(int size);
	CListFont& GetListFont(int size);
	CAggFonts* GetFontMgr() { return m_fonts; }

	// Theme change notification from upper-level code
	void NotifyThemeChanged();
#ifdef _DEBUG_THEME
	bool bdebugTheme;
	void FlipDebugTheme();
#endif

	bool HasVertexFocus();
};

//*****************************************************************************

IGraphRenderer *CreateNewGraphRenderer(CCanvas & canvas, IGraph * graph);
IGraphRenderer *CreateLegacyGraphRenderer(CCanvas & canvas, IGraph * graph);
IGraphRenderer *CreateAttGraphRenderer(CCanvas& canvas, IGraph* graph);
IGraphRenderer *CreateTimelineGraphRenderer(CCanvas& canvas, IGraph* graph);
IOverviewRenderer *CreateOverviewRenderer(CCanvas& canvas, IGraph* graph);

IRenderer* CreateIOSubgraphRenderer(CCanvas & canvas);
IRenderer* CreateDatasetRenderer(CCanvas & canvas);
IRenderer* CreateAttSubgraphRenderer(CCanvas & canvas);
IRenderer* CreateLegacySubgraphRenderer(CCanvas & canvas);

IRenderer* CreateLegacyVertexRenderer(CCanvas & canvas);
IRenderer *CreateTimelineVertexRenderer(CCanvas & parent);
IRenderer* CreateListItemRenderer(CCanvas & canvas);
IRenderer* CreateTextboxRenderer(CCanvas & parent);
IRenderer* CreateInputRenderer(CCanvas& canvas);
IRenderer* CreateOutputRenderer(CCanvas& canvas);
IRenderer* CreateAttVertexRenderer(CCanvas & parent);

IRenderer* CreateLegacyEdgeRenderer(CCanvas & canvas);
IRenderer* CreateTimelineEdgeRenderer(CCanvas & parent);
IRenderer* CreateEdgeRenderer(CCanvas & canvas);
IRenderer* CreateAttEdgeRenderer(CCanvas & parent);

IRenderer* CreateArrowTipRenderer(CCanvas & canvas);
IRenderer* CreateArrowTailRenderer(CCanvas & canvas);
IRenderer* CreateImageRenderer(CCanvas & canvas, IGraphItem * item);

