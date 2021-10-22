#pragma once

#include "wlib.h"

#include "SciTE.h"
#include "SciSyntaxMarker.h"
#include "LangRef.h"

#include "scilexer.h"
#include "Attribute.h"
#include "Cluster.h"
#include "FindReplace.h"

typedef CWinTraits<WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_TABSTOP, WS_EX_CLIENTEDGE> CScintillaWinTraits;

//  ===========================================================================
class CScintillaCtrlEx : public CScintillaCtrl //Missing stubs from atlscintilla
{
public:
    int PositionAfter(int position)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        return ::SendMessage(m_hWnd, SCI_POSITIONAFTER, (WPARAM) position, 0L);
    }

    int PositionBefore(int position)
    {
        ATLASSERT(::IsWindow(m_hWnd));
        return ::SendMessage(m_hWnd, SCI_POSITIONBEFORE, (WPARAM) position, 0L);
    };
};
//  ===========================================================================
enum FOLD_STYLE
{
    FOLD_STYLE_VSNET, 
    FOLD_STYLE_VSNETR, 
    FOLD_STYLE_PLUS, 
    FOLD_STYLE_ARROW
};

enum INDICATOR
{
    INDICATOR_UNKNOWN = 0,
    INDICATOR_MISSING, 
    INDICATOR_DIFFERENT,
    INDICATOR_LAST
};

enum PANE   //  See "arrParts" in mainfrm.cpp
{
    PANE_DEFAULT = ID_DEFAULT_PANE,
    PANE_POS,
    PANE_USER,
    PANE_LABEL,
    PANE_SERVERSION,
    PANE_CCVERSION,
    PANE_QUEUE,
    PANE_CLUSTER,
    PANE_LAST
};

enum TARGET_TYPE
{
    TARGET_UNKNOWN = 0,
    TARGET_THOR = 2,
    TARGET_HTHOR = 3,
    TARGET_ROXIE = 4,
    TARGET_LOCAL = 5
};

__interface ISourceSlot
{
    void UIUpdateTitle();
    void LinesAdded(int lines);
    void DoCheckAttributeSyntax(const CString &attr, IAttributeType * type, const CSyntaxErrorVector & errors);
    void PostStatus(const TCHAR *status, PANE pane = PANE_DEFAULT);
    void RefreshSyntax(ISciSyntaxMarker *errors, size_t curErr, bool forceShow);
    void NewSel();
    void Close();
    const void GetCompletionList(const std::_tstring & module, StdStringVector &list);
};

class CSourceSlotImpl : public ISourceSlot
{
public:
    void UIUpdateTitle() {}
    void LinesAdded(int) {}
    void DoCheckAttributeSyntax(const CString &, IAttributeType *, const CSyntaxErrorVector & /*errors*/) {}
    void PostStatus(const TCHAR * /*status*/, PANE pane = PANE_DEFAULT) {(pane);}
    void RefreshSyntax(ISciSyntaxMarker * /*errors*/, size_t /*curErr*/, bool /*forceShow*/) {}
    void NewSel() {}
    void Close() {}
    const void GetCompletionList(const std::_tstring & module, StdStringVector &list) {}
};

class CMacroEvent : public CUnknown
{
public:
    int m_message;
    uptr_t m_wParam;
    std::string m_lParam;

    BEGIN_CUNKNOWN
    END_CUNKNOWN(CUnknown)

    CMacroEvent(int message, uptr_t wParam, sptr_t lParam)
    {
        m_message = message;
        m_wParam = wParam;
        if (lParam)
            m_lParam = (const char *)lParam;
    }
};
typedef StlLinked<CMacroEvent> CMacroEventAdapt;
typedef std::vector<CMacroEventAdapt> CMacroEventVector;

