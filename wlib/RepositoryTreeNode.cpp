#include "StdAfx.h"
#include "..\en_us\resource.h"

#include ".\RepositoryTreeNode.h"
#include <util.h> //clib
#include <utilDateTime.h> //clib
#include <dali.h> //comms
#include <topology.h> //comms
#include <thread.h>

const TCHAR * const SYNTAXWARNING = _T("Warning");
const TCHAR * const SYNTAXERROR = _T("Error");
const TCHAR * const NULL_STRING = _T("");
static const std::_tstring LF(_T("\n"));

//  ===============================================================================================
CRepositoryBaseNode::CRepositoryBaseNode()
{
	ATLASSERT(false);
}
CRepositoryBaseNode::CRepositoryBaseNode(IRepositorySlot *pOwner) : m_owner(pOwner)
{
}
//  ===============================================================================================
CRepositoryNode::CRepositoryNode()
{
	ATLASSERT(false);
}
CRepositoryNode::CRepositoryNode(IRepositorySlot *pOwner, bool bVirtualNode) : CRepositoryBaseNode(pOwner), m_bVirtualNode(bVirtualNode)
{
}

CRepositoryNode::~CRepositoryNode()
{
}

void CRepositoryNode::SetLoadingNode(CLoadingNode * loading)
{
	m_loading = loading;
}

HTREEITEM CRepositoryNode::InsertBelow(WTL::CTreeViewCtrlEx &tc, HTREEITEM parent, bool bLast)
{
	if (!m_bVirtualNode)
		return CRepositoryBaseNode::InsertBelow(tc, parent, bLast);

	m_pTreeView = &tc;
	return NULL;
}

BOOL CRepositoryNode::Expand(UINT nCode)
{
	if (!m_bVirtualNode)
		return CRepositoryBaseNode::Expand(nCode);

	if ((nCode & TVE_COLLAPSE) == TVE_COLLAPSE)
		GetTreeView()->DeleteAllItems();
	else if (GetTreeView()->GetCount() == 0)
		ItemExpanding();
	return true;
}

void CRepositoryNode::ItemExpanding()
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	if (!m_loading)
		m_loading = new CLoadingNode();
	m_loading->InsertBelow(*GetTreeView(), m_bVirtualNode ? TVI_ROOT : *this);
	clib::thread run(__FUNCTION__, boost::bind(&GetModules, this, m_loading));
}

void CRepositoryNode::GetModules(CComPtr<CRepositoryNode> self, CComPtr<CLoadingNode> loading)
{
	self->m_owner->OnLoading();
	CComPtr<IRepository> rep = self->GetRepository();
	IModuleVector modules;
	rep->GetModules(_T(""), modules, false, false);
	self->m_owner->OnLoaded(modules);
	self->LoadModules(modules, loading);
}

void CRepositoryNode::LoadModules(IModuleVector & modules, CLoadingNode * loading)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	if (m_loading == loading)
	{
		m_loading->Delete();
		m_loading = NULL;
		WTL::CLockWindowUpdate lock(*GetTreeView());
		for (IModuleVector::iterator itr = modules.begin(); itr != modules.end(); ++itr)
		{
			CModuleNode * newNode = new CModuleNode(m_owner, itr->get());
			newNode->InsertBelow(*GetTreeView(), m_bVirtualNode ? TVI_ROOT : *this);
			newNode->operator ()(itr->get(), REFRESH_MODULE_UNKNOWN);
		}
	}
}

const TCHAR * CRepositoryNode::GetID()
{
	CComPtr<IRepository> rep = m_owner->GetRepository();
	if ( rep.p )
		return rep->GetID();
	else
		return _T("?");
}

const TCHAR * CRepositoryNode::GetDispText()
{
	CComPtr<IRepository> rep = m_owner->GetRepository();
	if ( rep.p )
		return rep->GetLabel();
	else
		return _T("?");
}

IRepository * CRepositoryNode::GetRepository()
{
	return m_owner->GetRepository();
}

