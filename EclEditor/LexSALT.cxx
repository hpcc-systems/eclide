// Scintilla source code edit control
/** @file LexSALT.cxx
** Lexer for SALT.
**/
// Copyright 1998-2001 by Neil Hodgson <neilh@scintilla.org>
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
    return (state <= SCE_SALT_COMMENT) ||
        (state == SCE_SALT_COMMENTDOC) || (state == SCE_SALT_COMMENTLINE);
}

enum SALT_LINETYPE {
    LINETYPE_NONE = 0,
    LINETYPE_NORMAL,
    LINETYPE_FIELDNAME,
    LINETYPE_FIELDLIST,
    LINETYPE_FIELDLIST_COMMAS,
    LINETYPE_FIELDNAME_SKIP_ONE,
};

static void ColouriseSaltDoc(unsigned int startPos, int length, int initStyle, WordList *keywordlists[],
    Accessor &styler) {

    WordList &keywords0 = *keywordlists[0];  // Most first parameters
    WordList &keywords1 = *keywordlists[1];  // Common Parameters
    WordList &keywords2 = *keywordlists[2];  // Parameters with a field
    WordList &keywords3 = *keywordlists[3];  // Parameters with fieldlist
    WordList &keywords4 = *keywordlists[4];  // Fieldname lists with commas
    WordList &keywords5 = *keywordlists[5];  // Fieldnames skip one first

    SALT_LINETYPE lineType = LINETYPE_NONE;

    bool continuationLine = false;
    bool collectingFieldName = false;
    int parens = 0;
    int skip = 0;
    int colonCount = 0;
    char s[1000];

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
                continuationLine = true;
                continue;
            }
        }

        sc.GetCurrentLowered(s, sizeof(s));

        // Determine if the current state should terminate.
        switch (sc.state) {
        case SCE_SALT_ARGUMENTS:
            if (sc.Match(')')) {
                collectingFieldName = false;
                parens--;
                if (parens == 0) {
                    sc.SetState(SCE_SALT_DEFAULT);
                }
            }
            else if (parens && sc.Match('(')) {
                sc.ChangeState(SCE_SALT_COMMAND);
                sc.SetState(SCE_SALT_SEPARATOR);
                if (keywords3.InList(s)) {
                    sc.ForwardSetState(SCE_SALT_FIELDNAME);
                    collectingFieldName = true;
                }
                else {
                    sc.ForwardSetState(SCE_SALT_ARGUMENTS);
                }
                parens++;
            }
            else if (parens && sc.Match(',')) {
                sc.SetState(SCE_SALT_SEPARATOR);
                sc.ForwardSetState(SCE_SALT_ARGUMENTS);
            }
            break;
        case SCE_SALT_OPERATOR:
            sc.SetState(SCE_SALT_DEFAULT);
            break;
        case SCE_SALT_COMMAND:
            if (sc.Match(':')) {
                sc.SetState(SCE_SALT_OPERATOR);
                sc.ForwardSetState(SCE_SALT_DEFAULT);
                sc.ForwardSetState(SCE_SALT_FIELDNAME);
            }
            break;
        case SCE_SALT_FIELDNAME:
            if (sc.Match(':')) {
                sc.SetState(SCE_SALT_OPERATOR);
                sc.ForwardSetState(SCE_SALT_DEFAULT);
                collectingFieldName = false;
            }
            else if (sc.Match(')')) {
                sc.SetState(SCE_SALT_OPERATOR);
                collectingFieldName = false;
                parens--;
                if (parens == 0) {
                    sc.SetState(SCE_SALT_DEFAULT);
                }
                else {
                    sc.SetState(SCE_SALT_ARGUMENTS);
                }
            }
            break;
        case SCE_SALT_COMMENT:
            if (sc.Match('*', '/')) {
                sc.Forward();
                sc.ForwardSetState(SCE_SALT_DEFAULT);
            }
            break;
        case SCE_SALT_COMMENTDOC:
            if (sc.Match('*', '/')) {
                sc.Forward();
                sc.ForwardSetState(SCE_SALT_DEFAULT);
            }
            break;
        case SCE_SALT_COMMENTLINE:
            if (sc.atLineStart) {
                sc.SetState(SCE_SALT_DEFAULT);
            }
            break;
        }

        // Determine if a new state should be entered.
        int lineCurrent = styler.GetLine(sc.currentPos);
        int lineState = styler.GetLineState(lineCurrent);

        if (sc.atLineStart) {
            if (lineType == LINETYPE_FIELDNAME || lineType == LINETYPE_FIELDLIST_COMMAS || lineType == LINETYPE_FIELDLIST_COMMAS)
            {
                sc.ChangeState(SCE_SALT_FIELDNAME);
            }
            sc.SetState(SCE_SALT_DEFAULT);
            lineType = LINETYPE_NONE;
            collectingFieldName = false;
            colonCount = 0;
            skip = 0;
            line++;
        }

        if (sc.state == SCE_SALT_DEFAULT) {
            if (lineState) {
                sc.SetState(lineState);
            }
            else if (lineType == LINETYPE_FIELDLIST_COMMAS && sc.Match(',') && collectingFieldName) {
                sc.ChangeState(SCE_SALT_FIELDNAME);
                sc.SetState(SCE_SALT_OPERATOR);
            }
            else if (sc.Match(':')) {
                colonCount++;
                if (keywords0.InList(s)) {
                    sc.ChangeState(SCE_SALT_COMMAND);
                    lineType = LINETYPE_NORMAL;
                }
                else if (keywords1.InList(s)) {
                    sc.ChangeState(SCE_SALT_COMMAND);
                }
                else if (keywords2.InList(s)) {
                    sc.ChangeState(SCE_SALT_COMMAND);
                    collectingFieldName = true;
                    lineType = LINETYPE_FIELDNAME;
                }
                else if (keywords3.InList(s)) {
                    sc.ChangeState(SCE_SALT_COMMAND);
                    collectingFieldName = true;
                    lineType = LINETYPE_FIELDLIST;
                }
                else if (keywords4.InList(s)) {
                    sc.ChangeState(SCE_SALT_COMMAND);
                    collectingFieldName = true;
                    lineType = LINETYPE_FIELDLIST_COMMAS;
                }
                else if (keywords5.InList(s)) {
                    sc.ChangeState(SCE_SALT_COMMAND);
                    lineType = LINETYPE_FIELDNAME_SKIP_ONE;
                    collectingFieldName = true;
                    skip = 1;
                }
                else if (lineType == LINETYPE_FIELDNAME_SKIP_ONE && (skip + 1) >= colonCount) {
                    sc.ChangeState(SCE_SALT_ARGUMENTS);
                    collectingFieldName = true;
                }
                else if (collectingFieldName && (lineType == LINETYPE_FIELDNAME || lineType == LINETYPE_FIELDLIST || lineType == LINETYPE_FIELDNAME_SKIP_ONE))
                {
                    sc.ChangeState(SCE_SALT_FIELDNAME);
                    if (lineType == LINETYPE_FIELDNAME)
                    {
                        collectingFieldName = false;
                    }
                }
                sc.SetState(SCE_SALT_OPERATOR);
            }
            else if (sc.Match('/', '*')) {
                if (sc.Match("/**") || sc.Match("/*!")) {	// Support of Qt/Doxygen doc. style
                    sc.SetState(SCE_SALT_COMMENT);
                }
                else {
                    sc.SetState(SCE_SALT_COMMENT);
                }
                sc.Forward();	// Eat the * so it isn't used for the end of the comment
            }
            else if (sc.Match('/', '/')) {
                sc.SetState(SCE_SALT_COMMENTLINE);
            }
            else if (!collectingFieldName || lineType == LINETYPE_FIELDNAME || lineType == LINETYPE_FIELDLIST) {
                bool digit = false;
                if (sc.Match('(') && !digit) {
                    sc.ChangeState(SCE_SALT_COMMAND);
                    sc.SetState(SCE_SALT_SEPARATOR);
                    if (keywords3.InList(s)) {
                        sc.ForwardSetState(SCE_SALT_FIELDNAME);
                        collectingFieldName = true;
                    }
                    else {
                        sc.ForwardSetState(SCE_SALT_ARGUMENTS);
                    }
                    parens++;
                }
            }
        }

        continuationLine = false;
    }
    sc.Complete();

}

