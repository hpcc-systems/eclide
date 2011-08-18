#pragma once

#include "global.h"
#include "AboutDlg.h"
#include "Platform.h"
#include "Scintilla.h"

class WordList2
{
private:
	std::set<std::string> m_words;

public:
	void insert(const std::string & s)
	{
		m_words.insert(s);
	}

	void Set(const std::string & s)
	{
		boost::tokenizer<> tok(s);
		for(boost::tokenizer<>::iterator itr = tok.begin(); itr != tok.end(); ++itr)
		{
			m_words.insert(*itr);
		}
	}

	bool contains(const std::string & s)
	{
		return (m_words.find(s) != m_words.end());
	}

	std::size_t size()
	{
		return m_words.size();
	}

	void GetWordsSartingWith(const std::string & prefix, std::string & result, bool autoCompleteIgnoreCase = false)
	{
		result.clear();
		for(std::set<std::string>::iterator itr = m_words.begin(); itr != m_words.end(); ++itr)
		{
			if ((autoCompleteIgnoreCase && _strnicmp(itr->c_str(), prefix.c_str(), prefix.length()) == 0) ||
			    (!autoCompleteIgnoreCase && itr->compare(0, prefix.length(), prefix) == 0))
			{
				if (itr != m_words.begin())
					result += " ";
				result += *itr;
			}
		}
	}
};

#define ELEMENTS(a)	(sizeof(a) / sizeof(a[0]))

enum IndentationStatus {
	isNone,		// no effect on indentation
	isBlockStart,	// indentation block begin such as "{" or VB "function"
	isBlockEnd,	// indentation end indicator such as "}" or VB "end"
	isKeyWordStart	// Keywords that cause indentation
};

struct StyleAndWords {
	int styleNumber;
	std::string words;
	bool IsEmpty() 
	{ 
		return words.length() == 0; 
	}
	bool IsSingleChar() 
	{ 
		return words.length() == 1; 
	}
};

template<class T>
class CSciTeExtension
{
protected:
	int m_bracesStyle;
	int m_braceCount;
	bool m_bracesSloppy;

	bool m_autoCompleteIgnoreCase;
	bool m_autoCCausedByOnlyOne;
	std::string m_autoCompleteStartCharacters;

	bool m_indentOpening;
	bool m_indentClosing;
	bool m_indentMaintain;
	bool m_indentAutoMaintain;

	bool m_autoCompleteAuto;

	int m_statementLookback;
	StyleAndWords m_statementIndent;
	StyleAndWords m_statementEnd;
	StyleAndWords m_blockStart;
	StyleAndWords m_blockEnd;

	bool m_lineNumbers;
	int m_lineNumbersWidth;
	enum { lineNumbersWidthDefault = 4};

	//  Printing Support  ---
	Scintilla::PRectangle m_pagesetupMargin;
	HGLOBAL m_hDevMode;
	HGLOBAL m_hDevNames;
	//  --- --- ---

public:
	CSciTeExtension()
	{
		m_bracesStyle = 0;
		m_braceCount = 0;
		m_bracesSloppy = true;

		m_autoCompleteIgnoreCase = true;
		m_autoCCausedByOnlyOne = false;
		m_autoCompleteAuto = false;

		m_indentOpening = false;
		m_indentClosing = false;
		m_indentMaintain = true;
		m_indentAutoMaintain = false;

		m_autoCompleteStartCharacters = "."; 

		m_statementLookback = 10;

		m_lineNumbersWidth = lineNumbersWidthDefault;
		
		m_hDevMode = NULL;
		m_hDevNames = NULL;
	}

	BEGIN_MSG_MAP(CSciTeExtension<T>)
		ALT_MSG_MAP(1)
	END_MSG_MAP()

	LRESULT OnMatchBrace(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		GoMatchingBrace(false);
		return 0;
	}

	//  Implementation  ---

	static bool IsBrace(char ch) 
	{
		return ch == '[' || ch == ']' || ch == '(' || ch == ')' || ch == '{' || ch == '}';
	}

	static bool IsAlphabetic(unsigned int ch) 
	{
		return ((ch >= 'A') && (ch <= 'Z')) || ((ch >= 'a') && (ch <= 'z'));
	}

	static int Minimum(int a, int b) 
	{
		if (a < b)
			return a;
		return b;
	}

	static int Maximum(int a, int b) 
	{
		if (a > b)
			return a;
		return b;
	}

	static bool includes(const StyleAndWords &symbols, const std::string value) 
	{
		if (symbols.words.length() == 0) 
		{
			return false;
		} 
		else if (IsAlphabetic(symbols.words[0])) 
		{
			// Set of symbols separated by spaces
			size_t lenVal = value.length();
			const char *symbol = symbols.words.c_str();
			while (symbol) {
				const char *symbolEnd = strchr(symbol, ' ');
				size_t lenSymbol = strlen(symbol);
				if (symbolEnd)
					lenSymbol = symbolEnd - symbol;
				if (lenSymbol == lenVal) {
					if (strncmp(symbol, value.c_str(), lenSymbol) == 0) {
						return true;
					}
				}
				symbol = symbolEnd;
				if (symbol)
					symbol++;
			}
		} 
		else 
		{
			// Set of individual characters. Only one character allowed for now
			char ch = symbols.words[0];
			return strchr(value.c_str(), ch) != 0;
		}
		return false;
	}

	int GetCurrentLineNumber() 
	{
		T* pT = static_cast<T*>(this);
		return pT->LineFromPosition(pT->GetCurrentPos());
	}

	void GetRange(int start, int end, char *text) 
	{
		T* pT = static_cast<T*>(this);
		CString _text;
		pT->GetTextRange(start, end, _text);
		strcpy(text, CT2CA(_text));
	}

	void GetLine(char *text, int sizeText, int line=-1) 
	{
		T* pT = static_cast<T*>(this);
		if (line < 0)
			line = pT->GetCurrentLineNumber();
		int lineStart = pT->PositionFromLine(line);
		int lineEnd = pT->GetLineEndPosition(line);
		int lineMax = lineStart + sizeText - 1;
		if (lineEnd > lineMax)
			lineEnd = lineMax;
		GetRange(lineStart, lineEnd, text);
		text[lineEnd - lineStart] = '\0';
	}

	bool RangeIsAllWhitespace(int start, int end) 
	{
		T* pT = static_cast<T*>(this);
		for (int i = start;i < end;i++) 
		{
			if ((pT->GetCharAt(i) != ' ') && (pT->GetCharAt(i) != '\t'))
				return false;
		}
		return true;
	}

