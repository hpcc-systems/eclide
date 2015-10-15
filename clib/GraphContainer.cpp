#include "StdAfx.h"

#include "GraphContainerImpl.h"
#include "XGMMLSaxHandler.h"
#include "Logger.h"

template<typename GraphType>
bool CGraphDataT<typename GraphType>::Load(const std::_tstring & xgmml, const IXGMMLRenderCallback * callback, bool redundantSubgraphs, bool passthroughVertices)
{
    CComInitialize com;
    CComPtr<ISAXXMLReader> pRdr; 
    HRESULT hr = pRdr.CoCreateInstance(CLSID_SAXXMLReader30);
    CComPtr <GraphViewXGMMLHandler> pMc = new GraphViewXGMMLHandler(this, callback, false, true, redundantSubgraphs, passthroughVertices);
    hr = pRdr->putContentHandler(pMc); 
    CComPtr<SAXErrorHandler> pErr = new SAXErrorHandler();
    if(SUCCEEDED(hr))
        pRdr->putErrorHandler(pErr);

    if (hr == S_OK)
    {
        hr = pRdr->parse(CComVariant(xgmml.c_str()));
        if (SUCCEEDED(hr))
        {
            return true;
        }
    }
    return false;
}

template<typename GraphType>
bool CGraphDataT<typename GraphType>::Merge(const std::_tstring & xgmml, const IXGMMLRenderCallback * callback, bool appendMissing, bool redundantSubgraphs, bool passthroughVertices)
{
    CComInitialize com;
    CComPtr<ISAXXMLReader> pRdr; 
    HRESULT hr = pRdr.CoCreateInstance(CLSID_SAXXMLReader30);
    CComPtr <GraphViewXGMMLHandler> pMc = new GraphViewXGMMLHandler(this, callback, true, appendMissing, redundantSubgraphs, passthroughVertices);
    hr = pRdr->putContentHandler(pMc); 
    CComPtr<SAXErrorHandler> pErr = new SAXErrorHandler();
    if(SUCCEEDED(hr))
        pRdr->putErrorHandler(pErr);

    if (hr == S_OK)
    {
        hr = pRdr->parse(CComVariant(xgmml.c_str()));
        if (SUCCEEDED(hr))
        {
            return true;
        }
    }
    return false;
}
// ============================================================================
IGraph * CreateDirectedGraph()
{
    return new CDirectedGraphData();
}

IGraph * CreateDirectedGraph(ISkipEdge * edgeFilter, ISkipVertex * vertexFilter, ISkipSubgraph * subgraphFilter)
{
    return new CDirectedGraphData(edgeFilter, vertexFilter, subgraphFilter);
}

IGraph * CreateUndirectedGraph()
{
    return new CUndirectedGraphData();
}
// ============================================================================
