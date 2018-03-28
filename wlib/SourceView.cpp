#include "stdafx.h"
#include "..\en_us\resource.h"

#include "SourceView.h"
#include "EclCC.h"
#include "AttributeType.h"
#include "UnicodeFile.h"
#include "util.h"
#include "eclparser.h"
#include "preferencedlg.h"
#include "SciLexer.h"

const char * const wordchars = "_0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char * const wordcharsWithDot = "._0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

static char *folder_xpm[] =
{
    "16 16 8 1",
    " 	c None",
    ".	c #909000",
    "+	c #000000",
    "@	c #EFE8EF",
    "#	c #FFF8CF",
    "$	c #FFF890",
    "%	c #CFC860",
    "&	c #FFC890",
    "                ",
    "  .....+        ",
    " .@##$$.+       ",
    ".%%%%%%%......  ",
    ".###########$%+ ",
    ".#$$$$$$$$$$&%+ ",
    ".#$$$$$$$&$&$%+ ",
    ".#$$$$$$$$&$&%+ ",
    ".#$$$$$&$&$&$%+ ",
    ".#$$$$$$&$&$&%+ ",
    ".#$$$&$&$&$&&%+ ",
    ".#&$&$&$&$&&&%+ ",
    ".%%%%%%%%%%%%%+ ",
    " ++++++++++++++ ",
    "                ",
    "                "};

static char * World_xpm[] = {
"20 20 32 1",
" 	c None",
".	c #B6B6B6",
"+	c #AAAAAA",
"@	c #868686",
"#	c #6E6E6E",
"$	c #4A4A4A",
"%	c #323232",
"&	c #000000",
"*	c #565656",
"=	c #006296",
"-	c #CECECE",
";	c #FFFFFF",
">	c #007AB9",
",	c #0092DC",
"'	c #48B8FF",
")	c #007373",
"!	c #00734A",
"~	c #00AAFF",
"{	c #8ED4FF",
"]	c #25AAFF",
"^	c #00B9B9",
"/	c #005050",
"(	c #DADADA",
"_	c #C5AA00",
":	c #626200",
"<	c #4A4A00",
"[	c #323200",
"}	c #3E3E3E",
"|	c #B1FFFF",
"1	c #48FFFF",
"2	c #004A73",
"3	c #003250",
"     ..+@##@+..     ",
"    .@$%&**&%$@.    ",
"   .*&=-;;-;>=&*.   ",
"  .*&,,';-;-'))&*.  ",
" .*&!~{{']']^///&*. ",
".@&!^;;'](((!_::<&@.",
".$!^.;']';;((_::<[$.",
"+%:'{']']]]^!_::<[%+",
"@&:...((((^!:_:<<[&@",
"#}<[.(;|||11_::<[>2#",
"#}<..((((],!:::<>>2#",
"@&@.((],],]/::<,>>&@",
"+%@.,],],],,/:<>>=%+",
".$[,,,,,,,,,,/<>=3$.",
".@&,>,@,>,>,@>/23&@.",
" .*&3@.@>>>.;@23&*. ",
"  .*&3%..;..@23&*.  ",
"   .*&3%;;;.@3&*.   ",
"    .@$%&%%&%$@.    ",
"     ..+@##@+..     "};

HMODULE CSourceCtrl::hScintilla = 0;

CSourceCtrl::CSourceCtrl(const AttrInfo & attrInfo, ISourceSlot * owner) : m_owner(owner), m_modified(false)
{
    if (!hScintilla)
        hScintilla = ::LoadLibrary(CScintillaCtrlEx::GetLibraryName());
    ATLASSERT(hScintilla);
    if (static_cast<bool>(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_DOUBLECLICKSELQUAL)) == true)
        m_WordCharacters = wordcharsWithDot;
    else
        m_WordCharacters = wordchars;
    
    if (attrInfo.AttributeType.length())
        m_type = AttributeTypeFromExtension(attrInfo.AttributeType);
    else
        m_type = CreateIAttributeECLType();

    m_other = NULL;
    m_recording = false;
    m_addedChar = '\0';
    m_targetType = TARGET_UNKNOWN;
}

CSourceCtrl::CSourceCtrl(ISourceSlot * owner) : m_owner(owner), m_modified(false)
{
    AttrInfo attrInfo;
    CSourceCtrl(attrInfo, owner);
}

