#include "stdafx.h"

#include "AggDC.h"
#include "AggBitmap.h"

using namespace GraphTypes;

//-----------------------------------------------------------------------------

CAggBitmap::CAggBitmap()
#ifdef _DEBUG
: started(false)
#endif
{

}

//-----------------------------------------------------------------------------

CAggBitmap::~CAggBitmap()
{
    Clear();
}

//-----------------------------------------------------------------------------

void CAggBitmap::Clear()
{
    m_ibmp=0;
#ifdef _DEBUG
    started=false;
#endif
}

//-----------------------------------------------------------------------------

CAggDC* CAggBitmap::BeginPaint(CAggDC& dc, const CSize& size, CAggFonts* fonts)
{
    CRect rc(0,0,size.cx,size.cy);
#ifdef _DEBUG
    assert(!started);
    started=true;
#endif
    // user's responsibility to free
    return	CAggDC::CreateMemoryDC(dc, fonts, &rc);
}

//-----------------------------------------------------------------------------

void CAggBitmap::EndPaint(CAggDC* dc)
{
#ifdef _DEBUG
    assert(started);
    started=false;
#endif

    HBITMAP hbmp=dc->DetachBitmap();
    if(hbmp)
        m_ibmp=CreateIBitmap(hbmp);
    else
        m_ibmp=0;
}

//-----------------------------------------------------------------------------

bool CAggBitmap::IsEmpty() const
{
    return m_ibmp==0 || !m_ibmp->IsValid();
}

//-----------------------------------------------------------------------------
//
// NB: unless flags has DT_CENTER set, only top left corner is used from rc

void CAggBitmap::Draw(CAggDC& dc, const CRect& rc, unsigned flags)
{
    if(IsEmpty())
        return;

    CSize size;
    bool sizeok=m_ibmp->GetSize(size);
    assert(sizeok);

    CRect rcblit(0, 0, size.cx, size.cy);

    if((flags&DT_CENTER) && !rc.IsRectEmpty())
    {
        int xoff=(rc.Width()-rcblit.Width())/2;
        int yoff=(rc.Height()-rcblit.Height())/2;
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

