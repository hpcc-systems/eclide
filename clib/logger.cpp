#include "stdafx.h"

#include "logger.h"
#include "SoapUtil.h"

const TCHAR * const LEVEL_TEXT[] =
{
	_T("FINEST:   "),
	_T("FINER:    "),
	_T("FINE:     "),
	_T("CONFIG:   "),
	_T("INFO:     "),
	_T("WARNING:  "),
	_T("SEVERE:   ")
};

Logger::Logger(const char * filePath, int line) : m_line(line)
{
	boost::filesystem::path path(filePath, boost::filesystem::native);
	m_file = path.leaf().string();
}

void Logger::operator()(LEVEL l)
{
#if _DEBUG
	std::_tclog << std::_tstring(LEVEL_TEXT[l]) << std::endl;
#endif
	if (l >= LEVEL_INFO)
		std::_tcerr << std::_tstring(LEVEL_TEXT[l]) << std::endl;
}

void Logger::operator()(LEVEL l, /*ATL::SOAPCLIENT_ERROR*/ int err)
{
	(*this)(l, boost::_tformat(_T("Soap Client Error:  %1% - %2%")) % err % GetSoapError(err));
}

/*
void Logger::operator()(LEVEL l, const boost::_tformat& fmt)
{
	std::string msg = fmt.str();
	msg.resize(128, ' ');
#if _DEBUG
	std::_tclog << std::_tstring(LEVEL_TEXT[l]) << msg << std::_tstring(CA2W(m_file.c_str())) << _T("(") << m_line << _T(")") << std::endl;
#endif
	if (l >= LEVEL_WARNING)
		std::_tcerr << std::_tstring(LEVEL_TEXT[l]) << msg << std::_tstring(CA2W(m_file.c_str())) << _T("(") << m_line << _T(")") << std::endl;
}
*/

void Logger::operator()(LEVEL l, const boost::_tformat& fmt)
{
	std::_tstring msg = fmt.str();
	msg.resize(512, _T(' '));
#if _DEBUG
	std::_tclog << std::_tstring(LEVEL_TEXT[l]) << msg << std::_tstring(CA2T(m_file.c_str())) << _T("(") << m_line << _T(")") << std::endl;
#endif
	if (l >= LEVEL_INFO)
		std::_tcerr << std::_tstring(LEVEL_TEXT[l]) << msg << std::_tstring(CA2T(m_file.c_str())) << _T("(") << m_line << _T(")") << std::endl;
}

void Logger::operator()(LEVEL l, const CComBSTR& str)
{
	(*this)(l, boost::_tformat(_T("%1%")) % std::_tstring(CW2T(str)));
}

void Logger::operator()(const std::_tstring & url, LEVEL l, const CComBSTR& str)
{
	if (url.empty())
		return;
	this->operator()(l, str);
}

void Logger::operator()(const CString & url, LEVEL l, const CComBSTR& str)
{
	if (url.IsEmpty())
		return;
	this->operator()(l, str);
}
