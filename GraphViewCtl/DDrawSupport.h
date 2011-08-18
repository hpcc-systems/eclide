#pragma once

#ifdef __HAVE_DIRECTX_HEADERS__

#include <ddraw.h>

class CDDrawSupport
{
	// Surface description

	class SurfaceDesc
	{
	public:
		SurfaceDesc()
		{
			memset(&m_srfDesc, 0, sizeof (DDSURFACEDESC));
			m_srfDesc.dwSize = sizeof (DDSURFACEDESC);
		}

		DDSURFACEDESC* operator&() { return &m_srfDesc; }

		void SetCapabilities(DWORD caps)
		{
			m_srfDesc.dwFlags |= DDSD_CAPS;
			m_srfDesc.ddsCaps.dwCaps = caps;
		}

		void SetDimensions(int width, int height)
		{
			m_srfDesc.dwFlags |= (DDSD_WIDTH | DDSD_HEIGHT);
			m_srfDesc.dwWidth = width;
			m_srfDesc.dwHeight = height;
		}

		int Height() const { return m_srfDesc.dwHeight; }
		int Width() const { return m_srfDesc.dwWidth; }
		int Pitch() const { return m_srfDesc.lPitch; }
		void* Buffer() const { return m_srfDesc.lpSurface; }
		DDPIXELFORMAT& PixelFormat () { return m_srfDesc.ddpfPixelFormat; }
	private:
		DDSURFACEDESC	m_srfDesc;
	};

	// Pixel format

	class PixelFormat
	{
	public:
		PixelFormat() {}
		PixelFormat(SurfaceDesc & desc)
		{
			Init (desc);
		}
		void Init(SurfaceDesc& desc);
		int BitsPp() const { return m_bpp; }
	private:
		int				m_bpp; // bits per pixel 4, 8, 16, 24, or 32
	};

	// Clipper for blitting

	class Clipper: public CComPtr<IDirectDrawClipper>
	{
	public:
		Clipper(CDDrawSupport& draw) 
		{
			HRESULT res = draw->CreateClipper(0, &p, 0);
			if (res != DD_OK)
				throw _T("Cannot create clipper");
		}

		void SetHWnd (HWND hwnd)
		{
			p->SetHWnd(0, hwnd);
		}
	};

	// Surface (base class)

	class SurfaceBuf; // forward for friend

	class Surface: public CComPtr<IDirectDrawSurface>
	{
		friend class SurfaceBuf;
	protected:
		// Locking and unlocking the whole surface
		void Lock(SurfaceDesc& desc)
		{
			ATLASSERT(p != 0);
			HRESULT res;
			do {

				res = p->Lock(0, &desc, 0, 0);
			} while(res == DDERR_WASSTILLDRAWING);

			if(res != DD_OK) 
				throw _T("Cannot lock surface");
		}

		void Unlock()
		{
			ATLASSERT(p != 0);
			p->Unlock(0); 
		}

	public:
		Surface() {}

		void SetClipper(IDirectDrawClipper* clipper)
		{
			ATLASSERT(p != 0);
			HRESULT res = p->SetClipper(clipper);
			if(res != DD_OK) 
				throw _T("Cannot set clipper");
		}

		void GetDescription(SurfaceDesc& desc)
		{
			HRESULT res = p->GetSurfaceDesc (&desc);
			if(res != DD_OK) 
				throw _T("Cannot get surface description");
		}

		void BltFrom(Surface& src, CRect* dstRect=0, CRect* srcRect=0)
		{
			ATLASSERT(p != 0);
			CRect rcsrc;
			if(srcRect==0)
			{
				rcsrc.left=rcsrc.top=0;
				rcsrc.right=dstRect->Width();
				rcsrc.bottom=dstRect->Height();
				srcRect=&rcsrc;
			}

			//HRESULT res = p->BltFast(dstRect->left, dstRect->top, src.p, srcRect, DDBLTFAST_NOCOLORKEY);
			HRESULT res = p->Blt(dstRect, src.p, srcRect, 0, 0);

			if(res==DDERR_INVALIDPARAMS)
				ATLASSERT(!_T("Invalid parameters"));
			if(res==DDERR_INVALIDRECT)
				ATLASSERT(!_T("Invalid rectangle"));
			if(res==DDERR_UNSUPPORTED)
				ATLASSERT(!_T("Not supported on this surface"));
			if(res != DD_OK) 
				throw _T("Cannot perform a blt");
		}

		void Fill(COLORREF color, CRect* rc);
	};

	// Surface buffer for direct memory access

