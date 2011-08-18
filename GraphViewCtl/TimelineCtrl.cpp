#include "StdAfx.h"
#include "resource.h"

using namespace WTL;

#include "AggCommon.h"
#include "AggDC.h"
#include "Notifications.h"
#include "TimelineCtrl.h"

using namespace GraphTypes;

//-----------------------------------------------------------------------------

template <class T> bool IsLeapYear(const T& y)
{ 
	return !(y%4) && ( (y%100) || !(y%400) ); 
}

//-----------------------------------------------------------------------------

const TCHAR* CTimelineCtrl::teststr=_T("ABCDEFGHIJKLMNOPQRSTUVXYZWabcdefghijklmnopqrstuvxyzw");

//-----------------------------------------------------------------------------

CTimelineCtrl::CTimelineCtrl()
:	m_months(this),
	m_topBarHeight(0),
	m_topMargin(4),	
	m_topLeftMargin(2),
	m_topTextHeight(24),
	m_bottomTextHeight(14),
	m_bottomBarHeight(0),
	m_bottomMargin(2),
	m_bottomLeftMargin(1),
	m_fittingSlack(4),
	m_fonts(0),
	m_wndWidth(0),
	m_yearModulo(1),
	m_hourModulo(1),
	m_minModulo(1),
	m_secModulo(1),
	m_calcTicks(true),
	m_24HourFormat(false),
	m_showTickMarks(false),
	m_showHalfTickMarks(true),
	m_tick1Height(10),
	m_tick2Height(5),
	m_halfTickSpacing(8),
	m_topTypeface("Times New Roman"),
	m_bottomTypeface("Verdana"),
	m_dividerColor(GraphTypes::ARGB(GraphTypes::Color::Black))
{
	DWORD dwtz=::GetTimeZoneInformation(&m_tz);
	ATLASSERT(dwtz!=TIME_ZONE_ID_INVALID);

	::GetSystemTime(&m_beginTimeUTC);
	m_endTimeUTC=m_beginTimeUTC;
	BOOL bres=::SystemTimeToTzSpecificLocalTime(&m_tz, &m_beginTimeUTC, &m_beginTime);
	ATLASSERT(bres);
	bres=::SystemTimeToFileTime(&m_beginTime, &m_beginTick);
	ATLASSERT(bres);
	m_endTime=m_beginTime;
	m_endTick=m_beginTick;

	m_daysinamonth[0]=31;
	m_daysinamonth[1]=28;
	m_daysinamonth[2]=31;
	m_daysinamonth[3]=30;
	m_daysinamonth[4]=31;
	m_daysinamonth[5]=30;
	m_daysinamonth[6]=31;
	m_daysinamonth[7]=31;
	m_daysinamonth[8]=30;
	m_daysinamonth[9]=31;
	m_daysinamonth[10]=30;
	m_daysinamonth[11]=31;

	m_lightBgrColor.SetFromCOLORREF(::GetSysColor(COLOR_BTNFACE));
//	m_darkBgrColor.SetFromCOLORREF(::GetSysColor(COLOR_INACTIVECAPTION));
	m_darkBgrColor.SetFromCOLORREF(::GetSysColor(COLOR_ACTIVECAPTION));
	m_darkBgrColor.MoreBright(50);

	m_HandOpenCursor = LoadCursor(ATL::_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(IDC_HAND_OPEN));
}

//-----------------------------------------------------------------------------

CTimelineCtrl::~CTimelineCtrl(void)
{
}

//-----------------------------------------------------------------------------

void CTimelineCtrl::SetFontMgr(CAggFonts* fonts) 
{ 
	m_fonts=fonts; 
	// months strings and string widths
	m_months.Init();
	CalcStandardTextWidths();
}

//-----------------------------------------------------------------------------

int CTimelineCtrl::CalcTopBarHeight() 
{
	SizeF size=CAggDC::GetScaledTextExtent(
			teststr, 
			m_topTypeface.c_str(), 
			m_fonts,
			m_topTextHeight);

	m_topFontPixelHeight=size.Height;
	return round_int(size.Height) + m_topMargin*2;
}

//-----------------------------------------------------------------------------

int CTimelineCtrl::CalcBottomBarHeight()
{
	SizeF size=CAggDC::GetScaledTextExtent(
			teststr, 
			m_bottomTypeface.c_str(), 
			m_fonts,
			m_bottomTextHeight);

	m_bottomFontPixelHeight=size.Height;
	return round_int(size.Height) + m_bottomMargin*2 + m_tick1Height;
}

//-----------------------------------------------------------------------------

int CTimelineCtrl::Height() 
{
	ATLASSERT(m_fonts); // must have font mgr before determining text height

	if(m_topBarHeight==0)
		m_topBarHeight=CalcTopBarHeight();
	if(m_bottomBarHeight==0)
		m_bottomBarHeight=CalcBottomBarHeight();

	// 1 is for bar divider line
	return m_topBarHeight+m_bottomBarHeight+1;
}

//-----------------------------------------------------------------------------

void CTimelineCtrl::CalcStandardTextWidths()
{
	SizeF size;
	TCHAR strbuff[128];

	REAL maxBottomWidth=0;

	for(int i=0; i<10; ++i)
	{
		::_itot(i, strbuff, 10);
		size=CAggDC::GetScaledTextExtent(
			strbuff, 
			m_bottomTypeface.c_str(), 
			m_fonts,
			m_bottomTextHeight);
		if(size.Width>maxBottomWidth)
			maxBottomWidth=size.Width;
	}

	REAL maxTopWidth=0;

	for(int i=0; i<10; ++i)
	{
		::_itot(i, strbuff, 10);
		size=CAggDC::GetScaledTextExtent(
			strbuff, 
			m_topTypeface.c_str(), 
			m_fonts,
			m_topTextHeight);
		if(size.Width>maxTopWidth)
			maxTopWidth=size.Width;
	}

	// for now just combine 2 numbers, later we may want to be more precise
	// and fit, for ex., only 1-12 for AM/PM hours

	m_secsBottomMaxWidth=maxBottomWidth*2; // two numbers
	m_secsTopMaxWidth=maxTopWidth*2; 
	
	m_minsBottomMaxWidth=maxBottomWidth*2;
	m_minsTopMaxWidth=maxTopWidth*2; 

	m_hoursBottomMaxWidth=maxBottomWidth*2;
	m_hoursTopMaxWidth=maxTopWidth*2; 

	size=CAggDC::GetScaledTextExtent(
			_T("AM"), 
			m_topTypeface.c_str(), 
			m_fonts,
			m_topTextHeight);

	m_hourSymbolTopWidth=size.Width;

	size=CAggDC::GetScaledTextExtent(
			_T("AM"), 
			m_bottomTypeface.c_str(), 
			m_fonts,
			m_bottomTextHeight);

	m_hourSymbolBottomWidth=size.Width; 

	m_daysBottomMaxWidth=maxBottomWidth*2;
	m_daysTopMaxWidth=maxTopWidth*2; // two numbers

	size=CAggDC::GetScaledTextExtent(
			_T(" "), 
			m_topTypeface.c_str(), 
			m_fonts,
			m_topTextHeight);

	m_spaceTopWidth=size.Width;

	size=CAggDC::GetScaledTextExtent(
			_T(" "), 
			m_bottomTypeface.c_str(), 
			m_fonts,
			m_bottomTextHeight);

	m_spaceBottomWidth=size.Width;

	// colon and comma for now displayed only on top bar

	size=CAggDC::GetScaledTextExtent(
			_T(","), 
			m_topTypeface.c_str(), 
			m_fonts,
			m_topTextHeight);

	m_commaTopWidth=size.Width;

	size=CAggDC::GetScaledTextExtent(
		_T(":"), 
			m_topTypeface.c_str(), 
			m_fonts,
			m_topTextHeight);

	m_colonTopWidth=size.Width;
}

//-----------------------------------------------------------------------------

void CTimelineCtrl::CalcTextVerticalPosition(RectF& rct, bool topbar)
{
	if(topbar)
	{
		// vertically center topbar text
		rct.Height=m_topFontPixelHeight;
		rct.y=m_topBarHeight-m_topFontPixelHeight-m_topMargin;
	}
	else
	{
		REAL barheight=REAL(Height());
		rct.Height=m_bottomFontPixelHeight;
		rct.y=barheight-m_tick1Height-m_bottomFontPixelHeight;
	}
}

//-----------------------------------------------------------------------------

void CTimelineCtrl::CalcState()
{
	bool fitok=false;

	if(m_beginTick.dwLowDateTime==m_endTick.dwLowDateTime && 
		m_beginTick.dwHighDateTime==m_endTick.dwHighDateTime)
		return;

	if(m_beginTime.wYear!=m_endTime.wYear)
	{
		m_topUnits=unitsYears;
		m_topFormat=formatYears;
		fitok=FitYears(true);
		ATLASSERT(fitok);
		if(m_yearModulo<10)
		{
			m_bottomUnits=unitsMonths;
			fitok=FitMonths(false);
		}
		else
		{
			m_bottomUnits=unitsYears;
		}
	}
	else if(m_beginTime.wMonth!=m_endTime.wMonth)
	{
		// we don't want to display more than 3 time units on the top bar
		if(m_endTime.wMonth-m_beginTime.wMonth>3)
		{
			m_topUnits=unitsYears;
			m_topFormat=formatYears;
			fitok=FitYears(true);
			ATLASSERT(fitok);
			m_bottomUnits=unitsMonths;
			fitok=FitMonths(false);
		}
		else
		{
			m_topUnits=unitsMonths;
			m_topFormat=formatMonths;
			m_bottomUnits=unitsDays;
			fitok=FitDays(false);
		}
	}
	else if(m_beginTime.wDay!=m_endTime.wDay)
	{
		if(m_endTime.wDay-m_beginTime.wDay>3)
		{
			m_topUnits=unitsMonths;
			m_topFormat=formatMonths;
			m_bottomUnits=unitsDays;
			fitok=FitDays(false);
		}
		else
		{
			m_topUnits=unitsDays;
			m_topFormat=formatDays;
			m_bottomUnits=unitsHours;
			fitok=FitHours(false);
		}
	}
	else if(m_beginTime.wHour!=m_endTime.wHour)
	{
		if(m_endTime.wHour-m_beginTime.wHour>3)
		{
			m_topUnits=unitsDays;
			m_topFormat=formatDays;
			m_bottomUnits=unitsHours;
			fitok=FitHours(false);
		}
		else
		{
			m_topUnits=unitsHours;
			m_topFormat=formatHours;
			m_bottomUnits=unitsMinutes;
			fitok=FitMinutes(false);
		}
	}
	else
	{
		if(m_endTime.wMinute-m_beginTime.wMinute>3)
		{
			m_topUnits=unitsHours;
			m_topFormat=formatHours;
			m_bottomUnits=unitsMinutes;
			fitok=FitMinutes(false);
		}
		else
		{
			m_topUnits=unitsMinutes;
			m_topFormat=formatMinutes;
			m_bottomUnits=unitsSeconds;
			fitok=FitSeconds();
		}
	}

	ATLASSERT(fitok);
}

//-----------------------------------------------------------------------------
// 
// On WM_SIZE message
// Time on the left side of timeline window remains fixed.

void CTimelineCtrl::ResizeTimeline(int pixels)
{
	if(pixels==0)
	{
		m_wndWidth=0;
		return;
	}

	TimeFromScreenPos(m_endTick, pixels);
	m_wndWidth=pixels;

	::FileTimeToSystemTime(&m_endTick, &m_endTime);

	CalcTimeIntervalWidths();
	CalcState();
	m_calcTicks=true;
	Invalidate();
}

//-----------------------------------------------------------------------------
//
// negative value scrolls to the left

