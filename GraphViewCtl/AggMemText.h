#pragma once

#include "GraphTypes.h"

class CCanvas;
class CRect;
class CAggDC;

#ifndef DT_TRANSPARENT
#define DT_TRANSPARENT 0x01000000
#endif

//*****************************************************************************

class CAggMemText
{
    GraphTypes::SizeF m_size;
    GraphTypes::SizeF m_scaledSize;
    GraphTypes::REAL m_scale;
    int m_dpix, m_dpiy;
    MARGINS m_Margins;
    CComPtr<GraphTypes::IBitmap> m_ibmp;
    std::string m_fontName;

public:
    CAggMemText(const char* font="Verdana");
    ~CAggMemText();

    GraphTypes::SizeF GetSize() const { return m_size; }
    GraphTypes::SizeF GetScaledSize() const { return m_scaledSize; }
    void CalcSize(const TCHAR* text, CCanvas* canvas);
    void SetMargins(const MARGINS& margins) { m_Margins=margins; }
    void Clear();

    void Prepare(
        CAggDC& dc, 
        const TCHAR* text, 
        GraphTypes::Color ctext,
        GraphTypes::Color cbkgnd,
        GraphTypes::Color cborder,
        GraphTypes::REAL lwidth,
        CCanvas* canvas);

    void Draw(CAggDC& dc, const CRect& rc, unsigned flags);
};