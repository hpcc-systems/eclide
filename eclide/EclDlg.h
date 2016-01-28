#pragma once

#include "SourceView.h"
#include "GotoDlg.h"
#include "PersistMap.h"
#include "EclCommand.h"
#include <findReplace.h>
#include "MainFrm.h"
#include "Combo.h"
#include <EclCC.h>

//  ===========================================================================
__interface IAttribute;

template<typename T>
class CEclDlgQBImpl:
    public CSourceDlgImpl<T>, 
    public CEclCommandMixin<T>
{
    typedef T thisClass;
    typedef CSourceDlgImpl<T> baseClass;
    typedef CEclCommandMixin<T> baseClassEclCmd;

public:
    CEclDlgQBImpl(ISourceSlot * owner) : baseClass(owner)
    {
        InitEclCommandMixin(::GetUIUpdate(), ::GetIMainFrame(), &m_view, owner);
    }

    //  ---  ---  ---
    virtual void GetTitle(CString & title) = 0;
    virtual bool DoSave(bool attrOnly) = 0;

    void DoInit()
    {
        m_view.DoInit();
    }

    bool DoQuerySave()
    {
        if (m_view.IsDirty())
        {
            BringWindowToTop();
            CString msg, title;
            GetTitle(title);
            msg.Format(_T("Save changes to %s?"), title);
            CString prog_title;
            prog_title.LoadString(IDR_MAINFRAME);
            switch(MessageBox(msg, prog_title, MB_YESNOCANCEL | MB_ICONQUESTION | MB_DEFBUTTON1))
            {
            case IDYES:
                if (!DoSave(false))
                    return false;
                break;
            case IDCANCEL:
                return false;
                break;
            default:
                break;
            }
        }
        return true;
    }

    BEGIN_MSG_MAP(thisClass)
        //This map just handles the events that the generic CEclCommandMixin doesn't handle
        //or doesn't handle fully

        MSG_WM_SETFOCUS(OnSetFocus)
        MSG_WM_KILLFOCUS(OnKillFocus)
        MSG_WM_CONTEXTMENU(OnContextMenu)

        MESSAGE_HANDLER_EX(CWM_GLOBALSAVE, OnFileSaveAll)
        MESSAGE_HANDLER_EX(CWM_GLOBALSAVEATTRSONLY, OnFileSaveAttrOnlyAll)
        MESSAGE_HANDLER_EX(CWM_GLOBALQUERYSAVE, OnFileQuerySaveAll)
        MESSAGE_HANDLER(CWM_INSERTTEXT, OnInsertText)
        MESSAGE_HANDLER_EX(CWM_SYNTAXCLEAR, OnClearSyntax)

        COMMAND_ID_HANDLER_EX(ID_FILE_SAVE, OnFileSave)
        COMMAND_ID_HANDLER_EX(ID_FILE_PRINT, OnFilePrint)

        COMMAND_ID_HANDLER_EX(ID_ECL_CLEARERRORS, OnEclClearErrors)
        COMMAND_ID_HANDLER_EX(ID_ECL_CHECKSYNTAX, OnEclCheckSyntax)
        COMMAND_ID_HANDLER(ID_ECL_CHECKCOMPLEXITY, OnEclCheckComplexity)
        COMMAND_ID_HANDLER_EX(ID_ECL_CHECKDEPENDENCY, OnEclCheckDependency)

        CHAIN_MSG_MAP_ALT(baseClassEclCmd, CEclCommandMixinMAP_ID)

        CHAIN_MSG_MAP(baseClass)
    END_MSG_MAP()

    bool UIUpdateMenuItems(CCmdUI * cui)
    {
        if (baseClassEclCmd::UIUpdateMenuItems(cui))
            return true;
        //UPDATEUI(cui, ID_ECL_CHECKSYNTAX, true);
        return false;
    }

    void OnSetFocus(HWND /*hWndOther*/)
    {
        SetMsgHandled(false);
        GetIMainFrame()->Send_RefreshSyntax(&m_view, m_view.SyntaxGetCurrent());
    }

    void OnKillFocus(HWND /*hWnd*/)
    {
        SetMsgHandled(false);
    }

    void OnContextMenu(HWND phWnd, CPoint pt)
    {
        WTL::CMenu m, edit;
        m.LoadMenu(IDR_MAINFRAME);
        edit = m.GetSubMenu(1);
        GetIMainFrame()->CloneState(edit);
        CString copyStr, titleStr;
        GetTitle(titleStr);
        titleStr.TrimLeft(_T("*"));
        copyStr.Format(_T("&Copy \"%s\"\tCtrl+Alt+C"), titleStr);
        m.ModifyMenu(ID_REPOSITORY_COPY, MF_BYCOMMAND | MF_STRING, ID_REPOSITORY_COPY, copyStr); 
        BOOL id = edit.TrackPopupMenuEx(TPM_RETURNCMD, pt.x, pt.y, phWnd, NULL);
        m.DestroyMenu();
        if (id)
            PostMessage(WM_COMMAND, id);
    }

    LRESULT OnClearSyntax(UINT /*uMsg*/, WPARAM /*linesAdded*/, LPARAM /*lParam*/)
    {
        m_view.SyntaxClearAll();
        GetIMainFrame()->Send_ClearSyntax();
        GetIMainFrame()->Send_RefreshStatusBar1(_T(""));
        return 0;
    }

    void OnFileSave(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
    {
        if (m_view.IsDirty())
            DoSave(false);
    }

    void OnEclCheckSyntax(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
    {
        T * pT = static_cast<T*>(this);
        if (pT->m_eclSlot)
            m_eclSlot->PostStatus(_T("Checking..."));

        GetIMainFrame()->Send_RefreshSyntax(NULL, 0, false);
        if (IsLocalRepositoryEnabled())
            GetIMainFrame()->DoFileSaveAll(true);

        pT->DoCheckSyntax();
    }

    LRESULT OnEclCheckComplexity(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/, BOOL & bHandled)
    {
        T * pT = static_cast<T*>(this);
        pT->DoCheckComplexity();
        return true;
    }

    void OnEclCheckDependency(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
    {
        T * pT = static_cast<T*>(this);
        pT->DoCheckDependency();
    }

    bool LayoutPages()
    {
        CDC dc = m_printer.CreatePrinterDC(m_devmode);
        if(dc.IsNull())
            return false;

        RECT rcPage;
        rcPage.left = rcPage.top = 0;
        rcPage.bottom = GetDeviceCaps(dc, PHYSICALHEIGHT);
        rcPage.right = GetDeviceCaps(dc, PHYSICALWIDTH);
        // We have to adjust the origin because 0,0 is not at the corner of the paper
        // but is at the corner of the printable region
        int nOffsetX = dc.GetDeviceCaps(PHYSICALOFFSETX);
        int nOffsetY = dc.GetDeviceCaps(PHYSICALOFFSETY);
        dc.SetViewportOrg(-nOffsetX, -nOffsetY);
        rcPage.right = MulDiv(rcPage.right, 1440, GetDeviceCaps(dc, LOGPIXELSX));
        rcPage.bottom = MulDiv(rcPage.bottom, 1440, GetDeviceCaps(dc, LOGPIXELSY));

        RECT rcOutput = rcPage;
        //convert from 1/1000" to twips
        rcOutput.left += MulDiv(m_rcMargin.left, 1440, 1000);
        rcOutput.right -= MulDiv(m_rcMargin.right, 1440, 1000);
        rcOutput.top += MulDiv(m_rcMargin.top, 1440, 1000);
        rcOutput.bottom -= MulDiv(m_rcMargin.bottom, 1440, 1000);

        RangeToFormat fr;
        fr.hdc = dc;
        fr.hdcTarget = dc;
        fr.rc = PRectangle(rcOutput.left, rcOutput.top, rcOutput.right, rcOutput.bottom);
        fr.rcPage = PRectangle(rcPage.left, rcPage.top, rcPage.right, rcPage.bottom);
        fr.chrg.cpMin = 0;
        fr.chrg.cpMax = -1;

        LONG n = m_view.GetTextLength();
        m_arrPages.RemoveAll();
        while (1)
        {
            m_arrPages.Add(fr.chrg.cpMin);
            LONG lRet = m_view.FormatRange(false, (long)&fr);
            if((lRet - fr.chrg.cpMin) == -1)
            {
                m_arrPages.RemoveAt(m_arrPages.GetSize()-1);
                break;
            }
            else
                fr.chrg.cpMin = lRet;
            if (fr.chrg.cpMin >= n)
                break;
        }

        m_view.FormatRange(false, NULL);

        return true;
    }

    void OnFilePrint(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
    {
        CString title;
        GetTitle(title);
        m_view.DoPrint(static_cast<const TCHAR *>(title));
    }

    void OnFilePrintDirect(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
    {
        CString title;
        GetTitle(title);
        m_view.DoPrint(static_cast<const TCHAR *>(title));
    }

    virtual bool PrintPage(UINT /*nPage*/, HDC /*hDC*/)
    {
        return false;
    }


    LRESULT OnFileSaveAll(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
    {
        if (m_view.IsDirty())
            DoSave(false);
        return 0;
    }

    LRESULT OnFileSaveAttrOnlyAll(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
    {
        if (m_view.IsDirty())
            DoSave(true);
        return 0;
    }

    LRESULT OnFileQuerySaveAll(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam)
    {
        bool * result = reinterpret_cast<bool *>(lParam);
        *result = DoQuerySave();
        return 0;
    }


    void OnEclClearErrors(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
    {
        m_view.SyntaxClearAll();
        GetIMainFrame()->Send_ClearSyntax();
        GetIMainFrame()->Send_RefreshStatusBar1(_T("")); 
    }
};
//  ===========================================================================
