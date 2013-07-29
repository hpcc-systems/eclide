#include "StdAfx.h"
#include "..\en_us\resource.h"

#include "SourceDataView.h"
#include "DialogAddBreakpoint.h"

//  ===========================================================================
CSourceDataView::CSourceDataView() : m_eclView(this)
{
	m_owner = NULL;
	m_loaded = false;
}

CSourceDataView::~CSourceDataView()
{
}

void CSourceDataView::SetOwner(IGraphViewSlot * owner)
{
	if (m_owner != owner)
	{
		m_owner = owner;
		if (m_owner)
		{
		}
	}
}

BOOL CSourceDataView::PreTranslateMessage(MSG* pMsg)
{
	//if (m_tabbedChildWindow.PreTranslateMessage(pMsg))
	//	return TRUE;

	return CWnd::PreTranslateMessage(pMsg);
}

void CSourceDataView::CalcEdgesForBreakpoints(CUniqueIDVector & vertices, CUniqueIDVector & edges)
{
	const SGV::CGraphViewCtrl & view = m_owner->GetGlobalGraphView();

	vertices.Reset();
	while(vertices.MoveNext())
	{
		bool found = false;

		{
			CUniqueIDVector inEdges;
			view.GetInEdges(&inEdges, vertices.Get());
			while(inEdges.MoveNext())
			{
				found = true;
				edges.Add(inEdges.Get());
			}
		}

		if (found == false)
		{
			CUniqueIDVector outEdges;
			view.GetOutEdges(&outEdges, vertices.Get());
			while(outEdges.MoveNext())
				edges.Add(outEdges.Get());
		}
	}
}

BOOL CSourceDataView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	NMHDR *nmhdr = (NMHDR *)lParam;
	if (nmhdr->hwndFrom == m_eclView)
	{
		Scintilla::SCNotification * notification = (Scintilla::SCNotification *)lParam;
		switch (notification->nmhdr.code)
		{
		case SCN_MARGINCLICK: 
			if (notification->margin == 1) 
			{
				int row = m_eclView.LineFromPosition(notification->position) + 1;
				BreakPointPosMap::iterator found = m_breakpointPos.find(row);
				if (found != m_breakpointPos.end())
				{
					CMenu bar;
					bar.LoadMenu(IDR_POPUP_DEBUGSOURCEVIEW);
					CMenu * popup = bar.GetSubMenu(0);
					int itemID = 0;
					found->second.Reset();
					while (found->second.MoveNext())
					{

						WTL::CMenuItemInfo mi;
						mi.fMask = MIIM_ID | MIIM_TYPE;
						mi.fType = MFT_STRING;
						TCHAR buff[255];
						_tcscpy(buff, found->second.Get().GetID());
						mi.dwTypeData = buff;
						mi.wID = ID_DEBUGSOURCEVIEW_LOCATE + 100 + itemID;
						popup->InsertMenuItem(ID_LOCATE_NONE, &mi, FALSE);
						++itemID;
					}
					popup->RemoveMenu(ID_LOCATE_NONE, MF_BYCOMMAND);

					CPoint pt;
					GetCursorPos(&pt);
					int id = popup->TrackPopupMenu(TPM_RETURNCMD | TPM_NONOTIFY, pt.x, pt.y, AfxGetMainWnd());
					switch (id)
					{
					case ID_DEBUGSOURCEVIEW_SETBREAKPOINT:
						{
							CUniqueIDVector edges;
							CalcEdgesForBreakpoints(found->second, edges);
							edges.Reset();
							while(edges.MoveNext())
							{
								m_owner->GetDebugSession()->AddBreakpoint(edges.Get().GetID(), _T("Break"), _T("Always"), _T(""), _T(""), _T("Always"), _T(""), false);
							}
						}
						break;
					case ID_DEBUGSOURCEVIEW_ADDBREAKPOINT:
						{
							CDialogAddBreakpoint dlg;
							if (dlg.DoModal() == IDOK)
							{
								CUniqueIDVector edges;
								CalcEdgesForBreakpoints(found->second, edges);
								edges.Reset();
								while(edges.MoveNext())
								{
									m_owner->GetDebugSession()->AddBreakpoint(edges.Get().GetID(), dlg.m_stringMode.c_str(), dlg.m_stringConditionMode.c_str(), dlg.m_stringField.c_str(), dlg.m_stringCondition.c_str(), dlg.m_stringCountMode.c_str(), dlg.m_stringCount.c_str(), dlg.m_caseSensitive != 0);
								}
							}
						}
						break;
					default:
						{
							int itemID = id - (ID_DEBUGSOURCEVIEW_LOCATE + 100);
							if (itemID >= 0 && itemID < found->second.size())
								m_owner->NewSelection(NULL, found->second[itemID], true, false);
						}
						break;
					}
				}
			}
			break;
		}
	}
	return CWnd::OnNotify(wParam, lParam, pResult);
}

