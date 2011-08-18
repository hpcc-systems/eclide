#include "StdAfx.h"
#include "Transform.h"


int CTransform::m_dpiXbase;
int CTransform::m_dpiYbase;

CTransform::CTransform() : 
	m_dpiX(72),
	m_dpiY(72),
	m_scale(1.0f),
	m_spacing(1.0f,1.0f),
	m_rotation(0),
	m_fontPointSize(DEFAULT_FONT_SIZE)
{
	InitBaseDpi();
	/// NB. if you remove this call MakeComposite() instead
	SetRotation(180);
}

CTransform::~CTransform()
{
}

void CTransform::InitBaseDpi()
{
	CWindowDC dc(::GetDesktopWindow());
	m_dpiXbase=GetDeviceCaps(dc, LOGPIXELSX);
	m_dpiYbase=GetDeviceCaps(dc, LOGPIXELSY);
}

void CTransform::SetDPI(int x, int y)
{
	// this gets called often with identical params

	if(m_dpiX!=x || m_dpiY!=y)
	{
		m_dpiX = x;
		m_dpiY = y;
		m_dpiScale = DpiScale(m_dpiX, m_dpiY, m_fontPointSize);
		MakeComposite();
	}
}

int CTransform::GetDPIX() const
{
	return m_dpiX;
}

int CTransform::GetDPIY() const
{
	return m_dpiY;
}

bool CTransform::SetScale(float scale)
{
	if (m_scale != scale)
	{
		m_scale = scale;
		SetFontPointSize((int)(ceil(DEFAULT_FONT_SIZE * m_scale)));
		MakeComposite();
		return true;
	}
	return false;
}

float CTransform::GetScale() const
{
	return m_scale;
}

bool CTransform::SetRotation(int rotation)
{
	if (m_rotation != rotation)
	{
		m_rotation = rotation;

		GraphTypes::REAL radians=ToRadian(m_rotation);
		GraphTypes::REAL cosinus=cos(radians);
		GraphTypes::REAL sinus=sin(radians);

		mR11=cosinus;
		mR12=-sinus;
		mR21=sinus;
		mR22=cosinus;

		miR11=cosinus;
		miR12=sinus;
		miR21=-sinus;
		miR22=cosinus;

		MakeComposite();
		return true;
	}
	return false;
}

int CTransform::GetRotation() const
{
	return m_rotation;
}

bool CTransform::SetSpacing(float spacing)
{
	if (m_spacing.x != spacing || m_spacing.y != spacing)
	{
		m_spacing.x = m_spacing.y = spacing;
		MakeComposite();
		return true;
	}
	return false;
}

bool CTransform::SetSpacing(const GraphTypes::PointF& spacing)
{
	if (m_spacing.x != spacing.x || m_spacing.y != spacing.y)
	{
		m_spacing = spacing;
		MakeComposite();
		return true;
	}
	return false;
}

float CTransform::GetSpacing() const
{
	ATLASSERT(m_spacing.x==m_spacing.y);
	return m_spacing.x;
}

const GraphTypes::PointF& CTransform::GetSpacingPoint() const
{
	return m_spacing;
}

void CTransform::SetFontPointSize(int fontSize)
{
	if (fontSize < 1)
		fontSize = 1;
	else if (fontSize > DEFAULT_FONT_SIZE)
		fontSize = DEFAULT_FONT_SIZE;
	m_fontPointSize = fontSize;
	m_dpiScale = DpiScale(m_dpiX, m_dpiY, m_fontPointSize);
}

int CTransform::GetFontPointSize() const
{
	return m_fontPointSize;
}

void CTransform::SetOffset(const GraphTypes::PointF & offset)
{
	// this gets called often with identical params
	if(m_offset.x!=offset.x || m_offset.y!=offset.y)
	{
		m_offset = offset;
		MakeComposite();
	}
}

const GraphTypes::PointF & CTransform::GetOffset() const
{
	return m_offset;
}

const DpiScale & CTransform::GetDpiScale() const
{
	return m_dpiScale;
}

void CTransform::TranslateTransform(GraphTypes::PointF &p) const
{
	p.x -= m_offset.x;
	p.y -= m_offset.y;
}

void CTransform::RotateTransform(GraphTypes::PointF &p) const
{
	GraphTypes::PointF newP;
	newP.x = p.x * mR11 + p.y * mR12;
	newP.y = p.x * mR21 + p.y * mR22;
	p = newP;
}

void CTransform::SpacingTransform(GraphTypes::PointF &p) const
{
	p.x *= m_spacing.x;
	p.y *= m_spacing.y;
}

void CTransform::ScaleTransform(GraphTypes::PointF &p) const
{
	p.x *= m_scale;
	p.y *= m_scale;
}