class CSourceCtrl : public CWindowImpl<CSourceCtrl, CScintillaCtrlEx, CScintillaWinTraits>,
    public CScintillaCommands<CSourceCtrl>,
    public CSciSyntaxMarker<CSourceCtrl>, 
    public CSciTeExtension<CSourceCtrl>
{
    typedef CSourceCtrl thisClass;
    typedef CWindowImpl<CSourceCtrl, CScintillaCtrlEx, CScintillaWinTraits> baseClass;
    typedef CSciTeExtension<CSourceCtrl> baseClass2;

protected:
    static HMODULE hScintilla;
    bool m_modified;
    StlLinked<ILangRef> m_langRef;
    CComPtr<IAttributeType> m_type;
    IAttribute *m_attribute;
    CSourceCtrl * m_other;

    CMacroEventVector m_macro;
    bool m_recording;
    char m_addedChar;

    TARGET_TYPE m_targetType;

public:
    ISourceSlot * m_owner;
    std::string m_WordCharacters;
    CSourceCtrl(const AttrInfo & attrInfo, ISourceSlot * owner);
    CSourceCtrl(ISourceSlot * owner);

    DECLARE_WND_SUPERCLASS(NULL, baseClass::GetWndClassName())

    void SetAttribute(IAttribute * attr);
    void SetType(IAttributeType * type);
    void SetOther(CSourceCtrl * other);
    void SetSourceType(Topology::ICluster * cluster);

    BOOL PreTranslateMessage(MSG* pMsg);

    void StartRecording();
    void StopRecording();
    void ResumeRecording();
    void PlaybackRecording();
    bool IsRecording();
    bool HasRecording();

    virtual int HandleNotify(SCNotification *notification);

    BEGIN_MSG_MAP_EX(thisClass)
        MSG_WM_DESTROY(OnDestroy)
        MSG_OCM_NOTIFY(OnNotify)
        MESSAGE_HANDLER_EX(EM_SETSEL, OnSetSel)
        CHAIN_MSG_MAP_ALT(CScintillaCommands<CSourceCtrl>, 1)
        CHAIN_MSG_MAP_ALT(CSciTeExtension<CSourceCtrl>, 1)
        DEFAULT_REFLECTION_HANDLER()
        MSG_WM_SETFOCUS(OnSetFocus)
        MSG_WM_KILLFOCUS(OnKillFocus)
    END_MSG_MAP()

    void OnDestroy();
    void OnSetFocus(HWND hWndOther);
    void OnKillFocus(HWND hWndOther);
    LRESULT OnNotify(int wParam, LPNMHDR lParam);
    LRESULT OnSetSel(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void Reformat();

    void DoInit();
    void InitLanguage(IAttributeType *attrType = NULL);
    void InitColors(ILangRef * langRef);

    int GetText(CString & pstrText) const;
    bool IsTextSelected();
    IAttribute *GetAttribute();
    unsigned GetPosition(unsigned lineNo, unsigned column);
    int SetSelLineCol(int lineNo, int colNo);
    void SetSelLine(int lineNo);
    void SetSelLine(int lineNo, int startAtCol);
    void ClearSel();
    void SelectWordAt(int lineNo, int column);
    void SetFoldStyle(FOLD_STYLE style);
    bool StartAutoComplete();

    bool DoFind(const std::_tstring & findText, DWORD flags, FINDMODE findmode, BOOL bAll = FALSE, BOOL bNext = TRUE, BOOL bFindWrap = FALSE);
    bool DoReplaceCurrent(const std::_tstring & findText, const CString & replaceText, DWORD flags, BOOL matchCase, BOOL bNext = TRUE);
    bool DoReplaceAll(const std::_tstring & findText, const CString & replaceText, DWORD flags);
    static const TCHAR* RFind (const TCHAR* str, const TCHAR ch);
    static const TCHAR* RFind (const TCHAR* str, const TCHAR* ss);

    bool IsDirty();
    void SetDirty(bool flag);

    virtual bool SaveFile(const CString & filename);
    virtual bool OpenFile(const CString & filename);

    void SetStyle(signed style, unsigned fore, unsigned back=0xffffff, const std::_tstring & font = _T(""), signed size=10, bool bold=false);
    static bool IsWordCharForSelWithPeriodECL(char ch);
    static bool IsWordCharForSelWithPeriod(char ch);
    static bool IsWordCharForSelNoPeriod(char ch);
    static bool IsWordCharForSelNoPeriodPlusHash(char ch);

    void UpdateStatusBar(bool closing = false);

protected:
    void SetMarker(int marker, int markerType, COLORREF fore, COLORREF back);
};
//  ===========================================================================
template<typename T>
class CSourceDlgImpl:
    public WTL::CDialogImpl<T>
{
    typedef T thisClass;
    typedef WTL::CDialogImpl<T> baseClass;

public:
    CSourceCtrl m_view;

    CSourceCtrl & GetView()
    {
        return m_view;
    }
    CSourceDlgImpl(const AttrInfo & attrInfo, ISourceSlot * owner) : m_view(attrInfo, owner)
    {
    }

    CSourceDlgImpl(ISourceSlot * owner) : m_view(owner)
    {
    }

    BOOL PreTranslateMessage(MSG* pMsg)
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

    void SetSource(const CString & ecl, bool resetSavePoint, bool clearUndo)
    {
        m_view.SetText(ecl);
        if (resetSavePoint)
        {
            ResetSavePoint(clearUndo);
        }
        else if(clearUndo)
        {
            m_view.EmptyUndoBuffer();
        }
    }

    void GetSource(CString & txt, bool selected = false)
    {
        if (selected)
            m_view.GetSelText(txt);
        else
            m_view.GetText(txt);
    }

    void SetSource(const CString & txt)
    {
        m_view.SetText(txt);
    }

    void SetSyntax(const CSyntaxErrorVector & errors)
    {
        m_view.SyntaxSet(errors);
    }

    void SetReadOnly(bool readOnly=true)
    {
        int targetOn = static_cast<int>(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_TARGETCOLOR));
        if (readOnly && targetOn == 0)
        {
            ILangRef * langRefGeneral = GetLangRef(ATTRIBUTE_TYPE_GENERAL);
            int color = langRefGeneral->GetColorBack(SCE_GEN_READONLY_BACKGROUND);
            m_view.StyleSetBack(STYLE_DEFAULT, color);
            for (int i = SCE_ECL_DEFAULT; i <= SCE_ECL_LAST; ++i)
                m_view.StyleSetBack(i, color);
        }
        m_view.SetReadOnly(readOnly);
    }

    void ResetSavePoint(bool emptyUndoBuffer = false)
    {
        if (emptyUndoBuffer)
            m_view.EmptyUndoBuffer();
        m_view.SetSavePoint();
    }

    bool IsDirty() 
    {
        return m_view.IsDirty();
    }

    void ShowLastError(const CString & sMessage, DWORD dwError)
    {
        ATLTRACE( _T("Entering : ShowLastError ( %s, 0x%x )\n" ), sMessage, dwError);
        
        CString sErrorMessage;
        sErrorMessage = sMessage;
        
        // get the error message
        LPTSTR lpMessageBuffer;
        ::FormatMessage ( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                        NULL, dwError,
                        MAKELANGID ( LANG_ENGLISH, SUBLANG_ENGLISH_US ),
                        (LPTSTR)&lpMessageBuffer, 0, NULL);
        sErrorMessage += lpMessageBuffer;
        ::LocalFree ( lpMessageBuffer ); // FormatMessage() requires this
        
        // display the error message
        CString title;
        title.LoadString(IDR_MAINFRAME);
        ::MessageBox ( NULL, sErrorMessage, title, MB_OK | MB_ICONEXCLAMATION );
    }

    void GetWordAtCurPos(CString & result)
    {
        int startPos = m_view.GetCurrentPos();
        int endPos = m_view.GetCurrentPos();
        int lengthDoc = m_view.GetLength();
        m_view.RangeExtendAndGrab(result, startPos, endPos, lengthDoc, m_view.IsWordCharForSelWithPeriod);
    }

    void GetWordAtCurPosNoPeriodPlusHash(CString & result)
    {
        int startPos = m_view.GetCurrentPos();
        int endPos = m_view.GetCurrentPos();
        int lengthDoc = m_view.GetLength();
        m_view.RangeExtendAndGrab(result, startPos, endPos, lengthDoc, m_view.IsWordCharForSelNoPeriodPlusHash);
    }

    BEGIN_MSG_MAP(thisClass)
        MSG_WM_INITDIALOG(OnInitDialog)
        MESSAGE_HANDLER_EX(CWM_INITIALIZE, OnInitialize)
        REFLECT_NOTIFICATIONS()
    END_MSG_MAP()

    LRESULT OnInitDialog(HWND /*hWnd*/, LPARAM /*lParam*/)
    {
        m_view.SubclassWindow(GetDlgItem(IDC_EDIT_ECL));
        m_view.DoInit();
        SendMessage(CWM_INITIALIZE);
        return 0;
    }

    LRESULT OnInitialize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
    {
        m_view.GrabFocus();
        return 0;
    }
};
//  ===========================================================================

