#include "StdAfx.h"
#include "resource.h"
#include "IPictureHelper.h"
#include "Renderer.h"
#include "metaContainer.h"
#include "canvas.h"

using namespace GraphTypes;

CMeta::CMeta() 
    : 
    m_defsubgraphimagevis(VISIBLE_SHOW), 
    m_defverteximagevis(VISIBLE_SHOW), 
    m_defedgeimagevis(VISIBLE_SHOW), 
    m_debug(false), m_splines(false), m_dotLeftRight(true), 
    m_overview(false), m_overviewOpacity(255),
    m_grid(false), m_avoidObstacles(false), m_edgeLabelsVisible(true),
    m_compressRows(false), m_mouseMode(MOUSE_MODE_GRAB), m_edgeLabelOpacity(0),
    m_readOnly(false), 
    m_emulateDecoration(true)  //Got to default to true as we don't get a chance to set it it to true before creating the window in an ActiveX control...
{
    m_fgNormalQBColors[int(WUVisualState_unknown)]=GraphTypes::Color::Color(128,128,128);
    m_bgNormalQBColors[int(WUVisualState_unknown)]=GraphTypes::Color::White;
    m_fgSelectedQBColors[int(WUVisualState_unknown)]=GraphTypes::Color::Color(128,128,128);
    m_bgSelectedQBColors[int(WUVisualState_unknown)]=GraphTypes::Color::Color(198,198,198);
    m_fgHotQBColors[int(WUVisualState_unknown)]=GraphTypes::Color::Color(128,128,128);
    m_bgHotQBColors[int(WUVisualState_unknown)]=GraphTypes::Color::Color(224,224,224);

    m_fgNormalQBColors[int(WUVisualState_running)]=GraphTypes::Color::Black;
    m_bgNormalQBColors[int(WUVisualState_running)]=GraphTypes::Color::White;
    m_fgSelectedQBColors[int(WUVisualState_running)]=GraphTypes::Color::Black;
    m_bgSelectedQBColors[int(WUVisualState_running)]=GraphTypes::Color::Color(153,185,250);
    m_fgHotQBColors[int(WUVisualState_running)]=GraphTypes::Color::Black;
    m_bgHotQBColors[int(WUVisualState_running)]=GraphTypes::Color::Color(214,231,255);

    m_fgNormalQBColors[int(WUVisualState_completed)]=GraphTypes::Color::Color(0,0,255);
    m_bgNormalQBColors[int(WUVisualState_completed)]=GraphTypes::Color::White;
    m_fgSelectedQBColors[int(WUVisualState_completed)]=GraphTypes::Color::Color(0,0,255);
    m_bgSelectedQBColors[int(WUVisualState_completed)]=GraphTypes::Color::Color(214,231,255);
    m_fgHotQBColors[int(WUVisualState_completed)]=GraphTypes::Color::Color(0,0,255);
    m_bgHotQBColors[int(WUVisualState_completed)]=GraphTypes::Color::Color(214,231,255);

    m_fgNormalQBColors[int(WUVisualState_failed)]=GraphTypes::Color::Color(255,0,0);
    m_bgNormalQBColors[int(WUVisualState_failed)]=GraphTypes::Color::White;
    m_fgSelectedQBColors[int(WUVisualState_failed)]=GraphTypes::Color::Color(255,0,0);
    m_bgSelectedQBColors[int(WUVisualState_failed)]=GraphTypes::Color::Color(255,123,123);
    m_fgHotQBColors[int(WUVisualState_failed)]=GraphTypes::Color::Color(255,0,0);
    m_bgHotQBColors[int(WUVisualState_failed)]=GraphTypes::Color::Color(255,174,174);
}

void CMeta::SetMetaDebug(bool debugMode)
{
    m_debug = debugMode;
}

bool CMeta::GetMetaDebug() const
{
    return m_debug;
}

void CMeta::SetMetaSplines(bool splines)
{
    m_splines = splines;
}

bool CMeta::GetMetaSplines() const
{
    return m_splines;
}

void CMeta::SetMetaDotLeftRight(bool leftRight)
{
    m_dotLeftRight = leftRight;
}

bool CMeta::GetMetaDotLeftRight() const
{
    return m_dotLeftRight;
}

void CMeta::SetOverviewVisible(bool visible)
{
    m_overview=visible;
}

bool CMeta::IsOverviewVisible() const
{
    return m_overview;
}

void CMeta::SetMouseMode(MOUSE_MODES val)
{
    m_mouseMode = val;
}

MOUSE_MODES CMeta::GetMouseMode() const
{
    return m_mouseMode;
}

void CMeta::SetEdgeLabelVisible(bool visible)
{
    m_edgeLabelsVisible=visible;
}

bool CMeta::IsEdgeLabelVisible() const
{
    return m_edgeLabelsVisible;
}

void CMeta::SetGridVisible(bool visible)
{
    m_grid=visible;
}

bool CMeta::IsGridVisible() const
{
    return m_grid;
}

void CMeta::SetOverviewOpacity(BYTE opacity)
{
    m_overviewOpacity=opacity;
}

BYTE CMeta::GetOverviewOpacity() const
{
    return m_overviewOpacity;
}

bool CMeta::EmulateDecoration(bool emulate)
{
    bool bChange=m_emulateDecoration!=emulate;
    m_emulateDecoration=emulate;
    return bChange;
}

bool CMeta::IsEmulateDecoration() const
{
    return m_emulateDecoration;
}

void CMeta::AvoidObstacles(bool avoid)
{
    m_avoidObstacles=avoid;
}

bool CMeta::IsAvoidObstacles() const
{
    return m_avoidObstacles;
}

bool CMeta::SetRowCompress(bool compress)
{
    bool change=compress!=m_compressRows;
    m_compressRows=compress;
    return change;
}

bool CMeta::GetRowCompress() const
{
    return m_compressRows;
}

bool CMeta::SetReadOnly(bool ro)
{
    bool change=ro!=m_readOnly;
    m_readOnly=ro;
    return change;
}

bool CMeta::IsReadOnly() const
{
    return m_readOnly;
}

