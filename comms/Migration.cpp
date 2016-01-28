#include "StdAfx.h"

#include "Migration.h"
#include "Repository.h"
#include "Logger.h"
#include "Global.h"
#include "utilDateTime.h"
#include "EclCC.h"

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
protected:
    CMigrationCallbackStub m_stub;
    IMigrationCallback * m_caller;	//Circular reference
    CComPtr<IRepository> m_targetRep;
    clib::CThreadQueue m_threads;

protected:
    static IModule * GetModule(IRepository * rep, const TCHAR * label, int retries)
    {
        IModule * retVal = NULL;
        for(int i = 0; i < retries; ++i)
        {
            retVal = rep->GetModule(label, false, true);
            if (retVal != NULL)
                break;
            Sleep(500);
        }
        return retVal;
    }

    static IModule * GetModulePlaceholder(IRepository * rep, const TCHAR * label, int retries)
    {
        IModule * retVal = NULL;
        for(int i = 0; i < retries; ++i)
        {
            retVal = rep->GetModulePlaceholder(label);
            if (retVal != NULL)
                break;
            Sleep(500);
        }
        return retVal;
    }

    static IAttribute * GetAttribute(IRepository * rep, const TCHAR * moduleLabel, const TCHAR * label, IAttributeType * type, int retries)
    {
        IAttribute * retVal = NULL;
        for(int i = 0; i < retries; ++i)
        {
            retVal = rep->GetAttribute(moduleLabel, label, type, 0, true, false, true);
            if (retVal != NULL)
                break;
            Sleep(20);	//This will happen if the attribute does not exist so need to be quicker...
        }
        return retVal;
    }

    static IAttribute * GetAttributePlaceholder(IRepository * rep, const TCHAR * moduleLabel, const TCHAR * label, IAttributeType * type, int retries)
    {
        IAttribute * retVal = NULL;
        for(int i = 0; i < retries; ++i)
        {
            retVal = rep->GetAttributePlaceholder(moduleLabel, label, type);
            if (retVal != NULL)
                break;
            Sleep(500);
        }
        return retVal;
    }

    static bool Checkout(IAttribute * attr, int retries)
    {
        bool retVal = false;
        for(int i = 0; i < retries; ++i)
        {
            retVal = attr->Checkout();
            if (retVal != false)
                break;
            Sleep(500);
        }
        return retVal;
    }

    static bool Checkin(IAttribute * attr, const TCHAR * comments, int retries)
    {
        bool retVal = false;
        for(int i = 0; i < retries; ++i)
        {
            retVal = attr->Checkin(comments);
            if (retVal != false)
                break;
            Sleep(500);
        }
        return retVal;
    }

    static bool SetText(IAttribute * attr, const TCHAR * ecl, bool noBroadcast, int retries)
    {
        bool retVal = false;
        for(int i = 0; i < retries; ++i)
        {
            retVal = attr->SetText(ecl, noBroadcast);
            if (retVal != false)
                break;
            Sleep(500);
        }
        return retVal;
    }

    static const TCHAR * GetText(IAttributeHistory * attr, int retries)
    {
        const TCHAR * retVal = NULL;
        for(int i = 0; i < retries; ++i)
        {
            retVal = attr->GetText(false, true);
            if (retVal != NULL)
                break;
            Sleep(500);
        }
        return retVal;
    }

    struct thread_AddEclToModule_params 
    {
        IModuleAdapt module; 
        std::_tstring moduleLabel; 
        std::_tstring attrLabel; 
        CComPtr<IAttributeType> type; 
        std::_tstring comment; 
        std::_tstring ecl; 
        std::_tstring modBy; 
        std::_tstring modWhen; 
        bool sandbox;
        bool setTextNoBroadcast;
        thread_AddEclToModule_params() 
        {
        }
    };

    static void thread_AddEclToModule(CComPtr<CMigration> self, thread_AddEclToModule_params params, int retries, int maxThreadCount)
    {
#ifndef _DEBUG
        try
        {
#endif
            if (params.module)
                params.moduleLabel = params.module->GetQualifiedLabel() + std::_tstring(_T(".")) + params.moduleLabel;
            self->m_caller->LogMsg((boost::_tformat(_T("%1%.%2%:  Migration Start")) % params.moduleLabel % params.attrLabel).str());
            IModuleAdapt toMod = GetModule(self->m_targetRep, params.moduleLabel.c_str(), retries);
            if (!toMod)
                toMod = GetModulePlaceholder(self->m_targetRep, params.moduleLabel.c_str(), retries);
            if (!toMod)
            {
                self->m_caller->LogMsg((boost::_tformat(_T("Failed (%1%.%2%):  AMT Unable To Locate \"toMod\"")) % params.moduleLabel % params.attrLabel).str());
                ATLASSERT(false);
                return;
            }
            IAttributeAdapt toAttr = GetAttribute(self->m_targetRep, params.moduleLabel.c_str(), params.attrLabel.c_str(), params.type, retries);
            if (!toAttr)
                toAttr = GetAttributePlaceholder(self->m_targetRep, params.moduleLabel.c_str(), params.attrLabel.c_str(), params.type, retries);
            if (!toAttr)
            {
                self->m_caller->LogMsg((boost::_tformat(_T("Failed (%1%.%2%):  AMT Unable To Locate \"toAttr\"")) % params.moduleLabel % params.attrLabel).str());
                ATLASSERT(false);
                return;
            }

            self->m_caller->PostStatus((boost::_tformat(_T("%1% Items remaining.")) % self->m_threads.Size()).str().c_str());
            self->m_caller->PostProgress(self->m_threads.Size());
            if (!toMod->Exists())
            {
                if (!toMod->Create())
                    self->m_caller->LogMsg((boost::_tformat(_T("Failed (%1%.%2%):  Create Folder Failed")) % params.moduleLabel % params.attrLabel).str());
            }

            if (!toAttr->Exists())
            {
                if (!toAttr->Create())
                    self->m_caller->LogMsg((boost::_tformat(_T("Failed (%1%.%2%):  Create File Failed")) % params.moduleLabel % params.attrLabel).str());
            }

            if (toAttr)
            {
                if (toAttr->IsLocked())
                    self->m_caller->LogMsg((boost::_tformat(_T("Failed (%1%.%2%):  Target File Locked")) % params.moduleLabel % params.attrLabel).str());
                else 
                {
                    bool result = true;
                    std::_tstring toEcl = GetText(toAttr->GetAsHistory(), retries);
                    if (boost::algorithm::equals(params.ecl, toEcl) && boost::algorithm::equals(toAttr->GetChecksum(), toAttr->GetChecksumLocal()))
                        self->m_caller->LogMsg((boost::_tformat(_T("%1%.%2%:  ECL already matches, skipping.")) % params.moduleLabel % params.attrLabel).str());
                    else
                    {
                        if (!params.sandbox && !toAttr->IsCheckedOut())
                        {
                            result = Checkout(toAttr, retries);
                            if (!result)
                                self->m_caller->LogMsg((boost::_tformat(_T("Failed (%1%.%2%):  Unable To Checkout Target File")) % params.moduleLabel % params.attrLabel).str());
                        }
                        if (result)
                        {
                            result = SetText(toAttr, params.ecl.c_str(), params.setTextNoBroadcast, retries);
                            if (!result)
                                self->m_caller->LogMsg((boost::_tformat(_T("Failed (%1%.%2%):  Unable To Save Target ECL")) % params.moduleLabel % params.attrLabel).str());
                        }
                    }
                    if (!params.sandbox && toAttr->IsCheckedOut())
                    {
                        result = Checkin(toAttr, params.comment.c_str(), retries);
                        if (!result)
                            self->m_caller->LogMsg((boost::_tformat(_T("Failed (%1%.%2%):  Unable To Checkin Target File")) % params.moduleLabel % params.attrLabel).str());
                    }
                }
            }
            self->m_caller->LogMsg((boost::_tformat(_T("%1%.%2%:  Migration End")) % params.moduleLabel % params.attrLabel).str());
            if (self->m_threads.Size() <= 1 || self->m_threads.Size() % maxThreadCount == 0)
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
            self->m_caller->LogMsg((boost::_tformat(_T("Exception(%1%.%2%):  %3%.")) % params.moduleLabel % params.attrLabel % e.what()).str());
        }
        catch (...)
        {
            self->m_caller->LogMsg((boost::_tformat(_T("Exception(%1%.%2%):  Unknown.")) % params.moduleLabel % params.attrLabel).str());
        }
#endif
    }

    static void thread_AddWsToRep(CComPtr<CMigration> self, IWorkspaceAdapt fromWorkspace, int retries, int maxThreadCount)
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
            if (self->m_threads.Size() <= 1 || self->m_threads.Size() % maxThreadCount == 0)
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

    static void thread_AddToRep(CComPtr<CMigration> self, IModuleAdapt targetModule, IAttributeHistoryAdapt fromAttr, const std::_tstring comment, bool sandbox, bool setTextNoBroadcast, int retries, int maxThreadCount)
    {
        thread_AddEclToModule_params params;
        params.module = targetModule;
        params.moduleLabel = fromAttr->GetModuleQualifiedLabel(true);
        params.attrLabel = fromAttr->GetLabel();
        params.type = fromAttr->GetType(); 
        params.comment = comment; 
        params.ecl = GetText(fromAttr, retries); 
        params.modBy = fromAttr->GetModifiedBy(); 
        params.modWhen = fromAttr->GetModifiedDate(); 
        params.sandbox = sandbox;
        params.setTextNoBroadcast = setTextNoBroadcast;
        thread_AddEclToModule(self, params, retries, maxThreadCount);
    }

