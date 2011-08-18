#pragma once

#include "meta.h"
#include "GraphTypes.h"
#include "GraphContainer.h"
#include "Iterators.h"

const int DEFAULT_EDGE_SHORTEST_PATH_COLOR = GraphTypes::Color::Red;
const int DEFAULT_EDGE_WIDTH = 4;

class CCanvas;

// These enum's are exposed via the Managed control, so the values must not change.
enum PROPERTY_ATTR
{
	PA_UNKNOWN = 0,
	PA_SHORT_NAME = 1,
	PA_LONG_NAME = 2,
	PA_SERIALIZE = 3,	
	PA_READ_ONLY = 4,
	PA_HIDDEN = 5,
	PA_READ_ONLY_NAME = 6,
	PA_DEFAULT_VISIBILITY = 7,
	PA_LAST = 8
};

enum MOUSE_MODES {
	 MOUSE_MODE_GRAB = 0,
	 MOUSE_MODE_SELECTION = 1,
	 MOUSE_MODE_DRAW_LINKS = 2
};

struct AttrValue
{
	PROPERTY_ATTR attr;
	CComVariant value;

	AttrValue() : attr(PA_UNKNOWN) {};
	AttrValue(const AttrValue& av) : attr(av.attr), value(av.value) {}
	AttrValue(PROPERTY_ATTR att, const CComVariant& val) : attr(att), value(val) {}

	AttrValue& operator=(const AttrValue& av)
	{
		attr=av.attr;
		value=av.value;
		return *this;
	}
};

typedef ISGVContainer<std::pair<AttrValue, int> > IPropAttributeContainer;
typedef CContainer<std::pair<AttrValue, int> > CPropAttributeVector;

struct PropAttributes
{
	typedef std::vector<AttrValue> AttrValVector;
	AttrValVector avec;	
	bool SetValue(PROPERTY_ATTR att, const CComVariant& val)
	{
		bool bFound=false;
		AttrValVector::iterator itr=avec.begin();
		while(itr!=avec.end())
		{
			if((*itr).attr==att)
			{
				bFound=true;
				break;
			}
			++itr;
		}

		if(bFound)
			(*itr).value=val;
		else
			avec.push_back(AttrValue(att,val));

		return bFound;
	}
};

typedef std::map<int, PropAttributes> PropAttrMap;
typedef std::map<CCategory, PropAttrMap> CategoryPropertyMap;
//*****************************************************************************

#ifndef EXCLUDE_BOOST
class CMeta
{
private:
	// properties attached to vertex and edge entities
	CategoryPropertyMap m_vertexPropertyMap;
	CategoryPropertyMap m_edgePropertyMap;
	CategoryPropertyMap m_subgraphPropertyMap;
protected:
	bool m_debug;
	MetaValueMap m_data;
	MetaIntMap m_data_int;

	MetaImageInfoMap m_subgraphImage;
	// subgraph image visibility per subgraph category
	MetaVisibleImgMap m_subgraphimagevis;
	VISIBLE m_defsubgraphimagevis;

	MetaImageInfoMap m_vertexImage;
	// vertex image visibility per vertex category
	MetaVisibleImgMap m_verteximagevis;
	VISIBLE m_defverteximagevis;

	MetaImageInfoMap m_edgeImage;
	// edge image visibility per category
	MetaVisibleImgMap m_edgeimagevis;
	VISIBLE m_defedgeimagevis;

	bool m_splines;
	bool m_dotLeftRight;
	bool m_overview;
	MOUSE_MODES m_mouseMode;
	bool m_edgeLabelsVisible;
	BYTE m_overviewOpacity;
	bool m_emulateDecoration; // decoration is scrollbars and overview
	bool m_grid;
	bool m_avoidObstacles;
	bool m_compressRows;
	BYTE m_edgeLabelOpacity;
	bool m_readOnly;

public:

	// maybe move these to private when accessors provided

	GraphTypes::Color m_fgNormalQBColors[WUVisualState_last];
	GraphTypes::Color m_bgNormalQBColors[WUVisualState_last];

	GraphTypes::Color m_fgSelectedQBColors[WUVisualState_last];
	GraphTypes::Color m_bgSelectedQBColors[WUVisualState_last];

	GraphTypes::Color m_fgHotQBColors[WUVisualState_last];
	GraphTypes::Color m_bgHotQBColors[WUVisualState_last];

	// construction
	CMeta();

