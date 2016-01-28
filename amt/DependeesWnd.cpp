#include "StdAfx.h"
#include "..\en_us\resource.h"

#include "DependeesWnd.h"
#include "msxml2.h"
#include "SAXHandlers.h"
#include "UnicodeFile.h"
#include <UtilFilesystem.h>

//  ===========================================================================
class DependencyTreeHandlerImpl : public StackHandlerImpl
{
public:
	std::_tstring m_attr;
	CDependency & m_dependency;

	DependencyTreeHandlerImpl(CDependency & dependency) : m_dependency(dependency)
	{
	}

	virtual HRESULT STDMETHODCALLTYPE startElement( 
		/* [in] */ const wchar_t *pwchNamespaceUri,
		/* [in] */ int cchNamespaceUri,
		/* [in] */ const wchar_t *pwchLocalName,
		/* [in] */ int cchLocalName,
		/* [in] */ const wchar_t *pwchQName,
		/* [in] */ int cchQName,
		/* [in] */ ISAXAttributes *pAttributes)
	{ 
		__super::startElement(pwchNamespaceUri, cchNamespaceUri, pwchLocalName, cchLocalName, pwchQName, cchQName, pAttributes);

		Element &e = m_stack.top();

		switch (m_stack.size())
		{
		case 2:
			{
				m_attr = e.m_attr[_T("module")] + _T(".") + e.m_attr[_T("name")];
				m_dependency.ClearLinks(m_attr);
			}
			break;

		case 3:
			break;
		}

		return S_OK; 
	}

	virtual HRESULT STDMETHODCALLTYPE endElement( 
		/* [in] */ const wchar_t *pwchNamespaceUri,
		/* [in] */ int cchNamespaceUri,
		/* [in] */ const wchar_t *pwchLocalName,
		/* [in] */ int cchLocalName,
		/* [in] */ const wchar_t *pwchQName,
		/* [in] */ int cchQName)
	{ 
		Element & e = m_stack.top();

		switch (m_stack.size())
		{
		case 2:
			break;

		case 3:
			{
				std::_tstring attr = e.m_attr[_T("module")] + _T(".") + e.m_attr[_T("name")];
				m_dependency.InsertLink(m_attr, attr);
			}
			break;
		}
		return __super::endElement(pwchNamespaceUri, cchNamespaceUri, pwchLocalName, cchLocalName, pwchQName, cchQName);
	}
};
//  ===========================================================================
unsigned int CDependency::StringToHash(const std::_tstring & _str) const
{
	std::_tstring str = _str;
	boost::algorithm::to_lower(str);
	StringUIntMapT::const_iterator found = m_attrHashXXX.find(str);
	if (found != m_attrHashXXX.end())
		return found->second;

	boost::crc_32_type computer;
	computer.reset();
	computer.process_bytes(str.c_str(), sizeof(TCHAR) * str.length());

	m_attrHashXXX[str] = computer.checksum();
	m_hashAttrXXX[computer.checksum()] = _str;

	return computer.checksum();
}

const TCHAR * CDependency::HashToString(unsigned int hash) const
{
	UIntStringMapT::const_iterator found = m_hashAttrXXX.find(hash);
	if (found != m_hashAttrXXX.end())
		return found->second.c_str();
	ATLASSERT(false);
	return m_emptyStr.c_str();
}

CDependency::CDependency()
{
}

void CDependency::SetEnvFolder(const boost::filesystem::path & p)
{
	m_envFolder = p;
}

void CDependency::Clear()
{
	m_envFolder = boost::filesystem::path();
	m_services.clear();
	m_xml.clear();
	m_attrGraph.clear();
	m_attrHashXXX.clear();
	m_hashAttrXXX.clear();
}

void CDependency::SetServices(const std::_tstring & services)
{
	m_services = services;
	m_xml.clear();
	m_attrGraph.clear();
	m_attrHashXXX.clear();
	m_hashAttrXXX.clear();
}

const TCHAR * CDependency::GetServices() const
{
	return m_services.c_str();
}

void CDependency::Save() const
{
	if (!m_services.empty())
	{
		{
			boost::filesystem::path p = m_envFolder;
			p = p / "services.txt";
			CUnicodeFile file;
			if (file.Create(p))
				file.Write(m_services);
		}

		{
			std::string fileName = boost::lexical_cast<std::string>(StringToHash(m_services));
			fileName = "services_" + fileName + ".txt";
			boost::filesystem::path p = m_envFolder;
			p = p / fileName;
			CUnicodeFile fileBackup;
			if (fileBackup.Create(p))
				fileBackup.Write(m_services);
		}

		SaveCache();
	}
}

