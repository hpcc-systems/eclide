#include "stdafx.h"

#include "Canvas.h"
#include "AggDC.h"
#include "AggCommon.h"
#include "AggMemText.h"

using namespace GraphTypes;

//-----------------------------------------------------------------------------

CAggMemText::CAggMemText(const char* font/*="Verdana"*/)
:	m_scale(0.0),
    m_fontName(font),
    m_dpix(0),
    m_dpiy(0)
{
    memset(&m_Margins, 0, sizeof(m_Margins));
}

//-----------------------------------------------------------------------------

CAggMemText::~CAggMemText()
{
    Clear();
}

//-----------------------------------------------------------------------------

void CAggMemText::CalcSize(const TCHAR* text, CCanvas* canvas)
{
    int newdpix=canvas->GetDPIX(), newdpiy=canvas->GetDPIY();
    bool changedDPI=false;

    if(newdpix!=m_dpix || newdpiy!=m_dpiy)
    {
        changedDPI=true;	
        m_dpix=newdpix;
        m_dpiy=newdpiy;
    }

    if(m_size.Height==0 || changedDPI)
    {
        m_size=CAggDC::GetScaledTextExtent(text, m_fontName.c_str(), canvas->m_fonts, (DEFAULT_FONT_SIZE-1)*canvas->GetDPIY()/72);

        PointF marg(
            REAL(m_Margins.cxLeftWidth+m_Margins.cxRightWidth),
            REAL(m_Margins.cyTopHeight+m_Margins.cyBottomHeight));

        canvas->DpiTransform(marg);

        m_size.Width+=marg.x;
        m_size.Height+=marg.y;
        m_scaledSize=m_size;
        canvas->ScaleTransform(m_scaledSize); 
    }

    if(changedDPI)
    {
        m_ibmp=0;
    }

    if(canvas->GetScale()!=m_scale)
    {
        m_scale=canvas->GetScale();
        m_scaledSize=m_size;
        canvas->ScaleTransform(m_scaledSize); // DPI taken into account when sizing
        m_ibmp=0;
    }
}

//-----------------------------------------------------------------------------

void CAggMemText::Clear()
{
    m_size.Width=m_size.Height=0;
    m_ibmp=0;
}

//-----------------------------------------------------------------------------

void CAggMemText::Prepare(
    CAggDC& dc, 
    const TCHAR* text, 
    Color ctext,
    Color cbkgnd,
    Color cborder,
    REAL lwidth,
    CCanvas* canvas)
{
    if(canvas->GetScale()!=m_scale)
    {
        m_scale=canvas->GetScale();
        m_scaledSize=m_size;
        canvas->ScaleTransform(m_scaledSize); // DPI taken into account when sizing
        m_ibmp=0;
    }

    if(!m_ibmp)
    {
        RectF rcf(REAL(0.0), REAL(0.0), m_scaledSize.Width, m_scaledSize.Height);
        CRect rc=static_cast<CRect>(rcf);
        if (rc.Height() < 1)
            rc.bottom = rc.top + 1;
        if (rc.Width() < 1)
            rc.right = rc.left + 1;
        boost::scoped_ptr<CAggDC> memDC(CAggDC::CreateMemoryDC(dc, canvas->m_fonts, &rc));
        ATLASSERT(memDC);

        if(memDC)
        {
            memDC->TransformReset();	
            memDC->Transform(m_scale);

            Color bcolor;
            bcolor.SetFromCOLORREF(GetSysColor(COLOR_WINDOW));

            // If background and outline colors are the same as COLOR_WINDOW
            // then there is no need to draw them because Agg memory DC
            // by default initializes its background to COLOR_WINDOW

            if(bcolor.GetValue()!=cbkgnd.GetValue() || bcolor.GetValue()!=cborder.GetValue())
            {
                REAL halfLineWidth=lwidth/2;

                PointF tl(halfLineWidth, halfLineWidth);
                PointF br(m_size.Width-halfLineWidth, m_size.Height-halfLineWidth);

                memDC->Rect(
                    tl, 
                    br,
                    cbkgnd,
                    cborder,
                    lwidth,
                    true);
            }

            if(m_Margins.cxLeftWidth!=0 || m_Margins.cyTopHeight!=0)
                rcf.Inflate(
                -REAL(m_Margins.cxLeftWidth)*m_scale,
                -REAL(m_Margins.cyTopHeight)*m_scale);

            // For some reason 2 needs to be subtracted from descent to get a good centering
            PointF descsubtract(REAL(0), REAL(2));
            // this subtraction value has to be adjusted for printing
            canvas->DpiTransform(descsubtract);
            memDC->DrawScaledText(
                text, 
                rcf,
                ctext,
                m_fontName.c_str(), 
                (DEFAULT_FONT_SIZE-1)*canvas->GetDPIY()/72,
                descsubtract);

            // extract handle and use it to create IBitmap
            HBITMAP hbmp=memDC->DetachBitmap();
            m_ibmp=CreateIBitmap(hbmp);
        }
    }
}

//-----------------------------------------------------------------------------
//
// NB: unless flags has DT_CENTER set, only top left corner is used from rc

void CAggMemText::Draw(CAggDC& dc, const CRect& rc, unsigned flags)
{
    if(m_ibmp==0 || !m_ibmp->IsValid())
        return;

    CRect rcblit(0, 0, round_int(m_scaledSize.Width), round_int(m_scaledSize.Height));

    if((flags&DT_CENTER) && !rc.IsRectEmpty())
    {
        int xoff=(rc.Width()-rcblit.Width())/2;
        int yoff=(rc.Height()-rcblit.Height())/2;
        ATLASSERT(xoff>=0 && yoff>=0);
        rcblit.OffsetRect(rc.left+xoff, rc.top+yoff);
    }
    else
    {
        rcblit.OffsetRect(rc.left, rc.top);
    }

    // blit will fail if rectangle empty which may happen at small scale
    if(!rcblit.IsRectEmpty())
    {
        boost::scoped_ptr<CBitmap> cbmp(m_ibmp->GetCBitmap());
        if(cbmp->IsNull())
            return;

        BOOL bBlitOk;

        if(flags&DT_TRANSPARENT)
        {
            bBlitOk=dc.DrawTransparent(
                (HBITMAP)(*cbmp),
                rcblit.left,
                rcblit.top,
                rcblit.Width(),
                rcblit.Height(),
                rcblit.Width(),
                rcblit.Height(),
                GetSysColor(COLOR_WINDOW));
        }
        else
        {
            bBlitOk=dc.SetDIBitsToDevice(
                rcblit.left,
                rcblit.top,
                rcblit.Width(),
                rcblit.Height(),
                0,
                0,
                0,
                m_ibmp->GetBitmapInfo().bmiHeader.biHeight,
                m_ibmp->GetBitsPtr(),
                &m_ibmp->GetBitmapInfo(),
                DIB_RGB_COLORS);
        }
    
        ATLASSERT(bBlitOk);	
    }

    // save memory when drawing to hires printer device
    // Not suppoerted anymore 
    ATLASSERT(dc.GetDeviceCaps(TECHNOLOGY)==DT_RASDISPLAY);
}

