#pragma once

#include "RecursiveMutex.h"
#include "comms.h"
#include "attributehistory.h"
#include "AttributeType.h"
#include "Workunit.h"

enum ATTRSTATE
{
	ATTRSTATE_UNKNOWN = 0,
	ATTRSTATE_LOCKED,
	ATTRSTATE_CHECKEDOUT,
	ATTRSTATE_FLAGGED,
	ATTRSTATE_EDITED,
	ATTRSTATE_NONE,
	ATTRSTATE_LOCKED2,
	ATTRSTATE_LOCKED3,
	ATTRSTATE_CHECKEDOUT_SANDBOXED,
	ATTRSTATE_SANDBOXED,
	ATTRSTATE_CHECKEDOUT_OTHER5,
	ATTRSTATE_ORPHANED,
	ATTRSTATE_PLACEHOLDER,
	ATTRSTATE_LAST,
};

const TCHAR * const ATTRSTATELABEL[] = 
{
	_T("Unknown"),
	_T("Locked"),
	_T("Checkedout"),
	_T("Flagged"),
	_T("Edited"),
	_T("None"),
	_T("Locked"),
	_T("Locked"),
	_T("Checkedout and Sandboxed"),
	_T("Sandboxed"),
	_T("Checkedout by Other"),
	_T("Orphaned"),
	_T("Placeholder"),
	_T("Last")
};

enum SecAccessFlags
{
	SecAccess_None = 0,
	SecAccess_Access = 1,
	SecAccess_Read = 3,
	SecAccess_Write = 7,
	SecAccess_Full = 255
};

__interface IAttribute;
typedef StlLinked<IAttribute> IAttributeAdapt;
typedef std::vector<IAttributeAdapt> IAttributeVector;

__interface IMigrationCallback;

typedef boost::signal<void(IAttribute *, bool, IAttribute *, bool deleted)> refresh_signal_type;
typedef refresh_signal_type::slot_type refresh_slot_type;

__interface IRepository;
__interface IModule;

__interface __declspec(uuid("1D743D5B-2719-4b7d-A5EB-4D5ACF34A493")) IAttribute : public clib::ILockableUnknown
{
	const TCHAR *GetURL() const;
	const TCHAR *GetID() const;
	IModule *GetModule() const;
	const TCHAR *GetModuleLabel() const;
	const TCHAR *GetLabel() const;
	const TCHAR *GetQualifiedLabel(bool excludeRoot = false) const;
	const SecAccessFlags GetAccess() const;
	IAttributeType * GetType() const;
	const TCHAR *GetText(bool refresh = true, bool noBroadcast = false) const;
	const TCHAR *GetChecksum() const;
	const TCHAR *GetChecksumLocal() const;
	const TCHAR *GetChecksumLocalTidied() const;
	int GetVersion() const;
	bool SetText(const TCHAR* ecl, bool noBroadcast = false);
	bool IsCheckedOut() const;
	bool IsSandboxed() const;
	bool IsLocked() const;
	const TCHAR *GetLockedBy() const;
	const TCHAR * GetModifiedDate() const;
	const TCHAR *GetModifiedBy() const;
	ATTRSTATE GetState() const;
	const TCHAR *GetStateLabel() const;
	unsigned GetHistory(IAttributeHistoryVector & attributes) const;
	bool Checkout();
	bool Checkin(const TCHAR* comments);
	IAttribute * Rename(const TCHAR* label);
	bool Delete();
	bool Exists() const;
	bool Create();
	int PreProcess(PREPROCESS_TYPE action, const TCHAR * overrideEcl, IAttributeVector & attrs, Dali::CEclExceptionVector & errs) const;

	boost::signals::connection on_refresh_connect(const refresh_slot_type& s);
	void Refresh(bool eclChanged = false, IAttribute * newAttrAsOldOneMoved = NULL, bool deleted = false);
	IRepository* GetRepository();
	IAttributeHistory * GetAsHistory();
};

typedef std::set<IAttributeAdapt> IAttributeSet;
typedef CUnknownT<IAttributeVector> IAttributeVectorPtr;
typedef CComPtr<IAttributeVectorPtr> IAttributeVectorAdapt;

COMMS_API const TCHAR *GetAttrStateLabel(int nIndex);

class IAttributeCompare
{
public:
	bool operator ()(IAttributeAdapt & l, IAttributeAdapt & r)
	{
		int compare = _tcsicmp(l->GetModuleLabel(), r->GetModuleLabel());
		if (compare < 0)
			return true;
		else if (compare == 0)
		{
			compare = _tcsicmp(l->GetLabel(), r->GetLabel());
			if (compare < 0)
				return true;
		}
		return false;
	}
};
