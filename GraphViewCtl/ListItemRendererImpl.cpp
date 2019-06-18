
#include "StdAfx.h"
#include "Canvas.h"

IRenderer* CreateListItemRenderer(CCanvas & canvas)
{
    IRenderer* ir=CreateTextboxRenderer(canvas);
    if(ir)
    {
        ITextboxRenderer* itr=static_cast<ITextboxRenderer*>(ir);
        itr->SetIOMode(ITextboxRenderer::iomBoth);
    }
    return ir;
}
