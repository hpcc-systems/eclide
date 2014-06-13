#pragma once

#include "GotoDlg.h" //wlib
#include "FindDlgMgr.h" //wlib
#include <PersistMap.h> //clib
#include <Dali.h> //comms
#include <Attribute.h> //comms
#include "AttrDependecy.h"
#include "thread.h"
#include "SourceView.h"
#include "PreferenceDlg.h"
#include <CustomMessages.h>

class CCmdUI;

template<class T>
class CEclCommandMixin : public CMessageMap
{
private:
	WTL::CUpdateUIBase *m_UpdateUI;
	CSourceCtrl	  *m_ecl;
	IFindReplace * m_findReplace;

public:
	ISourceSlot	  *m_eclSlot;
	#define CEclCommandMixinMAP_ID 10

	BEGIN_MSG_MAP(CEclCommandMixin< T >)
	ALT_MSG_MAP(CEclCommandMixinMAP_ID)
		MESSAGE_HANDLER(CWM_SUBMITDONE, OnSubmitDone)
		MESSAGE_HANDLER(CWM_INSERTTEXT, OnInsertText)
		MESSAGE_HANDLER(CWM_SYNTAXCLEAR, OnClearSyntax)
		MESSAGE_HANDLER(CWM_SYNTAXSYNC, OnSyncSyntax)

		//These need to be handled at a higher level
		//COMMAND_ID_HANDLER(ID_ECL_CHECKSYNTAX, OnEclCheckSyntax)

#ifdef ID_EDIT_UNDO
		COMMAND_ID_HANDLER(ID_EDIT_UNDO, OnEditUndo)
		COMMAND_ID_HANDLER(ID_EDIT_REDO, OnEditRedo)
		COMMAND_ID_HANDLER(ID_EDIT_CUT, OnEditCut)
		COMMAND_ID_HANDLER(ID_EDIT_COPY, OnEditCopy)
		COMMAND_ID_HANDLER(ID_REPOSITORY_COPY, OnRepositoryCopy)
		COMMAND_ID_HANDLER(ID_EDIT_PASTE, OnEditPaste)
		COMMAND_ID_HANDLER(ID_EDIT_DELETE, OnEditDelete)
		COMMAND_ID_HANDLER(ID_EDIT_SELECT_ALL, OnEditSelectAll)
#ifdef ID_EDIT_MATCHBRACE
		COMMAND_ID_HANDLER(ID_EDIT_MATCHBRACE, OnEditMatchBrace)
		COMMAND_ID_HANDLER(ID_EDIT_SELECTTOBRACE, OnEditSelectToBrace)
		COMMAND_ID_HANDLER(ID_EDIT_COMPLETEWORD, OnEditCompleteWord)
		COMMAND_ID_HANDLER(ID_EDIT_BLOCKCOMMENT, OnEditBlockComment)
		COMMAND_ID_HANDLER(ID_EDIT_BOXCOMMENT, OnEditBoxComment)
		COMMAND_ID_HANDLER(ID_EDIT_UPPERCASE, OnEditUppercase)
		COMMAND_ID_HANDLER(ID_EDIT_LOWERCASE, OnEditLowercase)
		COMMAND_ID_HANDLER(ID_EDIT_TOGGLELINENUMBERS, OnEditToggleLineNumbers)
		COMMAND_ID_HANDLER(ID_ADVANCED_SETTABSIZE, OnEditSetTabSize)
		COMMAND_ID_HANDLER(ID_EDIT_REFORMAT, OnEditReformat)
		COMMAND_ID_HANDLER(ID_EDIT_WORDWRAP, OnEditWordWrap)
		COMMAND_ID_HANDLER(ID_EDIT_FIND, OnEditFind)
		COMMAND_ID_HANDLER(ID_EDIT_REPLACE, OnEditReplace)
		COMMAND_ID_HANDLER(ID_EDIT_FINDNEXT, OnEditFindNext)
		COMMAND_ID_HANDLER(ID_EDIT_FINDPREV, OnEditFindPrev)
		COMMAND_ID_HANDLER(ID_EDIT_GOTO, OnEditGoto)
#endif
#endif

#ifdef ID_ECL_NEXTERROR
		COMMAND_ID_HANDLER(ID_ECL_NEXTERROR, OnEclNextError)
		COMMAND_ID_HANDLER(ID_ECL_PREVIOUSERROR, OnEclPreviousError)
		COMMAND_ID_HANDLER(ID_ECL_CLEARERRORS, OnEclClearErrors)
#endif
		COMMAND_ID_HANDLER(ID_MACRO_RECORD, OnMacroRecord)
		COMMAND_ID_HANDLER(ID_MACRO_PLAY, OnMacroPlay)

