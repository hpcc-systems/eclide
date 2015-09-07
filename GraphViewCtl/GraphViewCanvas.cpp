//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "IPictureHelper.h"
#include "GraphViewCanvas.h"
#include "touchgraph.h"
#include "circular.h"
#include "Lee.h"
#include "neato.h"
#include "null.h"
#include "util.h"
#include "ScaledWindows.h"
#include "unicodefile.h"
#include "AggDC.h"
#include "GraphViewDropTarget.h"
#include "comutil.h"
#include "utilDateTime.h"
#include "timelineLayout.h"
#include "TimelineCtrl.h"
#include "OverviewWnd.h"
#include "QueryBuilderUI.h"
#include "Global.h"
#include "util.h"
#include "FilterSubgraphEdges.h"

using namespace GraphTypes;

class CBuildingEdgeVisibility
{
	int & m_building;
public:
	CBuildingEdgeVisibility(int & building) : m_building(building)
	{
		++m_building;
	}
	~CBuildingEdgeVisibility()
	{
		--m_building;
	}
};

SGV::GRAPH_ITEM_TYPE Item2Type (IGraphItem* item) 
{
	if (0 != item)
	{
		if (CComQIPtr<IGraphVertex>(item))
			return SGV::GRAPH_ITEM_TYPE_VERTEX;

		if (CComQIPtr<IGraphEdge>(item))
			return SGV::GRAPH_ITEM_TYPE_EDGE;

		if (CComQIPtr<IGraphSubgraph>(item))
			return SGV::GRAPH_ITEM_TYPE_SUBGRAPH;
	}

	return SGV::GRAPH_ITEM_TYPE_UNKNOWN;
}

CGraphViewCanvas::CGraphViewCanvas() :
	m_Owner(NULL), m_dropTarget(NULL), m_randomDataGenerated(false),
	m_NoRedraw(0), m_bMouseDown(false), m_running(false), m_xmlSerializerAnb(this),
	m_breakFlag(false), m_timeline(0), m_inScrollDrag(false)
{
	m_graph = CreateDirectedGraph(this, this, this);
	m_canvas = new CCanvas(m_graph, m_meta);
	m_edgeFilter = new CFilterSubgraphEdges(*this);

	srand((unsigned)time(NULL));

	m_layout_spring1 = new CGraphTouchgraphLayoutImpl(m_graph,m_meta, m_canvas);
	m_layout_circle = new CGraphCircularLayoutImpl(m_graph, m_meta, m_canvas);
	m_layout_timeline = new CGraphTimelineLayoutImpl(m_graph, m_meta, m_canvas);
#ifdef LAYOUT_LEE
	m_layout_lee = new CGraphLeeLayoutImpl(m_graph, m_meta, m_canvas);
#endif
	m_layout_neato = new CGraphATTLayoutImpl(this, m_graph, m_canvas, m_meta, NEATO_MODE_NEATO);
	m_layout_fdp = new CGraphATTLayoutImpl(this, m_graph, m_canvas, m_meta, NEATO_MODE_FDP);
	m_layout_twopi = new CGraphATTLayoutImpl(this, m_graph, m_canvas, m_meta, NEATO_MODE_TWOPI);
	m_layout_dot = new CGraphATTLayoutImpl(this, m_graph, m_canvas, m_meta, NEATO_MODE_DOT);
	m_layout_circo = new CGraphATTLayoutImpl(this, m_graph, m_canvas, m_meta, NEATO_MODE_CIRCO);
	m_layout_visgedge = new CGraphATTLayoutImpl(this, m_graph, m_canvas, m_meta, NEATO_MODE_EDGES);
	m_layout_subgraph_tree = new CGraphATTLayoutImpl(this, m_graph, m_canvas, m_meta, NEATO_MODE_SUBGRAPH_TREE);

	m_layout_null = new CGraphNullLayoutImpl(m_graph, m_meta, m_canvas);
	m_layout = m_layout_null;
	m_stopping_layout = NULL;
	m_next_layout = NULL;
	m_lastLayout = SGV::LAYOUT_NULL;

	m_buildingEdgeVisibility = 0;

	m_visualState = WUVisualState_unknown;

	m_prevCenter.x = 1;
	m_prevCenter.y = 1;
}

CGraphViewCanvas::~CGraphViewCanvas()
{
	if (m_dropTarget)
		delete m_dropTarget;
	m_dropTarget=0;
}

void CGraphViewCanvas::OnClear()
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	Stop();
	SetRedraw(false);
	ClearSelected();

	CBuildingEdgeVisibility building(m_buildingEdgeVisibility);
	RemoveSubgraph(m_graph);
	m_canvas->Clear();
	m_edgeFilter->Reset();
	QBUI::DeleteNodePopups();
	SetRedraw(true);
}

void CGraphViewCanvas::OnClearEdges()
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	Stop();
	SetRedraw(false);

	CBuildingEdgeVisibility building(m_buildingEdgeVisibility);
	IGraphEdgeVector edges;
	m_graph->GetEdges(edges);
	for(IGraphEdgeVector::reverse_iterator itr = edges.rbegin(); itr != edges.rend(); ++itr)
	{
		CComPtr<IGraphEdge> edge = *itr;
		std::_tstring edgeID = edge->GetID().GetID();
		RemoveEdge(edge);
	}

	SetRedraw(true);
}

bool CGraphViewCanvas::LoadXGMMLHelper(const TCHAR* xgmml, WUVisualState state, bool redundantSubgraphs, bool passthroughVertices, const IXGMMLRenderCallback * callback)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	ATLASSERT(!m_running);
	m_visualState = state;
	if (callback == NULL)
		callback = this;
	CBuildingEdgeVisibility building(m_buildingEdgeVisibility);
	if (!m_graph->Load(xgmml, callback, redundantSubgraphs, passthroughVertices)) //Is it rkc "special" xml?
	{
		CString str = xgmml;//_T("123<xgmml>456</xgmml>789<xgmml>0ab</xgmml>cde");
		CString start_xml = _T("<xgmml>"), end_xml = _T("</xgmml>");
		int start_pos = str.Find(start_xml, 0);
		int end_pos = str.Find(end_xml, 0);
		if (start_pos >= 0 && end_pos >= 0 && end_pos > start_pos)
			return m_graph->Load((const TCHAR *)str.Mid(start_pos, (end_pos + end_xml.GetLength() - start_pos)), callback, redundantSubgraphs, passthroughVertices);
		return false;
	}
	return true;
}

void CGraphViewCanvas::LoadXGMML(const TCHAR* xgmml, WUVisualState state, bool redundantSubgraphs, bool passthroughVertices, bool minimizeIfLarge, const IXGMMLRenderCallback * callback)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	ATLASSERT(!m_running);
	m_visualState = state;
	if (callback == NULL)
		callback = this;
#ifdef _DEBUG
	//if (static_cast<bool>(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_DEBUG_LOGRESULTXML))==true)
	{
		CUnicodeFile f;
		if(f.Create(_T("C:\\temp\\xgmml_to_render.xml"), GENERIC_WRITE, CREATE_ALWAYS, CUnicodeFile::ENCODING_UTF_8))
		{
			f.Write(xgmml);
			f.Close();
		}
	}
#endif

	if (!LoadXGMMLHelper(xgmml, state, redundantSubgraphs, passthroughVertices, callback))
	{
		CUnicodeFile f;
		if (f.Open(xgmml))
		{
			std::_tstring xml;
			f.Read(xml);
			f.Close();
			LoadXGMMLHelper(xml.c_str(), state, redundantSubgraphs, passthroughVertices, callback);
		}
	}

	if (minimizeIfLarge && GetVertexCount() > 300)
	{
		CUniqueIDVector subgraphs;
		GetSubgraphs(&subgraphs);
		subgraphs.Reset();
		while(subgraphs.MoveNext())
		{
			//if (GetSubgraph(subgraphs.Get())->GetParent() != m_graph)
				SetSubgraphMinimized(subgraphs.Get(), true, true);
		}
	}
}

void CGraphViewCanvas::MergeXGMML(const TCHAR* xgmml, WUVisualState state, bool appendMissing, bool redundantSubgraphs, bool passthroughVertices, const IXGMMLRenderCallback * callback)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	ATLASSERT(!m_running);
	m_visualState = state;
	if (callback == NULL)
		callback = this;
#ifdef _DEBUG
	//if (static_cast<bool>(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_DEBUG_LOGRESULTXML))==true)
	{
		CUnicodeFile f;
		if(f.Create(_T("C:\\temp\\xgmml_to_render.xml"), GENERIC_WRITE, CREATE_ALWAYS, CUnicodeFile::ENCODING_UTF_8))
		{
			f.Write(xgmml);
			f.Close();
		}
	}
#endif
	CBuildingEdgeVisibility building(m_buildingEdgeVisibility);
	m_graph->Merge(xgmml, callback, appendMissing, redundantSubgraphs, passthroughVertices);
	QBUI::UpdateNodePopups();
	InvalidateLayout();
}

bool CGraphViewCanvas::HasItems()
{
	return m_graph->HasItems();
}

void CGraphViewCanvas::LoadXml(const TCHAR* xml, const CPoint& relativeLoadPos, int version)
{
	if (_tcslen(xml) == 0)
		return;

	CComInitialize com;

	CComPtr<ISAXXMLReader> pRdr; 

	HRESULT hr = pRdr.CoCreateInstance(CLSID_SAXXMLReader30);
	if (SUCCEEDED(hr))
	{
		CComPtr<CXmlAnbDeserializer> pHandler = new CXmlAnbDeserializer(this, m_canvas, relativeLoadPos, version);
		hr = pRdr->putContentHandler(pHandler); 
		assert(SUCCEEDED(hr));

		if (SUCCEEDED(hr))
		{
			hr = pRdr->parse(variant_t(xml));
			assert(SUCCEEDED(hr));
		}
	}
}

void CGraphViewCanvas::LoadXmlFile() 
{
	CString strExporters = _T("XML Document|*.XML||");
	CString strExporterDefault = _T(".XML");

	LPTSTR pch = strExporters.GetBuffer(0); // modify the buffer in place
	while ((pch = _tcschr(pch, '|')) != NULL)
		*pch++ = '\0';

	CFileDialogEx wndFileDialog(TRUE, strExporterDefault, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strExporters, m_hWnd);
	if (IDOK == wndFileDialog.DoModal())
	{
		CUnicodeFile file;
		file.Open(wndFileDialog.GetPathName());
		CComVariant xml;
		file.Read(xml);
		xml.ChangeType(VT_BSTR);
		LoadXml(bstr_t(xml));
	}
}

void CGraphViewCanvas::SetOwner(SGV::IGraphViewSlot * owner)
{
	m_Owner = owner;
	if(m_dropTarget)
		m_dropTarget->SetOwner(owner);
}

void CGraphViewCanvas::SetMessage(const std::_tstring & msg)
{
	m_message = msg;
	InvalidateLayout();	
}

void CGraphViewCanvas::ShowOverview(bool show)
{
	m_meta.SetOverviewVisible(show);
	if(!m_meta.IsEmulateDecoration())
		m_overview->ShowWindow(show?SW_SHOW:SW_HIDE);
	InvalidateLayout();	
}

bool CGraphViewCanvas::GetOverview() const
{
	return m_meta.IsOverviewVisible();
}

void CGraphViewCanvas::SetRowCompress(bool compress)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	if(m_meta.SetRowCompress(compress))
	{
		m_canvas->ClearCaches();
		InvalidateLayout();	
	}
}

bool CGraphViewCanvas::GetRowCompress() const
{
	return m_meta.GetRowCompress();
}

void CGraphViewCanvas::SetReadOnly(bool ro) 
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	if(m_meta.SetReadOnly(ro))
		InvalidateLayout();	
}

bool CGraphViewCanvas::IsReadOnly() const
{
	return m_meta.IsReadOnly();
}

#ifdef _DEBUG
void CGraphViewCanvas::SetGammaThreshold(REAL thr) 
{
	m_canvas->SetGammaThreshold(thr);
}

REAL CGraphViewCanvas::GetGammaThreshold() const
{
	return m_canvas->GetGammaThreshold();
}
#endif

void CGraphViewCanvas::SetEdgeLabelVisible(bool visible)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	m_meta.SetEdgeLabelVisible(visible);
	InvalidateLayout();	
}

bool CGraphViewCanvas::IsEdgeLabelVisible() const
{
	return m_meta.IsEdgeLabelVisible();
}

void CGraphViewCanvas::CalculateEdgeVisibility()
{
	//clib::recursive_mutex::scoped_lock lock(m_mutex);
	CBuildingEdgeVisibility building(m_buildingEdgeVisibility);
	m_edgeFilter->Reset();
	for(CComPtr<IEdgeIterator> itr = m_graph->GetEdgeIterator(); itr->IsValidItem(); itr->NextItem())
	{
		m_edgeFilter->CalcEdge(itr->Get());
	}
}

void CGraphViewCanvas::SetOverviewOpacity(BYTE opacity)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	if(!m_meta.IsEmulateDecoration())
	{
		ATLASSERT(m_overview->IsWindow());
		m_overview->SetOverviewOpacity(opacity);	
	}

	m_meta.SetOverviewOpacity(opacity);
	InvalidateLayout();	
}

BYTE CGraphViewCanvas::GetOverviewOpacity() const
{
	return m_meta.GetOverviewOpacity();
}

void CGraphViewCanvas::SetOverviewPosition(const CPoint& pos)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);

	if(m_meta.IsEmulateDecoration())
	{
		IOverviewRenderer* ir=m_canvas->GetOverviewRenderer();
		if(ir)
		{
			PointF origin(pos);
			ir->SetPosition(origin, GraphTypes::screen);
			InvalidateLayout();	
		}
	}
	else
	{
		ATLASSERT(m_overview->IsWindow());
		m_overview->SetOverviewPosition(pos);
	}
}

void CGraphViewCanvas::SetOverviewSize(const CSize& size) 
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	if(m_meta.IsEmulateDecoration())
	{
		IOverviewRenderer* ir=m_canvas->GetOverviewRenderer();
		if(ir)
		{
			RectF orgrc;
			ir->GetBoundsBox(orgrc);
			orgrc.Width=static_cast<GraphTypes::REAL>(size.cx);
			orgrc.Height= static_cast<GraphTypes::REAL>(size.cy);
			ir->SetBoundsBox(orgrc,SWP_NOMOVE|SWP_NOREDRAW);
			InvalidateLayout();	
		}
	}
	else
	{
		ATLASSERT(m_overview->IsWindow());
		m_overview->SetOverviewSize(size);
	}
}

bool CGraphViewCanvas::EmulateDecoration(bool emulate)
{
	return m_meta.EmulateDecoration(emulate);
}

bool CGraphViewCanvas::IsEmulateDecoration() const
{
	return m_meta.IsEmulateDecoration();
}

SGV::LAYOUT CGraphViewCanvas::GetLayout()
{
	return m_lastLayout;
}

void CGraphViewCanvas::SetLayout(SGV::LAYOUT layout)
{
	// in the case of spring, since it's threaded and takes time, we get out if currently running
	// and asked to go into spring mode again.
	if (m_running && (layout == SGV::LAYOUT_SPRING1 || layout == SGV::LAYOUT_SPRING2))
		return;

	Stop();	//Stop existing layout - should not be insside m_mutex;.
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	assert(!m_running);
	CComPtr<ILayout> oldLayout = m_layout;
	MSG msg;
	while (PeekMessage(&msg, *this, UM_LAYOUT_FINISHED, UM_LAYOUT_FINISHED, PM_REMOVE)) 
	{ 
		//  Remove any finished msgs.
	} 
	assert(!m_running);

	CalculateEdgeVisibility();

	m_lastLayout = layout;
	switch(layout)
	{
	case SGV::LAYOUT_SPRING1:
		m_layout = m_layout_spring1;
		break;

	case SGV::LAYOUT_CIRCLE:
		m_layout = m_layout_circle;
		break;

#ifdef LAYOUT_LEE
	case SGV::LAYOUT_LEE:
		m_layout = m_layout_lee;
		break;
#endif

	case SGV::LAYOUT_NEATO:
		m_layout = m_layout_neato;
		break;

	case SGV::LAYOUT_FDP:
		m_layout = m_layout_fdp;
		break;

	case SGV::LAYOUT_TIMELINE:
		m_layout = m_layout_timeline;
		break;

	case SGV::LAYOUT_TWOPI:
		m_layout = m_layout_twopi;
		break;

	case SGV::LAYOUT_DOT:
		m_layout = m_layout_dot;
		break;

	case SGV::LAYOUT_CIRCO:
		m_layout = m_layout_circo;
		break;

	case SGV::LAYOUT_SUBGRAPH_TREE:
		m_layout=m_layout_subgraph_tree;
		break;

	case SGV::LAYOUT_VISGEDGES:
		m_layout=m_layout_visgedge;
		break;

	case SGV::LAYOUT_NULL:
		m_layout = m_layout_null;
		break;

	default:
		break;
	}
	assert(!m_running);

	m_prevLayout.clear();
	GetGraphLayout(m_prevLayout);

	GraphTypes::RectF extent = GetRenderExtent();
	if (m_canvas->HasVertexFocus())
	{
		CRect pos;
		m_canvas->m_currentFocus->GetWindowRect(pos);
		m_prevCenter.x = static_cast<GraphTypes::REAL>(pos.left);
		m_prevCenter.y = static_cast<GraphTypes::REAL>(pos.top);
		ATLTRACE("v ");
	}
	else
	{
		extent.GetCenter(&m_prevCenter);
		ATLTRACE("g ");
	}
	ATLTRACE("pos before %f, %f\n", m_prevCenter.x, m_prevCenter.y);

	if (oldLayout != m_layout)
	{
		ClearAllEdgePoints();
	}

	Start();
}


IGraphItem* CGraphViewCanvas::GetGraphItem(const CUniqueID & id) const
{
	CComPtr<IGraphItem> gi;

	gi = m_graph->GetSubgraph(id);
	if(!gi)
	{
		gi = m_graph->GetVertex(id);
	}
	if(!gi)
	{
		gi = m_graph->GetEdge(id);
	}
	return gi;
}

void CGraphViewCanvas::SetProperty(const CUniqueID & id, int key, const CComVariant & val)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	CComPtr<IGraphItem> gi = GetGraphItem(id);
	if (gi)
		gi->SetProperty(key, val);
}

void CGraphViewCanvas::GetProperty(const CUniqueID & id, int key, CComVariant & result)
{
	CComPtr<IGraphItem> gi = GetGraphItem(id);
	if (gi)
		gi->GetProperty(key, result);
}

void CGraphViewCanvas::GetProperties(IPropertyIDContainer* results, const CUniqueID & id)
{
	CComPtr<IGraphItem> gi = GetGraphItem(id);
	if (gi)
		gi->GetProperties(results);
}

IUnknown * CGraphViewCanvas::GetPropertyUnknown(const CUniqueID & id, int key)
{
	CComPtr<IGraphItem> gi = GetGraphItem(id);
	if (gi)
		return gi->GetPropertyUnknown(key);
	return NULL;
}

void CGraphViewCanvas::SetMouseMode(MOUSE_MODES val)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	m_meta.SetMouseMode(val);
	InvalidateLayout();	
}

MOUSE_MODES CGraphViewCanvas::GetMouseMode() const
{
	return m_meta.GetMouseMode();
}

REAL CGraphViewCanvas::CalcScaleStep()
{
	REAL step=m_canvas->GetScale();
	if(step<1)
		step*=0.1f;
	else
		step=0.1f;
	return step;
}

void CGraphViewCanvas::GetMetaData(const SGV::PSGV_CATEGORYSTRUCT category, SGV::PSGV_METADATASTRUCT meta) const
{
	assert(category);
	assert(meta);
	CCategory cat (category->plugin,category->category);
	switch (meta->type)
	{
	case META_VERTEX:
		switch (meta->category)
		{
		case LABEL:	
			{
				std::_tstring string;
				meta->valueX = m_meta.GetMetaVertexLabel(cat, string);
			}	
			break;
		case XML_CATEGORY_CODE: 
			{
				std::_tstring xmlCatCode;
				meta->valueX = m_meta.GetMetaVertexXmlCatCode(cat, xmlCatCode);
			}	
			break;
		case XML_ANBICONFILE: 
			{
				std::_tstring xmlIconFile;
				meta->valueX = m_meta.GetMetaVertexXmlAnbIconFile(cat, xmlIconFile);
			}	
			break;
		case HIDE: 
			{
				meta->valueX = (m_meta.GetMetaVertexHide(cat) == VISIBLE_HIDE);
			}	
			break;
		case XML_CATEGORY_LOOKUP: 
			{
				CCategory catFound = m_meta.GetMetaVertexXmlCatLookup((TCHAR*)bstr_t(meta->valueX));
				category->plugin = catFound.GetPlugin().GetGUID();
				category->category = catFound.GetCategory();
			}	
			break;
		default:
			assert(!"Unknown category");
			break;
		}
		break;
	case META_EDGE:
		switch (meta->category)
		{
		case LABEL:
			{
				std::_tstring label;
				meta->valueX = m_meta.GetMetaEdgeLabel(cat, label);
			}
			break;
		case EDGE_COLOR:
			meta->valueX = m_meta.GetMetaEdgeColorNorm(cat);
			break;
		case XML_CATEGORY_CODE: 
			{
				std::_tstring xmlCatCode;
				meta->valueX = m_meta.GetMetaEdgeXmlCatCode(cat, xmlCatCode);
			}	
			break;
		case HIDE: 
			{
				meta->valueX = m_meta.GetMetaEdgeHide(cat);
			}	
			break;
		case XML_CATEGORY_LOOKUP: 
			{
				CCategory catFound = m_meta.GetMetaEdgeXmlCatLookup((TCHAR*)bstr_t(meta->valueX));
				category->plugin = catFound.GetPlugin().GetGUID();
				category->category = catFound.GetCategory();
			}	
			break;
		default:
			assert(!"Unknown category");
			break;
		}
		break;

	case META_SUBGRAPH:
		switch (meta->category)
		{
		case LABEL:	
			{
				std::_tstring string;
				meta->valueX = m_meta.GetMetaSubgraphLabel(cat, string);
			}	
			break;
		case XML_CATEGORY_CODE: 
			{
				std::_tstring xmlCatCode;
				meta->valueX = m_meta.GetMetaSubgraphXmlCatCode(cat, xmlCatCode);
			}	
			break;
		case XML_ANBICONFILE: 
			{
				std::_tstring xmlIconFile;
				meta->valueX = m_meta.GetMetaSubgraphXmlAnbIconFile(cat, xmlIconFile);
			}	
			break;
		case HIDE: 
			{
				meta->valueX = (m_meta.GetMetaSubgraphHide(cat) == VISIBLE_HIDE);
			}	
			break;
		case XML_CATEGORY_LOOKUP: 
			{
				CCategory catFound = m_meta.GetMetaSubgraphXmlCatLookup((TCHAR*)bstr_t(meta->valueX));
				category->plugin = catFound.GetPlugin().GetGUID();
				category->category = catFound.GetCategory();
			}	
			break;
		default:
			assert(!"Unknown category");
			break;
		}
		break;

	case META_CATEGORY:
		switch (meta->category)
		{
		case SPLINES:  
			meta->valueX = m_meta.GetMetaSplines();
			break;
		case DOT_LEFTRIGHT:  
			meta->valueX = m_meta.GetMetaDotLeftRight();
			break;
		case PLUGIN_NAME:
			std::_tstring name;
			meta->valueX = m_meta.GetMetaPluginName(category->plugin, name);
			break;
		}
		break;
	default:
		assert(!"Unknown type");
		break;
	}
}