	//  Global
	void SetMetaDebug(bool debugMode);
	bool GetMetaDebug() const;
	void SetMetaSplines(bool spines);
	bool GetMetaSplines() const;
	void SetMetaDotLeftRight(bool lr);
	bool GetMetaDotLeftRight() const;
	void SetOverviewVisible(bool visible);
	bool IsOverviewVisible() const;
	void SetMouseMode(MOUSE_MODES val);
	MOUSE_MODES GetMouseMode() const;
	void SetEdgeLabelVisible(bool visible);
	bool IsEdgeLabelVisible() const;
	void SetGridVisible(bool visible);
	bool IsGridVisible() const;
	void SetOverviewOpacity(BYTE opacity);
	BYTE GetOverviewOpacity() const;
	bool EmulateDecoration(bool emulate);
	bool IsEmulateDecoration() const;
	void AvoidObstacles(bool avoid);
	bool IsAvoidObstacles() const;
	bool SetRowCompress(bool compress);
	bool GetRowCompress() const;
	void SetEdgeLabelOpacity(BYTE opacity);
	BYTE GetEdgeLabelOpacity() const;
	bool SetReadOnly(bool ro); 
	bool IsReadOnly() const;


	CCategory GetMetaXmlCatLookup(const TCHAR* lookup, META_TYPE metaType, META_CAT metaCat) const;

	//Vertex
	void SetMetaVertexLabel(const CCategory & cat, const TCHAR* label);	//What the category means ("Person", "Company" etc.)
	const TCHAR* GetMetaVertexLabel(const CCategory & cat, std::_tstring & label) const;	
	void SetMetaVertexPropertyName(const CCategory & propertyID, const TCHAR* name);
	const TCHAR* GetMetaVertexPropertyName(const CCategory & cat, std::_tstring & name) const;
	void SetMetaVertexXmlCatCode(const CCategory & cat, const TCHAR* value);
	const TCHAR* GetMetaVertexXmlCatCode(const CCategory & cat, std::_tstring & retVal) const;
	CCategory GetMetaVertexXmlCatLookup(const TCHAR* lookup) const;
	void SetMetaVertexXmlAnbIconFile(const CCategory & cat, const TCHAR* value);
	const TCHAR* GetMetaVertexXmlAnbIconFile(const CCategory & cat, std::_tstring & retVal) const;
	void SetMetaVertexColorFillNorm(const CCategory & cat, unsigned long color);
	unsigned long GetMetaVertexColorFillNorm(const CCategory & cat) const;
	void SetMetaVertexColorBorderNorm(const CCategory & cat, unsigned long color);
	unsigned long GetMetaVertexColorBorderNorm(const CCategory & cat) const;
	void SetMetaVertexColorFillSel(const CCategory & cat, unsigned long color);
	unsigned long GetMetaVertexColorFillSel(const CCategory & cat) const;
	void SetMetaVertexColorBorderSel(const CCategory & cat, unsigned long color);
	unsigned long GetMetaVertexColorBorderSel(const CCategory & cat) const;
	void SetMetaVertexColorFillOver(const CCategory & cat, unsigned long color);
	unsigned long GetMetaVertexColorFillOver(const CCategory & cat) const;
	void SetMetaVertexColorBorderOver(const CCategory & cat, unsigned long color);
	unsigned long GetMetaVertexColorBorderOver(const CCategory & cat) const;
	void SetMetaVertexFontColor(const CCategory & cat, unsigned long color);
	unsigned long GetMetaVertexFontColor(const CCategory & cat) const;
	void SetMetaVertexFontColorSel(const CCategory & cat, unsigned long color);
	unsigned long GetMetaVertexFontColorSel(const CCategory & cat) const;
	void SetMetaVertexHide(const CCategory & cat, bool hide);
	VISIBLE GetMetaVertexHide(const CCategory & cat) const;

	// vertex images
	bool GetVertexImagesVisible() const;
	void SetVertexImagesVisible(bool visible);

	void SetVertexImageInfo(const CCategory& cat, const ImageInfo& iinfo, unsigned int overlay);
	const ImageInfoMaps* GetVertexImageInfo(const CCategory& cat) const;	

	void SetVertexImageVisible(const CCategory & cat, IMAGE_POSITION pos, VISIBLE visibility);
	VISIBLE GetVertexImageVisible(const CCategory & cat, IMAGE_POSITION pos) const;

