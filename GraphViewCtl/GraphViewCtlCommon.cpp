#include "StdAfx.h"
#include "meta.h"
#include ".\graphviewctlcommon.h"
#include ".\vertex.h"
#include ".\edge.h"
#include "DSeisintGraphControlEvents_CP.H"

STDMETHODIMP GraphViewCtlCommon::SetDefaultVertexIcon(long Category, long hBitmap)
{
	//CIconHandle icon;
	//icon.LoadIcon(_U_STRINGorID(IDI_ICON1), 16, 16);
	//m_view->SetMetaVertexIcon(Category, icon);
	return S_OK;
}

STDMETHODIMP GraphViewCtlCommon::SetDefaultVertexIconFile(long Category, BSTR FileName)
{
	CIconHandle icon;
	icon.LoadIcon((LPCTSTR)bstr_t(FileName), 16, 16, LR_LOADFROMFILE);
	m_view->SetMetaVertexIcon(Category, icon);
	return S_OK;
}

STDMETHODIMP GraphViewCtlCommon::SetDefaultVertexLabel(long Category, BSTR Label)
{
	if (m_view != NULL)
		m_view->SetMetaVertexLabel(Category, CString(Label));
	return S_OK;
}

STDMETHODIMP GraphViewCtlCommon::SetDefaultVertexColors(long Category, long NormalFill, long BorderFill, long SelectedFill, long SelectedBorder, long OverFill, long OverBorder)
{
	if (m_view != NULL)
		m_view->SetMetaVertexColor(Category, NormalFill, BorderFill, SelectedFill, SelectedBorder, OverFill, OverBorder);
	return S_OK;
}

STDMETHODIMP GraphViewCtlCommon::SetDefaultEdgeColors(long Category, long Normal, long ShortestPath)
{
	if (m_view != NULL)
		m_view->SetMetaEdgeColorNorm(Category, Normal, ShortestPath);
	return S_OK;
}

STDMETHODIMP GraphViewCtlCommon::SetDefaultEdgeLabel(long Category, BSTR Label)
{
	if (m_view != NULL)
		m_view->SetMetaEdgeLabel(Category, CString(Label));
	return S_OK;
}

STDMETHODIMP GraphViewCtlCommon::AddVertex(long Category, BSTR Identity, IVertex** Vertex)
{
	if (m_view == NULL)
		return E_FAIL;
	CComObject<CVertex>* pVertex;
	CComObject<CVertex>::CreateInstance(&pVertex);
	m_view->AddVertex((const int) Category, (TCHAR*)bstr_t(Identity));
	pVertex->QueryInterface (Vertex); // implicit AddRef
	pVertex->SetGraphView(m_view);
	(*Vertex)->put_Category(Category);
	(*Vertex)->put_Identity(Identity);
	return S_OK;
}

STDMETHODIMP GraphViewCtlCommon::GetVertex(long Category, BSTR Identity, IVertex** Vertex)
{
	if (m_view == NULL)
		return E_FAIL;
	if (-1 == m_view->GetVertex((const int) Category, (TCHAR*)bstr_t(Identity))) {
		*Vertex = NULL;
		return S_FALSE;
	}
	CComObject<CVertex>* pVertex;
	CComObject<CVertex>::CreateInstance(&pVertex);
	pVertex->QueryInterface (Vertex); // implicit AddRef
	pVertex->SetGraphView(m_view);
	(*Vertex)->put_Category(Category);
	(*Vertex)->put_Identity(Identity);
	return S_OK;
}

STDMETHODIMP GraphViewCtlCommon::AddEdge(long Category, IVertex* From, IVertex* To, IEdge** Edge)
{
	if (m_view == NULL)
		return E_FAIL;
	GraphViewCtlLib::IVertexPtr from = From;
	GraphViewCtlLib::IVertexPtr to = To;
	CComObject<CEdge>* pEdge;
	CComObject<CEdge>::CreateInstance(&pEdge);
	m_view->AddEdge(Category, from->Category, bstr_t(from->Identity), to->Category, bstr_t(to->Identity));
	pEdge->SetGraphView(m_view);
	pEdge->QueryInterface (Edge); // implicit AddRef
	(*Edge)->put_From(From);
	(*Edge)->put_To(To);
	(*Edge)->put_Category(Category);
	return S_OK;
}

