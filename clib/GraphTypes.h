#pragma once

#include "Util.h"

// NOTE: *MUST* match definition in GraphViewCtl.idl
enum VISIBLE
{
    VISIBLE_DEFAULT = 0,
    VISIBLE_HIDE,
    VISIBLE_SHOW
};

typedef std::vector<VISIBLE> VisibleVector;

enum IMAGE_POSITION
{
    POSITION_NORTH_WEST = 0,
    POSITION_NORTH,
    POSITION_NORTH_EAST,
    POSITION_WEST,
    POSITION_CENTER,
    POSITION_EAST,
    POSITION_SOUTH_WEST,
    POSITION_SOUTH,
    POSITION_SOUTH_EAST
};

// must match GraphViewCtl.h

#define DEFAULT_FONT_SIZE 10	//Works out to be 12 on my machine.
#define ACTUAL_FONT_SIZE 12
#define M_PI 3.14159265358979323846f
#define ToRadian( degree ) ((degree) * (M_PI / 180.0f)) 
#define ToDegree( radian ) ((radian) * (180.0f / M_PI))  

namespace GraphTypes {

typedef float REAL;

/*
class REAL
{
    float val;
public:
    REAL() throw()
    {
        val=0;
    }
    
    REAL(const REAL& r) throw()
    {
        val=r.val;
    }

    REAL(double r) throw()
    {
        val=static_cast<float>(r);
    }

    REAL(float r) throw()
    {
        val=r;
    }

    REAL(long r) throw()
    {
        val=static_cast<float>(r);
    }

    REAL(int r) throw()
    {
        val=static_cast<float>(r);
    }

    operator double() const
    {
        return static_cast<double>(val);
    }

    REAL& operator+=(const REAL& r)
    {
        val+=r.val;
        return *this;
    }

    REAL& operator-=(const REAL& r)
    {
        val-=r.val;
        return *this;
    }

};
*/


typedef std::vector<REAL> REALVector;

enum Coordinates
{
    world,
    screen
};

#define REAL_MAX            FLT_MAX
#define REAL_MIN            FLT_MIN
#define REAL_TOLERANCE     (FLT_MIN * 100)
#define REAL_EPSILON        1.192092896e-07F        /* FLT_EPSILON */

class SizeF;
class PointF;
class RectF;

class SizeF
{
public:
    SizeF()
    {
        Width = Height = 0.0f;
    }

    SizeF(IN const SizeF& size)
    {
        Width = size.Width;
        Height = size.Height;
    }

    SizeF(IN const SIZE& size)
    {
        Width = static_cast<REAL>(size.cx);
        Height = static_cast<REAL>(size.cy);
    }

    SizeF(IN REAL width,
          IN REAL height)
    {
        Width = width;
        Height = height;
    }

    SizeF operator+(IN const SizeF& sz) const
    {
        return SizeF(Width + sz.Width,
                     Height + sz.Height);
    }

    SizeF operator-(IN const SizeF& sz) const
    {
        return SizeF(Width - sz.Width,
                     Height - sz.Height);
    }

    BOOL Equals(IN const SizeF& sz) const
    {
        return (Width == sz.Width) && (Height == sz.Height);
    }

    BOOL Empty() const
    {
        return (Width == 0.0f && Height == 0.0f);
    }

    operator CSize() const
    {
        return CSize(round_int(Width),round_int(Height));
    }

public:

    REAL Width;
    REAL Height;
};

class PointF
{
public:
    PointF()
    {
        x = y = 0.0f;
    }

    PointF(IN const PointF &point)
    {
        x = point.x;
        y = point.y;
    }

    PointF(IN const SizeF &size)
    {
        x = size.Width;
        y = size.Height;
    }

    PointF(IN const POINT &point)
    {
        x = static_cast<REAL>(point.x)+0.5f;
        y = static_cast<REAL>(point.y)+0.5f;
    }

    PointF(IN LONG _x, IN LONG _y)
    {
        x = static_cast<REAL>(_x)+0.5f;
        y = static_cast<REAL>(_y)+0.5f;
    }

