#include "stdafx.h"

#include "ShellContextMenu.h"

#define SHELL_ID_FIRST 1
#define SHELL_ID_LAST  0x6FFF

HRESULT GetUIObjectOfFile(HWND hwnd, const std::_tstring & filePath, REFIID riid, void **ppv)
{
    *ppv = NULL;
    HRESULT hr;
    LPITEMIDLIST pidl;
    SFGAOF sfgao;
    if (SUCCEEDED(hr = SHParseDisplayName(filePath.c_str(), NULL, &pidl, 0, &sfgao))) {
        IShellFolder *psf;
        LPCITEMIDLIST pidlChild;
        if (SUCCEEDED(hr = SHBindToParent(pidl, IID_IShellFolder, (void**)&psf, &pidlChild))) {
            hr = psf->GetUIObjectOf(hwnd, 1, &pidlChild, riid, NULL, ppv);
            psf->Release();
        }
        CoTaskMemFree(pidl);
    }
    return hr;
}

CShellContextMenu::CShellContextMenu(HWND hwnd)
{
    m_hwnd = hwnd;
    m_pcm = NULL;
}

void CShellContextMenu::InsertTortoiseGitMenuItems(const std::_tstring & filePath, UINT menuIndex, WTL::CMenuHandle & m)
{
    if (SUCCEEDED(GetUIObjectOfFile(m_hwnd, filePath, IID_IContextMenu, (void**)& m_pcm))) {
        int origItemCount = m.GetMenuItemCount();
        m_pcm->QueryContextMenu(m, menuIndex, SHELL_ID_FIRST, SHELL_ID_LAST, CMF_NORMAL);
        int insertedCount = m.GetMenuItemCount() - origItemCount;
        bool isGitFolder = false;
        for (int i = menuIndex; i < menuIndex + insertedCount; ++i) {
            CString str;
            m.GetMenuString(i, str, MF_BYPOSITION);
            if (!isGitFolder && str.Find(_T("Git Sync")) >= 0) 
                isGitFolder = true;
            else if (!(isGitFolder && (str.Find(_T("Git &Commit")) >= 0 || str.Find(_T("TortoiseGit")) >= 0))) 
            {
                m.RemoveMenu(i, MF_BYPOSITION);
                --i;
                --insertedCount;
            }
        }
        if (origItemCount != m.GetMenuItemCount()) {
            m.InsertMenu(menuIndex, MF_BYPOSITION | MF_SEPARATOR);
        }
    }
}

void CShellContextMenu::InvokeCommand(UINT iCmd, const CPoint & pt) const
{
    if (iCmd > 0 && m_pcm) {
        CMINVOKECOMMANDINFOEX info = { 0 };
        info.cbSize = sizeof(info);
        info.fMask = CMIC_MASK_UNICODE | CMIC_MASK_PTINVOKE;
        if (GetKeyState(VK_CONTROL) < 0) {
            info.fMask |= CMIC_MASK_CONTROL_DOWN;
        }
        if (GetKeyState(VK_SHIFT) < 0) {
            info.fMask |= CMIC_MASK_SHIFT_DOWN;
        }
        info.hwnd = m_hwnd;
        info.lpVerb = MAKEINTRESOURCEA(iCmd - SHELL_ID_FIRST);
        info.lpVerbW = MAKEINTRESOURCEW(iCmd - SHELL_ID_FIRST);
        info.nShow = SW_SHOWNORMAL;
        info.ptInvoke = pt;
        m_pcm->InvokeCommand((LPCMINVOKECOMMANDINFO)&info);
    }
}
