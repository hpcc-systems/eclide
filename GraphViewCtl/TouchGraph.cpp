#include "stdafx.h"
#include "resource.h"

#include "TouchGraph.h"
#include "layoutImpl.h"
#include "Renderer.h"

using namespace GraphTypes;

CGraphTouchgraphLayoutImpl::CGraphTouchgraphLayoutImpl(IGraph * graph, CMeta & meta, CCanvas * canvas) : CLayout(graph, meta, canvas) 
{
	damper = 1.0; 
	maxMotion = 0;
	lastMaxMotion = 0;
	motionRatio = 0; 
	damping = true; 
	rigidity = 2;   
	newRigidity = rigidity;
	m_halt = false;
}

CGraphTouchgraphLayoutImpl::~CGraphTouchgraphLayoutImpl()
{
}

void CGraphTouchgraphLayoutImpl::append(IGraphItem * item, const PointF & p)
{
	__super::append(item, p);
	//ATLTRACE("%d %d\n", v->GetID() , m_dv.size());
	CUniqueID id = item->GetID();
	m_dv[id] = PointF(0L, 0L);
	m_repulsion[id] = 200;
//	m_justMadeLocal[id] = false;
}

void CGraphTouchgraphLayoutImpl::remove(IGraphItem * item)
{
	m_dv.erase(item->GetID());
	m_repulsion.erase(item->GetID());
//	m_justMadeLocal.erase(item->GetID());
	__super::remove(item);
}

void CGraphTouchgraphLayoutImpl::setRigidity(float r) 
{
	newRigidity = r;  //update rigidity at the end of the relax() thread
}

void attractiveForce(IGraphItem * u, IGraphItem * v)
{
}

//relaxEdges is more like tense edges up.  All edges pull nodes closes together;
void CGraphTouchgraphLayoutImpl::relaxEdges() 
{
	std::map<unsigned,IGraphVertex*> dupes;

	for (CComPtr<IEdgeIterator> itr = m_graph->GetEdgeIterator(); itr->IsValidItem() == true && !m_halt; itr->NextItem())
	{
		IGraphEdge * edge = itr->Get();
		if (m_meta.IsVisible(edge))
		{
			IGraphVertex* s = edge->GetSource();
			IGraphVertex* t = edge->GetTarget();
//TODO Identical to this edge pruning algo is used also in neato.cpp. Isolate into the 
//method when changing headers will be less painful
			CComPtr<IGraphSubgraph> ss=s->GetParent();
			CComPtr<IGraphSubgraph> st=t->GetParent();
			ATLASSERT(ss && st);

			bool addedge=true;
			bool sinsg=false, tinsg=false;

			unsigned shash, thash;
			if(ss->GetID()!=ROOT_ID) // edge for vertex that is not in the root graph
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

			if(addedge && st->GetID()!=ROOT_ID) // edge for vertex that is not in the root graph
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

			if(!addedge)
				continue;

			PointF posTo;
			IRenderer * ir = m_canvas->GetRenderer(t);
			if(ir)
			{
				ir->GetPosition(posTo);
			}
			PointF posFrom;
			ir = m_canvas->GetRenderer(s);
			if(ir)
			{
				ir->GetPosition(posFrom);
			}

			PointF vP(posTo.x - posFrom.x, posTo.y - posFrom.y);			
			float len = sqrt(vP.x * vP.x + vP.y * vP.y);

			//PointF
			float dx = vP.x * rigidity;  //rigidity makes edges tighter
			float dy = vP.y * rigidity;

			dx /= 150.0f * 100;
			dy /= 150.0f * 100;

			// Edges pull directly in proportion to the distance between the nodes. This is good,
			// because we want the edges to be stretchy.  The edges are ideal rubberbands.  They
			// They don't become springs when they are too short.  That only causes the graph to
			// oscillate.

//			ATLASSERT(dx < 1 && dy < 1);

			//if (m_justMadeLocal[to->GetID()] || !m_justMadeLocal[from->GetID()]) 
			//{
			//	m_dv[to->GetID()].x -= dx * len;
			//	m_dv[to->GetID()].y -= dy * len;
			//} 
			//else 
			{
				m_dv[t->GetID()].x -= dx * len / 10;
				m_dv[t->GetID()].y -= dy * len / 10;
			}

			//if (m_justMadeLocal[from->GetID()] || !m_justMadeLocal[to->GetID()]) 
			//{
			//	m_dv[from->GetID()].x += dx * len;
			//	m_dv[from->GetID()].y += dy * len;
			//} 
			//else 
			{
				m_dv[s->GetID()].x += dx * len / 10;
				m_dv[s->GetID()].y += dy * len / 10;
			}
		}
	}
}

