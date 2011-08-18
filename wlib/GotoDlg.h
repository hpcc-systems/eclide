#pragma once

#include "wlib.h"

WLIB_API INT_PTR DoModalGoto(int curr, int len, int &newLine);
WLIB_API INT_PTR DoModalSetTabSize(int curr, int max, int &tabSize);
WLIB_API INT_PTR DoModalGotoID(std::_tstring &id);