	unsigned int GetLinePartsInStyle(int line, int style1, int style2, std::string sv[], int len) 
	{
		T* pT = static_cast<T*>(this);
		for (int i = 0; i < len; i++)
			sv[i] = "";
		std::string s;
		int part = 0;
		int thisLineStart = pT->PositionFromLine(line);
		int nextLineStart = pT->PositionFromLine(line + 1);
		for (int pos = thisLineStart; pos < nextLineStart; pos++) 
		{
			if ((pT->GetStyleAt(pos) == style1) || (pT->GetStyleAt(pos) == style2)) 
			{
				char c[2];
				c[0] = (char)pT->GetCharAt(pos);
				c[1] = '\0';
				s += c;
			} 
			else if (s.length() > 0) 
			{
				if (part < len) {
					sv[part++] = s;
				}
				s = "";
			}
		}
		if ((s.length() > 0) && (part < len)) 
		{
			sv[part++] = s;
		}
		return part;
	}

	bool FindMatchingBracePosition(int &braceAtCaret, int &braceOpposite, bool sloppy) 
	{
		T* pT = static_cast<T*>(this);
		bool isInside = false;
		int bracesStyleCheck = m_bracesStyle;
		int caretPos = pT->GetCurrentPos();
		braceAtCaret = -1;
		braceOpposite = -1;
		char charBefore = '\0';
		char styleBefore = '\0';
		int lengthDoc = pT->GetLength();
		if ((lengthDoc > 0) && (caretPos > 0)) 
		{
			// Check to ensure not matching brace that is part of a multibyte character
			if (pT->PositionBefore(caretPos) == (caretPos - 1)) 
			{
				charBefore = (char)pT->GetCharAt(caretPos - 1);
				styleBefore = static_cast<char>(pT->GetStyleAt(caretPos - 1) & 31);
			}
		}
		// Priority goes to character before caret
		if (charBefore && IsBrace(charBefore) && ((styleBefore == bracesStyleCheck) || (!m_bracesStyle))) 
		{
			braceAtCaret = caretPos - 1;
		}
		bool colonMode = false;
		bool isAfter = true;
		if (lengthDoc > 0 && sloppy && (braceAtCaret < 0) && (caretPos < lengthDoc)) 
		{
			// No brace found so check other side
			// Check to ensure not matching brace that is part of a multibyte character
			if (pT->PositionAfter(caretPos) == (caretPos + 1)) 
			{
				char charAfter = (char)pT->GetCharAt(caretPos);
				//char styleAfter = static_cast<char>(pT->GetStyleAt(caretPos) & 31);
				if (charAfter && IsBrace(charAfter) && ((styleBefore == bracesStyleCheck) || (!m_bracesStyle))) 
				{
					braceAtCaret = caretPos;
					isAfter = false;
				}
			}
		}
		if (braceAtCaret >= 0) 
		{
			if (colonMode) 
			{
				int lineStart = pT->LineFromPosition(braceAtCaret);
				int lineMaxSubord = pT->GetLastChild(lineStart, -1);
				braceOpposite = pT->GetLineEndPosition(lineMaxSubord);
			} 
			else 
			{
				braceOpposite = pT->BraceMatch(braceAtCaret);
			}
			if (braceOpposite > braceAtCaret) 
			{
				isInside = isAfter;
			}
			else 
			{
				isInside = !isAfter;
			}
		}
		return isInside;
	}

	void GoMatchingBrace(bool select) 
	{
		T* pT = static_cast<T*>(this);
		int braceAtCaret = -1;
		int braceOpposite = -1;
		bool isInside = FindMatchingBracePosition(braceAtCaret, braceOpposite, true);
		// Convert the character positions into caret positions based on whether
		// the caret position was inside or outside the braces.
		if (isInside) {
			if (braceOpposite > braceAtCaret) {
				braceAtCaret++;
			} else {
				braceOpposite++;
			}
		} else {    // Outside
			if (braceOpposite > braceAtCaret) {
				braceOpposite++;
			} else {
				braceAtCaret++;
			}
		}
		if (braceOpposite >= 0) {
			pT->EnsureRangeVisible(braceOpposite, braceOpposite);
			if (select) {
				pT->SetSel(braceAtCaret, braceOpposite);
			} else {
				pT->SetSel(braceOpposite, braceOpposite);
			}
		}
	}

	bool StartAutoCompleteWord(bool onlyOneWord) 
	{
		T* pT = static_cast<T*>(this);
		char linebuf[1000];
		GetLine(linebuf, sizeof(linebuf));
		int current = pT->GetCaretInLine();
		if (current >= static_cast<int>(sizeof(linebuf)))
			return false;

		int startword = current;
		while (startword > 0 && (pT->m_WordCharacters.find(linebuf[startword - 1]) != std::string::npos))
			startword--;
		if (startword == current)
			return true;
		linebuf[current] = '\0';
		const char *root = linebuf + startword;
		int rootlen = current - startword;
		int doclen = pT->GetLength();
		Scintilla::TextToFind ft = {{0, 0}, 0, {0, 0}};
		ft.lpstrText = const_cast<char*>(root);
		ft.chrg.cpMin = 0;
		ft.chrgText.cpMin = 0;
		ft.chrgText.cpMax = 0;
		int flags = SCFIND_WORDSTART | (m_autoCompleteIgnoreCase ? 0 : SCFIND_MATCHCASE);
		int posCurrentWord = pT->GetCurrentPos() - rootlen;
		int minWordLength = 0;
		int nwords = 0;

		// wordsNear contains a list of words separated by single spaces and with a space
		// at the start and end. this makes it easy to search for words.
		std::string wordsNear;
		wordsNear += " ";

		for (;;) 
		{	// search all the document
			ft.chrg.cpMax = doclen;
			int posFind = pT->_FindText(flags, &ft);
			if (posFind == -1 || posFind >= doclen)
				break;
			if (posFind == posCurrentWord) 
			{
				ft.chrg.cpMin = posFind + rootlen;
				continue;
			}
			// Grab the word and put spaces around it
			const int wordMaxSize = 80;
			char wordstart[wordMaxSize];
			wordstart[0] = ' ';
			GetRange(posFind, Minimum(posFind + wordMaxSize - 3, doclen), wordstart + 1);
			char *wordend = wordstart + 1 + rootlen;
			while (pT->IsWordCharForSelNoPeriod(*wordend))
				wordend++;
			*wordend++ = ' ';
			*wordend = '\0';
			int wordlen = wordend - wordstart - 2;
			if (wordlen > rootlen) {
				if (wordsNear.find(wordstart) == std::string::npos) 
				{	// add a new entry
					wordsNear += wordstart + 1;
					if (minWordLength < wordlen)
						minWordLength = wordlen;

					nwords++;
					if (onlyOneWord && nwords > 1) {
						return true;
					}
				}
			}
			ft.chrg.cpMin = posFind + wordlen;
		}
		size_t length = wordsNear.length();
		if ((length > 2) && (!onlyOneWord || (minWordLength > rootlen))) 
		{
			WordList2 wl;
			wl.Set(wordsNear.c_str());
			std::string result;
			wl.GetWordsSartingWith("", result, m_autoCompleteIgnoreCase);
			pT->AutoCShow(rootlen, CA2T(result.c_str()));
		} 
		else 
		{
			pT->AutoCCancel();
		}
		return true;
	}

