#pragma once

#include "GraphTypes.h"	// Todo::Decide if/how/when these become public
#include "MetaContainer.h"
#ifndef EXCLUDE_BOOST
#include "XGMMLSaxHandler.h"
#endif

enum AppMode
{
    AppModeRelavint,
    AppModeGrETL,
    AppModeQBGraph
};

enum AppVisMode
{
    Default=-1, // for App modes that have only one vis mode
    RelavintStandard=0,
    RelavintTimeline=1
};

enum AppZoomMode
{
    AZMSelection, // default
    AZMZoomIn,
    AZMZoomOut,
    AZMMouseWheel
};

enum RendererTypes
{
    rtLegacyGraph,
    rtLegacyVertex,
    rtLegacyEdge,
    rtLegacySubgraph,

    rtGraph,
    rtListSubgraph,
    rtListItemVertex,
    rtListEdge,

    rtIOSubgraph,
    rtInputVertex,
    rtOutputVertex,

    rtDatasetSubgraph,

    rtAttGraph,
    rtAttSubgraph,
    rtAttVertex,
    rtAttEdge,

    rtTimelineGraph
};

namespace SGV {

//  EdgeID Helper(s)  ---
template <class T>
CUniqueID CreateEdgeID(const T & cat, const CUniqueID & fromID, const CUniqueID & toID)
{
    //ATLTRACE("CreateEdgeId FROM ID: %s TO ID: %s", fromID.GetID(), toID.GetID());
    unsigned int fromHash = fromID.GetHash();
    unsigned int toHash = toID.GetHash();
    unsigned int firstHash = fromHash < toHash ? fromHash : toHash;
    unsigned int secondHash = fromHash < toHash ? toHash : fromHash;

    TCHAR firstHashBuff[256];
    TCHAR secondHashBuff[256];
    _itot(firstHash, firstHashBuff, 10); 
    _itot(secondHash, secondHashBuff, 10); 
    return CUniqueID(cat, std::_tstring(firstHashBuff) + _T("--") + secondHashBuff);
}

enum GRAPH_ITEM_TYPE {
     GRAPH_ITEM_TYPE_UNKNOWN = 0,
     GRAPH_ITEM_TYPE_SUBGRAPH = 1,
     GRAPH_ITEM_TYPE_VERTEX = 2,
     GRAPH_ITEM_TYPE_EDGE = 3,
};

// NOTE: *MUST* match definition in GraphViewCtl.idl

enum SGV_NOTIFICATION {
     SGVN_START = 2000,
     SGVN_DOUBLECLICK,
     SGVN_RBUTTONDOWN,
     SGVN_LAST
};

// NOTE: *MUST* match definition in GraphViewCtl.idl
enum LAYOUT {
     LAYOUT_SPRING1 = 0,
     LAYOUT_CIRCLE,
     LAYOUT_NEATO,
     LAYOUT_TWOPI,
     LAYOUT_DOT,
     LAYOUT_CIRCO,
     LAYOUT_SFDP,
     LAYOUT_OSAGE,
     LAYOUT_NULL,
     LAYOUT_FDP,
     LAYOUT_SPRING2,
     LAYOUT_LEE,
     LAYOUT_VISGEDGES,
     LAYOUT_SUBGRAPH_TREE,
     LAYOUT_TIMELINE,
     LAYOUT_LAST
};

enum XML_SAVE_FORMAT
{
    XML_SAVE_FORMAT_ANB_COMPATIBLE = 1,
    XML_SAVE_FORMAT_WITH_IMAGES = 2,
    XML_SAVE_FORMAT_SELECTED_ONLY = 4
};

enum SGV_MESSAGE {
    SGV_START = 2000,
    SGV_GETINTERFACE,
    SGV_CLEAR,
    SGV_CLEAREDGES,
    SGV_SETLAYOUT,
    SGV_GETLAYOUT,
    SGV_SETOWNER,
    SGV_SETRENDERER,
    SGV_SETPROPERTY,
    SGV_GETPROPERTYUNKNOWN,
    SGV_GETPROPERTY,
    SGV_GETPROPERTIES,
    SGV_ADDVERTEX,
    SGV_ADDEDGE,
    SGV_ADDRANDOM,
    SGV_ADDRANDOMTIMES,
    SGV_ADDSUBGRAPH,
    SGV_LOADXGMML,
    SGV_MERGEXGMML,
    SGV_HASITEMS,
    SGV_LOADXML,
    SGV_LOADXMLFILE,
    SGV_GETMOUSEMODE,
    SGV_SETMOUSEMODE,
    SGV_GETSUBGRAPH,
    SGV_GETVERTEX,
    SGV_GETGRAPHITEM,
    SGV_GETVERTEXCOUNT,
    SGV_GETVERTEXNEIGHBOURCOUNT,
    SGV_GETCATEGORIES,
    SGV_GETPARENT,
    SGV_GETSUBGRAPHS,
    SGV_GETCHILDREN,
    SGV_GETVERTICES,
    SGV_GETADJACENTVERTICES,
    SGV_GETEDGES,
    SGV_GETADJACENTEDGES,
    SGV_GETEDGE,
    SGV_GETEDGECOUNT,
    SGV_GETINEDGES,
    SGV_GETOUTEDGES,
    SGV_REMOVEVERTEX,
    SGV_REMOVEEDGE,
    SGV_REMOVESUBGRAPH,
    SGV_GETMETADATA,
    SGV_SETMETADATA,
    SGV_SETTARGETBYCOUNT,
    SGV_SETVERTEXNEIGHBOUR,
    SGV_SETVERTEXVISIBLE,
    SGV_GETVERTEXVISIBLE,
    SGV_SETVERTEXPINNED,
    SGV_GETVERTEXPINNED,
    SGV_SETVERTEXIMAGE,
    SGV_GETVERTEXIMAGE,
    SGV_SETVERTEXIMAGESVISIBLE, // note plural
    SGV_GETVERTEXIMAGESVISIBLE,
    SGV_SETVERTEXIMAGEVISIBLE, // note singular
    SGV_GETVERTEXIMAGEVISIBLE,
    SGV_SETMETAVERTEXIMAGEVISIBLE,
    SGV_GETMETAVERTEXIMAGEVISIBLE,
    SGV_SETEDGEVISIBLE,
    SGV_GETEDGEVISIBLE,
    SGV_SETEDGEIMAGE,
    SGV_GETEDGEIMAGE,
    SGV_SETEDGEIMAGEVISIBLE, 
    SGV_GETEDGEIMAGEVISIBLE,
    SGV_SETMETAEDGEIMAGEVISIBLE, 
    SGV_GETMETAEDGEIMAGEVISIBLE, 
    SGV_SETEDGECOLOR,
    SGV_GETEDGECOLOR,
    SGV_GETEDGESOURCE,
    SGV_GETEDGETARGET,
    SGV_CENTERGRAPH,
    SGV_CENTERGRAPHITEM,
    SGV_CENTERFOCUSNODE,
    SGV_KICKIT,
    SGV_INVALIDATELAYOUT,
    SGV_SETSCALE,
    SGV_GETSCALE,
    SGV_SETSCALETOFIT,
    SGV_SETSCALETOWIDTH,
    SGV_SETITEMSCALETOFIT,
    SGV_SETSPACING,
    SGV_GETSPACING,
    SGV_SETROTATION,
    SGV_GETROTATION,
    SGV_SETOFFSET,
    SGV_GETOFFSET,
    SGV_GETFOCUS,
    SGV_SETFOCUS,
    SGV_GETSELECTED,
    SGV_SETSELECTED,
    SGV_HASSELECTEDVERTEX,
    SGV_GETBOUNDSBOX,
    SGV_SETBOUNDSBOX,
    SGV_RECALCDISPSIZE,
    SGV_GETWINDOWSTATE,
    SGV_SETWINDOWSTATE,
    SGV_GETWINDOWSIZE,
    SGV_SETWINDOWSIZE,
    SGV_RENDERTOCONTEXT,
    SGV_RENDERTOIMAGE,
    SGV_RENDERTOIMAGE1,
    SGV_RENDERTOBITMAP,
    SGV_RENDERTOBITMAP1,
    SGV_RENDERTOCLIPBOARD,
    SGV_SAVEAS,
    SGV_SAVEASNAME,
    SGV_SAVETOXML,
    SGV_SAVETOXMLFILE,
    SGV_GETRENDEREXTENT,
    SGV_FIND,
    SGV_GETDRAGDROPNOTIFICATIONS,
    SGV_SETDRAGDROPNOTIFICATIONS,
    SGV_GETDATA,
    SGV_GETMETA,
    SGV_GETPOSITION,
    SGV_SETPOSITION,
    SGV_GETLABEL,
    SGV_SETLABEL,
    SGV_SETZORDER,
    SGV_SETREDRAW,
    SGV_EDITGRAPHITEM,
    SGV_CREATEIPIC,
    SGV_SETMETAVERTEXIMAGE,
    SGV_SETMETAEDGEIMAGE,
    SGV_SETPROPERTYATTRIBUTE,
    SGV_GETPROPERTYATTRIBUTES,
    SGV_GETPROPERTYATTRIBUTEID,
    SGV_ADDENTITYPROPERTY,
    SGV_GETENTITYPROPERTIES,
    SGV_SETMESSAGE,
    SGV_SHOWOVERVIEW,
    SGV_GETOVERVIEW,
    SGV_SET_EDGE_LABEL_VISIBLE,
    SGV_GET_EDGE_LABEL_VISIBLE,
    SGV_SETOVERVIEWOPACITY,
    SGV_GETOVERVIEWOPACITY,
    SGV_SETOVERVIEWPOSITION,
    SGV_SETOVERVIEWSIZE,
    SGV_EMULATEDECORATION,
    SGV_ISEMULATEDECORATION,
    SGV_SETROWCOMPRESS,
    SGV_GETROWCOMPRESS,
    SGV_SETREADONLY,
    SGV_ISREADONLY,
    SGV_SETEDGEDATERANGE,
    SGV_SETAPPMODE,
    SGV_GETAPPMODE,
    SGV_SETAPPVISMODE,
    SGV_GETAPPVISMODE,
    SGV_SETAPPZOOMMODE,
    SGV_GETAPPZOOMMODE,
    SGV_FINDIDDIALOG,
    SGV_MERGEINTOSUBGRAPH,
    SGV_BREAKSUBGRAPH,
    SGV_SETSUBGRAPHVISIBLE,
    SGV_GETSUBGRAPHVISIBLE,
    SGV_SETSUBGRAPHIMAGE,
    SGV_GETSUBGRAPHIMAGE,
    SGV_SETSUBGRAPHIMAGEVISIBLE, // note singular
    SGV_GETSUBGRAPHIMAGEVISIBLE,
    SGV_SETMETASUBGRAPHIMAGE,
    SGV_GETMETASUBGRAPHIMAGE,
    SGV_SETMETASUBGRAPHIMAGEVISIBLE,
    SGV_GETMETASUBGRAPHIMAGEVISIBLE,
    SGV_SETSUBGRAPHPINNED,
    SGV_GETSUBGRAPHPINNED,
    SGV_SETSUBGRAPHMINIMIZED,
    SGV_GETSUBGRAPHMINIMIZED,
    SGV_ENSUREVISIBLE,
    SGV_LAST
};

typedef struct tagSGV_METADATASTRUCT {
    META_TYPE type;
    META_CAT category;
    CComVariant valueX;
} SGV_METADATASTRUCT,
*PSGV_METADATASTRUCT;

#ifndef EXCLUDE_BOOST
typedef struct tagSGV_METAITERATORPAIRSTRUCT {
    MetaValueMap::const_iterator first;
    MetaValueMap::const_iterator second;
} SGV_METAITERATORPAIRSTRUCT, 
*PSGV_METAITERATORPAIRSTRUCT;
#endif

typedef struct tagSGV_POINT {
    int x, y;
    int co; // from GraphTypes::Coordinates co;
    bool randomize; 
} SGV_POINT,
*PSGV_POINT;

typedef struct tagSGV_SIZE {
    int width;
    int height;
} SGV_SIZE,
*PSGV_SIZE;

typedef struct tagSGV_RECT {
    int left;
    int top;
    int right;
    int bottom;
} SGV_RECT,
*PSGV_RECT;

typedef struct tagSGV_WNDSTATE {
    int size; // IListWindowRenderer::WINDOW_SIZE
    int topindex;
} SGV_WNDSTATE,
*PSGV_WNDSTATE;

typedef struct tagSGV_COLOR {
    BYTE R;
    BYTE G;
    BYTE B;
    BYTE A;
} SGV_COLOR,
*PSGV_COLOR;

typedef struct tagSGV_STRING {
    const TCHAR * string;
} SGV_STRING,
*PSGV_STRING;

typedef struct tagSGV_DATERANGE {
    const TCHAR * from;
    const TCHAR * to;
} SGV_DATERANGE,
*PSGV_DATERANGE;

typedef struct tagSGV_CATEGORYSTRUCT {
    GUID plugin;
    int category;
} SGV_CATEGORYSTRUCT,
*PSGV_CATEGORYSTRUCT;

typedef struct tagSGV_UNIQUEIDSTRUCT {
    SGV_CATEGORYSTRUCT category;
    const TCHAR * id;
} SGV_UNIQUEIDSTRUCT,
*PSGV_UNIQUEIDSTRUCT;

typedef struct tagSGV_EDGESTRUCT
{
    SGV_UNIQUEIDSTRUCT id;
    SGV_UNIQUEIDSTRUCT idFrom;
    SGV_UNIQUEIDSTRUCT idTo;
} SGV_EDGESTRUCT,
*PSGV_EDGESTRUCT;

typedef struct tagSGV_SUBGRAPHSTRUCT
{
    SGV_UNIQUEIDSTRUCT id;
    const TCHAR* label;
    IGraphSubgraph* Subgraph;
    RendererTypes rType;
} SGV_SUBGRAPHSTRUCT,
*PSGV_SUBGRAPHSTRUCT;

typedef struct tagSGV_VERTEXSTRUCT
{
    SGV_UNIQUEIDSTRUCT id;
    IGraphSubgraph* Subgraph;
    RendererTypes rType;
} SGV_VERTEXSTRUCT,
*PSGV_VERTEXSTRUCT;

typedef struct tagSGV_FINDSTRUCT
{
    IUniqueIDContainer* results;
    const TCHAR * searchText;
    bool searchLabel;
    bool searchProperties;
    bool caseSensitive;
    bool visibleOnly;
    bool wholeWord;
    GUID pluginId;
    int categoryId;
    int propertyId;
} SGV_FINDSTRUCT,
*PSGV_FINDSTRUCT;

typedef struct tagSGV_REGIONSTRUCT {
    int x1, x2;
    int y1, y2;
    int w1, w2;
    int h1, h2;
} SGV_REGIONSTRUCT,
*PSGV_REGIONSTRUCT;

typedef struct tagSGV_XMLSAVESTRUCT {
    XML_SAVE_FORMAT saveFormat;
    CPoint relativeSavePos;
} SGV_XMLSAVESTRUCT,
*PSGV_XMLSAVESTRUCT;

typedef struct tagSGV_KEYVAL {
    int key;
    CComVariant val;
} SGV_KEYVAL,
*PSGV_KEYVAL;

typedef struct tagSGV_TARGETSTRUCT {
    SGV_CATEGORYSTRUCT category;
    enum SGV_TARGET_STYLE
    {	
        SGV_TARGET_STYLE_DEFAULT = 0,
        SGV_TARGET_STYLE_EDGE,	
        SGV_TARGET_STYLE_VERTEX
    } style;
    enum SGV_TARGET_STATE
    {
        SGV_TARGET_STATE_DEFAULT = 0,
        SGV_TARGET_STATE_HIDDEN,
        SGV_TARGET_STATE_VISIBLE
    } state;
    int count;
} SGV_TARGETSTRUCT,
*PSGV_TARGETSTRUCT;

typedef struct tagSGV_TRANSFORMSTRUCT {
    bool boolean;
    union {
        float real;
        int	integer;
    } value;
} SGV_TRANSFORMSTRUCT,
*PSGV_TRANSFORMSTRUCT;

typedef struct tagSGV_GRAPHITEMSTATESTRUCT {
    TRISTATE visible;
    TRISTATE pinned;
    TRISTATE selected;
    bool includeHidden;
} SGV_GRAPHITEMSTATESTRUCT,
*PSGV_GRAPHITEMSTATESTRUCT;

typedef struct tagSGV_ADJACENTVERTICESSTRUCT {
    IUniqueIDContainer* results;
    bool in;
    bool out;
} SGV_ADJACENTVERTICESSTRUCT,
*PSGV_ADJACENTVERTICESSTRUCT;

typedef struct tagSGV_GRAPHITEMIDANDSTATETRUCT {
    IUniqueIDContainer* results;
    TRISTATE visible;
    TRISTATE pinned;
    TRISTATE selected;
} SGV_GRAPHITEMIDANDSTATESTRUCT,
*PSGV_GRAPHITEMIDANDSTATESTRUCT;

typedef struct tagSGV_IMAGEINFOSTRUCT 
{
    ImageInfo imginfo;
    unsigned int overlay;
} SGV_IMAGEINFOSTRUCT,*PSGV_IMAGEINFOSTRUCT;

typedef struct tagSGV_IMAGEVISSTRUCT
{
    IMAGE_POSITION pos;
    unsigned int overlay;
    VISIBLE vis;
    bool itemSpecific;
} SGV_IMAGEVISSTRUCT,*PSGV_IMAGEVISSTRUCT;

// SGV_PROPERTYSTRUCT
// catch all struct for property-related calls
// initalize what you need and ignore the rest

typedef struct tagSGV_PROPERTYSTRUCT 
{
    int propid;
    PROPERTY_ATTR attr;
    CComVariant value;
    META_TYPE entity;
    SGV_CATEGORYSTRUCT cat;
    bool autoGenId;
} SGV_PROPERTYSTRUCT,*PSGV_PROPERTYSTRUCT;

typedef struct tagSGV_LOADXMLSTRUCT 
{
    int version;
    const TCHAR* xml;
} SGV_LOADXMLSTRUCT,*PSGV_LOADXMLSTRUCT;

#ifndef EXCLUDE_BOOST
typedef struct tagSGV_XGMMLSTRUCT 
{
    const TCHAR* xgmml;
    WUVisualState state;
    bool appendMissing;
    bool redundantSubgraphs;
    bool passthroughVertices;
    bool minimizeIfLarge;
    const IXGMMLRenderCallback * callback;
} SGV_XGMMLSTRUCT,*PSGV_XGMMLSTRUCT;
#endif

typedef struct tagSGV_MERGEINTOSUBGRAPHSTRUCT 
{
    const TCHAR * label;
    IUniqueIDContainer* nodes;
} SGV_MERGEINTOSUBGRAPHSTRUCT,*PSGV_MERGEINTOSUBGRAPHSTRUCT;

}	// namespace SGV

