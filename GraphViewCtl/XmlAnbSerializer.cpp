#include "StdAfx.h"

#include "GraphViewCanvas.h"
#include "graphcontainer.h"
#include "IPictureHelper.h"
#include "xmlanbserializer.h"

using namespace boost::tuples;

CXmlAnbDeserializer::CXmlAnbDeserializer(CGraphViewCanvas* view, CCanvas * canvas, const CPoint& relativeLoadPos, int version) : m_view(view), m_canvas(canvas), m_itemAdded(false), m_relativeLoadPos(relativeLoadPos), m_propertyMetaInFile(false), m_version(version)
{
}

HRESULT STDMETHODCALLTYPE CXmlAnbDeserializer::startElement(const wchar_t *pwchNamespaceUri, int cchNamespaceUri, const wchar_t *pwchLocalName, int cchLocalName, const wchar_t *pwchQName, int cchQName, ISAXAttributes *pAttributes)
{
    int finger = m_stack.size();
    if(finger==0)
        return __super::startElement(pwchNamespaceUri, cchNamespaceUri, pwchLocalName, cchLocalName, pwchQName, cchQName, pAttributes);

    Element& eicon = const_cast<Element &>(m_stack._Get_container()[--finger]);

    if (eicon.m_tag.compare(_T("Icon")) == 0)
    {
        Element& e = const_cast<Element &>(m_stack._Get_container()[--finger]);
        if (e.m_tag.compare(_T("Entity")) != 0)
            return __super::startElement(pwchNamespaceUri, cchNamespaceUri, pwchLocalName, cchLocalName, pwchQName, cchQName, pAttributes);

        Element& end = const_cast<Element &>(m_stack._Get_container()[--finger]);
        Element& chartItem = const_cast<Element &>(m_stack._Get_container()[--finger]);
        m_lastDeserializedItem=0; // last chart item deserialized is not relevant at this point

        bool forceVisible = e.m_attr[_T("Visible")].length() > 0;
        bool insubgraph=e.m_attr[_T("ParentId")].length()>0; // in subgraph but not in root subgraph
        if(insubgraph)
        {
            if(m_uidvector.empty())
                m_currSubgraphId=GetSubgraphUniqueID(bstr_t(e.m_attr[_T("ParentId")].c_str()));

            m_lastDeserializedItem = AddVertex(e.m_attr[_T("EntityId")].c_str(), chartItem.m_attr[_T("Label")].c_str(), forceVisible);
            ATLASSERT(m_lastDeserializedItem!=0);
            if (m_lastDeserializedItem!=0)
            {
                SetPosition(m_lastDeserializedItem, end.m_attr[_T("X")].c_str(), end.m_attr[_T("Y")].c_str());
                m_uidvector.push_back(m_lastDeserializedItem->GetID());
            }
        }
        else
        {
            if(m_uidvector.empty()) // vertex in root subgraph
            {
                m_lastDeserializedItem = AddVertex(e.m_attr[_T("EntityId")].c_str(), chartItem.m_attr[_T("Label")].c_str(), forceVisible);
                ATLASSERT(m_lastDeserializedItem!=0);
                if (m_lastDeserializedItem!=0)
                    SetPosition(m_lastDeserializedItem, end.m_attr[_T("X")].c_str(), end.m_attr[_T("Y")].c_str());
            }
            else // this should be subgraph with merged vertices IDs in m_uidvector
            {
                CUniqueID sgid=GetSubgraphUniqueID(bstr_t(e.m_attr[_T("EntityId")].c_str()));
                ATLASSERT(sgid==m_currSubgraphId);
                if(sgid==m_currSubgraphId)
                {
                    CUniqueIDVector subgraphitems;
                    std::vector<CUniqueID>::const_iterator itr=m_uidvector.begin();
                    while(itr!=m_uidvector.end())
                    {
                        subgraphitems.Add(*itr);
                        ++itr;
                    }
                    CUniqueID newsgid;
                    m_view->MergeIntoSubgraph(newsgid, &subgraphitems, chartItem.m_attr[_T("Label")].c_str());
                    ATLASSERT(newsgid==m_currSubgraphId);
                    m_lastDeserializedItem=m_view->GetSubgraph(newsgid);
                    ATLASSERT(m_lastDeserializedItem!=0);
                    if(m_lastDeserializedItem!=0)
                        SetPosition(m_lastDeserializedItem, end.m_attr[_T("X")].c_str(), end.m_attr[_T("Y")].c_str());
                }
                m_uidvector.clear();
            }
        }
    }
    return __super::startElement(pwchNamespaceUri, cchNamespaceUri, pwchLocalName, cchLocalName, pwchQName, cchQName, pAttributes);
}

