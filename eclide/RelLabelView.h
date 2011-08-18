// RelLabelViewView.h : interface of the CRelLabelView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_RELLABELVIEWVIEW_H__729DAABF_185A_4B2F_B14D_B33E4D1E6999__INCLUDED_)
#define AFX_RELLABELVIEWVIEW_H__729DAABF_185A_4B2F_B14D_B33E4D1E6999__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "RelationshipView.h"

class CRelLabelView : 
   public CWindowImpl<CRelLabelView, CWindow, CWinTraits<WS_CHILD|WS_CAPTION|WS_OVERLAPPED|WS_CLIPSIBLINGS|WS_THICKFRAME|WS_VISIBLE> >
{
public:
   DECLARE_WND_SUPERCLASS(NULL, CWindow::GetWndClassName())

   HWND m_hwndParent;

   BEGIN_MSG_MAP(CRelLabelView)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_WINDOWPOSCHANGING, OnWindowPosChanging)
      MESSAGE_HANDLER(WM_NCHITTEST, OnHitTest)
      MESSAGE_HANDLER(WM_NCPAINT, OnNcPaint)
   END_MSG_MAP()

   LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
   {
      m_hwndParent = ::GetParent(m_hWnd);
      ATLASSERT(::IsWindow(m_hwndParent));
      bHandled = FALSE;
      return 0;
   }

   LRESULT OnWindowPosChanging(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
   {
      ::SendMessage(m_hwndParent, WM_BINDER_REFRESH, 0, (LPARAM)m_hWnd);
      bHandled = FALSE;
      return 0;
   }

   LRESULT OnHitTest(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      return HTCAPTION;
   }

   LRESULT OnNcPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      CWindowDC dc(m_hWnd);
      CRect rc;
      GetWindowRect(rc);
      rc.MoveToXY(0,0);
      // Paint into this DC
      dc.FillRect(rc, (HBRUSH)(COLOR_WINDOW+1));
      dc.DrawEdge(rc, EDGE_RAISED, BF_RECT);
      TCHAR szText[128];
      GetWindowText(szText, sizeof(szText)/sizeof(TCHAR));
      rc.DeflateRect(5,5);
      dc.DrawText(szText, ::lstrlen(szText), &rc, DT_SINGLELINE|DT_CENTER|DT_VCENTER);
      return 0;
   }

};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RELLABELVIEWVIEW_H__729DAABF_185A_4B2F_B14D_B33E4D1E6999__INCLUDED_)
