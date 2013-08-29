#include "stdafx.h"
#include "..\en_us\resource.h"

#include "DockableDataViews.h"
#include "Repository.h"
#include "UtilFilesystem.h"

//  ===========================================================================
CDockableDataViews::CDockableDataViews() //: m_dlgview(NULL, NULL)
{
	m_owner = NULL;
}

CDockableDataViews::~CDockableDataViews()
{
}

void CDockableDataViews::SetOwner(IGraphViewSlot * owner)
{
	if (m_owner != owner)
	{
		m_owner = owner;
		CloseAllTabs();
		if (m_owner)
		{
			PrepTabEcl(m_owner->GetGlobalGraphView(), m_owner->GetWorkunit());
		}
	}
}

void CDockableDataViews::ActivateTab(const CWnd * wnd)
{
	int tabID = m_wndTab.GetTabFromHwnd(wnd->GetSafeHwnd());
	if (tabID >= 0)
		m_wndTab.SetActiveTab(tabID);
}

void CDockableDataViews::SetTabData(const ITableMap * data, const roxie::CSearchRowVector & searchResults)
{
	for(ITableMap::const_iterator itr = data->begin(); itr != data->end(); ++itr)
	{
		if (!boost::algorithm::equals(itr->first, _T("_")))
			SetTabData(itr->first, itr->second.m_table, false, searchResults);
	}
}

void CDockableDataViews::SetTabData(const std::_tstring & label, const ITable * data, bool hasRowNum, const roxie::CSearchRowVector & searchResults)
{
	StringResultMap::iterator itr = m_dataTabs.find(label);
	if (itr == m_dataTabs.end())
	{
		m_dataTabs[label] = new CDataView();
		m_dataTabs[label]->Create(NULL, label.c_str(), WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), &m_wndTab, 1);
		m_dataTabs[label]->SetFont(&afxGlobalData.fontRegular);
		m_wndTab.InsertTab(m_dataTabs[label], label.c_str(), 0, 0, FALSE);
	}
	m_dataTabs[label]->SetData(data, hasRowNum, searchResults);
	ActivateTab(m_dataTabs[label]);
}

void CDockableDataViews::PrepTabEcl(const SGV::CGraphViewCtrl & graphView, Dali::IWorkunit * wu)
{
	m_wndTab.SetRedraw(false);
	SetTabEcl(wu, -1, CUniqueID());	//Allways want builder first so we can delay load the attributes.
	CUniqueIDVector vertices;
	graphView.GetVertices(&vertices);
	while(vertices.MoveNext())
	{
		CString def = graphView.GetProperty(vertices.Get(), PROP_VERTEX_DEFINITION);
		ParsedDefinition pd;
		if (DefinitionToLocation(static_cast<const TCHAR *>(def), pd))
		{
			switch (pd.GetType())
			{
			case ParsedDefinition::BUILDER:
				SetTabEcl(wu, pd.row, vertices.Get());
				break;
			case ParsedDefinition::REMOTE:
				SetTabEcl(pd.module, pd.attribute, pd.row, vertices.Get());
				break;
			case ParsedDefinition::LOCAL:
				SetTabEcl(pd.module + pd.attribute, pd.row, vertices.Get());
				break;
			default:
				ATLASSERT(false);
			}
		}
	}
	m_wndTab.SetRedraw(true);
}

void CDockableDataViews::SetTabEcl(Dali::IWorkunit * wu, int row, const CUniqueID & id)
{
	std::_tstring label = _T("Builder");
	StringSourceMap::iterator itr = m_sourceTabs.find(label);
	if (itr == m_sourceTabs.end())
	{
		StlLinked<CBuilderDataView> builderDataView = new CBuilderDataView();
		m_sourceTabs[label] = builderDataView.get();
		builderDataView->Create(NULL, label.c_str(), WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), &m_wndTab, 1);
		builderDataView->SetFont(&afxGlobalData.fontRegular);
		m_wndTab.InsertTab(builderDataView, label.c_str(), 0, 0, FALSE);
		builderDataView->SetOwner(m_owner);
		builderDataView->SetSource(wu);
		builderDataView->LoadText();
	}
	if (row >= 0)
		m_sourceTabs[label]->SetBreakpointLocation(row, id);
}

