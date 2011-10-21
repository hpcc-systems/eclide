#include "StdAfx.h"
#include "..\en_us\resource.h"

#include "RepositoryCompareView.h"
#include "thread.h"
#include "LogView.h"
#include "AttrListDlg.h"
#include "Status.h"
#include "Util.h"
#include "Logger.h"
#include "MainFrm2.h"

#define MAX_THREAD_COUNT 2
//  ===========================================================================
class NoCaseCompare
{
public:
	bool operator ()(const std::_tstring & l, const std::_tstring & r) const
	{
		return _tcsicmp(l.c_str(), r.c_str()) < 0;
	}
};
//  ===========================================================================
typedef std::pair<IWorkspaceAdapt, IWorkspaceAdapt> WsWsPair;
class WsWsPairCompare
{
protected:
	IWorkspaceCompare m_wsCompare;

public:
	bool operator ()(WsWsPair & l, WsWsPair & r)
	{
		IWorkspaceAdapt _l = l.first.get();
		IWorkspaceAdapt _r = r.first.get();
		return m_wsCompare(_l, _r);
	}
};
typedef std::vector<WsWsPair> WsWsPairVector;
typedef std::map<std::_tstring, WsWsPair, NoCaseCompare> LabelWsWsPairMap;
//  ===========================================================================
typedef std::pair<IModuleAdapt, IModuleAdapt> ModModPair;
class ModModPairCompare
{
protected:
	IModuleCompare m_modCompare;

public:
	bool operator ()(ModModPair & l, ModModPair & r)
	{
		IModuleAdapt _l = l.first.get();
		IModuleAdapt _r = r.first.get();
		return m_modCompare(_l, _r);
	}
};
typedef std::vector<ModModPair> ModModPairVector;
typedef std::map<std::_tstring, ModModPair, NoCaseCompare> LabelModModPairMap;
//  ===========================================================================
typedef std::pair<IAttributeAdapt, IAttributeAdapt> AttrAttrPair;
class AttrAttrPairCompare
{
protected:
	IAttributeCompare m_attrCompare;

public:
	bool operator ()(AttrAttrPair & l, AttrAttrPair & r)
	{
		IAttributeAdapt _l = l.first.get();
		IAttributeAdapt _r = r.first.get();
		return m_attrCompare(_l, _r);
	}
};
typedef std::vector<AttrAttrPair> AttrAttrPairVector;
struct STRUCT_ATTRIBUTESLOADED
{
	WTL::CTreeItem treeItem;
	IAttributeVector m_lhs;
	IAttributeVector m_rhs;
	ModModPairVector m_modFiltered;
	AttrAttrPairVector m_filtered;
};
//  ===========================================================================
CRepositoryCompareView::CRepositoryCompareView() : 
		m_sel(this), 
		m_filter(AMT_SHOW_MATCHING | AMT_SHOW_NOT_MATCHING | AMT_SHOW_ORPHANS), 
		m_loadingModules(0), 
		m_selectThreads(5),
		m_expandThreads(5)

{
//	SetThemeClassList(L"button");
}

BOOL CRepositoryCompareView::PreTranslateMessage(MSG* pMsg)
{
	pMsg;
	return FALSE;
}

void CRepositoryCompareView::CreateData()
{
	for (int i = 0; i < 55; ++i)
	{
		CString txt = _T("Root");
		txt.Format(_T("Root - %i - %i"), i, i % 2);
		WTL::CTreeItem item = InsertItem(txt, TVI_ROOT, TVI_LAST);
		//SetCheckThreeState(item, (THREESTATE)((i%(int)THREESTATE_LAST)));
		for (int i = 0; i < 15; ++i)
		{
			CString txt;
			txt.Format(_T("Child - %i - %i"), i, i % 2);
			WTL::CTreeItem item2 = InsertItem(txt, item, TVI_LAST);
			//SetCheckThreeState(item2, (THREESTATE)((i%(int)THREESTATE_LAST)));
		}
	}
}

void CRepositoryCompareView::Init(IRepository * repLHS, IRepository * repRHS, const STRUCT_DALI & dali)
{
	m_repLHS = repLHS;
	m_repRHS = repRHS;

	m_daliLHS = dali;

	m_migrator = CreateIMigration(m_repRHS, this);

#ifndef _XXXDEBUG
	Load(NULL, false);
#endif
}

void CRepositoryCompareView::Kill()
{
	m_repLHS = NULL;
	m_repRHS = NULL;
	m_daliLHS.Clear();

	Clear();
	m_sel.Clear();
}

void thread_GetWorkspaces(IRepository * rep, IWorkspaceVector * workspaces)
{
	boost::filesystem::path envFolder;
	rep->GetEnvironmentFolder(envFolder);
	LogMsg((boost::_tformat(_T("thread_GetWorkspaces - %1% - Start")) % envFolder.native_file_string().c_str()).str());
	ATLASSERT(workspaces->size() == 0);
	rep->GetWorkspaces(workspaces, true);
	LogMsg((boost::_tformat(_T("thread_GetWorkspaces - %1% - End")) % envFolder.native_file_string().c_str()).str());
}

void thread_GetModules(IRepository * rep, std::_tstring parentLabel, IModuleVector * modules, bool checksum, bool noRefresh)
{
	LogMsg((boost::_tformat(_T("thread_GetModules - %1%(%2%) - Start")) % rep->GetLabel() % rep->GetRepositoryLabel()).str());
	ATLASSERT(modules->size() == 0);
	rep->GetModules(parentLabel.c_str(), *modules, checksum, noRefresh);
	LogMsg((boost::_tformat(_T("thread_GetModules - %1%(%2%) - End")) % rep->GetLabel() % rep->GetRepositoryLabel()).str());
}

#ifdef _SUPPORT_WORKSPACE
void loadWorkspaces(CRepositoryCompareView * self, WsWsPairVector & sortedWorkspaces, bool noRefresh)
{
	ATLASSERT(self->m_repLHS);
	ATLASSERT(self->m_repRHS);
	if (::InterlockedIncrement(&self->m_loadingModules) == 1)//We could get several "quick" requests, no point in refreshing screen for all
	{
		LogMsg(_T("Loading Workspaces - Start"));
		IWorkspaceVector lhsWorkspaces;
		clib::thread lhsWsRun(__FUNCTION__, boost::bind(&thread_GetWorkspaces, self->m_repLHS, &lhsWorkspaces));
		IWorkspaceVector rhsWorkspaces;
		clib::thread rhsWsRun(__FUNCTION__, boost::bind(&thread_GetWorkspaces, self->m_repRHS, &rhsWorkspaces));
		lhsWsRun.join();
		rhsWsRun.join();
		LogMsg(_T("Loading Workspaces - End"));

		LogMsg((boost::_tformat(_T("Merging Workspaces - Before:  %1% + %2% = %3% workspaces")) % lhsWorkspaces.size() % rhsWorkspaces.size() % (lhsWorkspaces.size() + rhsWorkspaces.size())).str());
		LabelWsWsPairMap mergedWss;
		for(IWorkspaceVector::const_iterator itr = lhsWorkspaces.begin(); itr != lhsWorkspaces.end(); ++itr)
		{
			mergedWss[itr->get()->GetLabel()].first = *itr;
		}
		for(IWorkspaceVector::const_iterator itr = rhsWorkspaces.begin(); itr != rhsWorkspaces.end(); ++itr)
		{
			mergedWss[itr->get()->GetLabel()].second = *itr;
		}

		for(LabelWsWsPairMap::const_iterator itr = mergedWss.begin(); itr != mergedWss.end(); ++itr)
		{
			CComPtr<IWorkspace> lhs = itr->second.first;
			CComPtr<IWorkspace> rhs = itr->second.second;
			if (!lhs)
				lhs = self->m_repLHS->CreateIWorkspace(itr->first.c_str(), true, true);
			if (!rhs)
				rhs = self->m_repRHS->CreateIWorkspace(itr->first.c_str(), true, true);
			if (CanShow(self->m_filter, lhs, rhs))
				sortedWorkspaces.push_back(std::make_pair(lhs, rhs));
		}
		LogMsg((boost::_tformat(_T("Merging Workspaces - After:  %1% Workspaces")) % mergedWss.size()).str());

		LogMsg((boost::_tformat(_T("Sorting Workspaces - Before:  %1%")) % sortedWorkspaces.size()).str());
		std::sort(sortedWorkspaces.begin(), sortedWorkspaces.end(), WsWsPairCompare());
		LogMsg((boost::_tformat(_T("Sorting Workspaces - After:  %1%")) % sortedWorkspaces.size()).str());
	}
	::InterlockedDecrement(&self->m_loadingModules);
}
#endif

