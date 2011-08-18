#pragma once

#include "graphtypes.h"

// IPicture and hBitmap
CLIB_API IPicture * GetIPictureFromBitmap(HBITMAP hBitmap);
CLIB_API int GetFirstPixel(HBITMAP hBitmap, COLORREF& firstPixel);
CLIB_API GraphTypes::IBitmap* GetIBitmapFromIHTMLElement(IHTMLElementRender* htmlElementRender);
CLIB_API GraphTypes::IBitmap* GetIBitmapFromIPicture(IPicture* ipic);

