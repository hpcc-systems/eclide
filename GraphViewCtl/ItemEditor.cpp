#include "stdafx.h"
#include "resource.h"
#include "Canvas.h"
#include "ItemEditor.h"

using namespace GraphTypes;

//-----------------------------------------------------------------------------

CItemEdit::CItemEdit(IGraphItem* item, CCanvas* canvas)
    : m_gi(item), m_canvas(canvas) 
{
    CComQIPtr<IGraphVertex> v=m_gi;
    if(v!=0)
    {
        m_parent=v->GetParent();
        m_index=v->GetOrder();
    }
}

//-----------------------------------------------------------------------------

bool CItemEdit::EditNext(unsigned newindex)
{
    bool bSuccess=IsWindow()!=0;

    if(bSuccess)
    {
        CComQIPtr<IRenderer> ir;

        m_gi=m_parent->GetChild(newindex);
        ATLASSERT(m_gi);

        bSuccess=m_gi!=0;
        if(bSuccess)
        {
            m_index=newindex;

#ifdef _DEBUG
            CComQIPtr<IGraphVertex> v=m_gi;
            if(v!=0)
            {
                ATLASSERT(m_index==v->GetOrder());
            }
#endif

            ir=m_canvas->GetRenderer(m_gi);
            ATLASSERT(ir);
            bSuccess=ir!=0;
        }
        
        if(bSuccess)
        {
            RectF rc;
            ir->GetBoundsBox(rc);
            CRect irc=static_cast<CRect>(rc);
            MoveWindow(&irc);
            UpdateText();
        }
    }

    return bSuccess;
}

//-----------------------------------------------------------------------------

void CItemEdit::RestoreState()
{
    IRenderer* ir;

    if(m_parent!=0)
    {
        m_gi=m_parent->GetChild(m_index);
        ATLASSERT(m_gi);
        if(m_gi)
        {
            ir=m_canvas->GetRenderer(m_gi);
            if(ir!=0)
                ir->SetSelected();

            ir=m_canvas->GetRenderer(m_parent);
            if(ir!=0)
            {
                ir->SetFocus();
                ir->SetSelected();
            }
        }
    }
}

//-----------------------------------------------------------------------------

bool CItemEdit::ChangeLabel(bool bAsk)
{
    CString str;
    GetWindowText(str);
    bool bChange=false, bMBox=false;

    if(str.GetLength() > 0 && m_curText != str)
    {
        if(bAsk)
        {
            CString title;
            title.LoadString(IDS_PROJNAME);
            bChange=MessageBox(_T("Change label"),
                title, MB_YESNO | MB_DEFBUTTON1 | MB_ICONQUESTION)==IDYES;
            bMBox=true;
        }
        else
        {
            bChange=true;
        }

        if(bChange)
        {
            std::_tstring label((const TCHAR*)str); 
            if(m_canvas->NotifyLabelEdited(
                    m_parent->GetID(),
                    m_gi->GetID(),
                    label)!=-1) // controller code allows change
                m_curText=str;
            // item will be be recreated after label change
            m_gi=0;
        }
    }

    return bMBox;
}

//-----------------------------------------------------------------------------

void CItemEdit::UpdateText()
{
    m_curText=m_gi->GetLabel();
    SetWindowText(m_curText);
    SetSel(0, -1);
}

//-----------------------------------------------------------------------------

LRESULT CItemEdit::MouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRes=0;
    CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam));
    CRect rc;

    GetClientRect(&rc);
    if(rc.PtInRect(pt))
    {
        DefWindowProc(uMsg, wParam, lParam);
        lRes=1; // indicate processed by us
    }
    else
    {
        if(uMsg==WM_LBUTTONDOWN||uMsg==WM_RBUTTONDOWN||uMsg==WM_MBUTTONDOWN)
        {
            if(ChangeLabel(true))
                lRes=1;
            m_canvas->DestroyEditor();
        }
    }

    return lRes;
}