void CTimelineCtrl::ScrollTimeline(int pixels)
{
	if(pixels==0)
		return;

	double rcwidth=double(m_wndWidth);
	Ftint64 t1;
	t1.ft=m_beginTick;
	Ftint64 t2;
	t2.ft=m_endTick;

	__int64 duration=t2.i64-t1.i64; 
	duration/=10000000; // 100ths of nanoseconds to seconds

	REAL ratio; // how many minimum units per pixel
	BarUnits units; // minimum units (seconds or minutes)

	if(m_topUnits==unitsYears && (m_endTime.wYear-m_beginTime.wYear)>1)
	{
		double mins=duration/60.0;
		ratio=rcwidth/mins;	
		units=unitsMinutes;
	}
	else
	{
		double secs=duration;
		ratio=rcwidth/secs;
		units=unitsSeconds;
	}

	int diff=abs(pixels);
	double nm=double(diff)/ratio; // get pixel diff in seconds or minutes
	__int64 nsec;

	if(nm<1.0)
	{
		ATLASSERT(units==unitsSeconds);
		nm*=1000; // milliseconds
		ATLASSERT(nm>=1.0);
		nsec=__int64(round_int(nm));
		nsec*=10000; // turn into nanoseconds
	}
	else
	{
		nsec=__int64(round_int(nm));
		if(units==unitsMinutes) // convert to seconds
			nsec*=60;
		nsec*=10000000; // nanoseconds
	}

	if(pixels>0)
	{
		t1.i64+=nsec;
		t2.i64+=nsec;
	}
	else
	{
		t1.i64-=nsec;
		t2.i64-=nsec;
	}

	m_beginTick=t1.ft;
	BOOL bok=::FileTimeToSystemTime(&m_beginTick, &m_beginTime);
	ATLASSERT(bok);

	m_endTick=t2.ft;
	bok=::FileTimeToSystemTime(&m_endTick, &m_endTime);
	ATLASSERT(bok);

	// This sort of calculation turns out to be less precise but I don't know yet why
	//TimeFromScreenPos(m_beginTick, pixels);
	//TimeFromScreenPos(m_endTick, m_wndWidth+pixels);

	//BOOL bok=::FileTimeToSystemTime(&m_beginTick, &m_beginTime);
	//bok&=::FileTimeToSystemTime(&m_endTick, &m_endTime);
	//ATLASSERT(bok);

	m_calcTicks=true;
	Invalidate();
}

//-----------------------------------------------------------------------------

void CTimelineCtrl::ScaleTimeline(double scale)
{
	if(scale<=0.0 || m_wndWidth==0)
	{
		ATLASSERT(0); 
		return;
	}

	double rcwidth=double(m_wndWidth);

	Ftint64 t1;
	t1.ft=m_beginTick;
	Ftint64 t2;
	t2.ft=m_endTick;

	__int64 duration=t2.i64-t1.i64; 
	duration/=10000000; // 100ths of nanoseconds to seconds

	REAL ratio; // how many minimum units per pixel
	BarUnits units; // minimum units (seconds or minutes)

	if(m_topUnits==unitsYears && (m_endTime.wYear-m_beginTime.wYear)>1)
	{
		double mins=duration/60.0;
		ratio=rcwidth/mins;	
		units=unitsMinutes;
	}
	else
	{
		double secs=duration;
		ratio=rcwidth/secs;
		units=unitsSeconds;
	}

	int newpixwidth=round_int(rcwidth*scale);
	ATLASSERT(newpixwidth!=m_wndWidth);

	// if zooming in and 1 second is more than a half of client area, do nothing
	if(units==unitsSeconds && newpixwidth<m_wndWidth && ratio>(rcwidth/2))
		return;

	double nm; // pixel diff in seconds or minutes
	if(newpixwidth>m_wndWidth)
		nm=(newpixwidth-rcwidth)/ratio; 
	else
		nm=(rcwidth-newpixwidth)/ratio;

	__int64 nsec=__int64(round_int(nm));
	if(units==unitsMinutes) // convert to seconds
		nsec*=60;

	nsec*=10000000; // to 100ths of nanoseconds
	nsec/=2; // add or subtract half from each side

	if(newpixwidth>m_wndWidth)
	{
		t1.i64-=nsec;
		t2.i64+=nsec;
	}
	else
	{
		t1.i64+=nsec;
		t2.i64-=nsec;
	}

	m_beginTick=t1.ft;
	BOOL ret=::FileTimeToSystemTime(&m_beginTick, &m_beginTime);
	ATLASSERT(ret);

	m_endTick=t2.ft;
	ret=::FileTimeToSystemTime(&m_endTick, &m_endTime);
	ATLASSERT(ret);

	CalcTimeIntervalWidths();
	CalcState();
	m_calcTicks=true;
	Invalidate();
}

//-----------------------------------------------------------------------------

void CTimelineCtrl::Set24HourFormat(bool set) 
{ 
	bool bChange=m_24HourFormat!=set;
	m_24HourFormat=set;
	if(bChange)
	{
		CalcState();	
		m_calcTicks=true;
		Invalidate();
	}
}

//-----------------------------------------------------------------------------

void CTimelineCtrl::SetTimeZone(const TIME_ZONE_INFORMATION& tz)
{
	bool bdiff=memcmp(&m_tz, &tz, sizeof TIME_ZONE_INFORMATION)!=0;
	if(bdiff)
	{
		m_tz=tz;
		BOOL bres=::SystemTimeToTzSpecificLocalTime(&m_tz, &m_beginTimeUTC, &m_beginTime);
		ATLASSERT(bres);
		bres=::SystemTimeToFileTime(&m_beginTime, &m_beginTick);
		ATLASSERT(bres);
		bres=::SystemTimeToTzSpecificLocalTime(&m_tz, &m_endTimeUTC, &m_endTime);
		ATLASSERT(bres);
		bres=::SystemTimeToFileTime(&m_endTime, &m_endTick);
		ATLASSERT(bres);

		CalcState();
		m_calcTicks=true;
		Invalidate();
	}
}

//-----------------------------------------------------------------------------

bool CTimelineCtrl::DisplayMonthCalendar() const
{
	bool display=true; //m_topUnits==unitsYears||m_topUnits==unitsMonths||m_topUnits==unitsDays;
	return display;
}

//-----------------------------------------------------------------------------

void CTimelineCtrl::FormatString(
	const SYSTEMTIME& st, TimeFormat format, TCHAR* str, unsigned len)
{
	TCHAR fmt[64];

	fmt[0]=_T('\0');

	switch(format)
	{
	case formatYears:
		_tcscpy(fmt,_T("yyyy"));
		break;
	case formatMonths:
		_tcscpy(fmt,_T("MMMM"));
		break;
	case formatShortMonths:
	case formatOneLetterMonths:
	case formatQuarterMonths:
		_tcscpy(fmt,_T("MMM"));
		break;
	case formatMonthAndYear:
		_tcscpy(fmt,_T("MMMM yyyy"));
		break;
	case formatDays:
	case formatDays15:
		_tcscpy(fmt,_T("d"));
		break;
	case formatDayMonthYear:
		_tcscpy(fmt,_T("MMMM d', 'yyyy"));
		break;
	case formatDayShortMonthYear:
		_tcscpy(fmt,_T("MMM d', 'yyyy"));
		break;
	case formatHours:
		if(m_24HourFormat)
			_tcscpy(fmt,_T("H"));
		else
			_tcscpy(fmt,_T("h tt"));
		break;
	case formatHourMinute:
		if(m_24HourFormat)
			_tcscpy(fmt,_T("H':'m"));
		else
			_tcscpy(fmt,_T("h':'m tt"));
		break;
	case formatMinutes:
		_tcscpy(fmt,_T("m"));
		break;
	case formatHourMinuteSecond:
		if(m_24HourFormat)
			_tcscpy(fmt,_T("H':'m':'s"));
		else
			_tcscpy(fmt,_T("h':'m':'s tt"));
		break;
	case formatSeconds:
		_tcscpy(fmt,_T("s"));
		break;
	default:
		ATLASSERT(0);
	}

	int ret;

	if(format>formatDayMonthYear) // careful to keep enums in 'natural' order
		ret=::GetTimeFormat(
				LOCALE_USER_DEFAULT,
				0,
				&st,
				fmt,
				str,
				len);
	else
		ret=::GetDateFormat(
				LOCALE_USER_DEFAULT,
				0,
				&st,
				fmt,
				str,
				len);

	ATLASSERT(ret);

	if(format==formatOneLetterMonths || format==formatQuarterMonths)
		str[1]=_T('\0');
}

//-----------------------------------------------------------------------------

void CTimelineCtrl::CalcHotSpots(const SYSTEMTIME& st, TimeFormat format, HotSpots& spots, REAL x)
{
	TCHAR* fmtYear=_T("yyyy");
	TCHAR* fmtMonth=_T("MMMM");
	TCHAR* fmtDay=_T("d");
	TCHAR* fmtHour12=_T("h");
	TCHAR* fmtHour24=_T("H");
	TCHAR* fmtAMPM=_T("tt");
	TCHAR* fmtHour12AMPM=_T("h tt");
	TCHAR* fmtMinute=_T("m");
	TCHAR strYear[10], strMonth[64], strDay[10], strHour[16], strMinute[10], strAMPM[10];
	SizeF sizeYear, sizeMonth, sizeDay, sizeHour, sizeMinute, sizeAMPM;


	// display year and month in all cases
	int ret=::GetDateFormat(
			LOCALE_USER_DEFAULT,
			0,
			&st,
			fmtYear,
			strYear,
			sizeof(strYear)/sizeof(strYear[0]));
	ATLASSERT(ret);
	ret=::GetDateFormat(
			LOCALE_USER_DEFAULT,
			0,
			&st,
			fmtMonth,
			strMonth,
			sizeof(strMonth)/sizeof(strMonth[0]));
	ATLASSERT(ret);

	sizeYear=CAggDC::GetScaledTextExtent(
		strYear, 
		m_topTypeface.c_str(), 
		m_fonts,
		m_topTextHeight);

	sizeMonth=CAggDC::GetScaledTextExtent(
		strMonth, 
		m_topTypeface.c_str(), 
		m_fonts,
		m_topTextHeight);

	spots.month.Width=sizeMonth.Width;
	spots.year.Width=sizeYear.Width;

	spots.st=st;
	spots.fmt=format;

	// N.B. y/Height expected to be set set externally as hotspots form a strip

	switch(format)
	{
	case formatHourMinute:
		{
			ret=::GetTimeFormat(
				LOCALE_USER_DEFAULT,
				0,
				&st,
				m_24HourFormat?fmtHour24:fmtHour12,
				strHour,
				sizeof(strHour)/sizeof(strHour[0]));

			ATLASSERT(ret);
			sizeHour=CAggDC::GetScaledTextExtent(
					strHour, 
					m_topTypeface.c_str(), 
					m_fonts,
					m_topTextHeight);

			ret=::GetTimeFormat(
				LOCALE_USER_DEFAULT,
				0,
				&st,
				fmtMinute,
				strMinute,
				sizeof(strMinute)/sizeof(strMinute[0]));

			ATLASSERT(ret);
			sizeMinute=CAggDC::GetScaledTextExtent(
					strMinute, 
					m_topTypeface.c_str(), 
					m_fonts,
					m_topTextHeight);


			if(!m_24HourFormat)
			{
				ret=::GetTimeFormat(
					LOCALE_USER_DEFAULT,
					0,
					&st,
					fmtAMPM,
					strAMPM,
					sizeof(strAMPM)/sizeof(strAMPM[0]));

				ATLASSERT(ret);
				sizeAMPM=CAggDC::GetScaledTextExtent(
						strAMPM, 
						m_topTypeface.c_str(), 
						m_fonts,
						m_topTextHeight);
			}

			spots.hour.x=x;
			spots.hour.Width=sizeHour.Width;
			spots.minute.x=spots.hour.x+spots.hour.Width+m_commaTopWidth;
			spots.minute.Width=sizeMinute.Width+sizeAMPM.Width;
			x=spots.minute.x+spots.minute.Width+m_commaTopWidth+m_spaceTopWidth;
			goto daymonthyear;
		}
	case formatHours:
		{
			ret=::GetTimeFormat(
				LOCALE_USER_DEFAULT,
				0,
				&st,
				m_24HourFormat?fmtHour24:fmtHour12AMPM,
				strHour,
				sizeof(strHour)/sizeof(strHour[0]));
			ATLASSERT(ret);
			sizeHour=CAggDC::GetScaledTextExtent(
					strHour, 
					m_topTypeface.c_str(), 
					m_fonts,
					m_topTextHeight);

			spots.hour.x=x;
			spots.hour.Width=sizeHour.Width;
			x=spots.hour.x+spots.hour.Width+m_commaTopWidth+m_spaceTopWidth;
		} // fall through
	case formatDayMonthYear:
		{
daymonthyear:
			ret=::GetDateFormat(
					LOCALE_USER_DEFAULT,
					0,
					&st,
					fmtDay,
					strDay,
					sizeof(strDay)/sizeof(strDay[0]));
			ATLASSERT(ret);
			sizeDay=CAggDC::GetScaledTextExtent(
					strDay, 
					m_topTypeface.c_str(), 
					m_fonts,
					m_topTextHeight);
			spots.month.x=x;
			spots.day.x=x+sizeMonth.Width+m_spaceTopWidth;
			spots.day.Width=sizeDay.Width;
			spots.year.x=spots.day.x+sizeDay.Width+m_commaTopWidth+m_spaceTopWidth;
		}
		break;
	case formatMonthAndYear:
		{
			spots.month.x=x;
			spots.year.x=x+sizeMonth.Width+m_spaceTopWidth;
		}
		break;
	default:
		ATLASSERT(0);
		spots.fmt=formatUnknown;
		break;
	};

}

