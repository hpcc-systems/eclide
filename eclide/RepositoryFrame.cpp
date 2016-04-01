#include "stdafx.h"
#include "..\en_us\resource.h"

#include "RepositoryFrame.h"

//  ===========================================================================
CRepositoryFrame::CRepositoryFrame()
{
    m_view.Init(this);
    m_view.SetRootLabel(_T("Repository"));
}

BEGIN_MESSAGE_MAP(CRepositoryFrame, baseClass)
    ON_MESSAGE(CWM_SYNCTOC, OnSyncToc)
    ON_WM_INITMENUPOPUP()
END_MESSAGE_MAP()

LRESULT CRepositoryFrame::OnSyncToc(WPARAM wParam, LPARAM lParam)
{
    CComPtr<IAttribute> attr(reinterpret_cast<IAttribute *>(wParam));
    m_view.Select(attr);
    return 0;
}

void CRepositoryFrame::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
    //ATLTRACE(_T("OnInitMenuPopup(pos=%d,isMenu=%u)\n"), nIndex, ::IsMenu(hMenu));
    if (nIndex > 0 && pPopupMenu != NULL)	//Invoke is the only sub menu
    {
        CWaitCursor wait;
        CComPtr<IAttribute> attr = m_view.GetFirstSelectedAttr();
        if (attr)
        {
            WTL::CMenu m = pPopupMenu->GetSafeHmenu();

            bool foundInvoke = false;
            if (static_cast<bool>(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_DISABLEINVOKE)) == false)
            {
                StlLinked<IEclProgram> program = ParseEcl(attr->GetText());
                if (program.get())
                {
                    for(unsigned i = 0; i < program->GetElementCount(); ++i)
                    {
                        CComQIPtr<IEclInvokeComment>invoke = program->GetElement(i);
                        if (invoke)
                        {
                            WTL::CMenuItemInfo mi;
                            mi.fMask = MIIM_ID | MIIM_TYPE;
                            mi.fType = MFT_STRING;
                            mi.dwTypeData = const_cast<LPTSTR>(invoke->GetLabel());
                            mi.wID = ID_REPOSITORY_INVOKE + 100 + i;
                            m.InsertMenuItem(ID_REPOSITORY_INVOKE, FALSE, &mi);
                            foundInvoke = true;
                        }
                    }
                }
            }
            if (foundInvoke)
            {
                m.RemoveMenu(ID_REPOSITORY_INVOKE_NONE, MF_BYCOMMAND);
            }
        }
    }
}
//  ===========================================================================
CRepositoryFilterFrame::CRepositoryFilterFrame()
{
    m_view.Init(this);
}

BEGIN_MESSAGE_MAP(CRepositoryFilterFrame, baseClass)
END_MESSAGE_MAP()

//  ===========================================================================
