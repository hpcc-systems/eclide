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

CLIB_API const TCHAR * SortedSerialize(StdStringVector & source, std::_tstring & target, const TCHAR * delim)
{
    std::sort(source.begin(), source.end(), StringCompareUpperCased());
    for(StdStringVector::const_iterator itr = source.begin(); itr != source.end(); ++itr)
    {
        if (!target.empty())
            target += delim;
        target += itr->c_str();
    }
    return target.c_str();
}

CLIB_API StdStringVector::size_type SortedDeserialize(const std::_tstring & source, StdStringVector & target, const TCHAR * delim)
{
    boost::algorithm::split(target, source, boost::algorithm::is_any_of(delim), boost::algorithm::token_compress_on).size(); 
    std::sort(target.begin(), target.end(), StringCompareUpperCased());
    return target.size();
}

#ifdef _DEBUG
class CTest {
public:
    CTest::CTest() {
        std::_tstring in, out;
        StdStringVector list;

        in = _T("a b c d e f");
        list.clear();
        out.clear();
        SortedDeserialize(in, list);
        SortedSerialize(list, out);
        ATLASSERT(list.size() == 6);
        ATLASSERT(boost::algorithm::equals(in, out));

        in = _T("a,b,c,d,e,f");
        list.clear();
        out.clear();
        SortedDeserialize(in, list, _T(","));
        SortedSerialize(list, out, _T(","));
        ATLASSERT(list.size() == 6);
        ATLASSERT(boost::algorithm::equals(in, out));
    }
} _SerializerTest;
#endif