//-----------------------------------------------------------------------------

void CTimelineCtrl::CalcHotSpotsList()
{
	m_hotSpots.clear();

	if(m_topUnits==unitsYears || m_topTickers.empty())
		return;

	HotSpots spots;

	TickerList::iterator itr=m_topTickers.begin();
	while(itr!=m_topTickers.end())
	{
		CalcTextVerticalPosition(spots.month, true);
		CalcTextVerticalPosition(spots.year, true);
		switch(m_topUnits)
		{
		case unitsMonths:
		{
			CalcHotSpots((*itr).st, formatMonthAndYear, spots, (*itr).position);
		}
		break;
		case unitsDays:
		{
			CalcTextVerticalPosition(spots.day, true);
			CalcHotSpots((*itr).st, formatDayMonthYear, spots, (*itr).position);
		}
		break;
		case unitsHours:
		{
			CalcTextVerticalPosition(spots.day, true);
			CalcTextVerticalPosition(spots.hour, true);
			CalcHotSpots((*itr).st, formatHours, spots, (*itr).position);
		}
		break;
		case unitsMinutes:
		{
			CalcTextVerticalPosition(spots.day, true);
			CalcTextVerticalPosition(spots.hour, true);
			CalcTextVerticalPosition(spots.minute, true);
			CalcHotSpots((*itr).st, formatHourMinute, spots, (*itr).position);
		}
		break;
		default:
			ATLASSERT(0);
			break;
		}; // switch

		m_hotSpots.push_back(spots);
		++itr;
	}
}

//-----------------------------------------------------------------------------

bool CTimelineCtrl::IsOverHotspot(const PointF& pt)
{
	bool is=false;
	BarUnits units;

	is=m_leftHotSpots.Intersect(pt, units);
	if(!is && !m_hotSpots.empty())
	{
		HotSpotList::iterator itr=m_hotSpots.begin();
		while(itr!=m_hotSpots.end())
		{
			is=(*itr).Intersect(pt, units);
			if(is)
				break;
			++itr;
		}
	}

	return is;
}

//-----------------------------------------------------------------------------

void CTimelineCtrl::SetTimeRange(const FILETIME& begin, const FILETIME& end)
{
	m_beginTick=begin;
	m_endTick=end;

	BOOL bOk=::FileTimeToSystemTime(&m_beginTick, &m_beginTime);
	bOk&=::FileTimeToSystemTime(&m_endTick, &m_endTime);

	ATLASSERT(bOk);
	CalcTimeIntervalWidths();
	CalcState();
	m_calcTicks=true;
	Invalidate();
}

//-----------------------------------------------------------------------------

void CTimelineCtrl::SetTimeRange(const SYSTEMTIME& begin, const SYSTEMTIME& end)
{
	m_beginTime=begin;
	m_endTime=end;

	BOOL bOk=::SystemTimeToFileTime(&m_beginTime, &m_beginTick);
	bOk&=::SystemTimeToFileTime(&m_endTime, &m_endTick);

	ATLASSERT(bOk);
	CalcTimeIntervalWidths();
	CalcState();
	m_calcTicks=true;
	Invalidate();
}

//-----------------------------------------------------------------------------

void CTimelineCtrl::GetTimeRange(FILETIME& begin, FILETIME& end)
{
	begin=m_beginTick;
	end=m_endTick;
}

//-----------------------------------------------------------------------------

void CTimelineCtrl::GetTimeRange(SYSTEMTIME& begin, SYSTEMTIME& end)
{
	begin=m_beginTime;
	end=m_endTime;
}

//-----------------------------------------------------------------------------

REAL CTimelineCtrl::ScreenPosFromTime(const FILETIME& time) const 
{
	double pos=0;

	// formula is: pos = width*(t-t1)/(t2-t1)

	Ftint64 t1;
	t1.ft=m_beginTick;
	Ftint64 t2;
	t2.ft=m_endTick;
	Ftint64 t;
	t.ft=time;

	__int64 t2_t1=t2.i64-t1.i64;
	if(t2_t1!=0)
	{
		__int64 t_t1=t.i64-t1.i64;
		pos=double(m_wndWidth)*(double(t_t1)/double(t2_t1));
	}

	return pos;
}

//-----------------------------------------------------------------------------

REAL CTimelineCtrl::ScreenSizeFromTimeInterval(const __int64& interval) const
{
	double size=0;

	// formula is: size = width*interval/(t2-t1)

	Ftint64 t1;
	t1.ft=m_beginTick;
	Ftint64 t2;
	t2.ft=m_endTick;

	__int64 t2_t1=t2.i64-t1.i64;
	if(t2_t1!=0)
		size=double(m_wndWidth)*(double(interval)/double(t2_t1));

	return size;
}

//-----------------------------------------------------------------------------

void CTimelineCtrl::TimeFromScreenPos(FILETIME& time, int pos) const
{
	int width=m_wndWidth;

	// formula is: t = (t2-t1/width)*pos+t1;

	Ftint64 t1;
	t1.ft=m_beginTick;
	Ftint64 t2;
	t2.ft=m_endTick;
	Ftint64 t;

	if(width==0)
	{
		t=t2;
	}
	else
	{
		__int64 t2_t1=t2.i64-t1.i64;
		// TODO - how to make sure this multiplication/division doesn't go bad?
		t.i64=((t2_t1)*pos)/width + t1.i64;
	}

	time=t.ft;
}

//-----------------------------------------------------------------------------

int CTimelineCtrl::DayOfYear(const SYSTEMTIME& st)
{
	if(IsLeapYear(st.wYear))
		m_daysinamonth[1]=29;
	else
		m_daysinamonth[1]=28;

	int days=st.wDay;
	for(unsigned i=1; i<st.wMonth; ++i)
		days+=m_daysinamonth[i-1];
	return days;
}

//-----------------------------------------------------------------------------

void CTimelineCtrl::NextMonth(SYSTEMTIME& syst)
{
	syst.wDay=1;
	syst.wHour=0;
	syst.wMinute=0;
	syst.wSecond=0;
	syst.wMilliseconds=0;
	IncrementMonth(syst);
}

//-----------------------------------------------------------------------------

void CTimelineCtrl::IncrementMonth(SYSTEMTIME& syst)
{
	syst.wMonth++;
	if(syst.wMonth>12)
	{
		syst.wYear++;
		syst.wMonth=1;
	}
}

//-----------------------------------------------------------------------------

void CTimelineCtrl::DecrementMonth(SYSTEMTIME& syst)
{
	syst.wMonth--;
	if(syst.wMonth==0)
	{
		syst.wMonth=12;
		syst.wYear--;
	}
}

//-----------------------------------------------------------------------------

void CTimelineCtrl::NextDay(SYSTEMTIME& syst)
{
	syst.wHour=0; 
	syst.wMinute=0; 
	syst.wSecond=0; 
	syst.wMilliseconds=0; 
	IncrementDay(syst);
}

//-----------------------------------------------------------------------------

void CTimelineCtrl::IncrementDay(SYSTEMTIME& syst)
{
	Ftint64 fti;
	BOOL ok=::SystemTimeToFileTime(&syst, &fti.ft);
	ATLASSERT(ok);
	fti.i64+=36000000000i64*24; // 1 day in 100 nsec units
	ok=::FileTimeToSystemTime(&fti.ft, &syst);
	ATLASSERT(ok);
}

//-----------------------------------------------------------------------------

void CTimelineCtrl::DecrementDay(SYSTEMTIME& syst)
{
	Ftint64 fti;
	BOOL ok=::SystemTimeToFileTime(&syst, &fti.ft);
	ATLASSERT(ok);
	fti.i64-=36000000000i64*24; // 1 day in 100 nsec units
	ok=::FileTimeToSystemTime(&fti.ft, &syst);
	ATLASSERT(ok);
}

//-----------------------------------------------------------------------------

void CTimelineCtrl::NextHour(SYSTEMTIME& syst)
{
	syst.wMinute=0; 
	syst.wSecond=0; 
	syst.wMilliseconds=0; 
	IncrementHour(syst);
}

//-----------------------------------------------------------------------------

void CTimelineCtrl::IncrementHour(SYSTEMTIME& syst)
{
	Ftint64 fti;
	BOOL ok=::SystemTimeToFileTime(&syst, &fti.ft);
	ATLASSERT(ok);
	fti.i64+=36000000000i64; // 1 hr in 100 nsec units
	ok=::FileTimeToSystemTime(&fti.ft, &syst);
	ATLASSERT(ok);
}

//-----------------------------------------------------------------------------

void CTimelineCtrl::DecrementHour(SYSTEMTIME& syst)
{
	Ftint64 fti;
	BOOL ok=::SystemTimeToFileTime(&syst, &fti.ft);
	ATLASSERT(ok);
	fti.i64-=36000000000i64; // 1 hour in 100 nsec units
	ok=::FileTimeToSystemTime(&fti.ft, &syst);
	ATLASSERT(ok);
}

//-----------------------------------------------------------------------------

void CTimelineCtrl::NextMinute(SYSTEMTIME& syst)
{
	syst.wSecond=0; 
	syst.wMilliseconds=0; 
	IncrementMinute(syst);
}

//-----------------------------------------------------------------------------

void CTimelineCtrl::IncrementMinute(SYSTEMTIME& syst)
{
	Ftint64 fti;
	BOOL ok=::SystemTimeToFileTime(&syst, &fti.ft);
	ATLASSERT(ok);
	fti.i64+=600000000i64; // 1 min in 100 nsec units
	ok=::FileTimeToSystemTime(&fti.ft, &syst);
	ATLASSERT(ok);
}

//-----------------------------------------------------------------------------

void CTimelineCtrl::DecrementMinute(SYSTEMTIME& syst)
{
	Ftint64 fti;
	BOOL ok=::SystemTimeToFileTime(&syst, &fti.ft);
	ATLASSERT(ok);
	fti.i64-=600000000i64; // 1 min in 100 nsec units
	ok=::FileTimeToSystemTime(&fti.ft, &syst);
	ATLASSERT(ok);
}

//-----------------------------------------------------------------------------

void CTimelineCtrl::NextSecond(SYSTEMTIME& syst)
{
	syst.wMilliseconds=0; 
	IncrementSecond(syst);
}

//-----------------------------------------------------------------------------

