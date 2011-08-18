#include "StdAfx.h"

#include "SaltErrorParser.h"

using namespace BOOST_SPIRIT_CLASSIC_NS;

struct SaltErrorS : public ParsedSaltError
{
	SaltErrorS() 
	{ 
		row = 0;
	}
};

std::ostream& operator<< (std::ostream& os, const SaltErrorS& def)
{
	std::string message = CT2A(def.message.c_str(), CP_UTF8);
	std::string other = CT2A(def.other.c_str(), CP_UTF8);
	os << "SaltError =\n"
		<< "    {\n"
		<< "    row       = " << def.row << "\n"
		<< "    message    = " << message << "\n"
		<< "    other = " << other << "\n"
		<< "    }\n";
	return os;
}

struct salt_closure : boost::spirit::classic::closure<salt_closure, SaltErrorS>
{
	member1 val;
};

struct salt_parser : public boost::spirit::classic::grammar<salt_parser, salt_closure::context_t>
{
	salt_parser()
	{
	}

	template<typename scannerT>
	struct definition
	{
		definition(const salt_parser& self)
		{
			using namespace boost::spirit::classic;
			using namespace phoenix;

#define ASSIGN(member, what) bind(&SaltErrorS::member)(self.val) = what
#define ASSIGNSTR(member, begin, end) bind(&SaltErrorS::member)(self.val) = construct_<std::_tstring>(begin, end)

			first =
				(
				def_string	= confix_p('(', row_num, ')') >> ':' >> message_str >> ':' >> other_str,

				row_num		= uint_p [ASSIGN(row, arg1)],

				message_str	= identifier1 [ASSIGNSTR(message, arg1, arg2)],

				other_str	= identifier2 [ASSIGNSTR(other, arg1, arg2)],

				identifier1 = 
					nocase_d
					[
						lexeme_d
						[
							*(anychar_p - ':')
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
		boost::spirit::classic::subrule<1> row_num;
		boost::spirit::classic::subrule<2> message_str;
		boost::spirit::classic::subrule<3> other_str;
		boost::spirit::classic::subrule<4> identifier1;
		boost::spirit::classic::subrule<5> identifier2;
		boost::spirit::classic::rule<scannerT> first;
		const boost::spirit::classic::rule<scannerT>& start() const
		{
			return first;
		}
	};
};

bool ParseSaltError(const std::_tstring & def_string, ParsedSaltError & result)
{
	using namespace std;
	using namespace boost::spirit::classic;
	using namespace phoenix;

	salt_parser salt_p;
	SaltErrorS result1;
	if (boost::spirit::classic::parse(def_string.c_str(), salt_p[phoenix::var(result1) = phoenix::arg1]).full)
	{
		result = result1;
		return true;
	}
	return false;
}
