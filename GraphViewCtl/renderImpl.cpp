#include "stdafx.h"
#include "resource.h"

#include "meta.h"
#include "renderImpl.h"

using namespace GraphTypes;

CRender::CRender(IGraph * graph, CMeta & meta) : m_graph(graph),
	m_meta(meta),
	m_draft(false)
{
}

VertexAt CRender::getVertexAt(const GraphTypes::PointF &p, const CUniqueID & overNode) 
{
	CComPtr<boost::scoped_lock_ref_counted> lock;
	VertexAt retVal(CUniqueID(), HITTEST_UNKNOWN);

	CComPtr<IGraphItem> item;
	CComPtr<IUnknown> unk;
	CComPtr<IRenderer> ir;
	CComQIPtr<ILegacyRenderer> lv;
	RectF rc;

	IGraphItemVector results;
	m_graph->GetChildren(results);

	for(IGraphItemVector::iterator itr=results.begin(); itr!=results.end(); ++itr)
	{
		item=*itr;	

		CUniqueID id = item->GetID();
		unk=item->GetPropertyUnknown(0);

		if(unk!=0)
		{
			lv=unk.p;
			if(lv!=0)
			{
				lv->GetTitlePos(rc);

				if (rc.Contains(p))
				{
					retVal = VertexAt(id, HITTEST_LABEL);
				}
				else 
				{
					lv->GetImagePos(rc);
					if (rc.Contains(p))
					{
						retVal = VertexAt(id, HITTEST_ICON);
					}
					else
					{
						lv->GetImagePos(rc);
						if(rc.Contains(p))
						{
							retVal = VertexAt(id, HITTEST_IMAGE);
						}
					}
				}
			}
			else 
			{
				ir=static_cast<IRenderer*>(unk.p);
				if(ir!=0)
				{
					ir->GetBoundsBox(rc);
	
					if (rc.Contains(p))
						retVal = VertexAt(id, HITTEST_BBOX);
				}
			}
		}
	}

	return retVal;
}

VertexAt CRender::getVertexAt(const CPoint &p, const CUniqueID & overNode) 
{
	return getVertexAt(PointF((float)p.x, (float)p.y), overNode);
}

void CRender::SelectItemsWithin(const RectF &r)
{
	CComPtr<boost::scoped_lock_ref_counted> lock;
	CComPtr<IGraphItem> item;
	CComPtr<IUnknown> unk;
	CComQIPtr<IRenderer> ir;
	RectF box;

	IGraphItemVector results;
	m_graph->GetChildren(results);

	for(IGraphItemVector::iterator itr=results.begin(); itr!=results.end(); ++itr)
	{
		item=*itr;			
		unk=item->GetPropertyUnknown(0);

		if(unk)
		{
			ir=static_cast<IRenderer*>(unk.p);
			if(ir!=0)
			{
				ir->GetBoundsBox(box);
				if(box.IntersectsWith(r))
					ir->SetSelected(true, true);
			}
		}
	}
}

RectF CRender::getGraphExtent(bool selectedOnly) const
{
	RectF retVal;
	bool first = true;
	CComPtr<boost::scoped_lock_ref_counted> lock;

	CComPtr<IGraphItem> item;
	CComPtr<IUnknown> unk;
	CComQIPtr<IListWindowRenderer> lw;
	CComQIPtr<ILegacyRenderer> lv;

	RectF rc;
	IGraphItemVector results;
	m_graph->GetChildren(results);

	for(IGraphItemVector::iterator itr=results.begin(); itr!=results.end(); ++itr)
	{
		item=*itr;	

		CUniqueID id = item->GetID();
		unk=item->GetPropertyUnknown(0);

		if(unk!=0)
		{
			lv=unk.p;
			if(lv!=0)
			{
				if (selectedOnly && !lv->IsSelected())
				{
					continue;
				}
			
				if (first)
				{
					lv->GetTitlePos(retVal);
					first = false;
				}
				else
				{
					lv->GetTitlePos(rc);
					retVal.Union(retVal, retVal, rc);
				}

				lv->GetIconPos(rc);
				retVal.Union(retVal, retVal, rc);
				lv->GetImagePos(rc);
				retVal.Union(retVal, retVal, rc);
			}
			else
			{
				lw=unk.p;
				if(lw!=0)
				{
					if (selectedOnly && !lw->IsSelected())
					{
						continue;
					}

					if (first)
					{
						lw->GetBoundsBox(retVal);
						first = false;
					}
					else
					{
						lw->GetBoundsBox(rc);
						retVal.Union(retVal, retVal, rc);
					}
				}
			}
		}
	}

	return retVal;
}

//-----------------------------------------------------------------------------

void CRender::AdjustOffsets(
	RectF& rOut,
	const RectF& rIn,
	PLACEMENT placement)
{
	switch (placement)
	{
	case PLACEMENT_LEFT:
		rOut.x = rIn.x - rOut.Width;
		rOut.y = rIn.y;
		break;
	case PLACEMENT_RIGHT:
		rOut.x = rIn.x + rIn.Width;
		rOut.y = rIn.y;
		break;
	case PLACEMENT_TOP:
		rOut.x = rIn.x + rIn.Width / 2 - rOut.Width / 2;
		rOut.y = rIn.y - rOut.Height + 1;
		break;
	case PLACEMENT_BOTTOM:
		rOut.x = rIn.x + rIn.Width / 2 - rOut.Width / 2;
		rOut.y = rIn.y + rIn.Height;
		break;
	}
}

//-----------------------------------------------------------------------------

RectF CRender::DrawBitmap(
	CDCHandle& dc,
	const RectF& rText,
	IPicture* image,
	const HTMLElement* element,
	const PLACEMENT& placement)
{
	if (image == NULL && (element == NULL || element->GetHtmlElement() == NULL))
		return rText;

	LONG width, height;
	RectF rImg; 
	if (image != NULL) 
	{
		image->get_Width( &width );
		image->get_Height( &height );
		CSize szPic(width, height);
		// convert OLE size into pixels
		dc.HIMETRICtoDP( &szPic );
		rImg.Width = static_cast<float>(szPic.cx);
		rImg.Height = static_cast<float>(szPic.cy);
	} 
	else if (element != NULL)
	{
		rImg = element->GetSize();
	}

	HRESULT hr = S_OK;
	//COLORREF transparent = RGB(253,0,200);
	COLORREF transparent = RGB(0,0,211); // C# transparent gif background color
	CMemBm bmp(dc, 0, 0, 0, (int)rImg.Width, (int)rImg.Height);
	CDCHandle hdcMem = bmp;
	hdcMem.FillSolidRect(0, 0, (int)rImg.Width, (int)rImg.Height, transparent);
	if (image != NULL) 
	{
		hr = image->Render(
			hdcMem,
			0,
			long(rImg.Height),
			long(rImg.Width),
			long(-rImg.Height),
			0,
			0,
			width, height, NULL );
	} 
	else if (element != NULL && element->GetHtmlElement() != NULL) 
	{
		hr = element->GetHtmlElement()->DrawToDC(hdcMem);
	}
	//TODO over support
	if (/*m_overNode != v ||*/ GetScale() > 1.0f) 
	{
		ScaleTransform(rImg);
	}

	AdjustOffsets(rImg, rText, placement);
	bmp.DrawTrans(
		dc,
		int(rImg.x),
		int(rImg.y),
		int(rImg.Width),
		int(rImg.Height),
		transparent);
	return rImg;
}
