#include "StdAfx.h"
#include "attributehistory.h"
#include "attribute.h"
#include "module.h"
#include "repository.h"
#include "SoapUtil.h"
#include "clib.h"
#include "cache.h"
#include "logger.h"

#if _COMMS_VER < 68200
using namespace WsAttributes;
#elif _COMMS_VER < 700000
#else
using namespace WsRepository;
#endif

class CAttributeHistory : public IAttributeHistory, public clib::CLockableUnknown
{
protected:
	CComPtr<IRepository> m_repository;
	CString m_module;
	CString m_label;
	IAttributeType * m_type;
	CString m_qualifiedLabel;
	int m_version;
	mutable CString m_ecl;
	mutable bool m_eclFetched;
	CString m_description;
	CString m_modifiedBy;
	CString m_modifiedDate;
	CString m_id;

public:
	IMPLEMENT_CLOCKABLEUNKNOWN;

	CAttributeHistory(const IRepository* rep, const TCHAR* module, const TCHAR* label, IAttributeType * type, unsigned version) 
		: m_module(module), m_label(label), m_type(type), m_version(version)
	{
		m_repository = const_cast<IRepository *>(rep);
		m_eclFetched = false;
		UpdateId();
	}

	virtual ~CAttributeHistory()
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
		ATLASSERT(!excludeRoot);
		return m_module;
	}

	const TCHAR *GetLabel() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_label;
	}

	const TCHAR *GetQualifiedLabel(bool excludeRoot = false) const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_qualifiedLabel;
	}

	IAttributeType *GetType() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_type;
	}

	const TCHAR *GetText(bool refresh, bool noBroadcast = false) const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		if (!m_eclFetched || refresh)
		{
			if (m_repository->GetAttributeText(m_module, m_label, m_type, m_version, false, m_ecl))
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
		return m_repository->GetAttribute(m_module, m_label, m_type);
	}

	bool SetLabel(const std::_tstring & label)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		std::_tstring error;
		return m_repository->UpdateLabel(label, static_cast<const TCHAR *>(m_module), static_cast<const TCHAR *>(m_label), m_type, m_version, error);
	}

#if _COMMS_VER < 68200
	void Update(const ECLAttribute * c)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		m_module = CW2T(c->ModuleName, CP_UTF8);
		m_label = CW2T(c->Name, CP_UTF8);
		m_qualifiedLabel = m_module + _T(".") + m_label;
		m_version = c->Version;
		if (SysStringLen(c->Text))
			m_ecl = CW2T(c->Text, CP_UTF8);
		m_description = CW2T(c->Description, CP_UTF8);
#if _COMMS_VER < 700000
		m_modifiedDate = CW2T(c->ModifiedDate, CP_UTF8);
#else
		ATLASSERT(!_T("ToDo"));
		//m_modifiedDate = CW2T(c->ModifiedDate, CP_UTF8);
#endif
		m_modifiedBy = CW2T(c->ModifiedBy, CP_UTF8);
		Refresh();
	}
#else
	void Update(const ns2__ECLAttribute * c)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		SAFE_ASSIGN2CSTRING(m_module, c->ModuleName);
		SAFE_ASSIGN2CSTRING(m_label, c->Name);
		m_qualifiedLabel = m_module + _T(".") + m_label;
		SAFE_ASSIGN(m_version, c->Version);
		SAFE_ASSIGN2CSTRING(m_ecl, c->Text);
		SAFE_ASSIGN2CSTRING(m_description, c->Description);
		SAFE_ASSIGN2CSTRING(m_description, c->Description);
		SAFE_ASSIGN2CSTRING(m_modifiedDate, c->ModifiedDate);
		SAFE_ASSIGN2CSTRING(m_modifiedBy, c->ModifiedBy);
		Refresh();
	}
#endif

	void Refresh()
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		UpdateId();
	}

	void UpdateId()
	{
		m_id = m_repository->GetID();
		m_id = m_id + _T("/") + m_module + _T("/") + m_label + _T("/") + m_type->GetRepositoryCode() + _T("/") + boost::lexical_cast<std::_tstring>(m_version).c_str();
	}
};

CAttributeHistory * CreateAttributeHistoryRaw(const IRepository* rep, const TCHAR* module, const TCHAR* label, IAttributeType * type, unsigned version, bool /*sandboxed*/)
{
	static CacheT<std::_tstring, CAttributeHistory> AttributeHistoryCache;
	return AttributeHistoryCache.Get(new CAttributeHistory(rep, module, label, type, version));
}

#if _COMMS_VER < 68200
IAttributeHistory * CreateAttributeHistory(const IRepository* rep, const std::_tstring &moduleLabel, const std::_tstring &label, IAttributeType * type, const ECLAttribute * data)
{
	CAttributeHistory * attr = CreateAttributeHistoryRaw(rep, moduleLabel.c_str(), label.c_str(), type, data->Version, data->IsSandbox);
	ATLASSERT(attr);
	attr->Update(data);
	return attr;
}
#else
IAttributeHistory * CreateAttributeHistory(const IRepository *rep, IAttributeType * type, const ns2__ECLAttribute * data)
{
	std::_tstring moduleName;
	SAFE_ASSIGN(moduleName, data->ModuleName);
	if (moduleName.empty())
		return NULL;
	std::_tstring name;
	SAFE_ASSIGN(name, data->Name);
	if (name.empty())
		return NULL;

	unsigned int version = 0;
	SAFE_ASSIGN(version, data->Version);
	bool isSandbox = false;
	SAFE_ASSIGN(isSandbox, data->IsSandbox);
	CAttributeHistory * attr = CreateAttributeHistoryRaw(rep, moduleName.c_str(), name.c_str(), type, version, isSandbox);
	ATLASSERT(attr);
	attr->Update(data);
	return attr;
}
#endif

