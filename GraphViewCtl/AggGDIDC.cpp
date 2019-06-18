#include "stdafx.h"
#include "resource.h"

#include <atlmisc.h>

#include "agg_basics.h"
#include "agg_pixfmt_rgba.h"
#include "agg_path_storage.h"
#include "./ctrl/agg_polygon_ctrl.h"
#include "agg_conv_bspline.h"

#include "AggCommon.h"
#include "AggHDCRasterizer.h"
#include "AggDC.h"

class CAggGDIDC : public CAggDC
{
    agg::path_storage m_path;
    agg::hdc_rasterizer m_ras;
    CRect m_rcUpdate;
    CAggFonts * m_fonts; 
    BYTE m_Alpha;

public:
    CAggGDIDC(CDC& dc, CAggFonts * fonts, CRect* rc=0, BYTE alpha=255);
    ~CAggGDIDC();

    void GetUpdateRect(CRect* rc) { *rc=m_rcUpdate; }
    void SetAlpha(BYTE alpha) { m_Alpha=alpha; }
    void TransformPush() { }
    void TransformPop() { }
    void Transform(GraphTypes::REAL scale) { }
    void Transform(const GraphTypes::PointF& offset) { }
    void TransformReset() {};
    void BeginPathCache(unsigned npaths) {};
    void EndPathCache() {}; 
    void SetGammaThreshold(double thr/*=-1*/) {}

    void Line(
        const GraphTypes::PointF& p1,
        const GraphTypes::PointF& p2, 
        const GraphTypes::Color& clr);

    void Lines(
        const GraphTypes::PointFVector& pts,
        const GraphTypes::Color& clr,
        const GraphTypes::REAL width=1.0);

    void Rect(
        const GraphTypes::PointF& tl, 
        const GraphTypes::PointF& br,
        const GraphTypes::Color& clrfill,
        const GraphTypes::Color& clroutline,
        GraphTypes::REAL outline_width=1.0,
        bool bAA=false);

    void SolidPolygon(
        const GraphTypes::PointF* points, 
        const GraphTypes::Color& clr,
        unsigned point_count);

    void SolidPolygon(
        const GraphTypes::PointF* points, 
        const GraphTypes::Color& clr,
        unsigned point_count,
        GraphTypes::REAL outline_width,
        const GraphTypes::Color& clroutline);

    void Bezier4(
        const GraphTypes::PointF& p1,
        const GraphTypes::PointF& p2,
        const GraphTypes::PointF& p3,
        const GraphTypes::PointF& p4,
        const GraphTypes::Color& clr,
        double width=1.0,
        bool bArrow=true,
        GraphTypes::REAL scale=1.0);

    void Bezier(
        const GraphTypes::PointFVector& points,
        const GraphTypes::Color& clr,
        double width=1.0,
        bool bArrow=true,
        bool bDashed=false,
        GraphTypes::REAL scale=1.0,
        const GraphTypes::Color& firstClrControlPoint = 0x00000000, 
        const GraphTypes::Color& lastClrControlPoint = 0x00000000);

    void LollyPop(
        const GraphTypes::RectF& rc, 
        const GraphTypes::Color& clr,
        const GraphTypes::Color& clroutline,
        bool bLeft);

    void LinearGradientRect(
        const GraphTypes::RectF& rc,
        const GraphTypes::Color& clrstart,
        const GraphTypes::Color& clrend,
        bool bHorizontal=true);

    unsigned DrawScaledText(
        const TCHAR* text,
        const GraphTypes::RectF& rc,
        const GraphTypes::Color& clr,
        const char* font, 
        int size,
        const GraphTypes::PointF & descsubtract);

    bool DrawBitmap(
        HBITMAP hBitmap,
        int x, 
        int y,
        int width,
        int height,
        int bmpWidth,
        int bmpHeight);

    bool DrawTransparent(
        HBITMAP hBitmap,
        int x, 
        int y,
        int width,
        int height,
        int bmpWidth,
        int bmpHeight,
        COLORREF color);

