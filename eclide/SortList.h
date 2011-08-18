#pragma once

#include "ListRow.h"
#include "GridMixIn.h"

enum SORT_DIRECTION
{
	SORT_DIRECTION_ASCENDING,
	SORT_DIRECTION_DESCENDING
};

template <class T, class TBase = CListViewCtrl, class TWinTraits = CControlWinTraits>
class CSortListViewCtrlImpl : 
	public CWindowImpl<T, TBase, TWinTraits>,
	public CGridMixIn<T>
{
	typedef CSortListViewCtrlImpl thisClass;
	typedef CWindowImpl<T, TBase, TWinTraits> baseClass;
protected:
	CImageList m_HeaderImgList;
	CHeaderCtrl m_Header;

	unsigned m_ColSort;
	SORT_DIRECTION m_ColSortDir;

public:
	DECLARE_WND_SUPERCLASS(NULL, baseClass::GetWndClassName())

	BEGIN_MSG_MAP(thisClass)
		MSG_WM_CREATE(OnCreate)
		MESSAGE_HANDLER(CWM_INITIALIZE, OnInitialize)
		MSG_WM_KEYDOWN(OnKeyDown)
		MSG_OCM_NOTIFY(OnNotify)

		CHAIN_MSG_MAP(CGridMixIn<T>)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	LRESULT OnCreate(LPCREATESTRUCT lParam)
	{
		SetMsgHandled(false);
		PostMessage(CWM_INITIALIZE);
		return 0;
	}

	LRESULT OnInitialize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		SetMsgHandled(false);
		T* pT = static_cast<T*>(this);
		pT->Refresh();
		return 0;
	}

	UINT GetItemState(int iItem, int iCol, UINT stateMask )
	{
		return TBase::GetItemState(iItem, stateMask);
	}

	LRESULT OnNotify(int nId, LPNMHDR nmhdr)
	{
		T* pT = static_cast<T*>(this);
		SetMsgHandled(false);
		switch(nId)
		{
		case pT->IDD:
			//SetMsgHandled(true);
			return pT->OnListNotify(nmhdr);
			break;

		default:
			break;
		}
		return 0;
	}

	LRESULT OnListNotify(LPNMHDR pnmhdr)
	{
		switch(pnmhdr->code)
		{
		case LVN_DELETEITEM:
			{
				NMLISTVIEW* plvdi = (NMLISTVIEW*) pnmhdr;
				CListRow * row = (CListRow *)plvdi->lParam;
				row->Release();
			}
			break;

		case LVN_GETDISPINFO:
			{
				NMLVDISPINFO* plvdi = (NMLVDISPINFO*) pnmhdr;
				CListRow * row = (CListRow *)plvdi->item.lParam;
				row->GetDispInfo(plvdi);
			}
			break;

		case LVN_COLUMNCLICK:
			{
				LPNMLISTVIEW nmlv = (LPNMLISTVIEW) pnmhdr;
				Sort(nmlv->iSubItem, CompareFunc, (LPARAM)this);
				SetMsgHandled(true);
			}
			break;
		}

		return 0;
	}

	void OnKeyDown(TCHAR vKey, UINT nRepCnt, UINT nFlags)
	{
		SetMsgHandled(false);
		T* pT = static_cast<T*>(this);
		switch (vKey) 
		{
			case VK_F5:
				SetMsgHandled(true);
				pT->Refresh();
				break;
		}
	}

	static int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
	{
		T *self = (T *)lParamSort;
		CListRow & l = *(CListRow *)lParam1;
		CListRow & r = *(CListRow *)lParam2;
		unsigned SortCol = self->GetSortColumn();
		//TODO:  string comparison not always the best cup of tea...

		CString lStr, rStr;
		l.GetText(SortCol, lStr);
		r.GetText(SortCol, rStr);
		if (self->GetSortDirection() == SORT_DIRECTION_ASCENDING)
			return lStr > rStr;
		return lStr < rStr;
	}

	void Sort(unsigned col, PFNLVCOMPARE pfnCompare, LPARAM lParamSort, SORT_DIRECTION dir = SORT_DIRECTION_ASCENDING)
	{
		HDITEM curItem;
		if (m_ColSort != col)
		{
			m_Header.GetItem(m_ColSort, &curItem);
			curItem.mask= HDI_IMAGE | HDI_BITMAP | HDI_FORMAT;
			curItem.fmt= HDF_LEFT | HDF_STRING;
			m_Header.SetItem(m_ColSort, &curItem);

			m_ColSort = col;
			m_ColSortDir = dir;
		}
		else
			m_ColSortDir = m_ColSortDir == SORT_DIRECTION_ASCENDING ? SORT_DIRECTION_DESCENDING : SORT_DIRECTION_ASCENDING;

		m_Header.GetItem(m_ColSort, &curItem);
		curItem.mask= HDI_IMAGE | HDI_FORMAT;
		curItem.iImage= m_ColSortDir;
		curItem.fmt= HDF_LEFT | HDF_IMAGE | HDF_BITMAP | HDF_STRING | HDF_BITMAP_ON_RIGHT;
		m_Header.SetItem(m_ColSort, &curItem);

		SortItems(pfnCompare, lParamSort);
	}

	SORT_DIRECTION GetSortDirection()
	{
		return m_ColSortDir;
	}

	unsigned GetSortColumn()
	{
		return m_ColSort;
	}

	thisClass& operator=(HWND hWnd)
	{
		if ( m_hWnd )
			DestroyWindow();
		if ( ::IsWindow(hWnd) )
			SubclassWindow(hWnd);

		m_ColSort = (unsigned) -1;
		m_ColSortDir = SORT_DIRECTION_DESCENDING;

		m_HeaderImgList.Create(16, 16, ILC_MASK, 2, 2);
		m_HeaderImgList.AddIcon(LoadIcon(_Module.m_hInstResource, MAKEINTRESOURCE(IDI_ASCENDING)));
		m_HeaderImgList.AddIcon(LoadIcon(_Module.m_hInstResource, MAKEINTRESOURCE(IDI_DESCENDING)));
		m_Header = GetHeader(); 
		m_Header.SetImageList(m_HeaderImgList.m_hImageList);

		return *this;
	}

	virtual void ClearRows()
	{
		DeleteAllItems();

		m_ColSort = (unsigned) -1;
		m_ColSortDir = SORT_DIRECTION_DESCENDING;
	}

	virtual void ClearCols()
	{
		while(DeleteColumn(0)) 
		{
		};
	}
};

