////  Barebones stdafx.h
//#include "stdversion.h"
//
//#define BOOST_ASIO_ENABLE_BUFFER_DEBUGGING 
//#define BOOST_ASIO_NO_DEFAULT_LINKED_LIBS
//#define BOOST_ASIO_DISABLE_IOCP 
//
//#include <tchar.h>
//#include <limits.h>
//
//#include <AtlBase.h>
//
////  ---  Needed to provide version of getaddrinfo on win2k  ---
//#include <Ws2tcpip.h>		
//#include <Wspiapi.h>
////  ---  ---  ---
//
//#include <string>
//namespace std
//{
//#ifdef _UNICODE
//	#define	_tstring wstring
//#else
//	#define	_tstring string
//#endif
//}
//
//#include <boost/asio.hpp>
//#include <boost/random.hpp>
//#include <boost/lexical_cast.hpp>
//#include <boost/thread.hpp>
//#include <boost/bind.hpp>
//#include <boost/format.hpp>
//namespace boost
//{
//#ifdef _UNICODE
//	#define _tformat wformat
//#else
//	#define _tformat format
//#endif
//}

#include "StdAfx.h"

#include "TestSocket.h"
#include <boost/random.hpp>

//using boost::asio::ip::tcp;
//
//unsigned long SwapEndian(unsigned long nLongNumber)
//{
//   unsigned long nRetNumber ;
//
//   __asm
//   {
//      mov eax, nLongNumber
//      xchg ah, al
//      ror eax, 16
//      xchg ah, al
//      mov nRetNumber, eax
//   }
//
//   return nRetNumber;
//}
//
CTestSocket::CTestSocket(const std::string & ip, const std::string & port) : m_ip(ip), m_port(port)
{
}

CTestSocket::~CTestSocket(void)
{
}

const TCHAR * CTestSocket::SendRequest(const std::_tstring & request, std::_tstring & response) const 
{
	std::string nrequest = CT2A(request.c_str(), CP_UTF8);
	std::string nresponse;
	SendRequest(nrequest, nresponse);
	response = CA2T(nresponse.c_str(), CP_UTF8);
	return response.c_str();
}

const char * CTestSocket::SendRequest(const std::string & request, std::string & response) const 
{
#ifdef ASIO
	try
	{
		boost::asio::io_service io_service;
		tcp::resolver resolver(io_service);
		tcp::resolver::query query(tcp::v4(), m_ip, m_port);
		tcp::resolver::iterator iterator = resolver.resolve(query);

		tcp::socket s(io_service);
		s.connect(*iterator);
		//Send
		union
		{
			unsigned int request_length;
			char request_buffer[4];
		} requestinfo, responseinfo;

		requestinfo.request_length = request.length();
		requestinfo.request_length = SwapEndian(requestinfo.request_length);

		std::vector<char> d2(request.begin(), request.end());

		s.write_some(boost::asio::buffer((void *)requestinfo.request_buffer, 4));
		s.write_some(boost::asio::buffer(d2));

		std::vector<char> buff;
		for(;;)
		{
			responseinfo.request_length = 0;
			boost::system::error_code error;
			std::size_t readSize = boost::asio::read(s, boost::asio::buffer((void *)responseinfo.request_buffer, 4));
			responseinfo.request_length = SwapEndian(responseinfo.request_length);
			if (responseinfo.request_length == 0)
				break;

			buff.resize(responseinfo.request_length);
			readSize = boost::asio::read(s, boost::asio::buffer((void *)&buff[0], responseinfo.request_length));
			response.append(&buff[0], responseinfo.request_length);
		}
	}
	catch (std::exception& e)
	{
		response.append((boost::format("<Exception><Source>Roxie</Source><Code>%1%</Code><Message>%2%</Message></Exception>") % "999" % e.what()).str().c_str());
	}
	catch(...)
	{
		std::cerr << "exception: ???\n";
	}
#endif
	return response.c_str();
}

