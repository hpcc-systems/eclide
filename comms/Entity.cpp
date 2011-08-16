#include "StdAfx.h"

#include "entity.h"
#include "analysis.h"
#include "SoapUtil.h"
#include "clib.h"
#include "thread.h"
#include "cache.h"
#include "logger.h"
#include "util.h"

namespace Analysis
{
const TCHAR * const ENTITY_TYPE_STRING[ENTITY_TYPE_LAST + 1] = 
{
	_T("Unknown"),
	_T("Person"), 
	_T("Company"),
	_T("Vin"), 
	_T("Phone"),
	_T("Email"),
	_T("eId"),
	_T("Address"),
	_T("City"),
	_T("State"),
	_T("SSN"),
	_T("FEIN"),
	_T("Country"),
	_T("URL"),
	_T("Docket"),
	_T("Date"),
	_T("Street"),
	_T("First Name"),
	_T("Starsign"),
	_T("Age"),
	_T("Ticker"),
	_T("Last")
};

class CEntity : public IEntity, public clib::CLockableUnknown
{
protected:
	CComPtr<IWebPage> m_webPage;
	std::_tstring m_id;

	std::_tstring m_orig;
	int m_classify_score;
	ENTITY_TYPE m_entity_type;
	std::_tstring m_category;
	std::_tstring m_cleanedInput;
	unsigned int m_localEntityNumber;
	bool m_verified;
	bool m_resolved;
	StlLinked<ITable> m_identities;
	std::_tstring m_serverResults;
	StlLinked<ITable> m_quickDetails;
	StlLinked<ITable> m_resolveDetails;
	IEntityVector m_outEdges;
	entity_signal_type on_refresh;

public:
	BEGIN_CUNKNOWN
		IMPLEMENT_INTERFACE(IEntity)
	END_CUNKNOWN(clib::CLockableUnknown)

public:
	CEntity(IWebPage * webPage, unsigned int entity_type, const std::_tstring & orig)
	{
		m_webPage = webPage;
		m_orig = orig;
		m_classify_score = 0;
		m_entity_type = (ENTITY_TYPE)entity_type;
		ATLASSERT(m_entity_type >= ENTITY_TYPE_UNKNOWN && m_entity_type < ENTITY_TYPE_LAST);
		m_category = ENTITY_TYPE_STRING[m_entity_type];
		m_id = std::_tstring(m_webPage ? m_webPage->GetID() : _T("")) + _T("/") + m_category + _T("/") + m_orig;
		boost::algorithm::replace_all(m_id, _T("&"), _T("_"));
		boost::algorithm::to_lower(m_id);
		boost::algorithm::trim(m_id);
		m_cleanedInput = _T("");
		m_localEntityNumber = 0;
		m_verified = false;
		m_resolved = false;
	}

	IWebPage * GetWebPage() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_webPage;
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

	const TCHAR * GetOrig() const 
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_orig.c_str();
	}

	int GetClassifyScore() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_classify_score;
	}

	ENTITY_TYPE GetEntityType() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_entity_type;
	}

	const TCHAR * GetCategory() const 
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_category.c_str();
	}

	const TCHAR * GetCleanedInput() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_cleanedInput.c_str();
	}

	unsigned int GetLocalEntityNumber() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_localEntityNumber;
	}

	bool GetVerified() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_verified;
	}

	bool GetResolved() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_resolved;
	}

	void SetIdentities(ITable * identities)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		m_identities = identities;
	}

	ITable * GetIdentities() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_identities;
	}

	void SetServerResults(const std::_tstring & results)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		m_serverResults = results;
	}

	const TCHAR * GetServerResults() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_serverResults.c_str();
	}

	ITable * GetDetails(bool sampleData) 
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		__int64 MaxDids = (int)GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_PREF_MAXDID); 
		__int64 MaxFull = (int)GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_PREF_MAXFULL); 

		if(m_resolved)
		{
			if (!m_resolveDetails)
			{
				CComPtr<IServer> server = AttachAnalysis();
				server->Enhance(m_webPage, this, sampleData ? MaxDids : 1, sampleData ? MaxFull : 1);
			}
			return m_resolveDetails;
		}
		else
		{
			if (!m_quickDetails)
			{
				CComPtr<IServer> server = AttachAnalysis();
				server->Enhance(m_webPage, this, sampleData ? MaxDids : 0, sampleData ? MaxFull : 1);
			}
			return m_quickDetails;
		}
	}

	void SetDetails(ITable * details)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		if(m_resolved)
		{
			m_resolveDetails = details;
		}
		else
		{
			m_quickDetails = details;
		}
	}

	void AppendOutEdge(IEntity * other)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		m_outEdges.push_back(other);
	}

	std::pair<IEntityVector::iterator, IEntityVector::iterator> GetOutEdges()
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return std::make_pair(m_outEdges.begin(), m_outEdges.end());
	}

	void GetAdvancedSearch(CAdvancedSearch & search, bool sampleData)
	{
		CComPtr<IHtmlHelper> htmlHelper = CreateIHtmlHelper(m_category, GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_PREF_SHOWALL));
