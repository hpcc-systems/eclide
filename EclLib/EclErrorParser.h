#pragma once

#include "EclLib.h"

struct ParsedEclError
{
	std::_tstring location;
	unsigned int row;
	unsigned int col;
	std::_tstring type;
	unsigned int code;
	std::_tstring message;

	ParsedEclError()
	{
		row = 0;
		col = 0;
		code = 0;
	}
};

ECLLIB_API bool ParseEclError(const std::_tstring & definition, ParsedEclError & result);
