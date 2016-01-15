#include "StdAfx.h"
#include "resource.h"
#include "GraphViewCanvas.h"
#include "QueryBuilderUI.h"

class CEditEnterHandler : public CWindowImpl<CEditEnterHandler, CEdit>
{
    BEGIN_MSG_MAP(CMyEdit)
        MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
    END_MSG_MAP()

    LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
    {
        if(wParam==VK_RETURN)
        {
            // pretend Find key was pressed
            GetParent().PostMessage(WM_COMMAND, MAKEWPARAM(IDC_FIND_BUTTON,BN_CLICKED), 0);
            bHandled=FALSE;
        }
        return 0;
    }

};

//*****************************************************************************

class CFindDialog : public CDialogImpl<CFindDialog>
{
    CGraphViewCanvas* m_canvasView;
    CUniqueIDVector m_findResults;
    std::_tstring m_key;
    std::_tstring m_valuetofind;
    unsigned m_loc;

    CEditEnterHandler m_edit;
public:
    static CFindDialog* m_dlg;

    CFindDialog::CFindDialog(CGraphViewCanvas* canvas)
        : m_canvasView(canvas),
        m_key(_T("id")),
        m_loc(0)
    {
    }

    BEGIN_MSG_MAP(CFindDialog)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        COMMAND_ID_HANDLER(IDOK, OnFind)
        COMMAND_ID_HANDLER(IDC_FIND_BUTTON, OnFind)
        COMMAND_HANDLER(IDC_FIND_EDIT, EN_CHANGE, OnEditChange)	
        COMMAND_ID_HANDLER(IDCANCEL, OnClose)
    END_MSG_MAP()

    enum { IDD = IDD_QBFIND };

    void CreateEx() 
    {
        CDialogImpl<CFindDialog>::Create(m_canvasView->m_hWnd);
        ATLASSERT(IsWindow());
    }

private:


    LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {	
        m_edit.SubclassWindow(GetDlgItem(IDC_FIND_EDIT));
        CWindow findbutt=GetDlgItem(IDC_FIND_BUTTON);
        ATLASSERT(findbutt.IsWindow());
        findbutt.EnableWindow(FALSE);
        return 0;
    }

    LRESULT OnFind(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        CWindow edit=GetDlgItem(IDC_FIND_EDIT);
        ATLASSERT(edit.IsWindow());
        TCHAR buff[64];
        edit.GetWindowText(buff,sizeof buff);

        if(m_valuetofind.compare(buff)==0)
        {
            m_loc++;

            if(m_loc>=m_findResults.size())
                m_loc=0;
        }
        else
        {
            m_valuetofind=buff;
            CComVariant tooltip;
            std::_tstring toolText, value;

            IGraph* graph=m_canvasView->GetGraph();
            m_findResults.clear();
            m_loc=0;

            for(CComPtr<IVertexIterator> itr = graph->GetVertexIterator(); itr->IsValidItem(); itr->NextItem())
            {
                IGraphVertex* vertex = itr->Get();
                ATLASSERT(vertex);
                CComQIPtr<CStringStringMap> attrs = vertex->GetPropertyUnknown(PROP_ATTRIBUTES);
                if (attrs)
                {
                    for(Element::StringStringMap::iterator itr = attrs->m_attr.begin(); itr != attrs->m_attr.end(); ++itr)
                    {
                        if (boost::algorithm::icontains(itr->second, m_valuetofind))
                        {
                            m_findResults.Add(vertex->GetID());
                            break;
                        }
                    }
                }
            }

            for(CComPtr<IEdgeIterator> itr = graph->GetEdgeIterator(); itr->IsValidItem(); itr->NextItem())
            {
                IGraphEdge* edge=itr->Get();	
                ATLASSERT(edge);
                CComQIPtr<CStringStringMap> attrs = edge->GetPropertyUnknown(PROP_ATTRIBUTES);
                if (attrs)
                {
                    for(Element::StringStringMap::iterator itr = attrs->m_attr.begin(); itr != attrs->m_attr.end(); ++itr)
                    {
                        if (boost::algorithm::icontains(itr->second, m_valuetofind))
                        {
                            m_findResults.Add(edge->GetID());
                            break;
                        }
                    }
                }
            }
        }

        unsigned cntres=m_findResults.size();
        if(cntres>0)
        {
            CUniqueID id = m_findResults[m_loc];
            m_canvasView->SetSelected(id, true, false);
            IGraphEdge* iedge=m_canvasView->GetEdge(id);
            if(iedge)
            {
                m_canvasView->CenterEdge(iedge, true);
            }
            else
            {
                m_canvasView->CenterVertex(id, 1.0f);
            }
        }

        CWindow findbutt=GetDlgItem(IDC_FIND_BUTTON);
        ATLASSERT(findbutt.IsWindow());
        if(cntres>1)
            findbutt.SetWindowText(_T("Find Next"));
        else
            findbutt.SetWindowText(_T("Find"));
        return 0;
    }

    LRESULT OnEditChange(WORD /*wNotifyCode*/, WORD wID, HWND hWndCtl, BOOL& /*bHandled*/)
    {
        CWindow edit(hWndCtl);
        ATLASSERT(edit.IsWindow());
        CWindow findbutt=GetDlgItem(IDC_FIND_BUTTON);
        ATLASSERT(findbutt.IsWindow());

        bool anytext=edit.GetWindowTextLength()>0;
        if(anytext)
        {
            TCHAR buff[64];
            edit.GetWindowText(buff,sizeof buff);
            if(m_valuetofind.compare(buff)!=0)
                findbutt.SetWindowText(_T("Find"));
        }
        findbutt.EnableWindow(anytext);
        return 0;
    }
    
    LRESULT OnClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
    {
        DestroyWindow();				
        bHandled = FALSE;
        return 0;
    }

    virtual void OnFinalMessage(HWND /*hWnd*/)
    {
        m_dlg=0;
        ::SetFocus(m_canvasView->m_hWnd);
        delete this;
    }
};

CFindDialog* CFindDialog::m_dlg;

//*****************************************************************************

void QBUI::LaunchFindDialog(CGraphViewCanvas* canvas)
{
    if(!CFindDialog::m_dlg)
        CFindDialog::m_dlg=new CFindDialog(canvas);
    if(CFindDialog::m_dlg)
    {
        if(!CFindDialog::m_dlg->IsWindow())
        {
            CFindDialog::m_dlg->CreateEx();
            CFindDialog::m_dlg->ShowWindow(SW_SHOW);
        }
    }
}
