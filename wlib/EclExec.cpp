#include "StdAfx.h"

#include "EclExec.h"

#include <RecursiveMutex.h> //clib
#include "Thread.h" //clib

void CEclExec::ExecEcl(const TCHAR *clusterName, const TCHAR *queueName, Dali::WUAction action, const TCHAR *eclSource, const TCHAR *eclPath, const TCHAR *scheduled, const TCHAR *label, int resultLimit, const TCHAR *debugSettings, bool archive, int maxRunTime, bool debug)
{
	if (scheduled && scheduled[0])
	{
		clib::thread run(__FUNCTION__, boost::bind(&EclSchedule, this, std::make_pair(clusterName, queueName), action, std::make_pair(eclSource, scheduled), label, resultLimit, debugSettings, archive, maxRunTime));
	}
	else
	{
		BindLimitStruct bls = {archive, maxRunTime, debug};
		clib::thread run(__FUNCTION__, boost::bind(&EclGo, this, std::make_pair(clusterName, queueName), action, eclSource, eclPath, label, resultLimit, debugSettings, bls));
	}
}

void CEclExec::ExecEclNoRefCount(const TCHAR *clusterName, const TCHAR *queueName, Dali::WUAction action, const TCHAR *eclSource, const TCHAR *eclPath)
{
	clib::thread run(__FUNCTION__, boost::bind(&EclGoNoRefCount, this, clusterName, queueName, action, eclSource, eclPath));
}

void CEclExec::WorkunitDelete(Dali::IWorkunit* wu)
{
	clib::thread run(__FUNCTION__, boost::bind(&DeleteWorkunit, wu));
}

void CEclExec::operator()(Dali::WUEvent evnt, Dali::IWorkunit* src)
{
	switch(evnt)
	{
	case Dali::WUEventUpdated:
		WorkunitUpdated(src);
		break;
	case Dali::WUEventDeleted:
		WorkunitDeleted(src);
		break;
	}
}

void CEclExec::WorkunitCreated(Dali::IWorkunit * /*src*/)
{
}

void CEclExec::WorkunitUpdated(Dali::IWorkunit * /*src*/)
{
}

void CEclExec::WorkunitDeleted(Dali::IWorkunit * /*src*/)
{
}

void UpdateDefaultQueue(const CString & queue)
{
	if (queue.Compare(CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_QUEUE))) != 0)
	{
		GetIConfig(QUERYBUILDER_CFG)->Set(GLOBAL_QUEUE, queue);
	}
}

void UpdateDefaultCluster(const CString & cluster)
{
	if (cluster.Compare(CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_CLUSTER))) != 0)
	{
		GetIConfig(QUERYBUILDER_CFG)->Set(GLOBAL_CLUSTER,cluster);
	}
}

//static 
void CEclExec::DeleteWorkunit(Dali::IWorkunit * workunit)
{
	Dali::IWorkunitVector workunits;
	workunits.push_back(workunit);
	StlLinked<Dali::IDali> server = Dali::AttachDali();
	ATLASSERT( server.isLinked() );
	server->DeleteWorkunits(&workunits);
}

//static 
void CEclExec::EclGo(CComPtr<CEclExec> t, std::pair<std::_tstring, std::_tstring> clusterQueue, Dali::WUAction action, CString ecl, CString path, CString label, int resultLimit, CString debugSettings, BindLimitStruct bls)
{
	CString wuid;
	StlLinked<Dali::IDali> server = Dali::AttachDali();
	StlLinked<Dali::IWorkunit> workunit = server->Submit(clusterQueue.first.c_str(), clusterQueue.second.c_str(), action, ecl, path, label, resultLimit, debugSettings, bls.archive, bls.maxRunTime, bls.debug);
	if ( workunit.isLinked() )
	{
		UpdateDefaultCluster(clusterQueue.first.c_str());
		UpdateDefaultQueue(clusterQueue.second.c_str());
		t->WorkunitCreated(workunit.get());
		workunit->on_refresh_connect(boost::ref(*t));
		workunit->MonitorState();
	}
}

//static 
void CEclExec::EclGoNoRefCount(CEclExec * t, CString cluster, CString queue, Dali::WUAction action, CString ecl, CString path)
{
	CString wuid;
	StlLinked<Dali::IDali> server = Dali::AttachDali();
	StlLinked<Dali::IWorkunit> workunit = server->Submit(cluster, queue, action, ecl, path, _T(""), 0, _T(""), false, 0, false);
	if ( workunit.isLinked() )
	{
		UpdateDefaultCluster(cluster);
		UpdateDefaultQueue(queue);
		t->WorkunitCreated(workunit.get());
		workunit->on_refresh_connect(boost::ref(*t));
		workunit->MonitorState();
	}
}

//static 
void CEclExec::EclSchedule(CComPtr<CEclExec> t, std::pair<std::_tstring, std::_tstring> clusterQueue, Dali::WUAction action, std::pair<std::_tstring, std::_tstring> eclWhen, CString label, int resultLimit, CString debugSettings, bool archive, int maxRunTime)
{
	CString wuid;
	StlLinked<Dali::IDali> server = Dali::AttachDali();
	StlLinked<Dali::IWorkunit> workunit = server->Schedule(clusterQueue.first.c_str(), clusterQueue.second.c_str(), eclWhen.first.c_str(), eclWhen.second.c_str(), label, resultLimit, debugSettings, archive);
	if ( workunit.isLinked() )
	{
		UpdateDefaultCluster(clusterQueue.first.c_str());
		UpdateDefaultQueue(clusterQueue.second.c_str());
		t->WorkunitCreated(workunit.get());
		workunit->on_refresh_connect(boost::ref(*t));
	}
}