void CDockableDataViews::SetTabEcl(const std::_tstring & moduleLabel, const std::_tstring & attributeLabel, int row, const CUniqueID & id)
{
	std::_tstring label = moduleLabel + _T(".") + attributeLabel;
	StringSourceMap::iterator itr = m_sourceTabs.find(label);
	if (itr == m_sourceTabs.end())
	{
		StlLinked<CAttributeDataView> attributeDataView = new CAttributeDataView();
		m_sourceTabs[label] = attributeDataView.get();
		attributeDataView->Create(NULL, label.c_str(), WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), &m_wndTab, 1);
		attributeDataView->SetFont(&afxGlobalData.fontRegular);
		m_wndTab.AddTab(attributeDataView, label.c_str(), 0, FALSE);
		attributeDataView->SetOwner(m_owner);
		attributeDataView->SetSource(moduleLabel, attributeLabel);
	}
	m_sourceTabs[label]->SetBreakpointLocation(row, id);
}

void CDockableDataViews::SetTabEcl(const std::_tstring & localFile, int row, const CUniqueID & id)
{
	std::_tstring label = localFile;
	StringSourceMap::iterator itr = m_sourceTabs.find(localFile);
	if (itr == m_sourceTabs.end())
	{
		StlLinked<CLocalDataView> attributeDataView = new CLocalDataView();
		m_sourceTabs[localFile] = attributeDataView.get();
		attributeDataView->Create(NULL, localFile.c_str(), WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), &m_wndTab, 1);
		attributeDataView->SetFont(&afxGlobalData.fontRegular);
		boost::filesystem::path p = stringToPath(localFile);
		m_wndTab.AddTab(attributeDataView, pathToWString(p.filename()).c_str(), 0, FALSE);
		attributeDataView->SetOwner(m_owner);
		attributeDataView->SetSource(localFile);
	}
	m_sourceTabs[localFile]->SetBreakpointLocation(row, id);
}

void CDockableDataViews::ClearTabData()
{
	for (StringResultMap::iterator itr = m_dataTabs.begin(); itr != m_dataTabs.end(); ++itr)
	{
		itr->second->SetData(NULL, false, roxie::CSearchRowVector());
	}
	SetTabSelection(_T("Data"), roxie::CSearchRowVector());
}

void CDockableDataViews::SetTabSelection(const std::_tstring & label, const roxie::CSearchRowVector & searchResults)
{
	StringResultMap::iterator itr = m_dataTabs.find(label);
	if (itr != m_dataTabs.end())
	{
		itr->second->SetTabSelection(searchResults);
		ActivateTab(itr->second);
	}
}

void CDockableDataViews::SetTabSelection(const std::_tstring & label, int row, int col)
{
	StringSourceMap::const_iterator itr = m_sourceTabs.find(label);
	if (itr != m_sourceTabs.end())
	{
		itr->second->SelectLine(row, col);
		ActivateTab(itr->second);
	}
}

void CDockableDataViews::ClearTabSelection()
{
	for(StringSourceMap::const_iterator itr = m_sourceTabs.begin(); itr != m_sourceTabs.end(); ++itr)
	{
		itr->second->ClearSelection();
	}
}

void CDockableDataViews::CloseAllTabs()
{
	m_wndTab.RemoveAllTabs();
	m_dataTabs.clear();
	m_sourceTabs.clear();
}

void CDockableDataViews::Send_Clear()
{
	m_wndTab.RemoveAllTabs();
	m_dataTabs.clear();
	m_sourceTabs.clear();
}
void CDockableDataViews::Post_Reset()
{
	Send_Clear();
}
BOOL CDockableDataViews::PreTranslateMessage(MSG* pMsg)
{
	//if (m_tabbedChildWindow.PreTranslateMessage(pMsg))
	//	return TRUE;

	return CWnd::PreTranslateMessage(pMsg);
}

IMPLEMENT_DYNCREATE(CDockableDataViews, CDockablePane)

BEGIN_MESSAGE_MAP(CDockableDataViews, CDockablePane)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
END_MESSAGE_MAP()

BOOL CDockableDataViews::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, ::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

int CDockableDataViews::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_wndTab.Create(CMFCTabCtrl::STYLE_3D_ONENOTE, ATL::CWindow::rcDefault, this, 1, CMFCTabCtrl::LOCATION_BOTTOM);
	m_wndTab.EnableTabDocumentsMenu();


	return 0;
}

BOOL CDockableDataViews::OnEraseBkgnd(CDC* dc)
{
	//if (m_dlgview && m_dlgview->GetTabCount())
	return 0;
	return CWnd::OnEraseBkgnd(dc);
}

void CDockableDataViews::OnSize(UINT nType, int cx, int cy)
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
