#pragma once

#ifdef SEISINT_LIBEXPORTS
#  define ECLLIB_API 
#else
#  ifdef ECLLIB_EXPORTS
#    define ECLLIB_API __declspec(dllexport)
#  else
#    define ECLLIB_API __declspec(dllimport)
#  endif
#endif