void loadModules(CRepositoryCompareView * self, const std::_tstring & parentLabel, ModModPairVector & sortedModules, bool noRefresh)
{
	ATLASSERT(self->m_repLHS);
	ATLASSERT(self->m_repRHS);
	if (::InterlockedIncrement(&self->m_loadingModules) == 1)//We could get several "quick" requests, no point in refreshing screen for all
	{
		LogMsg(_T("Loading Folders - Start"));
		IModuleVector lhsModules;
		clib::thread lhsRun(__FUNCTION__, boost::bind(&thread_GetModules, self->m_repLHS, parentLabel, &lhsModules, true, noRefresh));
		IModuleVector rhsModules;
		clib::thread rhsRun(__FUNCTION__, boost::bind(&thread_GetModules, self->m_repRHS, parentLabel, &rhsModules, true, noRefresh));
		lhsRun.join();
		rhsRun.join();
		LogMsg(_T("Loading Folders - End"));

		LogMsg((boost::_tformat(_T("Merging Folders - Before:  %1% + %2% = %3% folders")) % lhsModules.size() % rhsModules.size() % (lhsModules.size() + rhsModules.size())).str());
		LabelModModPairMap mergedMods;
		for(IModuleVector::const_iterator itr = lhsModules.begin(); itr != lhsModules.end(); ++itr)
		{
			mergedMods[itr->get()->GetQualifiedLabel(true)].first = *itr;
		}
		for(IModuleVector::const_iterator itr = rhsModules.begin(); itr != rhsModules.end(); ++itr)
		{
			mergedMods[itr->get()->GetQualifiedLabel(true)].second = *itr;
		}

		for(LabelModModPairMap::const_iterator itr = mergedMods.begin(); itr != mergedMods.end(); ++itr)
		{
			CComPtr<IModule> lhs = itr->second.first;
			CComPtr<IModule> rhs = itr->second.second;
			if (!lhs)
				lhs = self->m_repLHS->GetModulePlaceholder(itr->first.c_str());
			if (!rhs)
				rhs = self->m_repRHS->GetModulePlaceholder(itr->first.c_str());
			ATLASSERT(lhs && rhs);
			if (lhs->GetRepository() != self->m_repLHS.p || rhs->GetRepository() != self->m_repRHS.p)
			{
				ATLASSERT(false);
				return;
			}
			if (!lhs->IsPlugin() && !rhs->IsPlugin() && !lhs->IsTrash() && !rhs->IsTrash())
			{
				if (CanShow(self->m_filter, lhs, rhs))
					sortedModules.push_back(std::make_pair(lhs, rhs));
			}
		}
		LogMsg((boost::_tformat(_T("Merging Folders - After:  %1% folders")) % mergedMods.size()).str());

		LogMsg((boost::_tformat(_T("Sorting Folders - Before:  %1%")) % sortedModules.size()).str());
		std::sort(sortedModules.begin(), sortedModules.end(), ModModPairCompare());
		LogMsg((boost::_tformat(_T("Sorting Folders - After:  %1%")) % sortedModules.size()).str());
	}
	::InterlockedDecrement(&self->m_loadingModules);
}

void thread_LoadWorkspacesAndModules(CRepositoryCompareView * self, bool noRefresh)
{
	WsWsPairVector sortedWorkspaces;
	ModModPairVector sortedModules;
#ifdef _SUPPORT_WORKSPACE
	loadWorkspaces(self, sortedWorkspaces, noRefresh);
#endif
	loadModules(self, _T(""), sortedModules, noRefresh);
	self->SendMessage(UM_MODULESLOADED, (WPARAM)&sortedWorkspaces, (LPARAM)&sortedModules);
}

void thread_GetAttributes(IModule * module, IAttributeVector * attrs)
{
	LogMsg((boost::_tformat(_T("thread_GetAttributes - %1%(%2%) - Start")) % module->GetRepository()->GetLabel() % module->GetQualifiedLabel()).str());
	ATLASSERT(attrs->size() == 0);
	module->GetAttributes(*attrs);
	LogMsg((boost::_tformat(_T("thread_GetAttributes - %1%(%2%) - End")) % module->GetRepository()->GetLabel() % module->GetQualifiedLabel()).str());
}

#ifdef _SUPPORT_WORKSPACE
void thread_GetWsAttributes(IWorkspace * workspace, IAttributeVector * attrs)
{
	LogMsg((boost::_tformat(_T("thread_GetWsAttributes - %1% - Start")) % workspace->GetLabel()).str());
	ATLASSERT(attrs->size() == 0);
	IWorkspaceItemVector workspaceItems;
	workspace->GetWindows(workspaceItems);
	for(IWorkspaceItemVector::const_iterator itr = workspaceItems.begin();itr != workspaceItems.end(); ++itr)
	{
		if (itr->get()->GetType() == WORKSPACE_ITEM_ATTRIBUTE)
			attrs->push_back(itr->get()->GetAttribute());
	}
	LogMsg((boost::_tformat(_T("thread_GetWsAttributes - %1% - End")) % workspace->GetLabel()).str());
}

