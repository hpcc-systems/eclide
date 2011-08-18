#pragma once

#include "FindReplace.h"

FindReplace& GetFindReplace();
void FindReplaceOnEditFind(const std::_tstring &findWhat);
void FindReplaceOnEditReplace(const std::_tstring &replaceWhat);