void CSourceCtrl::SetSourceType(const CString & typeStr)
{
    CString typeLower = typeStr;
    typeLower.MakeLower();
    if (typeLower.Find(_T("roxie"),0) >= 0)
    {
        m_targetType = TARGET_ROXIE;
    }
    else if (typeLower.Find(_T("hthor"), 0) >= 0)
    {
        m_targetType = TARGET_HTHOR;
    }
    else if (typeLower.Find(_T("local"), 0) >= 0)
    {
        m_targetType = TARGET_LOCAL;
    }
    else if (typeLower.Find(_T("thor"), 0) >= 0)
    {
        m_targetType = TARGET_THOR;
    }
    else
    {
        m_targetType = TARGET_UNKNOWN;
    }
}

void CSourceCtrl::SetAttribute(IAttribute * attr)
{
    ATLASSERT(attr);
    m_attribute = attr;
    SetType(m_attribute->GetType());
}

void CSourceCtrl::SetType(IAttributeType * type)
{
    ATLASSERT(type);
    m_type = type;
}

void CSourceCtrl::SetOther(CSourceCtrl * other)
{
    m_other = other;
}

BOOL CSourceCtrl::PreTranslateMessage(MSG* /*pMsg*/)
{
    //if (__super::PreTranslateMessage(pMsg))
    //	return true;
    return FALSE;
}

void CSourceCtrl::StartRecording()
{
    m_macro.clear();
    StartRecord();
    m_recording = true;
}

void CSourceCtrl::StopRecording()
{
    m_recording = false;
    StopRecord();
}

void CSourceCtrl::ResumeRecording()
{
    StartRecord();
    m_recording = true;
}

void CSourceCtrl::PlaybackRecording()
{	
    BeginUndoAction();
    for (CMacroEventVector::const_iterator itr = m_macro.begin(); itr != m_macro.end(); ++itr)
    {   
        SendMessage(itr->get()->m_message, itr->get()->m_wParam, (LPARAM)itr->get()->m_lParam.c_str());
    }
    EndUndoAction();
}

bool CSourceCtrl::IsRecording()
{
    return m_recording;
}

bool CSourceCtrl::HasRecording()
{
    return m_macro.size() > 0;
}

void CSourceCtrl::OnDestroy()
{
    SetMsgHandled(false);
}

void CSourceCtrl::OnSetFocus(HWND /*hWndOther*/)
{
    UpdateStatusBar();
    SetMsgHandled(false);
}

void CSourceCtrl::OnKillFocus(HWND /*hWndOther*/)
{
    UpdateStatusBar(true);
    SetMsgHandled(false);
}

LRESULT CSourceCtrl::OnNotify(int /*wParam*/, LPNMHDR lParam)
{
    SetMsgHandled(false);
    if (lParam->hwndFrom == *this)
    {
        if (m_other)
        {
            ATLASSERT(m_other->IsWindow() && IsWindow());
            int otherFirstLine = m_other->GetFirstVisibleLine();
            int thisFirstLine = GetFirstVisibleLine();
            if (otherFirstLine != thisFirstLine)
                m_other->LineScroll(0, thisFirstLine - otherFirstLine);

            int otherXPos = m_other->GetXOffset();
            int thisXPos = GetXOffset();
            if (otherXPos != thisXPos)
                m_other->SetXOffset(thisXPos);

            int otherZoom = m_other->GetZoom();
            int thisZoom = GetZoom();
            if (otherZoom != thisZoom)
                m_other->SetZoom(thisZoom);

            //Scintilla::CharacterRange otherSel = m_other->GetSelection();
            //Scintilla::CharacterRange thisSel = GetSelection();
            //if (otherSel.cpMin != thisSel.cpMin || otherSel.cpMax != thisSel.cpMax)
            //	m_other->SetSel(thisSel);
        }
        HandleNotify((Scintilla::SCNotification *)lParam);
    }
    return 0;
}

void CSourceCtrl::SelectWordAt(int lineNo, int column)
{
    unsigned startPos = GetPosition(lineNo, column);
    SetSel(startPos, WordEndPosition(startPos, true));
}

