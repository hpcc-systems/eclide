#pragma once

#include <GraphContainer.h>
#include "GraphViewCtlApi.h"
#include "IPictureHelper.h"

#ifdef SEISINT_LIBEXPORTS
#  define GRAPHVIEWCTL_API
#else
#  ifdef GRAPHVIEWCTL_EXPORTS
#    define GRAPHVIEWCTL_API __declspec(dllexport)
#  else
#    define GRAPHVIEWCTL_API __declspec(dllimport)
#  endif
#endif

namespace SGV
{
__interface IGraphViewSlot //: public IUnknown
{
    void OnContextMenu(const CPoint& pos);
    void OnDoubleClick(const CPoint& pos);
    void OnDrop(HGLOBAL hg);
    void OnOver(const CPoint& pos);
    void OnSelected();

    void OnDoDragDrop(const CPoint& dragStartPos);
    bool OnLayoutFinished();
    void OnMetaLoaded();
    void OnScaleChanged();
    void OnSpacingChanged();

    void OnVertexClick(const CUniqueID &id, const CPoint& pos);
    void OnVertexContextMenu(const CUniqueID &id, const CPoint& pos);
    void OnVertexDoubleClick(const CUniqueID &id, const CPoint& pos);
    void OnVertexGotFocus(const CUniqueID &id);
    void OnVertexLostFocus(const CUniqueID &id);
    void OnVertexOver(const CUniqueID &id, const CPoint& pos);
    void OnVertexSelectionChanged(const CUniqueID &id);
    void OnVertexVisibilityChanged(const CUniqueID &id);
    void OnVertexCategoryVisibilityChanged(const CCategory &cat);

    void OnEdgeClick(const CUniqueID &id, const CPoint& pos);
    void OnEdgeContextMenu(const CUniqueID &id, const CPoint& pos);
    void OnEdgeDoubleClick(const CUniqueID &id, const CPoint& pos);

    void OnSubgraphClick(const CUniqueID &id, const CPoint& pos);
    void OnSubgraphContextMenu(const CUniqueID &id, const CPoint& pos);
    void OnSubgraphDoubleClick(const CUniqueID &id, const CPoint& pos);
    void OnSubgraphGotFocus(const CUniqueID &id);
    void OnSubgraphLostFocus(const CUniqueID &id);
    void OnSubgraphOver(const CUniqueID &id, const CPoint& pos);
    void OnSubgraphSelectionChanged(const CUniqueID &id);
    void OnSubgraphVisibilityChanged(const CUniqueID &/*id*/);
    void OnSubgraphCategoryVisibilityChanged(const CCategory &/*id*/);

    void OnTooltipLinkClick(const std::_tstring & linkAddress);
};

//------------------------------------------------------------------------------

template< class TBase > class CGraphViewCtrlT;		// forward reference

//------------------------------------------------------------------------------
// TODO: replace IGraphEdge and IGraphVertex with fuller versions of these:
class CGraphMeta
{
protected:
    HWND m_hWnd;

    CGraphMeta() {};

    void SetUIDData(SGV_UNIQUEIDSTRUCT& id_struct, const CCategory& cat, const TCHAR* id) const
    {
        id_struct.category.plugin = cat.GetPlugin().GetGUID();
        id_struct.category.category = cat.GetCategory();
        id_struct.id = id;
    }

    void SetImage(
        SGV_MESSAGE msg, // vertex or edge...
        const CUniqueID& uid,
        GraphTypes::IBitmap* image, 
        IMAGE_POSITION pos=POSITION_WEST,
        int marginX=0,
        int marginY=0,
        unsigned int overlay=0,
        COLORREF transcolor=0x00ffffff)
    {
        if (image != 0 && !image->IsValid())
            return;

        if (image != 0 && transcolor == 0x00ffffff)
            GetFirstPixel(*(image->GetCBitmap()), transcolor);

        ATLASSERT((transcolor & 0xff000000) == 0); // shouldn't ever happen (now)
        //	transcolor ^= 0xff000000; // remove alpha

        SGV_UNIQUEIDSTRUCT id_struct;
        SetUIDData(id_struct, uid.GetCategory(), uid.GetID());
        // for meta calls ID is ignored, only for vertex and edge specific calls is this used

        SGV_IMAGEINFOSTRUCT iinfo;
        iinfo.imginfo.image=image;
        iinfo.imginfo.pos=pos;
        iinfo.imginfo.marginX=marginX;
        iinfo.imginfo.marginY=marginY;
        iinfo.imginfo.transcolor=transcolor;
        iinfo.overlay=overlay;

        SendMessage(m_hWnd, msg, (WPARAM)&id_struct, (LPARAM)&iinfo);
    }

    HBITMAP GetImage(
        SGV_MESSAGE msg, // vertex or edge...
        const CUniqueID& uid,
        IMAGE_POSITION pos=POSITION_WEST,
        unsigned int overlay=0,
        bool itemSpecific=true)
    {
        SGV_UNIQUEIDSTRUCT id_struct;
        SetUIDData(id_struct, uid.GetCategory(), uid.GetID());
        // for meta calls ID is ignored, only for vertex and edge specific calls is this used

        SGV_IMAGEVISSTRUCT imgvis;
        imgvis.pos=pos;
        imgvis.overlay=overlay;
        imgvis.itemSpecific=itemSpecific;

        CComPtr<GraphTypes::IBitmap> iBmp = reinterpret_cast<GraphTypes::IBitmap*>(SendMessage(m_hWnd, msg, (WPARAM)&id_struct, (LPARAM)&imgvis));
        // NOTE: it is the callers responsibility to call DeleteObject to release the handle.
        if (iBmp != 0)
        {
            HBITMAP hBitmap = iBmp->GetCBitmap()->Detach();
            return hBitmap;
        }
        return 0;
    }

    void SetImageVisible(
        SGV_MESSAGE msg,
        const CUniqueID& uid,
        IMAGE_POSITION pos,
        bool visible)
    {
        SGV_UNIQUEIDSTRUCT id_struct;
        SetUIDData(id_struct, uid.GetCategory(), uid.GetID());
        // for meta calls ID is ignored, only for vertex and edge specific calls is this used

        SGV_IMAGEVISSTRUCT imgvis;
        imgvis.pos=pos;
        imgvis.vis=visible?VISIBLE_SHOW:VISIBLE_HIDE;

        SendMessage(m_hWnd, msg, (WPARAM)&id_struct, (LPARAM)&imgvis);
    }

    bool GetImageVisible(
        SGV_MESSAGE msg,
        const CUniqueID& uid,
        IMAGE_POSITION pos)
    {
        SGV_UNIQUEIDSTRUCT id_struct;
        SetUIDData(id_struct, uid.GetCategory(), uid.GetID());
        // for meta calls ID is ignored, only for vertex and edge specific calls is this used

        return SendMessage(m_hWnd, msg, (WPARAM)&id_struct, (LPARAM)pos)!=0;
    }

public:
    CGraphMeta(HWND hWnd) : m_hWnd(hWnd)
    {
    }

    void SetEdgeImage(
        const CCategory& cat,
        GraphTypes::IBitmap* image,
        IMAGE_POSITION pos=POSITION_WEST,
        int marginX=0,
        int marginY=0,
        unsigned int overlay=0,
        COLORREF transcolor=0x00ffffff)
    {
        SetImage(SGV_SETMETAEDGEIMAGE, CUniqueID(cat, _T("")), image, pos, marginX, marginY, overlay, transcolor);
    }

    HBITMAP GetEdgeImage(const CCategory& cat, IMAGE_POSITION pos, unsigned int overlay=0)
    {
        return GetImage(SGV_GETEDGEIMAGE, CUniqueID(cat, _T("")), pos, overlay, false);
    }

    void SetEdgeImageVisible(const CCategory& cat, IMAGE_POSITION pos, bool visible)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SetImageVisible(SGV_SETMETAEDGEIMAGEVISIBLE, CUniqueID(cat, _T("")), pos, visible);
    }

    bool GetEdgeImageVisible(const CCategory& cat, IMAGE_POSITION pos)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        return GetImageVisible(SGV_GETMETAEDGEIMAGEVISIBLE, CUniqueID(cat, _T("")), pos);
    }

    void SetVertexImage(
        const CCategory& cat,
        GraphTypes::IBitmap* ibmp,
        IMAGE_POSITION pos=POSITION_WEST,
        int marginX=0,
        int marginY=0,
        unsigned int overlay=0,
        COLORREF transcolor=0x00ffffff)
    {
        SetImage(SGV_SETMETAVERTEXIMAGE, CUniqueID(cat, _T("")), ibmp, pos, marginX, marginY, overlay, transcolor);
    }

    HBITMAP GetVertexImage(const CCategory& cat, IMAGE_POSITION pos, unsigned int overlay=0)
    {
        return GetImage(SGV_GETVERTEXIMAGE, CUniqueID(cat, _T("")), pos, overlay, false);
    }

    void SetVertexImageVisible(const CCategory& cat, IMAGE_POSITION pos, bool visible)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SetImageVisible(SGV_SETMETAVERTEXIMAGEVISIBLE, CUniqueID(cat, _T("")), pos, visible);
    }

    bool GetVertexImageVisible(const CCategory& cat, IMAGE_POSITION pos)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        return GetImageVisible(SGV_GETMETAVERTEXIMAGEVISIBLE, CUniqueID(cat, _T("")), pos);
    }

    void SetSubgraphImage(
        const CCategory& cat,
        GraphTypes::IBitmap* ibmp,
        IMAGE_POSITION pos=POSITION_WEST,
        int marginX=0,
        int marginY=0,
        unsigned int overlay=0,
        COLORREF transcolor=0x00ffffff)
    {
        SetImage(SGV_SETMETASUBGRAPHIMAGE, CUniqueID(cat, _T("")), ibmp, pos, marginX, marginY, overlay, transcolor);
    }

    HBITMAP GetSubgraphImage(const CCategory& cat, IMAGE_POSITION pos, unsigned int overlay=0, bool itemSpecific=false)
    {
        return GetImage(SGV_GETSUBGRAPHIMAGE, CUniqueID(cat, _T("")), pos, overlay, itemSpecific);
    }

    void SetSubgraphImageVisible(const CCategory& cat, IMAGE_POSITION pos, bool visible)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SetImageVisible(SGV_SETMETASUBGRAPHIMAGEVISIBLE, CUniqueID(cat, _T("")), pos, visible);
    }

    bool GetSubgraphImageVisible(const CCategory& cat, IMAGE_POSITION pos)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        return GetImageVisible(SGV_GETMETASUBGRAPHIMAGEVISIBLE, CUniqueID(cat, _T("")), pos);
    }

    unsigned int GetPropertyAttributes(IPropAttributeContainer* storage, META_TYPE entityType, const CCategory& category, const int id)
    {
        ATLASSERT(::IsWindow(m_hWnd));

        SGV_PROPERTYSTRUCT props;

        props.cat.plugin=category.GetPlugin().GetGUID();
        props.cat.category=category.GetCategory();
        props.propid=id;
        props.entity = entityType;

        return ::SendMessage(m_hWnd, SGV_GETPROPERTYATTRIBUTES, (WPARAM)&props, (LPARAM)storage);
    }

    const CComVariant& GetPropertyAttribute(CComVariant& retVal, PROPERTY_ATTR prop_attr, META_TYPE entityType, const CCategory& category, const int id)
    {
        CPropAttributeVector prop_attrs;
        CGraphMeta::GetPropertyAttributes(&prop_attrs, entityType, category, id);

        // TODO: this could all be optimised internally to use a map?
        while (prop_attrs.MoveNext())
        {
            AttrValue attrValue = prop_attrs.Get().first;
            if (attrValue.attr == prop_attr)
            {
                retVal = attrValue.value;
                break;
            }
        }
        return retVal;
    }

    unsigned int GetPropertyAttributeId(const CCategory& category, META_TYPE entityType, const PROPERTY_ATTR& attr, const CComVariant& value, bool autoGenId)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SGV_PROPERTYSTRUCT data;
        data.attr = attr;
        data.autoGenId = autoGenId;
        data.value = value;
        data.entity = entityType;
        data.cat.plugin = category.GetPlugin().GetGUID();
        data.cat.category = category.GetCategory();
        ::SendMessage(m_hWnd, SGV_GETPROPERTYATTRIBUTEID, (WPARAM)&data, (LPARAM)0);
        return data.propid;
    }

    void SetPropertyAttribute(META_TYPE entityType, const CCategory& category, const int id, PROPERTY_ATTR attr, const CComVariant& value)
    {
        ATLASSERT(::IsWindow(m_hWnd));

        SGV_PROPERTYSTRUCT props;

        props.cat.plugin=category.GetPlugin().GetGUID();
        props.cat.category=category.GetCategory();
        props.propid=id;
        props.attr=attr;
        props.entity = entityType;
        props.value=value;

        ::SendMessage(m_hWnd, SGV_SETPROPERTYATTRIBUTE, (WPARAM)&props, 0);
    }
};

