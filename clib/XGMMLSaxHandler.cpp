#include "StdAfx.h"

#include "XGMMLSaxHandler.h"
#include "GraphContainerImpl.h"

//  ===========================================================================
const TCHAR * LocalizeNumber(std::_tstring & prettyCount)
{
#define MAX_STR 1024
    TCHAR buff[MAX_STR];
    if (GetNumberFormat(LOCALE_USER_DEFAULT, 0, prettyCount.c_str(), NULL, buff, MAX_STR) > 0)
    {
        prettyCount = buff;
        if (boost::algorithm::contains(prettyCount, _T(".")))
        {
            boost::algorithm::trim_right_if(prettyCount, boost::algorithm::is_any_of("0")); 
            boost::algorithm::trim_right_if(prettyCount, boost::algorithm::is_any_of(".")); 
        }
    }
    return prettyCount.c_str();
}

const std::_tstring & constGet(const Element::StringStringMap & map, const std::_tstring & key, const std::_tstring & defVal  = _T(""))
{
    Element::StringStringMap::const_iterator found = map.find(key);
    if (found == map.end())
        return defVal;
    return found->second;
}

void CStringStringMap::MergeFrom(const CStringStringMap & other)
{
    for(Element::StringStringMap::const_iterator itr = other.m_attr.begin(); itr != other.m_attr.end(); ++itr)
    {
        m_attr[itr->first] = itr->second;
    }
}

void MergeCStringStringMap(IGraphItem * item, const Element & e)
{
    CComQIPtr<CStringStringMap> attrMap = item->GetPropertyUnknown(PROP_ATTRIBUTES);
    if (!attrMap)
        attrMap = new CStringStringMap();
    for(Element::StringStringMap::const_iterator itr = e.m_attr.begin(); itr != e.m_attr.end(); ++itr)
        attrMap->m_attr[itr->first] = itr->second;
    attrMap->m_attr[_T("_HashID")] = boost::lexical_cast<std::_tstring>(item->GetID().GetHash());
    item->SetPropertyUnknown(PROP_ATTRIBUTES, attrMap);
}

IGraphEdge * GetEdge(IGraph* graph, const CUniqueID & id, const CUniqueID & fromID, const CUniqueID & toID, bool merge, bool appendMissing)
{
    IGraphEdge * retVal = NULL;
    if (merge)
        retVal = graph->GetEdge(id);
    if (!retVal && appendMissing)
        retVal = graph->CreateEdge(id, fromID, toID);
    ATLASSERT((appendMissing && retVal) || !appendMissing);
    return retVal;
}

IGraphVertex * GetVertex(IGraph* graph, IGraphSubgraph* subgraph, const CUniqueID & id, bool merge, bool appendMissing)
{
    IGraphVertex * retVal = NULL;
    if (merge)
        retVal = graph->GetVertex(id);
    if (!retVal && subgraph && appendMissing)
        retVal = subgraph->CreateVertex(id);
    ATLASSERT((appendMissing && retVal) || !appendMissing);
    return retVal;
}

IGraphSubgraph * GetSubgraph(IGraph* graph, IGraphSubgraph* subgraph, const CUniqueID & id, const std::_tstring & label, bool merge, bool appendMissing)
{
    IGraphSubgraph * retVal = NULL;
    if (merge)
        retVal = graph->GetSubgraph(id);
    if (!retVal && subgraph && appendMissing)
        retVal = subgraph->CreateSubgraph(id, label.c_str());
    ATLASSERT((appendMissing && retVal) || !appendMissing);
    return retVal;
}
//  ===========================================================================
LazyCreateStack::LazyCreateStack(IGraph * graph, bool merge, bool appendMissing)
{
    m_graph = graph;
    m_merge = merge;
    m_appendMissing = appendMissing;
}

IGraphSubgraph * LazyCreateStack::Get(unsigned int depth)
{
    ATLASSERT(m_stack.size() >= depth + 1);
    const std::_tstring * id = &m_stack._Get_container()[m_stack.size() - depth - 1].first;
    IGraphSubgraphAdapt * subgraph = (IGraphSubgraphAdapt *)&m_stack._Get_container()[m_stack.size() - depth - 1].second;
    if (!*subgraph)
    {
        *subgraph = GetSubgraph(m_graph, Get(depth + 1), CUniqueID(guidDefault, XGMML_CAT_SUBGRAPH, *id), *id, m_merge, m_appendMissing);
    }
    ATLASSERT(*subgraph);
    return *subgraph;
}

