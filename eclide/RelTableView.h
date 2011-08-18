// RelTableViewView.h : interface of the CRelTableView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_RELTABLEVIEWVIEW_H__729DAABF_185A_4B2F_B14D_B33E4D1E6999__INCLUDED_)
#define AFX_RELTABLEVIEWVIEW_H__729DAABF_185A_4B2F_B14D_B33E4D1E6999__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "RelationshipView.h"

class CRelTableView : 
   public CWindowImpl<CRelTableView, CListBox, CWinTraits<WS_CHILD|WS_OVERLAPPED|WS_CLIPSIBLINGS|WS_CAPTION|WS_THICKFRAME|WS_VISIBLE> >
{
public:
   DECLARE_WND_SUPERCLASS(NULL, CListBox::GetWndClassName())

   HWND m_hwndParent;
   CRect rcOrigSize;

   BEGIN_MSG_MAP(CRelTableView)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_GETMINMAXINFO, OnGetMinMaxInfo)
      MESSAGE_HANDLER(WM_WINDOWPOSCHANGING, OnWindowPosChanging)
   END_MSG_MAP()

   LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
   {
      LRESULT lRes = DefWindowProc(uMsg, wParam, lParam);

      ModifyStyle(0,LBS_NOINTEGRALHEIGHT|LBS_EXTENDEDSEL);

      GetClientRect(&rcOrigSize);
      if( rcOrigSize.Width() < 100 ) rcOrigSize.right = rcOrigSize.left + 100;
      if( rcOrigSize.Height() < 20 ) rcOrigSize.bottom = rcOrigSize.top + 20;

      m_hwndParent = ::GetParent(m_hWnd);
      ATLASSERT(::IsWindow(m_hwndParent));

      return lRes;
   }

   LRESULT OnGetMinMaxInfo(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
   {
      LPMINMAXINFO info = (LPMINMAXINFO)lParam;
      info->ptMinTrackSize = CPoint(rcOrigSize.Width(),rcOrigSize.Height());
      return 0;
   }

   LRESULT OnWindowPosChanging(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
   {
      ::SendMessage(m_hwndParent, WM_BINDER_REFRESH, 0, (LPARAM)m_hWnd);
      bHandled = FALSE;
      return 0;
   }

};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RELTABLEVIEWVIEW_H__729DAABF_185A_4B2F_B14D_B33E4D1E6999__INCLUDED_)
