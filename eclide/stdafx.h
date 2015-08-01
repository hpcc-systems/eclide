// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

//  Target  ---
#include "stdversion.h"

//#define ENABLE_RIBBON_LAUNCH_BUTTON
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit
#define _ATL_IGNORE_NAMED_ARGS

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#include <afxdisp.h>        // MFC Automation classes

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC support for ribbons and control bars
#include <afxRegPath.h>

//  ATL  ---
#include "stdatl.h"

//  WTL  --
#define _WTL_NO_AUTOMATIC_NAMESPACE // Add this line so you won't get ambiguous symbol errors..
#define _WTL_NO_WTYPES
#define _WTL_NO_CSTRING
#define _CSTRING_NS
#include <atlapp.h>
extern WTL::CAppModule _Module;
#include "stdwtl.h"

//  WTLEX  ---
#include <exdisp.h>
#include <exdispid.h>

//  WTLEX  --
#include "atlwinmisc.h"
#include "atlgdix.h"
#include "CustomAutoComplete.h"
#include "DlgTabCtrl.h"
#include "dialogex.h"

//  Docking Tabs  --
#include "CustomTabCtrl.h"
#include "DotNetTabCtrl.h"
#include "TabbedFrame.h"

#define _TABBEDMDI_MESSAGES_EXTERN_REGISTER
#define _TABBEDMDI_MESSAGES_NO_WARN_ATL_MIN_CRT
#include "TabbedMDI.h"

#define _DEMO_TABBEDAUTOHIDEDOCKINGWINDOW_

#include <DWAutoHide.h>

#include <dbstate.h>
#include <DockingFrame.h>

#include <VC7LikeCaption.h>
#include <ExtDockingWindow.h>
#include <TabbedDockingWindow.h>

#include "CustomTabCtrl.h"
#include "TabbedFrame.h"
#include "TabbedMDI.h"
#include "DockMisc.h"
#include "ExtDockingWindow.h"
#include "DockingBox.h"
#include "TabDockingBox.h"
#include "DWAutoHide.h"
#include "DockingFrame.h"
#include "TabbedDockingWindow.h"
#include "sstate.h"
#include "SysToolbarCtrl.h"
#include "DotNetTabCtrl.h"

//// Scintilla  --
#define SCI_NAMESPACE
#include "platform.h"
#include "SciLexer.h"
#include "atlscintilla.h"

//  Other  --
#include "BevelLine.h"

//  To Be Removed  --
#include "PlainTextView.h"

//  STL  --
#include "stdstl.h"

//  BOOST --
#include "stdboost.h"

//  BugTrapper --
#include "BugTrap.h"

//  MFC Command Update Helper
#define	UPDATEUI(cui, id, val)	if (cui->m_nID == id){\
						cui->Enable(val);\
						return true;}

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


