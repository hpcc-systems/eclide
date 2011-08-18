#include "stdafx.h"
#include "resource.h"

#include "neato.h"
#include "cmdProcess.h"
#include "global.h"
#include "meta.h"
#include "Canvas.h"
#include "util.h"

#include "libagraphsp.h"
#include "unicodefile.h"

#include "SVGParser.h"
#include "XGMMLSaxHandler.h"

#define SPLINE_OFFSET_X 50
#define SPLINE_OFFSET_Y 50

using namespace GraphTypes;

typedef boost::tuples::tuple<GraphTypes::REAL, GraphTypes::REAL> REAL2Tuple;
typedef std::map<REAL2Tuple, GraphTypes::PointF> MapP2P;

static const char cluster_keyword[] = "cluster";

bool ShortestRoute(
	AttVisG::NPolygon& polys,
	const PointF& src,
	const PointF& dst,
	AttVisG::Polygon& route );

class CGraphItemCompare
{
protected:
	bool m_reverseSubgraphs;
public:
	CGraphItemCompare(bool reverseSubgraphs)
	{
		m_reverseSubgraphs = reverseSubgraphs;
	}

	bool operator ()(const IGraphItemAdapt & l, const IGraphItemAdapt & r)
	{
		const CUniqueID & l_id = l->GetID();
		const CUniqueID & r_id = r->GetID();
		if (l_id.GetCategory() == r_id.GetCategory())
		{
			try
			{
				int l_id_int = boost::lexical_cast<int>(l_id.GetID());
				int r_id_int = boost::lexical_cast<int>(r_id.GetID());
				return l_id_int < r_id_int;
			}
			catch(boost::bad_lexical_cast &)
			{
			}
			return _tcscmp(l_id.GetID(), r_id.GetID()) < 0;
		}
		else
			return l->GetCategory() < r->GetCategory();

		return l->GetID() < r->GetID();
	}
};

class CGraphEdgeCompare
{
public:
	bool operator ()(const IGraphEdgeAdapt & l, const IGraphEdgeAdapt & r)
	{
		CGraphItemCompare compare(false);
		if (l->GetTarget()->GetID() == r->GetTarget()->GetID())
		{
			CComVariant lTargetIndex, rTargetIndex;
			l->GetProperty(PROP_EDGE_TARGETINDEX, lTargetIndex);
			r->GetProperty(PROP_EDGE_TARGETINDEX, rTargetIndex);
			return lTargetIndex > rTargetIndex;
		}
		return compare(r->GetTarget(), l->GetTarget());
		//CComVariant lDotted, rDotted;
		//l->GetProperty(PROP_EDGE_DOTTED, lDotted);
		//r->GetProperty(PROP_EDGE_DOTTED, rDotted);
		//if (!lDotted.bVal && rDotted.bVal)
		//	return true;
		//if (lDotted.bVal && !rDotted.bVal)
		//	return false;
		//typedef std::vector<std::_tstring> split_vector_type;
		//split_vector_type lhsSplit, rhsSplit; 
		//boost::algorithm::split(lhsSplit, std::_tstring(l->GetID().GetID()), boost::algorithm::is_any_of(_T("_")));
		//boost::algorithm::split(rhsSplit, std::_tstring(r->GetID().GetID()), boost::algorithm::is_any_of(_T("_")));
		//try
		//{
		//	int lhsInt = boost::lexical_cast<int>(lhsSplit[0]);
		//	int rhsInt = boost::lexical_cast<int>(rhsSplit[0]);
		//	if (lhsInt == rhsInt)
		//	{
		//		lhsInt = 0;
		//		rhsInt = 0;
		//		if (lhsSplit.size() > 1)
		//			lhsInt = boost::lexical_cast<int>(lhsSplit[1]);
		//		if (rhsSplit.size() > 1)
		//			rhsInt = boost::lexical_cast<int>(rhsSplit[1]);
		//	}
		//	if (lhsInt < rhsInt)
		//		return true;
		//	else if (lhsInt > rhsInt)
		//		return false;
		//}
		//catch(boost::bad_lexical_cast &) 
		//{
		//}
		//return false;
	}
};
//	---------------------------------------------------------------------------