void loadWsAttributes(CRepositoryCompareView * self, CWorkspacePairNode * ws, int filter, STRUCT_ATTRIBUTESLOADED & info)
{
	LogMsg((boost::_tformat(_T("Loading Files (%1%) - Start")) % ws->GetDispText()).str());
	info.treeItem = *ws;
	clib::thread lhsRun(__FUNCTION__, boost::bind(&thread_GetWsAttributes, ws->m_lhs, &info.m_lhs));
	clib::thread rhsRun(__FUNCTION__, boost::bind(&thread_GetWsAttributes, ws->m_rhs, &info.m_rhs));
	lhsRun.join();
	rhsRun.join();
	LogMsg((boost::_tformat(_T("Loading Files (%1%) - End")) % ws->GetDispText()).str());

	LogMsg((boost::_tformat(_T("Merging Files - Before:  %1% + %2% = %3% files")) % info.m_lhs.size() % info.m_rhs.size() % (info.m_lhs.size() + info.m_rhs.size())).str());
	typedef std::map<std::_tstring, AttrAttrPair, NoCaseCompare> LabelAttrAttrPairMap;
	LabelAttrAttrPairMap mergedAttrs;
	for(IAttributeVector::const_iterator itr = info.m_lhs.begin(); itr != info.m_lhs.end(); ++itr)
	{
		mergedAttrs[itr->get()->GetLabel()].first = *itr;
	}
	for(IAttributeVector::const_iterator itr = info.m_rhs.begin(); itr != info.m_rhs.end(); ++itr)
	{
		mergedAttrs[itr->get()->GetLabel()].second = *itr;
	}
	LogMsg((boost::_tformat(_T("Merging Files - After:  %1% files")) % mergedAttrs.size()).str());

	std::vector<AttrAttrPair> sortedAttributes;
	for(LabelAttrAttrPairMap::const_iterator itr = mergedAttrs.begin(); itr != mergedAttrs.end(); ++itr)
	{
		CComPtr<IAttribute> lhs = itr->second.first;
		CComPtr<IAttribute> rhs = itr->second.second;
		if (!lhs)
			lhs = self->m_repLHS->GetAttributePlaceholder(ws->GetDispText(), itr->first.c_str(), rhs->GetType());
		if (!rhs)
			rhs = self->m_repRHS->GetAttributePlaceholder(ws->GetDispText(), itr->first.c_str(), lhs->GetType());
		sortedAttributes.push_back(std::make_pair(lhs, rhs));
	}
	std::sort(sortedAttributes.begin(), sortedAttributes.end(), AttrAttrPairCompare());
	for(std::vector<AttrAttrPair>::const_iterator itr = sortedAttributes.begin(); itr != sortedAttributes.end(); ++itr)
	{
		if (CanShow(filter, itr->first, itr->second))
		{
			info.m_filtered.push_back(std::make_pair(itr->first, itr->second));
		}
	}
}
#endif

#ifdef _SUPPORT_WORKSPACE
void thread_LoadWorkspaceItems(CRepositoryCompareView * self, CWorkspacePairNode * ws)
{
	STRUCT_ATTRIBUTESLOADED info;
	loadWsAttributes(self, ws, self->m_filter, info);
	self->SendMessage(UM_ATTRIBUTESLOADED, (WPARAM)&info);
}
#endif

void loadAttributes(CRepositoryCompareView * self, IModule * mod_lhs, IModule * mod_rhs, int filter, STRUCT_ATTRIBUTESLOADED & info)
{
	LogMsg((boost::_tformat(_T("Loading Files (%1%) - Start")) % mod_lhs->GetLabel()).str());
	clib::thread lhsRun(__FUNCTION__, boost::bind(&thread_GetAttributes, mod_lhs, &info.m_lhs));
	clib::thread rhsRun(__FUNCTION__, boost::bind(&thread_GetAttributes, mod_rhs, &info.m_rhs));
	loadModules(self, mod_lhs->GetQualifiedLabel(), info.m_modFiltered, true);
	lhsRun.join();
	rhsRun.join();
	LogMsg((boost::_tformat(_T("Loading Files (%1%) - End")) % mod_lhs->GetLabel()).str());

	LogMsg((boost::_tformat(_T("Merging Files - Before:  %1% + %2% = %3% files")) % info.m_lhs.size() % info.m_rhs.size() % (info.m_lhs.size() + info.m_rhs.size())).str());
	typedef std::map<std::_tstring, AttrAttrPair, NoCaseCompare> LabelAttrAttrPairMap;
	LabelAttrAttrPairMap mergedAttrs;
	for(IAttributeVector::const_iterator itr = info.m_lhs.begin(); itr != info.m_lhs.end(); ++itr)
	{
		mergedAttrs[itr->get()->GetLabel()].first = *itr;
	}
	for(IAttributeVector::const_iterator itr = info.m_rhs.begin(); itr != info.m_rhs.end(); ++itr)
	{
		mergedAttrs[itr->get()->GetLabel()].second = *itr;
	}
	LogMsg((boost::_tformat(_T("Merging Files - After:  %1% files")) % mergedAttrs.size()).str());

	std::vector<AttrAttrPair> sortedAttributes;
	for(LabelAttrAttrPairMap::const_iterator itr = mergedAttrs.begin(); itr != mergedAttrs.end(); ++itr)
	{
		CComPtr<IAttribute> lhs = itr->second.first;
		CComPtr<IAttribute> rhs = itr->second.second;
		if (!lhs)
			lhs = self->m_repLHS->GetAttributePlaceholder(mod_lhs->GetQualifiedLabel(true), itr->first.c_str(), rhs->GetType());  // Should this be mod_rhs???
		if (!rhs)
			rhs = self->m_repRHS->GetAttributePlaceholder(mod_lhs->GetQualifiedLabel(true), itr->first.c_str(), lhs->GetType());  
		sortedAttributes.push_back(std::make_pair(lhs, rhs));
	}
	std::sort(sortedAttributes.begin(), sortedAttributes.end(), AttrAttrPairCompare());
	for(std::vector<AttrAttrPair>::const_iterator itr = sortedAttributes.begin(); itr != sortedAttributes.end(); ++itr)
	{
		if (CanShow(filter, itr->first, itr->second))
		{
			info.m_filtered.push_back(std::make_pair(itr->first, itr->second));
		}
	}
}

void thread_LoadAttributes(CRepositoryCompareView * self, CModulePairNode * mod)
{
	STRUCT_ATTRIBUTESLOADED info;
	info.treeItem = *mod;
	loadAttributes(self, mod->m_lhs, mod->m_rhs, self->m_filter, info);
	self->SendMessage(UM_ATTRIBUTESLOADED, (WPARAM)&info);
}

void thread_LoadAttributeHistories(CRepositoryCompareView * self, CAttributePairNode * attr)
{
	LogMsg((boost::_tformat(_T("Loading Source Histories (%1%) - Start")) % attr->GetDispText()).str());
	STRUCT_ATTRIBUTEHISTORIESLOADED attrHistMsg;
	attrHistMsg.treeItem = attr;
	attrHistMsg.m_rhs = attr->m_rhs;
	ATLASSERT(attrHistMsg.m_lhs.size() == 0);
	attr->m_lhs->GetHistory(attrHistMsg.m_lhs);
	LogMsg((boost::_tformat(_T("Loading Source Histories (%1%) - End")) % attr->GetDispText()).str());
	self->SendMessage(UM_ATTRIBUTEHISTORIESLOADED, (WPARAM)&attrHistMsg);
}

void CRepositoryCompareView::Load(CTreeNode * item, bool noRefresh)
{
	CComPtr<CLoadingNode> loading = new CLoadingNode();
	if (!item)
	{
		loading->InsertBelow(*this, TVI_ROOT);
		clib::thread run(__FUNCTION__, boost::bind(&thread_LoadWorkspacesAndModules, this, noRefresh));
	}
	else if (CComQIPtr<CWorkspacePairNode> wsPairNode = item)
	{
#ifdef _SUPPORT_WORKSPACE
		loading->InsertBelow(*this, *item);
		m_expandThreads.Append(__FUNCTION__, boost::bind(&thread_LoadWorkspaceItems, this, wsPairNode));
#else
		ATLASSERT(false);
#endif
	}
	else if (CComQIPtr<CModulePairNode> modPairNode = item)
	{
		loading->InsertBelow(*this, *item);
		m_expandThreads.Append(__FUNCTION__, boost::bind(&thread_LoadAttributes, this, modPairNode));
	}
	else if (CComQIPtr<CAttributePairNode> attrPairNode = item)
	{
		loading->InsertBelow(*this, *item);
		clib::thread run(__FUNCTION__, boost::bind(&thread_LoadAttributeHistories, this, attrPairNode));
	}
}