int CSourceCtrl::HandleNotify(Scintilla::SCNotification *notification)
{
    if (!notification)
        return 0;

    switch (notification->nmhdr.code)
    {
    case SCEN_SETFOCUS:
    case SCEN_KILLFOCUS:
        break;
    case SCN_CHARADDED:
        m_addedChar = static_cast<char>(notification->ch);
        break;
    case SCN_UPDATEUI:
        if (m_addedChar != '\0' && !IsRecording())
        {
            CharAdded(m_addedChar);
            m_addedChar = '\0';
        }
        MatchBrace();
        UpdateStatusBar();
        break;
    case SCN_MODIFIED:
        if ( !m_modified && (notification->modificationType&(SC_MOD_INSERTTEXT|SC_MOD_DELETETEXT)))
        {
            m_modified = true;
            m_owner->UIUpdateTitle();
        }
        if (0 != (notification->modificationType & SC_MOD_CHANGEFOLD)) 
            FoldChanged(notification->line, notification->foldLevelNow, notification->foldLevelPrev);
        if ((notification->modificationType & SC_MOD_INSERTTEXT) || (notification->modificationType & SC_MOD_DELETETEXT))
            m_owner->LinesAdded(notification->linesAdded);
        break;
    case SCN_MARGINCLICK: 
        if (notification->margin == 2) 
            MarginClick(notification->position, notification->modifiers);
        break;
    case SCN_NEEDSHOWN: 
        EnsureRangeVisible(notification->position, notification->position + notification->length, false);
        break;
    case SCN_ZOOM:
        SetLineNumberWidth();
        break;
    case SCN_SAVEPOINTREACHED:
        m_modified = false;
        m_owner->UIUpdateTitle();
        break;
    case SCN_SAVEPOINTLEFT:
        m_modified = true;
        m_owner->UIUpdateTitle();
        break;
    case SCN_MACRORECORD:
        m_macro.push_back(new CMacroEvent(notification->message, notification->wParam, notification->lParam));
        break;
    default:
        break;
    }
    if (static_cast<bool>(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_TOOLTIP)) == true)
    {
        switch (notification->nmhdr.code)
        {
        case SCN_DWELLSTART: 
            {
                char message[200];
                if (INVALID_POSITION == notification->position) 
                {
                    sprintf(message, "%0d (%0d,%0d)", notification->position, notification->x, notification->y);
                } 
                else 
                {
                    int startPos = notification->position;
                    int endPos = notification->position;
                    int lengthDoc = GetLength();
                    CString _message;
                    RangeExtendAndGrab(_message, startPos, endPos, lengthDoc, IsWordCharForSelWithPeriod);
                    if (_message.GetLength() > 0)
                    {
                        std::_tstring tooltip = m_langRef->GetLangTooltip(static_cast<const TCHAR *>(_message));
                        if (tooltip.length())
                            CallTipShow(startPos, tooltip.c_str());
                    }
                }
            }
            break;
        case SCN_DWELLEND:
            CallTipCancel();
            break;
        default:
            break;
        }
    }
    return 0;//baseClass::HandleNotify(lParam);
}

LRESULT CSourceCtrl::OnSetSel(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    return 0;
}

void CSourceCtrl::Reformat()
{
    if (static_cast<bool>(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_DISABLEINVOKE)) == false)
    {
        CString ecl;
        GetText(ecl);
        StlLinked<IEclProgram> program = ParseEcl(ecl);
        if (program.get())
        {
            {
                std::_tstring _ecl;
                ecl = program->SerializeToXml(_ecl);
                SetText(ecl);
            }

            {
                std::_tstring _ecl;
                ecl = program->Serialize(_ecl);
                SetText(ecl);
            }
        }
    }
}