std::string & CGraphATTLayoutImpl::EncodeForDOT(const TCHAR *csv, std::string & encoded)
{
	encoded = CT2A(csv, CP_UTF8);
	boost::algorithm::trim_if(encoded, boost::algorithm::is_any_of("\r\n"));
	boost::algorithm::replace_all(encoded, "\"", "\\\"");
	boost::algorithm::replace_all(encoded, "\r", "");
	boost::algorithm::replace_all(encoded, "\n", "\\n");
#ifdef _DEBUG
#define NOBFUSCATE
#ifdef _OBFUSCATE
	for(int i = 0; i < encoded.length(); ++i)
	{
		if (encoded[i] >= 48 && encoded[i] <= 57)
		{
			encoded[i] = _T('3');
		}
		else if (encoded[i] >= 65 && encoded[i] <= 90)
		{
			encoded[i] = _T('Q');
		}
		else if (encoded[i] >= 97 && encoded[i] <= 122)
		{
			encoded[i] = _T('r');
		}
		else if (encoded[i] == 10 || encoded[i] == 13)
		{
			//ATLASSERT(false);
			//encoded[i] = _T('Q');
		}
		else if (_istalnum(encoded[i]))
		{
			ATLTRACE("Known char:  %s (%i)\n", encoded[i], i);
			//ATLASSERT(false);
			encoded[i] = _T('Q');
		}
		else
		{
			ATLTRACE("Unknown char:  %i\n", encoded[i]);
			encoded[i] = _T('?');
			//ATLASSERT(false);
		}
	}
#endif
#endif
	//encoded = " " + encoded + " ";
	return encoded;
}
//	---------------------------------------------------------------------------
CGraphATTLayoutImpl::CGraphATTLayoutImpl(CGraphViewCanvas * view, IGraph * graph, CCanvas* canvas, CMeta & meta, NEATO_MODE mode) 
					: m_view(view), CLayout(graph, meta, canvas), m_mode(mode), m_canvas(canvas)
{
}
//	---------------------------------------------------------------------------
void CGraphATTLayoutImpl::AppendFont(IGraphItem* item, std::stringstream& dotStream)
{
	CComPtr<IRenderer> unknown = m_canvas->GetRenderer(item);
	if (unknown) 
	{
		dotStream << "fontname=\"Verdana\"";
		dotStream << ", fontsize=" << m_canvas->GetFontPointSize()+2 << "";	
	}
}

void CGraphATTLayoutImpl::AppendPos(IGraphItem* item, std::stringstream& dotStream, bool prependComma)
{
	CComPtr<IRenderer> renderer = m_canvas->GetRenderer(item);
	if (renderer) 
	{
		bool isPinned=false;

		CComQIPtr<ILegacyRenderer> lr = renderer;
		if(lr!=0)
		{
			isPinned=lr->GetPinned();
		}
		else
		{
			CComQIPtr<ILegacySubgraphRenderer> lsgr = renderer;
			if(lsgr)
				isPinned=lsgr->GetPinned();
		}

		if (prependComma)
		{
			dotStream << ", ";
		}
		GraphTypes::RectF box;
		renderer->GetBoundsBox(box);
		dotStream << "pos=\"" << boost::lexical_cast<std::string>(box.x) << ", " << 
			boost::lexical_cast<std::string>(-1 * box.y) << "\"";
		//(isPinned ? "!\"" : "\"");
		if (isPinned)
		{
			dotStream << ", pin=true";
		}
	}
}

void CGraphATTLayoutImpl::AppendSize(IGraphItem* item, std::stringstream& dotStream, bool prependComma)
{
	CComQIPtr<IRenderer> renderer = m_canvas->GetRenderer(item);
	if (renderer) 
	{
		GraphTypes::RectF box;
		if (CComQIPtr<ILegacyRenderer> legacy_renderer = renderer)
		{
			GraphTypes::PointF origon(0.0f, 0.0f);
			GraphTypes::RectF title_box, image_box;
			legacy_renderer->GetBoundsBox(title_box);
			title_box.SetLocation(&origon);
			legacy_renderer->GetImagesBoundsBox(image_box);
			image_box.SetLocation(&origon);
			box.Union(box, title_box, image_box);
		}
		else if (CComQIPtr<ILegacySubgraphRenderer> legacy_renderer = renderer)
		{
			renderer->GetBoundsBox(box);
		}
		else if (CComQIPtr<IAttVertexRenderer> legacy_renderer = renderer)
		{
			GraphTypes::PointF origon(0.0f, 0.0f);
			GraphTypes::RectF title_box, image_box;
			legacy_renderer->GetBoundsBox(title_box);
			title_box.SetLocation(&origon);
			legacy_renderer->GetImagesBoundsBox(image_box);
			image_box.SetLocation(&origon);
			box.Union(box, title_box, image_box);
		}
		else if(CComQIPtr<IDatasetRenderer> dataset_renderer = renderer)
		{
			renderer->GetBoundsBox(box);
		}
		else if(CComQIPtr<ITextboxRenderer> dataset_renderer = renderer)
		{
			renderer->GetBoundsBox(box);
		}
		else
		{
			return; // don't do this for AT&T graph items
		}

		if (prependComma)
		{
			dotStream << ", ";
		}

		dotStream << "width=\"" << boost::lexical_cast<std::string>(box.Width / m_canvas->m_dpiYbase) << "\"";
		dotStream << ", height=\"" << boost::lexical_cast<std::string>(box.Height / m_canvas->m_dpiYbase) << "\"";
	}
}

