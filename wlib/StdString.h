#pragma once
#include "wlib.h"

template <class T>
void TrimString(T& string)
{
	for ( unsigned leftTrim=0; leftTrim < string.length(); leftTrim++ )
	{
		if ( !_istspace(string[leftTrim]) )
		{
			if ( leftTrim )
				string = string.substr(leftTrim,string.length()-leftTrim);
			break;
		}
	}
}

#if defined(__ATLDDX_H__)

template <class T>
class CWinDataExchangeStd : public WTL::CWinDataExchange<T>
{
public:
	BOOL DDX_Text_Required(UINT nID, UINT minLen)
	{
		T* pT = static_cast<T*>(this);
		BOOL bSuccess = TRUE;

		HWND hctrl = pT->GetDlgItem(nID);
		if ( !hctrl )
		{
			bSuccess = FALSE;
		}
		else
		{
			int nLen = ::GetWindowTextLength(hctrl);
			if ( nLen < (int)minLen )
			{
				_XData data = { ddxDataText };
				data.textData.nLength = nLen;
				data.textData.nMaxLength = -1;
				pT->OnDataValidateError(nID, TRUE, data);
				bSuccess = FALSE;
			}
		}
		return bSuccess;
	}

	BOOL DDX_Text(UINT nID, std::_tstring& text, int /*cbSize*/, BOOL bSave, BOOL bValidate = FALSE, int nLength = 0)
	{
		T* pT = static_cast<T*>(this);
		BOOL bSuccess = TRUE;

		HWND hctrl = pT->GetDlgItem(nID);
		if ( !hctrl )
		{
			bSuccess = FALSE;
		}
		else
		{
			if (bSave)
			{
				int nLen = ::GetWindowTextLength(hctrl);
				TCHAR *buffer = (TCHAR *)_alloca(sizeof(TCHAR)*(nLen+1));
				if ( ::GetWindowText(hctrl, buffer, nLen+1) )
					text = buffer;
				else
					text.erase();
			}
			else
			{
				bSuccess = ::SetWindowText(hctrl, text.c_str());
			}
		}

		if(!bSuccess)
		{
			pT->OnDataExchangeError(nID, bSave);
		}
		else if(bSave && bValidate)   // validation
		{
			ATLASSERT(nLength > 0);
			if((int)text.length() > nLength)
			{
				_XData data = { ddxDataText };
				data.textData.nLength = (int)text.length();
				data.textData.nMaxLength = nLength;
				pT->OnDataValidateError(nID, bSave, data);
				bSuccess = FALSE;
			}
		}
		return bSuccess;
	}
	BOOL DDX_Combo(UINT nID, std::_tstring& text, BOOL bSave)
	{
		T* pT = static_cast<T*>(this);
		BOOL bSuccess = TRUE;

		HWND hctrl = pT->GetDlgItem(nID);
		if ( !hctrl )
		{
			bSuccess = FALSE;
		}
		else
		{
			WTL::CComboBox combo(hctrl);
			if (bSave)
			{
				int nSel = combo.GetCurSel();
				if ( nSel < 0 )
				{
					int nLen = min(1024*4, combo.GetWindowTextLength());
					TCHAR *buffer = (TCHAR *)_alloca(sizeof(TCHAR)*(nLen+1));
					bSuccess = combo.GetWindowText(buffer, nLen+1);
					if ( bSuccess )
						text = buffer;
				}
				else
				{
					int nLen = min(1024*4, combo.GetLBTextLen(nSel));
					TCHAR *buffer = (TCHAR *)_alloca(sizeof(TCHAR)*(nLen+1));
					if ( combo.GetLBText(nSel, buffer) > 0 )
						text = buffer;
					else
						text.erase();
				}
			}
			else if ( text.size() )
			{
				int nItem = combo.FindStringExact(-1,text.c_str());
				if ( nItem >= 0 )
				{
					combo.SetCurSel(nItem);
				}
				else
				{
					bSuccess = ::SetWindowText(hctrl, text.c_str());
				}
			}
		}

		if(!bSuccess)
		{
			pT->OnDataExchangeError(nID, bSave);
		}
		return bSuccess;
	}

	BOOL DDX_Combo(UINT nID, int& val, BOOL bSave)
	{
		std::_tstring strValue;
		if ( !bSave )
		{
			TCHAR buffer[64] = {0};
			::_itot(val, buffer, 10 );
			strValue = buffer;
		}
		BOOL bSuccess = DDX_Combo(nID,strValue,bSave);
		if ( bSuccess && bSave )
		{
			try
			{
				val = boost::lexical_cast<int>(strValue);
			}
			catch(boost::bad_lexical_cast)
			{
				T* pT = static_cast<T*>(this);
				_XData data = { ddxDataInt };
				data.intData.nVal = strValue.length();
				data.intData.nMin = -1;
				data.intData.nMax = -1;
				pT->OnDataValidateError(nID, bSave, data);
				bSuccess = FALSE;
			}
		}
		return bSuccess;
	}
};

#define DDX_COMBO_VALUE(nID, var) \
		if(nCtlID == (UINT)-1 || nCtlID == nID) \
		{ \
			if(!DDX_Combo(nID, var, bSaveAndValidate)) \
				return FALSE; \
		}
#define DDX_TEXT_REQUIRED(nID) \
		if((nCtlID == (UINT)-1 || nCtlID == nID) && bSaveAndValidate) \
		{ \
			if(!DDX_Text_Required(nID, 1)) \
				return FALSE; \
		}
#define DDX_TEXT_REQUIRED_MAYBE(nID, bRequiredNow) \
		if((nCtlID == (UINT)-1 || nCtlID == nID) && bRequiredNow && bSaveAndValidate) \
		{ \
			if(!DDX_Text_Required(nID, 1)) \
				return FALSE; \
		}

#define DDX_TEXT_REQUIRED_ERROR(e) (e.nDataType == ddxDataText && e.textData.nMaxLength == -1)
#define DDX_INT_RANGE_ERROR(e) (e.nDataType == ddxDataInt)

#endif
