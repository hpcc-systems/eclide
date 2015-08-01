#include <string>
#include <stack>
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <vector>
#include <list>
#include <xtree>
#include <set>
#include <utility>
#include <unordered_map>
namespace std
{
#ifdef _UNICODE
	#define	_tstring wstring
	#define	_tcout wcout
	#define	_tclog wclog
	#define	_tcerr wcerr
	#define	_tofstream wofstream
	#define	_tifstream wifstream
#else
	#define	_tstring string
	#define	_tcout cout
	#define	_tclog clog
	#define	_tcerr cerr
	#define	_tofstream ofstream
	#define	_tifstream ifstream
#endif
}