	bool StartBlockComment(const std::string & commentMarker) 
	{
		T* pT = static_cast<T*>(this);
		std::string comment = commentMarker + " ";
		std::string long_comment = comment;
		char linebuf[1000];
		size_t comment_length = comment.length();
		size_t selectionStart = pT->GetSelectionStart();
		size_t selectionEnd = pT->GetSelectionEnd();
		size_t caretPosition = pT->GetCurrentPos();
		// checking if caret is located in _beginning_ of selected block
		bool move_caret = caretPosition < selectionEnd;
		int selStartLine = pT->LineFromPosition(selectionStart);
		int selEndLine = pT->LineFromPosition(selectionEnd);
		int lines = selEndLine - selStartLine;
		size_t firstSelLineStart = pT->PositionFromLine(selStartLine);
		// "caret return" is part of the last selected line
		if ((lines > 0) && (selectionEnd == static_cast<size_t>(pT->PositionFromLine(selEndLine))))
			selEndLine--;
		pT->BeginUndoAction();
		for (int i = selStartLine; i <= selEndLine; i++) 
		{
			int lineStart = pT->PositionFromLine(i);
			int lineIndent = lineStart;
			int lineEnd = pT->GetLineEndPosition(i);
			if ((lineEnd - lineIndent) >= static_cast<int>(sizeof(linebuf)))	// Avoid buffer size problems
				continue;

			lineIndent = pT->GetLineIndentPosition(i);
			GetRange(lineIndent, lineEnd, linebuf);

			// empty lines are not commented
			if (strlen(linebuf) < 1)
				continue;
			if (memcmp(linebuf, comment.c_str(), comment_length - 1) == 0) 
			{
				if (memcmp(linebuf, long_comment.c_str(), comment_length) == 0) 
				{
					// removing comment with space after it
					pT->SetSel(lineIndent, lineIndent + comment_length);
					pT->ReplaceSel(_T(""));
					if (i == selStartLine) // is this the first selected line?
						selectionStart -= comment_length;
					selectionEnd -= comment_length; // every iteration
					continue;
				} 
				else 
				{
					// removing comment _without_ space
					pT->SetSel(lineIndent, lineIndent + comment_length - 1);
					pT->ReplaceSel(_T(""));
					if (i == selStartLine) // is this the first selected line?
						selectionStart -= (comment_length - 1);
					selectionEnd -= (comment_length - 1); // every iteration
					continue;
				}
			}
			if (i == selStartLine) // is this the first selected line?
				selectionStart += comment_length;
			selectionEnd += comment_length; // every iteration
			pT->InsertText(lineIndent, CA2T(long_comment.c_str()));
		}
		// after uncommenting selection may promote itself to the lines
		// before the first initially selected line;
		// another problem - if only comment symbol was selected;
		if (selectionStart < firstSelLineStart) {
			if (selectionStart >= selectionEnd - (comment_length - 1))
				selectionEnd = firstSelLineStart;
			selectionStart = firstSelLineStart;
		}
		if (move_caret) {
			// moving caret to the beginning of selected block
			pT->GotoPos(selectionEnd);
			pT->SetCurrentPos(selectionStart);
		} else {
			pT->SetSel(selectionStart, selectionEnd);
		}
		pT->EndUndoAction();
		return true;
	}

	bool StartBoxComment(const std::string & startMarker, const std::string & middleMarker, const std::string & endMarker) 
	{
		T* pT = static_cast<T*>(this);
		std::string start_comment = startMarker;
		std::string middle_comment = middleMarker;
		std::string end_comment = endMarker;
		std::string white_space(" ");
		start_comment += white_space;
		middle_comment += white_space;
		white_space += end_comment;
		end_comment = white_space;
		size_t start_comment_length = start_comment.length();
		size_t middle_comment_length = middle_comment.length();
		size_t selectionStart = pT->GetSelectionStart();
		size_t selectionEnd = pT->GetSelectionEnd();
		size_t caretPosition = pT->GetCurrentPos();
		// checking if caret is located in _beginning_ of selected block
		bool move_caret = caretPosition < selectionEnd;
		size_t selStartLine = pT->LineFromPosition(selectionStart);
		size_t selEndLine = pT->LineFromPosition(selectionEnd);
		size_t lines = selEndLine - selStartLine;
		// "caret return" is part of the last selected line
		if ((lines > 0) && (selectionEnd == static_cast<size_t>(pT->PositionFromLine(selEndLine)))) {
			selEndLine--;
			lines--;
			// get rid of CRLF problems
			selectionEnd = pT->GetLineEndPosition(selEndLine);
		}
		pT->BeginUndoAction();
		// first commented line (start_comment)
		int lineStart = pT->PositionFromLine(selStartLine);
		pT->InsertText(lineStart, CA2T(start_comment.c_str()));
		selectionStart += start_comment_length;
		// lines between first and last commented lines (middle_comment)
		for (size_t i = selStartLine + 1; i <= selEndLine; i++) 
		{
			lineStart = pT->PositionFromLine(i);
			pT->InsertText(lineStart, CA2T(middle_comment.c_str()));
			selectionEnd += middle_comment_length;
		}
		// last commented line (end_comment)
		int lineEnd = pT->GetLineEndPosition(selEndLine);
		if (lines > 0) 
		{
			pT->InsertText(lineEnd, _T("\n"));
			pT->InsertText(lineEnd + 1, CA2T((end_comment.c_str() + 1)));
		} 
		else 
		{
			pT->InsertText(lineEnd, CA2T(end_comment.c_str()));
		}
		selectionEnd += (start_comment_length);
		if (move_caret) 
		{
			// moving caret to the beginning of selected block
			pT->GotoPos(selectionEnd);
			pT->SetCurrentPos(selectionStart);
		} 
		else 
		{
			pT->SetSel(selectionStart, selectionEnd);
		}
		pT->EndUndoAction();
		return true;
	}

	void EnsureRangeVisible(int posStart, int posEnd, bool enforcePolicy=true) 
	{
		T* pT = static_cast<T*>(this);
		int lineStart = pT->LineFromPosition(Minimum(posStart, posEnd));
		int lineEnd = pT->LineFromPosition(Maximum(posStart, posEnd));
		for (int line = lineStart; line <= lineEnd; line++) {
			enforcePolicy ? pT->EnsureVisibleEnforcePolicy(line) : pT->EnsureVisible(line);
		}
	}

	//  Notifications

