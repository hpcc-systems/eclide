#include "stdafx.h"
#include "..\en_us\resource.h"

#include "DfuFrame.h"

#ifdef DFU_WINDOWS
//  ===========================================================================

CDfuFrame::CDfuFrame()
{
    m_view.Init(this);
    m_view.SetRootLabel(_T("Datasets"));
    //GetIConfig(QUERYBUILDER_CFG)->ConnectSlot(boost::ref(*this));
}

BEGIN_MESSAGE_MAP(CDfuFrame, baseClass)
    ON_MESSAGE(CWM_SYNCTOC, OnSyncToc)
END_MESSAGE_MAP()

LRESULT CDfuFrame::OnSyncToc(WPARAM wParam, LPARAM lParam)
{
    return 0;
}

//  ===========================================================================

CDfuFilterFrame::CDfuFilterFrame() 
{
    m_view.Init(this);
}

BEGIN_MESSAGE_MAP(CDfuFilterFrame, baseClass)
END_MESSAGE_MAP()

//  ===========================================================================
#endif
