#pragma once

#include "layoutImpl.h"

class CGraphNullLayoutImpl : public CLayout
{
public:
    CGraphNullLayoutImpl(IGraph * graph, CMeta & meta, CCanvas * canvas) : CLayout(graph, meta, canvas)
    {
    }

    LAYOUTEVENT pulse(const GraphTypes::RectF & extent)
    {
        return LAYOUTEVENT_FINISHED;
    }

    void halt()
    {
    }
};

