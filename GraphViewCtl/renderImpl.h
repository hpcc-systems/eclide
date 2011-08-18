#pragma once

#include "meta.h"
#include "render.h"
#include "GraphContainer.h"
#include "MetaContainer.h"
#include "Renderer.h"
#include "Transform.h"

class CRender : public IRender, public CUnknown, public CTransform
{
	friend class CVertexRenderer;
	friend class CLegacyVertexRenderer;
	friend class CListWindowRenderer;
	friend class CEdgeRenderer;
	friend class CGraphRenderer;
	friend class CIconRenderer;
	friend class CImageRenderer;
	friend class CLegacyEdgeRenderer;
	friend class CLegacySubgraphRenderer;
	friend class CLegacyGraphRenderer;

protected:
	CComPtr<IGraph> m_graph;
	CMeta &m_meta;
	
	std::map<int, CIconHandle> m_metaIcon;
	std::map<int, HTMLElement> m_metaIconHtmlElement;
	bool m_draft;

	// TODO: do these belong here?
	CComPtr<IVertexPath> m_overToSel;
	CUniqueID m_overNode;
	CUniqueID m_focusNode;

public:
	CRender(IGraph * graph, CMeta & meta);

	IMPLEMENT_CUNKNOWN;

	IRenderer * CreateISubgraphRenderer(IGraphSubgraph * sg) { return 0; }
	IRenderer * CreateIVertexRenderer(IGraphVertex * v) { return 0; }
	IRenderer * CreateIEdgeRenderer(IGraphEdge * e) { return 0; }

	void SetMetaVertexLabel(unsigned cat, const CString & label);	//What the category means ("Person", "Company" etc.)
	void GetMetaVertexLabel(unsigned cat, CString &label);
	void SetMetaVertexIcon(unsigned cat, IPicture* icon);	
	void SetMetaVertexIconPlacement(PLACEMENT placementPosition);
	void SetMetaVertexImagePlacement(PLACEMENT placementPosition);
	void SetMetaVertexHtmlElement(unsigned cat, HTMLElement& icon);
	void SetMetaVertexColorFillNorm(unsigned cat, unsigned long color);
	void SetMetaVertexColorBorderNorm(unsigned cat, unsigned long color);
	void SetMetaVertexColorFillSel(unsigned cat, unsigned long color);
	void SetMetaVertexColorBorderSel(unsigned cat, unsigned long color);
	void SetMetaVertexColorFillOver(unsigned cat, unsigned long color);
	void SetMetaVertexColorBorderOver(unsigned cat, unsigned long color);
	void SetMetaVertexHide(unsigned cat, bool hide);
	bool GetMetaVertexHide(unsigned cat);

	void SetMetaEdgeLabel(unsigned cat, const CString & label);	//What the category means ("Relative", "Lived At", "On the Board" etc.)
	void SetMetaEdgeColorNorm(unsigned cat, unsigned long color);
	void SetMetaEdgeColorShortest(unsigned cat, unsigned long color);
	void SetMetaEdgeWidth(unsigned cat, unsigned short width);

	VertexAt getVertexAt(const GraphTypes::PointF &p, const CUniqueID & overNode);
	VertexAt getVertexAt(const CPoint &p, const CUniqueID & overNode);
	void SelectItemsWithin(const GraphTypes::RectF &r);
	GraphTypes::RectF getGraphExtent(bool selectedOnly) const;

	// utility methods
	GraphTypes::RectF DrawBitmap(CDCHandle& dc,	const GraphTypes::RectF& rText,	IPicture* image,
		const HTMLElement* element,	const PLACEMENT& placement);

	void AdjustOffsets(GraphTypes::RectF& rOut,	const GraphTypes::RectF& rIn, PLACEMENT placement);

//	virtual void append(const CUniqueID & id, const GraphTypes::RectF & r);

//	virtual void remove(const CUniqueID & id);

//	virtual GraphTypes::RectF getRect(const CUniqueID & id);
//	virtual GraphTypes::SizeF getSize(const CUniqueID & id);
//	virtual void setRect(const CUniqueID & id, const GraphTypes::RectF & r);
//	virtual void setIconRect(const CUniqueID & id, const GraphTypes::RectF & r);
//	virtual void setImageRect(const CUniqueID & id, const GraphTypes::RectF & r);

	virtual void SetTopParent(HWND hwnd) {}

	float getDraft() const { return m_draft; }
	void setDraft(bool draft) { m_draft = draft; }

	float getScale() const { return GetScale(); }
	void setScale(float scale) { SetScale(scale); }
	float getSpacing() const { return GetSpacing(); }
	void setSpacing(float spacing) { SetSpacing(spacing); }
	int getRotation() const { return GetRotation(); };
	void setRotation(int rotation) { SetRotation(rotation); };
	int getDpiX() const { return GetDPIX(); }
	int getDpiY() const { return GetDPIY(); }
	void AllTransform(GraphTypes::PointF &p) const {CTransform::AllTransform(p); };
	void AllTransform(GraphTypes::RectF &p) const {CTransform::AllTransform(p); };
	void RAllTransform(GraphTypes::PointF &p) const {CTransform::RAllTransform(p); };
	void RAllTransform(GraphTypes::RectF &p) const {CTransform::RAllTransform(p); };
	void RDpiTransform(GraphTypes::RectF &p) const {CTransform::RDpiTransform(p); };
};

