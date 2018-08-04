#include "StdAfx.h"

#include "combo.h"

//=================================================================
CComboTopologyBase::CComboTopologyBase()
{
}

CComboTopologyBase::~CComboTopologyBase()
{
}

const TCHAR* CComboTopologyBase::SelectDefault(const TCHAR *defSel)
{
	int d;
	if ( defSel )
		d = FindStringExact(-1,defSel);
	else 
		d = -1;

	if ( d < 0 )
	{
		d = FindStringExact(-1,m_defaultItem);
		if ( d < 0 )
			d = 0;
	}
	GetLBText(d,m_value);
	SetCurSel(d);
	return m_value;
}

void CComboTopologyBase::ResetIfNewIP(const TCHAR *ip)
{
	if ( m_topoIP != ip )
	{
		CWaitCursor cursor;
		CString curSelection;
		GetLBText(GetCurSel(),curSelection);
		Load(ip);
		SelectDefault(curSelection);
	}
}

void CComboTopologyBase::SetIP(const TCHAR *ip)
{
	if ( ip )
	{
		m_topoIP = ip;
	}
	else
	{
		m_topoIP = CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_TOPOLOGY));
	}
}
//=================================================================
CComboCluster::CComboCluster()
{
	m_includeAllClusters = false;
	m_defaultItem = _T("roxie");
}
CComboCluster::CComboCluster(bool includeAllClusters)
{
	m_includeAllClusters = includeAllClusters;
	m_defaultItem = _T("roxie");
}
void CComboCluster::RestrictType(const TCHAR *typeName)
{
	m_restrictToType = typeName;
}

void CComboCluster::Load(const TCHAR *ip)
{
	m_clusters.clear();
	ResetContent();
	SetIP(ip);

	//clib::thread run(__FUNCTION__, boost::bind(&GetClusters, this));
	GetClusters(this);
}

void CComboCluster::GetClusters(CComPtr<CComboCluster> self)
{
	StlLinked<Topology::ITopology> server = AttachTopology(self->m_topoIP, self->m_topoIP);
	Topology::IClusterVector clusters;
	if (self->m_includeAllClusters)
	{
		server->GetClusters(_T(""), clusters);
	}
	else
	{
		server->GetClusters(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_QUEUE), clusters, static_cast<const TCHAR *>(self->m_restrictToType));
	}
	self->LoadClusters(clusters);
}

void CComboCluster::LoadClusters(Topology::IClusterVector & clusters)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	//in order for GetSelectedCluster to work, they must stay in same order
	//unless we search the array by name!
	m_clusters = clusters;
	if (m_includeAllClusters)
	{
		InsertString(-1, ALL_CLUSTERS);
	}

	std::_tstring name;
	for( int i=0; i<(int)m_clusters.size(); i++ )
	{
		//InsertItem(-1, m_clusters[i]->GetName(), 0, 0, 0);
		InsertString(-1, m_clusters[i]->GetName());
	}
}

Topology::ICluster * CComboCluster::GetSelectedCluster()
{
	int nSel = GetCurSel();
	if ( nSel >= 0 && nSel < (int)m_clusters.size() )
	{
		return m_clusters[nSel];
	}
	return 0;
}

void CComboCluster::SelectDefault(const TCHAR *defSel)
{
	int d = FindStringExact(-1, defSel);
	if ( d < 0 )
	{
		d = FindStringExact(-1, m_includeAllClusters ? ALL_CLUSTERS : _T(""));
		if ( d < 0 )
			d = 0;
	}
	GetLBText(d,m_value);
	SetCurSel(d);
}

//=================================================================
CComboQueueCluster::CComboQueueCluster()
{
	m_defaultItem = _T("roxie");
}

void CComboQueueCluster::Load(const TCHAR *ip)
{
	boost::recursive_mutex::scoped_lock proc(m_mutex);
	m_queueClusters.clear();
	ResetContent();
	SetIP(ip);

	GetQueueClusters(this);
}

void thread_PreloadGetClusters(CComboQueueCluster * self, std::_tstring queue)
{
	StlLinked<Topology::ITopology> server = AttachTopology(self->m_topoIP, self->m_topoIP);
	Topology::IClusterVector clusters;
	server->GetClusters(queue.c_str(), clusters);
}