	class SurfaceBuf
	{
	public:
		SurfaceBuf(Surface& surface)
			: m_surface (surface), m_buf(0), m_pitch(0)
		{
			m_dds2=surface;
			SurfaceDesc desc;
			surface.Lock(desc);
			if(m_dds2)
				m_dds2->PageLock(0);
			m_format.Init(desc);
			int bpp = m_format.BitsPp();
			if(bpp != 32)
			{
				surface.Unlock();
				if(m_dds2)
					m_dds2->PageUnlock(0);
				throw _T("Only true color supported");
			}
			else
			{
				m_pitch = desc.Pitch();
				m_buf = static_cast<unsigned char *>(desc.Buffer());
			}
		}

		~SurfaceBuf ()
		{
			m_surface.Unlock();
			if(m_dds2)
				m_dds2->PageUnlock(0);
		}

		void* Buffer() const { return m_buf; }
		int Pitch() const { return m_pitch; }
	private:
		Surface& m_surface;
		unsigned char* m_buf;
		int			m_pitch;
		PixelFormat	m_format;
		CComQIPtr<IDirectDrawSurface2, &IID_IDirectDrawSurface2> m_dds2;
	};

	// Primary surface representing the screen

	class PrimarySurface: public Surface
	{
	public:
		PrimarySurface() {}
		PrimarySurface(CDDrawSupport& draw) { Init(draw); }
		void Init(CDDrawSupport& draw)
		{
			SurfaceDesc desc;
			desc.SetCapabilities(DDSCAPS_PRIMARYSURFACE);
			HRESULT res = draw->CreateSurface(&desc, &p, 0);
			if(res != DD_OK) 
				throw _T("Cannot create primary surface");
		}
	};

	// Off-screen surface

	class OffScreenSurface: public Surface
	{
	public:
		OffScreenSurface() {}
		OffScreenSurface(CDDrawSupport& draw, int width, int height) { Init(draw, width, height); }

		void Init(CDDrawSupport& draw, int width, int height)
		{
			SurfaceDesc desc;
			DWORD caps=DDSCAPS_OFFSCREENPLAIN + DDSCAPS_VIDEOMEMORY + DDSCAPS_LOCALVIDMEM;
			//DWORD caps=DDSCAPS_OFFSCREENPLAIN + DDSCAPS_SYSTEMMEMORY;
			desc.SetCapabilities(caps);
			desc.SetDimensions(width, height);
			HRESULT res = draw->CreateSurface(&desc, &p, 0);
			if(res != DD_OK)
				throw _T("Cannot create off-screen surface");
		}
	};

	struct StaticData
	{
		HMODULE m_hDLL;
		CComPtr<IDirectDraw> m_pDraw;
		DDCAPS m_caps;
		PrimarySurface* m_prim;
		OffScreenSurface* m_off;
		int m_width, m_height; 

		StaticData() : m_hDLL(0), m_prim(0), m_off(0), m_width(0), m_height(0)
		{
			m_hDLL=::LoadLibrary(_T("ddraw.dll"));
			if(m_hDLL)
			{
				typedef HRESULT (__stdcall *pDDrawCreate)(GUID*, LPDIRECTDRAW*, IUnknown*);
				pDDrawCreate pDDC=(pDDrawCreate)::GetProcAddress(m_hDLL, "DirectDrawCreate");
				if(pDDC)
				{
					HRESULT res = pDDC(0, &m_pDraw.p, 0);
					if(res == DD_OK) 
					{
						m_caps.dwSize=sizeof m_caps;
						res=m_pDraw->GetCaps(&m_caps, 0);
						if(res != DD_OK || !(m_caps.ddsCaps.dwCaps & DDSCAPS_OFFSCREENPLAIN))
						{
							ATLASSERT(!_T("Cannot get Direct Draw caps"));
							m_pDraw=0;
						}
					}
					else
					{
						ATLASSERT(!_T("Cannot create Direct Draw object"));
					}
				}
			}
		}

		~StaticData()
		{
			delete m_off;
			delete m_prim;
			m_pDraw=0;
			if(m_hDLL)
				::FreeLibrary(m_hDLL);
		}
	};

public:
	CDDrawSupport(void);
	~CDDrawSupport(void);

	bool IsAvailable() const;
	IDirectDraw* operator->() { return m_static.m_pDraw; }
	void* GetSurface(HWND hWnd, int width, int height, int& pitch);
	void Blt(CRect* dstRect);
	HDC GetDC();
	void ReleaseDC(HDC hDC);
	void Cleanup();
private:
	void SetCoopNormal(HWND h);

	static StaticData m_static;
	bool m_allOk;
	SurfaceBuf * m_buf;
	CWindow m_wnd;
};

#endif