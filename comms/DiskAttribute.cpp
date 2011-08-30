#include "StdAfx.h"
#include "DiskAttribute.h"
#include "module.h"
#include "DiskRepository.h"
#include "SoapUtil.h"
#include "clib.h"
#include "cache.h"
#include "logger.h"
#include "util.h"
#include "UnicodeFile.h"
#include "md5.hpp"
#include "ModuleHelper.h"

#if _COMMS_VER < 68200
using namespace WsAttributes;
#else
#endif

class CDiskAttribute : public IDiskAttribute, public IAttributeHistory, public clib::CLockableUnknown
{
protected:
	bool m_placeholder;
	IRepository * m_repository;
	CString m_id;
	CString m_url;
	mutable CComPtr<IModule> m_module;
	boost::filesystem::wpath m_path;
	CString m_moduleLabel;
	CString m_label;
	CString m_qualifiedLabel;
	CString m_qualifiedLabelNoRoot;
	CComPtr<IAttributeType> m_type;
	CString m_ecl;
	CString m_description;
	CString m_checksum;
	CString m_checksumLocal;
	CString m_checksumLocalTidied;
	bool m_checkedOut;
	bool m_sandboxed;
	bool m_locked;
	bool m_orphaned;
	unsigned m_version;
	CString m_lockedby;
	CString m_modifiedDate;
	CString m_modifiedBy;
	bool m_eclSet;

	refresh_signal_type on_refresh;

public:
	BEGIN_CLOCKABLEUNKNOWN
		IMPLEMENT_INTERFACE(IDiskAttribute)
		IMPLEMENT_INTERFACE(IAttribute)
		IMPLEMENT_INTERFACE(IAttributeHistory)
	END_CUNKNOWN(clib::CLockableUnknown)

	CDiskAttribute(const IRepository *rep, const TCHAR* module, const TCHAR* label, const TCHAR* type, const boost::filesystem::wpath & path, unsigned version, bool sandboxed, bool placeholder) 
		: m_moduleLabel(module), m_label(label), m_type(CreateIAttributeType(type)), m_version(version), m_sandboxed(sandboxed), m_placeholder(placeholder)
	{
		ATLASSERT(!boost::algorithm::contains(module, _T("\\")));
		ATLASSERT(!boost::algorithm::contains(module, _T("/")));
		ATLASSERT(boost::algorithm::contains(module, _T(".")));	
		m_repository = const_cast<IRepository *>(rep);
		m_path = path;
		m_url = path.native_file_string().c_str();
		m_checkedOut = false;
		m_locked = false;
		m_orphaned = false;
		m_eclSet = false;
		UpdateID();
	}

	virtual ~CDiskAttribute()
	{
	}

