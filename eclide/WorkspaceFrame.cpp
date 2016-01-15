#include "stdafx.h"
#include "..\en_us\resource.h"

#include "WorkspaceFrame.h"

#ifdef WORKSPACE_WINDOW
//  ===========================================================================
CWorkspaceFrame::CWorkspaceFrame()
{
    m_view.Init(this, this, this);
    m_view.SetRootLabel(_T("Workspace"));
}

void CWorkspaceFrame::SetCurrentWorkspace(IWorkspace * workspace)
{
    m_view.SetCurrentWorkspace(workspace);
}

IWorkspace * CWorkspaceFrame::GetCurrentWorkspace()
{
    return m_view.GetCurrentWorkspace();
}

void CWorkspaceFrame::RemoveWorkspace(IWorkspace * workspace)
{
    m_view.RemoveWorkspace(workspace);
}

BEGIN_MESSAGE_MAP(CWorkspaceFrame, baseClass)
    ON_MESSAGE(CWM_SYNCTOC, OnSyncToc)
END_MESSAGE_MAP()

LRESULT CWorkspaceFrame::OnSyncToc(WPARAM wParam, LPARAM lParam)
{
    CComPtr<IAttribute> attr(reinterpret_cast<IAttribute *>(wParam));
    m_view.Select(attr);
    return 0;
}
//  ===========================================================================
#endif