HRESULT STDMETHODCALLTYPE CXmlAnbDeserializer::endElement(const wchar_t *pwchNamespaceUri, int cchNamespaceUri, const wchar_t *pwchLocalName, int cchLocalName, const wchar_t *pwchQName, int cchQName) 
{
    Element& e = m_stack.top();
    int finger = m_stack.size();

    finger--; finger--; // skip unwanted

    if (e.m_tag.compare(_T("ViewSettings")) == 0)
    {
        //if (e.m_attr[_T("Zoom")].length())
        //{
        //	int zoom = boost::lexical_cast<ULONG>(e.m_attr[_T("Zoom")].c_str());
        //	m_view->SetScale((float)zoom/100.0f);
        //}
        //if (e.m_attr[_T("Spacing")].length())
        //{
        //	int spacing = boost::lexical_cast<ULONG>(e.m_attr[_T("Spacing")].c_str());
        //	m_view->SetSpacing((float)spacing/100.0f);
        //}
    }
    if (e.m_tag.compare(_T("Plugin")) == 0)
    {
        CGuid pluginId;
        pluginId = CT2A(e.m_attr[_T("Id")].c_str());
        m_view->GetMeta().SetMetaPluginName(pluginId.GetGUID(), e.m_attr[_T("Name")].c_str());
    }
    else if (e.m_tag.compare(_T("Card")) == 0)
    {
        if(m_lastDeserializedItem)
            AddCard(m_lastDeserializedItem, e.m_attr[_T("SourceReference")].c_str(), e.m_attr[_T("Summary")].c_str());
    }
    else if (e.m_tag.compare(_T("Color")) == 0)
    {
        finger--; // skip unwanted
        Element& entity = const_cast<Element &>(m_stack._Get_container()[finger]);
        Element& metaTag = const_cast<Element &>(m_stack._Get_container()[finger+1]);

        bstr_t strCat;
        const CCategory& cat = CategoryFromString(entity.m_attr[_T("Name")], metaTag.m_attr[_T("Id")].c_str(), strCat);

        COLORREF fill = 0, border = 0, font = 0;

        try
        {
            if (e.m_attr[_T("Fill")].length())
                fill = boost::lexical_cast<ULONG>(e.m_attr[_T("Fill")].c_str());
            if (e.m_attr[_T("Border")].length())
                border = boost::lexical_cast<ULONG>(e.m_attr[_T("Border")].c_str());
            if (e.m_attr[_T("Font")].length())
                font = boost::lexical_cast<ULONG>(e.m_attr[_T("Font")].c_str());
        }
        catch (const boost::bad_lexical_cast &) {}

        bool subgraph=entity.m_attr[_T("GraphItemType")]==_T("Subgraph");

        if(subgraph)
        {
            if (e.m_attr[_T("Name")].compare(_T("Normal")) == 0)
            {
                m_view->GetMeta().SetMetaSubgraphColorBorderNorm(cat, border);
                m_view->GetMeta().SetMetaSubgraphColorFillNorm(cat, fill);
                m_view->GetMeta().SetMetaSubgraphFontColor(cat, font);
            }
            else if (e.m_attr[_T("Name")].compare(_T("Selected")) == 0)
            {
                m_view->GetMeta().SetMetaSubgraphColorBorderSel(cat, border);
                m_view->GetMeta().SetMetaSubgraphColorFillSel(cat, fill);
                m_view->GetMeta().SetMetaSubgraphFontColorSel(cat, font);
            }
            else if (e.m_attr[_T("Name")].compare(_T("Hover")) == 0)
            {
                m_view->GetMeta().SetMetaSubgraphColorBorderOver(cat, border);
                m_view->GetMeta().SetMetaSubgraphColorFillOver(cat, fill);
            }
        }
        else
        {
            if (e.m_attr[_T("Name")].compare(_T("Normal")) == 0)
            {
                m_view->GetMeta().SetMetaVertexColorBorderNorm(cat, border);
                m_view->GetMeta().SetMetaVertexColorFillNorm(cat, fill);
                m_view->GetMeta().SetMetaVertexFontColor(cat, font);
            }
            else if (e.m_attr[_T("Name")].compare(_T("Selected")) == 0)
            {
                m_view->GetMeta().SetMetaVertexColorBorderSel(cat, border);
                m_view->GetMeta().SetMetaVertexColorFillSel(cat, fill);
                m_view->GetMeta().SetMetaVertexFontColorSel(cat, font);
            }
            else if (e.m_attr[_T("Name")].compare(_T("Hover")) == 0)
            {
                m_view->GetMeta().SetMetaVertexColorBorderOver(cat, border);
                m_view->GetMeta().SetMetaVertexColorFillOver(cat, fill);
            }
        }
    }
    else if (e.m_tag.compare(_T("PropertyType")) == 0)
    {
        finger--; finger--; // skip unwanted
        Element& entity = const_cast<Element &>(m_stack._Get_container()[finger]);
        Element& metaTag = const_cast<Element &>(m_stack._Get_container()[finger+1]);

        bstr_t strCat;
        const CCategory& cat = CategoryFromString(entity.m_attr[_T("Name")], metaTag.m_attr[_T("Id")].c_str(), strCat);

        variant_t propId(e.m_attr[_T("Id")].c_str());
        variant_t varAttr = e.m_attr[_T("Attr")].c_str();
        PROPERTY_ATTR attr = (PROPERTY_ATTR)(int)varAttr;

        CComVariant value = e.m_attr[_T("Value")].c_str();

        if (attr == PA_SERIALIZE || attr == PA_READ_ONLY || attr == PA_HIDDEN || attr == PA_READ_ONLY_NAME)
            value.ChangeType(VT_BOOL);

        bool subgraph=entity.m_attr[_T("GraphItemType")]==_T("Subgraph");
        if(subgraph)
            m_view->GetMeta().SetPropertyAttribute(META_SUBGRAPH, cat, propId, attr, value);
        else
            m_view->GetMeta().SetPropertyAttribute(META_VERTEX, cat, propId, attr, value);
        // flag that we found property meta
        if (!m_propertyMetaInFile)
            m_propertyMetaInFile = true;
    }
    else if (e.m_tag.compare(_T("Image")) == 0)
    {
        finger--; // skip unwanted
        Element& entity = const_cast<Element &>(m_stack._Get_container()[finger]);

        if (entity.m_tag.compare(_T("EntityType")) == 0)
        {
            Element& metaTag = const_cast<Element &>(m_stack._Get_container()[finger+1]);
            ImageInfo imginfo;
            GetImageInfo(e.m_attr[_T("Position")].c_str(), e.GetContent(), imginfo);
            if (imginfo.image != 0)
            {
                bool subgraph=entity.m_attr[_T("GraphItemType")]==_T("Subgraph");
                bstr_t strCat;
                const CCategory& cat = CategoryFromString(entity.m_attr[_T("Name")], metaTag.m_attr[_T("Id")].c_str(), strCat);

                if(subgraph)
                {
                    m_view->GetMeta().SetMetaSubgraphXmlCatCode(cat, strCat);
                    m_view->GetMeta().SetMetaSubgraphXmlAnbIconFile(cat, entity.m_attr[_T("IconFile")].c_str());
                }
                else
                {
                    m_view->GetMeta().SetMetaVertexXmlCatCode(cat, strCat);
                    m_view->GetMeta().SetMetaVertexXmlAnbIconFile(cat, entity.m_attr[_T("IconFile")].c_str());
                }

                if (metaTag.m_attr[_T("Label")].length())
                {
                    if(subgraph)
                        m_view->GetMeta().SetMetaSubgraphLabel(cat, metaTag.m_attr[_T("Label")].c_str());
                    else
                        m_view->GetMeta().SetMetaVertexLabel(cat, metaTag.m_attr[_T("Label")].c_str());
                }

                if (metaTag.m_attr[_T("Deselected")].length() > 0)
                {
                    // only set it if unset
                    if(subgraph)
                    {
                        if (m_view->GetMeta().GetMetaSubgraphHide(cat) == VISIBLE_DEFAULT)
                            m_view->GetMeta().SetMetaSubgraphHide(cat, true);
                    }
                    else
                    {
                        if (m_view->GetMeta().GetMetaVertexHide(cat) == VISIBLE_DEFAULT)
                            m_view->GetMeta().SetMetaVertexHide(cat, true);
                    }
                }

                if(subgraph)
                {
                    if (m_view->GetSubgraphImage(CUniqueID(cat, _T("")), imginfo.pos, 0, false) == NULL)
                        m_view->SetMetaSubgraphImage(cat, imginfo, 0); //tweak
                }
                else
                {
                    if (m_view->GetVertexImage(CUniqueID(cat, _T("")), imginfo.pos, 0, false) == NULL)
                        m_view->SetMetaVertexImage(cat, imginfo, 0); //tweak
                }
            }
        }
        else if (entity.m_tag.compare(_T("Entity")) == 0)
        {
            if (_tcslen(e.GetContent())>0)
            {
                ATLASSERT(m_lastDeserializedItem!=0);
                if(m_lastDeserializedItem!=0)
                    AddImage(m_lastDeserializedItem, e);
            }
        }
    }
    else if (e.m_tag.compare(_T("LinkStyle")) == 0)
    {
        Element& link = const_cast<Element &>(m_stack._Get_container()[finger--]);
        Element& chartItem = const_cast<Element &>(m_stack._Get_container()[finger]);
        AddLink(link.m_attr[_T("End1Id")].c_str(), link.m_attr[_T("End2Id")].c_str(), e.m_attr[_T("Type")].c_str(), chartItem.m_attr[_T("Label")].c_str());
    }
    else if (e.m_tag.compare(_T("Meta")) == 0)
    {
        Element& linkType = const_cast<Element &>(m_stack._Get_container()[finger]);
        if (linkType.m_tag.compare(_T("LinkType")) == 0)
        {
            bstr_t strCat;
            const CCategory& cat = CategoryFromString(linkType.m_attr[_T("Name")], e.m_attr[_T("Id")].c_str(), strCat);
            ULONG bgr = 0;
            try
            {
                bgr = boost::lexical_cast<int>(linkType.m_attr[_T("Colour")].c_str());
            }
            catch (const boost::bad_lexical_cast &) { ATLASSERT(!"Only bad file editing would cause this!"); }

            if (bgr != 0)
            {
                // ANB stores in annoying BGR format >> convert to (A)RGB
                GraphTypes::Color edgeColor(0xFF, LOBYTE(LOWORD(bgr)), HIBYTE(LOWORD(bgr)), LOBYTE(HIWORD(bgr)));
                //COLORREF storedColor = m_view->GetMeta().GetMetaEdgeColorNorm(cat);
                m_view->GetMeta().SetMetaEdgeColorNorm(cat, edgeColor.GetValue());
            }
            m_view->GetMeta().SetMetaEdgeXmlData(cat, strCat);
            if (e.m_attr[_T("Label")].length())
                m_view->GetMeta().SetMetaEdgeLabel(cat, e.m_attr[_T("Label")].c_str());
        }
    }
    return __super::endElement(pwchNamespaceUri, cchNamespaceUri, pwchLocalName, cchLocalName, pwchQName, cchQName);
}

const CCategory CXmlAnbDeserializer::CategoryFromString(const std::_tstring& strPluginWithCat, const TCHAR* strCatId, bstr_t& strCatOnly)
{
    CGuid plugin;
    int category = 0;
    try
    {
        category = boost::lexical_cast<int>(strCatId);
    }
    catch (const boost::bad_lexical_cast &) { ATLASSERT(!"Only bad file editing would cause this!"); }
    DeserializeCategory(strPluginWithCat, plugin, strCatOnly);
    return CCategory(plugin.GetGUID(), category);
}

IGraphVertex* CXmlAnbDeserializer::AddVertex(const TCHAR* entityId, const TCHAR* label, bool forceVisible)
{
    CComPtr<IGraphVertex> vertex;

    CUniqueID id = GetVertexUniqueID(bstr_t(entityId));
    ATLASSERT(!id.isEmpty());

    if (!id.isEmpty())
        vertex = m_view->AddVertex(id, GraphTypes::PointF(), label).first;
    if (vertex != NULL && forceVisible)
        m_view->SetVertexVisible(vertex->GetID(), forceVisible);
    return vertex;
}

void CXmlAnbDeserializer::SetPosition(const CComPtr<IGraphItem>& item, const TCHAR* xStr, const TCHAR* yStr)
{
    int x = 0, y = 0;
    if (_tcslen(xStr) && _tcslen(yStr))
    {
        try
        {
            x = boost::lexical_cast<int>(xStr);
            y = boost::lexical_cast<int>(yStr);
        }
        catch(const boost::bad_lexical_cast &) {}
    }

    // get offset
    GraphTypes::PointF offset(m_relativeLoadPos);
    m_view->GetCanvas()->RAllTransform(offset);

    // offset value by origin and scroll offset
    GraphTypes::PointF pt(static_cast<float>(x) + offset.x, static_cast<float>(y) + offset.y);

    CComPtr<IRenderer> ir = m_canvas->GetRenderer(item);
    ir->SetPosition(pt); // defaults to world
}

void CXmlAnbDeserializer::AddCard(const CComPtr<IGraphItem>& item, const CComVariant& name, const CComVariant& value)
{
    // web files (old) did not store property meta, and properties are way off so we ignore them.
    if (m_version == 1 && !m_propertyMetaInFile)
        return;

    ATLASSERT(name.vt == VT_BSTR);
    if (name.vt != VT_BSTR)
        return; // shouldn't ever happen

    CComQIPtr<IGraphVertex> vertex=item;
    CComQIPtr<IGraphSubgraph> sg=item;
    ATLASSERT((vertex || sg) && !(vertex && sg));

    int propId = -1;
    std::wstring str = name.bstrVal;
    int pos = str.find(L":");

    // must be old format
    if (pos == std::wstring::npos)
    {
        try
        {
            // backwards compatibility where old files stored the integer value only
            propId = boost::lexical_cast<int>(name.bstrVal);
        }
        catch(const boost::bad_lexical_cast &) {}
    }
    else
    // otherwise new format
    {
        // find PA_SHORT_NAME:PA_LONG_NAME in str
        CComVariant strPre = str.substr(0, pos).c_str();
        // alter str to be after ":"
        str = str.substr(pos+1);

        if(vertex)
            propId = m_view->GetMeta().GetPropertyAttributeId(META_VERTEX, vertex->GetCategory(), PA_SHORT_NAME, strPre);
        else
            propId = m_view->GetMeta().GetPropertyAttributeId(META_SUBGRAPH, sg->GetCategory(), PA_SHORT_NAME, strPre);


        if (propId == -1)
        {
            // try it as just numeric, ie: propid:PA_LONG_NAME
            try
            {
                propId = boost::lexical_cast<int>(strPre.bstrVal);
            }
            catch(const boost::bad_lexical_cast &) {}
        }

        // set meta property name
        if (propId != -1 && str.length() > 0)
        {
            if(vertex)
                m_view->SetPropertyAttribute(META_VERTEX, vertex->GetCategory(), propId, PA_LONG_NAME, str.c_str());
            else
                m_view->SetPropertyAttribute(META_SUBGRAPH, sg->GetCategory(), propId, PA_LONG_NAME, str.c_str());
        }
    }

    // set property
    if(vertex)
        vertex->SetProperty(propId, value);
    else
        sg->SetProperty(propId, value);
}

