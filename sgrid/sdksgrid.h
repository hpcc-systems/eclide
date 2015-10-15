#pragma once

#include "GridCtrl_src/CellRange.h"

namespace SGRID
{
//------------------------------------------------------------------------------
enum SGRID_MESSAGE 
{
    SGRID_START = 2000,
    SGRID_SETCALLBACKFUNC,
    SGRID_SETCALLBACKFUNC_PREPCACHE,
    SGRID_SETCALLBACKFUNC_CELLCLICK,
    SGRID_SETROWCOUNT,
    SGRID_SETCOLUMNCOUNT,
    SGRID_SETFIXEDROWCOUNT,
    SGRID_SETFIXEDCOLUMNCOUNT,
    SGRID_GETITEMCOUNT,
    SGRID_ENSUREVISIBLE,
    SGRID_GETVISIBLEROWCOUNT,

    SGRID_DOCOPY,

    SGRID_REDRAW,
    SGRID_REDRAWROWS,

    SGRID_SETROWSREQUESTED,
    SGRID_SETROWSLOADED,
    SGRID_ISFULLYLOADED,

    SGRID_SELECTNONE,
    SGRID_SELECTALL,
    SGRID_SELECTCOLUMN,
    SGRID_SELECTROW,

    SGRID_GETFOCUSCELL,
    SGRID_SETFOCUSCELL,
    SGRID_GETCELLFROMPOINT,
    SGRID_GETSELECTED,

    SGRID_SETFONT,

    SGRID_AUTOSIZEROWS,
    SGRID_AUTOSIZECOLUMNS,

    SGRID_SETIMAGELIST,

    SGRID_LAST
};
//------------------------------------------------------------------------------
// Used for Get/SetItem calls.
typedef struct _GV_ITEM {
    int      row,col;     // Row and Column of item
    UINT     mask;        // Mask for use in getting/setting cell data
    UINT     nState;      // cell state (focus/hilighted etc)
    DWORD    nFormat;     // Format of cell
    int      iImage;      // index of the list view item’s icon
    COLORREF crBkClr;     // Background colour (or CLR_DEFAULT)
    COLORREF crFgClr;     // Forground colour (or CLR_DEFAULT)
    LPARAM   lParam;      // 32-bit value to associate with item
    LOGFONT  lfFont;      // Cell font
    UINT     nMargin;     // Internal cell margin
    CString  strText;     // Text in cell
} GV_ITEM;

// This structure sent to Grid's parent in a WM_NOTIFY message
typedef struct tagNM_GRIDVIEW {
    NMHDR hdr;
    int   iRow;
    int   iColumn;
} NM_GRIDVIEW;

// This is sent to the Grid from child in-place edit controls
typedef struct tagGV_DISPINFO {
    NMHDR   hdr;
    GV_ITEM item;
} GV_DISPINFO;

// This is sent to the Grid from child in-place edit controls
typedef struct tagGV_CACHEHINT {
    NMHDR      hdr;
    CCellRange range;
} GV_CACHEHINT;

typedef struct tagGV_CELLRANGE {
    CCellRange range;
} GV_CELLRANGE;

typedef struct tagGV_CELL {
    CCellID cell;
} GV_CELL;

// For virtual mode callback
typedef BOOL (CALLBACK* GRIDCALLBACK)(GV_DISPINFO *, LPARAM);
typedef BOOL (CALLBACK* GRIDCALLBACK_PREPCACHE)(GV_CACHEHINT *, LPARAM);
typedef BOOL (CALLBACK* GRIDCALLBACK_CELLCLICK)(GV_CELL *, LPARAM);

//------------------------------------------------------------------------------
}