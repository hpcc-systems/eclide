#pragma once
#include "clib.h"

class CLIB_API CGridFocus
{
public:
    CGridFocus(void);
    ~CGridFocus(void);

    void SetFocus(__int64 row);
    void SetFocus(__int64 row, int column);
    void SetFocusLeft();
    void SetFocusHome();
    void SetFocusRight(int lastColumn);
    void SetFocusEnd(int lastColumn);
    void SetFocusUp();
    void SetFocusDown(__int64 lastRow);
    bool HasFocus(__int64 row, int column);
     
    __int64 inline FocusRow() const { return m_row; };
    int inline FocusColumn() const { return m_column; };

protected:
    __int64 m_row;
    int		m_column;
};
