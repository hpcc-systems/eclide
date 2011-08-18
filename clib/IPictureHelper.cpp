#include "StdAfx.h"

#include "ipicturehelper.h"
#include "membm.h"

CLIB_API IPicture * GetIPictureFromBitmap(HBITMAP hBitmap)
{
	PICTDESC pd;
	pd.cbSizeofstruct=sizeof(PICTDESC);
	pd.picType=PICTYPE_BITMAP;
	pd.bmp.hbitmap=hBitmap;
	pd.bmp.hpal=NULL;
	IPicture* iPicture = NULL;
	if (SUCCEEDED(OleCreatePictureIndirect(&pd,IID_IPicture,TRUE,(LPVOID*)&iPicture)))
		return iPicture;
	// failed
	DeleteObject((HGDIOBJ)hBitmap);
	return NULL;
}


// returns 16, 24, 32 bit depth
CLIB_API int GetFirstPixel(HBITMAP hBitmap, COLORREF& firstPixel)
{
	BITMAP bmp;
	if (::GetObject(hBitmap, sizeof(bmp), &bmp) && bmp.bmBits)
	{
		if (bmp.bmBitsPixel > 16)
		{
			unsigned offset=(((bmp.bmWidth*bmp.bmBitsPixel/8)+3)/4) * 4 * (bmp.bmHeight-1);
			firstPixel = *(COLORREF*)((BYTE*)bmp.bmBits + offset);
			// in memory it is laid out as BGR, so convert to RGB...
			firstPixel = RGB(GetBValue(firstPixel), GetGValue(firstPixel), GetRValue(firstPixel));
			return bmp.bmBitsPixel;
		}
		else
		{
			ATLASSERT(bmp.bmBitsPixel==16);
			// we need a temporary memory bitmap to paint into.
			CClientDC dcc(0);
			CMemBm bmpFirstPixel(dcc, 0, 0, 0, 1, 1);
			CDCHandle hdcH = bmpFirstPixel;

			HBITMAP hbmpold=dcc.SelectBitmap(hBitmap);
			hdcH.BitBlt(0, 0, 1, 1, dcc, 0, 0, SRCCOPY);
			dcc.SelectBitmap(hbmpold);

			// get trans color 
			firstPixel = bmpFirstPixel.GetFirstPixel();
			return bmp.bmBitsPixel;
		}
	}
	return 0;
}

CLIB_API GraphTypes::IBitmap* GetIBitmapFromIHTMLElement(IHTMLElementRender* htmlElementRender)
{
	CDC dc;

	{
		CClientDC dcc(0);
		dc.CreateCompatibleDC(dcc);
	}

	CComQIPtr<IHTMLElement> htmlElement = htmlElementRender;

	long height, width;
	htmlElement->get_offsetHeight(&height);
	htmlElement->get_offsetWidth(&width);

	void* buf;
	BITMAPINFO bmp_info; 
	bmp_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
	bmp_info.bmiHeader.biWidth = width; 
	bmp_info.bmiHeader.biHeight = height; 
	bmp_info.bmiHeader.biPlanes = 1; 
	bmp_info.bmiHeader.biBitCount = 32; 
	bmp_info.bmiHeader.biCompression = BI_RGB; 
	bmp_info.bmiHeader.biSizeImage = 0; 
	bmp_info.bmiHeader.biXPelsPerMeter = 0; 
	bmp_info.bmiHeader.biYPelsPerMeter = 0; 
	bmp_info.bmiHeader.biClrUsed = 0; 
	bmp_info.bmiHeader.biClrImportant = 0; 

	HBITMAP hbmp=0;
	hbmp = ::CreateDIBSection( 
		dc.m_hDC, 
		&bmp_info, 
		DIB_RGB_COLORS, 
		&buf, 
		0, 
		0 
		); 

	ATLASSERT(hbmp);

	GraphTypes::IBitmap* ibmp=0;

	if(hbmp && dc.m_hDC)
	{
		HBITMAP oldbmp=dc.SelectBitmap(hbmp);
		// NOTE: this fill should be irrelevent since image gets rendered next
		// It would fail on Vista anyway, since transcolor=0x00d3d3d3 now.
		dc.FillSolidRect(0, 0, width, height, RGB(0,0,211)); // default to trans color (GIF) for web (ASSUMPTION)
		//dc.FillSolidRect(0, 0, width, height, GetSysColor(COLOR_WINDOW));
		HRESULT hr=htmlElementRender->DrawToDC(dc.m_hDC);
		ATLASSERT(SUCCEEDED(hr));
		dc.SelectBitmap(oldbmp);
		ibmp=GraphTypes::CreateIBitmap(hbmp);
	}

	return ibmp;
}

CLIB_API GraphTypes::IBitmap* GetIBitmapFromIPicture(IPicture* ipic)
{
	GraphTypes::IBitmap* ibmp=0;
	if (!ipic)
		return ibmp;

	CDC dc;

	{
		CClientDC dcc(0);
		dc.CreateCompatibleDC(dcc);
	}

	OLE_YSIZE_HIMETRIC height;
	OLE_XSIZE_HIMETRIC width;
	ipic->get_Height(&height);
	ipic->get_Width(&width);

	CSize nsize(width, height);
	// convert OLE size into pixels
	dc.HIMETRICtoDP(&nsize);

	void* buf;
	BITMAPINFO bmp_info; 
	bmp_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
	bmp_info.bmiHeader.biWidth = nsize.cx; 
	bmp_info.bmiHeader.biHeight = nsize.cy; 
	bmp_info.bmiHeader.biPlanes = 1; 
	bmp_info.bmiHeader.biBitCount = 32; 
	bmp_info.bmiHeader.biCompression = BI_RGB; 
	bmp_info.bmiHeader.biSizeImage = 0; 
	bmp_info.bmiHeader.biXPelsPerMeter = 0; 
	bmp_info.bmiHeader.biYPelsPerMeter = 0; 
	bmp_info.bmiHeader.biClrUsed = 0; 
	bmp_info.bmiHeader.biClrImportant = 0; 

	HBITMAP hbmp=0;
	hbmp = ::CreateDIBSection( 
		dc.m_hDC, 
		&bmp_info, 
		DIB_RGB_COLORS, 
		&buf, 
		0, 
		0 
		); 

	ATLASSERT(hbmp);

	if(hbmp && dc.m_hDC)
	{
		HBITMAP oldbmp=dc.SelectBitmap(hbmp);
		HRESULT hr = ipic->Render(
			dc.m_hDC,
			0,
			nsize.cy,
			nsize.cx,
			-nsize.cy,
			0,
			0,
			width,
			height,
			0);

		ATLASSERT(SUCCEEDED(hr));
		dc.SelectBitmap(oldbmp);
		ibmp=GraphTypes::CreateIBitmap(hbmp);
	}

	return ibmp;
}