const char * CTestSocket::SendRequest2(const std::string & request, std::string & response) const 
{
#ifdef ASIO
	response = "";
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;
    int iResult;
    
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        ATLTRACE("WSAStartup failed: %d\n", iResult);
        return NULL;
    }

    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(m_ip.c_str(), m_port.c_str(), &hints, &result);
    if ( iResult != 0 ) {
        ATLTRACE("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return NULL;
    }

    // Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            ATLTRACE("Error at socket(): %ld\n", WSAGetLastError());
            freeaddrinfo(result);
            WSACleanup();
            return NULL;
        }

        // Connect to server.
        iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        ATLTRACE("Unable to connect to server!\n");
        WSACleanup();
        return NULL;
    }

	// Send an initial buffer
	union  
	{
		unsigned long length; 
		char buffer[sizeof(unsigned long)];
	} requestinfo, responseInfo;

	requestinfo.length = request.length();
	requestinfo.length = SwapEndian(requestinfo.length);

    iResult = send(ConnectSocket, requestinfo.buffer, 4, 0);
    if (iResult == SOCKET_ERROR) {
        ATLTRACE("send failed: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return NULL;
    }
    ATLTRACE("Bytes Sent: %ld\n", iResult);
	iResult = send(ConnectSocket, request.c_str(), request.length(), 0);
    if (iResult == SOCKET_ERROR) {
        ATLTRACE("send failed: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return NULL;
    }
    ATLTRACE("Bytes Sent: %ld\n", iResult);

	// Receive until the peer closes the connection
	std::vector<char> buff;
    do {
        iResult = recv(ConnectSocket, responseInfo.buffer, 4, 0);
		if (iResult != 4)
            ATLTRACE("Bytes expected, received: %d, %d\n", 4, iResult);

		if ( iResult > 0 )
            ATLTRACE("Bytes received: %d\n", iResult);
        else if ( iResult == 0 )
		{
            ATLTRACE("Connection closed\n");
			break;
		}
        else
		{
            ATLTRACE("recv failed: %d\n", WSAGetLastError());
			break;
		}
		responseInfo.length = SwapEndian(responseInfo.length);
		buff.resize(responseInfo.length);
		if (responseInfo.length == 0)
			break;
        iResult = recv(ConnectSocket, &buff[0], responseInfo.length, 0);
		if (iResult != responseInfo.length)
            ATLTRACE("Bytes expected, received: %d, %d\n", responseInfo.length, iResult);

		if ( iResult > 0 )
            ATLTRACE("Bytes received: %d\n", iResult);
        else if ( iResult == 0 )
		{
            ATLTRACE("Connection closed\n");
			break;
		}
        else
		{
            ATLTRACE("recv failed: %d\n", WSAGetLastError());
			break;
		}
		response.append(&buff[0], responseInfo.length);
    } while( iResult > 0 );

	// cleanup
    iResult = shutdown(ConnectSocket, SD_BOTH);
    if (iResult == SOCKET_ERROR) {
        ATLTRACE("shutdown failed: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return NULL;
    }

	closesocket(ConnectSocket);
    WSACleanup();
#endif

	return response.c_str();
}
//  ===========================================================================
CSessionID::CSessionID()
{
}

void CSessionID::GenerateID() const
{
	boost::mt19937 rng;
	time_t now_time;
	rng.seed(time(&now_time));
	boost::uniform_int<> range(0, INT_MAX);
	boost::variate_generator<boost::mt19937&, boost::uniform_int<> > generator(rng, range);
	m_id = boost::lexical_cast<std::_tstring>(generator());
}

void CSessionID::SetID(const std::_tstring & id)
{
	m_id = id;
}

const TCHAR * CSessionID::GetID() const
{
	if (m_id.empty())
		GenerateID();

	return m_id.c_str();
}
//  ===========================================================================
