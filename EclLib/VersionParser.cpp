#include "StdAfx.h"

#include "VersionParser.h"

using namespace BOOST_SPIRIT_CLASSIC_NS;

std::ostream& operator<< (std::ostream& os, const ParsedVersion& def)
{
	std::string preffix = CT2A(def.preffix.c_str(), CP_UTF8);
	std::string minorVersionAlpha = CT2A(def.minorVersionAlpha.c_str(), CP_UTF8);
	std::string suffixStr = CT2A(def.suffixStr.c_str(), CP_UTF8);
	std::string github = CT2A(def.github.c_str(), CP_UTF8);
	std::string buildAlpha = CT2A(def.buildAlpha.c_str(), CP_UTF8);
	os << "Version =\n"
		<< "    {\n"
		<< "    preffix = " << preffix << "\n"
		<< "    major version = " << def.majorVersion << "\n"
		<< "    minor version = " << def.minorVersion << "\n"
		<< "    minor version alpha = " << minorVersionAlpha << "\n"
		<< "    point version = " << def.pointVersion << "\n"
		<< "    suffixStr = " << suffixStr << "\n"
		<< "    suffixInt = " << def.suffixInt << "\n"
		<< "    github = " << github << "\n"
		<< "    build = " << def.build << "\n"
		<< "    build alpha = " << buildAlpha << "\n"
		<< "    }\n";
	return os;
}

struct version_closure : boost::spirit::classic::closure<version_closure, ParsedVersion>
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

#define ASSIGN(member, what) bind(&ParsedVersion::member)(self.val) = what
#define ASSIGNSTR(member, begin, end) bind(&ParsedVersion::member)(self.val) = construct_<std::_tstring>(begin, end)

			first =
				(
				def_string	= ( def_ide_str				[ASSIGN(type, ParsedVersion::ECLIDE)]
							  | def_eclcc_str			[ASSIGN(type, ParsedVersion::ECLCC)]
							  | def_oss_str				[ASSIGN(type, ParsedVersion::OSS)]
							  | def_build_str			[ASSIGN(type, ParsedVersion::BUILD)] 
							  ), 
				def_ide_str = majorVersion_num >> '.' >> minorVersion_num >> '.' >> pointVersion_num >> '.' >> build_num,
				def_eclcc_str = def_ecllang_str >> def_oss_str,
				def_oss_str = preffix_str >> delim >> majorVersion_num >> delim >> minorVersion_num >> delim >> pointVersion_num >> delim >> suffix_str >> !github_str,
				def_build_str = preffix_str >> '_' >> majorVersion_num >> !('_' >> minorVersion_num >> !('_' >> pointVersion_num)),

				delim =				ch_p('_') | '-' | '.',

				def_ecllang_str =   uint_p				[ASSIGN(lang_MajorVersion, arg1)]
									>> '.' >> uint_p	[ASSIGN(lang_MinorVersion, arg1)]
									>> '.' >> uint_p	[ASSIGN(lang_PointVersion, arg1)],

				preffix_str	=		(+alnum_p)			[ASSIGNSTR(preffix, arg1, arg2)],
				majorVersion_num =	uint_p				[ASSIGN(majorVersion, arg1)],
				minorVersion_num =	uint_p				[ASSIGN(minorVersion, arg1)]		>> !(alpha_p)	[ASSIGN(minorVersionAlpha, arg1)],
				pointVersion_num =	uint_p				[ASSIGN(pointVersion, arg1)],
				suffix_str =		(+alpha_p)			[ASSIGNSTR(suffixStr, arg1, arg2)]	>> uint_p		[ASSIGN(suffixInt, arg1)]
									| uint_p			[ASSIGN(suffixInt, arg1)]			>> (+alpha_p)	[ASSIGNSTR(suffixStr, arg1, arg2)]
									| uint_p			[ASSIGN(suffixInt, arg1)],
				github_str =		confix_p('[',
									(+anychar_p)		[ASSIGNSTR(github, arg1, arg2)], 
									']'),
				build_num =			uint_p				[ASSIGN(build, arg1)]				>> !(alpha_p	[ASSIGN(buildAlpha, arg1)])
				);
#undef ASSIGN
		}
		boost::spirit::classic::subrule<0> def_string;
		boost::spirit::classic::subrule<1> def_ide_str;
		boost::spirit::classic::subrule<2> def_oss_str;
		boost::spirit::classic::subrule<3> def_build_str;
		boost::spirit::classic::subrule<4> def_eclcc_str;
		boost::spirit::classic::subrule<5> def_ecllang_str;
		boost::spirit::classic::subrule<6> preffix_str;
		boost::spirit::classic::subrule<7> majorVersion_num;
		boost::spirit::classic::subrule<8> minorVersion_num;
		boost::spirit::classic::subrule<9> pointVersion_num;
		boost::spirit::classic::subrule<10> suffix_str;
		boost::spirit::classic::subrule<11> github_str;
		boost::spirit::classic::subrule<12> build_num;
		boost::spirit::classic::subrule<13> delim;
		boost::spirit::classic::rule<scannerT> first;
		const boost::spirit::classic::rule<scannerT>& start() const
		{
			return first;
		}
	};
};

bool ParseVersion(const std::_tstring & version_string, ParsedVersion & result)
{
	std::_tstring trim_version_string = version_string;
	boost::algorithm::trim(trim_version_string);
	using namespace std;
	using namespace boost::spirit::classic;
	using namespace phoenix;

	version_parser version_p;
	ParsedVersion result1;
	boost::spirit::classic::parse_info<std::_tstring::const_iterator> parse_result = boost::spirit::classic::parse(trim_version_string.begin(), trim_version_string.end(), version_p[phoenix::var(result1) = phoenix::arg1], space_p);
	if (parse_result.full)
	{
		result = result1;
		return true;
	}
#ifdef _DEBUG
	else
	{
		std::_tstring parsed(trim_version_string.begin(), parse_result.stop);
	}
#endif
	return false;
}
