#if !defined(AFX_BUTTONMENU_H__20051019_1B00_196B_95C4_0080AD509054__INCLUDED_)
#define AFX_BUTTONMENU_H__20051019_1B00_196B_95C4_0080AD509054__INCLUDED_

#pragma once

/////////////////////////////////////////////////////////////////////////////
// ButtonMenu - A menu button control
//
// Written by Bjarke Viksoe (bjarke@viksoe.dk)
// Copyright (c) 2005 Bjarke Viksoe.
//
// Add the following macro to the parent's message map:
//   REFLECT_NOTIFICATIONS()
//
// This code may be used in compiled form in any way you desire. This
// source file may be redistributed by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to you or your
// computer whatsoever. It's free, so don't hassle me about it.
//
// Add the following macro to the parent's message map:
//   REFLECT_NOTIFICATIONS()
//
// Beware of bugs.
//

#ifndef __cplusplus
  #error WTL requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLCTRLS_H__
  #error RtfStatic.h requires atlctrls.h to be included first
#endif

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")


/////////////////////////////////////////////////////////////////////////////
// 

// Extended button/menu styles
#define BMS_EX_CHECKBOXES      0x00000001
#define BMS_EX_FREESIZE        0x00000002
#define BMS_EX_SHAREIMAGELIST  0x00000004
#define BMS_EX_ICONSATSECOND   0x00000008
#define BMS_EX_HOTCHECKLINE    0x00000010


// Webdings font characters
#define ARROW_CHAR _T("\x36")
#define CHECK_CHAR _T("\x61")

#ifndef BlendRGB
   #define BlendRGB(c1, c2, factor) \
      RGB( GetRValue(c1) + ((GetRValue(c2) - GetRValue(c1)) * factor / 100L), \
           GetGValue(c1) + ((GetGValue(c2) - GetGValue(c1)) * factor / 100L), \
           GetBValue(c1) + ((GetBValue(c2) - GetBValue(c1)) * factor / 100L) )
#endif

