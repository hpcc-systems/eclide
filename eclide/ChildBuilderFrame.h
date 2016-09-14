#pragma once

__interface IAttribute;
__interface IAttributeHistory;
#include "Workunit.h"
#include "WorkspaceItem.h"
#include "SciSyntaxMarker.h"

enum BuilderStartup
{
    StartUnknown = 0,
    StartNoChange,
    StartEcl,
    StartExceptions,
    StartEclWatch,
    StartGraph,
    StartFirstResult,
    StartLast
};

class CMainFrame;
HWND OpenBuilderMDI(CMainFrame* pFrame, const CString &ecl, IWorkspaceItem * workspaceItem);
HWND OpenBuilderMDI(CMainFrame* pFrame, IWorkspaceItem * workspaceItem);
HWND OpenBuilderMDI(CMainFrame* pFrame, Dali::IWorkunit *src, IWorkspaceItem * workspaceItem, BuilderStartup startWith=StartFirstResult);
HWND OpenBuilderMDI(CMainFrame* pFrame, IAttribute *src, IWorkspaceItem * workspaceItem);
bool OpenFileBuilderMDI(CMainFrame* pFrame, const CString & filePath, IWorkspaceItem * workspaceItem, bool locked);
bool OpenFileBuilderMDI(CMainFrame* pFrame, const CString & filePath, IWorkspaceItem * workspaceItem, bool locked, const CSyntaxErrorVector & errors);
bool OpenFileBuilderMDI(CMainFrame* pFrame, const CString & filePath, IWorkspaceItem * workspaceItem, bool locked, Dali::IWorkunit *wu);


