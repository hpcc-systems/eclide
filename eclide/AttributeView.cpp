// BuilderView.cpp : Implementation of CCAttributeView

#include "stdafx.h"
#include "resource.h"      

#include "AttributeView.h"

BOOL CAttributeView::PreTranslateMessage(MSG* pMsg)
{
	return IsDialogMessage(pMsg);
}