void CGraphViewCanvas::SetMetaData(const SGV::PSGV_CATEGORYSTRUCT category, const SGV::PSGV_METADATASTRUCT meta)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	assert(category);
	assert(meta);
	CCategory cat (category->plugin,category->category);
	_variant_t value2 = meta->valueX;
	switch (meta->type)
	{
		case META_VERTEX:
			switch (meta->category)
			{
				case FONT_COLOR: 
					m_meta.SetMetaVertexFontColor(cat, value2); 
					break;
				case SEL_FONT_COLOR: 
					m_meta.SetMetaVertexFontColorSel(cat, value2); 
					break;
				case FILL_COLOR: 
					m_meta.SetMetaVertexColorFillNorm(cat, value2); 
					break;
				case BORDER_COLOR: 
					m_meta.SetMetaVertexColorBorderNorm(cat, value2); 
					break;
				case SEL_FILL_COLOR: 
					m_meta.SetMetaVertexColorFillSel(cat, value2); 
					break;
				case SEL_BORDER_COLOR: 
					m_meta.SetMetaVertexColorBorderSel(cat, value2); 
					break;
				case OVER_FILL_COLOR: 
					m_meta.SetMetaVertexColorFillOver(cat, value2); 
					break;
				case OVER_BORDER_COLOR: 
					m_meta.SetMetaVertexColorBorderOver(cat, value2); 
					break;
				case LABEL: 
					assert(value2.vt == VT_BSTR);
					m_meta.SetMetaVertexLabel(cat, bstr_t(value2)); 
					break;
				case HIDE: 
					{
						m_meta.SetMetaVertexHide(cat, value2); 
						if (m_Owner)
							m_Owner->OnVertexCategoryVisibilityChanged(cat);
						VISIBLE vis=value2?VISIBLE_HIDE:VISIBLE_SHOW;
						CUniqueIDVector vec;
						GetVertices(&vec, value2?TRISTATE_TRUE:TRISTATE_FALSE);

						while(vec.MoveNext())
						{
							const CUniqueID& id=vec.Get();

							if(id.GetCategory()==cat)
							{
								IGraphVertex* vertex=GetVertex(id);
								assert(vertex!=0);
								vertex->SetVisible(vis);
							}
						}

						InvalidateLayout();
					}
					break;
				case XML_CATEGORY_CODE: 
					m_meta.SetMetaVertexXmlCatCode(cat, bstr_t(value2)); 
					break;
				case XML_ANBICONFILE: 
					m_meta.SetMetaVertexXmlAnbIconFile(cat, bstr_t(value2)); 
					break;
				default: 
					assert(!"Unknown meta category");
					break;
			}
			break;

		case META_EDGE:
			switch (meta->category)
			{
				case EDGE_COLOR: 
					m_meta.SetMetaEdgeColorNorm(cat, value2); 
					break;
				case EDGE_SHORTEST_PATH_COLOR: 
					m_meta.SetMetaEdgeColorShortest(cat, value2); 
					break;
				case WIDTH: 
					m_meta.SetMetaEdgeWidth(cat, value2);
					break;
				case HIDE: 
					{
						m_meta.SetMetaEdgeHide(cat, value2);
						VISIBLE vis=value2?VISIBLE_HIDE:VISIBLE_SHOW;
						CUniqueIDVector vec;
						GetEdges(&vec, value2?TRISTATE_TRUE:TRISTATE_FALSE);

						while(vec.MoveNext())
						{
							const CUniqueID& id=vec.Get();

							if(id.GetCategory()==cat)
							{
								IGraphEdge* edge=GetEdge(id);
								assert(edge!=0);
								edge->SetVisible(vis);
								if (vis==VISIBLE_SHOW)
								{
									edge->GetSource()->SetVisible(vis);
									edge->GetTarget()->SetVisible(vis);
								}
							}
						}

						InvalidateLayout();
					}
					break;
				case XML_CATEGORY_CODE: 
					m_meta.SetMetaEdgeXmlData(cat, bstr_t(value2));
					break;
				case LABEL:
					m_meta.SetMetaEdgeLabel(cat, bstr_t(value2));
					break;
				default: 
					assert(!"Unknown meta category");
					break;
			}
			break;

		case META_SUBGRAPH:
			switch (meta->category)
			{
				case FONT_COLOR: 
					m_meta.SetMetaSubgraphFontColor(cat, value2); 
					break;
				case SEL_FONT_COLOR: 
					m_meta.SetMetaSubgraphFontColorSel(cat, value2); 
					break;
				case FILL_COLOR: 
					m_meta.SetMetaSubgraphColorFillNorm(cat, value2); 
					break;
				case BORDER_COLOR: 
					m_meta.SetMetaSubgraphColorBorderNorm(cat, value2); 
					break;
				case SEL_FILL_COLOR: 
					m_meta.SetMetaSubgraphColorFillSel(cat, value2); 
					break;
				case SEL_BORDER_COLOR: 
					m_meta.SetMetaSubgraphColorBorderSel(cat, value2); 
					break;
				case OVER_FILL_COLOR: 
					m_meta.SetMetaSubgraphColorFillOver(cat, value2); 
					break;
				case OVER_BORDER_COLOR: 
					m_meta.SetMetaSubgraphColorBorderOver(cat, value2); 
					break;
				case LABEL: 
					assert(value2.vt == VT_BSTR);
					m_meta.SetMetaSubgraphLabel(cat, bstr_t(value2)); 
					break;
				case HIDE: 
					{
						m_meta.SetMetaSubgraphHide(cat, value2); 
						if (m_Owner)
							m_Owner->OnSubgraphCategoryVisibilityChanged(cat);
						VISIBLE vis=value2?VISIBLE_HIDE:VISIBLE_SHOW;
						CUniqueIDVector vec;
						GetSubgraphs(&vec, value2?TRISTATE_TRUE:TRISTATE_FALSE);

						while(vec.MoveNext())
						{
							const CUniqueID& id=vec.Get();

							if(id.GetCategory()==cat)
							{
								IGraphSubgraph* sg=GetSubgraph(id);
								assert(sg!=0);
								sg->SetVisible(vis);
							}
						}

						InvalidateLayout();
					}
					break;
				case XML_CATEGORY_CODE: 
					m_meta.SetMetaSubgraphXmlCatCode(cat, bstr_t(value2)); 
					break;
				case XML_ANBICONFILE: 
					m_meta.SetMetaSubgraphXmlAnbIconFile(cat, bstr_t(value2)); 
					break;
				default: 
					assert(!"Unknown meta category");
					break;
			}
			break;

		case META_CATEGORY:
			switch (meta->category)
			{
			case SPLINES:  
				m_meta.SetMetaSplines(value2);
				break;
			case DOT_LEFTRIGHT:  
				m_meta.SetMetaDotLeftRight(value2);
				break;
			case PLUGIN_NAME:
				m_meta.SetMetaPluginName(category->plugin, bstr_t(value2));
				break;
			case AVOID_OBSTACLES:
				m_meta.AvoidObstacles(value2);
				break;
			default:
				assert(!"Unknown meta category");
				break;
			}
			break;
		default:
			assert(!"Unknown meta type");
			break;
	}
}

void CGraphViewCanvas::SetVertexVisible(const CUniqueID& id, bool visible)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	IGraphVertex* vertex=GetVertex(id);
	if(vertex!=0)
	{
		bool oldVal = m_meta.IsVisible(vertex);
		
		vertex->SetVisible(visible?VISIBLE_SHOW:VISIBLE_HIDE);

		if (visible != oldVal)
		{
			if(m_Owner) 
				m_Owner->OnVertexVisibilityChanged(vertex->GetID());
			InvalidateLayout();
		}
	}
}

bool CGraphViewCanvas::GetVertexVisible(const CUniqueID& id)
{
	bool vis=false;
	IGraphVertex* vertex=GetVertex(id);
	if(vertex!=0)
		vis=m_meta.IsVisible(vertex);
	return vis;
}

bool CGraphViewCanvas::GetVertexPinned(const CUniqueID& id)
{
	bool pinned=false;
	IGraphVertex* vertex=GetVertex(id);
	assert(vertex!=0);
	if(vertex!=0)
	{
		CComQIPtr<ILegacyRenderer> lr=m_canvas->GetRenderer(vertex);
		if(lr!=0)
			pinned=lr->GetPinned();
	}
	return pinned;
}

void CGraphViewCanvas::SetVertexPinned(const CUniqueID& id, bool pinned)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	IGraphVertex* vertex=GetVertex(id);
	assert(vertex!=0);
	if(vertex!=0)
	{
		CComQIPtr<ILegacyRenderer> lr=m_canvas->GetRenderer(vertex);
		if(lr!=0)
		{
			lr->SetPinned(pinned);
			InvalidateLayout();
		}
	}
}

void CGraphViewCanvas::SetMetaVertexImage(const CCategory& cat, const ImageInfo& iinfoIn, unsigned int overlay)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);

	m_meta.SetVertexImageInfo(cat, iinfoIn, overlay);
}

void CGraphViewCanvas::SetMetaEdgeImage(const CCategory& cat, const ImageInfo& iinfoIn, unsigned int overlay)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	m_meta.SetEdgeImageInfo(cat, iinfoIn, overlay);
}

void CGraphViewCanvas::SetVertexImage(const CUniqueID& id, const ImageInfo& iinfoIn, unsigned int overlay)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	IGraphVertex * vertex = GetVertex(id);
	if (vertex != NULL)
	{
		vertex->SetImage(iinfoIn, overlay);
		InvalidateLayout();
	}
}

GraphTypes::IBitmap* CGraphViewCanvas::GetVertexImage(const CUniqueID& id, IMAGE_POSITION pos, unsigned int overlay, bool vertexSpecific)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	CComPtr<GraphTypes::IBitmap> ibmp;
	const ImageInfoMaps* imaps = NULL;
	ImageInfoMaps::const_iterator itrmaps;
	MapImageInfo::const_iterator itrmap;

	IGraphVertex * vertex = GetVertex(id);

	if (vertex != NULL)
	{
		imaps=vertex->GetImages();
		itrmaps=imaps->find(static_cast<unsigned>(pos));
		if(itrmaps!=imaps->end())
		{
			const MapImageInfo& imap=itrmaps->second;
			MapImageInfo::const_iterator itrmap=imap.find(overlay);

			if(itrmap!=imap.end())
				ibmp=itrmap->second.image;
		}
	}
	if (ibmp == NULL && !vertexSpecific)
	{
		imaps=m_meta.GetVertexImageInfo(id.GetCategory());
		if(imaps!=0)
		{
			itrmaps=imaps->find(static_cast<unsigned>(pos));
			if(itrmaps!=imaps->end())
			{
				const MapImageInfo& imap=itrmaps->second;
				itrmap=imap.find(overlay);

				if(itrmap!=imap.end())
					ibmp=itrmap->second.image;
			}
		}
	}
	return ibmp;
}

void CGraphViewCanvas::SetEdgeImage(const CUniqueID& id, const ImageInfo& iinfoIn, unsigned int overlay)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	IGraphEdge * edge = GetEdge(id);
	if (edge != NULL)
	{
		edge->SetImage(iinfoIn, overlay);
		InvalidateLayout();
	}
}

GraphTypes::IBitmap* CGraphViewCanvas::GetEdgeImage(const CUniqueID& id, IMAGE_POSITION pos, unsigned int overlay, bool edgeSpecific)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	CComPtr<GraphTypes::IBitmap> ibmp;

	IGraphEdge* edge = GetEdge(id);
	if( edge != NULL)
	{
		const ImageInfoMaps* imaps=edge->GetImages();
		ImageInfoMaps::const_iterator itrmaps=imaps->find(static_cast<unsigned>(pos));
		if(itrmaps!=imaps->end())
		{
			const MapImageInfo& imap=itrmaps->second;
			MapImageInfo::const_iterator itrmap=imap.find(overlay);

			if(itrmap!=imap.end())
				ibmp=itrmap->second.image;
		}
		if (ibmp == NULL && !edgeSpecific)
		{
			imaps=m_meta.GetEdgeImageInfo(id.GetCategory());
			if(imaps!=0)
			{
				itrmaps=imaps->find(static_cast<unsigned>(pos));
				if(itrmaps!=imaps->end())
				{
					const MapImageInfo& imap=itrmaps->second;
					MapImageInfo::const_iterator itrmap=imap.find(overlay);

					if(itrmap!=imap.end())
						ibmp=itrmap->second.image;
				}
			}
		}
	}

	return ibmp;
}

bool CGraphViewCanvas::GetVertexImageVisible(const CUniqueID& id, IMAGE_POSITION pos)
{
	bool vis=false;
	IGraphVertex* v=GetVertex(id);
	if(v!=0)
		vis=m_meta.IsImageVisible(v, pos);
	return vis;
}

void CGraphViewCanvas::SetVertexImageVisible(const CUniqueID& id, IMAGE_POSITION pos, bool visible)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	IGraphVertex* vertex=GetVertex(id);
	if(vertex!=0)
	{
		vertex->SetImageVisible(pos, visible?VISIBLE_SHOW:VISIBLE_HIDE);
		InvalidateLayout();
	}
}

bool CGraphViewCanvas::GetMetaVertexImageVisible(const CCategory& cat, IMAGE_POSITION pos)
{
	return m_meta.GetVertexImageVisible(cat, pos)==VISIBLE_SHOW;
}

void CGraphViewCanvas::SetMetaVertexImageVisible(const CCategory& cat, IMAGE_POSITION pos, bool visible)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	VISIBLE vis=visible?VISIBLE_SHOW:VISIBLE_HIDE;
	m_meta.SetVertexImageVisible(cat, pos, vis);

	CUniqueIDVector vec;
	GetVertices(&vec, visible?TRISTATE_FALSE:TRISTATE_TRUE);

	while(vec.MoveNext())
	{
		const CUniqueID& id=vec.Get();

		if(id.GetCategory()==cat)
		{
			IGraphVertex* vertex=GetVertex(id);
			assert(vertex!=0);
			vertex->SetImageVisible(pos, vis);
		}
	}
	InvalidateLayout();
}

void CGraphViewCanvas::SetEdgeVisible(const CUniqueID& id, bool visible)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	IGraphEdge* edge=GetEdge(id);
	if(edge!=0)
	{
		edge->SetVisible(visible?VISIBLE_SHOW:VISIBLE_HIDE);
		InvalidateLayout();
	}
}

bool CGraphViewCanvas::GetEdgeVisible(const CUniqueID& id)
{
	bool vis=false;
	IGraphEdge* edge=GetEdge(id);
	if(edge!=0)
		vis=m_meta.IsVisible(edge);
	return vis;
}

bool CGraphViewCanvas::GetEdgeImageVisible(const CUniqueID& id, IMAGE_POSITION pos)
{
	bool vis=false;
	IGraphEdge* edge=GetEdge(id);
	if(edge!=0)
		vis=m_meta.IsImageVisible(edge, pos);
	return vis;
}

void CGraphViewCanvas::SetEdgeImageVisible(const CUniqueID& id, IMAGE_POSITION pos, bool visible)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	IGraphEdge* edge=GetEdge(id);
	if(edge!=0)
	{
		edge->SetImageVisible(pos, visible?VISIBLE_SHOW:VISIBLE_HIDE);
		InvalidateLayout();
	}
}

bool CGraphViewCanvas::GetMetaEdgeImageVisible(const CCategory& cat, IMAGE_POSITION pos)
{
	return m_meta.GetEdgeImageVisible(cat, pos)==VISIBLE_SHOW;
}

void CGraphViewCanvas::SetMetaEdgeImageVisible(const CCategory& cat, IMAGE_POSITION pos, bool visible)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	VISIBLE vis=visible?VISIBLE_SHOW:VISIBLE_HIDE;
	m_meta.SetEdgeImageVisible(cat, pos, vis);

	CUniqueIDVector vec;
	GetEdges(&vec, visible?TRISTATE_FALSE:TRISTATE_TRUE);

	while(vec.MoveNext())
	{
		const CUniqueID& id=vec.Get();
		if(id.GetCategory()==cat)
		{
			IGraphEdge* edge=GetEdge(id);
			assert(edge!=0);
			edge->SetImageVisible(pos, vis);
		}
	}
	InvalidateLayout();
}

void CGraphViewCanvas::GetEdgeColor(const CUniqueID& uid, GraphTypes::Color& color) const
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	IGraphEdge* edge=GetEdge(uid);
	if(edge!=0)
		color=edge->GetColor();
}

void CGraphViewCanvas::SetEdgeColor(const CUniqueID& uid, const GraphTypes::Color& color)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	IGraphEdge* edge=GetEdge(uid);
	if(edge!=0)
	{
		edge->SetColor(color);
		InvalidateLayout();
	}
}

bool CGraphViewCanvas::GetEdgeSource(const CUniqueID& uid, CUniqueID& src)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	IGraphEdge* edge=GetEdge(uid);
	IGraphItem* source=0;
	if(edge!=0)
		source=edge->GetSource();
	if(source)	
	{
		src=source->GetID();
		return true;
	}
	else
		return false;
}

bool CGraphViewCanvas::GetEdgeTarget(const CUniqueID& uid, CUniqueID& tgt)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	IGraphEdge* edge=GetEdge(uid);
	IGraphItem* target=0;
	if(edge!=0)
		target=edge->GetTarget();
	if(target)	
	{
		tgt=target->GetID();
		return true;
	}
	else
		return false;
}

void CGraphViewCanvas::SetSubgraphVisible(const CUniqueID& id, bool visible)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	IGraphSubgraph* sg=GetSubgraph(id);
	if(sg!=0)
	{
		bool oldVal = m_meta.IsVisible(sg);
		
		sg->SetVisible(visible?VISIBLE_SHOW:VISIBLE_HIDE);

		CComPtr<IGraphItemIterator> itr = sg->GetChildren(); 
		CComQIPtr<IGraphVertex> v;
		while(itr->IsValidItem())
		{
			v=itr->Get();
			ATLASSERT(v);
			if(v)
				v->SetVisible(visible?VISIBLE_SHOW:VISIBLE_HIDE);
			itr->NextItem();
		}

		if (visible != oldVal)
		{
			if(m_Owner) 
				m_Owner->OnSubgraphVisibilityChanged(sg->GetID());
			InvalidateLayout();
		}
	}
}

bool CGraphViewCanvas::GetSubgraphVisible(const CUniqueID& id)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	bool vis=false;
	IGraphSubgraph* sg=GetSubgraph(id);
	if(sg!=0)
		vis=m_meta.IsVisible(sg);
	return vis;
}

bool CGraphViewCanvas::GetSubgraphPinned(const CUniqueID& id) const
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	bool pinned=false;
	IGraphSubgraph* sg=GetSubgraph(id);
	assert(sg!=0);
	if(sg!=0)
	{
		CComQIPtr<ILegacySubgraphRenderer> lsgr=m_canvas->GetRenderer(sg);
		if(lsgr!=0)
			pinned=lsgr->GetPinned();
	}
	return pinned;
}

void CGraphViewCanvas::SetSubgraphPinned(const CUniqueID& id, bool pinned)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	IGraphSubgraph* sg=GetSubgraph(id);
	assert(sg!=0);
	if(sg!=0)
	{
		CComQIPtr<ILegacySubgraphRenderer> lsgr=m_canvas->GetRenderer(sg);
		if(lsgr!=0)
		{
			lsgr->SetPinned(pinned);
			InvalidateLayout();
		}
	}
}

bool CGraphViewCanvas::GetSubgraphMinimized(const CUniqueID& id) const
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	bool minimized=false;
	IGraphSubgraph* sg=GetSubgraph(id);
	assert(sg!=0);
	if(sg!=0)
	{
		if (CComQIPtr<IAttSubgraphRenderer> lsgr=m_canvas->GetRenderer(sg))
		{
			minimized=lsgr->IsMinimized();
		}
	}
	return minimized;
}

bool CGraphViewCanvas::EnsureVisible(const CUniqueID& id) const
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	bool retVal = false;
	if (IGraphVertex * v = GetVertex(id))
		retVal = m_edgeFilter->EnsureVisible(v);
	else if (IGraphEdge * e = GetEdge(id))
		retVal = m_edgeFilter->EnsureVisible(e);
	else if (IGraphSubgraph * sg = GetSubgraph(id))
		retVal = m_edgeFilter->EnsureVisible(sg);
	return retVal;
}

void CGraphViewCanvas::SetSubgraphMinimized(const CUniqueID& id, bool minimized, bool recurse)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	IGraphSubgraph* sg=GetSubgraph(id);
	assert(sg!=0);
	if(sg!=0)
	{
		//IGraphItemVector children;
		//sg->GetChildren(children);

		//for(IGraphItemVector::iterator itr=children.begin(); itr!=children.end(); ++itr)
		//{
		//	CComPtr<IGraphItem> gi = *itr;
		//	if(CComQIPtr<IGraphVertex> child_v = gi.p)
		//		SetVertexVisible(child_v->GetID(), !minimized);
		//	else if(CComQIPtr<IGraphSubgraph> child_sg = gi.p)
		//	{
		//		if (recurse)
		//			SetSubgraphMinimized(child_sg->GetID(), minimized);
		//	}
		//}
		if (CComQIPtr<IAttSubgraphRenderer> lsgr=m_canvas->GetRenderer(sg))
		{
			lsgr->SetWindowState(!minimized ? IListWindowRenderer::WINDOW_SIZE_NORM : IListWindowRenderer::WINDOW_SIZE_MIN);
			InvalidateLayout();
		}
	}
}

void CGraphViewCanvas::SetSubgraphImage(const CUniqueID& id, const ImageInfo& iinfo, unsigned int overlay)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	IGraphSubgraph* sg = GetSubgraph(id);
	if (sg != NULL)
	{
		sg->SetImage(iinfo, overlay);
		InvalidateLayout();
	}
}

GraphTypes::IBitmap* CGraphViewCanvas::GetSubgraphImage(const CUniqueID& id, IMAGE_POSITION pos, unsigned int overlay, bool subgraphSpecific)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	CComPtr<GraphTypes::IBitmap> ibmp;
	const ImageInfoMaps* imaps = NULL;
	ImageInfoMaps::const_iterator itrmaps;
	MapImageInfo::const_iterator itrmap;

	IGraphSubgraph* sg = GetSubgraph(id);

	if (sg != NULL)
	{
		imaps=sg->GetImages();
		itrmaps=imaps->find(static_cast<unsigned>(pos));
		if(itrmaps!=imaps->end())
		{
			const MapImageInfo& imap=itrmaps->second;
			MapImageInfo::const_iterator itrmap=imap.find(overlay);

			if(itrmap!=imap.end())
				ibmp=itrmap->second.image;
		}
	}
	if (ibmp == NULL && !subgraphSpecific)
	{
		imaps=m_meta.GetSubgraphImageInfo(id.GetCategory());
		if(imaps!=0)
		{
			itrmaps=imaps->find(static_cast<unsigned>(pos));
			if(itrmaps!=imaps->end())
			{
				const MapImageInfo& imap=itrmaps->second;
				itrmap=imap.find(overlay);

				if(itrmap!=imap.end())
					ibmp=itrmap->second.image;
			}
		}
	}
	return ibmp;
}

bool CGraphViewCanvas::GetSubgraphImageVisible(const CUniqueID& id, IMAGE_POSITION pos)
{
	bool vis=false;
	IGraphSubgraph* sg=GetSubgraph(id);
	if(sg!=0)
		vis=m_meta.IsImageVisible(sg, pos);
	return vis;
}

void CGraphViewCanvas::SetSubgraphImageVisible(const CUniqueID& id, IMAGE_POSITION pos, bool visible)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	IGraphSubgraph* sg=GetSubgraph(id);
	if(sg!=0)
	{
		sg->SetImageVisible(pos, visible?VISIBLE_SHOW:VISIBLE_HIDE);
		InvalidateLayout();
	}
}

void CGraphViewCanvas::SetMetaSubgraphImage(const CCategory& cat, const ImageInfo& iinfo, unsigned int overlay)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	m_meta.SetSubgraphImageInfo(cat, iinfo, overlay);
}

bool CGraphViewCanvas::GetMetaSubgraphImageVisible(const CCategory& cat, IMAGE_POSITION pos)
{
	return m_meta.GetSubgraphImageVisible(cat, pos)==VISIBLE_SHOW;
}

