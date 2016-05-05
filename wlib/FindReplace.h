/*
 Copyright (c) 2000 
 Author: Konstantin Boukreev 
 E-mail: konstantin@mail.primorye.ru 

 Created: 07.07.2000 11:58:15
 Version: 1.0.0

*/

#ifndef _FindReplace_40f6ea36_72a8_489f_8ba8_cdd73efd19e1
#define _FindReplace_40f6ea36_72a8_489f_8ba8_cdd73efd19e1

#if _MSC_VER > 1000 
#pragma once
#endif // _MSC_VER > 1000

enum FINDMODE
{
    FINDMODE_UNKNOWN = 0,
    FINDMODE_NONE,
    FINDMODE_WILDCARD,
    FINDMODE_REGEX,
    FINDMODE_LAST
};

class FindReplace
{
public:
    CSimpleArray<TCHAR*> m_FindHistory;
    CSimpleArray<TCHAR*> m_ReplaceHistory;

    HICON m_hPinned;
    HICON m_hUnpinned;
        
    DWORD m_dwFlags;
    TCHAR m_szFindWhat[256];
    TCHAR m_szReplaceWith[256];
    HWND  m_hWndOwner;
    UINT  m_findReplaceMsg;
    UINT  m_findReplaceMsgParam;

    FINDMODE m_mode;

    bool m_enableWildcardRegex;
    bool m_findWrap;

    bool m_bPinned;

    FindReplace(bool bPinned = true) 
    {			
        m_szFindWhat[0] = '\0';
        m_szReplaceWith[0] = '\0';		
        m_hWndOwner = 0;
        m_findReplaceMsg = RegisteredFindReplaceMsg(); //WM_COMMAND
        m_findReplaceMsgParam = 0; //IDD_FINDREPLACE
        m_bPinned = bPinned;
        m_dwFlags = FR_DOWN;
        m_mode = FINDMODE_NONE;
        m_enableWildcardRegex = false;
        m_findWrap = false;

        m_hPinned = LoadIcon(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(IDI_PINNED));			
        m_hUnpinned = LoadIcon(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(IDI_UNPINNED));
    }

    ~FindReplace() 
    {
        DestroyIcon(m_hPinned);
        DestroyIcon(m_hUnpinned);	

        int i;
        for (i = 0; i < m_FindHistory.GetSize(); i++)
            delete [] m_FindHistory[i];
            
        for (i = 0; i < m_ReplaceHistory.GetSize(); i++)
            delete [] m_ReplaceHistory[i];
    }
    
    LPCTSTR GetFindString() const {	return (LPCTSTR) m_szFindWhat; }
    LPCTSTR GetReplaceString() const { return (LPCTSTR)m_szReplaceWith; }
    
    BOOL SearchDown() const { return m_dwFlags & FR_DOWN ? TRUE : FALSE; }
    BOOL FindNext() const {	return m_dwFlags & FR_FINDNEXT ? TRUE : FALSE; }
    BOOL MatchCase() const { return m_dwFlags & FR_MATCHCASE ? TRUE : FALSE; }
    BOOL MatchWholeWord() const { return m_dwFlags & FR_WHOLEWORD ? TRUE : FALSE; }
    BOOL Wildcard() const { return m_mode == FINDMODE_WILDCARD; }
    BOOL Regex() const { return m_mode == FINDMODE_REGEX; }
    BOOL ReplaceCurrent() const { return m_dwFlags & FR_REPLACE ? TRUE : FALSE; }
    BOOL ReplaceAll() const	{ return m_dwFlags & FR_REPLACEALL ? TRUE : FALSE;	}
    BOOL IsTerminating() const { return m_dwFlags & FR_DIALOGTERM ? TRUE : FALSE; }
    BOOL IsFindWrap() const { return m_findWrap; }

    static DWORD RegisteredFindReplaceMsg() 
    {
        static UINT nMsgFindReplace = 0;
        if ( !nMsgFindReplace )
            nMsgFindReplace = ::RegisterWindowMessage(FINDMSGSTRING);
        return nMsgFindReplace;
    }
};

