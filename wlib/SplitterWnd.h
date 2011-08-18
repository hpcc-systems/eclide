#pragma once

template<bool t_bVertical = true>
class CMySplitterWindow : public WTL::CSplitterWindowImpl<CMySplitterWindow<t_bVertical>, t_bVertical>
{
public:
	DECLARE_WND_CLASS_EX(_T("WTL_MySplitterWindow"), CS_DBLCLKS, COLOR_WINDOW)

	void DrawSplitterBar(WTL::CDCHandle dc)
	{
		RECT rect;
		if(GetSplitterBarRect(&rect))
		{
			dc.FillRect(&rect, afxGlobalData.brBarFace);
			// draw 3D edge if needed
			if((GetExStyle() & WS_EX_CLIENTEDGE) != 0)
				dc.DrawEdge(&rect, EDGE_RAISED, false ? (BF_LEFT | BF_RIGHT) : (BF_TOP | BF_BOTTOM));
		}
	}
};
