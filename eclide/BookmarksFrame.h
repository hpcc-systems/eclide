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
    void ParseBookmarksEcl(std::_tstring ecl, std::_tstring user, std::_tstring inModule, std::_tstring inAttributeName, IAttributeType *attrType);
    void ParseBookmarksEcl(IAttribute *attribute);
};