void CGraphATTLayoutImpl::foreach(IGraphItem* item, std::stringstream& dotStream, unsigned int depth)
{
	if(CComQIPtr<IGraph> graph = item)
	{
		GraphTypes::RectF extent = m_canvas->getGraphExtent(false, world);
		GraphTypes::SizeF size;
		extent.GetSize(&size);
		size.Width = (REAL)std::min<int>((int)size.Width, 800);
		size.Height = (REAL)std::min<int>((int)size.Height, 600);

		dotStream << "graph 1 {" << std::endl;
		dotStream << "graph [id=\"A426B88A-D072-4a40-A5B2-E45D3C212335\", label=\"\"]" << std::endl;
		dotStream << "graph [";
		AppendFont(graph, dotStream);
//		AppendSize(graph, dotStream);
//		dotStream << ", dpi=96";
		dotStream << ", dpi=" << m_canvas->m_dpiYbase << "";
//		dotStream << ", ratio=\"expand\"";
//		dotStream << ", size=\"" << size.Width / m_canvas->m_dpiXbase << ", " << size.Height / m_canvas->m_dpiYbase << "\"";//"!" means desired size
		if (m_meta.GetMetaSplines() && !m_bigHack)
			dotStream << ", splines=true";
		else
			dotStream << ", splines=false";
		switch(m_mode)
		{
		case NEATO_MODE_NEATO:
			dotStream << ", sep=0.00"; 
			dotStream << ", overlap=scale";
			break;
		case NEATO_MODE_FDP:
			dotStream << ", sep=0.00"; 
			dotStream << ", start=random";
			break;
		case NEATO_MODE_TWOPI:
			dotStream << ", sep=0.00"; 
			//dotStream << ", overlap=scalexy";
			dotStream << ", overlap=scale";
			break;
		case NEATO_MODE_DOT:
		case NEATO_MODE_SUBGRAPH_TREE:
			if (m_meta.GetMetaDotLeftRight())
			{
				dotStream << ", rankdir=LR";
			}
			//dotStream << ", ordering=in, compound=true";
			dotStream << ", compound=true";
			break;
		case NEATO_MODE_CIRCO:
			dotStream << ", sep=0.00"; 
			dotStream << ", overlap=compress";
			break;
		default:
			break;
		}
		dotStream << "];" << std::endl;	

		dotStream << "node [";
		AppendFont(graph, dotStream);
		dotStream << ", shape=box";
		if (m_meta.GetMetaSplines())
		{
//			dotStream << ", margin=\"0.2,0.2\"";
		}
		else
		{
			dotStream << ", fixedsize=true";
		}

		dotStream << "];" << std::endl;	

		dotStream << "edge [";
		AppendFont(graph, dotStream);
		dotStream << "];" << std::endl;	

		//  Put edges at start sorted.
		std::_tstring label;
		std::map<unsigned,IGraphVertex*> dupes;

		IGraphEdgeVector edges;
		graph->GetEdges(edges);
		std::sort(edges.begin(), edges.end(), CGraphEdgeCompare());
		for (IGraphEdgeVector::iterator itr = edges.begin(); itr != edges.end(); ++itr)
		{
			IGraphEdge * edge = *itr;
			if (m_meta.IsVisible(edge))
			{
				CComPtr<IGraphVertex> s=edge->GetSource();
				CComPtr<IGraphVertex> t=edge->GetTarget();
				ATLASSERT(s && t);
				bool addedge=true;
				CComQIPtr<IAttEdgeRenderer> er=m_canvas->GetRenderer(edge);
				CComQIPtr<IEdgeRenderer> er2=m_canvas->GetRenderer(edge);
				if(m_mode==NEATO_MODE_SUBGRAPH_TREE || er!=0 || er2!=0)
				{
					CComQIPtr<IGraphSubgraph> ss = edge->GetPropertyUnknown(PROP_EDGE_VISIBLESOURCE);
					CComQIPtr<IGraphSubgraph> st = edge->GetPropertyUnknown(PROP_EDGE_VISIBLETARGET);

					dotStream << static_cast<unsigned int>(ss ? ss->GetID().GetHash() : s->GetID().GetHash());
					dotStream << " -- ";
					dotStream << static_cast<unsigned int>(st ? st->GetID().GetHash() : t->GetID().GetHash());
					dotStream << " [id=\"" << edge->GetID().GetHash() << "\"";
					if(ss || st)
					{
						if (ss)
							dotStream << " ltail=\"" << cluster_keyword << ss->GetID().GetHash() << "\"";
						if (st)
							dotStream << " lhead=\"" << cluster_keyword << st->GetID().GetHash() << "\"";
					}
					else
					{
					//	CComVariant loose;
					//	edge->GetProperty(PROP_EDGE_LOOSE, loose);
					//	if (loose.vt == VT_BOOL && loose.bVal)
					//		dotStream << " weight=\"0.9\"";
					}
				}
				else
				{
					CComPtr<IGraphSubgraph> ss=s->GetParent();
					CComPtr<IGraphSubgraph> st=t->GetParent();
					bool sinsg=false, tinsg=false;

					unsigned shash, thash;
					if(ss->GetID()!=graph->GetID()) // edge for vertex that is not in the root graph
					{
						sinsg=true;
						// see if the other side is in the map
						if(dupes.find(t->GetID().GetHash())==dupes.end())
						{
							dupes[t->GetID().GetHash()]=t;
							shash=ss->GetID().GetHash();
						}
						else
							addedge=false;
					}
					else
						shash=s->GetID().GetHash();

					if(addedge && st->GetID()!=graph->GetID()) // edge for vertex that is not in the root graph
					{
						tinsg=true;
						// see if the other side is in the map
						if(dupes.find(s->GetID().GetHash())==dupes.end())
						{
							dupes[s->GetID().GetHash()]=s;
							thash=st->GetID().GetHash();
						}
						else
							addedge=false;
					}
					else
						thash=t->GetID().GetHash();
					
					if(sinsg && tinsg) // skip edges connecting vertices inside subgraph 
						addedge=false; 

					if(addedge)
					{
						dotStream << shash;
						dotStream << " -- ";
						dotStream << thash;
						dotStream << " [id=\"" << edge->GetID().GetHash() << "\"";
					}
				}

				if (m_meta.IsEdgeLabelVisible())
				{
					std::string encoded;
					EncodeForDOT(edge->GetLabel(), encoded);
					if (encoded.size())
						dotStream << " label=\"" << encoded << "\"";
				}
				CComVariant compass;
				edge->GetProperty(PROP_EDGE_COMPASS, compass);
				CString _compass = compass;
				if (addedge && !_compass.IsEmpty())
					dotStream << " headport=\"" << CT2A(_compass) << "\"";
				if(addedge)
					dotStream << "];" << std::endl;
			}
		}
	}
	else if(CComQIPtr<IGraphSubgraph> subgraph = item)
	{
		if (m_meta.IsVisible(subgraph))
		{
			CComQIPtr<IAttSubgraphRenderer> iar=m_canvas->GetRenderer(subgraph); 
			CComQIPtr<ILegacySubgraphRenderer> ilr=m_canvas->GetRenderer(subgraph);
			CComQIPtr<IDatasetRenderer> idr=m_canvas->GetRenderer(subgraph);
			bool min=false;
			if(iar!=0)
			{
				min=iar->IsMinimized();
				if (!min)
				{
					IGraphItemVector children;
					CComPtr<MetaStateSkipper> skipper = new MetaStateSkipper(m_meta, m_canvas, TRISTATE_TRUE);
					subgraph->GetChildren(children, skipper);
					if (children.empty())
						min = true;
				}
			}
			else if(ilr!=0 || idr!=0)
				min=true; // for layout purposes legacy subgraph is always considered minimized

			dotStream << "subgraph " << cluster_keyword << subgraph->GetID().GetHash() << " {" << std::endl;
			dotStream << "graph";
			dotStream << " [id=\"" << subgraph->GetID().GetHash() << "\"";

			std::string encoded;
			EncodeForDOT(subgraph->GetLabel(), encoded);
			dotStream << " label=\"" << encoded << "\"";
			if(ilr || idr)
				AppendPos(subgraph, dotStream);
			AppendSize(subgraph, dotStream);
			dotStream << "];" << std::endl;

			if(min)	// min or empty needs an empty item to force generation of cluster.
				dotStream << static_cast<unsigned int>(subgraph->GetID().GetHash()) << " [shape=point label=\"\"];" << std::endl;
		}
	}
	else if(CComQIPtr<IGraphVertex> vertex = item)
	{
		if(m_mode!=NEATO_MODE_SUBGRAPH_TREE)
		{
			if (vertex && m_meta.IsVisible(vertex)) 
			{
				dotStream << static_cast<unsigned int>(vertex->GetID().GetHash());
				dotStream << " [id=\"" << vertex->GetID().GetHash() << "\"";

				std::string encoded;
				EncodeForDOT(vertex->GetLabel(), encoded);
				dotStream << " label=\"" << encoded << "\"";
				if (!m_meta.GetMetaSplines())
				{
					AppendPos(vertex, dotStream);
					AppendSize(vertex, dotStream);
				}
				dotStream << "];" << std::endl;
			}
		}
	}

	if (CComQIPtr<IGraphSubgraph> subgraph = item)	//Graph && Subgraph included.
	{
		if (m_meta.IsVisible(subgraph))
		{
			CComQIPtr<IAttSubgraphRenderer> iar=m_canvas->GetRenderer(subgraph); 
			CComQIPtr<ILegacySubgraphRenderer> ilr=m_canvas->GetRenderer(subgraph);
			bool min=false;
			if(iar!=0)
				min=iar->IsMinimized();
			else if(ilr!=0)
				min=true; // for layout purposes legacy subgraph is always considered minimized
			// don't deal with children for minimized AT&T or legacy subgraph
			if(!min)
			{
				IGraphItemVector children;
				CComPtr<MetaStateSkipper> skipper = new MetaStateSkipper(m_meta, m_canvas, TRISTATE_TRUE);
				subgraph->GetChildren(children, skipper);
				std::sort(children.begin(), children.end(), CGraphItemCompare(depth != 0));
				for(IGraphItemVector::iterator itr = children.begin(); itr != children.end(); ++itr) 
				{
					foreach(itr->get(), dotStream, depth + 1);
				}
			}
		}
	}

	if(CComQIPtr<IGraph> graph = item)
	{
		dotStream << "}" << std::endl;
	}
	else if(CComQIPtr<IGraphSubgraph> subgraph = item)
	{
		if (m_meta.IsVisible(subgraph))
		{
			dotStream << "}" << std::endl;
		}
	}
}

