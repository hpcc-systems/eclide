#pragma once

#include <uxtheme.h> // MARGINS
#include "GraphTypes.h"

#ifndef DT_TRANSPARENT
#define DT_TRANSPARENT 0x01000000
#endif

class CAggDC;
struct CAggFonts;

//*****************************************************************************

struct FontInfo
{
private:
    CFont* const m_font;
    std::string m_typeface; // Agg takes ASCII typeface
    CAggFonts* const m_fontMgr;
    const GraphTypes::REAL m_height;
public:
    GraphTypes::Color m_color;

    FontInfo(
        CFont* font=0,
        GraphTypes::Color color=0,
        CAggFonts* fontMgr=0,
        GraphTypes::REAL height=10)
        :	m_fontMgr(fontMgr),
            m_font(font), 
            m_height(height),
            m_color(color),
            m_typeface("Verdana")
    {
        assert(font || fontMgr);

        if(font && fontMgr)
        {
            LOGFONT lf;
            m_font->GetLogFont(&lf);
            m_typeface=CT2A(&(lf.lfFaceName[0]));
        }

        if(color.GetValue()==0)
            m_color.SetFromCOLORREF(GetSysColor(COLOR_WINDOWTEXT));
    }

    friend struct CDrawVertexHelper;
};

//*****************************************************************************

struct CDrawVertexHelper
{
    const TCHAR* m_text;
    GraphTypes::REAL m_scale;
    GraphTypes::Color m_cbkgnd;
    GraphTypes::Color m_cborder;
    MARGINS m_margins;
    GraphTypes::REAL m_borderwidth;
    bool m_printing;
    bool m_focusRect;
    bool m_pins;

    CDrawVertexHelper();
    void Draw(CAggDC& dc, const FontInfo& font, const GraphTypes::RectF& rc, unsigned flags=0);
    void Draw(CAggDC& dc, const FontInfo& font, const GraphTypes::RectF& rc, unsigned flags, const TCHAR * text);
    void DrawFocusAndPin(CAggDC& dc, const GraphTypes::RectF& rc);
    GraphTypes::SizeF CalcSize(CDCHandle& dc, const FontInfo& font);
    GraphTypes::SizeF CalcSize(CDCHandle& dc, const FontInfo& font, const TCHAR * text);
};