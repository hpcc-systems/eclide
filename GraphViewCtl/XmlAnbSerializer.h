#pragma once

#include "AESCrypto.h"
#include "GraphViewCtlApi.h"
#include "SAXHandlersBase.h"

// forward
class CGraphViewCanvas;

class CXmlAnbDeserializer : public StackHandlerImpl, public CAESCrypto
{
private:
    CGraphViewCanvas* m_view;
    CCanvas * m_canvas;
    int m_version;
    bool m_itemAdded;
    bool m_propertyMetaInFile;
    CPoint m_relativeLoadPos;
    std::vector<CUniqueID> m_uidvector;
    CUniqueID m_currSubgraphId;
    CComPtr<IGraphItem> m_lastDeserializedItem;
public:
    CXmlAnbDeserializer(CGraphViewCanvas* view, CCanvas * canvas, const CPoint& relativeLoadPos, int version);
protected:
    virtual HRESULT STDMETHODCALLTYPE startElement(const wchar_t *pwchNamespaceUri, int cchNamespaceUri, const wchar_t *pwchLocalName, int cchLocalName, const wchar_t *pwchQName, int cchQName, ISAXAttributes *pAttributes);
    virtual HRESULT STDMETHODCALLTYPE endElement(const wchar_t *pwchNamespaceUri, int cchNamespaceUri, const wchar_t *pwchLocalName, int cchLocalName, const wchar_t *pwchQName, int cchQName);

    const CCategory CategoryFromString(const std::_tstring& strPluginWithCat, const TCHAR* strCatId, bstr_t& strCatOnly);

    IGraphVertex* AddVertex(const TCHAR* entityId, const TCHAR* label, bool forceVisible);
    void SetPosition(const CComPtr<IGraphItem>& item, const TCHAR* xStr, const TCHAR* yStr);
    void AddCard(const CComPtr<IGraphItem>& item, const CComVariant& name, const CComVariant& value);
    const ImageInfo& GetImageInfo(const TCHAR* position, const TCHAR* imgStr, ImageInfo& imginfo);
    void AddImage(const CComPtr<IGraphItem>& item, Element& e);
    void AddLink(const TCHAR* end1Id, const TCHAR* end2Id, const TCHAR* type, const TCHAR* label);

    IPicture* IPictureFromString(const std::string& img, CComPtr<IPicture>& retVal) const;
    IMAGE_POSITION ImagePositionFromString(const bstr_t& pos);

    CUniqueID GetVertexUniqueID(const bstr_t& bstrId);
    CUniqueID GetSubgraphUniqueID(const bstr_t& bstrId);
    void DeserializeCategory(const std::_tstring& catIn, CGuid& guid, bstr_t& strCat);
    const CCategory& DeserializeVertexCategory(const std::_tstring& catIn, CCategory& category);
    const CCategory& DeserializeEdgeCategory(const std::_tstring& catIn, CCategory& category);
    const CCategory& DeserializeSubgraphCategory(const std::_tstring& catIn, CCategory& category);
};

class CXmlAnbSerializer : public CAESCrypto
{
private:
    CXmlAnbSerializer();
    CGraphViewCanvas* m_view;
public:
    CXmlAnbSerializer(CGraphViewCanvas* view);
    ~CXmlAnbSerializer(void);

    // just a stub to the deserializer
    HRESULT Load(const TCHAR* xml, const CPoint& relativeLoadPos = CPoint());
    const TCHAR* Save(std::_tstring & retVal, SGV::XML_SAVE_FORMAT saveFormat, const CPoint& relativeSavePos = CPoint());
protected:
    CComPtr<IMXWriter> m_pWriter;
    CComPtr<IMXAttributes> m_pMXAttrs;
    CComQIPtr<ISAXAttributes> m_pSAXAttrs;
    CComQIPtr<ISAXContentHandler> m_pContentHandler;

    void SavePlugin(const CGuid& pluginId, const TCHAR* name);
    void SaveVertexEntityType(const CCategory& cat, const TCHAR* icon, bool saveExtraMeta);
    void SaveSubgraphEntityType(const CCategory& cat, const TCHAR* icon, bool saveExtraMeta);
    void SavePropertyTypes(const CCategory& cat);
    void SaveColor(const BSTR name, COLORREF fill, COLORREF font, COLORREF border);
    void SaveLinkType(const CCategory& cat, bool saveExtraMeta);
    void SaveEntity(const CUniqueID& iID, SGV::XML_SAVE_FORMAT saveFormat, const GraphTypes::PointF& location);
    void SaveImage(const CComQIPtr<IPicture>& img, IMAGE_POSITION pos, const CSize& size);
    void SaveCards(const CUniqueID& iID, bool isvertex);
    void SaveLink(const CComPtr<IGraphEdge>& e);

    void IPictureToString(const CComPtr<IPicture>& img, std::string& retVal);
    BSTR ImagePositionToString(IMAGE_POSITION pos);

    bool CategoryToString(const CCategory& category, const std::_tstring & catCode, std::_tstring & result);
    bool VertexCategoryToString(const CCategory& category, std::_tstring & result);
    bool EdgeCategoryToString(const CCategory& category, std::_tstring & result);
    bool SubgraphCategoryToString(const CCategory& category, std::_tstring & result);
    const TCHAR* VertexUniqueIdToString(const CUniqueID& vertex, std::_tstring & result);
    const TCHAR* SubgraphUniqueIdToString(const CUniqueID& vertex, std::_tstring & result);
};