public:
    BEGIN_CUNKNOWN
    END_CUNKNOWN(CUnknown)

    bool m_setTextNoBroadcast;
    int m_retries;
    unsigned int m_maxThreadCount;

    CMigration(IMigrationCallback * caller, bool setTextNoBroadcast, IRepository * targetRep) : m_threads(0), m_setTextNoBroadcast(setTextNoBroadcast)
    {
        m_caller = caller;
        if (!m_caller)
            m_caller = &m_stub;
        m_targetRep = targetRep;

        m_retries = NUM_TRYS;
        m_maxThreadCount = MAX_THREAD_COUNT;
    }

    ~CMigration()
    {
    }

    void AddToRep(IWorkspaceAdapt fromWorkspace)
    {
        m_threads.Append(__FUNCTION__, boost::bind(&thread_AddWsToRep, this, fromWorkspace, m_retries, m_maxThreadCount));
    }

    void AddToRep(IModuleAdapt targetModule, IAttributeHistoryAdapt fromAttr, const std::_tstring & comment, bool sandbox)
    {
        m_threads.Append(__FUNCTION__, boost::bind(&thread_AddToRep, this, targetModule, fromAttr, comment, sandbox, m_setTextNoBroadcast, m_retries, m_maxThreadCount));
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

        thread_AddEclToModule_params params;
        params.module = module;
        params.moduleLabel = modLabel;
        params.attrLabel = attrLabel;
        params.type = type; 
        params.comment = comments; 
        params.ecl = ecl; 
        params.modBy = by; 
        params.modWhen = modDate; 
        params.sandbox = sandbox;
        params.setTextNoBroadcast = m_setTextNoBroadcast;
        m_threads.Append(__FUNCTION__, boost::bind(&thread_AddEclToModule, this, params, m_retries, m_maxThreadCount));
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
        if (m_threads.Size() <= m_maxThreadCount)
            m_threads.SetMaxThreadCount(m_threads.Size() / 2 + 1);
        else
            m_threads.SetMaxThreadCount(m_maxThreadCount);
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

class CLocalMigration : public CMigration
{
public:
    CLocalMigration(IMigrationCallback * caller, bool setTextNoBroadcast, IRepository * targetRep) : CMigration(caller, setTextNoBroadcast, targetRep)
    {
        m_retries = 1;
        m_maxThreadCount = 1;
    }
};

IMigration * CreateIMigration(IRepository * targetRep, bool setTextNoBroadcast, IMigrationCallback * caller)
{
    if (IsLocalRepositoryEnabled()) 
    {
        return new CLocalMigration(caller, setTextNoBroadcast, targetRep);
    }
    return new CMigration(caller, setTextNoBroadcast, targetRep);
}