void CDependency::Load()
{
	boost::filesystem::path p = m_envFolder;
	p = p / "services.txt";
	CUnicodeFile file;
	if (file.Open(p))
	{
		std::_tstring items;
		if (file.Read(items))
			SetServices(items);
	}

	LoadCache();
}

bool CDependency::CacheExists() const
{
	std::string fileName = boost::lexical_cast<std::string>(StringToHash(m_services));
	fileName = "dependencies_" + fileName + ".xml";
	boost::filesystem::path p = m_envFolder;
	p = p / fileName;
	return clib::filesystem::exists(p);
}

void CDependency::SaveCache() const
{
	std::string fileName = boost::lexical_cast<std::string>(StringToHash(m_services));
	fileName = "dependencies_" + fileName + ".xml";
	boost::filesystem::path p = m_envFolder;
	p = p / fileName;
	CUnicodeFile file;
	if (file.Create(p))
		file.Write(m_xml);
}

void CDependency::LoadCache() 
{
	std::string fileName = boost::lexical_cast<std::string>(StringToHash(m_services));
	fileName = "dependencies_" + fileName + ".xml";
	boost::filesystem::path p = m_envFolder;
	p = p / fileName;
	CUnicodeFile file;
	if (file.Open(p))
	{
		std::_tstring dependencies;
		if (file.Read(dependencies))
			SetXML(dependencies);
	}
}

void CDependency::SetXML(const std::_tstring & xml)
{
	if (!boost::algorithm::equals(m_xml, xml))
	{
		m_xml = xml;
		m_attrGraph.clear();
		m_attrHashXXX.clear();
		m_hashAttrXXX.clear();
		ParseDependencies();
		Save();
	}
}

bool CDependency::HasXML() const
{
	return !m_xml.empty();
}

const TCHAR * CDependency::GetXML() const
{
	return m_xml.c_str();
}

void CDependency::ClearLinks(const std::_tstring & attribute)
{
	m_attrGraph[StringToHash(attribute)].first.clear();
}

void CDependency::InsertLink(const std::_tstring & attribute, const std::_tstring & dependantAttribute)
{
	m_attrGraph[StringToHash(attribute)].first.insert(StringToHash(dependantAttribute));
	m_attrGraph[StringToHash(attribute)].second = WALK_UNKNOWN;
}

bool CDependency::ParseDependencies()
{
	CComInitialize com;
	CComPtr<ISAXXMLReader> pRdr; 
	HRESULT hr = pRdr.CoCreateInstance(CLSID_SAXXMLReader30);
	CComPtr <DependencyTreeHandlerImpl> pMc = new DependencyTreeHandlerImpl(*this);
	hr = pRdr->putContentHandler(pMc); 
	CComPtr<SAXErrorHandler> pErr = new SAXErrorHandler();
	if(SUCCEEDED(hr))
		pRdr->putErrorHandler(pErr);

	if (hr == S_OK)
	{
		hr = pRdr->parse(CComVariant(m_xml.c_str()));
		if (SUCCEEDED(hr))
			return true;
	}
	return false;
}

void CDependency::RefreshCrossreferences(WTL::CTreeViewCtrl & tree, const UIntSetT & _attrs, const UIntSetT & services)
{
	UIntVectorT ancestors;
	ancestors.reserve(100);
	if (!_attrs.empty())
	{
		for(UIntSetT::const_iterator itr = services.begin(); itr != services.end(); ++itr)
		{
			ClearInclusion();
			UIntSetT attrs = _attrs;
			UIntTreeItemMapT ancestorTreeItem;
			CalcInclusion(tree, *itr, attrs, ancestors, ancestorTreeItem);
			ATLASSERT(ancestors.empty());
		}
	}
}

void CDependency::ClearInclusion()
{
	for(UIntUIntSetBoolPairMapT::iterator itr = m_attrGraph.begin(); itr != m_attrGraph.end(); ++itr)
		itr->second.second = WALK_UNKNOWN;
}