//-----------------------------------------------------------------------------

void CItemEdit::OnFinalMessage(HWND /*hWnd*/)
{
    delete this;
}

//-----------------------------------------------------------------------------

LRESULT CItemEdit::OnCreate(
    UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
    LRESULT lRes = DefWindowProc(uMsg, wParam, lParam);
    if(lRes!=0)
    {
        SetFont(m_canvas->GetListFont(m_canvas->GetFontPixHeight()));
        UpdateText();
    }
    return lRes;
}

//-----------------------------------------------------------------------------

LRESULT CItemEdit::OnKeyDown(
    UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
    bHandled = FALSE;

    switch(wParam) 
    {
    case VK_ESCAPE:
        {
            RestoreState();
            m_canvas->DestroyEditor();
        }
        break;
    case VK_RETURN:
        {
            ChangeLabel(false);
            RestoreState();
            m_canvas->DestroyEditor();
        }
        break;
    case VK_TAB:
    case VK_UP:
    case VK_DOWN:
        {
            if(m_parent!=0)
            {
                unsigned newidx=m_index;
                bool bUp=wParam==VK_UP || (wParam==VK_TAB
                    && (GetAsyncKeyState(VK_SHIFT)&0x8000)!=0);

                if(bUp && m_index>0)
                {
                    newidx--;
                }
                else if(!bUp)
                {
                    newidx++;
                }
                
                bool gotonext=newidx!=m_index;

                IGraphItem* next = NULL;

                if(gotonext)
                {
                    ChangeLabel(false); 
                    m_canvas->Invalidate();
                    next=m_parent->GetChild(newidx);
                    gotonext=next!=0;
                }

                CComQIPtr<IListWindowRenderer> ilw;

                if(gotonext)
                {
                    ilw=m_canvas->GetRenderer(m_parent);
                    ATLASSERT(ilw);
                    gotonext=ilw!=0;
                }

                if(gotonext)
                {
                    // prevent editing item covered by scrollbars and outside viewport

                    CComPtr<IRenderer> ir=m_canvas->GetRenderer(next);
                    ATLASSERT(ir);
                    RectF rc;
                    ir->GetBoundsBox(rc);

                    RectF vp;
                    m_canvas->GetViewport(vp);
                    gotonext=rc.x>=vp.x &&
                        rc.y>=vp.y &&
                        (rc.x+rc.Width)<=(vp.x+vp.Width) &&
                        (rc.y+rc.Height)<=(vp.y+vp.Height);

                    // prevent editing item covered outside its parent

                    if(gotonext)
                    {
                        RectF rcparent;
                        ilw->GetBoundsBox(rcparent);
                        gotonext=rc.x>=rcparent.x &&
                            rc.y>=rcparent.y &&
                            (rc.x+rc.Width)<=(rcparent.x+rcparent.Width) &&
                            (rc.y+rc.Height)<=(rcparent.y+rcparent.Height);
                    }
                }

                if(gotonext)
                {
                    if(ilw->OnEditItem(next))
                    {
                        CWindow pwnd=GetParent();
                        pwnd.UpdateWindow(); // redraw to give it resize chance
                        EditNext(newidx);
                    }
                    else
                        m_gi=m_parent->GetChild(m_index);
                    bHandled = TRUE;
                }
                else
                {
                    m_gi=m_parent->GetChild(m_index);
                }
            }
        }
        break;
    }
    return 0;
}

//-----------------------------------------------------------------------------

LRESULT CItemEdit::OnChar(
    UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
    switch( LOWORD(wParam) )
    {
    case VK_ESCAPE:
    case VK_RETURN:
        // Do not BEEP!!!!
        return 0;
    }
    bHandled = FALSE;
    return 0;
}

//-----------------------------------------------------------------------------

LRESULT CItemEdit::OnGetDlgCode(
    UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
    return DefWindowProc(uMsg, wParam, lParam) | DLGC_WANTALLKEYS | DLGC_WANTARROWS;
}