//  ===============================================================================================
CModuleNode::CModuleNode()
{
	ATLASSERT(false);
}
CModuleNode::CModuleNode(IRepositorySlot *pOwner, IModule * mod) : CRepositoryBaseNode(pOwner), m_module(mod)
{
	ATLASSERT(mod);
	m_sigConn = m_module->on_refresh_connect(boost::ref(*this));
	m_nextExpandShouldBeFast = false;
}
CModuleNode::~CModuleNode()
{
	m_sigConn.disconnect();  //  TODO Should not be needed...
}
void CModuleNode::OnDblclk(const WTL::CTreeItem & /*ti*/)
{
	m_owner->OnDoubleClick(*m_module);
}

int CModuleNode::GetDispChildren()
{
	return m_module->HasChildren();
}

ATTRSTATE CModuleNode::GetState()
{
	switch (m_module->GetAccess())
	{
	case SecAccess_Write:
	case SecAccess_Full:
		return ATTRSTATE_NONE;
	}
	return ATTRSTATE_LOCKED;
}
void CModuleNode::operator()(IModule * /*module*/, REFRESH_MODULE refreshType)
{
	if (GetTreeView() && GetTreeView()->IsWindow())
	{
		AddRef();
		GetTreeView()->GetParent().PostMessage(RVUM_REFRESHMODULENODE, (WPARAM)this, refreshType);
	}
}

void CModuleNode::ItemExpanding()
{
	IModuleVector modules;
	IAttributeVector attributes;
	m_owner->OnLoading();
	m_module->GetModules(modules);
	if (m_nextExpandShouldBeFast)
	{
		m_module->GetAttributes(attributes, true);
		m_nextExpandShouldBeFast = false;
	}
	else
		m_module->GetAttributes(attributes);
	m_owner->OnLoaded(m_module, attributes);

	CLoadingNode * loadingNode = new CLoadingNode();
	loadingNode->Insert(*GetTreeView(), m_hTreeItem, TVI_FIRST);
	bool hasChildren = false;

	WTL::CLockWindowUpdate lock(*GetTreeView());
	for (IModuleVector::iterator itr = modules.begin(); itr != modules.end(); ++itr)
	{
		CModuleNode * newNode = new CModuleNode(m_owner, itr->get());
		newNode->InsertBelow(*GetTreeView(), *this);
		newNode->operator ()(newNode->GetModule(), REFRESH_MODULE_UNKNOWN);
		hasChildren = true;
	}
	for (IAttributeVector::iterator itr = attributes.begin(); itr != attributes.end(); ++itr)
	{
		CAttributeNode * newNode = new CAttributeNode(m_owner, itr->get());
		newNode->InsertBelow(*GetTreeView(), *this);
		newNode->operator ()(newNode->GetAttribute(), false, NULL, false);
		hasChildren = true;
	}
	if (hasChildren)
	{
		loadingNode->Delete();
	}
	else
	{
		loadingNode->SetEmpty();
	}
}

const TCHAR * CModuleNode::GetID()
{
	return m_module->GetID();
}

const TCHAR * CModuleNode::GetDispText()
{
	if (!m_module->Exists())
		return NULL_STRING;

	return m_module->GetLabel();
}

int CModuleNode::GetDispImage()
{
	if (!m_module->Exists())
		return IID_FOLDER_PLACEHOLDER;

	if (m_module->IsPlugin())
	{
		return IID_PLUGINFOLDER_CLOSED;
	}
	else if (m_module->IsTrash())
	{
		if (GetDispChildren())
		{
			return IID_TRASH_FULL;
		}
		return IID_TRASH_EMPTY;
	}
	return __super::GetDispImage();
}

int CModuleNode::GetDispSelectedImage()
{
	if (!m_module->Exists())
		return IID_FOLDER_PLACEHOLDER;

	if (m_module->IsPlugin())
	{
		return IID_PLUGINFOLDER_CLOSED;
	}
	else if (m_module->IsTrash())
	{
		if (GetDispChildren())
		{
			return IID_TRASH_FULL;
		}
		return IID_TRASH_EMPTY;
	}
	return __super::GetDispSelectedImage();
}

IModule * CModuleNode::GetModule()
{
	return m_module;
}

