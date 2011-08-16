#pragma once

#include "comms.h"
#include "Repository.h"
#include "Workspace.h"
#include "Thread.h"

__interface IMigrationCallback
{
	BOOL Invalidate(BOOL bErase = TRUE);
	void LogMsg(const std::_tstring & msg);
	void PostStatus(const TCHAR* pStr);
	void PostProgress(int progress);
};

__interface IMigration : public IUnknown
{
	void AddToRep(IWorkspaceAdapt fromWorkspace);
	void AddToRep(IAttributeHistoryAdapt fromAttr, const std::_tstring & comment, bool sandbox);
	void AddEclToRep(const std::_tstring & modLabel, const std::_tstring & attrLabel, IAttributeType * type, const std::_tstring & comment, const std::_tstring & ecl, const std::_tstring & by, bool sandbox);
	void AddEclToRep(const std::_tstring & modAttrLabel, IAttributeType * type, const std::_tstring & comment, const std::_tstring & ecl, const std::_tstring & by, bool sandbox);
	void Start();
	void Stop();
	void Join();
	int GetActiveMigrationCount();
};

//Typical usage:
//	SetMaxMigrationThreadCount(0);
//	AddToRep(...);
//	AddToRep(...);
//	AddToRep(...);
//	SetMaxMigrationThreadCount(5);
//	While(GetActiveMigrationCount() > 0)
//		::Sleep(1000);

COMMS_API IMigration * CreateIMigration(IRepository * targetRep, IMigrationCallback * caller = NULL);
