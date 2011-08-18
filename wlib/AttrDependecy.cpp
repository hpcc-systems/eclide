#include "StdAfx.h"
#include "..\en_us\resource.h"

#include "attrdependecy.h"
#include "global.h"
#include "RepositoryView.h"
#include "dali.h"
//#include "..\querybuilder\RepositoryFrame.h"
#include "atlsgrid.h"
#include <CustomMessages.h>

//  ===========================================================================
class CAttributeVectorView2 : public SGRID::CGridCtrl
{
	typedef CAttributeVectorView2 thisClass;
	typedef SGRID::CGridCtrl baseClass;
protected:
	IAttributeVector & m_attrs;

public:
	CAttributeVectorView2(IRepositorySlot * owner, IAttributeVector & attrs) : m_attrs(attrs)
	{
	}
	void Init()
	{
		SetCallbackFunc(GridCallback, (LPARAM)this);
		SetCallbackFuncPrepCache(GridCallbackPrepCache, (LPARAM)this);
		SetCallbackFuncCellClick(GridCallbackCellClick, (LPARAM)this);
		SetFixedRowCount(1);
		SetFixedColumnCount(0);
		SetRowCount(m_attrs.size());
		SetColumnCount(4);
	}
	void DoCopyAll(const std::_tstring & delim) const
	{
		std::_tstring txt;
		for(IAttributeVector::const_iterator itr = m_attrs.begin(); itr != m_attrs.end(); ++itr)
		{
			if (itr != m_attrs.begin())
				txt += delim;
			txt += (boost::_tformat(_T("%1%.%2%\t%3%\t%4%\t%5%")) %	itr->get()->GetModuleLabel() % 
																	itr->get()->GetLabel() % 
																	itr->get()->GetModifiedBy() % 
																	itr->get()->GetModifiedDate() % 
																	itr->get()->GetStateLabel()).str();
		}

		SetClipboard(txt.c_str());
	}
// ============================================================================
	BOOL CALLBACK GridCallback(SGRID::GV_DISPINFO *pDispInfo)
	{
		int col = pDispInfo->item.col;
		int row = pDispInfo->item.row;
		if (row == 0)
		{
			switch (col)
			{
			case 0:
				pDispInfo->item.strText = _T("Source");
				break;
			case 1:
				pDispInfo->item.strText = _T("User");
				break;
			case 2:
				pDispInfo->item.strText = _T("Date");
				break;
			case 3:
				pDispInfo->item.strText = _T("State");
				break;
			}
		}
		else
		{
			ATLASSERT(row > 0 && row <= (int)m_attrs.size());
			switch (col)
			{
			case 0:
				pDispInfo->item.strText = m_attrs[row-1]->GetQualifiedLabel();
				break;
			case 1:
				pDispInfo->item.strText = m_attrs[row-1]->GetModifiedBy();
				break;
			case 2:
				pDispInfo->item.strText = m_attrs[row-1]->GetModifiedDate();
				break;
			case 3:
				pDispInfo->item.strText = m_attrs[row-1]->GetStateLabel();
				pDispInfo->item.iImage = m_attrs[row-1]->GetState();
				break;
			}
		}
		return TRUE;
	}

	BOOL CALLBACK GridCallbackPrepCache(SGRID::GV_CACHEHINT *pDispInfo)
	{
		return TRUE;
	}

	BOOL CALLBACK GridCallbackCellClick(SGRID::GV_CELL *pCell)
	{
		return TRUE;
	}
	//  =====================
	bool RefreshRepository()
	{
		return true;
	}
	void DoCopy(const IAttributeVector & attrs, const std::_tstring & delim = _T(", "))
	{
	}
	void DoCheckout(IAttributeVector & attrs)
	{
	}
	void DoCheckin(IAttributeVector & attrs, const CString & comment)
	{
	}
	void DoRollback(IAttributeVector & attrs)
	{
	}
	void DoInsertModule(const CString label)
	{
	}
	void DoRenameModule(IModule * mod, const CString & label)
	{
	}
	void DoDeleteModule(IModuleVector & mods)
	{
	}
	bool DoInsertAttribute(IModule *mod, const CString label)
	{
		return false;
	}
	void DoRenameAttribute(IAttribute * attr, const CString & label)
	{
	}
	void DoMoveAttributeNoRefreshTarget(IAttributeVector & attrs, const CString & target)
	{
	}
	void DoMoveAttribute(IAttributeVector & attrs, const CString & target)
	{
	}
	void DoDeleteAttribute(IAttributeVector & attrs)
	{
	}

	//  Callbacks
	static BOOL CALLBACK GridCallback(SGRID::GV_DISPINFO *pDispInfo, LPARAM lParam)
	{
		thisClass * self = (thisClass * )lParam;
		return self->GridCallback(pDispInfo);
	}

