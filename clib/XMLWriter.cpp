#include "StdAfx.h"

#include "XMLWriter.h"

class CXMLWriter : public IXMLWriter, public CComInitialize, public CUnknown
{
protected:
    CComPtr<IMXWriter> pWriter;
    CComPtr<IMXAttributes> pMXAttrs;  
    CComQIPtr<ISAXAttributes> pSAXAttrs;
    CComQIPtr<ISAXContentHandler> pContentHandler;
    CComQIPtr<ISAXDTDHandler> pDTDHandler;
    CComQIPtr<ISAXLexicalHandler> pLexicalHandler;
    CComQIPtr<ISAXDeclHandler> pDeclHandler;
    CComQIPtr<ISAXErrorHandler> pErrorHandler;

    CComBSTR m_empty;

    std::stack<std::_tstring> m_elementStack;

public:
    BEGIN_CUNKNOWN
    END_CUNKNOWN(CUnknown)

    CXMLWriter(bool prettyPrint = false);

    void PushElement(const std::_tstring & element, const std::_tstring & value = _T(""));
    void PushElement(const std::_tstring & element, int value);
    void PopElement();
    const wchar_t * EndDocument(std::wstring & xml);
    const char * EndDocument(std::string & xml);
};
//  ===========================================================================
CXMLWriter::CXMLWriter(bool prettyPrint) 
{
    m_empty = L"";
    HRESULT hr = pWriter.CoCreateInstance(CLSID_MXXMLWriter30, NULL);
    _ASSERT(SUCCEEDED(hr));
    pWriter->put_omitXMLDeclaration(VARIANT_TRUE);

    // create attributes
    hr = pMXAttrs.CoCreateInstance(CLSID_SAXAttributes30, NULL);
    _ASSERT(SUCCEEDED(hr));

    pSAXAttrs = pMXAttrs;
    pContentHandler = pWriter;
    pDTDHandler = pWriter;
    pLexicalHandler = pWriter;
    pDeclHandler = pWriter;
    pErrorHandler = pWriter;

    CComVariant var(L"");
    hr = pWriter->put_output(var);
    _ASSERT(SUCCEEDED(hr));

    if(prettyPrint)
    {
        pWriter->put_indent(VARIANT_TRUE);
    }

    // and manually call necessary events to generate XML file
    pContentHandler->startDocument();
    pMXAttrs->clear();
}

void CXMLWriter::PushElement(const std::_tstring & element, const std::_tstring & value)
{
    m_elementStack.push(element);
    CComBSTR e(element.c_str());
    pContentHandler->startElement(m_empty, m_empty.Length(), m_empty, m_empty.Length(), e, e.Length(), pSAXAttrs);
    if (value.length())
    {
        CComBSTR v = CT2W(value.c_str(), CP_UTF8);
        pContentHandler->characters(v, v.Length());
    }
}

void CXMLWriter::PushElement(const std::_tstring & element, int value)
{
    PushElement(element, boost::lexical_cast<std::_tstring>(value));
}

void CXMLWriter::PopElement()
{
    CComBSTR e = m_elementStack.top().c_str();
    pContentHandler->endElement(m_empty, m_empty.Length(), m_empty, m_empty.Length(), e, e.Length());
    m_elementStack.pop();
}

const wchar_t * CXMLWriter::EndDocument(std::wstring & xml)
{
    pContentHandler->endDocument();

    // output XML
    CComVariant var(L"");
    HRESULT hr = pWriter->get_output(&var);
    _ASSERT(SUCCEEDED(hr));
    xml = CT2W(CString(var));

    return xml.c_str();
}
const char * CXMLWriter::EndDocument(std::string & xml)
{
    std::wstring wxml;
    EndDocument(wxml);
    xml = CW2A(wxml.c_str(), CP_UTF8);
    return xml.c_str();
}
//  ===========================================================================
CLIB_API IXMLWriter * CreateIXMLWriter(bool prettyPrint)
{
    return new CXMLWriter(prettyPrint);
}
