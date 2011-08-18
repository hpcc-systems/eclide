#include "StdAfx.h"
#include ".\Dfutreenode.h"
#include <util.h> //clib
#include <utilDateTime.h> //clib
#include <dali.h> //comms

const TCHAR * const SYNTAXWARNING = _T("Warning");
const TCHAR * const SYNTAXERROR = _T("Error");
static const CString LF(_T("\n"));

//  ===============================================================================================
CDfuBaseNode::CDfuBaseNode()
{
	ATLASSERT(false);
}
CDfuBaseNode::CDfuBaseNode(IDfuSlot *pOwner) : m_owner(pOwner)
{
}
//  ===============================================================================================
CDfuNode::CDfuNode()
{
	ATLASSERT(false);
}
CDfuNode::CDfuNode(IDfuSlot *pOwner, const CString & /*label*/) : CDfuBaseNode(pOwner)
{
}

CDfuNode::~CDfuNode()
{
}

void CDfuNode::ItemExpanding()
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	m_loading = new CLoadingNode();
	m_loading->InsertBelow(*GetTreeView(), *this);
	clib::thread run(__FUNCTION__, boost::bind(&GetFiles, this, m_loading));
}

void CDfuNode::GetFiles(CComPtr<CDfuNode> self, CComPtr<CLoadingNode> loading)
{
	CComPtr<Dfu::IServer> server = self->GetDfu();
	if (server)	//Not supported by all servers...
	{
		Dfu::IFileVector files;
		server->GetFiles(_T(""), &files);
		self->LoadFiles(files, loading);
	}
}

void CDfuNode::LoadFiles(Dfu::IFileVector & files, CLoadingNode * loading)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	if (m_loading == loading)
	{
		WTL::CLockWindowUpdate lock(*GetTreeView()); 
		for (Dfu::IFileVector::iterator itr = files.begin(); itr != files.end(); ++itr)
		{
			CFileNode * newNode = new CFileNode(m_owner, itr->get());
			newNode->InsertBelow(*GetTreeView(), *this);
			newNode->operator ()(itr->get());
		}
		m_loading->Delete();
		m_loading = NULL;
	}
}

const TCHAR * CDfuNode::GetID()
{
	CComPtr<Dfu::IServer> rep = m_owner->GetDfu();
	if ( rep.p )
		return rep->GetID();
	else
		return _T("?");
}

const TCHAR * CDfuNode::GetDispText()
{
	CComPtr<Dfu::IServer> dfu = m_owner->GetDfu();
	if ( dfu.p )
		return dfu->GetLabel();
	else
		return _T("?");
}

Dfu::IServer * CDfuNode::GetDfu()
{
	return m_owner->GetDfu();
}

//  ===============================================================================================

CFileNode::CFileNode()
{
	ATLASSERT(false);
}
CFileNode::CFileNode(IDfuSlot *pOwner, Dfu::IFile * file) : CDfuBaseNode(pOwner), m_file(file)
{
	ATLASSERT(file);
}

void CFileNode::OnDblclk(const WTL::CTreeItem & /*ti*/)
{
	//m_owner->OnDoubleClick(*m_file);
}

int CFileNode::GetDispChildren()
{
	return m_file->IsDir();
}

void CFileNode::operator()(Dfu::IFile * /*File*/)
{
	if (GetTreeView())
	{
	//	SetState(INDEXTOSTATEIMAGEMASK(GetState()), TVIS_STATEIMAGEMASK);
	}
}

void CFileNode::ItemExpanding()
{
	CComPtr<Dfu::IServer> server = m_owner->GetDfu();

	Dfu::IFileVector files;
	server->GetFiles(m_file->GetLabel(), &files);

	for (Dfu::IFileVector::iterator itr = files.begin(); itr != files.end(); ++itr)
	{
		CFileNode * newNode = new CFileNode(m_owner, itr->get());
		newNode->InsertBelow(*GetTreeView(), *this);
		newNode->operator ()(itr->get());
	}
}

const TCHAR * CFileNode::GetID()
{
	return m_file->GetLabel();
}

const TCHAR * CFileNode::GetDispText()
{
	return m_file->GetLeaf();
}

int CFileNode::GetDispImage()
{
	if (m_file->IsDir())
	{
		return IID_FOLDER_CLOSED;
	}
	return IID_DOCUMENT;
}

int CFileNode::GetDispSelectedImage()
{
	if (m_file->IsDir())
	{
		return IID_FOLDER_OPEN;
	}
	return IID_DOCUMENT;
}

Dfu::IFile * CFileNode::GetFile()
{
	return m_file;
}
//  ===============================================================================================
CDfuFilterNode::CDfuFilterNode()
{
	ATLASSERT(false);
}
CDfuFilterNode::CDfuFilterNode(IDfuSlot *pOwner, const CString & label, Dfu::IFileVectorPtr *results) : CDfuNode(pOwner, label), m_results(results)
{
}

CDfuBaseNode * CDfuFilterNode::RecursiveBuild(Dfu::IFile * item)
{
	CDfuBaseNode * parentNode = NULL;
	CComPtr<Dfu::IFile> parent = item->GetParent();
	parentNode = parent ? RecursiveBuild(parent) : this;
	ATLASSERT(parentNode);
	if (parentNode)
	{
		std::_tstring label = item->GetLabel();
		if (m_items.find(label) == m_items.end())
		{
			m_items[label] = new CFilterFileNode(m_owner, item);
			m_items[label]->InsertBelow(*GetTreeView(), *parentNode);
			if (parentNode != this)
			{
				parentNode->Expand();
			}
		}
		return m_items[label].get();
	}
	return NULL;
}

void CDfuFilterNode::ItemExpanding()
{
	m_items.clear();
	for(Dfu::IFileVector::iterator itr = m_results->begin(); itr != m_results->end(); ++itr)
	{
		RecursiveBuild(itr->get());
	}
}
//  ===============================================================================================