	static BOOL CALLBACK GridCallbackPrepCache(SGRID::GV_CACHEHINT *pDispInfo, LPARAM lParam)
	{
		thisClass * self = (thisClass * )lParam;
		return self->GridCallbackPrepCache(pDispInfo);
	}

	static BOOL CALLBACK GridCallbackCellClick(SGRID::GV_CELL *pCell, LPARAM lParam)
	{
		thisClass * self = (thisClass * )lParam;
		return self->GridCallbackCellClick(pCell);
	}
};
//  ===========================================================================
class CAttrDependencyDlg : 
	//public CRepositoryFrameT<CAttrDependencyDlg>, 
	public CRepositorySlotImpl,
	public CDialogImpl<CAttrDependencyDlg>, 
	public boost::signals::trackable
{
	typedef CAttrDependencyDlg thisClass;
	typedef CDialogImpl<thisClass> baseClass;

public:
	CAttributeVectorView2 m_view;

	CAttrDependencyDlg(IAttributeVectorPtr & attrs) : m_view(this, attrs)
	{
	}

	enum { IDD = IDD_ATTRDEPEND };

	//IRepositorySlot
	void OnDoubleClick(IAttribute & /*attr*/)
	{
	}
	//IRepositorySlot
	IRepository * GetRepository()
	{
		return CRepositorySlotImpl::GetRepository();
	}
	BEGIN_MSG_MAP(thisClass)
		MSG_WM_INITDIALOG(OnInitDialog)
		MESSAGE_HANDLER_EX(CWM_INITIALIZE, OnInitialize)
		MSG_WM_CLOSE(OnClose)

		COMMAND_ID_HANDLER_EX(IDOK, OnOk)
		COMMAND_ID_HANDLER_EX(IDC_BUTTON_COPY, OnCopy)
	END_MSG_MAP()
	LRESULT OnInitDialog(HWND /*wParam*/, LPARAM /*lParam*/)
	{
		CWaitCursor cursor;
		CenterWindow(GetParent());

		CWindow wndPlaceholder = GetDlgItem(IDC_STATIC_PLACEHOLDER);
		CRect rc;
		wndPlaceholder.GetWindowRect(rc);
		ScreenToClient(rc);
		wndPlaceholder.DestroyWindow();
		m_view.Create(*this, rc, NULL, WS_CHILD | WS_CLIPCHILDREN | WS_BORDER | WS_VISIBLE | WS_TABSTOP, NULL, IDC_STATIC_PLACEHOLDER);
		m_view.SetWindowPos(HWND_TOP, rc, 0);
		m_view.Init();

		CImageList img, state;
		img.CreateFromImage(IDB_REP,16,0,RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
		img.SetBkColor(GetSysColor(COLOR_WINDOW));
		state.CreateFromImage(IDB_REP_MASK, 8, 0, RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
		state.SetBkColor(GetSysColor(COLOR_WINDOW));
		m_view.SetImageList(state.Detach());

		PostMessage(CWM_INITIALIZE);

		return TRUE;
	}
	LRESULT OnInitialize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
	{
		m_view.AutoSizeColumns(0, 3);
		return 0;
	}
	void OnClose()
	{
		SetMsgHandled(false);
	}
	void OnOk(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
	{
		EndDialog(IDOK);
	}
	void OnCopy(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
	{
		m_view.DoCopyAll(_T("\r\n"));
	}
	BOOL EndDialog(int nRetCode)
	{
		return baseClass::EndDialog(nRetCode);
	}
};
//  ===========================================================================
void DoShowDependantAttributes(IRepository *rep, Dali::IDali *dali, const std::_tstring & queue, const std::_tstring & cluster, const std::_tstring & ecl, const std::_tstring & module, const std::_tstring & attr, Dali::CEclExceptionVector & errorResults)
{
	Dali::AttributeVector daliAttrs;
	dali->GetDependancies(cluster.c_str(), queue.c_str(), ecl.c_str(), module.c_str(), attr.c_str(), daliAttrs, errorResults);

	IAttributeVectorPtr attrs;
	for(Dali::AttributeVector::iterator itr = daliAttrs.begin(); itr != daliAttrs.end(); ++itr)
	{
		attrs.push_back(rep->GetAttribute(itr->first.c_str(), itr->second.c_str(), CreateIAttributeECLType()));
	}
	if (attrs.size())
	{
		CAttrDependencyDlg dlg(attrs);
		dlg.DoModal();
	}
	else
	{
		::MessageBox(NULL, DependancyErrorMsg, _T("Source Dependency"), MB_ICONINFORMATION);
	}
}