void CRepositoryCompareView::SelectAll()
{
	m_selectThreads.SetMaxThreadCount(0);
	ClearSelection();
	WTL::CTreeItem item = GetRootItem();
	int refreshViewCounter = 0;
	while(item)
	{
		WTL::CTreeItem nextItem = item.GetNextSibling();
		CComPtr<CTreeNode> tn = (CTreeNode *)item.GetData();
		//if (tn->IsExpanded())
		//{
		//	WTL::CTreeItem childItem = item.GetChild();
		//	while(childItem)
		//	{
		//		WTL::CTreeItem nextChildItem = childItem.GetNextSibling();
		//		CComPtr<CTreeNode> ctn = (CTreeNode *)childItem.GetData();
		//		m_sel.SetCheckThreeState(childItem, THREESTATE_BUSY);
		//		m_selectThreads.Append(__FUNCTION__, boost::bind(&thread_ItemClicked, this, ctn.p, false, true));
		//		childItem = nextChildItem;
		//	}
		//}
		//else
		{
			m_sel.SetCheckThreeState(item, THREESTATE_BUSY_UNCHECKED);
			m_selectThreads.Append(__FUNCTION__, boost::bind(&thread_ItemClicked, this, tn.p, false));
		}
		item = nextItem;
	}
	m_selectThreads.SetMaxThreadCount(MAX_THREAD_COUNT);
}

void CRepositoryCompareView::Clear()
{
	SelectItem(NULL);
	WTL::CLockWindowUpdate lock(*this);
	while(WTL::CTreeItem item = GetRootItem())
	{
		item.Delete();
	}
}

void CRepositoryCompareView::ClearSelection()
{
	m_sel.Clear();
}

void CRepositoryCompareView::ExpandAll()
{
	m_expandThreads.SetMaxThreadCount(0);
	WTL::CTreeItem item = GetRootItem();
	while(item)
	{
		CComPtr<CTreeNode> tn = (CTreeNode *)item.GetData();
		CComQIPtr<CModulePairNode> modPairNode = tn;
		CComQIPtr<CWorkspacePairNode> wsPairNode = tn;
		if (modPairNode || wsPairNode)
		{
			if (!tn->IsExpanded())
				tn->Expand();
		}
		item = item.GetNextSibling();
	}
	m_expandThreads.SetMaxThreadCount(MAX_THREAD_COUNT);
}
bool CRepositoryCompareView::SetFilter(int filter)
{
	if (m_filter != filter)
	{
		m_filter = filter;
		Clear();
		Load(NULL, true);
		return true;
	}
	return false;
}

bool CRepositoryCompareView::CanCopy() const
{
	return m_sel.HasSelection();
}

bool CRepositoryCompareView::CanPaste() const
{
	return HasClipboardText();
}

void CRepositoryCompareView::DoCopy() const
{
	std::_tstring attrs;
	m_sel.GetSelection(attrs);
	SetClipboard(attrs);
}

void thread_GetAttribute(CRepositoryCompareView * self, boost::recursive_mutex * mutex, STRUCT_PASTE * info, IRepository * rep, std::_tstring modAtrrLabel, CComPtr<IAttributeType> type)
{
	LogMsg((boost::_tformat(_T("Pasting File - %1% - Start")) % modAtrrLabel.c_str()).str());
	CComPtr<IAttribute> attr = rep->GetAttribute(modAtrrLabel.c_str(), type, 0, true, false);
	boost::recursive_mutex::scoped_lock proc(*mutex);
	if (attr)
		info->m_attrs.push_back(attr.p);
	else
	{
		if (info->m_errors.length() > 0)
			info->m_errors += _T("\r\n");
		info->m_errors += modAtrrLabel.c_str();
	}
	LogMsg((boost::_tformat(_T("Pasting File - %1% - End")) % modAtrrLabel.c_str()).str());
}

void thread_Paste(CRepositoryCompareView * self, IRepository * rep, std::_tstring attrStrings)
{
	boost::recursive_mutex mutex;
	LogMsg(_T("Pasting Files - Start"));
	STRUCT_PASTE info;
	typedef std::vector<std::_tstring> split_vector_type;
	split_vector_type lines; 
	boost::algorithm::split(lines, attrStrings, boost::algorithm::is_any_of(_T("\r\n")), boost::algorithm::token_compress_on); 
	self->m_selectThreads.SetMaxThreadCount(0);
	for(split_vector_type::const_iterator itr = lines.begin(); itr != lines.end(); ++itr)
	{
		split_vector_type labels; 
		boost::algorithm::split(labels, *itr, boost::algorithm::is_any_of(_T(".")), boost::algorithm::token_compress_on); 
		if (labels.size() == 2 || labels.size() == 3)  //Check for blank line
		{
			self->m_selectThreads.Append(__FUNCTION__, boost::bind(thread_GetAttribute, self, &mutex, &info, rep, *itr, labels.size() == 3 ? CreateIAttributeType(labels[2]) : CreateIAttributeECLType()));
		}
	}
	self->m_selectThreads.SetMaxThreadCount(MAX_THREAD_COUNT);
	self->m_selectThreads.Join();
	LogMsg(_T("Pasting Files - End"));
	self->SendMessage(UM_PASTE, (WPARAM)&info);
}

void CRepositoryCompareView::DoPaste(const std::_tstring & attributes) 
{
	std::_tstring attrs = attributes;
	clib::thread run (__FUNCTION__, boost::bind(&thread_Paste, this, m_repLHS, attrs));
}

bool CRepositoryCompareView::DoMigration()
{
	IWorkspaceVector workspaces;
	IAttributeHistoryVector attrs;
	m_sel.GetSelection(m_repLHS, workspaces, attrs);

	if (!attrs.size())
	{
		MessageBox(_T("Nothing can be copied!"), _T("Verify Addition"), MB_OK);
		return false;
	}
	else if (attrs.size())
	{
		bool sandbox = false;
		bool includeWorkspaces = true;
		if (IDOK == ::DoShowAttributesDlg(*this, m_repLHS.p, workspaces, attrs, sandbox, includeWorkspaces, _T("Selected Files"),_T("The following files will be updated:")) )
		{
			try
			{
				m_migrator->Stop();
				if (includeWorkspaces)
				{
					for (IWorkspaceVector::const_iterator i = workspaces.begin(); i != workspaces.end(); ++i)
						m_migrator->AddToRep(*i);
				}
				for (IAttributeHistoryVector::const_iterator i = attrs.begin(); i != attrs.end(); ++i)
					m_migrator->AddToRep(*i, _T(""), sandbox);
				m_migrator->Start();
				m_sel.Clear();
			}
			catch (std::exception & e)
			{
				::MessageBox(NULL, CA2T(e.what()), _T("AMT"), MB_ICONEXCLAMATION | MB_OK);
			}
			return true;
		}
	}
	return false;
}