void CSourceDataView::SetText(const std::_tstring & text)
{
	m_eclView.SetReadOnly(false);
	m_eclView.SetText(text.c_str());
	m_eclView.SetReadOnly(true);
}

void CSourceDataView::SetBreakpointLocation(int row, const CUniqueID & id)
{
	m_breakpointPos[row].Add(id);
	m_eclView.MarkerAdd(row - 1, MARKER_CIRCLE);
}

void CSourceDataView::SelectLine(int row, int col)
{
	LoadText();
	m_eclView.SetSelLine(row - 1, col);
}

void CSourceDataView::ClearSelection()
{
	m_eclView.ClearSel();
}

void CSourceDataView::LoadText()
{
	ATLASSERT(false);
}

IMPLEMENT_DYNCREATE(CSourceDataView, CWnd)

BEGIN_MESSAGE_MAP(CSourceDataView, CWnd)
	ON_WM_CREATE()
	//ON_WM_ERASEBKGND()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
END_MESSAGE_MAP()


BOOL CSourceDataView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	//cs.dwExStyle |= WS_EX_CLIENTEDGE;
	//cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, ::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

//void CSourceDataView::SetWU(Dali::IWorkunit * wu)
//{
//	m_wu = wu;
//}
//
//bool CSourceDataView::WorkunitDeleted(Dali::IWorkunit * wu)
//{
//	m_wu = NULL;
//	return true;
//}
//
int CSourceDataView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_eclView.Create(GetSafeHwnd(), ATL::CWindow::rcDefault);
	m_eclView.DoInit();
	m_eclView.SetReadOnly(true);

	return 0;
}

//BOOL CSourceDataView::OnEraseBkgnd(CDC* dc)
//{
//	return 1;
//}

void CSourceDataView::OnSetFocus(CWnd* wnd)
{
	if (!m_loaded)
	{
		LoadText();
		for(BreakPointPosMap::const_iterator itr = m_breakpointPos.begin(); itr != m_breakpointPos.end(); itr++)
		{
			m_eclView.MarkerAdd(itr->first - 1, MARKER_CIRCLE);
		}
		m_loaded = true;
	}
	CWnd::OnSetFocus(wnd);
}

void CSourceDataView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	CWnd::OnSize(nType, cx, cy);

	CRect rectClient;
	GetClientRect(rectClient);
	m_eclView.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
}
//  ===========================================================================
void CBuilderDataView::SetSource(Dali::IWorkunit * wu)
{
	m_wu = wu;
}
void CBuilderDataView::LoadText()
{
	if (!m_eclView.GetLength())
		SetText(m_wu->GetEcl());
}
//  ===========================================================================
void CAttributeDataView::SetSource(const std::_tstring & module, const std::_tstring & attribute)
{
	m_module = module;
	m_attribute = attribute;
}
void CAttributeDataView::LoadText()
{
	if (!m_attr)
	{
		CComPtr<IRepository> rep = AttachRepository();
		m_attr = rep->GetAttribute(m_module.c_str(), m_attribute.c_str(), CreateIAttributeECLType(), 0, true, true);
	}
	if (!m_eclView.GetLength())
		SetText(m_attr->GetText(false));
}
//  ===========================================================================
void CLocalDataView::SetSource(const std::_tstring & filePath)
{
	m_filePath = filePath;
}
void CLocalDataView::LoadText()
{
	if (!m_eclView.GetLength())
	{
		CUnicodeFile file;
		if (file.Open(m_filePath))
		{
			std::_tstring ecl;
			file.Read(ecl);
			SetText(ecl);
		}
	}
}
//  ===========================================================================
