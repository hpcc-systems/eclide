#pragma once

#include "GraphTypes.h"

class CAggDC;
struct CAggFonts;
class CRect;

#ifndef DT_TRANSPARENT
#define DT_TRANSPARENT 0x01000000
#endif

//*****************************************************************************

class CAggBitmap
{
    CComPtr<GraphTypes::IBitmap> m_ibmp;
#ifdef _DEBUG
    bool started;
public:
    void GetSize(CSize& size) { m_ibmp->GetSize(size); }
#endif

public:
    CAggBitmap();
    ~CAggBitmap();

    void Clear();
    CAggDC* BeginPaint(CAggDC& dc, const CSize& size, CAggFonts* fonts);
    void EndPaint(CAggDC* dc);
    bool IsEmpty() const;
    void Draw(CAggDC& dc, const CRect& rc, unsigned flags);
};
