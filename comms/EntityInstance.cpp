#include "StdAfx.h"

#include "EntityInstance.h"
#include "analysis.h"
#include "SoapUtil.h"
#include "clib.h"
#include "thread.h"
#include "cache.h"
#include "logger.h"
#include "util.h"
#include "mshtmdid.h"

static _ATL_FUNC_INFO OnMouseOverInfo = {CC_STDCALL, VT_EMPTY, 1, { VT_DISPATCH } };

namespace Analysis
{
class CEntityInstance : public IDispEventSimpleImpl<0, CEntityInstance, &DIID_HTMLElementEvents2>, public IEntityInstance, public clib::CLockableUnknown
{
protected:
	CComPtr<IWebPage> m_webPage;
	CComPtr<IEntity> m_entity;
	CComPtr<IMarkupPointer> m_textNode;
	CComPtr<IHTMLElement> m_highlightElem;
	std::_tstring m_url;
	std::_tstring m_id;
	std::_tstring m_html;
	std::_tstring m_escapedHtml;

	int m_docID;
	int m_pos;
	int m_x;
	int m_y;

	CComQIPtr<IHTMLDocument2> m_doc2;
	CComQIPtr<IMarkupServices> m_markupServices;
	CComQIPtr<IMarkupContainer> m_markupContainer;
	CComPtr<IMarkupPointer> m_lhs;
	CComPtr<IMarkupPointer> m_rhs;
	bool m_inited;
	bool m_highlighted;
	std::_tstring m_context;

public:
	BEGIN_CUNKNOWN
		IMPLEMENT_INTERFACE(IEntityInstance)
		IMPLEMENT_INTERFACE(IDispatch)
	END_CUNKNOWN(clib::CLockableUnknown)

	BEGIN_SINK_MAP(CEntityInstance)
		SINK_ENTRY_INFO(0, DIID_HTMLElementEvents2, DISPID_HTMLELEMENTEVENTS2_ONMOUSEOVER, OnMouseOver, &OnMouseOverInfo)
	END_SINK_MAP()

	CEntityInstance(const IWebPage * webPage, const IEntity * entity, const IMarkupPointer * textNode, const std::wstring & context, int docID, int pos)
	{
		m_webPage = const_cast<IWebPage *>(webPage);			//TODO:  gjs
		m_entity = const_cast<IEntity *>(entity);				//TODO:  gjs
		m_textNode = const_cast<IMarkupPointer *>(textNode);	//TODO:  gjs
		m_context = CW2T(context.c_str(), CP_UTF8);
		m_inited = false;
		m_highlighted = false;

		m_docID = docID;
		m_pos = pos;
		m_id = std::_tstring(entity->GetID()) + _T("/") + boost::lexical_cast<std::_tstring>(m_docID) + _T("/") + boost::lexical_cast<std::_tstring>(m_pos);
		boost::algorithm::replace_all(m_id, _T("&"), _T("_"));
		m_x = 0;
		m_y = 0;
	}

	void __stdcall OnMouseOver(IDispatch * e)
	{
		m_entity->GetSummary(m_html, false, false);
		EscapeXML(m_html, m_escapedHtml, true);
		RestoreHTML();
		Highlight();
		return;
	}

	void ConnectEvents(IHTMLElement* pElem)
	{
		HRESULT hr;
		IConnectionPointContainer* pCPC = NULL;
		IConnectionPoint* pCP = NULL;
		DWORD dwCookie;

		// Check that this is a connectable object.
		hr = pElem->QueryInterface(IID_IConnectionPointContainer, (void**)&pCPC);

		if (SUCCEEDED(hr))
		{
			// Find the connection point.
			hr = pCPC->FindConnectionPoint(DIID_HTMLElementEvents2, &pCP);

			if (SUCCEEDED(hr))
			{
				// Advise the connection point.
				// pUnk is the IUnknown interface pointer for your event sink
				CComQIPtr<IEntityInstance> pUnk = this;
				hr = pCP->Advise(pUnk, &dwCookie);

				if (SUCCEEDED(hr))
				{
					// Successfully advised
				}

				pCP->Release();
			}

			pCPC->Release();
		}
	} 

	void Init()
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		m_inited = true;
		HRESULT result = m_textNode->GetContainer(&m_markupContainer);
		ATLASSERT(SUCCEEDED(result));
		m_markupServices = m_markupContainer;
		result = m_markupContainer->OwningDoc(&m_doc2);
		ATLASSERT(SUCCEEDED(result));
		result = m_markupServices->CreateMarkupPointer(&m_lhs);
		ATLASSERT(SUCCEEDED(result));
		result = m_markupServices->CreateMarkupPointer(&m_rhs);
		ATLASSERT(SUCCEEDED(result));
		result = m_lhs->MoveToPointer(m_textNode);
		ATLASSERT(SUCCEEDED(result));
		result = m_rhs->MoveToPointer(m_textNode);
		ATLASSERT(SUCCEEDED(result));

		for(int i = 1; i < m_pos; ++i)
		{
			result = m_lhs->MoveUnit(MOVEUNIT_NEXTCHAR);
			ATLASSERT(SUCCEEDED(result));
		}
		//m_lhs->FindText(m_entity->GetOrig());

