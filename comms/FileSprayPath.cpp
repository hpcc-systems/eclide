#include "StdAfx.h"

#include "FileSprayPath.h"

#include "FileSpray.h"
#include "cache.h"

namespace WsFileSpray
{

enum FILESYSTEM
{
	UNKNOWN,
	PORTABLE_POSIX_NAME, 
    WINDOWS_NAME, 
    PORTABLE_NAME, 
    PORTABLE_DIRECTORY_NAME, 
    PORTABLE_FILE_NAME
};

class CFileSprayPath;
CFileSprayPath * CreateCFileSprayPath(IFileSpray * server, const std::_tstring & ip, const std::_tstring & folder, const std::_tstring & name = _T(""));
static CacheT<std::_tstring, CFileSprayPath> FileSprayPathCache;

class CFileSprayPath : public IFileSprayPath, public clib::CLockableUnknown
{
protected:
	CComPtr<IFileSpray> m_server;
	std::_tstring m_ip;
	std::_tstring m_folder;
	boost::filesystem::path m_path;
	std::_tstring m_pathStr;
	std::_tstring m_logicalPathStr;
	FILESYSTEM m_fileSystem;
	std::_tstring m_id;

	std::_tstring m_name;
	bool m_isDir;
	__int64 m_filesize;
	std::_tstring m_modifiedtime;

public:
	IMPLEMENT_CLOCKABLEUNKNOWN

	CFileSprayPath(IFileSpray * server, const std::_tstring & ip, const std::_tstring & folder, const std::_tstring & name) : m_server(server), m_ip(ip), m_folder(folder), m_name(name), m_filesize(0), m_isDir(false)
	{
		boost::algorithm::replace_all(m_folder, _T("$"), _T("_dollar"));
		m_fileSystem = GetFileSystem(m_folder); // avoids asserts for non-posix names && IsPortablePosixPath(m_name);
		m_path = ToPath(m_folder) / ToPath(m_name);
		ToString(m_path, m_pathStr);
		boost::algorithm::replace_all(m_pathStr, _T("_dollar"), _T("$"));
		m_logicalPathStr = m_pathStr;
		boost::algorithm::replace_all(m_logicalPathStr, _T("/"), _T("::"));
		boost::algorithm::replace_all(m_logicalPathStr, _T("\\"), _T("::"));
		if (!boost::algorithm::istarts_with(m_logicalPathStr, _T("::")))
		{
			m_logicalPathStr = std::_tstring(_T("fsas::")) + m_logicalPathStr; 
		}
		else
		{
			m_logicalPathStr = std::_tstring(_T("fsas")) + m_logicalPathStr; 
		}

		m_id = m_ip + m_pathStr;
	}

	const TCHAR * GetCacheID() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_id.c_str();
	}

	void Delete()
	{
		AddRef();
		FileSprayPathCache.Clear(GetCacheID());
		ATLASSERT(!FileSprayPathCache.Exists(GetCacheID()));
		Release();
	}

