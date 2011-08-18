#include "StdAfx.h"
#include "LocalFile.h"

#include "workunit.h"
#include "dali.h"
#include "SoapUtil.h"
#include "cache.h"

#if _COMMS_VER < 68200
using namespace WsWorkunits;
#else
#endif

namespace Dali
{
#if _COMMS_VER < 60600
#else
class CLocalFile : public ILocalFile, public clib::CLockableUnknown, public boost::noncopyable
{
protected:
	CString m_id;
	CString m_url;
	CString m_wuid;
    int m_type;
    CString m_source;
    CString m_destination;
	localfile_signal_type on_refresh;

public:
	IMPLEMENT_CLOCKABLEUNKNOWN;

	CLocalFile()
	{
		ATLASSERT(false);
	}
	CLocalFile(const CString & url, const CString & wuid, const CString & source) : m_url(url), m_wuid(wuid), m_source(source)
	{
		m_id = m_url + _T("/") + m_wuid + _T("/") + m_source;
		m_type = 0;
	}
	virtual ~CLocalFile()
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
	int GetType() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_type;
	}
	const TCHAR *GetSource() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_source;
	}
	const TCHAR *GetDestination() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_destination;
	}
#if _COMMS_VER < 68200
	void Update(const LogicalFileUpload * c)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		bool changed = false;
		if (m_source.Compare(CW2T(c->Source, CP_UTF8)) != 0 || 
			m_destination.Compare(CW2T(c->Destination, CP_UTF8)) != 0 || 
			m_type != c->Type)
		{
			changed = true;
		}
		m_type = c->Type;
		m_source = CW2T(c->Source, CP_UTF8);
		m_destination = CW2T(c->Destination, CP_UTF8);
		if (changed)
		{
			proc.unlock();
			Refresh();
		}
	}
#else
	void Update(const ns6__LogicalFileUpload * c)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		CString prev_source = m_source;
		CString prev_destinition = m_destination;
		int prev_type = m_type;

		SAFE_ASSIGN2CSTRING(m_source, c->Source);
		SAFE_ASSIGN2CSTRING(m_destination, c->Destination);
		SAFE_ASSIGN(m_type, c->Type);
		if (m_source.Compare(prev_source) != 0 || 
			m_destination.Compare(prev_destinition) != 0 || 
			m_type != prev_type)
		{
			proc.unlock();
			Refresh();
		}
	}
#endif
	void Refresh() 
	{
		on_refresh(this);
	}
	boost::signals::connection on_refresh_connect(const localfile_slot_type& s)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return on_refresh.connect(s); 
	}
};

static CacheT<std::_tstring, CLocalFile> LocalFileCache;

void ClearLocalFileCache()
{
	ATLTRACE(_T("LocalFile cache before clearing(size=%u)\r\n"), LocalFileCache.Size());
	LocalFileCache.Clear();
}

CLocalFile * CreateLocalFileRaw(const CString & url, const CString & wuid, const CString & source)
{
	return LocalFileCache.Get(new CLocalFile(url, wuid, source));
}

ILocalFile * CreateLocalFile(const CString & url, const CString & wuid, const CString & source)
{
	return CreateLocalFileRaw(url, wuid, source);
}

#if _COMMS_VER < 68200
ILocalFile * CreateLocalFile(const CString & url, const CString & wuid, const LogicalFileUpload * localfile)
{
	CComPtr<CLocalFile> lf = CreateLocalFileRaw(url, wuid, localfile->Source);
	ATLASSERT(lf);
	lf->Update(localfile);
	return lf;
}
#else
ILocalFile * CreateLocalFile(const CString & url, const CString & wuid, const ns6__LogicalFileUpload * localfile)
{
	CComPtr<CLocalFile> lf = CreateLocalFileRaw(url, wuid, localfile->Source->c_str());
	ATLASSERT(lf);
	lf->Update(localfile);
	return lf;
}
#endif
#endif
} //namespace Dali