void CSourceCtrl::DoInit()
{
    StyleResetDefault();
    SetStyle(STYLE_DEFAULT, 0x0);
    StyleClearAll();

    InitLanguage();

    SetTabWidth((int)GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_TAB_WIDTH));
    SetUseTabs(!(bool)GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_TAB_USESPACES));

    if (GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_LINENO))
        SetMarginWidthN(0, 32);

    SetViewWS((bool)GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SHOW_WS));

    if (GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_TREE))
    {
        SetMarginSensitiveN(1, true);

        SetMarginWidthN(2, 12);
        SetMarginTypeN(2, SC_MARGIN_SYMBOL);
        SetMarginMaskN(2, SC_MASK_FOLDERS);
        SetMarginSensitiveN(2, true);

        SetFoldStyle(FOLD_STYLE_VSNET);
        SetProperty(_T("fold"), _T("1"));
    }

    MarkerDefine(MARKER_ERROR, SC_MARK_EMPTY);
    MarkerDefine(MARKER_WARNING, SC_MARK_EMPTY);
    MarkerDefine(MARKER_ARROW, SC_MARK_ARROW);
    MarkerDefine(MARKER_ADDED, SC_MARK_CIRCLEPLUS);
    MarkerDefine(MARKER_DELETED, SC_MARK_CIRCLEMINUS);
    MarkerDefine(MARKER_MODIFIED, SC_MARK_CIRCLE);

    MarkerDefine(MARKER_CIRCLEMINUS, SC_MARK_CIRCLEMINUS);
    MarkerSetFore(MARKER_CIRCLEMINUS, 0x0000ff);
    MarkerDefine(MARKER_CIRCLE, SC_MARK_CIRCLE);
    MarkerSetFore(MARKER_CIRCLE, 0x0000ff);
    MarkerDefine(MARKER_CIRCLEPLUS, SC_MARK_CIRCLEPLUS);
    MarkerSetFore(MARKER_CIRCLEPLUS, 0x0000ff);

    RegisterImage(1, (LPBYTE)&folder_xpm);

    IndicSetStyle(0, INDIC_SQUIGGLE);//INDIC_TT, INDIC_DIAGONAL INDIC_STRIKE, INDIC_SQUIGGLE
    IndicSetFore(0, 0x0000FF);

    IndicSetStyle(1, INDIC_SQUIGGLE);//INDIC_TT, INDIC_DIAGONAL INDIC_STRIKE, INDIC_SQUIGGLE
    IndicSetFore(1, 0x007f00);

#ifdef _UNICODE	
    SetCodePage(SC_CP_UTF8);
#endif
    //SetEOLMode(SC_EOL_CRLF);
    UsePopUp(false);

    AutoCSetIgnoreCase(m_autoCompleteIgnoreCase);
    AutoCSetDropRestOfWord(true);
    AutoCSetMaxWidth(400);
    AutoCSetMaxHeight(10);

    WTL::CToolTipCtrl tool;
    tool.Create(m_hWnd);
    SetMouseDwellTime(tool.GetDelayTime(TTDT_INITIAL));

    SetWordChars(CA2T(m_WordCharacters.c_str()));

    SetWrapVisualFlags(SC_WRAPVISUALFLAG_END);

    SetScrollWidthTracking(true);

    SetPasteConvertEndings(true);

    SetFoldFlags(SC_FOLDFLAG_LINEAFTER_CONTRACTED);
}

void CSourceCtrl::InitLanguage(IAttributeType *attrType)
{
    if (attrType) {
        SetType(attrType);
        SetSourceType(_T(""));
    }
    else if (!m_type)
    {
        return;
    }
    m_langRef = CreateLangRef(m_type);
    SetLexer(m_langRef->GetLexerType());

    CString names;
    for (int i = 1; i <= m_langRef->GetLangCatCount(); ++i)
        SetKeyWords(i - 1, m_langRef->GetLangNames(i, names));

    InitColors(m_langRef);
}

void CSourceCtrl::InitColors(ILangRef * langRef)
{
    int backID;
    bool generalFlag = langRef->GetElementType().CompareNoCase(ATTRIBUTE_TYPE_GENERAL) == 0;
    ILangRef * langRefGeneral = GetLangRef(ATTRIBUTE_TYPE_GENERAL);
    int targetOn = static_cast<int>(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_TARGETCOLOR));

    for(int row = 0; row < langRef->GetColorRowCount(); ++row)
    {
        ATLTRACE(_T("Row:  %i\n"), row);
        int catID = backID = langRef->GetColorCatID(row);
        int backColor = langRef->GetColorBack(backID);
        if (targetOn && m_targetType != TARGET_UNKNOWN) {
            backColor = langRefGeneral->GetColorBack(m_targetType);
        }
        langRef->GetElementType();
        if (generalFlag && catID == 1)
        {
            SetCaretFore(langRef->GetColorFore(catID));
        }

        SetStyle(catID, langRef->GetColorFore(catID), backColor, langRef->GetFontName(catID), langRef->GetFontSize(catID), langRef->GetFontBold(catID));
    }
    AnnotationSetVisible(ANNOTATION_BOXED);
}

