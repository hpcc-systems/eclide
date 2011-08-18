#include "stdafx.h"
#include "resource.h"

#include "GraphViewCanvas.h"
#include "TimelineCtrl.h"
#include "TimelineDateTimeCtrl.h"
#include "OverviewWnd.h"

#include "GraphViewContainer.h"
#include "DLLVersion.h"

using namespace SGV;

HINSTANCE CGraphView::hInstance;
static const TCHAR GRAPHVIEWCLASS[] = _T("SGV_GraphView");

CGraphView::CGraphView()
{
	m_canvas = new CGraphViewCanvas();
	m_timeline = new CTimelineCtrl();
	m_overview = new COverviewWnd();
}

CGraphView::~CGraphView()
{
	ATLASSERT(!m_overview->IsWindow());
	delete m_overview;
	ATLASSERT(!m_timeline->IsWindow());
	delete m_timeline;
	ATLASSERT(!m_canvas->IsWindow());
	//delete m_canvas;
}

bool CGraphView::Register()
{
	HINSTANCE hInst = ::GetModuleHandle(NULL);
	return Register(hInst);
}

bool CGraphView::DoRegister(const TCHAR * className, HINSTANCE hInstance)
{
	WNDCLASSEX wndclass;
	//Check weather the class is registerd already
	if (!(::GetClassInfoEx(hInstance, className, &wndclass)))
	{
		//If not then we have to register the new class
		wndclass.cbSize = sizeof(wndclass);
		wndclass.style = CS_DBLCLKS;
		wndclass.lpfnWndProc = ::CGraphView::SWndProc;
		wndclass.cbClsExtra = 0;
		wndclass.cbWndExtra = sizeof(CGraphView *);
		wndclass.hInstance = hInstance;
		wndclass.hIcon = NULL;
		wndclass.hCursor = LoadCursor(0, IDC_ARROW);
		wndclass.hbrBackground = NULL;
		wndclass.lpszMenuName = NULL;
		wndclass.lpszClassName = className;
		wndclass.hIconSm = 0;

		if (!::RegisterClassEx(&wndclass))
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

bool CGraphView::Register(HINSTANCE hInstance_)
{
	hInstance = hInstance_;
	return DoRegister(GRAPHVIEWCLASS, hInstance);
}

bool CGraphView::Unregister() 
{
	return ::UnregisterClass(GRAPHVIEWCLASS, hInstance) != 0;
}

BOOL CGraphView::PassToChildren(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
	if(uMsg>SGV::SGV_START && uMsg<SGV::SGV_LAST)
	{
		if(m_canvas->ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult)) 
			return TRUE; 
	}
	return FALSE;
}

LONG_PTR  PASCAL CGraphView::SWndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LONG_PTR lParam) 
{
	// Find C++ object associated with window.
	CGraphView *sci = reinterpret_cast<CGraphView *>(::GetWindowLongPtr(hWnd, 0));
	// sci will be zero if WM_CREATE not seen yet
	if (sci == 0) 
	{
		if (iMessage == WM_CREATE) 
		{
			// Create C++ object associated with window
			IGraph * graph = (IGraph *)((LPCREATESTRUCT) lParam)->lpCreateParams;
			ATLASSERT(graph == NULL);
			sci = new CGraphView();

			sci->AddRef();
			sci->Attach(hWnd);
			::SetWindowLongPtr(hWnd, 0, reinterpret_cast<LONG_PTR>(sci));
			LRESULT lResult;
			if (!sci->ProcessWindowMessage(hWnd, iMessage, wParam, lParam, lResult, 0))
			{
				return ::DefWindowProc(hWnd, iMessage, wParam, lParam);
			}
			return lResult;
		} 
		else 
		{
			return ::DefWindowProc(hWnd, iMessage, wParam, lParam);
		}
	} 
	else 
	{
		LRESULT lResult = 0;
		if (!sci->ProcessWindowMessage(hWnd, iMessage, wParam, lParam, lResult, 0))
		{
			lResult = ::DefWindowProc(hWnd, iMessage, wParam, lParam);
		}
		if (iMessage == WM_NCDESTROY) 
		{
			::SetWindowLong(hWnd, 0, 0);
			sci->Detach();
			sci->Release();
		} 
		return lResult;
	}
}

