#pragma once

#include "EclLib.h"

struct ParsedSaltError
{
	unsigned int row;
	std::_tstring message;
	std::_tstring other;

	ParsedSaltError()
	{
		row = 0;
	}
};

ECLLIB_API bool ParseSaltError(const std::_tstring & definition, ParsedSaltError & result);
