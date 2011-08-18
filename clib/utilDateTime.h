#pragma once

#include "clib.h"

const TCHAR * const DATETIME_ZERO = _T("1970-01-01T00:00:00Z");
#define UTCSTRINGLEN 20 //2000-01-01T00:00:00Z

void CLIB_API DateTimeToUTC(const SYSTEMTIME &date, const SYSTEMTIME &time, std::_tstring & result);
void CLIB_API DateTimeToUTC(const SYSTEMTIME &datetime, std::_tstring & result);
bool CLIB_API UTCToSystemTime(const std::_tstring & utc, SYSTEMTIME &dateTime, bool adjustToLocal);

void CLIB_API DateTimeToUTC(const boost::gregorian::date &date, std::_tstring & result);
bool CLIB_API UTCToDateTime(const std::_tstring & utc, boost::gregorian::date &date, boost::posix_time::ptime &time);

CLIB_API const TCHAR* CurrentDateTimeString(std::_tstring &result);
CLIB_API const TCHAR* CurrentDateTimeUTCString(std::_tstring &result);

CLIB_API void PtimeToFileTime(const boost::posix_time::ptime &time, FILETIME& ft);

CLIB_API void TimeTToUTC(const time_t & time, std::_tstring & result);
CLIB_API void UTCToTimeT(const std::_tstring & result, time_t & time);
