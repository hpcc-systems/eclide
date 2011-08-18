#include "StdAfx.h"

#include "Canvas.h"
#include "ScaledWindows.h"



//*****************************************************************************



//*****************************************************************************

CScaledWindow::CScaledWindow(CCanvas* canvas, CScaledWindow* parent/*=0*/)
	: m_canvas(canvas),
	m_bFocus(false),
	m_bSelected(false),
	m_bRedraw(true),
	m_parent(parent),
	m_style(0),
	m_rcWnd(0,0,0,0),
	m_rcClient(0,0,0,0)
{
}

//-----------------------------------------------------------------------------

CScaledWindow::~CScaledWindow()
{
	if(m_canvas->GetCapture()==this)
		m_canvas->ReleaseCapture();
	if(m_canvas->GetKbdHook()==this)
		m_canvas->HookKbdMsgs(0);
	if(IsSelected())
		SetSelected(CUniqueID(), false);
	if(HasFocus())
		SetFocus(false);
}

//-----------------------------------------------------------------------------

LRESULT CScaledWindow::SendMessage(UINT uMsg, WPARAM wParam/*=0*/, LPARAM lParam/*=0*/)
{
	LRESULT lResult=0;
	ProcessWindowMessage(0, uMsg, wParam, lParam, lResult, 0);
	return lResult;
}

//-----------------------------------------------------------------------------

void CScaledWindow::SetFocus(bool bSet)
{ 
	if(m_bFocus!=bSet)
	{
		m_bFocus=bSet;

		if(bSet)
		{
			//ATLASSERT(m_canvas->m_currentFocus!=this);
			// There are cases where we get focus twice which
			// are difficult to solve correctly until we get
			// notifications that mouse has left canvas (for ex.
			// because of context menu click in repository tree)
			if(m_canvas->m_currentFocus!=this)
			{
				if(m_canvas->m_currentFocus)
					m_canvas->m_currentFocus->SetFocus(false);
				m_canvas->m_currentFocus=this;
			}
		}
		else
		{
			//ATLASSERT(m_canvas->m_currentFocus==this);
			m_canvas->m_currentFocus=0;
		}
	}
}

//-----------------------------------------------------------------------------

void CScaledWindow::SetSelected(
	const CUniqueID& id, bool bSelected/*=true*/, bool bAdd/*=false*/)
{
	if(m_bSelected!=bSelected)
	{
		m_bSelected=bSelected;
		CCanvas::WindowList::iterator itr=m_canvas->m_selectedList.begin();

		if(bSelected)
		{
			if(!bAdd) 
			{
				// deselect all others and remove them from the list
				while(itr!=m_canvas->m_selectedList.end())
				{
					itr->wnd->m_bSelected=false;
					itr->wnd->Invalidate();
					itr=m_canvas->m_selectedList.erase(itr);
				}
			}

			// add ourselves
			CCanvas::WndGlue gl;
			gl.wnd=this;
			gl.id=id;
			m_canvas->m_selectedList.push_back(gl);
		}
		else
		{
			// find ourself, deselect and remove from the list
			while(itr!=m_canvas->m_selectedList.end())
			{
				if(itr->wnd==this)
				{
					itr->wnd->m_bSelected=false;
					m_canvas->m_selectedList.erase(itr);
					break;
				}
				else
				{
					++itr;
				}
			}
		}

		Invalidate();
	}
	else
	{
		if(bSelected)
		{
			if(!bAdd) 
			{
				CCanvas::WindowList::iterator itr=m_canvas->m_selectedList.begin();

				// deselect everybody and clear the list
				while(itr!=m_canvas->m_selectedList.end())
				{
					itr->wnd->m_bSelected=false;
					itr->wnd->Invalidate();
					itr=m_canvas->m_selectedList.erase(itr);
				}

				CCanvas::WndGlue gl;
				gl.wnd=this;
				gl.id=id;

				// add ourselves
				m_bSelected=bSelected;
				m_canvas->m_selectedList.push_back(gl);
			}
		}
	}
}

//-----------------------------------------------------------------------------

void CScaledWindow::SetRedraw(bool bEnable/*=true*/, bool bInvalidate/*=false*/)
{
	if(bEnable!=m_bRedraw)
	{
		m_bRedraw=bEnable;
		if(bEnable && bInvalidate)
			Invalidate();
	}
}

//-----------------------------------------------------------------------------

void CScaledWindow::SetWindowPos(const CRect& pos, UINT flags)
{ 
	CRect rcold=m_rcWnd;

	if(flags&SWP_NOSIZE)
	{
		// calling us with no move and no size makes no sense but,
		// for now we have CalcDisplaySizes invoked before each paint
		// and it invokes us, so return for now
		if(flags&SWP_NOMOVE)
			return;

		ATLASSERT(!(flags&SWP_NOMOVE));
		CPoint offset=pos.TopLeft()-m_rcWnd.TopLeft();
		m_rcWnd.OffsetRect(offset);
	}
	else
	{
		if(flags&SWP_NOMOVE)
		{
			m_rcWnd.right=m_rcWnd.left+pos.Width();
			m_rcWnd.bottom=m_rcWnd.top+pos.Height();			
		}
		else
		{
			m_rcWnd=pos;
		}
	}

	m_rcClient=m_rcWnd;

	if(!(flags&SWP_NOREDRAW))
	{
		/* TODO enable this when moving items will invalidate edges
		if(m_rcWnd.IsRectEmpty())
		{
			Invalidate(); // didn't cache the rectangle so invalidate everything
		}
		else
		{
			CRect rcunion;
			rcunion.UnionRect(m_rcWnd, rcold);
			Invalidate(&rcunion, true);
		}
		*/
		Invalidate();
	}
}

