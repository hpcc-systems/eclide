// RoxieNetView.h : interface of the CRoxieNetView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "graph.h"
#include "touchgraph.h"
#include "../graph/layout/circular.h"
#include "../graph/render/twod.h"
#include "../graph/render/overview.h"
#include "WuHelper.h"

using namespace Gdiplus;

typedef boost::minstd_rand base_generator_type;
typedef std::pair<int, CString> VertexID;

class CRoxieNetView :
	public CScrollWindowImpl<CRoxieNetView>,
	public CGraphDataImpl<CRoxieNetView>,
	public CGraphTouchgraphLayoutImpl<CRoxieNetView>,
//	public CGraphCircularLayoutImpl<CRoxieNetView>,
//	public CGraphTwoDRenderImpl<CRoxieNetView>,
	public CGraphLiteRenderImpl<CRoxieNetView>,
//	public CGraphOverviewRenderImpl<CRoxieNetView>,
	public COffscreenDraw<CRoxieNetView>
{
	typedef CRoxieNetView thisclass;
	typedef CScrollWindowImpl<CRoxieNetView> baseClass;
	typedef COffscreenDraw<CRoxieNetView> baseClass2;
public:
    mutable boost::mutex m_mutex;
	std::map<VertexID, vertex_descriptor> m_lookup;
	WsWorkunitsMapper m_mapper;
	boost::thread_group threads;

public:
	UINT m_iTimerID;

	vertex_descriptor selNode;
	vertex_descriptor overNode;
	vertex_descriptor dragNode;
	ShortestPath pathToSel;
	Path overToSel;

	PointF dragNodeStartPos;
	POINT dragPageStartPos;

	CPoint mouseCapturePos;
	CPoint mouseCurrentPos;

	REAL m_scale;
	REAL m_rotate; 


public:
	DECLARE_WND_CLASS(_T("GraphView"))

	CRoxieNetView();

	BOOL PreTranslateMessage(MSG* pMsg);

	BEGIN_MSG_MAP(CRoxieNetView)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_TIMER(OnTimer)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_LBUTTONUP(OnLButtonUp)
		MSG_WM_LBUTTONDBLCLK(OnLButtonDblClick)
		MSG_WM_MOUSELEAVE(OnMouseLeave)

		CHAIN_MSG_MAP( baseClass2 )
		CHAIN_MSG_MAP( baseClass )
	END_MSG_MAP()

	LRESULT OnCreate(LPCREATESTRUCT lParam);
	void OnDestroy();

	void OnTimer(UINT id, TIMERPROC timerProc); 

	void OnLButtonDown(UINT vKey, CPoint &pos);
	void OnMouseMove(UINT vKey, CPoint &pos);
	void OnLButtonUp(UINT vKey, CPoint &pos);
	void OnLButtonDblClick(UINT vKey, CPoint &pos);
	void OnMouseLeave();

	void DoPaint(CDCHandle dc);

	bool isNewMouseOver();
	void setScale(REAL pScale);

	int ScrollWindowEx(int dx, int dy, UINT uFlags, LPCRECT lpRectScroll = NULL, LPCRECT lpRectClip = NULL, HRGN hRgnUpdate = NULL, LPRECT lpRectUpdate = NULL) throw();
	PointF toWorld(POINT p);
	PointF toWorld(RectF p);
	PointF toWorld(PointF p);

	void execute(vertex_descriptor v);
	void execute();
	vertex_descriptor conditionalAdd(const int cat, const CString & id, const PointF & defaultPos);
};
