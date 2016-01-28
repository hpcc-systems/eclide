#pragma once

#include "UniqueID.h"
#include "Iterators.h"
#include "GraphTypes.h"

// enum below is related to one in COMMS' Workunit.h
// but we have only 4 different color states,
// per GraphCache.cpp (server DOT creator code)

typedef std::map<int, CComVariant> PropertyMap;
typedef StlLinked<IUnknown> IUnknownAdapt;
typedef std::map<int, IUnknownAdapt> PropertyUnknownMap;

__interface IGraph;
__interface IGraphItem;
__interface IGraphSubgraph;
__interface IGraphVertex;
__interface IGraphEdge;
__interface IGraphItemIterator;
__interface ISubgraphIterator;
__interface IVertexIterator;
__interface IEdgeIterator;
__interface IVertexShortestPath;
__interface IVertexPath;
typedef StlLinked<IGraphItem> IGraphItemAdapt;
typedef std::vector<IGraphItemAdapt> IGraphItemVector;
typedef StlLinked<IGraphEdge> IGraphEdgeAdapt;
typedef std::vector<IGraphEdgeAdapt> IGraphEdgeVector;
typedef StlLinked<IGraphVertex> IGraphVertexAdapt;
typedef std::vector<IGraphVertexAdapt> IGraphVertexVector;
typedef StlLinked<IGraphSubgraph> IGraphSubgraphAdapt;
typedef std::vector<IGraphSubgraphAdapt> IGraphSubgraphVector;

enum WUVisualState 
{
    WUVisualState_unknown = 0,	//WUStateUnknown
    WUVisualState_running,	//WUStateCompiled, WUStateRunning, WUStateSubmitted, WUStateScheduled, WUStateCompiling, WUStateBlocked, WUStateWait
    WUVisualState_completed, //WUStateCompleted, WUStateArchived
    WUVisualState_failed,	//WUStateFailed, WUStateAborting, WUStateAborted
    WUVisualState_last	
};

const CUniqueID ROOT_ID(CCategory(guidDefault, 0), _T("root"));

__interface ISkipItem : public IUnknown
{
    bool SkipItem(IGraphItem * item) const;
};

__interface ISkipSubgraph : public IUnknown
{
    bool SkipSubgraph(IGraphSubgraph * sg) const;
};

__interface ISkipVertex : public IUnknown
{
    bool SkipVertex(IGraphVertex * v) const;
};

__interface ISkipEdge : public IUnknown
{
    bool SkipEdge(IGraphEdge * e) const;
};

enum GRAPHNOTIFICATION
{
    GRAPHNOTIFICATION_LABELCHANGED,
    GRAPHNOTIFICATION_SUBGRAPHADDED,
    GRAPHNOTIFICATION_SUBGRAPHREMOVED,
    GRAPHNOTIFICATION_VERTEXADDED,
    GRAPHNOTIFICATION_VERTEXREMOVED,
    GRAPHNOTIFICATION_EDGEADDED,
    GRAPHNOTIFICATION_EDGEREMOVED,
    GRAPHNOTIFICATION_LAST
};

#ifndef EXCLUDE_BOOST
typedef boost::signal<void(GRAPHNOTIFICATION modification, IGraphItem * item)> graph_signal_type;
typedef graph_signal_type::slot_type graph_slot_type;
#endif

__interface __declspec(uuid("A697F331-C28C-4ebc-94CC-5596D0123131")) IGraphItem : public IUnknown
{
    const CCategory & GetCategory() const;
    const CUniqueID & GetID() const;
    const TCHAR * GetLabel() const;
    void SetLabel(const TCHAR* label);

    IGraphSubgraph * GetParent() const;
    void SetParent(IGraphSubgraph* subgraph);

    void SetBorderColor(const GraphTypes::Color & newVal);
    const GraphTypes::Color & GetBorderColor();
    void SetFillColor(const GraphTypes::Color & newVal);
    const GraphTypes::Color & GetFillColor();
    void SetFontColor(const GraphTypes::Color & newVal);
    const GraphTypes::Color & GetFontColor();

    void SetVisible(VISIBLE visible);
    VISIBLE GetVisible() const;
    bool IsVisible() const;

    void SetImageVisible(IMAGE_POSITION pos, VISIBLE visible);
    VISIBLE GetImageVisible(IMAGE_POSITION pos) const;

    void GetProperty(int key, CComVariant& result) const;
    void SetProperty(int key, const CComVariant & val);
    void PropagateProperty(int key, const CComVariant & val);

    void GetProperties(IPropertyIDContainer* storage) const;

    IUnknown * GetPropertyUnknown(int key) const;
    void SetPropertyUnknown(int key, IUnknown * unknown);

    IUnknown * GetInternalUnknown(int key) const;
    void SetInternalUnknown(int key, IUnknown * unknown);

#ifndef EXCLUDE_BOOST
    boost::signals::connection Connect(const graph_slot_type& s);
    void Disconnect(boost::signals::connection& sc);
#endif
};