    PointF(IN REAL _x,
        IN REAL _y)
    {
        x = _x;
        y = _y;
    }

    ~PointF()
    {
    }

    PointF operator+(IN const PointF& point) const
    {
        return PointF(x + point.x,
            y + point.y);
    }

    PointF operator-(IN const PointF& point) const
    {
        return PointF(x - point.x,
            y - point.y);
    }

    BOOL Equals(IN const PointF& point)
    {
        return (x == point.x) && (y == point.y);
    }

    operator CPoint() const
    {
        return CPoint(floor_int(x), floor_int(y));
    }

public:

    REAL x;
    REAL y;
};

typedef std::vector<PointF> PointFVector;

class RectF
{
public:

    RectF()
    {
        x = y = Width = Height = 0.0f;
    }

    RectF(IN REAL _x,
          IN REAL _y,
          IN REAL width,
          IN REAL height)
    {
        x = _x;
        y = _y;
        Width = width;
        Height = height;
    }

    RectF(const RECT &r)
    {
        x = static_cast<REAL>(r.left)+0.5f;
        y = static_cast<REAL>(r.top)+0.5f;
        Width = static_cast<REAL>(r.right - r.left);
        Height = static_cast<REAL>(r.bottom - r.top);
    }

    RectF(IN const PointF& location,
          IN const SizeF& size)
    {
        x = location.x;
        y = location.y;
        Width = size.Width;
        Height = size.Height;
    }

    RectF(IN const SizeF& size)
    {
        x = 0.0f;
        y = 0.0f;
        Width = size.Width;
        Height = size.Height;
    }

    RectF* Clone() const
    {
        return new RectF(x, y, Width, Height);
    }

    VOID GetLocation(OUT PointF* point) const
    {
        point->x = x;
        point->y = y;
    }

    VOID SetLocation(IN const PointF* point)
    {
        x = point->x;
        y = point->y;
    }

    VOID GetSize(OUT SizeF* size) const
    {
        size->Width = Width;
        size->Height = Height;
    }

    VOID GetBounds(OUT RectF* rect) const
    {
        rect->x = x;
        rect->y = y;
        rect->Width = Width;
        rect->Height = Height;
    }

    VOID GetCenter(OUT PointF* point) const
    {
        point->x = x + Width / 2;
        point->y = y + Height / 2;
    }

    VOID GetCenterTop(OUT PointF* point) const
    {
        point->x = x + Width / 2;
        point->y = y;
    }

    REAL GetLeft() const
    {
        return x;
    }

    REAL GetTop() const
    {
        return y;
    }

    REAL GetRight() const
    {
        return x+Width;
    }

    REAL GetBottom() const
    {
        return y+Height;
    }

    BOOL IsEmptyArea() const
    {
        return (Width <= REAL_EPSILON) || (Height <= REAL_EPSILON);
    }

    BOOL Equals(IN const RectF & rect) const
    {
        return x == rect.x &&
               y == rect.y &&
               Width == rect.Width &&
               Height == rect.Height;
    }

    BOOL Contains(IN REAL _x,
                  IN REAL _y) const
    {
        return _x >= x && _x < x+Width &&
               _y >= y && _y < y+Height;
    }

    BOOL Contains(IN const PointF& pt) const
    {
        return Contains(pt.x, pt.y);
    }

    BOOL Contains(IN const RectF& rect) const
    {
        return (x <= rect.x) && (rect.GetRight() <= GetRight()) &&
               (y <= rect.y) && (rect.GetBottom() <= GetBottom());
    }

    VOID Inflate(IN REAL dx,
                 IN REAL dy)
    {
        x -= dx;
        y -= dy;
        Width += 2*dx;
        Height += 2*dy;
    }

    VOID Inflate(IN const PointF& point)
    {
        Inflate(point.x, point.y);
    }

    BOOL Intersect(IN const RectF& rect)
    {
        return Intersect(*this, *this, rect);
    }

