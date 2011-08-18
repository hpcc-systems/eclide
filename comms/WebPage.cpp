#include "StdAfx.h"

#include "webpage.h"
#include "analysis.h"
#include "SoapUtil.h"
#include "clib.h"
#include "thread.h"
#include "cache.h"
#include "logger.h"

namespace Analysis
{
//  ===========================================================================
#define THREAD_COUNT 5
#define BATCH_SIZE 1000

__interface IHTMLWalker
{
	void StartNode(IHTMLDOMNode * node);
	void EndNode(IHTMLDOMNode * node);
};
typedef CComPtr<IHTMLDOMTextNode> IHTMLDOMTextNodePtr;
typedef CAdapt<IHTMLDOMTextNodePtr> IHTMLDOMTextNodeAdapt;
typedef std::vector<IHTMLDOMTextNodeAdapt> IHTMLDOMTextNodeVector;
typedef std::map<std::_tstring, IEntityAdapt> StringIEntityMap;
typedef std::map<std::_tstring, IEntityInstanceAdapt> StringIEntityInstanceMap;
//  ===========================================================================
class AnalyzeWalker : public IHTMLWalker
{
protected:
	IHTMLDOMTextNodeVector * m_textNodes;

public:
	AnalyzeWalker(IHTMLDOMTextNodeVector * textNodes)
	{
		m_textNodes = textNodes;
	}

	void GetText(IHTMLDOMNode * node, CString & text)
	{
		CComVariant value;
		node->get_nodeValue(&value);
		text = value;
	}

	void StartNode(IHTMLDOMNode * node)
	{
		if (CComQIPtr<IHTMLDOMTextNode> textNode = node)
		{
			m_textNodes->push_back(textNode);
		}
	}

	void EndNode(IHTMLDOMNode * node)
	{
	}
};
//  ===========================================================================
class CWebPage : public IWebPage, public clib::CLockableUnknown, public boost::signals::trackable
{
protected:
	CComPtr<IHTMLDocument> m_doc;
	CComQIPtr<IHTMLDocument2> m_doc2;
	CComQIPtr<IHTMLDocument3> m_doc3;
	std::_tstring m_id;
	webpage_signal_type on_refresh;
	StringIEntityMap m_entities;
	StringIEntityInstanceMap m_entityInstances;
	std::_tstring m_resolveResult;
	clib::CThreadQueue m_classifyThreads;
	bool m_classified;
	clib::CThreadQueue m_resolveThreads;
	bool m_resolved;
	std::_tstring m_tpl;
	std::_tstring m_statsHTML;

public:
	BEGIN_CUNKNOWN
		IMPLEMENT_INTERFACE(IWebPage)
	END_CUNKNOWN(clib::CLockableUnknown)

public:
	CWebPage(IHTMLDocument * doc) : m_classifyThreads(THREAD_COUNT), m_resolveThreads(THREAD_COUNT)
	{
		m_doc = doc;
		m_doc2 = doc;
		m_doc3 = doc;

		CComBSTR bstr_id;
		m_doc3->get_uniqueID(&bstr_id);
		m_id = CW2T(bstr_id, CP_UTF8);
		//boost::algorithm::to_lower(m_id);
		//boost::algorithm::trim(m_id);
		m_classified = false;
		m_resolved = false;
		m_classifyThreads.MonitorEvents(boost::ref(*this));
		m_resolveThreads.MonitorEvents(boost::ref(*this));
	}

	~CWebPage()
	{
		//  Big temp hack
		m_doc3.Detach();
		m_doc2.Detach();
		m_doc.Detach();
	}

	const TCHAR * GetID() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_id.c_str();
	}

