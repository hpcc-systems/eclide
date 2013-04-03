#pragma once

#include "EclLib.h"

struct ParsedVersion
{
	std::_tstring preffix;
	unsigned int majorVersion;
	unsigned int minorVersion;
	std::_tstring minorVersionAlpha;
	unsigned int pointVersion;
	std::_tstring suffix;
	unsigned int build;
	std::_tstring buildAlpha;
	bool ossFlag;

	ParsedVersion()
	{
		majorVersion = 0;
		minorVersion = 0;
		pointVersion = 0;
		build = 0;
		ossFlag = false;
	}

	bool operator < (const ParsedVersion & other)
	{
		/*  Note:  ECL IDE Major version was brought in line with HPCC Platform for version 4.
			The new version order is (in historic order):
			QueryBuilder 3
			QueryBuilder 4
			QueryBuilder/eclide 5 (eclide was only released internally)
			eclide 6
			eclide 4
		*/

		int thisMajorVersion = majorVersion == 4 ? 14 : majorVersion;
		int otherMajorVersion = other.majorVersion == 4 ? 14 : other.majorVersion;
		if (thisMajorVersion == otherMajorVersion)
		{
			if (minorVersion == other.minorVersion)
			{
				if (pointVersion == other.pointVersion)
					return build < other.build;
				return pointVersion < other.pointVersion;
			}
			return minorVersion < other.minorVersion;
		}
		return thisMajorVersion < otherMajorVersion;
	}
};

ECLLIB_API bool ParseVersion(const std::_tstring & version, ParsedVersion & result);
