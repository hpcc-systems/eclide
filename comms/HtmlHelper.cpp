#include "stdafx.h"

#include "comms.h"
#include "HtmlHelper.h"
#include "util.h"

//  ===========================================================================
void CAdvancedSearch::Merge(CAdvancedSearch & other)
{
	if (other.m_first_name.length())
		m_first_name = other.m_first_name;
	if (other.m_mi.length())
		m_mi = other.m_mi;
	if (other.m_last_name.length())
		m_last_name = other.m_last_name;
	if (other.m_ssn.length())
		m_ssn = other.m_ssn;
	if (other.m_phone.length())
		m_phone = other.m_phone;
	if (other.m_street_address.length())
		m_street_address = other.m_street_address;
	if (other.m_city.length())
		m_city = other.m_city;
	if (other.m_state.length())
		m_state = other.m_state;
	if (other.m_county.length())
		m_county = other.m_county;
	if (other.m_zip.length())
		m_zip = other.m_zip;
}

void CAdvancedSearch::FormatLabel(std::_tstring & label, std::_tstring & content)
{
	if (boost::algorithm::iequals(label, _T("orig")))
	{
		label = _T("");
	}
	//else if (boost::algorithm::iequals(label, _T("resolved")))
	//{
	//	label = _T("Resolved");
	//	search.m_first_name = content;
	//}
	else if (boost::algorithm::iequals(label, _T("fname")))
	{
		label = _T("First Name");
		m_first_name = content;
	}
	else if (boost::algorithm::iequals(label, _T("mname")))
	{
		label = _T("Middle Name");
		m_mi = content;
	}
	else if (boost::algorithm::iequals(label, _T("lname")))
	{
		label = _T("Last Name");
		m_last_name = content;
	}
	else if (boost::algorithm::iequals(label, _T("name")))
	{
		label = _T("Name");
		m_company_name = content;
	}
	else if (boost::algorithm::iequals(label, _T("phone_number")))
	{
		label = _T("Phone Number");
		m_phone = content;
	}
	else if (boost::algorithm::iequals(label, _T("did")))
	{
		label = _T("Accurint");
		m_did = content;
	}
	else if (boost::algorithm::iequals(label, _T("bdid")))
	{
		label = _T("Accurint");
		m_bdid = content;
	}
	else if (boost::algorithm::iequals(label, _T("title")))
	{
		label = _T("Title");
	}
	else if (boost::algorithm::iequals(label, _T("street")))
	{
		label = _T("Street");
		m_street_address = content;
	}
	else if (boost::algorithm::iequals(label, _T("city")))
	{
		label = _T("City");
		m_city = content;
	}
	else if (boost::algorithm::iequals(label, _T("county")))
	{
		label = _T("County");
		m_county = content;
	}
	else if (boost::algorithm::iequals(label, _T("state")))
	{
		label = _T("State");
		m_state = content;
	}
	else if (boost::algorithm::iequals(label, _T("zip5")))
	{
		label = _T("Zip");
		m_zip = content;
	}
	else if (boost::algorithm::iequals(label, _T("country")))
	{
		label = _T("Name");
		m_county = content;
	}
	else if (boost::algorithm::iequals(label, _T("model_year")))
	{
		label = _T("Year");
	}
	else if (boost::algorithm::iequals(label, _T("make_description")))
	{
		label = _T("Manufacturer");
	}
	else if (boost::algorithm::iequals(label, _T("model_description")))
	{
		label = _T("Model");
	}
	else if (boost::algorithm::iequals(label, _T("vin")))
	{
		label = _T("Number");
		m_vin = content;
	}
	else if (boost::algorithm::iequals(label, _T("docket_number")))
	{
		label = _T("Number");
	}
	else if (boost::algorithm::iequals(label, _T("docid")))
	{
		label = _T("");
	}
	else if (boost::algorithm::iequals(label, _T("pos")))
	{
		label = _T("");
	}
	else if (boost::algorithm::iequals(label, _T("score")))
	{
		label = _T("");
	}
	else if (boost::algorithm::iequals(label, _T("verified")))
	{
		label = _T("");
	}
	else if (boost::algorithm::iequals(label, _T("resolved")))
	{
		label = _T("");
	}
	else if (boost::algorithm::iequals(label, _T("name_score")))
	{
		label = _T("");
	}
	else if (boost::algorithm::iequals(label, _T("clean")))
	{
		label = _T("");
	}
	else
	{
		label = _T("");
	}
}

