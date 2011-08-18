///////////////////////////////////////////////////////////////////////////////
// ExacT Soft (tm) 2001														 //
// Author: Vsevolod Gromov email: gromov@eti.co.ru; site: www.ambap.narod.ru //
// CMemBm class. implements memory bitmap, is able to copy part of source	 //
// bitmap to MemBm, or copy source DC contents to MemBm (screen capture)	 //
// use _WIN32_WINDOWS for conditional compiles (necessary for proper Brush	 //
// origin adjustment operations	and proper transparent draw calls			 //
///////////////////////////////////////////////////////////////////////////////
#pragma once

//{ memory bitmap class CMemBm
class CMemBm
{
public:
	// create memory bitmap from source bitmap.
	// !REMEMBER! hbmSrc MUST NOT be selected into any DC before calling this function
	// or following will just create copy of hDC contents, not the hbmSrc.
	bool Create(HDC hDC, HBITMAP hbmSrc, int iLeft, int iTop,
		int iWidth, int iHeight, DWORD dwRop = SRCCOPY );
	// create memory bitmap from explicit data (default is monochrome bitmap)
	bool Create( HDC hDC, int iWidth, int iHeight,
		UINT cPlanes = 1, UINT cBitsPerPel = 1, const void *lpvBits = NULL );
	//copy creation
	bool Create( const CMemBm& bmSrc, DWORD dwRop = SRCCOPY );
	//create from bitmap resource by name
	bool Create( HINSTANCE hInst, LPCTSTR pszName );
	//create from bitmap resource by ID
	bool Create( HINSTANCE hInst, UINT nID ){ return Create( hInst, MAKEINTRESOURCE(nID) ); };

	// default constructor
	CMemBm(){ Initialize(); };
	// inplace constructor
	CMemBm(HDC hDC, HBITMAP hbmSrc, int iLeft, int iTop,
		int iWidth, int iHeight, DWORD dwRop = SRCCOPY )
	{
		Initialize();
		Create(hDC, hbmSrc, iLeft, iTop, iWidth, iHeight, dwRop);
	};
	//copy constructor
	CMemBm( const CMemBm& bmSrc, DWORD dwRop = SRCCOPY )
	{
		Initialize();
		Create(bmSrc, dwRop);
	};
	// create memory bitmap from explicit data (default is monochrome bitmap)
	CMemBm( HDC hDC, int iWidth, int iHeight,
			UINT cPlanes = 1, UINT cBitsPerPel = 1, const void *lpvBits = NULL )
	{
		Initialize();
		Create( hDC, iWidth, iHeight, cPlanes, cBitsPerPel, lpvBits );
	};
	//construct from resource by name
	CMemBm( HINSTANCE hInst, LPCTSTR pszName )
	{
		Initialize();
		Create(hInst, pszName);
	};
	//construct from resource by ID
	CMemBm( HINSTANCE hInst, UINT nID )
	{
		Initialize();
		Create(hInst, nID);
	};

	virtual ~CMemBm(){	Cleanup();	};

public:
	//cleanup object data
	void Cleanup();
	// have we been created (successfully)
	bool Created() { return m_bCreated; }
	//get bitmap width
	int GetWidth(void) const;
	//get bitmap height
	int GetHeight(void) const;
	// get bits per pixel for our bitmap
	int GetBpp(void);
	// get color planes for our bitmap
	int GetPlanes(void);
	// get the first pixel of the bitmap (transparent)
	COLORREF GetFirstPixel();
	//return our memory dc with bitmap selected in it
	//(Blt operations - ready)
	operator HDC() const { return m_hdcImg; };
	//KPD: added Detach method
	HBITMAP Detach();
	DWORD DetachToClipboard();
	//boolean validation operator !
	bool operator !(){ return !m_bCreated; };
	//perform pattern fill, adjusting brush origin if neccessary
	void Fill( HDC hDC, int iLeft, int iTop, int iWidth, int iHeight,
		bool bAdjustBrushOrg = false, DWORD dwRop = PATCOPY );
	//do 1-to-1 draw
	bool Draw( HDC hDC, int iLeft, int iTop, DWORD dwRop = SRCCOPY );
	//perform draw (simple or stretched)
	bool Draw( HDC hDC, int iLeft, int iTop, int iWidth, int iHeight,
		DWORD dwRop = SRCCOPY, int iStretchMode = COLORONCOLOR );
	//do transparent drawing using WinAPI or program emulated code
	bool DrawTrans( HDC hDC, int iLeft, int iTop, int iWidth, int iHeight,
				COLORREF crTransparent );
	bool DrawTrans2( HDC hDC, int iLeft, int iTop, int iWidth, int iHeight,
				COLORREF crTransparent );
	//do alpha blending draw (works only when _WIN32_WINDOWS > 0x0400 else do nothing)
	bool DrawAlpha( HDC hDC, int iLeft, int iTop, int iWidth, int iHeight,
		const BLENDFUNCTION& blendFunc );

//data members
protected:
	HANDLE	m_hbmOld;
	HBITMAP m_hbmImg;
	HDC		m_hdcImg;
	//bitmap dimensions
	int		m_iWidth;
	int		m_iHeight;
	// number of color planes in our bitmap
	int		m_iPlanes;
	// bits per pixel in our bitmap
	int		m_iBpp;
	//"object created" flag
	bool	m_bCreated;
	//reset data members to 0
	void Initialize(){	m_hbmOld = m_hbmImg = NULL;
		m_hdcImg = NULL; m_iWidth = m_iHeight = 0;
		m_iPlanes = m_iBpp = 0; m_bCreated = false; };
	//stretching required
	bool RequireStretch( int iWidth, int iHeight )
	{ return (bool)( (m_iWidth != iWidth) || (m_iHeight != iHeight) ); };
};
//} memory bitmap class CMemBm
