// QueryBuilder.h : main header file for the QueryBuilder application
//
#pragma once

#ifndef __AFXWIN_H__
    #error "include 'stdafx.h' before including this file for PCH"
#endif

#include "..\en_us\resource.h"       // main symbols

// CQueryBuilderApp:
// See QueryBuilder.cpp for the implementation of this class
//

class CQueryBuilderApp : public CWinAppEx
{
public:
    CQueryBuilderApp();


// Overrides
public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();

// Implementation
protected:
    HMENU  m_hMDIMenu;
    HACCEL m_hMDIAccel;

public:
    UINT  m_nAppLook;
    BOOL  m_bHiColorIcons;

    virtual BOOL PumpMessage() override;

    virtual void PreLoadState();
    virtual void LoadCustomState();
    virtual void SaveCustomState();

    afx_msg void OnAppAbout();
    DECLARE_MESSAGE_MAP();

};

extern CQueryBuilderApp theApp;
