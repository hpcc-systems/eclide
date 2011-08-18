#pragma once

#include "wlib.h"
#include "dali.h"
#include "repository.h"

const TCHAR * const DependancyErrorMsg = _T("Unable to calculate dependancy tree (check syntax).");
const TCHAR * const ComplexityErrorMsg = _T("Unable to calculate complexity (check syntax and/or ensure action is present).");

void WLIB_API DoShowDependantAttributes(IRepository *rep, Dali::IDali *dali, const std::_tstring & queue, const std::_tstring & cluster, const std::_tstring & ecl, const std::_tstring & module, const std::_tstring & attr, Dali::CEclExceptionVector & errorResults);
