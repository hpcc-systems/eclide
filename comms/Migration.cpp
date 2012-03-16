#include "StdAfx.h"

#include "Migration.h"
#include "Repository.h"
#include "Logger.h"
#include "Global.h"
#include "utilDateTime.h"

#define NUM_TRYS 10
#define MAX_THREAD_COUNT 5

class CMigrationCallbackStub : public IMigrationCallback
{
	BOOL Invalidate(BOOL bErase = TRUE)
	{
		return TRUE;
	}
	void LogMsg(const std::_tstring & msg)
	{
	}
	void PostStatus(const TCHAR* pStr)
	{
	}
	void PostProgress(int progress)
	{
	}
};

class CMigration : public CUnknown, public IMigration
{
	CMigrationCallbackStub m_stub;
	IMigrationCallback * m_caller;	//Circular reference
	CComPtr<IRepository> m_targetRep;
	clib::CThreadQueue m_threads;

protected:
	static IModule * GetModule(IRepository * rep, const TCHAR * label)
	{
		IModule * retVal = NULL;
		for(int i = 0; i < NUM_TRYS; ++i)
		{
			retVal = rep->GetModule(label, false, true);
			if (retVal != NULL)
				break;
			Sleep(500);
		}
		return retVal;
	}

	static IModule * GetModulePlaceholder(IRepository * rep, const TCHAR * label)
	{
		IModule * retVal = NULL;
		for(int i = 0; i < NUM_TRYS; ++i)
		{
			retVal = rep->GetModulePlaceholder(label);
			if (retVal != NULL)
				break;
			Sleep(500);
		}
		return retVal;
	}

	static IAttribute * GetAttribute(IRepository * rep, const TCHAR * moduleLabel, const TCHAR * label, IAttributeType * type)
	{
		IAttribute * retVal = NULL;
		for(int i = 0; i < NUM_TRYS; ++i)
		{
			retVal = rep->GetAttribute(moduleLabel, label, type, 0, true, false, true);
			if (retVal != NULL)
				break;
			Sleep(20);	//This will happen if the attribute does not exist so need to be quicker...
		}
		return retVal;
	}

	static IAttribute * GetAttributePlaceholder(IRepository * rep, const TCHAR * moduleLabel, const TCHAR * label, IAttributeType * type)
	{
		IAttribute * retVal = NULL;
		for(int i = 0; i < NUM_TRYS; ++i)
		{
			retVal = rep->GetAttributePlaceholder(moduleLabel, label, type);
			if (retVal != NULL)
				break;
			Sleep(500);
		}
		return retVal;
	}

	static bool Checkout(IAttribute * attr)
	{
		bool retVal = false;
		for(int i = 0; i < NUM_TRYS; ++i)
		{
			retVal = attr->Checkout();
			if (retVal != false)
				break;
			Sleep(500);
		}
		return retVal;
	}

	static bool Checkin(IAttribute * attr, const TCHAR * comments)
	{
		bool retVal = false;
		for(int i = 0; i < NUM_TRYS; ++i)
		{
			retVal = attr->Checkin(comments);
			if (retVal != false)
				break;
			Sleep(500);
		}
		return retVal;
	}

	static bool SetText(IAttribute * attr, const TCHAR * ecl)
	{
		bool retVal = false;
		for(int i = 0; i < NUM_TRYS; ++i)
		{
			retVal = attr->SetText(ecl, true);
			if (retVal != false)
				break;
			Sleep(500);
		}
		return retVal;
	}

	static const TCHAR * GetText(IAttributeHistory * attr)
	{
		const TCHAR * retVal = NULL;
		for(int i = 0; i < NUM_TRYS; ++i)
		{
			retVal = attr->GetText(false, true);
			if (retVal != NULL)
				break;
			Sleep(500);
		}
		return retVal;
	}