void CTransform::DpiTransform(GraphTypes::PointF &p) const
{
	if (m_dpiX && m_dpiY)
	{
		p.x = (p.x * m_dpiX) / m_dpiXbase;
		p.y = (p.y * m_dpiY) / m_dpiYbase;
	}
}

void CTransform::AllTransform(GraphTypes::PointF &p) const
{
/*
	SpacingTransform(p);
	RotateTransform(p);
	ScaleTransform(p);
	TranslateTransform(p);
	DpiTransform(p);
*/
	GraphTypes::PointF tmp(p);

	p.x=m11 * tmp.x + m12 * tmp.y + m13;
	p.y=m21 * tmp.x + m22 * tmp.y + m23;

//	GraphTypes::PointF tmp=p, tmp1=p;
//	tmp1.x=m11*tmp.x + m12*tmp.y + m13;
//	tmp1.y=m21*tmp.x + m22*tmp.y + m23;

//	GraphTypes::REAL diffx=tmp1.x-p.x;
//	GraphTypes::REAL diffy=tmp1.y-p.y;
//	if(fabs(diffx)>0.001 || fabs(diffy)>0.001)
//		ATLASSERT(0);
}

void CTransform::RTranslateTransform(GraphTypes::PointF &p) const
{
	p.x += m_offset.x;
	p.y += m_offset.y;
}

void CTransform::RRotateTransform(GraphTypes::PointF &p) const
{
	GraphTypes::PointF newP;
	newP.x = p.x * miR11 + p.y * miR12;
	newP.y = p.x * miR21 + p.y * miR22;
	p = newP;
}

void CTransform::RSpacingTransform(GraphTypes::PointF &p) const
{
	p.x /= m_spacing.x;
	p.y /= m_spacing.y;
}

void CTransform::RScaleTransform(GraphTypes::PointF &p) const
{
	p.x /= m_scale;
	p.y /= m_scale;
}

void CTransform::RDpiTransform(GraphTypes::PointF &p) const
{
	if (m_dpiX && m_dpiY)
	{
		p.x = (p.x * m_dpiXbase) / m_dpiX;
		p.y = (p.y * m_dpiYbase) / m_dpiY;
	}
}

void CTransform::SizeTransform(GraphTypes::PointF &p) const
{
	p.x *=sm11;
	p.y *=sm22;
}

void CTransform::RSizeTransform(GraphTypes::PointF &p) const
{
	p.x *=ism11;
	p.y *=ism22;
}

void CTransform::RAllTransform(GraphTypes::PointF &p) const
{
/*
	RDpiTransform(p);
	RTranslateTransform(p);
	RScaleTransform(p);
	RRotateTransform(p);
	RSpacingTransform(p);
*/
	GraphTypes::PointF tmp(p);
	p.x=im11 * tmp.x + im12 * tmp.y + im13;
	p.y=im21 * tmp.x + im22 * tmp.y + im23;
}

void CTransform::TranslateTransform(GraphTypes::RectF &p) const
{
	p.x -= m_offset.x;
	p.y -= m_offset.y;
}

void CTransform::RotateTransform(GraphTypes::RectF &p) const
{
	GraphTypes::RectF newP;
	newP.x = p.x * mR11 + p.y * mR12;
	newP.y = p.x * mR21 + p.y * mR22;
	newP.Width = p.Width;
	newP.Height = p.Height;
	p = newP;
}

void CTransform::SpacingTransform(GraphTypes::RectF &p) const
{
	p.x *= m_spacing.x;
	p.y *= m_spacing.y;
}

void CTransform::ScaleTransform(GraphTypes::RectF &p) const
{
	p.x *= m_scale;
	p.y *= m_scale;
	p.Width *= m_scale;
	p.Height *= m_scale;
}

void CTransform::DpiTransform(GraphTypes::RectF &p) const
{
	if (m_dpiX && m_dpiY)
	{
		p.x = (p.x * m_dpiX) / m_dpiXbase;
		p.y = (p.y * m_dpiY) / m_dpiYbase;
		p.Width = (p.Width * m_dpiX) / m_dpiXbase;
		p.Height = (p.Height * m_dpiY) / m_dpiYbase;
	}
}

void CTransform::AllTransform(GraphTypes::RectF &p) const
{
/*
	SpacingTransform(p);
	RotateTransform(p);
	ScaleTransform(p);
	TranslateTransform(p);
	DpiTransform(p);
*/
	GraphTypes::PointF origin(p.x, p.y);
	p.x=m11 * origin.x + m12 * origin.y + m13;
	p.y=m21 * origin.x + m22 * origin.y + m23;

	p.Width*=sm11;
	p.Height*=sm22;
}