    static BOOL Intersect(OUT RectF& c,
                          IN const RectF& a,
                          IN const RectF& b)
    {
        REAL right = std::min<REAL>(a.GetRight(), b.GetRight());
        REAL bottom = std::min<REAL>(a.GetBottom(), b.GetBottom());
        REAL left = std::max<REAL>(a.GetLeft(), b.GetLeft());
        REAL top = std::max<REAL>(a.GetTop(), b.GetTop());

        c.x = left;
        c.y = top;
        c.Width = right - left;
        c.Height = bottom - top;
        return !c.IsEmptyArea();
    }

    BOOL IntersectsWith(IN const RectF& rect) const
    {
        return (GetLeft() < rect.GetRight() &&
                GetTop() < rect.GetBottom() &&
                GetRight() > rect.GetLeft() &&
                GetBottom() > rect.GetTop());
    }

    static BOOL Union(OUT RectF& c,
                      IN const RectF& a,
                      IN const RectF& b)
    {
        REAL right = std::max<REAL>(a.GetRight(), b.GetRight());
        REAL bottom = std::max<REAL>(a.GetBottom(), b.GetBottom());
        REAL left = std::min<REAL>(a.GetLeft(), b.GetLeft());
        REAL top = std::min<REAL>(a.GetTop(), b.GetTop());

        c.x = left;
        c.y = top;
        c.Width = right - left;
        c.Height = bottom - top;
        return !c.IsEmptyArea();
    }

    VOID Offset(IN const PointF& point)
    {
        Offset(point.x, point.y);
    }

    VOID Offset(IN REAL dx,
                IN REAL dy)
    {
        x += dx;
        y += dy;
    }

    operator CRect() const
    {
        CRect r;

        r.left=floor_int(x);
        r.top=floor_int(y);
        r.right=r.left+round_int(Width);
        r.bottom=r.top+round_int(Height);
        return r;
    }

    bool operator == (const RectF & other) const
    {
        return (other.x == x && other.y == y && other.Width == Width && other.Height == Height);
    }

public:

    REAL x;
    REAL y;
    REAL Width;
    REAL Height;
};

typedef std::vector<RectF> RectFVector;

//----------------------------------------------------------------------------
// Color mode
//----------------------------------------------------------------------------

enum ColorMode
{
    ColorModeARGB32 = 0,
    ColorModeARGB64 = 1
};

//----------------------------------------------------------------------------
// Color Channel flags
//----------------------------------------------------------------------------

enum ColorChannelFlags
{
    ColorChannelFlagsC = 0,
    ColorChannelFlagsM,
    ColorChannelFlagsY,
    ColorChannelFlagsK,
    ColorChannelFlagsLast
};

//----------------------------------------------------------------------------
// Color
//----------------------------------------------------------------------------
typedef DWORD ARGB;
typedef DWORDLONG ARGB64;

class CLIB_API Color
{
public:

    Color()
    {
        Argb = 0x0;
    }

    // Construct an opaque Color object with
    // the specified Red, Green, Blue values.
    //
    // Color values are not premultiplied.

    Color(IN BYTE r,
          IN BYTE g,
          IN BYTE b)
    {
        Argb = MakeARGB(255, r, g, b);
    }

    Color(IN BYTE a,
          IN BYTE r,
          IN BYTE g,
          IN BYTE b)
    {
        Argb = MakeARGB(a, r, g, b);
    }

    Color(IN ARGB argb)
    {
        Argb = argb;
    }

    BYTE GetAlpha() const
    {
        return (BYTE) (Argb >> AlphaShift);
    }

    BYTE GetA() const
    {
        return GetAlpha();
    }

    BYTE GetRed() const
    {
        return (BYTE) (Argb >> RedShift);
    }

    BYTE GetR() const
    {
        return GetRed();
    }

    BYTE GetGreen() const
    {
        return (BYTE) (Argb >> GreenShift);
    }

    BYTE GetG() const
    {
        return GetGreen();
    }

