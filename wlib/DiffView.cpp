#include "StdAfx.h"
#include "..\en_us\resource.h"

#include "DiffView.h"
#include "PreferenceDlg.h"
#include "TextCompare.h"

using namespace CLIB;

CDiffView::CDiffView() : m_lhs(this), m_rhs(this)
{
}

void CDiffView::SetText(const std::_tstring &lhs, const std::_tstring &rhs, IAttributeType *lhsType, IAttributeType *rhsType)
{
    CLineVector baseResult;
    CLineVector compResult;
    TextCompare(lhs, rhs, baseResult, compResult);

    m_lhs.SetReadOnly(false);
    m_lhs.ClearAll();
    m_lhs.SetReadOnly(true);
    m_lhs.SetProperty(_T("fold"), _T("0"));
    m_lhs.SetMarginWidthN(2, 0);
    //m_lhs.HideSelection(true);
    m_lhs.IndicSetStyle(1, INDIC_SQUIGGLE);
    m_lhs.IndicSetFore(1, 0x0000ff);
    m_lhs.SetLineState(0, 0);
    m_lhs.MarkerSetFore(MARKER_ADDED, 0x007f00);
    m_lhs.MarkerSetFore(MARKER_MODIFIED, 0x7f0000);
    m_lhs.MarkerSetFore(MARKER_DELETED, 0x00007f);
    m_lhs.MarkerSetAlpha(MARKER_ADDED, 200);
    m_lhs.MarkerSetAlpha(MARKER_MODIFIED, 200);
    m_lhs.MarkerSetAlpha(MARKER_DELETED, 200);

    m_rhs.SetReadOnly(false);
    m_rhs.ClearAll();
    m_rhs.SetReadOnly(true);
    m_rhs.SetProperty(_T("fold"), _T("0"));
    m_rhs.SetMarginWidthN(2, 0);
    //m_rhs.HideSelection(true);
    m_rhs.IndicSetStyle(1, INDIC_SQUIGGLE);
    m_rhs.IndicSetFore(1, 0x0000ff);
    m_rhs.SetLineState(0, 0);
    m_rhs.MarkerSetFore(MARKER_ADDED, 0x007f00);
    m_rhs.MarkerSetFore(MARKER_MODIFIED, 0x7f0000);
    m_rhs.MarkerSetFore(MARKER_DELETED, 0x00007f);
    m_rhs.MarkerSetAlpha(MARKER_ADDED, 200);
    m_rhs.MarkerSetAlpha(MARKER_MODIFIED, 200);
    m_rhs.MarkerSetAlpha(MARKER_DELETED, 200);

    std::_tstring clhs;
    for(CLineVector::iterator itr = baseResult.begin(); itr != baseResult.end(); ++itr)
    {
        //clhs += itr->ststus;
        clhs += itr->GetText();
        //clhs += _T("\n");
    }

    std::_tstring crhs;
    for(CLineVector::iterator itr = compResult.begin(); itr != compResult.end(); ++itr)
    {
        //crhs += itr->ststus;
        crhs += itr->GetText();
        //crhs += _T("\n");
    }

    m_lhs.SetReadOnly(false);
    m_lhs.InitLanguage(lhsType);
    m_lhs.SetText(clhs.c_str());
    int line = 0;
    ATLASSERT(baseResult.size() == compResult.size());
    for(CLineVector::iterator itr = baseResult.begin(); itr != baseResult.end(); ++itr)
    {
        int startPos = m_lhs.PositionFromLine(line);
        //int lineLen = m_lhs.PositionFromLine(line + 1) - startPos;
        switch(itr->GetType())
        {
        case CLine::TYPE_ADDED:
            m_lhs.SetLineState(line, SCE_ECL_ADDED);
            break;
        case CLine::TYPE_UNKNOWN:
            break;
        case CLine::TYPE_DELETED:
            //m_lhs.SetLineState(line, SCE_ECL_DELETED);
            m_lhs.MarkerAdd(line, MARKER_DELETED);
            break;
        case CLine::TYPE_MOVED:
            //m_lhs.SetLineState(line, SCE_ECL_MOVED);
            //m_rhs.SetLineState(line, 0);
            break;
        case CLine::TYPE_SIMILAR:
            //m_lhs.SetLineState(line, SCE_ECL_CHANGED);
            m_lhs.MarkerAdd(line, MARKER_MODIFIED);
            for(int i = 0; i < itr->GetLength(); ++i)
            {
                if (itr->GetCharType(i) == CLine::TYPE_SIMILAR)
                {
                    m_lhs.SetIndicatorCurrent(INDIC_SQUIGGLE);
                    m_lhs.IndicatorFillRange(startPos + i, 1);
                }
            }
            break;
        }
        ++line;
    }
    m_lhs.SetReadOnly(true);
    m_rhs.SetReadOnly(false);
    m_rhs.InitLanguage(rhsType);
    m_rhs.SetText(crhs.c_str());
    line = 0;
    for(CLineVector::iterator itr = compResult.begin(); itr != compResult.end(); ++itr)
    {
        int startPos = m_rhs.PositionFromLine(line);
        switch(itr->GetType())
        {
        case CLine::TYPE_ADDED:
            //m_rhs.SetLineState(line, SCE_ECL_ADDED);
            m_rhs.MarkerAdd(line, MARKER_ADDED);
            break;
        case CLine::TYPE_UNKNOWN:
            break;
        case CLine::TYPE_DELETED:
            m_rhs.SetLineState(line, SCE_ECL_DELETED);
            break;
        case CLine::TYPE_MOVED:
            //m_rhs.SetLineState(line, SCE_ECL_MOVED);
            //m_lhs.SetLineState(line, 0);
            break;
        case CLine::TYPE_SIMILAR:
            //m_rhs.SetLineState(line, SCE_ECL_CHANGED);
            m_rhs.MarkerAdd(line, MARKER_MODIFIED);
            for(int i = 0; i < itr->GetLength(); ++i)
            {
                if (itr->GetCharType(i) == CLine::TYPE_SIMILAR)
                {
                    m_rhs.SetIndicatorCurrent(1);
                    m_rhs.IndicatorFillRange(startPos + i, 1);
                }
            }
            break;
        }
        ++line;
    }
    m_rhs.SetReadOnly(true);
}

