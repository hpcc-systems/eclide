#include "StdAfx.h"
#include "dalitreenode.h"

#include "dali.h"
#include "attribute.h"
#include <utilDateTime.h> //clib

const TCHAR * const Weekdays[] = { _T("Sunday"), _T("Monday"), _T("Tuesday"), _T("Wednesday"), _T("Thursday"), _T("Friday"), _T("Saturday") };
const TCHAR * const PrevWeeks[] = { _T("Last Week"), _T("Two Weeks Ago"), _T("Three Weeks Ago"), _T("Four Weeks Ago"), _T("Five Weeks Ago"), _T("Six Weeks Ago"), _T("Seven Weeks Ago") };
const TCHAR * const Months[] = { _T("0 Month!"),_T("January"),_T("February"),_T("March"),_T("April"),_T("May"),_T("June"),_T("July"),_T("August"),_T("September"),_T("October"),_T("November"),_T("December") };

//  ===============================================================================================
CDaliBaseNode::CDaliBaseNode(IDaliSlot *owner, bool allUsers, const std::_tstring & cluster) : m_Owner(owner), m_AllUsers(allUsers), m_Cluster(cluster.c_str())
{
}
//  ===============================================================================================
CDaliNode::CDaliNode(IDaliSlot *owner, bool allUsers, const std::_tstring & cluster, bool bVirtualNode)	: CDaliBaseNode(owner, allUsers, cluster)
{
	m_bVirtualNode = bVirtualNode;
}

CDaliNode::~CDaliNode() 
{
	if (m_loadingNode)
	{
		m_loadingNode->Delete();
		m_loadingNode = NULL;
	}
}

HTREEITEM CDaliNode::GetRoot()
{
	return m_bVirtualNode ? TVI_ROOT : *this;
}

class Stub {
public:
	CDaliNode * m_orig;
	int m_allUsers;
	void operator() (Dali::IWorkunitVectorAdapt wus) {
		m_orig->callback(wus, m_allUsers);
	}
};

void CDaliNode::UpdateToday()
{
	using namespace boost::gregorian;
	date today = day_clock::local_day();
    date_duration oneDay(1);
	//  Days---
	//CDateFolderNode * todayNode = new CDateFolderNode(m_Owner, m_AllUsers, m_Cluster, _T("Today"), today, oneDay);
	//todayNode->InsertBelow(*GetTreeView(), *this);
	std::_tstring fromUTC, toUTC;
	::DateTimeToUTC(today, fromUTC);
	::DateTimeToUTC(today + oneDay, toUTC);

	//delete all the workunits that are under this parent node
	WTL::CTreeItem root( GetRoot(), GetTreeView() );
	for(WTL::CTreeItem curr = root.GetChild(); !curr.IsNull(); )
	{
		CTreeNode *node = reinterpret_cast<CTreeNode *>(curr.GetData());
		if(CComQIPtr<CWorkunitNode> wuNode = node)
		{
			curr = curr.GetNextSibling();
			wuNode->Delete();
		}
		else
			break;
	}

	StlLinked<Dali::IDali> server = Dali::AttachDali(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT), _T("Dali"));
	Dali::IWorkunitVector wus;
	CString user = m_AllUsers ? _T("") : GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_USER);
	if (m_loadingNode)
	{
		m_loadingNode->Delete();
		m_loadingNode = NULL;
	}
	m_loadingNode = new CLoadingNode();
	m_loadingNode->Insert(*GetTreeView(), TVI_ROOT, TVI_FIRST);
	int fetchLimit = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_WORKUNIT_FETCHLIMIT);
	
	Stub tmp;
	tmp.m_orig = this;
	tmp.m_allUsers = m_AllUsers ? 1 : -1;

	server->GetWorkunitsAsync(m_Cluster.c_str(), user, _T(""), fromUTC.c_str(), toUTC.c_str(), fetchLimit, tmp);
}

