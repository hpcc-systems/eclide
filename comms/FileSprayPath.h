#pragma once

#include "comms.h"
#include "RecursiveMutex.h"

namespace WsFileSpray
{
	__interface IFileSprayPath;
	typedef std::vector<StlLinked<IFileSprayPath> > IFileSprayPathVector;

	__interface IVisitor
	{
		void StartFolder(IFileSprayPath * path);
		void EndFolder(IFileSprayPath * path);
		void FileVistited(IFileSprayPath * path);
	};

	__interface IFileSprayPath : public clib::ILockableUnknown
	{
		void Delete();	// Also removes from cache
		const TCHAR * GetIP() const;
		const TCHAR * GetPath() const;
		const TCHAR * GetLogicalPath() const;
		const TCHAR * GetName() const;
		bool IsDir() const;
		__int64 GetFileSize() const;
		const TCHAR * GetModifiedTime() const;
		IFileSprayPath * GetParent() const;
		unsigned int GetParentage(IFileSprayPathVector * results) const;
		unsigned int GetChildren(IFileSprayPathVector * results, const std::_tstring & mask) const;
		void WalkChildren(const std::_tstring & mask, IVisitor * visitor) const;	
	};

	COMMS_API IFileSprayPath * CreateIFileSprayPath(const std::_tstring & ip, const std::_tstring & folder, const std::_tstring & name = _T(""));
}