//------------------------------------------------------------------------------

class CGraphItem : public CGraphMeta
{
protected:
    CUniqueID m_uniqueID;
    SGV_UNIQUEIDSTRUCT m_id_struct;

    CGraphItem() {};

    void SetUIDMsg(SGV_MESSAGE msg, bool visible)
    {
        // for meta calls ID is ignored, only for vertex and edge specific calls is this used
        SendMessage(m_hWnd, msg, (WPARAM)&m_id_struct, (LPARAM)visible);
    }

    bool GetUIDMsg(SGV_MESSAGE msg)
    {
        return SendMessage(m_hWnd, msg, (WPARAM)&m_id_struct, (LPARAM)0)!=0;
    }

public:
    CGraphItem(HWND hWnd, CUniqueID uniqueID)
        : CGraphMeta(hWnd), m_uniqueID(uniqueID)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        ATLASSERT(!uniqueID.isEmpty());
        SetUIDData(m_id_struct, m_uniqueID.GetCategory(), m_uniqueID.GetID());
    }

    const CUniqueID & GetID()
    {
        return m_uniqueID;
    }

    const TCHAR* GetLabel(std::_tstring & label)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        std::_tstring * pStr = &label;
        ::SendMessage(m_hWnd, SGV_GETLABEL, (WPARAM)&m_id_struct, (LPARAM)pStr);
        return label.c_str();
    }

    void SetLabel(const TCHAR* label)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        ::SendMessage(m_hWnd, SGV_SETLABEL, (WPARAM)&m_id_struct, (LPARAM)label);
    }

    virtual void SetVisible(bool visible) = 0;

    virtual bool GetVisible() = 0;

    virtual void SetImage(
        GraphTypes::IBitmap* image,
        IMAGE_POSITION pos=POSITION_WEST,
        int marginX=0,
        int marginY=0,
        unsigned int overlay=0,
        COLORREF transcolor=0x00ffffff) = 0;

    virtual HBITMAP GetImage(
        IMAGE_POSITION pos,
        unsigned int overlay=0,
        bool itemSpecific=true) = 0;

    virtual void SetPinned(bool pinned) = 0;

    virtual bool GetPinned() = 0;

    virtual void SetImageVisible(IMAGE_POSITION pos, bool visible) = 0;

    virtual bool GetImageVisible(IMAGE_POSITION pos) = 0;

    virtual unsigned int GetPropertyAttributes(IPropAttributeContainer* storage, const CCategory& category, const int id) = 0;

    virtual const CComVariant& GetPropertyAttribute(CComVariant& retVal, PROPERTY_ATTR prop_attr, const CCategory& category, const int id) = 0;

    virtual unsigned int GetPropertyAttributeId(const CCategory& category, const PROPERTY_ATTR& attr, const CComVariant& value, bool autoGenId=false) = 0;

    virtual void SetPropertyAttribute(META_TYPE entityType, const CCategory& category, const int id, PROPERTY_ATTR attr, const CComVariant& value) = 0;
};

//------------------------------------------------------------------------------

class CGraphEdge : public CGraphItem
{
protected:
    CGraphEdge() {};
public:
    CGraphEdge(HWND hWnd, CUniqueID uniqueID)
        : CGraphItem(hWnd, uniqueID)
    {
    }

    void SetColor(const GraphTypes::Color& color)
    {
        ATLASSERT(::IsWindow(m_hWnd));

        SGV_COLOR sgvcolor;

        sgvcolor.R=color.GetR();
        sgvcolor.G=color.GetG();
        sgvcolor.B=color.GetB();
        sgvcolor.A=color.GetA();

        ::SendMessage(m_hWnd, SGV_SETEDGECOLOR, (WPARAM)&m_id_struct, (LPARAM)&sgvcolor);
    }

    void SetEdgeDateRange(std::_tstring & from, std::_tstring & to)  //UST
    {
        ATLASSERT(::IsWindow(m_hWnd));

        SGV_DATERANGE dateRange;

        dateRange.from = from.c_str();
        dateRange.to = to.c_str();
        ::SendMessage(m_hWnd, SGV_SETEDGEDATERANGE, (WPARAM)&m_id_struct, (LPARAM)&dateRange);
    }

    void GetColor(GraphTypes::Color& color) const
    {
        ATLASSERT(::IsWindow(m_hWnd));

        SGV_COLOR sgvcolor;
        ::SendMessage(m_hWnd, SGV_GETEDGECOLOR, (WPARAM)&m_id_struct, (LPARAM)&sgvcolor);

        GraphTypes::Color c(sgvcolor.A, sgvcolor.R, sgvcolor.G, sgvcolor.B);
        color=c;
    }

    const CUniqueID& GetSource(CUniqueID& retVal) const
    {
        ::SendMessage(m_hWnd, SGV_GETEDGESOURCE, (WPARAM)&m_id_struct, (LPARAM)&retVal);
        return retVal; // merely convenient
    }

    const CUniqueID& GetTarget(CUniqueID& retVal) const
    {
        ::SendMessage(m_hWnd, SGV_GETEDGETARGET, (WPARAM)&m_id_struct, (LPARAM)&retVal);
        return retVal; // merely convenient
    }

    void SetVisible(bool visible)
    {
        SetUIDMsg(SGV_SETEDGEVISIBLE, visible);
    }

    bool GetVisible() 
    {
        return GetUIDMsg(SGV_GETEDGEVISIBLE);
    }

    void SetPinned(bool /*pinned*/)
    {
        // TODO: here for convenience of CGraphItem, does Pinning make sense for an Edge?
        // Could it mean pin both vertices perhaps?
    }

    bool GetPinned()
    {
        return false;
    }

    void SetImage(
        GraphTypes::IBitmap* image,
        IMAGE_POSITION pos=POSITION_WEST,
        int marginX=0,
        int marginY=0,
        unsigned int overlay=0,
        COLORREF transcolor=0x00ffffff)
    {
        CGraphMeta::SetImage(SGV_SETEDGEIMAGE, m_uniqueID, image, pos, marginX, marginY, overlay, transcolor);
    }

    HBITMAP GetImage(
        IMAGE_POSITION pos,
        unsigned int overlay=0,
        bool itemSpecific=true)
    {
        return CGraphMeta::GetImage(SGV_GETEDGEIMAGE, m_uniqueID, pos, overlay, itemSpecific);
    }

    void SetImageVisible(IMAGE_POSITION pos, bool visible)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        CGraphMeta::SetImageVisible(SGV_SETEDGEIMAGEVISIBLE, m_uniqueID, pos, visible);
    }

    bool GetImageVisible(IMAGE_POSITION pos)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        return CGraphMeta::GetImageVisible(SGV_GETEDGEIMAGEVISIBLE, m_uniqueID, pos);
    }

    unsigned int GetPropertyAttributes(IPropAttributeContainer* storage, const CCategory& category, const int id)
    {
        return CGraphMeta::GetPropertyAttributes(storage, META_EDGE, category, id);
    }

    const CComVariant& GetPropertyAttribute(CComVariant& retVal, PROPERTY_ATTR prop_attr, const CCategory& category, const int id)
    {
        return CGraphMeta::GetPropertyAttribute(retVal, prop_attr, META_EDGE, category, id);
    }

    unsigned int GetPropertyAttributeId(const CCategory& category, const PROPERTY_ATTR& attr, const CComVariant& value, bool autoGenId=false)
    {
        return CGraphMeta::GetPropertyAttributeId(category, META_EDGE, attr, value, autoGenId);
    }

    void SetPropertyAttribute(META_TYPE /*entityType*/, const CCategory& category, const int id, PROPERTY_ATTR attr, const CComVariant& value)
    {
        CGraphMeta::SetPropertyAttribute(META_EDGE, category, id, attr, value);
    }
};

//------------------------------------------------------------------------------
class CGraphVertex : public CGraphItem
{
protected:
    CGraphVertex() {};
public:
    CGraphVertex(HWND hWnd, CUniqueID uniqueID)
        : CGraphItem(hWnd, uniqueID)
    {
    }

    void SetPinned(bool pinned)
    {
        SetUIDMsg(SGV_SETVERTEXPINNED, pinned);
    }

    bool GetPinned()
    {
        return GetUIDMsg(SGV_GETVERTEXPINNED);
    }

    void SetVisible(bool visible)
    {
        SetUIDMsg(SGV_SETVERTEXVISIBLE, visible);
    }

    bool GetVisible()
    {
        return GetUIDMsg(SGV_GETVERTEXVISIBLE);
    }

    bool EnsureVisible()
    {
        return GetUIDMsg(SGV_ENSUREVISIBLE);
    }

    void SetImage(
        GraphTypes::IBitmap* image,
        IMAGE_POSITION pos=POSITION_WEST,
        int marginX=0,
        int marginY=0,
        unsigned int overlay=0,
        COLORREF transcolor=0x00ffffff)
    {
        CGraphMeta::SetImage(SGV_SETVERTEXIMAGE, m_uniqueID, image, pos, marginX, marginY, overlay, transcolor);
    }

    HBITMAP GetImage(
        IMAGE_POSITION pos,
        unsigned int overlay=0,
        bool itemSpecific=true)
    {
        return CGraphMeta::GetImage(SGV_GETVERTEXIMAGE, m_uniqueID, pos, overlay, itemSpecific);
    }

    void SetImageVisible(IMAGE_POSITION pos, bool visible)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        CGraphMeta::SetImageVisible(SGV_SETVERTEXIMAGEVISIBLE, m_uniqueID, pos, visible);
    }

    bool GetImageVisible(IMAGE_POSITION pos)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        return CGraphMeta::GetImageVisible(SGV_GETVERTEXIMAGEVISIBLE, m_uniqueID, pos);
    }

    unsigned int GetPropertyAttributes(IPropAttributeContainer* storage, const CCategory& category, const int id)
    {
        return CGraphMeta::GetPropertyAttributes(storage, META_VERTEX, category, id);
    }

    const CComVariant& GetPropertyAttribute(CComVariant& retVal, PROPERTY_ATTR prop_attr, const CCategory& category, const int id)
    {
        return CGraphMeta::GetPropertyAttribute(retVal, prop_attr, META_VERTEX, category, id);
    }

    unsigned int GetPropertyAttributeId(const CCategory& category, const PROPERTY_ATTR& attr, const CComVariant& value, bool autoGenId=false)
    {
        return CGraphMeta::GetPropertyAttributeId(category, META_VERTEX, attr, value, autoGenId);
    }

    void SetPropertyAttribute(META_TYPE /*entityType*/, const CCategory& category, const int id, PROPERTY_ATTR attr, const CComVariant& value)
    {
        CGraphMeta::SetPropertyAttribute(META_VERTEX, category, id, attr, value);
    }
};

//------------------------------------------------------------------------------
class CGraphSubgraph : public CGraphItem
{
protected:
    CGraphSubgraph() {};
public:
    CGraphSubgraph(HWND hWnd, CUniqueID uniqueID)
        : CGraphItem(hWnd, uniqueID)
    {
    }

    const CGraphSubgraph & GetParent(CGraphSubgraph & parent) const
    {
        SGV_UNIQUEIDSTRUCT parentID;
        ::SendMessage(m_hWnd, SGV_GETPARENT, (WPARAM)&m_id_struct, (LPARAM)&parentID);
        parent = CGraphSubgraph(m_hWnd, CUniqueID(parentID.category.plugin, parentID.category.category, parentID.id));
        return parent;
    }

