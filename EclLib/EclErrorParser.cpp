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
	os << "EclError =\n"
		<< "    {\n"
		<< "    other = " << location << "\n"
		<< "    row       = " << def.row << "\n"
		<< "    col       = " << def.col << "\n"
		<< "    code       = " << def.code << "\n"
		<< "    message    = " << message << "\n"
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
				def_string	= location_str >> confix_p('(', row_col, ')') >> ':' >> type_str >> alpha_p >> code_num >> ":" >> message_str,

				location_str = identifier2 [ASSIGNSTR(location, arg1, arg2)],

				row_col		= row_num >> ',' >> col_num,

				row_num		= uint_p [ASSIGN(row, arg1)],

				col_num		= uint_p [ASSIGN(col, arg1)],

				type_str	= identifier3 [ASSIGNSTR(type, arg1, arg2)],

				code_num	= uint_p [ASSIGN(code, arg1)],

				message_str	= identifier1 [ASSIGNSTR(message, arg1, arg2)],

				identifier1 = 
					nocase_d
					[
						lexeme_d
						[
							*anychar_p
						]
					],

				identifier2 = 
					nocase_d
					[
						lexeme_d
						[
							*(anychar_p - '(')
						]
					],

				identifier3 = 
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
		boost::spirit::classic::subrule<1> location_str;
		boost::spirit::classic::subrule<2> row_col;
		boost::spirit::classic::subrule<3> row_num;
		boost::spirit::classic::subrule<4> col_num;
		boost::spirit::classic::subrule<5> message_str;
		boost::spirit::classic::subrule<6> type_str;
		boost::spirit::classic::subrule<7> code_num;
		boost::spirit::classic::subrule<8> identifier1;
		boost::spirit::classic::subrule<9> identifier2;
		boost::spirit::classic::subrule<10> identifier3;
		boost::spirit::classic::rule<scannerT> first;
		const boost::spirit::classic::rule<scannerT>& start() const
		{
			return first;
		}
	};
};

bool ParseEclError(const std::_tstring & def_string, ParsedEclError & result)
{
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