__interface __declspec(uuid("A697F331-C28C-4ebc-94CC-5596D0123132")) IGraphSubgraph : public IGraphItem
{
    //General
    void Delete(bool promoteChildren = false);
    unsigned int GetOrder() const;
    void MoveToEnd();
    void MoveToEnd(const IGraphItem * child);

    //Subgraph
    IGraphSubgraph * CreateSubgraph(const CUniqueID &id, const TCHAR* label);
    IGraphSubgraph * GetSubgraph(const CUniqueID & id) const;
    IGraphSubgraph * GetSubgraph(unsigned int hash) const;

    void SetImage(const ImageInfo& iinfo, int overlay=0);
    const ImageInfoMaps* GetImages() const;
    void SetImageSize(IMAGE_POSITION pos, unsigned ovidx, const CSize& size);
    void GetImageSize(IMAGE_POSITION pos, unsigned ovidx, CSize& size) const;

    bool IsChild(IGraphSubgraph * subgraph) const;
    unsigned int GetChildren(IGraphItemVector & results) const;
    unsigned int GetChildren(IGraphItemVector & results, ISkipItem * callback_func) const;
    IGraphItemIterator * GetChildren();
    IGraphItemIterator * GetChildren(ISkipItem * callback_func);
    IGraphItemIterator * GetRChildren(ISkipItem * callback_func);
    void SetChildIndex(const IGraphItem * child, unsigned int pos);
    unsigned int GetChildIndex(const IGraphItem * child) const;
    IGraphItem * GetChild(unsigned int pos) const;
    void RemoveChild(const IGraphItem * child);
    void AttachItem(IGraphItem* vertex);
    void DetachItem(IGraphItem* vertex);

    //Vertex
    IGraphVertex * CreateVertex(const CUniqueID &id);
    IGraphVertex * GetVertex(const CUniqueID & id) const;
    IGraphVertex * GetVertex(unsigned int hashID) const;
    bool IsChild(IGraphVertex * vertex) const;

    //Edge
    IGraphEdge * GetEdge(const CUniqueID & id) const;
    IGraphEdge * GetEdge(unsigned int hash) const;

    //Signal Helper
    void Signal(GRAPHNOTIFICATION notification);

    //Hash Map Helper
    int GetCacheID() const;
};

typedef StlLinked<IGraphSubgraph> IGraphSubgraphAdapt;

__interface IPositionHelper
{
    void GetPosition(IGraphVertex * v, double & x, double & y);
    void SetPosition(IGraphVertex * v, double x, double y);
};

__interface IXGMMLRenderCallback;

__interface __declspec(uuid("A697F331-C28C-4ebc-94CC-5596D0123130")) IGraph : public IGraphSubgraph
{
    //  General
    //void Lock(CReadLockAdapt & lock);
    bool Load(const std::_tstring & xgmml, const IXGMMLRenderCallback * callback, bool redundantSubgraphs, bool passthroughVertices);
    bool Merge(const std::_tstring & xgmml, const IXGMMLRenderCallback * callback, bool appendMissing, bool redundantSubgraphs, bool passthroughVertices);

    bool HasItems();

    unsigned int GetAllChildren(IGraphItemVector & results) const;

    //  Subgraph Iterator
    unsigned int GetSubgraphs(IGraphSubgraphVector & results) const;
    ISubgraphIterator * GetSubgraphIterator() const;
    ISubgraphIterator * GetUserSubgraphIterator(ISkipSubgraph * callback_func) const;

    //  Vertex
    //void Randomize(unsigned vCount, unsigned eCount);
    unsigned int GetVertexCount() const;
    IVertexShortestPath * CalculateShortestPaths(const CUniqueID & from);

    //  Vertex Iterators
    IVertexIterator * GetVertexIterator() const;
    IVertexIterator * GetUserVertexIterator(ISkipVertex * callback_func) const;

    //  Edge
    IGraphEdge * CreateEdge(const CUniqueID & id, IGraphVertex * from, IGraphVertex * to);
    IGraphEdge * CreateEdge(const CUniqueID & id, const CUniqueID & from, const CUniqueID & to);
    unsigned int GetEdgeCount() const;

    //  Edge Iterator
    unsigned int GetEdges(IGraphEdgeVector & results) const;
    unsigned int GetEdges(IGraphEdgeVector & results, ISkipEdge * callback_func) const;
    IEdgeIterator * GetEdgeIterator();
    IEdgeIterator * GetUserEdgeIterator(ISkipEdge * callback_func);

#if BOOST_VERSION >= 103300
    //  Layouts
    void circleLayout(int radius);
    void fruchterman_reingold_force_directed_layout(IPositionHelper * posHelper, double width, double height, int iterations = 100);
#endif
};