    unsigned GetChildren(IUniqueIDContainer * results) const
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SGV_GRAPHITEMIDANDSTATESTRUCT data;
        data.results = results;
        data.visible = TRISTATE_BOTH;
        data.pinned = TRISTATE_BOTH;
        data.selected = TRISTATE_BOTH;
        return (unsigned)::SendMessage(m_hWnd, SGV_GETCHILDREN, (WPARAM)&m_id_struct, (LPARAM)&data);
    }

    void SetPinned(bool pinned)
    {
        SetUIDMsg(SGV_SETSUBGRAPHPINNED, pinned);
    }

    bool GetPinned()
    {
        return GetUIDMsg(SGV_GETSUBGRAPHPINNED);
    }

    void SetMinimized(bool minimized)
    {
        SetUIDMsg(SGV_SETSUBGRAPHMINIMIZED, minimized);
    }

    bool GetMinimized()
    {
        return GetUIDMsg(SGV_GETSUBGRAPHMINIMIZED);
    }

    void SetVisible(bool visible)
    {
        SetUIDMsg(SGV_SETSUBGRAPHVISIBLE, visible);
    }

    bool GetVisible()
    {
        return GetUIDMsg(SGV_GETSUBGRAPHVISIBLE);
    }

    void SetImage(
        GraphTypes::IBitmap* image,
        IMAGE_POSITION pos=POSITION_WEST,
        int marginX=0,
        int marginY=0,
        unsigned int overlay=0,
        COLORREF transcolor=0x00ffffff)
    {
        CGraphMeta::SetImage(SGV_SETSUBGRAPHIMAGE, m_uniqueID, image, pos, marginX, marginY, overlay, transcolor);
    }

    HBITMAP GetImage(
        IMAGE_POSITION pos,
        unsigned int overlay=0,
        bool itemSpecific=true)
    {
        return CGraphMeta::GetImage(SGV_GETSUBGRAPHIMAGE, m_uniqueID, pos, overlay, itemSpecific);
    }

    void SetImageVisible(IMAGE_POSITION pos, bool visible)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        CGraphMeta::SetImageVisible(SGV_SETSUBGRAPHIMAGEVISIBLE, m_uniqueID, pos, visible);
    }

    bool GetImageVisible(IMAGE_POSITION pos)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        return CGraphMeta::GetImageVisible(SGV_GETSUBGRAPHIMAGEVISIBLE, m_uniqueID, pos);
    }

    unsigned int GetPropertyAttributes(IPropAttributeContainer* storage, const CCategory& category, const int id)
    {
        return CGraphMeta::GetPropertyAttributes(storage, META_SUBGRAPH, category, id);
    }

    const CComVariant& GetPropertyAttribute(CComVariant& retVal, PROPERTY_ATTR prop_attr, const CCategory& category, const int id)
    {
        return CGraphMeta::GetPropertyAttribute(retVal, prop_attr, META_SUBGRAPH, category, id);
    }

    unsigned int GetPropertyAttributeId(const CCategory& category, const PROPERTY_ATTR& attr, const CComVariant& value, bool autoGenId=false)
    {
        return CGraphMeta::GetPropertyAttributeId(category, META_SUBGRAPH, attr, value, autoGenId);
    }

    void SetPropertyAttribute(META_TYPE /*entityType*/, const CCategory& category, const int id, PROPERTY_ATTR attr, const CComVariant& value)
    {
        CGraphMeta::SetPropertyAttribute(META_SUBGRAPH, category, id, attr, value);
    }
};

//------------------------------------------------------------------------------
template< class TBase >
class CGraphViewCtrlT : public TBase
{
protected:
    void SetUIDData(SGV_UNIQUEIDSTRUCT& id_struct, const CCategory& cat, const TCHAR* id) const
    {
        id_struct.category.plugin = cat.GetPlugin().GetGUID();
        id_struct.category.category = cat.GetCategory();
        id_struct.id = id;
    }
public:
    CGraphViewCtrlT(HWND hWnd = NULL) : TBase(hWnd) { }

    CGraphViewCtrlT< TBase >& operator=(HWND hWnd)
    {
        m_hWnd = hWnd;
        return *this;
    }

    HWND Create(HWND hWndParent, ATL::_U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
        DWORD dwStyle = 0, DWORD dwExStyle = 0,
        ATL::_U_MENUorID MenuOrID = 0U, HINSTANCE instance = NULL, LPVOID lpCreateParam = NULL)
    {
        if(rect.m_lpRect == NULL)
            rect.m_lpRect = &rcDefault;
        m_hWnd = ::CreateWindowEx(dwExStyle, GetWndClassName(), szWindowName, dwStyle, rect.m_lpRect->left, rect.m_lpRect->top, (rect.m_lpRect->right - rect.m_lpRect->left) / 2, (rect.m_lpRect->bottom - rect.m_lpRect->top) / 2, hWndParent, MenuOrID.m_hMenu, instance, lpCreateParam);
        return m_hWnd;
    }

    // Attributes

    static LPCTSTR GetWndClassName()
    {
        return _T("SGV_GraphView");
    }

    static LPCTSTR GetLibraryName()
    {
#if _MSC_VER < 1400
        return _T("GRAPHVIEWCTL.DLL");
#else
        return _T("GRAPHVIEWCTL.DLL");
#endif
    }

    HWND GetHWND()
    {
        ATLASSERT(::IsWindow(m_hWnd));
        return ::IsWindow(m_hWnd) ? m_hWnd : 0;
    }

    void Clear()
    {
        ATLASSERT(::IsWindow(m_hWnd));
        ::SendMessage(m_hWnd, SGV_CLEAR, (WPARAM)0, (LPARAM)0);
    }

    void ClearEdges()
    {
        ATLASSERT(::IsWindow(m_hWnd));
        ::SendMessage(m_hWnd, SGV_CLEAREDGES, (WPARAM)0, (LPARAM)0);
    }

