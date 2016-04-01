// AMT.h : main header file for the AMT application
//
#pragma once

#ifndef __AFXWIN_H__
    #error "include 'stdafx.h' before including this file for PCH"
#endif

#include "..\en_us\resource.h"


// CAMTApp:
// See AMT.cpp for the implementation of this class
//

class CAMTApp : public CWinAppEx
{
public:
    CAMTApp();


// Overrides
public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();

// Implementation

public:
    UINT  m_nAppLook;
    virtual void PreLoadState();
    virtual void LoadCustomState();
    virtual void SaveCustomState();

    afx_msg void OnAppAbout();
    afx_msg void OnFileNew();
    DECLARE_MESSAGE_MAP()
};

extern CAMTApp theApp;