void CDaliNode::operator()(Dali::IWorkunitVectorAdapt wus)
{
	callback(wus, -1);
}

void CDaliNode::callback(Dali::IWorkunitVectorAdapt wus, int allUsers)
{
	if (m_loadingNode)
	{
		m_loadingNode->Delete();
		m_loadingNode = NULL;
	}
	m_Owner->UpdateWorkunits(GetRoot(), wus, allUsers);
}

void CDaliNode::AddItems(WTL::CTreeViewCtrlEx &tv)
{
	using namespace boost::gregorian;
	m_pTreeView = &tv;
	HTREEITEM hRoot = GetRoot();

	date today = day_clock::local_day();
    date_duration oneDay(1), oneWeek(7);

	UpdateToday();

	CDateFolderNode * newNode;
	date curr = today - oneDay;
	for(; curr.day_of_week() != Saturday; curr -= oneDay)
	{
		newNode = new CDateFolderNode(m_Owner, m_AllUsers, m_Cluster, Weekdays[curr.day_of_week()], curr, oneDay);
		newNode->InsertBelow(tv, hRoot);
	}

	//  Weeks---
	date lastWeekStart = curr - oneWeek + oneDay;	//+1 as curr is at a saturday.
	newNode = new CDateFolderNode(m_Owner, m_AllUsers, m_Cluster, PrevWeeks[0], lastWeekStart, oneWeek);
	newNode->InsertBelow(tv, hRoot);

	curr = lastWeekStart - oneWeek;
	for(unsigned i = 1; (curr + oneWeek).month() == today.month(); curr -= oneWeek, ++i)
	{
		newNode = new CDateFolderNode(m_Owner, m_AllUsers, m_Cluster, PrevWeeks[i], curr, oneWeek);
		newNode->InsertBelow(tv, hRoot);
	}

	//  Months---
	date lastMonthStart(curr.year(), curr.month(), 1);
	newNode = new CDateFolderNode(m_Owner, m_AllUsers, m_Cluster, _T("Last Month"), lastMonthStart, (curr + oneWeek) - lastMonthStart);
	newNode->InsertBelow(tv, hRoot);
	if (lastMonthStart.month() > 1)
	{
		date nextMonth = lastMonthStart;
		curr = date(lastMonthStart.year(), lastMonthStart.month() - 1, 1);
		for(;;)
		{
			newNode = new CDateFolderNode(m_Owner, m_AllUsers, m_Cluster, Months[curr.month()], curr, nextMonth - curr);
			newNode->InsertBelow(tv, hRoot);
			nextMonth = curr;
			if (curr.month() <= 1)
				break;
			curr = date(curr.year(), curr.month() - 1, 1);
		} 
	}

	//  Last Year---
	date lastYearStart(curr.year() - 1, 1, 1);
	std::_tstring yearLabel = today.year() - lastYearStart.year() > 1 ? boost::lexical_cast<std::_tstring>(lastYearStart.year()) : _T("Last Year");
	newNode = new CDateFolderNode(m_Owner, m_AllUsers, m_Cluster, yearLabel, lastYearStart, (curr) - lastYearStart);
	newNode->InsertBelow(tv, hRoot);
	date nextYear = lastYearStart;
	curr = date(lastYearStart.year() - 1, 1, 1);
	while(curr.year() >= 2004)
	{
		yearLabel = boost::lexical_cast<std::_tstring>(curr.year());
		newNode = new CDateFolderNode(m_Owner, m_AllUsers, m_Cluster, yearLabel, curr, nextYear - curr);
		newNode->InsertBelow(tv, hRoot);
		nextYear = curr;
		curr = date(curr.year() - 1, 1, 1);
	} 
}
void CDaliNode::AddItem(WTL::CTreeViewCtrlEx &tv, Dali::IWorkunit * wu)
{
	CWorkunitNode * newNode = new CWorkunitNode(m_Owner, wu);
	newNode->Insert(tv, GetRoot(), TVI_FIRST);
	newNode->operator ()(Dali::WUEventUpdated, wu);
}
void CDaliNode::ItemExpanding()
{
	AddItems(*GetTreeView());
}

