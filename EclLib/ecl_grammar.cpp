#include "stdafx.h"
#include "ecl_grammar.h"

void DoParse(const std::string & ecl, tree_parse_info<> &info)
{
    ecl_grammar g;
#ifdef _DEBUG
    BOOST_SPIRIT_DEBUG_NODE(g);
#endif
    info = pt_parse(ecl.c_str(), g, space_p);
}