void CMeta::SetEdgeLabelOpacity(BYTE opacity)
{
    m_edgeLabelOpacity=opacity;
}

BYTE CMeta::GetEdgeLabelOpacity() const
{
    return m_edgeLabelOpacity;
}

CCategory CMeta::GetMetaXmlCatLookup(const TCHAR* lookup, META_TYPE metaType, META_CAT metaCat) const
{
    CCategory retVal;
    std::_tstring pluginWithCat = lookup;
    CGuid guidPluginLookup;
    std::_tstring strCatLookup;

    unsigned int pos = pluginWithCat.find('+');
    if (pos != std::_tstring::npos)
    {
        guidPluginLookup = CT2A(pluginWithCat.substr(0, pos).c_str());
        strCatLookup = pluginWithCat.substr(pos+1).c_str();
    }
    else
    {
        // support older calls / file formats where the plugin was not included
        strCatLookup = pluginWithCat.c_str();
        // create guid from default
        guidPluginLookup = guidDefault;
    }

    bool found = false;
    MetaValueMap::const_iterator itr = m_data.begin();
    while (itr != m_data.end())
    {
        const MetaTuple& tuple = itr->first;
        if (tuple.get<0>() == metaType && tuple.get<1>() == metaCat)
        {
            const CCategory& cat = tuple.get<2>();
            if (cat.GetPlugin() == guidPluginLookup && strCatLookup == (TCHAR*)bstr_t(itr->second))
            {
                retVal = cat;
                found = true;
                break;
            }
        }
        ++itr;
    }

    if (!found)
    {
        int cat = 0;
        if (strCatLookup.length())
        {
            try
            {
                cat = boost::lexical_cast<int>(strCatLookup);
            }
            catch (boost::bad_lexical_cast &)
            {
                // if bad will get "0" as category
            }
        }
        retVal = CCategory(guidPluginLookup.GetGUID(), cat); 
    }
    return retVal;
}

void CMeta::SetMetaVertexLabel(const CCategory & cat, const TCHAR* label)
{
    m_data[MetaTuple(META_VERTEX, LABEL, cat)] = label;
}

const TCHAR * CMeta::GetMetaVertexLabel(const CCategory & cat, std::_tstring & label) const
{
    MetaValueMap::const_iterator itr = m_data.find(MetaTuple(META_VERTEX, LABEL, cat));
    if (itr != m_data.end())
    {
        ATLASSERT(itr->second.vt == VT_BSTR);
        label = bstr_t(itr->second);
    }
    else
        label = _T("");
    return label.c_str();
}

void CMeta::SetMetaVertexXmlCatCode(const CCategory & cat, const TCHAR* value)
{
    m_data[MetaTuple(META_VERTEX, XML_CATEGORY_CODE, cat)] = value;
}

const TCHAR* CMeta::GetMetaVertexXmlCatCode(const CCategory & cat, std::_tstring & retVal) const
{
    retVal = _T("");
    MetaValueMap::const_iterator itr = m_data.find(MetaTuple(META_VERTEX, XML_CATEGORY_CODE, cat));
    if (itr != m_data.end() && itr->second.vt == VT_BSTR)
        retVal = bstr_t(itr->second);
    return retVal.c_str();
}

CCategory CMeta::GetMetaVertexXmlCatLookup(const TCHAR* lookup) const
{
    return GetMetaXmlCatLookup(lookup, META_VERTEX, XML_CATEGORY_CODE);
}

void CMeta::SetMetaVertexXmlAnbIconFile(const CCategory & cat, const TCHAR* value)
{
    m_data[MetaTuple(META_VERTEX, XML_ANBICONFILE, cat)] = value;
}

const TCHAR* CMeta::GetMetaVertexXmlAnbIconFile(const CCategory & cat, std::_tstring & retVal) const
{
    retVal = _T("");
    MetaValueMap::const_iterator itr = m_data.find(MetaTuple(META_VERTEX, XML_ANBICONFILE, cat));
    if (itr != m_data.end() && itr->second.vt == VT_BSTR)
        retVal = bstr_t(itr->second);
    return retVal.c_str();
}

void CMeta::SetMetaVertexColorFillNorm(const CCategory & cat, unsigned long color)
{
    if (color != 0)
        m_data_int[MetaTupleInt(META_VERTEX, FILL_COLOR, cat.GetHash())] = color;
}

unsigned long CMeta::GetMetaVertexColorFillNorm(const CCategory & cat) const
{
    MetaIntMap::const_iterator itr = m_data_int.find(MetaTupleInt(META_VERTEX, FILL_COLOR, cat.GetHash()));
    if (itr != m_data_int.end())
        return itr->second;
    Color clr;
    clr.SetFromCOLORREF(GetSysColor(COLOR_WINDOW));	
    return clr.GetValue();
}

void CMeta::SetMetaVertexColorBorderNorm(const CCategory & cat, unsigned long color)
{
    if (color != 0)
        m_data_int[MetaTupleInt(META_VERTEX, BORDER_COLOR, cat.GetHash())] = color;
}

unsigned long CMeta::GetMetaVertexColorBorderNorm(const CCategory & cat) const
{
    MetaIntMap::const_iterator itr = m_data_int.find(MetaTupleInt(META_VERTEX, BORDER_COLOR, cat.GetHash()));
    if (itr != m_data_int.end())
        return itr->second;
    Color clr;
    clr.SetFromCOLORREF(GetSysColor(COLOR_WINDOWTEXT));	
    return clr.GetValue();
}

void CMeta::SetMetaVertexColorFillSel(const CCategory & cat, unsigned long color)
{
    if (color != 0)
        m_data_int[MetaTupleInt(META_VERTEX, SEL_FILL_COLOR, cat.GetHash())] = color;
}

unsigned long CMeta::GetMetaVertexColorFillSel(const CCategory & cat) const
{
    MetaIntMap::const_iterator itr = m_data_int.find(MetaTupleInt(META_VERTEX, SEL_FILL_COLOR, cat.GetHash()));
    if (itr != m_data_int.end())
        return itr->second;
    Color clr;
    clr.SetFromCOLORREF(GetSysColor(COLOR_HIGHLIGHT));	
    return clr.GetValue();
}