void CTimelineCtrl::IncrementSecond(SYSTEMTIME& syst)
{
	Ftint64 fti;
	BOOL ok=::SystemTimeToFileTime(&syst, &fti.ft);
	ATLASSERT(ok);
	fti.i64+=10000000; // 1 sec in 100 nsec units
	ok=::FileTimeToSystemTime(&fti.ft, &syst);
	ATLASSERT(ok);
}

//-----------------------------------------------------------------------------

void CTimelineCtrl::DecrementSecond(SYSTEMTIME& syst)
{
	Ftint64 fti;
	BOOL ok=::SystemTimeToFileTime(&syst, &fti.ft);
	ATLASSERT(ok);
	fti.i64-=10000000; // 1 sec in 100 nsec units
	ok=::FileTimeToSystemTime(&fti.ft, &syst);
	ATLASSERT(ok);
}

//-----------------------------------------------------------------------------

void CTimelineCtrl::CalcTimeIntervalWidths()
{
	__int64 second=10000000;
	m_secondWidth=ScreenSizeFromTimeInterval(second);
	m_minuteWidth=m_secondWidth*60;
	m_hourWidth=m_minuteWidth*60;
	m_dayWidth=m_hourWidth*24;

	// ALternative calculations for precision tests
	//__int64 minute=600000000;
	//m_minuteWidth=ScreenSizeFromTimeInterval(minute);

	//__int64 hour=36000000000i64;
	//m_hourWidth=ScreenSizeFromTimeInterval(hour);

	//__int64 day=36000000000i64*24;
	//m_dayWidth=ScreenSizeFromTimeInterval(day);
}

//-----------------------------------------------------------------------------

void CTimelineCtrl::CalcTicks(void)
{
	REAL width=REAL(m_wndWidth);
	SYSTEMTIME syst=m_beginTime;
	FILETIME ft;

	m_topTickers.clear();
	m_bottomTickers.clear();

	REAL nextpos, distance;

	if(m_topUnits==unitsYears || m_bottomUnits==unitsYears)
	{
		// start with the tick for the next year

		syst.wYear+=1;
		syst.wMonth=1;
		syst.wDay=1;
		syst.wHour=0; 
		syst.wMinute=0; 
		syst.wSecond=0; 
		syst.wMilliseconds=0; 

		BOOL bOk=::SystemTimeToFileTime(&syst, &ft);
		ATLASSERT(bOk);

		nextpos=ScreenPosFromTime(ft); 

		Ticker tick={syst.wYear, nextpos, syst};

		while(nextpos<width)
		{
			tick.value=syst.wYear;
			tick.position=nextpos;
			tick.st=syst;

			if(m_topUnits==unitsYears)
				m_topTickers.push_back(tick);
			if(m_bottomUnits==unitsYears)
				m_bottomTickers.push_back(tick);

			syst.wYear++;
			bOk=::SystemTimeToFileTime(&syst, &ft);
			ATLASSERT(bOk);
			nextpos=ScreenPosFromTime(ft); 
		}
	}

	if(m_topUnits==unitsMonths || m_bottomUnits==unitsMonths)
	{
		syst=m_beginTime;
		NextMonth(syst);
		BOOL bOk=::SystemTimeToFileTime(&syst, &ft);
		ATLASSERT(bOk);

		nextpos=ScreenPosFromTime(ft); 
		Ticker tick={syst.wMonth, nextpos, syst};

		while(nextpos<width)
		{
			if(m_topUnits==unitsMonths)
				m_topTickers.push_back(tick);
			if(m_bottomUnits==unitsMonths)
				m_bottomTickers.push_back(tick);

			if(IsLeapYear(syst.wYear))
				m_daysinamonth[1]=29;
			else
				m_daysinamonth[1]=28;

			__int64 totdays=36000000000i64*24*m_daysinamonth[syst.wMonth-1];
			distance=ScreenSizeFromTimeInterval(totdays);

			nextpos+=distance;
			IncrementMonth(syst);
			tick.value=syst.wMonth;
			tick.position=nextpos;
			tick.st=syst;
		}
	}

	if(m_topUnits==unitsDays || m_bottomUnits==unitsDays)
	{
		syst=m_beginTime;
		NextDay(syst);
		BOOL bOk=::SystemTimeToFileTime(&syst, &ft);
		ATLASSERT(bOk);

		nextpos=ScreenPosFromTime(ft); 

		Ticker tick={syst.wDay, nextpos, syst};

		distance=m_dayWidth;

		while(nextpos<width)
		{
			if(m_topUnits==unitsDays)
				m_topTickers.push_back(tick);
			if(m_bottomUnits==unitsDays)
				m_bottomTickers.push_back(tick);

			nextpos+=distance;
			IncrementDay(syst);
			tick.value=syst.wDay;
			tick.position=nextpos;
			tick.st=syst;
		}
	}

	if(m_topUnits==unitsHours || m_bottomUnits==unitsHours)
	{
		syst=m_beginTime;
		NextHour(syst);
		BOOL bOk=::SystemTimeToFileTime(&syst, &ft);
		ATLASSERT(bOk);

		nextpos=ScreenPosFromTime(ft); 

		Ticker tick={syst.wHour, nextpos, syst};

		distance=m_hourWidth;

		while(nextpos<width)
		{
			if(m_topUnits==unitsHours)
				m_topTickers.push_back(tick);
			if(m_bottomUnits==unitsHours)
				m_bottomTickers.push_back(tick);

			nextpos+=distance;
			IncrementHour(syst);
			tick.value=syst.wHour;
			tick.position=nextpos;
			tick.st=syst;
		}
	}

	if(m_topUnits==unitsMinutes || m_bottomUnits==unitsMinutes)
	{
		syst=m_beginTime;
		NextMinute(syst);
		BOOL bOk=::SystemTimeToFileTime(&syst, &ft);
		ATLASSERT(bOk);

		nextpos=ScreenPosFromTime(ft); 

		Ticker tick={syst.wMinute, nextpos, syst};

		distance=m_minuteWidth;

		while(nextpos<width)
		{
			if(m_topUnits==unitsMinutes)
				m_topTickers.push_back(tick);
			if(m_bottomUnits==unitsMinutes)
				m_bottomTickers.push_back(tick);

			nextpos+=distance;
			IncrementMinute(syst);
			tick.value=syst.wMinute;
			tick.position=nextpos;
			tick.st=syst;
		}
	}

	if(m_bottomUnits==unitsSeconds)
	{
		syst=m_beginTime;
		NextSecond(syst);
		BOOL bOk=::SystemTimeToFileTime(&syst, &ft);
		ATLASSERT(bOk);

		nextpos=ScreenPosFromTime(ft); 

		Ticker tick={syst.wSecond, nextpos, syst};

		distance=m_secondWidth;

		while(nextpos<width)
		{
			m_bottomTickers.push_back(tick);

			nextpos+=distance;
			IncrementSecond(syst);
			tick.value=syst.wSecond;
			tick.position=nextpos;
		}
	}
}

//-----------------------------------------------------------------------------

int CTimelineCtrl::CurrentModuloDivisor(bool topbar) const
{
	int modulo=1;
	BarUnits units=topbar?m_topUnits:m_bottomUnits;
	switch(units)
	{
	case unitsYears:
		modulo=m_yearModulo;
		break;
	case unitsMonths:
		if(!topbar)
			modulo=m_monthModulo;
		break;
	case unitsDays:
		break;
	case unitsHours:
		if(!topbar)
			modulo=m_hourModulo;
		break;
	case unitsMinutes:
		if(!topbar)
			modulo=m_minModulo;
		break;
	case unitsSeconds:
		if(!topbar)
			modulo=m_secModulo;
		break;
	default:
		ATLASSERT(0);
		break;
	}
	return modulo;
}

//-----------------------------------------------------------------------------

bool CTimelineCtrl::IsValueVisible(unsigned value, bool topbar) const
{
	bool itis=true;
	if(!topbar && m_bottomUnits==unitsDays && m_bottomFormat==formatDays15)
	{
		itis=value==1 || value==15;
	}
	else
	{
		if(!topbar && m_bottomUnits==unitsMonths)
			value-=1;
		int modulo=CurrentModuloDivisor(topbar);
		if((value % modulo)!=0)
			itis=false;
	}
	return itis;
}

//-----------------------------------------------------------------------------

REAL CTimelineCtrl::FirstVisiblePosition(bool topbar)
{
	TickerList::iterator itr=topbar?m_topTickers.begin():m_bottomTickers.begin();
	TickerList::iterator itrend=topbar?m_topTickers.end():m_bottomTickers.end();

	REAL pos=0;

	while(itr!=itrend)
	{
		if(IsValueVisible((*itr).value, topbar))
		{
			pos=(*itr).position;
			break;
		}
		++itr;
	}
	return pos;
}

//-----------------------------------------------------------------------------

RectF CTimelineCtrl::DrawLeftmost(
	CAggDC& aggdc, const TCHAR* text, REAL rborder, bool topbar, bool leftAlign)
{
	const char* typeface=topbar?m_topTypeface.c_str():m_bottomTypeface.c_str();
	int fontHeight=topbar?m_topTextHeight:m_bottomTextHeight;

	SizeF size=CAggDC::GetScaledTextExtent(
		text, 
		typeface, 
		m_fonts,
		fontHeight);

	RectF rc;
	rc.Width=size.Width;

	if(leftAlign)
	{
		if(rc.Width+(topbar?m_topLeftMargin:m_bottomLeftMargin) > rborder)
		{
			if(topbar)
				rc.x=rborder-rc.Width;
			else
				rc.x=-rc.Width; // make it invisible
		}
		else
		{
			rc.x=topbar?m_topLeftMargin:m_bottomLeftMargin;
		}
	}
	else
	{
		ATLASSERT(0);
	}

	CalcTextVerticalPosition(rc, topbar);

	aggdc.TransformPush();

	PointF descsubtract;

	aggdc.DrawScaledText(
		text,
		rc,
		m_dividerColor,
		typeface,
		fontHeight,
		descsubtract);

	aggdc.TransformPop();

	return rc;
}

//-----------------------------------------------------------------------------
//
// Criteria for alternating colors can not be based only on even/odd values
// because values can be hidden when we merge time intervals to fit the text.
// For ex. displaying every 10 minute intervals all visible minute values will be
// even values and we won't get consistent alternating colors when scrolling.
// Thus this function must do modulo division which tells us how many values
// are merged into one visible interval

bool CTimelineCtrl::AlternatingColor(TimeFormat format, unsigned value, bool topbar)
{
	bool alternate=(value%2)!=0; // this is default

	//enum TimeFormat
	//{
	//	formatYears,
	//	formatMonths,
	//	formatShortMonths,
	//	formatOneLetterMonths,
	//	formatQuarterMonths,
	//	formatMonthAndYear,
	//	formatDays,
	//  formatDays15,
	//	formatDayMonthYear
	//	formatDayShortMonthYear,
	//	formatHours,
	//	formatHourMinute,
	//	formatMinutes,
	//	formatHourMinuteSecond,
	//	formatSeconds
	//};

	switch(format)
	{
	case formatYears:
		value/=m_yearModulo;
		alternate=(value%2)!=0; 
		break;
	case formatMonths:
	case formatShortMonths:
	case formatOneLetterMonths:
	case formatQuarterMonths:
		break;
	case formatMonthAndYear:
		break;
	case formatDays:
		break;
	case formatDays15: // only the 1st and 15th days in month are drawn
		alternate=value==1;
		break;
	case formatDayMonthYear:
	case formatDayShortMonthYear:
		break;
	case formatHours:
		if(!topbar) // when hours are displayed in top bar, modulo is implicitly 1
		{
			value/=m_hourModulo;
			alternate=(value%2)!=0; 
		}
		break;
	case formatHourMinute:
		break;
	case formatMinutes:
		if(!topbar)
		{
			value/=m_minModulo;
			alternate=(value%2)!=0; 
		}
		break;
	case formatHourMinuteSecond:
		break;
	case formatSeconds:
		if(!topbar)
		{
			value/=m_secModulo;
			alternate=(value%2)!=0; 
		}
		break;
	default:
		ATLASSERT(0);
		break;
	};

	return alternate;
}