int CRepositoryCompareView::GetMigrationCount() const
{
	return m_migrator->GetActiveMigrationCount();
}

int CRepositoryCompareView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	SetMsgHandled(false);
	PostMessage(UM_INITIALIZE);
	return 0;
}

LRESULT CRepositoryCompareView::OnInitialize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_ilNormal.CreateFromImage(IDB_REP,16,0,RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
	m_ilNormal.SetBkColor(GetSysColor(COLOR_WINDOW));
	//SetImageList(m_ilNormal, TVSIL_NORMAL); 

	m_ilState.CreateFromImage(IDB_REP_MASK, 8, 0, RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
	m_ilState.SetBkColor(GetSysColor(COLOR_WINDOW));

	m_ilCheck.Create(16, 16, ILC_COLOR32, 6, 0);
	m_ilCheck.SetBkColor(CLR_NONE);
	WTL::CIcon icon;
	int d = 0;
	icon.LoadIcon(IDI_UNCHECKED);
	d = m_ilCheck.AddIcon(icon.Detach());
	icon.LoadIcon(IDI_UNCHECKED);
	d = m_ilCheck.AddIcon(icon.Detach());
	icon.LoadIcon(IDI_CHECKED);
	d = m_ilCheck.AddIcon(icon.Detach());
	icon.LoadIcon(IDI_PARTIALCHECK);
	d = m_ilCheck.AddIcon(icon.Detach());
	icon.LoadIcon(IDI_RADIO_UNCHECKED);
	d = m_ilCheck.AddIcon(icon.Detach());
	icon.LoadIcon(IDI_RADIO_CHECKED);
	d = m_ilCheck.AddIcon(icon.Detach());
	icon.LoadIcon(IDI_BLANK);
	d = m_ilCheck.AddIcon(icon.Detach());
	icon.LoadIcon(IDI_BUSY);  //Busy unchecked
	d = m_ilCheck.AddIcon(icon.Detach());
	icon.LoadIcon(IDI_BUSY);  //Busy checked
	d = m_ilCheck.AddIcon(icon.Detach());
	icon.LoadIcon(IDI_BUSY);  //Busy partial checked
	d = m_ilCheck.AddIcon(icon.Detach());
	icon.LoadIcon(IDI_BUSY);  //Busy radio unchecked
	d = m_ilCheck.AddIcon(icon.Detach());
	icon.LoadIcon(IDI_BUSY);  //Busy radio checked
	d = m_ilCheck.AddIcon(icon.Detach());
	SetImageList(m_ilCheck, TVSIL_STATE); 
	m_jobCount = 0;
	SetTimer(0, 5000);
	return 0;
}

void CRepositoryCompareView::OnSize(UINT nType, CSize size)
{
	SetMsgHandled(false);
	Invalidate();
}

void CRepositoryCompareView::OnTimer(UINT_PTR nIDEvent)
{
	if (m_jobCount != (m_selectThreads.Size() + m_expandThreads.Size()))
	{
		m_jobCount = (m_selectThreads.Size() + m_expandThreads.Size());
		if (m_jobCount)
			PostStatus((boost::_tformat(_T("%1% Items remaining.")) % m_jobCount).str().c_str());
		else
			PostStatus(_T(""));
		PostProgress(m_jobCount);
	}
}

void gatherAttributes(CRepositoryCompareView * self, IModule * mod_lhs, IModule * mod_rhs, STRUCT_ITEMCLICKED & info)
{
	STRUCT_ATTRIBUTESLOADED modInfo;
	//  Not needed I think.  modInfo.treeItem = *moduleNode;
	loadAttributes(self, mod_lhs, mod_rhs, self->m_filter, modInfo);
	for(ModModPairVector::iterator itr = modInfo.m_modFiltered.begin(); itr != modInfo.m_modFiltered.end(); ++itr)
		gatherAttributes(self, itr->first, itr->second, info);

	for(std::vector<AttrAttrPair>::const_iterator itr = modInfo.m_filtered.begin(); itr != modInfo.m_filtered.end(); ++itr)
	{
		info.m_attrs.push_back(itr->first);
	}
}

void thread_ItemClicked(CRepositoryCompareView * self, CTreeNode * tn, bool shiftKey)
{
	STRUCT_ITEMCLICKED info;
	info.m_item = tn;
	if (CComQIPtr<CWorkspacePairNode> workspaceNode = tn)
	{
#ifdef _SUPPORT_WORKSPACE
		LogMsg((boost::_tformat(_T("Loading Content (%1%) - Start")) % workspaceNode->m_lhs->GetLabel()).str());
		STRUCT_ATTRIBUTESLOADED wsInfo;
		loadWsAttributes(self, workspaceNode, self->m_filter, wsInfo);
		for(std::vector<AttrAttrPair>::const_iterator itr = wsInfo.m_filtered.begin(); itr != wsInfo.m_filtered.end(); ++itr)
		{
			info.m_attrs.push_back(itr->first);
		}
		LogMsg((boost::_tformat(_T("Loading Content (%1%) - End")) % workspaceNode->m_lhs->GetLabel()).str());
#else
		ATLASSERT(false);
#endif
	}
	else if (CComQIPtr<CModulePairNode> moduleNode = tn)
	{
		LogMsg((boost::_tformat(_T("Loading Content (%1%) - Start")) % moduleNode->m_lhs->GetQualifiedLabel()).str());
		gatherAttributes(self, moduleNode->m_lhs, moduleNode->m_rhs, info);
		LogMsg((boost::_tformat(_T("Loading Content (%1%) - End")) % moduleNode->m_lhs->GetQualifiedLabel()).str());
	}
	else if (shiftKey && self->m_daliLHS.m_dali) 
	{
		if (CComQIPtr<CAttributePairNode> attrNode = tn)
		{
			LogMsg((boost::_tformat(_T("Calculating Dependencies (%1%) - Start")) % attrNode->m_lhs->GetQualifiedLabel()).str());
			CString ecl = attrNode->m_lhs->GetText(false);
			Dali::AttributeVector results;
			Dali::CEclExceptionVector errorResults;
			self->m_daliLHS.m_dali->GetDependancies(self->m_daliLHS.cluster.c_str(), self->m_daliLHS.queue.c_str(), ecl, attrNode->m_lhs->GetModuleQualifiedLabel(), attrNode->m_lhs->GetLabel(), results, errorResults);
			BOOST_FOREACH(Dali::Attribute attr, results)
			{
				info.m_depAttrs.push_back(self->m_repLHS->GetAttribute(attr.first.c_str(), attr.second.c_str(), CreateIAttributeECLType()));
			}
			LogMsg((boost::_tformat(_T("Calculating Dependencies (%1%) - End")) % attrNode->m_lhs->GetQualifiedLabel()).str());
		}
	}
	self->SendMessage(UM_ITEMCLICKED, (WPARAM)&info);
}

void CRepositoryCompareView::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetMsgHandled(false);
	UINT htFlags = 0;
	WTL::CTreeItem it = HitTest(point, &htFlags);   // See where the click was on
	if (it != NULL && ( htFlags & TVHT_ONITEMSTATEICON)) 
	{   
		SetMsgHandled(true);
		if(it.GetState(TVIS_SELECTED) & TVIS_SELECTED)	//Only multi-select if mouse is within selected item
		{
			m_selectThreads.SetMaxThreadCount(0);
			WTL::CTreeItem first_it = GetFirstSelectedItem();
			THREESTATE busyState = (THREESTATE)((int)THREESTATE_BUSY_UNCHECKED + (int)m_sel.GetCheckThreeState(it));
			for(WTL::CTreeItem it = first_it; it; it = GetNextSelectedItem(it))
			{
				m_sel.SetCheckThreeState(it, busyState);
			}

			m_selectThreads.SetMaxThreadCount(0);
			for(WTL::CTreeItem it = first_it; it; it = GetNextSelectedItem(it))
			{
				it.m_pTreeView = this;
				CComPtr<CTreeNode> tn = (CTreeNode *)it.GetData();
				m_selectThreads.Append(__FUNCTION__, boost::bind(&thread_ItemClicked, this, tn.p, (::GetKeyState(VK_SHIFT) & 0x80) == 0x80));
			}
			m_selectThreads.SetMaxThreadCount(MAX_THREAD_COUNT);
		}
		else
		{
			it.m_pTreeView = this;
			CComPtr<CTreeNode> tn = (CTreeNode *)it.GetData();
			THREESTATE busyState = (THREESTATE)((int)THREESTATE_BUSY_UNCHECKED + (int)m_sel.GetCheckThreeState(it));
			m_sel.SetCheckThreeState(it, busyState);
			clib::thread run(__FUNCTION__, boost::bind(&thread_ItemClicked, this, tn.p, (::GetKeyState(VK_SHIFT) & 0x80) == 0x80));
		}
	}
}

