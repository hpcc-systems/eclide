#pragma once

#include "GraphTypes.h"

//-----------------------------------------------------------------------------

struct CAggFonts;
class CAggDC;

//-----------------------------------------------------------------------------

class CTimelineCtrl : public CWindowImpl<CTimelineCtrl>
{
	//-------------------------------------------------------------------
	// Private data types

	enum BarUnits
	{
		unitsYears,
		unitsMonths,
		unitsDays,
		unitsHours,
		unitsMinutes,
		unitsSeconds
	};

	// keep in order of increasing enum value as time interval gets shorter
	enum TimeFormat
	{
		formatUnknown,
		formatYears,
		formatMonths,
		formatShortMonths,
		formatOneLetterMonths,
		formatQuarterMonths,
		formatMonthAndYear,
		formatDays,
		formatDays15, // 1st and 15th day in a month are displayed (can't be done as modulo)
		formatDayMonthYear,
		formatDayShortMonthYear,
		formatHours,
		formatHourMinute,
		formatMinutes,
		formatHourMinuteSecond,
		formatSeconds
	};

	union Ftint64
	{
		FILETIME ft;
		__int64 i64;
	};

	struct Ticker
	{
		unsigned value;
		GraphTypes::REAL position;
		SYSTEMTIME st;
	};

	typedef std::list<Ticker> TickerList;

	struct TDString
	{
		std::_tstring text;
		GraphTypes::REAL topwidth;
		GraphTypes::REAL bottomwidth;

		void Init(const TCHAR* ptext, const char* typeface, CAggFonts* fonts, int height, bool top);
	};

	struct MonthStrings
	{
		CTimelineCtrl* ctrl;

		TDString full[12];
		GraphTypes::REAL maxtopfull; // maximum string width
		GraphTypes::REAL maxbottomfull;

		TDString short3[12]; // 3 letters
		GraphTypes::REAL maxtopshort3; // maximum string width
		GraphTypes::REAL maxbottomshort3;

		TDString short1[12]; // 1 letter
		GraphTypes::REAL maxtopshort1; // maximum string width
		GraphTypes::REAL maxbottomshort1;

		MonthStrings(CTimelineCtrl* _ctrl) : ctrl(_ctrl) {}
		void Init();
	};

	struct HotSpots
	{
		SYSTEMTIME st;
		TimeFormat fmt;
		GraphTypes::RectF minute;
		GraphTypes::RectF hour;
		GraphTypes::RectF day;
		GraphTypes::RectF month;
		GraphTypes::RectF year;

		HotSpots();
		bool Intersect(GraphTypes::PointF pos, BarUnits& units);
	};

	typedef std::list<HotSpots> HotSpotList;

	//-------------------------------------------------------------------
	// Implementation

	// control's time range in two different formats
	FILETIME m_beginTick, m_endTick;
	SYSTEMTIME m_beginTime, m_endTime;
	SYSTEMTIME m_beginTimeUTC, m_endTimeUTC;
	TIME_ZONE_INFORMATION m_tz;

	int m_daysinamonth[12];

	TickerList m_topTickers;
	TickerList m_bottomTickers;
	HotSpotList m_hotSpots;
	HotSpots m_leftHotSpots; 

	bool m_calcTicks;
	MonthStrings m_months;

	BarUnits m_topUnits;
	TimeFormat m_topFormat;

	BarUnits m_bottomUnits;
	TimeFormat m_bottomFormat;
	
	int m_yearModulo, m_monthModulo, m_hourModulo, m_minModulo, m_secModulo;

	bool m_24HourFormat;
	bool m_showTickMarks;
	bool m_showHalfTickMarks;

	// width of 2 numbers plus possibly symbol suffix (like 10s or 10sec)
	GraphTypes::REAL m_secsTopMaxWidth, m_minsTopMaxWidth, m_hoursTopMaxWidth, m_daysTopMaxWidth;
	GraphTypes::REAL m_secsBottomMaxWidth, m_minsBottomMaxWidth, m_hoursBottomMaxWidth, m_daysBottomMaxWidth;
	GraphTypes::REAL m_hourSymbolTopWidth, m_hourSymbolBottomWidth; // AM

	// separator width for bottom bar
	GraphTypes::REAL m_spaceBottomWidth;
	// separator chars for top bar
	GraphTypes::REAL m_spaceTopWidth, m_commaTopWidth, m_colonTopWidth;


	// these widths are time interval in pixels, not string widths
	GraphTypes::REAL m_secondWidth,	m_minuteWidth, m_hourWidth, m_dayWidth;

	GraphTypes::REAL m_tick1Height;
	GraphTypes::REAL m_tick2Height; 
	GraphTypes::REAL m_halfTickSpacing; // if we have this space available, draw ticks in-between
	GraphTypes::REAL m_fittingSlack; // extra pixels to add when fitting text horizontally

	int m_wndWidth; // last pixel width that control was drawn at

	CAggFonts* m_fonts;
	static const TCHAR* teststr; // for measuring font pixel height

	std::string m_topTypeface;	 // not _tstring because Agg has no support for TCHAR
	int m_topTextHeight; // typographic height, not pixel height
	GraphTypes::REAL m_topFontPixelHeight;

