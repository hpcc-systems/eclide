#include "StdAfx.h"
#include "SMCVersion.h"

#include "smc.h"
#include "gSoapUtil.h"
#include "Cache.h"
#include <VersionParser.h>	//EclLib

#if _COMMS_VER < 68200
using namespace WsSMC;
#else
#endif

namespace SMC
{
class CVersion : public IVersion, public CUnknown
{
protected:
	std::_tstring m_id;
	std::_tstring m_url;
	std::_tstring m_rawversion;
	std::_tstring m_strversion;
	int m_CommsVer;	// Compatable with _COMMS_VER
	mutable clib::recursive_mutex m_mutex;

public:
	BEGIN_CUNKNOWN
	END_CUNKNOWN(CUnknown)

	//  Format One:	build_0689_44, build_0702_13_gentoo64_linux
	//  Format Two: 
	CVersion(const CString & url, const CString &version) : m_url(url), m_rawversion(version)
	{
		m_id = m_url + _T("/") + m_rawversion.c_str();
#ifdef _DEBUG
		//  <string>-|_<int>.<int>[<char>][.<int>].|-|_<string>[_<int>[<char>]]
		ParsedVersion test1, test2, test3, test4, test5, test6, test7;
		ATLASSERT(ParseVersion(_T("hpcc_100.10a.22_betaX_22a"), test1));
		ATLASSERT(ParseVersion(_T("hpcc-100.10a.22-betaX_22a"), test2));
		ATLASSERT(ParseVersion(_T("hpcc_100.10_betaX"), test3));
		ATLASSERT(ParseVersion(_T("hpcc-100.10-betaX"), test4));
		ATLASSERT(ParseVersion(_T("hpcc-100.10"), test5));
		ATLASSERT(ParseVersion(_T("hpcc_3.0_beta4_63910"), test6));
		//ATLASSERT(ParseVersion(_T("hpcc_100.10.1_betaX"), test2));
		//ATLASSERT(ParseVersion(_T("build_200"), test3));
		//ATLASSERT(ParseVersion(_T("build_200_2"), test4));
		//ATLASSERT(ParseVersion(_T("build_200_2_gentoo_linux"), test5));
		//ATLASSERT(ParseVersion(_T("build_200_gentoo_linux"), test6));
		//ATLASSERT(ParseVersion(_T("build_0702_15a_gentoo64_linux"), test7));
#endif
		ParsedVersion parsedVersion;
		bool parsed = ParseVersion((const TCHAR *)version, parsedVersion);
		//ATLASSERT(parsed);

		//m_strversion = parsedVersion.projectName;
		if (parsedVersion.majorVersion || parsedVersion.minorVersion || parsedVersion.pointVersion)
		{
			//if (!m_strversion.empty())
				//m_strversion += _T(" ");
			m_strversion += boost::lexical_cast<std::_tstring>(parsedVersion.majorVersion);
		}
		if (parsedVersion.minorVersion || parsedVersion.pointVersion)
		{
			m_strversion += _T(".");
			m_strversion += boost::lexical_cast<std::_tstring>(parsedVersion.minorVersion);
			m_strversion += parsedVersion.minorVersionAlpha;
		}
		if (parsedVersion.pointVersion)
		{
			m_strversion += _T(".");
			m_strversion += boost::lexical_cast<std::_tstring>(parsedVersion.pointVersion);
		}
		if (!parsedVersion.suffix.empty())
		{
			m_strversion += _T(" (");
			m_strversion += parsedVersion.suffix;
			m_strversion += _T(")");
		}
		if (parsedVersion.ossFlag)
			m_CommsVer = -1;
		else
			m_CommsVer = parsedVersion.majorVersion * 100 + parsedVersion.minorVersion;

		/*
		m_CommsVer = 0;
		m_id = m_url + _T("/") + m_rawversion.c_str();
		boost::char_separator<TCHAR> sep(_T("_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"));
		boost::_ttokenizer tokens(m_rawversion, sep);
		boost::_ttokenizer::iterator tok_iter = tokens.begin(); 
		std::_tstring commsVer;
		if (tok_iter != tokens.end())
		{
			m_strversion = *tok_iter;
			boost::algorithm::trim_left_if(m_strversion, boost::algorithm::is_any_of("0")); 
			commsVer = m_strversion;
			++tok_iter;
		}
		if (tok_iter != tokens.end())
		{
			m_strversion += _T(".");
			m_strversion += *tok_iter;
			commsVer += *tok_iter;
			++tok_iter;
		}
		else	//  No suffix equates to 00
		{
			m_strversion += _T(".00");
			commsVer += _T("00");
		}
		try
		{
			m_CommsVer = boost::lexical_cast<int>(commsVer);
		}
		catch(boost::bad_lexical_cast &)
		{
		}
		*/
	}

	//void Lock(CComPtr<clib::scoped_lock_ref_counted> & lock)
	//{
	//	lock = new clib::scoped_lock_ref_counted(m_mutex);
	//}

	const TCHAR * GetID()
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_id.c_str();
	}

	const TCHAR * GetCacheID()
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_id.c_str();
	}

	float Get()
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return static_cast<float>(_tstof(m_strversion.c_str()));
	}

	const TCHAR * GetString()
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_strversion.c_str();
	}

	bool IsCompatable()
	{
#ifdef CHECK_FOR_OLD_SERVER
		typedef std::vector<int> VersionHistory;
		VersionHistory versions;
		//  Push breaking versions into vector;
		versions.push_back(471);
		versions.push_back(491);
		versions.push_back(499);
		versions.push_back(505);
		versions.push_back(50509);
		versions.push_back(50514);
		versions.push_back(50515);
		versions.push_back(50520);
		versions.push_back(50523);
		versions.push_back(51100);
		versions.push_back(51300);
		versions.push_back(51400);
		versions.push_back(52501);
		versions.push_back(53300);
		versions.push_back(53306);
		versions.push_back(58300);
		versions.push_back(65101);
		int serverPos = 0;
		int clientPos = 0;
		for(VersionHistory::iterator itr = versions.begin(); itr != versions.end(); ++itr)
		{
			if (m_CommsVer < *itr)
			{
				++serverPos;
			}
			if (_COMMS_VER < *itr)
			{
				++clientPos;
			}
		}
		if (clientPos < serverPos)	//The bigger the pos the older it is.
		{
			return false;
		}
#endif
		return true;
	}

	void Update()
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
	}
};

CacheT<std::_tstring, CVersion> VersionCache;

IVersion * CreateVersion(const CString & url, const CString & version)
{
	//return new CVersion(url, version);
	CVersion * retVal = VersionCache.Get(new CVersion(url, version));
	retVal->Update();
	return retVal;
}
void ClearVersionCache()
{
	VersionCache.clear();
}
}

COMMS_API SMC::IVersion * GetCommsVersion()
{
	return SMC::CreateVersion(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_SMC), _COMMS_VER_STR);
}