	const TCHAR * GetCacheID() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_id.c_str();
	}

	static void threadClassify(CComPtr<CWebPage> self, ClassifyStructVector request)
	{	//Background Thread
		CComPtr<IServer> server = AttachAnalysis();
		server->Classify(self, request);
	}

	STATE GetState() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		if(m_resolved && m_resolveThreads.Size() == 0)
			return STATE_RESOLVED;
		if(m_resolved && m_resolveThreads.Size() > 0)
			return STATE_RESOLVING;
		else if (m_classified && m_classifyThreads.Size() == 0)
			return STATE_CLASSIFIED;
		else if (m_classified && m_classifyThreads.Size() > 0)
			return STATE_CLASSIFIYING;
		return STATE_UNKNOWN;
	}

	int GetClassifyJobCount() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_classifyThreads.Size();
	}

	static void PrepRequest(IHTMLDocument * doc, ClassifyStructVector & request)
	{
		CComQIPtr<IMarkupContainer> markupContainer = doc;
		CComQIPtr<IMarkupServices> markupServices = markupContainer;
		CComPtr<IMarkupPointer> finger;
		CComPtr<IMarkupPointer> prev;
		CComPtr<IMarkupPointer> end;
		markupServices->CreateMarkupPointer(&finger);
		markupServices->CreateMarkupPointer(&prev);
		markupServices->CreateMarkupPointer(&end);
		finger->MoveToContainer(markupContainer, true);
		prev->MoveToContainer(markupContainer, true);
		end->MoveToContainer(markupContainer, false);

		BOOL finished = false;
		HRESULT hr = S_OK;
		while(hr == S_OK && !finished)
		{
			MARKUP_CONTEXT_TYPE pContext = CONTEXT_TYPE_None;
			long pcch = -1;
			prev->MoveToPointer(finger);
			hr = finger->Right(true, &pContext, NULL, &pcch, NULL);
			if (pContext == CONTEXT_TYPE_Text && pcch >= 1)
			{
				ClassifyStruct newItem;
				newItem.text.resize(pcch);
				prev->Right(false, NULL, NULL, &pcch, &newItem.text[0]);
				std::wstring testStr = newItem.text;
				boost::algorithm::trim(testStr);
				if (testStr.length())
				{
					markupServices->CreateMarkupPointer(&newItem.textNode);
					newItem.textNode->MoveToPointer(prev);
					request.push_back(newItem);
				}
			}
			finger->IsRightOfOrEqualTo(end, &finished);
		}
	}

	bool InsertScript(IHTMLDocument * doc, const CComBSTR & scriptText)
	{
		if (CComQIPtr<IHTMLDocument3> doc3 = doc)
		{
			CComPtr<IHTMLElementCollection> pNodes; 
			doc3->getElementsByTagName(L"head", &pNodes);
			long length = 0;
			pNodes->get_length(&length);
			if(length == 1)
			{
				CComPtr<IDispatch> pDisp2;
				pNodes->item(CComVariant(0), CComVariant(0), &pDisp2);
				CComQIPtr<IHTMLDOMNode> headNode = pDisp2;
				CComQIPtr<IHTMLElement> headElement = pDisp2;

				CComQIPtr<IMarkupServices> muServ = doc;
				CComPtr<IMarkupPointer> lhs;
				CComPtr<IMarkupPointer> rhs;
				muServ->CreateMarkupPointer(&lhs);
				muServ->CreateMarkupPointer(&rhs);

				CComPtr<IHTMLElement> scriptElement;
				doc3->getElementById(L"LNBandScript", &scriptElement);
				if (!scriptElement)
				{
					lhs->MoveAdjacentToElement(headElement, ELEM_ADJ_BeforeEnd);
					rhs->MoveAdjacentToElement(headElement, ELEM_ADJ_BeforeEnd);
					CComPtr<IHTMLElement> newScript;
					muServ->CreateElement(TAGID_SCRIPT, L"id=\"LNBandScript\" language=\"javascript\" type=\"text/javascript\"", &newScript);
					CComQIPtr<IHTMLScriptElement> script = newScript;
					script->put_text(scriptText);
					HRESULT result = muServ->InsertElement(newScript, lhs, rhs);
					ATLASSERT(result == S_OK);
				}

				return true;
			}
		}
		return false;
	}

	void StartClassify(const CComBSTR & scriptText)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		if (m_classified)
		{
			proc.unlock();
			on_refresh(this, EVENT_CLASSIFIED);
			return;
		}
		m_classified = true;
		ClassifyStructVector request;
		
		//Process Main Frame
		InsertScript(m_doc, scriptText);
		PrepRequest(m_doc, request);
		
		//Process Child Frames
		CComQIPtr<IOleContainer> pContainer = m_doc;
		if (!pContainer)
			return;

		CComPtr<IEnumUnknown> pEnumerator;
		HRESULT hr = pContainer->EnumObjects(OLECONTF_EMBEDDINGS, &pEnumerator);
		if (FAILED(hr))
			return;

		CComPtr<IUnknown> pUnk;
		ULONG uFetched;
		for (UINT i = 0; S_OK == pEnumerator->Next(1, &pUnk, &uFetched); i++)
		{
			CComQIPtr<IWebBrowser2> pBrowser = pUnk;
			if (pBrowser)
			{
				CComPtr<IDispatch> spDispDoc;
				HRESULT hr = pBrowser->get_Document(&spDispDoc);
				if (SUCCEEDED(hr))
				{
					// ...and query for an HTML document.
					CComQIPtr<IHTMLDocument> spHTMLDoc = spDispDoc;
					if (spHTMLDoc)
					{
						InsertScript(spHTMLDoc, scriptText);
						PrepRequest(spHTMLDoc, request);
					}
				}
			}
			pUnk.Release();
		}

		m_classifyThreads.SetMaxThreadCount(0);
		ClassifyStructVector batchRequest;
		int textLen = 0;
		for(ClassifyStructVector::iterator itr = request.begin(); itr != request.end(); ++itr)
		{
			textLen += itr->text.length();
			batchRequest.push_back(*itr);
			if (textLen > BATCH_SIZE)
			{
				m_classifyThreads.Append(__FUNCTION__, boost::bind(&threadClassify, this, batchRequest));
				batchRequest.clear();
				textLen = 0;
			}
		}
		if (batchRequest.size())
		{
			m_classifyThreads.Append(__FUNCTION__, boost::bind(&threadClassify, this, batchRequest));
			batchRequest.clear();
		}
		m_classifyThreads.SetMaxThreadCount(THREAD_COUNT);
		//int textLen = 0;
		//BOOL finished = false;
		//HRESULT hr = S_OK;
		//while(hr == S_OK && !finished)
		//{
		//	MARKUP_CONTEXT_TYPE pContext = CONTEXT_TYPE_None;
		//	long pcch = -1;
		//	prev->MoveToPointer(finger);
		//	hr = finger->Right(true, &pContext, NULL, &pcch, NULL);
		//	if (pContext == CONTEXT_TYPE_Text && pcch > 1)
		//	{
		//		ClassifyStruct newItem;
		//		newItem.text.resize(pcch);
		//		prev->Right(false, NULL, NULL, &pcch, &newItem.text[0]);
		//		std::wstring testStr = newItem.text;
		//		boost::algorithm::trim(testStr);
		//		if (testStr.length())
		//		{
		//			markupServices->CreateMarkupPointer(&newItem.textNode);
		//			newItem.textNode->MoveToPointer(prev);
		//			request.push_back(newItem);
		//			textLen += pcch;
		//			if (textLen > 1000)
		//			{
		//				m_classifyThreads.Append(__FUNCTION__, boost::bind(&threadClassify, this, request));
		//				request.clear();
		//				textLen = 0;
		//			}
		//		}
		//	}
		//	finger->IsRightOfOrEqualTo(end, &finished);
		//}
		//if (request.size())
		//{
		//	m_classifyThreads.Append(__FUNCTION__, boost::bind(&threadClassify, this, request));
		//	request.clear();
		//}
		//m_classifyThreads.SetMaxThreadCount(THREAD_COUNT);
	}

	void AppendEntity(IEntity * entity)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		m_entities[entity->GetID()] = entity;
	}

	void AppendEntityInstance(IEntityInstance * entity)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		m_entityInstances[entity->GetID()] = entity;
	}

	static void threadResolve(CWebPage * self, Analysis::IEntityVector entities)
	{
		CComPtr<Analysis::IServer> server = Analysis::AttachAnalysis();
		server->Resolve(self, entities);
	}

	void StartResolve()
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		if (m_resolved)
		{
			proc.unlock();
			on_refresh(this, EVENT_RESOLVED);
			return;
		}
		m_resolved = true;
		Analysis::IEntityVector entities;
		GetEntities(&entities);
		m_resolveThreads.Append(__FUNCTION__, boost::bind(&threadResolve, this, entities));
	}

	void SetResolveResult(const std::_tstring & resolveResult)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		m_resolveResult = resolveResult;
	}

	const TCHAR * GetResolveResults() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_resolveResult.c_str();
	}

	void GetEntities(IEntityVector * entities) const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		for(StringIEntityMap::const_iterator itr = m_entities.begin(); itr != m_entities.end(); ++itr)
		{
			entities->push_back(itr->second);
		}
	}

	void GetEntityInstances(IEntityInstanceVector * instances) const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		for(StringIEntityInstanceMap::const_iterator itr = m_entityInstances.begin(); itr != m_entityInstances.end(); ++itr)
		{
			instances->push_back(itr->second);
		}
	}

	void SetStatsHTML(const std::_tstring & statsHTML)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		m_statsHTML = statsHTML;
	}

	const TCHAR * GetStatsHTML()
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_statsHTML.c_str();
	}

	const TCHAR * GetHtml(std::_tstring & html)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		CComPtr<IHTMLElement> element;
		m_doc3->get_documentElement(&element);
		CComBSTR bstr_html;
		element->get_innerHTML(&bstr_html);
		html = CW2T(bstr_html);
		return html.c_str();
	}

	void SetPopupTemplate(const std::_tstring & tpl)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		m_tpl = tpl;
	}

	const TCHAR *GetPopupTemplate()
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_tpl.c_str();
	}

	void Update()
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		proc.unlock();
	}

	boost::signals::connection on_refresh_connect(const webpage_slot_type& s)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return on_refresh.connect(s); 
	}

	void Lock(CComPtr<clib::scoped_lock_ref_counted> & lock) 
	{ 
		return CLockableUnknown::Lock(lock); 
	}

	void operator()(clib::CThreadQueue * threadQueue, clib::THREAD_EVENT evt)
	{
		if (threadQueue == &m_classifyThreads)
		{
			switch(evt)
			{
			case clib::THREAD_EVENT_END:
				on_refresh(this, EVENT_CLASSIFIED);
			}
		}
		else if (threadQueue == &m_resolveThreads)
		{
			switch(evt)
			{
			case clib::THREAD_EVENT_END:
				on_refresh(this, EVENT_RESOLVED);
			}
		}
	}
};

static CacheT<std::_tstring, CWebPage> WebPageCache;

IWebPage * CreateIWebPage(IHTMLDocument * doc)
{
	if (!doc)
		return NULL;

	CWebPage *webpage = WebPageCache.Get(new CWebPage(doc));
	webpage->Update();
	return webpage;
}

IWebPage * FindIWebPage(IHTMLDocument * doc)
{
	CComPtr<CWebPage> webpage = new CWebPage(doc);
	return WebPageCache.Exists(webpage->GetCacheID());
}

COMMS_API void ClearIWebPageCache()
{
	WebPageCache.clear();
}
//  ===========================================================================
}