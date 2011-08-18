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
		// Specifically for IDE version numbers
		if (majorVersion == other.majorVersion)
		{
			if (minorVersion == other.minorVersion)
			{
				if (pointVersion == other.pointVersion)
					return build < other.build;
				return pointVersion < other.pointVersion;
			}
			return minorVersion < other.minorVersion;
		}
		return majorVersion < other.majorVersion;
	}
};

ECLLIB_API bool ParseVersion(const std::_tstring & version, ParsedVersion & result);
