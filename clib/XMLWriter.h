#pragma once

#include "clib.h"
#include "msxml2.h"

__interface IXMLWriter : public IUnknown
{
    void PushElement(const std::_tstring & element, const std::_tstring & value = _T(""));
    void PushElement(const std::_tstring & element, int value);
    void PopElement();
    const wchar_t * EndDocument(std::wstring & xml);
    const char * EndDocument(std::string & xml);
};

CLIB_API IXMLWriter * CreateIXMLWriter(bool prettyPrint = false);