void CGraphViewCanvas::SetMetaSubgraphImageVisible(const CCategory& cat, IMAGE_POSITION pos, bool visible)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	VISIBLE vis=visible?VISIBLE_SHOW:VISIBLE_HIDE;
	m_meta.SetSubgraphImageVisible(cat, pos, vis);

	CUniqueIDVector vec;
	GetSubgraphs(&vec, visible?TRISTATE_FALSE:TRISTATE_TRUE);

	while(vec.MoveNext())
	{
		const CUniqueID& id=vec.Get();

		if(id.GetCategory()==cat)
		{
			IGraphSubgraph* sg=GetSubgraph(id);
			assert(sg!=0);
			sg->SetImageVisible(pos, vis);
		}
	}
	InvalidateLayout();
}

void CGraphViewCanvas::SetEdgeDateRange(const CUniqueID& uid, const std::_tstring & from, const std::_tstring & to)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	IGraphEdge* edge=GetEdge(uid);
	if(edge!=0)
	{
		boost::gregorian::date fromDate, toDate;
		boost::posix_time::ptime fromTime, toTime;
		UTCToDateTime(from.c_str(), fromDate, fromTime);
		UTCToDateTime(to.c_str(), toDate, toTime);
		ATLASSERT(fromTime<=toTime);
		edge->SetDateRange(boost::posix_time::time_period(fromTime, toTime));
		InvalidateLayout();
	}
}

void CGraphViewCanvas::SetApplicationMode(AppMode mode)
{
	m_canvas->SetApplicationMode(mode);
	switch (mode)
	{
	case AppModeQBGraph:
		ShowOverview(false);
		EmulateDecoration(true);
		SetOverviewOpacity(200);
		SetEdgeLabelVisible(true);
		m_meta.SetMetaSplines(true);
		m_meta.SetMetaDotLeftRight(false);
		SetRotation(0);
		ShowOverview(true);
		break;
	}
}

AppMode CGraphViewCanvas::GetApplicationMode() const
{
	return m_canvas->GetApplicationMode();
}

void CGraphViewCanvas::SetApplicationVisMode(AppVisMode mode)
{
	AppVisMode oldmode=m_canvas->GetApplicationVisMode();
	m_canvas->SetApplicationVisMode(mode);
	if(oldmode != mode)
	{
		if(m_canvas->GetApplicationMode()==AppModeRelavint)
		{
			CWindow parent=baseClass::GetParent();
			CRect rc;
			parent.GetClientRect(&rc);

			if(mode==RelavintTimeline)
			{
				int tlheight=m_timeline->Height();
				m_timeline->MoveWindow(0, 0, rc.Width(), tlheight);
				MoveWindow(0, tlheight, rc.Width(), rc.Height()-tlheight);
			}
			else
			{
				MoveWindow(0, 0, rc.Width(), rc.Height());
			}

			if(mode==RelavintTimeline)
			{
				using namespace boost::posix_time;
				using namespace boost::gregorian;

				ptime begin(date(2000,1,1), hours(0));
				ptime end(date(2000,1,1), hours(0));
				time_period totalSpan(begin, end);

				// calc total time span

				bool first = true;
				CComPtr<IEdgeIterator> itr = m_graph->GetEdgeIterator();
				for ( ; itr->IsValidItem(); itr->NextItem())
				{
					CComPtr<IGraphEdge> e = itr->Get();
					ATLASSERT(e);
					if (first)
					{
						first = false;
						totalSpan = e->GetDateRange();
					}
					else
					{
						totalSpan=totalSpan.span(e->GetDateRange());
					}
				}

				ATLASSERT(!totalSpan.is_null()); // really layout should have taken care of this
				ATLASSERT(totalSpan.begin()<=totalSpan.end());

				begin=totalSpan.begin();
				end=totalSpan.end();

				// converting from useless boost mindtrips to Win32 time

				tm begintm=to_tm(begin);
				tm endtm=to_tm(end);

				time_t begin_t=::mktime(&begintm);
				time_t end_t=::mktime(&endtm);

				FILETIME ftbegin;
				FILETIME ftend;

				LONGLONG ll = Int32x32To64(begin_t, 10000000) + 116444736000000000;
				ftbegin.dwLowDateTime=(DWORD)ll;
				ftbegin.dwHighDateTime=ll>>32;

				ll = Int32x32To64(end_t, 10000000) + 116444736000000000;
				ftend.dwLowDateTime=(DWORD)ll;
				ftend.dwHighDateTime=ll>>32;

				m_timeline->SetTimeRange(ftbegin, ftend);
			}

		}
		InvalidateLayout();
	}
}

AppVisMode CGraphViewCanvas::GetApplicationVisMode() const
{
	return m_canvas->GetApplicationVisMode();
}

void CGraphViewCanvas::SetApplicationZoomMode(AppZoomMode mode)
{
	m_canvas->SetApplicationZoomMode(mode);
}

AppZoomMode CGraphViewCanvas::GetApplicationZoomMode() const
{
	return m_canvas->GetApplicationZoomMode();
}

void CGraphViewCanvas::FindGraphItemByID()
{
	ATLASSERT(m_canvas->GetApplicationMode()==AppModeQBGraph);
	if(m_canvas->GetApplicationMode()==AppModeQBGraph)
		QBUI::LaunchFindDialog(this);
}

void CGraphViewCanvas::SetPropertyAttribute(META_TYPE entityType, const CCategory& category, const int id, PROPERTY_ATTR attr, const CComVariant& value)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	assert(entityType != META_UNKNOWN);
	m_meta.SetPropertyAttribute(entityType, category, id, attr, value);
}

unsigned CGraphViewCanvas::GetPropertyAttributes(IPropAttributeContainer* storage, META_TYPE entityType, const CCategory& category, const int id)
{
	assert(entityType != META_UNKNOWN);
	return m_meta.GetPropertyAttributes(storage, entityType, category, id);
}

unsigned int CGraphViewCanvas::GetPropertyAttributeId(META_TYPE entityType, const CCategory& category, PROPERTY_ATTR attr, const CComVariant& value, bool autoGenId)
{
	assert(entityType != META_UNKNOWN);
	return m_meta.GetPropertyAttributeId(entityType, category, attr, value, autoGenId);
}

// TODO: perhaps meta can implement this more efficiently...
bool CGraphViewCanvas::GetPropertyAttribute(META_TYPE type, const CCategory& category, const int id, PROPERTY_ATTR attr, _variant_t& retVal)
{
	bool itemFound = false;
	CPropAttributeVector prop_attrs;
	GetPropertyAttributes(&prop_attrs, type, category, id);
	while (prop_attrs.MoveNext())
	{
		AttrValue prop_attr(prop_attrs.Get().first);
		if (prop_attr.attr == attr)
		{
			retVal = prop_attr.value;
			itemFound = true;
			break;
		}
	}
	return itemFound;
}

IGraphSubgraph * CGraphViewCanvas::GetSubgraph(const CUniqueID & id) const
{
	return m_graph->GetSubgraph(id);
}

IGraphVertex * CGraphViewCanvas::GetVertex(const CUniqueID & id) const
{
	return m_graph->GetVertex(id);
}

IGraphVertex * CGraphViewCanvas::GetVertex(const CCategory & pCat, const TCHAR* pId) const
{
	CUniqueID id(pCat, (const TCHAR *)pId);
	return GetVertex(id);
}

IGraphSubgraph * CGraphViewCanvas::AddSubgraph(const CUniqueID & id, const TCHAR* label, IGraphSubgraph * Subgraph, RendererTypes rtype)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	IGraphSubgraph * retVal = Subgraph ? Subgraph->CreateSubgraph(id, label) : m_graph->CreateSubgraph(id, label);
	ATLASSERT(retVal);
	if(retVal)
	{
		m_canvas->CreateGraphItemRenderer(retVal, rtype);
	}
	return retVal;
}

void CGraphViewCanvas::RemoveSubgraph(const CUniqueID& id)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	CComPtr<IGraphSubgraph> sg = m_graph->GetSubgraph(id);
	assert(sg!=0);
	RemoveSubgraph(sg);
}

void CGraphViewCanvas::RemoveSubgraph(IGraphSubgraph * sg)
{
	if(sg==0) 
	{
		return;
	}

	clib::recursive_mutex::scoped_lock lock(m_mutex);

	IGraphItemVector children;
	sg->GetChildren(children);

	for(IGraphItemVector::iterator itr=children.begin(); itr!=children.end(); ++itr)
	{
		CComPtr<IGraphItem> gi = *itr;
		CComQIPtr<IGraphVertex> child_v = gi.p;
		if(child_v)
		{
			RemoveVertex(child_v);
		}
		CComQIPtr<IGraphSubgraph> child_sg = gi.p;
		if(child_sg)
		{
			RemoveSubgraph(child_sg);
		}
	}

	m_canvas->OnRemoveGraphItem(sg);
	sg->Delete();
	InvalidateLayout();
}

void CGraphViewCanvas::RandomizePosition(GraphTypes::PointF& pos)
{
#define DELTA_MAX 200
	GraphTypes::PointF delta;
	delta.x = static_cast<REAL>(MulDiv(rand(), DELTA_MAX, RAND_MAX) - DELTA_MAX / 2);
	delta.y = static_cast<REAL>(MulDiv(rand(), DELTA_MAX, RAND_MAX) - DELTA_MAX / 2);
	m_canvas->ScaleTransform(delta);
	pos.x += delta.x;
	pos.y += delta.y;
}

std::pair<IGraphVertex *, bool> CGraphViewCanvas::AddVertex(const CUniqueID & id, const GraphTypes::PointF & pos, const TCHAR* label, bool randomize, IGraphSubgraph * Subgraph, RendererTypes rtype)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	GraphTypes::PointF transpos = pos;
	if (randomize)
		RandomizePosition(transpos);
	m_canvas->RAllTransform(transpos);
	IGraphVertex* retVal = m_graph->GetVertex(id);
	bool retVal2 = false;
	if (retVal == NULL)
	{
		if (Subgraph)
		{
			retVal = Subgraph->CreateVertex(id);
		}
		else
		{
			retVal = m_graph->CreateVertex(id);
		}

		m_canvas->CreateGraphItemRenderer(retVal, rtype);
		std::_tstring lt=label;
		InitExternalMaps(retVal, transpos);
		retVal->SetLabel(lt.c_str());
		retVal2 = true;
		InvalidateLayout();
	}
	return std::make_pair(retVal, retVal2);
}

std::pair<IGraphVertex *, bool> CGraphViewCanvas::AddVertex(const CUniqueID & id, IGraphSubgraph * Subgraph, RendererTypes rtype)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	RECT r;
	GetClientRect(&r);
	RectF rectF(r);
	m_canvas->RDpiTransform(rectF);
	GraphTypes::PointF close_pos(rectF.Width / 2.0f, rectF.Height / 2.0f);
	return AddVertex(id, close_pos, _T(""), true, Subgraph, rtype);
}

std::pair<IGraphVertex *, bool> CGraphViewCanvas::AddVertex(const CUniqueID & id, const TCHAR* label, RendererTypes rtype)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	RECT r;
	GetClientRect(&r);
	RectF rectF(r);
	m_canvas->RDpiTransform(rectF);
	GraphTypes::PointF close_pos(rectF.Width / 2.0f, rectF.Height / 2.0f);
	return AddVertex(id, close_pos, label, true, NULL, rtype);
}

std::pair<IGraphVertex *, bool> CGraphViewCanvas::AddVertex(const CCategory & cat, const TCHAR* id, const TCHAR* label, RendererTypes rtype)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	return AddVertex(CUniqueID(cat, id), label, rtype);
}

std::pair<IGraphVertex *, bool> CGraphViewCanvas::AddVertex(const CCategory & cat, const TCHAR* id, const GraphTypes::PointF & pos, const TCHAR* label, RendererTypes rtype)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	return AddVertex(CUniqueID(cat, static_cast<const TCHAR *>(id)), pos, label, false, NULL, rtype);
}

void CGraphViewCanvas::RemoveVertex(const CUniqueID& v)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	CComPtr<IGraphVertex> iv=GetVertex(v);
	RemoveVertex(iv);
}

void CGraphViewCanvas::RemoveVertex(IGraphVertex * v)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	if (v == NULL)
		return;

	{
		CBuildingEdgeVisibility building(m_buildingEdgeVisibility);
		m_canvas->OnRemoveGraphItem(v);

		IGraphEdgeVector edges;
		CComPtr<IEdgeIterator> itr = v->GetAdjacentEdges();
		while(itr->IsValidItem())
		{
			CComPtr<IGraphEdge> edge = itr->Get();
			std::_tstring edgeID = edge->GetID().GetID();
			//ATLTRACE(_T("Removing %s\n"), edgeID.c_str());
			RemoveEdge(edge);
			//ATLTRACE(_T("Removed %s\n"), edgeID.c_str());
			itr = v->GetAdjacentEdges();
		}

		m_layout_spring1->remove(v);
		m_layout_circle->remove(v);
		m_layout_timeline->remove(v);
#ifdef LAYOUT_LEE
		m_layout_lee->remove(v);
#endif
		m_layout_neato->remove(v);
		m_layout_fdp->remove(v);
		m_layout_twopi->remove(v);
		m_layout_dot->remove(v);
		m_layout_circo->remove(v);
		m_layout_null->remove(v);

		v->Delete();
	}

	InvalidateLayout();
}

IGraphEdge * CGraphViewCanvas::AddEdge(const CUniqueID& edge_id, const CUniqueID & from, const CUniqueID & to, const TCHAR* label, RendererTypes rtype)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	if (from.isEmpty() || to.isEmpty() || edge_id.isEmpty())
		return NULL;

	CComPtr<IGraphEdge> retVal = m_graph->GetEdge(edge_id);

	if (retVal == NULL)
	{
		retVal = m_graph->CreateEdge(edge_id, from, to);
		assert(retVal);
		retVal->SetWeight(1);
		m_layout_spring1->append(retVal);
		m_layout_circle->append(retVal);
		m_layout_timeline->append(retVal);
#ifdef LAYOUT_LEE
		m_layout_lee->append(retVal);
#endif
		m_layout_neato->append(retVal);
		m_layout_fdp->append(retVal);
		m_layout_twopi->append(retVal);
		m_layout_dot->append(retVal);
		m_layout_circo->append(retVal);
		m_layout_null->append(retVal);
		retVal->SetLabel(label);
		m_canvas->CreateGraphItemRenderer(retVal, rtype);			
	}
	return retVal;
}

// Depracated AddEdge method
IGraphEdge * CGraphViewCanvas::AddEdge(const CCategory& cat, const CUniqueID & fromID, const CUniqueID & toID, const TCHAR* label, RendererTypes rtype)
{
	CUniqueID edge_id(SGV::CreateEdgeID(cat, fromID, toID));
	return AddEdge(edge_id, fromID, toID, label, rtype);
}

// Depracated AddEdge method
IGraphEdge * CGraphViewCanvas::AddEdge(const CUniqueID& edge_id, const CCategory & categoryFrom, const TCHAR* idFrom, const CCategory & categoryTo, const TCHAR* idTo, const TCHAR* label, RendererTypes rtype)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	return AddEdge(edge_id, CUniqueID(categoryFrom, idFrom), CUniqueID(categoryTo, idTo), label, rtype);
}

IGraphEdge * CGraphViewCanvas::GetEdge(const CUniqueID & id) const
{
	return m_graph->GetEdge(id);
}

//IGraphEdge * CGraphViewCanvas::GetEdge(const CCategory & pCat, IGraphVertex * from, IGraphVertex * to) const
//{
//	//TODO Remove
//	CUniqueID edge_id = SGV::CreateEdgeID(pCat, from->GetID(), to->GetID());
//	return m_graph->GetEdge(edge_id);
//}
//
//IGraphEdge * CGraphViewCanvas::GetEdge(const CCategory & category, const CCategory & categoryFrom, const TCHAR* idFrom, const CCategory & categoryTo, const TCHAR* idTo) const
//{
//	return GetEdge(category, GetVertex(categoryFrom, idFrom), GetVertex(categoryTo, idTo));
//}
//
//IGraphEdge * CGraphViewCanvas::GetEdge(const CCategory & category, const CUniqueID & from, const CUniqueID & to) const
//{
//	return GetEdge(category, GetVertex(from), GetVertex(to));
//}

void CGraphViewCanvas::RemoveEdge(const CUniqueID & id)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	CComPtr<IGraphEdge> e = GetEdge(id);
	assert(e!=0);
	RemoveEdge(e);
}

void CGraphViewCanvas::RemoveEdge(IGraphEdge * e)
{
	if(e==0) return;

	clib::recursive_mutex::scoped_lock lock(m_mutex);
	{
		CBuildingEdgeVisibility building(m_buildingEdgeVisibility);

		m_layout_spring1->remove(e);
		m_layout_circle->remove(e);
		m_layout_timeline->remove(e);
#ifdef LAYOUT_LEE
		m_layout_lee->remove(e);
#endif
		m_layout_neato->remove(e);
		m_layout_fdp->remove(e);
		m_layout_twopi->remove(e);
		m_layout_dot->remove(e);
		m_layout_circo->remove(e);
		m_layout_null->remove(e);
		e->Delete();
	}
	InvalidateLayout();
}

//0		1	2	3	4
//5		6	7	8	9	
//10	11	12	13	14
//15	16	17	18	19
//20	21	22	23	24

bool isCorner(int sideSize, int pos)
{
	int tl = 0;
	int tr = sideSize - 1;
	int br = sideSize * sideSize - 1;
	int bl = br - sideSize;
	if (pos == tl || pos == tr || pos == bl || pos == br)
		return true;
	return false;
}

bool isEdge(int sideSize, int pos)
{
	int tl = 0;
	int tr = sideSize - 1;
	int br = sideSize * sideSize - 1;
	int bl = br - sideSize;
	if (isCorner(sideSize, pos))
		return false;
	if (pos > tl && pos < tr)
		return true;
	if (pos > bl && pos < br)
		return true;
	if (pos % 5 == 0)
		return true;
	if ((pos + 1) % 5 == 0)
		return true;
	return false;
}

int XYtoPos(int SideSize, int x, int y)
{
	return (x * SideSize + y);
}

void CGraphViewCanvas::AddRandom(unsigned vCount, unsigned eCount)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
#define _TEST
#ifdef _TEST
	int SideSize = 16;
	int BoardSize = SideSize * SideSize;
	int Total2 = 4;
	int Total3 = (SideSize - 2) * 4;
	int Total4 = BoardSize - Total3 - Total2;
	CUniqueIDVector vectors;
	for(int i = 0; i < BoardSize; ++i)
	{
		CUniqueID id(CCategory(guidDefault, 99099), boost::lexical_cast<std::_tstring>(i).c_str());
		CComPtr<IGraphVertex> v;
		bool success;
		std::_tstring label;
		label += boost::lexical_cast<std::_tstring>(i);
		boost::tie(v, success) = AddVertex(id, label.c_str());
		if (success)
		{
			vectors.Add(id);
		}
	}
	CComPtr<IGraphEdge> e;
	CCategory cat(guidDefault, 99099);
	for(int i = 0 ; i < SideSize; ++i)
	{
		for(int j = 0 ; j < SideSize; ++j)
		{
			if (i + 1 < SideSize)
				e = AddEdge(cat, vectors[XYtoPos(SideSize, i, j)], vectors[XYtoPos(SideSize, i + 1, j)]);
			if (j + 1 < SideSize)
				e = AddEdge(cat, vectors[XYtoPos(SideSize, i, j)], vectors[XYtoPos(SideSize, i, j + 1)]);
		}
	}
#elif
	vCount=10, eCount=8;
	static int lastAdded = 0;
	CUniqueIDVector vectors;
	for(int i = lastAdded ; i < lastAdded + (int)vCount; ++i)
	{
		CUniqueID id(CCategory(guidDefault, 99099), boost::lexical_cast<std::_tstring>(i).c_str());
		CComPtr<IGraphVertex> v;
		bool success;
		std::_tstring label = _T("I needed a bigger label for testing:  ");
		label += boost::lexical_cast<std::_tstring>(i);
		boost::tie(v, success) = AddVertex(id, label.c_str());
		if (success)
		{
			vectors.Add(id);
			if (rand() < RAND_MAX / 3)
			{
				//v->SetVisible(VISIBLE_HIDE);
			}
		}
	}

	for(int i = lastAdded ; i < lastAdded + (int)vCount; ++i)
	{
		for (int j = i - (int)eCount; j < i; ++j)
		{
			CComPtr<IGraphEdge> e;
			CCategory cat(guidDefault, 99099);
			if (j >= lastAdded)
			{
				e = AddEdge(cat, vectors[j - lastAdded], vectors[i - lastAdded]);
			}
			else
			{
				e = AddEdge(cat, vectors[j + vCount - lastAdded], vectors[i - lastAdded]);
			}
			ATLASSERT(e);
		}
	}

	lastAdded += vCount; 
#endif

	AddRandomTimes();
}

#define MIN_YEAR 2006//1970
#define MAX_YEAR 2006
#define MIN_MONTH 1
#define MAX_MONTH 12
#define MIN_DAY 1
#define MAX_DAY 28
#define MIN_HOUR 0
#define MAX_HOUR 23
#define MIN_MINUTE 0
#define MAX_MINUTE 59

void CGraphViewCanvas::AddRandomTimes()
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);

	typedef boost::minstd_rand base_generator_type;
	base_generator_type generator(static_cast<unsigned int>(std::time(0)));
	boost::uniform_int<> year_dist(MIN_YEAR, MAX_YEAR);
	boost::uniform_int<> month_dist(MIN_MONTH, MAX_MONTH);
	boost::uniform_int<> day_dist(MIN_DAY, MAX_DAY);
	boost::uniform_int<> hour_dist(MIN_HOUR, MAX_HOUR);
	boost::uniform_int<> minute_dist(MIN_MINUTE, MAX_MINUTE);
	boost::variate_generator<base_generator_type&, boost::uniform_int<> > year(generator, year_dist);
	boost::variate_generator<base_generator_type&, boost::uniform_int<> > month(generator, month_dist);
	boost::variate_generator<base_generator_type&, boost::uniform_int<> > day(generator, day_dist);
	boost::variate_generator<base_generator_type&, boost::uniform_int<> > hour(generator, hour_dist);
	boost::variate_generator<base_generator_type&, boost::uniform_int<> > minute(generator, minute_dist);

	for(CComPtr<IEdgeIterator> itr = m_graph->GetEdgeIterator(); itr->IsValidItem(); itr->NextItem())
	{
		IGraphEdge* e=itr->Get();	
		ATLASSERT(e);
		{
			using namespace boost::gregorian;
			using namespace boost::posix_time; 
			ptime time1(date(year(),month(),day()), hours(hour()) + minutes(minute()));
			ATLASSERT(!time1.is_not_a_date_time() && !time1.is_infinity());
			ptime time2(date(year(),month(),day()), hours(hour()) + minutes(minute()));
			ATLASSERT(!time2.is_not_a_date_time() && !time2.is_infinity());
			ATLASSERT(time2!=time1);
			if(time2>time1)
			{
				time_period range(time1, time2);
				e->SetDateRange(range);
			}
			else
			{
				time_period range(time2, time1);
				e->SetDateRange(range);
			}
		}
	}
}

bool CGraphViewCanvas::GetFocus(CUniqueID& id) const
{
	// N.B. in general we are interested in all (not just vertex) nodes that can have focus
	assert(false);
	return false;
}

void CGraphViewCanvas::SetFocus(const CUniqueID & id)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	assert(false);
}

bool CGraphViewCanvas::IsSelected(const CUniqueID & id) const
{
	// TODO: I would think this lock would be needed?
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	CComQIPtr<IVertexRenderer> ir = GetRenderer(id);
	if(ir)
		return ir->IsSelected();
	return false;
}

void CGraphViewCanvas::SetSelected(const CUniqueID & id, bool select, bool append)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	CComPtr<IRenderer> ir = GetRenderer(id);
	if(ir)
		ir->SetSelected(select, append);
}

bool CGraphViewCanvas::HasSelectedVertex()
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	CUniqueIDVector vertices;
	GetVertices(&vertices, TRISTATE_BOTH, TRISTATE_BOTH, TRISTATE_TRUE);
	return vertices.size() > 0;
}