    int SetDIBitsToDevice(
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
        UINT uColorUse);

    bool DrawAlphaBlend(
        int x, 
        int y, 
        int nWidth, 
        int nHeight, 
        HDC hSrcDC, 
        int xSrc, 
        int ySrc, 
        int nSrcWidth, 
        int nSrcHeight, 
        BLENDFUNCTION bf);

    HBITMAP DetachBitmap();
};

//*****************************************************************************

using namespace GraphTypes;

//*****************************************************************************

CAggGDIDC::CAggGDIDC(CDC& dc, CAggFonts * fonts, CRect* rc/*=0*/, BYTE alpha/*=255*/)
    : m_Alpha(alpha), m_ras(dc.m_hDC)
{
    m_fonts=fonts;

    if(rc==0)
        dc.GetClipBox(&m_rcUpdate);
    else
        m_rcUpdate=*rc;	

    Attach(dc.m_hDC);
    ATLASSERT(m_hDC!=0);
    //FillSolidRect(0,0,m_rcUpdate.Width(),m_rcUpdate.Height(),GetSysColor(COLOR_WINDOW));
}

//-----------------------------------------------------------------------------

CAggGDIDC::~CAggGDIDC()
{
    Detach();
}

//*****************************************************************************

CAggDC* CAggDC::CreateGDIDC(
    CDC& dc, CAggFonts * fonts, CRect* rc/*=0*/, BYTE alpha/*=255*/)
{
    CAggGDIDC* aggdc=0;

    aggdc=new CAggGDIDC(dc, fonts, rc, alpha);

    return aggdc;
}

//-----------------------------------------------------------------------------

void CAggGDIDC::Line(
    const PointF& p1,
    const PointF& p2, 
    const Color& clr)
{
    m_ras.reset();

    m_ras.line_width(1);
    m_ras.line_color(agg::rgba8(clr.GetR(), clr.GetG(), clr.GetB(), clr.GetA()));
    m_ras.move_to(static_cast<int>(p1.x), static_cast<int>(p1.y));
    m_ras.line_to(static_cast<int>(p2.x), static_cast<int>(p2.y));
    m_ras.render_stroke_only();
}

//-----------------------------------------------------------------------------

void CAggGDIDC::Lines(
        const PointFVector& pts,
        const Color& clr,
        const REAL width/*=1.0*/)
{

}

//-----------------------------------------------------------------------------

void CAggGDIDC::Rect(
        const PointF& tl, 
        const PointF& br,
        const Color& clrfill,
        const Color& clroutline,
        REAL outline_width/*=1.0*/,
        bool bAA/*=false*/)
{

}

//-----------------------------------------------------------------------------

void CAggGDIDC::SolidPolygon(
    const PointF* points, 
    const Color& clr,
    unsigned point_count)
{
    m_ras.reset();
    m_ras.line_width(1);
    agg::rgba8 aggcolor(clr.GetR(), clr.GetG(), clr.GetB(), clr.GetA());
    m_ras.fill_color(aggcolor);
    m_ras.line_color(aggcolor);

    m_ras.move_to(static_cast<int>(points->x), static_cast<int>(points->y));
    while(--point_count>0)
    {
        points++;
        m_ras.line_to(static_cast<int>(points->x), static_cast<int>(points->y));
    }
    m_ras.close_polygon();
    //m_ras.render_fill_only();
    m_ras.render_fill_and_stroke();
}

//-----------------------------------------------------------------------------

void CAggGDIDC::SolidPolygon(
        const PointF* points, 
        const Color& clr,
        unsigned point_count,
        REAL outline_width,
        const Color& clroutline)
{

}

//-----------------------------------------------------------------------------

void CAggGDIDC::Bezier4(
        const PointF& p1,
        const PointF& p2,
        const PointF& p3,
        const PointF& p4,
        const Color& clr,
        double width/*=1.0*/,
        bool bArrow/*=true*/,
        REAL/*scale=1.0*/)
{

}

