#include "StdAfx.h"

#include "wildcard.h"

static bool WildMatchN ( const char *src, int srclen, int srcidx, const char *pat, int patlen, int patidx,bool nocase)
{
	char next_char;
	for (;;) 
	{
		if (patidx == patlen)
			return (srcidx == srclen);
		next_char = pat[patidx++];
		if (next_char == '?') 
		{
			if (srcidx == srclen)
				return false;
			srcidx++;
		}
		else if (next_char != '*') 
		{
			if (nocase) {
				if ((srcidx == srclen) || (toupper(src[srcidx])!=toupper(next_char)))
					return false;
			}
			else
				if ((srcidx == srclen) || (src[srcidx]!=next_char))
					return false;
			srcidx++;
		}
		else 
		{
			if (patidx == patlen)
				return true;
			while (srcidx < srclen) 
			{
				if (WildMatchN(src,srclen,srcidx, pat, patlen, patidx,nocase))
					return true;
				srcidx++;
			}
			return false;
		}
	}
}

CLIB_API bool WildMatch(const std::string & src, const std::string & pat, bool nocase)
{
	if (pat[0]=='*') 
	{
		// common case optimization
		int i = pat.length();
		int j = src.length();
		while (--i>0) 
		{
			if (pat[i]=='*') goto Normal;
			if (j--==0) return false;
			if (nocase) 
			{
				if ((toupper(pat[i])!=toupper(src[j]))&&(pat[i]!='?'))
					return false;
			}
			else
				if ((pat[i]!=src[j])&&(pat[i]!='?'))
					return false;
		}
		return true;
	}
Normal:
	return WildMatchN(src.c_str(),src.length(),0,pat.c_str(),pat.length(),0,nocase);
}