bool CGraphViewCanvas::IsHot(const CUniqueID & id) const
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	CComQIPtr<IVertexRenderer> ir = GetRenderer(id);
	if(ir)
		return ir->IsHot();
	return false;
}

bool CGraphViewCanvas::GetBoundsBox(const CUniqueID & id, CRect& rc)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	CComPtr<IRenderer> ir = GetRenderer(id);
	if(ir)
	{
		GraphTypes::RectF box;
		ir->GetBoundsBox(box);
		rc=box;
		return true;
	}
	return false;
}

void CGraphViewCanvas::SetBoundsBox(const CUniqueID & id, const CRect& rc)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	CComPtr<IRenderer> ir = GetRenderer(id);
	if(ir)
	{
		GraphTypes::RectF box(REAL(rc.left), REAL(rc.top), REAL(rc.Width()), REAL(rc.Height()));
		ir->SetBoundsBox(box, SWP_NOREDRAW);
	}
}

void CGraphViewCanvas::RecalcDispSize(const CUniqueID & id)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	CComPtr<IRenderer> ir = GetRenderer(id);
	if(ir)
	{
		CClientDC dc(m_hWnd);
		CDCHandle hdc(dc.m_hDC);
		ir->CalcDisplaySizes(hdc, true, false);
	}
}

bool CGraphViewCanvas::GetWindowState(const CUniqueID& id, IListWindowRenderer::WINDOW_SIZE& size, int& topindex)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	CComQIPtr<IListWindowRenderer> ir = GetRenderer(id);
	if(ir)
	{
		ir->GetWindowState(size, topindex);
		return true;
	}
	return false;
}

void CGraphViewCanvas::SetWindowState(const CUniqueID& id, IListWindowRenderer::WINDOW_SIZE size, int topindex)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	CComQIPtr<IListWindowRenderer> ir = GetRenderer(id);
	if(ir)
	{
		ir->SetWindowState(size, topindex);
	}
}

bool CGraphViewCanvas::GetWindowSize(const CUniqueID& id, CSize& size)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	CComQIPtr<IListWindowRenderer> ir = GetRenderer(id);
	if(ir)
	{
		GraphTypes::SizeF fsize;
		ir->GetWindowSize(fsize);
		size=fsize;
		return true;
	}
	return false;
}

void CGraphViewCanvas::SetWindowSize(const CUniqueID& id, const CSize& size)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	CComQIPtr<IListWindowRenderer> ir = GetRenderer(id);
	if(ir)
	{
		GraphTypes::SizeF fsize(REAL(size.cx), REAL(size.cy));
		ir->SetWindowSize(fsize);
	}
}

const CUniqueID & CGraphViewCanvas::GetParent(const CUniqueID & id, CUniqueID & parent) const
{
	IGraphItem * item = GetGraphItem(id);
	if (item)
	{
		IGraphSubgraph * subgraph = item->GetParent();
		if (subgraph)
			parent = subgraph->GetID();
	}
	return parent;
}

void CGraphViewCanvas::GetSubgraphs(IUniqueIDContainer * results, TRISTATE visible, TRISTATE pinned, TRISTATE selected, bool includeHidden) const
{
	int dummy; 
	CBuildingEdgeVisibility building(includeHidden ? m_buildingEdgeVisibility : dummy);
	CComPtr<MetaStateSkipper> skipper = new MetaStateSkipper(m_meta, m_canvas, visible, pinned, selected);
	for(CComPtr<ISubgraphIterator> itr = m_graph->GetUserSubgraphIterator(skipper); itr->IsValidItem(); itr->NextItem())
	{
		CComPtr<IGraphSubgraph> Subgraph = itr->Get();
		results->Add(Subgraph->GetID());
	}
}

unsigned CGraphViewCanvas::GetChildren(const CUniqueID& id, IUniqueIDContainer * results, TRISTATE visible, TRISTATE pinned, TRISTATE selected) const
{
	unsigned numchildren=0;
	CComPtr<IGraphSubgraph> subgraph = m_graph->GetSubgraph(id);

	if(subgraph)
	{
		CComPtr<MetaStateSkipper> skipper = new MetaStateSkipper(m_meta, m_canvas, visible, pinned, selected);
		IGraphItemVector children;
		numchildren=subgraph->GetChildren(children, skipper);
		for(IGraphItemVector::const_iterator itr=children.begin(); itr!=children.end(); ++itr)
		{
			//TODO change interface to indicate skipping subgraphs
			results->Add((*itr)->GetID());
		}
	}

	return numchildren;
}

// TODO: probably makes sense to move this to meta since it has dup'd code
void CGraphViewCanvas::GetVertices(IUniqueIDContainer * results, TRISTATE visible, TRISTATE pinned, TRISTATE selected, bool includeHidden) const
{
	int dummy; 
	CBuildingEdgeVisibility building(includeHidden ? m_buildingEdgeVisibility : dummy);
	CComPtr<MetaStateSkipper> skipper = new MetaStateSkipper(m_meta, m_canvas, visible, pinned, selected);
	for(CComPtr<IVertexIterator> itr = m_graph->GetUserVertexIterator(skipper); itr->IsValidItem(); itr->NextItem())
	{
		CComPtr<IGraphVertex> vertex = itr->Get();
		results->Add(vertex->GetID());
	}
}

void CGraphViewCanvas::GetAdjacentVertices(IUniqueIDContainer * results, const CUniqueID& uid, bool in, bool out) const
{
	CComPtr<IGraphVertex> v = m_graph->GetVertex(uid);
	if (v)
	{
		IGraphVertexVector vertices;
		if (in && out)
			v->GetAdjacentVertices(vertices);
		else if (in)
			v->GetAdjacentInVertices(vertices);
		else if (out)
			v->GetAdjacentOutVertices(vertices);

		for(IGraphVertexVector::iterator itr = vertices.begin(); itr != vertices.end(); ++itr)
		{
			if (uid != itr->get()->GetID())	//Not sure why this is needed
				results->Add(itr->get()->GetID());
		}
	}
}

unsigned CGraphViewCanvas::GetVertexCount(TRISTATE visible, TRISTATE pinned, TRISTATE selected) const
{
	unsigned count=0;
	if(visible!=TRISTATE_BOTH || selected!=TRISTATE_BOTH || pinned!=TRISTATE_BOTH)
	{
		CComPtr<MetaStateSkipper> skipper = new MetaStateSkipper(m_meta, m_canvas, visible, pinned, selected);
		for(CComPtr<IVertexIterator> itr = m_graph->GetUserVertexIterator(skipper); itr->IsValidItem(); itr->NextItem())
			count++;
	}
	else
	{
		count=m_graph->GetVertexCount();
	}

	return count;
}

void CGraphViewCanvas::GetEdges(IUniqueIDContainer * results, TRISTATE visible, TRISTATE selected, bool includeHidden) const
{
	int dummy; 
	CBuildingEdgeVisibility building(includeHidden ? m_buildingEdgeVisibility : dummy);
	CComPtr<MetaStateSkipper> skipper = new MetaStateSkipper(m_meta, m_canvas, visible, TRISTATE_BOTH, selected);
	IGraphEdgeVector edges;
	m_graph->GetEdges(edges, skipper);
	for (IGraphEdgeVector::iterator itr = edges.begin(); itr != edges.end(); ++itr)
	{
		IGraphEdge * edge = *itr;
		results->Add(edge->GetID());
	}
}

void CGraphViewCanvas::GetAdjacentEdges(IUniqueIDContainer * results, const CUniqueID& vertexId) const
{
	CComPtr<IGraphVertex> v = m_graph->GetVertex(vertexId);
	if (v == NULL)
	{
		assert(!"NULL Vertex given!");
		return;
	}
	IGraphEdgeVector edges;
	v->GetAdjacentEdges(edges);
	for(IGraphEdgeVector::iterator itr = edges.begin(); itr != edges.end(); ++itr)
	{
		IGraphEdge * edge = *itr;
		results->Add(edge->GetID());
	}
}

void CGraphViewCanvas::GetKnownCategories(META_TYPE entityType, ICategoryContainer * results) const
{
	MetaValueMap::const_iterator item, end;
	// iterate through all meta data and get meta_vertex name entries
	for (boost::tie(item, end) = m_meta.GetIterator(); item != end; ++item)
	{
		if ((item->first.get<0>()) == entityType && (item->first.get<1>()) == LABEL)
		{
			CCategory cat = (item->first.get<2>());
			results->Add(cat);
		}
	}
}

unsigned CGraphViewCanvas::GetEdgeCount() const
{
	return m_graph->GetEdgeCount();
}

void CGraphViewCanvas::GetInEdges(const CUniqueID &vertex, IUniqueIDContainer *results) const
{
	CBuildingEdgeVisibility building(m_buildingEdgeVisibility);
	CComPtr<IGraphVertex> v = m_graph->GetVertex(vertex);
	assert(v != NULL);

	IGraphEdgeVector edges;
	v->GetAdjacentInEdges(edges);
	for(IGraphEdgeVector::iterator eitr = edges.begin(); eitr != edges.end(); ++eitr)
	{
		IGraphEdge * edge = *eitr;
		results->Add(edge->GetID());
	}
}

void CGraphViewCanvas::GetOutEdges(const CUniqueID &vertex, IUniqueIDContainer *results) const
{
	CBuildingEdgeVisibility building(m_buildingEdgeVisibility);
	CComPtr<IGraphVertex> v = m_graph->GetVertex(vertex);
	assert(v != NULL);

	IGraphEdgeVector edges;
	v->GetAdjacentOutEdges(edges);
	for(IGraphEdgeVector::iterator eitr = edges.begin(); eitr != edges.end(); ++eitr)
	{
		IGraphEdge * edge = *eitr;
		results->Add(edge->GetID());
	}
}

void CGraphViewCanvas::ClearSelected()
{
	m_canvas->DeselectAll();
}

void CGraphViewCanvas::SetVertexImagesVisible(bool visible)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	m_meta.SetVertexImagesVisible(visible);
	InvalidateLayout();
}


bool CGraphViewCanvas::GetVertexImagesVisible() const
{
	return m_meta.GetVertexImagesVisible();
}


void CGraphViewCanvas::SetVertexNeighboursVisibleByEdgeCat(IGraphVertex * v, const CCategory & cat, VISIBLE visible)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	assert(v);
	IGraphEdgeVector edges;
	v->GetAdjacentEdges(edges);
	for(IGraphEdgeVector::iterator eitr = edges.begin(); eitr != edges.end(); ++eitr)
	{
		IGraphEdge * edge = *eitr;
		if (edge->GetCategory() == cat)
		{
			CComPtr<IGraphVertex> adj_vertex = edge->GetAdjacentVertex(v);
			assert(adj_vertex->GetID() != v->GetID());
			adj_vertex->SetVisible(visible);
		}
	}
	InvalidateLayout();
}

void CGraphViewCanvas::SetVertexNeighboursVisibleByVertexCat(IGraphVertex * v, const CCategory & cat, VISIBLE visible)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	assert(v);
	IGraphEdgeVector edges;
	v->GetAdjacentEdges(edges);
	for(IGraphEdgeVector::iterator eitr = edges.begin(); eitr != edges.end(); ++eitr)
	{
		IGraphEdge * edge = *eitr;
		IGraphVertex * adj_vertex = edge->GetAdjacentVertex(v);
		assert(adj_vertex->GetID() != v->GetID());
		if (adj_vertex->GetCategory() == cat)		
		{
			adj_vertex->SetVisible(visible);
		}
	}
	InvalidateLayout();
}

void CGraphViewCanvas::SetVertexNeighbour(IGraphVertex * v, SGV::PSGV_TARGETSTRUCT info = NULL)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	assert(v);

	if (info)
	{
		if (info->style == info->SGV_TARGET_STYLE_VERTEX)
		{
			CCategory category (info->category.plugin,info->category.category);

			if (info->state == info->SGV_TARGET_STATE_VISIBLE)
				SetVertexNeighboursVisibleByVertexCat(v,category,VISIBLE_SHOW);
			else
				SetVertexNeighboursVisibleByVertexCat(v,category,VISIBLE_HIDE);
		}
		else if (info->style == info->SGV_TARGET_STYLE_EDGE)
		{
			CCategory category (info->category.plugin,info->category.category);

			if (info->state == info->SGV_TARGET_STATE_VISIBLE)
				SetVertexNeighboursVisibleByEdgeCat(v,category,VISIBLE_SHOW);
			else
				SetVertexNeighboursVisibleByEdgeCat(v,category,VISIBLE_HIDE);
		}
	}
}

unsigned CGraphViewCanvas::GetVertexNeighbourCount(IGraphVertex * v, SGV::PSGV_TARGETSTRUCT info = NULL) const
{
	assert(v);
	unsigned retVal = 0;
	
	if (info)
	{
		if (info->style == info->SGV_TARGET_STYLE_VERTEX)
		{
			CCategory category (info->category.plugin,info->category.category);

			if (info->state == info->SGV_TARGET_STATE_VISIBLE)
				retVal = GetVertexVisibleNeighbourCountByVertexCat(v,category);
			else
				retVal = GetVertexHiddenNeighbourCountByVertexCat(v,category);
		}
		else if (info->style == info->SGV_TARGET_STYLE_EDGE)
		{
			CCategory category (info->category.plugin,info->category.category);

			if (info->state == info->SGV_TARGET_STATE_VISIBLE)
				retVal = GetVertexVisibleNeighbourCountByEdgeCat(v,category);
			else
				retVal = GetVertexHiddenNeighbourCountByEdgeCat(v,category);
		}
		else
		{
			if (info->state == info->SGV_TARGET_STATE_VISIBLE)
				retVal = GetVertexVisibleNeighbourCount(v);
			else if (info->state == info->SGV_TARGET_STATE_HIDDEN)
				retVal = GetVertexHiddenNeighbourCount(v);
		}
	}
	else
	{
		IGraphVertexVector vertices;
		return v->GetAdjacentVertices(vertices);
	}
	return retVal;
}
/*
unsigned CGraphViewCanvas::GetVertexNeighbourCount(IGraphVertex * v)
{
	unsigned retVal = 0;
	CComPtr<boost::scoped_lock_ref_counted> lock;
	for (StlLinked<IVertexIterator> n = v->getAdjacentVertexIterator(lock); n->IsValidItem(); n->NextItem())
	{
		++retVal;
	}
	return retVal;
}
*/
unsigned CGraphViewCanvas::GetVertexVisibleNeighbourCount(IGraphVertex * v) const
{
	assert(v);
	unsigned retVal = 0;

	IGraphVertexVector vertices;
	v->GetAdjacentVertices(vertices);
	for(IGraphVertexVector::iterator itr = vertices.begin(); itr != vertices.end(); ++itr)
	{
		if (m_meta.IsVisible(itr->get()))
		{
			++retVal;
		}
	}

	return retVal;
}
unsigned CGraphViewCanvas::GetVertexHiddenNeighbourCount(IGraphVertex * v) const
{
	assert(v);
	unsigned retVal = 0;

	IGraphVertexVector vertices;
	v->GetAdjacentVertices(vertices);
	for(IGraphVertexVector::iterator itr = vertices.begin(); itr != vertices.end(); ++itr)
	{
		if (m_meta.IsHidden(itr->get()))
		{
			++retVal;
		}
	}

	return retVal;
}
unsigned CGraphViewCanvas::GetVertexVisibleNeighbourCountByVertexCat(IGraphVertex * v, const CCategory & cat) const
{
	assert(v);
	unsigned retVal = 0;

	IGraphVertexVector vertices;
	v->GetAdjacentVertices(vertices);
	for(IGraphVertexVector::iterator itr = vertices.begin(); itr != vertices.end(); ++itr)
	{
		if (m_meta.IsVisible(itr->get()) && itr->get()->GetCategory() == cat)
		{
			++retVal;
		}
	}
	return retVal;
}

unsigned CGraphViewCanvas::GetVertexHiddenNeighbourCountByVertexCat(IGraphVertex * v, const CCategory & cat) const
{
	assert(v);
	unsigned retVal = 0;

	IGraphVertexVector vertices;
	v->GetAdjacentVertices(vertices);
	for(IGraphVertexVector::iterator itr = vertices.begin(); itr != vertices.end(); ++itr)
	{
		if (m_meta.IsHidden(itr->get()) && itr->get()->GetCategory() == cat)
		{
			++retVal;
		}
	}
	return retVal;
}

unsigned CGraphViewCanvas::GetVertexVisibleNeighbourCountByEdgeCat(IGraphVertex * v, const CCategory & cat) const
{
	assert(v);
	unsigned retVal = 0;
	IGraphEdgeVector edges;
	v->GetAdjacentEdges(edges);
	for(IGraphEdgeVector::iterator eitr = edges.begin(); eitr != edges.end(); ++eitr)
	{
		IGraphEdge * edge = *eitr;
		if (m_meta.IsVisible(edge) && edge->GetCategory() == cat)
			++retVal;
	}
	return retVal;
}

unsigned CGraphViewCanvas::GetVertexHiddenNeighbourCountByEdgeCat(IGraphVertex * v, const CCategory & cat) const
{
	assert(v);
	unsigned retVal = 0;
	IGraphEdgeVector edges;
	v->GetAdjacentEdges(edges);
	for(IGraphEdgeVector::iterator eitr = edges.begin(); eitr != edges.end(); ++eitr)
	{
		IGraphEdge * edge = *eitr;
		if (m_meta.IsHidden(edge) && edge->GetCategory() == cat)
			++retVal;
	}
	return retVal;
}

void CGraphViewCanvas::SetVerticesVisibleByEdgeCount(IUniqueIDContainer * results, int edgeCount, VISIBLE visibility)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	IGraphVertexVector toChange;
	for (CComPtr<IVertexIterator> n = m_graph->GetVertexIterator(); n->IsValidItem(); n->NextItem())
	{
		CComPtr<IGraphVertex> vertex = n->Get();
		if (m_meta.IsVisible(vertex))
		{
			int threshold = edgeCount;
			IGraphEdgeVector edges;
			vertex->GetAdjacentEdges(edges);
			for(IGraphEdgeVector::iterator e = edges.begin(); e != edges.end(); ++e)
			{
				IGraphEdge * edge = *e;
				if (m_meta.IsVisible(edge))
				{
					if (--threshold < 0)
					{
						break;
					}
				}
			}
			if (threshold == 0)
			{
				toChange.push_back(IGraphVertexAdapt(vertex));
			}
		}
	}

	for(IGraphVertexVector::iterator itr = toChange.begin(); itr != toChange.end(); ++itr)
	{
		itr->get()->SetVisible(visibility);
		if (results != NULL)
			results->Add(itr->get()->GetID());
	}

	InvalidateLayout();
}

const CUniqueID& CGraphViewCanvas::MergeIntoSubgraph(CUniqueID& retVal, IUniqueIDContainer* items, const TCHAR* label)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	ATLASSERT(items);

	items->Reset();
	bool bSuccess=items!=0 && items->MoveNext();

	IGraphSubgraph* isg;
	if(bSuccess)
	{
		unsigned int firstHash = items->Get().GetHash();
		CUniqueID id(CCategory(guidDefault, 0), _T("merged[") + boost::lexical_cast<std::_tstring>(firstHash) + _T("]"));
		retVal=id;

		// check if it exists
		if(m_graph->GetSubgraph(id)!=0)
		{
			items->Reset();
			return retVal;
		}

		isg=AddSubgraph(id, label, m_graph, rtLegacySubgraph); // no support for other subgraphs for now
		bSuccess=isg!=0;
	}

	if(bSuccess)
	{
		m_canvas->ClearCurrentHot(); // just in case
		m_canvas->ClearFocus();

		RectF bbox, rcunion;
		bool first=true;
		items->Reset();
		while(items->MoveNext())
		{
			const CUniqueID& item = items->Get();
			IGraphVertex* v=m_graph->GetVertex(item);
			ATLASSERT(v);
			if(v)
			{
				CComQIPtr<ILegacyRenderer> ir=m_canvas->GetRenderer(v);
				if(ir)
				{
					ir->SetSelected(false);
					ir->GetBoundsBox(bbox);
					if(!bbox.IsEmptyArea())
					{
						if(first)
						{
							rcunion=bbox;
							first=false;
						}
						else
						{
							rcunion.Union(rcunion, rcunion, bbox);
						}
					}
				}

				m_graph->DetachItem(v);
				isg->AttachItem(v);
			}
		}

		//TODO annoyiing when loading files ATLASSERT(!rcunion.IsEmptyArea());
		//fix via user code passing the position

		// position subgraph in the center of vertex union
		if(!rcunion.IsEmptyArea())
		{
			CComQIPtr<ILegacySubgraphRenderer> ir=m_canvas->GetRenderer(isg);
			PointF center;
			rcunion.GetCenter(&center);
			ir->SetPosition(center, GraphTypes::screen);
		}

		PointF p;
		m_layout_spring1->append(isg, p);
		m_layout_circle->append(isg, p);
		m_layout_timeline->append(isg, p);
#ifdef LAYOUT_LEE
		m_layout_lee->append(isg, p);
#endif
		m_layout_neato->append(isg, p);
		m_layout_fdp->append(isg, p);
		m_layout_twopi->append(isg, p);
		m_layout_dot->append(isg, p);
		m_layout_circo->append(isg, p);
		m_layout_null->append(isg, p);

		InvalidateLayout();

	}
	else
	{
		ATLASSERT(!_T("Couldn't create subgraph"));
	}

	items->Reset();
	return retVal;
}

// by default destroys subgraph after extracting its vertices
void CGraphViewCanvas::BreakSubgraph(IUniqueIDContainer* results, const CUniqueID& subgraph)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);

	IGraphSubgraph* isg = m_graph->GetSubgraph(subgraph);
	ATLASSERT(isg);
	if(isg==0)
		return;

	CComQIPtr<IGraphVertex> v;
	CComPtr<IGraphItemIterator> itr = isg->GetChildren(); 
	while(itr->IsValidItem())
	{
		v=itr->Get();
		ATLASSERT(v);
		if(v)
		{
			isg->DetachItem(v);
			m_graph->AttachItem(v);
			if(results)
				results->Add(v->GetID());
		}
		// iterator changes as we detach vertices so get it again
		itr = isg->GetChildren();
	}

	m_layout_spring1->remove(isg);
	m_layout_circle->remove(isg);
	m_layout_timeline->remove(isg);
#ifdef LAYOUT_LEE
	m_layout_lee->remove(isg);
#endif
	m_layout_neato->remove(isg);
	m_layout_fdp->remove(isg);
	m_layout_twopi->remove(isg);
	m_layout_dot->remove(isg);
	m_layout_circo->remove(isg);
	m_layout_null->remove(isg);
	RemoveSubgraph(isg);
}

void CGraphViewCanvas::SetTargetByCount(IUniqueIDContainer * results, SGV::PSGV_TARGETSTRUCT target)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	assert(target);
	VISIBLE visibility;
	if (target->state == SGV::SGV_TARGETSTRUCT::SGV_TARGET_STATE_VISIBLE)
		visibility = VISIBLE_SHOW;
	else
		visibility = VISIBLE_HIDE;
	if (target->style == SGV::SGV_TARGETSTRUCT::SGV_TARGET_STYLE_EDGE)
		SetVerticesVisibleByEdgeCount(results, target->count, visibility);
}

RectF CGraphViewCanvas::GetRenderExtent() 
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	CPaintDC dc(m_hWnd);
	return GetRenderExtent(dc.m_hDC);
}

RectF CGraphViewCanvas::GetRenderExtent(bool selectedOnly) 
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	CPaintDC dc(m_hWnd);
	return GetRenderExtent(dc.m_hDC, selectedOnly);
}

RectF CGraphViewCanvas::GetRenderExtent(HDC dc) 
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	return GetRenderExtent(dc, false);
}

RectF CGraphViewCanvas::GetRenderExtent(HDC hDC, bool selectedOnly) 
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	CMemBm memDC(hDC, NULL, 0, 0, 10, 10);
	CDC dc(memDC);
	RectF rc;
	{
		boost::scoped_ptr<CAggDC> aggdc(CAggDC::CreateGDIDC(dc, m_canvas->m_fonts));
		DoPaint(*aggdc, true);
		rc=m_canvas->getGraphExtent(selectedOnly);
	}
	return rc;
}