WALK CDependency::CalcInclusion(WTL::CTreeViewCtrl & tree, int attrHash, UIntSetT & targets, UIntVectorT & ancestors, UIntTreeItemMapT & ancestorTreeItem)
{
	//  Avoid infinite recursion.
	if (std::find(ancestors.rbegin(), ancestors.rend(), attrHash) != ancestors.rend())
		return WALK_FALSE;

	UIntUIntSetBoolPairMapT::iterator found = m_attrGraph.find(attrHash);
	if (found != m_attrGraph.end())
	{
		if (found->second.second != WALK_UNKNOWN)
			return found->second.second;

		ancestors.push_back(attrHash);
		found->second.second = WALK_FALSE;
		for(UIntSetT::iterator itr = found->second.first.begin(); itr != found->second.first.end(); ++itr)
		{
			if (CalcInclusion(tree, *itr, targets, ancestors, ancestorTreeItem) == WALK_TRUE)
				found->second.second = WALK_TRUE;
		}

		while (targets.find(attrHash) != targets.end())
		{
			found->second.second = WALK_TRUE;
			HTREEITEM parent = NULL;
			for(UIntVectorT::const_iterator itr = ancestors.begin(); itr != ancestors.end(); ++itr)
			{
				UIntTreeItemMapT::const_iterator found = ancestorTreeItem.find(*itr);
				if (found == ancestorTreeItem.end())
					ancestorTreeItem[*itr] = NULL;
				if (ancestorTreeItem[*itr] == NULL)
					ancestorTreeItem[*itr] = tree.InsertItem(HashToString(*itr), parent, TVI_SORT);
				parent = ancestorTreeItem[*itr];
			}
			targets.erase(attrHash);
		}
		ancestors.pop_back();

		return found->second.second;
	}

	return WALK_FALSE;
}

//
//	if (std::find(ancestors.rbegin(), ancestors.rend(), attrHash) != ancestors.rend())
//		return false;
//
//	UIntUIntSetBoolPairMapT::const_iterator found = m_attrGraph.find(attrHash);
//	if (found != m_attrGraph.end() && found->second.second == WALK_TRUE)
//	{
//		targets.erase(foundTarget);
//		HTREEITEM item = tree.InsertItem(HashToString(attrHash), parent, TVI_SORT);
//		ancestors.push_back(attrHash);
//		for(UIntSetT::const_iterator itr = found->second.first.begin(); itr != found->second.first.end(); ++itr)
//		{
//#define SHOW_ALL
//#ifdef SHOW_ALL
//		RecursiveLoadTree(tree, item, *itr, targets, ancestors);
//#else
//		if (RecursiveLoadTree(tree, item, *itr, targets, ancestors))
//			break;
//#endif
//		}
//		UIntSetT::const_iterator foundTarget = targets.find(attrHash);
//		if (foundTarget != targets.end())
//		{
//		}
//		ancestors.pop_back();
//		return true;
//	}
//	return false;
//}
//  ===========================================================================
CDependeesWnd::CDependeesWnd()
{
}

void CDependeesWnd::SetConfig(IConfig * cfgRHS)
{
	if (cfgRHS)
	{
		m_cfgRHS = cfgRHS;
		LoadAttributes();
		boost::filesystem::path p;
		m_cfgRHS->GetEnvironmentFolder(p);
		m_dependency.SetEnvFolder(p);
		m_dependency.Load();
		m_services.SetWindowText(m_dependency.GetServices());
		CalcCrossreferences();
	}
	else 
	{
		if (m_cfgRHS)
		{
			SaveAttributes();
			m_dependency.Save();
		}
		//  Order is important  ---
		SetAttributes(_T(""));
		m_dependency.Clear();
		m_tree.DeleteAllItems();
		m_services.SetWindowTextW(_T(""));
		m_cfgRHS = cfgRHS;
	}
}

BOOL CDependeesWnd::PreTranslateMessage(MSG* pMsg)
{
	if((pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) &&
		(pMsg->message < WM_MOUSEFIRST || pMsg->message > WM_MOUSELAST))
		return FALSE;

	HWND hWndCtl = ::GetFocus();
	if(IsChild(hWndCtl))
	{
		// find a direct child of the dialog from the window that has focus
		while(::GetParent(hWndCtl) != m_hWnd)
			hWndCtl = ::GetParent(hWndCtl);

		// give control a chance to translate this message
		if(::SendMessage(hWndCtl, WM_FORWARDMSG, 0, (LPARAM)pMsg) != 0)
			return TRUE;
	}

	return CWindow::IsDialogMessage(pMsg);
}

