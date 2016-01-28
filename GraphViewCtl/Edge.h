// Edge.h : Declaration of the CEdge

#pragma once
#include "resource.h"       // main symbols
#include "GraphViewContainer.h"
#include "GraphViewCtl.h"
#include "atlGraphView.h"

// CEdge

class ATL_NO_VTABLE CEdge : 
    public CComObjectRootEx<CComSingleThreadModel>,
//	public CComCoClass<CEdge, &CLSID_Edge>,
    public IDispatchImpl<IEdge>
{
public:
    CEdge() : m_edge(NULL)
    {
    }

    void Init(const CUniqueID& uniqueID, SGV::CGraphViewCtrl* pGraphCtrl)
    {
        m_pGraphCtrl = pGraphCtrl;
        // eventually removed when SGV::CGraphEdge has all we need
        m_uniqueID = uniqueID;
        m_edge = new SGV::CGraphEdge(pGraphCtrl->GetHWND(), uniqueID);
    }

    DECLARE_REGISTRY_RESOURCEID(IDR_EDGE)

    BEGIN_COM_MAP(CEdge)
        COM_INTERFACE_ENTRY(IEdge)
        COM_INTERFACE_ENTRY(IDispatch)
    END_COM_MAP()

    DECLARE_PROTECT_FINAL_CONSTRUCT()

    HRESULT FinalConstruct()
    {
        return S_OK;
    }
    void FinalRelease() 
    {
        // ideally SGV::CGraphEdge / SGV::CGraphItem could become reference counted, then we can just use
        // CComPtr<SGV::CGraphEdge> ...
        if (m_edge)
            delete m_edge;
    }
protected:
    SGV::CGraphViewCtrl* m_pGraphCtrl;
    CUniqueID m_uniqueID;
    SGV::CGraphEdge* m_edge;
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
        m_edge->SetImage(ibm, (IMAGE_POSITION)pos, marginX, marginY, (unsigned int)overlay);
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
        *pVal = (BOOL)m_edge->GetImageVisible((IMAGE_POSITION)position);
        return S_OK;
    }

    STDMETHOD(SetImageVisible)(ImagePosition position, BOOL visible)
    {
        m_edge->SetImageVisible((IMAGE_POSITION)position, visible == TRUE);
        return S_OK;
    }

    STDMETHOD(get_Color)(/* [retval][out] */ OLE_COLOR *pVal) 
    {
        GraphTypes::Color clr;
        m_edge->GetColor(clr);
        *pVal = clr.ToCOLORREF(); // TODO: now it's unsigned, should this be GetValue()? (ie. inc. Alpha)
        return S_OK;
    }

    STDMETHOD(put_Color)(/* [in] */ OLE_COLOR newVal) 
    {
        GraphTypes::Color clr;
        clr.SetFromCOLORREF(newVal);
        m_edge->SetColor(clr);
        return S_OK;
    }

    //  Relavint Compatability  
    STDMETHOD(SetHtmlElement)(IHTMLElementRender* element, ImagePosition pos, long width, long height, long marginX, long marginY, long overlay) 
    {
        return SetImage(element, pos, width, height, marginX, marginY, overlay);
    }
};

//OBJECT_ENTRY_AUTO(__uuidof(Edge), CEdge)
