#include "stdafx.h"
#include "..\en_us\resource.h"

#include "WorkunitFrame.h"

BEGIN_MESSAGE_MAP(CWorkunitTreeFrame, baseClass)
    ON_MESSAGE(CWM_SYNCTOC, OnSyncToc)
END_MESSAGE_MAP()

CWorkunitTreeFrame::CWorkunitTreeFrame()
{
}

LRESULT CWorkunitTreeFrame::OnSyncToc(WPARAM wParam, LPARAM lParam)
{
    CComPtr<Dali::IWorkunit> wu = reinterpret_cast<Dali::IWorkunit *>(wParam);
    m_view.Locate(wu, true);
    return 0;
}
