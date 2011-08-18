#include "StdAfx.h"

#include "UTCParser.h"
//#include "clib.h"
//bool CLIB_API UTCToDateTime(const std::_tstring & utc, SYSTEMTIME &dateTime);

struct timestamp : public tm
    {
    timestamp() { memset(this, 0, sizeof(*this)); }
    int tzoffset;
    };

std::ostream& operator<< (std::ostream& os, const timestamp& ts)
    {
    os << "timestamp =\n"
       << "    {\n"
       << "    tm_sec   = " << ts.tm_sec   << "\n"
       << "    tm_min   = " << ts.tm_min   << "\n"
       << "    tm_hour  = " << ts.tm_hour  << "\n"
       << "    tm_mday  = " << ts.tm_mday  << "\n"
       << "    tm_mon   = " << ts.tm_mon   << "\n"
       << "    tm_year  = " << ts.tm_year  << "\n"
       << "    tm_wday  = " << ts.tm_wday  << "\n"
       << "    tm_yday  = " << ts.tm_yday  << "\n"
       << "    tm_isdst = " << ts.tm_isdst << "\n"
       << "    tzoffset = " << ts.tzoffset << "\n"
       << "    }\n";
    return os;
    }

struct rfc_skipper : public boost::spirit::classic::grammar<rfc_skipper>
    {
    rfc_skipper()
        {
        }
    template<typename scannerT>
    struct definition
        {
        definition(const rfc_skipper& /*self*/)
            {
				using namespace boost::spirit::classic;

            first = lexeme_d
                    [
                        junk    = lwsp | comment,

                        lwsp    = +(    !str_p("\r\n")
                                        >> chset_p(" \t")
                                   ),

                        comment =  ch_p('(')
                                   >>  *(   lwsp
                                        |   ctext
                                        |   qpair
                                        |   comment
                                        )
                                   >> ')',

                        ctext   =  anychar_p - chset_p("()\\\r"),

                        qpair   =  ch_p('\\') >> anychar_p
                    ];
            }
        const boost::spirit::classic::rule<scannerT>& start() const
            {
            return first;
            }
        boost::spirit::classic::subrule<0>     junk;
        boost::spirit::classic::subrule<1>     lwsp;
        boost::spirit::classic::subrule<2>     comment;
        boost::spirit::classic::subrule<3>     ctext;
        boost::spirit::classic::subrule<4>     qpair;
        boost::spirit::classic::rule<scannerT> first;
        };
    };
const rfc_skipper rfc_skipper_p;

struct month_parser : boost::spirit::classic::symbols<int>
    {
    month_parser()
        {
        add ("jan", 0)("feb", 1)("mar", 2)("apr", 3)
            ("may", 4)("jun", 5)("jul", 6)("aug", 7)
            ("sep", 8)("oct", 9)("nov", 10)("dec", 11);
        }
    };
const month_parser month_p;

struct wday_parser : boost::spirit::classic::symbols<int>
    {
    wday_parser()
        {
        add ("sun", 0)("mon", 1)("tue", 2)("wed", 3)
            ("thu", 4)("fri", 5)("sat", 6);
        }
    };
const wday_parser wday_p;

struct timezone_parser : boost::spirit::classic::symbols<int>
    {
    timezone_parser()
        {
        add ("ut", 0)("gmt", 0)
            ("est", -18000)("edt", -14400)
            ("cst", -21600)("cdt", -18000)
            ("mst", -25200)("mdt", -21600)
            ("pst", -28800)("pdt", -25200)
            ("z", 0)("a", -3600)("m", -43200)("n", +3600)("y", +43200);
        }
    };
const timezone_parser timezone_p;

struct rfcdate_closure : boost::spirit::classic::closure<rfcdate_closure, timestamp>
    {
    member1 val;
    };