	void UpdateID()
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		m_id = m_repository->GetID();
		m_id = m_id + _T("/") + m_moduleLabel + _T("/") + m_label + _T("/") + m_type->GetRepositoryCode() + (m_placeholder ? _T("/placeholder") : _T(""));
		m_id.MakeLower();
	}

	IRepository* GetRepository()
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_repository;
	}
	const TCHAR *GetChecksum() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_checksum.IsEmpty() ? m_checksumLocal : m_checksum;
	}
	const TCHAR *GetChecksumLocal() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_checksumLocal;
	}
	const TCHAR *GetChecksumLocalTidied() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_checksumLocalTidied;
	}
	void UpdateChecksumLocal()
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		std::_tstring tmp = m_ecl;

		std::string checksum;
		std::string premd5 = CT2A(tmp.c_str());
		md5_string(premd5, checksum);
		m_checksumLocal = checksum.c_str();
		UpdateChecksumLocalTidied();
	}
	void UpdateChecksumLocalTidied()
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		std::_tstring tmp = m_ecl;
		std::_tstring search = _T("\r\n");
		std::_tstring replace = _T("\n");

		for(std::string::size_type idx = tmp.find(search); idx != std::string::npos; idx = tmp.find(search, idx + 1))
			tmp.replace(idx, search.length(), replace);
		std::string checksum;
		std::string premd5 = CT2A(tmp.c_str());
		md5_string(premd5, checksum);
		m_checksumLocalTidied = checksum.c_str();
	}
	const TCHAR *GetURL() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_url;
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
	IModule *GetModule() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		if (!m_module)
			m_module = m_repository->GetModule(m_moduleLabel);
		return m_module;
	}
	const TCHAR *GetModuleLabel() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_moduleLabel;
	}

	const TCHAR *GetLabel() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_label;
	}

	const TCHAR *GetQualifiedLabel(bool excludeRoot = false) const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		if (excludeRoot)
			return m_qualifiedLabelNoRoot;
		return m_qualifiedLabel;
	}

	const SecAccessFlags GetAccess() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return SecAccess_Full;
	}

	IAttributeType *GetType() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_type;
	}

	const TCHAR *GetText(bool refresh, bool noBroadcast = false) const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		if (refresh || !m_eclSet)
		{
			proc.unlock();
			StlLinked<IAttribute> attr = m_repository->GetAttribute(GetModuleLabel(), GetLabel(), GetType(), 0, true, true, noBroadcast);
			proc.lock();
		}
		return m_ecl;
	}

	int GetVersion() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_version;
	}

	bool SetText(const TCHAR* ecl, bool noBroadcast = false) 
	{
		bool result = false;
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		ATLASSERT(ecl);

		CUnicodeFile file;
		file.Create(m_path.native_file_string().c_str(), GENERIC_WRITE, CREATE_ALWAYS, CUnicodeFile::ENCODING_ANSI);
		if(file.IsOpen())
		{
			m_eclSet = true;
			m_ecl = ecl;
			file.Write(m_ecl);
			file.Close();
			UpdateChecksumLocal();
			m_checksum = m_checksumLocal;

			result = true;
		}
		return result;
	}

	bool IsCheckedOut() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_checkedOut;
	}

	bool IsSandboxed() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_sandboxed;
	}

	bool IsLocked() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_locked;
	}

	bool IsOrphaned() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_orphaned;
	}
	
	const TCHAR *GetLockedBy() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_lockedby;
	}

	const TCHAR * GetModifiedDate() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_modifiedDate;
	}

	const TCHAR * GetModifiedBy() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_modifiedBy;
	}

	ATTRSTATE GetState() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		if (IsOrphaned())
			return ATTRSTATE_ORPHANED;
		else if (IsCheckedOut())
		{
			if (IsSandboxed())
				return ATTRSTATE_CHECKEDOUT_SANDBOXED;
			else
				return ATTRSTATE_CHECKEDOUT;
		}
		else if (IsSandboxed())
			return ATTRSTATE_SANDBOXED;
		else if (IsLocked())
			return ATTRSTATE_LOCKED;
		return ATTRSTATE_NONE;
	}

	virtual const TCHAR *GetStateLabel() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return ATTRSTATELABEL[GetState()];
	}

	unsigned GetHistory(IAttributeHistoryVector & attributes) const
	{
		return attributes.size();
	}

	bool Checkout()
	{
    return true;
	}

	bool Checkin(const TCHAR* comments)
	{
    return true;
  }

	IAttribute * Rename(const TCHAR* label)
	{
		std::_tstring newFilename = label;
		newFilename += _T(".");
		newFilename += GetType()->GetRepositoryCode();
		boost::filesystem::wpath newPath = m_path;
		newPath.remove_filename();
		newPath = newPath / newFilename;
		try
		{
			boost::filesystem::rename(m_path, newPath);
		} catch (const std::exception & ex) {
			_DBGLOG(LEVEL_WARNING, ex.what());
			return NULL;
		}
		IAttribute * newAttr = m_repository->GetAttribute(GetModuleLabel(), label, GetType());
		Refresh(false, newAttr, false);
		return newAttr;
	}

	bool Delete()
	{
		try {
			if (boost::filesystem::exists(m_path))
			{
				int retVal = MoveToRecycleBin(m_path.native_file_string());
				if (retVal != 0)
					throw std::exception("Unknown Error During Folder Delete.", retVal);
				return true;
			}
		} catch (const std::exception & ex) {
			_DBGLOG(LEVEL_WARNING, ex.what());
			return false;
		}
		return false;
	}

	bool Exists() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return !m_placeholder;
	}

	bool Create()
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		if (!Exists())	
		{
			m_placeholder = false;
			proc.unlock();
			if (m_repository->InsertAttribute(GetModuleLabel(), GetLabel(), m_type) != NULL)
			{
				return true;
			}
		}
		return false;
	}

	void Update(const std::_tstring & moduleName, const std::_tstring & label, const std::_tstring & ecl)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		m_placeholder = false;
		m_moduleLabel = moduleName.c_str();
		m_label = label.c_str();
		m_qualifiedLabel = m_moduleLabel + _T(".") + m_label;
		CModuleHelper modHelper(m_qualifiedLabel);
		m_qualifiedLabelNoRoot = modHelper.GetQualifiedLabelNoRoot();
		m_checkedOut = false;
		m_sandboxed = false;
		m_locked = false;

		{
			HANDLE hFile = CreateFile(m_url,
				GENERIC_READ | GENERIC_WRITE, 
				0,
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL, NULL); 

			if (hFile == INVALID_HANDLE_VALUE) 
				m_locked = true;
			else
				CloseHandle(hFile);
		}

		m_orphaned = false;
		bool eclChanged = false;
		if (!ecl.empty())
		{
			eclChanged = m_ecl.Compare(ecl.c_str()) != 0;
			m_ecl = ecl.c_str();
			m_eclSet = true;
			UpdateChecksumLocal();
		}
		m_description = _T("");
		m_lockedby = _T("");
		m_version = 0;
		m_modifiedDate = _T("");
		m_modifiedBy = _T("");
		UpdateID();
		proc.unlock();
		Refresh(eclChanged, NULL, false);
	}

	int PreProcess(PREPROCESS_TYPE action, const TCHAR * overrideEcl, IAttributeVector & attrs, Dali::CEclExceptionVector & errs) const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return false;
	}

	boost::signals::connection on_refresh_connect(const refresh_slot_type& s)
    { 
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return on_refresh.connect(s); 
	}

	void Refresh(bool eclChanged, IAttribute * newAttrAsOldOneMoved, bool deleted)
	{
		on_refresh(this, eclChanged, newAttrAsOldOneMoved, deleted);
	}

	IAttributeHistory * GetAsHistory()	
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return this;
	}
	//const TCHAR *GetEcl()
	//{
	//	clib::recursive_mutex::scoped_lock proc(m_mutex);
	//	return GetEcl(false);
	//}
	const TCHAR *GetDescription() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return _T("");
	}
	IAttribute * GetAttribute() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		IAttribute * self = const_cast<CDiskAttribute *>(this);
		return self;
	}
	bool SetLabel(const std::_tstring & label)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return false;
	}
	//  IDiskAttribute  ---
	const TCHAR * GetPath()
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_url;
	}
};