void CGraphTouchgraphLayoutImpl::applyForce(IGraphItem * item1, IGraphItem * item2)
{
	PointF nOne;
	PointF nTwo;
	float dx=0;
	float dy=0;

	IRenderer * ir1=m_canvas->GetRenderer(item1);
	IRenderer * ir2=m_canvas->GetRenderer(item2);
	ir1->GetPosition(nOne);
	ir2->GetPosition(nTwo);

	PointF vP(nOne.x - nTwo.x, nOne.y - nTwo.y);			
	float len = vP.x * vP.x + vP.y * vP.y;
	if (len == 0)
	{
		dx = (float)(rand() % 10 - 5); //If two nodes are right on top of each other, randomly separate
		dy = (float)(rand() % 10 - 5);
	}
	else if (len < 600 * 600) 
	{ //600, because we don't want deleted nodes to fly too far away
		dx = vP.x / len;  // If it was sqrt(len) then a single node surrounded by many others will
		dy = vP.y / len;  // always look like a circle.  This might look good at first, but I think
		// it makes large graphs look ugly + it contributes to oscillation.  A
		// linear function does not fall off fast enough, so you get rough edges
		// in the 'force field'
	}

	int repSum = m_repulsion[item1->GetID()] * m_repulsion[item2->GetID()] / 100;

	//if(m_justMadeLocal[item1->GetID()] || !m_justMadeLocal[item2->GetID()]) {
	//	m_dv[item1->GetID()].x += dx*repSum*rigidity;
	//	m_dv[item1->GetID()].y += dy*repSum*rigidity;
	//}
	//else 
	{
		m_dv[item1->GetID()].x += dx*repSum*rigidity/10;
		m_dv[item1->GetID()].y += dy*repSum*rigidity/10;
	}

	//if (m_justMadeLocal[item2->GetID()] || !m_justMadeLocal[item1->GetID()]) 
	//{
	//	m_dv[item2->GetID()].x -= dx*repSum*rigidity;
	//	m_dv[item2->GetID()].y -= dy*repSum*rigidity;
	//}
	//else 
	{
		m_dv[item2->GetID()].x -= dx*repSum*rigidity/10;
		m_dv[item2->GetID()].y -= dy*repSum*rigidity/10;
	}
}

void CGraphTouchgraphLayoutImpl::avoidLabels() 
{
	int n1_count = 0;
	for(CComPtr<IGraphItemIterator> n1 = m_graph->GetChildren(new MetaVisibleSkipper(m_meta)); n1->IsValidItem(); n1->NextItem())
	{
		int n2_count = 0;
		CComPtr<IGraphItemIterator> n2 = m_graph->GetChildren(new MetaVisibleSkipper(m_meta));
		while(true)
		{
			if (!n2->IsValidItem())
				break;
			if (n2_count > n1_count)
				break;
			n2->NextItem();
			++n2_count;
		}
		for(; n2->IsValidItem() && !m_halt; n2->NextItem())
		{
			IGraphItem* item1 = n1->Get();
			IGraphItem* item2 = n2->Get();
			applyForce(item1, item2);
		}
	}
}

