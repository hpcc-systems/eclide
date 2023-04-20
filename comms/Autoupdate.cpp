#include "StdAfx.h"
#include "Autoupdate.h"

#include <Logger.h>
#include <Thread.h>
#include <VersionParser.h>

using boost::asio::ip::tcp;

class CAutoUpdate : public IAutoUpdate, public clib::CLockableUnknown
{
protected: 
    bool m_hasUpdate;

    std::string m_header;
    std::_tstring m_response;
    std::string m_error;

public:
    IMPLEMENT_CUNKNOWN;

    CAutoUpdate(void) 
    {
        m_hasUpdate = false;
    }

    ~CAutoUpdate(void)
    {
    }

    bool HasUpdate(const std::_tstring & _url, const std::_tstring & _product, const std::_tstring & currentVersion)
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        Check(_url, _product);
        ParsedVersion curr, remote;
        if (ParseVersion(currentVersion, curr) && ParseVersion(m_response, remote))
            return curr < remote;
        return false;
    }

    const TCHAR * GetResponse() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_response.c_str();
    }

    void Check(const std::_tstring & _url, const std::_tstring & _product)
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        std::string url = CT2A(_url.c_str());
        std::string product = CT2A(_product.c_str());
        try
        {
            boost::asio::io_service io_service;

            // Get a list of endpoints corresponding to the server name.
            tcp::resolver resolver(io_service);
            tcp::resolver::query query(url, "http");
            tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
            tcp::resolver::iterator end;

            // Try each endpoint until we successfully establish a connection.
            tcp::socket socket(io_service);
            boost::system::error_code error = boost::asio::error::host_not_found;
            while (error && endpoint_iterator != end)
            {
                socket.close();
                socket.connect(*endpoint_iterator++, error);
            }
            if (error)
                throw boost::system::system_error(error);

            // Form the request. We specify the "Connection: close" header so that the
            // server will close the socket after transmitting the response. This will
            // allow us to treat all data up until the EOF as the content.
            boost::asio::streambuf request;
            std::ostream request_stream(&request);
            request_stream << "GET " << product << " HTTP/1.0\r\n";
            request_stream << "Host: " << url << "\r\n";
            request_stream << "Accept: */*\r\n";
            request_stream << "Connection: close\r\n\r\n";

            // Send the request.
            boost::asio::write(socket, request);

            // Read the response status line. The response streambuf will automatically
            // grow to accommodate the entire line. The growth may be limited by passing
            // a maximum size to the streambuf constructor.
            boost::asio::streambuf response;
            boost::asio::read_until(socket, response, "\r\n");

            // Check that response is OK.
            std::istream response_stream(&response);
            std::string http_version;
            response_stream >> http_version;
            unsigned int status_code;
            response_stream >> status_code;
            std::string status_message;
            std::getline(response_stream, status_message);
            if (!response_stream || http_version.substr(0, 5) != "HTTP/")
            {
                m_error = "Invalid response";
                return;
            }
            if (status_code != 200)
            {
                m_error = "Response returned with status code ";
                try {
                    m_error += boost::lexical_cast<std::string>(status_code);
                } catch (const boost::bad_lexical_cast &) {
                    m_error += "\n";
                }
                return;
            }

            // Read the response headers, which are terminated by a blank line.
            boost::asio::read_until(socket, response, "\r\n\r\n");

            // Process the response headers.
            std::string header;
            while (std::getline(response_stream, header) && header != "\r")
                m_header += header + "\n";

            std::stringstream m_responseStream;
            // Write whatever content we already have to output.
            if (response.size() > 0)
                m_responseStream << &response;

            // Read until EOF, writing data to output as we go.
            while (boost::asio::read(socket, response, boost::asio::transfer_at_least(1), error))
                m_responseStream << &response;
            if (error != boost::asio::error::eof)
                throw boost::system::system_error(error);
            m_response = CA2T(m_responseStream.str().c_str());
            boost::algorithm::trim_if(m_response, boost::algorithm::is_any_of(_T("\r\n")));
        }
        catch (const std::exception & ex)
        {
            m_error = ex.what();
        }
        return;
    }
};

IAutoUpdate * CreateIAutoUpdate()
{
    return new CAutoUpdate();
}

COMMS_API bool DNSLookup(std::_tstring & url) {
    size_t found = url.find_first_of(_T("://"));
    if (found == std::string::npos) return false;
    std::_tstring protocol = url.substr(0, found);
    std::_tstring url_new = url.substr(found + 3);
    size_t found1 = url_new.find_first_of(_T(":"));
    if (found1 == std::string::npos) return false;
    std::_tstring host = url_new.substr(0, found1);
    size_t found2 = url_new.find_first_of(_T("/"));
    if (found2 == std::string::npos) return false;
    std::_tstring port = url_new.substr(found1 + 1, found2 - found1 - 1);
    std::_tstring path = url_new.substr(found2);

    //  localhost resolves in a strange way
    if (!boost::algorithm::iequals(host, _T("localhost"))) {
        boost::asio::io_service io_service;
        tcp::resolver resolver(io_service);
        try {
            std::string hostname = CT2A(host.c_str());
            tcp::resolver::query query(hostname, "");
            tcp::resolver::iterator found = resolver.resolve(query);
            if (found != tcp::resolver::iterator()) {
                tcp::endpoint end = *found;
                std::_tstring ipAddr = CA2T(end.address().to_string().c_str());
                url = protocol + _T("://") + ipAddr + _T(":") + port + path;
                return true;
            }
        }
        catch (std::exception& e) {
        }
    }
    return false;
}
