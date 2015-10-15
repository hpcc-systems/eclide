#include "stdafx.h"

#include "GraphTypes.h" 

namespace GraphTypes 
{

VOID Color::SetFromCOLORREF(IN COLORREF rgb)
{
    Argb = MakeARGB(255, GetRValue(rgb), GetGValue(rgb), GetBValue(rgb));
}

COLORREF Color::ToCOLORREF(ARGB argb)
{
    Color color(argb);
    return RGB(color.GetRed(), color.GetGreen(), color.GetBlue());
}

void Color::Complementary(Color& comp)
{
    comp.SetValue(MakeARGB(GetA(), 255-GetR(), 255-GetG(), 255-GetB()));
}

bool Color::IsDarker(const Color& color) const
{
    WORD mylum=GetLuminance();
    WORD hislum=color.GetLuminance();
    // eventually use percentage rather than raw value comparison
    return mylum<hislum;
}

bool Color::IsLighter(const Color& color) const
{
    WORD mylum=GetLuminance();
    WORD hislum=color.GetLuminance();
    return mylum>hislum;
}

#pragma warning(push)
#pragma warning(disable: 4244)

void Color::MoreBright(WORD step)
{
    COLORREF clrRGB=ToCOLORREF();
    WORD wHue, wLuminance, wSaturation;	
    ColorRGBToHLS(clrRGB, &wHue, &wLuminance, &wSaturation);
    if((unsigned(wLuminance)+unsigned(step))<=65535)
        wLuminance+=step;
    clrRGB=ColorHLSToRGB(wHue, wLuminance, wSaturation);
    SetFromCOLORREF(clrRGB);
}

void Color::LessBright(WORD step)
{
    COLORREF clrRGB=ToCOLORREF();
    WORD wHue, wLuminance, wSaturation;	
    ColorRGBToHLS(clrRGB, &wHue, &wLuminance, &wSaturation);
    if((int(wLuminance)-int(step))>=0)
        wLuminance-=step;
    clrRGB=ColorHLSToRGB(wHue, wLuminance, wSaturation);
    SetFromCOLORREF(clrRGB);
}

void Color::MoreSaturated(WORD step)
{
    COLORREF clrRGB=ToCOLORREF();
    WORD wHue, wLuminance, wSaturation;	
    ColorRGBToHLS(clrRGB, &wHue, &wLuminance, &wSaturation);
    if((unsigned(wSaturation)+unsigned(step))<=65535)
        wSaturation+=step;
    clrRGB=ColorHLSToRGB(wHue, wLuminance, wSaturation);
    SetFromCOLORREF(clrRGB);
}

void Color::LessSaturated(WORD step)
{
    COLORREF clrRGB=ToCOLORREF();
    WORD wHue, wLuminance, wSaturation;	
    ColorRGBToHLS(clrRGB, &wHue, &wLuminance, &wSaturation);
    if((int(wSaturation)-int(step))>=0)
        wSaturation-=step;
    clrRGB=ColorHLSToRGB(wHue, wLuminance, wSaturation);
    SetFromCOLORREF(clrRGB);
}

#pragma warning(pop)

WORD Color::GetHue() const
{
    COLORREF clrRGB=ToCOLORREF();
    WORD wHue, wLuminance, wSaturation;	
    ColorRGBToHLS(clrRGB, &wHue, &wLuminance, &wSaturation);
    return wHue;
}

WORD Color::GetLuminance() const
{
    COLORREF clrRGB=ToCOLORREF();
    WORD wHue, wLuminance, wSaturation;	
    ColorRGBToHLS(clrRGB, &wHue, &wLuminance, &wSaturation);
    return wLuminance;
}

WORD Color::GetSaturation() const
{
    COLORREF clrRGB=ToCOLORREF();
    WORD wHue, wLuminance, wSaturation;	
    ColorRGBToHLS(clrRGB, &wHue, &wLuminance, &wSaturation);
    return wSaturation;
}

void Color::GetHLS(WORD hls[3]) const
{
    COLORREF clrRGB=ToCOLORREF();
    ColorRGBToHLS(clrRGB, &hls[0], &hls[1], &hls[2]);
}

#define	DONT_INTERSECT    0
#define	DO_INTERSECT      1
#define COLLINEAR         2

#define SAME_SIGNS( a, b ) (((long) ((unsigned long) a ^ (unsigned long) b)) >= 0 )

int lines_intersect(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float  *x, float *y)
{
    float a1, a2, b1, b2, c1, c2; /* Coefficients of line eqns. */
    float r1, r2, r3, r4;         /* 'Sign' values */
    float denom, offset, num;     /* Intermediate values */

    /* Compute a1, b1, c1, where line joining points 1 and 2
    * is "a1 x  +  b1 y  +  c1  =  0".
    */

    a1 = y2 - y1;
    b1 = x1 - x2;
    c1 = x2 * y1 - x1 * y2;

    /* Compute r3 and r4.
    */


    r3 = a1 * x3 + b1 * y3 + c1;
    r4 = a1 * x4 + b1 * y4 + c1;

    /* Check signs of r3 and r4.  If both point 3 and point 4 lie on
    * same side of line 1, the line segments do not intersect.
    */

    if ( r3 != 0 &&
        r4 != 0 &&
        SAME_SIGNS( r3, r4 ))
        return ( DONT_INTERSECT );

    /* Compute a2, b2, c2 */

    a2 = y4 - y3;
    b2 = x3 - x4;
    c2 = x4 * y3 - x3 * y4;

    /* Compute r1 and r2 */

    r1 = a2 * x1 + b2 * y1 + c2;
    r2 = a2 * x2 + b2 * y2 + c2;

    /* Check signs of r1 and r2.  If both point 1 and point 2 lie
    * on same side of second line segment, the line segments do
    * not intersect.
    */

    if ( r1 != 0 &&
        r2 != 0 &&
        SAME_SIGNS( r1, r2 ))
        return ( DONT_INTERSECT );

    /* Line segments intersect: compute intersection point. 
    */

    denom = a1 * b2 - a2 * b1;
    if ( denom == 0 )
        return ( COLLINEAR );
    offset = denom < 0 ? - denom / 2 : denom / 2;

    /* The denom/2 is to get rounding instead of truncating.  It
    * is added or subtracted to the numerator, depending upon the
    * sign of the numerator.
    */

    num = b1 * c2 - b2 * c1;
    *x = ( num < 0 ? num - offset : num + offset ) / denom;

    num = a2 * c1 - a1 * c2;
    *y = ( num < 0 ? num - offset : num + offset ) / denom;

    return ( DO_INTERSECT );
} 

bool Intersect_Lines(const GraphTypes::PointF & p0,const GraphTypes::PointF & p1,const GraphTypes::PointF & p2, const GraphTypes::PointF & p3, GraphTypes::PointF & p)
{
    return (lines_intersect(p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, &p.x, &p.y) == DO_INTERSECT);
}

bool Intersect_Lines(const GraphTypes::RectF & r,const GraphTypes::PointF & p2,const GraphTypes::PointF & p3, GraphTypes::PointF & p)
{
    GraphTypes::PointF pTL(r.x, r.y);
    GraphTypes::PointF pTR(r.x + r.Width, r.y);
    GraphTypes::PointF pBL(r.x, r.y + r.Height);
    GraphTypes::PointF pBR(r.x + r.Width, r.y + r.Height);

    if (Intersect_Lines(pTL, pTR, p2, p3, p) || 
        Intersect_Lines(pTR, pBR, p2, p3, p) || 
        Intersect_Lines(pBL, pBR, p2, p3, p) ||
        Intersect_Lines(pTL, pBL, p2, p3, p))
    {
        return true;
    }
    return false;
}

unsigned long ToBGR(COLORREF rgb)
{
    Color c;
    c.SetFromCOLORREF(rgb);
    return c.ToBGR();
}

//*****************************************************************************

class CBitmapRefCnt : public IBitmap, public CUnknown
{
    BITMAPINFOHEADER    m_bmi;
    DWORD               m_dsBitfields[3]; 
    void*				m_pbits;
    HANDLE				m_hMMSection;
public:
    CBitmapRefCnt(HBITMAP hBitmap) : m_hMMSection(0), m_pbits(0)
    { 
        assert(hBitmap);

        memset(&m_bmi, 0, sizeof m_bmi);
        m_bmi.biSize=sizeof(BITMAPINFOHEADER);
        CClientDC cdc(0);
        int res=::GetDIBits(
            cdc.m_hDC,
            hBitmap,
            0,
            0,
            0,
            (BITMAPINFO*)&m_bmi,
            DIB_RGB_COLORS);

        assert(res);
        assert(m_bmi.biCompression==BI_RGB || m_bmi.biCompression==BI_BITFIELDS);
        unsigned sizeBytes=0;

        if(res)
        {
            sizeBytes=m_bmi.biSizeImage;
            assert(sizeBytes);
            m_hMMSection=::CreateFileMapping(
                INVALID_HANDLE_VALUE,
                0,
                PAGE_READWRITE, 
                0,
                sizeBytes,
                0);

            assert(m_hMMSection);
        }

        if(m_hMMSection)
        {
            void* pbits=::MapViewOfFile(
                m_hMMSection,
                FILE_MAP_WRITE | FILE_MAP_READ,
                0,
                0,
                0);

            assert(pbits);
            if(pbits)
            {
                int lines=::GetDIBits(
                    cdc.m_hDC,
                    hBitmap,
                    0, 
                    m_bmi.biHeight,
                    pbits,
                    (BITMAPINFO*)&m_bmi,
                    DIB_RGB_COLORS);

                unsigned widthbytes=(((m_bmi.biWidth*m_bmi.biBitCount/8)+3)/4)*4;
                unsigned calcsize=(unsigned)lines*widthbytes;
                assert(calcsize==sizeBytes);
                bool bunmap=::UnmapViewOfFile(pbits) != FALSE;
                assert(bunmap);
                ::DeleteObject(hBitmap);
                if(calcsize!=sizeBytes || !bunmap)
                    goto clear;
            }
            else
            {
clear:
                bool bOk=::CloseHandle(m_hMMSection) != FALSE;
                assert(bOk);
                m_hMMSection=0;
            }
        }
    }