static CacheT<std::_tstring, CDiskAttribute> DiskAttributeCache;

void ClearDiskAttributeCache()
{
	ATLTRACE(_T("File cache before clearing(size=%u)\r\n"), DiskAttributeCache.Size());
	DiskAttributeCache.Clear();
}

IAttribute * GetDiskAttribute(const IRepository *rep, const TCHAR* module, const TCHAR* label, IAttributeType * type, unsigned version, bool sandboxed, bool placeholder)
{
	CComPtr<CDiskAttribute> attr = new CDiskAttribute(rep, module, label, type->GetRepositoryCode(), boost::filesystem::wpath(), version, sandboxed, placeholder);
	return DiskAttributeCache.Exists(attr->GetCacheID());
}

CDiskAttribute * CreateDiskAttributeRaw(const IRepository *rep, const TCHAR* module, const TCHAR* label, const TCHAR* type, const boost::filesystem::wpath & path, unsigned version, bool sandboxed, bool placeholder)
{
	return DiskAttributeCache.Get(new CDiskAttribute(rep, module, label, type, path, version, sandboxed, placeholder));
}

//IAttribute * CreateDiskAttribute(const IRepository *rep, const TCHAR* module, const TCHAR* label, unsigned version, bool sandboxed)
//{
//	return CreateDiskAttributeRaw(rep, module, label, version, sandboxed, false);
//}

IAttribute * CreateDiskAttributePlaceholder(const IRepository *rep, const TCHAR* module, const TCHAR* label, const TCHAR* type, const boost::filesystem::wpath & path)
{
	return CreateDiskAttributeRaw(rep, module, label, type, path, 0, false, true);
}

IAttribute * CreateDiskAttribute(const IRepository *rep, const std::_tstring &moduleLabel, const std::_tstring &label, const std::_tstring &type, const boost::filesystem::wpath & path, const std::_tstring & ecl)
{
	if (moduleLabel.empty() || label.empty())
		return 0;

	CDiskAttribute * attr = CreateDiskAttributeRaw(rep, moduleLabel.c_str(), label.c_str(), type.c_str(), path, 0, false, false);
	ATLASSERT(attr);
	attr->Update(moduleLabel, label, ecl);
	return attr;
}
