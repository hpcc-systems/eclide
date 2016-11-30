// Scintilla source code edit control
/** @file LexDUD.cxx
 ** Lexer for DUD.
 **/
// Copyright 1998-2005 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif
#ifdef __BORLANDC__
// Borland C++ displays warnings in vector header without this
#pragma option -w-ccc -w-rch
#endif

#include <string>
#include <vector>
#include <map>
#include <algorithm>

#include "ILexer.h"
#include "Scintilla.h"
#include "SciLexer.h"

#include "PropSetSimple.h"
#include "WordList.h"
#include "LexAccessor.h"
#include "Accessor.h"
#include "StyleContext.h"
#include "CharacterSet.h"
#include "LexerModule.h"
#include "OptionSet.h"

#ifdef SCI_NAMESPACE
using namespace Scintilla;
#endif

static bool IsStreamCommentStyleDud(int style) {
    return style == SCE_DUD_COMMENT ||
        style == SCE_DUD_COMMENTLINE ||
        style == SCE_DUD_COMMENTDOC;
}

// Preconditions: sc.currentPos points to a character after '+' or '-'.
// The test for pos reaching 0 should be redundant,
// and is in only for safety measures.
// Limitation: this code will give the incorrect answer for code like
// a = b+++/ptn/...
// Putting a space between the '++' post-inc operator and the '+' binary op
// fixes this, and is highly recommended for readability anyway.
static bool FollowsPostfixOperator(StyleContext &sc, Accessor &styler) {
    int pos = (int) sc.currentPos;
    while (--pos > 0) {
        char ch = styler[pos];
        if (ch == '+' || ch == '-') {
            return styler[pos - 1] == ch;
        }
    }
    return false;
}

static void strtrim(char* str) {
    int start = 0; // number of leading spaces
    char* buffer = str;
    while (*str && (*str == ' ' || *str == '\r' || *str == '\n' || *str == '\t'))
    {
        *str++;
        ++start;
    }
    while (*str++); // move to end of string
    int end = str - buffer - 1;
    while (end > 0 && (buffer[end - 1] == ' ' || buffer[end - 1] == '\n' || buffer[end - 1] == '\r' || buffer[end - 1] == '\t')) --end; // backup over trailing spaces
    buffer[end] = 0; // remove trailing spaces
    if (end <= start || start == 0) return; // exit if no leading spaces or string is now empty
    str = buffer + start;
    while ((*buffer++ = *str++));  // remove leading spaces: K&R
}

enum HIPIE_SECTION {
    SECTION_NONE = 0,
    SECTION_PERMISSIONS = SCE_DUD_PERMISSIONS,
    SECTION_INPUTS = SCE_DUD_INPUTS,
    SECTION_OUTPUTS = SCE_DUD_OUTPUTS,
    SECTION_VISUALIZATION = SCE_DUD_VISUALIZATION,
    SECTION_RESOURCES = SCE_DUD_RESOURCES
};

static std::string GetRestOfLine(LexAccessor &styler, int start, bool allowSpace) {
    std::string restOfLine;
    int i = 0;
    char ch = styler.SafeGetCharAt(start, '\n');
    while ((ch != '\r') && (ch != '\n')) {
        if (allowSpace || (ch != ' '))
            restOfLine += tolower(ch);
        i++;
        ch = styler.SafeGetCharAt(start + i, '\n');
    }
    return restOfLine;
}