    BYTE GetBlue() const
    {
        return (BYTE) (Argb >> BlueShift);
    }

    BYTE GetB() const
    {
        return GetBlue();
    }

    ARGB GetValue() const
    {
        return Argb;
    }

    VOID SetValue(IN ARGB argb)
    {
        Argb = argb;
    }

    VOID SetFromCOLORREF(IN COLORREF rgb);

    COLORREF ToCOLORREF() const
    {
        return RGB(GetRed(), GetGreen(), GetBlue());
    }

    static COLORREF ToCOLORREF(ARGB argb);

    void Darken()
    {
        float r = GetR() * 2.0f / 3.0f;
        float g = GetG() * 2.0f / 3.0f;
        float b = GetB() * 2.0f / 3.0f;
        SetValue(MakeARGB(GetA(), static_cast<BYTE>(r), static_cast<BYTE>(g), static_cast<BYTE>(b)));
    }

    void Lighten()
    {
        float r = std::min<float>(GetR() * 2.0f, 255.0f);
        float g = std::min<float>(GetG() * 2.0f, 255.0f);
        float b = std::min<float>(GetB() * 2.0f, 255.0f);
        SetValue(MakeARGB(GetA(), static_cast<BYTE>(r), static_cast<BYTE>(g), static_cast<BYTE>(b)));
    }

    void MoreBright(WORD step=1);
    void LessBright(WORD step=1);
    void MoreSaturated(WORD step=1);
    void LessSaturated(WORD step=1);
    WORD GetHue() const;
    WORD GetLuminance() const;
    WORD GetSaturation() const;
    void GetHLS(WORD hls[3]) const;

    void Complementary(Color& comp);
    bool IsDarker(const Color& color) const;
    bool IsLighter(const Color& color) const;

public:

    // Common color constants
    
