#pragma once

#include "SourceView.h"
#include "EclCommand.h"
#include "SplitterWnd.h"

class CDiffView : public CWindowImpl<CDiffView>, public CEclCommandMixin<CDiffView>, public CSourceSlotImpl
{
protected:
    CMySplitterWindow<true> m_splitter;
    CSourceCtrl m_lhs;
    CSourceCtrl m_rhs;
    
public:
    CDiffView();

    DECLARE_WND_CLASS(NULL)

    void SetText(const std::_tstring &lhs, const std::_tstring &rhs);
    void SetViewEOL(bool showCRLF);
    void SetViewWS(bool showWhitespace);

    int GetDiffPrevious();
    int GetDiffNext();
    void GotoDiffPrevious();
    void GotoDiffNext();

    BOOL PreTranslateMessage(MSG* pMsg);

    BEGIN_MSG_MAP(CDiffView)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_SIZE(OnSize)
        REFLECT_NOTIFICATIONS()
    END_MSG_MAP()

    int OnCreate(LPCREATESTRUCT lpCreateStruct);
    void OnSize(UINT nType, CSize size);

    //  Virutals
    void GetTitle(CString & title) {title = _T("CDiffVIew");}
    IAttribute* GetAttribute() { return 0; } //for CEclCommandMixin (OnSubmitDone)
    //FindReplace	m_fr;

    ////  IFindReplace
    //FindReplace& GetFindReplace();
    //void FindReplaceOnEditFind(const std::_tstring &findWhat);
    //void FindReplaceOnEditReplace(const std::_tstring &replaceWhat);
};