void CMeta::SetMetaVertexColorBorderSel(const CCategory & cat, unsigned long color)
{
    if (color != 0)
        m_data_int[MetaTupleInt(META_VERTEX, SEL_BORDER_COLOR, cat.GetHash())] = color;
}

unsigned long CMeta::GetMetaVertexColorBorderSel(const CCategory & cat) const
{
    MetaIntMap::const_iterator itr = m_data_int.find(MetaTupleInt(META_VERTEX, SEL_BORDER_COLOR, cat.GetHash()));
    if (itr != m_data_int.end())
        return itr->second;
    Color clr;
    clr.SetFromCOLORREF(GetSysColor(COLOR_INACTIVECAPTION));	
    return clr.GetValue();
}

void CMeta::SetMetaVertexColorFillOver(const CCategory & cat, unsigned long color)
{
    if (color != 0)
        m_data_int[MetaTupleInt(META_VERTEX, OVER_FILL_COLOR, cat.GetHash())] = color;
}

unsigned long CMeta::GetMetaVertexColorFillOver(const CCategory & cat) const
{
    MetaIntMap::const_iterator itr = m_data_int.find(MetaTupleInt(META_VERTEX, OVER_FILL_COLOR, cat.GetHash()));
    if (itr != m_data_int.end())
        return itr->second;
    Color clr;
    clr.SetFromCOLORREF(GetSysColor(COLOR_INACTIVECAPTION));	
    return clr.GetValue();
}

void CMeta::SetMetaVertexColorBorderOver(const CCategory & cat, unsigned long color)
{
    if (color != 0)
        m_data_int[MetaTupleInt(META_VERTEX, OVER_BORDER_COLOR, cat.GetHash())] = color;
}

unsigned long CMeta::GetMetaVertexColorBorderOver(const CCategory & cat) const
{
    MetaIntMap::const_iterator itr = m_data_int.find(MetaTupleInt(META_VERTEX, OVER_BORDER_COLOR, cat.GetHash()));
    if (itr != m_data_int.end())
        return itr->second;
    Color clr;
    clr.SetFromCOLORREF(GetSysColor(COLOR_ACTIVECAPTION));	
    return clr.GetValue();
}

void CMeta::SetMetaVertexHide(const CCategory & cat, bool hide)
{
    m_data_int[MetaTupleInt(META_VERTEX, HIDE, cat.GetHash())] = hide ? VISIBLE_HIDE : VISIBLE_SHOW;
}

VISIBLE CMeta::GetMetaVertexHide(const CCategory & cat) const
{
    MetaIntMap::const_iterator itr = m_data_int.find(MetaTupleInt(META_VERTEX, HIDE, cat.GetHash()));
    if (itr != m_data_int.end())
        return static_cast<VISIBLE>(itr->second);
    // let caller know it is unset
    return VISIBLE_DEFAULT;
}

bool CMeta::GetVertexImagesVisible() const
{
    return m_defverteximagevis==VISIBLE_SHOW;
}
void CMeta::SetVertexImagesVisible(bool visible)
{
    m_defverteximagevis=visible?VISIBLE_SHOW:VISIBLE_HIDE;
}

void CMeta::SetMetaVertexFontColor(const CCategory & cat, unsigned long color)
{
    if (color != 0)
        m_data_int[MetaTupleInt(META_VERTEX, FONT_COLOR, cat.GetHash())] = color;
}

unsigned long CMeta::GetMetaVertexFontColor(const CCategory & cat) const
{
    MetaIntMap::const_iterator itr = m_data_int.find(MetaTupleInt(META_VERTEX, FONT_COLOR, cat.GetHash()));
    if (itr != m_data_int.end())
        return itr->second;
    Color clr;
    clr.SetFromCOLORREF(GetSysColor(COLOR_WINDOWTEXT));
    return clr.GetValue();
}

void CMeta::SetMetaVertexFontColorSel(const CCategory & cat, unsigned long color)
{
    if (color != 0)
        m_data_int[MetaTupleInt(META_VERTEX, SEL_FONT_COLOR, cat.GetHash())] = color;
}

unsigned long CMeta::GetMetaVertexFontColorSel(const CCategory & cat) const
{
    MetaIntMap::const_iterator itr = m_data_int.find(MetaTupleInt(META_VERTEX, SEL_FONT_COLOR, cat.GetHash()));
    if (itr != m_data_int.end())
        return itr->second;
    Color clr;
    clr.SetFromCOLORREF(GetSysColor(COLOR_HIGHLIGHTTEXT));
    return clr.GetValue();
}

void CMeta::SetMetaEdgeColorNorm(const CCategory & cat, unsigned long color)
{
    if (color != 0)
        m_data_int[MetaTupleInt(META_EDGE, EDGE_COLOR, cat.GetHash())] = color;
}

unsigned long CMeta::GetMetaEdgeColorNorm(const CCategory & cat) const
{
    MetaIntMap::const_iterator itr = m_data_int.find(MetaTupleInt(META_EDGE, EDGE_COLOR, cat.GetHash()));
    if (itr != m_data_int.end())
        return itr->second;
    Color clr;
    clr.SetFromCOLORREF(GetSysColor(COLOR_HIGHLIGHT));	
    return clr.GetValue();
}

void CMeta::SetMetaEdgeColorShortest(const CCategory & cat, unsigned long color)
{
    if (color != 0)
        m_data_int[MetaTupleInt(META_EDGE, EDGE_SHORTEST_PATH_COLOR, cat.GetHash())] = color;
}

unsigned long CMeta::GetMetaEdgeColorShortest(const CCategory & cat) const
{
    MetaIntMap::const_iterator itr = m_data_int.find(MetaTupleInt(META_EDGE, EDGE_SHORTEST_PATH_COLOR, cat.GetHash()));
    if (itr != m_data_int.end())
        return itr->second;
    return DEFAULT_EDGE_SHORTEST_PATH_COLOR;
}

