#pragma once
#include "resource.h"       // main symbols

#include "GraphViewContainer.h"
#include "GraphViewCtl.h"
#include "atlGraphView.h"

// CSubgraph

class ATL_NO_VTABLE CSubgraph : 
	public CComObjectRootEx<CComSingleThreadModel>,
//	public CComCoClass<CSubgraph, &CLSID_Subgraph>,
	public IDispatchImpl<ISubgraph>
{
public:
	CSubgraph() : m_subgraph(NULL)
	{
	}

	void Init(const CUniqueID& uniqueID, SGV::CGraphViewCtrl* pGraphCtrl)
	{
		m_pGraphCtrl = pGraphCtrl;
		// eventually removed when SGV::CGraphSubgraph has all we need
		m_uniqueID = uniqueID;
		m_subgraph = new SGV::CGraphSubgraph(pGraphCtrl->GetHWND(), uniqueID);
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_SUBGRAPH)

	BEGIN_COM_MAP(CSubgraph)
		COM_INTERFACE_ENTRY(ISubgraph)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	void FinalRelease() 
	{
		// ideally SGV::CGraphSubgraph / SGV::CGraphItem could become reference counted, then we can just use
		// CComPtr<SGV::CGraphSubgraph> ...
		if (m_subgraph)
			delete m_subgraph;
	}
protected:
	SGV::CGraphViewCtrl* m_pGraphCtrl;
	CUniqueID m_uniqueID;
	SGV::CGraphSubgraph* m_subgraph;
public:
	STDMETHOD(get_Category)(long* pVal)
	{
		*pVal = m_uniqueID.GetCategory().GetCategory();
		return S_OK;
	}

	STDMETHOD(get_Identity)(BSTR* pVal)
	{
		*pVal = ::SysAllocString(CT2W(m_uniqueID.GetID(), CP_UTF8));
		return S_OK;
	}

	STDMETHOD(get_Label)(BSTR* pVal)
	{
		*pVal = NULL;
		ATLASSERT(m_pGraphCtrl);
		std::_tstring label;
		CComVariant val = m_pGraphCtrl->GetLabel(m_uniqueID, label);
		*pVal = ::SysAllocString(val.vt == VT_BSTR ? val.bstrVal : L"");
		return S_OK;
	}

	STDMETHOD(put_Label)(BSTR newVal)
	{
		ATLASSERT(m_pGraphCtrl);
		m_pGraphCtrl->SetLabel(m_uniqueID, CW2T(newVal, CP_UTF8));
		return S_OK;
	}

	STDMETHOD(GetProperty)(long key, BSTR* pVal)
	{
		*pVal = NULL;
		ATLASSERT(m_pGraphCtrl);
		CComVariant val = m_pGraphCtrl->GetProperty(m_uniqueID, key);
		*pVal = ::SysAllocString(val.vt == VT_BSTR ? val.bstrVal : L"");
		return S_OK;
	}

	STDMETHOD(SetProperty)(long key, BSTR newVal)
	{
		ATLASSERT(m_pGraphCtrl);
		m_pGraphCtrl->SetProperty(m_uniqueID, key, newVal);
		return S_OK;
	}

	STDMETHOD(SetImage)(IHTMLElementRender* element, ImagePosition pos, long width, long height, long marginX, long marginY, long overlay)
	{
		CComPtr<GraphTypes::IBitmap> ibm=GetIBitmapFromIHTMLElement(element);
		m_subgraph->SetImage(ibm, (IMAGE_POSITION)pos, marginX, marginY, (unsigned int)overlay);
		return S_OK;
	}

	STDMETHOD(get_Visible)(BOOL* pVal)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(put_Visible)(BOOL visible)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(GetImageVisible)(ImagePosition position, BOOL* pVal)
	{
		*pVal = (BOOL)m_subgraph->GetImageVisible((IMAGE_POSITION)position);
		return S_OK;
	}

	STDMETHOD(SetImageVisible)(ImagePosition position, BOOL visible)
	{
		m_subgraph->SetImageVisible((IMAGE_POSITION)position, visible == TRUE);
		return S_OK;
	}

	STDMETHOD(get_Minimize)(/* [retval][out] */ BOOL *pVal) 
	{
		*pVal = m_subgraph->GetMinimized();
		return S_OK;
	}

	STDMETHOD(put_Minimize)(/* [in] */ BOOL newVal) 
	{
		m_subgraph->SetMinimized(newVal == TRUE);
		return S_OK;
	}

	//  Relavint Compatability  
	STDMETHOD(SetHtmlElement)(IHTMLElementRender* element, ImagePosition pos, long width, long height, long marginX, long marginY, long overlay) 
	{
		return SetImage(element, pos, width, height, marginX, marginY, overlay);
	}
};

//OBJECT_ENTRY_AUTO(__uuidof(Subgraph), CSubgraph)
