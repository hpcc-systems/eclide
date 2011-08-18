#pragma once

class CCommandManager
{
public:
	enum SIZE
	{
		SIZE_16,
		SIZE_24,
		SIZE_32,
		SIZE_48
	};

	enum COLOR_DEPTH
	{
		COLOR_DEPTH_16,
		COLOR_DEPTH_256,
		COLOR_DEPTH_XP
	};

protected:
	typedef std::map<int, CIcon> CommandIconMap;
	CommandIconMap m_commandList;

	typedef std::map<COLOR_DEPTH, CImageList> ColorDepthImageListMap;
	typedef std::map<SIZE, ColorDepthImageListMap> SizeColorDepthImageListMap;
	SizeColorDepthImageListMap m_imageLists;

public:
	CCommandManager();
	~CCommandManager();

	HICON SetCommandIcon(int command, ATL::_U_STRINGorID icon);
	void UpdateIcons(CMDICommandBarCtrl & cmdBar) const;
	void UpdateIcons(CToolBarCtrl & toolBar) const;
};
