#pragma once

#include <atltheme.h>

#include "clib.h"
#include "GraphTypes.h"
#include "Transform.h"

class CCanvas;
class CUniqueID;

typedef std::map<DpiScale, GraphTypes::SizeF> SizeFMap;
typedef std::map<DpiScale, GraphTypes::CRectEx> CRectMap;
typedef std::map<DpiScale, GraphTypes::CSizeEx> CSizeMap;
typedef std::map<DpiScale, GraphTypes::PointF> PointFMap;

#define ID_SCROLLTIMER 2000
#define SCROLLTIMERTICK 100 // 100 msec

// Scaled listbox notification 
#define LBN_LABELEDIT		WM_USER+100
// Scaled windows message
#define SWM_CHILDRENCHANGE	WM_USER+101

enum HIT_TEST_CODE
{
	HT_UNKNOWN,
	HT_BBOX,
	HT_CAPTION,
	HT_MAXBUTTON,
	HT_MINBUTTON,
	HT_CLOSEBUTTON,
	HT_SIZEGRIP,
	HT_CONTAINER,
	HT_LISTBOX,
	HT_SBUPBTN,
	HT_SBLEFTBTN,
	HT_SBDOWNBTN,
	HT_SBRIGHTBTN,
	HT_SBUPTRACK,
	HT_SBDOWNTRACK,
	HT_SBLEFTTRACK,
	HT_SBRIGHTTRACK,
	HT_SBVERTTHUMB,
	HT_SBHORZTHUMB
};

enum WINDOW_SIZE
{
	WINDOW_SIZE_MIN,
	WINDOW_SIZE_NORM,
	WINDOW_SIZE_MAX
};

enum BORDER_STYLE
{
	NONE,
	LINE,
	WINDOW_FRAME,
	DIALOG_FRAME,
	CLIENT_RAISED,
	CLIENT_SUNKEN,
};

enum CAPTION_STYLE
{
	CS_STANDARD,
	CS_SMALL
};

// really bitflags 
enum CONTROL_STYLES
{
	NO_MARGIN=1, // no margin in client rectangle
};

enum WINDOW_STATE
{
	WS_ACTIVE=1, // reuse of tmschema.h enums which start with 1
	WS_INACTIVE
};

enum BUTTON_STATE
{
	BS_NORMAL=1, // reuse of tmschema.h enums
	BS_HOT,
	BS_PUSHED,
	BS_DISABLED
};

//*****************************************************************************

class CScaledWindow	: public CMessageMap, public CUnknown
{
	friend class CCanvas;
protected:
	bool m_bFocus;
	bool m_bSelected;
	bool m_bRedraw;
	CScaledWindow * m_parent;
	CCanvas * m_canvas;
	int m_style;
	CRect m_rcWnd;
	CRect m_rcClient;

public:
	BEGIN_CUNKNOWN
	END_CUNKNOWN(CUnknown)

	CScaledWindow(CCanvas* canvas, CScaledWindow* parent=0);
	virtual ~CScaledWindow();

	int GetStyle(void) const { return m_style; }

	virtual void SetFocus(bool bSet=true);
	bool HasFocus() const { return m_bFocus; }

	void SetSelected(const CUniqueID& id, bool bSelected=true, bool bAdd=false);
	bool IsSelected(void) const { return m_bSelected; }

	void SetRedraw(bool bEnable=true, bool bInvalidate=false);
	bool IsRedraw() { return m_bRedraw; }

	void SetParent(CScaledWindow* parent) { m_parent=parent; }
	CScaledWindow* GetParent() const { return m_parent; }

	virtual void Invalidate(CRect* rc=0, bool bErase=false) 
	{
		(rc);
		(bErase);
	}

	virtual void SetWindowPos(const CRect& pos, UINT flags);
	void GetClientRect(CRect& rc) const { rc=m_rcClient; }
	void GetWindowRect(CRect& rc) const { rc=m_rcWnd; }

	LRESULT SendMessage(UINT uMsg, WPARAM wParam=0, LPARAM lParam=0);
};

//*****************************************************************************

struct CButtonState
{
	BUTTON_STATE m_state;
	CScaledWindow* m_parent;
	// no need for StlLinked here, CButtonState is always CScaledWindow member
	CCanvas* m_canvas; 

	CButtonState(CCanvas* canvas, CScaledWindow* parent=0)
		: m_canvas(canvas),
		m_state(BS_NORMAL),
		m_parent(parent) 
	{}

	~CButtonState();

	bool SetHot(bool bHot, BUTTON_STATE newstate=BS_NORMAL);
	bool IsHot() const { return m_state==BS_HOT; }
};

//*****************************************************************************

class CPartTheme
	: public CUnknown,
	public CTheme
{
public:
	CPartTheme();
	CPartTheme(const std::_tstring & part);
	~CPartTheme();

	const TCHAR * GetID() const;
	const TCHAR * GetCacheID() const;
	void NotifyThemeChanged();

private:
	std::_tstring m_id;
};

//*****************************************************************************

class CCaptionFont
	: public CUnknown,
	public CFont
{
public:
	CCaptionFont(int size);
	int GetCacheID() const;

private:
	int m_size;
};


//*****************************************************************************

class CListFont
	: public CUnknown,
	public CFont
{
public:
	CListFont(int size);
	int GetCacheID() const;

private:
	int m_size;
};

//*****************************************************************************
