#pragma once

#include "WorkspaceItem.h"

class CMainFrame;
HWND OpenGraphMDI(CMainFrame* pFrame, IWorkspaceItem * workspaceItem);
bool OpenFileGraphMDI(CMainFrame* pFrame, const CString &filePath, IWorkspaceItem * workspaceItem);