bool CSourceCtrl::IsWordCharForSelWithPeriodECL(char ch) 
{
    return (IsWordCharForSelWithPeriod(ch) || ch == '$' || ch == '-');
}

bool CSourceCtrl::IsWordCharForSelWithPeriod(char ch)
{
    return (strchr(wordchars, ch) != NULL || ch == '.');
}

bool CSourceCtrl::IsWordCharForSelNoPeriod(char ch) 
{
    if (ch == '.')
        return false;
    return (strchr(wordchars, ch) != NULL);
}

bool CSourceCtrl::IsWordCharForSelNoPeriodPlusHash(char ch) {
    if (ch == '.')
        return false;
    return (strchr(wordchars, ch) != NULL || ch == '#');
}

const TCHAR * wildcardToRegex(const std::_tstring & wildcard, std::_tstring & regex)
{
    //regex = _T("^");

    for (unsigned int i = 0; i < wildcard.length(); ++i) 
    {
        switch (wildcard[i])
        {
        case _T('*'):
            regex += _T(".*");
            break;
        case _T('?'):
            regex += _T('.');
            break;
        case _T('+'):
        case _T('('):
        case _T(')'):
        case _T('^'):
        case _T('$'):
        case _T('.'):
        case _T('{'):
        case _T('}'):
        case _T('['):
        case _T(']'):
        case _T('|'):
        case _T('\\'):
            regex += _T('\\');
            regex += wildcard[i];
            break;
        default:
            regex += wildcard[i];
            break;
        }
    }

    //regex += _T("$");
    return regex.c_str();
} 