static void ColouriseDUDDocSensitive(unsigned int startPos, int length, int initStyle, WordList *keywordlists[], Accessor &styler) {

    WordList &listSections = *keywordlists[0];
    WordList &listPermissions = *keywordlists[1];
    WordList &listPlugins = *keywordlists[2];
    WordList &listTypes = *keywordlists[3];
    WordList &listInput = *keywordlists[4];
    WordList &listOutput = *keywordlists[5];
    WordList &listVisualization = *keywordlists[6];
    WordList &listResources = *keywordlists[7];
    WordList &listMisc = *keywordlists[8];

    WordList twoWords;
    twoWords.Set("salt inline");

    WordList endWords;
    endWords.Set("end endgenerates endresources endvisualize");

    CharacterSet setWord(CharacterSet::setAlphaNum, "._", 0x80, true);

    char s[1000];
    bool checkword = false;
    HIPIE_SECTION hipieSection = SECTION_NONE;

    int endPos = startPos + length;
    StyleContext sc(startPos, length, initStyle, styler);

    int pos = 0;
    int sectionLine = 0;
    for (int i = 0; i < 20; i++) {
        std::string linestr = GetRestOfLine(styler, pos, true);
        if (listSections.InList(linestr.c_str())) {
            break;
        }
        pos += linestr.length() + 2;
        sectionLine++;
    }

    for (; sc.More(); sc.Forward()) {
        // Handle line continuation generically.
        if (sc.ch == '\\') {
            if (sc.chNext == '\n' || sc.chNext == '\r') {
                sc.Forward();
                if (sc.ch == '\r' && sc.chNext == '\n') {
                    sc.Forward();
                }
                continue;
            }
        }

        sc.GetCurrentLowered(s, sizeof(s));
        strtrim(s);
        if (strstr(s, "permissions")) {
            bool stophere = true;
        }

        // Determine if the current state should terminate.
        switch (sc.state) {
        case SCE_DUD_NUMBER:
            if (!(setWord.Contains(sc.ch) || ((sc.ch == '+' || sc.ch == '-') && (sc.chPrev == 'e' || sc.chPrev == 'E')))) {
                sc.SetState(SCE_DUD_DEFAULT);
            }
            break;
        case SCE_DUD_TWOWORDS:
            if (twoWords.InList(s)) {
                sc.ChangeState(SCE_DUD_SECTIONS);
                sc.ForwardSetState(SCE_DUD_DEFAULT);
            }
            break;
        case SCE_DUD_STRING:
            if (sc.Match('\"')) {
                sc.ForwardSetState(SCE_DUD_DEFAULT);
            }
            break;
        case SCE_DUD_SECTIONS:
        case SCE_DUD_PERMISSIONS:
        case SCE_DUD_PLUGINS:
        case SCE_DUD_TYPES:
        case SCE_DUD_INPUTS:
        case SCE_DUD_OUTPUTS:
        case SCE_DUD_VISUALIZATION:
        case SCE_DUD_RESOURCES:
        case SCE_DUD_MISC:
        case SCE_DUD_SEPARATOR:
            sc.SetState(SCE_SALT_DEFAULT);
            break;
        case SCE_DUD_COMMENT:
            if (sc.Match('*', '/')) {
                sc.Forward();
                sc.ForwardSetState(SCE_DUD_DEFAULT);
            }
            break;
        case SCE_DUD_COMMENTDOC:
            if (sc.Match('*', '/')) {
                sc.Forward();
                sc.ForwardSetState(SCE_DUD_DEFAULT);
            }
            break;
        case SCE_DUD_COMMENTLINE:
            if (sc.atLineStart) {
                sc.SetState(SCE_DUD_DEFAULT);
            }
            break;
        }

        // Determine if a new state should be entered.
        int lineCurrent = styler.GetLine(sc.currentPos);
        int lineState = styler.GetLineState(lineCurrent);

        if (sc.state == SCE_DUD_DEFAULT) {
            checkword = false;

            if (lineState) {
                sc.SetState(lineState);
            }
            else if (IsADigit(sc.ch)) {
                checkword = true;
            }
            else if (sc.ch == '\"') {
                sc.SetState(SCE_DUD_STRING);
            }
            else if (sc.ch == '(' || sc.ch == '<' || sc.ch == '{') {
                checkword = true;
            }
            else if (sc.ch == ')' || sc.ch == '>' || sc.ch == '}') {
                checkword = true;
            }
            else if (sc.ch == ',' || sc.ch == '=' || sc.ch == ':' || sc.ch == ' ' || sc.ch == ' ' || sc.ch == ';') {
                checkword = true;
            }
            else if (sc.ch == '\t' || sc.ch == '\r'|| sc.ch == '\n' || sc.atLineEnd) {
                checkword = true;
            }
            else if (sc.Match('/', '*')) {
                if (sc.Match("/**") || sc.Match("/*!")) {	// Support of Qt/Doxygen doc. style
                    sc.SetState(SCE_DUD_COMMENT);
                }
                else {
                    sc.SetState(SCE_DUD_COMMENT);
                }
                sc.Forward();	// Eat the * so it isn't used for the end of the comment
            }
            else if (sc.Match('/', '/')) {
                sc.SetState(SCE_DUD_COMMENTLINE);
            }
            // Edge condition for no final \r or \n
            if (sc.currentPos + 1 == endPos) {
                checkword = true;
                sc.Forward();
                sc.GetCurrentLowered(s, sizeof(s));
            }
            if (checkword) {
                if (stricmp(s, "generates") == 0) {
                    sc.ChangeState(SCE_DUD_SECTIONS);
                    sc.SetState(SCE_DUD_TWOWORDS);
                    continue;
                }
                if (listSections.InList(s)) {
                    sc.ChangeState(SCE_DUD_SECTIONS);
                    if (endWords.InList(s)) {
                        hipieSection = SECTION_NONE;
                    }
                    else if (stricmp(s, "inputs") == 0) {
                        hipieSection = SECTION_INPUTS;
                    }
                    else if (stricmp(s, "outputs") == 0) {
                        hipieSection = SECTION_OUTPUTS;
                    }
                    else if (stricmp(s, "permissions") == 0) {
                        hipieSection = SECTION_PERMISSIONS;
                    }
                    else if (stricmp(s, "visualize") == 0) {
                        hipieSection = SECTION_VISUALIZATION;
                    }
                    else if (stricmp(s, "resources") == 0) {
                        hipieSection = SECTION_VISUALIZATION;
                    }
                }
                else if (listPermissions.InList(s)) {
                    sc.ChangeState(SCE_DUD_PERMISSIONS);
                }
                else if (listPlugins.InList(s)) {
                    if (lineCurrent < sectionLine)
                        sc.ChangeState(SCE_DUD_PLUGINS);
                    else
                        sc.ChangeState(SCE_DUD_MISC);
                }
                else if (listTypes.InList(s)) {
                    sc.ChangeState(SCE_DUD_TYPES);
                    while (IsADigit(sc.ch)) {
                        sc.SetState(SCE_DUD_TYPES);
                        sc.Forward();
                    }
                }
                else if (listInput.InList(s)) {
                    sc.ChangeState(SCE_DUD_INPUTS);
                }
                else if (listOutput.InList(s)) {
                    sc.ChangeState(SCE_DUD_OUTPUTS);
                }
                else if (listVisualization.InList(s)) {
                    sc.ChangeState(SCE_DUD_VISUALIZATION);
                }
                else if (listResources.InList(s)) {
                    sc.ChangeState(SCE_DUD_RESOURCES);
                }
                else if (listMisc.InList(s)) {
                    sc.ChangeState(SCE_DUD_MISC);
                }
                sc.SetState(SCE_DUD_SEPARATOR);
            }
        }
    }
    sc.Complete();
}