void CXmlAnbDeserializer::AddImage(const CComPtr<IGraphItem>& item, Element& e)
{
    ImageInfo imginfo;
    try
    {
        CSize size(0,0);
        if (e.m_attr[_T("Width")].length() > 0)
            size.cx=boost::lexical_cast<int>(e.m_attr[_T("Width")].c_str());
        if (e.m_attr[_T("Height")].length() > 0)
            size.cy=boost::lexical_cast<int>(e.m_attr[_T("Height")].c_str());
        imginfo.SetSize(size);
    }
    catch(const boost::bad_lexical_cast &) {}

    GetImageInfo(e.m_attr[_T("Position")].c_str(), e.GetContent(), imginfo);
    if (imginfo.image != 0)
    {
        // check for "magic" transparent colors
        if (imginfo.transcolor != 0x00d3d3d3 && imginfo.transcolor != 0x00d30000 && imginfo.transcolor != 0x00d60000)
            imginfo.transcolor=0x0; // don't use first pixel as transparency in that case.

        CComQIPtr<IGraphVertex> vertex=item;
        if(vertex)
        {
            m_view->SetVertexImage(vertex->GetID(), imginfo, 0);
        }
        else
        {
            CComQIPtr<IGraphSubgraph> sg=item;
            ATLASSERT(sg);
            m_view->SetSubgraphImage(sg->GetID(), imginfo, 0);
        }
    }
}

const ImageInfo& CXmlAnbDeserializer::GetImageInfo(const TCHAR* position, const TCHAR* imgStr, ImageInfo& imginfo)
{
    CComPtr<IPicture> imgRead;
    variant_t varPos = position;
    varPos.ChangeType(VT_BSTR); // needed any more??
    bstr_t imgBase64 = imgStr;
    imginfo.pos=ImagePositionFromString(bstr_t(varPos));
    CComPtr<IPicture> ipic=IPictureFromString(std::string(CW2A(imgBase64, CP_UTF8)), imgRead);
    CComPtr<GraphTypes::IBitmap> iBitmap=GetIBitmapFromIPicture(ipic);
    boost::scoped_ptr<CBitmap> cbmp(iBitmap->GetCBitmap());
    COLORREF firstPixel;
    GetFirstPixel((HBITMAP)*cbmp, firstPixel);
    imginfo.image=iBitmap;
    imginfo.transcolor=firstPixel;
    return imginfo; // convenience only
}

void CXmlAnbDeserializer::AddLink(const TCHAR* end1Id, const TCHAR* end2Id, const TCHAR* type, const TCHAR* label)
{
    CUniqueID idFrom = GetVertexUniqueID(bstr_t(end1Id));
    CUniqueID idTo = GetVertexUniqueID(bstr_t(end2Id));

    if (!idFrom.isEmpty() && !idTo.isEmpty())
    {
        std::_tstring linkCat = type;
        CCategory category;
        m_view->AddEdge(DeserializeEdgeCategory(linkCat, category), idFrom, idTo, label);
    }
}

CUniqueID CXmlAnbDeserializer::GetVertexUniqueID(const bstr_t& bstrId)
{
    // get guid, category and id as full string
    std::_tstring id = (TCHAR*)bstrId;

    std::string buff = CT2A(id.c_str(), CP_UTF8);
    id = CA2T(Decrypt(Decode(buff)).c_str(), CP_UTF8);
    // check for old relavint icons, and remove if found (covered by relavint now)
    if (id.find(_T("7887DC5C9FE4AC41A3D05347BDBBC932+"))==0)
        id = id.substr(_tcslen(_T("7887DC5C9FE4AC41A3D05347BDBBC932+")));

    // disassemble string into parts
    CCategory cat;
    // find xxx:id in str
    std::_tstring strPre;
    int pos = id.find(':');
    ATLASSERT(pos != std::_tstring::npos);

    if (pos != std::_tstring::npos)
    {
        strPre = id.substr(0, pos);
        // alter id to be after ":"
        id = id.substr(pos+1);
    }
    // create Category from it
    DeserializeVertexCategory(strPre, cat);
    return CUniqueID(cat, id.c_str());
}

CUniqueID CXmlAnbDeserializer::GetSubgraphUniqueID(const bstr_t& bstrId)
{
    // get guid, category and id as full string
    std::_tstring id = (TCHAR*)bstrId;

    std::string buff = CT2A(id.c_str(), CP_UTF8);
    id = CA2T(Decrypt(Decode(buff)).c_str(), CP_UTF8);
    //TODO is this needed for subgraphs?
    // check for old relavint icons, and remove if found (covered by relavint now)
    if (id.find(_T("7887DC5C9FE4AC41A3D05347BDBBC932+"))==0)
        id = id.substr(_tcslen(_T("7887DC5C9FE4AC41A3D05347BDBBC932+")));

    // disassemble string into parts
    CCategory cat;
    // find xxx:id in str
    std::_tstring strPre;
    int pos = id.find(':');
    ATLASSERT(pos != std::_tstring::npos);

    if (pos != std::_tstring::npos)
    {
        strPre = id.substr(0, pos);
        // alter id to be after ":"
        id = id.substr(pos+1);
    }
    // create Category from it
    DeserializeSubgraphCategory(strPre, cat);
    return CUniqueID(cat, id.c_str());
}

const CCategory& CXmlAnbDeserializer::DeserializeVertexCategory(const std::_tstring& catIn, CCategory& category)
{
    CGuid guid;
    bstr_t strCat;

    DeserializeCategory(catIn, guid, strCat);

    std::_tstring strGuid;
    guid.ToString(strGuid);

    CString catFull = strGuid.c_str(); catFull += OLESTR("+"); catFull += (LPCWSTR)strCat;

    category = m_view->GetMeta().GetMetaVertexXmlCatLookup(catFull);
    if (category.isEmpty()) {
        CComVariant cat = (LPCWSTR) strCat;
        if (!SUCCEEDED(cat.ChangeType(VT_I4)))
            cat = -1;
        category = CCategory(guid.GetGUID(), cat.iVal);
    }
    return category;
}

const CCategory& CXmlAnbDeserializer::DeserializeEdgeCategory(const std::_tstring& catIn, CCategory& category)
{
    CGuid guid;
    bstr_t strCat;

    DeserializeCategory(catIn, guid, strCat);

    std::_tstring strGuid;
    guid.ToString(strGuid);

    CString catFull = strGuid.c_str(); catFull += OLESTR("+"); catFull += (LPCWSTR)strCat;

    category = m_view->GetMeta().GetMetaEdgeXmlCatLookup(catFull);
    if (category.isEmpty()) {
        CComVariant cat = (LPCWSTR) strCat;
        if (!SUCCEEDED(cat.ChangeType(VT_I4)))
            cat = -1;
        category = CCategory(guid.GetGUID(), cat.iVal);
    }
    return category;
}

const CCategory& CXmlAnbDeserializer::DeserializeSubgraphCategory(const std::_tstring& catIn, CCategory& category)
{
    CGuid guid;
    bstr_t strCat;

    DeserializeCategory(catIn, guid, strCat);

    std::_tstring strGuid;
    guid.ToString(strGuid);

    CString catFull = strGuid.c_str(); catFull += OLESTR("+"); catFull += (LPCWSTR)strCat;

    category = m_view->GetMeta().GetMetaSubgraphXmlCatLookup(catFull);
    if (category.isEmpty()) {
        CComVariant cat = (LPCWSTR) strCat;
        if (!SUCCEEDED(cat.ChangeType(VT_I4)))
            cat = -1;
        category = CCategory(guid.GetGUID(), cat.iVal);
    }
    return category;
}

void CXmlAnbDeserializer::DeserializeCategory(const std::_tstring& catIn, CGuid& guid, bstr_t& strCat)
{
    // find plugin_guid+cat in str
    int pos = catIn.find('+');
    if (pos != std::_tstring::npos)
    {
        guid = CT2A(catIn.substr(0, pos).c_str());
        strCat = catIn.substr(pos+1).c_str();
    }
    else
    {
        strCat = catIn.c_str();
        // create guid from default
        guid = guidDefault;
    }
}

IPicture* CXmlAnbDeserializer::IPictureFromString(const std::string& img, CComPtr<IPicture>& retVal) const
{
    if (img.length() > 0)
    {
        int len = Base64DecodeGetRequiredLength(img.length());

        BYTE* imgBytes = new BYTE[len + 1];

        Base64Decode(img.c_str(), len, imgBytes, &len);

        CComPtr<IStream> lpStream;
        if (SUCCEEDED(::CreateStreamOnHGlobal(NULL, TRUE, &lpStream)))
        {
            HRESULT hRes = S_OK;
            LARGE_INTEGER liStart = {0};
            
            hRes = lpStream->Seek(liStart, STREAM_SEEK_SET, 0);
            ATLASSERT(SUCCEEDED(hRes));

            ULONG lenWritten = 0;
            hRes = lpStream->Write(imgBytes, len, &lenWritten);
            ATLASSERT(SUCCEEDED(hRes));
            ATLASSERT(lenWritten == len);
            
            hRes = lpStream->Commit(STGC_DEFAULT);
            ATLASSERT(SUCCEEDED(hRes));

            hRes = lpStream->Seek(liStart, STREAM_SEEK_SET, 0);
            ATLASSERT(SUCCEEDED(hRes));

            hRes = ::OleLoadPicture(lpStream.p, len, TRUE, IID_IPicture, reinterpret_cast<void**>(&retVal));
            ATLASSERT(SUCCEEDED(hRes));
        }

        delete[](imgBytes);
    }
    return retVal; // purely for callers convenience
}

