#pragma once

#include "comms.h"
#include "recursivemutex.h"
#include "Entity.h"
#include "EntityInstance.h"

namespace Analysis
{
__interface IWebPage;

enum EVENT
{
	EVENT_UNKNOWN = 0,
	EVENT_CLASSIFIED,
	EVENT_RESOLVED,
	EVENT_LAST
};

enum STATE
{
	STATE_UNKNOWN = 0,
	STATE_CLASSIFIYING,
	STATE_CLASSIFIED,
	STATE_RESOLVING,
	STATE_RESOLVED,
	STATE_LAST
};

typedef boost::signal<void(IWebPage *, EVENT)> webpage_signal_type;
typedef webpage_signal_type::slot_type webpage_slot_type;

typedef StlLinked<IWebPage> IWebPageAdapt;
typedef std::vector<IWebPageAdapt> IWebPageVector;

__interface __declspec(uuid("86CED41D-8A5F-4dc0-8812-C7EEAEC708A4")) IWebPage : public clib::ILockableUnknown
{
	const TCHAR * GetID() const;

	STATE GetState() const;
	int GetClassifyJobCount() const;

	void StartClassify(const CComBSTR & scriptText);

	void AppendEntity(IEntity * entity);
	void AppendEntityInstance(IEntityInstance * entity);

	void GetEntities(IEntityVector * entities) const;
	void GetEntityInstances(IEntityInstanceVector * instances) const;

	void StartResolve();
	void SetResolveResult(const std::_tstring & resolveResult);
	const TCHAR * GetResolveResults() const;

	void SetStatsHTML(const std::_tstring & statsHtml);
	const TCHAR * GetStatsHTML();

	const TCHAR * GetHtml(std::_tstring & html);

	void SetPopupTemplate(const std::_tstring & tpl);
	const TCHAR *GetPopupTemplate();

	boost::signals::connection on_refresh_connect(const webpage_slot_type& s);
};

typedef CUnknownT<IWebPageVector> IWebPageVectorCom;
typedef CComPtr<IWebPageVectorCom> IWebPageVectorAdapt;

COMMS_API IWebPage * CreateIWebPage(IHTMLDocument * doc);
IWebPage * FindIWebPage(IHTMLDocument * doc);
COMMS_API void ClearIWebPageCache();
}