const TCHAR * CDaliNode::GetID()
{
	static const TCHAR * const idLabel = _T("HistoryID(*&^%$#@!)");
	return idLabel;
}

const TCHAR * CDaliNode::GetDispText()
{
	static const TCHAR * const rootLabel = _T("History");
	return rootLabel;
}

void CDaliNode::Refresh(bool allUsers, const std::_tstring & cluster)
{
	m_AllUsers = allUsers;
	m_Cluster = cluster.c_str();

	//Today is under the root, not a CDateFolderNode
	UpdateToday();

	//Refresh all the open CDateFolderNodes
	for(WTL::CTreeItem curr = GetChild(); !curr.IsNull(); curr = curr.GetNextSibling())
	{
		CComQIPtr<CDateFolderNode> node = reinterpret_cast<CTreeNode *>(curr.GetData());
		if(node)
			node->Refresh(allUsers, cluster);
	}
}
//  ===============================================================================================
CDateFolderNode::CDateFolderNode(IDaliSlot *owner, bool allUsers, const std::_tstring & cluster, const std::_tstring & label, const boost::gregorian::date &from, const boost::gregorian::date_duration &duration) : CDaliBaseNode(owner, allUsers, cluster), m_label(label), m_from(from), m_duration(duration)
{
	m_id = _T("CDateFolderNode");
	m_id += m_label;
	/*
	date_duration oneDay(1);
	m_label += _T(" (");
	m_label += to_simple_string(m_from).c_str();
	if (m_duration > oneDay)
	{
		m_label += _T(" -> ");
		date endDate = m_from + m_duration - oneDay;
		m_label += to_simple_string(endDate).c_str();
	}
	m_label += _T(")");
	*/
}

HRESULT STDMETHODCALLTYPE CDateFolderNode::QueryInterface(/* [in] */ REFIID riid, /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppUnk)
{
	*ppUnk = NULL;
	if (::InlineIsEqualGUID(riid, IID_IUnknown) ||
		::InlineIsEqualGUID(riid, __uuidof(CDateFolderNode)))
	{
		*ppUnk = (void*)(CDateFolderNode*)this;
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

void CDateFolderNode::AddItems(const std::_tstring &fromUTC, const std::_tstring &toUTC)
{
	//delete all the workunits that are under this parent node
	for(WTL::CTreeItem curr = GetChild(); !curr.IsNull(); )
	{
		CTreeNode *node = reinterpret_cast<CTreeNode *>(curr.GetData());
		CComQIPtr<CWorkunitNode> wuNode = node;
		if(wuNode)
		{
			curr = curr.GetNextSibling();
			wuNode->Delete();
		}
		else
			break;
	}

	//add a dummy node
	if (m_loadingNode)
	{
		m_loadingNode->Delete();
		m_loadingNode = NULL;
	}
	m_loadingNode = new CLoadingNode();
	m_loadingNode->Insert(*GetTreeView(), m_hTreeItem, TVI_FIRST);

	StlLinked<Dali::IDali> server = Dali::AttachDali(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT), _T("Dali"));
	Dali::IWorkunitVector wus;
	CString user = m_AllUsers ? _T("") : GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_USER);
	int fetchLimit = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_WORKUNIT_FETCHLIMIT);
	server->GetWorkunitsAsync(m_Cluster.c_str(), user, _T(""), fromUTC.c_str(), toUTC.c_str(), fetchLimit, boost::ref(*this));
}

void CDateFolderNode::operator()(Dali::IWorkunitVectorAdapt wus)
{
	if (m_loadingNode)
	{
		m_loadingNode->Delete();
		m_loadingNode = NULL;
	}
	m_Owner->UpdateWorkunits(m_hTreeItem, wus);
}

