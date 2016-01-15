#include "StdAfx.h"
#include "..\en_us\resource.h"

#include "EclDlgAttribute.h"
#include "Global.h"
#include "utilDateTime.h"
#include "Logger.h"
#include <EclCC.h>
#include <UtilFilesystem.h>

//  ===========================================================================
CAttributeDlg::CAttributeDlg(IAttribute *attribute, ISourceSlot * owner) : m_attribute(attribute), baseClass(owner)
{
    if (m_attribute)
    {
        m_sigConn = m_attribute->on_refresh_connect(boost::ref(*this));
        m_attrMonitor = new CAttributeMonitor(m_attribute);
    }
}

void CAttributeDlg::GetTitle(CString & title)
{
    title = m_view.IsDirty() ? "*" : "";
    title += CString(m_attribute->GetModuleQualifiedLabel()) + _T(".") + CString(m_attribute->GetLabel());
}

//bool CBuilderDlg::DoFileOpen(const CString & sPathName) 
//{
//  Help Alligning with BuilderDlg COmpare
//}
bool CAttributeDlg::DoSave(bool attrOnly)
{
    CWaitCursor wait;
    CString ecl;
    m_view.GetText(ecl);
    if (m_attribute->SetText(ecl))
    {
        //  Save local item for history  ---
        boost::filesystem::path path;
        GetIConfig(QUERYBUILDER_CFG)->GetEnvironmentFolder(path);
        path /= stringToPath(m_attribute->GetModuleQualifiedLabel());
        boost::filesystem::create_directories(path);
        path /= stringToPath(m_attribute->GetLabel());
        boost::filesystem::create_directories(path);

        CTime t = CTime::GetCurrentTime();
        std::_tstring now = t.FormatGmt(_T("%Y-%m-%dT%H_%M_%SZ.ecl"));
        path /= stringToPath(now);

        CUnicodeFile file;
        file.Create(pathToWString(path).c_str());
        file.Write(ecl);
        file.Close();
        //  ---  ---  ---

        m_view.SetSavePoint();
        IAttributeVector attrs;
        Dali::CEclExceptionVector errors;
        m_attribute->PreProcess(PREPROCESS_SAVE, NULL, attrs, errors);
        SendMessage(CWM_SUBMITDONE, Dali::WUActionCheck, (LPARAM)&errors);
        if (attrs.size())
        {
            if (!m_migrator)
                m_migrator = CreateIMigration(::AttachRepository(), false);
            m_migrator->Stop();
            for(IAttributeVector::const_iterator itr = attrs.begin(); itr != attrs.end(); ++itr)
                m_migrator->AddToRep(m_attribute->GetModule()->GetRootModule(), itr->get()->GetAsHistory(), (boost::_tformat(_T("Preprocessed (%1%) from %2%.")) % PREPROCESS_LABEL[PREPROCESS_SAVE] % m_attribute->GetQualifiedLabel()).str().c_str(), true);
            m_migrator->Start();
            SetTimer(0, 200);
        }
        return true;
    }
    return false;
}

void CAttributeDlg::OnTimer(UINT_PTR nIDEvent)
{
    if (m_migrator->GetActiveMigrationCount() == 0)
    {
        KillTimer(0);
        GetIMainFrame()->Send_RefreshRepositoryWindow(m_attribute->GetModule());
        GetIMainFrame()->SyncTOC(m_attribute->GetQualifiedLabel(), m_attribute->GetType());
    }
}
//bool CBuilderDlg::DoFileSave(const CString & sPathName) 
//{
//  Help Alligning with BuilderDlg COmpare
//}

void CAttributeDlg::DoCheckSyntax()
{
    m_view.SyntaxChecking();
    CString ecl;
    m_view.GetText(ecl);
    CString cluster(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_CLUSTER));
    CString module(m_attribute->GetModuleQualifiedLabel());
    CString attribute(m_attribute->GetLabel());

    if (CComPtr<IEclCC> eclcc = CreateIEclCC())
    {
        if (!DoSave(false))
            return;
    }
    if (m_attribute->GetType() == CreateIAttributeECLType())
    {
        clib::thread run(__FUNCTION__, boost::bind(&EclCheckSyntax, this, ecl, cluster, module, attribute, _T(""), _T(""), false, false));
    }
    else
    {
        IAttributeVector attrs;
        Dali::CEclExceptionVector errors;
        m_attribute->PreProcess(PREPROCESS_SYNTAXCHECK, ecl, attrs, errors);
        SendMessage(CWM_SUBMITDONE, Dali::WUActionCheck, (LPARAM)&errors);
    }
}