void LazyCreateStack::push(const std::_tstring & id, IGraphSubgraph * subgraph)
{
    m_stack.push(std::make_pair(id, subgraph));
}

void LazyCreateStack::pop()
{
    m_stack.pop();
}

IGraphSubgraph * LazyCreateStack::top()
{
    return Get(0);
}

bool LazyCreateStack::topIsVertex()
{
    return (m_stack.top().second.get() == NULL);
}
//  ===========================================================================
GraphViewXGMMLHandler::GraphViewXGMMLHandler(IGraph* graph, const IXGMMLRenderCallback * callback, bool merge, bool appendMissing, bool redundantSubgraphs, bool passthroughVertices) : m_graph(graph), m_callback(callback), m_merge(merge), m_appendMissing(appendMissing), m_redundantSubgraphs(redundantSubgraphs), m_passthroughVertices(passthroughVertices), m_stackSubgraph(graph, merge, appendMissing)
{
    _ASSERTE(m_graph != NULL);
    ATLASSERT(m_callback);
    m_stackSubgraph.push(graph->GetID().GetID(), graph);
}

HRESULT STDMETHODCALLTYPE GraphViewXGMMLHandler::endDocument()
    {
#define NDUPEDGEHACK
#ifdef DUPEDGEHACK
        static int dup_edge_id_hack = 0;
#endif
        for(EdgeMap::iterator itr = m_edges.begin(); itr != m_edges.end(); ++itr)
        {
#ifdef DUPEDGEHACK
            std::_tstring id = (boost::_tformat(_T("%1%_%2%")) % itr->first.get<0>() % ++dup_edge_id_hack).str();
#else
            std::_tstring id = itr->first.get<0>();
#endif
            std::_tstring source = itr->first.get<1>();
            std::_tstring target = itr->first.get<2>();
            bool dotted = itr->first.get<3>();
            int slaves = itr->first.get<4>();
            int started = itr->first.get<5>();
            int stopped = itr->first.get<6>();
            bool loose =  itr->first.get<7>();
            const Element & e = itr->second;

            CUniqueID fromID(guidDefault, XGMML_CAT_VERTEX, source.c_str());
            CUniqueID toID(guidDefault, XGMML_CAT_VERTEX, target.c_str());

            std::_tstring prettyCount = constGet(e.m_attr, _T("count"));
            if (prettyCount.length() && 
                    (!boost::algorithm::equals(prettyCount, _T("0")) || 
                    (boost::algorithm::equals(prettyCount, _T("0")) && boost::algorithm::equals(constGet(e.m_attr, _T("_eofSeen")), _T("1"))) || 
                    (boost::algorithm::equals(prettyCount, _T("0")) && boost::algorithm::equals(constGet(e.m_attr, _T("started")), _T("1")))))
            {
                LocalizeNumber(prettyCount);
            }
            else
                prettyCount = _T("");

            std::_tstring edgeLabel;
            if (constGet(e.m_attr, _T("label")).length() && prettyCount.length() && constGet(e.m_attr, _T("maxskew")).length() && constGet(e.m_attr, _T("minskew")).length())
                edgeLabel = (boost::_tformat(_T("%1%\n%2%\n+%3%%%, -%4%%%")) % constGet(e.m_attr, _T("label")) % prettyCount % constGet(e.m_attr, _T("maxskew")) % constGet(e.m_attr, _T("minskew"))).str();
            else if (prettyCount.length() && constGet(e.m_attr, _T("maxskew")).length() && constGet(e.m_attr, _T("minskew")).length())
                edgeLabel = (boost::_tformat(_T("%1%\n+%2%%%, -%3%%%")) % prettyCount % constGet(e.m_attr, _T("maxskew")) % constGet(e.m_attr, _T("minskew"))).str();
            else if (constGet(e.m_attr, _T("label")).length() && prettyCount.length())
                edgeLabel = (boost::_tformat(_T("%1%\n%2%")) % constGet(e.m_attr, _T("label")) % prettyCount).str();
            else if (prettyCount.length())
                edgeLabel = (boost::_tformat(_T("%1%")) % prettyCount).str();
            else if (constGet(e.m_attr, _T("label")).length())
                edgeLabel = (boost::_tformat(_T("%1%")) % constGet(e.m_attr, _T("label"))).str();

            IGraphEdge * edge = GetEdge(m_graph, CUniqueID(guidDefault, XGMML_CAT_EDGE, id), fromID, toID, m_merge, m_appendMissing);
            if (edge)
            {
                edge->SetLabel(edgeLabel.c_str());
                edge->SetProperty(PROP_EDGE_DOTTED, dotted);
                edge->SetProperty(PROP_EDGE_SLAVES, slaves);
                if (started > 0)
                    edge->PropagateProperty(PROP_EDGE_STARTED, started);
                edge->SetProperty(PROP_EDGE_STOPPED, stopped);
                int _targetIndex = 0;
                try {
                    _targetIndex = boost::lexical_cast<int>(constGet(e.m_attr, _T("_targetIndex"), _T("0")));
                }catch(boost::bad_lexical_cast &){}
                edge->SetProperty(PROP_EDGE_TARGETINDEX, _targetIndex);
                if (boost::algorithm::iequals(constGet(e.m_attr, _T("_roxieStarted")), _T("1")))
                    edge->PropagateProperty(PROP_ROXIESTARTED, true);
                edge->SetProperty(PROP_EDGE_LOOSE, loose);
                MergeCStringStringMap(edge, e);
                //std::_tstring tooltip;
                //edge->SetProperty(PROP_TOOLTIP, CreateTooltip(edgeLabel, e, tooltip));
                ATLASSERT(m_callback);
                if (m_callback)
                    edge->SetPropertyUnknown(PROP_CALLBACK, const_cast<IXGMMLRenderCallback *>(m_callback));
            }
        }

        if (!m_merge)
        {
            bool foundTrivialSubgraph = true;
            while(foundTrivialSubgraph)
            {
                foundTrivialSubgraph = false;
                IGraphSubgraphVector subgraphs;
                m_graph->GetSubgraphs(subgraphs);
                for(IGraphSubgraphVector::iterator itr = subgraphs.begin(); itr != subgraphs.end(); ++itr)
                {
                    if (CComQIPtr<IGraph> graph = itr->get()->GetParent())
                        continue;

                    IGraphItemVector items;
                    itr->get()->GetParent()->GetChildren(items);
                    if (items.size() == 1)
                    {
                        foundTrivialSubgraph = true;
                        itr->get()->Delete(true);
                    }
                }
            }

            //if (!m_passthroughVertices)
            //{
            //	bool foundPassThrough = true;
            //	while(foundPassThrough)
            //	{
            //		foundPassThrough = false;
            //		for(CComPtr<IVertexIterator> itr = m_graph->GetVertexIterator(); itr->IsValidItem(); itr->NextItem())
            //		{
            //			CComVariant isPassthrough;
            //			IGraphVertex * v = itr->Get();
            //			ATLASSERT(v);
            //			if (v)
            //				v->GetProperty(PROP_VERTEX_ISPASSTHROUGH, isPassthrough);
            //			if (isPassthrough.bVal)
            //			{
            //				foundPassThrough = true;
            //				CUniqueID vID = itr->Get()->GetID();
            //				IGraphEdgeVector inEdges, outEdges;
            //				itr->Get()->GetAdjacentInEdges(inEdges);
            //				itr->Get()->GetAdjacentOutEdges(outEdges);
            //				if (inEdges.size() == 1 && outEdges.size() == 1)
            //				{
            //					CUniqueID eID = inEdges[0]->GetID();
            //					CUniqueID vsID = inEdges[0]->GetSource()->GetID();
            //					CUniqueID vtID = outEdges[0]->GetTarget()->GetID();
            //					PropertyMap props;
            //					PropertyUnknownMap uprops;
            //					inEdges[0]->CopyProperties(props);
            //					inEdges[0]->CopyPropertyUnknowns(uprops);
            //					inEdges[0]->Delete();
            //					outEdges[0]->Delete();
            //					itr->Get()->Delete();
            //					CComPtr<IGraphEdge> newEdge = m_graph->CreateEdge(eID, vsID, vtID);
            //					newEdge->PasteProperties(props);
            //					newEdge->PastePropertyUnknowns(uprops);
            //				}
            //				else
            //				{
            //					ATLASSERT(!"Does this ever happen?");
            //					foundPassThrough = false;
            //				}
            //				break;
            //			}
            //		}
            //	}
            //}

            //for(CComPtr<IVertexIterator> itr = m_graph->GetVertexIterator(); itr->IsValidItem(); itr->NextItem())
            //{
            //	IGraphEdgeVector inEdges;
            //	itr->Get()->GetAdjacentInEdges(inEdges);
            //	int size = 0;
            //	for(IGraphEdgeVector::iterator itr = inEdges.begin(); itr != inEdges.end(); ++itr)
            //	{
            //		CComVariant dotted;
            //		itr->get()->GetProperty(PROP_EDGE_DOTTED, dotted);
            //		if (!dotted.bVal)
            //			++size;
            //			
            //	}
            //	if (inEdges.size() <= 1 || inEdges.size() > 5)
            //		continue;

            //	for(IGraphEdgeVector::iterator itr = inEdges.begin(); itr != inEdges.end(); ++itr)
            //	{
            //		CComVariant dotted;
            //		itr->get()->GetProperty(PROP_EDGE_DOTTED, dotted);
            //		if (dotted.bVal)
            //			continue;
            //		CComVariant targetIndex;
            //		itr->get()->GetProperty(PROP_EDGE_TARGETINDEX, targetIndex);
            //		switch (size)
            //		{
            //		case 2:
            //			if (targetIndex.intVal == 0)
            //				itr->get()->SetProperty(PROP_EDGE_COMPASS, _T("nw"));
            //			else if (targetIndex.intVal == 1)
            //				itr->get()->SetProperty(PROP_EDGE_COMPASS, _T("ne"));
            //			break;
            //		case 3:
            //			if (targetIndex.intVal == 0)
            //				itr->get()->SetProperty(PROP_EDGE_COMPASS, _T("nw"));
            //			else if (targetIndex.intVal == 1)
            //				itr->get()->SetProperty(PROP_EDGE_COMPASS, _T("n"));
            //			else if (targetIndex.intVal == 2)
            //				itr->get()->SetProperty(PROP_EDGE_COMPASS, _T("ne"));
            //			break;
            //		case 4:
            //			if (targetIndex.intVal == 0)
            //				itr->get()->SetProperty(PROP_EDGE_COMPASS, _T("w"));
            //			else if (targetIndex.intVal == 1)
            //				itr->get()->SetProperty(PROP_EDGE_COMPASS, _T("nw"));
            //			else if (targetIndex.intVal == 2)
            //				itr->get()->SetProperty(PROP_EDGE_COMPASS, _T("ne"));
            //			else if (targetIndex.intVal == 3)
            //				itr->get()->SetProperty(PROP_EDGE_COMPASS, _T("e"));
            //			break;
            //		case 5:
            //			if (targetIndex.intVal == 0)
            //				itr->get()->SetProperty(PROP_EDGE_COMPASS, _T("w"));
            //			else if (targetIndex.intVal == 1)
            //				itr->get()->SetProperty(PROP_EDGE_COMPASS, _T("nw"));
            //			else if (targetIndex.intVal == 2)
            //				itr->get()->SetProperty(PROP_EDGE_COMPASS, _T("n"));
            //			else if (targetIndex.intVal == 3)
            //				itr->get()->SetProperty(PROP_EDGE_COMPASS, _T("ne"));
            //			else if (targetIndex.intVal == 4)
            //				itr->get()->SetProperty(PROP_EDGE_COMPASS, _T("e"));
            //			break;
            //		}
            //	}
            //}
        }
        return __super::endDocument();
    }

