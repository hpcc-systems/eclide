#pragma once

#include "GraphTypes.h"

struct CAggFonts;
class CCanvas;
class CRect;

class CAggDC : public CDC
{
public:
	virtual ~CAggDC() {};

	static CAggDC* CreateMemoryDC(CDC& dc, CAggFonts * fonts, CRect* rc=0, BYTE alpha=255);
	static CAggDC* CreateGDIDC(CDC& dc, CAggFonts * fonts, CRect* rc=0, BYTE alpha=255);
	static CAggDC* CreateBlitMemoryDC(CDC& dc, CAggFonts * fonts, HWND hWnd, CRect* rc=0, BYTE alpha=255);

	virtual void GetUpdateRect(CRect* rc)=0;
	virtual void SetAlpha(BYTE alpha)=0;
	virtual void TransformPush()=0;
	virtual void TransformPop()=0;
	virtual void Transform(GraphTypes::REAL scale)=0;
	virtual void Transform(const GraphTypes::PointF& offset)=0; 
	virtual void TransformReset()=0;
	virtual void BeginPathCache(unsigned npaths)=0;
	virtual void EndPathCache()=0; 
	virtual void SetGammaThreshold(double thr=-1)=0;

	virtual void Line(
		const GraphTypes::PointF& p1,
		const GraphTypes::PointF& p2, 
		const GraphTypes::Color& clr)=0;

	virtual void Lines(
		const GraphTypes::PointFVector& pts,
		const GraphTypes::Color& clr,
		const GraphTypes::REAL width=1.0)=0;

	virtual void Rect(
		const GraphTypes::PointF& tl, 
		const GraphTypes::PointF& br,
		const GraphTypes::Color& clrfill,
		const GraphTypes::Color& clroutline,
		GraphTypes::REAL outline_width=1.0,
		bool bAA=false)=0;

	virtual void SolidPolygon(
		const GraphTypes::PointF* points, 
		const GraphTypes::Color& clr,
		unsigned point_count)=0;

	virtual void SolidPolygon(
		const GraphTypes::PointF* points, 
		const GraphTypes::Color& clr,
		unsigned point_count,
		GraphTypes::REAL outline_width,
		const GraphTypes::Color& clroutline)=0;

	virtual void Bezier4(
		const GraphTypes::PointF& p1,
		const GraphTypes::PointF& p2,
		const GraphTypes::PointF& p3,
		const GraphTypes::PointF& p4,
		const GraphTypes::Color& clr,
		double width=1.0,
		bool bArrow=true,
		GraphTypes::REAL scale=1.0)=0;

	virtual void Bezier(
		const GraphTypes::PointFVector& points,
		const GraphTypes::Color& clr,
		double width=1.0,
		bool bArrow=true,
		bool bDashed=false,
		GraphTypes::REAL scale=1.0,
		const GraphTypes::Color& firstClrControlPoint = 0x00000000, 
		const GraphTypes::Color& lastClrControlPoint = 0x00000000)=0;

	virtual void LollyPop(
		const GraphTypes::RectF& rc, 
		const GraphTypes::Color& clr,
		const GraphTypes::Color& clroutline,
		bool bLeft)=0;

	virtual void LinearGradientRect(
		const GraphTypes::RectF& rc,
		const GraphTypes::Color& clrstart,
		const GraphTypes::Color& clrend,
		bool bHorizontal=true)=0;

#ifndef _WIN32_WCE
	virtual unsigned DrawScaledText(
		const TCHAR* text,
		const GraphTypes::RectF& rc,
		const GraphTypes::Color& clr,
		const char* font,
		int size,
		const GraphTypes::PointF & descsubtract)=0;
#endif

	virtual bool DrawBitmap(
		HBITMAP hBitmap,
		int x, 
		int y,
		int width,
		int height,
		int bmpWidth,
		int bmpHeight)=0;

	virtual bool DrawTransparent(
		HBITMAP hBitmap,
		int x, 
		int y,
		int width,
		int height,
		int bmpWidth,
		int bmpHeight,
		COLORREF color)=0;

	virtual int SetDIBitsToDevice(
		int x, 
		int y, 
		DWORD dwWidth, 
		DWORD dwHeight, 
		int xSrc, 
		int ySrc, 
		UINT uStartScan, 
		UINT cScanLines, 
		CONST VOID* lpvBits, 
		CONST BITMAPINFO* lpbmi, 
		UINT uColorUse)=0;

	virtual bool DrawAlphaBlend(
		int x, 
		int y, 
		int nWidth, 
		int nHeight, 
		HDC hSrcDC, 
		int xSrc, 
		int ySrc, 
		int nSrcWidth, 
		int nSrcHeight, 
		BLENDFUNCTION bf)=0;

	virtual HBITMAP DetachBitmap() = 0;

#ifndef _WIN32_WCE
	static GraphTypes::SizeF GetScaledTextExtent(
		const TCHAR* text,
		const char* font,
		CAggFonts* fonts,
		int size,
		GraphTypes::REAL scale=-1);
#endif
};