void CDateFolderNode::ItemExpanding()
{
	std::_tstring fromUTC, toUTC;
	DateTimeToUTC(m_from, fromUTC);
	DateTimeToUTC(m_from + m_duration, toUTC);
	AddItems(fromUTC.c_str(), toUTC.c_str());
}

const TCHAR * CDateFolderNode::GetID()
{
	return m_id.c_str();
}

const TCHAR * CDateFolderNode::GetDispText()
{
	return m_label.c_str();
}

bool CDateFolderNode::Contains(Dali::IWorkunit * wu)
{
	boost::gregorian::date l_date(2004, 1, 1);
	boost::posix_time::ptime l_time(l_date, boost::posix_time::hours(1));
	UTCToDateTime(wu->GetDateTime(), l_date, l_time);
	if (l_date >= m_from && l_date < m_from + m_duration)// && time <= (m_from + m_duration))
		return true;
	return false;
}

void CDateFolderNode::Refresh(bool allUsers, const std::_tstring &cluster)
{
	m_AllUsers = allUsers;
	m_Cluster = cluster;
	if (IsExpanded())
	{
		Expand(TVE_COLLAPSE | TVE_COLLAPSERESET);
		Expand();
	}
}
//  ===============================================================================================
CWorkunitNode::CWorkunitNode(IDaliSlot *owner, Dali::IWorkunit * wu) : CDaliBaseNode(owner, true, _T("")), m_wu(wu)
{
	ATLASSERT(m_wu.p);
	m_label = m_wu->GetLabel();
	m_sc = m_wu->on_refresh_connect(boost::ref(*this));
}

CWorkunitNode::~CWorkunitNode()
{
	ATLASSERT(m_wu.p);
	m_sc.disconnect();
}