	std::string m_bottomTypeface;
	int m_bottomTextHeight;
	GraphTypes::REAL m_bottomFontPixelHeight;

	int m_topBarHeight;
	int m_topMargin;
	int m_topLeftMargin;

	int m_bottomBarHeight;
	int m_bottomMargin;
	int m_bottomLeftMargin;

	GraphTypes::Color m_dividerColor;
	GraphTypes::Color m_lightBgrColor;
	GraphTypes::Color m_darkBgrColor;

	HCURSOR m_HandOpenCursor;

	// WTL::CToolTipCtrl m_tooltip;

	//-------------------------------------------------------------------

	int CalcTopBarHeight();
	int CalcBottomBarHeight();
	void CalcStandardTextWidths();
	void CalcTextVerticalPosition(GraphTypes::RectF& rct, bool topbar);

	void FormatString(const SYSTEMTIME& st, TimeFormat format, TCHAR* str, unsigned len);
	void CalcHotSpots(const SYSTEMTIME& st, TimeFormat format, HotSpots& spots, GraphTypes::REAL x);
	void CalcHotSpotsList();
	bool IsOverHotspot(const GraphTypes::PointF& pt);
	void CalcState();

	GraphTypes::REAL ScreenPosFromTime(const FILETIME& time) const; 
	GraphTypes::REAL ScreenSizeFromTimeInterval(const __int64& interval) const;
	void TimeFromScreenPos(FILETIME& time, int pos) const;

	int DayOfYear(const SYSTEMTIME& st);
	void NextMonth(SYSTEMTIME& syst);
	void IncrementMonth(SYSTEMTIME& syst);
	void DecrementMonth(SYSTEMTIME& syst);
	void NextDay(SYSTEMTIME& syst);
	void IncrementDay(SYSTEMTIME& syst);
	void DecrementDay(SYSTEMTIME& syst);
	void NextHour(SYSTEMTIME& syst);
	void IncrementHour(SYSTEMTIME& syst);
	void DecrementHour(SYSTEMTIME& syst);
	void NextMinute(SYSTEMTIME& syst);
	void IncrementMinute(SYSTEMTIME& syst);
	void DecrementMinute(SYSTEMTIME& syst);
	void NextSecond(SYSTEMTIME& syst);
	void IncrementSecond(SYSTEMTIME& syst);
	void DecrementSecond(SYSTEMTIME& syst);

	void CalcTimeIntervalWidths();
	void CalcTicks(void);

	int CurrentModuloDivisor(bool topbar) const;
	bool IsValueVisible(unsigned value, bool topbar) const;
	GraphTypes::REAL FirstVisiblePosition(bool topbar);

	void ResizeTimeline(int pixels);

	GraphTypes::RectF DrawLeftmost(
		CAggDC& aggdc, const TCHAR* text, GraphTypes::REAL rborder, bool topbar, bool leftAlign);
	bool AlternatingColor(TimeFormat format, unsigned value, bool topbar);
	void DrawBackground(CAggDC& aggdc, bool topbar);

	bool FitSeconds();
	void DrawSeconds(CAggDC& aggdc);
	bool FitMinutes(bool topbar);
	void DrawMinutes(CAggDC& aggdc, bool topbar);
	bool FitHours(bool topbar);
	void DrawHours(CAggDC& aggdc, bool topbar);
	bool FitDays(bool topbar);
	void DrawDays(CAggDC& aggdc, bool topbar);
	bool FitMonths(bool topbar);
	void DrawMonths(CAggDC& aggdc, bool topbar);
	bool FitYears(bool topbar);
	void DrawYears(CAggDC& aggdc, bool topbar);

public:

	//-------------------------------------------------------------------
	// construction

	CTimelineCtrl();
	~CTimelineCtrl(void);

	//-------------------------------------------------------------------
	// methods

	void SetFontMgr(CAggFonts* fonts);
	int Height();
	void SetTimeRange(const FILETIME& begin, const FILETIME& end);
	void SetTimeRange(const SYSTEMTIME& begin, const SYSTEMTIME& end);
	void GetTimeRange(FILETIME& begin, FILETIME& end);
	void GetTimeRange(SYSTEMTIME& begin, SYSTEMTIME& end);
	void ScrollTimeline(int pixels);
	void ScaleTimeline(double scale);
	void Set24HourFormat(bool set);
	void SetTimeZone(const TIME_ZONE_INFORMATION& tz);
	void GetTimeZone(TIME_ZONE_INFORMATION& tz) const { tz=m_tz; }
	bool DisplayMonthCalendar() const;

	//-------------------------------------------------------------------
	// Windows

	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP(CTimelineCtrl)
		MSG_WM_CREATE(OnCreate)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_RBUTTONDOWN, OnRButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnLButtonDblclk)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		NOTIFY_CODE_HANDLER(MCN_SELECT, OnNotifyMCNSelect)
	END_MSG_MAP()

	LRESULT OnCreate(LPCREATESTRUCT lParam);

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnRButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLButtonDblclk(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnNotifyMCNSelect(WPARAM /*wParam*/, LPNMHDR lpnmhdr, BOOL& /*bHandled*/);
};