HRESULT STDMETHODCALLTYPE GraphViewXGMMLHandler::startElement(
        const wchar_t *pwchNamespaceUri,
        int cchNamespaceUri,
        const wchar_t *pwchLocalName,
        int cchLocalName,
        const wchar_t *pwchQName,
        int cchQName,
        ISAXAttributes *pAttributes)
    { 
        __super::startElement(pwchNamespaceUri, cchNamespaceUri, pwchLocalName, cchLocalName, pwchQName, cchQName, pAttributes);

        const Element & e = m_stack.top();
        std::_tstring id = constGet(e.m_attr, _T("id"));
        if (e.m_tag.compare(_T("Graph")) == 0)
        {
            m_stackSubgraph.push(id);
        }
        else if (e.m_tag.compare(_T("node")) == 0)
        {
            m_stackSubgraph.push(id);
        }

        return S_OK; 
    }
HRESULT STDMETHODCALLTYPE GraphViewXGMMLHandler::endElement( 
        const wchar_t *pwchNamespaceUri,
        int cchNamespaceUri,
        const wchar_t *pwchLocalName,
        int cchLocalName,
        const wchar_t *pwchQName,
        int cchQName)
    { 
        const Element & e = m_stack.top();

        if (e.m_tag.compare(_T("edge")) == 0)
        {
            bool dotted = false;
            int slaves = 0;
            int started = 0;
            int stopped = 0;
            bool loose = false;
            Element::StringStringMap::const_iterator itrDependsOn = e.m_attr.find(_T("_dependsOn"));
            if (itrDependsOn != e.m_attr.end())
                dotted = boost::algorithm::iequals(itrDependsOn->second, _T("1"));

            Element::StringStringMap::const_iterator itrChildGraph = e.m_attr.find(_T("_childGraph"));
            if (itrChildGraph != e.m_attr.end())
                dotted = dotted || boost::algorithm::iequals(itrChildGraph->second, _T("1"));

            Element::StringStringMap::const_iterator itrSlaves = e.m_attr.find(_T("slaves"));
            if (itrSlaves != e.m_attr.end())
                slaves = boost::lexical_cast<int>(itrSlaves->second);

            Element::StringStringMap::const_iterator itrStarted = e.m_attr.find(_T("started"));
            if (itrStarted != e.m_attr.end())
                started = boost::lexical_cast<int>(itrStarted->second);

            Element::StringStringMap::const_iterator itrStopped = e.m_attr.find(_T("stopped"));
            if (itrStopped != e.m_attr.end())
                stopped = boost::lexical_cast<int>(itrStopped->second);

            Element::StringStringMap::const_iterator itrSource = e.m_attr.find(_T("_sourceActivity"));
            Element::StringStringMap::const_iterator itrTarget = e.m_attr.find(_T("_targetActivity"));

            if (itrSource == e.m_attr.end() || itrTarget == e.m_attr.end())
            {
                itrSource = e.m_attr.find(_T("source"));
                itrTarget = e.m_attr.find(_T("target"));
            }
            else
                loose = true;

            if (itrSource == e.m_attr.end() || itrTarget == e.m_attr.end())
            {
                ATLASSERT(!"Missing edge info!");
            }
            else
            {
                std::_tstring id;
                Element::StringStringMap::const_iterator itrID = e.m_attr.find(_T("id"));
                if (itrID != e.m_attr.end())
                    id = itrID->second;
                if (id.length() == 0)
                    id = itrSource->second + _T("_") + itrTarget->second + _T("_madeup");
                m_edges[EdgeTuple(id, itrSource->second, itrTarget->second, dotted, slaves, started, stopped, loose)] = e;
            }
        }
        else if (e.m_tag.compare(_T("node")) == 0)
        {
            // nodes without the label are Subgraphs
            std::_tstring label = constGet(e.m_attr, _T("label"));
            std::_tstring shortLabel = constGet(e.m_attr, _T("shortlabel"));
            std::_tstring id = constGet(e.m_attr, _T("id"));
            if (!m_stackSubgraph.topIsVertex())
            {
                IGraphSubgraph * subgraph = m_stackSubgraph.top();
                if(boost::algorithm::equals(subgraph->GetID().GetID(), id))
                {
                    ATLASSERT(m_callback);
                    if (m_callback)
                        subgraph->SetPropertyUnknown(PROP_CALLBACK, const_cast<IXGMMLRenderCallback *>(m_callback));
                    if (label.size())
                        subgraph->SetLabel(label.c_str());
                    m_stackSubgraph.pop();
                }
            }
            else
            {
                m_stackSubgraph.pop();
                IGraphVertex * vertex = GetVertex(m_graph, m_stackSubgraph.top(), CUniqueID(guidDefault, XGMML_CAT_VERTEX, id), m_merge, m_appendMissing);
                if (vertex)
                {
                    if (!m_merge)
                        vertex->SetLabel(label.size() ? label.c_str() : shortLabel.c_str());
                    vertex->SetProperty(PROP_VERTEX_KIND, constGet(e.m_attr, _T("_kind")).c_str());
                    vertex->SetProperty(PROP_VERTEX_ISPASSTHROUGH, constGet(e.m_attr, _T("_isPassthrough")).length() > 0);
                    if (constGet(e.m_attr, _T("definition")).size())
                        vertex->SetProperty(PROP_VERTEX_DEFINITION, constGet(e.m_attr, _T("definition")).c_str());
                    MergeCStringStringMap(vertex, e);
                    //std::_tstring tooltip;
                    //vertex->SetProperty(PROP_TOOLTIP, CreateTooltip(label, e, tooltip));
                    ATLASSERT(m_callback);
                    if (m_callback)
                        vertex->SetPropertyUnknown(PROP_CALLBACK, const_cast<IXGMMLRenderCallback *>(m_callback));
                }
            }
        }
        else if (e.m_tag.compare(_T("Graph")) == 0)
        {
            IGraphSubgraph * subgraph = m_stackSubgraph.top();
            std::_tstring id = constGet(e.m_attr, _T("id"));
            if(boost::algorithm::equals(subgraph->GetID().GetID(), id))
            {
                ATLASSERT(m_callback);
                if (m_callback)
                    subgraph->SetPropertyUnknown(PROP_CALLBACK, const_cast<IXGMMLRenderCallback *>(m_callback));
                std::_tstring label = constGet(e.m_attr, _T("label"));
                if (label.size())
                    subgraph->SetLabel(label.c_str());
                m_stackSubgraph.pop();
            }
        }
        else if (e.m_tag.compare(_T("att")) == 0)
        {
            ATLASSERT(m_stack.size() >= 2);
            Element::StringStringMap::const_iterator itrName = e.m_attr.find(_T("name"));
            if (itrName != e.m_attr.end())
            {
                Element& e_parent = const_cast<Element &>(m_stack._Get_container()[m_stack.size() - 2]);
                e_parent.m_attr[itrName->second] = constGet(e.m_attr, _T("value"));
            }
        }

        return __super::endElement(pwchNamespaceUri, cchNamespaceUri, pwchLocalName, cchLocalName, pwchQName, cchQName);
    }
