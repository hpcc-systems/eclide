#pragma once

class CTestSocket 
{
protected:
    std::string m_ip;
    std::string m_port;

public:
    CTestSocket(const std::string & ip, const std::string & port);
    ~CTestSocket();

    const TCHAR * SendRequest(const std::_tstring & request, std::_tstring & response) const;

protected:
    const char * SendRequest(const std::string & request, std::string & response) const;
    const char * SendRequest2(const std::string & request, std::string & response) const;
};

class CSessionID
{
protected:
    mutable std::_tstring m_id;

public:
    CSessionID();

    void GenerateID() const;
    void SetID(const std::_tstring & id);
    const TCHAR * GetID() const;
};