    void SetMessage(const std::_tstring & msg)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        ::SendMessage(m_hWnd, SGV_SETMESSAGE, (WPARAM)msg.c_str(), (LPARAM)0);
    }

    void ShowOverview(bool bshow)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        ::SendMessage(m_hWnd, SGV_SHOWOVERVIEW, (WPARAM)bshow, (LPARAM)0);
    }

    bool GetOverview()
    {
        ATLASSERT(::IsWindow(m_hWnd));
        return ::SendMessage(m_hWnd, SGV_GETOVERVIEW, (WPARAM)0, (LPARAM)0) != 0;
    }


    void SetRowCompress(bool bcompress)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        ::SendMessage(m_hWnd, SGV_SETROWCOMPRESS, (WPARAM)bcompress, (LPARAM)0);
    }

    bool GetRowCompress() const
    {
        ATLASSERT(::IsWindow(m_hWnd));
        return ::SendMessage(m_hWnd, SGV_GETROWCOMPRESS, (WPARAM)0, (LPARAM)0) != 0;
    }

    void SetEdgeLabelsVisible(bool visible)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        ::SendMessage(m_hWnd, SGV_SET_EDGE_LABEL_VISIBLE, (WPARAM)visible, (LPARAM)0);
    }

    bool IsEdgeLabelVisible()
    {
        ATLASSERT(::IsWindow(m_hWnd));
        return ::SendMessage(m_hWnd, SGV_GET_EDGE_LABEL_VISIBLE, (WPARAM)0, (LPARAM)0) != 0;
    }

    void SetOverviewOpacity(BYTE opacity)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        ::SendMessage(m_hWnd, SGV_SETOVERVIEWOPACITY, (WPARAM)opacity, (LPARAM)0);
    }

    BYTE GetOverviewOpacity() const
    {
        ATLASSERT(::IsWindow(m_hWnd));
        return ::SendMessage(m_hWnd, SGV_GETOVERVIEWOPACITY, 0, 0);
    }

    void SetOverviewPosition(const CPoint& pos)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SGV_POINT point;
        point.x=pos.x;
        point.y=pos.y;
        ::SendMessage(m_hWnd, SGV_SETOVERVIEWPOSITION, (WPARAM)&point, 0);
    }

    void SetOverviewSize(const CSize& size) 
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SGV_SIZE dim;
        dim.width=size.cx;
        dim.height=size.cy;
        ::SendMessage(m_hWnd, SGV_SETOVERVIEWSIZE, (WPARAM)&dim, 0);
    }

    bool EmulateDecoration(bool emulate)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        return ::SendMessage(m_hWnd, SGV_EMULATEDECORATION, (WPARAM)emulate, 0) != 0;
    }

    bool IsEmulateDecoration() const
    {
        ATLASSERT(::IsWindow(m_hWnd));
        return ::SendMessage(m_hWnd, SGV_ISEMULATEDECORATION, 0, 0);
    }

    void SetReadOnly(bool ro)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        ::SendMessage(m_hWnd, SGV_SETREADONLY, (WPARAM)ro, 0);
    }

    bool IsReadOnly() const
    {
        ATLASSERT(::IsWindow(m_hWnd));
        return ::SendMessage(m_hWnd, SGV_ISREADONLY, 0, 0);
    }

    void SetLayout(LAYOUT layout)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        ::SendMessage(m_hWnd, SGV_SETLAYOUT, (WPARAM)layout, (LPARAM)0);
    }

    LAYOUT GetLayout()
    {
        ATLASSERT(::IsWindow(m_hWnd));
        return (LAYOUT)::SendMessage(m_hWnd, SGV_GETLAYOUT, (WPARAM)0, (LPARAM)0);
    }

    void SetOwner(IGraphViewSlot * owner)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        ::SendMessage(m_hWnd, SGV_SETOWNER, (WPARAM)0, (LPARAM)owner);
    }

    void SetProperty(const CUniqueID& uid, int key, const CComVariant& val)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SGV_UNIQUEIDSTRUCT id_struct;
        SetUIDData(id_struct, uid.GetCategory(), uid.GetID());
        SGV_KEYVAL keyval_struct;
        keyval_struct.key = key;
        keyval_struct.val = (CComVariant)val;
        ::SendMessage(m_hWnd, SGV_SETPROPERTY, (WPARAM)&id_struct, (LPARAM)&keyval_struct);
    }

    IUnknown * GetPropertyUnknown(const CUniqueID& uid, int key) const
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SGV_UNIQUEIDSTRUCT id_struct;
        SetUIDData(id_struct, uid.GetCategory(), uid.GetID());
        return (IUnknown *)::SendMessage(m_hWnd, SGV_GETPROPERTYUNKNOWN, (WPARAM)&id_struct, (LPARAM)key);
    }

    CComVariant GetProperty(const CUniqueID& uid, int key) const
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SGV_UNIQUEIDSTRUCT id_struct;
        id_struct.category.plugin = uid.GetCategory().GetPlugin().GetGUID();
        id_struct.category.category = uid.GetCategory().GetCategory();
        id_struct.id = uid.GetID();
        SGV_KEYVAL keyval_struct;
        keyval_struct.key = key;
        ::SendMessage(m_hWnd, SGV_GETPROPERTY, (WPARAM)&id_struct, (LPARAM)&keyval_struct);
        return keyval_struct.val;
    }

    void GetProperties(IPropertyIDContainer* results, const CUniqueID& uid)
    {
        SGV_UNIQUEIDSTRUCT id_struct;
        id_struct.category.plugin = uid.GetCategory().GetPlugin().GetGUID();
        id_struct.category.category = uid.GetCategory().GetCategory();
        id_struct.id = uid.GetID();
        ::SendMessage(m_hWnd, SGV_GETPROPERTIES, (WPARAM)results, (LPARAM)&id_struct);
    }

    void SetMouseMode(MOUSE_MODES mm)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        ::SendMessage(m_hWnd, SGV_SETMOUSEMODE, (WPARAM)mm, (LPARAM)0);
    }

    MOUSE_MODES GetMouseMode()
    {
        ATLASSERT(::IsWindow(m_hWnd));
        return (MOUSE_MODES)::SendMessage(m_hWnd, SGV_GETMOUSEMODE, (WPARAM)0, (LPARAM)0);
    }

    bool AddVertexNoPos(const CUniqueID & uid, const TCHAR* label = _T(""), IGraphSubgraph * Subgraph = NULL, RendererTypes rtype = rtLegacyVertex)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SetRedraw(false);

        SGV_VERTEXSTRUCT v_struct;
        SetUIDData(v_struct.id, uid.GetCategory(), uid.GetID());
        v_struct.Subgraph=Subgraph;
        v_struct.rType=rtype;
        bool retVal = false;
        if (::SendMessage(m_hWnd, SGV_ADDVERTEX, (WPARAM)&v_struct, (LPARAM)0))
        {
            retVal = true;
        }
        SetLabel(uid, label);
        SetRedraw(true);
        return retVal;
    }

    bool AddVertex(const CUniqueID & uid, const TCHAR* label = _T(""), bool randomize = false, int x = 0, int y = 0, IGraphSubgraph * Subgraph = NULL, RendererTypes rtype = rtLegacyVertex, GraphTypes::Coordinates co = GraphTypes::screen, int pos = -1)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SetRedraw(false);

        SGV_VERTEXSTRUCT v_struct;
        SetUIDData(v_struct.id, uid.GetCategory(), uid.GetID());
        v_struct.Subgraph=Subgraph;
        v_struct.rType=rtype;
        bool retVal = false;
        if (::SendMessage(m_hWnd, SGV_ADDVERTEX, (WPARAM)&v_struct, (LPARAM)0))
        {
            SetPosition(uid, x, y, randomize, co);
            retVal = true;
        }
        SetLabel(uid, label);
        if (pos != -1)
        {
            SetZOrder(uid, pos);
        }
        SetRedraw(true);
        return retVal;
    }

    bool AddVertex(const CUniqueID & uid, const TCHAR* label, IGraphSubgraph * Subgraph, RendererTypes rtype, int pos = -1)
    {
        return AddVertex(uid, label, true, 0, 0, Subgraph, rtype, GraphTypes::screen, pos);
    }

    bool AddEdge(const CUniqueID& edge_id, const CUniqueID& fromID, const CUniqueID& toID, const TCHAR* label = _T(""), RendererTypes rtype = rtLegacyEdge, bool allowDuplicate=true)
    {
        if (!allowDuplicate && GetEdge(edge_id))
        {
            return true;
        }
        SetRedraw(false);
        ATLASSERT(::IsWindow(m_hWnd));
        SGV_EDGESTRUCT edge_struct;
        
        SetUIDData(edge_struct.id, edge_id.GetCategory(), edge_id.GetID());
        SetUIDData(edge_struct.idFrom, fromID.GetCategory(), fromID.GetID());
        SetUIDData(edge_struct.idTo, toID.GetCategory(), toID.GetID());

        IGraphEdge* e = (IGraphEdge*)::SendMessage(m_hWnd, SGV_ADDEDGE, (WPARAM)&edge_struct, (LPARAM)0);
        ATLASSERT(e);
        if (e != 0)
        {
            SetLabel(edge_id, label);
            SetRedraw(true);
            return true;
        }
        return false;
    }

    // Depracated
    CUniqueID AddEdge(const CCategory& category, const CUniqueID& fromID, const CUniqueID& toID, const TCHAR* label = _T(""), RendererTypes rtype = rtLegacyEdge, bool allowDuplicate=true)
    {
        CUniqueID edge_id = SGV::CreateEdgeID(category, fromID, toID);
        AddEdge(edge_id, fromID, toID, label, rtype, allowDuplicate);
        return CUniqueID(edge_id);
    }

    bool HasEdge(const CUniqueID & uid)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SGV_UNIQUEIDSTRUCT id_struct;
        SetUIDData(id_struct, uid.GetCategory(), uid.GetID());
        if(::SendMessage(m_hWnd,SGV_GETEDGE,(WPARAM)&id_struct,(LPARAM)0)!=0)
            return true;
        return false;
    }

    IGraphEdge * GetEdge(const CUniqueID & uid)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SGV_UNIQUEIDSTRUCT id_struct;
        SetUIDData(id_struct, uid.GetCategory(), uid.GetID());
        return (IGraphEdge *)::SendMessage(m_hWnd,SGV_GETEDGE,(WPARAM)&id_struct,(LPARAM)0);
    }

    void AddRandom(unsigned vertices, unsigned edges)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        ::SendMessage(m_hWnd, SGV_ADDRANDOM, (WPARAM)0, MAKELPARAM(vertices,edges));
    }

    void AddRandomTimes()
    {
        ATLASSERT(::IsWindow(m_hWnd));
        ::SendMessage(m_hWnd, SGV_ADDRANDOMTIMES, (WPARAM)0, (LPARAM)0);
    }

    IGraphSubgraph * AddSubgraph(const CUniqueID & uid, const TCHAR* label= _T(""), RendererTypes rtype=rtDatasetSubgraph)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SGV_SUBGRAPHSTRUCT sg_struct;
        SetUIDData(sg_struct.id, uid.GetCategory(), uid.GetID());
        sg_struct.label=label;
        sg_struct.rType=rtype;
        IGraphSubgraph* Subgraph = (IGraphSubgraph*)::SendMessage(m_hWnd, SGV_ADDSUBGRAPH, (WPARAM)&sg_struct, (LPARAM)0);
        ATLASSERT(Subgraph);
        return Subgraph;
    }

    IGraphSubgraph * AddSubgraph(const CUniqueID & uid, const TCHAR* label= _T(""), int x = 0, int y = 0, RendererTypes rtype=rtDatasetSubgraph)
    {
        IGraphSubgraph* Subgraph = AddSubgraph(uid,label,rtype);
        ATLASSERT(Subgraph);
        if (Subgraph)
        {
            SetPosition(uid, x, y, false, GraphTypes::screen);	// gtp::this should be a message
        }
        return Subgraph;
    }

    void LoadXGMML(const TCHAR* xml, WUVisualState state, bool redundantSubgraphs, bool passthroughVertices, bool minimizeIfLarge, const IXGMMLRenderCallback * callback)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SGV_XGMMLSTRUCT xgmml_struct;
        xgmml_struct.xgmml = xml;
        xgmml_struct.state = state;
        xgmml_struct.redundantSubgraphs = redundantSubgraphs;
        xgmml_struct.passthroughVertices = passthroughVertices;
        xgmml_struct.minimizeIfLarge = minimizeIfLarge;
        xgmml_struct.callback = callback;
        ::SendMessage(m_hWnd, SGV_LOADXGMML, (WPARAM)&xgmml_struct, NULL);
    }
    
    void MergeXGMML(const TCHAR* xml, WUVisualState state, bool appendMissing, bool redundantSubgraphs, bool passthroughVertices, const IXGMMLRenderCallback * callback)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SGV_XGMMLSTRUCT xgmml_struct;
        xgmml_struct.xgmml = xml;
        xgmml_struct.state = state;
        xgmml_struct.appendMissing = appendMissing;
        xgmml_struct.redundantSubgraphs = redundantSubgraphs;
        xgmml_struct.passthroughVertices = passthroughVertices;
        xgmml_struct.callback = callback;
        ::SendMessage(m_hWnd, SGV_MERGEXGMML, (WPARAM)&xgmml_struct, NULL);
    }

    bool HasItems()
    {
        bool retVal;
        ::SendMessage(m_hWnd, SGV_HASITEMS, (WPARAM)&retVal, NULL);
        return retVal;
    }

    void LoadXml(const TCHAR* xml, const CPoint& relativeLoadPos = CPoint(0,0), int version = 1)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SGV_LOADXMLSTRUCT loadxml_struct;
        loadxml_struct.xml = xml;
        loadxml_struct.version = version;
        ::SendMessage(m_hWnd, SGV_LOADXML, (WPARAM)&loadxml_struct, (LPARAM)&relativeLoadPos);
    }

    void LoadXmlFile()
    {
        ATLASSERT(::IsWindow(m_hWnd));
        ::SendMessage(m_hWnd, SGV_LOADXMLFILE, (WPARAM)0, (LPARAM)0);
    }

    IGraphSubgraph* GetSubgraph(const CUniqueID& uid)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SGV_UNIQUEIDSTRUCT id_struct;
        SetUIDData(id_struct, uid.GetCategory(), uid.GetID());
        return (IGraphSubgraph*)::SendMessage(m_hWnd, SGV_GETSUBGRAPH, (WPARAM)&id_struct, (LPARAM)0);
    }

    IGraphVertex* GetVertex(const CUniqueID& uid)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SGV_UNIQUEIDSTRUCT id_struct;
        SetUIDData(id_struct, uid.GetCategory(), uid.GetID());
        return (IGraphVertex*)::SendMessage(m_hWnd, SGV_GETVERTEX, (WPARAM)&id_struct, (LPARAM)0);
    }

    IGraphItem* GetGraphItem(const CUniqueID& uid)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SGV_UNIQUEIDSTRUCT id_struct;
        SetUIDData(id_struct, uid.GetCategory(), uid.GetID());
        return (IGraphItem*)::SendMessage(m_hWnd, SGV_GETGRAPHITEM, (WPARAM)&id_struct, (LPARAM)0);
    }

    void RemoveVertex(const CUniqueID& uid)
    {
        ATLASSERT(::IsWindow(m_hWnd));

        SGV_UNIQUEIDSTRUCT id_struct;
        SetUIDData(id_struct, uid.GetCategory(), uid.GetID());
        ::SendMessage(m_hWnd, SGV_REMOVEVERTEX, (WPARAM)&id_struct, 0);
    }

    void RemoveEdge(const CUniqueID& uid)
    {
        ATLASSERT(::IsWindow(m_hWnd));

        SGV_UNIQUEIDSTRUCT id_struct;
        SetUIDData(id_struct, uid.GetCategory(), uid.GetID());
        ::SendMessage(m_hWnd, SGV_REMOVEEDGE, (WPARAM)&id_struct, (LPARAM)0);
    }

    void RemoveSubgraph(const CUniqueID& uid)
    {
        ATLASSERT(::IsWindow(m_hWnd));

        SGV_UNIQUEIDSTRUCT id_struct;
        SetUIDData(id_struct, uid.GetCategory(), uid.GetID());
        ::SendMessage(m_hWnd, SGV_REMOVESUBGRAPH, (WPARAM)&id_struct, 0);
    }

    unsigned GetVertexCount(TRISTATE visible=TRISTATE_BOTH, TRISTATE pinned=TRISTATE_BOTH, TRISTATE selected=TRISTATE_BOTH) const
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SGV_GRAPHITEMSTATESTRUCT data;
        data.visible = visible;
        data.pinned = pinned;
        data.selected = selected;
        return (unsigned)::SendMessage(m_hWnd, SGV_GETVERTEXCOUNT, (WPARAM)&data, (LPARAM)0);
    }

    void SetTargetByCount(IUniqueIDContainer * results, PSGV_TARGETSTRUCT target)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        ::SendMessage(m_hWnd, SGV_SETTARGETBYCOUNT, (WPARAM) results, (LPARAM) target);
    }

    //! Returns the vertices that were affected, ie. made visible / invisible.
    void SetVerticesVisibleByEdgeCount(IUniqueIDContainer * results, int edgeCount, bool visible)
    {
        SGV_TARGETSTRUCT data;
        data.category.plugin = guidDefault;
        data.category.category = 0;
        data.style = SGV_TARGETSTRUCT::SGV_TARGET_STYLE_EDGE;
        data.state = visible?SGV_TARGETSTRUCT::SGV_TARGET_STATE_VISIBLE:SGV_TARGETSTRUCT::SGV_TARGET_STATE_HIDDEN;
        data.count = edgeCount;
        SetTargetByCount(results, &data);
    }

    void SetVertexPinned(const CUniqueID& uid, bool pinned)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        CGraphVertex v(m_hWnd, uid);
        v.SetPinned(pinned);
    }

    bool GetVertexPinned(const CUniqueID& uid)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        CGraphVertex v(m_hWnd, uid);
        return v.GetPinned();
    }

    void SetSubgraphPinned(const CUniqueID& uid, bool pinned)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        CGraphSubgraph sg(m_hWnd, uid);
        sg.SetPinned(pinned);
    }

    bool GetSubgraphPinned(const CUniqueID& uid)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        CGraphSubgraph sg(m_hWnd, uid);
        return sg.GetPinned();
    }

    void SetMetaVertexHide(const CCategory & category, bool hide)
    {
        SetMetaData(category,META_VERTEX,HIDE,hide);
    }

    bool GetMetaVertexHide(const CCategory & category)
    {	
        ATLASSERT(::IsWindow(m_hWnd));
        CComVariant variant;
        GetMetaData(category,META_VERTEX,HIDE,variant);
        return (variant.boolVal != ATL_VARIANT_FALSE);
    }

    void SetMetaSubgraphHide(const CCategory & category, bool hide)
    {
        SetMetaData(category,META_SUBGRAPH,HIDE,hide);
    }

    bool GetMetaSubgraphHide(const CCategory & category)
    {	
        ATLASSERT(::IsWindow(m_hWnd));
        CComVariant variant;
        GetMetaData(category,META_SUBGRAPH,HIDE,variant);
        return (variant.boolVal != ATL_VARIANT_FALSE);
    }

    void SetVertexImagesVisible(bool visible)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        ::SendMessage(m_hWnd, SGV_SETVERTEXIMAGESVISIBLE, (WPARAM)visible, (LPARAM)0);
    }

    bool GetVertexImagesVisible()
    {
        ATLASSERT(::IsWindow(m_hWnd));
        return FALSE != ::SendMessage(m_hWnd, SGV_GETVERTEXIMAGESVISIBLE, (WPARAM)1, (LPARAM)0);
    }
    
    void SetMetaPluginName(const GUID& pluginId, const TCHAR* name)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        CCategory cat(pluginId, 0);
        SetMetaData(cat, META_CATEGORY, PLUGIN_NAME, CComVariant(name));
    }

    const TCHAR* GetMetaPluginName(const GUID& pluginId, std::_tstring & name)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        CCategory cat(pluginId, 0);
        CComVariant variant;
        GetMetaData(cat, META_CATEGORY, PLUGIN_NAME, variant);
        if (variant.vt == VT_BSTR)
            name = bstr_t(variant);
        return name.c_str();
    }

    void SetMetaEdgeHide(const CCategory & category, bool hide)
    {
        SetMetaData(category,META_EDGE,HIDE,hide);
    }

    bool GetMetaEdgeHide(const CCategory & category)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        CComVariant variant;
        GetMetaData(category,META_EDGE,HIDE,variant);
        return (variant.boolVal != ATL_VARIANT_FALSE);
    }

    //void SetEdgeImagesVisible(bool visible, IMAGE_POSITION pos)
    //{
    //	ATLASSERT(::IsWindow(m_hWnd));
    //	// TODO: make use of position
    //	::SendMessage(m_hWnd, SGV_SETVERTEXIMAGESVISIBLE, (WPARAM)visible, (LPARAM)0);
    //}

    //bool GetEdgeImagesVisible(IMAGE_POSITION pos)
    //{
    //	ATLASSERT(::IsWindow(m_hWnd));
    //	// TODO: make use of position
    //	return FALSE != ::SendMessage(m_hWnd, SGV_GETVERTEXIMAGESVISIBLE, (WPARAM)1, (LPARAM)0);
    //}

    unsigned GetVertexNeighbourCount(const CUniqueID& uid, const CCategory& category, tagSGV_TARGETSTRUCT::SGV_TARGET_STYLE style, tagSGV_TARGETSTRUCT::SGV_TARGET_STATE state)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        // TODO: I assume this will want to be pushed lower down?
        IGraphVertex* vertex = GetVertex(uid);
        ATLASSERT(vertex);
        SGV_TARGETSTRUCT data;
        data.category.plugin = category.GetPlugin().GetGUID();
        data.category.category = category.GetCategory();
        data.style = style;
        data.state = state;
        return (unsigned)::SendMessage(m_hWnd, SGV_GETVERTEXNEIGHBOURCOUNT, (WPARAM)&data, (LPARAM)vertex);
    }

    void SetVertexNeighbour(const CUniqueID& uid, const CCategory & category, tagSGV_TARGETSTRUCT::SGV_TARGET_STYLE style, tagSGV_TARGETSTRUCT::SGV_TARGET_STATE state)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        // TODO: I assume this will want to be pushed lower down?
        IGraphVertex* vertex = GetVertex(uid);
        ATLASSERT(vertex);
        SGV_TARGETSTRUCT data;
        data.category.plugin = category.GetPlugin().GetGUID();
        data.category.category = category.GetCategory();
        data.style = style;
        data.state = state;
        ::SendMessage(m_hWnd, SGV_SETVERTEXNEIGHBOUR, (WPARAM)&data, (LPARAM)vertex);
    }

    void SetVertexNeighboursVisibleByEdgeCat(const CUniqueID& uid, const CCategory & category, bool visible)
    {
        SGV_TARGETSTRUCT::SGV_TARGET_STYLE style;
        SGV_TARGETSTRUCT::SGV_TARGET_STATE state;
        style = SGV_TARGETSTRUCT::SGV_TARGET_STYLE_EDGE;
        if (visible)
            state = SGV_TARGETSTRUCT::SGV_TARGET_STATE_VISIBLE;
        else
            state = SGV_TARGETSTRUCT::SGV_TARGET_STATE_HIDDEN;
        SetVertexNeighbour(uid,category,style,state);
    }

    void SetVertexNeighboursVisibleByVertexCat(const CUniqueID& uid, const CCategory & category, bool visible)
    {
        SGV_TARGETSTRUCT::SGV_TARGET_STYLE style;
        SGV_TARGETSTRUCT::SGV_TARGET_STATE state;
        style = SGV_TARGETSTRUCT::SGV_TARGET_STYLE_VERTEX;
        if (visible)
            state = SGV_TARGETSTRUCT::SGV_TARGET_STATE_VISIBLE;
        else
            state = SGV_TARGETSTRUCT::SGV_TARGET_STATE_HIDDEN;
        SetVertexNeighbour(uid,category,style,state);
    }

    unsigned GetEdgeCount()
    {
        ATLASSERT(::IsWindow(m_hWnd));
        return (unsigned)::SendMessage(m_hWnd, SGV_GETEDGECOUNT, (WPARAM)0, (LPARAM)0);
    }

    unsigned long GetMetaVertexColor(const CCategory & category, META_CAT metaCategory)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        CComVariant variant;
        GetMetaData(category, META_VERTEX, metaCategory, variant);
        return variant;
    }

    unsigned long GetMetaEdgeColor(const CCategory & category, META_CAT metaCategory)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        CComVariant variant;
        GetMetaData(category, META_EDGE, metaCategory, variant);
        return variant.ulVal;
    }

    void GetEdgeColor(const CUniqueID& uid, GraphTypes::Color& color) const
    {
        ATLASSERT(::IsWindow(m_hWnd));

        SGV_UNIQUEIDSTRUCT id_struct;
        SGV_COLOR sgvcolor;

        SetUIDData(id_struct, uid.GetCategory(), uid.GetID());

        ::SendMessage(m_hWnd, SGV_GETEDGECOLOR, (WPARAM)&id_struct, (LPARAM)&sgvcolor);

        GraphTypes::Color c(sgvcolor.A, sgvcolor.R, sgvcolor.G, sgvcolor.B);
        color=c;
    }

    void SetEdgeColor(const CUniqueID& uid, const GraphTypes::Color& color)
    {
        ATLASSERT(::IsWindow(m_hWnd));

        SGV_UNIQUEIDSTRUCT id_struct;
        SGV_COLOR sgvcolor;

        SetUIDData(id_struct, uid.GetCategory(), uid.GetID());
        sgvcolor.R=color.GetR();
        sgvcolor.G=color.GetG();
        sgvcolor.B=color.GetB();
        sgvcolor.A=color.GetA();

        ::SendMessage(m_hWnd, SGV_SETEDGECOLOR, (WPARAM)&id_struct, (LPARAM)&sgvcolor);
    }

    void SetEdgeDateRange(const CUniqueID& uid, std::_tstring & from, std::_tstring & to)  //UST
    {
        ATLASSERT(::IsWindow(m_hWnd));

        SGV_UNIQUEIDSTRUCT id_struct;
        SGV_DATERANGE dateRange;

        dateRange.from = from.c_str();
        dateRange.to = to.c_str();
        ::SendMessage(m_hWnd, SGV_SETEDGEDATERANGE, (WPARAM)&id_struct, (LPARAM)&dateRange);
    }

    void SetAppMode(AppMode mode)
    {
        ATLASSERT(::IsWindow(m_hWnd));

        ::SendMessage(m_hWnd, SGV_SETAPPMODE, (WPARAM)mode, (LPARAM)0);
    }

    AppMode GetAppMode() const
    {
        ATLASSERT(::IsWindow(m_hWnd));

        return (AppMode)::SendMessage(m_hWnd, SGV_GETAPPMODE, (WPARAM)0, (LPARAM)0);
    }

    void SetAppVisMode(AppVisMode mode)
    {
        ATLASSERT(::IsWindow(m_hWnd));

        ::SendMessage(m_hWnd, SGV_SETAPPVISMODE, (WPARAM)mode, (LPARAM)0);
    }

    AppVisMode GetAppVisMode() const
    {
        ATLASSERT(::IsWindow(m_hWnd));

        return (AppVisMode)::SendMessage(m_hWnd, SGV_GETAPPVISMODE, (WPARAM)0, (LPARAM)0);
    }

    void SetAppZoomMode(AppZoomMode mode)
    {
        ATLASSERT(::IsWindow(m_hWnd));

        ::SendMessage(m_hWnd, SGV_SETAPPZOOMMODE, (WPARAM)mode, (LPARAM)0);
    }

    AppZoomMode GetAppZoomMode() const
    {
        ATLASSERT(::IsWindow(m_hWnd));

        return (AppZoomMode)::SendMessage(m_hWnd, SGV_GETAPPZOOMMODE, (WPARAM)0, (LPARAM)0);
    }

    void FindItemByID()
    {
        ATLASSERT(::IsWindow(m_hWnd));

        ::SendMessage(m_hWnd, SGV_FINDIDDIALOG, (WPARAM)0, (LPARAM)0);
    }

    const CUniqueID& MergeIntoSubgraph(CUniqueID& retVal, IUniqueIDContainer* nodes, const TCHAR* label)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SGV_MERGEINTOSUBGRAPHSTRUCT s;
        s.label = label;
        s.nodes = nodes;
        ::SendMessage(m_hWnd, SGV_MERGEINTOSUBGRAPH, (WPARAM)&s, (LPARAM)&retVal);
        return retVal;
    }

    void BreakSubgraph(IUniqueIDContainer* results, const CUniqueID& subgraph)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        // TODO: Look at impl of message
        ::SendMessage(m_hWnd, SGV_BREAKSUBGRAPH, (WPARAM)results, (LPARAM)&subgraph);
    }

    bool EnsureVisible(const CUniqueID & id)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SGV_UNIQUEIDSTRUCT id_struct;
        SetUIDData(id_struct, id.GetCategory(), id.GetID());
        return ::SendMessage(m_hWnd, SGV_ENSUREVISIBLE, (WPARAM)&id_struct, (LPARAM)0) != 0;
    }
    
    void SetMetaDebug(bool mode)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        ::SendMessage(m_hWnd, SGV_SETMETADEBUG, (WPARAM)0, (LPARAM)mode);
    }

    VARIANT GetMetaData(const CCategory & category, PSGV_METADATASTRUCT meta, CComVariant & variant)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        ATLASSERT(meta);
        SGV_CATEGORYSTRUCT data;
        data.plugin = category.GetPlugin().GetGUID();
        data.category = category.GetCategory();
        ::SendMessage(m_hWnd, SGV_GETMETADATA, (WPARAM)&data, (LPARAM)meta);
        variant = meta->valueX;
        return variant;
    }

    VARIANT GetMetaData(const CCategory & category, META_TYPE metaType, META_CAT metaCategory, CComVariant & variant)
    {
        SGV_METADATASTRUCT data;
        data.type = metaType;
        data.category = metaCategory;
        return GetMetaData(category, &data, variant);
    }

    CCategory GetMetaCategory(const TCHAR* lookup, META_TYPE metaType, META_CAT metaCategory)
    {
        SGV_CATEGORYSTRUCT data;
        SGV_METADATASTRUCT meta;
        meta.type = metaType;
        meta.category = metaCategory;
        meta.valueX = lookup;
        ::SendMessage(m_hWnd, SGV_GETMETADATA, (WPARAM)&data, (LPARAM)&meta);
        return CCategory(data.plugin,data.category);
    }

    void SetMetaData(const CCategory & category, PSGV_METADATASTRUCT meta, const CComVariant & variant)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        ATLASSERT(meta);
        SGV_CATEGORYSTRUCT data;
        data.plugin = category.GetPlugin().GetGUID();
        data.category = category.GetCategory();
        meta->valueX = variant;
        ::SendMessage(m_hWnd, SGV_SETMETADATA, (WPARAM)&data, (LPARAM)meta);
    }

    void SetMetaData(const CCategory & category, META_TYPE metaType, META_CAT metaCategory, const CComVariant & variant)
    {
        SGV_METADATASTRUCT data;
        data.type = metaType;
        data.category = metaCategory;
        SetMetaData(category, &data, variant);
    }
    
    CCategory GetMetaVertexXmlCat(const TCHAR* lookup)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        return GetMetaCategory(lookup,META_VERTEX,XML_CATEGORY_LOOKUP);
    }

    void GetMetaVertexXmlCatCode(const CCategory & category, std::_tstring & str)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        CComVariant variant;
        GetMetaData(category, META_VERTEX, XML_CATEGORY_CODE, variant);
        str = bstr_t(variant);
    }

    CCategory GetMetaEdgeXmlCat(const TCHAR* lookup)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        return GetMetaCategory(lookup,META_EDGE,XML_CATEGORY_LOOKUP);
    }

    void GetMetaEdgeXmlCatCode(const CCategory & category, std::_tstring & str)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        CComVariant variant;
        GetMetaData(category, META_EDGE, XML_CATEGORY_CODE, variant);
        str = bstr_t(variant);
    }

    void GetMetaVertexXmlAnbIconFile(const CCategory & category, std::_tstring & str)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        CComVariant variant;
        GetMetaData(category, META_VERTEX, XML_ANBICONFILE, variant);
        str = bstr_t(variant);
    }

    void GetMetaVertexLabel(const CCategory & category, std::_tstring & str)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        CComVariant variant;
        GetMetaData(category, META_VERTEX, LABEL, variant);
        str = bstr_t(variant);
    }

    void GetMetaEdgeLabel(const CCategory & category, std::_tstring & str)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        CComVariant variant;
        GetMetaData(category, META_EDGE, LABEL, variant);
        str = bstr_t(variant);
    }

    bool GetMetaSplines()
    {
        ATLASSERT(::IsWindow(m_hWnd));
        CComVariant variant;
        GetMetaData(CCategory(), META_CATEGORY, SPLINES, variant);
        return (variant.boolVal != ATL_VARIANT_FALSE);
    }

    void SetMetaSplines(bool splines)
    {
        SetMetaData(CCategory(), META_CATEGORY, SPLINES, splines);
    }

    void SetAvoidObstacles(bool avoid)
    {
        SetMetaData(CCategory(), META_CATEGORY, AVOID_OBSTACLES, avoid);
    }

    bool GetMetaDotLeftRight()
    {
        ATLASSERT(::IsWindow(m_hWnd));
        CComVariant variant;
        GetMetaData(CCategory(), META_CATEGORY, DOT_LEFTRIGHT, variant);
        return (variant.boolVal != ATL_VARIANT_FALSE);
    }

    void SetMetaDotLeftRight(bool leftRight)
    {
        SetMetaData(CCategory(), META_CATEGORY, DOT_LEFTRIGHT, leftRight);
    }

    unsigned long GetMetaEdgeColorNorm(const CCategory & category)
    {
        CComVariant variant;
        GetMetaData(category,META_EDGE,EDGE_COLOR, variant);
        return variant.ulVal;
    }

    void SetMetaEdgeColorNorm(const CCategory & category, unsigned long color)
    {
        SetMetaData(category,META_EDGE,EDGE_COLOR,color);
    }

    unsigned int GetPropertyAttributeId(const CCategory& category, META_TYPE entityType, const PROPERTY_ATTR& attr, const CComVariant& value, bool autoGenId=false)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SGV_PROPERTYSTRUCT data;
        data.attr = attr;
        data.autoGenId = autoGenId;
        data.value = value;
        data.entity = entityType;
        data.cat.plugin = category.GetPlugin().GetGUID();
        data.cat.category = category.GetCategory();
        ::SendMessage(m_hWnd, SGV_GETPROPERTYATTRIBUTEID, (WPARAM)&data, (LPARAM)0);
        return data.propid;
    }

    void SetMetaEdgeColorShortest(const CCategory & category, unsigned long color)
    {
        SetMetaData(category,META_EDGE,EDGE_SHORTEST_PATH_COLOR,color);
    }

    void SetMetaEdgeColors(const CCategory & category, unsigned long color, unsigned long shortest)
    {
        SetMetaEdgeColorNorm(category,color);
        SetMetaEdgeColorShortest(category,shortest);
    }

    void SetMetaVertexColorFillNorm(const CCategory & category, unsigned long color)
    {
        SetMetaData(category,META_VERTEX,FILL_COLOR,color);
    }

    void SetMetaVertexColorBorderNorm(const CCategory & category, unsigned long color)
    {
        SetMetaData(category,META_VERTEX,BORDER_COLOR,color);
    }

    void SetMetaVertexColorNorm(const CCategory & category, unsigned long fill, unsigned long border)
    {
        SetMetaVertexColorFillNorm(category,fill);
        SetMetaVertexColorBorderNorm(category,border);
    }
  
    void SetMetaVertexColorFillSel(const CCategory & category, unsigned long color)
    {
        SetMetaData(category,META_VERTEX,SEL_FILL_COLOR,color);
    }

    void SetMetaVertexColorBorderSel(const CCategory & category, unsigned long color)
    {
        SetMetaData(category,META_VERTEX,SEL_BORDER_COLOR,color);
    }

    void SetMetaVertexColorSel(const CCategory & category, unsigned long fill, unsigned long border)
    {
        SetMetaVertexColorFillSel(category,fill);
        SetMetaVertexColorBorderSel(category,border);
    }

    void SetMetaVertexColorFillOver(const CCategory & category, unsigned long color)
    {
        SetMetaData(category,META_VERTEX,OVER_FILL_COLOR,color);
    }

    void SetMetaVertexColorBorderOver(const CCategory & category, unsigned long color)
    {
        SetMetaData(category,META_VERTEX,OVER_BORDER_COLOR,color);
    }

    void SetMetaVertexColorOver(const CCategory & category, unsigned long fill, unsigned long border)
    {
        SetMetaVertexColorFillOver(category,fill);
        SetMetaVertexColorBorderOver(category,border);
    }

    void SetMetaVertexColor(const CCategory & category, unsigned long fill, unsigned long border, unsigned long selfill, unsigned long selborder, unsigned long overfill, unsigned long overborder)
    {
        SetMetaVertexColorNorm(category,fill,border);
        SetMetaVertexColorSel(category,selfill,selborder);
        SetMetaVertexColorOver(category,overfill,overborder);
    }

    void SetMetaVertexFontColor(const CCategory & category, unsigned long color)
    {
        SetMetaData(category,META_VERTEX,FONT_COLOR,color);
    }

    void SetMetaVertexFontColorSel(const CCategory & category, unsigned long color)
    {
        SetMetaData(category,META_VERTEX,SEL_FONT_COLOR,color);
    }

    void SetMetaVertexLabel(const CCategory & category, const TCHAR* label)
    {
        SetMetaData(category, META_VERTEX, LABEL, label);
    }

    void SetMetaEdgeLabel(const CCategory & category, const TCHAR* label)
    {
        SetMetaData(category, META_EDGE, LABEL, label);
    }

    void SetMetaEdgeWidth(const CCategory & category, unsigned short width)
    {
        SetMetaData(category, META_EDGE, WIDTH, width);
    }

    void SetMetaVertexXmlData(const CCategory & category, const TCHAR* name, const TCHAR* anbIconFile)
    {
        SetMetaData(category,META_VERTEX,XML_CATEGORY_CODE, name);
        SetMetaData(category,META_VERTEX,XML_ANBICONFILE, anbIconFile);
    }

    const TCHAR* GetMetaEdgeCatCode(const CCategory & category)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        return GetMetaBSTR(category,META_EDGE,XML_CATEGORY_CODE);
    }

    void SetMetaEdgeXmlData(const CCategory & category, const TCHAR* catCode)
    {
        SetMetaData(category,META_EDGE,XML_CATEGORY_CODE, catCode);
    }

    void GetKnownCategories(META_TYPE entityType, ICategoryContainer * results) const
    {
        ATLASSERT(::IsWindow(m_hWnd));
        ::SendMessage(m_hWnd, SGV_GETCATEGORIES, (WPARAM)results, (LPARAM)entityType);
    }

    void GetSubgraphs(IUniqueIDContainer * results, TRISTATE visible=TRISTATE_BOTH, TRISTATE pinned=TRISTATE_BOTH, TRISTATE selected=TRISTATE_BOTH, bool includeHidden = false) const
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SGV_GRAPHITEMSTATESTRUCT data;
        data.visible = visible;
        data.pinned = pinned;
        data.selected = selected;
        data.includeHidden = includeHidden;
        ::SendMessage(m_hWnd, SGV_GETSUBGRAPHS, (WPARAM)results, (LPARAM)&data);
    }

    unsigned GetChildren(const CUniqueID& parent, IUniqueIDContainer * results, TRISTATE visible=TRISTATE_BOTH, TRISTATE pinned=TRISTATE_BOTH, TRISTATE selected=TRISTATE_BOTH, bool includeHidden = false) const
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SGV_GRAPHITEMIDANDSTATESTRUCT data;
        data.results = results;
        data.visible = visible;
        data.pinned = pinned;
        data.selected = selected;
        SGV_UNIQUEIDSTRUCT id_struct;
        SetUIDData(id_struct, parent.GetCategory(), parent.GetID());

        return (unsigned)::SendMessage(m_hWnd, SGV_GETCHILDREN, (WPARAM)&id_struct, (LPARAM)&data);
    }

    void GetVertices(IUniqueIDContainer * results, TRISTATE visible=TRISTATE_BOTH, TRISTATE pinned=TRISTATE_BOTH, TRISTATE selected=TRISTATE_BOTH, bool includeHidden = false) const
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SGV_GRAPHITEMSTATESTRUCT data;
        data.visible = visible;
        data.pinned = pinned;
        data.selected = selected;
        data.includeHidden = includeHidden;
        ::SendMessage(m_hWnd, SGV_GETVERTICES, (WPARAM)results, (LPARAM)&data);
    }

    void GetAdjacentVertices(IUniqueIDContainer * results, const CUniqueID& uid, bool in = true, bool out = true) const
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SGV_UNIQUEIDSTRUCT id_struct;
        SGV_ADJACENTVERTICESSTRUCT data;
        data.results = results;
        data.in = in;
        data.out = out;
        SetUIDData(id_struct, uid.GetCategory(), uid.GetID());
        ::SendMessage(m_hWnd, SGV_GETADJACENTVERTICES, (WPARAM)&data, (LPARAM)&id_struct);
    }

    void GetEdges(IUniqueIDContainer * results, TRISTATE visible=TRISTATE_BOTH, TRISTATE selected=TRISTATE_BOTH, bool includeHidden = false) const
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SGV_GRAPHITEMSTATESTRUCT data;
        data.visible = visible;
        data.selected = selected;
        data.includeHidden = includeHidden;
        ::SendMessage(m_hWnd, SGV_GETEDGES, (WPARAM)results, (LPARAM)&data);
    }

    void GetAdjacentEdges(IUniqueIDContainer * results, const CUniqueID& vertexId) const
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SGV_UNIQUEIDSTRUCT id_struct;
        SetUIDData(id_struct, vertexId.GetCategory(), vertexId.GetID());
        ::SendMessage(m_hWnd, SGV_GETADJACENTEDGES, (WPARAM)results, (LPARAM)&id_struct);
    }

    void GetInEdges(IUniqueIDContainer * results, const CUniqueID& vertexId) const
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SGV_UNIQUEIDSTRUCT id_struct;
        SetUIDData(id_struct, vertexId.GetCategory(), vertexId.GetID());
        ::SendMessage(m_hWnd, SGV_GETINEDGES, (WPARAM)results, (LPARAM)&id_struct);
    }

    void GetOutEdges(IUniqueIDContainer * results, const CUniqueID& vertexId) const
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SGV_UNIQUEIDSTRUCT id_struct;
        SetUIDData(id_struct, vertexId.GetCategory(), vertexId.GetID());
        ::SendMessage(m_hWnd, SGV_GETOUTEDGES, (WPARAM)results, (LPARAM)&id_struct);
    }
    void CenterGraph()
    {
        ATLASSERT(::IsWindow(m_hWnd));
        ::SendMessage(m_hWnd, SGV_CENTERGRAPH, (WPARAM)0, (LPARAM)0);
    }

    void CenterGraphItem(const CUniqueID& uid, GraphTypes::REAL scale=-1.0f)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SGV_UNIQUEIDSTRUCT id_struct;
        SetUIDData(id_struct, uid.GetCategory(), uid.GetID());
        ::SendMessage(m_hWnd, SGV_CENTERGRAPHITEM, (WPARAM)&id_struct, (LPARAM)scale);
    }

    void CenterVertex(const CUniqueID& uid, GraphTypes::REAL scale=-1.0f)
    {
        CenterGraphItem(uid, scale);
    }

    void CenterFocusNode()
    {
        ATLASSERT(::IsWindow(m_hWnd));
        ::SendMessage(m_hWnd, SGV_CENTERFOCUSNODE, (WPARAM)0, (LPARAM)0);
    }

    void KickIt()
    {
        ATLASSERT(::IsWindow(m_hWnd));
        ::SendMessage(m_hWnd, SGV_KICKIT, (WPARAM)0, (LPARAM)0);
    }

    void InvalidateLayout()
    {
        ATLASSERT(::IsWindow(m_hWnd));
        ::SendMessage(m_hWnd, SGV_INVALIDATELAYOUT, (WPARAM)0, (LPARAM)0);
    }

    void SetScale(float scale)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SGV_TRANSFORMSTRUCT transform;
        transform.boolean = true;	// not used but set
        transform.value.real = scale;
        ::SendMessage(m_hWnd, SGV_SETSCALE, (WPARAM)&transform, (LPARAM)0);
    }

    float SetScaleToFit(bool selectedOnly = false, float maxSize = 1.0f)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SGV_TRANSFORMSTRUCT transform;
        transform.boolean = selectedOnly;	// not used but set
        transform.value.real = maxSize;
        ::SendMessage(m_hWnd, SGV_SETSCALETOFIT, (WPARAM)&transform, (LPARAM)0);
        return transform.value.real;
    }

    float SetScaleToWidth(bool selectedOnly = false, float maxSize = 1.0f, bool centerVScroll = true)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SGV_TRANSFORMSTRUCT transform;
        transform.boolean = selectedOnly;	// not used but set
        transform.value.real = maxSize;
        ::SendMessage(m_hWnd, SGV_SETSCALETOWIDTH, (WPARAM)&transform, (LPARAM)centerVScroll);
        return transform.value.real;
    }

    float SetItemScaleToFit(const CUniqueID& id, float maxSize = 1.0f)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SGV_UNIQUEIDSTRUCT id_struct;
        SetUIDData(id_struct, id.GetCategory(), id.GetID());
        SGV_TRANSFORMSTRUCT transform;
        transform.value.real = maxSize;
        ::SendMessage(m_hWnd, SGV_SETITEMSCALETOFIT, (WPARAM)&id_struct, (LPARAM)&transform);
        return transform.value.real;
    }

    float GetScale()
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SGV_TRANSFORMSTRUCT transform;
        transform.boolean = true;	// not used but set
        transform.value.real = 0.0;
        ::SendMessage(m_hWnd, SGV_GETSCALE, (WPARAM)&transform, (LPARAM)0);
        return transform.value.real;
    }

    void SetSpacing(float spacing)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SGV_TRANSFORMSTRUCT transform;
        transform.boolean = true;	// not used but set
        transform.value.real = spacing;
        ::SendMessage(m_hWnd, SGV_SETSPACING, (WPARAM)&transform, (LPARAM)0);
    }

    float GetSpacing()
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SGV_TRANSFORMSTRUCT transform;
        transform.boolean = true;	// not used but set
        transform.value.real = 0.0;
        ::SendMessage(m_hWnd, SGV_GETSPACING, (WPARAM)&transform, (LPARAM)0);
        return transform.value.real;
    }

    void SetRotation(float angle)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SGV_TRANSFORMSTRUCT transform;
        transform.boolean = true;	// not used but set
        transform.value.integer = static_cast<int>(angle);
        ::SendMessage(m_hWnd, SGV_SETROTATION, (WPARAM)&transform, (LPARAM)0);
    }

    float GetRotation()
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SGV_TRANSFORMSTRUCT transform;
        transform.boolean = true;	// not used but set
        transform.value.integer = 0;
        ::SendMessage(m_hWnd, SGV_GETROTATION, (WPARAM)&transform, (LPARAM)0);
        return static_cast<float>(transform.value.integer);
    }

    void SetOffset(SGV::PSGV_POINT pt)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        ::SendMessage(m_hWnd, SGV_SETOFFSET, (WPARAM)pt, (LPARAM)0);
    }

    void GetOffset(SGV::PSGV_POINT pt) const
    {
        ATLASSERT(::IsWindow(m_hWnd));
        ::SendMessage(m_hWnd, SGV_GETOFFSET, (WPARAM)pt, (LPARAM)0);
    }

    bool Focused(const CUniqueID testFocus)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        if(GetFocus()==testFocus)
            return true;
        return false;
    }

    CUniqueID GetFocus()
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SGV_UNIQUEIDSTRUCT id_struct;
        // NB: could be empty, ie. not set
        if(::SendMessage(m_hWnd, SGV_GETFOCUS, (WPARAM)&id_struct, (LPARAM)0)!=0)
            return CUniqueID(id_struct.category.plugin, id_struct.category.category, id_struct.id);
        else
            return CUniqueID();
    }

    void SetFocus(const CUniqueID& uid)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SGV_UNIQUEIDSTRUCT id_struct;
        SetUIDData(id_struct, uid.GetCategory(), uid.GetID());
        ::SendMessage(m_hWnd, SGV_SETFOCUS, (WPARAM)&id_struct, (LPARAM)0);
    }

    bool Selected(const CUniqueID testSelected)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SGV_UNIQUEIDSTRUCT id_struct;
        SetUIDData(id_struct, testSelected.GetCategory(), testSelected.GetID());
        return ::SendMessage(m_hWnd, SGV_GETSELECTED, (WPARAM)&id_struct, (LPARAM)0) != 0;
    }

    void SetSelected(const CUniqueID & uid, bool select, bool append)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SGV_UNIQUEIDSTRUCT id_struct;
        SetUIDData(id_struct, uid.GetCategory(), uid.GetID());
        ::SendMessage(m_hWnd, SGV_SETSELECTED, (WPARAM)&id_struct, MAKELPARAM(select, append));
    }

    bool HasSelectedVertex()
    {
        ATLASSERT(::IsWindow(m_hWnd));
        return ::SendMessage(m_hWnd, SGV_HASSELECTEDVERTEX, NULL, NULL) != 0;
    }

    void RenderTo(HDC dc, int x = 0, int y = 0, int pageX = 0, int pageY = 0, int pageW = 0, int pageH = 0)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        // test for a valid dc
        ATLASSERT(GetDeviceCaps(dc, LOGPIXELSX) != 0);

        SGV_REGIONSTRUCT region;
        ZeroMemory(&region, sizeof(SGV_REGIONSTRUCT));
        region.x1 = x;
        region.y1 = y;
        region.x2 = pageX;
        region.y2 = pageY;
        region.w2 = pageW;
        region.h2 = pageH;
        ::SendMessage(m_hWnd, SGV_RENDERTOCONTEXT, (WPARAM)&region, (LPARAM)dc);
    }

    void RenderTo(CImage &image)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        ::SendMessage(m_hWnd, SGV_RENDERTOIMAGE, (WPARAM)0, (LPARAM)&image);
    }

    void RenderTo(CImage &image, int x, int y, int w, int h)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SGV_REGIONSTRUCT region;
        ZeroMemory(&region, sizeof(SGV_REGIONSTRUCT));
        region.x1 = x;
        region.y1 = y;
        region.w1 = w;
        region.h1 = h;
        ::SendMessage(m_hWnd, SGV_RENDERTOIMAGE1, (WPARAM)&region, (LPARAM)&image);
    }

    void RenderTo(CBitmap &bitmap)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        ::SendMessage(m_hWnd, SGV_RENDERTOBITMAP, (WPARAM)0, (LPARAM)&bitmap);
    }

    void RenderTo(CBitmap &bitmap, int x, int y, int w, int h)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SGV_REGIONSTRUCT region;
        ZeroMemory(&region, sizeof(SGV_REGIONSTRUCT));
        region.x1 = x;
        region.y1 = y;
        region.w1 = w;
        region.h1 = h;
        ::SendMessage(m_hWnd, SGV_RENDERTOBITMAP1, (WPARAM)&region, (LPARAM)&bitmap);
    }

    DWORD RenderToClipboard(bool emptyFirst = true)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        return (DWORD)::SendMessage(m_hWnd, SGV_RENDERTOCLIPBOARD, (WPARAM)0, (LPARAM)emptyFirst);
    }

    bool SaveAs()
    {
        ATLASSERT(::IsWindow(m_hWnd));
        return ::SendMessage(m_hWnd, SGV_SAVEAS, (WPARAM)0, (LPARAM)0) != 0;
    }

    BOOL Save(LPCTSTR lpszFileName)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        return (BOOL)::SendMessage(m_hWnd, SGV_SAVEASNAME, (WPARAM)0, (LPARAM)lpszFileName);
    }

    void SaveToXml(std::_tstring & xml, XML_SAVE_FORMAT saveFormat, const CPoint& relativeSavePos = CPoint(0,0))
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SGV_XMLSAVESTRUCT xmlSaveStruct;
        xmlSaveStruct.saveFormat = saveFormat;
        xmlSaveStruct.relativeSavePos = relativeSavePos;
        std::_tstring * pStr = &xml;
        ::SendMessage(m_hWnd, SGV_SAVETOXML, (WPARAM)pStr, (LPARAM)&xmlSaveStruct);
    }

    void SaveToXmlFile()
    {
        ATLASSERT(::IsWindow(m_hWnd));
        ::SendMessage(m_hWnd, SGV_SAVETOXMLFILE, (WPARAM)0, (LPARAM)0);
    }

    GraphTypes::RectF GetRenderExtent(HDC hDC)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SGV_REGIONSTRUCT region;
        ZeroMemory(&region, sizeof(SGV_REGIONSTRUCT));
        ::SendMessage(m_hWnd, SGV_GETRENDEREXTENT, (WPARAM)&region, (LPARAM)hDC);
        GraphTypes::RectF rect(GraphTypes::REAL(region.x1),GraphTypes::REAL(region.y1),GraphTypes::REAL(region.w1),GraphTypes::REAL(region.h1));
        return rect;
    }

    void Find(IUniqueIDContainer* results, const TCHAR* searchText, bool searchLabel = true, bool searchProperties = false, bool caseSensitive = false, bool visibleOnly = true, bool wholeWord = false, const GUID& pluginId = GUID_NULL, const int categoryId = -1, const int propertyId = -1)
    {
        ATLASSERT(::IsWindow(m_hWnd));

        SGV_FINDSTRUCT find;
        find.results = results;
        find.searchText = searchText;
        find.searchLabel = searchLabel;
        find.searchProperties = searchProperties;
        find.caseSensitive = caseSensitive;
        find.visibleOnly = visibleOnly;
        find.wholeWord = wholeWord;
        find.categoryId = categoryId;
        find.pluginId = pluginId;
        find.propertyId = propertyId;

        ::SendMessage(m_hWnd, SGV_FIND, (WPARAM)&find, 0);
    }

    bool GetDragDropNotifications()
    {
        ATLASSERT(::IsWindow(m_hWnd));
        return ::SendMessage(m_hWnd, SGV_GETDRAGDROPNOTIFICATIONS, (WPARAM)0, (LPARAM)0)!=0;
    }

    void SetDragDropNotifications(bool value)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        ::SendMessage(m_hWnd, SGV_SETDRAGDROPNOTIFICATIONS, (WPARAM)0, (LPARAM)value);
    }

    void GetMeta(const TCHAR* url)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        ::SendMessage(m_hWnd, SGV_GETMETA, (WPARAM)0, (LPARAM)url);
    }

    void GetData(const TCHAR* url, const CUniqueID& uid)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        SGV_UNIQUEIDSTRUCT id_struct;
        SetUIDData(id_struct, uid.GetCategory(), uid.GetID());
        ::SendMessage(m_hWnd, SGV_GETDATA, (WPARAM)&id_struct, (LPARAM)url);
    }

    //  GJS  ---
    void SetPosition(const CUniqueID & uid, int x, int y, bool randomize = false, GraphTypes::Coordinates co=GraphTypes::screen)
    {
        ATLASSERT(::IsWindow(m_hWnd));

        SGV_UNIQUEIDSTRUCT id_struct;
        SetUIDData(id_struct, uid.GetCategory(), uid.GetID());

        SGV_POINT point;
        point.x = x;
        point.y = y;
        point.co=co;
        point.randomize = randomize;
        ::SendMessage(m_hWnd, SGV_SETPOSITION, (WPARAM)&id_struct, (LPARAM)&point);
    }

    void GetPosition(const CUniqueID& uid, CPoint& pos)
    {
        ATLASSERT(::IsWindow(m_hWnd));

        SGV_UNIQUEIDSTRUCT id_struct;
        SetUIDData(id_struct, uid.GetCategory(), uid.GetID());

        // returns screen co-ordinates
        ::SendMessage(m_hWnd, SGV_GETPOSITION, (WPARAM)&id_struct, (LPARAM)&pos);
    }

    const TCHAR* GetLabel(const CUniqueID& uid, std::_tstring & label)
    {
        ATLASSERT(::IsWindow(m_hWnd));

        SGV_UNIQUEIDSTRUCT id_struct;
        SetUIDData(id_struct, uid.GetCategory(), uid.GetID());
        std::_tstring * pStr = &label;
        ::SendMessage(m_hWnd, SGV_GETLABEL, (WPARAM)&id_struct, (LPARAM)pStr);
        return label.c_str();
    }

    void SetLabel(const CUniqueID& uid, const TCHAR* label)
    {
        ATLASSERT(::IsWindow(m_hWnd));

        SGV_UNIQUEIDSTRUCT id_struct;
        SetUIDData(id_struct, uid.GetCategory(), uid.GetID());
        ::SendMessage(m_hWnd, SGV_SETLABEL, (WPARAM)&id_struct, (LPARAM)label);
    }

    void SetZOrder(const CUniqueID& uid, int pos)
    {
        ATLASSERT(::IsWindow(m_hWnd));

        SGV_UNIQUEIDSTRUCT id_struct;
        SetUIDData(id_struct, uid.GetCategory(), uid.GetID());

        ::SendMessage(m_hWnd, SGV_SETZORDER, (WPARAM)&id_struct, (LPARAM)pos);
    }

    void SetRedraw(bool redraw)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        ::SendMessage(m_hWnd, SGV_SETREDRAW, (WPARAM)redraw, (LPARAM)0);
    }
    //  GJS  ---

    void EditGraphItem(const CUniqueID& uid)
    {
        ATLASSERT(::IsWindow(m_hWnd));

        SGV_UNIQUEIDSTRUCT id_struct;
        SetUIDData(id_struct, uid.GetCategory(), uid.GetID());
        ::SendMessage(m_hWnd, SGV_EDITGRAPHITEM, (WPARAM)&id_struct, 0);
    }

    IPicture* CreateIPicFromPath(const TCHAR* path)
    {
        ATLASSERT(::IsWindow(m_hWnd));

        return reinterpret_cast<IPicture*>
            (::SendMessage(m_hWnd, SGV_CREATEIPIC, (WPARAM)(path.c_str()), 0));
    }

    void SetPropertyAttribute(META_TYPE entityType, const CCategory& category, const int id, PROPERTY_ATTR attr, const CComVariant& value)
    {
        ATLASSERT(::IsWindow(m_hWnd));

        SGV_PROPERTYSTRUCT props;

        props.cat.plugin=category.GetPlugin().GetGUID();
        props.cat.category=category.GetCategory();
        props.propid=id;
        props.attr=attr;
        props.entity = entityType;
        props.value=value;

        ::SendMessage(m_hWnd, SGV_SETPROPERTYATTRIBUTE, (WPARAM)&props, 0);
    }

    void SetVertexPropertyAttribute(const CCategory& category, const int id, PROPERTY_ATTR attr, const CComVariant& value)
    {
        SetPropertyAttribute(META_VERTEX, category, id, attr, value); 
    }

    void SetEdgePropertyAttribute(const CCategory& category, const int id, PROPERTY_ATTR attr, const CComVariant& value)
    {
        SetPropertyAttribute(META_EDGE, category, id, attr, value); 
    }

    unsigned int GetPropertyAttributes(IPropAttributeContainer* storage, META_TYPE entityType, const CCategory& category, const int id)
    {
        ATLASSERT(::IsWindow(m_hWnd));

        SGV_PROPERTYSTRUCT props;

        props.cat.plugin=category.GetPlugin().GetGUID();
        props.cat.category=category.GetCategory();
        props.propid=id;
        props.entity = entityType;

        return ::SendMessage(m_hWnd, SGV_GETPROPERTYATTRIBUTES, (WPARAM)&props, (LPARAM)storage);
    }

    // allows a generic search for dedup'd attributes applied to all property id's
    unsigned int GetPropertyAttributes(IPropAttributeContainer* storage, META_TYPE entityType, const CCategory& category)
    {
        return GetPropertyAttributes(storage, entityType, category, -1);
    }

    unsigned int GetVertexPropertyAttributes(IPropAttributeContainer* storage, const CCategory& category, const int id)
    {
        return GetPropertyAttributes(storage, META_VERTEX, category, id);
    }

    unsigned int GetEdgePropertyAttributes(IPropAttributeContainer* storage, const CCategory& category, const int id)
    {
        return GetPropertyAttributes(storage, META_EDGE, category, id);
    }
};

