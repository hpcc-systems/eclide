#include "StdAfx.h"
#include "Dfu.h"

#include "clib.h"
#include "SoapUtil.h"
#include "cache.h"
#include "logger.h"
#include "global.h"
#include "util.h"
#include "utilDateTime.h"
#include "thread.h"

#if _COMMS_VER < 58800
#else
#if _COMMS_VER < 68200
using namespace WsDfu;
#else
#endif

namespace Dfu
{
class CFile;
CFile * CreateCFile(IServer * server, const std::_tstring & name);

class CFile : public IFile, public clib::CLockableUnknown
{
protected:
	CComPtr<IServer> m_server;
	std::_tstring m_id;
	std::_tstring m_label;
	StlLinked<CFile> m_parent;
	std::_tstring m_leaf;

	bool m_isDir;
	__int64 m_filesize;
	std::_tstring m_modifiedtime;

	refresh_signal_type on_refresh;
	
public:
	IMPLEMENT_CUNKNOWN;

	CFile(IServer * server, const std::_tstring & label) : m_server(server)
	{
		m_id = label;
		m_label = label;
		typedef std::vector<std::_tstring> split_vector_type;
		split_vector_type SplitVec; 
		boost::algorithm::split(SplitVec, label, boost::algorithm::is_any_of("::"), boost::algorithm::token_compress_on);
		ATLASSERT(SplitVec.size());
		std::_tstring parentLabel;
		for(unsigned int i = 0; i < SplitVec.size(); ++i)
		{
			if (i == SplitVec.size() - 1)
			{
				m_leaf = SplitVec[i];
			}
			else
			{
				if (i > 0)
				{
					parentLabel += _T("::");
				}
				parentLabel += SplitVec[i];
			}
		}
		if (parentLabel.size())
		{
			m_parent = CreateCFile(m_server, parentLabel);
			m_parent->IsDir(true);
		}
	}

	const TCHAR * GetLabel() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_label.c_str();
	}

	const TCHAR * GetLeaf() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_leaf.c_str();
	}

	bool IsDir() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_isDir;
	}

	void IsDir(bool dir)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		m_isDir = dir;
	}

	const TCHAR * GetCacheID() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_id.c_str();
	}

	//void Update(const PhysicalFileStruct * c)
	//{
	//	clib::recursive_mutex::scoped_lock proc(m_mutex);
	//	m_isDir = c->isDir;
	//	m_filesize = c->filesize;
	//	m_modifiedtime = CW2T(c->modifiedtime, CP_UTF8);
	//	proc.unlock();
	//	Refresh();
	//}

#if _COMMS_VER < 68200
	void Update(const DFULogicalFile * c)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		m_isDir = c->isDirectory;
		m_filesize = c->IntSize;
		m_modifiedtime = CW2T(c->Modified, CP_UTF8);
		proc.unlock();
		Refresh();
	}
#else
	void Update(const ns3__DFULogicalFile * c)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		m_isDir = c->isDirectory;
		SAFE_ASSIGN(m_filesize, c->IntSize);
		SAFE_ASSIGN(m_modifiedtime, c->Modified);
		proc.unlock();
		Refresh();
	}
#endif

	void Refresh()
	{
		on_refresh(this);
	}

	IFile * GetParent()
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_parent;
	}

	boost::signals::connection on_refresh_connect(const refresh_slot_type& s)
    { 
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return on_refresh.connect(s); 
	}

	bool operator < (const IFile & r) const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		if (IsDir() != r.IsDir())
		{
			return IsDir() > r.IsDir();
		}
		CString lhs = GetLeaf();
		return lhs.CompareNoCase(r.GetLeaf()) > 0 ? false : true;
	}
};

static CacheT<std::_tstring, CFile> FileCache;

void ClearDfuFileCache()
{
	ATLTRACE(_T("DFU File cache before clearing(size=%u)\r\n"), FileCache.Size());
	FileCache.Clear();
}

CFile * CreateCFile(IServer * server, const std::_tstring & name)
{
	return FileCache.Get(new CFile(server, name));
}

//IFile * CreateIFile(IServer * server, PhysicalFileStruct * info)
//{
//	CFile * retVal = CreateCFile(server, static_cast<const TCHAR *>(CW2T(info->name, CP_UTF8)));
//	retVal->Update(info);
//	return retVal;
//}

#if _COMMS_VER < 68200
IFile * CreateIFile(IServer * server, const std::_tstring & scope, DFULogicalFile * info)
{
	std::_tstring label;
	if (info->isDirectory)
	{
		label = scope;
		if (scope.size())
			label += _T("::");
		label += CW2T(info->Directory, CP_UTF8);
	}
	else
	{
		label = CW2T(info->Name, CP_UTF8);
	}
	CFile * retVal = CreateCFile(server, label);
	retVal->Update(info);
	return retVal;
}
#else
IFile * CreateIFile(IServer * server, const std::_tstring & scope, ns3__DFULogicalFile * info)
{
	std::_tstring label;
	if (info->isDirectory)
	{
		label = scope;
		if (scope.size())
			label += _T("::");
		if (info->Directory)
			label += *info->Directory;
	}
	else
	{
		SAFE_ASSIGN(label, info->Name);
	}
	CFile * retVal = CreateCFile(server, label);
	retVal->Update(info);
	return retVal;
}
#endif
}
#endif