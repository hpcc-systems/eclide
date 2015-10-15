#pragma once

#include "layoutImpl.h"

class CGraphTimelineLayoutImpl : public CLayout
{
public:
    CGraphTimelineLayoutImpl(IGraph * graph, CMeta & meta, CCanvas * canvas);

    LAYOUTEVENT pulse(const GraphTypes::RectF & extent);
    void halt();
};