RectF CGraphViewCanvas::GetSelectedRenderExtent() 
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	CPaintDC dc(m_hWnd);
	return GetSelectedRenderExtent(dc.m_hDC);
}

RectF CGraphViewCanvas::GetSelectedRenderExtent(HDC dc) 
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	return GetRenderExtent(dc, true);
}

void CGraphViewCanvas::Stop()
{
	m_breakFlag = true;
	m_layout->halt();
	clib::recursive_mutex::scoped_lock proc(m_pulsemutex);	//Block until layout has stopped
	m_breakFlag = false;
}

void CGraphViewCanvas::Start()
{
	if (!m_running)
	{
		Stop();
		GetRenderExtent();
		clib::thread thread(__FUNCTION__, boost::bind(&LayoutThread, this));
	}
}

void CGraphViewCanvas::delSelNode()
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	assert(false);
}

void CGraphViewCanvas::MoveAllChildren(const PointF & delta)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	float deltaX = delta.x;
	float deltaY = delta.y;

	if(deltaX==0 && deltaY==0)
		return;

	PointF p;
	for(CComPtr<ISubgraphIterator> itr = m_graph->GetSubgraphIterator(); itr->IsValidItem(); itr->NextItem())
	{
		CComPtr<IRenderer> ir=m_canvas->GetRenderer(itr->Get());
		if(ir)
		{
			ir->GetPosition(p);
			p.x -= deltaX;
			p.y -= deltaY;
			ir->SetPosition(p);
		}
	}

	for(CComPtr<IVertexIterator> itr = m_graph->GetVertexIterator(); itr->IsValidItem(); itr->NextItem())
	{
		CComPtr<IRenderer> ir=m_canvas->GetRenderer(itr->Get());
		if(ir)
		{
			ir->GetPosition(p);
			p.x -= deltaX;
			p.y -= deltaY;
			ir->SetPosition(p);
		}
	}

	for(CComPtr<IEdgeIterator> itr = m_graph->GetEdgeIterator(); itr->IsValidItem(); itr->NextItem())
	{
		CComQIPtr<IEdgeRenderer> ir=m_canvas->GetRenderer(itr->Get());
		if(ir)
		{
			ir->GetPosition(p);
			p.x -= deltaX;
			p.y -= deltaY;
			ir->SetPosition(p);

			RectF pos;
			ir->GetLabelPos(pos, world);	
			pos.x-=deltaX;
			pos.y-=deltaY;
			ir->SetLabelPos(pos, world);	
			PointFVector points;
			unsigned npoints=ir->GetPoints(points);

			// for now only AT&T edges have externally given label pos
			// legacy edges rely on default value 0,0 to center the label

			if(npoints>=2)
			{
				for(unsigned i=0; i<npoints; ++i)
				{
					points[i].x-=deltaX;
					points[i].y-=deltaY;
				}
				
				ir->SetPoints(points);
			}
		}
	}
}

void CGraphViewCanvas::ClearAllEdgePoints()
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	IGraphEdgeVector edges;
	m_graph->GetEdges(edges);
	PointFVector points;

	for(IGraphEdgeVector::iterator itr = edges.begin(); itr != edges.end(); ++itr)
	{
		IGraphEdge * edge = *itr;
		CComQIPtr<IEdgeRenderer> ire=m_canvas->GetRenderer(edge);
		assert(ire);
		ire->SetPoints(points);	
		assert(points.size()==0);
	}
}

void CGraphViewCanvas::CenterGraph() 
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);

	RectF extent=GetRenderExtent();

	if(!extent.IsEmptyArea())
	{
		PointF newcenter;
		extent.GetCenter(&newcenter);
		CRect rc;
		GetClientRect(&rc);
		// difference to add to current offset in order to get to the center
		PointF diff(newcenter.x-rc.Width()/2, newcenter.y-rc.Height()/2);
		PointF oldoffset=m_canvas->GetOffset();
		PointF newoffset(oldoffset.x+diff.x, oldoffset.y+diff.y);

		morph(oldoffset, newoffset);
	}
}

void CGraphViewCanvas::CenterFocusNode()
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	assert(false);
}

void CGraphViewCanvas::CenterVertex(const CUniqueID& id, REAL scale/*=-1*/)
{
#ifdef _DEBUG
	CComPtr<IGraphVertex> v = m_graph->GetVertex(id);
	assert(v);
#endif
	CenterGraphItem(id, scale);
}

void CGraphViewCanvas::CenterGraphItem(const CUniqueID& id, REAL scale/*=-1*/)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);

	if(CComPtr<IGraphEdge> e = m_graph->GetEdge(id))
		CenterItem(e);
	else if(CComPtr<IGraphVertex> vertex = m_graph->GetVertex(id))
		CenterItem(vertex);
	else if (CComPtr<IGraphSubgraph> subgraph = m_graph->GetSubgraph(id))
		CenterItem(subgraph);
}

void CGraphViewCanvas::CenterEdge(IGraphEdge * e, bool fitVertices/*=false*/)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	ATLASSERT(e);
	CComQIPtr<IEdgeRenderer> ir = m_canvas->GetRenderer(e);

	if(ir)
	{
		m_canvas->ClearCurrentHot();

		if(fitVertices)
		{
			RectF gRect;
			CComPtr<IGraphVertex> v = e->GetSource();
			ATLASSERT(v);
			CComQIPtr<IRenderer> ivr = m_canvas->GetRenderer(v);
			ATLASSERT(ivr);
			ivr->GetBoundsBox(gRect);
			v = e->GetTarget();
			ATLASSERT(v);
			ivr = m_canvas->GetRenderer(v);
			ATLASSERT(ivr);
			RectF bbox;
			ivr->GetBoundsBox(bbox);
			gRect.Union(gRect, gRect, bbox);
			ATLASSERT(!gRect.IsEmptyArea());

			if(gRect.IsEmptyArea())
				return;

			CRect rc;
			GetClientRect(&rc);
			REAL widthRatio = REAL(rc.Width()) / gRect.Width;
			REAL heightRatio = REAL(rc.Height()) / gRect.Height;

			REAL oldscale = m_canvas->GetScale();
			REAL newscale = (widthRatio < heightRatio ? widthRatio : heightRatio) * oldscale;
			if (newscale > 1.0)
				newscale = 1.0;
			
			m_canvas->SetScale(newscale);
			GetRenderExtent(false);

			ivr->GetBoundsBox(gRect);
			v = e->GetSource();
			ivr = m_canvas->GetRenderer(v);
			ivr->GetBoundsBox(bbox);
			gRect.Union(gRect, gRect, bbox);

			PointF newcenter;
			gRect.GetCenter(&newcenter);

			// difference to add to current offset in order to get to the center
			PointF diff(newcenter.x-REAL(rc.Width())/2, newcenter.y-REAL(rc.Height())/2);
			PointF oldoffset=m_canvas->GetOffset();
			PointF newoffset(oldoffset.x+diff.x, oldoffset.y+diff.y);

			morph(oldscale, newscale, oldoffset, newoffset);
		}
		else
		{
			RectF erect;
			ir->GetLabelPos(erect);
			PointF newcenter;
			erect.GetCenter(&newcenter);

			PointF oldoffset = m_canvas->GetOffset();
			m_canvas->CenterAt(newcenter);
			PointF newoffset = m_canvas->GetOffset();

			InvalidateLayout();
			morph(oldoffset, newoffset);
		}
	}
}

void CGraphViewCanvas::CenterItem(IGraphItem * item, bool zoomToFit)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	ATLASSERT(item);

	m_canvas->ClearCurrentHot();

	CComPtr<IRenderer> ir=m_canvas->GetRenderer(item);
	ATLASSERT(ir);

	REAL oldscale = m_canvas->GetScale();

	RectF bbox;
	ir->GetBoundsBox(bbox);
	PointF newcenter;
	bbox.GetCenter(&newcenter);
	CRect rc;
	GetClientRect(&rc);

	// difference to add to current offset in order to get to the center
	PointF diff(newcenter.x-REAL(rc.Width())/2, newcenter.y-REAL(rc.Height())/2);
	PointF oldoffset=m_canvas->GetOffset();
	PointF newoffset(oldoffset.x+diff.x, oldoffset.y+diff.y);

	morph(oldoffset, newoffset);
}

#define __MAXFITLOOPCOUNT__	5
#define __FITPIXELSLACK__	2

float CGraphViewCanvas::SetScaleToFit(bool selectedOnly, float maxScale)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);

	float startScale=m_canvas->GetScale(); // save for morph
	float prevScale=startScale; 

	RectF wRect;
	RectF gRect = GetRenderExtent(selectedOnly);

	if (gRect.Width==0 || gRect.Height == 0)
		return startScale;

	CRect rc;
	GetClientRect(&rc);
	wRect.Width= static_cast<GraphTypes::REAL>(rc.Width());
	wRect.Height= static_cast<GraphTypes::REAL>(rc.Height());

	//if (m_hSB.IsWindowVisible())
	{
		CRect rc;
		m_hSB.GetClientRect(rc);
		wRect.Height -= rc.Height();
	}

	//if (m_vSB.IsWindowVisible())
	{
		CRect rc;
		m_vSB.GetClientRect(rc);
		wRect.Width -= rc.Width();
	}

	float endScale;
	int count=0;

	do 
	{
		float widthRatio = wRect.Width / gRect.Width;
		float heightRatio = wRect.Height / gRect.Height;
		endScale = (widthRatio < heightRatio ? widthRatio : heightRatio) * prevScale;

		if (endScale > maxScale)
			endScale = maxScale;

		m_canvas->SetScale(endScale);
		gRect = GetRenderExtent(selectedOnly);
		gRect.Inflate(REAL(__FITPIXELSLACK__), REAL(__FITPIXELSLACK__));
		if((gRect.Width<=wRect.Width && gRect.Height<=wRect.Height) || count>__MAXFITLOOPCOUNT__)
			break;
		count++;
		prevScale = m_canvas->GetScale();
	} while (true);

	// desired new screen center in screen coordinates
	PointF newcenter;
	gRect.GetCenter(&newcenter);

	// difference to add to current offset in order to get to the center
	PointF diff(newcenter.x-wRect.Width/2, newcenter.y-wRect.Height/2);
	PointF startOffset=m_canvas->GetOffset();
	PointF endOffset(startOffset.x+diff.x, startOffset.y+diff.y);

	morph(startScale, endScale, startOffset, endOffset);

	return endScale;
}

float CGraphViewCanvas::SetScaleToWidth(bool selectedOnly, float maxSize, bool centerVScroll)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);

	float oldScale = m_canvas->GetScale();
	float retVal=oldScale;

	RectF wRect;
	m_canvas->GetViewport(wRect);

	RectF gRect = GetRenderExtent(selectedOnly);

	if (gRect.Width == 0 || gRect.Height == 0)
		return retVal;

	float widthRatio = wRect.Width / gRect.Width;
	retVal = widthRatio * oldScale;

	if (retVal > maxSize)
		retVal = maxSize;

	m_canvas->SetScale(retVal);
	gRect = GetRenderExtent(selectedOnly);

	// desired new screen center in screen coordinates
	PointF newcenter;
	if (centerVScroll)
	{
		gRect.GetCenter(&newcenter);
		// difference to add to current offset in order to get to the center
		PointF diff(newcenter.x-wRect.Width/2, newcenter.y-wRect.Height/2);
		PointF oldoffset=m_canvas->GetOffset();
		PointF newoffset(oldoffset.x+diff.x, oldoffset.y+diff.y);

		morph(oldScale, retVal, oldoffset, newoffset);
	}
	else
	{
		gRect.GetCenterTop(&newcenter);
		// difference to add to current offset in order to get to the center
		PointF diff(newcenter.x-wRect.Width/2, newcenter.y);
		PointF oldoffset=m_canvas->GetOffset();
		PointF newoffset(oldoffset.x+diff.x, oldoffset.y+diff.y);
		morph(oldScale, retVal, oldoffset, newoffset);
	}
	return retVal;
}

float CGraphViewCanvas::SetItemScaleToFit(const CUniqueID& id, float maxSize/*= 1.0f*/)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);

	float oldScale = m_canvas->GetScale();
	float retVal=oldScale;

	CComPtr<IGraphItem> item=GetGraphItem(id);
	if(item==0)
		return retVal;

	CComPtr<IRenderer> ir=m_canvas->GetRenderer(item);
	assert(ir);
	RectF wRect;
	RectF gRect;
	ir->GetBoundsBox(gRect);

	if (gRect.Width == 0 || gRect.Height == 0)
		return retVal;

	oldScale = m_canvas->GetScale();
	CRect rc;
	GetClientRect(&rc);
	wRect=rc;
	float widthRatio = wRect.Width / gRect.Width;
	float heightRatio = wRect.Height / gRect.Height;
	retVal = (widthRatio < heightRatio ? widthRatio : heightRatio) * oldScale;

	if (retVal > maxSize)
		retVal = maxSize;

	m_canvas->SetScale(retVal);
	GetRenderExtent(false);
	ir->GetBoundsBox(gRect);

	// desired new screen center in screen coordinates
	PointF newcenter;
	gRect.GetCenter(&newcenter);

	// difference to add to current offset in order to get to the center
	PointF diff(newcenter.x-wRect.Width/2, newcenter.y-wRect.Height/2);
	PointF oldoffset=m_canvas->GetOffset();
	PointF newoffset(oldoffset.x+diff.x, oldoffset.y+diff.y);

	morph(oldScale, retVal, oldoffset, newoffset);

	return retVal;
}


float CGraphViewCanvas::GetScale() const
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	return m_canvas->GetScale();
}

void CGraphViewCanvas::SetScale(float scale)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);

	// save old screen center in world coordinates
	PointF old;
	m_canvas->GetScreenCenter(old);
	// TODO: morph code needs to be made a meta optional value for web control, for now disabled.
	//GraphTypes::PointF old_offset = m_canvas->GetOffset();
	//float old_scale = m_canvas->GetScale();

	double old_scale=m_canvas->GetScale();
	if (m_canvas->SetScale(scale))
	{
		if(m_canvas->GetApplicationVisMode()==RelavintTimeline)
		{	
			if(scale>=0.001) // don't want infinites here
			{
				m_timeline->ScaleTimeline(old_scale/scale);
				m_timeline->UpdateWindow();
			}

		}

		// restore screen center
		m_canvas->CenterAt(old);
		//GraphTypes::PointF new_offset = m_canvas->GetOffset();
		//morph(old_scale, scale, old_offset, new_offset);
		InvalidateLayout();
		if (m_Owner)
		{
			m_Owner->OnScaleChanged();
		}
	}
}

void CGraphViewCanvas::SetScaleAt(const CPoint& pt, REAL step/*=0.25*/)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);

	if(m_canvas->GetApplicationMode()!=AppModeQBGraph)
		return;

	// save point in world coordinates
	PointF oldpt=pt;
	m_canvas->RAllTransform(oldpt);

	double old_scale=m_canvas->GetScale();
	double new_scale=old_scale+step;
	if(new_scale<0.005)
		new_scale=0.005;
	if(new_scale>5)
		new_scale=5;

	if(m_canvas->SetScale(static_cast<float>(new_scale)))
	{
		// restore point
		m_canvas->AllTransform(oldpt); // new screen coords of desired point

		// difference to add to current offset in order to maintan point
		PointF diff(oldpt.x-pt.x, oldpt.y-pt.y);

		if(diff.x!=0 || diff.y!=0)
		{
			PointF cur=m_canvas->GetOffset();

			cur.x+=diff.x;
			cur.y+=diff.y;

			SetOffset(cur);
			InvalidateLayout();
		}
	}
}


void CGraphViewCanvas::GetOffset(GraphTypes::PointF& offset) const
{
	offset=m_canvas->GetOffset();
}

void CGraphViewCanvas::SetSpacing(float spacing)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);

	// save old screen center in world coordinates
	PointF old;
	m_canvas->GetScreenCenter(old);
	bool bChanged=false;

	if(m_canvas->GetApplicationVisMode()==RelavintTimeline)
	{
		PointF ptspacing(spacing, 1.0);
		PointF old_spacing=m_canvas->GetSpacingPoint();
		bChanged=m_canvas->SetSpacing(ptspacing);
		if(bChanged)
		{
			if(old_spacing.x>=0.001) // don't want infinites here
			{
				m_timeline->ScaleTimeline(old_spacing.x/spacing);
				m_timeline->UpdateWindow();
			}
		}
	}
	else
	{
		bChanged=m_canvas->SetSpacing(spacing);
	}

	if(bChanged)
	{
		// restore screen center
		m_canvas->CenterAt(old);
		InvalidateLayout();
		if (m_Owner)
			m_Owner->OnSpacingChanged();
	}
}

int CGraphViewCanvas::GetRotation() const
{
	return m_canvas->GetRotation();
}

void CGraphViewCanvas::SetRotation(int angle)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	// save old screen center in world coordinates
	PointF old;
	m_canvas->GetScreenCenter(old);

	if (m_canvas->SetRotation(angle))
	{
		// restore screen center
		m_canvas->CenterAt(old);
		InvalidateLayout();
	}
}

void CGraphViewCanvas::SetOffset(const GraphTypes::PointF& offset)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	m_canvas->SetOffset(offset);
}

float CGraphViewCanvas::GetSpacing() const
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	if(m_canvas->GetApplicationVisMode()==RelavintTimeline)
	{
		PointF ptspacing=m_canvas->GetSpacingPoint();
		return ptspacing.x;
	}
	else
		return m_canvas->GetSpacing();
}

//-----------------------------------------------------------------------------

void CGraphViewCanvas::UpdateScrollBars(const CRect& rcClient)
{
	RectF graphExtent;
	m_canvas->GetGraphExtent(graphExtent);
	m_scrollOffset=m_canvas->GetOffset();

	CRect rcunion(rcClient);
	CRect rcvp(rcClient);
	CRect rcextent=graphExtent;
	CRect rcintersect;

	if(!rcextent.IsRectEmpty())
	{
		if(rcintersect.IntersectRect(&rcextent, &rcvp)!=0)	
		{
			if(!rcintersect.EqualRect(&rcextent)) // not everything is inside
				rcunion.UnionRect(&rcextent, &rcvp);
		}
		else // entire graph extent is outside the viewport
		{
			rcunion.UnionRect(&rcextent, &rcvp);
		}
	}

	int whScrollbar=GetSystemMetrics(SM_CYHSCROLL);

	bool bHSB=rcvp.Width()<rcunion.Width();
	bool bVSB=rcvp.Height()<rcunion.Height();
	if(bHSB)
		rcvp.DeflateRect(0,0,0,whScrollbar);
	if(bVSB)
		rcvp.DeflateRect(0,0,whScrollbar,0);

	// now with viewport excluding scrollbars calc scroll extent
	// and scrollbar visibility again
	if(bHSB || bVSB)
	{
		rcunion.UnionRect(&rcextent, &rcvp);

		bHSB=rcvp.Width()<rcunion.Width();
		bVSB=rcvp.Height()<rcunion.Height();
	}

	// +1 to prevent displaying scrollbars when page equals extent
	CSize page(rcvp.Width()+1,rcvp.Height()+1);  

	SCROLLINFO si;
	si.cbSize=sizeof si;
	si.fMask=SIF_RANGE|SIF_PAGE|SIF_POS;

	if(bHSB)
	{
		si.nMin=0;
		si.nMax=rcunion.Width();
		si.nPage=page.cx;
		si.nPos=0;
		if(rcunion.left < rcvp.left)
			si.nPos=rcvp.left-rcunion.left;

		m_scrollOffset.x-=si.nPos;
		//ATLASSERT(si.nPos>=0 && si.nPos<=(si.nMax-page.cx+1));

		m_hSB.SetScrollInfo(&si);
	}

	if(bVSB)
	{
		si.nMin=0;
		si.nMax=rcunion.Height();
		si.nPage=page.cy;
		si.nPos=0;
		if(rcunion.top < rcvp.top)
			si.nPos=rcvp.top-rcunion.top;

		m_scrollOffset.y-=si.nPos;
		//ATLASSERT(si.nPos>=0 && si.nPos<=(si.nMax-page.cy+1));

		m_vSB.SetScrollInfo(&si);
	}

	CRect vrcsb=rcClient;
	vrcsb.left=vrcsb.right-whScrollbar;
	CRect hrcsb=rcClient;
	hrcsb.top=hrcsb.bottom-whScrollbar;

	if(bHSB)
		vrcsb.bottom-=whScrollbar;

	if(bVSB)
		hrcsb.right-=whScrollbar;

	if(bVSB)
		m_vSB.MoveWindow(&vrcsb);

	if(bHSB)
		m_hSB.MoveWindow(&hrcsb);

	m_hSB.ShowWindow(bHSB?SW_SHOW:SW_HIDE);
	m_vSB.ShowWindow(bVSB?SW_SHOW:SW_HIDE);

	if(bVSB && bHSB)
	{
		CRect box=rcClient;
		box.left=box.right-whScrollbar;
		box.top=box.bottom-whScrollbar;
		m_sBox.MoveWindow(&box);
		m_sBox.ShowWindow(SW_SHOW);
	}
	else
	{
		m_sBox.ShowWindow(SW_HIDE);
	}
}

void CGraphViewCanvas::Scroll(const CPoint& pt)
{
	CClientDC dc(m_hWnd);

	CRect clientrc;
	GetClientRect(&clientrc);
	int whsb=GetSystemMetrics(SM_CYHSCROLL);
	if(m_vSB.IsWindowVisible())
		clientrc.right-=whsb;
	if(m_hSB.IsWindowVisible())
		clientrc.bottom-=whsb;

	CRect updaterc;
	BOOL bOk=FALSE;

	bOk=dc.ScrollDC(
		pt.x,
		pt.y,
		&clientrc,
		&clientrc,
		0,
		&updaterc);

#ifdef _DEBUG
	//CRect rcClient;
	//GetClientRect(&rcClient);
	//if(!rcClient.EqualRect(&updaterc))
	//	ATLTRACE(_T("Partial scroll update rect: l=%d, t=%d, r=%d, b=%d\r\n"),
	//		updaterc.left, updaterc.top, updaterc.right, updaterc.bottom);
	//else
	//	ATLTRACE(_T("Complete scroll client rect update\r\n"));
#endif
	if(bOk && !(m_canvas->GetApplicationVisMode()==RelavintTimeline))
	{
		InvalidateRect(&updaterc, FALSE);
	}
	else
	{
		Invalidate();
	}

	UpdateWindow();

}

void CGraphViewCanvas::KickIt()
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	PointF p;
	for(CComPtr<IVertexIterator> itr = m_graph->GetVertexIterator(); itr->IsValidItem(); itr->NextItem())
	{
		CComPtr<IGraphVertex> vertex = itr->Get();
		CComPtr<IRenderer> ir=m_canvas->GetRenderer(vertex);
		if(ir)
		{
			ir->GetPosition(p);
			p.x *= 2;
			p.y *= 2;
			ir->SetPosition(p);
		}
	}
	InvalidateLayout();
}

BOOL CGraphViewCanvas::PreTranslateMessage(MSG* pMsg)
{
	if((pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) &&
		(pMsg->message < WM_MOUSEFIRST || pMsg->message > WM_MOUSELAST))
		return FALSE;

	HWND hWndCtl = ::GetFocus();

	if(IsChild(hWndCtl))
	{
		// find a direct child of the dialog from the window that has focus
		while(::GetParent(hWndCtl) != m_hWnd)
			hWndCtl = ::GetParent(hWndCtl);

		// give control a chance to translate this message
		if(::SendMessage(hWndCtl, WM_FORWARDMSG, 0, (LPARAM)pMsg) != 0)
			return TRUE;
	}
	return IsDialogMessage(pMsg);
}