const TCHAR * CAdvancedSearch::GetAdvancedSearchURL(std::_tstring & url)
{
	url = _T("");
	if (m_last_name.length())
		url += _T("&LAST_NAME=") + m_last_name;
	if (m_first_name.length())
		url += _T("&FIRST_NAME=") + m_first_name;
	if (m_mi.length())
		url += _T("&MI=") + m_mi;
	if (m_ssn.length())
		url += _T("&SSN=") + m_ssn;
	if (m_street_address.length())
		url += _T("&STREET_ADDRESS=") + m_street_address;
	if (m_city.length())
		url += _T("&CITY=") + m_city;
	if (m_state.length())
		url += _T("&STATE=") + m_state;
	if (m_county.length())
		url += _T("&COUNTY=") + m_county;
	if (m_zip.length())
		url += _T("&ZIP=") + m_zip;
	if (m_phone.length())
		url += _T("&PHONE=") + m_phone;
	if (url.length())
	{
		boost::algorithm::replace_all(url, _T(" "), _T("%20"));
		url = _T("https://securedev.accurint.com/app/bps/misc?EVENT=SEARCH/SEARCH_ADV&SESSION_ID=184a2096a1f4c6d4587f6df09003e490") + url;
	}
	return url.c_str();
}

const TCHAR * CAdvancedSearch::GetBusinessSearchURL(std::_tstring & url)
{
	url = _T("");
	if (m_company_name.length())
		url += _T("&COMPANY_NAME=") + m_company_name;
	if (m_last_name.length())
		url += _T("&LAST_NAME=") + m_last_name;
	if (m_first_name.length())
		url += _T("&FIRST_NAME=") + m_first_name;
	if (m_mi.length())
		url += _T("&MI=") + m_mi;
	if (m_ssn.length())
		url += _T("&SSN=") + m_ssn;
	if (m_street_address.length())
		url += _T("&STREET_ADDRESS=") + m_street_address;
	if (m_city.length())
		url += _T("&CITY=") + m_city;
	if (m_state.length())
		url += _T("&STATE=") + m_state;
	if (m_zip.length())
		url += _T("&ZIP=") + m_zip;
	if (m_fein.length())
		url += _T("&FEIN=") + m_fein;
	if (m_phone.length())
		url += _T("&PHONE=") + m_phone;
	if (url.length())
	{
		boost::algorithm::replace_all(url, _T(" "), _T("%20"));
		url = _T("https://securedev.accurint.com/app/bps/main?EVENT=SEARCH/SEARCH_BUS&SESSION_ID=184a2096a1f4c6d4587f6df09003e490") + url;
	}
	return url.c_str();
}
const TCHAR * CAdvancedSearch::GetCompReportURL(std::_tstring & url)
{
	url = _T("");
	if (m_did.length())
	{
		url = _T("https://securedev.accurint.com/app/bps/report?EVENT=REPORT/COMPREHENSIVE&SESSION_ID=184a2096a1f4c6d4587f6df09003e490&UNIQUEID=") + m_did;
	}
	else if (m_bdid.length())
	{
		url = _T("https://securedev.accurint.com/app/bps/report?EVENT=REPORT/SHORT_BUS_REPORT&SESSION_ID=184a2096a1f4c6d4587f6df09003e490&UNIQUE_ID=") + m_bdid;
	}
	return url.c_str();
}

