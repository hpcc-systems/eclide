#include "StdAfx.h"

#include "EclErrorParser.h"

using namespace BOOST_SPIRIT_CLASSIC_NS;

struct EclErrorS : public ParsedEclError
{
	EclErrorS() 
	{ 
		row = 0;
		col = 0;
	}
};

std::ostream& operator<< (std::ostream& os, const EclErrorS& def)
{
	std::string location = CT2A(def.location.c_str(), CP_UTF8);
	std::string message = CT2A(def.message.c_str(), CP_UTF8);
	std::string other = CT2A(def.other.c_str(), CP_UTF8);
	os << "EclError =\n"
		<< "    {\n"
		<< "    location = " << location << "\n"
		<< "    row       = " << def.row << "\n"
		<< "    col       = " << def.col << "\n"
		<< "    code       = " << def.code << "\n"
		<< "    message    = " << message << "\n"
		<< "    other    = " << other << "\n"
		<< "    }\n";
	return os;
}

struct ecl_closure : boost::spirit::classic::closure<ecl_closure, EclErrorS>
{
	member1 val;
};

struct ecl_parser : public boost::spirit::classic::grammar<ecl_parser, ecl_closure::context_t>
{
	ecl_parser()
	{
	}

	template<typename scannerT>
	struct definition
	{
		definition(const ecl_parser& self)
		{
			using namespace boost::spirit::classic;
			using namespace phoenix;

#define ASSIGN(member, what) bind(&EclErrorS::member)(self.val) = what
#define ASSIGNSTR(member, begin, end) bind(&EclErrorS::member)(self.val) = construct_<std::_tstring>(begin, end)

			first =
				(
				def_string	=	( def_string_ecl		[ASSIGN(rule, ParsedEclError::ECL)]
								| def_string_salt		[ASSIGN(rule, ParsedEclError::SALT)]
								| def_string_kel		[ASSIGN(rule, ParsedEclError::KEL)]
								| def_string_other		[ASSIGN(rule, ParsedEclError::OTHER)] 
								), 
				def_string		= def_string_kel | def_string_ecl | def_string_salt | def_string_other,

				def_string_ecl	= location_str >> !confix_p('(', row_col, ')') >> ':' >> type_str >> alpha_p >> code_num >> ':' >> message_str,

				def_string_salt	= confix_p('(', row_num, ')') >> ':' >> identifier_anychar_notcolon [ASSIGNSTR(message, arg1, arg2)] >> ':' >> other_str,

				def_string_kel	= (!(alpha_p >> ':') >> identifier_anychar_notcolon) [ASSIGNSTR(location, arg1, arg2)] >> ':' >> row_col >> ':' >> type_str >> alpha_p >> code_num >> '-' >> message_str,

				def_string_other= type_str >> ":" >> message_str [ASSIGN(location, _T(""))],

				location_str = identifier_anychar_notparen [ASSIGNSTR(location, arg1, arg2)],

				row_col		= row_num >> ',' >> col_num,

				row_num		= uint_p [ASSIGN(row, arg1)],

				col_num		= uint_p [ASSIGN(col, arg1)],

				type_str	= identifier_alnum [ASSIGNSTR(type, arg1, arg2)],

				code_num	= uint_p [ASSIGN(code, arg1)],

				message_str	= identifier_anychar [ASSIGNSTR(message, arg1, arg2)],

				other_str	= identifier_anychar [ASSIGNSTR(other, arg1, arg2)],

				identifier_anychar = 
					nocase_d
					[
						lexeme_d
						[
							*anychar_p
						]
					],

				identifier_anychar_notparen = 
					nocase_d
					[
						lexeme_d
						[
							*(anychar_p - '(')
						]
					],

				identifier_anychar_notcolon = 
					nocase_d
					[
						lexeme_d
						[
							*(anychar_p - ':')
						]
					],

				identifier_alnum = 
					nocase_d
					[
						lexeme_d
						[
							*alnum_p
						]
					]

				);
#undef ASSIGN
		}
		boost::spirit::classic::subrule<0> def_string;
		boost::spirit::classic::subrule<1> def_string_ecl;
		boost::spirit::classic::subrule<2> def_string_other;
		boost::spirit::classic::subrule<3> def_string_salt;
		boost::spirit::classic::subrule<4> def_string_kel;
		boost::spirit::classic::subrule<5> location_str;
		boost::spirit::classic::subrule<6> row_col;
		boost::spirit::classic::subrule<7> row_num;
		boost::spirit::classic::subrule<8> col_num;
		boost::spirit::classic::subrule<9> message_str;
		boost::spirit::classic::subrule<10> other_str;
		boost::spirit::classic::subrule<11> type_str;
		boost::spirit::classic::subrule<12> code_num;
		boost::spirit::classic::subrule<13> identifier_anychar;
		boost::spirit::classic::subrule<14> identifier_alnum;
		boost::spirit::classic::subrule<15> identifier_anychar_notparen;
		boost::spirit::classic::subrule<16> identifier_anychar_notcolon;
		boost::spirit::classic::rule<scannerT> first;
		const boost::spirit::classic::rule<scannerT>& start() const
		{
			return first;
		}
	};
};

bool ParseEclError(const std::_tstring & def_string, ParsedEclError & result)
{
	if (def_string.empty()) {
		return false;
	}
	using namespace std;
	using namespace boost::spirit::classic;
	using namespace phoenix;

	ecl_parser ecl_p;
	EclErrorS result1;
	if (boost::spirit::classic::parse(def_string.c_str(), ecl_p[phoenix::var(result1) = phoenix::arg1], space_p).full)
	{
		result = result1;
		return true;
	}
	return false;
}