void CDiffView::SetViewEOL(bool showCRLF)
{
    m_lhs.SetViewEOL(showCRLF);
    m_rhs.SetViewEOL(showCRLF);
}

void CDiffView::SetViewWS(bool showWhitespace)
{
    m_lhs.SetViewWS(showWhitespace);
    m_rhs.SetViewWS(showWhitespace);
}

const int MARKER_ALL_MASK = 
        (1 << MARKER_ERROR) + 
        (1 << MARKER_WARNING) +
        (1 << MARKER_ARROW) +
        (1 << MARKER_ADDED) +
        (1 << MARKER_DELETED) +
        (1 << MARKER_MODIFIED) +
        (1 << MARKER_CIRCLEMINUS) +
        (1 << MARKER_CIRCLE) +
        (1 << MARKER_CIRCLEPLUS);

int GetDiffPrevious(CSourceCtrl & m_lhs, CSourceCtrl & m_rhs, int curLine)
{
    int prevLineLHS = m_lhs.MarkerPrevious(curLine, MARKER_ALL_MASK);
    int prevLineRHS = m_rhs.MarkerPrevious(curLine, MARKER_ALL_MASK);
    int prevLine = -1;
    if (prevLineLHS >= 0 || prevLineRHS >= 0)
        prevLine = prevLineLHS > prevLineRHS ? prevLineLHS : prevLineRHS;
    return prevLine;
}

void CDiffView::GotoDiffPrevious()
{
    int prevLine = GetDiffPrevious();
    int diffPrevious = ::GetDiffPrevious(m_lhs, m_rhs, prevLine - 1);
    while(diffPrevious == prevLine - 1)
    {
        prevLine = diffPrevious;	//Same as --curLine;
        diffPrevious = ::GetDiffPrevious(m_lhs, m_rhs, prevLine - 1);
    }
    if (prevLine >= 0)
    {
        m_lhs.SetFirstVisibleLine(prevLine);
        int lhsPos = m_lhs.PositionFromLine(prevLine);
        m_lhs.SetCurrentPos(lhsPos);
        m_lhs.SetSel(lhsPos, lhsPos + m_lhs.LineLength(prevLine));
        int rhsPos = m_rhs.PositionFromLine(prevLine);
        m_rhs.SetSel(rhsPos, rhsPos + m_rhs.LineLength(prevLine));
    }
}

