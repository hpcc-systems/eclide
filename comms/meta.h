#pragma once

#include "UniqueID.h"
#include "GraphTypes.h"

//  Meta Type
enum META_TYPE
{
	META_UNKNOWN = 0,
	META_CATEGORY,
	META_VERTEX,
	META_EDGE,
	META_SUBGRAPH,
	META_LAST
};

//  Meta Category
enum META_CAT
{
	//  Start of ECL Defines (these need to be in sync with ECL Defines)//
	DESCRIPTOR = 0,														//
	BORDER_COLOR,														//
	FILL_COLOR,															//
	SEL_BORDER_COLOR,													//
	SEL_FILL_COLOR,														//
	OVER_BORDER_COLOR,													//
	OVER_FILL_COLOR,													//
	FONT_COLOR,															//
	EDGE_COLOR,															//
	EDGE_HOT_COLOR,														//
	EDGE_SHORTEST_PATH_COLOR,											//
	LABEL_PLACEMENT,													//
	HIDE,																//
	XML_CATEGORY_CODE,													//
	XML_CATEGORY_LOOKUP,												//
	XML_ANBICONFILE,													//
	IMAGE_VISIBLE,														//
	IMAGE_PLACEMENT,													//
	IMAGE_SIZE,															//
	PROPERTY_NAME,														//
	IUNKNOWN,															//
	WIDTH,																//
	SPLINES,															//
	LABEL,																//
	SEL_FONT_COLOR,														//
	//  End of ECL Defines (you can muck around with stuff below this)  //
	TRANSPARENCY, // or alpha value 0-255
	PLUGIN_NAME,
	DOT_LEFTRIGHT,
	AVOID_OBSTACLES,
	LAST
};

enum VERTEX_COLUMNS
{
	VERTEX_CAT = 0,
	VERTEX_ID,
	VERTEX_USER
};

enum EDGE_COLUMNS
{
	EDGE_CAT = 0,
	EDGE_FROM_CAT,
	EDGE_FROM_ID,
	EDGE_TO_CAT,
	EDGE_TO_ID,
	EDGE_USER
};

#ifndef EXCLUDE_BOOST
typedef boost::tuples::tuple<META_TYPE, META_CAT, CCategory> MetaTuple;
typedef boost::tuples::tuple<META_TYPE, META_CAT, int> MetaTupleInt;
typedef std::map<MetaTuple, _variant_t> MetaValueMap;
typedef std::map<MetaTupleInt, int> MetaIntMap;
typedef std::map<MetaTuple, CAdapt<CComPtr<IPicture> > > MetaPictureMap;
typedef std::map<MetaTuple, HTMLElement> MetaHTMLElementMap;
typedef std::map<MetaTuple, CAdapt<CComPtr<IUnknown> > > MetaUnknownMap;
typedef std::map<MetaTuple, GraphTypes::SizeF> MetaSizeFMap;
typedef std::map<MetaTuple, VISIBLE> MetaVisibleMap;
//TODO: this needs to be fixed.
typedef std::map<MetaTuple, VisibleVector> MetaVisibleImgMap;

typedef std::map<MetaTuple, ImageInfoMaps> MetaImageInfoMap;
#endif 