typedef CGraphViewCtrlT<CWindow> CGraphViewCtrl;
//------------------------------------------------------------------------------
class CGraphViewCtrlPause
{
protected:
    CGraphViewCtrl * m_ctrl;

public:
    CGraphViewCtrlPause(CGraphViewCtrl * ctrl)
    {
        m_ctrl = ctrl;
        m_ctrl->SetRedraw(false);

    }

    ~CGraphViewCtrlPause()
    {
        m_ctrl->SetRedraw(true);
    }
};

//-----------------------------------------------------------------------------
class CRenderer
{
protected:
    CGraphViewCtrl* m_ctrl;
    CUniqueID m_id;
    SGV_UNIQUEIDSTRUCT m_id_struct;

public:
    CRenderer(CGraphViewCtrl* ctrl, const CUniqueID& id)
        : m_ctrl(ctrl), m_id(id) 
    {
        ATLASSERT(ctrl);
        SetUIDStruct(m_id_struct, m_id);
    }

    bool GetBoundsBox(CRect& box) const
    {
        SGV_RECT rc;

        ATLASSERT(::IsWindow(m_ctrl->GetHWND()));

        bool bRet=::SendMessage(m_ctrl->GetHWND(), SGV_GETBOUNDSBOX, (WPARAM)&m_id_struct, (LPARAM)&rc)!=0;
        if(bRet)
        {
            box.left=rc.left;
            box.top=rc.top;
            box.right=rc.right;
            box.bottom=rc.bottom;
        }

        return bRet;
    }