LRESULT CGraphViewCanvas::OnCreate(LPCREATESTRUCT lParam)
{
//	SetMsgHandled(false);
	m_canvas->SetTopParent(m_hWnd);
	Stop();
#ifdef SUPPORT_DRAGANDDROP
	// managed control doesn't want this
	InitDragAndDrop();
#endif
	m_hSB.Create(m_hWnd, 0, 0, WS_CHILD | SBS_HORZ | SBS_BOTTOMALIGN, 0, 1); 
	m_vSB.Create(m_hWnd, 0, 0, WS_CHILD | SBS_VERT | SBS_LEFTALIGN, 0, 2); 
	m_sBox.Create(m_hWnd, 0, 0, WS_CHILD | SBS_SIZEBOX | SBS_SIZEBOXBOTTOMRIGHTALIGN, 0, 3); 

	m_tooltip.Create(this);
	m_tooltip.SetNotify();

	m_tooltip.SetDirection(PPTOOLTIP_LEFTEDGE_VCENTER);
	m_tooltip.SetTransparency(0);
	//m_tooltip.SetTooltipShadow(3, 3);
	m_tooltip.SetEffectBk(CPPDrawManager::EFFECT_DIAGSHADE);
	m_tooltip.SetColorBk(RGB(255, 255, 255), RGB(240, 247, 255), RGB(192, 192, 208));
	m_tooltip.SetBorder( RGB(192, 192, 208), 1, 1);
	m_tooltip.SetDelayTime(PPTOOLTIP_TIME_AUTOPOP, 60000);
	m_tooltip.SetDelayTime(PPTOOLTIP_TIME_FADEIN, 0);
	m_tooltip.SetDelayTime(PPTOOLTIP_TIME_FADEOUT, 0);
	m_tooltip.EnableHyperlink();
	m_tooltip.SetCallbackHyperlink(m_hWnd, UM_TOOLTIP_LINKCLICKED);

	PPTOOLTIP_INFO ti;
	ti.nBehaviour = PPTOOLTIP_MULTIPLE_SHOW;
	ti.nIDTool = 0;
	ti.rectBounds = CRect(0, 0, 10000, 10000);
	ti.sTooltip = "";
	ti.nMask = PPTOOLTIP_MASK_BEHAVIOUR;
	ti.nGranularity = 5;
	m_tooltip.AddTool(this, ti);

	//CRect rc;  
	//GetClientRect(&rc);  
	//ClientToScreen(&rc); 
	//m_tooltip.ShowHelpTooltip( &rc.CenterPoint(), _T("Hello, this is a demo"));
	return 1;
}

void CGraphViewCanvas::OnDestroy()
{
	SetMsgHandled(false);
#ifdef SUPPORT_DRAGANDDROP
	RevokeDragDrop(m_hWnd);
#endif

	Stop();
	KillTimer(m_timer);
	m_tooltip.RemoveAllTools();
	if (m_tooltip.IsWindow())
		m_tooltip.DestroyWindow();
	ClearSelected();
}

void CGraphViewCanvas::OnHScroll(int scrollRequest, short scrollPos, HWND hWnd)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	if(m_meta.IsEmulateDecoration())
	{
		SetMsgHandled(false); /// TJ - not sure we need this at all
		return;
	}

	int change=0;

	SCROLLINFO si;
	si.cbSize=sizeof si;
	si.fMask=SIF_RANGE|SIF_POS|SIF_PAGE;

	m_hSB.GetScrollInfo(&si);
	int maxpos=si.nMax-si.nPage+1;

	if(scrollRequest!=SB_THUMBTRACK)
		m_inScrollDrag=false;

	switch(scrollRequest)
	{
	case SB_LINEDOWN:
		if(si.nPos<maxpos)
		{
			change=si.nPage/10;
		}
		break;

	case SB_LINEUP:
		if(si.nPos>si.nMin)
		{
			change=-(int)si.nPage/10;
		}
		break;

	case SB_PAGEDOWN:
		change=si.nPage;
		break;

	case SB_PAGEUP:
		change=-(int)si.nPage;
		break;

	case SB_TOP:
	case SB_BOTTOM:
	case SB_ENDSCROLL:
		break;

	case SB_THUMBTRACK:
		{
			m_inScrollDrag=true;
			PointF oldoffset=m_canvas->GetOffset();
			PointF offset=m_scrollOffset;
			offset.x+=scrollPos;
			change=round_int(offset.x-oldoffset.x);
		}
		break;

	case SB_THUMBPOSITION:
		{
			CRect rcclient;
			GetClientRect(&rcclient);
			UpdateScrollBars(rcclient);
		}
		break;
	}

	if(abs(change)>maxpos)
		change=change>0?maxpos:-maxpos;

	if(change!=0)
	{
		CRect rcclient;
		GetClientRect(&rcclient);
		RectF graphExtent;
		m_canvas->GetGraphExtent(graphExtent);
		graphExtent.Offset(static_cast<GraphTypes::REAL>(-change),0);
		m_canvas->StoreGraphExtent(graphExtent);
		if(scrollRequest!=SB_THUMBTRACK)
			UpdateScrollBars(rcclient);

		PointF offset=m_canvas->GetOffset();
		offset.x+=change;
		m_canvas->SetOffset(offset);

		if(m_canvas->GetApplicationVisMode()==RelavintTimeline)
		{
			m_timeline->ScrollTimeline(change);
			m_timeline->UpdateWindow(); // want immediate redraw
		}
		CPoint pt(-change,0);
		Scroll(pt);
	}
}

void CGraphViewCanvas::OnVScroll(int scrollRequest, short scrollPos, HWND hWnd)
{ 
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	if(m_meta.IsEmulateDecoration())
	{
		SetMsgHandled(false);
		return;
	}

	int change=0;

	SCROLLINFO si;
	si.cbSize=sizeof si;
	si.fMask=SIF_RANGE|SIF_POS|SIF_PAGE;

	m_vSB.GetScrollInfo(&si);
	int maxpos=si.nMax-si.nPage+1;

	if(scrollRequest!=SB_THUMBTRACK)
		m_inScrollDrag=false;

	switch(scrollRequest)
	{
	case SB_LINEDOWN:
		if(si.nPos<maxpos)
		{
			change=si.nPage/10;
		}
		break;

	case SB_LINEUP:
		if(si.nPos>si.nMin)
		{
			change=-(int)si.nPage/10;
		}
		break;

	case SB_PAGEDOWN:
		change=si.nPage;
		break;

	case SB_PAGEUP:
		change=-(int)si.nPage;
		break;

	case SB_TOP:
	case SB_BOTTOM:
	case SB_ENDSCROLL:
		break;

	case SB_THUMBTRACK:
		{
			m_inScrollDrag=true;
			PointF oldoffset=m_canvas->GetOffset();
			PointF offset=m_scrollOffset;
			offset.y+=scrollPos;
			change=round_int(offset.y-oldoffset.y);
		}
		break;

	case SB_THUMBPOSITION:
		{
			CRect rcclient;
			GetClientRect(&rcclient);
			UpdateScrollBars(rcclient);
		}
		break;
	}

	if(abs(change)>maxpos)
		change=change>0?maxpos:-maxpos;

	if(change!=0)
	{
		CRect rcclient;
		GetClientRect(&rcclient);
		RectF graphExtent;
		m_canvas->GetGraphExtent(graphExtent);
		graphExtent.Offset(0, static_cast<GraphTypes::REAL>(-change));
		m_canvas->StoreGraphExtent(graphExtent);
		if(scrollRequest!=SB_THUMBTRACK)
			UpdateScrollBars(rcclient);

		PointF offset=m_canvas->GetOffset();
		offset.y+=change;
		m_canvas->SetOffset(offset);

		CPoint pt(0,-change);
		Scroll(pt);
	}
}

BOOL CGraphViewCanvas::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if(m_canvas->GetApplicationMode()==AppModeQBGraph)
	{
		if (nFlags & MK_CONTROL)
		{
			int delta=zDelta/120;
			if(delta!=0)
			{
				REAL step=CalcScaleStep();
				double old_scale=m_canvas->GetScale();
				double new_scale=old_scale+delta*step;
				if(new_scale<0.005)
					new_scale=0.005;
				if(new_scale>5)
					new_scale=5;
				SetScale(static_cast<float>(new_scale));
				NMSGVHDR msg;
				msg.id = CUniqueID();
				m_canvas->NotifyParent(SCW_NOTIFYSCALECHANGE, &msg);
			}
		}
		else
		{
			int delta=zDelta;//  /120;
			if(delta!=0)
			{
				PointF pos;
				GetOffset(pos);
				pos.y -= delta;
				SetOffset(pos);
				Invalidate(false);
				UpdateWindow();
				InvalidateLayout();
			}
		}
	}
	return 0;
}

void CGraphViewCanvas::OnLButtonDown(UINT vKey, const CPoint &pos)
{
	::SetFocus(m_hWnd);
	SetCapture(); 
	m_bMouseDown = true;
	m_canvas->SendMessage(WM_LBUTTONDOWN, vKey, MAKELPARAM(pos.x,pos.y));
	InvalidateLayout();	
}

void CGraphViewCanvas::OnMouseMove(UINT vKey, const CPoint &pos)
{
	m_canvas->SendMessage(WM_MOUSEMOVE, vKey, MAKELPARAM(pos.x,pos.y));
}

void CGraphViewCanvas::OnLButtonUp(UINT vKey, const CPoint &clientPos)
{
	m_canvas->SendMessage(WM_LBUTTONUP, vKey, MAKELPARAM(clientPos.x,clientPos.y));
	ReleaseCapture(); 
	m_bMouseDown = false;
}

LRESULT CGraphViewCanvas::OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT lRes=FALSE;
	if(HWND(wParam)==m_hWnd)
		lRes=m_canvas->SendMessage(uMsg, wParam, lParam);
	bHandled=lRes!=FALSE;
	return lRes;
}

LRESULT CGraphViewCanvas::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	LRESULT lRes=m_canvas->SendMessage(uMsg, wParam, lParam);
	if(lRes==HTNOWHERE) // don't allow default lRes 0 from code that doesn't handle message
		lRes=HTCLIENT;
	return lRes;
}

void CGraphViewCanvas::OnKeyUp(TCHAR key, UINT p1, UINT p2)
{
	if(m_canvas->GetApplicationMode()==AppModeGrETL)
		InvalidateLayout(); //TODO currently needed only for GrETL dataset editing
}

LRESULT CGraphViewCanvas::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT lRet=0;
#ifdef _DEBUG 
	if(m_canvas->GetApplicationMode()==AppModeQBGraph)
	{
		if((::GetAsyncKeyState(VK_CONTROL)&0x8000))
		{
			if(wParam==VK_ADD)
				m_canvas->SetApplicationZoomMode(AZMZoomIn);
			else if(wParam==VK_SUBTRACT)
				m_canvas->SetApplicationZoomMode(AZMZoomOut);
			else if(wParam==VK_DECIMAL)
				m_canvas->SetApplicationZoomMode(AZMSelection);
			else if(wParam==VK_MULTIPLY)
				m_canvas->SetApplicationZoomMode(AZMMouseWheel);
			else if(wParam=='I')
				QBUI::LaunchFindDialog(this);
		}
	}
	else
#endif
	{
		lRet=m_canvas->SendMessage(uMsg, wParam, lParam);
	}
	return lRet;
}

LRESULT CGraphViewCanvas::OnGetDlgCode(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return DLGC_WANTALLKEYS;
}

LRESULT CGraphViewCanvas::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	return m_canvas->SendMessage(uMsg, wParam, lParam);
}

void CGraphViewCanvas::OnLButtonDblClick(UINT vKey, const CPoint &clientPos)
{
	m_canvas->SendMessage(WM_LBUTTONDBLCLK, vKey, MAKELPARAM(clientPos.x,clientPos.y));
	//AtlTrace("%s x:%ld y:%ld\n", "CGraphViewCanvas::OnLButtonDblClick", screenPos.x, screenPos.y);
}

void CGraphViewCanvas::OnSetFocus(HWND)
{
	// Relavint wants to maintain separate control and Windows focus
	if(m_canvas->GetApplicationMode()==AppModeRelavint)
		baseClass::GetParent().GetParent().SetFocus();
}

void CGraphViewCanvas::OnKillFocus(HWND)
{
	// Relavint wants to maintain separate control and Windows focus
	if(m_canvas->GetApplicationMode()==AppModeRelavint)
		m_canvas->ClearFocus();
}

void CGraphViewCanvas::OnContextMenu(HWND hWnd, const CPoint &screenPos)
{
	CPoint p = screenPos;
	if(p.x==-1 && p.y==-1) // keyboard
		GetCursorPos(&p);

	ScreenToClient(&p);
	if(p.x<0)
		p.x=0;
	if(p.y<0)
		p.y=0;

	LRESULT result = 0;
	CScaledWindow * hotWin = m_canvas->GetCurrentHot();
	if (hotWin)
	{
		result = hotWin->SendMessage(WM_CONTEXTMENU, (WPARAM)hWnd, MAKELPARAM(p.x, p.y));
	}
	else
	{
		result = m_canvas->SendMessage(WM_CONTEXTMENU, (WPARAM)hWnd, MAKELPARAM(screenPos.x,screenPos.y));
	}
	if (result == 0 && m_Owner != 0)
	{
		LRESULT lRes=0;
		// TODO: in future all this will be pushed lower down
		m_canvas->ReleaseCapture();

		// TODO: change the lower levels to listen on WM_CONTEXT instead
		WPARAM wParam=0;
		if(::GetAsyncKeyState(VK_CONTROL)&0x8000)
			wParam=MK_CONTROL;
		lRes=m_canvas->SendMessage(WM_RBUTTONDOWN, wParam, MAKELPARAM(p.x,p.y));
		ClientToScreen(&p);

		if(lRes!=0)
		{
			CComPtr<IGraphItem> gi = reinterpret_cast<IGraphItem*>(lRes);

			if (m_meta.IsVisible(gi))
			{
				if (CComQIPtr<IGraphVertex> gv = gi)
					m_Owner->OnVertexContextMenu(gi->GetID(), p);
				else if(CComQIPtr<IGraphEdge> ge = gi)
					m_Owner->OnEdgeContextMenu(gi->GetID(), p);
				else if(CComQIPtr<IGraphSubgraph> sg = gi)
					m_Owner->OnSubgraphContextMenu(gi->GetID(), p);
			}
		}
		else
		{
			m_Owner->OnContextMenu(p);
		}
	}
}

LRESULT CGraphViewCanvas::OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	return 0; // not needed?
}

void CGraphViewCanvas::OnMouseLeave()
{
	mouseCurrentPos.SetPoint(-1, -1);
	m_canvas->SendMessage(WM_MOUSELEAVE, 0, 0);
}

LRESULT CGraphViewCanvas::OnLayoutFinished(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	assert(!m_running);
	GetRenderExtent();

	bool noRecenter = false;
	if (m_Owner)
		noRecenter = m_Owner->OnLayoutFinished();

	if (!noRecenter)
	{
		NMSGVHDR msg;
		msg.id = CUniqueID();
		LRESULT result = m_canvas->NotifyParent(SCW_NOTIFYLAYOUTFINISHED, &msg);
		noRecenter = (result != 0);
	}

	if (!noRecenter && m_layout != m_layout_null && (m_prevCenter.x || m_prevCenter.y))
	{
		bool springLayout = (m_layout == m_layout_spring1);
		if (!springLayout)
		{
			GraphTypes::PointF center;
			GraphTypes::RectF extent = GetRenderExtent();
			if (m_canvas->HasVertexFocus())
			{
				CRect pos;
				m_canvas->m_currentFocus->GetWindowRect(pos);
				center.x = static_cast<GraphTypes::REAL>(pos.left);
				center.y = static_cast<GraphTypes::REAL>(pos.top);
				ATLTRACE("v ");
			}
			else
			{
				extent.GetCenter(&center);
				ATLTRACE("g ");
			}

			ATLTRACE("pos after %f, %f\n", center.x, center.y);
			m_canvas->RAllTransform(center);
			m_canvas->RAllTransform(m_prevCenter);
			PointF delta = center - m_prevCenter;
			PointF offset = m_canvas->GetOffset();

			MoveAllChildren(delta);

			PointFVector endPos;
			GetGraphLayout(endPos);

			morph(m_prevLayout, endPos);
		}
	}
	m_layout = m_layout_null;

	return 0;
};

LRESULT CGraphViewCanvas::OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if (m_running && m_layout != m_layout_spring1)
		return 0;

	clib::recursive_mutex::scoped_lock lock(m_mutex);
	CPaintDC dc(m_hWnd);
	CRect rc;
	dc.GetClipBox(&rc);

	CRect rcclient;
	GetClientRect(&rcclient);
	if (!m_message.empty())
	{
		boost::scoped_ptr<CAggDC> aggdc(CAggDC::CreateBlitMemoryDC(dc, m_canvas->m_fonts, m_hWnd, &rc));
		aggdc->TransformReset();
		if(IsReadOnly())
			aggdc->Rect(rcclient.TopLeft(), rcclient.BottomRight(), Color::Gray, Color::Gray);
		aggdc->DrawScaledText(m_message.c_str(), GraphTypes::RectF(static_cast<GraphTypes::REAL>(rcclient.CenterPoint().x), static_cast<GraphTypes::REAL>(rcclient.CenterPoint().y), 140.0f, 20.0f), Color::Black, "Verdana", 22, GraphTypes::PointF(0.0f, 0.0f));
	}
	else if(IsReadOnly())
	{
		boost::scoped_ptr<CAggDC> aggdc(CAggDC::CreateBlitMemoryDC(dc, m_canvas->m_fonts, m_hWnd, &rc));
		aggdc->TransformReset();
		aggdc->Rect(rcclient.TopLeft(), rcclient.BottomRight(), Color::Gray, Color::Gray);
	}
	else if (!m_NoRedraw)
	{
		//static int i = 0;
		//ATLTRACE("Painting:  %d\n", i++);
		//m_tooltip.SetToolRect(this, 0, rcclient);
		{
//#ifdef _DEBUG
//			if(rcclient.EqualRect(&rc))
//				ATLTRACE("Painting entire client\r\n");
//			else
//				ATLTRACE("Painting chunk\r\n");
//#endif
			if(!rc.IsRectEmpty())
			{
				boost::scoped_ptr<CAggDC> aggdc(CAggDC::CreateBlitMemoryDC(dc, m_canvas->m_fonts, m_hWnd, &rc));
				if(m_canvas->GetApplicationMode()==AppModeQBGraph)
				{
					REAL scale=m_canvas->GetScale();
					if(scale<0.025)
						aggdc->SetGammaThreshold(scale);
				}

				DoPaint(*aggdc);	//  Candidate of exception handling.
				if(!m_meta.IsEmulateDecoration())
				{
					if(m_meta.IsOverviewVisible() && m_overview->IsWindow())
						m_overview->Refresh();

					if(!m_inScrollDrag) // no update while dragging scrollbar thumb
						UpdateScrollBars(rcclient);
				}
			}
		}
	}
	if (!m_message.empty())
	{
		boost::scoped_ptr<CAggDC> aggdc(CAggDC::CreateBlitMemoryDC(dc, m_canvas->m_fonts, m_hWnd, &rc));
		aggdc->TransformReset();
		aggdc->DrawScaledText(m_message.c_str(), GraphTypes::RectF(static_cast<GraphTypes::REAL>(rcclient.CenterPoint().x), static_cast<GraphTypes::REAL>(rcclient.CenterPoint().y), 140.0f, 20.0f), Color::Black, "Verdana", 22, GraphTypes::PointF(0.0f, 0.0f));
	}
	if (m_hWnd == ::GetFocus())
		dc.DrawFocusRect(rcclient);
    return 0;
}

LRESULT CGraphViewCanvas::OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return 1; // handled; no need to erase background; do it in DoPaint();
}

LRESULT CGraphViewCanvas::OnThemeChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	m_canvas->NotifyThemeChanged();
	bHandled=FALSE;
	return 0;
}

LRESULT CGraphViewCanvas::OnNotifyTooltipLinkClick(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
	NMTOOLTIPLINKCLICK msg = {0};
	try
	{
		unsigned int hashId = boost::lexical_cast<unsigned int>((const TCHAR *)wParam);
		if (CComPtr<IGraphVertex> vertex = m_graph->GetVertex(hashId))
			msg.target = vertex->GetID();
		else if (CComPtr<IGraphEdge> edge = m_graph->GetEdge(hashId))
			msg.target = edge->GetID();
		else if (CComPtr<IGraphSubgraph> subgraph = m_graph->GetSubgraph(hashId))
			msg.target = subgraph->GetID();
	}
	catch (boost::bad_lexical_cast &)
	{
		msg.definition = (const TCHAR *)wParam;
	}

	m_tooltip.HideTooltip();
	m_canvas->NotifyParent(SCW_NOTIFYTOOLTIPLINKCLICK, &msg);
	return 0;
}

void CGraphViewCanvas::DoPaint(CAggDC& dc, bool calculateExtent)
{
	if (!m_NoRedraw)
		m_canvas->SendMessage(WM_PAINT, (WPARAM)&dc, 0);
}

#define THRESHOLD 1.0f
//#define MORPH_STEP 1.5f // made it calculated based on num of items (morphs less on very complex maps)

// N.B. we are passing the same start and end values when we want morph to ignore them

void CGraphViewCanvas::morph(const PointFVector &startPos, const PointFVector &endPos)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	PointF offset(0L,0L);
	REAL scale=m_canvas->GetScale();
	morph(startPos, endPos, scale, scale, offset, offset);
}

void CGraphViewCanvas::morph(REAL startScale, REAL endScale)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	PointF offset(0L,0L);
	PointFVector pos;
	morph(pos, pos, startScale, endScale, offset, offset);
}

void CGraphViewCanvas::morph(const PointF& startOffset, const PointF& endOffset)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	PointFVector pos;
	REAL scale=m_canvas->GetScale();
	morph(pos, pos, scale, scale, startOffset, endOffset);
}

void CGraphViewCanvas::morph(const PointFVector &startPos, const PointFVector &endPos, REAL startScale, REAL endScale)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	PointF offset(0L,0L);
	morph(startPos, endPos, startScale, endScale, offset, offset);
}

void CGraphViewCanvas::morph(const PointFVector &startPos, const PointFVector &endPos, const PointF& startOffset, const PointF& endOffset)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	REAL scale=m_canvas->GetScale();
	morph(startPos, endPos, scale, scale, startOffset, endOffset);
}

void CGraphViewCanvas::morph(REAL startScale, REAL endScale, const PointF& startOffset, const PointF& endOffset)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	PointFVector pos;
	morph(pos, pos, startScale, endScale, startOffset, endOffset);
}

void CGraphViewCanvas::morph(const PointFVector &_startPos, const PointFVector &endPos, REAL _startScale, REAL endScale, const PointF& _startOffset, const PointF& endOffset)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);

	// for now take this approach for all modes
	//if (m_canvas->GetApplicationMode() == AppModeQBGraph)
	{
		if (_startPos.size() > 0)
			SetGraphLayout(endPos);
		m_canvas->SetOffset(endOffset);
		m_canvas->SetScale(endScale);
		Invalidate(false);
		UpdateWindow();
		InvalidateLayout();
		return;
	}

	if (endPos.size() != _startPos.size())
	{
		ATLTRACE("Items added during background layout (%i == %i)\n", endPos.size(), _startPos.size());
		return;
	}

	IGraphItemVector children;
	if(_startPos.size()>0)
	{
		unsigned numc=m_graph->GetAllChildren(children);
		assert(endPos.size() == numc );
	}

	REAL MORPH_STEP = endPos.size() ? 1.0f + 100.0f / endPos.size() : 1.5f;
	if (MORPH_STEP > 2.0f)
		MORPH_STEP = 2.0f;

	PointFVector startPos = _startPos;

	REAL startScale = _startScale;
	PointF scrollStartPos=_startOffset, scrollEndPos=endOffset;

	HANDLE hThreadMe = ::GetCurrentThread();
	int priority = ::GetThreadPriority(hThreadMe);
	::SetThreadPriority(hThreadMe, THREAD_PRIORITY_NORMAL);

