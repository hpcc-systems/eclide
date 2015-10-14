// Vertex.h : Declaration of the CVertex

#pragma once
#include "resource.h"       // main symbols
#include "GraphViewContainer.h"
#include "GraphViewCtl.h"
#include "atlGraphView.h"
#include "olectl.h"

// CVertex

class ATL_NO_VTABLE CVertex : 
    public CComObjectRootEx<CComSingleThreadModel>,
//	public CComCoClass<CVertex, &CLSID_Vertex>,
    public IDispatchImpl<IVertex>
{
    typedef CComObjectRootEx<CComSingleThreadModel> objBase;
public:
    CVertex() : m_vertex(NULL)
    {
    }

    // eventually SGV::CGraphViewCtrl* pGraphCtrl >> becomes : HWND hWndCtrl
    void Init(const CUniqueID& uniqueID, SGV::CGraphViewCtrl* pGraphCtrl)
    {
        m_pGraphCtrl = pGraphCtrl;
        // eventually removed when SGV::CGraphVertex has all we need
        m_uniqueID = uniqueID;
        m_vertex = new SGV::CGraphVertex(pGraphCtrl->GetHWND(), uniqueID);
    }

    DECLARE_REGISTRY_RESOURCEID(IDR_VERTEX)

    BEGIN_COM_MAP(CVertex)
        COM_INTERFACE_ENTRY(IVertex)
        COM_INTERFACE_ENTRY(IDispatch)
    END_COM_MAP()

    DECLARE_PROTECT_FINAL_CONSTRUCT()

    HRESULT FinalConstruct()
    {
        return S_OK;
    }
    void FinalRelease() 
    {
        // ideally SGV::CGraphVertex / SGV::CGraphItem could become reference counted, then we can just use
        // CComPtr<SGV::CGraphVertex> ...
        if (m_vertex)
            delete m_vertex;
    }

#ifdef _DEBUG
    ULONG InternalAddRef()
    {
        return objBase::InternalAddRef();
    }

    ULONG InternalRelease()
    {
        return objBase::InternalRelease();
    }

#endif

protected:
    SGV::CGraphViewCtrl* m_pGraphCtrl;
    CUniqueID m_uniqueID; // eventually removed when SGV::CGraphVertex has all we need
    SGV::CGraphVertex* m_vertex;
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

    STDMETHOD(SetImage)(IHTMLElementRender* element, ImagePosition pos, long width, long height, long marginX, long marginY, long overlay) 
    {
        CComPtr<GraphTypes::IBitmap> ibmp=GetIBitmapFromIHTMLElement(element);
        m_vertex->SetImage(ibmp, (IMAGE_POSITION)pos, marginX, marginY, (unsigned int)overlay);
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
        if (key == 0)
        {
            std::_tstring label;
            m_pGraphCtrl->GetLabel(m_uniqueID, label);
            if (label.length() == 0)
                m_pGraphCtrl->SetLabel(m_uniqueID, CW2T(newVal, CP_UTF8));
        }
        m_pGraphCtrl->SetProperty(m_uniqueID, key, newVal);
        return S_OK;
    }

    STDMETHOD(GetVisibleLinkedVerticesCountByEdgeCat)(long category, long *pVal)
    {
        ATLASSERT(m_pGraphCtrl != NULL);
        *pVal = m_pGraphCtrl->GetVertexNeighbourCount(
            m_uniqueID,
            CCategory(guidDefault,category),
            SGV::SGV_TARGETSTRUCT::SGV_TARGET_STYLE_EDGE,SGV::SGV_TARGETSTRUCT::SGV_TARGET_STATE_VISIBLE);
        return S_OK;
    }

    STDMETHOD(GetHiddenLinkedVerticesCountByEdgeCat)(long category, long *pVal)
    {
        ATLASSERT(m_pGraphCtrl != NULL);
        *pVal = m_pGraphCtrl->GetVertexNeighbourCount(
            m_uniqueID,
            CCategory(guidDefault,category),
            SGV::SGV_TARGETSTRUCT::SGV_TARGET_STYLE_EDGE,SGV::SGV_TARGETSTRUCT::SGV_TARGET_STATE_HIDDEN);
        return S_OK;
    }

    STDMETHOD(GetVisibleLinkedVerticesCountByVertexCat)(long category, long *pVal)
    {
        ATLASSERT(m_pGraphCtrl != NULL);
        *pVal = m_pGraphCtrl->GetVertexNeighbourCount(
            m_uniqueID,
            CCategory(guidDefault,category),
            SGV::SGV_TARGETSTRUCT::SGV_TARGET_STYLE_VERTEX,SGV::SGV_TARGETSTRUCT::SGV_TARGET_STATE_VISIBLE);
        return S_OK;
    }

    STDMETHOD(GetHiddenLinkedVerticesCountByVertexCat)(long category, long *pVal)
    {
        ATLASSERT(m_pGraphCtrl != NULL);
        *pVal = m_pGraphCtrl->GetVertexNeighbourCount(
            m_uniqueID,
            CCategory(guidDefault,category),
            SGV::SGV_TARGETSTRUCT::SGV_TARGET_STYLE_VERTEX,SGV::SGV_TARGETSTRUCT::SGV_TARGET_STATE_HIDDEN);
        return S_OK;
    }

    STDMETHOD(SetLinkedVertexVisibilityByEdgeCat)(long category, BOOL visible)
    {
        ATLASSERT(m_pGraphCtrl != NULL);
        m_pGraphCtrl->SetVertexNeighboursVisibleByEdgeCat(
            m_uniqueID,
            CCategory(guidDefault,category),
            visible != FALSE);
        return S_OK;
    }

    STDMETHOD(SetLinkedVertexVisibilityByVertexCat)(long category, BOOL visible)
    {
        ATLASSERT(m_pGraphCtrl != NULL);
        m_pGraphCtrl->SetVertexNeighboursVisibleByVertexCat(
            m_uniqueID,
            CCategory(guidDefault,category),
            visible != FALSE);
        return S_OK;
    }

    STDMETHOD(get_Visible)(BOOL* pVal)
    {
        *pVal = (BOOL)m_vertex->GetVisible();
        return S_OK;
    }

    STDMETHOD(put_Visible)(BOOL visible)
    {
        m_vertex->SetVisible(visible == TRUE);
        return S_OK;
    }

    STDMETHOD(GetImageVisible)(ImagePosition position, BOOL* pVal)
    {
        ATLASSERT(m_pGraphCtrl != NULL);
        *pVal = (BOOL)m_vertex->GetImageVisible((IMAGE_POSITION)position);
        return S_OK;
    }

    STDMETHOD(SetImageVisible)(ImagePosition position, BOOL visible)
    {
        ATLASSERT(m_pGraphCtrl != NULL);
        m_vertex->SetImageVisible((IMAGE_POSITION)position, visible == TRUE);
        return S_OK;
    }

    STDMETHOD(get_X)(long* pVal)
    {
        ATLASSERT(m_pGraphCtrl);
        CPoint pt;
        m_pGraphCtrl->GetPosition(m_uniqueID, pt);
        *pVal = pt.x;
        return S_OK;
    }

    STDMETHOD(get_Y)(long* pVal)
    {
        ATLASSERT(m_pGraphCtrl);
        CPoint pt;
        m_pGraphCtrl->GetPosition(m_uniqueID, pt);
        *pVal = pt.y;
        return S_OK;
    }

    STDMETHOD(get_NormalBorderColor)(ULONG *color)
    {
        // TODO: will be phased out
        CComPtr<IGraphVertex> v = m_pGraphCtrl->GetVertex(m_uniqueID);
        if (v)
            *color = v->GetBorderColor().ToCOLORREF();  // TODO: now it's unsigned, should this be GetValue()? (ie. inc. Alpha)
        return S_OK;
    }
        
    STDMETHOD(put_NormalBorderColor)(ULONG color)
    {
        // TODO: will be phased out
        CComPtr<IGraphVertex> v = m_pGraphCtrl->GetVertex(m_uniqueID);
        if (v)
            v->SetBorderColor(color);
        return S_OK;
    }

    STDMETHOD(get_NormalFillColor)(ULONG *color)
    {
        // TODO: will be phased out
        CComPtr<IGraphVertex> v = m_pGraphCtrl->GetVertex(m_uniqueID);
        if (v)
            *color = v->GetFillColor().ToCOLORREF();  // TODO: now it's unsigned, should this be GetValue()? (ie. inc. Alpha)
        return S_OK;
    }
        
    STDMETHOD(put_NormalFillColor)(ULONG color)
    {
        // TODO: will be phased out
        CComPtr<IGraphVertex> v = m_pGraphCtrl->GetVertex(m_uniqueID);
        if (v)
            v->SetFillColor(color);
        return S_OK;
    }

    //  Relavint Compatability  
    STDMETHOD(SetHtmlElement)(IHTMLElementRender* element, ImagePosition pos, long width, long height, long marginX, long marginY, long overlay) 
    {
        //IPicture* ipic=GetIPictureFromIHTMLElement(element, width, height);
        return SetImage(element, pos, width, height, marginX, marginY, overlay);
    }
};

//OBJECT_ENTRY_AUTO(__uuidof(Vertex), CVertex)
