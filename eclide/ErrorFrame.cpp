#include "StdAfx.h"
#include "..\en_us\resource.h"

#include "ErrorFrame.h"

CErrorFrame::CErrorFrame() 
{
}

void CErrorFrame::Send_Clear()
{
    m_view.SendMessage(CWM_CLEAR);
}

void CErrorFrame::Send_Reset()
{
    m_view.SendMessage(CWM_RESET);
}

void CErrorFrame::Post_Reset()
{
    m_view.PostMessage(CWM_RESET);
}