void CGraphATTLayoutImpl::foreach_GrETL(IGraphItem* item, std::stringstream& dotStream, unsigned int depth)
{
	if(CComQIPtr<IGraph> graph = item)
	{
		GraphTypes::RectF extent = m_canvas->getGraphExtent(false, world);
		GraphTypes::SizeF size;
		extent.GetSize(&size);
		size.Width = (REAL)std::min<int>((int)size.Width, 800);
		size.Height = (REAL)std::min<int>((int)size.Height, 600);

		dotStream << "graph 1 {" << std::endl;
		dotStream << "graph [id=\"A426B88A-D072-4a40-A5B2-E45D3C212335\", label=\"\"]" << std::endl;
		dotStream << "graph [";
		AppendFont(graph, dotStream);
		dotStream << ", dpi=" << m_canvas->m_dpiYbase << "";
		if (m_meta.GetMetaSplines() && !m_bigHack)
			dotStream << ", splines=true";
		else
			dotStream << ", splines=false";
		dotStream << ", rankdir=LR";
		dotStream << ", ordering=in, compound=true";
		dotStream << "];" << std::endl;	

		dotStream << "node [";
		AppendFont(graph, dotStream);
		dotStream << ", shape=box";
		dotStream << ", fixedsize=true";
		dotStream << "];" << std::endl;	

		dotStream << "edge [";
		AppendFont(graph, dotStream);
		dotStream << "];" << std::endl;	

		//  Put edges at start sorted.
		std::_tstring label;
		std::map<unsigned, IGraphVertex*> vdupes;
		std::map<std::pair<unsigned int, unsigned int>, bool> edupes;
		IGraphEdgeVector edges;
		graph->GetEdges(edges);
		std::sort(edges.begin(), edges.end(), CGraphEdgeCompare());
		for (IGraphEdgeVector::iterator itr = edges.begin(); itr != edges.end(); ++itr)
		{
			IGraphEdge * edge = *itr;
			if (m_meta.IsVisible(edge))
			{
				CComPtr<IGraphVertex> s=edge->GetSource();
				CComPtr<IGraphVertex> t=edge->GetTarget();
				ATLASSERT(s && t);
				CComPtr<IGraphSubgraph> ss = s->GetParent();
				CComPtr<IGraphSubgraph> st = t->GetParent();
				ATLASSERT(ss && st);
				if (ss != st && !edupes[std::make_pair(ss->GetID().GetHash(), st->GetID().GetHash())])
				{
					dotStream << ss->GetID().GetHash();
					dotStream << " -- ";
					dotStream << st->GetID().GetHash();
					dotStream << " [id=\"" << edge->GetID().GetHash() << "\"";
					dotStream << "];" << std::endl;
					edupes[std::make_pair(ss->GetID().GetHash(), st->GetID().GetHash())] = true;
				}
			}
		}
	}
	else if(CComQIPtr<IGraphSubgraph> subgraph = item)
	{
		if (m_meta.IsVisible(subgraph))
		{
			dotStream << subgraph->GetID().GetHash();
			dotStream << " [id=\"" << subgraph->GetID().GetHash() << "\"";
			std::string encoded;
			EncodeForDOT(subgraph->GetLabel(), encoded);
			dotStream << " label=\"" << encoded << "\"";
			AppendPos(subgraph, dotStream);
			AppendSize(subgraph, dotStream);
			dotStream << "];" << std::endl;
		}
	}
	else if(CComQIPtr<IGraphVertex> vertex = item)
	{
	}

	if (CComQIPtr<IGraphSubgraph> subgraph = item)	//Graph && Subgraph included.
	{
		if (m_meta.IsVisible(subgraph))
		{
			bool min=false;
			if(CComQIPtr<IAttSubgraphRenderer> iar=m_canvas->GetRenderer(subgraph))
				min=iar->IsMinimized();
			else if(CComQIPtr<ILegacySubgraphRenderer> ilr=m_canvas->GetRenderer(subgraph))
				min=true; // for layout purposes legacy subgraph is always considered minimized
			// don't deal with children for minimized AT&T or legacy subgraph
			if(!min)
			{
				IGraphItemVector children;
				CComPtr<MetaStateSkipper> skipper = new MetaStateSkipper(m_meta, m_canvas, TRISTATE_TRUE);
				subgraph->GetChildren(children, skipper);
				std::sort(children.begin(), children.end(), CGraphItemCompare(depth != 0));
				for(IGraphItemVector::iterator itr = children.begin(); itr != children.end(); ++itr) 
				{
					foreach_GrETL(itr->get(), dotStream, depth + 1);
				}
			}
		}
	}

	if(CComQIPtr<IGraph> graph = item)
	{
		dotStream << "}" << std::endl;
	}
}

