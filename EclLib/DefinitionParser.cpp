#include "StdAfx.h"
#include "DefinitionParser.h"

using namespace BOOST_SPIRIT_CLASSIC_NS;

class definitionSxxx : public ParsedDefinition
{
public:
    definitionSxxx() 
	{ 
		row = 0;
		col = 0;
	}
};

std::ostream& operator<< (std::ostream& os, const ParsedDefinition& def)
{
	std::string module = CT2A(def.module.c_str(), CP_UTF8);
	std::string attribute = CT2A(def.attribute.c_str(), CP_UTF8);
	os << "definition =\n"
		<< "    {\n"
		<< "    folder    = " << module << "\n"
		<< "    file      = " << attribute << "\n"
		<< "    row       = " << def.row << "\n"
		<< "    col       = " << def.col << "\n"
		<< "    }\n";
	return os;
}

struct definition_closure : boost::spirit::classic::closure<definition_closure, ParsedDefinition>
{
	member1 val;
};

struct definition_parser : public boost::spirit::classic::grammar<definition_parser, definition_closure::context_t>
{
	definition_parser()
	{
	}

	template<typename scannerT>
	struct definition
	{
		definition(const definition_parser& self)
		{
			using namespace boost::spirit::classic;
			using namespace phoenix;

#define ASSIGN(member, what) bind(&ParsedDefinition::member)(self.val) = what
#define ASSIGNSTR(member, begin, end) bind(&ParsedDefinition::member)(self.val) = construct_<std::_tstring>(begin, end)

			first =
				(
				def_string	= ( builder_def				[ASSIGN(type, ParsedDefinition::BUILDER)]
							  | remote_def				[ASSIGN(type, ParsedDefinition::REMOTE)]
							  | local_def				[ASSIGN(type, ParsedDefinition::LOCAL)]
							  ), 

				builder_def	= confix_p('(', row_col, ')'),

				local_def	= folder >> file,

				folder		= (+(anychar_p - file))			[ASSIGNSTR(module, arg1, arg2)],

				file		= (+(anychar_p - (ch_p('\\') | '(')))	[ASSIGNSTR(attribute, arg1, arg2)]
							>> confix_p('(', row_col, ')'),

				remote_def	= mod_attr >> confix_p('(', row_col, ')'),

				mod_attr	= module >> '.' >> attribute,

				module		= identifier					[ASSIGNSTR(module, arg1, arg2)],

				attribute	= identifier					[ASSIGNSTR(attribute, arg1, arg2)],

				row_col		=  uint_p						[ASSIGN(row, arg1)]
							>> !(',' >> uint_p				[ASSIGN(col, arg1)]),

				identifier = 
					nocase_d
					[
						lexeme_d
						[
							(alpha_p | '_') >> *(alnum_p | '_')
						]
					]
				);
#undef ASSIGN
		}
		boost::spirit::classic::subrule<0> def_string;
		boost::spirit::classic::subrule<1> local_def;
		boost::spirit::classic::subrule<2> builder_def;
		boost::spirit::classic::subrule<3> remote_def;
		boost::spirit::classic::subrule<4> folder;
		boost::spirit::classic::subrule<5> file;
		boost::spirit::classic::subrule<6> mod_attr;
		boost::spirit::classic::subrule<7> module;
		boost::spirit::classic::subrule<8> attribute;
		boost::spirit::classic::subrule<9> row_col;
		boost::spirit::classic::subrule<10> identifier;
		boost::spirit::classic::rule<scannerT> first;
		const boost::spirit::classic::rule<scannerT>& start() const
		{
			return first;
		}
	};
};

bool DefinitionToLocation(const std::_tstring & def_string, ParsedDefinition & result)
{
	using namespace std;
	using namespace boost::spirit::classic;
	using namespace phoenix;

	definition_parser definition_p;
    ParsedDefinition result1;
	boost::spirit::classic::parse_info<std::_tstring::const_iterator> parse_result = boost::spirit::classic::parse(def_string.begin(), def_string.end(), definition_p[phoenix::var(result1) = phoenix::arg1], space_p);
	if (parse_result.full)
	{
		result = result1;
		return true;
	}
#ifdef _DEBUG
	else
	{
		std::_tstring parsed(def_string.begin(), parse_result.stop);
	}
#endif
	return false;
}

#ifdef _DEBUG
class _DefinitionToLocationTest 
{ 
public:
	_DefinitionToLocationTest() 
	{
		ParsedDefinition pd;
		ATLASSERT(DefinitionToLocation(_T("(1,2)"), pd));
		ATLASSERT(DefinitionToLocation(_T("c:\\temp\\test.ecl(2,3)"), pd));
		ATLASSERT(DefinitionToLocation(_T("c:\\temp\\test123.ecl(4,5)"), pd));
		ATLASSERT(DefinitionToLocation(_T("a.b(6,7)"), pd));
		//ATLASSERT(!DefinitionToLocation(_T("a.b.c(8,9)"), pd));
	}
} DefinitionToLocationTest;
#endif