HRESULT STDMETHODCALLTYPE CWorkunitNode::QueryInterface(/* [in] */ REFIID riid, /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppUnk)
{
	*ppUnk = NULL;
	if (::InlineIsEqualGUID(riid, IID_IUnknown) ||
		::InlineIsEqualGUID(riid, __uuidof(CWorkunitNode)))
	{
		*ppUnk = (void*)(CWorkunitNode*)this;
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

int CWorkunitNode::GetDispChildren()
{
	return 0;
}

int CWorkunitNode::GetDispImage()
{
	if ( !m_wu.p )
		return -1;
	else
	{
		if (m_wu->GetFollowup())
		{
			return m_wu->GetProtected() ? IID_DOCUMENT_FOLLOWUPLOCKED : IID_DOCUMENT_FOLLOWUP;
		}
		else
		{
			return m_wu->GetProtected() ? IID_DOCUMENT_LOCKED : IID_DOCUMENT;
		}
	}
}

int CWorkunitNode::GetDispSelectedImage()
{
	return GetDispImage();
}

Dali::WUState CWorkunitNode::GetState() 
{
	if ( !m_wu.p )
	{
		return Dali::WUStateUnknown;
	}
	return m_wu->GetState();
}

void CWorkunitNode::operator()(Dali::WUEvent evt, Dali::IWorkunit * /*wu*/) 
{
	switch (evt)
	{
	case Dali::WUEventUpdated:
		if ( GetTreeView() )
		{
			if (m_wu.p)
			{
				m_label = m_wu->GetLabel();
				bool bOwner = m_Owner ? m_Owner->ShowUserInLabel(m_wu->GetOwner()) : true;
				bool bCluster = m_Owner ? m_Owner->ShowClusterInLabel(m_wu->GetCluster()) : true;
				if ( bOwner || bCluster )
				{
					std::_tstring owner = m_wu->GetOwner();
					std::_tstring cluster = m_wu->GetCluster();
					if (!owner.empty() || !cluster.empty())
					{
						m_label += _T(" (");
						if (!owner.empty())
						{
							m_label += m_wu->GetOwner();
							if (!cluster.empty())
								m_label += _T("-");
						}
						if (!cluster.empty())
							m_label += m_wu->GetCluster();
						m_label += _T(")");
					}
				}
			}
			SetState(INDEXTOSTATEIMAGEMASK(GetStateIcon(GetState())), TVIS_STATEIMAGEMASK);
		}
		break;
	case Dali::WUEventDeleted:
		Delete();
		break;
	}
}

const TCHAR * const EMPTYSTR = _T("");

const TCHAR * CWorkunitNode::GetID()
{
	if (!m_wu.p)
		return EMPTYSTR;

	return m_wu->GetID();
}

const TCHAR * CWorkunitNode::GetDispText()
{
	return m_label.c_str();
}

Dali::IWorkunit * CWorkunitNode::GetWorkunit()
{
	if (!m_wu.p)
		return NULL;

	return m_wu;
}

void CWorkunitNode::OnDblclk(const WTL::CTreeItem &/*ti*/)
{
	if (!m_wu.p)
		return;

	m_Owner->OnDoubleClick(*m_wu);
}

static const std::_tstring LF(_T("\n"));

const TCHAR * CWorkunitNode::GetToolTip(std::_tstring & result)
{
	if (!m_wu.p)
		return EMPTYSTR;

	result = m_wu->GetWuid();
	result += LF + _T("Status:  ") + m_wu->GetStateLabel();
	result += LF + _T("Cluster:  ") + m_wu->GetCluster();
	result += LF + _T("Created by:  ") + m_wu->GetOwner();
	result += LF + _T("Date:  ") + m_wu->GetDateTime();
	std::_tstring label = m_wu->GetSnapshot();
	if (!label.empty())
		result += LF + _T("Label:  ") + label;
	return result.c_str();
}
//  ===============================================================================================
CSMCNode::CSMCNode(IDaliSlot *owner) : CTreeNode(), m_Owner(owner)
{
	
}

void CSMCNode::ItemExpanding()
{
}

const TCHAR * CSMCNode::GetID()
{
	static const TCHAR * const idLabel = _T("ActiveID(*&^%$#@!)");
	return idLabel;
}

const TCHAR * CSMCNode::GetDispText()
{
	static const TCHAR * const rootLabel = _T("Active");
	return rootLabel;
}

void CSMCNode::Refresh()
{
	Expand(TVE_COLLAPSE | TVE_COLLAPSERESET);
	Expand();
}
//  ===============================================================================================
CSMCClusterNode::CSMCClusterNode(IDaliSlot *owner, SMC::ICluster *cluster) : CTreeNode(), m_Owner(owner), m_cluster(cluster)
{
}

void CSMCClusterNode::ItemExpanding()
{
	CComPtr<Dali::IWorkunit> wu = m_cluster->GetRunning();
	if (wu)
	{
		CWorkunitNode * newNode = new CWorkunitNode(m_Owner, wu);
		newNode->InsertBelow(*GetTreeView(), *this);
		newNode->operator ()(Dali::WUEventUpdated, NULL);
	}
	CComPtr<clib::scoped_lock_ref_counted> lock;
	m_cluster->Lock(lock);
	for(int i = 0; i < m_cluster->GetQueueCount(); ++i)
	{
		wu = m_cluster->GetQueueItem(i);
		if (wu)
		{
			CWorkunitNode * newNode = new CWorkunitNode(m_Owner, wu);
			newNode->InsertBelow(*GetTreeView(), *this);
			newNode->operator ()(Dali::WUEventUpdated, NULL);
		}
	}
}

const TCHAR * CSMCClusterNode::GetID()
{
	return m_cluster->GetID();
}

const TCHAR * CSMCClusterNode::GetDispText()
{
	return m_cluster->GetLabel();
}

int CSMCClusterNode::GetDispChildren()
{
	if (m_cluster->GetRunning())
		return 1;
	return m_cluster->GetQueueCount();
}

void CSMCClusterNode::Refresh()
{
}
//  ===============================================================================================