//  ==========================================================================
//class CWriteStreamOnString : public IWriteStream
//{
//
//public:
//	std::string m_str;
//
//	virtual ~CWriteStreamOnString()
//	{
//	}
//
//	HRESULT WriteStream(LPCSTR szOut, int nLen, LPDWORD pdwWritten)
//	{
//		ATLENSURE_RETURN( szOut != NULL );
//
//		if (nLen < 0)
//		{
//			nLen = (int) strlen(szOut);
//		}
//
//		_ATLTRY
//		{
//			m_str.append(szOut, nLen);
//		}
//		_ATLCATCHALL()
//		{
//			return E_OUTOFMEMORY;
//		}
//
//		if (pdwWritten != NULL)
//		{
//			*pdwWritten = (DWORD) nLen;
//		}
//
//		return S_OK;
//	}
//
//	HRESULT FlushStream()
//	{
//		return S_OK;
//	}
//
//	void Cleanup()
//	{
//		m_str.clear();
//	}
//}; // class CWriteStreamOnCString
//

class CHtmlGen
{
protected:
    std::_tstring & m_html;

public:
    CHtmlGen(std::_tstring & html) : m_html(html)
    {
    }

    void table(int border, const std::_tstring & attr)
    {
        m_html += (boost::_tformat(_T("<table %1% border=\"%2%\">")) % attr % border).str();
    }

