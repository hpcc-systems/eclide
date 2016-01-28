#pragma once

#include "EclLib.h"

class ParsedDefinition
{
public:
    enum TYPE
    {
         UNKNOWN,
         BUILDER,
         LOCAL,
         REMOTE,
         LAST
    } m_type;
    std::_tstring module;
    std::_tstring attribute;
    unsigned int row;
    unsigned int col;

    ParsedDefinition()
    {
        m_type = UNKNOWN;
        row = 0;
        col = 0;
    }

    ParsedDefinition(const ParsedDefinition & other)
    {
        m_type = other.m_type;
        module = other.module;
        attribute = other.attribute;
        row = other.row;
        col = other.col;
    }

    TYPE GetType() const
    {
        return m_type;
    }
};

ECLLIB_API bool DefinitionToLocation(const std::_tstring & definition, ParsedDefinition & result);
