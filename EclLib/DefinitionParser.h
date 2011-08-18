#pragma once

#include "EclLib.h"

class ParsedDefinition
{
public:
	std::_tstring module;
	std::_tstring attribute;
	unsigned int row;
	unsigned int col;

	ParsedDefinition()
	{
		row = 0;
		col = 0;
	}

	ParsedDefinition(const ParsedDefinition & other)
	{
		module = other.module;
		attribute = other.attribute;
		row = other.row;
		col = other.col;
	}

	bool IsBuilder() const
	{
		return module.size() == 0 && attribute.size() == 0;
	}
};

ECLLIB_API bool DefinitionToLocation(const std::_tstring & definition, ParsedDefinition & result);