__interface __declspec(uuid("A697F331-C28C-4ebc-94CC-5596D0123133")) IGraphVertex : public IGraphItem
{
    void Delete();

    unsigned int GetOrder() const;
    void SetOrder(unsigned int pos);
    void MoveToEnd();

    void SetImage(const ImageInfo& iinfo, int overlay=0);
    const ImageInfoMaps* GetImages() const;
    void SetImageSize(IMAGE_POSITION pos, unsigned ovidx, const CSize& size);
    void GetImageSize(IMAGE_POSITION pos, unsigned ovidx, CSize& size) const;

    //  Vertex Iterators
    unsigned int GetAdjacentInVertices(IGraphVertexVector & results) const;
    unsigned int GetAdjacentOutVertices(IGraphVertexVector & results) const;
    unsigned int GetAdjacentVertices(IGraphVertexVector & results) const;

    //  Edge ---
    unsigned int GetEdgeCount() const;
    unsigned int GetInEdgeCount() const;
    unsigned int GetOutEdgeCount() const;
    //  Edge Iterators
    unsigned int GetAdjacentInEdges(IGraphEdgeVector & results) const;
    unsigned int GetAdjacentOutEdges(IGraphEdgeVector & results) const;
    unsigned int GetAdjacentEdges(IGraphEdgeVector & results) const;
    IEdgeIterator * GetAdjacentInEdges();
    IEdgeIterator * GetAdjacentOutEdges();
    IEdgeIterator * GetAdjacentEdges();

    //Signal Helper
    void Signal(GRAPHNOTIFICATION notification);

    //Hash Map Helper
    int GetCacheID() const;
};

__interface __declspec(uuid("A697F331-C28C-4ebc-94CC-5596D0123134")) IGraphEdge : public IGraphItem
{
    void Delete();

    void CopyProperties(PropertyMap & props) const;
    void PasteProperties(const PropertyMap & props);

    void CopyPropertyUnknowns(PropertyUnknownMap & props) const;
    void PastePropertyUnknowns(const PropertyUnknownMap & props);

    IGraphVertex * GetSource();
    IGraphVertex * GetTarget();
    void SetTarget(IGraphVertex * target);
    IGraphVertex * GetAdjacentVertex(IGraphVertex * from);

    void SetWeight(const double &weight);

    void SetColor(const GraphTypes::Color & newVal);
    const GraphTypes::Color & GetColor() const;

    void SetImage(const ImageInfo& iinfo, int overlay=0);
    const ImageInfoMaps* GetImages() const;

    //Hash Map Helper
    int GetCacheID() const;

#ifndef EXCLUDE_BOOST
    void SetDateRange(const boost::posix_time::time_period & timeRange);
    const boost::posix_time::time_period & GetDateRange();
#endif
};

typedef StlLinked<IGraphEdge> IGraphEdgeAdapt;

__interface IIterator : public IUnknown
{
    bool IsValidItem();
    void NextItem();
};

__interface IGraphItemIterator : public IIterator
{
    IGraphItem * Get();
};

__interface ISubgraphIterator : public IIterator
{
    IGraphSubgraph * Get();
};

__interface IVertexIterator : public IIterator
{
    IGraphVertex * Get();
};

__interface IEdgeIterator : public IIterator
{
    IGraphEdge * Get();
};

__interface IVertexShortestPath : public IUnknown
{
    IVertexPath * CalculateShortestRoute(IGraphVertex * v);
    IVertexPath * CalculateShortestRoute(const CUniqueID & id);
};

__interface IVertexPath : public IUnknown
{
    bool ContainsStep(IGraphVertex * u, IGraphVertex * v);
};


CLIB_API IGraph * CreateDirectedGraph();
CLIB_API IGraph * CreateDirectedGraph(ISkipEdge * edgeFilter, ISkipVertex * vertexFilter, ISkipSubgraph * subgraphFilter);
CLIB_API IGraph * CreateUndirectedGraph();