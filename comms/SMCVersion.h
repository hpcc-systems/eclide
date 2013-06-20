#pragma once

#include "RecursiveMutex.h"
#include "Dali.h"

namespace SMC
{
const TCHAR * const DEFAULT_VERSION = _T("build_0_0");
__interface IVersion : public IUnknown
{
	float Get();
	const TCHAR * GetString();

	const TCHAR * GetPrefix() const;
	unsigned int GetMajor() const;
	unsigned int GetMinor() const;
	unsigned int GetPoint() const;
	const TCHAR * GetSuffixString() const;
	unsigned int GetSuffixInteger() const;
	int Compare(const IVersion * other) const;
};
class IVersionCompare
{
public:
	enum DISTANCE_UPPERRANGE
	{
		DISTANCE_SUFFIXINT=100,
		DISTANCE_SUFFIXSTR=1000,
		DISTANCE_POINT=100000,
		DISTANCE_MINOR=100000000
	};
	unsigned int distance(IVersion * l, IVersion * r)
	{
		unsigned int retVal = 0;
		retVal += std::abs((int)r->GetSuffixInteger() - (int)l->GetSuffixInteger());
		retVal += std::abs(_tcscmp(r->GetSuffixString(), l->GetSuffixString()))				* 100;
		retVal += std::abs((int)r->GetPoint() - (int)l->GetPoint())							* 1000;
		retVal += std::abs((int)r->GetMinor() - (int)l->GetMinor())							* 100000;
		retVal += std::abs((int)r->GetMajor() - (int)l->GetMajor())							* 100000000;
		return retVal;
	}

	bool equals(IVersion * l, IVersion * r) const
	{
		return l->GetMajor() == r->GetMajor() && 
			l->GetMinor() == r->GetMinor() &&
			l->GetPoint() == r->GetPoint() &&
			_tcscmp(l->GetSuffixString(), r->GetSuffixString()) == 0 &&
			l->GetSuffixInteger() == r->GetSuffixInteger();
	}

	bool operator ()(IVersion * l, IVersion * r) const
	{
		if (l->GetMajor() == r->GetMajor())
		{
			if (l->GetMinor() == r->GetMinor())
			{
				if (l->GetPoint() == r->GetPoint())
				{
					if (_tcslen(l->GetSuffixString()) && !_tcslen(r->GetSuffixString())) // "" > "rc" > "closedown"
						return true;

					int strComp = _tcscmp(l->GetSuffixString(), r->GetSuffixString());  // "rc" > "closedown"
					if (strComp == 0)
						return l->GetSuffixInteger() < l->GetSuffixInteger();

					return strComp < 0;
				}
				return l->GetPoint() < r->GetPoint();
			}
			return l->GetMinor() < r->GetMinor();
		}
		return l->GetMajor() < r->GetMajor();
	}
};
}
COMMS_API SMC::IVersion * GetCommsVersion();

