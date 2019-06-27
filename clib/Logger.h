#pragma once

#include "clib.h"

#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#define __WFILE__ WIDEN(__FILE__)

#ifdef _UNICODE
#define __TFILE__ __WFILE__
#else 
#define __TFILE__ __FILE__
#endif

enum LEVEL
{
    LEVEL_FINEST = 0,
    LEVEL_FINER, 
    LEVEL_FINE, 
    LEVEL_CONFIG,
    LEVEL_DEBUG,
    LEVEL_INFO,
    LEVEL_WARNING, 
    LEVEL_SEVERE
};

class CLIB_API Logger
{
private:
    std::string m_file;
    int m_line;
    void operator()(LEVEL l, const boost::_tformat& fmt);

public:
    Logger(const char * filePath, int line);
    void operator()(LEVEL l);
    void operator()(LEVEL l, /*ATL::SOAPCLIENT_ERROR*/ int err);
//	void operator()(LEVEL l, const boost::format& fmt);
    void operator()(LEVEL l, const CComBSTR& str);
    void operator()(const std::_tstring & url, LEVEL l, const CComBSTR& str);
    void operator()(const CString & url, LEVEL l, const CComBSTR& str);
};

#ifdef _DEBUG
#	define _DBGLOG (Logger( __FILE__, __LINE__ ))
#else
#	define _DBGLOG (Logger( __FILE__, __LINE__ ))// __noop
#endif