	void CharAdded(char ch) 
	{
		T* pT = static_cast<T*>(this);
		Scintilla::CharacterRange crange = pT->GetSelection();
		int selStart = crange.cpMin;
		int selEnd = crange.cpMax;
		if ((selEnd == selStart) && (selStart > 0)) 
		{
			int style = pT->GetStyleAt(selStart - 1);
			//Platform::DebugPrintf("Char added %d style = %d %d\n", ch, style, braceCount);
			if (style != 1) {
				if (pT->CallTipActive())
				{
					if (ch == ')') 
					{
						m_braceCount--;
						if (m_braceCount < 1)
							pT->CallTipCancel();
						else
							StartCallTip();
					} 
					else if (ch == '(') 
					{
						m_braceCount++;
						StartCallTip();
					} else {
						ContinueCallTip();
					}
				} 
				else if (pT->AutoCActive()) 
				{
					if (ch == '(') 
					{
						m_braceCount++;
						StartCallTip();
					}
					else if (ch == ')') 
					{
						m_braceCount--;
					} 
					else if (pT->m_WordCharacters.find(ch) == std::string::npos) 
					{
						pT->AutoCCancel();
						if (pT->m_WordCharacters.find(ch) != std::string::npos) 
						{
							pT->StartAutoComplete();
						}
					} 
					else if (m_autoCCausedByOnlyOne) 
					{
						StartAutoCompleteWord(true);
					}
				}
				else if (HandleXml(ch)) 
				{
					// Handled in the routine
				} 
				else 
				{
					if (ch == '(') 
					{
						m_braceCount = 1;
						StartCallTip();
					} 
					else 
					{
						m_autoCCausedByOnlyOne = false;
						if (static_cast<bool>(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_MAINTAININDENT)) == true)
							MaintainIndentation(ch);
						else if (m_indentAutoMaintain)
							AutomaticIndentation(ch);

						if (m_autoCompleteStartCharacters.find(ch) != std::string::npos && static_cast<bool>(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_AUTOCOMPLETEONDOT)) == true) 
							pT->StartAutoComplete();
						else if (m_autoCompleteAuto && pT->m_WordCharacters.find(ch)!= std::string::npos) 
						{
							StartAutoCompleteWord(true);
							m_autoCCausedByOnlyOne = pT->AutoCActive() > 0 ? true : false;
						}
					}
				}
			}
		}
	}

	bool StartCallTip() 
	{
		//T* pT = static_cast<T*>(this);
		//todo
		/*
		char linebuf[1000];
		currentCallTip = 0;
		currentCallTipWord = "";
		GetLine(linebuf, sizeof(linebuf));
		int current = GetCaretInLine();
		int pos = pT->GetCurrentPos();
		int braces;
		do {
			braces = 0;
			while (current > 0 && (braces || linebuf[current - 1] != '(')) 
			{
				if (linebuf[current - 1] == '(')
					braces--;
				else if (linebuf[current - 1] == ')')
					braces++;
				current--;
				pos--;
			}
			if (current > 0) 
			{
				current--;
				pos--;
			} 
			else
				break;
			while (current > 0 && isspace(linebuf[current - 1])) 
			{
				current--;
				pos--;
			}
		} while (current > 0 && !calltipWordCharacters.contains(linebuf[current - 1]));
		if (current <= 0)
			return true;

		startCalltipWord = current - 1;
		while (startCalltipWord > 0 && calltipWordCharacters.contains(linebuf[startCalltipWord - 1])) 
		{
			startCalltipWord--;
		}

		linebuf[current] = '\0';
		currentCallTipWord = linebuf + startCalltipWord;
		functionDefinition = "";
		//Platform::DebugPrintf("word  is [%s] %d %d %d\n", currentCallTipWord.c_str(), currentCallTipWord.length(), pos, pos - rootlen);
		FillFunctionDefinition(pos);
		*/
		return true;
	}

	static bool IsCallTipSeparator(char ch) 
	{
		return (ch == ',') || (ch == ';');
	}

	void ContinueCallTip() 
	{
		//T* pT = static_cast<T*>(this);
		//todo:
		/*
		char linebuf[1000];
		GetLine(linebuf, sizeof(linebuf));
		int current = GetCaretInLine();

		int braces = 0;
		int commas = 0;
		for (int i = startCalltipWord; i < current; i++) {
			if (linebuf[i] == '(')
				braces++;
			else if (linebuf[i] == ')' && braces > 0)
				braces--;
			else if (braces == 1 && IsCallTipSeparator(linebuf[i]))
				commas++;
		}

		int startHighlight = 0;
		while (functionDefinition[startHighlight] && functionDefinition[startHighlight] != '(')
			startHighlight++;
		if (functionDefinition[startHighlight] == '(')
			startHighlight++;
		while (functionDefinition[startHighlight] && commas > 0) {
			if (IsCallTipSeparator(functionDefinition[startHighlight]))
				commas--;
			// If it reached the end of the argument list it means that the user typed in more
			// arguments than the ones listed in the calltip
			if (functionDefinition[startHighlight] == ')')
				commas = 0;
			else
				startHighlight++;
		}
		if (IsCallTipSeparator(functionDefinition[startHighlight]))
			startHighlight++;
		int endHighlight = startHighlight;
		while (functionDefinition[endHighlight] && !IsCallTipSeparator(functionDefinition[endHighlight]) && functionDefinition[endHighlight] != ')')
			endHighlight++;

		SendEditor(SCI_CALLTIPSETHLT, startHighlight, endHighlight);
		*/
	}

	bool StartAutoComplete() 
	{
		T* pT = static_cast<T*>(this);
		char linebuf[1000];
		GetLine(linebuf, sizeof(linebuf));
		int current = pT->GetCaretInLine();

		int startword = current;

		while ((startword > 0) && (pT->m_WordCharacters.find(linebuf[startword - 1]) != std::string::npos || m_autoCompleteStartCharacters.find(linebuf[startword - 1]) != std::string::npos))
			startword--;

		linebuf[current] = '\0';
		const char *root = linebuf + startword;
		int rootlen = current - startword;
		//todo
		/*
		if (apis) 
		{
			char *words = apis.GetNearestWords(root, rootlen, autoCompleteIgnoreCase);
			if (words) 
			{
				EliminateDuplicateWords(words);
				SendEditorString(SCI_AUTOCSHOW, rootlen, words);
				delete []words;
			}
		}
		*/
		return true;
	}

	bool HandleXml(char /*ch*/) 
	{
		return false;
	}

	IndentationStatus GetIndentState(int line) 
	{
		//T* pT = static_cast<T*>(this);
		// C like language indentation defined by braces and keywords
		IndentationStatus indentState = isNone;
		std::string controlWords[20];
		unsigned int parts = GetLinePartsInStyle(line, m_statementIndent.styleNumber,	-1, controlWords, ELEMENTS(controlWords));
		for (unsigned int i = 0; i < parts; i++) 
		{
			if (includes(m_statementIndent, controlWords[i]))
				indentState = isKeyWordStart;
		}
		// Braces override keywords
		std::string controlStrings[20];
		parts = GetLinePartsInStyle(line, m_blockEnd.styleNumber,	-1, controlStrings, ELEMENTS(controlStrings));
		for (unsigned int j = 0; j < parts; j++) 
		{
			if (includes(m_blockEnd, controlStrings[j]))
				indentState = isBlockEnd;
			if (includes(m_blockStart, controlStrings[j]))
				indentState = isBlockStart;
		}
		return indentState;
	}

	int IndentOfBlock(int line) 
	{
		if (line < 0)
			return 0;
		T* pT = static_cast<T*>(this);
		int indentSize = pT->GetIndent();
		int indentBlock = pT->GetLineIndentation(line);
		int backLine = line;
		IndentationStatus indentState = isNone;
		if (m_statementIndent.IsEmpty() && m_blockStart.IsEmpty() && m_blockEnd.IsEmpty())
			indentState = isBlockStart;	// Don't bother searching backwards

		int lineLimit = line - m_statementLookback;
		if (lineLimit < 0)
			lineLimit = 0;
		while ((backLine >= lineLimit) && (indentState == 0)) {
			indentState = GetIndentState(backLine);
			if (indentState != 0) {
				indentBlock = pT->GetLineIndentation(backLine);
				if (indentState == isBlockStart) {
					if (!m_indentOpening)
						indentBlock += indentSize;
				}
				if (indentState == isBlockEnd) {
					if (m_indentClosing)
						indentBlock -= indentSize;
					if (indentBlock < 0)
						indentBlock = 0;
				}
				if ((indentState == isKeyWordStart) && (backLine == line))
					indentBlock += indentSize;
			}
			backLine--;
		}
		return indentBlock;
	}

	void MaintainIndentation(char ch) 
	{
		T* pT = static_cast<T*>(this);
		int eolMode = pT->GetEOLMode();
		int curLine = GetCurrentLineNumber();
		int lastLine = curLine - 1;
		int indentAmount = 0;

		if (((eolMode == SC_EOL_CRLF || eolMode == SC_EOL_LF) && ch == '\n') ||	(eolMode == SC_EOL_CR && ch == '\r')) 
		{
			if (m_indentAutoMaintain) 
			{
				while (lastLine >= 0 && pT->GetLineLength(lastLine) == 0)
					lastLine--;
			}
			if (lastLine >= 0) 
			{
				indentAmount = pT->GetLineIndentation(lastLine);
			}
			if (indentAmount > 0) 
			{
				pT->SetLineIndentation(curLine, indentAmount);
				int d = pT->GetLineIndentPosition(curLine);
				pT->SetSel(d, d);
			}
		}
	}

	void AutomaticIndentation(char ch) 
	{
		T* pT = static_cast<T*>(this);
		Scintilla::CharacterRange crange = pT->GetSelection();
		int selStart = crange.cpMin;
		int curLine = GetCurrentLineNumber();
		int thisLineStart = pT->PositionFromLine(curLine);
		int indentSize = pT->GetIndent();
		int indentBlock = IndentOfBlock(curLine - 1);

		if (m_blockEnd.IsSingleChar() && ch == m_blockEnd.words[0]) 
		{	// Dedent maybe
			if (!m_indentClosing) 
			{
				if (RangeIsAllWhitespace(thisLineStart, selStart - 1)) 
				{
					pT->SetLineIndentation(curLine, indentBlock - indentSize);
				}
			}
		} 
		else if (!m_blockEnd.IsSingleChar() && (ch == ' ')) 
		{	// Dedent maybe
			if (!m_indentClosing && (GetIndentState(curLine) == isBlockEnd)) 
			{
			}
		}
		else if (ch == m_blockStart.words[0]) 
		{	// Dedent maybe if first on line and previous line was starting keyword
			if (!m_indentOpening && (GetIndentState(curLine - 1) == isKeyWordStart)) 
			{
				if (RangeIsAllWhitespace(thisLineStart, selStart - 1)) 
				{
					pT->SetLineIndentation(curLine, indentBlock - indentSize);
				}
			}
		} 
		else if ((ch == '\r' || ch == '\n') && (selStart == thisLineStart)) 
		{
			if (!m_indentClosing && !m_blockEnd.IsSingleChar()) 
			{	// Dedent previous line maybe
				std::string controlWords[1];
				if (GetLinePartsInStyle(curLine - 1, m_blockEnd.styleNumber, -1, controlWords, ELEMENTS(controlWords))) 
				{
					if (includes(m_blockEnd, controlWords[0])) 
					{
						// Check if first keyword on line is an ender
						pT->SetLineIndentation(curLine - 1, IndentOfBlock(curLine - 2) - indentSize);
						// Recalculate as may have changed previous line
						indentBlock = IndentOfBlock(curLine - 1);
					}
				}
			}
			pT->SetLineIndentation(curLine, indentBlock);
		}
	}

	void MatchBrace() 
	{
		T* pT = static_cast<T*>(this);
		int braceAtCaret = -1;
		int braceOpposite = -1;
		FindMatchingBracePosition(braceAtCaret, braceOpposite, m_bracesSloppy);
		if ((braceAtCaret != -1) && (braceOpposite == -1)) 
		{
			pT->BraceBadLight(braceAtCaret);
			pT->SetHighlightGuide(0);
		} 
		else 
		{
			char chBrace = static_cast<char>(pT->GetCharAt(braceAtCaret));
			pT->BraceHighlight(braceAtCaret, braceOpposite);
			int columnAtCaret = pT->GetColumn(braceAtCaret);
			int columnOpposite = pT->GetColumn(braceOpposite);
			if (chBrace == ':') 
			{
				int lineStart = pT->LineFromPosition(braceAtCaret);
				int indentPos = pT->GetLineIndentPosition(lineStart);
				int indentPosNext = pT->GetLineIndentPosition(lineStart + 1);
				columnAtCaret = pT->GetColumn(indentPos);
				int columnAtCaretNext = pT->GetColumn(indentPosNext);
				int indentSize = pT->GetIndent();
				if (columnAtCaretNext - indentSize > 1)
					columnAtCaret = columnAtCaretNext - indentSize;
				//Platform::DebugPrintf(": %d %d %d\n", lineStart, indentPos, columnAtCaret);
				if (columnOpposite == 0)	// If the final line of the structure is empty
					columnOpposite = columnAtCaret;
			}

			pT->SetHighlightGuide(Minimum(columnAtCaret, columnOpposite));
		}
	}

	void FoldChanged(int line, int levelNow, int levelPrev) 
	{
		T* pT = static_cast<T*>(this);
		//Platform::DebugPrintf("Fold %d %x->%x\n", line, levelPrev, levelNow);
		if (levelNow & SC_FOLDLEVELHEADERFLAG) 
		{
			if (!(levelPrev & SC_FOLDLEVELHEADERFLAG)) 
			{
				pT->SetFoldExpanded(line, true);
			}
		} 
		else if (levelPrev & SC_FOLDLEVELHEADERFLAG) 
		{
			//Platform::DebugPrintf("Fold removed %d-%d\n", line, SendEditor(SCI_GETLASTCHILD, line));
			if (!pT->GetFoldExpanded(line)) 
			{
				// Removing the fold from one that has been contracted so should expand
				// otherwise lines are left invisible with no way to make them visible
				Expand(line, true, false, 0, levelPrev);
			}
		}
	}

	void Expand(int &line, bool doExpand, bool force = false, int visLevels = 0, int level = -1) 
	{
		T* pT = static_cast<T*>(this);
		int lineMaxSubord = pT->GetLastChild(line, level & SC_FOLDLEVELNUMBERMASK);
		line++;
		while (line <= lineMaxSubord) 
		{
			if (force) 
			{
				if (visLevels > 0)
					pT->ShowLines(line, line);
				else
					pT->HideLines(line, line);
			} 
			else 
			{
				if (doExpand)
					pT->ShowLines(line, line);
			}
			int levelLine = level;
			if (levelLine == -1)
				levelLine = pT->GetFoldLevel(line);
			if (levelLine & SC_FOLDLEVELHEADERFLAG) 
			{
				if (force) 
				{
					if (visLevels > 1)
						pT->SetFoldExpanded(line, 1);
					else
						pT->SetFoldExpanded(line, 0);
					Expand(line, doExpand, force, visLevels - 1);
				} 
				else 
				{
					if (doExpand) 
					{
						if (!pT->GetFoldExpanded(line))
							pT->SetFoldExpanded(line, 1);
						Expand(line, true, force, visLevels - 1);
					} 
					else 
					{
						Expand(line, false, force, visLevels - 1);
					}
				}
			} 
			else 
			{
				line++;
			}
		}
	}

	bool MarginClick(int position, int modifiers) 
	{
		T* pT = static_cast<T*>(this);
		int lineClick = pT->LineFromPosition(position);
		//Platform::DebugPrintf("Margin click %d %d %x\n", position, lineClick,
		//	SendEditor(SCI_GETFOLDLEVEL, lineClick) & SC_FOLDLEVELHEADERFLAG);
		if ((modifiers & SCMOD_SHIFT) && (modifiers & SCMOD_CTRL)) 
		{
			FoldAll();
		} 
		else 
		{
			int levelClick = pT->GetFoldLevel(lineClick);
			if (levelClick & SC_FOLDLEVELHEADERFLAG) 
			{
				if (modifiers & SCMOD_SHIFT) 
				{
					// Ensure all children visible
					pT->SetFoldExpanded(lineClick, true);
					Expand(lineClick, true, true, 100, levelClick);
				} 
				else if (modifiers & SCMOD_CTRL) 
				{
					if (pT->GetFoldExpanded(lineClick)) 
					{
						// Contract this line and all children
						pT->SetFoldExpanded(lineClick, false);
						Expand(lineClick, false, true, 0, levelClick);
					} 
					else 
					{
						// Expand this line and all children
						pT->SetFoldExpanded(lineClick, 1);
						Expand(lineClick, true, true, 100, levelClick);
					}
				} 
				else 
				{
					// Toggle this line
					pT->ToggleFold(lineClick);
				}
			}
		}
		return true;
	}

	void FoldAll() 
	{
		T* pT = static_cast<T*>(this);
		pT->Colourise(0, -1);
		int maxLine = pT->GetLineCount();
		bool expanding = true;
		for (int lineSeek = 0; lineSeek < maxLine; lineSeek++) 
		{
			if (pT->GetFoldLevel(lineSeek) & SC_FOLDLEVELHEADERFLAG) 
			{
				expanding = !pT->GetFoldExpanded(lineSeek);
				break;
			}
		}

		for (int line = 0; line < maxLine; line++) 
		{
			int level = pT->GetFoldLevel(line);
			if ((level & SC_FOLDLEVELHEADERFLAG) && (SC_FOLDLEVELBASE == (level & SC_FOLDLEVELNUMBERMASK))) 
			{
				if (expanding) 
				{
					pT->SetFoldExpanded(line, 1);
					Expand(line, true, false, 0, level);
					line--;
				} 
				else 
				{
					int lineMaxSubord = pT->GetLastChild(line, -1);
					pT->SetFoldExpanded(line, 0);
					if (lineMaxSubord > line)
						pT->HideLines(line + 1, lineMaxSubord);
				}
			}
		}
	}

	void SetLineNumberWidth() 
	{
		T* pT = static_cast<T*>(this);
		if (pT->GetMarginWidthN(0))
		{
			// The 4 here allows for spacing: 1 poxel on left and 3 on right.
			int pixelWidth = 4 + m_lineNumbersWidth * pT->TextWidth(STYLE_LINENUMBER, _T("9"));
			pT->SetMarginWidthN(0, pixelWidth);
		}
	}

	void RangeExtendAndGrab(CString & sel, int &selStart, int &selEnd, int lengthDoc, bool (*ischarforsel)(char ch), bool stripEol=true) 
	{
		char buff[200];
		_RangeExtendAndGrab(buff, sizeof(buff), selStart, selEnd, lengthDoc, ischarforsel, stripEol);
		sel = CA2T(buff);
	}

	void _RangeExtendAndGrab(char *sel, int len, int &selStart, int &selEnd, int lengthDoc, bool (*ischarforsel)(char ch), bool stripEol=true) 
	{
		T* pT = static_cast<T*>(this);
		if (selStart == selEnd && ischarforsel) 
		{
			// Try and find a word at the caret
			while ((selStart > 0) && (ischarforsel(pT->GetCharAt(selStart - 1)))) 
			{
				selStart--;
			}
			while ((selEnd < lengthDoc) && (ischarforsel(pT->GetCharAt(selEnd)))) 
			{
				selEnd++;
			}
		}
		sel[0] = '\0';
		if (selEnd - selStart + 1 > len - 1) 
			selEnd = selStart + len - 1;
		if (selStart < selEnd) 
			GetRange(selStart, selEnd, sel);
		if (stripEol) 
		{
			// Change whole line selected but normally end of line characters not wanted.
			// Remove possible terminating \r, \n, or \r\n.
			size_t sellen = strlen(sel);
			if (sellen >= 2 && (sel[sellen - 2] == '\r' && sel[sellen - 1] == '\n')) {
				sel[sellen - 2] = '\0';
			} else if (sellen >= 1 && (sel[sellen - 1] == '\r' || sel[sellen - 1] == '\n')) {
				sel[sellen - 1] = '\0';
			}
		}
	}

	void DoPrint(const std::_tstring & title)
	{
		T* pT = static_cast<T*>(this);
		PRINTDLG pdlg = {
			sizeof(PRINTDLG), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		};
		pdlg.hwndOwner = pT->m_hWnd;
		pdlg.hInstance = _Module.GetModuleInstance();
		pdlg.Flags = PD_USEDEVMODECOPIES | PD_ALLPAGES | PD_RETURNDC;
		pdlg.nFromPage = 1;
		pdlg.nToPage = 1;
		pdlg.nMinPage = 1;
		pdlg.nMaxPage = 0xffffU; // We do not know how many pages in the
		// document until the printer is selected and the paper size is known.
		pdlg.nCopies = 1;
		pdlg.hDC = 0;
		pdlg.hDevMode = m_hDevMode;
		pdlg.hDevNames = m_hDevNames;

		// See if a range has been selected
		Scintilla::CharacterRange crange = pT->GetSelection();
		int startPos = crange.cpMin;
		int endPos = crange.cpMax;

		if (startPos == endPos) {
			pdlg.Flags |= PD_NOSELECTION;
		} else {
			pdlg.Flags |= PD_SELECTION;
		}
		if (!::PrintDlg(&pdlg)) {
			return;
		}

		m_hDevMode = pdlg.hDevMode;
		m_hDevNames = pdlg.hDevNames;

		HDC hdc = pdlg.hDC;

		Scintilla::PRectangle rectMargins, rectPhysMargins;
		Scintilla::Point ptPage;
		Scintilla::Point ptDpi;

		// Get printer resolution
		ptDpi.x = GetDeviceCaps(hdc, LOGPIXELSX);    // dpi in X direction
		ptDpi.y = GetDeviceCaps(hdc, LOGPIXELSY);    // dpi in Y direction

		// Start by getting the physical page size (in device units).
		ptPage.x = GetDeviceCaps(hdc, PHYSICALWIDTH);   // device units
		ptPage.y = GetDeviceCaps(hdc, PHYSICALHEIGHT);  // device units

		// Get the dimensions of the unprintable
		// part of the page (in device units).
		rectPhysMargins.left = GetDeviceCaps(hdc, PHYSICALOFFSETX);
		rectPhysMargins.top = GetDeviceCaps(hdc, PHYSICALOFFSETY);

		// To get the right and lower unprintable area,
		// we take the entire width and height of the paper and
		// subtract everything else.
		rectPhysMargins.right = ptPage.x						// total paper width
			- GetDeviceCaps(hdc, HORZRES) // printable width
			- rectPhysMargins.left;				// left unprintable margin

		rectPhysMargins.bottom = ptPage.y						// total paper height
			- GetDeviceCaps(hdc, VERTRES)	// printable height
			- rectPhysMargins.top;				// right unprintable margin

		// At this point, rectPhysMargins contains the widths of the
		// unprintable regions on all four sides of the page in device units.

		// Take in account the page setup given by the user (if one value is not null)
		if (m_pagesetupMargin.left != 0 || m_pagesetupMargin.right != 0 || m_pagesetupMargin.top != 0 || m_pagesetupMargin.bottom != 0) 
		{
			Scintilla::PRectangle rectSetup;

			// Convert the hundredths of millimeters (HiMetric) or
			// thousandths of inches (HiEnglish) margin values
			// from the Page Setup dialog to device units.
			// (There are 2540 hundredths of a mm in an inch.)

			TCHAR localeInfo[3];
			GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_IMEASURE, localeInfo, 3);

			if (localeInfo[0] == '0') {	// Metric system. '1' is US System
				rectSetup.left = MulDiv (m_pagesetupMargin.left, ptDpi.x, 2540);
				rectSetup.top = MulDiv (m_pagesetupMargin.top, ptDpi.y, 2540);
				rectSetup.right	= MulDiv(m_pagesetupMargin.right, ptDpi.x, 2540);
				rectSetup.bottom	= MulDiv(m_pagesetupMargin.bottom, ptDpi.y, 2540);
			} else {
				rectSetup.left	= MulDiv(m_pagesetupMargin.left, ptDpi.x, 1000);
				rectSetup.top	= MulDiv(m_pagesetupMargin.top, ptDpi.y, 1000);
				rectSetup.right	= MulDiv(m_pagesetupMargin.right, ptDpi.x, 1000);
				rectSetup.bottom	= MulDiv(m_pagesetupMargin.bottom, ptDpi.y, 1000);
			}

			// Dont reduce margins below the minimum printable area
			rectMargins.left	= pT->Maximum(rectPhysMargins.left, rectSetup.left);
			rectMargins.top		= pT->Maximum(rectPhysMargins.top, rectSetup.top);
			rectMargins.right	= pT->Maximum(rectPhysMargins.right, rectSetup.right);
			rectMargins.bottom	= pT->Maximum(rectPhysMargins.bottom, rectSetup.bottom);
		} else {
			rectMargins.left	= rectPhysMargins.left;
			rectMargins.top		= rectPhysMargins.top;
			rectMargins.right	= rectPhysMargins.right;
			rectMargins.bottom	= rectPhysMargins.bottom;
		}

		// rectMargins now contains the values used to shrink the printable
		// area of the page.

		// Convert device coordinates into logical coordinates
		DPtoLP(hdc, (LPPOINT)&rectMargins, 2);
		DPtoLP(hdc, (LPPOINT)&rectPhysMargins, 2);

		// Convert page size to logical units and we're done!
		DPtoLP(hdc, (LPPOINT)&ptPage, 1);

		std::_tstring headerFormat = _T("%s");//props.Get("print.header.format");
		std::_tstring footerFormat = _T("%d0");//props.Get("print.footer.format");

		TEXTMETRIC tm;
		std::_tstring headerOrFooter;	// Usually the path, date and page number

		std::_tstring headerStyle = _T("");//props.Get("print.header.style");

		int headerLineHeight = ::MulDiv(16, ptDpi.y, 72);
		HFONT fontHeader = ::CreateFont(headerLineHeight,
			0, 0, 0,
			FW_HEAVY,
			true,
			false,
			0, 0, 0,
			0, 0, 0,
			_T("Arial"));
		::SelectObject(hdc, fontHeader);
		::GetTextMetrics(hdc, &tm);
		headerLineHeight = tm.tmHeight + tm.tmExternalLeading;

		std::_tstring footerStyle = _T("");//props.Get("print.footer.style");
		//StyleDefinition sdFooter(footerStyle.c_str());

		int footerLineHeight = ::MulDiv(9,
			ptDpi.y, 72);
		HFONT fontFooter = ::CreateFont(footerLineHeight,
			0, 0, 0,
			FW_NORMAL,
			false,
			false,
			0, 0, 0,
			0, 0, 0,
			_T("Arial"));
		::SelectObject(hdc, fontFooter);
		::GetTextMetrics(hdc, &tm);
		footerLineHeight = tm.tmHeight + tm.tmExternalLeading;

		DOCINFO di = {sizeof(DOCINFO), 0, 0, 0, 0};
		di.lpszDocName = title.c_str();
		di.lpszOutput = 0;
		di.lpszDatatype = 0;
		di.fwType = 0;
		if (::StartDoc(hdc, &di) < 0) {
			std::_tstring msg = _T("Can not start printer document.");
			pT->MessageBox(msg.c_str(), _T("Warning"));
			return;
		}

		LONG lengthDoc = pT->GetLength();
		LONG lengthDocMax = lengthDoc;
		LONG lengthPrinted = 0;

		// Requested to print selection
		if (pdlg.Flags & PD_SELECTION) {
			if (startPos > endPos) {
				lengthPrinted = endPos;
				lengthDoc = startPos;
			} else {
				lengthPrinted = startPos;
				lengthDoc = endPos;
			}

			if (lengthPrinted < 0)
				lengthPrinted = 0;
			if (lengthDoc > lengthDocMax)
				lengthDoc = lengthDocMax;
		}

		// We must substract the physical margins from the printable area
		Scintilla::RangeToFormat frPrint;
		frPrint.hdc = hdc;
		frPrint.hdcTarget = hdc;
		frPrint.rc.left = rectMargins.left - rectPhysMargins.left;
		frPrint.rc.top = rectMargins.top - rectPhysMargins.top;
		frPrint.rc.right = ptPage.x - rectMargins.right - rectPhysMargins.left;
		frPrint.rc.bottom = ptPage.y - rectMargins.bottom - rectPhysMargins.top;
		frPrint.rcPage.left = 0;
		frPrint.rcPage.top = 0;
		frPrint.rcPage.right = ptPage.x - rectPhysMargins.left - rectPhysMargins.right - 1;
		frPrint.rcPage.bottom = ptPage.y - rectPhysMargins.top - rectPhysMargins.bottom - 1;
		if (headerFormat.size()) {
			frPrint.rc.top += headerLineHeight + headerLineHeight / 2;
		}
		if (footerFormat.size()) {
			frPrint.rc.bottom -= footerLineHeight + footerLineHeight / 2;
		}
		// Print each page
		int pageNum = 1;
		bool printPage;
		//PropSet propsPrint;
		//propsPrint.superPS = &m_props;
		//SetFileProperties(propsPrint);

		while (lengthPrinted < lengthDoc) {
			printPage = (!(pdlg.Flags & PD_PAGENUMS) ||
				(pageNum >= pdlg.nFromPage) && (pageNum <= pdlg.nToPage));

			char pageString[32];
			sprintf(pageString, "%0d", pageNum);
			//propsPrint.Set("CurrentPage", pageString);

			if (printPage) {
				::StartPage(hdc);

				if (headerFormat.size()) {
					std::_tstring sHeader = title;
					::SetTextColor(hdc, 0x000000);
					::SetBkColor(hdc, 0xffffff);
					::SelectObject(hdc, fontHeader);
					UINT ta = ::SetTextAlign(hdc, TA_BOTTOM);
					RECT rcw = {frPrint.rc.left, frPrint.rc.top - headerLineHeight - headerLineHeight / 2,
						frPrint.rc.right, frPrint.rc.top - headerLineHeight / 2};
					rcw.bottom = rcw.top + headerLineHeight;
					::ExtTextOut(hdc, frPrint.rc.left + 5, frPrint.rc.top - headerLineHeight / 2,
						ETO_OPAQUE, &rcw, sHeader.c_str(),
						static_cast<int>(sHeader.length()), NULL);
					::SetTextAlign(hdc, ta);
					HPEN pen = ::CreatePen(0, 1, 0x000000);
					HPEN penOld = static_cast<HPEN>(::SelectObject(hdc, pen));
					::MoveToEx(hdc, frPrint.rc.left, frPrint.rc.top - headerLineHeight / 4, NULL);
					::LineTo(hdc, frPrint.rc.right, frPrint.rc.top - headerLineHeight / 4);
					::SelectObject(hdc, penOld);
					::DeleteObject(pen);
				}
			}

			frPrint.chrg.cpMin = lengthPrinted;
			frPrint.chrg.cpMax = lengthDoc;

			lengthPrinted = pT->FormatRange(printPage, reinterpret_cast<LPARAM>(&frPrint));

			if (printPage) {
				if (footerFormat.size()) {
					std::_tstring version;
					GetAboutVersion(version);
					std::_tstring sFooter = boost::lexical_cast<std::_tstring>(pageNum).c_str();//propsPrint.GetExpanded("print.footer.format");
					sFooter += _T(" - ECL IDE Version ");
					sFooter += version;

					::SetTextColor(hdc, 0x000000);
					::SetBkColor(hdc, 0xffffff);
					::SelectObject(hdc, fontFooter);
					UINT ta = ::SetTextAlign(hdc, TA_TOP);
					RECT rcw = {frPrint.rc.left, frPrint.rc.bottom + footerLineHeight / 2,
						frPrint.rc.right, frPrint.rc.bottom + footerLineHeight + footerLineHeight / 2};
					::ExtTextOut(hdc, frPrint.rc.left + 5, frPrint.rc.bottom + footerLineHeight / 2,
						ETO_OPAQUE, &rcw, sFooter.c_str(),
						static_cast<int>(sFooter.length()), NULL);
					::SetTextAlign(hdc, ta);
					HPEN pen = ::CreatePen(0, 1, 0x000000);
					HPEN penOld = static_cast<HPEN>(::SelectObject(hdc, pen));
					::SetBkColor(hdc, 0x000000);
					::MoveToEx(hdc, frPrint.rc.left, frPrint.rc.bottom + footerLineHeight / 4, NULL);
					::LineTo(hdc, frPrint.rc.right, frPrint.rc.bottom + footerLineHeight / 4);
					::SelectObject(hdc, penOld);
					::DeleteObject(pen);
				}

				::EndPage(hdc);
			}
			pageNum++;

			if ((pdlg.Flags & PD_PAGENUMS) && (pageNum > pdlg.nToPage))
				break;
		}

		pT->FormatRange(FALSE, 0);

		::EndDoc(hdc);
		::DeleteDC(hdc);
		if (fontHeader) {
			::DeleteObject(fontHeader);
		}
		if (fontFooter) {
			::DeleteObject(fontFooter);
		}
		return;
	}
};