	//Edge
	void SetMetaEdgeColorNorm(const CCategory & cat, unsigned long color);
	unsigned long GetMetaEdgeColorNorm(const CCategory & cat) const;
	void SetMetaEdgeColorShortest(const CCategory & cat, unsigned long color);
	unsigned long GetMetaEdgeColorShortest(const CCategory & cat) const;
	void SetMetaEdgeWidth(const CCategory & cat, unsigned short width);
	unsigned short GetMetaEdgeWidth(const CCategory & cat) const;
	void SetMetaEdgeHide(const CCategory & cat, bool hide);
	bool GetMetaEdgeHide(const CCategory & cat) const;
	void SetMetaEdgeXmlData(const CCategory & cat, const TCHAR* catCode);
	const TCHAR* GetMetaEdgeXmlCatCode(const CCategory & cat, std::_tstring & retVal) const;
	CCategory GetMetaEdgeXmlCatLookup(const TCHAR* lookup) const;
	void SetMetaPluginName(const GUID& pluginId, const TCHAR* name);
	const TCHAR* GetMetaPluginName(const GUID& pluginId, std::_tstring & retVal) const;

	void SetMetaEdgeLabel(const CCategory & cat, const TCHAR* label);
	const TCHAR* GetMetaEdgeLabel(const CCategory & cat, std::_tstring & label) const;
	void SetMetaEdgeFontColor(const CCategory & cat, unsigned long color);
	unsigned long GetMetaEdgeFontColor(const CCategory & cat) const;

	void SetEdgeImageInfo(const CCategory& cat,	const ImageInfo& iinfo, unsigned int overlay);
	const ImageInfoMaps* GetEdgeImageInfo(const CCategory& cat) const;	

	void SetEdgeImageVisible(const CCategory & cat, IMAGE_POSITION pos, VISIBLE visibility);
	VISIBLE GetEdgeImageVisible(const CCategory & cat, IMAGE_POSITION pos) const;

	//Subgraph

	void SetMetaSubgraphLabel(const CCategory & cat, const TCHAR* label);	//What the category means ("Person", "Company" etc.)
	const TCHAR* GetMetaSubgraphLabel(const CCategory & cat, std::_tstring & label) const;	
	void SetMetaSubgraphColorFillNorm(const CCategory & cat, unsigned long color);
	unsigned long GetMetaSubgraphColorFillNorm(const CCategory & cat) const;
	void SetMetaSubgraphColorBorderNorm(const CCategory & cat, unsigned long color);
	unsigned long GetMetaSubgraphColorBorderNorm(const CCategory & cat) const;
	void SetMetaSubgraphColorFillSel(const CCategory & cat, unsigned long color);
	unsigned long GetMetaSubgraphColorFillSel(const CCategory & cat) const;
	void SetMetaSubgraphColorBorderSel(const CCategory & cat, unsigned long color);
	unsigned long GetMetaSubgraphColorBorderSel(const CCategory & cat) const;
	void SetMetaSubgraphColorFillOver(const CCategory & cat, unsigned long color);
	unsigned long GetMetaSubgraphColorFillOver(const CCategory & cat) const;
	void SetMetaSubgraphColorBorderOver(const CCategory & cat, unsigned long color);
	unsigned long GetMetaSubgraphColorBorderOver(const CCategory & cat) const;
	void SetMetaSubgraphFontColor(const CCategory & cat, unsigned long color);
	unsigned long GetMetaSubgraphFontColor(const CCategory & cat) const;
	void SetMetaSubgraphFontColorSel(const CCategory & cat, unsigned long color);
	unsigned long GetMetaSubgraphFontColorSel(const CCategory & cat) const;

	void SetMetaSubgraphHide(const CCategory & cat, bool hide);
	bool GetMetaSubgraphHide(const CCategory & cat) const;

	void SetSubgraphImageInfo(const CCategory& cat, const ImageInfo& iinfo, unsigned int overlay);
	const ImageInfoMaps* GetSubgraphImageInfo(const CCategory& cat) const;
	void SetSubgraphImageVisible(const CCategory & cat, IMAGE_POSITION pos, VISIBLE visibility);
	VISIBLE GetSubgraphImageVisible(const CCategory & cat, IMAGE_POSITION pos) const;

	const TCHAR* GetMetaSubgraphXmlCatCode(const CCategory & cat, std::_tstring & retVal) const;
	void SetMetaSubgraphXmlCatCode(const CCategory & cat, const TCHAR* value);
	const TCHAR* GetMetaSubgraphXmlAnbIconFile(const CCategory & cat, std::_tstring & retVal) const;
	void SetMetaSubgraphXmlAnbIconFile(const CCategory & cat, const TCHAR* value);
	CCategory GetMetaSubgraphXmlCatLookup(const TCHAR* lookup) const;