	typedef std::pair<std::_tstring, std::_tstring> ModAttrPair;
	static void thread_AddEclToModule(CComPtr<CMigration> self, IModuleAdapt module, ModAttrPair modAttr, IAttributeType * type, const std::_tstring comment, const std::_tstring ecl, const std::_tstring modBy, const std::_tstring modWhen, bool sandbox)
	{
#ifndef _DEBUG
		try
		{
#endif
			if (module)
				modAttr.first = module->GetLabel() + std::_tstring(_T(".")) + modAttr.first;
			self->m_caller->LogMsg((boost::_tformat(_T("%1%.%2%:  Migration Start")) % modAttr.first % modAttr.second).str());
			IModuleAdapt toMod = GetModule(self->m_targetRep, modAttr.first.c_str());
			if (!toMod)
				toMod = GetModulePlaceholder(self->m_targetRep, modAttr.first.c_str());
			if (!toMod)
			{
				self->m_caller->LogMsg((boost::_tformat(_T("Failed (%1%.%2%):  AMT Unable To Locate \"toMod\"")) % modAttr.first % modAttr.second).str());
				ATLASSERT(false);
				return;
			}
			IAttributeAdapt toAttr = GetAttribute(self->m_targetRep, modAttr.first.c_str(), modAttr.second.c_str(), type);
			if (!toAttr)
				toAttr = GetAttributePlaceholder(self->m_targetRep, modAttr.first.c_str(), modAttr.second.c_str(), type);
			if (!toAttr)
			{
				self->m_caller->LogMsg((boost::_tformat(_T("Failed (%1%.%2%):  AMT Unable To Locate \"toAttr\"")) % modAttr.first % modAttr.second).str());
				ATLASSERT(false);
				return;
			}

			self->m_caller->PostStatus((boost::_tformat(_T("%1% Items remaining.")) % self->m_threads.Size()).str().c_str());
			self->m_caller->PostProgress(self->m_threads.Size());
			if (!toMod->Exists())
			{
				if (!toMod->Create())
					self->m_caller->LogMsg((boost::_tformat(_T("Failed (%1%.%2%):  Create Folder Failed - See Warning Tab")) % modAttr.first % modAttr.second).str());
			}

			if (!toAttr->Exists())
			{
				if (!toAttr->Create())
					self->m_caller->LogMsg((boost::_tformat(_T("Failed (%1%.%2%):  Create File Failed - See Warning Tab")) % modAttr.first % modAttr.second).str());
			}

			if (toAttr)
			{
				if (toAttr->IsLocked())
					self->m_caller->LogMsg((boost::_tformat(_T("Failed (%1%.%2%):  Target File Locked")) % modAttr.first % modAttr.second).str());
				else 
				{
					bool result = true;
					std::_tstring toEcl = GetText(toAttr->GetAsHistory());
					if (boost::algorithm::equals(ecl, toEcl) && boost::algorithm::equals(toAttr->GetChecksum(), toAttr->GetChecksumLocal()))
						self->m_caller->LogMsg((boost::_tformat(_T("%1%.%2%:  ECL already matches, skipping.")) % modAttr.first % modAttr.second).str());
					else
					{
						if (!sandbox && !toAttr->IsCheckedOut())
						{
							result = Checkout(toAttr);
							if (!result)
								self->m_caller->LogMsg((boost::_tformat(_T("Failed (%1%.%2%):  Unable To Checkout Target File - See Warning Tab")) % modAttr.first % modAttr.second).str());
						}
						if (result)
						{
							result = SetText(toAttr, ecl.c_str());
							if (!result)
								self->m_caller->LogMsg((boost::_tformat(_T("Failed (%1%.%2%):  Unable To Save Target ECL - See Warning Tab")) % modAttr.first % modAttr.second).str());
						}
					}
					if (!sandbox && toAttr->IsCheckedOut())
					{
						result = Checkin(toAttr, comment.c_str());
						if (!result)
							self->m_caller->LogMsg((boost::_tformat(_T("Failed (%1%.%2%):  Unable To Checkin Target File - See Warning Tab")) % modAttr.first % modAttr.second).str());
					}
				}
			}
			self->m_caller->LogMsg((boost::_tformat(_T("%1%.%2%:  Migration End")) % modAttr.first % modAttr.second).str());
			if (self->m_threads.Size() <= 1 || self->m_threads.Size() % MAX_THREAD_COUNT == 0)
			{
				self->m_caller->Invalidate(false);
			}
			if (self->m_threads.Size() <= 1)
			{
				self->m_caller->PostStatus(_T(""));
				self->m_caller->PostProgress(0);
			}
			else
			{
				self->m_caller->PostStatus((boost::_tformat(_T("%1% Items remaining.")) % self->m_threads.Size()).str().c_str());
				self->m_caller->PostProgress(self->m_threads.Size());
			}
#ifndef _DEBUG
		}
		catch (std::exception & e)
		{
			self->m_caller->LogMsg((boost::_tformat(_T("Exception(%1%.%2%):  %3%.")) % modAttr.first % modAttr.second % e.what()).str());
		}
		catch (...)
		{
			self->m_caller->LogMsg((boost::_tformat(_T("Exception(%1%.%2%):  Unknown.")) % modAttr.first % modAttr.second).str());
		}
#endif
	}

