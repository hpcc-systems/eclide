#include "stdafx.h"

#include "Canvas.h"
#include "Lee.h"

#ifdef LEE_LAYOUT
#ifdef _DEBUG
#include "UnicodeFile.h"
#endif

//*****************************************************************************

typedef boost::adjacency_list<boost::vecS,
    boost::vecS, 
    boost::undirectedS,
    boost::no_property, 
    boost::property < boost::edge_weight_t, int >
> Grid;

typedef boost::graph_traits<Grid> Grid_Traits;
typedef Grid_Traits::vertex_descriptor grid_vertex_descriptor;
typedef Grid_Traits::edge_descriptor grid_edge_descriptor;
typedef Grid_Traits::vertex_iterator grid_vertex_iterator;
typedef Grid_Traits::adjacency_iterator grid_adjacency_iterator;
typedef Grid_Traits::edge_iterator grid_edge_iterator;
typedef Grid_Traits::vertices_size_type grid_vertices_size_type;
typedef std::vector<grid_vertex_descriptor> grid_vertex_descriptor_vector;
typedef	boost::property_map<Grid, boost::edge_weight_t>::type weight_mapXXX;

//*****************************************************************************
//
// Helper exception and visitor classes for Dijkstra shortest path algo
//
//*****************************************************************************

class ex_target_found
    : public std::out_of_range
{
public:
    ex_target_found(const std::string& whatString)
        : out_of_range(whatString)
    {}
};

//-----------------------------------------------------------------------------

template <class Vertex, class Tag>
struct target_visitor : public boost::default_dijkstra_visitor
{
    target_visitor(Vertex u) : v(u) { }

    template <class Vertex, class Graph>
        void discover_vertex(Vertex u, Graph& g) 
    {
        if ( u==v )
            throw ex_target_found("Shortest path target found");
    }

private:
  Vertex v;
};

//-----------------------------------------------------------------------------

template <class Vertex, class Tag>
target_visitor<Vertex, Tag>
target_visit(Vertex u, Tag) {
    return target_visitor<Vertex, Tag>(u);
}

//*****************************************************************************

class LeeRoutePlannerImpl
    :public LeeRoutePlanner
{
    GraphTypes::SizeF m_gridbox;
    size_t m_width, m_height;
    int m_minX, m_maxX, m_minY, m_maxY, m_border; 
    bool m_bgrid;
    Grid m_graph;

public:
    LeeRoutePlannerImpl(
        const GraphTypes::SizeF& gridbox, int border )
        : m_gridbox(gridbox), m_border(border), m_bgrid(false) {};
    ~LeeRoutePlannerImpl() {};

    virtual void Dispose();

    virtual void BeginAddPoints( void );

    virtual void AddPoint(
        const GraphTypes::PointF& pt,
        GraphTypes::PointF* aligned_pt=0 );	

    virtual bool EndAddPoints( void );

    virtual bool GetShortestPath( 
        const GraphTypes::PointF& start,
        const GraphTypes::PointF& end,
        GraphTypes::PointFVector& path );

private:	
    enum Direction
    {
        dirNotSet,
        dirVert,
        dirHorz,
        dirLeft,
        dirRight,
        dirUp,
        dirDown
    };

    grid_edge_descriptor AddEdge(
        grid_vertex_descriptor from,
        grid_vertex_descriptor to );

    void FindShortestPath(
        grid_vertex_descriptor from,
        grid_vertex_descriptor to,
        grid_vertex_descriptor_vector &result );

    void InitWeights( int weight=1 );

    void SetPathWeight(
        grid_vertex_descriptor_vector& path,
        int weightIncrement=1 );

    void AddAllEdges( void );

    grid_vertex_descriptor PointF2Descriptor( 
        const GraphTypes::PointF& point );

    void Descriptor2PointF(
        grid_vertex_descriptor vd,
        GraphTypes::PointF& point );

    Direction GetDirection(
        grid_vertex_descriptor from,
        grid_vertex_descriptor to );

#ifdef _DEBUG
    void Dump();
#endif

};

//*****************************************************************************

