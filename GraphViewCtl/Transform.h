#pragma once

#include <GraphTypes.h>

typedef boost::tuples::tuple<int, int, int> DpiScale;

class CTransform
{
private:
	int m_dpiX;
	int m_dpiY;
	float m_scale;
	int m_rotation;
	int m_fontPointSize;
	GraphTypes::PointF m_spacing;
	GraphTypes::PointF m_offset;
	DpiScale m_dpiScale;

	// composite transformation matrix for points
	GraphTypes::REAL m11, m12, m13, m21, m22, m23, m31, m32, m33; 
	// its inverse
	GraphTypes::REAL im11, im12, im13, im21, im22, im23, im31, im32, im33; 

	// size (width, height) transformation matrix and its inverse
	GraphTypes::REAL sm11, sm22;
	GraphTypes::REAL ism11, ism22;

	// rotation matrix and its inverse
	GraphTypes::REAL mR11, mR12, mR21, mR22;
	GraphTypes::REAL miR11, miR12, miR21, miR22;

	void MakeComposite();

public:
	CTransform();
	virtual ~CTransform();

	void SetDPI(int x, int y);
	int GetDPIX() const;
	int GetDPIY() const;
	bool SetScale(float scale);
	float GetScale() const;
	bool SetRotation(int rotation);
	int GetRotation() const;
	bool SetSpacing(float spacing);
	bool SetSpacing(const GraphTypes::PointF& spacing);
	float GetSpacing() const;
	const GraphTypes::PointF& GetSpacingPoint() const;
	void SetFontPointSize(int fontSize);
	int GetFontPointSize() const;
	void SetOffset(const GraphTypes::PointF & offset);
	const GraphTypes::PointF & GetOffset() const;

	const DpiScale & GetDpiScale() const;

	void TranslateTransform(GraphTypes::PointF &p) const;
	void RotateTransform(GraphTypes::PointF &p) const;
	void SpacingTransform(GraphTypes::PointF &p) const;
	void ScaleTransform(GraphTypes::PointF &p) const;
	void DpiTransform(GraphTypes::PointF &p) const;
	void AllTransform(GraphTypes::PointF &p) const;

	void RTranslateTransform(GraphTypes::PointF &p) const;
	void RRotateTransform(GraphTypes::PointF &p) const;
	void RSpacingTransform(GraphTypes::PointF &p) const;
	void RScaleTransform(GraphTypes::PointF &p) const;
	void RDpiTransform(GraphTypes::PointF &p) const;
	void RAllTransform(GraphTypes::PointF &p) const;
	void SizeTransform(GraphTypes::PointF &p) const;
	void RSizeTransform(GraphTypes::PointF &p) const;

	void TranslateTransform(GraphTypes::RectF &p) const;
	void RotateTransform(GraphTypes::RectF &p) const;
	void SpacingTransform(GraphTypes::RectF &p) const;
	void ScaleTransform(GraphTypes::RectF &p) const;
	void DpiTransform(GraphTypes::RectF &p) const;
	void AllTransform(GraphTypes::RectF &p) const;

	void RTranslateTransform(GraphTypes::RectF &p) const;
	void RRotateTransform(GraphTypes::RectF &p) const;
	void RSpacingTransform(GraphTypes::RectF &p) const;
	void RScaleTransform(GraphTypes::RectF &p) const;
	void RDpiTransform(GraphTypes::RectF &p) const;
	void RAllTransform(GraphTypes::RectF &p) const;

	int WidthTransform(int w) const;
	void SizeTransform(GraphTypes::SizeF &size) const;
	void RSizeTransform(GraphTypes::SizeF &size) const;

	void SpacingTransform(GraphTypes::SizeF &sz) const;
	void ScaleTransform(GraphTypes::SizeF &sz) const;

	static void InitBaseDpi(void);
	static int m_dpiXbase, m_dpiYbase;
};