static bool IsStreamCommentStyle(int style) {
    return style == SCE_C_COMMENT ||
        style == SCE_C_COMMENTDOC ||
        style == SCE_C_COMMENTDOCKEYWORD ||
        style == SCE_C_COMMENTDOCKEYWORDERROR;
}
// Store both the current line's fold level and the next lines in the
// level store to make it easy to pick up with each increment
// and to make it possible to fiddle the current level for "} else {".
static void FoldDUDDoc(unsigned int startPos, int length, int initStyle,
    WordList *[], Accessor &styler) {
    bool foldComment = true;
    bool foldPreprocessor = true;
    bool foldCompact = true;
    bool foldAtElse = true;
    unsigned int endPos = startPos + length;
    int visibleChars = 0;
    int lineCurrent = styler.GetLine(startPos);
    int levelCurrent = SC_FOLDLEVELBASE;
    if (lineCurrent > 0)
        levelCurrent = styler.LevelAt(lineCurrent - 1) >> 16;
    int levelMinCurrent = levelCurrent;
    int levelNext = levelCurrent;
    char chNext = styler[startPos];
    int styleNext = styler.StyleAt(startPos);
    int style = initStyle;
    for (unsigned int i = startPos; i < endPos; i++) {
        char ch = chNext;
        chNext = styler.SafeGetCharAt(i + 1);
        int stylePrev = style;
        style = styleNext;
        styleNext = styler.StyleAt(i + 1);
        bool atEOL = (ch == '\r' && chNext != '\n') || (ch == '\n');
        if (foldComment && IsStreamCommentStyleDud(style)) {
            if (!IsStreamCommentStyleDud(stylePrev)) {
                levelNext++;
            }
            else if (!IsStreamCommentStyleDud(styleNext) && !atEOL) {
                // Comments don't end at end of line and the next character may be unstyled.
                levelNext--;
            }
        }
        if (foldComment && (style == SCE_DUD_COMMENTLINE)) {
            if ((ch == '/') && (chNext == '/')) {
                char chNext2 = styler.SafeGetCharAt(i + 2);
                if (chNext2 == '{') {
                    levelNext++;
                }
                else if (chNext2 == '}') {
                    levelNext--;
                }
            }
        }
        if (atEOL || (i == endPos - 1)) {
            int levelUse = levelCurrent;
            if (foldAtElse) {
                levelUse = levelMinCurrent;
            }
            int lev = levelUse | levelNext << 16;
            if (visibleChars == 0 && foldCompact)
                lev |= SC_FOLDLEVELWHITEFLAG;
            if (levelUse < levelNext)
                lev |= SC_FOLDLEVELHEADERFLAG;
            if (lev != styler.LevelAt(lineCurrent)) {
                styler.SetLevel(lineCurrent, lev);
            }
            lineCurrent++;
            levelCurrent = levelNext;
            levelMinCurrent = levelCurrent;
            if (atEOL && (i == static_cast<unsigned int>(styler.Length() - 1))) {
                // There is an empty line at end of file so give it same level and empty
                styler.SetLevel(lineCurrent, (levelCurrent | levelCurrent << 16) | SC_FOLDLEVELWHITEFLAG);
            }
            visibleChars = 0;
        }
        if (!IsASpace(ch))
            visibleChars++;
    }
}

static const char * const dudWordLists[] = {
            "DUD Sections",
            "DUD Permissions",
            "DUD Plugins",
            "DUD Types",
            "DUD Input",
            "DUD Output",
            "DUD Visualization",
            "DUD Resources",
            "DUD Misc",
            0,
        };

LexerModule lmDUD(SCLEX_DUD, ColouriseDUDDocSensitive, "dud", FoldDUDDoc, dudWordLists);
