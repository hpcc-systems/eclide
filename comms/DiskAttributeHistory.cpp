#include "StdAfx.h"
#include "attributehistory.h"
#include "attribute.h"
#include "module.h"
#include "repository.h"
#include "SoapUtil.h"
#include "clib.h"
#include "cache.h"
#include "logger.h"
#include "UnicodeFile.h"
#include <UtilFilesystem.h>

#if _COMMS_VER < 68200
using namespace WsAttributes;
#elif _COMMS_VER < 700000
#else
using namespace WsRepository;
#endif

class CDiskAttributeHistory : public IAttributeHistory, public clib::CLockableUnknown
{
protected:
    boost::filesystem::path m_path;
    CComPtr<IAttribute> m_attribute;
    int m_version;
    mutable CString m_ecl;
    mutable bool m_eclFetched;
    CString m_description;
    CString m_modifiedBy;
    CString m_modifiedDate;
    CString m_id;

public:
    IMPLEMENT_CLOCKABLEUNKNOWN;

    CDiskAttributeHistory(const boost::filesystem::path & path, IAttribute * attribute, int version) 
        : m_version(version)
    {
        m_attribute = attribute;
        m_path = path;
        m_eclFetched = false;
        UpdateId();
    }

    virtual ~CDiskAttributeHistory()
    {
    }

    const TCHAR *GetID() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_id;
    }

    const TCHAR * GetCacheID() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_id;
    }

    const TCHAR *GetModuleQualifiedLabel(bool excludeRoot = false) const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_attribute->GetModuleQualifiedLabel(excludeRoot);
    }

    const TCHAR *GetLabel() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_attribute->GetLabel();
    }

    const TCHAR *GetQualifiedLabel(bool excludeRoot = false) const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_attribute->GetQualifiedLabel();
    }

    IAttributeType *GetType() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_attribute->GetType();
    }

    const TCHAR *GetText(bool refresh, bool noBroadcast = false) const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        if (!m_eclFetched || refresh)
        {
            CUnicodeFile file;
            file.Open(m_path);
            std::_tstring ecl;
            file.Read(ecl);
            file.Close();
            m_ecl = ecl.c_str();
            m_eclFetched = true;
        }
        return m_ecl;
    }

    const TCHAR *GetModifiedBy() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_modifiedBy;
    }

    const TCHAR * GetModifiedDate() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_modifiedDate;
    }

    const TCHAR *GetDescription() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_description;
    }

    int GetVersion() const	
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_version;
    }

    IAttribute * GetAttribute() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_attribute;
    }

    bool SetLabel(const std::_tstring & label)
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return false;
    }

    void Update()
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        m_description = pathToWString(m_path).c_str();
        m_modifiedDate = CA2T(boost::filesystem::basename(m_path).c_str());
        m_modifiedDate.Replace(_T("_"), _T(":"));
        m_modifiedBy = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_USER);
        Refresh();
    }

    void Refresh()
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        UpdateId();
    }

    void UpdateId()
    {
        m_id = m_path.string().c_str();
    }
};


IAttributeHistory * CreateDiskAttributeHistory(const boost::filesystem::path & path, const IAttribute * attribute, int version)
{
    static CacheT<std::_tstring, CDiskAttributeHistory> AttributeHistoryCache;
    CDiskAttributeHistory * attr = AttributeHistoryCache.Get(new CDiskAttributeHistory(path, const_cast<IAttribute *>(attribute), version));
    ATLASSERT(attr);
    attr->Update();
    return attr;
}
