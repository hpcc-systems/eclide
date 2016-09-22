// Scintilla source code edit control
/** @file LexESDL.cxx
 ** Lexer for C++, C, Java, and JavaScript.
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

static bool IsStreamCommentStyleEsdl(int style) {
    return style == SCE_ESDL_COMMENT ||
        style == SCE_ESDL_COMMENTLINE ||
        style == SCE_ESDL_COMMENTDOC;
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

static void ColouriseESDLDocSensitive(unsigned int startPos, int length, int initStyle, WordList *keywordlists[],
    Accessor &styler) {

    WordList &listStructures = *keywordlists[0];
    WordList &listDatatypes = *keywordlists[1];
    WordList &listAttributes = *keywordlists[2];

    char s[1000];
    int parens = 0;
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

        // Determine if the current state should terminate.
        switch (sc.state) {
        case SCE_ESDL_STRING:
            if (sc.Match('"')) {
                sc.ForwardSetState(SCE_ESDL_DEFAULT);
            }
            break;
        case SCE_ESDL_STRUCTURE:
        case SCE_ESDL_DATATYPE:
        case SCE_ESDL_ATTRIBUTE:
            sc.SetState(SCE_SALT_DEFAULT);
            break;
        case SCE_ESDL_COMMENT:
            if (sc.Match('*', '/')) {
                sc.Forward();
                sc.ForwardSetState(SCE_ESDL_DEFAULT);
            }
            break;
        case SCE_ESDL_COMMENTDOC:
            if (sc.Match('*', '/')) {
                sc.Forward();
                sc.ForwardSetState(SCE_ESDL_DEFAULT);
            }
            break;
        case SCE_ESDL_COMMENTLINE:
            if (sc.atLineStart) {
                sc.SetState(SCE_ESDL_DEFAULT);
            }
            break;
        }

        // Determine if a new state should be entered.
        int lineCurrent = styler.GetLine(sc.currentPos);
        int lineState = styler.GetLineState(lineCurrent);

        if (sc.state == SCE_ESDL_DEFAULT) {
            checkword = false;
            if (lineState) {
                sc.SetState(lineState);
            }
            else if (sc.ch == '"') {
                sc.SetState(SCE_ESDL_STRING);
            }
            else if (sc.ch == '(' || sc.ch == '<' || sc.ch == '{') {
                checkword = true;
                parens++;
            }
            else if (sc.ch == ')' || sc.ch == '>' || sc.ch == '}') {
                parens--;
            }
            else if (sc.ch == ' ') {
                strtrim(s);
                checkword = true;
            }
            else if (sc.ch == '\r') {
                strtrim(s);
                checkword = true;
            }
            else if (sc.Match('/', '*')) {
                if (sc.Match("/**") || sc.Match("/*!")) {	// Support of Qt/Doxygen doc. style
                    sc.SetState(SCE_ESDL_COMMENT);
                }
                else {
                    sc.SetState(SCE_ESDL_COMMENT);
                }
                sc.Forward();	// Eat the * so it isn't used for the end of the comment
            }
            else if (sc.Match('/', '/')) {
                sc.SetState(SCE_ESDL_COMMENTLINE);
            }
            if (checkword) {
                strtrim(s);
                if (listStructures.InList(s)) {
                    sc.ChangeState(SCE_ESDL_STRUCTURE);
                }
                else if (listDatatypes.InList(s)) {
                    sc.ChangeState(SCE_ESDL_DATATYPE);
                }
                else if (listAttributes.InList(s)) {
                    sc.ChangeState(SCE_ESDL_ATTRIBUTE);
                }
                sc.SetState(SCE_ESDL_DEFAULT);
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
static void FoldESDLDoc(unsigned int startPos, int length, int initStyle,
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
        if (foldComment && IsStreamCommentStyleEsdl(style)) {
            if (!IsStreamCommentStyleEsdl(stylePrev)) {
                levelNext++;
            }
            else if (!IsStreamCommentStyleEsdl(styleNext) && !atEOL) {
                // Comments don't end at end of line and the next character may be unstyled.
                levelNext--;
            }
        }
        if (foldComment && (style == SCE_ESDL_COMMENTLINE)) {
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

static const char * const esdlWordLists[] = {
            "ESDL Structures",
            "ESDL Datatypes",
            "ESDL Attributes",
            0,
        };

LexerModule lmESDL(SCLEX_ESDL, ColouriseESDLDocSensitive, "esdl", FoldESDLDoc, esdlWordLists);