void CRepositoryCompareView::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (point.x == -1 || point.y == - 1)
		GetCursorPos(&point);
	CPoint pt = point;
	ClientToScreen(&pt); 
	UINT flags;
	WTL::CTreeItem item = HitTest(point, &flags);
	CComPtr<CTreeNode> tn = (CTreeNode *)item.GetData();
	WTL::CMenu menu;
	menu.LoadMenu(IDR_POPUP_AMT_VIEW);
	WTL::CMenu popup = menu.GetSubMenu(0);
	if (!item)
	{
		menu.EnableMenuItem(ID_EDIT_ADDTOREP, MFS_DISABLED);
		menu.EnableMenuItem(ID_EDIT_ADDTOREP_DEP, MFS_DISABLED);
	}
	else if (CComQIPtr<CWorkspacePairNode> ws_tn = tn)
	{
		menu.EnableMenuItem(ID_EDIT_ADDTOREP_DEP, MFS_DISABLED);
	}
	else if (CComQIPtr<CModulePairNode> mod_tn = tn)
	{
		menu.EnableMenuItem(ID_EDIT_ADDTOREP_DEP, MFS_DISABLED);
	}
	if (!CanCopy())
	{
		menu.EnableMenuItem(ID_EDIT_COPY, MFS_DISABLED);
		menu.EnableMenuItem(ID_EDIT_CLEARALL, MFS_DISABLED);
	}
	if (!CanPaste())
	{
		menu.EnableMenuItem(ID_EDIT_PASTE, MFS_DISABLED);
	}
	int id = popup.TrackPopupMenuEx(TPM_RETURNCMD, pt.x, pt.y, m_hWnd, NULL);
	menu.DestroyMenu();
	switch ( id )
	{
	case ID_EDIT_CLEARALL:
		ClearSelection();
		break;
	case ID_EDIT_ADDTOREP:
		{
			m_sel.SetCheckThreeState(item, THREESTATE_BUSY_UNCHECKED);
			clib::thread run(__FUNCTION__, boost::bind(&thread_ItemClicked, this, tn.p, false));
		}
		break;
	case ID_EDIT_ADDTOREP_DEP:
		{
			CComPtr<CTreeNode> tn = (CTreeNode *)item.GetData();
			m_sel.SetCheckThreeState(item, THREESTATE_BUSY_UNCHECKED);
			clib::thread run(__FUNCTION__, boost::bind(&thread_ItemClicked, this, tn.p, true));
		}
		break;
	}
}

void CRepositoryCompareView::OnContextMenu(WTL::CWindow wnd, CPoint point)
{
}

//LRESULT CRepositoryCompareView::OnNotify(int idCtrl, LPNMHDR pnmh)
//{
//	switch(pnmh->code)
//	{
//	case TVN_GETDISPINFO:
//		SetMsgHandled(TRUE);
//		{
//			LPNMTVDISPINFO pTVDispInfo = (LPNMTVDISPINFO)pnmh;
//			if (pTVDispInfo->item.lParam)
//			{
//				CComPtr<CTreeNode> tn = (CTreeNode *)pTVDispInfo->item.lParam;
//				tn->GetDispInfo(pTVDispInfo->item);
//			}
//		}
//		break;
//	case TVN_ITEMEXPANDING:
//		SetMsgHandled(TRUE);
//		{
//			CWaitCursor w;
//			LPNMTREEVIEW pNMTreeView = (LPNMTREEVIEW)pnmh;
//			ATLASSERT(pNMTreeView->itemNew.lParam);
//			CComPtr<CTreeNode> tn = (CTreeNode *)pNMTreeView->itemNew.lParam;
//			if (pNMTreeView->action == TVE_EXPAND)
//			{
//				if (!(pNMTreeView->itemNew.state & TVIS_EXPANDEDONCE))
//					Load(tn, false);
//			}
//			else
//				Expand(pNMTreeView->itemNew.hItem, TVE_COLLAPSE | TVE_COLLAPSERESET);
//		}
//		break;
//	case TVN_DELETEITEM:
//		SetMsgHandled(TRUE);
//		{
//			LPNMTREEVIEW pNMTreeView = (LPNMTREEVIEW)pnmh;
//			ATLASSERT(pNMTreeView->itemOld.lParam);
//			CTreeNode * tn = (CTreeNode *)pNMTreeView->itemOld.lParam;
//			tn->SetData(NULL);
//			tn->Release();
//		}
//		break;
//	case TVN_GETINFOTIP:
//		SetMsgHandled(TRUE);
//		{
//			//LPNMTVGETINFOTIP pGetInfoTip = reinterpret_cast<LPNMTVGETINFOTIP>(pnmh);
//			//_tcscpy(pGetInfoTip->pszText, _T("GordonZZZ"));
//		}
//		break;
//	case TVN_BEGINLABELEDIT:
//		SetMsgHandled(TRUE);
//		{
//			LPNMTVDISPINFO dispInfo = (LPNMTVDISPINFO)pnmh;
//			ATLASSERT(dispInfo->item.lParam);
//			CComPtr<CTreeNode> tn = (CTreeNode *)dispInfo->item.lParam;
//			if (tn->CanEdit())
//			{
//				return false;
//			}
//			else
//			{
//				return true;
//			}
//		}
//		break;
//	case TVN_ENDLABELEDIT:
//		SetMsgHandled(TRUE);
//		{
//			LPNMTVDISPINFO dispInfo = (LPNMTVDISPINFO)pnmh;
//			ATLASSERT(dispInfo->item.lParam);
//			if(dispInfo->item.pszText != NULL)
//			{
//				CComPtr<CTreeNode> tn = (CTreeNode *)dispInfo->item.lParam;
//				return tn->Rename(dispInfo->item.pszText);
//			}
//		}
//		break;
//	case TVN_SELCHANGED :
//		SetMsgHandled(true);
//		{
//			LPNMTREEVIEW pnmtv = (LPNMTREEVIEW) pnmh; 
//			ATLTRACE(_T("TVN_SELCHANGED(%0x,%0x)\n"),  pnmtv->itemNew.hItem, pnmtv->itemOld.hItem );
//			if ( pnmtv->itemNew.hItem && pnmtv->itemNew.lParam )
//			{
//				CComPtr<CTreeNode> tn = (CTreeNode *)pnmtv->itemNew.lParam;
//				SendFrameMessage(UM_NEWSELECTION, (WPARAM)tn.p, (LPARAM)true);
//			}
//			else if ( pnmtv->itemOld.hItem && pnmtv->itemOld.lParam )
//			{
//				CComPtr<CTreeNode> tn = (CTreeNode *)pnmtv->itemOld.lParam;
//				SendFrameMessage(UM_NEWSELECTION, (WPARAM)tn.p, (LPARAM)true);
//			}
//		}
//		break;
//
//	case NM_CLICK:
//		{
//			DWORD dw = GetMessagePos();                   // retrieve mouse cursor position when msg was sent
//			CPoint p(GET_X_LPARAM(dw), GET_Y_LPARAM(dw)); // ..and put into point structure
//			ScreenToClient(&p);                    // make coords local to tree client area
//
//			UINT htFlags = 0;
//			WTL::CTreeItem it = HitTest(p, &htFlags);   // See where the click was on
//			if (it != NULL && ( htFlags & TVHT_ONITEMSTATEICON)) 
//			{   
//				SetMsgHandled(TRUE);
//				it.m_pTreeView = this;
//				CComPtr<CTreeNode> tn = (CTreeNode *)it.GetData();
//				m_sel.SetCheckThreeState(it, THREESTATE_BUSY);
//				clib::thread run(__FUNCTION__, boost::bind(&thread_ItemClicked, this, m_cfgLHS, tn.p, ::GetKeyState(VK_SHIFT) & 0x80));
//			}
//		}
//		break;
//	case NM_RCLICK:
//		SetMsgHandled(TRUE);
//		{
//			SendMessage(WM_CONTEXTMENU, (WPARAM)m_hWnd, GetMessagePos());
//		}
//		break;
//	case NM_DBLCLK:
//		//SetMsgHandled(TRUE);
//		//{
//		//	TVITEM item;
//		//	item.mask = TVIF_HANDLE;
//		//	item.hItem = GetSelectedItem();
//		//	if (GetItem(&item))
//		//	{
//		//		CComPtr<CTreeNode> tn = (CTreeNode *)item.lParam;
//		//		tn->OnDblclk(item.hItem);
//		//	}
//		//}
//		break;
//	}
//	return 0;
//}
//

