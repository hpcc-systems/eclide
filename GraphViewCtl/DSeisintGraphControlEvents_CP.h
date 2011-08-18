#pragma once

#include "GraphViewCtl.h"

template<class T>
class CProxyDSeisintGraphControlEvents :
	public IConnectionPointImpl<T, &__uuidof(DSeisintGraphControlEvents)>
{
public:
	//CProxyDSeisintGraphControlEvents() {}

	HRESULT Fire_OnEdgeContextMenu(long x, long y, IEdge* Edge)
	{
		return Fire_XYObjectEvent(DISPID_EdgeContextMenu, x, y, Edge);
	}
	HRESULT Fire_OnEdgeDoubleClick(long x, long y, IEdge* Edge)
	{
		return Fire_XYObjectEvent(DISPID_EdgeDoubleClick, x, y, Edge);
	}
	HRESULT Fire_OnGraphContextMenu(long x, long y)
	{
		return Fire_XYEvent(DISPID_GraphContextMenu, x, y);
	}
	HRESULT Fire_OnGraphDoubleClick(long x, long y)
	{
		return Fire_XYEvent(DISPID_GraphDoubleClick, x, y);
	}
	HRESULT Fire_OnVertexContextMenu(long x, long y, IVertex* pVal)
	{
		return Fire_XYObjectEvent(DISPID_VertexContextMenu, x, y, pVal);
	}
	HRESULT Fire_OnVertexDoubleClick(long x, long y, IVertex* pVal)
	{
		return Fire_XYObjectEvent(DISPID_VertexDoubleClick, x, y, pVal);
	}
	HRESULT Fire_OnVertexSingleClick(long x, long y, IVertex* pVal)
	{
		return Fire_XYObjectEvent(DISPID_VertexSingleClick, x, y, pVal);
	}
	HRESULT Fire_OnVertexMouseOver(long x, long y, IVertex* pVal)
	{
		return Fire_XYObjectEvent(DISPID_VertexMouseOver, x, y, pVal);
	}
	HRESULT Fire_OnGraphMouseOver(long x, long y)
	{
		return Fire_XYEvent(DISPID_GraphMouseOver, x, y);
	}
	HRESULT Fire_OnMetaDataLoaded()
	{
		return Fire_Event(DISPID_MetaDataLoaded);
	}
	HRESULT Fire_OnLayoutFinished()
	{
		return Fire_Event(DISPID_LayoutFinished);
	}
	HRESULT Fire_OnSubgraphMinClick(ISubgraph* pVal)
	{
		return Fire_ObjectEvent(DISPID_SubgraphMinClick, pVal);
	}
protected:
	HRESULT Fire_Event(int id)
	{
		HRESULT hr = S_OK;
		T * pThis = static_cast<T *>(this);
		int cConnections = m_vec.GetSize();

		for (int iConnection = 0; iConnection < cConnections; iConnection++)
		{
			pThis->Lock();
			CComPtr<IUnknown> punkConnection = m_vec.GetAt(iConnection);
			pThis->Unlock();

			IDispatch * pConnection = static_cast<IDispatch *>(punkConnection.p);

			if (pConnection)
			{
				CComVariant varResult;
				DISPPARAMS params = { NULL, NULL, 0, 0 };
				hr = pConnection->Invoke(id, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, &varResult, NULL, NULL);
			}
		}
		return hr;
	}
	HRESULT Fire_XYEvent(int id, long x, long y)
	{
		HRESULT hr = S_OK;
		T * pThis = static_cast<T *>(this);
		int cConnections = m_vec.GetSize();

		for (int iConnection = 0; iConnection < cConnections; iConnection++)
		{
			pThis->Lock();
			CComPtr<IUnknown> punkConnection = m_vec.GetAt(iConnection);
			pThis->Unlock();

			IDispatch * pConnection = static_cast<IDispatch *>(punkConnection.p);

			if (pConnection)
			{
				CComVariant varResult;
				CComVariant args[2];
				DISPPARAMS params = { args, NULL, 2, 0 };
				args[1] = x; 
				args[0] = y;
				hr = pConnection->Invoke(id, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, &varResult, NULL, NULL);
			}
		}
		return hr;
	}
	HRESULT Fire_ObjectEvent(int id, IDispatch* pDispatch)
	{
		HRESULT hr = S_OK;
		T * pThis = static_cast<T *>(this);
		int cConnections = m_vec.GetSize();

		for (int iConnection = 0; iConnection < cConnections; iConnection++)
		{
			pThis->Lock();
			CComPtr<IUnknown> punkConnection = m_vec.GetAt(iConnection);
			pThis->Unlock();

			IDispatch * pConnection = static_cast<IDispatch *>(punkConnection.p);

			if (pConnection)
			{
				CComVariant varResult;
				CComVariant args[3];
				DISPPARAMS params = { args, NULL, 1, 0 };
				args[0] = pDispatch;
				hr = pConnection->Invoke(id, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, &varResult, NULL, NULL);
			}
		}
		return hr;
	}
	HRESULT Fire_XYObjectEvent(int id, long x, long y, IDispatch* pDispatch)
	{
		HRESULT hr = S_OK;
		T * pThis = static_cast<T *>(this);
		int cConnections = m_vec.GetSize();

		for (int iConnection = 0; iConnection < cConnections; iConnection++)
		{
			pThis->Lock();
			CComPtr<IUnknown> punkConnection = m_vec.GetAt(iConnection);
			pThis->Unlock();

			IDispatch * pConnection = static_cast<IDispatch *>(punkConnection.p);

			if (pConnection)
			{
				CComVariant varResult;
				CComVariant args[3];
				DISPPARAMS params = { args, NULL, 3, 0 };
				args[2] = x; 
				args[1] = y;
				args[0] = pDispatch;
				hr = pConnection->Invoke(id, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, &varResult, NULL, NULL);
			}
		}
		return hr;
	}
};