void CModuleNode::SetNextExpandShouldBeFast()
{
	m_nextExpandShouldBeFast = true;
}
//  ===============================================================================================
CAttributeNode::CAttributeNode()
{
	m_syntaxCheck = SYNTAX_CHECK_UNKNOWN;
	ATLASSERT(false);
}
CAttributeNode::CAttributeNode(IRepositorySlot *pOwner, IAttribute * attr, bool displayQualified) : CRepositoryBaseNode(pOwner), m_attribute(attr), m_displayQualified(displayQualified)
{
	m_syntaxCheck = SYNTAX_CHECK_UNKNOWN;
	m_sigConn = m_attribute->on_refresh_connect(boost::ref(*this));
	GenerateDispText();
}

CAttributeNode::~CAttributeNode()
{
	m_sigConn.disconnect();  //  TODO Should not be needed...
}

void CAttributeNode::GenerateDispText()
{
	m_DispText = m_displayQualified ? m_attribute->GetQualifiedLabel() : m_attribute->GetLabel();
	m_DispText += m_attribute->GetType()->GetFileExtension();
	std::_tstring lockedBy = m_attribute->GetLockedBy();
	if (lockedBy.length())
	{
		m_DispText += _T(" (");
		m_DispText += lockedBy;
		m_DispText += _T(")");
	}
}
const TCHAR * CAttributeNode::GetID()
{
	return m_attribute->GetID();
}
const TCHAR * CAttributeNode::GetDispText()
{
	if (!m_attribute->Exists())
		return NULL_STRING;

	return m_DispText.c_str();
}

int CAttributeNode::GetDispChildren()
{
	return 0;
}

int CAttributeNode::GetDispImage()
{
	if (!m_attribute->Exists())
		return IID_DOCUMENT_PLACEHOLDER;

	switch(m_syntaxCheck)
	{
	case SYNTAX_CHECK_PENDING:
		return IID_DOCUMENT_SYNTAXPENDING;
	case SYNTAX_CHECK_PASS:
		return IID_DOCUMENT_SYNTAXPASS;
	case SYNTAX_CHECK_WARN:
		return IID_DOCUMENT_SYNTAXWARN;
	case SYNTAX_CHECK_FAIL:
		return IID_DOCUMENT_SYNTAXFAIL;
	}
	return IID_DOCUMENT;
}

int CAttributeNode::GetDispSelectedImage()
{
	return GetDispImage();
}

const TCHAR * CAttributeNode::GetToolTip(std::_tstring & result)
{
	result = m_attribute->GetQualifiedLabel();
	if (m_attribute->GetState() != ATTRSTATE_NONE)
	{
		result += LF + m_attribute->GetStateLabel();
		if (m_attribute->IsLocked())
		{
			result += std::_tstring(_T(":  ")) + m_attribute->GetLockedBy();
		}
	}
	result += LF + _T("Modified by:  ") + m_attribute->GetModifiedBy();
	result += LF + _T("Modified date:  ") + m_attribute->GetModifiedDate();
	result += LF + _T("Type:  ") + m_attribute->GetType()->GetDescription();
	if (m_syntaxSummary.length())
	{
		result += LF + m_syntaxSummary.c_str();
	}
	return result.c_str();
}

void CAttributeNode::OnDblclk(const WTL::CTreeItem & /*ti*/)
{
	m_owner->OnDoubleClick(*m_attribute);
}

IAttribute * CAttributeNode::GetAttribute()
{
	return m_attribute;
}

const TCHAR *CAttributeNode::GetText(bool /*bRefresh*/)
{
	return m_attribute->GetText();
}

void CAttributeNode::SetSyntaxState(SYNTAX_CHECK state)
{
	m_syntaxCheck = state;
}