    void SetBoundsBox(const CRect& box) const
    {
        SGV_RECT rc;

        ATLASSERT(::IsWindow(m_ctrl->GetHWND()));
        
        rc.left=box.left;
        rc.top=box.top;
        rc.right=box.right;
        rc.bottom=box.bottom;

        ::SendMessage(m_ctrl->GetHWND(), SGV_SETBOUNDSBOX, (WPARAM)&m_id_struct, (LPARAM)&rc);
    }

    void GetPosition(CPoint& pt) const
    {
        m_ctrl->GetPosition(m_id, pt); // always in screen coordinates
    }

    void SetPosition(const CPoint& pt) const
    {
        m_ctrl->SetPosition(m_id, pt.x, pt.y, false, GraphTypes::screen);
    }

    void SetSelected(bool bSelected=true, bool bAdd=false) const
    {
        m_ctrl->SetSelected(m_id, bSelected, bAdd);
    }

    bool HasSelectedVertex() const
    {
        return m_ctrl->HasSelectedVertex();
    }

    void SetFocus() const
    {
        m_ctrl->SetFocus(m_id);
    }

    void RecalcDisplaySize() const
    {
        ATLASSERT(::IsWindow(m_ctrl->GetHWND()));

        ::SendMessage(m_ctrl->GetHWND(), SGV_RECALCDISPSIZE, (WPARAM)&m_id_struct, 0);
    }