#ifndef _DEBUG
		try
		{
#endif
			CComPtr<ITable> details = GetDetails(sampleData);
			if (details)
			{
				htmlHelper->AppendTable(details, search);
			}
#ifndef _DEBUG
		}
		catch(...)	//Big hack for demo
		{
			ATLASSERT(false);
		}
#endif
		htmlHelper->Close();
	}

	const TCHAR * GetSummary(std::_tstring & html, bool includeHtmlHeader, bool includeHeader)
	{
		CComPtr<IHtmlHelper> htmlHelper = CreateIHtmlHelper(m_category, GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_PREF_SHOWALL));
		if (includeHtmlHeader)
		{
			htmlHelper->Open(_T("Entity"));
		}
		if (includeHeader)
		{
			htmlHelper->AppendHeading(GetCategory());
		}
#ifndef _DEBUG
		try
		{
#endif
			CComPtr<ITable> details = GetDetails(true);
			if (details)
			{
				std::_tstring ll;
				htmlHelper->AppendTable(details, ll);
				htmlHelper->AppendFooter(ll);
			}
#ifndef _DEBUG
		}
		catch(...)	//Big hack for demo
		{
			ATLASSERT(false);
		}
#endif
		html = htmlHelper->Close(includeHtmlHeader);
		return html.c_str();
	}

	const TCHAR * GetDebug(std::_tstring & html)
	{
		CComPtr<IHtmlHelper> htmlHelper = CreateIHtmlHelper(m_category, GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_PREF_SHOWALL));
#ifndef _DEBUG
		try
		{
#endif
			CComPtr<ITable> details = GetDetails(true);
			if (details)
			{
				htmlHelper->AppendTableDebug(details);
			}
#ifndef _DEBUG
		}
		catch(...)	//Big hack for demo
		{
			ATLASSERT(false);
		}
#endif
		html = htmlHelper->Close();
		return html.c_str();
	}

	void Update(const std::_tstring & cleanedInput, unsigned int localEntityNumber, int classify_score, bool verified, bool resolved)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		m_cleanedInput = cleanedInput;
		m_localEntityNumber = localEntityNumber;
		m_classify_score = classify_score;
		if (!m_verified)
			m_verified = verified;
		if (!m_resolved)
			m_resolved = resolved;
		proc.unlock();
		Refresh();
	}

	void Refresh() 
	{
		on_refresh(this);
	}

	boost::signals::connection on_refresh_connect(const entity_slot_type& s)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return on_refresh.connect(s); 
	}

	void Lock(CComPtr<clib::scoped_lock_ref_counted> & lock) 
	{ 
		return CLockableUnknown::Lock(lock); 
	}
};

static CacheT<std::_tstring, CEntity> EntityCache;

IEntity * CreateIEntity(IWebPage * webPage, unsigned int entity_type, const std::_tstring & text, const std::_tstring & cleanedInput, unsigned int localEntityNumber, int classify_score, bool verified, bool resolved)
{
	CEntity *entity = EntityCache.Get(new CEntity(webPage, entity_type, text));
	entity->Update(cleanedInput, localEntityNumber, classify_score, verified, resolved);
	return entity;
}
IEntity * FindIEntity(IWebPage * webPage, unsigned int entity_type, const std::_tstring & text)
{
	CComPtr<CEntity> entity = new CEntity(webPage, entity_type, text);
	return EntityCache.Exists(entity->GetCacheID());
}
}