IMAGE_POSITION CXmlAnbDeserializer::ImagePositionFromString(const bstr_t& pos)
{
    if (pos == bstr_t(OLESTR("NW")))
        return POSITION_NORTH_WEST;
    else if	(pos == bstr_t(OLESTR("N")))
        return POSITION_NORTH;
    else if	(pos == bstr_t(OLESTR("NE")))
        return POSITION_NORTH_EAST;
    else if	(pos == bstr_t(OLESTR("W")))
        return POSITION_WEST;
    else if	(pos == bstr_t(OLESTR("C")))
        return POSITION_CENTER;
    else if	(pos == bstr_t(OLESTR("E")))
        return POSITION_EAST;
    else if	(pos == bstr_t(OLESTR("SW")))
        return POSITION_SOUTH_WEST;
    else if	(pos == bstr_t(OLESTR("S")))
        return POSITION_SOUTH;
    else if	(pos == bstr_t(OLESTR("SE")))
        return POSITION_SOUTH_EAST;
    ATLASSERT(!"ImagePositionFromString - Function incorrectly called!");
    return POSITION_SOUTH;
}

///////////////////////////////////////////////////////////////////////////////////

CXmlAnbSerializer::CXmlAnbSerializer(CGraphViewCanvas* view) : m_view(view)
{
    CComInitialize com;
    HRESULT hr = S_OK;
    if (m_pWriter == NULL)
    {
        hr = m_pWriter.CoCreateInstance(CLSID_MXXMLWriter30, NULL);
        _ASSERT(SUCCEEDED(hr));
    }

    // create attributes
    if (m_pMXAttrs == NULL)
    {
        hr = m_pMXAttrs.CoCreateInstance(CLSID_SAXAttributes30, NULL);
        _ASSERT(SUCCEEDED(hr));
    }

    m_pSAXAttrs = m_pMXAttrs;

    // set them all to writer, writer implements all these interfaces
    m_pContentHandler = m_pWriter;
    //CComQIPtr<ISAXDTDHandler> pDTDHandler(pWriter);
    //CComQIPtr<ISAXLexicalHandler> pLexicalHandler(pWriter);
    //CComQIPtr<ISAXDeclHandler> pDeclHandler(pWriter);
    //CComQIPtr<ISAXErrorHandler> pErrorHandler(pWriter);
}

CXmlAnbSerializer::~CXmlAnbSerializer()
{
}

const TCHAR* CXmlAnbSerializer::Save(std::_tstring & retVal, SGV::XML_SAVE_FORMAT saveFormat, const CPoint& relativeSavePos)
{
    variant_t var;

    m_pWriter->put_indent(VARIANT_TRUE);

    HRESULT hr = m_pWriter->put_output(var);
    _ASSERT(SUCCEEDED(hr));

    // and manually call necessary events to generate XML file
    m_pContentHandler->startDocument();

    // <Chart ...>
    m_pMXAttrs->addAttribute(bstr_t(OLESTR("")), bstr_t(OLESTR("")), bstr_t(L"IdReferenceLinking"), bstr_t(OLESTR("")), bstr_t(L"false"));
    m_pMXAttrs->addAttribute(bstr_t(OLESTR("")), bstr_t(OLESTR("")), bstr_t(L"Rigorous"), bstr_t(OLESTR("")), bstr_t(L"true"));
    m_pContentHandler->startElement(OLESTR(""), 0, OLESTR(""), 0, L"Chart", wcslen(L"Chart"), m_pSAXAttrs);
    m_pMXAttrs->clear();

    MetaValueMap::const_iterator item, end;
    bool relavintSpecific=(saveFormat&SGV::XML_SAVE_FORMAT_ANB_COMPATIBLE) == 0;

    if (relavintSpecific)
    {
        //Store Zoom and Spacing
        m_pMXAttrs->addAttribute(bstr_t(OLESTR("")), bstr_t(OLESTR("")), bstr_t(L"Zoom"), bstr_t(OLESTR("")), bstr_t(static_cast<int>(100.0f * m_view->GetScale())));
        m_pMXAttrs->addAttribute(bstr_t(OLESTR("")), bstr_t(OLESTR("")), bstr_t(L"Spacing"), bstr_t(OLESTR("")), bstr_t(static_cast<int>(100.0f * m_view->GetSpacing())));
        m_pContentHandler->startElement(OLESTR(""), 0, OLESTR(""), 0, L"ViewSettings", wcslen(L"ViewSettings"), m_pSAXAttrs);
        m_pContentHandler->endElement(OLESTR(""), 0, OLESTR(""), 0, L"ViewSettings", wcslen(L"ViewSettings"));
        m_pMXAttrs->clear();

        // <PluginCollection>
        m_pContentHandler->startElement(OLESTR(""), 0, OLESTR(""), 0, L"PluginCollection", wcslen(L"PluginCollection"), m_pSAXAttrs);

        // iterate through all meta data and get the plugin names
        std::pair<MetaValueMap::const_iterator, MetaValueMap::const_iterator> itr_pair = m_view->GetMeta().GetIterator();
        item = itr_pair.first;
        end = itr_pair.second;
        for (; item != end; ++item) {
            if ((item->first.get<0>()) == META_CATEGORY && (item->first.get<1>()) == PLUGIN_NAME) {
                CCategory cat = (item->first.get<2>());
                SavePlugin(cat.GetPlugin(), bstr_t(item->second));
            }
        }

        // </PluginCollection>
        m_pContentHandler->endElement(OLESTR(""), 0, OLESTR(""), 0, L"PluginCollection", wcslen(L"PluginCollection"));
    }

    // <EntityTypeCollection>
    m_pContentHandler->startElement(OLESTR(""), 0, OLESTR(""), 0, L"EntityTypeCollection", wcslen(L"EntityTypeCollection"), m_pSAXAttrs);

    //TODO check if this iterator will ever return both vertices and subgraphs
    // iterate through all meta data and get the vertex and subgraph xml categories

    std::pair<MetaValueMap::const_iterator, MetaValueMap::const_iterator> itr_pair = m_view->GetMeta().GetIterator();
    item = itr_pair.first;
    end = itr_pair.second;
    for (; item != end; ++item) 
    {
        if((item->first.get<1>()) == XML_ANBICONFILE) 
        {
            CCategory cat = (item->first.get<2>());
            if ((item->first.get<0>()) == META_VERTEX)
                SaveVertexEntityType(cat, bstr_t(item->second), relavintSpecific);
            if(relavintSpecific)
                if ((item->first.get<0>()) == META_SUBGRAPH)
                    SaveSubgraphEntityType(cat, bstr_t(item->second), true);
        }
    }

    // </EntityTypeCollection>
    m_pContentHandler->endElement(OLESTR(""), 0, OLESTR(""), 0, L"EntityTypeCollection", wcslen(L"EntityTypeCollection"));

    // <LinkTypeCollection>
    m_pContentHandler->startElement(OLESTR(""), 0, OLESTR(""), 0, L"LinkTypeCollection", wcslen(L"LinkTypeCollection"), m_pSAXAttrs);

    // iterate through all meta data and get the edge xml categories
    itr_pair = m_view->GetMeta().GetIterator();
    item = itr_pair.first;
    end = itr_pair.second;
    for (; item != end; ++item) {
        if ((item->first.get<0>()) == META_EDGE && (item->first.get<1>()) == XML_CATEGORY_CODE) {
            CCategory cat = (item->first.get<2>());
            SaveLinkType(cat, relavintSpecific);
        }
    }

    // </LinkTypeCollection>
    m_pContentHandler->endElement(OLESTR(""), 0, OLESTR(""), 0, L"LinkTypeCollection", wcslen(L"LinkTypeCollection"));

    // <ChartItemCollection>
    m_pContentHandler->startElement(OLESTR(""), 0, OLESTR(""), 0, L"ChartItemCollection", wcslen(L"ChartItemCollection"), m_pSAXAttrs);

    CPoint itemPos;
    GraphTypes::PointF offset(relativeSavePos);
    m_view->GetCanvas()->RAllTransform(offset);
    offset.x = -offset.x;
    offset.y = -offset.y;

    CUniqueIDVector graphitems;

    // iterate all graph items, vertices & subgraphs, or selected only if requested
    m_view->GetChildren(ROOT_ID, &graphitems, TRISTATE_BOTH, TRISTATE_BOTH, saveFormat & SGV::XML_SAVE_FORMAT_SELECTED_ONLY ? TRISTATE_TRUE : TRISTATE_BOTH);
    while (graphitems.MoveNext())
    {
        CComQIPtr<IGraphVertex> vertex = m_view->GetVertex(graphitems.Get());
        if (saveFormat & SGV::XML_SAVE_FORMAT_SELECTED_ONLY)
        {
            if(vertex)
            {
                CUniqueIDVector adjVertices;
                m_view->GetAdjacentVertices(&adjVertices, graphitems.Get());
                while (adjVertices.MoveNext())
                {
                    m_view->GetPosition(adjVertices.Get(), &itemPos, GraphTypes::world);
                    itemPos.Offset(static_cast<POINT>(offset));
                    if (m_view->GetMeta().IsHidden(m_view->GetVertex(adjVertices.Get())))
                        SaveEntity(adjVertices.Get(), saveFormat, itemPos);
                }
            }
        }

        m_view->GetPosition(graphitems.Get(), &itemPos, GraphTypes::world);
        itemPos.Offset(static_cast<POINT>(offset));
        if(!vertex && !relavintSpecific) 
        {
            // encountering subgraph for ANB store just its vertices
            CUniqueIDVector items;
            m_view->GetChildren(graphitems.Get(), &items, TRISTATE_BOTH, TRISTATE_BOTH, TRISTATE_BOTH);
            while (items.MoveNext())
            {
                SaveEntity(items.Get(), saveFormat, itemPos);
            }
        }
        else
        {
            SaveEntity(graphitems.Get(), saveFormat, itemPos);
        }
    }

#ifdef _DEBUG
    int count = 0;
#endif
    IGraphEdgeVector edges;
    m_view->GetGraph()->GetEdges(edges);
    for(IGraphEdgeVector::const_iterator eitr = edges.begin(); eitr != edges.end(); ++eitr)
    {
        IGraphEdge * edge = *eitr;
#ifdef _DEBUG
        bool bBothSelected = (m_view->IsSelected(edge->GetSource()->GetID()) && m_view->IsSelected(edge->GetTarget()->GetID()));
        bool bBothSubgraphed = ((edge->GetSource()->GetParent()->GetID()!=ROOT_ID) && (edge->GetTarget()->GetParent()->GetID()!=ROOT_ID));
        bool bSSelectedTHiddenOrSubgraphed = (m_view->IsSelected(edge->GetSource()->GetID()) && (m_view->GetMeta().IsHidden(edge->GetTarget()) || (edge->GetTarget()->GetParent()->GetID()!=ROOT_ID)));
        bool bTSelectedSHiddenOrSubgraphed = (m_view->IsSelected(edge->GetTarget()->GetID()) && (m_view->GetMeta().IsHidden(edge->GetSource()) || (edge->GetSource()->GetParent()->GetID()!=ROOT_ID)));
#endif
        // include either all edges, or if both selected, or both are inside a subgraph, or only an edge where one item is selected, and connected to either a hidden item, or another selected item.
        if (!(saveFormat & SGV::XML_SAVE_FORMAT_SELECTED_ONLY)
            || (m_view->IsSelected(edge->GetSource()->GetID()) && m_view->IsSelected(edge->GetTarget()->GetID()))
            || ((edge->GetSource()->GetParent()->GetID()!=ROOT_ID) && (edge->GetTarget()->GetParent()->GetID()!=ROOT_ID))
            || (m_view->IsSelected(edge->GetSource()->GetID()) && (m_view->GetMeta().IsHidden(edge->GetTarget()) || (edge->GetTarget()->GetParent()->GetID()!=ROOT_ID)))
            || (m_view->IsSelected(edge->GetTarget()->GetID()) && (m_view->GetMeta().IsHidden(edge->GetSource()) || (edge->GetSource()->GetParent()->GetID()!=ROOT_ID))))
        {
#ifdef _DEBUG
            ATLTRACE("count %ld\n", ++count);
#endif
            SaveLink(eitr->get());
        }
    }

    // </ChartItemCollection>
    m_pContentHandler->endElement(OLESTR(""), 0, OLESTR(""), 0, L"ChartItemCollection", wcslen(L"ChartItemCollection"));

    // </Chart>
    m_pContentHandler->endElement(OLESTR(""), 0, OLESTR(""), 0, L"Chart", wcslen(L"Chart"));

    var = OLESTR("");
    m_pWriter->flush();
    hr = m_pWriter->get_output(&var);
    _ASSERT(SUCCEEDED(hr));
    retVal = CW2T(var.bstrVal, CP_UTF8);
    return retVal.c_str();
}

