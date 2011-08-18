#pragma once

class CGraphViewCanvas;
interface IGraphItem;

namespace QBUI
{
	static const unsigned ID_XGMMLUPDATETIMER=2100;

	void LaunchFindDialog(CGraphViewCanvas* canvas);
	void CreateNodePopup(HWND hwndOwner, const CPoint& pos, IGraphItem* item);
	void UpdateNodePopups();
	void DeleteNodePopups();
};
