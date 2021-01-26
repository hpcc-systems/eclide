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
void CBookmarksFrame::ParseBookmarksEcl(std::_tstring ecl, std::_tstring user, std::_tstring id, std::_tstring inModule, std::_tstring inAttributeName, IAttributeType *attrType)
{
    m_view.ParseBookmarksEcl(ecl, user, id, inModule, inAttributeName, attrType);
}
void CBookmarksFrame::ParseBookmarksEcl(IAttribute *attribute)
{
    m_view.ParseBookmarks(attribute);
}
void CBookmarksFrame::ParsePathBookmarksEcl(std::_tstring pathStr)
{
    CUnicodeFile file;
    std::_tstring text;
    file.Open(pathStr);
    if (file.IsOpen())
    {
        file.Read(text);
        file.Close();
        m_view.ParseBookmarksEcl(text.c_str(),_T(""), pathStr);
    }
}
bool CBookmarksFrame::HasBookmarks()
{
    return m_view.HasBookmarks();
}
bool CBookmarksFrame::BookmarkSelected()
{
    return m_view.GetSelectedCount() ? true : false;
}
int CBookmarksFrame::GetCount()
{
    return m_view.GetCount();
}
bool CBookmarksFrame::BookmarkFileExists(BM_FILE_TYPE fileType)
{
    return boost::filesystem::exists(m_view.BookmarksFilePath(fileType));
}
void CBookmarksFrame::View()
{
    PostMessage(ID_VIEW_BOOKMARKS);
}
void CBookmarksFrame::Open()
{
    m_view.Open();
}
void CBookmarksFrame::Next()
{
    int pos = m_view.GetListCtrl()->GetSelectionMark();
    m_view.GetListCtrl()->SetItemState(pos, 0, LVIS_FOCUSED | LVIS_SELECTED);
    if (pos + 1 >= m_view.GetListCtrl()->GetItemCount())
    {
        pos = -1;
    }
    pos++;
    m_view.OpenAttribute(pos);
    m_view.GetListCtrl()->SetItemState(pos, TVIS_SELECTED, TVIS_SELECTED);
    m_view.GetListCtrl()->SetSelectionMark(pos);
    m_view.GetListCtrl()->SetFocus();
}
void CBookmarksFrame::Prev()
{
    int pos = m_view.GetListCtrl()->GetSelectionMark();
    m_view.GetListCtrl()->SetItemState(pos, 0, LVIS_FOCUSED | LVIS_SELECTED);
    if (pos == 0)
    {
        pos = m_view.GetListCtrl()->GetItemCount();
    }
    pos--;
    m_view.OpenAttribute(pos);
    m_view.GetListCtrl()->SetItemState(pos, TVIS_SELECTED, TVIS_SELECTED);
    m_view.GetListCtrl()->SetSelectionMark(pos);
    m_view.GetListCtrl()->SetFocus();
}
void CBookmarksFrame::Save(bool saveState)
{
    m_view.Save(saveState);
}
void CBookmarksFrame::Load(bool mergeFlag, BM_FILE_TYPE fileType)
{
    m_view.Load(mergeFlag, fileType);
}
void CBookmarksFrame::Delete()
{
    m_view.DeleteMarkedBookmarks(true);
}
void CBookmarksFrame::Clear()
{
    m_view.Clear();
}
bool CBookmarksFrame::HasSelection()
{
    return m_view.HasSelection();
}
bool CBookmarksFrame::CanSave()
{
    return m_view.CanSave();
}
bool CBookmarksFrame::CanLoad()
{
    return m_view.CanLoad();
}
bool CBookmarksFrame::CanLoadMerge()
{
    return m_view.CanLoadMerge();
}
void CBookmarksFrame::BookmarkFilesState()
{
    m_view.BookmarkFilesState();
}