	const TCHAR * GetIP() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_ip.c_str();
	}

	const TCHAR * GetPath() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_pathStr.c_str();
	}

	const TCHAR * GetLogicalPath() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_logicalPathStr.c_str();
	}

	const TCHAR * GetName() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_name.c_str();
	}

	bool IsDir() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_isDir;
	}

	void SetDir(bool isDir)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		m_isDir = isDir;
	}

	__int64 GetFileSize() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_filesize;
	}

	const TCHAR * GetModifiedTime() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		return m_modifiedtime.c_str();
	}

	IFileSprayPath * GetParent() const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		boost::filesystem::path parent = m_path.branch_path();
		std::_tstring folder;
		ToString(parent, folder);
		if (folder.length())
		{
			CFileSprayPath * retVal = CreateCFileSprayPath(m_server, m_ip, folder);
			retVal->SetDir(true);
			return retVal;
		}
		return NULL;
	}

	unsigned int GetParentage(IFileSprayPathVector * results) const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		CComPtr<IFileSprayPath> item = const_cast<IFileSprayPath *>((const IFileSprayPath *)this);
		while(item)
		{
			results->push_back(const_cast<IFileSprayPath *>(item.p));
			item = item->GetParent();
		}
		return results->size();
	}

	unsigned int GetChildren(IFileSprayPathVector * results, const std::_tstring & mask) const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		std::_tstring path;
		return m_server->GetFiles(m_ip, m_pathStr, mask, results);
	}

	void WalkChildren(const std::_tstring & mask, IVisitor * visitor) const
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		std::_tstring path;
		return m_server->WalkFiles(m_ip, m_pathStr, mask, visitor);

		//IFileSprayPathVector children;
		//GetChildren(&children, mask);
		//for(IFileSprayPathVector::iterator itr = children.begin(); itr != children.end(); ++itr)
		//{
		//	CComPtr<IFileSprayPath> child = *itr;
		//	visitor->PathVistited(child);
		//	//  Remove from cache
		//	CFileSprayPath* raw_child = reinterpret_cast<CFileSprayPath*>(child.p);
		//	FileSprayPathCache.Clear(raw_child->GetCacheID());
		//}
		//children.clear();

		////  Now lets recurse the folders
		//GetChildren(&children, _T("*."));
		//for(IFileSprayPathVector::iterator itr = children.begin(); itr != children.end(); ++itr)
		//{
		//	CComPtr<IFileSprayPath> child = *itr;
		//	child->WalkChildren(visitor, mask);
		//	//  Remove from cache
		//	CFileSprayPath* raw_child = reinterpret_cast<CFileSprayPath*>(child.p);
		//	FileSprayPathCache.Clear(raw_child->GetCacheID());
		//}
	}

	void Update(const FileSpray::PhysicalFileStruct * c)
	{
		clib::recursive_mutex::scoped_lock proc(m_mutex);
		m_name = CW2T(c->name, CP_UTF8);
#if _COMMS_VER < 52501 
#else
		m_isDir = c->isDir;
		m_filesize = c->filesize;
		m_modifiedtime = CW2T(c->modifiedtime, CP_UTF8);
#endif
	}

	FILESYSTEM GetFileSystem(const std::_tstring & fileFolder)
	{
		try
		{
			boost::filesystem::path(CT2A(fileFolder.c_str()), boost::filesystem::portable_posix_name);
			return PORTABLE_POSIX_NAME;
		}	catch(boost::filesystem::filesystem_error &) { }
		try
		{
			boost::filesystem::path(CT2A(fileFolder.c_str()), boost::filesystem::native);
			return WINDOWS_NAME;
		}	catch(boost::filesystem::filesystem_error &) { }
		try
		{
			boost::filesystem::path(CT2A(fileFolder.c_str()), boost::filesystem::portable_name);
			return PORTABLE_NAME;
		}	catch(boost::filesystem::filesystem_error &) { }
		try
		{
			boost::filesystem::path(CT2A(fileFolder.c_str()), boost::filesystem::portable_directory_name);
			return PORTABLE_DIRECTORY_NAME;
		}	catch(boost::filesystem::filesystem_error &) { }
		try
		{
			boost::filesystem::path(CT2A(fileFolder.c_str()), boost::filesystem::portable_file_name);
			return PORTABLE_FILE_NAME;
		}	catch(boost::filesystem::filesystem_error &) { }
		return UNKNOWN;
	}

	boost::filesystem::path ToPath(const std::_tstring & fileFolder)
	{
		try
		{
			switch(m_fileSystem)
			{
			case PORTABLE_POSIX_NAME:
				return boost::filesystem::path(CT2A(fileFolder.c_str()), boost::filesystem::portable_posix_name);
				break;
			case WINDOWS_NAME:
				return boost::filesystem::path(CT2A(fileFolder.c_str()), boost::filesystem::native);
				break;
			case PORTABLE_NAME:
				return boost::filesystem::path(CT2A(fileFolder.c_str()), boost::filesystem::portable_name);
				break;
			case PORTABLE_DIRECTORY_NAME:
				return boost::filesystem::path(CT2A(fileFolder.c_str()), boost::filesystem::portable_directory_name);
				break;
			case PORTABLE_FILE_NAME:
				return boost::filesystem::path(CT2A(fileFolder.c_str()), boost::filesystem::portable_file_name);
				break;
			default:
				break;
			}
		}
		catch(boost::filesystem::filesystem_error &e)
		{
			(e);
			//ATLASSERT(false);
		}
		return boost::filesystem::path("invalid_folder_name");
	}
	
	const TCHAR * ToString(const boost::filesystem::path & path, std::_tstring & result) const
	{
		switch(m_fileSystem)
		{
		case PORTABLE_POSIX_NAME:
			result = CA2T(path.string().c_str());
			break;
		case WINDOWS_NAME:
			result = CA2T(path.native_file_string().c_str());
			break;
		case PORTABLE_NAME:
			result = CA2T(path.string().c_str());
			break;
		case PORTABLE_DIRECTORY_NAME:
			result = CA2T(path.string().c_str());
			break;
		case PORTABLE_FILE_NAME:
			result = CA2T(path.string().c_str());
			break;
		}
		return result.c_str();
	}
};

CFileSprayPath * CreateCFileSprayPath(IFileSpray * server, const std::_tstring & ip, const std::_tstring & folder, const std::_tstring & name)
{
	return FileSprayPathCache.Get(new CFileSprayPath(server, ip, folder, name));
}

IFileSprayPath * CreateIFileSprayPath(IFileSpray * server, const std::_tstring & ip, const std::_tstring & folder, const FileSpray::PhysicalFileStruct * data)
{
	std::_tstring name = CW2T(data->name, CP_UTF8);
	CFileSprayPath * retVal = CreateCFileSprayPath(server, ip, folder, name);
	ATLASSERT(retVal);
	retVal->Update(data);
	return retVal;
}

IFileSprayPath * CreateIFileSprayPath(const std::_tstring & ip, const std::_tstring & folder, const std::_tstring & name)
{
	CComPtr<WsFileSpray::IFileSpray> server = WsFileSpray::AttachFileSpray(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_FILESPRAY) ,_T("FileSpray"));
	return CreateCFileSprayPath(server, ip, folder, name);
}

}
