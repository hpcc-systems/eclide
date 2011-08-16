#include "stdafx.h"

#include "stringset.h"

class StringCompareUpperCased
{
public:
	bool operator ()(const std::_tstring & l, const std::_tstring & r)
	{
		return _tcsicmp(l.c_str(), r.c_str()) < 0;
	}
};

CLIB_API CString & Serialize(StdStringVector & source, CString & result)
{
	std::sort(source.begin(), source.end(), StringCompareUpperCased());
	for(StdStringVector::const_iterator itr = source.begin(); itr != source.end(); ++itr)
	{
		if (!result.IsEmpty())
			result += _T(" ");
		result += itr->c_str();
	}
	return result;
}

CLIB_API const TCHAR * Serialize(StdStringVector & source, std::_tstring & result)
{
	CString tmp;
	Serialize(source, tmp);
	result = (const TCHAR *)tmp;
	return result.c_str();
}

