#pragma once

#include "grid.h"

#include <searchable.h>
#include <Repository.h>
#include <Workunit.h>
#include <ResultFormat.h>
#include ".\eclexec.h"
#include "TabbedChildWindowEx.h"
#include "ChildBuilderFrame.h"

class ResultDataSource;
class CUnicodeFile;

typedef enum _eResultCopyObject
{
    eAll, eColumn, eRow
} ResultCopyObject;

__interface __declspec(uuid("95653D64-0CB5-4bd1-8AEC-2ADDAA16D2BD")) IResultView : public IUnknown
{
    HWND GetHwnd() const;
};

__interface __declspec(uuid("4FD79D99-06AF-4ca7-93DD-3052E903D8A2")) IResultGridView : public IResultView
{
    void RenameColumn(unsigned int col, const std::_tstring & label);
    void SplitColumn(unsigned int col, const SplitVector & positions);
};

class CResultContext
{
public:
    
    CResultContext(IResultView * view, HMENU menu, const CPoint& pt)
        : m_view(view), m_menu(menu), m_pt(pt)
    {
        m_row = m_column = m_cellCharPos = -1;
    }

    WTL::CMenu m_menu;
    CPoint m_pt;
    int m_row;
    int m_column;
    std::_tstring m_cellData;
    int m_cellCharPos;
    CComPtr<IResultView> m_view;
};

__interface IResultSlot 
{
    bool OnSingleClickResult(__int64 row, int col);
    bool OnDoubleClickResult();
    bool OnContextResult(const CResultContext &context);
    bool Drilldown(const CString &ecl);
    bool WorkunitUpdated(Dali::IWorkunit * workunit, bool bFirst);
    bool WorkunitDeleted(Dali::IWorkunit * workunit);
    void PostStatus(const TCHAR *status);
    void PostResults(__int64 start, int count, __int64 total);
};

class CResultSlotImpl : public IResultSlot
{
public:
    bool OnSingleClickResult(__int64 /*row*/, int /*col*/) { return false; } 
    bool OnDoubleClickResult() { return false; }
    bool OnContextResult(const CResultContext & /*context*/) { return false; }
    bool Drilldown(const CString & /*ecl*/) { return false; }
    bool WorkunitUpdated(Dali::IWorkunit * /*workunit*/, bool /*bFirst*/) { return false; }
    void PostStatus(const TCHAR * /*status*/) {}
    void PostResults(__int64 /*start*/, int /*count*/, __int64 /*total*/) {}
    bool WorkunitDeleted(Dali::IWorkunit *) {return false;}
};

__interface IResultViewer :	public ISearchable, public IUnknown
{
    HWND GetHwnd();
    HWND Create(HWND hWndParent);
    BOOL PreTranslateMessage(MSG* pMsg);
    int GetTabCount();
    void CloseAllTabs();
    void DestroyWindow();
    const TCHAR *GetID(CString & result);
    void PrepareForDisplay(bool bRefresh=true);
    void AutoSizeColumn(int column=-1);
    bool ToggleHexColumnDisplay(int column=-1);
    void RenameColumn(unsigned col, const std::_tstring& newname);
    bool UpdateUI(CCmdUI * cui);
    void ExecEcl(const TCHAR *clusterName, const TCHAR *queueName, Dali::WUAction action, const TCHAR *qualifiedLabel, const TCHAR *eclSource, const TCHAR *eclPath, const TCHAR *scheduled, const TCHAR *label, int resultLimit, const TCHAR *debugString, TYPE type, int maxRuntime, bool debug);
    void SetDeleteWorkunit(bool bDelete=true);
    void PostSelectRibbon();
};

typedef StlLinked<IResultViewer> IResultViewerAdapt;

IResultViewer *CreateIResultViewer(IResultSlot *owner);
IResultViewer *CreateIResultViewer(IResultSlot *owner, Dali::IWorkunit *wu, BuilderStartup startupMode);
IResultViewer *CreateIResultViewer(IResultSlot *owner, const CString & wuid);

const TCHAR * GetWorkUnitEclWatchURL(Dali::IWorkunit *wu, CString &url);
const TCHAR * GetFramedWorkUnitEclWatchURL(Dali::IWorkunit *wu, CString &url);
const TCHAR * GetFramedDesdlEclWatchURL(const std::_tstring & desdlID, CString &url);
const TCHAR * GetDesdlEclWatchURL(const std::_tstring & desdlID, CString &url);

