#include "stdafx.h"

#include "AggDC.h"
#include "AggCommon.h"
#include "DrawVertexHelper.h"

using namespace GraphTypes;

//-----------------------------------------------------------------------------

CDrawVertexHelper::CDrawVertexHelper()
    : 	m_text(0),
        m_scale(1.0),
        m_borderwidth(0.0),
        m_printing(false),
        m_focusRect(false),
        m_pins(false)
{
    m_cbkgnd.SetFromCOLORREF(GetSysColor(COLOR_WINDOW));
    m_cborder=m_cbkgnd;
    memset(&m_margins, 0, sizeof MARGINS);
}

//-----------------------------------------------------------------------------

void CDrawVertexHelper::Draw(CAggDC& dc, const FontInfo& font, const RectF& rc, unsigned flags)
{
    RectF pos = rc;
    std::_tstring text = m_text;
    typedef boost::tokenizer<boost::char_separator<TCHAR>, std::_tstring::const_iterator, std::_tstring> tokenizer;
    boost::char_separator<TCHAR> sep(_T("\r\n"));
    tokenizer tokens(text, sep);
    int line_count = 0;
    for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter)
        ++line_count;

    pos.Height = rc.Height / line_count;
    for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter)
    {
        Draw(dc, font, pos, flags, tok_iter->c_str());
        pos.y += (pos.Height);
    }
}

void CDrawVertexHelper::Draw(CAggDC& dc, const FontInfo& font, const GraphTypes::RectF& rc, unsigned flags, const TCHAR * text)
{
    if(text==0 || rc.IsEmptyArea())
        return;

    REAL border=m_borderwidth*m_scale;

    if(!m_printing)
    {
        assert(font.m_fontMgr);
        RectF rctext(rc);

        if(flags&DT_CENTER)
        {
            SizeF extent=CAggDC::GetScaledTextExtent(
                text,
                font.m_typeface.c_str(), 
                font.m_fontMgr,
                round_int(font.m_height));

            extent.Height*=m_scale;
            extent.Width*=m_scale;

            if(rc.Height>extent.Height)
                rctext.Inflate(0, (extent.Height-rc.Height)/2);
            if(rc.Width>extent.Width)
                rctext.Inflate((extent.Width-rc.Width)/2, 0);
        }

        if(flags&DT_TRANSPARENT)
        {
            //TODO: when there is no border rounding problems leave parts of
            //rectangle unpainted at high (200% and more) zoom levels. Thus -0.5.
            REAL halfBorder;
            if(border>0)
                halfBorder=border/2;
            else
                halfBorder=0.5;

            PointF tl(rc.x-halfBorder, rc.y-halfBorder);
            PointF br(rc.x+rc.Width+halfBorder, rc.y+rc.Height+halfBorder);
            dc.Rect(
                tl, 
                br,
                m_cbkgnd,
                m_cborder,
                border,
                true);
        }
        else
        {
            //TODO: when there is no border rounding problems leave parts of
            //rectangle unpainted at high (200% and more) zoom levels. Thus -0.5.
            REAL halfBorder;
            if(border>0)
                halfBorder=border/2;
            else
                halfBorder=0.5;

            PointF tl(rctext.x-halfBorder, rctext.y-halfBorder);
            PointF br(rctext.x+rctext.Width+halfBorder, rctext.y+rctext.Height+halfBorder);

            dc.Rect(
                tl, 
                br,
                m_cbkgnd,
                m_cborder,
                border,
                true);
        }

        RectF rcinner(rctext);
        rcinner.Inflate(
            -REAL(m_margins.cxLeftWidth)*m_scale,
            -REAL(m_margins.cyTopHeight)*m_scale);

        dc.TransformPush();
        dc.TransformReset();
        dc.Transform(m_scale);

        // For some reason 2(3,3.5?) needs to be subtracted from descent to get a good centering
        PointF descsubtract(REAL(0), REAL(3));
        dc.DrawScaledText(
            text, 
            rcinner,
            font.m_color,
            font.m_typeface.c_str(), 
            round_int(font.m_height),
            descsubtract);

        dc.TransformPop();
    }
    else
    {
        int isave=dc.SaveDC();
        CDCHandle hdc(dc.m_hDC);
        CAutoFont f(*font.m_font, hdc);
        CRect frameRc=rc;

        dc.SetTextColor(font.m_color.ToCOLORREF());
        dc.SetBkColor(m_cbkgnd.ToCOLORREF());

        if(flags&DT_TRANSPARENT)
        {
            dc.SetBkMode(TRANSPARENT);
        }
        else
        {
            if(border>0.1)
            {
                CPen borderPen;
                borderPen.CreatePen(PS_SOLID, 1, m_cborder.ToCOLORREF());
                CBrush fillBrush;
                fillBrush.CreateSolidBrush(m_cbkgnd.ToCOLORREF());

                CAutoPen p(borderPen, hdc);
                CAutoBrush b(fillBrush, hdc);

                dc.Rectangle(&frameRc);
            }
        }

        CRect textRect(frameRc);

        MARGINS scaled=m_margins;

        // don't scale margins for scale factors >1.0 since it would clip the text
        if(m_scale<1.0)
        {
            scaled.cxLeftWidth=round_int(scaled.cxLeftWidth * m_scale),
            scaled.cyTopHeight=round_int(scaled.cyTopHeight * m_scale),
            scaled.cxLeftWidth=round_int(scaled.cxLeftWidth * m_scale),
            scaled.cyBottomHeight=round_int(scaled.cyBottomHeight * m_scale);
        }

        textRect.DeflateRect(
            scaled.cxLeftWidth,
            scaled.cyTopHeight,
            scaled.cxLeftWidth,
            scaled.cyBottomHeight);

        dc.DrawText(
            text,
            -1,
            &textRect,
#ifndef LINE_WRAP
            DT_SINGLELINE |
            DT_VCENTER |
#else
            DT_EDITCONTROL |
#endif
            DT_CENTER |
            DT_NOPREFIX);

        dc.RestoreDC(isave);
    }

    DrawFocusAndPin(dc, rc);
}