STDMETHODIMP GraphViewCtlCommon::GetEdge(long Category, IVertex* From, IVertex* To, IEdge** Edge)
{
	if (m_view == NULL)
		return E_FAIL;
	GraphViewCtlLib::IVertexPtr from = From;
	GraphViewCtlLib::IVertexPtr to = To;
	
	if (edge_descriptor(-1, -1, 0) == m_view->GetEdge(Category, from->Category, bstr_t(from->Identity), to->Category, bstr_t(to->Identity))) {
		*Edge = NULL;
		return S_FALSE;
	}
	CComObject<CEdge>* pEdge;
	CComObject<CEdge>::CreateInstance(&pEdge);
	pEdge->SetGraphView(m_view);
	pEdge->QueryInterface (Edge); // implicit AddRef
	(*Edge)->put_From(From);
	(*Edge)->put_To(To);
	(*Edge)->put_Category(Category);
	return S_OK;
}

STDMETHODIMP GraphViewCtlCommon::Clear()
{
	if (m_view != NULL)
		m_view->Clear();
	return S_OK;
}

STDMETHODIMP GraphViewCtlCommon::GenerateRandomData()
{
	if (m_view == NULL)
		return E_FAIL;
#ifdef _DEBUG
	m_view->AddRandom(60, 60);
#else
	m_view->AddRandom(600, 600);
#endif
	return S_OK;
}

STDMETHODIMP GraphViewCtlCommon::get_Layout(LayoutType* pVal)
{
	*pVal = m_currentLayoutType;
	return S_OK;
}

STDMETHODIMP GraphViewCtlCommon::put_Layout(LayoutType newVal)
{
	m_currentLayoutType = newVal;
	if (m_view != NULL)
		m_view->SetLayout((enum LAYOUT)m_currentLayoutType);
	return S_OK;
}

STDMETHODIMP GraphViewCtlCommon::Recenter()
{
	if (m_view != NULL)
		m_view->CenterGraph();
	return S_OK;
}

STDMETHODIMP GraphViewCtlCommon::StopLayout()
{
	if (m_view != NULL)
		m_view->SetLayout(LAYOUT_NULL);
	return S_OK;
}

STDMETHODIMP GraphViewCtlCommon::get_Zoom(long* pVal)
{
	*pVal = 100;
	if (m_view != NULL)
		*pVal = (long)(100 * m_view->GetScale());
	return S_OK;
}

STDMETHODIMP GraphViewCtlCommon::put_Zoom(long newVal)
{
	if (m_view != NULL)
		m_view->SetScale(newVal / 100.0f);
	return S_OK;
}

// IGraphViewControlProperties
STDMETHODIMP GraphViewCtlCommon::GetVertexProperty(long Category, BSTR Identity, long key, BSTR* pVal)
{
	_ASSERTE(m_view != NULL);
	*pVal = bstr_t(m_view->GetProperty(Category, bstr_t(Identity), key)).copy();
	return S_OK;
}

STDMETHODIMP GraphViewCtlCommon::SetVertexProperty(long Category, BSTR Identity, long key, BSTR newVal)
{
	_ASSERTE(m_view != NULL);
	m_view->SetProperty(Category, bstr_t(Identity), key, bstr_t(newVal));
	return S_OK;
}

STDMETHODIMP GraphViewCtlCommon::GetEdgeProperty(long Category, long FromCategory, BSTR FromIdentity, long ToCategory, BSTR ToIdentity, long key, BSTR* pVal)
{
	_ASSERTE(m_view != NULL);
	*pVal = bstr_t(m_view->GetProperty(Category, FromCategory, bstr_t(FromIdentity), ToCategory, bstr_t(ToIdentity), key)).copy();
	return S_OK;
}

STDMETHODIMP GraphViewCtlCommon::SetEdgeProperty(long Category, long FromCategory, BSTR FromIdentity, long ToCategory, BSTR ToIdentity, long key, BSTR newVal)
{
	_ASSERTE(m_view != NULL);
	m_view->SetProperty(Category, FromCategory, bstr_t(FromIdentity), ToCategory, bstr_t(ToIdentity), key, bstr_t(newVal));
	return S_OK;
}
