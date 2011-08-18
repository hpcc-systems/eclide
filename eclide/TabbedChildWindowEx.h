#pragma once

template <class TTabCtrl = CDotNetTabCtrl<CTabViewTabItem> >
class CTabbedChildWindowEx :
	public CTabbedChildWindow<TTabCtrl>
{
public:
	CTabbedChildWindowEx() 
	{
	}
	~CTabbedChildWindowEx(void) 
	{
	}

	typedef CTabbedChildWindowEx<TTabCtrl> thisClass;
	typedef CTabbedChildWindow<TTabCtrl> baseClass;

	//DECLARE_WND_CLASS(_T("TabbedChildWindowEx"))

	BEGIN_MSG_MAP(thisClass)
		NOTIFY_CODE_HANDLER(CTCN_SELCHANGE, OnSelChange)
		CHAIN_MSG_MAP(baseClass)
	END_MSG_MAP()

	LRESULT OnSelChange(int /*idCtrl*/, LPNMHDR pnmh, BOOL& bHandled)
	{
		bHandled = false;
		//NMCTC2ITEMS *pItems = (NMCTC2ITEMS *)pnmh;
		//OnTabSelected(pItems->iItem1, pItems->iItem2);
		::SendMessage(GetParent(), WM_NOTIFY, pnmh->idFrom, (LPARAM)pnmh);
		return 0;
	}

};