//*****************************************************************************

CButtonState::~CButtonState()
{
	if(m_canvas && m_canvas->m_currentHot == this)
		m_canvas->m_currentHot = NULL;
}

bool CButtonState::SetHot(bool bHot, BUTTON_STATE newstate/*=BS_NORMAL*/)
{
	bool bChange=false;

	if(bHot)
	{
		if(m_canvas->m_currentHot!=this)
		{
			if(m_canvas->m_currentHot)
			{
				m_canvas->m_currentHot->SetHot(false);
			}
			bChange=m_state!=BS_HOT;
			m_state=BS_HOT;
			m_canvas->m_currentHot=this;
		}
	}
	else
	{
		bChange=m_state!=newstate;
		m_state=newstate;

		if(m_canvas->m_currentHot==this)
			m_canvas->m_currentHot=0;
	}

	if(bChange && m_parent)
	{
		CRect rc;
		m_parent->GetWindowRect(rc);
		m_parent->Invalidate(&rc);
	}

	return bChange;
}

//*****************************************************************************
CPartTheme::CPartTheme()
{
}

CPartTheme::CPartTheme(const std::_tstring & part)
	: m_id(part)
{
	std::wstring widePart = CT2W(part.c_str(), CP_UTF8);
	OpenThemeData(NULL, widePart.c_str());
}

CPartTheme::~CPartTheme()
{
	CloseThemeData();
}

//-----------------------------------------------------------------------------

const TCHAR * CPartTheme::GetID() const
{
	return m_id.c_str();
}

//-----------------------------------------------------------------------------

const TCHAR * CPartTheme::GetCacheID() const
{
	return m_id.c_str();
}

//-----------------------------------------------------------------------------

void CPartTheme::NotifyThemeChanged()
{
	CloseThemeData();
	OpenThemeData(NULL, CT2W(m_id.c_str()));
}

//*****************************************************************************

CCaptionFont::CCaptionFont(int size)
{
	bool bThemeSuccess=false;
	LOGFONTW lf;

/*
	CPartTheme& theme=canvas->GetPartTheme(_T("WINDOW"));

	if(!theme.IsThemeNull())
	{
		if(SUCCEEDED(theme.GetThemeSysFont(TMT_SMALLCAPTIONFONT,&lf)))
			bThemeSuccess=true;
	}
*/

	if(!bThemeSuccess)
	{
		NONCLIENTMETRICS nmm;
		nmm.cbSize=sizeof(NONCLIENTMETRICS);

		::SystemParametersInfo(
			SPI_GETNONCLIENTMETRICS,
			sizeof(NONCLIENTMETRICS),
			&nmm,
			0);

		memcpy(&lf,&nmm.lfSmCaptionFont,sizeof(LOGFONT));	
	}

	CreateFont(
			size,					   // nHeight
			lf.lfWidth,                // nWidth
			lf.lfEscapement,           // nEscapement
			lf.lfOrientation,          // nOrientation
			lf.lfWeight,               // nWeight
			lf.lfItalic,               // bItalic
			lf.lfUnderline,            // bUnderline
			lf.lfStrikeOut,            // cStrikeOut
			lf.lfCharSet,              // nCharSet
			lf.lfOutPrecision,         // nOutPrecision
			lf.lfClipPrecision,        // nClipPrecision
			lf.lfQuality,              // nQuality
			lf.lfPitchAndFamily,	   // nPitchAndFamily
			CW2T(lf.lfFaceName));      // lpszFacename
	
	ATLASSERT(!IsNull());
	m_size = size;
}

//-----------------------------------------------------------------------------

int CCaptionFont::GetCacheID() const
{
	return m_size;
}

//*****************************************************************************

CListFont::CListFont(int size)
{
	CreateFont(
			size,				       // nHeight
			0,			               // nWidth
			0,		                   // nEscapement
			0,                         // nOrientation
			FW_NORMAL,                 // nWeight
			FALSE,                     // bItalic
			FALSE,                     // bUnderline
			0,                         // cStrikeOut
			ANSI_CHARSET,              // nCharSet
			OUT_DEFAULT_PRECIS,        // nOutPrecision
			CLIP_DEFAULT_PRECIS,       // nClipPrecision
			DEFAULT_QUALITY,           // nQuality
			DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
			abs(size) <= 7 ? _T("Small Fonts") : _T("Verdana"));            // lpszFacename

	ATLASSERT(!IsNull());
	m_size = size;
}

//-----------------------------------------------------------------------------

int CListFont::GetCacheID() const
{
	return m_size;
}

//*****************************************************************************
