// ReleationshipChildFrm.h : interface of the CReleationshipChildFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHILDFRM_H__D1E1F91F_89D2_4E96_B9FA_7EA294867CCC__INCLUDED_)
#define AFX_CHILDFRM_H__D1E1F91F_89D2_4E96_B9FA_7EA294867CCC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CReleationshipChildFrame : 
   public CMDIChildWindowImpl<CReleationshipChildFrame>
{
public:
   DECLARE_FRAME_WND_CLASS(NULL, IDR_MDICHILD)

   CRelationshipView m_view;

   virtual void OnFinalMessage(HWND /*hWnd*/)
   {
      delete this;
   }

   BEGIN_MSG_MAP(CReleationshipChildFrame)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_FORWARDMSG, OnForwardMsg)
      CHAIN_MSG_MAP(CMDIChildWindowImpl<CReleationshipChildFrame>)
   END_MSG_MAP()

   LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
   {
      m_hWndClient = m_view.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);

      bHandled = FALSE;
      return 1;
   }

   LRESULT OnForwardMsg(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
   {
      LPMSG pMsg = (LPMSG)lParam;
      if(CMDIChildWindowImpl<CReleationshipChildFrame>::PreTranslateMessage(pMsg))
         return TRUE;
      return m_view.PreTranslateMessage(pMsg);
   }
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDFRM_H__D1E1F91F_89D2_4E96_B9FA_7EA294867CCC__INCLUDED_)
