#pragma once

#include "EclDlg.h"
#include "CtlColor.h"
#include "Migration.h"

//  ===========================================================================
class CAttributeDlg : 
	public CEclDlgQBImpl<CAttributeDlg>,
	public WTL::CDialogResize<CAttributeDlg>,
	public WTL::CWinDataExchange<CAttributeDlg>,
	public CCtlColor,
	public IMigrationCallback,
	public boost::signals::trackable
{
	typedef CAttributeDlg thisClass;
	typedef CEclDlgQBImpl<thisClass> baseClass;
protected:
	CComPtr<IAttribute> m_attribute;
	CComPtr<IMigration> m_migrator;
	std::_tstring m_cluster;
	boost::signals::connection m_sigConn;

public:
	enum { IDD = IDD_ATTRIBUTEVIEW };

	CAttributeDlg(IAttribute *attribute, ISourceSlot *owner);

	bool DoSave(bool attrOnly);
	void GetTitle(CString & title);
	void DoCheckSyntax();
	void DoCheckComplexity();
	void DoCheckDependency();
	void SetAttribute(IAttribute *attribute);
	IAttribute * GetAttribute();
	const TCHAR * GetCluster();
	bool CanExecute();

	BEGIN_MSG_MAP(thisClass)
		MSG_WM_INITDIALOG(OnInitDialog)
		MESSAGE_HANDLER_EX(CWM_INITIALIZE, OnInitialize)

		COMMAND_ID_HANDLER_EX(ID_ECL_SYNCTOC, OnEclSyncToc)
		COMMAND_ID_HANDLER_EX(ID_ECL_GOTO, OnEclGoto)
		COMMAND_ID_HANDLER_EX(ID_ECL_GOTOSYNCTOC, OnEclGotoSyncToc)

		CHAIN_MSG_MAP(CCtlColor)
		CHAIN_MSG_MAP(CDialogResize<thisClass>)
		CHAIN_MSG_MAP(CEclDlgQBImpl<thisClass>)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(thisClass)
		DLGRESIZE_CONTROL(IDC_EDIT_ECL, DLSZ_SIZE_X | DLSZ_SIZE_Y)
	END_DLGRESIZE_MAP()

	BEGIN_DDX_MAP(thisClass)
	END_DDX_MAP()

	LRESULT OnInitDialog(HWND hWnd, LPARAM lParam);
	LRESULT OnInitialize(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void OnEclSyncToc(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/);
	void OnEclGoto(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/);
	void OnEclGotoSyncToc(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/);

	//  IAttribute Notifications  ---
	void operator()(IAttribute * attr, bool eclChanged, IAttribute * newAttrAsOldOneMoved, bool deleted);

	//  IMigrationCallback  ---
	BOOL Invalidate(BOOL bErase = TRUE);
	void LogMsg(const std::_tstring & msg);
	void PostStatus(const TCHAR* pStr);
	void PostProgress(int progress);
};
//  ===========================================================================