		result = m_rhs->MoveToPointer(m_lhs);
		ATLASSERT(SUCCEEDED(result));
		std::_tstring entityText = GetEntity()->GetOrig();
		for(unsigned int i = 0; i < entityText.length(); ++i)
		{
			result = m_rhs->MoveUnit(MOVEUNIT_NEXTCHAR);
			ATLASSERT(SUCCEEDED(result));
		}

		result = m_lhs->SetGravity(POINTER_GRAVITY_Right);
		ATLASSERT(SUCCEEDED(result));
		result = m_rhs->SetGravity(POINTER_GRAVITY_Left);
		ATLASSERT(SUCCEEDED(result));

		//result = m_markupServices->CreateElement(TAGID_SPAN, (wchar_t *)(boost::wformat(L"onmouseover=\"return makeFalse(domTT_activate(this, event, 'caption', '%1%', 'content', '%2%', 'type', 'sticky', 'id', '%3%', 'closeLink', '[close]', 'direction', 'south', 'offsetY', -15));\"") % m_entity->GetCategory() % m_escapedHtml.c_str() % GetID()).str().c_str(), &m_highlightElem);
		//ATLASSERT(SUCCEEDED(result));
	}

	bool Inited() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_inited;
	}

	const TCHAR *GetID() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_id.c_str();
	}

	const TCHAR * GetCacheID() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_id.c_str();
	}

	IEntity * GetEntity() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_entity;
	}

	//IHTMLDOMTextNode * GetDOMTextNode() const
	//{
	//	clib::recursive_mutex::scoped_lock proc(m_mutex);
	//	return m_textNode;
	//}

	int GetDocID() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_docID;
	}

	int GetPos() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_pos;
	}

	int GetX() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_x;
	}

	void SetX(int x)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		m_x = x;
	}

	int GetY() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_y;
	}

	void SetY(int y)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		m_y = y;
	}

	void RestoreHTML()
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		if (!m_inited)
			Init();
		if (m_highlighted)
		{
			m_markupServices->RemoveElement(m_highlightElem);
			m_highlighted = false;
			m_highlightElem.Release();
		}
	}

	void Highlight()
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		if (!m_inited)
			Init();
		if (!m_highlighted)
		{
			BOOL lhsPos, rhsPos, checkLHS, checkRHS;
			HRESULT result = m_lhs->IsPositioned(&lhsPos);
			ATLASSERT(SUCCEEDED(result));
			result = m_rhs->IsPositioned(&rhsPos);
			ATLASSERT(SUCCEEDED(result));
			result = m_lhs->IsLeftOf(m_rhs, &checkLHS);
			ATLASSERT(SUCCEEDED(result));
			result = m_rhs->IsRightOf(m_lhs, &checkRHS);
			ATLASSERT(SUCCEEDED(result));
			ATLASSERT(lhsPos);
			ATLASSERT(rhsPos);
			ATLASSERT(checkLHS);
			ATLASSERT(checkRHS);
			ATLASSERT(!m_highlightElem);

			CComBSTR attrs = (boost::wformat(L"onmouseover=\"return makeFalse(domTT_activate(this, event, 'caption', '%1%', 'content', '%2%', 'type', 'sticky', 'id', '%3%', 'closeLink', '[close]', 'direction', 'south', 'offsetY', -15));\"") % m_entity->GetCategory() % m_escapedHtml.c_str() % GetID()).str().c_str();
			result = m_markupServices->CreateElement(TAGID_SPAN, attrs, &m_highlightElem);
			ATLASSERT(SUCCEEDED(result));

			CComPtr<IHTMLStyle> style;
			result = m_highlightElem->get_style(&style);
			ATLASSERT(SUCCEEDED(result));
			CComVariant highlightColor = GetEntity()->GetResolved() ? 0x66ffff : 0xffff66;
			result = style->put_backgroundColor(highlightColor);
			ATLASSERT(SUCCEEDED(result));

			result = m_markupServices->InsertElement(m_highlightElem, m_lhs, m_rhs);
			ATLASSERT(SUCCEEDED(result));
			ConnectEvents(m_highlightElem);

			m_highlighted = true;
		}
	}

	void SetFocus()
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		if (!m_inited)
			Init();

		if (m_highlighted)
		{
			CComVariant top = VARIANT_TRUE;
			m_highlightElem->scrollIntoView(top);
		}
	}

	const TCHAR * GetContext()
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_context.c_str();
	}

	void Lock(CComPtr<clib::scoped_lock_ref_counted> & lock) 
	{ 
		return CLockableUnknown::Lock(lock); 
	}
};

static CacheT<std::_tstring, CEntityInstance> EntityCache;

IEntityInstance * CreateIEntityInstance(const IWebPage * webPage, const IEntity * entity, const IMarkupPointer * textNode, const std::wstring & context, int docID, int pos)
{
	CEntityInstance *entityInstance = EntityCache.Get(new CEntityInstance(webPage, entity, textNode, context, docID, pos));
	return entityInstance;
}
}