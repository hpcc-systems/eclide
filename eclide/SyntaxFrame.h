#pragma once

#include "WTLDockablePane.h"
#include "SyntaxView.h"

class CSyntaxFrame :  
    public CWtlDockablePane<CSyntaxView>,
    public ISyntaxSlot, 
    public boost::signals::trackable
{
    typedef CWtlDockablePane<CSyntaxView> baseClass;
    typedef CSyntaxFrame thisClass;

protected:
    boost::signals::connection m_globalConnection;

public:
    CSyntaxFrame();
    void Send_Clear();
    void Post_Clear();
    void Send_Reset();
    void Post_Reset();
    void Send_Refresh(ISciSyntaxMarker *errors, size_t curErr);
    void Post_Refresh();
    bool OnDoubleClick();
    bool OnContext(CMenu &/*menu*/, CPoint &/*pt*/);
};

