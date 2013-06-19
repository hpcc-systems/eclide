#pragma once

#include "EclLib.h"

struct ParsedVersion
{
	enum TYPE
	{
		 UNKNOWN,
		 ECLIDE,
		 ECLCC,
		 OSS,
		 BUILD,
		 LAST
	} type;
	std::_tstring preffix;
	unsigned int majorVersion;
	unsigned int minorVersion;
	std::_tstring minorVersionAlpha;
	unsigned int pointVersion;
	std::_tstring suffixStr;
	unsigned int suffixInt;
	std::_tstring github;
	unsigned int build;
	std::_tstring buildAlpha;
	unsigned int lang_MajorVersion;
	unsigned int lang_MinorVersion;
	unsigned int lang_PointVersion;

	ParsedVersion()
	{
		majorVersion = 0;
		minorVersion = 0;
		pointVersion = 0;
		suffixInt = 0;
		lang_MajorVersion = 0;
		lang_MinorVersion = 0;
		lang_PointVersion = 0;
		build = 0;
	}

	/*  Note:  ECL IDE Major version was brought in line with HPCC Platform for version 4.
		The new version order is (in historic order):
		QueryBuilder 3
		QueryBuilder 4
		QueryBuilder/eclide 5 (eclide was only released internally)
		eclide 6
		eclide 4
	*/
	bool operator == (const ParsedVersion & other) const
	{
		int thisMajorVersion = majorVersion == 4 ? 14 : majorVersion;
		int otherMajorVersion = other.majorVersion == 4 ? 14 : other.majorVersion;
		if (thisMajorVersion == otherMajorVersion && minorVersion == other.minorVersion && pointVersion == other.pointVersion)
			return true;
		return false;
	}

	bool operator < (const ParsedVersion & other) const
	{
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