//-----------------------------------------------------------------------------

void CTimelineCtrl::DrawBackground(CAggDC& aggdc, bool topbar)
{
	bool balternate=false;
	bool barnoticks=topbar?m_topTickers.empty():m_bottomTickers.empty();
	RectF rcbk(0.0, 0.0, 0.0, m_topBarHeight);
	BarUnits units=topbar?m_topUnits:m_bottomUnits;
	TimeFormat format=topbar?m_topFormat:m_bottomFormat;

	if(!topbar)
	{
		rcbk.y=m_topBarHeight+1;
		rcbk.Height=m_bottomBarHeight;
	}

	unsigned value;

	if(barnoticks)
	{
		switch(units)
		{
		case unitsYears:
			value=m_beginTime.wYear;
			break;
		case unitsMonths:
			value=m_beginTime.wMonth;
			break;
		case unitsDays:
			value=DayOfYear(m_beginTime);
			break;
		case unitsHours:
			value=m_beginTime.wHour;
			break;
		case unitsMinutes:
			value=m_beginTime.wMinute;
			break;
		case unitsSeconds:
			value=m_beginTime.wSecond;
			break;
		default:
			ATLASSERT(0);
			break;
		}

		// TODO when topmost units are 10s of years this seems wrong
		balternate=AlternatingColor(format, value, topbar);
		rcbk.Width=m_wndWidth;
		aggdc.LinearGradientRect(rcbk, balternate?m_lightBgrColor:m_darkBgrColor, Color::White);
	}
	else
	{
		TickerList::iterator itr=topbar?m_topTickers.begin():m_bottomTickers.begin();
		TickerList::iterator itrend=topbar?m_topTickers.end():m_bottomTickers.end();

		if(IsValueVisible((*itr).value, topbar))
		{
			if(units==unitsDays && format!=formatDays15)
				value=DayOfYear((*itr).st);
			else
				value=(*itr).value;
			balternate=!AlternatingColor(format, value, topbar);
		}
		else
		{
			TickerList::iterator fitr=itr;
			while(fitr!=itrend)
			{
				if(IsValueVisible((*fitr).value, topbar))
				{
					if(units==unitsDays && format!=formatDays15)
						value=DayOfYear((*fitr).st);
					else
						value=(*fitr).value;
					balternate=!AlternatingColor(format, value, topbar);
					break;
				}
				++fitr;
			}
		}

		// once we know start value for color we alternate the rest
		while(itr!=itrend)
		{
			if(IsValueVisible((*itr).value, topbar))
			{
				rcbk.Width=(*itr).position-rcbk.x;
				aggdc.LinearGradientRect(rcbk, balternate?m_lightBgrColor:m_darkBgrColor, Color::White);
				rcbk.x=(*itr).position;
				balternate=!balternate;
			}
			++itr;
		}

		rcbk.Width=m_wndWidth-rcbk.x;
		aggdc.LinearGradientRect(rcbk, balternate?m_lightBgrColor:m_darkBgrColor, Color::White);
	}
}

//-----------------------------------------------------------------------------

bool CTimelineCtrl::FitSeconds()
{
	m_bottomFormat=formatSeconds;
	m_secModulo=1;

	// decide if we can fit seconds over or between tickers
	int modulos[5]={1,5,10,15,30};
	bool fit=false;
	int i=0; // current modulo index

	for(; i<(sizeof modulos/sizeof modulos[0]); ++i)
	{
		if((m_secsBottomMaxWidth+m_fittingSlack)<(m_secondWidth*modulos[i])) 
		{ 
			fit=true;
			break;
		}
	} 

	if(fit)
	{
		m_secModulo=modulos[i];
	}

	return fit;
}

//-----------------------------------------------------------------------------

void CTimelineCtrl::DrawSeconds(CAggDC& aggdc)
{
	ATLASSERT(m_bottomUnits==unitsSeconds);

	REAL barheight=REAL(Height());

	PointF p1, p2, p1m, p2m;

	p1.x=p2.x=0;
	p1.y=barheight;
	p2.y=barheight-m_tick1Height; 

	p1m.x=p2m.x=0;
	p1m.y=barheight;
	p2m.y=barheight-m_tick2Height; 

	SizeF size;
	RectF rct;
	PointF descsubtract;

	TickerList::iterator itr=m_bottomTickers.begin();
	TickerList::iterator itrend=m_bottomTickers.end();

	TCHAR strbuff[128];

	FormatString(m_beginTime, formatSeconds, strbuff, sizeof strbuff/sizeof strbuff[0]);

	REAL pos=FirstVisiblePosition(false);
	DrawLeftmost(aggdc, strbuff, pos, false, true);

	while(itr!=itrend)
	{
		if(IsValueVisible((*itr).value, false))
		{
			::_itot((*itr).value, strbuff, 10);

			size=CAggDC::GetScaledTextExtent(
				strbuff, 
				m_bottomTypeface.c_str(), 
				m_fonts,
				m_bottomTextHeight);

			rct.Width=size.Width;
			rct.x=(*itr).position+m_bottomLeftMargin;
			CalcTextVerticalPosition(rct, false);

			aggdc.TransformPush();

			aggdc.DrawScaledText(
				strbuff, 
				rct,
				m_dividerColor,
				m_bottomTypeface.c_str(),
				m_bottomTextHeight,
				descsubtract);

			aggdc.TransformPop();

			if(m_showTickMarks)
			{
				p1.x=(*itr).position;
				p2.x=p1.x;
				aggdc.Line(p1, p2, m_dividerColor);
			}

			if(m_showHalfTickMarks)
			{
				if(m_secModulo==1)
				{
					REAL half=m_secondWidth/2;
					if(half>=m_halfTickSpacing)
					{
						p1m.x=(*itr).position+half;
						p2m.x=p1m.x;
						aggdc.Line(p1m, p2m, m_dividerColor);
					}
				}
				else
				{
					if(m_secondWidth>=m_halfTickSpacing)
					{
						p1m.x=(*itr).position+m_secondWidth;
						p2m.x=p1m.x;
						for(int i=0; i<m_secModulo-1; ++i)
						{
							aggdc.Line(p1m, p2m, m_dividerColor);
							p1m.x+=m_secondWidth;
							p2m.x=p1m.x;
						}
					}
				}
			}
		}

		++itr;
	}
}

//-----------------------------------------------------------------------------

bool CTimelineCtrl::FitMinutes(bool topbar)
{
	TimeFormat* fmt=topbar?&m_topFormat:&m_bottomFormat;
	*fmt=formatMinutes;
	m_minModulo=1;

	// decide if we can fit minutes over or between tickers
	int modulos[5]={1,5,10,15,30};
	bool fit=false;

	int i=0; // current modulo index (used belo so keep out of loop)

	for(; i<(sizeof modulos/sizeof modulos[0]); ++i)
	{
		if((m_minsBottomMaxWidth+m_fittingSlack)<(m_minuteWidth*modulos[i])) 
		{
			fit=true;
			break;
		}
	} 

	if(fit)
	{
		m_minModulo=modulos[i];
	}

	return fit;
}

//-----------------------------------------------------------------------------

void CTimelineCtrl::DrawMinutes(CAggDC& aggdc, bool topbar)
{
	ATLASSERT(topbar?m_topUnits==unitsMinutes:m_bottomUnits==unitsMinutes);

	REAL barheight=REAL(Height());

	PointF p1, p2, p1m, p2m;

	if(!topbar)
	{
		p1.x=p2.x=0;
		p1.y=barheight;
		p2.y=barheight-m_tick1Height; 

		p1m.x=p2m.x=0;
		p1m.y=barheight;
		p2m.y=barheight-m_tick2Height; 
	}

	SizeF size;
	RectF rct;
	PointF descsubtract;

	TCHAR datestr[128];
	TCHAR strbuff[128];
	SYSTEMTIME st=m_beginTime;

	if(topbar) 
	{
		FormatString(st, formatHourMinute, strbuff, sizeof strbuff/sizeof strbuff[0]);
		_tcscat(strbuff, _T(", "));
		FormatString(st, formatDayMonthYear, datestr, sizeof datestr/sizeof datestr[0]);
		_tcscat(strbuff, datestr);
	}
	else
	{
		FormatString(st, formatMinutes, strbuff, sizeof strbuff/sizeof strbuff[0]);
	}

	unsigned count=topbar?m_topTickers.size():m_bottomTickers.size();
	if(count==0)
	{
		if(topbar)
		{
			CalcTextVerticalPosition(m_leftHotSpots.minute, true);
			CalcTextVerticalPosition(m_leftHotSpots.hour, true);
			CalcTextVerticalPosition(m_leftHotSpots.day, true);
			CalcTextVerticalPosition(m_leftHotSpots.month, true);
			CalcTextVerticalPosition(m_leftHotSpots.year, true);
			CalcHotSpots(m_beginTime, formatHourMinute, m_leftHotSpots, m_topLeftMargin);
		}

		size=CAggDC::GetScaledTextExtent(
			strbuff, 
			topbar?m_topTypeface.c_str():m_bottomTypeface.c_str(), 
			m_fonts,
			topbar?m_topTextHeight:m_bottomTextHeight);

		DrawLeftmost(aggdc, strbuff, size.Width+m_topLeftMargin, topbar, true);
		return;
	}

	REAL pos=FirstVisiblePosition(topbar);
	RectF rcleft=DrawLeftmost(aggdc, strbuff, pos, topbar, true);

	if(topbar)
	{
		CalcTextVerticalPosition(m_leftHotSpots.minute, true);
		CalcTextVerticalPosition(m_leftHotSpots.hour, true);
		CalcTextVerticalPosition(m_leftHotSpots.day, true);
		CalcTextVerticalPosition(m_leftHotSpots.month, true);
		CalcTextVerticalPosition(m_leftHotSpots.year, true);
		CalcHotSpots(m_beginTime, formatHourMinute, m_leftHotSpots, rcleft.x);
	}

	TickerList::iterator itr=topbar?m_topTickers.begin():m_bottomTickers.begin();
	TickerList::iterator itrend=topbar?m_topTickers.end():m_bottomTickers.end();

	while(itr!=itrend)
	{
		if(IsValueVisible((*itr).value, topbar))
		{
			if(topbar)
			{
				FormatString((*itr).st, formatHourMinute, strbuff, sizeof strbuff/sizeof strbuff[0]);
				_tcscat(strbuff, _T(", "));
				FormatString((*itr).st, formatDayMonthYear, datestr, sizeof datestr/sizeof datestr[0]);
				_tcscat(strbuff, datestr);
			}
			else
			{
				::_itot((*itr).value, strbuff, 10);
			}

			size=CAggDC::GetScaledTextExtent(
				strbuff, 
				topbar?m_topTypeface.c_str():m_bottomTypeface.c_str(), 
				m_fonts,
				topbar?m_topTextHeight:m_bottomTextHeight);

			rct.Width=size.Width;
			rct.x=(*itr).position+(topbar?m_topLeftMargin:m_bottomLeftMargin);
			CalcTextVerticalPosition(rct, topbar);

			aggdc.TransformPush();

			aggdc.DrawScaledText(
				strbuff, 
				rct,
				m_dividerColor,
				topbar?m_topTypeface.c_str():m_bottomTypeface.c_str(),
				topbar?m_topTextHeight:m_bottomTextHeight,
				descsubtract);

			aggdc.TransformPop();

			if(!topbar && m_showTickMarks)
			{
				p1.x=(*itr).position;
				p2.x=p1.x;
				aggdc.Line(p1, p2, m_dividerColor);
			}

			if(!topbar && m_showHalfTickMarks)
			{
				if(m_minModulo==1)
				{
					REAL half=m_minuteWidth/2;
					if(half>=m_halfTickSpacing)
					{
						p1m.x=(*itr).position+half;
						p2m.x=p1m.x;
						aggdc.Line(p1m, p2m, m_dividerColor);
					}
				}
				else
				{
					if(m_minuteWidth>=m_halfTickSpacing)
					{
						p1m.x=(*itr).position+m_minuteWidth;
						p2m.x=p1m.x;
						for(int i=0; i<m_minModulo-1; ++i)
						{
							aggdc.Line(p1m, p2m, m_dividerColor);
							p1m.x+=m_minuteWidth;
							p2m.x=p1m.x;
						}
					}
				}
			}
		}

		++itr;
	}
}

