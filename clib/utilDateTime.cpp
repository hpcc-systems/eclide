#include "stdafx.h"

#include "UtilDateTime.h"

void DateTimeToUTC(const SYSTEMTIME &d, const SYSTEMTIME &t, std::_tstring & result)
{
    using namespace boost::gregorian;
    using namespace boost::posix_time;

    ptime localTime(second_clock::local_time());
    ptime utcTime(second_clock::universal_time());
    time_duration delta = utcTime - localTime;

    ptime userTime(date(d.wYear, d.wMonth, d.wDay), hours(t.wHour) + minutes(t.wMinute) + seconds(t.wSecond)); 
    userTime = userTime + delta;
    CString utc;
    utc = to_iso_extended_string(userTime).c_str();
    utc += _T("Z");
    result = utc;
}

void DateTimeToUTC(const SYSTEMTIME &dt, std::_tstring & result)
{
    DateTimeToUTC(dt, dt, result);
}
//2000-01-01T00:00:00Z
//01234567890123456789

bool CorrectURCToDateTime(const std::_tstring & _utc, SYSTEMTIME &dateTime)
{
    using namespace boost::gregorian;
    using namespace boost::posix_time;

    CString utc = DATETIME_ZERO;
    memset(&dateTime, 0, sizeof(dateTime));

    std::string iso = CT2A(_utc.c_str());
    try
    {
        typedef boost::date_time::c_local_adjustor<ptime> local_adj;
        ptime time1(from_iso_string(iso)), time(time1);
        time = local_adj::utc_to_local(time1);

        date d = time.date();
        dateTime.wYear = d.year();
        dateTime.wMonth = d.month();
        dateTime.wDay = d.day();
        time_duration t = time.time_of_day();
        dateTime.wHour = (WORD)t.hours();
        dateTime.wMinute = (WORD)t.minutes();
        dateTime.wSecond = (WORD)t.seconds();
    }
    catch(std::exception &)
    {
        memset(&dateTime, 0, sizeof(dateTime));
        return false;
    }
    return true;
}

const TCHAR * Mid(const std::_tstring & str, unsigned int startPos, unsigned int length, std::_tstring & result)
{
    result.clear();
    result.reserve(length);
    for(unsigned int i = startPos; i < startPos + length; ++i)
    {
        if (i >= str.length())
            break;
        result += str[i];
    }
    return result.c_str();
}

bool UTCToSystemTime(const std::_tstring & _utc, SYSTEMTIME &dateTime, bool adjustToLocal)
{
    if (CorrectURCToDateTime(_utc, dateTime))
    {
        return true;
    }

    using namespace boost::gregorian;
    using namespace boost::posix_time;

    std::_tstring utc = DATETIME_ZERO;

    if (_utc > utc)
        utc = _utc;

    typedef boost::date_time::c_local_adjustor<ptime> local_adj;

    memset(&dateTime, 0, sizeof(dateTime));

    std::string iso;
    std::_tstring buff;
    //Patch dubious date time string
    iso += CT2A(Mid(utc, 0, 4, buff));
    iso += CT2A(Mid(utc, 5, 2, buff));
    iso += CT2A(Mid(utc, 8, 2, buff));
    iso += 'T';
    iso += CT2A(Mid(utc, 11, 2, buff));
    iso += CT2A(Mid(utc, 14, 2, buff));
    iso += CT2A(Mid(utc, 17, 2, buff));
    try
    {
        ptime time1(from_iso_string(iso));
        ptime time(time1);
        if (adjustToLocal)
            time = local_adj::utc_to_local(time1);

        date d = time.date();
        dateTime.wYear = d.year();
        dateTime.wMonth = d.month();
        dateTime.wDay = d.day();
        time_duration t = time.time_of_day();
        dateTime.wHour = (WORD)t.hours();
        dateTime.wMinute = (WORD)t.minutes();
        dateTime.wSecond = (WORD)t.seconds();
    }
    catch(boost::bad_lexical_cast &)
    {
        memset(&dateTime, 0, sizeof(dateTime));
        return false;
    }
    return true;
}