    enum
    {
        AliceBlue            = 0xFFF0F8FF,
        AntiqueWhite         = 0xFFFAEBD7,
        Aqua                 = 0xFF00FFFF,
        Aquamarine           = 0xFF7FFFD4,
        Azure                = 0xFFF0FFFF,
        Beige                = 0xFFF5F5DC,
        Bisque               = 0xFFFFE4C4,
        Black                = 0xFF000000,
        BlanchedAlmond       = 0xFFFFEBCD,
        Blue                 = 0xFF0000FF,
        BlueViolet           = 0xFF8A2BE2,
        Brown                = 0xFFA52A2A,
        BurlyWood            = 0xFFDEB887,
        CadetBlue            = 0xFF5F9EA0,
        Chartreuse           = 0xFF7FFF00,
        Chocolate            = 0xFFD2691E,
        Coral                = 0xFFFF7F50,
        CornflowerBlue       = 0xFF6495ED,
        Cornsilk             = 0xFFFFF8DC,
        Crimson              = 0xFFDC143C,
        Cyan                 = 0xFF00FFFF,
        DarkBlue             = 0xFF00008B,
        DarkCyan             = 0xFF008B8B,
        DarkGoldenrod        = 0xFFB8860B,
        DarkGray             = 0xFFA9A9A9,
        DarkGreen            = 0xFF006400,
        DarkKhaki            = 0xFFBDB76B,
        DarkMagenta          = 0xFF8B008B,
        DarkOliveGreen       = 0xFF556B2F,
        DarkOrange           = 0xFFFF8C00,
        DarkOrchid           = 0xFF9932CC,
        DarkRed              = 0xFF8B0000,
        DarkSalmon           = 0xFFE9967A,
        DarkSeaGreen         = 0xFF8FBC8B,
        DarkSlateBlue        = 0xFF483D8B,
        DarkSlateGray        = 0xFF2F4F4F,
        DarkTurquoise        = 0xFF00CED1,
        DarkViolet           = 0xFF9400D3,
        DeepPink             = 0xFFFF1493,
        DeepSkyBlue          = 0xFF00BFFF,
        DimGray              = 0xFF696969,
        DodgerBlue           = 0xFF1E90FF,
        Firebrick            = 0xFFB22222,
        FloralWhite          = 0xFFFFFAF0,
        ForestGreen          = 0xFF228B22,
        Fuchsia              = 0xFFFF00FF,
        Gainsboro            = 0xFFDCDCDC,
        GhostWhite           = 0xFFF8F8FF,
        Gold                 = 0xFFFFD700,
        Goldenrod            = 0xFFDAA520,
        Gray                 = 0xFF808080,
        Green                = 0xFF008000,
        GreenYellow          = 0xFFADFF2F,
        Honeydew             = 0xFFF0FFF0,
        HotPink              = 0xFFFF69B4,
        IndianRed            = 0xFFCD5C5C,
        Indigo               = 0xFF4B0082,
        Ivory                = 0xFFFFFFF0,
        Khaki                = 0xFFF0E68C,
        Lavender             = 0xFFE6E6FA,
        LavenderBlush        = 0xFFFFF0F5,
        LawnGreen            = 0xFF7CFC00,
        LemonChiffon         = 0xFFFFFACD,
        LightBlue            = 0xFFADD8E6,
        LightCoral           = 0xFFF08080,
        LightCyan            = 0xFFE0FFFF,
        LightGoldenrodYellow = 0xFFFAFAD2,
        LightGray            = 0xFFD3D3D3,
        LightGreen           = 0xFF90EE90,
        LightPink            = 0xFFFFB6C1,
        LightSalmon          = 0xFFFFA07A,
        LightSeaGreen        = 0xFF20B2AA,
        LightSkyBlue         = 0xFF87CEFA,
        LightSlateGray       = 0xFF778899,
        LightSteelBlue       = 0xFFB0C4DE,
        LightYellow          = 0xFFFFFFE0,
        Lime                 = 0xFF00FF00,
        LimeGreen            = 0xFF32CD32,
        Linen                = 0xFFFAF0E6,
        Magenta              = 0xFFFF00FF,
        Maroon               = 0xFF800000,
        MediumAquamarine     = 0xFF66CDAA,
        MediumBlue           = 0xFF0000CD,
        MediumOrchid         = 0xFFBA55D3,
        MediumPurple         = 0xFF9370DB,
        MediumSeaGreen       = 0xFF3CB371,
        MediumSlateBlue      = 0xFF7B68EE,
        MediumSpringGreen    = 0xFF00FA9A,
        MediumTurquoise      = 0xFF48D1CC,
        MediumVioletRed      = 0xFFC71585,
        MidnightBlue         = 0xFF191970,
        MintCream            = 0xFFF5FFFA,
        MistyRose            = 0xFFFFE4E1,
        Moccasin             = 0xFFFFE4B5,
        NavajoWhite          = 0xFFFFDEAD,
        Navy                 = 0xFF000080,
        OldLace              = 0xFFFDF5E6,
        Olive                = 0xFF808000,
        OliveDrab            = 0xFF6B8E23,
        Orange               = 0xFFFFA500,
        OrangeRed            = 0xFFFF4500,
        Orchid               = 0xFFDA70D6,
        PaleGoldenrod        = 0xFFEEE8AA,
        PaleGreen            = 0xFF98FB98,
        PaleTurquoise        = 0xFFAFEEEE,
        PaleVioletRed        = 0xFFDB7093,
        PapayaWhip           = 0xFFFFEFD5,
        PeachPuff            = 0xFFFFDAB9,
        Peru                 = 0xFFCD853F,
        Pink                 = 0xFFFFC0CB,
        Plum                 = 0xFFDDA0DD,
        PowderBlue           = 0xFFB0E0E6,
        Purple               = 0xFF800080,
        Red                  = 0xFFFF0000,
        RosyBrown            = 0xFFBC8F8F,
        RoyalBlue            = 0xFF4169E1,
        SaddleBrown          = 0xFF8B4513,
        Salmon               = 0xFFFA8072,
        SandyBrown           = 0xFFF4A460,
        SeaGreen             = 0xFF2E8B57,
        SeaShell             = 0xFFFFF5EE,
        Sienna               = 0xFFA0522D,
        Silver               = 0xFFC0C0C0,
        SkyBlue              = 0xFF87CEEB,
        SlateBlue            = 0xFF6A5ACD,
        SlateGray            = 0xFF708090,
        Snow                 = 0xFFFFFAFA,
        SpringGreen          = 0xFF00FF7F,
        SteelBlue            = 0xFF4682B4,
        Tan                  = 0xFFD2B48C,
        Teal                 = 0xFF008080,
        Thistle              = 0xFFD8BFD8,
        Tomato               = 0xFFFF6347,
        Transparent          = 0x00FFFFFF,
        Turquoise            = 0xFF40E0D0,
        Violet               = 0xFFEE82EE,
        Wheat                = 0xFFF5DEB3,
        White                = 0xFFFFFFFF,
        WhiteSmoke           = 0xFFF5F5F5,
        Yellow               = 0xFFFFFF00,
        YellowGreen          = 0xFF9ACD32
    };