void CComboQueueCluster::GetQueueClusters(CComPtr<CComboQueueCluster> self)
{
	StlLinked<Topology::ITopology> server = AttachTopology(self->m_topoIP, self->m_topoIP);
	QueueClusterPairVector queueClusters;
	Topology::IEclServerVector ecl_servers;
	server->GetEclServers(ecl_servers);

	clib::CThreadQueue preloadClusters(0);
	if (ecl_servers.empty())
	{
		preloadClusters.Append(__FUNCTION__, boost::bind(thread_PreloadGetClusters, self, _T("")));
	}
	else
	{
		for(Topology::IEclServerVector::const_iterator itr = ecl_servers.begin(); itr != ecl_servers.end(); ++itr)
			preloadClusters.Append(__FUNCTION__, boost::bind(thread_PreloadGetClusters, self, itr->get()->GetQueue()));
	}

	preloadClusters.SetMaxThreadCount(30);
	preloadClusters.Join();

	if (ecl_servers.empty())
	{
		Topology::IClusterVector clusters;
		server->GetClusters(_T(""), clusters);
		for(Topology::IClusterVector::const_iterator c_itr = clusters.begin(); c_itr != clusters.end(); ++c_itr)
		{
			queueClusters.push_back(std::make_pair((Topology::IEclServer *)NULL, c_itr->get()));
		}
	}
	else
	{
		std::map<std::_tstring, bool> dedupQueue; 
		for(Topology::IEclServerVector::const_iterator itr = ecl_servers.begin(); itr != ecl_servers.end(); ++itr)
		{
			if (dedupQueue[itr->get()->GetQueue()] == false)
			{
				dedupQueue[itr->get()->GetQueue()] = true;
				Topology::IClusterVector clusters;
				server->GetClusters(itr->get()->GetQueue(), clusters);
				for(Topology::IClusterVector::const_iterator c_itr = clusters.begin(); c_itr != clusters.end(); ++c_itr)
					queueClusters.push_back(std::make_pair(itr->get(), c_itr->get()));
			}
		}
	}

	self->LoadQueueCluster(queueClusters);
}

void CComboQueueCluster::LoadQueueCluster(const QueueClusterPairVector & queueClusters)
{
	boost::recursive_mutex::scoped_lock proc(m_mutex);
	m_queueClusters = queueClusters;
	std::_tstring queue;
	for(QueueClusterPairVector::const_iterator itr = m_queueClusters.begin(); itr != m_queueClusters.end(); ++itr)
	{
		if (itr->first && !boost::algorithm::equals(queue, itr->first.get()->GetQueue()))
		{
			queue = itr->first.get()->GetQueue();
			int row = InsertString(-1, queue.c_str());
			const QueueClusterPair * queueCluster = &(*itr);
			SetItemDataPtr(row, (void *)queueCluster);
		}
		std::_tstring str;
		if (!queue.empty())
			str += std::_tstring(_T("    "));
		str += itr->second.get()->GetID();
		int row = InsertString(-1, str.c_str());
		const QueueClusterPair * queueCluster = &(*itr);
		SetItemDataPtr(row, (void *)queueCluster);
	}
}

void CComboQueueCluster::SelectClusterQueue(const std::_tstring & queue, const std::_tstring & cluster)
{
	m_queue = queue;
	m_cluster = cluster;
	for (int i = 0; i < GetCount(); ++i)
	{
		QueueClusterPair * queueCluster = (QueueClusterPair *)GetItemDataPtr(i);
		if (queueCluster->first)
		{
			if (boost::algorithm::iequals(queue, queueCluster->first.get()->GetQueue()) && boost::algorithm::iequals(cluster, queueCluster->second.get()->GetID()))
			{
				SetCurSel(i);
				return;
			}
		}
		else
		{
			if (queue.empty() && boost::algorithm::iequals(cluster, queueCluster->second.get()->GetID()))
			{
				SetCurSel(i);
				return;
			}
		}
	}

	//  No matching pair found, select first item.
	SetCurSel(0);
	m_queue = GetSelectedQueue() != NULL ? GetSelectedQueue()->GetQueue() : _T("");
	m_cluster = GetSelectedCluster() != NULL ? GetSelectedCluster()->GetID() : _T("");
}

Topology::IEclServer * CComboQueueCluster::GetSelectedQueue()
{
	int nSel = GetCurSel();
	if (nSel >= 0)
	{
		int nSel = GetCurSel();
		QueueClusterPair * queueCluster = (QueueClusterPair *)GetItemDataPtr(nSel);
		if (queueCluster)
			return queueCluster->first;
	}
	return NULL;
}

Topology::ICluster * CComboQueueCluster::GetSelectedCluster()
{
	int nSel = GetCurSel();
	if (nSel >= 0)
	{
		QueueClusterPair * queueCluster = (QueueClusterPair *)GetItemDataPtr(nSel);
		if (queueCluster)
			return queueCluster->second;
	}
	return NULL;
}
//=================================================================
CComboDropZone::CComboDropZone()
{
	m_defaultItem = _T("");
}
void CComboDropZone::Load(const TCHAR *ip)
{
	ResetContent();

	SetIP(ip);

	StlLinked<Topology::ITopology> server = AttachTopology(m_topoIP, m_topoIP);
	server->GetDropZones(m_dropZones);
	//in order for GetSelectedDropZone to work, they must stay in same order!!!
	std::_tstring name;
	for( int i=0; i<(int)m_dropZones.size(); i++ )
	{
		InsertString(-1, m_dropZones[i]->GetName(name));
	}
}

Topology::IDropZone *CComboDropZone::GetSelectedDropZone()
{
	int nSel = GetCurSel();
	if ( nSel >= 0 && nSel < (int)m_dropZones.size() )
	{
		return m_dropZones[nSel];
	}
	return 0;
}