//-----------------------------------------------------------------------------

void CAggGDIDC::Bezier(
        const GraphTypes::PointFVector& points,
        const Color& clr,
        double width/*=1.0*/,
        bool bArrow/*=true*/,
        bool bDashed/*=false*/,
        REAL scale/*=1.0*/,
        const GraphTypes::Color& firstClrControlPoint, 
        const GraphTypes::Color& lastClrControlPoint)
{

}

//-----------------------------------------------------------------------------

void CAggGDIDC::LollyPop(
        const RectF& rc, 
        const Color& clr,
        const Color& clroutline,
        bool bLeft)
{

}

//-----------------------------------------------------------------------------

void CAggGDIDC::LinearGradientRect(
    const RectF& rc,
    const Color& clrstart,
    const Color& clrend,
    bool bHorizontal/*=true*/)
{
    TRIVERTEX vert[2];
    GRADIENT_RECT grc;

    CRect irc=rc;

    vert[0].x      = irc.left;
    vert[0].y      = irc.top;
    vert[0].Red    = clrstart.GetR()<<8;
    vert[0].Green  = clrstart.GetG()<<8;
    vert[0].Blue   = clrstart.GetB()<<8;
    vert[0].Alpha  = 0x0000;

    vert[1].x      = irc.right;
    vert[1].y      = irc.bottom; 
    vert[1].Red    = clrend.GetR()<<8;
    vert[1].Green  = clrend.GetG()<<8;
    vert[1].Blue   = clrend.GetB()<<8;
    vert[1].Alpha  = 0x0000;

    grc.UpperLeft  = 0;
    grc.LowerRight = 1;

    BOOL bok=::GradientFill(
        m_hDC,
        vert,
        2,
        &grc, 
        1,
        bHorizontal?GRADIENT_FILL_RECT_H:GRADIENT_FILL_RECT_V);

    ATLASSERT(bok);
}

//-----------------------------------------------------------------------------

unsigned CAggGDIDC::DrawScaledText(
    const TCHAR* text,
    const RectF& rc,
    const Color& clr,
    const char* font, 
    int size,
    const GraphTypes::PointF & descsubtract)
{
    return 0;
}

//-----------------------------------------------------------------------------
// TODO - share this code with CAggMemeoryDC instead of duplicating it
//-----------------------------------------------------------------------------

bool CAggGDIDC::DrawBitmap(
    HBITMAP hBitmap,
    int x, 
    int y,
    int width,
    int height,
    int bmpWidth,
    int bmpHeight)
{
    ATLASSERT(width>0&&height>0);

    CDC dcImage;
    bool bOk=dcImage.CreateCompatibleDC(m_hDC) != NULL;
    if(bOk)
    {
        HBITMAP oldbmp=dcImage.SelectBitmap(hBitmap);
        if(bmpWidth==width && bmpHeight==height)
            bOk=BitBlt(x, y, width, height, dcImage, 0, 0, SRCCOPY) != 0;
        else
            bOk=StretchBlt(x, y, width, height, dcImage, 0, 0, bmpWidth, bmpHeight, SRCCOPY) != 0;
        ATLASSERT(bOk);
        dcImage.SelectBitmap(oldbmp);
    }

    return bOk;
}

//-----------------------------------------------------------------------------
// TODO - share this code with CAggMemeoryDC instead of duplicating it
//-----------------------------------------------------------------------------

