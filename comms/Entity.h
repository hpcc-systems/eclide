#pragma once

#include "comms.h"
#include "clib.h"
#include "table.h"
#include "htmlhelper.h"

namespace Analysis
{
enum ENTITY_TYPE
{
	ENTITY_TYPE_UNKNOWN = 0,
	ENTITY_TYPE_PERSON, 
	ENTITY_TYPE_COMPANY, 
	ENTITY_TYPE_VIN, 
	ENTITY_TYPE_PHONE, 
	ENTITY_TYPE_EMAIL, 
	ENTITY_TYPE_EID, 
	ENTITY_TYPE_ADDRESS, 
	ENTITY_TYPE_CITY, 
	ENTITY_TYPE_STATE, 
	ENTITY_TYPE_SSN, 
	ENTITY_TYPE_FEIN, 
	ENTITY_TYPE_COUNTRY, 
	ENTITY_TYPE_URL, 
	ENTITY_TYPE_DOCKET, 
	ENTITY_TYPE_DATE, 
	ENTITY_TYPE_STREET, 
	ENTITY_TYPE_FIRSTNAME,
	ENTITY_TYPE_STARSIGN,
	ENTITY_TYPE_AGE,
	ENTITY_TYPE_TICKER,
	ENTITY_TYPE_LAST 
};

__interface IWebPage;
__interface IEntity;

typedef boost::signal<void(IEntity *)> entity_signal_type;
typedef entity_signal_type::slot_type entity_slot_type;

typedef StlLinked<IEntity> IEntityAdapt;
typedef std::vector<IEntityAdapt> IEntityVector;

__interface __declspec(uuid("78da599d-2bba-438c-8f32-014179c3aa2e")) IEntity : public clib::ILockableUnknown
{
	IWebPage * GetWebPage() const;
	const TCHAR * GetID() const;
	int GetClassifyScore() const;
	ENTITY_TYPE GetEntityType() const;
	const TCHAR * GetCategory() const;
	const TCHAR * GetOrig() const;
	const TCHAR * GetCleanedInput() const;
	unsigned int GetLocalEntityNumber() const;
	bool GetVerified() const;
	bool GetResolved() const;

	void SetIdentities(ITable * identities);
	ITable * GetIdentities() const;

	void SetServerResults(const std::_tstring & results);
	const TCHAR * GetServerResults() const;

	ITable * GetDetails(bool sampleData);
	void SetDetails(ITable * details);
	void GetAdvancedSearch(CAdvancedSearch & search, bool sampleData);
	const TCHAR * GetSummary(std::_tstring & html, bool includeHtmlHeader=true, bool includeHeader=true);
	const TCHAR * GetDebug(std::_tstring & html);

	void AppendOutEdge(IEntity * other);
	std::pair<IEntityVector::iterator, IEntityVector::iterator> GetOutEdges();

	boost::signals::connection on_refresh_connect(const entity_slot_type& s);
};

typedef CUnknownT<IEntityVector> IEntityVectorCom;
typedef CComPtr<IEntityVectorCom> IEntityVectorAdapt;

IEntity * CreateIEntity(IWebPage * webPage, unsigned int entity_type, const std::_tstring & text, const std::_tstring & cleanedInput, unsigned int localEntityNumber, int classify_score, bool verified, bool resolved);
IEntity * FindIEntity(IWebPage * webPage, unsigned int entity_type, const std::_tstring & text);
}
