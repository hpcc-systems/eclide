#include "StdAfx.h"
#include "..\en_us\resource.h"

#include "BookmarksFrame.h"

CBookmarksFrame::CBookmarksFrame()
{
    m_view.Init(this);
}
void CBookmarksFrame::Send_Clear()
{
    m_view.SendMessage(CWM_CLEAR);
}
void CBookmarksFrame::Post_Clear()
{
}
void CBookmarksFrame::Send_Reset()
{
}
void CBookmarksFrame::Post_Reset()
{
}
void CBookmarksFrame::Send_Refresh(ISciBookmarksMarker *errors, size_t curErr)
{
    m_view.SendMessage(CWM_REFRESH, reinterpret_cast<WPARAM>(errors), static_cast<int>(curErr));
}
void CBookmarksFrame::Post_Refresh()
{
}
bool CBookmarksFrame::OnDoubleClick()
{
    return false;
}
bool CBookmarksFrame::OnContext(CMenu &/*menu*/, CPoint &/*pt*/)
{
    return false;
}
void CBookmarksFrame::ParseBookmarksEcl(std::_tstring ecl, std::_tstring user, std::_tstring inModule, std::_tstring inAttributeName, IAttributeType *attrType)
{
    CBookmarksView &bookmarksFrame = (CBookmarksView &)m_view;
    bookmarksFrame.ParseBookmarksEcl(ecl, user, inModule, inAttributeName, attrType);
}
void CBookmarksFrame::ParseBookmarksEcl(IAttribute *attribute)
{
    CBookmarksView &bookmarksFrame = (CBookmarksView &)m_view;
    bookmarksFrame.ParseBookmarks(attribute);
}