void CXmlAnbSerializer::SavePlugin(const CGuid& pluginId, const TCHAR* name)
{
    std::_tstring pluginStr;
    m_pMXAttrs->addAttribute(bstr_t(OLESTR("")), bstr_t(OLESTR("")), bstr_t(L"Id"), bstr_t(OLESTR("")), bstr_t(pluginId.ToString(pluginStr)));
    m_pMXAttrs->addAttribute(bstr_t(OLESTR("")), bstr_t(OLESTR("")), bstr_t(L"Name"), bstr_t(OLESTR("")), bstr_t(name));
    // <Plugin Id=xxx Name="xxx">
    m_pContentHandler->startElement(OLESTR(""), 0, OLESTR(""), 0, L"Plugin", wcslen(L"Plugin"), m_pSAXAttrs);
    m_pMXAttrs->clear();

    // </Plugin>
    m_pContentHandler->endElement(OLESTR(""), 0, OLESTR(""), 0, L"Plugin", wcslen(L"Plugin"));
}

void CXmlAnbSerializer::IPictureToString(const CComPtr<IPicture>& img, std::string& retVal)
{
    if (img != NULL)
    {
        CComPtr<IStream> lpStream;
        if (SUCCEEDED(::CreateStreamOnHGlobal(NULL, TRUE, &lpStream)))
        {
            long srcLen = 0;
            img->SaveAsFile(lpStream, TRUE, &srcLen);
            bool status = SUCCEEDED(lpStream->Commit(STGC_DEFAULT));
            ATLASSERT(status);

            DWORD base64Flags = ATL_BASE64_FLAG_NOPAD | ATL_BASE64_FLAG_NOCRLF;
            int resultLen = Base64EncodeGetRequiredLength(srcLen, base64Flags) + 1;

            BYTE* source = (BYTE*)malloc(srcLen);
            char* result = (char*)malloc(resultLen);
            LARGE_INTEGER liStart = {0};
            lpStream->Seek(liStart, STREAM_SEEK_SET, 0);
            ULONG srcReadLen = 0; 
            lpStream->Read(source, srcLen, &srcReadLen);
            ATLASSERT(srcReadLen == srcLen);

            Base64Encode(source, srcLen, result, &resultLen, base64Flags);
            retVal.assign(result, resultLen);
            free(result);
            free(source);
        }
    }
}

BSTR CXmlAnbSerializer::ImagePositionToString(IMAGE_POSITION pos)
{
    switch (pos)
    {
    case POSITION_NORTH_WEST:
        return OLESTR("NW");
    case POSITION_NORTH:
        return OLESTR("N");
    case POSITION_NORTH_EAST:
        return OLESTR("NE");
    case POSITION_WEST:
        return OLESTR("W");
    case POSITION_CENTER:
        return OLESTR("C");
    case POSITION_EAST:
        return OLESTR("E");
    case POSITION_SOUTH_WEST:
        return OLESTR("SW");
    case POSITION_SOUTH:
        return OLESTR("S");
    case POSITION_SOUTH_EAST:
        return OLESTR("SE");
    default:
        return OLESTR("");
    }
}