bool CAggGDIDC::DrawTransparent(
    HBITMAP hBitmap,
    int x, 
    int y,
    int width,
    int height,
    int bmpWidth,
    int bmpHeight,
    COLORREF color)
{
    static bool bTestedOS;
    static bool bTransOk;

    ATLASSERT(width>0&&height>0);

    // Test OS support for non-leaky TransparentBlt
    if(!bTestedOS)
    {
        bTestedOS=true;

        OSVERSIONINFO osv;
        memset(&osv, 0, sizeof osv);
        osv.dwOSVersionInfoSize=sizeof(osv);
        ::GetVersionEx(&osv);
        if(osv.dwPlatformId == VER_PLATFORM_WIN32_NT ||
            (osv.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS &&
                osv.dwMajorVersion >= 4 && osv.dwMinorVersion >= 90))
            bTransOk=true;
    }

    CDC dcImage;
    bool bOk=dcImage.CreateCompatibleDC(m_hDC) != NULL;
    HBITMAP oldbmp=dcImage.SelectBitmap(hBitmap);

    // if OS supports non-leaky TransparentBlt use it (also on printer)
    if(bTransOk)
    {
#ifndef _WIN32_WCE
        bOk=TransparentBlt(x, y, width, height, dcImage, 0, 0, bmpWidth, bmpHeight, color) != 0;
        if (!bOk) // just in case (e.g. printing), try emulation
            goto tryEmulate;
#endif
    }
    else
    {
tryEmulate:
        CBitmap bmpMono;
        bOk=bmpMono.CreateBitmap(width, height, 1, 1, NULL) != NULL;
        if (!bOk)
            return bOk;

        CDC dcMono;
        // Create dc for the mask
        bOk = dcMono.CreateCompatibleDC(m_hDC) != NULL;
        if (!bOk)
            return bOk;

        // Select the mask bitmap into its dc
        HBITMAP oldBitmapMono = (HBITMAP)::SelectObject(dcMono, bmpMono);

        if(bOk)
        {
            // Build mask based on transparent colour

            COLORREF crOldImg=dcImage.SetBkColor(color);
            bOk=dcMono.StretchBlt(0, 0, width, height, dcImage, 0, 0, bmpWidth, bmpHeight, SRCCOPY) != 0;
            dcImage.SetBkColor(crOldImg);

            // True Mask method - no flicker if destination is not actual display
            if(bOk)
                bOk=StretchBlt(x, y, width, height, dcImage, 0, 0, bmpWidth, bmpHeight, SRCINVERT) != 0;

            if(bOk) 
            {
                // setup the destination for monochrome blit
                COLORREF crOldBack = SetBkColor(RGB(255,255,255));
                COLORREF crOldText = SetTextColor(RGB(0,0,0));

                bOk=BitBlt(x, y, width, height, dcMono, 0, 0, SRCAND) != 0;

                SetBkColor(crOldBack);
                SetTextColor(crOldText);
            }

            if(bOk)
                bOk=StretchBlt(x, y, width, height, dcImage, 0, 0, bmpWidth, bmpHeight, SRCINVERT) != 0;
        }

        dcMono.SelectBitmap(oldBitmapMono);
    }

    dcImage.SelectBitmap(oldbmp);

    return bOk;
}

//-----------------------------------------------------------------------------
int CAggGDIDC::SetDIBitsToDevice(
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
    UINT uColorUse)
{
    //m_mtx.transform(&x,&y);
    return ::SetDIBitsToDevice(m_hDC, x, y, dwWidth, dwHeight, xSrc, ySrc, uStartScan, cScanLines, lpvBits, lpbmi, uColorUse);
}
//-----------------------------------------------------------------------------

bool CAggGDIDC::DrawAlphaBlend(
    int x, 
    int y, 
    int nWidth, 
    int nHeight, 
    HDC hSrcDC, 
    int xSrc, 
    int ySrc, 
    int nSrcWidth, 
    int nSrcHeight, 
    BLENDFUNCTION bf)
{
    bool bOk=BitBlt(x, y, nWidth, nHeight, hSrcDC, xSrc, ySrc, SRCCOPY) != 0;
    return bOk;
}

//-----------------------------------------------------------------------------

HBITMAP CAggGDIDC::DetachBitmap()
{
    return 0;
}

//-----------------------------------------------------------------------------
