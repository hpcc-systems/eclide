#pragma once

#include "Attribute.h"

__interface __declspec(uuid("4B174C5F-DEEA-4702-BA66-5BCEFAC33012")) IDiskAttribute : public IAttribute
{
	const TCHAR * GetPath();
};