namespace WTL
{
template< class T, class TBase = CButton, class TWinTraits = CControlWinTraits >
class ATL_NO_VTABLE CButtonMenuImpl : 
   public CWindowImpl< T, TBase, TWinTraits >,
   public COwnerDraw< T >,
   public CCustomDraw< T >
{
public:
   DECLARE_WND_SUPERCLASS(NULL, TBase::GetWndClassName())

   enum
   {
      CX_MENUSPACE = 3,
      CY_MENUSPACE = 2,
      CX_ICONSPACE = 2,
      CX_CHECKSIZE = 16,
      CX_CHECKSPACE = 2,
   };

   CMenuHandle m_menu;
   CFont m_WebdingsFont;
   CFont m_MenuFont;
   CImageList m_Images;
   BOOL m_bIsMenuOwner;
   DWORD m_dwMenuStyle;
   DWORD m_dwCommCtrlVer;
   SIZE m_szMenuItem;

   CButtonMenuImpl() :
      m_bIsMenuOwner(FALSE),
      m_dwMenuStyle(0)
   {
   }
   ~CButtonMenuImpl()
   {
      if( m_bIsMenuOwner ) m_menu.DestroyMenu();
      if( (m_dwMenuStyle & BMS_EX_SHAREIMAGELIST) == 0 ) m_Images.Destroy();
   }

   // These are here because WTL fails to include both COwnerDraw and
   // CCustomDraw in same class with ATL3.
   BOOL m_bHandledOD;
   BOOL IsMsgHandled() const
   {
      return m_bHandledOD;
   }
   void SetMsgHandled(BOOL bHandled)
   {
      m_bHandledOD = bHandled;
   }

   // Operations

   BOOL SubclassWindow(HWND hWnd)
   {
      ATLASSERT(m_hWnd==NULL);
      ATLASSERT(::IsWindow(hWnd));
      BOOL bRet = CWindowImpl< T, TBase, TWinTraits >::SubclassWindow(hWnd);
      if( bRet ) _Init();
      return bRet;
   }
   void SetMenu(UINT nRes)
   {
      if( m_bIsMenuOwner ) m_menu.DestroyMenu();
      m_menu.LoadMenu(nRes);
      m_bIsMenuOwner = TRUE;
   }
   void SetMenu(HMENU hMenu)
   {
      if( m_bIsMenuOwner ) m_menu.DestroyMenu();
      m_menu.Attach(hMenu);
      m_bIsMenuOwner = FALSE;
   }
   void SetExtendedMenuStyle(DWORD dwStyle)
   {
      m_dwMenuStyle = dwStyle;
   }
   void SetImageList(HIMAGELIST hImages)
   {
      m_Images.Attach(hImages);
   }

   // Implementation

   void _Init()
   {
      ATLASSERT(::IsWindow(m_hWnd));

      // Which one of Bill Gate's inventions is this...
      static DWORD dwCommCtrlVer = 0;
      if( dwCommCtrlVer == 0 ) {
         DWORD dwMinor = 0;
         AtlGetCommCtrlVersion(&dwCommCtrlVer, &dwMinor);
      }
      m_dwCommCtrlVer = dwCommCtrlVer;

      // An ownerdrawn button draws crap on WinXP, but a default push-button
      // will send out NM_CUSTOMDRAW notifications, so let's branch on OS version...
      // BUG: We should actually use IsAppThemed() here!!!
      SetButtonStyle(m_dwCommCtrlVer >= 6 ? BS_PUSHBUTTON : BS_OWNERDRAW);

      CLogFont lf = AtlGetDefaultGuiFont();
      _tcscpy(lf.lfFaceName, _T("Webdings"));
      lf.lfCharSet = SYMBOL_CHARSET;
      lf.lfHeight = 18;
      if( m_WebdingsFont.IsNull() ) m_WebdingsFont.CreateFontIndirect(&lf);
      lf.SetMenuFont();
      if( m_MenuFont.IsNull() ) m_MenuFont.CreateFontIndirect(&lf);
   }

   CMenuHandle _PrepareMenu()
   {
      // Build a menu popup menu while figuring out the
      // best size for the menu.
      CClientDC dc = m_hWnd;
      HFONT hOldFont = dc.SelectFont(m_MenuFont);
      TEXTMETRIC tm = { 0 };
      dc.GetTextMetrics(&tm);
      SIZE szIcon = { 0, 0 };
      if( !m_Images.IsNull() ) m_Images.GetIconSize(szIcon);
      SIZE size = { 0, max(szIcon.cy, tm.tmHeight) };
      CMenuHandle sub = m_menu.GetSubMenu(0);
      UINT nMenuItems = (UINT) sub.GetMenuItemCount();
      CMenu menu;
      menu.CreatePopupMenu();
      for( UINT i = 0; i < nMenuItems; i++ ) {
         TCHAR szMenuText[200] = { 0 };
         CMenuItemInfo mii;
         mii.fMask = MIIM_TYPE | MIIM_STATE | MIIM_ID | MIIM_DATA;
         mii.dwTypeData = szMenuText;
         mii.cch = (sizeof(szMenuText) / sizeof(TCHAR)) - 1;
         sub.GetMenuItemInfo(i, TRUE, &mii);
         UINT uFlags = 0;
         if( (mii.fState & MF_CHECKED) != 0 ) uFlags |= MF_CHECKED;
         mii.fState = uFlags;
         mii.fType = MF_OWNERDRAW;
         mii.dwItemData = i;
         menu.InsertMenuItem(i, TRUE, &mii);
         SIZE szText = { 0 };
         dc.GetTextExtent(szMenuText, -1, &szText);
         if( szText.cx > size.cx ) size.cx = szText.cx;
      }
      dc.SelectFont(hOldFont);
      size.cy += CX_MENUSPACE * 2;
      size.cx += CY_MENUSPACE * 2;
      if( szIcon.cx > 0 ) size.cx += szIcon.cx + CX_ICONSPACE;
      if( (m_dwMenuStyle & BMS_EX_CHECKBOXES) != 0 ) {
         size.cx += CX_CHECKSIZE + CX_CHECKSPACE;
      }
      if( (m_dwMenuStyle & BMS_EX_FREESIZE) == 0 ) {
         RECT rcWindow = { 0 };
         GetWindowRect(&rcWindow);
         size.cx = max(size.cx, rcWindow.right - rcWindow.left);
         // Windows will add space for the checkmark; we've already
         // been there...
         size.cx -= ::GetSystemMetrics(SM_CYMENUCHECK);
      }
      m_szMenuItem = size;
      return menu.Detach();
   }

   // Message map and handlers

   BEGIN_MSG_MAP(CButtonMenuImpl)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_KEYUP, OnKeyUp)
      MESSAGE_HANDLER(WM_DRAWITEM, OnMenuDrawItem)
      MESSAGE_HANDLER(WM_MEASUREITEM, OnMenuMeasureItem)
      REFLECTED_COMMAND_CODE_HANDLER(BN_CLICKED, OnClicked)
      CHAIN_MSG_MAP_ALT( COwnerDraw< T >, 1 )
      CHAIN_MSG_MAP_ALT( CCustomDraw< T >, 1 )
   END_MSG_MAP()

   LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
   {
      LRESULT lRes = DefWindowProc();
      _Init();
      return lRes;
   }

   LRESULT OnKeyUp(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
   {
      if( wParam == VK_F4 ) Click();
      bHandled = FALSE;
      return 0;
   }

   LRESULT OnClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   {
      ATLASSERT(m_menu.IsMenu());      
      // Create a new popup menu but with the ownerdrawn flags.
      CMenu menu = _PrepareMenu();
      // Popup the menu under the button
      RECT rcWindow = { 0 };
      GetWindowRect(&rcWindow);
      // We want to position it slightly different depending
      // on the Windows version. It just looks better.
      POINT ptMenu = { rcWindow.left, rcWindow.bottom };
      if( m_dwCommCtrlVer >= 6 ) {
         ptMenu.x += 2;
         ptMenu.y -= 2;
      }
      UINT uAnim = 0;
#if(WINVER >= 0x0500)
      uAnim |= TPM_VERPOSANIMATION;
#endif
      // Let's tell Windows never to place the menu onto our button surface
      TPMPARAMS tpmp = { 0 };
      tpmp.cbSize = sizeof(TPMPARAMS);
      tpmp.rcExclude = rcWindow;
      ::InflateRect(&tpmp.rcExclude, -1, -1);
      // We are ready to show the menu.
      CWindow wndTop = GetTopLevelWindow();
      wndTop.SendMessage(WM_INITMENUPOPUP, (WPARAM) (HMENU) menu);
      UINT nCmd = (UINT) ::TrackPopupMenuEx(menu, 
         TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD | TPM_VERTICAL | uAnim,
         ptMenu.x, ptMenu.y,
         m_hWnd, &tpmp);
      if( nCmd == 0 ) return 0;
      // We got a result; let's post it to main window...
      wndTop.PostMessage(WM_COMMAND, MAKEWPARAM(nCmd, 0));
      return 0;
   }

   // Menu drawing

   LRESULT OnMenuMeasureItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
   {
      LPMEASUREITEMSTRUCT lpMIS = (LPMEASUREITEMSTRUCT) lParam;
      lpMIS->itemWidth = m_szMenuItem.cx;
      lpMIS->itemHeight = m_szMenuItem.cy;
      return 1;
   }
   LRESULT OnMenuDrawItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
   {
      LPDRAWITEMSTRUCT lpDIS = (LPDRAWITEMSTRUCT) lParam;
      CDCHandle dc = lpDIS->hDC;
      RECT rcItem = lpDIS->rcItem;
      UINT iIndex = lpDIS->itemData;
      // Get info about menuitem
      TCHAR szText[200] = { 0 };
      CMenuItemInfo mii;
      mii.fMask = MIIM_TYPE | MIIM_STATE;
      mii.dwTypeData = szText;
      mii.cch = (sizeof(szText) / sizeof(TCHAR)) - 1;
      m_menu.GetSubMenu(0).GetMenuItemInfo(iIndex, TRUE, &mii);
      // Fill background
      COLORREF clrText = ::GetSysColor(COLOR_MENUTEXT);
      COLORREF clrBack = ::GetSysColor(COLOR_MENU);
      if( (m_dwMenuStyle & BMS_EX_HOTCHECKLINE) != 0 && (lpDIS->itemState & ODS_CHECKED) != 0 ) {
         COLORREF clrHighlight = ::GetSysColor(COLOR_HIGHLIGHT);
         clrBack = BlendRGB(clrHighlight, clrBack, 80);
      }
      if( (lpDIS->itemState & ODS_SELECTED) != 0 ) {
         clrText = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
         clrBack = ::GetSysColor(COLOR_HIGHLIGHT);
      }
      if( (lpDIS->itemState & ODS_DISABLED) != 0 ) {
         clrText = ::GetSysColor(COLOR_GRAYTEXT);
      }
      dc.FillSolidRect(&rcItem, clrBack);
      // Draw text line
      ::InflateRect(&rcItem, -CX_MENUSPACE, -CY_MENUSPACE);
      dc.SetBkMode(TRANSPARENT);
      dc.SetTextColor(clrText);
      if( (m_dwMenuStyle & (BMS_EX_CHECKBOXES|BMS_EX_ICONSATSECOND)) == (BMS_EX_CHECKBOXES|BMS_EX_ICONSATSECOND) ) {
         if( (lpDIS->itemState & ODS_CHECKED) != 0 ) {
            HFONT hOldFont = dc.SelectFont(m_WebdingsFont);
            dc.DrawText(CHECK_CHAR, 1, &rcItem, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
            dc.SelectFont(hOldFont);
         }
         rcItem.left += CX_CHECKSIZE + CX_CHECKSPACE;
      }
      if( !m_Images.IsNull() ) {
         SIZE szIcon = { 0 };
         m_Images.GetIconSize(szIcon);
         CIcon icon = m_Images.GetIcon(iIndex, ILD_TRANSPARENT);
         POINT pt = { rcItem.left, rcItem.top };
         pt.y += ((rcItem.bottom - rcItem.top) / 2) - (szIcon.cy / 2);
         dc.DrawIconEx(pt.x, pt.y, icon, szIcon.cx, szIcon.cy);
         rcItem.left += szIcon.cx + CX_ICONSPACE;
      }
      if( (m_dwMenuStyle & (BMS_EX_CHECKBOXES|BMS_EX_ICONSATSECOND)) == (BMS_EX_CHECKBOXES) ) {
         if( (lpDIS->itemState & ODS_CHECKED) != 0 ) {
            HFONT hOldFont = dc.SelectFont(m_WebdingsFont);
            dc.DrawText(CHECK_CHAR, 1, &rcItem, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
            dc.SelectFont(hOldFont);
         }
         rcItem.left += CX_CHECKSIZE + CX_CHECKSPACE;
      }
      HFONT hOldFont = dc.SelectFont(m_MenuFont);
      dc.DrawText(szText, -1, &rcItem, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
      dc.SelectFont(hOldFont);
      return TRUE;
   }

   // COwnerDraw (pre-WindowsXP)

   void DrawItem(LPDRAWITEMSTRUCT lpDIS)
   {
      CDCHandle dc = lpDIS->hDC;
      RECT rcItem = lpDIS->rcItem;
      bool bSelected = (lpDIS->itemState & ODS_SELECTED) != 0;
      bool bDisabled = (lpDIS->itemState & ODS_DISABLED) != 0;
      bool bFocused = (lpDIS->itemState & ODS_FOCUS) != 0;

      RECT rcButton = rcItem;
      if( bFocused ) {
         dc.Draw3dRect(&rcButton, ::GetSysColor(COLOR_3DDKSHADOW), ::GetSysColor(COLOR_3DDKSHADOW));
         ::InflateRect(&rcButton, -1, -1);
      }
      UINT uState = 0;
      if( bSelected ) uState |= DFCS_PUSHED;
      if( bDisabled ) uState |= DFCS_INACTIVE;
      ::DrawFrameControl(dc, &rcButton, DFC_BUTTON, DFCS_BUTTONPUSH | uState);

      TCHAR szText[200] = { 0 };
      GetWindowText(szText, 199);
      if( bSelected ) ::OffsetRect(&rcItem, 1, 1);
      HFONT hOldFont = dc.SelectFont(GetFont());
      dc.SetBkMode(TRANSPARENT);
      dc.SetTextColor(::GetSysColor(bDisabled ? COLOR_GRAYTEXT : COLOR_BTNTEXT));
      if( bDisabled ) {
         RECT rcText = rcItem;
         RECT rcBound = rcText;
         dc.DrawText(szText, -1, &rcBound, DT_SINGLELINE | DT_LEFT | DT_TOP | DT_CALCRECT);
         ::OffsetRect(&rcText, (rcText.right - rcBound.right) / 2, (rcText.bottom - rcBound.bottom) / 2);
         ::DrawState(dc, NULL, NULL, (LPARAM) szText, 0, rcText.left, rcText.top, rcText.right, rcText.bottom, DST_PREFIXTEXT | DSS_DISABLED);
      }
      else {
         RECT rcText = rcItem;
         dc.DrawText(szText, -1, &rcText, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
      }
      RECT rcLine = { rcItem.right - 22, rcItem.top + 5, rcItem.right - 20, rcItem.bottom - 4 };
      dc.Draw3dRect(&rcLine, ::GetSysColor(COLOR_BTNSHADOW), ::GetSysColor(COLOR_BTNHIGHLIGHT));
      dc.SelectFont(m_WebdingsFont);
      RECT rcArrow = { rcItem.right - 18, rcItem.top, rcItem.right, rcItem.bottom };
      dc.DrawText(ARROW_CHAR, 1, &rcArrow, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
      HBRUSH hOldBrush = dc.SelectBrush((HBRUSH)::GetStockObject(HOLLOW_BRUSH));
      ::InflateRect(&rcButton, -3, -3);
      if( bFocused ) dc.DrawFocusRect(&rcButton);
      dc.SelectBrush(hOldBrush);
      dc.SelectFont(hOldFont);
   }

   // CCustomDraw (WindowsXP)

   DWORD OnPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW /*lpNMCD*/)
   {
      return CDRF_NOTIFYPOSTPAINT;
   }
   DWORD OnPostPaint(int /*idCtrl*/, LPNMCUSTOMDRAW lpNMCD)
   {
      CDCHandle dc = lpNMCD->hdc;
      RECT rc = lpNMCD->rc;
      RECT rcLine = { rc.right - 22, rc.top + 6, rc.right - 20, rc.bottom - 6 };
      bool bDisabled = (lpNMCD->uItemState & (CDIS_DISABLED|CDIS_GRAYED)) != 0;
      dc.Draw3dRect(&rcLine, ::GetSysColor(COLOR_BTNSHADOW), ::GetSysColor(COLOR_BTNHIGHLIGHT));
      HFONT hOldFont = dc.SelectFont(m_WebdingsFont);
      RECT rcArrow = { rc.right - 18, rc.top, rc.right, rc.bottom };
      dc.SetBkMode(TRANSPARENT);
      dc.SetTextColor(::GetSysColor(bDisabled ? COLOR_GRAYTEXT : COLOR_BTNTEXT));
      dc.DrawText(ARROW_CHAR, 1, &rcArrow, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
      dc.SelectFont(hOldFont);
      return CDRF_DODEFAULT;
   }
};

class CButtonMenuCtrl : public CButtonMenuImpl<CButtonMenuCtrl, CButton, CWinTraitsOR<BS_PUSHBUTTON|BS_CENTER|BS_VCENTER|WS_TABSTOP> >
{
public:
   DECLARE_WND_SUPERCLASS(_T("WTL_ButtonMenu"), GetWndClassName())  
};
}

#endif // !defined(AFX_BUTTONMENU_H__20051019_1B00_196B_95C4_0080AD509054__INCLUDED_)
