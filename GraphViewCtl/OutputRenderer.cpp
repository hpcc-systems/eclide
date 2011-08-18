#include "StdAfx.h"
#include "Canvas.h"

IRenderer* CreateOutputRenderer(CCanvas& canvas)
{
	IRenderer* ir=CreateTextboxRenderer(canvas);
	if(ir)
	{
		ITextboxRenderer* itr=static_cast<ITextboxRenderer*>(ir);
		itr->SetIOMode(ITextboxRenderer::iomOut);
	}
	return ir;
}