LRESULT CRepositoryCompareView::OnModulesLoaded(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if (!m_repLHS || !m_repRHS)//Logged out...
		return 0;

	CWaitCursor wait;
	WsWsPairVector * workspaces = (WsWsPairVector *)wParam;
	ModModPairVector * mods = (ModModPairVector *)lParam;

	WTL::CLockWindowUpdate lock(*this);
	while(WTL::CTreeItem item = GetRootItem())
	{
		item.Delete();
	}
	for(WsWsPairVector::const_iterator itr = workspaces->begin(); itr != workspaces->end(); ++itr)
	{
		CComPtr<CWorkspacePairNode> ws = new CWorkspacePairNode(itr->first, itr->second);
		ws->InsertBelow(*this, TVI_ROOT);
		m_sel.InitState(ws);
	}
	for(ModModPairVector::const_iterator itr = mods->begin(); itr != mods->end(); ++itr)
	{
		CComPtr<CModulePairNode> mod = new CModulePairNode(itr->first, itr->second);
		mod->InsertBelow(*this, TVI_ROOT);
		m_sel.InitState(mod);
	}
	return 0;
}

LRESULT CRepositoryCompareView::OnAttributesLoaded(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	CWaitCursor wait;
	STRUCT_ATTRIBUTESLOADED * info = (STRUCT_ATTRIBUTESLOADED *)wParam;
	if (info->m_filtered.empty())
	{
		info->treeItem.Delete();
		return 0;
	}
	WTL::CLockWindowUpdate lock(*this);
	while(WTL::CTreeItem item = info->treeItem.GetChild())
	{
		item.Delete();
	}
	for(ModModPairVector::const_iterator itr = info->m_modFiltered.begin(); itr != info->m_modFiltered.end(); ++itr)
	{
		CComPtr<CModulePairNode> mod = new CModulePairNode(itr->first, itr->second);
		mod->InsertBelow(*this, info->treeItem);
		m_sel.InitState(mod);
	}
	for(std::vector<AttrAttrPair>::const_iterator itr = info->m_filtered.begin(); itr != info->m_filtered.end(); ++itr)
	{
		CComPtr<CAttributePairNode> attr = new CAttributePairNode(itr->first, itr->second);
		attr->InsertBelow(*this, info->treeItem);
		m_sel.InitState(attr);
	}
	return 0;
}

typedef std::pair<IAttributeHistoryAdapt, IAttributeHistoryAdapt> AttrHistAttrHistPair;
typedef std::map<std::_tstring, AttrHistAttrHistPair, NoCaseCompare> LabelAttrHistAttrHistPairMap;
LRESULT CRepositoryCompareView::OnAttributeHistoriesLoaded(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	CWaitCursor wait;
	STRUCT_ATTRIBUTEHISTORIESLOADED * info = (STRUCT_ATTRIBUTEHISTORIESLOADED *)wParam;
	LogMsg((boost::_tformat(_T("Merging Source Histories - Before:  %1% + %2% = %3% Source Historys")) % info->m_lhs.size() % 0 % info->m_lhs.size()).str());
	WTL::CLockWindowUpdate lock(*this);
	while(WTL::CTreeItem item = info->treeItem->GetChild())
	{
		item.Delete();
	}
	for(IAttributeHistoryVector::const_iterator itr = info->m_lhs.begin(); itr != info->m_lhs.end(); ++itr)
	{
		CComPtr<CAttributeHistoryPairNode> hist = new CAttributeHistoryPairNode(*itr, info->m_rhs);
		hist->InsertBelow(*this, *info->treeItem);
		m_sel.InitState(hist);
	}
	LogMsg((boost::_tformat(_T("Merging Source Historys - After:  %1% Source Historys")) % info->m_lhs.size()).str());
	return 0;
}

LRESULT CRepositoryCompareView::OnItemClicked(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	STRUCT_ITEMCLICKED * info = (STRUCT_ITEMCLICKED *)wParam;
	m_sel.ItemClicked(info->m_item, &info->m_attrs, &info->m_depAttrs);
	//SendFrameMessage(UM_NEWCHECK);
	return 0;
}

LRESULT CRepositoryCompareView::OnPaste(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	CWaitCursor wait;
	STRUCT_PASTE * info = (STRUCT_PASTE *)wParam;
	if (info->m_errors.length())
	{
		std::_tstring sErrorMessage = (boost::_tformat(_T("The following files could not be located in the repository:\r\n%1%")) % info->m_errors).str();
		MessageBox(sErrorMessage.c_str(), _T("Attribute Migration Tool"), MB_OK | MB_ICONEXCLAMATION );	
	}
	m_sel.SetSelection(info->m_attrs, true);
	Invalidate();
	return 0;
}