void CMeta::SetMetaEdgeWidth(const CCategory & cat, unsigned short width)
{
    m_data_int[MetaTupleInt(META_EDGE, WIDTH, cat.GetHash())] = width;
}

unsigned short CMeta::GetMetaEdgeWidth(const CCategory & cat) const
{
    MetaIntMap::const_iterator itr = m_data_int.find(MetaTupleInt(META_EDGE, WIDTH, cat.GetHash()));
    if (itr != m_data_int.end())
        return itr->second;
    return DEFAULT_EDGE_WIDTH;
}

void CMeta::SetMetaEdgeHide(const CCategory & cat, bool hide)
{
    m_data_int[MetaTupleInt(META_EDGE, HIDE, cat.GetHash())] = static_cast<int>(hide);
}

bool CMeta::GetMetaEdgeHide(const CCategory & cat) const
{
    MetaIntMap::const_iterator itr = m_data_int.find(MetaTupleInt(META_EDGE, HIDE, cat.GetHash()));
    if (itr != m_data_int.end())
        return itr->second != 0;
    return false;
}

void CMeta::SetMetaEdgeXmlData(const CCategory & cat, const TCHAR* value)
{
    m_data[MetaTuple(META_EDGE, XML_CATEGORY_CODE, cat)] = value;
}

const TCHAR* CMeta::GetMetaEdgeXmlCatCode(const CCategory & cat, std::_tstring & retVal) const
{
    retVal = _T("");
    MetaValueMap::const_iterator itr = m_data.find(MetaTuple(META_EDGE, XML_CATEGORY_CODE, cat));
    if (itr != m_data.end() && itr->second.vt == VT_BSTR)
        retVal = bstr_t(itr->second);
    return retVal.c_str();
}

CCategory CMeta::GetMetaEdgeXmlCatLookup(const TCHAR* lookup) const
{
    return GetMetaXmlCatLookup(lookup, META_EDGE, XML_CATEGORY_CODE);
}

void CMeta::SetMetaPluginName(const GUID& pluginId, const TCHAR* name)
{
    m_data[MetaTuple(META_CATEGORY, PLUGIN_NAME, CCategory(pluginId, 0))] = name;
}

const TCHAR* CMeta::GetMetaPluginName(const GUID& pluginId, std::_tstring & retVal) const
{
    retVal = _T("");
    MetaValueMap::const_iterator itr = m_data.find(MetaTuple(META_CATEGORY, PLUGIN_NAME, CCategory(pluginId, 0)));
    if (itr != m_data.end() && itr->second.vt == VT_BSTR)
        retVal = bstr_t(itr->second);
    return retVal.c_str();
}

//------------------ image per vertex category  -------------------------------

void CMeta::SetVertexImageInfo(const CCategory& cat, const ImageInfo& iinfo, unsigned int overlay)
{
    m_vertexImage[MetaTuple(META_VERTEX, IUNKNOWN, cat)][static_cast<int>(iinfo.pos)][overlay] = iinfo;
}

const ImageInfoMaps* CMeta::GetVertexImageInfo(
    const CCategory& cat) const
{
    const ImageInfoMaps* info=0;
    MetaImageInfoMap::const_iterator itr=
        m_vertexImage.find(MetaTuple(META_VERTEX, IUNKNOWN, cat));

    if(itr != m_vertexImage.end())
        info=&itr->second;
    return info;
}

void CMeta::SetVertexImageVisible(const CCategory & cat, IMAGE_POSITION pos, VISIBLE visibility)
{
    m_verteximagevis[MetaTuple(META_VERTEX, IMAGE_VISIBLE, cat)][static_cast<unsigned>(pos)]=visibility;
}

VISIBLE CMeta::GetVertexImageVisible(const CCategory & cat, IMAGE_POSITION pos) const
{
    MetaVisibleImgMap::const_iterator itr =
        m_verteximagevis.find(MetaTuple(META_VERTEX, IMAGE_VISIBLE, cat));
    if (itr != m_verteximagevis.end())
        return itr->second[static_cast<unsigned>(pos)];
    return m_defverteximagevis;
}

//------------------ EOF image for vertex categories  -------------------------

//------------------ Image for edge categories ----------------------------

void CMeta::SetEdgeImageInfo(const CCategory& cat, const ImageInfo& iinfo, unsigned int overlay)
{
    m_edgeImage[MetaTuple(META_EDGE, IUNKNOWN, cat)][static_cast<int>(iinfo.pos)][overlay] = iinfo;
}

const ImageInfoMaps* CMeta::GetEdgeImageInfo(
        const CCategory& cat) const
{
    const ImageInfoMaps* info=0;
    MetaImageInfoMap::const_iterator itr=
        m_edgeImage.find(MetaTuple(META_EDGE, IUNKNOWN, cat));

    if(itr != m_edgeImage.end())
        info=&itr->second;
    return info;
}

void CMeta::SetEdgeImageVisible(const CCategory & cat, IMAGE_POSITION pos, VISIBLE visibility)
{
    m_edgeimagevis[MetaTuple(META_EDGE, IMAGE_VISIBLE, cat)][static_cast<unsigned>(pos)] = visibility;
}

VISIBLE CMeta::GetEdgeImageVisible(const CCategory & cat, IMAGE_POSITION pos) const
{
    MetaVisibleImgMap::const_iterator itr =
        m_edgeimagevis.find(MetaTuple(META_EDGE, IMAGE_VISIBLE, cat));
    if (itr != m_edgeimagevis.end())
        return itr->second[static_cast<unsigned>(pos)];
    return m_defedgeimagevis;
}

//------------------ EOF images for edge categories ------------------------


//------------------ label, label color for edge categories -------------------------------------

void CMeta::SetMetaEdgeLabel(const CCategory & cat, const TCHAR* label)
{
    m_data[MetaTuple(META_EDGE, LABEL, cat)] = label;
}

const TCHAR* CMeta::GetMetaEdgeLabel(const CCategory & cat, std::_tstring & label) const
{
    MetaValueMap::const_iterator itr = m_data.find(MetaTuple(META_EDGE, LABEL, cat));
    if (itr != m_data.end())
    {
        ATLASSERT(itr->second.vt == VT_BSTR);
        label = bstr_t(itr->second);
    }
    else
        label = _T("");
    return label.c_str();
}