void CAttributeDlg::DoCheckComplexity()
{
    CWaitCursor wait;
    m_view.SyntaxClearAll();
    CString ecl;
    GetSource(ecl);
    StlLinked<Dali::IDali> server = Dali::AttachDali(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT), _T("Dali"));
    std::_tstring complexity, complexitDisplay;
    Dali::CEclExceptionVector errorResults;
    server->CheckComplexity(GetCluster(), GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_QUEUE), ecl, m_attribute->GetModuleQualifiedLabel(), m_attribute->GetLabel(), complexity, errorResults);
    if (complexity.length() == 0)
    {
        complexitDisplay = ComplexityErrorMsg;
        SendMessage(CWM_SUBMITDONE, Dali::WUActionCheck, (LPARAM)&errorResults);
    }
    else
        complexitDisplay = _T("Complexity Count = \"") + complexity + _T("\"");
    MessageBox(complexitDisplay.c_str(), _T("Complexity Count"), MB_ICONINFORMATION);
}

void CAttributeDlg::DoCheckDependency()
{
    CWaitCursor wait;
    m_view.SyntaxClearAll();
    CString ecl;
    GetSource(ecl);
    StlLinked<IRepository> rep = ::AttachRepository();
    StlLinked<Dali::IDali> dali = Dali::AttachDali();
    Dali::CEclExceptionVector errorResults;
    DoShowDependantAttributes(rep, dali, static_cast<const TCHAR *>(CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_QUEUE))), GetCluster(), static_cast<const TCHAR *>(ecl), m_attribute->GetModuleQualifiedLabel(), m_attribute->GetLabel(), errorResults);
    SendMessage(CWM_SUBMITDONE, Dali::WUActionCheck, (LPARAM)&errorResults);
}

void CAttributeDlg::SetAttribute(IAttribute *attribute)
{
    if (m_attribute)
    {
        m_sigConn.disconnect();
        m_attrMonitor = NULL;
    }

    m_attribute = attribute;
    if (m_attribute)
    {
        m_sigConn = m_attribute->on_refresh_connect(boost::ref(*this));
        m_attrMonitor = new CAttributeMonitor(m_attribute);
    }
}

IAttribute *CAttributeDlg::GetAttribute()
{
    return m_attribute;
}

const TCHAR * CAttributeDlg::GetCluster()
{
    m_cluster = CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_CLUSTER));
    return m_cluster.c_str();
}

bool CAttributeDlg::CanExecute()
{
    CString queue = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_QUEUE);
    CString cluster = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_CLUSTER);
    if (cluster.GetLength() > 0 && m_view.GetLength())
        return true;
    return false;
}

LRESULT CAttributeDlg::OnInitDialog(HWND /*hWnd*/, LPARAM /*lParam*/)
{
    SetMsgHandled(false);
    DlgResize_Init(false, false);
    DoDataExchange();
    return 0;
}
LRESULT CAttributeDlg::OnInitialize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    SetMsgHandled(false);
    m_view.SetFocus(true);
    return 0;
}

void CAttributeDlg::OnEclSyncToc(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
{
    GetIMainFrame()->SyncTOC(m_attribute->GetQualifiedLabel(), m_attribute->GetType());
}

void CAttributeDlg::OnEclGoto(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
{
    CString message;
    GetWordAtCurPos(message);
    if (message[0])
        GetIMainFrame()->OpenAttribute(message, m_attribute->GetType(), m_attribute->GetModuleQualifiedLabel());
}

void CAttributeDlg::OnEclGotoSyncToc(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
{
    CString message;
    GetWordAtCurPos(message);
    if (message[0])
        GetIMainFrame()->SyncTOC(message, CreateIAttributeECLType(), m_attribute->GetModuleQualifiedLabel());
}

    //  IAttribute Notifications  ---
void CAttributeDlg::operator()(IAttribute * attr, bool eclChanged, IAttribute * newAttrAsOldOneMoved, bool deleted)
{
    if (eclChanged)
    {
        CString ecl;
        m_view.GetText(ecl);
        if (ecl.Compare(attr->GetText(false)) != 0)
            m_view.SetText(attr->GetText(false));
    }
    //TODO handle renamed and deleted.
    ATLASSERT(!newAttrAsOldOneMoved && !deleted);
}

    //  IMigrationCallback  ---
BOOL CAttributeDlg::Invalidate(BOOL bErase)
{
    return TRUE;
}
void CAttributeDlg::LogMsg(const std::_tstring & msg)
{
}
void CAttributeDlg::PostStatus(const TCHAR* pStr)
{
}
void CAttributeDlg::PostProgress(int progress)
{
}
