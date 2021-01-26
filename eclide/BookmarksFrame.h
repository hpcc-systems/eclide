#pragma once
#pragma once

#include "WTLDockablePane.h"
#include "BookmarksView.h"

class CBookmarksFrame :
    public CWtlDockablePane<CBookmarksView>,
    public IBookmarksSlot,
    public boost::signals::trackable
{
    typedef CWtlDockablePane<CBookmarksView> baseClass;
    typedef CBookmarksFrame thisClass;

protected:
    boost::signals::connection m_globalConnection;

public:
    CBookmarksFrame();
    void Send_Clear();
    void Post_Clear();
    void Send_Reset();
    void Post_Reset();
    void Send_Refresh(ISciBookmarksMarker *errors, size_t curErr);
    void Post_Refresh();
    bool OnDoubleClick();
    bool OnContext(CMenu &/*menu*/, CPoint &/*pt*/);
    void ParseBookmarksEcl(std::_tstring ecl, std::_tstring user=_T(""), std::_tstring id=_T(""), std::_tstring inModule=_T(""), std::_tstring inAttributeName=_T(""), IAttributeType *attrType=NULL);
    void ParseBookmarksEcl(IAttribute *attribute);
    void ParsePathBookmarksEcl(std::_tstring pathStr);
    bool HasBookmarks();
    bool BookmarkSelected();
    int GetCount();
    bool BookmarkFileExists(BM_FILE_TYPE fileType=BM_FILE_MASTER);
    bool HasSelection();
    bool CanSave();
    bool CanLoad();
    bool CanLoadMerge();
    void BookmarkFilesState();
    void View();
    void Open();
    void Next();
    void Prev();
    void Save(bool saveState=false);
    void Load(bool mergeFlag=false, BM_FILE_TYPE fileType=BM_FILE_MASTER);
    void Delete();
    void Clear();
};