void CGraphATTLayoutImpl::foreach_routeEdge(IGraphItem* item, std::stringstream& dotStream, unsigned int depth)
{
	if(CComQIPtr<IGraph> graph = item)
	{
		GraphTypes::RectF extent = m_canvas->getGraphExtent(false, world);
		GraphTypes::SizeF size;
		extent.GetSize(&size);
		size.Width = (REAL)std::min<int>((int)size.Width, 800);
		size.Height = (REAL)std::min<int>((int)size.Height, 600);

		dotStream << "graph 1 {" << std::endl;
		dotStream << "graph [id=\"A426B88A-D072-4a40-A5B2-E45D3C212335\", label=\"\"]" << std::endl;
		dotStream << "graph [";
		AppendFont(graph, dotStream);
		dotStream << ", dpi=" << m_canvas->m_dpiYbase << "";
		if (m_meta.GetMetaSplines() && !m_bigHack)
			dotStream << ", splines=true";
		else
			dotStream << ", splines=false";
		//dotStream << ", rankdir=LR";
		dotStream << ", ordering=in, compound=true";
		dotStream << "];" << std::endl;	

		dotStream << "node [";
		AppendFont(graph, dotStream);
		dotStream << ", shape=box";
		dotStream << ", fixedsize=true";
		dotStream << ", pin=true";
		dotStream << "];" << std::endl;	

		dotStream << "edge [";
		AppendFont(graph, dotStream);
		dotStream << "];" << std::endl;	

		//  Put edges at start sorted.
		std::_tstring label;
		IGraphEdgeVector edges;
		graph->GetEdges(edges);
		std::sort(edges.begin(), edges.end(), CGraphEdgeCompare());
		for (IGraphEdgeVector::iterator itr = edges.begin(); itr != edges.end(); ++itr)
		{
			IGraphEdge * edge = *itr;
			if (m_meta.IsVisible(edge))
			{
				CComPtr<IGraphVertex> s=edge->GetSource();
				CComPtr<IGraphVertex> t=edge->GetTarget();
				ATLASSERT(s && t);
				dotStream << s->GetID().GetHash();
				dotStream << " -- ";
				dotStream << t->GetID().GetHash();
				dotStream << " [id=\"" << edge->GetID().GetHash() << "\"";
				dotStream << "];" << std::endl;
			}
		}
	}
	else if(CComQIPtr<IGraphSubgraph> subgraph = item)
	{
	}
	else if(CComQIPtr<IGraphVertex> vertex = item)
	{
		if (m_meta.IsVisible(vertex))
		{
			dotStream << vertex->GetID().GetHash();
			dotStream << " [id=\"" << vertex->GetID().GetHash() << "\"";
			std::string encoded;
			EncodeForDOT(vertex->GetLabel(), encoded);
			dotStream << " label=\"" << encoded << "\"";
			AppendPos(vertex, dotStream);
			AppendSize(vertex, dotStream);
			dotStream << "];" << std::endl;
		}
	}

	if (CComQIPtr<IGraphSubgraph> subgraph = item)	//Graph && Subgraph included.
	{
		if (m_meta.IsVisible(subgraph))
		{
			bool min=false;
			if(CComQIPtr<IAttSubgraphRenderer> iar=m_canvas->GetRenderer(subgraph))
				min=iar->IsMinimized();
			else if(CComQIPtr<ILegacySubgraphRenderer> ilr=m_canvas->GetRenderer(subgraph))
				min=true; // for layout purposes legacy subgraph is always considered minimized
			// don't deal with children for minimized AT&T or legacy subgraph
			if(!min)
			{
				IGraphItemVector children;
				CComPtr<MetaStateSkipper> skipper = new MetaStateSkipper(m_meta, m_canvas, TRISTATE_TRUE);
				subgraph->GetChildren(children, skipper);
				std::sort(children.begin(), children.end(), CGraphItemCompare(depth != 0));
				for(IGraphItemVector::iterator itr = children.begin(); itr != children.end(); ++itr) 
				{
					foreach_routeEdge(itr->get(), dotStream, depth + 1);
				}
			}
		}
	}

	if(CComQIPtr<IGraph> graph = item)
	{
		dotStream << "}" << std::endl;
	}
}

