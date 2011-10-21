#pragma once

enum AMTPANE	//  See "arrParts" in mainfrm.cpp
{
	AMTPANE_DEFAULT = ID_DEFAULT_PANE,
	AMTPANE_STATUS,
	AMTPANE_PROGRESS,
	AMTPANE_LAST
};
void PostStatus(const TCHAR* pStr, AMTPANE pane = AMTPANE_DEFAULT); 
void PostProgress(int progress); 