    // Shift count and bit mask for A, R, G, B components
    
    enum
    {
        AlphaShift  = 24,
        RedShift    = 16,
        GreenShift  = 8,
        BlueShift   = 0
    };

    enum
    {
        AlphaMask   = 0xff000000,
        RedMask     = 0x00ff0000,
        GreenMask   = 0x0000ff00,
        BlueMask    = 0x000000ff
    };

    // Assemble A, R, G, B values into a 32-bit integer
    
    static ARGB MakeARGB(IN BYTE a,
                         IN BYTE r,
                         IN BYTE g,
                         IN BYTE b)
    {
        return (((ARGB) (b) <<  BlueShift) |
                ((ARGB) (g) << GreenShift) |
                ((ARGB) (r) <<   RedShift) |
                ((ARGB) (a) << AlphaShift));
    }

    unsigned long ToBGR()
    {
        return RGB(GetBlue(), GetGreen(), GetRed());
    }


protected:

    ARGB Argb;
};

CLIB_API bool Intersect_Lines(const GraphTypes::RectF & r,const GraphTypes::PointF & p2,const GraphTypes::PointF & p3, GraphTypes::PointF & p);
CLIB_API unsigned long ToBGR(COLORREF rgb);

class CRectEx : public CRect
{
public:
    CRectEx() : CRect(0, 0, 0, 0)
    {
    }

    CRectEx(const CRect & r) : CRect(r)
    {
    }
};
class CSizeEx : public CSize
{
public:
    CSizeEx() : CSize(0, 0)
    {
    }

    CSizeEx(const CSize & s) : CSize(s)
    {
    }
};

//-----------------------------------------------------------------------------

__interface IBitmap	: public IUnknown
{
    WTL::CBitmap* GetCBitmap();
    bool IsValid() const;
    bool GetSize(SIZE& size) const;
    void* GetBitsPtr() const;
    const BITMAPINFO& GetBitmapInfo() const;
};

CLIB_API IBitmap* CreateIBitmap(HBITMAP hBitmap);

};

class CAutoPen
{
protected:
    WTL::CDCHandle &m_dc;
    WTL::CPen &m_pen;
    HPEN m_oldPen;

public:
    CAutoPen(WTL::CPen &newPen, WTL::CDCHandle &dc) : m_dc(dc), m_pen(newPen)
    {
        m_oldPen = m_dc.SelectPen(m_pen);
    }

    ~CAutoPen()
    {
#ifndef _WIN32_WCE
        if (m_oldPen == NULL || ::GetObjectType(m_oldPen) == OBJ_PEN || ::GetObjectType(m_oldPen) == OBJ_EXTPEN)
            m_dc.SelectPen(m_oldPen);
#else
        if (m_oldPen == NULL || ::GetObjectType(m_oldPen) == OBJ_PEN)
            m_dc.SelectPen(m_oldPen);
#endif
    }
};