    static void SetUIDStruct(SGV_UNIQUEIDSTRUCT& id_struct, const CUniqueID& id)
    {
        id_struct.category.plugin = id.GetCategory().GetPlugin().GetGUID();
        id_struct.category.category = id.GetCategory().GetCategory();
        id_struct.id = id.GetID();
    }

};
//-----------------------------------------------------------------------------
#ifdef NO_LONGER_USED
class CListWindowRenderer : public CRenderer
{
public:
    CListWindowRenderer(CGraphViewCtrl* ctrl, const CUniqueID& id)
        : CRenderer(ctrl,id) 
    {}

    void SetWindowState(IListWindowRenderer::WINDOW_SIZE size=IListWindowRenderer::WINDOW_SIZE_NORM,
        int topindex=0) const
    {
        ATLASSERT(::IsWindow(m_ctrl->GetHWND()));

        SGV_WNDSTATE state;
        state.size=static_cast<int>(size);
        state.topindex=topindex;

        ::SendMessage(m_ctrl->GetHWND(), SGV_SETWINDOWSTATE, (WPARAM)&m_id_struct, (LPARAM)&state);
    }

    bool GetWindowState(IListWindowRenderer::WINDOW_SIZE& size, int& topindex) const
    {
        bool bOk=false;

        ATLASSERT(::IsWindow(m_ctrl->GetHWND()));

        SGV_WNDSTATE state;

        bOk=::SendMessage(m_ctrl->GetHWND(), SGV_GETWINDOWSTATE, (WPARAM)&m_id_struct, (LPARAM)&state)!=0;

        if(bOk)
        {
            size=static_cast<IListWindowRenderer::WINDOW_SIZE>(state.size);
            topindex=state.topindex;
        }

        return bOk;
    }

