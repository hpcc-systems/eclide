#pragma once

#include "clib.h"
#include "table.h"

namespace Dali
{
__interface IResult : public IUnknown
{
	const TCHAR *GetID() const;
	const TCHAR *GetName() const;
	int GetSequence() const;
	const TCHAR *GetValue() const;
	const TCHAR *GetLink() const;
	const TCHAR *GetFileName() const;
	__int64 GetTotal() const;
	const TCHAR *GetECLType(const std::_tstring & colID) const;
	unsigned GetResultData(__int64 start, int count, ITable * result) const;
	unsigned GetResultData(__int64 start, int count, ITable * result, __int64 & total) const;
};

typedef StlLinked<IResult> IResultAdapt;
typedef std::vector<IResultAdapt> IResultVector;
typedef std::map<std::_tstring, IResultAdapt> IResultMap;
}
