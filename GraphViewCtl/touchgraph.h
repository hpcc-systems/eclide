#pragma once

#include "layoutImpl.h"
#include "GraphTypes.h"
#include "Thread.h"

class CGraphTouchgraphLayoutImpl : public CLayout
{
private:
    //  Vertex Properties
    std::map<CUniqueID, GraphTypes::PointF> m_dv;
//	std::map<CUniqueID, bool> m_justMadeLocal;
    std::map<CUniqueID, int> m_repulsion;

    //  Edge Properties
    //  Can't use edge_descriptor as a persistent key...
    //std::map<EdgeTuple, float> m_length;

    clib::thread *m_thread;

    float damper;
    float maxMotion;
    float lastMaxMotion;
    float motionRatio;
    bool damping;
    float rigidity;
    float newRigidity;
    bool m_halt;

    GraphTypes::RectF m_extent;

public:
    CGraphTouchgraphLayoutImpl(IGraph * graph, CMeta & meta, CCanvas * canvas);
    ~CGraphTouchgraphLayoutImpl();
    virtual void init();
    virtual void append(IGraphItem * item, const GraphTypes::PointF & p);
    virtual void remove(IGraphItem * item);
    void setRigidity(float r);
    void attractiveForce(IGraphItem * u, IGraphItem * v);
    void relaxEdges();
    void applyForce(IGraphItem * u, IGraphItem * v);
    void avoidLabels();
    void avoidLabels2();
    void startDamper();
    void stopDamper();
    void resetDamper();
    void stopMotion();
    void damp();
    void moveNodes();
    void relax();
    LAYOUTEVENT pulse(const GraphTypes::RectF & extent);
    void halt();
    virtual void setPoint(IGraphItem * item, const GraphTypes::PointF & p);
};