    ~CBitmapRefCnt()
    {
        if(m_hMMSection)
        {
            ::CloseHandle(m_hMMSection);
            m_hMMSection=0; 
        }
    }

    BEGIN_CUNKNOWN
    END_CUNKNOWN(CUnknown)

    CBitmap* GetCBitmap() 
    { 
        CBitmap* pbmp=new CBitmap();

        if(m_hMMSection)
        {
            CClientDC cdc(0);

            pbmp->CreateDIBSection(
                cdc.m_hDC,
                (const BITMAPINFO*)&m_bmi,
                DIB_RGB_COLORS,
                &m_pbits,
                m_hMMSection,
                0);

            assert(!pbmp->IsNull());
        }
        return pbmp; 
    }

    bool IsValid() const { return m_hMMSection!=0; }

    bool GetSize(SIZE& size) const
    {
        if(m_hMMSection)
        {
            size.cx=m_bmi.biWidth;
            size.cy=m_bmi.biHeight;
            return true;
        }
        else
        {
            size.cx=size.cy=0;
            return false;
        }
    }

    // this method should be called only *AFTER* GetCBitmap() call

    void* GetBitsPtr() const { return m_pbits; }

    const BITMAPINFO& GetBitmapInfo() const { return (const BITMAPINFO&)m_bmi; }

};

CLIB_API IBitmap* CreateIBitmap(HBITMAP hBitmap)
{
    CBitmapRefCnt* cbmp = new CBitmapRefCnt(hBitmap);
    return cbmp;
}

}
//void test()
//{
//	boost::read_write_mutex mutex(boost::read_write_scheduling_policy::writer_priority);
//	{
//		boost::read_write_mutex::scoped_write_lock writeLock(mutex);
//	}
//	{
//		boost::read_write_mutex::scoped_read_lock readLock(mutex);
//	}
//	MutexedT<GraphTypes::PointF> p;
//	{
//		CComPtr<CWriteLock> lock;
//		p.Get(lock) = GraphTypes::PointF(10.0f, 12.0f);
//	}
//	{
//		CComPtr<CReadLock> lock;
//		const GraphTypes::PointF & p2 = p.Get(lock);
//	}
//	std::map<std::_tstring, MutexedT<GraphTypes::PointF> > PointMap;
//	{
//		CComPtr<CWriteLock> lock;
//		PointMap[_T("Gordon")].Get(lock).x = 11.0f;
//	}
//}