class CAutoBrush
{
protected:
    WTL::CDCHandle &m_dc;
    WTL::CBrush &m_brush;
    HBRUSH m_oldBrush;

public:
    CAutoBrush(WTL::CBrush &newBrush, WTL::CDCHandle &dc) : m_dc(dc), m_brush(newBrush)
    {
        m_oldBrush = m_dc.SelectBrush(m_brush);
    }

    ~CAutoBrush()
    {
        if (m_oldBrush == NULL || ::GetObjectType(m_oldBrush) == OBJ_BRUSH)
            m_dc.SelectBrush(m_oldBrush);
    }
};

class CAutoFont
{
protected:
    WTL::CDCHandle &m_dc;
    WTL::CFont &m_font;
    HFONT m_oldFont;

public:
    CAutoFont(WTL::CFont &newFont, WTL::CDCHandle &dc) : m_dc(dc), m_font(newFont)
    {
        m_oldFont = m_dc.SelectFont(m_font);
    }

    ~CAutoFont()
    {
        if (m_oldFont == NULL || ::GetObjectType(m_oldFont) == OBJ_FONT)
            m_dc.SelectFont(m_oldFont);
    }
};

#ifndef _WIN32_WCE
class HTMLElement
{
protected:
    CComPtr<IHTMLElementRender> m_htmlElement;
    GraphTypes::SizeF m_size;

public:
    HTMLElement() 
    {
    }
    HTMLElement(const HTMLElement & c) 
    {
        *this = c;
    }
    HTMLElement & operator = (const HTMLElement & c)
    {
        m_htmlElement = c.m_htmlElement;
        m_size = c.m_size;
        return *this;
    }
    bool operator < (const HTMLElement & c) const
    {
        return m_htmlElement.p < c.m_htmlElement.p;
    }

    HTMLElement(IHTMLElementRender* htmlElement, float width, float height) : m_htmlElement(htmlElement), m_size(width, height) 
    {
    }
    IHTMLElementRender * GetHtmlElement() const
    {
        return m_htmlElement;
    }
    GraphTypes::SizeF GetSize() const
    {
        return m_size;
    }
};
#endif

struct ImageInfo
{
    ImageInfo()
        : image(0), 
        pos(POSITION_WEST), 
        marginX(0), 
        marginY(0),
        size(0,0),
        transcolor(0),
        opaqueness(0xFF)
    {}

#ifdef _DEBUG
    ~ImageInfo() {}
#endif

    ImageInfo(const ImageInfo& info)
        : image(info.image),
        pos(info.pos),
        marginX(info.marginX),
        marginY(info.marginY),
        size(info.size),
        transcolor(info.transcolor),
        opaqueness(info.opaqueness)
    {
    }

    ImageInfo& operator=(const ImageInfo& info)
    {
        image=info.image;
        pos=info.pos;
        marginX=info.marginX;
        marginY=info.marginY;
        size=info.size;
        transcolor=info.transcolor;
        opaqueness=info.opaqueness;

        return *this;
    }

    void SetTransparentColor(COLORREF tc) 
    {
        transcolor=tc;
        opaqueness=0;
    }

    bool IsTransparent() const { return opaqueness==0; }

    void GetSize(SIZE& size_) const
    { 
        if(size.cx==0 && size.cy==0)
          image->GetSize(size);
        size_=size;
    }

    void SetSize(const SIZE& size_) 
    {
        size=size_;
    }

    void GetSourceSize(SIZE& size_) const
    {
        image->GetSize(size_);
    }

    CComPtr<GraphTypes::IBitmap> image;
    IMAGE_POSITION pos;
    int marginX;
    int marginY;
    mutable CSize size;
    COLORREF transcolor;
    // if 0xFF fully opaque, if 0x00, then transcolor is transparent color
    unsigned int opaqueness; 

};


typedef std::map<unsigned int, ImageInfo> MapImageInfo;
typedef std::map<unsigned int, MapImageInfo> ImageInfoMaps;
