#pragma once

#include "comms.h"
#include "StringSet.h"
#include "RecursiveMutex.h"
#include "FileSprayPath.h"
#include "DFUWorkunit.h"

//  ===========================================================================
namespace WsFileSpray
{
	__interface IFileSpray : public clib::ILockableUnknown
	{
		const TCHAR *GetID() const;
		const TCHAR *GetLabel() const;
		const TCHAR *GetUrl() const;
		//  DaFileSrv
		unsigned int GetFiles(const std::_tstring & _NetAddr, const std::_tstring & _Path, const std::_tstring & _Mask, IFileSprayPathVector * results);
		void WalkFiles(const std::_tstring & _NetAddr, const std::_tstring & _Path, const std::_tstring & _Mask, IVisitor * visitor);
		IFileSprayPath * GetFolder(const std::_tstring & _NetAddr, const std::_tstring & _Path);

		//  DFU
		IWorkunit * GetWorkunit(const std::_tstring & _wuid) const;
		IWorkunit * SprayVariable(const std::_tstring & _sourceIP, const std::_tstring & _sourcePath, const std::_tstring & _destGroup, const std::_tstring & _destLogicalName) const;
		IWorkunit * SprayFixed(const std::_tstring & _sourceIP, const std::_tstring & _sourcePath, int _sourceRecordSize, const std::_tstring & _destGroup, const std::_tstring & _destLogicalName, const std::_tstring & prefix, bool _nosplit) const;

		void MonitorState(const std::_tstring & wuid) const;	
	};

	typedef StlLinked<IFileSpray> IFileSprayAdapt;

	COMMS_API IFileSpray * AttachFileSpray(const CString & url, const CString & label);
	COMMS_API IFileSpray * AttachFileSpray();
}
