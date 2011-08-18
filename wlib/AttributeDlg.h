#pragma once

#include "wlib.h"

__interface IAttribute;
__interface IRepository;
namespace Dali
{
	__interface IDali;
}

WLIB_API INT_PTR DoGetAttr(CComPtr<IAttribute> &attr);
WLIB_API INT_PTR DoGetAttr(IRepository *rep, CComPtr<IAttribute> &attr, const TCHAR *prompt=0);