bool CSourceCtrl::DoFind(const std::_tstring & _searchTerm, DWORD flags, FINDMODE findmode, BOOL bAll, BOOL bNext, BOOL bFindWrap)
{
    Scintilla::CharacterRange chrg = GetSelection();
    Scintilla::TextToFind ft;

    if (bNext) 
    {
        ft.chrg.cpMin = bAll ? 0 : chrg.cpMax;
        ft.chrg.cpMax = GetLength();
    } 
    else
    {
        ft.chrg.cpMin = bAll ? GetLength() : chrg.cpMin - 1;
        ft.chrg.cpMax = 0;
    }

    std::_tstring searchTerm;
    switch (findmode)
    {
    case FINDMODE_WILDCARD:
        wildcardToRegex(_searchTerm, searchTerm);
        flags = SCFIND_REGEXP;
        break;
    case FINDMODE_REGEX:
        searchTerm = _searchTerm;
        flags = SCFIND_REGEXP;
        break;
    default:
        searchTerm = _searchTerm;
        break;
    }

    std::string findStr = CT2A(searchTerm.c_str(), CP_UTF8);
    ft.lpstrText = const_cast<char *>(findStr.c_str());

    if (_FindText(flags, &ft) == -1)
    {
        if (bFindWrap)
        {
            if (bNext)
            {
                ft.chrg.cpMin = 0;
                ft.chrg.cpMax = GetLength();
            }
            else
            {
                ft.chrg.cpMin = GetLength();
                ft.chrg.cpMax = 0;
            }

            if (_FindText(flags, &ft) == -1)
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    
    SetSel(ft.chrgText.cpMin, ft.chrgText.cpMax);
    return true;
}

bool CSourceCtrl::DoReplaceCurrent(const std::_tstring & findText, const CString & replaceText, DWORD flags, BOOL matchCase, BOOL bNext)
{
    Scintilla::CharacterRange chrg = GetSelection();

    if(chrg.cpMin != chrg.cpMax)
    {
        CString text;
        GetSelText(text);
        int nRet;
        if(matchCase)
            nRet = _tcscmp(text, findText.c_str());
        else
            nRet = _tcsicmp(text, findText.c_str());
        if(nRet == 0)
            ReplaceSel(replaceText);
    }

    return DoFind(findText, flags, FINDMODE_NONE, false, bNext);	
}

bool CSourceCtrl::DoReplaceAll(const std::_tstring & findText, const CString & replaceText, DWORD flags)
{
    CWaitCursor wait;
    if(!DoFind(findText, flags, FINDMODE_NONE, true))
        return false;

    SetRedraw(FALSE);
    BeginUndoAction();
    do 
    {
        ReplaceSel(replaceText);
    } while(DoFind(findText, flags, FINDMODE_NONE));
    EndUndoAction();
    SetRedraw(TRUE);
    Invalidate();
    UpdateWindow();

    return true;
}

int CSourceCtrl::GetText(CString & result) const
{
    int retVal = baseClass::GetText(GetTextLength() + 1, result);
    return retVal;
}

bool CSourceCtrl::IsTextSelected()
{
    Scintilla::CharacterRange chrg = GetSelection();
    return chrg.cpMax == chrg.cpMin ? false : true;
}

unsigned CSourceCtrl::GetPosition(unsigned lineNo, unsigned column)
{
    return PositionFromLine(lineNo) + column;
}

int CSourceCtrl::SetSelLineCol(int lineNo, int colNo)
{
    Scintilla::CharacterRange cr;
    cr.cpMin = GetPosition(lineNo, colNo);
    cr.cpMax = cr.cpMin;
    baseClass::SetSel(cr);
    return cr.cpMin;
}

void CSourceCtrl::SetSelLine(int lineNo)
{
    Scintilla::CharacterRange cr;
    cr.cpMin = GetPosition(lineNo, 0);
    cr.cpMax = cr.cpMin + GetLineLength(lineNo);
    baseClass::SetSel(cr);
    SetXOffset(0);
}

void CSourceCtrl::SetSelLine(int lineNo, int startAtCol)
{
    Scintilla::CharacterRange cr;
    cr.cpMin = GetPosition(lineNo, 0);
    cr.cpMax = cr.cpMin + GetLineLength(lineNo);
    cr.cpMin += startAtCol > 0 ? startAtCol - 1 : 0;
    baseClass::SetSel(cr);
    SetXOffset(0);
}

void CSourceCtrl::ClearSel()
{
    int xpos = GetXOffset();
    Scintilla::CharacterRange cr;
    cr.cpMin = baseClass::GetCurrentPos();
    cr.cpMax = cr.cpMin;
    baseClass::SetSel(cr);
    SetXOffset(xpos);
}

const TCHAR* CSourceCtrl::RFind (const TCHAR* str, const TCHAR ch)
{
    TCHAR* p = (TCHAR*)str;	
    while (*p++);
    while (--p != str)
        if(*p == ch) 
            return p;	
    return 0;
}

const TCHAR* CSourceCtrl::RFind (const TCHAR* str, const TCHAR* ss)
{
    TCHAR* p = (TCHAR*)str;	
    while (*p++);
    while (--p != str) {
        const TCHAR* p1 = ss;
        while(*p1) { 
            if(*p1 == *p) {
                return p;
            }
            p1++;
        }
    }
    return 0;
}

bool CSourceCtrl::IsDirty()
{
    return m_modified;
}

void CSourceCtrl::SetDirty(bool flag)
{
    m_modified = flag;
}

const int blockSize = 131072;

bool CSourceCtrl::SaveFile(const CString & filename)
{
    CString text;
    GetText(text);
    CUnicodeFile file;
    if (file.Create(filename))
    {
        file.Write(text);
        return true;
    }

    return false;
}

bool CSourceCtrl::OpenFile(const CString & filename)
{
    if (boost::algorithm::iends_with(static_cast<const TCHAR *>(filename), _T(".dll")))
    {
        std::_tstring ecl;
        if (GetPluginECL(static_cast<const TCHAR *>(filename), ecl))
        {
            SetText(ecl.c_str());
            return true;
        }
        else
        {
            SetText(_T("Unable to extract ECL definition."));
        }
    }
    else 
    {
        std::_tstring text;
        CUnicodeFile file;
        if (file.Open(filename))
        {
            file.Read(text);
            SetText(text.c_str());
            return true;
        }
    }
    return false;
}

void CSourceCtrl::SetFoldStyle(FOLD_STYLE style)
{
    switch (style)
    {
    case FOLD_STYLE_VSNET:
        {
            SetMarker(SC_MARKNUM_FOLDEROPEN, SC_MARK_BOXMINUS, RGB(0xff, 0xff, 0xff), RGB(0x80, 0x80, 0x80));
            SetMarker(SC_MARKNUM_FOLDER, SC_MARK_BOXPLUS, RGB(0xff, 0xff, 0xff), RGB(0x80, 0x80, 0x80));
            SetMarker(SC_MARKNUM_FOLDERSUB, SC_MARK_VLINE, RGB(0xff, 0xff, 0xff), RGB(0x80, 0x80, 0x80));
            SetMarker(SC_MARKNUM_FOLDERTAIL, SC_MARK_LCORNER, RGB(0xff, 0xff, 0xff), RGB(0x80, 0x80, 0x80));
            SetMarker(SC_MARKNUM_FOLDEREND, SC_MARK_BOXPLUSCONNECTED, RGB(0xff, 0xff, 0xff), RGB(0x80, 0x80, 0x80));
            SetMarker(SC_MARKNUM_FOLDEROPENMID, SC_MARK_BOXMINUSCONNECTED, RGB(0xff, 0xff, 0xff), RGB(0x80, 0x80, 0x80));
            SetMarker(SC_MARKNUM_FOLDERMIDTAIL, SC_MARK_TCORNER, RGB(0xff, 0xff, 0xff), RGB(0x80, 0x80, 0x80));
        }
        break;
    case FOLD_STYLE_VSNETR:
        {
            SetMarker(SC_MARKNUM_FOLDEROPEN, SC_MARK_CIRCLEMINUS, RGB(0xff, 0xff, 0xff), RGB(0x40, 0x40, 0x40));
            SetMarker(SC_MARKNUM_FOLDER, SC_MARK_CIRCLEPLUS, RGB(0xff, 0xff, 0xff), RGB(0x40, 0x40, 0x40));
            SetMarker(SC_MARKNUM_FOLDERSUB, SC_MARK_VLINE, RGB(0xff, 0xff, 0xff), RGB(0x40, 0x40, 0x40));
            SetMarker(SC_MARKNUM_FOLDERTAIL, SC_MARK_LCORNERCURVE, RGB(0xff, 0xff, 0xff), RGB(0x40, 0x40, 0x40));
            SetMarker(SC_MARKNUM_FOLDEREND, SC_MARK_CIRCLEPLUSCONNECTED, RGB(0xff, 0xff, 0xff), RGB(0x40, 0x40, 0x40));
            SetMarker(SC_MARKNUM_FOLDEROPENMID, SC_MARK_CIRCLEMINUSCONNECTED, RGB(0xff, 0xff, 0xff), RGB(0x40, 0x40, 0x40));
            SetMarker(SC_MARKNUM_FOLDERMIDTAIL, SC_MARK_TCORNERCURVE, RGB(0xff, 0xff, 0xff), RGB(0x40, 0x40, 0x40));
        }
        break;
    case FOLD_STYLE_PLUS:
        {
            SetMarker(SC_MARKNUM_FOLDEROPEN, SC_MARK_MINUS, RGB(0xff, 0xff, 0xff), RGB(0, 0, 0));
            SetMarker(SC_MARKNUM_FOLDER, SC_MARK_PLUS, RGB(0xff, 0xff, 0xff), RGB(0, 0, 0));
            SetMarker(SC_MARKNUM_FOLDERSUB, SC_MARK_EMPTY, RGB(0xff, 0xff, 0xff), RGB(0, 0, 0));
            SetMarker(SC_MARKNUM_FOLDERTAIL, SC_MARK_EMPTY, RGB(0xff, 0xff, 0xff), RGB(0, 0, 0));
            SetMarker(SC_MARKNUM_FOLDEREND, SC_MARK_EMPTY, RGB(0xff, 0xff, 0xff), RGB(0, 0, 0));
            SetMarker(SC_MARKNUM_FOLDEROPENMID, SC_MARK_EMPTY, RGB(0xff, 0xff, 0xff), RGB(0, 0, 0));
            SetMarker(SC_MARKNUM_FOLDERMIDTAIL, SC_MARK_EMPTY, RGB(0xff, 0xff, 0xff), RGB(0, 0, 0));
        }
        break;

    case FOLD_STYLE_ARROW:
        //fall thru
    default:
        {
            SetMarker(SC_MARKNUM_FOLDEROPEN, SC_MARK_ARROWDOWN, RGB(0, 0, 0), RGB(0, 0, 0));
            SetMarker(SC_MARKNUM_FOLDER, SC_MARK_ARROW, RGB(0, 0, 0), RGB(0, 0, 0));
            SetMarker(SC_MARKNUM_FOLDERSUB, SC_MARK_EMPTY, RGB(0, 0, 0), RGB(0, 0, 0));
            SetMarker(SC_MARKNUM_FOLDERTAIL, SC_MARK_EMPTY, RGB(0, 0, 0), RGB(0, 0, 0));
            SetMarker(SC_MARKNUM_FOLDEREND, SC_MARK_EMPTY, RGB(0xff, 0xff, 0xff), RGB(0, 0, 0));
            SetMarker(SC_MARKNUM_FOLDEROPENMID, SC_MARK_EMPTY, RGB(0xff, 0xff, 0xff), RGB(0, 0, 0));
            SetMarker(SC_MARKNUM_FOLDERMIDTAIL, SC_MARK_EMPTY, RGB(0xff, 0xff, 0xff), RGB(0, 0, 0));
        }
        break;
    }
}

void CSourceCtrl::SetStyle(signed style, unsigned fore, unsigned back, const std::_tstring & font, signed _size, bool bold)
{
    CString face;
    if (font.length())
        face = font.c_str();
    else
        face = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_FONT);

    int size = _size >= 1 ? _size : GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_FONTSIZE);
    if (fore)
        StyleSetFore(style, fore);
    StyleSetBack(style, back);
    if (size >= 1)
        StyleSetSize(style, size);
    if (face)
        StyleSetFont(style, face);
    StyleSetBold(style, bold);
    StyleSetEOLFilled(style, true);
}