const TCHAR * CAdvancedSearch::GetVehicleReportURL(std::_tstring & url)
{
	url = _T("");
	if (m_vin.length())
	{
		url = _T("https://securedev.accurint.com/app/bps/main?EVENT=SEARCH/SEARCH_DL_REG&SESSION_ID=184a2096a1f4c6d4587f6df09003e490&VIN=") + m_vin;
	}
	return url.c_str();
}

const TCHAR * CAdvancedSearch::GetRelavintURL(std::_tstring & url)
{
	url = _T("");
	if (m_did.length())
	{
		url = _T("https://securedev.accurint.com/app/bps/report?GRAPHVIEW_LICENSE=1&EVENT=GRAPHVIEW/CHECK_COST&GRAPHVIEW_COST=1&TYPE=PERSON&SESSION_ID=184a2096a1f4c6d4587f6df09003e490&UNIQUEID=") + m_did;
	}
	return url.c_str();
}
//  ===========================================================================
class CHtmlHelper : public IHtmlHelper, public CUnknown
{
protected:
	CWriteStreamOnCString stream;
	HTML_SCHEME scheme;
	CHtmlGen html;
	std::_tstring m_category;
	bool m_showAll;

public:
	BEGIN_CUNKNOWN
	END_CUNKNOWN(CUnknown)

	CHtmlHelper(const std::_tstring & category, bool showAll);
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

protected:
	bool AppendTable(CComPtr<ITable> table, std::_tstring & longestLine, CAdvancedSearch & search, unsigned int depth);
	bool AppendTable(CComPtr<ITable> data, int col, std::_tstring & longestLine, CAdvancedSearch & search, unsigned int depth);
};

CHtmlHelper::CHtmlHelper(const std::_tstring & category, bool showAll) : m_category(category), m_showAll(showAll)
{
	html.Initialize(&stream);
	html.SetScheme(&scheme);
	boost::algorithm::to_lower(m_category);
	m_category[0] -= ('a' - 'A');
}

void CHtmlHelper::Open(const std::_tstring & title)
{
	html.html();
	html.head();
	html.title(title.c_str());
	html.headEnd();
	html.body(0, 0, 0, 0 , AtlHtmlAttrs(_T("style=\"%s\""), _T("font-family: Verdana; font-size: x-small;")));
}

void CHtmlHelper::AppendHeading(const std::_tstring & heading)
{
	html.strong(heading.c_str());
	html.br();
}

void CHtmlHelper::AppendText(const std::_tstring & text, bool lineBreak)
{
	html.WriteRaw(text.c_str());
	if (lineBreak)
	{
		html.br();
	}
}

void CHtmlHelper::AppendLink(const std::_tstring & text, const std::_tstring & url, bool lineBreak)
{
	html.a(url.c_str(), text.c_str(), AtlHtmlAttrs(_T("target=\"_blank\"")));
	if (lineBreak)
	{
		html.br();
	}
}

void CHtmlHelper::AppendHLine()
{
	html.hr();
}

bool CHtmlHelper::skipCol(const std::_tstring & colLabel)
{
	if (!m_showAll && colLabel.length() && colLabel[0] == '_')
	{
		return true;
	}
	return false;
}

enum TABLE_TYPE
{
	TABLE_TYPE_UNKNOWN,
	TABLE_TYPE_BUSINESS,
	TABLE_TYPE_PERSON,
	TABLE_TYPE_VEHICLE,
	TABLE_TYPE_ADDRESS,
	TABLE_TYPE_OTHER,
	TABLE_TYPE_LAST
};

