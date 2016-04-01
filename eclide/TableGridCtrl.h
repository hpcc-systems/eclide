#pragma once

#include "atlsgrid.h"
#include "table.h"

//  ===========================================================================
class CTableGridCtrl : 
    public SGRID::CGridCtrl
{
    typedef CTableGridCtrl thisClass;
    typedef SGRID::CGridCtrl baseClass;

protected:
    StlLinked<ITable> m_data;

public:
    CTableGridCtrl();
    ~CTableGridCtrl();
    
    void SetData(const ITable* data, bool hasRowNum);
    
    virtual BOOL Callback(SGRID::GV_DISPINFO *pDispInfo);
    virtual BOOL CallbackPrepCache(SGRID::GV_CACHEHINT *pDispInfo);
    virtual BOOL CallbackCellClick(SGRID::GV_CELL *pCell);

    void GotoLine(__int64 line, int col);
    __int64 CurrentLine();
    __int64 HitTestForColumn(const POINT &pnt, int *pCol);
    bool HasSelection();

    //  Statics  ---
    static BOOL CALLBACK GridCallback(SGRID::GV_DISPINFO *pDispInfo, LPARAM lParam);
    static BOOL CALLBACK GridCallbackPrepCache(SGRID::GV_CACHEHINT *pDispInfo, LPARAM lParam);
    static BOOL CALLBACK GridCallbackCellClick(SGRID::GV_CELL *pCell, LPARAM lParam);
};
//  ===========================================================================
