#pragma once

#include "Searchable.h"
#include "UnicodeFile.h"
#include "resultFormat.h"

class CTabPane : 
	public CUnknown,
	public ISearchable
{
public:
	CTabPane() {};
	virtual ~CTabPane() {};

protected:
	CString m_path; //file save
	CString m_name; //tab name

public:
	virtual const CString &GetTabName() const { return m_name; };
	virtual const CString &GetTabToolTip(CString &tip) const { tip = m_name; return tip; };
	virtual void SetNamePath(const CString & sPathName);
	virtual CString GetFilePath() { return m_path; };

	virtual bool CanSelect() { return true; }
	virtual bool CanCopy() = 0;
	virtual bool HasResults() const = 0;
	virtual void GetResults(__int64 & /*total*/, __int64 & /*loaded*/) {};
	virtual void SaveSelectionToFile(CUnicodeFile & /*file*/) {};
	virtual void SaveToFile(const CResultFormatter & /*formatter*/) {};
	virtual void GotoLine(__int64 /*line*/, __int64 /*col*/) {};
	virtual HWND GetHWND() = 0;
	virtual bool UpdateUI(CCmdUI * /*pUIUpdate*/) {return false;};
	virtual void Refresh() {};
	virtual void AutoSizeColumn(int /*column*/) {};
	virtual bool ToggleHexColumnDisplay(int /*column*/) { return false; };
	virtual void RenameColumn(unsigned /*col*/, const std::_tstring& /*newname*/) {};
};

typedef StlLinked<CTabPane> CTabPanePtr;
typedef std::vector<CTabPanePtr> CTabPanePtrVector;