TABLE_TYPE FormatTableLabel(const std::_tstring & label, std::_tstring & result, const std::_tstring & category)
{
	if (boost::algorithm::iequals(label, _T("people")))
	{
		result = _T("Person:");
		return TABLE_TYPE_PERSON;
	}
	else if (boost::algorithm::iequals(label, _T("clean")))
	{
		result = category + _T(" (Cleaned):");
		return TABLE_TYPE_OTHER;
	}
	else if (boost::algorithm::iequals(label, _T("businesses")))
	{
		result = _T("Business:");
		return TABLE_TYPE_BUSINESS;
	}
	else if (boost::algorithm::iequals(label, _T("addresses")))
	{
		result = _T("Address:");
		return TABLE_TYPE_ADDRESS;
	}
	else
	{
#ifdef _DEBUG
		result = label;
#endif
	}
	return TABLE_TYPE_OTHER;
}

bool CHtmlHelper::AppendTable(CComPtr<ITable> table, CAdvancedSearch & search)
{
	std::_tstring longestLine = m_category + _T("    [close]");
	return AppendTable(table, longestLine, search, 0);
}

bool CHtmlHelper::AppendTable(CComPtr<ITable> table, std::_tstring & longestLine)
{
	CAdvancedSearch search;
	longestLine = m_category + _T("    [close]");
	return AppendTable(table, longestLine, search, 0);
}

bool CHtmlHelper::AppendTable(CComPtr<ITable> table, std::_tstring & longestLine, CAdvancedSearch & search, unsigned int depth)
{
	int found = 0;
	std::_tstring orig = table->GetCell(0, _T("orig"));
	//bool resolved = boost::algorithm::iequals(table->GetCell(0, _T("resolved")), _T("true"));
	if (orig.length())
	{
		html.span(0, AtlHtmlAttrs(_T("style=\"font-family: Verdana, Helvetica; font-size: 10px; font-weight: normal\"")));//, AtlHtmlAttrs(_T("style=\"%s\""), _T("color: #f1f1ff")));
		html.i(orig.c_str());
		if (orig.length() > longestLine.length())
			longestLine = orig;
		html.spanEnd();
	}
	html.table(0, AtlHtmlAttrs(_T("style=\"font-family: Verdana, Helvetica; font-size: 10px\" cellspacing=\"1\" cellpadding=\"1\"")));
	std::_tstring ll;
	for(unsigned int tableCol = 0; tableCol < table->GetColumnCount(); ++tableCol)
	{
		std::_tstring line;
		found += AppendTable(table, tableCol, line, search, depth);
		if (line.length() > longestLine.length())
			longestLine = line;
	}
	html.tableEnd();
	if (found > 0)
	{
		return true;
	}
	return false;
}

bool CHtmlHelper::AppendTable(CComPtr<ITable> data, int col, std::_tstring & longestLine, CAdvancedSearch & _search, unsigned int depth)
{
	if (CComPtr<ITable> childdata = data->GetChildDataset(0, col))
	{
		std::_tstring label;
		TABLE_TYPE table_type = FormatTableLabel(data->GetColumn(col), label, m_category);
		if (label.empty())
			return false;

		CHtmlHelper childHelper(m_category, true);
		CAdvancedSearch search;
		if (childHelper.AppendTable(childdata, longestLine, search, depth + 1))
		{
			html.tr();
			html.td(0, AtlHtmlAttrs(_T("valign=\"top\"")));
			std::_tstring url;
			switch(table_type)
			{
			case TABLE_TYPE_ADDRESS:
				_search.Merge(search);
				break;
			case TABLE_TYPE_BUSINESS:
				search.GetBusinessSearchURL(url);
				break;
			case TABLE_TYPE_VEHICLE:
				search.GetVehicleReportURL(url);
				break;
			}
			if (url.empty())
				search.GetAdvancedSearchURL(url);
			if (url.empty())
				search.GetBusinessSearchURL(url);
			if (url.empty())
				search.GetVehicleReportURL(url);

			if (url.length())
			{
				html.b();
				AppendLink(label, url); 	
				html.bEnd();
			}
			else
				html.b(label.c_str()); 	
			html.tdEnd();
			html.td((const TCHAR *)childHelper.Close(false));	
			html.trEnd();
			longestLine = label + _T("    ") + longestLine;
			return true;
		}
	}
	else
	{
		std::_tstring label = data->GetColumn(col);
		std::_tstring content = data->GetCell(0, col);
		_search.FormatLabel(label, content);
		if (label.empty())
			return false;

		html.tr();
		html.td();		html.b(label.c_str()); html.tdEnd();	
		html.td();		
		std::_tstring url;
		_search.GetCompReportURL(url);
		std::_tstring rurl;
		_search.GetRelavintURL(rurl);
		if (url.length() || rurl.length())
		{
			if (url.length())
			{
				html.b();
				AppendLink(_T("Report"), url); 	
				html.bEnd();
				if (rurl.length())
					AppendText(_T(" - "));
			}
			if (rurl.length())
			{
				html.b();
				AppendLink(_T("Relavint"), rurl); 	
				html.bEnd();
			}
			_search.m_did.empty();
			_search.m_bdid.empty();
		}
		else
			AppendText(data->GetCell(0, col));
		html.tdEnd();	
		html.trEnd();
		longestLine = label + _T("  ") + content;
		return true;
	}
	return false;
}