void CXmlAnbSerializer::SaveEntity(const CUniqueID& iID, SGV::XML_SAVE_FORMAT saveFormat, const GraphTypes::PointF& location)
{
    ATLASSERT(m_view);
    std::_tstring category;
    CComQIPtr<IGraphVertex> vertex = m_view->GetVertex(iID);
    CComQIPtr<IGraphSubgraph> sg = m_view->GetSubgraph(iID);
    ATLASSERT((vertex || sg) && !(vertex && sg));
    bool isvertex=vertex!=0;

    // serialize vertices before subgraph itself
    if(!isvertex)
    {
        CUniqueIDVector items;
        m_view->GetChildren(iID, &items, TRISTATE_BOTH, TRISTATE_BOTH, TRISTATE_BOTH);
        while (items.MoveNext())
            SaveEntity(items.Get(), saveFormat, location);
    }

    bool metaDefined = isvertex ?
        VertexCategoryToString(iID.GetCategory(), category)
    :	SubgraphCategoryToString(iID.GetCategory(), category);

    std::_tstring label(isvertex?vertex->GetLabel():sg->GetLabel());

    std::_tstring catId;
    if(isvertex)
        VertexUniqueIdToString(iID, catId);
    else
        SubgraphUniqueIdToString(iID, catId);

    std::string buff = CT2A(catId.c_str(), CP_UTF8);
    catId = CA2T(Encode(Encrypt(buff)).c_str(), CP_UTF8);

    // <ChartItem>
    m_pMXAttrs->addAttribute(bstr_t(OLESTR("")), bstr_t(OLESTR("")), bstr_t(L"Label"), bstr_t(OLESTR("")), bstr_t(label.c_str()));
    m_pContentHandler->startElement(OLESTR(""), 0, OLESTR(""), 0, L"ChartItem", wcslen(L"ChartItem"), m_pSAXAttrs);
    m_pMXAttrs->clear();
    // <End>
    m_pMXAttrs->addAttribute(bstr_t(OLESTR("")), bstr_t(OLESTR("")), bstr_t(L"X"), bstr_t(OLESTR("")), bstr_t((int)location.x));
    m_pMXAttrs->addAttribute(bstr_t(OLESTR("")), bstr_t(OLESTR("")), bstr_t(L"Y"), bstr_t(OLESTR("")), bstr_t((int)location.y));
    m_pContentHandler->startElement(OLESTR(""), 0, OLESTR(""), 0, L"End", wcslen(L"End"), m_pSAXAttrs);
    m_pMXAttrs->clear();

    // <Entity>
    m_pMXAttrs->addAttribute(bstr_t(OLESTR("")), bstr_t(OLESTR("")), bstr_t(L"EntityId"), bstr_t(OLESTR("")), bstr_t(catId.c_str()));
    m_pMXAttrs->addAttribute(bstr_t(OLESTR("")), bstr_t(OLESTR("")), bstr_t(L"Identity"), bstr_t(OLESTR("")), bstr_t(catId.c_str()));

    if ((saveFormat&SGV::XML_SAVE_FORMAT_ANB_COMPATIBLE) == 0)
    {
        if((isvertex && m_view->GetVertexVisible(iID)) || 
            (!isvertex && m_view->GetSubgraphVisible(iID))) 
            m_pMXAttrs->addAttribute(bstr_t(OLESTR("")), bstr_t(OLESTR("")), bstr_t(L"Visible"), bstr_t(OLESTR("")), bstr_t(OLESTR("-1")));

        if(isvertex)
        {
            CComQIPtr<IGraphSubgraph> parent = vertex->GetParent();
            CUniqueID pid=parent->GetID();
            if(pid!=ROOT_ID) // not top level vertex	
            {
                std::_tstring parentId;
                SubgraphUniqueIdToString(pid, parentId);
                std::string buff = CT2A(parentId.c_str(), CP_UTF8);
                parentId = CA2T(Encode(Encrypt(buff)).c_str(), CP_UTF8);
                m_pMXAttrs->addAttribute(bstr_t(OLESTR("")), bstr_t(OLESTR("")), bstr_t(L"ParentId"), bstr_t(OLESTR("")), bstr_t(parentId.c_str()));
            }
        }
    }

    m_pContentHandler->startElement(OLESTR(""), 0, OLESTR(""), 0, L"Entity", wcslen(L"Entity"), m_pSAXAttrs);
    m_pMXAttrs->clear();

    // <Icon>
    m_pContentHandler->startElement(OLESTR(""), 0, OLESTR(""), 0, L"Icon", wcslen(L"Icon"), m_pSAXAttrs);
    // <IconStyle>
    if (metaDefined)
        m_pMXAttrs->addAttribute(bstr_t(OLESTR("")), bstr_t(OLESTR("")), bstr_t(L"Type"), bstr_t(OLESTR("")), bstr_t(category.c_str()));
    else
        m_pMXAttrs->addAttribute(bstr_t(OLESTR("")), bstr_t(OLESTR("")), bstr_t(L"Type"), bstr_t(OLESTR("")), bstr_t(L"Entity"));
    m_pContentHandler->startElement(OLESTR(""), 0, OLESTR(""), 0, L"IconStyle", wcslen(L"IconStyle"), m_pSAXAttrs);
    m_pMXAttrs->clear();
    // </IconStyle>
    m_pContentHandler->endElement(OLESTR(""), 0, OLESTR(""), 0, L"IconStyle", wcslen(L"IconStyle"));
    // </Icon>
    m_pContentHandler->endElement(OLESTR(""), 0, OLESTR(""), 0, L"Icon", wcslen(L"Icon"));
    // note this is ORDER dependant!
    SaveCards(iID, isvertex);

    if ((saveFormat&SGV::XML_SAVE_FORMAT_ANB_COMPATIBLE) == 0)
    {
        // <Images>
        m_pContentHandler->startElement(OLESTR(""), 0, OLESTR(""), 0, L"Images", wcslen(L"Images"), m_pSAXAttrs);

        // check images
        const ImageInfoMaps* imgmaps=isvertex?vertex->GetImages():sg->GetImages();
        if(imgmaps != NULL)
        {
            for(ImageInfoMaps::const_iterator itrmap=imgmaps->begin(); itrmap != imgmaps->end(); ++itrmap)
            {
                for(MapImageInfo::const_iterator itrimg =itrmap->second.begin(); itrimg != itrmap->second.end(); ++itrimg)
                {
                    if (itrimg->second.image != 0)
                    {
                        boost::scoped_ptr<CBitmap> cbmp(itrimg->second.image->GetCBitmap());
                        CComQIPtr<IPicture> ipic=GetIPictureFromBitmap((HBITMAP)(*cbmp));
                        ATLASSERT(ipic != NULL);
                        CSize size;
                        itrimg->second.GetSize(size);
                        SaveImage(ipic, itrimg->second.pos, size);
                    }
                }
            }
        }

        //// check all images
        //for (int pos = POSITION_NORTH_WEST; pos <= POSITION_SOUTH_EAST; pos++)
        //{
        //	// get the image at given position, layer 0, for this specific vertex (i.e. ignore meta)
        //	CComPtr<GraphTypes::IBitmap> iBitmap = m_view->GetVertexImage(iID, static_cast<IMAGE_POSITION>(pos), 0, true);
        //	if (iBitmap != 0)
        //	{
        //		boost::scoped_ptr<CBitmap> cbmp(iBitmap->GetCBitmap());
        //		CComQIPtr<IPicture> iPicture = GetIPictureFromBitmap(static_cast<HBITMAP>(*cbmp));
        //		ATLASSERT(iPicture != 0);
        //		CSize size;
        //		cbmp.get()->GetSize(size);
        //		SaveImage(iPicture, static_cast<IMAGE_POSITION>(pos), size);
        //	}
        //}

        // </Images>
        m_pContentHandler->endElement(OLESTR(""), 0, OLESTR(""), 0, L"Images", wcslen(L"Images"));
    }

    // </Entity>
    m_pContentHandler->endElement(OLESTR(""), 0, OLESTR(""), 0, L"Entity", wcslen(L"Entity"));
    // </End>
    m_pContentHandler->endElement(OLESTR(""), 0, OLESTR(""), 0, L"End", wcslen(L"End"));
    // </ChartItem>
    m_pContentHandler->endElement(OLESTR(""), 0, OLESTR(""), 0, L"ChartItem", wcslen(L"ChartItem"));
}

void CXmlAnbSerializer::SaveImage(const CComQIPtr<IPicture>& img, IMAGE_POSITION pos, const CSize& size)
{
    if (img != NULL)
    {
        // <Image Position="NW">
        m_pMXAttrs->addAttribute(bstr_t(OLESTR("")), bstr_t(OLESTR("")), bstr_t(L"Position"), bstr_t(OLESTR("")), bstr_t(ImagePositionToString(static_cast<IMAGE_POSITION>(pos))));
        m_pMXAttrs->addAttribute(bstr_t(OLESTR("")), bstr_t(OLESTR("")), bstr_t(L"Width"), bstr_t(OLESTR("")), bstr_t(size.cx));
        m_pMXAttrs->addAttribute(bstr_t(OLESTR("")), bstr_t(OLESTR("")), bstr_t(L"Height"), bstr_t(OLESTR("")), bstr_t(size.cy));

        m_pContentHandler->startElement(OLESTR(""), 0, OLESTR(""), 0, L"Image", wcslen(L"Image"), m_pSAXAttrs);
        //base 64 encode image bytes
        std::string base64Str;
        IPictureToString(img, base64Str);
        // <Image Position=XX ... attribute

        // set blob of chars (image b64 encoded)
        m_pContentHandler->characters(CA2W(base64Str.c_str(), CP_UTF8), base64Str.length());
        // </Image>
        m_pContentHandler->endElement(OLESTR(""), 0, OLESTR(""), 0, L"Image", wcslen(L"Image"));
        m_pMXAttrs->clear();
    }
}

void CXmlAnbSerializer::SaveCards(const CUniqueID& iID, bool isvertex)
{
    // <CardCollection>
    m_pContentHandler->startElement(OLESTR(""), 0, OLESTR(""), 0, L"CardCollection", wcslen(L"CardCollection"), m_pSAXAttrs);

    CPropertyIDVector properties;
    m_view->GetProperties(&properties, iID);

    // iterate through all the properties for a given vertex or subgraph
    while (properties.MoveNext())
    {
        int propId = properties.Get();
        if (propId == -1)
            continue; // skip the first property (label)
        CComVariant val;
        m_view->GetProperty(iID, propId, val);

        if (val.vt == VT_BSTR)
        {
            variant_t varPropId;
            variant_t varPropCode;
            variant_t varPropDisplayName;

            // TODO: put back when complete - SHORT_NAME
            //m_view->GetPropertyAttribute(META_VERTEX, iID.GetCategory(), propId, PA_SHORT_NAME, varPropCode);
            if(isvertex)
                m_view->GetPropertyAttribute(META_VERTEX, iID.GetCategory(), propId, PA_LONG_NAME, varPropDisplayName);
            else
                m_view->GetPropertyAttribute(META_SUBGRAPH, iID.GetCategory(), propId, PA_LONG_NAME, varPropDisplayName);

            ATLASSERT(varPropDisplayName.vt == VT_BSTR || varPropDisplayName.vt == VT_EMPTY);

            if (varPropDisplayName.vt == VT_EMPTY)
                continue;

            varPropId = propId;
            varPropId.ChangeType(VT_BSTR);
            varPropCode.ChangeType(VT_BSTR);

            bstr_t propStr;

            // Use the Property Code (short name) if available
            if (wcslen(varPropCode.bstrVal))
                propStr = varPropCode;
            else
                propStr = varPropId;

            propStr += OLESTR(":");
            propStr += bstr_t(varPropDisplayName);

            // <Card>
            m_pMXAttrs->addAttribute(bstr_t(OLESTR("")), bstr_t(OLESTR("")), bstr_t(L"SourceReference"), bstr_t(OLESTR("")), propStr);
            m_pMXAttrs->addAttribute(bstr_t(OLESTR("")), bstr_t(OLESTR("")), bstr_t(L"Summary"), bstr_t(OLESTR("")), val.bstrVal);
            m_pContentHandler->startElement(OLESTR(""), 0, OLESTR(""), 0, L"Card", wcslen(L"Card"), m_pSAXAttrs);
            m_pMXAttrs->clear();
            // </Card>
            m_pContentHandler->endElement(OLESTR(""), 0, OLESTR(""), 0, L"Card", wcslen(L"Card"));
        }
    }

    // </CardCollection>
    m_pContentHandler->endElement(OLESTR(""), 0, OLESTR(""), 0, L"CardCollection", wcslen(L"CardCollection"));
}