    void table(int border = 0)
    {
        m_html += (boost::_tformat(_T("<table border=\"%1%\">")) % border).str();
    }

    void tableEnd()
    {
        m_html += _T("</table>");
    }

    void tr()
    {
        m_html += _T("<tr>");
    }

    void trEnd()
    {
        m_html += _T("</tr>");
    }

    void hr()
    {
        m_html += _T("<hr>");
    }

    void td()
    {
        m_html += _T("<td>");
    }

    void td(const std::_tstring & body)
    {
        m_html += (boost::_tformat(_T("<td>%1%")) % body).str();
    }

    void td(int dunno, const std::_tstring & attr)
    {
        m_html += (boost::_tformat(_T("<td %1%>")) % attr).str();
    }

    void td(const std::_tstring & body, const std::_tstring & attr)
    {
        m_html += (boost::_tformat(_T("<td %1%>%2%")) % attr % body).str();
    }

    void tdEnd()
    {
        m_html += _T("</td>");
    }

    void StartTag(const std::_tstring & tag, const std::_tstring & body, const std::_tstring & attr)
    {
        m_html += (boost::_tformat(_T("<%1% %2%>%3%</%1%>")) %tag % attr % body).str();
    }

    void b(const std::_tstring & body)
    {
        m_html += (boost::_tformat(_T("<b>%1%</b>")) % body).str();
    }

