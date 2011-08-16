#pragma once

#include "comms.h"
#include "global.h"
#include "table.h"
#include "StringSet.h"
#include "WebPage.h"
#include "Entity.h"
#include "EntityInstance.h"

//Special password so it will persist in cfg file...
static const SectionLabelDefault GLOBAL_USERLNBAND(SectionLabel(_T("General"), _T("User_LNBand")), _T(""));
static const SectionLabelDefault GLOBAL_PASSWORDLNBAND(SectionLabel(_T("General"), _T("Password_LNBand")), _T(""));
static const SectionLabelDefault GLOBAL_PREF_MAXDID(SectionLabel(_T("SEEP"), _T("MaxDID")), 1);
static const SectionLabelDefault GLOBAL_PREF_MAXFULL(SectionLabel(_T("SEEP"), _T("MaxFull")), 0);

namespace Analysis
{
struct ClassifyStruct
{
	IMarkupPointer * textNode;
	std::wstring text;
};
typedef std::vector<ClassifyStruct> ClassifyStructVector;

__interface IServer;
typedef boost::signal<void(const std::_tstring action, const std::_tstring req, const std::_tstring resp)> logging_signal_type;
typedef logging_signal_type::slot_type logging_slot_type;
__interface IServer : public IUnknown
{
	const TCHAR *GetID() const;
	const TCHAR *GetLabel() const;

	bool Classify(const std::_tstring & text, IEntityVector * entityVector);
	bool Classify(IWebPage * webPage, ClassifyStructVector & request);
	bool Resolve(IWebPage * webPage, const IEntityVector & entityVector);
	bool Enhance(IWebPage * webPage, IEntity * entity, __int64 maxdids, __int64 maxfull);

	//logging connections
	boost::signals::connection on_logging_connect(const logging_slot_type& s);
	void on_logging_disconnect(boost::signals::connection& sc);
};

typedef StlLinked<IServer> IServerAdapt;
typedef std::vector<IServerAdapt> IServerVector;

COMMS_API IServer * AttachAnalysis();
}