void CDependeesWnd::SetAttributes(const std::_tstring & attrs)
{
	m_attrs.SendMessage(WM_SETTEXT, 0, (LPARAM)attrs.c_str());
	m_tree.DeleteAllItems();
}

void CDependeesWnd::GetAttributes(std::_tstring & attributes, const std::_tstring & delim)
{
	for (int i = 0; i < m_attrs.GetLineCount(); ++i)
	{
		TCHAR buff[1024];
		int len = m_attrs.GetLine(i, buff, 1024);
		if (len)
		{
			if (!attributes.empty())
				attributes += delim;
			attributes.append(buff, len);
		}
	}
}

bool CDependeesWnd::HasAttributes()
{
	std::_tstring attributes;
	GetAttributes(attributes, _T(""));
	return !attributes.empty();
}

void CDependeesWnd::SaveAttributes()
{
	std::_tstring attrs;
	GetAttributes(attrs, _T("\r\n"));
	if (!attrs.empty())
	{
		boost::filesystem::path p;
		m_cfgRHS->GetEnvironmentFolder(p);
		p = p / "sources.txt";
		CUnicodeFile file;
		if (file.Create(p))
			file.Write(attrs);
	}
}

void CDependeesWnd::LoadAttributes()
{
	boost::filesystem::path p;
	m_cfgRHS->GetEnvironmentFolder(p);
	p = p / "sources.txt";
	CUnicodeFile file;
	if (file.Open(p))
	{
		std::_tstring attrs;
		if (file.Read(attrs))
			SetAttributes(attrs);
	}
}

void CDependeesWnd::SetServices(const std::_tstring & services)
{
	m_services.SendMessage(WM_SETTEXT, 0, (LPARAM)services.c_str());
	m_tree.DeleteAllItems();
	m_dependency.SetServices(services);
}

const TCHAR * CDependeesWnd::GetServices(std::_tstring & services, const std::_tstring & delim) const
{
	for (int i = 0; i < m_services.GetLineCount(); ++i)
	{
		TCHAR buff[1024];
		int len = m_services.GetLine(i, buff, 1024);
		if (len)
		{
			if (!services.empty())
				services += delim;
			services.append(buff, len);
		}
	}
	return services.c_str();
}

bool CDependeesWnd::HasServices()
{
	std::_tstring services;
	GetServices(services, _T(""));
	return !services.empty();
}

bool CDependeesWnd::SetDependencies(const std::_tstring & depends)
{
	m_dependency.SetXML(depends);
	CalcCrossreferences();
	return true;
}

void CDependeesWnd::CalcCrossreferences()
{
	//m_graph.LoadGraph(m_dependency.GetXML());
	UIntSetT serviceAttrs;
	if (m_services.HasText())
	{
		for (int i = 0; i < m_services.GetLineCount(); ++i)
		{
			std::_tstring attr;
			TCHAR buff[1024];
			int len = m_services.GetLine(i, buff, 1024);
			attr.append(buff, len);
			serviceAttrs.insert(m_dependency.StringToHash(attr));
		}
	}

	UIntSetT changedAttrs;
	if (m_attrs.HasText())
	{
		for (int i = 0; i < m_attrs.GetLineCount(); ++i)
		{
			std::_tstring attr;
			TCHAR buff[1024];
			int len = m_attrs.GetLine(i, buff, 1024);
			attr.append(buff, len);
			if (!attr.empty())
				changedAttrs.insert(m_dependency.StringToHash(attr));
		}
	}

	m_tree.SetRedraw(false);
	m_tree.DeleteAllItems();
	if (!m_dependency.HasXML())
		m_dependency.LoadCache();

	if (!m_dependency.HasXML())
	{
		m_tree.InsertItem(_T("No cached dependencies for the monitored source list."), NULL, TVI_SORT);
	}
	else
	{
		m_dependency.RefreshCrossreferences(m_tree, changedAttrs, serviceAttrs);
	}
	m_tree.SetRedraw(true);
}