struct rfcdate_parser : public boost::spirit::classic::grammar<rfcdate_parser, rfcdate_closure::context_t>
    {
    rfcdate_parser()
        {
        }
    template<typename scannerT>
    struct definition
        {
        definition(const rfcdate_parser& self)
            {
				using namespace boost::spirit::classic;
            using namespace phoenix;

#define ASSIGN(member, what) bind(&timestamp::member)(self.val) = what

            first =
                (
                    date_time = !(    lexeme_d
                                      [
                                          as_lower_d
                                          [
                                              wday_p [ASSIGN(tm_wday, arg1)]
                                          ]
                                      ]
                                      >> ','
                                 )
                                 >> (date | utc_date)
                                 >> !time
                                 >> !zone,

                    date      =  uint_p [ASSIGN(tm_mday, arg1)]
                                 >> lexeme_d
                                    [
                                        as_lower_d
                                        [
                                            month_p [ASSIGN(tm_mon, arg1)]
                                        ]
                                    ]
                                 >> (    limit_d(0u, 99u)
                                         [
                                             uint_p [ASSIGN(tm_year, arg1)]
                                         ]
                                    |    min_limit_d(1900u)
                                         [
                                             uint_p [ASSIGN(tm_year, arg1 - 1900)]
                                         ]
                                    ),

                    utc_date =  uint_p [ASSIGN(tm_year, arg1)]
                                >> '-'
                                >> uint_p [ASSIGN(tm_mon, arg1)]
                                >> '-'
                                >> uint_p [ASSIGN(tm_mday, arg1)]
								>> !ch_p('T'),

					time      = uint_p [ASSIGN(tm_hour, arg1)]
                                >> ':'
                                >> uint_p [ASSIGN(tm_min, arg1)]
                                >> !(
                                        ':'
                                        >> uint_p [ASSIGN(tm_sec, arg1)]
                                    ),

                    zone      = ch_p('+')  >>  uint4_p
                                               [
                                                   ASSIGN(tzoffset,
                                                          ((arg1 / 100) * 60 + (arg1 % 100)) * 60)
                                               ]
                              | ch_p('-')  >>  uint4_p
                                               [
                                                   ASSIGN(tzoffset,
                                                          -((arg1 / 100) * 60 + (arg1 % 100)) * 60)
                                               ]
                              | lexeme_d
                                [
                                    as_lower_d
                                    [
                                        timezone_p [ASSIGN(tzoffset, arg1)]
                                    ]
                                ]
                );
#undef ASSIGN
            }
        const boost::spirit::classic::rule<scannerT>& start() const
            {
            return first;
            }
        boost::spirit::classic::subrule<0> date_time;
        boost::spirit::classic::subrule<1> date;
        boost::spirit::classic::subrule<2> time;
        boost::spirit::classic::subrule<3> zone;
        boost::spirit::classic::subrule<4> utc_date;
        boost::spirit::classic::uint_parser<int, 10, 4, 4>  uint4_p;
        boost::spirit::classic::rule<scannerT> first;
        };
    };

bool UTCToDateTime(const std::_tstring & utc, tm &result)
{
	using namespace std;
	using namespace boost::spirit::classic;
	using namespace phoenix;

	rfcdate_parser rfcdate_p;
    timestamp  result1;
	if (boost::spirit::classic::parse(utc.c_str(), rfcdate_p[phoenix::var(result1) = phoenix::arg1], rfc_skipper_p).full)
	{
		result = result1;
		return true;
	}
	return false;
}

//ECLLIB_API bool UTCToDateTime2(const std::_tstring & utc, SYSTEMTIME &dateTime)
//{
//	UTCToDateTime(utc, dateTime);
//	tm dt;
//	if (UTCToDateTime(utc, dt))
//	{
//		dateTime.wYear = (WORD)dt.tm_year + 1900;
//		dateTime.wMonth = (WORD)dt.tm_mon + 1; 
//		dateTime.wDayOfWeek = (WORD)dt.tm_wday;
//		dateTime.wDay = (WORD)dt.tm_mday;
//		dateTime.wHour = (WORD)dt.tm_hour;
//		dateTime.wMinute = (WORD)dt.tm_min;
//		dateTime.wSecond = (WORD)dt.tm_sec;
//		dateTime.wMilliseconds = 0;
//		return true;
//	}
//	return false;
//}
