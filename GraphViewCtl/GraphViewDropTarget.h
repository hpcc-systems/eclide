#pragma once

#include "DragDropImpl.h"

__interface IGraphViewSlot;

class CGraphViewDropTarget : public CIDropTarget
{
	SGV::IGraphViewSlot* m_Owner;

public:
	CGraphViewDropTarget(HWND hTargetWnd)
	  : CIDropTarget(hTargetWnd), m_Owner(0)
	{
	}	
	
	//----------------------------------------------------------------------------

	virtual bool OnDrop(
		FORMATETC* pFmtEtc,
		STGMEDIUM& medium,
		DWORD *pdwEffect)
	{
		if(pFmtEtc->cfFormat == CF_TEXT && medium.tymed == TYMED_HGLOBAL)
		{
			if(m_Owner)
				m_Owner->OnDrop(medium.hGlobal);
		}

		return true; // let base free the medium
	}

	//----------------------------------------------------------------------------

	void SetOwner(SGV::IGraphViewSlot* owner)
	{
		m_Owner=owner; 
	}

	//----------------------------------------------------------------------------

	virtual HRESULT STDMETHODCALLTYPE DragOver( 
        DWORD grfKeyState,
        POINTL pt,
        DWORD __RPC_FAR *pdwEffect)
	{
		//ScreenToClient(m_hTargetWnd,&hit.pt);
		return CIDropTarget::DragOver(grfKeyState, pt, pdwEffect);
	}

	//----------------------------------------------------------------------------

	virtual HRESULT STDMETHODCALLTYPE DragLeave(void)
	{
		return CIDropTarget::DragLeave();
	}

};
