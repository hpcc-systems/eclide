#pragma once

#include "RecursiveMutex.h"
#include "comms.h"

__interface IAttributeType;
__interface IAttribute;
__interface __declspec(uuid("5408B16C-8103-47aa-906B-347727AAED24")) IAttributeHistory : public clib::ILockableUnknown
{
	const TCHAR *GetID() const;
	const TCHAR *GetModuleQualifiedLabel() const;
	const TCHAR *GetLabel() const;
	const TCHAR *GetQualifiedLabel(bool excludeRoot = false) const;
	IAttributeType *GetType() const;
	const TCHAR *GetText(bool refresh = true, bool noBroadcast = false) const;
	const TCHAR *GetModifiedDate() const;
	const TCHAR *GetModifiedBy() const;
	const TCHAR *GetDescription() const;
	int GetVersion() const;
	IAttribute * GetAttribute() const;
	bool SetLabel(const std::_tstring & label);
};

typedef StlLinked<IAttributeHistory> IAttributeHistoryAdapt;
typedef std::vector<IAttributeHistoryAdapt> IAttributeHistoryVector;

class IAttributeHistoryCompare
{
public:
	bool operator ()(IAttributeHistoryAdapt & l, IAttributeHistoryAdapt & r)
	{
		int compare = _tcsicmp(l->GetModuleQualifiedLabel(), r->GetModuleQualifiedLabel());
		if (compare < 0)
			return true;
		else if (compare == 0)
		{
			compare = _tcsicmp(l->GetLabel(), r->GetLabel());
			if (compare < 0)
				return true;
			else if (compare == 0)
			{
				compare = _tcsicmp(l->GetModifiedDate(), r->GetModifiedDate());
				if (compare < 0)
					return true;
			}
		}
		return false;
	}
};
