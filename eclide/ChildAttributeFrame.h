#pragma once

#include "SciSyntaxMarker.h"
#include "WorkspaceItem.h"
#include "FindReplace.h"

__interface IModule;
__interface IAttribute;
__interface IAttributeHistory;

class CMainFrame;

HWND OpenAttributeMDI(CMainFrame* pFrame, IWorkspaceItem * workspaceItem);
HWND OpenAttributeMDI(CMainFrame* pFrame, IAttribute * attribute, const std::_tstring & searchTerm, FINDMODE findmode, IWorkspaceItem * workspaceItem);
HWND OpenAttributeMDI(CMainFrame* pFrame, IAttribute * attribute, IWorkspaceItem * workspaceItem, bool bHistoryView, Dali::CEclExceptionVector * errors);
HWND OpenAttributeMDI(CMainFrame* pFrame, const std::_tstring & module, const std::_tstring & attr, IAttributeType * type, IWorkspaceItem * workspaceItem, bool bHistoryView=false, const CSyntaxErrorVector & attrSyntax = CSyntaxErrorVector());
HWND OpenAttributeMDI(CMainFrame* pFrame, const std::_tstring & module, const std::_tstring & attr, IAttributeType * type, unsigned int row, unsigned int col, IWorkspaceItem * workspaceItem);
HWND OpenAttributeMDI(CMainFrame* pFrame, IAttribute * attribute, IWorkspaceItem * workspaceItem, bool bHistoryView=false, 
	const CSyntaxErrorVector & attrSyntax = CSyntaxErrorVector(), Dali::CEclExceptionVector * errors = NULL, const std::_tstring & searchTerm = _T(""), FINDMODE findmode = FINDMODE_NONE, unsigned int row = 0, unsigned int col = 0);
void SaveAttributeMDI(IAttribute * attribute);
bool GetAttributeText(IAttribute * attribute, CString & result);
bool SetAttributeText(IAttribute * attribute, const std::_tstring & text, bool resetDirtyFlag, bool clearUndo);
void UpdateAttributeMDI(IAttribute * from, IAttribute * to);