const TCHAR *GetSyntaxTargetCluster(std::_tstring & targetCluster)
{
	StlLinked<Topology::ITopology> server = AttachTopology(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_TOPOLOGY), _T("Topology"));
	Topology::IClusterVector clusters;
	server->GetClusters(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_QUEUE), clusters);
	targetCluster = _T("");
	for (Topology::IClusterVector::const_iterator itr = clusters.begin(); itr != clusters.end(); ++itr) {
		if (targetCluster.empty()) {
			//  Default to first cluster  ---
			targetCluster = itr->get()->GetName();
		}
		if (boost::algorithm::iequals(itr->get()->GetName(), _T("hthor")) || boost::algorithm::iequals(itr->get()->GetType(), _T("hthor"))) {
			//  Prefer hthor cluster  ---
			//  TODO:  Retest when HPCC-13787 is resolved  ---
			targetCluster = itr->get()->GetName();
			break;
		} else if (boost::algorithm::icontains(itr->get()->GetName(), _T("hthor"))) {
			targetCluster = itr->get()->GetName();
			//  Continue in case we find an actual hthor  ---
		}
	}
	return targetCluster.c_str();
}

SYNTAX_CHECK CAttributeNode::CheckSyntax()
{
	std::_tstring targetCluster;
	Dali::CEclExceptionVector errors;
	StlLinked<Dali::IDali> dali = Dali::AttachDali(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT), _T("Dali"));
	if (dali->CheckSyntax(GetSyntaxTargetCluster(targetCluster), GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_QUEUE), m_attribute->GetModuleQualifiedLabel(), m_attribute->GetLabel(), _T(""), m_attribute->GetText(), -1, _T(""), false, false, errors))
	{
		m_syntaxCheck = SYNTAX_CHECK_PASS;
		m_syntaxSummary.clear();
		for(Dali::CEclExceptionVector::iterator itr = errors.begin(); itr != errors.end(); ++itr)
		{	
			if (itr->get()->m_severity.CompareNoCase(SYNTAXWARNING) == 0)
			{
				m_syntaxCheck = SYNTAX_CHECK_WARN;	
				if (m_syntaxSummary.length())
				{
					m_syntaxSummary += LF;
				}
				m_syntaxSummary += itr->get()->m_severity + _T(":  ") + itr->get()->m_message;
			}
			else if (itr->get()->m_severity.CompareNoCase(SYNTAXERROR) == 0)
			{
				m_syntaxCheck = SYNTAX_CHECK_FAIL;	
				m_syntaxSummary = itr->get()->m_severity + _T(":  ") + itr->get()->m_message;
				break;
			}
		}
	}
	return m_syntaxCheck;
}

ATTRSTATE CAttributeNode::GetState() 
{
	return m_attribute->GetState();
}

void CAttributeNode::operator()(IAttribute * /*attr*/, bool eclChanged, IAttribute * newAttrAsOldOneMoved, bool deleted) 
{
	if (GetTreeView())
	{
		if (newAttrAsOldOneMoved || deleted)
			Delete();
		else
		{
			GenerateDispText();
			SetState(-1, TVIS_STATEIMAGEMASK);
			SetState(INDEXTOSTATEIMAGEMASK(GetStateIcon(GetState())), TVIS_STATEIMAGEMASK);
		}
	}
}

void CAttributeNode::operator()(ATTRSTATE state)
{
	if (GetTreeView())
	{
		SetState(-1, TVIS_STATEIMAGEMASK);
		SetState(INDEXTOSTATEIMAGEMASK(GetStateIcon(state)), TVIS_STATEIMAGEMASK);
	}
}

void CAttributeNode::Refresh()
{
	CComPtr<IRepository> rep = AttachRepository();
	m_sigConn.disconnect();
	CComPtr<IAttribute> attr = rep->GetAttribute(m_attribute->GetQualifiedLabel(), m_attribute->GetType());
	m_sigConn = m_attribute->on_refresh_connect(boost::ref(*this));
	operator ()(m_attribute, false, NULL, false);
	if (IsExpanded())
	{
		Expand(TVE_COLLAPSE | TVE_COLLAPSERESET);
		Expand();
	}
}

bool CAttributeNode::CanEdit()
{
	return true;
}
//  ===============================================================================================
CRepositoryFilterNode::CRepositoryFilterNode()
{
	ATLASSERT(false);
}
CRepositoryFilterNode::CRepositoryFilterNode(IRepositorySlot *pOwner, const std::_tstring & label, IAttributeVectorPtr *results) : CRepositoryNode(pOwner, false), m_results(results)
{
}