LRESULT CRepositoryCompareView::OnInvalidate(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return baseClass::Invalidate((BOOL)wParam);
}

LRESULT CRepositoryCompareView::OnSelChanged(WPARAM wParam, LPNMHDR pNMHDR, BOOL& bHandled)
{
	if (m_bSelectionComplete)
	{
		bHandled = true;
		LPNMTREEVIEW pnmtv = (LPNMTREEVIEW) pNMHDR; 
		ATLTRACE(_T("TVN_SELCHANGED(%0x,%0x)\n"),  pnmtv->itemNew.hItem, pnmtv->itemOld.hItem );
		if ( pnmtv->itemNew.hItem && pnmtv->itemNew.lParam )
		{
			CComPtr<CTreeNode> tn = (CTreeNode *)pnmtv->itemNew.lParam;
			PostFrameMessage(UM_NEWSELECTION, (WPARAM)tn.p, (LPARAM)true);
		}
		else if ( pnmtv->itemOld.hItem && pnmtv->itemOld.lParam )
		{
			CComPtr<CTreeNode> tn = (CTreeNode *)pnmtv->itemOld.lParam;
			PostFrameMessage(UM_NEWSELECTION, (WPARAM)tn.p, (LPARAM)false);
		}
	}
	return 0;
}

LRESULT CRepositoryCompareView::OnItemExpanding(WPARAM wParam, LPNMHDR pNMHDR, BOOL& bHandled)
{
	bHandled = true;
	CWaitCursor w;
	LPNMTREEVIEW pNMTreeView = (LPNMTREEVIEW)pNMHDR;
	ATLASSERT(pNMTreeView->itemNew.lParam);
	CComPtr<CTreeNode> tn = (CTreeNode *)pNMTreeView->itemNew.lParam;
	if (pNMTreeView->action == TVE_EXPAND)
	{
		if (!(pNMTreeView->itemNew.state & TVIS_EXPANDEDONCE))
			Load(tn, false);
	}
	else
	{
		bHandled = false;
		//Expand(pNMTreeView->itemNew.hItem, TVE_COLLAPSE | TVE_COLLAPSERESET);
	}
	return 0;
}
LRESULT CRepositoryCompareView::OnDeleteItem(WPARAM wParam, LPNMHDR pNMHDR, BOOL& bHandled)
{
	bHandled = true;
	LPNMTREEVIEW pNMTreeView = (LPNMTREEVIEW)pNMHDR;
	ATLASSERT(pNMTreeView->itemOld.lParam);
	CTreeNode * tn = (CTreeNode *)pNMTreeView->itemOld.lParam;
	tn->SetData(NULL);
	tn->Release();
	return 0;
}

DWORD CRepositoryCompareView::OnPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW /*lpNMCustomDraw*/)
{        
	return CDRF_NOTIFYITEMDRAW;
}

DWORD CRepositoryCompareView::OnItemPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW lpNMCustomDraw)
{
	return CDRF_NOTIFYPOSTPAINT;
}

DWORD CRepositoryCompareView::OnItemPostPaint(int /*idCtrl*/, LPNMCUSTOMDRAW lpNMCustomDraw)
{
	LPNMTVCUSTOMDRAW lpCustomDraw = reinterpret_cast<LPNMTVCUSTOMDRAW>( lpNMCustomDraw );
	CComPtr<CTreeNode> item = reinterpret_cast<CTreeNode *>(lpCustomDraw->nmcd.lItemlParam);
	CComQIPtr<CTreePairNode> pairItem = (CUnknown *)item;
	if (!pairItem)
		return CDRF_DODEFAULT;

	CRect rcLHS;
	if (!GetItemRect((HTREEITEM)lpCustomDraw->nmcd.dwItemSpec, rcLHS, true))
		return CDRF_DODEFAULT;

	CRect rcRow = lpCustomDraw->nmcd.rc;
	CRect rcRHS(rcRow.Width() / 2 + rcLHS.left, rcLHS.top, rcRow.Width() / 2 + rcLHS.right, rcLHS.bottom);
	WTL::CDCHandle dcPaint( lpCustomDraw->nmcd.hdc );
	int nContextState = dcPaint.SaveDC();
	int nOldBackClr = dcPaint.SetBkColor(lpCustomDraw->clrTextBk);
	{
		CBrush brush;
		brush.CreateSolidBrush(lpCustomDraw->clrTextBk);
		dcPaint.FillRect(rcLHS, brush);
	}

	int cxState, cyState;
	m_ilState.GetIconSize(cxState, cyState);
	//CRect rcCheck(rcLHS.left - rcLHS.Height(), rcLHS.top, rcLHS.left, rcLHS.bottom);
	//DrawThemeBackground(dcPaint, BP_CHECKBOX, CBS_MIXEDNORMAL, rcCheck);	
	m_ilState.Draw(dcPaint, GetStateIcon(pairItem->GetState()), rcLHS.left, rcLHS.top, ILD_TRANSPARENT);
	int cxNormal, cyNormal;
	m_ilNormal.GetIconSize(cxNormal, cyNormal);
	m_ilNormal.Draw(dcPaint, (lpCustomDraw->nmcd.uItemState & CDIS_SELECTED ? pairItem->GetDispSelectedImage() : pairItem->GetDispImage()), rcLHS.left + cxState, rcLHS.top, ILD_TRANSPARENT);
	if (pairItem->hasLHS())
	{
		rcLHS.left += cxState + cxNormal + 3;
		rcLHS.right += cxState + cxNormal + 3;
		int nOldClr = dcPaint.SetTextColor(lpCustomDraw->clrText);
		dcPaint.DrawText(item->GetDispText(), -1, &rcLHS, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
		dcPaint.SetTextColor(nOldClr);
	}

	m_ilState.Draw(dcPaint, GetStateIcon(pairItem->GetStateRHS()), rcRHS.left, rcRHS.top, ILD_TRANSPARENT);
	m_ilNormal.Draw(dcPaint, (lpCustomDraw->nmcd.uItemState & CDIS_SELECTED ? pairItem->GetDispSelectedImageRHS() : pairItem->GetDispImageRHS()), rcRHS.left + cxState, rcRHS.top, ILD_TRANSPARENT);

	if (pairItem->hasRHS())
	{
		rcRHS.left += cxState + cxNormal + 3;
		rcRHS.right += cxState + cxNormal + 3;
		int nOldClr = dcPaint.SetTextColor(lpCustomDraw->clrText);
		dcPaint.DrawText(item->GetDispText(), -1, &rcRHS, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
		dcPaint.SetTextColor(nOldClr);
	}

	dcPaint.SetBkColor(nOldBackClr);
	dcPaint.RestoreDC( nContextState );
	return CDRF_DODEFAULT;
}

//  IMigrationCallback
BOOL CRepositoryCompareView::Invalidate(BOOL bErase) throw()
{
	return SendMessage(UM_INVALIDATE, (WPARAM)bErase);
}

void CRepositoryCompareView::LogMsg(const std::_tstring & msg)
{
	::LogMsg(msg);
}

void CRepositoryCompareView::PostStatus(const TCHAR* pStr)
{
	::PostStatus(pStr, AMTPANE_STATUS);
}

void CRepositoryCompareView::PostProgress(int progress)
{
	::PostProgress(progress);
}