//-----------------------------------------------------------------------------

void CDrawVertexHelper::DrawFocusAndPin(CAggDC& dc, const GraphTypes::RectF& rc)
{
    if(rc.IsEmptyArea())
        return;

    CRect frameRc=rc;

    if(m_focusRect && rc.Height>8)
    {
        frameRc.DeflateRect(2,2);
        dc.DrawFocusRect(&frameRc);
    }

    if(m_pins)
    {
        int pinSize = 2;

        dc.Rectangle(
            frameRc.left - pinSize,
            frameRc.top - pinSize,
            frameRc.left + pinSize,
            frameRc.top + pinSize);
        dc.Rectangle(
            frameRc.right - pinSize,
            frameRc.top - pinSize,
            frameRc.right + pinSize,
            frameRc.top + pinSize);
        dc.Rectangle(
            frameRc.left - pinSize,
            frameRc.bottom - pinSize,
            frameRc.left + pinSize,
            frameRc.bottom + pinSize);
        dc.Rectangle(
            frameRc.right - pinSize,
            frameRc.bottom - pinSize,
            frameRc.right + pinSize,
            frameRc.bottom + pinSize);
    }
}

//-----------------------------------------------------------------------------

SizeF CDrawVertexHelper::CalcSize(CDCHandle& dc, const FontInfo& font)
{
    SizeF retVal;
    std::_tstring text = m_text;
    typedef boost::tokenizer<boost::char_separator<TCHAR>, std::_tstring::const_iterator, std::_tstring> tokenizer;
    boost::char_separator<TCHAR> sep(_T("\r\n"));
    tokenizer tokens(text, sep);
    int line_count = 0;
    for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter)
    {
        SizeF tmpVal = CalcSize(dc, font, tok_iter->c_str());
        retVal.Height += tmpVal.Height;
        if (tmpVal.Width > retVal.Width)
            retVal.Width = tmpVal.Width;
        ++line_count;
    }
    retVal.Height += ((line_count - 1) * font.m_height / 3);
    return retVal;
}

SizeF CDrawVertexHelper::CalcSize(CDCHandle& dc, const FontInfo& font, const TCHAR * text)
{
    SizeF size;

    if(!text)
        return size;

    if(!m_printing)
    {
        assert(font.m_fontMgr);

        size=CAggDC::GetScaledTextExtent(
            text, 
            font.m_typeface.c_str(), 
            font.m_fontMgr,
            round_int(font.m_height));

        PointF marg(
            REAL(m_margins.cxLeftWidth+m_margins.cxRightWidth),
            REAL(m_margins.cyTopHeight+m_margins.cyBottomHeight));

        size.Width+=marg.x;
        size.Height+=marg.y;

        if(m_scale!=1.0)
        {
            size.Width*=m_scale;
            size.Height*=m_scale;
        }
    }
    else
    {
        CDCHandle hdc(dc.m_hDC);
        CAutoFont f(*font.m_font, hdc);

        CSize szExtent;

#ifndef LINE_WRAP
        dc.GetTextExtent(text, -1, &szExtent);
#else
        CRect rctext(0,0,0,0);
        dc.DrawText(
            text,
            -1,
            &rctext,
            DT_CALCRECT |
            DT_EDITCONTROL |
            DT_CENTER |
            DT_NOPREFIX);

        szExtent.cx=rctext.Width();
        szExtent.cy=rctext.Height();
#endif
        size.Width=REAL(szExtent.cx);
        size.Height=REAL(szExtent.cy);

        MARGINS scaled=m_margins;
        // don't scale margins for scale factors >1.0 since it would clip the text
        if(m_scale<1.0)
        {
            scaled.cxLeftWidth = round_int(scaled.cxLeftWidth * m_scale),
            scaled.cyTopHeight = round_int(scaled.cyTopHeight * m_scale),
            scaled.cxLeftWidth = round_int(scaled.cxLeftWidth * m_scale),
            scaled.cyBottomHeight = round_int(scaled.cyBottomHeight * m_scale);
        }

        size.Width+=scaled.cxLeftWidth+scaled.cxRightWidth;
        size.Height+=scaled.cyTopHeight+scaled.cyBottomHeight;
    }

    return size;
}