void CXmlAnbSerializer::SaveLink(const CComPtr<IGraphEdge>& e) 
{
    std::_tstring category;
    ATLASSERT(m_view);
    if (e == NULL)
    {
        ATLASSERT(FALSE); // bad
        return;
    }
    CComPtr<IGraphVertex> source = e->GetSource();
    CComPtr<IGraphVertex> target = e->GetTarget();
#ifdef _DEBUG
    bool metaDefined = 
#endif
    EdgeCategoryToString(e->GetCategory(), category);
#ifdef _DEBUG
    ATLASSERT(metaDefined);
#endif

    CUniqueID vIdFrom = source->GetID();
    CUniqueID vIdTo = target->GetID();

    std::_tstring catIdFrom;
    VertexUniqueIdToString(vIdFrom, catIdFrom);
    std::_tstring catIdTo;
    VertexUniqueIdToString(vIdTo, catIdTo);
    // encrypt from
    std::string buff = CT2A(catIdFrom.c_str(), CP_UTF8);
    catIdFrom = CA2T(Encode(Encrypt(buff)).c_str(), CP_UTF8);

    buff = CT2A(catIdTo.c_str(), CP_UTF8);
    catIdTo = CA2T(Encode(Encrypt(buff)).c_str(), CP_UTF8);

    // <ChartItem>
    if (_tcslen(e->GetLabel()))
        m_pMXAttrs->addAttribute(bstr_t(OLESTR("")), bstr_t(OLESTR("")), bstr_t(L"Label"), bstr_t(OLESTR("")), bstr_t(e->GetLabel()));
    m_pContentHandler->startElement(OLESTR(""), 0, OLESTR(""), 0, L"ChartItem", wcslen(L"ChartItem"), m_pSAXAttrs);
    m_pMXAttrs->clear();
    // <Link>
    m_pMXAttrs->addAttribute(bstr_t(OLESTR("")), bstr_t(OLESTR("")), bstr_t(L"End1Id"), bstr_t(OLESTR("")), bstr_t(catIdFrom.c_str()));
    m_pMXAttrs->addAttribute(bstr_t(OLESTR("")), bstr_t(OLESTR("")), bstr_t(L"End2Id"), bstr_t(OLESTR("")), bstr_t(catIdTo.c_str()));
    m_pContentHandler->startElement(OLESTR(""), 0, OLESTR(""), 0, L"Link", wcslen(L"Link"), m_pSAXAttrs);
    m_pMXAttrs->clear();
    // <LinkStyle>
    m_pMXAttrs->addAttribute(bstr_t(OLESTR("")), bstr_t(OLESTR("")), bstr_t(L"Type"), bstr_t(OLESTR("")), bstr_t(category.c_str()));
    m_pMXAttrs->addAttribute(bstr_t(OLESTR("")), bstr_t(OLESTR("")), bstr_t(L"ArrowStyle"), bstr_t(OLESTR("")), bstr_t(L"ArrowOnHead"));
    m_pContentHandler->startElement(OLESTR(""), 0, OLESTR(""), 0, L"LinkStyle", wcslen(L"LinkStyle"), m_pSAXAttrs);
    m_pMXAttrs->clear();
    // </LinkStyle>
    m_pContentHandler->endElement(OLESTR(""), 0, OLESTR(""), 0, L"LinkStyle", wcslen(L"LinkStyle"));
    // </Link>
    m_pContentHandler->endElement(OLESTR(""), 0, OLESTR(""), 0, L"Link", wcslen(L"Link"));
    // </ChartItem>
    m_pContentHandler->endElement(OLESTR(""), 0, OLESTR(""), 0, L"ChartItem", wcslen(L"ChartItem"));
}

bool CXmlAnbSerializer::CategoryToString(const CCategory& category, const std::_tstring & catCode, std::_tstring & result)
{
    bool metaFound = true;
    if (category.GetPlugin() == guidDefault)
        result = _T("");
    else
    {
        std::_tstring pluginStr;
        result = category.GetPlugin().ToString(pluginStr);
        result += _T("+");
    }
    if (catCode.length())
        result += catCode;
    else {
        TCHAR str[12];
        result += _itot(category.GetCategory(), &str[0], 10);
        metaFound = false;
    }
    return metaFound;
}

bool CXmlAnbSerializer::VertexCategoryToString(const CCategory& category, std::_tstring & result)
{
    std::_tstring catCode;
    m_view->GetMeta().GetMetaVertexXmlCatCode(category, catCode);
    return CategoryToString(category, catCode, result);
}

bool CXmlAnbSerializer::EdgeCategoryToString(const CCategory& category, std::_tstring & result)
{
    std::_tstring catCode;
    m_view->GetMeta().GetMetaEdgeXmlCatCode(category, catCode);
    return CategoryToString(category, catCode, result);
}

bool CXmlAnbSerializer::SubgraphCategoryToString(const CCategory& category, std::_tstring & result)
{
    std::_tstring catCode;
    m_view->GetMeta().GetMetaSubgraphXmlCatCode(category, catCode);
    return CategoryToString(category, catCode, result);
}

const TCHAR* CXmlAnbSerializer::VertexUniqueIdToString(const CUniqueID& vID, std::_tstring & result)
{
    std::_tstring cat;
    VertexCategoryToString(vID.GetCategory(), cat);
    result += cat;
    result += _T(":");
    result += vID.GetID();
    return result.c_str();
}

const TCHAR* CXmlAnbSerializer::SubgraphUniqueIdToString(const CUniqueID& sID, std::_tstring & result)
{
    std::_tstring cat;
    SubgraphCategoryToString(sID.GetCategory(), cat);
    result += cat;
    result += _T(":");
    result += sID.GetID();
    return result.c_str();
}

void CXmlAnbSerializer::SaveVertexEntityType(const CCategory& cat, const TCHAR* icon, bool saveExtraMeta)
{
    std::_tstring xmlCategory;
#ifdef _DEBUG
    bool bmetadefined=
#endif
    VertexCategoryToString(cat, xmlCategory);
#ifdef _DEBUG
    ATLASSERT(bmetadefined);
#endif

    m_pMXAttrs->clear();
    // <EntityType>
    m_pMXAttrs->addAttribute(bstr_t(OLESTR("")), bstr_t(OLESTR("")), bstr_t(L"Name"), bstr_t(OLESTR("")), bstr_t(xmlCategory.c_str()));
    m_pMXAttrs->addAttribute(bstr_t(OLESTR("")), bstr_t(OLESTR("")), bstr_t(L"IconFile"), bstr_t(OLESTR("")), bstr_t(icon));
    m_pContentHandler->startElement(OLESTR(""), 0, OLESTR(""), 0, L"EntityType", wcslen(L"EntityType"), m_pSAXAttrs);
    m_pMXAttrs->clear();

    if (saveExtraMeta)
    {
        // <Meta>
        const BSTR metaTag = L"Meta";
        m_pMXAttrs->addAttribute(bstr_t(OLESTR("")), bstr_t(OLESTR("")), bstr_t(L"Id"), bstr_t(OLESTR("")), bstr_t(variant_t(cat.GetCategory())));
        std::_tstring label;
        if (_tcslen(m_view->GetMeta().GetMetaVertexLabel(cat, label)))
            m_pMXAttrs->addAttribute(bstr_t(OLESTR("")), bstr_t(OLESTR("")), bstr_t(L"Label"), bstr_t(OLESTR("")), bstr_t(label.c_str()));
        if (m_view->GetMeta().GetMetaVertexHide(cat) == VISIBLE_HIDE)
            m_pMXAttrs->addAttribute(bstr_t(OLESTR("")), bstr_t(OLESTR("")), bstr_t(L"Deselected"), bstr_t(OLESTR("")), bstr_t(OLESTR("-1")));
        m_pContentHandler->startElement(OLESTR(""), 0, OLESTR(""), 0, metaTag, wcslen(metaTag), m_pSAXAttrs);
        m_pMXAttrs->clear();

        SavePropertyTypes(cat);

        // check for any meta images
        const ImageInfoMaps* imgmaps=m_view->GetMeta().GetVertexImageInfo(cat);
        if(imgmaps != NULL)
        {
            for(ImageInfoMaps::const_iterator itrmap=imgmaps->begin(); itrmap != imgmaps->end(); ++itrmap)
            {
                for(MapImageInfo::const_iterator itrimg =itrmap->second.begin(); itrimg != itrmap->second.end(); ++itrimg)
                {
                    if (itrimg->second.image != 0)
                    {
                        boost::scoped_ptr<CBitmap> cbmp(itrimg->second.image->GetCBitmap());
                        CComQIPtr<IPicture> ipic=GetIPictureFromBitmap((HBITMAP)(*cbmp));
                        ATLASSERT(ipic != NULL);
                        CSize size;
                        itrimg->second.GetSize(size);
                        SaveImage(ipic, itrimg->second.pos, size);
                    }
                }
            }
        }
        
        SaveColor(L"Normal", m_view->GetMeta().GetMetaVertexColorFillNorm(cat), m_view->GetMeta().GetMetaVertexFontColor(cat), m_view->GetMeta().GetMetaVertexColorBorderNorm(cat));
        SaveColor(L"Selected", m_view->GetMeta().GetMetaVertexColorFillSel(cat), m_view->GetMeta().GetMetaVertexFontColorSel(cat), m_view->GetMeta().GetMetaVertexColorBorderSel(cat));
        SaveColor(L"Hover", m_view->GetMeta().GetMetaVertexColorFillOver(cat), 0/*m_view->GetMeta().GetMetaVertexFontColorOver(cat)*/, m_view->GetMeta().GetMetaVertexColorBorderOver(cat));
        
        // </Meta>
        m_pContentHandler->endElement(OLESTR(""), 0, OLESTR(""), 0, metaTag, wcslen(metaTag));
    }

    // </EntityType>
    m_pContentHandler->endElement(OLESTR(""), 0, OLESTR(""), 0, L"EntityType", wcslen(L"EntityType"));
}