class ScaleRestore
{
protected:
	CComPtr<CGraphViewCanvas> m_view;
	CComPtr<CCanvas> m_canvas;
	float m_oldScale;
	int m_oldRotate;
	float m_oldSpacing;

public:
	ScaleRestore(CGraphViewCanvas * view, CCanvas *canvas, NEATO_MODE mode, bool splines) : m_view(view), m_canvas(canvas)
	{
		m_oldScale = m_canvas->GetScale();
		m_oldRotate = m_canvas->GetRotation();
		m_oldSpacing = m_canvas->GetSpacing();
		m_canvas->SetScale(1.0f);
		m_canvas->SetSpacing(1.0f);
		m_canvas->SetRotation(0);
	}

	~ScaleRestore()
	{
		m_canvas->SetRotation(m_oldRotate);
		m_canvas->SetSpacing(m_oldSpacing);
		m_canvas->SetScale(m_oldScale);
	}

};
//	---------------------------------------------------------------------------
LAYOUTEVENT CGraphATTLayoutImpl::pulse(const GraphTypes::RectF & extent)
{
	bool retVal = false;

	m_bigHack = false;
	if (pulse2(extent))
		return LAYOUTEVENT_FINISHED;

	m_bigHack = true;
	if (pulse2(extent))
		return LAYOUTEVENT_FINISHED;

	return LAYOUTEVENT_ABORTED;
}

