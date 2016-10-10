// Scintilla source code edit control
/** @file LexGENERAL.cxx
 ** Lexer for general color preferences. Not an attribute type.
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

static void ColouriseGENERALDocSensitive(unsigned int startPos, int length, int initStyle, WordList *keywordlists[], Accessor &styler) {

    WordList &headerList = *keywordlists[0];
    WordList &thorList = *keywordlists[1];
    WordList &hthorList = *keywordlists[2];
    WordList &roxieList = *keywordlists[3];
    WordList &localList = *keywordlists[4];

    char s[1000];
    bool checkword = false;
    int linetype = 0;

    StyleContext sc(startPos, length, initStyle, styler);
    int line = 0;

    for (; sc.More(); sc.Forward()) {
        sc.GetCurrentLowered(s, sizeof(s));
        strtrim(s);

        // Determine if the current state should terminate.
        switch (sc.state) {
        case SCE_GEN_HEADER:
            if (sc.chPrev == ':') {
                sc.ChangeState(SCE_GEN_DEFAULT);
                sc.SetState(SCE_GEN_DELIMITER);
                if (linetype == SCE_GEN_CARET)
                    sc.ForwardSetState(SCE_GEN_CARET);
                else
                    sc.ForwardSetState(SCE_GEN_DEFAULT);
            }
            break;
        case SCE_GEN_CARET:
        case SCE_GEN_THOR_BACKGROUND:
        case SCE_GEN_HTHOR_BACKGROUND:
        case SCE_GEN_ROXIE_BACKGROUND:
        case SCE_GEN_LOCAL_BACKGROUND:
            if (sc.atLineEnd)
            {
                sc.SetState(SCE_GEN_DEFAULT);
            }
            break;
        }

        if (sc.state == SCE_GEN_DEFAULT) {
            checkword = false;
            if (headerList.InList(s)) {
                if (stricmp(s, "caret") == 0)
                {
                    sc.SetState(SCE_GEN_HEADER);
                    linetype = SCE_GEN_CARET;
                }
                else if (stricmp(s, "target") == 0)
                {
                    sc.ChangeState(SCE_GEN_HEADER);
                    linetype = SCE_GEN_TARGET;
                }
            }
            else {
                if (thorList.InList(s)) {
                    sc.ChangeState(SCE_GEN_THOR_BACKGROUND);
                    sc.SetState(SCE_GEN_THOR_BACKGROUND);
                }
                else if (hthorList.InList(s)) {
                    sc.ChangeState(SCE_GEN_HTHOR_BACKGROUND);
                    sc.SetState(SCE_GEN_HTHOR_BACKGROUND);
                }
                else if (roxieList.InList(s)) {
                    sc.ChangeState(SCE_GEN_ROXIE_BACKGROUND);
                    sc.SetState(SCE_GEN_ROXIE_BACKGROUND);
                }
                else if (localList.InList(s)) {
                    sc.ChangeState(SCE_GEN_LOCAL_BACKGROUND);
                    sc.SetState(SCE_GEN_LOCAL_BACKGROUND);
                }
            }
        }
    }
    sc.Complete();
}

static void FoldGENERALDoc(unsigned int startPos, int length, int initStyle,
    WordList *[], Accessor &styler) {
}

static const char * const generalWordLists[] = {
            "Headers",
            "Thor",
            "HThor",
            "Roxie",
            "Local",
            0,
        };

LexerModule lmGENERAL(SCLEX_GENERAL, ColouriseGENERALDocSensitive, "general", FoldGENERALDoc, generalWordLists);