	static void thread_AddWsToRep(CComPtr<CMigration> self, IWorkspaceAdapt fromWorkspace)
	{
#ifndef _DEBUG
		try
		{
#endif
			self->m_caller->LogMsg((boost::_tformat(_T("%1%:  Workspace Migration Start")) % fromWorkspace->GetLabel()).str());
			IWorkspaceAdapt toWorkspace = self->m_targetRep->CreateIWorkspace(fromWorkspace->GetLabel());
			if (!toWorkspace)
			{
				self->m_caller->LogMsg((boost::_tformat(_T("Failed (%1%):  AMT Unable To Locate \"toWorkspace\"")) % fromWorkspace->GetLabel()).str());
				ATLASSERT(false);
				return;
			}
			self->m_caller->PostStatus((boost::_tformat(_T("%1% Items remaining.")) % self->m_threads.Size()).str().c_str());
			self->m_caller->PostProgress(self->m_threads.Size());
			IWorkspaceItemVector items;
			fromWorkspace->GetWindows(items);
			toWorkspace->SetWindows(items);
			self->m_caller->LogMsg((boost::_tformat(_T("%1%:  Workspace Migration End")) % fromWorkspace->GetLabel()).str());
			if (self->m_threads.Size() <= 1 || self->m_threads.Size() % MAX_THREAD_COUNT == 0)
			{
				self->m_caller->Invalidate(false);
			}
			if (self->m_threads.Size() <= 1)
			{
				self->m_caller->PostStatus(_T(""));
				self->m_caller->PostProgress(0);
			}
			else
			{
				self->m_caller->PostStatus((boost::_tformat(_T("%1% Items remaining.")) % self->m_threads.Size()).str().c_str());
				self->m_caller->PostProgress(self->m_threads.Size());
			}
#ifndef _DEBUG
		}
		catch (std::exception & e)
		{
			self->m_caller->LogMsg((boost::_tformat(_T("Exception(%1%):  %2%.")) % fromWorkspace->GetLabel() % e.what()).str());
		}
		catch (...)
		{
			self->m_caller->LogMsg((boost::_tformat(_T("Exception(%1%):  Unknown.")) % fromWorkspace->GetLabel()).str());
		}
#endif
	}

	static void thread_AddToRep(CComPtr<CMigration> self, IAttributeHistoryAdapt fromAttr, const std::_tstring comment, bool sandbox)
	{
		thread_AddEclToModule(self, NULL, ModAttrPair(fromAttr->GetModuleQualifiedLabel(true), fromAttr->GetLabel()), fromAttr->GetType(), comment, GetText(fromAttr), fromAttr->GetModifiedBy(), fromAttr->GetModifiedDate(), sandbox);
	}


public:
	BEGIN_CUNKNOWN
	END_CUNKNOWN(CUnknown)

