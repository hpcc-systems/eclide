#include "StdAfx.h"

#include "ImageHelper.h"

#include "cximage\ximage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//  ===========================================================================

HBITMAP ToGrayScale(HICON icon)
{
	CxImage ximg_gr;
	ximg_gr.CreateFromHICON(icon);
	ximg_gr.GrayScale();
	return ximg_gr.MakeBitmap();
}

//  ===========================================================================

HBITMAP Modify(HICON icon, int iBrightess = 0, int iContrast = 75);
HBITMAP Modify(HICON icon, int iBrightess, int iContrast)
{
		CxImage ximg_ht;
		ximg_ht.CreateFromHICON(icon);

		RGBQUAD oldColor,newColor,tmpColor;

		//this will be a little tricky:
		//oldColor = ximg_ht.GetPixelColor(0,0); // first.. get first pixel color (used as transparent one)
		ximg_ht.Light(iBrightess, iContrast); //then change contrast 75% more...
		//newColor = ximg_ht.GetPixelColor(0,0); // get new color
		//for(long y=0;y< (long)ximg_ht.GetHeight();y++) //and replace it!
		//{
		//	for(long x=0;x<(long)ximg_ht.GetWidth();x++)
		//	{
		//		tmpColor = ximg_ht.GetPixelColor(x,y);
		//		if ((tmpColor.rgbBlue == newColor.rgbBlue)&&
		//			(tmpColor.rgbGreen == newColor.rgbGreen) &&
		//			(tmpColor.rgbRed == newColor.rgbRed))
		//		{
		//			ximg_ht.SetPixelColor(x,y,oldColor);
		//		}
		//	}
		//}
		//imgList_ht.Add(ximg_ht.MakeBitmap(), CLR_NONE);
		return ximg_ht.MakeBitmap();
}