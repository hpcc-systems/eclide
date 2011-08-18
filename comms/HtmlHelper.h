#pragma once

#include "table.h"
#include "global.h"
#include <atlhtml.h>

const SectionLabelDefault GLOBAL_PREF_SHOWALL(SectionLabel(_T("SEEP"), _T("ShowAll")), 0);
//  ===========================================================================
class COMMS_API CAdvancedSearch
{
public:
	std::_tstring m_first_name;
	std::_tstring m_mi;
	std::_tstring m_last_name;
	std::_tstring m_ssn;
	std::_tstring m_street_address;
	std::_tstring m_city;
	std::_tstring m_state;
	std::_tstring m_county;
	std::_tstring m_zip;
	std::_tstring m_did;
	std::_tstring m_bdid;
	std::_tstring m_phone;
	std::_tstring m_company_name;
	std::_tstring m_fein;
	std::_tstring m_vin;

	void Merge(CAdvancedSearch & other);

	void FormatLabel(std::_tstring & label, std::_tstring & content);

	const TCHAR * GetAdvancedSearchURL(std::_tstring & url);
	const TCHAR * GetBusinessSearchURL(std::_tstring & url);
	const TCHAR * GetCompReportURL(std::_tstring & url);
	const TCHAR * GetVehicleReportURL(std::_tstring & url);
	const TCHAR * GetRelavintURL(std::_tstring & url);
};
//  ===========================================================================
__interface IHtmlHelper : public IUnknown
{
	void Open(const std::_tstring & title);
	void AppendHeading(const std::_tstring & heading);
	void AppendText(const std::_tstring & text, bool lineBreak = false);
	void AppendLink(const std::_tstring & text, const std::_tstring & url, bool lineBreak = false);
	void AppendHLine();
	bool skipCol(const std::_tstring & colLabel);
	bool AppendTable(CComPtr<ITable> table, CAdvancedSearch & search);
	bool AppendTable(CComPtr<ITable> table, std::_tstring & longestLine);
	bool AppendTableDebug(CComPtr<ITable> table);
	void AppendFooter(const std::_tstring & footer);
	CString Close(bool includeCloseTag=true);
};

COMMS_API IHtmlHelper *	CreateIHtmlHelper(const std::_tstring & category, bool showAll);
//  ===========================================================================