bool CGraphATTLayoutImpl::pulse2(const GraphTypes::RectF & extent)
{
	ScaleRestore restorer(m_view, m_canvas, m_mode, m_meta.GetMetaSplines());
	if (m_mode == NEATO_MODE_EDGES && m_meta.IsAvoidObstacles()==true)
	{
		CComPtr<ISkipEdge> skipper=new MetaVisibleSkipper(m_meta);
		IGraphEdgeVector eresults;

		unsigned edgecount=m_graph->GetEdges(eresults, skipper);

		if(edgecount>0)
		{
			PointFVector vpsrc, vpdest;
			PointFVector vpsrcoff, vpdestoff;

			CComPtr<IGraphEdge> edge;
			PointF src, tgt, srcoff, tgtoff, pt;

			for(IGraphEdgeVector::iterator eitr=eresults.begin();
				eitr!=eresults.end();
				++eitr)
			{
				CComQIPtr<IEdgeRenderer> ier=m_canvas->GetRenderer(*eitr);
				if(ier!=0)
				{
					ier->GetConnectionPoints(src, tgt, screen, &srcoff, &tgtoff);
					vpsrc.push_back(src);
					vpdest.push_back(tgt);
					vpsrcoff.push_back(srcoff);
					vpdestoff.push_back(tgtoff);
				}
			}
	
			AttVisG::Polygon route;
			IGraphItemVector results;
			unsigned childcount=m_graph->GetChildren(results, new MetaVisibleSkipper(m_meta));

			AttVisG::NPolygon polys(childcount);
			polys.Autodelete(true);
			AttVisG::Polygon* obs;
			CComQIPtr<IGraphSubgraph> subgraph;
			CComQIPtr<IListWindowRenderer> ilw;
			RectF rcbox;
			PointFVector::iterator itrsrcp, itrdstp;
			MapP2P pointmap;

			for(IGraphItemVector::iterator itr=results.begin(); itr!=results.end(); ++itr)
			{
				subgraph=*itr;	

				if(subgraph!=0)
				{
					ilw=m_canvas->GetRenderer(subgraph);

					if(ilw!=0)
					{
						ilw->GetContainerRect(rcbox);
						itrsrcp=vpsrc.begin();
						itrdstp=vpdest.begin();

						bool inobstacle=false;

						while(itrsrcp!=vpsrc.end())
						{
							if(rcbox.Contains(*itrsrcp))
							{
								inobstacle=true;
								break;
							}

							if(rcbox.Contains(*itrdstp))
							{
								inobstacle=true;
								break;
							}

							++itrsrcp;
							++itrdstp;
						}

						if(!inobstacle)
						{
							REAL2Tuple key;
							PointF val;

							obs=new AttVisG::Polygon(rcbox);
							polys.Append(obs);

							obs->GetPoint(val, 0);
							key=REAL2Tuple(val.x,val.y);

							val.x-=SPLINE_OFFSET_X;
							val.y-=SPLINE_OFFSET_Y;
							pointmap[key]=val;

							obs->GetPoint(val, 1);
							key=REAL2Tuple(val.x,val.y);

							val.x-=SPLINE_OFFSET_X;
							val.y+=SPLINE_OFFSET_Y;
							pointmap[key]=val;

							obs->GetPoint(val, 2);
							key=REAL2Tuple(val.x,val.y);

							val.x+=SPLINE_OFFSET_X;
							val.y+=SPLINE_OFFSET_Y;
							pointmap[key]=val;

							obs->GetPoint(val, 3);
							key=REAL2Tuple(val.x,val.y);

							val.x+=SPLINE_OFFSET_X;
							val.y-=SPLINE_OFFSET_Y;
							pointmap[key]=val;
						}
					}
				}
			}

			unsigned stcount=0;
			PointFVector vp;
			PointFVector::iterator itrsrcpoff, itrdstpoff;

			itrsrcpoff=vpsrcoff.begin();
			itrdstpoff=vpdestoff.begin();

			for(IGraphEdgeVector::iterator eitr=eresults.begin(); eitr!=eresults.end(); ++eitr)
			{
				CComQIPtr<IEdgeRenderer> ier=m_canvas->GetRenderer(*eitr);
				if(ier!=0)
				{
					bool bOk=ShortestRoute(
						polys,
						vpsrc[stcount],
						vpdest[stcount],
						route);

					stcount++;

					vp.clear();

					if(bOk && route.count()>2)
					{
						// add first and last point twice 

						route.GetPoint(pt, 0);
						vp.push_back(pt);
						vp.push_back(pt);

						MapP2P::iterator imap;
						REAL2Tuple key;

						for(unsigned i=1; i<route.count()-1; ++i)
						{
							route.GetPoint(pt, i);
							key=REAL2Tuple(pt.x,pt.y);
							imap=pointmap.find(key);
							if(imap!=pointmap.end())
							{
								pt=(*imap).second;
							}
							vp.push_back(pt);
						}

						route.GetPoint(pt, route.count()-1);
						vp.push_back(pt);
						vp.push_back(pt);

						unsigned last=vp.size()-1;

						// 'massage' points to move edge curve away from endpoints

						if((*itrsrcpoff).x>0)
						{
							vp[1].x+=SPLINE_OFFSET_X;
						}
						else
						{
							vp[1].x-=SPLINE_OFFSET_X;
						}


						if((*itrdstpoff).x>0)
						{
							vp[last-1].x+=SPLINE_OFFSET_X;
						}
						else
						{
							vp[last-1].x-=SPLINE_OFFSET_X;
						}

						ier->SetPoints(vp, screen);
						route.transfer(0,0);
					}
				}

				++itrsrcpoff;
				++itrdstpoff;
			}
		}

		return true;
	}

	std::_tstring formatbuf;
	RectF bbox;

	std::string svgResult;
	bool retVal = false;
	std::stringstream dotStream;
	{
		switch(m_canvas->GetApplicationMode())
		{
		case AppModeGrETL:
			if (m_mode == NEATO_MODE_EDGES && m_meta.IsAvoidObstacles()==true)
				foreach_routeEdge(m_graph, dotStream, 0);
			else
				foreach_GrETL(m_graph, dotStream, 0);
			//m_mode = NEATO_MODE_DOT;
			break;
		default:
			foreach(m_graph, dotStream, 0);
			break;
		}
		{
			TCHAR format [] = _T("svg");
			std::string buffer = dotStream.str();
			// TODO: KPD this can be a meta value, or class member, or whatever, but it CANNOT be
			// a situation where a generic control is going and getting a *Query Builder* app
			// config setting (since the managed control is NOT Query Builder nor is it an app :-)
#if defined(_DEBUG) //&& !defined(SEISINT_LIBEXPORTS)
			//if (static_cast<bool>(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_DEBUG_LOGRESULTXML))==true)
			{
				std::ofstream file("c:\\temp\\neato_cpp.dot");
				file << buffer;
			}
#endif

			std::string scale = boost::lexical_cast<std::string>(m_canvas->m_dpiYbase);
			std::ostringstream out;
			switch(m_mode)
			{
			case NEATO_MODE_NEATO:
			case NEATO_MODE_EDGES:
				retVal = DoLayout("neato", buffer.c_str(), CT2A(format), scale.c_str(), svgResult);
				break;
			case NEATO_MODE_FDP:
				retVal = DoLayout("fdp", buffer.c_str(), CT2A(format), scale.c_str(), svgResult);
				break;
			case NEATO_MODE_TWOPI:
				retVal = DoLayout("twopi", buffer.c_str(), CT2A(format), scale.c_str(), svgResult);
				break;
			case NEATO_MODE_DOT:
			case NEATO_MODE_SUBGRAPH_TREE:
				retVal = DoLayout("dot", buffer.c_str(), CT2A(format), scale.c_str(), svgResult);
				break;
			case NEATO_MODE_CIRCO:
				retVal = DoLayout("circo", buffer.c_str(), CT2A(format), scale.c_str(), svgResult);
				break;
			}
		}
	}
	{
#if defined(_DEBUG)// && !defined(SEISINT_LIBEXPORTS)
		//	if (static_cast<bool>(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_DEBUG_LOGRESULTXML))==true)
		{
			std::ofstream file("c:\\temp\\neato_cpp.xml");
			file << svgResult;
		}
#endif
		std::_tstring content = CA2T(svgResult.c_str(), CP_UTF8);
		mergeSVG(content.c_str(), m_graph, m_mode);
	}
	return retVal;
}

