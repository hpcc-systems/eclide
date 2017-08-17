#pragma once

#include "wlib.h"

#include <Dali.h> //comms
#include <Workunit.h> //comms

enum TYPE {
    TYPE_ECL,
    TYPE_ARCHIVE,
    TYPE_DESDL
};

class WLIB_API CEclExec : public CUnknown, public boost::signals::trackable
{
public:
    IMPLEMENT_CUNKNOWN
    virtual ~CEclExec()
    {
    }

    virtual void ExecEcl(const TCHAR *clusterName, const TCHAR *queueName, Dali::WUAction action, const TCHAR *attrQualifiedLabel, const TCHAR *eclSource, const TCHAR *eclPath, const TCHAR *scheduled, const TCHAR *label, int resultLimit, const TCHAR *debugSettings, TYPE type, int maxRunTime=0, bool debug=false);
    virtual void ExecEclNoRefCount(const TCHAR *clusterName, const TCHAR *queueName, Dali::WUAction action, const TCHAR *attrQualifiedLabel, const TCHAR *eclSource, const TCHAR *eclPath);

    virtual void WorkunitCreated(Dali::IWorkunit * src);
    virtual void WorkunitUpdated(Dali::IWorkunit * src);
    virtual void WorkunitDeleted(Dali::IWorkunit * src);

    virtual void DesdlPublish(const std::_tstring & desdlID);

    // Callback - don't call directly
    void operator()(Dali::WUEvent evnt, Dali::IWorkunit *wu);

    //Helper function
    void WorkunitDelete(Dali::IWorkunit* wu);

protected:
    CEclExec()
    {
    }

private:
    struct BindLimitStruct
    {
        TYPE type;
        int maxRunTime;
        bool debug;
    };
    static void EclGo(CComPtr<CEclExec> bf, std::pair<std::_tstring, std::_tstring> clusterQueue, std::pair<Dali::WUAction, std::_tstring> actionAttrQualifiedLabel, CString ecl, CString path, CString label, int resultLimit, CString debugSettings, BindLimitStruct bls);
    static void EclGoNoRefCount(CEclExec * bf, CString cluster, CString queue, Dali::WUAction action, CString attrQualifiedLabel, CString ecl, CString path);
    static void EclSchedule(CComPtr<CEclExec> bf, std::pair<std::_tstring, std::_tstring> clusterQueue, std::pair<Dali::WUAction, std::_tstring> actionAttrQualifiedLabel, std::pair<std::_tstring, std::_tstring> eclWhen, CString label, int resultLimit, CString debugSettings, TYPE type, int maxRunTime);
    static void DeleteWorkunit(Dali::IWorkunit * src);
};