		MESSAGE_HANDLER(WM_COMMAND, OnFindReplaceCmd)

		if(m_ecl && m_ecl->ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult))
			return TRUE;

	END_MSG_MAP()

	CEclCommandMixin()
	{
		m_ecl = 0;
		m_UpdateUI = 0;
		m_eclSlot = NULL;
	}

	void InitEclCommandMixin(WTL::CUpdateUIBase *UIBase, IFindReplace * findReplace, CSourceCtrl *ecl, ISourceSlot* eclSlot) 
	{
		m_UpdateUI = UIBase;
		m_findReplace = findReplace;
		m_ecl = ecl;
		m_eclSlot = eclSlot;
	}

	void UIUpdateMenuItems(WTL::CUpdateUIBase *menuManager)
	{
		m_UpdateUI = menuManager;
		UIUpdateMenuItems();
	}

	bool UIUpdateMenuItems(CCmdUI * cui)
	{
		T * pT = static_cast<T*>(this);
		bool bCanSubmit = pT->CanExecute();
		bool bHasContents = (m_ecl->GetLength() > 0);
		bool bReadOnly = m_ecl->IsReadOnly() == TRUE;
		bool bHasSelection = (m_ecl->GetSelectionEnd() - m_ecl->GetSelectionStart() > 0);
		bool bCanCut = bHasSelection && !bReadOnly;

		UPDATEUI(cui, ID_EDIT_UNDO, m_ecl->CanUndo());
		UPDATEUI(cui, ID_EDIT_REDO, m_ecl->CanRedo());
		UPDATEUI(cui, ID_EDIT_COPY, bHasSelection);
		UPDATEUI(cui, ID_EDIT_CUT, bCanCut );
		UPDATEUI(cui, ID_EDIT_DELETE, true);
		UPDATEUI(cui, ID_EDIT_PASTE, m_ecl->CanPaste());
		UPDATEUI(cui, ID_EDIT_SELECT_ALL, bHasContents);
		UPDATEUI(cui, ID_EDIT_MATCHBRACE, bHasContents);
		UPDATEUI(cui, ID_EDIT_SELECTTOBRACE, bHasContents);
		UPDATEUI(cui, ID_EDIT_COMPLETEWORD, bHasContents && !bReadOnly);
		UPDATEUI(cui, ID_EDIT_BLOCKCOMMENT, bHasContents && !bReadOnly);
		UPDATEUI(cui, ID_EDIT_BOXCOMMENT, bHasContents && !bReadOnly);
		UPDATEUI(cui, ID_EDIT_UPPERCASE, bHasSelection && !bReadOnly);
		UPDATEUI(cui, ID_EDIT_LOWERCASE, bHasSelection && !bReadOnly);

		if (cui->m_nID == ID_EDIT_TOGGLELINENUMBERS)
		{
			cui->Enable(true);
			cui->SetCheck(m_ecl->GetMarginWidthN(0) > 0);
			return true;
		}

		UPDATEUI(cui, ID_ADVANCED_SETTABSIZE, true);
		UPDATEUI(cui, ID_EDIT_REFORMAT, bHasContents && !bReadOnly);

		if (cui->m_nID == ID_EDIT_WORDWRAP)
		{
			cui->Enable(true);
			cui->SetCheck(m_ecl->GetWrapMode() > 0);
			return true;
		}

		UPDATEUI(cui, ID_EDIT_FIND, bHasContents);
		UPDATEUI(cui, ID_EDIT_FINDNEXT, (bHasContents && GetFindReplace().m_szFindWhat[0] != 0));
		UPDATEUI(cui, ID_EDIT_FINDPREV, (bHasContents && GetFindReplace().m_szFindWhat[0] != 0));
		UPDATEUI(cui, ID_EDIT_REPLACE, bHasContents && !bReadOnly);
		UPDATEUI(cui, ID_EDIT_GOTO, bHasContents);

		if (cui->m_nID == ID_EDIT_FINDREPLACECMD)
			return true;

		UPDATEUI(cui, ID_ECL_NEXTERROR, m_ecl->SyntaxHasNext());
		UPDATEUI(cui, ID_ECL_PREVIOUSERROR, m_ecl->SyntaxHasPrevious());
		UPDATEUI(cui, ID_ECL_CLEARERRORS, m_ecl->SyntaxGetCount() > 0);
		UPDATEUI(cui, ID_ECL_GOTO, bHasContents);
		UPDATEUI(cui, ID_ECL_CHECKCOMPLEXITY, bCanSubmit);
		UPDATEUI(cui, ID_ECL_CHECKDEPENDENCY, bCanSubmit);

		//we really shouldn't be setting these since we don't handle them
		UPDATEUI(cui, ID_FILE_SAVE, m_ecl->GetModify());	
		UPDATEUI(cui, ID_FILE_PRINT, TRUE);
		UPDATEUI(cui, ID_ECL_CHECKSYNTAX, bCanSubmit);
		UPDATEUI(cui, ID_ECL_GOTOSYNCTOC, bHasContents);

		if (cui->m_nID == ID_MACRO_RECORD)
		{
			GetIMainFrame()->m_buttMacroRecord->SetImageIndex(!m_ecl->IsRecording() ? 47 : 48, false);
			cui->SetText(!m_ecl->IsRecording() ? _T("Record") : _T("Stop"));
		}
		UPDATEUI(cui, ID_MACRO_RECORD, true);
		UPDATEUI(cui, ID_MACRO_PLAY, !m_ecl->IsRecording() && m_ecl->HasRecording());
		return false;
	}

	void DoInit()
	{
		m_ecl->DoInit();
	}

	void DoEditFind()
	{
		CString text;
		m_ecl->GetSelText(text);
		if (!text.GetLength())
		{
			long pos = m_ecl->GetCurrentPos();
			long startPos = m_ecl->WordStartPosition(pos, true);
			long endPos = m_ecl->WordEndPosition(pos, true);
			m_ecl->GetTextRange(startPos, endPos, text);
		}
		m_findReplace->FindReplaceOnEditFind(std::_tstring(text), true, false);
	}

	void DoEditFindReplace()
	{
		CString text;
		m_ecl->GetSelText(text);
		ATLASSERT(m_findReplace);
		m_findReplace->FindReplaceOnEditReplace(std::_tstring(text));
	}

	void DoShowCurrentError()
	{
		if (m_eclSlot)
		{
			CString syntaxStatus;
			m_ecl->SyntaxGetMessage(syntaxStatus);
			m_eclSlot->RefreshSyntax(m_ecl, m_ecl->SyntaxGetCurrent(), false);
			m_eclSlot->PostStatus(syntaxStatus);
		}
	}

	void SyntaxClearAll()
	{
		m_ecl->SyntaxClearAll();
	}

	void DoCheckSyntax()
	{
		T * pT = static_cast<T*>(this);
		SyntaxClearAll();
		IAttribute* attr = pT->GetAttribute();
		if ( attr )
		{
			CString ecl;
			m_ecl->GetText(ecl);
			CString cluster(pT->GetCluster());
			CString module(attr->GetModuleLabel());
			CString attribute(attr->GetLabel());
			clib::thread run(__FUNCTION__, boost::bind(&EclCheckSyntax, pT, ecl, cluster, module, attribute));
		}
	}

	static void EclCheckSyntax(T * self, CString ecl, CString cluster, CString module, CString attr, CString path, CString debugString, bool archive, bool noCommonPrivateAttributes)
	{
		Dali::CEclExceptionVector errors;
		StlLinked<Dali::IDali> dali = Dali::AttachDali(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT), _T("Dali"));

		//  Always send submit done (even on failure)  ---
		dali->CheckSyntax(cluster, GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_QUEUE), module, attr, path, ecl, -1, debugString, archive, noCommonPrivateAttributes, errors);
		if (self->IsWindow())
			self->SendMessage(CWM_SUBMITDONE, Dali::WUActionCheck, (LPARAM)&errors);
	}

	void AnnotationClearAll()
	{
		m_ecl->AnnotationClearAll();
	}

	void SavePersistInfo(CPersistMap & persistInfo)
	{
		CString ecl;
		m_ecl->GetText(ecl);
		persistInfo.Set(PERSIST_ECL, static_cast<const TCHAR *>(ecl));
		persistInfo.Set(PERSIST_DIRTY, boost::lexical_cast<std::_tstring>(m_ecl->IsDirty()));
	}

	void RestorePersistInfo(const CPersistMap & persistInfo)
	{
		CString persistEcl = persistInfo.Get(PERSIST_ECL);
		CString currentEcl;
		m_ecl->GetText(currentEcl);
		if (persistEcl.Compare(currentEcl) != 0)
			m_ecl->SetText(persistEcl);
		else
		{
			m_ecl->EmptyUndoBuffer();
			m_ecl->SetSavePoint();
		}
	}

	LRESULT OnFindReplaceCmd(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
#ifdef ID_EDIT_FINDREPLACECMD
		if ( wParam != ID_EDIT_FINDREPLACECMD )
		{
			bHandled = FALSE;
			return 1;
		}
#endif

		//ATLASSERT(lParam == (LPARAM)&GetFindReplace);
		if(m_findReplace->GetFindReplace().FindNext())
		{
			if(!m_ecl->DoFind(m_findReplace->GetFindReplace().m_szFindWhat, m_findReplace->GetFindReplace().m_dwFlags, m_findReplace->GetFindReplace().m_mode, false, m_findReplace->GetFindReplace().SearchDown())) {
				//m_eclSlot->PostStatus(_T("Error. Can't found '%s'"), m_findReplace->GetFindReplace().m_szFindWhat); 
				::MessageBeep(MB_ICONEXCLAMATION);
			}
			else {		
				int nRow = 0; int nCol = 0;				
				nRow = m_ecl->LineFromPosition(m_ecl->GetCurrentPos());
				nCol = m_ecl->GetColumn(m_ecl->GetCurrentPos());

				//m_eclSlot->PostStatus(_T("Ok. Found '%s' in {%d, %d}"), m_findReplace->GetFindReplace().m_szFindWhat, nRow, nCol); 
			}
		}
		else if(m_findReplace->GetFindReplace().ReplaceCurrent())
		{
			if(!m_ecl->DoReplaceCurrent(m_findReplace->GetFindReplace().m_szFindWhat, m_findReplace->GetFindReplace().m_szReplaceWith, m_findReplace->GetFindReplace().m_dwFlags, m_findReplace->GetFindReplace().MatchCase()))
				::MessageBeep(MB_ICONEXCLAMATION);
		}
		else if(m_findReplace->GetFindReplace().ReplaceAll())
		{
			if(!m_ecl->DoReplaceAll(m_findReplace->GetFindReplace().m_szFindWhat, m_findReplace->GetFindReplace().m_szReplaceWith, m_findReplace->GetFindReplace().m_dwFlags))
				::MessageBeep(MB_ICONEXCLAMATION);
		}

		if(m_findReplace->GetFindReplace().IsTerminating()) {				
			// nothing
		}

		return 0;
	}

	LRESULT OnInsertText(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = true;
		CString * label = (CString *)lParam;
		m_ecl->InsertText(m_ecl->GetCurrentPos(), *label);
		return 0;
	}

	LRESULT OnClearSyntax(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = true;
		m_ecl->SyntaxClearAll();
		return 0;
	}

	LRESULT OnSyncSyntax(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = true;
		if (m_ecl->SyntaxGoto(wParam))
			DoShowCurrentError();
		return 0;
	}

	LRESULT OnEditUndo(UINT /*code*/, UINT /*id*/, HWND /*hwndControl*/, BOOL &bHandled)
	{
		bHandled = true;
		m_ecl->Undo();
		return 0;
	}
	LRESULT OnEditRedo(UINT /*code*/, UINT /*id*/, HWND /*hwndControl*/, BOOL &bHandled)
	{
		bHandled = true;
		m_ecl->Redo();
		return 0;
	}
	LRESULT OnEditCut(UINT /*code*/, UINT /*id*/, HWND /*hwndControl*/, BOOL &bHandled)
	{
		bHandled = true;
		m_ecl->Cut();
		return 0;
	}
	LRESULT OnEditCopy(UINT /*code*/, UINT /*id*/, HWND /*hwndControl*/, BOOL &bHandled)
	{
		bHandled = true;
		m_ecl->Copy();
		return 0;
	}
	LRESULT OnRepositoryCopy(UINT /*code*/, UINT /*id*/, HWND /*hwndControl*/, BOOL &bHandled)
	{
		T * pT = static_cast<T*>(this);
		bHandled = true;
		CString titleStr;
		pT->GetTitle(titleStr);
		titleStr.TrimLeft(_T("*"));
		SetClipboard((const TCHAR *)titleStr);
		return 0;
	}
	LRESULT OnEditPaste(UINT /*code*/, UINT /*id*/, HWND /*hwndControl*/, BOOL &bHandled)
	{
		bHandled = true;
		m_ecl->Paste();
		return 0;
	}
	LRESULT OnEditSelectAll(UINT /*code*/, UINT /*id*/, HWND /*hwndControl*/, BOOL &bHandled)
	{
		m_ecl->SelectAll();
		bHandled = true;
		return 0;
	}
	LRESULT OnEditDelete(UINT /*code*/, UINT /*id*/, HWND /*hwndControl*/, BOOL &bHandled)
	{
		bHandled = true;
		m_ecl->Clear();
		return 0;
	}
	LRESULT OnEditMatchBrace(UINT /*code*/, UINT /*id*/, HWND /*hwndControl*/, BOOL &bHandled)
	{
		bHandled = true;
		m_ecl->GoMatchingBrace(false);
		return 0;
	}
	LRESULT OnEditWordWrap(UINT /*code*/, UINT /*id*/, HWND /*hwndControl*/, BOOL &bHandled)
	{
		bHandled = true;
		m_ecl->SetWrapMode(m_ecl->GetWrapMode() == 0 ? 1 : 0);
		return 0;
	}
	LRESULT OnEditSelectToBrace(UINT /*code*/, UINT /*id*/, HWND /*hwndControl*/, BOOL &bHandled)
	{
		bHandled = true;
		m_ecl->GoMatchingBrace(true);
		return 0;
	}
	LRESULT OnEditCompleteWord(UINT /*code*/, UINT /*id*/, HWND /*hwndControl*/, BOOL &bHandled)
	{
		bHandled = true;
		m_ecl->StartAutoComplete();
		return 0;
	}
	LRESULT OnEditBlockComment(UINT /*code*/, UINT /*id*/, HWND /*hwndControl*/, BOOL &bHandled)
	{
		bHandled = true;
		m_ecl->StartBlockComment("//");
		return 0;
	}
	LRESULT OnEditBoxComment(UINT /*code*/, UINT /*id*/, HWND /*hwndControl*/, BOOL &bHandled)
	{
		bHandled = true;
		m_ecl->StartBoxComment("/*", "  ", "*/");
		return 0;
	}
	LRESULT OnEditUppercase(UINT /*code*/, UINT /*id*/, HWND /*hwndControl*/, BOOL &bHandled)
	{
		bHandled = true;
		m_ecl->UpperCase();
		return 0;
	}
	LRESULT OnEditLowercase(UINT /*code*/, UINT /*id*/, HWND /*hwndControl*/, BOOL &bHandled)
	{
		bHandled = true;
		m_ecl->LowerCase();
		return 0;
	}
	LRESULT OnEditToggleLineNumbers(UINT /*code*/, UINT /*id*/, HWND /*hwndControl*/, BOOL &bHandled)
	{
		bHandled = true;
		if (m_ecl->GetMarginWidthN(0) == 0)
			m_ecl->SetMarginWidthN(0, 32);
		else
			m_ecl->SetMarginWidthN(0, 0);
		return 0;
	}
	LRESULT OnEditSetTabSize(UINT /*code*/, UINT /*id*/, HWND /*hwndControl*/, BOOL &bHandled)
	{
		bHandled = true;
		int tabSize;
		if (IDOK == DoModalSetTabSize(m_ecl->GetTabWidth(), 16, tabSize))
		{
			m_ecl->SetTabWidth(tabSize);
			GetIConfig(QUERYBUILDER_CFG)->Set(GLOBAL_TAB_WIDTH, tabSize);
		}
		return 0;
	}
	LRESULT OnEditReformat(UINT /*code*/, UINT /*id*/, HWND /*hwndControl*/, BOOL &bHandled)
	{
		bHandled = true;
		m_ecl->Reformat();
		return 0;
	}
	LRESULT OnEditFind(UINT /*code*/, UINT /*id*/, HWND /*hwndControl*/, BOOL &bHandled)
	{
		bHandled = true;
		DoEditFind();
		return 0;
	}
	LRESULT OnEditReplace(UINT /*code*/, UINT /*id*/, HWND /*hwndControl*/, BOOL &bHandled)
	{
		bHandled = true;
		DoEditFindReplace();
		return 0;
	}
	LRESULT OnEditFindNext(UINT /*code*/, UINT /*id*/, HWND /*hwndControl*/, BOOL &bHandled)
	{
		bHandled = true;
		if(m_findReplace->GetFindReplace().m_szFindWhat[0] == 0)
		{
			DoEditFind();
			return 0;
		}

		//ATLASSERT(!IsEmpty());

		if (!m_ecl->DoFind(m_findReplace->GetFindReplace().m_szFindWhat, m_findReplace->GetFindReplace().m_dwFlags, m_findReplace->GetFindReplace().m_mode, false, true))
		{
			::MessageBeep(MB_ICONEXCLAMATION);
			m_ecl->DoFind(m_findReplace->GetFindReplace().m_szFindWhat, m_findReplace->GetFindReplace().m_dwFlags, m_findReplace->GetFindReplace().m_mode, true, true);
		}
		return 0;
	}

	LRESULT OnEditFindPrev(UINT /*code*/, UINT /*id*/, HWND /*hwndControl*/, BOOL &/*bHandled*/)
	{
		if(m_findReplace->GetFindReplace().m_szFindWhat[0] == 0)
		{
			DoEditFind();
			return 0;
		}

		//ATLASSERT(!IsEmpty());

		if (!m_ecl->DoFind(m_findReplace->GetFindReplace().m_szFindWhat, m_findReplace->GetFindReplace().m_dwFlags, m_findReplace->GetFindReplace().m_mode, false, false))
		{
			::MessageBeep(MB_ICONEXCLAMATION);
			m_ecl->DoFind(m_findReplace->GetFindReplace().m_szFindWhat, m_findReplace->GetFindReplace().m_dwFlags, m_findReplace->GetFindReplace().m_mode, true, false);
		}
		return 0;
	}
	LRESULT OnEditGoto(UINT /*code*/, UINT /*id*/, HWND /*hwndControl*/, BOOL &bHandled)
	{
		int line;
		if (IDOK == DoModalGoto(m_ecl->GetCurrentLineNumber() + 1, m_ecl->GetLineCount(), line))
			m_ecl->GotoLine(line);
		bHandled = true;
		return 0;
	}

	//These could be implemented the same as Syntax, but aren't

	//LRESULT OnEclCheckSyntax(UINT /*code*/, UINT /*id*/, HWND /*hwndControl*/, BOOL &bHandled)
	//{
	//	T * pT = static_cast<T*>(this);
	//	pT->DoCheckSyntax();
	//	bHandled = true;
	//	return 0;
	//}
	//LRESULT OnEclCheckComplexity(UINT /*code*/, UINT /*id*/, HWND /*hwndControl*/, BOOL &bHandled)
	//{
	//	T * pT = static_cast<T*>(this);
	//	pT->DoCheckComplexity();
	//	bHandled = true;
	//	return 0;
	//}

	//LRESULT OnEclCheckDependency(UINT /*code*/, UINT /*id*/, HWND /*hwndControl*/, BOOL &bHandled)
	//{
	//	T * pT = static_cast<T*>(this);
	//	pT->DoCheckDependency();
	//	bHandled = true;
	//	return 0;
	//}

	LRESULT	OnSubmitDone(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_ecl->SyntaxClearAll();
		T * pT = static_cast<T*>(this);
		Dali::WUAction action = static_cast<Dali::WUAction>(wParam);
		if (action != Dali::WUActionCheck)
		{
			bHandled = false;
			return 0;
		}
		bHandled = true;

		Dali::CEclExceptionVector * errors = reinterpret_cast<Dali::CEclExceptionVector *>(lParam);
		if (errors == NULL)
		{
			CString title;
			title.LoadString(IDR_MAINFRAME);
			pT->MessageBox(_T("Syntax Check Failed"), title, MB_ICONASTERISK);
			return 0;
		}

		//  ---
		CComPtr<IAttribute> attr = pT->GetAttribute();
		int warn = 0, err = 0;
		for (Dali::CEclExceptionVector::iterator itr = errors->begin(); itr != errors->end(); ++itr)
		{
			StlLinked<Dali::CEclException> e = itr->get();
			if (e->m_severity == Dali::CEclException::ECL_EXCEPTION_ERROR)
			{
				m_ecl->SyntaxAdd(SYNTAX_ERROR, e->m_code, e->m_fileName, e->m_fileType, e->m_message, e->m_lineNo - 1, e->m_column - 1);
				err++;
			}
		}
		for (Dali::CEclExceptionVector::iterator itr = errors->begin(); itr != errors->end(); ++itr)
		{
			StlLinked<Dali::CEclException> e = itr->get();
			if (e->m_severity == Dali::CEclException::ECL_EXCEPTION_WARNING && e->m_code != 1033)	//  Push sandboxed warnings to the end.
			{
				m_ecl->SyntaxAdd(SYNTAX_WARNING, e->m_code, e->m_fileName, e->m_fileType, e->m_message, e->m_lineNo - 1, e->m_column - 1);
				warn++;
			}
		}
		for (Dali::CEclExceptionVector::iterator itr = errors->begin(); itr != errors->end(); ++itr)
		{
			StlLinked<Dali::CEclException> e = itr->get();
			if (e->m_severity == Dali::CEclException::ECL_EXCEPTION_WARNING && e->m_code == 1033)	//  Push sandboxed warnings to the end.
			{
				m_ecl->SyntaxAdd(SYNTAX_WARNING, e->m_code, e->m_fileName, e->m_fileType, e->m_message, e->m_lineNo - 1, e->m_column - 1);
				warn++;
			}
		}
		for (Dali::CEclExceptionVector::iterator itr = errors->begin(); itr != errors->end(); ++itr)
		{
			StlLinked<Dali::CEclException> e = itr->get();
			if (e->m_severity != Dali::CEclException::ECL_EXCEPTION_ERROR && e->m_severity != Dali::CEclException::ECL_EXCEPTION_WARNING)
			{
				m_ecl->SyntaxAdd(SYNTAX_INFO, e->m_code, e->m_fileName, e->m_fileType, e->m_message, e->m_lineNo - 1, e->m_column - 1);
			}
		}

		if ( m_eclSlot )
		{
			_variant_t errCount(err), warnCount(warn);
			CString syntaxStatus = errCount;
			syntaxStatus += _T(" Errors, ");
			syntaxStatus += warnCount;
			syntaxStatus += _T(" Warnings");
			m_eclSlot->PostStatus(syntaxStatus);
			m_eclSlot->RefreshSyntax(m_ecl, 0, err || warn);
		}
		return 0;
	}

	LRESULT OnEclNextError(UINT /*code*/, UINT /*id*/, HWND /*hwndControl*/, BOOL &bHandled)
	{
		if (m_ecl->SyntaxSelNext())
			DoShowCurrentError();
		bHandled = true;
		return 0;
	}

	LRESULT OnEclPreviousError(UINT /*code*/, UINT /*id*/, HWND /*hwndControl*/, BOOL &bHandled)
	{
		if (m_ecl->SyntaxSelPrevious())
			DoShowCurrentError();
		bHandled = true;
		return 0;
	}

	LRESULT OnEclClearErrors(UINT /*code*/, UINT /*id*/, HWND /*hwndControl*/, BOOL &bHandled)
	{
		m_ecl->SyntaxClearAll();
		if ( m_eclSlot )
		{
			m_eclSlot->PostStatus(0); 
		}
		bHandled = true;
		return 0;
	}

	LRESULT OnMacroRecord(UINT /*code*/, UINT /*id*/, HWND /*hwndControl*/, BOOL &bHandled)
	{
		if (m_ecl->IsRecording())
			m_ecl->StopRecording();
		else
			m_ecl->StartRecording();
		bHandled = true;
		return 0;
	}

	LRESULT OnMacroPlay(UINT /*code*/, UINT /*id*/, HWND /*hwndControl*/, BOOL &bHandled)
	{
		m_ecl->PlaybackRecording();
		bHandled = true;
		return 0;
	}
};
