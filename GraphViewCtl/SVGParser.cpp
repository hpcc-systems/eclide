#include "StdAfx.h"

#include "SVGParser.h"
#include "XGMMLSaxHandler.h"

CGraphATTSVGHandler::CGraphATTSVGHandler(IGraph* graph, CCanvas* canvas, CMeta * meta, NEATO_MODE mode) 
                    :m_graph(graph), m_canvas(canvas), m_mode(mode), m_meta(meta), m_dpi(72)
{
    _ASSERTE(m_graph != NULL);
    m_valid = false;
}
CGraphATTSVGHandler::~CGraphATTSVGHandler()
{
    int d = 0;
}
//	---------------------------------------------------------------------------
void CGraphATTSVGHandler::setBoundingBox(IGraphItem* item)
{
    ATLASSERT(item);
    if (!item)
        return;

    CComQIPtr<IRenderer> renderer = m_canvas->GetRenderer(item);
    if (renderer) {
        ATLASSERT(renderer);

        PointList::iterator itr = m_points.begin();
        if (itr != m_points.end()) {
            GraphTypes::REAL left	= itr->first;
            GraphTypes::REAL right	= itr->first;
            GraphTypes::REAL top		= itr->second;
            GraphTypes::REAL bottom	= itr->second;
            for (itr++; itr != m_points.end(); itr++) {
                left	= std::min<GraphTypes::REAL>(left,itr->first);
                right	= std::max<GraphTypes::REAL>(right,itr->first);
                top		= std::min<GraphTypes::REAL>(top,itr->second);
                bottom	= std::max<GraphTypes::REAL>(bottom,itr->second);
            }

            GraphTypes::RectF rectF;
            rectF.Width  = (right  - left) * m_canvas->m_dpiXbase / m_dpi;
            rectF.Height = (bottom - top) * m_canvas->m_dpiYbase / m_dpi;
            rectF.x = (left) * m_canvas->m_dpiXbase / m_dpi;
            rectF.y = (top) * m_canvas->m_dpiYbase / m_dpi;

            if(m_mode==NEATO_MODE_SUBGRAPH_TREE)
            {
                GraphTypes::PointF pt(rectF.x, rectF.y); 
                renderer->SetPosition(pt, GraphTypes::screen);
            }
            else if (m_meta->GetMetaSplines())
            {
                renderer->SetBoundsBox(rectF, SWP_NOREDRAW);
            }
            else
            {
                //GraphTypes::PointF pt(rectF.x + rectF.Width / 2, rectF.y + rectF.Height / 2);
                GraphTypes::PointF pt(rectF.x, rectF.y); 
                renderer->SetPosition(pt);
            }
        }
    }
}
//	---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE CGraphATTSVGHandler::endDocument()
{
    return S_OK;
}
//	---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE CGraphATTSVGHandler::startElement( 
    const wchar_t *pwchNamespaceUri,
    int cchNamespaceUri,
    const wchar_t *pwchLocalName,
    int cchLocalName,
    const wchar_t *pwchQName,
    int cchQName,
    ISAXAttributes *pAttributes)
{ 
    __super::startElement(pwchNamespaceUri, cchNamespaceUri, pwchLocalName, cchLocalName, pwchQName, cchQName, pAttributes);

    Element &e = m_stack.top();

    if (0 == e.m_tag.compare(_T("svg"))) 
    {
        m_valid = true;
        return S_OK; 
    }

    if (m_valid == true && 0 == e.m_tag.compare(_T("g"))) 
    {
        m_points.clear();
        m_title.clear();
        m_text.clear();
        m_subgraph = NULL;
        m_vertex = NULL;
        m_edge = NULL;

        if (0 == e.m_attr[_T("class")].compare(_T("graph")))
        {
            m_class = CGraphATTSVGHandler::graph;
        }
        else
        {
            unsigned int hash = 0;
            try
            {
                hash = boost::lexical_cast<unsigned int>(e.m_attr[_T("id")]);
            }
            catch(boost::bad_lexical_cast &)
            {
                //  Could be a dummy node to force cluster generation.
            }
            if (hash == 0)
                m_class = CGraphATTSVGHandler::dummy_node;
            else if (0 == e.m_attr[_T("class")].compare(_T("cluster")))
            {
                m_class = CGraphATTSVGHandler::cluster;
                m_subgraph = m_graph->GetSubgraph(hash);//CUniqueID(guidDefault, XGMML_CAT_SUBGRAPH, e.m_attr[_T("id")]));
                ATLASSERT(m_subgraph);
                m_text_count = 0;
                m_text_x = 0;
                m_text_y = 0;
            }
            else if (0 == e.m_attr[_T("class")].compare(_T("node")))
            {
                switch(m_canvas->GetApplicationMode())
                {
                case AppModeGrETL:
                    if (m_mode == NEATO_MODE_EDGES)
                    {
                        m_class = CGraphATTSVGHandler::node;
                        m_vertex = m_graph->GetVertex(hash);
                        ATLASSERT(m_vertex);
                    }
                    else
                    {
                        m_class = CGraphATTSVGHandler::cluster;
                        m_subgraph = m_graph->GetSubgraph(hash);//CUniqueID(guidDefault, XGMML_CAT_SUBGRAPH, e.m_attr[_T("id")]));
                        ATLASSERT(m_subgraph);
                    }
                    break;
                default:
                    m_class = CGraphATTSVGHandler::node;
                    m_vertex = m_graph->GetVertex(hash);
                    ATLASSERT(m_vertex);
                    break;
                }
                m_text_count = 0;
                m_text_x = 0;
                m_text_y = 0;
            }
            else if (0 == e.m_attr[_T("class")].compare(_T("edge")))
            {
                m_class = CGraphATTSVGHandler::edge;
                m_edge = m_graph->GetEdge(hash);//CUniqueID(guidDefault, XGMML_CAT_EDGE, e.m_attr[_T("id")]));
                ATLASSERT(m_edge);
                m_text_count = 0;
                m_text_x = 0;
                m_text_y = 0;
            }
            else
                m_class = CGraphATTSVGHandler::unknown;
        }
    }
    return S_OK; 
}
//	---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE CGraphATTSVGHandler::endElement( 
    const wchar_t *pwchNamespaceUri,
    int cchNamespaceUri,
    const wchar_t *pwchLocalName,
    int cchLocalName,
    const wchar_t *pwchQName,
    int cchQName)
{ 
    Element &e = m_stack.top();

    if (m_valid == true) {
        if (0 == e.m_tag.compare(_T("g"))) 
        {
            if (m_subgraph)
            {
                ATLASSERT(m_mode!=NEATO_MODE_SUBGRAPH_TREE);
                setBoundingBox(m_subgraph);
            }	
            else if(m_vertex)
            {
                setBoundingBox(m_vertex);
            }
            else if(m_edge)
            {
                // we don't care about edge points in this mode
                if(m_mode!=NEATO_MODE_SUBGRAPH_TREE)
                {
                    CComQIPtr<IEdgeRenderer> renderer = m_canvas->GetRenderer(m_edge);
                    if (renderer) {
                        if (m_text_count)
                        {
                            GraphTypes::PointF pointF (m_text_x / m_text_count, m_text_y / m_text_count);
                            pointF.x = (pointF.x * m_canvas->m_dpiXbase) / m_dpi;
                            pointF.y = (pointF.y * m_canvas->m_dpiYbase) / m_dpi;
                            renderer->SetLabelPos(GraphTypes::RectF(pointF, GraphTypes::SizeF()), GraphTypes::screen);
                        }
                        GraphTypes::PointFVector epts;
                        if (m_meta->GetMetaSplines()) {
                            for (PointList::iterator itr = m_points.begin(); itr != m_points.end(); ++itr)
                            {
                                GraphTypes::PointF pointF (itr->first, itr->second);
                                pointF.x = (pointF.x * m_canvas->m_dpiXbase) / m_dpi;
                                pointF.y = (pointF.y * m_canvas->m_dpiYbase) / m_dpi;
                                epts.push_back(pointF);
                            }
                            renderer->SetPoints(epts, GraphTypes::screen);
                        }
                        else
                        {
                            renderer->SetPoints(epts, GraphTypes::screen);
                        }
                    }
                }
            }
        }
        else if (0 == e.m_tag.compare(_T("title"))) {
            m_title = e.GetContent();
        }
        else if (0 == e.m_tag.compare(_T("text")))  {
            m_text  = e.GetContent();
            try
            {
                m_text_x += boost::lexical_cast<GraphTypes::REAL>(e.m_attr[_T("x")]);
                m_text_y += boost::lexical_cast<GraphTypes::REAL>(e.m_attr[_T("y")]);
                ++m_text_count;
            }
            catch (boost::bad_lexical_cast &)
            {
                m_text_x = 0;
                m_text_y = 0;
            }
        }
        else { 
            std::string content;

            if (0 == e.m_tag.compare(_T("polygon"))) {
                content = CT2A(e.m_attr[_T("points")].c_str());
            }
            else if (0 == e.m_tag.compare(_T("path"))) {
                content = CT2A(e.m_attr[_T("d")].c_str());
            }

            if (content.length()) {
                boost::char_separator<char>  separators ("AaCcHhLlMmQqSsTtVvZz, ");	// svg codes & separators
                boost::tokenizer<boost::char_separator<char> > tokens(content,separators);
                boost::tokenizer<boost::char_separator<char> >::iterator itr = tokens.begin();

                while (itr != tokens.end()) {
                    try
                    {
                        float x = boost::lexical_cast<float>((*itr++));
                        float y = boost::lexical_cast<float>((*itr++));
                        m_points.push_back(std::pair<GraphTypes::REAL, GraphTypes::REAL>(x, y));
                    }
                    catch (boost::bad_lexical_cast &)
                    {
                    }
                }
            }
        }
    }

    return __super::endElement(pwchNamespaceUri, cchNamespaceUri, pwchLocalName, cchLocalName, pwchQName, cchQName);
}
//	---------------------------------------------------------------------------

