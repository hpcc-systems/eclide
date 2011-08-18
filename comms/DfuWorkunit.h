#pragma once

#include "comms.h"
#include "recursivemutex.h"

namespace WsFileSpray
{
__interface IWorkunit;

typedef boost::signal<void(IWorkunit *)> workunit_signal_type;
typedef workunit_signal_type::slot_type workunit_slot_type;

enum WUState 
{
	DFUstate_unknown,
	DFUstate_scheduled,
	DFUstate_queued,
	DFUstate_started,
	DFUstate_aborted,
	DFUstate_failed,
	DFUstate_finished,
	DFUstate_monitoring,
	WUStateSize	//Don't forget to update the string table below..
};

const TCHAR * const WUState_TEXT[] =
{
	_T("Unknown"), 
	_T("Scheduled"), 
	_T("Queued"), 
	_T("Started"), 
	_T("Aborted"), 
	_T("Failed"), 
	_T("Finished"), 
	_T("Monitoring"), 
	_T("ERROR-Please Report"),
};

__interface IWorkunit : public clib::ILockableUnknown
{
	const TCHAR *GetID() const;
	const TCHAR *GetWuid() const;
	WUState GetState() const;
	const TCHAR *GetStateLabel() const;
	bool IsComplete() const;
	int GetPercentDone() const;
	const TCHAR * GetProgressMessage() const;
			
	void MonitorState();	//Notifications come thru "on_refresh_connect"

	boost::signals::connection on_refresh_connect(const workunit_slot_type& s);
};

typedef StlLinked<IWorkunit> IWorkunitAdapt;
typedef std::vector<IWorkunitAdapt> IWorkunitVector;
}
