#include "stdafx.h"
#include "..\en_us\resource.h"

#include "DockablePropertyGridViews.h"
#include "Repository.h"

//  ===========================================================================
CDockablePropertyGridViews::CDockablePropertyGridViews() //: m_dlgview(NULL, NULL)
{
	m_owner = NULL;
}

CDockablePropertyGridViews::~CDockablePropertyGridViews()
{
}

void CDockablePropertyGridViews::SetOwner(IGraphViewSlot * owner)
{
	if (m_owner != owner)
	{
		m_owner = owner;
		CloseAllTabs();
		if (m_owner)
		{
			PrepTabPropertyGrid(&m_owner->GetGlobalGraphView(), &m_owner->GetActiveGraphView());
		}
	}
}

void CDockablePropertyGridViews::ActivateTab(const CWnd * wnd)
{
	int tabID = m_wndTab.GetTabFromHwnd(wnd->GetSafeHwnd());
	if (tabID >= 0)
		m_wndTab.SetActiveTab(tabID);
}

typedef std::map<CUniqueID, CStringStringMap> CUniqueIDCStringStringMap;
typedef std::map<int, COLUMN_TYPE_ENUM> ColTypeMap;
void LoadDataIntoTable(const CUniqueID & id, CStringStringMap * properties, ColTypeMap & colTypeMap, ITable * data)
{
	__int64 row = data->GetRowCount();
	unsigned int col = -1;
	switch (id.GetCategory().GetCategory())
	{
	case XGMML_CAT_VERTEX:
		col = data->FindColumn(_T("Vertex ID"), true, CT_INT);
		break;
	case XGMML_CAT_EDGE:
		col = data->FindColumn(_T("Edge ID"), true, CT_INT);
		break;
	}
	data->SetCell(row, col, id.GetID());
	for(Element::StringStringMap::const_iterator itr = properties->m_attr.begin(); itr != properties->m_attr.end(); ++itr)
	{
#ifndef _DEBUG
		if (!boost::algorithm::starts_with(itr->first, _T("_")))
#endif
		{
			col = data->FindColumn(itr->first);
			std::_tstring val = itr->second;
			boost::algorithm::trim(val);
			if (col == -1)
			{
				col = data->FindColumn(itr->first, true, CT_INT);
				colTypeMap[col] = CT_INT;
			}
			if (colTypeMap[col] == CT_INT)
			{
				if (itr->second.empty())
				{
					//  Do nothing for empty.
				}
				else
				{
					for(std::size_t i = 0; i < val.length(); ++i)
					{
						if (!_istdigit(val[0]))
						{
							colTypeMap[col] = CT_STRING;
							data->SetColumn(col, itr->first, CT_STRING);
							break;
						}
					}
				}
			}
			data->SetCell(row, col, val);
		}
	}
}

void LoadPropertyMap(const CUniqueID & id, CComQIPtr<CStringStringMap> propertyMap, CUniqueIDCStringStringMap & results)
{
	if (!propertyMap)
		return;

	results[id].MergeFrom(*propertyMap);
}

void LoadProperties(const SGV::CGraphViewCtrl * graph, CUniqueIDCStringStringMap & results)
{
	if (!graph)
		return;

	CUniqueIDVector vertices;
	graph->GetVertices(&vertices);
	while(vertices.MoveNext())
	{
		LoadPropertyMap(vertices.Get(), graph->GetPropertyUnknown(vertices.Get(), PROP_ATTRIBUTES), results);
	}

	CUniqueIDVector edges;
	graph->GetEdges(&edges);
	while(edges.MoveNext())
	{
		LoadPropertyMap(edges.Get(), graph->GetPropertyUnknown(edges.Get(), PROP_ATTRIBUTES), results);
	}
}

void CDockablePropertyGridViews::PrepTabPropertyGrid(const SGV::CGraphViewCtrl * globalGraphView, const SGV::CGraphViewCtrl * activeGraphView)
{
	ColTypeMap colTypeMap;
	CUniqueIDCStringStringMap properties;
	CComPtr<ITable> vertexData = CreateISparseTable();
	CComPtr<ITable> edgeData = CreateISparseTable();

	LoadProperties(globalGraphView, properties);
	LoadProperties(activeGraphView, properties);

	for(CUniqueIDCStringStringMap::iterator itr = properties.begin(); itr != properties.end(); ++itr)
	{
		switch (itr->first.GetCategory().GetCategory())
		{
		case XGMML_CAT_VERTEX:
			LoadDataIntoTable(itr->first, &itr->second, colTypeMap, vertexData);
			break;
		case XGMML_CAT_EDGE:
			LoadDataIntoTable(itr->first, &itr->second, colTypeMap, edgeData);
			break;
		}
	}

	SetTabPropertyGrid(_T("Vertices"), vertexData);
	SetTabPropertyGrid(_T("Edges"), edgeData);
}

void CDockablePropertyGridViews::SetTabPropertyGrid(const std::_tstring & label, const ITable* data)
{
	StringPropertyGridMap::const_iterator itr = m_propertyGridTabs.find(label);
	if (itr == m_propertyGridTabs.end())
	{
		bool edges = boost::algorithm::iequals(label, _T("edges"));
		StlLinked<CPropertyGridView> propertyGridView = new CPropertyGridView(edges ? XGMML_CAT_EDGE : XGMML_CAT_VERTEX);
		m_propertyGridTabs[label] = propertyGridView.get();
		propertyGridView->Create(NULL, label.c_str(), WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), &m_wndTab, 1);
		propertyGridView->SetFont(&afxGlobalData.fontRegular);
		m_wndTab.AddTab(propertyGridView, label.c_str(), 0, FALSE);
		propertyGridView->SetOwner(m_owner);
	}
	m_propertyGridTabs[label]->SetData(data);
}

void CDockablePropertyGridViews::ClearTabSetTabPropertyGrid()
{
}

void CDockablePropertyGridViews::CloseAllTabs()
{
	m_wndTab.RemoveAllTabs();
	m_propertyGridTabs.clear();
}

void CDockablePropertyGridViews::Send_Clear()
{
	m_wndTab.RemoveAllTabs();
	m_propertyGridTabs.clear();
}
void CDockablePropertyGridViews::Post_Reset()
{
	Send_Clear();
}
BOOL CDockablePropertyGridViews::PreTranslateMessage(MSG* pMsg)
{
	//if (m_tabbedChildWindow.PreTranslateMessage(pMsg))
	//	return TRUE;

	return CWnd::PreTranslateMessage(pMsg);
}

IMPLEMENT_DYNCREATE(CDockablePropertyGridViews, CDockablePane)

BEGIN_MESSAGE_MAP(CDockablePropertyGridViews, CDockablePane)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
END_MESSAGE_MAP()


BOOL CDockablePropertyGridViews::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, ::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

int CDockablePropertyGridViews::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_wndTab.Create(CMFCTabCtrl::STYLE_3D_ONENOTE, ATL::CWindow::rcDefault, this, 1, CMFCTabCtrl::LOCATION_BOTTOM);
	m_wndTab.EnableTabDocumentsMenu();


	return 0;
}

BOOL CDockablePropertyGridViews::OnEraseBkgnd(CDC* dc)
{
	//if (m_dlgview && m_dlgview->GetTabCount())
	return 0;
	return CWnd::OnEraseBkgnd(dc);
}

void CDockablePropertyGridViews::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	//if (m_dlgview)
	//{
		CRect rectClient;
		GetClientRect(rectClient);
		m_wndTab.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
	//}
}
//  ===========================================================================
