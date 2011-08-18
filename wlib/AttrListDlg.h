#pragma once

#include "wlib.h"
#include <Repository.h> 
#include <Workspace.h> 
#include "RepositoryTreeNode.h"

WLIB_API INT_PTR DoShowAttributesDlg(HWND hwndParent, IRepositoryAdapt rep, IWorkspaceVector & workspaces, IAttributeHistoryVector & attrs, bool & sandbox, bool & includeWorkspaces, const TCHAR* title=0, const TCHAR* prompt=0);
WLIB_API void DoShowSyntaxDlg(HWND hwndParent, IRepositoryAdapt rep, IAttributeVector & attrs, IRepositorySlot * repSlot);
WLIB_API void DoConfirmImportDlg(HWND hwndParent, const boost::filesystem::path & path);