#define NEWMORPH
#ifdef NEWMORPH
	for(int i = 0; i < 10; ++i)
	{
		if(_startScale!=endScale)
		{
			REAL scaleVal = (endScale - startScale) / MORPH_STEP;
			startScale += scaleVal;
			m_canvas->SetScale(startScale);
		}

		if (_startOffset.x!=endOffset.x || _startOffset.y!=endOffset.y)
		{
			REAL deltaX = scrollEndPos.x - scrollStartPos.x;
			REAL deltaY = scrollEndPos.y - scrollStartPos.y;

			REAL xVal = deltaX / MORPH_STEP;
			REAL yVal = deltaY / MORPH_STEP;

			scrollStartPos.x += xVal;
			scrollStartPos.y += yVal;

			m_canvas->SetOffset(scrollStartPos);
		}
		if(_startPos.size()>0)
		{
			int index = 0;
			IGraphItemVector::iterator ic;
			REAL xVal, yVal;

			for(ic=children.begin(); ic!=children.end(); ++ic, ++index)
			{
				REAL deltaX = endPos[index].x - startPos[index].x;
				REAL deltaY = endPos[index].y - startPos[index].y;

				xVal = deltaX / MORPH_STEP;
				yVal = deltaY / MORPH_STEP;

				startPos[index].x += xVal;
				startPos[index].y += yVal;
			}
		
			SetGraphLayout(startPos);
		}

		if (IsWindow() && IsWindowVisible())
		{
			Invalidate(false);
			UpdateWindow();
		}
	}
#else
	while(true)
	{
		bool done = true;

		if(_startScale!=endScale)
		{
			REAL scaleVal = (endScale - startScale) / MORPH_STEP;
			if (scaleVal * 100 >= -THRESHOLD && scaleVal * 100 <= THRESHOLD)
				startScale = endScale;
			else
			{
				startScale += scaleVal;
				done = false;
			}

			m_canvas->SetScale(startScale);
		}

		if (_startOffset.x!=endOffset.x || _startOffset.y!=endOffset.y)
		{
			REAL deltaX = scrollEndPos.x - scrollStartPos.x;
			REAL deltaY = scrollEndPos.y - scrollStartPos.y;

			REAL xVal = deltaX / MORPH_STEP;
			REAL yVal = deltaY / MORPH_STEP;

			if (xVal >= -THRESHOLD && xVal <= THRESHOLD)
				scrollStartPos.x = scrollEndPos.x;
			else
			{
				scrollStartPos.x += xVal;
				done = false;
			}

			if (yVal >= -THRESHOLD && yVal <= THRESHOLD)
				scrollStartPos.y = scrollEndPos.y;
			else
			{
				scrollStartPos.y += yVal;
				done = false;
			}

			m_canvas->SetOffset(scrollStartPos);
		}

		if(_startPos.size()>0)
		{
			int index = 0;
			IGraphItemVector::iterator ic;
			REAL xVal, yVal;

			for(ic=children.begin(); ic!=children.end(); ++ic, ++index)
			{
				REAL deltaX = endPos[index].x - startPos[index].x;
				REAL deltaY = endPos[index].y - startPos[index].y;

				xVal = deltaX / MORPH_STEP;
				yVal = deltaY / MORPH_STEP;

				if (xVal >= -THRESHOLD && xVal <= THRESHOLD)
					startPos[index].x = endPos[index].x;
				else
				{
					startPos[index].x += xVal;
					done = false;
				}

				if (yVal >= -THRESHOLD && yVal <= THRESHOLD)
				{
					startPos[index].y = endPos[index].y;
				}
				else
				{
					startPos[index].y += yVal;
					done = false;
				}
			}
		
			SetGraphLayout(startPos);
		}

		if (IsWindow() && IsWindowVisible())
		{
			Invalidate(false);
			UpdateWindow();
		}

		if (done)
		{
			break;
		}
	}
#endif
	::SetThreadPriority(hThreadMe, priority);

	// from here we just store the new position(s)
	if(_startPos.size()>0)
		SetGraphLayout(endPos);

	if (_startOffset.x!=endOffset.x ||_startOffset.y!=endOffset.y)
		m_canvas->SetOffset(endOffset);

	if(_startScale!=endScale)
		m_canvas->SetScale(endScale);

	InvalidateLayout();
}

int CGraphViewCanvas::ScrollWindowEx(int dx, int dy, UINT uFlags, LPCRECT lpRectScroll, LPCRECT lpRectClip, HRGN hRgnUpdate, LPRECT lpRectUpdate) throw()
{
	assert(::IsWindow(m_hWnd));
	return 0;
}

bool ContainsWholeWord(const std::_tstring & str, const std::_tstring & find)
{
	std::vector<std::_tstring> ResultCopy;
	boost::algorithm::split(ResultCopy, str,  boost::algorithm::is_space(), boost::algorithm::token_compress_on);
	if (std::find(ResultCopy.begin(), ResultCopy.end(), find) != ResultCopy.end())
	{
		return true;
	}
	return false;
}

void CGraphViewCanvas::Find(IUniqueIDContainer* results, const TCHAR* searchText, bool searchLabel, bool searchProperties, bool caseSensitive, bool visibleOnly, bool wholeWord, const GUID& pluginId, const int categoryId, const int propertyId)
{
	if (searchText == 0 || _tcslen(searchText) == 0)
		return;

	// string to look for
	CString find = searchText;

	if (!caseSensitive)
		find = find.MakeLower();

	// the current string item to match on
	CString item;

	// loop through all nodes
	CBuildingEdgeVisibility building(m_buildingEdgeVisibility);
	for(CComPtr<IVertexIterator> itr = m_graph->GetVertexIterator(); itr->IsValidItem(); itr->NextItem())
	{
		CComPtr<IGraphVertex> vertex = itr->Get();
		if (vertex != NULL)
		{
			// skip invisible vertices or not?
			if (visibleOnly && !m_meta.IsVisible(vertex))
				continue;

			// plugin check
			if (pluginId == GUID_NULL || vertex->GetID().GetPlugin() == pluginId)
			{
				// category check
				if (categoryId == -1 || categoryId == vertex->GetID().GetCategory().GetCategory())
				{
					if (searchLabel)
					{
						item = vertex->GetLabel();

						if (!caseSensitive)
							item = item.MakeLower();

						if (wholeWord)
						{
							if (ContainsWholeWord(static_cast<const TCHAR *>(item), static_cast<const TCHAR *>(find)))
							{
								results->Add(vertex->GetID());
								continue;
							}
						}
						else if (item.Find(find) != -1)
						{
							results->Add(vertex->GetID());
							continue;
						}
					}

					if (searchProperties)
					{
						CPropertyIDVector properties;
						vertex->GetProperties(&properties);

						// search all properties
						while (properties.MoveNext())
						{
							int currentPropId = properties.Get();

							if (propertyId == -1 || propertyId == currentPropId)
							{
								CComVariant result;
								vertex->GetProperty(currentPropId, result);
								if (result.vt == VT_BSTR)
									item = result;
								else
									item.Empty();

								if (!caseSensitive)
									item = item.MakeLower();

								if (wholeWord)
								{
									if (ContainsWholeWord(static_cast<const TCHAR *>(item), static_cast<const TCHAR *>(find)))
									{
										results->Add(vertex->GetID());
										continue;
									}
								}
								else  if (item.Find(find) != -1)
								{
									results->Add(vertex->GetID());
									break;
								}
							}
						}
					}
				}
			}
		}
	}
}

#include "AggBitmap.h"

void CGraphViewCanvas::RenderTo(HDC hDC, int x, int y, int pageX, int pageY, int pageW, int pageH)
{
	if(::GetMapMode(hDC)!=MM_TEXT)
		::SetMapMode(hDC, MM_TEXT);

	// test for a valid dc
	assert(GetDeviceCaps(hDC, LOGPIXELSX) != 0);
	// debug tracing
	ATLTRACE(_T("RenderTo:  %d, %d, %d, %d, %d, %d\n"), x, y, pageX, pageY, pageW, pageH);

	// used later
	CRect pageRect(pageX, pageY, pageX+pageW, pageY+pageH);

	if(pageRect.Width()!=0 || pageRect.Height()!=0)
	{
		CRgn rgn = CreateRectRgn(pageRect.left, pageRect.top, pageRect.right, pageRect.bottom);
		SelectClipRgn(hDC, rgn);
	}
	
	SetViewportOrgEx(hDC, -x+pageRect.left, -y+pageRect.top, 0);

	CDC cdc(hDC);
	{
		//boost::scoped_ptr<CAggDC> aggdc(CAggDC::CreateGDIDC(cdc, m_canvas->m_fonts));
		//m_canvas->SendMessage(WM_PAINT, (WPARAM)(aggdc.get()), 0);

		bool oldEmulateDecoration = m_meta.IsEmulateDecoration();
		m_meta.EmulateDecoration(false);
		CClientDC hDC2(m_hWnd);
		//if(::GetMapMode(hDC2)!=MM_TEXT)
		//	::SetMapMode(hDC2, MM_TEXT);
		//SetViewportOrgEx(hDC2, -x+pageRect.left, -y+pageRect.top, 0);
		CDC cdc2(hDC2);
		boost::scoped_ptr<CAggDC> aggdc2(CAggDC::CreateMemoryDC(cdc2, m_canvas->m_fonts, &pageRect));
		aggdc2->Transform(PointF((float)-x, (float)-y));
		//aggdc2->Line(GraphTypes::PointF(0.0f, 0.0f), GraphTypes::PointF((LONG)pageW, (LONG)pageH), 0xf0f0f0f0);
		m_canvas->SendMessage(WM_PAINT, (WPARAM)(aggdc2.get()), 0);
		//CAggBitmap bmp;
		//boost::scoped_ptr<CAggDC> tmpaggdc(bmp.BeginPaint(*aggdc2, CSize(pageW, pageH), m_canvas->m_fonts));
		//aggdc->Line(GraphTypes::PointF(0.0f, 0.0f), GraphTypes::PointF((LONG)pageW, (LONG)pageH), 0xf0f0f0f0);
		//bmp.EndPaint(tmpaggdc.get());
		//bmp.Draw(*aggdc2, CRect(0, 0, pageW, pageH), 0);

		//cdc.SelectBitmap(aggdc2->DetachBitmap());
		cdc.BitBlt(x, y, pageW, pageH, *aggdc2, 0, 0, SRCCOPY);
		//CImage image;
		//image.Attach(aggdc2->DetachBitmap());
		//image.Save(_T("c:\\temp\\tst.png"));
		m_meta.EmulateDecoration(oldEmulateDecoration);
	}
	cdc.Detach();
	SelectClipRgn(hDC, 0);
}

void CGraphViewCanvas::RenderTo(CImage &result)
{
	RectF rect = GetRenderExtent();
	RenderTo(result, (int)rect.x, (int)rect.y, (int)rect.Width, (int)rect.Height);
}

void CGraphViewCanvas::RenderTo(CImage &result, int x, int y, int w, int h)
{
	CClientDC dc(m_hWnd);
	CMemBm bmp(dc, NULL, 0, 0, w, h);
	CDCHandle hdc(bmp);
	hdc.FillSolidRect(0,0,w,h,GetSysColor(COLOR_WINDOW));
	RenderTo(bmp, x, y, 0, 0, w, h);
	result.Attach(bmp.Detach());
}

void CGraphViewCanvas::RenderTo(CBitmap &result)
{
	RectF rect = GetRenderExtent();
	RenderTo(result, (int)rect.x, (int)rect.y, (int)rect.Width, (int)rect.Height);
}

void CGraphViewCanvas::RenderTo(CBitmap &result, int x, int y, int w, int h)
{
	CClientDC dc(m_hWnd);
	CMemBm bmp(dc, NULL, 0, 0, w, h);
	CDCHandle hdc(bmp);
	hdc.FillSolidRect(0,0,w,h,GetSysColor(COLOR_WINDOW));
	RenderTo(bmp, x, y, 0, 0, w, h);
	result = bmp.Detach();
}

DWORD CGraphViewCanvas::RenderToClipboard(bool emptyFirst)
{
	CImage bmp;
	RenderTo(bmp);

	HRESULT retVal = ERROR_SUCCESS;
	OpenClipboard();
	if (emptyFirst)
		EmptyClipboard();
	if (SetClipboardData(CF_BITMAP, bmp.Detach()) == NULL)
		retVal = GetLastError();
	CloseClipboard();

	return retVal;
}

void errhandler(const std::string & msg, HWND hWnd)
{
	assert(false);
}

PBITMAPINFO CreateBitmapInfoStruct(HWND hwnd, HBITMAP hBmp)
{ 
	BITMAP bmp; 
	PBITMAPINFO pbmi; 
	WORD    cClrBits; 

	// Retrieve the bitmap color format, width, and height. 
	if (!GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp)) 
		errhandler("GetObject", hwnd); 

	// Convert the color format to a count of bits. 
	cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel); 
	if (cClrBits == 1) 
		cClrBits = 1; 
	else if (cClrBits <= 4) 
		cClrBits = 4; 
	else if (cClrBits <= 8) 
		cClrBits = 8; 
	else if (cClrBits <= 16) 
		cClrBits = 16; 
	else if (cClrBits <= 24) 
		cClrBits = 24; 
	else cClrBits = 32; 

	// Allocate memory for the BITMAPINFO structure. (This structure 
	// contains a BITMAPINFOHEADER structure and an array of RGBQUAD 
	// data structures.) 

	if (cClrBits != 24) 
		pbmi = (PBITMAPINFO) LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * (1<< cClrBits)); 

	// There is no RGBQUAD array for the 24-bit-per-pixel format. 

	else 
		pbmi = (PBITMAPINFO) LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER)); 

	// Initialize the fields in the BITMAPINFO structure. 

	pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
	pbmi->bmiHeader.biWidth = bmp.bmWidth; 
	pbmi->bmiHeader.biHeight = bmp.bmHeight; 
	pbmi->bmiHeader.biPlanes = bmp.bmPlanes; 
	pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel; 
	if (cClrBits < 24) 
		pbmi->bmiHeader.biClrUsed = (1<<cClrBits); 

	// If the bitmap is not compressed, set the BI_RGB flag. 
	pbmi->bmiHeader.biCompression = BI_RGB; 

	// Compute the number of bytes in the array of color 
	// indices and store the result in biSizeImage. 
	// For Windows NT, the width must be DWORD aligned unless 
	// the bitmap is RLE compressed. This example shows this. 
	// For Windows 95/98/Me, the width must be WORD aligned unless the 
	// bitmap is RLE compressed.
	pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits +31) & ~31) /8 * pbmi->bmiHeader.biHeight; 
	// Set biClrImportant to 0, indicating that all of the 
	// device colors are important. 
	pbmi->bmiHeader.biClrImportant = 0; 
	return pbmi; 
} 

void CreateBMPFile(HWND hwnd, LPCTSTR pszFile, PBITMAPINFO pbi, HBITMAP hBMP, HDC hDC) 
{ 
	HANDLE hf;                 // file handle 
	BITMAPFILEHEADER hdr;       // bitmap file-header 
	PBITMAPINFOHEADER pbih;     // bitmap info-header 
	LPBYTE lpBits;              // memory pointer 
	DWORD dwTotal;              // total count of bytes 
	DWORD cb;                   // incremental count of bytes 
	BYTE *hp;                   // byte pointer 
	DWORD dwTmp; 

	pbih = (PBITMAPINFOHEADER) pbi; 
	lpBits = (LPBYTE) GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

	if (!lpBits) 
		errhandler("GlobalAlloc", hwnd); 

	// Retrieve the color table (RGBQUAD array) and the bits 
	// (array of palette indices) from the DIB. 
	if (!GetDIBits(hDC, hBMP, 0, (WORD) pbih->biHeight, lpBits, pbi, DIB_RGB_COLORS)) 
	{
		errhandler("GetDIBits", hwnd); 
	}

	// Create the .BMP file. 
	hf = CreateFile(pszFile, 
		GENERIC_READ | GENERIC_WRITE, 
		(DWORD) 0, 
		NULL, 
		CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		(HANDLE) NULL); 
	if (hf == INVALID_HANDLE_VALUE) 
		errhandler("CreateFile", hwnd); 
	hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M" 
	// Compute the size of the entire file. 
	hdr.bfSize = (DWORD) (sizeof(BITMAPFILEHEADER) + pbih->biSize + pbih->biClrUsed * sizeof(RGBQUAD) + pbih->biSizeImage); 
	hdr.bfReserved1 = 0; 
	hdr.bfReserved2 = 0; 

	// Compute the offset to the array of color indices. 
	hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) + pbih->biSize + pbih->biClrUsed * sizeof (RGBQUAD); 

	// Copy the BITMAPFILEHEADER into the .BMP file. 
	if (!WriteFile(hf, (LPVOID) &hdr, sizeof(BITMAPFILEHEADER), (LPDWORD) &dwTmp,  NULL)) 
	{
		errhandler("WriteFile", hwnd); 
	}

	// Copy the BITMAPINFOHEADER and RGBQUAD array into the file. 
	if (!WriteFile(hf, (LPVOID) pbih, sizeof(BITMAPINFOHEADER) + pbih->biClrUsed * sizeof (RGBQUAD), (LPDWORD) &dwTmp, (NULL)))
		errhandler("WriteFile", hwnd); 

	// Copy the array of color indices into the .BMP file. 
	dwTotal = cb = pbih->biSizeImage; 
	hp = lpBits; 
	if (!WriteFile(hf, (LPSTR) hp, (int) cb, (LPDWORD) &dwTmp,NULL)) 
		errhandler("WriteFile", hwnd); 

	// Close the .BMP file. 
	if (!CloseHandle(hf)) 
		errhandler("CloseHandle", hwnd); 

	// Free memory. 
	GlobalFree((HGLOBAL)lpBits);
}

bool CGraphViewCanvas::SaveAs() 
{
	CString strExporters;
	CString strExporterDefault;
	CSimpleArray<GUID> aguidFileTypes;
	if (SupportsJPEG())
	{
		CImage img;
		img.GetExporterFilterString(strExporters, aguidFileTypes);
		strExporterDefault = _T(".JPG");
	}
	else
	{
		strExporters = _T("BMP (*.BMP;*.DIB;*.RLE)|*.BMP||");
		strExporterDefault = _T(".BMP");
	}

	if(GetApplicationMode()==AppModeRelavint)
	{
		int pos=strExporters.Find(_T("||"));
		if(pos!=-1)
		{
			pos++;
			strExporters.Insert(pos, _T("Relavint+ document (*.REL)|*.REL||"));
		}
	}

	LPTSTR pch = strExporters.GetBuffer(0); // modify the buffer in place
	while ((pch = _tcschr(pch, '|')) != NULL)
		*pch++ = '\0';

	CFileDialogEx wndFileDialog(FALSE, strExporterDefault, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strExporters, m_hWnd);
	if (IDOK == wndFileDialog.DoModal())
	{
		CString ext=wndFileDialog.GetFileExt();
		if(ext.CompareNoCase(_T(".rel"))==0)
		{
			PointF offset;
			GetOffset(offset);
			CPoint ptoff=offset;
			std::_tstring xml;
			SaveToXml(xml, SGV::XML_SAVE_FORMAT_WITH_IMAGES, ptoff);
			std::_tstring pathName = wndFileDialog.GetPathName().GetString();
			CUnicodeFile f;
			if(f.Create(pathName.c_str(), GENERIC_WRITE, CREATE_ALWAYS, CUnicodeFile::ENCODING_UTF_16))
			{
				f.Write(xml.c_str());
				f.Close();
			}
		}
		else
		{
			Save(wndFileDialog.GetPathName());
		}
	}
	
	InvalidateLayout();
	return true;
}

BOOL CGraphViewCanvas::Save(LPCTSTR lpszFileName) 
{
	if (SupportsJPEG())
	{
		CImage img;
		RenderTo(img);
		return img.Save(lpszFileName);
	}

	CBitmap bmp;
	RenderTo(bmp);

	PBITMAPINFO pbi = CreateBitmapInfoStruct(m_hWnd, bmp);
	CreateBMPFile(m_hWnd, lpszFileName, pbi, bmp, CDC(GetDC()));

	return TRUE;
}

bool CGraphViewCanvas::SupportsJPEG() const
{
	bool fRet = true;
	DWORD dwToken = 0;
	HMODULE hmod = LoadLibrary(_T("gdiplus.dll"));
	if (!hmod)
		return false;
	FreeLibrary(hmod);

	try
	{
		Gdiplus::GdiplusStartupInput input;
		Gdiplus::GdiplusStartupOutput output;
		if(Gdiplus::GdiplusStartup(&dwToken, &input, &output) != Gdiplus::Ok)
			fRet = false;

		if(dwToken != 0)
			Gdiplus::GdiplusShutdown(dwToken);
	}
	catch(...)	//Module not found
	{
		fRet = false;
	}

	return fRet;
}

void CGraphViewCanvas::SaveToXml(std::_tstring& xml, SGV::XML_SAVE_FORMAT saveFormat, const CPoint& relativeSavePos)
{
	m_xmlSerializerAnb.Save(xml, saveFormat, relativeSavePos);
}

void CGraphViewCanvas::SaveToXmlFile() 
{
	CString strExporters = _T("XML Document|*.XML||");
	CString strExporterDefault = _T(".XML");

	LPTSTR pch = strExporters.GetBuffer(0); // modify the buffer in place
	while ((pch = _tcschr(pch, '|')) != NULL)
		*pch++ = '\0';

	CFileDialogEx wndFileDialog(FALSE, strExporterDefault, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strExporters, m_hWnd);
	if (IDOK == wndFileDialog.DoModal())
	{
		CAtlFile file;
		file.Create(wndFileDialog.GetPathName(), GENERIC_READ|GENERIC_WRITE, FILE_SHARE_WRITE, CREATE_ALWAYS);

		std::_tstring xml;
		SaveToXml(xml, SGV::XML_SAVE_FORMAT_ANB_COMPATIBLE);

		// replace "UTF-16" with "UTF-8 " in the xml doc since setting the encoding
		// as UTF-8 using MSXML doesn't work!
		//strncpy(strstr(xml, "UTF-16"), "UTF-8\" ", 7);

		file.Write(CT2W(xml.c_str(), CP_UTF8), xml.length());
	}
}

void CGraphViewCanvas::operator()(LAYOUTEVENT e)
{
	switch(e)
	{
	case LAYOUTEVENT_FINISHED:
		PostMessage(UM_LAYOUT_FINISHED);
		break;
	case LAYOUTEVENT_ITERATION:
		if (IsWindow() && IsWindowVisible())
		{
			//ATLTRACE("LayoutInvalidate\n");
			Invalidate(false);
		}
		break;
	}
}

void CGraphViewCanvas::InitExternalMaps(IGraphVertex * v, const PointF & defaultPos)
{
	assert(v);

	CComPtr<IRenderer> ir=m_canvas->GetRenderer(v);
	assert(ir);
	if(ir)
		ir->SetPosition(defaultPos);

	v->SetVisible(VISIBLE_DEFAULT);

	m_layout_spring1->append(v, defaultPos);
	m_layout_circle->append(v, defaultPos);
	m_layout_timeline->append(v, defaultPos);
#ifdef LAYOUT_LEE
	m_layout_lee->append(v ,defaultPos);
#endif
	m_layout_neato->append(v, defaultPos);
	m_layout_fdp->append(v, defaultPos);
	m_layout_twopi->append(v, defaultPos);
	m_layout_dot->append(v, defaultPos);
	m_layout_circo->append(v, defaultPos);
	m_layout_null->append(v, defaultPos);

	InvalidateLayout();
}

void CGraphViewCanvas::InvalidateLayout()
{
	if (IsWindow() && IsWindowVisible() && !m_running && m_NoRedraw == 0)
	{
		Invalidate(false);
	}
}

