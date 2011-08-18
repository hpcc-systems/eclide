// GraphViewControl.cpp : Implementation of CGraphViewControl
#include "stdafx.h"
#include ".\graphviewcontrol.h"

using namespace VerticesCollection;

//*****************************************************************************

void IVertexCopy::init(IVertex** p)
{
	*p=0;
}

void IVertexCopy::destroy(IVertex** p)
{
	if(*p)
	{
		(*p)->Release();
		*p=0;
	}
}

HRESULT IVertexCopy::copy(IVertex** pTo, const IVertexAdapt* pFrom)
{
	ATLASSERT(*pTo && pFrom);
	*pTo = pFrom->get();
	if (*pTo)
		(*pTo)->AddRef();
	return S_OK;
}

//*****************************************************************************

void IVertexCopy2Variant::init(VARIANT* p) 
{
	p->vt = VT_EMPTY;
}

void IVertexCopy2Variant::destroy(VARIANT* p) 
{
	if(p->vt==VT_DISPATCH && p->pdispVal!=0)
		p->pdispVal->Release();
	VariantClear(p);
}

HRESULT IVertexCopy2Variant::copy(VARIANT* dest, const IVertexAdapt* src)
{
		ATLASSERT(dest && src);
		dest->vt = VT_DISPATCH;
		dest->pdispVal = src->get();
		if(dest->pdispVal)
			dest->pdispVal->AddRef();
		return S_OK;
}

//*****************************************************************************

HRESULT CGraphViewControl::GetSelectedVertices(IVertices** collection)
{
	CComObject<CVertices>* pVertices;
	HRESULT hRes=CComObject<CVertices>::CreateInstance(&pVertices);
	if(SUCCEEDED(hRes))
	{
		pVertices->AddRef();
		*collection=pVertices;

		CUniqueIDVector results;
		m_view.GetVertices(&results, TRISTATE_BOTH, TRISTATE_BOTH, TRISTATE_TRUE);

		while (results.MoveNext())
		{
			IVertex* pv=CreateVertex(results.Get());
			if(pv)
				pVertices->AddVertex(pv);
		}

		return S_OK;
	}
	return E_FAIL;
}

HRESULT CGraphViewControl::FindVertices(BSTR label, /* [defaultvalue][in] */ BOOL searchLabel, /* [defaultvalue][in] */ BOOL searchProperties, IVertices** collection)
{
	CComObject<CVertices>* pVertices;
	HRESULT hRes=CComObject<CVertices>::CreateInstance(&pVertices);
	if(SUCCEEDED(hRes))
	{
		pVertices->AddRef();
		*collection=pVertices;

		CUniqueIDVector results;
		std::_tstring _label=CW2T(label);
		m_view.Find(&results, _label.c_str(), searchLabel != FALSE, searchProperties != FALSE);

		while (results.MoveNext())
		{
			IVertex* pv=CreateVertex(results.Get());
			if(pv)
				pVertices->AddVertex(pv);
		}

		return S_OK;
	}
	return E_FAIL;
}

//*****************************************************************************

void CVertices::AddVertex(IVertex* v)
{
	IVertexAdapt var(v);
	m_coll.push_back(var);
	// let collection control the lifetime
	v->Release();
}