void CRepositoryFilterNode::ItemExpanding()
{
	CComPtr<IRepository> rep = m_owner->GetRepository();
	IModuleVector modules;
	rep->GetAllModules(modules, false, true, true);
	for(IAttributeVector::iterator itr = m_results->begin(); itr != m_results->end(); ++itr)
	{
		std::_tstring module = itr->get()->GetModuleQualifiedLabel();
		if (m_modules.find(module) == m_modules.end())
		{
			IModule * found = Find(&modules, module);
			ATLASSERT(found);
			m_modules[module] = new CModuleFilterNode(m_owner, found);
			ATLASSERT(m_modules[module].get());
		}
		m_modules[module]->AddAttribute(itr->get());
	}

	for(IModuleVector::iterator itr = modules.begin(); itr != modules.end(); ++itr)
	{
		if (m_modules.find(itr->get()->GetQualifiedLabel()) != m_modules.end())
		{
			m_modules[itr->get()->GetQualifiedLabel()]->InsertBelow(*GetTreeView(), *this);
			m_modules[itr->get()->GetQualifiedLabel()]->Expand();
		}
	}
}
//  ===============================================================================================
CModuleFilterNode::CModuleFilterNode(IRepositorySlot *owner, IModule * mod) : CModuleNode(owner, mod)
{
}

void CModuleFilterNode::AddAttribute(IAttribute *attr)
{
	m_attrs.push_back(attr);
}
void CModuleFilterNode::ItemExpanding()
{
	for(IAttributeVector::iterator itr = m_attrs.begin(); itr != m_attrs.end(); ++itr)
	{
		CAttributeNode * newNode = new CAttributeNode(m_owner, itr->get());
		newNode->InsertBelow(*GetTreeView(), *this);
		newNode->operator ()(newNode->GetAttribute(), false, NULL, false);
	}
}
const TCHAR * CModuleFilterNode::GetDispText()
{
	return m_module->GetQualifiedLabel();
}
//=============================================================================
CAttributeHistoryNode::CAttributeHistoryNode() 
{
	m_syntaxCheck = SYNTAX_CHECK_UNKNOWN;
	ATLASSERT(false);
	m_singleDisplayMode = false;
}

CAttributeHistoryNode::CAttributeHistoryNode(IRepositorySlot *pOwner, IAttributeHistory * history, bool singleDisplayMode) : CRepositoryBaseNode(pOwner), m_attributeHistory(history)
{
	m_syntaxCheck = SYNTAX_CHECK_UNKNOWN;
	//A single history is likely to change?
	//m_attributeHistory->on_refresh_connect(boost::ref(*this));
	m_singleDisplayMode = singleDisplayMode;
	GenerateDispText();
}

void CAttributeHistoryNode::GenerateDispText()
{
	CString t;
	try
	{
		std::_tstring dateTime = m_attributeHistory->GetModifiedDate();
		if (!dateTime.empty())
		{
			SYSTEMTIME dt;
			::UTCToSystemTime(dateTime, dt, true);
			t.Format(_T("%d-%02d-%02d, %02d:%02d:%02d"), dt.wYear, dt.wMonth, dt.wDay, dt.wHour, dt.wMinute, dt.wSecond);
		}
	}
	catch(boost::bad_lexical_cast)
	{
		ATLTRACE(_T("GenerateDispText failed on UTC(%s)\n"), m_attributeHistory->GetModifiedDate());
		t = m_attributeHistory->GetModifiedDate();
	}

	CString modBy = m_attributeHistory->GetModifiedBy();
	m_DispText = _T("");
	if (m_singleDisplayMode)
		m_DispText = m_attributeHistory->GetQualifiedLabel();

	if (!t.IsEmpty() || !modBy.IsEmpty())
	{
		m_DispText += _T(" (");
		m_DispText += t;
		if (!modBy.IsEmpty())
		{
			m_DispText += _T(" - ");
			m_DispText += modBy;
		}
		m_DispText += _T(")");
	}
}

const TCHAR * CAttributeHistoryNode::GetID()
{
	return m_attributeHistory->GetID();
}

const TCHAR * CAttributeHistoryNode::GetDispText()
{
	return m_DispText.c_str();
}

