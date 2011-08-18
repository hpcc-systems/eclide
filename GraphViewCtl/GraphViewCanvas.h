#pragma once

#include "GraphViewContainer.h"
#include "GraphViewCtlApi.h"
#include "Canvas.h"
#include "Notifications.h"
#include "FruchtermanReingold.h"
#include "XmlAnbSerializer.h"
#include "XgmmlSaxHandler.h"
#include "..\cpptooltip\PPTooltip.h"
#include <CustomMessages.h>

enum UM
{
	UM_FIRST = CWM_LAST + 1,
	UM_LAYOUT_FINISHED,
	UM_TOOLTIP_LINKCLICKED,
	UM_LAST
};

#define MSG_SGV_GETINTERFACE(func) \
	if (uMsg == SGV::SGV_GETINTERFACE) \
	{ \
		SetMsgHandled(TRUE); \
		lResult = (int)func(); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_CLEAR(func) \
	if (uMsg == SGV::SGV_CLEAR) \
	{ \
		SetMsgHandled(TRUE); \
		func(); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_CLEAREDGES(func) \
	if (uMsg == SGV::SGV_CLEAREDGES) \
	{ \
		SetMsgHandled(TRUE); \
		func(); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETLAYOUT(func) \
	if (uMsg == SGV::SGV_SETLAYOUT) \
	{ \
		SetMsgHandled(TRUE); \
		func((SGV::LAYOUT)wParam); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETLAYOUT(func) \
	if (uMsg == SGV::SGV_GETLAYOUT) \
	{ \
		SetMsgHandled(TRUE); \
		lResult = (LRESULT)func(); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETOWNER(func) \
	if (uMsg == SGV::SGV_SETOWNER) \
	{ \
		SetMsgHandled(TRUE); \
		func(reinterpret_cast<SGV::IGraphViewSlot*>(lParam)); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETPROPERTY(func) \
	if (uMsg == SGV::SGV_SETPROPERTY) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT data = (SGV::PSGV_UNIQUEIDSTRUCT)wParam; \
		SGV::PSGV_KEYVAL keyval = (SGV::PSGV_KEYVAL)lParam; \
		func(CUniqueID(data->category.plugin, data->category.category, data->id), keyval->key, keyval->val); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETPROPERTYUNKNOWN(func) \
	if (uMsg == SGV::SGV_GETPROPERTYUNKNOWN) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT data = (SGV::PSGV_UNIQUEIDSTRUCT)wParam; \
		CComPtr<IUnknown> retVal = func(CUniqueID(data->category.plugin, data->category.category, data->id), lParam); \
		lResult = (LRESULT)retVal.p; \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETPROPERTY(func) \
	if (uMsg == SGV::SGV_GETPROPERTY) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT data = (SGV::PSGV_UNIQUEIDSTRUCT)wParam; \
		SGV::PSGV_KEYVAL keyval = (SGV::PSGV_KEYVAL)lParam; \
		func(CUniqueID(data->category.plugin, data->category.category, data->id), keyval->key, keyval->val); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETPROPERTIES(func) \
	if (uMsg == SGV::SGV_GETPROPERTIES) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT data = (SGV::PSGV_UNIQUEIDSTRUCT)lParam; \
		func((IPropertyIDContainer*)wParam, CUniqueID(data->category.plugin, data->category.category, data->id)); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETMOUSEMODE(func) \
	if (uMsg == SGV::SGV_SETMOUSEMODE) \
	{ \
		SetMsgHandled(TRUE); \
		func(static_cast<MOUSE_MODES>(wParam)); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETMOUSEMODE(func) \
	if (uMsg == SGV::SGV_GETMOUSEMODE) \
	{ \
		SetMsgHandled(TRUE); \
		lResult = (LRESULT) func(); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_ADDVERTEX(func) \
	if (uMsg == SGV::SGV_ADDVERTEX) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_VERTEXSTRUCT p1 = (SGV::PSGV_VERTEXSTRUCT)wParam; \
		CComPtr<IGraphVertex> v;\
		bool created; \
		boost::tie(v, created) = func(CUniqueID(p1->id.category.plugin,p1->id.category.category,p1->id.id), (IGraphSubgraph*)p1->Subgraph, (RendererTypes)p1->rType); \
		lResult = created;\
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_ADDEDGE(func) \
	if (uMsg == SGV::SGV_ADDEDGE) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_EDGESTRUCT p1 = (SGV::PSGV_EDGESTRUCT)wParam; \
		lResult = (LRESULT)(func(CUniqueID(p1->id.category.plugin,p1->id.category.category,p1->id.id), CUniqueID(p1->idFrom.category.plugin,p1->idFrom.category.category,p1->idFrom.id),CUniqueID(p1->idTo.category.plugin,p1->idTo.category.category,p1->idTo.id))); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETEDGES(func) \
	if (uMsg == SGV::SGV_GETEDGES) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_GRAPHITEMSTATESTRUCT data = (SGV::PSGV_GRAPHITEMSTATESTRUCT)lParam; \
		func((IUniqueIDContainer *)wParam, data->visible, data->selected, data->includeHidden); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETADJACENTEDGES(func) \
	if (uMsg == SGV::SGV_GETADJACENTEDGES) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT p1 = (SGV::PSGV_UNIQUEIDSTRUCT)lParam; \
		func((IUniqueIDContainer *)wParam, CUniqueID(CCategory(p1->category.plugin, p1->category.category), p1->id)); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETEDGE(func) \
	if (uMsg == SGV::SGV_GETEDGE) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT p1 = (SGV::PSGV_UNIQUEIDSTRUCT)wParam; \
		lResult = (LRESULT)(func(CUniqueID(CCategory(p1->category.plugin, p1->category.category), p1->id))); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_ADDRANDOM(func) \
	if (uMsg == SGV::SGV_ADDRANDOM) \
	{ \
		SetMsgHandled(TRUE); \
		func(LOWORD(lParam),HIWORD(lParam)); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_ADDRANDOMTIMES(func) \
	if (uMsg == SGV::SGV_ADDRANDOMTIMES) \
	{ \
		SetMsgHandled(TRUE); \
		func(); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_ADDSUBGRAPH(func) \
	if (uMsg == SGV::SGV_ADDSUBGRAPH) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_SUBGRAPHSTRUCT p1 = (SGV::PSGV_SUBGRAPHSTRUCT)wParam; \
		lResult = (LRESULT)(func(CUniqueID(p1->id.category.plugin,p1->id.category.category,p1->id.id), p1->label, p1->Subgraph,  p1->rType)); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_LOADXGMML(func) \
	if (uMsg == SGV::SGV_LOADXGMML) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_XGMMLSTRUCT p1 = (SGV::PSGV_XGMMLSTRUCT)wParam; \
		func(p1->xgmml, p1->state, p1->redundantSubgraphs, p1->passthroughVertices, p1->minimizeIfLarge, p1->callback); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_MERGEXGMML(func) \
	if (uMsg == SGV::SGV_MERGEXGMML) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_XGMMLSTRUCT p1 = (SGV::PSGV_XGMMLSTRUCT)wParam; \
		func(p1->xgmml, p1->state, p1->appendMissing, p1->redundantSubgraphs, p1->passthroughVertices, p1->callback); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_HASITEMS(func) \
	if (uMsg == SGV::SGV_HASITEMS) \
	{ \
		SetMsgHandled(TRUE); \
		bool * p1 = (bool *)wParam; \
		*p1 = func(); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_LOADXMLFILE(func) \
	if (uMsg == SGV::SGV_LOADXMLFILE) \
	{ \
		SetMsgHandled(TRUE); \
		func(); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_LOADXML(func) \
	if (uMsg == SGV::SGV_LOADXML) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_LOADXMLSTRUCT p1 = (SGV::PSGV_LOADXMLSTRUCT)wParam; \
		func(p1->xml, *(CPoint*)lParam, p1->version); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETSUBGRAPH(func) \
	if (uMsg == SGV::SGV_GETSUBGRAPH) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT p1 = (SGV::PSGV_UNIQUEIDSTRUCT)wParam; \
		lResult = (LRESULT)(func(CUniqueID(p1->category.plugin,p1->category.category,p1->id))); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETVERTEX(func) \
	if (uMsg == SGV::SGV_GETVERTEX) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT p1 = (SGV::PSGV_UNIQUEIDSTRUCT)wParam; \
		lResult = (LRESULT)(func(CUniqueID(p1->category.plugin,p1->category.category,p1->id))); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETGRAPHITEM(func) \
	if (uMsg == SGV::SGV_GETGRAPHITEM) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT p1 = (SGV::PSGV_UNIQUEIDSTRUCT)wParam; \
		lResult = (LRESULT)(func(CUniqueID(p1->category.plugin,p1->category.category,p1->id))); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETVERTEXCOUNT(func) \
	if (uMsg == SGV::SGV_GETVERTEXCOUNT) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_GRAPHITEMSTATESTRUCT data = (SGV::PSGV_GRAPHITEMSTATESTRUCT)wParam; \
		lResult = (LRESULT)(func(data->visible, data->pinned, data->selected)); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETTARGETBYCOUNT(func) \
	if (uMsg == SGV::SGV_SETTARGETBYCOUNT) \
	{ \
		SetMsgHandled(TRUE); \
		func((IUniqueIDContainer *)wParam, (SGV::PSGV_TARGETSTRUCT)lParam); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETCATEGORIES(func) \
	if (uMsg == SGV::SGV_GETCATEGORIES) \
	{ \
		SetMsgHandled(TRUE); \
		func((META_TYPE)lParam, (ICategoryContainer *)wParam); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETPARENT(func) \
	if (uMsg == SGV::SGV_GETPARENT) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT pid = (SGV::PSGV_UNIQUEIDSTRUCT)wParam; \
		SGV::PSGV_UNIQUEIDSTRUCT pResultID = (SGV::PSGV_UNIQUEIDSTRUCT)lParam; \
		CUniqueID parent; \
		func(CUniqueID(pid->category.plugin,pid->category.category,pid->id), parent); \
		pResultID->category.plugin = parent.GetCategory().GetPlugin().GetGUID(); \
		pResultID->category.category = parent.GetCategory().GetCategory(); \
		pResultID->id = parent.GetID(); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETSUBGRAPHS(func) \
	if (uMsg == SGV::SGV_GETSUBGRAPHS) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_GRAPHITEMSTATESTRUCT data = (SGV::PSGV_GRAPHITEMSTATESTRUCT)lParam; \
		func((IUniqueIDContainer *)wParam, data->visible, data->pinned, data->selected, data->includeHidden); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETCHILDREN(func) \
	if (uMsg == SGV::SGV_GETCHILDREN) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT pid = (SGV::PSGV_UNIQUEIDSTRUCT)wParam; \
		SGV::PSGV_GRAPHITEMIDANDSTATESTRUCT data = (SGV::PSGV_GRAPHITEMIDANDSTATESTRUCT)lParam; \
		lResult=(LRESULT)func(	\
			CUniqueID(pid->category.plugin,pid->category.category,pid->id), \
			data->results, data->visible, data->pinned, data->selected); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETVERTICES(func) \
	if (uMsg == SGV::SGV_GETVERTICES) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_GRAPHITEMSTATESTRUCT data = (SGV::PSGV_GRAPHITEMSTATESTRUCT)lParam; \
		func((IUniqueIDContainer *)wParam, data->visible, data->pinned, data->selected, data->includeHidden); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETADJACENTVERTICES(func) \
	if (uMsg == SGV::SGV_GETADJACENTVERTICES) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT id = (SGV::PSGV_UNIQUEIDSTRUCT)lParam; \
		SGV::PSGV_ADJACENTVERTICESSTRUCT data = (SGV::PSGV_ADJACENTVERTICESSTRUCT)wParam; \
		func((IUniqueIDContainer *)data->results, CUniqueID(id->category.plugin,id->category.category,id->id), data->in, data->out); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETVERTEXNEIGHBOURCOUNT(func) \
	if (uMsg == SGV::SGV_GETVERTEXNEIGHBOURCOUNT) \
	{ \
		SetMsgHandled(TRUE); \
		lResult = (LRESULT) (func((IGraphVertex*)lParam,(SGV::PSGV_TARGETSTRUCT)wParam)); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETVERTEXNEIGHBOUR(func) \
	if (uMsg == SGV::SGV_SETVERTEXNEIGHBOUR) \
	{ \
		SetMsgHandled(TRUE); \
		func((IGraphVertex*)lParam,(SGV::PSGV_TARGETSTRUCT)wParam); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETEDGECOUNT(func) \
	if (uMsg == SGV::SGV_GETEDGECOUNT) \
	{ \
		SetMsgHandled(TRUE); \
		lResult = (LRESULT) (func()); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETINEDGES(func) \
	if (uMsg == SGV::SGV_GETINEDGES) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT p1 = (SGV::PSGV_UNIQUEIDSTRUCT)lParam; \
		func(CUniqueID(CCategory(p1->category.plugin, p1->category.category), p1->id), (IUniqueIDContainer *)wParam); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETOUTEDGES(func) \
	if (uMsg == SGV::SGV_GETOUTEDGES) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT p1 = (SGV::PSGV_UNIQUEIDSTRUCT)lParam; \
		func(CUniqueID(CCategory(p1->category.plugin, p1->category.category), p1->id), (IUniqueIDContainer *)wParam); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_REMOVEVERTEX(func) \
	if (uMsg == SGV::SGV_REMOVEVERTEX) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT data = (SGV::PSGV_UNIQUEIDSTRUCT)wParam; \
		func(CUniqueID(data->category.plugin, data->category.category, data->id)); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_REMOVEEDGE(func) \
	if (uMsg == SGV::SGV_REMOVEEDGE) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT data = (SGV::PSGV_UNIQUEIDSTRUCT)wParam; \
		func(CUniqueID(data->category.plugin, data->category.category, data->id)); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_REMOVESUBGRAPH(func) \
	if (uMsg == SGV::SGV_REMOVESUBGRAPH) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT data = (SGV::PSGV_UNIQUEIDSTRUCT)wParam; \
		func(CUniqueID(data->category.plugin, data->category.category, data->id)); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETMETADATA(func) \
	if (uMsg == SGV::SGV_GETMETADATA) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_CATEGORYSTRUCT category = (SGV::PSGV_CATEGORYSTRUCT)wParam; \
		SGV::PSGV_METADATASTRUCT data = (SGV::PSGV_METADATASTRUCT)lParam; \
		func(category,data); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETMETADATA(func) \
	if (uMsg == SGV::SGV_SETMETADATA) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_CATEGORYSTRUCT category = (SGV::PSGV_CATEGORYSTRUCT)wParam; \
		SGV::PSGV_METADATASTRUCT data = (SGV::PSGV_METADATASTRUCT)lParam; \
		func(category,data); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETMETADEBUG(func) \
	if (uMsg == SGV::SGV_SETMETADEBUG) \
	{ \
		SetMsgHandled(TRUE); \
		func(lParam != 0); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETMETAVERTEXHTMLELEMENT(func) \
	if (uMsg == SGV::SGV_SETMETAVERTEXHTMLELEMENT) \
	{ \
		SetMsgHandled(TRUE); \
		CCategory& category  = *(CCategory*)wParam;   \
		HTMLElement& element = *(HTMLElement*)lParam; \
		func(category,element); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETMETAEDGELABEL(func) \
	if (uMsg == SGV::SGV_SETMETAEDGELABEL) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_CATEGORYSTRUCT cat = (SGV::PSGV_CATEGORYSTRUCT)wParam; \
		func(CCategory(cat->plugin,cat->category), (const TCHAR *)lParam); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETMETAEDGEWIDTH(func) \
	if (uMsg == SGV::SGV_SETMETAEDGEWIDTH) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_CATEGORYSTRUCT cat = (SGV::PSGV_CATEGORYSTRUCT)wParam; \
		func(CCategory(cat->plugin,cat->category),(unsigned short)lParam); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_CENTERGRAPH(func) \
	if (uMsg == SGV::SGV_CENTERGRAPH) \
	{ \
		SetMsgHandled(TRUE); \
		func(); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_CENTERGRAPHITEM(func) \
	if (uMsg == SGV::SGV_CENTERGRAPHITEM) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT data = (SGV::PSGV_UNIQUEIDSTRUCT)wParam; \
		func(CUniqueID(data->category.plugin, data->category.category, data->id), GraphTypes::REAL(lParam)); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_CENTERFOCUSNODE(func) \
	if (uMsg == SGV::SGV_CENTERFOCUSNODE) \
	{ \
		SetMsgHandled(TRUE); \
		func(); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_KICKIT(func) \
	if (uMsg == SGV::SGV_KICKIT) \
	{ \
		SetMsgHandled(TRUE); \
		func(); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_INVALIDATELAYOUT(func) \
	if (uMsg == SGV::SGV_INVALIDATELAYOUT) \
	{ \
		SetMsgHandled(TRUE); \
		func(); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETSCALE(func) \
	if (uMsg == SGV::SGV_SETSCALE) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_TRANSFORMSTRUCT transform = (SGV::PSGV_TRANSFORMSTRUCT)wParam; \
		func(transform->value.real); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETSCALETOFIT(func) \
	if (uMsg == SGV::SGV_SETSCALETOFIT) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_TRANSFORMSTRUCT transform = (SGV::PSGV_TRANSFORMSTRUCT)wParam; \
		transform->value.real = func(transform->boolean,transform->value.real); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETSCALETOWIDTH(func) \
	if (uMsg == SGV::SGV_SETSCALETOWIDTH) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_TRANSFORMSTRUCT transform = (SGV::PSGV_TRANSFORMSTRUCT)wParam; \
		transform->value.real = func(transform->boolean,transform->value.real, (bool)(lParam!=0)); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETITEMSCALETOFIT(func) \
	if (uMsg == SGV::SGV_SETITEMSCALETOFIT) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT data = (SGV::PSGV_UNIQUEIDSTRUCT)wParam; \
		SGV::PSGV_TRANSFORMSTRUCT transform = (SGV::PSGV_TRANSFORMSTRUCT)lParam; \
		transform->value.real = func(CUniqueID(data->category.plugin, data->category.category, data->id)); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETSCALE(func) \
	if (uMsg == SGV::SGV_GETSCALE) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_TRANSFORMSTRUCT transform = (SGV::PSGV_TRANSFORMSTRUCT)wParam; \
		transform->value.real = func(); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETSPACING(func) \
	if (uMsg == SGV::SGV_SETSPACING) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_TRANSFORMSTRUCT transform = (SGV::PSGV_TRANSFORMSTRUCT)wParam; \
		func(transform->value.real); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETSPACING(func) \
	if (uMsg == SGV::SGV_GETSPACING) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_TRANSFORMSTRUCT transform = (SGV::PSGV_TRANSFORMSTRUCT)wParam; \
		transform->value.real = func(); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETROTATION(func) \
	if (uMsg == SGV::SGV_SETROTATION) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_TRANSFORMSTRUCT transform = (SGV::PSGV_TRANSFORMSTRUCT)wParam; \
		func(transform->value.integer); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETROTATION(func) \
	if (uMsg == SGV::SGV_GETROTATION) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_TRANSFORMSTRUCT transform = (SGV::PSGV_TRANSFORMSTRUCT)wParam; \
		transform->value.integer = func(); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETOFFSET(func) \
	if (uMsg == SGV::SGV_SETOFFSET) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_POINT pt = reinterpret_cast<SGV::PSGV_POINT>(wParam); \
		GraphTypes::PointF point((LONG)pt->x, (LONG)pt->y); \
		func(point); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETOFFSET(func) \
	if (uMsg == SGV::SGV_GETOFFSET) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_POINT pt = reinterpret_cast<SGV::PSGV_POINT>(wParam); \
		GraphTypes::PointF point; \
		func(point); \
		pt->x=static_cast<int>(point.x); \
		pt->y=static_cast<int>(point.y); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETFOCUS(func) \
	if (uMsg == SGV::SGV_GETFOCUS) \
	{ \
		SetMsgHandled(TRUE); \
		CUniqueID id;	\
		LRESULT lResult = (LRESULT)func(id); \
		if(lResult!=0)	\
		{	\
			SGV::PSGV_UNIQUEIDSTRUCT id_struct = (SGV::PSGV_UNIQUEIDSTRUCT)wParam; \
			id_struct->category.plugin = id.GetCategory().GetPlugin().GetGUID();	\
			id_struct->category.category = id.GetCategory().GetCategory();	\
			id_struct->id = id.GetID();	\
		}	\
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETFOCUS(func) \
	if (uMsg == SGV::SGV_SETFOCUS) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT data = (SGV::PSGV_UNIQUEIDSTRUCT)wParam; \
		func(CUniqueID(data->category.plugin, data->category.category, data->id)); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETSELECTED(func) \
	if (uMsg == SGV::SGV_GETSELECTED) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT data = (SGV::PSGV_UNIQUEIDSTRUCT)wParam; \
		lResult = func(CUniqueID(data->category.plugin, data->category.category, data->id)); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETSELECTED(func) \
	if (uMsg == SGV::SGV_SETSELECTED) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT data = (SGV::PSGV_UNIQUEIDSTRUCT)wParam; \
		func(CUniqueID(data->category.plugin, data->category.category, data->id),LOWORD(lParam) != 0, HIWORD(lParam) != 0); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_HASSELECTEDVERTEX(func) \
	if (uMsg == SGV::SGV_HASSELECTEDVERTEX) \
	{ \
		SetMsgHandled(TRUE); \
		lResult = func(); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETBOUNDSBOX(func) \
	if (uMsg == SGV::SGV_GETBOUNDSBOX) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT data = (SGV::PSGV_UNIQUEIDSTRUCT)wParam; \
		CRect rc; \
		lResult = func(CUniqueID(data->category.plugin, data->category.category, data->id), rc); \
		SGV::PSGV_RECT prc=(SGV::PSGV_RECT)lParam; \
		prc->left=rc.left; prc->top=rc.top; prc->right=rc.right; prc->bottom=rc.bottom; \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETBOUNDSBOX(func) \
	if (uMsg == SGV::SGV_SETBOUNDSBOX) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT data = (SGV::PSGV_UNIQUEIDSTRUCT)wParam; \
		SGV::PSGV_RECT prc=(SGV::PSGV_RECT)lParam; \
		CRect rc(prc->left, prc->top, prc->right, prc->bottom); \
		func(CUniqueID(data->category.plugin, data->category.category, data->id), rc); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_RECALCDISPSIZE(func) \
	if (uMsg == SGV::SGV_RECALCDISPSIZE) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT data = (SGV::PSGV_UNIQUEIDSTRUCT)wParam; \
		func(CUniqueID(data->category.plugin, data->category.category, data->id)); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}


#define MSG_SGV_GETWINDOWSTATE(func) \
	if (uMsg == SGV::SGV_GETWINDOWSTATE) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT data = (SGV::PSGV_UNIQUEIDSTRUCT)wParam; \
		IListWindowRenderer::WINDOW_SIZE size; int topindex; \
		lResult = func(CUniqueID(data->category.plugin, data->category.category, data->id), size, topindex); \
		SGV::PSGV_WNDSTATE state=(SGV::PSGV_WNDSTATE)lParam; \
		state->size=static_cast<int>(size); state->topindex=topindex; \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETWINDOWSTATE(func) \
	if (uMsg == SGV::SGV_SETWINDOWSTATE) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT data = (SGV::PSGV_UNIQUEIDSTRUCT)wParam; \
		SGV::PSGV_WNDSTATE state=(SGV::PSGV_WNDSTATE)lParam; \
		IListWindowRenderer::WINDOW_SIZE size=static_cast<IListWindowRenderer::WINDOW_SIZE>(state->size); \
		func(CUniqueID(data->category.plugin, data->category.category, data->id), size, state->topindex); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETWINDOWSIZE(func) \
	if (uMsg == SGV::SGV_GETWINDOWSIZE) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT data = (SGV::PSGV_UNIQUEIDSTRUCT)wParam; \
		CSize size; \
		lResult = func(CUniqueID(data->category.plugin, data->category.category, data->id), size); \
		SGV::PSGV_SIZE wndsize=(SGV::PSGV_SIZE)lParam; \
		wndsize->width=size.cx;  wndsize->height=size.cy; \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETWINDOWSIZE(func) \
	if (uMsg == SGV::SGV_SETWINDOWSIZE) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT data = (SGV::PSGV_UNIQUEIDSTRUCT)wParam; \
		SGV::PSGV_SIZE wndsize=(SGV::PSGV_SIZE)lParam; \
		CSize size(wndsize->width, wndsize->height); \
		func(CUniqueID(data->category.plugin, data->category.category, data->id), size); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}


#define MSG_SGV_RENDERTOCONTEXT(func) \
	if (uMsg == SGV::SGV_RENDERTOCONTEXT) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_REGIONSTRUCT region = (SGV::PSGV_REGIONSTRUCT)wParam; \
		func((HDC)lParam,region->x1,region->y1,region->x2,region->y2,region->w2,region->h2); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_RENDERTOIMAGE(func) \
	if (uMsg == SGV::SGV_RENDERTOIMAGE) \
	{ \
		SetMsgHandled(TRUE); \
		CImage &image = *(CImage*)lParam; \
		func(image); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_RENDERTOIMAGE1(func) \
	if (uMsg == SGV::SGV_RENDERTOIMAGE1) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_REGIONSTRUCT region = (SGV::PSGV_REGIONSTRUCT)wParam; \
		CImage &image = *(CImage*)lParam; \
		func(image,region->x1,region->y1,region->w1,region->h1); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_RENDERTOBITMAP(func) \
	if (uMsg == SGV::SGV_RENDERTOBITMAP) \
	{ \
		SetMsgHandled(TRUE); \
		CBitmap &bitmap = *(CBitmap*)lParam; \
		func(bitmap); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_RENDERTOBITMAP1(func) \
	if (uMsg == SGV::SGV_RENDERTOBITMAP1) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_REGIONSTRUCT region = (SGV::PSGV_REGIONSTRUCT)wParam; \
		CBitmap &bitmap = *(CBitmap*)lParam; \
		func(bitmap,region->x1,region->y1,region->w1,region->h1); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_RENDERTOCLIPBOARD(func) \
	if (uMsg == SGV::SGV_RENDERTOCLIPBOARD) \
	{ \
		SetMsgHandled(TRUE); \
		lResult = (LRESULT)func((bool)(lParam!=0)); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SAVEAS(func) \
	if (uMsg == SGV::SGV_SAVEAS) \
	{ \
		SetMsgHandled(TRUE); \
		lResult = (LRESULT)func(); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SAVEASNAME(func) \
	if (uMsg == SGV::SGV_SAVEASNAME) \
	{ \
		SetMsgHandled(TRUE); \
		lResult = (LRESULT)func((LPCTSTR)lParam); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SAVETOXMLFILE(func) \
	if (uMsg == SGV::SGV_SAVETOXMLFILE) \
	{ \
		SetMsgHandled(TRUE); \
		func(); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SAVETOXML(func) \
	if (uMsg == SGV::SGV_SAVETOXML) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_XMLSAVESTRUCT p = (SGV::PSGV_XMLSAVESTRUCT)lParam; \
		func(*(reinterpret_cast<std::_tstring*>(wParam)), p->saveFormat, p->relativeSavePos); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETCURSORS(func) \
	if (uMsg == SGV::SGV_SETCURSORS) \
	{ \
		SetMsgHandled(TRUE); \
		func((HCURSOR)wParam,(HCURSOR)lParam); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETRENDEREXTENT(func) \
	if (uMsg == SGV::SGV_GETRENDEREXTENT) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_REGIONSTRUCT region = (SGV::PSGV_REGIONSTRUCT)wParam; \
		GraphTypes::RectF rect = func((HDC)lParam); \
		region->x1 = static_cast<int>(rect.x); \
		region->y1 = static_cast<int>(rect.y); \
		region->w1 = static_cast<int>(rect.Width); \
		region->h1 = static_cast<int>(rect.Height); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_FIND(func) \
	if (uMsg == SGV::SGV_FIND) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_FINDSTRUCT find = (SGV::PSGV_FINDSTRUCT)wParam; \
		func(find->results, find->searchText, find->searchLabel, find->searchProperties, find->caseSensitive, find->visibleOnly, find->wholeWord, find->pluginId, find->categoryId, find->propertyId); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETDRAGDROPNOTIFICATIONS(func) \
	if (uMsg == SGV::SGV_GETDRAGDROPNOTIFICATIONS) \
	{ \
		SetMsgHandled(TRUE); \
		lResult=static_cast<LRESULT>(func()); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETDRAGDROPNOTIFICATIONS(func) \
	if (uMsg == SGV::SGV_SETDRAGDROPNOTIFICATIONS) \
	{ \
		SetMsgHandled(TRUE); \
		func(lParam!=0); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETDATA(func) \
	if (uMsg == SGV::SGV_GETDATA) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT data = (SGV::PSGV_UNIQUEIDSTRUCT)wParam; \
		func((const TCHAR *)lParam, CUniqueID(data->category.plugin, data->category.category, data->id)); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETMETA(func) \
	if (uMsg == SGV::SGV_GETMETA) \
	{ \
		SetMsgHandled(TRUE); \
		func((const TCHAR*)lParam); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETPOSITION(func) \
	if (uMsg == SGV::SGV_SETPOSITION) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT data = (SGV::PSGV_UNIQUEIDSTRUCT)wParam; \
		SGV::PSGV_POINT point = (SGV::PSGV_POINT)lParam; \
		func(CUniqueID(data->category.plugin, data->category.category, data->id), CPoint(int(point->x), int(point->y)), static_cast<GraphTypes::Coordinates>(point->co), point->randomize); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETPOSITION(func) \
	if (uMsg == SGV::SGV_GETPOSITION) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT data = (SGV::PSGV_UNIQUEIDSTRUCT)wParam; \
		func(CUniqueID(data->category.plugin, data->category.category, data->id), reinterpret_cast<CPoint*>(lParam)); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETLABEL(func) \
	if (uMsg == SGV::SGV_GETLABEL) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT data = (SGV::PSGV_UNIQUEIDSTRUCT)wParam; \
		func(CUniqueID(data->category.plugin, data->category.category, data->id), *(reinterpret_cast<std::_tstring*>(lParam))); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETLABEL(func) \
	if (uMsg == SGV::SGV_SETLABEL) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT data = (SGV::PSGV_UNIQUEIDSTRUCT)wParam; \
		func(CUniqueID(data->category.plugin, data->category.category, data->id), (const TCHAR*)lParam); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETZORDER(func) \
	if (uMsg == SGV::SGV_SETZORDER) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT data = (SGV::PSGV_UNIQUEIDSTRUCT)wParam; \
		func(CUniqueID(data->category.plugin, data->category.category, data->id), lParam); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETREDRAW(func) \
	if (uMsg == SGV::SGV_SETREDRAW) \
	{ \
		SetMsgHandled(TRUE); \
		func(wParam == 1); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_EDITGRAPHITEM(func) \
	if (uMsg == SGV::SGV_EDITGRAPHITEM) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT data = (SGV::PSGV_UNIQUEIDSTRUCT)wParam; \
		func(CUniqueID(data->category.plugin, data->category.category, data->id)); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_CREATEIPIC(func) \
	if (uMsg == SGV::SGV_CREATEIPIC) \
	{ \
		SetMsgHandled(TRUE); \
		const TCHAR* str=reinterpret_cast<const TCHAR*>(wParam); \
		lResult=reinterpret_cast<LRESULT>(func(str)); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETVERTEXVISIBLE(func) \
	if (uMsg == SGV::SGV_SETVERTEXVISIBLE) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT uid = reinterpret_cast<SGV::PSGV_UNIQUEIDSTRUCT>(wParam); \
		func(CUniqueID(uid->category.plugin,uid->category.category, uid->id), lParam!=0); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETVERTEXVISIBLE(func) \
	if (uMsg == SGV::SGV_GETVERTEXVISIBLE) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT uid = reinterpret_cast<SGV::PSGV_UNIQUEIDSTRUCT>(wParam); \
		lResult=(LRESULT) func(CUniqueID(uid->category.plugin,uid->category.category, uid->id)); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_ENSUREVISIBLE(func) \
	if (uMsg == SGV::SGV_ENSUREVISIBLE) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT uid = reinterpret_cast<SGV::PSGV_UNIQUEIDSTRUCT>(wParam); \
		lResult = (LRESULT)func(CUniqueID(uid->category.plugin,uid->category.category, uid->id)); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETMETAVERTEXIMAGE(func) \
	if (uMsg == SGV::SGV_SETMETAVERTEXIMAGE) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT id = reinterpret_cast<SGV::PSGV_UNIQUEIDSTRUCT>(wParam); \
		SGV::PSGV_IMAGEINFOSTRUCT iinfo = reinterpret_cast<SGV::PSGV_IMAGEINFOSTRUCT>(lParam); \
		func(CCategory(id->category.plugin,id->category.category), iinfo->imginfo, iinfo->overlay); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETMETAEDGEIMAGE(func) \
	if (uMsg == SGV::SGV_SETMETAEDGEIMAGE) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT id = reinterpret_cast<SGV::PSGV_UNIQUEIDSTRUCT>(wParam); \
		SGV::PSGV_IMAGEINFOSTRUCT iinfo = reinterpret_cast<SGV::PSGV_IMAGEINFOSTRUCT>(lParam); \
		func(CCategory(id->category.plugin,id->category.category), iinfo->imginfo, iinfo->overlay); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETVERTEXIMAGE(func) \
	if (uMsg == SGV::SGV_SETVERTEXIMAGE) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT uid = reinterpret_cast<SGV::PSGV_UNIQUEIDSTRUCT>(wParam); \
		SGV::PSGV_IMAGEINFOSTRUCT iinfo = reinterpret_cast<SGV::PSGV_IMAGEINFOSTRUCT>(lParam); \
		func(CUniqueID(uid->category.plugin,uid->category.category, uid->id), iinfo->imginfo, iinfo->overlay); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETVERTEXIMAGE(func) \
	if (uMsg == SGV::SGV_GETVERTEXIMAGE) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT uid = reinterpret_cast<SGV::PSGV_UNIQUEIDSTRUCT>(wParam); \
		SGV::PSGV_IMAGEVISSTRUCT imgvis = reinterpret_cast<SGV::PSGV_IMAGEVISSTRUCT>(lParam); \
		lResult = reinterpret_cast<LRESULT>(func(CUniqueID(uid->category.plugin,uid->category.category, uid->id), imgvis->pos, imgvis->overlay, imgvis->itemSpecific)); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETEDGEIMAGE(func) \
	if (uMsg == SGV::SGV_SETEDGEIMAGE) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT uid = reinterpret_cast<SGV::PSGV_UNIQUEIDSTRUCT>(wParam); \
		SGV::PSGV_IMAGEINFOSTRUCT iinfo = reinterpret_cast<SGV::PSGV_IMAGEINFOSTRUCT>(lParam); \
		func(CUniqueID(uid->category.plugin,uid->category.category, uid->id), iinfo->imginfo, iinfo->overlay); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETEDGEIMAGE(func) \
	if (uMsg == SGV::SGV_GETEDGEIMAGE) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT uid = reinterpret_cast<SGV::PSGV_UNIQUEIDSTRUCT>(wParam); \
		SGV::PSGV_IMAGEVISSTRUCT imgvis = reinterpret_cast<SGV::PSGV_IMAGEVISSTRUCT>(lParam); \
		lResult = reinterpret_cast<LRESULT>(func(CUniqueID(uid->category.plugin,uid->category.category, uid->id), imgvis->pos, imgvis->overlay, imgvis->itemSpecific)); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETVERTEXIMAGESVISIBLE(func) \
	if (uMsg == SGV::SGV_GETVERTEXIMAGESVISIBLE) \
	{ \
		SetMsgHandled(TRUE); \
		lResult = (LRESULT) func(); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETVERTEXIMAGESVISIBLE(func) \
	if (uMsg == SGV::SGV_SETVERTEXIMAGESVISIBLE) \
	{ \
		SetMsgHandled(TRUE); \
		func(wParam != 0); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETVERTEXIMAGEVISIBLE(func) \
	if (uMsg == SGV::SGV_GETVERTEXIMAGEVISIBLE) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT uid = reinterpret_cast<SGV::PSGV_UNIQUEIDSTRUCT>(wParam); \
		IMAGE_POSITION pos = static_cast<IMAGE_POSITION>(lParam); \
		lResult = (LRESULT) func(CUniqueID(uid->category.plugin,uid->category.category, uid->id), pos); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETVERTEXIMAGEVISIBLE(func) \
	if (uMsg == SGV::SGV_SETVERTEXIMAGEVISIBLE) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT uid = reinterpret_cast<SGV::PSGV_UNIQUEIDSTRUCT>(wParam); \
		SGV::PSGV_IMAGEVISSTRUCT imgvis = reinterpret_cast<SGV::PSGV_IMAGEVISSTRUCT>(lParam); \
		func(CUniqueID(uid->category.plugin,uid->category.category, uid->id), imgvis->pos, imgvis->vis==VISIBLE_SHOW); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETMETAVERTEXIMAGEVISIBLE(func) \
	if (uMsg == SGV::SGV_GETMETAVERTEXIMAGEVISIBLE) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT uid = reinterpret_cast<SGV::PSGV_UNIQUEIDSTRUCT>(wParam); \
		SGV::PSGV_IMAGEVISSTRUCT imgvis = reinterpret_cast<SGV::PSGV_IMAGEVISSTRUCT>(lParam); \
		lResult= (LRESULT)func(CCategory(uid->category.plugin,uid->category.category), imgvis->pos); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETMETAVERTEXIMAGEVISIBLE(func) \
	if (uMsg == SGV::SGV_SETMETAVERTEXIMAGEVISIBLE) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT uid = reinterpret_cast<SGV::PSGV_UNIQUEIDSTRUCT>(wParam); \
		SGV::PSGV_IMAGEVISSTRUCT imgvis = reinterpret_cast<SGV::PSGV_IMAGEVISSTRUCT>(lParam); \
		func(CCategory(uid->category.plugin,uid->category.category), imgvis->pos, imgvis->vis==VISIBLE_SHOW); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}


#define MSG_SGV_SETEDGEVISIBLE(func) \
	if (uMsg == SGV::SGV_SETEDGEVISIBLE) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT uid = reinterpret_cast<SGV::PSGV_UNIQUEIDSTRUCT>(wParam); \
		func(CUniqueID(uid->category.plugin,uid->category.category, uid->id), lParam!=0); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETEDGEVISIBLE(func) \
	if (uMsg == SGV::SGV_GETEDGEVISIBLE) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT uid = reinterpret_cast<SGV::PSGV_UNIQUEIDSTRUCT>(wParam); \
		lResult=(LRESULT) func(CUniqueID(uid->category.plugin,uid->category.category, uid->id)); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}


#define MSG_SGV_GETEDGEIMAGEVISIBLE(func) \
	if (uMsg == SGV::SGV_GETEDGEIMAGEVISIBLE) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT uid = reinterpret_cast<SGV::PSGV_UNIQUEIDSTRUCT>(wParam); \
		IMAGE_POSITION pos=static_cast<IMAGE_POSITION>(lParam); \
		lResult = (LRESULT) func(CUniqueID(uid->category.plugin,uid->category.category, uid->id), pos); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETEDGEIMAGEVISIBLE(func) \
	if (uMsg == SGV::SGV_SETEDGEIMAGEVISIBLE) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT uid = reinterpret_cast<SGV::PSGV_UNIQUEIDSTRUCT>(wParam); \
		SGV::PSGV_IMAGEVISSTRUCT imgvis=reinterpret_cast<SGV::PSGV_IMAGEVISSTRUCT>(lParam); \
		func(CUniqueID(uid->category.plugin,uid->category.category, uid->id), imgvis->pos, imgvis->vis==VISIBLE_SHOW); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETMETAEDGEIMAGEVISIBLE(func) \
	if (uMsg == SGV::SGV_GETMETAEDGEIMAGEVISIBLE) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT uid = reinterpret_cast<SGV::PSGV_UNIQUEIDSTRUCT>(wParam); \
		SGV::PSGV_IMAGEVISSTRUCT imgvis=reinterpret_cast<SGV::PSGV_IMAGEVISSTRUCT>(lParam); \
		lResult = (LRESULT)func(CCategory(uid->category.plugin,uid->category.category), imgvis->pos); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETMETAEDGEIMAGEVISIBLE(func) \
	if (uMsg == SGV::SGV_SETMETAEDGEIMAGEVISIBLE) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT uid = reinterpret_cast<SGV::PSGV_UNIQUEIDSTRUCT>(wParam); \
		SGV::PSGV_IMAGEVISSTRUCT imgvis=reinterpret_cast<SGV::PSGV_IMAGEVISSTRUCT>(lParam); \
		func(CCategory(uid->category.plugin,uid->category.category), imgvis->pos, imgvis->vis==VISIBLE_SHOW); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETEDGESOURCE(func)	\
	if (uMsg == SGV::SGV_GETEDGESOURCE) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT edgeid = reinterpret_cast<SGV::PSGV_UNIQUEIDSTRUCT>(wParam); \
		CUniqueID* retVal = (CUniqueID*)lParam;	\
		lResult=(LRESULT)func(CUniqueID(edgeid->category.plugin,edgeid->category.category, edgeid->id), *retVal);	\
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETEDGETARGET(func)	\
	if (uMsg == SGV::SGV_GETEDGETARGET) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT edgeid = reinterpret_cast<SGV::PSGV_UNIQUEIDSTRUCT>(wParam); \
		CUniqueID* retVal = (CUniqueID*)lParam;	\
		lResult=(LRESULT)func(CUniqueID(edgeid->category.plugin,edgeid->category.category, edgeid->id), *retVal);	\
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETEDGECOLOR(func) \
	if (uMsg == SGV::SGV_GETEDGECOLOR) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT uid = reinterpret_cast<SGV::PSGV_UNIQUEIDSTRUCT>(wParam); \
		GraphTypes::Color color; \
		func(CUniqueID(uid->category.plugin,uid->category.category, uid->id), color); \
		SGV::PSGV_COLOR sgvcolor=reinterpret_cast<SGV::PSGV_COLOR>(lParam); \
		sgvcolor->A=color.GetA(); \
		sgvcolor->R=color.GetR(); \
		sgvcolor->G=color.GetG(); \
		sgvcolor->B=color.GetB(); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETEDGECOLOR(func) \
	if (uMsg == SGV::SGV_SETEDGECOLOR) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT uid = reinterpret_cast<SGV::PSGV_UNIQUEIDSTRUCT>(wParam); \
		SGV::PSGV_COLOR sgvcolor=reinterpret_cast<SGV::PSGV_COLOR>(lParam); \
		GraphTypes::Color color(sgvcolor->A, sgvcolor->R, sgvcolor->G, sgvcolor->B); \
		func(CUniqueID(uid->category.plugin,uid->category.category, uid->id), color); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETEDGEDATERANGE(func) \
	if (uMsg == SGV::SGV_SETEDGEDATERANGE) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT uid = reinterpret_cast<SGV::PSGV_UNIQUEIDSTRUCT>(wParam); \
		SGV::PSGV_DATERANGE dateRange=reinterpret_cast<SGV::PSGV_DATERANGE>(lParam); \
		func(CUniqueID(uid->category.plugin,uid->category.category, uid->id), dateRange->from, dateRange->to); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETAPPMODE(func) \
	if (uMsg == SGV::SGV_SETAPPMODE) \
	{ \
		SetMsgHandled(TRUE); \
		func(static_cast<AppMode>(wParam)); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETAPPMODE(func) \
	if (uMsg == SGV::SGV_GETAPPMODE) \
	{ \
		SetMsgHandled(TRUE); \
		lResult=(LRESULT)func(); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETAPPVISMODE(func) \
	if (uMsg == SGV::SGV_SETAPPVISMODE) \
	{ \
		SetMsgHandled(TRUE); \
		func(static_cast<AppVisMode>(wParam)); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETAPPVISMODE(func) \
	if (uMsg == SGV::SGV_GETAPPVISMODE) \
	{ \
		SetMsgHandled(TRUE); \
		lResult=(LRESULT)func(); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETAPPZOOMMODE(func) \
	if (uMsg == SGV::SGV_SETAPPZOOMMODE) \
	{ \
		SetMsgHandled(TRUE); \
		func(static_cast<AppZoomMode>(wParam)); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETAPPZOOMMODE(func) \
	if (uMsg == SGV::SGV_GETAPPZOOMMODE) \
	{ \
		SetMsgHandled(TRUE); \
		lResult=(LRESULT)func(); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_FINDIDDIALOG(func) \
	if (uMsg == SGV::SGV_FINDIDDIALOG) \
	{ \
		SetMsgHandled(TRUE); \
		func(); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETVERTEXPINNED(func) \
	if (uMsg == SGV::SGV_SETVERTEXPINNED) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT uid = reinterpret_cast<SGV::PSGV_UNIQUEIDSTRUCT>(wParam); \
		func(CUniqueID(uid->category.plugin,uid->category.category, uid->id), lParam!=0); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETVERTEXPINNED(func) \
	if (uMsg == SGV::SGV_GETVERTEXPINNED) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT uid = reinterpret_cast<SGV::PSGV_UNIQUEIDSTRUCT>(wParam); \
		lResult=(LRESULT) func(CUniqueID(uid->category.plugin,uid->category.category, uid->id)); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETPROPERTYATTRIBUTE(func) \
	if (uMsg == SGV::SGV_SETPROPERTYATTRIBUTE) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_PROPERTYSTRUCT prop = reinterpret_cast<SGV::PSGV_PROPERTYSTRUCT>(wParam); \
		func(prop->entity, CCategory(prop->cat.plugin,prop->cat.category), prop->propid, prop->attr, prop->value); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETPROPERTYATTRIBUTES(func) \
	if (uMsg == SGV::SGV_GETPROPERTYATTRIBUTES) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_PROPERTYSTRUCT prop = reinterpret_cast<SGV::PSGV_PROPERTYSTRUCT>(wParam); \
		IPropAttributeContainer* ipa = reinterpret_cast<IPropAttributeContainer*>(lParam); \
		lResult=(LRESULT)func(ipa, prop->entity, CCategory(prop->cat.plugin,prop->cat.category), prop->propid); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETPROPERTYATTRIBUTEID(func) \
	if (uMsg == SGV::SGV_GETPROPERTYATTRIBUTEID) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_PROPERTYSTRUCT prop = reinterpret_cast<SGV::PSGV_PROPERTYSTRUCT>(wParam); \
		prop->propid = func(prop->entity, CCategory(prop->cat.plugin,prop->cat.category), prop->attr, prop->value, prop->autoGenId); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETMESSAGE(func) \
	if (uMsg == SGV::SGV_SETMESSAGE) \
	{ \
		SetMsgHandled(TRUE); \
		func((const TCHAR *)wParam); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SHOWOVERVIEW(func) \
	if (uMsg == SGV::SGV_SHOWOVERVIEW) \
	{ \
		SetMsgHandled(TRUE); \
		bool show=wParam!=0; \
		func(show); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETOVERVIEW(func) \
	if (uMsg == SGV::SGV_GETOVERVIEW) \
	{ \
		SetMsgHandled(TRUE); \
		lResult = (LRESULT) func(); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SET_EDGE_LABEL_VISIBLE(func) \
	if (uMsg == SGV::SGV_SET_EDGE_LABEL_VISIBLE) \
	{ \
		SetMsgHandled(TRUE); \
		bool show=wParam!=0; \
		func(show); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GET_EDGE_LABEL_VISIBLE(func) \
	if (uMsg == SGV::SGV_GET_EDGE_LABEL_VISIBLE) \
	{ \
		SetMsgHandled(TRUE); \
		lResult = (LRESULT) func(); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETOVERVIEWOPACITY(func) \
	if (uMsg == SGV::SGV_SETOVERVIEWOPACITY) \
	{ \
		SetMsgHandled(TRUE); \
		func(wParam); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETOVERVIEWOPACITY(func) \
	if (uMsg == SGV::SGV_GETOVERVIEWOPACITY) \
	{ \
		SetMsgHandled(TRUE); \
		lResult = (LRESULT) func(); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETOVERVIEWPOSITION(func) \
	if (uMsg == SGV::SGV_SETOVERVIEWPOSITION) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_POINT point=reinterpret_cast<SGV::PSGV_POINT>(wParam); \
		func(CPoint(int(point->x), int(point->y))); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETOVERVIEWSIZE(func) \
	if (uMsg == SGV::SGV_SETOVERVIEWSIZE) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_SIZE size=reinterpret_cast<SGV::PSGV_SIZE>(wParam); \
		func(CSize(size->width, size->height)); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_EMULATEDECORATION(func) \
	if (uMsg == SGV::SGV_EMULATEDECORATION) \
	{ \
		SetMsgHandled(TRUE); \
		lResult = (LRESULT)func((bool)(wParam!=0)); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_ISEMULATEDECORATION(func) \
	if (uMsg == SGV::SGV_ISEMULATEDECORATION) \
	{ \
		SetMsgHandled(TRUE); \
		lResult = (LRESULT)func(); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETROWCOMPRESS(func) \
	if (uMsg == SGV::SGV_SETROWCOMPRESS) \
	{ \
		SetMsgHandled(TRUE); \
        func(wParam!=0); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETROWCOMPRESS(func) \
	if (uMsg == SGV::SGV_GETROWCOMPRESS) \
	{ \
		SetMsgHandled(TRUE); \
		lResult = (LRESULT) func(); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETREADONLY(func) \
	if (uMsg == SGV::SGV_SETREADONLY) \
	{ \
		SetMsgHandled(TRUE); \
        func(wParam!=0); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_ISREADONLY(func) \
	if (uMsg == SGV::SGV_ISREADONLY) \
	{ \
		SetMsgHandled(TRUE); \
		lResult = (LRESULT) func(); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_MERGEINTOSUBGRAPH(func) \
	if (uMsg == SGV::SGV_MERGEINTOSUBGRAPH) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_MERGEINTOSUBGRAPHSTRUCT p1 = reinterpret_cast<SGV::PSGV_MERGEINTOSUBGRAPHSTRUCT>(wParam); \
		CUniqueID& subgraph=*(CUniqueID*)lParam; \
		func(subgraph, p1->nodes, p1->label); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_BREAKSUBGRAPH(func) \
	if (uMsg == SGV::SGV_BREAKSUBGRAPH) \
	{ \
		SetMsgHandled(TRUE); \
		IUniqueIDContainer* results=reinterpret_cast<IUniqueIDContainer*>(wParam); \
		const CUniqueID& subgraph=*(CUniqueID*)lParam; \
		func(results, subgraph); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}


#define MSG_SGV_SETSUBGRAPHVISIBLE(func) \
	if (uMsg == SGV::SGV_SETSUBGRAPHVISIBLE) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT uid = reinterpret_cast<SGV::PSGV_UNIQUEIDSTRUCT>(wParam); \
		func(CUniqueID(uid->category.plugin,uid->category.category, uid->id), lParam!=0); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETSUBGRAPHVISIBLE(func) \
	if (uMsg == SGV::SGV_GETSUBGRAPHVISIBLE) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT uid = reinterpret_cast<SGV::PSGV_UNIQUEIDSTRUCT>(wParam); \
		lResult=(LRESULT) func(CUniqueID(uid->category.plugin,uid->category.category, uid->id)); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETSUBGRAPHIMAGE(func) \
	if (uMsg == SGV::SGV_SETSUBGRAPHIMAGE) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT uid = reinterpret_cast<SGV::PSGV_UNIQUEIDSTRUCT>(wParam); \
		SGV::PSGV_IMAGEINFOSTRUCT iinfo = reinterpret_cast<SGV::PSGV_IMAGEINFOSTRUCT>(lParam); \
		func(CUniqueID(uid->category.plugin,uid->category.category, uid->id), iinfo->imginfo, iinfo->overlay); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETSUBGRAPHIMAGE(func) \
	if (uMsg == SGV::SGV_GETSUBGRAPHIMAGE) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT uid = reinterpret_cast<SGV::PSGV_UNIQUEIDSTRUCT>(wParam); \
		SGV::PSGV_IMAGEVISSTRUCT imgvis = reinterpret_cast<SGV::PSGV_IMAGEVISSTRUCT>(lParam); \
		lResult = reinterpret_cast<LRESULT>(func(CUniqueID(uid->category.plugin,uid->category.category, uid->id), imgvis->pos, imgvis->overlay, imgvis->itemSpecific)); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETSUBGRAPHIMAGEVISIBLE(func) \
	if (uMsg == SGV::SGV_GETSUBGRAPHIMAGEVISIBLE) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT uid = reinterpret_cast<SGV::PSGV_UNIQUEIDSTRUCT>(wParam); \
		IMAGE_POSITION pos = static_cast<IMAGE_POSITION>(lParam); \
		lResult = (LRESULT) func(CUniqueID(uid->category.plugin,uid->category.category, uid->id), pos); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETSUBGRAPHIMAGEVISIBLE(func) \
	if (uMsg == SGV::SGV_SETSUBGRAPHIMAGEVISIBLE) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT uid = reinterpret_cast<SGV::PSGV_UNIQUEIDSTRUCT>(wParam); \
		SGV::PSGV_IMAGEVISSTRUCT imgvis = reinterpret_cast<SGV::PSGV_IMAGEVISSTRUCT>(lParam); \
		func(CUniqueID(uid->category.plugin,uid->category.category, uid->id), imgvis->pos, imgvis->vis==VISIBLE_SHOW); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}


#define MSG_SGV_SETMETASUBGRAPHIMAGE(func) \
	if (uMsg == SGV::SGV_SETMETASUBGRAPHIMAGE) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT id = reinterpret_cast<SGV::PSGV_UNIQUEIDSTRUCT>(wParam); \
		SGV::PSGV_IMAGEINFOSTRUCT iinfo = reinterpret_cast<SGV::PSGV_IMAGEINFOSTRUCT>(lParam); \
		func(CCategory(id->category.plugin,id->category.category), iinfo->imginfo, iinfo->overlay); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETMETASUBGRAPHIMAGEVISIBLE(func) \
	if (uMsg == SGV::SGV_GETMETASUBGRAPHIMAGEVISIBLE) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT uid = reinterpret_cast<SGV::PSGV_UNIQUEIDSTRUCT>(wParam); \
		SGV::PSGV_IMAGEVISSTRUCT imgvis = reinterpret_cast<SGV::PSGV_IMAGEVISSTRUCT>(lParam); \
		lResult= (LRESULT)func(CCategory(uid->category.plugin,uid->category.category), imgvis->pos); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETMETASUBGRAPHIMAGEVISIBLE(func) \
	if (uMsg == SGV::SGV_SETMETASUBGRAPHIMAGEVISIBLE) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT uid = reinterpret_cast<SGV::PSGV_UNIQUEIDSTRUCT>(wParam); \
		SGV::PSGV_IMAGEVISSTRUCT imgvis = reinterpret_cast<SGV::PSGV_IMAGEVISSTRUCT>(lParam); \
		func(CCategory(uid->category.plugin,uid->category.category), imgvis->pos, imgvis->vis==VISIBLE_SHOW); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETSUBGRAPHPINNED(func) \
	if (uMsg == SGV::SGV_SETSUBGRAPHPINNED) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT uid = reinterpret_cast<SGV::PSGV_UNIQUEIDSTRUCT>(wParam); \
		func(CUniqueID(uid->category.plugin,uid->category.category, uid->id), lParam!=0); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETSUBGRAPHPINNED(func) \
	if (uMsg == SGV::SGV_GETSUBGRAPHPINNED) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT uid = reinterpret_cast<SGV::PSGV_UNIQUEIDSTRUCT>(wParam); \
		lResult=(LRESULT) func(CUniqueID(uid->category.plugin,uid->category.category, uid->id)); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_SETSUBGRAPHMINIMIZED(func) \
	if (uMsg == SGV::SGV_SETSUBGRAPHMINIMIZED) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT uid = reinterpret_cast<SGV::PSGV_UNIQUEIDSTRUCT>(wParam); \
		func(CUniqueID(uid->category.plugin,uid->category.category, uid->id), lParam!=0); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}

#define MSG_SGV_GETSUBGRAPHMINIMIZED(func) \
	if (uMsg == SGV::SGV_GETSUBGRAPHMINIMIZED) \
	{ \
		SetMsgHandled(TRUE); \
		SGV::PSGV_UNIQUEIDSTRUCT uid = reinterpret_cast<SGV::PSGV_UNIQUEIDSTRUCT>(wParam); \
		lResult=(LRESULT) func(CUniqueID(uid->category.plugin,uid->category.category, uid->id)); \
		if(IsMsgHandled()!=0) \
			return TRUE; \
	}
//  ===========================================================================
class CIDropTarget;
class CAggDC;
class CTimelineCtrl;
class COverviewWnd;
class CFilterSubgraphEdges;

__interface SGV::IGraphViewSlot;
//  ===========================================================================
typedef CWinTraits<WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS> CGraphCanvasWinTraits;
class CGraphViewCanvas : 
	public CWindowImpl<CGraphViewCanvas, CWindow, CGraphCanvasWinTraits>, 
	public ISkipEdge,
	public ISkipVertex,
	public ISkipSubgraph,
	public IXGMMLRenderCallback,
	public CUnknown,
	public boost::signals::trackable
{
	typedef CGraphViewCanvas thisclass;
	typedef CWindowImpl<CGraphViewCanvas, CWindow, CGraphCanvasWinTraits> baseClass;
	friend void LayoutThread(CGraphViewCanvas * self);
protected:
	CPPToolTip m_tooltip;
	bool m_randomDataGenerated;

	CComPtr<IGraph> m_graph;
	StlLinked<CFilterSubgraphEdges> m_edgeFilter;
	mutable int m_buildingEdgeVisibility;

	CMeta m_meta;
	CIDropTarget* m_dropTarget; // no CComPtr for IDropTarget because of custom methods

	CComPtr<ILayout> m_layout_spring1;
	CComPtr<ILayout> m_layout_circle;
	CComPtr<ILayout> m_layout_timeline;
	CComPtr<ILayout> m_layout_lee;
	CComPtr<ILayout> m_layout_neato;
	CComPtr<ILayout> m_layout_fdp;
	CComPtr<ILayout> m_layout_twopi;
	CComPtr<ILayout> m_layout_dot;
	CComPtr<ILayout> m_layout_circo;
	CComPtr<ILayout> m_layout_visgedge;
	CComPtr<ILayout> m_layout_subgraph_tree;
	CComPtr<ILayout> m_layout_null;
	CComPtr<ILayout> m_layout;
	SGV::LAYOUT m_lastLayout;
	CComPtr<ILayout> m_stopping_layout;
	CComPtr<ILayout> m_next_layout;

	StlLinked<CCanvas> m_canvas;
	CScrollBar m_hSB, m_vSB, m_sBox;
	GraphTypes::PointF m_scrollOffset;
	bool m_inScrollDrag;
	CTimelineCtrl* m_timeline;
	COverviewWnd* m_overview;

	CComPtr<IVertexPath> m_overToSel;

	bool m_bMouseDown;

	CPoint mouseCapturePos;
	CPoint mouseCurrentPos;

	CXmlAnbSerializer m_xmlSerializerAnb;
	SGV::IGraphViewSlot* m_Owner;

	UINT m_timer;

	ULONG m_NoRedraw;
	GraphTypes::PointFVector m_prevLayout;
	GraphTypes::PointF m_prevCenter;

	bool m_running;
	bool m_breakFlag;
	mutable clib::recursive_mutex m_pulsemutex;
	mutable clib::recursive_mutex m_mutex;
	CComPtr<clib::scoped_lock_ref_counted> m_lock;

	std::_tstring m_message;

	WUVisualState m_visualState;

public:
	BEGIN_CUNKNOWN
		IMPLEMENT_INTERFACE(IXGMMLRenderCallback)
	END_CUNKNOWN(CUnknown);

	CGraphViewCanvas();
	~CGraphViewCanvas();

	CMeta & GetMeta() { return m_meta; }
	IGraph* GetGraph() { return m_graph; }

	const StlLinked<CCanvas>& GetCanvas() { return m_canvas; }
	void AttachTimeline(CTimelineCtrl* tl) { m_timeline=tl; m_canvas->AttachTimeline(tl); }
	void AttachOverview(COverviewWnd* ov) { m_overview=ov; }
	void OnClear();
	void OnClearEdges();
	bool LoadXGMMLHelper(const TCHAR* xgmml, WUVisualState state, bool redundantSubgraphs, bool passthroughVertices, const IXGMMLRenderCallback * callback);
	void LoadXGMML(const TCHAR* xgmml, WUVisualState state, bool redundantSubgraphs, bool passthroughVertices, bool minimizeIfLarge, const IXGMMLRenderCallback * callback);
	void MergeXGMML(const TCHAR* xgmml, WUVisualState state, bool missingAppend, bool redundantSubgraphs, bool passthroughVertices, const IXGMMLRenderCallback * callback);
	bool HasItems();
	void LoadXml(const TCHAR* xml, const CPoint& relativeLoadPos = CPoint(), int version = 1);
	void LoadXmlFile();
	void SetMessage(const std::_tstring & msg);
	void ShowOverview(bool show);
	bool GetOverview() const;
	void SetOverviewOpacity(BYTE opacity);
	BYTE GetOverviewOpacity() const;
	void SetOverviewPosition(const CPoint& pos);
	void SetOverviewSize(const CSize& size); 
	bool EmulateDecoration(bool emulate);
	bool IsEmulateDecoration() const;
	void SetRowCompress(bool compress);
	bool GetRowCompress() const;
	void SetReadOnly(bool ro); 
	bool IsReadOnly() const;  
#ifdef _DEBUG
	void SetGammaThreshold(GraphTypes::REAL thr); 
	GraphTypes::REAL GetGammaThreshold() const;
#endif

	bool IsEdgeLabelVisible() const;
	void SetEdgeLabelVisible(bool visible);
	void CalculateEdgeVisibility();

	void SetOwner(SGV::IGraphViewSlot * owner);
	void SetLayout(SGV::LAYOUT layout);
	SGV::LAYOUT GetLayout();
	void SetRenderer(const CUniqueID & id, RendererTypes r);
	void SetRenderer(RendererTypes r);

	IGraphItem* GetGraphItem(const CUniqueID & id) const;

	void SetProperty(const CUniqueID & id, int key, const CComVariant & val);
	void GetProperty(const CUniqueID & id, int key, CComVariant & result);

	IUnknown * GetPropertyUnknown(const CUniqueID & id, int key);

	void GetProperties(IPropertyIDContainer* results, const CUniqueID & id);

	void SetMouseMode(MOUSE_MODES val);
	MOUSE_MODES GetMouseMode() const;
	GraphTypes::REAL CalcScaleStep();

	void SetMetaData(const SGV::PSGV_CATEGORYSTRUCT, const SGV::PSGV_METADATASTRUCT data);
	void GetMetaData(const SGV::PSGV_CATEGORYSTRUCT, SGV::PSGV_METADATASTRUCT data) const;

	void SetVertexVisible(const CUniqueID& id, bool visible);
	bool GetVertexVisible(const CUniqueID& id);

	bool GetVertexPinned(const CUniqueID& id);
	void SetVertexPinned(const CUniqueID& id, bool pinned);
	
	void SetMetaVertexImage(const CCategory& cat, const ImageInfo& iinfo, unsigned int overlay);
	void SetMetaEdgeImage(const CCategory& cat, const ImageInfo& iinfo, unsigned int overlay);

	void SetVertexImagesVisible(bool visible);
	bool GetVertexImagesVisible() const;

	void SetVertexImage(const CUniqueID& id, const ImageInfo& iinfo, unsigned int overlay);
	GraphTypes::IBitmap* GetVertexImage(const CUniqueID& id, IMAGE_POSITION pos, unsigned int overlay, bool vertexSpecific);

	bool GetVertexImageVisible(const CUniqueID& id, IMAGE_POSITION pos);
	void SetVertexImageVisible(const CUniqueID& id, IMAGE_POSITION pos, bool visible);
	bool GetMetaVertexImageVisible(const CCategory& cat, IMAGE_POSITION pos);
	void SetMetaVertexImageVisible(const CCategory& cat, IMAGE_POSITION pos, bool visible);

	void SetEdgeVisible(const CUniqueID& id, bool visible);
	bool GetEdgeVisible(const CUniqueID& id);

	void SetEdgeImage(const CUniqueID& id, const ImageInfo& iinfo, unsigned int overlay);
	GraphTypes::IBitmap* GetEdgeImage(const CUniqueID& id, IMAGE_POSITION pos, unsigned int overlay, bool edgeSpecific);

	bool GetEdgeImageVisible(const CUniqueID& id, IMAGE_POSITION pos);
	void SetEdgeImageVisible(const CUniqueID& id, IMAGE_POSITION pos, bool visible);
	bool GetMetaEdgeImageVisible(const CCategory& cat, IMAGE_POSITION pos);
	void SetMetaEdgeImageVisible(const CCategory& cat, IMAGE_POSITION pos, bool visible);
	void GetEdgeColor(const CUniqueID& uid, GraphTypes::Color& color) const;
	void SetEdgeColor(const CUniqueID& uid, const GraphTypes::Color& color);
	bool GetEdgeSource(const CUniqueID& uid, CUniqueID& src);
	bool GetEdgeTarget(const CUniqueID& uid, CUniqueID& tgt);

	void SetSubgraphVisible(const CUniqueID& id, bool visible);
	bool GetSubgraphVisible(const CUniqueID& id);

	bool GetSubgraphPinned(const CUniqueID& id) const;
	void SetSubgraphPinned(const CUniqueID& id, bool pinned);

	bool GetSubgraphMinimized(const CUniqueID& id) const;
	void SetSubgraphMinimized(const CUniqueID& id, bool minimized, bool recurse = false);
	bool EnsureVisible(const CUniqueID& id) const;

	void SetSubgraphImage(const CUniqueID& id, const ImageInfo& iinfo, unsigned int overlay);
	GraphTypes::IBitmap* GetSubgraphImage(const CUniqueID& id, IMAGE_POSITION pos, unsigned int overlay, bool subgraphSpecific);
	bool GetSubgraphImageVisible(const CUniqueID& id, IMAGE_POSITION pos);
	void SetSubgraphImageVisible(const CUniqueID& id, IMAGE_POSITION pos, bool visible);
	void SetMetaSubgraphImage(const CCategory& cat, const ImageInfo& iinfo, unsigned int overlay);
	bool GetMetaSubgraphImageVisible(const CCategory& cat, IMAGE_POSITION pos);
	void SetMetaSubgraphImageVisible(const CCategory& cat, IMAGE_POSITION pos, bool visible);

	void SetEdgeDateRange(const CUniqueID& uid, const std::_tstring & from, const std::_tstring & to);
	void SetApplicationMode(AppMode mode);
	AppMode GetApplicationMode() const;
	void SetApplicationVisMode(AppVisMode vismode);
	AppVisMode GetApplicationVisMode() const;
	void SetApplicationZoomMode(AppZoomMode vismode);
	AppZoomMode GetApplicationZoomMode() const;
	void FindGraphItemByID();

	void SetPropertyAttribute(META_TYPE entityType, const CCategory& category, const int id, PROPERTY_ATTR attr, const CComVariant& value);
	unsigned GetPropertyAttributes(IPropAttributeContainer* storage, META_TYPE entityType, const CCategory& category, const int id);
	// TODO: It probably makes sense to expose this to atlCGraphViewCanvas.h also?
	bool GetPropertyAttribute(META_TYPE type, const CCategory& category, const int id, PROPERTY_ATTR attr, _variant_t& retVal);
	unsigned int GetPropertyAttributeId(META_TYPE entityType, const CCategory& category, PROPERTY_ATTR attr, const CComVariant& value, bool autoGenId);

	HWND GetHWND() const { return m_hWnd; };

	void AddRandom(unsigned vertices, unsigned edges);
	void AddRandomTimes();
	IGraphSubgraph * GetSubgraph(const CUniqueID & id) const;
	IGraphVertex * GetVertex(const CUniqueID & id) const;
	IGraphVertex * GetVertex(const CCategory & cat, const TCHAR* id) const;

	IGraphSubgraph * AddSubgraph(const CUniqueID & id, const TCHAR* label, IGraphSubgraph * Subgraph, RendererTypes rtype);
	void RemoveSubgraph(IGraphSubgraph * sg);
	void RemoveSubgraph(const CUniqueID& id);

	void RandomizePosition(GraphTypes::PointF& pos);
	std::pair<IGraphVertex *, bool> AddVertex(const CUniqueID & id, IGraphSubgraph * subgraph, RendererTypes rtype);
	std::pair<IGraphVertex *, bool> AddVertex(const CUniqueID & id, const GraphTypes::PointF & pos, const TCHAR* label= _T(""), bool randomize = false, IGraphSubgraph * subgraph = NULL, RendererTypes rtype=rtLegacyVertex);
	std::pair<IGraphVertex *, bool> AddVertex(const CUniqueID & id, const TCHAR* label = _T(""), RendererTypes rtype=rtLegacyVertex);
	std::pair<IGraphVertex *, bool> AddVertex(const CCategory & cat, const TCHAR* id, const TCHAR* label = _T(""), RendererTypes rtype=rtLegacyVertex);
	std::pair<IGraphVertex *, bool> AddVertex(const CCategory & cat, const TCHAR* id, const GraphTypes::PointF & pos, const TCHAR* label = _T(""), RendererTypes rtype=rtLegacyVertex);
	void RemoveVertex(IGraphVertex * v);
	void RemoveVertex(const CUniqueID& id);

	IGraphEdge * AddEdge(const CUniqueID& edge_id, const CUniqueID & from, const CUniqueID & to, const TCHAR* label = _T(""), RendererTypes rtype=rtLegacyEdge);
	IGraphEdge * AddEdge(const CCategory& category, const CUniqueID & from, const CUniqueID & to, const TCHAR* label = _T(""), RendererTypes rtype=rtLegacyEdge);
	IGraphEdge * AddEdge(const CUniqueID& edge_id, const CCategory & categoryFrom, const TCHAR*idFrom, const CCategory & categoryTo, const TCHAR*idTo, const TCHAR* label = _T(""), RendererTypes rtype=rtLegacyEdge);

	//IGraphEdge * GetEdge(const CUniqueID& edge_id, IGraphVertex * from, IGraphVertex * to) const;
	//IGraphEdge * GetEdge(const CUniqueID& edge_id, const CCategory & categoryFrom, const TCHAR*idFrom, const CCategory & categoryTo, const TCHAR*idTo) const;
	//IGraphEdge * GetEdge(const CUniqueID& edge_id, const CUniqueID & from, const CUniqueID & to) const;
	IGraphEdge * GetEdge(const CUniqueID& edge_id) const;
	
	void RemoveEdge(IGraphEdge * e);
	void RemoveEdge(const CUniqueID & id);

	void GetKnownCategories(META_TYPE entityType, ICategoryContainer * results) const;
	const CUniqueID & GetParent(const CUniqueID & id, CUniqueID & parent) const;
	void GetSubgraphs(IUniqueIDContainer * results, TRISTATE visible=TRISTATE_BOTH, TRISTATE pinned=TRISTATE_BOTH, TRISTATE selected=TRISTATE_BOTH, bool includeHidden=false) const;
	unsigned GetChildren(const CUniqueID& id, IUniqueIDContainer * results, TRISTATE visible=TRISTATE_BOTH, TRISTATE pinned=TRISTATE_BOTH, TRISTATE selected=TRISTATE_BOTH) const;
	void GetVertices(IUniqueIDContainer * results, TRISTATE visible=TRISTATE_BOTH, TRISTATE pinned=TRISTATE_BOTH, TRISTATE selected=TRISTATE_BOTH, bool includeHidden=false) const;
	void GetAdjacentVertices(IUniqueIDContainer * results, const CUniqueID& uid, bool in=true, bool out=true) const;
	void GetEdges(IUniqueIDContainer * results, TRISTATE visible=TRISTATE_BOTH, TRISTATE selected=TRISTATE_BOTH, bool includeHidden=false) const;
	void GetAdjacentEdges(IUniqueIDContainer * results, const CUniqueID& vertexId) const;
	unsigned GetVertexCount(TRISTATE visible=TRISTATE_BOTH, TRISTATE pinned=TRISTATE_BOTH, TRISTATE selected=TRISTATE_BOTH) const;
	unsigned GetEdgeCount() const;
	void GetInEdges(const CUniqueID &vertex, IUniqueIDContainer *results) const;
	void GetOutEdges(const CUniqueID &vertex, IUniqueIDContainer *results) const;

	IVertexIterator* GetSelectedVertexIterator() const;
	IVertexIterator* GetVisibleVertexIterator() const;
	IVertexIterator * GetUserVertexIterator(ISkipVertex * callback_func) const;

	void ClearSelected();

	void SetVertexNeighbour(IGraphVertex * v, SGV::PSGV_TARGETSTRUCT info);
	void SetVertexNeighboursVisibleByEdgeCat(IGraphVertex * v, const CCategory & cat, VISIBLE visible);
	void SetVertexNeighboursVisibleByVertexCat(IGraphVertex * v, const CCategory & cat, VISIBLE visible);

	unsigned GetVertexNeighbourCount(IGraphVertex * v, SGV::PSGV_TARGETSTRUCT info) const;
//	unsigned GetVertexNeighbourCount(IGraphVertex * v);
	unsigned GetVertexVisibleNeighbourCount(IGraphVertex * v) const;
	unsigned GetVertexHiddenNeighbourCount(IGraphVertex * v) const;
	unsigned GetVertexVisibleNeighbourCountByVertexCat(IGraphVertex * v, const CCategory & cat) const;
	unsigned GetVertexHiddenNeighbourCountByVertexCat(IGraphVertex * v, const CCategory & cat) const;
	unsigned GetVertexVisibleNeighbourCountByEdgeCat(IGraphVertex * v, const CCategory & cat) const;
	unsigned GetVertexHiddenNeighbourCountByEdgeCat(IGraphVertex * v, const CCategory & cat) const;
	void SetVerticesVisibleByEdgeCount(IUniqueIDContainer * results, int edgeCount, VISIBLE visibility);
	void SetTargetByCount(IUniqueIDContainer * results, SGV::PSGV_TARGETSTRUCT target);

	const CUniqueID& MergeIntoSubgraph(CUniqueID& retVal, IUniqueIDContainer* nodes, const TCHAR* label);
	void BreakSubgraph(IUniqueIDContainer* results, const CUniqueID& subgraph);

	GraphTypes::RectF GetRenderExtent();
	GraphTypes::RectF GetRenderExtent(bool selectedOnly);
	GraphTypes::RectF GetRenderExtent(HDC dc);
	GraphTypes::RectF GetRenderExtent(HDC dc, bool selectedOnly);
	
	GraphTypes::RectF GetSelectedRenderExtent();
	GraphTypes::RectF GetSelectedRenderExtent(HDC dc);

	void Find(IUniqueIDContainer* results, const TCHAR* searchText, bool searchLabel, bool searchProperties, bool caseSensitive, bool visibleOnly, bool wholeWord, const GUID& pluginId, const int categoryId, const int propertyId);

	void CenterGraph();
	void CenterVertex(const CUniqueID& id, GraphTypes::REAL scale=-1.0f);
	void CenterGraphItem(const CUniqueID& id, GraphTypes::REAL scale=-1.0f);
	void CenterEdge(IGraphEdge * edge, bool fitVertices=false);
	void CenterItem(IGraphItem * item, bool zoomToFit=false);
	void CenterFocusNode();

	void SetScale(float pScale);
	float GetScale() const;
	float SetScaleToFit(bool selectedOnly = false, float maxSize = 1.0f);
	float SetScaleToWidth(bool selectedOnly = false, float maxSize = 1.0f, bool centerVScroll = true);
	float SetItemScaleToFit(const CUniqueID& id, float maxSize = 1.0f);
	void SetScaleAt(const CPoint& pt, GraphTypes::REAL step=0.25f);
	void SetSpacing(float pSpacing);
	float GetSpacing() const;
	void SetRotation(int angle);
	int GetRotation() const;
	void SetOffset(const GraphTypes::PointF& offset);
	void GetOffset(GraphTypes::PointF& offset) const;
	void UpdateScrollBars(const CRect& rcClient);
#ifndef __OLD_SCROLLBARS__
	void Scroll(const CPoint& pt);
#endif
	void KickIt();

	void RenderTo(HDC hDC);
	void RenderTo(HDC hDC, int x, int y, int pageX, int pageY, int pageW, int pageH);
	void RenderTo(CImage &result);
	void RenderTo(CImage &result, int x, int y, int w, int h);
	void RenderTo(CBitmap &result);
	void RenderTo(CBitmap &result, int x, int y, int w, int h);
	DWORD RenderToClipboard(bool emptyFirst);
	bool SaveAs();
	BOOL Save(LPCTSTR lpszFileName);
	void SaveToXmlFile();
	void SaveToXml(std::_tstring & xml, SGV::XML_SAVE_FORMAT saveFormat, const CPoint& relativeSavePos = CPoint());
	bool SupportsJPEG() const;

	void operator()(LAYOUTEVENT e);

	int ScrollWindowEx(int dx, int dy, UINT uFlags, LPCRECT lpRectScroll = NULL, LPCRECT lpRectClip = NULL, HRGN hRgnUpdate = NULL, LPRECT lpRectUpdate = NULL) throw();

	bool GetDragDropNotifications();
	void SetDragDropNotifications(bool value);
#ifdef COMMS_LAYER
	void GetMeta(const TCHAR* url);
	void GetData(const TCHAR* url, const CUniqueID& uid);
#endif

	IRenderer* GetRenderer(const CUniqueID & id) const;
	void SetPosition(const CUniqueID & id, const CPoint &pt, GraphTypes::Coordinates co, bool randomize);
	void GetPosition(const CUniqueID & id, CPoint* result, GraphTypes::Coordinates co = GraphTypes::screen);
	void GetLabel(const CUniqueID & id, std::_tstring & label);
	void SetLabel(const CUniqueID & id, const TCHAR* label);
	void SetZOrder(const CUniqueID & id, int pos);
	void SetRedraw(bool redraw);

	void EditGraphItem(const CUniqueID & id);
	IPicture* CreateIPicFromPath(const TCHAR* ppath);
	void GetGraphLayout(GraphTypes::PointFVector & result) const;
	void SetGraphLayout(const GraphTypes::PointFVector & result);

	bool IsSelected(const CUniqueID & id) const;
	void SetSelected(const CUniqueID & id, bool select, bool append);
	bool HasSelectedVertex();

	bool IsHot(const CUniqueID & id) const;
protected:
	void MoveAllChildren(const GraphTypes::PointF & delta);
	void ClearAllEdgePoints();

	// Renderers methods

	bool GetFocus(CUniqueID& id) const;
	void SetFocus(const CUniqueID& id);

	bool GetBoundsBox(const CUniqueID& id, CRect& rc);
	void SetBoundsBox(const CUniqueID& id, const CRect& rc);

	void RecalcDispSize(const CUniqueID& id);

	bool GetWindowState(const CUniqueID& id, IListWindowRenderer::WINDOW_SIZE& size, int& topindex);
	void SetWindowState(const CUniqueID& id, IListWindowRenderer::WINDOW_SIZE size, int topindex);

	bool GetWindowSize(const CUniqueID& id, CSize& size);
	void SetWindowSize(const CUniqueID& id, const CSize& size);

	// end Renderers methods

	void Start();
	void Stop();

	void delSelNode();
	void InitExternalMaps(IGraphVertex * v, const GraphTypes::PointF & defaultPos);
	void InvalidateLayout();

	bool isNewMouseOver();

	void morph(const GraphTypes::PointFVector &startPos, const GraphTypes::PointFVector &endPos);
	void morph(GraphTypes::REAL startScale, GraphTypes::REAL endScale);
	void morph(const GraphTypes::PointF& startOffset, const GraphTypes::PointF& endOffset);
	void morph(const GraphTypes::PointFVector &startPos, const GraphTypes::PointFVector &endPos, GraphTypes::REAL startScale, GraphTypes::REAL endScale);
	void morph(const GraphTypes::PointFVector &startPos, const GraphTypes::PointFVector &endPos, const GraphTypes::PointF& startOffset, const GraphTypes::PointF& endOffset);
	void morph(GraphTypes::REAL startScale, GraphTypes::REAL endScale, const GraphTypes::PointF& startOffset, const GraphTypes::PointF& endOffset);
	void morph(const GraphTypes::PointFVector &_startPos, const GraphTypes::PointFVector &endPos, GraphTypes::REAL _startScale, GraphTypes::REAL endScale, const GraphTypes::PointF& _startOffset, const GraphTypes::PointF& endOffset);

	bool InitDragAndDrop();

public:
	BOOL PreTranslateMessage(MSG* pMsg);

	BEGIN_MSG_MAP_EX(thisClass)
		{
			MSG Msg = {0};
			Msg.hwnd = hWnd;
			Msg.message = uMsg;
			Msg.wParam = wParam;
			Msg.lParam = lParam;
			Msg.time;
			CPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			ClientToScreen(&pt);
			Msg.pt = pt;
			m_tooltip.RelayEvent(&Msg);
		}
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_LBUTTONUP(OnLButtonUp)
		MSG_WM_LBUTTONDBLCLK(OnLButtonDblClick)
		MESSAGE_HANDLER(WM_RBUTTONDOWN, OnRButtonDown)
		MSG_WM_MOUSELEAVE(OnMouseLeave)
		MSG_WM_HSCROLL(OnHScroll)
		MSG_WM_VSCROLL(OnVScroll)
		MSG_WM_MOUSEWHEEL(OnMouseWheel)
		MESSAGE_HANDLER(WM_KEYDOWN,OnKeyDown)
		MSG_WM_KEYUP(OnKeyUp)
		MESSAGE_HANDLER(UM_LAYOUT_FINISHED, OnLayoutFinished)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_PRINTCLIENT, OnPaint)
        MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		MESSAGE_HANDLER(WM_THEMECHANGED, OnThemeChanged)
		MESSAGE_HANDLER(WM_GETDLGCODE,OnGetDlgCode)
		MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
		MESSAGE_HANDLER(WM_NCHITTEST, OnNcHitTest)
		MESSAGE_HANDLER(UM_TOOLTIP_LINKCLICKED, OnNotifyTooltipLinkClick)
		MSG_WM_KILLFOCUS(OnKillFocus)
		MSG_WM_SETFOCUS(OnSetFocus)
		MSG_WM_CONTEXTMENU(OnContextMenu)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)

		NOTIFY_CODE_HANDLER(UDM_TOOLTIP_DISPLAY, OnNotifyDisplayTooltip)

		NOTIFY_CODE_HANDLER(SCW_NOTIFYSELECTED, OnNotifySelected)
		NOTIFY_CODE_HANDLER(SCW_NOTIFYITEMSCONNECTED, OnNotifyItemsConnected)
		NOTIFY_CODE_HANDLER(SCW_NOTIFYRECALCEDGEPOINTS, OnNotifyRecalcEdges)
		NOTIFY_CODE_HANDLER(SCW_NOTIFYFOCUSCHANGED, OnNotifyFocusChanged)
		NOTIFY_CODE_HANDLER(SCW_NOTIFYHOTCHANGED, OnNotifyHotChanged)
		NOTIFY_CODE_HANDLER(SCW_NOTIFYDODRAGDROP, OnNotifyDoDragDrop)
		NOTIFY_CODE_HANDLER(SCW_NOTIFYSCROLL, OnNotifyScroll)
		NOTIFY_CODE_HANDLER(SCW_NOTIFYMOUSEMESSAGE, OnNotifyMouseMessage)
		NOTIFY_CODE_HANDLER(SCW_NOTIFYLAYOUTFINISHED, OnNotifyLayoutFinished)

		MSG_SGV_CLEAR(OnClear)
		MSG_SGV_CLEAREDGES(OnClearEdges)
		MSG_SGV_SETLAYOUT(SetLayout)
		MSG_SGV_GETLAYOUT(GetLayout)
		MSG_SGV_SETOWNER(SetOwner)
		MSG_SGV_GETMOUSEMODE(GetMouseMode)
		MSG_SGV_SETMOUSEMODE(SetMouseMode)
		MSG_SGV_ADDVERTEX(AddVertex)
		MSG_SGV_ADDEDGE(AddEdge)
		MSG_SGV_ADDRANDOM(AddRandom)
		MSG_SGV_ADDRANDOMTIMES(AddRandomTimes)
		MSG_SGV_ADDSUBGRAPH(AddSubgraph)
		MSG_SGV_LOADXGMML(LoadXGMML)
		MSG_SGV_MERGEXGMML(MergeXGMML)
		MSG_SGV_HASITEMS(HasItems)
		MSG_SGV_LOADXML(LoadXml)
		MSG_SGV_LOADXMLFILE(LoadXmlFile)
		MSG_SGV_GETSUBGRAPH(GetSubgraph)
		MSG_SGV_GETVERTEX(GetVertex)
		MSG_SGV_GETGRAPHITEM(GetGraphItem)
		MSG_SGV_GETVERTEXCOUNT(GetVertexCount)
		MSG_SGV_GETVERTEXNEIGHBOURCOUNT(GetVertexNeighbourCount)
		MSG_SGV_GETEDGES(GetEdges)
		MSG_SGV_GETADJACENTEDGES(GetAdjacentEdges)
		MSG_SGV_GETEDGE(GetEdge)
		MSG_SGV_GETEDGECOUNT(GetEdgeCount)
		MSG_SGV_GETINEDGES(GetInEdges)
		MSG_SGV_GETOUTEDGES(GetOutEdges)
		MSG_SGV_GETCATEGORIES(GetKnownCategories)
		MSG_SGV_GETPARENT(GetParent)
		MSG_SGV_GETSUBGRAPHS(GetSubgraphs)
		MSG_SGV_GETCHILDREN(GetChildren)
		MSG_SGV_GETVERTICES(GetVertices)
		MSG_SGV_GETADJACENTVERTICES(GetAdjacentVertices)
		MSG_SGV_GETMETADATA(GetMetaData)
		MSG_SGV_SETVERTEXVISIBLE(SetVertexVisible)
		MSG_SGV_GETVERTEXVISIBLE(GetVertexVisible)
		MSG_SGV_SETVERTEXPINNED(SetVertexPinned)
		MSG_SGV_GETVERTEXPINNED(GetVertexPinned)
		MSG_SGV_SETMETAVERTEXIMAGE(SetMetaVertexImage)
		MSG_SGV_SETMETAEDGEIMAGE(SetMetaEdgeImage)
		MSG_SGV_SETVERTEXIMAGE(SetVertexImage)
		MSG_SGV_GETVERTEXIMAGE(GetVertexImage)
		MSG_SGV_GETVERTEXIMAGESVISIBLE(GetVertexImagesVisible)
		MSG_SGV_SETVERTEXIMAGESVISIBLE(SetVertexImagesVisible)
		MSG_SGV_GETVERTEXIMAGEVISIBLE(GetVertexImageVisible)
		MSG_SGV_SETVERTEXIMAGEVISIBLE(SetVertexImageVisible)
		MSG_SGV_GETMETAVERTEXIMAGEVISIBLE(GetMetaVertexImageVisible)
		MSG_SGV_SETMETAVERTEXIMAGEVISIBLE(SetMetaVertexImageVisible)
		MSG_SGV_SETEDGEVISIBLE(SetEdgeVisible)
		MSG_SGV_GETEDGEVISIBLE(GetEdgeVisible)
		MSG_SGV_SETEDGEIMAGE(SetEdgeImage)
		MSG_SGV_GETEDGEIMAGE(GetEdgeImage)
		MSG_SGV_GETEDGEIMAGEVISIBLE(GetEdgeImageVisible)
		MSG_SGV_SETEDGEIMAGEVISIBLE(SetEdgeImageVisible)
		MSG_SGV_GETMETAEDGEIMAGEVISIBLE(GetMetaEdgeImageVisible)
		MSG_SGV_SETMETAEDGEIMAGEVISIBLE(SetMetaEdgeImageVisible)
		MSG_SGV_GETEDGECOLOR(GetEdgeColor)
		MSG_SGV_SETEDGECOLOR(SetEdgeColor)
		MSG_SGV_SETEDGEDATERANGE(SetEdgeDateRange)
		MSG_SGV_SETAPPMODE(SetApplicationMode)
		MSG_SGV_GETAPPMODE(GetApplicationMode)
		MSG_SGV_SETAPPVISMODE(SetApplicationVisMode)
		MSG_SGV_GETAPPVISMODE(GetApplicationVisMode)
		MSG_SGV_SETAPPZOOMMODE(SetApplicationZoomMode)
		MSG_SGV_GETAPPZOOMMODE(GetApplicationZoomMode)
		MSG_SGV_FINDIDDIALOG(FindGraphItemByID)
		MSG_SGV_GETEDGESOURCE(GetEdgeSource)
		MSG_SGV_GETEDGETARGET(GetEdgeTarget)
		MSG_SGV_REMOVEVERTEX(RemoveVertex)
		MSG_SGV_REMOVEEDGE(RemoveEdge)
		MSG_SGV_REMOVESUBGRAPH(RemoveSubgraph)
		MSG_SGV_SETMETADATA(SetMetaData)
		MSG_SGV_SETTARGETBYCOUNT(SetTargetByCount)
		MSG_SGV_SETVERTEXNEIGHBOUR(SetVertexNeighbour)
		MSG_SGV_CENTERGRAPH(CenterGraph)
		MSG_SGV_CENTERGRAPHITEM(CenterGraphItem)
		MSG_SGV_CENTERFOCUSNODE(CenterFocusNode)
		MSG_SGV_KICKIT(KickIt)
		MSG_SGV_INVALIDATELAYOUT(InvalidateLayout)
		MSG_SGV_SETSCALE(SetScale)
		MSG_SGV_GETSCALE(GetScale)
		MSG_SGV_SETSCALETOFIT(SetScaleToFit)
		MSG_SGV_SETSCALETOWIDTH(SetScaleToWidth)
		MSG_SGV_SETITEMSCALETOFIT(SetItemScaleToFit)
		MSG_SGV_SETSPACING(SetSpacing)
		MSG_SGV_GETSPACING(GetSpacing)
		MSG_SGV_SETROTATION(SetRotation)
		MSG_SGV_GETROTATION(GetRotation)
		MSG_SGV_SETOFFSET(SetOffset)
		MSG_SGV_GETOFFSET(GetOffset)
		MSG_SGV_GETFOCUS(GetFocus)
		MSG_SGV_SETFOCUS(SetFocus)
		MSG_SGV_GETSELECTED(IsSelected)
		MSG_SGV_SETSELECTED(SetSelected)
		MSG_SGV_HASSELECTEDVERTEX(HasSelectedVertex)
		MSG_SGV_GETBOUNDSBOX(GetBoundsBox)
		MSG_SGV_SETBOUNDSBOX(SetBoundsBox)
		MSG_SGV_RECALCDISPSIZE(RecalcDispSize)
		MSG_SGV_GETWINDOWSTATE(GetWindowState)
		MSG_SGV_SETWINDOWSTATE(SetWindowState)
		MSG_SGV_GETWINDOWSIZE(GetWindowSize)
		MSG_SGV_SETWINDOWSIZE(SetWindowSize)
		MSG_SGV_RENDERTOCONTEXT(RenderTo)
		MSG_SGV_RENDERTOIMAGE(RenderTo)
		MSG_SGV_RENDERTOIMAGE1(RenderTo)
		MSG_SGV_RENDERTOBITMAP(RenderTo)
		MSG_SGV_RENDERTOBITMAP1(RenderTo)
		MSG_SGV_RENDERTOCLIPBOARD(RenderToClipboard)
		MSG_SGV_SAVEAS(SaveAs)
		MSG_SGV_SAVEASNAME(Save)
		MSG_SGV_SAVETOXML(SaveToXml)
		MSG_SGV_SAVETOXMLFILE(SaveToXmlFile)
		MSG_SGV_GETRENDEREXTENT(GetRenderExtent)
		MSG_SGV_FIND(Find)
		MSG_SGV_GETDRAGDROPNOTIFICATIONS(GetDragDropNotifications)
		MSG_SGV_SETDRAGDROPNOTIFICATIONS(SetDragDropNotifications)
#ifdef COMMS_LAYER
		MSG_SGV_GETDATA(GetData)
		MSG_SGV_GETMETA(GetMeta)
#endif
		MSG_SGV_GETPOSITION(GetPosition)
		MSG_SGV_SETPOSITION(SetPosition)
		MSG_SGV_GETLABEL(GetLabel)
		MSG_SGV_SETLABEL(SetLabel)
		MSG_SGV_SETZORDER(SetZOrder)
		MSG_SGV_SETREDRAW(SetRedraw)
		MSG_SGV_EDITGRAPHITEM(EditGraphItem)
		MSG_SGV_CREATEIPIC(CreateIPicFromPath)
		MSG_SGV_GETPROPERTYUNKNOWN(GetPropertyUnknown)
		MSG_SGV_SETPROPERTY(SetProperty)
		MSG_SGV_GETPROPERTY(GetProperty)
		MSG_SGV_GETPROPERTIES(GetProperties)
		MSG_SGV_SETPROPERTYATTRIBUTE(SetPropertyAttribute)
		MSG_SGV_GETPROPERTYATTRIBUTES(GetPropertyAttributes)
		MSG_SGV_GETPROPERTYATTRIBUTEID(GetPropertyAttributeId)
		MSG_SGV_SETMESSAGE(SetMessage)
		MSG_SGV_SHOWOVERVIEW(ShowOverview)
		MSG_SGV_GETOVERVIEW(GetOverview)
		MSG_SGV_SET_EDGE_LABEL_VISIBLE(SetEdgeLabelVisible)
		MSG_SGV_GET_EDGE_LABEL_VISIBLE(IsEdgeLabelVisible)
		MSG_SGV_SETOVERVIEWOPACITY(SetOverviewOpacity)
		MSG_SGV_GETOVERVIEWOPACITY(GetOverviewOpacity)
		MSG_SGV_SETOVERVIEWPOSITION(SetOverviewPosition)
		MSG_SGV_SETOVERVIEWSIZE(SetOverviewSize) 
		MSG_SGV_EMULATEDECORATION(EmulateDecoration) 
		MSG_SGV_ISEMULATEDECORATION(IsEmulateDecoration)
		MSG_SGV_SETROWCOMPRESS(SetRowCompress)
		MSG_SGV_GETROWCOMPRESS(GetRowCompress)
		MSG_SGV_SETREADONLY(SetReadOnly)
		MSG_SGV_ISREADONLY(IsReadOnly)
		MSG_SGV_MERGEINTOSUBGRAPH(MergeIntoSubgraph)
		MSG_SGV_BREAKSUBGRAPH(BreakSubgraph)
		MSG_SGV_SETSUBGRAPHVISIBLE(SetSubgraphVisible)
		MSG_SGV_GETSUBGRAPHVISIBLE(GetSubgraphVisible)
		MSG_SGV_SETSUBGRAPHIMAGE(SetSubgraphImage)
		MSG_SGV_GETSUBGRAPHIMAGE(GetSubgraphImage)
		MSG_SGV_GETSUBGRAPHIMAGEVISIBLE(GetSubgraphImageVisible)
		MSG_SGV_SETSUBGRAPHIMAGEVISIBLE(SetSubgraphImageVisible)
		MSG_SGV_SETMETASUBGRAPHIMAGE(SetMetaSubgraphImage)
		MSG_SGV_GETMETASUBGRAPHIMAGEVISIBLE(GetMetaSubgraphImageVisible)
		MSG_SGV_SETMETASUBGRAPHIMAGEVISIBLE(SetMetaSubgraphImageVisible)
		MSG_SGV_SETSUBGRAPHPINNED(SetSubgraphPinned)
		MSG_SGV_GETSUBGRAPHPINNED(GetSubgraphPinned)
		MSG_SGV_SETSUBGRAPHMINIMIZED(SetSubgraphMinimized)
		MSG_SGV_GETSUBGRAPHMINIMIZED(GetSubgraphMinimized)
		MSG_SGV_ENSUREVISIBLE(EnsureVisible)

		REFLECT_NOTIFICATIONS()
	//	CHAIN_MSG_MAP( baseClass )
	END_MSG_MAP()

	LRESULT OnCreate(LPCREATESTRUCT lParam);
	void OnDestroy();
	void OnLButtonDown(UINT vKey, const CPoint &pos);
	void OnMouseMove(UINT vKey, const CPoint &pos);
	void OnLButtonUp(UINT vKey, const CPoint &pos);
	void OnLButtonDblClick(UINT vKey, const CPoint &pos);
	void OnContextMenu(HWND hWnd, const CPoint &pos);
	void OnMouseLeave();
	void OnHScroll(int scrollRequest, short scrollPos, HWND hWnd);
	void OnVScroll(int scrollRequest, short scrollPos, HWND hWnd);
	BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	void OnKeyUp(TCHAR key, UINT p1, UINT p2);
	void DoPaint(CAggDC& dc, bool calculateExtent = false);
	void OnSetFocus(HWND);
	void OnKillFocus(HWND);
	LRESULT OnLayoutFinished(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	//  Derived  ---
	LRESULT OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnThemeChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnGetDlgCode(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSetCursor(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnNcHitTest(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnRButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnNotifyTooltipLinkClick(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	LRESULT OnNotifyDisplayTooltip(WPARAM wParam, LPNMHDR lpnmhdr, BOOL& bHandled);

	LRESULT OnNotifySelected(WPARAM wParam, LPNMHDR lpnmhdr, BOOL& bHandled);
	LRESULT OnNotifyItemsConnected(WPARAM wParam, LPNMHDR lpnmhdr, BOOL& bHandled);
	LRESULT OnNotifyRecalcEdges(WPARAM wParam, LPNMHDR lpnmhdr, BOOL& bHandled);
	LRESULT OnNotifyFocusChanged(WPARAM wParam, LPNMHDR lpnmhdr, BOOL& bHandled);
	LRESULT OnNotifyHotChanged(WPARAM wParam, LPNMHDR lpnmhdr, BOOL& bHandled);
	LRESULT OnNotifyDoDragDrop(WPARAM wParam, LPNMHDR lpnmhdr, BOOL& bHandled);
	LRESULT OnNotifyScroll(WPARAM wParam, LPNMHDR lpnmhdr, BOOL& bHandled);
	LRESULT OnNotifyMouseMessage(WPARAM wParam, LPNMHDR lpnmhdr, BOOL& bHandled);
	LRESULT OnNotifyLayoutFinished(WPARAM wParam, LPNMHDR lpnmhdr, BOOL& bHandled);

	void lock(CComPtr<clib::scoped_lock_ref_counted> & lock);

	//  --  ISkip  --
	bool SkipEdge(IGraphEdge * e) const;
	bool SkipVertex(IGraphVertex * v) const;
	bool SkipSubgraph(IGraphSubgraph * sg) const;

	//  ---  IXGMMLRenderCallback  ---
	GraphTypes::Color GetColor(IGraphSubgraph * subgraph) const;
	void GetColor(IGraphVertex * vertex, GraphTypes::Color &borderColor, GraphTypes::Color & fillColor, GraphTypes::Color & fontColor) const;
	void GetColor(IGraphEdge * edge, GraphTypes::Color & edgeColor, GraphTypes::Color & fontColor, GraphTypes::Color & firstControlPointColor, GraphTypes::Color & lastControlPointColor) const;
};
//  ===========================================================================