LeeRoutePlanner* LeeRoutePlanner::Create( 
    const GraphTypes::SizeF& gridbox,
    int border )
{
    LeeRoutePlanner* planner=0;

    planner=new LeeRoutePlannerImpl(gridbox, border);

    return planner;
}

//-----------------------------------------------------------------------------

void LeeRoutePlannerImpl::Dispose()
{
    delete this;
}

//-----------------------------------------------------------------------------

void LeeRoutePlannerImpl::BeginAddPoints( void )
{
    m_minX=boost::integer_traits<int>::const_max; 
    m_maxX=boost::integer_traits<int>::const_min;
    m_minY=boost::integer_traits<int>::const_max; 
    m_maxY=boost::integer_traits<int>::const_min;

    m_bgrid=false;
}

//-----------------------------------------------------------------------------

void LeeRoutePlannerImpl::AddPoint(
    const GraphTypes::PointF& pt,
    GraphTypes::PointF* aligned_pt/*=0*/ )
{
    CPoint ipt;

    assert(!m_bgrid); // don't allow adding points after grid gets created
    ipt.x=int(pt.x/m_gridbox.Width); 
    ipt.y=int(pt.y/m_gridbox.Height);

    if (ipt.x<m_minX)
        m_minX=ipt.x;
    if (ipt.y<m_minY)
        m_minY=ipt.y;

    if (ipt.x>m_maxX)
        m_maxX=ipt.x;
    if (ipt.y>m_maxY)
        m_maxY=ipt.y;

    if ( aligned_pt )
    {
        aligned_pt->x=ipt.x*m_gridbox.Width;
        aligned_pt->y=ipt.y*m_gridbox.Height;
    }
}

//-----------------------------------------------------------------------------

bool LeeRoutePlannerImpl::EndAddPoints( void )
{
    bool bSuccess=false;

    m_width=abs(m_maxX-m_minX)+m_border*2;
    m_height=abs(m_maxY-m_minY)+m_border*2;

    assert(m_height>0);
    assert(m_width>0);

    bSuccess=m_width>0 && m_height>0;

    if ( bSuccess )
    {
        grid_vertices_size_type	i, numvertices;	

        numvertices=grid_vertices_size_type((m_width+1) * (m_height+1));

        for (i=0; i<numvertices; i++)
            add_vertex(m_graph);

        assert(numvertices==num_vertices(m_graph));

        bSuccess=numvertices==num_vertices(m_graph);

        if ( bSuccess )
        {
            AddAllEdges();
            InitWeights();
            m_bgrid=true; 
        }
    }

    return bSuccess;
}

//-----------------------------------------------------------------------------

bool LeeRoutePlannerImpl::GetShortestPath( 
    const GraphTypes::PointF& start,
    const GraphTypes::PointF& end,
    GraphTypes::PointFVector& path )
{
    bool bSuccess=m_bgrid;

    if ( bSuccess ) // make sure EndAddPoints was called
    {
        grid_vertex_descriptor from, to;

        from=PointF2Descriptor(start);
        to=PointF2Descriptor(end);

        if ( from!=to )
        {
            grid_vertex_descriptor_vector vpath;
            grid_vertex_descriptor_vector::reverse_iterator ri, rend;

            FindShortestPath( from, to, vpath );
            SetPathWeight( vpath );

            if (vpath.size() >= 3)
            {
                grid_vertex_descriptor first;
                GraphTypes::PointF pt;
                Direction dir = dirNotSet, lastDir=dirNotSet;

                path.clear();
                ri=vpath.rbegin();
                rend=vpath.rend();

                // don't store first and last points - they belong to vertices
                ++ri;
                --rend;
                first=from;

                for ( ; ri!=rend; ++ri)
                {
                    Descriptor2PointF(*ri, pt);
                    dir=GetDirection(first, *ri);
                    if (dir==lastDir) // don't store colinear points
                        path.pop_back(); 

                    path.push_back(pt);
                    lastDir=dir;
                    first=*ri;
                }

                dir=GetDirection(first, to);
                if (dir==lastDir) // don't store colinear points
                    path.pop_back();

            }
            else
            {
                bSuccess=false;
            }
        } 
    }

    return bSuccess;
}

//-----------------------------------------------------------------------------

