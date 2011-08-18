#include "StdAfx.h"

#include "VersionParser.h"

using namespace BOOST_SPIRIT_CLASSIC_NS;

struct ParsedVersionS : public ParsedVersion
{
	ParsedVersionS() 
	{ 
		majorVersion = 0;
		minorVersion = 0;
		pointVersion = 0;
		build = 0;
		ossFlag = false;
	}
};

std::ostream& operator<< (std::ostream& os, const ParsedVersionS& def)
{
	std::string preffix = CT2A(def.preffix.c_str(), CP_UTF8);
	std::string minorVersionAlpha = CT2A(def.minorVersionAlpha.c_str(), CP_UTF8);
	std::string suffix = CT2A(def.suffix.c_str(), CP_UTF8);
	std::string buildAlpha = CT2A(def.buildAlpha.c_str(), CP_UTF8);
	os << "Version =\n"
		<< "    {\n"
		<< "    preffix = " << preffix << "\n"
		<< "    major version = " << def.majorVersion << "\n"
		<< "    minor version = " << def.minorVersion << "\n"
		<< "    minor version alpha = " << minorVersionAlpha << "\n"
		<< "    point version = " << def.pointVersion << "\n"
		<< "    suffix = " << suffix << "\n"
		<< "    build = " << def.build << "\n"
		<< "    build alpha = " << buildAlpha << "\n"
		<< "    }\n";
	return os;
}

struct version_closure : boost::spirit::classic::closure<version_closure, ParsedVersionS>
{
	member1 val;
};

struct version_parser : public boost::spirit::classic::grammar<version_parser, version_closure::context_t>
{
	version_parser()
	{
	}

	template<typename scannerT>
	struct definition
	{
		definition(const version_parser& self)
		{
			using namespace boost::spirit::classic;
			using namespace phoenix;

#define ASSIGN(member, what) bind(&ParsedVersionS::member)(self.val) = what
#define ASSIGNSTR(member, begin, end) bind(&ParsedVersionS::member)(self.val) = construct_<std::_tstring>(begin, end)

			first =
				(
				def_string	= def_ide_str | def_oss_str | def_build_str, 
				def_ide_str = majorVersion_num >> delim >> minorVersion_num >> delim >> pointVersion_num >> delim >> build_num,
				def_oss_str = preffix_str >> delim >> majorVersion_num >> isOSS_bool >> minorVersion_num >> !(delim >> pointVersion_num) >> !(delim >> suffix_str) >> !(delim >> build_num),
				def_build_str = preffix_str >> '_' >> majorVersion_num >> !('_' >> minorVersion_num) >> !('_' >> pointVersion_num),

				delim =				ch_p('_') | '-' | '.',

				preffix_str	=		identifier1		[ASSIGNSTR(preffix, arg1, arg2)],
				majorVersion_num =	uint_p			[ASSIGN(majorVersion, arg1)],
				minorVersion_num =	uint_p			[ASSIGN(minorVersion, arg1)] >> !(alpha_p [ASSIGN(minorVersionAlpha, arg1)]),
				pointVersion_num =	uint_p			[ASSIGN(pointVersion, arg1)],
				suffix_str =		identifier1		[ASSIGNSTR(suffix, arg1, arg2)],
				build_num =			uint_p			[ASSIGN(build, arg1)] >> !(alpha_p [ASSIGN(buildAlpha, arg1)]),
				isOSS_bool =		ch_p('.')		[ASSIGN(ossFlag, true)],

				identifier1 = 
					nocase_d
					[
						lexeme_d
						[
							*(anychar_p - delim)
						]
					],

				identifier2 = 
					nocase_d
					[
						lexeme_d
						[
							*anychar_p
						]
					]
				);
#undef ASSIGN
		}
		boost::spirit::classic::subrule<0> def_string;
		boost::spirit::classic::subrule<1> def_ide_str;
		boost::spirit::classic::subrule<2> def_oss_str;
		boost::spirit::classic::subrule<3> def_build_str;
		boost::spirit::classic::subrule<4> preffix_str;
		boost::spirit::classic::subrule<5> majorVersion_num;
		boost::spirit::classic::subrule<6> minorVersion_num;
		boost::spirit::classic::subrule<7> pointVersion_num;
		boost::spirit::classic::subrule<8> suffix_str;
		boost::spirit::classic::subrule<9> build_num;
		boost::spirit::classic::subrule<10> delim;
		boost::spirit::classic::subrule<11> isOSS_bool;
		boost::spirit::classic::subrule<12> identifier1;
		boost::spirit::classic::subrule<13> identifier2;
		boost::spirit::classic::rule<scannerT> first;
		const boost::spirit::classic::rule<scannerT>& start() const
		{
			return first;
		}
	};
};

bool ParseVersion(const std::_tstring & version_string, ParsedVersion & result)
{
	using namespace std;
	using namespace boost::spirit::classic;
	using namespace phoenix;

	version_parser version_p;
	ParsedVersionS result1;
	if (boost::spirit::classic::parse(version_string.c_str(), version_p[phoenix::var(result1) = phoenix::arg1]).full)
	{
		result = result1;
		return true;
	}
	return false;
}
