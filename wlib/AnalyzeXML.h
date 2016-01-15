#pragma once

__interface __declspec(uuid("C9FA255E-9B16-419e-A0E0-0588EB2AEBA4")) IXMLAnalyzer : public IUnknown
{
    void Analyze(const std::_tstring& xml);
    bool IsXML() const;
    unsigned CountColumns() const;
    void GetXPath(std::_tstring& xpath) const;
    void GetColumnName(unsigned column, std::_tstring& name) const;
};

IXMLAnalyzer* CreateIXMLAnalyzer();