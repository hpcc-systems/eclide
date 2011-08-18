#pragma once

#include "ScaledScrollBar.h"

class CCanvas;

class CScrollSupport : public CMessageMap
{
protected:
	CScaledScrollBar m_vSB;
	CScaledScrollBar m_hSB;
	CCanvas* m_pcanvas;
	CRect m_rcWorld;
	CRect m_rcClient;

public:
	CScrollSupport(CCanvas* canvas, CScaledWindow* parent=0);
	virtual ~CScrollSupport();

	bool DispatchToScrollbars(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult);
	void UpdateScrollbars(const CRect& rcextent);
	void PaintScrollbars(WPARAM wParam);
	void SetScrollRect(const CRect& wndrc);
	void ModifyWindowRect(CRect& rc);

protected:
	BEGIN_MSG_MAP(CScrollSupport)
		MESSAGE_HANDLER(WM_VSCROLL, OnVScroll)
		MESSAGE_HANDLER(WM_HSCROLL, OnHScroll)
	END_MSG_MAP()

	LRESULT OnVScroll(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnHScroll(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};
