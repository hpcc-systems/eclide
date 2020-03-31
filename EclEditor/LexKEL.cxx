// Scintilla source code edit control
/** @file LexKEL.cxx
 ** Lexer for KEL.
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

static bool IsSpaceEquiv(int state) {
    return (state <= SCE_C_COMMENTDOC) ||
        // including SCE_C_DEFAULT, SCE_C_COMMENT, SCE_C_COMMENTLINE
        (state == SCE_C_COMMENTLINEDOC) || (state == SCE_C_COMMENTDOCKEYWORD) ||
        (state == SCE_C_COMMENTDOCKEYWORDERROR);
}

static bool IsStreamCommentStyleKel(int style) {
    return style == SCE_KEL_COMMENT ||
        style == SCE_KEL_COMMENTLINE ||
        style == SCE_KEL_COMMENTDOC;
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

static void ColouriseKELDocSensitive(unsigned int startPos, int length, int initStyle, WordList *keywordlists[], Accessor &styler) {

    WordList &listWord1 = *keywordlists[0];
    WordList &listAggregate = *keywordlists[1];
    WordList &listDate = *keywordlists[2];
    WordList &listFunction = *keywordlists[3];
    WordList &listECL = *keywordlists[4];
    WordList &listType = *keywordlists[5];
    WordList &listBoolean = *keywordlists[6];

    char s[1000];
    bool checkword = false;

    // look back to set chPrevNonWhite properly for better regex colouring
    if (startPos > 0) {
        int back = startPos;
        while (--back && IsSpaceEquiv(styler.StyleAt(back)))
            ;
    }

    StyleContext sc(startPos, length, initStyle, styler);
    int line = 0;

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

        // Determine if the current state should terminate.
        switch (sc.state) {
        case SCE_KEL_STRING:
            if (sc.Match('\'')) {
                sc.ForwardSetState(SCE_KEL_DEFAULT);
            }
            break;
        case SCE_KEL_SEPARATOR:
        case SCE_KEL_WORD1:
        case SCE_KEL_AGGREGATE:
        case SCE_KEL_DATE:
        case SCE_KEL_ECL:
        case SCE_KEL_FUNCTION:
        case SCE_KEL_TYPE:
        case SCE_KEL_BOOLEAN:
            sc.SetState(SCE_SALT_DEFAULT);
            break;
        case SCE_KEL_COMMENT:
            if (sc.Match('*', '/')) {
                sc.Forward();
                sc.ForwardSetState(SCE_KEL_DEFAULT);
            }
            break;
        case SCE_KEL_COMMENTDOC:
            if (sc.Match('*', '/')) {
                sc.Forward();
                sc.ForwardSetState(SCE_KEL_DEFAULT);
            }
            break;
        case SCE_KEL_COMMENTLINE:
            if (sc.atLineStart) {
                sc.SetState(SCE_KEL_DEFAULT);
            }
            break;
        }

        // Determine if a new state should be entered.
        int lineCurrent = styler.GetLine(sc.currentPos);
        int lineState = styler.GetLineState(lineCurrent);

        if (sc.state == SCE_KEL_DEFAULT) {
            checkword = false;
            if (lineState) {
                sc.SetState(lineState);
            }
            else if (sc.ch == '\'') {
                sc.SetState(SCE_KEL_STRING);
            }
            else if (sc.ch == '(' || sc.ch == '<' || sc.ch == '{') {
                checkword = true;
            }
            else if (sc.ch == ')' || sc.ch == '>' || sc.ch == '}') {
                checkword = true;
            }
            else if (sc.ch == ',' || sc.ch == '=' || sc.ch == ':' ||  sc.ch == ' ' || sc.ch == ' ') {
                checkword = true;
            }
            else if (sc.ch == '\r'|| sc.ch == '\n') {
                checkword = true;
            }
            else if (sc.Match('/', '*')) {
                if (sc.Match("/**") || sc.Match("/*!")) {	// Support of Qt/Doxygen doc. style
                    sc.SetState(SCE_KEL_COMMENT);
                }
                else {
                    sc.SetState(SCE_KEL_COMMENT);
                }
                sc.Forward();	// Eat the * so it isn't used for the end of the comment
            }
            else if (sc.Match('/', '/')) {
                sc.SetState(SCE_KEL_COMMENTLINE);
            }
            if (checkword) {
                if (listWord1.InList(s)) {
                    sc.ChangeState(SCE_KEL_WORD1);
                }
                else if (listAggregate.InList(s)) {
                    sc.ChangeState(SCE_KEL_AGGREGATE);
                }
                else if (listDate.InList(s)) {
                    sc.ChangeState(SCE_KEL_DATE);
                }
                else if (listFunction.InList(s)) {
                    sc.ChangeState(SCE_KEL_FUNCTION);
                }
                else if (listECL.InList(s)) {
                    sc.ChangeState(SCE_KEL_ECL);
                }
                else if (listType.InList(s)) {
                    sc.ChangeState(SCE_KEL_TYPE);
                }
                else if (listBoolean.InList(s)) {
                    sc.ChangeState(SCE_KEL_BOOLEAN);
                }
                sc.SetState(SCE_KEL_SEPARATOR);
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
static void FoldKELDoc(unsigned int startPos, int length, int initStyle,
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
        if (foldComment && IsStreamCommentStyleKel(style)) {
            if (!IsStreamCommentStyleKel(stylePrev)) {
                levelNext++;
            }
            else if (!IsStreamCommentStyleKel(styleNext) && !atEOL) {
                // Comments don't end at end of line and the next character may be unstyled.
                levelNext--;
            }
        }
        if (foldComment && (style == SCE_KEL_COMMENTLINE)) {
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

static const char * const kelWordLists[] = {
            "KEL Word",
            "KEL Aggregate",
            "KEL Date",
            "KEL Function",
            "KEL ECL",
            "KEL Type",
            "KEL Boolean",
            0,
        };

LexerModule lmKEL(SCLEX_KEL, ColouriseKELDocSensitive, "kel", FoldKELDoc, kelWordLists);