void CLIB_API DateTimeToUTC(const boost::gregorian::date &date, std::_tstring & result)
{
    using namespace boost::gregorian;
    using namespace boost::posix_time;

    ptime localTime(second_clock::local_time());
    ptime utcTime(second_clock::universal_time());
    time_duration delta = utcTime - localTime;

    ptime userTime(date, hours(0) + minutes(0) + seconds(0)); 
    userTime = userTime + delta;
    CString utc;
    utc = to_iso_extended_string(userTime).c_str();
    utc += _T("Z");
    result = utc;
}

bool CLIB_API UTCToDateTime(const std::_tstring & utc, boost::gregorian::date &_date, boost::posix_time::ptime &_time)
{
    using namespace boost::gregorian;
    using namespace boost::posix_time;
    if (utc.length() == 0)
        return false;

    typedef boost::date_time::c_local_adjustor<ptime> local_adj;

    std::string iso;
    std::_tstring buff;
    iso += CT2A(Mid(utc, 0, 4, buff));
    iso += CT2A(Mid(utc, 5, 2, buff));
    iso += CT2A(Mid(utc, 8, 2, buff));
    iso += 'T';
    iso += CT2A(Mid(utc, 11, 2, buff));
    iso += CT2A(Mid(utc, 14, 2, buff));
    iso += CT2A(Mid(utc, 17, 2, buff));
    ptime tmpTime(from_iso_string(iso));
    _time = local_adj::utc_to_local(tmpTime);
    _date = _time.date();
    return true;
}

CLIB_API const TCHAR* CurrentDateTimeString(std::_tstring & result)
{
    CTime t = CTime::GetCurrentTime();
    result = t.Format(_T("%c"));
    return result.c_str();
}

CLIB_API const TCHAR* CurrentDateTimeUTCString(std::_tstring & result)
{
    CTime t = CTime::GetCurrentTime();
    result = t.FormatGmt(_T("%Y-%m-%dT%H:%M:%SZ"));
    return result.c_str();
}

CLIB_API void PtimeToFileTime(const boost::posix_time::ptime &time, FILETIME& ft)
{
    tm _tm=to_tm(time);
    //tm_sec 
    //Seconds after minute (0 – 59). 
    //tm_min 
    //Minutes after hour (0 – 59). 
    //tm_hour 
    //Hours after midnight (0 – 23). 
    //tm_mday 
    //Day of month (1 – 31). 
    //tm_mon 
    //Month (0 – 11; January = 0). 
    //tm_year 
    //Year (current year minus 1900). 
    //tm_wday 
    //Day of week (0 – 6; Sunday = 0). 
    //tm_yday 
    //Day of year (0 – 365; January 1 = 0). 
    //tm_isdst 
    //Positive value if daylight saving time is in effect;
    //0 if daylight saving time is not in effect; 
    //negative value if status of daylight saving time is unknown.
    //If the TZ environment variable is set, the C run-time library assumes
    //rules appropriate to the United States for implementing
    //the calculation of daylight-saving time (DST). 

    SYSTEMTIME st;
    memset(&st, 0, sizeof(st)); 
    st.wYear=_tm.tm_year+1900;
    st.wMonth=_tm.tm_mon+1;
    st.wDay=_tm.tm_mday;
    st.wHour=_tm.tm_hour;
    st.wMinute=_tm.tm_min;
    st.wSecond=_tm.tm_sec;
    BOOL ok=::SystemTimeToFileTime(&st,&ft);
    ATLASSERT(ok);
}

CLIB_API void TimeFromSystemTime(const SYSTEMTIME * pTime, time_t & time)
{
    struct tm tm;
    memset(&tm, 0, sizeof(tm));

    tm.tm_year = pTime->wYear - 1900;
    tm.tm_mon = pTime->wMonth - 1;
    tm.tm_mday = pTime->wDay;

    tm.tm_hour = pTime->wHour;
    tm.tm_min = pTime->wMinute;
    tm.tm_sec = pTime->wSecond;

    time = mktime(&tm);
}

CLIB_API void UTCToTimeT(const std::_tstring & utc, time_t & time)
{
    SYSTEMTIME dateTime;
    UTCToSystemTime(utc, dateTime, false);
    TimeFromSystemTime(&dateTime, time);
}

CLIB_API void TimeTToUTC(const time_t & time, std::_tstring & result)
{
    CTime t(time);
    result = t.FormatGmt(_T("%Y-%m-%dT%H:%M:%SZ"));
}
