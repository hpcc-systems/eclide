#include "StdAfx.h"
#include "SMCVersion.h"

#include "smc.h"
#include "SoapUtil.h"
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
    ParsedVersion m_parsedVersion;
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
        static bool unitTest = false;
        if (!unitTest)
        {
            unitTest = true;
            ParsedVersion test;
            ATLASSERT(ParseVersion(_T("4.3.2.1"), test));
            ATLASSERT(test.type == ParsedVersion::ECLIDE);
            ATLASSERT(ParseVersion(_T("community_4.0.0-9rc"), test));
            ATLASSERT(test.type == ParsedVersion::OSS);
            ATLASSERT(ParseVersion(_T("community_4.0.0-rc9"), test));
            ATLASSERT(test.type == ParsedVersion::OSS);
            ATLASSERT(ParseVersion(_T("community_4.0.0-rc9\r\n"), test));
            ATLASSERT(test.type == ParsedVersion::OSS);
            ATLASSERT(ParseVersion(_T("community_4.0.0-rc9[abc]"), test));
            ATLASSERT(test.type == ParsedVersion::OSS);
            ATLASSERT(ParseVersion(_T("community_4.0.0-rc9[abc+def]"), test));
            ATLASSERT(test.type == ParsedVersion::OSS);
            ATLASSERT(ParseVersion(_T("community_4.0.0-rc9[abc:def]"), test));
            ATLASSERT(test.type == ParsedVersion::OSS);
            ATLASSERT(ParseVersion(_T("community_4.0.0-rc9[abc_def]"), test));
            ATLASSERT(test.type == ParsedVersion::OSS);
            ATLASSERT(ParseVersion(_T("community_4.0.0-rc9[community_4.0.0-rc9-4-g826364-dirty]"), test));
            ATLASSERT(test.type == ParsedVersion::OSS);
            ATLASSERT(ParseVersion(_T("build_3_08"), test));
            ATLASSERT(test.type == ParsedVersion::BUILD);
            ATLASSERT(ParseVersion(_T("internal_3.10.8-8"), test));
            ATLASSERT(test.type == ParsedVersion::OSS);
            ATLASSERT(ParseVersion(_T("community_5.6.7-rc8"), test));
            ATLASSERT(test.type == ParsedVersion::OSS);
            ATLASSERT(ParseVersion(_T("4.3.2 community_5.6.7-rc8"), test));
            ATLASSERT(test.type == ParsedVersion::ECLCC);
            ATLASSERT(!ParseVersion(_T("4.3.2-abc"), test));
            ATLASSERT(!ParseVersion(_T("4.3.2"), test));
            ATLASSERT(!ParseVersion(_T("4.3"), test));
            ATLASSERT(!ParseVersion(_T("4"), test));
        }
#endif
        ParseVersion((const TCHAR *)version, m_parsedVersion);
        m_strversion = version;
        boost::algorithm::trim(m_strversion);
    }

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

    const TCHAR * GetPrefix() const
    {
        return m_parsedVersion.preffix.c_str();
    }

    unsigned int GetMajor() const
    {
        return m_parsedVersion.majorVersion;
    }

    unsigned int GetMinor() const
    {
        return m_parsedVersion.minorVersion;
    }

    unsigned int GetPoint() const
    {
        return m_parsedVersion.pointVersion;
    }

    const TCHAR * GetSuffixString() const
    {
        return m_parsedVersion.suffixStr.c_str();
    }

    unsigned int GetSuffixInteger() const
    {
        return m_parsedVersion.suffixInt;
    }

    int Compare(const IVersion * other) const
    {
        //  Ignore prefix  ---
        const CVersion * otherBase = dynamic_cast<const CVersion *>(other);
        if (otherBase)
        {
            if (m_parsedVersion == otherBase->m_parsedVersion)
            {
                int retVal = m_parsedVersion.suffixStr.compare(otherBase->m_parsedVersion.suffixStr);
                if (retVal)
                    return retVal;
                if (m_parsedVersion.suffixInt == otherBase->m_parsedVersion.suffixInt)
                    return 0;
                if (m_parsedVersion.suffixInt < otherBase->m_parsedVersion.suffixInt)
                    return -1;
                return 1;
            }
            if (m_parsedVersion < otherBase->m_parsedVersion)
                return -1;
            return 1;
        }
        assert(false);
        return -1;
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