//-----------------------------------------------------------------------------

bool CTimelineCtrl::FitHours(bool topbar)
{
	TimeFormat* fmt=topbar?&m_topFormat:&m_bottomFormat;
	*fmt=formatHours;
	m_hourModulo=1;

	REAL width=m_hoursBottomMaxWidth;
	if(!m_24HourFormat)
		width+=m_spaceBottomWidth+m_hourSymbolBottomWidth;

	// decide if we can fit strings over or between the tickers
	int modulos[4]={1,3,6,12};
	bool fit=false;

	int i=0; // current modulo index

	for(; i<(sizeof modulos/sizeof modulos[0]); ++i)
	{
		if((width+m_fittingSlack)<(m_hourWidth*modulos[i])) 
		{
			fit=true;
			break;
		}
	} 

	if(fit)
	{
		m_hourModulo=modulos[i];
	}

	return fit;
}

//-----------------------------------------------------------------------------

void CTimelineCtrl::DrawHours(CAggDC& aggdc, bool topbar)
{
	ATLASSERT(topbar?m_topUnits==unitsHours:m_bottomUnits==unitsHours);

	REAL barheight=REAL(Height());
	PointF p1, p2, p1m, p2m;

	if(!topbar)
	{
		p1.x=p2.x=0;
		p1.y=barheight;
		p2.y=barheight-m_tick1Height; 

		p1m.x=p2m.x=0;
		p1m.y=barheight;
		p2m.y=barheight-m_tick2Height; 
	}

	SizeF size;
	RectF rct;
	PointF descsubtract;
	TCHAR strbuff[128];
	TCHAR datestr[128];

	TickerList::iterator itr=topbar?m_topTickers.begin():m_bottomTickers.begin();
	TickerList::iterator itrend=topbar?m_topTickers.end():m_bottomTickers.end();

	SYSTEMTIME st=m_beginTime;

	FormatString(st, formatHours, strbuff, sizeof strbuff/sizeof strbuff[0]);

	if(topbar) 
	{
		_tcscat(strbuff, _T(", "));
		FormatString(st, formatDayMonthYear, datestr, sizeof datestr/sizeof datestr[0]);
		_tcscat(strbuff, datestr);
	}

	unsigned count=topbar?m_topTickers.size():m_bottomTickers.size();
	if(count==0)
	{
		if(topbar)
		{
			CalcTextVerticalPosition(m_leftHotSpots.hour, true);
			CalcTextVerticalPosition(m_leftHotSpots.day, true);
			CalcTextVerticalPosition(m_leftHotSpots.month, true);
			CalcTextVerticalPosition(m_leftHotSpots.year, true);
			CalcHotSpots(m_beginTime, formatHours, m_leftHotSpots, m_topLeftMargin);
		}

		size=CAggDC::GetScaledTextExtent(
			strbuff, 
			topbar?m_topTypeface.c_str():m_bottomTypeface.c_str(), 
			m_fonts,
			topbar?m_topTextHeight:m_bottomTextHeight);

		DrawLeftmost(aggdc, strbuff, size.Width+m_topLeftMargin, topbar, true);

		return;
	}

	REAL pos=FirstVisiblePosition(topbar);
	RectF rcleft=DrawLeftmost(aggdc, strbuff, pos, topbar, true);

	if(topbar)
	{
		CalcTextVerticalPosition(m_leftHotSpots.hour, true);
		CalcTextVerticalPosition(m_leftHotSpots.day, true);
		CalcTextVerticalPosition(m_leftHotSpots.month, true);
		CalcTextVerticalPosition(m_leftHotSpots.year, true);
		CalcHotSpots(m_beginTime, formatHours, m_leftHotSpots, rcleft.x);
	}

	while(itr!=itrend)
	{
		if(IsValueVisible((*itr).value, topbar))
		{
			FormatString((*itr).st, formatHours, strbuff, sizeof strbuff/sizeof strbuff[0]);

			if(topbar)
			{
				_tcscat(strbuff, _T(", "));
				FormatString((*itr).st, formatDayMonthYear, datestr, sizeof datestr/sizeof datestr[0]);
				_tcscat(strbuff, datestr);
			}

			size=CAggDC::GetScaledTextExtent(
				strbuff, 
				topbar?m_topTypeface.c_str():m_bottomTypeface.c_str(), 
				m_fonts,
				topbar?m_topTextHeight:m_bottomTextHeight);

			rct.Width=size.Width;
			rct.x=(*itr).position+(topbar?m_topLeftMargin:m_bottomLeftMargin);
			CalcTextVerticalPosition(rct, topbar);

			aggdc.TransformPush();

			aggdc.DrawScaledText(
				strbuff, 
				rct,
				m_dividerColor,
				topbar?m_topTypeface.c_str():m_bottomTypeface.c_str(),
				topbar?m_topTextHeight:m_bottomTextHeight,
				descsubtract);

			aggdc.TransformPop();

			if(!topbar && m_showTickMarks)
			{
				p1.x=(*itr).position;
				p2.x=p1.x;
				aggdc.Line(p1, p2, m_dividerColor);
			}

			if(!topbar && m_showHalfTickMarks)
			{
				if(m_hourModulo==1)
				{
					REAL half=m_hourWidth/2;
					if(half>=m_halfTickSpacing)
					{
						p1m.x=(*itr).position+half;
						p2m.x=p1m.x;
						aggdc.Line(p1m, p2m, m_dividerColor);
					}
				}
				else
				{
					if(m_hourWidth>=m_halfTickSpacing)
					{
						p1m.x=(*itr).position+m_hourWidth;
						p2m.x=p1m.x;
						for(int i=0; i<m_hourModulo-1; ++i)
						{
							aggdc.Line(p1m, p2m, m_dividerColor);
							p1m.x+=m_hourWidth;
							p2m.x=p1m.x;
						}
					}
				}
			}
		}

		++itr;
	}
}

//-----------------------------------------------------------------------------

bool CTimelineCtrl::FitDays(bool topbar)
{
	TimeFormat* fmt=topbar?&m_topFormat:&m_bottomFormat;
	*fmt=formatDays;

	// decide if we can fit day string over or between the tickers
	int modulos[2]={1,15};
	bool fit=false;

	int i=0; // current modulo index
	for(; i<(sizeof modulos/sizeof modulos[0]); ++i)
	{
		if((m_daysBottomMaxWidth+m_fittingSlack)<(m_dayWidth*modulos[i])) 
		{
			fit=true;
			break;
		}
	} 

	if(fit && i==1)
	{
		*fmt=formatDays15;
	}

	return fit;
}

//-----------------------------------------------------------------------------

void CTimelineCtrl::DrawDays(CAggDC& aggdc, bool topbar)
{
	ATLASSERT(topbar?m_topUnits==unitsDays:m_bottomUnits==unitsDays);

	REAL barheight=REAL(Height());

	PointF p1, p2, p1m, p2m;

	if(!topbar)
	{
		p1.x=p2.x=0;
		p1.y=barheight;
		p2.y=barheight-m_tick1Height; 

		p1m.x=p2m.x=0;
		p1m.y=barheight;
		p2m.y=barheight-m_tick2Height; 
	}

	SizeF size;
	RectF rct;
	PointF descsubtract;

	TickerList::iterator itr=topbar?m_topTickers.begin():m_bottomTickers.begin();
	TickerList::iterator itrend=topbar?m_topTickers.end():m_bottomTickers.end();

	SYSTEMTIME st=m_beginTime;
	TCHAR strbuff[128];

	if(topbar) // draw leftmost day text
	{
		FormatString(st, formatDayMonthYear, strbuff, sizeof strbuff/sizeof strbuff[0]);
	}
	else
	{
		FormatString(st, formatDays, strbuff, sizeof strbuff/sizeof strbuff[0]);
	}

	unsigned count=topbar?m_topTickers.size():m_bottomTickers.size();
	if(count==0)
	{
		if(topbar)
		{
			CalcTextVerticalPosition(m_leftHotSpots.day, true);
			CalcTextVerticalPosition(m_leftHotSpots.month, true);
			CalcTextVerticalPosition(m_leftHotSpots.year, true);
			CalcHotSpots(m_beginTime, formatDayMonthYear, m_leftHotSpots, m_topLeftMargin);
		}

		size=CAggDC::GetScaledTextExtent(
			strbuff, 
			topbar?m_topTypeface.c_str():m_bottomTypeface.c_str(), 
			m_fonts,
			topbar?m_topTextHeight:m_bottomTextHeight);

		DrawLeftmost(aggdc, strbuff, size.Width+m_topLeftMargin, topbar, true);
		return;
	}

	REAL pos=FirstVisiblePosition(topbar);
	RectF rcleft=DrawLeftmost(aggdc, strbuff, pos, topbar, true);

	if(topbar)
	{
		CalcTextVerticalPosition(m_leftHotSpots.day, true);
		CalcTextVerticalPosition(m_leftHotSpots.month, true);
		CalcTextVerticalPosition(m_leftHotSpots.year, true);
		CalcHotSpots(m_beginTime, formatDayMonthYear, m_leftHotSpots, rcleft.x);
	}

	int lcount=0;
	while(itr!=itrend)
	{
		if(IsValueVisible((*itr).value, topbar))
		{
			if(topbar)
			{
				FormatString((*itr).st, formatDayMonthYear, strbuff, sizeof strbuff/sizeof strbuff[0]);
			}
			else
			{
				::_itot((*itr).value, strbuff, 10);
			}

			size=CAggDC::GetScaledTextExtent(
				strbuff, 
				topbar?m_topTypeface.c_str():m_bottomTypeface.c_str(), 
				m_fonts,
				topbar?m_topTextHeight:m_bottomTextHeight);

			rct.Width=size.Width;
			rct.x=(*itr).position+(topbar?m_topLeftMargin:m_bottomLeftMargin);
			CalcTextVerticalPosition(rct, topbar);

			aggdc.TransformPush();

			aggdc.DrawScaledText(
				strbuff, 
				rct,
				m_dividerColor,
				topbar?m_topTypeface.c_str():m_bottomTypeface.c_str(),
				topbar?m_topTextHeight:m_bottomTextHeight,
				descsubtract);

			aggdc.TransformPop();

			if(!topbar && m_showTickMarks)
			{
				p1.x=(*itr).position;
				p2.x=p1.x;
				aggdc.Line(p1, p2, m_dividerColor);
			}

			if(!topbar && m_showHalfTickMarks)
			{
				if(m_bottomFormat==formatDays)
				{
					REAL half=m_dayWidth/2;
					if(half>=m_halfTickSpacing)
					{
						p1m.x=(*itr).position+half;
						p2m.x=p1m.x;
						aggdc.Line(p1m, p2m, m_dividerColor);

						// ticks to the left of the first one
						if(lcount==0)
						{
							p1m.x=(*itr).position-half;
							if(p1m.x>0)
							{
								p2m.x=p1m.x;
								aggdc.Line(p1m, p2m, m_dividerColor);
							}
						}
					}
				}
				else
				{
					p1m.x=(*itr).position+m_dayWidth;
					p2m.x=p1m.x;
					int limit=13;
					if((*itr).st.wDay==15)
					{
						limit=m_daysinamonth[(*itr).st.wMonth-1];

						if((*itr).st.wMonth==2)
						{
							if(IsLeapYear((*itr).st.wYear))
								limit=IsLeapYear((*itr).st.wYear)?29:28;
						}

						limit-=15;
					}

					for(int i=0; i<limit; ++i)
					{
						aggdc.Line(p1m, p2m, m_dividerColor);
						p1m.x+=m_dayWidth;
						p2m.x=p1m.x;
					}

					// ticks to the left
					if(lcount==0)
					{
						p1m.x=(*itr).position-m_dayWidth;
						p2m.x=p1m.x;
						limit=13;
						for(int i=0; i<limit && p1m.x>0; ++i)
						{
							aggdc.Line(p1m, p2m, m_dividerColor);
							p1m.x-=m_dayWidth;
							p2m.x=p1m.x;
						}
					}
				}
			}

			++lcount;

		} // value is visible
		++itr;
	}
}