int CAttributeHistoryNode::GetDispChildren()
{
	return 0;
}

int CAttributeHistoryNode::GetDispImage()
{
	switch(m_syntaxCheck)
	{
	case SYNTAX_CHECK_PENDING:
		return IID_DOCUMENT_SYNTAXPENDING;
	case SYNTAX_CHECK_PASS:
		return IID_DOCUMENT_SYNTAXPASS;
	case SYNTAX_CHECK_FAIL:
		return IID_DOCUMENT_SYNTAXFAIL;
	}
	return IID_DOCUMENT_HISTORY;
}

int CAttributeHistoryNode::GetDispSelectedImage()
{
	return GetDispImage();
}

void CAttributeHistoryNode::SetSyntaxState(SYNTAX_CHECK state)
{
	m_syntaxCheck = state;
}

SYNTAX_CHECK CAttributeHistoryNode::GetSyntaxState() const
{
	return m_syntaxCheck;
}

SYNTAX_CHECK CAttributeHistoryNode::CheckSyntax()
{
	std::_tstring targetCluster;
	m_errors.clear();
	StlLinked<Dali::IDali> dali = Dali::AttachDali(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT), _T("Dali"));
	if (dali->CheckSyntax(GetSyntaxTargetCluster(targetCluster), GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_QUEUE), m_attributeHistory->GetAttribute()->GetModuleQualifiedLabel(), m_attributeHistory->GetAttribute()->GetLabel(), _T(""), m_attributeHistory->GetText(), -1, _T(""), false, false, m_errors))
	{
		m_syntaxCheck = SYNTAX_CHECK_PASS;
		for(Dali::CEclExceptionVector::iterator itr = m_errors.begin(); itr != m_errors.end(); ++itr)
		{	
			if (itr->get()->m_severity)
			{
				m_syntaxCheck = SYNTAX_CHECK_FAIL;
				break;
			}
		}
	}
	return m_syntaxCheck;
}

void CAttributeHistoryNode::OnDblclk(const WTL::CTreeItem & /*ti*/)
{
	m_owner->OnDoubleClick(*m_attributeHistory, &m_errors);
}

void CAttributeHistoryNode::operator()(IAttribute * /*attr*/, bool eclChanged, IAttribute * newAttrAsOldOneMoved, bool deleted) 
{
	if (GetTreeView())
	{
		GenerateDispText();
		bool isCurrentVersion = (m_attributeHistory->GetVersion() == m_attributeHistory->GetAttribute()->GetVersion());
		SetState(-1, TVIS_STATEIMAGEMASK);
		SetState(INDEXTOSTATEIMAGEMASK(GetStateIcon(isCurrentVersion ? m_attributeHistory->GetAttribute()->GetState() : ATTRSTATE_NONE)), TVIS_STATEIMAGEMASK);
	}

	//TODO handle renamed and deleted.
	ATLASSERT(!newAttrAsOldOneMoved && !deleted);

	//CString modfiedDate(m_attributeHistory->GetModifiedDate());
	//if( modfiedDate.IsEmpty() )
	//{
	//	SetState(INDEXTOSTATEIMAGEMASK(ATTRSTATE_NONE), TVIS_STATEIMAGEMASK);
	//}
	//GenerateDispText();
}

void CAttributeHistoryNode::Refresh()
{
	GenerateDispText();
	WTL::CTreeItem parent = GetParent();
	parent.Expand(TVE_COLLAPSE | TVE_COLLAPSERESET);
	parent.Expand();
}

const TCHAR *CAttributeHistoryNode::GetText()
{
	return m_attributeHistory->GetText();
}
//  ===========================================================================
CAttributeHistoryNodePlaceHolder::CAttributeHistoryNodePlaceHolder(IRepositorySlot *pOwner, IAttributeHistory * hist, bool singleMode) : CAttributeHistoryNode(pOwner,hist,singleMode)
{
}

const TCHAR * CAttributeHistoryNodePlaceHolder::GetID() 
{ 
	return __super::GetID();
}
const TCHAR * CAttributeHistoryNodePlaceHolder::GetDispText() 
{ 
	return _T(""); 
}
