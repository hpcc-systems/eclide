#include "StdAfx.h"

//NOTE - if defined, has to be defined in AggMemoryDC.cpp too
#ifdef __HAVE_DIRECTX_HEADERS__

#include "DDrawSupport.h"

CDDrawSupport::StaticData CDDrawSupport::m_static;

//-----------------------------------------------------------------------------

CDDrawSupport::CDDrawSupport(void)
 : m_allOk(false), m_buf(0)
{
	m_allOk=m_static.m_pDraw!=0;
}

//-----------------------------------------------------------------------------

CDDrawSupport::~CDDrawSupport(void)
{
}

//-----------------------------------------------------------------------------

bool CDDrawSupport::IsAvailable() const
{
	return m_allOk;
}

//-----------------------------------------------------------------------------

void CDDrawSupport::SetCoopNormal(HWND h)
{
	HRESULT res = m_static.m_pDraw->SetCooperativeLevel(h, DDSCL_NORMAL);
	if(res != DD_OK) 
		throw _T("Cannot set normal cooperative level");
}

//-----------------------------------------------------------------------------

void* CDDrawSupport::GetSurface(HWND hWnd, int width, int height, int& pitch)
{
	void* psurf=0;

	if(m_allOk && width!=0 && height!=0)
	{
		try
		{
			SetCoopNormal(hWnd);
			m_wnd.Attach(hWnd);

			if(m_static.m_off==0 ||
				width>m_static.m_width ||
				height>m_static.m_height)
			{
				if(m_static.m_off!=0)
					delete m_static.m_off;
				m_static.m_off = new OffScreenSurface(*this, width, height);
				if(m_static.m_off==0)
					throw _T("Could not create offscreen surface");
				m_static.m_width=width;
				m_static.m_height=height;
			}

			if(!(m_static.m_caps.dwCaps & DDCAPS_BLTCOLORFILL)) 
				throw _T("No color fill");

			CRect rc(0,0,width,height);
			m_static.m_off->Fill(GetSysColor(COLOR_WINDOW), &rc);
			m_buf = new SurfaceBuf(*m_static.m_off);
			pitch = m_buf->Pitch();
			psurf = m_buf->Buffer();

			if(m_static.m_prim==0)
				m_static.m_prim = new PrimarySurface(*this);

			Clipper clip(*this);
			clip.SetHWnd(hWnd);
			m_static.m_prim->SetClipper(clip.p);

		}
		catch(TCHAR*)
		{
			m_allOk=false;
			psurf=0;
			Cleanup();
		}
	}

	return psurf;
}

//-----------------------------------------------------------------------------

void CDDrawSupport::Blt(CRect* dstRect)
{
	if(m_allOk)
	{
		try
		{
			delete m_buf;
			m_buf=0;
			m_wnd.ClientToScreen(dstRect);
			m_static.m_prim->BltFrom(*m_static.m_off, dstRect);
			m_static.m_prim->SetClipper(0);	
		}
		catch(TCHAR*)
		{
			m_allOk=false;
			Cleanup();
		}
	}
}

//-----------------------------------------------------------------------------

HDC CDDrawSupport::GetDC()
{
	HDC hdc;
	HRESULT res;

	do {
		res = m_static.m_off->p->GetDC(&hdc);
	} while (res == DDERR_WASSTILLDRAWING);

	if (res != DD_OK)
	{
		m_allOk=false;
		throw _T("Cannot get DC from surface");
	}
	return hdc;
}

//-----------------------------------------------------------------------------

void CDDrawSupport::ReleaseDC(HDC hDC)
{
	m_static.m_off->p->ReleaseDC(hDC);
}

//*****************************************************************************

void CDDrawSupport::PixelFormat::Init(SurfaceDesc& desc)
{
	DDPIXELFORMAT& format = desc.PixelFormat();
	if (format.dwFlags != DDPF_RGB && format.dwFlags != (DDPF_RGB+DDPF_ALPHAPIXELS))
		throw _T("Direct Draw: Non-RGB formats not supported");
	m_bpp = format.dwRGBBitCount;
	if(m_bpp!=32)
		throw _T("Only 32 bit depth is supported");
}

//-----------------------------------------------------------------------------

void CDDrawSupport::Surface::Fill(COLORREF color, CRect* rc)
{
	ATLASSERT(p!=0);
	DDBLTFX bltFx;
	bltFx.dwSize = sizeof (DDBLTFX);
	bltFx.dwFillColor = color;

	HRESULT res = p->Blt(rc, 0, 0, DDBLT_COLORFILL, &bltFx);
	if(res != DD_OK) 
		throw _T("Cannot perform a blt");
}

//-----------------------------------------------------------------------------

void CDDrawSupport::Cleanup()
{
	delete m_buf;
	m_buf=0;

	delete m_static.m_off;
	m_static.m_off=0;

	delete m_static.m_prim;
	m_static.m_prim=0;
}

#endif #__HAVE_DIRECTX_HEADERS__