void CMeta::SetMetaEdgeFontColor(const CCategory & cat, unsigned long color)
{
    if (color != 0)
        m_data_int[MetaTupleInt(META_EDGE, FONT_COLOR, cat.GetHash())] = color;
}

unsigned long CMeta::GetMetaEdgeFontColor(const CCategory & cat) const
{
    MetaIntMap::const_iterator itr = m_data_int.find(MetaTupleInt(META_EDGE, FONT_COLOR, cat.GetHash()));
    if (itr != m_data_int.end())
        return itr->second;
    Color clr;
    clr.SetFromCOLORREF(GetSysColor(COLOR_WINDOWTEXT));
    return clr.GetValue();
}

//------------------ EOF label, label color for edge categories ---------------

//------------------ Subgraph label, colors, images, visibility, etc. ---------

void CMeta::SetMetaSubgraphLabel(const CCategory & cat, const TCHAR* label)
{
    m_data[MetaTuple(META_SUBGRAPH, LABEL, cat)] = label;
}

const TCHAR* CMeta::GetMetaSubgraphLabel(const CCategory & cat, std::_tstring & label) const
{
    MetaValueMap::const_iterator itr = m_data.find(MetaTuple(META_SUBGRAPH, LABEL, cat));
    if (itr != m_data.end())
    {
        ATLASSERT(itr->second.vt == VT_BSTR);
        label = bstr_t(itr->second);
    }
    else
        label = _T("");
    return label.c_str();
}

void CMeta::SetMetaSubgraphColorFillNorm(const CCategory & cat, unsigned long color)
{
    if (color != 0)
        m_data_int[MetaTupleInt(META_SUBGRAPH, FILL_COLOR, cat.GetHash())] = color;
}

unsigned long CMeta::GetMetaSubgraphColorFillNorm(const CCategory & cat) const
{
    MetaIntMap::const_iterator itr = m_data_int.find(MetaTupleInt(META_SUBGRAPH, FILL_COLOR, cat.GetHash()));
    if (itr != m_data_int.end())
        return itr->second;
    Color clr;
    clr.SetFromCOLORREF(GetSysColor(COLOR_WINDOW));	
    return clr.GetValue();
}

void CMeta::SetMetaSubgraphColorBorderNorm(const CCategory & cat, unsigned long color)
{
    if (color != 0)
        m_data_int[MetaTupleInt(META_SUBGRAPH, BORDER_COLOR, cat.GetHash())] = color;
}

unsigned long CMeta::GetMetaSubgraphColorBorderNorm(const CCategory & cat) const
{
    MetaIntMap::const_iterator itr = m_data_int.find(MetaTupleInt(META_SUBGRAPH, BORDER_COLOR, cat.GetHash()));
    if (itr != m_data_int.end())
        return itr->second;
    Color clr;
    clr.SetFromCOLORREF(GetSysColor(COLOR_WINDOWTEXT));	
    return clr.GetValue();
}

void CMeta::SetMetaSubgraphColorFillSel(const CCategory & cat, unsigned long color)
{
    if (color != 0)
        m_data_int[MetaTupleInt(META_SUBGRAPH, SEL_FILL_COLOR, cat.GetHash())] = color;
}

unsigned long CMeta::GetMetaSubgraphColorFillSel(const CCategory & cat) const
{
    MetaIntMap::const_iterator itr = m_data_int.find(MetaTupleInt(META_SUBGRAPH, SEL_FILL_COLOR, cat.GetHash()));
    if (itr != m_data_int.end())
        return itr->second;
    Color clr;
    clr.SetFromCOLORREF(GetSysColor(COLOR_HIGHLIGHT));	
    return clr.GetValue();
}

void CMeta::SetMetaSubgraphColorBorderSel(const CCategory & cat, unsigned long color)
{
    if (color != 0)
        m_data_int[MetaTupleInt(META_SUBGRAPH, SEL_BORDER_COLOR, cat.GetHash())] = color;
}

unsigned long CMeta::GetMetaSubgraphColorBorderSel(const CCategory & cat) const
{
    MetaIntMap::const_iterator itr = m_data_int.find(MetaTupleInt(META_SUBGRAPH, SEL_BORDER_COLOR, cat.GetHash()));
    if (itr != m_data_int.end())
        return itr->second;
    Color clr;
    clr.SetFromCOLORREF(GetSysColor(COLOR_INACTIVECAPTION));	
    return clr.GetValue();
}

void CMeta::SetMetaSubgraphColorFillOver(const CCategory & cat, unsigned long color)
{
    if (color != 0)
        m_data_int[MetaTupleInt(META_SUBGRAPH, OVER_FILL_COLOR, cat.GetHash())] = color;
}

unsigned long CMeta::GetMetaSubgraphColorFillOver(const CCategory & cat) const
{
    MetaIntMap::const_iterator itr = m_data_int.find(MetaTupleInt(META_SUBGRAPH, OVER_FILL_COLOR, cat.GetHash()));
    if (itr != m_data_int.end())
        return itr->second;
    Color clr;
    clr.SetFromCOLORREF(GetSysColor(COLOR_INACTIVECAPTION));	
    return clr.GetValue();
}

void CMeta::SetMetaSubgraphColorBorderOver(const CCategory & cat, unsigned long color)
{
    if (color != 0)
        m_data_int[MetaTupleInt(META_SUBGRAPH, OVER_BORDER_COLOR, cat.GetHash())] = color;
}

unsigned long CMeta::GetMetaSubgraphColorBorderOver(const CCategory & cat) const
{
    MetaIntMap::const_iterator itr = m_data_int.find(MetaTupleInt(META_SUBGRAPH, OVER_BORDER_COLOR, cat.GetHash()));
    if (itr != m_data_int.end())
        return itr->second;
    Color clr;
    clr.SetFromCOLORREF(GetSysColor(COLOR_ACTIVECAPTION));	
    return clr.GetValue();
}