    void SetWindowSize(const CSize& size) const
    {
        ATLASSERT(::IsWindow(m_ctrl->GetHWND()));

        SGV_SIZE sgvsize;
        sgvsize.width=size.cx;
        sgvsize.height=size.cy;
        ::SendMessage(m_ctrl->GetHWND(), SGV_SETWINDOWSIZE, (WPARAM)&m_id_struct, (LPARAM)&sgvsize);
    }

    bool GetWindowSize(CSize& size) const
    {
        bool bOk=false;

        ATLASSERT(::IsWindow(m_ctrl->GetHWND()));

        SGV_SIZE sgvsize;
        bOk=::SendMessage(m_ctrl->GetHWND(), SGV_GETWINDOWSIZE, (WPARAM)&m_id_struct, (LPARAM)&sgvsize)!=0;
        if(bOk)
        {
            size.cx=sgvsize.width;
            size.cy=sgvsize.height;
        }

        return bOk;
    }
};
#endif
//  ===========================================================================

GRAPHVIEWCTL_API bool Register();
GRAPHVIEWCTL_API bool Register(HINSTANCE hInstance_);

GRAPHVIEWCTL_API const TCHAR * GetServerGraphViewCtlVersion(std::_tstring & version);
GRAPHVIEWCTL_API HRESULT RegisterServerGraphViewCtl(void);
GRAPHVIEWCTL_API HRESULT UnregisterServerGraphViewCtl(void);

} // namespace SGV