void CXmlAnbSerializer::SaveSubgraphEntityType(const CCategory& cat, const TCHAR* icon, bool saveExtraMeta)
{
    std::_tstring xmlCategory;
#ifdef _DEBUG
    bool bmetadefined=
#endif
    SubgraphCategoryToString(cat, xmlCategory);
#ifdef _DEBUG
    ATLASSERT(bmetadefined);
#endif

    m_pMXAttrs->clear();
    // <EntityType>
    m_pMXAttrs->addAttribute(bstr_t(OLESTR("")), bstr_t(OLESTR("")), bstr_t(L"Name"), bstr_t(OLESTR("")), bstr_t(xmlCategory.c_str()));
    m_pMXAttrs->addAttribute(bstr_t(OLESTR("")), bstr_t(OLESTR("")), bstr_t(L"IconFile"), bstr_t(OLESTR("")), bstr_t(icon));
    m_pMXAttrs->addAttribute(bstr_t(OLESTR("")), bstr_t(OLESTR("")), bstr_t(L"GraphItemType"), bstr_t(OLESTR("")), bstr_t(L"Subgraph"));
    m_pContentHandler->startElement(OLESTR(""), 0, OLESTR(""), 0, L"EntityType", wcslen(L"EntityType"), m_pSAXAttrs);
    m_pMXAttrs->clear();

    if (saveExtraMeta)
    {
        // <Meta>
        const BSTR metaTag = L"Meta";
        m_pMXAttrs->addAttribute(bstr_t(OLESTR("")), bstr_t(OLESTR("")), bstr_t(L"Id"), bstr_t(OLESTR("")), bstr_t(variant_t(cat.GetCategory())));
        std::_tstring label;
        if (_tcslen(m_view->GetMeta().GetMetaSubgraphLabel(cat, label)))
            m_pMXAttrs->addAttribute(bstr_t(OLESTR("")), bstr_t(OLESTR("")), bstr_t(L"Label"), bstr_t(OLESTR("")), bstr_t(label.c_str()));
        if (m_view->GetMeta().GetMetaSubgraphHide(cat) == VISIBLE_HIDE)
            m_pMXAttrs->addAttribute(bstr_t(OLESTR("")), bstr_t(OLESTR("")), bstr_t(L"Deselected"), bstr_t(OLESTR("")), bstr_t(OLESTR("-1")));
        m_pContentHandler->startElement(OLESTR(""), 0, OLESTR(""), 0, metaTag, wcslen(metaTag), m_pSAXAttrs);
        m_pMXAttrs->clear();

        SavePropertyTypes(cat);

        // check for any meta images
        const ImageInfoMaps* imgmaps=m_view->GetMeta().GetSubgraphImageInfo(cat);
        if(imgmaps != NULL)
        {
            for(ImageInfoMaps::const_iterator itrmap=imgmaps->begin(); itrmap != imgmaps->end(); ++itrmap)
            {
                for(MapImageInfo::const_iterator itrimg =itrmap->second.begin(); itrimg != itrmap->second.end(); ++itrimg)
                {
                    if (itrimg->second.image != 0)
                    {
                        boost::scoped_ptr<CBitmap> cbmp(itrimg->second.image->GetCBitmap());
                        CComQIPtr<IPicture> ipic=GetIPictureFromBitmap((HBITMAP)(*cbmp));
                        ATLASSERT(ipic != NULL);
                        CSize size;
                        itrimg->second.GetSize(size);
                        SaveImage(ipic, itrimg->second.pos, size);
                    }
                }
            }
        }
        
        SaveColor(L"Normal", m_view->GetMeta().GetMetaSubgraphColorFillNorm(cat), m_view->GetMeta().GetMetaSubgraphFontColor(cat), m_view->GetMeta().GetMetaSubgraphColorBorderNorm(cat));
        SaveColor(L"Selected", m_view->GetMeta().GetMetaSubgraphColorFillSel(cat), m_view->GetMeta().GetMetaSubgraphFontColorSel(cat), m_view->GetMeta().GetMetaSubgraphColorBorderSel(cat));
        SaveColor(L"Hover", m_view->GetMeta().GetMetaSubgraphColorFillOver(cat), 0/*m_view->GetMeta().GetMetaVertexFontColorOver(cat)*/, m_view->GetMeta().GetMetaSubgraphColorBorderOver(cat));
        
        // </Meta>
        m_pContentHandler->endElement(OLESTR(""), 0, OLESTR(""), 0, metaTag, wcslen(metaTag));
    }

    // </EntityType>
    m_pContentHandler->endElement(OLESTR(""), 0, OLESTR(""), 0, L"EntityType", wcslen(L"EntityType"));
}

void CXmlAnbSerializer::SavePropertyTypes(const CCategory& cat)
{
    std::pair<CategoryPropertyMap::const_iterator, CategoryPropertyMap::const_iterator> itrPair = m_view->GetMeta().GetCategoryPropertyAttrIterator(META_VERTEX, cat);

    // if iterator equals end of list
    if (itrPair.first != itrPair.second)
    {
        CategoryPropertyMap::const_iterator itr = itrPair.first;

        // <PropertyTypeCollection>
        m_pContentHandler->startElement(OLESTR(""), 0, OLESTR(""), 0, L"PropertyTypeCollection", wcslen(L"PropertyTypeCollection"), m_pSAXAttrs);

        PropAttrMap::const_iterator itrProps;
        // iterate all properties.
        for (itrProps = itr->second.begin(); itrProps != itr->second.end(); ++itrProps)
        {
            const PropAttributes& pa = itrProps->second;
            PropAttributes::AttrValVector::const_iterator itrv;	
            // iterate all property attributes.
            for (itrv = pa.avec.begin(); itrv != pa.avec.end(); ++itrv)
            {
                bstr_t val(itrv->value);
                if (val.length())
                {
                    m_pMXAttrs->addAttribute(bstr_t(OLESTR("")), bstr_t(OLESTR("")), bstr_t(L"Id"), bstr_t(OLESTR("")), bstr_t(variant_t(itrProps->first)));
                    m_pMXAttrs->addAttribute(bstr_t(OLESTR("")), bstr_t(OLESTR("")), bstr_t(L"Attr"), bstr_t(OLESTR("")), bstr_t(variant_t(itrv->attr)));
                    m_pMXAttrs->addAttribute(bstr_t(OLESTR("")), bstr_t(OLESTR("")), bstr_t(L"Value"), bstr_t(OLESTR("")), val);
                    // <PropertyType>
                    m_pContentHandler->startElement(OLESTR(""), 0, OLESTR(""), 0, L"PropertyType", wcslen(L"PropertyType"), m_pSAXAttrs);
                    m_pMXAttrs->clear();
                    // </PropertyType>
                    m_pContentHandler->endElement(OLESTR(""), 0, OLESTR(""), 0, L"PropertyType", wcslen(L"PropertyType"));
                }
            }
        }

        // </PropertyTypeCollection>
        m_pContentHandler->endElement(OLESTR(""), 0, OLESTR(""), 0, L"PropertyTypeCollection", wcslen(L"PropertyTypeCollection"));
    }
}

void CXmlAnbSerializer::SaveColor(const BSTR name, COLORREF fill, COLORREF font, COLORREF border)
{
    // <Color Name="xxx" Fill=xxx Font=xxx Border=xxx>
    const BSTR colorTag = L"Color";
    m_pMXAttrs->addAttribute(bstr_t(OLESTR("")), bstr_t(OLESTR("")), bstr_t(L"Name"), bstr_t(OLESTR("")), bstr_t(name));
    if (fill != 0)
        m_pMXAttrs->addAttribute(bstr_t(OLESTR("")), bstr_t(OLESTR("")), bstr_t(L"Fill"), bstr_t(OLESTR("")), bstr_t(variant_t(fill)));
    if (font != 0)
        m_pMXAttrs->addAttribute(bstr_t(OLESTR("")), bstr_t(OLESTR("")), bstr_t(L"Font"), bstr_t(OLESTR("")), bstr_t(variant_t(font)));
    if (border != 0)
        m_pMXAttrs->addAttribute(bstr_t(OLESTR("")), bstr_t(OLESTR("")), bstr_t(L"Border"), bstr_t(OLESTR("")), bstr_t(variant_t(border)));
    m_pContentHandler->startElement(OLESTR(""), 0, OLESTR(""), 0, colorTag, wcslen(colorTag), m_pSAXAttrs);
    m_pMXAttrs->clear();
    // </Color>
    m_pContentHandler->endElement(OLESTR(""), 0, OLESTR(""), 0, colorTag, wcslen(colorTag));
}

void CXmlAnbSerializer::SaveLinkType(const CCategory& cat, bool saveExtraMeta)
{
    variant_t color = m_view->GetMeta().GetMetaEdgeColorNorm(cat);
    color.ulVal &= 0x00FFFFFF; // remove alpha value
    color.ulVal = GraphTypes::ToBGR(color.ulVal); // convert to ANB BGR format
    color.ChangeType(VT_BSTR);

    std::_tstring xmlCategory;
#ifdef _DEBUG
    bool metaDefined = 
#endif
    EdgeCategoryToString(cat, xmlCategory);
#ifdef _DEBUG
    ATLASSERT(metaDefined);
#endif

    m_pMXAttrs->clear();
    // <LinkType>
    m_pMXAttrs->addAttribute(bstr_t(OLESTR("")), bstr_t(OLESTR("")), bstr_t(L"Name"), bstr_t(OLESTR("")), bstr_t(xmlCategory.c_str()));
    // Please note deliberate english spelling to be compatible with Analyst's Notebook.
    m_pMXAttrs->addAttribute(bstr_t(OLESTR("")), bstr_t(OLESTR("")), bstr_t(L"Colour"), bstr_t(OLESTR("")), color.bstrVal);
    m_pContentHandler->startElement(OLESTR(""), 0, OLESTR(""), 0, L"LinkType", wcslen(L"LinkType"), m_pSAXAttrs);
    m_pMXAttrs->clear();

    if (saveExtraMeta)
    {
        // <Meta>
        m_pMXAttrs->addAttribute(bstr_t(OLESTR("")), bstr_t(OLESTR("")), bstr_t(L"Id"), bstr_t(OLESTR("")), bstr_t(variant_t(cat.GetCategory())));
        std::_tstring label;
        if (_tcslen(m_view->GetMeta().GetMetaEdgeLabel(cat, label)))
            m_pMXAttrs->addAttribute(bstr_t(OLESTR("")), bstr_t(OLESTR("")), bstr_t(L"Label"), bstr_t(OLESTR("")), bstr_t(label.c_str()));
        m_pContentHandler->startElement(OLESTR(""), 0, OLESTR(""), 0, L"Meta", wcslen(L"Meta"), m_pSAXAttrs);
        m_pMXAttrs->clear();
        // </Meta>
        m_pContentHandler->endElement(OLESTR(""), 0, OLESTR(""), 0, L"Meta", wcslen(L"Meta"));
    }

    // </LinkType>
    m_pContentHandler->endElement(OLESTR(""), 0, OLESTR(""), 0, L"LinkType", wcslen(L"LinkType"));
}