void CMeta::SetMetaSubgraphFontColor(const CCategory & cat, unsigned long color)
{
    if (color != 0)
        m_data_int[MetaTupleInt(META_SUBGRAPH, FONT_COLOR, cat.GetHash())] = color;
}

unsigned long CMeta::GetMetaSubgraphFontColor(const CCategory & cat) const
{
    MetaIntMap::const_iterator itr = m_data_int.find(MetaTupleInt(META_SUBGRAPH, FONT_COLOR, cat.GetHash()));
    if (itr != m_data_int.end())
        return itr->second;
    Color clr;
    clr.SetFromCOLORREF(GetSysColor(COLOR_WINDOWTEXT));
    return clr.GetValue();
}

void CMeta::SetMetaSubgraphFontColorSel(const CCategory & cat, unsigned long color)
{
    if (color != 0)
        m_data_int[MetaTupleInt(META_SUBGRAPH, SEL_FONT_COLOR, cat.GetHash())] = color;
}

unsigned long CMeta::GetMetaSubgraphFontColorSel(const CCategory & cat) const
{
    MetaIntMap::const_iterator itr = m_data_int.find(MetaTupleInt(META_SUBGRAPH, SEL_FONT_COLOR, cat.GetHash()));
    if (itr != m_data_int.end())
        return itr->second;
    Color clr;
    clr.SetFromCOLORREF(GetSysColor(COLOR_HIGHLIGHTTEXT));
    return clr.GetValue();
}

void CMeta::SetMetaSubgraphHide(const CCategory & cat, bool hide)
{
    m_data_int[MetaTupleInt(META_SUBGRAPH, HIDE, cat.GetHash())] = static_cast<int>(hide);
}

bool CMeta::GetMetaSubgraphHide(const CCategory & cat) const
{
    MetaIntMap::const_iterator itr = m_data_int.find(MetaTupleInt(META_SUBGRAPH, HIDE, cat.GetHash()));
    if (itr != m_data_int.end())
        return itr->second != 0;
    return false;
}

//------------------ Images for subgraph ----------------------------

void CMeta::SetSubgraphImageInfo(const CCategory& cat, const ImageInfo& iinfo, unsigned int overlay)
{
    m_subgraphImage[MetaTuple(META_SUBGRAPH, IUNKNOWN, cat)][static_cast<int>(iinfo.pos)][overlay] = iinfo;
}

const ImageInfoMaps* CMeta::GetSubgraphImageInfo(
        const CCategory& cat) const
{
    const ImageInfoMaps* info=0;
    MetaImageInfoMap::const_iterator itr=
        m_subgraphImage.find(MetaTuple(META_SUBGRAPH, IUNKNOWN, cat));

    if(itr != m_subgraphImage.end())
        info=&itr->second;
    return info;
}

void CMeta::SetSubgraphImageVisible(const CCategory & cat, IMAGE_POSITION pos, VISIBLE visibility)
{
    m_subgraphimagevis[MetaTuple(META_SUBGRAPH, IMAGE_VISIBLE, cat)][static_cast<unsigned>(pos)] = visibility;
}

VISIBLE CMeta::GetSubgraphImageVisible(const CCategory & cat, IMAGE_POSITION pos) const
{
    MetaVisibleImgMap::const_iterator itr =
        m_subgraphimagevis.find(MetaTuple(META_SUBGRAPH, IMAGE_VISIBLE, cat));
    if (itr != m_subgraphimagevis.end())
        return itr->second[static_cast<unsigned>(pos)];
    return m_defsubgraphimagevis;
}

const TCHAR* CMeta::GetMetaSubgraphXmlCatCode(const CCategory & cat, std::_tstring & retVal) const
{
    retVal = _T("");
    MetaValueMap::const_iterator itr = m_data.find(MetaTuple(META_SUBGRAPH, XML_CATEGORY_CODE, cat));
    if (itr != m_data.end() && itr->second.vt == VT_BSTR)
        retVal = bstr_t(itr->second);
    return retVal.c_str();
}

void CMeta::SetMetaSubgraphXmlCatCode(const CCategory & cat, const TCHAR* value)
{
    m_data[MetaTuple(META_SUBGRAPH, XML_CATEGORY_CODE, cat)] = value;
}

const TCHAR* CMeta::GetMetaSubgraphXmlAnbIconFile(const CCategory & cat, std::_tstring & retVal) const
{
    retVal = _T("");
    MetaValueMap::const_iterator itr = m_data.find(MetaTuple(META_SUBGRAPH, XML_ANBICONFILE, cat));
    if (itr != m_data.end() && itr->second.vt == VT_BSTR)
        retVal = bstr_t(itr->second);
    return retVal.c_str();
}

void CMeta::SetMetaSubgraphXmlAnbIconFile(const CCategory & cat, const TCHAR* value)
{
    m_data[MetaTuple(META_SUBGRAPH, XML_ANBICONFILE, cat)] = value;
}

CCategory CMeta::GetMetaSubgraphXmlCatLookup(const TCHAR* lookup) const
{
    return GetMetaXmlCatLookup(lookup, META_SUBGRAPH, XML_CATEGORY_CODE);
}


//------------------ EOF Subgraph colors, images, visibility, etc. ------------


//------------------ Version 2.0 properties -----------------------------------

void CMeta::SetPropertyAttribute(META_TYPE entityType, const CCategory& category,
    const int id, PROPERTY_ATTR attr, const CComVariant& value)
{
    PropAttributes* propAttrs = NULL;

    switch (entityType)
    {
    case META_VERTEX:
        propAttrs = &m_vertexPropertyMap[category][id];
        break;
    case META_EDGE:
        propAttrs = &m_edgePropertyMap[category][id];
        break;
    case META_SUBGRAPH:
        propAttrs = &m_subgraphPropertyMap[category][id];
        break;
    }
    if (propAttrs)
        propAttrs->SetValue(attr, value);
}

//-----------------------------------------------------------------------------