void CGraphATTLayoutImpl::halt()
{
}
//	---------------------------------------------------------------------------
void CGraphATTLayoutImpl::mergeSVG(const TCHAR* svg, IGraph* graph, NEATO_MODE mode)
{
	CComInitialize com;
	CComPtr<ISAXXMLReader> reader; 
	HRESULT hr = reader.CoCreateInstance(CLSID_SAXXMLReader30);

	CComPtr <CGraphATTSVGHandler> handler = new CGraphATTSVGHandler(graph, m_canvas, &m_meta, mode);
	hr = reader->putContentHandler(handler); 
	if (hr == S_OK)
	{
		hr = reader->parse(CComVariant(svg));
	}
}
//	---------------------------------------------------------------------------
void QuickTest(IGraph * /*graph*/, CMeta & /*meta*/)
{
}
//	---------------------------------------------------------------------------
bool ShortestRoute(
	AttVisG::NPolygon& polys,
	const PointF& src,
	const PointF& dst,
	AttVisG::Polygon& route )
{
	bool bSuccess=false;
	Ppolyline_t pathstore;
	Ppoint_t psrc, pdst;
	
	psrc.x=src.x;
	psrc.y=src.y;
	pdst.x=dst.x;
	pdst.y=dst.y;

	int res=shortest_route(
		reinterpret_cast<Ppoly_t**>(polys.memory()),
		polys.count(),
		&psrc,
		&pdst,
		&pathstore);

	bSuccess=res!=0;

	if(bSuccess)
		route.transfer(pathstore.ps, pathstore.pn);

	return bSuccess;
}
//neato -Tjpeg -otmp.jpeg < d:\temp\test.dot
