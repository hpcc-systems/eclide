#include "StdAfx.h"
#include "..\en_us\resource.h"

#include "syntaxframe.h"

CSyntaxFrame::CSyntaxFrame() 
{
	m_view.Init(this);
}
void CSyntaxFrame::Send_Clear()
{
	m_view.SendMessage(CWM_CLEAR);
}
void CSyntaxFrame::Post_Clear()
{
}
void CSyntaxFrame::Send_Reset()
{
}
void CSyntaxFrame::Post_Reset()
{
}
void CSyntaxFrame::Send_Refresh(ISciSyntaxMarker *errors, size_t curErr)
{
	m_view.SendMessage(CWM_REFRESH, reinterpret_cast<WPARAM>(errors), static_cast<int>(curErr));
}
void CSyntaxFrame::Post_Refresh()
{
}
bool CSyntaxFrame::OnDoubleClick()
{
	return false;
}
bool CSyntaxFrame::OnContext(CMenu &/*menu*/, CPoint &/*pt*/)
{
	return false;
}