std::pair<CategoryPropertyMap::const_iterator, CategoryPropertyMap::const_iterator> CMeta::GetCategoryPropertyAttrIterator(META_TYPE entityType, const CCategory& category)
{
    std::pair<CategoryPropertyMap::const_iterator, CategoryPropertyMap::const_iterator> retval;

    switch (entityType)
    {
    case META_VERTEX:
        retval.first = m_vertexPropertyMap.find(category);
        retval.second = m_vertexPropertyMap.end();
        break;
    case META_EDGE:
        retval.first = m_edgePropertyMap.find(category);
        retval.second = m_edgePropertyMap.end();
        break;
    case META_SUBGRAPH:
        retval.first = m_subgraphPropertyMap.find(category);
        retval.second = m_subgraphPropertyMap.end();
        break;
    }
    return retval;
}

unsigned int CMeta::GetPropertyAttributes(IPropAttributeContainer* storage,
    META_TYPE entityType, const CCategory& category, const int id)
{
    unsigned count=0;

    std::pair<CategoryPropertyMap::const_iterator, CategoryPropertyMap::const_iterator> itrPair = GetCategoryPropertyAttrIterator(entityType, category);

    // if iterator equals end of list
    if (itrPair.first == itrPair.second)
        return count;

    CategoryPropertyMap::const_iterator itr = itrPair.first;

    PropAttrMap::const_iterator itrProps;

    // -1 is a special case asking for all attributes
    if (id == -1)
        itrProps = itr->second.begin();
    else
        itrProps = itr->second.find(id);

    while (itrProps != itr->second.end())
    {
        const PropAttributes& pa = itrProps->second;
        PropAttributes::AttrValVector::const_iterator itrv=pa.avec.begin();	
        while(itrv!=pa.avec.end())
        {
            storage->Add(std::pair<AttrValue, int>(*itrv, itrProps->first));
            ++itrv;
            ++count;
        }

        // only do it once for the specified property unless -1 is given.
        if (id != -1)
            break;

        ++itrProps;
    }
    return count;
}

unsigned int CMeta::GetPropertyAttributes(IPropAttributeContainer* storage,
    META_TYPE entityType, const CCategory& category)
{
    return GetPropertyAttributes(storage, entityType, category, -1);
}

unsigned int CMeta::GetPropertyAttributeId(META_TYPE entityType, const CCategory& category, const PROPERTY_ATTR& attr, const CComVariant& value, bool autoGen)
{
    int id = -1;
    std::pair<CategoryPropertyMap::const_iterator, CategoryPropertyMap::const_iterator> itrPair = GetCategoryPropertyAttrIterator(entityType, category);

    // only continue if the iterator is not at end of list (empty)
    if (itrPair.first != itrPair.second)
    {
        CategoryPropertyMap::const_iterator itr = itrPair.first;

        PropAttrMap::const_iterator itrPropIdToAttrs;
        for (itrPropIdToAttrs = itr->second.begin(); itrPropIdToAttrs != itr->second.end(); ++itrPropIdToAttrs)
        {
            if (itrPropIdToAttrs == itr->second.end())
                break; // empty

            const PropAttributes& pa = itrPropIdToAttrs->second;
            PropAttributes::AttrValVector::const_iterator itrv=pa.avec.begin();	
            while (itrv != pa.avec.end())
            {
                if (itrv->attr == attr && itrv->value == value)
                {
                    id = itrPropIdToAttrs->first;
                    break;
                }
                ++itrv;
            }
        }
    }
    if (autoGen && id == -1)
    {
        // TODO: this can probably be done better
        PropAttrMap* propAttrMap = 0;
        switch (entityType)
        {
        case META_VERTEX:
            propAttrMap = &m_vertexPropertyMap[category];
            break;
        case META_EDGE:
            propAttrMap = &m_edgePropertyMap[category];
            break;
        case META_SUBGRAPH:
            propAttrMap = &m_subgraphPropertyMap[category];
            break;
        }

        PropAttrMap::reverse_iterator itr = propAttrMap->rbegin();
        if (itr != propAttrMap->rend())
            id = itr->first;
        PropAttributes* propAttrs = &((*propAttrMap)[++id]);
        propAttrs->SetValue(attr, value);
    }
    return id;
}

//------------------ EOF version 2.0 properties --------------------------------

std::pair<MetaValueMap::const_iterator, MetaValueMap::const_iterator> CMeta::GetIterator() const
{ 
    std::pair<MetaValueMap::const_iterator, MetaValueMap::const_iterator> retVal;
    retVal.first = m_data.begin();
    retVal.second = m_data.end();
    return retVal;
}

bool CMeta::IsHidden(IGraphItem * i) const
{
    CComQIPtr<IGraphEdge> e=i;
    if(e)
    {
        return IsHidden(e);
    }
    CComQIPtr<IGraphVertex> v=i;
    if(v)
    {
        return IsHidden(v);
    }
    CComQIPtr<IGraphSubgraph> sg=i;
    if(sg)
    {
        return IsHidden(sg);
    }
    return false;
}

bool CMeta::IsHidden(IGraphSubgraph * sg) const
{
    VISIBLE visible = sg->GetVisible();
    if (visible == VISIBLE_DEFAULT)
        return GetMetaSubgraphHide(sg->GetCategory());
    else if(visible == VISIBLE_HIDE)
        return true;
    return false;
}

bool CMeta::IsHidden(IGraphVertex * v) const
{
    VISIBLE visible = v->GetVisible();
    if (visible == VISIBLE_DEFAULT)
        return GetMetaVertexHide(v->GetCategory()) == VISIBLE_HIDE;
    else if (visible == VISIBLE_HIDE)
        return true;
    return false;
}

bool CMeta::IsHidden(IGraphEdge * e) const
{
    bool retVal = false;
    VISIBLE visible = e->GetVisible();
    if (visible == VISIBLE_DEFAULT)
        retVal = GetMetaEdgeHide(e->GetCategory());
    else if(visible == VISIBLE_HIDE)
        retVal = true;
    CComPtr<IGraphVertex> source = e->GetSource();
    CComPtr<IGraphVertex> target = e->GetTarget();
    return (retVal || IsHidden(source) || IsHidden(target));
}

bool CMeta::IsVisible(IGraphItem * i) const
{
    return !IsHidden(i);
}