//-----------------------------------------------------------------------------

bool CTimelineCtrl::FitMonths(bool topbar)
{
	bool fit=true;
	TimeFormat* fmt=topbar?&m_topFormat:&m_bottomFormat;
	*fmt=formatMonths;
	m_monthModulo=1;

	unsigned count=topbar?m_topTickers.size():m_bottomTickers.size();
	if(count<2)
		return fit;

	// decide what format of month can we fit over or in between the tick marks
	REAL distance=m_dayWidth*28; // shortest month

	REAL maxwidth=topbar ? m_months.maxtopfull : m_months.maxbottomfull;

	if(distance<maxwidth)
	{
		maxwidth=topbar ? m_months.maxtopshort3 : m_months.maxbottomshort3;
		if(distance>=maxwidth)
		{
			*fmt=formatShortMonths;
		}
		else
		{
			maxwidth=topbar ? m_months.maxtopshort1 : m_months.maxbottomshort1;
			if(distance>=maxwidth)
			{
				*fmt=formatOneLetterMonths;
			}
			else
			{
				// try quarters
				if((distance*3)>=maxwidth)
				{
					*fmt=formatQuarterMonths;
					m_monthModulo=3;
				}
				else
				{
					fit=false;
				}
			}
		}
	}

	return fit;
}

//-----------------------------------------------------------------------------

void CTimelineCtrl::DrawMonths(CAggDC& aggdc, bool topbar)
{
	ATLASSERT(topbar?m_topUnits==unitsMonths:m_bottomUnits==unitsMonths);

	REAL barheight=REAL(Height());

	PointF p1, p2;
	if(!topbar)
	{
		p1.x=p2.x=0;
		p1.y=barheight;
		p2.y=barheight-m_tick1Height; 
	}

	RectF rct;
	SizeF size;
	PointF descsubtract;

	TDString* strings;
	const TCHAR* text=0;
	TickerList::iterator itr=topbar?m_topTickers.begin():m_bottomTickers.begin();
	TickerList::iterator itrend=topbar?m_topTickers.end():m_bottomTickers.end();

	SYSTEMTIME st=m_beginTime;
	TCHAR strbuff[128];

	if(topbar)
	{
		FormatString(st, formatMonthAndYear, strbuff, sizeof strbuff/sizeof strbuff[0]);
	}
	else
	{
		switch(m_bottomFormat)
		{
		case formatMonths:
			strings=m_months.full;
			break;
		case formatShortMonths:
			strings=m_months.short3;
			break;
		case formatOneLetterMonths:
		case formatQuarterMonths:
			strings=m_months.short1;
			break;
		default:
			ATLASSERT(0);
			break;
		};

		int value=(*itr).value-1; // previous month
		if(value==0)
			value=12;
		text=strings[value-1].text.c_str();
		_tcscpy(strbuff, text);
	}

	unsigned count=topbar?m_topTickers.size():m_bottomTickers.size();
	if(count==0)
	{
		if(topbar)
		{
			CalcTextVerticalPosition(m_leftHotSpots.month, true);
			CalcTextVerticalPosition(m_leftHotSpots.year, true);
			CalcHotSpots(m_beginTime, formatMonthAndYear, m_leftHotSpots, m_topLeftMargin);
		}

		size=CAggDC::GetScaledTextExtent(
			strbuff, 
			topbar?m_topTypeface.c_str():m_bottomTypeface.c_str(), 
			m_fonts,
			topbar?m_topTextHeight:m_bottomTextHeight);

		DrawLeftmost(aggdc, strbuff, size.Width+m_topLeftMargin, topbar, true);
		return;
	}

	REAL pos=FirstVisiblePosition(topbar);
	RectF rcleft=DrawLeftmost(aggdc, strbuff, pos, topbar, true);
	// leftmost hotspots
	if(topbar)
	{
		CalcTextVerticalPosition(m_leftHotSpots.month, true);
		CalcTextVerticalPosition(m_leftHotSpots.year, true);
		CalcHotSpots(m_beginTime, formatMonthAndYear, m_leftHotSpots, rcleft.x);
	}

	while(itr!=itrend)
	{
		if(IsValueVisible((*itr).value, topbar))
		{
			if(topbar)
			{
				FormatString((*itr).st, formatMonthAndYear, strbuff, sizeof strbuff/sizeof strbuff[0]);
				text=strbuff;
				size=CAggDC::GetScaledTextExtent(
					strbuff, 
					m_topTypeface.c_str(), 
					m_fonts,
					m_topTextHeight);

				rct.Width=size.Width;
			}
			else
			{
				text=strings[(*itr).value-1].text.c_str();
				rct.Width=strings[(*itr).value-1].bottomwidth;
			}

			rct.x=(*itr).position+(topbar?m_topLeftMargin:m_bottomLeftMargin);
			CalcTextVerticalPosition(rct, topbar);

			aggdc.TransformPush();

			aggdc.DrawScaledText(
				text, 
				rct,
				m_dividerColor,
				topbar?m_topTypeface.c_str():m_bottomTypeface.c_str(),
				topbar?m_topTextHeight:m_bottomTextHeight,
				descsubtract);

			aggdc.TransformPop();

			if(!topbar && m_showTickMarks)
			{
				p1.x=(*itr).position;
				p2.x=p1.x;
				aggdc.Line(p1, p2, m_dividerColor);
			}
		}

		++itr;
	}
}

//-----------------------------------------------------------------------------

bool CTimelineCtrl::FitYears(bool topbar)
{
	TimeFormat* fmt=topbar?&m_topFormat:&m_bottomFormat;
	*fmt=formatYears;
	m_yearModulo=1;

	unsigned count=topbar?m_topTickers.size():m_bottomTickers.size();
	bool fit=true;
	if(count<3)
		return fit;

	REAL distance=m_dayWidth*365;
	REAL yearStrWidth=m_secsTopMaxWidth /*2 numbers*/ * 2; // TODO - make better
	if(yearStrWidth>distance)
		fit=false;

	if(!fit)
	{
		if(yearStrWidth<distance*10)
			m_yearModulo=10;
		else
			m_yearModulo=20;

		ATLASSERT(yearStrWidth<distance*20);
		fit=true;
	}

	return fit;
}

//-----------------------------------------------------------------------------

void CTimelineCtrl::DrawYears(CAggDC& aggdc, bool topbar)
{
	ATLASSERT(topbar?m_topUnits==unitsYears:m_bottomUnits==unitsYears);

	REAL barheight=REAL(Height());

	PointF p1, p2;
	if(!topbar)
	{
		p1.x=p2.x=0;
		p1.y=barheight;
		p2.y=barheight-m_tick1Height; 
	}

	SizeF size;
	RectF rct;
	PointF descsubtract;
	TCHAR strbuff[64];

	unsigned count=topbar?m_topTickers.size():m_bottomTickers.size();
	if(count==0)
	{
		FormatString(m_beginTime, formatYears, strbuff, sizeof strbuff/sizeof strbuff[0]);

		size=CAggDC::GetScaledTextExtent(
			strbuff, 
			topbar?m_topTypeface.c_str():m_bottomTypeface.c_str(), 
			m_fonts,
			topbar?m_topTextHeight:m_bottomTextHeight);

		DrawLeftmost(aggdc, strbuff, size.Width+m_topLeftMargin, topbar, true);
		return;
	}

	TickerList::iterator itr=topbar?m_topTickers.begin():m_bottomTickers.begin();
	TickerList::iterator itrend=topbar?m_topTickers.end():m_bottomTickers.end();

	// draw leftmost year text
	
	::_itot((*itr).value-1, strbuff, 10);  

	REAL pos=FirstVisiblePosition(topbar);
	DrawLeftmost(aggdc, strbuff, pos, topbar, true);

	while(itr!=itrend)
	{
		if(IsValueVisible((*itr).value, topbar))
		{
			::_itot((*itr).value, strbuff, 10);

			size=CAggDC::GetScaledTextExtent(
				strbuff, 
				topbar?m_topTypeface.c_str():m_bottomTypeface.c_str(), 
				m_fonts,
				topbar?m_topTextHeight:m_bottomTextHeight);

			rct.Width=size.Width;
			rct.x=(*itr).position+(topbar?m_topLeftMargin:m_bottomLeftMargin);
			CalcTextVerticalPosition(rct, topbar);

			aggdc.TransformPush();

			aggdc.DrawScaledText(
				strbuff, 
				rct,
				m_dividerColor,
				topbar?m_topTypeface.c_str():m_bottomTypeface.c_str(),
				topbar?m_topTextHeight:m_bottomTextHeight,
				descsubtract);

			aggdc.TransformPop();

			if(!topbar && m_showTickMarks)
			{
				p1.x=(*itr).position;
				p2.x=p1.x;
				aggdc.Line(p1, p2, m_dividerColor);
			}
		}

		++itr;
	}
}

//-----------------------------------------------------------------------------

void CTimelineCtrl::TDString::Init(
	const TCHAR* ptext, const char* typeface, CAggFonts* fonts, int height, bool top)
{
	if(text.empty())
		text=ptext;

	SizeF size=CAggDC::GetScaledTextExtent(
		ptext, 
		typeface, 
		fonts,
		height);

	if(top)
		topwidth=size.Width;
	else
		bottomwidth=size.Width;
}

//-----------------------------------------------------------------------------

void CTimelineCtrl::MonthStrings::Init()
{
	SYSTEMTIME st;
	memset(&st,0,sizeof SYSTEMTIME);
	st.wYear=2000;
	st.wDay=1;

	maxtopfull=0; 
	maxbottomfull=0;
	maxtopshort3=0; 
	maxbottomshort3=0;
	maxtopshort1=0; 
	maxbottomshort1=0;

	TimeFormat formats[3]={
		formatMonths,
		formatShortMonths,
		formatOneLetterMonths };

	TDString* strings[3]={
		full,
		short3,
		short1};

	REAL* maxtops[3]={
		&maxtopfull,
		&maxtopshort3,
		&maxtopshort1};

	REAL* maxbottoms[3]={
		&maxbottomfull,
		&maxbottomshort3,
		&maxbottomshort1};

	TCHAR sbuff[128];

	for(int i=0; i<(sizeof formats/sizeof formats[0]); ++i)
	{
		st.wMonth=1;
		while(st.wMonth<13)
		{
			ctrl->FormatString(st, formats[i], sbuff, sizeof(sbuff)/sizeof(TCHAR));

			strings[i][st.wMonth-1].Init(
				sbuff, 
				ctrl->m_topTypeface.c_str(), 
				ctrl->m_fonts, 
				ctrl->m_topTextHeight,
				true);

			if(strings[i][st.wMonth-1].topwidth > *maxtops[i])
				*maxtops[i]=strings[i][st.wMonth-1].topwidth;

			strings[i][st.wMonth-1].Init(
				sbuff, 
				ctrl->m_bottomTypeface.c_str(), 
				ctrl->m_fonts, 
				ctrl->m_bottomTextHeight,
				false);

			if(strings[i][st.wMonth-1].bottomwidth > *maxbottoms[i])
				*maxbottoms[i]=strings[i][st.wMonth-1].bottomwidth;

			st.wMonth++;
		}
	}
}

//-----------------------------------------------------------------------------