    void br()
    {
        m_html += _T("<br>");
    }

    void code(const std::_tstring & body)
    {
        m_html += (boost::_tformat(_T("<code>%1%</code>")) % body).str();
    }
};

//  ==========================================================================
const TCHAR * CreateTextTooltip(IGraphItem * edge, CStringStringMap * attrs, std::_tstring & tooltip)
{
    for (Element::StringStringMap::const_iterator itr = attrs->m_attr.begin(); itr != attrs->m_attr.end(); ++itr)
    {
#ifndef _DEBUG
        if (!boost::algorithm::starts_with(itr->first, _T("_")))
            continue;
#endif
        std::_tstring second = itr->second;
        boost::algorithm::trim_if(second, boost::algorithm::is_any_of("\r\n"));
        boost::algorithm::replace_all(second, _T("\n"), _T(" - "));
        tooltip += (boost::_tformat(_T("%1%\t%2%\r\n")) % itr->first % second).str();
    }
    return tooltip.c_str();
}

const TCHAR * CreateHTMLTooltip(IGraphEdge * edge, CStringStringMap * attrs, std::_tstring & tooltip)
{
    Element::StringStringMap known, unknown;
    for (Element::StringStringMap::const_iterator itr = attrs->m_attr.begin(); itr != attrs->m_attr.end(); ++itr)
    {
        if (boost::algorithm::iequals(itr->first, _T("id")))
            known[itr->first] = itr->second;
        else if (boost::algorithm::iequals(itr->first, _T("count")))
            known[itr->first] = itr->second;
        else if (boost::algorithm::iequals(itr->first, _T("max")))
            known[itr->first] = itr->second;
        else if (boost::algorithm::iequals(itr->first, _T("maxEndpoint")))
            known[itr->first] = itr->second;
        else if (boost::algorithm::iequals(itr->first, _T("maxSkew")))
            known[itr->first] = itr->second;
        else if (boost::algorithm::iequals(itr->first, _T("min")))
            known[itr->first] = itr->second;
        else if (boost::algorithm::iequals(itr->first, _T("minEndpoint")))
            known[itr->first] = itr->second;
        else if (boost::algorithm::iequals(itr->first, _T("minSkew")))
            known[itr->first] = itr->second;
        else if (boost::algorithm::iequals(itr->first, _T("slaves")))
            known[itr->first] = itr->second;
        else if (boost::algorithm::iequals(itr->first, _T("started")))
            known[itr->first] = itr->second;
        else if (boost::algorithm::iequals(itr->first, _T("stopped")))
            known[itr->first] = itr->second;
        else if (boost::algorithm::iequals(itr->first, _T("source")))
            known[itr->first] = itr->second;
        else if (boost::algorithm::iequals(itr->first, _T("target")))
            known[itr->first] = itr->second;
        else
        {
#ifndef _DEBUG
            if (!boost::algorithm::starts_with(itr->first, _T("_")))
#endif
                unknown[itr->first] = itr->second;
        }
    }

    CHtmlGen html(tooltip);

    if (attrs)
    {
        html.table(0, _T("align=\"center\""));
        html.tr();
        html.td(0, _T("border=\"1\""));
        std::_tstring hashId = boost::lexical_cast<std::_tstring>(edge->GetSource()->GetID().GetHash());
        html.StartTag(_T("a"), edge->GetSource()->GetLabel(), (boost::_tformat(_T("msg=%1%")) % hashId).str().c_str());
        html.tdEnd();
        html.trEnd();
        html.tr();
        html.td(_T("|"));
        html.tdEnd();
        html.trEnd();
        html.tr();
        html.td();
        html.b(known[_T("id")].c_str());
        html.tdEnd();
        html.trEnd();
        html.tr();
        html.td(_T("|"));
        html.tdEnd();
        html.trEnd();
        html.tr();
        html.td(_T("V"));
        html.tdEnd();
        html.trEnd();
        html.tr();
        html.td(0, _T("border=\"1\""));
        hashId = boost::lexical_cast<std::_tstring>(edge->GetTarget()->GetID().GetHash());
        html.StartTag(_T("a"), edge->GetTarget()->GetLabel(), (boost::_tformat(_T("msg=%1%")) % hashId).str().c_str());
        html.tdEnd();
        html.trEnd();
        html.tableEnd();

        if (known[_T("count")].size())
        {
            html.br();
            html.table(0);
            html.tr();
            html.td();
            html.b(_T("Count:"));
            html.tdEnd();
            html.td(LocalizeNumber(known[_T("count")]), _T("align=\"right\""));
            html.tdEnd();
            html.trEnd();
            html.tableEnd();
        }

        if (known[_T("max")].size())
        {
            html.br();
            html.table(0);
            html.tr();
            html.td(_T("   "));
            html.tdEnd();
            html.td();
            html.b(_T("Skew"));
            html.tdEnd();
            html.td();
            html.b(_T("Node"));
            html.tdEnd();
            html.td();
            html.b(_T("Rows"));
            html.tdEnd();
            html.trEnd();

            html.tr();
            html.td();
            html.b(_T("Max"));
            html.tdEnd();
            html.td((boost::_tformat(_T("+%1%%%")) % known[_T("maxskew")]).str().c_str());
            html.tdEnd();
            html.td(known[_T("maxEndpoint")].c_str());
            html.tdEnd();
            html.td(LocalizeNumber(known[_T("max")]));
            html.tdEnd();
            html.trEnd();

            html.tr();
            html.td();
            html.b(_T("Min"));
            html.tdEnd();
            html.td((boost::_tformat(_T("-%1%%%")) % known[_T("minskew")]).str().c_str());
            html.tdEnd();
            html.td(known[_T("minEndpoint")].c_str());
            html.tdEnd();
            html.td(LocalizeNumber(known[_T("min")]));
            html.tdEnd();
            html.trEnd();
            html.tableEnd();
        }

        if (known[_T("slaves")].size())
        {
            html.br();
            html.table(0);
            html.tr();
            html.td();
            html.b(_T("Slaves"));
            html.tdEnd();
            html.td();
            html.b(_T("Started"));
            html.tdEnd();
            html.td();
            html.b(_T("Stopped"));
            html.tdEnd();
            html.trEnd();

            html.tr();
            html.td(known[_T("slaves")].c_str());
            html.tdEnd();
            html.td(known[_T("started")].c_str());
            html.tdEnd();
            html.td(known[_T("stopped")].c_str());
            html.tdEnd();
            html.trEnd();
            html.tableEnd();
        }

        if (unknown.size())
        {
            html.hr();
            html.table();
            for (Element::StringStringMap::const_iterator itr = unknown.begin(); itr != unknown.end(); ++itr)
            {
                html.tr();
                html.td();
                html.b(itr->first.c_str());
                html.tdEnd();
                html.td();
                html.code(itr->second.c_str());
                html.tdEnd();
                html.trEnd();
            }
            html.tableEnd();
        }
    }

#ifdef _DEBUG
    SetClipboard(tooltip.c_str());
#endif
    return tooltip.c_str();
}