void CTransform::RTranslateTransform(GraphTypes::RectF &p) const
{
	p.x += m_offset.x;
	p.y += m_offset.y;
}
void CTransform::RRotateTransform(GraphTypes::RectF &p) const
{
	GraphTypes::RectF newP;
	newP.x = p.x * miR11 + p.y * miR12;
	newP.y = p.x * miR21 + p.y * miR22;
	newP.Width = p.Width;
	newP.Height = p.Height;
	p = newP;
}
void CTransform::RSpacingTransform(GraphTypes::RectF &p) const
{
	p.x /= m_spacing.x;
	p.y /= m_spacing.y;
}
void CTransform::RScaleTransform(GraphTypes::RectF &p) const
{
	p.x /= m_scale;
	p.y /= m_scale;
	p.Width /= m_scale;
	p.Height /= m_scale;
}
void CTransform::RDpiTransform(GraphTypes::RectF &p) const
{
	if (m_dpiX && m_dpiY)
	{
		p.x = (p.x * m_dpiXbase) / m_dpiX;
		p.y = (p.y * m_dpiYbase) / m_dpiY;
		p.Width = (p.Width * m_dpiXbase) / m_dpiX;
		p.Height = (p.Height * m_dpiYbase) / m_dpiY;
	}
}
void CTransform::RAllTransform(GraphTypes::RectF &p) const
{
/*
	RDpiTransform(p);
	RTranslateTransform(p);
	RScaleTransform(p);
	RRotateTransform(p);
	RSpacingTransform(p);
*/
	GraphTypes::PointF topleft(p.x, p.y);
	p.x=im11 * topleft.x + im12 * topleft.y + im13;
	p.y=im21 * topleft.x + im22 * topleft.y + im23;

	p.Width*=ism11;
	p.Height*=ism22;
}

int CTransform::WidthTransform(int w) const
{
	float retVal = static_cast<float>(w) * m_scale;	//Scale
	retVal = (retVal * m_dpiX) / m_dpiXbase;		//DPI
	return static_cast<int>(retVal);
}

void CTransform::SpacingTransform(GraphTypes::SizeF &sz) const
{
	sz.Width *= m_spacing.x;
	sz.Height *= m_spacing.y;
}

void CTransform::ScaleTransform(GraphTypes::SizeF &sz) const
{
	sz.Width *= m_scale;
	sz.Height *= m_scale;
}

void CTransform::SizeTransform(GraphTypes::SizeF &size) const
{
	size.Width *=sm11;
	size.Height *=sm22;
}

void CTransform::RSizeTransform(GraphTypes::SizeF &size) const
{
	size.Width *=ism11;
	size.Height *=ism22;
}


void CTransform::MakeComposite()
{
	// initialize to unit (rotation and translation will initialize other elems)
	m31=0; m32=0; m33=1;
	im31=0; im32=0; im33=1;

	// rotation with spacing 

	m11=mR11*m_spacing.x;
	m12=mR12*m_spacing.y;
	m21=mR21*m_spacing.x;
	m22=mR22*m_spacing.y;

	// scale

	m11*=m_scale; 
	m12*=m_scale; 

	m21*=m_scale;
	m22*=m_scale;

	// translation

	m13=-m_offset.x;
	m23=-m_offset.y;

	// DPI

	GraphTypes::REAL dpx=static_cast<GraphTypes::REAL>(m_dpiX)/static_cast<GraphTypes::REAL>(m_dpiXbase);
	GraphTypes::REAL dpy=static_cast<GraphTypes::REAL>(m_dpiY)/static_cast<GraphTypes::REAL>(m_dpiYbase);

	m11*=dpx;
	m12*=dpx;

	m21*=dpy;
	m22*=dpy;

	m13*=dpx;
	m23*=dpy;

	// INVERSE 

	// translation, DPI, scale
	GraphTypes::REAL tmpim13=m_offset.x/(dpx*m_scale);
	GraphTypes::REAL tmpim23=m_offset.y/(dpy*m_scale);

	// rotation

	im11=miR11/(dpx*m_scale);
	im12=miR12/(dpx*m_scale);
	im21=miR21/(dpy*m_scale);
	im22=miR22/(dpy*m_scale);

	im13=tmpim13*miR11 + tmpim23*miR12;
	im23=tmpim13*miR21 + tmpim23*miR22;

	// spacing

	im11/=m_spacing.x;
	im12/=m_spacing.y;
	im21/=m_spacing.x;
	im22/=m_spacing.y;

	im13/=m_spacing.x;
	im23/=m_spacing.y;

	// size transformation matrices

	sm11=dpx*m_scale;
	sm22=dpy*m_scale;
	ism11=1/sm11;
	ism22=1/sm22;
}