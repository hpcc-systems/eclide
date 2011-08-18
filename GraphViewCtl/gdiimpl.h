#pragma once

#include "renderImpl.h"

class CGraphFastRenderImpl : public CRender
{
protected:
	CComPtr<IGraphRenderer> m_graphRender;
	bool m_calculateExtent;
public:
	CGraphFastRenderImpl(IGraph * graph, CMeta & meta);

	//  IRender Virtuals
	void append(IGraphSubgraph * g, const GraphTypes::RectF & r);
	void append(const CUniqueID & v, const GraphTypes::RectF & r);
	LRESULT SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam); 
	void SetTopParent(HWND hwnd);

};