CTimelineCtrl::HotSpots::HotSpots() : fmt(formatUnknown)
{
}

//-----------------------------------------------------------------------------

bool CTimelineCtrl::HotSpots::Intersect(PointF pos, BarUnits& units)
{
	bool yes=false;

	switch(fmt)
	{
	case formatHourMinute:
		if(minute.Contains(pos))
		{
			yes=true;
			units=unitsMinutes;
			break;
		} // fall through
	case formatHours:
		if(hour.Contains(pos))
		{
			yes=true;
			units=unitsHours;
			break;
		} // fall through
	case formatDayMonthYear:
		if(day.Contains(pos))
		{
			yes=true;
			units=unitsDays;
			break;
		} // fall through
	case formatMonthAndYear:
		{
			if(month.Contains(pos))
			{
				yes=true;
				units=unitsMonths;
			}
			else if(year.Contains(pos))
			{
				yes=true;
				units=unitsYears;
			}
		}
		break;
	default:
		break;
	};

	return yes;
}

//-----------------------------------------------------------------------------
// 
// messages
//
//-----------------------------------------------------------------------------

LRESULT CTimelineCtrl::OnCreate(LPCREATESTRUCT lParam)
{
	//m_tooltip.Create(m_hWnd);
	//m_tooltip.Activate(TRUE);

	return 1;
}

//-----------------------------------------------------------------------------

LRESULT CTimelineCtrl::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	ATLASSERT(m_fonts); // must have font mgr before drawing

	if(m_beginTick.dwLowDateTime==m_endTick.dwLowDateTime &&
		m_beginTick.dwHighDateTime==m_endTick.dwHighDateTime)
		return 0;

	CPaintDC dc(m_hWnd);
	if(m_topBarHeight==0)
		m_topBarHeight=CalcTopBarHeight();

	if(m_calcTicks)
	{
		CalcTicks();
		CalcHotSpotsList();
		m_calcTicks=false;
	}

	CRect rc;
	GetClientRect(&rc);

	boost::scoped_ptr<CAggDC> aggdc(CAggDC::CreateBlitMemoryDC(dc, m_fonts, m_hWnd, &rc));
	PointF p1(0.0, REAL(m_topBarHeight));
	PointF p2(REAL(rc.Width()), REAL(m_topBarHeight));
	aggdc->Line(p1, p2, m_dividerColor);

	m_leftHotSpots.fmt=formatUnknown; // effectively clears the old one

	DrawBackground(*aggdc, true);
	switch(m_topUnits)
	{
	case unitsMinutes:
		DrawMinutes(*aggdc, true);
		break;
	case unitsHours:
		DrawHours(*aggdc, true);
		break;
	case unitsDays:
		DrawDays(*aggdc, true);
		break;
	case unitsMonths:
		DrawMonths(*aggdc, true);
		break;
	case unitsYears:
		DrawYears(*aggdc, true);
		break;
	default:
		ATLASSERT(0);
		break;
	};

	DrawBackground(*aggdc, false);

	switch(m_bottomUnits)
	{
	case unitsSeconds:
		DrawSeconds(*aggdc);
		break;
	case unitsMinutes:
		DrawMinutes(*aggdc, false);
		break;
	case unitsHours:
		DrawHours(*aggdc, false);
		break;
	case unitsDays:
		DrawDays(*aggdc, false);
		break;
	case unitsMonths:
		DrawMonths(*aggdc, false);
		break;
	case unitsYears:
		DrawYears(*aggdc, false);
		break;
	default:
		ATLASSERT(0);
		break;
	};// switch

	p1.y=p2.y=Height();
	aggdc->Line(p1, p2, m_dividerColor);

	return 0;
}

//-----------------------------------------------------------------------------

LRESULT CTimelineCtrl::OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return 1;
}

//-----------------------------------------------------------------------------

LRESULT CTimelineCtrl::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	int newwidth=GET_X_LPARAM(lParam);
	ResizeTimeline(newwidth);
	return 0;
}

//-----------------------------------------------------------------------------

LRESULT CTimelineCtrl::OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	PointF pt(REAL(GET_X_LPARAM(lParam)),REAL(GET_Y_LPARAM(lParam))); 

	BarUnits units;
	SYSTEMTIME st;
	bool intersect=m_leftHotSpots.Intersect(pt, units);
	if(intersect)
	{
		st=m_leftHotSpots.st;
	}
	else if(!m_hotSpots.empty())
	{
		HotSpotList::iterator itr=m_hotSpots.begin();
		while(itr!=m_hotSpots.end())
		{
			intersect=(*itr).Intersect(pt, units);
			if(intersect)
			{
				st=(*itr).st;
				break;
			}
			++itr;
		}
	}

	if(intersect)
	{
		// for now reuse double click
		NMTIMELINEDBLCLK nmclick;
		nmclick.begin=nmclick.end=st; 

		nmclick.begin.wMilliseconds=nmclick.end.wMilliseconds=0;
		nmclick.begin.wSecond=nmclick.end.wSecond=0;

		switch(units)
		{
		case unitsYears:
			nmclick.begin.wMinute=nmclick.end.wMinute=0;
			nmclick.begin.wHour=nmclick.end.wHour=0;
			nmclick.begin.wDay=nmclick.end.wDay=1;
			nmclick.begin.wMonth=nmclick.end.wMonth=1;
			nmclick.end.wYear++;
			ATLASSERT(nmclick.begin.wDay==nmclick.end.wDay && nmclick.end.wDay==1);
			break;
		case unitsMonths:
			nmclick.begin.wMinute=nmclick.end.wMinute=0;
			nmclick.begin.wHour=nmclick.end.wHour=0;
			nmclick.begin.wDay=nmclick.end.wDay=1;
			IncrementMonth(nmclick.end);
			break;
		case unitsDays:
			nmclick.begin.wMinute=nmclick.end.wMinute=0;
			nmclick.begin.wHour=nmclick.end.wHour=0;
			IncrementDay(nmclick.end);
			break;
		case unitsHours:
			nmclick.begin.wMinute=nmclick.end.wMinute=0;
			IncrementHour(nmclick.end);
			break;
		case unitsMinutes:
			IncrementMinute(nmclick.end);
			break;
		default:
			ATLASSERT(0);
			break;
		}

		nmclick.hdr.hwndFrom=m_hWnd;
		nmclick.hdr.idFrom=GetDlgCtrlID();
		nmclick.hdr.code=SCW_NOTIFY_TIMELINE_DBLCLK;

		GetParent().SendMessage(WM_NOTIFY, 0, (LPARAM) &nmclick);

	}

	return 0;
}

//-----------------------------------------------------------------------------

LRESULT CTimelineCtrl::OnRButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	CPoint pt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)); 

	FILETIME ft;
	TimeFromScreenPos(ft, pt.x);

	NMTIMELINERCLK nmclick;
	nmclick.pt=pt;
	BOOL bOk=::FileTimeToSystemTime(&ft, &nmclick.time);
	ATLASSERT(bOk);

	nmclick.hdr.hwndFrom=m_hWnd;
	nmclick.hdr.idFrom=GetDlgCtrlID();
	nmclick.hdr.code=SCW_NOTIFY_TIMELINE_RCLICK;

	GetParent().SendMessage(WM_NOTIFY, 0, (LPARAM) &nmclick);

	return 0;
}

//-----------------------------------------------------------------------------

LRESULT CTimelineCtrl::OnLButtonDblclk(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	PointF pt(REAL(GET_X_LPARAM(lParam)),REAL(GET_Y_LPARAM(lParam))); 

	NMTIMELINEDBLCLK nmclick;

	bool topbar=pt.y<m_topBarHeight;
	TickerList::iterator itr=topbar?m_topTickers.begin():m_bottomTickers.begin();
	TickerList::iterator itrend=topbar?m_topTickers.end():m_bottomTickers.end();

	bool found=false;
	while(itr!=itrend)
	{
		if(pt.x>(*itr).position)
		{
			TickerList::iterator next=itr;
			++next;
			if(next==itrend)
			{
				nmclick.begin=(*itr).st; 
				SYSTEMTIME nextst=(*itr).st;

				BarUnits units=topbar?m_topUnits:m_bottomUnits;
				switch(units)
				{
				case unitsYears:
					nextst.wYear++;
					break;
				case unitsMonths:
					IncrementMonth(nextst);
					break;
				case unitsDays:
					IncrementDay(nextst);
					break;
				case unitsHours:
					IncrementHour(nextst);
					break;
				case unitsMinutes:
					IncrementMinute(nextst);
					break;
				case unitsSeconds:
					IncrementSecond(nextst);
					break;
				default:
					ATLASSERT(0);
					break;
				};

				nmclick.end=nextst;
				found=true;
				break;
			}
			else if(pt.x<(*next).position)
			{
				nmclick.begin=(*itr).st; 
				nmclick.end=(*next).st;
				found=true;
				break;
			}
		}
		else
		{
			nmclick.end=(*itr).st; 
			SYSTEMTIME prevst=(*itr).st;
			BarUnits units=topbar?m_topUnits:m_bottomUnits;
			switch(units)
			{
			case unitsYears:
				prevst.wYear--;
				break;
			case unitsMonths:
				DecrementMonth(prevst);
				break;
			case unitsDays:
				DecrementDay(prevst);
				break;
			case unitsHours:
				DecrementHour(prevst);
				break;
			case unitsMinutes:
				DecrementMinute(prevst);
				break;
			case unitsSeconds:
				DecrementSecond(prevst);
				break;
			default:
				ATLASSERT(0);
				break;
			};

			nmclick.begin=prevst;
			found=true;
			break;
		}
		++itr;
	}

	if(found)
	{
		nmclick.hdr.hwndFrom=m_hWnd;
		nmclick.hdr.idFrom=GetDlgCtrlID();
		nmclick.hdr.code=SCW_NOTIFY_TIMELINE_DBLCLK;

		GetParent().SendMessage(WM_NOTIFY, 0, (LPARAM) &nmclick);
	}

	return 0;
}

//-----------------------------------------------------------------------------

LRESULT CTimelineCtrl::OnMouseMove(
	UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	PointF p(REAL(GET_X_LPARAM(lParam)),REAL(GET_Y_LPARAM(lParam))); 

	if(IsOverHotspot(p))
	{
		SetCursor(m_HandOpenCursor);
	}
	else
	{
		if(GetCursor()==m_HandOpenCursor)
			SetCursor(LoadCursor(NULL, IDC_ARROW));
	}

	return 0;
}

//-----------------------------------------------------------------------------

LRESULT CTimelineCtrl::OnNotifyMCNSelect(WPARAM /*wParam*/, LPNMHDR lpnmhdr, BOOL& /*bHandled*/)
{
	LPNMSELCHANGE lpNMSelChange = (LPNMSELCHANGE)lpnmhdr;

	Ftint64 time;

	BOOL ok=::SystemTimeToFileTime(&lpNMSelChange->stSelStart, &time.ft);
	ATLASSERT(ok);

	// selected date will be on the left, right will be existing time interval
	Ftint64 t1;
	t1.ft=m_beginTick;
	Ftint64 t2;
	t2.ft=m_endTick;

	__int64 t2_t1=t2.i64-t1.i64;

	t2.i64=time.i64 + t2_t1;

	// reuse double click for now
	NMTIMELINEDBLCLK nmclick;
	nmclick.begin=lpNMSelChange->stSelStart;
	ok=::FileTimeToSystemTime(&t2.ft, &nmclick.end);
	ATLASSERT(ok);

	::SendMessage(lpNMSelChange->nmhdr.hwndFrom, WM_CLOSE, 0, 0);

	nmclick.hdr.hwndFrom=m_hWnd;
	nmclick.hdr.idFrom=GetDlgCtrlID();
	nmclick.hdr.code=SCW_NOTIFY_TIMELINE_DBLCLK;

	GetParent().SendMessage(WM_NOTIFY, 0, (LPARAM) &nmclick);

	return 0;
}