bool CHtmlHelper::AppendTableDebug(CComPtr<ITable> table)
{
	if (!table)
		return false;

	//  Header
	html.table(0, AtlHtmlAttrs(_T("style=\"%s\" bordercolor=\"%s\" cellspacing=\"%s\" cellpadding=\"%s\" border=\"%s\""), _T("font-family: Verdana; font-size: x-small"), _T("tan"), _T("0"), _T("1"), _T("1")));
	html.tr(AtlHtmlAttrs(_T("bgcolor=\"%s\""), _T("tan")));
	for(unsigned int col = 0; col < table->GetColumnCount(); ++col)
	{
		if (skipCol(table->GetColumn(col)))
		{
			continue;
		}
		html.td();
		html.strong(table->GetColumn(col));
		html.tdEnd();
	}
	html.trEnd();

	//Content
	for (int row = 0; row < table->GetRowCount(); ++row)
	{
		html.tr();
		for(unsigned int col = 0; col < table->GetColumnCount(); ++col)
		{
			if (skipCol(table->GetColumn(col)))
			{
				continue;
			}
			html.td();
			if (!AppendTableDebug(table->GetChildDataset(row, col)))
			{
				std::_tstring _cell = table->GetCell(row, col);
				std::_tstring cell;
				html.WriteRaw(EscapeXML(_cell, cell));
			}
			html.tdEnd();
		}
		html.trEnd();
	}
	html.tableEnd();
	return true;
}

void CHtmlHelper::AppendFooter(const std::_tstring & footer)
{
	html.span(0, AtlHtmlAttrs(_T("style=\"font-family: Verdana, Helvetica; font-size: 10px; font-weight: normal; color: #f1f1ff\"")));
	std::_tstring tmp = footer;
	for (unsigned int i = 0; i < tmp.length(); ++i)
	{
		if ((tmp[i] >= 'a' && tmp[i] <= 'z') || (tmp[i] >= 'A' && tmp[i] <= 'Z') || (tmp[i] >= '0' && tmp[i] <= '9'))
		{
			//Goodness
		}
		else
			tmp[i] = _T('_');

	}
	html.WriteRaw(_T("["));
	html.WriteRaw(tmp.c_str());
	html.WriteRaw(_T("]"));
	html.spanEnd();
}

CString CHtmlHelper::Close(bool includeCloseTag)
{
	if (includeCloseTag)
	{
		html.bodyEnd();
		html.htmlEnd();
	}
	return static_cast<const char *>(stream.m_str);
}
//  ===========================================================================
COMMS_API IHtmlHelper * CreateIHtmlHelper(const std::_tstring & category, bool showAll)
{
	return new CHtmlHelper(category, showAll);
}