const TCHAR * CreateHTMLTooltip(IGraphVertex * vertex, CStringStringMap * attrs, std::_tstring & tooltip)
{
    if (!attrs)
        return tooltip.c_str();

    Element::StringStringMap known, unknown;
    for (Element::StringStringMap::const_iterator itr = attrs->m_attr.begin(); itr != attrs->m_attr.end(); ++itr)
    {
        if (boost::algorithm::iequals(itr->first, _T("definition")))
            known[itr->first] = itr->second;
        else
        {
#ifndef _DEBUG
            if (!boost::algorithm::starts_with(itr->first, _T("_")))
#endif
                unknown[itr->first] = itr->second;
        }
    }

    CHtmlGen html(tooltip);
    html.table(0, _T("align=\"center\""));
    html.tr();
    html.td(vertex->GetLabel(), _T("border=\"1\""));
    html.tdEnd();
    html.trEnd();
    html.tableEnd();

    if (known[_T("definition")].size())
    {
        html.br();
        html.table();
        html.tr();
        html.td();
        html.b(_T("Definition"));
        html.tdEnd();
        html.td();
        html.StartTag(_T("a"), known[_T("definition")].c_str(), (boost::_tformat(_T("msg=%1%")) % known[_T("definition")]).str().c_str());
        html.tdEnd();
        html.trEnd();
        html.tableEnd();
    }

    if (unknown.size())
    {
        html.hr();
        html.table();
        for (Element::StringStringMap::const_iterator itr = unknown.begin(); itr != unknown.end(); ++itr)
        {
#ifndef _DEBUG
            if (boost::algorithm::starts_with(itr->first, _T("_")))
                continue;
#endif
            html.tr();
            html.td();
            html.b(itr->first.c_str());
            html.tdEnd();
            html.td();
            html.code(itr->second.c_str());
            html.tdEnd();
            html.trEnd();
        }
        html.tableEnd();
    }

#ifdef _DEBUG
    SetClipboard(tooltip.c_str());
#endif
    return tooltip.c_str();
}

CLIB_API const TCHAR * CreateTooltip(IGraphEdge * edge, CStringStringMap * attrs, std::_tstring & tooltip, bool textOnly)
{
    if (textOnly)
        return CreateTextTooltip(edge, attrs, tooltip);
    return CreateHTMLTooltip(edge, attrs, tooltip);
}

CLIB_API const TCHAR * CreateTooltip(IGraphVertex * vertex, CStringStringMap * attrs, std::_tstring & tooltip, bool textOnly)
{
    if (textOnly)
        return CreateTextTooltip(vertex, attrs, tooltip);
    return CreateHTMLTooltip(vertex, attrs, tooltip);
}
//  ==========================================================================
