#pragma once

#pragma warning(disable:4251)
#pragma warning(disable:4275)
#pragma warning(disable:4503)
#pragma warning(disable:4127)

//  Leak Checking ---
#if defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#endif

//  Target  ---
#include "stdversion.h"

// Platform SDK  ---
#include "stdplatform.h"

//  ATL  ---
#include <atlbase.h>
#include <atlstr.h>

//  WTL  ---
#define _WTL_NO_CSTRING
#include <atlapp.h>
#define _Module (*_pModule)

//  STL  ---
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <xtree>
#include <string>
#include <map>
#include <stack>
namespace std
{
#ifdef _UNICODE
    #define	_tstring wstring
#else
    #define	_tstring string
#endif
}

//  BOOST  ---
#define BOOST_SPIRIT_THREADSAFE 
#define PHOENIX_THREADSAFE
#define BOOST_LIB_DIAGNOSTIC
#ifdef _DEBUG
#define BOOST_SPIRIT_DEBUG  
#define BOOST_SPIRIT_DEBUG_OUT std::cerr
#endif
#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_symbols.hpp>
#include <boost/spirit/include/classic_lists.hpp>
#include <boost/spirit/include/classic_confix.hpp>
#include <boost/spirit/include/classic_chset.hpp>
#include <boost/spirit/include/classic_attribute.hpp>
#include <boost/spirit/include/classic_ast.hpp>
#include <boost/spirit/include/classic_tree_to_xml.hpp>
#include <boost/spirit/include/classic_parse_tree.hpp>
#include <boost/spirit/include/phoenix1_primitives.hpp>
#include <boost/spirit/include/phoenix1_operators.hpp>
#include <boost/spirit/include/phoenix1_binders.hpp>
#pragma warning(push)
#pragma warning(disable:4701)	//...\lexical_cast.hpp(191) : warning C4701: local variable 'result' may be used without having been initialized
#include <boost/lexical_cast.hpp>  
#pragma warning(pop)
#include <boost/concept_check.hpp>
#include <boost/concept_archetype.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp> 
#include <boost/format.hpp> 
#include <boost/serialization/map.hpp> 
#include <boost/serialization/vector.hpp> 
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
namespace boost
{
#ifdef _UNICODE
    #define _tformat wformat
#else
    #define _tformat format
#endif
}

//  Leak Checking ---
#ifdef _DEBUG 
# include <stdlib.h>
# include <crtdbg.h>
# define GJS_DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__) 
# define new GJS_DEBUG_NEW 
#endif

#pragma warning(3:4702;error:4702)	 //warning C4702: unreachable code