	std::pair<MetaValueMap::const_iterator, MetaValueMap::const_iterator> GetIterator() const;

	bool IsHidden(IGraphItem * i) const;
	bool IsHidden(IGraphSubgraph * sg) const;
	bool IsHidden(IGraphVertex * v) const;
	bool IsHidden(IGraphEdge * e) const;
	bool IsVisible(IGraphItem * i) const;
	bool IsVisible(IGraphSubgraph * sg) const;
	bool IsVisible(IGraphVertex * v) const;
	bool IsVisible(IGraphEdge * e) const;
	unsigned int GetVisibleUnpinnedVertexCount(IGraph * graph, const CCanvas * canvas) const;

	bool IsImageVisible(IGraphItem * i, IMAGE_POSITION pos) const;
	bool IsImageVisible(IGraphSubgraph * sg, IMAGE_POSITION pos) const;
	bool IsImageVisible(IGraphVertex * v, IMAGE_POSITION pos) const;
	bool IsImageVisible(IGraphEdge * e, IMAGE_POSITION pos) const;

	//  Version 2.0 Properties
	void SetPropertyAttribute(META_TYPE entityType, const CCategory& category, const int id, PROPERTY_ATTR attr, const CComVariant& value);
	std::pair<CategoryPropertyMap::const_iterator, CategoryPropertyMap::const_iterator> CMeta::GetCategoryPropertyAttrIterator(META_TYPE entityType, const CCategory& category);
	unsigned int GetPropertyAttributes(IPropAttributeContainer* storage, META_TYPE entityType, const CCategory& category, const int id);
	unsigned int GetPropertyAttributes(IPropAttributeContainer* storage, META_TYPE entityType, const CCategory& category);
	unsigned int GetPropertyAttributeId(META_TYPE entityType, const CCategory& category, const PROPERTY_ATTR& attr, const CComVariant& value, bool autoGen=false);
};

class MetaSkipper : public ISkipItem, public ISkipSubgraph, public ISkipVertex, public ISkipEdge, public CUnknown
{
protected:
	 const CMeta & m_meta;

public:
	IMPLEMENT_CUNKNOWN;
	MetaSkipper(const CMeta & meta) : m_meta(meta)
	{
	}

	bool SkipSubgraph(IGraphSubgraph * /*sg*/)
	{
		ATLASSERT(!"Abstract Base Class");
		return true;
	}

	bool SkipVertex(IGraphVertex * /*v*/)
	{
		ATLASSERT(!"Abstract Base Class");
		return true;
	}
	virtual bool SkipEdge(IGraphEdge * /*e*/)
	{
		ATLASSERT(!"Abstract Base Class");
		return true;
	}
	virtual bool SkipItem(IGraphItem * /*i*/)
	{
		ATLASSERT(!"Abstract Base Class");
		return true;
	}
};

class MetaVisibleSkipper : public MetaSkipper
{
protected:

public:
	MetaVisibleSkipper(const CMeta & meta) : MetaSkipper(meta)
	{
	}

	virtual bool SkipItem(IGraphItem * i) const
	{
		return m_meta.IsHidden(i);
	}

	virtual bool SkipSubgraph(IGraphSubgraph * sg) const
	{
		return m_meta.IsHidden(sg);
	}

	virtual bool SkipVertex(IGraphVertex * v) const
	{
		return m_meta.IsHidden(v);
	}

	virtual bool SkipEdge(IGraphEdge * e) const
	{
		return m_meta.IsHidden(e);
	}
};

class CCanvas;

class MetaStateSkipper : public MetaSkipper
{
protected:
	const CCanvas * m_canvas;
	TRISTATE m_visible;
	TRISTATE m_pinned;
	TRISTATE m_selected;

public:
	MetaStateSkipper(const CMeta & meta, const CCanvas * canvas, TRISTATE visible=TRISTATE_BOTH, TRISTATE pinned=TRISTATE_BOTH, TRISTATE selected=TRISTATE_BOTH);
	bool SkipSubgraph(IGraphSubgraph * sg) const;
	bool SkipVertex(IGraphVertex * v) const;
	bool SkipEdge(IGraphEdge * e) const;
	bool SkipItem(IGraphItem * i) const;
};
#endif

