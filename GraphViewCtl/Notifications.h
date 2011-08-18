#pragma once

#include "GraphContainer.h" // IGraphItem

#define SCW_NOTIFY_FIRST			(0U-3300U)
#define SCW_NOTIFYSELECTED			(SCW_NOTIFY_FIRST-1)
#define SCW_NOTIFYITEMSCONNECTED	(SCW_NOTIFY_FIRST-2)
#define SCW_NOTIFYLABELEDITED		(SCW_NOTIFY_FIRST-3)
#define SCW_NOTIFYRECALCEDGEPOINTS	(SCW_NOTIFY_FIRST-4)
#define SCW_NOTIFYFOCUSCHANGED		(SCW_NOTIFY_FIRST-5)
#define SCW_NOTIFYVISIBILITY    	(SCW_NOTIFY_FIRST-6)
#define SCW_NOTIFYDODRAGDROP    	(SCW_NOTIFY_FIRST-7)
#define SCW_NOTIFYVERTEXCONTEXT    	(SCW_NOTIFY_FIRST-8)
#define SCW_NOTIFYSUBGRAPHCONTEXT  	(SCW_NOTIFY_FIRST-9)
#define SCW_NOTIFYGRAPHCONTEXT  	(SCW_NOTIFY_FIRST-10)
#define SCW_NOTIFYLAYOUTFINISHED  	(SCW_NOTIFY_FIRST-11)
#define SCW_NOTIFYHOTCHANGED		(SCW_NOTIFY_FIRST-12)
#define SCW_NOTIFYSCROLL			(SCW_NOTIFY_FIRST-13)
#define SCW_NOTIFY_TIMELINE_RCLICK	(SCW_NOTIFY_FIRST-14)
#define SCW_NOTIFY_TIMELINE_DBLCLK	(SCW_NOTIFY_FIRST-15)
#define SCW_NOTIFYMOUSEMESSAGE    	(SCW_NOTIFY_FIRST-16)
#define SCW_NOTIFYEDGECONTEXT    	(SCW_NOTIFY_FIRST-17)
#define SCW_NOTIFYSCALECHANGE    	(SCW_NOTIFY_FIRST-18)
#define SCW_NOTIFYMINCLICKED	  	(SCW_NOTIFY_FIRST-19)
#define SCW_NOTIFYCALCTOOLTIP		(SCW_NOTIFY_FIRST-20)
#define SCW_NOTIFYTOOLTIPLINKCLICK	(SCW_NOTIFY_FIRST-21)

// selection change notification structure
typedef struct tagNMSGVHDR 
{
	NMHDR hdr;
	CUniqueID id;
} NMSGVHDR, *LPNMSGVHDR;

// selection change notification structure
typedef struct tagNMNOTIFYCALCTOOLTIP
{
	NMHDR hdr;
	CUniqueID id;
	bool supressTooltip;
} NMNOTIFYCALCTOOLTIP , *LPNMNOTIFYCALCTOOLTIP;

// selection change notification structure
typedef struct tagNMSELECTIONCHANGE 
{
	NMHDR hdr;
	CUniqueID idSelect;
	CUniqueID idDeselect;
} NMSELECTIONCHANGE , *LPNMSELECTIONCHANGE;

// visibility change notification structure
typedef struct tagNMVISIBILITYCHANGE 
{
	NMHDR hdr;
	CUniqueID id;
} NMVISIBILITYCHANGE , *LPNMVISIBILITYCHANGE;

// items connected notification structure
typedef struct tagNMITEMCONN 
{
	NMHDR hdr;
	CUniqueID idfrom;
	CUniqueID idto;
} NMITEMCONN , *LPNMITEMCONN;

// item's label edited notification structure
typedef struct tagNMLABELEDITED 
{
	NMHDR hdr;
	CUniqueID		parentid;
	CUniqueID		id;
	std::_tstring	newlabel;
} NMLABELEDITED , *LPNMLABELEDITED;

// recalculate points for edges to avoid obstacles notification structure
typedef struct tagNMRECALCEDGEPOINTS 
{
	NMHDR hdr;
} NMRECALCEDGEPOINTS , *LPNMRECALCEDGEPOINTS;

// item got focus
typedef struct tagNMFOCUSCHANGED 
{
	NMHDR hdr;
	CUniqueID id;
	bool bGained;
} NMFOCUSCHANGED , *LPNMFOCUSCHANGED;

typedef struct tagNMDODRAGDROP
{
	NMHDR hdr;
	CPoint dragStartPos;
} NMDODRAGDROP , *LPNMDODRAGDROP;

// item context clicked
typedef struct tagNMCONTEXTMENU 
{
	NMHDR hdr;
	CUniqueID id;
	CPoint pt;
} NMCONTEXTMENU, *LPNMCONTEXTMENU;

// mouse message
typedef struct tagNMMOUSEMESSAGE 
{
	NMHDR hdr;
	UINT uMsg;
	CUniqueID id;
	CPoint pt;
} NMMOUSEMESSAGE, *LPNMMOUSEMESSAGE;

// canvas has been scrolled
typedef struct tagNMSCROLL
{
	NMHDR hdr;
	CPoint pt;
} NMSCROLL, *LPNMSCROLL;

// right mouse click time notification structure
typedef struct tagNMTIMELINERCLK 
{
	NMHDR hdr;
	SYSTEMTIME time;
	CPoint pt;
} NMTIMELINERCLK, *LPNMTIMELINERCLK;

// double click time notification structure
typedef struct tagNMTIMELINEDBLCLK 
{
	NMHDR hdr;
	SYSTEMTIME begin, end;
} NMTIMELINEDBLCLK, *LPNMTIMELINEDBLCLK;

// tooltip hyperlink clicked
typedef struct tagNMTOOLTIPLINKCLICK
{
	NMHDR hdr;
	CUniqueID target;
	std::_tstring definition;
	std::_tstring copy;
} NMTOOLTIPLINKCLICK, *LPNMTOOLTIPLINKCLICK;
