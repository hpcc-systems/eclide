#pragma once

#ifdef WIN32
#  ifdef STATIC_LIB
#    define LIBAGRAPH_API
#  elif defined LIBAGRAPH_EXPORTS
#    define LIBAGRAPH_API __declspec(dllexport)
#  else
#    define LIBAGRAPH_API __declspec(dllimport)
#  endif
#else
#	if __GNUC__ >= 4
#		define LIBAGRAPH_API __attribute__ ((visibility("default")))
#	else
#		define LIBAGRAPH_API
#	endif
#endif

LIBAGRAPH_API bool DoLayout(const char * layout, const char* mem, const char* format, const char* scale, std::string & result);

