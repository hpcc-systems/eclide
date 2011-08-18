// RelationshipView.h : interface of the CRelationshipView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_RELATIONSHIPVIEW_H__729DAABF_185A_4B2F_B14D_B33E4D1E6999__INCLUDED_)
#define AFX_RELATIONSHIPVIEW_H__729DAABF_185A_4B2F_B14D_B33E4D1E6999__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define WM_BINDER_REFRESH WM_APP+3214

#include "RelTableView.h"
#include "RelLabelView.h"
#include "RelBinder.h"


typedef CWinTraits<WS_CHILD|WS_BORDER|WS_OVERLAPPED|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_VISIBLE> CScrollTraits;

class CRelationshipView : 
	public COffscreenDraw<CRelationshipView>, 
	public CScrollWindowImpl<CRelationshipView, CWindow, CScrollTraits >

{
public:
	DECLARE_WND_CLASS_EX(NULL, CS_DBLCLKS, (HBRUSH)(COLOR_WINDOWFRAME + 1))

	typedef CScrollWindowImpl<CRelationshipView, CWindow, CScrollTraits > parentClass;

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		pMsg;
		return FALSE;
	}

	BEGIN_MSG_MAP(CRelationshipView)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_VSCROLL, OnVScroll)
		MESSAGE_HANDLER(WM_HSCROLL, OnHScroll)
		MESSAGE_HANDLER(WM_BINDER_REFRESH, OnRefresh)

		CHAIN_MSG_MAP( COffscreenDraw<CRelationshipView> )
		CHAIN_MSG_MAP( parentClass )
	END_MSG_MAP()

	// -- Control interface

	BOOL AddWindow(HWND hwnd)
	{
		ATLASSERT(::IsWindow(hwnd));
		ATLASSERT(::GetParent(hwnd)==m_hWnd);
		m_entities.Add(hwnd);
		return TRUE;
	};

	BOOL AddRelation(CRelationshipBinder* binder)
	{
		ATLASSERT(binder);
		m_relations.Add(binder);
		return TRUE;
	};

	// -- Menu handler
	// -- Message handler

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LRESULT lRes = parentClass::OnCreate(uMsg,wParam,lParam,bHandled);
		SetScrollSize(10000,10000); // set virtual playground
		return lRes;
	}

	LRESULT OnVScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{      
		parentClass::OnVScroll(uMsg,wParam,lParam,bHandled);
		OnRefresh(0,0,0,bHandled);
		return 0;
	}

	LRESULT OnHScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		parentClass::OnHScroll(uMsg,wParam,lParam,bHandled);
		OnRefresh(0,0,0,bHandled);
		return 0;
	}

	LRESULT OnRefresh(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		HWND hwnd = (HWND)lParam;
		CClientDC dc(m_hWnd);      

		Invalidate(TRUE); // clear and invalidate all

		/*      CPoint ptOffset;
		GetScrollOffset(ptOffset);
		for( int i=0; i<m_relations.GetSize(); i++ ) {
		if( m_relations[i]->GetSourceHwnd()==hwnd ||
		m_relations[i]->GetTargetHwnd()==hwnd ) {
		m_relations[i]->Draw(dc, m_hWnd, ptOffset);
		}
		};
		*/
		return 0;
	};

	void DoPaint(CDCHandle dc)
	{
		SIZE scrollSize;
		GetScrollSize(scrollSize);
		RECT clientRect;
		clientRect.right = scrollSize.cx;
		clientRect.bottom = scrollSize.cy;
		dc.FillSolidRect(&clientRect, ::GetSysColor(COLOR_BTNFACE));

		CPoint ptOffset(0, 0);
		GetScrollOffset(ptOffset);
		for( int i=0; i<m_relations.GetSize(); i++ ) {
			m_relations[i]->Draw(dc, m_hWnd, ptOffset);
		}

		return;
	}

	CSimpleArray<HWND> m_entities;
	CSimpleArray<CRelationshipBinder*> m_relations;

};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RELATIONSHIPVIEW_H__729DAABF_185A_4B2F_B14D_B33E4D1E6999__INCLUDED_)