bool CComboDropZone::SelectByMachineIP(const TCHAR *ip)
{
	for( int i=0; i<(int)m_dropZones.size(); i++ )
	{
		std::_tstring machineIP;
		const Topology::IMachine *machine = m_dropZones[i]->GetMachine();
		if ( machine )
		{
			machine->GetNetaddress(machineIP);
			if ( machineIP == ip )
				return SetCurSel(i) >= 0;
		}
	}
	return false;
}

//  ===========================================================================
void CComboState::Load()
{
	ResetContent();
	for (int i = Dali::WUStateUnknown; i < Dali::WUStateSize; ++i)
	{
		AddString(Dali::WUState_TEXT[i]);
	}
}

Dali::WUState CComboState::GetState()
{
	return GetState(m_value);
}

Dali::WUState CComboState::GetState(const CString& state)
{
	for (int i = Dali::WUStateUnknown; i < Dali::WUStateSize; ++i)
	{
		if (state.CompareNoCase(Dali::WUState_TEXT[i]) == 0)
			return static_cast<Dali::WUState>(i);
	}
	return Dali::WUStateUnknown;
}
//  ===========================================================================
void CComboLabel::Load(const TCHAR *label)
{
	ResetContent();
	if (label)
	{
		m_value = label;
	}
	else
	{
		m_value = _T("");
	}

	AddString(_T(""));
	clib::thread run(__FUNCTION__, boost::bind(&GetLabels, this));
}

void CComboLabel::Load(CComPtr<IRepository> rep, const TCHAR *label)
{
	ResetContent();
	if (label)
	{
		m_value = label;
	}
	else
	{
		m_value = _T("");
	}

	AddString(_T(""));
	clib::thread run(__FUNCTION__, boost::bind(&GetLabels2, this, rep));
}

void CComboLabel::GetLabels(CComPtr<CComboLabel> self)
{
	CRepLabelVector labels;
	CComPtr<IRepository> rep = AttachRepository();
	rep->GetLabels(labels);
	self->LoadLabels(labels);
}

void CComboLabel::GetLabels2(CComPtr<CComboLabel> self, CComPtr<IRepository> rep)
{
	CRepLabelVector labels;
	rep->GetLabels(labels);
	self->LoadLabels(labels);
}

void CComboLabel::LoadLabels(CRepLabelVector & labels)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	for(CRepLabelVector::iterator itr = labels.begin(); itr != labels.end(); ++itr)
	{
		InsertString(-1,itr->get()->m_label.c_str());
	}
}

static const TCHAR DefaultLabel[] = _T("");

void CComboLabel::SelectDefault(const TCHAR *defSel)
{
	int d = FindStringExact(-1, defSel);
	if ( d < 0 )
	{
		d = FindStringExact(-1, DefaultLabel);
		if ( d < 0 )
			d = 0;
	}
	GetLBText(d,m_value);
	SetCurSel(d);
}

CComboLabel & CComboLabel::operator = (HWND hWnd)
{
	CComboBoxEx2::operator =(hWnd);
	return *this;
}
//  ===========================================================================
void CComboModule::Load(const TCHAR *label, bool includeBlank, bool includeReadOnly)
{
	ResetContent();
	if (label)
	{
		m_value = label;
	}
	else
	{
		m_value = _T("");
	}

	if (includeBlank)
		AddString(_T(""));
	clib::thread run(__FUNCTION__, boost::bind(&GetModules, this, includeReadOnly));
}

void CComboModule::GetModules(CComPtr<CComboModule> self, bool includeReadOnly)
{
	CComPtr<IRepository> rep = AttachRepository();

	IModuleVector modules;
	rep->GetAllModules(modules);
	self->LoadModules(modules, includeReadOnly);
}

void CComboModule::LoadModules(IModuleVector & modules, bool includeReadOnly)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	for(IModuleVector::iterator itr = modules.begin(); itr != modules.end(); ++itr)
	{
		if (includeReadOnly || ((itr->get()->GetAccess() & SecAccess_Write) == SecAccess_Write))
		{
			int item = InsertString(-1,itr->get()->GetQualifiedLabel());
			SetItemDataPtr(item, itr->get());
		}
	}
	SetCurSel(0);
	GetSelectedModule();
}

void CComboModule::SelectDefault(const TCHAR *defSel)
{
	int d = FindStringExact(-1, defSel);
	if ( d < 0 )
	{
		d = FindStringExact(-1, DefaultLabel);
		if ( d < 0 )
			d = 0;
	}
	GetLBText(d,m_value);
	SetCurSel(d);
}

CComboModule & CComboModule::operator = (HWND hWnd)
{
	CComboBoxEx2::operator =(hWnd);
	return *this;
}

const TCHAR * CComboModule::GetSelectedModule()
{
	GetLBText(GetCurSel(), m_value);
	return m_value;
}

IModule * CComboModule::GetSelectedIModule()
{
	return (IModule *)this->GetItemDataPtr(GetCurSel());
}
//  ===========================================================================