void CGraphTouchgraphLayoutImpl::avoidLabels2() 
{
    typedef std::list<IGraphItem*> bucket_t;
    typedef std::vector<bucket_t> buckets_t;

	double two_k = double(2) * std::sqrt(m_extent.Width * m_extent.Height / m_graph->GetVertexCount());

    std::size_t columns = std::size_t(m_extent.Width / two_k + double(1));
    std::size_t rows = std::size_t(m_extent.Height / two_k + double(1));
    buckets_t buckets(rows * columns);
	for(CComPtr<IGraphItemIterator> itr = m_graph->GetChildren(new MetaVisibleSkipper(m_meta)); itr->IsValidItem(); itr->NextItem())
	{
		IGraphItem* item = itr->Get();
		IRenderer * ir1=m_canvas->GetRenderer(item);

		PointF pos;
		ir1->GetPosition(pos);

		std::size_t column = std::size_t((pos.x - m_extent.x + m_extent.Width  / 2) / two_k);
		std::size_t row    = std::size_t((pos.y - m_extent.y + m_extent.Height / 2) / two_k);

		if (column >= columns) 
		{
			column = columns - 1;
		}

		if (row >= rows) 
		{
			row = rows - 1;
		}

		//ATLTRACE(_T("%s put into %i, %i\n"), vertex1->GetLabel(), column, row);
		buckets[row * columns + column].push_back(item);
	}

#ifdef _DEBUG
	//for (unsigned int row = 0; row < rows && !m_halt; ++row)
	//{
	//	for (unsigned int column = 0; column < columns && !m_halt; ++column) 
	//	{
	//	    bucket_t& bklist=buckets[row * column + row];
	//		unsigned bklistsize=bklist.size();
	//		int debug_here;
	//		debug_here=1;
	//	}
	//}
#endif

	for (unsigned int row = 0; row < rows && !m_halt; ++row)
	{
		for (unsigned int column = 0; column < columns && !m_halt; ++column) 
		{
			bucket_t& bucket = buckets[row * columns + column];
			typedef bucket_t::iterator bucket_iterator;
			for (bucket_iterator u = bucket.begin(); u != bucket.end() && !m_halt; ++u) 
			{
				// Repulse vertices in this bucket
				bucket_iterator v = u;
				for (++v; v != bucket.end() && !m_halt; ++v) {
					applyForce(*u, *v);
					applyForce(*v, *u);
				}

				std::size_t adj_start_row = row == 0? 0 : row - 1;
				std::size_t adj_end_row = row == rows - 1? row : row + 1;
				std::size_t adj_start_column = column == 0? 0 : column - 1;
				std::size_t adj_end_column = column == columns - 1? column : column + 1;
				for (unsigned int other_row = adj_start_row; other_row <= adj_end_row && !m_halt; ++other_row)
				{
					for (unsigned int other_column = adj_start_column; other_column <= adj_end_column && !m_halt; ++other_column)
					{
						if (other_row != row || other_column != column) 
						{
							// Repulse vertices in this bucket
							bucket_t& other_bucket = buckets[other_row * columns + other_column];
							for (v = other_bucket.begin(); v != other_bucket.end() && !m_halt; ++v)
							{
								applyForce(*u, *v);
							}
						}
					}
				}
			}
		}
	}
}

void CGraphTouchgraphLayoutImpl::init()
{
	resetDamper();
	__super::init();
	m_halt = false;
}

void CGraphTouchgraphLayoutImpl::startDamper() 
{
	damping = true;
}

void CGraphTouchgraphLayoutImpl::stopDamper() 
{
	damping = false;
	damper = 1.0;     //A value of 1.0 means no damping
}

void CGraphTouchgraphLayoutImpl::resetDamper() 
{  //reset the damper, but don't keep damping.
	damping = true;
	damper = 1.0f;
	maxMotion = 100;
}

void CGraphTouchgraphLayoutImpl::stopMotion() 
{  // stabilize the graph, but do so gently by setting the damper to a low value
	damping = true;
	if (damper > 0.3f) 
		damper = 0.3f;
	else
		damper = 0.0f;
}
void CGraphTouchgraphLayoutImpl::damp() 
{
	if (damping) {
		if(motionRatio<=0.1f) 
		{  
			ATLTRACE("Damper %f\n", damper);
#define SIMPLE_DAMPING
#ifdef SIMPLE_DAMPING
			damper *= (1.0f + motionRatio / 8);	
#else
			//This is important.  Only damp when the graph starts to move faster
			//When there is noise, you damp roughly half the time. (Which is a lot)
			//
			//If things are slowing down, then you can let them do so on their own,
			//without damping.

			//If max motion<0.2, damp away
			//If by the time the damper has ticked down to 0.9, maxMotion is still>1, damp away
			//We never want the damper to be negative though
			if ((maxMotion<20 || (maxMotion>100 && damper<0.9f)) && damper > 0.01f) 
				damper -= 0.01f;
			//If we've slowed down significanly, damp more aggresively (then the line two below)
			else if (maxMotion<40 && damper > 0.003f) 
				damper -= 0.003f;
			//If max motion is pretty high, and we just started damping, then only damp slightly
			else if(damper>0.0001f) 
				damper -=0.0001f;
#endif
		}
	}
	//if(maxMotion < 33.3 && damping) 
	//	damper = 0;
}