const TCHAR * CDependeesWnd::GetCrossreferenced(std::_tstring & xref) const
{
	HTREEITEM item = m_tree.GetRootItem();
	while(item)
	{
		if (!xref.empty())
			xref += _T("\r\n");
		CString txt;
		m_tree.GetItemText(item, txt);
		xref += (const TCHAR *)txt;
		item = m_tree.GetNextSiblingItem(item);
	}
	return xref.c_str();
}

bool CDependeesWnd::HasCrossreferenced() const
{
	return m_tree.GetRootItem() != NULL;
}

void CDependeesWnd::SaveDependencies()
{
	ATLASSERT(false);
}

bool CDependeesWnd::HasDependency()
{
	return m_dependency.CacheExists();
}

int CDependeesWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	SetMsgHandled(false);

	ModifyStyleEx(WS_EX_CLIENTEDGE, 0);

	m_splitterH.Create(m_hWnd, ATL::CWindow::rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_splitterV.Create(m_splitterH, ATL::CWindow::rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	//m_splitterH2.Create(m_splitterH, ATL::CWindow::rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_paneAttrs.Create(m_splitterV, _T("Modified Files"));
	m_paneAttrs.SetPaneContainerExtendedStyle(PANECNT_NOCLOSEBUTTON, PANECNT_NOCLOSEBUTTON);
	m_attrs.Create(m_paneAttrs, ATL::CWindow::rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_NOHIDESEL, WS_EX_CLIENTEDGE);
	m_attrs.SetFont(WTL::AtlGetDefaultGuiFont());
	m_attrs.ModifyStyle(NULL, ES_AUTOHSCROLL | ES_AUTOVSCROLL);
	m_attrs.SetLimitText(100000);
	m_paneAttrs.SetClient(m_attrs);

	m_paneServices.Create(m_splitterV, _T("Monitored Files"));
	m_paneServices.SetPaneContainerExtendedStyle(PANECNT_NOCLOSEBUTTON, PANECNT_NOCLOSEBUTTON);
	m_services.Create(m_paneServices, ATL::CWindow::rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_NOHIDESEL, WS_EX_CLIENTEDGE);
	m_services.SetFont(WTL::AtlGetDefaultGuiFont());
	m_services.ModifyStyle(NULL, ES_AUTOHSCROLL | ES_AUTOVSCROLL);
	m_services.SetLimitText(100000);
	m_paneServices.SetClient(m_services);

	m_paneCrossref.Create(m_splitterH, _T("Monitored Files Dependant on Modified Files"));
	m_paneCrossref.SetPaneContainerExtendedStyle(PANECNT_NOCLOSEBUTTON, PANECNT_NOCLOSEBUTTON);
	m_tree.Create(m_paneCrossref, ATL::CWindow::rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);
	m_tree.SetFont(WTL::AtlGetDefaultGuiFont());
	m_tree.ModifyStyle(NULL, TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS);
	m_paneCrossref.SetClient(m_tree);

	//m_paneGraph.Create(m_splitterH2, _T("Graph"));
	//m_paneGraph.SetPaneContainerExtendedStyle(PANECNT_NOCLOSEBUTTON, PANECNT_NOCLOSEBUTTON);
	//m_graph.Create(m_paneGraph);
	//m_graph.LoadGraph(_T(""));
	//m_paneGraph.SetClient(m_graph);

	m_splitterV.SetSplitterPanes(m_paneAttrs, m_paneServices);
	m_splitterV.SetSplitterPosPct(50);

	//m_splitterH2.SetSplitterPanes(m_paneCrossref, m_paneGraph);
	//m_splitterH2.SetSplitterPosPct(100);

	m_splitterH.SetSplitterPanes(m_splitterV, m_paneCrossref);
	m_splitterH.SetSplitterPosPct(33);

	return 0;
}

void CDependeesWnd::OnSize(UINT nType, CSize size)
{
	SetMsgHandled(false);
	if( nType != SIZE_MINIMIZED )
	{
		UINT flags = SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSENDCHANGING;
		CRect rcClient;
		GetClientRect(rcClient);
		m_splitterH.SetWindowPos(NULL, rcClient, flags);
	}
}

LRESULT CDependeesWnd::OnEnChangeEdit(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (hWndCtl == m_attrs)
	{
		m_tree.DeleteAllItems();
	}
	else if(hWndCtl == m_services)
	{
		m_tree.DeleteAllItems();
		std::_tstring services;
		GetServices(services, _T("\r\n"));
		m_dependency.SetServices(services);
	}
	return 0;
}