LRESULT CGraphViewCanvas::OnNotifyDisplayTooltip(WPARAM wParam, LPNMHDR lpnmhdr, BOOL& bHandled)
{
	//*result = 0;
	if (CScaledWindow * hotWindow = m_canvas->GetCurrentHot())
	{
		NM_PPTOOLTIP_DISPLAY * pNotify = (NM_PPTOOLTIP_DISPLAY*)lpnmhdr;
		if (CComQIPtr<IRenderer> r = hotWindow)
		{
			NMNOTIFYCALCTOOLTIP msg = {0};
			msg.id = r->GetID();
			msg.supressTooltip = false;
			m_canvas->NotifyParent(SCW_NOTIFYCALCTOOLTIP, &msg);
			if (!msg.supressTooltip)
			{
				std::_tstring tooltip;
				if (CComQIPtr<IGraphEdge> item = GetGraphItem(r->GetID()))
				{
					CComQIPtr<CStringStringMap> attrs = item->GetPropertyUnknown(PROP_ATTRIBUTES);
					CreateTooltip(item, attrs, tooltip);
				}
				else if (CComQIPtr<IGraphVertex> item = GetGraphItem(r->GetID()))
				{
					CComQIPtr<CStringStringMap> attrs = item->GetPropertyUnknown(PROP_ATTRIBUTES);
					CreateTooltip(item, attrs, tooltip);
				}
				CPoint pt = *pNotify->pt;
				ScreenToClient(&pt);
				pNotify->ti->sTooltip = tooltip.c_str();
			}
		}
	} //if
	return 0;
}

LRESULT CGraphViewCanvas::OnNotifySelected(WPARAM /*wParam*/, LPNMHDR lpnmhdr, BOOL& /*bHandled*/)
{
	LPNMSELECTIONCHANGE lpnm=reinterpret_cast<LPNMSELECTIONCHANGE>(lpnmhdr);
	if (m_Owner)
	{
		if (lpnm->idSelect == m_graph->GetID()) // graph
		{
			m_Owner->OnSelected();
		}
		else
		{
			IGraphVertex* v=GetVertex(lpnm->idSelect);
			if(v)
			{
				m_Owner->OnVertexSelectionChanged(lpnm->idSelect);
			}
			else
			{
				IGraphSubgraph* sg=GetSubgraph(lpnm->idSelect);
				if(sg)
					m_Owner->OnSubgraphSelectionChanged(lpnm->idSelect);
			}
		}
	}
	// handled so return 1, else 0
	return 1;
}

LRESULT CGraphViewCanvas::OnNotifyItemsConnected(WPARAM wParam, LPNMHDR lpnmhdr, BOOL& bHandled)
{
	CCategory cat(guidDefault, 0);
	LPNMITEMCONN lpnm=reinterpret_cast<LPNMITEMCONN>(lpnmhdr);
    AddEdge(cat, lpnm->idfrom, lpnm->idto);
	return 1;
}

LRESULT CGraphViewCanvas::OnNotifyRecalcEdges(WPARAM wParam, LPNMHDR lpnmhdr, BOOL& bHandled)
{
	bHandled=TRUE;
	RectF previousExtent = m_canvas->getGraphExtent(false, world);
	m_layout_visgedge->pulse(previousExtent);
	return 0;
}

LRESULT CGraphViewCanvas::OnNotifyFocusChanged(WPARAM wParam, LPNMHDR lpnmhdr, BOOL& bHandled)
{
	LPNMFOCUSCHANGED lpnm=reinterpret_cast<LPNMFOCUSCHANGED>(lpnmhdr);
	if(lpnm->bGained)
		m_Owner->OnVertexGotFocus(lpnm->id);
	else
		m_Owner->OnVertexLostFocus(lpnm->id);
	return 1;
}

LRESULT CGraphViewCanvas::OnNotifyHotChanged(WPARAM wParam, LPNMHDR lpnmhdr, BOOL& bHandled)
{
	LPNMFOCUSCHANGED lpnm=reinterpret_cast<LPNMFOCUSCHANGED>(lpnmhdr);
	lpnm;
	return 1;
}

LRESULT CGraphViewCanvas::OnNotifyDoDragDrop(WPARAM wParam, LPNMHDR lpnmhdr, BOOL& bHandled)
{
	LPNMDODRAGDROP lpnm=reinterpret_cast<LPNMDODRAGDROP>(lpnmhdr);
	m_Owner->OnDoDragDrop(lpnm->dragStartPos);
	return 0;
}

LRESULT CGraphViewCanvas::OnNotifyScroll(WPARAM wParam, LPNMHDR lpnmhdr, BOOL& bHandled)
{
	LPNMSCROLL lpnm=reinterpret_cast<LPNMSCROLL>(lpnmhdr);

	if(m_canvas->GetApplicationVisMode()==RelavintTimeline)
	{
		m_timeline->ScrollTimeline(-(lpnm->pt.x));
		m_timeline->UpdateWindow(); // want immediate redraw
	}

	if(!m_meta.IsEmulateDecoration())
		Scroll(lpnm->pt);
	return 0;
}

LRESULT CGraphViewCanvas::OnNotifyMouseMessage(WPARAM wParam, LPNMHDR lpnmhdr, BOOL& bHandled)
{
	LPNMMOUSEMESSAGE lpnm=reinterpret_cast<LPNMMOUSEMESSAGE>(lpnmhdr);

	if(m_Owner && (	m_canvas->GetApplicationVisMode()	== RelavintStandard ||
					m_canvas->GetApplicationMode()		== AppModeQBGraph))
	{
		CPoint screenPos = lpnm->pt;
		ClientToScreen(&screenPos);

		IGraphVertex* v=GetVertex(lpnm->id);
		IGraphSubgraph* sg=GetSubgraph(lpnm->id);
		IGraphEdge* e=GetEdge(lpnm->id);

		switch(lpnm->uMsg)
		{
		case WM_LBUTTONDBLCLK:
			if(v)
				m_Owner->OnVertexDoubleClick(lpnm->id, screenPos);
			else if(sg)
				m_Owner->OnSubgraphDoubleClick(lpnm->id, screenPos);
			else if(e)
				m_Owner->OnEdgeDoubleClick(lpnm->id, screenPos);
			break;
		case WM_LBUTTONUP:
			if(v)
				m_Owner->OnVertexClick(lpnm->id, screenPos);
			else if(sg)
				m_Owner->OnSubgraphClick(lpnm->id, screenPos);
			else if(e)
				m_Owner->OnEdgeClick(lpnm->id, screenPos);
			break;
		default:
			break;
		}
	}

	return 0;
}

LRESULT CGraphViewCanvas::OnNotifyLayoutFinished(WPARAM wParam, LPNMHDR lpnmhdr, BOOL& bHandled)
{
	return 0;
}

bool CGraphViewCanvas::InitDragAndDrop()
{
	bool bSuccess;

	m_dropTarget = new CGraphViewDropTarget(m_hWnd);

	bSuccess=m_dropTarget!=0;

	if(bSuccess)
	{
		m_dropTarget->AddRef();
		bSuccess=SUCCEEDED(RegisterDragDrop(m_hWnd, m_dropTarget))!=0; 
	}

	if(bSuccess)
	{
		m_dropTarget->Release(); // RegisterDragDrop AddRefs
		FORMATETC ftetc={0};
		ftetc.cfFormat = CF_TEXT;
		ftetc.dwAspect = DVASPECT_CONTENT;
		ftetc.lindex = -1;
		ftetc.tymed = TYMED_HGLOBAL;
		m_dropTarget->AddSuportedFormat(ftetc);
	}
	else
	{
		if(m_dropTarget)
		{
			m_dropTarget->Release(); 
			m_dropTarget=0;
		}
	}

	return bSuccess;
}

bool CGraphViewCanvas::GetDragDropNotifications()
{
	return m_canvas->DragDropNotifications();
}

void CGraphViewCanvas::SetDragDropNotifications(bool value)
{
	m_canvas->DragDropNotifications(value);
}

IRenderer* CGraphViewCanvas::GetRenderer(const CUniqueID & id) const
{
	CComPtr<IGraphSubgraph> Subgraph = m_graph->GetSubgraph(id);
	if(Subgraph)
	{
		return m_canvas->GetRenderer(Subgraph);
	}
	else
	{
		CComPtr<IGraphVertex> v = m_graph->GetVertex(id);
		if(v)
		{
			return m_canvas->GetRenderer(v);
		}
		else
		{
			CComPtr<IGraphEdge> e = m_graph->GetEdge(id);
			if(e)
				return m_canvas->GetRenderer(e);
		}
	}
	return NULL;
}

void CGraphViewCanvas::SetPosition(const CUniqueID & id, const CPoint &pt, Coordinates co, bool randomize)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);

	CComPtr<IRenderer> ir = GetRenderer(id);
	if(ir)
	{
		PointF p(pt);
		if (randomize)
		{
			RandomizePosition(p);
		}
		ir->SetPosition(p, co);
	}
}

void CGraphViewCanvas::GetPosition(const CUniqueID & id, CPoint* result, GraphTypes::Coordinates co)
{
	PointF pt;
	CComPtr<IRenderer> ir = GetRenderer(id);
	if(ir)
		ir->GetPosition(pt, co);
	result->x = round_int(pt.x);
	result->y = round_int(pt.y);
}

void CGraphViewCanvas::GetLabel(const CUniqueID & id, std::_tstring & label)
{
	CComPtr<IGraphItem> item = GetGraphItem(id);
	if(item)
		label = item->GetLabel();
}

void CGraphViewCanvas::SetLabel(const CUniqueID & id, const TCHAR* label)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);

	if (label == NULL || _tcslen(label) == 0)
		return;

	CComPtr<IGraphItem> item = GetGraphItem(id);
	if(item)
	{
		item->SetLabel(label);
		InvalidateLayout();
	}
}

void CGraphViewCanvas::SetZOrder(const CUniqueID & id, int pos)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);

	CComQIPtr<IGraphVertex> item = GetGraphItem(id);
	if(item)
	{
		item->SetOrder(pos);
	}
}

void CGraphViewCanvas::SetRedraw(bool redraw)
{
	if (redraw)
	{
		if (!InterlockedDecrement(reinterpret_cast<long*>(&m_NoRedraw)))
		{
			assert(m_lock);
			m_lock.Release();
			InvalidateLayout();
		}
	}
	else
	{
		if (InterlockedIncrement(reinterpret_cast<long*>(&m_NoRedraw)) == 1)
		{
			assert(!m_lock);
			lock(m_lock);
		}
	}
}

void CGraphViewCanvas::EditGraphItem(const CUniqueID & id)
{
	CComPtr<IGraphItem> gi = m_graph->GetVertex(id);
	if(gi)
	{
		m_canvas->AttachEditor(gi);
	}
}

// Use HTTP "http:\\somedomain.com\somepath\image.bmp" for remote,
// paths like "res:\\c:\somewhere.exe; 120" for resources
// and "file:\\d:\somewhere\else.bmp" or ".ico" for local bmp and icon files.

IPicture* CGraphViewCanvas::CreateIPicFromPath(const TCHAR* ppath)
{
	std::_tstring path(ppath);
	IPicture* ipic=0;
	const std::_tstring	
		http=_T("http:\\\\"),
		httpu=_T("http://"),
		res=_T("res:\\\\"),
		file=_T("file:\\\\");

	bool IsHttp=path.find(http)==0||path.find(httpu)==0;
	bool IsRes=path.find(res)==0;
	bool IsFile=path.find(file)==0 || !(IsHttp||IsRes);

	if(IsFile)
	{
		unsigned extpos=path.length()-4;
		bool IsIcon=path.find(_T(".ico"),extpos)==extpos;

		HANDLE hnd=LoadImage(
			0,
			path.c_str(),
			IsIcon?IMAGE_ICON:IMAGE_BITMAP, 
			0,
			0,
			LR_LOADFROMFILE | LR_CREATEDIBSECTION);

		HRESULT hRes=E_FAIL;

		if(hnd)
		{
			PICTDESC desc;
			memset(&desc, 0, sizeof desc);
			desc.cbSizeofstruct=sizeof desc;
			if(IsIcon)
			{
				desc.picType=PICTYPE_ICON;
				desc.icon.hicon=static_cast<HICON>(hnd);
			}
			else
			{
				desc.picType=PICTYPE_BITMAP;
				desc.bmp.hbitmap=static_cast<HBITMAP>(hnd);
			}

			hRes=OleCreatePictureIndirect(
				&desc,
				IID_IPicture,
				TRUE,
				reinterpret_cast<void**>(&ipic));

			if(FAILED(hRes))
			{
				if(IsIcon)
					DestroyIcon(desc.icon.hicon);
				else
					DeleteObject(hnd);
			}
		}

	}
	else if(IsHttp)
	{
		IMoniker* monk;
		HRESULT hRes=CreateURLMoniker(
			0,
			CT2W(path.c_str()),
			&monk);

		if(SUCCEEDED(hRes))
		{
			IBindCtx* ibind;

			hRes=CreateBindCtx(0, &ibind);
			/*
			HRESULT CreateAsyncBindCtx(
				DWORD dwReserved,
				IBindStatusCallback * pbsc,
				IEnumFORMATETC * penumfmtetc,
				IBindCtx ** ppbc
			);
			*/

			if(SUCCEEDED(hRes))
			{
				IStream* istream;

				hRes=monk->BindToStorage(
					ibind,
					0,
					IID_IStream,
					reinterpret_cast<void**>(&istream));

				if(SUCCEEDED(hRes))
				{
					assert(istream!=0);
					hRes=OleLoadPicture(
						istream,
						0,
						TRUE,
						IID_IPicture,
						reinterpret_cast<void**>(&ipic));

					istream->Release();					
				}
				ibind->Release();
			}
			monk->Release();
		}
	}

	return ipic;
}

void CGraphViewCanvas::GetGraphLayout(GraphTypes::PointFVector & result) const
{
	PointF pt;
	CComPtr<IRenderer> ir;
	IGraphItemVector children;

	m_graph->GetAllChildren(children);

	assert(result.empty());

	IGraphItemVector::const_iterator ic=children.begin();
	while(ic!=children.end())
	{
		ir=m_canvas->GetRenderer(*ic);
		if(ir)
		{
			ir->GetPosition(pt);
			result.push_back(pt);
		}
		++ic;
	}
}

void CGraphViewCanvas::SetGraphLayout(const GraphTypes::PointFVector & result)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	CComPtr<IRenderer> ir;
	IGraphItemVector children;

	unsigned numc=m_graph->GetAllChildren(children);
	assert(numc==result.size());

	IGraphItemVector::const_iterator ic=children.begin();
	PointFVector::const_iterator ip=result.begin();
	while(ic!=children.end())
	{
		ir=m_canvas->GetRenderer(*ic);
		if(ir)
		{
			ir->SetPosition(*ip);
		}
		++ic;
		++ip;
	}
}

void CGraphViewCanvas::lock(CComPtr<clib::scoped_lock_ref_counted> & lock)
{
	lock = new clib::scoped_lock_ref_counted(m_mutex);
}

//  --  ISkip*  --
bool CGraphViewCanvas::SkipEdge(IGraphEdge * e) const
{
	if (m_buildingEdgeVisibility > 0)
		return false;

	return m_edgeFilter->SkipEdge(e);
}
bool CGraphViewCanvas::SkipVertex(IGraphVertex * v) const
{
	return false;
}
bool CGraphViewCanvas::SkipSubgraph(IGraphSubgraph * sg) const
{
	return false;
}
//  ---  IXGMMLRenderCallback  ---
GraphTypes::Color CGraphViewCanvas::GetColor(IGraphSubgraph * subgraph) const
{
	switch(m_visualState)
	{
	case WUVisualState_completed:
		return GraphTypes::Color::Blue;
	case WUVisualState_failed:
		return GraphTypes::Color::Red;
	default:
		{
			CComVariant val;
			subgraph->GetProperty(PROP_EDGE_STARTED, val);
			if (val.vt != VT_EMPTY && val.intVal > 0)
				return GraphTypes::Color::Black;
		}
		break;
	}
	return GraphTypes::Color::DarkGray;
}

void CGraphViewCanvas::GetColor(IGraphVertex * vertex, GraphTypes::Color &borderColor, GraphTypes::Color & fillColor, GraphTypes::Color & fontColor) const
{
	borderColor = m_meta.m_fgNormalQBColors[m_visualState];
	fillColor = m_meta.m_bgNormalQBColors[m_visualState];
	if (m_visualState == WUVisualState_running)
	{
		CComVariant val;
		vertex->GetProperty(PROP_EDGE_STARTED, val);
		if (val.vt != VT_EMPTY && val.intVal > 0)
			borderColor = GraphTypes::Color::Black;
	}
	if (IsHot(vertex->GetID()))
	{
		borderColor = m_meta.m_fgHotQBColors[m_visualState];
		fillColor = m_meta.m_bgHotQBColors[m_visualState];
	}
	else if (IsSelected(vertex->GetID()))
	{
		borderColor = m_meta.m_fgSelectedQBColors[m_visualState];
		fillColor = m_meta.m_bgSelectedQBColors[m_visualState];
	}
	fontColor = borderColor;
}

void CGraphViewCanvas::GetColor(IGraphEdge * edge, GraphTypes::Color & edgeColor, GraphTypes::Color & fontColor, GraphTypes::Color & firstControlPointColor, GraphTypes::Color & lastControlPointColor) const
{
	edgeColor = GraphTypes::Color::DarkGray;

	switch(m_visualState)
	{
	case WUVisualState_completed:
		edgeColor = GraphTypes::Color::Blue;
		break;
	case WUVisualState_failed:
		edgeColor = GraphTypes::Color::Red;
		break;
	default:
		{
			CComVariant val;
			edge->GetProperty(PROP_EDGE_STARTED, val);
			if (val.vt != VT_EMPTY && val.intVal > 0)
				edgeColor = GraphTypes::Color::Black;
		}
		break;
	}
	fontColor = edgeColor;
}
//  ===============================================================================================
#ifdef COMMS_LAYER
typedef Service::CServiceT<CSecureSoapSocketClient> CServiceSecure;

void CGraphViewCanvas::GetMeta(const TCHAR* url)
{
	SOAPCLIENT_ERROR error;
	CComInitialize com;
	CServiceSecure server;
	server.SetUrl(url);
	Service::VertexMetaInformation *vertexInformation;
	Service::EdgeMetaInformation *edgeInformation;
	int vertexCount, edgeCount;
	if (server.GetMeta(&vertexInformation, &vertexCount, &edgeInformation, &edgeCount) == S_OK)
	{
		for(int i = 0; i < vertexCount; ++i)
		{
			m_meta.SetMetaVertexLabel(CCategory(guidDefault, vertexInformation[i].Category), bstr_t(vertexInformation[i].Label));
			m_meta.SetMetaVertexColorBorderNorm(CCategory(guidDefault, vertexInformation[i].Category), vertexInformation[i].Normal.Border);
			m_meta.SetMetaVertexColorFillNorm(CCategory(guidDefault, vertexInformation[i].Category), vertexInformation[i].Normal.Fill);
			m_meta.SetMetaVertexColorBorderSel(CCategory(guidDefault, vertexInformation[i].Category), vertexInformation[i].Selected.Border);
			m_meta.SetMetaVertexColorFillSel(CCategory(guidDefault, vertexInformation[i].Category), vertexInformation[i].Selected.Fill);
			m_meta.SetMetaVertexColorBorderOver(CCategory(guidDefault, vertexInformation[i].Category), vertexInformation[i].Hot.Border);
			m_meta.SetMetaVertexColorFillOver(CCategory(guidDefault, vertexInformation[i].Category), vertexInformation[i].Hot.Fill);
		}

		for(int i = 0; i < edgeCount; ++i)
		{
			m_meta.SetMetaEdgeLabel(CCategory(guidDefault, edgeInformation[i].Category), bstr_t(edgeInformation[i].Label));
			m_meta.SetMetaEdgeColorNorm(CCategory(guidDefault, edgeInformation[i].Category), edgeInformation[i].Normal);
			m_meta.SetMetaEdgeColorShortest(CCategory(guidDefault, edgeInformation[i].Category), edgeInformation[i].Shortest);
			m_meta.SetMetaEdgeWidth(CCategory(guidDefault, edgeInformation[i].Category), edgeInformation[i].Width);
		}
		AtlCleanupValue(vertexInformation);
		AtlCleanupValue(edgeInformation);
	}
	else
	{
		error = server.GetClientError();
	}
}

void CGraphViewCanvas::GetData(const TCHAR* url, const CUniqueID& uid) 
{
	SOAPCLIENT_ERROR error;
	CComInitialize com;
	CServiceSecure server;
	server.SetUrl(url);
	Service::UniqueID id;
	id.Category = uid.GetCategory().GetCategory();
	id.ID = CT2W(uid.GetID(), CP_UTF8);
	Service::Vertex *vertices;
	Service::Edge *edges;
	int vertexCount, edgeCount;
	if (server.GetData(id, &vertices, &vertexCount, &edges, &edgeCount) == S_OK)
	{
		for(int i = 0; i < vertexCount; ++i)
		{
			CComPtr<IGraphVertex> v;
			bool created;
			boost::tie(v, created) = AddVertex(CCategory(guidDefault, vertices[i].ID.Category), CW2T(vertices[i].ID.ID, CP_UTF8));
#if _MSC_VER < 1400
			Service::Property *Property0 = vertices[i].Properties;
			int __Property0_nSizeIs = vertices[i].__Properties_nSizeIs;
#else
			Service::Property *Property0 = vertices[i].Properties.Property0;
			int __Property0_nSizeIs = vertices[i].Properties.__Property0_nSizeIs;
#endif
			for(int j = 0; j < __Property0_nSizeIs; ++j)
			{
				v->SetProperty(Property0[j].Key, Property0[j].Value);
			}
		}
		for(int i = 0; i < edgeCount; ++i)
		{
			CComPtr<IGraphEdge> e = AddEdge(CCategory(guidDefault, edges[i].ID.Category), CCategory(guidDefault, edges[i].From.Category), CW2T(edges[i].From.ID, CP_UTF8), CCategory(guidDefault, edges[i].To.Category), CW2T(edges[i].To.ID, CP_UTF8));
#if _MSC_VER < 1400
			Service::Property *Property0 = edges[i].Properties;
			int __Property0_nSizeIs = edges[i].__Properties_nSizeIs;
#else
			Service::Property *Property0 = edges[i].Properties.Property0;
			int __Property0_nSizeIs = edges[i].Properties.__Property0_nSizeIs;
#endif
			for(int j = 0; j < __Property0_nSizeIs; ++j)
			{
				e->SetProperty(Property0[j].Key, Property0[j].Value);
			}
		}
		AtlCleanupValue(vertices);
		AtlCleanupValue(edges);
	}
	else
	{
		error = server.GetClientError();
	}
}
#endif
//  ===============================================================================================
void LayoutThread(CGraphViewCanvas * self)
{
	clib::recursive_mutex::scoped_lock proc(self->m_pulsemutex);
	ATLTRACE("Starting layout\n");
	assert(!self->m_running);
	self->m_running = true;
	self->m_canvas->SetRunning(true);
	self->m_layout->init();
#ifdef SEISINT_LIBEXPORTS
	unsigned int throttle = 0;
#endif

	while(self->m_breakFlag == false)
	{
		{
			RectF previousExtent = self->m_canvas->getGraphExtent(false, world);
			clib::recursive_mutex::scoped_lock proc(self->m_mutex);
			LAYOUTEVENT doneFlag = self->m_layout->pulse(previousExtent);
			if (doneFlag == LAYOUTEVENT_FINISHED || doneFlag == LAYOUTEVENT_ABORTED || self->m_breakFlag)
			{
				break;
			}
		}

#ifdef SEISINT_LIBEXPORT
		if (throttle % 8 == 0)
		{
#endif
			(*self)(LAYOUTEVENT_ITERATION);
#ifdef SEISINT_LIBEXPORT
		}
		throttle++;
#endif
	}
	self->m_canvas->SetRunning(false);
	self->m_running = false;
	if (self->m_breakFlag)
	{
		self->m_breakFlag = false;
	}
	else
	{
		(*self)(LAYOUTEVENT_FINISHED);	//Only signal if we finished naturally.
	}
	ATLTRACE("Stopping layout\n");
}
//  ===============================================================================================