	CMigration(IMigrationCallback * caller, IRepository * targetRep) : m_threads(0)
	{
		m_caller = caller;
		if (!m_caller)
			m_caller = &m_stub;
		m_targetRep = targetRep;
	}

	~CMigration()
	{
	}

	void AddToRep(IWorkspaceAdapt fromWorkspace)
	{
		m_threads.Append(__FUNCTION__, boost::bind(&thread_AddWsToRep, this, fromWorkspace));
	}

	void AddToRep(IAttributeHistoryAdapt fromAttr, const std::_tstring & comment, bool sandbox)
	{
		m_threads.Append(__FUNCTION__, boost::bind(&thread_AddToRep, this, fromAttr, comment, sandbox));
	}

	void AddEclToRep(const std::_tstring & modLabel, const std::_tstring & attrLabel, IAttributeType * type, const std::_tstring & comment, const std::_tstring & ecl, const std::_tstring & by, bool sandbox)
	{
		AddEclToModule(NULL, modLabel, attrLabel, type, comment, ecl, by, sandbox);
	}

	void AddEclToRep(const std::_tstring & modAttrLabel, IAttributeType * type, const std::_tstring & comment, const std::_tstring & ecl, const std::_tstring & by, bool sandbox)
	{
		AddEclToModule(NULL, modAttrLabel, type, comment, ecl, by, sandbox);
	}

	void AddEclToModule(IModuleAdapt module, const std::_tstring & modLabel, const std::_tstring & attrLabel, IAttributeType * type, const std::_tstring & comment, const std::_tstring & ecl, const std::_tstring & by, bool sandbox)
	{
		std::_tstring modDate;
		CurrentDateTimeUTCString(modDate);
		std::_tstring comments = comment.length() ? comment : (boost::_tformat(_T("%1%, Copied with AMT from File Modified by %2%")) % modDate % by).str();
		m_threads.Append(__FUNCTION__, boost::bind(&thread_AddEclToModule, this, module, ModAttrPair(modLabel, attrLabel), type, comments, ecl, by, modDate, sandbox));
	}

	void AddEclToModule(IModuleAdapt module, const std::_tstring & modAttrLabel, IAttributeType * type, const std::_tstring & comment, const std::_tstring & ecl, const std::_tstring & by, bool sandbox)
	{
		typedef std::vector<std::_tstring> split_vector_type;
		split_vector_type SplitVec; 
		boost::algorithm::split(SplitVec, modAttrLabel, boost::algorithm::is_any_of("."), boost::algorithm::token_compress_on);
		std::_tstring modLabel, attrLabel;
		for (split_vector_type::size_type i = 0; i < SplitVec.size(); ++i)
		{
			if (i == SplitVec.size() - 1)
				attrLabel = SplitVec[i];
			else 
			{
				if (!modLabel.empty())
					modLabel += _T(".");
				modLabel += SplitVec[i];
			}
		}

		if (!modLabel.empty() && !attrLabel.empty())
			AddEclToModule(module, modLabel, attrLabel, type, comment, ecl, by, sandbox);
	}

	void Start()
	{
		if (m_threads.Size() <= MAX_THREAD_COUNT)
			m_threads.SetMaxThreadCount(m_threads.Size() / 2 + 1);
		else
			m_threads.SetMaxThreadCount(MAX_THREAD_COUNT);
	}

	void Stop()
	{
		m_threads.SetMaxThreadCount(0);
	}

	void Join()
	{
		m_threads.Join();
	}

	int GetActiveMigrationCount()
	{
		return m_threads.Size();
	}
};

IMigration * CreateIMigration(IRepository * targetRep, IMigrationCallback * caller)
{
	return new CMigration(caller, targetRep);
}