void CSourceCtrl::SetMarker(int marker, int markerType, COLORREF fore, COLORREF back)
{
    MarkerDefine(marker, markerType);
    MarkerSetFore(marker, fore);
    MarkerSetBack(marker, back);
}

bool CSourceCtrl::StartAutoComplete()
{
    CString word;
    int currentPos = GetCurrentPos();
    int currentCatID = GetStyleAt(currentPos);
    if (m_langRef->IsComment(currentCatID))
        return false;

    int startPos = currentPos;
    int endPos = currentPos;
    int lengthDoc = GetLength();
    RangeExtendAndGrab(word, startPos, endPos, lengthDoc, IsWordCharForSelWithPeriodECL);
    if (word.CompareNoCase(_T("l.")) != 0 &&
        word.CompareNoCase(_T("r.")) != 0 &&
        word.CompareNoCase(_T("left.")) != 0 &&
        word.CompareNoCase(_T("right.")) != 0 &&
        word.CompareNoCase(_T("self.")) != 0)
    {
        CWaitCursor wait;
        int periodPos = 0;
        for (int newPos = word.Find('.'); newPos >= 0; newPos = word.Find('.', newPos + 1))
        {
            periodPos = newPos;
        }
        StdStringVector list;
        if (periodPos == 0)
            m_langRef->GetLangNamesAutoC(1, list);

        std::_tstring  module(word.Mid(0, word.GetLength() - 1));
        if (m_type->IsTypeOf(ATTRIBUTE_TYPE_ECL))
        {
            if (IsLocalRepositoryEnabled() == TRI_BOOL_TRUE)
            {
                if (boost::algorithm::starts_with(static_cast<const TCHAR *>(word),_T("$")))
                {
                    boost::algorithm::replace_first(module, _T("$"), m_attribute->GetModuleQualifiedLabel(true));
                }
                m_owner->GetCompletionList(module, list);
            }
            else
            {
                m_langRef->GetAutoC(module, list);
            }
        }
        if (list.size())
        {
            std::_tstring listStr;
            SortedSerialize(list, listStr);
            AutoCShow(currentPos - (startPos + (periodPos == 0 ? 0 : periodPos + 1)), listStr.c_str());
        }
    }
    return false;
}

void CSourceCtrl::UpdateStatusBar(bool closing)
{
    if (closing)
    {
        m_owner->PostStatus(_T(""), PANE_POS);
    }
    else
    {
        int nPos = GetCurrentPos();
        int nRow = LineFromPosition(nPos);
        int nCol = GetColumn(nPos);
        std::_tstring pos = (boost::_tformat(_T("%1%, %2%")) % boost::lexical_cast<std::_tstring>(nRow + 1) % boost::lexical_cast<std::_tstring>(nCol + 1)).str();
        m_owner->PostStatus(pos.c_str(), PANE_POS);
    }
}