void CGraphTouchgraphLayoutImpl::moveNodes() 
{
	lastMaxMotion = maxMotion;
	float maxMotionA;
	maxMotionA=0;

	PointF pos;
	for(CComPtr<IGraphItemIterator> itr = m_graph->GetChildren(new MetaVisibleSkipper(m_meta)); itr->IsValidItem(); itr->NextItem())
	{
		IGraphItem* item=itr->Get();	

		float dx = m_dv[item->GetID()].x;
		float dy = m_dv[item->GetID()].y;
		dx*=damper;  //The damper slows things down.  It cuts down jiggling at the last moment, and optimizes
		dy*=damper;  //layout.  As an experiment, get rid of the damper in these lines, and make a
		//long straight line of nodes.  It wiggles too much and doesn't straighten out.

		m_dv[item->GetID()].x = dx/2;   //Slow down, but don't stop.  Nodes in motion store momentum.  This helps when the force
		m_dv[item->GetID()].y = dy/2;   //on a node is very low, but you still want to get optimal layout.

		float distMoved = sqrt(dx*dx+dy*dy); //how far did the node actually move?
		//ATLTRACE(_T("Node Moved %f\n"), distMoved);

		CComQIPtr<ILegacyRenderer> lr = m_canvas->GetRenderer(item);
		if(lr && !lr->GetPinned() && !lr->GetDragging())
		{
			lr->GetPosition(pos);
			pos.x += std::max<float>(-30, std::min<float>(30, dx)); //don't move faster then 30 units at a time.
			pos.y += std::max<float>(-30, std::min<float>(30, dy)); //I forget when this is important.  Stopping severed nodes from flying away?
			lr->SetPosition(pos);
		}
		else
		{
			CComQIPtr<ILegacySubgraphRenderer> lsgr = m_canvas->GetRenderer(item);
			if(lsgr && !lsgr->GetPinned())
			{
				lsgr->GetPosition(pos);
				pos.x += std::max<float>(-30, std::min<float>(30, dx)); //don't move faster then 30 units at a time.
				pos.y += std::max<float>(-30, std::min<float>(30, dy)); //I forget when this is important.  Stopping severed nodes from flying away?
				lsgr->SetPosition(pos);
			}
		}

		maxMotionA=std::max<float>(distMoved, maxMotionA);
	}

	maxMotion = maxMotionA;
	if (maxMotion>0) 
		motionRatio = lastMaxMotion/maxMotion-1; //subtract 1 to make a m_positive value mean that
	else 
		motionRatio = 0;                         //things are moving faster
}

void CGraphTouchgraphLayoutImpl::relax() 
{
	for (int i = 0; i < 3 && !m_halt; ++i) 
	{
		relaxEdges();
		avoidLabels2();
		moveNodes();
		damp();
	}
	if(rigidity != newRigidity) 
		rigidity = newRigidity; //update rigidity
}

LAYOUTEVENT CGraphTouchgraphLayoutImpl::pulse(const GraphTypes::RectF & extent)
{
	m_extent = extent;
	ATLTRACE("%f\t%i\t%f\r\n", damper, damping, maxMotion);
	if (((damper < 0.33) && damping) || (maxMotion < 2.000)) 
	{
		//m_justMadeLocal.clear();
		return LAYOUTEVENT_FINISHED;
	}
	relax();
	return LAYOUTEVENT_ITERATION;
}

void CGraphTouchgraphLayoutImpl::halt()
{
	m_halt = true;
}

void CGraphTouchgraphLayoutImpl::setPoint(IGraphItem * item, const PointF & p)
{
//TODO making separate pinnable interface will get rid of checking two renderers
	CComPtr<IRenderer> ir=m_canvas->GetRenderer(item);
	CComQIPtr<ILegacyRenderer> lr=ir;
	if(lr!=0)
	{
		if(!lr->GetPinned())
			lr->SetPosition(p);
	}
	else
	{
		CComQIPtr<ILegacySubgraphRenderer> lsgr=ir;
		if(lsgr!=0 && !lsgr->GetPinned())
			lsgr->SetPosition(p);
	}
	resetDamper();
}
