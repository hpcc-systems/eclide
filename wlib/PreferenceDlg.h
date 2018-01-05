#pragma once

#include "wlib.h"
#include "Global.h"

const TCHAR g_szIniFile[] = _T("eclide.ini");

static const SectionLabelDefault GLOBAL_ACTIVEWORKUNIT_REFRESH(SectionLabel(_T("ActiveWorkunit"), _T("Refresh")), 10);

static const TCHAR GeneralSection[](_T("General"));
static const SectionLabelDefault GLOBAL_OPENMDICHILDMAX(SectionLabel(GeneralSection, _T("OpenWindowMax")), 0);
static const SectionLabelDefault GLOBAL_AUTOSAVEFREQ(SectionLabel(GeneralSection, _T("AutoSaveFreq")), 10);
static const SectionLabelDefault GLOBAL_AUTOOPENRESULT(SectionLabel(GeneralSection, _T("AutoOpenResult")), 1);
static const SectionLabelDefault GLOBAL_IGNORESERVERVERSION(SectionLabel(GeneralSection, _T("IgnoreServerVersion")), 0);
static const SectionLabelDefault GLOBAL_DISABLEAUTOUPDATE(SectionLabel(GeneralSection, _T("DisableAutoUpdate")), 0);

static const TCHAR ResultSection[](_T("Results"));
static const SectionLabelDefault GLOBAL_FONT_RESULT(SectionLabel(ResultSection, _T("ResultFont")), _T("Courier New"));
static const SectionLabelDefault GLOBAL_FONTSIZE_RESULT(SectionLabel(ResultSection, _T("ResultFontSize")), 10);

static const TCHAR EditorSection[](_T("Editor"));
static const SectionLabelDefault GLOBAL_MAINTAININDENT(SectionLabel(EditorSection, _T("MaintainIndent")), 1);
static const SectionLabelDefault GLOBAL_TAB_WIDTH(SectionLabel(EditorSection, _T("TabWidth")), 2);
static const SectionLabelDefault GLOBAL_TAB_USESPACES(SectionLabel(EditorSection, _T("UseSpaces")), true);
static const SectionLabelDefault GLOBAL_LINENO(SectionLabel(EditorSection, _T("LineNo")), 0);
static const SectionLabelDefault GLOBAL_TREE(SectionLabel(EditorSection, _T("Tree")), 1);
static const SectionLabelDefault GLOBAL_FONT(SectionLabel(EditorSection, _T("Font")), _T("Courier New"));
static const SectionLabelDefault GLOBAL_FONTSIZE(SectionLabel(EditorSection, _T("FontSize")), 10);
static const SectionLabelDefault GLOBAL_TOOLTIP(SectionLabel(EditorSection, _T("Tooltip")), 1);
static const SectionLabelDefault GLOBAL_SHOW_WS(SectionLabel(EditorSection, _T("ShowWS")), false);
static const SectionLabelDefault GLOBAL_TARGETCOLOR(SectionLabel(EditorSection, _T("TargetColor")), 0);
static const SectionLabelDefault GLOBAL_SYNCREPOSITORY(SectionLabel(EditorSection, _T("SyncRepository")), 0);
static const SectionLabelDefault GLOBAL_AUTOCOMPLETEONDOT(SectionLabel(EditorSection, _T("AutoCompleteOnDot")), 1);
static const SectionLabelDefault GLOBAL_DOUBLECLICKSELQUAL(SectionLabel(EditorSection, _T("DoubleClickSelQualLabel")), 0);

static const TCHAR DebugSection[](_T("Debug"));
static const SectionLabelDefault GLOBAL_DISABLEINVOKE(SectionLabel(DebugSection, _T("DisableInoke")), 0);

static const TCHAR CompareSection[](_T("Compare"));
static const SectionLabelDefault GLOBAL_IGNOREWHITESPACE(SectionLabel(CompareSection, _T("IgnoreWhitespace")), 0);
static const SectionLabelDefault GLOBAL_SHOWCRLF(SectionLabel(CompareSection, _T("ShowCRLF")), 0);

WLIB_API bool ShowPreferencesDlg(CComPtr<IConfig> config, bool disableEnvironmentSettings = false);