int CDiffView::GetDiffPrevious()
{
    int curLine = m_lhs.GetCurrentLine();
    int diffPrevious = ::GetDiffPrevious(m_lhs, m_rhs, curLine - 1);
    while(diffPrevious == curLine - 1)
    {
        curLine = diffPrevious;	//Same as --curLine;
        diffPrevious = ::GetDiffPrevious(m_lhs, m_rhs, curLine - 1);
    }
    return diffPrevious;
    //int curLine = m_lhs.GetCurrentLine() - 1;
    //int prevLineLHS = m_lhs.MarkerPrevious(curLine, MARKER_ALL_MASK);
    //int prevLineRHS = m_rhs.MarkerPrevious(curLine, MARKER_ALL_MASK);
    //int prevLine = -1;
    //if (prevLineLHS >= 0 || prevLineRHS >= 0)
    //	prevLine = prevLineLHS > prevLineRHS ? prevLineLHS : prevLineRHS;
    //return prevLine;
} 

int GetDiffNext(CSourceCtrl & m_lhs, CSourceCtrl & m_rhs, int curLine)
{
    int nextLineLHS = m_lhs.MarkerNext(curLine, MARKER_ALL_MASK);
    int nextLineRHS = m_rhs.MarkerNext(curLine, MARKER_ALL_MASK);
    int nextLine = -1;
    if (nextLineLHS >= 0 || nextLineRHS >= 0)
    {
        if (nextLineLHS >= 0 && nextLineRHS >= 0)
            nextLine = nextLineLHS < nextLineRHS ? nextLineLHS : nextLineRHS;
        else 
            nextLine = nextLineLHS > nextLineRHS ? nextLineLHS : nextLineRHS;
    }
    return nextLine;
}

void CDiffView::GotoDiffNext()
{
    int nextLine = GetDiffNext();
    if (nextLine >= 0)
    {
        m_lhs.SetFirstVisibleLine(nextLine);
        int lhsPos = m_lhs.PositionFromLine(nextLine);
        m_lhs.SetCurrentPos(lhsPos);
        m_lhs.SetSel(lhsPos, lhsPos + m_lhs.LineLength(nextLine));
        int rhsPos = m_rhs.PositionFromLine(nextLine);
        m_rhs.SetSel(rhsPos, rhsPos + m_rhs.LineLength(nextLine));

    }
}

int CDiffView::GetDiffNext()
{
    int curLine = m_lhs.GetCurrentLine();
    int diffNext = ::GetDiffNext(m_lhs, m_rhs, curLine + 1);
    while(diffNext == curLine + 1)
    {
        curLine = diffNext;	//Same as --curLine;
        diffNext = ::GetDiffNext(m_lhs, m_rhs, curLine + 1);
    }
    return diffNext;

    //int nextLineLHS = m_lhs.MarkerNext(curLine, MARKER_ALL_MASK);
    //int nextLineRHS = m_rhs.MarkerNext(curLine, MARKER_ALL_MASK);
    //int nextLine = -1;
    //if (nextLineLHS >= 0 || nextLineRHS >= 0)
    //{
    //	if (nextLineLHS >= 0 && nextLineRHS >= 0)
    //		nextLine = nextLineLHS < nextLineRHS ? nextLineLHS : nextLineRHS;
    //	else 
    //		nextLine = nextLineLHS > nextLineRHS ? nextLineLHS : nextLineRHS;
    //}
    //return nextLine;
}

BOOL CDiffView::PreTranslateMessage(MSG* pMsg)
{
    pMsg;
    return FALSE;
}

int CDiffView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    SetMsgHandled(false);

    ModifyStyleEx(WS_EX_CLIENTEDGE, 0);

    m_splitter.Create(m_hWnd, ATL::CWindow::rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

    m_lhs.Create(m_splitter, ATL::CWindow::rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);
    m_lhs.DoInit();
    m_lhs.SetReadOnly(true);

    m_rhs.Create(m_splitter, ATL::CWindow::rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);
    m_rhs.DoInit();
    m_rhs.SetReadOnly(true);

    m_splitter.SetSplitterPanes(m_lhs, m_rhs);
    m_splitter.SetSplitterPosPct(50);

    m_lhs.SetOther(&m_rhs);
    m_rhs.SetOther(&m_lhs);
    return 0;
}

void CDiffView::OnSize(UINT nType, CSize size)
{
    SetMsgHandled(false);
    if( nType != SIZE_MINIMIZED )
    {
        UINT flags = SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSENDCHANGING;
        CRect rcClient;
        GetClientRect(rcClient);
        m_splitter.SetWindowPos(NULL, rcClient, flags);
    }
}
