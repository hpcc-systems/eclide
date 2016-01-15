#pragma once

#include "wlib.h"

class WLIB_API CShellContextMenu {
private: 
    HWND m_hwnd;
    IContextMenu * m_pcm;

public: 
    CShellContextMenu(HWND hwnd);
    void InsertTortoiseGitMenuItems(const std::_tstring & filePath, UINT menuIndex, WTL::CMenuHandle & m);
    void InvokeCommand(UINT iCmd, const CPoint & pt) const;
};