LRESULT CGraphView::OnCreate(LPCREATESTRUCT lParam)
{
	LRESULT lRes=DefWindowProc();
	if(lRes!=-1)
	{
		CRect rc(lParam->x, lParam->y, lParam->cx-lParam->x, lParam->cy-lParam->y);
		if(m_canvas->Create(m_hWnd, rc, _T("Canvas"), WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0U, 1U, NULL))
		{
			m_timeline->SetFontMgr(m_canvas->GetCanvas()->GetFontMgr());
			if(m_timeline->Create(m_hWnd, NULL, _T("Timeline"), WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0U, 2U, NULL))
			{
				m_canvas->AttachTimeline(m_timeline);
				m_canvas->AttachOverview(m_overview);

				CPoint ptparent(lParam->x,lParam->y);
				GetParent().ClientToScreen(&ptparent);
				CRect ovdefRC(10,10,110,110);
				ovdefRC.OffsetRect(ptparent);
				DWORD exstyle=m_overview->GetWndExStyle(0);
				::AdjustWindowRectEx(&ovdefRC, m_overview->GetWndStyle(0), FALSE, exstyle);

			    if(m_overview->IsLayered())
					exstyle |= WS_EX_LAYERED;					
				m_overview->Create(m_hWnd, ovdefRC, _T("Overview"), 0, exstyle, 0U, NULL);
				ATLASSERT(m_overview->IsWindow());
				m_overview->Init(m_canvas, m_canvas->GetGraph());
			    m_overview->SetLayeredWindowAttributes(0, m_canvas->GetOverviewOpacity(), LWA_ALPHA);
				lRes=1;
			}
		}
	}
	return lRes;
}

void CGraphView::OnDestroy()
{
	SetMsgHandled(false);

	if(m_overview->IsWindow())			// Was a popup window...
		m_overview->DestroyWindow();
}

void CGraphView::OnSize(UINT wParam, const CSize & size)
{
	ATLASSERT(m_canvas->IsWindow());
	if(m_canvas->GetApplicationVisMode()==RelavintTimeline)
	{
		int tlheight=m_timeline->Height();
		m_timeline->MoveWindow(0, 0, size.cx, tlheight);
		m_canvas->MoveWindow(0, tlheight, size.cx, size.cy-tlheight);
	}
	else
	{
		m_canvas->MoveWindow(0, 0, size.cx, size.cy);
	}
}

LRESULT CGraphView::OnEraseBkgnd(HDC)
{
	return 1;
}

LRESULT CGraphView::OnNotifyTimelineRClick(WPARAM /*wParam*/, LPNMHDR lpnmhdr, BOOL& /*bHandled*/)
{
	LPNMTIMELINERCLK lpnm=(LPNMTIMELINERCLK)lpnmhdr;

	if(m_timeline->DisplayMonthCalendar())
	{
		CTimelineDateTimeCtrl* calendar=new CTimelineDateTimeCtrl;

		calendar->Create(m_timeline->m_hWnd, 0, 0, WS_POPUP | WS_BORDER);
		if(calendar->IsWindow())
		{
			BOOL ok=calendar->SetCurSel(&lpnm->time);
			ATLASSERT(ok);
			CRect rc;
			MonthCal_GetMinReqRect(calendar->m_hWnd, &rc);
			CPoint pt=lpnm->pt;
			m_timeline->ClientToScreen(&pt);
			rc.OffsetRect(pt);
			calendar->MoveWindow(&rc);
			calendar->ShowWindow(SW_NORMAL);
		}
	}

#ifdef _DEBUG
	//using namespace boost::posix_time;
	//using namespace boost::gregorian;
	//FILETIME ft;
	//BOOL bOk=::SystemTimeToFileTime(&lpnm->time, &ft);
	//ATLASSERT(bOk);

	//ptime time=from_ftime<ptime>(ft);
	//std::string timestr=to_simple_string(time);
	//MessageBox(CA2T(timestr.c_str()));
#endif
	return 0;
}

LRESULT CGraphView::OnNotifyTimelineDblClk(WPARAM /*wParam*/, LPNMHDR lpnmhdr, BOOL& /*bHandled*/)
{
	LPNMTIMELINEDBLCLK lpnm=(LPNMTIMELINEDBLCLK)lpnmhdr;

	FILETIME ft1, ft2;
	::SystemTimeToFileTime(&lpnm->begin, &ft1);
	::SystemTimeToFileTime(&lpnm->end, &ft2);

	if(m_canvas->GetCanvas().get()->IsTimelineRangeWithinLimits(ft1, ft2))
	{
		m_timeline->SetTimeRange(lpnm->begin, lpnm->end);
		m_canvas->SetLayout(SGV::LAYOUT_TIMELINE);
	}
	return 0;
}
//=============================================================================

bool SGV::Register()
{
	return CGraphView::Register();
}

bool SGV::Register(HINSTANCE hInstance_)
{
	return CGraphView::Register(hInstance_);
}

const TCHAR * SGV::GetServerGraphViewCtlVersion(std::_tstring & version)
{
	CDLLVersion fvi;
	if(fvi.Open(CGraphView::hInstance))
	{
		TCHAR szVer[ 512 ] = { 0 };
		::wsprintf( szVer, 
			_T( "%d.%d.%d.%d" ),
			fvi.GetFileVersionMajor(),  // Major version
			fvi.GetFileVersionMinor(),  // Minor version
			fvi.GetFileVersionBuild(),  // Build number
			fvi.GetFileVersionQFE()     // QFE
			);

		version = szVer;
		fvi.Close();
	}
	return version.c_str();
}

HRESULT CALLBACK DllGetVersion(DLLVERSIONINFO *pdvi)
{
	return S_OK;
}