template <class T>
class FindReplaceDlgT 
    : public WTL::CDialogImpl<T>	
{
protected:
    FindReplace* m_fr;

public:
    FindReplaceDlgT() 		
    {
        m_fr = 0;
    }

    BEGIN_MSG_MAP(FindReplaceDlgT)
        COMMAND_ID_HANDLER(IDCANCEL, OnClose)
        MESSAGE_HANDLER(WM_ACTIVATE, OnActivate)
        MESSAGE_HANDLER(WM_SYSCOMMAND, OnSysCommand)		
    END_MSG_MAP()

    HWND CreateEx(FindReplace* fr) 
    {
        ATLASSERT(0 != fr);

        m_fr = fr;
        HWND hWnd =  CDialogImpl<T>::Create( m_fr->m_hWndOwner );

        ATLASSERT(m_hWnd == hWnd);
        ATLASSERT(0 != hWnd);

        return hWnd;
    }

protected:
    LRESULT OnClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
    {
        DestroyWindow();				
        bHandled = FALSE;
        return 0;
    }

    LRESULT OnActivate(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
    {
        bHandled = FALSE;

        WORD fActive = LOWORD(wParam);

        if ( !m_fr->m_bPinned && fActive == WA_INACTIVE )
        {			
            PostMessage(WM_CLOSE);
        }
        return 0;
    }
    
    LRESULT OnSysCommand(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
    {
        UINT nCommand = wParam & 0xFFF0;
        if ( nCommand == SC_MOUSEMENU || nCommand == SC_DEFAULT )
        {	
            m_fr->m_bPinned = !m_fr->m_bPinned;
            UpdateIcon();			
        }
        else
        {
            bHandled = FALSE;
        }
        return 0;
    }	
    
    void UpdateIcon()
    {
        if ( m_fr->m_bPinned )
        {
            SetIcon(m_fr->m_hPinned, TRUE);
            SetIcon(m_fr->m_hPinned, FALSE);
        }
        else 
        {
            SetIcon(m_fr->m_hUnpinned, TRUE);
            SetIcon(m_fr->m_hUnpinned, FALSE);
        }
    }	

    virtual void OnFinalMessage(HWND /*hWnd*/)
    {	
        m_fr->m_dwFlags &= ~(FR_FINDNEXT | FR_REPLACE | FR_REPLACEALL);
        m_fr->m_dwFlags |= FR_DIALOGTERM;
        ::PostMessage(m_fr->m_hWndOwner, m_fr->m_findReplaceMsg, m_fr->m_findReplaceMsgParam, 0);
    }

    void FillCombo(DWORD dwCombo, CSimpleArray<TCHAR*>& ra)
    {
        WTL::CComboBox combo(GetDlgItem(dwCombo));
        
        for (int i = ra.GetSize() - 1; i >= 0 ; i--)
        {
            combo.AddString( ra[i] );
        }
    }

    void UpdateHistory(DWORD dwCombo, CSimpleArray<TCHAR*>& ra, TCHAR* pStr, DWORD cb)
    {
        int nLen = GetDlgItemText(dwCombo, pStr, cb);
         
        ATLASSERT(nLen != 0);
        
        for (int i = 0; i < ra.GetSize(); i++ )
            if (lstrcmp(pStr, ra[i]) == 0) {
                delete [] ra[i];
                ra.RemoveAt(i);				
                break;
            }
                    
        TCHAR* pAdd = new TCHAR[nLen + 1];
        lstrcpy(pAdd, pStr);
        ra.Add(pAdd);
    }
    
};

class FindDlg : 
    public FindReplaceDlgT<FindDlg>	
{	
    typedef FindReplaceDlgT<FindDlg> theBase;

public:
    
    enum { IDD = IDD_FIND };

    BEGIN_MSG_MAP(FindDlg)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        COMMAND_ID_HANDLER(IDOK, OnFind)
        COMMAND_HANDLER(IDC_COMBO_FIND, CBN_SELCHANGE, OnUpdateUI)	
        COMMAND_HANDLER(IDC_COMBO_FIND, CBN_EDITCHANGE, OnUpdateUI)	
        COMMAND_HANDLER(IDC_COMBO_FIND, CBN_CLOSEUP, OnUpdateUI)		
        COMMAND_HANDLER(IDC_CHECK_WILDCARD, BN_CLICKED, OnBnClickedWildcard)
        COMMAND_HANDLER(IDC_CHECK_REGEX, BN_CLICKED, OnBnClickedRegex)
        CHAIN_MSG_MAP(theBase)
    END_MSG_MAP()
        
//////////	
private:

    LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {	
        CenterWindow(GetParent());		
            
        FillCombo(IDC_COMBO_FIND, m_fr->m_FindHistory);
        
        CheckDlgButton(IDC_CHECK_WHOLE_WORD, m_fr->MatchWholeWord() ? BST_CHECKED : BST_UNCHECKED);
        CheckDlgButton(IDC_CHECK_CASE, m_fr->MatchCase() ? BST_CHECKED : BST_UNCHECKED);
        CheckDlgButton(IDC_CHECK_WILDCARD, m_fr->Wildcard() ? BST_CHECKED : BST_UNCHECKED);
        CheckDlgButton(IDC_CHECK_REGEX, m_fr->Regex() ? BST_CHECKED : BST_UNCHECKED);
        CheckRadioButton(IDC_RADIO_UP, IDC_RADIO_DOWN, m_fr->SearchDown() ? IDC_RADIO_DOWN : IDC_RADIO_UP);		
        SetDlgItemText(IDC_COMBO_FIND, m_fr->m_szFindWhat);
        CheckDlgButton(IDC_FINDWRAP, m_fr->IsFindWrap() ? BST_CHECKED : BST_UNCHECKED);

        if (m_fr->m_enableWildcardRegex)
        {
            switch(m_fr->m_mode)
            {
            case FINDMODE_WILDCARD:
                CheckDlgButton(IDC_CHECK_WILDCARD, BST_CHECKED);
                ::EnableWindow(GetDlgItem(IDC_CHECK_CASE), false);
                ::EnableWindow(GetDlgItem(IDC_CHECK_WHOLE_WORD), false);
                break;
            case FINDMODE_REGEX:
                CheckDlgButton(IDC_CHECK_REGEX, BST_CHECKED);
                ::EnableWindow(GetDlgItem(IDC_CHECK_CASE), false);
                ::EnableWindow(GetDlgItem(IDC_CHECK_WHOLE_WORD), false);
                break;
            }
        }
        else
        {
            ::EnableWindow(GetDlgItem(IDC_CHECK_WILDCARD), false);
            ::EnableWindow(GetDlgItem(IDC_CHECK_REGEX), false);
        }

        BOOL b;
        OnUpdateUI(0, 0, 0, b);
        UpdateIcon();
        return 1;
    }

    LRESULT OnFind(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        UpdateHistory(IDC_COMBO_FIND, m_fr->m_FindHistory, m_fr->m_szFindWhat, 
            sizeof m_fr->m_szFindWhat / sizeof TCHAR);

        m_fr->m_dwFlags = FR_FINDNEXT;		
        m_fr->m_dwFlags |= ((IsDlgButtonChecked(IDC_CHECK_WHOLE_WORD) == BST_CHECKED) ? FR_WHOLEWORD : 0);
        m_fr->m_dwFlags |= ((IsDlgButtonChecked(IDC_CHECK_CASE) == BST_CHECKED) ? FR_MATCHCASE : 0);
        m_fr->m_dwFlags |= ((IsDlgButtonChecked(IDC_RADIO_DOWN) == BST_CHECKED) ? FR_DOWN : 0);

        m_fr->m_findWrap = IsDlgButtonChecked(IDC_FINDWRAP) == BST_CHECKED;

        if (IsDlgButtonChecked(IDC_CHECK_WILDCARD) == BST_CHECKED)
            m_fr->m_mode = FINDMODE_WILDCARD;
        else if (IsDlgButtonChecked(IDC_CHECK_REGEX) == BST_CHECKED)
            m_fr->m_mode = FINDMODE_REGEX;
        else
            m_fr->m_mode = FINDMODE_NONE;
        
        ::SendMessage(m_fr->m_hWndOwner, m_fr->m_findReplaceMsg, m_fr->m_findReplaceMsgParam, 0);

        if (!m_fr->m_bPinned)
            PostMessage(WM_CLOSE);

        return 0;
    }	

    LRESULT OnUpdateUI(WORD wNotifyCode, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        BOOL bEnable = TRUE;
        if (wNotifyCode != CBN_SELCHANGE) 
        {			
            bEnable = ::GetWindowTextLength(GetDlgItem(IDC_COMBO_FIND)) ? TRUE : FALSE;
        }
        ::EnableWindow(GetDlgItem(IDOK), bEnable);
        return 0;
    }

    LRESULT OnBnClickedWildcard(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        if (IsDlgButtonChecked(IDC_CHECK_WILDCARD) == BST_CHECKED)
        {
            CheckDlgButton(IDC_CHECK_REGEX, BST_UNCHECKED);
            ::EnableWindow(GetDlgItem(IDC_CHECK_CASE), false);
            ::EnableWindow(GetDlgItem(IDC_CHECK_WHOLE_WORD), false);
        }
        else
        {
            ::EnableWindow(GetDlgItem(IDC_CHECK_CASE), true);
            ::EnableWindow(GetDlgItem(IDC_CHECK_WHOLE_WORD), true);
        }

        return 0;
    }

    LRESULT OnBnClickedRegex(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        if (IsDlgButtonChecked(IDC_CHECK_REGEX) == BST_CHECKED)
        {
            CheckDlgButton(IDC_CHECK_WILDCARD, BST_UNCHECKED);
            ::EnableWindow(GetDlgItem(IDC_CHECK_CASE), false);
            ::EnableWindow(GetDlgItem(IDC_CHECK_WHOLE_WORD), false);
        }
        else
        {
            ::EnableWindow(GetDlgItem(IDC_CHECK_CASE), true);
            ::EnableWindow(GetDlgItem(IDC_CHECK_WHOLE_WORD), true);
        }

        return 0;
    }
};

class ReplaceDlg : 
    public FindReplaceDlgT<ReplaceDlg>	
{	
    typedef FindReplaceDlgT<ReplaceDlg> theBase;

public:
    
    enum { IDD = IDD_REPLACE };

    BEGIN_MSG_MAP(FindDlg)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        COMMAND_ID_HANDLER(IDOK, OnFind)
        COMMAND_ID_HANDLER(IDC_BUTTON_REPLACE, OnFind)
        COMMAND_ID_HANDLER(IDC_BUTTON_REPLACE_ALL, OnFind)
        COMMAND_HANDLER(IDC_COMBO_FIND, CBN_EDITCHANGE, OnUpdateUI)	
        COMMAND_HANDLER(IDC_COMBO_FIND, CBN_CLOSEUP, OnUpdateUI)		
        COMMAND_HANDLER(IDC_COMBO_FIND, CBN_SELCHANGE, OnUpdateUI)	
        COMMAND_HANDLER(IDC_COMBO_REPLACE, CBN_EDITCHANGE, OnUpdateUI_Replace)	
        COMMAND_HANDLER(IDC_COMBO_REPLACE, CBN_CLOSEUP, OnUpdateUI_Replace)		
        CHAIN_MSG_MAP(theBase)
    END_MSG_MAP()
        
//////////	
private:

    LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {	
        CenterWindow(GetParent());		
        
        FillCombo(IDC_COMBO_FIND, m_fr->m_FindHistory);
        FillCombo(IDC_COMBO_REPLACE, m_fr->m_ReplaceHistory);

        CheckDlgButton(IDC_CHECK_WHOLE_WORD, m_fr->MatchWholeWord() ? BST_CHECKED : BST_UNCHECKED);
        CheckDlgButton(IDC_CHECK_CASE, m_fr->MatchCase() ? BST_CHECKED : BST_UNCHECKED);
        
        SetDlgItemText(IDC_COMBO_FIND, m_fr->m_szFindWhat);
        SetDlgItemText(IDC_COMBO_REPLACE, m_fr->m_szReplaceWith);

        BOOL b;
        OnUpdateUI(0, 0, 0, b);
        OnUpdateUI_Replace(0, 0, 0, b);
        UpdateIcon();
        return 1;
    }

    LRESULT OnFind(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        UpdateHistory(IDC_COMBO_FIND, m_fr->m_FindHistory, m_fr->m_szFindWhat, 
            sizeof m_fr->m_szFindWhat / sizeof TCHAR);

        if (wID != IDOK)
            UpdateHistory(IDC_COMBO_REPLACE, m_fr->m_ReplaceHistory, m_fr->m_szReplaceWith, 
                sizeof m_fr->m_szReplaceWith/ sizeof TCHAR);

        if (wID == IDOK)
            m_fr->m_dwFlags = FR_FINDNEXT;		
        else if (wID == IDC_BUTTON_REPLACE)
            m_fr->m_dwFlags = FR_REPLACE;		
        else if (wID == IDC_BUTTON_REPLACE_ALL)
            m_fr->m_dwFlags = FR_REPLACEALL;	
        else
            ATLASSERT(0);

        m_fr->m_dwFlags |= ((IsDlgButtonChecked(IDC_CHECK_WHOLE_WORD) == BST_CHECKED) ? FR_WHOLEWORD : 0);
        m_fr->m_dwFlags |= ((IsDlgButtonChecked(IDC_CHECK_CASE) == BST_CHECKED) ? FR_MATCHCASE : 0);
        m_fr->m_dwFlags |= FR_DOWN;
        
        ::SendMessage(m_fr->m_hWndOwner, m_fr->m_findReplaceMsg, m_fr->m_findReplaceMsgParam, 0);

        if (!m_fr->m_bPinned)
            PostMessage(WM_CLOSE);

        return 0;
    }	

    LRESULT OnUpdateUI(WORD wNotifyCode, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        BOOL bEnable = TRUE;
        if (wNotifyCode != CBN_SELCHANGE) 
        {			
            bEnable = ::GetWindowTextLength(GetDlgItem(IDC_COMBO_FIND)) ? TRUE : FALSE;
        }
        ::EnableWindow(GetDlgItem(IDOK), bEnable);				 
        ::EnableWindow(GetDlgItem(IDC_BUTTON_REPLACE), bEnable);
        ::EnableWindow(GetDlgItem(IDC_BUTTON_REPLACE_ALL), bEnable);		
        return 0;
    }

    LRESULT OnUpdateUI_Replace(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        BOOL bEnable = ::GetWindowTextLength(GetDlgItem(IDC_COMBO_FIND)) ? TRUE : FALSE;

        if (bEnable && (wID != CBN_SELCHANGE)) {			
            bEnable = true;//::GetWindowTextLength(GetDlgItem(IDC_COMBO_REPLACE)) ? TRUE : FALSE;
        }

        ::EnableWindow(GetDlgItem(IDC_BUTTON_REPLACE), bEnable);
        ::EnableWindow(GetDlgItem(IDC_BUTTON_REPLACE_ALL), bEnable);		
        return 0;
    }
    
};

__interface IFindReplace
{
    FindReplace& GetFindReplace();
    void FindReplaceOnEditFind(const std::_tstring &findWhat, bool enableWildcardRegex, bool enableTooltip);
    void FindReplaceOnEditReplace(const std::_tstring &replaceWhat);
};

#endif //_FindReplace_40f6ea36_72a8_489f_8ba8_cdd73efd19e1