static bool IsStreamCommentStyle(int style) {
    return style == SCE_SALT_COMMENT ||
        style == SCE_SALT_COMMENTDOC;
}

static bool IsStreamCommentStyleSalt(int style) {
    return style == SCE_SALT_COMMENT ||
        style == SCE_SALT_COMMENTLINE ||
        style == SCE_SALT_COMMENTDOC;
}

// Store both the current line's fold level and the next lines in the
// level store to make it easy to pick up with each increment
// and to make it possible to fiddle the current level for "} else {".
static void FoldSaltDoc(unsigned int startPos, int length, int initStyle,
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
        if (foldComment && IsStreamCommentStyleSalt(style)) {
            if (!IsStreamCommentStyleSalt(stylePrev)) {
                levelNext++;
            }
            else if (!IsStreamCommentStyleSalt(styleNext) && !atEOL) {
                // Comments don't end at end of line and the next character may be unstyled.
                levelNext--;
            }
        }
        if (foldComment && (style == SCE_SALT_COMMENTLINE)) {
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

static const char * const SaltWordListDesc[] = {
    "Commands",
    "ParameterWithFields",
    "Parameters",
    "ParamsWithFieldlists",
    "ParamsWithFieldCommas",
    "SkipOneFieldlists",
    0
};

LexerModule lmSALT(
    SCLEX_SALT,
    ColouriseSaltDoc,
    "salt",
    FoldSaltDoc,
    SaltWordListDesc);