bool CMeta::IsVisible(IGraphSubgraph * sg) const
{
    return !IsHidden(sg);
}

bool CMeta::IsVisible(IGraphVertex * v) const
{
    return !IsHidden(v);
}

bool CMeta::IsVisible(IGraphEdge * e) const
{
    return !IsHidden(e);
}

unsigned int CMeta::GetVisibleUnpinnedVertexCount(IGraph * graph, const CCanvas * canvas) const
{
    unsigned int count = 0;

    CComPtr<MetaStateSkipper> skipper = new MetaStateSkipper(*this, canvas, TRISTATE_TRUE, TRISTATE_FALSE);
    CComPtr<IVertexIterator> itr = graph->GetUserVertexIterator(skipper);
    while (itr->IsValidItem())
    {
        itr->NextItem();
        count++;
    }
    return count;
}

bool CMeta::IsImageVisible(IGraphItem * i, IMAGE_POSITION pos) const
{
    CComQIPtr<IGraphEdge> e=i;
    if(e)
    {
        return IsImageVisible(e, pos);
    }
    CComQIPtr<IGraphVertex> v=i;
    if(v)
    {
        return IsImageVisible(v, pos);
    }
    CComQIPtr<IGraphSubgraph> sg=i;
    if(sg)
    {
        return IsImageVisible(sg, pos);
    }
    return false;
}

bool CMeta::IsImageVisible(IGraphSubgraph* sg, IMAGE_POSITION pos) const
{
    VISIBLE vis=sg->GetImageVisible(pos);
    if(vis==VISIBLE_DEFAULT)
        return GetSubgraphImageVisible(sg->GetCategory(), pos)==VISIBLE_SHOW;
    else
        return vis==VISIBLE_SHOW;
}

bool CMeta::IsImageVisible(IGraphVertex* v, IMAGE_POSITION pos) const
{
    VISIBLE vis=v->GetImageVisible(pos);
    if(vis==VISIBLE_DEFAULT)
        return GetVertexImageVisible(v->GetCategory(), pos)==VISIBLE_SHOW;
    else
        return vis==VISIBLE_SHOW;
}

bool CMeta::IsImageVisible(IGraphEdge* e, IMAGE_POSITION pos) const
{
    VISIBLE vis=e->GetImageVisible(pos);
    if(vis==VISIBLE_DEFAULT)
        return GetEdgeImageVisible(e->GetCategory(), pos)==VISIBLE_SHOW;
    else
        return vis==VISIBLE_SHOW;
}

MetaStateSkipper::MetaStateSkipper(const CMeta & meta, const CCanvas * canvas, TRISTATE visible, TRISTATE pinned, TRISTATE selected)
: MetaSkipper(meta), m_canvas(canvas), m_visible(visible), m_pinned(pinned), m_selected(selected)
{
}

bool MetaStateSkipper::SkipSubgraph(IGraphSubgraph * sg) const
{
    switch(m_visible)
    {
    case TRISTATE_TRUE:
        if (m_meta.IsHidden(sg))
        {
            return true;
        }
        break;
    case TRISTATE_FALSE:
        if (m_meta.IsVisible(sg))
        {
            return true;
        }
        break;
    }

    CComQIPtr<ILegacySubgraphRenderer> lsgr=m_canvas->GetRenderer(sg);
    if(lsgr!=0)
    {
        switch(m_pinned)
        {
        case TRISTATE_TRUE:
            if (!lsgr->GetPinned())
                return true;
            break;
        case TRISTATE_FALSE:
            if (lsgr->GetPinned())
                return true;
            break;
        }
    }

    // the most base subgraph renderer that implements IsSelected()
    CComQIPtr<IListWindowRenderer> ir=m_canvas->GetRenderer(sg);
    switch(m_selected)
    {
    case TRISTATE_TRUE:
        if (!ir->IsSelected())
            return true;
        break;
    case TRISTATE_FALSE:
        if (ir->IsSelected())
            return true;
        break;
    }

    return false;
}


bool MetaStateSkipper::SkipVertex(IGraphVertex * v) const
{
    switch(m_visible)
    {
    case TRISTATE_TRUE:
        if (m_meta.IsHidden(v))
            return true;
        break;
    case TRISTATE_FALSE:
        if (m_meta.IsVisible(v))
            return true;
        break;
    }

    CComQIPtr<IVertexRenderer> lr = m_canvas->GetRenderer(v);
    if(lr!=0)
    {
        switch(m_pinned)
        {
        case TRISTATE_TRUE:
            if (!lr->GetPinned())
                return true;
            break;
        case TRISTATE_FALSE:
            if (lr->GetPinned())
                return true;
            break;
        }

        switch(m_selected)
        {
        case TRISTATE_TRUE:
            if (!lr->IsSelected())
                return true;
            break;
        case TRISTATE_FALSE:
            if (lr->IsSelected())
                return true;
            break;
        }
    }

    return false;
}

bool MetaStateSkipper::SkipEdge(IGraphEdge * e) const
{
    switch(m_visible)
    {
    case TRISTATE_TRUE:
        if (m_meta.IsHidden(e))
        {
            return true;
        }
        break;
    case TRISTATE_FALSE:
        if (m_meta.IsVisible(e))
        {
            return true;
        }
        break;
    }

    CComQIPtr<IEdgeRenderer> lr = m_canvas->GetRenderer(e);
    switch(m_selected)
    {
    case TRISTATE_TRUE:
        if (!lr->IsSelected())
            return true;
        break;
    case TRISTATE_FALSE:
        if (lr->IsSelected())
            return true;
        break;
    }

    return false;
}

bool MetaStateSkipper::SkipItem(IGraphItem * i) const
{
    CComQIPtr<IGraphVertex> v=i;
    if(v)
        return SkipVertex(v);

    CComQIPtr<IGraphEdge> e=i;
    if(e)
        return SkipEdge(e);

    CComQIPtr<IGraphSubgraph> s=i;
    if(s)
        return SkipSubgraph(s);

    ATLASSERT(!"What is it?");
    return false;
}