grid_edge_descriptor LeeRoutePlannerImpl::AddEdge(
    grid_vertex_descriptor from,
    grid_vertex_descriptor to)
{
    std::pair<grid_edge_descriptor, bool> newEdge = 
        add_edge(from, to, m_graph);

    assert(newEdge.second);
    return newEdge.first;
}

//------------------------------------------------------------------------------

void LeeRoutePlannerImpl::InitWeights( int weight/*=1*/ )
{
    weight_mapXXX weightmap = get(boost::edge_weight, m_graph);

    grid_edge_iterator i, iend;
    for (boost::tie(i, iend) = edges(m_graph); i != iend; ++i)
    {
        weightmap[*i]=weight;
    }
}

//------------------------------------------------------------------------------

void LeeRoutePlannerImpl::AddAllEdges( void )
{
    grid_vertices_size_type i, j;
    grid_vertex_descriptor start;

    // all horizontal edges
    start=0;
    for (j=0; j<=m_height; ++j) 
    {
        for (i=0; i<m_width; ++i)
        {
            AddEdge(start, start+1);
            start++;
        }
        start++;
    }

    // all vertical edges
    start=0;
    for (j=0; j<m_height; ++j) 
    {
        for (i=0; i<=m_width; ++i)
        {
            AddEdge(start, start+m_width+1);
            start++;
        }
    }
}

//------------------------------------------------------------------------------

void LeeRoutePlannerImpl::SetPathWeight(
    grid_vertex_descriptor_vector& path,
    int weightIncrement/*=1*/ )
{
    weight_mapXXX weightmap = get(boost::edge_weight, m_graph);
    grid_edge_descriptor e;
    grid_vertex_descriptor from, to;
    grid_vertex_descriptor_vector::iterator i;
    bool exists;

    i=path.begin();
    from=*i;
    ++i;

    while( i!=path.end() )
    {
        to=*i;
        tie(e, exists) = edge(from, to, m_graph);
        assert(exists);
        weightmap[e]+=weightIncrement;
        from=to;
        ++i;
    };
}

//------------------------------------------------------------------------------

void LeeRoutePlannerImpl::FindShortestPath(
    grid_vertex_descriptor from,
    grid_vertex_descriptor to,
    grid_vertex_descriptor_vector &result)
{
    grid_vertex_descriptor_vector p(num_vertices(m_graph));
    std::vector<int> d(num_vertices(m_graph));

    result.clear();

    try
    {
        dijkstra_shortest_paths(
            m_graph, 
            from,
            boost::predecessor_map(&p[0]).distance_map(&d[0]).visitor(target_visit(to,boost::on_discover_vertex())));	
    }
    catch (ex_target_found& ex)
    {
        ex;	// all ok, we found target node
    }

/*
// go back via decreasing distance

    grid_vertex_descriptor first, next;

    first=to;
    next=to;
    
    while ( true )
    {
        next=first-1;
        if( next==from )
            break;
        if ( d[next]<d[first] )
        {
            result.push_back(next);
            first=next;
        }
        else
        {
            next=first+1;
            if ( next==from )
                break;
            if ( d[next]<d[first] )
            {
                result.push_back(next);
                first=next;
            }
            else
            {
                next=first+m_width+1;
                if ( next==from )
                    break;
                if ( d[next]<d[first] )
                {
                    result.push_back(next);
                    first=next;
                }
                else
                {
                    next=first-m_width-1;
                    if ( next==from )
                        break;
                    if ( d[next]<d[first] )
                    {
                        result.push_back(next);
                        first=next;
                    }
                    else
                    {
                        assert(0);
                    }
                }
            }
        }
    }

*/
    result.push_back(to);

    grid_vertex_descriptor parent=p[to];

    while( parent!=from )
    {
        result.push_back(parent);
        parent=p[parent];
    }

    result.push_back(from);
}

//------------------------------------------------------------------------------

grid_vertex_descriptor LeeRoutePlannerImpl::PointF2Descriptor( 
    const GraphTypes::PointF& point )
{
    CPoint pt;

    pt.x=int(point.x/m_gridbox.Width); // floor? speed? (i.e. float division)
    pt.y=int(point.y/m_gridbox.Height);

    pt.x+=abs(m_minX)+m_border;
    pt.y+=abs(m_minY)+m_border;

    return grid_vertex_descriptor(pt.y*(m_width+1) + pt.x);
}

//------------------------------------------------------------------------------

void LeeRoutePlannerImpl::Descriptor2PointF(
    grid_vertex_descriptor vd,
    GraphTypes::PointF& point )
{
    CPoint pt;

    pt.x=(int(vd))%(m_width+1);
    pt.y=(int(vd))/(m_width+1);

    pt.x-=abs(m_minX)+m_border;
    pt.y-=abs(m_minY)+m_border;

    point.x=m_gridbox.Width*pt.x;
    point.y=m_gridbox.Height*pt.y;
}

//-----------------------------------------------------------------------------

LeeRoutePlannerImpl::Direction LeeRoutePlannerImpl::GetDirection(
    grid_vertex_descriptor from,
    grid_vertex_descriptor to )
{
    Direction dir;
    int diff;

    diff=int(from-to);
    if ( diff==-1 || diff==1 )
        dir=dirHorz;
    else
        dir=dirVert;

    return dir;
}

//*****************************************************************************

CGraphLeeLayoutImpl::CGraphLeeLayoutImpl(IGraph * graph, CMeta & meta, CCanvas * canvas) : CLayout(graph, meta, canvas)
{
}

//-----------------------------------------------------------------------------

LAYOUTEVENT CGraphLeeLayoutImpl::pulse(const GraphTypes::RectF & extent)
{
    size_t verticy_count = m_meta.GetVisibleUnpinnedVertexCount(m_graph, m_canvas);

    if (verticy_count > 1)
    {
        GraphTypes::PointF vp;
        LeeRoutePlanner* plannerLee;

        // GraphTypes::SizeF gridSize;

        //gridSize.Width=32.0;
        //gridSize.Height=32.0;

        plannerLee=LeeRoutePlanner::Create(); //gridSize);
        assert(plannerLee);

        plannerLee->BeginAddPoints();

        for(StlLinked<IVertexIterator> itr = m_graph->GetUserVertexIterator(new MetaStateSkipper(m_meta, m_canvas, TRISTATE_TRUE, TRISTATE_FALSE)); itr->IsValidItem(); itr->NextItem())
        {
            CComPtr<IGraphVertex> vertex = itr->Get();
            CComPtr<IRenderer> ir=m_canvas->GetRenderer(vertex);
            ir->GetPosition(vp);
            plannerLee->AddPoint(vp, &vp);
            ir->SetPosition(vp);
        }

        plannerLee->EndAddPoints();

        IGraphEdgeVector edges;
        m_graph->GetEdges(edges);
        for (IGraphEdgeVector::iterator itr = edges.begin(); itr != edges.end(); ++itr)
        {
            IGraphEdge * edge = *itr;
            IGraphVertex * from = edge->GetSource();
            IGraphVertex * to = edge->GetTarget();
            GraphTypes::PointF start, end;
            GraphTypes::PointFVector path;
            CComPtr<IRenderer> irf=m_canvas->GetRenderer(from);
            CComPtr<IRenderer> irt=m_canvas->GetRenderer(to);
            CComQIPtr<IEdgeRenderer> ire=m_canvas->GetRenderer(edge);

            irf->GetPosition(start);
            irt->GetPosition(end);

            if ( plannerLee->GetShortestPath( 
                 start,
                 end,
                 path ) )
            {
                ire->SetPoints(path);
            }
        }

        plannerLee->Dispose();
    }

    return LAYOUTEVENT_FINISHED;
}

void CGraphLeeLayoutImpl::halt()
{
}

//-----------------------------------------------------------------------------

#ifdef _DEBUG
void LeeRoutePlannerImpl::Dump()
{
//	CUnicodeFile f;
//	TCHAR szbuff[128], szbuff1[64], szbuff2[64];

//	f.Create(_T("C:\\temp\\route.txt"));


//	f.Write(szbuff);
//	f.Write(_T("\n"));
}
#endif

//-----------------------------------------------------------------------------
#endif
